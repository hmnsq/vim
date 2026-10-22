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

#شامل "vim.h"

#اگر defined(FEAT_PROP_POPUP)

قسمتعریف ساخت {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_اندراج_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
قسمتعریف ساخت {
    صحیح		فعال;
    صحیح		winrow;
    صحیح		wincol;
    صحیح		اونچائی;
    صحیح		چوڑائی;
    صحیح		leftoff;
    صحیح		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
قسمتعریف ساخت {
    schar_T	*سطریں;
    صحیح		*attrs;
    u8char_T	*linesuc;
    صحیح		آغاز_صف;
    صحیح		آغاز_ستون;
    صحیح		صفیں_2;
    صحیح		ستون_تعداد;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
قسمتعریف ساخت {
    linenr_T	firstline;
    صحیح		blend;
    صحیح		flags;
    صحیح		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
قسمتعریف ساخت {
    صحیح		winrow;
    صحیح		wincol;
    صحیح		چوڑائی;
    صحیح		اونچائی;
    صحیح		leftcol;
    صحیح		leftoff;
    صحیح		has_scrollbar;
    صحیح		topoff;
    صحیح		bottomoff;
    صحیح		leftclip;
    صحیح		rightclip;
} popup_layout_T;

ساکن poppos_اندراج_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
// Window used for ":echowindow"
ساکن win_T *پیغام_win_2 = تہی;

// Time used for the next ":echowindow" message in msec.
ساکن صحیح  پیغام_win_وقت = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ساکن صحیح    آغاز_پیغام_win_timer = FALSE;

ساکن خالی may_آغاز_پیغام_win_timer(win_T *wp);
#اگر_ختم

ساکن صحیح popup_on_cmdline = FALSE;

ساکن خالی popup_adjust_position(win_T *wp);
ساکن bool popup_area_بدلا(win_T *wp, popup_area_T *area);
ساکن خالی popup_redraw_exposed_area(popup_area_T *area);
ساکن خالی popup_save_area(win_T *wp, popup_area_T *area);
ساکن خالی popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ساکن خالی popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ساکن bool popup_layout_بدلا(win_T *wp, popup_layout_T *layout);
ساکن bool popup_style_بدلا(win_T *wp, popup_style_snapshot_T *style);
ساکن خالی popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ساکن خالی popup_save_layout(win_T *wp, popup_layout_T *layout);
ساکن خالی redraw_under_popup_area(صحیح winrow, صحیح wincol, صحیح اونچائی,
	صحیح چوڑائی, صحیح leftoff);
ساکن خالی redraw_overlapped_opacity_popups(صحیح winrow, صحیح wincol,
	صحیح اونچائی, صحیح چوڑائی, صحیح leftoff, صحیح zindex);
ساکن خالی redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ساکن صحیح
popup_options_one(dict_T *dict, char_u *کلید)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    صحیح		n = 0;

    di = dict_ڈھونڈو(dict, کلید, -1);
    اگر (di == تہی)
	واپس MAXCOL;

    val = tv_get_string(&di->di_tv);
    اگر (STRNCMP(val, "cursor", 6) != 0)
	واپس dict_get_نمبر_check(dict, کلید);

    setcursor_mayforce(TRUE);
    s = val + 6;
    اگر (*s != NUL)
    {
	endp = s;
	اگر (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((حرف *)s, (حرف **)&endp, 10);
	اگر (endp != تہی && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_عبارت_str), val);
	    واپس 0;
	}
    }

    اگر (STRCMP(کلید, "line") == 0)
	n = screen_screenrow() + 1 + n;
    ورنہ // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    اگر (n == 0)
	n = -1;
    واپس n;
}

    ساکن صحیح
سیٹ_padding_border(dict_T *dict, صحیح *array, حرف *name, صحیح max_val)
{
    dictitem_T	*di;

    di = dict_ڈھونڈو(dict, (char_u *)name, -1);
    اگر (di == تہی)
	واپس OK;

    اگر (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	واپس FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    صحیح		i;
    صحیح		nr;

    برائے (i = 0; i < 4; ++i)
	array[i] = 1;
    اگر (list == تہی)
	واپس OK;

    CHECK_LIST_MATERIALIZE(list);
    برائے (i = 0, li = list->lv_پہلا; i < 4 && i < list->lv_len;
	    ++i, li = li->li_اگلا)
    {
	nr = (صحیح)tv_get_نمبر(&li->li_tv);
	اگر (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    واپس OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ساکن خالی
سیٹ_moved_اقدار(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.ستون;
    wp->w_popup_maxcol = curwin->w_cursor.ستون;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساکن خالی
سیٹ_moved_columns(win_T *wp, صحیح flags)
{
    char_u	*ptr;
    صحیح		len = ڈھونڈو_ident_under_cursor(&ptr, flags | ڈھونڈو_NOERROR);
    صحیح		mincol;

    اگر (len <= 0)
	واپس;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (صحیح)(ptr - ml_get_curline());
    اگر (curwin->w_cursor.ستون < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.ستون;
	wp->w_popup_maxcol = curwin->w_cursor.ستون;
	واپس;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ساکن خالی
سیٹ_mousemoved_اقدار(win_T *wp)
{
    wp->w_popup_mouse_صف = mouse_صف;
    wp->w_popup_mouse_mincol = mouse_ستون;
    wp->w_popup_mouse_maxcol = mouse_ستون;
}

    ساکن خالی
update_popup_استعمال_کرتا_mouse_move(خالی)
{
    popup_استعمال_کرتا_mouse_move = FALSE;
    اگر (!popup_visible)
	واپس;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_popup_mouse_صف != 0)
	{
	    popup_استعمال_کرتا_mouse_move = TRUE;
	    واپس;
	}
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_popup_mouse_صف != 0)
	{
	    popup_استعمال_کرتا_mouse_move = TRUE;
	    واپس;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساکن خالی
سیٹ_mousemoved_columns(win_T *wp, صحیح flags)
{
    win_T	*textwp;
    char_u	*متن;
    صحیح		ستون;
    pos_T	pos;
    colnr_T	mcol;

    اگر (ڈھونڈو_لفظ_under_cursor(mouse_صف, mouse_ستون, TRUE, flags,
				  &textwp, &pos.lnum, &متن, تہی, &ستون) != OK)
	واپس;

    // convert text column to mouse column
    pos.ستون = ستون;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, تہی, تہی, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.ستون = ستون + (colnr_T)STRLEN(متن) - 1;
    getvcol(textwp, &pos, تہی, تہی, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(متن);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    صحیح
popup_on_border(win_T *wp, صحیح صف, صحیح ستون)
{
    واپس (صف == 0 && wp->w_popup_border[0] > 0)
	    || (صف == popup_اونچائی(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (ستون == 0 && wp->w_popup_border[3] > 0)
	    || (ستون == popup_چوڑائی(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    صحیح
popup_close_if_on_X(win_T *wp, صحیح صف, صحیح ستون)
{
    اگر (wp->w_popup_close == POPCLOSE_BUTTON
	    && صف == 0 && ستون == popup_چوڑائی(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	واپس TRUE;
    }
    واپس FALSE;
}

// Values set when dragging a popup window starts.
ساکن صحیح drag_آغاز_صف;
ساکن صحیح drag_آغاز_ستون;
ساکن صحیح drag_آغاز_wantline;
ساکن صحیح drag_آغاز_wantcol;
ساکن صحیح drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    خالی
popup_آغاز_drag(win_T *wp, صحیح صف, صحیح ستون)
{
    drag_آغاز_صف = mouse_صف;
    drag_آغاز_ستون = mouse_ستون;
    اگر (wp->w_wantline <= 0)
	drag_آغاز_wantline = wp->w_winrow + 1;
    ورنہ
	drag_آغاز_wantline = wp->w_wantline;
    اگر (wp->w_wantcol == 0)
	drag_آغاز_wantcol = wp->w_wincol + 1;
    ورنہ
	drag_آغاز_wantcol = wp->w_wantcol;

    // Stop centering the popup
    اگر (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && صف == popup_اونچائی(wp) - 1
			    && ستون == popup_چوڑائی(wp) - 1;

    اگر (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	اگر (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	اگر (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    خالی
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    اگر (!win_valid_popup(wp))
	واپس;

    اگر ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	صحیح چوڑائی_inc = mouse_ستون - drag_آغاز_ستون;
	صحیح اونچائی_inc = mouse_صف - drag_آغاز_صف;

	اگر (چوڑائی_inc != 0)
	{
	    صحیح چوڑائی = wp->w_چوڑائی + چوڑائی_inc;

	    اگر (چوڑائی < 1)
		چوڑائی = 1;
	    wp->w_minwidth = چوڑائی;
	    wp->w_maxwidth = چوڑائی;
	    drag_آغاز_ستون = mouse_ستون;
	}

	اگر (اونچائی_inc != 0)
	{
	    صحیح اونچائی = wp->w_اونچائی + اونچائی_inc;

	    اگر (اونچائی < 1)
		اونچائی = 1;
	    wp->w_minheight = اونچائی;
	    wp->w_maxheight = اونچائی;
	    drag_آغاز_صف = mouse_صف;
	}

	popup_adjust_position(wp);
	واپس;
    }

    اگر (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	واپس;
    wp->w_wantline = drag_آغاز_wantline + (mouse_صف - drag_آغاز_صف);
    اگر (wp->w_wantline < 1)
	wp->w_wantline = 1;
    اگر (wp->w_wantline > صفیں)
	wp->w_wantline = صفیں;
    wp->w_wantcol = drag_آغاز_wantcol + (mouse_ستون - drag_آغاز_ستون);
    اگر (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    اگر (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    خالی
popup_سیٹ_firstline(win_T *wp)
{
    صحیح	    اونچائی = wp->w_اونچائی;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    جب (wp->w_firstline > 1 && wp->w_اونچائی < اونچائی)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    صحیح
popup_is_اندر_scrollbar(win_T *wp, صحیح صف, صحیح ستون)
{
    واپس wp->w_has_scrollbar
	&& صف >= wp->w_popup_border[0]
	&& صف < popup_اونچائی(wp) - wp->w_popup_border[2]
	&& ستون == popup_چوڑائی(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    خالی
popup_handle_scrollbar_click(win_T *wp, صحیح صف, صحیح ستون)
{
    اگر (!popup_is_اندر_scrollbar(wp, صف, ستون))
	واپس;

    صحیح	    اونچائی = popup_اونچائی(wp);
    صحیح	    new_topline = wp->w_topline;

    اگر (صف >= اونچائی / 2)
    {
	// Click in lower half, scroll down.
	اگر (wp->w_topline < wp->w_بفر->b_ml.ml_سطر_گنتی)
	    ++new_topline;
    }
    ورنہ اگر (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    اگر (new_topline == wp->w_topline)
	واپس;

    سیٹ_topline(wp, new_topline);
    اگر (wp == curwin)
    {
	اگر (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	ورنہ اگر (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_سیٹ_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#اگر defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ساکن خالی
popup_جوڑو_timeout(win_T *wp, صحیح وقت, صحیح close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((حرف *)cbbuf, حجم(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    اگر (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	واپس;

    wp->w_popup_timer = create_timer(وقت, 0);
    callback_T cb = get_callback(&tv);
    اگر (cb.cb_name != تہی && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#اگر_ختم

    ساکن poppos_T
get_pos_اندراج(dict_T *d, صحیح give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    صحیح	    nr;

    اگر (str == تہی)
	واپس POPPOS_NONE;

    برائے (nr = 0; nr < (صحیح)ARRAY_لمبائی(poppos_entries); ++nr)
	اگر (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    واپس poppos_entries[nr].pp_val;

    اگر (give_error)
	semsg(_(e_invalid_دلیل_str), str);
    واپس POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ساکن خالی
apply_move_options(win_T *wp, dict_T *d)
{
    صحیح		nr;
    char_u	*str;
    dictitem_T	*di;

    اگر ((nr = dict_get_نمبر_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    اگر ((nr = dict_get_نمبر_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    اگر ((nr = dict_get_نمبر_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    اگر ((nr = dict_get_نمبر_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    اگر (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    اگر (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    اگر (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_اندراج(d, TRUE);

	اگر (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    اگر (str != تہی)
    {
	wp->w_popup_prop_type = 0;
	اگر (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_ڈھونڈو(d, (char_u *)"textpropwin", -1);
	    اگر (di != تہی)
	    {
		wp->w_popup_prop_win = ڈھونڈو_win_by_nr_or_id(&di->di_tv);
		اگر (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = ڈھونڈو_prop_type_id(str, wp->w_popup_prop_win->w_بفر);
	    اگر (nr <= 0)
		nr = ڈھونڈو_prop_type_id(str, تہی);
	    اگر (nr <= 0)
		semsg(_(e_invalid_دلیل_str), str);
	    ورنہ
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_ڈھونڈو(d, (char_u *)"textpropid", -1);
    اگر (di != تہی)
	wp->w_popup_prop_id = dict_get_نمبر(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ساکن خالی
handle_moved_دلیل(win_T *wp, dictitem_T *di, صحیح mousemoved)
{
    اگر (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != تہی)
    {
	char_u  *s = di->di_tv.vval.v_string;
	صحیح	flags = 0;

	اگر (STRCMP(s, "word") == 0)
	    flags = ڈھونڈو_IDENT | ڈھونڈو_STRING;
	ورنہ اگر (STRCMP(s, "WORD") == 0)
	    flags = ڈھونڈو_STRING;
	ورنہ اگر (STRCMP(s, "expr") == 0)
	    flags = ڈھونڈو_IDENT | ڈھونڈو_STRING | ڈھونڈو_EVAL;
	ورنہ اگر (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_دلیل_str), s);
	اگر (flags != 0)
	{
	    اگر (mousemoved)
		سیٹ_mousemoved_columns(wp, flags);
	    ورنہ
		سیٹ_moved_columns(wp, flags);
	}
    }
    ورنہ اگر (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != تہی
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	صحیح	    mincol;
	صحیح	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_پہلا;
	اگر (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_نمبر(&l->lv_پہلا->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    اگر (mousemoved)
		wp->w_popup_mouse_صف = nr;
	    ورنہ
		wp->w_popup_lnum = nr;
	    li = li->li_اگلا;
	    اگر (nr == 0)
		wp->w_popup_curwin = تہی;
	}

	mincol = tv_get_نمبر(&li->li_tv);
	maxcol = tv_get_نمبر(&li->li_اگلا->li_tv);
	اگر (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	ورنہ
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    ورنہ
	semsg(_(e_invalid_دلیل_str), tv_get_string(&di->di_tv));
}

    ساکن خالی
check_highlight(dict_T *dict, حرف *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_ڈھونڈو(dict, (char_u *)name, -1);
    اگر (di == تہی)
	واپس;

    اگر (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_قدر_for_دلیل_str), name);
    ورنہ
    {
	str = tv_get_string(&di->di_tv);
	اگر (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ساکن خالی
popup_show_curline(win_T *wp)
{
    اگر (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    ورنہ اگر (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_اونچائی + 1;
	اگر (wp->w_topline < 1)
	    wp->w_topline = 1;
	ورنہ اگر (wp->w_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_topline = wp->w_بفر->b_ml.ml_سطر_گنتی;
	جب (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_بفر->b_ml.ml_سطر_گنتی
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_اونچائی + 1) > wp->w_اونچائی)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    اگر (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ساکن char_u *
popup_get_sign_name(win_T *wp)
{
    ساکن حرف    buf[30];

    vim_snprintf(buf, حجم(buf), "popup-%d", wp->w_id);
    واپس (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ساکن خالی
popup_highlight_curline(win_T *wp)
{
    صحیح	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_بفر, (char_u *)"PopUpMenu");

    اگر ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	اگر (!sign_exists_by_name(sign_name))
	{
	    حرف *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, تہی, (char_u *)linehl, تہی, تہی, تہی,
		    تہی, SIGN_DEF_PRIO);
	}

	sign_رکھو(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_بفر, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ورنہ
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_آخری_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ساکن صحیح
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    صحیح		nr;
    char_u	*str;

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#اگر_ختم

    // TODO: flip

    di = dict_ڈھونڈو(dict, (char_u *)"firstline", -1);
    اگر (di != تہی)
    {
	wp->w_firstline = dict_get_نمبر(dict, "firstline");
	اگر (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    اگر (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    اگر (str != تہی)
    {
	char_u	*title = vim_strsave(str);

	اگر (title != تہی)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    اگر (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	ورنہ
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	ورنہ
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	ورنہ
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	ورنہ
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	ورنہ
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_ڈھونڈو(dict, (char_u *)"opacity", -1);
    اگر (di != تہی)
    {
	nr = dict_get_نمبر(dict, "opacity");
	اگر (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	ورنہ اگر (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	ورنہ اگر (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	ورنہ
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    اگر (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    ورنہ
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_ڈھونڈو(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    اگر (di != تہی && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != تہی)
    {
	image_T		    *img;
	image_T		    *cur_img = تہی;
	image_placement_T   *رکھو;
	صحیح		    cw, ch;

	اگر (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    اگر (wp->w_popup_imagep != تہی)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = تہی;
	    }
	    جاؤ image_مکمل;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	اگر (wp->w_popup_imagep != تہی && wp->w_popup_imagep->img != تہی)
	    cur_img = wp->w_popup_imagep->img;

	img = جوڑو_image(di->di_tv.vval.v_dict, cur_img, true);
	اگر (img == تہی)
	    واپس FAIL;

	اگر (img == cur_img)
	    جاؤ image_مکمل;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	رکھو = image_placement_new(img, wp->w_id, iplace_use, false);

	اگر (رکھو == تہی)
	{
	    image_unref(img);
	    واپس FAIL;
	}

	اگر (wp->w_popup_imagep != تہی)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = رکھو;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	اگر (dict_ڈھونڈو(dict, (char_u *)"minwidth", -1) == تہی)
	    wp->w_minwidth = cw;
	اگر (dict_ڈھونڈو(dict, (char_u *)"maxwidth", -1) == تہی)
	    wp->w_maxwidth = cw;
	اگر (dict_ڈھونڈو(dict, (char_u *)"minheight", -1) == تہی)
	    wp->w_minheight = ch;
	اگر (dict_ڈھونڈو(dict, (char_u *)"maxheight", -1) == تہی)
	    wp->w_maxheight = ch;
image_مکمل:
	;
    }
    ورنہ
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	اگر (wp->w_popup_imagep != تہی)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(تہی, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	اگر (wp->w_popup_imagep != تہی)
	    image_placement_سیٹ_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_اونچائی, wp->w_چوڑائی);
    }
#اگر_ختم

    di = dict_ڈھونڈو(dict, (char_u *)"close", -1);
    اگر (di != تہی)
    {
	صحیح ok = TRUE;

	اگر (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != تہی)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    اگر (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    ورنہ اگر (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    ورنہ اگر (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    ورنہ
		ok = FALSE;
	}
	ورنہ
	    ok = FALSE;
	اگر (!ok)
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    اگر (str != تہی)
    {
	حرف *errmsg = update_wincolor(wp, str);

	اگر (errmsg == تہی)
	    سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ورنہ
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    اگر (str != تہی)
    {
	حرف *errmsg = update_winhighlight(wp, str);

	اگر (errmsg == تہی)
	    سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ورنہ
	    emsg(_(errmsg));
    }

    اگر (سیٹ_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	سیٹ_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	واپس FAIL;

    di = dict_ڈھونڈو(dict, (char_u *)"borderhighlight", -1);
    اگر (di != تہی)
    {
	اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == تہی)
	{
	    emsg(_(e_list_required));
	    واپس FAIL;
	}
	ورنہ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحیح		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    اگر (list->lv_len == 0)
	    {
		برائے (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = تہی;
		}
	    }
	    ورنہ
	    {
		برائے (i = 0, li = list->lv_پہلا; i < 4 && i < list->lv_len;
						     ++i, li = li->li_اگلا)
		{
		    str = tv_get_string(&li->li_tv);
		    اگر (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			اگر (hl != تہی)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		اگر (list->lv_len == 1 && wp->w_border_highlight[0] != تہی)
		    برائے (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			اگر (hl != تہی)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_ڈھونڈو(dict, (char_u *)"borderchars", -1);
    اگر (di != تہی)
    {
	اگر (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    واپس FAIL;
	}
	ورنہ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحیح		i;

	    اگر (list != تہی)
	    {
		CHECK_LIST_MATERIALIZE(list);
		برائے (i = 0, li = list->lv_پہلا; i < 8 && i < list->lv_len;
							 ++i, li = li->li_اگلا)
		{
		    str = tv_get_string(&li->li_tv);
		    اگر (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		اگر (list->lv_len == 1)
		    برائے (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		اگر (list->lv_len == 2)
		{
		    برائے (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    برائے (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_ڈھونڈو(dict, (char_u *)"zindex", -1);
    اگر (di != تہی)
    {
	wp->w_zindex = dict_get_نمبر(dict, "zindex");
	اگر (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	اگر (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_ڈھونڈو(dict, (char_u *)"mask", -1);
    اگر (di != تہی)
    {
	صحیح ok = FALSE;

	اگر (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != تہی)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_اشیاء(di->di_tv.vval.v_list, li)
	    {
		اگر (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == تہی
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    توڑو;
		}
		ورنہ
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	اگر (ok)
	{
	    wp->w_popup_نقاب = di->di_tv.vval.v_list;
	    ++wp->w_popup_نقاب->lv_refcount;
	    VIM_CLEAR(wp->w_popup_نقاب_cells);
	}
	ورنہ
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "mask");
	    واپس FAIL;
	}
    }

#اگر defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_نمبر(dict, "time");
    اگر (nr > 0)
	popup_جوڑو_timeout(wp, nr, TRUE);
#اگر_ختم

    di = dict_ڈھونڈو(dict, (char_u *)"moved", -1);
    اگر (di != تہی)
    {
	سیٹ_moved_اقدار(wp);
	handle_moved_دلیل(wp, di, FALSE);
    }

    di = dict_ڈھونڈو(dict, (char_u *)"mousemoved", -1);
    اگر (di != تہی)
    {
	سیٹ_mousemoved_اقدار(wp);
	handle_moved_دلیل(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    اگر (nr != -1)
    {
	اگر (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	ورنہ
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_ڈھونڈو(dict, (char_u *)"filter", -1);
    اگر (di != تہی)
    {
	callback_T	callback = get_callback(&di->di_tv);

	اگر (callback.cb_name != تہی)
	{
	    free_callback(&wp->w_filter_cb);
	    سیٹ_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	ورنہ
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    اگر (str != تہی)
    {
	اگر (STRCMP(str, "a") == 0)
	    wp->w_filter_منوال = منوال_ALL;
	ورنہ
	    wp->w_filter_منوال = منوال_str2flags(str);
    }

    di = dict_ڈھونڈو(dict, (char_u *)"callback", -1);
    اگر (di == تہی)
	واپس OK;

    callback_T	callback = get_callback(&di->di_tv);
    اگر (callback.cb_name == تہی)
	واپس OK;

    free_callback(&wp->w_close_cb);
    سیٹ_callback(&wp->w_close_cb, &callback);
    واپس OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ساکن صحیح
apply_options(win_T *wp, dict_T *dict, صحیح create)
{
    صحیح		nr;

    apply_move_options(wp, dict);

    اگر (create)
	سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    اگر (apply_general_options(wp, dict) == FAIL)
	واپس FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    اگر (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    اگر (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	اگر (wp->w_firstline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_cursor.lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
	ورنہ اگر (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_اونچائی)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    اگر (create)
	popup_نقاب_refresh = TRUE;
    popup_highlight_curline(wp);

    واپس OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ساکن خالی
جوڑو_popup_لڑیاں(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_اشیاء(l, li)
	اگر (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == تہی ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ساکن خالی
جوڑو_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_اشیاء(l, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_دلیل_1_list_شے_nr_dictionary_required), lnum + 1);
	    واپس;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == تہی ? تہی : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == تہی ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    برائے (li = l->lv_پہلا; li != تہی; li = li->li_اگلا, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_ڈھونڈو(dict, (char_u *)"props", -1);
	اگر (di != تہی)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		واپس;
	    }
	    plist = di->di_tv.vval.v_list;
	    اگر (plist != تہی)
	    {
		FOR_ALL_LIST_اشیاء(plist, pli)
		{
		    اگر (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			واپس;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    اگر (dict != تہی)
		    {
			صحیح ستون = dict_get_نمبر(dict, "col");

			prop_جوڑو_common(lnum, ستون, dict, buf, تہی);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    صحیح
popup_چوٹی_extra(win_T *wp)
{
    صحیح	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    اگر (extra == 0 && wp->w_popup_title != تہی && *wp->w_popup_title != NUL)
	واپس 1;
    واپس extra;
}

/*
 * Get the padding plus border at the left.
 */
    صحیح
popup_بایاں_extra(win_T *wp)
{
    واپس wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    صحیح
popup_اونچائی(win_T *wp)
{
    واپس wp->w_اونچائی
	    + popup_چوٹی_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    صحیح
popup_چوڑائی(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    واپس wp->w_چوڑائی
	    + popup_extra_چوڑائی(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    صحیح
popup_extra_چوڑائی(win_T *wp)
{
    واپس wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ساکن win_T *
popup_get_clipwin(win_T *wp)
{
    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	واپس تہی;
    اگر (win_valid(wp->w_popup_prop_win))
	واپس wp->w_popup_prop_win;
    واپس تہی;
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
قسمتعریف ساخت {
    صحیح چوٹی_extra;
    صحیح bot_extra;
    صحیح بایاں_extra;
    صحیح دایاں_extra;

    صحیح clip_چوٹی_content;
    صحیح clip_bot_content;
    صحیح clip_بایاں_content;
    صحیح clip_دایاں_content;

    صحیح eff_چوٹی_extra;
    صحیح eff_bot_extra;
    صحیح eff_بایاں_extra;
    صحیح eff_دایاں_extra;

    صحیح eff_border[4];
    صحیح eff_padding[4];

    صحیح eff_اونچائی;
    صحیح eff_چوڑائی;
} popup_clip_T;

    ساکن خالی
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    صحیح h, w;

    cl->چوٹی_extra = popup_چوٹی_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->دایاں_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_چوٹی_content = wp->w_popup_topoff - cl->چوٹی_extra;
    اگر (cl->clip_چوٹی_content < 0)
	cl->clip_چوٹی_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    اگر (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_بایاں_content = wp->w_popup_leftclip - cl->بایاں_extra;
    اگر (cl->clip_بایاں_content < 0)
	cl->clip_بایاں_content = 0;
    cl->clip_دایاں_content = wp->w_popup_rightclip - cl->دایاں_extra;
    اگر (cl->clip_دایاں_content < 0)
	cl->clip_دایاں_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	صحیح clip = wp->w_popup_topoff;
	صحیح b = wp->w_popup_border[0];
	صحیح p = wp->w_popup_padding[0];
	صحیح rem;

	اگر (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	ورنہ
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	صحیح clip = wp->w_popup_bottomoff;
	صحیح b = wp->w_popup_border[2];
	صحیح p = wp->w_popup_padding[2];
	صحیح rem;

	اگر (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	ورنہ
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
	صحیح excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_اونچائی;
	اگر (excess > 0)
	{
	    اگر (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		اگر (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		ورنہ
		    cl->eff_border[0] -= excess;
	    }
	    ورنہ
		cl->eff_padding[0] -= excess;
	}
    }
    {
	صحیح excess = wp->w_popup_topoff - cl->چوٹی_extra - wp->w_اونچائی;
	اگر (excess > 0)
	{
	    اگر (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		اگر (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		ورنہ
		    cl->eff_border[2] -= excess;
	    }
	    ورنہ
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_چوٹی_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_بایاں_extra = wp->w_popup_leftclip > 0 ? 0 : cl->بایاں_extra;
    cl->eff_دایاں_extra = wp->w_popup_rightclip > 0 ? 0 : cl->دایاں_extra;

    h = wp->w_اونچائی - cl->clip_چوٹی_content - cl->clip_bot_content;
    اگر (h < 0)
	h = 0;
    cl->eff_اونچائی = cl->eff_چوٹی_extra + h + cl->eff_bot_extra;

    w = wp->w_چوڑائی - cl->clip_بایاں_content - cl->clip_دایاں_content;
    اگر (w < 0)
	w = 0;
    cl->eff_چوڑائی = cl->eff_بایاں_extra + w + cl->eff_دایاں_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
قسمتعریف ساخت {
    صحیح		اونچائی;
    صحیح		چوڑائی;
    صحیح		winrow;
    صحیح		wincol;
    صحیح		leftcol;
    صحیح		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ساکن خالی
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->اونچائی  = wp->w_اونچائی;
    sv->چوڑائی   = wp->w_چوڑائی;
    sv->winrow  = wp->w_winrow;
    sv->wincol  = wp->w_wincol;
    sv->leftcol = wp->w_leftcol;
    sv->p_wrap  = wp->w_p_wrap;
    sv->topline = wp->w_topline;
}

    ساکن خالی
popup_geom_restore(win_T *wp, popup_geom_save_T *sv)
{
    wp->w_p_wrap  = sv->p_wrap;
    wp->w_leftcol = sv->leftcol;
    wp->w_wincol  = sv->wincol;
    wp->w_winrow  = sv->winrow;
    wp->w_topline = sv->topline;
    wp->w_چوڑائی   = sv->چوڑائی;
    wp->w_اونچائی  = sv->اونچائی;
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
    ساکن خالی
popup_screenpos_above_چوٹی(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	صحیح	    *screen_صف,
	صحیح	    *screen_scol,
	صحیح	    *screen_ccol,
	صحیح	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    صحیح	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (صحیح)win_ستون_off(prop_win) - (صحیح)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (صحیح)scol + coloff;
    *screen_ccol = (صحیح)ccol + coloff;
    *screen_ecol = (صحیح)ecol + coloff;
    *screen_صف = prop_win->w_winrow + 1
				 - (صحیح)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ساکن خالی
popup_hide_for_textprop(win_T *wp)
{
    اگر ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	واپس;
    wp->w_popup_flags |= POPF_HIDDEN;
    اگر (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    ساکن bool
popup_ڈھونڈو_prop_above_چوٹی(
	win_T	    *wp,
	win_T	    *prop_win,
	صحیح	    max_رسائی,
	textprop_T  *prop,
	linenr_T    *ملا_lnum)
{
    linenr_T	پہلا;

    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	واپس false;

    پہلا = max_رسائی > 0 ? prop_win->w_topline - max_رسائی : 1;
    اگر (پہلا < 1)
	پہلا = 1;
    واپس ڈھونڈو_prop_اندر_سطریں(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, ملا_lnum, پہلا, prop_win->w_topline - 1);
}

/*
 * Compute and assign w_popup_topoff/bottomoff/leftclip/rightclip from the
 * host (textprop) window's content rectangle when POPF_CLIPWINDOW is set.
 * The popup's logical geometry (w_winrow, w_height, w_width) is preserved;
 * only the *off/clip fields record how much of each edge falls outside.
 * Returns true when the popup has scrolled completely past one of the host
 * edges, in which case the caller must hide it.
 */
    ساکن bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    صحیح	    extra_h, extra_w;
    صحیح	    popup_چوٹی, popup_bottom, popup_بایاں, popup_دایاں;
    صحیح	    کل_h, کل_w;

    اگر (cw == تہی)
	واپس false;

    extra_h = popup_چوٹی_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_چوڑائی(wp);

    popup_چوٹی = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_اونچائی + extra_h;
    popup_بایاں = wp->w_wincol;
    popup_دایاں = wp->w_wincol + wp->w_چوڑائی + extra_w;
    کل_h = wp->w_اونچائی + extra_h;
    کل_w = wp->w_چوڑائی + extra_w;

    اگر (popup_چوٹی < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_چوٹی;
    اگر (popup_bottom > cw->w_winrow + cw->w_اونچائی)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_اونچائی);
    اگر (popup_بایاں < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_بایاں;
    اگر (popup_دایاں > cw->w_wincol + cw->w_چوڑائی)
	wp->w_popup_rightclip = popup_دایاں - (cw->w_wincol + cw->w_چوڑائی);

    واپس wp->w_popup_topoff >= کل_h
	|| wp->w_popup_bottomoff >= کل_h
	|| wp->w_popup_leftclip >= کل_w
	|| wp->w_popup_rightclip >= کل_w;
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
    ساکن خالی
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_اونچائی -= cl->clip_چوٹی_content + cl->clip_bot_content;
	اگر (wp->w_اونچائی < 0)
	    wp->w_اونچائی = 0;
	اگر (cl->clip_چوٹی_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_چوٹی_content;
    }
    اگر (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	اگر (cl->clip_بایاں_content > 0 || cl->clip_دایاں_content > 0)
	    wp->w_p_wrap = FALSE;
	اگر (cl->clip_دایاں_content > 0)
	{
	    wp->w_چوڑائی -= cl->clip_دایاں_content;
	    اگر (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
	اگر (cl->clip_بایاں_content > 0)
	{
	    wp->w_leftcol += cl->clip_بایاں_content;
	    wp->w_wincol += cl->clip_بایاں_content;
	    wp->w_چوڑائی -= cl->clip_بایاں_content;
	    اگر (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
    }
}

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ساکن خالی
popup_position_image(win_T *wp)
{
    linenr_T	    صف;
    colnr_T	    ستون;

    صحیح iw, ih;

    // Visible cell width and height of popup itself (not image!)
    صحیح	visible_چوڑائی;
    صحیح	visible_اونچائی;

    // Crop dimensions of image (in pixels).
    صحیح	crop_x, crop_y, crop_چوڑائی, crop_اونچائی;

    popup_clip_T clip;

    اگر (wp->w_popup_imagep == تہی || wp->w_popup_flags & POPF_HIDDEN)
	واپس;

    image_placement_سیٹ_enable(wp->w_popup_imagep, true);

    صف = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    ستون = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    اگر (wp->w_popup_imagep->img == تہی)
	// Just set the bounding box
	جاؤ باہر_نکلو;

    visible_چوڑائی =
	wp->w_چوڑائی - clip.clip_بایاں_content - clip.clip_دایاں_content;
    visible_اونچائی =
	wp->w_اونچائی - clip.clip_چوٹی_content - clip.clip_bot_content;

    اگر (visible_چوڑائی < 0 || visible_اونچائی < 0)
	واپس;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_بایاں_content;
    crop_y = clip.clip_چوٹی_content;
    crop_چوڑائی = iw - crop_x - clip.clip_دایاں_content;
    crop_اونچائی = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    صف -= wp->w_popup_border[0] - clip.eff_border[0];
    صف -= wp->w_popup_padding[0] - clip.eff_padding[0];

    ستون -= wp->w_popup_border[3] - clip.eff_border[3];
    ستون -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    اگر (crop_چوڑائی > visible_چوڑائی)
	crop_چوڑائی = visible_چوڑائی;
    اگر (crop_اونچائی > visible_اونچائی)
	crop_اونچائی = visible_اونچائی;

    image_placement_سیٹ_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_چوڑائی, crop_اونچائی);
باہر_نکلو:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_سیٹ_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_سیٹ_position(wp->w_popup_imagep, صف, ستون);
    image_placement_سیٹ_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_اونچائی, clip.eff_چوڑائی);
}
#اگر_ختم


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    ساکن خالی
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    صحیح		wrapped = 0;
    صحیح		maxwidth;
    صحیح		maxwidth_no_scrollbar;
    صحیح		چوڑائی_with_scrollbar = 0;
    صحیح		مستعمل_maxwidth = FALSE;
    صحیح		margin_چوڑائی = 0;
    صحیح		maxspace;
    صحیح		center_vert = FALSE;
    صحیح		center_hor = FALSE;
    صحیح		allow_adjust_بایاں = !wp->w_popup_fixed;
    صحیح		چوٹی_extra = popup_چوٹی_extra(wp);
    صحیح		دایاں_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    صحیح		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    صحیح		بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    صحیح		extra_اونچائی = چوٹی_extra + bot_extra;
    صحیح		extra_چوڑائی = بایاں_extra + دایاں_extra;
    صحیح		w_اونچائی_before_حد_بندی;
    popup_layout_T org_layout;
    صحیح		minwidth, minheight;
    صحیح		maxheight = صفیں;
    صحیح		wantline = wp->w_wantline;  // adjusted for textprop
    صحیح		wantcol = wp->w_wantcol;    // adjusted for textprop
    صحیح		use_wantcol = wantcol != 0;
    صحیح		adjust_اونچائی_for_چوٹی_aligned = FALSE;

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
    اگر (wp->w_popup_آخری_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    اگر (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	صحیح	    screen_صف;
	صحیح	    screen_scol;
	صحیح	    screen_ccol;
	صحیح	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_چوٹی = false;
	اگر (!ڈھونڈو_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    اگر (popup_ڈھونڈو_prop_above_چوٹی(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_چوٹی = true;
	    ورنہ
	    {
		اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
		    اگر (wp->w_popup_imagep != تہی)
			image_placement_clear(wp->w_popup_imagep);
#اگر_ختم
		    popup_hide_for_textprop(wp);
		    اگر (wp->w_winrow + popup_اونچائی(wp) >= cmdline_صف)
			clear_cmdline = TRUE;
		}
		واپس;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.ستون = prop.tp_ستون;
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.ستون += prop.tp_len - 1;
	اگر (prop_above_چوٹی)
	    popup_screenpos_above_چوٹی(prop_win, &pos, prop_lnum, &screen_صف,
				     &screen_scol, &screen_ccol, &screen_ecol);
	ورنہ
	    textpos2screenpos(prop_win, &pos, &screen_صف,
				     &screen_scol, &screen_ccol, &screen_ecol);

	اگر (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_چوڑائی = 0;
	    واپس;
	}
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_صف + wantline + 1;
	ورنہ
	    // above the text
	    wantline = screen_صف + wantline - 1;
	center_vert = FALSE;
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	ورنہ
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    ورنہ
    {
	// If no line was specified default to vertical centering.
	اگر (wantline == 0)
	    center_vert = TRUE;
	ورنہ اگر (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	اگر (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    اگر (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    ورنہ
    {
	اگر (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    اگر (wp->w_winrow >= صفیں)
		wp->w_winrow = صفیں - 1;
	}
	اگر (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_صف
				    - wp->w_بفر->b_ml.ml_سطر_گنتی - 1, 0);
	}

	اگر (!use_wantcol)
	    center_hor = TRUE;
	ورنہ اگر (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    اگر (wp->w_wincol > firstwin->w_wincol + topframe->fr_چوڑائی
						- بایاں_extra - دایاں_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی
						- بایاں_extra - دایاں_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    اگر (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_چوڑائی
					- wp->w_wincol - بایاں_extra - دایاں_extra;
    maxwidth = maxspace;
    اگر (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_بایاں = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    اگر (wp->w_p_nu || wp->w_p_rnu)
	margin_چوڑائی = نمبر_چوڑائی(wp) + 1;
#اگر_تعریف_شدہ FEAT_FOLDING
    margin_چوڑائی += wp->w_p_fdc;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
    اگر (signcolumn_on(wp))
	margin_چوڑائی += 2;
#اگر_ختم
    اگر (margin_چوڑائی >= maxwidth)
	margin_چوڑائی = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#اگر_تعریف_شدہ FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    اگر (wp->w_بفر->b_رکن != تہی)
    {
	اگر (minwidth == 0)
	    minwidth = 20;
	اگر (minheight == 0)
	    minheight = 5;
    }
#اگر_ختم

    اگر (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    ورنہ اگر (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_صف - 1;

    // start at the desired first line
    اگر (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	اگر (wp->w_firstline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_firstline = 0;
	ورنہ
	    wp->w_topline = wp->w_firstline;
    }
    اگر (wp->w_topline < 1)
	wp->w_topline = 1;
    ورنہ اگر (wp->w_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	wp->w_topline = wp->w_بفر->b_ml.ml_سطر_گنتی;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_چوڑائی = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	صحیح saved_w_چوڑائی = wp->w_چوڑائی;

	اگر (wp->w_چوڑائی < maxwidth)
	    wp->w_چوڑائی = maxwidth;
	برائے (ln = 1; ln <= wp->w_بفر->b_ml.ml_سطر_گنتی; ++ln)
	{
	    صحیح len = linetabsize(wp, ln) + margin_چوڑائی;

	    اگر (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    اگر (saved_w_چوڑائی < len)
		saved_w_چوڑائی = len;
	    اگر (wp->w_maxwidth > 0 && saved_w_چوڑائی >= wp->w_maxwidth)
		توڑو;
	}
	wp->w_چوڑائی = saved_w_چوڑائی;
    }
    اگر (wp->w_firstline < 0)
	lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
    ورنہ
	lnum = wp->w_topline;
    جب (lnum >= 1 && lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	صحیح len;
	صحیح w_چوڑائی = wp->w_چوڑائی;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	اگر (wp->w_چوڑائی < maxwidth)
	    wp->w_چوڑائی = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_چوڑائی = w_چوڑائی;

	اگر (len + margin_چوڑائی > maxwidth
		&& allow_adjust_بایاں
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    صحیح سرکاؤ_by = len + margin_چوڑائی - maxwidth;

	    اگر (سرکاؤ_by > wp->w_wincol)
	    {
		صحیح truncate_سرکاؤ = سرکاؤ_by - wp->w_wincol;

		سرکاؤ_by -= truncate_سرکاؤ;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    اگر (wp->w_maxwidth > 0 && maxwidth + سرکاؤ_by > wp->w_maxwidth)
		سرکاؤ_by = wp->w_maxwidth - maxwidth;

	    اگر (سرکاؤ_by > 0)
	    {
		wp->w_wincol -= سرکاؤ_by;
		maxwidth += سرکاؤ_by;
	    }
	    wp->w_چوڑائی = maxwidth;
	}
	اگر (wp->w_p_wrap)
	{
	    جب (len + margin_چوڑائی > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_چوڑائی;
		wp->w_چوڑائی = maxwidth;
		مستعمل_maxwidth = TRUE;
	    }
	}
	اگر (wp->w_چوڑائی < len + margin_چوڑائی)
	{
	    wp->w_چوڑائی = len + margin_چوڑائی;
	    اگر (wp->w_maxwidth > 0 && wp->w_چوڑائی > wp->w_maxwidth)
		wp->w_چوڑائی = wp->w_maxwidth;
	}

	اگر (wp->w_firstline < 0)
	    --lnum;
	ورنہ
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	اگر (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_بفر->b_ml.ml_سطر_گنتی - lnum)
		       + wrapped >= maxheight)
	    توڑو;
    }

    اگر (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی);
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (wp->w_بفر->b_رکن != تہی && !رکن_is_ختم_شدہ(wp->w_بفر))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#اگر_ختم
    maxwidth_no_scrollbar = maxwidth;
    اگر (wp->w_has_scrollbar)
    {
	++دایاں_extra;
	++extra_چوڑائی;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	اگر (maxwidth + دایاں_extra >= maxspace
		&& (مستعمل_maxwidth || (minwidth > 0 && wp->w_چوڑائی < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    اگر (wp->w_popup_title != تہی && *wp->w_popup_title != NUL)
    {
	صحیح title_len = vim_strsize(wp->w_popup_title) + 2 - extra_چوڑائی;

	اگر (minwidth < title_len)
	    minwidth = title_len;
    }

    اگر (minwidth > 0 && wp->w_چوڑائی < minwidth)
	wp->w_چوڑائی = minwidth;
    اگر (wp->w_چوڑائی > maxwidth)
    {
	اگر (wp->w_چوڑائی > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_چوڑائی - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	اگر (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    صحیح off = wp->w_چوڑائی - maxwidth;
	    extra_چوڑائی -= MIN(off, دایاں_extra);
	    wp->w_چوڑائی = maxwidth_no_scrollbar;
	}
	ورنہ
	{
	    wp->w_چوڑائی = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    چوڑائی_with_scrollbar = maxwidth_no_scrollbar - دایاں_extra;
	}
    }
    اگر (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_چوڑائی - wp->w_چوڑائی - extra_چوڑائی) / 2;
	اگر (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    ورنہ اگر (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	صحیح leftoff = wantcol - (wp->w_چوڑائی + extra_چوڑائی);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	اگر (leftoff >= 0)
	    wp->w_wincol = leftoff;
	ورنہ اگر (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_چوڑائی -= wp->w_leftcol;
	    اگر (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
    }

    اگر (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	صحیح want_ستون = 0;

	// try to show the right border and any scrollbar
	want_ستون = بایاں_extra + wp->w_چوڑائی + دایاں_extra;
	اگر (want_ستون > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_ستون >= firstwin->w_wincol + topframe->fr_چوڑائی)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی - want_ستون;
	    اگر (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_اونچائی = wp->w_بفر->b_ml.ml_سطر_گنتی - wp->w_topline
								 + 1 + wrapped;
    اگر (minheight > 0 && wp->w_اونچائی < minheight)
	wp->w_اونچائی = minheight;
    اگر (maxheight > 0 && wp->w_اونچائی > maxheight)
	wp->w_اونچائی = maxheight;
    w_اونچائی_before_حد_بندی = wp->w_اونچائی;
    اگر (wp->w_اونچائی > صفیں - wp->w_winrow)
	wp->w_اونچائی = صفیں - wp->w_winrow;

    اگر (center_vert)
    {
	wp->w_winrow = (صفیں - wp->w_اونچائی - extra_اونچائی) / 2;
	اگر (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    ورنہ اگر (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	اگر ((wp->w_اونچائی + extra_اونچائی) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_اونچائی + extra_اونچائی);
	ورنہ اگر (wantline * 2 >= صفیں || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_اونچائی = wantline - extra_اونچائی;
	}
	ورنہ
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_اونچائی_for_چوٹی_aligned = TRUE;
	}
    }
    ورنہ اگر (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	صحیح check_اونچائی = (wp->w_popup_flags & POPF_INFO) ? wp->w_اونچائی
						    : w_اونچائی_before_حد_بندی;
	اگر (wp != popup_dragwin
		&& wantline + (check_اونچائی + extra_اونچائی) - 1 > صفیں
		&& wantline * 2 > صفیں
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_اونچائی = w_اونچائی_before_حد_بندی;
	    wp->w_winrow = wantline - 2 - wp->w_اونچائی - extra_اونچائی;
	    اگر (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_اونچائی + 2;
	    اگر (wp->w_winrow < 0)
	    {
		wp->w_اونچائی += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	ورنہ
	{
	    wp->w_winrow = wantline - 1;
	    adjust_اونچائی_for_چوٹی_aligned = TRUE;
	}
    }

    اگر (adjust_اونچائی_for_چوٹی_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_اونچائی + extra_اونچائی > صفیں)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_اونچائی = صفیں - wp->w_winrow - extra_اونچائی;
#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (wp->w_بفر->b_رکن == تہی || رکن_is_ختم_شدہ(wp->w_بفر))
#اگر_ختم
	{
	    // The width already accounts for a scrollbar that was there.
	    اگر (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		اگر (چوڑائی_with_scrollbar > 0)
		    wp->w_چوڑائی = چوڑائی_with_scrollbar;
		ورنہ
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_چوڑائی;
	    }
	}
    }

    // make sure w_winrow is valid
    اگر (wp->w_winrow >= صفیں)
	wp->w_winrow = صفیں - 1;
    ورنہ اگر (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    اگر (wp->w_wincol + wp->w_چوڑائی + extra_چوڑائی
				    > firstwin->w_wincol + topframe->fr_چوڑائی)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی
						- wp->w_چوڑائی - extra_چوڑائی;
    اگر (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    اگر (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    اگر (wp->w_wincol + wp->w_چوڑائی + extra_چوڑائی
				    > firstwin->w_wincol + topframe->fr_چوڑائی)
    {
	صحیح avail = firstwin->w_wincol + topframe->fr_چوڑائی
						- wp->w_wincol - extra_چوڑائی;
	wp->w_چوڑائی = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	اگر (wp->w_winrow + wp->w_اونچائی + extra_اونچائی > صفیں)
	    wp->w_winrow = صفیں - wp->w_اونچائی - extra_اونچائی;
	اگر (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	اگر (wp->w_winrow + wp->w_اونچائی + extra_اونچائی > صفیں)
	{
	    صحیح avail = صفیں - wp->w_winrow - extra_اونچائی;
	    wp->w_اونچائی = avail > 0 ? avail : 0;
	}
    }

    اگر (wp->w_اونچائی != org_layout.اونچائی)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    اگر (popup_compute_clipwindow_offsets(wp))
    {
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
	اگر (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    اگر (wp->w_popup_imagep != تہی)
		image_placement_clear(wp->w_popup_imagep);
#اگر_ختم

	popup_hide_for_textprop(wp);
	واپس;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_آخری_changedtick = CHANGEDTICK(wp->w_بفر);
    اگر (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_بفر);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_چوڑائی = wp->w_popup_prop_win->w_چوڑائی;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_اونچائی;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    اگر (popup_layout_بدلا(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	اگر (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_نقاب_refresh = TRUE;
    }

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    popup_position_image(wp);
#اگر_ختم
}

قسمتعریف شمار
{
    TYPE_معمول,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_پیغام_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    ساکن صحیح
popup_is_notification(create_type_T type)
{
    واپس type == TYPE_NOTIFICATION || type == TYPE_پیغام_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ساکن خالی
popup_سیٹ_بفر_متن(buf_T *buf, typval_T متن)
{
    صحیح	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    برائے (lnum = buf->b_ml.ml_سطر_گنتی; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_بفر;

    // Add text to the buffer.
    اگر (متن.v_type == VAR_STRING)
    {
	char_u *s = متن.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == تہی ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    ورنہ
    {
	list_T *l = متن.vval.v_list;

	اگر (l != تہی && l->lv_len > 0)
	{
	    اگر (l->lv_پہلا == &range_list_شے)
		emsg(_(e_using_نمبر_as_string));
	    ورنہ اگر (l->lv_پہلا->li_tv.v_type == VAR_STRING)
		// list of strings
		جوڑو_popup_لڑیاں(buf, l);
	    ورنہ
		// list of dictionaries
		جوڑو_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_سطر_گنتی);
    curbuf = curwin->w_بفر;
}

#تعریف_کرو سیٹ_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    کرو {						    \
	اگر (wp != تہی)					    \
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
    } جب (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ساکن صحیح
parse_popup_option(win_T *wp, صحیح is_preview)
{
    char_u *p =
#اگر_تعریف_شدہ FEAT_QUICKFIX
	!is_preview ? p_cpp :
#اگر_ختم
	p_pvp;
    صحیح	    border_enabled = FALSE;

    اگر (wp != تہی)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    برائے ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	صحیح	x;

	e = vim_strchr(p, ':');
	اگر (e == تہی || e[1] == NUL)
	    واپس FAIL;

	p = vim_strchr(e, ',');
	اگر (p == تہی)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	اگر (STRNCMP(s, "height:", 7) == 0)
	{
	    اگر (dig != p)
		واپس FAIL;
	    اگر (wp != تہی)
	    {
		اگر (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	ورنہ اگر (STRNCMP(s, "width:", 6) == 0)
	{
	    اگر (dig != p)
		واپس FAIL;
	    اگر (wp != تہی)
	    {
		اگر (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	ورنہ اگر (STRNCMP(s, "highlight:", 10) == 0)
	{
	    اگر (wp != تہی)
	    {
		حرف *errmsg;
		صحیح c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		اگر (errmsg == تہی)
		    سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		ورنہ
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	ورنہ اگر (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    اگر (*arg == NUL || *arg == ',')
		واپس FAIL;
	    اگر (wp != تہی)
	    {
		برائے (صحیح i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	ورنہ اگر (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحیح		i;
	    صحیح		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    صحیح		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    اگر (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		اگر (wp != تہی)
		{
		    برائے (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		جاری;
	    }

	    token = vim_strnsave(arg, token_len);
	    اگر (token == تہی)
		واپس FAIL;

	    اگر ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		اگر (STRCMP(token, "single") == 0)
		    سیٹ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		ورنہ اگر (STRCMP(token, "double") == 0)
		    سیٹ_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		ورنہ اگر (STRCMP(token, "round") == 0)
		    سیٹ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		ورنہ اگر (STRCMP(token, "on") == 0)
		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		ورنہ اگر (STRCMP(token, "ascii") == 0)
		    سیٹ_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		ورنہ اگر (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    صحیح		برآمد[8];
		    صحیح		failed = FALSE;

		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    برائے (i = 0; i < 8 && !failed; i++)
		    {
			اگر (*q == NUL)
			    failed = TRUE;
			ورنہ
			{
			    برآمد[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    اگر (i < 7)
			    {
				اگر (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    اگر (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			واپس FAIL;
		    }
		    سیٹ_BORDER_CHARS(برآمد[0], برآمد[1], برآمد[2], برآمد[3], برآمد[4],
			    برآمد[5], برآمد[6], برآمد[7]);
		}
	    }
	    ورنہ
	    {
		vim_free(token);
		واپس FAIL;
	    }

	    اگر (wp != تہی)
	    {
		برائے (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	ورنہ اگر (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		واپس FAIL;
	    on = on && mouse_has(MOUSE_داخل_کرو) && (border_enabled || is_preview);
	    اگر (wp != تہی)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	ورنہ اگر (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		واپس FAIL;
	    اگر (wp != تہی)
	    {
		اگر (on && mouse_has(MOUSE_داخل_کرو))
		    wp->w_popup_flags |= POPF_RESIZE;
		ورنہ
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	ورنہ اگر (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		واپس FAIL;
	    اگر (wp != تہی)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	ورنہ اگر (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    صحیح		شے = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    صحیح		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    اگر (!menu && !شے)
		واپس FAIL;
	    اگر (wp != تہی && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	ورنہ اگر (STRNCMP(s, "opacity:", 8) == 0)
	{
	    اگر (dig != p || x < 0 || x > 100)
		واپس FAIL;
	    اگر (wp != تہی)
	    {
		اگر (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		ورنہ
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	ورنہ
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحیح
parse_previewpopup(win_T *wp)
{
    واپس parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحیح
parse_completepopup(win_T *wp)
{
    واپس parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    خالی
popup_سیٹ_wantpos_cursor(win_T *wp, صحیح چوڑائی, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    اگر (d != تہی)
	ppt = get_pos_اندراج(d, FALSE);

    setcursor_mayforce(TRUE);
    اگر (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    ورنہ
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	اگر (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    اگر (wp->w_wantcol > Columns - چوڑائی)
    {
	wp->w_wantcol = Columns - چوڑائی;
	اگر (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    خالی
popup_سیٹ_wantpos_rowcol(win_T *wp, صحیح صف, صحیح ستون)
{
    wp->w_wantline = صف;
    wp->w_wantcol = ستون;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ساکن خالی
جوڑو_border_بایاں_دایاں_padding(win_T *wp)
{
    صحیح i;

    برائے (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#اگر_تعریف_شدہ FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ساکن صحیح
popup_terminal_exists(خالی)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_بفر->b_رکن != تہی)
	    واپس TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	    اگر (wp->w_بفر->b_رکن != تہی)
		واپس TRUE;
    واپس FALSE;
}
#اگر_ختم

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    خالی
popup_redraw_all(خالی)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    صحیح
popup_need_redraw(خالی)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    واپس TRUE;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Set the color for a notification window.
 */
    ساکن خالی
popup_update_color(win_T *wp, create_type_T type)
{
    حرف    *hiname = type == TYPE_پیغام_WIN
				       ? "MessageWindow" : "PopupNotification";
    حرف    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    اگر (errmsg == تہی)
	سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    ورنہ
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ساکن win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = تہی;
    صحیح		tabnr = 0;
    صحیح		new_بفر;
    buf_T	*buf = تہی;
    dict_T	*d = تہی;
    صحیح		i;

    اگر (check_secure())
	واپس تہی;

    اگر (argvars != تہی)
    {
	اگر (اندر_vim9script()
		&& (check_for_string_or_نمبر_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    واپس تہی;

	// Check that arguments look OK.
	اگر (argvars[0].v_type == VAR_نمبر)
	{
	    buf = buflist_findnr(argvars[0].vval.v_نمبر);
	    اگر (buf == تہی)
	    {
		semsg(_(e_بفر_nr_does_not_exist), argvars[0].vval.v_نمبر);
		واپس تہی;
	    }
#اگر_تعریف_شدہ FEAT_TERMINAL
	    اگر (buf->b_رکن != تہی && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_دوسرا_popup_with_terminal));
		واپس تہی;
	    }
#اگر_ختم
	}
	ورنہ اگر (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != تہی)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != تہی))
	{
	    emsg(_(e_بفر_نمبر_متن_or_list_required));
	    واپس تہی;
	}
	اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    واپس تہی;
	d = argvars[1].vval.v_dict;
    }

    اگر (d != تہی)
    {
	اگر (dict_has_کلید(d, "tabpage"))
	    tabnr = (صحیح)dict_get_نمبر(d, "tabpage");
	ورنہ اگر (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	ورنہ
	    tabnr = 0;
	اگر (tabnr > 0)
	{
	    tp = ڈھونڈو_tabpage(tabnr);
	    اگر (tp == تہی)
	    {
		semsg(_(e_tabpage_not_ملا_nr), tabnr);
		واپس تہی;
	    }
	}
    }
    ورنہ اگر (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    اگر (buf != تہی && buf->b_locked_بانٹو)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_کھڑکی_مقصد_a_closing_بفر));
	واپس تہی;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    اگر (wp == تہی)
	واپس تہی;
    اگر (rettv != تہی)
	rettv->vval.v_نمبر = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    اگر (buf != تہی)
    {
	// use existing buffer
	new_بفر = FALSE;
	win_init_popup_win(wp, buf);
	سیٹ_local_options_default(wp, FALSE);
	تبادلہ_exists_action = SEA_READONLY;
	بفر_ensure_loaded(buf);
	تبادلہ_exists_action = SEA_NONE;
    }
    ورنہ
    {
	// create a new buffer associated with the popup
	new_بفر = TRUE;
	buf = buflist_new(تہی, تہی, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	اگر (buf == تہی)
	{
	    win_free_popup(wp);
	    واپس تہی;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	سیٹ_local_options_default(wp, TRUE);
	سیٹ_string_option_براہ_راست_اندر_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	سیٹ_string_option_براہ_راست_اندر_buf(buf, (char_u *)"bufhidden", -1,
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

    اگر (tp != تہی)
    {
	// popup on specified tab page
	wp->w_اگلا = tp->tp_پہلا_popupwin;
	tp->tp_پہلا_popupwin = wp;
    }
    ورنہ اگر (tabnr == 0)
    {
	// popup on current tab page
	wp->w_اگلا = curtab->tp_پہلا_popupwin;
	curtab->tp_پہلا_popupwin = wp;
    }
    ورنہ // (tabnr < 0)
    {
	win_T *پچھلا = پہلا_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	اگر (پہلا_popupwin == تہی)
	    پہلا_popupwin = wp;
	ورنہ
	{
	    جب (پچھلا->w_اگلا != تہی)
		پچھلا = پچھلا->w_اگلا;
	    پچھلا->w_اگلا = wp;
	}
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#اگر_ختم
    }

    اگر (new_بفر && argvars != تہی)
	popup_سیٹ_بفر_متن(buf, argvars[0]);

    اگر (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    اگر (type == TYPE_ATCURSOR)
    {
	popup_سیٹ_wantpos_cursor(wp, 0, d);
	سیٹ_moved_اقدار(wp);
	سیٹ_moved_columns(wp, ڈھونڈو_STRING);
    }

    اگر (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_صف;
	اگر (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_ستون + 1;
	سیٹ_mousemoved_اقدار(wp);
	سیٹ_mousemoved_columns(wp, ڈھونڈو_IDENT + ڈھونڈو_STRING + ڈھونڈو_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    اگر (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	صحیح	اونچائی = buf->b_ml.ml_سطر_گنتی + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	برائے (twp = پہلا_popupwin; twp != تہی; twp = nextwin)
	{
	    nextwin = twp->w_اگلا;
	    اگر (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + اونچائی
		    && twp->w_winrow + popup_اونچائی(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_اونچائی(twp) + 1;
		nextwin = پہلا_popupwin;
	    }
	}
	اگر (wp->w_wantline + اونچائی > صفیں)
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
	برائے (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    اگر (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	جوڑو_border_بایاں_دایاں_padding(wp);
    }

    اگر (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	اگر (callback.cb_name != تہی)
	{
	    سیٹ_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    برائے (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    برائے (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    اگر (type == TYPE_PREVIEW)
    {
	اگر (mouse_has(MOUSE_داخل_کرو))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	برائے (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_سیٹ_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#اگر_تعریف_شدہ FEAT_QUICKFIX
    اگر (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	اگر (mouse_has(MOUSE_داخل_کرو))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	جوڑو_border_بایاں_دایاں_padding(wp);
	parse_completepopup(wp);
    }
#اگر_ختم

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_منوال = منوال_ALL;

    اگر (d != تہی)
    {
	// Deal with options.
	اگر (apply_options(wp, d, TRUE) == FAIL)
	{
	    (خالی)popup_close(wp->w_id, FALSE);
	    واپس تہی;
	}
    }

#اگر_تعریف_شدہ FEAT_TIMERS
    اگر (popup_is_notification(type) && wp->w_popup_timer == تہی)
	popup_جوڑو_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#اگر_ختم

    popup_adjust_position(wp);

    wp->w_vsep_چوڑائی = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_نقاب_refresh = TRUE;

#اگر_تعریف_شدہ FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    اگر (buf->b_رکن != تہی)
	win_enter(wp, FALSE);
#اگر_ختم

    واپس wp;
}

/*
 * popup_clear()
 */
    خالی
f_popup_clear(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح force = FALSE;

    اگر (اندر_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
	force = (صحیح)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    خالی
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_معمول);
}

/*
 * popup_atcursor({text}, {options})
 */
    خالی
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    خالی
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ساکن خالی
invoke_popup_callback(win_T *wp, typval_T *نتیجہ)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = (varnumber_T)wp->w_id;

    اگر (نتیجہ != تہی && نتیجہ->v_type != VAR_UNKNOWN)
	copy_tv(نتیجہ, &argv[1]);
    ورنہ
    {
	argv[1].v_type = VAR_نمبر;
	argv[1].vval.v_نمبر = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    اگر (نتیجہ != تہی)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ساکن خالی
عقب_مقصد_prevwin(win_T *wp)
{
    اگر (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    ورنہ
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ساکن خالی
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    صحیح id;

    اگر (!win_valid(wp))
       واپس;

    id = wp->w_id;

#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (wp == curwin && curbuf->b_رکن != تہی)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    اگر (owp != curwin && owp->w_بفر->b_رکن != تہی)
		توڑو;
	اگر (owp != تہی)
	    win_enter(owp, FALSE);
	ورنہ
	{
	    FOR_ALL_POPUPWINS_اندر_TAB(curtab, owp)
		اگر (owp != curwin && owp->w_بفر->b_رکن != تہی)
		    توڑو;
	    اگر (owp != تہی)
		win_enter(owp, FALSE);
	    ورنہ
		عقب_مقصد_prevwin(wp);
	}
    }
#اگر_ختم

    // Just in case a check higher up is missing.
    اگر (wp == curwin && ERROR_IF_POPUP_کھڑکی)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	واپس;
    }

    CHECK_CURBUF;
    اگر (wp->w_close_cb.cb_name != تہی)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    خالی
popup_close_with_retval(win_T *wp, صحیح retval)
{
    typval_T res;

    res.v_type = VAR_نمبر;
    res.vval.v_نمبر = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    خالی
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ساکن خالی
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    اگر (wp != mouse_wp
	    && wp->w_popup_mouse_صف != 0
	    && (wp->w_popup_mouse_صف != mouse_صف
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
    خالی
popup_handle_mouse_moved(خالی)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    صحیح	    صف = mouse_صف;
    صحیح	    ستون = mouse_ستون;

    // find the window where the mouse is in
    mouse_wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);

    برائے (wp = پہلا_popupwin; wp != تہی; wp = nextwp)
    {
	nextwp = wp->w_اگلا;
	check_mouse_moved(wp, mouse_wp);
    }
    برائے (wp = curtab->tp_پہلا_popupwin; wp != تہی; wp = nextwp)
    {
	nextwp = wp->w_اگلا;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ساکن خالی
filter_handle_drag(win_T *wp, صحیح c, typval_T *rettv)
{
    صحیح	صف = mouse_صف;
    صحیح	ستون = mouse_ستون;

    اگر ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_کلید(c)
	    && (wp == popup_dragwin
			  || wp == mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_نمبر = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    خالی
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحیح		c;
    linenr_T	old_lnum;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    id = tv_get_نمبر(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    اگر (wp == تہی)
	واپس;

    c = *کلید;
    اگر (c == K_SPECIAL && کلید[1] != NUL)
	c = مقصد_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_TRUE;
    res.v_type = VAR_نمبر;

    old_lnum = wp->w_cursor.lnum;
    اگر (c == 'k' || c == 'K' || c == K_اوپر || c == Ctrl_P)
    {
	اگر (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	ورنہ
	    wp->w_cursor.lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
    }
    اگر (c == 'j' || c == 'J' || c == K_نیچے_کو || c == Ctrl_N)
    {
	اگر (wp->w_cursor.lnum < wp->w_بفر->b_ml.ml_سطر_گنتی)
	    ++wp->w_cursor.lnum;
	ورنہ
	    wp->w_cursor.lnum = 1;
    }
    اگر (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	واپس;
    }

    اگر (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_نمبر = -1;
	popup_close_and_callback(wp, &res);
	واپس;
    }
    اگر (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_نمبر = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	واپس;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    خالی
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحیح		c;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    id = tv_get_نمبر(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    اگر (wp == تہی)
	واپس;

    c = *کلید;
    اگر (c == CAR && need_wait_return)
	واپس;
    اگر (c == K_SPECIAL && کلید[1] != NUL)
	c = مقصد_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_TRUE;

    اگر (c == 'y' || c == 'Y')
	res.vval.v_نمبر = 1;
    ورنہ اگر (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_نمبر = 0;
    ورنہ
    {
	filter_handle_drag(wp, c, rettv);
	واپس;
    }

    // Invoke callback
    res.v_type = VAR_نمبر;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    خالی
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    خالی
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    خالی
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ساکن win_T *
ڈھونڈو_popup_win(صحیح id)
{
    win_T *wp = win_id2wp(id);

    اگر (wp != تہی && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_کھڑکی_nr_is_not_popup_کھڑکی), id);
	واپس تہی;
    }
    واپس wp;
}

/*
 * popup_close({id})
 */
    خالی
f_popup_close(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح		id;
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    اگر (
#اگر_تعریف_شدہ FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_رکن == تہی &&
#اگر_ختم
	    ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    wp = ڈھونڈو_popup_win(id);
    اگر (wp != تہی)
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
    ساکن خالی
popup_clear_نقاب_for(win_T *wp)
{
    صحیح r, c;
    صحیح صف_آغاز, ستون_آغاز, صف_انتہا, ستون_انتہا;

    اگر (popup_نقاب == تہی || !popup_visible)
	واپس;

    صف_آغاز = MAX(wp->w_winrow, 0);
    ستون_آغاز = MAX(wp->w_wincol, 0);
    صف_انتہا = MIN(wp->w_winrow + popup_اونچائی(wp), (صحیح)screen_صفیں);
    ستون_انتہا = MIN(wp->w_wincol + popup_چوڑائی(wp), (صحیح)screen_Columns);

    برائے (r = صف_آغاز; r < صف_انتہا; ++r)
	برائے (c = ستون_آغاز; c < ستون_انتہا; ++c)
	    popup_نقاب[r * screen_Columns + c] = 0;
}

    خالی
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    صحیح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (error_if_رکن_popup_کھڑکی())
	واپس;
#اگر_ختم
    اگر ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	واپس;

    popup_save_area(wp, &old_area);

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != تہی)
	image_placement_clear(wp->w_popup_imagep);
#اگر_ختم

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    اگر (wp->w_winrow + popup_اونچائی(wp) >= cmdline_صف)
	clear_cmdline = TRUE;

    اگر (was_visible)
	popup_clear_نقاب_for(wp);

    اگر (old_area.فعال)
	popup_redraw_exposed_area(&old_area);
    ورنہ
	redraw_all_later(UPD_NOT_VALID);

    حالت_redraw_all();
    اگر (!old_area.فعال)
	popup_نقاب_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    خالی
f_popup_hide(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح		id;
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    خالی
popup_show(win_T *wp)
{
    bool popup_فعال;

    اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	واپس;

    popup_فعال = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    اگر (popup_فعال)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطریں_valid = 0;
	اگر (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    ورنہ
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_نقاب_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    خالی
f_popup_show(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح		id;
    win_T	*wp;

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#اگر_تعریف_شدہ FEAT_QUICKFIX
    اگر (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#اگر_ختم

    rettv->vval.v_نمبر = 0;
}

/*
 * popup_settext({id}, {text})
 */
    خالی
f_popup_settext(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	واپس;

    id = (صحیح)tv_get_نمبر(&argvars[0]);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;

    popup_save_area(wp, &old_area);

    اگر (check_for_string_or_list_arg(argvars, 1) == FAIL)
	واپس;

    popup_سیٹ_بفر_متن(wp->w_بفر, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطریں_valid = 0;
    اگر (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    اگر (popup_area_بدلا(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    خالی
f_popup_setbuf(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_FALSE;

    اگر (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_بفر_arg(argvars, 1) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(&argvars[0]);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;

    buf = tv_get_buf_from_arg(&argvars[1]);

    اگر (buf == تہی)
	واپس;
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (buf->b_رکن != تہی && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_دوسرا_popup_with_terminal));
	واپس;
    }
#اگر_ختم

    اگر (wp->w_بفر != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_بفر->b_nwindows--;
	win_init_popup_win(wp, buf);
	سیٹ_local_options_default(wp, FALSE);
	تبادلہ_exists_action = SEA_READONLY;
	بفر_ensure_loaded(buf);
	تبادلہ_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	اگر (popup_area_بدلا(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_نمبر = VVAL_TRUE;
}

    ساکن خالی
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    صحیح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != تہی)
	image_placement_free(wp->w_popup_imagep);
#اگر_ختم

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_بفر->b_locked = FALSE;
    اگر (wp->w_winrow + popup_اونچائی(wp) >= cmdline_صف)
	clear_cmdline = TRUE;

    اگر (was_visible)
	popup_clear_نقاب_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    اگر (wp == پیغام_win_2)
	پیغام_win_2 = تہی;
#اگر_ختم

    اگر (!old_area.فعال)
	redraw_all_later(UPD_NOT_VALID);
    حالت_redraw_all();
    اگر (!old_area.فعال)
	popup_نقاب_refresh = TRUE;
}

    ساکن خالی
error_for_popup_کھڑکی(خالی)
{
    emsg(_(e_not_allowed_اندر_popup_کھڑکی));
}

    صحیح
error_if_popup_کھڑکی(صحیح also_with_رکن غیر_مستعمل)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    اگر (WIN_IS_POPUP(curwin)
#اگر_تعریف_شدہ FEAT_TERMINAL
	    && (also_with_رکن || curbuf->b_رکن == تہی)
#اگر_ختم
	    )
    {
	error_for_popup_کھڑکی();
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    صحیح
popup_close(صحیح id, صحیح force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*پچھلا = تہی;

    // go through global popups
    برائے (wp = پہلا_popupwin; wp != تہی; پچھلا = wp, wp = wp->w_اگلا)
	اگر (wp->w_id == id)
	{
	    اگر (wp == curwin)
	    {
		اگر (!force)
		{
		    error_for_popup_کھڑکی();
		    واپس FAIL;
		}
		عقب_مقصد_prevwin(wp);
	    }
	    اگر (پچھلا == تہی)
		پہلا_popupwin = wp->w_اگلا;
	    ورنہ
		پچھلا->w_اگلا = wp->w_اگلا;
#اگر_تعریف_شدہ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#اگر_ختم
	    popup_free(wp);
	    واپس OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	اگر (popup_close_tabpage(tp, id, force) == OK)
	    واپس OK;
    واپس FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    صحیح
popup_close_tabpage(tabpage_T *tp, صحیح id, صحیح force)
{
    win_T	*wp;
    win_T	**جڑ = &tp->tp_پہلا_popupwin;
    win_T	*پچھلا = تہی;

    برائے (wp = *جڑ; wp != تہی; پچھلا = wp, wp = wp->w_اگلا)
	اگر (wp->w_id == id)
	{
	    اگر (wp == curwin)
	    {
		اگر (!force)
		{
		    error_for_popup_کھڑکی();
		    واپس FAIL;
		}
		عقب_مقصد_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    اگر (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    اگر (پچھلا == تہی)
		*جڑ = wp->w_اگلا;
	    ورنہ
		پچھلا->w_اگلا = wp->w_اگلا;
#اگر_تعریف_شدہ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#اگر_ختم
	    popup_free(wp);
	    واپس OK;
	}
    واپس FAIL;
}

    خالی
close_all_popups(صحیح force)
{
    اگر (!force && ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    جب (پہلا_popupwin != تہی)
	اگر (popup_close(پہلا_popupwin->w_id, force) == FAIL)
	    واپس;
    جب (curtab->tp_پہلا_popupwin != تہی)
	اگر (popup_close(curtab->tp_پہلا_popupwin->w_id, force) == FAIL)
	    واپس;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ساکن خالی
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->فعال = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->اونچائی = popup_اونچائی(wp);
    area->چوڑائی = popup_چوڑائی(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    ساکن خالی
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    صحیح i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    برائے (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ساکن bool
popup_style_بدلا(win_T *wp, popup_style_snapshot_T *style)
{
    صحیح i;

    اگر (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	واپس true;
    برائے (i = 0; i < 4; i++)
	اگر (style->border_highlight[i] != wp->w_border_highlight[i])
	    واپس true;
    واپس false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ساکن خالی
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->چوڑائی = wp->w_چوڑائی;
    layout->اونچائی = wp->w_اونچائی;
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
    ساکن bool
popup_layout_بدلا(win_T *wp, popup_layout_T *layout)
{
    واپس layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->چوڑائی != wp->w_چوڑائی
	|| layout->اونچائی != wp->w_اونچائی
	|| layout->has_scrollbar != wp->w_has_scrollbar
	|| layout->topoff != wp->w_popup_topoff
	|| layout->bottomoff != wp->w_popup_bottomoff
	|| layout->leftclip != wp->w_popup_leftclip
	|| layout->rightclip != wp->w_popup_rightclip;
}

/*
 * Return true when the popup no longer covers the saved area.
 */
    ساکن bool
popup_area_بدلا(win_T *wp, popup_area_T *area)
{
    واپس area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->اونچائی != popup_اونچائی(wp)
	|| area->چوڑائی != popup_چوڑائی(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ساکن خالی
mark_overlapped_opacity_popup(win_T *wp, صحیح area_چوٹی, صحیح area_bot,
	صحیح area_بایاں, صحیح area_دایاں, صحیح zindex)
{
    اگر ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_اونچائی(wp) <= area_چوٹی
	    || wp->w_wincol >= area_دایاں
	    || wp->w_wincol + popup_چوڑائی(wp) - wp->w_popup_leftoff
							      <= area_بایاں)
	واپس;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطریں_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ساکن خالی
redraw_overlapped_opacity_popups(صحیح winrow, صحیح wincol, صحیح اونچائی, صحیح چوڑائی,
	صحیح leftoff, صحیح zindex)
{
    win_T	*wp;
    صحیح		area_چوٹی = winrow;
    صحیح		area_bot = winrow + اونچائی;
    صحیح		area_بایاں = wincol;
    صحیح		area_دایاں = wincol + چوڑائی - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_چوٹی, area_bot, area_بایاں,
		area_دایاں, zindex);
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_چوٹی, area_bot, area_بایاں,
		area_دایاں, zindex);

    اگر (must_redraw < UPD_VALID)
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
    ساکن خالی
refill_pum_bg_صف_from_بفر(صحیح r, صحیح بایاں, صحیح دایاں)
{
    صحیح		سطر_cp = r;
    صحیح		ستون_cp = بایاں;
    win_T	*wp;
    linenr_T	lnum;
    صحیح		صف_for_lnum;
    صحیح		ستون_for_lnum = 0;
    char_u	*سطر;
    char_u	*p;
    صحیح		win_متن_ستون;
    صحیح		screen_ستون;
    صحیح		soff_بنیاد = (r - pum_bg_چوٹی) * pum_bg_ستون_تعداد;
    صحیح		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    برائے (c = بایاں; c < دایاں; ++c)
    {
	pum_bg_سطریں[soff_بنیاد + c] = ' ';
	اگر (pum_bg_attrs != تہی)
	    pum_bg_attrs[soff_بنیاد + c] = 0;
	اگر (enc_utf8 && pum_bg_سطریں_UC != تہی)
	    pum_bg_سطریں_UC[soff_بنیاد + c] = 0;
	اگر (enc_utf8)
	{
	    صحیح k;
	    برائے (k = 0; k < MAX_MCO; ++k)
		اگر (pum_bg_سطریں_C[k] != تہی)
		    pum_bg_سطریں_C[k][soff_بنیاد + c] = 0;
	}
    }

    wp = mouse_ڈھونڈو_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
    اگر (wp == تہی || سطر_cp < 0 || سطر_cp >= wp->w_اونچائی)
	واپس;
    اگر (wp->w_بفر == تہی || wp->w_بفر->b_ml.ml_mfp == تہی)
	واپس;

    // Compute the buffer line for this screen row.
    صف_for_lnum = سطر_cp;
    اگر (mouse_comp_pos(wp, &صف_for_lnum, &ستون_for_lnum, &lnum, تہی))
	واپس;	// past end of buffer
    اگر (lnum < 1 || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	واپس;

    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    اگر (سطر == تہی)
	واپس;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_متن_ستون = wp->w_wincol + win_ستون_off(wp);
    اگر (!wp->w_p_wrap)
	win_متن_ستون -= wp->w_leftcol;
    screen_ستون = win_متن_ستون;
    p = سطر;
    جب (*p != NUL && screen_ستون < دایاں)
    {
	صحیح char_cells;
	صحیح byte_گنتی;
	صحیح soff = soff_بنیاد + screen_ستون;
	صحیح اندر_range = (screen_ستون >= بایاں && screen_ستون < دایاں);

	اگر (*p == '\t')
	{
	    صحیح ts = (صحیح)wp->w_بفر->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_ستون - win_متن_ستون) % ts) : 1;
	    byte_گنتی = 1;
	    اگر (اندر_range)
	    {
		pum_bg_سطریں[soff] = ' ';
		اگر (enc_utf8 && pum_bg_سطریں_UC != تہی)
		    pum_bg_سطریں_UC[soff] = 0;
	    }
	}
	ورنہ اگر (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_گنتی = mb_ptr2len(p);
	    اگر (اندر_range)
	    {
		pum_bg_سطریں[soff] = *p;
		اگر (enc_utf8 && pum_bg_سطریں_UC != تہی)
		    pum_bg_سطریں_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	ورنہ
	{
	    char_cells = 1;
	    byte_گنتی = 1;
	    اگر (اندر_range)
	    {
		pum_bg_سطریں[soff] = (*p < 0x20) ? ' ' : *p;
		اگر (enc_utf8 && pum_bg_سطریں_UC != تہی)
		    pum_bg_سطریں_UC[soff] = 0;
	    }
	}
	اگر (اندر_range && pum_bg_attrs != تہی)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_گنتی;
	screen_ستون += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ساکن خالی
popup_redraw_exposed_area(popup_area_T *area)
{
    اگر (!area->فعال)
	واپس;

    redraw_under_popup_area(area->winrow, area->wincol, area->اونچائی,
	    area->چوڑائی, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->اونچائی, area->چوڑائی, area->leftoff, area->zindex);

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
    اگر (pum_bg_سطریں != تہی
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->اونچائی > pum_bg_چوٹی)
    {
	صحیح چوٹی = MAX(area->winrow, pum_bg_چوٹی);
	صحیح bot = MIN(area->winrow + area->اونچائی, pum_bg_bot);
	صحیح بایاں = MAX(area->wincol, 0);
	صحیح دایاں = MIN(area->wincol + area->چوڑائی, pum_bg_ستون_تعداد);
	صحیح r;

	برائے (r = چوٹی; r < bot; ++r)
	    refill_pum_bg_صف_from_بفر(r, بایاں, دایاں);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ساکن خالی
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->سطریں);
    vim_free(saved_screen->attrs);
    vim_free(saved_screen->linesuc);
    CLEAR_اشارہ_گر(saved_screen);
}

/*
 * Save the screen area that opacity padding may need to blend against.
 * On entry "saved_screen" must be zero-initialised.  When the snapshot is
 * unavailable (no opacity popup, no padding, allocation failure) "lines" is
 * left NULL and callers should fall back to plain screen_fill().  The caller
 * must always release the snapshot with popup_free_saved_screen().
 */
    ساکن خالی
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    اگر (screen_opacity_popup == تہی
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	واپس;

    saved_screen->آغاز_صف = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->آغاز_ستون = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->صفیں_2 = wp->w_popup_padding[0] + wp->w_اونچائی
						    + wp->w_popup_padding[2];
    saved_screen->ستون_تعداد = wp->w_popup_padding[3] + wp->w_چوڑائی
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    اگر (saved_screen->آغاز_ستون > 0)
    {
	--saved_screen->آغاز_ستون;
	++saved_screen->ستون_تعداد;
    }

    saved_screen->سطریں = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->صفیں_2 * saved_screen->ستون_تعداد);
    saved_screen->attrs = ALLOC_CLEAR_MULT(صحیح,
				  saved_screen->صفیں_2 * saved_screen->ستون_تعداد);
    اگر (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->صفیں_2 * saved_screen->ستون_تعداد);

    اگر (saved_screen->سطریں == تہی || saved_screen->attrs == تہی)
	واپس;

    برائے (صحیح r = 0; r < saved_screen->صفیں_2; r++)
    {
	صحیح screen_صف = saved_screen->آغاز_صف + r;

	اگر (screen_صف >= 0 && screen_صف < screen_صفیں)
	    برائے (صحیح c = 0; c < saved_screen->ستون_تعداد; c++)
	    {
		صحیح screen_ستون = saved_screen->آغاز_ستون + c;

		اگر (screen_ستون >= 0 && screen_ستون < screen_Columns)
		{
		    صحیح off = سطر_انحراف[screen_صف] + screen_ستون;
		    صحیح save_off = r * saved_screen->ستون_تعداد + c;

		    saved_screen->سطریں[save_off] = Screen_سطریں[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    اگر (enc_utf8 && saved_screen->linesuc != تہی)
			saved_screen->linesuc[save_off] = Screen_سطریں_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ساکن خالی
redraw_under_popup_area(صحیح winrow, صحیح wincol, صحیح اونچائی, صحیح چوڑائی, صحیح leftoff)
{
    صحیح	    r;

    برائے (r = winrow; r < winrow + اونچائی && r < screen_صفیں; ++r)
    {
	صحیح	    c;
	win_T	    *پچھلا_twp = تہی;

	اگر (r >= cmdline_صف)
	{
	    clear_cmdline = TRUE;
	    جاری;
	}

	برائے (c = wincol; c < wincol + چوڑائی - leftoff && c < screen_Columns; ++c)
	{
	    صحیح	    سطر_cp = r;
	    صحیح	    ستون_cp = c;
	    win_T   *twp;

	    twp = mouse_ڈھونڈو_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
	    اگر (twp != تہی && twp != پچھلا_twp)
	    {
		پچھلا_twp = twp;
		اگر (سطر_cp < twp->w_اونچائی)
		{
		    linenr_T lnum;

		    (خالی)mouse_comp_pos(twp, &سطر_cp, &ستون_cp, &lnum, تہی);
		    redrawWinline(twp, lnum);
		}
		ورنہ اگر (سطر_cp == twp->w_اونچائی)
		    twp->w_redr_حالت = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    خالی
f_popup_move(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;  // invalid {id}

    اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	واپس;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    اگر (wp->w_winrow + wp->w_اونچائی >= cmdline_صف)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    اگر (popup_area_بدلا(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    خالی
f_popup_setoptions(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    صحیح		need_redraw = FALSE;
    صحیح		need_reposition = FALSE;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;  // invalid {id}

    اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	واپس;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (خالی)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    اگر (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    اگر (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    ورنہ اگر (popup_style_بدلا(wp, &old_style))
	need_redraw = TRUE;

    اگر (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    اگر (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_نقاب_refresh = TRUE;
    }
    ورنہ اگر (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطریں_valid = 0;
	اگر (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    اگر (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_نقاب_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    اگر (popup_area_بدلا(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    خالی
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    صحیح		چوٹی_extra;
    صحیح		بایاں_extra;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;  // invalid {id}
    چوٹی_extra = popup_چوٹی_extra(wp);
    بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_تالا_سائز(&dict->dv_hashtab, 11);

    dict_جوڑو_نمبر(dict, "line", wp->w_winrow + 1);
    dict_جوڑو_نمبر(dict, "col", wp->w_wincol + 1);
    dict_جوڑو_نمبر(dict, "width", wp->w_چوڑائی + بایاں_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_جوڑو_نمبر(dict, "height", wp->w_اونچائی + چوٹی_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_جوڑو_نمبر(dict, "core_line", wp->w_winrow + 1 + چوٹی_extra);
    dict_جوڑو_نمبر(dict, "core_col", wp->w_wincol + 1 + بایاں_extra);
    dict_جوڑو_نمبر(dict, "core_width", wp->w_چوڑائی);
    dict_جوڑو_نمبر(dict, "core_height", wp->w_اونچائی);

    dict_جوڑو_نمبر(dict, "scrollbar", wp->w_has_scrollbar);
    dict_جوڑو_نمبر(dict, "firstline", wp->w_topline);
    dict_جوڑو_نمبر(dict, "lastline", wp->w_botline - 1);
    dict_جوڑو_نمبر(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    خالی
f_popup_list(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    FOR_ALL_POPUPWINS(wp)
	list_append_نمبر(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	    list_append_نمبر(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    خالی
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    صحیح		صف;
    صحیح		ستون;
    win_T	*wp;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    صف = tv_get_نمبر(&argvars[0]) - 1;
    ستون = tv_get_نمبر(&argvars[1]) - 1;
    wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
    اگر (wp != تہی && WIN_IS_POPUP(wp))
	rettv->vval.v_نمبر = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ساکن خالی
get_padding_border(dict_T *dict, صحیح *array, حرف *name)
{
    list_T  *list;
    صحیح	    i;

    اگر (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	واپس;

    list = list_alloc();
    اگر (list == تہی)
	واپس;

    اگر (dict_جوڑو_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	واپس;
    }
    اگر (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	برائے (i = 0; i < 4; ++i)
	    list_append_نمبر(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ساکن خالی
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحیح	    i;

    برائے (i = 0; i < 4; ++i)
	اگر (wp->w_border_highlight[i] != تہی)
	    توڑو;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    اگر (i == 4 && !wp->w_border_highlight_isset)
	واپس;

    list = list_alloc();
    اگر (list == تہی)
	واپس;

    اگر (dict_جوڑو_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	واپس;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    اگر (i == 4)
	واپس;
    برائے (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ساکن خالی
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحیح	    i;
    char_u  buf[NUMBUFLEN];
    صحیح	    len;

    برائے (i = 0; i < 8; ++i)
	اگر (wp->w_border_char[i] != 0)
	    توڑو;
    اگر (i == 8)
	واپس;

    list = list_alloc();
    اگر (list == تہی)
	واپس;

    اگر (dict_جوڑو_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	واپس;
    }
    برائے (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ساکن خالی
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    اگر (list != تہی)
    {
	اگر (dict_جوڑو_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	ورنہ
	{
	    list_append_نمبر(list, wp->w_popup_lnum);
	    list_append_نمبر(list, wp->w_popup_mincol);
	    list_append_نمبر(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    اگر (list == تہی)
	واپس;

    اگر (dict_جوڑو_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	واپس;
    }
    list_append_نمبر(list, wp->w_popup_mouse_صف);
    list_append_نمبر(list, wp->w_popup_mouse_mincol);
    list_append_نمبر(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    خالی
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    tabpage_T	*tp;
    صحیح		i;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = ڈھونڈو_popup_win(id);
    اگر (wp == تہی)
	واپس;

    dict = rettv->vval.v_dict;
    dict_جوڑو_نمبر(dict, "line", wp->w_wantline);
    dict_جوڑو_نمبر(dict, "col", wp->w_wantcol);
    dict_جوڑو_نمبر(dict, "minwidth", wp->w_minwidth);
    dict_جوڑو_نمبر(dict, "minheight", wp->w_minheight);
    dict_جوڑو_نمبر(dict, "maxheight", wp->w_maxheight);
    dict_جوڑو_نمبر(dict, "maxwidth", wp->w_maxwidth);
    dict_جوڑو_نمبر(dict, "firstline", wp->w_firstline);
    dict_جوڑو_نمبر(dict, "scrollbar", wp->w_want_scrollbar);
    dict_جوڑو_نمبر(dict, "zindex", wp->w_zindex);
    dict_جوڑو_نمبر(dict, "fixed", wp->w_popup_fixed);
    اگر (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = متن_prop_type_by_id(
		wp->w_popup_prop_win->w_بفر,
		wp->w_popup_prop_type);

	اگر (pt != تہی)
	    dict_جوڑو_string(dict, "textprop", pt->pt_name);
	dict_جوڑو_نمبر(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_جوڑو_نمبر(dict, "textpropid", wp->w_popup_prop_id);
    }

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != تہی && wp->w_popup_imagep->img != تہی)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	اگر (dict_جوڑو_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#اگر_ختم

    dict_جوڑو_string(dict, "title", wp->w_popup_title);
    dict_جوڑو_نمبر(dict, "wrap", wp->w_p_wrap);
    dict_جوڑو_نمبر(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_جوڑو_نمبر(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_جوڑو_نمبر(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_جوڑو_نمبر(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_جوڑو_نمبر(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_جوڑو_نمبر(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_جوڑو_نمبر(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_جوڑو_نمبر(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_جوڑو_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_جوڑو_string(dict, "highlights", wp->w_p_whl);
    اگر (wp->w_scrollbar_highlight != تہی)
	dict_جوڑو_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    اگر (wp->w_thumb_highlight != تہی)
	dict_جوڑو_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_اندر_TAB(tp, twp)
	    اگر (twp->w_id == id)
		توڑو;
	اگر (twp != تہی)
	    توڑو;
	++i;
    }
    اگر (tp == تہی)
	i = -1;  // must be global
    ورنہ اگر (tp == curtab)
	i = 0;
    dict_جوڑو_نمبر(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    اگر (wp->w_filter_cb.cb_name != تہی)
	dict_جوڑو_callback(dict, "filter", &wp->w_filter_cb);
    اگر (wp->w_close_cb.cb_name != تہی)
	dict_جوڑو_callback(dict, "callback", &wp->w_close_cb);

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(poppos_entries); ++i)
	اگر (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_جوڑو_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(صحیح)poppos_entries[i].pp_name.لمبائی);
	    توڑو;
	}

    اگر (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_جوڑو_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    ورنہ اگر (wp->w_popup_close == POPCLOSE_CLICK)
	dict_جوڑو_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    ورنہ
	dict_جوڑو_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#اگر defined(FEAT_TIMERS)
    dict_جوڑو_نمبر(dict, "time", wp->w_popup_timer != تہی
	    ?  (طویل)wp->w_popup_timer->tr_interval : 0L);
#اگر_ختم
}

#اگر defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    صحیح
error_if_رکن_popup_کھڑکی(خالی)
{
    اگر (WIN_IS_POPUP(curwin) && curbuf->b_رکن != تہی
					   && رکن_کام_running(curbuf->b_رکن))
    {
	emsg(_(e_not_allowed_for_terminal_اندر_popup_کھڑکی));
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    خالی
popup_reset_handled(صحیح handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
ڈھونڈو_اگلا_popup(صحیح سب_سے_نیچا, صحیح handled_flag)
{
    win_T   *wp;
    win_T   *ملا_wp;
    صحیح	    ملا_zindex;

    ملا_zindex = سب_سے_نیچا ? زیادہ_سے_زیادہ_عدد : 0;
    ملا_wp = تہی;
    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (سب_سے_نیچا ? wp->w_zindex < ملا_zindex
		    : wp->w_zindex > ملا_zindex))
	{
	    ملا_zindex = wp->w_zindex;
	    ملا_wp = wp;
	}
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (سب_سے_نیچا ? wp->w_zindex < ملا_zindex
		    : wp->w_zindex > ملا_zindex))
	{
	    ملا_zindex = wp->w_zindex;
	    ملا_wp = wp;
	}

    اگر (ملا_wp != تہی)
	ملا_wp->w_popup_handled |= handled_flag;
    واپس ملا_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ساکن صحیح
invoke_popup_filter(win_T *wp, صحیح c)
{
    صحیح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    صحیح		پچھلا_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    اگر (c == Ctrl_C)
    {
	صحیح save_got_int = got_int;
	صحیح was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	اگر (was_curwin && wp == curwin)
	    واپس -1;
	واپس TRUE;
    }

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_مقصد_buf(c, mod_نقاب, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    اگر (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    ورنہ
    {
	اگر (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	اگر (win_valid_popup(wp) && did_emsg > پچھلا_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    اگر (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	ورنہ
	{
	    اگر (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    واپس res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    صحیح
popup_do_filter(صحیح c)
{
    ساکن صحیح	recursive = FALSE;
    صحیح		res = FALSE;
    win_T	*wp;
    صحیح		save_کلید_Typed = کلید_Typed;
    صحیح		state;
    صحیح		was_must_redraw = must_redraw;

#اگر_تعریف_شدہ FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    اگر (popup_is_popup(curwin) && curbuf->b_رکن != تہی)
	واپس FALSE;
#اگر_ختم

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    اگر (State == منوال_HITRETURN || State == منوال_ASKMORE
	    || State == منوال_CONFIRM || State == منوال_EXTERNCMD)
	واپس FALSE;

    اگر (recursive)
	واپس FALSE;
    recursive = TRUE;

    اگر (c == K_LEFTMOUSE)
    {
	صحیح صف = mouse_صف;
	صحیح ستون = mouse_ستون;

	wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
	اگر (wp != تہی && popup_close_if_on_X(wp, صف, ستون))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    جب (res == FALSE
		     && (wp = ڈھونڈو_اگلا_popup(FALSE, POPUP_HANDLED_2)) != تہی)
	اگر (wp->w_filter_cb.cb_name != تہی
		&& (wp->w_filter_منوال & state) != 0)
	    res = invoke_popup_filter(wp, c);

    اگر (must_redraw > was_must_redraw)
    {
	صحیح save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    کلید_Typed = save_کلید_Typed;

    // When interrupted return FALSE to avoid looping.
    واپس res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    صحیح
popup_no_mapping(خالی)
{
    صحیح	    باری;
    win_T   *wp;

    برائے (باری = 1; باری <= 2; ++باری)
	برائے (wp = باری == 1 ? پہلا_popupwin : curtab->tp_پہلا_popupwin;
						   wp != تہی; wp = wp->w_اگلا)
	    اگر (wp->w_filter_cb.cb_name != تہی
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		واپس TRUE;
    واپس FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    خالی
popup_check_cursor_pos(خالی)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    جب ((wp = ڈھونڈو_اگلا_popup(TRUE, POPUP_HANDLED_3)) != تہی)
	اگر (wp->w_popup_curwin != تہی
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.ستون < wp->w_popup_mincol
		    || curwin->w_cursor.ستون > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ساکن خالی
popup_update_نقاب(win_T *wp, صحیح چوڑائی, صحیح اونچائی)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    صحیح		صف, ستون;

    اگر (wp->w_popup_نقاب == تہی || چوڑائی == 0 || اونچائی == 0)
    {
	VIM_CLEAR(wp->w_popup_نقاب_cells);
	واپس;
    }
    اگر (wp->w_popup_نقاب_cells != تہی
	    && wp->w_popup_نقاب_اونچائی == اونچائی
	    && wp->w_popup_نقاب_چوڑائی == چوڑائی)
	واپس;  // cache is still valid

    vim_free(wp->w_popup_نقاب_cells);
    اگر (چوڑائی > 0 && (size_t)اونچائی > سائز_MAX / (size_t)چوڑائی)
    {
	wp->w_popup_نقاب_cells = تہی;
	واپس;
    }
    wp->w_popup_نقاب_cells = alloc_clear((size_t)چوڑائی * اونچائی);
    اگر (wp->w_popup_نقاب_cells == تہی)
	واپس;
    cells = wp->w_popup_نقاب_cells;

    FOR_ALL_LIST_اشیاء(wp->w_popup_نقاب, lio)
    {
	صحیح ستون_تعداد, cole;
	صحیح سطریں, linee;

	li = lio->li_tv.vval.v_list->lv_پہلا;
	ستون_تعداد = tv_get_نمبر(&li->li_tv);
	اگر (ستون_تعداد < 0)
	    ستون_تعداد = چوڑائی + ستون_تعداد + 1;
	اگر (ستون_تعداد <= 0)
	    ستون_تعداد = 1;
	li = li->li_اگلا;
	cole = tv_get_نمبر(&li->li_tv);
	اگر (cole < 0)
	    cole = چوڑائی + cole + 1;
	اگر (cole > چوڑائی)
	    cole = چوڑائی;
	li = li->li_اگلا;
	سطریں = tv_get_نمبر(&li->li_tv);
	اگر (سطریں < 0)
	    سطریں = اونچائی + سطریں + 1;
	اگر (سطریں <= 0)
	    سطریں = 1;
	li = li->li_اگلا;
	linee = tv_get_نمبر(&li->li_tv);
	اگر (linee < 0)
	    linee = اونچائی + linee + 1;
	اگر (linee > اونچائی)
	    linee = اونچائی;

	برائے (صف = سطریں - 1; صف < linee; ++صف)
	    برائے (ستون = ستون_تعداد - 1; ستون < cole; ++ستون)
		cells[صف * چوڑائی + ستون] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ساکن صحیح
popup_masked(win_T *wp, صحیح چوڑائی, صحیح اونچائی, صحیح screencol, صحیح screenline)
{
    صحیح ستون = screencol - wp->w_wincol + wp->w_popup_leftoff;
    صحیح سطر = screenline - wp->w_winrow;

    واپس ستون >= 0 && ستون < چوڑائی
	    && سطر >= 0 && سطر < اونچائی
	    && wp->w_popup_نقاب_cells[سطر * چوڑائی + ستون];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ساکن خالی
update_popup_transparent(win_T *wp, صحیح val)
{
    اگر (wp->w_popup_نقاب == تہی)
	واپس;

    صحیح		چوڑائی = popup_چوڑائی(wp);
    صحیح		اونچائی = popup_اونچائی(wp);
    listitem_T	*lio, *li;
    صحیح		ستون_تعداد, cole;
    صحیح		سطریں, linee;
    صحیح		ستون, سطر;

    FOR_ALL_LIST_اشیاء(wp->w_popup_نقاب, lio)
    {
	li = lio->li_tv.vval.v_list->lv_پہلا;
	ستون_تعداد = tv_get_نمبر(&li->li_tv);
	اگر (ستون_تعداد < 0)
	    ستون_تعداد = چوڑائی + ستون_تعداد + 1;
	li = li->li_اگلا;
	cole = tv_get_نمبر(&li->li_tv);
	اگر (cole < 0)
	    cole = چوڑائی + cole + 1;
	li = li->li_اگلا;
	سطریں = tv_get_نمبر(&li->li_tv);
	اگر (سطریں < 0)
	    سطریں = اونچائی + سطریں + 1;
	li = li->li_اگلا;
	linee = tv_get_نمبر(&li->li_tv);
	اگر (linee < 0)
	    linee = اونچائی + linee + 1;

	--ستون_تعداد;
	ستون_تعداد -= wp->w_popup_leftoff;
	اگر (ستون_تعداد < 0)
	    ستون_تعداد = 0;
	cole -= wp->w_popup_leftoff;
	--سطریں;
	اگر (سطریں < 0)
	    سطریں = 0;
	برائے (سطر = سطریں; سطر < linee
		&& سطر + wp->w_winrow < screen_صفیں; ++سطر)
	    برائے (ستون = ستون_تعداد; ستون < cole
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
    ساکن صحیح
check_popup_unhidden(win_T *wp)
{
    اگر (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    ملا = false;

	اگر ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    واپس FALSE;
	اگر (ڈھونڈو_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    ملا = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	ورنہ اگر (popup_ڈھونڈو_prop_above_چوٹی(wp, wp->w_popup_prop_win,
					    popup_اونچائی(wp), &prop, &lnum))
	    ملا = true;
	اگر (ملا)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    واپس TRUE;
	}
    }
    واپس FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ساکن صحیح
popup_need_position_adjust(win_T *wp)
{
    اگر (wp->w_popup_آخری_changedtick != CHANGEDTICK(wp->w_بفر))
	واپس TRUE;
    اگر (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_بفر)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_چوڑائی != wp->w_popup_prop_win->w_چوڑائی
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_اونچائی))
	واپس TRUE;

    // May need to adjust the width if the cursor moved.
    واپس wp->w_cursor.lnum != wp->w_popup_آخری_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ساکن مختصر *opacity_zindex = تہی;
ساکن صحیح    opacity_zindex_صفیں = 0;
ساکن صحیح    opacity_zindex_ستون_تعداد = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ساکن خالی
popup_mark_opacity_zindex(win_T *wp)
{
    صحیح	    چوڑائی;
    صحیح	    اونچائی;
    صحیح	    r, c;

    اگر (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	واپس;

    چوڑائی = popup_چوڑائی(wp);
    اونچائی = popup_اونچائی(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    برائے (r = wp->w_winrow;
		       r < wp->w_winrow + اونچائی - wp->w_popup_topoff
						&& r < screen_صفیں; ++r)
	برائے (c = wp->w_wincol;
		 c < wp->w_wincol + چوڑائی - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    صحیح off = r * screen_Columns + c;
	    اگر (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ساکن خالی
redraw_win_under_opacity_popup(win_T *wp)
{
    صحیح	    اونچائی;
    صحیح	    چوڑائی;
    صحیح	    r;

    اگر (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	واپس;

    اونچائی = popup_اونچائی(wp);
    چوڑائی = popup_چوڑائی(wp);
    برائے (r = wp->w_winrow;
		       r < wp->w_winrow + اونچائی && r < screen_صفیں; ++r)
    {
	صحیح	    ستون;
	win_T	    *پچھلا_twp = تہی;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	برائے (ستون = wp->w_wincol;
		ستون < wp->w_wincol + چوڑائی && ستون < screen_Columns; ++ستون)
	{
	    صحیح	    سطر_cp = r;
	    صحیح	    ستون_cp = ستون;
	    win_T   *twp;

	    twp = mouse_ڈھونڈو_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
	    اگر (twp != تہی && twp != پچھلا_twp)
	    {
		پچھلا_twp = twp;
		اگر (سطر_cp < twp->w_اونچائی)
		{
		    linenr_T lnum;

#اگر_تعریف_شدہ FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    اگر (رکن_do_update_کھڑکی(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    ورنہ
#اگر_ختم
		    {
			(خالی)mouse_comp_pos(twp, &سطر_cp, &ستون_cp, &lnum,
									تہی);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		ورنہ اگر (سطر_cp == twp->w_اونچائی)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_حالت = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    صحیح
popup_is_under_opacity(صحیح صف, صحیح ستون)
{
    اگر (opacity_zindex == تہی
	    || صف < 0 || صف >= opacity_zindex_صفیں
	    || ستون < 0 || ستون >= opacity_zindex_ستون_تعداد)
	واپس FALSE;
    واپس opacity_zindex[صف * opacity_zindex_ستون_تعداد + ستون] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    صحیح
popup_is_over_opacity(صحیح صف, صحیح ستون)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& صف >= wp->w_winrow
		&& صف < wp->w_winrow + popup_اونچائی(wp)
		&& ستون >= wp->w_wincol
		&& ستون < wp->w_wincol + popup_چوڑائی(wp))
	    واپس TRUE;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& صف >= wp->w_winrow
		&& صف < wp->w_winrow + popup_اونچائی(wp)
		&& ستون >= wp->w_wincol
		&& ستون < wp->w_wincol + popup_چوڑائی(wp))
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    صحیح
popup_is_under_opacity_range(صحیح صف, صحیح آغاز_ستون, صحیح انتہا_ستون)
{
    صحیح ستون;

    اگر (opacity_zindex == تہی
	    || صف < 0 || صف >= opacity_zindex_صفیں)
	واپس FALSE;
    برائے (ستون = آغاز_ستون; ستون < انتہا_ستون && ستون < opacity_zindex_ستون_تعداد; ++ستون)
	اگر (opacity_zindex[صف * opacity_zindex_ستون_تعداد + ستون] > screen_zindex)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    خالی
may_update_popup_نقاب(صحیح type)
{
    win_T	*wp;
    مختصر	*نقاب;
    صحیح		سطر, ستون;
    صحیح		redraw_all_popups = FALSE;
    صحیح		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    اگر (popup_نقاب_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_نقاب_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_popup_flags & POPF_HIDDEN)
	    popup_نقاب_refresh |= check_popup_unhidden(wp);
	ورنہ اگر (popup_need_position_adjust(wp))
	    popup_نقاب_refresh = TRUE;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_popup_flags & POPF_HIDDEN)
	    popup_نقاب_refresh |= check_popup_unhidden(wp);
	ورنہ اگر (popup_need_position_adjust(wp))
	    popup_نقاب_refresh = TRUE;

    // Force background windows to redraw rows under opacity popups.
    // Opacity popups don't participate in popup_mask, so their area
    // wouldn't normally be redrawn.  Without this, ScreenAttrs retains
    // blended values from the previous cycle, causing blend accumulation.
    // This must run every cycle, not just when popup_mask_refresh is set.
    //
    // Also build the opacity_zindex array used by screen_char() to suppress
    // output for cells under opacity popups during background draw.
    {
	صحیح has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    اگر ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    اگر ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	اگر (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_صفیں = 0;
	    opacity_zindex_ستون_تعداد = 0;
	}
	ورنہ
	{
	    اگر (opacity_zindex_صفیں != screen_صفیں
		    || opacity_zindex_ستون_تعداد != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(مختصر,
					screen_صفیں * screen_Columns);
		opacity_zindex_صفیں = screen_صفیں;
		opacity_zindex_ستون_تعداد = screen_Columns;
	    }
	    اگر (opacity_zindex != تہی)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_صفیں * screen_Columns * حجم(مختصر));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    اگر (!popup_نقاب_refresh)
	واپس;

    // Need to update the mask, something has changed.
    popup_نقاب_refresh = FALSE;
    popup_نقاب_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    اگر (redrawing_all_win)
	نقاب = popup_نقاب;
    ورنہ
	نقاب = popup_نقاب_اگلا;
    vim_memset(نقاب, 0, (size_t)screen_صفیں * screen_Columns * حجم(مختصر));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    جب ((wp = ڈھونڈو_اگلا_popup(TRUE, POPUP_HANDLED_4)) != تہی)
    {
	صحیح چوڑائی;
	صحیح اونچائی;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	اگر (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    اگر (wp->w_popup_flags & POPF_HIDDEN)
		جاری;
	}

	چوڑائی = popup_چوڑائی(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    اونچائی = cl.eff_اونچائی;
	}
	ورنہ
	    اونچائی = popup_اونچائی(wp);
	popup_update_نقاب(wp, چوڑائی, اونچائی);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    جاری;

	{
	    صحیح نقاب_آغاز = wp->w_winrow;
	    صحیح نقاب_انتہا = نقاب_آغاز + اونچائی;
	    صحیح نقاب_ستون_آغاز = wp->w_wincol + wp->w_popup_leftclip;
	    صحیح نقاب_ستون_انتہا = wp->w_wincol + چوڑائی - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    برائے (سطر = نقاب_آغاز;
			    سطر < نقاب_انتہا && سطر < screen_صفیں; ++سطر)
		برائے (ستون = نقاب_ستون_آغاز;
		     ستون < نقاب_ستون_انتہا && ستون < screen_Columns; ++ستون)
		    اگر (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(سطر, ستون, FALSE))
			نقاب[سطر * screen_Columns + ستون] = POPUPMENU_ZINDEX;
		    ورنہ اگر (wp->w_popup_نقاب_cells == تہی
				    || !popup_masked(wp, چوڑائی, اونچائی, ستون, سطر))
			نقاب[سطر * screen_Columns + ستون] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    اگر (نقاب == popup_نقاب_اگلا)
    {
	صحیح	    *plines_cache = ALLOC_CLEAR_MULT(صحیح, صفیں);
	win_T	    *پچھلا_wp = تہی;

	برائے (سطر = 0; سطر < screen_صفیں; ++سطر)
	{
	    صحیح	    ستون_مکمل = 0;

	    برائے (ستون = 0; ستون < screen_Columns; ++ستون)
	    {
		صحیح off = سطر * screen_Columns + ستون;

		اگر (popup_نقاب[off] != popup_نقاب_اگلا[off])
		{
		    popup_نقاب[off] = popup_نقاب_اگلا[off];

		    اگر (سطر >= cmdline_صف)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			اگر (!پیغام_scrolled && popup_نقاب_اگلا[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    ورنہ اگر (ستون >= ستون_مکمل)
		    {
			linenr_T	lnum;
			صحیح		سطر_cp = سطر;
			صحیح		ستون_cp = ستون;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_ڈھونڈو_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
			اگر (wp != تہی)
			{
#اگر defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    اگر (bt_terminal(wp->w_بفر))
				redraw_win_later(wp, UPD_NOT_VALID);
			    ورنہ
#اگر_ختم
			    {
				اگر (wp != پچھلا_wp)
				{
				    vim_memset(plines_cache, 0,
							   حجم(صحیح) * صفیں);
				    پچھلا_wp = wp;
				}

				اگر (سطر_cp >= wp->w_اونچائی)
				    // In (or below) status line
				    wp->w_redr_حالت = true;
				ورنہ
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (خالی)mouse_comp_pos(wp, &سطر_cp, &ستون_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    ستون_مکمل = wp->w_wincol + wp->w_چوڑائی - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_استعمال_کرتا_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    خالی
may_update_popup_position(خالی)
{
    اگر (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
ساکن schar_T *بنیاد_screenlines = تہی;
ساکن صحیح *بنیاد_screenattrs = تہی;
ساکن u8char_T *بنیاد_screenlinesuc = تہی;
ساکن صحیح بنیاد_screen_صفیں = 0;
ساکن صحیح بنیاد_screen_ستون_تعداد = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    صحیح
popup_get_بنیاد_screen_cell(صحیح صف, صحیح ستون, schar_T *linep, صحیح *attrp,
							  u8char_T *ucp)
{
    اگر (بنیاد_screenlines == تہی || بنیاد_screenattrs == تہی)
	واپس FALSE;
    اگر (صف < 0 || ستون < 0 || صف >= بنیاد_screen_صفیں
						     || ستون >= بنیاد_screen_ستون_تعداد)
	واپس FALSE;

    صحیح off = صف * بنیاد_screen_ستون_تعداد + ستون;
    اگر (linep != تہی)
	*linep = بنیاد_screenlines[off];
    اگر (attrp != تہی)
	*attrp = بنیاد_screenattrs[off];
    اگر (ucp != تہی)
    {
	اگر (enc_utf8 && بنیاد_screenlinesuc != تہی)
	    *ucp = بنیاد_screenlinesuc[off];
	ورنہ
	    *ucp = 0;
    }
    واپس TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    خالی
popup_سیٹ_بنیاد_screen_cell(صحیح صف, صحیح ستون, schar_T سطر, صحیح attr, u8char_T uc)
{
    اگر (بنیاد_screenlines == تہی || بنیاد_screenattrs == تہی)
	واپس;
    اگر (صف < 0 || ستون < 0 || صف >= بنیاد_screen_صفیں
						     || ستون >= بنیاد_screen_ستون_تعداد)
	واپس;

    صحیح off = صف * بنیاد_screen_ستون_تعداد + ستون;
    بنیاد_screenlines[off] = سطر;
    بنیاد_screenattrs[off] = attr;
    اگر (enc_utf8 && بنیاد_screenlinesuc != تہی)
	بنیاد_screenlinesuc[off] = uc;
}
#اگر_ختم

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ساکن خالی
draw_opacity_padding_cell(
	صحیح		صف,
	صحیح		ستون,
	popup_saved_screen_T *saved_screen,
	صحیح		pad_آغاز_ستون,
	صحیح		pad_انتہا_ستون)
{
    اگر (سطر_انحراف == تہی || Screen_سطریں == تہی || ScreenAttrs == تہی
	    || (enc_utf8 && Screen_سطریں_UC == تہی))
	واپس;

    صحیح off = سطر_انحراف[صف] + ستون;
    صحیح r = صف - saved_screen->آغاز_صف;
    صحیح c = ستون - saved_screen->آغاز_ستون;

    اگر (r >= 0 && r < saved_screen->صفیں_2 && c >= 0 && c < saved_screen->ستون_تعداد)
    {
	صحیح save_off = r * saved_screen->ستون_تعداد + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	اگر (enc_utf8 && saved_screen->linesuc != تہی)
	{
	    صحیح بنیاد_ستون = ستون - 1;
	    صحیح بنیاد_off = off - 1;
	    صحیح بنیاد_save_off = save_off - 1;
	    صحیح wide_پچھلا = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    اگر (بنیاد_off >= 0)
	    {
		اگر (Screen_سطریں_UC[بنیاد_off] != 0
			&& utf_char2cells(Screen_سطریں_UC[بنیاد_off]) == 2
			&& Screen_سطریں[off] == 0)
		    wide_پچھلا = TRUE;
	    }
	    اگر (!wide_پچھلا && save_off > 0)
	    {
		اگر (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->سطریں[save_off] == 0)
		    wide_پچھلا = TRUE;
	    }

	    اگر (wide_پچھلا && بنیاد_ستون >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		اگر (بنیاد_ستون < pad_آغاز_ستون)
		{
		    اگر (Screen_سطریں_UC[بنیاد_off] != 0
			    && utf_char2cells(Screen_سطریں_UC[بنیاد_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_سطریں[بنیاد_off] = ' ';
			Screen_سطریں_UC[بنیاد_off] = 0;
			ScreenAttrs[بنیاد_off] = saved_screen->attrs[بنیاد_save_off];
			popup_سیٹ_بنیاد_screen_cell(صف, بنیاد_ستون,
				Screen_سطریں[بنیاد_off],
				ScreenAttrs[بنیاد_off],
				Screen_سطریں_UC[بنیاد_off]);
			screen_char(بنیاد_off, صف, بنیاد_ستون);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_سطریں[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[بنیاد_save_off];
			اگر (enc_utf8)
			    Screen_سطریں_UC[off] = 0;
			صحیح popup_attr_val =
					get_win_attr(screen_opacity_popup);
			صحیح blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_سیٹ_بنیاد_screen_cell(صف, ستون,
				Screen_سطریں[off], ScreenAttrs[off],
				Screen_سطریں_UC[off]);
			screen_char(off, صف, ستون);
			واپس;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    اگر (بنیاد_save_off >= 0
			    && saved_screen->linesuc[بنیاد_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[بنیاد_save_off]) == 2
			    && Screen_سطریں[بنیاد_off] == ' '
			    && Screen_سطریں_UC[بنیاد_off] == 0)
		    {
			صحیح popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			صحیح blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_سطریں[بنیاد_off] =
				    saved_screen->سطریں[بنیاد_save_off];
			Screen_سطریں_UC[بنیاد_off] =
				    saved_screen->linesuc[بنیاد_save_off];
			ScreenAttrs[بنیاد_off] =
				    saved_screen->attrs[بنیاد_save_off];
			ScreenAttrs[بنیاد_off] = hl_blend_attr(
				    ScreenAttrs[بنیاد_off],
				    popup_attr_val, blend, TRUE);

			Screen_سطریں[off] = 0;
			Screen_سطریں_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[بنیاد_off];

			popup_سیٹ_بنیاد_screen_cell(صف, بنیاد_ستون,
				    Screen_سطریں[بنیاد_off],
				    ScreenAttrs[بنیاد_off],
				    Screen_سطریں_UC[بنیاد_off]);
			popup_سیٹ_بنیاد_screen_cell(صف, ستون,
				    Screen_سطریں[off],
				    ScreenAttrs[off],
				    Screen_سطریں_UC[off]);
			screen_char(بنیاد_off, صف, بنیاد_ستون);
			واپس;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_سطریں[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[بنیاد_save_off];
		    اگر (enc_utf8)
			Screen_سطریں_UC[off] = 0;
		    صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_سیٹ_بنیاد_screen_cell(صف, ستون, Screen_سطریں[off],
					       ScreenAttrs[off], Screen_سطریں_UC[off]);
		    screen_char(off, صف, ستون);
		    واپس;
		}

		// Base cell is inside the saved area, redraw the wide char.
		اگر (save_off > 0)
		{
		    Screen_سطریں[بنیاد_off] = saved_screen->سطریں[بنیاد_save_off];
		    ScreenAttrs[بنیاد_off] = saved_screen->attrs[بنیاد_save_off];
		    Screen_سطریں[off] = saved_screen->سطریں[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_سطریں_UC[بنیاد_off] = saved_screen->linesuc[بنیاد_save_off];
		    Screen_سطریں_UC[off] = saved_screen->linesuc[save_off];

		    صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[بنیاد_off] = hl_blend_attr(ScreenAttrs[بنیاد_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[بنیاد_off];
		    popup_سیٹ_بنیاد_screen_cell(صف, بنیاد_ستون, Screen_سطریں[بنیاد_off],
					       ScreenAttrs[بنیاد_off], Screen_سطریں_UC[بنیاد_off]);
		    popup_سیٹ_بنیاد_screen_cell(صف, ستون, Screen_سطریں[off],
					       ScreenAttrs[off], Screen_سطریں_UC[off]);
		    screen_char(بنیاد_off, صف, بنیاد_ستون);
		}
		واپس;
	    }
	}
	Screen_سطریں[off] = saved_screen->سطریں[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	اگر (enc_utf8)
	    Screen_سطریں_UC[off] = saved_screen->linesuc != تہی
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	اگر (enc_utf8 && Screen_سطریں_UC[off] != 0
		&& utf_char2cells(Screen_سطریں_UC[off]) == 2
		&& ستون + 1 >= pad_انتہا_ستون)
	{
	    Screen_سطریں[off] = ' ';
	    Screen_سطریں_UC[off] = 0;
	}

	صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
	صحیح blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_سیٹ_بنیاد_screen_cell(صف, ستون, Screen_سطریں[off],
				   ScreenAttrs[off], Screen_سطریں_UC[off]);
	screen_char(off, صف, ستون);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ساکن خالی
fill_opacity_padding(
	صحیح		آغاز_صف,
	صحیح		انتہا_صف,
	صحیح		آغاز_ستون,
	صحیح		انتہا_ستون,
	popup_saved_screen_T *saved_screen)
{
    برائے (صحیح pad_صف = آغاز_صف; pad_صف < انتہا_صف; pad_صف++)
	برائے (صحیح pad_ستون = آغاز_ستون; pad_ستون < انتہا_ستون; pad_ستون++)
	    draw_opacity_padding_cell(pad_صف, pad_ستون, saved_screen,
		    آغاز_ستون, انتہا_ستون);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    خالی
update_popups(خالی (*win_update)(win_T *wp))
{
    win_T   *wp;
    صحیح	    چوٹی_off;
    صحیح	    بایاں_extra;
    صحیح	    کل_چوڑائی;
    صحیح	    کل_اونچائی_2;
    صحیح	    چوٹی_padding;
    صحیح	    popup_attr;
    صحیح	    border_attr[4];
    صحیح	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    صحیح	    صف;
    صحیح	    wincol;
    صحیح	    padcol = 0;
    صحیح	    padendcol = 0;
    صحیح	    i;
    صحیح	    sb_thumb_چوٹی = 0;
    صحیح	    sb_thumb_اونچائی = 0;
    صحیح	    attr_scroll = 0;
    صحیح	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (بنیاد_screenlines != تہی)
    {
	vim_free(بنیاد_screenlines);
	بنیاد_screenlines = تہی;
    }
    اگر (بنیاد_screenattrs != تہی)
    {
	vim_free(بنیاد_screenattrs);
	بنیاد_screenattrs = تہی;
    }
    اگر (بنیاد_screenlinesuc != تہی)
    {
	vim_free(بنیاد_screenlinesuc);
	بنیاد_screenlinesuc = تہی;
    }
    بنیاد_screen_صفیں = 0;
    بنیاد_screen_ستون_تعداد = 0;
#اگر_ختم

    جب ((wp = ڈھونڈو_اگلا_popup(TRUE, POPUP_HANDLED_5)) != تہی)
    {
	صحیح	    title_len = 0;
	صحیح	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	ورنہ
	    screen_opacity_popup = تہی;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (screen_opacity_popup != تہی)
	{
	    اگر (بنیاد_screenlines != تہی)
	    {
		vim_free(بنیاد_screenlines);
		بنیاد_screenlines = تہی;
	    }
	    اگر (بنیاد_screenattrs != تہی)
	    {
		vim_free(بنیاد_screenattrs);
		بنیاد_screenattrs = تہی;
	    }
	    اگر (بنیاد_screenlinesuc != تہی)
	    {
		vim_free(بنیاد_screenlinesuc);
		بنیاد_screenlinesuc = تہی;
	    }

	    بنیاد_screen_صفیں = screen_صفیں;
	    بنیاد_screen_ستون_تعداد = screen_Columns;
	    بنیاد_screenlines = ALLOC_MULT(schar_T,
				    بنیاد_screen_صفیں * بنیاد_screen_ستون_تعداد);
	    بنیاد_screenattrs = ALLOC_MULT(صحیح,
				    بنیاد_screen_صفیں * بنیاد_screen_ستون_تعداد);
	    اگر (enc_utf8)
		بنیاد_screenlinesuc = ALLOC_MULT(u8char_T,
				    بنیاد_screen_صفیں * بنیاد_screen_ستون_تعداد);

	    اگر (بنیاد_screenlines != تہی && بنیاد_screenattrs != تہی)
	    {
		برائے (صحیح r = 0; r < بنیاد_screen_صفیں; r++)
		{
		    صحیح off = سطر_انحراف[r];
		    صحیح بنیاد_off = r * بنیاد_screen_ستون_تعداد;
		    برائے (صحیح c = 0; c < بنیاد_screen_ستون_تعداد; c++)
		    {
			بنیاد_screenlines[بنیاد_off + c] = Screen_سطریں[off + c];
			بنیاد_screenattrs[بنیاد_off + c] = ScreenAttrs[off + c];
			اگر (enc_utf8 && بنیاد_screenlinesuc != تہی)
			    بنیاد_screenlinesuc[بنیاد_off + c] =
					Screen_سطریں_UC[off + c];
		    }
		}
	    }
	}
#اگر_ختم

	// Save background ScreenLines for padding opacity before win_update()
	// overwrites them.
	popup_saved_screen_T saved_screen = { 0 };

	popup_save_padding_screen(wp, &saved_screen);

	// Set flags in popup_transparent[] for masked cells.
	update_popup_transparent(wp, 1);

	// adjust w_winrow and w_wincol for border and padding, since
	// win_update() doesn't handle them.
	چوٹی_off = popup_چوٹی_extra(wp);
	بایاں_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	اگر (wp->w_wincol + بایاں_extra < 0)
	    بایاں_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(چوٹی_off - wp->w_popup_topoff, 0);
	wp->w_wincol += بایاں_extra;

	// Draw the popup text, unless it's off screen.
	اگر (wp->w_winrow < screen_صفیں && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    اگر (wp->w_popup_آخری_curline != wp->w_cursor.lnum)
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
	    اگر (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    ورنہ اگر (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(چوٹی_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= بایاں_extra;

	// Add offset for border and padding if not done already.
	اگر ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += بایاں_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	اگر ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += چوٹی_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	اگر (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    کل_چوڑائی = cl.eff_چوڑائی;
	ورنہ
	    کل_چوڑائی = popup_چوڑائی(wp) - wp->w_popup_rightoff;
	اگر (کل_چوڑائی < 0)
	    کل_چوڑائی = 0;
	اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    کل_اونچائی_2 = cl.eff_اونچائی;
	ورنہ
	    کل_اونچائی_2 = popup_اونچائی(wp);
	popup_attr = get_win_attr(wp);

	اگر (wp->w_winrow + کل_اونچائی_2 > cmdline_صف)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	ورنہ
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	اگر (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	ورنہ
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    برائے (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    اگر (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	برائے (i = 0; i < 8; ++i)
	    اگر (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	برائے (i = 0; i < 4; ++i)
	{
	    اگر (wp->w_border_highlight[i] != تہی)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    ورنہ اگر (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    ورنہ
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	اگر (wp->w_popup_title != تہی)
	{
	    صحیح	    title_attr;

	    اگر (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != تہی)
		title_attr = border_attr[0];
	    ورنہ اگر (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    ورنہ
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    اگر (title_len > کل_چوڑائی - 2)
	    {
		صحیح	title_byte_len = (صحیح)STRLEN(wp->w_popup_title);
		char_u  *title_متن = alloc(title_byte_len + 1);

		اگر (title_متن != تہی)
		{
		    trunc_string(wp->w_popup_title, title_متن,
					  کل_چوڑائی - 2, title_byte_len + 1);
		    screen_puts(title_متن, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_متن);
		}

		title_len = کل_چوڑائی - 2;
	    }
	    ورنہ
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	چوٹی_padding = cl.eff_padding[0];
	اگر (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    اگر (title_len > 0)
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, title_wincol,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			title_wincol + title_len, wincol + کل_چوڑائی,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    ورنہ
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + کل_چوڑائی,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + کل_چوڑائی - 1, border_attr[1]);
	    }
	}
	ورنہ اگر (cl.eff_padding[0] == 0 && popup_چوٹی_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    چوٹی_padding = 1;

	اگر (چوٹی_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + کل_چوڑائی - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    اگر (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	اگر (چوٹی_padding > 0)
	{
	    صف = wp->w_winrow + cl.eff_border[0];
	    اگر (title_len > 0 && صف == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		اگر (screen_opacity_popup != تہی && saved_screen.سطریں != تہی)
		{
		    // Left of title
		    fill_opacity_padding(صف, صف + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(صف, صف + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		ورنہ
		{
		    screen_fill(صف, صف + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(صف, صف + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		صف += 1;
		چوٹی_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    اگر (screen_opacity_popup != تہی && saved_screen.سطریں != تہی)
	    {
		fill_opacity_padding(صف, صف + چوٹی_padding, padcol, padendcol,
			&saved_screen);
	    }
	    ورنہ
	    {
		screen_fill(صف, صف + چوٹی_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	اگر (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_بفر->b_ml.ml_سطر_گنتی;
	    صحیح		اونچائی = wp->w_اونچائی;
	    صحیح		آخری;

	    sb_thumb_اونچائی = ((linenr_T)اونچائی * اونچائی + linecount / 2)
								   / linecount;
	    اگر (wp->w_topline > 1 && sb_thumb_اونچائی == اونچائی)
		--sb_thumb_اونچائی;  // scrolled, no full thumb
	    اگر (sb_thumb_اونچائی == 0)
		sb_thumb_اونچائی = 1;
	    اگر (linecount <= wp->w_اونچائی || wp->w_اونچائی == 0)
		// it just fits, avoid divide by zero
		sb_thumb_چوٹی = 0;
	    ورنہ
		sb_thumb_چوٹی = (wp->w_topline - 1
				+ (linecount / wp->w_اونچائی) / 2)
				* (wp->w_اونچائی - sb_thumb_اونچائی)
						  / (linecount - wp->w_اونچائی);
	    اگر (wp->w_topline > 1 && sb_thumb_چوٹی == 0 && اونچائی > 1)
		sb_thumb_چوٹی = 1;  // show it's scrolled
	    آخری = کل_اونچائی_2 - چوٹی_off - wp->w_popup_border[2];
	    اگر (sb_thumb_چوٹی >= آخری)
		// show at least one character
		sb_thumb_چوٹی = آخری - 1;

	    اگر (wp->w_scrollbar_highlight != تہی)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    ورنہ
		attr_scroll = highlight_attr[HLF_PSB];
	    اگر (wp->w_thumb_highlight != تہی)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    ورنہ
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	برائے (i = cl.eff_border[0]; i < کل_اونچائی_2 - cl.eff_border[2]; ++i)
	{
	    صحیح	pad_بایاں;
	    // left and right padding only needed next to the body
	    صحیح do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < کل_اونچائی_2 - cl.eff_border[2] - cl.eff_padding[2];

	    صف = wp->w_winrow + i;

	    // left border
	    اگر (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, صف, wincol, border_attr[3]);
	    }
	    اگر (do_padding && cl.eff_padding[3] > 0)
	    {
		صحیح ستون = wincol + wp->w_popup_border[3];

		// left padding
		pad_بایاں = wp->w_popup_padding[3];
		اگر (ستون < 0)
		{
		    pad_بایاں += ستون;
		    ستون = 0;
		}
		اگر (pad_بایاں > 0)
		{
		    اگر (screen_opacity_popup != تہی && saved_screen.سطریں != تہی)
			fill_opacity_padding(صف, صف + 1, ستون, ستون + pad_بایاں,
				&saved_screen);
		    ورنہ
			screen_fill(صف, صف + 1, ستون, ستون + pad_بایاں,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    اگر (wp->w_has_scrollbar)
	    {
		صحیح سطر = i - چوٹی_off;
		صحیح scroll_ستون = wp->w_wincol + کل_چوڑائی - 1
						       - wp->w_popup_border[1];

		اگر (سطر >= 0 && سطر < wp->w_اونچائی)
		    screen_putchar(' ', صف, scroll_ستون,
			    سطر >= sb_thumb_چوٹی
				       && سطر < sb_thumb_چوٹی + sb_thumb_اونچائی
						  ? attr_thumb : attr_scroll);
		ورنہ
		    screen_putchar(' ', صف, scroll_ستون, popup_attr);
	    }
	    // right border
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, صف, wincol + کل_چوڑائی - 1, border_attr[1]);
	    }
	    // right padding
	    اگر (do_padding && cl.eff_padding[1] > 0)
	    {
		صحیح pad_ستون_آغاز = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_چوڑائی;
		صحیح pad_ستون_انتہا = pad_ستون_آغاز + wp->w_popup_padding[1];

		اگر (screen_opacity_popup != تہی && saved_screen.سطریں != تہی)
		    fill_opacity_padding(صف, صف + 1, pad_ستون_آغاز, pad_ستون_انتہا,
			    &saved_screen);
		ورنہ
		    screen_fill(صف, صف + 1, pad_ستون_آغاز, pad_ستون_انتہا,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	اگر (wp->w_popup_shadow)
	{
	    صحیح ستون = wincol + کل_چوڑائی;
	    برائے (i = 0; i < کل_اونچائی_2; ++i)
	    {
		صف = wp->w_winrow + i + 1;
		put_shadow_char(صف, ستون);
		put_shadow_char(صف, ستون + 1);
	    }
	}

	اگر (cl.eff_padding[2] > 0)
	{
	    // bottom padding -- sits right after the visible content rows.
	    // Derive the row from total_height so it always lands inside the
	    // popup's drawn extent, including the corner case where the top
	    // clip consumes more rows than the content itself (so the visible
	    // content height is zero).  A formula based on
	    // w_height - clip_top_content - clip_bot_content can go negative
	    // there and would draw the padding above w_winrow.
	    صف = wp->w_winrow + کل_اونچائی_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    اگر (screen_opacity_popup != تہی && saved_screen.سطریں != تہی)
		fill_opacity_padding(صف, صف + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    ورنہ
		screen_fill(صف, صف + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	اگر (cl.eff_border[2] > 0)
	{
	    // bottom border
	    صف = wp->w_winrow + کل_اونچائی_2 - 1;
	    screen_fill(صف, صف + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + کل_چوڑائی,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, صف, wincol + کل_چوڑائی - 1, border_attr[2]);
	    }
	}

	اگر (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    صف = wp->w_winrow + کل_اونچائی_2;
	    برائے (صحیح ستون = 2 + (wincol < 0 ? 0 : wincol);
		    ستون < wincol + کل_چوڑائی; ستون++)
		put_shadow_char(صف, ستون);
	}

	اگر (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + کل_چوڑائی - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = تہی;

	// Back to the normal zindex.
	screen_zindex = 0;

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	// if this was the message window popup may start the timer now
	may_آغاز_پیغام_win_timer(wp);
#اگر_ختم

	اگر (override_success)
	    نکالو_highlight_overrides();

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
	popup_position_image(wp);
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    VIM_CLEAR(بنیاد_screenlines);
    VIM_CLEAR(بنیاد_screenattrs);
    VIM_CLEAR(بنیاد_screenlinesuc);
    بنیاد_screen_صفیں = 0;
    بنیاد_screen_ستون_تعداد = 0;
#اگر_ختم

#اگر defined(FEAT_تلاش_EXTRA)
    // In case win_update() called start_search_hl().
    انتہا_تلاش_hl();
#اگر_ختم
}

/*
 * Mark references in callbacks of one popup window.
 */
    ساکن صحیح
سیٹ_ref_اندر_one_popup(win_T *wp, صحیح copyID)
{
    صحیح		abort = FALSE;
    typval_T	tv;

    اگر (wp->w_close_cb.cb_partial != تہی)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
    }
    اگر (wp->w_filter_cb.cb_partial != تہی)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
    }
    abort = abort || سیٹ_ref_اندر_list(wp->w_popup_نقاب, copyID);
    واپس abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    صحیح
سیٹ_ref_اندر_popups(صحیح copyID)
{
    صحیح		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    برائے (wp = پہلا_popupwin; !abort && wp != تہی; wp = wp->w_اگلا)
	abort = abort || سیٹ_ref_اندر_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	برائے (wp = tp->tp_پہلا_popupwin; !abort && wp != تہی; wp = wp->w_اگلا)
	    abort = abort || سیٹ_ref_اندر_one_popup(wp, copyID);
	اگر (abort)
	    توڑو;
    }
    واپس abort;
}

    صحیح
popup_is_popup(win_T *wp)
{
    واپس wp->w_popup_flags != 0;
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ڈھونڈو_preview_کھڑکی(خالی)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_p_pvw)
	    واپس wp;
    واپس تہی;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ڈھونڈو_info_کھڑکی(خالی)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_popup_flags & POPF_INFO)
	    واپس wp;
    واپس تہی;
}
#اگر_ختم

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(صحیح dir, bool by_صفحہ)
{
#اگر_تعریف_شدہ FEAT_QUICKFIX
    win_T	*wp = popup_ڈھونڈو_info_کھڑکی();
    صحیح		by;
    linenr_T	new_topline;

    اگر (wp == تہی)
	واپس false;

    by = by_صفحہ ? (wp->w_اونچائی > 2 ? wp->w_اونچائی - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    اگر (new_topline < 1)
	new_topline = 1;
    اگر (new_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	new_topline = wp->w_بفر->b_ml.ml_سطر_گنتی;
    اگر (new_topline != wp->w_topline)
    {
	سیٹ_topline(wp, new_topline);
	popup_سیٹ_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    واپس true;
#ورنہ
    واپس false;
#اگر_ختم
}

    خالی
f_popup_findecho(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
    rettv->vval.v_نمبر = پیغام_win_2 == تہی ? 0 : پیغام_win_2->w_id;
#ورنہ
    rettv->vval.v_نمبر = 0;
#اگر_ختم
}

    خالی
f_popup_findinfo(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_تعریف_شدہ FEAT_QUICKFIX
    win_T   *wp = popup_ڈھونڈو_info_کھڑکی();

    rettv->vval.v_نمبر = wp == تہی ? 0 : wp->w_id;
#ورنہ
    rettv->vval.v_نمبر = 0;
#اگر_ختم
}

    خالی
f_popup_findpreview(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
#اگر_تعریف_شدہ FEAT_QUICKFIX
    win_T   *wp = popup_ڈھونڈو_preview_کھڑکی();

    rettv->vval.v_نمبر = wp == تہی ? 0 : wp->w_id;
#ورنہ
    rettv->vval.v_نمبر = 0;
#اگر_ختم
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    صحیح
popup_create_preview_کھڑکی(صحیح info)
{
    win_T *wp = popup_create(تہی, تہی, info ? TYPE_INFO : TYPE_PREVIEW);

    اگر (wp == تہی)
	واپس FAIL;
    اگر (info)
	wp->w_popup_flags |= POPF_INFO;
    ورنہ
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    اگر (wp->w_minwidth > 0)
	wp->w_چوڑائی = wp->w_minwidth;
    ورنہ اگر (wp->w_maxwidth > 0)
	wp->w_چوڑائی = wp->w_maxwidth;
    ورنہ
	wp->w_چوڑائی = curwin->w_چوڑائی;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_بفر->b_locked = FALSE;

    win_enter(wp, FALSE);
    واپس OK;
}

/*
 * Close any preview popup.
 */
    خالی
popup_close_preview(خالی)
{
    win_T *wp = popup_ڈھونڈو_preview_کھڑکی();

    اگر (wp != تہی)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    خالی
popup_hide_info(خالی)
{
    win_T *wp = popup_ڈھونڈو_info_کھڑکی();

    اگر (wp != تہی)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	اگر (State & منوال_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    خالی
popup_close_info(خالی)
{
    win_T *wp = popup_ڈھونڈو_info_کھڑکی();

    اگر (wp != تہی)
	popup_close_with_retval(wp, -1);
}
#اگر_ختم

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    صحیح
popup_overlaps_cmdline(خالی)
{
    واپس popup_on_cmdline;
}

#اگر defined(HAS_پیغام_کھڑکی)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_پیغام_win(خالی)
{
    اگر (پیغام_win_2 != تہی)
	واپس پیغام_win_2;

    صحیح i;

    پیغام_win_2 = popup_create(تہی, تہی, TYPE_پیغام_WIN);

    اگر (پیغام_win_2 == تہی)
	واپس تہی;

    // use the full screen width
    پیغام_win_2->w_چوڑائی = Columns;

    // position at bottom of screen
    پیغام_win_2->w_popup_pos = POPPOS_BOTTOM;
    پیغام_win_2->w_wantcol = 1;
    پیغام_win_2->w_minwidth = 9999;
    پیغام_win_2->w_firstline = -1;

    // no padding, border at the top
    برائے (i = 0; i < 4; ++i)
	پیغام_win_2->w_popup_padding[i] = 0;
    برائے (i = 1; i < 4; ++i)
	پیغام_win_2->w_popup_border[i] = 0;

    اگر (پیغام_win_2->w_popup_timer != تہی)
	پیغام_win_2->w_popup_timer->tr_keep = TRUE;
    واپس پیغام_win_2;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    خالی
popup_show_پیغام_win(خالی)
{
    اگر (پیغام_win_2 == تہی)
	واپس;

    اگر ((پیغام_win_2->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(پیغام_win_2, TYPE_پیغام_WIN);
	popup_show(پیغام_win_2);
    }
    آغاز_پیغام_win_timer = TRUE;
}

    ساکن خالی
may_آغاز_پیغام_win_timer(win_T *wp)
{
    اگر (wp == پیغام_win_2 && آغاز_پیغام_win_timer)
    {
	اگر (پیغام_win_2->w_popup_timer != تہی)
	{
	    پیغام_win_2->w_popup_timer->tr_interval = پیغام_win_وقت;
	    timer_آغاز(پیغام_win_2->w_popup_timer);
	    پیغام_win_وقت = 3000;
	}
	آغاز_پیغام_win_timer = FALSE;
    }
}

    صحیح
popup_پیغام_win_visible(خالی)
{
    واپس پیغام_win_2 != تہی
	&& (پیغام_win_2->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    خالی
popup_hide_پیغام_win(خالی)
{
    اگر (پیغام_win_2 != تہی)
	popup_hide(پیغام_win_2);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ساکن صحیح save_پیغام_didout = FALSE;
ساکن صحیح save_پیغام_ستون = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ساکن صحیح ew_پیغام_didout = FALSE;
ساکن صحیح ew_پیغام_ستون = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    خالی
آغاز_echowindow(صحیح وقت_sec)
{
    اندر_echowindow = TRUE;
    save_پیغام_didout = پیغام_didout;
    save_پیغام_ستون = پیغام_ستون;
    پیغام_didout = ew_پیغام_didout;
    پیغام_ستون = ew_پیغام_ستون;
    اگر (وقت_sec != 0)
	پیغام_win_وقت = وقت_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    خالی
انتہا_echowindow(خالی)
{
    اندر_echowindow = FALSE;

    اگر ((State & منوال_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_پیغام_didout = TRUE;
    ew_پیغام_ستون = پیغام_ستون == 0 ? 1 : پیغام_ستون;
    پیغام_didout = save_پیغام_didout;
    پیغام_ستون = save_پیغام_ستون;
}
#اگر_ختم

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    صحیح
popup_win_closed(win_T *win)
{
    صحیح	    باری;
    win_T   *wp;
    win_T   *اگلا;
    صحیح	    ret = FALSE;

    برائے (باری = 1; باری <= 2; ++باری)
	برائے (wp = باری == 1 ? پہلا_popupwin : curtab->tp_پہلا_popupwin;
							 wp != تہی; wp = اگلا)
	{
	    اگلا = wp->w_اگلا;
	    اگر (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    واپس ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    خالی
popup_سیٹ_title(win_T *wp)
{
    اگر (wp->w_بفر->b_fname == تہی)
	واپس;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_بفر, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_بفر->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    اگر (wp->w_popup_title != تہی)
	vim_snprintf((حرف *)wp->w_popup_title, len, " %s ",
		wp->w_بفر->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#اگر defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    خالی
popup_update_preview_title(خالی)
{
    win_T *wp = popup_ڈھونڈو_preview_کھڑکی();

    اگر (wp != تہی)
	popup_سیٹ_title(wp);
}
#اگر_ختم

#اگر_ختم // FEAT_PROP_POPUP
