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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_PROP_POPUP)

തരനിർവചനം ഘടന {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		active;
    പൂർണ്ണസംഖ്യ		winrow;
    പൂർണ്ണസംഖ്യ		wincol;
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		leftoff;
    പൂർണ്ണസംഖ്യ		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
തരനിർവചനം ഘടന {
    schar_T	*വരികൾ;
    പൂർണ്ണസംഖ്യ		*attrs;
    u8char_T	*linesuc;
    പൂർണ്ണസംഖ്യ		start_row;
    പൂർണ്ണസംഖ്യ		start_col;
    പൂർണ്ണസംഖ്യ		rows;
    പൂർണ്ണസംഖ്യ		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
തരനിർവചനം ഘടന {
    linenr_T	firstline;
    പൂർണ്ണസംഖ്യ		blend;
    പൂർണ്ണസംഖ്യ		flags;
    പൂർണ്ണസംഖ്യ		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		winrow;
    പൂർണ്ണസംഖ്യ		wincol;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		leftcol;
    പൂർണ്ണസംഖ്യ		leftoff;
    പൂർണ്ണസംഖ്യ		has_scrollbar;
    പൂർണ്ണസംഖ്യ		topoff;
    പൂർണ്ണസംഖ്യ		bottomoff;
    പൂർണ്ണസംഖ്യ		leftclip;
    പൂർണ്ണസംഖ്യ		rightclip;
} popup_layout_T;

സ്ഥിതം poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
സ്ഥിതം win_T *message_win = ശൂന്യ;

// Time used for the next ":echowindow" message in msec.
സ്ഥിതം പൂർണ്ണസംഖ്യ  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
സ്ഥിതം പൂർണ്ണസംഖ്യ    start_message_win_timer = FALSE;

സ്ഥിതം ശൂന്യം may_start_message_win_timer(win_T *wp);
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ popup_on_cmdline = FALSE;

സ്ഥിതം ശൂന്യം popup_adjust_position(win_T *wp);
സ്ഥിതം bool popup_area_changed(win_T *wp, popup_area_T *area);
സ്ഥിതം ശൂന്യം popup_redraw_exposed_area(popup_area_T *area);
സ്ഥിതം ശൂന്യം popup_save_area(win_T *wp, popup_area_T *area);
സ്ഥിതം ശൂന്യം popup_free_saved_screen(popup_saved_screen_T *saved_screen);
സ്ഥിതം ശൂന്യം popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
സ്ഥിതം bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
സ്ഥിതം bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
സ്ഥിതം ശൂന്യം popup_save_style(win_T *wp, popup_style_snapshot_T *style);
സ്ഥിതം ശൂന്യം popup_save_layout(win_T *wp, popup_layout_T *layout);
സ്ഥിതം ശൂന്യം redraw_under_popup_area(പൂർണ്ണസംഖ്യ winrow, പൂർണ്ണസംഖ്യ wincol, പൂർണ്ണസംഖ്യ height,
	പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ leftoff);
സ്ഥിതം ശൂന്യം redraw_overlapped_opacity_popups(പൂർണ്ണസംഖ്യ winrow, പൂർണ്ണസംഖ്യ wincol,
	പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ leftoff, പൂർണ്ണസംഖ്യ zindex);
സ്ഥിതം ശൂന്യം redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_options_one(dict_T *dict, char_u *താക്കോൽ)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    പൂർണ്ണസംഖ്യ		n = 0;

    di = dict_find(dict, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ MAXCOL;

    val = tv_get_string(&di->di_tv);
    എങ്കിൽ (STRNCMP(val, "cursor", 6) != 0)
	തിരികെ dict_get_സംഖ്യ_check(dict, താക്കോൽ);

    setcursor_mayforce(TRUE);
    s = val + 6;
    എങ്കിൽ (*s != NUL)
    {
	endp = s;
	എങ്കിൽ (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((അക്ഷരം *)s, (അക്ഷരം **)&endp, 10);
	എങ്കിൽ (endp != ശൂന്യ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    തിരികെ 0;
	}
    }

    എങ്കിൽ (STRCMP(താക്കോൽ, "line") == 0)
	n = screen_screenrow() + 1 + n;
    അല്ലെങ്കിൽ // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    എങ്കിൽ (n == 0)
	n = -1;
    തിരികെ n;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_padding_border(dict_T *dict, പൂർണ്ണസംഖ്യ *array, അക്ഷരം *name, പൂർണ്ണസംഖ്യ max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ OK;

    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	തിരികെ FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		nr;

    വേണ്ടി (i = 0; i < 4; ++i)
	array[i] = 1;
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ OK;

    CHECK_LIST_MATERIALIZE(list);
    വേണ്ടി (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    തിരികെ OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    സ്ഥിതം ശൂന്യം
set_moved_മൂല്യങ്ങൾ(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    സ്ഥിതം ശൂന്യം
set_moved_columns(win_T *wp, പൂർണ്ണസംഖ്യ flags)
{
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    പൂർണ്ണസംഖ്യ		mincol;

    എങ്കിൽ (len <= 0)
	തിരികെ;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (പൂർണ്ണസംഖ്യ)(ptr - ml_get_curline());
    എങ്കിൽ (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	തിരികെ;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    സ്ഥിതം ശൂന്യം
set_mousemoved_മൂല്യങ്ങൾ(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    സ്ഥിതം ശൂന്യം
update_popup_uses_mouse_move(ശൂന്യം)
{
    popup_uses_mouse_move = FALSE;
    എങ്കിൽ (!popup_visible)
	തിരികെ;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    തിരികെ;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    തിരികെ;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    സ്ഥിതം ശൂന്യം
set_mousemoved_columns(win_T *wp, പൂർണ്ണസംഖ്യ flags)
{
    win_T	*textwp;
    char_u	*text;
    പൂർണ്ണസംഖ്യ		col;
    pos_T	pos;
    colnr_T	mcol;

    എങ്കിൽ (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, ശൂന്യ, &col) != OK)
	തിരികെ;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ശൂന്യ, ശൂന്യ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, ശൂന്യ, ശൂന്യ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    പൂർണ്ണസംഖ്യ
popup_on_border(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    തിരികെ (row == 0 && wp->w_popup_border[0] > 0)
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
    പൂർണ്ണസംഖ്യ
popup_close_if_on_X(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

// Values set when dragging a popup window starts.
സ്ഥിതം പൂർണ്ണസംഖ്യ drag_start_row;
സ്ഥിതം പൂർണ്ണസംഖ്യ drag_start_col;
സ്ഥിതം പൂർണ്ണസംഖ്യ drag_start_wantline;
സ്ഥിതം പൂർണ്ണസംഖ്യ drag_start_wantcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    ശൂന്യം
popup_start_drag(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    എങ്കിൽ (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    അല്ലെങ്കിൽ
	drag_start_wantline = wp->w_wantline;
    എങ്കിൽ (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    അല്ലെങ്കിൽ
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    എങ്കിൽ (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    എങ്കിൽ (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	എങ്കിൽ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	എങ്കിൽ (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    ശൂന്യം
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    എങ്കിൽ (!win_valid_popup(wp))
	തിരികെ;

    എങ്കിൽ ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	പൂർണ്ണസംഖ്യ width_inc = mouse_col - drag_start_col;
	പൂർണ്ണസംഖ്യ height_inc = mouse_row - drag_start_row;

	എങ്കിൽ (width_inc != 0)
	{
	    പൂർണ്ണസംഖ്യ width = wp->w_width + width_inc;

	    എങ്കിൽ (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	എങ്കിൽ (height_inc != 0)
	{
	    പൂർണ്ണസംഖ്യ height = wp->w_height + height_inc;

	    എങ്കിൽ (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	തിരികെ;
    }

    എങ്കിൽ (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	തിരികെ;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    എങ്കിൽ (wp->w_wantline < 1)
	wp->w_wantline = 1;
    എങ്കിൽ (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    എങ്കിൽ (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    എങ്കിൽ (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    ശൂന്യം
popup_set_firstline(win_T *wp)
{
    പൂർണ്ണസംഖ്യ	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    ആകുമ്പോൾ (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    പൂർണ്ണസംഖ്യ
popup_is_in_scrollbar(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    തിരികെ wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    ശൂന്യം
popup_handle_scrollbar_click(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (!popup_is_in_scrollbar(wp, row, col))
	തിരികെ;

    പൂർണ്ണസംഖ്യ	    height = popup_height(wp);
    പൂർണ്ണസംഖ്യ	    new_topline = wp->w_topline;

    എങ്കിൽ (row >= height / 2)
    {
	// Click in lower half, scroll down.
	എങ്കിൽ (wp->w_topline < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    ++new_topline;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    എങ്കിൽ (new_topline == wp->w_topline)
	തിരികെ;

    set_topline(wp, new_topline);
    എങ്കിൽ (wp == curwin)
    {
	എങ്കിൽ (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#എങ്കിൽ defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    സ്ഥിതം ശൂന്യം
popup_add_timeout(win_T *wp, പൂർണ്ണസംഖ്യ time, പൂർണ്ണസംഖ്യ close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((അക്ഷരം *)cbbuf, വലുപ്പം(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    എങ്കിൽ (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	തിരികെ;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    എങ്കിൽ (cb.cb_name != ശൂന്യ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    സ്ഥിതം poppos_T
get_pos_entry(dict_T *d, പൂർണ്ണസംഖ്യ give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    പൂർണ്ണസംഖ്യ	    nr;

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ POPPOS_NONE;

    വേണ്ടി (nr = 0; nr < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(poppos_entries); ++nr)
	എങ്കിൽ (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    തിരികെ poppos_entries[nr].pp_val;

    എങ്കിൽ (give_error)
	semsg(_(e_invalid_argument_str), str);
    തിരികെ POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    സ്ഥിതം ശൂന്യം
apply_move_options(win_T *wp, dict_T *d)
{
    പൂർണ്ണസംഖ്യ		nr;
    char_u	*str;
    dictitem_T	*di;

    എങ്കിൽ ((nr = dict_get_സംഖ്യ_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    എങ്കിൽ ((nr = dict_get_സംഖ്യ_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    എങ്കിൽ ((nr = dict_get_സംഖ്യ_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    എങ്കിൽ ((nr = dict_get_സംഖ്യ_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    എങ്കിൽ (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    എങ്കിൽ (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    എങ്കിൽ (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	എങ്കിൽ (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    എങ്കിൽ (str != ശൂന്യ)
    {
	wp->w_popup_prop_type = 0;
	എങ്കിൽ (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    എങ്കിൽ (di != ശൂന്യ)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		എങ്കിൽ (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    എങ്കിൽ (nr <= 0)
		nr = find_prop_type_id(str, ശൂന്യ);
	    എങ്കിൽ (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    അല്ലെങ്കിൽ
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    എങ്കിൽ (di != ശൂന്യ)
	wp->w_popup_prop_id = dict_get_സംഖ്യ(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    സ്ഥിതം ശൂന്യം
handle_moved_argument(win_T *wp, dictitem_T *di, പൂർണ്ണസംഖ്യ mousemoved)
{
    എങ്കിൽ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ശൂന്യ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	പൂർണ്ണസംഖ്യ	flags = 0;

	എങ്കിൽ (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	എങ്കിൽ (flags != 0)
	{
	    എങ്കിൽ (mousemoved)
		set_mousemoved_columns(wp, flags);
	    അല്ലെങ്കിൽ
		set_moved_columns(wp, flags);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ശൂന്യ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	പൂർണ്ണസംഖ്യ	    mincol;
	പൂർണ്ണസംഖ്യ	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	എങ്കിൽ (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_സംഖ്യ(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    എങ്കിൽ (mousemoved)
		wp->w_popup_mouse_row = nr;
	    അല്ലെങ്കിൽ
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    എങ്കിൽ (nr == 0)
		wp->w_popup_curwin = ശൂന്യ;
	}

	mincol = tv_get_സംഖ്യ(&li->li_tv);
	maxcol = tv_get_സംഖ്യ(&li->li_next->li_tv);
	എങ്കിൽ (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	അല്ലെങ്കിൽ
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    സ്ഥിതം ശൂന്യം
check_highlight(dict_T *dict, അക്ഷരം *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_മൂല്യം_for_argument_str), name);
    അല്ലെങ്കിൽ
    {
	str = tv_get_string(&di->di_tv);
	എങ്കിൽ (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    സ്ഥിതം ശൂന്യം
popup_show_curline(win_T *wp)
{
    എങ്കിൽ (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	എങ്കിൽ (wp->w_topline < 1)
	    wp->w_topline = 1;
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_topline > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    wp->w_topline = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	ആകുമ്പോൾ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_വരി_എണ്ണം
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    എങ്കിൽ (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    സ്ഥിതം char_u *
popup_get_sign_name(win_T *wp)
{
    സ്ഥിതം അക്ഷരം    buf[30];

    vim_snprintf(buf, വലുപ്പം(buf), "popup-%d", wp->w_id);
    തിരികെ (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    സ്ഥിതം ശൂന്യം
popup_highlight_curline(win_T *wp)
{
    പൂർണ്ണസംഖ്യ	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    എങ്കിൽ ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	എങ്കിൽ (!sign_exists_by_name(sign_name))
	{
	    അക്ഷരം *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ശൂന്യ, (char_u *)linehl, ശൂന്യ, ശൂന്യ, ശൂന്യ,
		    ശൂന്യ, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    അല്ലെങ്കിൽ
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	wp->w_firstline = dict_get_സംഖ്യ(dict, "firstline");
	എങ്കിൽ (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    എങ്കിൽ (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    എങ്കിൽ (str != ശൂന്യ)
    {
	char_u	*title = vim_strsave(str);

	എങ്കിൽ (title != ശൂന്യ)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    എങ്കിൽ (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	nr = dict_get_സംഖ്യ(dict, "opacity");
	എങ്കിൽ (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	അല്ലെങ്കിൽ
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    എങ്കിൽ (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    അല്ലെങ്കിൽ
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ശൂന്യ)
    {
	image_T		    *img;
	image_T		    *cur_img = ശൂന്യ;
	image_placement_T   *place;
	പൂർണ്ണസംഖ്യ		    cw, ch;

	എങ്കിൽ (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ശൂന്യ;
	    }
	    പോകുക image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ && wp->w_popup_imagep->img != ശൂന്യ)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	എങ്കിൽ (img == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (img == cur_img)
	    പോകുക image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	എങ്കിൽ (place == ശൂന്യ)
	{
	    image_unref(img);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	എങ്കിൽ (dict_find(dict, (char_u *)"minwidth", -1) == ശൂന്യ)
	    wp->w_minwidth = cw;
	എങ്കിൽ (dict_find(dict, (char_u *)"maxwidth", -1) == ശൂന്യ)
	    wp->w_maxwidth = cw;
	എങ്കിൽ (dict_find(dict, (char_u *)"minheight", -1) == ശൂന്യ)
	    wp->w_minheight = ch;
	എങ്കിൽ (dict_find(dict, (char_u *)"maxheight", -1) == ശൂന്യ)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    അല്ലെങ്കിൽ
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ശൂന്യ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ ok = TRUE;

	എങ്കിൽ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ശൂന്യ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    എങ്കിൽ (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    അല്ലെങ്കിൽ
		ok = FALSE;
	}
	അല്ലെങ്കിൽ
	    ok = FALSE;
	എങ്കിൽ (!ok)
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    എങ്കിൽ (str != ശൂന്യ)
    {
	അക്ഷരം *errmsg = update_wincolor(wp, str);

	എങ്കിൽ (errmsg == ശൂന്യ)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	അല്ലെങ്കിൽ
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    എങ്കിൽ (str != ശൂന്യ)
    {
	അക്ഷരം *errmsg = update_winhighlight(wp, str);

	എങ്കിൽ (errmsg == ശൂന്യ)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	അല്ലെങ്കിൽ
	    emsg(_(errmsg));
    }

    എങ്കിൽ (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	തിരികെ FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	എങ്കിൽ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ശൂന്യ)
	{
	    emsg(_(e_list_required));
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    പൂർണ്ണസംഖ്യ		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    എങ്കിൽ (list->lv_len == 0)
	    {
		വേണ്ടി (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ശൂന്യ;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    എങ്കിൽ (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			എങ്കിൽ (hl != ശൂന്യ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		എങ്കിൽ (list->lv_len == 1 && wp->w_border_highlight[0] != ശൂന്യ)
		    വേണ്ടി (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			എങ്കിൽ (hl != ശൂന്യ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    പൂർണ്ണസംഖ്യ		i;

	    എങ്കിൽ (list != ശൂന്യ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		വേണ്ടി (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    എങ്കിൽ (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		എങ്കിൽ (list->lv_len == 1)
		    വേണ്ടി (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		എങ്കിൽ (list->lv_len == 2)
		{
		    വേണ്ടി (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    വേണ്ടി (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	wp->w_zindex = dict_get_സംഖ്യ(dict, "zindex");
	എങ്കിൽ (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	എങ്കിൽ (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ ok = FALSE;

	എങ്കിൽ (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ശൂന്യ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ഇനങ്ങൾ(di->di_tv.vval.v_list, li)
	    {
		എങ്കിൽ (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ശൂന്യ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	എങ്കിൽ (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "mask");
	    തിരികെ FAIL;
	}
    }

#എങ്കിൽ defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_സംഖ്യ(dict, "time");
    എങ്കിൽ (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	set_moved_മൂല്യങ്ങൾ(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	set_mousemoved_മൂല്യങ്ങൾ(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    എങ്കിൽ (di != ശൂന്യ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	എങ്കിൽ (callback.cb_name != ശൂന്യ)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    എങ്കിൽ (nr != -1)
    {
	എങ്കിൽ (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    എങ്കിൽ (str != ശൂന്യ)
    {
	എങ്കിൽ (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	അല്ലെങ്കിൽ
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ OK;

    callback_T	callback = get_callback(&di->di_tv);
    എങ്കിൽ (callback.cb_name == ശൂന്യ)
	തിരികെ OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    തിരികെ OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
apply_options(win_T *wp, dict_T *dict, പൂർണ്ണസംഖ്യ create)
{
    പൂർണ്ണസംഖ്യ		nr;

    apply_move_options(wp, dict);

    എങ്കിൽ (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    എങ്കിൽ (apply_general_options(wp, dict) == FAIL)
	തിരികെ FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    എങ്കിൽ (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    എങ്കിൽ (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	എങ്കിൽ (wp->w_firstline > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    എങ്കിൽ (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    തിരികെ OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    സ്ഥിതം ശൂന്യം
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
	എങ്കിൽ (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ശൂന്യ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    സ്ഥിതം ശൂന്യം
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    തിരികെ;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ശൂന്യ ? ശൂന്യ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ശൂന്യ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    വേണ്ടി (li = l->lv_first; li != ശൂന്യ; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	എങ്കിൽ (di != ശൂന്യ)
	{
	    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		തിരികെ;
	    }
	    plist = di->di_tv.vval.v_list;
	    എങ്കിൽ (plist != ശൂന്യ)
	    {
		FOR_ALL_LIST_ഇനങ്ങൾ(plist, pli)
		{
		    എങ്കിൽ (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			തിരികെ;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    എങ്കിൽ (dict != ശൂന്യ)
		    {
			പൂർണ്ണസംഖ്യ col = dict_get_സംഖ്യ(dict, "col");

			prop_add_common(lnum, col, dict, buf, ശൂന്യ);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    പൂർണ്ണസംഖ്യ
popup_മുകൾ_extra(win_T *wp)
{
    പൂർണ്ണസംഖ്യ	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    എങ്കിൽ (extra == 0 && wp->w_popup_title != ശൂന്യ && *wp->w_popup_title != NUL)
	തിരികെ 1;
    തിരികെ extra;
}

/*
 * Get the padding plus border at the left.
 */
    പൂർണ്ണസംഖ്യ
popup_ഇടത്_extra(win_T *wp)
{
    തിരികെ wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    പൂർണ്ണസംഖ്യ
popup_height(win_T *wp)
{
    തിരികെ wp->w_height
	    + popup_മുകൾ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    പൂർണ്ണസംഖ്യ
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    തിരികെ wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    പൂർണ്ണസംഖ്യ
popup_extra_width(win_T *wp)
{
    തിരികെ wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    സ്ഥിതം win_T *
popup_get_clipwin(win_T *wp)
{
    എങ്കിൽ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	തിരികെ ശൂന്യ;
    എങ്കിൽ (win_valid(wp->w_popup_prop_win))
	തിരികെ wp->w_popup_prop_win;
    തിരികെ ശൂന്യ;
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
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ മുകൾ_extra;
    പൂർണ്ണസംഖ്യ bot_extra;
    പൂർണ്ണസംഖ്യ ഇടത്_extra;
    പൂർണ്ണസംഖ്യ വലത്_extra;

    പൂർണ്ണസംഖ്യ clip_മുകൾ_content;
    പൂർണ്ണസംഖ്യ clip_bot_content;
    പൂർണ്ണസംഖ്യ clip_ഇടത്_content;
    പൂർണ്ണസംഖ്യ clip_വലത്_content;

    പൂർണ്ണസംഖ്യ eff_മുകൾ_extra;
    പൂർണ്ണസംഖ്യ eff_bot_extra;
    പൂർണ്ണസംഖ്യ eff_ഇടത്_extra;
    പൂർണ്ണസംഖ്യ eff_വലത്_extra;

    പൂർണ്ണസംഖ്യ eff_border[4];
    പൂർണ്ണസംഖ്യ eff_padding[4];

    പൂർണ്ണസംഖ്യ eff_height;
    പൂർണ്ണസംഖ്യ eff_width;
} popup_clip_T;

    സ്ഥിതം ശൂന്യം
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    പൂർണ്ണസംഖ്യ h, w;

    cl->മുകൾ_extra = popup_മുകൾ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ഇടത്_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->വലത്_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_മുകൾ_content = wp->w_popup_topoff - cl->മുകൾ_extra;
    എങ്കിൽ (cl->clip_മുകൾ_content < 0)
	cl->clip_മുകൾ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    എങ്കിൽ (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ഇടത്_content = wp->w_popup_leftclip - cl->ഇടത്_extra;
    എങ്കിൽ (cl->clip_ഇടത്_content < 0)
	cl->clip_ഇടത്_content = 0;
    cl->clip_വലത്_content = wp->w_popup_rightclip - cl->വലത്_extra;
    എങ്കിൽ (cl->clip_വലത്_content < 0)
	cl->clip_വലത്_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	പൂർണ്ണസംഖ്യ clip = wp->w_popup_topoff;
	പൂർണ്ണസംഖ്യ b = wp->w_popup_border[0];
	പൂർണ്ണസംഖ്യ p = wp->w_popup_padding[0];
	പൂർണ്ണസംഖ്യ rem;

	എങ്കിൽ (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	അല്ലെങ്കിൽ
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	പൂർണ്ണസംഖ്യ clip = wp->w_popup_bottomoff;
	പൂർണ്ണസംഖ്യ b = wp->w_popup_border[2];
	പൂർണ്ണസംഖ്യ p = wp->w_popup_padding[2];
	പൂർണ്ണസംഖ്യ rem;

	എങ്കിൽ (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	അല്ലെങ്കിൽ
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
	പൂർണ്ണസംഖ്യ excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	എങ്കിൽ (excess > 0)
	{
	    എങ്കിൽ (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		എങ്കിൽ (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		അല്ലെങ്കിൽ
		    cl->eff_border[0] -= excess;
	    }
	    അല്ലെങ്കിൽ
		cl->eff_padding[0] -= excess;
	}
    }
    {
	പൂർണ്ണസംഖ്യ excess = wp->w_popup_topoff - cl->മുകൾ_extra - wp->w_height;
	എങ്കിൽ (excess > 0)
	{
	    എങ്കിൽ (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		എങ്കിൽ (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		അല്ലെങ്കിൽ
		    cl->eff_border[2] -= excess;
	    }
	    അല്ലെങ്കിൽ
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_മുകൾ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ഇടത്_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ഇടത്_extra;
    cl->eff_വലത്_extra = wp->w_popup_rightclip > 0 ? 0 : cl->വലത്_extra;

    h = wp->w_height - cl->clip_മുകൾ_content - cl->clip_bot_content;
    എങ്കിൽ (h < 0)
	h = 0;
    cl->eff_height = cl->eff_മുകൾ_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_ഇടത്_content - cl->clip_വലത്_content;
    എങ്കിൽ (w < 0)
	w = 0;
    cl->eff_width = cl->eff_ഇടത്_extra + w + cl->eff_വലത്_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		winrow;
    പൂർണ്ണസംഖ്യ		wincol;
    പൂർണ്ണസംഖ്യ		leftcol;
    പൂർണ്ണസംഖ്യ		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    സ്ഥിതം ശൂന്യം
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

    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
popup_screenpos_above_മുകൾ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	പൂർണ്ണസംഖ്യ	    *screen_row,
	പൂർണ്ണസംഖ്യ	    *screen_scol,
	പൂർണ്ണസംഖ്യ	    *screen_ccol,
	പൂർണ്ണസംഖ്യ	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    പൂർണ്ണസംഖ്യ	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (പൂർണ്ണസംഖ്യ)win_col_off(prop_win) - (പൂർണ്ണസംഖ്യ)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (പൂർണ്ണസംഖ്യ)scol + coloff;
    *screen_ccol = (പൂർണ്ണസംഖ്യ)ccol + coloff;
    *screen_ecol = (പൂർണ്ണസംഖ്യ)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (പൂർണ്ണസംഖ്യ)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    സ്ഥിതം ശൂന്യം
popup_hide_for_textprop(win_T *wp)
{
    എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	തിരികെ;
    wp->w_popup_flags |= POPF_HIDDEN;
    എങ്കിൽ (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    സ്ഥിതം bool
popup_find_prop_above_മുകൾ(
	win_T	    *wp,
	win_T	    *prop_win,
	പൂർണ്ണസംഖ്യ	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    എങ്കിൽ (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	തിരികെ false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    എങ്കിൽ (first < 1)
	first = 1;
    തിരികെ find_prop_in_വരികൾ(prop_win,
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
    സ്ഥിതം bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    പൂർണ്ണസംഖ്യ	    extra_h, extra_w;
    പൂർണ്ണസംഖ്യ	    popup_മുകൾ, popup_bottom, popup_ഇടത്, popup_വലത്;
    പൂർണ്ണസംഖ്യ	    total_h, total_w;

    എങ്കിൽ (cw == ശൂന്യ)
	തിരികെ false;

    extra_h = popup_മുകൾ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_മുകൾ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_ഇടത് = wp->w_wincol;
    popup_വലത് = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    എങ്കിൽ (popup_മുകൾ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_മുകൾ;
    എങ്കിൽ (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    എങ്കിൽ (popup_ഇടത് < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ഇടത്;
    എങ്കിൽ (popup_വലത് > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_വലത് - (cw->w_wincol + cw->w_width);

    തിരികെ wp->w_popup_topoff >= total_h
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
    സ്ഥിതം ശൂന്യം
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    എങ്കിൽ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_മുകൾ_content + cl->clip_bot_content;
	എങ്കിൽ (wp->w_height < 0)
	    wp->w_height = 0;
	എങ്കിൽ (cl->clip_മുകൾ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_മുകൾ_content;
    }
    എങ്കിൽ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	എങ്കിൽ (cl->clip_ഇടത്_content > 0 || cl->clip_വലത്_content > 0)
	    wp->w_p_wrap = FALSE;
	എങ്കിൽ (cl->clip_വലത്_content > 0)
	{
	    wp->w_width -= cl->clip_വലത്_content;
	    എങ്കിൽ (wp->w_width < 0)
		wp->w_width = 0;
	}
	എങ്കിൽ (cl->clip_ഇടത്_content > 0)
	{
	    wp->w_leftcol += cl->clip_ഇടത്_content;
	    wp->w_wincol += cl->clip_ഇടത്_content;
	    wp->w_width -= cl->clip_ഇടത്_content;
	    എങ്കിൽ (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    സ്ഥിതം ശൂന്യം
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    പൂർണ്ണസംഖ്യ iw, ih;

    // Visible cell width and height of popup itself (not image!)
    പൂർണ്ണസംഖ്യ	visible_width;
    പൂർണ്ണസംഖ്യ	visible_height;

    // Crop dimensions of image (in pixels).
    പൂർണ്ണസംഖ്യ	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    എങ്കിൽ (wp->w_popup_imagep == ശൂന്യ || wp->w_popup_flags & POPF_HIDDEN)
	തിരികെ;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    എങ്കിൽ (wp->w_popup_imagep->img == ശൂന്യ)
	// Just set the bounding box
	പോകുക exit;

    visible_width =
	wp->w_width - clip.clip_ഇടത്_content - clip.clip_വലത്_content;
    visible_height =
	wp->w_height - clip.clip_മുകൾ_content - clip.clip_bot_content;

    എങ്കിൽ (visible_width < 0 || visible_height < 0)
	തിരികെ;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ഇടത്_content;
    crop_y = clip.clip_മുകൾ_content;
    crop_width = iw - crop_x - clip.clip_വലത്_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    എങ്കിൽ (crop_width > visible_width)
	crop_width = visible_width;
    എങ്കിൽ (crop_height > visible_height)
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
    സ്ഥിതം ശൂന്യം
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		wrapped = 0;
    പൂർണ്ണസംഖ്യ		maxwidth;
    പൂർണ്ണസംഖ്യ		maxwidth_no_scrollbar;
    പൂർണ്ണസംഖ്യ		width_with_scrollbar = 0;
    പൂർണ്ണസംഖ്യ		used_maxwidth = FALSE;
    പൂർണ്ണസംഖ്യ		margin_width = 0;
    പൂർണ്ണസംഖ്യ		maxspace;
    പൂർണ്ണസംഖ്യ		center_vert = FALSE;
    പൂർണ്ണസംഖ്യ		center_hor = FALSE;
    പൂർണ്ണസംഖ്യ		allow_adjust_ഇടത് = !wp->w_popup_fixed;
    പൂർണ്ണസംഖ്യ		മുകൾ_extra = popup_മുകൾ_extra(wp);
    പൂർണ്ണസംഖ്യ		വലത്_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    പൂർണ്ണസംഖ്യ		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    പൂർണ്ണസംഖ്യ		ഇടത്_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    പൂർണ്ണസംഖ്യ		extra_height = മുകൾ_extra + bot_extra;
    പൂർണ്ണസംഖ്യ		extra_width = ഇടത്_extra + വലത്_extra;
    പൂർണ്ണസംഖ്യ		w_height_before_limit;
    popup_layout_T org_layout;
    പൂർണ്ണസംഖ്യ		minwidth, minheight;
    പൂർണ്ണസംഖ്യ		maxheight = Rows;
    പൂർണ്ണസംഖ്യ		wantline = wp->w_wantline;  // adjusted for textprop
    പൂർണ്ണസംഖ്യ		wantcol = wp->w_wantcol;    // adjusted for textprop
    പൂർണ്ണസംഖ്യ		use_wantcol = wantcol != 0;
    പൂർണ്ണസംഖ്യ		adjust_height_for_മുകൾ_aligned = FALSE;

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
    എങ്കിൽ (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    എങ്കിൽ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	പൂർണ്ണസംഖ്യ	    screen_row;
	പൂർണ്ണസംഖ്യ	    screen_scol;
	പൂർണ്ണസംഖ്യ	    screen_ccol;
	പൂർണ്ണസംഖ്യ	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_മുകൾ = false;
	എങ്കിൽ (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    എങ്കിൽ (popup_find_prop_above_മുകൾ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_മുകൾ = true;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    എങ്കിൽ (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		തിരികെ;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	എങ്കിൽ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	എങ്കിൽ (prop_above_മുകൾ)
	    popup_screenpos_above_മുകൾ(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	അല്ലെങ്കിൽ
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	എങ്കിൽ (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    തിരികെ;
	}
	എങ്കിൽ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	അല്ലെങ്കിൽ
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	എങ്കിൽ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	അല്ലെങ്കിൽ
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// If no line was specified default to vertical centering.
	എങ്കിൽ (wantline == 0)
	    center_vert = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	എങ്കിൽ (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    എങ്കിൽ (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    എങ്കിൽ (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	എങ്കിൽ (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_വരി_എണ്ണം - 1, 0);
	}

	എങ്കിൽ (!use_wantcol)
	    center_hor = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    എങ്കിൽ (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- ഇടത്_extra - വലത്_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- ഇടത്_extra - വലത്_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    എങ്കിൽ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - ഇടത്_extra - വലത്_extra;
    maxwidth = maxspace;
    എങ്കിൽ (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ഇടത് = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    എങ്കിൽ (wp->w_p_nu || wp->w_p_rnu)
	margin_width = സംഖ്യ_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    എങ്കിൽ (signcolumn_on(wp))
	margin_width += 2;
#endif
    എങ്കിൽ (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    എങ്കിൽ (wp->w_buffer->b_term != ശൂന്യ)
    {
	എങ്കിൽ (minwidth == 0)
	    minwidth = 20;
	എങ്കിൽ (minheight == 0)
	    minheight = 5;
    }
#endif

    എങ്കിൽ (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    എങ്കിൽ (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	എങ്കിൽ (wp->w_firstline > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    wp->w_firstline = 0;
	അല്ലെങ്കിൽ
	    wp->w_topline = wp->w_firstline;
    }
    എങ്കിൽ (wp->w_topline < 1)
	wp->w_topline = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_topline > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	wp->w_topline = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;

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
	പൂർണ്ണസംഖ്യ saved_w_width = wp->w_width;

	എങ്കിൽ (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	വേണ്ടി (ln = 1; ln <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം; ++ln)
	{
	    പൂർണ്ണസംഖ്യ len = linetabsize(wp, ln) + margin_width;

	    എങ്കിൽ (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    എങ്കിൽ (saved_w_width < len)
		saved_w_width = len;
	    എങ്കിൽ (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		നിർത്തുക;
	}
	wp->w_width = saved_w_width;
    }
    എങ്കിൽ (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
    അല്ലെങ്കിൽ
	lnum = wp->w_topline;
    ആകുമ്പോൾ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
    {
	പൂർണ്ണസംഖ്യ len;
	പൂർണ്ണസംഖ്യ w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	എങ്കിൽ (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	എങ്കിൽ (len + margin_width > maxwidth
		&& allow_adjust_ഇടത്
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    പൂർണ്ണസംഖ്യ shift_by = len + margin_width - maxwidth;

	    എങ്കിൽ (shift_by > wp->w_wincol)
	    {
		പൂർണ്ണസംഖ്യ truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    എങ്കിൽ (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    എങ്കിൽ (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	എങ്കിൽ (wp->w_p_wrap)
	{
	    ആകുമ്പോൾ (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	എങ്കിൽ (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    എങ്കിൽ (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	എങ്കിൽ (wp->w_firstline < 0)
	    --lnum;
	അല്ലെങ്കിൽ
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	എങ്കിൽ (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_വരി_എണ്ണം - lnum)
		       + wrapped >= maxheight)
	    നിർത്തുക;
    }

    എങ്കിൽ (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം);
#ifdef FEAT_TERMINAL
    എങ്കിൽ (wp->w_buffer->b_term != ശൂന്യ && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    എങ്കിൽ (wp->w_has_scrollbar)
    {
	++വലത്_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	എങ്കിൽ (maxwidth + വലത്_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    എങ്കിൽ (wp->w_popup_title != ശൂന്യ && *wp->w_popup_title != NUL)
    {
	പൂർണ്ണസംഖ്യ title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	എങ്കിൽ (minwidth < title_len)
	    minwidth = title_len;
    }

    എങ്കിൽ (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    എങ്കിൽ (wp->w_width > maxwidth)
    {
	എങ്കിൽ (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	എങ്കിൽ (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    പൂർണ്ണസംഖ്യ off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, വലത്_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	അല്ലെങ്കിൽ
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - വലത്_extra;
	}
    }
    എങ്കിൽ (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	എങ്കിൽ (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	പൂർണ്ണസംഖ്യ leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	എങ്കിൽ (leftoff >= 0)
	    wp->w_wincol = leftoff;
	അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    എങ്കിൽ (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    എങ്കിൽ (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	പൂർണ്ണസംഖ്യ want_col = 0;

	// try to show the right border and any scrollbar
	want_col = ഇടത്_extra + wp->w_width + വലത്_extra;
	എങ്കിൽ (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    എങ്കിൽ (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_വരി_എണ്ണം - wp->w_topline
								 + 1 + wrapped;
    എങ്കിൽ (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    എങ്കിൽ (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    എങ്കിൽ (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    എങ്കിൽ (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	എങ്കിൽ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	എങ്കിൽ ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	അല്ലെങ്കിൽ എങ്കിൽ (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	അല്ലെങ്കിൽ
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_മുകൾ_aligned = TRUE;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	പൂർണ്ണസംഖ്യ check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	എങ്കിൽ (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    എങ്കിൽ (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    എങ്കിൽ (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_മുകൾ_aligned = TRUE;
	}
    }

    എങ്കിൽ (adjust_height_for_മുകൾ_aligned && wp->w_want_scrollbar
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
	എങ്കിൽ (wp->w_buffer->b_term == ശൂന്യ || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    എങ്കിൽ (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		എങ്കിൽ (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		അല്ലെങ്കിൽ
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    എങ്കിൽ (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    എങ്കിൽ (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    എങ്കിൽ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    എങ്കിൽ (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    എങ്കിൽ (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	പൂർണ്ണസംഖ്യ avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    എങ്കിൽ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	എങ്കിൽ (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	എങ്കിൽ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	എങ്കിൽ (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    പൂർണ്ണസംഖ്യ avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    എങ്കിൽ (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    എങ്കിൽ (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	എങ്കിൽ (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	തിരികെ;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    എങ്കിൽ (win_valid(wp->w_popup_prop_win))
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
    എങ്കിൽ (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	എങ്കിൽ (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

തരനിർവചനം എണ്ണൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_is_notification(create_type_T type)
{
    തിരികെ type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    സ്ഥിതം ശൂന്യം
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    പൂർണ്ണസംഖ്യ	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    വേണ്ടി (lnum = buf->b_ml.ml_വരി_എണ്ണം; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    എങ്കിൽ (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ശൂന്യ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    അല്ലെങ്കിൽ
    {
	list_T *l = text.vval.v_list;

	എങ്കിൽ (l != ശൂന്യ && l->lv_len > 0)
	{
	    എങ്കിൽ (l->lv_first == &range_list_item)
		emsg(_(e_using_സംഖ്യ_as_string));
	    അല്ലെങ്കിൽ എങ്കിൽ (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    അല്ലെങ്കിൽ
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_വരി_എണ്ണം);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    ചെയ്യുക {						    \
	എങ്കിൽ (wp != ശൂന്യ)					    \
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
    } ആകുമ്പോൾ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_popup_option(win_T *wp, പൂർണ്ണസംഖ്യ is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    പൂർണ്ണസംഖ്യ	    border_enabled = FALSE;

    എങ്കിൽ (wp != ശൂന്യ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    വേണ്ടി ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	പൂർണ്ണസംഖ്യ	x;

	e = vim_strchr(p, ':');
	എങ്കിൽ (e == ശൂന്യ || e[1] == NUL)
	    തിരികെ FAIL;

	p = vim_strchr(e, ',');
	എങ്കിൽ (p == ശൂന്യ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	എങ്കിൽ (STRNCMP(s, "height:", 7) == 0)
	{
	    എങ്കിൽ (dig != p)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		എങ്കിൽ (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "width:", 6) == 0)
	{
	    എങ്കിൽ (dig != p)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		എങ്കിൽ (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "highlight:", 10) == 0)
	{
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		അക്ഷരം *errmsg;
		പൂർണ്ണസംഖ്യ c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		എങ്കിൽ (errmsg == ശൂന്യ)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		അല്ലെങ്കിൽ
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    എങ്കിൽ (*arg == NUL || *arg == ',')
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    പൂർണ്ണസംഖ്യ		i;
	    പൂർണ്ണസംഖ്യ		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    പൂർണ്ണസംഖ്യ		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    എങ്കിൽ (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		എങ്കിൽ (wp != ശൂന്യ)
		{
		    വേണ്ടി (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		തുടരുക;
	    }

	    token = vim_strnsave(arg, token_len);
	    എങ്കിൽ (token == ശൂന്യ)
		തിരികെ FAIL;

	    എങ്കിൽ ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		എങ്കിൽ (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    പൂർണ്ണസംഖ്യ		out[8];
		    പൂർണ്ണസംഖ്യ		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    വേണ്ടി (i = 0; i < 8 && !failed; i++)
		    {
			എങ്കിൽ (*q == NUL)
			    failed = TRUE;
			അല്ലെങ്കിൽ
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    എങ്കിൽ (i < 7)
			    {
				എങ്കിൽ (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    എങ്കിൽ (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			തിരികെ FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		vim_free(token);
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		വേണ്ടി (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    പൂർണ്ണസംഖ്യ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    പൂർണ്ണസംഖ്യ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    എങ്കിൽ (!on && !off)
		തിരികെ FAIL;
	    on = on && mouse_has(MOUSE_തിരുകുക) && (border_enabled || is_preview);
	    എങ്കിൽ (wp != ശൂന്യ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    പൂർണ്ണസംഖ്യ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    പൂർണ്ണസംഖ്യ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    എങ്കിൽ (!on && !off)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		എങ്കിൽ (on && mouse_has(MOUSE_തിരുകുക))
		    wp->w_popup_flags |= POPF_RESIZE;
		അല്ലെങ്കിൽ
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    പൂർണ്ണസംഖ്യ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    പൂർണ്ണസംഖ്യ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    എങ്കിൽ (!on && !off)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    പൂർണ്ണസംഖ്യ		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    പൂർണ്ണസംഖ്യ		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    എങ്കിൽ (!menu && !item)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(s, "opacity:", 8) == 0)
	{
	    എങ്കിൽ (dig != p || x < 0 || x > 100)
		തിരികെ FAIL;
	    എങ്കിൽ (wp != ശൂന്യ)
	    {
		എങ്കിൽ (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		അല്ലെങ്കിൽ
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    പൂർണ്ണസംഖ്യ
parse_previewpopup(win_T *wp)
{
    തിരികെ parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    പൂർണ്ണസംഖ്യ
parse_completepopup(win_T *wp)
{
    തിരികെ parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    ശൂന്യം
popup_set_wantpos_cursor(win_T *wp, പൂർണ്ണസംഖ്യ width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    എങ്കിൽ (d != ശൂന്യ)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    എങ്കിൽ (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    അല്ലെങ്കിൽ
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	എങ്കിൽ (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    എങ്കിൽ (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	എങ്കിൽ (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    ശൂന്യം
popup_set_wantpos_rowcol(win_T *wp, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    സ്ഥിതം ശൂന്യം
add_border_ഇടത്_വലത്_padding(win_T *wp)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_terminal_exists(ശൂന്യം)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp->w_buffer->b_term != ശൂന്യ)
	    തിരികെ TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    എങ്കിൽ (wp->w_buffer->b_term != ശൂന്യ)
		തിരികെ TRUE;
    തിരികെ FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    ശൂന്യം
popup_redraw_all(ശൂന്യം)
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
    പൂർണ്ണസംഖ്യ
popup_need_redraw(ശൂന്യം)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    തിരികെ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Set the color for a notification window.
 */
    സ്ഥിതം ശൂന്യം
popup_update_color(win_T *wp, create_type_T type)
{
    അക്ഷരം    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    അക്ഷരം    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    എങ്കിൽ (errmsg == ശൂന്യ)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    അല്ലെങ്കിൽ
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    സ്ഥിതം win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		tabnr = 0;
    പൂർണ്ണസംഖ്യ		new_buffer;
    buf_T	*buf = ശൂന്യ;
    dict_T	*d = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (check_secure())
	തിരികെ ശൂന്യ;

    എങ്കിൽ (argvars != ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script()
		&& (check_for_string_or_സംഖ്യ_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    തിരികെ ശൂന്യ;

	// Check that arguments look OK.
	എങ്കിൽ (argvars[0].v_type == VAR_സംഖ്യ)
	{
	    buf = buflist_findnr(argvars[0].vval.v_സംഖ്യ);
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_സംഖ്യ);
		തിരികെ ശൂന്യ;
	    }
#ifdef FEAT_TERMINAL
	    എങ്കിൽ (buf->b_term != ശൂന്യ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		തിരികെ ശൂന്യ;
	    }
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ശൂന്യ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ശൂന്യ))
	{
	    emsg(_(e_buffer_സംഖ്യ_text_or_list_required));
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    തിരികെ ശൂന്യ;
	d = argvars[1].vval.v_dict;
    }

    എങ്കിൽ (d != ശൂന്യ)
    {
	എങ്കിൽ (dict_has_താക്കോൽ(d, "tabpage"))
	    tabnr = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "tabpage");
	അല്ലെങ്കിൽ എങ്കിൽ (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	അല്ലെങ്കിൽ
	    tabnr = 0;
	എങ്കിൽ (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    എങ്കിൽ (tp == ശൂന്യ)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		തിരികെ ശൂന്യ;
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    എങ്കിൽ (buf != ശൂന്യ && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	തിരികെ ശൂന്യ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (rettv != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    എങ്കിൽ (buf != ശൂന്യ)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    അല്ലെങ്കിൽ
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(ശൂന്യ, ശൂന്യ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    win_free_popup(wp);
	    തിരികെ ശൂന്യ;
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

    എങ്കിൽ (tp != ശൂന്യ)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    അല്ലെങ്കിൽ // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	എങ്കിൽ (first_popupwin == ശൂന്യ)
	    first_popupwin = wp;
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (prev->w_next != ശൂന്യ)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    എങ്കിൽ (new_buffer && argvars != ശൂന്യ)
	popup_set_buffer_text(buf, argvars[0]);

    എങ്കിൽ (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    എങ്കിൽ (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_മൂല്യങ്ങൾ(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    എങ്കിൽ (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	എങ്കിൽ (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_മൂല്യങ്ങൾ(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    എങ്കിൽ (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	പൂർണ്ണസംഖ്യ	height = buf->b_ml.ml_വരി_എണ്ണം + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	വേണ്ടി (twp = first_popupwin; twp != ശൂന്യ; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    എങ്കിൽ (twp != wp
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
	എങ്കിൽ (wp->w_wantline + height > Rows)
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
	വേണ്ടി (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    എങ്കിൽ (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_ഇടത്_വലത്_padding(wp);
    }

    എങ്കിൽ (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	എങ്കിൽ (callback.cb_name != ശൂന്യ)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    വേണ്ടി (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    വേണ്ടി (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    എങ്കിൽ (type == TYPE_PREVIEW)
    {
	എങ്കിൽ (mouse_has(MOUSE_തിരുകുക))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	വേണ്ടി (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    എങ്കിൽ (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	എങ്കിൽ (mouse_has(MOUSE_തിരുകുക))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_ഇടത്_വലത്_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    എങ്കിൽ (d != ശൂന്യ)
    {
	// Deal with options.
	എങ്കിൽ (apply_options(wp, d, TRUE) == FAIL)
	{
	    (ശൂന്യം)popup_close(wp->w_id, FALSE);
	    തിരികെ ശൂന്യ;
	}
    }

#ifdef FEAT_TIMERS
    എങ്കിൽ (popup_is_notification(type) && wp->w_popup_timer == ശൂന്യ)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    എങ്കിൽ (buf->b_term != ശൂന്യ)
	win_enter(wp, FALSE);
#endif

    തിരികെ wp;
}

/*
 * popup_clear()
 */
    ശൂന്യം
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ force = FALSE;

    എങ്കിൽ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
	force = (പൂർണ്ണസംഖ്യ)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    ശൂന്യം
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    ശൂന്യം
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    ശൂന്യം
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    സ്ഥിതം ശൂന്യം
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_സംഖ്യ;
    argv[0].vval.v_സംഖ്യ = (varnumber_T)wp->w_id;

    എങ്കിൽ (result != ശൂന്യ && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    അല്ലെങ്കിൽ
    {
	argv[1].v_type = VAR_സംഖ്യ;
	argv[1].vval.v_സംഖ്യ = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    എങ്കിൽ (result != ശൂന്യ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    സ്ഥിതം ശൂന്യം
back_to_prevwin(win_T *wp)
{
    എങ്കിൽ (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    അല്ലെങ്കിൽ
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    സ്ഥിതം ശൂന്യം
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    പൂർണ്ണസംഖ്യ id;

    എങ്കിൽ (!win_valid(wp))
       തിരികെ;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    എങ്കിൽ (wp == curwin && curbuf->b_term != ശൂന്യ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    എങ്കിൽ (owp != curwin && owp->w_buffer->b_term != ശൂന്യ)
		നിർത്തുക;
	എങ്കിൽ (owp != ശൂന്യ)
	    win_enter(owp, FALSE);
	അല്ലെങ്കിൽ
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		എങ്കിൽ (owp != curwin && owp->w_buffer->b_term != ശൂന്യ)
		    നിർത്തുക;
	    എങ്കിൽ (owp != ശൂന്യ)
		win_enter(owp, FALSE);
	    അല്ലെങ്കിൽ
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    എങ്കിൽ (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	തിരികെ;
    }

    CHECK_CURBUF;
    എങ്കിൽ (wp->w_close_cb.cb_name != ശൂന്യ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    ശൂന്യം
popup_close_with_retval(win_T *wp, പൂർണ്ണസംഖ്യ retval)
{
    typval_T res;

    res.v_type = VAR_സംഖ്യ;
    res.vval.v_സംഖ്യ = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    ശൂന്യം
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    സ്ഥിതം ശൂന്യം
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    എങ്കിൽ (wp != mouse_wp
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
    ശൂന്യം
popup_handle_mouse_moved(ശൂന്യം)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    പൂർണ്ണസംഖ്യ	    row = mouse_row;
    പൂർണ്ണസംഖ്യ	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    വേണ്ടി (wp = first_popupwin; wp != ശൂന്യ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    വേണ്ടി (wp = curtab->tp_first_popupwin; wp != ശൂന്യ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    സ്ഥിതം ശൂന്യം
filter_handle_drag(win_T *wp, പൂർണ്ണസംഖ്യ c, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ	row = mouse_row;
    പൂർണ്ണസംഖ്യ	col = mouse_col;

    എങ്കിൽ ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_താക്കോൽ(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_സംഖ്യ = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    ശൂന്യം
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    char_u	*താക്കോൽ;
    typval_T	res;
    പൂർണ്ണസംഖ്യ		c;
    linenr_T	old_lnum;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    id = tv_get_സംഖ്യ(&argvars[0]);
    wp = win_id2wp(id);
    താക്കോൽ = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    c = *താക്കോൽ;
    എങ്കിൽ (c == K_SPECIAL && താക്കോൽ[1] != NUL)
	c = TO_SPECIAL(താക്കോൽ[1], താക്കോൽ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
    res.v_type = VAR_സംഖ്യ;

    old_lnum = wp->w_cursor.lnum;
    എങ്കിൽ (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	എങ്കിൽ (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	അല്ലെങ്കിൽ
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
    }
    എങ്കിൽ (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	എങ്കിൽ (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    ++wp->w_cursor.lnum;
	അല്ലെങ്കിൽ
	    wp->w_cursor.lnum = 1;
    }
    എങ്കിൽ (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	തിരികെ;
    }

    എങ്കിൽ (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_സംഖ്യ = -1;
	popup_close_and_callback(wp, &res);
	തിരികെ;
    }
    എങ്കിൽ (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_സംഖ്യ = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	തിരികെ;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    ശൂന്യം
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    char_u	*താക്കോൽ;
    typval_T	res;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    id = tv_get_സംഖ്യ(&argvars[0]);
    wp = win_id2wp(id);
    താക്കോൽ = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    c = *താക്കോൽ;
    എങ്കിൽ (c == CAR && need_wait_return)
	തിരികെ;
    എങ്കിൽ (c == K_SPECIAL && താക്കോൽ[1] != NUL)
	c = TO_SPECIAL(താക്കോൽ[1], താക്കോൽ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = VVAL_TRUE;

    എങ്കിൽ (c == 'y' || c == 'Y')
	res.vval.v_സംഖ്യ = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_സംഖ്യ = 0;
    അല്ലെങ്കിൽ
    {
	filter_handle_drag(wp, c, rettv);
	തിരികെ;
    }

    // Invoke callback
    res.v_type = VAR_സംഖ്യ;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    ശൂന്യം
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    ശൂന്യം
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    ശൂന്യം
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    സ്ഥിതം win_T *
find_popup_win(പൂർണ്ണസംഖ്യ id)
{
    win_T *wp = win_id2wp(id);

    എങ്കിൽ (wp != ശൂന്യ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	തിരികെ ശൂന്യ;
    }
    തിരികെ wp;
}

/*
 * popup_close({id})
 */
    ശൂന്യം
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    എങ്കിൽ (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == ശൂന്യ &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    wp = find_popup_win(id);
    എങ്കിൽ (wp != ശൂന്യ)
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
    സ്ഥിതം ശൂന്യം
popup_clear_mask_for(win_T *wp)
{
    പൂർണ്ണസംഖ്യ r, c;
    പൂർണ്ണസംഖ്യ row_start, col_start, row_end, col_end;

    എങ്കിൽ (popup_mask == ശൂന്യ || !popup_visible)
	തിരികെ;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (പൂർണ്ണസംഖ്യ)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (പൂർണ്ണസംഖ്യ)screen_Columns);

    വേണ്ടി (r = row_start; r < row_end; ++r)
	വേണ്ടി (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    ശൂന്യം
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    പൂർണ്ണസംഖ്യ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    എങ്കിൽ (error_if_term_popup_window())
	തിരികെ;
#endif
    എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	തിരികെ;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    എങ്കിൽ (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    എങ്കിൽ (was_visible)
	popup_clear_mask_for(wp);

    എങ്കിൽ (old_area.active)
	popup_redraw_exposed_area(&old_area);
    അല്ലെങ്കിൽ
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    എങ്കിൽ (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    ശൂന്യം
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    ശൂന്യം
popup_show(win_T *wp)
{
    bool popup_active;

    എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	തിരികെ;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    എങ്കിൽ (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_വരികൾ_valid = 0;
	എങ്കിൽ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    അല്ലെങ്കിൽ
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    ശൂന്യം
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;

    rettv->v_type = VAR_സംഖ്യ;
    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    എങ്കിൽ (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_സംഖ്യ = 0;
}

/*
 * popup_settext({id}, {text})
 */
    ശൂന്യം
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    popup_area_T	old_area;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    popup_save_area(wp, &old_area);

    എങ്കിൽ (check_for_string_or_list_arg(argvars, 1) == FAIL)
	തിരികെ;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_വരികൾ_valid = 0;
    എങ്കിൽ (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    എങ്കിൽ (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    ശൂന്യം
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = VVAL_FALSE;

    എങ്കിൽ (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    buf = tv_get_buf_from_arg(&argvars[1]);

    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;
#ifdef FEAT_TERMINAL
    എങ്കിൽ (buf->b_term != ശൂന്യ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	തിരികെ;
    }
#endif

    എങ്കിൽ (wp->w_buffer != buf)
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

	എങ്കിൽ (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
}

    സ്ഥിതം ശൂന്യം
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    പൂർണ്ണസംഖ്യ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    എങ്കിൽ (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    എങ്കിൽ (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    എങ്കിൽ (wp == message_win)
	message_win = ശൂന്യ;
#endif

    എങ്കിൽ (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    എങ്കിൽ (!old_area.active)
	popup_mask_refresh = TRUE;
}

    സ്ഥിതം ശൂന്യം
error_for_popup_window(ശൂന്യം)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    പൂർണ്ണസംഖ്യ
error_if_popup_window(പൂർണ്ണസംഖ്യ also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    എങ്കിൽ (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == ശൂന്യ)
#endif
	    )
    {
	error_for_popup_window();
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
popup_close(പൂർണ്ണസംഖ്യ id, പൂർണ്ണസംഖ്യ force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ശൂന്യ;

    // go through global popups
    വേണ്ടി (wp = first_popupwin; wp != ശൂന്യ; prev = wp, wp = wp->w_next)
	എങ്കിൽ (wp->w_id == id)
	{
	    എങ്കിൽ (wp == curwin)
	    {
		എങ്കിൽ (!force)
		{
		    error_for_popup_window();
		    തിരികെ FAIL;
		}
		back_to_prevwin(wp);
	    }
	    എങ്കിൽ (prev == ശൂന്യ)
		first_popupwin = wp->w_next;
	    അല്ലെങ്കിൽ
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    തിരികെ OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	എങ്കിൽ (popup_close_tabpage(tp, id, force) == OK)
	    തിരികെ OK;
    തിരികെ FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    പൂർണ്ണസംഖ്യ
popup_close_tabpage(tabpage_T *tp, പൂർണ്ണസംഖ്യ id, പൂർണ്ണസംഖ്യ force)
{
    win_T	*wp;
    win_T	**വേര് = &tp->tp_first_popupwin;
    win_T	*prev = ശൂന്യ;

    വേണ്ടി (wp = *വേര്; wp != ശൂന്യ; prev = wp, wp = wp->w_next)
	എങ്കിൽ (wp->w_id == id)
	{
	    എങ്കിൽ (wp == curwin)
	    {
		എങ്കിൽ (!force)
		{
		    error_for_popup_window();
		    തിരികെ FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    എങ്കിൽ (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    എങ്കിൽ (prev == ശൂന്യ)
		*വേര് = wp->w_next;
	    അല്ലെങ്കിൽ
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    തിരികെ OK;
	}
    തിരികെ FAIL;
}

    ശൂന്യം
close_all_popups(പൂർണ്ണസംഖ്യ force)
{
    എങ്കിൽ (!force && ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;
    ആകുമ്പോൾ (first_popupwin != ശൂന്യ)
	എങ്കിൽ (popup_close(first_popupwin->w_id, force) == FAIL)
	    തിരികെ;
    ആകുമ്പോൾ (curtab->tp_first_popupwin != ശൂന്യ)
	എങ്കിൽ (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    തിരികെ;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    പൂർണ്ണസംഖ്യ i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    വേണ്ടി (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    സ്ഥിതം bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	തിരികെ true;
    വേണ്ടി (i = 0; i < 4; i++)
	എങ്കിൽ (style->border_highlight[i] != wp->w_border_highlight[i])
	    തിരികെ true;
    തിരികെ false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    തിരികെ layout->winrow != wp->w_winrow
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
    സ്ഥിതം bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    തിരികെ area->winrow != wp->w_winrow
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
    സ്ഥിതം ശൂന്യം
mark_overlapped_opacity_popup(win_T *wp, പൂർണ്ണസംഖ്യ area_മുകൾ, പൂർണ്ണസംഖ്യ area_bot,
	പൂർണ്ണസംഖ്യ area_ഇടത്, പൂർണ്ണസംഖ്യ area_വലത്, പൂർണ്ണസംഖ്യ zindex)
{
    എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_മുകൾ
	    || wp->w_wincol >= area_വലത്
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_ഇടത്)
	തിരികെ;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_വരികൾ_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    സ്ഥിതം ശൂന്യം
redraw_overlapped_opacity_popups(പൂർണ്ണസംഖ്യ winrow, പൂർണ്ണസംഖ്യ wincol, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ width,
	പൂർണ്ണസംഖ്യ leftoff, പൂർണ്ണസംഖ്യ zindex)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		area_മുകൾ = winrow;
    പൂർണ്ണസംഖ്യ		area_bot = winrow + height;
    പൂർണ്ണസംഖ്യ		area_ഇടത് = wincol;
    പൂർണ്ണസംഖ്യ		area_വലത് = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_മുകൾ, area_bot, area_ഇടത്,
		area_വലത്, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_മുകൾ, area_bot, area_ഇടത്,
		area_വലത്, zindex);

    എങ്കിൽ (must_redraw < UPD_VALID)
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
    സ്ഥിതം ശൂന്യം
refill_pum_bg_row_from_buffer(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ ഇടത്, പൂർണ്ണസംഖ്യ വലത്)
{
    പൂർണ്ണസംഖ്യ		വരി_cp = r;
    പൂർണ്ണസംഖ്യ		col_cp = ഇടത്;
    win_T	*wp;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		row_for_lnum;
    പൂർണ്ണസംഖ്യ		col_for_lnum = 0;
    char_u	*വരി;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		win_text_col;
    പൂർണ്ണസംഖ്യ		screen_col;
    പൂർണ്ണസംഖ്യ		soff_base = (r - pum_bg_മുകൾ) * pum_bg_cols;
    പൂർണ്ണസംഖ്യ		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    വേണ്ടി (c = ഇടത്; c < വലത്; ++c)
    {
	pum_bg_വരികൾ[soff_base + c] = ' ';
	എങ്കിൽ (pum_bg_attrs != ശൂന്യ)
	    pum_bg_attrs[soff_base + c] = 0;
	എങ്കിൽ (enc_utf8 && pum_bg_വരികൾ_UC != ശൂന്യ)
	    pum_bg_വരികൾ_UC[soff_base + c] = 0;
	എങ്കിൽ (enc_utf8)
	{
	    പൂർണ്ണസംഖ്യ k;
	    വേണ്ടി (k = 0; k < MAX_MCO; ++k)
		എങ്കിൽ (pum_bg_വരികൾ_C[k] != ശൂന്യ)
		    pum_bg_വരികൾ_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&വരി_cp, &col_cp, IGNORE_POPUP);
    എങ്കിൽ (wp == ശൂന്യ || വരി_cp < 0 || വരി_cp >= wp->w_height)
	തിരികെ;
    എങ്കിൽ (wp->w_buffer == ശൂന്യ || wp->w_buffer->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ;

    // Compute the buffer line for this screen row.
    row_for_lnum = വരി_cp;
    എങ്കിൽ (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, ശൂന്യ))
	തിരികെ;	// past end of buffer
    എങ്കിൽ (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	തിരികെ;

    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
    എങ്കിൽ (വരി == ശൂന്യ)
	തിരികെ;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    എങ്കിൽ (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = വരി;
    ആകുമ്പോൾ (*p != NUL && screen_col < വലത്)
    {
	പൂർണ്ണസംഖ്യ char_cells;
	പൂർണ്ണസംഖ്യ byte_എണ്ണം;
	പൂർണ്ണസംഖ്യ soff = soff_base + screen_col;
	പൂർണ്ണസംഖ്യ in_range = (screen_col >= ഇടത് && screen_col < വലത്);

	എങ്കിൽ (*p == '\t')
	{
	    പൂർണ്ണസംഖ്യ ts = (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_എണ്ണം = 1;
	    എങ്കിൽ (in_range)
	    {
		pum_bg_വരികൾ[soff] = ' ';
		എങ്കിൽ (enc_utf8 && pum_bg_വരികൾ_UC != ശൂന്യ)
		    pum_bg_വരികൾ_UC[soff] = 0;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_എണ്ണം = mb_ptr2len(p);
	    എങ്കിൽ (in_range)
	    {
		pum_bg_വരികൾ[soff] = *p;
		എങ്കിൽ (enc_utf8 && pum_bg_വരികൾ_UC != ശൂന്യ)
		    pum_bg_വരികൾ_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    char_cells = 1;
	    byte_എണ്ണം = 1;
	    എങ്കിൽ (in_range)
	    {
		pum_bg_വരികൾ[soff] = (*p < 0x20) ? ' ' : *p;
		എങ്കിൽ (enc_utf8 && pum_bg_വരികൾ_UC != ശൂന്യ)
		    pum_bg_വരികൾ_UC[soff] = 0;
	    }
	}
	എങ്കിൽ (in_range && pum_bg_attrs != ശൂന്യ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_എണ്ണം;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    സ്ഥിതം ശൂന്യം
popup_redraw_exposed_area(popup_area_T *area)
{
    എങ്കിൽ (!area->active)
	തിരികെ;

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
    എങ്കിൽ (pum_bg_വരികൾ != ശൂന്യ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_മുകൾ)
    {
	പൂർണ്ണസംഖ്യ മുകൾ = MAX(area->winrow, pum_bg_മുകൾ);
	പൂർണ്ണസംഖ്യ bot = MIN(area->winrow + area->height, pum_bg_bot);
	പൂർണ്ണസംഖ്യ ഇടത് = MAX(area->wincol, 0);
	പൂർണ്ണസംഖ്യ വലത് = MIN(area->wincol + area->width, pum_bg_cols);
	പൂർണ്ണസംഖ്യ r;

	വേണ്ടി (r = മുകൾ; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, ഇടത്, വലത്);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    സ്ഥിതം ശൂന്യം
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->വരികൾ);
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
    സ്ഥിതം ശൂന്യം
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    എങ്കിൽ (screen_opacity_popup == ശൂന്യ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	തിരികെ;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    എങ്കിൽ (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->വരികൾ = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ,
				  saved_screen->rows * saved_screen->cols);
    എങ്കിൽ (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    എങ്കിൽ (saved_screen->വരികൾ == ശൂന്യ || saved_screen->attrs == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < saved_screen->rows; r++)
    {
	പൂർണ്ണസംഖ്യ screen_row = saved_screen->start_row + r;

	എങ്കിൽ (screen_row >= 0 && screen_row < screen_Rows)
	    വേണ്ടി (പൂർണ്ണസംഖ്യ c = 0; c < saved_screen->cols; c++)
	    {
		പൂർണ്ണസംഖ്യ screen_col = saved_screen->start_col + c;

		എങ്കിൽ (screen_col >= 0 && screen_col < screen_Columns)
		{
		    പൂർണ്ണസംഖ്യ off = വരി_Offset[screen_row] + screen_col;
		    പൂർണ്ണസംഖ്യ save_off = r * saved_screen->cols + c;

		    saved_screen->വരികൾ[save_off] = Screen_വരികൾ[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    എങ്കിൽ (enc_utf8 && saved_screen->linesuc != ശൂന്യ)
			saved_screen->linesuc[save_off] = Screen_വരികൾ_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    സ്ഥിതം ശൂന്യം
redraw_under_popup_area(പൂർണ്ണസംഖ്യ winrow, പൂർണ്ണസംഖ്യ wincol, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ leftoff)
{
    പൂർണ്ണസംഖ്യ	    r;

    വേണ്ടി (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	പൂർണ്ണസംഖ്യ	    c;
	win_T	    *prev_twp = ശൂന്യ;

	എങ്കിൽ (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    തുടരുക;
	}

	വേണ്ടി (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    പൂർണ്ണസംഖ്യ	    വരി_cp = r;
	    പൂർണ്ണസംഖ്യ	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&വരി_cp, &col_cp, IGNORE_POPUP);
	    എങ്കിൽ (twp != ശൂന്യ && twp != prev_twp)
	    {
		prev_twp = twp;
		എങ്കിൽ (വരി_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (ശൂന്യം)mouse_comp_pos(twp, &വരി_cp, &col_cp, &lnum, ശൂന്യ);
		    redrawWinline(twp, lnum);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (വരി_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    ശൂന്യം
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    popup_area_T	old_area;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;  // invalid {id}

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	തിരികെ;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    എങ്കിൽ (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    എങ്കിൽ (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    ശൂന്യം
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    പൂർണ്ണസംഖ്യ		need_redraw = FALSE;
    പൂർണ്ണസംഖ്യ		need_reposition = FALSE;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;  // invalid {id}

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	തിരികെ;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (ശൂന്യം)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    എങ്കിൽ (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    എങ്കിൽ (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    എങ്കിൽ (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    എങ്കിൽ (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_വരികൾ_valid = 0;
	എങ്കിൽ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    എങ്കിൽ (old_style.blend != wp->w_popup_blend)
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

    എങ്കിൽ (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    ശൂന്യം
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		മുകൾ_extra;
    പൂർണ്ണസംഖ്യ		ഇടത്_extra;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;  // invalid {id}
    മുകൾ_extra = popup_മുകൾ_extra(wp);
    ഇടത്_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_സംഖ്യ(dict, "line", wp->w_winrow + 1);
    dict_add_സംഖ്യ(dict, "col", wp->w_wincol + 1);
    dict_add_സംഖ്യ(dict, "width", wp->w_width + ഇടത്_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_സംഖ്യ(dict, "height", wp->w_height + മുകൾ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_സംഖ്യ(dict, "core_line", wp->w_winrow + 1 + മുകൾ_extra);
    dict_add_സംഖ്യ(dict, "core_col", wp->w_wincol + 1 + ഇടത്_extra);
    dict_add_സംഖ്യ(dict, "core_width", wp->w_width);
    dict_add_സംഖ്യ(dict, "core_height", wp->w_height);

    dict_add_സംഖ്യ(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_സംഖ്യ(dict, "firstline", wp->w_topline);
    dict_add_സംഖ്യ(dict, "lastline", wp->w_botline - 1);
    dict_add_സംഖ്യ(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    ശൂന്യം
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    FOR_ALL_POPUPWINS(wp)
	list_append_സംഖ്യ(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_സംഖ്യ(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    ശൂന്യം
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    win_T	*wp;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    row = tv_get_സംഖ്യ(&argvars[0]) - 1;
    col = tv_get_സംഖ്യ(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    എങ്കിൽ (wp != ശൂന്യ && WIN_IS_POPUP(wp))
	rettv->vval.v_സംഖ്യ = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    സ്ഥിതം ശൂന്യം
get_padding_border(dict_T *dict, പൂർണ്ണസംഖ്യ *array, അക്ഷരം *name)
{
    list_T  *list;
    പൂർണ്ണസംഖ്യ	    i;

    എങ്കിൽ (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	തിരികെ;

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	തിരികെ;
    }
    എങ്കിൽ (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	വേണ്ടി (i = 0; i < 4; ++i)
	    list_append_സംഖ്യ(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    സ്ഥിതം ശൂന്യം
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; i < 4; ++i)
	എങ്കിൽ (wp->w_border_highlight[i] != ശൂന്യ)
	    നിർത്തുക;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    എങ്കിൽ (i == 4 && !wp->w_border_highlight_isset)
	തിരികെ;

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	തിരികെ;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    എങ്കിൽ (i == 4)
	തിരികെ;
    വേണ്ടി (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    സ്ഥിതം ശൂന്യം
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    പൂർണ്ണസംഖ്യ	    i;
    char_u  buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ	    len;

    വേണ്ടി (i = 0; i < 8; ++i)
	എങ്കിൽ (wp->w_border_char[i] != 0)
	    നിർത്തുക;
    എങ്കിൽ (i == 8)
	തിരികെ;

    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	തിരികെ;
    }
    വേണ്ടി (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    സ്ഥിതം ശൂന്യം
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    എങ്കിൽ (list != ശൂന്യ)
    {
	എങ്കിൽ (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	അല്ലെങ്കിൽ
	{
	    list_append_സംഖ്യ(list, wp->w_popup_lnum);
	    list_append_സംഖ്യ(list, wp->w_popup_mincol);
	    list_append_സംഖ്യ(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	തിരികെ;
    }
    list_append_സംഖ്യ(list, wp->w_popup_mouse_row);
    list_append_സംഖ്യ(list, wp->w_popup_mouse_mincol);
    list_append_സംഖ്യ(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    ശൂന്യം
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		id;
    win_T	*wp;
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = find_popup_win(id);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ;

    dict = rettv->vval.v_dict;
    dict_add_സംഖ്യ(dict, "line", wp->w_wantline);
    dict_add_സംഖ്യ(dict, "col", wp->w_wantcol);
    dict_add_സംഖ്യ(dict, "minwidth", wp->w_minwidth);
    dict_add_സംഖ്യ(dict, "minheight", wp->w_minheight);
    dict_add_സംഖ്യ(dict, "maxheight", wp->w_maxheight);
    dict_add_സംഖ്യ(dict, "maxwidth", wp->w_maxwidth);
    dict_add_സംഖ്യ(dict, "firstline", wp->w_firstline);
    dict_add_സംഖ്യ(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_സംഖ്യ(dict, "zindex", wp->w_zindex);
    dict_add_സംഖ്യ(dict, "fixed", wp->w_popup_fixed);
    എങ്കിൽ (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	എങ്കിൽ (pt != ശൂന്യ)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_സംഖ്യ(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_സംഖ്യ(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    എങ്കിൽ (wp->w_popup_imagep != ശൂന്യ && wp->w_popup_imagep->img != ശൂന്യ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	എങ്കിൽ (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_സംഖ്യ(dict, "wrap", wp->w_p_wrap);
    dict_add_സംഖ്യ(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_സംഖ്യ(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_സംഖ്യ(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_സംഖ്യ(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_സംഖ്യ(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_സംഖ്യ(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_സംഖ്യ(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_സംഖ്യ(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    എങ്കിൽ (wp->w_scrollbar_highlight != ശൂന്യ)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    എങ്കിൽ (wp->w_thumb_highlight != ശൂന്യ)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    എങ്കിൽ (twp->w_id == id)
		നിർത്തുക;
	എങ്കിൽ (twp != ശൂന്യ)
	    നിർത്തുക;
	++i;
    }
    എങ്കിൽ (tp == ശൂന്യ)
	i = -1;  // must be global
    അല്ലെങ്കിൽ എങ്കിൽ (tp == curtab)
	i = 0;
    dict_add_സംഖ്യ(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    എങ്കിൽ (wp->w_filter_cb.cb_name != ശൂന്യ)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    എങ്കിൽ (wp->w_close_cb.cb_name != ശൂന്യ)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(poppos_entries); ++i)
	എങ്കിൽ (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(പൂർണ്ണസംഖ്യ)poppos_entries[i].pp_name.length);
	    നിർത്തുക;
	}

    എങ്കിൽ (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    അല്ലെങ്കിൽ
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#എങ്കിൽ defined(FEAT_TIMERS)
    dict_add_സംഖ്യ(dict, "time", wp->w_popup_timer != ശൂന്യ
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    പൂർണ്ണസംഖ്യ
error_if_term_popup_window(ശൂന്യം)
{
    എങ്കിൽ (WIN_IS_POPUP(curwin) && curbuf->b_term != ശൂന്യ
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    ശൂന്യം
popup_reset_handled(പൂർണ്ണസംഖ്യ handled_flag)
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
find_next_popup(പൂർണ്ണസംഖ്യ lowest, പൂർണ്ണസംഖ്യ handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    പൂർണ്ണസംഖ്യ	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = ശൂന്യ;
    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    എങ്കിൽ (found_wp != ശൂന്യ)
	found_wp->w_popup_handled |= handled_flag;
    തിരികെ found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
invoke_popup_filter(win_T *wp, പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    പൂർണ്ണസംഖ്യ		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    എങ്കിൽ (c == Ctrl_C)
    {
	പൂർണ്ണസംഖ്യ save_got_int = got_int;
	പൂർണ്ണസംഖ്യ was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	എങ്കിൽ (was_curwin && wp == curwin)
	    തിരികെ -1;
	തിരികെ TRUE;
    }

    argv[0].v_type = VAR_സംഖ്യ;
    argv[0].vval.v_സംഖ്യ = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    എങ്കിൽ (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	എങ്കിൽ (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    എങ്കിൽ (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    തിരികെ res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    പൂർണ്ണസംഖ്യ
popup_do_filter(പൂർണ്ണസംഖ്യ c)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;
    പൂർണ്ണസംഖ്യ		res = FALSE;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		save_താക്കോൽ_Typed = താക്കോൽ_Typed;
    പൂർണ്ണസംഖ്യ		state;
    പൂർണ്ണസംഖ്യ		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    എങ്കിൽ (popup_is_popup(curwin) && curbuf->b_term != ശൂന്യ)
	തിരികെ FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    എങ്കിൽ (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	തിരികെ FALSE;

    എങ്കിൽ (recursive)
	തിരികെ FALSE;
    recursive = TRUE;

    എങ്കിൽ (c == K_LEFTMOUSE)
    {
	പൂർണ്ണസംഖ്യ row = mouse_row;
	പൂർണ്ണസംഖ്യ col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	എങ്കിൽ (wp != ശൂന്യ && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    ആകുമ്പോൾ (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != ശൂന്യ)
	എങ്കിൽ (wp->w_filter_cb.cb_name != ശൂന്യ
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    എങ്കിൽ (must_redraw > was_must_redraw)
    {
	പൂർണ്ണസംഖ്യ save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    താക്കോൽ_Typed = save_താക്കോൽ_Typed;

    // When interrupted return FALSE to avoid looping.
    തിരികെ res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    പൂർണ്ണസംഖ്യ
popup_no_mapping(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    round;
    win_T   *wp;

    വേണ്ടി (round = 1; round <= 2; ++round)
	വേണ്ടി (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != ശൂന്യ; wp = wp->w_next)
	    എങ്കിൽ (wp->w_filter_cb.cb_name != ശൂന്യ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    ശൂന്യം
popup_check_cursor_pos(ശൂന്യം)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    ആകുമ്പോൾ ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != ശൂന്യ)
	എങ്കിൽ (wp->w_popup_curwin != ശൂന്യ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    സ്ഥിതം ശൂന്യം
popup_update_mask(win_T *wp, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    പൂർണ്ണസംഖ്യ		row, col;

    എങ്കിൽ (wp->w_popup_mask == ശൂന്യ || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	തിരികെ;
    }
    എങ്കിൽ (wp->w_popup_mask_cells != ശൂന്യ
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	തിരികെ;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    എങ്കിൽ (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = ശൂന്യ;
	തിരികെ;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    എങ്കിൽ (wp->w_popup_mask_cells == ശൂന്യ)
	തിരികെ;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_ഇനങ്ങൾ(wp->w_popup_mask, lio)
    {
	പൂർണ്ണസംഖ്യ cols, cole;
	പൂർണ്ണസംഖ്യ വരികൾ, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (cols < 0)
	    cols = width + cols + 1;
	എങ്കിൽ (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (cole < 0)
	    cole = width + cole + 1;
	എങ്കിൽ (cole > width)
	    cole = width;
	li = li->li_next;
	വരികൾ = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (വരികൾ < 0)
	    വരികൾ = height + വരികൾ + 1;
	എങ്കിൽ (വരികൾ <= 0)
	    വരികൾ = 1;
	li = li->li_next;
	linee = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (linee < 0)
	    linee = height + linee + 1;
	എങ്കിൽ (linee > height)
	    linee = height;

	വേണ്ടി (row = വരികൾ - 1; row < linee; ++row)
	    വേണ്ടി (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_masked(win_T *wp, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ screencol, പൂർണ്ണസംഖ്യ screenline)
{
    പൂർണ്ണസംഖ്യ col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    പൂർണ്ണസംഖ്യ വരി = screenline - wp->w_winrow;

    തിരികെ col >= 0 && col < width
	    && വരി >= 0 && വരി < height
	    && wp->w_popup_mask_cells[വരി * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    സ്ഥിതം ശൂന്യം
update_popup_transparent(win_T *wp, പൂർണ്ണസംഖ്യ val)
{
    എങ്കിൽ (wp->w_popup_mask == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ		width = popup_width(wp);
    പൂർണ്ണസംഖ്യ		height = popup_height(wp);
    listitem_T	*lio, *li;
    പൂർണ്ണസംഖ്യ		cols, cole;
    പൂർണ്ണസംഖ്യ		വരികൾ, linee;
    പൂർണ്ണസംഖ്യ		col, വരി;

    FOR_ALL_LIST_ഇനങ്ങൾ(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	വരികൾ = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (വരികൾ < 0)
	    വരികൾ = height + വരികൾ + 1;
	li = li->li_next;
	linee = tv_get_സംഖ്യ(&li->li_tv);
	എങ്കിൽ (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	എങ്കിൽ (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--വരികൾ;
	എങ്കിൽ (വരികൾ < 0)
	    വരികൾ = 0;
	വേണ്ടി (വരി = വരികൾ; വരി < linee
		&& വരി + wp->w_winrow < screen_Rows; ++വരി)
	    വേണ്ടി (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(വരി + wp->w_winrow) * screen_Columns
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_popup_unhidden(win_T *wp)
{
    എങ്കിൽ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	എങ്കിൽ ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    തിരികെ FALSE;
	എങ്കിൽ (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	അല്ലെങ്കിൽ എങ്കിൽ (popup_find_prop_above_മുകൾ(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	എങ്കിൽ (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_need_position_adjust(win_T *wp)
{
    എങ്കിൽ (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	തിരികെ TRUE;
    എങ്കിൽ (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	തിരികെ TRUE;

    // May need to adjust the width if the cursor moved.
    തിരികെ wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
സ്ഥിതം short *opacity_zindex = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ    opacity_zindex_rows = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    സ്ഥിതം ശൂന്യം
popup_mark_opacity_zindex(win_T *wp)
{
    പൂർണ്ണസംഖ്യ	    width;
    പൂർണ്ണസംഖ്യ	    height;
    പൂർണ്ണസംഖ്യ	    r, c;

    എങ്കിൽ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	തിരികെ;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    വേണ്ടി (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	വേണ്ടി (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    പൂർണ്ണസംഖ്യ off = r * screen_Columns + c;
	    എങ്കിൽ (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    സ്ഥിതം ശൂന്യം
redraw_win_under_opacity_popup(win_T *wp)
{
    പൂർണ്ണസംഖ്യ	    height;
    പൂർണ്ണസംഖ്യ	    width;
    പൂർണ്ണസംഖ്യ	    r;

    എങ്കിൽ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	തിരികെ;

    height = popup_height(wp);
    width = popup_width(wp);
    വേണ്ടി (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	പൂർണ്ണസംഖ്യ	    col;
	win_T	    *prev_twp = ശൂന്യ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	വേണ്ടി (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    പൂർണ്ണസംഖ്യ	    വരി_cp = r;
	    പൂർണ്ണസംഖ്യ	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&വരി_cp, &col_cp, IGNORE_POPUP);
	    എങ്കിൽ (twp != ശൂന്യ && twp != prev_twp)
	    {
		prev_twp = twp;
		എങ്കിൽ (വരി_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    എങ്കിൽ (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    അല്ലെങ്കിൽ
#endif
		    {
			(ശൂന്യം)mouse_comp_pos(twp, &വരി_cp, &col_cp, &lnum,
									ശൂന്യ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (വരി_cp == twp->w_height)
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
    പൂർണ്ണസംഖ്യ
popup_is_under_opacity(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (opacity_zindex == ശൂന്യ
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	തിരികെ FALSE;
    തിരികെ opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    പൂർണ്ണസംഖ്യ
popup_is_over_opacity(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    തിരികെ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    പൂർണ്ണസംഖ്യ
popup_is_under_opacity_range(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ start_col, പൂർണ്ണസംഖ്യ end_col)
{
    പൂർണ്ണസംഖ്യ col;

    എങ്കിൽ (opacity_zindex == ശൂന്യ
	    || row < 0 || row >= opacity_zindex_rows)
	തിരികെ FALSE;
    വേണ്ടി (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	എങ്കിൽ (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    ശൂന്യം
may_update_popup_mask(പൂർണ്ണസംഖ്യ type)
{
    win_T	*wp;
    short	*mask;
    പൂർണ്ണസംഖ്യ		വരി, col;
    പൂർണ്ണസംഖ്യ		redraw_all_popups = FALSE;
    പൂർണ്ണസംഖ്യ		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    എങ്കിൽ (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	അല്ലെങ്കിൽ എങ്കിൽ (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	അല്ലെങ്കിൽ എങ്കിൽ (popup_need_position_adjust(wp))
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
	പൂർണ്ണസംഖ്യ has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	എങ്കിൽ (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    എങ്കിൽ (opacity_zindex != ശൂന്യ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * വലുപ്പം(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    എങ്കിൽ (!popup_mask_refresh)
	തിരികെ;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    എങ്കിൽ (redrawing_all_win)
	mask = popup_mask;
    അല്ലെങ്കിൽ
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * വലുപ്പം(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    ആകുമ്പോൾ ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ width;
	പൂർണ്ണസംഖ്യ height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	എങ്കിൽ (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    എങ്കിൽ (wp->w_popup_flags & POPF_HIDDEN)
		തുടരുക;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	എങ്കിൽ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	അല്ലെങ്കിൽ
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    തുടരുക;

	{
	    പൂർണ്ണസംഖ്യ mask_start = wp->w_winrow;
	    പൂർണ്ണസംഖ്യ mask_end = mask_start + height;
	    പൂർണ്ണസംഖ്യ mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    പൂർണ്ണസംഖ്യ mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    വേണ്ടി (വരി = mask_start;
			    വരി < mask_end && വരി < screen_Rows; ++വരി)
		വേണ്ടി (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    എങ്കിൽ (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(വരി, col, FALSE))
			mask[വരി * screen_Columns + col] = POPUPMENU_ZINDEX;
		    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_mask_cells == ശൂന്യ
				    || !popup_masked(wp, width, height, col, വരി))
			mask[വരി * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    എങ്കിൽ (mask == popup_mask_next)
    {
	പൂർണ്ണസംഖ്യ	    *plines_cache = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ, Rows);
	win_T	    *prev_wp = ശൂന്യ;

	വേണ്ടി (വരി = 0; വരി < screen_Rows; ++വരി)
	{
	    പൂർണ്ണസംഖ്യ	    col_done = 0;

	    വേണ്ടി (col = 0; col < screen_Columns; ++col)
	    {
		പൂർണ്ണസംഖ്യ off = വരി * screen_Columns + col;

		എങ്കിൽ (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    എങ്കിൽ (വരി >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			എങ്കിൽ (!സന്ദേശം_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (col >= col_done)
		    {
			linenr_T	lnum;
			പൂർണ്ണസംഖ്യ		വരി_cp = വരി;
			പൂർണ്ണസംഖ്യ		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&വരി_cp, &col_cp, IGNORE_POPUP);
			എങ്കിൽ (wp != ശൂന്യ)
			{
#എങ്കിൽ defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    എങ്കിൽ (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    അല്ലെങ്കിൽ
#endif
			    {
				എങ്കിൽ (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   വലുപ്പം(പൂർണ്ണസംഖ്യ) * Rows);
				    prev_wp = wp;
				}

				എങ്കിൽ (വരി_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				അല്ലെങ്കിൽ
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (ശൂന്യം)mouse_comp_pos(wp, &വരി_cp, &col_cp,
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
    ശൂന്യം
may_update_popup_position(ശൂന്യം)
{
    എങ്കിൽ (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
സ്ഥിതം schar_T *base_screenlines = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ *base_screenattrs = ശൂന്യ;
സ്ഥിതം u8char_T *base_screenlinesuc = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ base_screen_rows = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    പൂർണ്ണസംഖ്യ
popup_get_base_screen_cell(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, schar_T *linep, പൂർണ്ണസംഖ്യ *attrp,
							  u8char_T *ucp)
{
    എങ്കിൽ (base_screenlines == ശൂന്യ || base_screenattrs == ശൂന്യ)
	തിരികെ FALSE;
    എങ്കിൽ (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ off = row * base_screen_cols + col;
    എങ്കിൽ (linep != ശൂന്യ)
	*linep = base_screenlines[off];
    എങ്കിൽ (attrp != ശൂന്യ)
	*attrp = base_screenattrs[off];
    എങ്കിൽ (ucp != ശൂന്യ)
    {
	എങ്കിൽ (enc_utf8 && base_screenlinesuc != ശൂന്യ)
	    *ucp = base_screenlinesuc[off];
	അല്ലെങ്കിൽ
	    *ucp = 0;
    }
    തിരികെ TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    ശൂന്യം
popup_set_base_screen_cell(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, schar_T വരി, പൂർണ്ണസംഖ്യ attr, u8char_T uc)
{
    എങ്കിൽ (base_screenlines == ശൂന്യ || base_screenattrs == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	തിരികെ;

    പൂർണ്ണസംഖ്യ off = row * base_screen_cols + col;
    base_screenlines[off] = വരി;
    base_screenattrs[off] = attr;
    എങ്കിൽ (enc_utf8 && base_screenlinesuc != ശൂന്യ)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
സ്ഥിതം ശൂന്യം
draw_opacity_padding_cell(
	പൂർണ്ണസംഖ്യ		row,
	പൂർണ്ണസംഖ്യ		col,
	popup_saved_screen_T *saved_screen,
	പൂർണ്ണസംഖ്യ		pad_start_col,
	പൂർണ്ണസംഖ്യ		pad_end_col)
{
    എങ്കിൽ (വരി_Offset == ശൂന്യ || Screen_വരികൾ == ശൂന്യ || ScreenAttrs == ശൂന്യ
	    || (enc_utf8 && Screen_വരികൾ_UC == ശൂന്യ))
	തിരികെ;

    പൂർണ്ണസംഖ്യ off = വരി_Offset[row] + col;
    പൂർണ്ണസംഖ്യ r = row - saved_screen->start_row;
    പൂർണ്ണസംഖ്യ c = col - saved_screen->start_col;

    എങ്കിൽ (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	പൂർണ്ണസംഖ്യ save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	എങ്കിൽ (enc_utf8 && saved_screen->linesuc != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ base_col = col - 1;
	    പൂർണ്ണസംഖ്യ base_off = off - 1;
	    പൂർണ്ണസംഖ്യ base_save_off = save_off - 1;
	    പൂർണ്ണസംഖ്യ wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    എങ്കിൽ (base_off >= 0)
	    {
		എങ്കിൽ (Screen_വരികൾ_UC[base_off] != 0
			&& utf_char2cells(Screen_വരികൾ_UC[base_off]) == 2
			&& Screen_വരികൾ[off] == 0)
		    wide_prev = TRUE;
	    }
	    എങ്കിൽ (!wide_prev && save_off > 0)
	    {
		എങ്കിൽ (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->വരികൾ[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    എങ്കിൽ (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		എങ്കിൽ (base_col < pad_start_col)
		{
		    എങ്കിൽ (Screen_വരികൾ_UC[base_off] != 0
			    && utf_char2cells(Screen_വരികൾ_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_വരികൾ[base_off] = ' ';
			Screen_വരികൾ_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_വരികൾ[base_off],
				ScreenAttrs[base_off],
				Screen_വരികൾ_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_വരികൾ[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			എങ്കിൽ (enc_utf8)
			    Screen_വരികൾ_UC[off] = 0;
			പൂർണ്ണസംഖ്യ popup_attr_val =
					get_win_attr(screen_opacity_popup);
			പൂർണ്ണസംഖ്യ blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_വരികൾ[off], ScreenAttrs[off],
				Screen_വരികൾ_UC[off]);
			screen_char(off, row, col);
			തിരികെ;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    എങ്കിൽ (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_വരികൾ[base_off] == ' '
			    && Screen_വരികൾ_UC[base_off] == 0)
		    {
			പൂർണ്ണസംഖ്യ popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			പൂർണ്ണസംഖ്യ blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_വരികൾ[base_off] =
				    saved_screen->വരികൾ[base_save_off];
			Screen_വരികൾ_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_വരികൾ[off] = 0;
			Screen_വരികൾ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_വരികൾ[base_off],
				    ScreenAttrs[base_off],
				    Screen_വരികൾ_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_വരികൾ[off],
				    ScreenAttrs[off],
				    Screen_വരികൾ_UC[off]);
			screen_char(base_off, row, base_col);
			തിരികെ;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_വരികൾ[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    എങ്കിൽ (enc_utf8)
			Screen_വരികൾ_UC[off] = 0;
		    പൂർണ്ണസംഖ്യ popup_attr_val = get_win_attr(screen_opacity_popup);
		    പൂർണ്ണസംഖ്യ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_വരികൾ[off],
					       ScreenAttrs[off], Screen_വരികൾ_UC[off]);
		    screen_char(off, row, col);
		    തിരികെ;
		}

		// Base cell is inside the saved area, redraw the wide char.
		എങ്കിൽ (save_off > 0)
		{
		    Screen_വരികൾ[base_off] = saved_screen->വരികൾ[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_വരികൾ[off] = saved_screen->വരികൾ[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_വരികൾ_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_വരികൾ_UC[off] = saved_screen->linesuc[save_off];

		    പൂർണ്ണസംഖ്യ popup_attr_val = get_win_attr(screen_opacity_popup);
		    പൂർണ്ണസംഖ്യ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_വരികൾ[base_off],
					       ScreenAttrs[base_off], Screen_വരികൾ_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_വരികൾ[off],
					       ScreenAttrs[off], Screen_വരികൾ_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		തിരികെ;
	    }
	}
	Screen_വരികൾ[off] = saved_screen->വരികൾ[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	എങ്കിൽ (enc_utf8)
	    Screen_വരികൾ_UC[off] = saved_screen->linesuc != ശൂന്യ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	എങ്കിൽ (enc_utf8 && Screen_വരികൾ_UC[off] != 0
		&& utf_char2cells(Screen_വരികൾ_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_വരികൾ[off] = ' ';
	    Screen_വരികൾ_UC[off] = 0;
	}

	പൂർണ്ണസംഖ്യ popup_attr_val = get_win_attr(screen_opacity_popup);
	പൂർണ്ണസംഖ്യ blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_വരികൾ[off],
				   ScreenAttrs[off], Screen_വരികൾ_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
സ്ഥിതം ശൂന്യം
fill_opacity_padding(
	പൂർണ്ണസംഖ്യ		start_row,
	പൂർണ്ണസംഖ്യ		end_row,
	പൂർണ്ണസംഖ്യ		start_col,
	പൂർണ്ണസംഖ്യ		end_col,
	popup_saved_screen_T *saved_screen)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ pad_row = start_row; pad_row < end_row; pad_row++)
	വേണ്ടി (പൂർണ്ണസംഖ്യ pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    ശൂന്യം
update_popups(ശൂന്യം (*win_update)(win_T *wp))
{
    win_T   *wp;
    പൂർണ്ണസംഖ്യ	    മുകൾ_off;
    പൂർണ്ണസംഖ്യ	    ഇടത്_extra;
    പൂർണ്ണസംഖ്യ	    total_width;
    പൂർണ്ണസംഖ്യ	    total_height;
    പൂർണ്ണസംഖ്യ	    മുകൾ_padding;
    പൂർണ്ണസംഖ്യ	    popup_attr;
    പൂർണ്ണസംഖ്യ	    border_attr[4];
    പൂർണ്ണസംഖ്യ	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    പൂർണ്ണസംഖ്യ	    row;
    പൂർണ്ണസംഖ്യ	    wincol;
    പൂർണ്ണസംഖ്യ	    padcol = 0;
    പൂർണ്ണസംഖ്യ	    padendcol = 0;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    sb_thumb_മുകൾ = 0;
    പൂർണ്ണസംഖ്യ	    sb_thumb_height = 0;
    പൂർണ്ണസംഖ്യ	    attr_scroll = 0;
    പൂർണ്ണസംഖ്യ	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (base_screenlines != ശൂന്യ)
    {
	vim_free(base_screenlines);
	base_screenlines = ശൂന്യ;
    }
    എങ്കിൽ (base_screenattrs != ശൂന്യ)
    {
	vim_free(base_screenattrs);
	base_screenattrs = ശൂന്യ;
    }
    എങ്കിൽ (base_screenlinesuc != ശൂന്യ)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = ശൂന്യ;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    ആകുമ്പോൾ ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	    title_len = 0;
	പൂർണ്ണസംഖ്യ	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = തള്ളുക_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	അല്ലെങ്കിൽ
	    screen_opacity_popup = ശൂന്യ;

#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (screen_opacity_popup != ശൂന്യ)
	{
	    എങ്കിൽ (base_screenlines != ശൂന്യ)
	    {
		vim_free(base_screenlines);
		base_screenlines = ശൂന്യ;
	    }
	    എങ്കിൽ (base_screenattrs != ശൂന്യ)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = ശൂന്യ;
	    }
	    എങ്കിൽ (base_screenlinesuc != ശൂന്യ)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = ശൂന്യ;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(പൂർണ്ണസംഖ്യ,
				    base_screen_rows * base_screen_cols);
	    എങ്കിൽ (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    എങ്കിൽ (base_screenlines != ശൂന്യ && base_screenattrs != ശൂന്യ)
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < base_screen_rows; r++)
		{
		    പൂർണ്ണസംഖ്യ off = വരി_Offset[r];
		    പൂർണ്ണസംഖ്യ base_off = r * base_screen_cols;
		    വേണ്ടി (പൂർണ്ണസംഖ്യ c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_വരികൾ[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			എങ്കിൽ (enc_utf8 && base_screenlinesuc != ശൂന്യ)
			    base_screenlinesuc[base_off + c] =
					Screen_വരികൾ_UC[off + c];
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
	മുകൾ_off = popup_മുകൾ_extra(wp);
	ഇടത്_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	എങ്കിൽ (wp->w_wincol + ഇടത്_extra < 0)
	    ഇടത്_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(മുകൾ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ഇടത്_extra;

	// Draw the popup text, unless it's off screen.
	എങ്കിൽ (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    എങ്കിൽ (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    എങ്കിൽ (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(മുകൾ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ഇടത്_extra;

	// Add offset for border and padding if not done already.
	എങ്കിൽ ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ഇടത്_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	എങ്കിൽ ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += മുകൾ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	എങ്കിൽ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	അല്ലെങ്കിൽ
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	എങ്കിൽ (total_width < 0)
	    total_width = 0;
	എങ്കിൽ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	അല്ലെങ്കിൽ
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	എങ്കിൽ (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	അല്ലെങ്കിൽ
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	എങ്കിൽ (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	അല്ലെങ്കിൽ
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    വേണ്ടി (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    എങ്കിൽ (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	വേണ്ടി (i = 0; i < 8; ++i)
	    എങ്കിൽ (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	വേണ്ടി (i = 0; i < 4; ++i)
	{
	    എങ്കിൽ (wp->w_border_highlight[i] != ശൂന്യ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    അല്ലെങ്കിൽ
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	എങ്കിൽ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	എങ്കിൽ (wp->w_popup_title != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	    title_attr;

	    എങ്കിൽ (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ശൂന്യ)
		title_attr = border_attr[0];
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    അല്ലെങ്കിൽ
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    എങ്കിൽ (title_len > total_width - 2)
	    {
		പൂർണ്ണസംഖ്യ	title_byte_len = (പൂർണ്ണസംഖ്യ)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		എങ്കിൽ (title_text != ശൂന്യ)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    അല്ലെങ്കിൽ
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	മുകൾ_padding = cl.eff_padding[0];
	എങ്കിൽ (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    എങ്കിൽ (title_len > 0)
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
	    അല്ലെങ്കിൽ
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    എങ്കിൽ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cl.eff_padding[0] == 0 && popup_മുകൾ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    മുകൾ_padding = 1;

	എങ്കിൽ (മുകൾ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    എങ്കിൽ (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	എങ്കിൽ (മുകൾ_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    എങ്കിൽ (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		എങ്കിൽ (screen_opacity_popup != ശൂന്യ && saved_screen.വരികൾ != ശൂന്യ)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		അല്ലെങ്കിൽ
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		മുകൾ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    എങ്കിൽ (screen_opacity_popup != ശൂന്യ && saved_screen.വരികൾ != ശൂന്യ)
	    {
		fill_opacity_padding(row, row + മുകൾ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    അല്ലെങ്കിൽ
	    {
		screen_fill(row, row + മുകൾ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	എങ്കിൽ (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	    പൂർണ്ണസംഖ്യ		height = wp->w_height;
	    പൂർണ്ണസംഖ്യ		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    എങ്കിൽ (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    എങ്കിൽ (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    എങ്കിൽ (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_മുകൾ = 0;
	    അല്ലെങ്കിൽ
		sb_thumb_മുകൾ = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    എങ്കിൽ (wp->w_topline > 1 && sb_thumb_മുകൾ == 0 && height > 1)
		sb_thumb_മുകൾ = 1;  // show it's scrolled
	    last = total_height - മുകൾ_off - wp->w_popup_border[2];
	    എങ്കിൽ (sb_thumb_മുകൾ >= last)
		// show at least one character
		sb_thumb_മുകൾ = last - 1;

	    എങ്കിൽ (wp->w_scrollbar_highlight != ശൂന്യ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    അല്ലെങ്കിൽ
		attr_scroll = highlight_attr[HLF_PSB];
	    എങ്കിൽ (wp->w_thumb_highlight != ശൂന്യ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    അല്ലെങ്കിൽ
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	വേണ്ടി (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    പൂർണ്ണസംഖ്യ	pad_ഇടത്;
	    // left and right padding only needed next to the body
	    പൂർണ്ണസംഖ്യ do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    എങ്കിൽ (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    എങ്കിൽ (do_padding && cl.eff_padding[3] > 0)
	    {
		പൂർണ്ണസംഖ്യ col = wincol + wp->w_popup_border[3];

		// left padding
		pad_ഇടത് = wp->w_popup_padding[3];
		എങ്കിൽ (col < 0)
		{
		    pad_ഇടത് += col;
		    col = 0;
		}
		എങ്കിൽ (pad_ഇടത് > 0)
		{
		    എങ്കിൽ (screen_opacity_popup != ശൂന്യ && saved_screen.വരികൾ != ശൂന്യ)
			fill_opacity_padding(row, row + 1, col, col + pad_ഇടത്,
				&saved_screen);
		    അല്ലെങ്കിൽ
			screen_fill(row, row + 1, col, col + pad_ഇടത്,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    എങ്കിൽ (wp->w_has_scrollbar)
	    {
		പൂർണ്ണസംഖ്യ വരി = i - മുകൾ_off;
		പൂർണ്ണസംഖ്യ scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		എങ്കിൽ (വരി >= 0 && വരി < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    വരി >= sb_thumb_മുകൾ
				       && വരി < sb_thumb_മുകൾ + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		അല്ലെങ്കിൽ
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    എങ്കിൽ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    എങ്കിൽ (do_padding && cl.eff_padding[1] > 0)
	    {
		പൂർണ്ണസംഖ്യ pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		പൂർണ്ണസംഖ്യ pad_col_end = pad_col_start + wp->w_popup_padding[1];

		എങ്കിൽ (screen_opacity_popup != ശൂന്യ && saved_screen.വരികൾ != ശൂന്യ)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		അല്ലെങ്കിൽ
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	എങ്കിൽ (wp->w_popup_shadow)
	{
	    പൂർണ്ണസംഖ്യ col = wincol + total_width;
	    വേണ്ടി (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	എങ്കിൽ (cl.eff_padding[2] > 0)
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
	    എങ്കിൽ (screen_opacity_popup != ശൂന്യ && saved_screen.വരികൾ != ശൂന്യ)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    അല്ലെങ്കിൽ
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	എങ്കിൽ (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    എങ്കിൽ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	എങ്കിൽ (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	എങ്കിൽ (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ശൂന്യ;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	എങ്കിൽ (override_success)
	    എടുക്കുക_highlight_overrides();

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

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
    // In case win_update() called start_search_hl().
    end_തിരയൽ_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_one_popup(win_T *wp, പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    typval_T	tv;

    എങ്കിൽ (wp->w_close_cb.cb_partial != ശൂന്യ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    }
    എങ്കിൽ (wp->w_filter_cb.cb_partial != ശൂന്യ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    തിരികെ abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_popups(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    വേണ്ടി (wp = first_popupwin; !abort && wp != ശൂന്യ; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	വേണ്ടി (wp = tp->tp_first_popupwin; !abort && wp != ശൂന്യ; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	എങ്കിൽ (abort)
	    നിർത്തുക;
    }
    തിരികെ abort;
}

    പൂർണ്ണസംഖ്യ
popup_is_popup(win_T *wp)
{
    തിരികെ wp->w_popup_flags != 0;
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(ശൂന്യം)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp->w_p_pvw)
	    തിരികെ wp;
    തിരികെ ശൂന്യ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(ശൂന്യം)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	എങ്കിൽ (wp->w_popup_flags & POPF_INFO)
	    തിരികെ wp;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(പൂർണ്ണസംഖ്യ dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    പൂർണ്ണസംഖ്യ		by;
    linenr_T	new_topline;

    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    എങ്കിൽ (new_topline < 1)
	new_topline = 1;
    എങ്കിൽ (new_topline > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
	new_topline = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
    എങ്കിൽ (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    തിരികെ true;
#അല്ലെങ്കിൽ
    തിരികെ false;
#endif
}

    ശൂന്യം
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_സംഖ്യ = message_win == ശൂന്യ ? 0 : message_win->w_id;
#അല്ലെങ്കിൽ
    rettv->vval.v_സംഖ്യ = 0;
#endif
}

    ശൂന്യം
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_സംഖ്യ = wp == ശൂന്യ ? 0 : wp->w_id;
#അല്ലെങ്കിൽ
    rettv->vval.v_സംഖ്യ = 0;
#endif
}

    ശൂന്യം
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_സംഖ്യ = wp == ശൂന്യ ? 0 : wp->w_id;
#അല്ലെങ്കിൽ
    rettv->vval.v_സംഖ്യ = 0;
#endif
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    പൂർണ്ണസംഖ്യ
popup_create_preview_window(പൂർണ്ണസംഖ്യ info)
{
    win_T *wp = popup_create(ശൂന്യ, ശൂന്യ, info ? TYPE_INFO : TYPE_PREVIEW);

    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (info)
	wp->w_popup_flags |= POPF_INFO;
    അല്ലെങ്കിൽ
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    എങ്കിൽ (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    അല്ലെങ്കിൽ
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    തിരികെ OK;
}

/*
 * Close any preview popup.
 */
    ശൂന്യം
popup_close_preview(ശൂന്യം)
{
    win_T *wp = popup_find_preview_window();

    എങ്കിൽ (wp != ശൂന്യ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    ശൂന്യം
popup_hide_info(ശൂന്യം)
{
    win_T *wp = popup_find_info_window();

    എങ്കിൽ (wp != ശൂന്യ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	എങ്കിൽ (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    ശൂന്യം
popup_close_info(ശൂന്യം)
{
    win_T *wp = popup_find_info_window();

    എങ്കിൽ (wp != ശൂന്യ)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    പൂർണ്ണസംഖ്യ
popup_overlaps_cmdline(ശൂന്യം)
{
    തിരികെ popup_on_cmdline;
}

#എങ്കിൽ defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(ശൂന്യം)
{
    എങ്കിൽ (message_win != ശൂന്യ)
	തിരികെ message_win;

    പൂർണ്ണസംഖ്യ i;

    message_win = popup_create(ശൂന്യ, ശൂന്യ, TYPE_MESSAGE_WIN);

    എങ്കിൽ (message_win == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    വേണ്ടി (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    വേണ്ടി (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    എങ്കിൽ (message_win->w_popup_timer != ശൂന്യ)
	message_win->w_popup_timer->tr_keep = TRUE;
    തിരികെ message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    ശൂന്യം
popup_show_message_win(ശൂന്യം)
{
    എങ്കിൽ (message_win == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    സ്ഥിതം ശൂന്യം
may_start_message_win_timer(win_T *wp)
{
    എങ്കിൽ (wp == message_win && start_message_win_timer)
    {
	എങ്കിൽ (message_win->w_popup_timer != ശൂന്യ)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    പൂർണ്ണസംഖ്യ
popup_message_win_visible(ശൂന്യം)
{
    തിരികെ message_win != ശൂന്യ
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    ശൂന്യം
popup_hide_message_win(ശൂന്യം)
{
    എങ്കിൽ (message_win != ശൂന്യ)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
സ്ഥിതം പൂർണ്ണസംഖ്യ save_സന്ദേശം_didout = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ save_സന്ദേശം_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
സ്ഥിതം പൂർണ്ണസംഖ്യ ew_സന്ദേശം_didout = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ ew_സന്ദേശം_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    ശൂന്യം
start_echowindow(പൂർണ്ണസംഖ്യ time_sec)
{
    in_echowindow = TRUE;
    save_സന്ദേശം_didout = സന്ദേശം_didout;
    save_സന്ദേശം_col = സന്ദേശം_col;
    സന്ദേശം_didout = ew_സന്ദേശം_didout;
    സന്ദേശം_col = ew_സന്ദേശം_col;
    എങ്കിൽ (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    ശൂന്യം
end_echowindow(ശൂന്യം)
{
    in_echowindow = FALSE;

    എങ്കിൽ ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_സന്ദേശം_didout = TRUE;
    ew_സന്ദേശം_col = സന്ദേശം_col == 0 ? 1 : സന്ദേശം_col;
    സന്ദേശം_didout = save_സന്ദേശം_didout;
    സന്ദേശം_col = save_സന്ദേശം_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    പൂർണ്ണസംഖ്യ
popup_win_closed(win_T *win)
{
    പൂർണ്ണസംഖ്യ	    round;
    win_T   *wp;
    win_T   *next;
    പൂർണ്ണസംഖ്യ	    ret = FALSE;

    വേണ്ടി (round = 1; round <= 2; ++round)
	വേണ്ടി (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != ശൂന്യ; wp = next)
	{
	    next = wp->w_next;
	    എങ്കിൽ (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    തിരികെ ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    ശൂന്യം
popup_set_title(win_T *wp)
{
    എങ്കിൽ (wp->w_buffer->b_fname == ശൂന്യ)
	തിരികെ;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    എങ്കിൽ (wp->w_popup_title != ശൂന്യ)
	vim_snprintf((അക്ഷരം *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    ശൂന്യം
popup_update_preview_title(ശൂന്യം)
{
    win_T *wp = popup_find_preview_window();

    എങ്കിൽ (wp != ശൂന്യ)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
