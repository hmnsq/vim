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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_PROP_POPUP)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_ਇੰਦਰਾਜ_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		ਸਰਗਰਮ;
    ਪੂਰਨ		winrow;
    ਪੂਰਨ		wincol;
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		leftoff;
    ਪੂਰਨ		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    schar_T	*ਸਤਰਾਂ;
    ਪੂਰਨ		*attrs;
    u8char_T	*linesuc;
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਪੂਰਨ		ਪੰਕਤੀਆਂ_2;
    ਪੂਰਨ		ਖਾਨੇ_ਗਿਣਤੀ;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    linenr_T	firstline;
    ਪੂਰਨ		blend;
    ਪੂਰਨ		flags;
    ਪੂਰਨ		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		winrow;
    ਪੂਰਨ		wincol;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		leftcol;
    ਪੂਰਨ		leftoff;
    ਪੂਰਨ		has_scrollbar;
    ਪੂਰਨ		topoff;
    ਪੂਰਨ		bottomoff;
    ਪੂਰਨ		leftclip;
    ਪੂਰਨ		rightclip;
} popup_layout_T;

ਸਥਿਰ poppos_ਇੰਦਰਾਜ_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
// Window used for ":echowindow"
ਸਥਿਰ win_T *ਸੁਨੇਹਾ_win_2 = ਨਿਰਾ;

// Time used for the next ":echowindow" message in msec.
ਸਥਿਰ ਪੂਰਨ  ਸੁਨੇਹਾ_win_ਸਮਾਂ = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ਸਥਿਰ ਪੂਰਨ    ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer = FALSE;

ਸਥਿਰ ਖਾਲੀ may_ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer(win_T *wp);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ popup_on_cmdline = FALSE;

ਸਥਿਰ ਖਾਲੀ popup_adjust_position(win_T *wp);
ਸਥਿਰ bool popup_area_ਬਦਲਿਆ(win_T *wp, popup_area_T *area);
ਸਥਿਰ ਖਾਲੀ popup_redraw_exposed_area(popup_area_T *area);
ਸਥਿਰ ਖਾਲੀ popup_save_area(win_T *wp, popup_area_T *area);
ਸਥਿਰ ਖਾਲੀ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ਸਥਿਰ ਖਾਲੀ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ਸਥਿਰ bool popup_layout_ਬਦਲਿਆ(win_T *wp, popup_layout_T *layout);
ਸਥਿਰ bool popup_style_ਬਦਲਿਆ(win_T *wp, popup_style_snapshot_T *style);
ਸਥਿਰ ਖਾਲੀ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ਸਥਿਰ ਖਾਲੀ popup_save_layout(win_T *wp, popup_layout_T *layout);
ਸਥਿਰ ਖਾਲੀ redraw_under_popup_area(ਪੂਰਨ winrow, ਪੂਰਨ wincol, ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ leftoff);
ਸਥਿਰ ਖਾਲੀ redraw_overlapped_opacity_popups(ਪੂਰਨ winrow, ਪੂਰਨ wincol,
	ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ leftoff, ਪੂਰਨ zindex);
