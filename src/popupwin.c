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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_PROP_POPUP)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		active;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    schar_T	*ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*attrs;
    u8char_T	*linesuc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rows;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    linenr_T	firstline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_scrollbar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		topoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bottomoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftclip;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rightclip;
} popup_layout_T;

ସ୍ଥିର poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
ସ୍ଥିର win_T *message_win = ଖାଲି;

// Time used for the next ":echowindow" message in msec.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା    start_message_win_timer = FALSE;

ସ୍ଥିର ଶୂନ୍ୟ may_start_message_win_timer(win_T *wp);
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_on_cmdline = FALSE;

ସ୍ଥିର ଶୂନ୍ୟ popup_adjust_position(win_T *wp);
ସ୍ଥିର bool popup_area_changed(win_T *wp, popup_area_T *area);
ସ୍ଥିର ଶୂନ୍ୟ popup_redraw_exposed_area(popup_area_T *area);
ସ୍ଥିର ଶୂନ୍ୟ popup_save_area(win_T *wp, popup_area_T *area);
ସ୍ଥିର ଶୂନ୍ୟ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ସ୍ଥିର ଶୂନ୍ୟ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ସ୍ଥିର bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
ସ୍ଥିର bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
ସ୍ଥିର ଶୂନ୍ୟ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ସ୍ଥିର ଶୂନ୍ୟ popup_save_layout(win_T *wp, popup_layout_T *layout);
ସ୍ଥିର ଶୂନ୍ୟ redraw_under_popup_area(ପୂର୍ଣ୍ଣସଂଖ୍ୟା winrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wincol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftoff);
ସ୍ଥିର ଶୂନ୍ୟ redraw_overlapped_opacity_popups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା winrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wincol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftoff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା zindex);
ସ୍ଥିର ଶୂନ୍ୟ redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_options_one(dict_T *dict, char_u *ଚାବି)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;

    di = dict_find(dict, ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ MAXCOL;

    val = tv_get_string(&di->di_tv);
    ଯଦି (STRNCMP(val, "cursor", 6) != 0)
	ଫେରନ୍ତୁ dict_get_ସଂଖ୍ୟା_check(dict, ଚାବି);

    setcursor_mayforce(TRUE);
    s = val + 6;
    ଯଦି (*s != NUL)
    {
	endp = s;
	ଯଦି (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((ଅକ୍ଷର *)s, (ଅକ୍ଷର **)&endp, 10);
	ଯଦି (endp != ଖାଲି && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    ଫେରନ୍ତୁ 0;
	}
    }

    ଯଦି (STRCMP(ଚାବି, "line") == 0)
	n = screen_screenrow() + 1 + n;
    ଅନ୍ୟଥା // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    ଯଦି (n == 0)
	n = -1;
    ଫେରନ୍ତୁ n;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_padding_border(dict_T *dict, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *array, ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯଦି (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	ଫେରନ୍ତୁ FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    ପାଇଁ (i = 0; i < 4; ++i)
	array[i] = 1;
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    CHECK_LIST_MATERIALIZE(list);
    ପାଇଁ (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_moved_ମୂଲ୍ୟଗୁଡ଼ିକ(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_moved_columns(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincol;

    ଯଦି (len <= 0)
	ଫେରନ୍ତୁ;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ptr - ml_get_curline());
    ଯଦି (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	ଫେରନ୍ତୁ;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_mousemoved_ମୂଲ୍ୟଗୁଡ଼ିକ(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    ସ୍ଥିର ଶୂନ୍ୟ
update_popup_uses_mouse_move(ଶୂନ୍ୟ)
{
    popup_uses_mouse_move = FALSE;
    ଯଦି (!popup_visible)
	ଫେରନ୍ତୁ;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ଫେରନ୍ତୁ;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    ଫେରନ୍ତୁ;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_mousemoved_columns(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    win_T	*textwp;
    char_u	*text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    pos_T	pos;
    colnr_T	mcol;

    ଯଦି (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, ଖାଲି, &col) != OK)
	ଫେରନ୍ତୁ;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ଖାଲି, ଖାଲି, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, ଖାଲି, ଖାଲି, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_on_border(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଫେରନ୍ତୁ (row == 0 && wp->w_popup_border[0] > 0)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_close_if_on_X(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

// Values set when dragging a popup window starts.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drag_start_row;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drag_start_col;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drag_start_wantline;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drag_start_wantcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    ଶୂନ୍ୟ
popup_start_drag(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    ଯଦି (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    ଅନ୍ୟଥା
	drag_start_wantline = wp->w_wantline;
    ଯଦି (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    ଅନ୍ୟଥା
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    ଯଦି (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    ଯଦି (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	ଯଦି (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	ଯଦି (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    ଶୂନ୍ୟ
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    ଯଦି (!win_valid_popup(wp))
	ଫେରନ୍ତୁ;

    ଯଦି ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width_inc = mouse_col - drag_start_col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height_inc = mouse_row - drag_start_row;

	ଯଦି (width_inc != 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = wp->w_width + width_inc;

	    ଯଦି (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	ଯଦି (height_inc != 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା height = wp->w_height + height_inc;

	    ଯଦି (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	ଫେରନ୍ତୁ;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    ଯଦି (wp->w_wantline < 1)
	wp->w_wantline = 1;
    ଯଦି (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    ଯଦି (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    ଯଦି (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    ଶୂନ୍ୟ
popup_set_firstline(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    ଯେତେବେଳେ (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_in_scrollbar(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଫେରନ୍ତୁ wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    ଶୂନ୍ୟ
popup_handle_scrollbar_click(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (!popup_is_in_scrollbar(wp, row, col))
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height = popup_height(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    new_topline = wp->w_topline;

    ଯଦି (row >= height / 2)
    {
	// Click in lower half, scroll down.
	ଯଦି (wp->w_topline < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ++new_topline;
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    ଯଦି (new_topline == wp->w_topline)
	ଫେରନ୍ତୁ;

    set_topline(wp, new_topline);
    ଯଦି (wp == curwin)
    {
	ଯଦି (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	ଅନ୍ୟଥା ଯଦି (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ଯଦି defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_add_timeout(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା time, ପୂର୍ଣ୍ଣସଂଖ୍ୟା close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((ଅକ୍ଷର *)cbbuf, ଆକାର(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    ଯଦି (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	ଫେରନ୍ତୁ;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    ଯଦି (cb.cb_name != ଖାଲି && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    ସ୍ଥିର poppos_T
get_pos_entry(dict_T *d, ପୂର୍ଣ୍ଣସଂଖ୍ୟା give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr;

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ POPPOS_NONE;

    ପାଇଁ (nr = 0; nr < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(poppos_entries); ++nr)
	ଯଦି (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    ଫେରନ୍ତୁ poppos_entries[nr].pp_val;

    ଯଦି (give_error)
	semsg(_(e_invalid_argument_str), str);
    ଫେରନ୍ତୁ POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
apply_move_options(win_T *wp, dict_T *d)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    char_u	*str;
    dictitem_T	*di;

    ଯଦି ((nr = dict_get_ସଂଖ୍ୟା_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    ଯଦି ((nr = dict_get_ସଂଖ୍ୟା_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    ଯଦି ((nr = dict_get_ସଂଖ୍ୟା_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    ଯଦି ((nr = dict_get_ସଂଖ୍ୟା_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    ଯଦି (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    ଯଦି (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    ଯଦି (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	ଯଦି (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    ଯଦି (str != ଖାଲି)
    {
	wp->w_popup_prop_type = 0;
	ଯଦି (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    ଯଦି (di != ଖାଲି)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		ଯଦି (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    ଯଦି (nr <= 0)
		nr = find_prop_type_id(str, ଖାଲି);
	    ଯଦି (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    ଅନ୍ୟଥା
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    ଯଦି (di != ଖାଲି)
	wp->w_popup_prop_id = dict_get_ସଂଖ୍ୟା(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_moved_argument(win_T *wp, dictitem_T *di, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mousemoved)
{
    ଯଦି (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ଖାଲି)
    {
	char_u  *s = di->di_tv.vval.v_string;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags = 0;

	ଯଦି (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	ଅନ୍ୟଥା ଯଦି (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	ଅନ୍ୟଥା ଯଦି (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	ଅନ୍ୟଥା ଯଦି (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	ଯଦି (flags != 0)
	{
	    ଯଦି (mousemoved)
		set_mousemoved_columns(wp, flags);
	    ଅନ୍ୟଥା
		set_moved_columns(wp, flags);
	}
    }
    ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ଖାଲି
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mincol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	ଯଦି (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_ସଂଖ୍ୟା(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    ଯଦି (mousemoved)
		wp->w_popup_mouse_row = nr;
	    ଅନ୍ୟଥା
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    ଯଦି (nr == 0)
		wp->w_popup_curwin = ଖାଲି;
	}

	mincol = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	maxcol = tv_get_ସଂଖ୍ୟା(&li->li_next->li_tv);
	ଯଦି (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	ଅନ୍ୟଥା
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    ସ୍ଥିର ଶୂନ୍ୟ
check_highlight(dict_T *dict, ଅକ୍ଷର *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), name);
    ଅନ୍ୟଥା
    {
	str = tv_get_string(&di->di_tv);
	ଯଦି (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_show_curline(win_T *wp)
{
    ଯଦି (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    ଅନ୍ୟଥା ଯଦି (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	ଯଦି (wp->w_topline < 1)
	    wp->w_topline = 1;
	ଅନ୍ୟଥା ଯଦି (wp->w_topline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    wp->w_topline = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	ଯେତେବେଳେ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    ଯଦି (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ସ୍ଥିର char_u *
popup_get_sign_name(win_T *wp)
{
    ସ୍ଥିର ଅକ୍ଷର    buf[30];

    vim_snprintf(buf, ଆକାର(buf), "popup-%d", wp->w_id);
    ଫେରନ୍ତୁ (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_highlight_curline(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    ଯଦି ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	ଯଦି (!sign_exists_by_name(sign_name))
	{
	    ଅକ୍ଷର *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ଖାଲି, (char_u *)linehl, ଖାଲି, ଖାଲି, ଖାଲି,
		    ଖାଲି, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ଅନ୍ୟଥା
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    ଯଦି (di != ଖାଲି)
    {
	wp->w_firstline = dict_get_ସଂଖ୍ୟା(dict, "firstline");
	ଯଦି (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    ଯଦି (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    ଯଦି (str != ଖାଲି)
    {
	char_u	*title = vim_strsave(str);

	ଯଦି (title != ଖାଲି)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    ଯଦି (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    ଯଦି (di != ଖାଲି)
    {
	nr = dict_get_ସଂଖ୍ୟା(dict, "opacity");
	ଯଦି (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	ଅନ୍ୟଥା ଯଦି (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	ଅନ୍ୟଥା ଯଦି (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	ଅନ୍ୟଥା
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    ଯଦି (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    ଅନ୍ୟଥା
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ଖାଲି)
    {
	image_T		    *img;
	image_T		    *cur_img = ଖାଲି;
	image_placement_T   *place;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    cw, ch;

	ଯଦି (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    ଯଦି (wp->w_popup_imagep != ଖାଲି)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ଖାଲି;
	    }
	    ଯାଅ image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	ଯଦି (wp->w_popup_imagep != ଖାଲି && wp->w_popup_imagep->img != ଖାଲି)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	ଯଦି (img == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (img == cur_img)
	    ଯାଅ image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	ଯଦି (place == ଖାଲି)
	{
	    image_unref(img);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (wp->w_popup_imagep != ଖାଲି)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	ଯଦି (dict_find(dict, (char_u *)"minwidth", -1) == ଖାଲି)
	    wp->w_minwidth = cw;
	ଯଦି (dict_find(dict, (char_u *)"maxwidth", -1) == ଖାଲି)
	    wp->w_maxwidth = cw;
	ଯଦି (dict_find(dict, (char_u *)"minheight", -1) == ଖାଲି)
	    wp->w_minheight = ch;
	ଯଦି (dict_find(dict, (char_u *)"maxheight", -1) == ଖାଲି)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    ଅନ୍ୟଥା
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	ଯଦି (wp->w_popup_imagep != ଖାଲି)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ଖାଲି, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	ଯଦି (wp->w_popup_imagep != ଖାଲି)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    ଯଦି (di != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ok = TRUE;

	ଯଦି (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ଖାଲି)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    ଯଦି (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    ଅନ୍ୟଥା
		ok = FALSE;
	}
	ଅନ୍ୟଥା
	    ok = FALSE;
	ଯଦି (!ok)
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    ଯଦି (str != ଖାଲି)
    {
	ଅକ୍ଷର *errmsg = update_wincolor(wp, str);

	ଯଦି (errmsg == ଖାଲି)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ଅନ୍ୟଥା
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    ଯଦି (str != ଖାଲି)
    {
	ଅକ୍ଷର *errmsg = update_winhighlight(wp, str);

	ଯଦି (errmsg == ଖାଲି)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ଅନ୍ୟଥା
	    emsg(_(errmsg));
    }

    ଯଦି (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    ଯଦି (di != ଖାଲି)
    {
	ଯଦି (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ଖାଲି)
	{
	    emsg(_(e_list_required));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    ଯଦି (list->lv_len == 0)
	    {
		ପାଇଁ (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ଖାଲି;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    ଯଦି (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			ଯଦି (hl != ଖାଲି)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		ଯଦି (list->lv_len == 1 && wp->w_border_highlight[0] != ଖାଲି)
		    ପାଇଁ (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			ଯଦି (hl != ଖାଲି)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    ଯଦି (di != ଖାଲି)
    {
	ଯଦି (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	    ଯଦି (list != ଖାଲି)
	    {
		CHECK_LIST_MATERIALIZE(list);
		ପାଇଁ (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    ଯଦି (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		ଯଦି (list->lv_len == 1)
		    ପାଇଁ (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		ଯଦି (list->lv_len == 2)
		{
		    ପାଇଁ (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    ପାଇଁ (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    ଯଦି (di != ଖାଲି)
    {
	wp->w_zindex = dict_get_ସଂଖ୍ୟା(dict, "zindex");
	ଯଦି (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	ଯଦି (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    ଯଦି (di != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ok = FALSE;

	ଯଦି (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ଖାଲି)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(di->di_tv.vval.v_list, li)
	    {
		ଯଦି (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ଖାଲି
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	ଯଦି (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), "mask");
	    ଫେରନ୍ତୁ FAIL;
	}
    }

#ଯଦି defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_ସଂଖ୍ୟା(dict, "time");
    ଯଦି (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    ଯଦି (di != ଖାଲି)
    {
	set_moved_ମୂଲ୍ୟଗୁଡ଼ିକ(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    ଯଦି (di != ଖାଲି)
    {
	set_mousemoved_ମୂଲ୍ୟଗୁଡ଼ିକ(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    ଯଦି (di != ଖାଲି)
    {
	callback_T	callback = get_callback(&di->di_tv);

	ଯଦି (callback.cb_name != ଖାଲି)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    ଯଦି (nr != -1)
    {
	ଯଦି (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    ଯଦି (str != ଖାଲି)
    {
	ଯଦି (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	ଅନ୍ୟଥା
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    callback_T	callback = get_callback(&di->di_tv);
    ଯଦି (callback.cb_name == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    ଫେରନ୍ତୁ OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_options(win_T *wp, dict_T *dict, ପୂର୍ଣ୍ଣସଂଖ୍ୟା create)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    apply_move_options(wp, dict);

    ଯଦି (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    ଯଦି (apply_general_options(wp, dict) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    ଯଦି (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    ଯଦି (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	ଯଦି (wp->w_firstline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	ଅନ୍ୟଥା ଯଦି (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    ଯଦି (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    ଫେରନ୍ତୁ OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
	ଯଦି (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ଖାଲି ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    ଫେରନ୍ତୁ;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ଖାଲି ? ଖାଲି : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ଖାଲି ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    ପାଇଁ (li = l->lv_first; li != ଖାଲି; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ଫେରନ୍ତୁ;
	    }
	    plist = di->di_tv.vval.v_list;
	    ଯଦି (plist != ଖାଲି)
	    {
		FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(plist, pli)
		{
		    ଯଦି (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			ଫେରନ୍ତୁ;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    ଯଦି (dict != ଖାଲି)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = dict_get_ସଂଖ୍ୟା(dict, "col");

			prop_add_common(lnum, col, dict, buf, ଖାଲି);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_ଶୀର୍ଷ_extra(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    ଯଦି (extra == 0 && wp->w_popup_title != ଖାଲି && *wp->w_popup_title != NUL)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ extra;
}

/*
 * Get the padding plus border at the left.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_ବାମ_extra(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_height(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_height
	    + popup_ଶୀର୍ଷ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    ଫେରନ୍ତୁ wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_extra_width(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ସ୍ଥିର win_T *
popup_get_clipwin(win_T *wp)
{
    ଯଦି (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (win_valid(wp->w_popup_prop_win))
	ଫେରନ୍ତୁ wp->w_popup_prop_win;
    ଫେରନ୍ତୁ ଖାଲି;
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bot_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ_extra;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_ଶୀର୍ଷ_content;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_bot_content;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_ବାମ_content;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_ଡାହାଣ_content;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_ଶୀର୍ଷ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_bot_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_ବାମ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_ଡାହାଣ_extra;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_border[4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_padding[4];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eff_width;
} popup_clip_T;

    ସ୍ଥିର ଶୂନ୍ୟ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h, w;

    cl->ଶୀର୍ଷ_extra = popup_ଶୀର୍ଷ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ବାମ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->ଡାହାଣ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_ଶୀର୍ଷ_content = wp->w_popup_topoff - cl->ଶୀର୍ଷ_extra;
    ଯଦି (cl->clip_ଶୀର୍ଷ_content < 0)
	cl->clip_ଶୀର୍ଷ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    ଯଦି (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ବାମ_content = wp->w_popup_leftclip - cl->ବାମ_extra;
    ଯଦି (cl->clip_ବାମ_content < 0)
	cl->clip_ବାମ_content = 0;
    cl->clip_ଡାହାଣ_content = wp->w_popup_rightclip - cl->ଡାହାଣ_extra;
    ଯଦି (cl->clip_ଡାହାଣ_content < 0)
	cl->clip_ଡାହାଣ_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip = wp->w_popup_topoff;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = wp->w_popup_border[0];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = wp->w_popup_padding[0];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rem;

	ଯଦି (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	ଅନ୍ୟଥା
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip = wp->w_popup_bottomoff;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = wp->w_popup_border[2];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = wp->w_popup_padding[2];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rem;

	ଯଦି (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	ଅନ୍ୟଥା
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	ଯଦି (excess > 0)
	{
	    ଯଦି (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		ଯଦି (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		ଅନ୍ୟଥା
		    cl->eff_border[0] -= excess;
	    }
	    ଅନ୍ୟଥା
		cl->eff_padding[0] -= excess;
	}
    }
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା excess = wp->w_popup_topoff - cl->ଶୀର୍ଷ_extra - wp->w_height;
	ଯଦି (excess > 0)
	{
	    ଯଦି (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		ଯଦି (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		ଅନ୍ୟଥା
		    cl->eff_border[2] -= excess;
	    }
	    ଅନ୍ୟଥା
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_ଶୀର୍ଷ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ବାମ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ବାମ_extra;
    cl->eff_ଡାହାଣ_extra = wp->w_popup_rightclip > 0 ? 0 : cl->ଡାହାଣ_extra;

    h = wp->w_height - cl->clip_ଶୀର୍ଷ_content - cl->clip_bot_content;
    ଯଦି (h < 0)
	h = 0;
    cl->eff_height = cl->eff_ଶୀର୍ଷ_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_ବାମ_content - cl->clip_ଡାହାଣ_content;
    ଯଦି (w < 0)
	w = 0;
    cl->eff_width = cl->eff_ବାମ_extra + w + cl->eff_ଡାହାଣ_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leftcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ସ୍ଥିର ଶୂନ୍ୟ
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

    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_screenpos_above_ଶୀର୍ଷ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_scol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_ccol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)win_col_off(prop_win) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)scol + coloff;
    *screen_ccol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ccol + coloff;
    *screen_ecol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_hide_for_textprop(win_T *wp)
{
    ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ଫେରନ୍ତୁ;
    wp->w_popup_flags |= POPF_HIDDEN;
    ଯଦି (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    ସ୍ଥିର bool
popup_find_prop_above_ଶୀର୍ଷ(
	win_T	    *wp,
	win_T	    *prop_win,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    ଯଦି (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	ଫେରନ୍ତୁ false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    ଯଦି (first < 1)
	first = 1;
    ଫେରନ୍ତୁ find_prop_in_ଧାଡ଼ିଗୁଡ଼ିକ(prop_win,
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
    ସ୍ଥିର bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    extra_h, extra_w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    popup_ଶୀର୍ଷ, popup_bottom, popup_ବାମ, popup_ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    total_h, total_w;

    ଯଦି (cw == ଖାଲି)
	ଫେରନ୍ତୁ false;

    extra_h = popup_ଶୀର୍ଷ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_ଶୀର୍ଷ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_ବାମ = wp->w_wincol;
    popup_ଡାହାଣ = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    ଯଦି (popup_ଶୀର୍ଷ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_ଶୀର୍ଷ;
    ଯଦି (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    ଯଦି (popup_ବାମ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ବାମ;
    ଯଦି (popup_ଡାହାଣ > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_ଡାହାଣ - (cw->w_wincol + cw->w_width);

    ଫେରନ୍ତୁ wp->w_popup_topoff >= total_h
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    ଯଦି (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_ଶୀର୍ଷ_content + cl->clip_bot_content;
	ଯଦି (wp->w_height < 0)
	    wp->w_height = 0;
	ଯଦି (cl->clip_ଶୀର୍ଷ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_ଶୀର୍ଷ_content;
    }
    ଯଦି (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	ଯଦି (cl->clip_ବାମ_content > 0 || cl->clip_ଡାହାଣ_content > 0)
	    wp->w_p_wrap = FALSE;
	ଯଦି (cl->clip_ଡାହାଣ_content > 0)
	{
	    wp->w_width -= cl->clip_ଡାହାଣ_content;
	    ଯଦି (wp->w_width < 0)
		wp->w_width = 0;
	}
	ଯଦି (cl->clip_ବାମ_content > 0)
	{
	    wp->w_leftcol += cl->clip_ବାମ_content;
	    wp->w_wincol += cl->clip_ବାମ_content;
	    wp->w_width -= cl->clip_ବାମ_content;
	    ଯଦି (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା iw, ih;

    // Visible cell width and height of popup itself (not image!)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	visible_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	visible_height;

    // Crop dimensions of image (in pixels).
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    ଯଦି (wp->w_popup_imagep == ଖାଲି || wp->w_popup_flags & POPF_HIDDEN)
	ଫେରନ୍ତୁ;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    ଯଦି (wp->w_popup_imagep->img == ଖାଲି)
	// Just set the bounding box
	ଯାଅ exit;

    visible_width =
	wp->w_width - clip.clip_ବାମ_content - clip.clip_ଡାହାଣ_content;
    visible_height =
	wp->w_height - clip.clip_ଶୀର୍ଷ_content - clip.clip_bot_content;

    ଯଦି (visible_width < 0 || visible_height < 0)
	ଫେରନ୍ତୁ;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ବାମ_content;
    crop_y = clip.clip_ଶୀର୍ଷ_content;
    crop_width = iw - crop_x - clip.clip_ଡାହାଣ_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    ଯଦି (crop_width > visible_width)
	crop_width = visible_width;
    ଯଦି (crop_height > visible_height)
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrapped = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth_no_scrollbar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width_with_scrollbar = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_maxwidth = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		margin_width = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxspace;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		center_vert = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		center_hor = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allow_adjust_ବାମ = !wp->w_popup_fixed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଶୀର୍ଷ_extra = popup_ଶୀର୍ଷ_extra(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଡାହାଣ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବାମ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_height = ଶୀର୍ଷ_extra + bot_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_width = ବାମ_extra + ଡାହାଣ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w_height_before_limit;
    popup_layout_T org_layout;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minwidth, minheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxheight = Rows;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wantline = wp->w_wantline;  // adjusted for textprop
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wantcol = wp->w_wantcol;    // adjusted for textprop
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_wantcol = wantcol != 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		adjust_height_for_ଶୀର୍ଷ_aligned = FALSE;

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
    ଯଦି (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    ଯଦି (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    screen_row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    screen_scol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    screen_ccol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_ଶୀର୍ଷ = false;
	ଯଦି (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    ଯଦି (popup_find_prop_above_ଶୀର୍ଷ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_ଶୀର୍ଷ = true;
	    ଅନ୍ୟଥା
	    {
		ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    ଯଦି (wp->w_popup_imagep != ଖାଲି)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    ଯଦି (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		ଫେରନ୍ତୁ;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	ଯଦି (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	ଯଦି (prop_above_ଶୀର୍ଷ)
	    popup_screenpos_above_ଶୀର୍ଷ(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	ଅନ୍ୟଥା
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	ଯଦି (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	ଅନ୍ୟଥା
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	ଯଦି (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	ଅନ୍ୟଥା
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// If no line was specified default to vertical centering.
	ଯଦି (wantline == 0)
	    center_vert = TRUE;
	ଅନ୍ୟଥା ଯଦି (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	ଯଦି (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    ଯଦି (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    ଯଦି (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	ଯଦି (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା - 1, 0);
	}

	ଯଦି (!use_wantcol)
	    center_hor = TRUE;
	ଅନ୍ୟଥା ଯଦି (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    ଯଦି (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- ବାମ_extra - ଡାହାଣ_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- ବାମ_extra - ଡାହାଣ_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    ଯଦି (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - ବାମ_extra - ଡାହାଣ_extra;
    maxwidth = maxspace;
    ଯଦି (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ବାମ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    ଯଦି (wp->w_p_nu || wp->w_p_rnu)
	margin_width = ସଂଖ୍ୟା_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    ଯଦି (signcolumn_on(wp))
	margin_width += 2;
#endif
    ଯଦି (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    ଯଦି (wp->w_buffer->b_term != ଖାଲି)
    {
	ଯଦି (minwidth == 0)
	    minwidth = 20;
	ଯଦି (minheight == 0)
	    minheight = 5;
    }
#endif

    ଯଦି (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    ଅନ୍ୟଥା ଯଦି (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    ଯଦି (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	ଯଦି (wp->w_firstline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    wp->w_firstline = 0;
	ଅନ୍ୟଥା
	    wp->w_topline = wp->w_firstline;
    }
    ଯଦି (wp->w_topline < 1)
	wp->w_topline = 1;
    ଅନ୍ୟଥା ଯଦି (wp->w_topline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	wp->w_topline = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_w_width = wp->w_width;

	ଯଦି (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	ପାଇଁ (ln = 1; ln <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++ln)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = linetabsize(wp, ln) + margin_width;

	    ଯଦି (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    ଯଦି (saved_w_width < len)
		saved_w_width = len;
	    ଯଦି (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		ଭାଙ୍ଗ;
	}
	wp->w_width = saved_w_width;
    }
    ଯଦି (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଅନ୍ୟଥା
	lnum = wp->w_topline;
    ଯେତେବେଳେ (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	ଯଦି (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	ଯଦି (len + margin_width > maxwidth
		&& allow_adjust_ବାମ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift_by = len + margin_width - maxwidth;

	    ଯଦି (shift_by > wp->w_wincol)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    ଯଦି (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    ଯଦି (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	ଯଦି (wp->w_p_wrap)
	{
	    ଯେତେବେଳେ (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	ଯଦି (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    ଯଦି (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	ଯଦି (wp->w_firstline < 0)
	    --lnum;
	ଅନ୍ୟଥା
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	ଯଦି (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum)
		       + wrapped >= maxheight)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
#ifdef FEAT_TERMINAL
    ଯଦି (wp->w_buffer->b_term != ଖାଲି && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    ଯଦି (wp->w_has_scrollbar)
    {
	++ଡାହାଣ_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	ଯଦି (maxwidth + ଡାହାଣ_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    ଯଦି (wp->w_popup_title != ଖାଲି && *wp->w_popup_title != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	ଯଦି (minwidth < title_len)
	    minwidth = title_len;
    }

    ଯଦି (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    ଯଦି (wp->w_width > maxwidth)
    {
	ଯଦି (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	ଯଦି (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, ଡାହାଣ_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	ଅନ୍ୟଥା
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - ଡାହାଣ_extra;
	}
    }
    ଯଦି (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	ଯଦି (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	ଯଦି (leftoff >= 0)
	    wp->w_wincol = leftoff;
	ଅନ୍ୟଥା ଯଦି (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    ଯଦି (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    ଯଦି (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_col = 0;

	// try to show the right border and any scrollbar
	want_col = ବାମ_extra + wp->w_width + ଡାହାଣ_extra;
	ଯଦି (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    ଯଦି (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା - wp->w_topline
								 + 1 + wrapped;
    ଯଦି (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    ଯଦି (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    ଯଦି (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    ଯଦି (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	ଯଦି (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	ଯଦି ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	ଅନ୍ୟଥା ଯଦି (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	ଅନ୍ୟଥା
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_ଶୀର୍ଷ_aligned = TRUE;
	}
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	ଯଦି (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    ଯଦି (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    ଯଦି (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	ଅନ୍ୟଥା
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_ଶୀର୍ଷ_aligned = TRUE;
	}
    }

    ଯଦି (adjust_height_for_ଶୀର୍ଷ_aligned && wp->w_want_scrollbar
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
	ଯଦି (wp->w_buffer->b_term == ଖାଲି || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    ଯଦି (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		ଯଦି (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		ଅନ୍ୟଥା
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    ଯଦି (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    ଅନ୍ୟଥା ଯଦି (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    ଯଦି (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    ଯଦି (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    ଯଦି (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    ଯଦି (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    ଯଦି (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	ଯଦି (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	ଯଦି (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	ଯଦି (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    ଯଦି (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    ଯଦି (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	ଯଦି (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    ଯଦି (wp->w_popup_imagep != ଖାଲି)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	ଫେରନ୍ତୁ;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    ଯଦି (win_valid(wp->w_popup_prop_win))
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
    ଯଦି (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	ଯଦି (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_notification(create_type_T type)
{
    ଫେରନ୍ତୁ type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    ପାଇଁ (lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    ଯଦି (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ଖାଲି ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    ଅନ୍ୟଥା
    {
	list_T *l = text.vval.v_list;

	ଯଦି (l != ଖାଲି && l->lv_len > 0)
	{
	    ଯଦି (l->lv_first == &range_list_item)
		emsg(_(e_using_ସଂଖ୍ୟା_as_string));
	    ଅନ୍ୟଥା ଯଦି (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    ଅନ୍ୟଥା
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    କର {						    \
	ଯଦି (wp != ଖାଲି)					    \
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
    } ଯେତେବେଳେ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_popup_option(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    border_enabled = FALSE;

    ଯଦି (wp != ଖାଲି)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    ପାଇଁ ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x;

	e = vim_strchr(p, ':');
	ଯଦି (e == ଖାଲି || e[1] == NUL)
	    ଫେରନ୍ତୁ FAIL;

	p = vim_strchr(e, ',');
	ଯଦି (p == ଖାଲି)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	ଯଦି (STRNCMP(s, "height:", 7) == 0)
	{
	    ଯଦି (dig != p)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
	    {
		ଯଦି (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "width:", 6) == 0)
	{
	    ଯଦି (dig != p)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
	    {
		ଯଦି (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "highlight:", 10) == 0)
	{
	    ଯଦି (wp != ଖାଲି)
	    {
		ଅକ୍ଷର *errmsg;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		ଯଦି (errmsg == ଖାଲି)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		ଅନ୍ୟଥା
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    ଯଦି (*arg == NUL || *arg == ',')
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    ଯଦି (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		ଯଦି (wp != ଖାଲି)
		{
		    ପାଇଁ (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ଜାରି;
	    }

	    token = vim_strnsave(arg, token_len);
	    ଯଦି (token == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		ଯଦି (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		ଅନ୍ୟଥା ଯଦି (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		ଅନ୍ୟଥା ଯଦି (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		ଅନ୍ୟଥା ଯଦି (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		ଅନ୍ୟଥା ଯଦି (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		ଅନ୍ୟଥା ଯଦି (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		out[8];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    ପାଇଁ (i = 0; i < 8 && !failed; i++)
		    {
			ଯଦି (*q == NUL)
			    failed = TRUE;
			ଅନ୍ୟଥା
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    ଯଦି (i < 7)
			    {
				ଯଦି (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    ଯଦି (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			ଫେରନ୍ତୁ FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		vim_free(token);
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (wp != ଖାଲି)
	    {
		ପାଇଁ (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ଯଦି (!on && !off)
		ଫେରନ୍ତୁ FAIL;
	    on = on && mouse_has(MOUSE_ଭର୍ତ୍ତି) && (border_enabled || is_preview);
	    ଯଦି (wp != ଖାଲି)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ଯଦି (!on && !off)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
	    {
		ଯଦି (on && mouse_has(MOUSE_ଭର୍ତ୍ତି))
		    wp->w_popup_flags |= POPF_RESIZE;
		ଅନ୍ୟଥା
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ଯଦି (!on && !off)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    ଯଦି (!menu && !item)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(s, "opacity:", 8) == 0)
	{
	    ଯଦି (dig != p || x < 0 || x > 100)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (wp != ଖାଲି)
	    {
		ଯଦି (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		ଅନ୍ୟଥା
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_previewpopup(win_T *wp)
{
    ଫେରନ୍ତୁ parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_completepopup(win_T *wp)
{
    ଫେରନ୍ତୁ parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    ଶୂନ୍ୟ
popup_set_wantpos_cursor(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    ଯଦି (d != ଖାଲି)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    ଯଦି (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    ଅନ୍ୟଥା
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	ଯଦି (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    ଯଦି (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	ଯଦି (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    ଶୂନ୍ୟ
popup_set_wantpos_rowcol(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_border_ବାମ_ଡାହାଣ_padding(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_terminal_exists(ଶୂନ୍ୟ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp->w_buffer->b_term != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ଯଦି (wp->w_buffer->b_term != ଖାଲି)
		ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    ଶୂନ୍ୟ
popup_redraw_all(ଶୂନ୍ୟ)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_need_redraw(ଶୂନ୍ୟ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ଫେରନ୍ତୁ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set the color for a notification window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_update_color(win_T *wp, create_type_T type)
{
    ଅକ୍ଷର    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    ଅକ୍ଷର    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    ଯଦି (errmsg == ଖାଲି)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    ଅନ୍ୟଥା
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ସ୍ଥିର win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabnr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_buffer;
    buf_T	*buf = ଖାଲି;
    dict_T	*d = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (argvars != ଖାଲି)
    {
	ଯଦି (in_vim9script()
		&& (check_for_string_or_ସଂଖ୍ୟା_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    ଫେରନ୍ତୁ ଖାଲି;

	// Check that arguments look OK.
	ଯଦି (argvars[0].v_type == VAR_ସଂଖ୍ୟା)
	{
	    buf = buflist_findnr(argvars[0].vval.v_ସଂଖ୍ୟା);
	    ଯଦି (buf == ଖାଲି)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_ସଂଖ୍ୟା);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
#ifdef FEAT_TERMINAL
	    ଯଦି (buf->b_term != ଖାଲି && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
#endif
	}
	ଅନ୍ୟଥା ଯଦି (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ଖାଲି)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ଖାଲି))
	{
	    emsg(_(e_buffer_ସଂଖ୍ୟା_text_or_list_required));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	d = argvars[1].vval.v_dict;
    }

    ଯଦି (d != ଖାଲି)
    {
	ଯଦି (dict_has_ଚାବି(d, "tabpage"))
	    tabnr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "tabpage");
	ଅନ୍ୟଥା ଯଦି (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	ଅନ୍ୟଥା
	    tabnr = 0;
	ଯଦି (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    ଯଦି (tp == ଖାଲି)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    ଯଦି (buf != ଖାଲି && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (rettv != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    ଯଦି (buf != ଖାଲି)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    ଅନ୍ୟଥା
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(ଖାଲି, ଖାଲି, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	ଯଦି (buf == ଖାଲି)
	{
	    win_free_popup(wp);
	    ଫେରନ୍ତୁ ଖାଲି;
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

    ଯଦି (tp != ଖାଲି)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    ଅନ୍ୟଥା ଯଦି (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    ଅନ୍ୟଥା // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	ଯଦି (first_popupwin == ଖାଲି)
	    first_popupwin = wp;
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (prev->w_next != ଖାଲି)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    ଯଦି (new_buffer && argvars != ଖାଲି)
	popup_set_buffer_text(buf, argvars[0]);

    ଯଦି (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    ଯଦି (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_ମୂଲ୍ୟଗୁଡ଼ିକ(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    ଯଦି (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	ଯଦି (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_ମୂଲ୍ୟଗୁଡ଼ିକ(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    ଯଦି (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	height = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	ପାଇଁ (twp = first_popupwin; twp != ଖାଲି; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    ଯଦି (twp != wp
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
	ଯଦି (wp->w_wantline + height > Rows)
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
	ପାଇଁ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    ଯଦି (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_ବାମ_ଡାହାଣ_padding(wp);
    }

    ଯଦି (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	ଯଦି (callback.cb_name != ଖାଲି)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    ପାଇଁ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    ପାଇଁ (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    ଯଦି (type == TYPE_PREVIEW)
    {
	ଯଦି (mouse_has(MOUSE_ଭର୍ତ୍ତି))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ପାଇଁ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    ଯଦି (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	ଯଦି (mouse_has(MOUSE_ଭର୍ତ୍ତି))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_ବାମ_ଡାହାଣ_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    ଯଦି (d != ଖାଲି)
    {
	// Deal with options.
	ଯଦି (apply_options(wp, d, TRUE) == FAIL)
	{
	    (ଶୂନ୍ୟ)popup_close(wp->w_id, FALSE);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

#ifdef FEAT_TIMERS
    ଯଦି (popup_is_notification(type) && wp->w_popup_timer == ଖାଲି)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    ଯଦି (buf->b_term != ଖାଲି)
	win_enter(wp, FALSE);
#endif

    ଫେରନ୍ତୁ wp;
}

/*
 * popup_clear()
 */
    ଶୂନ୍ୟ
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା force = FALSE;

    ଯଦି (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	force = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_ସଂଖ୍ୟା;
    argv[0].vval.v_ସଂଖ୍ୟା = (varnumber_T)wp->w_id;

    ଯଦି (result != ଖାଲି && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    ଅନ୍ୟଥା
    {
	argv[1].v_type = VAR_ସଂଖ୍ୟା;
	argv[1].vval.v_ସଂଖ୍ୟା = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    ଯଦି (result != ଖାଲି)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
back_to_prevwin(win_T *wp)
{
    ଯଦି (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    ଅନ୍ୟଥା
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;

    ଯଦି (!win_valid(wp))
       ଫେରନ୍ତୁ;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    ଯଦି (wp == curwin && curbuf->b_term != ଖାଲି)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    ଯଦି (owp != curwin && owp->w_buffer->b_term != ଖାଲି)
		ଭାଙ୍ଗ;
	ଯଦି (owp != ଖାଲି)
	    win_enter(owp, FALSE);
	ଅନ୍ୟଥା
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		ଯଦି (owp != curwin && owp->w_buffer->b_term != ଖାଲି)
		    ଭାଙ୍ଗ;
	    ଯଦି (owp != ଖାଲି)
		win_enter(owp, FALSE);
	    ଅନ୍ୟଥା
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    ଯଦି (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	ଫେରନ୍ତୁ;
    }

    CHECK_CURBUF;
    ଯଦି (wp->w_close_cb.cb_name != ଖାଲି)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    ଶୂନ୍ୟ
popup_close_with_retval(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval)
{
    typval_T res;

    res.v_type = VAR_ସଂଖ୍ୟା;
    res.vval.v_ସଂଖ୍ୟା = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    ଶୂନ୍ୟ
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ସ୍ଥିର ଶୂନ୍ୟ
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    ଯଦି (wp != mouse_wp
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
    ଶୂନ୍ୟ
popup_handle_mouse_moved(ଶୂନ୍ୟ)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    ପାଇଁ (wp = first_popupwin; wp != ଖାଲି; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    ପାଇଁ (wp = curtab->tp_first_popupwin; wp != ଖାଲି; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
filter_handle_drag(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col = mouse_col;

    ଯଦି ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_ଚାବି(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_ସଂଖ୍ୟା = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    ଶୂନ୍ୟ
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    char_u	*ଚାବି;
    typval_T	res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    linenr_T	old_lnum;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    id = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    wp = win_id2wp(id);
    ଚାବି = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    c = *ଚାବି;
    ଯଦି (c == K_SPECIAL && ଚାବି[1] != NUL)
	c = TO_SPECIAL(ଚାବି[1], ଚାବି[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
    res.v_type = VAR_ସଂଖ୍ୟା;

    old_lnum = wp->w_cursor.lnum;
    ଯଦି (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	ଯଦି (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	ଅନ୍ୟଥା
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    }
    ଯଦି (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	ଯଦି (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ++wp->w_cursor.lnum;
	ଅନ୍ୟଥା
	    wp->w_cursor.lnum = 1;
    }
    ଯଦି (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	ଫେରନ୍ତୁ;
    }

    ଯଦି (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_ସଂଖ୍ୟା = -1;
	popup_close_and_callback(wp, &res);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_ସଂଖ୍ୟା = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	ଫେରନ୍ତୁ;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    ଶୂନ୍ୟ
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    char_u	*ଚାବି;
    typval_T	res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    id = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    wp = win_id2wp(id);
    ଚାବି = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    c = *ଚାବି;
    ଯଦି (c == CAR && need_wait_return)
	ଫେରନ୍ତୁ;
    ଯଦି (c == K_SPECIAL && ଚାବି[1] != NUL)
	c = TO_SPECIAL(ଚାବି[1], ଚାବି[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;

    ଯଦି (c == 'y' || c == 'Y')
	res.vval.v_ସଂଖ୍ୟା = 1;
    ଅନ୍ୟଥା ଯଦି (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_ସଂଖ୍ୟା = 0;
    ଅନ୍ୟଥା
    {
	filter_handle_drag(wp, c, rettv);
	ଫେରନ୍ତୁ;
    }

    // Invoke callback
    res.v_type = VAR_ସଂଖ୍ୟା;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    ଶୂନ୍ୟ
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ସ୍ଥିର win_T *
find_popup_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    win_T *wp = win_id2wp(id);

    ଯଦି (wp != ଖାଲି && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ wp;
}

/*
 * popup_close({id})
 */
    ଶୂନ୍ୟ
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    ଯଦି (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == ଖାଲି &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    wp = find_popup_win(id);
    ଯଦି (wp != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_clear_mask_for(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row_start, col_start, row_end, col_end;

    ଯଦି (popup_mask == ଖାଲି || !popup_visible)
	ଫେରନ୍ତୁ;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)screen_Columns);

    ପାଇଁ (r = row_start; r < row_end; ++r)
	ପାଇଁ (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    ଶୂନ୍ୟ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    ଯଦି (error_if_term_popup_window())
	ଫେରନ୍ତୁ;
#endif
    ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ଫେରନ୍ତୁ;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    ଯଦି (wp->w_popup_imagep != ଖାଲି)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    ଯଦି (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    ଯଦି (was_visible)
	popup_clear_mask_for(wp);

    ଯଦି (old_area.active)
	popup_redraw_exposed_area(&old_area);
    ଅନ୍ୟଥା
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    ଯଦି (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    ଶୂନ୍ୟ
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    ଶୂନ୍ୟ
popup_show(win_T *wp)
{
    bool popup_active;

    ଯଦି ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	ଫେରନ୍ତୁ;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    ଯଦି (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
	ଯଦି (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    ଅନ୍ୟଥା
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    ଶୂନ୍ୟ
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;

    rettv->v_type = VAR_ସଂଖ୍ୟା;
    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    ଯଦି (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_ସଂଖ୍ୟା = 0;
}

/*
 * popup_settext({id}, {text})
 */
    ଶୂନ୍ୟ
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    popup_area_T	old_area;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    popup_save_area(wp, &old_area);

    ଯଦି (check_for_string_or_list_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
    ଯଦି (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    ଯଦି (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    ଶୂନ୍ୟ
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;

    ଯଦି (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf_from_arg(&argvars[1]);

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;
#ifdef FEAT_TERMINAL
    ଯଦି (buf->b_term != ଖାଲି && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (wp->w_buffer != buf)
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

	ଯଦି (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    ଯଦି (wp->w_popup_imagep != ଖାଲି)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    ଯଦି (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    ଯଦି (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    ଯଦି (wp == message_win)
	message_win = ଖାଲି;
#endif

    ଯଦି (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    ଯଦି (!old_area.active)
	popup_mask_refresh = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
error_for_popup_window(ଶୂନ୍ୟ)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
error_if_popup_window(ପୂର୍ଣ୍ଣସଂଖ୍ୟା also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    ଯଦି (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == ଖାଲି)
#endif
	    )
    {
	error_for_popup_window();
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ଖାଲି;

    // go through global popups
    ପାଇଁ (wp = first_popupwin; wp != ଖାଲି; prev = wp, wp = wp->w_next)
	ଯଦି (wp->w_id == id)
	{
	    ଯଦି (wp == curwin)
	    {
		ଯଦି (!force)
		{
		    error_for_popup_window();
		    ଫେରନ୍ତୁ FAIL;
		}
		back_to_prevwin(wp);
	    }
	    ଯଦି (prev == ଖାଲି)
		first_popupwin = wp->w_next;
	    ଅନ୍ୟଥା
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ଫେରନ୍ତୁ OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	ଯଦି (popup_close_tabpage(tp, id, force) == OK)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_close_tabpage(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    win_T	*wp;
    win_T	**ମୂଳ = &tp->tp_first_popupwin;
    win_T	*prev = ଖାଲି;

    ପାଇଁ (wp = *ମୂଳ; wp != ଖାଲି; prev = wp, wp = wp->w_next)
	ଯଦି (wp->w_id == id)
	{
	    ଯଦି (wp == curwin)
	    {
		ଯଦି (!force)
		{
		    error_for_popup_window();
		    ଫେରନ୍ତୁ FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    ଯଦି (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    ଯଦି (prev == ଖାଲି)
		*ମୂଳ = wp->w_next;
	    ଅନ୍ୟଥା
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    ଫେରନ୍ତୁ OK;
	}
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
close_all_popups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ଯଦି (!force && ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    ଯେତେବେଳେ (first_popupwin != ଖାଲି)
	ଯଦି (popup_close(first_popupwin->w_id, force) == FAIL)
	    ଫେରନ୍ତୁ;
    ଯେତେବେଳେ (curtab->tp_first_popupwin != ଖାଲି)
	ଯଦି (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    ଫେରନ୍ତୁ;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    ପାଇଁ (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ସ୍ଥିର bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	ଫେରନ୍ତୁ true;
    ପାଇଁ (i = 0; i < 4; i++)
	ଯଦି (style->border_highlight[i] != wp->w_border_highlight[i])
	    ଫେରନ୍ତୁ true;
    ଫେରନ୍ତୁ false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    ଫେରନ୍ତୁ layout->winrow != wp->w_winrow
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
    ସ୍ଥିର bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    ଫେରନ୍ତୁ area->winrow != wp->w_winrow
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
    ସ୍ଥିର ଶୂନ୍ୟ
mark_overlapped_opacity_popup(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା area_ଶୀର୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା area_bot,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା area_ବାମ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା area_ଡାହାଣ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା zindex)
{
    ଯଦି ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_ଶୀର୍ଷ
	    || wp->w_wincol >= area_ଡାହାଣ
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_ବାମ)
	ଫେରନ୍ତୁ;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
redraw_overlapped_opacity_popups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା winrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wincol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftoff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା zindex)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_ଶୀର୍ଷ = winrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_bot = winrow + height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_ବାମ = wincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		area_ଡାହାଣ = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_ଶୀର୍ଷ, area_bot, area_ବାମ,
		area_ଡାହାଣ, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_ଶୀର୍ଷ, area_bot, area_ବାମ,
		area_ଡାହାଣ, zindex);

    ଯଦି (must_redraw < UPD_VALID)
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
    ସ୍ଥିର ଶୂନ୍ୟ
refill_pum_bg_row_from_buffer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_cp = r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_cp = ବାମ;
    win_T	*wp;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row_for_lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_for_lnum = 0;
    char_u	*ଧାଡ଼ି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		win_text_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		soff_base = (r - pum_bg_ଶୀର୍ଷ) * pum_bg_cols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    ପାଇଁ (c = ବାମ; c < ଡାହାଣ; ++c)
    {
	pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ[soff_base + c] = ' ';
	ଯଦି (pum_bg_attrs != ଖାଲି)
	    pum_bg_attrs[soff_base + c] = 0;
	ଯଦି (enc_utf8 && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି)
	    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC[soff_base + c] = 0;
	ଯଦି (enc_utf8)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
	    ପାଇଁ (k = 0; k < MAX_MCO; ++k)
		ଯଦି (pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] != ଖାଲି)
		    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&ଧାଡ଼ି_cp, &col_cp, IGNORE_POPUP);
    ଯଦି (wp == ଖାଲି || ଧାଡ଼ି_cp < 0 || ଧାଡ଼ି_cp >= wp->w_height)
	ଫେରନ୍ତୁ;
    ଯଦି (wp->w_buffer == ଖାଲି || wp->w_buffer->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Compute the buffer line for this screen row.
    row_for_lnum = ଧାଡ଼ି_cp;
    ଯଦି (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, ଖାଲି))
	ଫେରନ୍ତୁ;	// past end of buffer
    ଯଦି (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;

    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ଯଦି (ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    ଯଦି (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = ଧାଡ଼ି;
    ଯେତେବେଳେ (*p != NUL && screen_col < ଡାହାଣ)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_cells;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା byte_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା soff = soff_base + screen_col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_range = (screen_col >= ବାମ && screen_col < ଡାହାଣ);

	ଯଦି (*p == '\t')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_ଗଣନା = 1;
	    ଯଦି (in_range)
	    {
		pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ[soff] = ' ';
		ଯଦି (enc_utf8 && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି)
		    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC[soff] = 0;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_ଗଣନା = mb_ptr2len(p);
	    ଯଦି (in_range)
	    {
		pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ[soff] = *p;
		ଯଦି (enc_utf8 && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି)
		    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	ଅନ୍ୟଥା
	{
	    char_cells = 1;
	    byte_ଗଣନା = 1;
	    ଯଦି (in_range)
	    {
		pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ[soff] = (*p < 0x20) ? ' ' : *p;
		ଯଦି (enc_utf8 && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି)
		    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC[soff] = 0;
	    }
	}
	ଯଦି (in_range && pum_bg_attrs != ଖାଲି)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_ଗଣନା;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_redraw_exposed_area(popup_area_T *area)
{
    ଯଦି (!area->active)
	ଫେରନ୍ତୁ;

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
    ଯଦି (pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_ଶୀର୍ଷ)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ = MAX(area->winrow, pum_bg_ଶୀର୍ଷ);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bot = MIN(area->winrow + area->height, pum_bg_bot);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ = MAX(area->wincol, 0);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ = MIN(area->wincol + area->width, pum_bg_cols);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	ପାଇଁ (r = ଶୀର୍ଷ; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, ବାମ, ଡାହାଣ);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ);
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
    ସ୍ଥିର ଶୂନ୍ୟ
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    ଯଦି (screen_opacity_popup == ଖାଲି
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	ଫେରନ୍ତୁ;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    ଯଦି (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା,
				  saved_screen->rows * saved_screen->cols);
    ଯଦି (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    ଯଦି (saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି || saved_screen->attrs == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < saved_screen->rows; r++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_row = saved_screen->start_row + r;

	ଯଦି (screen_row >= 0 && screen_row < screen_Rows)
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = 0; c < saved_screen->cols; c++)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_col = saved_screen->start_col + c;

		ଯଦି (screen_col >= 0 && screen_col < screen_Columns)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ଧାଡ଼ି_Offset[screen_row] + screen_col;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_off = r * saved_screen->cols + c;

		    saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[save_off] = Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    ଯଦି (enc_utf8 && saved_screen->linesuc != ଖାଲି)
			saved_screen->linesuc[save_off] = Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
redraw_under_popup_area(ପୂର୍ଣ୍ଣସଂଖ୍ୟା winrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wincol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftoff)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r;

    ପାଇଁ (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
	win_T	    *prev_twp = ଖାଲି;

	ଯଦି (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    ଜାରି;
	}

	ପାଇଁ (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଧାଡ଼ି_cp = r;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&ଧାଡ଼ି_cp, &col_cp, IGNORE_POPUP);
	    ଯଦି (twp != ଖାଲି && twp != prev_twp)
	    {
		prev_twp = twp;
		ଯଦି (ଧାଡ଼ି_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (ଶୂନ୍ୟ)mouse_comp_pos(twp, &ଧାଡ଼ି_cp, &col_cp, &lnum, ଖାଲି);
		    redrawWinline(twp, lnum);
		}
		ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    ଶୂନ୍ୟ
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    popup_area_T	old_area;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;  // invalid {id}

    ଯଦି (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    ଯଦି (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    ଯଦି (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    ଶୂନ୍ୟ
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_redraw = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_reposition = FALSE;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;  // invalid {id}

    ଯଦି (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (ଶୂନ୍ୟ)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    ଯଦି (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    ଯଦି (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    ଯଦି (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    ଯଦି (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
	ଯଦି (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    ଯଦି (old_style.blend != wp->w_popup_blend)
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

    ଯଦି (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    ଶୂନ୍ୟ
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଶୀର୍ଷ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବାମ_extra;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;  // invalid {id}
    ଶୀର୍ଷ_extra = popup_ଶୀର୍ଷ_extra(wp);
    ବାମ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_ସଂଖ୍ୟା(dict, "line", wp->w_winrow + 1);
    dict_add_ସଂଖ୍ୟା(dict, "col", wp->w_wincol + 1);
    dict_add_ସଂଖ୍ୟା(dict, "width", wp->w_width + ବାମ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_ସଂଖ୍ୟା(dict, "height", wp->w_height + ଶୀର୍ଷ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_ସଂଖ୍ୟା(dict, "core_line", wp->w_winrow + 1 + ଶୀର୍ଷ_extra);
    dict_add_ସଂଖ୍ୟା(dict, "core_col", wp->w_wincol + 1 + ବାମ_extra);
    dict_add_ସଂଖ୍ୟା(dict, "core_width", wp->w_width);
    dict_add_ସଂଖ୍ୟା(dict, "core_height", wp->w_height);

    dict_add_ସଂଖ୍ୟା(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_ସଂଖ୍ୟା(dict, "firstline", wp->w_topline);
    dict_add_ସଂଖ୍ୟା(dict, "lastline", wp->w_botline - 1);
    dict_add_ସଂଖ୍ୟା(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    ଶୂନ୍ୟ
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    FOR_ALL_POPUPWINS(wp)
	list_append_ସଂଖ୍ୟା(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    ଶୂନ୍ୟ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    win_T	*wp;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    row = tv_get_ସଂଖ୍ୟା(&argvars[0]) - 1;
    col = tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ଯଦି (wp != ଖାଲି && WIN_IS_POPUP(wp))
	rettv->vval.v_ସଂଖ୍ୟା = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_padding_border(dict_T *dict, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *array, ଅକ୍ଷର *name)
{
    list_T  *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	ଫେରନ୍ତୁ;

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	ପାଇଁ (i = 0; i < 4; ++i)
	    list_append_ସଂଖ୍ୟା(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < 4; ++i)
	ଯଦି (wp->w_border_highlight[i] != ଖାଲି)
	    ଭାଙ୍ଗ;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    ଯଦି (i == 4 && !wp->w_border_highlight_isset)
	ଫେରନ୍ତୁ;

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	ଫେରନ୍ତୁ;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    ଯଦି (i == 4)
	ଫେରନ୍ତୁ;
    ପାଇଁ (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    char_u  buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

    ପାଇଁ (i = 0; i < 8; ++i)
	ଯଦି (wp->w_border_char[i] != 0)
	    ଭାଙ୍ଗ;
    ଯଦି (i == 8)
	ଫେରନ୍ତୁ;

    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	ଫେରନ୍ତୁ;
    }
    ପାଇଁ (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    ଯଦି (list != ଖାଲି)
    {
	ଯଦି (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	ଅନ୍ୟଥା
	{
	    list_append_ସଂଖ୍ୟା(list, wp->w_popup_lnum);
	    list_append_ସଂଖ୍ୟା(list, wp->w_popup_mincol);
	    list_append_ସଂଖ୍ୟା(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	ଫେରନ୍ତୁ;
    }
    list_append_ସଂଖ୍ୟା(list, wp->w_popup_mouse_row);
    list_append_ସଂଖ୍ୟା(list, wp->w_popup_mouse_mincol);
    list_append_ସଂଖ୍ୟା(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    ଶୂନ୍ୟ
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    win_T	*wp;
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = find_popup_win(id);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    dict = rettv->vval.v_dict;
    dict_add_ସଂଖ୍ୟା(dict, "line", wp->w_wantline);
    dict_add_ସଂଖ୍ୟା(dict, "col", wp->w_wantcol);
    dict_add_ସଂଖ୍ୟା(dict, "minwidth", wp->w_minwidth);
    dict_add_ସଂଖ୍ୟା(dict, "minheight", wp->w_minheight);
    dict_add_ସଂଖ୍ୟା(dict, "maxheight", wp->w_maxheight);
    dict_add_ସଂଖ୍ୟା(dict, "maxwidth", wp->w_maxwidth);
    dict_add_ସଂଖ୍ୟା(dict, "firstline", wp->w_firstline);
    dict_add_ସଂଖ୍ୟା(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_ସଂଖ୍ୟା(dict, "zindex", wp->w_zindex);
    dict_add_ସଂଖ୍ୟା(dict, "fixed", wp->w_popup_fixed);
    ଯଦି (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	ଯଦି (pt != ଖାଲି)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_ସଂଖ୍ୟା(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_ସଂଖ୍ୟା(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    ଯଦି (wp->w_popup_imagep != ଖାଲି && wp->w_popup_imagep->img != ଖାଲି)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	ଯଦି (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_ସଂଖ୍ୟା(dict, "wrap", wp->w_p_wrap);
    dict_add_ସଂଖ୍ୟା(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_ସଂଖ୍ୟା(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_ସଂଖ୍ୟା(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_ସଂଖ୍ୟା(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_ସଂଖ୍ୟା(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_ସଂଖ୍ୟା(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_ସଂଖ୍ୟା(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_ସଂଖ୍ୟା(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    ଯଦି (wp->w_scrollbar_highlight != ଖାଲି)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    ଯଦି (wp->w_thumb_highlight != ଖାଲି)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    ଯଦି (twp->w_id == id)
		ଭାଙ୍ଗ;
	ଯଦି (twp != ଖାଲି)
	    ଭାଙ୍ଗ;
	++i;
    }
    ଯଦି (tp == ଖାଲି)
	i = -1;  // must be global
    ଅନ୍ୟଥା ଯଦି (tp == curtab)
	i = 0;
    dict_add_ସଂଖ୍ୟା(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    ଯଦି (wp->w_filter_cb.cb_name != ଖାଲି)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    ଯଦି (wp->w_close_cb.cb_name != ଖାଲି)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(poppos_entries); ++i)
	ଯଦି (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)poppos_entries[i].pp_name.length);
	    ଭାଙ୍ଗ;
	}

    ଯଦି (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    ଅନ୍ୟଥା ଯଦି (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    ଅନ୍ୟଥା
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#ଯଦି defined(FEAT_TIMERS)
    dict_add_ସଂଖ୍ୟା(dict, "time", wp->w_popup_timer != ଖାଲି
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#ଯଦି defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
error_if_term_popup_window(ଶୂନ୍ୟ)
{
    ଯଦି (WIN_IS_POPUP(curwin) && curbuf->b_term != ଖାଲି
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    ଶୂନ୍ୟ
popup_reset_handled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା handled_flag)
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
find_next_popup(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lowest, ପୂର୍ଣ୍ଣସଂଖ୍ୟା handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = ଖାଲି;
    FOR_ALL_POPUPWINS(wp)
	ଯଦି ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    ଯଦି (found_wp != ଖାଲି)
	found_wp->w_popup_handled |= handled_flag;
    ଫେରନ୍ତୁ found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
invoke_popup_filter(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    ଯଦି (c == Ctrl_C)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_got_int = got_int;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	ଯଦି (was_curwin && wp == curwin)
	    ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ TRUE;
    }

    argv[0].v_type = VAR_ସଂଖ୍ୟା;
    argv[0].vval.v_ସଂଖ୍ୟା = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    ଯଦି (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	ଯଦି (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    ଯଦି (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    ଫେରନ୍ତୁ res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_do_filter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FALSE;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ଚାବି_Typed = ଚାବି_Typed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    ଯଦି (popup_is_popup(curwin) && curbuf->b_term != ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    ଯଦି (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ FALSE;
    recursive = TRUE;

    ଯଦି (c == K_LEFTMOUSE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = mouse_row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	ଯଦି (wp != ଖାଲି && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    ଯେତେବେଳେ (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != ଖାଲି)
	ଯଦି (wp->w_filter_cb.cb_name != ଖାଲି
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    ଯଦି (must_redraw > was_must_redraw)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    ଚାବି_Typed = save_ଚାବି_Typed;

    // When interrupted return FALSE to avoid looping.
    ଫେରନ୍ତୁ res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_no_mapping(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    round;
    win_T   *wp;

    ପାଇଁ (round = 1; round <= 2; ++round)
	ପାଇଁ (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != ଖାଲି; wp = wp->w_next)
	    ଯଦି (wp->w_filter_cb.cb_name != ଖାଲି
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    ଶୂନ୍ୟ
popup_check_cursor_pos(ଶୂନ୍ୟ)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    ଯେତେବେଳେ ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != ଖାଲି)
	ଯଦି (wp->w_popup_curwin != ଖାଲି
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_update_mask(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row, col;

    ଯଦି (wp->w_popup_mask == ଖାଲି || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (wp->w_popup_mask_cells != ଖାଲି
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	ଫେରନ୍ତୁ;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    ଯଦି (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    ଯଦି (wp->w_popup_mask_cells == ଖାଲି)
	ଫେରନ୍ତୁ;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(wp->w_popup_mask, lio)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, cole;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ିଗୁଡ଼ିକ, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (cols < 0)
	    cols = width + cols + 1;
	ଯଦି (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (cole < 0)
	    cole = width + cole + 1;
	ଯଦି (cole > width)
	    cole = width;
	li = li->li_next;
	ଧାଡ଼ିଗୁଡ଼ିକ = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	    ଧାଡ଼ିଗୁଡ଼ିକ = height + ଧାଡ଼ିଗୁଡ଼ିକ + 1;
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ <= 0)
	    ଧାଡ଼ିଗୁଡ଼ିକ = 1;
	li = li->li_next;
	linee = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (linee < 0)
	    linee = height + linee + 1;
	ଯଦି (linee > height)
	    linee = height;

	ପାଇଁ (row = ଧାଡ଼ିଗୁଡ଼ିକ - 1; row < linee; ++row)
	    ପାଇଁ (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_masked(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା screencol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା screenline)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି = screenline - wp->w_winrow;

    ଫେରନ୍ତୁ col >= 0 && col < width
	    && ଧାଡ଼ି >= 0 && ଧାଡ଼ି < height
	    && wp->w_popup_mask_cells[ଧାଡ଼ି * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_popup_transparent(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    ଯଦି (wp->w_popup_mask == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width = popup_width(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height = popup_height(wp);
    listitem_T	*lio, *li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cols, cole;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ିଗୁଡ଼ିକ, linee;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col, ଧାଡ଼ି;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	ଧାଡ଼ିଗୁଡ଼ିକ = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	    ଧାଡ଼ିଗୁଡ଼ିକ = height + ଧାଡ଼ିଗୁଡ଼ିକ + 1;
	li = li->li_next;
	linee = tv_get_ସଂଖ୍ୟା(&li->li_tv);
	ଯଦି (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	ଯଦି (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--ଧାଡ଼ିଗୁଡ଼ିକ;
	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	    ଧାଡ଼ିଗୁଡ଼ିକ = 0;
	ପାଇଁ (ଧାଡ଼ି = ଧାଡ଼ିଗୁଡ଼ିକ; ଧାଡ଼ି < linee
		&& ଧାଡ଼ି + wp->w_winrow < screen_Rows; ++ଧାଡ଼ି)
	    ପାଇଁ (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(ଧାଡ଼ି + wp->w_winrow) * screen_Columns
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_popup_unhidden(win_T *wp)
{
    ଯଦି (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	ଯଦି ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    ଫେରନ୍ତୁ FALSE;
	ଯଦି (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	ଅନ୍ୟଥା ଯଦି (popup_find_prop_above_ଶୀର୍ଷ(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	ଯଦି (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_need_position_adjust(win_T *wp)
{
    ଯଦି (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	ଫେରନ୍ତୁ TRUE;

    // May need to adjust the width if the cursor moved.
    ଫେରନ୍ତୁ wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ସ୍ଥିର short *opacity_zindex = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା    opacity_zindex_rows = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
popup_mark_opacity_zindex(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r, c;

    ଯଦି (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ଫେରନ୍ତୁ;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    ପାଇଁ (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	ପାଇଁ (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = r * screen_Columns + c;
	    ଯଦି (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
redraw_win_under_opacity_popup(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r;

    ଯଦି (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ଫେରନ୍ତୁ;

    height = popup_height(wp);
    width = popup_width(wp);
    ପାଇଁ (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col;
	win_T	    *prev_twp = ଖାଲି;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	ପାଇଁ (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଧାଡ଼ି_cp = r;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&ଧାଡ଼ି_cp, &col_cp, IGNORE_POPUP);
	    ଯଦି (twp != ଖାଲି && twp != prev_twp)
	    {
		prev_twp = twp;
		ଯଦି (ଧାଡ଼ି_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    ଯଦି (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    ଅନ୍ୟଥା
#endif
		    {
			(ଶୂନ୍ୟ)mouse_comp_pos(twp, &ଧାଡ଼ି_cp, &col_cp, &lnum,
									ଖାଲି);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_cp == twp->w_height)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_under_opacity(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (opacity_zindex == ଖାଲି
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_over_opacity(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ଯଦି ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ଫେରନ୍ତୁ TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_under_opacity_range(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

    ଯଦି (opacity_zindex == ଖାଲି
	    || row < 0 || row >= opacity_zindex_rows)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	ଯଦି (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    ଶୂନ୍ୟ
may_update_popup_mask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    win_T	*wp;
    short	*mask;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି, col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		redraw_all_popups = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    ଯଦି (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	ଅନ୍ୟଥା ଯଦି (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	ଅନ୍ୟଥା ଯଦି (popup_need_position_adjust(wp))
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ଯଦି ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ଯଦି ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	ଯଦି (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    ଯଦି (opacity_zindex != ଖାଲି)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * ଆକାର(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    ଯଦି (!popup_mask_refresh)
	ଫେରନ୍ତୁ;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    ଯଦି (redrawing_all_win)
	mask = popup_mask;
    ଅନ୍ୟଥା
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * ଆକାର(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    ଯେତେବେଳେ ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	ଯଦି (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    ଯଦି (wp->w_popup_flags & POPF_HIDDEN)
		ଜାରି;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	ଯଦି (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	ଅନ୍ୟଥା
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	ଯଦି ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ଜାରି;

	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask_start = wp->w_winrow;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask_end = mask_start + height;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    ପାଇଁ (ଧାଡ଼ି = mask_start;
			    ଧାଡ଼ି < mask_end && ଧାଡ଼ି < screen_Rows; ++ଧାଡ଼ି)
		ପାଇଁ (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    ଯଦି (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(ଧାଡ଼ି, col, FALSE))
			mask[ଧାଡ଼ି * screen_Columns + col] = POPUPMENU_ZINDEX;
		    ଅନ୍ୟଥା ଯଦି (wp->w_popup_mask_cells == ଖାଲି
				    || !popup_masked(wp, width, height, col, ଧାଡ଼ି))
			mask[ଧାଡ଼ି * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    ଯଦି (mask == popup_mask_next)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *plines_cache = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, Rows);
	win_T	    *prev_wp = ଖାଲି;

	ପାଇଁ (ଧାଡ଼ି = 0; ଧାଡ଼ି < screen_Rows; ++ଧାଡ଼ି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col_done = 0;

	    ପାଇଁ (col = 0; col < screen_Columns; ++col)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ଧାଡ଼ି * screen_Columns + col;

		ଯଦି (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    ଯଦି (ଧାଡ଼ି >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			ଯଦି (!ସନ୍ଦେଶ_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    ଅନ୍ୟଥା ଯଦି (col >= col_done)
		    {
			linenr_T	lnum;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_cp = ଧାଡ଼ି;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&ଧାଡ଼ି_cp, &col_cp, IGNORE_POPUP);
			ଯଦି (wp != ଖାଲି)
			{
#ଯଦି defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    ଯଦି (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    ଅନ୍ୟଥା
#endif
			    {
				ଯଦି (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * Rows);
				    prev_wp = wp;
				}

				ଯଦି (ଧାଡ଼ି_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				ଅନ୍ୟଥା
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (ଶୂନ୍ୟ)mouse_comp_pos(wp, &ଧାଡ଼ି_cp, &col_cp,
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
    ଶୂନ୍ୟ
may_update_popup_position(ଶୂନ୍ୟ)
{
    ଯଦି (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
ସ୍ଥିର schar_T *base_screenlines = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *base_screenattrs = ଖାଲି;
ସ୍ଥିର u8char_T *base_screenlinesuc = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_screen_rows = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_get_base_screen_cell(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, schar_T *linep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *attrp,
							  u8char_T *ucp)
{
    ଯଦି (base_screenlines == ଖାଲି || base_screenattrs == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = row * base_screen_cols + col;
    ଯଦି (linep != ଖାଲି)
	*linep = base_screenlines[off];
    ଯଦି (attrp != ଖାଲି)
	*attrp = base_screenattrs[off];
    ଯଦି (ucp != ଖାଲି)
    {
	ଯଦି (enc_utf8 && base_screenlinesuc != ଖାଲି)
	    *ucp = base_screenlinesuc[off];
	ଅନ୍ୟଥା
	    *ucp = 0;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    ଶୂନ୍ୟ
popup_set_base_screen_cell(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, schar_T ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, u8char_T uc)
{
    ଯଦି (base_screenlines == ଖାଲି || base_screenattrs == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = row * base_screen_cols + col;
    base_screenlines[off] = ଧାଡ଼ି;
    base_screenattrs[off] = attr;
    ଯଦି (enc_utf8 && base_screenlinesuc != ଖାଲି)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ସ୍ଥିର ଶୂନ୍ୟ
draw_opacity_padding_cell(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
	popup_saved_screen_T *saved_screen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pad_start_col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pad_end_col)
{
    ଯଦି (ଧାଡ଼ି_Offset == ଖାଲି || Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି || ScreenAttrs == ଖାଲି
	    || (enc_utf8 && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC == ଖାଲି))
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ଧାଡ଼ି_Offset[row] + col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = row - saved_screen->start_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = col - saved_screen->start_col;

    ଯଦି (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	ଯଦି (enc_utf8 && saved_screen->linesuc != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_col = col - 1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_off = off - 1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_save_off = save_off - 1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    ଯଦି (base_off >= 0)
	    {
		ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] != 0
			&& utf_char2cells(Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off]) == 2
			&& Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] == 0)
		    wide_prev = TRUE;
	    }
	    ଯଦି (!wide_prev && save_off > 0)
	    {
		ଯଦି (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    ଯଦି (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		ଯଦି (base_col < pad_start_col)
		{
		    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] != 0
			    && utf_char2cells(Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off] = ' ';
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off],
				ScreenAttrs[base_off],
				Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			ଯଦି (enc_utf8)
			    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = 0;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr_val =
					get_win_attr(screen_opacity_popup);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off], ScreenAttrs[off],
				Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]);
			screen_char(off, row, col);
			ଫେରନ୍ତୁ;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    ଯଦି (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off] == ' '
			    && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] == 0)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off] =
				    saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[base_save_off];
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = 0;
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off],
				    ScreenAttrs[base_off],
				    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off],
				    ScreenAttrs[off],
				    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]);
			screen_char(base_off, row, base_col);
			ଫେରନ୍ତୁ;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    ଯଦି (enc_utf8)
			Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr_val = get_win_attr(screen_opacity_popup);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off],
					       ScreenAttrs[off], Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]);
		    screen_char(off, row, col);
		    ଫେରନ୍ତୁ;
		}

		// Base cell is inside the saved area, redraw the wide char.
		ଯଦି (save_off > 0)
		{
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off] = saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = saved_screen->linesuc[save_off];

		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr_val = get_win_attr(screen_opacity_popup);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_ଧାଡ଼ିଗୁଡ଼ିକ[base_off],
					       ScreenAttrs[base_off], Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off],
					       ScreenAttrs[off], Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		ଫେରନ୍ତୁ;
	    }
	}
	Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = saved_screen->ଧାଡ଼ିଗୁଡ଼ିକ[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	ଯଦି (enc_utf8)
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = saved_screen->linesuc != ଖାଲି
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	ଯଦି (enc_utf8 && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] != 0
		&& utf_char2cells(Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off] = ' ';
	    Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] = 0;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr_val = get_win_attr(screen_opacity_popup);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off],
				   ScreenAttrs[off], Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ସ୍ଥିର ଶୂନ୍ୟ
fill_opacity_padding(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_col,
	popup_saved_screen_T *saved_screen)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା pad_row = start_row; pad_row < end_row; pad_row++)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    ଶୂନ୍ୟ
update_popups(ଶୂନ୍ୟ (*win_update)(win_T *wp))
{
    win_T   *wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଶୀର୍ଷ_off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ବାମ_extra;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    total_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    total_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଶୀର୍ଷ_padding;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    popup_attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    border_attr[4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    wincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    padcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    padendcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sb_thumb_ଶୀର୍ଷ = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sb_thumb_height = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr_scroll = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    ଯଦି (base_screenlines != ଖାଲି)
    {
	vim_free(base_screenlines);
	base_screenlines = ଖାଲି;
    }
    ଯଦି (base_screenattrs != ଖାଲି)
    {
	vim_free(base_screenattrs);
	base_screenattrs = ଖାଲି;
    }
    ଯଦି (base_screenlinesuc != ଖାଲି)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = ଖାଲି;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    ଯେତେବେଳେ ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    title_len = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ଠେଲ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	ଯଦି ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	ଅନ୍ୟଥା
	    screen_opacity_popup = ଖାଲି;

#ifdef FEAT_PROP_POPUP
	ଯଦି (screen_opacity_popup != ଖାଲି)
	{
	    ଯଦି (base_screenlines != ଖାଲି)
	    {
		vim_free(base_screenlines);
		base_screenlines = ଖାଲି;
	    }
	    ଯଦି (base_screenattrs != ଖାଲି)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = ଖାଲି;
	    }
	    ଯଦି (base_screenlinesuc != ଖାଲି)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = ଖାଲି;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା,
				    base_screen_rows * base_screen_cols);
	    ଯଦି (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    ଯଦି (base_screenlines != ଖାଲି && base_screenattrs != ଖାଲି)
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < base_screen_rows; r++)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ଧାଡ଼ି_Offset[r];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_off = r * base_screen_cols;
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			ଯଦି (enc_utf8 && base_screenlinesuc != ଖାଲି)
			    base_screenlinesuc[base_off + c] =
					Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off + c];
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
	ଶୀର୍ଷ_off = popup_ଶୀର୍ଷ_extra(wp);
	ବାମ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	ଯଦି (wp->w_wincol + ବାମ_extra < 0)
	    ବାମ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(ଶୀର୍ଷ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ବାମ_extra;

	// Draw the popup text, unless it's off screen.
	ଯଦି (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    ଯଦି (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    ଯଦି (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    ଅନ୍ୟଥା ଯଦି (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(ଶୀର୍ଷ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ବାମ_extra;

	// Add offset for border and padding if not done already.
	ଯଦି ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ବାମ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	ଯଦି ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += ଶୀର୍ଷ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	ଯଦି (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	ଅନ୍ୟଥା
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	ଯଦି (total_width < 0)
	    total_width = 0;
	ଯଦି (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	ଅନ୍ୟଥା
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	ଯଦି (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	ଅନ୍ୟଥା
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	ଯଦି (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	ଅନ୍ୟଥା
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    ପାଇଁ (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    ଯଦି (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	ପାଇଁ (i = 0; i < 8; ++i)
	    ଯଦି (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	ପାଇଁ (i = 0; i < 4; ++i)
	{
	    ଯଦି (wp->w_border_highlight[i] != ଖାଲି)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    ଅନ୍ୟଥା ଯଦି (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    ଅନ୍ୟଥା
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    ଯଦି ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	ଯଦି ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	ଯଦି (wp->w_popup_title != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    title_attr;

	    ଯଦି (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ଖାଲି)
		title_attr = border_attr[0];
	    ଅନ୍ୟଥା ଯଦି (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    ଅନ୍ୟଥା
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    ଯଦି (title_len > total_width - 2)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	title_byte_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		ଯଦି (title_text != ଖାଲି)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    ଅନ୍ୟଥା
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	ଶୀର୍ଷ_padding = cl.eff_padding[0];
	ଯଦି (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    ଯଦି (title_len > 0)
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
	    ଅନ୍ୟଥା
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    ଯଦି (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (cl.eff_padding[0] == 0 && popup_ଶୀର୍ଷ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    ଶୀର୍ଷ_padding = 1;

	ଯଦି (ଶୀର୍ଷ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    ଯଦି (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	ଯଦି (ଶୀର୍ଷ_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    ଯଦି (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		ଯଦି (screen_opacity_popup != ଖାଲି && saved_screen.ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		ଅନ୍ୟଥା
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		ଶୀର୍ଷ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    ଯଦି (screen_opacity_popup != ଖାଲି && saved_screen.ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
	    {
		fill_opacity_padding(row, row + ଶୀର୍ଷ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    ଅନ୍ୟଥା
	    {
		screen_fill(row, row + ଶୀର୍ଷ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	ଯଦି (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height = wp->w_height;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    ଯଦି (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    ଯଦି (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    ଯଦି (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_ଶୀର୍ଷ = 0;
	    ଅନ୍ୟଥା
		sb_thumb_ଶୀର୍ଷ = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    ଯଦି (wp->w_topline > 1 && sb_thumb_ଶୀର୍ଷ == 0 && height > 1)
		sb_thumb_ଶୀର୍ଷ = 1;  // show it's scrolled
	    last = total_height - ଶୀର୍ଷ_off - wp->w_popup_border[2];
	    ଯଦି (sb_thumb_ଶୀର୍ଷ >= last)
		// show at least one character
		sb_thumb_ଶୀର୍ଷ = last - 1;

	    ଯଦି (wp->w_scrollbar_highlight != ଖାଲି)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    ଅନ୍ୟଥା
		attr_scroll = highlight_attr[HLF_PSB];
	    ଯଦି (wp->w_thumb_highlight != ଖାଲି)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    ଅନ୍ୟଥା
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	ପାଇଁ (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pad_ବାମ;
	    // left and right padding only needed next to the body
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    ଯଦି (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    ଯଦି (do_padding && cl.eff_padding[3] > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = wincol + wp->w_popup_border[3];

		// left padding
		pad_ବାମ = wp->w_popup_padding[3];
		ଯଦି (col < 0)
		{
		    pad_ବାମ += col;
		    col = 0;
		}
		ଯଦି (pad_ବାମ > 0)
		{
		    ଯଦି (screen_opacity_popup != ଖାଲି && saved_screen.ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
			fill_opacity_padding(row, row + 1, col, col + pad_ବାମ,
				&saved_screen);
		    ଅନ୍ୟଥା
			screen_fill(row, row + 1, col, col + pad_ବାମ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    ଯଦି (wp->w_has_scrollbar)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି = i - ଶୀର୍ଷ_off;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		ଯଦି (ଧାଡ଼ି >= 0 && ଧାଡ଼ି < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    ଧାଡ଼ି >= sb_thumb_ଶୀର୍ଷ
				       && ଧାଡ଼ି < sb_thumb_ଶୀର୍ଷ + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		ଅନ୍ୟଥା
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    ଯଦି (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    ଯଦି (do_padding && cl.eff_padding[1] > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pad_col_end = pad_col_start + wp->w_popup_padding[1];

		ଯଦି (screen_opacity_popup != ଖାଲି && saved_screen.ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		ଅନ୍ୟଥା
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	ଯଦି (wp->w_popup_shadow)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = wincol + total_width;
	    ପାଇଁ (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	ଯଦି (cl.eff_padding[2] > 0)
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
	    ଯଦି (screen_opacity_popup != ଖାଲି && saved_screen.ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    ଅନ୍ୟଥା
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	ଯଦି (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    ଯଦି (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	ଯଦି (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	ଯଦି (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ଖାଲି;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	ଯଦି (override_success)
	    କାଢ଼_highlight_overrides();

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

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
    // In case win_update() called start_search_hl().
    end_ସନ୍ଧାନ_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_one_popup(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    typval_T	tv;

    ଯଦି (wp->w_close_cb.cb_partial != ଖାଲି)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
    }
    ଯଦି (wp->w_filter_cb.cb_partial != ଖାଲି)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    ଫେରନ୍ତୁ abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_popups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    ପାଇଁ (wp = first_popupwin; !abort && wp != ଖାଲି; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	ପାଇଁ (wp = tp->tp_first_popupwin; !abort && wp != ଖାଲି; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	ଯଦି (abort)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ abort;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_is_popup(win_T *wp)
{
    ଫେରନ୍ତୁ wp->w_popup_flags != 0;
}

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(ଶୂନ୍ୟ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp->w_p_pvw)
	    ଫେରନ୍ତୁ wp;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(ଶୂନ୍ୟ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ଯଦି (wp->w_popup_flags & POPF_INFO)
	    ଫେରନ୍ତୁ wp;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		by;
    linenr_T	new_topline;

    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    ଯଦି (new_topline < 1)
	new_topline = 1;
    ଯଦି (new_topline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	new_topline = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯଦି (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ଫେରନ୍ତୁ true;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ false;
#endif
}

    ଶୂନ୍ୟ
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_ସଂଖ୍ୟା = message_win == ଖାଲି ? 0 : message_win->w_id;
#ଅନ୍ୟଥା
    rettv->vval.v_ସଂଖ୍ୟା = 0;
#endif
}

    ଶୂନ୍ୟ
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_ସଂଖ୍ୟା = wp == ଖାଲି ? 0 : wp->w_id;
#ଅନ୍ୟଥା
    rettv->vval.v_ସଂଖ୍ୟା = 0;
#endif
}

    ଶୂନ୍ୟ
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_ସଂଖ୍ୟା = wp == ଖାଲି ? 0 : wp->w_id;
#ଅନ୍ୟଥା
    rettv->vval.v_ସଂଖ୍ୟା = 0;
#endif
}

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_create_preview_window(ପୂର୍ଣ୍ଣସଂଖ୍ୟା info)
{
    win_T *wp = popup_create(ଖାଲି, ଖାଲି, info ? TYPE_INFO : TYPE_PREVIEW);

    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (info)
	wp->w_popup_flags |= POPF_INFO;
    ଅନ୍ୟଥା
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    ଯଦି (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    ଅନ୍ୟଥା ଯଦି (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    ଅନ୍ୟଥା
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    ଫେରନ୍ତୁ OK;
}

/*
 * Close any preview popup.
 */
    ଶୂନ୍ୟ
popup_close_preview(ଶୂନ୍ୟ)
{
    win_T *wp = popup_find_preview_window();

    ଯଦି (wp != ଖାଲି)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    ଶୂନ୍ୟ
popup_hide_info(ଶୂନ୍ୟ)
{
    win_T *wp = popup_find_info_window();

    ଯଦି (wp != ଖାଲି)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	ଯଦି (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    ଶୂନ୍ୟ
popup_close_info(ଶୂନ୍ୟ)
{
    win_T *wp = popup_find_info_window();

    ଯଦି (wp != ଖାଲି)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_overlaps_cmdline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ popup_on_cmdline;
}

#ଯଦି defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(ଶୂନ୍ୟ)
{
    ଯଦି (message_win != ଖାଲି)
	ଫେରନ୍ତୁ message_win;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    message_win = popup_create(ଖାଲି, ଖାଲି, TYPE_MESSAGE_WIN);

    ଯଦି (message_win == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    ପାଇଁ (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    ପାଇଁ (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    ଯଦି (message_win->w_popup_timer != ଖାଲି)
	message_win->w_popup_timer->tr_keep = TRUE;
    ଫେରନ୍ତୁ message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    ଶୂନ୍ୟ
popup_show_message_win(ଶୂନ୍ୟ)
{
    ଯଦି (message_win == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
may_start_message_win_timer(win_T *wp)
{
    ଯଦି (wp == message_win && start_message_win_timer)
    {
	ଯଦି (message_win->w_popup_timer != ଖାଲି)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_message_win_visible(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ message_win != ଖାଲି
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    ଶୂନ୍ୟ
popup_hide_message_win(ଶୂନ୍ୟ)
{
    ଯଦି (message_win != ଖାଲି)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ସନ୍ଦେଶ_didout = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ସନ୍ଦେଶ_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ew_ସନ୍ଦେଶ_didout = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ew_ସନ୍ଦେଶ_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    ଶୂନ୍ୟ
start_echowindow(ପୂର୍ଣ୍ଣସଂଖ୍ୟା time_sec)
{
    in_echowindow = TRUE;
    save_ସନ୍ଦେଶ_didout = ସନ୍ଦେଶ_didout;
    save_ସନ୍ଦେଶ_col = ସନ୍ଦେଶ_col;
    ସନ୍ଦେଶ_didout = ew_ସନ୍ଦେଶ_didout;
    ସନ୍ଦେଶ_col = ew_ସନ୍ଦେଶ_col;
    ଯଦି (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    ଶୂନ୍ୟ
end_echowindow(ଶୂନ୍ୟ)
{
    in_echowindow = FALSE;

    ଯଦି ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_ସନ୍ଦେଶ_didout = TRUE;
    ew_ସନ୍ଦେଶ_col = ସନ୍ଦେଶ_col == 0 ? 1 : ସନ୍ଦେଶ_col;
    ସନ୍ଦେଶ_didout = save_ସନ୍ଦେଶ_didout;
    ସନ୍ଦେଶ_col = save_ସନ୍ଦେଶ_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_win_closed(win_T *win)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    round;
    win_T   *wp;
    win_T   *next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret = FALSE;

    ପାଇଁ (round = 1; round <= 2; ++round)
	ପାଇଁ (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != ଖାଲି; wp = next)
	{
	    next = wp->w_next;
	    ଯଦି (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    ଫେରନ୍ତୁ ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    ଶୂନ୍ୟ
popup_set_title(win_T *wp)
{
    ଯଦି (wp->w_buffer->b_fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    ଯଦି (wp->w_popup_title != ଖାଲି)
	vim_snprintf((ଅକ୍ଷର *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    ଶୂନ୍ୟ
popup_update_preview_title(ଶୂନ୍ୟ)
{
    win_T *wp = popup_find_preview_window();

    ଯଦି (wp != ଖାଲି)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
