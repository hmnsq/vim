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

#ಆದರೆ defined(FEAT_PROP_POPUP)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_ನಮೂದು_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ಸಕ್ರಿಯ;
    ಪೂರ್ಣಾಂಕ		winrow;
    ಪೂರ್ಣಾಂಕ		wincol;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		leftoff;
    ಪೂರ್ಣಾಂಕ		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    schar_T	*ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		*attrs;
    u8char_T	*linesuc;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲುಗಳು_2;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    linenr_T	firstline;
    ಪೂರ್ಣಾಂಕ		blend;
    ಪೂರ್ಣಾಂಕ		flags;
    ಪೂರ್ಣಾಂಕ		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		winrow;
    ಪೂರ್ಣಾಂಕ		wincol;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		leftcol;
    ಪೂರ್ಣಾಂಕ		leftoff;
    ಪೂರ್ಣಾಂಕ		has_scrollbar;
    ಪೂರ್ಣಾಂಕ		topoff;
    ಪೂರ್ಣಾಂಕ		bottomoff;
    ಪೂರ್ಣಾಂಕ		leftclip;
    ಪೂರ್ಣಾಂಕ		rightclip;
} popup_layout_T;

ಸ್ಥಿರ poppos_ನಮೂದು_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
// Window used for ":echowindow"
ಸ್ಥಿರ win_T *ಸಂದೇಶ_win_2 = ಖಾಲಿ;