ਸਥਿਰ ਖਾਲੀ redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_options_one(dict_T *dict, char_u *ਕੁੰਜੀ)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    ਪੂਰਨ		n = 0;

    di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ MAXCOL;

    val = tv_get_string(&di->di_tv);
    ਜੇ (STRNCMP(val, "cursor", 6) != 0)
	ਵਾਪਸ dict_get_ਸੰਖਿਆ_check(dict, ਕੁੰਜੀ);

    setcursor_mayforce(TRUE);
    s = val + 6;
    ਜੇ (*s != NUL)
    {
	endp = s;
	ਜੇ (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((ਅੱਖਰ *)s, (ਅੱਖਰ **)&endp, 10);
	ਜੇ (endp != ਨਿਰਾ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), val);
	    ਵਾਪਸ 0;
	}
    }

    ਜੇ (STRCMP(ਕੁੰਜੀ, "line") == 0)
	n = screen_screenrow() + 1 + n;
    ਨਹੀਂਤਾਂ // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    ਜੇ (n == 0)
	n = -1;
    ਵਾਪਸ n;
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_padding_border(dict_T *dict, ਪੂਰਨ *array, ਅੱਖਰ *name, ਪੂਰਨ max_val)
{
    dictitem_T	*di;

    di = dict_ਲੱਭੋ(dict, (char_u *)name, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਜੇ (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	ਵਾਪਸ FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    ਪੂਰਨ		i;
    ਪੂਰਨ		nr;

    ਲਈ (i = 0; i < 4; ++i)
	array[i] = 1;
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ OK;

    CHECK_LIST_MATERIALIZE(list);
    ਲਈ (i = 0, li = list->lv_ਪਹਿਲਾ; i < 4 && i < list->lv_len;
	    ++i, li = li->li_ਅਗਲਾ)
    {
	nr = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    ਵਾਪਸ OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_moved_ਮੁੱਲਾਂ(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.ਖਾਨਾ;
    wp->w_popup_maxcol = curwin->w_cursor.ਖਾਨਾ;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_moved_columns(win_T *wp, ਪੂਰਨ flags)
{
    char_u	*ptr;
    ਪੂਰਨ		len = ਲੱਭੋ_ident_under_cursor(&ptr, flags | ਲੱਭੋ_NOERROR);
    ਪੂਰਨ		mincol;

    ਜੇ (len <= 0)
	ਵਾਪਸ;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (ਪੂਰਨ)(ptr - ml_get_curline());
    ਜੇ (curwin->w_cursor.ਖਾਨਾ < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.ਖਾਨਾ;
	wp->w_popup_maxcol = curwin->w_cursor.ਖਾਨਾ;
	ਵਾਪਸ;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_mousemoved_ਮੁੱਲਾਂ(win_T *wp)
{
    wp->w_popup_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    wp->w_popup_mouse_mincol = mouse_ਖਾਨਾ;
    wp->w_popup_mouse_maxcol = mouse_ਖਾਨਾ;
}

    ਸਥਿਰ ਖਾਲੀ
update_popup_ਵਰਤਦਾ_mouse_move(ਖਾਲੀ)
{
    popup_ਵਰਤਦਾ_mouse_move = FALSE;
    ਜੇ (!popup_visible)
	ਵਾਪਸ;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_popup_mouse_ਪੰਕਤੀ != 0)
	{
	    popup_ਵਰਤਦਾ_mouse_move = TRUE;
	    ਵਾਪਸ;
	}
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_popup_mouse_ਪੰਕਤੀ != 0)
	{
	    popup_ਵਰਤਦਾ_mouse_move = TRUE;
	    ਵਾਪਸ;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_mousemoved_columns(win_T *wp, ਪੂਰਨ flags)
{
    win_T	*textwp;
    char_u	*ਪਾਠ;
    ਪੂਰਨ		ਖਾਨਾ;
    pos_T	pos;
    colnr_T	mcol;

    ਜੇ (ਲੱਭੋ_ਸ਼ਬਦ_under_cursor(mouse_ਪੰਕਤੀ, mouse_ਖਾਨਾ, TRUE, flags,
				  &textwp, &pos.lnum, &ਪਾਠ, ਨਿਰਾ, &ਖਾਨਾ) != OK)
	ਵਾਪਸ;

    // convert text column to mouse column
    pos.ਖਾਨਾ = ਖਾਨਾ;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ਨਿਰਾ, ਨਿਰਾ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.ਖਾਨਾ = ਖਾਨਾ + (colnr_T)STRLEN(ਪਾਠ) - 1;
    getvcol(textwp, &pos, ਨਿਰਾ, ਨਿਰਾ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(ਪਾਠ);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    ਪੂਰਨ
popup_on_border(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਵਾਪਸ (ਪੰਕਤੀ == 0 && wp->w_popup_border[0] > 0)
	    || (ਪੰਕਤੀ == popup_ਉਚਾਈ(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (ਖਾਨਾ == 0 && wp->w_popup_border[3] > 0)
	    || (ਖਾਨਾ == popup_ਚੌੜਾਈ(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    ਪੂਰਨ
popup_close_if_on_X(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (wp->w_popup_close == POPCLOSE_BUTTON
	    && ਪੰਕਤੀ == 0 && ਖਾਨਾ == popup_ਚੌੜਾਈ(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

// Values set when dragging a popup window starts.
ਸਥਿਰ ਪੂਰਨ drag_ਸ਼ੁਰੂ_ਪੰਕਤੀ;
ਸਥਿਰ ਪੂਰਨ drag_ਸ਼ੁਰੂ_ਖਾਨਾ;
ਸਥਿਰ ਪੂਰਨ drag_ਸ਼ੁਰੂ_wantline;
ਸਥਿਰ ਪੂਰਨ drag_ਸ਼ੁਰੂ_wantcol;
ਸਥਿਰ ਪੂਰਨ drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    ਖਾਲੀ
popup_ਸ਼ੁਰੂ_drag(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    drag_ਸ਼ੁਰੂ_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    drag_ਸ਼ੁਰੂ_ਖਾਨਾ = mouse_ਖਾਨਾ;
    ਜੇ (wp->w_wantline <= 0)
	drag_ਸ਼ੁਰੂ_wantline = wp->w_winrow + 1;
    ਨਹੀਂਤਾਂ
	drag_ਸ਼ੁਰੂ_wantline = wp->w_wantline;
    ਜੇ (wp->w_wantcol == 0)
	drag_ਸ਼ੁਰੂ_wantcol = wp->w_wincol + 1;
    ਨਹੀਂਤਾਂ
	drag_ਸ਼ੁਰੂ_wantcol = wp->w_wantcol;

    // Stop centering the popup
    ਜੇ (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && ਪੰਕਤੀ == popup_ਉਚਾਈ(wp) - 1
			    && ਖਾਨਾ == popup_ਚੌੜਾਈ(wp) - 1;

    ਜੇ (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	ਜੇ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	ਜੇ (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    ਖਾਲੀ
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    ਜੇ (!win_valid_popup(wp))
	ਵਾਪਸ;

    ਜੇ ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	ਪੂਰਨ ਚੌੜਾਈ_inc = mouse_ਖਾਨਾ - drag_ਸ਼ੁਰੂ_ਖਾਨਾ;
	ਪੂਰਨ ਉਚਾਈ_inc = mouse_ਪੰਕਤੀ - drag_ਸ਼ੁਰੂ_ਪੰਕਤੀ;

	ਜੇ (ਚੌੜਾਈ_inc != 0)
	{
	    ਪੂਰਨ ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ + ਚੌੜਾਈ_inc;

	    ਜੇ (ਚੌੜਾਈ < 1)
		ਚੌੜਾਈ = 1;
	    wp->w_minwidth = ਚੌੜਾਈ;
	    wp->w_maxwidth = ਚੌੜਾਈ;
	    drag_ਸ਼ੁਰੂ_ਖਾਨਾ = mouse_ਖਾਨਾ;
	}

	ਜੇ (ਉਚਾਈ_inc != 0)
	{
	    ਪੂਰਨ ਉਚਾਈ = wp->w_ਉਚਾਈ + ਉਚਾਈ_inc;

	    ਜੇ (ਉਚਾਈ < 1)
		ਉਚਾਈ = 1;
	    wp->w_minheight = ਉਚਾਈ;
	    wp->w_maxheight = ਉਚਾਈ;
	    drag_ਸ਼ੁਰੂ_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	}

	popup_adjust_position(wp);
	ਵਾਪਸ;
    }

    ਜੇ (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	ਵਾਪਸ;
    wp->w_wantline = drag_ਸ਼ੁਰੂ_wantline + (mouse_ਪੰਕਤੀ - drag_ਸ਼ੁਰੂ_ਪੰਕਤੀ);
    ਜੇ (wp->w_wantline < 1)
	wp->w_wantline = 1;
    ਜੇ (wp->w_wantline > ਪੰਕਤੀਆਂ)
	wp->w_wantline = ਪੰਕਤੀਆਂ;
    wp->w_wantcol = drag_ਸ਼ੁਰੂ_wantcol + (mouse_ਖਾਨਾ - drag_ਸ਼ੁਰੂ_ਖਾਨਾ);
    ਜੇ (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    ਜੇ (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    ਖਾਲੀ
popup_ਸੈੱਟ_firstline(win_T *wp)
{
    ਪੂਰਨ	    ਉਚਾਈ = wp->w_ਉਚਾਈ;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    ਜਦੋਂ (wp->w_firstline > 1 && wp->w_ਉਚਾਈ < ਉਚਾਈ)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    ਪੂਰਨ
popup_is_ਵਿੱਚ_scrollbar(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਵਾਪਸ wp->w_has_scrollbar
	&& ਪੰਕਤੀ >= wp->w_popup_border[0]
	&& ਪੰਕਤੀ < popup_ਉਚਾਈ(wp) - wp->w_popup_border[2]
	&& ਖਾਨਾ == popup_ਚੌੜਾਈ(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    ਖਾਲੀ
popup_handle_scrollbar_click(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (!popup_is_ਵਿੱਚ_scrollbar(wp, ਪੰਕਤੀ, ਖਾਨਾ))
	ਵਾਪਸ;

    ਪੂਰਨ	    ਉਚਾਈ = popup_ਉਚਾਈ(wp);
    ਪੂਰਨ	    new_topline = wp->w_topline;

    ਜੇ (ਪੰਕਤੀ >= ਉਚਾਈ / 2)
    {
	// Click in lower half, scroll down.
	ਜੇ (wp->w_topline < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ++new_topline;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    ਜੇ (new_topline == wp->w_topline)
	ਵਾਪਸ;

    ਸੈੱਟ_topline(wp, new_topline);
    ਜੇ (wp == curwin)
    {
	ਜੇ (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_ਸੈੱਟ_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ਜੇ defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ਸਥਿਰ ਖਾਲੀ
popup_ਜੋੜੋ_timeout(win_T *wp, ਪੂਰਨ ਸਮਾਂ, ਪੂਰਨ close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((ਅੱਖਰ *)cbbuf, ਆਕਾਰ(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    ਜੇ (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	ਵਾਪਸ;

    wp->w_popup_timer = create_timer(ਸਮਾਂ, 0);
    callback_T cb = get_callback(&tv);
    ਜੇ (cb.cb_name != ਨਿਰਾ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ poppos_T
get_pos_ਇੰਦਰਾਜ(dict_T *d, ਪੂਰਨ give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    ਪੂਰਨ	    nr;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ POPPOS_NONE;

    ਲਈ (nr = 0; nr < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(poppos_entries); ++nr)
	ਜੇ (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    ਵਾਪਸ poppos_entries[nr].pp_val;

    ਜੇ (give_error)
	semsg(_(e_invalid_ਦਲੀਲ_str), str);
    ਵਾਪਸ POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ਸਥਿਰ ਖਾਲੀ
apply_move_options(win_T *wp, dict_T *d)
{
    ਪੂਰਨ		nr;
    char_u	*str;
    dictitem_T	*di;

    ਜੇ ((nr = dict_get_ਸੰਖਿਆ_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    ਜੇ ((nr = dict_get_ਸੰਖਿਆ_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    ਜੇ ((nr = dict_get_ਸੰਖਿਆ_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    ਜੇ ((nr = dict_get_ਸੰਖਿਆ_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    ਜੇ (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    ਜੇ (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    ਜੇ (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_ਇੰਦਰਾਜ(d, TRUE);

	ਜੇ (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    ਜੇ (str != ਨਿਰਾ)
    {
	wp->w_popup_prop_type = 0;
	ਜੇ (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_ਲੱਭੋ(d, (char_u *)"textpropwin", -1);
	    ਜੇ (di != ਨਿਰਾ)
	    {
		wp->w_popup_prop_win = ਲੱਭੋ_win_by_nr_or_id(&di->di_tv);
		ਜੇ (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = ਲੱਭੋ_prop_type_id(str, wp->w_popup_prop_win->w_ਬਫਰ);
	    ਜੇ (nr <= 0)
		nr = ਲੱਭੋ_prop_type_id(str, ਨਿਰਾ);
	    ਜੇ (nr <= 0)
		semsg(_(e_invalid_ਦਲੀਲ_str), str);
	    ਨਹੀਂਤਾਂ
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_ਲੱਭੋ(d, (char_u *)"textpropid", -1);
    ਜੇ (di != ਨਿਰਾ)
	wp->w_popup_prop_id = dict_get_ਸੰਖਿਆ(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_moved_ਦਲੀਲ(win_T *wp, dictitem_T *di, ਪੂਰਨ mousemoved)
{
    ਜੇ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ਨਿਰਾ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	ਪੂਰਨ	flags = 0;

	ਜੇ (STRCMP(s, "word") == 0)
	    flags = ਲੱਭੋ_IDENT | ਲੱਭੋ_STRING;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "WORD") == 0)
	    flags = ਲੱਭੋ_STRING;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "expr") == 0)
	    flags = ਲੱਭੋ_IDENT | ਲੱਭੋ_STRING | ਲੱਭੋ_EVAL;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_ਦਲੀਲ_str), s);
	ਜੇ (flags != 0)
	{
	    ਜੇ (mousemoved)
		ਸੈੱਟ_mousemoved_columns(wp, flags);
	    ਨਹੀਂਤਾਂ
		ਸੈੱਟ_moved_columns(wp, flags);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ਨਿਰਾ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	ਪੂਰਨ	    mincol;
	ਪੂਰਨ	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ਪਹਿਲਾ;
	ਜੇ (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_ਸੰਖਿਆ(&l->lv_ਪਹਿਲਾ->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    ਜੇ (mousemoved)
		wp->w_popup_mouse_ਪੰਕਤੀ = nr;
	    ਨਹੀਂਤਾਂ
		wp->w_popup_lnum = nr;
	    li = li->li_ਅਗਲਾ;
	    ਜੇ (nr == 0)
		wp->w_popup_curwin = ਨਿਰਾ;
	}

	mincol = tv_get_ਸੰਖਿਆ(&li->li_tv);
	maxcol = tv_get_ਸੰਖਿਆ(&li->li_ਅਗਲਾ->li_tv);
	ਜੇ (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&di->di_tv));
}

    ਸਥਿਰ ਖਾਲੀ
check_highlight(dict_T *dict, ਅੱਖਰ *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_ਲੱਭੋ(dict, (char_u *)name, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), name);
    ਨਹੀਂਤਾਂ
    {
	str = tv_get_string(&di->di_tv);
	ਜੇ (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_show_curline(win_T *wp)
{
    ਜੇ (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_ਉਚਾਈ + 1;
	ਜੇ (wp->w_topline < 1)
	    wp->w_topline = 1;
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_topline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    wp->w_topline = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਜਦੋਂ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_ਉਚਾਈ + 1) > wp->w_ਉਚਾਈ)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    ਜੇ (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ਸਥਿਰ char_u *
popup_get_sign_name(win_T *wp)
{
    ਸਥਿਰ ਅੱਖਰ    buf[30];

    vim_snprintf(buf, ਆਕਾਰ(buf), "popup-%d", wp->w_id);
    ਵਾਪਸ (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_highlight_curline(win_T *wp)
{
    ਪੂਰਨ	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_ਬਫਰ, (char_u *)"PopUpMenu");

    ਜੇ ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	ਜੇ (!sign_exists_by_name(sign_name))
	{
	    ਅੱਖਰ *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ਨਿਰਾ, (char_u *)linehl, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
		    ਨਿਰਾ, SIGN_DEF_PRIO);
	}

	sign_ਰੱਖੋ(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_ਬਫਰ, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ਨਹੀਂਤਾਂ
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_ਆਖਰੀ_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ਸਥਿਰ ਪੂਰਨ
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    ਪੂਰਨ		nr;
    char_u	*str;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#ਜੇ_ਅੰਤ

    // TODO: flip

    di = dict_ਲੱਭੋ(dict, (char_u *)"firstline", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	wp->w_firstline = dict_get_ਸੰਖਿਆ(dict, "firstline");
	ਜੇ (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    ਜੇ (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    ਜੇ (str != ਨਿਰਾ)
    {
	char_u	*title = vim_strsave(str);

	ਜੇ (title != ਨਿਰਾ)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    ਜੇ (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"opacity", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	nr = dict_get_ਸੰਖਿਆ(dict, "opacity");
	ਜੇ (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	ਨਹੀਂਤਾਂ ਜੇ (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	ਨਹੀਂਤਾਂ ਜੇ (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    ਜੇ (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    ਨਹੀਂਤਾਂ
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_ਲੱਭੋ(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ਨਿਰਾ)
    {
	image_T		    *img;
	image_T		    *cur_img = ਨਿਰਾ;
	image_placement_T   *ਰੱਖੋ;
	ਪੂਰਨ		    cw, ch;

	ਜੇ (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ਨਿਰਾ;
	    }
	    ਜਾਓ image_ਪੂਰਾ;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	ਜੇ (wp->w_popup_imagep != ਨਿਰਾ && wp->w_popup_imagep->img != ਨਿਰਾ)
	    cur_img = wp->w_popup_imagep->img;

	img = ਜੋੜੋ_image(di->di_tv.vval.v_dict, cur_img, true);
	ਜੇ (img == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (img == cur_img)
	    ਜਾਓ image_ਪੂਰਾ;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	ਰੱਖੋ = image_placement_new(img, wp->w_id, iplace_use, false);

	ਜੇ (ਰੱਖੋ == ਨਿਰਾ)
	{
	    image_unref(img);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = ਰੱਖੋ;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	ਜੇ (dict_ਲੱਭੋ(dict, (char_u *)"minwidth", -1) == ਨਿਰਾ)
	    wp->w_minwidth = cw;
	ਜੇ (dict_ਲੱਭੋ(dict, (char_u *)"maxwidth", -1) == ਨਿਰਾ)
	    wp->w_maxwidth = cw;
	ਜੇ (dict_ਲੱਭੋ(dict, (char_u *)"minheight", -1) == ਨਿਰਾ)
	    wp->w_minheight = ch;
	ਜੇ (dict_ਲੱਭੋ(dict, (char_u *)"maxheight", -1) == ਨਿਰਾ)
	    wp->w_maxheight = ch;
image_ਪੂਰਾ:
	;
    }
    ਨਹੀਂਤਾਂ
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ਨਿਰਾ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	    image_placement_ਸੈੱਟ_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_ਉਚਾਈ, wp->w_ਚੌੜਾਈ);
    }
#ਜੇ_ਅੰਤ

    di = dict_ਲੱਭੋ(dict, (char_u *)"close", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਪੂਰਨ ok = TRUE;

	ਜੇ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ਨਿਰਾ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    ਜੇ (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    ਨਹੀਂਤਾਂ
		ok = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    ok = FALSE;
	ਜੇ (!ok)
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    ਜੇ (str != ਨਿਰਾ)
    {
	ਅੱਖਰ *errmsg = update_wincolor(wp, str);

	ਜੇ (errmsg == ਨਿਰਾ)
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ਨਹੀਂਤਾਂ
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    ਜੇ (str != ਨਿਰਾ)
    {
	ਅੱਖਰ *errmsg = update_winhighlight(wp, str);

	ਜੇ (errmsg == ਨਿਰਾ)
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ਨਹੀਂਤਾਂ
	    emsg(_(errmsg));
    }

    ਜੇ (ਸੈੱਟ_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	ਸੈੱਟ_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	ਵਾਪਸ FAIL;

    di = dict_ਲੱਭੋ(dict, (char_u *)"borderhighlight", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਜੇ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ਨਿਰਾ)
	{
	    emsg(_(e_list_required));
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ਪੂਰਨ		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    ਜੇ (list->lv_len == 0)
	    {
		ਲਈ (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ਨਿਰਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (i = 0, li = list->lv_ਪਹਿਲਾ; i < 4 && i < list->lv_len;
						     ++i, li = li->li_ਅਗਲਾ)
		{
		    str = tv_get_string(&li->li_tv);
		    ਜੇ (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			ਜੇ (hl != ਨਿਰਾ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		ਜੇ (list->lv_len == 1 && wp->w_border_highlight[0] != ਨਿਰਾ)
		    ਲਈ (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			ਜੇ (hl != ਨਿਰਾ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"borderchars", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਜੇ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ਪੂਰਨ		i;

	    ਜੇ (list != ਨਿਰਾ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		ਲਈ (i = 0, li = list->lv_ਪਹਿਲਾ; i < 8 && i < list->lv_len;
							 ++i, li = li->li_ਅਗਲਾ)
		{
		    str = tv_get_string(&li->li_tv);
		    ਜੇ (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		ਜੇ (list->lv_len == 1)
		    ਲਈ (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		ਜੇ (list->lv_len == 2)
		{
		    ਲਈ (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    ਲਈ (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_ਲੱਭੋ(dict, (char_u *)"zindex", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	wp->w_zindex = dict_get_ਸੰਖਿਆ(dict, "zindex");
	ਜੇ (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	ਜੇ (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"mask", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਪੂਰਨ ok = FALSE;

	ਜੇ (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ਨਿਰਾ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(di->di_tv.vval.v_list, li)
	    {
		ਜੇ (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ਨਿਰਾ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	ਜੇ (ok)
	{
	    wp->w_popup_ਨਕਾਬ = di->di_tv.vval.v_list;
	    ++wp->w_popup_ਨਕਾਬ->lv_refcount;
	    VIM_CLEAR(wp->w_popup_ਨਕਾਬ_cells);
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "mask");
	    ਵਾਪਸ FAIL;
	}
    }

#ਜੇ defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_ਸੰਖਿਆ(dict, "time");
    ਜੇ (nr > 0)
	popup_ਜੋੜੋ_timeout(wp, nr, TRUE);
#ਜੇ_ਅੰਤ

    di = dict_ਲੱਭੋ(dict, (char_u *)"moved", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਸੈੱਟ_moved_ਮੁੱਲਾਂ(wp);
	handle_moved_ਦਲੀਲ(wp, di, FALSE);
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"mousemoved", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਸੈੱਟ_mousemoved_ਮੁੱਲਾਂ(wp);
	handle_moved_ਦਲੀਲ(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"filter", -1);
    ਜੇ (di != ਨਿਰਾ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	ਜੇ (callback.cb_name != ਨਿਰਾ)
	{
	    free_callback(&wp->w_filter_cb);
	    ਸੈੱਟ_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    ਜੇ (nr != -1)
    {
	ਜੇ (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    ਜੇ (str != ਨਿਰਾ)
    {
	ਜੇ (STRCMP(str, "a") == 0)
	    wp->w_filter_ਬਹੁਲਕ = ਬਹੁਲਕ_ALL;
	ਨਹੀਂਤਾਂ
	    wp->w_filter_ਬਹੁਲਕ = ਬਹੁਲਕ_str2flags(str);
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"callback", -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ OK;

    callback_T	callback = get_callback(&di->di_tv);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ OK;

    free_callback(&wp->w_close_cb);
    ਸੈੱਟ_callback(&wp->w_close_cb, &callback);
    ਵਾਪਸ OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ਸਥਿਰ ਪੂਰਨ
apply_options(win_T *wp, dict_T *dict, ਪੂਰਨ create)
{
    ਪੂਰਨ		nr;

    apply_move_options(wp, dict);

    ਜੇ (create)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    ਜੇ (apply_general_options(wp, dict) == FAIL)
	ਵਾਪਸ FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    ਜੇ (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    ਜੇ (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	ਜੇ (wp->w_firstline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    wp->w_cursor.lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_ਉਚਾਈ)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    ਜੇ (create)
	popup_ਨਕਾਬ_refresh = TRUE;
    popup_highlight_curline(wp);

    ਵਾਪਸ OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_popup_ਲੜੀਆਂ(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
	ਜੇ (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ਨਿਰਾ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_ਦਲੀਲ_1_list_ਵਸਤੂ_nr_dictionary_required), lnum + 1);
	    ਵਾਪਸ;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ਨਿਰਾ ? ਨਿਰਾ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ਨਿਰਾ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    ਲਈ (li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_ਲੱਭੋ(dict, (char_u *)"props", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ਵਾਪਸ;
	    }
	    plist = di->di_tv.vval.v_list;
	    ਜੇ (plist != ਨਿਰਾ)
	    {
		FOR_ALL_LIST_ਚੀਜ਼ਾਂ(plist, pli)
		{
		    ਜੇ (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			ਵਾਪਸ;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    ਜੇ (dict != ਨਿਰਾ)
		    {
			ਪੂਰਨ ਖਾਨਾ = dict_get_ਸੰਖਿਆ(dict, "col");

			prop_ਜੋੜੋ_common(lnum, ਖਾਨਾ, dict, buf, ਨਿਰਾ);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    ਪੂਰਨ
popup_ਸਿਖਰ_extra(win_T *wp)
{
    ਪੂਰਨ	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    ਜੇ (extra == 0 && wp->w_popup_title != ਨਿਰਾ && *wp->w_popup_title != NUL)
	ਵਾਪਸ 1;
    ਵਾਪਸ extra;
}

/*
 * Get the padding plus border at the left.
 */
    ਪੂਰਨ
popup_ਖੱਬਾ_extra(win_T *wp)
{
    ਵਾਪਸ wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    ਪੂਰਨ
popup_ਉਚਾਈ(win_T *wp)
{
    ਵਾਪਸ wp->w_ਉਚਾਈ
	    + popup_ਸਿਖਰ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    ਪੂਰਨ
popup_ਚੌੜਾਈ(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    ਵਾਪਸ wp->w_ਚੌੜਾਈ
	    + popup_extra_ਚੌੜਾਈ(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    ਪੂਰਨ
popup_extra_ਚੌੜਾਈ(win_T *wp)
{
    ਵਾਪਸ wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ਸਥਿਰ win_T *
popup_get_clipwin(win_T *wp)
{
    ਜੇ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (win_valid(wp->w_popup_prop_win))
	ਵਾਪਸ wp->w_popup_prop_win;
    ਵਾਪਸ ਨਿਰਾ;
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ ਸਿਖਰ_extra;
    ਪੂਰਨ bot_extra;
    ਪੂਰਨ ਖੱਬਾ_extra;
    ਪੂਰਨ ਸੱਜਾ_extra;

    ਪੂਰਨ clip_ਸਿਖਰ_content;
    ਪੂਰਨ clip_bot_content;
    ਪੂਰਨ clip_ਖੱਬਾ_content;
    ਪੂਰਨ clip_ਸੱਜਾ_content;

    ਪੂਰਨ eff_ਸਿਖਰ_extra;
    ਪੂਰਨ eff_bot_extra;
    ਪੂਰਨ eff_ਖੱਬਾ_extra;
    ਪੂਰਨ eff_ਸੱਜਾ_extra;

    ਪੂਰਨ eff_border[4];
    ਪੂਰਨ eff_padding[4];

    ਪੂਰਨ eff_ਉਚਾਈ;
    ਪੂਰਨ eff_ਚੌੜਾਈ;
} popup_clip_T;

    ਸਥਿਰ ਖਾਲੀ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    ਪੂਰਨ h, w;

    cl->ਸਿਖਰ_extra = popup_ਸਿਖਰ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ਖੱਬਾ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->ਸੱਜਾ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_ਸਿਖਰ_content = wp->w_popup_topoff - cl->ਸਿਖਰ_extra;
    ਜੇ (cl->clip_ਸਿਖਰ_content < 0)
	cl->clip_ਸਿਖਰ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    ਜੇ (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ਖੱਬਾ_content = wp->w_popup_leftclip - cl->ਖੱਬਾ_extra;
    ਜੇ (cl->clip_ਖੱਬਾ_content < 0)
	cl->clip_ਖੱਬਾ_content = 0;
    cl->clip_ਸੱਜਾ_content = wp->w_popup_rightclip - cl->ਸੱਜਾ_extra;
    ਜੇ (cl->clip_ਸੱਜਾ_content < 0)
	cl->clip_ਸੱਜਾ_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	ਪੂਰਨ clip = wp->w_popup_topoff;
	ਪੂਰਨ b = wp->w_popup_border[0];
	ਪੂਰਨ p = wp->w_popup_padding[0];
	ਪੂਰਨ rem;

	ਜੇ (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	ਨਹੀਂਤਾਂ
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	ਪੂਰਨ clip = wp->w_popup_bottomoff;
	ਪੂਰਨ b = wp->w_popup_border[2];
	ਪੂਰਨ p = wp->w_popup_padding[2];
	ਪੂਰਨ rem;

	ਜੇ (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	ਨਹੀਂਤਾਂ
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
	ਪੂਰਨ excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_ਉਚਾਈ;
	ਜੇ (excess > 0)
	{
	    ਜੇ (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		ਜੇ (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		ਨਹੀਂਤਾਂ
		    cl->eff_border[0] -= excess;
	    }
	    ਨਹੀਂਤਾਂ
		cl->eff_padding[0] -= excess;
	}
    }
    {
	ਪੂਰਨ excess = wp->w_popup_topoff - cl->ਸਿਖਰ_extra - wp->w_ਉਚਾਈ;
	ਜੇ (excess > 0)
	{
	    ਜੇ (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		ਜੇ (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		ਨਹੀਂਤਾਂ
		    cl->eff_border[2] -= excess;
	    }
	    ਨਹੀਂਤਾਂ
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_ਸਿਖਰ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ਖੱਬਾ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ਖੱਬਾ_extra;
    cl->eff_ਸੱਜਾ_extra = wp->w_popup_rightclip > 0 ? 0 : cl->ਸੱਜਾ_extra;

    h = wp->w_ਉਚਾਈ - cl->clip_ਸਿਖਰ_content - cl->clip_bot_content;
    ਜੇ (h < 0)
	h = 0;
    cl->eff_ਉਚਾਈ = cl->eff_ਸਿਖਰ_extra + h + cl->eff_bot_extra;

    w = wp->w_ਚੌੜਾਈ - cl->clip_ਖੱਬਾ_content - cl->clip_ਸੱਜਾ_content;
    ਜੇ (w < 0)
	w = 0;
    cl->eff_ਚੌੜਾਈ = cl->eff_ਖੱਬਾ_extra + w + cl->eff_ਸੱਜਾ_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		winrow;
    ਪੂਰਨ		wincol;
    ਪੂਰਨ		leftcol;
    ਪੂਰਨ		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ਸਥਿਰ ਖਾਲੀ
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->ਉਚਾਈ  = wp->w_ਉਚਾਈ;
    sv->ਚੌੜਾਈ   = wp->w_ਚੌੜਾਈ;
    sv->winrow  = wp->w_winrow;
    sv->wincol  = wp->w_wincol;
    sv->leftcol = wp->w_leftcol;
    sv->p_wrap  = wp->w_p_wrap;
    sv->topline = wp->w_topline;
}

    ਸਥਿਰ ਖਾਲੀ
popup_geom_restore(win_T *wp, popup_geom_save_T *sv)
{
    wp->w_p_wrap  = sv->p_wrap;
    wp->w_leftcol = sv->leftcol;
    wp->w_wincol  = sv->wincol;
    wp->w_winrow  = sv->winrow;
    wp->w_topline = sv->topline;
    wp->w_ਚੌੜਾਈ   = sv->ਚੌੜਾਈ;
    wp->w_ਉਚਾਈ  = sv->ਉਚਾਈ;
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
    ਸਥਿਰ ਖਾਲੀ
popup_screenpos_above_ਸਿਖਰ(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	ਪੂਰਨ	    *screen_ਪੰਕਤੀ,
	ਪੂਰਨ	    *screen_scol,
	ਪੂਰਨ	    *screen_ccol,
	ਪੂਰਨ	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    ਪੂਰਨ	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (ਪੂਰਨ)win_ਖਾਨਾ_off(prop_win) - (ਪੂਰਨ)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (ਪੂਰਨ)scol + coloff;
    *screen_ccol = (ਪੂਰਨ)ccol + coloff;
    *screen_ecol = (ਪੂਰਨ)ecol + coloff;
    *screen_ਪੰਕਤੀ = prop_win->w_winrow + 1
				 - (ਪੂਰਨ)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_hide_for_textprop(win_T *wp)
{
    ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ਵਾਪਸ;
    wp->w_popup_flags |= POPF_HIDDEN;
    ਜੇ (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    ਸਥਿਰ bool
popup_ਲੱਭੋ_prop_above_ਸਿਖਰ(
	win_T	    *wp,
	win_T	    *prop_win,
	ਪੂਰਨ	    max_ਪਹੁੰਚ,
	textprop_T  *prop,
	linenr_T    *ਮਿਲਿਆ_lnum)
{
    linenr_T	ਪਹਿਲਾ;

    ਜੇ (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	ਵਾਪਸ false;

    ਪਹਿਲਾ = max_ਪਹੁੰਚ > 0 ? prop_win->w_topline - max_ਪਹੁੰਚ : 1;
    ਜੇ (ਪਹਿਲਾ < 1)
	ਪਹਿਲਾ = 1;
    ਵਾਪਸ ਲੱਭੋ_prop_ਵਿੱਚ_ਸਤਰਾਂ(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, ਮਿਲਿਆ_lnum, ਪਹਿਲਾ, prop_win->w_topline - 1);
}

/*
 * Compute and assign w_popup_topoff/bottomoff/leftclip/rightclip from the
 * host (textprop) window's content rectangle when POPF_CLIPWINDOW is set.
 * The popup's logical geometry (w_winrow, w_height, w_width) is preserved;
 * only the *off/clip fields record how much of each edge falls outside.
 * Returns true when the popup has scrolled completely past one of the host
 * edges, in which case the caller must hide it.
 */
    ਸਥਿਰ bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    ਪੂਰਨ	    extra_h, extra_w;
    ਪੂਰਨ	    popup_ਸਿਖਰ, popup_bottom, popup_ਖੱਬਾ, popup_ਸੱਜਾ;
    ਪੂਰਨ	    ਕੁੱਲ_h, ਕੁੱਲ_w;

    ਜੇ (cw == ਨਿਰਾ)
	ਵਾਪਸ false;

    extra_h = popup_ਸਿਖਰ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_ਚੌੜਾਈ(wp);

    popup_ਸਿਖਰ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_ਉਚਾਈ + extra_h;
    popup_ਖੱਬਾ = wp->w_wincol;
    popup_ਸੱਜਾ = wp->w_wincol + wp->w_ਚੌੜਾਈ + extra_w;
    ਕੁੱਲ_h = wp->w_ਉਚਾਈ + extra_h;
    ਕੁੱਲ_w = wp->w_ਚੌੜਾਈ + extra_w;

    ਜੇ (popup_ਸਿਖਰ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_ਸਿਖਰ;
    ਜੇ (popup_bottom > cw->w_winrow + cw->w_ਉਚਾਈ)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_ਉਚਾਈ);
    ਜੇ (popup_ਖੱਬਾ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ਖੱਬਾ;
    ਜੇ (popup_ਸੱਜਾ > cw->w_wincol + cw->w_ਚੌੜਾਈ)
	wp->w_popup_rightclip = popup_ਸੱਜਾ - (cw->w_wincol + cw->w_ਚੌੜਾਈ);

    ਵਾਪਸ wp->w_popup_topoff >= ਕੁੱਲ_h
	|| wp->w_popup_bottomoff >= ਕੁੱਲ_h
	|| wp->w_popup_leftclip >= ਕੁੱਲ_w
	|| wp->w_popup_rightclip >= ਕੁੱਲ_w;
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
    ਸਥਿਰ ਖਾਲੀ
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    ਜੇ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_ਉਚਾਈ -= cl->clip_ਸਿਖਰ_content + cl->clip_bot_content;
	ਜੇ (wp->w_ਉਚਾਈ < 0)
	    wp->w_ਉਚਾਈ = 0;
	ਜੇ (cl->clip_ਸਿਖਰ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_ਸਿਖਰ_content;
    }
    ਜੇ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	ਜੇ (cl->clip_ਖੱਬਾ_content > 0 || cl->clip_ਸੱਜਾ_content > 0)
	    wp->w_p_wrap = FALSE;
	ਜੇ (cl->clip_ਸੱਜਾ_content > 0)
	{
	    wp->w_ਚੌੜਾਈ -= cl->clip_ਸੱਜਾ_content;
	    ਜੇ (wp->w_ਚੌੜਾਈ < 0)
		wp->w_ਚੌੜਾਈ = 0;
	}
	ਜੇ (cl->clip_ਖੱਬਾ_content > 0)
	{
	    wp->w_leftcol += cl->clip_ਖੱਬਾ_content;
	    wp->w_wincol += cl->clip_ਖੱਬਾ_content;
	    wp->w_ਚੌੜਾਈ -= cl->clip_ਖੱਬਾ_content;
	    ਜੇ (wp->w_ਚੌੜਾਈ < 0)
		wp->w_ਚੌੜਾਈ = 0;
	}
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ਸਥਿਰ ਖਾਲੀ
popup_position_image(win_T *wp)
{
    linenr_T	    ਪੰਕਤੀ;
    colnr_T	    ਖਾਨਾ;

    ਪੂਰਨ iw, ih;

    // Visible cell width and height of popup itself (not image!)
    ਪੂਰਨ	visible_ਚੌੜਾਈ;
    ਪੂਰਨ	visible_ਉਚਾਈ;

    // Crop dimensions of image (in pixels).
    ਪੂਰਨ	crop_x, crop_y, crop_ਚੌੜਾਈ, crop_ਉਚਾਈ;

    popup_clip_T clip;

    ਜੇ (wp->w_popup_imagep == ਨਿਰਾ || wp->w_popup_flags & POPF_HIDDEN)
	ਵਾਪਸ;

    image_placement_ਸੈੱਟ_enable(wp->w_popup_imagep, true);

    ਪੰਕਤੀ = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    ਖਾਨਾ = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    ਜੇ (wp->w_popup_imagep->img == ਨਿਰਾ)
	// Just set the bounding box
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    visible_ਚੌੜਾਈ =
	wp->w_ਚੌੜਾਈ - clip.clip_ਖੱਬਾ_content - clip.clip_ਸੱਜਾ_content;
    visible_ਉਚਾਈ =
	wp->w_ਉਚਾਈ - clip.clip_ਸਿਖਰ_content - clip.clip_bot_content;

    ਜੇ (visible_ਚੌੜਾਈ < 0 || visible_ਉਚਾਈ < 0)
	ਵਾਪਸ;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ਖੱਬਾ_content;
    crop_y = clip.clip_ਸਿਖਰ_content;
    crop_ਚੌੜਾਈ = iw - crop_x - clip.clip_ਸੱਜਾ_content;
    crop_ਉਚਾਈ = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    ਪੰਕਤੀ -= wp->w_popup_border[0] - clip.eff_border[0];
    ਪੰਕਤੀ -= wp->w_popup_padding[0] - clip.eff_padding[0];

    ਖਾਨਾ -= wp->w_popup_border[3] - clip.eff_border[3];
    ਖਾਨਾ -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    ਜੇ (crop_ਚੌੜਾਈ > visible_ਚੌੜਾਈ)
	crop_ਚੌੜਾਈ = visible_ਚੌੜਾਈ;
    ਜੇ (crop_ਉਚਾਈ > visible_ਉਚਾਈ)
	crop_ਉਚਾਈ = visible_ਉਚਾਈ;

    image_placement_ਸੈੱਟ_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_ਚੌੜਾਈ, crop_ਉਚਾਈ);
ਬਾਹਰ_ਨਿਕਲੋ:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_ਸੈੱਟ_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_ਸੈੱਟ_position(wp->w_popup_imagep, ਪੰਕਤੀ, ਖਾਨਾ);
    image_placement_ਸੈੱਟ_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_ਉਚਾਈ, clip.eff_ਚੌੜਾਈ);
}
#ਜੇ_ਅੰਤ


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    ਪੂਰਨ		wrapped = 0;
    ਪੂਰਨ		maxwidth;
    ਪੂਰਨ		maxwidth_no_scrollbar;
    ਪੂਰਨ		ਚੌੜਾਈ_with_scrollbar = 0;
    ਪੂਰਨ		ਵਰਤਿਆ_maxwidth = FALSE;
    ਪੂਰਨ		margin_ਚੌੜਾਈ = 0;
    ਪੂਰਨ		maxspace;
    ਪੂਰਨ		center_vert = FALSE;
    ਪੂਰਨ		center_hor = FALSE;
    ਪੂਰਨ		allow_adjust_ਖੱਬਾ = !wp->w_popup_fixed;
    ਪੂਰਨ		ਸਿਖਰ_extra = popup_ਸਿਖਰ_extra(wp);
    ਪੂਰਨ		ਸੱਜਾ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    ਪੂਰਨ		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    ਪੂਰਨ		ਖੱਬਾ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    ਪੂਰਨ		extra_ਉਚਾਈ = ਸਿਖਰ_extra + bot_extra;
    ਪੂਰਨ		extra_ਚੌੜਾਈ = ਖੱਬਾ_extra + ਸੱਜਾ_extra;
    ਪੂਰਨ		w_ਉਚਾਈ_before_ਹੱਦਬੰਦੀ;
    popup_layout_T org_layout;
    ਪੂਰਨ		minwidth, minheight;
    ਪੂਰਨ		maxheight = ਪੰਕਤੀਆਂ;
    ਪੂਰਨ		wantline = wp->w_wantline;  // adjusted for textprop
    ਪੂਰਨ		wantcol = wp->w_wantcol;    // adjusted for textprop
    ਪੂਰਨ		use_wantcol = wantcol != 0;
    ਪੂਰਨ		adjust_ਉਚਾਈ_for_ਸਿਖਰ_aligned = FALSE;

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
    ਜੇ (wp->w_popup_ਆਖਰੀ_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    ਜੇ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	ਪੂਰਨ	    screen_ਪੰਕਤੀ;
	ਪੂਰਨ	    screen_scol;
	ਪੂਰਨ	    screen_ccol;
	ਪੂਰਨ	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_ਸਿਖਰ = false;
	ਜੇ (!ਲੱਭੋ_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    ਜੇ (popup_ਲੱਭੋ_prop_above_ਸਿਖਰ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_ਸਿਖਰ = true;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
		    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
			image_placement_clear(wp->w_popup_imagep);
#ਜੇ_ਅੰਤ
		    popup_hide_for_textprop(wp);
		    ਜੇ (wp->w_winrow + popup_ਉਚਾਈ(wp) >= cmdline_ਪੰਕਤੀ)
			clear_cmdline = TRUE;
		}
		ਵਾਪਸ;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.ਖਾਨਾ = prop.tp_ਖਾਨਾ;
	ਜੇ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.ਖਾਨਾ += prop.tp_len - 1;
	ਜੇ (prop_above_ਸਿਖਰ)
	    popup_screenpos_above_ਸਿਖਰ(prop_win, &pos, prop_lnum, &screen_ਪੰਕਤੀ,
				     &screen_scol, &screen_ccol, &screen_ecol);
	ਨਹੀਂਤਾਂ
	    textpos2screenpos(prop_win, &pos, &screen_ਪੰਕਤੀ,
				     &screen_scol, &screen_ccol, &screen_ecol);

	ਜੇ (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_ਚੌੜਾਈ = 0;
	    ਵਾਪਸ;
	}
	ਜੇ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_ਪੰਕਤੀ + wantline + 1;
	ਨਹੀਂਤਾਂ
	    // above the text
	    wantline = screen_ਪੰਕਤੀ + wantline - 1;
	center_vert = FALSE;
	ਜੇ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	ਨਹੀਂਤਾਂ
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// If no line was specified default to vertical centering.
	ਜੇ (wantline == 0)
	    center_vert = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	ਜੇ (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    ਜੇ (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    ਜੇ (wp->w_winrow >= ਪੰਕਤੀਆਂ)
		wp->w_winrow = ਪੰਕਤੀਆਂ - 1;
	}
	ਜੇ (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_ਪੰਕਤੀ
				    - wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - 1, 0);
	}

	ਜੇ (!use_wantcol)
	    center_hor = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    ਜੇ (wp->w_wincol > firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
						- ਖੱਬਾ_extra - ਸੱਜਾ_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
						- ਖੱਬਾ_extra - ਸੱਜਾ_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    ਜੇ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
					- wp->w_wincol - ਖੱਬਾ_extra - ਸੱਜਾ_extra;
    maxwidth = maxspace;
    ਜੇ (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ਖੱਬਾ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    ਜੇ (wp->w_p_nu || wp->w_p_rnu)
	margin_ਚੌੜਾਈ = ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    margin_ਚੌੜਾਈ += wp->w_p_fdc;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    ਜੇ (signcolumn_on(wp))
	margin_ਚੌੜਾਈ += 2;
#ਜੇ_ਅੰਤ
    ਜੇ (margin_ਚੌੜਾਈ >= maxwidth)
	margin_ਚੌੜਾਈ = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    ਜੇ (wp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
    {
	ਜੇ (minwidth == 0)
	    minwidth = 20;
	ਜੇ (minheight == 0)
	    minheight = 5;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_ਪੰਕਤੀ - 1;

    // start at the desired first line
    ਜੇ (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	ਜੇ (wp->w_firstline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    wp->w_firstline = 0;
	ਨਹੀਂਤਾਂ
	    wp->w_topline = wp->w_firstline;
    }
    ਜੇ (wp->w_topline < 1)
	wp->w_topline = 1;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_topline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	wp->w_topline = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_ਚੌੜਾਈ = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	ਪੂਰਨ saved_w_ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;

	ਜੇ (wp->w_ਚੌੜਾਈ < maxwidth)
	    wp->w_ਚੌੜਾਈ = maxwidth;
	ਲਈ (ln = 1; ln <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++ln)
	{
	    ਪੂਰਨ len = linetabsize(wp, ln) + margin_ਚੌੜਾਈ;

	    ਜੇ (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    ਜੇ (saved_w_ਚੌੜਾਈ < len)
		saved_w_ਚੌੜਾਈ = len;
	    ਜੇ (wp->w_maxwidth > 0 && saved_w_ਚੌੜਾਈ >= wp->w_maxwidth)
		ਰੋਕੋ;
	}
	wp->w_ਚੌੜਾਈ = saved_w_ਚੌੜਾਈ;
    }
    ਜੇ (wp->w_firstline < 0)
	lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ
	lnum = wp->w_topline;
    ਜਦੋਂ (lnum >= 1 && lnum <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਪੂਰਨ len;
	ਪੂਰਨ w_ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	ਜੇ (wp->w_ਚੌੜਾਈ < maxwidth)
	    wp->w_ਚੌੜਾਈ = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_ਚੌੜਾਈ = w_ਚੌੜਾਈ;

	ਜੇ (len + margin_ਚੌੜਾਈ > maxwidth
		&& allow_adjust_ਖੱਬਾ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    ਪੂਰਨ ਖਿਸਕਾਓ_by = len + margin_ਚੌੜਾਈ - maxwidth;

	    ਜੇ (ਖਿਸਕਾਓ_by > wp->w_wincol)
	    {
		ਪੂਰਨ truncate_ਖਿਸਕਾਓ = ਖਿਸਕਾਓ_by - wp->w_wincol;

		ਖਿਸਕਾਓ_by -= truncate_ਖਿਸਕਾਓ;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    ਜੇ (wp->w_maxwidth > 0 && maxwidth + ਖਿਸਕਾਓ_by > wp->w_maxwidth)
		ਖਿਸਕਾਓ_by = wp->w_maxwidth - maxwidth;

	    ਜੇ (ਖਿਸਕਾਓ_by > 0)
	    {
		wp->w_wincol -= ਖਿਸਕਾਓ_by;
		maxwidth += ਖਿਸਕਾਓ_by;
	    }
	    wp->w_ਚੌੜਾਈ = maxwidth;
	}
	ਜੇ (wp->w_p_wrap)
	{
	    ਜਦੋਂ (len + margin_ਚੌੜਾਈ > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_ਚੌੜਾਈ;
		wp->w_ਚੌੜਾਈ = maxwidth;
		ਵਰਤਿਆ_maxwidth = TRUE;
	    }
	}
	ਜੇ (wp->w_ਚੌੜਾਈ < len + margin_ਚੌੜਾਈ)
	{
	    wp->w_ਚੌੜਾਈ = len + margin_ਚੌੜਾਈ;
	    ਜੇ (wp->w_maxwidth > 0 && wp->w_ਚੌੜਾਈ > wp->w_maxwidth)
		wp->w_ਚੌੜਾਈ = wp->w_maxwidth;
	}

	ਜੇ (wp->w_firstline < 0)
	    --lnum;
	ਨਹੀਂਤਾਂ
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	ਜੇ (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum)
		       + wrapped >= maxheight)
	    ਰੋਕੋ;
    }

    ਜੇ (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (wp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ && !ਪਦ_is_ਮੁਕੰਮਲ(wp->w_ਬਫਰ))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#ਜੇ_ਅੰਤ
    maxwidth_no_scrollbar = maxwidth;
    ਜੇ (wp->w_has_scrollbar)
    {
	++ਸੱਜਾ_extra;
	++extra_ਚੌੜਾਈ;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	ਜੇ (maxwidth + ਸੱਜਾ_extra >= maxspace
		&& (ਵਰਤਿਆ_maxwidth || (minwidth > 0 && wp->w_ਚੌੜਾਈ < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    ਜੇ (wp->w_popup_title != ਨਿਰਾ && *wp->w_popup_title != NUL)
    {
	ਪੂਰਨ title_len = vim_strsize(wp->w_popup_title) + 2 - extra_ਚੌੜਾਈ;

	ਜੇ (minwidth < title_len)
	    minwidth = title_len;
    }

    ਜੇ (minwidth > 0 && wp->w_ਚੌੜਾਈ < minwidth)
	wp->w_ਚੌੜਾਈ = minwidth;
    ਜੇ (wp->w_ਚੌੜਾਈ > maxwidth)
    {
	ਜੇ (wp->w_ਚੌੜਾਈ > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_ਚੌੜਾਈ - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	ਜੇ (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    ਪੂਰਨ off = wp->w_ਚੌੜਾਈ - maxwidth;
	    extra_ਚੌੜਾਈ -= MIN(off, ਸੱਜਾ_extra);
	    wp->w_ਚੌੜਾਈ = maxwidth_no_scrollbar;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_ਚੌੜਾਈ = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    ਚੌੜਾਈ_with_scrollbar = maxwidth_no_scrollbar - ਸੱਜਾ_extra;
	}
    }
    ਜੇ (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_ਚੌੜਾਈ - wp->w_ਚੌੜਾਈ - extra_ਚੌੜਾਈ) / 2;
	ਜੇ (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	ਪੂਰਨ leftoff = wantcol - (wp->w_ਚੌੜਾਈ + extra_ਚੌੜਾਈ);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	ਜੇ (leftoff >= 0)
	    wp->w_wincol = leftoff;
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_ਚੌੜਾਈ -= wp->w_leftcol;
	    ਜੇ (wp->w_ਚੌੜਾਈ < 0)
		wp->w_ਚੌੜਾਈ = 0;
	}
    }

    ਜੇ (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	ਪੂਰਨ want_ਖਾਨਾ = 0;

	// try to show the right border and any scrollbar
	want_ਖਾਨਾ = ਖੱਬਾ_extra + wp->w_ਚੌੜਾਈ + ਸੱਜਾ_extra;
	ਜੇ (want_ਖਾਨਾ > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_ਖਾਨਾ >= firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ - want_ਖਾਨਾ;
	    ਜੇ (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_ਉਚਾਈ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - wp->w_topline
								 + 1 + wrapped;
    ਜੇ (minheight > 0 && wp->w_ਉਚਾਈ < minheight)
	wp->w_ਉਚਾਈ = minheight;
    ਜੇ (maxheight > 0 && wp->w_ਉਚਾਈ > maxheight)
	wp->w_ਉਚਾਈ = maxheight;
    w_ਉਚਾਈ_before_ਹੱਦਬੰਦੀ = wp->w_ਉਚਾਈ;
    ਜੇ (wp->w_ਉਚਾਈ > ਪੰਕਤੀਆਂ - wp->w_winrow)
	wp->w_ਉਚਾਈ = ਪੰਕਤੀਆਂ - wp->w_winrow;

    ਜੇ (center_vert)
    {
	wp->w_winrow = (ਪੰਕਤੀਆਂ - wp->w_ਉਚਾਈ - extra_ਉਚਾਈ) / 2;
	ਜੇ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	ਜੇ ((wp->w_ਉਚਾਈ + extra_ਉਚਾਈ) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_ਉਚਾਈ + extra_ਉਚਾਈ);
	ਨਹੀਂਤਾਂ ਜੇ (wantline * 2 >= ਪੰਕਤੀਆਂ || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_ਉਚਾਈ = wantline - extra_ਉਚਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_ਉਚਾਈ_for_ਸਿਖਰ_aligned = TRUE;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	ਪੂਰਨ check_ਉਚਾਈ = (wp->w_popup_flags & POPF_INFO) ? wp->w_ਉਚਾਈ
						    : w_ਉਚਾਈ_before_ਹੱਦਬੰਦੀ;
	ਜੇ (wp != popup_dragwin
		&& wantline + (check_ਉਚਾਈ + extra_ਉਚਾਈ) - 1 > ਪੰਕਤੀਆਂ
		&& wantline * 2 > ਪੰਕਤੀਆਂ
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_ਉਚਾਈ = w_ਉਚਾਈ_before_ਹੱਦਬੰਦੀ;
	    wp->w_winrow = wantline - 2 - wp->w_ਉਚਾਈ - extra_ਉਚਾਈ;
	    ਜੇ (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_ਉਚਾਈ + 2;
	    ਜੇ (wp->w_winrow < 0)
	    {
		wp->w_ਉਚਾਈ += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_winrow = wantline - 1;
	    adjust_ਉਚਾਈ_for_ਸਿਖਰ_aligned = TRUE;
	}
    }

    ਜੇ (adjust_ਉਚਾਈ_for_ਸਿਖਰ_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_ਉਚਾਈ + extra_ਉਚਾਈ > ਪੰਕਤੀਆਂ)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_ਉਚਾਈ = ਪੰਕਤੀਆਂ - wp->w_winrow - extra_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (wp->w_ਬਫਰ->b_ਪਦ == ਨਿਰਾ || ਪਦ_is_ਮੁਕੰਮਲ(wp->w_ਬਫਰ))
#ਜੇ_ਅੰਤ
	{
	    // The width already accounts for a scrollbar that was there.
	    ਜੇ (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		ਜੇ (ਚੌੜਾਈ_with_scrollbar > 0)
		    wp->w_ਚੌੜਾਈ = ਚੌੜਾਈ_with_scrollbar;
		ਨਹੀਂਤਾਂ
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_ਚੌੜਾਈ;
	    }
	}
    }

    // make sure w_winrow is valid
    ਜੇ (wp->w_winrow >= ਪੰਕਤੀਆਂ)
	wp->w_winrow = ਪੰਕਤੀਆਂ - 1;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    ਜੇ (wp->w_wincol + wp->w_ਚੌੜਾਈ + extra_ਚੌੜਾਈ
				    > firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
						- wp->w_ਚੌੜਾਈ - extra_ਚੌੜਾਈ;
    ਜੇ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    ਜੇ (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    ਜੇ (wp->w_wincol + wp->w_ਚੌੜਾਈ + extra_ਚੌੜਾਈ
				    > firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
    {
	ਪੂਰਨ avail = firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
						- wp->w_wincol - extra_ਚੌੜਾਈ;
	wp->w_ਚੌੜਾਈ = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    ਜੇ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	ਜੇ (wp->w_winrow + wp->w_ਉਚਾਈ + extra_ਉਚਾਈ > ਪੰਕਤੀਆਂ)
	    wp->w_winrow = ਪੰਕਤੀਆਂ - wp->w_ਉਚਾਈ - extra_ਉਚਾਈ;
	ਜੇ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	ਜੇ (wp->w_winrow + wp->w_ਉਚਾਈ + extra_ਉਚਾਈ > ਪੰਕਤੀਆਂ)
	{
	    ਪੂਰਨ avail = ਪੰਕਤੀਆਂ - wp->w_winrow - extra_ਉਚਾਈ;
	    wp->w_ਉਚਾਈ = avail > 0 ? avail : 0;
	}
    }

    ਜੇ (wp->w_ਉਚਾਈ != org_layout.ਉਚਾਈ)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    ਜੇ (popup_compute_clipwindow_offsets(wp))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
	ਜੇ (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
		image_placement_clear(wp->w_popup_imagep);
#ਜੇ_ਅੰਤ

	popup_hide_for_textprop(wp);
	ਵਾਪਸ;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_ਆਖਰੀ_changedtick = CHANGEDTICK(wp->w_ਬਫਰ);
    ਜੇ (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_ਬਫਰ);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_ਚੌੜਾਈ = wp->w_popup_prop_win->w_ਚੌੜਾਈ;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_ਉਚਾਈ;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    ਜੇ (popup_layout_ਬਦਲਿਆ(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	ਜੇ (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_ਨਕਾਬ_refresh = TRUE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    popup_position_image(wp);
#ਜੇ_ਅੰਤ
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    TYPE_ਸਾਧਾਰਨ,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_ਸੁਨੇਹਾ_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_is_notification(create_type_T type)
{
    ਵਾਪਸ type == TYPE_NOTIFICATION || type == TYPE_ਸੁਨੇਹਾ_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ਸਥਿਰ ਖਾਲੀ
popup_ਸੈੱਟ_ਬਫਰ_ਪਾਠ(buf_T *buf, typval_T ਪਾਠ)
{
    ਪੂਰਨ	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    ਲਈ (lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_ਬਫਰ;

    // Add text to the buffer.
    ਜੇ (ਪਾਠ.v_type == VAR_STRING)
    {
	char_u *s = ਪਾਠ.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ਨਿਰਾ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    ਨਹੀਂਤਾਂ
    {
	list_T *l = ਪਾਠ.vval.v_list;

	ਜੇ (l != ਨਿਰਾ && l->lv_len > 0)
	{
	    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
		emsg(_(e_using_ਸੰਖਿਆ_as_string));
	    ਨਹੀਂਤਾਂ ਜੇ (l->lv_ਪਹਿਲਾ->li_tv.v_type == VAR_STRING)
		// list of strings
		ਜੋੜੋ_popup_ਲੜੀਆਂ(buf, l);
	    ਨਹੀਂਤਾਂ
		// list of dictionaries
		ਜੋੜੋ_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
    curbuf = curwin->w_ਬਫਰ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    ਕਰੋ {						    \
	ਜੇ (wp != ਨਿਰਾ)					    \
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
    } ਜਦੋਂ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_popup_option(win_T *wp, ਪੂਰਨ is_preview)
{
    char_u *p =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	!is_preview ? p_cpp :
#ਜੇ_ਅੰਤ
	p_pvp;
    ਪੂਰਨ	    border_enabled = FALSE;

    ਜੇ (wp != ਨਿਰਾ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    ਲਈ ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	ਪੂਰਨ	x;

	e = vim_strchr(p, ':');
	ਜੇ (e == ਨਿਰਾ || e[1] == NUL)
	    ਵਾਪਸ FAIL;

	p = vim_strchr(e, ',');
	ਜੇ (p == ਨਿਰਾ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	ਜੇ (STRNCMP(s, "height:", 7) == 0)
	{
	    ਜੇ (dig != p)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਜੇ (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "width:", 6) == 0)
	{
	    ਜੇ (dig != p)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਜੇ (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "highlight:", 10) == 0)
	{
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਅੱਖਰ *errmsg;
		ਪੂਰਨ c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		ਜੇ (errmsg == ਨਿਰਾ)
		    ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		ਨਹੀਂਤਾਂ
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    ਜੇ (*arg == NUL || *arg == ',')
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਲਈ (ਪੂਰਨ i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ਪੂਰਨ		i;
	    ਪੂਰਨ		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    ਪੂਰਨ		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    ਜੇ (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		ਜੇ (wp != ਨਿਰਾ)
		{
		    ਲਈ (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    ਸੈੱਟ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ਜਾਰੀ;
	    }

	    token = vim_strnsave(arg, token_len);
	    ਜੇ (token == ਨਿਰਾ)
		ਵਾਪਸ FAIL;

	    ਜੇ ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		ਜੇ (STRCMP(token, "single") == 0)
		    ਸੈੱਟ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(token, "double") == 0)
		    ਸੈੱਟ_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(token, "round") == 0)
		    ਸੈੱਟ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(token, "on") == 0)
		    ਸੈੱਟ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(token, "ascii") == 0)
		    ਸੈੱਟ_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    ਪੂਰਨ		ਬਾਹਰਲਾ[8];
		    ਪੂਰਨ		failed = FALSE;

		    ਸੈੱਟ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    ਲਈ (i = 0; i < 8 && !failed; i++)
		    {
			ਜੇ (*q == NUL)
			    failed = TRUE;
			ਨਹੀਂਤਾਂ
			{
			    ਬਾਹਰਲਾ[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    ਜੇ (i < 7)
			    {
				ਜੇ (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    ਜੇ (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			ਵਾਪਸ FAIL;
		    }
		    ਸੈੱਟ_BORDER_CHARS(ਬਾਹਰਲਾ[0], ਬਾਹਰਲਾ[1], ਬਾਹਰਲਾ[2], ਬਾਹਰਲਾ[3], ਬਾਹਰਲਾ[4],
			    ਬਾਹਰਲਾ[5], ਬਾਹਰਲਾ[6], ਬਾਹਰਲਾ[7]);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		vim_free(token);
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਲਈ (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    ਪੂਰਨ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ਪੂਰਨ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ਜੇ (!on && !off)
		ਵਾਪਸ FAIL;
	    on = on && mouse_has(MOUSE_ਪਾਓ) && (border_enabled || is_preview);
	    ਜੇ (wp != ਨਿਰਾ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ਪੂਰਨ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ਪੂਰਨ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ਜੇ (!on && !off)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਜੇ (on && mouse_has(MOUSE_ਪਾਓ))
		    wp->w_popup_flags |= POPF_RESIZE;
		ਨਹੀਂਤਾਂ
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ਪੂਰਨ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ਪੂਰਨ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ਜੇ (!on && !off)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    ਪੂਰਨ		ਵਸਤੂ = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    ਪੂਰਨ		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    ਜੇ (!menu && !ਵਸਤੂ)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(s, "opacity:", 8) == 0)
	{
	    ਜੇ (dig != p || x < 0 || x > 100)
		ਵਾਪਸ FAIL;
	    ਜੇ (wp != ਨਿਰਾ)
	    {
		ਜੇ (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		ਨਹੀਂਤਾਂ
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ਪੂਰਨ
parse_previewpopup(win_T *wp)
{
    ਵਾਪਸ parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ਪੂਰਨ
parse_completepopup(win_T *wp)
{
    ਵਾਪਸ parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    ਖਾਲੀ
popup_ਸੈੱਟ_wantpos_cursor(win_T *wp, ਪੂਰਨ ਚੌੜਾਈ, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    ਜੇ (d != ਨਿਰਾ)
	ppt = get_pos_ਇੰਦਰਾਜ(d, FALSE);

    setcursor_mayforce(TRUE);
    ਜੇ (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    ਨਹੀਂਤਾਂ
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	ਜੇ (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    ਜੇ (wp->w_wantcol > Columns - ਚੌੜਾਈ)
    {
	wp->w_wantcol = Columns - ਚੌੜਾਈ;
	ਜੇ (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    ਖਾਲੀ
popup_ਸੈੱਟ_wantpos_rowcol(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    wp->w_wantline = ਪੰਕਤੀ;
    wp->w_wantcol = ਖਾਨਾ;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_border_ਖੱਬਾ_ਸੱਜਾ_padding(win_T *wp)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_terminal_exists(ਖਾਲੀ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
		ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    ਖਾਲੀ
popup_redraw_all(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    ਪੂਰਨ
popup_need_redraw(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ਵਾਪਸ TRUE;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Set the color for a notification window.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_update_color(win_T *wp, create_type_T type)
{
    ਅੱਖਰ    *hiname = type == TYPE_ਸੁਨੇਹਾ_WIN
				       ? "MessageWindow" : "PopupNotification";
    ਅੱਖਰ    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    ਜੇ (errmsg == ਨਿਰਾ)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    ਨਹੀਂਤਾਂ
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ਸਥਿਰ win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ਨਿਰਾ;
    ਪੂਰਨ		tabnr = 0;
    ਪੂਰਨ		new_ਬਫਰ;
    buf_T	*buf = ਨਿਰਾ;
    dict_T	*d = ਨਿਰਾ;
    ਪੂਰਨ		i;

    ਜੇ (check_secure())
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (argvars != ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script()
		&& (check_for_string_or_ਸੰਖਿਆ_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    ਵਾਪਸ ਨਿਰਾ;

	// Check that arguments look OK.
	ਜੇ (argvars[0].v_type == VAR_ਸੰਖਿਆ)
	{
	    buf = buflist_findnr(argvars[0].vval.v_ਸੰਖਿਆ);
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		semsg(_(e_ਬਫਰ_nr_does_not_exist), argvars[0].vval.v_ਸੰਖਿਆ);
		ਵਾਪਸ ਨਿਰਾ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (buf->b_ਪਦ != ਨਿਰਾ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_ਦੂਜਾ_popup_with_terminal));
		ਵਾਪਸ ਨਿਰਾ;
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ਨਿਰਾ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ਨਿਰਾ))
	{
	    emsg(_(e_ਬਫਰ_ਸੰਖਿਆ_ਪਾਠ_or_list_required));
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	d = argvars[1].vval.v_dict;
    }

    ਜੇ (d != ਨਿਰਾ)
    {
	ਜੇ (dict_has_ਕੁੰਜੀ(d, "tabpage"))
	    tabnr = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "tabpage");
	ਨਹੀਂਤਾਂ ਜੇ (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	ਨਹੀਂਤਾਂ
	    tabnr = 0;
	ਜੇ (tabnr > 0)
	{
	    tp = ਲੱਭੋ_tabpage(tabnr);
	    ਜੇ (tp == ਨਿਰਾ)
	    {
		semsg(_(e_tabpage_not_ਮਿਲਿਆ_nr), tabnr);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    ਜੇ (buf != ਨਿਰਾ && buf->b_locked_ਵੰਡੋ)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_ਖਿੜਕੀ_ਮੰਜ਼ਿਲ_a_closing_ਬਫਰ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (rettv != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    ਜੇ (buf != ਨਿਰਾ)
    {
	// use existing buffer
	new_ਬਫਰ = FALSE;
	win_init_popup_win(wp, buf);
	ਸੈੱਟ_local_options_default(wp, FALSE);
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_READONLY;
	ਬਫਰ_ensure_loaded(buf);
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
    }
    ਨਹੀਂਤਾਂ
    {
	// create a new buffer associated with the popup
	new_ਬਫਰ = TRUE;
	buf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    win_free_popup(wp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	ਸੈੱਟ_local_options_default(wp, TRUE);
	ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_buf(buf, (char_u *)"bufhidden", -1,
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

    ਜੇ (tp != ਨਿਰਾ)
    {
	// popup on specified tab page
	wp->w_ਅਗਲਾ = tp->tp_ਪਹਿਲਾ_popupwin;
	tp->tp_ਪਹਿਲਾ_popupwin = wp;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tabnr == 0)
    {
	// popup on current tab page
	wp->w_ਅਗਲਾ = curtab->tp_ਪਹਿਲਾ_popupwin;
	curtab->tp_ਪਹਿਲਾ_popupwin = wp;
    }
    ਨਹੀਂਤਾਂ // (tabnr < 0)
    {
	win_T *prev = ਪਹਿਲਾ_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	ਜੇ (ਪਹਿਲਾ_popupwin == ਨਿਰਾ)
	    ਪਹਿਲਾ_popupwin = wp;
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (prev->w_ਅਗਲਾ != ਨਿਰਾ)
		prev = prev->w_ਅਗਲਾ;
	    prev->w_ਅਗਲਾ = wp;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#ਜੇ_ਅੰਤ
    }

    ਜੇ (new_ਬਫਰ && argvars != ਨਿਰਾ)
	popup_ਸੈੱਟ_ਬਫਰ_ਪਾਠ(buf, argvars[0]);

    ਜੇ (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    ਜੇ (type == TYPE_ATCURSOR)
    {
	popup_ਸੈੱਟ_wantpos_cursor(wp, 0, d);
	ਸੈੱਟ_moved_ਮੁੱਲਾਂ(wp);
	ਸੈੱਟ_moved_columns(wp, ਲੱਭੋ_STRING);
    }

    ਜੇ (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_ਪੰਕਤੀ;
	ਜੇ (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_ਖਾਨਾ + 1;
	ਸੈੱਟ_mousemoved_ਮੁੱਲਾਂ(wp);
	ਸੈੱਟ_mousemoved_columns(wp, ਲੱਭੋ_IDENT + ਲੱਭੋ_STRING + ਲੱਭੋ_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    ਜੇ (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	ਪੂਰਨ	ਉਚਾਈ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	ਲਈ (twp = ਪਹਿਲਾ_popupwin; twp != ਨਿਰਾ; twp = nextwin)
	{
	    nextwin = twp->w_ਅਗਲਾ;
	    ਜੇ (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + ਉਚਾਈ
		    && twp->w_winrow + popup_ਉਚਾਈ(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_ਉਚਾਈ(twp) + 1;
		nextwin = ਪਹਿਲਾ_popupwin;
	    }
	}
	ਜੇ (wp->w_wantline + ਉਚਾਈ > ਪੰਕਤੀਆਂ)
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
	ਲਈ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    ਜੇ (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	ਜੋੜੋ_border_ਖੱਬਾ_ਸੱਜਾ_padding(wp);
    }

    ਜੇ (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	ਜੇ (callback.cb_name != ਨਿਰਾ)
	{
	    ਸੈੱਟ_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    ਲਈ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    ਲਈ (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    ਜੇ (type == TYPE_PREVIEW)
    {
	ਜੇ (mouse_has(MOUSE_ਪਾਓ))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ਲਈ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_ਸੈੱਟ_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਜੇ (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	ਜੇ (mouse_has(MOUSE_ਪਾਓ))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ਜੋੜੋ_border_ਖੱਬਾ_ਸੱਜਾ_padding(wp);
	parse_completepopup(wp);
    }
#ਜੇ_ਅੰਤ

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_ਬਹੁਲਕ = ਬਹੁਲਕ_ALL;

    ਜੇ (d != ਨਿਰਾ)
    {
	// Deal with options.
	ਜੇ (apply_options(wp, d, TRUE) == FAIL)
	{
	    (ਖਾਲੀ)popup_close(wp->w_id, FALSE);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਜੇ (popup_is_notification(type) && wp->w_popup_timer == ਨਿਰਾ)
	popup_ਜੋੜੋ_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#ਜੇ_ਅੰਤ

    popup_adjust_position(wp);

    wp->w_vsep_ਚੌੜਾਈ = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_ਨਕਾਬ_refresh = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    ਜੇ (buf->b_ਪਦ != ਨਿਰਾ)
	win_enter(wp, FALSE);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ wp;
}

/*
 * popup_clear()
 */
    ਖਾਲੀ
f_popup_clear(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ force = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	force = (ਪੂਰਨ)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    ਖਾਲੀ
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ਸਾਧਾਰਨ);
}

/*
 * popup_atcursor({text}, {options})
 */
    ਖਾਲੀ
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    ਖਾਲੀ
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_popup_callback(win_T *wp, typval_T *ਨਤੀਜਾ)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = (varnumber_T)wp->w_id;

    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && ਨਤੀਜਾ->v_type != VAR_UNKNOWN)
	copy_tv(ਨਤੀਜਾ, &argv[1]);
    ਨਹੀਂਤਾਂ
    {
	argv[1].v_type = VAR_ਸੰਖਿਆ;
	argv[1].vval.v_ਸੰਖਿਆ = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਿੱਠ_ਮੰਜ਼ਿਲ_prevwin(win_T *wp)
{
    ਜੇ (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    ਨਹੀਂਤਾਂ
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ਸਥਿਰ ਖਾਲੀ
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    ਪੂਰਨ id;

    ਜੇ (!win_valid(wp))
       ਵਾਪਸ;

    id = wp->w_id;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (wp == curwin && curbuf->b_ਪਦ != ਨਿਰਾ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    ਜੇ (owp != curwin && owp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
		ਰੋਕੋ;
	ਜੇ (owp != ਨਿਰਾ)
	    win_enter(owp, FALSE);
	ਨਹੀਂਤਾਂ
	{
	    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, owp)
		ਜੇ (owp != curwin && owp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
		    ਰੋਕੋ;
	    ਜੇ (owp != ਨਿਰਾ)
		win_enter(owp, FALSE);
	    ਨਹੀਂਤਾਂ
		ਪਿੱਠ_ਮੰਜ਼ਿਲ_prevwin(wp);
	}
    }
#ਜੇ_ਅੰਤ

    // Just in case a check higher up is missing.
    ਜੇ (wp == curwin && ERROR_IF_POPUP_ਖਿੜਕੀ)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	ਵਾਪਸ;
    }

    CHECK_CURBUF;
    ਜੇ (wp->w_close_cb.cb_name != ਨਿਰਾ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    ਖਾਲੀ
popup_close_with_retval(win_T *wp, ਪੂਰਨ retval)
{
    typval_T res;

    res.v_type = VAR_ਸੰਖਿਆ;
    res.vval.v_ਸੰਖਿਆ = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    ਖਾਲੀ
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ਸਥਿਰ ਖਾਲੀ
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    ਜੇ (wp != mouse_wp
	    && wp->w_popup_mouse_ਪੰਕਤੀ != 0
	    && (wp->w_popup_mouse_ਪੰਕਤੀ != mouse_ਪੰਕਤੀ
		|| mouse_ਖਾਨਾ < wp->w_popup_mouse_mincol
		|| mouse_ਖਾਨਾ > wp->w_popup_mouse_maxcol))
    {
	// Careful: this makes "wp" invalid.
	popup_close_with_retval(wp, -2);
    }
}

/*
 * Called when the mouse moved: may close a popup with "mousemoved".
 */
    ਖਾਲੀ
popup_handle_mouse_moved(ਖਾਲੀ)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    ਪੂਰਨ	    ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ	    ਖਾਨਾ = mouse_ਖਾਨਾ;

    // find the window where the mouse is in
    mouse_wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);

    ਲਈ (wp = ਪਹਿਲਾ_popupwin; wp != ਨਿਰਾ; wp = nextwp)
    {
	nextwp = wp->w_ਅਗਲਾ;
	check_mouse_moved(wp, mouse_wp);
    }
    ਲਈ (wp = curtab->tp_ਪਹਿਲਾ_popupwin; wp != ਨਿਰਾ; wp = nextwp)
    {
	nextwp = wp->w_ਅਗਲਾ;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ਸਥਿਰ ਖਾਲੀ
filter_handle_drag(win_T *wp, ਪੂਰਨ c, typval_T *rettv)
{
    ਪੂਰਨ	ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ	ਖਾਨਾ = mouse_ਖਾਨਾ;

    ਜੇ ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_ਕੁੰਜੀ(c)
	    && (wp == popup_dragwin
			  || wp == mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_ਸੰਖਿਆ = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    ਖਾਲੀ
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		id;
    win_T	*wp;
    char_u	*ਕੁੰਜੀ;
    typval_T	res;
    ਪੂਰਨ		c;
    linenr_T	old_lnum;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    id = tv_get_ਸੰਖਿਆ(&argvars[0]);
    wp = win_id2wp(id);
    ਕੁੰਜੀ = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    c = *ਕੁੰਜੀ;
    ਜੇ (c == K_SPECIAL && ਕੁੰਜੀ[1] != NUL)
	c = ਮੰਜ਼ਿਲ_SPECIAL(ਕੁੰਜੀ[1], ਕੁੰਜੀ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
    res.v_type = VAR_ਸੰਖਿਆ;

    old_lnum = wp->w_cursor.lnum;
    ਜੇ (c == 'k' || c == 'K' || c == K_ਉੱਪਰ || c == Ctrl_P)
    {
	ਜੇ (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	ਨਹੀਂਤਾਂ
	    wp->w_cursor.lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    }
    ਜੇ (c == 'j' || c == 'J' || c == K_ਥੱਲੇ || c == Ctrl_N)
    {
	ਜੇ (wp->w_cursor.lnum < wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ++wp->w_cursor.lnum;
	ਨਹੀਂਤਾਂ
	    wp->w_cursor.lnum = 1;
    }
    ਜੇ (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	ਵਾਪਸ;
    }

    ਜੇ (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_ਸੰਖਿਆ = -1;
	popup_close_and_callback(wp, &res);
	ਵਾਪਸ;
    }
    ਜੇ (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_ਸੰਖਿਆ = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	ਵਾਪਸ;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    ਖਾਲੀ
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		id;
    win_T	*wp;
    char_u	*ਕੁੰਜੀ;
    typval_T	res;
    ਪੂਰਨ		c;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    id = tv_get_ਸੰਖਿਆ(&argvars[0]);
    wp = win_id2wp(id);
    ਕੁੰਜੀ = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    c = *ਕੁੰਜੀ;
    ਜੇ (c == CAR && need_wait_return)
	ਵਾਪਸ;
    ਜੇ (c == K_SPECIAL && ਕੁੰਜੀ[1] != NUL)
	c = ਮੰਜ਼ਿਲ_SPECIAL(ਕੁੰਜੀ[1], ਕੁੰਜੀ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;

    ਜੇ (c == 'y' || c == 'Y')
	res.vval.v_ਸੰਖਿਆ = 1;
    ਨਹੀਂਤਾਂ ਜੇ (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_ਸੰਖਿਆ = 0;
    ਨਹੀਂਤਾਂ
    {
	filter_handle_drag(wp, c, rettv);
	ਵਾਪਸ;
    }

    // Invoke callback
    res.v_type = VAR_ਸੰਖਿਆ;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    ਖਾਲੀ
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    ਖਾਲੀ
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    ਖਾਲੀ
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ਸਥਿਰ win_T *
ਲੱਭੋ_popup_win(ਪੂਰਨ id)
{
    win_T *wp = win_id2wp(id);

    ਜੇ (wp != ਨਿਰਾ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_ਖਿੜਕੀ_nr_is_not_popup_ਖਿੜਕੀ), id);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ wp;
}

/*
 * popup_close({id})
 */
    ਖਾਲੀ
f_popup_close(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		id;
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_ਪਦ == ਨਿਰਾ &&
#ਜੇ_ਅੰਤ
	    ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp != ਨਿਰਾ)
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
    ਸਥਿਰ ਖਾਲੀ
popup_clear_ਨਕਾਬ_for(win_T *wp)
{
    ਪੂਰਨ r, c;
    ਪੂਰਨ ਪੰਕਤੀ_ਸ਼ੁਰੂ, ਖਾਨਾ_ਸ਼ੁਰੂ, ਪੰਕਤੀ_ਸਿਰਾ, ਖਾਨਾ_ਸਿਰਾ;

    ਜੇ (popup_ਨਕਾਬ == ਨਿਰਾ || !popup_visible)
	ਵਾਪਸ;

    ਪੰਕਤੀ_ਸ਼ੁਰੂ = MAX(wp->w_winrow, 0);
    ਖਾਨਾ_ਸ਼ੁਰੂ = MAX(wp->w_wincol, 0);
    ਪੰਕਤੀ_ਸਿਰਾ = MIN(wp->w_winrow + popup_ਉਚਾਈ(wp), (ਪੂਰਨ)screen_ਪੰਕਤੀਆਂ);
    ਖਾਨਾ_ਸਿਰਾ = MIN(wp->w_wincol + popup_ਚੌੜਾਈ(wp), (ਪੂਰਨ)screen_Columns);

    ਲਈ (r = ਪੰਕਤੀ_ਸ਼ੁਰੂ; r < ਪੰਕਤੀ_ਸਿਰਾ; ++r)
	ਲਈ (c = ਖਾਨਾ_ਸ਼ੁਰੂ; c < ਖਾਨਾ_ਸਿਰਾ; ++c)
	    popup_ਨਕਾਬ[r * screen_Columns + c] = 0;
}

    ਖਾਲੀ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    ਪੂਰਨ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (error_if_ਪਦ_popup_ਖਿੜਕੀ())
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ਵਾਪਸ;

    popup_save_area(wp, &old_area);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	image_placement_clear(wp->w_popup_imagep);
#ਜੇ_ਅੰਤ

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    ਜੇ (wp->w_winrow + popup_ਉਚਾਈ(wp) >= cmdline_ਪੰਕਤੀ)
	clear_cmdline = TRUE;

    ਜੇ (was_visible)
	popup_clear_ਨਕਾਬ_for(wp);

    ਜੇ (old_area.ਸਰਗਰਮ)
	popup_redraw_exposed_area(&old_area);
    ਨਹੀਂਤਾਂ
	redraw_all_later(UPD_NOT_VALID);

    ਸਥਿਤੀ_redraw_all();
    ਜੇ (!old_area.ਸਰਗਰਮ)
	popup_ਨਕਾਬ_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    ਖਾਲੀ
f_popup_hide(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		id;
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    ਖਾਲੀ
popup_show(win_T *wp)
{
    bool popup_ਸਰਗਰਮ;

    ਜੇ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	ਵਾਪਸ;

    popup_ਸਰਗਰਮ = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    ਜੇ (popup_ਸਰਗਰਮ)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ਸਤਰਾਂ_valid = 0;
	ਜੇ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    ਨਹੀਂਤਾਂ
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_ਨਕਾਬ_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    ਖਾਲੀ
f_popup_show(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		id;
    win_T	*wp;

    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਜੇ (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#ਜੇ_ਅੰਤ

    rettv->vval.v_ਸੰਖਿਆ = 0;
}

/*
 * popup_settext({id}, {text})
 */
    ਖਾਲੀ
f_popup_settext(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    popup_save_area(wp, &old_area);

    ਜੇ (check_for_string_or_list_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    popup_ਸੈੱਟ_ਬਫਰ_ਪਾਠ(wp->w_ਬਫਰ, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ਸਤਰਾਂ_valid = 0;
    ਜੇ (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    ਜੇ (popup_area_ਬਦਲਿਆ(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    ਖਾਲੀ
f_popup_setbuf(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;

    ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਬਫਰ_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    buf = tv_get_buf_from_arg(&argvars[1]);

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (buf->b_ਪਦ != ਨਿਰਾ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_ਦੂਜਾ_popup_with_terminal));
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (wp->w_ਬਫਰ != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_ਬਫਰ->b_nwindows--;
	win_init_popup_win(wp, buf);
	ਸੈੱਟ_local_options_default(wp, FALSE);
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_READONLY;
	ਬਫਰ_ensure_loaded(buf);
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	ਜੇ (popup_area_ਬਦਲਿਆ(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    ਪੂਰਨ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ)
	image_placement_free(wp->w_popup_imagep);
#ਜੇ_ਅੰਤ

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_ਬਫਰ->b_locked = FALSE;
    ਜੇ (wp->w_winrow + popup_ਉਚਾਈ(wp) >= cmdline_ਪੰਕਤੀ)
	clear_cmdline = TRUE;

    ਜੇ (was_visible)
	popup_clear_ਨਕਾਬ_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    ਜੇ (wp == ਸੁਨੇਹਾ_win_2)
	ਸੁਨੇਹਾ_win_2 = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (!old_area.ਸਰਗਰਮ)
	redraw_all_later(UPD_NOT_VALID);
    ਸਥਿਤੀ_redraw_all();
    ਜੇ (!old_area.ਸਰਗਰਮ)
	popup_ਨਕਾਬ_refresh = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
error_for_popup_ਖਿੜਕੀ(ਖਾਲੀ)
{
    emsg(_(e_not_allowed_ਵਿੱਚ_popup_ਖਿੜਕੀ));
}

    ਪੂਰਨ
error_if_popup_ਖਿੜਕੀ(ਪੂਰਨ also_with_ਪਦ ਅਣਵਰਤਿਆ)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    ਜੇ (WIN_IS_POPUP(curwin)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    && (also_with_ਪਦ || curbuf->b_ਪਦ == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
	    )
    {
	error_for_popup_ਖਿੜਕੀ();
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    ਪੂਰਨ
popup_close(ਪੂਰਨ id, ਪੂਰਨ force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ਨਿਰਾ;

    // go through global popups
    ਲਈ (wp = ਪਹਿਲਾ_popupwin; wp != ਨਿਰਾ; prev = wp, wp = wp->w_ਅਗਲਾ)
	ਜੇ (wp->w_id == id)
	{
	    ਜੇ (wp == curwin)
	    {
		ਜੇ (!force)
		{
		    error_for_popup_ਖਿੜਕੀ();
		    ਵਾਪਸ FAIL;
		}
		ਪਿੱਠ_ਮੰਜ਼ਿਲ_prevwin(wp);
	    }
	    ਜੇ (prev == ਨਿਰਾ)
		ਪਹਿਲਾ_popupwin = wp->w_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
		prev->w_ਅਗਲਾ = wp->w_ਅਗਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#ਜੇ_ਅੰਤ
	    popup_free(wp);
	    ਵਾਪਸ OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	ਜੇ (popup_close_tabpage(tp, id, force) == OK)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    ਪੂਰਨ
popup_close_tabpage(tabpage_T *tp, ਪੂਰਨ id, ਪੂਰਨ force)
{
    win_T	*wp;
    win_T	**ਜੜ੍ਹ = &tp->tp_ਪਹਿਲਾ_popupwin;
    win_T	*prev = ਨਿਰਾ;

    ਲਈ (wp = *ਜੜ੍ਹ; wp != ਨਿਰਾ; prev = wp, wp = wp->w_ਅਗਲਾ)
	ਜੇ (wp->w_id == id)
	{
	    ਜੇ (wp == curwin)
	    {
		ਜੇ (!force)
		{
		    error_for_popup_ਖਿੜਕੀ();
		    ਵਾਪਸ FAIL;
		}
		ਪਿੱਠ_ਮੰਜ਼ਿਲ_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    ਜੇ (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    ਜੇ (prev == ਨਿਰਾ)
		*ਜੜ੍ਹ = wp->w_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
		prev->w_ਅਗਲਾ = wp->w_ਅਗਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#ਜੇ_ਅੰਤ
	    popup_free(wp);
	    ਵਾਪਸ OK;
	}
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
close_all_popups(ਪੂਰਨ force)
{
    ਜੇ (!force && ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    ਜਦੋਂ (ਪਹਿਲਾ_popupwin != ਨਿਰਾ)
	ਜੇ (popup_close(ਪਹਿਲਾ_popupwin->w_id, force) == FAIL)
	    ਵਾਪਸ;
    ਜਦੋਂ (curtab->tp_ਪਹਿਲਾ_popupwin != ਨਿਰਾ)
	ਜੇ (popup_close(curtab->tp_ਪਹਿਲਾ_popupwin->w_id, force) == FAIL)
	    ਵਾਪਸ;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->ਸਰਗਰਮ = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->ਉਚਾਈ = popup_ਉਚਾਈ(wp);
    area->ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    ਪੂਰਨ i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    ਲਈ (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ਸਥਿਰ bool
popup_style_ਬਦਲਿਆ(win_T *wp, popup_style_snapshot_T *style)
{
    ਪੂਰਨ i;

    ਜੇ (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	ਵਾਪਸ true;
    ਲਈ (i = 0; i < 4; i++)
	ਜੇ (style->border_highlight[i] != wp->w_border_highlight[i])
	    ਵਾਪਸ true;
    ਵਾਪਸ false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;
    layout->ਉਚਾਈ = wp->w_ਉਚਾਈ;
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
    ਸਥਿਰ bool
popup_layout_ਬਦਲਿਆ(win_T *wp, popup_layout_T *layout)
{
    ਵਾਪਸ layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->ਚੌੜਾਈ != wp->w_ਚੌੜਾਈ
	|| layout->ਉਚਾਈ != wp->w_ਉਚਾਈ
	|| layout->has_scrollbar != wp->w_has_scrollbar
	|| layout->topoff != wp->w_popup_topoff
	|| layout->bottomoff != wp->w_popup_bottomoff
	|| layout->leftclip != wp->w_popup_leftclip
	|| layout->rightclip != wp->w_popup_rightclip;
}

/*
 * Return true when the popup no longer covers the saved area.
 */
    ਸਥਿਰ bool
popup_area_ਬਦਲਿਆ(win_T *wp, popup_area_T *area)
{
    ਵਾਪਸ area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->ਉਚਾਈ != popup_ਉਚਾਈ(wp)
	|| area->ਚੌੜਾਈ != popup_ਚੌੜਾਈ(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ਸਥਿਰ ਖਾਲੀ
mark_overlapped_opacity_popup(win_T *wp, ਪੂਰਨ area_ਸਿਖਰ, ਪੂਰਨ area_bot,
	ਪੂਰਨ area_ਖੱਬਾ, ਪੂਰਨ area_ਸੱਜਾ, ਪੂਰਨ zindex)
{
    ਜੇ ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_ਉਚਾਈ(wp) <= area_ਸਿਖਰ
	    || wp->w_wincol >= area_ਸੱਜਾ
	    || wp->w_wincol + popup_ਚੌੜਾਈ(wp) - wp->w_popup_leftoff
							      <= area_ਖੱਬਾ)
	ਵਾਪਸ;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ਸਤਰਾਂ_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_overlapped_opacity_popups(ਪੂਰਨ winrow, ਪੂਰਨ wincol, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ ਚੌੜਾਈ,
	ਪੂਰਨ leftoff, ਪੂਰਨ zindex)
{
    win_T	*wp;
    ਪੂਰਨ		area_ਸਿਖਰ = winrow;
    ਪੂਰਨ		area_bot = winrow + ਉਚਾਈ;
    ਪੂਰਨ		area_ਖੱਬਾ = wincol;
    ਪੂਰਨ		area_ਸੱਜਾ = wincol + ਚੌੜਾਈ - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_ਸਿਖਰ, area_bot, area_ਖੱਬਾ,
		area_ਸੱਜਾ, zindex);
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_ਸਿਖਰ, area_bot, area_ਖੱਬਾ,
		area_ਸੱਜਾ, zindex);

    ਜੇ (must_redraw < UPD_VALID)
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
    ਸਥਿਰ ਖਾਲੀ
refill_pum_bg_ਪੰਕਤੀ_from_ਬਫਰ(ਪੂਰਨ r, ਪੂਰਨ ਖੱਬਾ, ਪੂਰਨ ਸੱਜਾ)
{
    ਪੂਰਨ		ਸਤਰ_cp = r;
    ਪੂਰਨ		ਖਾਨਾ_cp = ਖੱਬਾ;
    win_T	*wp;
    linenr_T	lnum;
    ਪੂਰਨ		ਪੰਕਤੀ_for_lnum;
    ਪੂਰਨ		ਖਾਨਾ_for_lnum = 0;
    char_u	*ਸਤਰ;
    char_u	*p;
    ਪੂਰਨ		win_ਪਾਠ_ਖਾਨਾ;
    ਪੂਰਨ		screen_ਖਾਨਾ;
    ਪੂਰਨ		soff_ਅਧਾਰ = (r - pum_bg_ਸਿਖਰ) * pum_bg_ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    ਲਈ (c = ਖੱਬਾ; c < ਸੱਜਾ; ++c)
    {
	pum_bg_ਸਤਰਾਂ[soff_ਅਧਾਰ + c] = ' ';
	ਜੇ (pum_bg_attrs != ਨਿਰਾ)
	    pum_bg_attrs[soff_ਅਧਾਰ + c] = 0;
	ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ)
	    pum_bg_ਸਤਰਾਂ_UC[soff_ਅਧਾਰ + c] = 0;
	ਜੇ (enc_utf8)
	{
	    ਪੂਰਨ k;
	    ਲਈ (k = 0; k < MAX_MCO; ++k)
		ਜੇ (pum_bg_ਸਤਰਾਂ_C[k] != ਨਿਰਾ)
		    pum_bg_ਸਤਰਾਂ_C[k][soff_ਅਧਾਰ + c] = 0;
	}
    }

    wp = mouse_ਲੱਭੋ_win(&ਸਤਰ_cp, &ਖਾਨਾ_cp, IGNORE_POPUP);
    ਜੇ (wp == ਨਿਰਾ || ਸਤਰ_cp < 0 || ਸਤਰ_cp >= wp->w_ਉਚਾਈ)
	ਵਾਪਸ;
    ਜੇ (wp->w_ਬਫਰ == ਨਿਰਾ || wp->w_ਬਫਰ->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ;

    // Compute the buffer line for this screen row.
    ਪੰਕਤੀ_for_lnum = ਸਤਰ_cp;
    ਜੇ (mouse_comp_pos(wp, &ਪੰਕਤੀ_for_lnum, &ਖਾਨਾ_for_lnum, &lnum, ਨਿਰਾ))
	ਵਾਪਸ;	// past end of buffer
    ਜੇ (lnum < 1 || lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    ਜੇ (ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_ਪਾਠ_ਖਾਨਾ = wp->w_wincol + win_ਖਾਨਾ_off(wp);
    ਜੇ (!wp->w_p_wrap)
	win_ਪਾਠ_ਖਾਨਾ -= wp->w_leftcol;
    screen_ਖਾਨਾ = win_ਪਾਠ_ਖਾਨਾ;
    p = ਸਤਰ;
    ਜਦੋਂ (*p != NUL && screen_ਖਾਨਾ < ਸੱਜਾ)
    {
	ਪੂਰਨ char_cells;
	ਪੂਰਨ byte_ਗਿਣਤੀ;
	ਪੂਰਨ soff = soff_ਅਧਾਰ + screen_ਖਾਨਾ;
	ਪੂਰਨ ਵਿੱਚ_range = (screen_ਖਾਨਾ >= ਖੱਬਾ && screen_ਖਾਨਾ < ਸੱਜਾ);

	ਜੇ (*p == '\t')
	{
	    ਪੂਰਨ ts = (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_ਖਾਨਾ - win_ਪਾਠ_ਖਾਨਾ) % ts) : 1;
	    byte_ਗਿਣਤੀ = 1;
	    ਜੇ (ਵਿੱਚ_range)
	    {
		pum_bg_ਸਤਰਾਂ[soff] = ' ';
		ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ)
		    pum_bg_ਸਤਰਾਂ_UC[soff] = 0;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_ਗਿਣਤੀ = mb_ptr2len(p);
	    ਜੇ (ਵਿੱਚ_range)
	    {
		pum_bg_ਸਤਰਾਂ[soff] = *p;
		ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ)
		    pum_bg_ਸਤਰਾਂ_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_cells = 1;
	    byte_ਗਿਣਤੀ = 1;
	    ਜੇ (ਵਿੱਚ_range)
	    {
		pum_bg_ਸਤਰਾਂ[soff] = (*p < 0x20) ? ' ' : *p;
		ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ)
		    pum_bg_ਸਤਰਾਂ_UC[soff] = 0;
	    }
	}
	ਜੇ (ਵਿੱਚ_range && pum_bg_attrs != ਨਿਰਾ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_ਗਿਣਤੀ;
	screen_ਖਾਨਾ += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_redraw_exposed_area(popup_area_T *area)
{
    ਜੇ (!area->ਸਰਗਰਮ)
	ਵਾਪਸ;

    redraw_under_popup_area(area->winrow, area->wincol, area->ਉਚਾਈ,
	    area->ਚੌੜਾਈ, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->ਉਚਾਈ, area->ਚੌੜਾਈ, area->leftoff, area->zindex);

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
    ਜੇ (pum_bg_ਸਤਰਾਂ != ਨਿਰਾ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->ਉਚਾਈ > pum_bg_ਸਿਖਰ)
    {
	ਪੂਰਨ ਸਿਖਰ = MAX(area->winrow, pum_bg_ਸਿਖਰ);
	ਪੂਰਨ bot = MIN(area->winrow + area->ਉਚਾਈ, pum_bg_bot);
	ਪੂਰਨ ਖੱਬਾ = MAX(area->wincol, 0);
	ਪੂਰਨ ਸੱਜਾ = MIN(area->wincol + area->ਚੌੜਾਈ, pum_bg_ਖਾਨੇ_ਗਿਣਤੀ);
	ਪੂਰਨ r;

	ਲਈ (r = ਸਿਖਰ; r < bot; ++r)
	    refill_pum_bg_ਪੰਕਤੀ_from_ਬਫਰ(r, ਖੱਬਾ, ਸੱਜਾ);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->ਸਤਰਾਂ);
    vim_free(saved_screen->attrs);
    vim_free(saved_screen->linesuc);
    CLEAR_ਸੂਚਕ(saved_screen);
}

/*
 * Save the screen area that opacity padding may need to blend against.
 * On entry "saved_screen" must be zero-initialised.  When the snapshot is
 * unavailable (no opacity popup, no padding, allocation failure) "lines" is
 * left NULL and callers should fall back to plain screen_fill().  The caller
 * must always release the snapshot with popup_free_saved_screen().
 */
    ਸਥਿਰ ਖਾਲੀ
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    ਜੇ (screen_opacity_popup == ਨਿਰਾ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	ਵਾਪਸ;

    saved_screen->ਸ਼ੁਰੂ_ਪੰਕਤੀ = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->ਸ਼ੁਰੂ_ਖਾਨਾ = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->ਪੰਕਤੀਆਂ_2 = wp->w_popup_padding[0] + wp->w_ਉਚਾਈ
						    + wp->w_popup_padding[2];
    saved_screen->ਖਾਨੇ_ਗਿਣਤੀ = wp->w_popup_padding[3] + wp->w_ਚੌੜਾਈ
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    ਜੇ (saved_screen->ਸ਼ੁਰੂ_ਖਾਨਾ > 0)
    {
	--saved_screen->ਸ਼ੁਰੂ_ਖਾਨਾ;
	++saved_screen->ਖਾਨੇ_ਗਿਣਤੀ;
    }

    saved_screen->ਸਤਰਾਂ = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->ਪੰਕਤੀਆਂ_2 * saved_screen->ਖਾਨੇ_ਗਿਣਤੀ);
    saved_screen->attrs = ALLOC_CLEAR_MULT(ਪੂਰਨ,
				  saved_screen->ਪੰਕਤੀਆਂ_2 * saved_screen->ਖਾਨੇ_ਗਿਣਤੀ);
    ਜੇ (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->ਪੰਕਤੀਆਂ_2 * saved_screen->ਖਾਨੇ_ਗਿਣਤੀ);

    ਜੇ (saved_screen->ਸਤਰਾਂ == ਨਿਰਾ || saved_screen->attrs == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ r = 0; r < saved_screen->ਪੰਕਤੀਆਂ_2; r++)
    {
	ਪੂਰਨ screen_ਪੰਕਤੀ = saved_screen->ਸ਼ੁਰੂ_ਪੰਕਤੀ + r;

	ਜੇ (screen_ਪੰਕਤੀ >= 0 && screen_ਪੰਕਤੀ < screen_ਪੰਕਤੀਆਂ)
	    ਲਈ (ਪੂਰਨ c = 0; c < saved_screen->ਖਾਨੇ_ਗਿਣਤੀ; c++)
	    {
		ਪੂਰਨ screen_ਖਾਨਾ = saved_screen->ਸ਼ੁਰੂ_ਖਾਨਾ + c;

		ਜੇ (screen_ਖਾਨਾ >= 0 && screen_ਖਾਨਾ < screen_Columns)
		{
		    ਪੂਰਨ off = ਸਤਰ_ਵਿੱਥ[screen_ਪੰਕਤੀ] + screen_ਖਾਨਾ;
		    ਪੂਰਨ save_off = r * saved_screen->ਖਾਨੇ_ਗਿਣਤੀ + c;

		    saved_screen->ਸਤਰਾਂ[save_off] = Screen_ਸਤਰਾਂ[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    ਜੇ (enc_utf8 && saved_screen->linesuc != ਨਿਰਾ)
			saved_screen->linesuc[save_off] = Screen_ਸਤਰਾਂ_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_under_popup_area(ਪੂਰਨ winrow, ਪੂਰਨ wincol, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ leftoff)
{
    ਪੂਰਨ	    r;

    ਲਈ (r = winrow; r < winrow + ਉਚਾਈ && r < screen_ਪੰਕਤੀਆਂ; ++r)
    {
	ਪੂਰਨ	    c;
	win_T	    *prev_twp = ਨਿਰਾ;

	ਜੇ (r >= cmdline_ਪੰਕਤੀ)
	{
	    clear_cmdline = TRUE;
	    ਜਾਰੀ;
	}

	ਲਈ (c = wincol; c < wincol + ਚੌੜਾਈ - leftoff && c < screen_Columns; ++c)
	{
	    ਪੂਰਨ	    ਸਤਰ_cp = r;
	    ਪੂਰਨ	    ਖਾਨਾ_cp = c;
	    win_T   *twp;

	    twp = mouse_ਲੱਭੋ_win(&ਸਤਰ_cp, &ਖਾਨਾ_cp, IGNORE_POPUP);
	    ਜੇ (twp != ਨਿਰਾ && twp != prev_twp)
	    {
		prev_twp = twp;
		ਜੇ (ਸਤਰ_cp < twp->w_ਉਚਾਈ)
		{
		    linenr_T lnum;

		    (ਖਾਲੀ)mouse_comp_pos(twp, &ਸਤਰ_cp, &ਖਾਨਾ_cp, &lnum, ਨਿਰਾ);
		    redrawWinline(twp, lnum);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_cp == twp->w_ਉਚਾਈ)
		    twp->w_redr_ਸਥਿਤੀ = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    ਖਾਲੀ
f_popup_move(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    dict_T	*dict;
    ਪੂਰਨ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;  // invalid {id}

    ਜੇ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    ਜੇ (wp->w_winrow + wp->w_ਉਚਾਈ >= cmdline_ਪੰਕਤੀ)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    ਜੇ (popup_area_ਬਦਲਿਆ(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    ਖਾਲੀ
f_popup_setoptions(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    dict_T	*dict;
    ਪੂਰਨ		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    ਪੂਰਨ		need_redraw = FALSE;
    ਪੂਰਨ		need_reposition = FALSE;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;  // invalid {id}

    ਜੇ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (ਖਾਲੀ)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    ਜੇ (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    ਜੇ (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (popup_style_ਬਦਲਿਆ(wp, &old_style))
	need_redraw = TRUE;

    ਜੇ (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    ਜੇ (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_ਨਕਾਬ_refresh = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ਸਤਰਾਂ_valid = 0;
	ਜੇ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    ਜੇ (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_ਨਕਾਬ_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    ਜੇ (popup_area_ਬਦਲਿਆ(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    ਖਾਲੀ
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ਪੂਰਨ		id;
    win_T	*wp;
    ਪੂਰਨ		ਸਿਖਰ_extra;
    ਪੂਰਨ		ਖੱਬਾ_extra;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;  // invalid {id}
    ਸਿਖਰ_extra = popup_ਸਿਖਰ_extra(wp);
    ਖੱਬਾ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_ਤਾਲਾ_ਮਾਪ(&dict->dv_hashtab, 11);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "line", wp->w_winrow + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", wp->w_wincol + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "width", wp->w_ਚੌੜਾਈ + ਖੱਬਾ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "height", wp->w_ਉਚਾਈ + ਸਿਖਰ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "core_line", wp->w_winrow + 1 + ਸਿਖਰ_extra);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "core_col", wp->w_wincol + 1 + ਖੱਬਾ_extra);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "core_width", wp->w_ਚੌੜਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "core_height", wp->w_ਉਚਾਈ);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "scrollbar", wp->w_has_scrollbar);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "firstline", wp->w_topline);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lastline", wp->w_botline - 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    ਖਾਲੀ
f_popup_list(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    FOR_ALL_POPUPWINS(wp)
	list_append_ਸੰਖਿਆ(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	    list_append_ਸੰਖਿਆ(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    ਖਾਲੀ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪੰਕਤੀ = tv_get_ਸੰਖਿਆ(&argvars[0]) - 1;
    ਖਾਨਾ = tv_get_ਸੰਖਿਆ(&argvars[1]) - 1;
    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
    ਜੇ (wp != ਨਿਰਾ && WIN_IS_POPUP(wp))
	rettv->vval.v_ਸੰਖਿਆ = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ਸਥਿਰ ਖਾਲੀ
get_padding_border(dict_T *dict, ਪੂਰਨ *array, ਅੱਖਰ *name)
{
    list_T  *list;
    ਪੂਰਨ	    i;

    ਜੇ (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	ਵਾਪਸ;

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	ਵਾਪਸ;
    }
    ਜੇ (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	ਲਈ (i = 0; i < 4; ++i)
	    list_append_ਸੰਖਿਆ(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ਸਥਿਰ ਖਾਲੀ
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < 4; ++i)
	ਜੇ (wp->w_border_highlight[i] != ਨਿਰਾ)
	    ਰੋਕੋ;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    ਜੇ (i == 4 && !wp->w_border_highlight_isset)
	ਵਾਪਸ;

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	ਵਾਪਸ;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    ਜੇ (i == 4)
	ਵਾਪਸ;
    ਲਈ (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ਸਥਿਰ ਖਾਲੀ
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ਪੂਰਨ	    i;
    char_u  buf[NUMBUFLEN];
    ਪੂਰਨ	    len;

    ਲਈ (i = 0; i < 8; ++i)
	ਜੇ (wp->w_border_char[i] != 0)
	    ਰੋਕੋ;
    ਜੇ (i == 8)
	ਵਾਪਸ;

    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	ਵਾਪਸ;
    }
    ਲਈ (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ਸਥਿਰ ਖਾਲੀ
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    ਜੇ (list != ਨਿਰਾ)
    {
	ਜੇ (dict_ਜੋੜੋ_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	ਨਹੀਂਤਾਂ
	{
	    list_append_ਸੰਖਿਆ(list, wp->w_popup_lnum);
	    list_append_ਸੰਖਿਆ(list, wp->w_popup_mincol);
	    list_append_ਸੰਖਿਆ(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (dict_ਜੋੜੋ_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	ਵਾਪਸ;
    }
    list_append_ਸੰਖਿਆ(list, wp->w_popup_mouse_ਪੰਕਤੀ);
    list_append_ਸੰਖਿਆ(list, wp->w_popup_mouse_mincol);
    list_append_ਸੰਖਿਆ(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    ਖਾਲੀ
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ਪੂਰਨ		id;
    win_T	*wp;
    tabpage_T	*tp;
    ਪੂਰਨ		i;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = ਲੱਭੋ_popup_win(id);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    dict = rettv->vval.v_dict;
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "line", wp->w_wantline);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", wp->w_wantcol);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "minwidth", wp->w_minwidth);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "minheight", wp->w_minheight);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "maxheight", wp->w_maxheight);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "maxwidth", wp->w_maxwidth);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "firstline", wp->w_firstline);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "scrollbar", wp->w_want_scrollbar);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "zindex", wp->w_zindex);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "fixed", wp->w_popup_fixed);
    ਜੇ (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = ਪਾਠ_prop_type_by_id(
		wp->w_popup_prop_win->w_ਬਫਰ,
		wp->w_popup_prop_type);

	ਜੇ (pt != ਨਿਰਾ)
	    dict_ਜੋੜੋ_string(dict, "textprop", pt->pt_name);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "textpropid", wp->w_popup_prop_id);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    ਜੇ (wp->w_popup_imagep != ਨਿਰਾ && wp->w_popup_imagep->img != ਨਿਰਾ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	ਜੇ (dict_ਜੋੜੋ_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#ਜੇ_ਅੰਤ

    dict_ਜੋੜੋ_string(dict, "title", wp->w_popup_title);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "wrap", wp->w_p_wrap);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_ਜੋੜੋ_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_ਜੋੜੋ_string(dict, "highlights", wp->w_p_whl);
    ਜੇ (wp->w_scrollbar_highlight != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    ਜੇ (wp->w_thumb_highlight != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, twp)
	    ਜੇ (twp->w_id == id)
		ਰੋਕੋ;
	ਜੇ (twp != ਨਿਰਾ)
	    ਰੋਕੋ;
	++i;
    }
    ਜੇ (tp == ਨਿਰਾ)
	i = -1;  // must be global
    ਨਹੀਂਤਾਂ ਜੇ (tp == curtab)
	i = 0;
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    ਜੇ (wp->w_filter_cb.cb_name != ਨਿਰਾ)
	dict_ਜੋੜੋ_callback(dict, "filter", &wp->w_filter_cb);
    ਜੇ (wp->w_close_cb.cb_name != ਨਿਰਾ)
	dict_ਜੋੜੋ_callback(dict, "callback", &wp->w_close_cb);

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(poppos_entries); ++i)
	ਜੇ (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_ਜੋੜੋ_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(ਪੂਰਨ)poppos_entries[i].pp_name.ਲੰਮਾਈ);
	    ਰੋਕੋ;
	}

    ਜੇ (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_ਜੋੜੋ_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_close == POPCLOSE_CLICK)
	dict_ਜੋੜੋ_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#ਜੇ defined(FEAT_TIMERS)
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "time", wp->w_popup_timer != ਨਿਰਾ
	    ?  (ਲੰਮਾ)wp->w_popup_timer->tr_interval : 0L);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    ਪੂਰਨ
error_if_ਪਦ_popup_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਜੇ (WIN_IS_POPUP(curwin) && curbuf->b_ਪਦ != ਨਿਰਾ
					   && ਪਦ_ਕੰਮ_running(curbuf->b_ਪਦ))
    {
	emsg(_(e_not_allowed_for_terminal_ਵਿੱਚ_popup_ਖਿੜਕੀ));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    ਖਾਲੀ
popup_reset_handled(ਪੂਰਨ handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
ਲੱਭੋ_ਅਗਲਾ_popup(ਪੂਰਨ ਸਭ_ਤੋਂ_ਨੀਵਾਂ, ਪੂਰਨ handled_flag)
{
    win_T   *wp;
    win_T   *ਮਿਲਿਆ_wp;
    ਪੂਰਨ	    ਮਿਲਿਆ_zindex;

    ਮਿਲਿਆ_zindex = ਸਭ_ਤੋਂ_ਨੀਵਾਂ ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : 0;
    ਮਿਲਿਆ_wp = ਨਿਰਾ;
    FOR_ALL_POPUPWINS(wp)
	ਜੇ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (ਸਭ_ਤੋਂ_ਨੀਵਾਂ ? wp->w_zindex < ਮਿਲਿਆ_zindex
		    : wp->w_zindex > ਮਿਲਿਆ_zindex))
	{
	    ਮਿਲਿਆ_zindex = wp->w_zindex;
	    ਮਿਲਿਆ_wp = wp;
	}
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (ਸਭ_ਤੋਂ_ਨੀਵਾਂ ? wp->w_zindex < ਮਿਲਿਆ_zindex
		    : wp->w_zindex > ਮਿਲਿਆ_zindex))
	{
	    ਮਿਲਿਆ_zindex = wp->w_zindex;
	    ਮਿਲਿਆ_wp = wp;
	}

    ਜੇ (ਮਿਲਿਆ_wp != ਨਿਰਾ)
	ਮਿਲਿਆ_wp->w_popup_handled |= handled_flag;
    ਵਾਪਸ ਮਿਲਿਆ_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ਸਥਿਰ ਪੂਰਨ
invoke_popup_filter(win_T *wp, ਪੂਰਨ c)
{
    ਪੂਰਨ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    ਪੂਰਨ		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    ਜੇ (c == Ctrl_C)
    {
	ਪੂਰਨ save_got_int = got_int;
	ਪੂਰਨ was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	ਜੇ (was_curwin && wp == curwin)
	    ਵਾਪਸ -1;
	ਵਾਪਸ TRUE;
    }

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_ਮੰਜ਼ਿਲ_buf(c, mod_ਨਕਾਬ, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    ਜੇ (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	ਜੇ (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    ਜੇ (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    ਵਾਪਸ res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    ਪੂਰਨ
popup_do_filter(ਪੂਰਨ c)
{
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;
    ਪੂਰਨ		res = FALSE;
    win_T	*wp;
    ਪੂਰਨ		save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    ਪੂਰਨ		state;
    ਪੂਰਨ		was_must_redraw = must_redraw;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    ਜੇ (popup_is_popup(curwin) && curbuf->b_ਪਦ != ਨਿਰਾ)
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE
	    || State == ਬਹੁਲਕ_CONFIRM || State == ਬਹੁਲਕ_EXTERNCMD)
	ਵਾਪਸ FALSE;

    ਜੇ (recursive)
	ਵਾਪਸ FALSE;
    recursive = TRUE;

    ਜੇ (c == K_LEFTMOUSE)
    {
	ਪੂਰਨ ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	ਪੂਰਨ ਖਾਨਾ = mouse_ਖਾਨਾ;

	wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
	ਜੇ (wp != ਨਿਰਾ && popup_close_if_on_X(wp, ਪੰਕਤੀ, ਖਾਨਾ))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    ਜਦੋਂ (res == FALSE
		     && (wp = ਲੱਭੋ_ਅਗਲਾ_popup(FALSE, POPUP_HANDLED_2)) != ਨਿਰਾ)
	ਜੇ (wp->w_filter_cb.cb_name != ਨਿਰਾ
		&& (wp->w_filter_ਬਹੁਲਕ & state) != 0)
	    res = invoke_popup_filter(wp, c);

    ਜੇ (must_redraw > was_must_redraw)
    {
	ਪੂਰਨ save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;

    // When interrupted return FALSE to avoid looping.
    ਵਾਪਸ res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    ਪੂਰਨ
popup_no_mapping(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਗੇੜਾ;
    win_T   *wp;

    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	ਲਈ (wp = ਗੇੜਾ == 1 ? ਪਹਿਲਾ_popupwin : curtab->tp_ਪਹਿਲਾ_popupwin;
						   wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    ਜੇ (wp->w_filter_cb.cb_name != ਨਿਰਾ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    ਖਾਲੀ
popup_check_cursor_pos(ਖਾਲੀ)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    ਜਦੋਂ ((wp = ਲੱਭੋ_ਅਗਲਾ_popup(TRUE, POPUP_HANDLED_3)) != ਨਿਰਾ)
	ਜੇ (wp->w_popup_curwin != ਨਿਰਾ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.ਖਾਨਾ < wp->w_popup_mincol
		    || curwin->w_cursor.ਖਾਨਾ > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ਸਥਿਰ ਖਾਲੀ
popup_update_ਨਕਾਬ(win_T *wp, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;

    ਜੇ (wp->w_popup_ਨਕਾਬ == ਨਿਰਾ || ਚੌੜਾਈ == 0 || ਉਚਾਈ == 0)
    {
	VIM_CLEAR(wp->w_popup_ਨਕਾਬ_cells);
	ਵਾਪਸ;
    }
    ਜੇ (wp->w_popup_ਨਕਾਬ_cells != ਨਿਰਾ
	    && wp->w_popup_ਨਕਾਬ_ਉਚਾਈ == ਉਚਾਈ
	    && wp->w_popup_ਨਕਾਬ_ਚੌੜਾਈ == ਚੌੜਾਈ)
	ਵਾਪਸ;  // cache is still valid

    vim_free(wp->w_popup_ਨਕਾਬ_cells);
    ਜੇ (ਚੌੜਾਈ > 0 && (size_t)ਉਚਾਈ > ਮਾਪ_MAX / (size_t)ਚੌੜਾਈ)
    {
	wp->w_popup_ਨਕਾਬ_cells = ਨਿਰਾ;
	ਵਾਪਸ;
    }
    wp->w_popup_ਨਕਾਬ_cells = alloc_clear((size_t)ਚੌੜਾਈ * ਉਚਾਈ);
    ਜੇ (wp->w_popup_ਨਕਾਬ_cells == ਨਿਰਾ)
	ਵਾਪਸ;
    cells = wp->w_popup_ਨਕਾਬ_cells;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(wp->w_popup_ਨਕਾਬ, lio)
    {
	ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, cole;
	ਪੂਰਨ ਸਤਰਾਂ, linee;

	li = lio->li_tv.vval.v_list->lv_ਪਹਿਲਾ;
	ਖਾਨੇ_ਗਿਣਤੀ = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ < 0)
	    ਖਾਨੇ_ਗਿਣਤੀ = ਚੌੜਾਈ + ਖਾਨੇ_ਗਿਣਤੀ + 1;
	ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ <= 0)
	    ਖਾਨੇ_ਗਿਣਤੀ = 1;
	li = li->li_ਅਗਲਾ;
	cole = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (cole < 0)
	    cole = ਚੌੜਾਈ + cole + 1;
	ਜੇ (cole > ਚੌੜਾਈ)
	    cole = ਚੌੜਾਈ;
	li = li->li_ਅਗਲਾ;
	ਸਤਰਾਂ = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (ਸਤਰਾਂ < 0)
	    ਸਤਰਾਂ = ਉਚਾਈ + ਸਤਰਾਂ + 1;
	ਜੇ (ਸਤਰਾਂ <= 0)
	    ਸਤਰਾਂ = 1;
	li = li->li_ਅਗਲਾ;
	linee = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (linee < 0)
	    linee = ਉਚਾਈ + linee + 1;
	ਜੇ (linee > ਉਚਾਈ)
	    linee = ਉਚਾਈ;

	ਲਈ (ਪੰਕਤੀ = ਸਤਰਾਂ - 1; ਪੰਕਤੀ < linee; ++ਪੰਕਤੀ)
	    ਲਈ (ਖਾਨਾ = ਖਾਨੇ_ਗਿਣਤੀ - 1; ਖਾਨਾ < cole; ++ਖਾਨਾ)
		cells[ਪੰਕਤੀ * ਚੌੜਾਈ + ਖਾਨਾ] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_masked(win_T *wp, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ screencol, ਪੂਰਨ screenline)
{
    ਪੂਰਨ ਖਾਨਾ = screencol - wp->w_wincol + wp->w_popup_leftoff;
    ਪੂਰਨ ਸਤਰ = screenline - wp->w_winrow;

    ਵਾਪਸ ਖਾਨਾ >= 0 && ਖਾਨਾ < ਚੌੜਾਈ
	    && ਸਤਰ >= 0 && ਸਤਰ < ਉਚਾਈ
	    && wp->w_popup_ਨਕਾਬ_cells[ਸਤਰ * ਚੌੜਾਈ + ਖਾਨਾ];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ਸਥਿਰ ਖਾਲੀ
update_popup_transparent(win_T *wp, ਪੂਰਨ val)
{
    ਜੇ (wp->w_popup_ਨਕਾਬ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ		ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp);
    ਪੂਰਨ		ਉਚਾਈ = popup_ਉਚਾਈ(wp);
    listitem_T	*lio, *li;
    ਪੂਰਨ		ਖਾਨੇ_ਗਿਣਤੀ, cole;
    ਪੂਰਨ		ਸਤਰਾਂ, linee;
    ਪੂਰਨ		ਖਾਨਾ, ਸਤਰ;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(wp->w_popup_ਨਕਾਬ, lio)
    {
	li = lio->li_tv.vval.v_list->lv_ਪਹਿਲਾ;
	ਖਾਨੇ_ਗਿਣਤੀ = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ < 0)
	    ਖਾਨੇ_ਗਿਣਤੀ = ਚੌੜਾਈ + ਖਾਨੇ_ਗਿਣਤੀ + 1;
	li = li->li_ਅਗਲਾ;
	cole = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (cole < 0)
	    cole = ਚੌੜਾਈ + cole + 1;
	li = li->li_ਅਗਲਾ;
	ਸਤਰਾਂ = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (ਸਤਰਾਂ < 0)
	    ਸਤਰਾਂ = ਉਚਾਈ + ਸਤਰਾਂ + 1;
	li = li->li_ਅਗਲਾ;
	linee = tv_get_ਸੰਖਿਆ(&li->li_tv);
	ਜੇ (linee < 0)
	    linee = ਉਚਾਈ + linee + 1;

	--ਖਾਨੇ_ਗਿਣਤੀ;
	ਖਾਨੇ_ਗਿਣਤੀ -= wp->w_popup_leftoff;
	ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ < 0)
	    ਖਾਨੇ_ਗਿਣਤੀ = 0;
	cole -= wp->w_popup_leftoff;
	--ਸਤਰਾਂ;
	ਜੇ (ਸਤਰਾਂ < 0)
	    ਸਤਰਾਂ = 0;
	ਲਈ (ਸਤਰ = ਸਤਰਾਂ; ਸਤਰ < linee
		&& ਸਤਰ + wp->w_winrow < screen_ਪੰਕਤੀਆਂ; ++ਸਤਰ)
	    ਲਈ (ਖਾਨਾ = ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ < cole
		    && ਖਾਨਾ + wp->w_wincol < screen_Columns; ++ਖਾਨਾ)
		popup_transparent[(ਸਤਰ + wp->w_winrow) * screen_Columns
		    + ਖਾਨਾ + wp->w_wincol] = val;
    }
}

/*
 * Only called when popup window "wp" is hidden: If the window is positioned
 * next to a text property, and it is now visible, then  unhide the popup.
 * We don't check if visible popups become hidden, that is done in
 * popup_adjust_position().
 * Return TRUE if the popup became unhidden.
 */
    ਸਥਿਰ ਪੂਰਨ
check_popup_unhidden(win_T *wp)
{
    ਜੇ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    ਮਿਲਿਆ = false;

	ਜੇ ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    ਵਾਪਸ FALSE;
	ਜੇ (ਲੱਭੋ_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    ਮਿਲਿਆ = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	ਨਹੀਂਤਾਂ ਜੇ (popup_ਲੱਭੋ_prop_above_ਸਿਖਰ(wp, wp->w_popup_prop_win,
					    popup_ਉਚਾਈ(wp), &prop, &lnum))
	    ਮਿਲਿਆ = true;
	ਜੇ (ਮਿਲਿਆ)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_need_position_adjust(win_T *wp)
{
    ਜੇ (wp->w_popup_ਆਖਰੀ_changedtick != CHANGEDTICK(wp->w_ਬਫਰ))
	ਵਾਪਸ TRUE;
    ਜੇ (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_ਬਫਰ)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_ਚੌੜਾਈ != wp->w_popup_prop_win->w_ਚੌੜਾਈ
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_ਉਚਾਈ))
	ਵਾਪਸ TRUE;

    // May need to adjust the width if the cursor moved.
    ਵਾਪਸ wp->w_cursor.lnum != wp->w_popup_ਆਖਰੀ_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ਸਥਿਰ ਛੋਟਾ *opacity_zindex = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ    opacity_zindex_ਪੰਕਤੀਆਂ = 0;
ਸਥਿਰ ਪੂਰਨ    opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_mark_opacity_zindex(win_T *wp)
{
    ਪੂਰਨ	    ਚੌੜਾਈ;
    ਪੂਰਨ	    ਉਚਾਈ;
    ਪੂਰਨ	    r, c;

    ਜੇ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ਵਾਪਸ;

    ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp);
    ਉਚਾਈ = popup_ਉਚਾਈ(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    ਲਈ (r = wp->w_winrow;
		       r < wp->w_winrow + ਉਚਾਈ - wp->w_popup_topoff
						&& r < screen_ਪੰਕਤੀਆਂ; ++r)
	ਲਈ (c = wp->w_wincol;
		 c < wp->w_wincol + ਚੌੜਾਈ - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    ਪੂਰਨ off = r * screen_Columns + c;
	    ਜੇ (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_win_under_opacity_popup(win_T *wp)
{
    ਪੂਰਨ	    ਉਚਾਈ;
    ਪੂਰਨ	    ਚੌੜਾਈ;
    ਪੂਰਨ	    r;

    ਜੇ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ਵਾਪਸ;

    ਉਚਾਈ = popup_ਉਚਾਈ(wp);
    ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp);
    ਲਈ (r = wp->w_winrow;
		       r < wp->w_winrow + ਉਚਾਈ && r < screen_ਪੰਕਤੀਆਂ; ++r)
    {
	ਪੂਰਨ	    ਖਾਨਾ;
	win_T	    *prev_twp = ਨਿਰਾ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	ਲਈ (ਖਾਨਾ = wp->w_wincol;
		ਖਾਨਾ < wp->w_wincol + ਚੌੜਾਈ && ਖਾਨਾ < screen_Columns; ++ਖਾਨਾ)
	{
	    ਪੂਰਨ	    ਸਤਰ_cp = r;
	    ਪੂਰਨ	    ਖਾਨਾ_cp = ਖਾਨਾ;
	    win_T   *twp;

	    twp = mouse_ਲੱਭੋ_win(&ਸਤਰ_cp, &ਖਾਨਾ_cp, IGNORE_POPUP);
	    ਜੇ (twp != ਨਿਰਾ && twp != prev_twp)
	    {
		prev_twp = twp;
		ਜੇ (ਸਤਰ_cp < twp->w_ਉਚਾਈ)
		{
		    linenr_T lnum;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    ਜੇ (ਪਦ_do_update_ਖਿੜਕੀ(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    {
			(ਖਾਲੀ)mouse_comp_pos(twp, &ਸਤਰ_cp, &ਖਾਨਾ_cp, &lnum,
									ਨਿਰਾ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_cp == twp->w_ਉਚਾਈ)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_ਸਥਿਤੀ = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    ਪੂਰਨ
popup_is_under_opacity(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (opacity_zindex == ਨਿਰਾ
	    || ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= opacity_zindex_ਪੰਕਤੀਆਂ
	    || ਖਾਨਾ < 0 || ਖਾਨਾ >= opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ opacity_zindex[ਪੰਕਤੀ * opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    ਪੂਰਨ
popup_is_over_opacity(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ਜੇ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ਪੰਕਤੀ >= wp->w_winrow
		&& ਪੰਕਤੀ < wp->w_winrow + popup_ਉਚਾਈ(wp)
		&& ਖਾਨਾ >= wp->w_wincol
		&& ਖਾਨਾ < wp->w_wincol + popup_ਚੌੜਾਈ(wp))
	    ਵਾਪਸ TRUE;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ਪੰਕਤੀ >= wp->w_winrow
		&& ਪੰਕਤੀ < wp->w_winrow + popup_ਉਚਾਈ(wp)
		&& ਖਾਨਾ >= wp->w_wincol
		&& ਖਾਨਾ < wp->w_wincol + popup_ਚੌੜਾਈ(wp))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    ਪੂਰਨ
popup_is_under_opacity_range(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ)
{
    ਪੂਰਨ ਖਾਨਾ;

    ਜੇ (opacity_zindex == ਨਿਰਾ
	    || ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= opacity_zindex_ਪੰਕਤੀਆਂ)
	ਵਾਪਸ FALSE;
    ਲਈ (ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ < ਸਿਰਾ_ਖਾਨਾ && ਖਾਨਾ < opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ; ++ਖਾਨਾ)
	ਜੇ (opacity_zindex[ਪੰਕਤੀ * opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ] > screen_zindex)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    ਖਾਲੀ
may_update_popup_ਨਕਾਬ(ਪੂਰਨ type)
{
    win_T	*wp;
    ਛੋਟਾ	*ਨਕਾਬ;
    ਪੂਰਨ		ਸਤਰ, ਖਾਨਾ;
    ਪੂਰਨ		redraw_all_popups = FALSE;
    ਪੂਰਨ		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    ਜੇ (popup_ਨਕਾਬ_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_ਨਕਾਬ_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ਨਕਾਬ_refresh |= check_popup_unhidden(wp);
	ਨਹੀਂਤਾਂ ਜੇ (popup_need_position_adjust(wp))
	    popup_ਨਕਾਬ_refresh = TRUE;
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ਨਕਾਬ_refresh |= check_popup_unhidden(wp);
	ਨਹੀਂਤਾਂ ਜੇ (popup_need_position_adjust(wp))
	    popup_ਨਕਾਬ_refresh = TRUE;

    // Force background windows to redraw rows under opacity popups.
    // Opacity popups don't participate in popup_mask, so their area
    // wouldn't normally be redrawn.  Without this, ScreenAttrs retains
    // blended values from the previous cycle, causing blend accumulation.
    // This must run every cycle, not just when popup_mask_refresh is set.
    //
    // Also build the opacity_zindex array used by screen_char() to suppress
    // output for cells under opacity popups during background draw.
    {
	ਪੂਰਨ has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ਜੇ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ਜੇ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	ਜੇ (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_ਪੰਕਤੀਆਂ = 0;
	    opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (opacity_zindex_ਪੰਕਤੀਆਂ != screen_ਪੰਕਤੀਆਂ
		    || opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(ਛੋਟਾ,
					screen_ਪੰਕਤੀਆਂ * screen_Columns);
		opacity_zindex_ਪੰਕਤੀਆਂ = screen_ਪੰਕਤੀਆਂ;
		opacity_zindex_ਖਾਨੇ_ਗਿਣਤੀ = screen_Columns;
	    }
	    ਜੇ (opacity_zindex != ਨਿਰਾ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_ਪੰਕਤੀਆਂ * screen_Columns * ਆਕਾਰ(ਛੋਟਾ));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    ਜੇ (!popup_ਨਕਾਬ_refresh)
	ਵਾਪਸ;

    // Need to update the mask, something has changed.
    popup_ਨਕਾਬ_refresh = FALSE;
    popup_ਨਕਾਬ_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    ਜੇ (redrawing_all_win)
	ਨਕਾਬ = popup_ਨਕਾਬ;
    ਨਹੀਂਤਾਂ
	ਨਕਾਬ = popup_ਨਕਾਬ_ਅਗਲਾ;
    vim_memset(ਨਕਾਬ, 0, (size_t)screen_ਪੰਕਤੀਆਂ * screen_Columns * ਆਕਾਰ(ਛੋਟਾ));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    ਜਦੋਂ ((wp = ਲੱਭੋ_ਅਗਲਾ_popup(TRUE, POPUP_HANDLED_4)) != ਨਿਰਾ)
    {
	ਪੂਰਨ ਚੌੜਾਈ;
	ਪੂਰਨ ਉਚਾਈ;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	ਜੇ (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    ਜੇ (wp->w_popup_flags & POPF_HIDDEN)
		ਜਾਰੀ;
	}

	ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	ਜੇ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    ਉਚਾਈ = cl.eff_ਉਚਾਈ;
	}
	ਨਹੀਂਤਾਂ
	    ਉਚਾਈ = popup_ਉਚਾਈ(wp);
	popup_update_ਨਕਾਬ(wp, ਚੌੜਾਈ, ਉਚਾਈ);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	ਜੇ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ਜਾਰੀ;

	{
	    ਪੂਰਨ ਨਕਾਬ_ਸ਼ੁਰੂ = wp->w_winrow;
	    ਪੂਰਨ ਨਕਾਬ_ਸਿਰਾ = ਨਕਾਬ_ਸ਼ੁਰੂ + ਉਚਾਈ;
	    ਪੂਰਨ ਨਕਾਬ_ਖਾਨਾ_ਸ਼ੁਰੂ = wp->w_wincol + wp->w_popup_leftclip;
	    ਪੂਰਨ ਨਕਾਬ_ਖਾਨਾ_ਸਿਰਾ = wp->w_wincol + ਚੌੜਾਈ - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    ਲਈ (ਸਤਰ = ਨਕਾਬ_ਸ਼ੁਰੂ;
			    ਸਤਰ < ਨਕਾਬ_ਸਿਰਾ && ਸਤਰ < screen_ਪੰਕਤੀਆਂ; ++ਸਤਰ)
		ਲਈ (ਖਾਨਾ = ਨਕਾਬ_ਖਾਨਾ_ਸ਼ੁਰੂ;
		     ਖਾਨਾ < ਨਕਾਬ_ਖਾਨਾ_ਸਿਰਾ && ਖਾਨਾ < screen_Columns; ++ਖਾਨਾ)
		    ਜੇ (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(ਸਤਰ, ਖਾਨਾ, FALSE))
			ਨਕਾਬ[ਸਤਰ * screen_Columns + ਖਾਨਾ] = POPUPMENU_ZINDEX;
		    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_ਨਕਾਬ_cells == ਨਿਰਾ
				    || !popup_masked(wp, ਚੌੜਾਈ, ਉਚਾਈ, ਖਾਨਾ, ਸਤਰ))
			ਨਕਾਬ[ਸਤਰ * screen_Columns + ਖਾਨਾ] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    ਜੇ (ਨਕਾਬ == popup_ਨਕਾਬ_ਅਗਲਾ)
    {
	ਪੂਰਨ	    *plines_cache = ALLOC_CLEAR_MULT(ਪੂਰਨ, ਪੰਕਤੀਆਂ);
	win_T	    *prev_wp = ਨਿਰਾ;

	ਲਈ (ਸਤਰ = 0; ਸਤਰ < screen_ਪੰਕਤੀਆਂ; ++ਸਤਰ)
	{
	    ਪੂਰਨ	    ਖਾਨਾ_ਪੂਰਾ = 0;

	    ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < screen_Columns; ++ਖਾਨਾ)
	    {
		ਪੂਰਨ off = ਸਤਰ * screen_Columns + ਖਾਨਾ;

		ਜੇ (popup_ਨਕਾਬ[off] != popup_ਨਕਾਬ_ਅਗਲਾ[off])
		{
		    popup_ਨਕਾਬ[off] = popup_ਨਕਾਬ_ਅਗਲਾ[off];

		    ਜੇ (ਸਤਰ >= cmdline_ਪੰਕਤੀ)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			ਜੇ (!ਸੁਨੇਹਾ_scrolled && popup_ਨਕਾਬ_ਅਗਲਾ[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨਾ >= ਖਾਨਾ_ਪੂਰਾ)
		    {
			linenr_T	lnum;
			ਪੂਰਨ		ਸਤਰ_cp = ਸਤਰ;
			ਪੂਰਨ		ਖਾਨਾ_cp = ਖਾਨਾ;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_ਲੱਭੋ_win(&ਸਤਰ_cp, &ਖਾਨਾ_cp, IGNORE_POPUP);
			ਜੇ (wp != ਨਿਰਾ)
			{
#ਜੇ defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    ਜੇ (bt_terminal(wp->w_ਬਫਰ))
				redraw_win_later(wp, UPD_NOT_VALID);
			    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			    {
				ਜੇ (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   ਆਕਾਰ(ਪੂਰਨ) * ਪੰਕਤੀਆਂ);
				    prev_wp = wp;
				}

				ਜੇ (ਸਤਰ_cp >= wp->w_ਉਚਾਈ)
				    // In (or below) status line
				    wp->w_redr_ਸਥਿਤੀ = true;
				ਨਹੀਂਤਾਂ
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (ਖਾਲੀ)mouse_comp_pos(wp, &ਸਤਰ_cp, &ਖਾਨਾ_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    ਖਾਨਾ_ਪੂਰਾ = wp->w_wincol + wp->w_ਚੌੜਾਈ - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_ਵਰਤਦਾ_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    ਖਾਲੀ
may_update_popup_position(ਖਾਲੀ)
{
    ਜੇ (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
ਸਥਿਰ schar_T *ਅਧਾਰ_screenlines = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ *ਅਧਾਰ_screenattrs = ਨਿਰਾ;
ਸਥਿਰ u8char_T *ਅਧਾਰ_screenlinesuc = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ = 0;
ਸਥਿਰ ਪੂਰਨ ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    ਪੂਰਨ
popup_get_ਅਧਾਰ_screen_cell(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, schar_T *linep, ਪੂਰਨ *attrp,
							  u8char_T *ucp)
{
    ਜੇ (ਅਧਾਰ_screenlines == ਨਿਰਾ || ਅਧਾਰ_screenattrs == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0 || ਪੰਕਤੀ >= ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ
						     || ਖਾਨਾ >= ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ)
	ਵਾਪਸ FALSE;

    ਪੂਰਨ off = ਪੰਕਤੀ * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ;
    ਜੇ (linep != ਨਿਰਾ)
	*linep = ਅਧਾਰ_screenlines[off];
    ਜੇ (attrp != ਨਿਰਾ)
	*attrp = ਅਧਾਰ_screenattrs[off];
    ਜੇ (ucp != ਨਿਰਾ)
    {
	ਜੇ (enc_utf8 && ਅਧਾਰ_screenlinesuc != ਨਿਰਾ)
	    *ucp = ਅਧਾਰ_screenlinesuc[off];
	ਨਹੀਂਤਾਂ
	    *ucp = 0;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    ਖਾਲੀ
popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, schar_T ਸਤਰ, ਪੂਰਨ attr, u8char_T uc)
{
    ਜੇ (ਅਧਾਰ_screenlines == ਨਿਰਾ || ਅਧਾਰ_screenattrs == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0 || ਪੰਕਤੀ >= ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ
						     || ਖਾਨਾ >= ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    ਪੂਰਨ off = ਪੰਕਤੀ * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ;
    ਅਧਾਰ_screenlines[off] = ਸਤਰ;
    ਅਧਾਰ_screenattrs[off] = attr;
    ਜੇ (enc_utf8 && ਅਧਾਰ_screenlinesuc != ਨਿਰਾ)
	ਅਧਾਰ_screenlinesuc[off] = uc;
}
#ਜੇ_ਅੰਤ

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ਸਥਿਰ ਖਾਲੀ
draw_opacity_padding_cell(
	ਪੂਰਨ		ਪੰਕਤੀ,
	ਪੂਰਨ		ਖਾਨਾ,
	popup_saved_screen_T *saved_screen,
	ਪੂਰਨ		pad_ਸ਼ੁਰੂ_ਖਾਨਾ,
	ਪੂਰਨ		pad_ਸਿਰਾ_ਖਾਨਾ)
{
    ਜੇ (ਸਤਰ_ਵਿੱਥ == ਨਿਰਾ || Screen_ਸਤਰਾਂ == ਨਿਰਾ || ScreenAttrs == ਨਿਰਾ
	    || (enc_utf8 && Screen_ਸਤਰਾਂ_UC == ਨਿਰਾ))
	ਵਾਪਸ;

    ਪੂਰਨ off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ;
    ਪੂਰਨ r = ਪੰਕਤੀ - saved_screen->ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    ਪੂਰਨ c = ਖਾਨਾ - saved_screen->ਸ਼ੁਰੂ_ਖਾਨਾ;

    ਜੇ (r >= 0 && r < saved_screen->ਪੰਕਤੀਆਂ_2 && c >= 0 && c < saved_screen->ਖਾਨੇ_ਗਿਣਤੀ)
    {
	ਪੂਰਨ save_off = r * saved_screen->ਖਾਨੇ_ਗਿਣਤੀ + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	ਜੇ (enc_utf8 && saved_screen->linesuc != ਨਿਰਾ)
	{
	    ਪੂਰਨ ਅਧਾਰ_ਖਾਨਾ = ਖਾਨਾ - 1;
	    ਪੂਰਨ ਅਧਾਰ_off = off - 1;
	    ਪੂਰਨ ਅਧਾਰ_save_off = save_off - 1;
	    ਪੂਰਨ wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    ਜੇ (ਅਧਾਰ_off >= 0)
	    {
		ਜੇ (Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] != 0
			&& utf_char2cells(Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off]) == 2
			&& Screen_ਸਤਰਾਂ[off] == 0)
		    wide_prev = TRUE;
	    }
	    ਜੇ (!wide_prev && save_off > 0)
	    {
		ਜੇ (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->ਸਤਰਾਂ[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    ਜੇ (wide_prev && ਅਧਾਰ_ਖਾਨਾ >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		ਜੇ (ਅਧਾਰ_ਖਾਨਾ < pad_ਸ਼ੁਰੂ_ਖਾਨਾ)
		{
		    ਜੇ (Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] != 0
			    && utf_char2cells(Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_ਸਤਰਾਂ[ਅਧਾਰ_off] = ' ';
			Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] = 0;
			ScreenAttrs[ਅਧਾਰ_off] = saved_screen->attrs[ਅਧਾਰ_save_off];
			popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ,
				Screen_ਸਤਰਾਂ[ਅਧਾਰ_off],
				ScreenAttrs[ਅਧਾਰ_off],
				Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off]);
			screen_char(ਅਧਾਰ_off, ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_ਸਤਰਾਂ[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[ਅਧਾਰ_save_off];
			ਜੇ (enc_utf8)
			    Screen_ਸਤਰਾਂ_UC[off] = 0;
			ਪੂਰਨ popup_attr_val =
					get_win_attr(screen_opacity_popup);
			ਪੂਰਨ blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ,
				Screen_ਸਤਰਾਂ[off], ScreenAttrs[off],
				Screen_ਸਤਰਾਂ_UC[off]);
			screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
			ਵਾਪਸ;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    ਜੇ (ਅਧਾਰ_save_off >= 0
			    && saved_screen->linesuc[ਅਧਾਰ_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[ਅਧਾਰ_save_off]) == 2
			    && Screen_ਸਤਰਾਂ[ਅਧਾਰ_off] == ' '
			    && Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] == 0)
		    {
			ਪੂਰਨ popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			ਪੂਰਨ blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_ਸਤਰਾਂ[ਅਧਾਰ_off] =
				    saved_screen->ਸਤਰਾਂ[ਅਧਾਰ_save_off];
			Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] =
				    saved_screen->linesuc[ਅਧਾਰ_save_off];
			ScreenAttrs[ਅਧਾਰ_off] =
				    saved_screen->attrs[ਅਧਾਰ_save_off];
			ScreenAttrs[ਅਧਾਰ_off] = hl_blend_attr(
				    ScreenAttrs[ਅਧਾਰ_off],
				    popup_attr_val, blend, TRUE);

			Screen_ਸਤਰਾਂ[off] = 0;
			Screen_ਸਤਰਾਂ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[ਅਧਾਰ_off];

			popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ,
				    Screen_ਸਤਰਾਂ[ਅਧਾਰ_off],
				    ScreenAttrs[ਅਧਾਰ_off],
				    Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off]);
			popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ,
				    Screen_ਸਤਰਾਂ[off],
				    ScreenAttrs[off],
				    Screen_ਸਤਰਾਂ_UC[off]);
			screen_char(ਅਧਾਰ_off, ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ);
			ਵਾਪਸ;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_ਸਤਰਾਂ[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[ਅਧਾਰ_save_off];
		    ਜੇ (enc_utf8)
			Screen_ਸਤਰਾਂ_UC[off] = 0;
		    ਪੂਰਨ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ਪੂਰਨ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ, Screen_ਸਤਰਾਂ[off],
					       ScreenAttrs[off], Screen_ਸਤਰਾਂ_UC[off]);
		    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
		    ਵਾਪਸ;
		}

		// Base cell is inside the saved area, redraw the wide char.
		ਜੇ (save_off > 0)
		{
		    Screen_ਸਤਰਾਂ[ਅਧਾਰ_off] = saved_screen->ਸਤਰਾਂ[ਅਧਾਰ_save_off];
		    ScreenAttrs[ਅਧਾਰ_off] = saved_screen->attrs[ਅਧਾਰ_save_off];
		    Screen_ਸਤਰਾਂ[off] = saved_screen->ਸਤਰਾਂ[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off] = saved_screen->linesuc[ਅਧਾਰ_save_off];
		    Screen_ਸਤਰਾਂ_UC[off] = saved_screen->linesuc[save_off];

		    ਪੂਰਨ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ਪੂਰਨ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[ਅਧਾਰ_off] = hl_blend_attr(ScreenAttrs[ਅਧਾਰ_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[ਅਧਾਰ_off];
		    popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ, Screen_ਸਤਰਾਂ[ਅਧਾਰ_off],
					       ScreenAttrs[ਅਧਾਰ_off], Screen_ਸਤਰਾਂ_UC[ਅਧਾਰ_off]);
		    popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ, Screen_ਸਤਰਾਂ[off],
					       ScreenAttrs[off], Screen_ਸਤਰਾਂ_UC[off]);
		    screen_char(ਅਧਾਰ_off, ਪੰਕਤੀ, ਅਧਾਰ_ਖਾਨਾ);
		}
		ਵਾਪਸ;
	    }
	}
	Screen_ਸਤਰਾਂ[off] = saved_screen->ਸਤਰਾਂ[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	ਜੇ (enc_utf8)
	    Screen_ਸਤਰਾਂ_UC[off] = saved_screen->linesuc != ਨਿਰਾ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
		&& utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) == 2
		&& ਖਾਨਾ + 1 >= pad_ਸਿਰਾ_ਖਾਨਾ)
	{
	    Screen_ਸਤਰਾਂ[off] = ' ';
	    Screen_ਸਤਰਾਂ_UC[off] = 0;
	}

	ਪੂਰਨ popup_attr_val = get_win_attr(screen_opacity_popup);
	ਪੂਰਨ blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_ਸੈੱਟ_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ, Screen_ਸਤਰਾਂ[off],
				   ScreenAttrs[off], Screen_ਸਤਰਾਂ_UC[off]);
	screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ਸਥਿਰ ਖਾਲੀ
fill_opacity_padding(
	ਪੂਰਨ		ਸ਼ੁਰੂ_ਪੰਕਤੀ,
	ਪੂਰਨ		ਸਿਰਾ_ਪੰਕਤੀ,
	ਪੂਰਨ		ਸ਼ੁਰੂ_ਖਾਨਾ,
	ਪੂਰਨ		ਸਿਰਾ_ਖਾਨਾ,
	popup_saved_screen_T *saved_screen)
{
    ਲਈ (ਪੂਰਨ pad_ਪੰਕਤੀ = ਸ਼ੁਰੂ_ਪੰਕਤੀ; pad_ਪੰਕਤੀ < ਸਿਰਾ_ਪੰਕਤੀ; pad_ਪੰਕਤੀ++)
	ਲਈ (ਪੂਰਨ pad_ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ; pad_ਖਾਨਾ < ਸਿਰਾ_ਖਾਨਾ; pad_ਖਾਨਾ++)
	    draw_opacity_padding_cell(pad_ਪੰਕਤੀ, pad_ਖਾਨਾ, saved_screen,
		    ਸ਼ੁਰੂ_ਖਾਨਾ, ਸਿਰਾ_ਖਾਨਾ);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    ਖਾਲੀ
update_popups(ਖਾਲੀ (*win_update)(win_T *wp))
{
    win_T   *wp;
    ਪੂਰਨ	    ਸਿਖਰ_off;
    ਪੂਰਨ	    ਖੱਬਾ_extra;
    ਪੂਰਨ	    ਕੁੱਲ_ਚੌੜਾਈ;
    ਪੂਰਨ	    ਕੁੱਲ_ਉਚਾਈ_2;
    ਪੂਰਨ	    ਸਿਖਰ_padding;
    ਪੂਰਨ	    popup_attr;
    ਪੂਰਨ	    border_attr[4];
    ਪੂਰਨ	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    ਪੂਰਨ	    ਪੰਕਤੀ;
    ਪੂਰਨ	    wincol;
    ਪੂਰਨ	    padcol = 0;
    ਪੂਰਨ	    padendcol = 0;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    sb_thumb_ਸਿਖਰ = 0;
    ਪੂਰਨ	    sb_thumb_ਉਚਾਈ = 0;
    ਪੂਰਨ	    attr_scroll = 0;
    ਪੂਰਨ	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (ਅਧਾਰ_screenlines != ਨਿਰਾ)
    {
	vim_free(ਅਧਾਰ_screenlines);
	ਅਧਾਰ_screenlines = ਨਿਰਾ;
    }
    ਜੇ (ਅਧਾਰ_screenattrs != ਨਿਰਾ)
    {
	vim_free(ਅਧਾਰ_screenattrs);
	ਅਧਾਰ_screenattrs = ਨਿਰਾ;
    }
    ਜੇ (ਅਧਾਰ_screenlinesuc != ਨਿਰਾ)
    {
	vim_free(ਅਧਾਰ_screenlinesuc);
	ਅਧਾਰ_screenlinesuc = ਨਿਰਾ;
    }
    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ = 0;
    ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ ((wp = ਲੱਭੋ_ਅਗਲਾ_popup(TRUE, POPUP_HANDLED_5)) != ਨਿਰਾ)
    {
	ਪੂਰਨ	    title_len = 0;
	ਪੂਰਨ	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	ਜੇ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	ਨਹੀਂਤਾਂ
	    screen_opacity_popup = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (screen_opacity_popup != ਨਿਰਾ)
	{
	    ਜੇ (ਅਧਾਰ_screenlines != ਨਿਰਾ)
	    {
		vim_free(ਅਧਾਰ_screenlines);
		ਅਧਾਰ_screenlines = ਨਿਰਾ;
	    }
	    ਜੇ (ਅਧਾਰ_screenattrs != ਨਿਰਾ)
	    {
		vim_free(ਅਧਾਰ_screenattrs);
		ਅਧਾਰ_screenattrs = ਨਿਰਾ;
	    }
	    ਜੇ (ਅਧਾਰ_screenlinesuc != ਨਿਰਾ)
	    {
		vim_free(ਅਧਾਰ_screenlinesuc);
		ਅਧਾਰ_screenlinesuc = ਨਿਰਾ;
	    }

	    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ = screen_ਪੰਕਤੀਆਂ;
	    ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ = screen_Columns;
	    ਅਧਾਰ_screenlines = ALLOC_MULT(schar_T,
				    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ);
	    ਅਧਾਰ_screenattrs = ALLOC_MULT(ਪੂਰਨ,
				    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ);
	    ਜੇ (enc_utf8)
		ਅਧਾਰ_screenlinesuc = ALLOC_MULT(u8char_T,
				    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ);

	    ਜੇ (ਅਧਾਰ_screenlines != ਨਿਰਾ && ਅਧਾਰ_screenattrs != ਨਿਰਾ)
	    {
		ਲਈ (ਪੂਰਨ r = 0; r < ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ; r++)
		{
		    ਪੂਰਨ off = ਸਤਰ_ਵਿੱਥ[r];
		    ਪੂਰਨ ਅਧਾਰ_off = r * ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ;
		    ਲਈ (ਪੂਰਨ c = 0; c < ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ; c++)
		    {
			ਅਧਾਰ_screenlines[ਅਧਾਰ_off + c] = Screen_ਸਤਰਾਂ[off + c];
			ਅਧਾਰ_screenattrs[ਅਧਾਰ_off + c] = ScreenAttrs[off + c];
			ਜੇ (enc_utf8 && ਅਧਾਰ_screenlinesuc != ਨਿਰਾ)
			    ਅਧਾਰ_screenlinesuc[ਅਧਾਰ_off + c] =
					Screen_ਸਤਰਾਂ_UC[off + c];
		    }
		}
	    }
	}
#ਜੇ_ਅੰਤ

	// Save background ScreenLines for padding opacity before win_update()
	// overwrites them.
	popup_saved_screen_T saved_screen = { 0 };

	popup_save_padding_screen(wp, &saved_screen);

	// Set flags in popup_transparent[] for masked cells.
	update_popup_transparent(wp, 1);

	// adjust w_winrow and w_wincol for border and padding, since
	// win_update() doesn't handle them.
	ਸਿਖਰ_off = popup_ਸਿਖਰ_extra(wp);
	ਖੱਬਾ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	ਜੇ (wp->w_wincol + ਖੱਬਾ_extra < 0)
	    ਖੱਬਾ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(ਸਿਖਰ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ਖੱਬਾ_extra;

	// Draw the popup text, unless it's off screen.
	ਜੇ (wp->w_winrow < screen_ਪੰਕਤੀਆਂ && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    ਜੇ (wp->w_popup_ਆਖਰੀ_curline != wp->w_cursor.lnum)
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
	    ਜੇ (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(ਸਿਖਰ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ਖੱਬਾ_extra;

	// Add offset for border and padding if not done already.
	ਜੇ ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ਖੱਬਾ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	ਜੇ ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += ਸਿਖਰ_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	ਜੇ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    ਕੁੱਲ_ਚੌੜਾਈ = cl.eff_ਚੌੜਾਈ;
	ਨਹੀਂਤਾਂ
	    ਕੁੱਲ_ਚੌੜਾਈ = popup_ਚੌੜਾਈ(wp) - wp->w_popup_rightoff;
	ਜੇ (ਕੁੱਲ_ਚੌੜਾਈ < 0)
	    ਕੁੱਲ_ਚੌੜਾਈ = 0;
	ਜੇ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    ਕੁੱਲ_ਉਚਾਈ_2 = cl.eff_ਉਚਾਈ;
	ਨਹੀਂਤਾਂ
	    ਕੁੱਲ_ਉਚਾਈ_2 = popup_ਉਚਾਈ(wp);
	popup_attr = get_win_attr(wp);

	ਜੇ (wp->w_winrow + ਕੁੱਲ_ਉਚਾਈ_2 > cmdline_ਪੰਕਤੀ)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	ਨਹੀਂਤਾਂ
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	ਜੇ (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	ਨਹੀਂਤਾਂ
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    ਲਈ (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    ਜੇ (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	ਲਈ (i = 0; i < 8; ++i)
	    ਜੇ (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	ਲਈ (i = 0; i < 4; ++i)
	{
	    ਜੇ (wp->w_border_highlight[i] != ਨਿਰਾ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    ਨਹੀਂਤਾਂ
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    ਜੇ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	ਜੇ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	ਜੇ (wp->w_popup_title != ਨਿਰਾ)
	{
	    ਪੂਰਨ	    title_attr;

	    ਜੇ (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ਨਿਰਾ)
		title_attr = border_attr[0];
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    ਨਹੀਂਤਾਂ
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    ਜੇ (title_len > ਕੁੱਲ_ਚੌੜਾਈ - 2)
	    {
		ਪੂਰਨ	title_byte_len = (ਪੂਰਨ)STRLEN(wp->w_popup_title);
		char_u  *title_ਪਾਠ = alloc(title_byte_len + 1);

		ਜੇ (title_ਪਾਠ != ਨਿਰਾ)
		{
		    trunc_string(wp->w_popup_title, title_ਪਾਠ,
					  ਕੁੱਲ_ਚੌੜਾਈ - 2, title_byte_len + 1);
		    screen_puts(title_ਪਾਠ, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_ਪਾਠ);
		}

		title_len = ਕੁੱਲ_ਚੌੜਾਈ - 2;
	    }
	    ਨਹੀਂਤਾਂ
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	ਸਿਖਰ_padding = cl.eff_padding[0];
	ਜੇ (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    ਜੇ (title_len > 0)
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, title_wincol,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			title_wincol + title_len, wincol + ਕੁੱਲ_ਚੌੜਾਈ,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + ਕੁੱਲ_ਚੌੜਾਈ,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    ਜੇ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + ਕੁੱਲ_ਚੌੜਾਈ - 1, border_attr[1]);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cl.eff_padding[0] == 0 && popup_ਸਿਖਰ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    ਸਿਖਰ_padding = 1;

	ਜੇ (ਸਿਖਰ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + ਕੁੱਲ_ਚੌੜਾਈ - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    ਜੇ (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	ਜੇ (ਸਿਖਰ_padding > 0)
	{
	    ਪੰਕਤੀ = wp->w_winrow + cl.eff_border[0];
	    ਜੇ (title_len > 0 && ਪੰਕਤੀ == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		ਜੇ (screen_opacity_popup != ਨਿਰਾ && saved_screen.ਸਤਰਾਂ != ਨਿਰਾ)
		{
		    // Left of title
		    fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		ਨਹੀਂਤਾਂ
		{
		    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		ਪੰਕਤੀ += 1;
		ਸਿਖਰ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    ਜੇ (screen_opacity_popup != ਨਿਰਾ && saved_screen.ਸਤਰਾਂ != ਨਿਰਾ)
	    {
		fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + ਸਿਖਰ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + ਸਿਖਰ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	ਜੇ (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਪੂਰਨ		ਉਚਾਈ = wp->w_ਉਚਾਈ;
	    ਪੂਰਨ		ਆਖਰੀ;

	    sb_thumb_ਉਚਾਈ = ((linenr_T)ਉਚਾਈ * ਉਚਾਈ + linecount / 2)
								   / linecount;
	    ਜੇ (wp->w_topline > 1 && sb_thumb_ਉਚਾਈ == ਉਚਾਈ)
		--sb_thumb_ਉਚਾਈ;  // scrolled, no full thumb
	    ਜੇ (sb_thumb_ਉਚਾਈ == 0)
		sb_thumb_ਉਚਾਈ = 1;
	    ਜੇ (linecount <= wp->w_ਉਚਾਈ || wp->w_ਉਚਾਈ == 0)
		// it just fits, avoid divide by zero
		sb_thumb_ਸਿਖਰ = 0;
	    ਨਹੀਂਤਾਂ
		sb_thumb_ਸਿਖਰ = (wp->w_topline - 1
				+ (linecount / wp->w_ਉਚਾਈ) / 2)
				* (wp->w_ਉਚਾਈ - sb_thumb_ਉਚਾਈ)
						  / (linecount - wp->w_ਉਚਾਈ);
	    ਜੇ (wp->w_topline > 1 && sb_thumb_ਸਿਖਰ == 0 && ਉਚਾਈ > 1)
		sb_thumb_ਸਿਖਰ = 1;  // show it's scrolled
	    ਆਖਰੀ = ਕੁੱਲ_ਉਚਾਈ_2 - ਸਿਖਰ_off - wp->w_popup_border[2];
	    ਜੇ (sb_thumb_ਸਿਖਰ >= ਆਖਰੀ)
		// show at least one character
		sb_thumb_ਸਿਖਰ = ਆਖਰੀ - 1;

	    ਜੇ (wp->w_scrollbar_highlight != ਨਿਰਾ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    ਨਹੀਂਤਾਂ
		attr_scroll = highlight_attr[HLF_PSB];
	    ਜੇ (wp->w_thumb_highlight != ਨਿਰਾ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    ਨਹੀਂਤਾਂ
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	ਲਈ (i = cl.eff_border[0]; i < ਕੁੱਲ_ਉਚਾਈ_2 - cl.eff_border[2]; ++i)
	{
	    ਪੂਰਨ	pad_ਖੱਬਾ;
	    // left and right padding only needed next to the body
	    ਪੂਰਨ do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < ਕੁੱਲ_ਉਚਾਈ_2 - cl.eff_border[2] - cl.eff_padding[2];

	    ਪੰਕਤੀ = wp->w_winrow + i;

	    // left border
	    ਜੇ (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, ਪੰਕਤੀ, wincol, border_attr[3]);
	    }
	    ਜੇ (do_padding && cl.eff_padding[3] > 0)
	    {
		ਪੂਰਨ ਖਾਨਾ = wincol + wp->w_popup_border[3];

		// left padding
		pad_ਖੱਬਾ = wp->w_popup_padding[3];
		ਜੇ (ਖਾਨਾ < 0)
		{
		    pad_ਖੱਬਾ += ਖਾਨਾ;
		    ਖਾਨਾ = 0;
		}
		ਜੇ (pad_ਖੱਬਾ > 0)
		{
		    ਜੇ (screen_opacity_popup != ਨਿਰਾ && saved_screen.ਸਤਰਾਂ != ਨਿਰਾ)
			fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ, ਖਾਨਾ + pad_ਖੱਬਾ,
				&saved_screen);
		    ਨਹੀਂਤਾਂ
			screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ, ਖਾਨਾ + pad_ਖੱਬਾ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    ਜੇ (wp->w_has_scrollbar)
	    {
		ਪੂਰਨ ਸਤਰ = i - ਸਿਖਰ_off;
		ਪੂਰਨ scroll_ਖਾਨਾ = wp->w_wincol + ਕੁੱਲ_ਚੌੜਾਈ - 1
						       - wp->w_popup_border[1];

		ਜੇ (ਸਤਰ >= 0 && ਸਤਰ < wp->w_ਉਚਾਈ)
		    screen_putchar(' ', ਪੰਕਤੀ, scroll_ਖਾਨਾ,
			    ਸਤਰ >= sb_thumb_ਸਿਖਰ
				       && ਸਤਰ < sb_thumb_ਸਿਖਰ + sb_thumb_ਉਚਾਈ
						  ? attr_thumb : attr_scroll);
		ਨਹੀਂਤਾਂ
		    screen_putchar(' ', ਪੰਕਤੀ, scroll_ਖਾਨਾ, popup_attr);
	    }
	    // right border
	    ਜੇ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, ਪੰਕਤੀ, wincol + ਕੁੱਲ_ਚੌੜਾਈ - 1, border_attr[1]);
	    }
	    // right padding
	    ਜੇ (do_padding && cl.eff_padding[1] > 0)
	    {
		ਪੂਰਨ pad_ਖਾਨਾ_ਸ਼ੁਰੂ = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_ਚੌੜਾਈ;
		ਪੂਰਨ pad_ਖਾਨਾ_ਸਿਰਾ = pad_ਖਾਨਾ_ਸ਼ੁਰੂ + wp->w_popup_padding[1];

		ਜੇ (screen_opacity_popup != ਨਿਰਾ && saved_screen.ਸਤਰਾਂ != ਨਿਰਾ)
		    fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, pad_ਖਾਨਾ_ਸ਼ੁਰੂ, pad_ਖਾਨਾ_ਸਿਰਾ,
			    &saved_screen);
		ਨਹੀਂਤਾਂ
		    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, pad_ਖਾਨਾ_ਸ਼ੁਰੂ, pad_ਖਾਨਾ_ਸਿਰਾ,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	ਜੇ (wp->w_popup_shadow)
	{
	    ਪੂਰਨ ਖਾਨਾ = wincol + ਕੁੱਲ_ਚੌੜਾਈ;
	    ਲਈ (i = 0; i < ਕੁੱਲ_ਉਚਾਈ_2; ++i)
	    {
		ਪੰਕਤੀ = wp->w_winrow + i + 1;
		put_shadow_char(ਪੰਕਤੀ, ਖਾਨਾ);
		put_shadow_char(ਪੰਕਤੀ, ਖਾਨਾ + 1);
	    }
	}

	ਜੇ (cl.eff_padding[2] > 0)
	{
	    // bottom padding -- sits right after the visible content rows.
	    // Derive the row from total_height so it always lands inside the
	    // popup's drawn extent, including the corner case where the top
	    // clip consumes more rows than the content itself (so the visible
	    // content height is zero).  A formula based on
	    // w_height - clip_top_content - clip_bot_content can go negative
	    // there and would draw the padding above w_winrow.
	    ਪੰਕਤੀ = wp->w_winrow + ਕੁੱਲ_ਉਚਾਈ_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    ਜੇ (screen_opacity_popup != ਨਿਰਾ && saved_screen.ਸਤਰਾਂ != ਨਿਰਾ)
		fill_opacity_padding(ਪੰਕਤੀ, ਪੰਕਤੀ + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    ਨਹੀਂਤਾਂ
		screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	ਜੇ (cl.eff_border[2] > 0)
	{
	    // bottom border
	    ਪੰਕਤੀ = wp->w_winrow + ਕੁੱਲ_ਉਚਾਈ_2 - 1;
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + ਕੁੱਲ_ਚੌੜਾਈ,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    ਜੇ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, ਪੰਕਤੀ, wincol + ਕੁੱਲ_ਚੌੜਾਈ - 1, border_attr[2]);
	    }
	}

	ਜੇ (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    ਪੰਕਤੀ = wp->w_winrow + ਕੁੱਲ_ਉਚਾਈ_2;
	    ਲਈ (ਪੂਰਨ ਖਾਨਾ = 2 + (wincol < 0 ? 0 : wincol);
		    ਖਾਨਾ < wincol + ਕੁੱਲ_ਚੌੜਾਈ; ਖਾਨਾ++)
		put_shadow_char(ਪੰਕਤੀ, ਖਾਨਾ);
	}

	ਜੇ (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + ਕੁੱਲ_ਚੌੜਾਈ - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ਨਿਰਾ;

	// Back to the normal zindex.
	screen_zindex = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	// if this was the message window popup may start the timer now
	may_ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer(wp);
#ਜੇ_ਅੰਤ

	ਜੇ (override_success)
	    ਕੱਢੋ_highlight_overrides();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
	popup_position_image(wp);
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    VIM_CLEAR(ਅਧਾਰ_screenlines);
    VIM_CLEAR(ਅਧਾਰ_screenattrs);
    VIM_CLEAR(ਅਧਾਰ_screenlinesuc);
    ਅਧਾਰ_screen_ਪੰਕਤੀਆਂ = 0;
    ਅਧਾਰ_screen_ਖਾਨੇ_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
    // In case win_update() called start_search_hl().
    ਸਿਰਾ_ਖੋਜ_hl();
#ਜੇ_ਅੰਤ
}

/*
 * Mark references in callbacks of one popup window.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_one_popup(win_T *wp, ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    typval_T	tv;

    ਜੇ (wp->w_close_cb.cb_partial != ਨਿਰਾ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਜੇ (wp->w_filter_cb.cb_partial != ਨਿਰਾ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    }
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_list(wp->w_popup_ਨਕਾਬ, copyID);
    ਵਾਪਸ abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_popups(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    ਲਈ (wp = ਪਹਿਲਾ_popupwin; !abort && wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	ਲਈ (wp = tp->tp_ਪਹਿਲਾ_popupwin; !abort && wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_one_popup(wp, copyID);
	ਜੇ (abort)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ abort;
}

    ਪੂਰਨ
popup_is_popup(win_T *wp)
{
    ਵਾਪਸ wp->w_popup_flags != 0;
}

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ਲੱਭੋ_preview_ਖਿੜਕੀ(ਖਾਲੀ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_p_pvw)
	    ਵਾਪਸ wp;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ਲੱਭੋ_info_ਖਿੜਕੀ(ਖਾਲੀ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_popup_flags & POPF_INFO)
	    ਵਾਪਸ wp;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(ਪੂਰਨ dir, bool by_ਪੰਨਾ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    win_T	*wp = popup_ਲੱਭੋ_info_ਖਿੜਕੀ();
    ਪੂਰਨ		by;
    linenr_T	new_topline;

    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ false;

    by = by_ਪੰਨਾ ? (wp->w_ਉਚਾਈ > 2 ? wp->w_ਉਚਾਈ - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    ਜੇ (new_topline < 1)
	new_topline = 1;
    ਜੇ (new_topline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	new_topline = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (new_topline != wp->w_topline)
    {
	ਸੈੱਟ_topline(wp, new_topline);
	popup_ਸੈੱਟ_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ਵਾਪਸ true;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ false;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
f_popup_findecho(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
    rettv->vval.v_ਸੰਖਿਆ = ਸੁਨੇਹਾ_win_2 == ਨਿਰਾ ? 0 : ਸੁਨੇਹਾ_win_2->w_id;
#ਨਹੀਂਤਾਂ
    rettv->vval.v_ਸੰਖਿਆ = 0;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
f_popup_findinfo(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    win_T   *wp = popup_ਲੱਭੋ_info_ਖਿੜਕੀ();

    rettv->vval.v_ਸੰਖਿਆ = wp == ਨਿਰਾ ? 0 : wp->w_id;
#ਨਹੀਂਤਾਂ
    rettv->vval.v_ਸੰਖਿਆ = 0;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
f_popup_findpreview(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    win_T   *wp = popup_ਲੱਭੋ_preview_ਖਿੜਕੀ();

    rettv->vval.v_ਸੰਖਿਆ = wp == ਨਿਰਾ ? 0 : wp->w_id;
#ਨਹੀਂਤਾਂ
    rettv->vval.v_ਸੰਖਿਆ = 0;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    ਪੂਰਨ
popup_create_preview_ਖਿੜਕੀ(ਪੂਰਨ info)
{
    win_T *wp = popup_create(ਨਿਰਾ, ਨਿਰਾ, info ? TYPE_INFO : TYPE_PREVIEW);

    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (info)
	wp->w_popup_flags |= POPF_INFO;
    ਨਹੀਂਤਾਂ
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    ਜੇ (wp->w_minwidth > 0)
	wp->w_ਚੌੜਾਈ = wp->w_minwidth;
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_maxwidth > 0)
	wp->w_ਚੌੜਾਈ = wp->w_maxwidth;
    ਨਹੀਂਤਾਂ
	wp->w_ਚੌੜਾਈ = curwin->w_ਚੌੜਾਈ;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_ਬਫਰ->b_locked = FALSE;

    win_enter(wp, FALSE);
    ਵਾਪਸ OK;
}

/*
 * Close any preview popup.
 */
    ਖਾਲੀ
popup_close_preview(ਖਾਲੀ)
{
    win_T *wp = popup_ਲੱਭੋ_preview_ਖਿੜਕੀ();

    ਜੇ (wp != ਨਿਰਾ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    ਖਾਲੀ
popup_hide_info(ਖਾਲੀ)
{
    win_T *wp = popup_ਲੱਭੋ_info_ਖਿੜਕੀ();

    ਜੇ (wp != ਨਿਰਾ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    ਖਾਲੀ
popup_close_info(ਖਾਲੀ)
{
    win_T *wp = popup_ਲੱਭੋ_info_ਖਿੜਕੀ();

    ਜੇ (wp != ਨਿਰਾ)
	popup_close_with_retval(wp, -1);
}
#ਜੇ_ਅੰਤ

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    ਪੂਰਨ
popup_overlaps_cmdline(ਖਾਲੀ)
{
    ਵਾਪਸ popup_on_cmdline;
}

#ਜੇ defined(HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_ਸੁਨੇਹਾ_win(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_win_2 != ਨਿਰਾ)
	ਵਾਪਸ ਸੁਨੇਹਾ_win_2;

    ਪੂਰਨ i;

    ਸੁਨੇਹਾ_win_2 = popup_create(ਨਿਰਾ, ਨਿਰਾ, TYPE_ਸੁਨੇਹਾ_WIN);

    ਜੇ (ਸੁਨੇਹਾ_win_2 == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // use the full screen width
    ਸੁਨੇਹਾ_win_2->w_ਚੌੜਾਈ = Columns;

    // position at bottom of screen
    ਸੁਨੇਹਾ_win_2->w_popup_pos = POPPOS_BOTTOM;
    ਸੁਨੇਹਾ_win_2->w_wantcol = 1;
    ਸੁਨੇਹਾ_win_2->w_minwidth = 9999;
    ਸੁਨੇਹਾ_win_2->w_firstline = -1;

    // no padding, border at the top
    ਲਈ (i = 0; i < 4; ++i)
	ਸੁਨੇਹਾ_win_2->w_popup_padding[i] = 0;
    ਲਈ (i = 1; i < 4; ++i)
	ਸੁਨੇਹਾ_win_2->w_popup_border[i] = 0;

    ਜੇ (ਸੁਨੇਹਾ_win_2->w_popup_timer != ਨਿਰਾ)
	ਸੁਨੇਹਾ_win_2->w_popup_timer->tr_keep = TRUE;
    ਵਾਪਸ ਸੁਨੇਹਾ_win_2;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    ਖਾਲੀ
popup_show_ਸੁਨੇਹਾ_win(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_win_2 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ ((ਸੁਨੇਹਾ_win_2->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(ਸੁਨੇਹਾ_win_2, TYPE_ਸੁਨੇਹਾ_WIN);
	popup_show(ਸੁਨੇਹਾ_win_2);
    }
    ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
may_ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer(win_T *wp)
{
    ਜੇ (wp == ਸੁਨੇਹਾ_win_2 && ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer)
    {
	ਜੇ (ਸੁਨੇਹਾ_win_2->w_popup_timer != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_win_2->w_popup_timer->tr_interval = ਸੁਨੇਹਾ_win_ਸਮਾਂ;
	    timer_ਸ਼ੁਰੂ(ਸੁਨੇਹਾ_win_2->w_popup_timer);
	    ਸੁਨੇਹਾ_win_ਸਮਾਂ = 3000;
	}
	ਸ਼ੁਰੂ_ਸੁਨੇਹਾ_win_timer = FALSE;
    }
}

    ਪੂਰਨ
popup_ਸੁਨੇਹਾ_win_visible(ਖਾਲੀ)
{
    ਵਾਪਸ ਸੁਨੇਹਾ_win_2 != ਨਿਰਾ
	&& (ਸੁਨੇਹਾ_win_2->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    ਖਾਲੀ
popup_hide_ਸੁਨੇਹਾ_win(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_win_2 != ਨਿਰਾ)
	popup_hide(ਸੁਨੇਹਾ_win_2);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ਸਥਿਰ ਪੂਰਨ save_ਸੁਨੇਹਾ_didout = FALSE;
ਸਥਿਰ ਪੂਰਨ save_ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ਸਥਿਰ ਪੂਰਨ ew_ਸੁਨੇਹਾ_didout = FALSE;
ਸਥਿਰ ਪੂਰਨ ew_ਸੁਨੇਹਾ_ਖਾਨਾ = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    ਖਾਲੀ
ਸ਼ੁਰੂ_echowindow(ਪੂਰਨ ਸਮਾਂ_sec)
{
    ਵਿੱਚ_echowindow = TRUE;
    save_ਸੁਨੇਹਾ_didout = ਸੁਨੇਹਾ_didout;
    save_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਸੁਨੇਹਾ_didout = ew_ਸੁਨੇਹਾ_didout;
    ਸੁਨੇਹਾ_ਖਾਨਾ = ew_ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਜੇ (ਸਮਾਂ_sec != 0)
	ਸੁਨੇਹਾ_win_ਸਮਾਂ = ਸਮਾਂ_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    ਖਾਲੀ
ਸਿਰਾ_echowindow(ਖਾਲੀ)
{
    ਵਿੱਚ_echowindow = FALSE;

    ਜੇ ((State & ਬਹੁਲਕ_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_ਸੁਨੇਹਾ_didout = TRUE;
    ew_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ == 0 ? 1 : ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਸੁਨੇਹਾ_didout = save_ਸੁਨੇਹਾ_didout;
    ਸੁਨੇਹਾ_ਖਾਨਾ = save_ਸੁਨੇਹਾ_ਖਾਨਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    ਪੂਰਨ
popup_win_closed(win_T *win)
{
    ਪੂਰਨ	    ਗੇੜਾ;
    win_T   *wp;
    win_T   *ਅਗਲਾ;
    ਪੂਰਨ	    ret = FALSE;

    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	ਲਈ (wp = ਗੇੜਾ == 1 ? ਪਹਿਲਾ_popupwin : curtab->tp_ਪਹਿਲਾ_popupwin;
							 wp != ਨਿਰਾ; wp = ਅਗਲਾ)
	{
	    ਅਗਲਾ = wp->w_ਅਗਲਾ;
	    ਜੇ (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    ਵਾਪਸ ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    ਖਾਲੀ
popup_ਸੈੱਟ_title(win_T *wp)
{
    ਜੇ (wp->w_ਬਫਰ->b_fname == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_ਬਫਰ, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_ਬਫਰ->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    ਜੇ (wp->w_popup_title != ਨਿਰਾ)
	vim_snprintf((ਅੱਖਰ *)wp->w_popup_title, len, " %s ",
		wp->w_ਬਫਰ->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    ਖਾਲੀ
popup_update_preview_title(ਖਾਲੀ)
{
    win_T *wp = popup_ਲੱਭੋ_preview_ਖਿੜਕੀ();

    ਜੇ (wp != ਨਿਰਾ)
	popup_ਸੈੱਟ_title(wp);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // FEAT_PROP_POPUP
