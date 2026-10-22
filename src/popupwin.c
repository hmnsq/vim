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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_PROP_POPUP)

প্রকারনির্দেশ সংরচনা {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		active;
    পূর্ণাঙ্ক		winrow;
    পূর্ণাঙ্ক		wincol;
    পূর্ণাঙ্ক		height;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		leftoff;
    পূর্ণাঙ্ক		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
প্রকারনির্দেশ সংরচনা {
    schar_T	*সাৰিগুলি;
    পূর্ণাঙ্ক		*attrs;
    u8char_T	*linesuc;
    পূর্ণাঙ্ক		start_row;
    পূর্ণাঙ্ক		start_col;
    পূর্ণাঙ্ক		rows;
    পূর্ণাঙ্ক		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
প্রকারনির্দেশ সংরচনা {
    linenr_T	firstline;
    পূর্ণাঙ্ক		blend;
    পূর্ণাঙ্ক		flags;
    পূর্ণাঙ্ক		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		winrow;
    পূর্ণাঙ্ক		wincol;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		height;
    পূর্ণাঙ্ক		leftcol;
    পূর্ণাঙ্ক		leftoff;
    পূর্ণাঙ্ক		has_scrollbar;
    পূর্ণাঙ্ক		topoff;
    পূর্ণাঙ্ক		bottomoff;
    পূর্ণাঙ্ক		leftclip;
    পূর্ণাঙ্ক		rightclip;
} popup_layout_T;

স্থির poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
স্থির win_T *message_win = রিক্ত;

// Time used for the next ":echowindow" message in msec.
স্থির পূর্ণাঙ্ক  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
স্থির পূর্ণাঙ্ক    start_message_win_timer = FALSE;

স্থির শূন্য may_start_message_win_timer(win_T *wp);
#endif

স্থির পূর্ণাঙ্ক popup_on_cmdline = FALSE;

স্থির শূন্য popup_adjust_position(win_T *wp);
স্থির bool popup_area_changed(win_T *wp, popup_area_T *area);
স্থির শূন্য popup_redraw_exposed_area(popup_area_T *area);
স্থির শূন্য popup_save_area(win_T *wp, popup_area_T *area);
স্থির শূন্য popup_free_saved_screen(popup_saved_screen_T *saved_screen);
স্থির শূন্য popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
স্থির bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
স্থির bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
স্থির শূন্য popup_save_style(win_T *wp, popup_style_snapshot_T *style);
স্থির শূন্য popup_save_layout(win_T *wp, popup_layout_T *layout);
স্থির শূন্য redraw_under_popup_area(পূর্ণাঙ্ক winrow, পূর্ণাঙ্ক wincol, পূর্ণাঙ্ক height,
	পূর্ণাঙ্ক width, পূর্ণাঙ্ক leftoff);
স্থির শূন্য redraw_overlapped_opacity_popups(পূর্ণাঙ্ক winrow, পূর্ণাঙ্ক wincol,
	পূর্ণাঙ্ক height, পূর্ণাঙ্ক width, পূর্ণাঙ্ক leftoff, পূর্ণাঙ্ক zindex);
স্থির শূন্য redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    স্থির পূর্ণাঙ্ক
popup_options_one(dict_T *dict, char_u *চাবি)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    পূর্ণাঙ্ক		n = 0;

    di = dict_find(dict, চাবি, -1);
    যদি (di == রিক্ত)
	ঘূৰাই MAXCOL;

    val = tv_get_string(&di->di_tv);
    যদি (STRNCMP(val, "cursor", 6) != 0)
	ঘূৰাই dict_get_সংখ্যা_check(dict, চাবি);

    setcursor_mayforce(TRUE);
    s = val + 6;
    যদি (*s != NUL)
    {
	endp = s;
	যদি (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((ৱর্ণ *)s, (ৱর্ণ **)&endp, 10);
	যদি (endp != রিক্ত && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    ঘূৰাই 0;
	}
    }

    যদি (STRCMP(চাবি, "line") == 0)
	n = screen_screenrow() + 1 + n;
    অন্যথা // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    যদি (n == 0)
	n = -1;
    ঘূৰাই n;
}

    স্থির পূর্ণাঙ্ক
set_padding_border(dict_T *dict, পূর্ণাঙ্ক *array, ৱর্ণ *name, পূর্ণাঙ্ক max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    যদি (di == রিক্ত)
	ঘূৰাই OK;

    যদি (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	ঘূৰাই FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		nr;

    বাবে (i = 0; i < 4; ++i)
	array[i] = 1;
    যদি (list == রিক্ত)
	ঘূৰাই OK;

    CHECK_LIST_MATERIALIZE(list);
    বাবে (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&li->li_tv);
	যদি (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    ঘূৰাই OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    স্থির শূন্য
set_moved_মানসমূহ(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    স্থির শূন্য
set_moved_columns(win_T *wp, পূর্ণাঙ্ক flags)
{
    char_u	*ptr;
    পূর্ণাঙ্ক		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    পূর্ণাঙ্ক		mincol;

    যদি (len <= 0)
	ঘূৰাই;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (পূর্ণাঙ্ক)(ptr - ml_get_curline());
    যদি (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	ঘূৰাই;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    স্থির শূন্য
set_mousemoved_মানসমূহ(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    স্থির শূন্য
update_popup_uses_mouse_move(শূন্য)
{
    popup_uses_mouse_move = FALSE;
    যদি (!popup_visible)
	ঘূৰাই;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ঘূৰাই;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ঘূৰাই;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    স্থির শূন্য
set_mousemoved_columns(win_T *wp, পূর্ণাঙ্ক flags)
{
    win_T	*textwp;
    char_u	*text;
    পূর্ণাঙ্ক		col;
    pos_T	pos;
    colnr_T	mcol;

    যদি (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, রিক্ত, &col) != OK)
	ঘূৰাই;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, রিক্ত, রিক্ত, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, রিক্ত, রিক্ত, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    পূর্ণাঙ্ক
popup_on_border(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    ঘূৰাই (row == 0 && wp->w_popup_border[0] > 0)
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
    পূর্ণাঙ্ক
popup_close_if_on_X(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

// Values set when dragging a popup window starts.
স্থির পূর্ণাঙ্ক drag_start_row;
স্থির পূর্ণাঙ্ক drag_start_col;
স্থির পূর্ণাঙ্ক drag_start_wantline;
স্থির পূর্ণাঙ্ক drag_start_wantcol;
স্থির পূর্ণাঙ্ক drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    শূন্য
popup_start_drag(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    যদি (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    অন্যথা
	drag_start_wantline = wp->w_wantline;
    যদি (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    অন্যথা
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    যদি (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    যদি (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	যদি (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	যদি (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    শূন্য
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    যদি (!win_valid_popup(wp))
	ঘূৰাই;

    যদি ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	পূর্ণাঙ্ক width_inc = mouse_col - drag_start_col;
	পূর্ণাঙ্ক height_inc = mouse_row - drag_start_row;

	যদি (width_inc != 0)
	{
	    পূর্ণাঙ্ক width = wp->w_width + width_inc;

	    যদি (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	যদি (height_inc != 0)
	{
	    পূর্ণাঙ্ক height = wp->w_height + height_inc;

	    যদি (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	ঘূৰাই;
    }

    যদি (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	ঘূৰাই;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    যদি (wp->w_wantline < 1)
	wp->w_wantline = 1;
    যদি (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    যদি (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    যদি (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    শূন্য
popup_set_firstline(win_T *wp)
{
    পূর্ণাঙ্ক	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    যেতিয়ালৈ (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    পূর্ণাঙ্ক
popup_is_in_scrollbar(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    ঘূৰাই wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    শূন্য
popup_handle_scrollbar_click(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (!popup_is_in_scrollbar(wp, row, col))
	ঘূৰাই;

    পূর্ণাঙ্ক	    height = popup_height(wp);
    পূর্ণাঙ্ক	    new_topline = wp->w_topline;

    যদি (row >= height / 2)
    {
	// Click in lower half, scroll down.
	যদি (wp->w_topline < wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    ++new_topline;
    }
    অন্যথা যদি (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    যদি (new_topline == wp->w_topline)
	ঘূৰাই;

    set_topline(wp, new_topline);
    যদি (wp == curwin)
    {
	যদি (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	অন্যথা যদি (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#যদি defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    স্থির শূন্য
popup_add_timeout(win_T *wp, পূর্ণাঙ্ক time, পূর্ণাঙ্ক close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((ৱর্ণ *)cbbuf, পরিমাণ(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    যদি (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	ঘূৰাই;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    যদি (cb.cb_name != রিক্ত && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    স্থির poppos_T
get_pos_entry(dict_T *d, পূর্ণাঙ্ক give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    পূর্ণাঙ্ক	    nr;

    যদি (str == রিক্ত)
	ঘূৰাই POPPOS_NONE;

    বাবে (nr = 0; nr < (পূর্ণাঙ্ক)ARRAY_LENGTH(poppos_entries); ++nr)
	যদি (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    ঘূৰাই poppos_entries[nr].pp_val;

    যদি (give_error)
	semsg(_(e_invalid_argument_str), str);
    ঘূৰাই POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    স্থির শূন্য
apply_move_options(win_T *wp, dict_T *d)
{
    পূর্ণাঙ্ক		nr;
    char_u	*str;
    dictitem_T	*di;

    যদি ((nr = dict_get_সংখ্যা_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    যদি ((nr = dict_get_সংখ্যা_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    যদি ((nr = dict_get_সংখ্যা_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    যদি ((nr = dict_get_সংখ্যা_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    যদি (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    যদি (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    যদি (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	যদি (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    যদি (str != রিক্ত)
    {
	wp->w_popup_prop_type = 0;
	যদি (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    যদি (di != রিক্ত)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		যদি (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    যদি (nr <= 0)
		nr = find_prop_type_id(str, রিক্ত);
	    যদি (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    অন্যথা
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    যদি (di != রিক্ত)
	wp->w_popup_prop_id = dict_get_সংখ্যা(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    স্থির শূন্য
handle_moved_argument(win_T *wp, dictitem_T *di, পূর্ণাঙ্ক mousemoved)
{
    যদি (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != রিক্ত)
    {
	char_u  *s = di->di_tv.vval.v_string;
	পূর্ণাঙ্ক	flags = 0;

	যদি (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	অন্যথা যদি (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	অন্যথা যদি (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	অন্যথা যদি (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	যদি (flags != 0)
	{
	    যদি (mousemoved)
		set_mousemoved_columns(wp, flags);
	    অন্যথা
		set_moved_columns(wp, flags);
	}
    }
    অন্যথা যদি (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != রিক্ত
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	পূর্ণাঙ্ক	    mincol;
	পূর্ণাঙ্ক	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	যদি (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_সংখ্যা(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    যদি (mousemoved)
		wp->w_popup_mouse_row = nr;
	    অন্যথা
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    যদি (nr == 0)
		wp->w_popup_curwin = রিক্ত;
	}

	mincol = tv_get_সংখ্যা(&li->li_tv);
	maxcol = tv_get_সংখ্যা(&li->li_next->li_tv);
	যদি (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	অন্যথা
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    অন্যথা
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    স্থির শূন্য
check_highlight(dict_T *dict, ৱর্ণ *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    যদি (di == রিক্ত)
	ঘূৰাই;

    যদি (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_মান_for_argument_str), name);
    অন্যথা
    {
	str = tv_get_string(&di->di_tv);
	যদি (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    স্থির শূন্য
popup_show_curline(win_T *wp)
{
    যদি (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    অন্যথা যদি (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	যদি (wp->w_topline < 1)
	    wp->w_topline = 1;
	অন্যথা যদি (wp->w_topline > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    wp->w_topline = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	যেতিয়ালৈ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_সাৰি_গুনতি
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    যদি (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    স্থির char_u *
popup_get_sign_name(win_T *wp)
{
    স্থির ৱর্ণ    buf[30];

    vim_snprintf(buf, পরিমাণ(buf), "popup-%d", wp->w_id);
    ঘূৰাই (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    স্থির শূন্য
popup_highlight_curline(win_T *wp)
{
    পূর্ণাঙ্ক	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    যদি ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	যদি (!sign_exists_by_name(sign_name))
	{
	    ৱর্ণ *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, রিক্ত, (char_u *)linehl, রিক্ত, রিক্ত, রিক্ত,
		    রিক্ত, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    অন্যথা
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    স্থির পূর্ণাঙ্ক
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    পূর্ণাঙ্ক		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    যদি (di != রিক্ত)
    {
	wp->w_firstline = dict_get_সংখ্যা(dict, "firstline");
	যদি (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    যদি (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    যদি (str != রিক্ত)
    {
	char_u	*title = vim_strsave(str);

	যদি (title != রিক্ত)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    যদি (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    যদি (di != রিক্ত)
    {
	nr = dict_get_সংখ্যা(dict, "opacity");
	যদি (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	অন্যথা যদি (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	অন্যথা যদি (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	অন্যথা
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    যদি (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    অন্যথা
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    যদি (di != রিক্ত && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != রিক্ত)
    {
	image_T		    *img;
	image_T		    *cur_img = রিক্ত;
	image_placement_T   *place;
	পূর্ণাঙ্ক		    cw, ch;

	যদি (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    যদি (wp->w_popup_imagep != রিক্ত)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = রিক্ত;
	    }
	    গচ্ছ image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	যদি (wp->w_popup_imagep != রিক্ত && wp->w_popup_imagep->img != রিক্ত)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	যদি (img == রিক্ত)
	    ঘূৰাই FAIL;

	যদি (img == cur_img)
	    গচ্ছ image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	যদি (place == রিক্ত)
	{
	    image_unref(img);
	    ঘূৰাই FAIL;
	}

	যদি (wp->w_popup_imagep != রিক্ত)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	যদি (dict_find(dict, (char_u *)"minwidth", -1) == রিক্ত)
	    wp->w_minwidth = cw;
	যদি (dict_find(dict, (char_u *)"maxwidth", -1) == রিক্ত)
	    wp->w_maxwidth = cw;
	যদি (dict_find(dict, (char_u *)"minheight", -1) == রিক্ত)
	    wp->w_minheight = ch;
	যদি (dict_find(dict, (char_u *)"maxheight", -1) == রিক্ত)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    অন্যথা
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	যদি (wp->w_popup_imagep != রিক্ত)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(রিক্ত, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	যদি (wp->w_popup_imagep != রিক্ত)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    যদি (di != রিক্ত)
    {
	পূর্ণাঙ্ক ok = TRUE;

	যদি (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != রিক্ত)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    যদি (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    অন্যথা যদি (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    অন্যথা যদি (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    অন্যথা
		ok = FALSE;
	}
	অন্যথা
	    ok = FALSE;
	যদি (!ok)
	    semsg(_(e_invalid_মান_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    যদি (str != রিক্ত)
    {
	ৱর্ণ *errmsg = update_wincolor(wp, str);

	যদি (errmsg == রিক্ত)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	অন্যথা
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    যদি (str != রিক্ত)
    {
	ৱর্ণ *errmsg = update_winhighlight(wp, str);

	যদি (errmsg == রিক্ত)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	অন্যথা
	    emsg(_(errmsg));
    }

    যদি (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	ঘূৰাই FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    যদি (di != রিক্ত)
    {
	যদি (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == রিক্ত)
	{
	    emsg(_(e_list_required));
	    ঘূৰাই FAIL;
	}
	অন্যথা
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    পূর্ণাঙ্ক		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    যদি (list->lv_len == 0)
	    {
		বাবে (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = রিক্ত;
		}
	    }
	    অন্যথা
	    {
		বাবে (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    যদি (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			যদি (hl != রিক্ত)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		যদি (list->lv_len == 1 && wp->w_border_highlight[0] != রিক্ত)
		    বাবে (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			যদি (hl != রিক্ত)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    যদি (di != রিক্ত)
    {
	যদি (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ঘূৰাই FAIL;
	}
	অন্যথা
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    পূর্ণাঙ্ক		i;

	    যদি (list != রিক্ত)
	    {
		CHECK_LIST_MATERIALIZE(list);
		বাবে (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    যদি (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		যদি (list->lv_len == 1)
		    বাবে (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		যদি (list->lv_len == 2)
		{
		    বাবে (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    বাবে (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    যদি (di != রিক্ত)
    {
	wp->w_zindex = dict_get_সংখ্যা(dict, "zindex");
	যদি (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	যদি (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    যদি (di != রিক্ত)
    {
	পূর্ণাঙ্ক ok = FALSE;

	যদি (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != রিক্ত)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_বস্তুসমূহ(di->di_tv.vval.v_list, li)
	    {
		যদি (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == রিক্ত
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ভঙ্গ;
		}
		অন্যথা
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	যদি (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	অন্যথা
	{
	    semsg(_(e_invalid_মান_for_argument_str), "mask");
	    ঘূৰাই FAIL;
	}
    }

#যদি defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_সংখ্যা(dict, "time");
    যদি (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    যদি (di != রিক্ত)
    {
	set_moved_মানসমূহ(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    যদি (di != রিক্ত)
    {
	set_mousemoved_মানসমূহ(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    যদি (nr != -1)
    {
	যদি (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    যদি (di != রিক্ত)
    {
	callback_T	callback = get_callback(&di->di_tv);

	যদি (callback.cb_name != রিক্ত)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    যদি (nr != -1)
    {
	যদি (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    যদি (str != রিক্ত)
    {
	যদি (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	অন্যথা
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    যদি (di == রিক্ত)
	ঘূৰাই OK;

    callback_T	callback = get_callback(&di->di_tv);
    যদি (callback.cb_name == রিক্ত)
	ঘূৰাই OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    ঘূৰাই OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    স্থির পূর্ণাঙ্ক
apply_options(win_T *wp, dict_T *dict, পূর্ণাঙ্ক create)
{
    পূর্ণাঙ্ক		nr;

    apply_move_options(wp, dict);

    যদি (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    যদি (apply_general_options(wp, dict) == FAIL)
	ঘূৰাই FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    যদি (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    যদি (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	যদি (wp->w_firstline > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	অন্যথা যদি (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    যদি (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    ঘূৰাই OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    স্থির শূন্য
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_বস্তুসমূহ(l, li)
	যদি (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == রিক্ত ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    স্থির শূন্য
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	যদি (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    ঘূৰাই;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == রিক্ত ? রিক্ত : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == রিক্ত ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    বাবে (li = l->lv_first; li != রিক্ত; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	যদি (di != রিক্ত)
	{
	    যদি (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ঘূৰাই;
	    }
	    plist = di->di_tv.vval.v_list;
	    যদি (plist != রিক্ত)
	    {
		FOR_ALL_LIST_বস্তুসমূহ(plist, pli)
		{
		    যদি (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			ঘূৰাই;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    যদি (dict != রিক্ত)
		    {
			পূর্ণাঙ্ক col = dict_get_সংখ্যা(dict, "col");

			prop_add_common(lnum, col, dict, buf, রিক্ত);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    পূর্ণাঙ্ক
popup_শীৰ্ষ_extra(win_T *wp)
{
    পূর্ণাঙ্ক	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    যদি (extra == 0 && wp->w_popup_title != রিক্ত && *wp->w_popup_title != NUL)
	ঘূৰাই 1;
    ঘূৰাই extra;
}

/*
 * Get the padding plus border at the left.
 */
    পূর্ণাঙ্ক
popup_বাম_extra(win_T *wp)
{
    ঘূৰাই wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    পূর্ণাঙ্ক
popup_height(win_T *wp)
{
    ঘূৰাই wp->w_height
	    + popup_শীৰ্ষ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    পূর্ণাঙ্ক
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    ঘূৰাই wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    পূর্ণাঙ্ক
popup_extra_width(win_T *wp)
{
    ঘূৰাই wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    স্থির win_T *
popup_get_clipwin(win_T *wp)
{
    যদি (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	ঘূৰাই রিক্ত;
    যদি (win_valid(wp->w_popup_prop_win))
	ঘূৰাই wp->w_popup_prop_win;
    ঘূৰাই রিক্ত;
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
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক শীৰ্ষ_extra;
    পূর্ণাঙ্ক bot_extra;
    পূর্ণাঙ্ক বাম_extra;
    পূর্ণাঙ্ক ডান_extra;

    পূর্ণাঙ্ক clip_শীৰ্ষ_content;
    পূর্ণাঙ্ক clip_bot_content;
    পূর্ণাঙ্ক clip_বাম_content;
    পূর্ণাঙ্ক clip_ডান_content;

    পূর্ণাঙ্ক eff_শীৰ্ষ_extra;
    পূর্ণাঙ্ক eff_bot_extra;
    পূর্ণাঙ্ক eff_বাম_extra;
    পূর্ণাঙ্ক eff_ডান_extra;

    পূর্ণাঙ্ক eff_border[4];
    পূর্ণাঙ্ক eff_padding[4];

    পূর্ণাঙ্ক eff_height;
    পূর্ণাঙ্ক eff_width;
} popup_clip_T;

    স্থির শূন্য
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    পূর্ণাঙ্ক h, w;

    cl->শীৰ্ষ_extra = popup_শীৰ্ষ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->বাম_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->ডান_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_শীৰ্ষ_content = wp->w_popup_topoff - cl->শীৰ্ষ_extra;
    যদি (cl->clip_শীৰ্ষ_content < 0)
	cl->clip_শীৰ্ষ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    যদি (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_বাম_content = wp->w_popup_leftclip - cl->বাম_extra;
    যদি (cl->clip_বাম_content < 0)
	cl->clip_বাম_content = 0;
    cl->clip_ডান_content = wp->w_popup_rightclip - cl->ডান_extra;
    যদি (cl->clip_ডান_content < 0)
	cl->clip_ডান_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	পূর্ণাঙ্ক clip = wp->w_popup_topoff;
	পূর্ণাঙ্ক b = wp->w_popup_border[0];
	পূর্ণাঙ্ক p = wp->w_popup_padding[0];
	পূর্ণাঙ্ক rem;

	যদি (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	অন্যথা
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	পূর্ণাঙ্ক clip = wp->w_popup_bottomoff;
	পূর্ণাঙ্ক b = wp->w_popup_border[2];
	পূর্ণাঙ্ক p = wp->w_popup_padding[2];
	পূর্ণাঙ্ক rem;

	যদি (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	অন্যথা
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
	পূর্ণাঙ্ক excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	যদি (excess > 0)
	{
	    যদি (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		যদি (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		অন্যথা
		    cl->eff_border[0] -= excess;
	    }
	    অন্যথা
		cl->eff_padding[0] -= excess;
	}
    }
    {
	পূর্ণাঙ্ক excess = wp->w_popup_topoff - cl->শীৰ্ষ_extra - wp->w_height;
	যদি (excess > 0)
	{
	    যদি (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		যদি (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		অন্যথা
		    cl->eff_border[2] -= excess;
	    }
	    অন্যথা
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_শীৰ্ষ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_বাম_extra = wp->w_popup_leftclip > 0 ? 0 : cl->বাম_extra;
    cl->eff_ডান_extra = wp->w_popup_rightclip > 0 ? 0 : cl->ডান_extra;

    h = wp->w_height - cl->clip_শীৰ্ষ_content - cl->clip_bot_content;
    যদি (h < 0)
	h = 0;
    cl->eff_height = cl->eff_শীৰ্ষ_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_বাম_content - cl->clip_ডান_content;
    যদি (w < 0)
	w = 0;
    cl->eff_width = cl->eff_বাম_extra + w + cl->eff_ডান_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		height;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		winrow;
    পূর্ণাঙ্ক		wincol;
    পূর্ণাঙ্ক		leftcol;
    পূর্ণাঙ্ক		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    স্থির শূন্য
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

    স্থির শূন্য
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
    স্থির শূন্য
popup_screenpos_above_শীৰ্ষ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	পূর্ণাঙ্ক	    *screen_row,
	পূর্ণাঙ্ক	    *screen_scol,
	পূর্ণাঙ্ক	    *screen_ccol,
	পূর্ণাঙ্ক	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    পূর্ণাঙ্ক	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (পূর্ণাঙ্ক)win_col_off(prop_win) - (পূর্ণাঙ্ক)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (পূর্ণাঙ্ক)scol + coloff;
    *screen_ccol = (পূর্ণাঙ্ক)ccol + coloff;
    *screen_ecol = (পূর্ণাঙ্ক)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (পূর্ণাঙ্ক)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    স্থির শূন্য
popup_hide_for_textprop(win_T *wp)
{
    যদি ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ঘূৰাই;
    wp->w_popup_flags |= POPF_HIDDEN;
    যদি (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    স্থির bool
popup_find_prop_above_শীৰ্ষ(
	win_T	    *wp,
	win_T	    *prop_win,
	পূর্ণাঙ্ক	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    যদি (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	ঘূৰাই false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    যদি (first < 1)
	first = 1;
    ঘূৰাই find_prop_in_সাৰিগুলি(prop_win,
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
    স্থির bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    পূর্ণাঙ্ক	    extra_h, extra_w;
    পূর্ণাঙ্ক	    popup_শীৰ্ষ, popup_bottom, popup_বাম, popup_ডান;
    পূর্ণাঙ্ক	    total_h, total_w;

    যদি (cw == রিক্ত)
	ঘূৰাই false;

    extra_h = popup_শীৰ্ষ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_শীৰ্ষ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_বাম = wp->w_wincol;
    popup_ডান = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    যদি (popup_শীৰ্ষ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_শীৰ্ষ;
    যদি (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    যদি (popup_বাম < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_বাম;
    যদি (popup_ডান > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_ডান - (cw->w_wincol + cw->w_width);

    ঘূৰাই wp->w_popup_topoff >= total_h
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
    স্থির শূন্য
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    যদি (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_শীৰ্ষ_content + cl->clip_bot_content;
	যদি (wp->w_height < 0)
	    wp->w_height = 0;
	যদি (cl->clip_শীৰ্ষ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_শীৰ্ষ_content;
    }
    যদি (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	যদি (cl->clip_বাম_content > 0 || cl->clip_ডান_content > 0)
	    wp->w_p_wrap = FALSE;
	যদি (cl->clip_ডান_content > 0)
	{
	    wp->w_width -= cl->clip_ডান_content;
	    যদি (wp->w_width < 0)
		wp->w_width = 0;
	}
	যদি (cl->clip_বাম_content > 0)
	{
	    wp->w_leftcol += cl->clip_বাম_content;
	    wp->w_wincol += cl->clip_বাম_content;
	    wp->w_width -= cl->clip_বাম_content;
	    যদি (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    স্থির শূন্য
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    পূর্ণাঙ্ক iw, ih;

    // Visible cell width and height of popup itself (not image!)
    পূর্ণাঙ্ক	visible_width;
    পূর্ণাঙ্ক	visible_height;

    // Crop dimensions of image (in pixels).
    পূর্ণাঙ্ক	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    যদি (wp->w_popup_imagep == রিক্ত || wp->w_popup_flags & POPF_HIDDEN)
	ঘূৰাই;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    যদি (wp->w_popup_imagep->img == রিক্ত)
	// Just set the bounding box
	গচ্ছ exit;

    visible_width =
	wp->w_width - clip.clip_বাম_content - clip.clip_ডান_content;
    visible_height =
	wp->w_height - clip.clip_শীৰ্ষ_content - clip.clip_bot_content;

    যদি (visible_width < 0 || visible_height < 0)
	ঘূৰাই;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_বাম_content;
    crop_y = clip.clip_শীৰ্ষ_content;
    crop_width = iw - crop_x - clip.clip_ডান_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    যদি (crop_width > visible_width)
	crop_width = visible_width;
    যদি (crop_height > visible_height)
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
    স্থির শূন্য
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		wrapped = 0;
    পূর্ণাঙ্ক		maxwidth;
    পূর্ণাঙ্ক		maxwidth_no_scrollbar;
    পূর্ণাঙ্ক		width_with_scrollbar = 0;
    পূর্ণাঙ্ক		used_maxwidth = FALSE;
    পূর্ণাঙ্ক		margin_width = 0;
    পূর্ণাঙ্ক		maxspace;
    পূর্ণাঙ্ক		center_vert = FALSE;
    পূর্ণাঙ্ক		center_hor = FALSE;
    পূর্ণাঙ্ক		allow_adjust_বাম = !wp->w_popup_fixed;
    পূর্ণাঙ্ক		শীৰ্ষ_extra = popup_শীৰ্ষ_extra(wp);
    পূর্ণাঙ্ক		ডান_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    পূর্ণাঙ্ক		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    পূর্ণাঙ্ক		বাম_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    পূর্ণাঙ্ক		extra_height = শীৰ্ষ_extra + bot_extra;
    পূর্ণাঙ্ক		extra_width = বাম_extra + ডান_extra;
    পূর্ণাঙ্ক		w_height_before_limit;
    popup_layout_T org_layout;
    পূর্ণাঙ্ক		minwidth, minheight;
    পূর্ণাঙ্ক		maxheight = Rows;
    পূর্ণাঙ্ক		wantline = wp->w_wantline;  // adjusted for textprop
    পূর্ণাঙ্ক		wantcol = wp->w_wantcol;    // adjusted for textprop
    পূর্ণাঙ্ক		use_wantcol = wantcol != 0;
    পূর্ণাঙ্ক		adjust_height_for_শীৰ্ষ_aligned = FALSE;

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
    যদি (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    যদি (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	পূর্ণাঙ্ক	    screen_row;
	পূর্ণাঙ্ক	    screen_scol;
	পূর্ণাঙ্ক	    screen_ccol;
	পূর্ণাঙ্ক	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_শীৰ্ষ = false;
	যদি (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    যদি (popup_find_prop_above_শীৰ্ষ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_শীৰ্ষ = true;
	    অন্যথা
	    {
		যদি ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    যদি (wp->w_popup_imagep != রিক্ত)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    যদি (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		ঘূৰাই;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	যদি (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	যদি (prop_above_শীৰ্ষ)
	    popup_screenpos_above_শীৰ্ষ(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	অন্যথা
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	যদি (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    ঘূৰাই;
	}
	যদি (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	অন্যথা
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	যদি (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	অন্যথা
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    অন্যথা
    {
	// If no line was specified default to vertical centering.
	যদি (wantline == 0)
	    center_vert = TRUE;
	অন্যথা যদি (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	যদি (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    যদি (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    অন্যথা
    {
	যদি (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    যদি (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	যদি (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_সাৰি_গুনতি - 1, 0);
	}

	যদি (!use_wantcol)
	    center_hor = TRUE;
	অন্যথা যদি (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    যদি (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- বাম_extra - ডান_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- বাম_extra - ডান_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    যদি (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - বাম_extra - ডান_extra;
    maxwidth = maxspace;
    যদি (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_বাম = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    যদি (wp->w_p_nu || wp->w_p_rnu)
	margin_width = সংখ্যা_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    যদি (signcolumn_on(wp))
	margin_width += 2;
#endif
    যদি (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    যদি (wp->w_buffer->b_term != রিক্ত)
    {
	যদি (minwidth == 0)
	    minwidth = 20;
	যদি (minheight == 0)
	    minheight = 5;
    }
#endif

    যদি (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    অন্যথা যদি (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    যদি (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	যদি (wp->w_firstline > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    wp->w_firstline = 0;
	অন্যথা
	    wp->w_topline = wp->w_firstline;
    }
    যদি (wp->w_topline < 1)
	wp->w_topline = 1;
    অন্যথা যদি (wp->w_topline > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	wp->w_topline = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;

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
	পূর্ণাঙ্ক saved_w_width = wp->w_width;

	যদি (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	বাবে (ln = 1; ln <= wp->w_buffer->b_ml.ml_সাৰি_গুনতি; ++ln)
	{
	    পূর্ণাঙ্ক len = linetabsize(wp, ln) + margin_width;

	    যদি (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    যদি (saved_w_width < len)
		saved_w_width = len;
	    যদি (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		ভঙ্গ;
	}
	wp->w_width = saved_w_width;
    }
    যদি (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
    অন্যথা
	lnum = wp->w_topline;
    যেতিয়ালৈ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
    {
	পূর্ণাঙ্ক len;
	পূর্ণাঙ্ক w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	যদি (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	যদি (len + margin_width > maxwidth
		&& allow_adjust_বাম
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    পূর্ণাঙ্ক shift_by = len + margin_width - maxwidth;

	    যদি (shift_by > wp->w_wincol)
	    {
		পূর্ণাঙ্ক truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    যদি (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    যদি (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	যদি (wp->w_p_wrap)
	{
	    যেতিয়ালৈ (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	যদি (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    যদি (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	যদি (wp->w_firstline < 0)
	    --lnum;
	অন্যথা
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	যদি (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_সাৰি_গুনতি - lnum)
		       + wrapped >= maxheight)
	    ভঙ্গ;
    }

    যদি (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_সাৰি_গুনতি);
#ifdef FEAT_TERMINAL
    যদি (wp->w_buffer->b_term != রিক্ত && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    যদি (wp->w_has_scrollbar)
    {
	++ডান_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	যদি (maxwidth + ডান_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    যদি (wp->w_popup_title != রিক্ত && *wp->w_popup_title != NUL)
    {
	পূর্ণাঙ্ক title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	যদি (minwidth < title_len)
	    minwidth = title_len;
    }

    যদি (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    যদি (wp->w_width > maxwidth)
    {
	যদি (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	যদি (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    পূর্ণাঙ্ক off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, ডান_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	অন্যথা
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - ডান_extra;
	}
    }
    যদি (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	যদি (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    অন্যথা যদি (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	পূর্ণাঙ্ক leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	যদি (leftoff >= 0)
	    wp->w_wincol = leftoff;
	অন্যথা যদি (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    যদি (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    যদি (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	পূর্ণাঙ্ক want_col = 0;

	// try to show the right border and any scrollbar
	want_col = বাম_extra + wp->w_width + ডান_extra;
	যদি (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    যদি (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_সাৰি_গুনতি - wp->w_topline
								 + 1 + wrapped;
    যদি (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    যদি (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    যদি (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    যদি (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	যদি (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    অন্যথা যদি (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	যদি ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	অন্যথা যদি (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	অন্যথা
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_শীৰ্ষ_aligned = TRUE;
	}
    }
    অন্যথা যদি (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	পূর্ণাঙ্ক check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	যদি (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    যদি (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    যদি (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	অন্যথা
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_শীৰ্ষ_aligned = TRUE;
	}
    }

    যদি (adjust_height_for_শীৰ্ষ_aligned && wp->w_want_scrollbar
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
	যদি (wp->w_buffer->b_term == রিক্ত || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    যদি (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		যদি (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		অন্যথা
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    যদি (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    অন্যথা যদি (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    যদি (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    যদি (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    যদি (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    যদি (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	পূর্ণাঙ্ক avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    যদি (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	যদি (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	যদি (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	যদি (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    পূর্ণাঙ্ক avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    যদি (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    যদি (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	যদি (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    যদি (wp->w_popup_imagep != রিক্ত)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	ঘূৰাই;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    যদি (win_valid(wp->w_popup_prop_win))
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
    যদি (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	যদি (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

প্রকারনির্দেশ গণনা
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
    স্থির পূর্ণাঙ্ক
popup_is_notification(create_type_T type)
{
    ঘূৰাই type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    স্থির শূন্য
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    পূর্ণাঙ্ক	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    বাবে (lnum = buf->b_ml.ml_সাৰি_গুনতি; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    যদি (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == রিক্ত ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    অন্যথা
    {
	list_T *l = text.vval.v_list;

	যদি (l != রিক্ত && l->lv_len > 0)
	{
	    যদি (l->lv_first == &range_list_item)
		emsg(_(e_using_সংখ্যা_as_string));
	    অন্যথা যদি (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    অন্যথা
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_সাৰি_গুনতি);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    কৰা {						    \
	যদি (wp != রিক্ত)					    \
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
    } যেতিয়ালৈ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    স্থির পূর্ণাঙ্ক
parse_popup_option(win_T *wp, পূর্ণাঙ্ক is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    পূর্ণাঙ্ক	    border_enabled = FALSE;

    যদি (wp != রিক্ত)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    বাবে ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	পূর্ণাঙ্ক	x;

	e = vim_strchr(p, ':');
	যদি (e == রিক্ত || e[1] == NUL)
	    ঘূৰাই FAIL;

	p = vim_strchr(e, ',');
	যদি (p == রিক্ত)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	যদি (STRNCMP(s, "height:", 7) == 0)
	{
	    যদি (dig != p)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
	    {
		যদি (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	অন্যথা যদি (STRNCMP(s, "width:", 6) == 0)
	{
	    যদি (dig != p)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
	    {
		যদি (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	অন্যথা যদি (STRNCMP(s, "highlight:", 10) == 0)
	{
	    যদি (wp != রিক্ত)
	    {
		ৱর্ণ *errmsg;
		পূর্ণাঙ্ক c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		যদি (errmsg == রিক্ত)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		অন্যথা
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	অন্যথা যদি (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    যদি (*arg == NUL || *arg == ',')
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
	    {
		বাবে (পূর্ণাঙ্ক i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	অন্যথা যদি (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    পূর্ণাঙ্ক		i;
	    পূর্ণাঙ্ক		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    পূর্ণাঙ্ক		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    যদি (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		যদি (wp != রিক্ত)
		{
		    বাবে (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		অনুৱর্ত;
	    }

	    token = vim_strnsave(arg, token_len);
	    যদি (token == রিক্ত)
		ঘূৰাই FAIL;

	    যদি ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		যদি (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		অন্যথা যদি (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		অন্যথা যদি (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		অন্যথা যদি (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		অন্যথা যদি (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		অন্যথা যদি (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    পূর্ণাঙ্ক		out[8];
		    পূর্ণাঙ্ক		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    বাবে (i = 0; i < 8 && !failed; i++)
		    {
			যদি (*q == NUL)
			    failed = TRUE;
			অন্যথা
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    যদি (i < 7)
			    {
				যদি (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    যদি (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			ঘূৰাই FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    অন্যথা
	    {
		vim_free(token);
		ঘূৰাই FAIL;
	    }

	    যদি (wp != রিক্ত)
	    {
		বাবে (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	অন্যথা যদি (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    পূর্ণাঙ্ক		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    পূর্ণাঙ্ক		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    যদি (!on && !off)
		ঘূৰাই FAIL;
	    on = on && mouse_has(MOUSE_ঢোকাও) && (border_enabled || is_preview);
	    যদি (wp != রিক্ত)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	অন্যথা যদি (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    পূর্ণাঙ্ক		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    পূর্ণাঙ্ক		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    যদি (!on && !off)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
	    {
		যদি (on && mouse_has(MOUSE_ঢোকাও))
		    wp->w_popup_flags |= POPF_RESIZE;
		অন্যথা
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	অন্যথা যদি (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    পূর্ণাঙ্ক		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    পূর্ণাঙ্ক		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    যদি (!on && !off)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	অন্যথা যদি (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    পূর্ণাঙ্ক		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    পূর্ণাঙ্ক		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    যদি (!menu && !item)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	অন্যথা যদি (STRNCMP(s, "opacity:", 8) == 0)
	{
	    যদি (dig != p || x < 0 || x > 100)
		ঘূৰাই FAIL;
	    যদি (wp != রিক্ত)
	    {
		যদি (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		অন্যথা
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	অন্যথা
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    পূর্ণাঙ্ক
parse_previewpopup(win_T *wp)
{
    ঘূৰাই parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    পূর্ণাঙ্ক
parse_completepopup(win_T *wp)
{
    ঘূৰাই parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    শূন্য
popup_set_wantpos_cursor(win_T *wp, পূর্ণাঙ্ক width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    যদি (d != রিক্ত)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    যদি (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    অন্যথা
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	যদি (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    যদি (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	যদি (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    শূন্য
popup_set_wantpos_rowcol(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    স্থির শূন্য
add_border_বাম_ডান_padding(win_T *wp)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    স্থির পূর্ণাঙ্ক
popup_terminal_exists(শূন্য)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_buffer->b_term != রিক্ত)
	    ঘূৰাই TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    যদি (wp->w_buffer->b_term != রিক্ত)
		ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    শূন্য
popup_redraw_all(শূন্য)
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
    পূর্ণাঙ্ক
popup_need_redraw(শূন্য)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	যদি ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ঘূৰাই TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Set the color for a notification window.
 */
    স্থির শূন্য
popup_update_color(win_T *wp, create_type_T type)
{
    ৱর্ণ    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    ৱর্ণ    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    যদি (errmsg == রিক্ত)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    অন্যথা
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    স্থির win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = রিক্ত;
    পূর্ণাঙ্ক		tabnr = 0;
    পূর্ণাঙ্ক		new_buffer;
    buf_T	*buf = রিক্ত;
    dict_T	*d = রিক্ত;
    পূর্ণাঙ্ক		i;

    যদি (check_secure())
	ঘূৰাই রিক্ত;

    যদি (argvars != রিক্ত)
    {
	যদি (in_vim9script()
		&& (check_for_string_or_সংখ্যা_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    ঘূৰাই রিক্ত;

	// Check that arguments look OK.
	যদি (argvars[0].v_type == VAR_সংখ্যা)
	{
	    buf = buflist_findnr(argvars[0].vval.v_সংখ্যা);
	    যদি (buf == রিক্ত)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_সংখ্যা);
		ঘূৰাই রিক্ত;
	    }
#ifdef FEAT_TERMINAL
	    যদি (buf->b_term != রিক্ত && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		ঘূৰাই রিক্ত;
	    }
#endif
	}
	অন্যথা যদি (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != রিক্ত)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != রিক্ত))
	{
	    emsg(_(e_buffer_সংখ্যা_text_or_list_required));
	    ঘূৰাই রিক্ত;
	}
	যদি (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    ঘূৰাই রিক্ত;
	d = argvars[1].vval.v_dict;
    }

    যদি (d != রিক্ত)
    {
	যদি (dict_has_চাবি(d, "tabpage"))
	    tabnr = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "tabpage");
	অন্যথা যদি (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	অন্যথা
	    tabnr = 0;
	যদি (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    যদি (tp == রিক্ত)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		ঘূৰাই রিক্ত;
	    }
	}
    }
    অন্যথা যদি (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    যদি (buf != রিক্ত && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	ঘূৰাই রিক্ত;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    যদি (wp == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি (rettv != রিক্ত)
	rettv->vval.v_সংখ্যা = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    যদি (buf != রিক্ত)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    অন্যথা
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(রিক্ত, রিক্ত, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	যদি (buf == রিক্ত)
	{
	    win_free_popup(wp);
	    ঘূৰাই রিক্ত;
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

    যদি (tp != রিক্ত)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    অন্যথা যদি (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    অন্যথা // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	যদি (first_popupwin == রিক্ত)
	    first_popupwin = wp;
	অন্যথা
	{
	    যেতিয়ালৈ (prev->w_next != রিক্ত)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    যদি (new_buffer && argvars != রিক্ত)
	popup_set_buffer_text(buf, argvars[0]);

    যদি (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    যদি (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_মানসমূহ(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    যদি (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	যদি (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_মানসমূহ(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    যদি (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	পূর্ণাঙ্ক	height = buf->b_ml.ml_সাৰি_গুনতি + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	বাবে (twp = first_popupwin; twp != রিক্ত; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    যদি (twp != wp
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
	যদি (wp->w_wantline + height > Rows)
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
	বাবে (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    যদি (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_বাম_ডান_padding(wp);
    }

    যদি (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	যদি (callback.cb_name != রিক্ত)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    বাবে (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    বাবে (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    যদি (type == TYPE_PREVIEW)
    {
	যদি (mouse_has(MOUSE_ঢোকাও))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	বাবে (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    যদি (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	যদি (mouse_has(MOUSE_ঢোকাও))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_বাম_ডান_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    যদি (d != রিক্ত)
    {
	// Deal with options.
	যদি (apply_options(wp, d, TRUE) == FAIL)
	{
	    (শূন্য)popup_close(wp->w_id, FALSE);
	    ঘূৰাই রিক্ত;
	}
    }

#ifdef FEAT_TIMERS
    যদি (popup_is_notification(type) && wp->w_popup_timer == রিক্ত)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    যদি (buf->b_term != রিক্ত)
	win_enter(wp, FALSE);
#endif

    ঘূৰাই wp;
}

/*
 * popup_clear()
 */
    শূন্য
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক force = FALSE;

    যদি (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
	force = (পূর্ণাঙ্ক)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    শূন্য
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    শূন্য
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    শূন্য
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    স্থির শূন্য
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_সংখ্যা;
    argv[0].vval.v_সংখ্যা = (varnumber_T)wp->w_id;

    যদি (result != রিক্ত && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    অন্যথা
    {
	argv[1].v_type = VAR_সংখ্যা;
	argv[1].vval.v_সংখ্যা = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    যদি (result != রিক্ত)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    স্থির শূন্য
back_to_prevwin(win_T *wp)
{
    যদি (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    অন্যথা
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    স্থির শূন্য
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    পূর্ণাঙ্ক id;

    যদি (!win_valid(wp))
       ঘূৰাই;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    যদি (wp == curwin && curbuf->b_term != রিক্ত)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    যদি (owp != curwin && owp->w_buffer->b_term != রিক্ত)
		ভঙ্গ;
	যদি (owp != রিক্ত)
	    win_enter(owp, FALSE);
	অন্যথা
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		যদি (owp != curwin && owp->w_buffer->b_term != রিক্ত)
		    ভঙ্গ;
	    যদি (owp != রিক্ত)
		win_enter(owp, FALSE);
	    অন্যথা
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    যদি (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	ঘূৰাই;
    }

    CHECK_CURBUF;
    যদি (wp->w_close_cb.cb_name != রিক্ত)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    শূন্য
popup_close_with_retval(win_T *wp, পূর্ণাঙ্ক retval)
{
    typval_T res;

    res.v_type = VAR_সংখ্যা;
    res.vval.v_সংখ্যা = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    শূন্য
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    স্থির শূন্য
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    যদি (wp != mouse_wp
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
    শূন্য
popup_handle_mouse_moved(শূন্য)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    পূর্ণাঙ্ক	    row = mouse_row;
    পূর্ণাঙ্ক	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    বাবে (wp = first_popupwin; wp != রিক্ত; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    বাবে (wp = curtab->tp_first_popupwin; wp != রিক্ত; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    স্থির শূন্য
filter_handle_drag(win_T *wp, পূর্ণাঙ্ক c, typval_T *rettv)
{
    পূর্ণাঙ্ক	row = mouse_row;
    পূর্ণাঙ্ক	col = mouse_col;

    যদি ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_চাবি(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_সংখ্যা = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    শূন্য
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    char_u	*চাবি;
    typval_T	res;
    পূর্ণাঙ্ক		c;
    linenr_T	old_lnum;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    id = tv_get_সংখ্যা(&argvars[0]);
    wp = win_id2wp(id);
    চাবি = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    যদি (wp == রিক্ত)
	ঘূৰাই;

    c = *চাবি;
    যদি (c == K_SPECIAL && চাবি[1] != NUL)
	c = TO_SPECIAL(চাবি[1], চাবি[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_TRUE;
    res.v_type = VAR_সংখ্যা;

    old_lnum = wp->w_cursor.lnum;
    যদি (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	যদি (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	অন্যথা
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
    }
    যদি (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	যদি (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	    ++wp->w_cursor.lnum;
	অন্যথা
	    wp->w_cursor.lnum = 1;
    }
    যদি (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	ঘূৰাই;
    }

    যদি (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_সংখ্যা = -1;
	popup_close_and_callback(wp, &res);
	ঘূৰাই;
    }
    যদি (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_সংখ্যা = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	ঘূৰাই;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    শূন্য
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    char_u	*চাবি;
    typval_T	res;
    পূর্ণাঙ্ক		c;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    id = tv_get_সংখ্যা(&argvars[0]);
    wp = win_id2wp(id);
    চাবি = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    যদি (wp == রিক্ত)
	ঘূৰাই;

    c = *চাবি;
    যদি (c == CAR && need_wait_return)
	ঘূৰাই;
    যদি (c == K_SPECIAL && চাবি[1] != NUL)
	c = TO_SPECIAL(চাবি[1], চাবি[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_TRUE;

    যদি (c == 'y' || c == 'Y')
	res.vval.v_সংখ্যা = 1;
    অন্যথা যদি (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_সংখ্যা = 0;
    অন্যথা
    {
	filter_handle_drag(wp, c, rettv);
	ঘূৰাই;
    }

    // Invoke callback
    res.v_type = VAR_সংখ্যা;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    শূন্য
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    শূন্য
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    শূন্য
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    স্থির win_T *
find_popup_win(পূর্ণাঙ্ক id)
{
    win_T *wp = win_id2wp(id);

    যদি (wp != রিক্ত && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই wp;
}

/*
 * popup_close({id})
 */
    শূন্য
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    যদি (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == রিক্ত &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	ঘূৰাই;

    wp = find_popup_win(id);
    যদি (wp != রিক্ত)
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
    স্থির শূন্য
popup_clear_mask_for(win_T *wp)
{
    পূর্ণাঙ্ক r, c;
    পূর্ণাঙ্ক row_start, col_start, row_end, col_end;

    যদি (popup_mask == রিক্ত || !popup_visible)
	ঘূৰাই;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (পূর্ণাঙ্ক)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (পূর্ণাঙ্ক)screen_Columns);

    বাবে (r = row_start; r < row_end; ++r)
	বাবে (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    শূন্য
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    পূর্ণাঙ্ক			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    যদি (error_if_term_popup_window())
	ঘূৰাই;
#endif
    যদি ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ঘূৰাই;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    যদি (wp->w_popup_imagep != রিক্ত)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    যদি (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    যদি (was_visible)
	popup_clear_mask_for(wp);

    যদি (old_area.active)
	popup_redraw_exposed_area(&old_area);
    অন্যথা
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    যদি (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    শূন্য
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    শূন্য
popup_show(win_T *wp)
{
    bool popup_active;

    যদি ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	ঘূৰাই;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    যদি (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_সাৰিগুলি_valid = 0;
	যদি (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    অন্যথা
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    শূন্য
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;

    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    যদি (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_সংখ্যা = 0;
}

/*
 * popup_settext({id}, {text})
 */
    শূন্য
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    popup_area_T	old_area;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;

    popup_save_area(wp, &old_area);

    যদি (check_for_string_or_list_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_সাৰিগুলি_valid = 0;
    যদি (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    যদি (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    শূন্য
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণাঙ্ক			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_FALSE;

    যদি (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;

    buf = tv_get_buf_from_arg(&argvars[1]);

    যদি (buf == রিক্ত)
	ঘূৰাই;
#ifdef FEAT_TERMINAL
    যদি (buf->b_term != রিক্ত && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	ঘূৰাই;
    }
#endif

    যদি (wp->w_buffer != buf)
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

	যদি (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_সংখ্যা = VVAL_TRUE;
}

    স্থির শূন্য
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    পূর্ণাঙ্ক			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    যদি (wp->w_popup_imagep != রিক্ত)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    যদি (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    যদি (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    যদি (wp == message_win)
	message_win = রিক্ত;
#endif

    যদি (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    যদি (!old_area.active)
	popup_mask_refresh = TRUE;
}

    স্থির শূন্য
error_for_popup_window(শূন্য)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    পূর্ণাঙ্ক
error_if_popup_window(পূর্ণাঙ্ক also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    যদি (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == রিক্ত)
#endif
	    )
    {
	error_for_popup_window();
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    পূর্ণাঙ্ক
popup_close(পূর্ণাঙ্ক id, পূর্ণাঙ্ক force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = রিক্ত;

    // go through global popups
    বাবে (wp = first_popupwin; wp != রিক্ত; prev = wp, wp = wp->w_next)
	যদি (wp->w_id == id)
	{
	    যদি (wp == curwin)
	    {
		যদি (!force)
		{
		    error_for_popup_window();
		    ঘূৰাই FAIL;
		}
		back_to_prevwin(wp);
	    }
	    যদি (prev == রিক্ত)
		first_popupwin = wp->w_next;
	    অন্যথা
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ঘূৰাই OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	যদি (popup_close_tabpage(tp, id, force) == OK)
	    ঘূৰাই OK;
    ঘূৰাই FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    পূর্ণাঙ্ক
popup_close_tabpage(tabpage_T *tp, পূর্ণাঙ্ক id, পূর্ণাঙ্ক force)
{
    win_T	*wp;
    win_T	**মূল = &tp->tp_first_popupwin;
    win_T	*prev = রিক্ত;

    বাবে (wp = *মূল; wp != রিক্ত; prev = wp, wp = wp->w_next)
	যদি (wp->w_id == id)
	{
	    যদি (wp == curwin)
	    {
		যদি (!force)
		{
		    error_for_popup_window();
		    ঘূৰাই FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    যদি (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    যদি (prev == রিক্ত)
		*মূল = wp->w_next;
	    অন্যথা
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ঘূৰাই OK;
	}
    ঘূৰাই FAIL;
}

    শূন্য
close_all_popups(পূর্ণাঙ্ক force)
{
    যদি (!force && ERROR_IF_ANY_POPUP_WINDOW)
	ঘূৰাই;
    যেতিয়ালৈ (first_popupwin != রিক্ত)
	যদি (popup_close(first_popupwin->w_id, force) == FAIL)
	    ঘূৰাই;
    যেতিয়ালৈ (curtab->tp_first_popupwin != রিক্ত)
	যদি (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    ঘূৰাই;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    স্থির শূন্য
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
    স্থির শূন্য
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    পূর্ণাঙ্ক i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    বাবে (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    স্থির bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    পূর্ণাঙ্ক i;

    যদি (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	ঘূৰাই true;
    বাবে (i = 0; i < 4; i++)
	যদি (style->border_highlight[i] != wp->w_border_highlight[i])
	    ঘূৰাই true;
    ঘূৰাই false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    স্থির শূন্য
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
    স্থির bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    ঘূৰাই layout->winrow != wp->w_winrow
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
    স্থির bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    ঘূৰাই area->winrow != wp->w_winrow
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
    স্থির শূন্য
mark_overlapped_opacity_popup(win_T *wp, পূর্ণাঙ্ক area_শীৰ্ষ, পূর্ণাঙ্ক area_bot,
	পূর্ণাঙ্ক area_বাম, পূর্ণাঙ্ক area_ডান, পূর্ণাঙ্ক zindex)
{
    যদি ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_শীৰ্ষ
	    || wp->w_wincol >= area_ডান
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_বাম)
	ঘূৰাই;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_সাৰিগুলি_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    স্থির শূন্য
redraw_overlapped_opacity_popups(পূর্ণাঙ্ক winrow, পূর্ণাঙ্ক wincol, পূর্ণাঙ্ক height, পূর্ণাঙ্ক width,
	পূর্ণাঙ্ক leftoff, পূর্ণাঙ্ক zindex)
{
    win_T	*wp;
    পূর্ণাঙ্ক		area_শীৰ্ষ = winrow;
    পূর্ণাঙ্ক		area_bot = winrow + height;
    পূর্ণাঙ্ক		area_বাম = wincol;
    পূর্ণাঙ্ক		area_ডান = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_শীৰ্ষ, area_bot, area_বাম,
		area_ডান, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_শীৰ্ষ, area_bot, area_বাম,
		area_ডান, zindex);

    যদি (must_redraw < UPD_VALID)
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
    স্থির শূন্য
refill_pum_bg_row_from_buffer(পূর্ণাঙ্ক r, পূর্ণাঙ্ক বাম, পূর্ণাঙ্ক ডান)
{
    পূর্ণাঙ্ক		সাৰি_cp = r;
    পূর্ণাঙ্ক		col_cp = বাম;
    win_T	*wp;
    linenr_T	lnum;
    পূর্ণাঙ্ক		row_for_lnum;
    পূর্ণাঙ্ক		col_for_lnum = 0;
    char_u	*সাৰি;
    char_u	*p;
    পূর্ণাঙ্ক		win_text_col;
    পূর্ণাঙ্ক		screen_col;
    পূর্ণাঙ্ক		soff_base = (r - pum_bg_শীৰ্ষ) * pum_bg_cols;
    পূর্ণাঙ্ক		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    বাবে (c = বাম; c < ডান; ++c)
    {
	pum_bg_সাৰিগুলি[soff_base + c] = ' ';
	যদি (pum_bg_attrs != রিক্ত)
	    pum_bg_attrs[soff_base + c] = 0;
	যদি (enc_utf8 && pum_bg_সাৰিগুলি_UC != রিক্ত)
	    pum_bg_সাৰিগুলি_UC[soff_base + c] = 0;
	যদি (enc_utf8)
	{
	    পূর্ণাঙ্ক k;
	    বাবে (k = 0; k < MAX_MCO; ++k)
		যদি (pum_bg_সাৰিগুলি_C[k] != রিক্ত)
		    pum_bg_সাৰিগুলি_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&সাৰি_cp, &col_cp, IGNORE_POPUP);
    যদি (wp == রিক্ত || সাৰি_cp < 0 || সাৰি_cp >= wp->w_height)
	ঘূৰাই;
    যদি (wp->w_buffer == রিক্ত || wp->w_buffer->b_ml.ml_mfp == রিক্ত)
	ঘূৰাই;

    // Compute the buffer line for this screen row.
    row_for_lnum = সাৰি_cp;
    যদি (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, রিক্ত))
	ঘূৰাই;	// past end of buffer
    যদি (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	ঘূৰাই;

    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
    যদি (সাৰি == রিক্ত)
	ঘূৰাই;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    যদি (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = সাৰি;
    যেতিয়ালৈ (*p != NUL && screen_col < ডান)
    {
	পূর্ণাঙ্ক char_cells;
	পূর্ণাঙ্ক byte_গুনতি;
	পূর্ণাঙ্ক soff = soff_base + screen_col;
	পূর্ণাঙ্ক in_range = (screen_col >= বাম && screen_col < ডান);

	যদি (*p == '\t')
	{
	    পূর্ণাঙ্ক ts = (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_গুনতি = 1;
	    যদি (in_range)
	    {
		pum_bg_সাৰিগুলি[soff] = ' ';
		যদি (enc_utf8 && pum_bg_সাৰিগুলি_UC != রিক্ত)
		    pum_bg_সাৰিগুলি_UC[soff] = 0;
	    }
	}
	অন্যথা যদি (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_গুনতি = mb_ptr2len(p);
	    যদি (in_range)
	    {
		pum_bg_সাৰিগুলি[soff] = *p;
		যদি (enc_utf8 && pum_bg_সাৰিগুলি_UC != রিক্ত)
		    pum_bg_সাৰিগুলি_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	অন্যথা
	{
	    char_cells = 1;
	    byte_গুনতি = 1;
	    যদি (in_range)
	    {
		pum_bg_সাৰিগুলি[soff] = (*p < 0x20) ? ' ' : *p;
		যদি (enc_utf8 && pum_bg_সাৰিগুলি_UC != রিক্ত)
		    pum_bg_সাৰিগুলি_UC[soff] = 0;
	    }
	}
	যদি (in_range && pum_bg_attrs != রিক্ত)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_গুনতি;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    স্থির শূন্য
popup_redraw_exposed_area(popup_area_T *area)
{
    যদি (!area->active)
	ঘূৰাই;

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
    যদি (pum_bg_সাৰিগুলি != রিক্ত
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_শীৰ্ষ)
    {
	পূর্ণাঙ্ক শীৰ্ষ = MAX(area->winrow, pum_bg_শীৰ্ষ);
	পূর্ণাঙ্ক bot = MIN(area->winrow + area->height, pum_bg_bot);
	পূর্ণাঙ্ক বাম = MAX(area->wincol, 0);
	পূর্ণাঙ্ক ডান = MIN(area->wincol + area->width, pum_bg_cols);
	পূর্ণাঙ্ক r;

	বাবে (r = শীৰ্ষ; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, বাম, ডান);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    স্থির শূন্য
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->সাৰিগুলি);
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
    স্থির শূন্য
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    যদি (screen_opacity_popup == রিক্ত
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	ঘূৰাই;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    যদি (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->সাৰিগুলি = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(পূর্ণাঙ্ক,
				  saved_screen->rows * saved_screen->cols);
    যদি (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    যদি (saved_screen->সাৰিগুলি == রিক্ত || saved_screen->attrs == রিক্ত)
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক r = 0; r < saved_screen->rows; r++)
    {
	পূর্ণাঙ্ক screen_row = saved_screen->start_row + r;

	যদি (screen_row >= 0 && screen_row < screen_Rows)
	    বাবে (পূর্ণাঙ্ক c = 0; c < saved_screen->cols; c++)
	    {
		পূর্ণাঙ্ক screen_col = saved_screen->start_col + c;

		যদি (screen_col >= 0 && screen_col < screen_Columns)
		{
		    পূর্ণাঙ্ক off = সাৰি_Offset[screen_row] + screen_col;
		    পূর্ণাঙ্ক save_off = r * saved_screen->cols + c;

		    saved_screen->সাৰিগুলি[save_off] = Screen_সাৰিগুলি[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    যদি (enc_utf8 && saved_screen->linesuc != রিক্ত)
			saved_screen->linesuc[save_off] = Screen_সাৰিগুলি_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    স্থির শূন্য
redraw_under_popup_area(পূর্ণাঙ্ক winrow, পূর্ণাঙ্ক wincol, পূর্ণাঙ্ক height, পূর্ণাঙ্ক width, পূর্ণাঙ্ক leftoff)
{
    পূর্ণাঙ্ক	    r;

    বাবে (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	পূর্ণাঙ্ক	    c;
	win_T	    *prev_twp = রিক্ত;

	যদি (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    অনুৱর্ত;
	}

	বাবে (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    পূর্ণাঙ্ক	    সাৰি_cp = r;
	    পূর্ণাঙ্ক	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&সাৰি_cp, &col_cp, IGNORE_POPUP);
	    যদি (twp != রিক্ত && twp != prev_twp)
	    {
		prev_twp = twp;
		যদি (সাৰি_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (শূন্য)mouse_comp_pos(twp, &সাৰি_cp, &col_cp, &lnum, রিক্ত);
		    redrawWinline(twp, lnum);
		}
		অন্যথা যদি (সাৰি_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    শূন্য
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    popup_area_T	old_area;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;  // invalid {id}

    যদি (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ঘূৰাই;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    যদি (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    যদি (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    শূন্য
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    পূর্ণাঙ্ক		need_redraw = FALSE;
    পূর্ণাঙ্ক		need_reposition = FALSE;

    যদি (check_secure())
	ঘূৰাই;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;  // invalid {id}

    যদি (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ঘূৰাই;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (শূন্য)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    যদি (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    যদি (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    অন্যথা যদি (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    যদি (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    যদি (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    অন্যথা যদি (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_সাৰিগুলি_valid = 0;
	যদি (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    যদি (old_style.blend != wp->w_popup_blend)
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

    যদি (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    শূন্য
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    পূর্ণাঙ্ক		শীৰ্ষ_extra;
    পূর্ণাঙ্ক		বাম_extra;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;  // invalid {id}
    শীৰ্ষ_extra = popup_শীৰ্ষ_extra(wp);
    বাম_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_সংখ্যা(dict, "line", wp->w_winrow + 1);
    dict_add_সংখ্যা(dict, "col", wp->w_wincol + 1);
    dict_add_সংখ্যা(dict, "width", wp->w_width + বাম_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_সংখ্যা(dict, "height", wp->w_height + শীৰ্ষ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_সংখ্যা(dict, "core_line", wp->w_winrow + 1 + শীৰ্ষ_extra);
    dict_add_সংখ্যা(dict, "core_col", wp->w_wincol + 1 + বাম_extra);
    dict_add_সংখ্যা(dict, "core_width", wp->w_width);
    dict_add_সংখ্যা(dict, "core_height", wp->w_height);

    dict_add_সংখ্যা(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_সংখ্যা(dict, "firstline", wp->w_topline);
    dict_add_সংখ্যা(dict, "lastline", wp->w_botline - 1);
    dict_add_সংখ্যা(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    শূন্য
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;
    FOR_ALL_POPUPWINS(wp)
	list_append_সংখ্যা(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_সংখ্যা(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    শূন্য
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		row;
    পূর্ণাঙ্ক		col;
    win_T	*wp;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    row = tv_get_সংখ্যা(&argvars[0]) - 1;
    col = tv_get_সংখ্যা(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    যদি (wp != রিক্ত && WIN_IS_POPUP(wp))
	rettv->vval.v_সংখ্যা = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    স্থির শূন্য
get_padding_border(dict_T *dict, পূর্ণাঙ্ক *array, ৱর্ণ *name)
{
    list_T  *list;
    পূর্ণাঙ্ক	    i;

    যদি (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	ঘূৰাই;

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    যদি (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	ঘূৰাই;
    }
    যদি (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	বাবে (i = 0; i < 4; ++i)
	    list_append_সংখ্যা(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    স্থির শূন্য
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    পূর্ণাঙ্ক	    i;

    বাবে (i = 0; i < 4; ++i)
	যদি (wp->w_border_highlight[i] != রিক্ত)
	    ভঙ্গ;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    যদি (i == 4 && !wp->w_border_highlight_isset)
	ঘূৰাই;

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    যদি (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	ঘূৰাই;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    যদি (i == 4)
	ঘূৰাই;
    বাবে (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    স্থির শূন্য
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    পূর্ণাঙ্ক	    i;
    char_u  buf[NUMBUFLEN];
    পূর্ণাঙ্ক	    len;

    বাবে (i = 0; i < 8; ++i)
	যদি (wp->w_border_char[i] != 0)
	    ভঙ্গ;
    যদি (i == 8)
	ঘূৰাই;

    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    যদি (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	ঘূৰাই;
    }
    বাবে (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    স্থির শূন্য
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    যদি (list != রিক্ত)
    {
	যদি (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	অন্যথা
	{
	    list_append_সংখ্যা(list, wp->w_popup_lnum);
	    list_append_সংখ্যা(list, wp->w_popup_mincol);
	    list_append_সংখ্যা(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    যদি (list == রিক্ত)
	ঘূৰাই;

    যদি (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	ঘূৰাই;
    }
    list_append_সংখ্যা(list, wp->w_popup_mouse_row);
    list_append_সংখ্যা(list, wp->w_popup_mouse_mincol);
    list_append_সংখ্যা(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    শূন্য
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    পূর্ণাঙ্ক		id;
    win_T	*wp;
    tabpage_T	*tp;
    পূর্ণাঙ্ক		i;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = find_popup_win(id);
    যদি (wp == রিক্ত)
	ঘূৰাই;

    dict = rettv->vval.v_dict;
    dict_add_সংখ্যা(dict, "line", wp->w_wantline);
    dict_add_সংখ্যা(dict, "col", wp->w_wantcol);
    dict_add_সংখ্যা(dict, "minwidth", wp->w_minwidth);
    dict_add_সংখ্যা(dict, "minheight", wp->w_minheight);
    dict_add_সংখ্যা(dict, "maxheight", wp->w_maxheight);
    dict_add_সংখ্যা(dict, "maxwidth", wp->w_maxwidth);
    dict_add_সংখ্যা(dict, "firstline", wp->w_firstline);
    dict_add_সংখ্যা(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_সংখ্যা(dict, "zindex", wp->w_zindex);
    dict_add_সংখ্যা(dict, "fixed", wp->w_popup_fixed);
    যদি (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	যদি (pt != রিক্ত)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_সংখ্যা(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_সংখ্যা(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    যদি (wp->w_popup_imagep != রিক্ত && wp->w_popup_imagep->img != রিক্ত)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	যদি (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_সংখ্যা(dict, "wrap", wp->w_p_wrap);
    dict_add_সংখ্যা(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_সংখ্যা(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_সংখ্যা(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_সংখ্যা(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_সংখ্যা(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_সংখ্যা(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_সংখ্যা(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_সংখ্যা(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    যদি (wp->w_scrollbar_highlight != রিক্ত)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    যদি (wp->w_thumb_highlight != রিক্ত)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    যদি (twp->w_id == id)
		ভঙ্গ;
	যদি (twp != রিক্ত)
	    ভঙ্গ;
	++i;
    }
    যদি (tp == রিক্ত)
	i = -1;  // must be global
    অন্যথা যদি (tp == curtab)
	i = 0;
    dict_add_সংখ্যা(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    যদি (wp->w_filter_cb.cb_name != রিক্ত)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    যদি (wp->w_close_cb.cb_name != রিক্ত)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(poppos_entries); ++i)
	যদি (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(পূর্ণাঙ্ক)poppos_entries[i].pp_name.length);
	    ভঙ্গ;
	}

    যদি (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    অন্যথা যদি (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    অন্যথা
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#যদি defined(FEAT_TIMERS)
    dict_add_সংখ্যা(dict, "time", wp->w_popup_timer != রিক্ত
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#যদি defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    পূর্ণাঙ্ক
error_if_term_popup_window(শূন্য)
{
    যদি (WIN_IS_POPUP(curwin) && curbuf->b_term != রিক্ত
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    শূন্য
popup_reset_handled(পূর্ণাঙ্ক handled_flag)
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
find_next_popup(পূর্ণাঙ্ক lowest, পূর্ণাঙ্ক handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    পূর্ণাঙ্ক	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = রিক্ত;
    FOR_ALL_POPUPWINS(wp)
	যদি ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    যদি (found_wp != রিক্ত)
	found_wp->w_popup_handled |= handled_flag;
    ঘূৰাই found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    স্থির পূর্ণাঙ্ক
invoke_popup_filter(win_T *wp, পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    পূর্ণাঙ্ক		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    যদি (c == Ctrl_C)
    {
	পূর্ণাঙ্ক save_got_int = got_int;
	পূর্ণাঙ্ক was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	যদি (was_curwin && wp == curwin)
	    ঘূৰাই -1;
	ঘূৰাই TRUE;
    }

    argv[0].v_type = VAR_সংখ্যা;
    argv[0].vval.v_সংখ্যা = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    যদি (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    অন্যথা
    {
	যদি (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	যদি (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    যদি (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	অন্যথা
	{
	    যদি (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    ঘূৰাই res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    পূর্ণাঙ্ক
popup_do_filter(পূর্ণাঙ্ক c)
{
    স্থির পূর্ণাঙ্ক	recursive = FALSE;
    পূর্ণাঙ্ক		res = FALSE;
    win_T	*wp;
    পূর্ণাঙ্ক		save_চাবি_Typed = চাবি_Typed;
    পূর্ণাঙ্ক		state;
    পূর্ণাঙ্ক		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    যদি (popup_is_popup(curwin) && curbuf->b_term != রিক্ত)
	ঘূৰাই FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    যদি (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	ঘূৰাই FALSE;

    যদি (recursive)
	ঘূৰাই FALSE;
    recursive = TRUE;

    যদি (c == K_LEFTMOUSE)
    {
	পূর্ণাঙ্ক row = mouse_row;
	পূর্ণাঙ্ক col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	যদি (wp != রিক্ত && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    যেতিয়ালৈ (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != রিক্ত)
	যদি (wp->w_filter_cb.cb_name != রিক্ত
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    যদি (must_redraw > was_must_redraw)
    {
	পূর্ণাঙ্ক save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    চাবি_Typed = save_চাবি_Typed;

    // When interrupted return FALSE to avoid looping.
    ঘূৰাই res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    পূর্ণাঙ্ক
popup_no_mapping(শূন্য)
{
    পূর্ণাঙ্ক	    round;
    win_T   *wp;

    বাবে (round = 1; round <= 2; ++round)
	বাবে (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != রিক্ত; wp = wp->w_next)
	    যদি (wp->w_filter_cb.cb_name != রিক্ত
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    শূন্য
popup_check_cursor_pos(শূন্য)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    যেতিয়ালৈ ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != রিক্ত)
	যদি (wp->w_popup_curwin != রিক্ত
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    স্থির শূন্য
popup_update_mask(win_T *wp, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    পূর্ণাঙ্ক		row, col;

    যদি (wp->w_popup_mask == রিক্ত || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	ঘূৰাই;
    }
    যদি (wp->w_popup_mask_cells != রিক্ত
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	ঘূৰাই;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    যদি (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = রিক্ত;
	ঘূৰাই;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    যদি (wp->w_popup_mask_cells == রিক্ত)
	ঘূৰাই;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_বস্তুসমূহ(wp->w_popup_mask, lio)
    {
	পূর্ণাঙ্ক cols, cole;
	পূর্ণাঙ্ক সাৰিগুলি, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_সংখ্যা(&li->li_tv);
	যদি (cols < 0)
	    cols = width + cols + 1;
	যদি (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_সংখ্যা(&li->li_tv);
	যদি (cole < 0)
	    cole = width + cole + 1;
	যদি (cole > width)
	    cole = width;
	li = li->li_next;
	সাৰিগুলি = tv_get_সংখ্যা(&li->li_tv);
	যদি (সাৰিগুলি < 0)
	    সাৰিগুলি = height + সাৰিগুলি + 1;
	যদি (সাৰিগুলি <= 0)
	    সাৰিগুলি = 1;
	li = li->li_next;
	linee = tv_get_সংখ্যা(&li->li_tv);
	যদি (linee < 0)
	    linee = height + linee + 1;
	যদি (linee > height)
	    linee = height;

	বাবে (row = সাৰিগুলি - 1; row < linee; ++row)
	    বাবে (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    স্থির পূর্ণাঙ্ক
popup_masked(win_T *wp, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height, পূর্ণাঙ্ক screencol, পূর্ণাঙ্ক screenline)
{
    পূর্ণাঙ্ক col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    পূর্ণাঙ্ক সাৰি = screenline - wp->w_winrow;

    ঘূৰাই col >= 0 && col < width
	    && সাৰি >= 0 && সাৰি < height
	    && wp->w_popup_mask_cells[সাৰি * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    স্থির শূন্য
update_popup_transparent(win_T *wp, পূর্ণাঙ্ক val)
{
    যদি (wp->w_popup_mask == রিক্ত)
	ঘূৰাই;

    পূর্ণাঙ্ক		width = popup_width(wp);
    পূর্ণাঙ্ক		height = popup_height(wp);
    listitem_T	*lio, *li;
    পূর্ণাঙ্ক		cols, cole;
    পূর্ণাঙ্ক		সাৰিগুলি, linee;
    পূর্ণাঙ্ক		col, সাৰি;

    FOR_ALL_LIST_বস্তুসমূহ(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_সংখ্যা(&li->li_tv);
	যদি (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_সংখ্যা(&li->li_tv);
	যদি (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	সাৰিগুলি = tv_get_সংখ্যা(&li->li_tv);
	যদি (সাৰিগুলি < 0)
	    সাৰিগুলি = height + সাৰিগুলি + 1;
	li = li->li_next;
	linee = tv_get_সংখ্যা(&li->li_tv);
	যদি (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	যদি (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--সাৰিগুলি;
	যদি (সাৰিগুলি < 0)
	    সাৰিগুলি = 0;
	বাবে (সাৰি = সাৰিগুলি; সাৰি < linee
		&& সাৰি + wp->w_winrow < screen_Rows; ++সাৰি)
	    বাবে (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(সাৰি + wp->w_winrow) * screen_Columns
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
    স্থির পূর্ণাঙ্ক
check_popup_unhidden(win_T *wp)
{
    যদি (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	যদি ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    ঘূৰাই FALSE;
	যদি (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	অন্যথা যদি (popup_find_prop_above_শীৰ্ষ(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	যদি (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    স্থির পূর্ণাঙ্ক
popup_need_position_adjust(win_T *wp)
{
    যদি (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	ঘূৰাই TRUE;
    যদি (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	ঘূৰাই TRUE;

    // May need to adjust the width if the cursor moved.
    ঘূৰাই wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
স্থির short *opacity_zindex = রিক্ত;
স্থির পূর্ণাঙ্ক    opacity_zindex_rows = 0;
স্থির পূর্ণাঙ্ক    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    স্থির শূন্য
popup_mark_opacity_zindex(win_T *wp)
{
    পূর্ণাঙ্ক	    width;
    পূর্ণাঙ্ক	    height;
    পূর্ণাঙ্ক	    r, c;

    যদি (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ঘূৰাই;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    বাবে (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	বাবে (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    পূর্ণাঙ্ক off = r * screen_Columns + c;
	    যদি (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    স্থির শূন্য
redraw_win_under_opacity_popup(win_T *wp)
{
    পূর্ণাঙ্ক	    height;
    পূর্ণাঙ্ক	    width;
    পূর্ণাঙ্ক	    r;

    যদি (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ঘূৰাই;

    height = popup_height(wp);
    width = popup_width(wp);
    বাবে (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	পূর্ণাঙ্ক	    col;
	win_T	    *prev_twp = রিক্ত;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	বাবে (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    পূর্ণাঙ্ক	    সাৰি_cp = r;
	    পূর্ণাঙ্ক	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&সাৰি_cp, &col_cp, IGNORE_POPUP);
	    যদি (twp != রিক্ত && twp != prev_twp)
	    {
		prev_twp = twp;
		যদি (সাৰি_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    যদি (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    অন্যথা
#endif
		    {
			(শূন্য)mouse_comp_pos(twp, &সাৰি_cp, &col_cp, &lnum,
									রিক্ত);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		অন্যথা যদি (সাৰি_cp == twp->w_height)
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
    পূর্ণাঙ্ক
popup_is_under_opacity(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (opacity_zindex == রিক্ত
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	ঘূৰাই FALSE;
    ঘূৰাই opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    পূর্ণাঙ্ক
popup_is_over_opacity(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	যদি ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ঘূৰাই TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    পূর্ণাঙ্ক
popup_is_under_opacity_range(পূর্ণাঙ্ক row, পূর্ণাঙ্ক start_col, পূর্ণাঙ্ক end_col)
{
    পূর্ণাঙ্ক col;

    যদি (opacity_zindex == রিক্ত
	    || row < 0 || row >= opacity_zindex_rows)
	ঘূৰাই FALSE;
    বাবে (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	যদি (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    শূন্য
may_update_popup_mask(পূর্ণাঙ্ক type)
{
    win_T	*wp;
    short	*mask;
    পূর্ণাঙ্ক		সাৰি, col;
    পূর্ণাঙ্ক		redraw_all_popups = FALSE;
    পূর্ণাঙ্ক		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    যদি (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	অন্যথা যদি (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	অন্যথা যদি (popup_need_position_adjust(wp))
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
	পূর্ণাঙ্ক has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    যদি ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    যদি ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	যদি (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	অন্যথা
	{
	    যদি (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    যদি (opacity_zindex != রিক্ত)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * পরিমাণ(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    যদি (!popup_mask_refresh)
	ঘূৰাই;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    যদি (redrawing_all_win)
	mask = popup_mask;
    অন্যথা
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * পরিমাণ(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    যেতিয়ালৈ ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != রিক্ত)
    {
	পূর্ণাঙ্ক width;
	পূর্ণাঙ্ক height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	যদি (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    যদি (wp->w_popup_flags & POPF_HIDDEN)
		অনুৱর্ত;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	যদি (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	অন্যথা
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	যদি ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    অনুৱর্ত;

	{
	    পূর্ণাঙ্ক mask_start = wp->w_winrow;
	    পূর্ণাঙ্ক mask_end = mask_start + height;
	    পূর্ণাঙ্ক mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    পূর্ণাঙ্ক mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    বাবে (সাৰি = mask_start;
			    সাৰি < mask_end && সাৰি < screen_Rows; ++সাৰি)
		বাবে (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    যদি (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(সাৰি, col, FALSE))
			mask[সাৰি * screen_Columns + col] = POPUPMENU_ZINDEX;
		    অন্যথা যদি (wp->w_popup_mask_cells == রিক্ত
				    || !popup_masked(wp, width, height, col, সাৰি))
			mask[সাৰি * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    যদি (mask == popup_mask_next)
    {
	পূর্ণাঙ্ক	    *plines_cache = ALLOC_CLEAR_MULT(পূর্ণাঙ্ক, Rows);
	win_T	    *prev_wp = রিক্ত;

	বাবে (সাৰি = 0; সাৰি < screen_Rows; ++সাৰি)
	{
	    পূর্ণাঙ্ক	    col_done = 0;

	    বাবে (col = 0; col < screen_Columns; ++col)
	    {
		পূর্ণাঙ্ক off = সাৰি * screen_Columns + col;

		যদি (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    যদি (সাৰি >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			যদি (!বাৰ্তা_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    অন্যথা যদি (col >= col_done)
		    {
			linenr_T	lnum;
			পূর্ণাঙ্ক		সাৰি_cp = সাৰি;
			পূর্ণাঙ্ক		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&সাৰি_cp, &col_cp, IGNORE_POPUP);
			যদি (wp != রিক্ত)
			{
#যদি defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    যদি (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    অন্যথা
#endif
			    {
				যদি (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   পরিমাণ(পূর্ণাঙ্ক) * Rows);
				    prev_wp = wp;
				}

				যদি (সাৰি_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				অন্যথা
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (শূন্য)mouse_comp_pos(wp, &সাৰি_cp, &col_cp,
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
    শূন্য
may_update_popup_position(শূন্য)
{
    যদি (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
স্থির schar_T *base_screenlines = রিক্ত;
স্থির পূর্ণাঙ্ক *base_screenattrs = রিক্ত;
স্থির u8char_T *base_screenlinesuc = রিক্ত;
স্থির পূর্ণাঙ্ক base_screen_rows = 0;
স্থির পূর্ণাঙ্ক base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    পূর্ণাঙ্ক
popup_get_base_screen_cell(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, schar_T *linep, পূর্ণাঙ্ক *attrp,
							  u8char_T *ucp)
{
    যদি (base_screenlines == রিক্ত || base_screenattrs == রিক্ত)
	ঘূৰাই FALSE;
    যদি (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ঘূৰাই FALSE;

    পূর্ণাঙ্ক off = row * base_screen_cols + col;
    যদি (linep != রিক্ত)
	*linep = base_screenlines[off];
    যদি (attrp != রিক্ত)
	*attrp = base_screenattrs[off];
    যদি (ucp != রিক্ত)
    {
	যদি (enc_utf8 && base_screenlinesuc != রিক্ত)
	    *ucp = base_screenlinesuc[off];
	অন্যথা
	    *ucp = 0;
    }
    ঘূৰাই TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    শূন্য
popup_set_base_screen_cell(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, schar_T সাৰি, পূর্ণাঙ্ক attr, u8char_T uc)
{
    যদি (base_screenlines == রিক্ত || base_screenattrs == রিক্ত)
	ঘূৰাই;
    যদি (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ঘূৰাই;

    পূর্ণাঙ্ক off = row * base_screen_cols + col;
    base_screenlines[off] = সাৰি;
    base_screenattrs[off] = attr;
    যদি (enc_utf8 && base_screenlinesuc != রিক্ত)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
স্থির শূন্য
draw_opacity_padding_cell(
	পূর্ণাঙ্ক		row,
	পূর্ণাঙ্ক		col,
	popup_saved_screen_T *saved_screen,
	পূর্ণাঙ্ক		pad_start_col,
	পূর্ণাঙ্ক		pad_end_col)
{
    যদি (সাৰি_Offset == রিক্ত || Screen_সাৰিগুলি == রিক্ত || ScreenAttrs == রিক্ত
	    || (enc_utf8 && Screen_সাৰিগুলি_UC == রিক্ত))
	ঘূৰাই;

    পূর্ণাঙ্ক off = সাৰি_Offset[row] + col;
    পূর্ণাঙ্ক r = row - saved_screen->start_row;
    পূর্ণাঙ্ক c = col - saved_screen->start_col;

    যদি (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	পূর্ণাঙ্ক save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	যদি (enc_utf8 && saved_screen->linesuc != রিক্ত)
	{
	    পূর্ণাঙ্ক base_col = col - 1;
	    পূর্ণাঙ্ক base_off = off - 1;
	    পূর্ণাঙ্ক base_save_off = save_off - 1;
	    পূর্ণাঙ্ক wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    যদি (base_off >= 0)
	    {
		যদি (Screen_সাৰিগুলি_UC[base_off] != 0
			&& utf_char2cells(Screen_সাৰিগুলি_UC[base_off]) == 2
			&& Screen_সাৰিগুলি[off] == 0)
		    wide_prev = TRUE;
	    }
	    যদি (!wide_prev && save_off > 0)
	    {
		যদি (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->সাৰিগুলি[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    যদি (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		যদি (base_col < pad_start_col)
		{
		    যদি (Screen_সাৰিগুলি_UC[base_off] != 0
			    && utf_char2cells(Screen_সাৰিগুলি_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_সাৰিগুলি[base_off] = ' ';
			Screen_সাৰিগুলি_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_সাৰিগুলি[base_off],
				ScreenAttrs[base_off],
				Screen_সাৰিগুলি_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_সাৰিগুলি[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			যদি (enc_utf8)
			    Screen_সাৰিগুলি_UC[off] = 0;
			পূর্ণাঙ্ক popup_attr_val =
					get_win_attr(screen_opacity_popup);
			পূর্ণাঙ্ক blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_সাৰিগুলি[off], ScreenAttrs[off],
				Screen_সাৰিগুলি_UC[off]);
			screen_char(off, row, col);
			ঘূৰাই;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    যদি (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_সাৰিগুলি[base_off] == ' '
			    && Screen_সাৰিগুলি_UC[base_off] == 0)
		    {
			পূর্ণাঙ্ক popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			পূর্ণাঙ্ক blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_সাৰিগুলি[base_off] =
				    saved_screen->সাৰিগুলি[base_save_off];
			Screen_সাৰিগুলি_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_সাৰিগুলি[off] = 0;
			Screen_সাৰিগুলি_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_সাৰিগুলি[base_off],
				    ScreenAttrs[base_off],
				    Screen_সাৰিগুলি_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_সাৰিগুলি[off],
				    ScreenAttrs[off],
				    Screen_সাৰিগুলি_UC[off]);
			screen_char(base_off, row, base_col);
			ঘূৰাই;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_সাৰিগুলি[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    যদি (enc_utf8)
			Screen_সাৰিগুলি_UC[off] = 0;
		    পূর্ণাঙ্ক popup_attr_val = get_win_attr(screen_opacity_popup);
		    পূর্ণাঙ্ক blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_সাৰিগুলি[off],
					       ScreenAttrs[off], Screen_সাৰিগুলি_UC[off]);
		    screen_char(off, row, col);
		    ঘূৰাই;
		}

		// Base cell is inside the saved area, redraw the wide char.
		যদি (save_off > 0)
		{
		    Screen_সাৰিগুলি[base_off] = saved_screen->সাৰিগুলি[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_সাৰিগুলি[off] = saved_screen->সাৰিগুলি[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_সাৰিগুলি_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_সাৰিগুলি_UC[off] = saved_screen->linesuc[save_off];

		    পূর্ণাঙ্ক popup_attr_val = get_win_attr(screen_opacity_popup);
		    পূর্ণাঙ্ক blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_সাৰিগুলি[base_off],
					       ScreenAttrs[base_off], Screen_সাৰিগুলি_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_সাৰিগুলি[off],
					       ScreenAttrs[off], Screen_সাৰিগুলি_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		ঘূৰাই;
	    }
	}
	Screen_সাৰিগুলি[off] = saved_screen->সাৰিগুলি[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	যদি (enc_utf8)
	    Screen_সাৰিগুলি_UC[off] = saved_screen->linesuc != রিক্ত
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	যদি (enc_utf8 && Screen_সাৰিগুলি_UC[off] != 0
		&& utf_char2cells(Screen_সাৰিগুলি_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_সাৰিগুলি[off] = ' ';
	    Screen_সাৰিগুলি_UC[off] = 0;
	}

	পূর্ণাঙ্ক popup_attr_val = get_win_attr(screen_opacity_popup);
	পূর্ণাঙ্ক blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_সাৰিগুলি[off],
				   ScreenAttrs[off], Screen_সাৰিগুলি_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
স্থির শূন্য
fill_opacity_padding(
	পূর্ণাঙ্ক		start_row,
	পূর্ণাঙ্ক		end_row,
	পূর্ণাঙ্ক		start_col,
	পূর্ণাঙ্ক		end_col,
	popup_saved_screen_T *saved_screen)
{
    বাবে (পূর্ণাঙ্ক pad_row = start_row; pad_row < end_row; pad_row++)
	বাবে (পূর্ণাঙ্ক pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    শূন্য
update_popups(শূন্য (*win_update)(win_T *wp))
{
    win_T   *wp;
    পূর্ণাঙ্ক	    শীৰ্ষ_off;
    পূর্ণাঙ্ক	    বাম_extra;
    পূর্ণাঙ্ক	    total_width;
    পূর্ণাঙ্ক	    total_height;
    পূর্ণাঙ্ক	    শীৰ্ষ_padding;
    পূর্ণাঙ্ক	    popup_attr;
    পূর্ণাঙ্ক	    border_attr[4];
    পূর্ণাঙ্ক	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    পূর্ণাঙ্ক	    row;
    পূর্ণাঙ্ক	    wincol;
    পূর্ণাঙ্ক	    padcol = 0;
    পূর্ণাঙ্ক	    padendcol = 0;
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    sb_thumb_শীৰ্ষ = 0;
    পূর্ণাঙ্ক	    sb_thumb_height = 0;
    পূর্ণাঙ্ক	    attr_scroll = 0;
    পূর্ণাঙ্ক	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    যদি (base_screenlines != রিক্ত)
    {
	vim_free(base_screenlines);
	base_screenlines = রিক্ত;
    }
    যদি (base_screenattrs != রিক্ত)
    {
	vim_free(base_screenattrs);
	base_screenattrs = রিক্ত;
    }
    যদি (base_screenlinesuc != রিক্ত)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = রিক্ত;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    যেতিয়ালৈ ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != রিক্ত)
    {
	পূর্ণাঙ্ক	    title_len = 0;
	পূর্ণাঙ্ক	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	যদি ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	অন্যথা
	    screen_opacity_popup = রিক্ত;

#ifdef FEAT_PROP_POPUP
	যদি (screen_opacity_popup != রিক্ত)
	{
	    যদি (base_screenlines != রিক্ত)
	    {
		vim_free(base_screenlines);
		base_screenlines = রিক্ত;
	    }
	    যদি (base_screenattrs != রিক্ত)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = রিক্ত;
	    }
	    যদি (base_screenlinesuc != রিক্ত)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = রিক্ত;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(পূর্ণাঙ্ক,
				    base_screen_rows * base_screen_cols);
	    যদি (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    যদি (base_screenlines != রিক্ত && base_screenattrs != রিক্ত)
	    {
		বাবে (পূর্ণাঙ্ক r = 0; r < base_screen_rows; r++)
		{
		    পূর্ণাঙ্ক off = সাৰি_Offset[r];
		    পূর্ণাঙ্ক base_off = r * base_screen_cols;
		    বাবে (পূর্ণাঙ্ক c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_সাৰিগুলি[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			যদি (enc_utf8 && base_screenlinesuc != রিক্ত)
			    base_screenlinesuc[base_off + c] =
					Screen_সাৰিগুলি_UC[off + c];
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
	শীৰ্ষ_off = popup_শীৰ্ষ_extra(wp);
	বাম_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	যদি (wp->w_wincol + বাম_extra < 0)
	    বাম_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(শীৰ্ষ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += বাম_extra;

	// Draw the popup text, unless it's off screen.
	যদি (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    যদি (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    যদি (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    অন্যথা যদি (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(শীৰ্ষ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= বাম_extra;

	// Add offset for border and padding if not done already.
	যদি ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += বাম_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	যদি ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += শীৰ্ষ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	যদি (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	অন্যথা
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	যদি (total_width < 0)
	    total_width = 0;
	যদি (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	অন্যথা
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	যদি (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	অন্যথা
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	যদি (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	অন্যথা
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    বাবে (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    যদি (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	বাবে (i = 0; i < 8; ++i)
	    যদি (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	বাবে (i = 0; i < 4; ++i)
	{
	    যদি (wp->w_border_highlight[i] != রিক্ত)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    অন্যথা যদি (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    অন্যথা
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    যদি ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	যদি ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	যদি (wp->w_popup_title != রিক্ত)
	{
	    পূর্ণাঙ্ক	    title_attr;

	    যদি (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != রিক্ত)
		title_attr = border_attr[0];
	    অন্যথা যদি (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    অন্যথা
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    যদি (title_len > total_width - 2)
	    {
		পূর্ণাঙ্ক	title_byte_len = (পূর্ণাঙ্ক)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		যদি (title_text != রিক্ত)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    অন্যথা
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	শীৰ্ষ_padding = cl.eff_padding[0];
	যদি (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    যদি (title_len > 0)
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
	    অন্যথা
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    যদি (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	অন্যথা যদি (cl.eff_padding[0] == 0 && popup_শীৰ্ষ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    শীৰ্ষ_padding = 1;

	যদি (শীৰ্ষ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    যদি (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	যদি (শীৰ্ষ_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    যদি (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		যদি (screen_opacity_popup != রিক্ত && saved_screen.সাৰিগুলি != রিক্ত)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		অন্যথা
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		শীৰ্ষ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    যদি (screen_opacity_popup != রিক্ত && saved_screen.সাৰিগুলি != রিক্ত)
	    {
		fill_opacity_padding(row, row + শীৰ্ষ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    অন্যথা
	    {
		screen_fill(row, row + শীৰ্ষ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	যদি (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	    পূর্ণাঙ্ক		height = wp->w_height;
	    পূর্ণাঙ্ক		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    যদি (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    যদি (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    যদি (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_শীৰ্ষ = 0;
	    অন্যথা
		sb_thumb_শীৰ্ষ = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    যদি (wp->w_topline > 1 && sb_thumb_শীৰ্ষ == 0 && height > 1)
		sb_thumb_শীৰ্ষ = 1;  // show it's scrolled
	    last = total_height - শীৰ্ষ_off - wp->w_popup_border[2];
	    যদি (sb_thumb_শীৰ্ষ >= last)
		// show at least one character
		sb_thumb_শীৰ্ষ = last - 1;

	    যদি (wp->w_scrollbar_highlight != রিক্ত)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    অন্যথা
		attr_scroll = highlight_attr[HLF_PSB];
	    যদি (wp->w_thumb_highlight != রিক্ত)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    অন্যথা
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	বাবে (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    পূর্ণাঙ্ক	pad_বাম;
	    // left and right padding only needed next to the body
	    পূর্ণাঙ্ক do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    যদি (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    যদি (do_padding && cl.eff_padding[3] > 0)
	    {
		পূর্ণাঙ্ক col = wincol + wp->w_popup_border[3];

		// left padding
		pad_বাম = wp->w_popup_padding[3];
		যদি (col < 0)
		{
		    pad_বাম += col;
		    col = 0;
		}
		যদি (pad_বাম > 0)
		{
		    যদি (screen_opacity_popup != রিক্ত && saved_screen.সাৰিগুলি != রিক্ত)
			fill_opacity_padding(row, row + 1, col, col + pad_বাম,
				&saved_screen);
		    অন্যথা
			screen_fill(row, row + 1, col, col + pad_বাম,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    যদি (wp->w_has_scrollbar)
	    {
		পূর্ণাঙ্ক সাৰি = i - শীৰ্ষ_off;
		পূর্ণাঙ্ক scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		যদি (সাৰি >= 0 && সাৰি < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    সাৰি >= sb_thumb_শীৰ্ষ
				       && সাৰি < sb_thumb_শীৰ্ষ + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		অন্যথা
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    যদি (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    যদি (do_padding && cl.eff_padding[1] > 0)
	    {
		পূর্ণাঙ্ক pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		পূর্ণাঙ্ক pad_col_end = pad_col_start + wp->w_popup_padding[1];

		যদি (screen_opacity_popup != রিক্ত && saved_screen.সাৰিগুলি != রিক্ত)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		অন্যথা
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	যদি (wp->w_popup_shadow)
	{
	    পূর্ণাঙ্ক col = wincol + total_width;
	    বাবে (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	যদি (cl.eff_padding[2] > 0)
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
	    যদি (screen_opacity_popup != রিক্ত && saved_screen.সাৰিগুলি != রিক্ত)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    অন্যথা
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	যদি (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    যদি (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	যদি (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    বাবে (পূর্ণাঙ্ক col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	যদি (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = রিক্ত;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	যদি (override_success)
	    বেৰ_কৰো_highlight_overrides();

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

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
    // In case win_update() called start_search_hl().
    end_অনুসন্ধান_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    স্থির পূর্ণাঙ্ক
set_ref_in_one_popup(win_T *wp, পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    typval_T	tv;

    যদি (wp->w_close_cb.cb_partial != রিক্ত)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
    }
    যদি (wp->w_filter_cb.cb_partial != রিক্ত)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    ঘূৰাই abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    পূর্ণাঙ্ক
set_ref_in_popups(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    বাবে (wp = first_popupwin; !abort && wp != রিক্ত; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	বাবে (wp = tp->tp_first_popupwin; !abort && wp != রিক্ত; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	যদি (abort)
	    ভঙ্গ;
    }
    ঘূৰাই abort;
}

    পূর্ণাঙ্ক
popup_is_popup(win_T *wp)
{
    ঘূৰাই wp->w_popup_flags != 0;
}

#যদি defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(শূন্য)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_p_pvw)
	    ঘূৰাই wp;
    ঘূৰাই রিক্ত;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(শূন্য)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_popup_flags & POPF_INFO)
	    ঘূৰাই wp;
    ঘূৰাই রিক্ত;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(পূর্ণাঙ্ক dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    পূর্ণাঙ্ক		by;
    linenr_T	new_topline;

    যদি (wp == রিক্ত)
	ঘূৰাই false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    যদি (new_topline < 1)
	new_topline = 1;
    যদি (new_topline > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
	new_topline = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
    যদি (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ঘূৰাই true;
#অন্যথা
    ঘূৰাই false;
#endif
}

    শূন্য
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_সংখ্যা = message_win == রিক্ত ? 0 : message_win->w_id;
#অন্যথা
    rettv->vval.v_সংখ্যা = 0;
#endif
}

    শূন্য
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_সংখ্যা = wp == রিক্ত ? 0 : wp->w_id;
#অন্যথা
    rettv->vval.v_সংখ্যা = 0;
#endif
}

    শূন্য
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_সংখ্যা = wp == রিক্ত ? 0 : wp->w_id;
#অন্যথা
    rettv->vval.v_সংখ্যা = 0;
#endif
}

#যদি defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    পূর্ণাঙ্ক
popup_create_preview_window(পূর্ণাঙ্ক info)
{
    win_T *wp = popup_create(রিক্ত, রিক্ত, info ? TYPE_INFO : TYPE_PREVIEW);

    যদি (wp == রিক্ত)
	ঘূৰাই FAIL;
    যদি (info)
	wp->w_popup_flags |= POPF_INFO;
    অন্যথা
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    যদি (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    অন্যথা যদি (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    অন্যথা
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    ঘূৰাই OK;
}

/*
 * Close any preview popup.
 */
    শূন্য
popup_close_preview(শূন্য)
{
    win_T *wp = popup_find_preview_window();

    যদি (wp != রিক্ত)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    শূন্য
popup_hide_info(শূন্য)
{
    win_T *wp = popup_find_info_window();

    যদি (wp != রিক্ত)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	যদি (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    শূন্য
popup_close_info(শূন্য)
{
    win_T *wp = popup_find_info_window();

    যদি (wp != রিক্ত)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    পূর্ণাঙ্ক
popup_overlaps_cmdline(শূন্য)
{
    ঘূৰাই popup_on_cmdline;
}

#যদি defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(শূন্য)
{
    যদি (message_win != রিক্ত)
	ঘূৰাই message_win;

    পূর্ণাঙ্ক i;

    message_win = popup_create(রিক্ত, রিক্ত, TYPE_MESSAGE_WIN);

    যদি (message_win == রিক্ত)
	ঘূৰাই রিক্ত;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    বাবে (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    বাবে (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    যদি (message_win->w_popup_timer != রিক্ত)
	message_win->w_popup_timer->tr_keep = TRUE;
    ঘূৰাই message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    শূন্য
popup_show_message_win(শূন্য)
{
    যদি (message_win == রিক্ত)
	ঘূৰাই;

    যদি ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    স্থির শূন্য
may_start_message_win_timer(win_T *wp)
{
    যদি (wp == message_win && start_message_win_timer)
    {
	যদি (message_win->w_popup_timer != রিক্ত)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    পূর্ণাঙ্ক
popup_message_win_visible(শূন্য)
{
    ঘূৰাই message_win != রিক্ত
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    শূন্য
popup_hide_message_win(শূন্য)
{
    যদি (message_win != রিক্ত)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
স্থির পূর্ণাঙ্ক save_বাৰ্তা_didout = FALSE;
স্থির পূর্ণাঙ্ক save_বাৰ্তা_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
স্থির পূর্ণাঙ্ক ew_বাৰ্তা_didout = FALSE;
স্থির পূর্ণাঙ্ক ew_বাৰ্তা_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    শূন্য
start_echowindow(পূর্ণাঙ্ক time_sec)
{
    in_echowindow = TRUE;
    save_বাৰ্তা_didout = বাৰ্তা_didout;
    save_বাৰ্তা_col = বাৰ্তা_col;
    বাৰ্তা_didout = ew_বাৰ্তা_didout;
    বাৰ্তা_col = ew_বাৰ্তা_col;
    যদি (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    শূন্য
end_echowindow(শূন্য)
{
    in_echowindow = FALSE;

    যদি ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_বাৰ্তা_didout = TRUE;
    ew_বাৰ্তা_col = বাৰ্তা_col == 0 ? 1 : বাৰ্তা_col;
    বাৰ্তা_didout = save_বাৰ্তা_didout;
    বাৰ্তা_col = save_বাৰ্তা_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    পূর্ণাঙ্ক
popup_win_closed(win_T *win)
{
    পূর্ণাঙ্ক	    round;
    win_T   *wp;
    win_T   *next;
    পূর্ণাঙ্ক	    ret = FALSE;

    বাবে (round = 1; round <= 2; ++round)
	বাবে (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != রিক্ত; wp = next)
	{
	    next = wp->w_next;
	    যদি (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    ঘূৰাই ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    শূন্য
popup_set_title(win_T *wp)
{
    যদি (wp->w_buffer->b_fname == রিক্ত)
	ঘূৰাই;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    যদি (wp->w_popup_title != রিক্ত)
	vim_snprintf((ৱর্ণ *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#যদি defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    শূন্য
popup_update_preview_title(শূন্য)
{
    win_T *wp = popup_find_preview_window();

    যদি (wp != রিক্ত)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
