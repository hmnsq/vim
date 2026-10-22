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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_PROP_POPUP)

प्रकारनिर्देश संरचना {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		active;
    पूर्णाङ्क		winrow;
    पूर्णाङ्क		wincol;
    पूर्णाङ्क		height;
    पूर्णाङ्क		width;
    पूर्णाङ्क		leftoff;
    पूर्णाङ्क		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
प्रकारनिर्देश संरचना {
    schar_T	*पंक्तियाँ;
    पूर्णाङ्क		*attrs;
    u8char_T	*linesuc;
    पूर्णाङ्क		start_row;
    पूर्णाङ्क		start_col;
    पूर्णाङ्क		rows;
    पूर्णाङ्क		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
प्रकारनिर्देश संरचना {
    linenr_T	firstline;
    पूर्णाङ्क		blend;
    पूर्णाङ्क		flags;
    पूर्णाङ्क		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		winrow;
    पूर्णाङ्क		wincol;
    पूर्णाङ्क		width;
    पूर्णाङ्क		height;
    पूर्णाङ्क		leftcol;
    पूर्णाङ्क		leftoff;
    पूर्णाङ्क		has_scrollbar;
    पूर्णाङ्क		topoff;
    पूर्णाङ्क		bottomoff;
    पूर्णाङ्क		leftclip;
    पूर्णाङ्क		rightclip;
} popup_layout_T;

स्थिर poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
स्थिर win_T *message_win = रिक्त;

// Time used for the next ":echowindow" message in msec.
स्थिर पूर्णाङ्क  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
स्थिर पूर्णाङ्क    start_message_win_timer = FALSE;

स्थिर शून्य may_start_message_win_timer(win_T *wp);
#endif

स्थिर पूर्णाङ्क popup_on_cmdline = FALSE;

स्थिर शून्य popup_adjust_position(win_T *wp);
स्थिर bool popup_area_changed(win_T *wp, popup_area_T *area);
स्थिर शून्य popup_redraw_exposed_area(popup_area_T *area);
स्थिर शून्य popup_save_area(win_T *wp, popup_area_T *area);
स्थिर शून्य popup_free_saved_screen(popup_saved_screen_T *saved_screen);
स्थिर शून्य popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
स्थिर bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
स्थिर bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
स्थिर शून्य popup_save_style(win_T *wp, popup_style_snapshot_T *style);
स्थिर शून्य popup_save_layout(win_T *wp, popup_layout_T *layout);
स्थिर शून्य redraw_under_popup_area(पूर्णाङ्क winrow, पूर्णाङ्क wincol, पूर्णाङ्क height,
	पूर्णाङ्क width, पूर्णाङ्क leftoff);
स्थिर शून्य redraw_overlapped_opacity_popups(पूर्णाङ्क winrow, पूर्णाङ्क wincol,
	पूर्णाङ्क height, पूर्णाङ्क width, पूर्णाङ्क leftoff, पूर्णाङ्क zindex);
स्थिर शून्य redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    स्थिर पूर्णाङ्क
popup_options_one(dict_T *dict, char_u *कुंजी)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    पूर्णाङ्क		n = 0;

    di = dict_find(dict, कुंजी, -1);
    यदि (di == रिक्त)
	प्रत्यागम MAXCOL;

    val = tv_get_string(&di->di_tv);
    यदि (STRNCMP(val, "cursor", 6) != 0)
	प्रत्यागम dict_get_संख्या_check(dict, कुंजी);

    setcursor_mayforce(TRUE);
    s = val + 6;
    यदि (*s != NUL)
    {
	endp = s;
	यदि (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((वर्ण *)s, (वर्ण **)&endp, 10);
	यदि (endp != रिक्त && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    प्रत्यागम 0;
	}
    }

    यदि (STRCMP(कुंजी, "line") == 0)
	n = screen_screenrow() + 1 + n;
    अन्यथा // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    यदि (n == 0)
	n = -1;
    प्रत्यागम n;
}

    स्थिर पूर्णाङ्क
set_padding_border(dict_T *dict, पूर्णाङ्क *array, वर्ण *name, पूर्णाङ्क max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    यदि (di == रिक्त)
	प्रत्यागम OK;

    यदि (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	प्रत्यागम FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    पूर्णाङ्क		i;
    पूर्णाङ्क		nr;

    प्रति (i = 0; i < 4; ++i)
	array[i] = 1;
    यदि (list == रिक्त)
	प्रत्यागम OK;

    CHECK_LIST_MATERIALIZE(list);
    प्रति (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (पूर्णाङ्क)tv_get_संख्या(&li->li_tv);
	यदि (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    प्रत्यागम OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    स्थिर शून्य
set_moved_मूल्यावली(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    स्थिर शून्य
set_moved_columns(win_T *wp, पूर्णाङ्क flags)
{
    char_u	*ptr;
    पूर्णाङ्क		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    पूर्णाङ्क		mincol;

    यदि (len <= 0)
	प्रत्यागम;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (पूर्णाङ्क)(ptr - ml_get_curline());
    यदि (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	प्रत्यागम;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    स्थिर शून्य
set_mousemoved_मूल्यावली(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    स्थिर शून्य
update_popup_uses_mouse_move(शून्य)
{
    popup_uses_mouse_move = FALSE;
    यदि (!popup_visible)
	प्रत्यागम;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	यदि (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    प्रत्यागम;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    प्रत्यागम;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    स्थिर शून्य
set_mousemoved_columns(win_T *wp, पूर्णाङ्क flags)
{
    win_T	*textwp;
    char_u	*text;
    पूर्णाङ्क		col;
    pos_T	pos;
    colnr_T	mcol;

    यदि (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, रिक्त, &col) != OK)
	प्रत्यागम;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, रिक्त, रिक्त, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, रिक्त, रिक्त, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    पूर्णाङ्क
popup_on_border(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    प्रत्यागम (row == 0 && wp->w_popup_border[0] > 0)
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
    पूर्णाङ्क
popup_close_if_on_X(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    यदि (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

// Values set when dragging a popup window starts.
स्थिर पूर्णाङ्क drag_start_row;
स्थिर पूर्णाङ्क drag_start_col;
स्थिर पूर्णाङ्क drag_start_wantline;
स्थिर पूर्णाङ्क drag_start_wantcol;
स्थिर पूर्णाङ्क drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    शून्य
popup_start_drag(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    यदि (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    अन्यथा
	drag_start_wantline = wp->w_wantline;
    यदि (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    अन्यथा
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    यदि (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    यदि (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	यदि (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	यदि (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    शून्य
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    यदि (!win_valid_popup(wp))
	प्रत्यागम;

    यदि ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	पूर्णाङ्क width_inc = mouse_col - drag_start_col;
	पूर्णाङ्क height_inc = mouse_row - drag_start_row;

	यदि (width_inc != 0)
	{
	    पूर्णाङ्क width = wp->w_width + width_inc;

	    यदि (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	यदि (height_inc != 0)
	{
	    पूर्णाङ्क height = wp->w_height + height_inc;

	    यदि (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	प्रत्यागम;
    }

    यदि (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	प्रत्यागम;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    यदि (wp->w_wantline < 1)
	wp->w_wantline = 1;
    यदि (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    यदि (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    यदि (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    शून्य
popup_set_firstline(win_T *wp)
{
    पूर्णाङ्क	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    यावत् (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    पूर्णाङ्क
popup_is_in_scrollbar(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    प्रत्यागम wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    शून्य
popup_handle_scrollbar_click(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    यदि (!popup_is_in_scrollbar(wp, row, col))
	प्रत्यागम;

    पूर्णाङ्क	    height = popup_height(wp);
    पूर्णाङ्क	    new_topline = wp->w_topline;

    यदि (row >= height / 2)
    {
	// Click in lower half, scroll down.
	यदि (wp->w_topline < wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	    ++new_topline;
    }
    अन्यथा यदि (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    यदि (new_topline == wp->w_topline)
	प्रत्यागम;

    set_topline(wp, new_topline);
    यदि (wp == curwin)
    {
	यदि (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	अन्यथा यदि (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#यदि defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    स्थिर शून्य
popup_add_timeout(win_T *wp, पूर्णाङ्क time, पूर्णाङ्क close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((वर्ण *)cbbuf, परिमाण(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    यदि (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	प्रत्यागम;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    यदि (cb.cb_name != रिक्त && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    स्थिर poppos_T
get_pos_entry(dict_T *d, पूर्णाङ्क give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    पूर्णाङ्क	    nr;

    यदि (str == रिक्त)
	प्रत्यागम POPPOS_NONE;

    प्रति (nr = 0; nr < (पूर्णाङ्क)ARRAY_LENGTH(poppos_entries); ++nr)
	यदि (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    प्रत्यागम poppos_entries[nr].pp_val;

    यदि (give_error)
	semsg(_(e_invalid_argument_str), str);
    प्रत्यागम POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    स्थिर शून्य
apply_move_options(win_T *wp, dict_T *d)
{
    पूर्णाङ्क		nr;
    char_u	*str;
    dictitem_T	*di;

    यदि ((nr = dict_get_संख्या_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    यदि ((nr = dict_get_संख्या_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    यदि ((nr = dict_get_संख्या_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    यदि ((nr = dict_get_संख्या_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    यदि (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    यदि (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    यदि (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	यदि (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    यदि (str != रिक्त)
    {
	wp->w_popup_prop_type = 0;
	यदि (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    यदि (di != रिक्त)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		यदि (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    यदि (nr <= 0)
		nr = find_prop_type_id(str, रिक्त);
	    यदि (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    अन्यथा
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    यदि (di != रिक्त)
	wp->w_popup_prop_id = dict_get_संख्या(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    स्थिर शून्य
handle_moved_argument(win_T *wp, dictitem_T *di, पूर्णाङ्क mousemoved)
{
    यदि (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != रिक्त)
    {
	char_u  *s = di->di_tv.vval.v_string;
	पूर्णाङ्क	flags = 0;

	यदि (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	अन्यथा यदि (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	अन्यथा यदि (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	अन्यथा यदि (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	यदि (flags != 0)
	{
	    यदि (mousemoved)
		set_mousemoved_columns(wp, flags);
	    अन्यथा
		set_moved_columns(wp, flags);
	}
    }
    अन्यथा यदि (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != रिक्त
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	पूर्णाङ्क	    mincol;
	पूर्णाङ्क	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	यदि (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_संख्या(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    यदि (mousemoved)
		wp->w_popup_mouse_row = nr;
	    अन्यथा
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    यदि (nr == 0)
		wp->w_popup_curwin = रिक्त;
	}

	mincol = tv_get_संख्या(&li->li_tv);
	maxcol = tv_get_संख्या(&li->li_next->li_tv);
	यदि (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	अन्यथा
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    अन्यथा
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    स्थिर शून्य
check_highlight(dict_T *dict, वर्ण *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    यदि (di == रिक्त)
	प्रत्यागम;

    यदि (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_मूल्य_for_argument_str), name);
    अन्यथा
    {
	str = tv_get_string(&di->di_tv);
	यदि (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    स्थिर शून्य
popup_show_curline(win_T *wp)
{
    यदि (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    अन्यथा यदि (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	यदि (wp->w_topline < 1)
	    wp->w_topline = 1;
	अन्यथा यदि (wp->w_topline > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	    wp->w_topline = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
	यावत् (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_पंक्ति_गिनती
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    यदि (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    स्थिर char_u *
popup_get_sign_name(win_T *wp)
{
    स्थिर वर्ण    buf[30];

    vim_snprintf(buf, परिमाण(buf), "popup-%d", wp->w_id);
    प्रत्यागम (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    स्थिर शून्य
popup_highlight_curline(win_T *wp)
{
    पूर्णाङ्क	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    यदि ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	यदि (!sign_exists_by_name(sign_name))
	{
	    वर्ण *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, रिक्त, (char_u *)linehl, रिक्त, रिक्त, रिक्त,
		    रिक्त, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    अन्यथा
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    स्थिर पूर्णाङ्क
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    पूर्णाङ्क		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    यदि (di != रिक्त)
    {
	wp->w_firstline = dict_get_संख्या(dict, "firstline");
	यदि (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    यदि (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    यदि (str != रिक्त)
    {
	char_u	*title = vim_strsave(str);

	यदि (title != रिक्त)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    यदि (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    यदि (di != रिक्त)
    {
	nr = dict_get_संख्या(dict, "opacity");
	यदि (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	अन्यथा यदि (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	अन्यथा यदि (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	अन्यथा
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    यदि (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    अन्यथा
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    यदि (di != रिक्त && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != रिक्त)
    {
	image_T		    *img;
	image_T		    *cur_img = रिक्त;
	image_placement_T   *place;
	पूर्णाङ्क		    cw, ch;

	यदि (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    यदि (wp->w_popup_imagep != रिक्त)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = रिक्त;
	    }
	    गच्छ image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	यदि (wp->w_popup_imagep != रिक्त && wp->w_popup_imagep->img != रिक्त)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	यदि (img == रिक्त)
	    प्रत्यागम FAIL;

	यदि (img == cur_img)
	    गच्छ image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	यदि (place == रिक्त)
	{
	    image_unref(img);
	    प्रत्यागम FAIL;
	}

	यदि (wp->w_popup_imagep != रिक्त)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	यदि (dict_find(dict, (char_u *)"minwidth", -1) == रिक्त)
	    wp->w_minwidth = cw;
	यदि (dict_find(dict, (char_u *)"maxwidth", -1) == रिक्त)
	    wp->w_maxwidth = cw;
	यदि (dict_find(dict, (char_u *)"minheight", -1) == रिक्त)
	    wp->w_minheight = ch;
	यदि (dict_find(dict, (char_u *)"maxheight", -1) == रिक्त)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    अन्यथा
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	यदि (wp->w_popup_imagep != रिक्त)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(रिक्त, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	यदि (wp->w_popup_imagep != रिक्त)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    यदि (di != रिक्त)
    {
	पूर्णाङ्क ok = TRUE;

	यदि (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != रिक्त)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    यदि (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    अन्यथा यदि (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    अन्यथा यदि (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    अन्यथा
		ok = FALSE;
	}
	अन्यथा
	    ok = FALSE;
	यदि (!ok)
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    यदि (str != रिक्त)
    {
	वर्ण *errmsg = update_wincolor(wp, str);

	यदि (errmsg == रिक्त)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	अन्यथा
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    यदि (str != रिक्त)
    {
	वर्ण *errmsg = update_winhighlight(wp, str);

	यदि (errmsg == रिक्त)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	अन्यथा
	    emsg(_(errmsg));
    }

    यदि (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	प्रत्यागम FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    यदि (di != रिक्त)
    {
	यदि (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == रिक्त)
	{
	    emsg(_(e_list_required));
	    प्रत्यागम FAIL;
	}
	अन्यथा
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    पूर्णाङ्क		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    यदि (list->lv_len == 0)
	    {
		प्रति (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = रिक्त;
		}
	    }
	    अन्यथा
	    {
		प्रति (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    यदि (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			यदि (hl != रिक्त)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		यदि (list->lv_len == 1 && wp->w_border_highlight[0] != रिक्त)
		    प्रति (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			यदि (hl != रिक्त)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    यदि (di != रिक्त)
    {
	यदि (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    प्रत्यागम FAIL;
	}
	अन्यथा
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    पूर्णाङ्क		i;

	    यदि (list != रिक्त)
	    {
		CHECK_LIST_MATERIALIZE(list);
		प्रति (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    यदि (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		यदि (list->lv_len == 1)
		    प्रति (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		यदि (list->lv_len == 2)
		{
		    प्रति (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    प्रति (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    यदि (di != रिक्त)
    {
	wp->w_zindex = dict_get_संख्या(dict, "zindex");
	यदि (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	यदि (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    यदि (di != रिक्त)
    {
	पूर्णाङ्क ok = FALSE;

	यदि (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != रिक्त)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_वस्तुएँ(di->di_tv.vval.v_list, li)
	    {
		यदि (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == रिक्त
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    भङ्ग;
		}
		अन्यथा
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	यदि (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	अन्यथा
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), "mask");
	    प्रत्यागम FAIL;
	}
    }

#यदि defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_संख्या(dict, "time");
    यदि (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    यदि (di != रिक्त)
    {
	set_moved_मूल्यावली(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    यदि (di != रिक्त)
    {
	set_mousemoved_मूल्यावली(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    यदि (nr != -1)
    {
	यदि (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    यदि (di != रिक्त)
    {
	callback_T	callback = get_callback(&di->di_tv);

	यदि (callback.cb_name != रिक्त)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    यदि (nr != -1)
    {
	यदि (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    यदि (str != रिक्त)
    {
	यदि (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	अन्यथा
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    यदि (di == रिक्त)
	प्रत्यागम OK;

    callback_T	callback = get_callback(&di->di_tv);
    यदि (callback.cb_name == रिक्त)
	प्रत्यागम OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    प्रत्यागम OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    स्थिर पूर्णाङ्क
apply_options(win_T *wp, dict_T *dict, पूर्णाङ्क create)
{
    पूर्णाङ्क		nr;

    apply_move_options(wp, dict);

    यदि (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    यदि (apply_general_options(wp, dict) == FAIL)
	प्रत्यागम FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    यदि (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    यदि (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	यदि (wp->w_firstline > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
	अन्यथा यदि (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    यदि (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    प्रत्यागम OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    स्थिर शून्य
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_वस्तुएँ(l, li)
	यदि (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == रिक्त ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    स्थिर शून्य
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_वस्तुएँ(l, li)
    {
	यदि (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    प्रत्यागम;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == रिक्त ? रिक्त : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == रिक्त ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    प्रति (li = l->lv_first; li != रिक्त; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	यदि (di != रिक्त)
	{
	    यदि (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		प्रत्यागम;
	    }
	    plist = di->di_tv.vval.v_list;
	    यदि (plist != रिक्त)
	    {
		FOR_ALL_LIST_वस्तुएँ(plist, pli)
		{
		    यदि (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			प्रत्यागम;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    यदि (dict != रिक्त)
		    {
			पूर्णाङ्क col = dict_get_संख्या(dict, "col");

			prop_add_common(lnum, col, dict, buf, रिक्त);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    पूर्णाङ्क
popup_शीर्ष_extra(win_T *wp)
{
    पूर्णाङ्क	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    यदि (extra == 0 && wp->w_popup_title != रिक्त && *wp->w_popup_title != NUL)
	प्रत्यागम 1;
    प्रत्यागम extra;
}

/*
 * Get the padding plus border at the left.
 */
    पूर्णाङ्क
popup_बायाँ_extra(win_T *wp)
{
    प्रत्यागम wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    पूर्णाङ्क
popup_height(win_T *wp)
{
    प्रत्यागम wp->w_height
	    + popup_शीर्ष_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    पूर्णाङ्क
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    प्रत्यागम wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    पूर्णाङ्क
popup_extra_width(win_T *wp)
{
    प्रत्यागम wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    स्थिर win_T *
popup_get_clipwin(win_T *wp)
{
    यदि (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	प्रत्यागम रिक्त;
    यदि (win_valid(wp->w_popup_prop_win))
	प्रत्यागम wp->w_popup_prop_win;
    प्रत्यागम रिक्त;
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
प्रकारनिर्देश संरचना {
    पूर्णाङ्क शीर्ष_extra;
    पूर्णाङ्क bot_extra;
    पूर्णाङ्क बायाँ_extra;
    पूर्णाङ्क दायाँ_extra;

    पूर्णाङ्क clip_शीर्ष_content;
    पूर्णाङ्क clip_bot_content;
    पूर्णाङ्क clip_बायाँ_content;
    पूर्णाङ्क clip_दायाँ_content;

    पूर्णाङ्क eff_शीर्ष_extra;
    पूर्णाङ्क eff_bot_extra;
    पूर्णाङ्क eff_बायाँ_extra;
    पूर्णाङ्क eff_दायाँ_extra;

    पूर्णाङ्क eff_border[4];
    पूर्णाङ्क eff_padding[4];

    पूर्णाङ्क eff_height;
    पूर्णाङ्क eff_width;
} popup_clip_T;

    स्थिर शून्य
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    पूर्णाङ्क h, w;

    cl->शीर्ष_extra = popup_शीर्ष_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->बायाँ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->दायाँ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_शीर्ष_content = wp->w_popup_topoff - cl->शीर्ष_extra;
    यदि (cl->clip_शीर्ष_content < 0)
	cl->clip_शीर्ष_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    यदि (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_बायाँ_content = wp->w_popup_leftclip - cl->बायाँ_extra;
    यदि (cl->clip_बायाँ_content < 0)
	cl->clip_बायाँ_content = 0;
    cl->clip_दायाँ_content = wp->w_popup_rightclip - cl->दायाँ_extra;
    यदि (cl->clip_दायाँ_content < 0)
	cl->clip_दायाँ_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	पूर्णाङ्क clip = wp->w_popup_topoff;
	पूर्णाङ्क b = wp->w_popup_border[0];
	पूर्णाङ्क p = wp->w_popup_padding[0];
	पूर्णाङ्क rem;

	यदि (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	अन्यथा
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	पूर्णाङ्क clip = wp->w_popup_bottomoff;
	पूर्णाङ्क b = wp->w_popup_border[2];
	पूर्णाङ्क p = wp->w_popup_padding[2];
	पूर्णाङ्क rem;

	यदि (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	अन्यथा
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
	पूर्णाङ्क excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	यदि (excess > 0)
	{
	    यदि (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		यदि (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		अन्यथा
		    cl->eff_border[0] -= excess;
	    }
	    अन्यथा
		cl->eff_padding[0] -= excess;
	}
    }
    {
	पूर्णाङ्क excess = wp->w_popup_topoff - cl->शीर्ष_extra - wp->w_height;
	यदि (excess > 0)
	{
	    यदि (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		यदि (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		अन्यथा
		    cl->eff_border[2] -= excess;
	    }
	    अन्यथा
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_शीर्ष_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_बायाँ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->बायाँ_extra;
    cl->eff_दायाँ_extra = wp->w_popup_rightclip > 0 ? 0 : cl->दायाँ_extra;

    h = wp->w_height - cl->clip_शीर्ष_content - cl->clip_bot_content;
    यदि (h < 0)
	h = 0;
    cl->eff_height = cl->eff_शीर्ष_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_बायाँ_content - cl->clip_दायाँ_content;
    यदि (w < 0)
	w = 0;
    cl->eff_width = cl->eff_बायाँ_extra + w + cl->eff_दायाँ_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		height;
    पूर्णाङ्क		width;
    पूर्णाङ्क		winrow;
    पूर्णाङ्क		wincol;
    पूर्णाङ्क		leftcol;
    पूर्णाङ्क		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    स्थिर शून्य
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

    स्थिर शून्य
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
    स्थिर शून्य
popup_screenpos_above_शीर्ष(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	पूर्णाङ्क	    *screen_row,
	पूर्णाङ्क	    *screen_scol,
	पूर्णाङ्क	    *screen_ccol,
	पूर्णाङ्क	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    पूर्णाङ्क	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (पूर्णाङ्क)win_col_off(prop_win) - (पूर्णाङ्क)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (पूर्णाङ्क)scol + coloff;
    *screen_ccol = (पूर्णाङ्क)ccol + coloff;
    *screen_ecol = (पूर्णाङ्क)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (पूर्णाङ्क)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    स्थिर शून्य
popup_hide_for_textprop(win_T *wp)
{
    यदि ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	प्रत्यागम;
    wp->w_popup_flags |= POPF_HIDDEN;
    यदि (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    स्थिर bool
popup_find_prop_above_शीर्ष(
	win_T	    *wp,
	win_T	    *prop_win,
	पूर्णाङ्क	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    यदि (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	प्रत्यागम false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    यदि (first < 1)
	first = 1;
    प्रत्यागम find_prop_in_पंक्तियाँ(prop_win,
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
    स्थिर bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    पूर्णाङ्क	    extra_h, extra_w;
    पूर्णाङ्क	    popup_शीर्ष, popup_bottom, popup_बायाँ, popup_दायाँ;
    पूर्णाङ्क	    total_h, total_w;

    यदि (cw == रिक्त)
	प्रत्यागम false;

    extra_h = popup_शीर्ष_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_शीर्ष = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_बायाँ = wp->w_wincol;
    popup_दायाँ = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    यदि (popup_शीर्ष < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_शीर्ष;
    यदि (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    यदि (popup_बायाँ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_बायाँ;
    यदि (popup_दायाँ > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_दायाँ - (cw->w_wincol + cw->w_width);

    प्रत्यागम wp->w_popup_topoff >= total_h
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
    स्थिर शून्य
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    यदि (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_शीर्ष_content + cl->clip_bot_content;
	यदि (wp->w_height < 0)
	    wp->w_height = 0;
	यदि (cl->clip_शीर्ष_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_शीर्ष_content;
    }
    यदि (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	यदि (cl->clip_बायाँ_content > 0 || cl->clip_दायाँ_content > 0)
	    wp->w_p_wrap = FALSE;
	यदि (cl->clip_दायाँ_content > 0)
	{
	    wp->w_width -= cl->clip_दायाँ_content;
	    यदि (wp->w_width < 0)
		wp->w_width = 0;
	}
	यदि (cl->clip_बायाँ_content > 0)
	{
	    wp->w_leftcol += cl->clip_बायाँ_content;
	    wp->w_wincol += cl->clip_बायाँ_content;
	    wp->w_width -= cl->clip_बायाँ_content;
	    यदि (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    स्थिर शून्य
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    पूर्णाङ्क iw, ih;

    // Visible cell width and height of popup itself (not image!)
    पूर्णाङ्क	visible_width;
    पूर्णाङ्क	visible_height;

    // Crop dimensions of image (in pixels).
    पूर्णाङ्क	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    यदि (wp->w_popup_imagep == रिक्त || wp->w_popup_flags & POPF_HIDDEN)
	प्रत्यागम;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    यदि (wp->w_popup_imagep->img == रिक्त)
	// Just set the bounding box
	गच्छ exit;

    visible_width =
	wp->w_width - clip.clip_बायाँ_content - clip.clip_दायाँ_content;
    visible_height =
	wp->w_height - clip.clip_शीर्ष_content - clip.clip_bot_content;

    यदि (visible_width < 0 || visible_height < 0)
	प्रत्यागम;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_बायाँ_content;
    crop_y = clip.clip_शीर्ष_content;
    crop_width = iw - crop_x - clip.clip_दायाँ_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    यदि (crop_width > visible_width)
	crop_width = visible_width;
    यदि (crop_height > visible_height)
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
    स्थिर शून्य
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    पूर्णाङ्क		wrapped = 0;
    पूर्णाङ्क		maxwidth;
    पूर्णाङ्क		maxwidth_no_scrollbar;
    पूर्णाङ्क		width_with_scrollbar = 0;
    पूर्णाङ्क		used_maxwidth = FALSE;
    पूर्णाङ्क		margin_width = 0;
    पूर्णाङ्क		maxspace;
    पूर्णाङ्क		center_vert = FALSE;
    पूर्णाङ्क		center_hor = FALSE;
    पूर्णाङ्क		allow_adjust_बायाँ = !wp->w_popup_fixed;
    पूर्णाङ्क		शीर्ष_extra = popup_शीर्ष_extra(wp);
    पूर्णाङ्क		दायाँ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    पूर्णाङ्क		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    पूर्णाङ्क		बायाँ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    पूर्णाङ्क		extra_height = शीर्ष_extra + bot_extra;
    पूर्णाङ्क		extra_width = बायाँ_extra + दायाँ_extra;
    पूर्णाङ्क		w_height_before_limit;
    popup_layout_T org_layout;
    पूर्णाङ्क		minwidth, minheight;
    पूर्णाङ्क		maxheight = Rows;
    पूर्णाङ्क		wantline = wp->w_wantline;  // adjusted for textprop
    पूर्णाङ्क		wantcol = wp->w_wantcol;    // adjusted for textprop
    पूर्णाङ्क		use_wantcol = wantcol != 0;
    पूर्णाङ्क		adjust_height_for_शीर्ष_aligned = FALSE;

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
    यदि (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    यदि (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	पूर्णाङ्क	    screen_row;
	पूर्णाङ्क	    screen_scol;
	पूर्णाङ्क	    screen_ccol;
	पूर्णाङ्क	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_शीर्ष = false;
	यदि (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    यदि (popup_find_prop_above_शीर्ष(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_शीर्ष = true;
	    अन्यथा
	    {
		यदि ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    यदि (wp->w_popup_imagep != रिक्त)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    यदि (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		प्रत्यागम;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	यदि (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	यदि (prop_above_शीर्ष)
	    popup_screenpos_above_शीर्ष(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	अन्यथा
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	यदि (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    प्रत्यागम;
	}
	यदि (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	अन्यथा
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	यदि (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	अन्यथा
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    अन्यथा
    {
	// If no line was specified default to vertical centering.
	यदि (wantline == 0)
	    center_vert = TRUE;
	अन्यथा यदि (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	यदि (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    यदि (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    अन्यथा
    {
	यदि (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    यदि (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	यदि (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_पंक्ति_गिनती - 1, 0);
	}

	यदि (!use_wantcol)
	    center_hor = TRUE;
	अन्यथा यदि (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    यदि (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- बायाँ_extra - दायाँ_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- बायाँ_extra - दायाँ_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    यदि (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - बायाँ_extra - दायाँ_extra;
    maxwidth = maxspace;
    यदि (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_बायाँ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    यदि (wp->w_p_nu || wp->w_p_rnu)
	margin_width = संख्या_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    यदि (signcolumn_on(wp))
	margin_width += 2;
#endif
    यदि (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    यदि (wp->w_buffer->b_term != रिक्त)
    {
	यदि (minwidth == 0)
	    minwidth = 20;
	यदि (minheight == 0)
	    minheight = 5;
    }
#endif

    यदि (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    अन्यथा यदि (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    यदि (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	यदि (wp->w_firstline > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	    wp->w_firstline = 0;
	अन्यथा
	    wp->w_topline = wp->w_firstline;
    }
    यदि (wp->w_topline < 1)
	wp->w_topline = 1;
    अन्यथा यदि (wp->w_topline > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	wp->w_topline = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;

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
	पूर्णाङ्क saved_w_width = wp->w_width;

	यदि (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	प्रति (ln = 1; ln <= wp->w_buffer->b_ml.ml_पंक्ति_गिनती; ++ln)
	{
	    पूर्णाङ्क len = linetabsize(wp, ln) + margin_width;

	    यदि (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    यदि (saved_w_width < len)
		saved_w_width = len;
	    यदि (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		भङ्ग;
	}
	wp->w_width = saved_w_width;
    }
    यदि (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
    अन्यथा
	lnum = wp->w_topline;
    यावत् (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
    {
	पूर्णाङ्क len;
	पूर्णाङ्क w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	यदि (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	यदि (len + margin_width > maxwidth
		&& allow_adjust_बायाँ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    पूर्णाङ्क shift_by = len + margin_width - maxwidth;

	    यदि (shift_by > wp->w_wincol)
	    {
		पूर्णाङ्क truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    यदि (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    यदि (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	यदि (wp->w_p_wrap)
	{
	    यावत् (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	यदि (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    यदि (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	यदि (wp->w_firstline < 0)
	    --lnum;
	अन्यथा
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	यदि (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_पंक्ति_गिनती - lnum)
		       + wrapped >= maxheight)
	    भङ्ग;
    }

    यदि (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_पंक्ति_गिनती);
#ifdef FEAT_TERMINAL
    यदि (wp->w_buffer->b_term != रिक्त && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    यदि (wp->w_has_scrollbar)
    {
	++दायाँ_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	यदि (maxwidth + दायाँ_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    यदि (wp->w_popup_title != रिक्त && *wp->w_popup_title != NUL)
    {
	पूर्णाङ्क title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	यदि (minwidth < title_len)
	    minwidth = title_len;
    }

    यदि (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    यदि (wp->w_width > maxwidth)
    {
	यदि (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	यदि (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    पूर्णाङ्क off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, दायाँ_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	अन्यथा
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - दायाँ_extra;
	}
    }
    यदि (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	यदि (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    अन्यथा यदि (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	पूर्णाङ्क leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	यदि (leftoff >= 0)
	    wp->w_wincol = leftoff;
	अन्यथा यदि (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    यदि (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    यदि (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	पूर्णाङ्क want_col = 0;

	// try to show the right border and any scrollbar
	want_col = बायाँ_extra + wp->w_width + दायाँ_extra;
	यदि (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    यदि (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_पंक्ति_गिनती - wp->w_topline
								 + 1 + wrapped;
    यदि (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    यदि (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    यदि (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    यदि (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	यदि (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    अन्यथा यदि (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	यदि ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	अन्यथा यदि (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	अन्यथा
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_शीर्ष_aligned = TRUE;
	}
    }
    अन्यथा यदि (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	पूर्णाङ्क check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	यदि (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    यदि (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    यदि (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	अन्यथा
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_शीर्ष_aligned = TRUE;
	}
    }

    यदि (adjust_height_for_शीर्ष_aligned && wp->w_want_scrollbar
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
	यदि (wp->w_buffer->b_term == रिक्त || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    यदि (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		यदि (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		अन्यथा
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    यदि (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    अन्यथा यदि (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    यदि (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    यदि (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    यदि (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    यदि (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	पूर्णाङ्क avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    यदि (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	यदि (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	यदि (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	यदि (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    पूर्णाङ्क avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    यदि (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    यदि (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	यदि (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    यदि (wp->w_popup_imagep != रिक्त)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	प्रत्यागम;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    यदि (win_valid(wp->w_popup_prop_win))
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
    यदि (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	यदि (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

प्रकारनिर्देश गणना
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
    स्थिर पूर्णाङ्क
popup_is_notification(create_type_T type)
{
    प्रत्यागम type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    स्थिर शून्य
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    पूर्णाङ्क	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    प्रति (lnum = buf->b_ml.ml_पंक्ति_गिनती; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    यदि (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == रिक्त ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    अन्यथा
    {
	list_T *l = text.vval.v_list;

	यदि (l != रिक्त && l->lv_len > 0)
	{
	    यदि (l->lv_first == &range_list_item)
		emsg(_(e_using_संख्या_as_string));
	    अन्यथा यदि (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    अन्यथा
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_पंक्ति_गिनती);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    कुरु {						    \
	यदि (wp != रिक्त)					    \
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
    } यावत् (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    स्थिर पूर्णाङ्क
parse_popup_option(win_T *wp, पूर्णाङ्क is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    पूर्णाङ्क	    border_enabled = FALSE;

    यदि (wp != रिक्त)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    प्रति ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	पूर्णाङ्क	x;

	e = vim_strchr(p, ':');
	यदि (e == रिक्त || e[1] == NUL)
	    प्रत्यागम FAIL;

	p = vim_strchr(e, ',');
	यदि (p == रिक्त)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	यदि (STRNCMP(s, "height:", 7) == 0)
	{
	    यदि (dig != p)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
	    {
		यदि (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	अन्यथा यदि (STRNCMP(s, "width:", 6) == 0)
	{
	    यदि (dig != p)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
	    {
		यदि (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	अन्यथा यदि (STRNCMP(s, "highlight:", 10) == 0)
	{
	    यदि (wp != रिक्त)
	    {
		वर्ण *errmsg;
		पूर्णाङ्क c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		यदि (errmsg == रिक्त)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		अन्यथा
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	अन्यथा यदि (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    यदि (*arg == NUL || *arg == ',')
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
	    {
		प्रति (पूर्णाङ्क i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	अन्यथा यदि (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    पूर्णाङ्क		i;
	    पूर्णाङ्क		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    पूर्णाङ्क		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    यदि (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		यदि (wp != रिक्त)
		{
		    प्रति (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		अनुवर्त;
	    }

	    token = vim_strnsave(arg, token_len);
	    यदि (token == रिक्त)
		प्रत्यागम FAIL;

	    यदि ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		यदि (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		अन्यथा यदि (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		अन्यथा यदि (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		अन्यथा यदि (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		अन्यथा यदि (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		अन्यथा यदि (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    पूर्णाङ्क		out[8];
		    पूर्णाङ्क		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    प्रति (i = 0; i < 8 && !failed; i++)
		    {
			यदि (*q == NUL)
			    failed = TRUE;
			अन्यथा
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    यदि (i < 7)
			    {
				यदि (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    यदि (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			प्रत्यागम FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    अन्यथा
	    {
		vim_free(token);
		प्रत्यागम FAIL;
	    }

	    यदि (wp != रिक्त)
	    {
		प्रति (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	अन्यथा यदि (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    पूर्णाङ्क		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    पूर्णाङ्क		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    यदि (!on && !off)
		प्रत्यागम FAIL;
	    on = on && mouse_has(MOUSE_जोड़ो) && (border_enabled || is_preview);
	    यदि (wp != रिक्त)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	अन्यथा यदि (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    पूर्णाङ्क		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    पूर्णाङ्क		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    यदि (!on && !off)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
	    {
		यदि (on && mouse_has(MOUSE_जोड़ो))
		    wp->w_popup_flags |= POPF_RESIZE;
		अन्यथा
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	अन्यथा यदि (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    पूर्णाङ्क		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    पूर्णाङ्क		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    यदि (!on && !off)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	अन्यथा यदि (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    पूर्णाङ्क		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    पूर्णाङ्क		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    यदि (!menu && !item)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	अन्यथा यदि (STRNCMP(s, "opacity:", 8) == 0)
	{
	    यदि (dig != p || x < 0 || x > 100)
		प्रत्यागम FAIL;
	    यदि (wp != रिक्त)
	    {
		यदि (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		अन्यथा
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	अन्यथा
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    पूर्णाङ्क
parse_previewpopup(win_T *wp)
{
    प्रत्यागम parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    पूर्णाङ्क
parse_completepopup(win_T *wp)
{
    प्रत्यागम parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    शून्य
popup_set_wantpos_cursor(win_T *wp, पूर्णाङ्क width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    यदि (d != रिक्त)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    यदि (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    अन्यथा
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	यदि (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    यदि (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	यदि (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    शून्य
popup_set_wantpos_rowcol(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    स्थिर शून्य
add_border_बायाँ_दायाँ_padding(win_T *wp)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    स्थिर पूर्णाङ्क
popup_terminal_exists(शून्य)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	यदि (wp->w_buffer->b_term != रिक्त)
	    प्रत्यागम TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    यदि (wp->w_buffer->b_term != रिक्त)
		प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    शून्य
popup_redraw_all(शून्य)
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
    पूर्णाङ्क
popup_need_redraw(शून्य)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	यदि ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    प्रत्यागम TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Set the color for a notification window.
 */
    स्थिर शून्य
popup_update_color(win_T *wp, create_type_T type)
{
    वर्ण    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    वर्ण    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    यदि (errmsg == रिक्त)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    अन्यथा
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    स्थिर win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = रिक्त;
    पूर्णाङ्क		tabnr = 0;
    पूर्णाङ्क		new_buffer;
    buf_T	*buf = रिक्त;
    dict_T	*d = रिक्त;
    पूर्णाङ्क		i;

    यदि (check_secure())
	प्रत्यागम रिक्त;

    यदि (argvars != रिक्त)
    {
	यदि (in_vim9script()
		&& (check_for_string_or_संख्या_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    प्रत्यागम रिक्त;

	// Check that arguments look OK.
	यदि (argvars[0].v_type == VAR_संख्या)
	{
	    buf = buflist_findnr(argvars[0].vval.v_संख्या);
	    यदि (buf == रिक्त)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_संख्या);
		प्रत्यागम रिक्त;
	    }
#ifdef FEAT_TERMINAL
	    यदि (buf->b_term != रिक्त && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		प्रत्यागम रिक्त;
	    }
#endif
	}
	अन्यथा यदि (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != रिक्त)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != रिक्त))
	{
	    emsg(_(e_buffer_संख्या_text_or_list_required));
	    प्रत्यागम रिक्त;
	}
	यदि (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    प्रत्यागम रिक्त;
	d = argvars[1].vval.v_dict;
    }

    यदि (d != रिक्त)
    {
	यदि (dict_has_कुंजी(d, "tabpage"))
	    tabnr = (पूर्णाङ्क)dict_get_संख्या(d, "tabpage");
	अन्यथा यदि (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	अन्यथा
	    tabnr = 0;
	यदि (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    यदि (tp == रिक्त)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		प्रत्यागम रिक्त;
	    }
	}
    }
    अन्यथा यदि (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    यदि (buf != रिक्त && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	प्रत्यागम रिक्त;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    यदि (wp == रिक्त)
	प्रत्यागम रिक्त;
    यदि (rettv != रिक्त)
	rettv->vval.v_संख्या = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    यदि (buf != रिक्त)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    अन्यथा
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(रिक्त, रिक्त, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	यदि (buf == रिक्त)
	{
	    win_free_popup(wp);
	    प्रत्यागम रिक्त;
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

    यदि (tp != रिक्त)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    अन्यथा यदि (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    अन्यथा // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	यदि (first_popupwin == रिक्त)
	    first_popupwin = wp;
	अन्यथा
	{
	    यावत् (prev->w_next != रिक्त)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    यदि (new_buffer && argvars != रिक्त)
	popup_set_buffer_text(buf, argvars[0]);

    यदि (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    यदि (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_मूल्यावली(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    यदि (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	यदि (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_मूल्यावली(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    यदि (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	पूर्णाङ्क	height = buf->b_ml.ml_पंक्ति_गिनती + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	प्रति (twp = first_popupwin; twp != रिक्त; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    यदि (twp != wp
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
	यदि (wp->w_wantline + height > Rows)
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
	प्रति (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    यदि (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_बायाँ_दायाँ_padding(wp);
    }

    यदि (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	यदि (callback.cb_name != रिक्त)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    प्रति (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    प्रति (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    यदि (type == TYPE_PREVIEW)
    {
	यदि (mouse_has(MOUSE_जोड़ो))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	प्रति (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    यदि (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	यदि (mouse_has(MOUSE_जोड़ो))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_बायाँ_दायाँ_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    यदि (d != रिक्त)
    {
	// Deal with options.
	यदि (apply_options(wp, d, TRUE) == FAIL)
	{
	    (शून्य)popup_close(wp->w_id, FALSE);
	    प्रत्यागम रिक्त;
	}
    }

#ifdef FEAT_TIMERS
    यदि (popup_is_notification(type) && wp->w_popup_timer == रिक्त)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    यदि (buf->b_term != रिक्त)
	win_enter(wp, FALSE);
#endif

    प्रत्यागम wp;
}

/*
 * popup_clear()
 */
    शून्य
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क force = FALSE;

    यदि (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
	force = (पूर्णाङ्क)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    शून्य
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    शून्य
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    शून्य
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    स्थिर शून्य
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_संख्या;
    argv[0].vval.v_संख्या = (varnumber_T)wp->w_id;

    यदि (result != रिक्त && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    अन्यथा
    {
	argv[1].v_type = VAR_संख्या;
	argv[1].vval.v_संख्या = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    यदि (result != रिक्त)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    स्थिर शून्य
back_to_prevwin(win_T *wp)
{
    यदि (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    अन्यथा
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    स्थिर शून्य
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    पूर्णाङ्क id;

    यदि (!win_valid(wp))
       प्रत्यागम;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    यदि (wp == curwin && curbuf->b_term != रिक्त)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    यदि (owp != curwin && owp->w_buffer->b_term != रिक्त)
		भङ्ग;
	यदि (owp != रिक्त)
	    win_enter(owp, FALSE);
	अन्यथा
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		यदि (owp != curwin && owp->w_buffer->b_term != रिक्त)
		    भङ्ग;
	    यदि (owp != रिक्त)
		win_enter(owp, FALSE);
	    अन्यथा
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    यदि (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	प्रत्यागम;
    }

    CHECK_CURBUF;
    यदि (wp->w_close_cb.cb_name != रिक्त)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    शून्य
popup_close_with_retval(win_T *wp, पूर्णाङ्क retval)
{
    typval_T res;

    res.v_type = VAR_संख्या;
    res.vval.v_संख्या = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    शून्य
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    स्थिर शून्य
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    यदि (wp != mouse_wp
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
    शून्य
popup_handle_mouse_moved(शून्य)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    पूर्णाङ्क	    row = mouse_row;
    पूर्णाङ्क	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    प्रति (wp = first_popupwin; wp != रिक्त; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    प्रति (wp = curtab->tp_first_popupwin; wp != रिक्त; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    स्थिर शून्य
filter_handle_drag(win_T *wp, पूर्णाङ्क c, typval_T *rettv)
{
    पूर्णाङ्क	row = mouse_row;
    पूर्णाङ्क	col = mouse_col;

    यदि ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_कुंजी(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_संख्या = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    शून्य
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		id;
    win_T	*wp;
    char_u	*कुंजी;
    typval_T	res;
    पूर्णाङ्क		c;
    linenr_T	old_lnum;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    id = tv_get_संख्या(&argvars[0]);
    wp = win_id2wp(id);
    कुंजी = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    यदि (wp == रिक्त)
	प्रत्यागम;

    c = *कुंजी;
    यदि (c == K_SPECIAL && कुंजी[1] != NUL)
	c = TO_SPECIAL(कुंजी[1], कुंजी[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_TRUE;
    res.v_type = VAR_संख्या;

    old_lnum = wp->w_cursor.lnum;
    यदि (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	यदि (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	अन्यथा
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
    }
    यदि (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	यदि (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	    ++wp->w_cursor.lnum;
	अन्यथा
	    wp->w_cursor.lnum = 1;
    }
    यदि (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	प्रत्यागम;
    }

    यदि (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_संख्या = -1;
	popup_close_and_callback(wp, &res);
	प्रत्यागम;
    }
    यदि (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_संख्या = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	प्रत्यागम;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    शून्य
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		id;
    win_T	*wp;
    char_u	*कुंजी;
    typval_T	res;
    पूर्णाङ्क		c;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    id = tv_get_संख्या(&argvars[0]);
    wp = win_id2wp(id);
    कुंजी = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    यदि (wp == रिक्त)
	प्रत्यागम;

    c = *कुंजी;
    यदि (c == CAR && need_wait_return)
	प्रत्यागम;
    यदि (c == K_SPECIAL && कुंजी[1] != NUL)
	c = TO_SPECIAL(कुंजी[1], कुंजी[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_TRUE;

    यदि (c == 'y' || c == 'Y')
	res.vval.v_संख्या = 1;
    अन्यथा यदि (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_संख्या = 0;
    अन्यथा
    {
	filter_handle_drag(wp, c, rettv);
	प्रत्यागम;
    }

    // Invoke callback
    res.v_type = VAR_संख्या;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    शून्य
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    शून्य
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    शून्य
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    स्थिर win_T *
find_popup_win(पूर्णाङ्क id)
{
    win_T *wp = win_id2wp(id);

    यदि (wp != रिक्त && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम wp;
}

/*
 * popup_close({id})
 */
    शून्य
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		id;
    win_T	*wp;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    यदि (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == रिक्त &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    wp = find_popup_win(id);
    यदि (wp != रिक्त)
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
    स्थिर शून्य
popup_clear_mask_for(win_T *wp)
{
    पूर्णाङ्क r, c;
    पूर्णाङ्क row_start, col_start, row_end, col_end;

    यदि (popup_mask == रिक्त || !popup_visible)
	प्रत्यागम;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (पूर्णाङ्क)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (पूर्णाङ्क)screen_Columns);

    प्रति (r = row_start; r < row_end; ++r)
	प्रति (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    शून्य
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    पूर्णाङ्क			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    यदि (error_if_term_popup_window())
	प्रत्यागम;
#endif
    यदि ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	प्रत्यागम;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    यदि (wp->w_popup_imagep != रिक्त)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    यदि (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    यदि (was_visible)
	popup_clear_mask_for(wp);

    यदि (old_area.active)
	popup_redraw_exposed_area(&old_area);
    अन्यथा
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    यदि (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    शून्य
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		id;
    win_T	*wp;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    शून्य
popup_show(win_T *wp)
{
    bool popup_active;

    यदि ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	प्रत्यागम;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    यदि (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_पंक्तियाँ_valid = 0;
	यदि (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    अन्यथा
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    शून्य
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		id;
    win_T	*wp;

    rettv->v_type = VAR_संख्या;
    rettv->vval.v_संख्या = -1;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    यदि (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_संख्या = 0;
}

/*
 * popup_settext({id}, {text})
 */
    शून्य
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		id;
    win_T	*wp;
    popup_area_T	old_area;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(&argvars[0]);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;

    popup_save_area(wp, &old_area);

    यदि (check_for_string_or_list_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_पंक्तियाँ_valid = 0;
    यदि (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    यदि (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    शून्य
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_FALSE;

    यदि (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(&argvars[0]);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;

    buf = tv_get_buf_from_arg(&argvars[1]);

    यदि (buf == रिक्त)
	प्रत्यागम;
#ifdef FEAT_TERMINAL
    यदि (buf->b_term != रिक्त && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	प्रत्यागम;
    }
#endif

    यदि (wp->w_buffer != buf)
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

	यदि (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_संख्या = VVAL_TRUE;
}

    स्थिर शून्य
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    पूर्णाङ्क			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    यदि (wp->w_popup_imagep != रिक्त)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    यदि (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    यदि (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    यदि (wp == message_win)
	message_win = रिक्त;
#endif

    यदि (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    यदि (!old_area.active)
	popup_mask_refresh = TRUE;
}

    स्थिर शून्य
error_for_popup_window(शून्य)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    पूर्णाङ्क
error_if_popup_window(पूर्णाङ्क also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    यदि (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == रिक्त)
#endif
	    )
    {
	error_for_popup_window();
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    पूर्णाङ्क
popup_close(पूर्णाङ्क id, पूर्णाङ्क force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = रिक्त;

    // go through global popups
    प्रति (wp = first_popupwin; wp != रिक्त; prev = wp, wp = wp->w_next)
	यदि (wp->w_id == id)
	{
	    यदि (wp == curwin)
	    {
		यदि (!force)
		{
		    error_for_popup_window();
		    प्रत्यागम FAIL;
		}
		back_to_prevwin(wp);
	    }
	    यदि (prev == रिक्त)
		first_popupwin = wp->w_next;
	    अन्यथा
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    प्रत्यागम OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	यदि (popup_close_tabpage(tp, id, force) == OK)
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    पूर्णाङ्क
popup_close_tabpage(tabpage_T *tp, पूर्णाङ्क id, पूर्णाङ्क force)
{
    win_T	*wp;
    win_T	**मूल = &tp->tp_first_popupwin;
    win_T	*prev = रिक्त;

    प्रति (wp = *मूल; wp != रिक्त; prev = wp, wp = wp->w_next)
	यदि (wp->w_id == id)
	{
	    यदि (wp == curwin)
	    {
		यदि (!force)
		{
		    error_for_popup_window();
		    प्रत्यागम FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    यदि (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    यदि (prev == रिक्त)
		*मूल = wp->w_next;
	    अन्यथा
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    प्रत्यागम OK;
	}
    प्रत्यागम FAIL;
}

    शून्य
close_all_popups(पूर्णाङ्क force)
{
    यदि (!force && ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;
    यावत् (first_popupwin != रिक्त)
	यदि (popup_close(first_popupwin->w_id, force) == FAIL)
	    प्रत्यागम;
    यावत् (curtab->tp_first_popupwin != रिक्त)
	यदि (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    प्रत्यागम;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    स्थिर शून्य
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
    स्थिर शून्य
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    पूर्णाङ्क i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    प्रति (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    स्थिर bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    पूर्णाङ्क i;

    यदि (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	प्रत्यागम true;
    प्रति (i = 0; i < 4; i++)
	यदि (style->border_highlight[i] != wp->w_border_highlight[i])
	    प्रत्यागम true;
    प्रत्यागम false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    स्थिर शून्य
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
    स्थिर bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    प्रत्यागम layout->winrow != wp->w_winrow
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
    स्थिर bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    प्रत्यागम area->winrow != wp->w_winrow
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
    स्थिर शून्य
mark_overlapped_opacity_popup(win_T *wp, पूर्णाङ्क area_शीर्ष, पूर्णाङ्क area_bot,
	पूर्णाङ्क area_बायाँ, पूर्णाङ्क area_दायाँ, पूर्णाङ्क zindex)
{
    यदि ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_शीर्ष
	    || wp->w_wincol >= area_दायाँ
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_बायाँ)
	प्रत्यागम;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_पंक्तियाँ_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    स्थिर शून्य
redraw_overlapped_opacity_popups(पूर्णाङ्क winrow, पूर्णाङ्क wincol, पूर्णाङ्क height, पूर्णाङ्क width,
	पूर्णाङ्क leftoff, पूर्णाङ्क zindex)
{
    win_T	*wp;
    पूर्णाङ्क		area_शीर्ष = winrow;
    पूर्णाङ्क		area_bot = winrow + height;
    पूर्णाङ्क		area_बायाँ = wincol;
    पूर्णाङ्क		area_दायाँ = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_शीर्ष, area_bot, area_बायाँ,
		area_दायाँ, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_शीर्ष, area_bot, area_बायाँ,
		area_दायाँ, zindex);

    यदि (must_redraw < UPD_VALID)
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
    स्थिर शून्य
refill_pum_bg_row_from_buffer(पूर्णाङ्क r, पूर्णाङ्क बायाँ, पूर्णाङ्क दायाँ)
{
    पूर्णाङ्क		पंक्ति_cp = r;
    पूर्णाङ्क		col_cp = बायाँ;
    win_T	*wp;
    linenr_T	lnum;
    पूर्णाङ्क		row_for_lnum;
    पूर्णाङ्क		col_for_lnum = 0;
    char_u	*पंक्ति;
    char_u	*p;
    पूर्णाङ्क		win_text_col;
    पूर्णाङ्क		screen_col;
    पूर्णाङ्क		soff_base = (r - pum_bg_शीर्ष) * pum_bg_cols;
    पूर्णाङ्क		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    प्रति (c = बायाँ; c < दायाँ; ++c)
    {
	pum_bg_पंक्तियाँ[soff_base + c] = ' ';
	यदि (pum_bg_attrs != रिक्त)
	    pum_bg_attrs[soff_base + c] = 0;
	यदि (enc_utf8 && pum_bg_पंक्तियाँ_UC != रिक्त)
	    pum_bg_पंक्तियाँ_UC[soff_base + c] = 0;
	यदि (enc_utf8)
	{
	    पूर्णाङ्क k;
	    प्रति (k = 0; k < MAX_MCO; ++k)
		यदि (pum_bg_पंक्तियाँ_C[k] != रिक्त)
		    pum_bg_पंक्तियाँ_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&पंक्ति_cp, &col_cp, IGNORE_POPUP);
    यदि (wp == रिक्त || पंक्ति_cp < 0 || पंक्ति_cp >= wp->w_height)
	प्रत्यागम;
    यदि (wp->w_buffer == रिक्त || wp->w_buffer->b_ml.ml_mfp == रिक्त)
	प्रत्यागम;

    // Compute the buffer line for this screen row.
    row_for_lnum = पंक्ति_cp;
    यदि (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, रिक्त))
	प्रत्यागम;	// past end of buffer
    यदि (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	प्रत्यागम;

    पंक्ति = ml_get_buf(wp->w_buffer, lnum, FALSE);
    यदि (पंक्ति == रिक्त)
	प्रत्यागम;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    यदि (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = पंक्ति;
    यावत् (*p != NUL && screen_col < दायाँ)
    {
	पूर्णाङ्क char_cells;
	पूर्णाङ्क byte_गिनती;
	पूर्णाङ्क soff = soff_base + screen_col;
	पूर्णाङ्क in_range = (screen_col >= बायाँ && screen_col < दायाँ);

	यदि (*p == '\t')
	{
	    पूर्णाङ्क ts = (पूर्णाङ्क)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_गिनती = 1;
	    यदि (in_range)
	    {
		pum_bg_पंक्तियाँ[soff] = ' ';
		यदि (enc_utf8 && pum_bg_पंक्तियाँ_UC != रिक्त)
		    pum_bg_पंक्तियाँ_UC[soff] = 0;
	    }
	}
	अन्यथा यदि (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_गिनती = mb_ptr2len(p);
	    यदि (in_range)
	    {
		pum_bg_पंक्तियाँ[soff] = *p;
		यदि (enc_utf8 && pum_bg_पंक्तियाँ_UC != रिक्त)
		    pum_bg_पंक्तियाँ_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	अन्यथा
	{
	    char_cells = 1;
	    byte_गिनती = 1;
	    यदि (in_range)
	    {
		pum_bg_पंक्तियाँ[soff] = (*p < 0x20) ? ' ' : *p;
		यदि (enc_utf8 && pum_bg_पंक्तियाँ_UC != रिक्त)
		    pum_bg_पंक्तियाँ_UC[soff] = 0;
	    }
	}
	यदि (in_range && pum_bg_attrs != रिक्त)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_गिनती;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    स्थिर शून्य
popup_redraw_exposed_area(popup_area_T *area)
{
    यदि (!area->active)
	प्रत्यागम;

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
    यदि (pum_bg_पंक्तियाँ != रिक्त
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_शीर्ष)
    {
	पूर्णाङ्क शीर्ष = MAX(area->winrow, pum_bg_शीर्ष);
	पूर्णाङ्क bot = MIN(area->winrow + area->height, pum_bg_bot);
	पूर्णाङ्क बायाँ = MAX(area->wincol, 0);
	पूर्णाङ्क दायाँ = MIN(area->wincol + area->width, pum_bg_cols);
	पूर्णाङ्क r;

	प्रति (r = शीर्ष; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, बायाँ, दायाँ);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    स्थिर शून्य
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->पंक्तियाँ);
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
    स्थिर शून्य
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    यदि (screen_opacity_popup == रिक्त
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	प्रत्यागम;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    यदि (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->पंक्तियाँ = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(पूर्णाङ्क,
				  saved_screen->rows * saved_screen->cols);
    यदि (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    यदि (saved_screen->पंक्तियाँ == रिक्त || saved_screen->attrs == रिक्त)
	प्रत्यागम;

    प्रति (पूर्णाङ्क r = 0; r < saved_screen->rows; r++)
    {
	पूर्णाङ्क screen_row = saved_screen->start_row + r;

	यदि (screen_row >= 0 && screen_row < screen_Rows)
	    प्रति (पूर्णाङ्क c = 0; c < saved_screen->cols; c++)
	    {
		पूर्णाङ्क screen_col = saved_screen->start_col + c;

		यदि (screen_col >= 0 && screen_col < screen_Columns)
		{
		    पूर्णाङ्क off = पंक्ति_Offset[screen_row] + screen_col;
		    पूर्णाङ्क save_off = r * saved_screen->cols + c;

		    saved_screen->पंक्तियाँ[save_off] = Screen_पंक्तियाँ[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    यदि (enc_utf8 && saved_screen->linesuc != रिक्त)
			saved_screen->linesuc[save_off] = Screen_पंक्तियाँ_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    स्थिर शून्य
redraw_under_popup_area(पूर्णाङ्क winrow, पूर्णाङ्क wincol, पूर्णाङ्क height, पूर्णाङ्क width, पूर्णाङ्क leftoff)
{
    पूर्णाङ्क	    r;

    प्रति (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	पूर्णाङ्क	    c;
	win_T	    *prev_twp = रिक्त;

	यदि (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    अनुवर्त;
	}

	प्रति (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    पूर्णाङ्क	    पंक्ति_cp = r;
	    पूर्णाङ्क	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&पंक्ति_cp, &col_cp, IGNORE_POPUP);
	    यदि (twp != रिक्त && twp != prev_twp)
	    {
		prev_twp = twp;
		यदि (पंक्ति_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (शून्य)mouse_comp_pos(twp, &पंक्ति_cp, &col_cp, &lnum, रिक्त);
		    redrawWinline(twp, lnum);
		}
		अन्यथा यदि (पंक्ति_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    शून्य
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    पूर्णाङ्क		id;
    win_T	*wp;
    popup_area_T	old_area;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;  // invalid {id}

    यदि (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	प्रत्यागम;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    यदि (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    यदि (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    शून्य
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    पूर्णाङ्क		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    पूर्णाङ्क		need_redraw = FALSE;
    पूर्णाङ्क		need_reposition = FALSE;

    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;  // invalid {id}

    यदि (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	प्रत्यागम;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (शून्य)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    यदि (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    यदि (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    अन्यथा यदि (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    यदि (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    यदि (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    अन्यथा यदि (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_पंक्तियाँ_valid = 0;
	यदि (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    यदि (old_style.blend != wp->w_popup_blend)
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

    यदि (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    शून्य
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    पूर्णाङ्क		id;
    win_T	*wp;
    पूर्णाङ्क		शीर्ष_extra;
    पूर्णाङ्क		बायाँ_extra;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;  // invalid {id}
    शीर्ष_extra = popup_शीर्ष_extra(wp);
    बायाँ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_संख्या(dict, "line", wp->w_winrow + 1);
    dict_add_संख्या(dict, "col", wp->w_wincol + 1);
    dict_add_संख्या(dict, "width", wp->w_width + बायाँ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_संख्या(dict, "height", wp->w_height + शीर्ष_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_संख्या(dict, "core_line", wp->w_winrow + 1 + शीर्ष_extra);
    dict_add_संख्या(dict, "core_col", wp->w_wincol + 1 + बायाँ_extra);
    dict_add_संख्या(dict, "core_width", wp->w_width);
    dict_add_संख्या(dict, "core_height", wp->w_height);

    dict_add_संख्या(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_संख्या(dict, "firstline", wp->w_topline);
    dict_add_संख्या(dict, "lastline", wp->w_botline - 1);
    dict_add_संख्या(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    शून्य
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;
    FOR_ALL_POPUPWINS(wp)
	list_append_संख्या(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_संख्या(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    शून्य
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		col;
    win_T	*wp;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    row = tv_get_संख्या(&argvars[0]) - 1;
    col = tv_get_संख्या(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    यदि (wp != रिक्त && WIN_IS_POPUP(wp))
	rettv->vval.v_संख्या = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    स्थिर शून्य
get_padding_border(dict_T *dict, पूर्णाङ्क *array, वर्ण *name)
{
    list_T  *list;
    पूर्णाङ्क	    i;

    यदि (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	प्रत्यागम;

    list = list_alloc();
    यदि (list == रिक्त)
	प्रत्यागम;

    यदि (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	प्रत्यागम;
    }
    यदि (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	प्रति (i = 0; i < 4; ++i)
	    list_append_संख्या(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    स्थिर शून्य
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < 4; ++i)
	यदि (wp->w_border_highlight[i] != रिक्त)
	    भङ्ग;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    यदि (i == 4 && !wp->w_border_highlight_isset)
	प्रत्यागम;

    list = list_alloc();
    यदि (list == रिक्त)
	प्रत्यागम;

    यदि (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	प्रत्यागम;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    यदि (i == 4)
	प्रत्यागम;
    प्रति (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    स्थिर शून्य
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    पूर्णाङ्क	    i;
    char_u  buf[NUMBUFLEN];
    पूर्णाङ्क	    len;

    प्रति (i = 0; i < 8; ++i)
	यदि (wp->w_border_char[i] != 0)
	    भङ्ग;
    यदि (i == 8)
	प्रत्यागम;

    list = list_alloc();
    यदि (list == रिक्त)
	प्रत्यागम;

    यदि (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	प्रत्यागम;
    }
    प्रति (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    स्थिर शून्य
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    यदि (list != रिक्त)
    {
	यदि (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	अन्यथा
	{
	    list_append_संख्या(list, wp->w_popup_lnum);
	    list_append_संख्या(list, wp->w_popup_mincol);
	    list_append_संख्या(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    यदि (list == रिक्त)
	प्रत्यागम;

    यदि (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	प्रत्यागम;
    }
    list_append_संख्या(list, wp->w_popup_mouse_row);
    list_append_संख्या(list, wp->w_popup_mouse_mincol);
    list_append_संख्या(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    शून्य
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    पूर्णाङ्क		id;
    win_T	*wp;
    tabpage_T	*tp;
    पूर्णाङ्क		i;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    id = (पूर्णाङ्क)tv_get_संख्या(argvars);
    wp = find_popup_win(id);
    यदि (wp == रिक्त)
	प्रत्यागम;

    dict = rettv->vval.v_dict;
    dict_add_संख्या(dict, "line", wp->w_wantline);
    dict_add_संख्या(dict, "col", wp->w_wantcol);
    dict_add_संख्या(dict, "minwidth", wp->w_minwidth);
    dict_add_संख्या(dict, "minheight", wp->w_minheight);
    dict_add_संख्या(dict, "maxheight", wp->w_maxheight);
    dict_add_संख्या(dict, "maxwidth", wp->w_maxwidth);
    dict_add_संख्या(dict, "firstline", wp->w_firstline);
    dict_add_संख्या(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_संख्या(dict, "zindex", wp->w_zindex);
    dict_add_संख्या(dict, "fixed", wp->w_popup_fixed);
    यदि (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	यदि (pt != रिक्त)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_संख्या(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_संख्या(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    यदि (wp->w_popup_imagep != रिक्त && wp->w_popup_imagep->img != रिक्त)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	यदि (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_संख्या(dict, "wrap", wp->w_p_wrap);
    dict_add_संख्या(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_संख्या(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_संख्या(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_संख्या(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_संख्या(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_संख्या(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_संख्या(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_संख्या(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    यदि (wp->w_scrollbar_highlight != रिक्त)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    यदि (wp->w_thumb_highlight != रिक्त)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    यदि (twp->w_id == id)
		भङ्ग;
	यदि (twp != रिक्त)
	    भङ्ग;
	++i;
    }
    यदि (tp == रिक्त)
	i = -1;  // must be global
    अन्यथा यदि (tp == curtab)
	i = 0;
    dict_add_संख्या(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    यदि (wp->w_filter_cb.cb_name != रिक्त)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    यदि (wp->w_close_cb.cb_name != रिक्त)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(poppos_entries); ++i)
	यदि (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(पूर्णाङ्क)poppos_entries[i].pp_name.length);
	    भङ्ग;
	}

    यदि (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    अन्यथा यदि (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    अन्यथा
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#यदि defined(FEAT_TIMERS)
    dict_add_संख्या(dict, "time", wp->w_popup_timer != रिक्त
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#यदि defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    पूर्णाङ्क
error_if_term_popup_window(शून्य)
{
    यदि (WIN_IS_POPUP(curwin) && curbuf->b_term != रिक्त
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    शून्य
popup_reset_handled(पूर्णाङ्क handled_flag)
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
find_next_popup(पूर्णाङ्क lowest, पूर्णाङ्क handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    पूर्णाङ्क	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = रिक्त;
    FOR_ALL_POPUPWINS(wp)
	यदि ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    यदि (found_wp != रिक्त)
	found_wp->w_popup_handled |= handled_flag;
    प्रत्यागम found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    स्थिर पूर्णाङ्क
invoke_popup_filter(win_T *wp, पूर्णाङ्क c)
{
    पूर्णाङ्क		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    पूर्णाङ्क		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    यदि (c == Ctrl_C)
    {
	पूर्णाङ्क save_got_int = got_int;
	पूर्णाङ्क was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	यदि (was_curwin && wp == curwin)
	    प्रत्यागम -1;
	प्रत्यागम TRUE;
    }

    argv[0].v_type = VAR_संख्या;
    argv[0].vval.v_संख्या = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    यदि (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    अन्यथा
    {
	यदि (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	यदि (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    यदि (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	अन्यथा
	{
	    यदि (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    प्रत्यागम res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    पूर्णाङ्क
popup_do_filter(पूर्णाङ्क c)
{
    स्थिर पूर्णाङ्क	recursive = FALSE;
    पूर्णाङ्क		res = FALSE;
    win_T	*wp;
    पूर्णाङ्क		save_कुंजी_Typed = कुंजी_Typed;
    पूर्णाङ्क		state;
    पूर्णाङ्क		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    यदि (popup_is_popup(curwin) && curbuf->b_term != रिक्त)
	प्रत्यागम FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    यदि (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	प्रत्यागम FALSE;

    यदि (recursive)
	प्रत्यागम FALSE;
    recursive = TRUE;

    यदि (c == K_LEFTMOUSE)
    {
	पूर्णाङ्क row = mouse_row;
	पूर्णाङ्क col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	यदि (wp != रिक्त && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    यावत् (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != रिक्त)
	यदि (wp->w_filter_cb.cb_name != रिक्त
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    यदि (must_redraw > was_must_redraw)
    {
	पूर्णाङ्क save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    कुंजी_Typed = save_कुंजी_Typed;

    // When interrupted return FALSE to avoid looping.
    प्रत्यागम res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    पूर्णाङ्क
popup_no_mapping(शून्य)
{
    पूर्णाङ्क	    round;
    win_T   *wp;

    प्रति (round = 1; round <= 2; ++round)
	प्रति (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != रिक्त; wp = wp->w_next)
	    यदि (wp->w_filter_cb.cb_name != रिक्त
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    शून्य
popup_check_cursor_pos(शून्य)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    यावत् ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != रिक्त)
	यदि (wp->w_popup_curwin != रिक्त
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    स्थिर शून्य
popup_update_mask(win_T *wp, पूर्णाङ्क width, पूर्णाङ्क height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    पूर्णाङ्क		row, col;

    यदि (wp->w_popup_mask == रिक्त || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	प्रत्यागम;
    }
    यदि (wp->w_popup_mask_cells != रिक्त
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	प्रत्यागम;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    यदि (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = रिक्त;
	प्रत्यागम;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    यदि (wp->w_popup_mask_cells == रिक्त)
	प्रत्यागम;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_वस्तुएँ(wp->w_popup_mask, lio)
    {
	पूर्णाङ्क cols, cole;
	पूर्णाङ्क पंक्तियाँ, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_संख्या(&li->li_tv);
	यदि (cols < 0)
	    cols = width + cols + 1;
	यदि (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_संख्या(&li->li_tv);
	यदि (cole < 0)
	    cole = width + cole + 1;
	यदि (cole > width)
	    cole = width;
	li = li->li_next;
	पंक्तियाँ = tv_get_संख्या(&li->li_tv);
	यदि (पंक्तियाँ < 0)
	    पंक्तियाँ = height + पंक्तियाँ + 1;
	यदि (पंक्तियाँ <= 0)
	    पंक्तियाँ = 1;
	li = li->li_next;
	linee = tv_get_संख्या(&li->li_tv);
	यदि (linee < 0)
	    linee = height + linee + 1;
	यदि (linee > height)
	    linee = height;

	प्रति (row = पंक्तियाँ - 1; row < linee; ++row)
	    प्रति (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    स्थिर पूर्णाङ्क
popup_masked(win_T *wp, पूर्णाङ्क width, पूर्णाङ्क height, पूर्णाङ्क screencol, पूर्णाङ्क screenline)
{
    पूर्णाङ्क col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    पूर्णाङ्क पंक्ति = screenline - wp->w_winrow;

    प्रत्यागम col >= 0 && col < width
	    && पंक्ति >= 0 && पंक्ति < height
	    && wp->w_popup_mask_cells[पंक्ति * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    स्थिर शून्य
update_popup_transparent(win_T *wp, पूर्णाङ्क val)
{
    यदि (wp->w_popup_mask == रिक्त)
	प्रत्यागम;

    पूर्णाङ्क		width = popup_width(wp);
    पूर्णाङ्क		height = popup_height(wp);
    listitem_T	*lio, *li;
    पूर्णाङ्क		cols, cole;
    पूर्णाङ्क		पंक्तियाँ, linee;
    पूर्णाङ्क		col, पंक्ति;

    FOR_ALL_LIST_वस्तुएँ(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_संख्या(&li->li_tv);
	यदि (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_संख्या(&li->li_tv);
	यदि (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	पंक्तियाँ = tv_get_संख्या(&li->li_tv);
	यदि (पंक्तियाँ < 0)
	    पंक्तियाँ = height + पंक्तियाँ + 1;
	li = li->li_next;
	linee = tv_get_संख्या(&li->li_tv);
	यदि (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	यदि (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--पंक्तियाँ;
	यदि (पंक्तियाँ < 0)
	    पंक्तियाँ = 0;
	प्रति (पंक्ति = पंक्तियाँ; पंक्ति < linee
		&& पंक्ति + wp->w_winrow < screen_Rows; ++पंक्ति)
	    प्रति (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(पंक्ति + wp->w_winrow) * screen_Columns
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
    स्थिर पूर्णाङ्क
check_popup_unhidden(win_T *wp)
{
    यदि (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	यदि ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    प्रत्यागम FALSE;
	यदि (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	अन्यथा यदि (popup_find_prop_above_शीर्ष(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	यदि (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    स्थिर पूर्णाङ्क
popup_need_position_adjust(win_T *wp)
{
    यदि (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	प्रत्यागम TRUE;
    यदि (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	प्रत्यागम TRUE;

    // May need to adjust the width if the cursor moved.
    प्रत्यागम wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
स्थिर short *opacity_zindex = रिक्त;
स्थिर पूर्णाङ्क    opacity_zindex_rows = 0;
स्थिर पूर्णाङ्क    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    स्थिर शून्य
popup_mark_opacity_zindex(win_T *wp)
{
    पूर्णाङ्क	    width;
    पूर्णाङ्क	    height;
    पूर्णाङ्क	    r, c;

    यदि (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	प्रत्यागम;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    प्रति (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	प्रति (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    पूर्णाङ्क off = r * screen_Columns + c;
	    यदि (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    स्थिर शून्य
redraw_win_under_opacity_popup(win_T *wp)
{
    पूर्णाङ्क	    height;
    पूर्णाङ्क	    width;
    पूर्णाङ्क	    r;

    यदि (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	प्रत्यागम;

    height = popup_height(wp);
    width = popup_width(wp);
    प्रति (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	पूर्णाङ्क	    col;
	win_T	    *prev_twp = रिक्त;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	प्रति (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    पूर्णाङ्क	    पंक्ति_cp = r;
	    पूर्णाङ्क	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&पंक्ति_cp, &col_cp, IGNORE_POPUP);
	    यदि (twp != रिक्त && twp != prev_twp)
	    {
		prev_twp = twp;
		यदि (पंक्ति_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    यदि (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    अन्यथा
#endif
		    {
			(शून्य)mouse_comp_pos(twp, &पंक्ति_cp, &col_cp, &lnum,
									रिक्त);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		अन्यथा यदि (पंक्ति_cp == twp->w_height)
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
    पूर्णाङ्क
popup_is_under_opacity(पूर्णाङ्क row, पूर्णाङ्क col)
{
    यदि (opacity_zindex == रिक्त
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	प्रत्यागम FALSE;
    प्रत्यागम opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    पूर्णाङ्क
popup_is_over_opacity(पूर्णाङ्क row, पूर्णाङ्क col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	यदि ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    प्रत्यागम TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    पूर्णाङ्क
popup_is_under_opacity_range(पूर्णाङ्क row, पूर्णाङ्क start_col, पूर्णाङ्क end_col)
{
    पूर्णाङ्क col;

    यदि (opacity_zindex == रिक्त
	    || row < 0 || row >= opacity_zindex_rows)
	प्रत्यागम FALSE;
    प्रति (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	यदि (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    शून्य
may_update_popup_mask(पूर्णाङ्क type)
{
    win_T	*wp;
    short	*mask;
    पूर्णाङ्क		पंक्ति, col;
    पूर्णाङ्क		redraw_all_popups = FALSE;
    पूर्णाङ्क		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    यदि (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	यदि (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	अन्यथा यदि (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	अन्यथा यदि (popup_need_position_adjust(wp))
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
	पूर्णाङ्क has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    यदि ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    यदि ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	यदि (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	अन्यथा
	{
	    यदि (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    यदि (opacity_zindex != रिक्त)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * परिमाण(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    यदि (!popup_mask_refresh)
	प्रत्यागम;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    यदि (redrawing_all_win)
	mask = popup_mask;
    अन्यथा
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * परिमाण(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    यावत् ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != रिक्त)
    {
	पूर्णाङ्क width;
	पूर्णाङ्क height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	यदि (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    यदि (wp->w_popup_flags & POPF_HIDDEN)
		अनुवर्त;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	यदि (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	अन्यथा
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	यदि ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    अनुवर्त;

	{
	    पूर्णाङ्क mask_start = wp->w_winrow;
	    पूर्णाङ्क mask_end = mask_start + height;
	    पूर्णाङ्क mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    पूर्णाङ्क mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    प्रति (पंक्ति = mask_start;
			    पंक्ति < mask_end && पंक्ति < screen_Rows; ++पंक्ति)
		प्रति (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    यदि (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(पंक्ति, col, FALSE))
			mask[पंक्ति * screen_Columns + col] = POPUPMENU_ZINDEX;
		    अन्यथा यदि (wp->w_popup_mask_cells == रिक्त
				    || !popup_masked(wp, width, height, col, पंक्ति))
			mask[पंक्ति * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    यदि (mask == popup_mask_next)
    {
	पूर्णाङ्क	    *plines_cache = ALLOC_CLEAR_MULT(पूर्णाङ्क, Rows);
	win_T	    *prev_wp = रिक्त;

	प्रति (पंक्ति = 0; पंक्ति < screen_Rows; ++पंक्ति)
	{
	    पूर्णाङ्क	    col_done = 0;

	    प्रति (col = 0; col < screen_Columns; ++col)
	    {
		पूर्णाङ्क off = पंक्ति * screen_Columns + col;

		यदि (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    यदि (पंक्ति >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			यदि (!संदेश_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    अन्यथा यदि (col >= col_done)
		    {
			linenr_T	lnum;
			पूर्णाङ्क		पंक्ति_cp = पंक्ति;
			पूर्णाङ्क		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&पंक्ति_cp, &col_cp, IGNORE_POPUP);
			यदि (wp != रिक्त)
			{
#यदि defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    यदि (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    अन्यथा
#endif
			    {
				यदि (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   परिमाण(पूर्णाङ्क) * Rows);
				    prev_wp = wp;
				}

				यदि (पंक्ति_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				अन्यथा
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (शून्य)mouse_comp_pos(wp, &पंक्ति_cp, &col_cp,
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
    शून्य
may_update_popup_position(शून्य)
{
    यदि (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
स्थिर schar_T *base_screenlines = रिक्त;
स्थिर पूर्णाङ्क *base_screenattrs = रिक्त;
स्थिर u8char_T *base_screenlinesuc = रिक्त;
स्थिर पूर्णाङ्क base_screen_rows = 0;
स्थिर पूर्णाङ्क base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    पूर्णाङ्क
popup_get_base_screen_cell(पूर्णाङ्क row, पूर्णाङ्क col, schar_T *linep, पूर्णाङ्क *attrp,
							  u8char_T *ucp)
{
    यदि (base_screenlines == रिक्त || base_screenattrs == रिक्त)
	प्रत्यागम FALSE;
    यदि (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	प्रत्यागम FALSE;

    पूर्णाङ्क off = row * base_screen_cols + col;
    यदि (linep != रिक्त)
	*linep = base_screenlines[off];
    यदि (attrp != रिक्त)
	*attrp = base_screenattrs[off];
    यदि (ucp != रिक्त)
    {
	यदि (enc_utf8 && base_screenlinesuc != रिक्त)
	    *ucp = base_screenlinesuc[off];
	अन्यथा
	    *ucp = 0;
    }
    प्रत्यागम TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    शून्य
popup_set_base_screen_cell(पूर्णाङ्क row, पूर्णाङ्क col, schar_T पंक्ति, पूर्णाङ्क attr, u8char_T uc)
{
    यदि (base_screenlines == रिक्त || base_screenattrs == रिक्त)
	प्रत्यागम;
    यदि (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	प्रत्यागम;

    पूर्णाङ्क off = row * base_screen_cols + col;
    base_screenlines[off] = पंक्ति;
    base_screenattrs[off] = attr;
    यदि (enc_utf8 && base_screenlinesuc != रिक्त)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
स्थिर शून्य
draw_opacity_padding_cell(
	पूर्णाङ्क		row,
	पूर्णाङ्क		col,
	popup_saved_screen_T *saved_screen,
	पूर्णाङ्क		pad_start_col,
	पूर्णाङ्क		pad_end_col)
{
    यदि (पंक्ति_Offset == रिक्त || Screen_पंक्तियाँ == रिक्त || ScreenAttrs == रिक्त
	    || (enc_utf8 && Screen_पंक्तियाँ_UC == रिक्त))
	प्रत्यागम;

    पूर्णाङ्क off = पंक्ति_Offset[row] + col;
    पूर्णाङ्क r = row - saved_screen->start_row;
    पूर्णाङ्क c = col - saved_screen->start_col;

    यदि (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	पूर्णाङ्क save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	यदि (enc_utf8 && saved_screen->linesuc != रिक्त)
	{
	    पूर्णाङ्क base_col = col - 1;
	    पूर्णाङ्क base_off = off - 1;
	    पूर्णाङ्क base_save_off = save_off - 1;
	    पूर्णाङ्क wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    यदि (base_off >= 0)
	    {
		यदि (Screen_पंक्तियाँ_UC[base_off] != 0
			&& utf_char2cells(Screen_पंक्तियाँ_UC[base_off]) == 2
			&& Screen_पंक्तियाँ[off] == 0)
		    wide_prev = TRUE;
	    }
	    यदि (!wide_prev && save_off > 0)
	    {
		यदि (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->पंक्तियाँ[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    यदि (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		यदि (base_col < pad_start_col)
		{
		    यदि (Screen_पंक्तियाँ_UC[base_off] != 0
			    && utf_char2cells(Screen_पंक्तियाँ_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_पंक्तियाँ[base_off] = ' ';
			Screen_पंक्तियाँ_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_पंक्तियाँ[base_off],
				ScreenAttrs[base_off],
				Screen_पंक्तियाँ_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_पंक्तियाँ[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			यदि (enc_utf8)
			    Screen_पंक्तियाँ_UC[off] = 0;
			पूर्णाङ्क popup_attr_val =
					get_win_attr(screen_opacity_popup);
			पूर्णाङ्क blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_पंक्तियाँ[off], ScreenAttrs[off],
				Screen_पंक्तियाँ_UC[off]);
			screen_char(off, row, col);
			प्रत्यागम;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    यदि (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_पंक्तियाँ[base_off] == ' '
			    && Screen_पंक्तियाँ_UC[base_off] == 0)
		    {
			पूर्णाङ्क popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			पूर्णाङ्क blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_पंक्तियाँ[base_off] =
				    saved_screen->पंक्तियाँ[base_save_off];
			Screen_पंक्तियाँ_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_पंक्तियाँ[off] = 0;
			Screen_पंक्तियाँ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_पंक्तियाँ[base_off],
				    ScreenAttrs[base_off],
				    Screen_पंक्तियाँ_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_पंक्तियाँ[off],
				    ScreenAttrs[off],
				    Screen_पंक्तियाँ_UC[off]);
			screen_char(base_off, row, base_col);
			प्रत्यागम;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_पंक्तियाँ[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    यदि (enc_utf8)
			Screen_पंक्तियाँ_UC[off] = 0;
		    पूर्णाङ्क popup_attr_val = get_win_attr(screen_opacity_popup);
		    पूर्णाङ्क blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_पंक्तियाँ[off],
					       ScreenAttrs[off], Screen_पंक्तियाँ_UC[off]);
		    screen_char(off, row, col);
		    प्रत्यागम;
		}

		// Base cell is inside the saved area, redraw the wide char.
		यदि (save_off > 0)
		{
		    Screen_पंक्तियाँ[base_off] = saved_screen->पंक्तियाँ[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_पंक्तियाँ[off] = saved_screen->पंक्तियाँ[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_पंक्तियाँ_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_पंक्तियाँ_UC[off] = saved_screen->linesuc[save_off];

		    पूर्णाङ्क popup_attr_val = get_win_attr(screen_opacity_popup);
		    पूर्णाङ्क blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_पंक्तियाँ[base_off],
					       ScreenAttrs[base_off], Screen_पंक्तियाँ_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_पंक्तियाँ[off],
					       ScreenAttrs[off], Screen_पंक्तियाँ_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		प्रत्यागम;
	    }
	}
	Screen_पंक्तियाँ[off] = saved_screen->पंक्तियाँ[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	यदि (enc_utf8)
	    Screen_पंक्तियाँ_UC[off] = saved_screen->linesuc != रिक्त
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	यदि (enc_utf8 && Screen_पंक्तियाँ_UC[off] != 0
		&& utf_char2cells(Screen_पंक्तियाँ_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_पंक्तियाँ[off] = ' ';
	    Screen_पंक्तियाँ_UC[off] = 0;
	}

	पूर्णाङ्क popup_attr_val = get_win_attr(screen_opacity_popup);
	पूर्णाङ्क blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_पंक्तियाँ[off],
				   ScreenAttrs[off], Screen_पंक्तियाँ_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
स्थिर शून्य
fill_opacity_padding(
	पूर्णाङ्क		start_row,
	पूर्णाङ्क		end_row,
	पूर्णाङ्क		start_col,
	पूर्णाङ्क		end_col,
	popup_saved_screen_T *saved_screen)
{
    प्रति (पूर्णाङ्क pad_row = start_row; pad_row < end_row; pad_row++)
	प्रति (पूर्णाङ्क pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    शून्य
update_popups(शून्य (*win_update)(win_T *wp))
{
    win_T   *wp;
    पूर्णाङ्क	    शीर्ष_off;
    पूर्णाङ्क	    बायाँ_extra;
    पूर्णाङ्क	    total_width;
    पूर्णाङ्क	    total_height;
    पूर्णाङ्क	    शीर्ष_padding;
    पूर्णाङ्क	    popup_attr;
    पूर्णाङ्क	    border_attr[4];
    पूर्णाङ्क	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    पूर्णाङ्क	    row;
    पूर्णाङ्क	    wincol;
    पूर्णाङ्क	    padcol = 0;
    पूर्णाङ्क	    padendcol = 0;
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    sb_thumb_शीर्ष = 0;
    पूर्णाङ्क	    sb_thumb_height = 0;
    पूर्णाङ्क	    attr_scroll = 0;
    पूर्णाङ्क	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    यदि (base_screenlines != रिक्त)
    {
	vim_free(base_screenlines);
	base_screenlines = रिक्त;
    }
    यदि (base_screenattrs != रिक्त)
    {
	vim_free(base_screenattrs);
	base_screenattrs = रिक्त;
    }
    यदि (base_screenlinesuc != रिक्त)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = रिक्त;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    यावत् ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != रिक्त)
    {
	पूर्णाङ्क	    title_len = 0;
	पूर्णाङ्क	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = धकेलो_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	यदि ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	अन्यथा
	    screen_opacity_popup = रिक्त;

#ifdef FEAT_PROP_POPUP
	यदि (screen_opacity_popup != रिक्त)
	{
	    यदि (base_screenlines != रिक्त)
	    {
		vim_free(base_screenlines);
		base_screenlines = रिक्त;
	    }
	    यदि (base_screenattrs != रिक्त)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = रिक्त;
	    }
	    यदि (base_screenlinesuc != रिक्त)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = रिक्त;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(पूर्णाङ्क,
				    base_screen_rows * base_screen_cols);
	    यदि (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    यदि (base_screenlines != रिक्त && base_screenattrs != रिक्त)
	    {
		प्रति (पूर्णाङ्क r = 0; r < base_screen_rows; r++)
		{
		    पूर्णाङ्क off = पंक्ति_Offset[r];
		    पूर्णाङ्क base_off = r * base_screen_cols;
		    प्रति (पूर्णाङ्क c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_पंक्तियाँ[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			यदि (enc_utf8 && base_screenlinesuc != रिक्त)
			    base_screenlinesuc[base_off + c] =
					Screen_पंक्तियाँ_UC[off + c];
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
	शीर्ष_off = popup_शीर्ष_extra(wp);
	बायाँ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	यदि (wp->w_wincol + बायाँ_extra < 0)
	    बायाँ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(शीर्ष_off - wp->w_popup_topoff, 0);
	wp->w_wincol += बायाँ_extra;

	// Draw the popup text, unless it's off screen.
	यदि (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    यदि (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    यदि (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    अन्यथा यदि (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(शीर्ष_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= बायाँ_extra;

	// Add offset for border and padding if not done already.
	यदि ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += बायाँ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	यदि ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += शीर्ष_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	यदि (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	अन्यथा
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	यदि (total_width < 0)
	    total_width = 0;
	यदि (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	अन्यथा
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	यदि (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	अन्यथा
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	यदि (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	अन्यथा
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    प्रति (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    यदि (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	प्रति (i = 0; i < 8; ++i)
	    यदि (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	प्रति (i = 0; i < 4; ++i)
	{
	    यदि (wp->w_border_highlight[i] != रिक्त)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    अन्यथा यदि (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    अन्यथा
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    यदि ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	यदि ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	यदि (wp->w_popup_title != रिक्त)
	{
	    पूर्णाङ्क	    title_attr;

	    यदि (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != रिक्त)
		title_attr = border_attr[0];
	    अन्यथा यदि (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    अन्यथा
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    यदि (title_len > total_width - 2)
	    {
		पूर्णाङ्क	title_byte_len = (पूर्णाङ्क)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		यदि (title_text != रिक्त)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    अन्यथा
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	शीर्ष_padding = cl.eff_padding[0];
	यदि (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    यदि (title_len > 0)
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
	    अन्यथा
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    यदि (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	अन्यथा यदि (cl.eff_padding[0] == 0 && popup_शीर्ष_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    शीर्ष_padding = 1;

	यदि (शीर्ष_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    यदि (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	यदि (शीर्ष_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    यदि (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		यदि (screen_opacity_popup != रिक्त && saved_screen.पंक्तियाँ != रिक्त)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		अन्यथा
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		शीर्ष_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    यदि (screen_opacity_popup != रिक्त && saved_screen.पंक्तियाँ != रिक्त)
	    {
		fill_opacity_padding(row, row + शीर्ष_padding, padcol, padendcol,
			&saved_screen);
	    }
	    अन्यथा
	    {
		screen_fill(row, row + शीर्ष_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	यदि (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
	    पूर्णाङ्क		height = wp->w_height;
	    पूर्णाङ्क		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    यदि (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    यदि (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    यदि (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_शीर्ष = 0;
	    अन्यथा
		sb_thumb_शीर्ष = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    यदि (wp->w_topline > 1 && sb_thumb_शीर्ष == 0 && height > 1)
		sb_thumb_शीर्ष = 1;  // show it's scrolled
	    last = total_height - शीर्ष_off - wp->w_popup_border[2];
	    यदि (sb_thumb_शीर्ष >= last)
		// show at least one character
		sb_thumb_शीर्ष = last - 1;

	    यदि (wp->w_scrollbar_highlight != रिक्त)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    अन्यथा
		attr_scroll = highlight_attr[HLF_PSB];
	    यदि (wp->w_thumb_highlight != रिक्त)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    अन्यथा
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	प्रति (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    पूर्णाङ्क	pad_बायाँ;
	    // left and right padding only needed next to the body
	    पूर्णाङ्क do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    यदि (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    यदि (do_padding && cl.eff_padding[3] > 0)
	    {
		पूर्णाङ्क col = wincol + wp->w_popup_border[3];

		// left padding
		pad_बायाँ = wp->w_popup_padding[3];
		यदि (col < 0)
		{
		    pad_बायाँ += col;
		    col = 0;
		}
		यदि (pad_बायाँ > 0)
		{
		    यदि (screen_opacity_popup != रिक्त && saved_screen.पंक्तियाँ != रिक्त)
			fill_opacity_padding(row, row + 1, col, col + pad_बायाँ,
				&saved_screen);
		    अन्यथा
			screen_fill(row, row + 1, col, col + pad_बायाँ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    यदि (wp->w_has_scrollbar)
	    {
		पूर्णाङ्क पंक्ति = i - शीर्ष_off;
		पूर्णाङ्क scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		यदि (पंक्ति >= 0 && पंक्ति < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    पंक्ति >= sb_thumb_शीर्ष
				       && पंक्ति < sb_thumb_शीर्ष + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		अन्यथा
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    यदि (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    यदि (do_padding && cl.eff_padding[1] > 0)
	    {
		पूर्णाङ्क pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		पूर्णाङ्क pad_col_end = pad_col_start + wp->w_popup_padding[1];

		यदि (screen_opacity_popup != रिक्त && saved_screen.पंक्तियाँ != रिक्त)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		अन्यथा
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	यदि (wp->w_popup_shadow)
	{
	    पूर्णाङ्क col = wincol + total_width;
	    प्रति (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	यदि (cl.eff_padding[2] > 0)
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
	    यदि (screen_opacity_popup != रिक्त && saved_screen.पंक्तियाँ != रिक्त)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    अन्यथा
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	यदि (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    यदि (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	यदि (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    प्रति (पूर्णाङ्क col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	यदि (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = रिक्त;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	यदि (override_success)
	    निकालो_highlight_overrides();

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

#यदि defined(FEAT_खोज_EXTRA)
    // In case win_update() called start_search_hl().
    end_खोज_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    स्थिर पूर्णाङ्क
set_ref_in_one_popup(win_T *wp, पूर्णाङ्क copyID)
{
    पूर्णाङ्क		abort = FALSE;
    typval_T	tv;

    यदि (wp->w_close_cb.cb_partial != रिक्त)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    यदि (wp->w_filter_cb.cb_partial != रिक्त)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    प्रत्यागम abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    पूर्णाङ्क
set_ref_in_popups(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    प्रति (wp = first_popupwin; !abort && wp != रिक्त; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	प्रति (wp = tp->tp_first_popupwin; !abort && wp != रिक्त; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	यदि (abort)
	    भङ्ग;
    }
    प्रत्यागम abort;
}

    पूर्णाङ्क
popup_is_popup(win_T *wp)
{
    प्रत्यागम wp->w_popup_flags != 0;
}

#यदि defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(शून्य)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि (wp->w_p_pvw)
	    प्रत्यागम wp;
    प्रत्यागम रिक्त;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(शून्य)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि (wp->w_popup_flags & POPF_INFO)
	    प्रत्यागम wp;
    प्रत्यागम रिक्त;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(पूर्णाङ्क dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    पूर्णाङ्क		by;
    linenr_T	new_topline;

    यदि (wp == रिक्त)
	प्रत्यागम false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    यदि (new_topline < 1)
	new_topline = 1;
    यदि (new_topline > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
	new_topline = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
    यदि (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    प्रत्यागम true;
#अन्यथा
    प्रत्यागम false;
#endif
}

    शून्य
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_संख्या = message_win == रिक्त ? 0 : message_win->w_id;
#अन्यथा
    rettv->vval.v_संख्या = 0;
#endif
}

    शून्य
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_संख्या = wp == रिक्त ? 0 : wp->w_id;
#अन्यथा
    rettv->vval.v_संख्या = 0;
#endif
}

    शून्य
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_संख्या = wp == रिक्त ? 0 : wp->w_id;
#अन्यथा
    rettv->vval.v_संख्या = 0;
#endif
}

#यदि defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    पूर्णाङ्क
popup_create_preview_window(पूर्णाङ्क info)
{
    win_T *wp = popup_create(रिक्त, रिक्त, info ? TYPE_INFO : TYPE_PREVIEW);

    यदि (wp == रिक्त)
	प्रत्यागम FAIL;
    यदि (info)
	wp->w_popup_flags |= POPF_INFO;
    अन्यथा
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    यदि (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    अन्यथा यदि (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    अन्यथा
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    प्रत्यागम OK;
}

/*
 * Close any preview popup.
 */
    शून्य
popup_close_preview(शून्य)
{
    win_T *wp = popup_find_preview_window();

    यदि (wp != रिक्त)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    शून्य
popup_hide_info(शून्य)
{
    win_T *wp = popup_find_info_window();

    यदि (wp != रिक्त)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	यदि (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    शून्य
popup_close_info(शून्य)
{
    win_T *wp = popup_find_info_window();

    यदि (wp != रिक्त)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    पूर्णाङ्क
popup_overlaps_cmdline(शून्य)
{
    प्रत्यागम popup_on_cmdline;
}

#यदि defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(शून्य)
{
    यदि (message_win != रिक्त)
	प्रत्यागम message_win;

    पूर्णाङ्क i;

    message_win = popup_create(रिक्त, रिक्त, TYPE_MESSAGE_WIN);

    यदि (message_win == रिक्त)
	प्रत्यागम रिक्त;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    प्रति (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    प्रति (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    यदि (message_win->w_popup_timer != रिक्त)
	message_win->w_popup_timer->tr_keep = TRUE;
    प्रत्यागम message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    शून्य
popup_show_message_win(शून्य)
{
    यदि (message_win == रिक्त)
	प्रत्यागम;

    यदि ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    स्थिर शून्य
may_start_message_win_timer(win_T *wp)
{
    यदि (wp == message_win && start_message_win_timer)
    {
	यदि (message_win->w_popup_timer != रिक्त)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    पूर्णाङ्क
popup_message_win_visible(शून्य)
{
    प्रत्यागम message_win != रिक्त
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    शून्य
popup_hide_message_win(शून्य)
{
    यदि (message_win != रिक्त)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
स्थिर पूर्णाङ्क save_संदेश_didout = FALSE;
स्थिर पूर्णाङ्क save_संदेश_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
स्थिर पूर्णाङ्क ew_संदेश_didout = FALSE;
स्थिर पूर्णाङ्क ew_संदेश_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    शून्य
start_echowindow(पूर्णाङ्क time_sec)
{
    in_echowindow = TRUE;
    save_संदेश_didout = संदेश_didout;
    save_संदेश_col = संदेश_col;
    संदेश_didout = ew_संदेश_didout;
    संदेश_col = ew_संदेश_col;
    यदि (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    शून्य
end_echowindow(शून्य)
{
    in_echowindow = FALSE;

    यदि ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_संदेश_didout = TRUE;
    ew_संदेश_col = संदेश_col == 0 ? 1 : संदेश_col;
    संदेश_didout = save_संदेश_didout;
    संदेश_col = save_संदेश_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    पूर्णाङ्क
popup_win_closed(win_T *win)
{
    पूर्णाङ्क	    round;
    win_T   *wp;
    win_T   *next;
    पूर्णाङ्क	    ret = FALSE;

    प्रति (round = 1; round <= 2; ++round)
	प्रति (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != रिक्त; wp = next)
	{
	    next = wp->w_next;
	    यदि (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    प्रत्यागम ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    शून्य
popup_set_title(win_T *wp)
{
    यदि (wp->w_buffer->b_fname == रिक्त)
	प्रत्यागम;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    यदि (wp->w_popup_title != रिक्त)
	vim_snprintf((वर्ण *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#यदि defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    शून्य
popup_update_preview_title(शून्य)
{
    win_T *wp = popup_find_preview_window();

    यदि (wp != रिक्त)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
