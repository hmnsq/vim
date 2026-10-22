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
} poppos_اندراج_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
تعريفنوع بنية {
    صحيح		سرگرم;
    صحيح		winrow;
    صحيح		wincol;
    صحيح		اچائی;
    صحيح		چوڑائی;
    صحيح		leftoff;
    صحيح		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
تعريفنوع بنية {
    schar_T	*سطریں;
    صحيح		*attrs;
    u8char_T	*linesuc;
    صحيح		شروع_صف;
    صحيح		شروع_خانہ;
    صحيح		صفاں_2;
    صحيح		خانے_گنتی;
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
    صحيح		چوڑائی;
    صحيح		اچائی;
    صحيح		leftcol;
    صحيح		leftoff;
    صحيح		has_scrollbar;
    صحيح		topoff;
    صحيح		bottomoff;
    صحيح		leftclip;
    صحيح		rightclip;
} popup_layout_T;

ساكن poppos_اندراج_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#جے_تعریف_شدہ HAS_سنیہا_کھڑکی
// Window used for ":echowindow"
ساكن win_T *سنیہا_win = عدم;

// Time used for the next ":echowindow" message in msec.
ساكن صحيح  سنیہا_win_ویلا = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ساكن صحيح    شروع_سنیہا_win_timer = FALSE;

ساكن فراغ may_شروع_سنیہا_win_timer(win_T *wp);
#جے_انت

ساكن صحيح popup_on_cmdline = FALSE;

ساكن فراغ popup_adjust_position(win_T *wp);
ساكن bool popup_area_بدلیا(win_T *wp, popup_area_T *area);
ساكن فراغ popup_redraw_exposed_area(popup_area_T *area);
ساكن فراغ popup_save_area(win_T *wp, popup_area_T *area);
ساكن فراغ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ساكن فراغ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ساكن bool popup_layout_بدلیا(win_T *wp, popup_layout_T *layout);
ساكن bool popup_style_بدلیا(win_T *wp, popup_style_snapshot_T *style);
ساكن فراغ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ساكن فراغ popup_save_layout(win_T *wp, popup_layout_T *layout);
ساكن فراغ redraw_under_popup_area(صحيح winrow, صحيح wincol, صحيح اچائی,
	صحيح چوڑائی, صحيح leftoff);
ساكن فراغ redraw_overlapped_opacity_popups(صحيح winrow, صحيح wincol,
	صحيح اچائی, صحيح چوڑائی, صحيح leftoff, صحيح zindex);
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

    di = dict_لبھو(dict, کلید, -1);
    إذا (di == عدم)
	أرجع MAXCOL;

    val = tv_get_string(&di->di_tv);
    إذا (STRNCMP(val, "cursor", 6) != 0)
	أرجع dict_get_نمبر_check(dict, کلید);

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
سیٹ_padding_border(dict_T *dict, صحيح *array, حرف *name, صحيح max_val)
{
    dictitem_T	*di;

    di = dict_لبھو(dict, (char_u *)name, -1);
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
    لكل (i = 0, li = list->lv_پہلا; i < 4 && i < list->lv_len;
	    ++i, li = li->li_اگلا)
    {
	nr = (صحيح)tv_get_نمبر(&li->li_tv);
	إذا (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    أرجع OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ساكن فراغ
سیٹ_moved_اقدار(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.خانہ;
    wp->w_popup_maxcol = curwin->w_cursor.خانہ;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساكن فراغ
سیٹ_moved_columns(win_T *wp, صحيح flags)
{
    char_u	*ptr;
    صحيح		len = لبھو_ident_under_cursor(&ptr, flags | لبھو_NOERROR);
    صحيح		mincol;

    إذا (len <= 0)
	أرجع;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (صحيح)(ptr - ml_get_curline());
    إذا (curwin->w_cursor.خانہ < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.خانہ;
	wp->w_popup_maxcol = curwin->w_cursor.خانہ;
	أرجع;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ساكن فراغ
سیٹ_mousemoved_اقدار(win_T *wp)
{
    wp->w_popup_mouse_صف = mouse_صف;
    wp->w_popup_mouse_mincol = mouse_خانہ;
    wp->w_popup_mouse_maxcol = mouse_خانہ;
}

    ساكن فراغ
update_popup_ورتدا_mouse_move(فراغ)
{
    popup_ورتدا_mouse_move = FALSE;
    إذا (!popup_visible)
	أرجع;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_popup_mouse_صف != 0)
	{
	    popup_ورتدا_mouse_move = TRUE;
	    أرجع;
	}
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_popup_mouse_صف != 0)
	{
	    popup_ورتدا_mouse_move = TRUE;
	    أرجع;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساكن فراغ
سیٹ_mousemoved_columns(win_T *wp, صحيح flags)
{
    win_T	*textwp;
    char_u	*متن;
    صحيح		خانہ;
    pos_T	pos;
    colnr_T	mcol;

    إذا (لبھو_لفظ_under_cursor(mouse_صف, mouse_خانہ, TRUE, flags,
				  &textwp, &pos.lnum, &متن, عدم, &خانہ) != OK)
	أرجع;

    // convert text column to mouse column
    pos.خانہ = خانہ;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, عدم, عدم, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.خانہ = خانہ + (colnr_T)STRLEN(متن) - 1;
    getvcol(textwp, &pos, عدم, عدم, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(متن);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    صحيح
popup_on_border(win_T *wp, صحيح صف, صحيح خانہ)
{
    أرجع (صف == 0 && wp->w_popup_border[0] > 0)
	    || (صف == popup_اچائی(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (خانہ == 0 && wp->w_popup_border[3] > 0)
	    || (خانہ == popup_چوڑائی(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    صحيح
popup_close_if_on_X(win_T *wp, صحيح صف, صحيح خانہ)
{
    إذا (wp->w_popup_close == POPCLOSE_BUTTON
	    && صف == 0 && خانہ == popup_چوڑائی(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	أرجع TRUE;
    }
    أرجع FALSE;
}

// Values set when dragging a popup window starts.
ساكن صحيح drag_شروع_صف;
ساكن صحيح drag_شروع_خانہ;
ساكن صحيح drag_شروع_wantline;
ساكن صحيح drag_شروع_wantcol;
ساكن صحيح drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    فراغ
popup_شروع_drag(win_T *wp, صحيح صف, صحيح خانہ)
{
    drag_شروع_صف = mouse_صف;
    drag_شروع_خانہ = mouse_خانہ;
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
			    && صف == popup_اچائی(wp) - 1
			    && خانہ == popup_چوڑائی(wp) - 1;

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
	صحيح چوڑائی_inc = mouse_خانہ - drag_شروع_خانہ;
	صحيح اچائی_inc = mouse_صف - drag_شروع_صف;

	إذا (چوڑائی_inc != 0)
	{
	    صحيح چوڑائی = wp->w_چوڑائی + چوڑائی_inc;

	    إذا (چوڑائی < 1)
		چوڑائی = 1;
	    wp->w_minwidth = چوڑائی;
	    wp->w_maxwidth = چوڑائی;
	    drag_شروع_خانہ = mouse_خانہ;
	}

	إذا (اچائی_inc != 0)
	{
	    صحيح اچائی = wp->w_اچائی + اچائی_inc;

	    إذا (اچائی < 1)
		اچائی = 1;
	    wp->w_minheight = اچائی;
	    wp->w_maxheight = اچائی;
	    drag_شروع_صف = mouse_صف;
	}

	popup_adjust_position(wp);
	أرجع;
    }

    إذا (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	أرجع;
    wp->w_wantline = drag_شروع_wantline + (mouse_صف - drag_شروع_صف);
    إذا (wp->w_wantline < 1)
	wp->w_wantline = 1;
    إذا (wp->w_wantline > صفاں)
	wp->w_wantline = صفاں;
    wp->w_wantcol = drag_شروع_wantcol + (mouse_خانہ - drag_شروع_خانہ);
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
popup_سیٹ_firstline(win_T *wp)
{
    صحيح	    اچائی = wp->w_اچائی;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    طالما (wp->w_firstline > 1 && wp->w_اچائی < اچائی)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    صحيح
popup_is_وچ_scrollbar(win_T *wp, صحيح صف, صحيح خانہ)
{
    أرجع wp->w_has_scrollbar
	&& صف >= wp->w_popup_border[0]
	&& صف < popup_اچائی(wp) - wp->w_popup_border[2]
	&& خانہ == popup_چوڑائی(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    فراغ
popup_handle_scrollbar_click(win_T *wp, صحيح صف, صحيح خانہ)
{
    إذا (!popup_is_وچ_scrollbar(wp, صف, خانہ))
	أرجع;

    صحيح	    اچائی = popup_اچائی(wp);
    صحيح	    new_topline = wp->w_topline;

    إذا (صف >= اچائی / 2)
    {
	// Click in lower half, scroll down.
	إذا (wp->w_topline < wp->w_بفر->b_ml.ml_سطر_گنتی)
	    ++new_topline;
    }
    وإلا إذا (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    إذا (new_topline == wp->w_topline)
	أرجع;

    سیٹ_topline(wp, new_topline);
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
    popup_سیٹ_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#إذا defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ساكن فراغ
popup_جوڑو_timeout(win_T *wp, صحيح ویلا, صحيح close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((حرف *)cbbuf, حجم(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    إذا (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	أرجع;

    wp->w_popup_timer = create_timer(ویلا, 0);
    callback_T cb = get_callback(&tv);
    إذا (cb.cb_name != عدم && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#جے_انت

    ساكن poppos_T
get_pos_اندراج(dict_T *d, صحيح give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    صحيح	    nr;

    إذا (str == عدم)
	أرجع POPPOS_NONE;

    لكل (nr = 0; nr < (صحيح)ARRAY_لمائی(poppos_entries); ++nr)
	إذا (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    أرجع poppos_entries[nr].pp_val;

    إذا (give_error)
	semsg(_(e_invalid_دلیل_str), str);
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

    إذا ((nr = dict_get_نمبر_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    إذا ((nr = dict_get_نمبر_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    إذا ((nr = dict_get_نمبر_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    إذا ((nr = dict_get_نمبر_def(d, "maxheight", -1)) >= 0)
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
	poppos_T ppt = get_pos_اندراج(d, TRUE);

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
	    di = dict_لبھو(d, (char_u *)"textpropwin", -1);
	    إذا (di != عدم)
	    {
		wp->w_popup_prop_win = لبھو_win_by_nr_or_id(&di->di_tv);
		إذا (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = لبھو_prop_type_id(str, wp->w_popup_prop_win->w_بفر);
	    إذا (nr <= 0)
		nr = لبھو_prop_type_id(str, عدم);
	    إذا (nr <= 0)
		semsg(_(e_invalid_دلیل_str), str);
	    وإلا
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_لبھو(d, (char_u *)"textpropid", -1);
    إذا (di != عدم)
	wp->w_popup_prop_id = dict_get_نمبر(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ساكن فراغ
handle_moved_دلیل(win_T *wp, dictitem_T *di, صحيح mousemoved)
{
    إذا (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != عدم)
    {
	char_u  *s = di->di_tv.vval.v_string;
	صحيح	flags = 0;

	إذا (STRCMP(s, "word") == 0)
	    flags = لبھو_IDENT | لبھو_STRING;
	وإلا إذا (STRCMP(s, "WORD") == 0)
	    flags = لبھو_STRING;
	وإلا إذا (STRCMP(s, "expr") == 0)
	    flags = لبھو_IDENT | لبھو_STRING | لبھو_EVAL;
	وإلا إذا (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_دلیل_str), s);
	إذا (flags != 0)
	{
	    إذا (mousemoved)
		سیٹ_mousemoved_columns(wp, flags);
	    وإلا
		سیٹ_moved_columns(wp, flags);
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
	li = l->lv_پہلا;
	إذا (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_نمبر(&l->lv_پہلا->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    إذا (mousemoved)
		wp->w_popup_mouse_صف = nr;
	    وإلا
		wp->w_popup_lnum = nr;
	    li = li->li_اگلا;
	    إذا (nr == 0)
		wp->w_popup_curwin = عدم;
	}

	mincol = tv_get_نمبر(&li->li_tv);
	maxcol = tv_get_نمبر(&li->li_اگلا->li_tv);
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
	semsg(_(e_invalid_دلیل_str), tv_get_string(&di->di_tv));
}

    ساكن فراغ
check_highlight(dict_T *dict, حرف *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_لبھو(dict, (char_u *)name, -1);
    إذا (di == عدم)
	أرجع;

    إذا (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_قدر_for_دلیل_str), name);
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
	wp->w_topline = wp->w_cursor.lnum - wp->w_اچائی + 1;
	إذا (wp->w_topline < 1)
	    wp->w_topline = 1;
	وإلا إذا (wp->w_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_topline = wp->w_بفر->b_ml.ml_سطر_گنتی;
	طالما (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_بفر->b_ml.ml_سطر_گنتی
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_اچائی + 1) > wp->w_اچائی)
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

    buf_delete_signs(wp->w_بفر, (char_u *)"PopUpMenu");

    إذا ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	إذا (!sign_exists_by_name(sign_name))
	{
	    حرف *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, عدم, (char_u *)linehl, عدم, عدم, عدم,
		    عدم, SIGN_DEF_PRIO);
	}

	sign_رکھو(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_بفر, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    وإلا
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_آخری_curline = wp->w_cursor.lnum;
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

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#جے_انت

    // TODO: flip

    di = dict_لبھو(dict, (char_u *)"firstline", -1);
    إذا (di != عدم)
    {
	wp->w_firstline = dict_get_نمبر(dict, "firstline");
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

    di = dict_لبھو(dict, (char_u *)"opacity", -1);
    إذا (di != عدم)
    {
	nr = dict_get_نمبر(dict, "opacity");
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

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    إذا (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    وإلا
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_لبھو(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    إذا (di != عدم && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != عدم)
    {
	image_T		    *img;
	image_T		    *cur_img = عدم;
	image_placement_T   *رکھو;
	صحيح		    cw, ch;

	إذا (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    إذا (wp->w_popup_imagep != عدم)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = عدم;
	    }
	    اذهب image_پورا;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	إذا (wp->w_popup_imagep != عدم && wp->w_popup_imagep->img != عدم)
	    cur_img = wp->w_popup_imagep->img;

	img = جوڑو_image(di->di_tv.vval.v_dict, cur_img, true);
	إذا (img == عدم)
	    أرجع FAIL;

	إذا (img == cur_img)
	    اذهب image_پورا;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	رکھو = image_placement_new(img, wp->w_id, iplace_use, false);

	إذا (رکھو == عدم)
	{
	    image_unref(img);
	    أرجع FAIL;
	}

	إذا (wp->w_popup_imagep != عدم)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = رکھو;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	إذا (dict_لبھو(dict, (char_u *)"minwidth", -1) == عدم)
	    wp->w_minwidth = cw;
	إذا (dict_لبھو(dict, (char_u *)"maxwidth", -1) == عدم)
	    wp->w_maxwidth = cw;
	إذا (dict_لبھو(dict, (char_u *)"minheight", -1) == عدم)
	    wp->w_minheight = ch;
	إذا (dict_لبھو(dict, (char_u *)"maxheight", -1) == عدم)
	    wp->w_maxheight = ch;
image_پورا:
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
	    image_placement_سیٹ_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_اچائی, wp->w_چوڑائی);
    }
#جے_انت

    di = dict_لبھو(dict, (char_u *)"close", -1);
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
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    إذا (str != عدم)
    {
	حرف *errmsg = update_wincolor(wp, str);

	إذا (errmsg == عدم)
	    سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وإلا
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    إذا (str != عدم)
    {
	حرف *errmsg = update_winhighlight(wp, str);

	إذا (errmsg == عدم)
	    سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وإلا
	    emsg(_(errmsg));
    }

    إذا (سیٹ_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	سیٹ_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	أرجع FAIL;

    di = dict_لبھو(dict, (char_u *)"borderhighlight", -1);
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
		لكل (i = 0, li = list->lv_پہلا; i < 4 && i < list->lv_len;
						     ++i, li = li->li_اگلا)
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

    di = dict_لبھو(dict, (char_u *)"borderchars", -1);
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
		لكل (i = 0, li = list->lv_پہلا; i < 8 && i < list->lv_len;
							 ++i, li = li->li_اگلا)
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

    di = dict_لبھو(dict, (char_u *)"zindex", -1);
    إذا (di != عدم)
    {
	wp->w_zindex = dict_get_نمبر(dict, "zindex");
	إذا (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	إذا (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_لبھو(dict, (char_u *)"mask", -1);
    إذا (di != عدم)
    {
	صحيح ok = FALSE;

	إذا (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != عدم)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_اشیاء(di->di_tv.vval.v_list, li)
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
	    wp->w_popup_نقاب = di->di_tv.vval.v_list;
	    ++wp->w_popup_نقاب->lv_refcount;
	    VIM_CLEAR(wp->w_popup_نقاب_cells);
	}
	وإلا
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), "mask");
	    أرجع FAIL;
	}
    }

#إذا defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_نمبر(dict, "time");
    إذا (nr > 0)
	popup_جوڑو_timeout(wp, nr, TRUE);
#جے_انت

    di = dict_لبھو(dict, (char_u *)"moved", -1);
    إذا (di != عدم)
    {
	سیٹ_moved_اقدار(wp);
	handle_moved_دلیل(wp, di, FALSE);
    }

    di = dict_لبھو(dict, (char_u *)"mousemoved", -1);
    إذا (di != عدم)
    {
	سیٹ_mousemoved_اقدار(wp);
	handle_moved_دلیل(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    إذا (nr != -1)
    {
	إذا (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	وإلا
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_لبھو(dict, (char_u *)"filter", -1);
    إذا (di != عدم)
    {
	callback_T	callback = get_callback(&di->di_tv);

	إذا (callback.cb_name != عدم)
	{
	    free_callback(&wp->w_filter_cb);
	    سیٹ_callback(&wp->w_filter_cb, &callback);
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
	    wp->w_filter_منوال = منوال_ALL;
	وإلا
	    wp->w_filter_منوال = منوال_str2flags(str);
    }

    di = dict_لبھو(dict, (char_u *)"callback", -1);
    إذا (di == عدم)
	أرجع OK;

    callback_T	callback = get_callback(&di->di_tv);
    إذا (callback.cb_name == عدم)
	أرجع OK;

    free_callback(&wp->w_close_cb);
    سیٹ_callback(&wp->w_close_cb, &callback);
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
	سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"signcolumn", -1,
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
	إذا (wp->w_firstline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_cursor.lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
	وإلا إذا (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_اچائی)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    إذا (create)
	popup_نقاب_refresh = TRUE;
    popup_highlight_curline(wp);

    أرجع OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ساكن فراغ
جوڑو_popup_لڑیاں(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_اشیاء(l, li)
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
	إذا (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_دلیل_1_list_شے_nr_dictionary_required), lnum + 1);
	    أرجع;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == عدم ? عدم : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == عدم ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    لكل (li = l->lv_پہلا; li != عدم; li = li->li_اگلا, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_لبھو(dict, (char_u *)"props", -1);
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
		FOR_ALL_LIST_اشیاء(plist, pli)
		{
		    إذا (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			أرجع;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    إذا (dict != عدم)
		    {
			صحيح خانہ = dict_get_نمبر(dict, "col");

			prop_جوڑو_common(lnum, خانہ, dict, buf, عدم);
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
popup_چوٹی_extra(win_T *wp)
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
popup_بایاں_extra(win_T *wp)
{
    أرجع wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    صحيح
popup_اچائی(win_T *wp)
{
    أرجع wp->w_اچائی
	    + popup_چوٹی_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    صحيح
popup_چوڑائی(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    أرجع wp->w_چوڑائی
	    + popup_extra_چوڑائی(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    صحيح
popup_extra_چوڑائی(win_T *wp)
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
    صحيح چوٹی_extra;
    صحيح bot_extra;
    صحيح بایاں_extra;
    صحيح دایاں_extra;

    صحيح clip_چوٹی_content;
    صحيح clip_bot_content;
    صحيح clip_بایاں_content;
    صحيح clip_دایاں_content;

    صحيح eff_چوٹی_extra;
    صحيح eff_bot_extra;
    صحيح eff_بایاں_extra;
    صحيح eff_دایاں_extra;

    صحيح eff_border[4];
    صحيح eff_padding[4];

    صحيح eff_اچائی;
    صحيح eff_چوڑائی;
} popup_clip_T;

    ساكن فراغ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    صحيح h, w;

    cl->چوٹی_extra = popup_چوٹی_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->دایاں_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_چوٹی_content = wp->w_popup_topoff - cl->چوٹی_extra;
    إذا (cl->clip_چوٹی_content < 0)
	cl->clip_چوٹی_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    إذا (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_بایاں_content = wp->w_popup_leftclip - cl->بایاں_extra;
    إذا (cl->clip_بایاں_content < 0)
	cl->clip_بایاں_content = 0;
    cl->clip_دایاں_content = wp->w_popup_rightclip - cl->دایاں_extra;
    إذا (cl->clip_دایاں_content < 0)
	cl->clip_دایاں_content = 0;

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
	صحيح excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_اچائی;
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
	صحيح excess = wp->w_popup_topoff - cl->چوٹی_extra - wp->w_اچائی;
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

    cl->eff_چوٹی_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_بایاں_extra = wp->w_popup_leftclip > 0 ? 0 : cl->بایاں_extra;
    cl->eff_دایاں_extra = wp->w_popup_rightclip > 0 ? 0 : cl->دایاں_extra;

    h = wp->w_اچائی - cl->clip_چوٹی_content - cl->clip_bot_content;
    إذا (h < 0)
	h = 0;
    cl->eff_اچائی = cl->eff_چوٹی_extra + h + cl->eff_bot_extra;

    w = wp->w_چوڑائی - cl->clip_بایاں_content - cl->clip_دایاں_content;
    إذا (w < 0)
	w = 0;
    cl->eff_چوڑائی = cl->eff_بایاں_extra + w + cl->eff_دایاں_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
تعريفنوع بنية {
    صحيح		اچائی;
    صحيح		چوڑائی;
    صحيح		winrow;
    صحيح		wincol;
    صحيح		leftcol;
    صحيح		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ساكن فراغ
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->اچائی  = wp->w_اچائی;
    sv->چوڑائی   = wp->w_چوڑائی;
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
    wp->w_چوڑائی   = sv->چوڑائی;
    wp->w_اچائی  = sv->اچائی;
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
popup_screenpos_above_چوٹی(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	صحيح	    *screen_صف,
	صحيح	    *screen_scol,
	صحيح	    *screen_ccol,
	صحيح	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    صحيح	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (صحيح)win_خانہ_off(prop_win) - (صحيح)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (صحيح)scol + coloff;
    *screen_ccol = (صحيح)ccol + coloff;
    *screen_ecol = (صحيح)ecol + coloff;
    *screen_صف = prop_win->w_winrow + 1
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
popup_لبھو_prop_above_چوٹی(
	win_T	    *wp,
	win_T	    *prop_win,
	صحيح	    max_اپڑ,
	textprop_T  *prop,
	linenr_T    *لبھیا_lnum)
{
    linenr_T	پہلا;

    إذا (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	أرجع false;

    پہلا = max_اپڑ > 0 ? prop_win->w_topline - max_اپڑ : 1;
    إذا (پہلا < 1)
	پہلا = 1;
    أرجع لبھو_prop_وچ_سطریں(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, لبھیا_lnum, پہلا, prop_win->w_topline - 1);
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
    صحيح	    popup_چوٹی, popup_bottom, popup_بایاں, popup_دایاں;
    صحيح	    کل_h, کل_w;

    إذا (cw == عدم)
	أرجع false;

    extra_h = popup_چوٹی_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_چوڑائی(wp);

    popup_چوٹی = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_اچائی + extra_h;
    popup_بایاں = wp->w_wincol;
    popup_دایاں = wp->w_wincol + wp->w_چوڑائی + extra_w;
    کل_h = wp->w_اچائی + extra_h;
    کل_w = wp->w_چوڑائی + extra_w;

    إذا (popup_چوٹی < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_چوٹی;
    إذا (popup_bottom > cw->w_winrow + cw->w_اچائی)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_اچائی);
    إذا (popup_بایاں < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_بایاں;
    إذا (popup_دایاں > cw->w_wincol + cw->w_چوڑائی)
	wp->w_popup_rightclip = popup_دایاں - (cw->w_wincol + cw->w_چوڑائی);

    أرجع wp->w_popup_topoff >= کل_h
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
    ساكن فراغ
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_اچائی -= cl->clip_چوٹی_content + cl->clip_bot_content;
	إذا (wp->w_اچائی < 0)
	    wp->w_اچائی = 0;
	إذا (cl->clip_چوٹی_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_چوٹی_content;
    }
    إذا (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	إذا (cl->clip_بایاں_content > 0 || cl->clip_دایاں_content > 0)
	    wp->w_p_wrap = FALSE;
	إذا (cl->clip_دایاں_content > 0)
	{
	    wp->w_چوڑائی -= cl->clip_دایاں_content;
	    إذا (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
	إذا (cl->clip_بایاں_content > 0)
	{
	    wp->w_leftcol += cl->clip_بایاں_content;
	    wp->w_wincol += cl->clip_بایاں_content;
	    wp->w_چوڑائی -= cl->clip_بایاں_content;
	    إذا (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
    }
}

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ساكن فراغ
popup_position_image(win_T *wp)
{
    linenr_T	    صف;
    colnr_T	    خانہ;

    صحيح iw, ih;

    // Visible cell width and height of popup itself (not image!)
    صحيح	visible_چوڑائی;
    صحيح	visible_اچائی;

    // Crop dimensions of image (in pixels).
    صحيح	crop_x, crop_y, crop_چوڑائی, crop_اچائی;

    popup_clip_T clip;

    إذا (wp->w_popup_imagep == عدم || wp->w_popup_flags & POPF_HIDDEN)
	أرجع;

    image_placement_سیٹ_enable(wp->w_popup_imagep, true);

    صف = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    خانہ = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    إذا (wp->w_popup_imagep->img == عدم)
	// Just set the bounding box
	اذهب باہر_نکلو;

    visible_چوڑائی =
	wp->w_چوڑائی - clip.clip_بایاں_content - clip.clip_دایاں_content;
    visible_اچائی =
	wp->w_اچائی - clip.clip_چوٹی_content - clip.clip_bot_content;

    إذا (visible_چوڑائی < 0 || visible_اچائی < 0)
	أرجع;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_بایاں_content;
    crop_y = clip.clip_چوٹی_content;
    crop_چوڑائی = iw - crop_x - clip.clip_دایاں_content;
    crop_اچائی = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    صف -= wp->w_popup_border[0] - clip.eff_border[0];
    صف -= wp->w_popup_padding[0] - clip.eff_padding[0];

    خانہ -= wp->w_popup_border[3] - clip.eff_border[3];
    خانہ -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    إذا (crop_چوڑائی > visible_چوڑائی)
	crop_چوڑائی = visible_چوڑائی;
    إذا (crop_اچائی > visible_اچائی)
	crop_اچائی = visible_اچائی;

    image_placement_سیٹ_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_چوڑائی, crop_اچائی);
باہر_نکلو:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_سیٹ_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_سیٹ_position(wp->w_popup_imagep, صف, خانہ);
    image_placement_سیٹ_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_اچائی, clip.eff_چوڑائی);
}
#جے_انت


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
    صحيح		چوڑائی_with_scrollbar = 0;
    صحيح		ورتیا_maxwidth = FALSE;
    صحيح		margin_چوڑائی = 0;
    صحيح		maxspace;
    صحيح		center_vert = FALSE;
    صحيح		center_hor = FALSE;
    صحيح		allow_adjust_بایاں = !wp->w_popup_fixed;
    صحيح		چوٹی_extra = popup_چوٹی_extra(wp);
    صحيح		دایاں_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    صحيح		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    صحيح		بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    صحيح		extra_اچائی = چوٹی_extra + bot_extra;
    صحيح		extra_چوڑائی = بایاں_extra + دایاں_extra;
    صحيح		w_اچائی_before_حد_بندی;
    popup_layout_T org_layout;
    صحيح		minwidth, minheight;
    صحيح		maxheight = صفاں;
    صحيح		wantline = wp->w_wantline;  // adjusted for textprop
    صحيح		wantcol = wp->w_wantcol;    // adjusted for textprop
    صحيح		use_wantcol = wantcol != 0;
    صحيح		adjust_اچائی_for_چوٹی_aligned = FALSE;

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
    إذا (wp->w_popup_آخری_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    إذا (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	صحيح	    screen_صف;
	صحيح	    screen_scol;
	صحيح	    screen_ccol;
	صحيح	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_چوٹی = false;
	إذا (!لبھو_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    إذا (popup_لبھو_prop_above_چوٹی(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_چوٹی = true;
	    وإلا
	    {
		إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#جے_تعریف_شدہ FEAT_IMAGE_POPUP
		    إذا (wp->w_popup_imagep != عدم)
			image_placement_clear(wp->w_popup_imagep);
#جے_انت
		    popup_hide_for_textprop(wp);
		    إذا (wp->w_winrow + popup_اچائی(wp) >= cmdline_صف)
			clear_cmdline = TRUE;
		}
		أرجع;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.خانہ = prop.tp_خانہ;
	إذا (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.خانہ += prop.tp_len - 1;
	إذا (prop_above_چوٹی)
	    popup_screenpos_above_چوٹی(prop_win, &pos, prop_lnum, &screen_صف,
				     &screen_scol, &screen_ccol, &screen_ecol);
	وإلا
	    textpos2screenpos(prop_win, &pos, &screen_صف,
				     &screen_scol, &screen_ccol, &screen_ecol);

	إذا (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_چوڑائی = 0;
	    أرجع;
	}
	إذا (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_صف + wantline + 1;
	وإلا
	    // above the text
	    wantline = screen_صف + wantline - 1;
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
	    إذا (wp->w_winrow >= صفاں)
		wp->w_winrow = صفاں - 1;
	}
	إذا (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_صف
				    - wp->w_بفر->b_ml.ml_سطر_گنتی - 1, 0);
	}

	إذا (!use_wantcol)
	    center_hor = TRUE;
	وإلا إذا (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    إذا (wp->w_wincol > firstwin->w_wincol + topframe->fr_چوڑائی
						- بایاں_extra - دایاں_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی
						- بایاں_extra - دایاں_extra - 1;
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
    maxspace = firstwin->w_wincol + topframe->fr_چوڑائی
					- wp->w_wincol - بایاں_extra - دایاں_extra;
    maxwidth = maxspace;
    إذا (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_بایاں = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    إذا (wp->w_p_nu || wp->w_p_rnu)
	margin_چوڑائی = نمبر_چوڑائی(wp) + 1;
#جے_تعریف_شدہ FEAT_FOLDING
    margin_چوڑائی += wp->w_p_fdc;
#جے_انت
#جے_تعریف_شدہ FEAT_SIGNS
    إذا (signcolumn_on(wp))
	margin_چوڑائی += 2;
#جے_انت
    إذا (margin_چوڑائی >= maxwidth)
	margin_چوڑائی = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#جے_تعریف_شدہ FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    إذا (wp->w_بفر->b_رکن != عدم)
    {
	إذا (minwidth == 0)
	    minwidth = 20;
	إذا (minheight == 0)
	    minheight = 5;
    }
#جے_انت

    إذا (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_صف - 1;

    // start at the desired first line
    إذا (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	إذا (wp->w_firstline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	    wp->w_firstline = 0;
	وإلا
	    wp->w_topline = wp->w_firstline;
    }
    إذا (wp->w_topline < 1)
	wp->w_topline = 1;
    وإلا إذا (wp->w_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
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
	صحيح saved_w_چوڑائی = wp->w_چوڑائی;

	إذا (wp->w_چوڑائی < maxwidth)
	    wp->w_چوڑائی = maxwidth;
	لكل (ln = 1; ln <= wp->w_بفر->b_ml.ml_سطر_گنتی; ++ln)
	{
	    صحيح len = linetabsize(wp, ln) + margin_چوڑائی;

	    إذا (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    إذا (saved_w_چوڑائی < len)
		saved_w_چوڑائی = len;
	    إذا (wp->w_maxwidth > 0 && saved_w_چوڑائی >= wp->w_maxwidth)
		اكسر;
	}
	wp->w_چوڑائی = saved_w_چوڑائی;
    }
    إذا (wp->w_firstline < 0)
	lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
    وإلا
	lnum = wp->w_topline;
    طالما (lnum >= 1 && lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	صحيح len;
	صحيح w_چوڑائی = wp->w_چوڑائی;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	إذا (wp->w_چوڑائی < maxwidth)
	    wp->w_چوڑائی = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_چوڑائی = w_چوڑائی;

	إذا (len + margin_چوڑائی > maxwidth
		&& allow_adjust_بایاں
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    صحيح کھسکاؤ_by = len + margin_چوڑائی - maxwidth;

	    إذا (کھسکاؤ_by > wp->w_wincol)
	    {
		صحيح truncate_کھسکاؤ = کھسکاؤ_by - wp->w_wincol;

		کھسکاؤ_by -= truncate_کھسکاؤ;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    إذا (wp->w_maxwidth > 0 && maxwidth + کھسکاؤ_by > wp->w_maxwidth)
		کھسکاؤ_by = wp->w_maxwidth - maxwidth;

	    إذا (کھسکاؤ_by > 0)
	    {
		wp->w_wincol -= کھسکاؤ_by;
		maxwidth += کھسکاؤ_by;
	    }
	    wp->w_چوڑائی = maxwidth;
	}
	إذا (wp->w_p_wrap)
	{
	    طالما (len + margin_چوڑائی > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_چوڑائی;
		wp->w_چوڑائی = maxwidth;
		ورتیا_maxwidth = TRUE;
	    }
	}
	إذا (wp->w_چوڑائی < len + margin_چوڑائی)
	{
	    wp->w_چوڑائی = len + margin_چوڑائی;
	    إذا (wp->w_maxwidth > 0 && wp->w_چوڑائی > wp->w_maxwidth)
		wp->w_چوڑائی = wp->w_maxwidth;
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
			       : wp->w_بفر->b_ml.ml_سطر_گنتی - lnum)
		       + wrapped >= maxheight)
	    اكسر;
    }

    إذا (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی);
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (wp->w_بفر->b_رکن != عدم && !رکن_is_مکمل(wp->w_بفر))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#جے_انت
    maxwidth_no_scrollbar = maxwidth;
    إذا (wp->w_has_scrollbar)
    {
	++دایاں_extra;
	++extra_چوڑائی;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	إذا (maxwidth + دایاں_extra >= maxspace
		&& (ورتیا_maxwidth || (minwidth > 0 && wp->w_چوڑائی < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    إذا (wp->w_popup_title != عدم && *wp->w_popup_title != NUL)
    {
	صحيح title_len = vim_strsize(wp->w_popup_title) + 2 - extra_چوڑائی;

	إذا (minwidth < title_len)
	    minwidth = title_len;
    }

    إذا (minwidth > 0 && wp->w_چوڑائی < minwidth)
	wp->w_چوڑائی = minwidth;
    إذا (wp->w_چوڑائی > maxwidth)
    {
	إذا (wp->w_چوڑائی > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_چوڑائی - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	إذا (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    صحيح off = wp->w_چوڑائی - maxwidth;
	    extra_چوڑائی -= MIN(off, دایاں_extra);
	    wp->w_چوڑائی = maxwidth_no_scrollbar;
	}
	وإلا
	{
	    wp->w_چوڑائی = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    چوڑائی_with_scrollbar = maxwidth_no_scrollbar - دایاں_extra;
	}
    }
    إذا (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_چوڑائی - wp->w_چوڑائی - extra_چوڑائی) / 2;
	إذا (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	صحيح leftoff = wantcol - (wp->w_چوڑائی + extra_چوڑائی);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	إذا (leftoff >= 0)
	    wp->w_wincol = leftoff;
	وإلا إذا (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_چوڑائی -= wp->w_leftcol;
	    إذا (wp->w_چوڑائی < 0)
		wp->w_چوڑائی = 0;
	}
    }

    إذا (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	صحيح want_خانہ = 0;

	// try to show the right border and any scrollbar
	want_خانہ = بایاں_extra + wp->w_چوڑائی + دایاں_extra;
	إذا (want_خانہ > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_خانہ >= firstwin->w_wincol + topframe->fr_چوڑائی)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی - want_خانہ;
	    إذا (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_اچائی = wp->w_بفر->b_ml.ml_سطر_گنتی - wp->w_topline
								 + 1 + wrapped;
    إذا (minheight > 0 && wp->w_اچائی < minheight)
	wp->w_اچائی = minheight;
    إذا (maxheight > 0 && wp->w_اچائی > maxheight)
	wp->w_اچائی = maxheight;
    w_اچائی_before_حد_بندی = wp->w_اچائی;
    إذا (wp->w_اچائی > صفاں - wp->w_winrow)
	wp->w_اچائی = صفاں - wp->w_winrow;

    إذا (center_vert)
    {
	wp->w_winrow = (صفاں - wp->w_اچائی - extra_اچائی) / 2;
	إذا (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	إذا ((wp->w_اچائی + extra_اچائی) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_اچائی + extra_اچائی);
	وإلا إذا (wantline * 2 >= صفاں || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_اچائی = wantline - extra_اچائی;
	}
	وإلا
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_اچائی_for_چوٹی_aligned = TRUE;
	}
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	صحيح check_اچائی = (wp->w_popup_flags & POPF_INFO) ? wp->w_اچائی
						    : w_اچائی_before_حد_بندی;
	إذا (wp != popup_dragwin
		&& wantline + (check_اچائی + extra_اچائی) - 1 > صفاں
		&& wantline * 2 > صفاں
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_اچائی = w_اچائی_before_حد_بندی;
	    wp->w_winrow = wantline - 2 - wp->w_اچائی - extra_اچائی;
	    إذا (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_اچائی + 2;
	    إذا (wp->w_winrow < 0)
	    {
		wp->w_اچائی += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	وإلا
	{
	    wp->w_winrow = wantline - 1;
	    adjust_اچائی_for_چوٹی_aligned = TRUE;
	}
    }

    إذا (adjust_اچائی_for_چوٹی_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_اچائی + extra_اچائی > صفاں)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_اچائی = صفاں - wp->w_winrow - extra_اچائی;
#جے_تعریف_شدہ FEAT_TERMINAL
	إذا (wp->w_بفر->b_رکن == عدم || رکن_is_مکمل(wp->w_بفر))
#جے_انت
	{
	    // The width already accounts for a scrollbar that was there.
	    إذا (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		إذا (چوڑائی_with_scrollbar > 0)
		    wp->w_چوڑائی = چوڑائی_with_scrollbar;
		وإلا
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_چوڑائی;
	    }
	}
    }

    // make sure w_winrow is valid
    إذا (wp->w_winrow >= صفاں)
	wp->w_winrow = صفاں - 1;
    وإلا إذا (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    إذا (wp->w_wincol + wp->w_چوڑائی + extra_چوڑائی
				    > firstwin->w_wincol + topframe->fr_چوڑائی)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_چوڑائی
						- wp->w_چوڑائی - extra_چوڑائی;
    إذا (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    إذا (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    إذا (wp->w_wincol + wp->w_چوڑائی + extra_چوڑائی
				    > firstwin->w_wincol + topframe->fr_چوڑائی)
    {
	صحيح avail = firstwin->w_wincol + topframe->fr_چوڑائی
						- wp->w_wincol - extra_چوڑائی;
	wp->w_چوڑائی = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    إذا (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	إذا (wp->w_winrow + wp->w_اچائی + extra_اچائی > صفاں)
	    wp->w_winrow = صفاں - wp->w_اچائی - extra_اچائی;
	إذا (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	إذا (wp->w_winrow + wp->w_اچائی + extra_اچائی > صفاں)
	{
	    صحيح avail = صفاں - wp->w_winrow - extra_اچائی;
	    wp->w_اچائی = avail > 0 ? avail : 0;
	}
    }

    إذا (wp->w_اچائی != org_layout.اچائی)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    إذا (popup_compute_clipwindow_offsets(wp))
    {
#جے_تعریف_شدہ FEAT_IMAGE_POPUP
	إذا (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    إذا (wp->w_popup_imagep != عدم)
		image_placement_clear(wp->w_popup_imagep);
#جے_انت

	popup_hide_for_textprop(wp);
	أرجع;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_آخری_changedtick = CHANGEDTICK(wp->w_بفر);
    إذا (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_بفر);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_چوڑائی = wp->w_popup_prop_win->w_چوڑائی;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_اچائی;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    إذا (popup_layout_بدلیا(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	إذا (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_نقاب_refresh = TRUE;
    }

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    popup_position_image(wp);
#جے_انت
}

تعريفنوع تعداد
{
    TYPE_عام,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_سنیہا_WIN,	// similar to TYPE_NOTIFICATION
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
    أرجع type == TYPE_NOTIFICATION || type == TYPE_سنیہا_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ساكن فراغ
popup_سیٹ_بفر_متن(buf_T *buf, typval_T متن)
{
    صحيح	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    لكل (lnum = buf->b_ml.ml_سطر_گنتی; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_بفر;

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
	    إذا (l->lv_پہلا == &range_list_شے)
		emsg(_(e_using_نمبر_as_string));
	    وإلا إذا (l->lv_پہلا->li_tv.v_type == VAR_STRING)
		// list of strings
		جوڑو_popup_لڑیاں(buf, l);
	    وإلا
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
#جے_تعریف_شدہ FEAT_QUICKFIX
	!is_preview ? p_cpp :
#جے_انت
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
		    سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"wincolor",
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
		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
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
		    سیٹ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		وإلا إذا (STRCMP(token, "double") == 0)
		    سیٹ_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		وإلا إذا (STRCMP(token, "round") == 0)
		    سیٹ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		وإلا إذا (STRCMP(token, "on") == 0)
		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		وإلا إذا (STRCMP(token, "ascii") == 0)
		    سیٹ_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		وإلا إذا (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    صحيح		باہرلا[8];
		    صحيح		failed = FALSE;

		    سیٹ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    لكل (i = 0; i < 8 && !failed; i++)
		    {
			إذا (*q == NUL)
			    failed = TRUE;
			وإلا
			{
			    باہرلا[i] = mb_ptr2char(q);
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
		    سیٹ_BORDER_CHARS(باہرلا[0], باہرلا[1], باہرلا[2], باہرلا[3], باہرلا[4],
			    باہرلا[5], باہرلا[6], باہرلا[7]);
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
	    on = on && mouse_has(MOUSE_داخل_کرو) && (border_enabled || is_preview);
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
		إذا (on && mouse_has(MOUSE_داخل_کرو))
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
	    صحيح		شے = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    صحيح		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    إذا (!menu && !شے)
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
popup_سیٹ_wantpos_cursor(win_T *wp, صحيح چوڑائی, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    إذا (d != عدم)
	ppt = get_pos_اندراج(d, FALSE);

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
    إذا (wp->w_wantcol > Columns - چوڑائی)
    {
	wp->w_wantcol = Columns - چوڑائی;
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
popup_سیٹ_wantpos_rowcol(win_T *wp, صحيح صف, صحيح خانہ)
{
    wp->w_wantline = صف;
    wp->w_wantcol = خانہ;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ساكن فراغ
جوڑو_border_بایاں_دایاں_padding(win_T *wp)
{
    صحيح i;

    لكل (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#جے_تعریف_شدہ FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ساكن صحيح
popup_terminal_exists(فراغ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_بفر->b_رکن != عدم)
	    أرجع TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_وچ_TAB(tp, wp)
	    إذا (wp->w_بفر->b_رکن != عدم)
		أرجع TRUE;
    أرجع FALSE;
}
#جے_انت

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    فراغ
popup_redraw_all(فراغ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
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
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
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
    حرف    *hiname = type == TYPE_سنیہا_WIN
				       ? "MessageWindow" : "PopupNotification";
    حرف    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    إذا (errmsg == عدم)
	سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"wincolor", -1,
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
    صحيح		new_بفر;
    buf_T	*buf = عدم;
    dict_T	*d = عدم;
    صحيح		i;

    إذا (check_secure())
	أرجع عدم;

    إذا (argvars != عدم)
    {
	إذا (وچ_vim9script()
		&& (check_for_string_or_نمبر_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    أرجع عدم;

	// Check that arguments look OK.
	إذا (argvars[0].v_type == VAR_نمبر)
	{
	    buf = buflist_findnr(argvars[0].vval.v_نمبر);
	    إذا (buf == عدم)
	    {
		semsg(_(e_بفر_nr_does_not_exist), argvars[0].vval.v_نمبر);
		أرجع عدم;
	    }
#جے_تعریف_شدہ FEAT_TERMINAL
	    إذا (buf->b_رکن != عدم && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_دوجا_popup_with_terminal));
		أرجع عدم;
	    }
#جے_انت
	}
	وإلا إذا (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != عدم)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != عدم))
	{
	    emsg(_(e_بفر_نمبر_متن_or_list_required));
	    أرجع عدم;
	}
	إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    أرجع عدم;
	d = argvars[1].vval.v_dict;
    }

    إذا (d != عدم)
    {
	إذا (dict_has_کلید(d, "tabpage"))
	    tabnr = (صحيح)dict_get_نمبر(d, "tabpage");
	وإلا إذا (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	وإلا
	    tabnr = 0;
	إذا (tabnr > 0)
	{
	    tp = لبھو_tabpage(tabnr);
	    إذا (tp == عدم)
	    {
		semsg(_(e_tabpage_not_لبھیا_nr), tabnr);
		أرجع عدم;
	    }
	}
    }
    وإلا إذا (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    إذا (buf != عدم && buf->b_locked_ونڈو)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_کھڑکی_منزل_a_closing_بفر));
	أرجع عدم;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    إذا (wp == عدم)
	أرجع عدم;
    إذا (rettv != عدم)
	rettv->vval.v_نمبر = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    إذا (buf != عدم)
    {
	// use existing buffer
	new_بفر = FALSE;
	win_init_popup_win(wp, buf);
	سیٹ_local_options_default(wp, FALSE);
	وٹاندرا_exists_action = SEA_READONLY;
	بفر_ensure_loaded(buf);
	وٹاندرا_exists_action = SEA_NONE;
    }
    وإلا
    {
	// create a new buffer associated with the popup
	new_بفر = TRUE;
	buf = buflist_new(عدم, عدم, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	إذا (buf == عدم)
	{
	    win_free_popup(wp);
	    أرجع عدم;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	سیٹ_local_options_default(wp, TRUE);
	سیٹ_string_option_سدھا_وچ_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	سیٹ_string_option_سدھا_وچ_buf(buf, (char_u *)"bufhidden", -1,
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
	wp->w_اگلا = tp->tp_پہلا_popupwin;
	tp->tp_پہلا_popupwin = wp;
    }
    وإلا إذا (tabnr == 0)
    {
	// popup on current tab page
	wp->w_اگلا = curtab->tp_پہلا_popupwin;
	curtab->tp_پہلا_popupwin = wp;
    }
    وإلا // (tabnr < 0)
    {
	win_T *prev = پہلا_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	إذا (پہلا_popupwin == عدم)
	    پہلا_popupwin = wp;
	وإلا
	{
	    طالما (prev->w_اگلا != عدم)
		prev = prev->w_اگلا;
	    prev->w_اگلا = wp;
	}
#جے_تعریف_شدہ FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#جے_انت
    }

    إذا (new_بفر && argvars != عدم)
	popup_سیٹ_بفر_متن(buf, argvars[0]);

    إذا (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    إذا (type == TYPE_ATCURSOR)
    {
	popup_سیٹ_wantpos_cursor(wp, 0, d);
	سیٹ_moved_اقدار(wp);
	سیٹ_moved_columns(wp, لبھو_STRING);
    }

    إذا (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_صف;
	إذا (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_خانہ + 1;
	سیٹ_mousemoved_اقدار(wp);
	سیٹ_mousemoved_columns(wp, لبھو_IDENT + لبھو_STRING + لبھو_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    إذا (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	صحيح	اچائی = buf->b_ml.ml_سطر_گنتی + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	لكل (twp = پہلا_popupwin; twp != عدم; twp = nextwin)
	{
	    nextwin = twp->w_اگلا;
	    إذا (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + اچائی
		    && twp->w_winrow + popup_اچائی(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_اچائی(twp) + 1;
		nextwin = پہلا_popupwin;
	    }
	}
	إذا (wp->w_wantline + اچائی > صفاں)
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
	جوڑو_border_بایاں_دایاں_padding(wp);
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
	    سیٹ_callback(&wp->w_filter_cb, &callback);
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
	إذا (mouse_has(MOUSE_داخل_کرو))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	لكل (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_سیٹ_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#جے_تعریف_شدہ FEAT_QUICKFIX
    إذا (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	إذا (mouse_has(MOUSE_داخل_کرو))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	جوڑو_border_بایاں_دایاں_padding(wp);
	parse_completepopup(wp);
    }
#جے_انت

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_منوال = منوال_ALL;

    إذا (d != عدم)
    {
	// Deal with options.
	إذا (apply_options(wp, d, TRUE) == FAIL)
	{
	    (فراغ)popup_close(wp->w_id, FALSE);
	    أرجع عدم;
	}
    }

#جے_تعریف_شدہ FEAT_TIMERS
    إذا (popup_is_notification(type) && wp->w_popup_timer == عدم)
	popup_جوڑو_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#جے_انت

    popup_adjust_position(wp);

    wp->w_vsep_چوڑائی = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_نقاب_refresh = TRUE;

#جے_تعریف_شدہ FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    إذا (buf->b_رکن != عدم)
	win_enter(wp, FALSE);
#جے_انت

    أرجع wp;
}

/*
 * popup_clear()
 */
    فراغ
f_popup_clear(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح force = FALSE;

    إذا (وچ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
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
    popup_create(argvars, rettv, TYPE_عام);
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
invoke_popup_callback(win_T *wp, typval_T *نتیجہ)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = (varnumber_T)wp->w_id;

    إذا (نتیجہ != عدم && نتیجہ->v_type != VAR_UNKNOWN)
	copy_tv(نتیجہ, &argv[1]);
    وإلا
    {
	argv[1].v_type = VAR_نمبر;
	argv[1].vval.v_نمبر = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    إذا (نتیجہ != عدم)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ساكن فراغ
پٹھ_منزل_prevwin(win_T *wp)
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

#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (wp == curwin && curbuf->b_رکن != عدم)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    إذا (owp != curwin && owp->w_بفر->b_رکن != عدم)
		اكسر;
	إذا (owp != عدم)
	    win_enter(owp, FALSE);
	وإلا
	{
	    FOR_ALL_POPUPWINS_وچ_TAB(curtab, owp)
		إذا (owp != curwin && owp->w_بفر->b_رکن != عدم)
		    اكسر;
	    إذا (owp != عدم)
		win_enter(owp, FALSE);
	    وإلا
		پٹھ_منزل_prevwin(wp);
	}
    }
#جے_انت

    // Just in case a check higher up is missing.
    إذا (wp == curwin && ERROR_IF_POPUP_کھڑکی)
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

    res.v_type = VAR_نمبر;
    res.vval.v_نمبر = retval;
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
	    && wp->w_popup_mouse_صف != 0
	    && (wp->w_popup_mouse_صف != mouse_صف
		|| mouse_خانہ < wp->w_popup_mouse_mincol
		|| mouse_خانہ > wp->w_popup_mouse_maxcol))
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
    صحيح	    صف = mouse_صف;
    صحيح	    خانہ = mouse_خانہ;

    // find the window where the mouse is in
    mouse_wp = mouse_لبھو_win(&صف, &خانہ, لبھو_POPUP);

    لكل (wp = پہلا_popupwin; wp != عدم; wp = nextwp)
    {
	nextwp = wp->w_اگلا;
	check_mouse_moved(wp, mouse_wp);
    }
    لكل (wp = curtab->tp_پہلا_popupwin; wp != عدم; wp = nextwp)
    {
	nextwp = wp->w_اگلا;
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
    صحيح	صف = mouse_صف;
    صحيح	خانہ = mouse_خانہ;

    إذا ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_کلید(c)
	    && (wp == popup_dragwin
			  || wp == mouse_لبھو_win(&صف, &خانہ, لبھو_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_نمبر = 0;
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

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    id = tv_get_نمبر(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    إذا (wp == عدم)
	أرجع;

    c = *کلید;
    إذا (c == K_SPECIAL && کلید[1] != NUL)
	c = منزل_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_TRUE;
    res.v_type = VAR_نمبر;

    old_lnum = wp->w_cursor.lnum;
    إذا (c == 'k' || c == 'K' || c == K_اتے || c == Ctrl_P)
    {
	إذا (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	وإلا
	    wp->w_cursor.lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
    }
    إذا (c == 'j' || c == 'J' || c == K_تھلے || c == Ctrl_N)
    {
	إذا (wp->w_cursor.lnum < wp->w_بفر->b_ml.ml_سطر_گنتی)
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
	res.vval.v_نمبر = -1;
	popup_close_and_callback(wp, &res);
	أرجع;
    }
    إذا (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_نمبر = wp->w_cursor.lnum;
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

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    id = tv_get_نمبر(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    إذا (wp == عدم)
	أرجع;

    c = *کلید;
    إذا (c == CAR && need_wait_return)
	أرجع;
    إذا (c == K_SPECIAL && کلید[1] != NUL)
	c = منزل_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_TRUE;

    إذا (c == 'y' || c == 'Y')
	res.vval.v_نمبر = 1;
    وإلا إذا (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_نمبر = 0;
    وإلا
    {
	filter_handle_drag(wp, c, rettv);
	أرجع;
    }

    // Invoke callback
    res.v_type = VAR_نمبر;
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
لبھو_popup_win(صحيح id)
{
    win_T *wp = win_id2wp(id);

    إذا (wp != عدم && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_کھڑکی_nr_is_not_popup_کھڑکی), id);
	أرجع عدم;
    }
    أرجع wp;
}

/*
 * popup_close({id})
 */
    فراغ
f_popup_close(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح		id;
    win_T	*wp;

    إذا (وچ_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    إذا (
#جے_تعریف_شدہ FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_رکن == عدم &&
#جے_انت
	    ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع;

    wp = لبھو_popup_win(id);
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
popup_clear_نقاب_for(win_T *wp)
{
    صحيح r, c;
    صحيح صف_شروع, خانہ_شروع, صف_سرا, خانہ_سرا;

    إذا (popup_نقاب == عدم || !popup_visible)
	أرجع;

    صف_شروع = MAX(wp->w_winrow, 0);
    خانہ_شروع = MAX(wp->w_wincol, 0);
    صف_سرا = MIN(wp->w_winrow + popup_اچائی(wp), (صحيح)screen_صفاں);
    خانہ_سرا = MIN(wp->w_wincol + popup_چوڑائی(wp), (صحيح)screen_Columns);

    لكل (r = صف_شروع; r < صف_سرا; ++r)
	لكل (c = خانہ_شروع; c < خانہ_سرا; ++c)
	    popup_نقاب[r * screen_Columns + c] = 0;
}

    فراغ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    صحيح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (error_if_رکن_popup_کھڑکی())
	أرجع;
#جے_انت
    إذا ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	أرجع;

    popup_save_area(wp, &old_area);

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم)
	image_placement_clear(wp->w_popup_imagep);
#جے_انت

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    إذا (wp->w_winrow + popup_اچائی(wp) >= cmdline_صف)
	clear_cmdline = TRUE;

    إذا (was_visible)
	popup_clear_نقاب_for(wp);

    إذا (old_area.سرگرم)
	popup_redraw_exposed_area(&old_area);
    وإلا
	redraw_all_later(UPD_NOT_VALID);

    حالت_redraw_all();
    إذا (!old_area.سرگرم)
	popup_نقاب_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    فراغ
f_popup_hide(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح		id;
    win_T	*wp;

    إذا (وچ_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    فراغ
popup_show(win_T *wp)
{
    bool popup_سرگرم;

    إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	أرجع;

    popup_سرگرم = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (popup_سرگرم)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطریں_valid = 0;
	إذا (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    وإلا
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_نقاب_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    فراغ
f_popup_show(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح		id;
    win_T	*wp;

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = -1;

    إذا (وچ_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#جے_تعریف_شدہ FEAT_QUICKFIX
    إذا (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#جے_انت

    rettv->vval.v_نمبر = 0;
}

/*
 * popup_settext({id}, {text})
 */
    فراغ
f_popup_settext(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح		id;
    win_T	*wp;
    popup_area_T	old_area;

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_نمبر(&argvars[0]);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    popup_save_area(wp, &old_area);

    إذا (check_for_string_or_list_arg(argvars, 1) == FAIL)
	أرجع;

    popup_سیٹ_بفر_متن(wp->w_بفر, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطریں_valid = 0;
    إذا (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    إذا (popup_area_بدلیا(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    فراغ
f_popup_setbuf(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_FALSE;

    إذا (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_بفر_arg(argvars, 1) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(&argvars[0]);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[1]);

    إذا (buf == عدم)
	أرجع;
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (buf->b_رکن != عدم && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_دوجا_popup_with_terminal));
	أرجع;
    }
#جے_انت

    إذا (wp->w_بفر != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_بفر->b_nwindows--;
	win_init_popup_win(wp, buf);
	سیٹ_local_options_default(wp, FALSE);
	وٹاندرا_exists_action = SEA_READONLY;
	بفر_ensure_loaded(buf);
	وٹاندرا_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	إذا (popup_area_بدلیا(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_نمبر = VVAL_TRUE;
}

    ساكن فراغ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    صحيح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم)
	image_placement_free(wp->w_popup_imagep);
#جے_انت

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_بفر->b_locked = FALSE;
    إذا (wp->w_winrow + popup_اچائی(wp) >= cmdline_صف)
	clear_cmdline = TRUE;

    إذا (was_visible)
	popup_clear_نقاب_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#جے_تعریف_شدہ HAS_سنیہا_کھڑکی
    إذا (wp == سنیہا_win)
	سنیہا_win = عدم;
#جے_انت

    إذا (!old_area.سرگرم)
	redraw_all_later(UPD_NOT_VALID);
    حالت_redraw_all();
    إذا (!old_area.سرگرم)
	popup_نقاب_refresh = TRUE;
}

    ساكن فراغ
error_for_popup_کھڑکی(فراغ)
{
    emsg(_(e_not_allowed_وچ_popup_کھڑکی));
}

    صحيح
error_if_popup_کھڑکی(صحيح also_with_رکن بن_ورتیا)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    إذا (WIN_IS_POPUP(curwin)
#جے_تعریف_شدہ FEAT_TERMINAL
	    && (also_with_رکن || curbuf->b_رکن == عدم)
#جے_انت
	    )
    {
	error_for_popup_کھڑکی();
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
    لكل (wp = پہلا_popupwin; wp != عدم; prev = wp, wp = wp->w_اگلا)
	إذا (wp->w_id == id)
	{
	    إذا (wp == curwin)
	    {
		إذا (!force)
		{
		    error_for_popup_کھڑکی();
		    أرجع FAIL;
		}
		پٹھ_منزل_prevwin(wp);
	    }
	    إذا (prev == عدم)
		پہلا_popupwin = wp->w_اگلا;
	    وإلا
		prev->w_اگلا = wp->w_اگلا;
#جے_تعریف_شدہ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#جے_انت
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
    win_T	**جڑ = &tp->tp_پہلا_popupwin;
    win_T	*prev = عدم;

    لكل (wp = *جڑ; wp != عدم; prev = wp, wp = wp->w_اگلا)
	إذا (wp->w_id == id)
	{
	    إذا (wp == curwin)
	    {
		إذا (!force)
		{
		    error_for_popup_کھڑکی();
		    أرجع FAIL;
		}
		پٹھ_منزل_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    إذا (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    إذا (prev == عدم)
		*جڑ = wp->w_اگلا;
	    وإلا
		prev->w_اگلا = wp->w_اگلا;
#جے_تعریف_شدہ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#جے_انت
	    popup_free(wp);
	    أرجع OK;
	}
    أرجع FAIL;
}

    فراغ
close_all_popups(صحيح force)
{
    إذا (!force && ERROR_IF_ANY_POPUP_کھڑکی)
	أرجع;
    طالما (پہلا_popupwin != عدم)
	إذا (popup_close(پہلا_popupwin->w_id, force) == FAIL)
	    أرجع;
    طالما (curtab->tp_پہلا_popupwin != عدم)
	إذا (popup_close(curtab->tp_پہلا_popupwin->w_id, force) == FAIL)
	    أرجع;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ساكن فراغ
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->سرگرم = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->اچائی = popup_اچائی(wp);
    area->چوڑائی = popup_چوڑائی(wp);
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
popup_style_بدلیا(win_T *wp, popup_style_snapshot_T *style)
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
    layout->چوڑائی = wp->w_چوڑائی;
    layout->اچائی = wp->w_اچائی;
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
popup_layout_بدلیا(win_T *wp, popup_layout_T *layout)
{
    أرجع layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->چوڑائی != wp->w_چوڑائی
	|| layout->اچائی != wp->w_اچائی
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
popup_area_بدلیا(win_T *wp, popup_area_T *area)
{
    أرجع area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->اچائی != popup_اچائی(wp)
	|| area->چوڑائی != popup_چوڑائی(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ساكن فراغ
mark_overlapped_opacity_popup(win_T *wp, صحيح area_چوٹی, صحيح area_bot,
	صحيح area_بایاں, صحيح area_دایاں, صحيح zindex)
{
    إذا ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_اچائی(wp) <= area_چوٹی
	    || wp->w_wincol >= area_دایاں
	    || wp->w_wincol + popup_چوڑائی(wp) - wp->w_popup_leftoff
							      <= area_بایاں)
	أرجع;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطریں_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ساكن فراغ
redraw_overlapped_opacity_popups(صحيح winrow, صحيح wincol, صحيح اچائی, صحيح چوڑائی,
	صحيح leftoff, صحيح zindex)
{
    win_T	*wp;
    صحيح		area_چوٹی = winrow;
    صحيح		area_bot = winrow + اچائی;
    صحيح		area_بایاں = wincol;
    صحيح		area_دایاں = wincol + چوڑائی - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_چوٹی, area_bot, area_بایاں,
		area_دایاں, zindex);
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_چوٹی, area_bot, area_بایاں,
		area_دایاں, zindex);

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
refill_pum_bg_صف_from_بفر(صحيح r, صحيح بایاں, صحيح دایاں)
{
    صحيح		سطر_cp = r;
    صحيح		خانہ_cp = بایاں;
    win_T	*wp;
    linenr_T	lnum;
    صحيح		صف_for_lnum;
    صحيح		خانہ_for_lnum = 0;
    char_u	*سطر;
    char_u	*p;
    صحيح		win_متن_خانہ;
    صحيح		screen_خانہ;
    صحيح		soff_ادھار = (r - pum_bg_چوٹی) * pum_bg_خانے_گنتی;
    صحيح		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    لكل (c = بایاں; c < دایاں; ++c)
    {
	pum_bg_سطریں[soff_ادھار + c] = ' ';
	إذا (pum_bg_attrs != عدم)
	    pum_bg_attrs[soff_ادھار + c] = 0;
	إذا (enc_utf8 && pum_bg_سطریں_UC != عدم)
	    pum_bg_سطریں_UC[soff_ادھار + c] = 0;
	إذا (enc_utf8)
	{
	    صحيح k;
	    لكل (k = 0; k < MAX_MCO; ++k)
		إذا (pum_bg_سطریں_C[k] != عدم)
		    pum_bg_سطریں_C[k][soff_ادھار + c] = 0;
	}
    }

    wp = mouse_لبھو_win(&سطر_cp, &خانہ_cp, IGNORE_POPUP);
    إذا (wp == عدم || سطر_cp < 0 || سطر_cp >= wp->w_اچائی)
	أرجع;
    إذا (wp->w_بفر == عدم || wp->w_بفر->b_ml.ml_mfp == عدم)
	أرجع;

    // Compute the buffer line for this screen row.
    صف_for_lnum = سطر_cp;
    إذا (mouse_comp_pos(wp, &صف_for_lnum, &خانہ_for_lnum, &lnum, عدم))
	أرجع;	// past end of buffer
    إذا (lnum < 1 || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	أرجع;

    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    إذا (سطر == عدم)
	أرجع;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_متن_خانہ = wp->w_wincol + win_خانہ_off(wp);
    إذا (!wp->w_p_wrap)
	win_متن_خانہ -= wp->w_leftcol;
    screen_خانہ = win_متن_خانہ;
    p = سطر;
    طالما (*p != NUL && screen_خانہ < دایاں)
    {
	صحيح char_cells;
	صحيح byte_گنتی;
	صحيح soff = soff_ادھار + screen_خانہ;
	صحيح وچ_range = (screen_خانہ >= بایاں && screen_خانہ < دایاں);

	إذا (*p == '\t')
	{
	    صحيح ts = (صحيح)wp->w_بفر->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_خانہ - win_متن_خانہ) % ts) : 1;
	    byte_گنتی = 1;
	    إذا (وچ_range)
	    {
		pum_bg_سطریں[soff] = ' ';
		إذا (enc_utf8 && pum_bg_سطریں_UC != عدم)
		    pum_bg_سطریں_UC[soff] = 0;
	    }
	}
	وإلا إذا (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_گنتی = mb_ptr2len(p);
	    إذا (وچ_range)
	    {
		pum_bg_سطریں[soff] = *p;
		إذا (enc_utf8 && pum_bg_سطریں_UC != عدم)
		    pum_bg_سطریں_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	وإلا
	{
	    char_cells = 1;
	    byte_گنتی = 1;
	    إذا (وچ_range)
	    {
		pum_bg_سطریں[soff] = (*p < 0x20) ? ' ' : *p;
		إذا (enc_utf8 && pum_bg_سطریں_UC != عدم)
		    pum_bg_سطریں_UC[soff] = 0;
	    }
	}
	إذا (وچ_range && pum_bg_attrs != عدم)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_گنتی;
	screen_خانہ += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ساكن فراغ
popup_redraw_exposed_area(popup_area_T *area)
{
    إذا (!area->سرگرم)
	أرجع;

    redraw_under_popup_area(area->winrow, area->wincol, area->اچائی,
	    area->چوڑائی, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->اچائی, area->چوڑائی, area->leftoff, area->zindex);

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
    إذا (pum_bg_سطریں != عدم
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->اچائی > pum_bg_چوٹی)
    {
	صحيح چوٹی = MAX(area->winrow, pum_bg_چوٹی);
	صحيح bot = MIN(area->winrow + area->اچائی, pum_bg_bot);
	صحيح بایاں = MAX(area->wincol, 0);
	صحيح دایاں = MIN(area->wincol + area->چوڑائی, pum_bg_خانے_گنتی);
	صحيح r;

	لكل (r = چوٹی; r < bot; ++r)
	    refill_pum_bg_صف_from_بفر(r, بایاں, دایاں);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ساكن فراغ
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
    ساكن فراغ
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    إذا (screen_opacity_popup == عدم
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	أرجع;

    saved_screen->شروع_صف = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->شروع_خانہ = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->صفاں_2 = wp->w_popup_padding[0] + wp->w_اچائی
						    + wp->w_popup_padding[2];
    saved_screen->خانے_گنتی = wp->w_popup_padding[3] + wp->w_چوڑائی
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    إذا (saved_screen->شروع_خانہ > 0)
    {
	--saved_screen->شروع_خانہ;
	++saved_screen->خانے_گنتی;
    }

    saved_screen->سطریں = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->صفاں_2 * saved_screen->خانے_گنتی);
    saved_screen->attrs = ALLOC_CLEAR_MULT(صحيح,
				  saved_screen->صفاں_2 * saved_screen->خانے_گنتی);
    إذا (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->صفاں_2 * saved_screen->خانے_گنتی);

    إذا (saved_screen->سطریں == عدم || saved_screen->attrs == عدم)
	أرجع;

    لكل (صحيح r = 0; r < saved_screen->صفاں_2; r++)
    {
	صحيح screen_صف = saved_screen->شروع_صف + r;

	إذا (screen_صف >= 0 && screen_صف < screen_صفاں)
	    لكل (صحيح c = 0; c < saved_screen->خانے_گنتی; c++)
	    {
		صحيح screen_خانہ = saved_screen->شروع_خانہ + c;

		إذا (screen_خانہ >= 0 && screen_خانہ < screen_Columns)
		{
		    صحيح off = سطر_وتھ[screen_صف] + screen_خانہ;
		    صحيح save_off = r * saved_screen->خانے_گنتی + c;

		    saved_screen->سطریں[save_off] = Screen_سطریں[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    إذا (enc_utf8 && saved_screen->linesuc != عدم)
			saved_screen->linesuc[save_off] = Screen_سطریں_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ساكن فراغ
redraw_under_popup_area(صحيح winrow, صحيح wincol, صحيح اچائی, صحيح چوڑائی, صحيح leftoff)
{
    صحيح	    r;

    لكل (r = winrow; r < winrow + اچائی && r < screen_صفاں; ++r)
    {
	صحيح	    c;
	win_T	    *prev_twp = عدم;

	إذا (r >= cmdline_صف)
	{
	    clear_cmdline = TRUE;
	    استمر;
	}

	لكل (c = wincol; c < wincol + چوڑائی - leftoff && c < screen_Columns; ++c)
	{
	    صحيح	    سطر_cp = r;
	    صحيح	    خانہ_cp = c;
	    win_T   *twp;

	    twp = mouse_لبھو_win(&سطر_cp, &خانہ_cp, IGNORE_POPUP);
	    إذا (twp != عدم && twp != prev_twp)
	    {
		prev_twp = twp;
		إذا (سطر_cp < twp->w_اچائی)
		{
		    linenr_T lnum;

		    (فراغ)mouse_comp_pos(twp, &سطر_cp, &خانہ_cp, &lnum, عدم);
		    redrawWinline(twp, lnum);
		}
		وإلا إذا (سطر_cp == twp->w_اچائی)
		    twp->w_redr_حالت = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    فراغ
f_popup_move(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    dict_T	*dict;
    صحيح		id;
    win_T	*wp;
    popup_area_T	old_area;

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;  // invalid {id}

    إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	أرجع;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    إذا (wp->w_winrow + wp->w_اچائی >= cmdline_صف)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    إذا (popup_area_بدلیا(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    فراغ
f_popup_setoptions(typval_T *argvars, typval_T *rettv بن_ورتیا)
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

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
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
	    && wp->w_firstline <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    إذا (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    وإلا إذا (popup_style_بدلیا(wp, &old_style))
	need_redraw = TRUE;

    إذا (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    إذا (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_نقاب_refresh = TRUE;
    }
    وإلا إذا (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطریں_valid = 0;
	إذا (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    إذا (old_style.blend != wp->w_popup_blend)
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

    إذا (popup_area_بدلیا(wp, &old_area))
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
    صحيح		چوٹی_extra;
    صحيح		بایاں_extra;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (وچ_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;  // invalid {id}
    چوٹی_extra = popup_چوٹی_extra(wp);
    بایاں_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_جندرا_ناپ(&dict->dv_hashtab, 11);

    dict_جوڑو_نمبر(dict, "line", wp->w_winrow + 1);
    dict_جوڑو_نمبر(dict, "col", wp->w_wincol + 1);
    dict_جوڑو_نمبر(dict, "width", wp->w_چوڑائی + بایاں_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_جوڑو_نمبر(dict, "height", wp->w_اچائی + چوٹی_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_جوڑو_نمبر(dict, "core_line", wp->w_winrow + 1 + چوٹی_extra);
    dict_جوڑو_نمبر(dict, "core_col", wp->w_wincol + 1 + بایاں_extra);
    dict_جوڑو_نمبر(dict, "core_width", wp->w_چوڑائی);
    dict_جوڑو_نمبر(dict, "core_height", wp->w_اچائی);

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
    فراغ
f_popup_list(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    FOR_ALL_POPUPWINS(wp)
	list_append_نمبر(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_وچ_TAB(tp, wp)
	    list_append_نمبر(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    فراغ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    صحيح		صف;
    صحيح		خانہ;
    win_T	*wp;

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	أرجع;

    صف = tv_get_نمبر(&argvars[0]) - 1;
    خانہ = tv_get_نمبر(&argvars[1]) - 1;
    wp = mouse_لبھو_win(&صف, &خانہ, لبھو_POPUP);
    إذا (wp != عدم && WIN_IS_POPUP(wp))
	rettv->vval.v_نمبر = wp->w_id;
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

    إذا (dict_جوڑو_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    إذا (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	لكل (i = 0; i < 4; ++i)
	    list_append_نمبر(list, array[i]);
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

    إذا (dict_جوڑو_list(dict, "borderhighlight", list) == FAIL)
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

    إذا (dict_جوڑو_list(dict, "borderchars", list) == FAIL)
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
	إذا (dict_جوڑو_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	وإلا
	{
	    list_append_نمبر(list, wp->w_popup_lnum);
	    list_append_نمبر(list, wp->w_popup_mincol);
	    list_append_نمبر(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    إذا (dict_جوڑو_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    list_append_نمبر(list, wp->w_popup_mouse_صف);
    list_append_نمبر(list, wp->w_popup_mouse_mincol);
    list_append_نمبر(list, wp->w_popup_mouse_maxcol);
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

    إذا (وچ_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_نمبر(argvars);
    wp = لبھو_popup_win(id);
    إذا (wp == عدم)
	أرجع;

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
    إذا (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = متن_prop_type_by_id(
		wp->w_popup_prop_win->w_بفر,
		wp->w_popup_prop_type);

	إذا (pt != عدم)
	    dict_جوڑو_string(dict, "textprop", pt->pt_name);
	dict_جوڑو_نمبر(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_جوڑو_نمبر(dict, "textpropid", wp->w_popup_prop_id);
    }

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم && wp->w_popup_imagep->img != عدم)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	إذا (dict_جوڑو_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#جے_انت

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
    إذا (wp->w_scrollbar_highlight != عدم)
	dict_جوڑو_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    إذا (wp->w_thumb_highlight != عدم)
	dict_جوڑو_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_وچ_TAB(tp, twp)
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
    dict_جوڑو_نمبر(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    إذا (wp->w_filter_cb.cb_name != عدم)
	dict_جوڑو_callback(dict, "filter", &wp->w_filter_cb);
    إذا (wp->w_close_cb.cb_name != عدم)
	dict_جوڑو_callback(dict, "callback", &wp->w_close_cb);

    لكل (i = 0; i < (صحيح)ARRAY_لمائی(poppos_entries); ++i)
	إذا (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_جوڑو_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(صحيح)poppos_entries[i].pp_name.لمائی);
	    اكسر;
	}

    إذا (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_جوڑو_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    وإلا إذا (wp->w_popup_close == POPCLOSE_CLICK)
	dict_جوڑو_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    وإلا
	dict_جوڑو_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#إذا defined(FEAT_TIMERS)
    dict_جوڑو_نمبر(dict, "time", wp->w_popup_timer != عدم
	    ?  (لما)wp->w_popup_timer->tr_interval : 0L);
#جے_انت
}

#إذا defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    صحيح
error_if_رکن_popup_کھڑکی(فراغ)
{
    إذا (WIN_IS_POPUP(curwin) && curbuf->b_رکن != عدم
					   && رکن_کم_running(curbuf->b_رکن))
    {
	emsg(_(e_not_allowed_for_terminal_وچ_popup_کھڑکی));
	أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

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
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
لبھو_اگلا_popup(صحيح سبھ_توں_نیواں, صحيح handled_flag)
{
    win_T   *wp;
    win_T   *لبھیا_wp;
    صحيح	    لبھیا_zindex;

    لبھیا_zindex = سبھ_توں_نیواں ? ودھ_توں_ودھ_عدد : 0;
    لبھیا_wp = عدم;
    FOR_ALL_POPUPWINS(wp)
	إذا ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (سبھ_توں_نیواں ? wp->w_zindex < لبھیا_zindex
		    : wp->w_zindex > لبھیا_zindex))
	{
	    لبھیا_zindex = wp->w_zindex;
	    لبھیا_wp = wp;
	}
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (سبھ_توں_نیواں ? wp->w_zindex < لبھیا_zindex
		    : wp->w_zindex > لبھیا_zindex))
	{
	    لبھیا_zindex = wp->w_zindex;
	    لبھیا_wp = wp;
	}

    إذا (لبھیا_wp != عدم)
	لبھیا_wp->w_popup_handled |= handled_flag;
    أرجع لبھیا_wp;
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

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_منزل_buf(c, mod_نقاب, FALSE, buf)] = NUL;
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

#جے_تعریف_شدہ FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    إذا (popup_is_popup(curwin) && curbuf->b_رکن != عدم)
	أرجع FALSE;
#جے_انت

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    إذا (State == منوال_HITRETURN || State == منوال_ASKMORE
	    || State == منوال_CONFIRM || State == منوال_EXTERNCMD)
	أرجع FALSE;

    إذا (recursive)
	أرجع FALSE;
    recursive = TRUE;

    إذا (c == K_LEFTMOUSE)
    {
	صحيح صف = mouse_صف;
	صحيح خانہ = mouse_خانہ;

	wp = mouse_لبھو_win(&صف, &خانہ, لبھو_POPUP);
	إذا (wp != عدم && popup_close_if_on_X(wp, صف, خانہ))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    طالما (res == FALSE
		     && (wp = لبھو_اگلا_popup(FALSE, POPUP_HANDLED_2)) != عدم)
	إذا (wp->w_filter_cb.cb_name != عدم
		&& (wp->w_filter_منوال & state) != 0)
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
    صحيح	    گیڑا;
    win_T   *wp;

    لكل (گیڑا = 1; گیڑا <= 2; ++گیڑا)
	لكل (wp = گیڑا == 1 ? پہلا_popupwin : curtab->tp_پہلا_popupwin;
						   wp != عدم; wp = wp->w_اگلا)
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
    طالما ((wp = لبھو_اگلا_popup(TRUE, POPUP_HANDLED_3)) != عدم)
	إذا (wp->w_popup_curwin != عدم
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.خانہ < wp->w_popup_mincol
		    || curwin->w_cursor.خانہ > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ساكن فراغ
popup_update_نقاب(win_T *wp, صحيح چوڑائی, صحيح اچائی)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    صحيح		صف, خانہ;

    إذا (wp->w_popup_نقاب == عدم || چوڑائی == 0 || اچائی == 0)
    {
	VIM_CLEAR(wp->w_popup_نقاب_cells);
	أرجع;
    }
    إذا (wp->w_popup_نقاب_cells != عدم
	    && wp->w_popup_نقاب_اچائی == اچائی
	    && wp->w_popup_نقاب_چوڑائی == چوڑائی)
	أرجع;  // cache is still valid

    vim_free(wp->w_popup_نقاب_cells);
    إذا (چوڑائی > 0 && (size_t)اچائی > ناپ_MAX / (size_t)چوڑائی)
    {
	wp->w_popup_نقاب_cells = عدم;
	أرجع;
    }
    wp->w_popup_نقاب_cells = alloc_clear((size_t)چوڑائی * اچائی);
    إذا (wp->w_popup_نقاب_cells == عدم)
	أرجع;
    cells = wp->w_popup_نقاب_cells;

    FOR_ALL_LIST_اشیاء(wp->w_popup_نقاب, lio)
    {
	صحيح خانے_گنتی, cole;
	صحيح سطریں, linee;

	li = lio->li_tv.vval.v_list->lv_پہلا;
	خانے_گنتی = tv_get_نمبر(&li->li_tv);
	إذا (خانے_گنتی < 0)
	    خانے_گنتی = چوڑائی + خانے_گنتی + 1;
	إذا (خانے_گنتی <= 0)
	    خانے_گنتی = 1;
	li = li->li_اگلا;
	cole = tv_get_نمبر(&li->li_tv);
	إذا (cole < 0)
	    cole = چوڑائی + cole + 1;
	إذا (cole > چوڑائی)
	    cole = چوڑائی;
	li = li->li_اگلا;
	سطریں = tv_get_نمبر(&li->li_tv);
	إذا (سطریں < 0)
	    سطریں = اچائی + سطریں + 1;
	إذا (سطریں <= 0)
	    سطریں = 1;
	li = li->li_اگلا;
	linee = tv_get_نمبر(&li->li_tv);
	إذا (linee < 0)
	    linee = اچائی + linee + 1;
	إذا (linee > اچائی)
	    linee = اچائی;

	لكل (صف = سطریں - 1; صف < linee; ++صف)
	    لكل (خانہ = خانے_گنتی - 1; خانہ < cole; ++خانہ)
		cells[صف * چوڑائی + خانہ] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ساكن صحيح
popup_masked(win_T *wp, صحيح چوڑائی, صحيح اچائی, صحيح screencol, صحيح screenline)
{
    صحيح خانہ = screencol - wp->w_wincol + wp->w_popup_leftoff;
    صحيح سطر = screenline - wp->w_winrow;

    أرجع خانہ >= 0 && خانہ < چوڑائی
	    && سطر >= 0 && سطر < اچائی
	    && wp->w_popup_نقاب_cells[سطر * چوڑائی + خانہ];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ساكن فراغ
update_popup_transparent(win_T *wp, صحيح val)
{
    إذا (wp->w_popup_نقاب == عدم)
	أرجع;

    صحيح		چوڑائی = popup_چوڑائی(wp);
    صحيح		اچائی = popup_اچائی(wp);
    listitem_T	*lio, *li;
    صحيح		خانے_گنتی, cole;
    صحيح		سطریں, linee;
    صحيح		خانہ, سطر;

    FOR_ALL_LIST_اشیاء(wp->w_popup_نقاب, lio)
    {
	li = lio->li_tv.vval.v_list->lv_پہلا;
	خانے_گنتی = tv_get_نمبر(&li->li_tv);
	إذا (خانے_گنتی < 0)
	    خانے_گنتی = چوڑائی + خانے_گنتی + 1;
	li = li->li_اگلا;
	cole = tv_get_نمبر(&li->li_tv);
	إذا (cole < 0)
	    cole = چوڑائی + cole + 1;
	li = li->li_اگلا;
	سطریں = tv_get_نمبر(&li->li_tv);
	إذا (سطریں < 0)
	    سطریں = اچائی + سطریں + 1;
	li = li->li_اگلا;
	linee = tv_get_نمبر(&li->li_tv);
	إذا (linee < 0)
	    linee = اچائی + linee + 1;

	--خانے_گنتی;
	خانے_گنتی -= wp->w_popup_leftoff;
	إذا (خانے_گنتی < 0)
	    خانے_گنتی = 0;
	cole -= wp->w_popup_leftoff;
	--سطریں;
	إذا (سطریں < 0)
	    سطریں = 0;
	لكل (سطر = سطریں; سطر < linee
		&& سطر + wp->w_winrow < screen_صفاں; ++سطر)
	    لكل (خانہ = خانے_گنتی; خانہ < cole
		    && خانہ + wp->w_wincol < screen_Columns; ++خانہ)
		popup_transparent[(سطر + wp->w_winrow) * screen_Columns
		    + خانہ + wp->w_wincol] = val;
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
	bool	    لبھیا = false;

	إذا ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    أرجع FALSE;
	إذا (لبھو_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    لبھیا = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	وإلا إذا (popup_لبھو_prop_above_چوٹی(wp, wp->w_popup_prop_win,
					    popup_اچائی(wp), &prop, &lnum))
	    لبھیا = true;
	إذا (لبھیا)
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
    إذا (wp->w_popup_آخری_changedtick != CHANGEDTICK(wp->w_بفر))
	أرجع TRUE;
    إذا (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_بفر)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_چوڑائی != wp->w_popup_prop_win->w_چوڑائی
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_اچائی))
	أرجع TRUE;

    // May need to adjust the width if the cursor moved.
    أرجع wp->w_cursor.lnum != wp->w_popup_آخری_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ساكن چھوٹا *opacity_zindex = عدم;
ساكن صحيح    opacity_zindex_صفاں = 0;
ساكن صحيح    opacity_zindex_خانے_گنتی = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ساكن فراغ
popup_mark_opacity_zindex(win_T *wp)
{
    صحيح	    چوڑائی;
    صحيح	    اچائی;
    صحيح	    r, c;

    إذا (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	أرجع;

    چوڑائی = popup_چوڑائی(wp);
    اچائی = popup_اچائی(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    لكل (r = wp->w_winrow;
		       r < wp->w_winrow + اچائی - wp->w_popup_topoff
						&& r < screen_صفاں; ++r)
	لكل (c = wp->w_wincol;
		 c < wp->w_wincol + چوڑائی - wp->w_popup_leftoff
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
    صحيح	    اچائی;
    صحيح	    چوڑائی;
    صحيح	    r;

    إذا (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	أرجع;

    اچائی = popup_اچائی(wp);
    چوڑائی = popup_چوڑائی(wp);
    لكل (r = wp->w_winrow;
		       r < wp->w_winrow + اچائی && r < screen_صفاں; ++r)
    {
	صحيح	    خانہ;
	win_T	    *prev_twp = عدم;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	لكل (خانہ = wp->w_wincol;
		خانہ < wp->w_wincol + چوڑائی && خانہ < screen_Columns; ++خانہ)
	{
	    صحيح	    سطر_cp = r;
	    صحيح	    خانہ_cp = خانہ;
	    win_T   *twp;

	    twp = mouse_لبھو_win(&سطر_cp, &خانہ_cp, IGNORE_POPUP);
	    إذا (twp != عدم && twp != prev_twp)
	    {
		prev_twp = twp;
		إذا (سطر_cp < twp->w_اچائی)
		{
		    linenr_T lnum;

#جے_تعریف_شدہ FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    إذا (رکن_do_update_کھڑکی(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    وإلا
#جے_انت
		    {
			(فراغ)mouse_comp_pos(twp, &سطر_cp, &خانہ_cp, &lnum,
									عدم);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		وإلا إذا (سطر_cp == twp->w_اچائی)
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
    صحيح
popup_is_under_opacity(صحيح صف, صحيح خانہ)
{
    إذا (opacity_zindex == عدم
	    || صف < 0 || صف >= opacity_zindex_صفاں
	    || خانہ < 0 || خانہ >= opacity_zindex_خانے_گنتی)
	أرجع FALSE;
    أرجع opacity_zindex[صف * opacity_zindex_خانے_گنتی + خانہ] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    صحيح
popup_is_over_opacity(صحيح صف, صحيح خانہ)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	إذا ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& صف >= wp->w_winrow
		&& صف < wp->w_winrow + popup_اچائی(wp)
		&& خانہ >= wp->w_wincol
		&& خانہ < wp->w_wincol + popup_چوڑائی(wp))
	    أرجع TRUE;
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& صف >= wp->w_winrow
		&& صف < wp->w_winrow + popup_اچائی(wp)
		&& خانہ >= wp->w_wincol
		&& خانہ < wp->w_wincol + popup_چوڑائی(wp))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    صحيح
popup_is_under_opacity_range(صحيح صف, صحيح شروع_خانہ, صحيح سرا_خانہ)
{
    صحيح خانہ;

    إذا (opacity_zindex == عدم
	    || صف < 0 || صف >= opacity_zindex_صفاں)
	أرجع FALSE;
    لكل (خانہ = شروع_خانہ; خانہ < سرا_خانہ && خانہ < opacity_zindex_خانے_گنتی; ++خانہ)
	إذا (opacity_zindex[صف * opacity_zindex_خانے_گنتی + خانہ] > screen_zindex)
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
may_update_popup_نقاب(صحيح type)
{
    win_T	*wp;
    چھوٹا	*نقاب;
    صحيح		سطر, خانہ;
    صحيح		redraw_all_popups = FALSE;
    صحيح		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    إذا (popup_نقاب_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_نقاب_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_popup_flags & POPF_HIDDEN)
	    popup_نقاب_refresh |= check_popup_unhidden(wp);
	وإلا إذا (popup_need_position_adjust(wp))
	    popup_نقاب_refresh = TRUE;
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_popup_flags & POPF_HIDDEN)
	    popup_نقاب_refresh |= check_popup_unhidden(wp);
	وإلا إذا (popup_need_position_adjust(wp))
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
	صحيح has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    إذا ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
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
	    opacity_zindex_صفاں = 0;
	    opacity_zindex_خانے_گنتی = 0;
	}
	وإلا
	{
	    إذا (opacity_zindex_صفاں != screen_صفاں
		    || opacity_zindex_خانے_گنتی != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(چھوٹا,
					screen_صفاں * screen_Columns);
		opacity_zindex_صفاں = screen_صفاں;
		opacity_zindex_خانے_گنتی = screen_Columns;
	    }
	    إذا (opacity_zindex != عدم)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_صفاں * screen_Columns * حجم(چھوٹا));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    إذا (!popup_نقاب_refresh)
	أرجع;

    // Need to update the mask, something has changed.
    popup_نقاب_refresh = FALSE;
    popup_نقاب_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    إذا (redrawing_all_win)
	نقاب = popup_نقاب;
    وإلا
	نقاب = popup_نقاب_اگلا;
    vim_memset(نقاب, 0, (size_t)screen_صفاں * screen_Columns * حجم(چھوٹا));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    طالما ((wp = لبھو_اگلا_popup(TRUE, POPUP_HANDLED_4)) != عدم)
    {
	صحيح چوڑائی;
	صحيح اچائی;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	إذا (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    إذا (wp->w_popup_flags & POPF_HIDDEN)
		استمر;
	}

	چوڑائی = popup_چوڑائی(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    اچائی = cl.eff_اچائی;
	}
	وإلا
	    اچائی = popup_اچائی(wp);
	popup_update_نقاب(wp, چوڑائی, اچائی);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	إذا ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    استمر;

	{
	    صحيح نقاب_شروع = wp->w_winrow;
	    صحيح نقاب_سرا = نقاب_شروع + اچائی;
	    صحيح نقاب_خانہ_شروع = wp->w_wincol + wp->w_popup_leftclip;
	    صحيح نقاب_خانہ_سرا = wp->w_wincol + چوڑائی - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    لكل (سطر = نقاب_شروع;
			    سطر < نقاب_سرا && سطر < screen_صفاں; ++سطر)
		لكل (خانہ = نقاب_خانہ_شروع;
		     خانہ < نقاب_خانہ_سرا && خانہ < screen_Columns; ++خانہ)
		    إذا (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(سطر, خانہ, FALSE))
			نقاب[سطر * screen_Columns + خانہ] = POPUPMENU_ZINDEX;
		    وإلا إذا (wp->w_popup_نقاب_cells == عدم
				    || !popup_masked(wp, چوڑائی, اچائی, خانہ, سطر))
			نقاب[سطر * screen_Columns + خانہ] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    إذا (نقاب == popup_نقاب_اگلا)
    {
	صحيح	    *plines_cache = ALLOC_CLEAR_MULT(صحيح, صفاں);
	win_T	    *prev_wp = عدم;

	لكل (سطر = 0; سطر < screen_صفاں; ++سطر)
	{
	    صحيح	    خانہ_پورا = 0;

	    لكل (خانہ = 0; خانہ < screen_Columns; ++خانہ)
	    {
		صحيح off = سطر * screen_Columns + خانہ;

		إذا (popup_نقاب[off] != popup_نقاب_اگلا[off])
		{
		    popup_نقاب[off] = popup_نقاب_اگلا[off];

		    إذا (سطر >= cmdline_صف)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			إذا (!پیغام_scrolled && popup_نقاب_اگلا[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    وإلا إذا (خانہ >= خانہ_پورا)
		    {
			linenr_T	lnum;
			صحيح		سطر_cp = سطر;
			صحيح		خانہ_cp = خانہ;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_لبھو_win(&سطر_cp, &خانہ_cp, IGNORE_POPUP);
			إذا (wp != عدم)
			{
#إذا defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    إذا (bt_terminal(wp->w_بفر))
				redraw_win_later(wp, UPD_NOT_VALID);
			    وإلا
#جے_انت
			    {
				إذا (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   حجم(صحيح) * صفاں);
				    prev_wp = wp;
				}

				إذا (سطر_cp >= wp->w_اچائی)
				    // In (or below) status line
				    wp->w_redr_حالت = true;
				وإلا
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (فراغ)mouse_comp_pos(wp, &سطر_cp, &خانہ_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    خانہ_پورا = wp->w_wincol + wp->w_چوڑائی - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_ورتدا_mouse_move();
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

#جے_تعریف_شدہ FEAT_PROP_POPUP
ساكن schar_T *ادھار_screenlines = عدم;
ساكن صحيح *ادھار_screenattrs = عدم;
ساكن u8char_T *ادھار_screenlinesuc = عدم;
ساكن صحيح ادھار_screen_صفاں = 0;
ساكن صحيح ادھار_screen_خانے_گنتی = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    صحيح
popup_get_ادھار_screen_cell(صحيح صف, صحيح خانہ, schar_T *linep, صحيح *attrp,
							  u8char_T *ucp)
{
    إذا (ادھار_screenlines == عدم || ادھار_screenattrs == عدم)
	أرجع FALSE;
    إذا (صف < 0 || خانہ < 0 || صف >= ادھار_screen_صفاں
						     || خانہ >= ادھار_screen_خانے_گنتی)
	أرجع FALSE;

    صحيح off = صف * ادھار_screen_خانے_گنتی + خانہ;
    إذا (linep != عدم)
	*linep = ادھار_screenlines[off];
    إذا (attrp != عدم)
	*attrp = ادھار_screenattrs[off];
    إذا (ucp != عدم)
    {
	إذا (enc_utf8 && ادھار_screenlinesuc != عدم)
	    *ucp = ادھار_screenlinesuc[off];
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
popup_سیٹ_ادھار_screen_cell(صحيح صف, صحيح خانہ, schar_T سطر, صحيح attr, u8char_T uc)
{
    إذا (ادھار_screenlines == عدم || ادھار_screenattrs == عدم)
	أرجع;
    إذا (صف < 0 || خانہ < 0 || صف >= ادھار_screen_صفاں
						     || خانہ >= ادھار_screen_خانے_گنتی)
	أرجع;

    صحيح off = صف * ادھار_screen_خانے_گنتی + خانہ;
    ادھار_screenlines[off] = سطر;
    ادھار_screenattrs[off] = attr;
    إذا (enc_utf8 && ادھار_screenlinesuc != عدم)
	ادھار_screenlinesuc[off] = uc;
}
#جے_انت

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ساكن فراغ
draw_opacity_padding_cell(
	صحيح		صف,
	صحيح		خانہ,
	popup_saved_screen_T *saved_screen,
	صحيح		pad_شروع_خانہ,
	صحيح		pad_سرا_خانہ)
{
    إذا (سطر_وتھ == عدم || Screen_سطریں == عدم || ScreenAttrs == عدم
	    || (enc_utf8 && Screen_سطریں_UC == عدم))
	أرجع;

    صحيح off = سطر_وتھ[صف] + خانہ;
    صحيح r = صف - saved_screen->شروع_صف;
    صحيح c = خانہ - saved_screen->شروع_خانہ;

    إذا (r >= 0 && r < saved_screen->صفاں_2 && c >= 0 && c < saved_screen->خانے_گنتی)
    {
	صحيح save_off = r * saved_screen->خانے_گنتی + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	إذا (enc_utf8 && saved_screen->linesuc != عدم)
	{
	    صحيح ادھار_خانہ = خانہ - 1;
	    صحيح ادھار_off = off - 1;
	    صحيح ادھار_save_off = save_off - 1;
	    صحيح wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    إذا (ادھار_off >= 0)
	    {
		إذا (Screen_سطریں_UC[ادھار_off] != 0
			&& utf_char2cells(Screen_سطریں_UC[ادھار_off]) == 2
			&& Screen_سطریں[off] == 0)
		    wide_prev = TRUE;
	    }
	    إذا (!wide_prev && save_off > 0)
	    {
		إذا (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->سطریں[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    إذا (wide_prev && ادھار_خانہ >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		إذا (ادھار_خانہ < pad_شروع_خانہ)
		{
		    إذا (Screen_سطریں_UC[ادھار_off] != 0
			    && utf_char2cells(Screen_سطریں_UC[ادھار_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_سطریں[ادھار_off] = ' ';
			Screen_سطریں_UC[ادھار_off] = 0;
			ScreenAttrs[ادھار_off] = saved_screen->attrs[ادھار_save_off];
			popup_سیٹ_ادھار_screen_cell(صف, ادھار_خانہ,
				Screen_سطریں[ادھار_off],
				ScreenAttrs[ادھار_off],
				Screen_سطریں_UC[ادھار_off]);
			screen_char(ادھار_off, صف, ادھار_خانہ);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_سطریں[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[ادھار_save_off];
			إذا (enc_utf8)
			    Screen_سطریں_UC[off] = 0;
			صحيح popup_attr_val =
					get_win_attr(screen_opacity_popup);
			صحيح blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_سیٹ_ادھار_screen_cell(صف, خانہ,
				Screen_سطریں[off], ScreenAttrs[off],
				Screen_سطریں_UC[off]);
			screen_char(off, صف, خانہ);
			أرجع;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    إذا (ادھار_save_off >= 0
			    && saved_screen->linesuc[ادھار_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[ادھار_save_off]) == 2
			    && Screen_سطریں[ادھار_off] == ' '
			    && Screen_سطریں_UC[ادھار_off] == 0)
		    {
			صحيح popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			صحيح blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_سطریں[ادھار_off] =
				    saved_screen->سطریں[ادھار_save_off];
			Screen_سطریں_UC[ادھار_off] =
				    saved_screen->linesuc[ادھار_save_off];
			ScreenAttrs[ادھار_off] =
				    saved_screen->attrs[ادھار_save_off];
			ScreenAttrs[ادھار_off] = hl_blend_attr(
				    ScreenAttrs[ادھار_off],
				    popup_attr_val, blend, TRUE);

			Screen_سطریں[off] = 0;
			Screen_سطریں_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[ادھار_off];

			popup_سیٹ_ادھار_screen_cell(صف, ادھار_خانہ,
				    Screen_سطریں[ادھار_off],
				    ScreenAttrs[ادھار_off],
				    Screen_سطریں_UC[ادھار_off]);
			popup_سیٹ_ادھار_screen_cell(صف, خانہ,
				    Screen_سطریں[off],
				    ScreenAttrs[off],
				    Screen_سطریں_UC[off]);
			screen_char(ادھار_off, صف, ادھار_خانہ);
			أرجع;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_سطریں[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[ادھار_save_off];
		    إذا (enc_utf8)
			Screen_سطریں_UC[off] = 0;
		    صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحيح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_سیٹ_ادھار_screen_cell(صف, خانہ, Screen_سطریں[off],
					       ScreenAttrs[off], Screen_سطریں_UC[off]);
		    screen_char(off, صف, خانہ);
		    أرجع;
		}

		// Base cell is inside the saved area, redraw the wide char.
		إذا (save_off > 0)
		{
		    Screen_سطریں[ادھار_off] = saved_screen->سطریں[ادھار_save_off];
		    ScreenAttrs[ادھار_off] = saved_screen->attrs[ادھار_save_off];
		    Screen_سطریں[off] = saved_screen->سطریں[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_سطریں_UC[ادھار_off] = saved_screen->linesuc[ادھار_save_off];
		    Screen_سطریں_UC[off] = saved_screen->linesuc[save_off];

		    صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحيح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[ادھار_off] = hl_blend_attr(ScreenAttrs[ادھار_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[ادھار_off];
		    popup_سیٹ_ادھار_screen_cell(صف, ادھار_خانہ, Screen_سطریں[ادھار_off],
					       ScreenAttrs[ادھار_off], Screen_سطریں_UC[ادھار_off]);
		    popup_سیٹ_ادھار_screen_cell(صف, خانہ, Screen_سطریں[off],
					       ScreenAttrs[off], Screen_سطریں_UC[off]);
		    screen_char(ادھار_off, صف, ادھار_خانہ);
		}
		أرجع;
	    }
	}
	Screen_سطریں[off] = saved_screen->سطریں[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	إذا (enc_utf8)
	    Screen_سطریں_UC[off] = saved_screen->linesuc != عدم
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	إذا (enc_utf8 && Screen_سطریں_UC[off] != 0
		&& utf_char2cells(Screen_سطریں_UC[off]) == 2
		&& خانہ + 1 >= pad_سرا_خانہ)
	{
	    Screen_سطریں[off] = ' ';
	    Screen_سطریں_UC[off] = 0;
	}

	صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
	صحيح blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_سیٹ_ادھار_screen_cell(صف, خانہ, Screen_سطریں[off],
				   ScreenAttrs[off], Screen_سطریں_UC[off]);
	screen_char(off, صف, خانہ);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ساكن فراغ
fill_opacity_padding(
	صحيح		شروع_صف,
	صحيح		سرا_صف,
	صحيح		شروع_خانہ,
	صحيح		سرا_خانہ,
	popup_saved_screen_T *saved_screen)
{
    لكل (صحيح pad_صف = شروع_صف; pad_صف < سرا_صف; pad_صف++)
	لكل (صحيح pad_خانہ = شروع_خانہ; pad_خانہ < سرا_خانہ; pad_خانہ++)
	    draw_opacity_padding_cell(pad_صف, pad_خانہ, saved_screen,
		    شروع_خانہ, سرا_خانہ);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    فراغ
update_popups(فراغ (*win_update)(win_T *wp))
{
    win_T   *wp;
    صحيح	    چوٹی_off;
    صحيح	    بایاں_extra;
    صحيح	    کل_چوڑائی;
    صحيح	    کل_اچائی_2;
    صحيح	    چوٹی_padding;
    صحيح	    popup_attr;
    صحيح	    border_attr[4];
    صحيح	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    صحيح	    صف;
    صحيح	    wincol;
    صحيح	    padcol = 0;
    صحيح	    padendcol = 0;
    صحيح	    i;
    صحيح	    sb_thumb_چوٹی = 0;
    صحيح	    sb_thumb_اچائی = 0;
    صحيح	    attr_scroll = 0;
    صحيح	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (ادھار_screenlines != عدم)
    {
	vim_free(ادھار_screenlines);
	ادھار_screenlines = عدم;
    }
    إذا (ادھار_screenattrs != عدم)
    {
	vim_free(ادھار_screenattrs);
	ادھار_screenattrs = عدم;
    }
    إذا (ادھار_screenlinesuc != عدم)
    {
	vim_free(ادھار_screenlinesuc);
	ادھار_screenlinesuc = عدم;
    }
    ادھار_screen_صفاں = 0;
    ادھار_screen_خانے_گنتی = 0;
#جے_انت

    طالما ((wp = لبھو_اگلا_popup(TRUE, POPUP_HANDLED_5)) != عدم)
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

	override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

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

#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (screen_opacity_popup != عدم)
	{
	    إذا (ادھار_screenlines != عدم)
	    {
		vim_free(ادھار_screenlines);
		ادھار_screenlines = عدم;
	    }
	    إذا (ادھار_screenattrs != عدم)
	    {
		vim_free(ادھار_screenattrs);
		ادھار_screenattrs = عدم;
	    }
	    إذا (ادھار_screenlinesuc != عدم)
	    {
		vim_free(ادھار_screenlinesuc);
		ادھار_screenlinesuc = عدم;
	    }

	    ادھار_screen_صفاں = screen_صفاں;
	    ادھار_screen_خانے_گنتی = screen_Columns;
	    ادھار_screenlines = ALLOC_MULT(schar_T,
				    ادھار_screen_صفاں * ادھار_screen_خانے_گنتی);
	    ادھار_screenattrs = ALLOC_MULT(صحيح,
				    ادھار_screen_صفاں * ادھار_screen_خانے_گنتی);
	    إذا (enc_utf8)
		ادھار_screenlinesuc = ALLOC_MULT(u8char_T,
				    ادھار_screen_صفاں * ادھار_screen_خانے_گنتی);

	    إذا (ادھار_screenlines != عدم && ادھار_screenattrs != عدم)
	    {
		لكل (صحيح r = 0; r < ادھار_screen_صفاں; r++)
		{
		    صحيح off = سطر_وتھ[r];
		    صحيح ادھار_off = r * ادھار_screen_خانے_گنتی;
		    لكل (صحيح c = 0; c < ادھار_screen_خانے_گنتی; c++)
		    {
			ادھار_screenlines[ادھار_off + c] = Screen_سطریں[off + c];
			ادھار_screenattrs[ادھار_off + c] = ScreenAttrs[off + c];
			إذا (enc_utf8 && ادھار_screenlinesuc != عدم)
			    ادھار_screenlinesuc[ادھار_off + c] =
					Screen_سطریں_UC[off + c];
		    }
		}
	    }
	}
#جے_انت

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
	إذا (wp->w_wincol + بایاں_extra < 0)
	    بایاں_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(چوٹی_off - wp->w_popup_topoff, 0);
	wp->w_wincol += بایاں_extra;

	// Draw the popup text, unless it's off screen.
	إذا (wp->w_winrow < screen_صفاں && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    إذا (wp->w_popup_آخری_curline != wp->w_cursor.lnum)
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

	wp->w_winrow -= MAX(چوٹی_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= بایاں_extra;

	// Add offset for border and padding if not done already.
	إذا ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += بایاں_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	إذا ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
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
	إذا (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    کل_چوڑائی = cl.eff_چوڑائی;
	وإلا
	    کل_چوڑائی = popup_چوڑائی(wp) - wp->w_popup_rightoff;
	إذا (کل_چوڑائی < 0)
	    کل_چوڑائی = 0;
	إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    کل_اچائی_2 = cl.eff_اچائی;
	وإلا
	    کل_اچائی_2 = popup_اچائی(wp);
	popup_attr = get_win_attr(wp);

	إذا (wp->w_winrow + کل_اچائی_2 > cmdline_صف)
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
	    إذا (title_len > کل_چوڑائی - 2)
	    {
		صحيح	title_byte_len = (صحيح)STRLEN(wp->w_popup_title);
		char_u  *title_متن = alloc(title_byte_len + 1);

		إذا (title_متن != عدم)
		{
		    trunc_string(wp->w_popup_title, title_متن,
					  کل_چوڑائی - 2, title_byte_len + 1);
		    screen_puts(title_متن, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_متن);
		}

		title_len = کل_چوڑائی - 2;
	    }
	    وإلا
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	چوٹی_padding = cl.eff_padding[0];
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
			title_wincol + title_len, wincol + کل_چوڑائی,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    وإلا
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + کل_چوڑائی,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + کل_چوڑائی - 1, border_attr[1]);
	    }
	}
	وإلا إذا (cl.eff_padding[0] == 0 && popup_چوٹی_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    چوٹی_padding = 1;

	إذا (چوٹی_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + کل_چوڑائی - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    إذا (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	إذا (چوٹی_padding > 0)
	{
	    صف = wp->w_winrow + cl.eff_border[0];
	    إذا (title_len > 0 && صف == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		إذا (screen_opacity_popup != عدم && saved_screen.سطریں != عدم)
		{
		    // Left of title
		    fill_opacity_padding(صف, صف + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(صف, صف + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		وإلا
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
	    إذا (screen_opacity_popup != عدم && saved_screen.سطریں != عدم)
	    {
		fill_opacity_padding(صف, صف + چوٹی_padding, padcol, padendcol,
			&saved_screen);
	    }
	    وإلا
	    {
		screen_fill(صف, صف + چوٹی_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	إذا (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_بفر->b_ml.ml_سطر_گنتی;
	    صحيح		اچائی = wp->w_اچائی;
	    صحيح		آخری;

	    sb_thumb_اچائی = ((linenr_T)اچائی * اچائی + linecount / 2)
								   / linecount;
	    إذا (wp->w_topline > 1 && sb_thumb_اچائی == اچائی)
		--sb_thumb_اچائی;  // scrolled, no full thumb
	    إذا (sb_thumb_اچائی == 0)
		sb_thumb_اچائی = 1;
	    إذا (linecount <= wp->w_اچائی || wp->w_اچائی == 0)
		// it just fits, avoid divide by zero
		sb_thumb_چوٹی = 0;
	    وإلا
		sb_thumb_چوٹی = (wp->w_topline - 1
				+ (linecount / wp->w_اچائی) / 2)
				* (wp->w_اچائی - sb_thumb_اچائی)
						  / (linecount - wp->w_اچائی);
	    إذا (wp->w_topline > 1 && sb_thumb_چوٹی == 0 && اچائی > 1)
		sb_thumb_چوٹی = 1;  // show it's scrolled
	    آخری = کل_اچائی_2 - چوٹی_off - wp->w_popup_border[2];
	    إذا (sb_thumb_چوٹی >= آخری)
		// show at least one character
		sb_thumb_چوٹی = آخری - 1;

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
	لكل (i = cl.eff_border[0]; i < کل_اچائی_2 - cl.eff_border[2]; ++i)
	{
	    صحيح	pad_بایاں;
	    // left and right padding only needed next to the body
	    صحيح do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < کل_اچائی_2 - cl.eff_border[2] - cl.eff_padding[2];

	    صف = wp->w_winrow + i;

	    // left border
	    إذا (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, صف, wincol, border_attr[3]);
	    }
	    إذا (do_padding && cl.eff_padding[3] > 0)
	    {
		صحيح خانہ = wincol + wp->w_popup_border[3];

		// left padding
		pad_بایاں = wp->w_popup_padding[3];
		إذا (خانہ < 0)
		{
		    pad_بایاں += خانہ;
		    خانہ = 0;
		}
		إذا (pad_بایاں > 0)
		{
		    إذا (screen_opacity_popup != عدم && saved_screen.سطریں != عدم)
			fill_opacity_padding(صف, صف + 1, خانہ, خانہ + pad_بایاں,
				&saved_screen);
		    وإلا
			screen_fill(صف, صف + 1, خانہ, خانہ + pad_بایاں,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    إذا (wp->w_has_scrollbar)
	    {
		صحيح سطر = i - چوٹی_off;
		صحيح scroll_خانہ = wp->w_wincol + کل_چوڑائی - 1
						       - wp->w_popup_border[1];

		إذا (سطر >= 0 && سطر < wp->w_اچائی)
		    screen_putchar(' ', صف, scroll_خانہ,
			    سطر >= sb_thumb_چوٹی
				       && سطر < sb_thumb_چوٹی + sb_thumb_اچائی
						  ? attr_thumb : attr_scroll);
		وإلا
		    screen_putchar(' ', صف, scroll_خانہ, popup_attr);
	    }
	    // right border
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, صف, wincol + کل_چوڑائی - 1, border_attr[1]);
	    }
	    // right padding
	    إذا (do_padding && cl.eff_padding[1] > 0)
	    {
		صحيح pad_خانہ_شروع = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_چوڑائی;
		صحيح pad_خانہ_سرا = pad_خانہ_شروع + wp->w_popup_padding[1];

		إذا (screen_opacity_popup != عدم && saved_screen.سطریں != عدم)
		    fill_opacity_padding(صف, صف + 1, pad_خانہ_شروع, pad_خانہ_سرا,
			    &saved_screen);
		وإلا
		    screen_fill(صف, صف + 1, pad_خانہ_شروع, pad_خانہ_سرا,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	إذا (wp->w_popup_shadow)
	{
	    صحيح خانہ = wincol + کل_چوڑائی;
	    لكل (i = 0; i < کل_اچائی_2; ++i)
	    {
		صف = wp->w_winrow + i + 1;
		put_shadow_char(صف, خانہ);
		put_shadow_char(صف, خانہ + 1);
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
	    صف = wp->w_winrow + کل_اچائی_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    إذا (screen_opacity_popup != عدم && saved_screen.سطریں != عدم)
		fill_opacity_padding(صف, صف + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    وإلا
		screen_fill(صف, صف + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	إذا (cl.eff_border[2] > 0)
	{
	    // bottom border
	    صف = wp->w_winrow + کل_اچائی_2 - 1;
	    screen_fill(صف, صف + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + کل_چوڑائی,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, صف, wincol + کل_چوڑائی - 1, border_attr[2]);
	    }
	}

	إذا (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    صف = wp->w_winrow + کل_اچائی_2;
	    لكل (صحيح خانہ = 2 + (wincol < 0 ? 0 : wincol);
		    خانہ < wincol + کل_چوڑائی; خانہ++)
		put_shadow_char(صف, خانہ);
	}

	إذا (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + کل_چوڑائی - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = عدم;

	// Back to the normal zindex.
	screen_zindex = 0;

#جے_تعریف_شدہ HAS_سنیہا_کھڑکی
	// if this was the message window popup may start the timer now
	may_شروع_سنیہا_win_timer(wp);
#جے_انت

	إذا (override_success)
	    نکالو_highlight_overrides();

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
	popup_position_image(wp);
#جے_انت
    }

#جے_تعریف_شدہ FEAT_PROP_POPUP
    VIM_CLEAR(ادھار_screenlines);
    VIM_CLEAR(ادھار_screenattrs);
    VIM_CLEAR(ادھار_screenlinesuc);
    ادھار_screen_صفاں = 0;
    ادھار_screen_خانے_گنتی = 0;
#جے_انت

#إذا defined(FEAT_تلاش_EXTRA)
    // In case win_update() called start_search_hl().
    سرا_تلاش_hl();
#جے_انت
}

/*
 * Mark references in callbacks of one popup window.
 */
    ساكن صحيح
سیٹ_ref_وچ_one_popup(win_T *wp, صحيح copyID)
{
    صحيح		abort = FALSE;
    typval_T	tv;

    إذا (wp->w_close_cb.cb_partial != عدم)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || سیٹ_ref_وچ_شے(&tv, copyID, عدم, عدم, عدم);
    }
    إذا (wp->w_filter_cb.cb_partial != عدم)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || سیٹ_ref_وچ_شے(&tv, copyID, عدم, عدم, عدم);
    }
    abort = abort || سیٹ_ref_وچ_list(wp->w_popup_نقاب, copyID);
    أرجع abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    صحيح
سیٹ_ref_وچ_popups(صحيح copyID)
{
    صحيح		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    لكل (wp = پہلا_popupwin; !abort && wp != عدم; wp = wp->w_اگلا)
	abort = abort || سیٹ_ref_وچ_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	لكل (wp = tp->tp_پہلا_popupwin; !abort && wp != عدم; wp = wp->w_اگلا)
	    abort = abort || سیٹ_ref_وچ_one_popup(wp, copyID);
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
popup_لبھو_preview_کھڑکی(فراغ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_p_pvw)
	    أرجع wp;
    أرجع عدم;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_لبھو_info_کھڑکی(فراغ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_popup_flags & POPF_INFO)
	    أرجع wp;
    أرجع عدم;
}
#جے_انت

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(صحيح dir, bool by_ورقہ)
{
#جے_تعریف_شدہ FEAT_QUICKFIX
    win_T	*wp = popup_لبھو_info_کھڑکی();
    صحيح		by;
    linenr_T	new_topline;

    إذا (wp == عدم)
	أرجع false;

    by = by_ورقہ ? (wp->w_اچائی > 2 ? wp->w_اچائی - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    إذا (new_topline < 1)
	new_topline = 1;
    إذا (new_topline > wp->w_بفر->b_ml.ml_سطر_گنتی)
	new_topline = wp->w_بفر->b_ml.ml_سطر_گنتی;
    إذا (new_topline != wp->w_topline)
    {
	سیٹ_topline(wp, new_topline);
	popup_سیٹ_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    أرجع true;
#وإلا
    أرجع false;
#جے_انت
}

    فراغ
f_popup_findecho(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
#جے_تعریف_شدہ HAS_سنیہا_کھڑکی
    rettv->vval.v_نمبر = سنیہا_win == عدم ? 0 : سنیہا_win->w_id;
#وإلا
    rettv->vval.v_نمبر = 0;
#جے_انت
}

    فراغ
f_popup_findinfo(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
#جے_تعریف_شدہ FEAT_QUICKFIX
    win_T   *wp = popup_لبھو_info_کھڑکی();

    rettv->vval.v_نمبر = wp == عدم ? 0 : wp->w_id;
#وإلا
    rettv->vval.v_نمبر = 0;
#جے_انت
}

    فراغ
f_popup_findpreview(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
#جے_تعریف_شدہ FEAT_QUICKFIX
    win_T   *wp = popup_لبھو_preview_کھڑکی();

    rettv->vval.v_نمبر = wp == عدم ? 0 : wp->w_id;
#وإلا
    rettv->vval.v_نمبر = 0;
#جے_انت
}

#إذا defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    صحيح
popup_create_preview_کھڑکی(صحيح info)
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
	wp->w_چوڑائی = wp->w_minwidth;
    وإلا إذا (wp->w_maxwidth > 0)
	wp->w_چوڑائی = wp->w_maxwidth;
    وإلا
	wp->w_چوڑائی = curwin->w_چوڑائی;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_بفر->b_locked = FALSE;

    win_enter(wp, FALSE);
    أرجع OK;
}

/*
 * Close any preview popup.
 */
    فراغ
popup_close_preview(فراغ)
{
    win_T *wp = popup_لبھو_preview_کھڑکی();

    إذا (wp != عدم)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    فراغ
popup_hide_info(فراغ)
{
    win_T *wp = popup_لبھو_info_کھڑکی();

    إذا (wp != عدم)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	إذا (State & منوال_CMDLINE)
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
    win_T *wp = popup_لبھو_info_کھڑکی();

    إذا (wp != عدم)
	popup_close_with_retval(wp, -1);
}
#جے_انت

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    صحيح
popup_overlaps_cmdline(فراغ)
{
    أرجع popup_on_cmdline;
}

#إذا defined(HAS_سنیہا_کھڑکی)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_سنیہا_win(فراغ)
{
    إذا (سنیہا_win != عدم)
	أرجع سنیہا_win;

    صحيح i;

    سنیہا_win = popup_create(عدم, عدم, TYPE_سنیہا_WIN);

    إذا (سنیہا_win == عدم)
	أرجع عدم;

    // use the full screen width
    سنیہا_win->w_چوڑائی = Columns;

    // position at bottom of screen
    سنیہا_win->w_popup_pos = POPPOS_BOTTOM;
    سنیہا_win->w_wantcol = 1;
    سنیہا_win->w_minwidth = 9999;
    سنیہا_win->w_firstline = -1;

    // no padding, border at the top
    لكل (i = 0; i < 4; ++i)
	سنیہا_win->w_popup_padding[i] = 0;
    لكل (i = 1; i < 4; ++i)
	سنیہا_win->w_popup_border[i] = 0;

    إذا (سنیہا_win->w_popup_timer != عدم)
	سنیہا_win->w_popup_timer->tr_keep = TRUE;
    أرجع سنیہا_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    فراغ
popup_show_سنیہا_win(فراغ)
{
    إذا (سنیہا_win == عدم)
	أرجع;

    إذا ((سنیہا_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(سنیہا_win, TYPE_سنیہا_WIN);
	popup_show(سنیہا_win);
    }
    شروع_سنیہا_win_timer = TRUE;
}

    ساكن فراغ
may_شروع_سنیہا_win_timer(win_T *wp)
{
    إذا (wp == سنیہا_win && شروع_سنیہا_win_timer)
    {
	إذا (سنیہا_win->w_popup_timer != عدم)
	{
	    سنیہا_win->w_popup_timer->tr_interval = سنیہا_win_ویلا;
	    timer_شروع(سنیہا_win->w_popup_timer);
	    سنیہا_win_ویلا = 3000;
	}
	شروع_سنیہا_win_timer = FALSE;
    }
}

    صحيح
popup_سنیہا_win_visible(فراغ)
{
    أرجع سنیہا_win != عدم
	&& (سنیہا_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    فراغ
popup_hide_سنیہا_win(فراغ)
{
    إذا (سنیہا_win != عدم)
	popup_hide(سنیہا_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ساكن صحيح save_پیغام_didout = FALSE;
ساكن صحيح save_پیغام_خانہ = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ساكن صحيح ew_پیغام_didout = FALSE;
ساكن صحيح ew_پیغام_خانہ = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    فراغ
شروع_echowindow(صحيح ویلا_sec)
{
    وچ_echowindow = TRUE;
    save_پیغام_didout = پیغام_didout;
    save_پیغام_خانہ = پیغام_خانہ;
    پیغام_didout = ew_پیغام_didout;
    پیغام_خانہ = ew_پیغام_خانہ;
    إذا (ویلا_sec != 0)
	سنیہا_win_ویلا = ویلا_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    فراغ
سرا_echowindow(فراغ)
{
    وچ_echowindow = FALSE;

    إذا ((State & منوال_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_پیغام_didout = TRUE;
    ew_پیغام_خانہ = پیغام_خانہ == 0 ? 1 : پیغام_خانہ;
    پیغام_didout = save_پیغام_didout;
    پیغام_خانہ = save_پیغام_خانہ;
}
#جے_انت

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    صحيح
popup_win_closed(win_T *win)
{
    صحيح	    گیڑا;
    win_T   *wp;
    win_T   *اگلا;
    صحيح	    ret = FALSE;

    لكل (گیڑا = 1; گیڑا <= 2; ++گیڑا)
	لكل (wp = گیڑا == 1 ? پہلا_popupwin : curtab->tp_پہلا_popupwin;
							 wp != عدم; wp = اگلا)
	{
	    اگلا = wp->w_اگلا;
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
popup_سیٹ_title(win_T *wp)
{
    إذا (wp->w_بفر->b_fname == عدم)
	أرجع;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_بفر, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_بفر->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    إذا (wp->w_popup_title != عدم)
	vim_snprintf((حرف *)wp->w_popup_title, len, " %s ",
		wp->w_بفر->b_fname);
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
    win_T *wp = popup_لبھو_preview_کھڑکی();

    إذا (wp != عدم)
	popup_سیٹ_title(wp);
}
#جے_انت

#جے_انت // FEAT_PROP_POPUP
