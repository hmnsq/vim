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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_PROP_POPUP)

પ્રકારવ્યાખ્યા સંરચના {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		active;
    પૂર્ણાંક		winrow;
    પૂર્ણાંક		wincol;
    પૂર્ણાંક		height;
    પૂર્ણાંક		width;
    પૂર્ણાંક		leftoff;
    પૂર્ણાંક		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
પ્રકારવ્યાખ્યા સંરચના {
    schar_T	*લીટીઓ;
    પૂર્ણાંક		*attrs;
    u8char_T	*linesuc;
    પૂર્ણાંક		start_row;
    પૂર્ણાંક		start_col;
    પૂર્ણાંક		rows;
    પૂર્ણાંક		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
પ્રકારવ્યાખ્યા સંરચના {
    linenr_T	firstline;
    પૂર્ણાંક		blend;
    પૂર્ણાંક		flags;
    પૂર્ણાંક		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		winrow;
    પૂર્ણાંક		wincol;
    પૂર્ણાંક		width;
    પૂર્ણાંક		height;
    પૂર્ણાંક		leftcol;
    પૂર્ણાંક		leftoff;
    પૂર્ણાંક		has_scrollbar;
    પૂર્ણાંક		topoff;
    પૂર્ણાંક		bottomoff;
    પૂર્ણાંક		leftclip;
    પૂર્ણાંક		rightclip;
} popup_layout_T;

સ્થિર poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
સ્થિર win_T *message_win = ખાલી;

// Time used for the next ":echowindow" message in msec.
સ્થિર પૂર્ણાંક  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
સ્થિર પૂર્ણાંક    start_message_win_timer = FALSE;

સ્થિર શૂન્ય may_start_message_win_timer(win_T *wp);
#endif

સ્થિર પૂર્ણાંક popup_on_cmdline = FALSE;

સ્થિર શૂન્ય popup_adjust_position(win_T *wp);
સ્થિર bool popup_area_changed(win_T *wp, popup_area_T *area);
સ્થિર શૂન્ય popup_redraw_exposed_area(popup_area_T *area);
સ્થિર શૂન્ય popup_save_area(win_T *wp, popup_area_T *area);
સ્થિર શૂન્ય popup_free_saved_screen(popup_saved_screen_T *saved_screen);
સ્થિર શૂન્ય popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
સ્થિર bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
સ્થિર bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
સ્થિર શૂન્ય popup_save_style(win_T *wp, popup_style_snapshot_T *style);
સ્થિર શૂન્ય popup_save_layout(win_T *wp, popup_layout_T *layout);
સ્થિર શૂન્ય redraw_under_popup_area(પૂર્ણાંક winrow, પૂર્ણાંક wincol, પૂર્ણાંક height,
	પૂર્ણાંક width, પૂર્ણાંક leftoff);
સ્થિર શૂન્ય redraw_overlapped_opacity_popups(પૂર્ણાંક winrow, પૂર્ણાંક wincol,
	પૂર્ણાંક height, પૂર્ણાંક width, પૂર્ણાંક leftoff, પૂર્ણાંક zindex);
સ્થિર શૂન્ય redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    સ્થિર પૂર્ણાંક
popup_options_one(dict_T *dict, char_u *ચાવી)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    પૂર્ણાંક		n = 0;

    di = dict_find(dict, ચાવી, -1);
    જો (di == ખાલી)
	પાછા MAXCOL;

    val = tv_get_string(&di->di_tv);
    જો (STRNCMP(val, "cursor", 6) != 0)
	પાછા dict_get_સંખ્યા_check(dict, ચાવી);

    setcursor_mayforce(TRUE);
    s = val + 6;
    જો (*s != NUL)
    {
	endp = s;
	જો (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((અક્ષર *)s, (અક્ષર **)&endp, 10);
	જો (endp != ખાલી && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    પાછા 0;
	}
    }

    જો (STRCMP(ચાવી, "line") == 0)
	n = screen_screenrow() + 1 + n;
    નહીંતર // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    જો (n == 0)
	n = -1;
    પાછા n;
}

    સ્થિર પૂર્ણાંક
set_padding_border(dict_T *dict, પૂર્ણાંક *array, અક્ષર *name, પૂર્ણાંક max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    જો (di == ખાલી)
	પાછા OK;

    જો (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	પાછા FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    પૂર્ણાંક		i;
    પૂર્ણાંક		nr;

    માટે (i = 0; i < 4; ++i)
	array[i] = 1;
    જો (list == ખાલી)
	પાછા OK;

    CHECK_LIST_MATERIALIZE(list);
    માટે (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (પૂર્ણાંક)tv_get_સંખ્યા(&li->li_tv);
	જો (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    પાછા OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    સ્થિર શૂન્ય
set_moved_કિંમતો(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    સ્થિર શૂન્ય
set_moved_columns(win_T *wp, પૂર્ણાંક flags)
{
    char_u	*ptr;
    પૂર્ણાંક		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    પૂર્ણાંક		mincol;

    જો (len <= 0)
	પાછા;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (પૂર્ણાંક)(ptr - ml_get_curline());
    જો (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	પાછા;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    સ્થિર શૂન્ય
set_mousemoved_કિંમતો(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    સ્થિર શૂન્ય
update_popup_uses_mouse_move(શૂન્ય)
{
    popup_uses_mouse_move = FALSE;
    જો (!popup_visible)
	પાછા;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    પાછા;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    પાછા;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    સ્થિર શૂન્ય
set_mousemoved_columns(win_T *wp, પૂર્ણાંક flags)
{
    win_T	*textwp;
    char_u	*text;
    પૂર્ણાંક		col;
    pos_T	pos;
    colnr_T	mcol;

    જો (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, ખાલી, &col) != OK)
	પાછા;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ખાલી, ખાલી, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, ખાલી, ખાલી, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    પૂર્ણાંક
popup_on_border(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    પાછા (row == 0 && wp->w_popup_border[0] > 0)
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
    પૂર્ણાંક
popup_close_if_on_X(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	પાછા TRUE;
    }
    પાછા FALSE;
}

// Values set when dragging a popup window starts.
સ્થિર પૂર્ણાંક drag_start_row;
સ્થિર પૂર્ણાંક drag_start_col;
સ્થિર પૂર્ણાંક drag_start_wantline;
સ્થિર પૂર્ણાંક drag_start_wantcol;
સ્થિર પૂર્ણાંક drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    શૂન્ય
popup_start_drag(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    જો (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    નહીંતર
	drag_start_wantline = wp->w_wantline;
    જો (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    નહીંતર
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    જો (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    જો (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	જો (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	જો (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    શૂન્ય
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    જો (!win_valid_popup(wp))
	પાછા;

    જો ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	પૂર્ણાંક width_inc = mouse_col - drag_start_col;
	પૂર્ણાંક height_inc = mouse_row - drag_start_row;

	જો (width_inc != 0)
	{
	    પૂર્ણાંક width = wp->w_width + width_inc;

	    જો (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	જો (height_inc != 0)
	{
	    પૂર્ણાંક height = wp->w_height + height_inc;

	    જો (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	પાછા;
    }

    જો (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	પાછા;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    જો (wp->w_wantline < 1)
	wp->w_wantline = 1;
    જો (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    જો (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    જો (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    શૂન્ય
popup_set_firstline(win_T *wp)
{
    પૂર્ણાંક	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    જ્યારે (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    પૂર્ણાંક
popup_is_in_scrollbar(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    પાછા wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    શૂન્ય
popup_handle_scrollbar_click(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (!popup_is_in_scrollbar(wp, row, col))
	પાછા;

    પૂર્ણાંક	    height = popup_height(wp);
    પૂર્ણાંક	    new_topline = wp->w_topline;

    જો (row >= height / 2)
    {
	// Click in lower half, scroll down.
	જો (wp->w_topline < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    ++new_topline;
    }
    નહીંતર જો (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    જો (new_topline == wp->w_topline)
	પાછા;

    set_topline(wp, new_topline);
    જો (wp == curwin)
    {
	જો (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	નહીંતર જો (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#જો defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    સ્થિર શૂન્ય
popup_add_timeout(win_T *wp, પૂર્ણાંક time, પૂર્ણાંક close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((અક્ષર *)cbbuf, કદ(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    જો (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	પાછા;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    જો (cb.cb_name != ખાલી && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    સ્થિર poppos_T
get_pos_entry(dict_T *d, પૂર્ણાંક give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    પૂર્ણાંક	    nr;

    જો (str == ખાલી)
	પાછા POPPOS_NONE;

    માટે (nr = 0; nr < (પૂર્ણાંક)ARRAY_LENGTH(poppos_entries); ++nr)
	જો (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    પાછા poppos_entries[nr].pp_val;

    જો (give_error)
	semsg(_(e_invalid_argument_str), str);
    પાછા POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    સ્થિર શૂન્ય
apply_move_options(win_T *wp, dict_T *d)
{
    પૂર્ણાંક		nr;
    char_u	*str;
    dictitem_T	*di;

    જો ((nr = dict_get_સંખ્યા_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    જો ((nr = dict_get_સંખ્યા_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    જો ((nr = dict_get_સંખ્યા_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    જો ((nr = dict_get_સંખ્યા_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    જો (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    જો (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    જો (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	જો (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    જો (str != ખાલી)
    {
	wp->w_popup_prop_type = 0;
	જો (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    જો (di != ખાલી)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		જો (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    જો (nr <= 0)
		nr = find_prop_type_id(str, ખાલી);
	    જો (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    નહીંતર
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    જો (di != ખાલી)
	wp->w_popup_prop_id = dict_get_સંખ્યા(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    સ્થિર શૂન્ય
handle_moved_argument(win_T *wp, dictitem_T *di, પૂર્ણાંક mousemoved)
{
    જો (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ખાલી)
    {
	char_u  *s = di->di_tv.vval.v_string;
	પૂર્ણાંક	flags = 0;

	જો (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	નહીંતર જો (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	નહીંતર જો (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	નહીંતર જો (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	જો (flags != 0)
	{
	    જો (mousemoved)
		set_mousemoved_columns(wp, flags);
	    નહીંતર
		set_moved_columns(wp, flags);
	}
    }
    નહીંતર જો (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ખાલી
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	પૂર્ણાંક	    mincol;
	પૂર્ણાંક	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	જો (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_સંખ્યા(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    જો (mousemoved)
		wp->w_popup_mouse_row = nr;
	    નહીંતર
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    જો (nr == 0)
		wp->w_popup_curwin = ખાલી;
	}

	mincol = tv_get_સંખ્યા(&li->li_tv);
	maxcol = tv_get_સંખ્યા(&li->li_next->li_tv);
	જો (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	નહીંતર
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    નહીંતર
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    સ્થિર શૂન્ય
check_highlight(dict_T *dict, અક્ષર *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    જો (di == ખાલી)
	પાછા;

    જો (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_કિંમત_for_argument_str), name);
    નહીંતર
    {
	str = tv_get_string(&di->di_tv);
	જો (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    સ્થિર શૂન્ય
popup_show_curline(win_T *wp)
{
    જો (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    નહીંતર જો (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	જો (wp->w_topline < 1)
	    wp->w_topline = 1;
	નહીંતર જો (wp->w_topline > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    wp->w_topline = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	જ્યારે (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_લીટી_ગણતરી
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    જો (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    સ્થિર char_u *
popup_get_sign_name(win_T *wp)
{
    સ્થિર અક્ષર    buf[30];

    vim_snprintf(buf, કદ(buf), "popup-%d", wp->w_id);
    પાછા (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    સ્થિર શૂન્ય
popup_highlight_curline(win_T *wp)
{
    પૂર્ણાંક	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    જો ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	જો (!sign_exists_by_name(sign_name))
	{
	    અક્ષર *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ખાલી, (char_u *)linehl, ખાલી, ખાલી, ખાલી,
		    ખાલી, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    નહીંતર
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    સ્થિર પૂર્ણાંક
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    પૂર્ણાંક		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    જો (di != ખાલી)
    {
	wp->w_firstline = dict_get_સંખ્યા(dict, "firstline");
	જો (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    જો (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    જો (str != ખાલી)
    {
	char_u	*title = vim_strsave(str);

	જો (title != ખાલી)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    જો (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    જો (di != ખાલી)
    {
	nr = dict_get_સંખ્યા(dict, "opacity");
	જો (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	નહીંતર જો (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	નહીંતર જો (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	નહીંતર
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    જો (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    નહીંતર
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    જો (di != ખાલી && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ખાલી)
    {
	image_T		    *img;
	image_T		    *cur_img = ખાલી;
	image_placement_T   *place;
	પૂર્ણાંક		    cw, ch;

	જો (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    જો (wp->w_popup_imagep != ખાલી)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ખાલી;
	    }
	    જાઓ image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	જો (wp->w_popup_imagep != ખાલી && wp->w_popup_imagep->img != ખાલી)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	જો (img == ખાલી)
	    પાછા FAIL;

	જો (img == cur_img)
	    જાઓ image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	જો (place == ખાલી)
	{
	    image_unref(img);
	    પાછા FAIL;
	}

	જો (wp->w_popup_imagep != ખાલી)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	જો (dict_find(dict, (char_u *)"minwidth", -1) == ખાલી)
	    wp->w_minwidth = cw;
	જો (dict_find(dict, (char_u *)"maxwidth", -1) == ખાલી)
	    wp->w_maxwidth = cw;
	જો (dict_find(dict, (char_u *)"minheight", -1) == ખાલી)
	    wp->w_minheight = ch;
	જો (dict_find(dict, (char_u *)"maxheight", -1) == ખાલી)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    નહીંતર
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	જો (wp->w_popup_imagep != ખાલી)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ખાલી, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	જો (wp->w_popup_imagep != ખાલી)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    જો (di != ખાલી)
    {
	પૂર્ણાંક ok = TRUE;

	જો (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ખાલી)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    જો (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    નહીંતર જો (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    નહીંતર જો (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    નહીંતર
		ok = FALSE;
	}
	નહીંતર
	    ok = FALSE;
	જો (!ok)
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    જો (str != ખાલી)
    {
	અક્ષર *errmsg = update_wincolor(wp, str);

	જો (errmsg == ખાલી)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	નહીંતર
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    જો (str != ખાલી)
    {
	અક્ષર *errmsg = update_winhighlight(wp, str);

	જો (errmsg == ખાલી)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	નહીંતર
	    emsg(_(errmsg));
    }

    જો (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	પાછા FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    જો (di != ખાલી)
    {
	જો (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ખાલી)
	{
	    emsg(_(e_list_required));
	    પાછા FAIL;
	}
	નહીંતર
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    પૂર્ણાંક		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    જો (list->lv_len == 0)
	    {
		માટે (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ખાલી;
		}
	    }
	    નહીંતર
	    {
		માટે (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    જો (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			જો (hl != ખાલી)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		જો (list->lv_len == 1 && wp->w_border_highlight[0] != ખાલી)
		    માટે (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			જો (hl != ખાલી)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    જો (di != ખાલી)
    {
	જો (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    પાછા FAIL;
	}
	નહીંતર
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    પૂર્ણાંક		i;

	    જો (list != ખાલી)
	    {
		CHECK_LIST_MATERIALIZE(list);
		માટે (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    જો (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		જો (list->lv_len == 1)
		    માટે (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		જો (list->lv_len == 2)
		{
		    માટે (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    માટે (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    જો (di != ખાલી)
    {
	wp->w_zindex = dict_get_સંખ્યા(dict, "zindex");
	જો (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	જો (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    જો (di != ખાલી)
    {
	પૂર્ણાંક ok = FALSE;

	જો (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ખાલી)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_વસ્તુઓ(di->di_tv.vval.v_list, li)
	    {
		જો (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ખાલી
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    તોડો;
		}
		નહીંતર
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	જો (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	નહીંતર
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), "mask");
	    પાછા FAIL;
	}
    }

#જો defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_સંખ્યા(dict, "time");
    જો (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    જો (di != ખાલી)
    {
	set_moved_કિંમતો(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    જો (di != ખાલી)
    {
	set_mousemoved_કિંમતો(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    જો (nr != -1)
    {
	જો (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    જો (di != ખાલી)
    {
	callback_T	callback = get_callback(&di->di_tv);

	જો (callback.cb_name != ખાલી)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    જો (nr != -1)
    {
	જો (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    જો (str != ખાલી)
    {
	જો (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	નહીંતર
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    જો (di == ખાલી)
	પાછા OK;

    callback_T	callback = get_callback(&di->di_tv);
    જો (callback.cb_name == ખાલી)
	પાછા OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    પાછા OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    સ્થિર પૂર્ણાંક
apply_options(win_T *wp, dict_T *dict, પૂર્ણાંક create)
{
    પૂર્ણાંક		nr;

    apply_move_options(wp, dict);

    જો (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    જો (apply_general_options(wp, dict) == FAIL)
	પાછા FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    જો (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    જો (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	જો (wp->w_firstline > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	નહીંતર જો (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    જો (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    પાછા OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    સ્થિર શૂન્ય
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_વસ્તુઓ(l, li)
	જો (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ખાલી ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    સ્થિર શૂન્ય
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	જો (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    પાછા;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ખાલી ? ખાલી : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ખાલી ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    માટે (li = l->lv_first; li != ખાલી; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	જો (di != ખાલી)
	{
	    જો (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		પાછા;
	    }
	    plist = di->di_tv.vval.v_list;
	    જો (plist != ખાલી)
	    {
		FOR_ALL_LIST_વસ્તુઓ(plist, pli)
		{
		    જો (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			પાછા;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    જો (dict != ખાલી)
		    {
			પૂર્ણાંક col = dict_get_સંખ્યા(dict, "col");

			prop_add_common(lnum, col, dict, buf, ખાલી);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    પૂર્ણાંક
popup_ટોચ_extra(win_T *wp)
{
    પૂર્ણાંક	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    જો (extra == 0 && wp->w_popup_title != ખાલી && *wp->w_popup_title != NUL)
	પાછા 1;
    પાછા extra;
}

/*
 * Get the padding plus border at the left.
 */
    પૂર્ણાંક
popup_ડાબું_extra(win_T *wp)
{
    પાછા wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    પૂર્ણાંક
popup_height(win_T *wp)
{
    પાછા wp->w_height
	    + popup_ટોચ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    પૂર્ણાંક
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    પાછા wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    પૂર્ણાંક
popup_extra_width(win_T *wp)
{
    પાછા wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    સ્થિર win_T *
popup_get_clipwin(win_T *wp)
{
    જો (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	પાછા ખાલી;
    જો (win_valid(wp->w_popup_prop_win))
	પાછા wp->w_popup_prop_win;
    પાછા ખાલી;
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
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક ટોચ_extra;
    પૂર્ણાંક bot_extra;
    પૂર્ણાંક ડાબું_extra;
    પૂર્ણાંક જમણું_extra;

    પૂર્ણાંક clip_ટોચ_content;
    પૂર્ણાંક clip_bot_content;
    પૂર્ણાંક clip_ડાબું_content;
    પૂર્ણાંક clip_જમણું_content;

    પૂર્ણાંક eff_ટોચ_extra;
    પૂર્ણાંક eff_bot_extra;
    પૂર્ણાંક eff_ડાબું_extra;
    પૂર્ણાંક eff_જમણું_extra;

    પૂર્ણાંક eff_border[4];
    પૂર્ણાંક eff_padding[4];

    પૂર્ણાંક eff_height;
    પૂર્ણાંક eff_width;
} popup_clip_T;

    સ્થિર શૂન્ય
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    પૂર્ણાંક h, w;

    cl->ટોચ_extra = popup_ટોચ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ડાબું_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->જમણું_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_ટોચ_content = wp->w_popup_topoff - cl->ટોચ_extra;
    જો (cl->clip_ટોચ_content < 0)
	cl->clip_ટોચ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    જો (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ડાબું_content = wp->w_popup_leftclip - cl->ડાબું_extra;
    જો (cl->clip_ડાબું_content < 0)
	cl->clip_ડાબું_content = 0;
    cl->clip_જમણું_content = wp->w_popup_rightclip - cl->જમણું_extra;
    જો (cl->clip_જમણું_content < 0)
	cl->clip_જમણું_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	પૂર્ણાંક clip = wp->w_popup_topoff;
	પૂર્ણાંક b = wp->w_popup_border[0];
	પૂર્ણાંક p = wp->w_popup_padding[0];
	પૂર્ણાંક rem;

	જો (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	નહીંતર
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	પૂર્ણાંક clip = wp->w_popup_bottomoff;
	પૂર્ણાંક b = wp->w_popup_border[2];
	પૂર્ણાંક p = wp->w_popup_padding[2];
	પૂર્ણાંક rem;

	જો (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	નહીંતર
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
	પૂર્ણાંક excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	જો (excess > 0)
	{
	    જો (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		જો (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		નહીંતર
		    cl->eff_border[0] -= excess;
	    }
	    નહીંતર
		cl->eff_padding[0] -= excess;
	}
    }
    {
	પૂર્ણાંક excess = wp->w_popup_topoff - cl->ટોચ_extra - wp->w_height;
	જો (excess > 0)
	{
	    જો (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		જો (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		નહીંતર
		    cl->eff_border[2] -= excess;
	    }
	    નહીંતર
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_ટોચ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ડાબું_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ડાબું_extra;
    cl->eff_જમણું_extra = wp->w_popup_rightclip > 0 ? 0 : cl->જમણું_extra;

    h = wp->w_height - cl->clip_ટોચ_content - cl->clip_bot_content;
    જો (h < 0)
	h = 0;
    cl->eff_height = cl->eff_ટોચ_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_ડાબું_content - cl->clip_જમણું_content;
    જો (w < 0)
	w = 0;
    cl->eff_width = cl->eff_ડાબું_extra + w + cl->eff_જમણું_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		height;
    પૂર્ણાંક		width;
    પૂર્ણાંક		winrow;
    પૂર્ણાંક		wincol;
    પૂર્ણાંક		leftcol;
    પૂર્ણાંક		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    સ્થિર શૂન્ય
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

    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
popup_screenpos_above_ટોચ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	પૂર્ણાંક	    *screen_row,
	પૂર્ણાંક	    *screen_scol,
	પૂર્ણાંક	    *screen_ccol,
	પૂર્ણાંક	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    પૂર્ણાંક	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (પૂર્ણાંક)win_col_off(prop_win) - (પૂર્ણાંક)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (પૂર્ણાંક)scol + coloff;
    *screen_ccol = (પૂર્ણાંક)ccol + coloff;
    *screen_ecol = (પૂર્ણાંક)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (પૂર્ણાંક)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    સ્થિર શૂન્ય
popup_hide_for_textprop(win_T *wp)
{
    જો ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	પાછા;
    wp->w_popup_flags |= POPF_HIDDEN;
    જો (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    સ્થિર bool
popup_find_prop_above_ટોચ(
	win_T	    *wp,
	win_T	    *prop_win,
	પૂર્ણાંક	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    જો (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	પાછા false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    જો (first < 1)
	first = 1;
    પાછા find_prop_in_લીટીઓ(prop_win,
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
    સ્થિર bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    પૂર્ણાંક	    extra_h, extra_w;
    પૂર્ણાંક	    popup_ટોચ, popup_bottom, popup_ડાબું, popup_જમણું;
    પૂર્ણાંક	    total_h, total_w;

    જો (cw == ખાલી)
	પાછા false;

    extra_h = popup_ટોચ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_ટોચ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_ડાબું = wp->w_wincol;
    popup_જમણું = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    જો (popup_ટોચ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_ટોચ;
    જો (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    જો (popup_ડાબું < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ડાબું;
    જો (popup_જમણું > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_જમણું - (cw->w_wincol + cw->w_width);

    પાછા wp->w_popup_topoff >= total_h
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
    સ્થિર શૂન્ય
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    જો (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_ટોચ_content + cl->clip_bot_content;
	જો (wp->w_height < 0)
	    wp->w_height = 0;
	જો (cl->clip_ટોચ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_ટોચ_content;
    }
    જો (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	જો (cl->clip_ડાબું_content > 0 || cl->clip_જમણું_content > 0)
	    wp->w_p_wrap = FALSE;
	જો (cl->clip_જમણું_content > 0)
	{
	    wp->w_width -= cl->clip_જમણું_content;
	    જો (wp->w_width < 0)
		wp->w_width = 0;
	}
	જો (cl->clip_ડાબું_content > 0)
	{
	    wp->w_leftcol += cl->clip_ડાબું_content;
	    wp->w_wincol += cl->clip_ડાબું_content;
	    wp->w_width -= cl->clip_ડાબું_content;
	    જો (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    સ્થિર શૂન્ય
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    પૂર્ણાંક iw, ih;

    // Visible cell width and height of popup itself (not image!)
    પૂર્ણાંક	visible_width;
    પૂર્ણાંક	visible_height;

    // Crop dimensions of image (in pixels).
    પૂર્ણાંક	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    જો (wp->w_popup_imagep == ખાલી || wp->w_popup_flags & POPF_HIDDEN)
	પાછા;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    જો (wp->w_popup_imagep->img == ખાલી)
	// Just set the bounding box
	જાઓ exit;

    visible_width =
	wp->w_width - clip.clip_ડાબું_content - clip.clip_જમણું_content;
    visible_height =
	wp->w_height - clip.clip_ટોચ_content - clip.clip_bot_content;

    જો (visible_width < 0 || visible_height < 0)
	પાછા;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ડાબું_content;
    crop_y = clip.clip_ટોચ_content;
    crop_width = iw - crop_x - clip.clip_જમણું_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    જો (crop_width > visible_width)
	crop_width = visible_width;
    જો (crop_height > visible_height)
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
    સ્થિર શૂન્ય
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    પૂર્ણાંક		wrapped = 0;
    પૂર્ણાંક		maxwidth;
    પૂર્ણાંક		maxwidth_no_scrollbar;
    પૂર્ણાંક		width_with_scrollbar = 0;
    પૂર્ણાંક		used_maxwidth = FALSE;
    પૂર્ણાંક		margin_width = 0;
    પૂર્ણાંક		maxspace;
    પૂર્ણાંક		center_vert = FALSE;
    પૂર્ણાંક		center_hor = FALSE;
    પૂર્ણાંક		allow_adjust_ડાબું = !wp->w_popup_fixed;
    પૂર્ણાંક		ટોચ_extra = popup_ટોચ_extra(wp);
    પૂર્ણાંક		જમણું_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    પૂર્ણાંક		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    પૂર્ણાંક		ડાબું_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    પૂર્ણાંક		extra_height = ટોચ_extra + bot_extra;
    પૂર્ણાંક		extra_width = ડાબું_extra + જમણું_extra;
    પૂર્ણાંક		w_height_before_limit;
    popup_layout_T org_layout;
    પૂર્ણાંક		minwidth, minheight;
    પૂર્ણાંક		maxheight = Rows;
    પૂર્ણાંક		wantline = wp->w_wantline;  // adjusted for textprop
    પૂર્ણાંક		wantcol = wp->w_wantcol;    // adjusted for textprop
    પૂર્ણાંક		use_wantcol = wantcol != 0;
    પૂર્ણાંક		adjust_height_for_ટોચ_aligned = FALSE;

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
    જો (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    જો (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	પૂર્ણાંક	    screen_row;
	પૂર્ણાંક	    screen_scol;
	પૂર્ણાંક	    screen_ccol;
	પૂર્ણાંક	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_ટોચ = false;
	જો (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    જો (popup_find_prop_above_ટોચ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_ટોચ = true;
	    નહીંતર
	    {
		જો ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    જો (wp->w_popup_imagep != ખાલી)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    જો (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		પાછા;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	જો (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	જો (prop_above_ટોચ)
	    popup_screenpos_above_ટોચ(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	નહીંતર
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	જો (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    પાછા;
	}
	જો (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	નહીંતર
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	જો (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	નહીંતર
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    નહીંતર
    {
	// If no line was specified default to vertical centering.
	જો (wantline == 0)
	    center_vert = TRUE;
	નહીંતર જો (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	જો (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    જો (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    નહીંતર
    {
	જો (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    જો (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	જો (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_લીટી_ગણતરી - 1, 0);
	}

	જો (!use_wantcol)
	    center_hor = TRUE;
	નહીંતર જો (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    જો (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- ડાબું_extra - જમણું_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- ડાબું_extra - જમણું_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    જો (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - ડાબું_extra - જમણું_extra;
    maxwidth = maxspace;
    જો (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ડાબું = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    જો (wp->w_p_nu || wp->w_p_rnu)
	margin_width = સંખ્યા_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    જો (signcolumn_on(wp))
	margin_width += 2;
#endif
    જો (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    જો (wp->w_buffer->b_term != ખાલી)
    {
	જો (minwidth == 0)
	    minwidth = 20;
	જો (minheight == 0)
	    minheight = 5;
    }
#endif

    જો (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    નહીંતર જો (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    જો (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	જો (wp->w_firstline > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    wp->w_firstline = 0;
	નહીંતર
	    wp->w_topline = wp->w_firstline;
    }
    જો (wp->w_topline < 1)
	wp->w_topline = 1;
    નહીંતર જો (wp->w_topline > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	wp->w_topline = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;

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
	પૂર્ણાંક saved_w_width = wp->w_width;

	જો (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	માટે (ln = 1; ln <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી; ++ln)
	{
	    પૂર્ણાંક len = linetabsize(wp, ln) + margin_width;

	    જો (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    જો (saved_w_width < len)
		saved_w_width = len;
	    જો (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		તોડો;
	}
	wp->w_width = saved_w_width;
    }
    જો (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
    નહીંતર
	lnum = wp->w_topline;
    જ્યારે (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
    {
	પૂર્ણાંક len;
	પૂર્ણાંક w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	જો (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	જો (len + margin_width > maxwidth
		&& allow_adjust_ડાબું
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    પૂર્ણાંક shift_by = len + margin_width - maxwidth;

	    જો (shift_by > wp->w_wincol)
	    {
		પૂર્ણાંક truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    જો (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    જો (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	જો (wp->w_p_wrap)
	{
	    જ્યારે (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	જો (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    જો (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	જો (wp->w_firstline < 0)
	    --lnum;
	નહીંતર
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	જો (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_લીટી_ગણતરી - lnum)
		       + wrapped >= maxheight)
	    તોડો;
    }

    જો (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી);
#ifdef FEAT_TERMINAL
    જો (wp->w_buffer->b_term != ખાલી && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    જો (wp->w_has_scrollbar)
    {
	++જમણું_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	જો (maxwidth + જમણું_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    જો (wp->w_popup_title != ખાલી && *wp->w_popup_title != NUL)
    {
	પૂર્ણાંક title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	જો (minwidth < title_len)
	    minwidth = title_len;
    }

    જો (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    જો (wp->w_width > maxwidth)
    {
	જો (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	જો (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    પૂર્ણાંક off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, જમણું_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	નહીંતર
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - જમણું_extra;
	}
    }
    જો (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	જો (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    નહીંતર જો (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	પૂર્ણાંક leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	જો (leftoff >= 0)
	    wp->w_wincol = leftoff;
	નહીંતર જો (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    જો (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    જો (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	પૂર્ણાંક want_col = 0;

	// try to show the right border and any scrollbar
	want_col = ડાબું_extra + wp->w_width + જમણું_extra;
	જો (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    જો (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_લીટી_ગણતરી - wp->w_topline
								 + 1 + wrapped;
    જો (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    જો (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    જો (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    જો (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	જો (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    નહીંતર જો (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	જો ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	નહીંતર જો (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	નહીંતર
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_ટોચ_aligned = TRUE;
	}
    }
    નહીંતર જો (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	પૂર્ણાંક check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	જો (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    જો (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    જો (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	નહીંતર
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_ટોચ_aligned = TRUE;
	}
    }

    જો (adjust_height_for_ટોચ_aligned && wp->w_want_scrollbar
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
	જો (wp->w_buffer->b_term == ખાલી || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    જો (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		જો (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		નહીંતર
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    જો (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    નહીંતર જો (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    જો (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    જો (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    જો (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    જો (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	પૂર્ણાંક avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    જો (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	જો (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	જો (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	જો (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    પૂર્ણાંક avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    જો (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    જો (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	જો (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    જો (wp->w_popup_imagep != ખાલી)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	પાછા;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    જો (win_valid(wp->w_popup_prop_win))
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
    જો (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	જો (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

પ્રકારવ્યાખ્યા ગણના
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
    સ્થિર પૂર્ણાંક
popup_is_notification(create_type_T type)
{
    પાછા type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    સ્થિર શૂન્ય
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    પૂર્ણાંક	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    માટે (lnum = buf->b_ml.ml_લીટી_ગણતરી; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    જો (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ખાલી ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    નહીંતર
    {
	list_T *l = text.vval.v_list;

	જો (l != ખાલી && l->lv_len > 0)
	{
	    જો (l->lv_first == &range_list_item)
		emsg(_(e_using_સંખ્યા_as_string));
	    નહીંતર જો (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    નહીંતર
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_લીટી_ગણતરી);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    કરો {						    \
	જો (wp != ખાલી)					    \
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
    } જ્યારે (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    સ્થિર પૂર્ણાંક
parse_popup_option(win_T *wp, પૂર્ણાંક is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    પૂર્ણાંક	    border_enabled = FALSE;

    જો (wp != ખાલી)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    માટે ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	પૂર્ણાંક	x;

	e = vim_strchr(p, ':');
	જો (e == ખાલી || e[1] == NUL)
	    પાછા FAIL;

	p = vim_strchr(e, ',');
	જો (p == ખાલી)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	જો (STRNCMP(s, "height:", 7) == 0)
	{
	    જો (dig != p)
		પાછા FAIL;
	    જો (wp != ખાલી)
	    {
		જો (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	નહીંતર જો (STRNCMP(s, "width:", 6) == 0)
	{
	    જો (dig != p)
		પાછા FAIL;
	    જો (wp != ખાલી)
	    {
		જો (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	નહીંતર જો (STRNCMP(s, "highlight:", 10) == 0)
	{
	    જો (wp != ખાલી)
	    {
		અક્ષર *errmsg;
		પૂર્ણાંક c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		જો (errmsg == ખાલી)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		નહીંતર
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	નહીંતર જો (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    જો (*arg == NUL || *arg == ',')
		પાછા FAIL;
	    જો (wp != ખાલી)
	    {
		માટે (પૂર્ણાંક i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	નહીંતર જો (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    પૂર્ણાંક		i;
	    પૂર્ણાંક		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    પૂર્ણાંક		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    જો (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		જો (wp != ખાલી)
		{
		    માટે (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ચાલુ;
	    }

	    token = vim_strnsave(arg, token_len);
	    જો (token == ખાલી)
		પાછા FAIL;

	    જો ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		જો (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		નહીંતર જો (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		નહીંતર જો (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		નહીંતર જો (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		નહીંતર જો (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		નહીંતર જો (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    પૂર્ણાંક		out[8];
		    પૂર્ણાંક		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    માટે (i = 0; i < 8 && !failed; i++)
		    {
			જો (*q == NUL)
			    failed = TRUE;
			નહીંતર
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    જો (i < 7)
			    {
				જો (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    જો (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			પાછા FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    નહીંતર
	    {
		vim_free(token);
		પાછા FAIL;
	    }

	    જો (wp != ખાલી)
	    {
		માટે (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	નહીંતર જો (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    પૂર્ણાંક		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    પૂર્ણાંક		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    જો (!on && !off)
		પાછા FAIL;
	    on = on && mouse_has(MOUSE_ઉમેરો) && (border_enabled || is_preview);
	    જો (wp != ખાલી)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	નહીંતર જો (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    પૂર્ણાંક		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    પૂર્ણાંક		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    જો (!on && !off)
		પાછા FAIL;
	    જો (wp != ખાલી)
	    {
		જો (on && mouse_has(MOUSE_ઉમેરો))
		    wp->w_popup_flags |= POPF_RESIZE;
		નહીંતર
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	નહીંતર જો (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    પૂર્ણાંક		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    પૂર્ણાંક		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    જો (!on && !off)
		પાછા FAIL;
	    જો (wp != ખાલી)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	નહીંતર જો (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    પૂર્ણાંક		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    પૂર્ણાંક		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    જો (!menu && !item)
		પાછા FAIL;
	    જો (wp != ખાલી && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	નહીંતર જો (STRNCMP(s, "opacity:", 8) == 0)
	{
	    જો (dig != p || x < 0 || x > 100)
		પાછા FAIL;
	    જો (wp != ખાલી)
	    {
		જો (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		નહીંતર
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	નહીંતર
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    પૂર્ણાંક
parse_previewpopup(win_T *wp)
{
    પાછા parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    પૂર્ણાંક
parse_completepopup(win_T *wp)
{
    પાછા parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    શૂન્ય
popup_set_wantpos_cursor(win_T *wp, પૂર્ણાંક width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    જો (d != ખાલી)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    જો (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    નહીંતર
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	જો (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    જો (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	જો (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    શૂન્ય
popup_set_wantpos_rowcol(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    સ્થિર શૂન્ય
add_border_ડાબું_જમણું_padding(win_T *wp)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    સ્થિર પૂર્ણાંક
popup_terminal_exists(શૂન્ય)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_buffer->b_term != ખાલી)
	    પાછા TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    જો (wp->w_buffer->b_term != ખાલી)
		પાછા TRUE;
    પાછા FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    શૂન્ય
popup_redraw_all(શૂન્ય)
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
    પૂર્ણાંક
popup_need_redraw(શૂન્ય)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	જો ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    પાછા TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Set the color for a notification window.
 */
    સ્થિર શૂન્ય
popup_update_color(win_T *wp, create_type_T type)
{
    અક્ષર    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    અક્ષર    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    જો (errmsg == ખાલી)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    નહીંતર
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    સ્થિર win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ખાલી;
    પૂર્ણાંક		tabnr = 0;
    પૂર્ણાંક		new_buffer;
    buf_T	*buf = ખાલી;
    dict_T	*d = ખાલી;
    પૂર્ણાંક		i;

    જો (check_secure())
	પાછા ખાલી;

    જો (argvars != ખાલી)
    {
	જો (in_vim9script()
		&& (check_for_string_or_સંખ્યા_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    પાછા ખાલી;

	// Check that arguments look OK.
	જો (argvars[0].v_type == VAR_સંખ્યા)
	{
	    buf = buflist_findnr(argvars[0].vval.v_સંખ્યા);
	    જો (buf == ખાલી)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_સંખ્યા);
		પાછા ખાલી;
	    }
#ifdef FEAT_TERMINAL
	    જો (buf->b_term != ખાલી && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		પાછા ખાલી;
	    }
#endif
	}
	નહીંતર જો (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ખાલી)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ખાલી))
	{
	    emsg(_(e_buffer_સંખ્યા_text_or_list_required));
	    પાછા ખાલી;
	}
	જો (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    પાછા ખાલી;
	d = argvars[1].vval.v_dict;
    }

    જો (d != ખાલી)
    {
	જો (dict_has_ચાવી(d, "tabpage"))
	    tabnr = (પૂર્ણાંક)dict_get_સંખ્યા(d, "tabpage");
	નહીંતર જો (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	નહીંતર
	    tabnr = 0;
	જો (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    જો (tp == ખાલી)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		પાછા ખાલી;
	    }
	}
    }
    નહીંતર જો (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    જો (buf != ખાલી && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	પાછા ખાલી;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    જો (wp == ખાલી)
	પાછા ખાલી;
    જો (rettv != ખાલી)
	rettv->vval.v_સંખ્યા = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    જો (buf != ખાલી)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    નહીંતર
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(ખાલી, ખાલી, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	જો (buf == ખાલી)
	{
	    win_free_popup(wp);
	    પાછા ખાલી;
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

    જો (tp != ખાલી)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    નહીંતર જો (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    નહીંતર // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	જો (first_popupwin == ખાલી)
	    first_popupwin = wp;
	નહીંતર
	{
	    જ્યારે (prev->w_next != ખાલી)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    જો (new_buffer && argvars != ખાલી)
	popup_set_buffer_text(buf, argvars[0]);

    જો (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    જો (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_કિંમતો(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    જો (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	જો (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_કિંમતો(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    જો (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	પૂર્ણાંક	height = buf->b_ml.ml_લીટી_ગણતરી + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	માટે (twp = first_popupwin; twp != ખાલી; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    જો (twp != wp
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
	જો (wp->w_wantline + height > Rows)
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
	માટે (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    જો (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_ડાબું_જમણું_padding(wp);
    }

    જો (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	જો (callback.cb_name != ખાલી)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    માટે (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    માટે (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    જો (type == TYPE_PREVIEW)
    {
	જો (mouse_has(MOUSE_ઉમેરો))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	માટે (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    જો (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	જો (mouse_has(MOUSE_ઉમેરો))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_ડાબું_જમણું_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    જો (d != ખાલી)
    {
	// Deal with options.
	જો (apply_options(wp, d, TRUE) == FAIL)
	{
	    (શૂન્ય)popup_close(wp->w_id, FALSE);
	    પાછા ખાલી;
	}
    }

#ifdef FEAT_TIMERS
    જો (popup_is_notification(type) && wp->w_popup_timer == ખાલી)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    જો (buf->b_term != ખાલી)
	win_enter(wp, FALSE);
#endif

    પાછા wp;
}

/*
 * popup_clear()
 */
    શૂન્ય
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક force = FALSE;

    જો (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
	force = (પૂર્ણાંક)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    શૂન્ય
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    શૂન્ય
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    શૂન્ય
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    સ્થિર શૂન્ય
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_સંખ્યા;
    argv[0].vval.v_સંખ્યા = (varnumber_T)wp->w_id;

    જો (result != ખાલી && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    નહીંતર
    {
	argv[1].v_type = VAR_સંખ્યા;
	argv[1].vval.v_સંખ્યા = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    જો (result != ખાલી)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    સ્થિર શૂન્ય
back_to_prevwin(win_T *wp)
{
    જો (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    નહીંતર
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    સ્થિર શૂન્ય
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    પૂર્ણાંક id;

    જો (!win_valid(wp))
       પાછા;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    જો (wp == curwin && curbuf->b_term != ખાલી)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    જો (owp != curwin && owp->w_buffer->b_term != ખાલી)
		તોડો;
	જો (owp != ખાલી)
	    win_enter(owp, FALSE);
	નહીંતર
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		જો (owp != curwin && owp->w_buffer->b_term != ખાલી)
		    તોડો;
	    જો (owp != ખાલી)
		win_enter(owp, FALSE);
	    નહીંતર
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    જો (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	પાછા;
    }

    CHECK_CURBUF;
    જો (wp->w_close_cb.cb_name != ખાલી)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    શૂન્ય
popup_close_with_retval(win_T *wp, પૂર્ણાંક retval)
{
    typval_T res;

    res.v_type = VAR_સંખ્યા;
    res.vval.v_સંખ્યા = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    શૂન્ય
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    સ્થિર શૂન્ય
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    જો (wp != mouse_wp
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
    શૂન્ય
popup_handle_mouse_moved(શૂન્ય)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    પૂર્ણાંક	    row = mouse_row;
    પૂર્ણાંક	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    માટે (wp = first_popupwin; wp != ખાલી; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    માટે (wp = curtab->tp_first_popupwin; wp != ખાલી; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    સ્થિર શૂન્ય
filter_handle_drag(win_T *wp, પૂર્ણાંક c, typval_T *rettv)
{
    પૂર્ણાંક	row = mouse_row;
    પૂર્ણાંક	col = mouse_col;

    જો ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_ચાવી(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_સંખ્યા = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    શૂન્ય
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		id;
    win_T	*wp;
    char_u	*ચાવી;
    typval_T	res;
    પૂર્ણાંક		c;
    linenr_T	old_lnum;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    id = tv_get_સંખ્યા(&argvars[0]);
    wp = win_id2wp(id);
    ચાવી = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    જો (wp == ખાલી)
	પાછા;

    c = *ચાવી;
    જો (c == K_SPECIAL && ચાવી[1] != NUL)
	c = TO_SPECIAL(ચાવી[1], ચાવી[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_TRUE;
    res.v_type = VAR_સંખ્યા;

    old_lnum = wp->w_cursor.lnum;
    જો (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	જો (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	નહીંતર
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
    }
    જો (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	જો (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    ++wp->w_cursor.lnum;
	નહીંતર
	    wp->w_cursor.lnum = 1;
    }
    જો (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	પાછા;
    }

    જો (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_સંખ્યા = -1;
	popup_close_and_callback(wp, &res);
	પાછા;
    }
    જો (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_સંખ્યા = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	પાછા;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    શૂન્ય
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		id;
    win_T	*wp;
    char_u	*ચાવી;
    typval_T	res;
    પૂર્ણાંક		c;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    id = tv_get_સંખ્યા(&argvars[0]);
    wp = win_id2wp(id);
    ચાવી = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    જો (wp == ખાલી)
	પાછા;

    c = *ચાવી;
    જો (c == CAR && need_wait_return)
	પાછા;
    જો (c == K_SPECIAL && ચાવી[1] != NUL)
	c = TO_SPECIAL(ચાવી[1], ચાવી[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_TRUE;

    જો (c == 'y' || c == 'Y')
	res.vval.v_સંખ્યા = 1;
    નહીંતર જો (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_સંખ્યા = 0;
    નહીંતર
    {
	filter_handle_drag(wp, c, rettv);
	પાછા;
    }

    // Invoke callback
    res.v_type = VAR_સંખ્યા;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    શૂન્ય
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    શૂન્ય
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    શૂન્ય
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    સ્થિર win_T *
find_popup_win(પૂર્ણાંક id)
{
    win_T *wp = win_id2wp(id);

    જો (wp != ખાલી && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	પાછા ખાલી;
    }
    પાછા wp;
}

/*
 * popup_close({id})
 */
    શૂન્ય
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		id;
    win_T	*wp;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    જો (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == ખાલી &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    wp = find_popup_win(id);
    જો (wp != ખાલી)
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
    સ્થિર શૂન્ય
popup_clear_mask_for(win_T *wp)
{
    પૂર્ણાંક r, c;
    પૂર્ણાંક row_start, col_start, row_end, col_end;

    જો (popup_mask == ખાલી || !popup_visible)
	પાછા;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (પૂર્ણાંક)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (પૂર્ણાંક)screen_Columns);

    માટે (r = row_start; r < row_end; ++r)
	માટે (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    શૂન્ય
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    પૂર્ણાંક			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    જો (error_if_term_popup_window())
	પાછા;
#endif
    જો ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	પાછા;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    જો (wp->w_popup_imagep != ખાલી)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    જો (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    જો (was_visible)
	popup_clear_mask_for(wp);

    જો (old_area.active)
	popup_redraw_exposed_area(&old_area);
    નહીંતર
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    જો (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    શૂન્ય
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		id;
    win_T	*wp;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    શૂન્ય
popup_show(win_T *wp)
{
    bool popup_active;

    જો ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	પાછા;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    જો (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_લીટીઓ_valid = 0;
	જો (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    નહીંતર
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    શૂન્ય
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		id;
    win_T	*wp;

    rettv->v_type = VAR_સંખ્યા;
    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    જો (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_સંખ્યા = 0;
}

/*
 * popup_settext({id}, {text})
 */
    શૂન્ય
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		id;
    win_T	*wp;
    popup_area_T	old_area;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;

    popup_save_area(wp, &old_area);

    જો (check_for_string_or_list_arg(argvars, 1) == FAIL)
	પાછા;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_લીટીઓ_valid = 0;
    જો (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    જો (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    શૂન્ય
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_FALSE;

    જો (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;

    buf = tv_get_buf_from_arg(&argvars[1]);

    જો (buf == ખાલી)
	પાછા;
#ifdef FEAT_TERMINAL
    જો (buf->b_term != ખાલી && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	પાછા;
    }
#endif

    જો (wp->w_buffer != buf)
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

	જો (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_સંખ્યા = VVAL_TRUE;
}

    સ્થિર શૂન્ય
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    પૂર્ણાંક			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    જો (wp->w_popup_imagep != ખાલી)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    જો (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    જો (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    જો (wp == message_win)
	message_win = ખાલી;
#endif

    જો (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    જો (!old_area.active)
	popup_mask_refresh = TRUE;
}

    સ્થિર શૂન્ય
error_for_popup_window(શૂન્ય)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    પૂર્ણાંક
error_if_popup_window(પૂર્ણાંક also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    જો (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == ખાલી)
#endif
	    )
    {
	error_for_popup_window();
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    પૂર્ણાંક
popup_close(પૂર્ણાંક id, પૂર્ણાંક force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ખાલી;

    // go through global popups
    માટે (wp = first_popupwin; wp != ખાલી; prev = wp, wp = wp->w_next)
	જો (wp->w_id == id)
	{
	    જો (wp == curwin)
	    {
		જો (!force)
		{
		    error_for_popup_window();
		    પાછા FAIL;
		}
		back_to_prevwin(wp);
	    }
	    જો (prev == ખાલી)
		first_popupwin = wp->w_next;
	    નહીંતર
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    પાછા OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	જો (popup_close_tabpage(tp, id, force) == OK)
	    પાછા OK;
    પાછા FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    પૂર્ણાંક
popup_close_tabpage(tabpage_T *tp, પૂર્ણાંક id, પૂર્ણાંક force)
{
    win_T	*wp;
    win_T	**મૂળ = &tp->tp_first_popupwin;
    win_T	*prev = ખાલી;

    માટે (wp = *મૂળ; wp != ખાલી; prev = wp, wp = wp->w_next)
	જો (wp->w_id == id)
	{
	    જો (wp == curwin)
	    {
		જો (!force)
		{
		    error_for_popup_window();
		    પાછા FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    જો (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    જો (prev == ખાલી)
		*મૂળ = wp->w_next;
	    નહીંતર
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    પાછા OK;
	}
    પાછા FAIL;
}

    શૂન્ય
close_all_popups(પૂર્ણાંક force)
{
    જો (!force && ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;
    જ્યારે (first_popupwin != ખાલી)
	જો (popup_close(first_popupwin->w_id, force) == FAIL)
	    પાછા;
    જ્યારે (curtab->tp_first_popupwin != ખાલી)
	જો (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    પાછા;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    પૂર્ણાંક i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    માટે (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    સ્થિર bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    પૂર્ણાંક i;

    જો (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	પાછા true;
    માટે (i = 0; i < 4; i++)
	જો (style->border_highlight[i] != wp->w_border_highlight[i])
	    પાછા true;
    પાછા false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    સ્થિર શૂન્ય
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
    સ્થિર bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    પાછા layout->winrow != wp->w_winrow
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
    સ્થિર bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    પાછા area->winrow != wp->w_winrow
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
    સ્થિર શૂન્ય
mark_overlapped_opacity_popup(win_T *wp, પૂર્ણાંક area_ટોચ, પૂર્ણાંક area_bot,
	પૂર્ણાંક area_ડાબું, પૂર્ણાંક area_જમણું, પૂર્ણાંક zindex)
{
    જો ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_ટોચ
	    || wp->w_wincol >= area_જમણું
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_ડાબું)
	પાછા;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_લીટીઓ_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    સ્થિર શૂન્ય
redraw_overlapped_opacity_popups(પૂર્ણાંક winrow, પૂર્ણાંક wincol, પૂર્ણાંક height, પૂર્ણાંક width,
	પૂર્ણાંક leftoff, પૂર્ણાંક zindex)
{
    win_T	*wp;
    પૂર્ણાંક		area_ટોચ = winrow;
    પૂર્ણાંક		area_bot = winrow + height;
    પૂર્ણાંક		area_ડાબું = wincol;
    પૂર્ણાંક		area_જમણું = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_ટોચ, area_bot, area_ડાબું,
		area_જમણું, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_ટોચ, area_bot, area_ડાબું,
		area_જમણું, zindex);

    જો (must_redraw < UPD_VALID)
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
    સ્થિર શૂન્ય
refill_pum_bg_row_from_buffer(પૂર્ણાંક r, પૂર્ણાંક ડાબું, પૂર્ણાંક જમણું)
{
    પૂર્ણાંક		લીટી_cp = r;
    પૂર્ણાંક		col_cp = ડાબું;
    win_T	*wp;
    linenr_T	lnum;
    પૂર્ણાંક		row_for_lnum;
    પૂર્ણાંક		col_for_lnum = 0;
    char_u	*લીટી;
    char_u	*p;
    પૂર્ણાંક		win_text_col;
    પૂર્ણાંક		screen_col;
    પૂર્ણાંક		soff_base = (r - pum_bg_ટોચ) * pum_bg_cols;
    પૂર્ણાંક		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    માટે (c = ડાબું; c < જમણું; ++c)
    {
	pum_bg_લીટીઓ[soff_base + c] = ' ';
	જો (pum_bg_attrs != ખાલી)
	    pum_bg_attrs[soff_base + c] = 0;
	જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી)
	    pum_bg_લીટીઓ_UC[soff_base + c] = 0;
	જો (enc_utf8)
	{
	    પૂર્ણાંક k;
	    માટે (k = 0; k < MAX_MCO; ++k)
		જો (pum_bg_લીટીઓ_C[k] != ખાલી)
		    pum_bg_લીટીઓ_C[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&લીટી_cp, &col_cp, IGNORE_POPUP);
    જો (wp == ખાલી || લીટી_cp < 0 || લીટી_cp >= wp->w_height)
	પાછા;
    જો (wp->w_buffer == ખાલી || wp->w_buffer->b_ml.ml_mfp == ખાલી)
	પાછા;

    // Compute the buffer line for this screen row.
    row_for_lnum = લીટી_cp;
    જો (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, ખાલી))
	પાછા;	// past end of buffer
    જો (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	પાછા;

    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
    જો (લીટી == ખાલી)
	પાછા;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    જો (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = લીટી;
    જ્યારે (*p != NUL && screen_col < જમણું)
    {
	પૂર્ણાંક char_cells;
	પૂર્ણાંક byte_ગણતરી;
	પૂર્ણાંક soff = soff_base + screen_col;
	પૂર્ણાંક in_range = (screen_col >= ડાબું && screen_col < જમણું);

	જો (*p == '\t')
	{
	    પૂર્ણાંક ts = (પૂર્ણાંક)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_ગણતરી = 1;
	    જો (in_range)
	    {
		pum_bg_લીટીઓ[soff] = ' ';
		જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી)
		    pum_bg_લીટીઓ_UC[soff] = 0;
	    }
	}
	નહીંતર જો (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_ગણતરી = mb_ptr2len(p);
	    જો (in_range)
	    {
		pum_bg_લીટીઓ[soff] = *p;
		જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી)
		    pum_bg_લીટીઓ_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	નહીંતર
	{
	    char_cells = 1;
	    byte_ગણતરી = 1;
	    જો (in_range)
	    {
		pum_bg_લીટીઓ[soff] = (*p < 0x20) ? ' ' : *p;
		જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી)
		    pum_bg_લીટીઓ_UC[soff] = 0;
	    }
	}
	જો (in_range && pum_bg_attrs != ખાલી)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_ગણતરી;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    સ્થિર શૂન્ય
popup_redraw_exposed_area(popup_area_T *area)
{
    જો (!area->active)
	પાછા;

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
    જો (pum_bg_લીટીઓ != ખાલી
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_ટોચ)
    {
	પૂર્ણાંક ટોચ = MAX(area->winrow, pum_bg_ટોચ);
	પૂર્ણાંક bot = MIN(area->winrow + area->height, pum_bg_bot);
	પૂર્ણાંક ડાબું = MAX(area->wincol, 0);
	પૂર્ણાંક જમણું = MIN(area->wincol + area->width, pum_bg_cols);
	પૂર્ણાંક r;

	માટે (r = ટોચ; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, ડાબું, જમણું);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    સ્થિર શૂન્ય
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->લીટીઓ);
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
    સ્થિર શૂન્ય
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    જો (screen_opacity_popup == ખાલી
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	પાછા;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    જો (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->લીટીઓ = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(પૂર્ણાંક,
				  saved_screen->rows * saved_screen->cols);
    જો (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    જો (saved_screen->લીટીઓ == ખાલી || saved_screen->attrs == ખાલી)
	પાછા;

    માટે (પૂર્ણાંક r = 0; r < saved_screen->rows; r++)
    {
	પૂર્ણાંક screen_row = saved_screen->start_row + r;

	જો (screen_row >= 0 && screen_row < screen_Rows)
	    માટે (પૂર્ણાંક c = 0; c < saved_screen->cols; c++)
	    {
		પૂર્ણાંક screen_col = saved_screen->start_col + c;

		જો (screen_col >= 0 && screen_col < screen_Columns)
		{
		    પૂર્ણાંક off = લીટી_Offset[screen_row] + screen_col;
		    પૂર્ણાંક save_off = r * saved_screen->cols + c;

		    saved_screen->લીટીઓ[save_off] = Screen_લીટીઓ[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    જો (enc_utf8 && saved_screen->linesuc != ખાલી)
			saved_screen->linesuc[save_off] = Screen_લીટીઓ_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    સ્થિર શૂન્ય
redraw_under_popup_area(પૂર્ણાંક winrow, પૂર્ણાંક wincol, પૂર્ણાંક height, પૂર્ણાંક width, પૂર્ણાંક leftoff)
{
    પૂર્ણાંક	    r;

    માટે (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	પૂર્ણાંક	    c;
	win_T	    *prev_twp = ખાલી;

	જો (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    ચાલુ;
	}

	માટે (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    પૂર્ણાંક	    લીટી_cp = r;
	    પૂર્ણાંક	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&લીટી_cp, &col_cp, IGNORE_POPUP);
	    જો (twp != ખાલી && twp != prev_twp)
	    {
		prev_twp = twp;
		જો (લીટી_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (શૂન્ય)mouse_comp_pos(twp, &લીટી_cp, &col_cp, &lnum, ખાલી);
		    redrawWinline(twp, lnum);
		}
		નહીંતર જો (લીટી_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    શૂન્ય
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    પૂર્ણાંક		id;
    win_T	*wp;
    popup_area_T	old_area;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;  // invalid {id}

    જો (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	પાછા;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    જો (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    જો (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    શૂન્ય
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    પૂર્ણાંક		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    પૂર્ણાંક		need_redraw = FALSE;
    પૂર્ણાંક		need_reposition = FALSE;

    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;  // invalid {id}

    જો (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	પાછા;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (શૂન્ય)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    જો (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    જો (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    નહીંતર જો (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    જો (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    જો (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    નહીંતર જો (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_લીટીઓ_valid = 0;
	જો (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    જો (old_style.blend != wp->w_popup_blend)
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

    જો (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    શૂન્ય
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    પૂર્ણાંક		id;
    win_T	*wp;
    પૂર્ણાંક		ટોચ_extra;
    પૂર્ણાંક		ડાબું_extra;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;  // invalid {id}
    ટોચ_extra = popup_ટોચ_extra(wp);
    ડાબું_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_સંખ્યા(dict, "line", wp->w_winrow + 1);
    dict_add_સંખ્યા(dict, "col", wp->w_wincol + 1);
    dict_add_સંખ્યા(dict, "width", wp->w_width + ડાબું_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_સંખ્યા(dict, "height", wp->w_height + ટોચ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_સંખ્યા(dict, "core_line", wp->w_winrow + 1 + ટોચ_extra);
    dict_add_સંખ્યા(dict, "core_col", wp->w_wincol + 1 + ડાબું_extra);
    dict_add_સંખ્યા(dict, "core_width", wp->w_width);
    dict_add_સંખ્યા(dict, "core_height", wp->w_height);

    dict_add_સંખ્યા(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_સંખ્યા(dict, "firstline", wp->w_topline);
    dict_add_સંખ્યા(dict, "lastline", wp->w_botline - 1);
    dict_add_સંખ્યા(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    શૂન્ય
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    FOR_ALL_POPUPWINS(wp)
	list_append_સંખ્યા(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_સંખ્યા(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    શૂન્ય
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    win_T	*wp;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    row = tv_get_સંખ્યા(&argvars[0]) - 1;
    col = tv_get_સંખ્યા(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    જો (wp != ખાલી && WIN_IS_POPUP(wp))
	rettv->vval.v_સંખ્યા = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    સ્થિર શૂન્ય
get_padding_border(dict_T *dict, પૂર્ણાંક *array, અક્ષર *name)
{
    list_T  *list;
    પૂર્ણાંક	    i;

    જો (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	પાછા;

    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    જો (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	પાછા;
    }
    જો (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	માટે (i = 0; i < 4; ++i)
	    list_append_સંખ્યા(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    સ્થિર શૂન્ય
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    પૂર્ણાંક	    i;

    માટે (i = 0; i < 4; ++i)
	જો (wp->w_border_highlight[i] != ખાલી)
	    તોડો;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    જો (i == 4 && !wp->w_border_highlight_isset)
	પાછા;

    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    જો (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	પાછા;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    જો (i == 4)
	પાછા;
    માટે (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    સ્થિર શૂન્ય
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    પૂર્ણાંક	    i;
    char_u  buf[NUMBUFLEN];
    પૂર્ણાંક	    len;

    માટે (i = 0; i < 8; ++i)
	જો (wp->w_border_char[i] != 0)
	    તોડો;
    જો (i == 8)
	પાછા;

    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    જો (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	પાછા;
    }
    માટે (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    સ્થિર શૂન્ય
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    જો (list != ખાલી)
    {
	જો (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	નહીંતર
	{
	    list_append_સંખ્યા(list, wp->w_popup_lnum);
	    list_append_સંખ્યા(list, wp->w_popup_mincol);
	    list_append_સંખ્યા(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    જો (list == ખાલી)
	પાછા;

    જો (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	પાછા;
    }
    list_append_સંખ્યા(list, wp->w_popup_mouse_row);
    list_append_સંખ્યા(list, wp->w_popup_mouse_mincol);
    list_append_સંખ્યા(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    શૂન્ય
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    પૂર્ણાંક		id;
    win_T	*wp;
    tabpage_T	*tp;
    પૂર્ણાંક		i;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = find_popup_win(id);
    જો (wp == ખાલી)
	પાછા;

    dict = rettv->vval.v_dict;
    dict_add_સંખ્યા(dict, "line", wp->w_wantline);
    dict_add_સંખ્યા(dict, "col", wp->w_wantcol);
    dict_add_સંખ્યા(dict, "minwidth", wp->w_minwidth);
    dict_add_સંખ્યા(dict, "minheight", wp->w_minheight);
    dict_add_સંખ્યા(dict, "maxheight", wp->w_maxheight);
    dict_add_સંખ્યા(dict, "maxwidth", wp->w_maxwidth);
    dict_add_સંખ્યા(dict, "firstline", wp->w_firstline);
    dict_add_સંખ્યા(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_સંખ્યા(dict, "zindex", wp->w_zindex);
    dict_add_સંખ્યા(dict, "fixed", wp->w_popup_fixed);
    જો (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	જો (pt != ખાલી)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_સંખ્યા(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_સંખ્યા(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    જો (wp->w_popup_imagep != ખાલી && wp->w_popup_imagep->img != ખાલી)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	જો (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_સંખ્યા(dict, "wrap", wp->w_p_wrap);
    dict_add_સંખ્યા(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_સંખ્યા(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_સંખ્યા(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_સંખ્યા(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_સંખ્યા(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_સંખ્યા(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_સંખ્યા(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_સંખ્યા(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    જો (wp->w_scrollbar_highlight != ખાલી)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    જો (wp->w_thumb_highlight != ખાલી)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    જો (twp->w_id == id)
		તોડો;
	જો (twp != ખાલી)
	    તોડો;
	++i;
    }
    જો (tp == ખાલી)
	i = -1;  // must be global
    નહીંતર જો (tp == curtab)
	i = 0;
    dict_add_સંખ્યા(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    જો (wp->w_filter_cb.cb_name != ખાલી)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    જો (wp->w_close_cb.cb_name != ખાલી)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(poppos_entries); ++i)
	જો (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(પૂર્ણાંક)poppos_entries[i].pp_name.length);
	    તોડો;
	}

    જો (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    નહીંતર જો (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    નહીંતર
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#જો defined(FEAT_TIMERS)
    dict_add_સંખ્યા(dict, "time", wp->w_popup_timer != ખાલી
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#જો defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    પૂર્ણાંક
error_if_term_popup_window(શૂન્ય)
{
    જો (WIN_IS_POPUP(curwin) && curbuf->b_term != ખાલી
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    શૂન્ય
popup_reset_handled(પૂર્ણાંક handled_flag)
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
find_next_popup(પૂર્ણાંક lowest, પૂર્ણાંક handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    પૂર્ણાંક	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = ખાલી;
    FOR_ALL_POPUPWINS(wp)
	જો ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    જો (found_wp != ખાલી)
	found_wp->w_popup_handled |= handled_flag;
    પાછા found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    સ્થિર પૂર્ણાંક
invoke_popup_filter(win_T *wp, પૂર્ણાંક c)
{
    પૂર્ણાંક		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    પૂર્ણાંક		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    જો (c == Ctrl_C)
    {
	પૂર્ણાંક save_got_int = got_int;
	પૂર્ણાંક was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	જો (was_curwin && wp == curwin)
	    પાછા -1;
	પાછા TRUE;
    }

    argv[0].v_type = VAR_સંખ્યા;
    argv[0].vval.v_સંખ્યા = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    જો (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    નહીંતર
    {
	જો (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	જો (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    જો (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	નહીંતર
	{
	    જો (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    પાછા res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    પૂર્ણાંક
popup_do_filter(પૂર્ણાંક c)
{
    સ્થિર પૂર્ણાંક	recursive = FALSE;
    પૂર્ણાંક		res = FALSE;
    win_T	*wp;
    પૂર્ણાંક		save_ચાવી_Typed = ચાવી_Typed;
    પૂર્ણાંક		state;
    પૂર્ણાંક		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    જો (popup_is_popup(curwin) && curbuf->b_term != ખાલી)
	પાછા FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    જો (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	પાછા FALSE;

    જો (recursive)
	પાછા FALSE;
    recursive = TRUE;

    જો (c == K_LEFTMOUSE)
    {
	પૂર્ણાંક row = mouse_row;
	પૂર્ણાંક col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	જો (wp != ખાલી && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    જ્યારે (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != ખાલી)
	જો (wp->w_filter_cb.cb_name != ખાલી
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    જો (must_redraw > was_must_redraw)
    {
	પૂર્ણાંક save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    ચાવી_Typed = save_ચાવી_Typed;

    // When interrupted return FALSE to avoid looping.
    પાછા res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    પૂર્ણાંક
popup_no_mapping(શૂન્ય)
{
    પૂર્ણાંક	    round;
    win_T   *wp;

    માટે (round = 1; round <= 2; ++round)
	માટે (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != ખાલી; wp = wp->w_next)
	    જો (wp->w_filter_cb.cb_name != ખાલી
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		પાછા TRUE;
    પાછા FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    શૂન્ય
popup_check_cursor_pos(શૂન્ય)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    જ્યારે ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != ખાલી)
	જો (wp->w_popup_curwin != ખાલી
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    સ્થિર શૂન્ય
popup_update_mask(win_T *wp, પૂર્ણાંક width, પૂર્ણાંક height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    પૂર્ણાંક		row, col;

    જો (wp->w_popup_mask == ખાલી || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	પાછા;
    }
    જો (wp->w_popup_mask_cells != ખાલી
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	પાછા;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    જો (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = ખાલી;
	પાછા;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    જો (wp->w_popup_mask_cells == ખાલી)
	પાછા;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_વસ્તુઓ(wp->w_popup_mask, lio)
    {
	પૂર્ણાંક cols, cole;
	પૂર્ણાંક લીટીઓ, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_સંખ્યા(&li->li_tv);
	જો (cols < 0)
	    cols = width + cols + 1;
	જો (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_સંખ્યા(&li->li_tv);
	જો (cole < 0)
	    cole = width + cole + 1;
	જો (cole > width)
	    cole = width;
	li = li->li_next;
	લીટીઓ = tv_get_સંખ્યા(&li->li_tv);
	જો (લીટીઓ < 0)
	    લીટીઓ = height + લીટીઓ + 1;
	જો (લીટીઓ <= 0)
	    લીટીઓ = 1;
	li = li->li_next;
	linee = tv_get_સંખ્યા(&li->li_tv);
	જો (linee < 0)
	    linee = height + linee + 1;
	જો (linee > height)
	    linee = height;

	માટે (row = લીટીઓ - 1; row < linee; ++row)
	    માટે (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    સ્થિર પૂર્ણાંક
popup_masked(win_T *wp, પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક screencol, પૂર્ણાંક screenline)
{
    પૂર્ણાંક col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    પૂર્ણાંક લીટી = screenline - wp->w_winrow;

    પાછા col >= 0 && col < width
	    && લીટી >= 0 && લીટી < height
	    && wp->w_popup_mask_cells[લીટી * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    સ્થિર શૂન્ય
update_popup_transparent(win_T *wp, પૂર્ણાંક val)
{
    જો (wp->w_popup_mask == ખાલી)
	પાછા;

    પૂર્ણાંક		width = popup_width(wp);
    પૂર્ણાંક		height = popup_height(wp);
    listitem_T	*lio, *li;
    પૂર્ણાંક		cols, cole;
    પૂર્ણાંક		લીટીઓ, linee;
    પૂર્ણાંક		col, લીટી;

    FOR_ALL_LIST_વસ્તુઓ(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_સંખ્યા(&li->li_tv);
	જો (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_સંખ્યા(&li->li_tv);
	જો (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	લીટીઓ = tv_get_સંખ્યા(&li->li_tv);
	જો (લીટીઓ < 0)
	    લીટીઓ = height + લીટીઓ + 1;
	li = li->li_next;
	linee = tv_get_સંખ્યા(&li->li_tv);
	જો (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	જો (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--લીટીઓ;
	જો (લીટીઓ < 0)
	    લીટીઓ = 0;
	માટે (લીટી = લીટીઓ; લીટી < linee
		&& લીટી + wp->w_winrow < screen_Rows; ++લીટી)
	    માટે (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(લીટી + wp->w_winrow) * screen_Columns
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
    સ્થિર પૂર્ણાંક
check_popup_unhidden(win_T *wp)
{
    જો (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	જો ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    પાછા FALSE;
	જો (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	નહીંતર જો (popup_find_prop_above_ટોચ(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	જો (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    પાછા TRUE;
	}
    }
    પાછા FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    સ્થિર પૂર્ણાંક
popup_need_position_adjust(win_T *wp)
{
    જો (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	પાછા TRUE;
    જો (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	પાછા TRUE;

    // May need to adjust the width if the cursor moved.
    પાછા wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
સ્થિર short *opacity_zindex = ખાલી;
સ્થિર પૂર્ણાંક    opacity_zindex_rows = 0;
સ્થિર પૂર્ણાંક    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    સ્થિર શૂન્ય
popup_mark_opacity_zindex(win_T *wp)
{
    પૂર્ણાંક	    width;
    પૂર્ણાંક	    height;
    પૂર્ણાંક	    r, c;

    જો (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	પાછા;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    માટે (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	માટે (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    પૂર્ણાંક off = r * screen_Columns + c;
	    જો (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    સ્થિર શૂન્ય
redraw_win_under_opacity_popup(win_T *wp)
{
    પૂર્ણાંક	    height;
    પૂર્ણાંક	    width;
    પૂર્ણાંક	    r;

    જો (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	પાછા;

    height = popup_height(wp);
    width = popup_width(wp);
    માટે (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	પૂર્ણાંક	    col;
	win_T	    *prev_twp = ખાલી;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	માટે (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    પૂર્ણાંક	    લીટી_cp = r;
	    પૂર્ણાંક	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&લીટી_cp, &col_cp, IGNORE_POPUP);
	    જો (twp != ખાલી && twp != prev_twp)
	    {
		prev_twp = twp;
		જો (લીટી_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    જો (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    નહીંતર
#endif
		    {
			(શૂન્ય)mouse_comp_pos(twp, &લીટી_cp, &col_cp, &lnum,
									ખાલી);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		નહીંતર જો (લીટી_cp == twp->w_height)
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
    પૂર્ણાંક
popup_is_under_opacity(પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (opacity_zindex == ખાલી
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	પાછા FALSE;
    પાછા opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    પૂર્ણાંક
popup_is_over_opacity(પૂર્ણાંક row, પૂર્ણાંક col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	જો ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    પાછા TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    પૂર્ણાંક
popup_is_under_opacity_range(પૂર્ણાંક row, પૂર્ણાંક start_col, પૂર્ણાંક end_col)
{
    પૂર્ણાંક col;

    જો (opacity_zindex == ખાલી
	    || row < 0 || row >= opacity_zindex_rows)
	પાછા FALSE;
    માટે (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	જો (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    શૂન્ય
may_update_popup_mask(પૂર્ણાંક type)
{
    win_T	*wp;
    short	*mask;
    પૂર્ણાંક		લીટી, col;
    પૂર્ણાંક		redraw_all_popups = FALSE;
    પૂર્ણાંક		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    જો (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	નહીંતર જો (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	નહીંતર જો (popup_need_position_adjust(wp))
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
	પૂર્ણાંક has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    જો ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    જો ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	જો (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	નહીંતર
	{
	    જો (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    જો (opacity_zindex != ખાલી)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * કદ(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    જો (!popup_mask_refresh)
	પાછા;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	જો (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    જો (redrawing_all_win)
	mask = popup_mask;
    નહીંતર
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * કદ(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    જ્યારે ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != ખાલી)
    {
	પૂર્ણાંક width;
	પૂર્ણાંક height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	જો (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    જો (wp->w_popup_flags & POPF_HIDDEN)
		ચાલુ;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	જો (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	નહીંતર
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	જો ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ચાલુ;

	{
	    પૂર્ણાંક mask_start = wp->w_winrow;
	    પૂર્ણાંક mask_end = mask_start + height;
	    પૂર્ણાંક mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    પૂર્ણાંક mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    માટે (લીટી = mask_start;
			    લીટી < mask_end && લીટી < screen_Rows; ++લીટી)
		માટે (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    જો (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(લીટી, col, FALSE))
			mask[લીટી * screen_Columns + col] = POPUPMENU_ZINDEX;
		    નહીંતર જો (wp->w_popup_mask_cells == ખાલી
				    || !popup_masked(wp, width, height, col, લીટી))
			mask[લીટી * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    જો (mask == popup_mask_next)
    {
	પૂર્ણાંક	    *plines_cache = ALLOC_CLEAR_MULT(પૂર્ણાંક, Rows);
	win_T	    *prev_wp = ખાલી;

	માટે (લીટી = 0; લીટી < screen_Rows; ++લીટી)
	{
	    પૂર્ણાંક	    col_done = 0;

	    માટે (col = 0; col < screen_Columns; ++col)
	    {
		પૂર્ણાંક off = લીટી * screen_Columns + col;

		જો (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    જો (લીટી >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			જો (!સંદેશ_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    નહીંતર જો (col >= col_done)
		    {
			linenr_T	lnum;
			પૂર્ણાંક		લીટી_cp = લીટી;
			પૂર્ણાંક		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&લીટી_cp, &col_cp, IGNORE_POPUP);
			જો (wp != ખાલી)
			{
#જો defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    જો (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    નહીંતર
#endif
			    {
				જો (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   કદ(પૂર્ણાંક) * Rows);
				    prev_wp = wp;
				}

				જો (લીટી_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				નહીંતર
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (શૂન્ય)mouse_comp_pos(wp, &લીટી_cp, &col_cp,
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
    શૂન્ય
may_update_popup_position(શૂન્ય)
{
    જો (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
સ્થિર schar_T *base_screenlines = ખાલી;
સ્થિર પૂર્ણાંક *base_screenattrs = ખાલી;
સ્થિર u8char_T *base_screenlinesuc = ખાલી;
સ્થિર પૂર્ણાંક base_screen_rows = 0;
સ્થિર પૂર્ણાંક base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    પૂર્ણાંક
popup_get_base_screen_cell(પૂર્ણાંક row, પૂર્ણાંક col, schar_T *linep, પૂર્ણાંક *attrp,
							  u8char_T *ucp)
{
    જો (base_screenlines == ખાલી || base_screenattrs == ખાલી)
	પાછા FALSE;
    જો (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	પાછા FALSE;

    પૂર્ણાંક off = row * base_screen_cols + col;
    જો (linep != ખાલી)
	*linep = base_screenlines[off];
    જો (attrp != ખાલી)
	*attrp = base_screenattrs[off];
    જો (ucp != ખાલી)
    {
	જો (enc_utf8 && base_screenlinesuc != ખાલી)
	    *ucp = base_screenlinesuc[off];
	નહીંતર
	    *ucp = 0;
    }
    પાછા TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    શૂન્ય
popup_set_base_screen_cell(પૂર્ણાંક row, પૂર્ણાંક col, schar_T લીટી, પૂર્ણાંક attr, u8char_T uc)
{
    જો (base_screenlines == ખાલી || base_screenattrs == ખાલી)
	પાછા;
    જો (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	પાછા;

    પૂર્ણાંક off = row * base_screen_cols + col;
    base_screenlines[off] = લીટી;
    base_screenattrs[off] = attr;
    જો (enc_utf8 && base_screenlinesuc != ખાલી)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
સ્થિર શૂન્ય
draw_opacity_padding_cell(
	પૂર્ણાંક		row,
	પૂર્ણાંક		col,
	popup_saved_screen_T *saved_screen,
	પૂર્ણાંક		pad_start_col,
	પૂર્ણાંક		pad_end_col)
{
    જો (લીટી_Offset == ખાલી || Screen_લીટીઓ == ખાલી || ScreenAttrs == ખાલી
	    || (enc_utf8 && Screen_લીટીઓ_UC == ખાલી))
	પાછા;

    પૂર્ણાંક off = લીટી_Offset[row] + col;
    પૂર્ણાંક r = row - saved_screen->start_row;
    પૂર્ણાંક c = col - saved_screen->start_col;

    જો (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	પૂર્ણાંક save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	જો (enc_utf8 && saved_screen->linesuc != ખાલી)
	{
	    પૂર્ણાંક base_col = col - 1;
	    પૂર્ણાંક base_off = off - 1;
	    પૂર્ણાંક base_save_off = save_off - 1;
	    પૂર્ણાંક wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    જો (base_off >= 0)
	    {
		જો (Screen_લીટીઓ_UC[base_off] != 0
			&& utf_char2cells(Screen_લીટીઓ_UC[base_off]) == 2
			&& Screen_લીટીઓ[off] == 0)
		    wide_prev = TRUE;
	    }
	    જો (!wide_prev && save_off > 0)
	    {
		જો (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->લીટીઓ[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    જો (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		જો (base_col < pad_start_col)
		{
		    જો (Screen_લીટીઓ_UC[base_off] != 0
			    && utf_char2cells(Screen_લીટીઓ_UC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_લીટીઓ[base_off] = ' ';
			Screen_લીટીઓ_UC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				Screen_લીટીઓ[base_off],
				ScreenAttrs[base_off],
				Screen_લીટીઓ_UC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_લીટીઓ[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			જો (enc_utf8)
			    Screen_લીટીઓ_UC[off] = 0;
			પૂર્ણાંક popup_attr_val =
					get_win_attr(screen_opacity_popup);
			પૂર્ણાંક blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				Screen_લીટીઓ[off], ScreenAttrs[off],
				Screen_લીટીઓ_UC[off]);
			screen_char(off, row, col);
			પાછા;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    જો (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && Screen_લીટીઓ[base_off] == ' '
			    && Screen_લીટીઓ_UC[base_off] == 0)
		    {
			પૂર્ણાંક popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			પૂર્ણાંક blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_લીટીઓ[base_off] =
				    saved_screen->લીટીઓ[base_save_off];
			Screen_લીટીઓ_UC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			Screen_લીટીઓ[off] = 0;
			Screen_લીટીઓ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    Screen_લીટીઓ[base_off],
				    ScreenAttrs[base_off],
				    Screen_લીટીઓ_UC[base_off]);
			popup_set_base_screen_cell(row, col,
				    Screen_લીટીઓ[off],
				    ScreenAttrs[off],
				    Screen_લીટીઓ_UC[off]);
			screen_char(base_off, row, base_col);
			પાછા;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_લીટીઓ[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    જો (enc_utf8)
			Screen_લીટીઓ_UC[off] = 0;
		    પૂર્ણાંક popup_attr_val = get_win_attr(screen_opacity_popup);
		    પૂર્ણાંક blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, Screen_લીટીઓ[off],
					       ScreenAttrs[off], Screen_લીટીઓ_UC[off]);
		    screen_char(off, row, col);
		    પાછા;
		}

		// Base cell is inside the saved area, redraw the wide char.
		જો (save_off > 0)
		{
		    Screen_લીટીઓ[base_off] = saved_screen->લીટીઓ[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    Screen_લીટીઓ[off] = saved_screen->લીટીઓ[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_લીટીઓ_UC[base_off] = saved_screen->linesuc[base_save_off];
		    Screen_લીટીઓ_UC[off] = saved_screen->linesuc[save_off];

		    પૂર્ણાંક popup_attr_val = get_win_attr(screen_opacity_popup);
		    પૂર્ણાંક blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, Screen_લીટીઓ[base_off],
					       ScreenAttrs[base_off], Screen_લીટીઓ_UC[base_off]);
		    popup_set_base_screen_cell(row, col, Screen_લીટીઓ[off],
					       ScreenAttrs[off], Screen_લીટીઓ_UC[off]);
		    screen_char(base_off, row, base_col);
		}
		પાછા;
	    }
	}
	Screen_લીટીઓ[off] = saved_screen->લીટીઓ[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	જો (enc_utf8)
	    Screen_લીટીઓ_UC[off] = saved_screen->linesuc != ખાલી
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0
		&& utf_char2cells(Screen_લીટીઓ_UC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    Screen_લીટીઓ[off] = ' ';
	    Screen_લીટીઓ_UC[off] = 0;
	}

	પૂર્ણાંક popup_attr_val = get_win_attr(screen_opacity_popup);
	પૂર્ણાંક blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, Screen_લીટીઓ[off],
				   ScreenAttrs[off], Screen_લીટીઓ_UC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
સ્થિર શૂન્ય
fill_opacity_padding(
	પૂર્ણાંક		start_row,
	પૂર્ણાંક		end_row,
	પૂર્ણાંક		start_col,
	પૂર્ણાંક		end_col,
	popup_saved_screen_T *saved_screen)
{
    માટે (પૂર્ણાંક pad_row = start_row; pad_row < end_row; pad_row++)
	માટે (પૂર્ણાંક pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    શૂન્ય
update_popups(શૂન્ય (*win_update)(win_T *wp))
{
    win_T   *wp;
    પૂર્ણાંક	    ટોચ_off;
    પૂર્ણાંક	    ડાબું_extra;
    પૂર્ણાંક	    total_width;
    પૂર્ણાંક	    total_height;
    પૂર્ણાંક	    ટોચ_padding;
    પૂર્ણાંક	    popup_attr;
    પૂર્ણાંક	    border_attr[4];
    પૂર્ણાંક	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    પૂર્ણાંક	    row;
    પૂર્ણાંક	    wincol;
    પૂર્ણાંક	    padcol = 0;
    પૂર્ણાંક	    padendcol = 0;
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    sb_thumb_ટોચ = 0;
    પૂર્ણાંક	    sb_thumb_height = 0;
    પૂર્ણાંક	    attr_scroll = 0;
    પૂર્ણાંક	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    જો (base_screenlines != ખાલી)
    {
	vim_free(base_screenlines);
	base_screenlines = ખાલી;
    }
    જો (base_screenattrs != ખાલી)
    {
	vim_free(base_screenattrs);
	base_screenattrs = ખાલી;
    }
    જો (base_screenlinesuc != ખાલી)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = ખાલી;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    જ્યારે ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != ખાલી)
    {
	પૂર્ણાંક	    title_len = 0;
	પૂર્ણાંક	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	જો ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	નહીંતર
	    screen_opacity_popup = ખાલી;

#ifdef FEAT_PROP_POPUP
	જો (screen_opacity_popup != ખાલી)
	{
	    જો (base_screenlines != ખાલી)
	    {
		vim_free(base_screenlines);
		base_screenlines = ખાલી;
	    }
	    જો (base_screenattrs != ખાલી)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = ખાલી;
	    }
	    જો (base_screenlinesuc != ખાલી)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = ખાલી;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(પૂર્ણાંક,
				    base_screen_rows * base_screen_cols);
	    જો (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    જો (base_screenlines != ખાલી && base_screenattrs != ખાલી)
	    {
		માટે (પૂર્ણાંક r = 0; r < base_screen_rows; r++)
		{
		    પૂર્ણાંક off = લીટી_Offset[r];
		    પૂર્ણાંક base_off = r * base_screen_cols;
		    માટે (પૂર્ણાંક c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = Screen_લીટીઓ[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			જો (enc_utf8 && base_screenlinesuc != ખાલી)
			    base_screenlinesuc[base_off + c] =
					Screen_લીટીઓ_UC[off + c];
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
	ટોચ_off = popup_ટોચ_extra(wp);
	ડાબું_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	જો (wp->w_wincol + ડાબું_extra < 0)
	    ડાબું_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(ટોચ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ડાબું_extra;

	// Draw the popup text, unless it's off screen.
	જો (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    જો (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    જો (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    નહીંતર જો (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(ટોચ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ડાબું_extra;

	// Add offset for border and padding if not done already.
	જો ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ડાબું_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	જો ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += ટોચ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	જો (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	નહીંતર
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	જો (total_width < 0)
	    total_width = 0;
	જો (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	નહીંતર
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	જો (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	નહીંતર
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	જો (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	નહીંતર
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    માટે (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    જો (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	માટે (i = 0; i < 8; ++i)
	    જો (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	માટે (i = 0; i < 4; ++i)
	{
	    જો (wp->w_border_highlight[i] != ખાલી)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    નહીંતર જો (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    નહીંતર
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    જો ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	જો ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	જો (wp->w_popup_title != ખાલી)
	{
	    પૂર્ણાંક	    title_attr;

	    જો (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ખાલી)
		title_attr = border_attr[0];
	    નહીંતર જો (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    નહીંતર
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    જો (title_len > total_width - 2)
	    {
		પૂર્ણાંક	title_byte_len = (પૂર્ણાંક)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		જો (title_text != ખાલી)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    નહીંતર
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	ટોચ_padding = cl.eff_padding[0];
	જો (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    જો (title_len > 0)
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
	    નહીંતર
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    જો (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	નહીંતર જો (cl.eff_padding[0] == 0 && popup_ટોચ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    ટોચ_padding = 1;

	જો (ટોચ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    જો (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	જો (ટોચ_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    જો (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		જો (screen_opacity_popup != ખાલી && saved_screen.લીટીઓ != ખાલી)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		નહીંતર
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		ટોચ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    જો (screen_opacity_popup != ખાલી && saved_screen.લીટીઓ != ખાલી)
	    {
		fill_opacity_padding(row, row + ટોચ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    નહીંતર
	    {
		screen_fill(row, row + ટોચ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	જો (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	    પૂર્ણાંક		height = wp->w_height;
	    પૂર્ણાંક		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    જો (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    જો (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    જો (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_ટોચ = 0;
	    નહીંતર
		sb_thumb_ટોચ = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    જો (wp->w_topline > 1 && sb_thumb_ટોચ == 0 && height > 1)
		sb_thumb_ટોચ = 1;  // show it's scrolled
	    last = total_height - ટોચ_off - wp->w_popup_border[2];
	    જો (sb_thumb_ટોચ >= last)
		// show at least one character
		sb_thumb_ટોચ = last - 1;

	    જો (wp->w_scrollbar_highlight != ખાલી)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    નહીંતર
		attr_scroll = highlight_attr[HLF_PSB];
	    જો (wp->w_thumb_highlight != ખાલી)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    નહીંતર
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	માટે (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    પૂર્ણાંક	pad_ડાબું;
	    // left and right padding only needed next to the body
	    પૂર્ણાંક do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    જો (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    જો (do_padding && cl.eff_padding[3] > 0)
	    {
		પૂર્ણાંક col = wincol + wp->w_popup_border[3];

		// left padding
		pad_ડાબું = wp->w_popup_padding[3];
		જો (col < 0)
		{
		    pad_ડાબું += col;
		    col = 0;
		}
		જો (pad_ડાબું > 0)
		{
		    જો (screen_opacity_popup != ખાલી && saved_screen.લીટીઓ != ખાલી)
			fill_opacity_padding(row, row + 1, col, col + pad_ડાબું,
				&saved_screen);
		    નહીંતર
			screen_fill(row, row + 1, col, col + pad_ડાબું,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    જો (wp->w_has_scrollbar)
	    {
		પૂર્ણાંક લીટી = i - ટોચ_off;
		પૂર્ણાંક scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		જો (લીટી >= 0 && લીટી < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    લીટી >= sb_thumb_ટોચ
				       && લીટી < sb_thumb_ટોચ + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		નહીંતર
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    જો (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    જો (do_padding && cl.eff_padding[1] > 0)
	    {
		પૂર્ણાંક pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		પૂર્ણાંક pad_col_end = pad_col_start + wp->w_popup_padding[1];

		જો (screen_opacity_popup != ખાલી && saved_screen.લીટીઓ != ખાલી)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		નહીંતર
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	જો (wp->w_popup_shadow)
	{
	    પૂર્ણાંક col = wincol + total_width;
	    માટે (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	જો (cl.eff_padding[2] > 0)
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
	    જો (screen_opacity_popup != ખાલી && saved_screen.લીટીઓ != ખાલી)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    નહીંતર
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	જો (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    જો (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	જો (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    માટે (પૂર્ણાંક col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	જો (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ખાલી;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	જો (override_success)
	    કાઢો_highlight_overrides();

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

#જો defined(FEAT_શોધ_EXTRA)
    // In case win_update() called start_search_hl().
    end_શોધ_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    સ્થિર પૂર્ણાંક
set_ref_in_one_popup(win_T *wp, પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    typval_T	tv;

    જો (wp->w_close_cb.cb_partial != ખાલી)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
    }
    જો (wp->w_filter_cb.cb_partial != ખાલી)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    પાછા abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    પૂર્ણાંક
set_ref_in_popups(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    માટે (wp = first_popupwin; !abort && wp != ખાલી; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	માટે (wp = tp->tp_first_popupwin; !abort && wp != ખાલી; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	જો (abort)
	    તોડો;
    }
    પાછા abort;
}

    પૂર્ણાંક
popup_is_popup(win_T *wp)
{
    પાછા wp->w_popup_flags != 0;
}

#જો defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(શૂન્ય)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_p_pvw)
	    પાછા wp;
    પાછા ખાલી;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(શૂન્ય)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_popup_flags & POPF_INFO)
	    પાછા wp;
    પાછા ખાલી;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(પૂર્ણાંક dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    પૂર્ણાંક		by;
    linenr_T	new_topline;

    જો (wp == ખાલી)
	પાછા false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    જો (new_topline < 1)
	new_topline = 1;
    જો (new_topline > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	new_topline = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
    જો (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    પાછા true;
#નહીંતર
    પાછા false;
#endif
}

    શૂન્ય
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_સંખ્યા = message_win == ખાલી ? 0 : message_win->w_id;
#નહીંતર
    rettv->vval.v_સંખ્યા = 0;
#endif
}

    શૂન્ય
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_સંખ્યા = wp == ખાલી ? 0 : wp->w_id;
#નહીંતર
    rettv->vval.v_સંખ્યા = 0;
#endif
}

    શૂન્ય
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_સંખ્યા = wp == ખાલી ? 0 : wp->w_id;
#નહીંતર
    rettv->vval.v_સંખ્યા = 0;
#endif
}

#જો defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    પૂર્ણાંક
popup_create_preview_window(પૂર્ણાંક info)
{
    win_T *wp = popup_create(ખાલી, ખાલી, info ? TYPE_INFO : TYPE_PREVIEW);

    જો (wp == ખાલી)
	પાછા FAIL;
    જો (info)
	wp->w_popup_flags |= POPF_INFO;
    નહીંતર
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    જો (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    નહીંતર જો (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    નહીંતર
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    પાછા OK;
}

/*
 * Close any preview popup.
 */
    શૂન્ય
popup_close_preview(શૂન્ય)
{
    win_T *wp = popup_find_preview_window();

    જો (wp != ખાલી)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    શૂન્ય
popup_hide_info(શૂન્ય)
{
    win_T *wp = popup_find_info_window();

    જો (wp != ખાલી)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	જો (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    શૂન્ય
popup_close_info(શૂન્ય)
{
    win_T *wp = popup_find_info_window();

    જો (wp != ખાલી)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    પૂર્ણાંક
popup_overlaps_cmdline(શૂન્ય)
{
    પાછા popup_on_cmdline;
}

#જો defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(શૂન્ય)
{
    જો (message_win != ખાલી)
	પાછા message_win;

    પૂર્ણાંક i;

    message_win = popup_create(ખાલી, ખાલી, TYPE_MESSAGE_WIN);

    જો (message_win == ખાલી)
	પાછા ખાલી;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    માટે (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    માટે (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    જો (message_win->w_popup_timer != ખાલી)
	message_win->w_popup_timer->tr_keep = TRUE;
    પાછા message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    શૂન્ય
popup_show_message_win(શૂન્ય)
{
    જો (message_win == ખાલી)
	પાછા;

    જો ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    સ્થિર શૂન્ય
may_start_message_win_timer(win_T *wp)
{
    જો (wp == message_win && start_message_win_timer)
    {
	જો (message_win->w_popup_timer != ખાલી)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    પૂર્ણાંક
popup_message_win_visible(શૂન્ય)
{
    પાછા message_win != ખાલી
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    શૂન્ય
popup_hide_message_win(શૂન્ય)
{
    જો (message_win != ખાલી)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
સ્થિર પૂર્ણાંક save_સંદેશ_didout = FALSE;
સ્થિર પૂર્ણાંક save_સંદેશ_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
સ્થિર પૂર્ણાંક ew_સંદેશ_didout = FALSE;
સ્થિર પૂર્ણાંક ew_સંદેશ_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    શૂન્ય
start_echowindow(પૂર્ણાંક time_sec)
{
    in_echowindow = TRUE;
    save_સંદેશ_didout = સંદેશ_didout;
    save_સંદેશ_col = સંદેશ_col;
    સંદેશ_didout = ew_સંદેશ_didout;
    સંદેશ_col = ew_સંદેશ_col;
    જો (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    શૂન્ય
end_echowindow(શૂન્ય)
{
    in_echowindow = FALSE;

    જો ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_સંદેશ_didout = TRUE;
    ew_સંદેશ_col = સંદેશ_col == 0 ? 1 : સંદેશ_col;
    સંદેશ_didout = save_સંદેશ_didout;
    સંદેશ_col = save_સંદેશ_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    પૂર્ણાંક
popup_win_closed(win_T *win)
{
    પૂર્ણાંક	    round;
    win_T   *wp;
    win_T   *next;
    પૂર્ણાંક	    ret = FALSE;

    માટે (round = 1; round <= 2; ++round)
	માટે (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != ખાલી; wp = next)
	{
	    next = wp->w_next;
	    જો (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    પાછા ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    શૂન્ય
popup_set_title(win_T *wp)
{
    જો (wp->w_buffer->b_fname == ખાલી)
	પાછા;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    જો (wp->w_popup_title != ખાલી)
	vim_snprintf((અક્ષર *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#જો defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    શૂન્ય
popup_update_preview_title(શૂન્ય)
{
    win_T *wp = popup_find_preview_window();

    જો (wp != ખાલી)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
