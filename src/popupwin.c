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

تعریفنوع ساختار {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
تعریفنوع ساختار {
    صحیح		active;
    صحیح		winrow;
    صحیح		wincol;
    صحیح		height;
    صحیح		width;
    صحیح		leftoff;
    صحیح		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
تعریفنوع ساختار {
    schar_T	*سطرها;
    صحیح		*attrs;
    u8char_T	*linesuc;
    صحیح		start_row;
    صحیح		start_col;
    صحیح		rows;
    صحیح		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
تعریفنوع ساختار {
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
تعریفنوع ساختار {
    صحیح		winrow;
    صحیح		wincol;
    صحیح		width;
    صحیح		height;
    صحیح		leftcol;
    صحیح		leftoff;
    صحیح		has_scrollbar;
    صحیح		topoff;
    صحیح		bottomoff;
    صحیح		leftclip;
    صحیح		rightclip;
} popup_layout_T;

ایستا poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
ایستا win_T *message_win = پوچ;

// Time used for the next ":echowindow" message in msec.
ایستا صحیح  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ایستا صحیح    start_message_win_timer = FALSE;

ایستا تهی may_start_message_win_timer(win_T *wp);
#endif

ایستا صحیح popup_on_cmdline = FALSE;

ایستا تهی popup_adjust_position(win_T *wp);
ایستا bool popup_area_changed(win_T *wp, popup_area_T *area);
ایستا تهی popup_redraw_exposed_area(popup_area_T *area);
ایستا تهی popup_save_area(win_T *wp, popup_area_T *area);
ایستا تهی popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ایستا تهی popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ایستا bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
ایستا bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
ایستا تهی popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ایستا تهی popup_save_layout(win_T *wp, popup_layout_T *layout);
ایستا تهی redraw_under_popup_area(صحیح winrow, صحیح wincol, صحیح height,
	صحیح width, صحیح leftoff);
ایستا تهی redraw_overlapped_opacity_popups(صحیح winrow, صحیح wincol,
	صحیح height, صحیح width, صحیح leftoff, صحیح zindex);
ایستا تهی redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ایستا صحیح
popup_options_one(dict_T *dict, char_u *کلید)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    صحیح		n = 0;

    di = dict_find(dict, کلید, -1);
    اگر (di == پوچ)
	بازگشت MAXCOL;

    val = tv_get_string(&di->di_tv);
    اگر (STRNCMP(val, "cursor", 6) != 0)
	بازگشت dict_get_عدد_check(dict, کلید);

    setcursor_mayforce(TRUE);
    s = val + 6;
    اگر (*s != NUL)
    {
	endp = s;
	اگر (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((نویسه *)s, (نویسه **)&endp, 10);
	اگر (endp != پوچ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    بازگشت 0;
	}
    }

    اگر (STRCMP(کلید, "line") == 0)
	n = screen_screenrow() + 1 + n;
    وگرنه // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    اگر (n == 0)
	n = -1;
    بازگشت n;
}

    ایستا صحیح
set_padding_border(dict_T *dict, صحیح *array, نویسه *name, صحیح max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    اگر (di == پوچ)
	بازگشت OK;

    اگر (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	بازگشت FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    صحیح		i;
    صحیح		nr;

    برای (i = 0; i < 4; ++i)
	array[i] = 1;
    اگر (list == پوچ)
	بازگشت OK;

    CHECK_LIST_MATERIALIZE(list);
    برای (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (صحیح)tv_get_عدد(&li->li_tv);
	اگر (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    بازگشت OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ایستا تهی
set_moved_مقادیر(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ایستا تهی
set_moved_columns(win_T *wp, صحیح flags)
{
    char_u	*ptr;
    صحیح		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    صحیح		mincol;

    اگر (len <= 0)
	بازگشت;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (صحیح)(ptr - ml_get_curline());
    اگر (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	بازگشت;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ایستا تهی
set_mousemoved_مقادیر(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    ایستا تهی
update_popup_uses_mouse_move(تهی)
{
    popup_uses_mouse_move = FALSE;
    اگر (!popup_visible)
	بازگشت;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    بازگشت;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    بازگشت;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ایستا تهی
set_mousemoved_columns(win_T *wp, صحیح flags)
{
    win_T	*textwp;
    char_u	*text;
    صحیح		col;
    pos_T	pos;
    colnr_T	mcol;

    اگر (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, پوچ, &col) != OK)
	بازگشت;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, پوچ, پوچ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, پوچ, پوچ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    صحیح
popup_on_border(win_T *wp, صحیح row, صحیح col)
{
    بازگشت (row == 0 && wp->w_popup_border[0] > 0)
	    || (row == popup_height(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (col == 0 && wp->w_popup_border[3] > 0)
	    || (col == popup_width(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    صحیح
popup_close_if_on_X(win_T *wp, صحیح row, صحیح col)
{
    اگر (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

// Values set when dragging a popup window starts.
ایستا صحیح drag_start_row;
ایستا صحیح drag_start_col;
ایستا صحیح drag_start_wantline;
ایستا صحیح drag_start_wantcol;
ایستا صحیح drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    تهی
popup_start_drag(win_T *wp, صحیح row, صحیح col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    اگر (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    وگرنه
	drag_start_wantline = wp->w_wantline;
    اگر (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    وگرنه
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    اگر (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

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
    تهی
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    اگر (!win_valid_popup(wp))
	بازگشت;

    اگر ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	صحیح width_inc = mouse_col - drag_start_col;
	صحیح height_inc = mouse_row - drag_start_row;

	اگر (width_inc != 0)
	{
	    صحیح width = wp->w_width + width_inc;

	    اگر (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	اگر (height_inc != 0)
	{
	    صحیح height = wp->w_height + height_inc;

	    اگر (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	بازگشت;
    }

    اگر (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	بازگشت;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    اگر (wp->w_wantline < 1)
	wp->w_wantline = 1;
    اگر (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    اگر (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    اگر (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    تهی
popup_set_firstline(win_T *wp)
{
    صحیح	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    وقتی (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    صحیح
popup_is_in_scrollbar(win_T *wp, صحیح row, صحیح col)
{
    بازگشت wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    تهی
popup_handle_scrollbar_click(win_T *wp, صحیح row, صحیح col)
{
    اگر (!popup_is_in_scrollbar(wp, row, col))
	بازگشت;

    صحیح	    height = popup_height(wp);
    صحیح	    new_topline = wp->w_topline;

    اگر (row >= height / 2)
    {
	// Click in lower half, scroll down.
	اگر (wp->w_topline < wp->w_buffer->b_ml.ml_سطر_شمار)
	    ++new_topline;
    }
    وگرنه اگر (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    اگر (new_topline == wp->w_topline)
	بازگشت;

    set_topline(wp, new_topline);
    اگر (wp == curwin)
    {
	اگر (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	وگرنه اگر (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#اگر defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ایستا تهی
popup_add_timeout(win_T *wp, صحیح time, صحیح close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((نویسه *)cbbuf, اندازه(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    اگر (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	بازگشت;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    اگر (cb.cb_name != پوچ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    ایستا poppos_T
get_pos_entry(dict_T *d, صحیح give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    صحیح	    nr;

    اگر (str == پوچ)
	بازگشت POPPOS_NONE;

    برای (nr = 0; nr < (صحیح)ARRAY_LENGTH(poppos_entries); ++nr)
	اگر (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    بازگشت poppos_entries[nr].pp_val;

    اگر (give_error)
	semsg(_(e_invalid_argument_str), str);
    بازگشت POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ایستا تهی
apply_move_options(win_T *wp, dict_T *d)
{
    صحیح		nr;
    char_u	*str;
    dictitem_T	*di;

    اگر ((nr = dict_get_عدد_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    اگر ((nr = dict_get_عدد_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    اگر ((nr = dict_get_عدد_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    اگر ((nr = dict_get_عدد_def(d, "maxheight", -1)) >= 0)
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
	poppos_T ppt = get_pos_entry(d, TRUE);

	اگر (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    اگر (str != پوچ)
    {
	wp->w_popup_prop_type = 0;
	اگر (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    اگر (di != پوچ)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		اگر (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    اگر (nr <= 0)
		nr = find_prop_type_id(str, پوچ);
	    اگر (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    وگرنه
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    اگر (di != پوچ)
	wp->w_popup_prop_id = dict_get_عدد(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ایستا تهی
handle_moved_argument(win_T *wp, dictitem_T *di, صحیح mousemoved)
{
    اگر (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != پوچ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	صحیح	flags = 0;

	اگر (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	وگرنه اگر (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	وگرنه اگر (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	وگرنه اگر (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	اگر (flags != 0)
	{
	    اگر (mousemoved)
		set_mousemoved_columns(wp, flags);
	    وگرنه
		set_moved_columns(wp, flags);
	}
    }
    وگرنه اگر (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != پوچ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	صحیح	    mincol;
	صحیح	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	اگر (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_عدد(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    اگر (mousemoved)
		wp->w_popup_mouse_row = nr;
	    وگرنه
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    اگر (nr == 0)
		wp->w_popup_curwin = پوچ;
	}

	mincol = tv_get_عدد(&li->li_tv);
	maxcol = tv_get_عدد(&li->li_next->li_tv);
	اگر (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	وگرنه
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    وگرنه
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    ایستا تهی
check_highlight(dict_T *dict, نویسه *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    اگر (di == پوچ)
	بازگشت;

    اگر (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_مقدار_for_argument_str), name);
    وگرنه
    {
	str = tv_get_string(&di->di_tv);
	اگر (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ایستا تهی
popup_show_curline(win_T *wp)
{
    اگر (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    وگرنه اگر (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	اگر (wp->w_topline < 1)
	    wp->w_topline = 1;
	وگرنه اگر (wp->w_topline > wp->w_buffer->b_ml.ml_سطر_شمار)
	    wp->w_topline = wp->w_buffer->b_ml.ml_سطر_شمار;
	وقتی (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_سطر_شمار
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
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
    ایستا char_u *
popup_get_sign_name(win_T *wp)
{
    ایستا نویسه    buf[30];

    vim_snprintf(buf, اندازه(buf), "popup-%d", wp->w_id);
    بازگشت (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ایستا تهی
popup_highlight_curline(win_T *wp)
{
    صحیح	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    اگر ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	اگر (!sign_exists_by_name(sign_name))
	{
	    نویسه *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, پوچ, (char_u *)linehl, پوچ, پوچ, پوچ,
		    پوچ, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    وگرنه
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ایستا صحیح
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    صحیح		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    اگر (di != پوچ)
    {
	wp->w_firstline = dict_get_عدد(dict, "firstline");
	اگر (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    اگر (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    اگر (str != پوچ)
    {
	char_u	*title = vim_strsave(str);

	اگر (title != پوچ)
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
	وگرنه
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	وگرنه
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	وگرنه
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	وگرنه
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	وگرنه
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    اگر (di != پوچ)
    {
	nr = dict_get_عدد(dict, "opacity");
	اگر (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	وگرنه اگر (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	وگرنه اگر (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	وگرنه
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    اگر (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    وگرنه
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    اگر (di != پوچ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != پوچ)
    {
	image_T		    *img;
	image_T		    *cur_img = پوچ;
	image_placement_T   *place;
	صحیح		    cw, ch;

	اگر (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    اگر (wp->w_popup_imagep != پوچ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = پوچ;
	    }
	    برو image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	اگر (wp->w_popup_imagep != پوچ && wp->w_popup_imagep->img != پوچ)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	اگر (img == پوچ)
	    بازگشت FAIL;

	اگر (img == cur_img)
	    برو image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	اگر (place == پوچ)
	{
	    image_unref(img);
	    بازگشت FAIL;
	}

	اگر (wp->w_popup_imagep != پوچ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	اگر (dict_find(dict, (char_u *)"minwidth", -1) == پوچ)
	    wp->w_minwidth = cw;
	اگر (dict_find(dict, (char_u *)"maxwidth", -1) == پوچ)
	    wp->w_maxwidth = cw;
	اگر (dict_find(dict, (char_u *)"minheight", -1) == پوچ)
	    wp->w_minheight = ch;
	اگر (dict_find(dict, (char_u *)"maxheight", -1) == پوچ)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    وگرنه
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	اگر (wp->w_popup_imagep != پوچ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(پوچ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	اگر (wp->w_popup_imagep != پوچ)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    اگر (di != پوچ)
    {
	صحیح ok = TRUE;

	اگر (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != پوچ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    اگر (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    وگرنه اگر (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    وگرنه اگر (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    وگرنه
		ok = FALSE;
	}
	وگرنه
	    ok = FALSE;
	اگر (!ok)
	    semsg(_(e_invalid_مقدار_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    اگر (str != پوچ)
    {
	نویسه *errmsg = update_wincolor(wp, str);

	اگر (errmsg == پوچ)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وگرنه
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    اگر (str != پوچ)
    {
	نویسه *errmsg = update_winhighlight(wp, str);

	اگر (errmsg == پوچ)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وگرنه
	    emsg(_(errmsg));
    }

    اگر (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	بازگشت FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    اگر (di != پوچ)
    {
	اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == پوچ)
	{
	    emsg(_(e_list_required));
	    بازگشت FAIL;
	}
	وگرنه
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحیح		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    اگر (list->lv_len == 0)
	    {
		برای (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = پوچ;
		}
	    }
	    وگرنه
	    {
		برای (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    اگر (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			اگر (hl != پوچ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		اگر (list->lv_len == 1 && wp->w_border_highlight[0] != پوچ)
		    برای (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			اگر (hl != پوچ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    اگر (di != پوچ)
    {
	اگر (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    بازگشت FAIL;
	}
	وگرنه
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحیح		i;

	    اگر (list != پوچ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		برای (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    اگر (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		اگر (list->lv_len == 1)
		    برای (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		اگر (list->lv_len == 2)
		{
		    برای (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    برای (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    اگر (di != پوچ)
    {
	wp->w_zindex = dict_get_عدد(dict, "zindex");
	اگر (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	اگر (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    اگر (di != پوچ)
    {
	صحیح ok = FALSE;

	اگر (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != پوچ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_اقلام(di->di_tv.vval.v_list, li)
	    {
		اگر (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == پوچ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    بشکن;
		}
		وگرنه
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	اگر (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	وگرنه
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), "mask");
	    بازگشت FAIL;
	}
    }

#اگر defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_عدد(dict, "time");
    اگر (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    اگر (di != پوچ)
    {
	set_moved_مقادیر(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    اگر (di != پوچ)
    {
	set_mousemoved_مقادیر(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    اگر (nr != -1)
    {
	اگر (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	وگرنه
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    اگر (di != پوچ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	اگر (callback.cb_name != پوچ)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    اگر (nr != -1)
    {
	اگر (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	وگرنه
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    اگر (str != پوچ)
    {
	اگر (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	وگرنه
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    اگر (di == پوچ)
	بازگشت OK;

    callback_T	callback = get_callback(&di->di_tv);
    اگر (callback.cb_name == پوچ)
	بازگشت OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    بازگشت OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ایستا صحیح
apply_options(win_T *wp, dict_T *dict, صحیح create)
{
    صحیح		nr;

    apply_move_options(wp, dict);

    اگر (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    اگر (apply_general_options(wp, dict) == FAIL)
	بازگشت FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    اگر (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    اگر (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	اگر (wp->w_firstline > wp->w_buffer->b_ml.ml_سطر_شمار)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_سطر_شمار;
	وگرنه اگر (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    اگر (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    بازگشت OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ایستا تهی
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_اقلام(l, li)
	اگر (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == پوچ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ایستا تهی
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_اقلام(l, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    بازگشت;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == پوچ ? پوچ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == پوچ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    برای (li = l->lv_first; li != پوچ; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	اگر (di != پوچ)
	{
	    اگر (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		بازگشت;
	    }
	    plist = di->di_tv.vval.v_list;
	    اگر (plist != پوچ)
	    {
		FOR_ALL_LIST_اقلام(plist, pli)
		{
		    اگر (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			بازگشت;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    اگر (dict != پوچ)
		    {
			صحیح col = dict_get_عدد(dict, "col");

			prop_add_common(lnum, col, dict, buf, پوچ);
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
popup_بالاترین_extra(win_T *wp)
{
    صحیح	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    اگر (extra == 0 && wp->w_popup_title != پوچ && *wp->w_popup_title != NUL)
	بازگشت 1;
    بازگشت extra;
}

/*
 * Get the padding plus border at the left.
 */
    صحیح
popup_چپ_extra(win_T *wp)
{
    بازگشت wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    صحیح
popup_height(win_T *wp)
{
    بازگشت wp->w_height
	    + popup_بالاترین_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    صحیح
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    بازگشت wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    صحیح
popup_extra_width(win_T *wp)
{
    بازگشت wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ایستا win_T *
popup_get_clipwin(win_T *wp)
{
    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	بازگشت پوچ;
    اگر (win_valid(wp->w_popup_prop_win))
	بازگشت wp->w_popup_prop_win;
    بازگشت پوچ;
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
تعریفنوع ساختار {
    صحیح بالاترین_extra;
    صحیح bot_extra;
    صحیح چپ_extra;
    صحیح راست_extra;

    صحیح clip_بالاترین_content;
    صحیح clip_bot_content;
    صحیح clip_چپ_content;
    صحیح clip_راست_content;

    صحیح eff_بالاترین_extra;
    صحیح eff_bot_extra;
    صحیح eff_چپ_extra;
    صحیح eff_راست_extra;

    صحیح eff_border[4];
    صحیح eff_padding[4];

    صحیح eff_height;
    صحیح eff_width;
} popup_clip_T;

    ایستا تهی
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    صحیح h, w;

    cl->بالاترین_extra = popup_بالاترین_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->راست_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_بالاترین_content = wp->w_popup_topoff - cl->بالاترین_extra;
    اگر (cl->clip_بالاترین_content < 0)
	cl->clip_بالاترین_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    اگر (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_چپ_content = wp->w_popup_leftclip - cl->چپ_extra;
    اگر (cl->clip_چپ_content < 0)
	cl->clip_چپ_content = 0;
    cl->clip_راست_content = wp->w_popup_rightclip - cl->راست_extra;
    اگر (cl->clip_راست_content < 0)
	cl->clip_راست_content = 0;

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
	وگرنه
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
	وگرنه
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
	صحیح excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	اگر (excess > 0)
	{
	    اگر (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		اگر (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		وگرنه
		    cl->eff_border[0] -= excess;
	    }
	    وگرنه
		cl->eff_padding[0] -= excess;
	}
    }
    {
	صحیح excess = wp->w_popup_topoff - cl->بالاترین_extra - wp->w_height;
	اگر (excess > 0)
	{
	    اگر (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		اگر (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		وگرنه
		    cl->eff_border[2] -= excess;
	    }
	    وگرنه
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_بالاترین_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_چپ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->چپ_extra;
    cl->eff_راست_extra = wp->w_popup_rightclip > 0 ? 0 : cl->راست_extra;

    h = wp->w_height - cl->clip_بالاترین_content - cl->clip_bot_content;
    اگر (h < 0)
	h = 0;
    cl->eff_height = cl->eff_بالاترین_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_چپ_content - cl->clip_راست_content;
    اگر (w < 0)
	w = 0;
    cl->eff_width = cl->eff_چپ_extra + w + cl->eff_راست_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
تعریفنوع ساختار {
    صحیح		height;
    صحیح		width;
    صحیح		winrow;
    صحیح		wincol;
    صحیح		leftcol;
    صحیح		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ایستا تهی
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->height  = wp->w_height;
    sv->width   = wp->w_width;
    sv->winrow  = wp->w_winrow;
    sv->wincol  = wp->w_wincol;
    sv->leftcol = wp->w_leftcol;
    sv->p_wrap  = wp->w_p_wrap;
    sv->topline = wp->w_topline;
}

    ایستا تهی
popup_geom_restore(win_T *wp, popup_geom_save_T *sv)
{
    wp->w_p_wrap  = sv->p_wrap;
    wp->w_leftcol = sv->leftcol;
    wp->w_wincol  = sv->wincol;
    wp->w_winrow  = sv->winrow;
    wp->w_topline = sv->topline;
    wp->w_width   = sv->width;
    wp->w_height  = sv->height;
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
    ایستا تهی
popup_screenpos_above_بالاترین(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	صحیح	    *screen_row,
	صحیح	    *screen_scol,
	صحیح	    *screen_ccol,
	صحیح	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    صحیح	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (صحیح)win_col_off(prop_win) - (صحیح)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (صحیح)scol + coloff;
    *screen_ccol = (صحیح)ccol + coloff;
    *screen_ecol = (صحیح)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (صحیح)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ایستا تهی
popup_hide_for_textprop(win_T *wp)
{
    اگر ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	بازگشت;
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
    ایستا bool
popup_find_prop_above_بالاترین(
	win_T	    *wp,
	win_T	    *prop_win,
	صحیح	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	بازگشت false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    اگر (first < 1)
	first = 1;
    بازگشت find_prop_in_سطرها(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, found_lnum, first, prop_win->w_topline - 1);
}

/*
 * Compute and assign w_popup_topoff/bottomoff/leftclip/rightclip from the
 * host (textprop) window's content rectangle when POPF_CLIPWINDOW is set.
 * The popup's logical geometry (w_winrow, w_height, w_width) is preserved;
 * only the *off/clip fields record how much of each edge falls outside.
 * Returns true when the popup has scrolled completely past one of the host
 * edges, in which case the caller must hide it.
 */
    ایستا bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    صحیح	    extra_h, extra_w;
    صحیح	    popup_بالاترین, popup_bottom, popup_چپ, popup_راست;
    صحیح	    total_h, total_w;

    اگر (cw == پوچ)
	بازگشت false;

    extra_h = popup_بالاترین_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_بالاترین = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_چپ = wp->w_wincol;
    popup_راست = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    اگر (popup_بالاترین < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_بالاترین;
    اگر (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    اگر (popup_چپ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_چپ;
    اگر (popup_راست > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_راست - (cw->w_wincol + cw->w_width);

    بازگشت wp->w_popup_topoff >= total_h
	|| wp->w_popup_bottomoff >= total_h
	|| wp->w_popup_leftclip >= total_w
	|| wp->w_popup_rightclip >= total_w;
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
    ایستا تهی
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_بالاترین_content + cl->clip_bot_content;
	اگر (wp->w_height < 0)
	    wp->w_height = 0;
	اگر (cl->clip_بالاترین_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_بالاترین_content;
    }
    اگر (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	اگر (cl->clip_چپ_content > 0 || cl->clip_راست_content > 0)
	    wp->w_p_wrap = FALSE;
	اگر (cl->clip_راست_content > 0)
	{
	    wp->w_width -= cl->clip_راست_content;
	    اگر (wp->w_width < 0)
		wp->w_width = 0;
	}
	اگر (cl->clip_چپ_content > 0)
	{
	    wp->w_leftcol += cl->clip_چپ_content;
	    wp->w_wincol += cl->clip_چپ_content;
	    wp->w_width -= cl->clip_چپ_content;
	    اگر (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ایستا تهی
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    صحیح iw, ih;

    // Visible cell width and height of popup itself (not image!)
    صحیح	visible_width;
    صحیح	visible_height;

    // Crop dimensions of image (in pixels).
    صحیح	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    اگر (wp->w_popup_imagep == پوچ || wp->w_popup_flags & POPF_HIDDEN)
	بازگشت;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    اگر (wp->w_popup_imagep->img == پوچ)
	// Just set the bounding box
	برو exit;

    visible_width =
	wp->w_width - clip.clip_چپ_content - clip.clip_راست_content;
    visible_height =
	wp->w_height - clip.clip_بالاترین_content - clip.clip_bot_content;

    اگر (visible_width < 0 || visible_height < 0)
	بازگشت;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_چپ_content;
    crop_y = clip.clip_بالاترین_content;
    crop_width = iw - crop_x - clip.clip_راست_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    اگر (crop_width > visible_width)
	crop_width = visible_width;
    اگر (crop_height > visible_height)
	crop_height = visible_height;

    image_placement_set_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_width, crop_height);
exit:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_set_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_set_position(wp->w_popup_imagep, row, col);
    image_placement_set_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_height, clip.eff_width);
}
#endif


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    ایستا تهی
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    صحیح		wrapped = 0;
    صحیح		maxwidth;
    صحیح		maxwidth_no_scrollbar;
    صحیح		width_with_scrollbar = 0;
    صحیح		used_maxwidth = FALSE;
    صحیح		margin_width = 0;
    صحیح		maxspace;
    صحیح		center_vert = FALSE;
    صحیح		center_hor = FALSE;
    صحیح		allow_adjust_چپ = !wp->w_popup_fixed;
    صحیح		بالاترین_extra = popup_بالاترین_extra(wp);
    صحیح		راست_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    صحیح		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    صحیح		چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    صحیح		extra_height = بالاترین_extra + bot_extra;
    صحیح		extra_width = چپ_extra + راست_extra;
    صحیح		w_height_before_limit;
    popup_layout_T org_layout;
    صحیح		minwidth, minheight;
    صحیح		maxheight = Rows;
    صحیح		wantline = wp->w_wantline;  // adjusted for textprop
    صحیح		wantcol = wp->w_wantcol;    // adjusted for textprop
    صحیح		use_wantcol = wantcol != 0;
    صحیح		adjust_height_for_بالاترین_aligned = FALSE;

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
    اگر (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    اگر (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	صحیح	    screen_row;
	صحیح	    screen_scol;
	صحیح	    screen_ccol;
	صحیح	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_بالاترین = false;
	اگر (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    اگر (popup_find_prop_above_بالاترین(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_بالاترین = true;
	    وگرنه
	    {
		اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    اگر (wp->w_popup_imagep != پوچ)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    اگر (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		بازگشت;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	اگر (prop_above_بالاترین)
	    popup_screenpos_above_بالاترین(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	وگرنه
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	اگر (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    بازگشت;
	}
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	وگرنه
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	اگر (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	وگرنه
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    وگرنه
    {
	// If no line was specified default to vertical centering.
	اگر (wantline == 0)
	    center_vert = TRUE;
	وگرنه اگر (wantline < 0)
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
    وگرنه
    {
	اگر (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    اگر (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	اگر (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_سطر_شمار - 1, 0);
	}

	اگر (!use_wantcol)
	    center_hor = TRUE;
	وگرنه اگر (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    اگر (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- چپ_extra - راست_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- چپ_extra - راست_extra - 1;
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
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - چپ_extra - راست_extra;
    maxwidth = maxspace;
    اگر (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_چپ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    اگر (wp->w_p_nu || wp->w_p_rnu)
	margin_width = عدد_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    اگر (signcolumn_on(wp))
	margin_width += 2;
#endif
    اگر (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    اگر (wp->w_buffer->b_term != پوچ)
    {
	اگر (minwidth == 0)
	    minwidth = 20;
	اگر (minheight == 0)
	    minheight = 5;
    }
#endif

    اگر (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    وگرنه اگر (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    اگر (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	اگر (wp->w_firstline > wp->w_buffer->b_ml.ml_سطر_شمار)
	    wp->w_firstline = 0;
	وگرنه
	    wp->w_topline = wp->w_firstline;
    }
    اگر (wp->w_topline < 1)
	wp->w_topline = 1;
    وگرنه اگر (wp->w_topline > wp->w_buffer->b_ml.ml_سطر_شمار)
	wp->w_topline = wp->w_buffer->b_ml.ml_سطر_شمار;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_width = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	صحیح saved_w_width = wp->w_width;

	اگر (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	برای (ln = 1; ln <= wp->w_buffer->b_ml.ml_سطر_شمار; ++ln)
	{
	    صحیح len = linetabsize(wp, ln) + margin_width;

	    اگر (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    اگر (saved_w_width < len)
		saved_w_width = len;
	    اگر (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		بشکن;
	}
	wp->w_width = saved_w_width;
    }
    اگر (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_سطر_شمار;
    وگرنه
	lnum = wp->w_topline;
    وقتی (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_سطر_شمار)
    {
	صحیح len;
	صحیح w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	اگر (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	اگر (len + margin_width > maxwidth
		&& allow_adjust_چپ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    صحیح shift_by = len + margin_width - maxwidth;

	    اگر (shift_by > wp->w_wincol)
	    {
		صحیح truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    اگر (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    اگر (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	اگر (wp->w_p_wrap)
	{
	    وقتی (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	اگر (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    اگر (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	اگر (wp->w_firstline < 0)
	    --lnum;
	وگرنه
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	اگر (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_سطر_شمار - lnum)
		       + wrapped >= maxheight)
	    بشکن;
    }

    اگر (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_سطر_شمار);
#ifdef FEAT_TERMINAL
    اگر (wp->w_buffer->b_term != پوچ && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    اگر (wp->w_has_scrollbar)
    {
	++راست_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	اگر (maxwidth + راست_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    اگر (wp->w_popup_title != پوچ && *wp->w_popup_title != NUL)
    {
	صحیح title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	اگر (minwidth < title_len)
	    minwidth = title_len;
    }

    اگر (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    اگر (wp->w_width > maxwidth)
    {
	اگر (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	اگر (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    صحیح off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, راست_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	وگرنه
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - راست_extra;
	}
    }
    اگر (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	اگر (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    وگرنه اگر (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	صحیح leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	اگر (leftoff >= 0)
	    wp->w_wincol = leftoff;
	وگرنه اگر (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    اگر (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    اگر (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	صحیح want_col = 0;

	// try to show the right border and any scrollbar
	want_col = چپ_extra + wp->w_width + راست_extra;
	اگر (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    اگر (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_سطر_شمار - wp->w_topline
								 + 1 + wrapped;
    اگر (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    اگر (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    اگر (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    اگر (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	اگر (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    وگرنه اگر (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	اگر ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	وگرنه اگر (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	وگرنه
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_بالاترین_aligned = TRUE;
	}
    }
    وگرنه اگر (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	صحیح check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	اگر (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    اگر (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    اگر (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	وگرنه
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_بالاترین_aligned = TRUE;
	}
    }

    اگر (adjust_height_for_بالاترین_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_height + extra_height > Rows)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_height = Rows - wp->w_winrow - extra_height;
#ifdef FEAT_TERMINAL
	اگر (wp->w_buffer->b_term == پوچ || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    اگر (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		اگر (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		وگرنه
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    اگر (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    وگرنه اگر (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    اگر (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    اگر (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    اگر (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    اگر (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	صحیح avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    اگر (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	اگر (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	اگر (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	اگر (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    صحیح avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    اگر (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    اگر (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	اگر (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    اگر (wp->w_popup_imagep != پوچ)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	بازگشت;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    اگر (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_buffer);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_width = wp->w_popup_prop_win->w_width;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_height;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    اگر (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	اگر (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

تعریفنوع شمارش
{
    TYPE_NORMAL,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_MESSAGE_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    ایستا صحیح
popup_is_notification(create_type_T type)
{
    بازگشت type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ایستا تهی
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    صحیح	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    برای (lnum = buf->b_ml.ml_سطر_شمار; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    اگر (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == پوچ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    وگرنه
    {
	list_T *l = text.vval.v_list;

	اگر (l != پوچ && l->lv_len > 0)
	{
	    اگر (l->lv_first == &range_list_item)
		emsg(_(e_using_عدد_as_string));
	    وگرنه اگر (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    وگرنه
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_سطر_شمار);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    انجامبده {						    \
	اگر (wp != پوچ)					    \
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
    } وقتی (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ایستا صحیح
parse_popup_option(win_T *wp, صحیح is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    صحیح	    border_enabled = FALSE;

    اگر (wp != پوچ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    برای ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	صحیح	x;

	e = vim_strchr(p, ':');
	اگر (e == پوچ || e[1] == NUL)
	    بازگشت FAIL;

	p = vim_strchr(e, ',');
	اگر (p == پوچ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	اگر (STRNCMP(s, "height:", 7) == 0)
	{
	    اگر (dig != p)
		بازگشت FAIL;
	    اگر (wp != پوچ)
	    {
		اگر (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	وگرنه اگر (STRNCMP(s, "width:", 6) == 0)
	{
	    اگر (dig != p)
		بازگشت FAIL;
	    اگر (wp != پوچ)
	    {
		اگر (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	وگرنه اگر (STRNCMP(s, "highlight:", 10) == 0)
	{
	    اگر (wp != پوچ)
	    {
		نویسه *errmsg;
		صحیح c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		اگر (errmsg == پوچ)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		وگرنه
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	وگرنه اگر (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    اگر (*arg == NUL || *arg == ',')
		بازگشت FAIL;
	    اگر (wp != پوچ)
	    {
		برای (صحیح i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	وگرنه اگر (STRNCMP(s, "border:", 7) == 0)
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
		اگر (wp != پوچ)
		{
		    برای (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ادامه;
	    }

	    token = vim_strnsave(arg, token_len);
	    اگر (token == پوچ)
		بازگشت FAIL;

	    اگر ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		اگر (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		وگرنه اگر (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		وگرنه اگر (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		وگرنه اگر (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		وگرنه اگر (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		وگرنه اگر (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    صحیح		out[8];
		    صحیح		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    برای (i = 0; i < 8 && !failed; i++)
		    {
			اگر (*q == NUL)
			    failed = TRUE;
			وگرنه
			{
			    out[i] = mb_ptr2char(q);
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
			بازگشت FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    وگرنه
	    {
		vim_free(token);
		بازگشت FAIL;
	    }

	    اگر (wp != پوچ)
	    {
		برای (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	وگرنه اگر (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		بازگشت FAIL;
	    on = on && mouse_has(MOUSE_درج) && (border_enabled || is_preview);
	    اگر (wp != پوچ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	وگرنه اگر (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		بازگشت FAIL;
	    اگر (wp != پوچ)
	    {
		اگر (on && mouse_has(MOUSE_درج))
		    wp->w_popup_flags |= POPF_RESIZE;
		وگرنه
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	وگرنه اگر (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحیح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحیح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    اگر (!on && !off)
		بازگشت FAIL;
	    اگر (wp != پوچ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	وگرنه اگر (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    صحیح		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    صحیح		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    اگر (!menu && !item)
		بازگشت FAIL;
	    اگر (wp != پوچ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	وگرنه اگر (STRNCMP(s, "opacity:", 8) == 0)
	{
	    اگر (dig != p || x < 0 || x > 100)
		بازگشت FAIL;
	    اگر (wp != پوچ)
	    {
		اگر (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		وگرنه
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	وگرنه
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحیح
parse_previewpopup(win_T *wp)
{
    بازگشت parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحیح
parse_completepopup(win_T *wp)
{
    بازگشت parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    تهی
popup_set_wantpos_cursor(win_T *wp, صحیح width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    اگر (d != پوچ)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    اگر (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    وگرنه
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
    اگر (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	اگر (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    تهی
popup_set_wantpos_rowcol(win_T *wp, صحیح row, صحیح col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ایستا تهی
add_border_چپ_راست_padding(win_T *wp)
{
    صحیح i;

    برای (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ایستا صحیح
popup_terminal_exists(تهی)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_buffer->b_term != پوچ)
	    بازگشت TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    اگر (wp->w_buffer->b_term != پوچ)
		بازگشت TRUE;
    بازگشت FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    تهی
popup_redraw_all(تهی)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    صحیح
popup_need_redraw(تهی)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    بازگشت TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Set the color for a notification window.
 */
    ایستا تهی
popup_update_color(win_T *wp, create_type_T type)
{
    نویسه    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    نویسه    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    اگر (errmsg == پوچ)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    وگرنه
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ایستا win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = پوچ;
    صحیح		tabnr = 0;
    صحیح		new_buffer;
    buf_T	*buf = پوچ;
    dict_T	*d = پوچ;
    صحیح		i;

    اگر (check_secure())
	بازگشت پوچ;

    اگر (argvars != پوچ)
    {
	اگر (in_vim9script()
		&& (check_for_string_or_عدد_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    بازگشت پوچ;

	// Check that arguments look OK.
	اگر (argvars[0].v_type == VAR_عدد)
	{
	    buf = buflist_findnr(argvars[0].vval.v_عدد);
	    اگر (buf == پوچ)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_عدد);
		بازگشت پوچ;
	    }
#ifdef FEAT_TERMINAL
	    اگر (buf->b_term != پوچ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		بازگشت پوچ;
	    }
#endif
	}
	وگرنه اگر (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != پوچ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != پوچ))
	{
	    emsg(_(e_buffer_عدد_text_or_list_required));
	    بازگشت پوچ;
	}
	اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    بازگشت پوچ;
	d = argvars[1].vval.v_dict;
    }

    اگر (d != پوچ)
    {
	اگر (dict_has_کلید(d, "tabpage"))
	    tabnr = (صحیح)dict_get_عدد(d, "tabpage");
	وگرنه اگر (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	وگرنه
	    tabnr = 0;
	اگر (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    اگر (tp == پوچ)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		بازگشت پوچ;
	    }
	}
    }
    وگرنه اگر (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    اگر (buf != پوچ && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	بازگشت پوچ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    اگر (wp == پوچ)
	بازگشت پوچ;
    اگر (rettv != پوچ)
	rettv->vval.v_عدد = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    اگر (buf != پوچ)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    وگرنه
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(پوچ, پوچ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	اگر (buf == پوچ)
	{
	    win_free_popup(wp);
	    بازگشت پوچ;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	set_local_options_default(wp, TRUE);
	set_string_option_direct_in_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	set_string_option_direct_in_buf(buf, (char_u *)"bufhidden", -1,
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

    اگر (tp != پوچ)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    وگرنه اگر (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    وگرنه // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	اگر (first_popupwin == پوچ)
	    first_popupwin = wp;
	وگرنه
	{
	    وقتی (prev->w_next != پوچ)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    اگر (new_buffer && argvars != پوچ)
	popup_set_buffer_text(buf, argvars[0]);

    اگر (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    اگر (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_مقادیر(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    اگر (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	اگر (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_مقادیر(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    اگر (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	صحیح	height = buf->b_ml.ml_سطر_شمار + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	برای (twp = first_popupwin; twp != پوچ; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    اگر (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + height
		    && twp->w_winrow + popup_height(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_height(twp) + 1;
		nextwin = first_popupwin;
	    }
	}
	اگر (wp->w_wantline + height > Rows)
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
	برای (i = 0; i < 4; ++i)
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
	add_border_چپ_راست_padding(wp);
    }

    اگر (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	اگر (callback.cb_name != پوچ)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    برای (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    برای (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    اگر (type == TYPE_PREVIEW)
    {
	اگر (mouse_has(MOUSE_درج))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	برای (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    اگر (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	اگر (mouse_has(MOUSE_درج))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_چپ_راست_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    اگر (d != پوچ)
    {
	// Deal with options.
	اگر (apply_options(wp, d, TRUE) == FAIL)
	{
	    (تهی)popup_close(wp->w_id, FALSE);
	    بازگشت پوچ;
	}
    }

#ifdef FEAT_TIMERS
    اگر (popup_is_notification(type) && wp->w_popup_timer == پوچ)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    اگر (buf->b_term != پوچ)
	win_enter(wp, FALSE);
#endif

    بازگشت wp;
}

/*
 * popup_clear()
 */
    تهی
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح force = FALSE;

    اگر (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
	force = (صحیح)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    تهی
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    تهی
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    تهی
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ایستا تهی
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)wp->w_id;

    اگر (result != پوچ && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    وگرنه
    {
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    اگر (result != پوچ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ایستا تهی
back_to_prevwin(win_T *wp)
{
    اگر (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    وگرنه
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ایستا تهی
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    صحیح id;

    اگر (!win_valid(wp))
       بازگشت;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    اگر (wp == curwin && curbuf->b_term != پوچ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    اگر (owp != curwin && owp->w_buffer->b_term != پوچ)
		بشکن;
	اگر (owp != پوچ)
	    win_enter(owp, FALSE);
	وگرنه
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		اگر (owp != curwin && owp->w_buffer->b_term != پوچ)
		    بشکن;
	    اگر (owp != پوچ)
		win_enter(owp, FALSE);
	    وگرنه
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    اگر (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	بازگشت;
    }

    CHECK_CURBUF;
    اگر (wp->w_close_cb.cb_name != پوچ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    تهی
popup_close_with_retval(win_T *wp, صحیح retval)
{
    typval_T res;

    res.v_type = VAR_عدد;
    res.vval.v_عدد = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    تهی
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ایستا تهی
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    اگر (wp != mouse_wp
	    && wp->w_popup_mouse_row != 0
	    && (wp->w_popup_mouse_row != mouse_row
		|| mouse_col < wp->w_popup_mouse_mincol
		|| mouse_col > wp->w_popup_mouse_maxcol))
    {
	// Careful: this makes "wp" invalid.
	popup_close_with_retval(wp, -2);
    }
}

/*
 * Called when the mouse moved: may close a popup with "mousemoved".
 */
    تهی
popup_handle_mouse_moved(تهی)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    صحیح	    row = mouse_row;
    صحیح	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    برای (wp = first_popupwin; wp != پوچ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    برای (wp = curtab->tp_first_popupwin; wp != پوچ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ایستا تهی
filter_handle_drag(win_T *wp, صحیح c, typval_T *rettv)
{
    صحیح	row = mouse_row;
    صحیح	col = mouse_col;

    اگر ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_کلید(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_عدد = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    تهی
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحیح		c;
    linenr_T	old_lnum;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    id = tv_get_عدد(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    اگر (wp == پوچ)
	بازگشت;

    c = *کلید;
    اگر (c == K_SPECIAL && کلید[1] != NUL)
	c = TO_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_TRUE;
    res.v_type = VAR_عدد;

    old_lnum = wp->w_cursor.lnum;
    اگر (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	اگر (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	وگرنه
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_سطر_شمار;
    }
    اگر (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	اگر (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_سطر_شمار)
	    ++wp->w_cursor.lnum;
	وگرنه
	    wp->w_cursor.lnum = 1;
    }
    اگر (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	بازگشت;
    }

    اگر (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_عدد = -1;
	popup_close_and_callback(wp, &res);
	بازگشت;
    }
    اگر (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_عدد = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	بازگشت;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    تهی
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحیح		c;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    id = tv_get_عدد(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    اگر (wp == پوچ)
	بازگشت;

    c = *کلید;
    اگر (c == CAR && need_wait_return)
	بازگشت;
    اگر (c == K_SPECIAL && کلید[1] != NUL)
	c = TO_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_TRUE;

    اگر (c == 'y' || c == 'Y')
	res.vval.v_عدد = 1;
    وگرنه اگر (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_عدد = 0;
    وگرنه
    {
	filter_handle_drag(wp, c, rettv);
	بازگشت;
    }

    // Invoke callback
    res.v_type = VAR_عدد;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    تهی
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    تهی
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    تهی
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ایستا win_T *
find_popup_win(صحیح id)
{
    win_T *wp = win_id2wp(id);

    اگر (wp != پوچ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	بازگشت پوچ;
    }
    بازگشت wp;
}

/*
 * popup_close({id})
 */
    تهی
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح		id;
    win_T	*wp;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    اگر (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == پوچ &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    wp = find_popup_win(id);
    اگر (wp != پوچ)
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
    ایستا تهی
popup_clear_mask_for(win_T *wp)
{
    صحیح r, c;
    صحیح row_start, col_start, row_end, col_end;

    اگر (popup_mask == پوچ || !popup_visible)
	بازگشت;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (صحیح)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (صحیح)screen_Columns);

    برای (r = row_start; r < row_end; ++r)
	برای (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    تهی
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    صحیح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    اگر (error_if_term_popup_window())
	بازگشت;
#endif
    اگر ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	بازگشت;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != پوچ)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    اگر (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    اگر (was_visible)
	popup_clear_mask_for(wp);

    اگر (old_area.active)
	popup_redraw_exposed_area(&old_area);
    وگرنه
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    اگر (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    تهی
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح		id;
    win_T	*wp;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    تهی
popup_show(win_T *wp)
{
    bool popup_active;

    اگر ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	بازگشت;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    اگر (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطرها_valid = 0;
	اگر (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    وگرنه
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    تهی
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح		id;
    win_T	*wp;

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = -1;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    اگر (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_عدد = 0;
}

/*
 * popup_settext({id}, {text})
 */
    تهی
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	بازگشت;

    id = (صحیح)tv_get_عدد(&argvars[0]);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;

    popup_save_area(wp, &old_area);

    اگر (check_for_string_or_list_arg(argvars, 1) == FAIL)
	بازگشت;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطرها_valid = 0;
    اگر (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    اگر (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    تهی
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    اگر (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(&argvars[0]);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;

    buf = tv_get_buf_from_arg(&argvars[1]);

    اگر (buf == پوچ)
	بازگشت;
#ifdef FEAT_TERMINAL
    اگر (buf->b_term != پوچ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	بازگشت;
    }
#endif

    اگر (wp->w_buffer != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_buffer->b_nwindows--;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	اگر (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_عدد = VVAL_TRUE;
}

    ایستا تهی
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    صحیح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != پوچ)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    اگر (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    اگر (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    اگر (wp == message_win)
	message_win = پوچ;
#endif

    اگر (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    اگر (!old_area.active)
	popup_mask_refresh = TRUE;
}

    ایستا تهی
error_for_popup_window(تهی)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    صحیح
error_if_popup_window(صحیح also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    اگر (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == پوچ)
#endif
	    )
    {
	error_for_popup_window();
	بازگشت TRUE;
    }
    بازگشت FALSE;
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
    win_T	*prev = پوچ;

    // go through global popups
    برای (wp = first_popupwin; wp != پوچ; prev = wp, wp = wp->w_next)
	اگر (wp->w_id == id)
	{
	    اگر (wp == curwin)
	    {
		اگر (!force)
		{
		    error_for_popup_window();
		    بازگشت FAIL;
		}
		back_to_prevwin(wp);
	    }
	    اگر (prev == پوچ)
		first_popupwin = wp->w_next;
	    وگرنه
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    بازگشت OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	اگر (popup_close_tabpage(tp, id, force) == OK)
	    بازگشت OK;
    بازگشت FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    صحیح
popup_close_tabpage(tabpage_T *tp, صحیح id, صحیح force)
{
    win_T	*wp;
    win_T	**ریشه = &tp->tp_first_popupwin;
    win_T	*prev = پوچ;

    برای (wp = *ریشه; wp != پوچ; prev = wp, wp = wp->w_next)
	اگر (wp->w_id == id)
	{
	    اگر (wp == curwin)
	    {
		اگر (!force)
		{
		    error_for_popup_window();
		    بازگشت FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    اگر (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    اگر (prev == پوچ)
		*ریشه = wp->w_next;
	    وگرنه
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    بازگشت OK;
	}
    بازگشت FAIL;
}

    تهی
close_all_popups(صحیح force)
{
    اگر (!force && ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    وقتی (first_popupwin != پوچ)
	اگر (popup_close(first_popupwin->w_id, force) == FAIL)
	    بازگشت;
    وقتی (curtab->tp_first_popupwin != پوچ)
	اگر (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    بازگشت;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ایستا تهی
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->height = popup_height(wp);
    area->width = popup_width(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    ایستا تهی
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
    برای (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ایستا bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    صحیح i;

    اگر (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	بازگشت true;
    برای (i = 0; i < 4; i++)
	اگر (style->border_highlight[i] != wp->w_border_highlight[i])
	    بازگشت true;
    بازگشت false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ایستا تهی
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->width = wp->w_width;
    layout->height = wp->w_height;
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
    ایستا bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    بازگشت layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->width != wp->w_width
	|| layout->height != wp->w_height
	|| layout->has_scrollbar != wp->w_has_scrollbar
	|| layout->topoff != wp->w_popup_topoff
	|| layout->bottomoff != wp->w_popup_bottomoff
	|| layout->leftclip != wp->w_popup_leftclip
	|| layout->rightclip != wp->w_popup_rightclip;
}

/*
 * Return true when the popup no longer covers the saved area.
 */
    ایستا bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    بازگشت area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->height != popup_height(wp)
	|| area->width != popup_width(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ایستا تهی
mark_overlapped_opacity_popup(win_T *wp, صحیح area_بالاترین, صحیح area_bot,
	صحیح area_چپ, صحیح area_راست, صحیح zindex)
{
    اگر ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_بالاترین
	    || wp->w_wincol >= area_راست
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_چپ)
	بازگشت;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطرها_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ایستا تهی
redraw_overlapped_opacity_popups(صحیح winrow, صحیح wincol, صحیح height, صحیح width,
	صحیح leftoff, صحیح zindex)
{
    win_T	*wp;
    صحیح		area_بالاترین = winrow;
    صحیح		area_bot = winrow + height;
    صحیح		area_چپ = wincol;
    صحیح		area_راست = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_بالاترین, area_bot, area_چپ,
		area_راست, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_بالاترین, area_bot, area_چپ,
		area_راست, zindex);

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
    ایستا تهی
refill_pum_bg_row_from_buffer(صحیح r, صحیح چپ, صحیح راست)
{
    صحیح		سطر_cp = r;
    صحیح		col_cp = چپ;
    win_T	*wp;
    linenr_T	lnum;
    صحیح		row_for_lnum;
    صحیح		col_for_lnum = 0;
    char_u	*سطر;
    char_u	*p;
    صحیح		win_text_col;
    صحیح		screen_col;
    صحیح		soff_base = (r - pum_bg_بالاترین) * pum_bg_cols;
    صحیح		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    برای (c = چپ; c < راست; ++c)
    {
	pum_bg_سطرها[soff_base + c] = ' ';
	اگر (pum_bg_attrs != پوچ)
	    pum_bg_attrs[soff_base + c] = 0;
	اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ)
	    pum_bg_سطرها_UC[soff_base + c] = 0;
	اگر (enc_utf8)
	{
	    صحیح k;
	    برای (k = 0; k < MAX_MCO; ++k)
		اگر (pum_bg_سطرها_C[k] != پوچ)
		    pum_bg_سطرها_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&سطر_cp, &col_cp, IGNORE_POPUP);
    اگر (wp == پوچ || سطر_cp < 0 || سطر_cp >= wp->w_height)
	بازگشت;
    اگر (wp->w_buffer == پوچ || wp->w_buffer->b_ml.ml_mfp == پوچ)
	بازگشت;

    // Compute the buffer line for this screen row.
    row_for_lnum = سطر_cp;
    اگر (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, پوچ))
	بازگشت;	// past end of buffer
    اگر (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_سطر_شمار)
	بازگشت;

    سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);
    اگر (سطر == پوچ)
	بازگشت;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    اگر (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = سطر;
    وقتی (*p != NUL && screen_col < راست)
    {
	صحیح char_cells;
	صحیح byte_شمار;
	صحیح soff = soff_base + screen_col;
	صحیح in_range = (screen_col >= چپ && screen_col < راست);

	اگر (*p == '\t')
	{
	    صحیح ts = (صحیح)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_شمار = 1;
	    اگر (in_range)
	    {
		pum_bg_سطرها[soff] = ' ';
		اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ)
		    pum_bg_سطرها_UC[soff] = 0;
	    }
	}
	وگرنه اگر (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_شمار = mb_ptr2len(p);
	    اگر (in_range)
	    {
		pum_bg_سطرها[soff] = *p;
		اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ)
		    pum_bg_سطرها_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	وگرنه
	{
	    char_cells = 1;
	    byte_شمار = 1;
	    اگر (in_range)
	    {
		pum_bg_سطرها[soff] = (*p < 0x20) ? ' ' : *p;
		اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ)
		    pum_bg_سطرها_UC[soff] = 0;
	    }
	}
	اگر (in_range && pum_bg_attrs != پوچ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_شمار;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ایستا تهی
popup_redraw_exposed_area(popup_area_T *area)
{
    اگر (!area->active)
	بازگشت;

    redraw_under_popup_area(area->winrow, area->wincol, area->height,
	    area->width, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->height, area->width, area->leftoff, area->zindex);

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
    اگر (pum_bg_سطرها != پوچ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_بالاترین)
    {
	صحیح بالاترین = MAX(area->winrow, pum_bg_بالاترین);
	صحیح bot = MIN(area->winrow + area->height, pum_bg_bot);
	صحیح چپ = MAX(area->wincol, 0);
	صحیح راست = MIN(area->wincol + area->width, pum_bg_cols);
	صحیح r;

	برای (r = بالاترین; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, چپ, راست);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ایستا تهی
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->سطرها);
    vim_free(saved_screen->attrs);
    vim_free(saved_screen->linesuc);
    CLEAR_POINTER(saved_screen);
}

/*
 * Save the screen area that opacity padding may need to blend against.
 * On entry "saved_screen" must be zero-initialised.  When the snapshot is
 * unavailable (no opacity popup, no padding, allocation failure) "lines" is
 * left NULL and callers should fall back to plain screen_fill().  The caller
 * must always release the snapshot with popup_free_saved_screen().
 */
    ایستا تهی
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    اگر (screen_opacity_popup == پوچ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	بازگشت;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    اگر (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->سطرها = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(صحیح,
				  saved_screen->rows * saved_screen->cols);
    اگر (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    اگر (saved_screen->سطرها == پوچ || saved_screen->attrs == پوچ)
	بازگشت;

    برای (صحیح r = 0; r < saved_screen->rows; r++)
    {
	صحیح screen_row = saved_screen->start_row + r;

	اگر (screen_row >= 0 && screen_row < screen_Rows)
	    برای (صحیح c = 0; c < saved_screen->cols; c++)
	    {
		صحیح screen_col = saved_screen->start_col + c;

		اگر (screen_col >= 0 && screen_col < screen_Columns)
		{
		    صحیح off = سطر_Offset[screen_row] + screen_col;
		    صحیح save_off = r * saved_screen->cols + c;

		    saved_screen->سطرها[save_off] = Screen_سطرها[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    اگر (enc_utf8 && saved_screen->linesuc != پوچ)
			saved_screen->linesuc[save_off] = Screen_سطرها_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ایستا تهی
redraw_under_popup_area(صحیح winrow, صحیح wincol, صحیح height, صحیح width, صحیح leftoff)
{
    صحیح	    r;

    برای (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	صحیح	    c;
	win_T	    *prev_twp = پوچ;

	اگر (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    ادامه;
	}

	برای (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    صحیح	    سطر_cp = r;
	    صحیح	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&سطر_cp, &col_cp, IGNORE_POPUP);
	    اگر (twp != پوچ && twp != prev_twp)
	    {
		prev_twp = twp;
		اگر (سطر_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (تهی)mouse_comp_pos(twp, &سطر_cp, &col_cp, &lnum, پوچ);
		    redrawWinline(twp, lnum);
		}
		وگرنه اگر (سطر_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    تهی
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;  // invalid {id}

    اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	بازگشت;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    اگر (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    اگر (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    تهی
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    صحیح		need_redraw = FALSE;
    صحیح		need_reposition = FALSE;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;  // invalid {id}

    اگر (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	بازگشت;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (تهی)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    اگر (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_سطر_شمار)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    اگر (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    وگرنه اگر (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    اگر (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    اگر (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    وگرنه اگر (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطرها_valid = 0;
	اگر (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    اگر (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    اگر (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    تهی
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    صحیح		بالاترین_extra;
    صحیح		چپ_extra;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;  // invalid {id}
    بالاترین_extra = popup_بالاترین_extra(wp);
    چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_عدد(dict, "line", wp->w_winrow + 1);
    dict_add_عدد(dict, "col", wp->w_wincol + 1);
    dict_add_عدد(dict, "width", wp->w_width + چپ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_عدد(dict, "height", wp->w_height + بالاترین_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_عدد(dict, "core_line", wp->w_winrow + 1 + بالاترین_extra);
    dict_add_عدد(dict, "core_col", wp->w_wincol + 1 + چپ_extra);
    dict_add_عدد(dict, "core_width", wp->w_width);
    dict_add_عدد(dict, "core_height", wp->w_height);

    dict_add_عدد(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_عدد(dict, "firstline", wp->w_topline);
    dict_add_عدد(dict, "lastline", wp->w_botline - 1);
    dict_add_عدد(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    تهی
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    FOR_ALL_POPUPWINS(wp)
	list_append_عدد(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_عدد(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    تهی
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    صحیح		row;
    صحیح		col;
    win_T	*wp;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    row = tv_get_عدد(&argvars[0]) - 1;
    col = tv_get_عدد(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    اگر (wp != پوچ && WIN_IS_POPUP(wp))
	rettv->vval.v_عدد = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ایستا تهی
get_padding_border(dict_T *dict, صحیح *array, نویسه *name)
{
    list_T  *list;
    صحیح	    i;

    اگر (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	بازگشت;

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    اگر (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	بازگشت;
    }
    اگر (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	برای (i = 0; i < 4; ++i)
	    list_append_عدد(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ایستا تهی
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحیح	    i;

    برای (i = 0; i < 4; ++i)
	اگر (wp->w_border_highlight[i] != پوچ)
	    بشکن;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    اگر (i == 4 && !wp->w_border_highlight_isset)
	بازگشت;

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    اگر (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	بازگشت;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    اگر (i == 4)
	بازگشت;
    برای (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ایستا تهی
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحیح	    i;
    char_u  buf[NUMBUFLEN];
    صحیح	    len;

    برای (i = 0; i < 8; ++i)
	اگر (wp->w_border_char[i] != 0)
	    بشکن;
    اگر (i == 8)
	بازگشت;

    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    اگر (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	بازگشت;
    }
    برای (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ایستا تهی
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    اگر (list != پوچ)
    {
	اگر (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	وگرنه
	{
	    list_append_عدد(list, wp->w_popup_lnum);
	    list_append_عدد(list, wp->w_popup_mincol);
	    list_append_عدد(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    اگر (list == پوچ)
	بازگشت;

    اگر (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	بازگشت;
    }
    list_append_عدد(list, wp->w_popup_mouse_row);
    list_append_عدد(list, wp->w_popup_mouse_mincol);
    list_append_عدد(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    تهی
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحیح		id;
    win_T	*wp;
    tabpage_T	*tp;
    صحیح		i;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = find_popup_win(id);
    اگر (wp == پوچ)
	بازگشت;

    dict = rettv->vval.v_dict;
    dict_add_عدد(dict, "line", wp->w_wantline);
    dict_add_عدد(dict, "col", wp->w_wantcol);
    dict_add_عدد(dict, "minwidth", wp->w_minwidth);
    dict_add_عدد(dict, "minheight", wp->w_minheight);
    dict_add_عدد(dict, "maxheight", wp->w_maxheight);
    dict_add_عدد(dict, "maxwidth", wp->w_maxwidth);
    dict_add_عدد(dict, "firstline", wp->w_firstline);
    dict_add_عدد(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_عدد(dict, "zindex", wp->w_zindex);
    dict_add_عدد(dict, "fixed", wp->w_popup_fixed);
    اگر (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	اگر (pt != پوچ)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_عدد(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_عدد(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    اگر (wp->w_popup_imagep != پوچ && wp->w_popup_imagep->img != پوچ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	اگر (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_عدد(dict, "wrap", wp->w_p_wrap);
    dict_add_عدد(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_عدد(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_عدد(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_عدد(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_عدد(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_عدد(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_عدد(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_عدد(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    اگر (wp->w_scrollbar_highlight != پوچ)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    اگر (wp->w_thumb_highlight != پوچ)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    اگر (twp->w_id == id)
		بشکن;
	اگر (twp != پوچ)
	    بشکن;
	++i;
    }
    اگر (tp == پوچ)
	i = -1;  // must be global
    وگرنه اگر (tp == curtab)
	i = 0;
    dict_add_عدد(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    اگر (wp->w_filter_cb.cb_name != پوچ)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    اگر (wp->w_close_cb.cb_name != پوچ)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(poppos_entries); ++i)
	اگر (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(صحیح)poppos_entries[i].pp_name.length);
	    بشکن;
	}

    اگر (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    وگرنه اگر (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    وگرنه
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#اگر defined(FEAT_TIMERS)
    dict_add_عدد(dict, "time", wp->w_popup_timer != پوچ
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#اگر defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    صحیح
error_if_term_popup_window(تهی)
{
    اگر (WIN_IS_POPUP(curwin) && curbuf->b_term != پوچ
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    تهی
popup_reset_handled(صحیح handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
find_next_popup(صحیح lowest, صحیح handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    صحیح	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = پوچ;
    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    اگر (found_wp != پوچ)
	found_wp->w_popup_handled |= handled_flag;
    بازگشت found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ایستا صحیح
invoke_popup_filter(win_T *wp, صحیح c)
{
    صحیح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    صحیح		prev_did_emsg = did_emsg;

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
	    بازگشت -1;
	بازگشت TRUE;
    }

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
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
    وگرنه
    {
	اگر (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	اگر (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    اگر (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	وگرنه
	{
	    اگر (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    بازگشت res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    صحیح
popup_do_filter(صحیح c)
{
    ایستا صحیح	recursive = FALSE;
    صحیح		res = FALSE;
    win_T	*wp;
    صحیح		save_کلید_Typed = کلید_Typed;
    صحیح		state;
    صحیح		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    اگر (popup_is_popup(curwin) && curbuf->b_term != پوچ)
	بازگشت FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    اگر (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	بازگشت FALSE;

    اگر (recursive)
	بازگشت FALSE;
    recursive = TRUE;

    اگر (c == K_LEFTMOUSE)
    {
	صحیح row = mouse_row;
	صحیح col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	اگر (wp != پوچ && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    وقتی (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != پوچ)
	اگر (wp->w_filter_cb.cb_name != پوچ
		&& (wp->w_filter_mode & state) != 0)
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
    بازگشت res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    صحیح
popup_no_mapping(تهی)
{
    صحیح	    round;
    win_T   *wp;

    برای (round = 1; round <= 2; ++round)
	برای (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != پوچ; wp = wp->w_next)
	    اگر (wp->w_filter_cb.cb_name != پوچ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    تهی
popup_check_cursor_pos(تهی)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    وقتی ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != پوچ)
	اگر (wp->w_popup_curwin != پوچ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ایستا تهی
popup_update_mask(win_T *wp, صحیح width, صحیح height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    صحیح		row, col;

    اگر (wp->w_popup_mask == پوچ || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	بازگشت;
    }
    اگر (wp->w_popup_mask_cells != پوچ
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	بازگشت;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    اگر (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = پوچ;
	بازگشت;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    اگر (wp->w_popup_mask_cells == پوچ)
	بازگشت;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_اقلام(wp->w_popup_mask, lio)
    {
	صحیح cols, cole;
	صحیح سطرها, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_عدد(&li->li_tv);
	اگر (cols < 0)
	    cols = width + cols + 1;
	اگر (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_عدد(&li->li_tv);
	اگر (cole < 0)
	    cole = width + cole + 1;
	اگر (cole > width)
	    cole = width;
	li = li->li_next;
	سطرها = tv_get_عدد(&li->li_tv);
	اگر (سطرها < 0)
	    سطرها = height + سطرها + 1;
	اگر (سطرها <= 0)
	    سطرها = 1;
	li = li->li_next;
	linee = tv_get_عدد(&li->li_tv);
	اگر (linee < 0)
	    linee = height + linee + 1;
	اگر (linee > height)
	    linee = height;

	برای (row = سطرها - 1; row < linee; ++row)
	    برای (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ایستا صحیح
popup_masked(win_T *wp, صحیح width, صحیح height, صحیح screencol, صحیح screenline)
{
    صحیح col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    صحیح سطر = screenline - wp->w_winrow;

    بازگشت col >= 0 && col < width
	    && سطر >= 0 && سطر < height
	    && wp->w_popup_mask_cells[سطر * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ایستا تهی
update_popup_transparent(win_T *wp, صحیح val)
{
    اگر (wp->w_popup_mask == پوچ)
	بازگشت;

    صحیح		width = popup_width(wp);
    صحیح		height = popup_height(wp);
    listitem_T	*lio, *li;
    صحیح		cols, cole;
    صحیح		سطرها, linee;
    صحیح		col, سطر;

    FOR_ALL_LIST_اقلام(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_عدد(&li->li_tv);
	اگر (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_عدد(&li->li_tv);
	اگر (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	سطرها = tv_get_عدد(&li->li_tv);
	اگر (سطرها < 0)
	    سطرها = height + سطرها + 1;
	li = li->li_next;
	linee = tv_get_عدد(&li->li_tv);
	اگر (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	اگر (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--سطرها;
	اگر (سطرها < 0)
	    سطرها = 0;
	برای (سطر = سطرها; سطر < linee
		&& سطر + wp->w_winrow < screen_Rows; ++سطر)
	    برای (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(سطر + wp->w_winrow) * screen_Columns
		    + col + wp->w_wincol] = val;
    }
}

/*
 * Only called when popup window "wp" is hidden: If the window is positioned
 * next to a text property, and it is now visible, then  unhide the popup.
 * We don't check if visible popups become hidden, that is done in
 * popup_adjust_position().
 * Return TRUE if the popup became unhidden.
 */
    ایستا صحیح
check_popup_unhidden(win_T *wp)
{
    اگر (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	اگر ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    بازگشت FALSE;
	اگر (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	وگرنه اگر (popup_find_prop_above_بالاترین(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	اگر (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ایستا صحیح
popup_need_position_adjust(win_T *wp)
{
    اگر (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	بازگشت TRUE;
    اگر (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	بازگشت TRUE;

    // May need to adjust the width if the cursor moved.
    بازگشت wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ایستا short *opacity_zindex = پوچ;
ایستا صحیح    opacity_zindex_rows = 0;
ایستا صحیح    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ایستا تهی
popup_mark_opacity_zindex(win_T *wp)
{
    صحیح	    width;
    صحیح	    height;
    صحیح	    r, c;

    اگر (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	بازگشت;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    برای (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	برای (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
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
    ایستا تهی
redraw_win_under_opacity_popup(win_T *wp)
{
    صحیح	    height;
    صحیح	    width;
    صحیح	    r;

    اگر (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	بازگشت;

    height = popup_height(wp);
    width = popup_width(wp);
    برای (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	صحیح	    col;
	win_T	    *prev_twp = پوچ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	برای (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    صحیح	    سطر_cp = r;
	    صحیح	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&سطر_cp, &col_cp, IGNORE_POPUP);
	    اگر (twp != پوچ && twp != prev_twp)
	    {
		prev_twp = twp;
		اگر (سطر_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    اگر (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    وگرنه
#endif
		    {
			(تهی)mouse_comp_pos(twp, &سطر_cp, &col_cp, &lnum,
									پوچ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		وگرنه اگر (سطر_cp == twp->w_height)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_status = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    صحیح
popup_is_under_opacity(صحیح row, صحیح col)
{
    اگر (opacity_zindex == پوچ
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	بازگشت FALSE;
    بازگشت opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    صحیح
popup_is_over_opacity(صحیح row, صحیح col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	اگر ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    بازگشت TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    صحیح
popup_is_under_opacity_range(صحیح row, صحیح start_col, صحیح end_col)
{
    صحیح col;

    اگر (opacity_zindex == پوچ
	    || row < 0 || row >= opacity_zindex_rows)
	بازگشت FALSE;
    برای (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	اگر (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    تهی
may_update_popup_mask(صحیح type)
{
    win_T	*wp;
    short	*mask;
    صحیح		سطر, col;
    صحیح		redraw_all_popups = FALSE;
    صحیح		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    اگر (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	وگرنه اگر (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	وگرنه اگر (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;

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
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
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
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	وگرنه
	{
	    اگر (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    اگر (opacity_zindex != پوچ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * اندازه(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    اگر (!popup_mask_refresh)
	بازگشت;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    اگر (redrawing_all_win)
	mask = popup_mask;
    وگرنه
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * اندازه(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    وقتی ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != پوچ)
    {
	صحیح width;
	صحیح height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	اگر (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    اگر (wp->w_popup_flags & POPF_HIDDEN)
		ادامه;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	وگرنه
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ادامه;

	{
	    صحیح mask_start = wp->w_winrow;
	    صحیح mask_end = mask_start + height;
	    صحیح mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    صحیح mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    برای (سطر = mask_start;
			    سطر < mask_end && سطر < screen_Rows; ++سطر)
		برای (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    اگر (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(سطر, col, FALSE))
			mask[سطر * screen_Columns + col] = POPUPMENU_ZINDEX;
		    وگرنه اگر (wp->w_popup_mask_cells == پوچ
				    || !popup_masked(wp, width, height, col, سطر))
			mask[سطر * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    اگر (mask == popup_mask_next)
    {
	صحیح	    *plines_cache = ALLOC_CLEAR_MULT(صحیح, Rows);
	win_T	    *prev_wp = پوچ;

	برای (سطر = 0; سطر < screen_Rows; ++سطر)
	{
	    صحیح	    col_done = 0;

	    برای (col = 0; col < screen_Columns; ++col)
	    {
		صحیح off = سطر * screen_Columns + col;

		اگر (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    اگر (سطر >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			اگر (!پیام_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    وگرنه اگر (col >= col_done)
		    {
			linenr_T	lnum;
			صحیح		سطر_cp = سطر;
			صحیح		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&سطر_cp, &col_cp, IGNORE_POPUP);
			اگر (wp != پوچ)
			{
#اگر defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    اگر (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    وگرنه
#endif
			    {
				اگر (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   اندازه(صحیح) * Rows);
				    prev_wp = wp;
				}

				اگر (سطر_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				وگرنه
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (تهی)mouse_comp_pos(wp, &سطر_cp, &col_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    col_done = wp->w_wincol + wp->w_width - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_uses_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    تهی
may_update_popup_position(تهی)
{
    اگر (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
ایستا schar_T *base_screenlines = پوچ;
ایستا صحیح *base_screenattrs = پوچ;
ایستا u8char_T *base_screenlinesuc = پوچ;
ایستا صحیح base_screen_rows = 0;
ایستا صحیح base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    صحیح
popup_get_base_screen_cell(صحیح row, صحیح col, schar_T *linep, صحیح *attrp,
							  u8char_T *ucp)
{
    اگر (base_screenlines == پوچ || base_screenattrs == پوچ)
	بازگشت FALSE;
    اگر (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	بازگشت FALSE;

    صحیح off = row * base_screen_cols + col;
    اگر (linep != پوچ)
	*linep = base_screenlines[off];
    اگر (attrp != پوچ)
	*attrp = base_screenattrs[off];
    اگر (ucp != پوچ)
    {
	اگر (enc_utf8 && base_screenlinesuc != پوچ)
	    *ucp = base_screenlinesuc[off];
	وگرنه
	    *ucp = 0;
    }
    بازگشت TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    تهی
popup_set_base_screen_cell(صحیح row, صحیح col, schar_T سطر, صحیح attr, u8char_T uc)
{
    اگر (base_screenlines == پوچ || base_screenattrs == پوچ)
	بازگشت;
    اگر (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	بازگشت;

    صحیح off = row * base_screen_cols + col;
    base_screenlines[off] = سطر;
    base_screenattrs[off] = attr;
    اگر (enc_utf8 && base_screenlinesuc != پوچ)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ایستا تهی
draw_opacity_padding_cell(
	صحیح		row,
	صحیح		col,
	popup_saved_screen_T *saved_screen,
	صحیح		pad_start_col,
	صحیح		pad_end_col)
{
    اگر (سطر_Offset == پوچ || Screen_سطرها == پوچ || ScreenAttrs == پوچ
	    || (enc_utf8 && Screen_سطرها_UC == پوچ))
	بازگشت;

    صحیح off = سطر_Offset[row] + col;
    صحیح r = row - saved_screen->start_row;
    صحیح c = col - saved_screen->start_col;

    اگر (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	صحیح save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	اگر (enc_utf8 && saved_screen->linesuc != پوچ)
	{
	    صحیح base_col = col - 1;
	    صحیح base_off = off - 1;
	    صحیح base_save_off = save_off - 1;
	    صحیح wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    اگر (base_off >= 0)
	    {
		اگر (Screen_سطرها_UC[base_off] != 0
			&& utf_char2cells(Screen_سطرها_UC[base_off]) == 2
			&& Screen_سطرها[off] == 0)
		    wide_prev = TRUE;
	    }
	    اگر (!wide_prev && save_off > 0)
	    {
		اگر (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->سطرها[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    اگر (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		اگر (base_col < pad_start_col)
		{
		    اگر (Screen_سطرها_UC[base_off] != 0
			    && utf_char2cells(Screen_سطرها_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_سطرها[base_off] = ' ';
			Screen_سطرها_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_سطرها[base_off],
				ScreenAttrs[base_off],
				Screen_سطرها_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_سطرها[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			اگر (enc_utf8)
			    Screen_سطرها_UC[off] = 0;
			صحیح popup_attr_val =
					get_win_attr(screen_opacity_popup);
			صحیح blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_سطرها[off], ScreenAttrs[off],
				Screen_سطرها_UC[off]);
			screen_char(off, row, col);
			بازگشت;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    اگر (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_سطرها[base_off] == ' '
			    && Screen_سطرها_UC[base_off] == 0)
		    {
			صحیح popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			صحیح blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_سطرها[base_off] =
				    saved_screen->سطرها[base_save_off];
			Screen_سطرها_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_سطرها[off] = 0;
			Screen_سطرها_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_سطرها[base_off],
				    ScreenAttrs[base_off],
				    Screen_سطرها_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_سطرها[off],
				    ScreenAttrs[off],
				    Screen_سطرها_UC[off]);
			screen_char(base_off, row, base_col);
			بازگشت;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_سطرها[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    اگر (enc_utf8)
			Screen_سطرها_UC[off] = 0;
		    صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_سطرها[off],
					       ScreenAttrs[off], Screen_سطرها_UC[off]);
		    screen_char(off, row, col);
		    بازگشت;
		}

		// Base cell is inside the saved area, redraw the wide char.
		اگر (save_off > 0)
		{
		    Screen_سطرها[base_off] = saved_screen->سطرها[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_سطرها[off] = saved_screen->سطرها[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_سطرها_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_سطرها_UC[off] = saved_screen->linesuc[save_off];

		    صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_سطرها[base_off],
					       ScreenAttrs[base_off], Screen_سطرها_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_سطرها[off],
					       ScreenAttrs[off], Screen_سطرها_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		بازگشت;
	    }
	}
	Screen_سطرها[off] = saved_screen->سطرها[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	اگر (enc_utf8)
	    Screen_سطرها_UC[off] = saved_screen->linesuc != پوچ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	اگر (enc_utf8 && Screen_سطرها_UC[off] != 0
		&& utf_char2cells(Screen_سطرها_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_سطرها[off] = ' ';
	    Screen_سطرها_UC[off] = 0;
	}

	صحیح popup_attr_val = get_win_attr(screen_opacity_popup);
	صحیح blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_سطرها[off],
				   ScreenAttrs[off], Screen_سطرها_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ایستا تهی
fill_opacity_padding(
	صحیح		start_row,
	صحیح		end_row,
	صحیح		start_col,
	صحیح		end_col,
	popup_saved_screen_T *saved_screen)
{
    برای (صحیح pad_row = start_row; pad_row < end_row; pad_row++)
	برای (صحیح pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    تهی
update_popups(تهی (*win_update)(win_T *wp))
{
    win_T   *wp;
    صحیح	    بالاترین_off;
    صحیح	    چپ_extra;
    صحیح	    total_width;
    صحیح	    total_height;
    صحیح	    بالاترین_padding;
    صحیح	    popup_attr;
    صحیح	    border_attr[4];
    صحیح	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    صحیح	    row;
    صحیح	    wincol;
    صحیح	    padcol = 0;
    صحیح	    padendcol = 0;
    صحیح	    i;
    صحیح	    sb_thumb_بالاترین = 0;
    صحیح	    sb_thumb_height = 0;
    صحیح	    attr_scroll = 0;
    صحیح	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    اگر (base_screenlines != پوچ)
    {
	vim_free(base_screenlines);
	base_screenlines = پوچ;
    }
    اگر (base_screenattrs != پوچ)
    {
	vim_free(base_screenattrs);
	base_screenattrs = پوچ;
    }
    اگر (base_screenlinesuc != پوچ)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = پوچ;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    وقتی ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != پوچ)
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

	override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	اگر ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	وگرنه
	    screen_opacity_popup = پوچ;

#ifdef FEAT_PROP_POPUP
	اگر (screen_opacity_popup != پوچ)
	{
	    اگر (base_screenlines != پوچ)
	    {
		vim_free(base_screenlines);
		base_screenlines = پوچ;
	    }
	    اگر (base_screenattrs != پوچ)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = پوچ;
	    }
	    اگر (base_screenlinesuc != پوچ)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = پوچ;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(صحیح,
				    base_screen_rows * base_screen_cols);
	    اگر (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    اگر (base_screenlines != پوچ && base_screenattrs != پوچ)
	    {
		برای (صحیح r = 0; r < base_screen_rows; r++)
		{
		    صحیح off = سطر_Offset[r];
		    صحیح base_off = r * base_screen_cols;
		    برای (صحیح c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_سطرها[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			اگر (enc_utf8 && base_screenlinesuc != پوچ)
			    base_screenlinesuc[base_off + c] =
					Screen_سطرها_UC[off + c];
		    }
		}
	    }
	}
#endif

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
	اگر (wp->w_wincol + چپ_extra < 0)
	    چپ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(بالاترین_off - wp->w_popup_topoff, 0);
	wp->w_wincol += چپ_extra;

	// Draw the popup text, unless it's off screen.
	اگر (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    اگر (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    وگرنه اگر (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(بالاترین_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= چپ_extra;

	// Add offset for border and padding if not done already.
	اگر ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += چپ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	اگر ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
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
	اگر (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	وگرنه
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	اگر (total_width < 0)
	    total_width = 0;
	اگر (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	وگرنه
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	اگر (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	وگرنه
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
	وگرنه
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    برای (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    اگر (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	برای (i = 0; i < 8; ++i)
	    اگر (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	برای (i = 0; i < 4; ++i)
	{
	    اگر (wp->w_border_highlight[i] != پوچ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    وگرنه اگر (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    وگرنه
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
	اگر (wp->w_popup_title != پوچ)
	{
	    صحیح	    title_attr;

	    اگر (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != پوچ)
		title_attr = border_attr[0];
	    وگرنه اگر (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    وگرنه
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    اگر (title_len > total_width - 2)
	    {
		صحیح	title_byte_len = (صحیح)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		اگر (title_text != پوچ)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    وگرنه
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	بالاترین_padding = cl.eff_padding[0];
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
			title_wincol + title_len, wincol + total_width,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    وگرنه
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	وگرنه اگر (cl.eff_padding[0] == 0 && popup_بالاترین_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    بالاترین_padding = 1;

	اگر (بالاترین_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    اگر (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	اگر (بالاترین_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    اگر (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		اگر (screen_opacity_popup != پوچ && saved_screen.سطرها != پوچ)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		وگرنه
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		بالاترین_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    اگر (screen_opacity_popup != پوچ && saved_screen.سطرها != پوچ)
	    {
		fill_opacity_padding(row, row + بالاترین_padding, padcol, padendcol,
			&saved_screen);
	    }
	    وگرنه
	    {
		screen_fill(row, row + بالاترین_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	اگر (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_سطر_شمار;
	    صحیح		height = wp->w_height;
	    صحیح		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    اگر (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    اگر (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    اگر (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_بالاترین = 0;
	    وگرنه
		sb_thumb_بالاترین = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    اگر (wp->w_topline > 1 && sb_thumb_بالاترین == 0 && height > 1)
		sb_thumb_بالاترین = 1;  // show it's scrolled
	    last = total_height - بالاترین_off - wp->w_popup_border[2];
	    اگر (sb_thumb_بالاترین >= last)
		// show at least one character
		sb_thumb_بالاترین = last - 1;

	    اگر (wp->w_scrollbar_highlight != پوچ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    وگرنه
		attr_scroll = highlight_attr[HLF_PSB];
	    اگر (wp->w_thumb_highlight != پوچ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    وگرنه
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	برای (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    صحیح	pad_چپ;
	    // left and right padding only needed next to the body
	    صحیح do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    اگر (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    اگر (do_padding && cl.eff_padding[3] > 0)
	    {
		صحیح col = wincol + wp->w_popup_border[3];

		// left padding
		pad_چپ = wp->w_popup_padding[3];
		اگر (col < 0)
		{
		    pad_چپ += col;
		    col = 0;
		}
		اگر (pad_چپ > 0)
		{
		    اگر (screen_opacity_popup != پوچ && saved_screen.سطرها != پوچ)
			fill_opacity_padding(row, row + 1, col, col + pad_چپ,
				&saved_screen);
		    وگرنه
			screen_fill(row, row + 1, col, col + pad_چپ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    اگر (wp->w_has_scrollbar)
	    {
		صحیح سطر = i - بالاترین_off;
		صحیح scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		اگر (سطر >= 0 && سطر < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    سطر >= sb_thumb_بالاترین
				       && سطر < sb_thumb_بالاترین + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		وگرنه
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    اگر (do_padding && cl.eff_padding[1] > 0)
	    {
		صحیح pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		صحیح pad_col_end = pad_col_start + wp->w_popup_padding[1];

		اگر (screen_opacity_popup != پوچ && saved_screen.سطرها != پوچ)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		وگرنه
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	اگر (wp->w_popup_shadow)
	{
	    صحیح col = wincol + total_width;
	    برای (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
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
	    row = wp->w_winrow + total_height
		    - cl.eff_padding[2] - cl.eff_border[2];
	    اگر (screen_opacity_popup != پوچ && saved_screen.سطرها != پوچ)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    وگرنه
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	اگر (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    اگر (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	اگر (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    برای (صحیح col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	اگر (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = پوچ;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	اگر (override_success)
	    برداشت_highlight_overrides();

#ifdef FEAT_IMAGE_POPUP
	popup_position_image(wp);
#endif
    }

#ifdef FEAT_PROP_POPUP
    VIM_CLEAR(base_screenlines);
    VIM_CLEAR(base_screenattrs);
    VIM_CLEAR(base_screenlinesuc);
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

#اگر defined(FEAT_جستجو_EXTRA)
    // In case win_update() called start_search_hl().
    end_جستجو_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    ایستا صحیح
set_ref_in_one_popup(win_T *wp, صحیح copyID)
{
    صحیح		abort = FALSE;
    typval_T	tv;

    اگر (wp->w_close_cb.cb_partial != پوچ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
    }
    اگر (wp->w_filter_cb.cb_partial != پوچ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    بازگشت abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    صحیح
set_ref_in_popups(صحیح copyID)
{
    صحیح		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    برای (wp = first_popupwin; !abort && wp != پوچ; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	برای (wp = tp->tp_first_popupwin; !abort && wp != پوچ; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	اگر (abort)
	    بشکن;
    }
    بازگشت abort;
}

    صحیح
popup_is_popup(win_T *wp)
{
    بازگشت wp->w_popup_flags != 0;
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(تهی)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_p_pvw)
	    بازگشت wp;
    بازگشت پوچ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(تهی)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_popup_flags & POPF_INFO)
	    بازگشت wp;
    بازگشت پوچ;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(صحیح dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    صحیح		by;
    linenr_T	new_topline;

    اگر (wp == پوچ)
	بازگشت false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    اگر (new_topline < 1)
	new_topline = 1;
    اگر (new_topline > wp->w_buffer->b_ml.ml_سطر_شمار)
	new_topline = wp->w_buffer->b_ml.ml_سطر_شمار;
    اگر (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    بازگشت true;
#وگرنه
    بازگشت false;
#endif
}

    تهی
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_عدد = message_win == پوچ ? 0 : message_win->w_id;
#وگرنه
    rettv->vval.v_عدد = 0;
#endif
}

    تهی
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_عدد = wp == پوچ ? 0 : wp->w_id;
#وگرنه
    rettv->vval.v_عدد = 0;
#endif
}

    تهی
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_عدد = wp == پوچ ? 0 : wp->w_id;
#وگرنه
    rettv->vval.v_عدد = 0;
#endif
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    صحیح
popup_create_preview_window(صحیح info)
{
    win_T *wp = popup_create(پوچ, پوچ, info ? TYPE_INFO : TYPE_PREVIEW);

    اگر (wp == پوچ)
	بازگشت FAIL;
    اگر (info)
	wp->w_popup_flags |= POPF_INFO;
    وگرنه
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    اگر (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    وگرنه اگر (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    وگرنه
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    بازگشت OK;
}

/*
 * Close any preview popup.
 */
    تهی
popup_close_preview(تهی)
{
    win_T *wp = popup_find_preview_window();

    اگر (wp != پوچ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    تهی
popup_hide_info(تهی)
{
    win_T *wp = popup_find_info_window();

    اگر (wp != پوچ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	اگر (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    تهی
popup_close_info(تهی)
{
    win_T *wp = popup_find_info_window();

    اگر (wp != پوچ)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    صحیح
popup_overlaps_cmdline(تهی)
{
    بازگشت popup_on_cmdline;
}

#اگر defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(تهی)
{
    اگر (message_win != پوچ)
	بازگشت message_win;

    صحیح i;

    message_win = popup_create(پوچ, پوچ, TYPE_MESSAGE_WIN);

    اگر (message_win == پوچ)
	بازگشت پوچ;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    برای (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    برای (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    اگر (message_win->w_popup_timer != پوچ)
	message_win->w_popup_timer->tr_keep = TRUE;
    بازگشت message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    تهی
popup_show_message_win(تهی)
{
    اگر (message_win == پوچ)
	بازگشت;

    اگر ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    ایستا تهی
may_start_message_win_timer(win_T *wp)
{
    اگر (wp == message_win && start_message_win_timer)
    {
	اگر (message_win->w_popup_timer != پوچ)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    صحیح
popup_message_win_visible(تهی)
{
    بازگشت message_win != پوچ
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    تهی
popup_hide_message_win(تهی)
{
    اگر (message_win != پوچ)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ایستا صحیح save_پیام_didout = FALSE;
ایستا صحیح save_پیام_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ایستا صحیح ew_پیام_didout = FALSE;
ایستا صحیح ew_پیام_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    تهی
start_echowindow(صحیح time_sec)
{
    in_echowindow = TRUE;
    save_پیام_didout = پیام_didout;
    save_پیام_col = پیام_col;
    پیام_didout = ew_پیام_didout;
    پیام_col = ew_پیام_col;
    اگر (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    تهی
end_echowindow(تهی)
{
    in_echowindow = FALSE;

    اگر ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_پیام_didout = TRUE;
    ew_پیام_col = پیام_col == 0 ? 1 : پیام_col;
    پیام_didout = save_پیام_didout;
    پیام_col = save_پیام_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    صحیح
popup_win_closed(win_T *win)
{
    صحیح	    round;
    win_T   *wp;
    win_T   *next;
    صحیح	    ret = FALSE;

    برای (round = 1; round <= 2; ++round)
	برای (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != پوچ; wp = next)
	{
	    next = wp->w_next;
	    اگر (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    بازگشت ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    تهی
popup_set_title(win_T *wp)
{
    اگر (wp->w_buffer->b_fname == پوچ)
	بازگشت;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    اگر (wp->w_popup_title != پوچ)
	vim_snprintf((نویسه *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#اگر defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    تهی
popup_update_preview_title(تهی)
{
    win_T *wp = popup_find_preview_window();

    اگر (wp != پوچ)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
