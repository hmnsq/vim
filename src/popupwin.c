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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_PROP_POPUP)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		active;
    ಪೂರ್ಣಾಙ್ಕ		winrow;
    ಪೂರ್ಣಾಙ್ಕ		wincol;
    ಪೂರ್ಣಾಙ್ಕ		height;
    ಪೂರ್ಣಾಙ್ಕ		width;
    ಪೂರ್ಣಾಙ್ಕ		leftoff;
    ಪೂರ್ಣಾಙ್ಕ		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    schar_T	*ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		*attrs;
    u8char_T	*linesuc;
    ಪೂರ್ಣಾಙ್ಕ		start_row;
    ಪೂರ್ಣಾಙ್ಕ		start_col;
    ಪೂರ್ಣಾಙ್ಕ		rows;
    ಪೂರ್ಣಾಙ್ಕ		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    linenr_T	firstline;
    ಪೂರ್ಣಾಙ್ಕ		blend;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		winrow;
    ಪೂರ್ಣಾಙ್ಕ		wincol;
    ಪೂರ್ಣಾಙ್ಕ		width;
    ಪೂರ್ಣಾಙ್ಕ		height;
    ಪೂರ್ಣಾಙ್ಕ		leftcol;
    ಪೂರ್ಣಾಙ್ಕ		leftoff;
    ಪೂರ್ಣಾಙ್ಕ		has_scrollbar;
    ಪೂರ್ಣಾಙ್ಕ		topoff;
    ಪೂರ್ಣಾಙ್ಕ		bottomoff;
    ಪೂರ್ಣಾಙ್ಕ		leftclip;
    ಪೂರ್ಣಾಙ್ಕ		rightclip;
} popup_layout_T;

ಸ್ಥಿರ poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
ಸ್ಥಿರ win_T *message_win = ರಿಕ್ತ;

