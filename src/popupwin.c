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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_PROP_POPUP)

வகைவரை கட்டமைப்பு {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
வகைவரை கட்டமைப்பு {
    முழு		active;
    முழு		winrow;
    முழு		wincol;
    முழு		height;
    முழு		width;
    முழு		leftoff;
    முழு		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
வகைவரை கட்டமைப்பு {
    schar_T	*வரிகள்;
    முழு		*attrs;
    u8char_T	*linesuc;
    முழு		start_row;
    முழு		start_col;
    முழு		rows;
    முழு		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
வகைவரை கட்டமைப்பு {
    linenr_T	firstline;
    முழு		blend;
    முழு		flags;
    முழு		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
வகைவரை கட்டமைப்பு {
    முழு		winrow;
    முழு		wincol;
    முழு		width;
    முழு		height;
    முழு		leftcol;
    முழு		leftoff;
    முழு		has_scrollbar;
    முழு		topoff;
    முழு		bottomoff;
    முழு		leftclip;
    முழு		rightclip;
} popup_layout_T;

நிலையான poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
நிலையான win_T *message_win = வெற்று;

// Time used for the next ":echowindow" message in msec.
நிலையான முழு  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
நிலையான முழு    start_message_win_timer = FALSE;

நிலையான வெறுமை may_start_message_win_timer(win_T *wp);
#endif

நிலையான முழு popup_on_cmdline = FALSE;

நிலையான வெறுமை popup_adjust_position(win_T *wp);
நிலையான bool popup_area_changed(win_T *wp, popup_area_T *area);
நிலையான வெறுமை popup_redraw_exposed_area(popup_area_T *area);
நிலையான வெறுமை popup_save_area(win_T *wp, popup_area_T *area);
நிலையான வெறுமை popup_free_saved_screen(popup_saved_screen_T *saved_screen);
நிலையான வெறுமை popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
நிலையான bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
நிலையான bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
நிலையான வெறுமை popup_save_style(win_T *wp, popup_style_snapshot_T *style);
நிலையான வெறுமை popup_save_layout(win_T *wp, popup_layout_T *layout);
நிலையான வெறுமை redraw_under_popup_area(முழு winrow, முழு wincol, முழு height,
	முழு width, முழு leftoff);
நிலையான வெறுமை redraw_overlapped_opacity_popups(முழு winrow, முழு wincol,
	முழு height, முழு width, முழு leftoff, முழு zindex);
நிலையான வெறுமை redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    நிலையான முழு
popup_options_one(dict_T *dict, char_u *சாவி)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    முழு		n = 0;

    di = dict_find(dict, சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு MAXCOL;

    val = tv_get_string(&di->di_tv);
    எனில் (STRNCMP(val, "cursor", 6) != 0)
	திரும்பு dict_get_எண்_check(dict, சாவி);

    setcursor_mayforce(TRUE);
    s = val + 6;
    எனில் (*s != NUL)
    {
	endp = s;
	எனில் (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((எழுத்து *)s, (எழுத்து **)&endp, 10);
	எனில் (endp != வெற்று && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    திரும்பு 0;
	}
    }

    எனில் (STRCMP(சாவி, "line") == 0)
	n = screen_screenrow() + 1 + n;
    இல்லையெனில் // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    எனில் (n == 0)
	n = -1;
    திரும்பு n;
}

    நிலையான முழு
set_padding_border(dict_T *dict, முழு *array, எழுத்து *name, முழு max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    எனில் (di == வெற்று)
	திரும்பு OK;

    எனில் (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	திரும்பு FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    முழு		i;
    முழு		nr;

    க்கு (i = 0; i < 4; ++i)
	array[i] = 1;
    எனில் (list == வெற்று)
	திரும்பு OK;

    CHECK_LIST_MATERIALIZE(list);
    க்கு (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (முழு)tv_get_எண்(&li->li_tv);
	எனில் (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    திரும்பு OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    நிலையான வெறுமை
set_moved_மதிப்புகள்(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    நிலையான வெறுமை
set_moved_columns(win_T *wp, முழு flags)
{
    char_u	*ptr;
    முழு		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    முழு		mincol;

    எனில் (len <= 0)
	திரும்பு;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (முழு)(ptr - ml_get_curline());
    எனில் (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	திரும்பு;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    நிலையான வெறுமை
set_mousemoved_மதிப்புகள்(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    நிலையான வெறுமை
update_popup_uses_mouse_move(வெறுமை)
{
    popup_uses_mouse_move = FALSE;
    எனில் (!popup_visible)
	திரும்பு;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	எனில் (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    திரும்பு;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    திரும்பு;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    நிலையான வெறுமை
set_mousemoved_columns(win_T *wp, முழு flags)
{
    win_T	*textwp;
    char_u	*text;
    முழு		col;
    pos_T	pos;
    colnr_T	mcol;

    எனில் (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, வெற்று, &col) != OK)
	திரும்பு;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, வெற்று, வெற்று, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, வெற்று, வெற்று, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    முழு
popup_on_border(win_T *wp, முழு row, முழு col)
{
    திரும்பு (row == 0 && wp->w_popup_border[0] > 0)
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
    முழு
popup_close_if_on_X(win_T *wp, முழு row, முழு col)
{
    எனில் (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

// Values set when dragging a popup window starts.
நிலையான முழு drag_start_row;
நிலையான முழு drag_start_col;
நிலையான முழு drag_start_wantline;
நிலையான முழு drag_start_wantcol;
நிலையான முழு drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    வெறுமை
popup_start_drag(win_T *wp, முழு row, முழு col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    எனில் (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    இல்லையெனில்
	drag_start_wantline = wp->w_wantline;
    எனில் (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    இல்லையெனில்
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    எனில் (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    எனில் (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	எனில் (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	எனில் (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    வெறுமை
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    எனில் (!win_valid_popup(wp))
	திரும்பு;

    எனில் ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	முழு width_inc = mouse_col - drag_start_col;
	முழு height_inc = mouse_row - drag_start_row;

	எனில் (width_inc != 0)
	{
	    முழு width = wp->w_width + width_inc;

	    எனில் (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	எனில் (height_inc != 0)
	{
	    முழு height = wp->w_height + height_inc;

	    எனில் (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	திரும்பு;
    }

    எனில் (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	திரும்பு;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    எனில் (wp->w_wantline < 1)
	wp->w_wantline = 1;
    எனில் (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    எனில் (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    எனில் (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    வெறுமை
popup_set_firstline(win_T *wp)
{
    முழு	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    போது (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    முழு
popup_is_in_scrollbar(win_T *wp, முழு row, முழு col)
{
    திரும்பு wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    வெறுமை
popup_handle_scrollbar_click(win_T *wp, முழு row, முழு col)
{
    எனில் (!popup_is_in_scrollbar(wp, row, col))
	திரும்பு;

    முழு	    height = popup_height(wp);
    முழு	    new_topline = wp->w_topline;

    எனில் (row >= height / 2)
    {
	// Click in lower half, scroll down.
	எனில் (wp->w_topline < wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    ++new_topline;
    }
    இல்லையெனில் எனில் (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    எனில் (new_topline == wp->w_topline)
	திரும்பு;

    set_topline(wp, new_topline);
    எனில் (wp == curwin)
    {
	எனில் (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	இல்லையெனில் எனில் (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#எனில் defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    நிலையான வெறுமை
popup_add_timeout(win_T *wp, முழு time, முழு close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((எழுத்து *)cbbuf, அளவு(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    எனில் (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	திரும்பு;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    எனில் (cb.cb_name != வெற்று && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    நிலையான poppos_T
get_pos_entry(dict_T *d, முழு give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    முழு	    nr;

    எனில் (str == வெற்று)
	திரும்பு POPPOS_NONE;

    க்கு (nr = 0; nr < (முழு)ARRAY_LENGTH(poppos_entries); ++nr)
	எனில் (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    திரும்பு poppos_entries[nr].pp_val;

    எனில் (give_error)
	semsg(_(e_invalid_argument_str), str);
    திரும்பு POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    நிலையான வெறுமை
apply_move_options(win_T *wp, dict_T *d)
{
    முழு		nr;
    char_u	*str;
    dictitem_T	*di;

    எனில் ((nr = dict_get_எண்_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    எனில் ((nr = dict_get_எண்_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    எனில் ((nr = dict_get_எண்_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    எனில் ((nr = dict_get_எண்_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    எனில் (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    எனில் (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    எனில் (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	எனில் (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    எனில் (str != வெற்று)
    {
	wp->w_popup_prop_type = 0;
	எனில் (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    எனில் (di != வெற்று)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		எனில் (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    எனில் (nr <= 0)
		nr = find_prop_type_id(str, வெற்று);
	    எனில் (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    இல்லையெனில்
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    எனில் (di != வெற்று)
	wp->w_popup_prop_id = dict_get_எண்(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    நிலையான வெறுமை
handle_moved_argument(win_T *wp, dictitem_T *di, முழு mousemoved)
{
    எனில் (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != வெற்று)
    {
	char_u  *s = di->di_tv.vval.v_string;
	முழு	flags = 0;

	எனில் (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	இல்லையெனில் எனில் (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	இல்லையெனில் எனில் (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	இல்லையெனில் எனில் (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	எனில் (flags != 0)
	{
	    எனில் (mousemoved)
		set_mousemoved_columns(wp, flags);
	    இல்லையெனில்
		set_moved_columns(wp, flags);
	}
    }
    இல்லையெனில் எனில் (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != வெற்று
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	முழு	    mincol;
	முழு	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	எனில் (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_எண்(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    எனில் (mousemoved)
		wp->w_popup_mouse_row = nr;
	    இல்லையெனில்
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    எனில் (nr == 0)
		wp->w_popup_curwin = வெற்று;
	}

	mincol = tv_get_எண்(&li->li_tv);
	maxcol = tv_get_எண்(&li->li_next->li_tv);
	எனில் (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	இல்லையெனில்
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    நிலையான வெறுமை
check_highlight(dict_T *dict, எழுத்து *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    எனில் (di == வெற்று)
	திரும்பு;

    எனில் (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_மதிப்பு_for_argument_str), name);
    இல்லையெனில்
    {
	str = tv_get_string(&di->di_tv);
	எனில் (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    நிலையான வெறுமை
popup_show_curline(win_T *wp)
{
    எனில் (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    இல்லையெனில் எனில் (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	எனில் (wp->w_topline < 1)
	    wp->w_topline = 1;
	இல்லையெனில் எனில் (wp->w_topline > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    wp->w_topline = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	போது (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_வரி_மொத்தம்
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    எனில் (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    நிலையான char_u *
popup_get_sign_name(win_T *wp)
{
    நிலையான எழுத்து    buf[30];

    vim_snprintf(buf, அளவு(buf), "popup-%d", wp->w_id);
    திரும்பு (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    நிலையான வெறுமை
popup_highlight_curline(win_T *wp)
{
    முழு	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    எனில் ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	எனில் (!sign_exists_by_name(sign_name))
	{
	    எழுத்து *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, வெற்று, (char_u *)linehl, வெற்று, வெற்று, வெற்று,
		    வெற்று, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    இல்லையெனில்
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    நிலையான முழு
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    முழு		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    எனில் (di != வெற்று)
    {
	wp->w_firstline = dict_get_எண்(dict, "firstline");
	எனில் (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    எனில் (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    எனில் (str != வெற்று)
    {
	char_u	*title = vim_strsave(str);

	எனில் (title != வெற்று)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    எனில் (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    எனில் (di != வெற்று)
    {
	nr = dict_get_எண்(dict, "opacity");
	எனில் (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	இல்லையெனில் எனில் (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	இல்லையெனில் எனில் (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	இல்லையெனில்
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    எனில் (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    இல்லையெனில்
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    எனில் (di != வெற்று && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != வெற்று)
    {
	image_T		    *img;
	image_T		    *cur_img = வெற்று;
	image_placement_T   *place;
	முழு		    cw, ch;

	எனில் (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    எனில் (wp->w_popup_imagep != வெற்று)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = வெற்று;
	    }
	    செல் image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	எனில் (wp->w_popup_imagep != வெற்று && wp->w_popup_imagep->img != வெற்று)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	எனில் (img == வெற்று)
	    திரும்பு FAIL;

	எனில் (img == cur_img)
	    செல் image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	எனில் (place == வெற்று)
	{
	    image_unref(img);
	    திரும்பு FAIL;
	}

	எனில் (wp->w_popup_imagep != வெற்று)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	எனில் (dict_find(dict, (char_u *)"minwidth", -1) == வெற்று)
	    wp->w_minwidth = cw;
	எனில் (dict_find(dict, (char_u *)"maxwidth", -1) == வெற்று)
	    wp->w_maxwidth = cw;
	எனில் (dict_find(dict, (char_u *)"minheight", -1) == வெற்று)
	    wp->w_minheight = ch;
	எனில் (dict_find(dict, (char_u *)"maxheight", -1) == வெற்று)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    இல்லையெனில்
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	எனில் (wp->w_popup_imagep != வெற்று)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(வெற்று, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	எனில் (wp->w_popup_imagep != வெற்று)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    எனில் (di != வெற்று)
    {
	முழு ok = TRUE;

	எனில் (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != வெற்று)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    எனில் (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    இல்லையெனில் எனில் (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    இல்லையெனில் எனில் (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    இல்லையெனில்
		ok = FALSE;
	}
	இல்லையெனில்
	    ok = FALSE;
	எனில் (!ok)
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    எனில் (str != வெற்று)
    {
	எழுத்து *errmsg = update_wincolor(wp, str);

	எனில் (errmsg == வெற்று)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	இல்லையெனில்
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    எனில் (str != வெற்று)
    {
	எழுத்து *errmsg = update_winhighlight(wp, str);

	எனில் (errmsg == வெற்று)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	இல்லையெனில்
	    emsg(_(errmsg));
    }

    எனில் (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	திரும்பு FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    எனில் (di != வெற்று)
    {
	எனில் (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == வெற்று)
	{
	    emsg(_(e_list_required));
	    திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    முழு		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    எனில் (list->lv_len == 0)
	    {
		க்கு (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = வெற்று;
		}
	    }
	    இல்லையெனில்
	    {
		க்கு (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    எனில் (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			எனில் (hl != வெற்று)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		எனில் (list->lv_len == 1 && wp->w_border_highlight[0] != வெற்று)
		    க்கு (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			எனில் (hl != வெற்று)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    எனில் (di != வெற்று)
    {
	எனில் (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    முழு		i;

	    எனில் (list != வெற்று)
	    {
		CHECK_LIST_MATERIALIZE(list);
		க்கு (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    எனில் (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		எனில் (list->lv_len == 1)
		    க்கு (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		எனில் (list->lv_len == 2)
		{
		    க்கு (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    க்கு (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    எனில் (di != வெற்று)
    {
	wp->w_zindex = dict_get_எண்(dict, "zindex");
	எனில் (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	எனில் (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    எனில் (di != வெற்று)
    {
	முழு ok = FALSE;

	எனில் (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != வெற்று)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_பொருட்கள்(di->di_tv.vval.v_list, li)
	    {
		எனில் (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == வெற்று
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    நிறுத்து;
		}
		இல்லையெனில்
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	எனில் (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), "mask");
	    திரும்பு FAIL;
	}
    }

#எனில் defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_எண்(dict, "time");
    எனில் (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    எனில் (di != வெற்று)
    {
	set_moved_மதிப்புகள்(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    எனில் (di != வெற்று)
    {
	set_mousemoved_மதிப்புகள்(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    எனில் (nr != -1)
    {
	எனில் (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    எனில் (di != வெற்று)
    {
	callback_T	callback = get_callback(&di->di_tv);

	எனில் (callback.cb_name != வெற்று)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    எனில் (nr != -1)
    {
	எனில் (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    எனில் (str != வெற்று)
    {
	எனில் (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	இல்லையெனில்
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    எனில் (di == வெற்று)
	திரும்பு OK;

    callback_T	callback = get_callback(&di->di_tv);
    எனில் (callback.cb_name == வெற்று)
	திரும்பு OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    திரும்பு OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    நிலையான முழு
apply_options(win_T *wp, dict_T *dict, முழு create)
{
    முழு		nr;

    apply_move_options(wp, dict);

    எனில் (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    எனில் (apply_general_options(wp, dict) == FAIL)
	திரும்பு FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    எனில் (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    எனில் (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	எனில் (wp->w_firstline > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	இல்லையெனில் எனில் (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    எனில் (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    திரும்பு OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    நிலையான வெறுமை
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_பொருட்கள்(l, li)
	எனில் (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == வெற்று ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    நிலையான வெறுமை
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	எனில் (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    திரும்பு;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == வெற்று ? வெற்று : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == வெற்று ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    க்கு (li = l->lv_first; li != வெற்று; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		திரும்பு;
	    }
	    plist = di->di_tv.vval.v_list;
	    எனில் (plist != வெற்று)
	    {
		FOR_ALL_LIST_பொருட்கள்(plist, pli)
		{
		    எனில் (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			திரும்பு;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    எனில் (dict != வெற்று)
		    {
			முழு col = dict_get_எண்(dict, "col");

			prop_add_common(lnum, col, dict, buf, வெற்று);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    முழு
popup_உச்சி_extra(win_T *wp)
{
    முழு	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    எனில் (extra == 0 && wp->w_popup_title != வெற்று && *wp->w_popup_title != NUL)
	திரும்பு 1;
    திரும்பு extra;
}

/*
 * Get the padding plus border at the left.
 */
    முழு
popup_இடது_extra(win_T *wp)
{
    திரும்பு wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    முழு
popup_height(win_T *wp)
{
    திரும்பு wp->w_height
	    + popup_உச்சி_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    முழு
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    திரும்பு wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    முழு
popup_extra_width(win_T *wp)
{
    திரும்பு wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    நிலையான win_T *
popup_get_clipwin(win_T *wp)
{
    எனில் (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	திரும்பு வெற்று;
    எனில் (win_valid(wp->w_popup_prop_win))
	திரும்பு wp->w_popup_prop_win;
    திரும்பு வெற்று;
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
வகைவரை கட்டமைப்பு {
    முழு உச்சி_extra;
    முழு bot_extra;
    முழு இடது_extra;
    முழு வலது_extra;

    முழு clip_உச்சி_content;
    முழு clip_bot_content;
    முழு clip_இடது_content;
    முழு clip_வலது_content;

    முழு eff_உச்சி_extra;
    முழு eff_bot_extra;
    முழு eff_இடது_extra;
    முழு eff_வலது_extra;

    முழு eff_border[4];
    முழு eff_padding[4];

    முழு eff_height;
    முழு eff_width;
} popup_clip_T;

    நிலையான வெறுமை
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    முழு h, w;

    cl->உச்சி_extra = popup_உச்சி_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->இடது_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->வலது_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_உச்சி_content = wp->w_popup_topoff - cl->உச்சி_extra;
    எனில் (cl->clip_உச்சி_content < 0)
	cl->clip_உச்சி_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    எனில் (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_இடது_content = wp->w_popup_leftclip - cl->இடது_extra;
    எனில் (cl->clip_இடது_content < 0)
	cl->clip_இடது_content = 0;
    cl->clip_வலது_content = wp->w_popup_rightclip - cl->வலது_extra;
    எனில் (cl->clip_வலது_content < 0)
	cl->clip_வலது_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	முழு clip = wp->w_popup_topoff;
	முழு b = wp->w_popup_border[0];
	முழு p = wp->w_popup_padding[0];
	முழு rem;

	எனில் (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	இல்லையெனில்
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	முழு clip = wp->w_popup_bottomoff;
	முழு b = wp->w_popup_border[2];
	முழு p = wp->w_popup_padding[2];
	முழு rem;

	எனில் (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	இல்லையெனில்
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
	முழு excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	எனில் (excess > 0)
	{
	    எனில் (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		எனில் (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		இல்லையெனில்
		    cl->eff_border[0] -= excess;
	    }
	    இல்லையெனில்
		cl->eff_padding[0] -= excess;
	}
    }
    {
	முழு excess = wp->w_popup_topoff - cl->உச்சி_extra - wp->w_height;
	எனில் (excess > 0)
	{
	    எனில் (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		எனில் (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		இல்லையெனில்
		    cl->eff_border[2] -= excess;
	    }
	    இல்லையெனில்
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_உச்சி_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_இடது_extra = wp->w_popup_leftclip > 0 ? 0 : cl->இடது_extra;
    cl->eff_வலது_extra = wp->w_popup_rightclip > 0 ? 0 : cl->வலது_extra;

    h = wp->w_height - cl->clip_உச்சி_content - cl->clip_bot_content;
    எனில் (h < 0)
	h = 0;
    cl->eff_height = cl->eff_உச்சி_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_இடது_content - cl->clip_வலது_content;
    எனில் (w < 0)
	w = 0;
    cl->eff_width = cl->eff_இடது_extra + w + cl->eff_வலது_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
வகைவரை கட்டமைப்பு {
    முழு		height;
    முழு		width;
    முழு		winrow;
    முழு		wincol;
    முழு		leftcol;
    முழு		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    நிலையான வெறுமை
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

    நிலையான வெறுமை
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
    நிலையான வெறுமை
popup_screenpos_above_உச்சி(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	முழு	    *screen_row,
	முழு	    *screen_scol,
	முழு	    *screen_ccol,
	முழு	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    முழு	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (முழு)win_col_off(prop_win) - (முழு)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (முழு)scol + coloff;
    *screen_ccol = (முழு)ccol + coloff;
    *screen_ecol = (முழு)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (முழு)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    நிலையான வெறுமை
popup_hide_for_textprop(win_T *wp)
{
    எனில் ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	திரும்பு;
    wp->w_popup_flags |= POPF_HIDDEN;
    எனில் (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    நிலையான bool
popup_find_prop_above_உச்சி(
	win_T	    *wp,
	win_T	    *prop_win,
	முழு	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    எனில் (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	திரும்பு false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    எனில் (first < 1)
	first = 1;
    திரும்பு find_prop_in_வரிகள்(prop_win,
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
    நிலையான bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    முழு	    extra_h, extra_w;
    முழு	    popup_உச்சி, popup_bottom, popup_இடது, popup_வலது;
    முழு	    total_h, total_w;

    எனில் (cw == வெற்று)
	திரும்பு false;

    extra_h = popup_உச்சி_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_உச்சி = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_இடது = wp->w_wincol;
    popup_வலது = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    எனில் (popup_உச்சி < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_உச்சி;
    எனில் (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    எனில் (popup_இடது < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_இடது;
    எனில் (popup_வலது > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_வலது - (cw->w_wincol + cw->w_width);

    திரும்பு wp->w_popup_topoff >= total_h
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
    நிலையான வெறுமை
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    எனில் (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_உச்சி_content + cl->clip_bot_content;
	எனில் (wp->w_height < 0)
	    wp->w_height = 0;
	எனில் (cl->clip_உச்சி_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_உச்சி_content;
    }
    எனில் (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	எனில் (cl->clip_இடது_content > 0 || cl->clip_வலது_content > 0)
	    wp->w_p_wrap = FALSE;
	எனில் (cl->clip_வலது_content > 0)
	{
	    wp->w_width -= cl->clip_வலது_content;
	    எனில் (wp->w_width < 0)
		wp->w_width = 0;
	}
	எனில் (cl->clip_இடது_content > 0)
	{
	    wp->w_leftcol += cl->clip_இடது_content;
	    wp->w_wincol += cl->clip_இடது_content;
	    wp->w_width -= cl->clip_இடது_content;
	    எனில் (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    நிலையான வெறுமை
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    முழு iw, ih;

    // Visible cell width and height of popup itself (not image!)
    முழு	visible_width;
    முழு	visible_height;

    // Crop dimensions of image (in pixels).
    முழு	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    எனில் (wp->w_popup_imagep == வெற்று || wp->w_popup_flags & POPF_HIDDEN)
	திரும்பு;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    எனில் (wp->w_popup_imagep->img == வெற்று)
	// Just set the bounding box
	செல் exit;

    visible_width =
	wp->w_width - clip.clip_இடது_content - clip.clip_வலது_content;
    visible_height =
	wp->w_height - clip.clip_உச்சி_content - clip.clip_bot_content;

    எனில் (visible_width < 0 || visible_height < 0)
	திரும்பு;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_இடது_content;
    crop_y = clip.clip_உச்சி_content;
    crop_width = iw - crop_x - clip.clip_வலது_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    எனில் (crop_width > visible_width)
	crop_width = visible_width;
    எனில் (crop_height > visible_height)
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
    நிலையான வெறுமை
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    முழு		wrapped = 0;
    முழு		maxwidth;
    முழு		maxwidth_no_scrollbar;
    முழு		width_with_scrollbar = 0;
    முழு		used_maxwidth = FALSE;
    முழு		margin_width = 0;
    முழு		maxspace;
    முழு		center_vert = FALSE;
    முழு		center_hor = FALSE;
    முழு		allow_adjust_இடது = !wp->w_popup_fixed;
    முழு		உச்சி_extra = popup_உச்சி_extra(wp);
    முழு		வலது_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    முழு		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    முழு		இடது_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    முழு		extra_height = உச்சி_extra + bot_extra;
    முழு		extra_width = இடது_extra + வலது_extra;
    முழு		w_height_before_limit;
    popup_layout_T org_layout;
    முழு		minwidth, minheight;
    முழு		maxheight = Rows;
    முழு		wantline = wp->w_wantline;  // adjusted for textprop
    முழு		wantcol = wp->w_wantcol;    // adjusted for textprop
    முழு		use_wantcol = wantcol != 0;
    முழு		adjust_height_for_உச்சி_aligned = FALSE;

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
    எனில் (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    எனில் (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	முழு	    screen_row;
	முழு	    screen_scol;
	முழு	    screen_ccol;
	முழு	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_உச்சி = false;
	எனில் (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    எனில் (popup_find_prop_above_உச்சி(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_உச்சி = true;
	    இல்லையெனில்
	    {
		எனில் ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    எனில் (wp->w_popup_imagep != வெற்று)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    எனில் (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		திரும்பு;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	எனில் (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	எனில் (prop_above_உச்சி)
	    popup_screenpos_above_உச்சி(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	இல்லையெனில்
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	எனில் (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    திரும்பு;
	}
	எனில் (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	இல்லையெனில்
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	எனில் (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	இல்லையெனில்
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    இல்லையெனில்
    {
	// If no line was specified default to vertical centering.
	எனில் (wantline == 0)
	    center_vert = TRUE;
	இல்லையெனில் எனில் (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	எனில் (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    எனில் (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    இல்லையெனில்
    {
	எனில் (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    எனில் (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	எனில் (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_வரி_மொத்தம் - 1, 0);
	}

	எனில் (!use_wantcol)
	    center_hor = TRUE;
	இல்லையெனில் எனில் (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    எனில் (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- இடது_extra - வலது_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- இடது_extra - வலது_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    எனில் (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - இடது_extra - வலது_extra;
    maxwidth = maxspace;
    எனில் (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_இடது = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    எனில் (wp->w_p_nu || wp->w_p_rnu)
	margin_width = எண்_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    எனில் (signcolumn_on(wp))
	margin_width += 2;
#endif
    எனில் (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    எனில் (wp->w_buffer->b_term != வெற்று)
    {
	எனில் (minwidth == 0)
	    minwidth = 20;
	எனில் (minheight == 0)
	    minheight = 5;
    }
#endif

    எனில் (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    இல்லையெனில் எனில் (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    எனில் (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	எனில் (wp->w_firstline > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    wp->w_firstline = 0;
	இல்லையெனில்
	    wp->w_topline = wp->w_firstline;
    }
    எனில் (wp->w_topline < 1)
	wp->w_topline = 1;
    இல்லையெனில் எனில் (wp->w_topline > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	wp->w_topline = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;

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
	முழு saved_w_width = wp->w_width;

	எனில் (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	க்கு (ln = 1; ln <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்; ++ln)
	{
	    முழு len = linetabsize(wp, ln) + margin_width;

	    எனில் (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    எனில் (saved_w_width < len)
		saved_w_width = len;
	    எனில் (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		நிறுத்து;
	}
	wp->w_width = saved_w_width;
    }
    எனில் (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
    இல்லையெனில்
	lnum = wp->w_topline;
    போது (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
    {
	முழு len;
	முழு w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	எனில் (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	எனில் (len + margin_width > maxwidth
		&& allow_adjust_இடது
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    முழு shift_by = len + margin_width - maxwidth;

	    எனில் (shift_by > wp->w_wincol)
	    {
		முழு truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    எனில் (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    எனில் (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	எனில் (wp->w_p_wrap)
	{
	    போது (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	எனில் (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    எனில் (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	எனில் (wp->w_firstline < 0)
	    --lnum;
	இல்லையெனில்
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	எனில் (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_வரி_மொத்தம் - lnum)
		       + wrapped >= maxheight)
	    நிறுத்து;
    }

    எனில் (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்);
#ifdef FEAT_TERMINAL
    எனில் (wp->w_buffer->b_term != வெற்று && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    எனில் (wp->w_has_scrollbar)
    {
	++வலது_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	எனில் (maxwidth + வலது_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    எனில் (wp->w_popup_title != வெற்று && *wp->w_popup_title != NUL)
    {
	முழு title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	எனில் (minwidth < title_len)
	    minwidth = title_len;
    }

    எனில் (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    எனில் (wp->w_width > maxwidth)
    {
	எனில் (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	எனில் (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    முழு off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, வலது_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	இல்லையெனில்
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - வலது_extra;
	}
    }
    எனில் (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	எனில் (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    இல்லையெனில் எனில் (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	முழு leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	எனில் (leftoff >= 0)
	    wp->w_wincol = leftoff;
	இல்லையெனில் எனில் (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    எனில் (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    எனில் (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	முழு want_col = 0;

	// try to show the right border and any scrollbar
	want_col = இடது_extra + wp->w_width + வலது_extra;
	எனில் (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    எனில் (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_வரி_மொத்தம் - wp->w_topline
								 + 1 + wrapped;
    எனில் (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    எனில் (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    எனில் (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    எனில் (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	எனில் (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    இல்லையெனில் எனில் (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	எனில் ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	இல்லையெனில் எனில் (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	இல்லையெனில்
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_உச்சி_aligned = TRUE;
	}
    }
    இல்லையெனில் எனில் (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	முழு check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	எனில் (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    எனில் (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    எனில் (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	இல்லையெனில்
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_உச்சி_aligned = TRUE;
	}
    }

    எனில் (adjust_height_for_உச்சி_aligned && wp->w_want_scrollbar
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
	எனில் (wp->w_buffer->b_term == வெற்று || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    எனில் (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		எனில் (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		இல்லையெனில்
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    எனில் (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    இல்லையெனில் எனில் (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    எனில் (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    எனில் (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    எனில் (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    எனில் (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	முழு avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    எனில் (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	எனில் (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	எனில் (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	எனில் (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    முழு avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    எனில் (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    எனில் (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	எனில் (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    எனில் (wp->w_popup_imagep != வெற்று)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	திரும்பு;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    எனில் (win_valid(wp->w_popup_prop_win))
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
    எனில் (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	எனில் (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

வகைவரை எண்ணிக்கை
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
    நிலையான முழு
popup_is_notification(create_type_T type)
{
    திரும்பு type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    நிலையான வெறுமை
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    முழு	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    க்கு (lnum = buf->b_ml.ml_வரி_மொத்தம்; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    எனில் (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == வெற்று ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    இல்லையெனில்
    {
	list_T *l = text.vval.v_list;

	எனில் (l != வெற்று && l->lv_len > 0)
	{
	    எனில் (l->lv_first == &range_list_item)
		emsg(_(e_using_எண்_as_string));
	    இல்லையெனில் எனில் (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    இல்லையெனில்
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_வரி_மொத்தம்);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    செய் {						    \
	எனில் (wp != வெற்று)					    \
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
    } போது (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    நிலையான முழு
parse_popup_option(win_T *wp, முழு is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    முழு	    border_enabled = FALSE;

    எனில் (wp != வெற்று)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    க்கு ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	முழு	x;

	e = vim_strchr(p, ':');
	எனில் (e == வெற்று || e[1] == NUL)
	    திரும்பு FAIL;

	p = vim_strchr(e, ',');
	எனில் (p == வெற்று)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	எனில் (STRNCMP(s, "height:", 7) == 0)
	{
	    எனில் (dig != p)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
	    {
		எனில் (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(s, "width:", 6) == 0)
	{
	    எனில் (dig != p)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
	    {
		எனில் (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(s, "highlight:", 10) == 0)
	{
	    எனில் (wp != வெற்று)
	    {
		எழுத்து *errmsg;
		முழு c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		எனில் (errmsg == வெற்று)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		இல்லையெனில்
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    எனில் (*arg == NUL || *arg == ',')
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
	    {
		க்கு (முழு i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    முழு		i;
	    முழு		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    முழு		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    எனில் (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		எனில் (wp != வெற்று)
		{
		    க்கு (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		தொடர்;
	    }

	    token = vim_strnsave(arg, token_len);
	    எனில் (token == வெற்று)
		திரும்பு FAIL;

	    எனில் ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		எனில் (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		இல்லையெனில் எனில் (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		இல்லையெனில் எனில் (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		இல்லையெனில் எனில் (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		இல்லையெனில் எனில் (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		இல்லையெனில் எனில் (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    முழு		out[8];
		    முழு		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    க்கு (i = 0; i < 8 && !failed; i++)
		    {
			எனில் (*q == NUL)
			    failed = TRUE;
			இல்லையெனில்
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    எனில் (i < 7)
			    {
				எனில் (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    எனில் (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			திரும்பு FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    இல்லையெனில்
	    {
		vim_free(token);
		திரும்பு FAIL;
	    }

	    எனில் (wp != வெற்று)
	    {
		க்கு (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	இல்லையெனில் எனில் (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    முழு		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    முழு		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    எனில் (!on && !off)
		திரும்பு FAIL;
	    on = on && mouse_has(MOUSE_செருகு) && (border_enabled || is_preview);
	    எனில் (wp != வெற்று)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	இல்லையெனில் எனில் (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    முழு		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    முழு		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    எனில் (!on && !off)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
	    {
		எனில் (on && mouse_has(MOUSE_செருகு))
		    wp->w_popup_flags |= POPF_RESIZE;
		இல்லையெனில்
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    முழு		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    முழு		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    எனில் (!on && !off)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	இல்லையெனில் எனில் (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    முழு		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    முழு		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    எனில் (!menu && !item)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	இல்லையெனில் எனில் (STRNCMP(s, "opacity:", 8) == 0)
	{
	    எனில் (dig != p || x < 0 || x > 100)
		திரும்பு FAIL;
	    எனில் (wp != வெற்று)
	    {
		எனில் (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		இல்லையெனில்
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	இல்லையெனில்
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    முழு
parse_previewpopup(win_T *wp)
{
    திரும்பு parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    முழு
parse_completepopup(win_T *wp)
{
    திரும்பு parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    வெறுமை
popup_set_wantpos_cursor(win_T *wp, முழு width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    எனில் (d != வெற்று)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    எனில் (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    இல்லையெனில்
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	எனில் (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    எனில் (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	எனில் (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    வெறுமை
popup_set_wantpos_rowcol(win_T *wp, முழு row, முழு col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    நிலையான வெறுமை
add_border_இடது_வலது_padding(win_T *wp)
{
    முழு i;

    க்கு (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    நிலையான முழு
popup_terminal_exists(வெறுமை)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	எனில் (wp->w_buffer->b_term != வெற்று)
	    திரும்பு TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    எனில் (wp->w_buffer->b_term != வெற்று)
		திரும்பு TRUE;
    திரும்பு FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    வெறுமை
popup_redraw_all(வெறுமை)
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
    முழு
popup_need_redraw(வெறுமை)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	எனில் ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    திரும்பு TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Set the color for a notification window.
 */
    நிலையான வெறுமை
popup_update_color(win_T *wp, create_type_T type)
{
    எழுத்து    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    எழுத்து    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    எனில் (errmsg == வெற்று)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    இல்லையெனில்
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    நிலையான win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = வெற்று;
    முழு		tabnr = 0;
    முழு		new_buffer;
    buf_T	*buf = வெற்று;
    dict_T	*d = வெற்று;
    முழு		i;

    எனில் (check_secure())
	திரும்பு வெற்று;

    எனில் (argvars != வெற்று)
    {
	எனில் (in_vim9script()
		&& (check_for_string_or_எண்_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    திரும்பு வெற்று;

	// Check that arguments look OK.
	எனில் (argvars[0].v_type == VAR_எண்)
	{
	    buf = buflist_findnr(argvars[0].vval.v_எண்);
	    எனில் (buf == வெற்று)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_எண்);
		திரும்பு வெற்று;
	    }
#ifdef FEAT_TERMINAL
	    எனில் (buf->b_term != வெற்று && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		திரும்பு வெற்று;
	    }
#endif
	}
	இல்லையெனில் எனில் (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != வெற்று)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != வெற்று))
	{
	    emsg(_(e_buffer_எண்_text_or_list_required));
	    திரும்பு வெற்று;
	}
	எனில் (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    திரும்பு வெற்று;
	d = argvars[1].vval.v_dict;
    }

    எனில் (d != வெற்று)
    {
	எனில் (dict_has_சாவி(d, "tabpage"))
	    tabnr = (முழு)dict_get_எண்(d, "tabpage");
	இல்லையெனில் எனில் (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	இல்லையெனில்
	    tabnr = 0;
	எனில் (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    எனில் (tp == வெற்று)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		திரும்பு வெற்று;
	    }
	}
    }
    இல்லையெனில் எனில் (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    எனில் (buf != வெற்று && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	திரும்பு வெற்று;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    எனில் (wp == வெற்று)
	திரும்பு வெற்று;
    எனில் (rettv != வெற்று)
	rettv->vval.v_எண் = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    எனில் (buf != வெற்று)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    இல்லையெனில்
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(வெற்று, வெற்று, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	எனில் (buf == வெற்று)
	{
	    win_free_popup(wp);
	    திரும்பு வெற்று;
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

    எனில் (tp != வெற்று)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    இல்லையெனில் எனில் (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    இல்லையெனில் // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	எனில் (first_popupwin == வெற்று)
	    first_popupwin = wp;
	இல்லையெனில்
	{
	    போது (prev->w_next != வெற்று)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    எனில் (new_buffer && argvars != வெற்று)
	popup_set_buffer_text(buf, argvars[0]);

    எனில் (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    எனில் (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_மதிப்புகள்(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    எனில் (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	எனில் (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_மதிப்புகள்(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    எனில் (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	முழு	height = buf->b_ml.ml_வரி_மொத்தம் + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	க்கு (twp = first_popupwin; twp != வெற்று; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    எனில் (twp != wp
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
	எனில் (wp->w_wantline + height > Rows)
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
	க்கு (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    எனில் (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_இடது_வலது_padding(wp);
    }

    எனில் (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	எனில் (callback.cb_name != வெற்று)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    க்கு (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    க்கு (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    எனில் (type == TYPE_PREVIEW)
    {
	எனில் (mouse_has(MOUSE_செருகு))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	க்கு (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    எனில் (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	எனில் (mouse_has(MOUSE_செருகு))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_இடது_வலது_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    எனில் (d != வெற்று)
    {
	// Deal with options.
	எனில் (apply_options(wp, d, TRUE) == FAIL)
	{
	    (வெறுமை)popup_close(wp->w_id, FALSE);
	    திரும்பு வெற்று;
	}
    }

#ifdef FEAT_TIMERS
    எனில் (popup_is_notification(type) && wp->w_popup_timer == வெற்று)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    எனில் (buf->b_term != வெற்று)
	win_enter(wp, FALSE);
#endif

    திரும்பு wp;
}

/*
 * popup_clear()
 */
    வெறுமை
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு force = FALSE;

    எனில் (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
	force = (முழு)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    வெறுமை
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    வெறுமை
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    வெறுமை
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    நிலையான வெறுமை
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_எண்;
    argv[0].vval.v_எண் = (varnumber_T)wp->w_id;

    எனில் (result != வெற்று && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    இல்லையெனில்
    {
	argv[1].v_type = VAR_எண்;
	argv[1].vval.v_எண் = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    எனில் (result != வெற்று)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    நிலையான வெறுமை
back_to_prevwin(win_T *wp)
{
    எனில் (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    இல்லையெனில்
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    நிலையான வெறுமை
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    முழு id;

    எனில் (!win_valid(wp))
       திரும்பு;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    எனில் (wp == curwin && curbuf->b_term != வெற்று)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    எனில் (owp != curwin && owp->w_buffer->b_term != வெற்று)
		நிறுத்து;
	எனில் (owp != வெற்று)
	    win_enter(owp, FALSE);
	இல்லையெனில்
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		எனில் (owp != curwin && owp->w_buffer->b_term != வெற்று)
		    நிறுத்து;
	    எனில் (owp != வெற்று)
		win_enter(owp, FALSE);
	    இல்லையெனில்
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    எனில் (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	திரும்பு;
    }

    CHECK_CURBUF;
    எனில் (wp->w_close_cb.cb_name != வெற்று)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    வெறுமை
popup_close_with_retval(win_T *wp, முழு retval)
{
    typval_T res;

    res.v_type = VAR_எண்;
    res.vval.v_எண் = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    வெறுமை
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    நிலையான வெறுமை
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    எனில் (wp != mouse_wp
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
    வெறுமை
popup_handle_mouse_moved(வெறுமை)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    முழு	    row = mouse_row;
    முழு	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    க்கு (wp = first_popupwin; wp != வெற்று; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    க்கு (wp = curtab->tp_first_popupwin; wp != வெற்று; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    நிலையான வெறுமை
filter_handle_drag(win_T *wp, முழு c, typval_T *rettv)
{
    முழு	row = mouse_row;
    முழு	col = mouse_col;

    எனில் ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_சாவி(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_எண் = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    வெறுமை
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    முழு		id;
    win_T	*wp;
    char_u	*சாவி;
    typval_T	res;
    முழு		c;
    linenr_T	old_lnum;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    id = tv_get_எண்(&argvars[0]);
    wp = win_id2wp(id);
    சாவி = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    எனில் (wp == வெற்று)
	திரும்பு;

    c = *சாவி;
    எனில் (c == K_SPECIAL && சாவி[1] != NUL)
	c = TO_SPECIAL(சாவி[1], சாவி[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_TRUE;
    res.v_type = VAR_எண்;

    old_lnum = wp->w_cursor.lnum;
    எனில் (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	எனில் (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	இல்லையெனில்
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
    }
    எனில் (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	எனில் (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    ++wp->w_cursor.lnum;
	இல்லையெனில்
	    wp->w_cursor.lnum = 1;
    }
    எனில் (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	திரும்பு;
    }

    எனில் (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_எண் = -1;
	popup_close_and_callback(wp, &res);
	திரும்பு;
    }
    எனில் (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_எண் = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	திரும்பு;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    வெறுமை
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    முழு		id;
    win_T	*wp;
    char_u	*சாவி;
    typval_T	res;
    முழு		c;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    id = tv_get_எண்(&argvars[0]);
    wp = win_id2wp(id);
    சாவி = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    எனில் (wp == வெற்று)
	திரும்பு;

    c = *சாவி;
    எனில் (c == CAR && need_wait_return)
	திரும்பு;
    எனில் (c == K_SPECIAL && சாவி[1] != NUL)
	c = TO_SPECIAL(சாவி[1], சாவி[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_TRUE;

    எனில் (c == 'y' || c == 'Y')
	res.vval.v_எண் = 1;
    இல்லையெனில் எனில் (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_எண் = 0;
    இல்லையெனில்
    {
	filter_handle_drag(wp, c, rettv);
	திரும்பு;
    }

    // Invoke callback
    res.v_type = VAR_எண்;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    வெறுமை
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    வெறுமை
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    வெறுமை
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    நிலையான win_T *
find_popup_win(முழு id)
{
    win_T *wp = win_id2wp(id);

    எனில் (wp != வெற்று && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	திரும்பு வெற்று;
    }
    திரும்பு wp;
}

/*
 * popup_close({id})
 */
    வெறுமை
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு		id;
    win_T	*wp;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    எனில் (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == வெற்று &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    wp = find_popup_win(id);
    எனில் (wp != வெற்று)
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
    நிலையான வெறுமை
popup_clear_mask_for(win_T *wp)
{
    முழு r, c;
    முழு row_start, col_start, row_end, col_end;

    எனில் (popup_mask == வெற்று || !popup_visible)
	திரும்பு;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (முழு)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (முழு)screen_Columns);

    க்கு (r = row_start; r < row_end; ++r)
	க்கு (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    வெறுமை
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    முழு			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    எனில் (error_if_term_popup_window())
	திரும்பு;
#endif
    எனில் ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	திரும்பு;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    எனில் (wp->w_popup_imagep != வெற்று)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    எனில் (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    எனில் (was_visible)
	popup_clear_mask_for(wp);

    எனில் (old_area.active)
	popup_redraw_exposed_area(&old_area);
    இல்லையெனில்
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    எனில் (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    வெறுமை
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு		id;
    win_T	*wp;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    வெறுமை
popup_show(win_T *wp)
{
    bool popup_active;

    எனில் ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	திரும்பு;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    எனில் (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_வரிகள்_valid = 0;
	எனில் (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    இல்லையெனில்
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    வெறுமை
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு		id;
    win_T	*wp;

    rettv->v_type = VAR_எண்;
    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    எனில் (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_எண் = 0;
}

/*
 * popup_settext({id}, {text})
 */
    வெறுமை
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு		id;
    win_T	*wp;
    popup_area_T	old_area;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	திரும்பு;

    id = (முழு)tv_get_எண்(&argvars[0]);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;

    popup_save_area(wp, &old_area);

    எனில் (check_for_string_or_list_arg(argvars, 1) == FAIL)
	திரும்பு;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_வரிகள்_valid = 0;
    எனில் (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    எனில் (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    வெறுமை
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    முழு			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_FALSE;

    எனில் (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(&argvars[0]);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;

    buf = tv_get_buf_from_arg(&argvars[1]);

    எனில் (buf == வெற்று)
	திரும்பு;
#ifdef FEAT_TERMINAL
    எனில் (buf->b_term != வெற்று && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	திரும்பு;
    }
#endif

    எனில் (wp->w_buffer != buf)
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

	எனில் (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_எண் = VVAL_TRUE;
}

    நிலையான வெறுமை
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    முழு			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    எனில் (wp->w_popup_imagep != வெற்று)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    எனில் (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    எனில் (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    எனில் (wp == message_win)
	message_win = வெற்று;
#endif

    எனில் (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    எனில் (!old_area.active)
	popup_mask_refresh = TRUE;
}

    நிலையான வெறுமை
error_for_popup_window(வெறுமை)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    முழு
error_if_popup_window(முழு also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    எனில் (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == வெற்று)
#endif
	    )
    {
	error_for_popup_window();
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    முழு
popup_close(முழு id, முழு force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = வெற்று;

    // go through global popups
    க்கு (wp = first_popupwin; wp != வெற்று; prev = wp, wp = wp->w_next)
	எனில் (wp->w_id == id)
	{
	    எனில் (wp == curwin)
	    {
		எனில் (!force)
		{
		    error_for_popup_window();
		    திரும்பு FAIL;
		}
		back_to_prevwin(wp);
	    }
	    எனில் (prev == வெற்று)
		first_popupwin = wp->w_next;
	    இல்லையெனில்
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    திரும்பு OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	எனில் (popup_close_tabpage(tp, id, force) == OK)
	    திரும்பு OK;
    திரும்பு FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    முழு
popup_close_tabpage(tabpage_T *tp, முழு id, முழு force)
{
    win_T	*wp;
    win_T	**வேர் = &tp->tp_first_popupwin;
    win_T	*prev = வெற்று;

    க்கு (wp = *வேர்; wp != வெற்று; prev = wp, wp = wp->w_next)
	எனில் (wp->w_id == id)
	{
	    எனில் (wp == curwin)
	    {
		எனில் (!force)
		{
		    error_for_popup_window();
		    திரும்பு FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    எனில் (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    எனில் (prev == வெற்று)
		*வேர் = wp->w_next;
	    இல்லையெனில்
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    திரும்பு OK;
	}
    திரும்பு FAIL;
}

    வெறுமை
close_all_popups(முழு force)
{
    எனில் (!force && ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;
    போது (first_popupwin != வெற்று)
	எனில் (popup_close(first_popupwin->w_id, force) == FAIL)
	    திரும்பு;
    போது (curtab->tp_first_popupwin != வெற்று)
	எனில் (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    திரும்பு;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    முழு i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    க்கு (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    நிலையான bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    முழு i;

    எனில் (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	திரும்பு true;
    க்கு (i = 0; i < 4; i++)
	எனில் (style->border_highlight[i] != wp->w_border_highlight[i])
	    திரும்பு true;
    திரும்பு false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    நிலையான வெறுமை
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
    நிலையான bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    திரும்பு layout->winrow != wp->w_winrow
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
    நிலையான bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    திரும்பு area->winrow != wp->w_winrow
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
    நிலையான வெறுமை
mark_overlapped_opacity_popup(win_T *wp, முழு area_உச்சி, முழு area_bot,
	முழு area_இடது, முழு area_வலது, முழு zindex)
{
    எனில் ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_உச்சி
	    || wp->w_wincol >= area_வலது
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_இடது)
	திரும்பு;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_வரிகள்_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    நிலையான வெறுமை
redraw_overlapped_opacity_popups(முழு winrow, முழு wincol, முழு height, முழு width,
	முழு leftoff, முழு zindex)
{
    win_T	*wp;
    முழு		area_உச்சி = winrow;
    முழு		area_bot = winrow + height;
    முழு		area_இடது = wincol;
    முழு		area_வலது = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_உச்சி, area_bot, area_இடது,
		area_வலது, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_உச்சி, area_bot, area_இடது,
		area_வலது, zindex);

    எனில் (must_redraw < UPD_VALID)
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
    நிலையான வெறுமை
refill_pum_bg_row_from_buffer(முழு r, முழு இடது, முழு வலது)
{
    முழு		வரி_cp = r;
    முழு		col_cp = இடது;
    win_T	*wp;
    linenr_T	lnum;
    முழு		row_for_lnum;
    முழு		col_for_lnum = 0;
    char_u	*வரி;
    char_u	*p;
    முழு		win_text_col;
    முழு		screen_col;
    முழு		soff_base = (r - pum_bg_உச்சி) * pum_bg_cols;
    முழு		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    க்கு (c = இடது; c < வலது; ++c)
    {
	pum_bg_வரிகள்[soff_base + c] = ' ';
	எனில் (pum_bg_attrs != வெற்று)
	    pum_bg_attrs[soff_base + c] = 0;
	எனில் (enc_utf8 && pum_bg_வரிகள்_UC != வெற்று)
	    pum_bg_வரிகள்_UC[soff_base + c] = 0;
	எனில் (enc_utf8)
	{
	    முழு k;
	    க்கு (k = 0; k < MAX_MCO; ++k)
		எனில் (pum_bg_வரிகள்_C[k] != வெற்று)
		    pum_bg_வரிகள்_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&வரி_cp, &col_cp, IGNORE_POPUP);
    எனில் (wp == வெற்று || வரி_cp < 0 || வரி_cp >= wp->w_height)
	திரும்பு;
    எனில் (wp->w_buffer == வெற்று || wp->w_buffer->b_ml.ml_mfp == வெற்று)
	திரும்பு;

    // Compute the buffer line for this screen row.
    row_for_lnum = வரி_cp;
    எனில் (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, வெற்று))
	திரும்பு;	// past end of buffer
    எனில் (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	திரும்பு;

    வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);
    எனில் (வரி == வெற்று)
	திரும்பு;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    எனில் (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = வரி;
    போது (*p != NUL && screen_col < வலது)
    {
	முழு char_cells;
	முழு byte_மொத்தம்;
	முழு soff = soff_base + screen_col;
	முழு in_range = (screen_col >= இடது && screen_col < வலது);

	எனில் (*p == '\t')
	{
	    முழு ts = (முழு)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_மொத்தம் = 1;
	    எனில் (in_range)
	    {
		pum_bg_வரிகள்[soff] = ' ';
		எனில் (enc_utf8 && pum_bg_வரிகள்_UC != வெற்று)
		    pum_bg_வரிகள்_UC[soff] = 0;
	    }
	}
	இல்லையெனில் எனில் (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_மொத்தம் = mb_ptr2len(p);
	    எனில் (in_range)
	    {
		pum_bg_வரிகள்[soff] = *p;
		எனில் (enc_utf8 && pum_bg_வரிகள்_UC != வெற்று)
		    pum_bg_வரிகள்_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	இல்லையெனில்
	{
	    char_cells = 1;
	    byte_மொத்தம் = 1;
	    எனில் (in_range)
	    {
		pum_bg_வரிகள்[soff] = (*p < 0x20) ? ' ' : *p;
		எனில் (enc_utf8 && pum_bg_வரிகள்_UC != வெற்று)
		    pum_bg_வரிகள்_UC[soff] = 0;
	    }
	}
	எனில் (in_range && pum_bg_attrs != வெற்று)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_மொத்தம்;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    நிலையான வெறுமை
popup_redraw_exposed_area(popup_area_T *area)
{
    எனில் (!area->active)
	திரும்பு;

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
    எனில் (pum_bg_வரிகள் != வெற்று
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_உச்சி)
    {
	முழு உச்சி = MAX(area->winrow, pum_bg_உச்சி);
	முழு bot = MIN(area->winrow + area->height, pum_bg_bot);
	முழு இடது = MAX(area->wincol, 0);
	முழு வலது = MIN(area->wincol + area->width, pum_bg_cols);
	முழு r;

	க்கு (r = உச்சி; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, இடது, வலது);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    நிலையான வெறுமை
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->வரிகள்);
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
    நிலையான வெறுமை
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    எனில் (screen_opacity_popup == வெற்று
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	திரும்பு;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    எனில் (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->வரிகள் = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(முழு,
				  saved_screen->rows * saved_screen->cols);
    எனில் (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    எனில் (saved_screen->வரிகள் == வெற்று || saved_screen->attrs == வெற்று)
	திரும்பு;

    க்கு (முழு r = 0; r < saved_screen->rows; r++)
    {
	முழு screen_row = saved_screen->start_row + r;

	எனில் (screen_row >= 0 && screen_row < screen_Rows)
	    க்கு (முழு c = 0; c < saved_screen->cols; c++)
	    {
		முழு screen_col = saved_screen->start_col + c;

		எனில் (screen_col >= 0 && screen_col < screen_Columns)
		{
		    முழு off = வரி_Offset[screen_row] + screen_col;
		    முழு save_off = r * saved_screen->cols + c;

		    saved_screen->வரிகள்[save_off] = Screen_வரிகள்[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    எனில் (enc_utf8 && saved_screen->linesuc != வெற்று)
			saved_screen->linesuc[save_off] = Screen_வரிகள்_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    நிலையான வெறுமை
redraw_under_popup_area(முழு winrow, முழு wincol, முழு height, முழு width, முழு leftoff)
{
    முழு	    r;

    க்கு (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	முழு	    c;
	win_T	    *prev_twp = வெற்று;

	எனில் (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    தொடர்;
	}

	க்கு (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    முழு	    வரி_cp = r;
	    முழு	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&வரி_cp, &col_cp, IGNORE_POPUP);
	    எனில் (twp != வெற்று && twp != prev_twp)
	    {
		prev_twp = twp;
		எனில் (வரி_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (வெறுமை)mouse_comp_pos(twp, &வரி_cp, &col_cp, &lnum, வெற்று);
		    redrawWinline(twp, lnum);
		}
		இல்லையெனில் எனில் (வரி_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    வெறுமை
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    முழு		id;
    win_T	*wp;
    popup_area_T	old_area;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;  // invalid {id}

    எனில் (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	திரும்பு;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    எனில் (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    எனில் (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    வெறுமை
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    முழு		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    முழு		need_redraw = FALSE;
    முழு		need_reposition = FALSE;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;  // invalid {id}

    எனில் (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	திரும்பு;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (வெறுமை)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    எனில் (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    எனில் (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    இல்லையெனில் எனில் (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    எனில் (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    எனில் (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    இல்லையெனில் எனில் (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_வரிகள்_valid = 0;
	எனில் (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    எனில் (old_style.blend != wp->w_popup_blend)
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

    எனில் (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    வெறுமை
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    முழு		id;
    win_T	*wp;
    முழு		உச்சி_extra;
    முழு		இடது_extra;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;  // invalid {id}
    உச்சி_extra = popup_உச்சி_extra(wp);
    இடது_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_எண்(dict, "line", wp->w_winrow + 1);
    dict_add_எண்(dict, "col", wp->w_wincol + 1);
    dict_add_எண்(dict, "width", wp->w_width + இடது_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_எண்(dict, "height", wp->w_height + உச்சி_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_எண்(dict, "core_line", wp->w_winrow + 1 + உச்சி_extra);
    dict_add_எண்(dict, "core_col", wp->w_wincol + 1 + இடது_extra);
    dict_add_எண்(dict, "core_width", wp->w_width);
    dict_add_எண்(dict, "core_height", wp->w_height);

    dict_add_எண்(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_எண்(dict, "firstline", wp->w_topline);
    dict_add_எண்(dict, "lastline", wp->w_botline - 1);
    dict_add_எண்(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    வெறுமை
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    FOR_ALL_POPUPWINS(wp)
	list_append_எண்(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_எண்(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    வெறுமை
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    முழு		row;
    முழு		col;
    win_T	*wp;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    row = tv_get_எண்(&argvars[0]) - 1;
    col = tv_get_எண்(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    எனில் (wp != வெற்று && WIN_IS_POPUP(wp))
	rettv->vval.v_எண் = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    நிலையான வெறுமை
get_padding_border(dict_T *dict, முழு *array, எழுத்து *name)
{
    list_T  *list;
    முழு	    i;

    எனில் (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	திரும்பு;

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    எனில் (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	திரும்பு;
    }
    எனில் (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	க்கு (i = 0; i < 4; ++i)
	    list_append_எண்(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    நிலையான வெறுமை
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    முழு	    i;

    க்கு (i = 0; i < 4; ++i)
	எனில் (wp->w_border_highlight[i] != வெற்று)
	    நிறுத்து;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    எனில் (i == 4 && !wp->w_border_highlight_isset)
	திரும்பு;

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    எனில் (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	திரும்பு;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    எனில் (i == 4)
	திரும்பு;
    க்கு (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    நிலையான வெறுமை
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    முழு	    i;
    char_u  buf[NUMBUFLEN];
    முழு	    len;

    க்கு (i = 0; i < 8; ++i)
	எனில் (wp->w_border_char[i] != 0)
	    நிறுத்து;
    எனில் (i == 8)
	திரும்பு;

    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    எனில் (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	திரும்பு;
    }
    க்கு (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    நிலையான வெறுமை
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    எனில் (list != வெற்று)
    {
	எனில் (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	இல்லையெனில்
	{
	    list_append_எண்(list, wp->w_popup_lnum);
	    list_append_எண்(list, wp->w_popup_mincol);
	    list_append_எண்(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    எனில் (list == வெற்று)
	திரும்பு;

    எனில் (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	திரும்பு;
    }
    list_append_எண்(list, wp->w_popup_mouse_row);
    list_append_எண்(list, wp->w_popup_mouse_mincol);
    list_append_எண்(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    வெறுமை
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    முழு		id;
    win_T	*wp;
    tabpage_T	*tp;
    முழு		i;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = find_popup_win(id);
    எனில் (wp == வெற்று)
	திரும்பு;

    dict = rettv->vval.v_dict;
    dict_add_எண்(dict, "line", wp->w_wantline);
    dict_add_எண்(dict, "col", wp->w_wantcol);
    dict_add_எண்(dict, "minwidth", wp->w_minwidth);
    dict_add_எண்(dict, "minheight", wp->w_minheight);
    dict_add_எண்(dict, "maxheight", wp->w_maxheight);
    dict_add_எண்(dict, "maxwidth", wp->w_maxwidth);
    dict_add_எண்(dict, "firstline", wp->w_firstline);
    dict_add_எண்(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_எண்(dict, "zindex", wp->w_zindex);
    dict_add_எண்(dict, "fixed", wp->w_popup_fixed);
    எனில் (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	எனில் (pt != வெற்று)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_எண்(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_எண்(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    எனில் (wp->w_popup_imagep != வெற்று && wp->w_popup_imagep->img != வெற்று)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	எனில் (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_எண்(dict, "wrap", wp->w_p_wrap);
    dict_add_எண்(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_எண்(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_எண்(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_எண்(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_எண்(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_எண்(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_எண்(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_எண்(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    எனில் (wp->w_scrollbar_highlight != வெற்று)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    எனில் (wp->w_thumb_highlight != வெற்று)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    எனில் (twp->w_id == id)
		நிறுத்து;
	எனில் (twp != வெற்று)
	    நிறுத்து;
	++i;
    }
    எனில் (tp == வெற்று)
	i = -1;  // must be global
    இல்லையெனில் எனில் (tp == curtab)
	i = 0;
    dict_add_எண்(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    எனில் (wp->w_filter_cb.cb_name != வெற்று)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    எனில் (wp->w_close_cb.cb_name != வெற்று)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(poppos_entries); ++i)
	எனில் (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(முழு)poppos_entries[i].pp_name.length);
	    நிறுத்து;
	}

    எனில் (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    இல்லையெனில் எனில் (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    இல்லையெனில்
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#எனில் defined(FEAT_TIMERS)
    dict_add_எண்(dict, "time", wp->w_popup_timer != வெற்று
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#எனில் defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    முழு
error_if_term_popup_window(வெறுமை)
{
    எனில் (WIN_IS_POPUP(curwin) && curbuf->b_term != வெற்று
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    வெறுமை
popup_reset_handled(முழு handled_flag)
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
find_next_popup(முழு lowest, முழு handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    முழு	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = வெற்று;
    FOR_ALL_POPUPWINS(wp)
	எனில் ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    எனில் (found_wp != வெற்று)
	found_wp->w_popup_handled |= handled_flag;
    திரும்பு found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    நிலையான முழு
invoke_popup_filter(win_T *wp, முழு c)
{
    முழு		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    முழு		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    எனில் (c == Ctrl_C)
    {
	முழு save_got_int = got_int;
	முழு was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	எனில் (was_curwin && wp == curwin)
	    திரும்பு -1;
	திரும்பு TRUE;
    }

    argv[0].v_type = VAR_எண்;
    argv[0].vval.v_எண் = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    எனில் (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    இல்லையெனில்
    {
	எனில் (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	எனில் (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    எனில் (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	இல்லையெனில்
	{
	    எனில் (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    திரும்பு res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    முழு
popup_do_filter(முழு c)
{
    நிலையான முழு	recursive = FALSE;
    முழு		res = FALSE;
    win_T	*wp;
    முழு		save_சாவி_Typed = சாவி_Typed;
    முழு		state;
    முழு		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    எனில் (popup_is_popup(curwin) && curbuf->b_term != வெற்று)
	திரும்பு FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    எனில் (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	திரும்பு FALSE;

    எனில் (recursive)
	திரும்பு FALSE;
    recursive = TRUE;

    எனில் (c == K_LEFTMOUSE)
    {
	முழு row = mouse_row;
	முழு col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	எனில் (wp != வெற்று && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    போது (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != வெற்று)
	எனில் (wp->w_filter_cb.cb_name != வெற்று
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    எனில் (must_redraw > was_must_redraw)
    {
	முழு save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    சாவி_Typed = save_சாவி_Typed;

    // When interrupted return FALSE to avoid looping.
    திரும்பு res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    முழு
popup_no_mapping(வெறுமை)
{
    முழு	    round;
    win_T   *wp;

    க்கு (round = 1; round <= 2; ++round)
	க்கு (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != வெற்று; wp = wp->w_next)
	    எனில் (wp->w_filter_cb.cb_name != வெற்று
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    வெறுமை
popup_check_cursor_pos(வெறுமை)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    போது ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != வெற்று)
	எனில் (wp->w_popup_curwin != வெற்று
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    நிலையான வெறுமை
popup_update_mask(win_T *wp, முழு width, முழு height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    முழு		row, col;

    எனில் (wp->w_popup_mask == வெற்று || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	திரும்பு;
    }
    எனில் (wp->w_popup_mask_cells != வெற்று
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	திரும்பு;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    எனில் (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = வெற்று;
	திரும்பு;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    எனில் (wp->w_popup_mask_cells == வெற்று)
	திரும்பு;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_பொருட்கள்(wp->w_popup_mask, lio)
    {
	முழு cols, cole;
	முழு வரிகள், linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_எண்(&li->li_tv);
	எனில் (cols < 0)
	    cols = width + cols + 1;
	எனில் (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_எண்(&li->li_tv);
	எனில் (cole < 0)
	    cole = width + cole + 1;
	எனில் (cole > width)
	    cole = width;
	li = li->li_next;
	வரிகள் = tv_get_எண்(&li->li_tv);
	எனில் (வரிகள் < 0)
	    வரிகள் = height + வரிகள் + 1;
	எனில் (வரிகள் <= 0)
	    வரிகள் = 1;
	li = li->li_next;
	linee = tv_get_எண்(&li->li_tv);
	எனில் (linee < 0)
	    linee = height + linee + 1;
	எனில் (linee > height)
	    linee = height;

	க்கு (row = வரிகள் - 1; row < linee; ++row)
	    க்கு (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    நிலையான முழு
popup_masked(win_T *wp, முழு width, முழு height, முழு screencol, முழு screenline)
{
    முழு col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    முழு வரி = screenline - wp->w_winrow;

    திரும்பு col >= 0 && col < width
	    && வரி >= 0 && வரி < height
	    && wp->w_popup_mask_cells[வரி * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    நிலையான வெறுமை
update_popup_transparent(win_T *wp, முழு val)
{
    எனில் (wp->w_popup_mask == வெற்று)
	திரும்பு;

    முழு		width = popup_width(wp);
    முழு		height = popup_height(wp);
    listitem_T	*lio, *li;
    முழு		cols, cole;
    முழு		வரிகள், linee;
    முழு		col, வரி;

    FOR_ALL_LIST_பொருட்கள்(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_எண்(&li->li_tv);
	எனில் (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_எண்(&li->li_tv);
	எனில் (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	வரிகள் = tv_get_எண்(&li->li_tv);
	எனில் (வரிகள் < 0)
	    வரிகள் = height + வரிகள் + 1;
	li = li->li_next;
	linee = tv_get_எண்(&li->li_tv);
	எனில் (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	எனில் (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--வரிகள்;
	எனில் (வரிகள் < 0)
	    வரிகள் = 0;
	க்கு (வரி = வரிகள்; வரி < linee
		&& வரி + wp->w_winrow < screen_Rows; ++வரி)
	    க்கு (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(வரி + wp->w_winrow) * screen_Columns
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
    நிலையான முழு
check_popup_unhidden(win_T *wp)
{
    எனில் (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	எனில் ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    திரும்பு FALSE;
	எனில் (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	இல்லையெனில் எனில் (popup_find_prop_above_உச்சி(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	எனில் (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    நிலையான முழு
popup_need_position_adjust(win_T *wp)
{
    எனில் (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	திரும்பு TRUE;
    எனில் (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	திரும்பு TRUE;

    // May need to adjust the width if the cursor moved.
    திரும்பு wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
நிலையான short *opacity_zindex = வெற்று;
நிலையான முழு    opacity_zindex_rows = 0;
நிலையான முழு    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    நிலையான வெறுமை
popup_mark_opacity_zindex(win_T *wp)
{
    முழு	    width;
    முழு	    height;
    முழு	    r, c;

    எனில் (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	திரும்பு;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    க்கு (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	க்கு (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    முழு off = r * screen_Columns + c;
	    எனில் (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    நிலையான வெறுமை
redraw_win_under_opacity_popup(win_T *wp)
{
    முழு	    height;
    முழு	    width;
    முழு	    r;

    எனில் (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	திரும்பு;

    height = popup_height(wp);
    width = popup_width(wp);
    க்கு (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	முழு	    col;
	win_T	    *prev_twp = வெற்று;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	க்கு (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    முழு	    வரி_cp = r;
	    முழு	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&வரி_cp, &col_cp, IGNORE_POPUP);
	    எனில் (twp != வெற்று && twp != prev_twp)
	    {
		prev_twp = twp;
		எனில் (வரி_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    எனில் (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    இல்லையெனில்
#endif
		    {
			(வெறுமை)mouse_comp_pos(twp, &வரி_cp, &col_cp, &lnum,
									வெற்று);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		இல்லையெனில் எனில் (வரி_cp == twp->w_height)
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
    முழு
popup_is_under_opacity(முழு row, முழு col)
{
    எனில் (opacity_zindex == வெற்று
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	திரும்பு FALSE;
    திரும்பு opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    முழு
popup_is_over_opacity(முழு row, முழு col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	எனில் ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    திரும்பு TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    முழு
popup_is_under_opacity_range(முழு row, முழு start_col, முழு end_col)
{
    முழு col;

    எனில் (opacity_zindex == வெற்று
	    || row < 0 || row >= opacity_zindex_rows)
	திரும்பு FALSE;
    க்கு (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	எனில் (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    வெறுமை
may_update_popup_mask(முழு type)
{
    win_T	*wp;
    short	*mask;
    முழு		வரி, col;
    முழு		redraw_all_popups = FALSE;
    முழு		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    எனில் (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	எனில் (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	இல்லையெனில் எனில் (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	இல்லையெனில் எனில் (popup_need_position_adjust(wp))
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
	முழு has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    எனில் ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    எனில் ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	எனில் (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	இல்லையெனில்
	{
	    எனில் (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    எனில் (opacity_zindex != வெற்று)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * அளவு(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    எனில் (!popup_mask_refresh)
	திரும்பு;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    எனில் (redrawing_all_win)
	mask = popup_mask;
    இல்லையெனில்
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * அளவு(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    போது ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != வெற்று)
    {
	முழு width;
	முழு height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	எனில் (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    எனில் (wp->w_popup_flags & POPF_HIDDEN)
		தொடர்;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	எனில் (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	இல்லையெனில்
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	எனில் ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    தொடர்;

	{
	    முழு mask_start = wp->w_winrow;
	    முழு mask_end = mask_start + height;
	    முழு mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    முழு mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    க்கு (வரி = mask_start;
			    வரி < mask_end && வரி < screen_Rows; ++வரி)
		க்கு (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    எனில் (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(வரி, col, FALSE))
			mask[வரி * screen_Columns + col] = POPUPMENU_ZINDEX;
		    இல்லையெனில் எனில் (wp->w_popup_mask_cells == வெற்று
				    || !popup_masked(wp, width, height, col, வரி))
			mask[வரி * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    எனில் (mask == popup_mask_next)
    {
	முழு	    *plines_cache = ALLOC_CLEAR_MULT(முழு, Rows);
	win_T	    *prev_wp = வெற்று;

	க்கு (வரி = 0; வரி < screen_Rows; ++வரி)
	{
	    முழு	    col_done = 0;

	    க்கு (col = 0; col < screen_Columns; ++col)
	    {
		முழு off = வரி * screen_Columns + col;

		எனில் (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    எனில் (வரி >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			எனில் (!செய்தி_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    இல்லையெனில் எனில் (col >= col_done)
		    {
			linenr_T	lnum;
			முழு		வரி_cp = வரி;
			முழு		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&வரி_cp, &col_cp, IGNORE_POPUP);
			எனில் (wp != வெற்று)
			{
#எனில் defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    எனில் (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    இல்லையெனில்
#endif
			    {
				எனில் (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   அளவு(முழு) * Rows);
				    prev_wp = wp;
				}

				எனில் (வரி_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				இல்லையெனில்
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (வெறுமை)mouse_comp_pos(wp, &வரி_cp, &col_cp,
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
    வெறுமை
may_update_popup_position(வெறுமை)
{
    எனில் (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
நிலையான schar_T *base_screenlines = வெற்று;
நிலையான முழு *base_screenattrs = வெற்று;
நிலையான u8char_T *base_screenlinesuc = வெற்று;
நிலையான முழு base_screen_rows = 0;
நிலையான முழு base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    முழு
popup_get_base_screen_cell(முழு row, முழு col, schar_T *linep, முழு *attrp,
							  u8char_T *ucp)
{
    எனில் (base_screenlines == வெற்று || base_screenattrs == வெற்று)
	திரும்பு FALSE;
    எனில் (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	திரும்பு FALSE;

    முழு off = row * base_screen_cols + col;
    எனில் (linep != வெற்று)
	*linep = base_screenlines[off];
    எனில் (attrp != வெற்று)
	*attrp = base_screenattrs[off];
    எனில் (ucp != வெற்று)
    {
	எனில் (enc_utf8 && base_screenlinesuc != வெற்று)
	    *ucp = base_screenlinesuc[off];
	இல்லையெனில்
	    *ucp = 0;
    }
    திரும்பு TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    வெறுமை
popup_set_base_screen_cell(முழு row, முழு col, schar_T வரி, முழு attr, u8char_T uc)
{
    எனில் (base_screenlines == வெற்று || base_screenattrs == வெற்று)
	திரும்பு;
    எனில் (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	திரும்பு;

    முழு off = row * base_screen_cols + col;
    base_screenlines[off] = வரி;
    base_screenattrs[off] = attr;
    எனில் (enc_utf8 && base_screenlinesuc != வெற்று)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
நிலையான வெறுமை
draw_opacity_padding_cell(
	முழு		row,
	முழு		col,
	popup_saved_screen_T *saved_screen,
	முழு		pad_start_col,
	முழு		pad_end_col)
{
    எனில் (வரி_Offset == வெற்று || Screen_வரிகள் == வெற்று || ScreenAttrs == வெற்று
	    || (enc_utf8 && Screen_வரிகள்_UC == வெற்று))
	திரும்பு;

    முழு off = வரி_Offset[row] + col;
    முழு r = row - saved_screen->start_row;
    முழு c = col - saved_screen->start_col;

    எனில் (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	முழு save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	எனில் (enc_utf8 && saved_screen->linesuc != வெற்று)
	{
	    முழு base_col = col - 1;
	    முழு base_off = off - 1;
	    முழு base_save_off = save_off - 1;
	    முழு wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    எனில் (base_off >= 0)
	    {
		எனில் (Screen_வரிகள்_UC[base_off] != 0
			&& utf_char2cells(Screen_வரிகள்_UC[base_off]) == 2
			&& Screen_வரிகள்[off] == 0)
		    wide_prev = TRUE;
	    }
	    எனில் (!wide_prev && save_off > 0)
	    {
		எனில் (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->வரிகள்[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    எனில் (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		எனில் (base_col < pad_start_col)
		{
		    எனில் (Screen_வரிகள்_UC[base_off] != 0
			    && utf_char2cells(Screen_வரிகள்_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_வரிகள்[base_off] = ' ';
			Screen_வரிகள்_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_வரிகள்[base_off],
				ScreenAttrs[base_off],
				Screen_வரிகள்_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_வரிகள்[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			எனில் (enc_utf8)
			    Screen_வரிகள்_UC[off] = 0;
			முழு popup_attr_val =
					get_win_attr(screen_opacity_popup);
			முழு blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_வரிகள்[off], ScreenAttrs[off],
				Screen_வரிகள்_UC[off]);
			screen_char(off, row, col);
			திரும்பு;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    எனில் (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_வரிகள்[base_off] == ' '
			    && Screen_வரிகள்_UC[base_off] == 0)
		    {
			முழு popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			முழு blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_வரிகள்[base_off] =
				    saved_screen->வரிகள்[base_save_off];
			Screen_வரிகள்_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_வரிகள்[off] = 0;
			Screen_வரிகள்_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_வரிகள்[base_off],
				    ScreenAttrs[base_off],
				    Screen_வரிகள்_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_வரிகள்[off],
				    ScreenAttrs[off],
				    Screen_வரிகள்_UC[off]);
			screen_char(base_off, row, base_col);
			திரும்பு;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_வரிகள்[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    எனில் (enc_utf8)
			Screen_வரிகள்_UC[off] = 0;
		    முழு popup_attr_val = get_win_attr(screen_opacity_popup);
		    முழு blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_வரிகள்[off],
					       ScreenAttrs[off], Screen_வரிகள்_UC[off]);
		    screen_char(off, row, col);
		    திரும்பு;
		}

		// Base cell is inside the saved area, redraw the wide char.
		எனில் (save_off > 0)
		{
		    Screen_வரிகள்[base_off] = saved_screen->வரிகள்[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_வரிகள்[off] = saved_screen->வரிகள்[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_வரிகள்_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_வரிகள்_UC[off] = saved_screen->linesuc[save_off];

		    முழு popup_attr_val = get_win_attr(screen_opacity_popup);
		    முழு blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_வரிகள்[base_off],
					       ScreenAttrs[base_off], Screen_வரிகள்_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_வரிகள்[off],
					       ScreenAttrs[off], Screen_வரிகள்_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		திரும்பு;
	    }
	}
	Screen_வரிகள்[off] = saved_screen->வரிகள்[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	எனில் (enc_utf8)
	    Screen_வரிகள்_UC[off] = saved_screen->linesuc != வெற்று
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	எனில் (enc_utf8 && Screen_வரிகள்_UC[off] != 0
		&& utf_char2cells(Screen_வரிகள்_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_வரிகள்[off] = ' ';
	    Screen_வரிகள்_UC[off] = 0;
	}

	முழு popup_attr_val = get_win_attr(screen_opacity_popup);
	முழு blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_வரிகள்[off],
				   ScreenAttrs[off], Screen_வரிகள்_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
நிலையான வெறுமை
fill_opacity_padding(
	முழு		start_row,
	முழு		end_row,
	முழு		start_col,
	முழு		end_col,
	popup_saved_screen_T *saved_screen)
{
    க்கு (முழு pad_row = start_row; pad_row < end_row; pad_row++)
	க்கு (முழு pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    வெறுமை
update_popups(வெறுமை (*win_update)(win_T *wp))
{
    win_T   *wp;
    முழு	    உச்சி_off;
    முழு	    இடது_extra;
    முழு	    total_width;
    முழு	    total_height;
    முழு	    உச்சி_padding;
    முழு	    popup_attr;
    முழு	    border_attr[4];
    முழு	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    முழு	    row;
    முழு	    wincol;
    முழு	    padcol = 0;
    முழு	    padendcol = 0;
    முழு	    i;
    முழு	    sb_thumb_உச்சி = 0;
    முழு	    sb_thumb_height = 0;
    முழு	    attr_scroll = 0;
    முழு	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    எனில் (base_screenlines != வெற்று)
    {
	vim_free(base_screenlines);
	base_screenlines = வெற்று;
    }
    எனில் (base_screenattrs != வெற்று)
    {
	vim_free(base_screenattrs);
	base_screenattrs = வெற்று;
    }
    எனில் (base_screenlinesuc != வெற்று)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = வெற்று;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    போது ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != வெற்று)
    {
	முழு	    title_len = 0;
	முழு	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = தள்ளு_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	எனில் ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	இல்லையெனில்
	    screen_opacity_popup = வெற்று;

#ifdef FEAT_PROP_POPUP
	எனில் (screen_opacity_popup != வெற்று)
	{
	    எனில் (base_screenlines != வெற்று)
	    {
		vim_free(base_screenlines);
		base_screenlines = வெற்று;
	    }
	    எனில் (base_screenattrs != வெற்று)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = வெற்று;
	    }
	    எனில் (base_screenlinesuc != வெற்று)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = வெற்று;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(முழு,
				    base_screen_rows * base_screen_cols);
	    எனில் (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    எனில் (base_screenlines != வெற்று && base_screenattrs != வெற்று)
	    {
		க்கு (முழு r = 0; r < base_screen_rows; r++)
		{
		    முழு off = வரி_Offset[r];
		    முழு base_off = r * base_screen_cols;
		    க்கு (முழு c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_வரிகள்[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			எனில் (enc_utf8 && base_screenlinesuc != வெற்று)
			    base_screenlinesuc[base_off + c] =
					Screen_வரிகள்_UC[off + c];
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
	உச்சி_off = popup_உச்சி_extra(wp);
	இடது_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	எனில் (wp->w_wincol + இடது_extra < 0)
	    இடது_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(உச்சி_off - wp->w_popup_topoff, 0);
	wp->w_wincol += இடது_extra;

	// Draw the popup text, unless it's off screen.
	எனில் (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    எனில் (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    எனில் (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    இல்லையெனில் எனில் (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(உச்சி_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= இடது_extra;

	// Add offset for border and padding if not done already.
	எனில் ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += இடது_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	எனில் ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += உச்சி_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	எனில் (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	இல்லையெனில்
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	எனில் (total_width < 0)
	    total_width = 0;
	எனில் (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	இல்லையெனில்
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	எனில் (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	இல்லையெனில்
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	எனில் (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	இல்லையெனில்
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    க்கு (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    எனில் (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	க்கு (i = 0; i < 8; ++i)
	    எனில் (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	க்கு (i = 0; i < 4; ++i)
	{
	    எனில் (wp->w_border_highlight[i] != வெற்று)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    இல்லையெனில் எனில் (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    இல்லையெனில்
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    எனில் ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	எனில் ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	எனில் (wp->w_popup_title != வெற்று)
	{
	    முழு	    title_attr;

	    எனில் (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != வெற்று)
		title_attr = border_attr[0];
	    இல்லையெனில் எனில் (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    இல்லையெனில்
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    எனில் (title_len > total_width - 2)
	    {
		முழு	title_byte_len = (முழு)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		எனில் (title_text != வெற்று)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    இல்லையெனில்
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	உச்சி_padding = cl.eff_padding[0];
	எனில் (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    எனில் (title_len > 0)
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
	    இல்லையெனில்
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    எனில் (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	இல்லையெனில் எனில் (cl.eff_padding[0] == 0 && popup_உச்சி_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    உச்சி_padding = 1;

	எனில் (உச்சி_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    எனில் (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	எனில் (உச்சி_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    எனில் (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		எனில் (screen_opacity_popup != வெற்று && saved_screen.வரிகள் != வெற்று)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		இல்லையெனில்
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		உச்சி_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    எனில் (screen_opacity_popup != வெற்று && saved_screen.வரிகள் != வெற்று)
	    {
		fill_opacity_padding(row, row + உச்சி_padding, padcol, padendcol,
			&saved_screen);
	    }
	    இல்லையெனில்
	    {
		screen_fill(row, row + உச்சி_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	எனில் (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	    முழு		height = wp->w_height;
	    முழு		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    எனில் (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    எனில் (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    எனில் (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_உச்சி = 0;
	    இல்லையெனில்
		sb_thumb_உச்சி = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    எனில் (wp->w_topline > 1 && sb_thumb_உச்சி == 0 && height > 1)
		sb_thumb_உச்சி = 1;  // show it's scrolled
	    last = total_height - உச்சி_off - wp->w_popup_border[2];
	    எனில் (sb_thumb_உச்சி >= last)
		// show at least one character
		sb_thumb_உச்சி = last - 1;

	    எனில் (wp->w_scrollbar_highlight != வெற்று)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    இல்லையெனில்
		attr_scroll = highlight_attr[HLF_PSB];
	    எனில் (wp->w_thumb_highlight != வெற்று)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    இல்லையெனில்
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	க்கு (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    முழு	pad_இடது;
	    // left and right padding only needed next to the body
	    முழு do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    எனில் (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    எனில் (do_padding && cl.eff_padding[3] > 0)
	    {
		முழு col = wincol + wp->w_popup_border[3];

		// left padding
		pad_இடது = wp->w_popup_padding[3];
		எனில் (col < 0)
		{
		    pad_இடது += col;
		    col = 0;
		}
		எனில் (pad_இடது > 0)
		{
		    எனில் (screen_opacity_popup != வெற்று && saved_screen.வரிகள் != வெற்று)
			fill_opacity_padding(row, row + 1, col, col + pad_இடது,
				&saved_screen);
		    இல்லையெனில்
			screen_fill(row, row + 1, col, col + pad_இடது,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    எனில் (wp->w_has_scrollbar)
	    {
		முழு வரி = i - உச்சி_off;
		முழு scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		எனில் (வரி >= 0 && வரி < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    வரி >= sb_thumb_உச்சி
				       && வரி < sb_thumb_உச்சி + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		இல்லையெனில்
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    எனில் (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    எனில் (do_padding && cl.eff_padding[1] > 0)
	    {
		முழு pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		முழு pad_col_end = pad_col_start + wp->w_popup_padding[1];

		எனில் (screen_opacity_popup != வெற்று && saved_screen.வரிகள் != வெற்று)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		இல்லையெனில்
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	எனில் (wp->w_popup_shadow)
	{
	    முழு col = wincol + total_width;
	    க்கு (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	எனில் (cl.eff_padding[2] > 0)
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
	    எனில் (screen_opacity_popup != வெற்று && saved_screen.வரிகள் != வெற்று)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    இல்லையெனில்
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	எனில் (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    எனில் (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	எனில் (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    க்கு (முழு col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	எனில் (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = வெற்று;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	எனில் (override_success)
	    எடு_highlight_overrides();

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

#எனில் defined(FEAT_தேடல்_EXTRA)
    // In case win_update() called start_search_hl().
    end_தேடல்_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    நிலையான முழு
set_ref_in_one_popup(win_T *wp, முழு copyID)
{
    முழு		abort = FALSE;
    typval_T	tv;

    எனில் (wp->w_close_cb.cb_partial != வெற்று)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
    }
    எனில் (wp->w_filter_cb.cb_partial != வெற்று)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    திரும்பு abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    முழு
set_ref_in_popups(முழு copyID)
{
    முழு		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    க்கு (wp = first_popupwin; !abort && wp != வெற்று; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	க்கு (wp = tp->tp_first_popupwin; !abort && wp != வெற்று; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	எனில் (abort)
	    நிறுத்து;
    }
    திரும்பு abort;
}

    முழு
popup_is_popup(win_T *wp)
{
    திரும்பு wp->w_popup_flags != 0;
}

#எனில் defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(வெறுமை)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp->w_p_pvw)
	    திரும்பு wp;
    திரும்பு வெற்று;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(வெறுமை)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	எனில் (wp->w_popup_flags & POPF_INFO)
	    திரும்பு wp;
    திரும்பு வெற்று;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(முழு dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    முழு		by;
    linenr_T	new_topline;

    எனில் (wp == வெற்று)
	திரும்பு false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    எனில் (new_topline < 1)
	new_topline = 1;
    எனில் (new_topline > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
	new_topline = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
    எனில் (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    திரும்பு true;
#இல்லையெனில்
    திரும்பு false;
#endif
}

    வெறுமை
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_எண் = message_win == வெற்று ? 0 : message_win->w_id;
#இல்லையெனில்
    rettv->vval.v_எண் = 0;
#endif
}

    வெறுமை
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_எண் = wp == வெற்று ? 0 : wp->w_id;
#இல்லையெனில்
    rettv->vval.v_எண் = 0;
#endif
}

    வெறுமை
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_எண் = wp == வெற்று ? 0 : wp->w_id;
#இல்லையெனில்
    rettv->vval.v_எண் = 0;
#endif
}

#எனில் defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    முழு
popup_create_preview_window(முழு info)
{
    win_T *wp = popup_create(வெற்று, வெற்று, info ? TYPE_INFO : TYPE_PREVIEW);

    எனில் (wp == வெற்று)
	திரும்பு FAIL;
    எனில் (info)
	wp->w_popup_flags |= POPF_INFO;
    இல்லையெனில்
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    எனில் (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    இல்லையெனில் எனில் (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    இல்லையெனில்
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    திரும்பு OK;
}

/*
 * Close any preview popup.
 */
    வெறுமை
popup_close_preview(வெறுமை)
{
    win_T *wp = popup_find_preview_window();

    எனில் (wp != வெற்று)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    வெறுமை
popup_hide_info(வெறுமை)
{
    win_T *wp = popup_find_info_window();

    எனில் (wp != வெற்று)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	எனில் (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    வெறுமை
popup_close_info(வெறுமை)
{
    win_T *wp = popup_find_info_window();

    எனில் (wp != வெற்று)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    முழு
popup_overlaps_cmdline(வெறுமை)
{
    திரும்பு popup_on_cmdline;
}

#எனில் defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(வெறுமை)
{
    எனில் (message_win != வெற்று)
	திரும்பு message_win;

    முழு i;

    message_win = popup_create(வெற்று, வெற்று, TYPE_MESSAGE_WIN);

    எனில் (message_win == வெற்று)
	திரும்பு வெற்று;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    க்கு (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    க்கு (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    எனில் (message_win->w_popup_timer != வெற்று)
	message_win->w_popup_timer->tr_keep = TRUE;
    திரும்பு message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    வெறுமை
popup_show_message_win(வெறுமை)
{
    எனில் (message_win == வெற்று)
	திரும்பு;

    எனில் ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    நிலையான வெறுமை
may_start_message_win_timer(win_T *wp)
{
    எனில் (wp == message_win && start_message_win_timer)
    {
	எனில் (message_win->w_popup_timer != வெற்று)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    முழு
popup_message_win_visible(வெறுமை)
{
    திரும்பு message_win != வெற்று
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    வெறுமை
popup_hide_message_win(வெறுமை)
{
    எனில் (message_win != வெற்று)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
நிலையான முழு save_செய்தி_didout = FALSE;
நிலையான முழு save_செய்தி_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
நிலையான முழு ew_செய்தி_didout = FALSE;
நிலையான முழு ew_செய்தி_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    வெறுமை
start_echowindow(முழு time_sec)
{
    in_echowindow = TRUE;
    save_செய்தி_didout = செய்தி_didout;
    save_செய்தி_col = செய்தி_col;
    செய்தி_didout = ew_செய்தி_didout;
    செய்தி_col = ew_செய்தி_col;
    எனில் (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    வெறுமை
end_echowindow(வெறுமை)
{
    in_echowindow = FALSE;

    எனில் ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_செய்தி_didout = TRUE;
    ew_செய்தி_col = செய்தி_col == 0 ? 1 : செய்தி_col;
    செய்தி_didout = save_செய்தி_didout;
    செய்தி_col = save_செய்தி_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    முழு
popup_win_closed(win_T *win)
{
    முழு	    round;
    win_T   *wp;
    win_T   *next;
    முழு	    ret = FALSE;

    க்கு (round = 1; round <= 2; ++round)
	க்கு (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != வெற்று; wp = next)
	{
	    next = wp->w_next;
	    எனில் (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    திரும்பு ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    வெறுமை
popup_set_title(win_T *wp)
{
    எனில் (wp->w_buffer->b_fname == வெற்று)
	திரும்பு;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    எனில் (wp->w_popup_title != வெற்று)
	vim_snprintf((எழுத்து *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#எனில் defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    வெறுமை
popup_update_preview_title(வெறுமை)
{
    win_T *wp = popup_find_preview_window();

    எனில் (wp != வெற்று)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