// Time used for the next ":echowindow" message in msec.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಸಂದೇಶ_win_ಸಮಯ = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ    ಆರಂಭ_ಸಂದೇಶ_win_timer = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ may_ಆರಂಭ_ಸಂದೇಶ_win_timer(win_T *wp);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ popup_on_cmdline = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ popup_adjust_position(win_T *wp);
ಸ್ಥಿರ bool popup_area_ಬದಲಾಯಿತು(win_T *wp, popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_redraw_exposed_area(popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_area(win_T *wp, popup_area_T *area);
ಸ್ಥಿರ ಶೂನ್ಯ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ಸ್ಥಿರ bool popup_layout_ಬದಲಾಯಿತು(win_T *wp, popup_layout_T *layout);
ಸ್ಥಿರ bool popup_style_ಬದಲಾಯಿತು(win_T *wp, popup_style_snapshot_T *style);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ಸ್ಥಿರ ಶೂನ್ಯ popup_save_layout(win_T *wp, popup_layout_T *layout);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_under_popup_area(ಪೂರ್ಣಾಂಕ winrow, ಪೂರ್ಣಾಂಕ wincol, ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ leftoff);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_overlapped_opacity_popups(ಪೂರ್ಣಾಂಕ winrow, ಪೂರ್ಣಾಂಕ wincol,
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ leftoff, ಪೂರ್ಣಾಂಕ zindex);
ಸ್ಥಿರ ಶೂನ್ಯ redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_options_one(dict_T *dict, char_u *ಕೀಲಿ)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    ಪೂರ್ಣಾಂಕ		n = 0;

    di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ MAXCOL;

    val = tv_get_string(&di->di_tv);
    ಆದರೆ (STRNCMP(val, "cursor", 6) != 0)
	ಹಿಂತಿರುಗಿ dict_get_ಸಂಖ್ಯೆ_check(dict, ಕೀಲಿ);

    setcursor_mayforce(TRUE);
    s = val + 6;
    ಆದರೆ (*s != NUL)
    {
	endp = s;
	ಆದರೆ (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((ಅಕ್ಷರ *)s, (ಅಕ್ಷರ **)&endp, 10);
	ಆದರೆ (endp != ಖಾಲಿ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), val);
	    ಹಿಂತಿರುಗಿ 0;
	}
    }

    ಆದರೆ (STRCMP(ಕೀಲಿ, "line") == 0)
	n = screen_screenrow() + 1 + n;
    ಇಲ್ಲದಿದ್ದರೆ // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    ಆದರೆ (n == 0)
	n = -1;
    ಹಿಂತಿರುಗಿ n;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_padding_border(dict_T *dict, ಪೂರ್ಣಾಂಕ *array, ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ max_val)
{
    dictitem_T	*di;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)name, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		nr;

    ಗಾಗಿ (i = 0; i < 4; ++i)
	array[i] = 1;
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    CHECK_LIST_MATERIALIZE(list);
    ಗಾಗಿ (i = 0, li = list->lv_ಮೊದಲ; i < 4 && i < list->lv_len;
	    ++i, li = li->li_ಮುಂದಿನ)
    {
	nr = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_moved_ಮೌಲ್ಯಗಳು(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.ಲಂಬಸಾಲು;
    wp->w_popup_maxcol = curwin->w_cursor.ಲಂಬಸಾಲು;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_moved_columns(win_T *wp, ಪೂರ್ಣಾಂಕ flags)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		len = ಕಂಡುಹಿಡಿ_ident_under_cursor(&ptr, flags | ಕಂಡುಹಿಡಿ_NOERROR);
    ಪೂರ್ಣಾಂಕ		mincol;

    ಆದರೆ (len <= 0)
	ಹಿಂತಿರುಗಿ;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (ಪೂರ್ಣಾಂಕ)(ptr - ml_get_curline());
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.ಲಂಬಸಾಲು;
	wp->w_popup_maxcol = curwin->w_cursor.ಲಂಬಸಾಲು;
	ಹಿಂತಿರುಗಿ;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_mousemoved_ಮೌಲ್ಯಗಳು(win_T *wp)
{
    wp->w_popup_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    wp->w_popup_mouse_mincol = mouse_ಲಂಬಸಾಲು;
    wp->w_popup_mouse_maxcol = mouse_ಲಂಬಸಾಲು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_popup_ಬಳಸುತ್ತದೆ_mouse_move(ಶೂನ್ಯ)
{
    popup_ಬಳಸುತ್ತದೆ_mouse_move = FALSE;
    ಆದರೆ (!popup_visible)
	ಹಿಂತಿರುಗಿ;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_popup_mouse_ಅಡ್ಡಸಾಲು != 0)
	{
	    popup_ಬಳಸುತ್ತದೆ_mouse_move = TRUE;
	    ಹಿಂತಿರುಗಿ;
	}
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_popup_mouse_ಅಡ್ಡಸಾಲು != 0)
	{
	    popup_ಬಳಸುತ್ತದೆ_mouse_move = TRUE;
	    ಹಿಂತಿರುಗಿ;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_mousemoved_columns(win_T *wp, ಪೂರ್ಣಾಂಕ flags)
{
    win_T	*textwp;
    char_u	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    pos_T	pos;
    colnr_T	mcol;

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಶಬ್ದ_under_cursor(mouse_ಅಡ್ಡಸಾಲು, mouse_ಲಂಬಸಾಲು, TRUE, flags,
				  &textwp, &pos.lnum, &ಪಠ್ಯ, ಖಾಲಿ, &ಲಂಬಸಾಲು) != OK)
	ಹಿಂತಿರುಗಿ;

    // convert text column to mouse column
    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ಖಾಲಿ, ಖಾಲಿ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + (colnr_T)STRLEN(ಪಠ್ಯ) - 1;
    getvcol(textwp, &pos, ಖಾಲಿ, ಖಾಲಿ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(ಪಠ್ಯ);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    ಪೂರ್ಣಾಂಕ
popup_on_border(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಹಿಂತಿರುಗಿ (ಅಡ್ಡಸಾಲು == 0 && wp->w_popup_border[0] > 0)
	    || (ಅಡ್ಡಸಾಲು == popup_ಎತ್ತರ(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (ಲಂಬಸಾಲು == 0 && wp->w_popup_border[3] > 0)
	    || (ಲಂಬಸಾಲು == popup_ಅಗಲ(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    ಪೂರ್ಣಾಂಕ
popup_close_if_on_X(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (wp->w_popup_close == POPCLOSE_BUTTON
	    && ಅಡ್ಡಸಾಲು == 0 && ಲಂಬಸಾಲು == popup_ಅಗಲ(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

// Values set when dragging a popup window starts.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drag_ಆರಂಭ_ಅಡ್ಡಸಾಲು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drag_ಆರಂಭ_ಲಂಬಸಾಲು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drag_ಆರಂಭ_wantline;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drag_ಆರಂಭ_wantcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    ಶೂನ್ಯ
popup_ಆರಂಭ_drag(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    drag_ಆರಂಭ_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    drag_ಆರಂಭ_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    ಆದರೆ (wp->w_wantline <= 0)
	drag_ಆರಂಭ_wantline = wp->w_winrow + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	drag_ಆರಂಭ_wantline = wp->w_wantline;
    ಆದರೆ (wp->w_wantcol == 0)
	drag_ಆರಂಭ_wantcol = wp->w_wincol + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	drag_ಆರಂಭ_wantcol = wp->w_wantcol;

    // Stop centering the popup
    ಆದರೆ (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && ಅಡ್ಡಸಾಲು == popup_ಎತ್ತರ(wp) - 1
			    && ಲಂಬಸಾಲು == popup_ಅಗಲ(wp) - 1;

    ಆದರೆ (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	ಆದರೆ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	ಆದರೆ (wp->w_popup_pos == POPPOS_BOTLEFT)
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
    ಆದರೆ (!win_valid_popup(wp))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	ಪೂರ್ಣಾಂಕ ಅಗಲ_inc = mouse_ಲಂಬಸಾಲು - drag_ಆರಂಭ_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ_inc = mouse_ಅಡ್ಡಸಾಲು - drag_ಆರಂಭ_ಅಡ್ಡಸಾಲು;

	ಆದರೆ (ಅಗಲ_inc != 0)
	{
	    ಪೂರ್ಣಾಂಕ ಅಗಲ = wp->w_ಅಗಲ + ಅಗಲ_inc;

	    ಆದರೆ (ಅಗಲ < 1)
		ಅಗಲ = 1;
	    wp->w_minwidth = ಅಗಲ;
	    wp->w_maxwidth = ಅಗಲ;
	    drag_ಆರಂಭ_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	}

	ಆದರೆ (ಎತ್ತರ_inc != 0)
	{
	    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = wp->w_ಎತ್ತರ + ಎತ್ತರ_inc;

	    ಆದರೆ (ಎತ್ತರ < 1)
		ಎತ್ತರ = 1;
	    wp->w_minheight = ಎತ್ತರ;
	    wp->w_maxheight = ಎತ್ತರ;
	    drag_ಆರಂಭ_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	}

	popup_adjust_position(wp);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	ಹಿಂತಿರುಗಿ;
    wp->w_wantline = drag_ಆರಂಭ_wantline + (mouse_ಅಡ್ಡಸಾಲು - drag_ಆರಂಭ_ಅಡ್ಡಸಾಲು);
    ಆದರೆ (wp->w_wantline < 1)
	wp->w_wantline = 1;
    ಆದರೆ (wp->w_wantline > ಅಡ್ಡಸಾಲುಗಳು)
	wp->w_wantline = ಅಡ್ಡಸಾಲುಗಳು;
    wp->w_wantcol = drag_ಆರಂಭ_wantcol + (mouse_ಲಂಬಸಾಲು - drag_ಆರಂಭ_ಲಂಬಸಾಲು);
    ಆದರೆ (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    ಆದರೆ (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    ಶೂನ್ಯ
popup_ಗಣ_firstline(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ = wp->w_ಎತ್ತರ;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    ಆಗ (wp->w_firstline > 1 && wp->w_ಎತ್ತರ < ಎತ್ತರ)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    ಪೂರ್ಣಾಂಕ
popup_is_ಒಳ_scrollbar(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಹಿಂತಿರುಗಿ wp->w_has_scrollbar
	&& ಅಡ್ಡಸಾಲು >= wp->w_popup_border[0]
	&& ಅಡ್ಡಸಾಲು < popup_ಎತ್ತರ(wp) - wp->w_popup_border[2]
	&& ಲಂಬಸಾಲು == popup_ಅಗಲ(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    ಶೂನ್ಯ
popup_handle_scrollbar_click(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (!popup_is_ಒಳ_scrollbar(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ = popup_ಎತ್ತರ(wp);
    ಪೂರ್ಣಾಂಕ	    new_topline = wp->w_topline;

    ಆದರೆ (ಅಡ್ಡಸಾಲು >= ಎತ್ತರ / 2)
    {
	// Click in lower half, scroll down.
	ಆದರೆ (wp->w_topline < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ++new_topline;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    ಆದರೆ (new_topline == wp->w_topline)
	ಹಿಂತಿರುಗಿ;

    ಗಣ_topline(wp, new_topline);
    ಆದರೆ (wp == curwin)
    {
	ಆದರೆ (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_ಗಣ_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ಆದರೆ defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_ಕೂಡಿಸು_timeout(win_T *wp, ಪೂರ್ಣಾಂಕ ಸಮಯ, ಪೂರ್ಣಾಂಕ close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((ಅಕ್ಷರ *)cbbuf, ಗಾತ್ರ(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    ಆದರೆ (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	ಹಿಂತಿರುಗಿ;

    wp->w_popup_timer = create_timer(ಸಮಯ, 0);
    callback_T cb = get_callback(&tv);
    ಆದರೆ (cb.cb_name != ಖಾಲಿ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ poppos_T
get_pos_ನಮೂದು(dict_T *d, ಪೂರ್ಣಾಂಕ give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    ಪೂರ್ಣಾಂಕ	    nr;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ POPPOS_NONE;

    ಗಾಗಿ (nr = 0; nr < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(poppos_entries); ++nr)
	ಆದರೆ (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    ಹಿಂತಿರುಗಿ poppos_entries[nr].pp_val;

    ಆದರೆ (give_error)
	semsg(_(e_invalid_ವಾದ_str), str);
    ಹಿಂತಿರುಗಿ POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_move_options(win_T *wp, dict_T *d)
{
    ಪೂರ್ಣಾಂಕ		nr;
    char_u	*str;
    dictitem_T	*di;

    ಆದರೆ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    ಆದರೆ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    ಆದರೆ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    ಆದರೆ ((nr = dict_get_ಸಂಖ್ಯೆ_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    ಆದರೆ (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    ಆದರೆ (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    ಆದರೆ (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_ನಮೂದು(d, TRUE);

	ಆದರೆ (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	wp->w_popup_prop_type = 0;
	ಆದರೆ (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"textpropwin", -1);
	    ಆದರೆ (di != ಖಾಲಿ)
	    {
		wp->w_popup_prop_win = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&di->di_tv);
		ಆದರೆ (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = ಕಂಡುಹಿಡಿ_prop_type_id(str, wp->w_popup_prop_win->w_ಬಫರ್);
	    ಆದರೆ (nr <= 0)
		nr = ಕಂಡುಹಿಡಿ_prop_type_id(str, ಖಾಲಿ);
	    ಆದರೆ (nr <= 0)
		semsg(_(e_invalid_ವಾದ_str), str);
	    ಇಲ್ಲದಿದ್ದರೆ
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"textpropid", -1);
    ಆದರೆ (di != ಖಾಲಿ)
	wp->w_popup_prop_id = dict_get_ಸಂಖ್ಯೆ(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_moved_ವಾದ(win_T *wp, dictitem_T *di, ಪೂರ್ಣಾಂಕ mousemoved)
{
    ಆದರೆ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ಖಾಲಿ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	ಪೂರ್ಣಾಂಕ	flags = 0;

	ಆದರೆ (STRCMP(s, "word") == 0)
	    flags = ಕಂಡುಹಿಡಿ_IDENT | ಕಂಡುಹಿಡಿ_STRING;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "WORD") == 0)
	    flags = ಕಂಡುಹಿಡಿ_STRING;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "expr") == 0)
	    flags = ಕಂಡುಹಿಡಿ_IDENT | ಕಂಡುಹಿಡಿ_STRING | ಕಂಡುಹಿಡಿ_EVAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_ವಾದ_str), s);
	ಆದರೆ (flags != 0)
	{
	    ಆದರೆ (mousemoved)
		ಗಣ_mousemoved_columns(wp, flags);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗಣ_moved_columns(wp, flags);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ಖಾಲಿ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	ಪೂರ್ಣಾಂಕ	    mincol;
	ಪೂರ್ಣಾಂಕ	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ಮೊದಲ;
	ಆದರೆ (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_ಸಂಖ್ಯೆ(&l->lv_ಮೊದಲ->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    ಆದರೆ (mousemoved)
		wp->w_popup_mouse_ಅಡ್ಡಸಾಲು = nr;
	    ಇಲ್ಲದಿದ್ದರೆ
		wp->w_popup_lnum = nr;
	    li = li->li_ಮುಂದಿನ;
	    ಆದರೆ (nr == 0)
		wp->w_popup_curwin = ಖಾಲಿ;
	}

	mincol = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	maxcol = tv_get_ಸಂಖ್ಯೆ(&li->li_ಮುಂದಿನ->li_tv);
	ಆದರೆ (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&di->di_tv));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
check_highlight(dict_T *dict, ಅಕ್ಷರ *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)name, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), name);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	str = tv_get_string(&di->di_tv);
	ಆದರೆ (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_show_curline(win_T *wp)
{
    ಆದರೆ (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_ಎತ್ತರ + 1;
	ಆದರೆ (wp->w_topline < 1)
	    wp->w_topline = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_topline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_topline = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಆಗ (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_ಎತ್ತರ + 1) > wp->w_ಎತ್ತರ)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    ಆದರೆ (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ಸ್ಥಿರ char_u *
popup_get_sign_name(win_T *wp)
{
    ಸ್ಥಿರ ಅಕ್ಷರ    buf[30];

    vim_snprintf(buf, ಗಾತ್ರ(buf), "popup-%d", wp->w_id);
    ಹಿಂತಿರುಗಿ (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_highlight_curline(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_ಬಫರ್, (char_u *)"PopUpMenu");

    ಆದರೆ ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	ಆದರೆ (!sign_exists_by_name(sign_name))
	{
	    ಅಕ್ಷರ *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ಖಾಲಿ, (char_u *)linehl, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
		    ಖಾಲಿ, SIGN_DEF_PRIO);
	}

	sign_ಇರಿಸು(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_ಬಫರ್, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_ಕೊನೆಯ_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		nr;
    char_u	*str;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#ಆದರೆ_ಅಂತ್ಯ

    // TODO: flip

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"firstline", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	wp->w_firstline = dict_get_ಸಂಖ್ಯೆ(dict, "firstline");
	ಆದರೆ (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    ಆದರೆ (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	char_u	*title = vim_strsave(str);

	ಆದರೆ (title != ಖಾಲಿ)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    ಆದರೆ (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"opacity", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	nr = dict_get_ಸಂಖ್ಯೆ(dict, "opacity");
	ಆದರೆ (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    ಆದರೆ (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    ಇಲ್ಲದಿದ್ದರೆ
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ಖಾಲಿ)
    {
	image_T		    *img;
	image_T		    *cur_img = ಖಾಲಿ;
	image_placement_T   *ಇರಿಸು;
	ಪೂರ್ಣಾಂಕ		    cw, ch;

	ಆದರೆ (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ಖಾಲಿ;
	    }
	    ಹೋಗು image_ಮುಗಿದ;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ && wp->w_popup_imagep->img != ಖಾಲಿ)
	    cur_img = wp->w_popup_imagep->img;

	img = ಕೂಡಿಸು_image(di->di_tv.vval.v_dict, cur_img, true);
	ಆದರೆ (img == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (img == cur_img)
	    ಹೋಗು image_ಮುಗಿದ;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	ಇರಿಸು = image_placement_new(img, wp->w_id, iplace_use, false);

	ಆದರೆ (ಇರಿಸು == ಖಾಲಿ)
	{
	    image_unref(img);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = ಇರಿಸು;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	ಆದರೆ (dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"minwidth", -1) == ಖಾಲಿ)
	    wp->w_minwidth = cw;
	ಆದರೆ (dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"maxwidth", -1) == ಖಾಲಿ)
	    wp->w_maxwidth = cw;
	ಆದರೆ (dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"minheight", -1) == ಖಾಲಿ)
	    wp->w_minheight = ch;
	ಆದರೆ (dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"maxheight", -1) == ಖಾಲಿ)
	    wp->w_maxheight = ch;
image_ಮುಗಿದ:
	;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ಖಾಲಿ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	    image_placement_ಗಣ_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_ಎತ್ತರ, wp->w_ಅಗಲ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"close", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ok = TRUE;

	ಆದರೆ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ಖಾಲಿ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    ಆದರೆ (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    ಇಲ್ಲದಿದ್ದರೆ
		ok = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ok = FALSE;
	ಆದರೆ (!ok)
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *errmsg = update_wincolor(wp, str);

	ಆದರೆ (errmsg == ಖಾಲಿ)
	    ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *errmsg = update_winhighlight(wp, str);

	ಆದರೆ (errmsg == ಖಾಲಿ)
	    ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(errmsg));
    }

    ಆದರೆ (ಗಣ_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	ಗಣ_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"borderhighlight", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಆದರೆ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ಖಾಲಿ)
	{
	    emsg(_(e_list_required));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ಪೂರ್ಣಾಂಕ		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    ಆದರೆ (list->lv_len == 0)
	    {
		ಗಾಗಿ (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ಖಾಲಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (i = 0, li = list->lv_ಮೊದಲ; i < 4 && i < list->lv_len;
						     ++i, li = li->li_ಮುಂದಿನ)
		{
		    str = tv_get_string(&li->li_tv);
		    ಆದರೆ (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			ಆದರೆ (hl != ಖಾಲಿ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		ಆದರೆ (list->lv_len == 1 && wp->w_border_highlight[0] != ಖಾಲಿ)
		    ಗಾಗಿ (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			ಆದರೆ (hl != ಖಾಲಿ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"borderchars", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ಪೂರ್ಣಾಂಕ		i;

	    ಆದರೆ (list != ಖಾಲಿ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		ಗಾಗಿ (i = 0, li = list->lv_ಮೊದಲ; i < 8 && i < list->lv_len;
							 ++i, li = li->li_ಮುಂದಿನ)
		{
		    str = tv_get_string(&li->li_tv);
		    ಆದರೆ (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		ಆದರೆ (list->lv_len == 1)
		    ಗಾಗಿ (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		ಆದರೆ (list->lv_len == 2)
		{
		    ಗಾಗಿ (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    ಗಾಗಿ (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"zindex", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	wp->w_zindex = dict_get_ಸಂಖ್ಯೆ(dict, "zindex");
	ಆದರೆ (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	ಆದರೆ (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"mask", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ok = FALSE;

	ಆದರೆ (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ಖಾಲಿ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(di->di_tv.vval.v_list, li)
	    {
		ಆದರೆ (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ಖಾಲಿ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	ಆದರೆ (ok)
	{
	    wp->w_popup_ಮುಖವಾಡ = di->di_tv.vval.v_list;
	    ++wp->w_popup_ಮುಖವಾಡ->lv_refcount;
	    VIM_CLEAR(wp->w_popup_ಮುಖವಾಡ_cells);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "mask");
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

#ಆದರೆ defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_ಸಂಖ್ಯೆ(dict, "time");
    ಆದರೆ (nr > 0)
	popup_ಕೂಡಿಸು_timeout(wp, nr, TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"moved", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಗಣ_moved_ಮೌಲ್ಯಗಳು(wp);
	handle_moved_ವಾದ(wp, di, FALSE);
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"mousemoved", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಗಣ_mousemoved_ಮೌಲ್ಯಗಳು(wp);
	handle_moved_ವಾದ(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"filter", -1);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	ಆದರೆ (callback.cb_name != ಖಾಲಿ)
	{
	    free_callback(&wp->w_filter_cb);
	    ಗಣ_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    ಆದರೆ (nr != -1)
    {
	ಆದರೆ (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	ಆದರೆ (STRCMP(str, "a") == 0)
	    wp->w_filter_ಬಹುಲಕ = ಬಹುಲಕ_ALL;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_filter_ಬಹುಲಕ = ಬಹುಲಕ_str2flags(str);
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"callback", -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    callback_T	callback = get_callback(&di->di_tv);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    free_callback(&wp->w_close_cb);
    ಗಣ_callback(&wp->w_close_cb, &callback);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
apply_options(win_T *wp, dict_T *dict, ಪೂರ್ಣಾಂಕ create)
{
    ಪೂರ್ಣಾಂಕ		nr;

    apply_move_options(wp, dict);

    ಆದರೆ (create)
	ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    ಆದರೆ (apply_general_options(wp, dict) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    ಆದರೆ (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    ಆದರೆ (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	ಆದರೆ (wp->w_firstline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_cursor.lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_ಎತ್ತರ)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    ಆದರೆ (create)
	popup_ಮುಖವಾಡ_refresh = TRUE;
    popup_highlight_curline(wp);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_popup_ಅಕ್ಷರಸರಣಿಗಳು(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	ಆದರೆ (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ಖಾಲಿ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_ವಾದ_1_list_ಅಂಶ_nr_dictionary_required), lnum + 1);
	    ಹಿಂತಿರುಗಿ;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ಖಾಲಿ ? ಖಾಲಿ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ಖಾಲಿ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    ಗಾಗಿ (li = l->lv_ಮೊದಲ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"props", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		ಹಿಂತಿರುಗಿ;
	    }
	    plist = di->di_tv.vval.v_list;
	    ಆದರೆ (plist != ಖಾಲಿ)
	    {
		FOR_ALL_LIST_ವಸ್ತುಗಳು(plist, pli)
		{
		    ಆದರೆ (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			ಹಿಂತಿರುಗಿ;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    ಆದರೆ (dict != ಖಾಲಿ)
		    {
			ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = dict_get_ಸಂಖ್ಯೆ(dict, "col");

			prop_ಕೂಡಿಸು_common(lnum, ಲಂಬಸಾಲು, dict, buf, ಖಾಲಿ);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    ಪೂರ್ಣಾಂಕ
popup_ತುದಿ_extra(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    ಆದರೆ (extra == 0 && wp->w_popup_title != ಖಾಲಿ && *wp->w_popup_title != NUL)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ extra;
}

/*
 * Get the padding plus border at the left.
 */
    ಪೂರ್ಣಾಂಕ
popup_ಎಡ_extra(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    ಪೂರ್ಣಾಂಕ
popup_ಎತ್ತರ(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_ಎತ್ತರ
	    + popup_ತುದಿ_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    ಪೂರ್ಣಾಂಕ
popup_ಅಗಲ(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    ಹಿಂತಿರುಗಿ wp->w_ಅಗಲ
	    + popup_extra_ಅಗಲ(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    ಪೂರ್ಣಾಂಕ
popup_extra_ಅಗಲ(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_popup_padding[3] + wp->w_popup_border[3]
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
    ಆದರೆ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (win_valid(wp->w_popup_prop_win))
	ಹಿಂತಿರುಗಿ wp->w_popup_prop_win;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ತುದಿ_extra;
    ಪೂರ್ಣಾಂಕ bot_extra;
    ಪೂರ್ಣಾಂಕ ಎಡ_extra;
    ಪೂರ್ಣಾಂಕ ಬಲ_extra;

    ಪೂರ್ಣಾಂಕ clip_ತುದಿ_content;
    ಪೂರ್ಣಾಂಕ clip_bot_content;
    ಪೂರ್ಣಾಂಕ clip_ಎಡ_content;
    ಪೂರ್ಣಾಂಕ clip_ಬಲ_content;

    ಪೂರ್ಣಾಂಕ eff_ತುದಿ_extra;
    ಪೂರ್ಣಾಂಕ eff_bot_extra;
    ಪೂರ್ಣಾಂಕ eff_ಎಡ_extra;
    ಪೂರ್ಣಾಂಕ eff_ಬಲ_extra;

    ಪೂರ್ಣಾಂಕ eff_border[4];
    ಪೂರ್ಣಾಂಕ eff_padding[4];

    ಪೂರ್ಣಾಂಕ eff_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ eff_ಅಗಲ;
} popup_clip_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    ಪೂರ್ಣಾಂಕ h, w;

    cl->ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->ಬಲ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_ತುದಿ_content = wp->w_popup_topoff - cl->ತುದಿ_extra;
    ಆದರೆ (cl->clip_ತುದಿ_content < 0)
	cl->clip_ತುದಿ_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    ಆದರೆ (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ಎಡ_content = wp->w_popup_leftclip - cl->ಎಡ_extra;
    ಆದರೆ (cl->clip_ಎಡ_content < 0)
	cl->clip_ಎಡ_content = 0;
    cl->clip_ಬಲ_content = wp->w_popup_rightclip - cl->ಬಲ_extra;
    ಆದರೆ (cl->clip_ಬಲ_content < 0)
	cl->clip_ಬಲ_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	ಪೂರ್ಣಾಂಕ clip = wp->w_popup_topoff;
	ಪೂರ್ಣಾಂಕ b = wp->w_popup_border[0];
	ಪೂರ್ಣಾಂಕ p = wp->w_popup_padding[0];
	ಪೂರ್ಣಾಂಕ rem;

	ಆದರೆ (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	ಪೂರ್ಣಾಂಕ clip = wp->w_popup_bottomoff;
	ಪೂರ್ಣಾಂಕ b = wp->w_popup_border[2];
	ಪೂರ್ಣಾಂಕ p = wp->w_popup_padding[2];
	ಪೂರ್ಣಾಂಕ rem;

	ಆದರೆ (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
	ಪೂರ್ಣಾಂಕ excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_ಎತ್ತರ;
	ಆದರೆ (excess > 0)
	{
	    ಆದರೆ (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		ಆದರೆ (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    cl->eff_border[0] -= excess;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cl->eff_padding[0] -= excess;
	}
    }
    {
	ಪೂರ್ಣಾಂಕ excess = wp->w_popup_topoff - cl->ತುದಿ_extra - wp->w_ಎತ್ತರ;
	ಆದರೆ (excess > 0)
	{
	    ಆದರೆ (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		ಆದರೆ (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    cl->eff_border[2] -= excess;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_ತುದಿ_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ಎಡ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ಎಡ_extra;
    cl->eff_ಬಲ_extra = wp->w_popup_rightclip > 0 ? 0 : cl->ಬಲ_extra;

    h = wp->w_ಎತ್ತರ - cl->clip_ತುದಿ_content - cl->clip_bot_content;
    ಆದರೆ (h < 0)
	h = 0;
    cl->eff_ಎತ್ತರ = cl->eff_ತುದಿ_extra + h + cl->eff_bot_extra;

    w = wp->w_ಅಗಲ - cl->clip_ಎಡ_content - cl->clip_ಬಲ_content;
    ಆದರೆ (w < 0)
	w = 0;
    cl->eff_ಅಗಲ = cl->eff_ಎಡ_extra + w + cl->eff_ಬಲ_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		winrow;
    ಪೂರ್ಣಾಂಕ		wincol;
    ಪೂರ್ಣಾಂಕ		leftcol;
    ಪೂರ್ಣಾಂಕ		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->ಎತ್ತರ  = wp->w_ಎತ್ತರ;
    sv->ಅಗಲ   = wp->w_ಅಗಲ;
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
    wp->w_ಅಗಲ   = sv->ಅಗಲ;
    wp->w_ಎತ್ತರ  = sv->ಎತ್ತರ;
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
	ಪೂರ್ಣಾಂಕ	    *screen_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	    *screen_scol,
	ಪೂರ್ಣಾಂಕ	    *screen_ccol,
	ಪೂರ್ಣಾಂಕ	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    ಪೂರ್ಣಾಂಕ	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (ಪೂರ್ಣಾಂಕ)win_ಲಂಬಸಾಲು_off(prop_win) - (ಪೂರ್ಣಾಂಕ)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (ಪೂರ್ಣಾಂಕ)scol + coloff;
    *screen_ccol = (ಪೂರ್ಣಾಂಕ)ccol + coloff;
    *screen_ecol = (ಪೂರ್ಣಾಂಕ)ecol + coloff;
    *screen_ಅಡ್ಡಸಾಲು = prop_win->w_winrow + 1
				 - (ಪೂರ್ಣಾಂಕ)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_hide_for_textprop(win_T *wp)
{
    ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ಹಿಂತಿರುಗಿ;
    wp->w_popup_flags |= POPF_HIDDEN;
    ಆದರೆ (win_valid(wp->w_popup_prop_win))
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
popup_ಕಂಡುಹಿಡಿ_prop_above_ತುದಿ(
	win_T	    *wp,
	win_T	    *prop_win,
	ಪೂರ್ಣಾಂಕ	    max_ವ್ಯಾಪ್ತಿ,
	textprop_T  *prop,
	linenr_T    *ಸಿಕ್ಕಿತು_lnum)
{
    linenr_T	ಮೊದಲ;

    ಆದರೆ (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	ಹಿಂತಿರುಗಿ false;

    ಮೊದಲ = max_ವ್ಯಾಪ್ತಿ > 0 ? prop_win->w_topline - max_ವ್ಯಾಪ್ತಿ : 1;
    ಆದರೆ (ಮೊದಲ < 1)
	ಮೊದಲ = 1;
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_prop_ಒಳ_ಸಾಲುಗಳು(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, ಸಿಕ್ಕಿತು_lnum, ಮೊದಲ, prop_win->w_topline - 1);
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
    ಪೂರ್ಣಾಂಕ	    extra_h, extra_w;
    ಪೂರ್ಣಾಂಕ	    popup_ತುದಿ, popup_bottom, popup_ಎಡ, popup_ಬಲ;
    ಪೂರ್ಣಾಂಕ	    ಒಟ್ಟು_h, ಒಟ್ಟು_w;

    ಆದರೆ (cw == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    extra_h = popup_ತುದಿ_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_ಅಗಲ(wp);

    popup_ತುದಿ = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_ಎತ್ತರ + extra_h;
    popup_ಎಡ = wp->w_wincol;
    popup_ಬಲ = wp->w_wincol + wp->w_ಅಗಲ + extra_w;
    ಒಟ್ಟು_h = wp->w_ಎತ್ತರ + extra_h;
    ಒಟ್ಟು_w = wp->w_ಅಗಲ + extra_w;

    ಆದರೆ (popup_ತುದಿ < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_ತುದಿ;
    ಆದರೆ (popup_bottom > cw->w_winrow + cw->w_ಎತ್ತರ)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_ಎತ್ತರ);
    ಆದರೆ (popup_ಎಡ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ಎಡ;
    ಆದರೆ (popup_ಬಲ > cw->w_wincol + cw->w_ಅಗಲ)
	wp->w_popup_rightclip = popup_ಬಲ - (cw->w_wincol + cw->w_ಅಗಲ);

    ಹಿಂತಿರುಗಿ wp->w_popup_topoff >= ಒಟ್ಟು_h
	|| wp->w_popup_bottomoff >= ಒಟ್ಟು_h
	|| wp->w_popup_leftclip >= ಒಟ್ಟು_w
	|| wp->w_popup_rightclip >= ಒಟ್ಟು_w;
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
    ಆದರೆ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_ಎತ್ತರ -= cl->clip_ತುದಿ_content + cl->clip_bot_content;
	ಆದರೆ (wp->w_ಎತ್ತರ < 0)
	    wp->w_ಎತ್ತರ = 0;
	ಆದರೆ (cl->clip_ತುದಿ_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_ತುದಿ_content;
    }
    ಆದರೆ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	ಆದರೆ (cl->clip_ಎಡ_content > 0 || cl->clip_ಬಲ_content > 0)
	    wp->w_p_wrap = FALSE;
	ಆದರೆ (cl->clip_ಬಲ_content > 0)
	{
	    wp->w_ಅಗಲ -= cl->clip_ಬಲ_content;
	    ಆದರೆ (wp->w_ಅಗಲ < 0)
		wp->w_ಅಗಲ = 0;
	}
	ಆದರೆ (cl->clip_ಎಡ_content > 0)
	{
	    wp->w_leftcol += cl->clip_ಎಡ_content;
	    wp->w_wincol += cl->clip_ಎಡ_content;
	    wp->w_ಅಗಲ -= cl->clip_ಎಡ_content;
	    ಆದರೆ (wp->w_ಅಗಲ < 0)
		wp->w_ಅಗಲ = 0;
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_position_image(win_T *wp)
{
    linenr_T	    ಅಡ್ಡಸಾಲು;
    colnr_T	    ಲಂಬಸಾಲು;

    ಪೂರ್ಣಾಂಕ iw, ih;

    // Visible cell width and height of popup itself (not image!)
    ಪೂರ್ಣಾಂಕ	visible_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	visible_ಎತ್ತರ;

    // Crop dimensions of image (in pixels).
    ಪೂರ್ಣಾಂಕ	crop_x, crop_y, crop_ಅಗಲ, crop_ಎತ್ತರ;

    popup_clip_T clip;

    ಆದರೆ (wp->w_popup_imagep == ಖಾಲಿ || wp->w_popup_flags & POPF_HIDDEN)
	ಹಿಂತಿರುಗಿ;

    image_placement_ಗಣ_enable(wp->w_popup_imagep, true);

    ಅಡ್ಡಸಾಲು = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    ಲಂಬಸಾಲು = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    ಆದರೆ (wp->w_popup_imagep->img == ಖಾಲಿ)
	// Just set the bounding box
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    visible_ಅಗಲ =
	wp->w_ಅಗಲ - clip.clip_ಎಡ_content - clip.clip_ಬಲ_content;
    visible_ಎತ್ತರ =
	wp->w_ಎತ್ತರ - clip.clip_ತುದಿ_content - clip.clip_bot_content;

    ಆದರೆ (visible_ಅಗಲ < 0 || visible_ಎತ್ತರ < 0)
	ಹಿಂತಿರುಗಿ;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ಎಡ_content;
    crop_y = clip.clip_ತುದಿ_content;
    crop_ಅಗಲ = iw - crop_x - clip.clip_ಬಲ_content;
    crop_ಎತ್ತರ = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    ಅಡ್ಡಸಾಲು -= wp->w_popup_border[0] - clip.eff_border[0];
    ಅಡ್ಡಸಾಲು -= wp->w_popup_padding[0] - clip.eff_padding[0];

    ಲಂಬಸಾಲು -= wp->w_popup_border[3] - clip.eff_border[3];
    ಲಂಬಸಾಲು -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    ಆದರೆ (crop_ಅಗಲ > visible_ಅಗಲ)
	crop_ಅಗಲ = visible_ಅಗಲ;
    ಆದರೆ (crop_ಎತ್ತರ > visible_ಎತ್ತರ)
	crop_ಎತ್ತರ = visible_ಎತ್ತರ;

    image_placement_ಗಣ_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_ಅಗಲ, crop_ಎತ್ತರ);
ನಿರ್ಗಮಿಸು:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_ಗಣ_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_ಗಣ_position(wp->w_popup_imagep, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    image_placement_ಗಣ_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_ಎತ್ತರ, clip.eff_ಅಗಲ);
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		wrapped = 0;
    ಪೂರ್ಣಾಂಕ		maxwidth;
    ಪೂರ್ಣಾಂಕ		maxwidth_no_scrollbar;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ_with_scrollbar = 0;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_maxwidth = FALSE;
    ಪೂರ್ಣಾಂಕ		margin_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ		maxspace;
    ಪೂರ್ಣಾಂಕ		center_vert = FALSE;
    ಪೂರ್ಣಾಂಕ		center_hor = FALSE;
    ಪೂರ್ಣಾಂಕ		allow_adjust_ಎಡ = !wp->w_popup_fixed;
    ಪೂರ್ಣಾಂಕ		ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    ಪೂರ್ಣಾಂಕ		ಬಲ_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    ಪೂರ್ಣಾಂಕ		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    ಪೂರ್ಣಾಂಕ		ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    ಪೂರ್ಣಾಂಕ		extra_ಎತ್ತರ = ತುದಿ_extra + bot_extra;
    ಪೂರ್ಣಾಂಕ		extra_ಅಗಲ = ಎಡ_extra + ಬಲ_extra;
    ಪೂರ್ಣಾಂಕ		w_ಎತ್ತರ_before_ಎಲ್ಲೆ;
    popup_layout_T org_layout;
    ಪೂರ್ಣಾಂಕ		minwidth, minheight;
    ಪೂರ್ಣಾಂಕ		maxheight = ಅಡ್ಡಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		wantline = wp->w_wantline;  // adjusted for textprop
    ಪೂರ್ಣಾಂಕ		wantcol = wp->w_wantcol;    // adjusted for textprop
    ಪೂರ್ಣಾಂಕ		use_wantcol = wantcol != 0;
    ಪೂರ್ಣಾಂಕ		adjust_ಎತ್ತರ_for_ತುದಿ_aligned = FALSE;

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
    ಆದರೆ (wp->w_popup_ಕೊನೆಯ_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    ಆದರೆ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	ಪೂರ್ಣಾಂಕ	    screen_ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ	    screen_scol;
	ಪೂರ್ಣಾಂಕ	    screen_ccol;
	ಪೂರ್ಣಾಂಕ	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_ತುದಿ = false;
	ಆದರೆ (!ಕಂಡುಹಿಡಿ_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    ಆದರೆ (popup_ಕಂಡುಹಿಡಿ_prop_above_ತುದಿ(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_ತುದಿ = true;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
		    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
			image_placement_clear(wp->w_popup_imagep);
#ಆದರೆ_ಅಂತ್ಯ
		    popup_hide_for_textprop(wp);
		    ಆದರೆ (wp->w_winrow + popup_ಎತ್ತರ(wp) >= cmdline_ಅಡ್ಡಸಾಲು)
			clear_cmdline = TRUE;
		}
		ಹಿಂತಿರುಗಿ;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.ಲಂಬಸಾಲು = prop.tp_ಲಂಬಸಾಲು;
	ಆದರೆ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.ಲಂಬಸಾಲು += prop.tp_len - 1;
	ಆದರೆ (prop_above_ತುದಿ)
	    popup_screenpos_above_ತುದಿ(prop_win, &pos, prop_lnum, &screen_ಅಡ್ಡಸಾಲು,
				     &screen_scol, &screen_ccol, &screen_ecol);
	ಇಲ್ಲದಿದ್ದರೆ
	    textpos2screenpos(prop_win, &pos, &screen_ಅಡ್ಡಸಾಲು,
				     &screen_scol, &screen_ccol, &screen_ecol);

	ಆದರೆ (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_ಅಗಲ = 0;
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_ಅಡ್ಡಸಾಲು + wantline + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    // above the text
	    wantline = screen_ಅಡ್ಡಸಾಲು + wantline - 1;
	center_vert = FALSE;
	ಆದರೆ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	ಇಲ್ಲದಿದ್ದರೆ
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// If no line was specified default to vertical centering.
	ಆದರೆ (wantline == 0)
	    center_vert = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	ಆದರೆ (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    ಆದರೆ (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    ಆದರೆ (wp->w_winrow >= ಅಡ್ಡಸಾಲುಗಳು)
		wp->w_winrow = ಅಡ್ಡಸಾಲುಗಳು - 1;
	}
	ಆದರೆ (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_ಅಡ್ಡಸಾಲು
				    - wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - 1, 0);
	}

	ಆದರೆ (!use_wantcol)
	    center_hor = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    ಆದರೆ (wp->w_wincol > firstwin->w_wincol + topframe->fr_ಅಗಲ
						- ಎಡ_extra - ಬಲ_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_ಅಗಲ
						- ಎಡ_extra - ಬಲ_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    ಆದರೆ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_ಅಗಲ
					- wp->w_wincol - ಎಡ_extra - ಬಲ_extra;
    maxwidth = maxspace;
    ಆದರೆ (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ಎಡ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    ಆದರೆ (wp->w_p_nu || wp->w_p_rnu)
	margin_ಅಗಲ = ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    margin_ಅಗಲ += wp->w_p_fdc;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    ಆದರೆ (signcolumn_on(wp))
	margin_ಅಗಲ += 2;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (margin_ಅಗಲ >= maxwidth)
	margin_ಅಗಲ = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
    {
	ಆದರೆ (minwidth == 0)
	    minwidth = 20;
	ಆದರೆ (minheight == 0)
	    minheight = 5;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_ಅಡ್ಡಸಾಲು - 1;

    // start at the desired first line
    ಆದರೆ (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	ಆದರೆ (wp->w_firstline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    wp->w_firstline = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_topline = wp->w_firstline;
    }
    ಆದರೆ (wp->w_topline < 1)
	wp->w_topline = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_topline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	wp->w_topline = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_ಅಗಲ = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	ಪೂರ್ಣಾಂಕ saved_w_ಅಗಲ = wp->w_ಅಗಲ;

	ಆದರೆ (wp->w_ಅಗಲ < maxwidth)
	    wp->w_ಅಗಲ = maxwidth;
	ಗಾಗಿ (ln = 1; ln <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++ln)
	{
	    ಪೂರ್ಣಾಂಕ len = linetabsize(wp, ln) + margin_ಅಗಲ;

	    ಆದರೆ (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    ಆದರೆ (saved_w_ಅಗಲ < len)
		saved_w_ಅಗಲ = len;
	    ಆದರೆ (wp->w_maxwidth > 0 && saved_w_ಅಗಲ >= wp->w_maxwidth)
		ನಿಲ್ಲಿಸು;
	}
	wp->w_ಅಗಲ = saved_w_ಅಗಲ;
    }
    ಆದರೆ (wp->w_firstline < 0)
	lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ
	lnum = wp->w_topline;
    ಆಗ (lnum >= 1 && lnum <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಪೂರ್ಣಾಂಕ len;
	ಪೂರ್ಣಾಂಕ w_ಅಗಲ = wp->w_ಅಗಲ;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	ಆದರೆ (wp->w_ಅಗಲ < maxwidth)
	    wp->w_ಅಗಲ = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_ಅಗಲ = w_ಅಗಲ;

	ಆದರೆ (len + margin_ಅಗಲ > maxwidth
		&& allow_adjust_ಎಡ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    ಪೂರ್ಣಾಂಕ ಸರಿಸು_by = len + margin_ಅಗಲ - maxwidth;

	    ಆದರೆ (ಸರಿಸು_by > wp->w_wincol)
	    {
		ಪೂರ್ಣಾಂಕ truncate_ಸರಿಸು = ಸರಿಸು_by - wp->w_wincol;

		ಸರಿಸು_by -= truncate_ಸರಿಸು;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    ಆದರೆ (wp->w_maxwidth > 0 && maxwidth + ಸರಿಸು_by > wp->w_maxwidth)
		ಸರಿಸು_by = wp->w_maxwidth - maxwidth;

	    ಆದರೆ (ಸರಿಸು_by > 0)
	    {
		wp->w_wincol -= ಸರಿಸು_by;
		maxwidth += ಸರಿಸು_by;
	    }
	    wp->w_ಅಗಲ = maxwidth;
	}
	ಆದರೆ (wp->w_p_wrap)
	{
	    ಆಗ (len + margin_ಅಗಲ > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_ಅಗಲ;
		wp->w_ಅಗಲ = maxwidth;
		ಬಳಸಿದ_maxwidth = TRUE;
	    }
	}
	ಆದರೆ (wp->w_ಅಗಲ < len + margin_ಅಗಲ)
	{
	    wp->w_ಅಗಲ = len + margin_ಅಗಲ;
	    ಆದರೆ (wp->w_maxwidth > 0 && wp->w_ಅಗಲ > wp->w_maxwidth)
		wp->w_ಅಗಲ = wp->w_maxwidth;
	}

	ಆದರೆ (wp->w_firstline < 0)
	    --lnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	ಆದರೆ (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum)
		       + wrapped >= maxheight)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ && !ಪದ_is_ಪೂರ್ಣಗೊಂಡ(wp->w_ಬಫರ್))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#ಆದರೆ_ಅಂತ್ಯ
    maxwidth_no_scrollbar = maxwidth;
    ಆದರೆ (wp->w_has_scrollbar)
    {
	++ಬಲ_extra;
	++extra_ಅಗಲ;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	ಆದರೆ (maxwidth + ಬಲ_extra >= maxspace
		&& (ಬಳಸಿದ_maxwidth || (minwidth > 0 && wp->w_ಅಗಲ < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    ಆದರೆ (wp->w_popup_title != ಖಾಲಿ && *wp->w_popup_title != NUL)
    {
	ಪೂರ್ಣಾಂಕ title_len = vim_strsize(wp->w_popup_title) + 2 - extra_ಅಗಲ;

	ಆದರೆ (minwidth < title_len)
	    minwidth = title_len;
    }

    ಆದರೆ (minwidth > 0 && wp->w_ಅಗಲ < minwidth)
	wp->w_ಅಗಲ = minwidth;
    ಆದರೆ (wp->w_ಅಗಲ > maxwidth)
    {
	ಆದರೆ (wp->w_ಅಗಲ > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_ಅಗಲ - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	ಆದರೆ (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    ಪೂರ್ಣಾಂಕ off = wp->w_ಅಗಲ - maxwidth;
	    extra_ಅಗಲ -= MIN(off, ಬಲ_extra);
	    wp->w_ಅಗಲ = maxwidth_no_scrollbar;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_ಅಗಲ = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    ಅಗಲ_with_scrollbar = maxwidth_no_scrollbar - ಬಲ_extra;
	}
    }
    ಆದರೆ (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_ಅಗಲ - wp->w_ಅಗಲ - extra_ಅಗಲ) / 2;
	ಆದರೆ (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	ಪೂರ್ಣಾಂಕ leftoff = wantcol - (wp->w_ಅಗಲ + extra_ಅಗಲ);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	ಆದರೆ (leftoff >= 0)
	    wp->w_wincol = leftoff;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_ಅಗಲ -= wp->w_leftcol;
	    ಆದರೆ (wp->w_ಅಗಲ < 0)
		wp->w_ಅಗಲ = 0;
	}
    }

    ಆದರೆ (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	ಪೂರ್ಣಾಂಕ want_ಲಂಬಸಾಲು = 0;

	// try to show the right border and any scrollbar
	want_ಲಂಬಸಾಲು = ಎಡ_extra + wp->w_ಅಗಲ + ಬಲ_extra;
	ಆದರೆ (want_ಲಂಬಸಾಲು > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_ಲಂಬಸಾಲು >= firstwin->w_wincol + topframe->fr_ಅಗಲ)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_ಅಗಲ - want_ಲಂಬಸಾಲು;
	    ಆದರೆ (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_ಎತ್ತರ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - wp->w_topline
								 + 1 + wrapped;
    ಆದರೆ (minheight > 0 && wp->w_ಎತ್ತರ < minheight)
	wp->w_ಎತ್ತರ = minheight;
    ಆದರೆ (maxheight > 0 && wp->w_ಎತ್ತರ > maxheight)
	wp->w_ಎತ್ತರ = maxheight;
    w_ಎತ್ತರ_before_ಎಲ್ಲೆ = wp->w_ಎತ್ತರ;
    ಆದರೆ (wp->w_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು - wp->w_winrow)
	wp->w_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು - wp->w_winrow;

    ಆದರೆ (center_vert)
    {
	wp->w_winrow = (ಅಡ್ಡಸಾಲುಗಳು - wp->w_ಎತ್ತರ - extra_ಎತ್ತರ) / 2;
	ಆದರೆ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	ಆದರೆ ((wp->w_ಎತ್ತರ + extra_ಎತ್ತರ) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_ಎತ್ತರ + extra_ಎತ್ತರ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wantline * 2 >= ಅಡ್ಡಸಾಲುಗಳು || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_ಎತ್ತರ = wantline - extra_ಎತ್ತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_ಎತ್ತರ_for_ತುದಿ_aligned = TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	ಪೂರ್ಣಾಂಕ check_ಎತ್ತರ = (wp->w_popup_flags & POPF_INFO) ? wp->w_ಎತ್ತರ
						    : w_ಎತ್ತರ_before_ಎಲ್ಲೆ;
	ಆದರೆ (wp != popup_dragwin
		&& wantline + (check_ಎತ್ತರ + extra_ಎತ್ತರ) - 1 > ಅಡ್ಡಸಾಲುಗಳು
		&& wantline * 2 > ಅಡ್ಡಸಾಲುಗಳು
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_ಎತ್ತರ = w_ಎತ್ತರ_before_ಎಲ್ಲೆ;
	    wp->w_winrow = wantline - 2 - wp->w_ಎತ್ತರ - extra_ಎತ್ತರ;
	    ಆದರೆ (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_ಎತ್ತರ + 2;
	    ಆದರೆ (wp->w_winrow < 0)
	    {
		wp->w_ಎತ್ತರ += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_winrow = wantline - 1;
	    adjust_ಎತ್ತರ_for_ತುದಿ_aligned = TRUE;
	}
    }

    ಆದರೆ (adjust_ಎತ್ತರ_for_ತುದಿ_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_ಎತ್ತರ + extra_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು - wp->w_winrow - extra_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ == ಖಾಲಿ || ಪದ_is_ಪೂರ್ಣಗೊಂಡ(wp->w_ಬಫರ್))
#ಆದರೆ_ಅಂತ್ಯ
	{
	    // The width already accounts for a scrollbar that was there.
	    ಆದರೆ (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		ಆದರೆ (ಅಗಲ_with_scrollbar > 0)
		    wp->w_ಅಗಲ = ಅಗಲ_with_scrollbar;
		ಇಲ್ಲದಿದ್ದರೆ
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_ಅಗಲ;
	    }
	}
    }

    // make sure w_winrow is valid
    ಆದರೆ (wp->w_winrow >= ಅಡ್ಡಸಾಲುಗಳು)
	wp->w_winrow = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    ಆದರೆ (wp->w_wincol + wp->w_ಅಗಲ + extra_ಅಗಲ
				    > firstwin->w_wincol + topframe->fr_ಅಗಲ)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_ಅಗಲ
						- wp->w_ಅಗಲ - extra_ಅಗಲ;
    ಆದರೆ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    ಆದರೆ (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    ಆದರೆ (wp->w_wincol + wp->w_ಅಗಲ + extra_ಅಗಲ
				    > firstwin->w_wincol + topframe->fr_ಅಗಲ)
    {
	ಪೂರ್ಣಾಂಕ avail = firstwin->w_wincol + topframe->fr_ಅಗಲ
						- wp->w_wincol - extra_ಅಗಲ;
	wp->w_ಅಗಲ = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    ಆದರೆ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	ಆದರೆ (wp->w_winrow + wp->w_ಎತ್ತರ + extra_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು)
	    wp->w_winrow = ಅಡ್ಡಸಾಲುಗಳು - wp->w_ಎತ್ತರ - extra_ಎತ್ತರ;
	ಆದರೆ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	ಆದರೆ (wp->w_winrow + wp->w_ಎತ್ತರ + extra_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು)
	{
	    ಪೂರ್ಣಾಂಕ avail = ಅಡ್ಡಸಾಲುಗಳು - wp->w_winrow - extra_ಎತ್ತರ;
	    wp->w_ಎತ್ತರ = avail > 0 ? avail : 0;
	}
    }

    ಆದರೆ (wp->w_ಎತ್ತರ != org_layout.ಎತ್ತರ)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    ಆದರೆ (popup_compute_clipwindow_offsets(wp))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
	ಆದರೆ (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
		image_placement_clear(wp->w_popup_imagep);
#ಆದರೆ_ಅಂತ್ಯ

	popup_hide_for_textprop(wp);
	ಹಿಂತಿರುಗಿ;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_ಕೊನೆಯ_changedtick = CHANGEDTICK(wp->w_ಬಫರ್);
    ಆದರೆ (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_ಬಫರ್);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_ಅಗಲ = wp->w_popup_prop_win->w_ಅಗಲ;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_ಎತ್ತರ;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    ಆದರೆ (popup_layout_ಬದಲಾಯಿತು(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	ಆದರೆ (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_ಮುಖವಾಡ_refresh = TRUE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    popup_position_image(wp);
#ಆದರೆ_ಅಂತ್ಯ
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    TYPE_ಸಾಮಾನ್ಯ,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_ಸಂದೇಶ_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_is_notification(create_type_T type)
{
    ಹಿಂತಿರುಗಿ type == TYPE_NOTIFICATION || type == TYPE_ಸಂದೇಶ_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_ಗಣ_ಬಫರ್_ಪಠ್ಯ(buf_T *buf, typval_T ಪಠ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    ಗಾಗಿ (lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_ಬಫರ್;

    // Add text to the buffer.
    ಆದರೆ (ಪಠ್ಯ.v_type == VAR_STRING)
    {
	char_u *s = ಪಠ್ಯ.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ಖಾಲಿ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	list_T *l = ಪಠ್ಯ.vval.v_list;

	ಆದರೆ (l != ಖಾಲಿ && l->lv_len > 0)
	{
	    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
		emsg(_(e_using_ಸಂಖ್ಯೆ_as_string));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l->lv_ಮೊದಲ->li_tv.v_type == VAR_STRING)
		// list of strings
		ಕೂಡಿಸು_popup_ಅಕ್ಷರಸರಣಿಗಳು(buf, l);
	    ಇಲ್ಲದಿದ್ದರೆ
		// list of dictionaries
		ಕೂಡಿಸು_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    curbuf = curwin->w_ಬಫರ್;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    ಮಾಡು {						    \
	ಆದರೆ (wp != ಖಾಲಿ)					    \
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
    } ಆಗ (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_popup_option(win_T *wp, ಪೂರ್ಣಾಂಕ is_preview)
{
    char_u *p =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	!is_preview ? p_cpp :
#ಆದರೆ_ಅಂತ್ಯ
	p_pvp;
    ಪೂರ್ಣಾಂಕ	    border_enabled = FALSE;

    ಆದರೆ (wp != ಖಾಲಿ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    ಗಾಗಿ ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	ಪೂರ್ಣಾಂಕ	x;

	e = vim_strchr(p, ':');
	ಆದರೆ (e == ಖಾಲಿ || e[1] == NUL)
	    ಹಿಂತಿರುಗಿ FAIL;

	p = vim_strchr(e, ',');
	ಆದರೆ (p == ಖಾಲಿ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	ಆದರೆ (STRNCMP(s, "height:", 7) == 0)
	{
	    ಆದರೆ (dig != p)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಆದರೆ (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "width:", 6) == 0)
	{
	    ಆದರೆ (dig != p)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಆದರೆ (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "highlight:", 10) == 0)
	{
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಅಕ್ಷರ *errmsg;
		ಪೂರ್ಣಾಂಕ c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		ಆದರೆ (errmsg == ಖಾಲಿ)
		    ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    ಆದರೆ (*arg == NUL || *arg == ',')
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಂಕ		i;
	    ಪೂರ್ಣಾಂಕ		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    ಪೂರ್ಣಾಂಕ		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    ಆದರೆ (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		ಆದರೆ (wp != ಖಾಲಿ)
		{
		    ಗಾಗಿ (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    ಗಣ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		ಮುಂದುವರಿ;
	    }

	    token = vim_strnsave(arg, token_len);
	    ಆದರೆ (token == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		ಆದರೆ (STRCMP(token, "single") == 0)
		    ಗಣ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(token, "double") == 0)
		    ಗಣ_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(token, "round") == 0)
		    ಗಣ_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(token, "on") == 0)
		    ಗಣ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(token, "ascii") == 0)
		    ಗಣ_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    ಪೂರ್ಣಾಂಕ		ಹೊರಹೋಗು[8];
		    ಪೂರ್ಣಾಂಕ		failed = FALSE;

		    ಗಣ_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    ಗಾಗಿ (i = 0; i < 8 && !failed; i++)
		    {
			ಆದರೆ (*q == NUL)
			    failed = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಹೊರಹೋಗು[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    ಆದರೆ (i < 7)
			    {
				ಆದರೆ (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    ಆದರೆ (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ಗಣ_BORDER_CHARS(ಹೊರಹೋಗು[0], ಹೊರಹೋಗು[1], ಹೊರಹೋಗು[2], ಹೊರಹೋಗು[3], ಹೊರಹೋಗು[4],
			    ಹೊರಹೋಗು[5], ಹೊರಹೋಗು[6], ಹೊರಹೋಗು[7]);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_free(token);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಗಾಗಿ (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    ಪೂರ್ಣಾಂಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಂಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಆದರೆ (!on && !off)
		ಹಿಂತಿರುಗಿ FAIL;
	    on = on && mouse_has(MOUSE_ಸೇರಿಸು) && (border_enabled || is_preview);
	    ಆದರೆ (wp != ಖಾಲಿ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಂಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಂಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಆದರೆ (!on && !off)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಆದರೆ (on && mouse_has(MOUSE_ಸೇರಿಸು))
		    wp->w_popup_flags |= POPF_RESIZE;
		ಇಲ್ಲದಿದ್ದರೆ
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ಪೂರ್ಣಾಂಕ		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ಪೂರ್ಣಾಂಕ		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ಆದರೆ (!on && !off)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    ಪೂರ್ಣಾಂಕ		ಅಂಶ = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    ಪೂರ್ಣಾಂಕ		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    ಆದರೆ (!menu && !ಅಂಶ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(s, "opacity:", 8) == 0)
	{
	    ಆದರೆ (dig != p || x < 0 || x > 100)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (wp != ಖಾಲಿ)
	    {
		ಆದರೆ (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		ಇಲ್ಲದಿದ್ದರೆ
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಪೂರ್ಣಾಂಕ
parse_previewpopup(win_T *wp)
{
    ಹಿಂತಿರುಗಿ parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ಪೂರ್ಣಾಂಕ
parse_completepopup(win_T *wp)
{
    ಹಿಂತಿರುಗಿ parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    ಶೂನ್ಯ
popup_ಗಣ_wantpos_cursor(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಗಲ, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    ಆದರೆ (d != ಖಾಲಿ)
	ppt = get_pos_ನಮೂದು(d, FALSE);

    setcursor_mayforce(TRUE);
    ಆದರೆ (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	ಆದರೆ (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    ಆದರೆ (wp->w_wantcol > Columns - ಅಗಲ)
    {
	wp->w_wantcol = Columns - ಅಗಲ;
	ಆದರೆ (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    ಶೂನ್ಯ
popup_ಗಣ_wantpos_rowcol(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    wp->w_wantline = ಅಡ್ಡಸಾಲು;
    wp->w_wantcol = ಲಂಬಸಾಲು;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_border_ಎಡ_ಬಲ_padding(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_terminal_exists(ಶೂನ್ಯ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    ಶೂನ್ಯ
popup_redraw_all(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    ಪೂರ್ಣಾಂಕ
popup_need_redraw(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set the color for a notification window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_update_color(win_T *wp, create_type_T type)
{
    ಅಕ್ಷರ    *hiname = type == TYPE_ಸಂದೇಶ_WIN
				       ? "MessageWindow" : "PopupNotification";
    ಅಕ್ಷರ    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    ಆದರೆ (errmsg == ಖಾಲಿ)
	ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    ಇಲ್ಲದಿದ್ದರೆ
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
    tabpage_T	*tp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		tabnr = 0;
    ಪೂರ್ಣಾಂಕ		new_ಬಫರ್;
    buf_T	*buf = ಖಾಲಿ;
    dict_T	*d = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (argvars != ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script()
		&& (check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// Check that arguments look OK.
	ಆದರೆ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    buf = buflist_findnr(argvars[0].vval.v_ಸಂಖ್ಯೆ);
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		semsg(_(e_ಬಫರ್_nr_does_not_exist), argvars[0].vval.v_ಸಂಖ್ಯೆ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_ಎರಡನೇ_popup_with_terminal));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ಖಾಲಿ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ಖಾಲಿ))
	{
	    emsg(_(e_ಬಫರ್_ಸಂಖ್ಯೆ_ಪಠ್ಯ_or_list_required));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	d = argvars[1].vval.v_dict;
    }

    ಆದರೆ (d != ಖಾಲಿ)
    {
	ಆದರೆ (dict_has_ಕೀಲಿ(d, "tabpage"))
	    tabnr = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "tabpage");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	ಇಲ್ಲದಿದ್ದರೆ
	    tabnr = 0;
	ಆದರೆ (tabnr > 0)
	{
	    tp = ಕಂಡುಹಿಡಿ_tabpage(tabnr);
	    ಆದರೆ (tp == ಖಾಲಿ)
	    {
		semsg(_(e_tabpage_not_ಸಿಕ್ಕಿತು_nr), tabnr);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    ಆದರೆ (buf != ಖಾಲಿ && buf->b_locked_ವಿಭಜಿಸು)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_ಕಿಟಕಿ_ಗಮ್ಯ_a_closing_ಬಫರ್));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (rettv != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    ಆದರೆ (buf != ಖಾಲಿ)
    {
	// use existing buffer
	new_ಬಫರ್ = FALSE;
	win_init_popup_win(wp, buf);
	ಗಣ_local_options_default(wp, FALSE);
	ಅದಲುಬದಲು_exists_action = SEA_READONLY;
	ಬಫರ್_ensure_loaded(buf);
	ಅದಲುಬದಲು_exists_action = SEA_NONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// create a new buffer associated with the popup
	new_ಬಫರ್ = TRUE;
	buf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    win_free_popup(wp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	ಗಣ_local_options_default(wp, TRUE);
	ಗಣ_string_option_ನೇರ_ಒಳ_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	ಗಣ_string_option_ನೇರ_ಒಳ_buf(buf, (char_u *)"bufhidden", -1,
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

    ಆದರೆ (tp != ಖಾಲಿ)
    {
	// popup on specified tab page
	wp->w_ಮುಂದಿನ = tp->tp_ಮೊದಲ_popupwin;
	tp->tp_ಮೊದಲ_popupwin = wp;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tabnr == 0)
    {
	// popup on current tab page
	wp->w_ಮುಂದಿನ = curtab->tp_ಮೊದಲ_popupwin;
	curtab->tp_ಮೊದಲ_popupwin = wp;
    }
    ಇಲ್ಲದಿದ್ದರೆ // (tabnr < 0)
    {
	win_T *prev = ಮೊದಲ_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	ಆದರೆ (ಮೊದಲ_popupwin == ಖಾಲಿ)
	    ಮೊದಲ_popupwin = wp;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (prev->w_ಮುಂದಿನ != ಖಾಲಿ)
		prev = prev->w_ಮುಂದಿನ;
	    prev->w_ಮುಂದಿನ = wp;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (new_ಬಫರ್ && argvars != ಖಾಲಿ)
	popup_ಗಣ_ಬಫರ್_ಪಠ್ಯ(buf, argvars[0]);

    ಆದರೆ (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    ಆದರೆ (type == TYPE_ATCURSOR)
    {
	popup_ಗಣ_wantpos_cursor(wp, 0, d);
	ಗಣ_moved_ಮೌಲ್ಯಗಳು(wp);
	ಗಣ_moved_columns(wp, ಕಂಡುಹಿಡಿ_STRING);
    }

    ಆದರೆ (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_ಅಡ್ಡಸಾಲು;
	ಆದರೆ (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_ಲಂಬಸಾಲು + 1;
	ಗಣ_mousemoved_ಮೌಲ್ಯಗಳು(wp);
	ಗಣ_mousemoved_columns(wp, ಕಂಡುಹಿಡಿ_IDENT + ಕಂಡುಹಿಡಿ_STRING + ಕಂಡುಹಿಡಿ_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    ಆದರೆ (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	ಪೂರ್ಣಾಂಕ	ಎತ್ತರ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	ಗಾಗಿ (twp = ಮೊದಲ_popupwin; twp != ಖಾಲಿ; twp = nextwin)
	{
	    nextwin = twp->w_ಮುಂದಿನ;
	    ಆದರೆ (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + ಎತ್ತರ
		    && twp->w_winrow + popup_ಎತ್ತರ(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_ಎತ್ತರ(twp) + 1;
		nextwin = ಮೊದಲ_popupwin;
	    }
	}
	ಆದರೆ (wp->w_wantline + ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು)
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
	ಗಾಗಿ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    ಆದರೆ (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	ಕೂಡಿಸು_border_ಎಡ_ಬಲ_padding(wp);
    }

    ಆದರೆ (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	ಆದರೆ (callback.cb_name != ಖಾಲಿ)
	{
	    ಗಣ_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    ಗಾಗಿ (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    ಗಾಗಿ (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    ಆದರೆ (type == TYPE_PREVIEW)
    {
	ಆದರೆ (mouse_has(MOUSE_ಸೇರಿಸು))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ಗಾಗಿ (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_ಗಣ_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಆದರೆ (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	ಆದರೆ (mouse_has(MOUSE_ಸೇರಿಸು))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	ಕೂಡಿಸು_border_ಎಡ_ಬಲ_padding(wp);
	parse_completepopup(wp);
    }
#ಆದರೆ_ಅಂತ್ಯ

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_ಬಹುಲಕ = ಬಹುಲಕ_ALL;

    ಆದರೆ (d != ಖಾಲಿ)
    {
	// Deal with options.
	ಆದರೆ (apply_options(wp, d, TRUE) == FAIL)
	{
	    (ಶೂನ್ಯ)popup_close(wp->w_id, FALSE);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಆದರೆ (popup_is_notification(type) && wp->w_popup_timer == ಖಾಲಿ)
	popup_ಕೂಡಿಸು_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#ಆದರೆ_ಅಂತ್ಯ

    popup_adjust_position(wp);

    wp->w_vsep_ಅಗಲ = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_ಮುಖವಾಡ_refresh = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ)
	win_enter(wp, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ wp;
}

/*
 * popup_clear()
 */
    ಶೂನ್ಯ
f_popup_clear(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ force = FALSE;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	force = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    ಶೂನ್ಯ
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ಸಾಮಾನ್ಯ);
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
invoke_popup_callback(win_T *wp, typval_T *ಫಲಿತಾಂಶ)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)wp->w_id;

    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && ಫಲಿತಾಂಶ->v_type != VAR_UNKNOWN)
	copy_tv(ಫಲಿತಾಂಶ, &argv[1]);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	argv[1].vval.v_ಸಂಖ್ಯೆ = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಹಿಂಭಾಗ_ಗಮ್ಯ_prevwin(win_T *wp)
{
    ಆದರೆ (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    ಪೂರ್ಣಾಂಕ id;

    ಆದರೆ (!win_valid(wp))
       ಹಿಂತಿರುಗಿ;

    id = wp->w_id;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (wp == curwin && curbuf->b_ಪದ != ಖಾಲಿ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    ಆದರೆ (owp != curwin && owp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (owp != ಖಾಲಿ)
	    win_enter(owp, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, owp)
		ಆದರೆ (owp != curwin && owp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (owp != ಖಾಲಿ)
		win_enter(owp, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂಭಾಗ_ಗಮ್ಯ_prevwin(wp);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Just in case a check higher up is missing.
    ಆದರೆ (wp == curwin && ERROR_IF_POPUP_ಕಿಟಕಿ)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	ಹಿಂತಿರುಗಿ;
    }

    CHECK_CURBUF;
    ಆದರೆ (wp->w_close_cb.cb_name != ಖಾಲಿ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    ಶೂನ್ಯ
popup_close_with_retval(win_T *wp, ಪೂರ್ಣಾಂಕ retval)
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
    ಆದರೆ (wp != mouse_wp
	    && wp->w_popup_mouse_ಅಡ್ಡಸಾಲು != 0
	    && (wp->w_popup_mouse_ಅಡ್ಡಸಾಲು != mouse_ಅಡ್ಡಸಾಲು
		|| mouse_ಲಂಬಸಾಲು < wp->w_popup_mouse_mincol
		|| mouse_ಲಂಬಸಾಲು > wp->w_popup_mouse_maxcol))
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
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

    // find the window where the mouse is in
    mouse_wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);

    ಗಾಗಿ (wp = ಮೊದಲ_popupwin; wp != ಖಾಲಿ; wp = nextwp)
    {
	nextwp = wp->w_ಮುಂದಿನ;
	check_mouse_moved(wp, mouse_wp);
    }
    ಗಾಗಿ (wp = curtab->tp_ಮೊದಲ_popupwin; wp != ಖಾಲಿ; wp = nextwp)
    {
	nextwp = wp->w_ಮುಂದಿನ;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
filter_handle_drag(win_T *wp, ಪೂರ್ಣಾಂಕ c, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

    ಆದರೆ ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_ಕೀಲಿ(c)
	    && (wp == popup_dragwin
			  || wp == mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    ಶೂನ್ಯ
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    char_u	*ಕೀಲಿ;
    typval_T	res;
    ಪೂರ್ಣಾಂಕ		c;
    linenr_T	old_lnum;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = win_id2wp(id);
    ಕೀಲಿ = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    c = *ಕೀಲಿ;
    ಆದರೆ (c == K_SPECIAL && ಕೀಲಿ[1] != NUL)
	c = ಗಮ್ಯ_SPECIAL(ಕೀಲಿ[1], ಕೀಲಿ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
    res.v_type = VAR_ಸಂಖ್ಯೆ;

    old_lnum = wp->w_cursor.lnum;
    ಆದರೆ (c == 'k' || c == 'K' || c == K_ಮೇಲೆ || c == Ctrl_P)
    {
	ಆದರೆ (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_cursor.lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }
    ಆದರೆ (c == 'j' || c == 'J' || c == K_ಕೆಳಕ್ಕೆ || c == Ctrl_N)
    {
	ಆದರೆ (wp->w_cursor.lnum < wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ++wp->w_cursor.lnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_cursor.lnum = 1;
    }
    ಆದರೆ (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_ಸಂಖ್ಯೆ = -1;
	popup_close_and_callback(wp, &res);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_ಸಂಖ್ಯೆ = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	ಹಿಂತಿರುಗಿ;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    ಶೂನ್ಯ
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    char_u	*ಕೀಲಿ;
    typval_T	res;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = win_id2wp(id);
    ಕೀಲಿ = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    c = *ಕೀಲಿ;
    ಆದರೆ (c == CAR && need_wait_return)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (c == K_SPECIAL && ಕೀಲಿ[1] != NUL)
	c = ಗಮ್ಯ_SPECIAL(ಕೀಲಿ[1], ಕೀಲಿ[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;

    ಆದರೆ (c == 'y' || c == 'Y')
	res.vval.v_ಸಂಖ್ಯೆ = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_ಸಂಖ್ಯೆ = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	filter_handle_drag(wp, c, rettv);
	ಹಿಂತಿರುಗಿ;
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
ಕಂಡುಹಿಡಿ_popup_win(ಪೂರ್ಣಾಂಕ id)
{
    win_T *wp = win_id2wp(id);

    ಆದರೆ (wp != ಖಾಲಿ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_ಕಿಟಕಿ_nr_is_not_popup_ಕಿಟಕಿ), id);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ wp;
}

/*
 * popup_close({id})
 */
    ಶೂನ್ಯ
f_popup_close(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_ಪದ == ಖಾಲಿ &&
#ಆದರೆ_ಅಂತ್ಯ
	    ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp != ಖಾಲಿ)
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
popup_clear_ಮುಖವಾಡ_for(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ r, c;
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_ಆರಂಭ, ಲಂಬಸಾಲು_ಆರಂಭ, ಅಡ್ಡಸಾಲು_ಕೊನೆ, ಲಂಬಸಾಲು_ಕೊನೆ;

    ಆದರೆ (popup_ಮುಖವಾಡ == ಖಾಲಿ || !popup_visible)
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು_ಆರಂಭ = MAX(wp->w_winrow, 0);
    ಲಂಬಸಾಲು_ಆರಂಭ = MAX(wp->w_wincol, 0);
    ಅಡ್ಡಸಾಲು_ಕೊನೆ = MIN(wp->w_winrow + popup_ಎತ್ತರ(wp), (ಪೂರ್ಣಾಂಕ)screen_ಅಡ್ಡಸಾಲುಗಳು);
    ಲಂಬಸಾಲು_ಕೊನೆ = MIN(wp->w_wincol + popup_ಅಗಲ(wp), (ಪೂರ್ಣಾಂಕ)screen_Columns);

    ಗಾಗಿ (r = ಅಡ್ಡಸಾಲು_ಆರಂಭ; r < ಅಡ್ಡಸಾಲು_ಕೊನೆ; ++r)
	ಗಾಗಿ (c = ಲಂಬಸಾಲು_ಆರಂಭ; c < ಲಂಬಸಾಲು_ಕೊನೆ; ++c)
	    popup_ಮುಖವಾಡ[r * screen_Columns + c] = 0;
}

    ಶೂನ್ಯ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    ಪೂರ್ಣಾಂಕ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (error_if_ಪದ_popup_ಕಿಟಕಿ())
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	ಹಿಂತಿರುಗಿ;

    popup_save_area(wp, &old_area);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	image_placement_clear(wp->w_popup_imagep);
#ಆದರೆ_ಅಂತ್ಯ

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    ಆದರೆ (wp->w_winrow + popup_ಎತ್ತರ(wp) >= cmdline_ಅಡ್ಡಸಾಲು)
	clear_cmdline = TRUE;

    ಆದರೆ (was_visible)
	popup_clear_ಮುಖವಾಡ_for(wp);

    ಆದರೆ (old_area.ಸಕ್ರಿಯ)
	popup_redraw_exposed_area(&old_area);
    ಇಲ್ಲದಿದ್ದರೆ
	redraw_all_later(UPD_NOT_VALID);

    ಸ್ಥಿತಿ_redraw_all();
    ಆದರೆ (!old_area.ಸಕ್ರಿಯ)
	popup_ಮುಖವಾಡ_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    ಶೂನ್ಯ
f_popup_hide(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    ಶೂನ್ಯ
popup_show(win_T *wp)
{
    bool popup_ಸಕ್ರಿಯ;

    ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	ಹಿಂತಿರುಗಿ;

    popup_ಸಕ್ರಿಯ = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    ಆದರೆ (popup_ಸಕ್ರಿಯ)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ಸಾಲುಗಳು_valid = 0;
	ಆದರೆ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_ಮುಖವಾಡ_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    ಶೂನ್ಯ
f_popup_show(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಆದರೆ (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#ಆದರೆ_ಅಂತ್ಯ

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}

/*
 * popup_settext({id}, {text})
 */
    ಶೂನ್ಯ
f_popup_settext(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    popup_save_area(wp, &old_area);

    ಆದರೆ (check_for_string_or_list_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    popup_ಗಣ_ಬಫರ್_ಪಠ್ಯ(wp->w_ಬಫರ್, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ಸಾಲುಗಳು_valid = 0;
    ಆದರೆ (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    ಆದರೆ (popup_area_ಬದಲಾಯಿತು(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    ಶೂನ್ಯ
f_popup_setbuf(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಬಫರ್_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf_from_arg(&argvars[1]);

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_ಎರಡನೇ_popup_with_terminal));
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp->w_ಬಫರ್ != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_ಬಫರ್->b_nwindows--;
	win_init_popup_win(wp, buf);
	ಗಣ_local_options_default(wp, FALSE);
	ಅದಲುಬದಲು_exists_action = SEA_READONLY;
	ಬಫರ್_ensure_loaded(buf);
	ಅದಲುಬದಲು_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	ಆದರೆ (popup_area_ಬದಲಾಯಿತು(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    ಪೂರ್ಣಾಂಕ			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ)
	image_placement_free(wp->w_popup_imagep);
#ಆದರೆ_ಅಂತ್ಯ

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_ಬಫರ್->b_locked = FALSE;
    ಆದರೆ (wp->w_winrow + popup_ಎತ್ತರ(wp) >= cmdline_ಅಡ್ಡಸಾಲು)
	clear_cmdline = TRUE;

    ಆದರೆ (was_visible)
	popup_clear_ಮುಖವಾಡ_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    ಆದರೆ (wp == ಸಂದೇಶ_win_2)
	ಸಂದೇಶ_win_2 = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!old_area.ಸಕ್ರಿಯ)
	redraw_all_later(UPD_NOT_VALID);
    ಸ್ಥಿತಿ_redraw_all();
    ಆದರೆ (!old_area.ಸಕ್ರಿಯ)
	popup_ಮುಖವಾಡ_refresh = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
error_for_popup_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    emsg(_(e_not_allowed_ಒಳ_popup_ಕಿಟಕಿ));
}

    ಪೂರ್ಣಾಂಕ
error_if_popup_ಕಿಟಕಿ(ಪೂರ್ಣಾಂಕ also_with_ಪದ ಬಳಸದ)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    ಆದರೆ (WIN_IS_POPUP(curwin)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    && (also_with_ಪದ || curbuf->b_ಪದ == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	error_for_popup_ಕಿಟಕಿ();
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
popup_close(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ಖಾಲಿ;

    // go through global popups
    ಗಾಗಿ (wp = ಮೊದಲ_popupwin; wp != ಖಾಲಿ; prev = wp, wp = wp->w_ಮುಂದಿನ)
	ಆದರೆ (wp->w_id == id)
	{
	    ಆದರೆ (wp == curwin)
	    {
		ಆದರೆ (!force)
		{
		    error_for_popup_ಕಿಟಕಿ();
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಹಿಂಭಾಗ_ಗಮ್ಯ_prevwin(wp);
	    }
	    ಆದರೆ (prev == ಖಾಲಿ)
		ಮೊದಲ_popupwin = wp->w_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		prev->w_ಮುಂದಿನ = wp->w_ಮುಂದಿನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#ಆದರೆ_ಅಂತ್ಯ
	    popup_free(wp);
	    ಹಿಂತಿರುಗಿ OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	ಆದರೆ (popup_close_tabpage(tp, id, force) == OK)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    ಪೂರ್ಣಾಂಕ
popup_close_tabpage(tabpage_T *tp, ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ force)
{
    win_T	*wp;
    win_T	**ಬೇರು = &tp->tp_ಮೊದಲ_popupwin;
    win_T	*prev = ಖಾಲಿ;

    ಗಾಗಿ (wp = *ಬೇರು; wp != ಖಾಲಿ; prev = wp, wp = wp->w_ಮುಂದಿನ)
	ಆದರೆ (wp->w_id == id)
	{
	    ಆದರೆ (wp == curwin)
	    {
		ಆದರೆ (!force)
		{
		    error_for_popup_ಕಿಟಕಿ();
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಹಿಂಭಾಗ_ಗಮ್ಯ_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    ಆದರೆ (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    ಆದರೆ (prev == ಖಾಲಿ)
		*ಬೇರು = wp->w_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		prev->w_ಮುಂದಿನ = wp->w_ಮುಂದಿನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#ಆದರೆ_ಅಂತ್ಯ
	    popup_free(wp);
	    ಹಿಂತಿರುಗಿ OK;
	}
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
close_all_popups(ಪೂರ್ಣಾಂಕ force)
{
    ಆದರೆ (!force && ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    ಆಗ (ಮೊದಲ_popupwin != ಖಾಲಿ)
	ಆದರೆ (popup_close(ಮೊದಲ_popupwin->w_id, force) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    ಆಗ (curtab->tp_ಮೊದಲ_popupwin != ಖಾಲಿ)
	ಆದರೆ (popup_close(curtab->tp_ಮೊದಲ_popupwin->w_id, force) == FAIL)
	    ಹಿಂತಿರುಗಿ;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->ಸಕ್ರಿಯ = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->ಎತ್ತರ = popup_ಎತ್ತರ(wp);
    area->ಅಗಲ = popup_ಅಗಲ(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    ಪೂರ್ಣಾಂಕ i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    ಗಾಗಿ (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ಸ್ಥಿರ bool
popup_style_ಬದಲಾಯಿತು(win_T *wp, popup_style_snapshot_T *style)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	ಹಿಂತಿರುಗಿ true;
    ಗಾಗಿ (i = 0; i < 4; i++)
	ಆದರೆ (style->border_highlight[i] != wp->w_border_highlight[i])
	    ಹಿಂತಿರುಗಿ true;
    ಹಿಂತಿರುಗಿ false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->ಅಗಲ = wp->w_ಅಗಲ;
    layout->ಎತ್ತರ = wp->w_ಎತ್ತರ;
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
popup_layout_ಬದಲಾಯಿತು(win_T *wp, popup_layout_T *layout)
{
    ಹಿಂತಿರುಗಿ layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->ಅಗಲ != wp->w_ಅಗಲ
	|| layout->ಎತ್ತರ != wp->w_ಎತ್ತರ
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
popup_area_ಬದಲಾಯಿತು(win_T *wp, popup_area_T *area)
{
    ಹಿಂತಿರುಗಿ area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->ಎತ್ತರ != popup_ಎತ್ತರ(wp)
	|| area->ಅಗಲ != popup_ಅಗಲ(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mark_overlapped_opacity_popup(win_T *wp, ಪೂರ್ಣಾಂಕ area_ತುದಿ, ಪೂರ್ಣಾಂಕ area_bot,
	ಪೂರ್ಣಾಂಕ area_ಎಡ, ಪೂರ್ಣಾಂಕ area_ಬಲ, ಪೂರ್ಣಾಂಕ zindex)
{
    ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_ಎತ್ತರ(wp) <= area_ತುದಿ
	    || wp->w_wincol >= area_ಬಲ
	    || wp->w_wincol + popup_ಅಗಲ(wp) - wp->w_popup_leftoff
							      <= area_ಎಡ)
	ಹಿಂತಿರುಗಿ;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_ಸಾಲುಗಳು_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_overlapped_opacity_popups(ಪೂರ್ಣಾಂಕ winrow, ಪೂರ್ಣಾಂಕ wincol, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ,
	ಪೂರ್ಣಾಂಕ leftoff, ಪೂರ್ಣಾಂಕ zindex)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		area_ತುದಿ = winrow;
    ಪೂರ್ಣಾಂಕ		area_bot = winrow + ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		area_ಎಡ = wincol;
    ಪೂರ್ಣಾಂಕ		area_ಬಲ = wincol + ಅಗಲ - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_ತುದಿ, area_bot, area_ಎಡ,
		area_ಬಲ, zindex);
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_ತುದಿ, area_bot, area_ಎಡ,
		area_ಬಲ, zindex);

    ಆದರೆ (must_redraw < UPD_VALID)
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
refill_pum_bg_ಅಡ್ಡಸಾಲು_from_ಬಫರ್(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ ಎಡ_2, ಪೂರ್ಣಾಂಕ ಬಲ_2)
{
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_cp = r;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_cp = ಎಡ_2;
    win_T	*wp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು_for_lnum;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_for_lnum = 0;
    char_u	*ಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		win_ಪಠ್ಯ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		screen_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		soff_ತಳ = (r - pum_bg_ತುದಿ) * pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    ಗಾಗಿ (c = ಎಡ_2; c < ಬಲ_2; ++c)
    {
	pum_bg_ಸಾಲುಗಳು[soff_ತಳ + c] = ' ';
	ಆದರೆ (pum_bg_attrs != ಖಾಲಿ)
	    pum_bg_attrs[soff_ತಳ + c] = 0;
	ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
	    pum_bg_ಸಾಲುಗಳು_UC[soff_ತಳ + c] = 0;
	ಆದರೆ (enc_utf8)
	{
	    ಪೂರ್ಣಾಂಕ k;
	    ಗಾಗಿ (k = 0; k < MAX_MCO; ++k)
		ಆದರೆ (pum_bg_ಸಾಲುಗಳು_C[k] != ಖಾಲಿ)
		    pum_bg_ಸಾಲುಗಳು_C[k][soff_ತಳ + c] = 0;
	}
    }

    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, IGNORE_POPUP);
    ಆದರೆ (wp == ಖಾಲಿ || ಸಾಲು_cp < 0 || ಸಾಲು_cp >= wp->w_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (wp->w_ಬಫರ್ == ಖಾಲಿ || wp->w_ಬಫರ್->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Compute the buffer line for this screen row.
    ಅಡ್ಡಸಾಲು_for_lnum = ಸಾಲು_cp;
    ಆದರೆ (mouse_comp_pos(wp, &ಅಡ್ಡಸಾಲು_for_lnum, &ಲಂಬಸಾಲು_for_lnum, &lnum, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;	// past end of buffer
    ಆದರೆ (lnum < 1 || lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_ಪಠ್ಯ_ಲಂಬಸಾಲು = wp->w_wincol + win_ಲಂಬಸಾಲು_off(wp);
    ಆದರೆ (!wp->w_p_wrap)
	win_ಪಠ್ಯ_ಲಂಬಸಾಲು -= wp->w_leftcol;
    screen_ಲಂಬಸಾಲು = win_ಪಠ್ಯ_ಲಂಬಸಾಲು;
    p = ಸಾಲು;
    ಆಗ (*p != NUL && screen_ಲಂಬಸಾಲು < ಬಲ_2)
    {
	ಪೂರ್ಣಾಂಕ char_cells;
	ಪೂರ್ಣಾಂಕ byte_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ soff = soff_ತಳ + screen_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಒಳ_range = (screen_ಲಂಬಸಾಲು >= ಎಡ_2 && screen_ಲಂಬಸಾಲು < ಬಲ_2);

	ಆದರೆ (*p == '\t')
	{
	    ಪೂರ್ಣಾಂಕ ts = (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_ಲಂಬಸಾಲು - win_ಪಠ್ಯ_ಲಂಬಸಾಲು) % ts) : 1;
	    byte_ಎಣಿಕೆ = 1;
	    ಆದರೆ (ಒಳ_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = ' ';
		ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_ಎಣಿಕೆ = mb_ptr2len(p);
	    ಆದರೆ (ಒಳ_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = *p;
		ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_cells = 1;
	    byte_ಎಣಿಕೆ = 1;
	    ಆದರೆ (ಒಳ_range)
	    {
		pum_bg_ಸಾಲುಗಳು[soff] = (*p < 0x20) ? ' ' : *p;
		ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
		    pum_bg_ಸಾಲುಗಳು_UC[soff] = 0;
	    }
	}
	ಆದರೆ (ಒಳ_range && pum_bg_attrs != ಖಾಲಿ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_ಎಣಿಕೆ;
	screen_ಲಂಬಸಾಲು += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_redraw_exposed_area(popup_area_T *area)
{
    ಆದರೆ (!area->ಸಕ್ರಿಯ)
	ಹಿಂತಿರುಗಿ;

    redraw_under_popup_area(area->winrow, area->wincol, area->ಎತ್ತರ,
	    area->ಅಗಲ, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->ಎತ್ತರ, area->ಅಗಲ, area->leftoff, area->zindex);

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
    ಆದರೆ (pum_bg_ಸಾಲುಗಳು != ಖಾಲಿ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->ಎತ್ತರ > pum_bg_ತುದಿ)
    {
	ಪೂರ್ಣಾಂಕ ತುದಿ = MAX(area->winrow, pum_bg_ತುದಿ);
	ಪೂರ್ಣಾಂಕ bot = MIN(area->winrow + area->ಎತ್ತರ, pum_bg_bot);
	ಪೂರ್ಣಾಂಕ ಎಡ_2 = MAX(area->wincol, 0);
	ಪೂರ್ಣಾಂಕ ಬಲ_2 = MIN(area->wincol + area->ಅಗಲ, pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	ಪೂರ್ಣಾಂಕ r;

	ಗಾಗಿ (r = ತುದಿ; r < bot; ++r)
	    refill_pum_bg_ಅಡ್ಡಸಾಲು_from_ಬಫರ್(r, ಎಡ_2, ಬಲ_2);
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
    CLEAR_ಸೂಚಕ(saved_screen);
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
    ಆದರೆ (screen_opacity_popup == ಖಾಲಿ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	ಹಿಂತಿರುಗಿ;

    saved_screen->ಆರಂಭ_ಅಡ್ಡಸಾಲು = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->ಆರಂಭ_ಲಂಬಸಾಲು = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2 = wp->w_popup_padding[0] + wp->w_ಎತ್ತರ
						    + wp->w_popup_padding[2];
    saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = wp->w_popup_padding[3] + wp->w_ಅಗಲ
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    ಆದರೆ (saved_screen->ಆರಂಭ_ಲಂಬಸಾಲು > 0)
    {
	--saved_screen->ಆರಂಭ_ಲಂಬಸಾಲು;
	++saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    }

    saved_screen->ಸಾಲುಗಳು = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2 * saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    saved_screen->attrs = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ,
				  saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2 * saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಆದರೆ (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2 * saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    ಆದರೆ (saved_screen->ಸಾಲುಗಳು == ಖಾಲಿ || saved_screen->attrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2; r++)
    {
	ಪೂರ್ಣಾಂಕ screen_ಅಡ್ಡಸಾಲು = saved_screen->ಆರಂಭ_ಅಡ್ಡಸಾಲು + r;

	ಆದರೆ (screen_ಅಡ್ಡಸಾಲು >= 0 && screen_ಅಡ್ಡಸಾಲು < screen_ಅಡ್ಡಸಾಲುಗಳು)
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = 0; c < saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; c++)
	    {
		ಪೂರ್ಣಾಂಕ screen_ಲಂಬಸಾಲು = saved_screen->ಆರಂಭ_ಲಂಬಸಾಲು + c;

		ಆದರೆ (screen_ಲಂಬಸಾಲು >= 0 && screen_ಲಂಬಸಾಲು < screen_Columns)
		{
		    ಪೂರ್ಣಾಂಕ off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[screen_ಅಡ್ಡಸಾಲು] + screen_ಲಂಬಸಾಲು;
		    ಪೂರ್ಣಾಂಕ save_off = r * saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + c;

		    saved_screen->ಸಾಲುಗಳು[save_off] = Screen_ಸಾಲುಗಳು[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    ಆದರೆ (enc_utf8 && saved_screen->linesuc != ಖಾಲಿ)
			saved_screen->linesuc[save_off] = Screen_ಸಾಲುಗಳು_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_under_popup_area(ಪೂರ್ಣಾಂಕ winrow, ಪೂರ್ಣಾಂಕ wincol, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ leftoff)
{
    ಪೂರ್ಣಾಂಕ	    r;

    ಗಾಗಿ (r = winrow; r < winrow + ಎತ್ತರ && r < screen_ಅಡ್ಡಸಾಲುಗಳು; ++r)
    {
	ಪೂರ್ಣಾಂಕ	    c;
	win_T	    *prev_twp = ಖಾಲಿ;

	ಆದರೆ (r >= cmdline_ಅಡ್ಡಸಾಲು)
	{
	    clear_cmdline = TRUE;
	    ಮುಂದುವರಿ;
	}

	ಗಾಗಿ (c = wincol; c < wincol + ಅಗಲ - leftoff && c < screen_Columns; ++c)
	{
	    ಪೂರ್ಣಾಂಕ	    ಸಾಲು_cp = r;
	    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_cp = c;
	    win_T   *twp;

	    twp = mouse_ಕಂಡುಹಿಡಿ_win(&ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, IGNORE_POPUP);
	    ಆದರೆ (twp != ಖಾಲಿ && twp != prev_twp)
	    {
		prev_twp = twp;
		ಆದರೆ (ಸಾಲು_cp < twp->w_ಎತ್ತರ)
		{
		    linenr_T lnum;

		    (ಶೂನ್ಯ)mouse_comp_pos(twp, &ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, &lnum, ಖಾಲಿ);
		    redrawWinline(twp, lnum);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_cp == twp->w_ಎತ್ತರ)
		    twp->w_redr_ಸ್ಥಿತಿ = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    ಶೂನ್ಯ
f_popup_move(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    popup_area_T	old_area;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // invalid {id}

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    ಆದರೆ (wp->w_winrow + wp->w_ಎತ್ತರ >= cmdline_ಅಡ್ಡಸಾಲು)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    ಆದರೆ (popup_area_ಬದಲಾಯಿತು(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    ಶೂನ್ಯ
f_popup_setoptions(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    ಪೂರ್ಣಾಂಕ		need_redraw = FALSE;
    ಪೂರ್ಣಾಂಕ		need_reposition = FALSE;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // invalid {id}

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (ಶೂನ್ಯ)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    ಆದರೆ (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    ಆದರೆ (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_style_ಬದಲಾಯಿತು(wp, &old_style))
	need_redraw = TRUE;

    ಆದರೆ (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    ಆದರೆ (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_ಮುಖವಾಡ_refresh = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_ಸಾಲುಗಳು_valid = 0;
	ಆದರೆ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    ಆದರೆ (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_ಮುಖವಾಡ_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    ಆದರೆ (popup_area_ಬದಲಾಯಿತು(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    ಶೂನ್ಯ
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ತುದಿ_extra;
    ಪೂರ್ಣಾಂಕ		ಎಡ_extra;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // invalid {id}
    ತುದಿ_extra = popup_ತುದಿ_extra(wp);
    ಎಡ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_ಬೀಗ_ಗಾತ್ರ(&dict->dv_hashtab, 11);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "line", wp->w_winrow + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", wp->w_wincol + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "width", wp->w_ಅಗಲ + ಎಡ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "height", wp->w_ಎತ್ತರ + ತುದಿ_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "core_line", wp->w_winrow + 1 + ತುದಿ_extra);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "core_col", wp->w_wincol + 1 + ಎಡ_extra);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "core_width", wp->w_ಅಗಲ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "core_height", wp->w_ಎತ್ತರ);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "scrollbar", wp->w_has_scrollbar);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "firstline", wp->w_topline);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lastline", wp->w_botline - 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    ಶೂನ್ಯ
f_popup_list(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    FOR_ALL_POPUPWINS(wp)
	list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    ಶೂನ್ಯ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;
    ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&argvars[1]) - 1;
    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
    ಆದರೆ (wp != ಖಾಲಿ && WIN_IS_POPUP(wp))
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_padding_border(dict_T *dict, ಪೂರ್ಣಾಂಕ *array, ಅಕ್ಷರ *name)
{
    list_T  *list;
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	ಹಿಂತಿರುಗಿ;

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	ಗಾಗಿ (i = 0; i < 4; ++i)
	    list_append_ಸಂಖ್ಯೆ(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < 4; ++i)
	ಆದರೆ (wp->w_border_highlight[i] != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    ಆದರೆ (i == 4 && !wp->w_border_highlight_isset)
	ಹಿಂತಿರುಗಿ;

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	ಹಿಂತಿರುಗಿ;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    ಆದರೆ (i == 4)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ಪೂರ್ಣಾಂಕ	    i;
    char_u  buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ	    len;

    ಗಾಗಿ (i = 0; i < 8; ++i)
	ಆದರೆ (wp->w_border_char[i] != 0)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i == 8)
	ಹಿಂತಿರುಗಿ;

    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (i = 0; i < 8; ++i)
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
    ಆದರೆ (list != ಖಾಲಿ)
    {
	ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_lnum);
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mincol);
	    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	ಹಿಂತಿರುಗಿ;
    }
    list_append_ಸಂಖ್ಯೆ(list, wp->w_popup_mouse_ಅಡ್ಡಸಾಲು);
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
    ಪೂರ್ಣಾಂಕ		id;
    win_T	*wp;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = ಕಂಡುಹಿಡಿ_popup_win(id);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dict = rettv->vval.v_dict;
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "line", wp->w_wantline);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", wp->w_wantcol);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "minwidth", wp->w_minwidth);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "minheight", wp->w_minheight);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "maxheight", wp->w_maxheight);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "maxwidth", wp->w_maxwidth);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "firstline", wp->w_firstline);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "scrollbar", wp->w_want_scrollbar);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "zindex", wp->w_zindex);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "fixed", wp->w_popup_fixed);
    ಆದರೆ (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = ಪಠ್ಯ_prop_type_by_id(
		wp->w_popup_prop_win->w_ಬಫರ್,
		wp->w_popup_prop_type);

	ಆದರೆ (pt != ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_string(dict, "textprop", pt->pt_name);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "textpropid", wp->w_popup_prop_id);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    ಆದರೆ (wp->w_popup_imagep != ಖಾಲಿ && wp->w_popup_imagep->img != ಖಾಲಿ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	ಆದರೆ (dict_ಕೂಡಿಸು_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#ಆದರೆ_ಅಂತ್ಯ

    dict_ಕೂಡಿಸು_string(dict, "title", wp->w_popup_title);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "wrap", wp->w_p_wrap);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_ಕೂಡಿಸು_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_ಕೂಡಿಸು_string(dict, "highlights", wp->w_p_whl);
    ಆದರೆ (wp->w_scrollbar_highlight != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    ಆದರೆ (wp->w_thumb_highlight != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, twp)
	    ಆದರೆ (twp->w_id == id)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (twp != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	++i;
    }
    ಆದರೆ (tp == ಖಾಲಿ)
	i = -1;  // must be global
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp == curtab)
	i = 0;
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    ಆದರೆ (wp->w_filter_cb.cb_name != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_callback(dict, "filter", &wp->w_filter_cb);
    ಆದರೆ (wp->w_close_cb.cb_name != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_callback(dict, "callback", &wp->w_close_cb);

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(poppos_entries); ++i)
	ಆದರೆ (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_ಕೂಡಿಸು_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(ಪೂರ್ಣಾಂಕ)poppos_entries[i].pp_name.ಉದ್ದಳತೆ);
	    ನಿಲ್ಲಿಸು;
	}

    ಆದರೆ (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_ಕೂಡಿಸು_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_close == POPCLOSE_CLICK)
	dict_ಕೂಡಿಸು_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#ಆದರೆ defined(FEAT_TIMERS)
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "time", wp->w_popup_timer != ಖಾಲಿ
	    ?  (ದೀರ್ಘ)wp->w_popup_timer->tr_interval : 0L);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    ಪೂರ್ಣಾಂಕ
error_if_ಪದ_popup_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (WIN_IS_POPUP(curwin) && curbuf->b_ಪದ != ಖಾಲಿ
					   && ಪದ_ಕೆಲಸ_running(curbuf->b_ಪದ))
    {
	emsg(_(e_not_allowed_for_terminal_ಒಳ_popup_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    ಶೂನ್ಯ
popup_reset_handled(ಪೂರ್ಣಾಂಕ handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(ಪೂರ್ಣಾಂಕ ಅತಿಕಡಿಮೆ, ಪೂರ್ಣಾಂಕ handled_flag)
{
    win_T   *wp;
    win_T   *ಸಿಕ್ಕಿತು_wp;
    ಪೂರ್ಣಾಂಕ	    ಸಿಕ್ಕಿತು_zindex;

    ಸಿಕ್ಕಿತು_zindex = ಅತಿಕಡಿಮೆ ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : 0;
    ಸಿಕ್ಕಿತು_wp = ಖಾಲಿ;
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (ಅತಿಕಡಿಮೆ ? wp->w_zindex < ಸಿಕ್ಕಿತು_zindex
		    : wp->w_zindex > ಸಿಕ್ಕಿತು_zindex))
	{
	    ಸಿಕ್ಕಿತು_zindex = wp->w_zindex;
	    ಸಿಕ್ಕಿತು_wp = wp;
	}
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (ಅತಿಕಡಿಮೆ ? wp->w_zindex < ಸಿಕ್ಕಿತು_zindex
		    : wp->w_zindex > ಸಿಕ್ಕಿತು_zindex))
	{
	    ಸಿಕ್ಕಿತು_zindex = wp->w_zindex;
	    ಸಿಕ್ಕಿತು_wp = wp;
	}

    ಆದರೆ (ಸಿಕ್ಕಿತು_wp != ಖಾಲಿ)
	ಸಿಕ್ಕಿತು_wp->w_popup_handled |= handled_flag;
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
invoke_popup_filter(win_T *wp, ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    ಪೂರ್ಣಾಂಕ		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    ಆದರೆ (c == Ctrl_C)
    {
	ಪೂರ್ಣಾಂಕ save_got_int = got_int;
	ಪೂರ್ಣಾಂಕ was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	ಆದರೆ (was_curwin && wp == curwin)
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_ಗಮ್ಯ_buf(c, mod_ಮುಖವಾಡ, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    ಆದರೆ (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	ಆದರೆ (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    ಆದರೆ (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    ಪೂರ್ಣಾಂಕ
popup_do_filter(ಪೂರ್ಣಾಂಕ c)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;
    ಪೂರ್ಣಾಂಕ		res = FALSE;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಪೂರ್ಣಾಂಕ		state;
    ಪೂರ್ಣಾಂಕ		was_must_redraw = must_redraw;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    ಆದರೆ (popup_is_popup(curwin) && curbuf->b_ಪದ != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE
	    || State == ಬಹುಲಕ_CONFIRM || State == ಬಹುಲಕ_EXTERNCMD)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ FALSE;
    recursive = TRUE;

    ಆದರೆ (c == K_LEFTMOUSE)
    {
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

	wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
	ಆದರೆ (wp != ಖಾಲಿ && popup_close_if_on_X(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    ಆಗ (res == FALSE
		     && (wp = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(FALSE, POPUP_HANDLED_2)) != ಖಾಲಿ)
	ಆದರೆ (wp->w_filter_cb.cb_name != ಖಾಲಿ
		&& (wp->w_filter_ಬಹುಲಕ & state) != 0)
	    res = invoke_popup_filter(wp, c);

    ಆದರೆ (must_redraw > was_must_redraw)
    {
	ಪೂರ್ಣಾಂಕ save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;

    // When interrupted return FALSE to avoid looping.
    ಹಿಂತಿರುಗಿ res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    ಪೂರ್ಣಾಂಕ
popup_no_mapping(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    ಸುತ್ತು;
    win_T   *wp;

    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	ಗಾಗಿ (wp = ಸುತ್ತು == 1 ? ಮೊದಲ_popupwin : curtab->tp_ಮೊದಲ_popupwin;
						   wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    ಆದರೆ (wp->w_filter_cb.cb_name != ಖಾಲಿ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಆಗ ((wp = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(TRUE, POPUP_HANDLED_3)) != ಖಾಲಿ)
	ಆದರೆ (wp->w_popup_curwin != ಖಾಲಿ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.ಲಂಬಸಾಲು < wp->w_popup_mincol
		    || curwin->w_cursor.ಲಂಬಸಾಲು > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_update_ಮುಖವಾಡ(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;

    ಆದರೆ (wp->w_popup_ಮುಖವಾಡ == ಖಾಲಿ || ಅಗಲ == 0 || ಎತ್ತರ == 0)
    {
	VIM_CLEAR(wp->w_popup_ಮುಖವಾಡ_cells);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (wp->w_popup_ಮುಖವಾಡ_cells != ಖಾಲಿ
	    && wp->w_popup_ಮುಖವಾಡ_ಎತ್ತರ == ಎತ್ತರ
	    && wp->w_popup_ಮುಖವಾಡ_ಅಗಲ == ಅಗಲ)
	ಹಿಂತಿರುಗಿ;  // cache is still valid

    vim_free(wp->w_popup_ಮುಖವಾಡ_cells);
    ಆದರೆ (ಅಗಲ > 0 && (size_t)ಎತ್ತರ > ಗಾತ್ರ_MAX / (size_t)ಅಗಲ)
    {
	wp->w_popup_ಮುಖವಾಡ_cells = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
    wp->w_popup_ಮುಖವಾಡ_cells = alloc_clear((size_t)ಅಗಲ * ಎತ್ತರ);
    ಆದರೆ (wp->w_popup_ಮುಖವಾಡ_cells == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    cells = wp->w_popup_ಮುಖವಾಡ_cells;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(wp->w_popup_ಮುಖವಾಡ, lio)
    {
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, cole;
	ಪೂರ್ಣಾಂಕ ಸಾಲುಗಳು, linee;

	li = lio->li_tv.vval.v_list->lv_ಮೊದಲ;
	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0)
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಅಗಲ + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + 1;
	ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ <= 0)
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 1;
	li = li->li_ಮುಂದಿನ;
	cole = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (cole < 0)
	    cole = ಅಗಲ + cole + 1;
	ಆದರೆ (cole > ಅಗಲ)
	    cole = ಅಗಲ;
	li = li->li_ಮುಂದಿನ;
	ಸಾಲುಗಳು = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = ಎತ್ತರ + ಸಾಲುಗಳು + 1;
	ಆದರೆ (ಸಾಲುಗಳು <= 0)
	    ಸಾಲುಗಳು = 1;
	li = li->li_ಮುಂದಿನ;
	linee = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (linee < 0)
	    linee = ಎತ್ತರ + linee + 1;
	ಆದರೆ (linee > ಎತ್ತರ)
	    linee = ಎತ್ತರ;

	ಗಾಗಿ (ಅಡ್ಡಸಾಲು = ಸಾಲುಗಳು - 1; ಅಡ್ಡಸಾಲು < linee; ++ಅಡ್ಡಸಾಲು)
	    ಗಾಗಿ (ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1; ಲಂಬಸಾಲು < cole; ++ಲಂಬಸಾಲು)
		cells[ಅಡ್ಡಸಾಲು * ಅಗಲ + ಲಂಬಸಾಲು] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_masked(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ screencol, ಪೂರ್ಣಾಂಕ screenline)
{
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = screencol - wp->w_wincol + wp->w_popup_leftoff;
    ಪೂರ್ಣಾಂಕ ಸಾಲು = screenline - wp->w_winrow;

    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು < ಅಗಲ
	    && ಸಾಲು >= 0 && ಸಾಲು < ಎತ್ತರ
	    && wp->w_popup_ಮುಖವಾಡ_cells[ಸಾಲು * ಅಗಲ + ಲಂಬಸಾಲು];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_popup_transparent(win_T *wp, ಪೂರ್ಣಾಂಕ val)
{
    ಆದರೆ (wp->w_popup_ಮುಖವಾಡ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ		ಅಗಲ = popup_ಅಗಲ(wp);
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ = popup_ಎತ್ತರ(wp);
    listitem_T	*lio, *li;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, cole;
    ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು, linee;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು, ಸಾಲು;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(wp->w_popup_ಮುಖವಾಡ, lio)
    {
	li = lio->li_tv.vval.v_list->lv_ಮೊದಲ;
	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0)
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಅಗಲ + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + 1;
	li = li->li_ಮುಂದಿನ;
	cole = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (cole < 0)
	    cole = ಅಗಲ + cole + 1;
	li = li->li_ಮುಂದಿನ;
	ಸಾಲುಗಳು = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = ಎತ್ತರ + ಸಾಲುಗಳು + 1;
	li = li->li_ಮುಂದಿನ;
	linee = tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
	ಆದರೆ (linee < 0)
	    linee = ಎತ್ತರ + linee + 1;

	--ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ -= wp->w_popup_leftoff;
	ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0)
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
	cole -= wp->w_popup_leftoff;
	--ಸಾಲುಗಳು;
	ಆದರೆ (ಸಾಲುಗಳು < 0)
	    ಸಾಲುಗಳು = 0;
	ಗಾಗಿ (ಸಾಲು = ಸಾಲುಗಳು; ಸಾಲು < linee
		&& ಸಾಲು + wp->w_winrow < screen_ಅಡ್ಡಸಾಲುಗಳು; ++ಸಾಲು)
	    ಗಾಗಿ (ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು < cole
		    && ಲಂಬಸಾಲು + wp->w_wincol < screen_Columns; ++ಲಂಬಸಾಲು)
		popup_transparent[(ಸಾಲು + wp->w_winrow) * screen_Columns
		    + ಲಂಬಸಾಲು + wp->w_wincol] = val;
    }
}

/*
 * Only called when popup window "wp" is hidden: If the window is positioned
 * next to a text property, and it is now visible, then  unhide the popup.
 * We don't check if visible popups become hidden, that is done in
 * popup_adjust_position().
 * Return TRUE if the popup became unhidden.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_popup_unhidden(win_T *wp)
{
    ಆದರೆ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    ಸಿಕ್ಕಿತು = false;

	ಆದರೆ ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (ಕಂಡುಹಿಡಿ_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    ಸಿಕ್ಕಿತು = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_ಕಂಡುಹಿಡಿ_prop_above_ತುದಿ(wp, wp->w_popup_prop_win,
					    popup_ಎತ್ತರ(wp), &prop, &lnum))
	    ಸಿಕ್ಕಿತು = true;
	ಆದರೆ (ಸಿಕ್ಕಿತು)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_need_position_adjust(win_T *wp)
{
    ಆದರೆ (wp->w_popup_ಕೊನೆಯ_changedtick != CHANGEDTICK(wp->w_ಬಫರ್))
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_ಬಫರ್)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_ಅಗಲ != wp->w_popup_prop_win->w_ಅಗಲ
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_ಎತ್ತರ))
	ಹಿಂತಿರುಗಿ TRUE;

    // May need to adjust the width if the cursor moved.
    ಹಿಂತಿರುಗಿ wp->w_cursor.lnum != wp->w_popup_ಕೊನೆಯ_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ಸ್ಥಿರ ಗಿಡ್ಡ *opacity_zindex = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ    opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ    opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_mark_opacity_zindex(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ	    r, c;

    ಆದರೆ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ಹಿಂತಿರುಗಿ;

    ಅಗಲ = popup_ಅಗಲ(wp);
    ಎತ್ತರ = popup_ಎತ್ತರ(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    ಗಾಗಿ (r = wp->w_winrow;
		       r < wp->w_winrow + ಎತ್ತರ - wp->w_popup_topoff
						&& r < screen_ಅಡ್ಡಸಾಲುಗಳು; ++r)
	ಗಾಗಿ (c = wp->w_wincol;
		 c < wp->w_wincol + ಅಗಲ - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    ಪೂರ್ಣಾಂಕ off = r * screen_Columns + c;
	    ಆದರೆ (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_win_under_opacity_popup(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    r;

    ಆದರೆ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	ಹಿಂತಿರುಗಿ;

    ಎತ್ತರ = popup_ಎತ್ತರ(wp);
    ಅಗಲ = popup_ಅಗಲ(wp);
    ಗಾಗಿ (r = wp->w_winrow;
		       r < wp->w_winrow + ಎತ್ತರ && r < screen_ಅಡ್ಡಸಾಲುಗಳು; ++r)
    {
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು;
	win_T	    *prev_twp = ಖಾಲಿ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	ಗಾಗಿ (ಲಂಬಸಾಲು = wp->w_wincol;
		ಲಂಬಸಾಲು < wp->w_wincol + ಅಗಲ && ಲಂಬಸಾಲು < screen_Columns; ++ಲಂಬಸಾಲು)
	{
	    ಪೂರ್ಣಾಂಕ	    ಸಾಲು_cp = r;
	    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_cp = ಲಂಬಸಾಲು;
	    win_T   *twp;

	    twp = mouse_ಕಂಡುಹಿಡಿ_win(&ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, IGNORE_POPUP);
	    ಆದರೆ (twp != ಖಾಲಿ && twp != prev_twp)
	    {
		prev_twp = twp;
		ಆದರೆ (ಸಾಲು_cp < twp->w_ಎತ್ತರ)
		{
		    linenr_T lnum;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    ಆದರೆ (ಪದ_do_update_ಕಿಟಕಿ(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    {
			(ಶೂನ್ಯ)mouse_comp_pos(twp, &ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, &lnum,
									ಖಾಲಿ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_cp == twp->w_ಎತ್ತರ)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_ಸ್ಥಿತಿ = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    ಪೂರ್ಣಾಂಕ
popup_is_under_opacity(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (opacity_zindex == ಖಾಲಿ
	    || ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು
	    || ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ opacity_zindex[ಅಡ್ಡಸಾಲು * opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    ಪೂರ್ಣಾಂಕ
popup_is_over_opacity(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ಅಡ್ಡಸಾಲು >= wp->w_winrow
		&& ಅಡ್ಡಸಾಲು < wp->w_winrow + popup_ಎತ್ತರ(wp)
		&& ಲಂಬಸಾಲು >= wp->w_wincol
		&& ಲಂಬಸಾಲು < wp->w_wincol + popup_ಅಗಲ(wp))
	    ಹಿಂತಿರುಗಿ TRUE;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ಅಡ್ಡಸಾಲು >= wp->w_winrow
		&& ಅಡ್ಡಸಾಲು < wp->w_winrow + popup_ಎತ್ತರ(wp)
		&& ಲಂಬಸಾಲು >= wp->w_wincol
		&& ಲಂಬಸಾಲು < wp->w_wincol + popup_ಅಗಲ(wp))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    ಪೂರ್ಣಾಂಕ
popup_is_under_opacity_range(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

    ಆದರೆ (opacity_zindex == ಖಾಲಿ
	    || ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು < ಕೊನೆ_ಲಂಬಸಾಲು && ಲಂಬಸಾಲು < opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ++ಲಂಬಸಾಲು)
	ಆದರೆ (opacity_zindex[ಅಡ್ಡಸಾಲು * opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು] > screen_zindex)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    ಶೂನ್ಯ
may_update_popup_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ type)
{
    win_T	*wp;
    ಗಿಡ್ಡ	*ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು, ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		redraw_all_popups = FALSE;
    ಪೂರ್ಣಾಂಕ		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    ಆದರೆ (popup_ಮುಖವಾಡ_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_ಮುಖವಾಡ_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ಮುಖವಾಡ_refresh |= check_popup_unhidden(wp);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_need_position_adjust(wp))
	    popup_ಮುಖವಾಡ_refresh = TRUE;
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ಮುಖವಾಡ_refresh |= check_popup_unhidden(wp);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (popup_need_position_adjust(wp))
	    popup_ಮುಖವಾಡ_refresh = TRUE;

    // Force background windows to redraw rows under opacity popups.
    // Opacity popups don't participate in popup_mask, so their area
    // wouldn't normally be redrawn.  Without this, ScreenAttrs retains
    // blended values from the previous cycle, causing blend accumulation.
    // This must run every cycle, not just when popup_mask_refresh is set.
    //
    // Also build the opacity_zindex array used by screen_char() to suppress
    // output for cells under opacity popups during background draw.
    {
	ಪೂರ್ಣಾಂಕ has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	ಆದರೆ (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು = 0;
	    opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು != screen_ಅಡ್ಡಸಾಲುಗಳು
		    || opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(ಗಿಡ್ಡ,
					screen_ಅಡ್ಡಸಾಲುಗಳು * screen_Columns);
		opacity_zindex_ಅಡ್ಡಸಾಲುಗಳು = screen_ಅಡ್ಡಸಾಲುಗಳು;
		opacity_zindex_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen_Columns;
	    }
	    ಆದರೆ (opacity_zindex != ಖಾಲಿ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_ಅಡ್ಡಸಾಲುಗಳು * screen_Columns * ಗಾತ್ರ(ಗಿಡ್ಡ));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    ಆದರೆ (!popup_ಮುಖವಾಡ_refresh)
	ಹಿಂತಿರುಗಿ;

    // Need to update the mask, something has changed.
    popup_ಮುಖವಾಡ_refresh = FALSE;
    popup_ಮುಖವಾಡ_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    ಆದರೆ (redrawing_all_win)
	ಮುಖವಾಡ = popup_ಮುಖವಾಡ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಮುಖವಾಡ = popup_ಮುಖವಾಡ_ಮುಂದಿನ;
    vim_memset(ಮುಖವಾಡ, 0, (size_t)screen_ಅಡ್ಡಸಾಲುಗಳು * screen_Columns * ಗಾತ್ರ(ಗಿಡ್ಡ));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    ಆಗ ((wp = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(TRUE, POPUP_HANDLED_4)) != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	ಆದರೆ (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    ಆದರೆ (wp->w_popup_flags & POPF_HIDDEN)
		ಮುಂದುವರಿ;
	}

	ಅಗಲ = popup_ಅಗಲ(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	ಆದರೆ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    ಎತ್ತರ = cl.eff_ಎತ್ತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಎತ್ತರ = popup_ಎತ್ತರ(wp);
	popup_update_ಮುಖವಾಡ(wp, ಅಗಲ, ಎತ್ತರ);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    ಮುಂದುವರಿ;

	{
	    ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_ಆರಂಭ = wp->w_winrow;
	    ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_ಕೊನೆ = ಮುಖವಾಡ_ಆರಂಭ + ಎತ್ತರ;
	    ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_ಲಂಬಸಾಲು_ಆರಂಭ = wp->w_wincol + wp->w_popup_leftclip;
	    ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_ಲಂಬಸಾಲು_ಕೊನೆ = wp->w_wincol + ಅಗಲ - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    ಗಾಗಿ (ಸಾಲು = ಮುಖವಾಡ_ಆರಂಭ;
			    ಸಾಲು < ಮುಖವಾಡ_ಕೊನೆ && ಸಾಲು < screen_ಅಡ್ಡಸಾಲುಗಳು; ++ಸಾಲು)
		ಗಾಗಿ (ಲಂಬಸಾಲು = ಮುಖವಾಡ_ಲಂಬಸಾಲು_ಆರಂಭ;
		     ಲಂಬಸಾಲು < ಮುಖವಾಡ_ಲಂಬಸಾಲು_ಕೊನೆ && ಲಂಬಸಾಲು < screen_Columns; ++ಲಂಬಸಾಲು)
		    ಆದರೆ (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(ಸಾಲು, ಲಂಬಸಾಲು, FALSE))
			ಮುಖವಾಡ[ಸಾಲು * screen_Columns + ಲಂಬಸಾಲು] = POPUPMENU_ZINDEX;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_ಮುಖವಾಡ_cells == ಖಾಲಿ
				    || !popup_masked(wp, ಅಗಲ, ಎತ್ತರ, ಲಂಬಸಾಲು, ಸಾಲು))
			ಮುಖವಾಡ[ಸಾಲು * screen_Columns + ಲಂಬಸಾಲು] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    ಆದರೆ (ಮುಖವಾಡ == popup_ಮುಖವಾಡ_ಮುಂದಿನ)
    {
	ಪೂರ್ಣಾಂಕ	    *plines_cache = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ, ಅಡ್ಡಸಾಲುಗಳು);
	win_T	    *prev_wp = ಖಾಲಿ;

	ಗಾಗಿ (ಸಾಲು = 0; ಸಾಲು < screen_ಅಡ್ಡಸಾಲುಗಳು; ++ಸಾಲು)
	{
	    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_ಮುಗಿದ = 0;

	    ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < screen_Columns; ++ಲಂಬಸಾಲು)
	    {
		ಪೂರ್ಣಾಂಕ off = ಸಾಲು * screen_Columns + ಲಂಬಸಾಲು;

		ಆದರೆ (popup_ಮುಖವಾಡ[off] != popup_ಮುಖವಾಡ_ಮುಂದಿನ[off])
		{
		    popup_ಮುಖವಾಡ[off] = popup_ಮುಖವಾಡ_ಮುಂದಿನ[off];

		    ಆದರೆ (ಸಾಲು >= cmdline_ಅಡ್ಡಸಾಲು)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			ಆದರೆ (!ಸಂದೇಶ_scrolled && popup_ಮುಖವಾಡ_ಮುಂದಿನ[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು >= ಲಂಬಸಾಲು_ಮುಗಿದ)
		    {
			linenr_T	lnum;
			ಪೂರ್ಣಾಂಕ		ಸಾಲು_cp = ಸಾಲು;
			ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_cp = ಲಂಬಸಾಲು;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, IGNORE_POPUP);
			ಆದರೆ (wp != ಖಾಲಿ)
			{
#ಆದರೆ defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    ಆದರೆ (bt_terminal(wp->w_ಬಫರ್))
				redraw_win_later(wp, UPD_NOT_VALID);
			    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			    {
				ಆದರೆ (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಅಡ್ಡಸಾಲುಗಳು);
				    prev_wp = wp;
				}

				ಆದರೆ (ಸಾಲು_cp >= wp->w_ಎತ್ತರ)
				    // In (or below) status line
				    wp->w_redr_ಸ್ಥಿತಿ = true;
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (ಶೂನ್ಯ)mouse_comp_pos(wp, &ಸಾಲು_cp, &ಲಂಬಸಾಲು_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    ಲಂಬಸಾಲು_ಮುಗಿದ = wp->w_wincol + wp->w_ಅಗಲ - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_ಬಳಸುತ್ತದೆ_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    ಶೂನ್ಯ
may_update_popup_position(ಶೂನ್ಯ)
{
    ಆದರೆ (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
ಸ್ಥಿರ schar_T *ತಳ_screenlines = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *ತಳ_screenattrs = ಖಾಲಿ;
ಸ್ಥಿರ u8char_T *ತಳ_screenlinesuc = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    ಪೂರ್ಣಾಂಕ
popup_get_ತಳ_screen_cell(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, schar_T *linep, ಪೂರ್ಣಾಂಕ *attrp,
							  u8char_T *ucp)
{
    ಆದರೆ (ತಳ_screenlines == ಖಾಲಿ || ತಳ_screenattrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು
						     || ಲಂಬಸಾಲು >= ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ off = ಅಡ್ಡಸಾಲು * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು;
    ಆದರೆ (linep != ಖಾಲಿ)
	*linep = ತಳ_screenlines[off];
    ಆದರೆ (attrp != ಖಾಲಿ)
	*attrp = ತಳ_screenattrs[off];
    ಆದರೆ (ucp != ಖಾಲಿ)
    {
	ಆದರೆ (enc_utf8 && ತಳ_screenlinesuc != ಖಾಲಿ)
	    *ucp = ತಳ_screenlinesuc[off];
	ಇಲ್ಲದಿದ್ದರೆ
	    *ucp = 0;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    ಶೂನ್ಯ
popup_ಗಣ_ತಳ_screen_cell(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, schar_T ಸಾಲು, ಪೂರ್ಣಾಂಕ attr, u8char_T uc)
{
    ಆದರೆ (ತಳ_screenlines == ಖಾಲಿ || ತಳ_screenattrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು
						     || ಲಂಬಸಾಲು >= ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ off = ಅಡ್ಡಸಾಲು * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು;
    ತಳ_screenlines[off] = ಸಾಲು;
    ತಳ_screenattrs[off] = attr;
    ಆದರೆ (enc_utf8 && ತಳ_screenlinesuc != ಖಾಲಿ)
	ತಳ_screenlinesuc[off] = uc;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
draw_opacity_padding_cell(
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	popup_saved_screen_T *saved_screen,
	ಪೂರ್ಣಾಂಕ		pad_ಆರಂಭ_ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		pad_ಕೊನೆ_ಲಂಬಸಾಲು)
{
    ಆದರೆ (ಸಾಲು_ಸ್ಥಾನಾಂತರ == ಖಾಲಿ || Screen_ಸಾಲುಗಳು == ಖಾಲಿ || ScreenAttrs == ಖಾಲಿ
	    || (enc_utf8 && Screen_ಸಾಲುಗಳು_UC == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ r = ಅಡ್ಡಸಾಲು - saved_screen->ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ c = ಲಂಬಸಾಲು - saved_screen->ಆರಂಭ_ಲಂಬಸಾಲು;

    ಆದರೆ (r >= 0 && r < saved_screen->ಅಡ್ಡಸಾಲುಗಳು_2 && c >= 0 && c < saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    {
	ಪೂರ್ಣಾಂಕ save_off = r * saved_screen->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	ಆದರೆ (enc_utf8 && saved_screen->linesuc != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ ತಳ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1;
	    ಪೂರ್ಣಾಂಕ ತಳ_off = off - 1;
	    ಪೂರ್ಣಾಂಕ ತಳ_save_off = save_off - 1;
	    ಪೂರ್ಣಾಂಕ wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    ಆದರೆ (ತಳ_off >= 0)
	    {
		ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[ತಳ_off] != 0
			&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[ತಳ_off]) == 2
			&& Screen_ಸಾಲುಗಳು[off] == 0)
		    wide_prev = TRUE;
	    }
	    ಆದರೆ (!wide_prev && save_off > 0)
	    {
		ಆದರೆ (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->ಸಾಲುಗಳು[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    ಆದರೆ (wide_prev && ತಳ_ಲಂಬಸಾಲು >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		ಆದರೆ (ತಳ_ಲಂಬಸಾಲು < pad_ಆರಂಭ_ಲಂಬಸಾಲು)
		{
		    ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[ತಳ_off] != 0
			    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[ತಳ_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_ಸಾಲುಗಳು[ತಳ_off] = ' ';
			Screen_ಸಾಲುಗಳು_UC[ತಳ_off] = 0;
			ScreenAttrs[ತಳ_off] = saved_screen->attrs[ತಳ_save_off];
			popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು,
				Screen_ಸಾಲುಗಳು[ತಳ_off],
				ScreenAttrs[ತಳ_off],
				Screen_ಸಾಲುಗಳು_UC[ತಳ_off]);
			screen_char(ತಳ_off, ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_ಸಾಲುಗಳು[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[ತಳ_save_off];
			ಆದರೆ (enc_utf8)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ಪೂರ್ಣಾಂಕ popup_attr_val =
					get_win_attr(screen_opacity_popup);
			ಪೂರ್ಣಾಂಕ blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು,
				Screen_ಸಾಲುಗಳು[off], ScreenAttrs[off],
				Screen_ಸಾಲುಗಳು_UC[off]);
			screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
			ಹಿಂತಿರುಗಿ;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    ಆದರೆ (ತಳ_save_off >= 0
			    && saved_screen->linesuc[ತಳ_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[ತಳ_save_off]) == 2
			    && Screen_ಸಾಲುಗಳು[ತಳ_off] == ' '
			    && Screen_ಸಾಲುಗಳು_UC[ತಳ_off] == 0)
		    {
			ಪೂರ್ಣಾಂಕ popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			ಪೂರ್ಣಾಂಕ blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_ಸಾಲುಗಳು[ತಳ_off] =
				    saved_screen->ಸಾಲುಗಳು[ತಳ_save_off];
			Screen_ಸಾಲುಗಳು_UC[ತಳ_off] =
				    saved_screen->linesuc[ತಳ_save_off];
			ScreenAttrs[ತಳ_off] =
				    saved_screen->attrs[ತಳ_save_off];
			ScreenAttrs[ತಳ_off] = hl_blend_attr(
				    ScreenAttrs[ತಳ_off],
				    popup_attr_val, blend, TRUE);

			Screen_ಸಾಲುಗಳು[off] = 0;
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[ತಳ_off];

			popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು,
				    Screen_ಸಾಲುಗಳು[ತಳ_off],
				    ScreenAttrs[ತಳ_off],
				    Screen_ಸಾಲುಗಳು_UC[ತಳ_off]);
			popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು,
				    Screen_ಸಾಲುಗಳು[off],
				    ScreenAttrs[off],
				    Screen_ಸಾಲುಗಳು_UC[off]);
			screen_char(ತಳ_off, ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು);
			ಹಿಂತಿರುಗಿ;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_ಸಾಲುಗಳು[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[ತಳ_save_off];
		    ಆದರೆ (enc_utf8)
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
		    ಪೂರ್ಣಾಂಕ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಂಕ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, Screen_ಸಾಲುಗಳು[off],
					       ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
		    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
		    ಹಿಂತಿರುಗಿ;
		}

		// Base cell is inside the saved area, redraw the wide char.
		ಆದರೆ (save_off > 0)
		{
		    Screen_ಸಾಲುಗಳು[ತಳ_off] = saved_screen->ಸಾಲುಗಳು[ತಳ_save_off];
		    ScreenAttrs[ತಳ_off] = saved_screen->attrs[ತಳ_save_off];
		    Screen_ಸಾಲುಗಳು[off] = saved_screen->ಸಾಲುಗಳು[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_ಸಾಲುಗಳು_UC[ತಳ_off] = saved_screen->linesuc[ತಳ_save_off];
		    Screen_ಸಾಲುಗಳು_UC[off] = saved_screen->linesuc[save_off];

		    ಪೂರ್ಣಾಂಕ popup_attr_val = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಂಕ blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[ತಳ_off] = hl_blend_attr(ScreenAttrs[ತಳ_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[ತಳ_off];
		    popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು, Screen_ಸಾಲುಗಳು[ತಳ_off],
					       ScreenAttrs[ತಳ_off], Screen_ಸಾಲುಗಳು_UC[ತಳ_off]);
		    popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, Screen_ಸಾಲುಗಳು[off],
					       ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
		    screen_char(ತಳ_off, ಅಡ್ಡಸಾಲು, ತಳ_ಲಂಬಸಾಲು);
		}
		ಹಿಂತಿರುಗಿ;
	    }
	}
	Screen_ಸಾಲುಗಳು[off] = saved_screen->ಸಾಲುಗಳು[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	ಆದರೆ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off] = saved_screen->linesuc != ಖಾಲಿ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& ಲಂಬಸಾಲು + 1 >= pad_ಕೊನೆ_ಲಂಬಸಾಲು)
	{
	    Screen_ಸಾಲುಗಳು[off] = ' ';
	    Screen_ಸಾಲುಗಳು_UC[off] = 0;
	}

	ಪೂರ್ಣಾಂಕ popup_attr_val = get_win_attr(screen_opacity_popup);
	ಪೂರ್ಣಾಂಕ blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_ಗಣ_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, Screen_ಸಾಲುಗಳು[off],
				   ScreenAttrs[off], Screen_ಸಾಲುಗಳು_UC[off]);
	screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
fill_opacity_padding(
	ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಲಂಬಸಾಲು,
	popup_saved_screen_T *saved_screen)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pad_ಅಡ್ಡಸಾಲು = ಆರಂಭ_ಅಡ್ಡಸಾಲು; pad_ಅಡ್ಡಸಾಲು < ಕೊನೆ_ಅಡ್ಡಸಾಲು; pad_ಅಡ್ಡಸಾಲು++)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pad_ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು; pad_ಲಂಬಸಾಲು < ಕೊನೆ_ಲಂಬಸಾಲು; pad_ಲಂಬಸಾಲು++)
	    draw_opacity_padding_cell(pad_ಅಡ್ಡಸಾಲು, pad_ಲಂಬಸಾಲು, saved_screen,
		    ಆರಂಭ_ಲಂಬಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    ಶೂನ್ಯ
update_popups(ಶೂನ್ಯ (*win_update)(win_T *wp))
{
    win_T   *wp;
    ಪೂರ್ಣಾಂಕ	    ತುದಿ_off;
    ಪೂರ್ಣಾಂಕ	    ಎಡ_extra;
    ಪೂರ್ಣಾಂಕ	    ಒಟ್ಟು_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    ಒಟ್ಟು_ಎತ್ತರ_2;
    ಪೂರ್ಣಾಂಕ	    ತುದಿ_padding;
    ಪೂರ್ಣಾಂಕ	    popup_attr;
    ಪೂರ್ಣಾಂಕ	    border_attr[4];
    ಪೂರ್ಣಾಂಕ	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    wincol;
    ಪೂರ್ಣಾಂಕ	    padcol = 0;
    ಪೂರ್ಣಾಂಕ	    padendcol = 0;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    sb_thumb_ತುದಿ = 0;
    ಪೂರ್ಣಾಂಕ	    sb_thumb_ಎತ್ತರ = 0;
    ಪೂರ್ಣಾಂಕ	    attr_scroll = 0;
    ಪೂರ್ಣಾಂಕ	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (ತಳ_screenlines != ಖಾಲಿ)
    {
	vim_free(ತಳ_screenlines);
	ತಳ_screenlines = ಖಾಲಿ;
    }
    ಆದರೆ (ತಳ_screenattrs != ಖಾಲಿ)
    {
	vim_free(ತಳ_screenattrs);
	ತಳ_screenattrs = ಖಾಲಿ;
    }
    ಆದರೆ (ತಳ_screenlinesuc != ಖಾಲಿ)
    {
	vim_free(ತಳ_screenlinesuc);
	ತಳ_screenlinesuc = ಖಾಲಿ;
    }
    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು = 0;
    ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ ((wp = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(TRUE, POPUP_HANDLED_5)) != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	    title_len = 0;
	ಪೂರ್ಣಾಂಕ	    title_wincol;
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
	ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	ಇಲ್ಲದಿದ್ದರೆ
	    screen_opacity_popup = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (screen_opacity_popup != ಖಾಲಿ)
	{
	    ಆದರೆ (ತಳ_screenlines != ಖಾಲಿ)
	    {
		vim_free(ತಳ_screenlines);
		ತಳ_screenlines = ಖಾಲಿ;
	    }
	    ಆದರೆ (ತಳ_screenattrs != ಖಾಲಿ)
	    {
		vim_free(ತಳ_screenattrs);
		ತಳ_screenattrs = ಖಾಲಿ;
	    }
	    ಆದರೆ (ತಳ_screenlinesuc != ಖಾಲಿ)
	    {
		vim_free(ತಳ_screenlinesuc);
		ತಳ_screenlinesuc = ಖಾಲಿ;
	    }

	    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು = screen_ಅಡ್ಡಸಾಲುಗಳು;
	    ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen_Columns;
	    ತಳ_screenlines = ALLOC_MULT(schar_T,
				    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	    ತಳ_screenattrs = ALLOC_MULT(ಪೂರ್ಣಾಂಕ,
				    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	    ಆದರೆ (enc_utf8)
		ತಳ_screenlinesuc = ALLOC_MULT(u8char_T,
				    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

	    ಆದರೆ (ತಳ_screenlines != ಖಾಲಿ && ತಳ_screenattrs != ಖಾಲಿ)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು; r++)
		{
		    ಪೂರ್ಣಾಂಕ off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[r];
		    ಪೂರ್ಣಾಂಕ ತಳ_off = r * ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = 0; c < ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; c++)
		    {
			ತಳ_screenlines[ತಳ_off + c] = Screen_ಸಾಲುಗಳು[off + c];
			ತಳ_screenattrs[ತಳ_off + c] = ScreenAttrs[off + c];
			ಆದರೆ (enc_utf8 && ತಳ_screenlinesuc != ಖಾಲಿ)
			    ತಳ_screenlinesuc[ತಳ_off + c] =
					Screen_ಸಾಲುಗಳು_UC[off + c];
		    }
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

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
	ಆದರೆ (wp->w_wincol + ಎಡ_extra < 0)
	    ಎಡ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(ತುದಿ_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ಎಡ_extra;

	// Draw the popup text, unless it's off screen.
	ಆದರೆ (wp->w_winrow < screen_ಅಡ್ಡಸಾಲುಗಳು && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    ಆದರೆ (wp->w_popup_ಕೊನೆಯ_curline != wp->w_cursor.lnum)
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
	    ಆದರೆ (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(ತುದಿ_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ಎಡ_extra;

	// Add offset for border and padding if not done already.
	ಆದರೆ ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ಎಡ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	ಆದರೆ ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
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
	ಆದರೆ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    ಒಟ್ಟು_ಅಗಲ = cl.eff_ಅಗಲ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಒಟ್ಟು_ಅಗಲ = popup_ಅಗಲ(wp) - wp->w_popup_rightoff;
	ಆದರೆ (ಒಟ್ಟು_ಅಗಲ < 0)
	    ಒಟ್ಟು_ಅಗಲ = 0;
	ಆದರೆ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    ಒಟ್ಟು_ಎತ್ತರ_2 = cl.eff_ಎತ್ತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಒಟ್ಟು_ಎತ್ತರ_2 = popup_ಎತ್ತರ(wp);
	popup_attr = get_win_attr(wp);

	ಆದರೆ (wp->w_winrow + ಒಟ್ಟು_ಎತ್ತರ_2 > cmdline_ಅಡ್ಡಸಾಲು)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	ಆದರೆ (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    ಗಾಗಿ (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    ಆದರೆ (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	ಗಾಗಿ (i = 0; i < 8; ++i)
	    ಆದರೆ (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	ಗಾಗಿ (i = 0; i < 4; ++i)
	{
	    ಆದರೆ (wp->w_border_highlight[i] != ಖಾಲಿ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    ಇಲ್ಲದಿದ್ದರೆ
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	ಆದರೆ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	ಆದರೆ (wp->w_popup_title != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	    title_attr;

	    ಆದರೆ (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ಖಾಲಿ)
		title_attr = border_attr[0];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    ಇಲ್ಲದಿದ್ದರೆ
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    ಆದರೆ (title_len > ಒಟ್ಟು_ಅಗಲ - 2)
	    {
		ಪೂರ್ಣಾಂಕ	title_byte_len = (ಪೂರ್ಣಾಂಕ)STRLEN(wp->w_popup_title);
		char_u  *title_ಪಠ್ಯ = alloc(title_byte_len + 1);

		ಆದರೆ (title_ಪಠ್ಯ != ಖಾಲಿ)
		{
		    trunc_string(wp->w_popup_title, title_ಪಠ್ಯ,
					  ಒಟ್ಟು_ಅಗಲ - 2, title_byte_len + 1);
		    screen_puts(title_ಪಠ್ಯ, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_ಪಠ್ಯ);
		}

		title_len = ಒಟ್ಟು_ಅಗಲ - 2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	ತುದಿ_padding = cl.eff_padding[0];
	ಆದರೆ (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    ಆದರೆ (title_len > 0)
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, title_wincol,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			title_wincol + title_len, wincol + ಒಟ್ಟು_ಅಗಲ,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + ಒಟ್ಟು_ಅಗಲ,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    ಆದರೆ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + ಒಟ್ಟು_ಅಗಲ - 1, border_attr[1]);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cl.eff_padding[0] == 0 && popup_ತುದಿ_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    ತುದಿ_padding = 1;

	ಆದರೆ (ತುದಿ_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + ಒಟ್ಟು_ಅಗಲ - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    ಆದರೆ (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	ಆದರೆ (ತುದಿ_padding > 0)
	{
	    ಅಡ್ಡಸಾಲು = wp->w_winrow + cl.eff_border[0];
	    ಆದರೆ (title_len > 0 && ಅಡ್ಡಸಾಲು == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && saved_screen.ಸಾಲುಗಳು != ಖಾಲಿ)
		{
		    // Left of title
		    fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		ಅಡ್ಡಸಾಲು += 1;
		ತುದಿ_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && saved_screen.ಸಾಲುಗಳು != ಖಾಲಿ)
	    {
		fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + ತುದಿ_padding, padcol, padendcol,
			&saved_screen);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + ತುದಿ_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	ಆದರೆ (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ = wp->w_ಎತ್ತರ;
	    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ;

	    sb_thumb_ಎತ್ತರ = ((linenr_T)ಎತ್ತರ * ಎತ್ತರ + linecount / 2)
								   / linecount;
	    ಆದರೆ (wp->w_topline > 1 && sb_thumb_ಎತ್ತರ == ಎತ್ತರ)
		--sb_thumb_ಎತ್ತರ;  // scrolled, no full thumb
	    ಆದರೆ (sb_thumb_ಎತ್ತರ == 0)
		sb_thumb_ಎತ್ತರ = 1;
	    ಆದರೆ (linecount <= wp->w_ಎತ್ತರ || wp->w_ಎತ್ತರ == 0)
		// it just fits, avoid divide by zero
		sb_thumb_ತುದಿ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		sb_thumb_ತುದಿ = (wp->w_topline - 1
				+ (linecount / wp->w_ಎತ್ತರ) / 2)
				* (wp->w_ಎತ್ತರ - sb_thumb_ಎತ್ತರ)
						  / (linecount - wp->w_ಎತ್ತರ);
	    ಆದರೆ (wp->w_topline > 1 && sb_thumb_ತುದಿ == 0 && ಎತ್ತರ > 1)
		sb_thumb_ತುದಿ = 1;  // show it's scrolled
	    ಕೊನೆಯ = ಒಟ್ಟು_ಎತ್ತರ_2 - ತುದಿ_off - wp->w_popup_border[2];
	    ಆದರೆ (sb_thumb_ತುದಿ >= ಕೊನೆಯ)
		// show at least one character
		sb_thumb_ತುದಿ = ಕೊನೆಯ - 1;

	    ಆದರೆ (wp->w_scrollbar_highlight != ಖಾಲಿ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    ಇಲ್ಲದಿದ್ದರೆ
		attr_scroll = highlight_attr[HLF_PSB];
	    ಆದರೆ (wp->w_thumb_highlight != ಖಾಲಿ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    ಇಲ್ಲದಿದ್ದರೆ
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	ಗಾಗಿ (i = cl.eff_border[0]; i < ಒಟ್ಟು_ಎತ್ತರ_2 - cl.eff_border[2]; ++i)
	{
	    ಪೂರ್ಣಾಂಕ	pad_ಎಡ;
	    // left and right padding only needed next to the body
	    ಪೂರ್ಣಾಂಕ do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < ಒಟ್ಟು_ಎತ್ತರ_2 - cl.eff_border[2] - cl.eff_padding[2];

	    ಅಡ್ಡಸಾಲು = wp->w_winrow + i;

	    // left border
	    ಆದರೆ (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, ಅಡ್ಡಸಾಲು, wincol, border_attr[3]);
	    }
	    ಆದರೆ (do_padding && cl.eff_padding[3] > 0)
	    {
		ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = wincol + wp->w_popup_border[3];

		// left padding
		pad_ಎಡ = wp->w_popup_padding[3];
		ಆದರೆ (ಲಂಬಸಾಲು < 0)
		{
		    pad_ಎಡ += ಲಂಬಸಾಲು;
		    ಲಂಬಸಾಲು = 0;
		}
		ಆದರೆ (pad_ಎಡ > 0)
		{
		    ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && saved_screen.ಸಾಲುಗಳು != ಖಾಲಿ)
			fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + pad_ಎಡ,
				&saved_screen);
		    ಇಲ್ಲದಿದ್ದರೆ
			screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು, ಲಂಬಸಾಲು + pad_ಎಡ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    ಆದರೆ (wp->w_has_scrollbar)
	    {
		ಪೂರ್ಣಾಂಕ ಸಾಲು = i - ತುದಿ_off;
		ಪೂರ್ಣಾಂಕ scroll_ಲಂಬಸಾಲು = wp->w_wincol + ಒಟ್ಟು_ಅಗಲ - 1
						       - wp->w_popup_border[1];

		ಆದರೆ (ಸಾಲು >= 0 && ಸಾಲು < wp->w_ಎತ್ತರ)
		    screen_putchar(' ', ಅಡ್ಡಸಾಲು, scroll_ಲಂಬಸಾಲು,
			    ಸಾಲು >= sb_thumb_ತುದಿ
				       && ಸಾಲು < sb_thumb_ತುದಿ + sb_thumb_ಎತ್ತರ
						  ? attr_thumb : attr_scroll);
		ಇಲ್ಲದಿದ್ದರೆ
		    screen_putchar(' ', ಅಡ್ಡಸಾಲು, scroll_ಲಂಬಸಾಲು, popup_attr);
	    }
	    // right border
	    ಆದರೆ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, ಅಡ್ಡಸಾಲು, wincol + ಒಟ್ಟು_ಅಗಲ - 1, border_attr[1]);
	    }
	    // right padding
	    ಆದರೆ (do_padding && cl.eff_padding[1] > 0)
	    {
		ಪೂರ್ಣಾಂಕ pad_ಲಂಬಸಾಲು_ಆರಂಭ = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_ಅಗಲ;
		ಪೂರ್ಣಾಂಕ pad_ಲಂಬಸಾಲು_ಕೊನೆ = pad_ಲಂಬಸಾಲು_ಆರಂಭ + wp->w_popup_padding[1];

		ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && saved_screen.ಸಾಲುಗಳು != ಖಾಲಿ)
		    fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, pad_ಲಂಬಸಾಲು_ಆರಂಭ, pad_ಲಂಬಸಾಲು_ಕೊನೆ,
			    &saved_screen);
		ಇಲ್ಲದಿದ್ದರೆ
		    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, pad_ಲಂಬಸಾಲು_ಆರಂಭ, pad_ಲಂಬಸಾಲು_ಕೊನೆ,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	ಆದರೆ (wp->w_popup_shadow)
	{
	    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = wincol + ಒಟ್ಟು_ಅಗಲ;
	    ಗಾಗಿ (i = 0; i < ಒಟ್ಟು_ಎತ್ತರ_2; ++i)
	    {
		ಅಡ್ಡಸಾಲು = wp->w_winrow + i + 1;
		put_shadow_char(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
		put_shadow_char(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
	    }
	}

	ಆದರೆ (cl.eff_padding[2] > 0)
	{
	    // bottom padding -- sits right after the visible content rows.
	    // Derive the row from total_height so it always lands inside the
	    // popup's drawn extent, including the corner case where the top
	    // clip consumes more rows than the content itself (so the visible
	    // content height is zero).  A formula based on
	    // w_height - clip_top_content - clip_bot_content can go negative
	    // there and would draw the padding above w_winrow.
	    ಅಡ್ಡಸಾಲು = wp->w_winrow + ಒಟ್ಟು_ಎತ್ತರ_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && saved_screen.ಸಾಲುಗಳು != ಖಾಲಿ)
		fill_opacity_padding(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    ಇಲ್ಲದಿದ್ದರೆ
		screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	ಆದರೆ (cl.eff_border[2] > 0)
	{
	    // bottom border
	    ಅಡ್ಡಸಾಲು = wp->w_winrow + ಒಟ್ಟು_ಎತ್ತರ_2 - 1;
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + ಒಟ್ಟು_ಅಗಲ,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    ಆದರೆ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, ಅಡ್ಡಸಾಲು, wincol + ಒಟ್ಟು_ಅಗಲ - 1, border_attr[2]);
	    }
	}

	ಆದರೆ (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    ಅಡ್ಡಸಾಲು = wp->w_winrow + ಒಟ್ಟು_ಎತ್ತರ_2;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 2 + (wincol < 0 ? 0 : wincol);
		    ಲಂಬಸಾಲು < wincol + ಒಟ್ಟು_ಅಗಲ; ಲಂಬಸಾಲು++)
		put_shadow_char(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	}

	ಆದರೆ (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + ಒಟ್ಟು_ಅಗಲ - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ಖಾಲಿ;

	// Back to the normal zindex.
	screen_zindex = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	// if this was the message window popup may start the timer now
	may_ಆರಂಭ_ಸಂದೇಶ_win_timer(wp);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (override_success)
	    ತೆಗೆ_highlight_overrides();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
	popup_position_image(wp);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    VIM_CLEAR(ತಳ_screenlines);
    VIM_CLEAR(ತಳ_screenattrs);
    VIM_CLEAR(ತಳ_screenlinesuc);
    ತಳ_screen_ಅಡ್ಡಸಾಲುಗಳು = 0;
    ತಳ_screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
    // In case win_update() called start_search_hl().
    ಕೊನೆ_ಹುಡುಕು_hl();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Mark references in callbacks of one popup window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_one_popup(win_T *wp, ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    typval_T	tv;

    ಆದರೆ (wp->w_close_cb.cb_partial != ಖಾಲಿ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಆದರೆ (wp->w_filter_cb.cb_partial != ಖಾಲಿ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    }
    abort = abort || ಗಣ_ref_ಒಳ_list(wp->w_popup_ಮುಖವಾಡ, copyID);
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_popups(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    ಗಾಗಿ (wp = ಮೊದಲ_popupwin; !abort && wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	abort = abort || ಗಣ_ref_ಒಳ_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	ಗಾಗಿ (wp = tp->tp_ಮೊದಲ_popupwin; !abort && wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    abort = abort || ಗಣ_ref_ಒಳ_one_popup(wp, copyID);
	ಆದರೆ (abort)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ abort;
}

    ಪೂರ್ಣಾಂಕ
popup_is_popup(win_T *wp)
{
    ಹಿಂತಿರುಗಿ wp->w_popup_flags != 0;
}

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ಕಂಡುಹಿಡಿ_preview_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_p_pvw)
	    ಹಿಂತಿರುಗಿ wp;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_popup_flags & POPF_INFO)
	    ಹಿಂತಿರುಗಿ wp;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(ಪೂರ್ಣಾಂಕ dir, bool by_ಪುಟ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    win_T	*wp = popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ();
    ಪೂರ್ಣಾಂಕ		by;
    linenr_T	new_topline;

    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    by = by_ಪುಟ ? (wp->w_ಎತ್ತರ > 2 ? wp->w_ಎತ್ತರ - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    ಆದರೆ (new_topline < 1)
	new_topline = 1;
    ಆದರೆ (new_topline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	new_topline = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (new_topline != wp->w_topline)
    {
	ಗಣ_topline(wp, new_topline);
	popup_ಗಣ_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    ಹಿಂತಿರುಗಿ true;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ false;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
f_popup_findecho(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
    rettv->vval.v_ಸಂಖ್ಯೆ = ಸಂದೇಶ_win_2 == ಖಾಲಿ ? 0 : ಸಂದೇಶ_win_2->w_id;
#ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
f_popup_findinfo(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    win_T   *wp = popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ();

    rettv->vval.v_ಸಂಖ್ಯೆ = wp == ಖಾಲಿ ? 0 : wp->w_id;
#ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
f_popup_findpreview(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    win_T   *wp = popup_ಕಂಡುಹಿಡಿ_preview_ಕಿಟಕಿ();

    rettv->vval.v_ಸಂಖ್ಯೆ = wp == ಖಾಲಿ ? 0 : wp->w_id;
#ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    ಪೂರ್ಣಾಂಕ
popup_create_preview_ಕಿಟಕಿ(ಪೂರ್ಣಾಂಕ info)
{
    win_T *wp = popup_create(ಖಾಲಿ, ಖಾಲಿ, info ? TYPE_INFO : TYPE_PREVIEW);

    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (info)
	wp->w_popup_flags |= POPF_INFO;
    ಇಲ್ಲದಿದ್ದರೆ
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    ಆದರೆ (wp->w_minwidth > 0)
	wp->w_ಅಗಲ = wp->w_minwidth;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_maxwidth > 0)
	wp->w_ಅಗಲ = wp->w_maxwidth;
    ಇಲ್ಲದಿದ್ದರೆ
	wp->w_ಅಗಲ = curwin->w_ಅಗಲ;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_ಬಫರ್->b_locked = FALSE;

    win_enter(wp, FALSE);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Close any preview popup.
 */
    ಶೂನ್ಯ
popup_close_preview(ಶೂನ್ಯ)
{
    win_T *wp = popup_ಕಂಡುಹಿಡಿ_preview_ಕಿಟಕಿ();

    ಆದರೆ (wp != ಖಾಲಿ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    ಶೂನ್ಯ
popup_hide_info(ಶೂನ್ಯ)
{
    win_T *wp = popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ();

    ಆದರೆ (wp != ಖಾಲಿ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
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
    win_T *wp = popup_ಕಂಡುಹಿಡಿ_info_ಕಿಟಕಿ();

    ಆದರೆ (wp != ಖಾಲಿ)
	popup_close_with_retval(wp, -1);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    ಪೂರ್ಣಾಂಕ
popup_overlaps_cmdline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ popup_on_cmdline;
}

#ಆದರೆ defined(HAS_ಸಂದೇಶ_ಕಿಟಕಿ)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_ಸಂದೇಶ_win(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_win_2 != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ_win_2;

    ಪೂರ್ಣಾಂಕ i;

    ಸಂದೇಶ_win_2 = popup_create(ಖಾಲಿ, ಖಾಲಿ, TYPE_ಸಂದೇಶ_WIN);

    ಆದರೆ (ಸಂದೇಶ_win_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // use the full screen width
    ಸಂದೇಶ_win_2->w_ಅಗಲ = Columns;

    // position at bottom of screen
    ಸಂದೇಶ_win_2->w_popup_pos = POPPOS_BOTTOM;
    ಸಂದೇಶ_win_2->w_wantcol = 1;
    ಸಂದೇಶ_win_2->w_minwidth = 9999;
    ಸಂದೇಶ_win_2->w_firstline = -1;

    // no padding, border at the top
    ಗಾಗಿ (i = 0; i < 4; ++i)
	ಸಂದೇಶ_win_2->w_popup_padding[i] = 0;
    ಗಾಗಿ (i = 1; i < 4; ++i)
	ಸಂದೇಶ_win_2->w_popup_border[i] = 0;

    ಆದರೆ (ಸಂದೇಶ_win_2->w_popup_timer != ಖಾಲಿ)
	ಸಂದೇಶ_win_2->w_popup_timer->tr_keep = TRUE;
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_win_2;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    ಶೂನ್ಯ
popup_show_ಸಂದೇಶ_win(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_win_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((ಸಂದೇಶ_win_2->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(ಸಂದೇಶ_win_2, TYPE_ಸಂದೇಶ_WIN);
	popup_show(ಸಂದೇಶ_win_2);
    }
    ಆರಂಭ_ಸಂದೇಶ_win_timer = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_ಆರಂಭ_ಸಂದೇಶ_win_timer(win_T *wp)
{
    ಆದರೆ (wp == ಸಂದೇಶ_win_2 && ಆರಂಭ_ಸಂದೇಶ_win_timer)
    {
	ಆದರೆ (ಸಂದೇಶ_win_2->w_popup_timer != ಖಾಲಿ)
	{
	    ಸಂದೇಶ_win_2->w_popup_timer->tr_interval = ಸಂದೇಶ_win_ಸಮಯ;
	    timer_ಆರಂಭ(ಸಂದೇಶ_win_2->w_popup_timer);
	    ಸಂದೇಶ_win_ಸಮಯ = 3000;
	}
	ಆರಂಭ_ಸಂದೇಶ_win_timer = FALSE;
    }
}

    ಪೂರ್ಣಾಂಕ
popup_ಸಂದೇಶ_win_visible(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ_win_2 != ಖಾಲಿ
	&& (ಸಂದೇಶ_win_2->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    ಶೂನ್ಯ
popup_hide_ಸಂದೇಶ_win(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_win_2 != ಖಾಲಿ)
	popup_hide(ಸಂದೇಶ_win_2);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_ಸಂದೇಶ_didout = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ew_ಸಂದೇಶ_didout = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ew_ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    ಶೂನ್ಯ
ಆರಂಭ_echowindow(ಪೂರ್ಣಾಂಕ ಸಮಯ_sec)
{
    ಒಳ_echowindow = TRUE;
    save_ಸಂದೇಶ_didout = ಸಂದೇಶ_didout;
    save_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಸಂದೇಶ_didout = ew_ಸಂದೇಶ_didout;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = ew_ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಆದರೆ (ಸಮಯ_sec != 0)
	ಸಂದೇಶ_win_ಸಮಯ = ಸಮಯ_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    ಶೂನ್ಯ
ಕೊನೆ_echowindow(ಶೂನ್ಯ)
{
    ಒಳ_echowindow = FALSE;

    ಆದರೆ ((State & ಬಹುಲಕ_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_ಸಂದೇಶ_didout = TRUE;
    ew_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು == 0 ? 1 : ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಸಂದೇಶ_didout = save_ಸಂದೇಶ_didout;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = save_ಸಂದೇಶ_ಲಂಬಸಾಲು;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    ಪೂರ್ಣಾಂಕ
popup_win_closed(win_T *win)
{
    ಪೂರ್ಣಾಂಕ	    ಸುತ್ತು;
    win_T   *wp;
    win_T   *ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ	    ret = FALSE;

    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	ಗಾಗಿ (wp = ಸುತ್ತು == 1 ? ಮೊದಲ_popupwin : curtab->tp_ಮೊದಲ_popupwin;
							 wp != ಖಾಲಿ; wp = ಮುಂದಿನ)
	{
	    ಮುಂದಿನ = wp->w_ಮುಂದಿನ;
	    ಆದರೆ (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    ಶೂನ್ಯ
popup_ಗಣ_title(win_T *wp)
{
    ಆದರೆ (wp->w_ಬಫರ್->b_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_ಬಫರ್, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_ಬಫರ್->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    ಆದರೆ (wp->w_popup_title != ಖಾಲಿ)
	vim_snprintf((ಅಕ್ಷರ *)wp->w_popup_title, len, " %s ",
		wp->w_ಬಫರ್->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    ಶೂನ್ಯ
popup_update_preview_title(ಶೂನ್ಯ)
{
    win_T *wp = popup_ಕಂಡುಹಿಡಿ_preview_ಕಿಟಕಿ();

    ಆದರೆ (wp != ಖಾಲಿ)
	popup_ಗಣ_title(wp);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // FEAT_PROP_POPUP