// Time used for the next ":echowindow" message in msec.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ    start_message_win_timer = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ may_start_message_win_timer(win_T *wp);
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ popup_on_cmdline = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ popup_adjust_position(win_T *wp);
ಸ್ಥಿರ bool popup_area_changed(win_T *wp, popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_redraw_exposed_area(popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_area(win_T *wp, popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ಸ್ಥಿರ bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
ಸ್ಥಿರ bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_layout(win_T *wp, popup_layout_T *layout);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_under_popup_area(ಪೂರ್ಣಾಙ್ಕ winrow, ಪೂರ್ಣಾಙ್ಕ wincol, ಪೂರ್ಣಾಙ್ಕ height,
	ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ leftoff);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_overlapped_opacity_popups(ಪೂರ್ಣಾಙ್ಕ winrow, ಪೂರ್ಣಾಙ್ಕ wincol,
	ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ leftoff, ಪೂರ್ಣಾಙ್ಕ zindex);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_options_one(dict_T *dict, char_u *ಕೀಲಿ)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    ಪೂರ್ಣಾಙ್ಕ		n = 0;

    di = dict_find(dict, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ MAXCOL;

    val = tv_get_string(&di->di_tv);
    ಯದಿ (STRNCMP(val, "cursor", 6) != 0)
	ಪ್ರತ್ಯಾಗಮ dict_get_ಸಂಖ್ಯೆ_check(dict, ಕೀಲಿ);

    setcursor_mayforce(TRUE);
    s = val + 6;
    ಯದಿ (*s != NUL)
    {
	endp = s;
	ಯದಿ (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((ವರ್ಣ *)s, (ವರ್ಣ **)&endp, 10);
	ಯದಿ (endp != ರಿಕ್ತ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }

    ಯದಿ (STRCMP(ಕೀಲಿ, "line") == 0)
	n = screen_screenrow() + 1 + n;
    ಅನ್ಯಥಾ // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    ಯದಿ (n == 0)
	n = -1;
    ಪ್ರತ್ಯಾಗಮ n;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_padding_border(dict_T *dict, ಪೂರ್ಣಾಙ್ಕ *array, ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		nr;

    ಪ್ರತಿ (i = 0; i < 4; ++i)
	array[i] = 1;
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    CHECK_LIST_MATERIALIZE(list);
    ಪ್ರತಿ (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_moved_ಮೌಲ್ಯಗಳು(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_moved_columns(win_T *wp, ಪೂರ್ಣಾಙ್ಕ flags)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    ಪೂರ್ಣಾಙ್ಕ		mincol;

    ಯದಿ (len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (ಪೂರ್ಣಾಙ್ಕ)(ptr - ml_get_curline());
    ಯದಿ (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	ಪ್ರತ್ಯಾಗಮ;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_mousemoved_ಮೌಲ್ಯಗಳು(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_popup_uses_mouse_move(ಶೂನ್ಯ)
{
    popup_uses_mouse_move = FALSE;
    ಯದಿ (!popup_visible)
	ಪ್ರತ್ಯಾಗಮ;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_mousemoved_columns(win_T *wp, ಪೂರ್ಣಾಙ್ಕ flags)
{
    win_T	*textwp;
    char_u	*text;
    ಪೂರ್ಣಾಙ್ಕ		col;
    pos_T	pos;
    colnr_T	mcol;

    ಯದಿ (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, ರಿಕ್ತ, &col) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, ರಿಕ್ತ, ರಿಕ್ತ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_on_border(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪ್ರತ್ಯಾಗಮ (row == 0 && wp->w_popup_border[0] > 0)
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
    ಪೂರ್ಣಾಙ್ಕ
popup_close_if_on_X(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// Values set when dragging a popup window starts.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drag_start_row;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drag_start_col;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drag_start_wantline;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drag_start_wantcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    ಶೂನ್ಯ
popup_start_drag(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    ಯದಿ (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    ಅನ್ಯಥಾ
	drag_start_wantline = wp->w_wantline;
    ಯದಿ (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    ಅನ್ಯಥಾ
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    ಯದಿ (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    ಯದಿ (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	ಯದಿ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	ಯದಿ (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    ಶೂನ್ಯ
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    ಯದಿ (!win_valid_popup(wp))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	ಪೂರ್ಣಾಙ್ಕ width_inc = mouse_col - drag_start_col;
	ಪೂರ್ಣಾಙ್ಕ height_inc = mouse_row - drag_start_row;

	ಯದಿ (width_inc != 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ width = wp->w_width + width_inc;

	    ಯದಿ (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	ಯದಿ (height_inc != 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ height = wp->w_height + height_inc;

	    ಯದಿ (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	ಪ್ರತ್ಯಾಗಮ;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    ಯದಿ (wp->w_wantline < 1)
	wp->w_wantline = 1;
    ಯದಿ (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    ಯದಿ (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    ಯದಿ (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    ಶೂನ್ಯ
popup_set_firstline(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    ಯಾವತ್ (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_is_in_scrollbar(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    ಶೂನ್ಯ
popup_handle_scrollbar_click(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (!popup_is_in_scrollbar(wp, row, col))
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	    height = popup_height(wp);
    ಪೂರ್ಣಾಙ್ಕ	    new_topline = wp->w_topline;

    ಯದಿ (row >= height / 2)
    {
	// Click in lower half, scroll down.
	ಯದಿ (wp->w_topline < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ++new_topline;
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    ಯದಿ (new_topline == wp->w_topline)
	ಪ್ರತ್ಯಾಗಮ;

    set_topline(wp, new_topline);
    ಯದಿ (wp == curwin)
    {
	ಯದಿ (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ಯದಿ defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_add_timeout(win_T *wp, ಪೂರ್ಣಾಙ್ಕ time, ಪೂರ್ಣಾಙ್ಕ close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((ವರ್ಣ *)cbbuf, ಪರಿಮಾಣ(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    ಯದಿ (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    ಯದಿ (cb.cb_name != ರಿಕ್ತ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    ಸ್ಥಿರ poppos_T
get_pos_entry(dict_T *d, ಪೂರ್ಣಾಙ್ಕ give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    ಪೂರ್ಣಾಙ್ಕ	    nr;

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ POPPOS_NONE;

    ಪ್ರತಿ (nr = 0; nr < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(poppos_entries); ++nr)
	ಯದಿ (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    ಪ್ರತ್ಯಾಗಮ poppos_entries[nr].pp_val;

    ಯದಿ (give_error)
	semsg(_(e_invalid_argument_str), str);
    ಪ್ರತ್ಯಾಗಮ POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_move_options(win_T *wp, dict_T *d)
{
    ಪೂರ್ಣಾಙ್ಕ		nr;
    char_u	*str;
    dictitem_T	*di;

    ಯದಿ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    ಯದಿ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    ಯದಿ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    ಯದಿ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    ಯದಿ (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    ಯದಿ (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    ಯದಿ (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	ಯದಿ (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	wp->w_popup_prop_type = 0;
	ಯದಿ (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    ಯದಿ (di != ರಿಕ್ತ)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		ಯದಿ (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    ಯದಿ (nr <= 0)
		nr = find_prop_type_id(str, ರಿಕ್ತ);
	    ಯದಿ (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    ಅನ್ಯಥಾ
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    ಯದಿ (di != ರಿಕ್ತ)
	wp->w_popup_prop_id = dict_get_ಸಂಖ್ಯೆ(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_moved_argument(win_T *wp, dictitem_T *di, ಪೂರ್ಣಾಙ್ಕ mousemoved)
{
    ಯದಿ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ರಿಕ್ತ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	ಪೂರ್ಣಾಙ್ಕ	flags = 0;

	ಯದಿ (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	ಯದಿ (flags != 0)
	{
	    ಯದಿ (mousemoved)
		set_mousemoved_columns(wp, flags);
	    ಅನ್ಯಥಾ
		set_moved_columns(wp, flags);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ರಿಕ್ತ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	ಪೂರ್ಣಾಙ್ಕ	    mincol;
	ಪೂರ್ಣಾಙ್ಕ	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	ಯದಿ (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_ಸಂಖ್ಯೆ(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    ಯದಿ (mousemoved)
		wp->w_popup_mouse_row = nr;
	    ಅನ್ಯಥಾ
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    ಯದಿ (nr == 0)
		wp->w_popup_curwin = ರಿಕ್ತ;
	}

	mincol = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	maxcol = tv_get_ಸಂಖ್ಯೆ(&li->li_next->li_tv);
	ಯದಿ (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_highlight(dict_T *dict, ವರ್ಣ *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), name);
    ಅನ್ಯಥಾ
    {
	str = tv_get_string(&di->di_tv);
	ಯದಿ (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_show_curline(win_T *wp)
{
    ಯದಿ (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	ಯದಿ (wp->w_topline < 1)
	    wp->w_topline = 1;
	ಅನ್ಯಥಾ ಯದಿ (wp->w_topline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_topline = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಯಾವತ್ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    ಯದಿ (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ಸ್ಥಿರ char_u *
popup_get_sign_name(win_T *wp)
{
    ಸ್ಥಿರ ವರ್ಣ    buf[30];

    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "popup-%d", wp->w_id);
    ಪ್ರತ್ಯಾಗಮ (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_highlight_curline(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    ಯದಿ ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	ಯದಿ (!sign_exists_by_name(sign_name))
	{
	    ವರ್ಣ *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ರಿಕ್ತ, (char_u *)linehl, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
		    ರಿಕ್ತ, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ಅನ್ಯಥಾ
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	wp->w_firstline = dict_get_ಸಂಖ್ಯೆ(dict, "firstline");
	ಯದಿ (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    ಯದಿ (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	char_u	*title = vim_strsave(str);

	ಯದಿ (title != ರಿಕ್ತ)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    ಯದಿ (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	nr = dict_get_ಸಂಖ್ಯೆ(dict, "opacity");
	ಯದಿ (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	ಅನ್ಯಥಾ ಯದಿ (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	ಅನ್ಯಥಾ ಯದಿ (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    ಯದಿ (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    ಅನ್ಯಥಾ
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ರಿಕ್ತ)
    {
	image_T		    *img;
	image_T		    *cur_img = ರಿಕ್ತ;
	image_placement_T   *place;
	ಪೂರ್ಣಾಙ್ಕ		    cw, ch;

	ಯದಿ (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ರಿಕ್ತ;
	    }
	    ಗಚ್ಛ image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ && wp->w_popup_imagep->img != ರಿಕ್ತ)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	ಯದಿ (img == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (img == cur_img)
	    ಗಚ್ಛ image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	ಯದಿ (place == ರಿಕ್ತ)
	{
	    image_unref(img);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	ಯದಿ (dict_find(dict, (char_u *)"minwidth", -1) == ರಿಕ್ತ)
	    wp->w_minwidth = cw;
	ಯದಿ (dict_find(dict, (char_u *)"maxwidth", -1) == ರಿಕ್ತ)
	    wp->w_maxwidth = cw;
	ಯದಿ (dict_find(dict, (char_u *)"minheight", -1) == ರಿಕ್ತ)
	    wp->w_minheight = ch;
	ಯದಿ (dict_find(dict, (char_u *)"maxheight", -1) == ರಿಕ್ತ)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    ಅನ್ಯಥಾ
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ರಿಕ್ತ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ ok = TRUE;

	ಯದಿ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ರಿಕ್ತ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    ಯದಿ (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    ಅನ್ಯಥಾ
		ok = FALSE;
	}
	ಅನ್ಯಥಾ
	    ok = FALSE;
	ಯದಿ (!ok)
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	ವರ್ಣ *errmsg = update_wincolor(wp, str);

	ಯದಿ (errmsg == ರಿಕ್ತ)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ಅನ್ಯಥಾ
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	ವರ್ಣ *errmsg = update_winhighlight(wp, str);

	ಯದಿ (errmsg == ರಿಕ್ತ)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ಅನ್ಯಥಾ
	    emsg(_(errmsg));
    }

    ಯದಿ (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಯದಿ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ರಿಕ್ತ)
	{
	    emsg(_(e_list_required));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ಪೂರ್ಣಾಙ್ಕ		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    ಯದಿ (list->lv_len == 0)
	    {
		ಪ್ರತಿ (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ರಿಕ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    ಯದಿ (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			ಯದಿ (hl != ರಿಕ್ತ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		ಯದಿ (list->lv_len == 1 && wp->w_border_highlight[0] != ರಿಕ್ತ)
		    ಪ್ರತಿ (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			ಯದಿ (hl != ರಿಕ್ತ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಯದಿ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ಪೂರ್ಣಾಙ್ಕ		i;

	    ಯದಿ (list != ರಿಕ್ತ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		ಪ್ರತಿ (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    ಯದಿ (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		ಯದಿ (list->lv_len == 1)
		    ಪ್ರತಿ (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		ಯದಿ (list->lv_len == 2)
		{
		    ಪ್ರತಿ (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    ಪ್ರತಿ (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	wp->w_zindex = dict_get_ಸಂಖ್ಯೆ(dict, "zindex");
	ಯದಿ (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	ಯದಿ (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ ok = FALSE;

	ಯದಿ (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ರಿಕ್ತ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(di->di_tv.vval.v_list, li)
	    {
		ಯದಿ (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ರಿಕ್ತ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	ಯದಿ (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), "mask");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

#ಯದಿ defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_ಸಂಖ್ಯೆ(dict, "time");
    ಯದಿ (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	set_moved_ಮೌಲ್ಯಗಳು(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	set_mousemoved_ಮೌಲ್ಯಗಳು(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	ಯದಿ (callback.cb_name != ರಿಕ್ತ)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    ಯದಿ (nr != -1)
    {
	ಯದಿ (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	ಯದಿ (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	ಅನ್ಯಥಾ
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    callback_T	callback = get_callback(&di->di_tv);
    ಯದಿ (callback.cb_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
apply_options(win_T *wp, dict_T *dict, ಪೂರ್ಣಾಙ್ಕ create)
{
    ಪೂರ್ಣಾಙ್ಕ		nr;

    apply_move_options(wp, dict);

    ಯದಿ (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    ಯದಿ (apply_general_options(wp, dict) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    ಯದಿ (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    ಯದಿ (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	ಯದಿ (wp->w_firstline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    ಯದಿ (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	ಯದಿ (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ರಿಕ್ತ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ರಿಕ್ತ ? ರಿಕ್ತ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ರಿಕ್ತ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    ಪ್ರತಿ (li = l->lv_first; li != ರಿಕ್ತ; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    plist = di->di_tv.vval.v_list;
	    ಯದಿ (plist != ರಿಕ್ತ)
	    {
		FOR_ALL_LIST_ವಸ್ತುಗಳು(plist, pli)
		{
		    ಯದಿ (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			ಪ್ರತ್ಯಾಗಮ;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    ಯದಿ (dict != ರಿಕ್ತ)
		    {
			ಪೂರ್ಣಾಙ್ಕ col = dict_get_ಸಂಖ್ಯೆ(dict, "col");

			prop_add_common(lnum, col, dict, buf, ರಿಕ್ತ);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_ತುದಿ_extra(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    ಯದಿ (extra == 0 && wp->w_popup_title != ರಿಕ್ತ && *wp->w_popup_title != NUL)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ extra;
}

/*
 * Get the padding plus border at the left.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_ಎಡ_extra(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_height(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_height
	    + popup_ತುದಿ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    ಪ್ರತ್ಯಾಗಮ wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_extra_width(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ಸ್ಥಿರ win_T *
popup_get_clipwin(win_T *wp)
{
    ಯದಿ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (win_valid(wp->w_popup_prop_win))
	ಪ್ರತ್ಯಾಗಮ wp->w_popup_prop_win;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ_extra;
    ಪೂರ್ಣಾಙ್ಕ bot_extra;
    ಪೂರ್ಣಾಙ್ಕ ಎಡ_extra;
    ಪೂರ್ಣಾಙ್ಕ ಬಲ_extra;

    ಪೂರ್ಣಾಙ್ಕ clip_ತುದಿ_content;
    ಪೂರ್ಣಾಙ್ಕ clip_bot_content;
    ಪೂರ್ಣಾಙ್ಕ clip_ಎಡ_content;
    ಪೂರ್ಣಾಙ್ಕ clip_ಬಲ_content;

    ಪೂರ್ಣಾಙ್ಕ eff_ತುದಿ_extra;
    ಪೂರ್ಣಾಙ್ಕ eff_bot_extra;
    ಪೂರ್ಣಾಙ್ಕ eff_ಎಡ_extra;
    ಪೂರ್ಣಾಙ್ಕ eff_ಬಲ_extra;

    ಪೂರ್ಣಾಙ್ಕ eff_border[4];
    ಪೂರ್ಣಾಙ್ಕ eff_padding[4];

    ಪೂರ್ಣಾಙ್ಕ eff_height;
    ಪೂರ್ಣಾಙ್ಕ eff_width;
} popup_clip_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    ಪೂರ್ಣಾಙ್ಕ h, w;

    cl->ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->ಬಲ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_ತುದಿ_content = wp->w_popup_topoff - cl->ತುದಿ_extra;
    ಯದಿ (cl->clip_ತುದಿ_content < 0)
	cl->clip_ತುದಿ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    ಯದಿ (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ಎಡ_content = wp->w_popup_leftclip - cl->ಎಡ_extra;
    ಯದಿ (cl->clip_ಎಡ_content < 0)
	cl->clip_ಎಡ_content = 0;
    cl->clip_ಬಲ_content = wp->w_popup_rightclip - cl->ಬಲ_extra;
    ಯದಿ (cl->clip_ಬಲ_content < 0)
	cl->clip_ಬಲ_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	ಪೂರ್ಣಾಙ್ಕ clip = wp->w_popup_topoff;
	ಪೂರ್ಣಾಙ್ಕ b = wp->w_popup_border[0];
	ಪೂರ್ಣಾಙ್ಕ p = wp->w_popup_padding[0];
	ಪೂರ್ಣಾಙ್ಕ rem;

	ಯದಿ (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	ಅನ್ಯಥಾ
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	ಪೂರ್ಣಾಙ್ಕ clip = wp->w_popup_bottomoff;
	ಪೂರ್ಣಾಙ್ಕ b = wp->w_popup_border[2];
	ಪೂರ್ಣಾಙ್ಕ p = wp->w_popup_padding[2];
	ಪೂರ್ಣಾಙ್ಕ rem;

	ಯದಿ (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	ಅನ್ಯಥಾ
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
	ಪೂರ್ಣಾಙ್ಕ excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	ಯದಿ (excess > 0)
	{
	    ಯದಿ (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		ಯದಿ (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		ಅನ್ಯಥಾ
		    cl->eff_border[0] -= excess;
	    }
	    ಅನ್ಯಥಾ
		cl->eff_padding[0] -= excess;
	}
    }
    {
	ಪೂರ್ಣಾಙ್ಕ excess = wp->w_popup_topoff - cl->ತುದಿ_extra - wp->w_height;
	ಯದಿ (excess > 0)
	{
	    ಯದಿ (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		ಯದಿ (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		ಅನ್ಯಥಾ
		    cl->eff_border[2] -= excess;
	    }
	    ಅನ್ಯಥಾ
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_ತುದಿ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ಎಡ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ಎಡ_extra;
    cl->eff_ಬಲ_extra = wp->w_popup_rightclip > 0 ? 0 : cl->ಬಲ_extra;

    h = wp->w_height - cl->clip_ತುದಿ_content - cl->clip_bot_content;
    ಯದಿ (h < 0)
	h = 0;
    cl->eff_height = cl->eff_ತುದಿ_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_ಎಡ_content - cl->clip_ಬಲ_content;
    ಯದಿ (w < 0)
	w = 0;
    cl->eff_width = cl->eff_ಎಡ_extra + w + cl->eff_ಬಲ_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		height;
    ಪೂರ್ಣಾಙ್ಕ		width;
    ಪೂರ್ಣಾಙ್ಕ		winrow;
    ಪೂರ್ಣಾಙ್ಕ		wincol;
    ಪೂರ್ಣಾಙ್ಕ		leftcol;
    ಪೂರ್ಣಾಙ್ಕ		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
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

    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_screenpos_above_ತುದಿ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	ಪೂರ್ಣಾಙ್ಕ	    *screen_row,
	ಪೂರ್ಣಾಙ್ಕ	    *screen_scol,
	ಪೂರ್ಣಾಙ್ಕ	    *screen_ccol,
	ಪೂರ್ಣಾಙ್ಕ	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    ಪೂರ್ಣಾಙ್ಕ	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (ಪೂರ್ಣಾಙ್ಕ)win_col_off(prop_win) - (ಪೂರ್ಣಾಙ್ಕ)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (ಪೂರ್ಣಾಙ್ಕ)scol + coloff;
    *screen_ccol = (ಪೂರ್ಣಾಙ್ಕ)ccol + coloff;
    *screen_ecol = (ಪೂರ್ಣಾಙ್ಕ)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (ಪೂರ್ಣಾಙ್ಕ)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_hide_for_textprop(win_T *wp)
{
    ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ಪ್ರತ್ಯಾಗಮ;
    wp->w_popup_flags |= POPF_HIDDEN;
    ಯದಿ (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    ಸ್ಥಿರ bool
popup_find_prop_above_ತುದಿ(
	win_T	    *wp,
	win_T	    *prop_win,
	ಪೂರ್ಣಾಙ್ಕ	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    ಯದಿ (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	ಪ್ರತ್ಯಾಗಮ false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    ಯದಿ (first < 1)
	first = 1;
    ಪ್ರತ್ಯಾಗಮ find_prop_in_ಸಾಲುಗಳು(prop_win,
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
    ಸ್ಥಿರ bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    ಪೂರ್ಣಾಙ್ಕ	    extra_h, extra_w;
    ಪೂರ್ಣಾಙ್ಕ	    popup_ತುದಿ, popup_bottom, popup_ಎಡ, popup_ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ	    total_h, total_w;

    ಯದಿ (cw == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    extra_h = popup_ತುದಿ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_ತುದಿ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_ಎಡ = wp->w_wincol;
    popup_ಬಲ = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    ಯದಿ (popup_ತುದಿ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_ತುದಿ;
    ಯದಿ (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    ಯದಿ (popup_ಎಡ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ಎಡ;
    ಯದಿ (popup_ಬಲ > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_ಬಲ - (cw->w_wincol + cw->w_width);

    ಪ್ರತ್ಯಾಗಮ wp->w_popup_topoff >= total_h
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    ಯದಿ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_ತುದಿ_content + cl->clip_bot_content;
	ಯದಿ (wp->w_height < 0)
	    wp->w_height = 0;
	ಯದಿ (cl->clip_ತುದಿ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_ತುದಿ_content;
    }
    ಯದಿ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	ಯದಿ (cl->clip_ಎಡ_content > 0 || cl->clip_ಬಲ_content > 0)
	    wp->w_p_wrap = FALSE;
	ಯದಿ (cl->clip_ಬಲ_content > 0)
	{
	    wp->w_width -= cl->clip_ಬಲ_content;
	    ಯದಿ (wp->w_width < 0)
		wp->w_width = 0;
	}
	ಯದಿ (cl->clip_ಎಡ_content > 0)
	{
	    wp->w_leftcol += cl->clip_ಎಡ_content;
	    wp->w_wincol += cl->clip_ಎಡ_content;
	    wp->w_width -= cl->clip_ಎಡ_content;
	    ಯದಿ (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    ಪೂರ್ಣಾಙ್ಕ iw, ih;

    // Visible cell width and height of popup itself (not image!)
    ಪೂರ್ಣಾಙ್ಕ	visible_width;
    ಪೂರ್ಣಾಙ್ಕ	visible_height;

    // Crop dimensions of image (in pixels).
    ಪೂರ್ಣಾಙ್ಕ	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    ಯದಿ (wp->w_popup_imagep == ರಿಕ್ತ || wp->w_popup_flags & POPF_HIDDEN)
	ಪ್ರತ್ಯಾಗಮ;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    ಯದಿ (wp->w_popup_imagep->img == ರಿಕ್ತ)
	// Just set the bounding box
	ಗಚ್ಛ exit;

    visible_width =
	wp->w_width - clip.clip_ಎಡ_content - clip.clip_ಬಲ_content;
    visible_height =
	wp->w_height - clip.clip_ತುದಿ_content - clip.clip_bot_content;

    ಯದಿ (visible_width < 0 || visible_height < 0)
	ಪ್ರತ್ಯಾಗಮ;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ಎಡ_content;
    crop_y = clip.clip_ತುದಿ_content;
    crop_width = iw - crop_x - clip.clip_ಬಲ_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    ಯದಿ (crop_width > visible_width)
	crop_width = visible_width;
    ಯದಿ (crop_height > visible_height)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		wrapped = 0;
    ಪೂರ್ಣಾಙ್ಕ		maxwidth;
    ಪೂರ್ಣಾಙ್ಕ		maxwidth_no_scrollbar;
    ಪೂರ್ಣಾಙ್ಕ		width_with_scrollbar = 0;
    ಪೂರ್ಣಾಙ್ಕ		used_maxwidth = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		margin_width = 0;
    ಪೂರ್ಣಾಙ್ಕ		maxspace;
    ಪೂರ್ಣಾಙ್ಕ		center_vert = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		center_hor = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		allow_adjust_ಎಡ = !wp->w_popup_fixed;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    ಪೂರ್ಣಾಙ್ಕ		ಬಲ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    ಪೂರ್ಣಾಙ್ಕ		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    ಪೂರ್ಣಾಙ್ಕ		ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    ಪೂರ್ಣಾಙ್ಕ		extra_height = ತುದಿ_extra + bot_extra;
    ಪೂರ್ಣಾಙ್ಕ		extra_width = ಎಡ_extra + ಬಲ_extra;
    ಪೂರ್ಣಾಙ್ಕ		w_height_before_limit;
    popup_layout_T org_layout;
    ಪೂರ್ಣಾಙ್ಕ		minwidth, minheight;
    ಪೂರ್ಣಾಙ್ಕ		maxheight = Rows;
    ಪೂರ್ಣಾಙ್ಕ		wantline = wp->w_wantline;  // adjusted for textprop
    ಪೂರ್ಣಾಙ್ಕ		wantcol = wp->w_wantcol;    // adjusted for textprop
    ಪೂರ್ಣಾಙ್ಕ		use_wantcol = wantcol != 0;
    ಪೂರ್ಣಾಙ್ಕ		adjust_height_for_ತುದಿ_aligned = FALSE;

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
    ಯದಿ (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    ಯದಿ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	ಪೂರ್ಣಾಙ್ಕ	    screen_row;
	ಪೂರ್ಣಾಙ್ಕ	    screen_scol;
	ಪೂರ್ಣಾಙ್ಕ	    screen_ccol;
	ಪೂರ್ಣಾಙ್ಕ	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_ತುದಿ = false;
	ಯದಿ (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    ಯದಿ (popup_find_prop_above_ತುದಿ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_ತುದಿ = true;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    ಯದಿ (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	ಯದಿ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	ಯದಿ (prop_above_ತುದಿ)
	    popup_screenpos_above_ತುದಿ(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	ಅನ್ಯಥಾ
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	ಯದಿ (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	ಅನ್ಯಥಾ
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	ಯದಿ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	ಅನ್ಯಥಾ
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// If no line was specified default to vertical centering.
	ಯದಿ (wantline == 0)
	    center_vert = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	ಯದಿ (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    ಯದಿ (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    ಯದಿ (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	ಯದಿ (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - 1, 0);
	}

	ಯದಿ (!use_wantcol)
	    center_hor = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    ಯದಿ (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- ಎಡ_extra - ಬಲ_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- ಎಡ_extra - ಬಲ_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    ಯದಿ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - ಎಡ_extra - ಬಲ_extra;
    maxwidth = maxspace;
    ಯದಿ (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ಎಡ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    ಯದಿ (wp->w_p_nu || wp->w_p_rnu)
	margin_width = ಸಂಖ್ಯೆ_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    ಯದಿ (signcolumn_on(wp))
	margin_width += 2;
#endif
    ಯದಿ (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    ಯದಿ (wp->w_buffer->b_term != ರಿಕ್ತ)
    {
	ಯದಿ (minwidth == 0)
	    minwidth = 20;
	ಯದಿ (minheight == 0)
	    minheight = 5;
    }
#endif

    ಯದಿ (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    ಯದಿ (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	ಯದಿ (wp->w_firstline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_firstline = 0;
	ಅನ್ಯಥಾ
	    wp->w_topline = wp->w_firstline;
    }
    ಯದಿ (wp->w_topline < 1)
	wp->w_topline = 1;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_topline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	wp->w_topline = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

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
	ಪೂರ್ಣಾಙ್ಕ saved_w_width = wp->w_width;

	ಯದಿ (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	ಪ್ರತಿ (ln = 1; ln <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++ln)
	{
	    ಪೂರ್ಣಾಙ್ಕ len = linetabsize(wp, ln) + margin_width;

	    ಯದಿ (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    ಯದಿ (saved_w_width < len)
		saved_w_width = len;
	    ಯದಿ (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		ಭಙ್ಗ;
	}
	wp->w_width = saved_w_width;
    }
    ಯದಿ (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಅನ್ಯಥಾ
	lnum = wp->w_topline;
    ಯಾವತ್ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಪೂರ್ಣಾಙ್ಕ len;
	ಪೂರ್ಣಾಙ್ಕ w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	ಯದಿ (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	ಯದಿ (len + margin_width > maxwidth
		&& allow_adjust_ಎಡ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    ಪೂರ್ಣಾಙ್ಕ shift_by = len + margin_width - maxwidth;

	    ಯದಿ (shift_by > wp->w_wincol)
	    {
		ಪೂರ್ಣಾಙ್ಕ truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    ಯದಿ (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    ಯದಿ (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	ಯದಿ (wp->w_p_wrap)
	{
	    ಯಾವತ್ (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	ಯದಿ (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    ಯದಿ (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	ಯದಿ (wp->w_firstline < 0)
	    --lnum;
	ಅನ್ಯಥಾ
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	ಯದಿ (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum)
		       + wrapped >= maxheight)
	    ಭಙ್ಗ;
    }

    ಯದಿ (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
#ifdef FEAT_TERMINAL
    ಯದಿ (wp->w_buffer->b_term != ರಿಕ್ತ && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    ಯದಿ (wp->w_has_scrollbar)
    {
	++ಬಲ_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	ಯದಿ (maxwidth + ಬಲ_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    ಯದಿ (wp->w_popup_title != ರಿಕ್ತ && *wp->w_popup_title != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	ಯದಿ (minwidth < title_len)
	    minwidth = title_len;
    }

    ಯದಿ (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    ಯದಿ (wp->w_width > maxwidth)
    {
	ಯದಿ (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	ಯದಿ (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, ಬಲ_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - ಬಲ_extra;
	}
    }
    ಯದಿ (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	ಯದಿ (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	ಪೂರ್ಣಾಙ್ಕ leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	ಯದಿ (leftoff >= 0)
	    wp->w_wincol = leftoff;
	ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    ಯದಿ (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    ಯದಿ (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	ಪೂರ್ಣಾಙ್ಕ want_col = 0;

	// try to show the right border and any scrollbar
	want_col = ಎಡ_extra + wp->w_width + ಬಲ_extra;
	ಯದಿ (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    ಯದಿ (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - wp->w_topline
								 + 1 + wrapped;
    ಯದಿ (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    ಯದಿ (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    ಯದಿ (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    ಯದಿ (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	ಯದಿ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	ಯದಿ ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	ಅನ್ಯಥಾ ಯದಿ (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	ಅನ್ಯಥಾ
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_ತುದಿ_aligned = TRUE;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	ಪೂರ್ಣಾಙ್ಕ check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	ಯದಿ (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    ಯದಿ (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    ಯದಿ (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_ತುದಿ_aligned = TRUE;
	}
    }

    ಯದಿ (adjust_height_for_ತುದಿ_aligned && wp->w_want_scrollbar
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
	ಯದಿ (wp->w_buffer->b_term == ರಿಕ್ತ || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    ಯದಿ (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		ಯದಿ (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		ಅನ್ಯಥಾ
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    ಯದಿ (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    ಯದಿ (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    ಯದಿ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    ಯದಿ (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    ಯದಿ (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	ಪೂರ್ಣಾಙ್ಕ avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    ಯದಿ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	ಯದಿ (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	ಯದಿ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	ಯದಿ (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    ಪೂರ್ಣಾಙ್ಕ avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    ಯದಿ (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    ಯದಿ (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	ಯದಿ (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    ಯದಿ (win_valid(wp->w_popup_prop_win))
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
    ಯದಿ (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	ಯದಿ (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_is_notification(create_type_T type)
{
    ಪ್ರತ್ಯಾಗಮ type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    ಪೂರ್ಣಾಙ್ಕ	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    ಪ್ರತಿ (lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    ಯದಿ (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ರಿಕ್ತ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    ಅನ್ಯಥಾ
    {
	list_T *l = text.vval.v_list;

	ಯದಿ (l != ರಿಕ್ತ && l->lv_len > 0)
	{
	    ಯದಿ (l->lv_first == &range_list_item)
		emsg(_(e_using_ಸಂಖ್ಯೆ_as_string));
	    ಅನ್ಯಥಾ ಯದಿ (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    ಅನ್ಯಥಾ
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    ಕುರು {						    \
	ಯದಿ (wp != ರಿಕ್ತ)					    \
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
    } ಯಾವತ್ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_popup_option(win_T *wp, ಪೂರ್ಣಾಙ್ಕ is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    ಪೂರ್ಣಾಙ್ಕ	    border_enabled = FALSE;

    ಯದಿ (wp != ರಿಕ್ತ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    ಪ್ರತಿ ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	ಪೂರ್ಣಾಙ್ಕ	x;

	e = vim_strchr(p, ':');
	ಯದಿ (e == ರಿಕ್ತ || e[1] == NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	p = vim_strchr(e, ',');
	ಯದಿ (p == ರಿಕ್ತ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	ಯದಿ (STRNCMP(s, "height:", 7) == 0)
	{
	    ಯದಿ (dig != p)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಯದಿ (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "width:", 6) == 0)
	{
	    ಯದಿ (dig != p)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಯದಿ (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "highlight:", 10) == 0)
	{
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ವರ್ಣ *errmsg;
		ಪೂರ್ಣಾಙ್ಕ c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		ಯದಿ (errmsg == ರಿಕ್ತ)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		ಅನ್ಯಥಾ
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    ಯದಿ (*arg == NUL || *arg == ',')
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಙ್ಕ		i;
	    ಪೂರ್ಣಾಙ್ಕ		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    ಪೂರ್ಣಾಙ್ಕ		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    ಯದಿ (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		ಯದಿ (wp != ರಿಕ್ತ)
		{
		    ಪ್ರತಿ (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ಅನುವರ್ತ;
	    }

	    token = vim_strnsave(arg, token_len);
	    ಯದಿ (token == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    ಯದಿ ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		ಯದಿ (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    ಪೂರ್ಣಾಙ್ಕ		out[8];
		    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    ಪ್ರತಿ (i = 0; i < 8 && !failed; i++)
		    {
			ಯದಿ (*q == NUL)
			    failed = TRUE;
			ಅನ್ಯಥಾ
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    ಯದಿ (i < 7)
			    {
				ಯದಿ (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    ಯದಿ (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		vim_free(token);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    ಪೂರ್ಣಾಙ್ಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಙ್ಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಯದಿ (!on && !off)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    on = on && mouse_has(MOUSE_ಸೇರಿಸು) && (border_enabled || is_preview);
	    ಯದಿ (wp != ರಿಕ್ತ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಙ್ಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಙ್ಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಯದಿ (!on && !off)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಯದಿ (on && mouse_has(MOUSE_ಸೇರಿಸು))
		    wp->w_popup_flags |= POPF_RESIZE;
		ಅನ್ಯಥಾ
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಙ್ಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಙ್ಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಯದಿ (!on && !off)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    ಪೂರ್ಣಾಙ್ಕ		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    ಪೂರ್ಣಾಙ್ಕ		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    ಯದಿ (!menu && !item)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(s, "opacity:", 8) == 0)
	{
	    ಯದಿ (dig != p || x < 0 || x > 100)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (wp != ರಿಕ್ತ)
	    {
		ಯದಿ (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		ಅನ್ಯಥಾ
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಪೂರ್ಣಾಙ್ಕ
parse_previewpopup(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಪೂರ್ಣಾಙ್ಕ
parse_completepopup(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    ಶೂನ್ಯ
popup_set_wantpos_cursor(win_T *wp, ಪೂರ್ಣಾಙ್ಕ width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    ಯದಿ (d != ರಿಕ್ತ)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    ಯದಿ (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    ಅನ್ಯಥಾ
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	ಯದಿ (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    ಯದಿ (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	ಯದಿ (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    ಶೂನ್ಯ
popup_set_wantpos_rowcol(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_border_ಎಡ_ಬಲ_padding(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_terminal_exists(ಶೂನ್ಯ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_buffer->b_term != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ಯದಿ (wp->w_buffer->b_term != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    ಶೂನ್ಯ
popup_redraw_all(ಶೂನ್ಯ)
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
    ಪೂರ್ಣಾಙ್ಕ
popup_need_redraw(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set the color for a notification window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_update_color(win_T *wp, create_type_T type)
{
    ವರ್ಣ    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    ವರ್ಣ    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    ಯದಿ (errmsg == ರಿಕ್ತ)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    ಅನ್ಯಥಾ
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ಸ್ಥಿರ win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		tabnr = 0;
    ಪೂರ್ಣಾಙ್ಕ		new_buffer;
    buf_T	*buf = ರಿಕ್ತ;
    dict_T	*d = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (argvars != ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script()
		&& (check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	// Check that arguments look OK.
	ಯದಿ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    buf = buflist_findnr(argvars[0].vval.v_ಸಂಖ್ಯೆ);
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_ಸಂಖ್ಯೆ);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
#ifdef FEAT_TERMINAL
	    ಯದಿ (buf->b_term != ರಿಕ್ತ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ರಿಕ್ತ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ರಿಕ್ತ))
	{
	    emsg(_(e_buffer_ಸಂಖ್ಯೆ_text_or_list_required));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	d = argvars[1].vval.v_dict;
    }

    ಯದಿ (d != ರಿಕ್ತ)
    {
	ಯದಿ (dict_has_ಕೀಲಿ(d, "tabpage"))
	    tabnr = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "tabpage");
	ಅನ್ಯಥಾ ಯದಿ (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	ಅನ್ಯಥಾ
	    tabnr = 0;
	ಯದಿ (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    ಯದಿ (tp == ರಿಕ್ತ)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    ಯದಿ (buf != ರಿಕ್ತ && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (rettv != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    ಯದಿ (buf != ರಿಕ್ತ)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    ಅನ್ಯಥಾ
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(ರಿಕ್ತ, ರಿಕ್ತ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    win_free_popup(wp);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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

    ಯದಿ (tp != ರಿಕ್ತ)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    ಅನ್ಯಥಾ ಯದಿ (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    ಅನ್ಯಥಾ // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	ಯದಿ (first_popupwin == ರಿಕ್ತ)
	    first_popupwin = wp;
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (prev->w_next != ರಿಕ್ತ)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    ಯದಿ (new_buffer && argvars != ರಿಕ್ತ)
	popup_set_buffer_text(buf, argvars[0]);

    ಯದಿ (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    ಯದಿ (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_ಮೌಲ್ಯಗಳು(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    ಯದಿ (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	ಯದಿ (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_ಮೌಲ್ಯಗಳು(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    ಯದಿ (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	ಪೂರ್ಣಾಙ್ಕ	height = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	ಪ್ರತಿ (twp = first_popupwin; twp != ರಿಕ್ತ; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    ಯದಿ (twp != wp
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
	ಯದಿ (wp->w_wantline + height > Rows)
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
	ಪ್ರತಿ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    ಯದಿ (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_ಎಡ_ಬಲ_padding(wp);
    }

    ಯದಿ (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	ಯದಿ (callback.cb_name != ರಿಕ್ತ)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    ಪ್ರತಿ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    ಪ್ರತಿ (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    ಯದಿ (type == TYPE_PREVIEW)
    {
	ಯದಿ (mouse_has(MOUSE_ಸೇರಿಸು))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ಪ್ರತಿ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    ಯದಿ (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	ಯದಿ (mouse_has(MOUSE_ಸೇರಿಸು))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_ಎಡ_ಬಲ_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    ಯದಿ (d != ರಿಕ್ತ)
    {
	// Deal with options.
	ಯದಿ (apply_options(wp, d, TRUE) == FAIL)
	{
	    (ಶೂನ್ಯ)popup_close(wp->w_id, FALSE);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

#ifdef FEAT_TIMERS
    ಯದಿ (popup_is_notification(type) && wp->w_popup_timer == ರಿಕ್ತ)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    ಯದಿ (buf->b_term != ರಿಕ್ತ)
	win_enter(wp, FALSE);
#endif

    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * popup_clear()
 */
    ಶೂನ್ಯ
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ force = FALSE;

    ಯದಿ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
	force = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)wp->w_id;

    ಯದಿ (result != ರಿಕ್ತ && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    ಅನ್ಯಥಾ
    {
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    ಯದಿ (result != ರಿಕ್ತ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
back_to_prevwin(win_T *wp)
{
    ಯದಿ (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    ಅನ್ಯಥಾ
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    ಪೂರ್ಣಾಙ್ಕ id;

    ಯದಿ (!win_valid(wp))
       ಪ್ರತ್ಯಾಗಮ;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    ಯದಿ (wp == curwin && curbuf->b_term != ರಿಕ್ತ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    ಯದಿ (owp != curwin && owp->w_buffer->b_term != ರಿಕ್ತ)
		ಭಙ್ಗ;
	ಯದಿ (owp != ರಿಕ್ತ)
	    win_enter(owp, FALSE);
	ಅನ್ಯಥಾ
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		ಯದಿ (owp != curwin && owp->w_buffer->b_term != ರಿಕ್ತ)
		    ಭಙ್ಗ;
	    ಯದಿ (owp != ರಿಕ್ತ)
		win_enter(owp, FALSE);
	    ಅನ್ಯಥಾ
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    ಯದಿ (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	ಪ್ರತ್ಯಾಗಮ;
    }

    CHECK_CURBUF;
    ಯದಿ (wp->w_close_cb.cb_name != ರಿಕ್ತ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    ಶೂನ್ಯ
popup_close_with_retval(win_T *wp, ಪೂರ್ಣಾಙ್ಕ retval)
{
    typval_T res;

    res.v_type = VAR_ಸಂಖ್ಯೆ;
    res.vval.v_ಸಂಖ್ಯೆ = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    ಶೂನ್ಯ
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    ಯದಿ (wp != mouse_wp
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
    ಶೂನ್ಯ
popup_handle_mouse_moved(ಶೂನ್ಯ)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    ಪೂರ್ಣಾಙ್ಕ	    row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    ಪ್ರತಿ (wp = first_popupwin; wp != ರಿಕ್ತ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    ಪ್ರತಿ (wp = curtab->tp_first_popupwin; wp != ರಿಕ್ತ; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
filter_handle_drag(win_T *wp, ಪೂರ್ಣಾಙ್ಕ c, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ	row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ	col = mouse_col;

    ಯದಿ ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_ಕೀಲಿ(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    ಶೂನ್ಯ
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    char_u	*ಕೀಲಿ;
    typval_T	res;
    ಪೂರ್ಣಾಙ್ಕ		c;
    linenr_T	old_lnum;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = win_id2wp(id);
    ಕೀಲಿ = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    c = *ಕೀಲಿ;
    ಯದಿ (c == K_SPECIAL && ಕೀಲಿ[1] != NUL)
	c = TO_SPECIAL(ಕೀಲಿ[1], ಕೀಲಿ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
    res.v_type = VAR_ಸಂಖ್ಯೆ;

    old_lnum = wp->w_cursor.lnum;
    ಯದಿ (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	ಯದಿ (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	ಅನ್ಯಥಾ
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }
    ಯದಿ (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	ಯದಿ (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ++wp->w_cursor.lnum;
	ಅನ್ಯಥಾ
	    wp->w_cursor.lnum = 1;
    }
    ಯದಿ (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_ಸಂಖ್ಯೆ = -1;
	popup_close_and_callback(wp, &res);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_ಸಂಖ್ಯೆ = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	ಪ್ರತ್ಯಾಗಮ;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    ಶೂನ್ಯ
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    char_u	*ಕೀಲಿ;
    typval_T	res;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = win_id2wp(id);
    ಕೀಲಿ = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    c = *ಕೀಲಿ;
    ಯದಿ (c == CAR && need_wait_return)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (c == K_SPECIAL && ಕೀಲಿ[1] != NUL)
	c = TO_SPECIAL(ಕೀಲಿ[1], ಕೀಲಿ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;

    ಯದಿ (c == 'y' || c == 'Y')
	res.vval.v_ಸಂಖ್ಯೆ = 1;
    ಅನ್ಯಥಾ ಯದಿ (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_ಸಂಖ್ಯೆ = 0;
    ಅನ್ಯಥಾ
    {
	filter_handle_drag(wp, c, rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Invoke callback
    res.v_type = VAR_ಸಂಖ್ಯೆ;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ಸ್ಥಿರ win_T *
find_popup_win(ಪೂರ್ಣಾಙ್ಕ id)
{
    win_T *wp = win_id2wp(id);

    ಯದಿ (wp != ರಿಕ್ತ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * popup_close({id})
 */
    ಶೂನ್ಯ
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    ಯದಿ (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == ರಿಕ್ತ &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_popup_win(id);
    ಯದಿ (wp != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_clear_mask_for(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ r, c;
    ಪೂರ್ಣಾಙ್ಕ row_start, col_start, row_end, col_end;

    ಯದಿ (popup_mask == ರಿಕ್ತ || !popup_visible)
	ಪ್ರತ್ಯಾಗಮ;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (ಪೂರ್ಣಾಙ್ಕ)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (ಪೂರ್ಣಾಙ್ಕ)screen_Columns);

    ಪ್ರತಿ (r = row_start; r < row_end; ++r)
	ಪ್ರತಿ (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    ಶೂನ್ಯ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    ಪೂರ್ಣಾಙ್ಕ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    ಯದಿ (error_if_term_popup_window())
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ಪ್ರತ್ಯಾಗಮ;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    ಯದಿ (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    ಯದಿ (was_visible)
	popup_clear_mask_for(wp);

    ಯದಿ (old_area.active)
	popup_redraw_exposed_area(&old_area);
    ಅನ್ಯಥಾ
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    ಯದಿ (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    ಶೂನ್ಯ
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    ಶೂನ್ಯ
popup_show(win_T *wp)
{
    bool popup_active;

    ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	ಪ್ರತ್ಯಾಗಮ;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    ಯದಿ (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ಸಾಲುಗಳು_valid = 0;
	ಯದಿ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    ಅನ್ಯಥಾ
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    ಶೂನ್ಯ
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    ಯದಿ (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * popup_settext({id}, {text})
 */
    ಶೂನ್ಯ
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    popup_save_area(wp, &old_area);

    ಯದಿ (check_for_string_or_list_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ಸಾಲುಗಳು_valid = 0;
    ಯದಿ (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    ಯದಿ (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    ಶೂನ್ಯ
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಯದಿ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf_from_arg(&argvars[1]);

    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_TERMINAL
    ಯದಿ (buf->b_term != ರಿಕ್ತ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (wp->w_buffer != buf)
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

	ಯದಿ (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    ಪೂರ್ಣಾಙ್ಕ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    ಯದಿ (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    ಯದಿ (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    ಯದಿ (wp == message_win)
	message_win = ರಿಕ್ತ;
#endif

    ಯದಿ (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    ಯದಿ (!old_area.active)
	popup_mask_refresh = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
error_for_popup_window(ಶೂನ್ಯ)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    ಪೂರ್ಣಾಙ್ಕ
error_if_popup_window(ಪೂರ್ಣಾಙ್ಕ also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    ಯದಿ (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == ರಿಕ್ತ)
#endif
	    )
    {
	error_for_popup_window();
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_close(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ರಿಕ್ತ;

    // go through global popups
    ಪ್ರತಿ (wp = first_popupwin; wp != ರಿಕ್ತ; prev = wp, wp = wp->w_next)
	ಯದಿ (wp->w_id == id)
	{
	    ಯದಿ (wp == curwin)
	    {
		ಯದಿ (!force)
		{
		    error_for_popup_window();
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		back_to_prevwin(wp);
	    }
	    ಯದಿ (prev == ರಿಕ್ತ)
		first_popupwin = wp->w_next;
	    ಅನ್ಯಥಾ
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	ಯದಿ (popup_close_tabpage(tp, id, force) == OK)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_close_tabpage(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ force)
{
    win_T	*wp;
    win_T	**ಬೇರು = &tp->tp_first_popupwin;
    win_T	*prev = ರಿಕ್ತ;

    ಪ್ರತಿ (wp = *ಬೇರು; wp != ರಿಕ್ತ; prev = wp, wp = wp->w_next)
	ಯದಿ (wp->w_id == id)
	{
	    ಯದಿ (wp == curwin)
	    {
		ಯದಿ (!force)
		{
		    error_for_popup_window();
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    ಯದಿ (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    ಯದಿ (prev == ರಿಕ್ತ)
		*ಬೇರು = wp->w_next;
	    ಅನ್ಯಥಾ
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
close_all_popups(ಪೂರ್ಣಾಙ್ಕ force)
{
    ಯದಿ (!force && ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (first_popupwin != ರಿಕ್ತ)
	ಯದಿ (popup_close(first_popupwin->w_id, force) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (curtab->tp_first_popupwin != ರಿಕ್ತ)
	ಯದಿ (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    ಪ್ರತಿ (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ಸ್ಥಿರ bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತಿ (i = 0; i < 4; i++)
	ಯದಿ (style->border_highlight[i] != wp->w_border_highlight[i])
	    ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    ಪ್ರತ್ಯಾಗಮ layout->winrow != wp->w_winrow
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
    ಸ್ಥಿರ bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    ಪ್ರತ್ಯಾಗಮ area->winrow != wp->w_winrow
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
    ಸ್ಥಿರ ಶೂನ್ಯ
mark_overlapped_opacity_popup(win_T *wp, ಪೂರ್ಣಾಙ್ಕ area_ತುದಿ, ಪೂರ್ಣಾಙ್ಕ area_bot,
	ಪೂರ್ಣಾಙ್ಕ area_ಎಡ, ಪೂರ್ಣಾಙ್ಕ area_ಬಲ, ಪೂರ್ಣಾಙ್ಕ zindex)
{
    ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_ತುದಿ
	    || wp->w_wincol >= area_ಬಲ
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_ಎಡ)
	ಪ್ರತ್ಯಾಗಮ;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ಸಾಲುಗಳು_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_overlapped_opacity_popups(ಪೂರ್ಣಾಙ್ಕ winrow, ಪೂರ್ಣಾಙ್ಕ wincol, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ width,
	ಪೂರ್ಣಾಙ್ಕ leftoff, ಪೂರ್ಣಾಙ್ಕ zindex)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		area_ತುದಿ = winrow;
    ಪೂರ್ಣಾಙ್ಕ		area_bot = winrow + height;
    ಪೂರ್ಣಾಙ್ಕ		area_ಎಡ = wincol;
    ಪೂರ್ಣಾಙ್ಕ		area_ಬಲ = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_ತುದಿ, area_bot, area_ಎಡ,
		area_ಬಲ, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_ತುದಿ, area_bot, area_ಎಡ,
		area_ಬಲ, zindex);

    ಯದಿ (must_redraw < UPD_VALID)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
refill_pum_bg_row_from_buffer(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ ಎಡ, ಪೂರ್ಣಾಙ್ಕ ಬಲ)
{
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_cp = r;
    ಪೂರ್ಣಾಙ್ಕ		col_cp = ಎಡ;
    win_T	*wp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		row_for_lnum;
    ಪೂರ್ಣಾಙ್ಕ		col_for_lnum = 0;
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		win_text_col;
    ಪೂರ್ಣಾಙ್ಕ		screen_col;
    ಪೂರ್ಣಾಙ್ಕ		soff_base = (r - pum_bg_ತುದಿ) * pum_bg_cols;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    ಪ್ರತಿ (c = ಎಡ; c < ಬಲ; ++c)
    {
	pum_bg_ಸಾಲುಗಳು[soff_base + c] = ' ';
	ಯದಿ (pum_bg_attrs != ರಿಕ್ತ)
	    pum_bg_attrs[soff_base + c] = 0;
	ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
	    pum_bg_ಸಾಲುಗಳು_UC[soff_base + c] = 0;
	ಯದಿ (enc_utf8)
	{
	    ಪೂರ್ಣಾಙ್ಕ k;
	    ಪ್ರತಿ (k = 0; k < MAX_MCO; ++k)
		ಯದಿ (pum_bg_ಸಾಲುಗಳು_C[k] != ರಿಕ್ತ)
		    pum_bg_ಸಾಲುಗಳು_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&ಸಾಲು_cp, &col_cp, IGNORE_POPUP);
    ಯದಿ (wp == ರಿಕ್ತ || ಸಾಲು_cp < 0 || ಸಾಲು_cp >= wp->w_height)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (wp->w_buffer == ರಿಕ್ತ || wp->w_buffer->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Compute the buffer line for this screen row.
    row_for_lnum = ಸಾಲು_cp;
    ಯದಿ (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;	// past end of buffer
    ಯದಿ (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    ಯದಿ (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = ಸಾಲು;
    ಯಾವತ್ (*p != NUL && screen_col < ಬಲ)
    {
	ಪೂರ್ಣಾಙ್ಕ char_cells;
	ಪೂರ್ಣಾಙ್ಕ byte_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ soff = soff_base + screen_col;
	ಪೂರ್ಣಾಙ್ಕ in_range = (screen_col >= ಎಡ && screen_col < ಬಲ);

	ಯದಿ (*p == '\t')
	{
	    ಪೂರ್ಣಾಙ್ಕ ts = (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_ಎಣಿಕೆ = 1;
	    ಯದಿ (in_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = ' ';
		ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = 0;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_ಎಣಿಕೆ = mb_ptr2len(p);
	    ಯದಿ (in_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = *p;
		ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    char_cells = 1;
	    byte_ಎಣಿಕೆ = 1;
	    ಯದಿ (in_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = (*p < 0x20) ? ' ' : *p;
		ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = 0;
	    }
	}
	ಯದಿ (in_range && pum_bg_attrs != ರಿಕ್ತ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_ಎಣಿಕೆ;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_redraw_exposed_area(popup_area_T *area)
{
    ಯದಿ (!area->active)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಯದಿ (pum_bg_ಸಾಲುಗಳು != ರಿಕ್ತ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_ತುದಿ)
    {
	ಪೂರ್ಣಾಙ್ಕ ತುದಿ = MAX(area->winrow, pum_bg_ತುದಿ);
	ಪೂರ್ಣಾಙ್ಕ bot = MIN(area->winrow + area->height, pum_bg_bot);
	ಪೂರ್ಣಾಙ್ಕ ಎಡ = MAX(area->wincol, 0);
	ಪೂರ್ಣಾಙ್ಕ ಬಲ = MIN(area->wincol + area->width, pum_bg_cols);
	ಪೂರ್ಣಾಙ್ಕ r;

	ಪ್ರತಿ (r = ತುದಿ; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, ಎಡ, ಬಲ);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->ಸಾಲುಗಳು);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    ಯದಿ (screen_opacity_popup == ರಿಕ್ತ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	ಪ್ರತ್ಯಾಗಮ;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    ಯದಿ (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->ಸಾಲುಗಳು = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಙ್ಕ,
				  saved_screen->rows * saved_screen->cols);
    ಯದಿ (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    ಯದಿ (saved_screen->ಸಾಲುಗಳು == ರಿಕ್ತ || saved_screen->attrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = 0; r < saved_screen->rows; r++)
    {
	ಪೂರ್ಣಾಙ್ಕ screen_row = saved_screen->start_row + r;

	ಯದಿ (screen_row >= 0 && screen_row < screen_Rows)
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ c = 0; c < saved_screen->cols; c++)
	    {
		ಪೂರ್ಣಾಙ್ಕ screen_col = saved_screen->start_col + c;

		ಯದಿ (screen_col >= 0 && screen_col < screen_Columns)
		{
		    ಪೂರ್ಣಾಙ್ಕ off = ಸಾಲು_Offset[screen_row] + screen_col;
		    ಪೂರ್ಣಾಙ್ಕ save_off = r * saved_screen->cols + c;

		    saved_screen->ಸಾಲುಗಳು[save_off] = Screen_ಸಾಲುಗಳು[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    ಯದಿ (enc_utf8 && saved_screen->linesuc != ರಿಕ್ತ)
			saved_screen->linesuc[save_off] = Screen_ಸಾಲುಗಳು_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_under_popup_area(ಪೂರ್ಣಾಙ್ಕ winrow, ಪೂರ್ಣಾಙ್ಕ wincol, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ leftoff)
{
    ಪೂರ್ಣಾಙ್ಕ	    r;

    ಪ್ರತಿ (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	ಪೂರ್ಣಾಙ್ಕ	    c;
	win_T	    *prev_twp = ರಿಕ್ತ;

	ಯದಿ (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    ಅನುವರ್ತ;
	}

	ಪ್ರತಿ (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    ಸಾಲು_cp = r;
	    ಪೂರ್ಣಾಙ್ಕ	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&ಸಾಲು_cp, &col_cp, IGNORE_POPUP);
	    ಯದಿ (twp != ರಿಕ್ತ && twp != prev_twp)
	    {
		prev_twp = twp;
		ಯದಿ (ಸಾಲು_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (ಶೂನ್ಯ)mouse_comp_pos(twp, &ಸಾಲು_cp, &col_cp, &lnum, ರಿಕ್ತ);
		    redrawWinline(twp, lnum);
		}
		ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    ಶೂನ್ಯ
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // invalid {id}

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    ಯದಿ (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    ಯದಿ (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    ಶೂನ್ಯ
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    ಪೂರ್ಣಾಙ್ಕ		need_redraw = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		need_reposition = FALSE;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // invalid {id}

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (ಶೂನ್ಯ)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    ಯದಿ (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    ಯದಿ (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    ಯದಿ (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    ಯದಿ (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ಸಾಲುಗಳು_valid = 0;
	ಯದಿ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    ಯದಿ (old_style.blend != wp->w_popup_blend)
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

    ಯದಿ (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    ಶೂನ್ಯ
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ_extra;
    ಪೂರ್ಣಾಙ್ಕ		ಎಡ_extra;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // invalid {id}
    ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_ಸಂಖ್ಯೆ(dict, "line", wp->w_winrow + 1);
    dict_add_ಸಂಖ್ಯೆ(dict, "col", wp->w_wincol + 1);
    dict_add_ಸಂಖ್ಯೆ(dict, "width", wp->w_width + ಎಡ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_ಸಂಖ್ಯೆ(dict, "height", wp->w_height + ತುದಿ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_ಸಂಖ್ಯೆ(dict, "core_line", wp->w_winrow + 1 + ತುದಿ_extra);
    dict_add_ಸಂಖ್ಯೆ(dict, "core_col", wp->w_wincol + 1 + ಎಡ_extra);
    dict_add_ಸಂಖ್ಯೆ(dict, "core_width", wp->w_width);
    dict_add_ಸಂಖ್ಯೆ(dict, "core_height", wp->w_height);

    dict_add_ಸಂಖ್ಯೆ(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_ಸಂಖ್ಯೆ(dict, "firstline", wp->w_topline);
    dict_add_ಸಂಖ್ಯೆ(dict, "lastline", wp->w_botline - 1);
    dict_add_ಸಂಖ್ಯೆ(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    ಶೂನ್ಯ
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    FOR_ALL_POPUPWINS(wp)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    ಶೂನ್ಯ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    win_T	*wp;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    row = tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;
    col = tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ಯದಿ (wp != ರಿಕ್ತ && WIN_IS_POPUP(wp))
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_padding_border(dict_T *dict, ಪೂರ್ಣಾಙ್ಕ *array, ವರ್ಣ *name)
{
    list_T  *list;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಯದಿ (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	ಪ್ರತ್ಯಾಗಮ;

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	ಪ್ರತಿ (i = 0; i < 4; ++i)
	    list_append_ಸಂಖ್ಯೆ(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < 4; ++i)
	ಯದಿ (wp->w_border_highlight[i] != ರಿಕ್ತ)
	    ಭಙ್ಗ;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    ಯದಿ (i == 4 && !wp->w_border_highlight_isset)
	ಪ್ರತ್ಯಾಗಮ;

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	ಪ್ರತ್ಯಾಗಮ;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    ಯದಿ (i == 4)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    char_u  buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ	    len;

    ಪ್ರತಿ (i = 0; i < 8; ++i)
	ಯದಿ (wp->w_border_char[i] != 0)
	    ಭಙ್ಗ;
    ಯದಿ (i == 8)
	ಪ್ರತ್ಯಾಗಮ;

    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    ಯದಿ (list != ರಿಕ್ತ)
    {
	ಯದಿ (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	ಅನ್ಯಥಾ
	{
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_lnum);
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mincol);
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	ಪ್ರತ್ಯಾಗಮ;
    }
    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mouse_row);
    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mouse_mincol);
    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    ಶೂನ್ಯ
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		id;
    win_T	*wp;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = find_popup_win(id);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    dict = rettv->vval.v_dict;
    dict_add_ಸಂಖ್ಯೆ(dict, "line", wp->w_wantline);
    dict_add_ಸಂಖ್ಯೆ(dict, "col", wp->w_wantcol);
    dict_add_ಸಂಖ್ಯೆ(dict, "minwidth", wp->w_minwidth);
    dict_add_ಸಂಖ್ಯೆ(dict, "minheight", wp->w_minheight);
    dict_add_ಸಂಖ್ಯೆ(dict, "maxheight", wp->w_maxheight);
    dict_add_ಸಂಖ್ಯೆ(dict, "maxwidth", wp->w_maxwidth);
    dict_add_ಸಂಖ್ಯೆ(dict, "firstline", wp->w_firstline);
    dict_add_ಸಂಖ್ಯೆ(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_ಸಂಖ್ಯೆ(dict, "zindex", wp->w_zindex);
    dict_add_ಸಂಖ್ಯೆ(dict, "fixed", wp->w_popup_fixed);
    ಯದಿ (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	ಯದಿ (pt != ರಿಕ್ತ)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_ಸಂಖ್ಯೆ(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_ಸಂಖ್ಯೆ(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    ಯದಿ (wp->w_popup_imagep != ರಿಕ್ತ && wp->w_popup_imagep->img != ರಿಕ್ತ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	ಯದಿ (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_ಸಂಖ್ಯೆ(dict, "wrap", wp->w_p_wrap);
    dict_add_ಸಂಖ್ಯೆ(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_ಸಂಖ್ಯೆ(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_ಸಂಖ್ಯೆ(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    ಯದಿ (wp->w_scrollbar_highlight != ರಿಕ್ತ)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    ಯದಿ (wp->w_thumb_highlight != ರಿಕ್ತ)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    ಯದಿ (twp->w_id == id)
		ಭಙ್ಗ;
	ಯದಿ (twp != ರಿಕ್ತ)
	    ಭಙ್ಗ;
	++i;
    }
    ಯದಿ (tp == ರಿಕ್ತ)
	i = -1;  // must be global
    ಅನ್ಯಥಾ ಯದಿ (tp == curtab)
	i = 0;
    dict_add_ಸಂಖ್ಯೆ(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    ಯದಿ (wp->w_filter_cb.cb_name != ರಿಕ್ತ)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    ಯದಿ (wp->w_close_cb.cb_name != ರಿಕ್ತ)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(poppos_entries); ++i)
	ಯದಿ (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(ಪೂರ್ಣಾಙ್ಕ)poppos_entries[i].pp_name.length);
	    ಭಙ್ಗ;
	}

    ಯದಿ (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    ಅನ್ಯಥಾ
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#ಯದಿ defined(FEAT_TIMERS)
    dict_add_ಸಂಖ್ಯೆ(dict, "time", wp->w_popup_timer != ರಿಕ್ತ
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    ಪೂರ್ಣಾಙ್ಕ
error_if_term_popup_window(ಶೂನ್ಯ)
{
    ಯದಿ (WIN_IS_POPUP(curwin) && curbuf->b_term != ರಿಕ್ತ
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    ಶೂನ್ಯ
popup_reset_handled(ಪೂರ್ಣಾಙ್ಕ handled_flag)
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
find_next_popup(ಪೂರ್ಣಾಙ್ಕ lowest, ಪೂರ್ಣಾಙ್ಕ handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    ಪೂರ್ಣಾಙ್ಕ	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = ರಿಕ್ತ;
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    ಯದಿ (found_wp != ರಿಕ್ತ)
	found_wp->w_popup_handled |= handled_flag;
    ಪ್ರತ್ಯಾಗಮ found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
invoke_popup_filter(win_T *wp, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    ಪೂರ್ಣಾಙ್ಕ		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    ಯದಿ (c == Ctrl_C)
    {
	ಪೂರ್ಣಾಙ್ಕ save_got_int = got_int;
	ಪೂರ್ಣಾಙ್ಕ was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	ಯದಿ (was_curwin && wp == curwin)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    ಯದಿ (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	ಯದಿ (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    ಯದಿ (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_do_filter(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		res = FALSE;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಪೂರ್ಣಾಙ್ಕ		state;
    ಪೂರ್ಣಾಙ್ಕ		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    ಯದಿ (popup_is_popup(curwin) && curbuf->b_term != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    ಯದಿ (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    recursive = TRUE;

    ಯದಿ (c == K_LEFTMOUSE)
    {
	ಪೂರ್ಣಾಙ್ಕ row = mouse_row;
	ಪೂರ್ಣಾಙ್ಕ col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	ಯದಿ (wp != ರಿಕ್ತ && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    ಯಾವತ್ (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != ರಿಕ್ತ)
	ಯದಿ (wp->w_filter_cb.cb_name != ರಿಕ್ತ
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    ಯದಿ (must_redraw > was_must_redraw)
    {
	ಪೂರ್ಣಾಙ್ಕ save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;

    // When interrupted return FALSE to avoid looping.
    ಪ್ರತ್ಯಾಗಮ res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_no_mapping(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    round;
    win_T   *wp;

    ಪ್ರತಿ (round = 1; round <= 2; ++round)
	ಪ್ರತಿ (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != ರಿಕ್ತ; wp = wp->w_next)
	    ಯದಿ (wp->w_filter_cb.cb_name != ರಿಕ್ತ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    ಶೂನ್ಯ
popup_check_cursor_pos(ಶೂನ್ಯ)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    ಯಾವತ್ ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != ರಿಕ್ತ)
	ಯದಿ (wp->w_popup_curwin != ರಿಕ್ತ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_update_mask(win_T *wp, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    ಪೂರ್ಣಾಙ್ಕ		row, col;

    ಯದಿ (wp->w_popup_mask == ರಿಕ್ತ || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (wp->w_popup_mask_cells != ರಿಕ್ತ
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	ಪ್ರತ್ಯಾಗಮ;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    ಯದಿ (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    ಯದಿ (wp->w_popup_mask_cells == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(wp->w_popup_mask, lio)
    {
	ಪೂರ್ಣಾಙ್ಕ cols, cole;
	ಪೂರ್ಣಾಙ್ಕ ಸಾಲುಗಳು, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (cols < 0)
	    cols = width + cols + 1;
	ಯದಿ (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (cole < 0)
	    cole = width + cole + 1;
	ಯದಿ (cole > width)
	    cole = width;
	li = li->li_next;
	ಸಾಲುಗಳು = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = height + ಸಾಲುಗಳು + 1;
	ಯದಿ (ಸಾಲುಗಳು <= 0)
	    ಸಾಲುಗಳು = 1;
	li = li->li_next;
	linee = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (linee < 0)
	    linee = height + linee + 1;
	ಯದಿ (linee > height)
	    linee = height;

	ಪ್ರತಿ (row = ಸಾಲುಗಳು - 1; row < linee; ++row)
	    ಪ್ರತಿ (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_masked(win_T *wp, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ screencol, ಪೂರ್ಣಾಙ್ಕ screenline)
{
    ಪೂರ್ಣಾಙ್ಕ col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    ಪೂರ್ಣಾಙ್ಕ ಸಾಲು = screenline - wp->w_winrow;

    ಪ್ರತ್ಯಾಗಮ col >= 0 && col < width
	    && ಸಾಲು >= 0 && ಸಾಲು < height
	    && wp->w_popup_mask_cells[ಸಾಲು * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_popup_transparent(win_T *wp, ಪೂರ್ಣಾಙ್ಕ val)
{
    ಯದಿ (wp->w_popup_mask == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ		width = popup_width(wp);
    ಪೂರ್ಣಾಙ್ಕ		height = popup_height(wp);
    listitem_T	*lio, *li;
    ಪೂರ್ಣಾಙ್ಕ		cols, cole;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲುಗಳು, linee;
    ಪೂರ್ಣಾಙ್ಕ		col, ಸಾಲು;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	ಸಾಲುಗಳು = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = height + ಸಾಲುಗಳು + 1;
	li = li->li_next;
	linee = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಯದಿ (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	ಯದಿ (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--ಸಾಲುಗಳು;
	ಯದಿ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = 0;
	ಪ್ರತಿ (ಸಾಲು = ಸಾಲುಗಳು; ಸಾಲು < linee
		&& ಸಾಲು + wp->w_winrow < screen_Rows; ++ಸಾಲು)
	    ಪ್ರತಿ (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(ಸಾಲು + wp->w_winrow) * screen_Columns
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_popup_unhidden(win_T *wp)
{
    ಯದಿ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	ಯದಿ ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	ಅನ್ಯಥಾ ಯದಿ (popup_find_prop_above_ತುದಿ(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	ಯದಿ (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_need_position_adjust(win_T *wp)
{
    ಯದಿ (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // May need to adjust the width if the cursor moved.
    ಪ್ರತ್ಯಾಗಮ wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ಸ್ಥಿರ short *opacity_zindex = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ    opacity_zindex_rows = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_mark_opacity_zindex(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    width;
    ಪೂರ್ಣಾಙ್ಕ	    height;
    ಪೂರ್ಣಾಙ್ಕ	    r, c;

    ಯದಿ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ಪ್ರತ್ಯಾಗಮ;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    ಪ್ರತಿ (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	ಪ್ರತಿ (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    ಪೂರ್ಣಾಙ್ಕ off = r * screen_Columns + c;
	    ಯದಿ (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_win_under_opacity_popup(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    height;
    ಪೂರ್ಣಾಙ್ಕ	    width;
    ಪೂರ್ಣಾಙ್ಕ	    r;

    ಯದಿ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ಪ್ರತ್ಯಾಗಮ;

    height = popup_height(wp);
    width = popup_width(wp);
    ಪ್ರತಿ (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	ಪೂರ್ಣಾಙ್ಕ	    col;
	win_T	    *prev_twp = ರಿಕ್ತ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	ಪ್ರತಿ (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    ಸಾಲು_cp = r;
	    ಪೂರ್ಣಾಙ್ಕ	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&ಸಾಲು_cp, &col_cp, IGNORE_POPUP);
	    ಯದಿ (twp != ರಿಕ್ತ && twp != prev_twp)
	    {
		prev_twp = twp;
		ಯದಿ (ಸಾಲು_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    ಯದಿ (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    ಅನ್ಯಥಾ
#endif
		    {
			(ಶೂನ್ಯ)mouse_comp_pos(twp, &ಸಾಲು_cp, &col_cp, &lnum,
									ರಿಕ್ತ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_cp == twp->w_height)
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
    ಪೂರ್ಣಾಙ್ಕ
popup_is_under_opacity(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (opacity_zindex == ರಿಕ್ತ
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_is_over_opacity(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ಯದಿ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_is_under_opacity_range(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ start_col, ಪೂರ್ಣಾಙ್ಕ end_col)
{
    ಪೂರ್ಣಾಙ್ಕ col;

    ಯದಿ (opacity_zindex == ರಿಕ್ತ
	    || row < 0 || row >= opacity_zindex_rows)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	ಯದಿ (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    ಶೂನ್ಯ
may_update_popup_mask(ಪೂರ್ಣಾಙ್ಕ type)
{
    win_T	*wp;
    short	*mask;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು, col;
    ಪೂರ್ಣಾಙ್ಕ		redraw_all_popups = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    ಯದಿ (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	ಅನ್ಯಥಾ ಯದಿ (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	ಅನ್ಯಥಾ ಯದಿ (popup_need_position_adjust(wp))
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
	ಪೂರ್ಣಾಙ್ಕ has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ಯದಿ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ಯದಿ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	ಯದಿ (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    ಯದಿ (opacity_zindex != ರಿಕ್ತ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * ಪರಿಮಾಣ(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    ಯದಿ (!popup_mask_refresh)
	ಪ್ರತ್ಯಾಗಮ;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    ಯದಿ (redrawing_all_win)
	mask = popup_mask;
    ಅನ್ಯಥಾ
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * ಪರಿಮಾಣ(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    ಯಾವತ್ ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ width;
	ಪೂರ್ಣಾಙ್ಕ height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	ಯದಿ (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    ಯದಿ (wp->w_popup_flags & POPF_HIDDEN)
		ಅನುವರ್ತ;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	ಯದಿ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	ಅನ್ಯಥಾ
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	ಯದಿ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ಅನುವರ್ತ;

	{
	    ಪೂರ್ಣಾಙ್ಕ mask_start = wp->w_winrow;
	    ಪೂರ್ಣಾಙ್ಕ mask_end = mask_start + height;
	    ಪೂರ್ಣಾಙ್ಕ mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    ಪೂರ್ಣಾಙ್ಕ mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    ಪ್ರತಿ (ಸಾಲು = mask_start;
			    ಸಾಲು < mask_end && ಸಾಲು < screen_Rows; ++ಸಾಲು)
		ಪ್ರತಿ (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    ಯದಿ (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(ಸಾಲು, col, FALSE))
			mask[ಸಾಲು * screen_Columns + col] = POPUPMENU_ZINDEX;
		    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_mask_cells == ರಿಕ್ತ
				    || !popup_masked(wp, width, height, col, ಸಾಲು))
			mask[ಸಾಲು * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    ಯದಿ (mask == popup_mask_next)
    {
	ಪೂರ್ಣಾಙ್ಕ	    *plines_cache = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಙ್ಕ, Rows);
	win_T	    *prev_wp = ರಿಕ್ತ;

	ಪ್ರತಿ (ಸಾಲು = 0; ಸಾಲು < screen_Rows; ++ಸಾಲು)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    col_done = 0;

	    ಪ್ರತಿ (col = 0; col < screen_Columns; ++col)
	    {
		ಪೂರ್ಣಾಙ್ಕ off = ಸಾಲು * screen_Columns + col;

		ಯದಿ (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    ಯದಿ (ಸಾಲು >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			ಯದಿ (!ಸಂದೇಶ_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (col >= col_done)
		    {
			linenr_T	lnum;
			ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_cp = ಸಾಲು;
			ಪೂರ್ಣಾಙ್ಕ		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&ಸಾಲು_cp, &col_cp, IGNORE_POPUP);
			ಯದಿ (wp != ರಿಕ್ತ)
			{
#ಯದಿ defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    ಯದಿ (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    ಅನ್ಯಥಾ
#endif
			    {
				ಯದಿ (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * Rows);
				    prev_wp = wp;
				}

				ಯದಿ (ಸಾಲು_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				ಅನ್ಯಥಾ
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (ಶೂನ್ಯ)mouse_comp_pos(wp, &ಸಾಲು_cp, &col_cp,
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
    ಶೂನ್ಯ
may_update_popup_position(ಶೂನ್ಯ)
{
    ಯದಿ (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
ಸ್ಥಿರ schar_T *base_screenlines = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *base_screenattrs = ರಿಕ್ತ;
ಸ್ಥಿರ u8char_T *base_screenlinesuc = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ base_screen_rows = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_get_base_screen_cell(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, schar_T *linep, ಪೂರ್ಣಾಙ್ಕ *attrp,
							  u8char_T *ucp)
{
    ಯದಿ (base_screenlines == ರಿಕ್ತ || base_screenattrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ off = row * base_screen_cols + col;
    ಯದಿ (linep != ರಿಕ್ತ)
	*linep = base_screenlines[off];
    ಯದಿ (attrp != ರಿಕ್ತ)
	*attrp = base_screenattrs[off];
    ಯದಿ (ucp != ರಿಕ್ತ)
    {
	ಯದಿ (enc_utf8 && base_screenlinesuc != ರಿಕ್ತ)
	    *ucp = base_screenlinesuc[off];
	ಅನ್ಯಥಾ
	    *ucp = 0;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    ಶೂನ್ಯ
popup_set_base_screen_cell(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, schar_T ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ attr, u8char_T uc)
{
    ಯದಿ (base_screenlines == ರಿಕ್ತ || base_screenattrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ off = row * base_screen_cols + col;
    base_screenlines[off] = ಸಾಲು;
    base_screenattrs[off] = attr;
    ಯದಿ (enc_utf8 && base_screenlinesuc != ರಿಕ್ತ)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
draw_opacity_padding_cell(
	ಪೂರ್ಣಾಙ್ಕ		row,
	ಪೂರ್ಣಾಙ್ಕ		col,
	popup_saved_screen_T *saved_screen,
	ಪೂರ್ಣಾಙ್ಕ		pad_start_col,
	ಪೂರ್ಣಾಙ್ಕ		pad_end_col)
{
    ಯದಿ (ಸಾಲು_Offset == ರಿಕ್ತ || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ || ScreenAttrs == ರಿಕ್ತ
	    || (enc_utf8 && Screen_ಸಾಲುಗಳು_UC == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ off = ಸಾಲು_Offset[row] + col;
    ಪೂರ್ಣಾಙ್ಕ r = row - saved_screen->start_row;
    ಪೂರ್ಣಾಙ್ಕ c = col - saved_screen->start_col;

    ಯದಿ (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	ಪೂರ್ಣಾಙ್ಕ save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	ಯದಿ (enc_utf8 && saved_screen->linesuc != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ base_col = col - 1;
	    ಪೂರ್ಣಾಙ್ಕ base_off = off - 1;
	    ಪೂರ್ಣಾಙ್ಕ base_save_off = save_off - 1;
	    ಪೂರ್ಣಾಙ್ಕ wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    ಯದಿ (base_off >= 0)
	    {
		ಯದಿ (Screen_ಸಾಲುಗಳು_UC[base_off] != 0
			&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[base_off]) == 2
			&& Screen_ಸಾಲುಗಳು[off] == 0)
		    wide_prev = TRUE;
	    }
	    ಯದಿ (!wide_prev && save_off > 0)
	    {
		ಯದಿ (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->ಸಾಲುಗಳು[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    ಯದಿ (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		ಯದಿ (base_col < pad_start_col)
		{
		    ಯದಿ (Screen_ಸಾಲುಗಳು_UC[base_off] != 0
			    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_ಸಾಲುಗಳು[base_off] = ' ';
			Screen_ಸಾಲುಗಳು_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_ಸಾಲುಗಳು[base_off],
				ScreenAttrs[base_off],
				Screen_ಸಾಲುಗಳು_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_ಸಾಲುಗಳು[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			ಯದಿ (enc_utf8)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ಪೂರ್ಣಾಙ್ಕ popup_attr_val =
					get_win_attr(screen_opacity_popup);
			ಪೂರ್ಣಾಙ್ಕ blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_ಸಾಲುಗಳು[off], ScreenAttrs[off],
				Screen_ಸಾಲುಗಳು_UC[off]);
			screen_char(off, row, col);
			ಪ್ರತ್ಯಾಗಮ;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    ಯದಿ (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_ಸಾಲುಗಳು[base_off] == ' '
			    && Screen_ಸಾಲುಗಳು_UC[base_off] == 0)
		    {
			ಪೂರ್ಣಾಙ್ಕ popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			ಪೂರ್ಣಾಙ್ಕ blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_ಸಾಲುಗಳು[base_off] =
				    saved_screen->ಸಾಲುಗಳು[base_save_off];
			Screen_ಸಾಲುಗಳು_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_ಸಾಲುಗಳು[off] = 0;
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_ಸಾಲುಗಳು[base_off],
				    ScreenAttrs[base_off],
				    Screen_ಸಾಲುಗಳು_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_ಸಾಲುಗಳು[off],
				    ScreenAttrs[off],
				    Screen_ಸಾಲುಗಳು_UC[off]);
			screen_char(base_off, row, base_col);
			ಪ್ರತ್ಯಾಗಮ;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_ಸಾಲುಗಳು[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    ಯದಿ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
		    ಪೂರ್ಣಾಙ್ಕ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಙ್ಕ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_ಸಾಲುಗಳು[off],
					       ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
		    screen_char(off, row, col);
		    ಪ್ರತ್ಯಾಗಮ;
		}

		// Base cell is inside the saved area, redraw the wide char.
		ಯದಿ (save_off > 0)
		{
		    Screen_ಸಾಲುಗಳು[base_off] = saved_screen->ಸಾಲುಗಳು[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_ಸಾಲುಗಳು[off] = saved_screen->ಸಾಲುಗಳು[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_ಸಾಲುಗಳು_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_ಸಾಲುಗಳು_UC[off] = saved_screen->linesuc[save_off];

		    ಪೂರ್ಣಾಙ್ಕ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಙ್ಕ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_ಸಾಲುಗಳು[base_off],
					       ScreenAttrs[base_off], Screen_ಸಾಲುಗಳು_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_ಸಾಲುಗಳು[off],
					       ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
	Screen_ಸಾಲುಗಳು[off] = saved_screen->ಸಾಲುಗಳು[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	ಯದಿ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off] = saved_screen->linesuc != ರಿಕ್ತ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_ಸಾಲುಗಳು[off] = ' ';
	    Screen_ಸಾಲುಗಳು_UC[off] = 0;
	}

	ಪೂರ್ಣಾಙ್ಕ popup_attr_val = get_win_attr(screen_opacity_popup);
	ಪೂರ್ಣಾಙ್ಕ blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_ಸಾಲುಗಳು[off],
				   ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
fill_opacity_padding(
	ಪೂರ್ಣಾಙ್ಕ		start_row,
	ಪೂರ್ಣಾಙ್ಕ		end_row,
	ಪೂರ್ಣಾಙ್ಕ		start_col,
	ಪೂರ್ಣಾಙ್ಕ		end_col,
	popup_saved_screen_T *saved_screen)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ pad_row = start_row; pad_row < end_row; pad_row++)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    ಶೂನ್ಯ
update_popups(ಶೂನ್ಯ (*win_update)(win_T *wp))
{
    win_T   *wp;
    ಪೂರ್ಣಾಙ್ಕ	    ತುದಿ_off;
    ಪೂರ್ಣಾಙ್ಕ	    ಎಡ_extra;
    ಪೂರ್ಣಾಙ್ಕ	    total_width;
    ಪೂರ್ಣಾಙ್ಕ	    total_height;
    ಪೂರ್ಣಾಙ್ಕ	    ತುದಿ_padding;
    ಪೂರ್ಣಾಙ್ಕ	    popup_attr;
    ಪೂರ್ಣಾಙ್ಕ	    border_attr[4];
    ಪೂರ್ಣಾಙ್ಕ	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    ಪೂರ್ಣಾಙ್ಕ	    row;
    ಪೂರ್ಣಾಙ್ಕ	    wincol;
    ಪೂರ್ಣಾಙ್ಕ	    padcol = 0;
    ಪೂರ್ಣಾಙ್ಕ	    padendcol = 0;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    sb_thumb_ತುದಿ = 0;
    ಪೂರ್ಣಾಙ್ಕ	    sb_thumb_height = 0;
    ಪೂರ್ಣಾಙ್ಕ	    attr_scroll = 0;
    ಪೂರ್ಣಾಙ್ಕ	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    ಯದಿ (base_screenlines != ರಿಕ್ತ)
    {
	vim_free(base_screenlines);
	base_screenlines = ರಿಕ್ತ;
    }
    ಯದಿ (base_screenattrs != ರಿಕ್ತ)
    {
	vim_free(base_screenattrs);
	base_screenattrs = ರಿಕ್ತ;
    }
    ಯದಿ (base_screenlinesuc != ರಿಕ್ತ)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = ರಿಕ್ತ;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    ಯಾವತ್ ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	    title_len = 0;
	ಪೂರ್ಣಾಙ್ಕ	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	ಯದಿ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	ಅನ್ಯಥಾ
	    screen_opacity_popup = ರಿಕ್ತ;

#ifdef FEAT_PROP_POPUP
	ಯದಿ (screen_opacity_popup != ರಿಕ್ತ)
	{
	    ಯದಿ (base_screenlines != ರಿಕ್ತ)
	    {
		vim_free(base_screenlines);
		base_screenlines = ರಿಕ್ತ;
	    }
	    ಯದಿ (base_screenattrs != ರಿಕ್ತ)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = ರಿಕ್ತ;
	    }
	    ಯದಿ (base_screenlinesuc != ರಿಕ್ತ)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = ರಿಕ್ತ;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ,
				    base_screen_rows * base_screen_cols);
	    ಯದಿ (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    ಯದಿ (base_screenlines != ರಿಕ್ತ && base_screenattrs != ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = 0; r < base_screen_rows; r++)
		{
		    ಪೂರ್ಣಾಙ್ಕ off = ಸಾಲು_Offset[r];
		    ಪೂರ್ಣಾಙ್ಕ base_off = r * base_screen_cols;
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_ಸಾಲುಗಳು[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			ಯದಿ (enc_utf8 && base_screenlinesuc != ರಿಕ್ತ)
			    base_screenlinesuc[base_off + c] =
					Screen_ಸಾಲುಗಳು_UC[off + c];
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
	ತುದಿ_off = popup_ತುದಿ_extra(wp);
	ಎಡ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	ಯದಿ (wp->w_wincol + ಎಡ_extra < 0)
	    ಎಡ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(ತುದಿ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ಎಡ_extra;

	// Draw the popup text, unless it's off screen.
	ಯದಿ (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    ಯದಿ (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    ಯದಿ (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(ತುದಿ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ಎಡ_extra;

	// Add offset for border and padding if not done already.
	ಯದಿ ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ಎಡ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	ಯದಿ ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += ತುದಿ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	ಯದಿ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	ಅನ್ಯಥಾ
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	ಯದಿ (total_width < 0)
	    total_width = 0;
	ಯದಿ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	ಅನ್ಯಥಾ
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	ಯದಿ (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	ಅನ್ಯಥಾ
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	ಯದಿ (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	ಅನ್ಯಥಾ
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    ಪ್ರತಿ (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    ಯದಿ (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	ಪ್ರತಿ (i = 0; i < 8; ++i)
	    ಯದಿ (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	ಪ್ರತಿ (i = 0; i < 4; ++i)
	{
	    ಯದಿ (wp->w_border_highlight[i] != ರಿಕ್ತ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    ಅನ್ಯಥಾ
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    ಯದಿ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	ಯದಿ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	ಯದಿ (wp->w_popup_title != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    title_attr;

	    ಯದಿ (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ರಿಕ್ತ)
		title_attr = border_attr[0];
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    ಅನ್ಯಥಾ
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    ಯದಿ (title_len > total_width - 2)
	    {
		ಪೂರ್ಣಾಙ್ಕ	title_byte_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		ಯದಿ (title_text != ರಿಕ್ತ)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    ಅನ್ಯಥಾ
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	ತುದಿ_padding = cl.eff_padding[0];
	ಯದಿ (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    ಯದಿ (title_len > 0)
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
	    ಅನ್ಯಥಾ
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    ಯದಿ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (cl.eff_padding[0] == 0 && popup_ತುದಿ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    ತುದಿ_padding = 1;

	ಯದಿ (ತುದಿ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    ಯದಿ (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	ಯದಿ (ತುದಿ_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    ಯದಿ (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && saved_screen.ಸಾಲುಗಳು != ರಿಕ್ತ)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		ಅನ್ಯಥಾ
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		ತುದಿ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && saved_screen.ಸಾಲುಗಳು != ರಿಕ್ತ)
	    {
		fill_opacity_padding(row, row + ತುದಿ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    ಅನ್ಯಥಾ
	    {
		screen_fill(row, row + ತುದಿ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	ಯದಿ (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಪೂರ್ಣಾಙ್ಕ		height = wp->w_height;
	    ಪೂರ್ಣಾಙ್ಕ		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    ಯದಿ (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    ಯದಿ (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    ಯದಿ (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_ತುದಿ = 0;
	    ಅನ್ಯಥಾ
		sb_thumb_ತುದಿ = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    ಯದಿ (wp->w_topline > 1 && sb_thumb_ತುದಿ == 0 && height > 1)
		sb_thumb_ತುದಿ = 1;  // show it's scrolled
	    last = total_height - ತುದಿ_off - wp->w_popup_border[2];
	    ಯದಿ (sb_thumb_ತುದಿ >= last)
		// show at least one character
		sb_thumb_ತುದಿ = last - 1;

	    ಯದಿ (wp->w_scrollbar_highlight != ರಿಕ್ತ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    ಅನ್ಯಥಾ
		attr_scroll = highlight_attr[HLF_PSB];
	    ಯದಿ (wp->w_thumb_highlight != ರಿಕ್ತ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    ಅನ್ಯಥಾ
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	ಪ್ರತಿ (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    ಪೂರ್ಣಾಙ್ಕ	pad_ಎಡ;
	    // left and right padding only needed next to the body
	    ಪೂರ್ಣಾಙ್ಕ do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    ಯದಿ (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    ಯದಿ (do_padding && cl.eff_padding[3] > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ col = wincol + wp->w_popup_border[3];

		// left padding
		pad_ಎಡ = wp->w_popup_padding[3];
		ಯದಿ (col < 0)
		{
		    pad_ಎಡ += col;
		    col = 0;
		}
		ಯದಿ (pad_ಎಡ > 0)
		{
		    ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && saved_screen.ಸಾಲುಗಳು != ರಿಕ್ತ)
			fill_opacity_padding(row, row + 1, col, col + pad_ಎಡ,
				&saved_screen);
		    ಅನ್ಯಥಾ
			screen_fill(row, row + 1, col, col + pad_ಎಡ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    ಯದಿ (wp->w_has_scrollbar)
	    {
		ಪೂರ್ಣಾಙ್ಕ ಸಾಲು = i - ತುದಿ_off;
		ಪೂರ್ಣಾಙ್ಕ scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		ಯದಿ (ಸಾಲು >= 0 && ಸಾಲು < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    ಸಾಲು >= sb_thumb_ತುದಿ
				       && ಸಾಲು < sb_thumb_ತುದಿ + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		ಅನ್ಯಥಾ
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    ಯದಿ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    ಯದಿ (do_padding && cl.eff_padding[1] > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		ಪೂರ್ಣಾಙ್ಕ pad_col_end = pad_col_start + wp->w_popup_padding[1];

		ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && saved_screen.ಸಾಲುಗಳು != ರಿಕ್ತ)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		ಅನ್ಯಥಾ
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	ಯದಿ (wp->w_popup_shadow)
	{
	    ಪೂರ್ಣಾಙ್ಕ col = wincol + total_width;
	    ಪ್ರತಿ (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	ಯದಿ (cl.eff_padding[2] > 0)
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
	    ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && saved_screen.ಸಾಲುಗಳು != ರಿಕ್ತ)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    ಅನ್ಯಥಾ
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	ಯದಿ (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    ಯದಿ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	ಯದಿ (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	ಯದಿ (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ರಿಕ್ತ;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	ಯದಿ (override_success)
	    ತೆಗೆ_highlight_overrides();

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

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
    // In case win_update() called start_search_hl().
    end_ಹುಡುಕು_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_one_popup(win_T *wp, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    typval_T	tv;

    ಯದಿ (wp->w_close_cb.cb_partial != ರಿಕ್ತ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಯದಿ (wp->w_filter_cb.cb_partial != ರಿಕ್ತ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_popups(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    ಪ್ರತಿ (wp = first_popupwin; !abort && wp != ರಿಕ್ತ; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	ಪ್ರತಿ (wp = tp->tp_first_popupwin; !abort && wp != ರಿಕ್ತ; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	ಯದಿ (abort)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ abort;
}

    ಪೂರ್ಣಾಙ್ಕ
popup_is_popup(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ wp->w_popup_flags != 0;
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(ಶೂನ್ಯ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_p_pvw)
	    ಪ್ರತ್ಯಾಗಮ wp;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(ಶೂನ್ಯ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_popup_flags & POPF_INFO)
	    ಪ್ರತ್ಯಾಗಮ wp;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(ಪೂರ್ಣಾಙ್ಕ dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    ಪೂರ್ಣಾಙ್ಕ		by;
    linenr_T	new_topline;

    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    ಯದಿ (new_topline < 1)
	new_topline = 1;
    ಯದಿ (new_topline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	new_topline = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ಪ್ರತ್ಯಾಗಮ true;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ false;
#endif
}

    ಶೂನ್ಯ
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_ಸಂಖ್ಯೆ = message_win == ರಿಕ್ತ ? 0 : message_win->w_id;
#ಅನ್ಯಥಾ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#endif
}

    ಶೂನ್ಯ
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_ಸಂಖ್ಯೆ = wp == ರಿಕ್ತ ? 0 : wp->w_id;
#ಅನ್ಯಥಾ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#endif
}

    ಶೂನ್ಯ
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_ಸಂಖ್ಯೆ = wp == ರಿಕ್ತ ? 0 : wp->w_id;
#ಅನ್ಯಥಾ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#endif
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_create_preview_window(ಪೂರ್ಣಾಙ್ಕ info)
{
    win_T *wp = popup_create(ರಿಕ್ತ, ರಿಕ್ತ, info ? TYPE_INFO : TYPE_PREVIEW);

    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (info)
	wp->w_popup_flags |= POPF_INFO;
    ಅನ್ಯಥಾ
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    ಯದಿ (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    ಅನ್ಯಥಾ ಯದಿ (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    ಅನ್ಯಥಾ
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Close any preview popup.
 */
    ಶೂನ್ಯ
popup_close_preview(ಶೂನ್ಯ)
{
    win_T *wp = popup_find_preview_window();

    ಯದಿ (wp != ರಿಕ್ತ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    ಶೂನ್ಯ
popup_hide_info(ಶೂನ್ಯ)
{
    win_T *wp = popup_find_info_window();

    ಯದಿ (wp != ರಿಕ್ತ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	ಯದಿ (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    ಶೂನ್ಯ
popup_close_info(ಶೂನ್ಯ)
{
    win_T *wp = popup_find_info_window();

    ಯದಿ (wp != ರಿಕ್ತ)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_overlaps_cmdline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ popup_on_cmdline;
}

#ಯದಿ defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(ಶೂನ್ಯ)
{
    ಯದಿ (message_win != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ message_win;

    ಪೂರ್ಣಾಙ್ಕ i;

    message_win = popup_create(ರಿಕ್ತ, ರಿಕ್ತ, TYPE_MESSAGE_WIN);

    ಯದಿ (message_win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    ಪ್ರತಿ (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    ಪ್ರತಿ (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    ಯದಿ (message_win->w_popup_timer != ರಿಕ್ತ)
	message_win->w_popup_timer->tr_keep = TRUE;
    ಪ್ರತ್ಯಾಗಮ message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    ಶೂನ್ಯ
popup_show_message_win(ಶೂನ್ಯ)
{
    ಯದಿ (message_win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_start_message_win_timer(win_T *wp)
{
    ಯದಿ (wp == message_win && start_message_win_timer)
    {
	ಯದಿ (message_win->w_popup_timer != ರಿಕ್ತ)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
popup_message_win_visible(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ message_win != ರಿಕ್ತ
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    ಶೂನ್ಯ
popup_hide_message_win(ಶೂನ್ಯ)
{
    ಯದಿ (message_win != ರಿಕ್ತ)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_ಸಂದೇಶ_didout = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_ಸಂದೇಶ_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ew_ಸಂದೇಶ_didout = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ew_ಸಂದೇಶ_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    ಶೂನ್ಯ
start_echowindow(ಪೂರ್ಣಾಙ್ಕ time_sec)
{
    in_echowindow = TRUE;
    save_ಸಂದೇಶ_didout = ಸಂದೇಶ_didout;
    save_ಸಂದೇಶ_col = ಸಂದೇಶ_col;
    ಸಂದೇಶ_didout = ew_ಸಂದೇಶ_didout;
    ಸಂದೇಶ_col = ew_ಸಂದೇಶ_col;
    ಯದಿ (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    ಶೂನ್ಯ
end_echowindow(ಶೂನ್ಯ)
{
    in_echowindow = FALSE;

    ಯದಿ ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_ಸಂದೇಶ_didout = TRUE;
    ew_ಸಂದೇಶ_col = ಸಂದೇಶ_col == 0 ? 1 : ಸಂದೇಶ_col;
    ಸಂದೇಶ_didout = save_ಸಂದೇಶ_didout;
    ಸಂದೇಶ_col = save_ಸಂದೇಶ_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    ಪೂರ್ಣಾಙ್ಕ
popup_win_closed(win_T *win)
{
    ಪೂರ್ಣಾಙ್ಕ	    round;
    win_T   *wp;
    win_T   *next;
    ಪೂರ್ಣಾಙ್ಕ	    ret = FALSE;

    ಪ್ರತಿ (round = 1; round <= 2; ++round)
	ಪ್ರತಿ (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != ರಿಕ್ತ; wp = next)
	{
	    next = wp->w_next;
	    ಯದಿ (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    ಶೂನ್ಯ
popup_set_title(win_T *wp)
{
    ಯದಿ (wp->w_buffer->b_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    ಯದಿ (wp->w_popup_title != ರಿಕ್ತ)
	vim_snprintf((ವರ್ಣ *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    ಶೂನ್ಯ
popup_update_preview_title(ಶೂನ್ಯ)
{
    win_T *wp = popup_find_preview_window();

    ಯದಿ (wp != ರಿಕ್ತ)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
