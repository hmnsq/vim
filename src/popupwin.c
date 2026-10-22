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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_PROP_POPUP)

రకనిర్వచనం నిర్మాణం {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_నమోదు_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		చురుకైన;
    పూర్ణాంకం		winrow;
    పూర్ణాంకం		wincol;
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		leftoff;
    పూర్ణాంకం		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
రకనిర్వచనం నిర్మాణం {
    schar_T	*పంక్తులు;
    పూర్ణాంకం		*attrs;
    u8char_T	*linesuc;
    పూర్ణాంకం		ప్రారంభం_అడ్డువరుస;
    పూర్ణాంకం		ప్రారంభం_నిలువు;
    పూర్ణాంకం		అడ్డువరుసలు_2;
    పూర్ణాంకం		నిలువుల_సంఖ్య;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
రకనిర్వచనం నిర్మాణం {
    linenr_T	firstline;
    పూర్ణాంకం		blend;
    పూర్ణాంకం		flags;
    పూర్ణాంకం		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		winrow;
    పూర్ణాంకం		wincol;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		leftcol;
    పూర్ణాంకం		leftoff;
    పూర్ణాంకం		has_scrollbar;
    పూర్ణాంకం		topoff;
    పూర్ణాంకం		bottomoff;
    పూర్ణాంకం		leftclip;
    పూర్ణాంకం		rightclip;
} popup_layout_T;

స్థిర poppos_నమోదు_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
// Window used for ":echowindow"
స్థిర win_T *సందేశం_win_2 = ఖాళీ;

// Time used for the next ":echowindow" message in msec.
స్థిర పూర్ణాంకం  సందేశం_win_సమయం = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
స్థిర పూర్ణాంకం    ప్రారంభం_సందేశం_win_timer = FALSE;

స్థిర శూన్య may_ప్రారంభం_సందేశం_win_timer(win_T *wp);
#అయితే_ముగింపు

స్థిర పూర్ణాంకం popup_on_cmdline = FALSE;

స్థిర శూన్య popup_adjust_position(win_T *wp);
స్థిర bool popup_area_మారింది(win_T *wp, popup_area_T *area);
స్థిర శూన్య popup_redraw_exposed_area(popup_area_T *area);
స్థిర శూన్య popup_save_area(win_T *wp, popup_area_T *area);
స్థిర శూన్య popup_free_saved_screen(popup_saved_screen_T *saved_screen);
స్థిర శూన్య popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
స్థిర bool popup_layout_మారింది(win_T *wp, popup_layout_T *layout);
స్థిర bool popup_style_మారింది(win_T *wp, popup_style_snapshot_T *style);
స్థిర శూన్య popup_save_style(win_T *wp, popup_style_snapshot_T *style);
స్థిర శూన్య popup_save_layout(win_T *wp, popup_layout_T *layout);
స్థిర శూన్య redraw_under_popup_area(పూర్ణాంకం winrow, పూర్ణాంకం wincol, పూర్ణాంకం ఎత్తు,
	పూర్ణాంకం వెడల్పు, పూర్ణాంకం leftoff);
స్థిర శూన్య redraw_overlapped_opacity_popups(పూర్ణాంకం winrow, పూర్ణాంకం wincol,
	పూర్ణాంకం ఎత్తు, పూర్ణాంకం వెడల్పు, పూర్ణాంకం leftoff, పూర్ణాంకం zindex);
స్థిర శూన్య redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    స్థిర పూర్ణాంకం
popup_options_one(dict_T *dict, char_u *కీలకం)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    పూర్ణాంకం		n = 0;

    di = dict_కనుగొను(dict, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి MAXCOL;

    val = tv_get_string(&di->di_tv);
    ఒకవేళ (STRNCMP(val, "cursor", 6) != 0)
	తిరిగి dict_get_సంఖ్య_check(dict, కీలకం);

    setcursor_mayforce(TRUE);
    s = val + 6;
    ఒకవేళ (*s != NUL)
    {
	endp = s;
	ఒకవేళ (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((అక్షరం *)s, (అక్షరం **)&endp, 10);
	ఒకవేళ (endp != ఖాళీ && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), val);
	    తిరిగి 0;
	}
    }

    ఒకవేళ (STRCMP(కీలకం, "line") == 0)
	n = screen_screenrow() + 1 + n;
    లేకపోతే // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    ఒకవేళ (n == 0)
	n = -1;
    తిరిగి n;
}

    స్థిర పూర్ణాంకం
సమితి_padding_border(dict_T *dict, పూర్ణాంకం *array, అక్షరం *name, పూర్ణాంకం max_val)
{
    dictitem_T	*di;

    di = dict_కనుగొను(dict, (char_u *)name, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి OK;

    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	తిరిగి FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    పూర్ణాంకం		i;
    పూర్ణాంకం		nr;

    కొరకు (i = 0; i < 4; ++i)
	array[i] = 1;
    ఒకవేళ (list == ఖాళీ)
	తిరిగి OK;

    CHECK_LIST_MATERIALIZE(list);
    కొరకు (i = 0, li = list->lv_మొదటి; i < 4 && i < list->lv_len;
	    ++i, li = li->li_తదుపరి)
    {
	nr = (పూర్ణాంకం)tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    తిరిగి OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    స్థిర శూన్య
సమితి_moved_విలువలు(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.నిలువు;
    wp->w_popup_maxcol = curwin->w_cursor.నిలువు;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    స్థిర శూన్య
సమితి_moved_columns(win_T *wp, పూర్ణాంకం flags)
{
    char_u	*ptr;
    పూర్ణాంకం		len = కనుగొను_ident_under_cursor(&ptr, flags | కనుగొను_NOERROR);
    పూర్ణాంకం		mincol;

    ఒకవేళ (len <= 0)
	తిరిగి;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (పూర్ణాంకం)(ptr - ml_get_curline());
    ఒకవేళ (curwin->w_cursor.నిలువు < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.నిలువు;
	wp->w_popup_maxcol = curwin->w_cursor.నిలువు;
	తిరిగి;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    స్థిర శూన్య
సమితి_mousemoved_విలువలు(win_T *wp)
{
    wp->w_popup_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
    wp->w_popup_mouse_mincol = mouse_నిలువు;
    wp->w_popup_mouse_maxcol = mouse_నిలువు;
}

    స్థిర శూన్య
update_popup_ఉపయోగిస్తుంది_mouse_move(శూన్య)
{
    popup_ఉపయోగిస్తుంది_mouse_move = FALSE;
    ఒకవేళ (!popup_visible)
	తిరిగి;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_popup_mouse_అడ్డువరుస != 0)
	{
	    popup_ఉపయోగిస్తుంది_mouse_move = TRUE;
	    తిరిగి;
	}
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_popup_mouse_అడ్డువరుస != 0)
	{
	    popup_ఉపయోగిస్తుంది_mouse_move = TRUE;
	    తిరిగి;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    స్థిర శూన్య
సమితి_mousemoved_columns(win_T *wp, పూర్ణాంకం flags)
{
    win_T	*textwp;
    char_u	*పాఠ్యం;
    పూర్ణాంకం		నిలువు;
    pos_T	pos;
    colnr_T	mcol;

    ఒకవేళ (కనుగొను_మాట_under_cursor(mouse_అడ్డువరుస, mouse_నిలువు, TRUE, flags,
				  &textwp, &pos.lnum, &పాఠ్యం, ఖాళీ, &నిలువు) != OK)
	తిరిగి;

    // convert text column to mouse column
    pos.నిలువు = నిలువు;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, ఖాళీ, ఖాళీ, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.నిలువు = నిలువు + (colnr_T)STRLEN(పాఠ్యం) - 1;
    getvcol(textwp, &pos, ఖాళీ, ఖాళీ, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(పాఠ్యం);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    పూర్ణాంకం
popup_on_border(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    తిరిగి (అడ్డువరుస == 0 && wp->w_popup_border[0] > 0)
	    || (అడ్డువరుస == popup_ఎత్తు(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (నిలువు == 0 && wp->w_popup_border[3] > 0)
	    || (నిలువు == popup_వెడల్పు(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    పూర్ణాంకం
popup_close_if_on_X(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (wp->w_popup_close == POPCLOSE_BUTTON
	    && అడ్డువరుస == 0 && నిలువు == popup_వెడల్పు(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

// Values set when dragging a popup window starts.
స్థిర పూర్ణాంకం drag_ప్రారంభం_అడ్డువరుస;
స్థిర పూర్ణాంకం drag_ప్రారంభం_నిలువు;
స్థిర పూర్ణాంకం drag_ప్రారంభం_wantline;
స్థిర పూర్ణాంకం drag_ప్రారంభం_wantcol;
స్థిర పూర్ణాంకం drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    శూన్య
popup_ప్రారంభం_drag(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    drag_ప్రారంభం_అడ్డువరుస = mouse_అడ్డువరుస;
    drag_ప్రారంభం_నిలువు = mouse_నిలువు;
    ఒకవేళ (wp->w_wantline <= 0)
	drag_ప్రారంభం_wantline = wp->w_winrow + 1;
    లేకపోతే
	drag_ప్రారంభం_wantline = wp->w_wantline;
    ఒకవేళ (wp->w_wantcol == 0)
	drag_ప్రారంభం_wantcol = wp->w_wincol + 1;
    లేకపోతే
	drag_ప్రారంభం_wantcol = wp->w_wantcol;

    // Stop centering the popup
    ఒకవేళ (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && అడ్డువరుస == popup_ఎత్తు(wp) - 1
			    && నిలువు == popup_వెడల్పు(wp) - 1;

    ఒకవేళ (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	ఒకవేళ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	ఒకవేళ (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    శూన్య
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    ఒకవేళ (!win_valid_popup(wp))
	తిరిగి;

    ఒకవేళ ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	పూర్ణాంకం వెడల్పు_inc = mouse_నిలువు - drag_ప్రారంభం_నిలువు;
	పూర్ణాంకం ఎత్తు_inc = mouse_అడ్డువరుస - drag_ప్రారంభం_అడ్డువరుస;

	ఒకవేళ (వెడల్పు_inc != 0)
	{
	    పూర్ణాంకం వెడల్పు = wp->w_వెడల్పు + వెడల్పు_inc;

	    ఒకవేళ (వెడల్పు < 1)
		వెడల్పు = 1;
	    wp->w_minwidth = వెడల్పు;
	    wp->w_maxwidth = వెడల్పు;
	    drag_ప్రారంభం_నిలువు = mouse_నిలువు;
	}

	ఒకవేళ (ఎత్తు_inc != 0)
	{
	    పూర్ణాంకం ఎత్తు = wp->w_ఎత్తు + ఎత్తు_inc;

	    ఒకవేళ (ఎత్తు < 1)
		ఎత్తు = 1;
	    wp->w_minheight = ఎత్తు;
	    wp->w_maxheight = ఎత్తు;
	    drag_ప్రారంభం_అడ్డువరుస = mouse_అడ్డువరుస;
	}

	popup_adjust_position(wp);
	తిరిగి;
    }

    ఒకవేళ (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	తిరిగి;
    wp->w_wantline = drag_ప్రారంభం_wantline + (mouse_అడ్డువరుస - drag_ప్రారంభం_అడ్డువరుస);
    ఒకవేళ (wp->w_wantline < 1)
	wp->w_wantline = 1;
    ఒకవేళ (wp->w_wantline > అడ్డువరుసలు)
	wp->w_wantline = అడ్డువరుసలు;
    wp->w_wantcol = drag_ప్రారంభం_wantcol + (mouse_నిలువు - drag_ప్రారంభం_నిలువు);
    ఒకవేళ (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    ఒకవేళ (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    శూన్య
popup_సమితి_firstline(win_T *wp)
{
    పూర్ణాంకం	    ఎత్తు = wp->w_ఎత్తు;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    అయితే (wp->w_firstline > 1 && wp->w_ఎత్తు < ఎత్తు)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    పూర్ణాంకం
popup_is_లోపల_scrollbar(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    తిరిగి wp->w_has_scrollbar
	&& అడ్డువరుస >= wp->w_popup_border[0]
	&& అడ్డువరుస < popup_ఎత్తు(wp) - wp->w_popup_border[2]
	&& నిలువు == popup_వెడల్పు(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    శూన్య
popup_handle_scrollbar_click(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (!popup_is_లోపల_scrollbar(wp, అడ్డువరుస, నిలువు))
	తిరిగి;

    పూర్ణాంకం	    ఎత్తు = popup_ఎత్తు(wp);
    పూర్ణాంకం	    new_topline = wp->w_topline;

    ఒకవేళ (అడ్డువరుస >= ఎత్తు / 2)
    {
	// Click in lower half, scroll down.
	ఒకవేళ (wp->w_topline < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    ++new_topline;
    }
    లేకపోతే ఒకవేళ (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    ఒకవేళ (new_topline == wp->w_topline)
	తిరిగి;

    సమితి_topline(wp, new_topline);
    ఒకవేళ (wp == curwin)
    {
	ఒకవేళ (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	లేకపోతే ఒకవేళ (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_సమితి_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ఒకవేళ defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    స్థిర శూన్య
popup_కూడు_timeout(win_T *wp, పూర్ణాంకం సమయం, పూర్ణాంకం close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((అక్షరం *)cbbuf, పరిమాణం(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    ఒకవేళ (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	తిరిగి;

    wp->w_popup_timer = create_timer(సమయం, 0);
    callback_T cb = get_callback(&tv);
    ఒకవేళ (cb.cb_name != ఖాళీ && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#అయితే_ముగింపు

    స్థిర poppos_T
get_pos_నమోదు(dict_T *d, పూర్ణాంకం give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    పూర్ణాంకం	    nr;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి POPPOS_NONE;

    కొరకు (nr = 0; nr < (పూర్ణాంకం)ARRAY_నిడివి(poppos_entries); ++nr)
	ఒకవేళ (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    తిరిగి poppos_entries[nr].pp_val;

    ఒకవేళ (give_error)
	semsg(_(e_invalid_వాదం_str), str);
    తిరిగి POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    స్థిర శూన్య
apply_move_options(win_T *wp, dict_T *d)
{
    పూర్ణాంకం		nr;
    char_u	*str;
    dictitem_T	*di;

    ఒకవేళ ((nr = dict_get_సంఖ్య_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    ఒకవేళ ((nr = dict_get_సంఖ్య_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    ఒకవేళ ((nr = dict_get_సంఖ్య_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    ఒకవేళ ((nr = dict_get_సంఖ్య_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    ఒకవేళ (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    ఒకవేళ (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    ఒకవేళ (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_నమోదు(d, TRUE);

	ఒకవేళ (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    ఒకవేళ (str != ఖాళీ)
    {
	wp->w_popup_prop_type = 0;
	ఒకవేళ (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_కనుగొను(d, (char_u *)"textpropwin", -1);
	    ఒకవేళ (di != ఖాళీ)
	    {
		wp->w_popup_prop_win = కనుగొను_win_by_nr_or_id(&di->di_tv);
		ఒకవేళ (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = కనుగొను_prop_type_id(str, wp->w_popup_prop_win->w_బఫరు);
	    ఒకవేళ (nr <= 0)
		nr = కనుగొను_prop_type_id(str, ఖాళీ);
	    ఒకవేళ (nr <= 0)
		semsg(_(e_invalid_వాదం_str), str);
	    లేకపోతే
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_కనుగొను(d, (char_u *)"textpropid", -1);
    ఒకవేళ (di != ఖాళీ)
	wp->w_popup_prop_id = dict_get_సంఖ్య(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    స్థిర శూన్య
handle_moved_వాదం(win_T *wp, dictitem_T *di, పూర్ణాంకం mousemoved)
{
    ఒకవేళ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ఖాళీ)
    {
	char_u  *s = di->di_tv.vval.v_string;
	పూర్ణాంకం	flags = 0;

	ఒకవేళ (STRCMP(s, "word") == 0)
	    flags = కనుగొను_IDENT | కనుగొను_STRING;
	లేకపోతే ఒకవేళ (STRCMP(s, "WORD") == 0)
	    flags = కనుగొను_STRING;
	లేకపోతే ఒకవేళ (STRCMP(s, "expr") == 0)
	    flags = కనుగొను_IDENT | కనుగొను_STRING | కనుగొను_EVAL;
	లేకపోతే ఒకవేళ (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_వాదం_str), s);
	ఒకవేళ (flags != 0)
	{
	    ఒకవేళ (mousemoved)
		సమితి_mousemoved_columns(wp, flags);
	    లేకపోతే
		సమితి_moved_columns(wp, flags);
	}
    }
    లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != ఖాళీ
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	పూర్ణాంకం	    mincol;
	పూర్ణాంకం	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_మొదటి;
	ఒకవేళ (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_సంఖ్య(&l->lv_మొదటి->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    ఒకవేళ (mousemoved)
		wp->w_popup_mouse_అడ్డువరుస = nr;
	    లేకపోతే
		wp->w_popup_lnum = nr;
	    li = li->li_తదుపరి;
	    ఒకవేళ (nr == 0)
		wp->w_popup_curwin = ఖాళీ;
	}

	mincol = tv_get_సంఖ్య(&li->li_tv);
	maxcol = tv_get_సంఖ్య(&li->li_తదుపరి->li_tv);
	ఒకవేళ (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	లేకపోతే
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), tv_get_string(&di->di_tv));
}

    స్థిర శూన్య
check_highlight(dict_T *dict, అక్షరం *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_కనుగొను(dict, (char_u *)name, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి;

    ఒకవేళ (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_విలువ_for_వాదం_str), name);
    లేకపోతే
    {
	str = tv_get_string(&di->di_tv);
	ఒకవేళ (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    స్థిర శూన్య
popup_show_curline(win_T *wp)
{
    ఒకవేళ (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    లేకపోతే ఒకవేళ (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_ఎత్తు + 1;
	ఒకవేళ (wp->w_topline < 1)
	    wp->w_topline = 1;
	లేకపోతే ఒకవేళ (wp->w_topline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    wp->w_topline = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	అయితే (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_ఎత్తు + 1) > wp->w_ఎత్తు)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    ఒకవేళ (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    స్థిర char_u *
popup_get_sign_name(win_T *wp)
{
    స్థిర అక్షరం    buf[30];

    vim_snprintf(buf, పరిమాణం(buf), "popup-%d", wp->w_id);
    తిరిగి (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    స్థిర శూన్య
popup_highlight_curline(win_T *wp)
{
    పూర్ణాంకం	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_బఫరు, (char_u *)"PopUpMenu");

    ఒకవేళ ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	ఒకవేళ (!sign_exists_by_name(sign_name))
	{
	    అక్షరం *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, ఖాళీ, (char_u *)linehl, ఖాళీ, ఖాళీ, ఖాళీ,
		    ఖాళీ, SIGN_DEF_PRIO);
	}

	sign_ఉంచు(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_బఫరు, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    లేకపోతే
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_చివరి_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    స్థిర పూర్ణాంకం
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    పూర్ణాంకం		nr;
    char_u	*str;

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#అయితే_ముగింపు

    // TODO: flip

    di = dict_కనుగొను(dict, (char_u *)"firstline", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	wp->w_firstline = dict_get_సంఖ్య(dict, "firstline");
	ఒకవేళ (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    ఒకవేళ (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    ఒకవేళ (str != ఖాళీ)
    {
	char_u	*title = vim_strsave(str);

	ఒకవేళ (title != ఖాళీ)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    ఒకవేళ (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_కనుగొను(dict, (char_u *)"opacity", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	nr = dict_get_సంఖ్య(dict, "opacity");
	ఒకవేళ (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	లేకపోతే ఒకవేళ (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	లేకపోతే ఒకవేళ (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	లేకపోతే
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    ఒకవేళ (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    లేకపోతే
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_కనుగొను(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != ఖాళీ)
    {
	image_T		    *img;
	image_T		    *cur_img = ఖాళీ;
	image_placement_T   *ఉంచు;
	పూర్ణాంకం		    cw, ch;

	ఒకవేళ (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = ఖాళీ;
	    }
	    వెళ్ళు image_పూర్తి;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	ఒకవేళ (wp->w_popup_imagep != ఖాళీ && wp->w_popup_imagep->img != ఖాళీ)
	    cur_img = wp->w_popup_imagep->img;

	img = కూడు_image(di->di_tv.vval.v_dict, cur_img, true);
	ఒకవేళ (img == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (img == cur_img)
	    వెళ్ళు image_పూర్తి;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	ఉంచు = image_placement_new(img, wp->w_id, iplace_use, false);

	ఒకవేళ (ఉంచు == ఖాళీ)
	{
	    image_unref(img);
	    తిరిగి FAIL;
	}

	ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = ఉంచు;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	ఒకవేళ (dict_కనుగొను(dict, (char_u *)"minwidth", -1) == ఖాళీ)
	    wp->w_minwidth = cw;
	ఒకవేళ (dict_కనుగొను(dict, (char_u *)"maxwidth", -1) == ఖాళీ)
	    wp->w_maxwidth = cw;
	ఒకవేళ (dict_కనుగొను(dict, (char_u *)"minheight", -1) == ఖాళీ)
	    wp->w_minheight = ch;
	ఒకవేళ (dict_కనుగొను(dict, (char_u *)"maxheight", -1) == ఖాళీ)
	    wp->w_maxheight = ch;
image_పూర్తి:
	;
    }
    లేకపోతే
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(ఖాళీ, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	    image_placement_సమితి_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_ఎత్తు, wp->w_వెడల్పు);
    }
#అయితే_ముగింపు

    di = dict_కనుగొను(dict, (char_u *)"close", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	పూర్ణాంకం ok = TRUE;

	ఒకవేళ (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != ఖాళీ)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    ఒకవేళ (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    లేకపోతే ఒకవేళ (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    లేకపోతే ఒకవేళ (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    లేకపోతే
		ok = FALSE;
	}
	లేకపోతే
	    ok = FALSE;
	ఒకవేళ (!ok)
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    ఒకవేళ (str != ఖాళీ)
    {
	అక్షరం *errmsg = update_wincolor(wp, str);

	ఒకవేళ (errmsg == ఖాళీ)
	    సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	లేకపోతే
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    ఒకవేళ (str != ఖాళీ)
    {
	అక్షరం *errmsg = update_winhighlight(wp, str);

	ఒకవేళ (errmsg == ఖాళీ)
	    సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	లేకపోతే
	    emsg(_(errmsg));
    }

    ఒకవేళ (సమితి_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	సమితి_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	తిరిగి FAIL;

    di = dict_కనుగొను(dict, (char_u *)"borderhighlight", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	ఒకవేళ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ఖాళీ)
	{
	    emsg(_(e_list_required));
	    తిరిగి FAIL;
	}
	లేకపోతే
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    పూర్ణాంకం		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    ఒకవేళ (list->lv_len == 0)
	    {
		కొరకు (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = ఖాళీ;
		}
	    }
	    లేకపోతే
	    {
		కొరకు (i = 0, li = list->lv_మొదటి; i < 4 && i < list->lv_len;
						     ++i, li = li->li_తదుపరి)
		{
		    str = tv_get_string(&li->li_tv);
		    ఒకవేళ (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			ఒకవేళ (hl != ఖాళీ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		ఒకవేళ (list->lv_len == 1 && wp->w_border_highlight[0] != ఖాళీ)
		    కొరకు (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			ఒకవేళ (hl != ఖాళీ)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_కనుగొను(dict, (char_u *)"borderchars", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    తిరిగి FAIL;
	}
	లేకపోతే
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    పూర్ణాంకం		i;

	    ఒకవేళ (list != ఖాళీ)
	    {
		CHECK_LIST_MATERIALIZE(list);
		కొరకు (i = 0, li = list->lv_మొదటి; i < 8 && i < list->lv_len;
							 ++i, li = li->li_తదుపరి)
		{
		    str = tv_get_string(&li->li_tv);
		    ఒకవేళ (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		ఒకవేళ (list->lv_len == 1)
		    కొరకు (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		ఒకవేళ (list->lv_len == 2)
		{
		    కొరకు (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    కొరకు (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_కనుగొను(dict, (char_u *)"zindex", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	wp->w_zindex = dict_get_సంఖ్య(dict, "zindex");
	ఒకవేళ (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	ఒకవేళ (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_కనుగొను(dict, (char_u *)"mask", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	పూర్ణాంకం ok = FALSE;

	ఒకవేళ (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != ఖాళీ)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_వస్తువులు(di->di_tv.vval.v_list, li)
	    {
		ఒకవేళ (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == ఖాళీ
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    ఆపు;
		}
		లేకపోతే
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	ఒకవేళ (ok)
	{
	    wp->w_popup_ముసుగు = di->di_tv.vval.v_list;
	    ++wp->w_popup_ముసుగు->lv_refcount;
	    VIM_CLEAR(wp->w_popup_ముసుగు_cells);
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), "mask");
	    తిరిగి FAIL;
	}
    }

#ఒకవేళ defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_సంఖ్య(dict, "time");
    ఒకవేళ (nr > 0)
	popup_కూడు_timeout(wp, nr, TRUE);
#అయితే_ముగింపు

    di = dict_కనుగొను(dict, (char_u *)"moved", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	సమితి_moved_విలువలు(wp);
	handle_moved_వాదం(wp, di, FALSE);
    }

    di = dict_కనుగొను(dict, (char_u *)"mousemoved", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	సమితి_mousemoved_విలువలు(wp);
	handle_moved_వాదం(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_కనుగొను(dict, (char_u *)"filter", -1);
    ఒకవేళ (di != ఖాళీ)
    {
	callback_T	callback = get_callback(&di->di_tv);

	ఒకవేళ (callback.cb_name != ఖాళీ)
	{
	    free_callback(&wp->w_filter_cb);
	    సమితి_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    ఒకవేళ (nr != -1)
    {
	ఒకవేళ (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    ఒకవేళ (str != ఖాళీ)
    {
	ఒకవేళ (STRCMP(str, "a") == 0)
	    wp->w_filter_బహుళకం = బహుళకం_ALL;
	లేకపోతే
	    wp->w_filter_బహుళకం = బహుళకం_str2flags(str);
    }

    di = dict_కనుగొను(dict, (char_u *)"callback", -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి OK;

    callback_T	callback = get_callback(&di->di_tv);
    ఒకవేళ (callback.cb_name == ఖాళీ)
	తిరిగి OK;

    free_callback(&wp->w_close_cb);
    సమితి_callback(&wp->w_close_cb, &callback);
    తిరిగి OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    స్థిర పూర్ణాంకం
apply_options(win_T *wp, dict_T *dict, పూర్ణాంకం create)
{
    పూర్ణాంకం		nr;

    apply_move_options(wp, dict);

    ఒకవేళ (create)
	సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    ఒకవేళ (apply_general_options(wp, dict) == FAIL)
	తిరిగి FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    ఒకవేళ (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    ఒకవేళ (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	ఒకవేళ (wp->w_firstline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    wp->w_cursor.lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	లేకపోతే ఒకవేళ (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_ఎత్తు)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    ఒకవేళ (create)
	popup_ముసుగు_refresh = TRUE;
    popup_highlight_curline(wp);

    తిరిగి OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    స్థిర శూన్య
కూడు_popup_అక్షరశ్రేణులు(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_వస్తువులు(l, li)
	ఒకవేళ (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == ఖాళీ ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    స్థిర శూన్య
కూడు_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_వాదం_1_list_అంశం_nr_dictionary_required), lnum + 1);
	    తిరిగి;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == ఖాళీ ? ఖాళీ : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == ఖాళీ ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    కొరకు (li = l->lv_మొదటి; li != ఖాళీ; li = li->li_తదుపరి, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_కనుగొను(dict, (char_u *)"props", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		తిరిగి;
	    }
	    plist = di->di_tv.vval.v_list;
	    ఒకవేళ (plist != ఖాళీ)
	    {
		FOR_ALL_LIST_వస్తువులు(plist, pli)
		{
		    ఒకవేళ (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			తిరిగి;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    ఒకవేళ (dict != ఖాళీ)
		    {
			పూర్ణాంకం నిలువు = dict_get_సంఖ్య(dict, "col");

			prop_కూడు_common(lnum, నిలువు, dict, buf, ఖాళీ);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    పూర్ణాంకం
popup_శిఖరం_extra(win_T *wp)
{
    పూర్ణాంకం	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    ఒకవేళ (extra == 0 && wp->w_popup_title != ఖాళీ && *wp->w_popup_title != NUL)
	తిరిగి 1;
    తిరిగి extra;
}

/*
 * Get the padding plus border at the left.
 */
    పూర్ణాంకం
popup_ఎడమ_extra(win_T *wp)
{
    తిరిగి wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    పూర్ణాంకం
popup_ఎత్తు(win_T *wp)
{
    తిరిగి wp->w_ఎత్తు
	    + popup_శిఖరం_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    పూర్ణాంకం
popup_వెడల్పు(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    తిరిగి wp->w_వెడల్పు
	    + popup_extra_వెడల్పు(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    పూర్ణాంకం
popup_extra_వెడల్పు(win_T *wp)
{
    తిరిగి wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    స్థిర win_T *
popup_get_clipwin(win_T *wp)
{
    ఒకవేళ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	తిరిగి ఖాళీ;
    ఒకవేళ (win_valid(wp->w_popup_prop_win))
	తిరిగి wp->w_popup_prop_win;
    తిరిగి ఖాళీ;
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
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం శిఖరం_extra;
    పూర్ణాంకం bot_extra;
    పూర్ణాంకం ఎడమ_extra;
    పూర్ణాంకం కుడి_extra;

    పూర్ణాంకం clip_శిఖరం_content;
    పూర్ణాంకం clip_bot_content;
    పూర్ణాంకం clip_ఎడమ_content;
    పూర్ణాంకం clip_కుడి_content;

    పూర్ణాంకం eff_శిఖరం_extra;
    పూర్ణాంకం eff_bot_extra;
    పూర్ణాంకం eff_ఎడమ_extra;
    పూర్ణాంకం eff_కుడి_extra;

    పూర్ణాంకం eff_border[4];
    పూర్ణాంకం eff_padding[4];

    పూర్ణాంకం eff_ఎత్తు;
    పూర్ణాంకం eff_వెడల్పు;
} popup_clip_T;

    స్థిర శూన్య
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    పూర్ణాంకం h, w;

    cl->శిఖరం_extra = popup_శిఖరం_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->ఎడమ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->కుడి_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_శిఖరం_content = wp->w_popup_topoff - cl->శిఖరం_extra;
    ఒకవేళ (cl->clip_శిఖరం_content < 0)
	cl->clip_శిఖరం_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    ఒకవేళ (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_ఎడమ_content = wp->w_popup_leftclip - cl->ఎడమ_extra;
    ఒకవేళ (cl->clip_ఎడమ_content < 0)
	cl->clip_ఎడమ_content = 0;
    cl->clip_కుడి_content = wp->w_popup_rightclip - cl->కుడి_extra;
    ఒకవేళ (cl->clip_కుడి_content < 0)
	cl->clip_కుడి_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	పూర్ణాంకం clip = wp->w_popup_topoff;
	పూర్ణాంకం b = wp->w_popup_border[0];
	పూర్ణాంకం p = wp->w_popup_padding[0];
	పూర్ణాంకం rem;

	ఒకవేళ (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	లేకపోతే
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	పూర్ణాంకం clip = wp->w_popup_bottomoff;
	పూర్ణాంకం b = wp->w_popup_border[2];
	పూర్ణాంకం p = wp->w_popup_padding[2];
	పూర్ణాంకం rem;

	ఒకవేళ (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	లేకపోతే
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
	పూర్ణాంకం excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_ఎత్తు;
	ఒకవేళ (excess > 0)
	{
	    ఒకవేళ (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		ఒకవేళ (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		లేకపోతే
		    cl->eff_border[0] -= excess;
	    }
	    లేకపోతే
		cl->eff_padding[0] -= excess;
	}
    }
    {
	పూర్ణాంకం excess = wp->w_popup_topoff - cl->శిఖరం_extra - wp->w_ఎత్తు;
	ఒకవేళ (excess > 0)
	{
	    ఒకవేళ (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		ఒకవేళ (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		లేకపోతే
		    cl->eff_border[2] -= excess;
	    }
	    లేకపోతే
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_శిఖరం_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_ఎడమ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->ఎడమ_extra;
    cl->eff_కుడి_extra = wp->w_popup_rightclip > 0 ? 0 : cl->కుడి_extra;

    h = wp->w_ఎత్తు - cl->clip_శిఖరం_content - cl->clip_bot_content;
    ఒకవేళ (h < 0)
	h = 0;
    cl->eff_ఎత్తు = cl->eff_శిఖరం_extra + h + cl->eff_bot_extra;

    w = wp->w_వెడల్పు - cl->clip_ఎడమ_content - cl->clip_కుడి_content;
    ఒకవేళ (w < 0)
	w = 0;
    cl->eff_వెడల్పు = cl->eff_ఎడమ_extra + w + cl->eff_కుడి_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		winrow;
    పూర్ణాంకం		wincol;
    పూర్ణాంకం		leftcol;
    పూర్ణాంకం		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    స్థిర శూన్య
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->ఎత్తు  = wp->w_ఎత్తు;
    sv->వెడల్పు   = wp->w_వెడల్పు;
    sv->winrow  = wp->w_winrow;
    sv->wincol  = wp->w_wincol;
    sv->leftcol = wp->w_leftcol;
    sv->p_wrap  = wp->w_p_wrap;
    sv->topline = wp->w_topline;
}

    స్థిర శూన్య
popup_geom_restore(win_T *wp, popup_geom_save_T *sv)
{
    wp->w_p_wrap  = sv->p_wrap;
    wp->w_leftcol = sv->leftcol;
    wp->w_wincol  = sv->wincol;
    wp->w_winrow  = sv->winrow;
    wp->w_topline = sv->topline;
    wp->w_వెడల్పు   = sv->వెడల్పు;
    wp->w_ఎత్తు  = sv->ఎత్తు;
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
    స్థిర శూన్య
popup_screenpos_above_శిఖరం(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	పూర్ణాంకం	    *screen_అడ్డువరుస,
	పూర్ణాంకం	    *screen_scol,
	పూర్ణాంకం	    *screen_ccol,
	పూర్ణాంకం	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    పూర్ణాంకం	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (పూర్ణాంకం)win_నిలువు_off(prop_win) - (పూర్ణాంకం)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (పూర్ణాంకం)scol + coloff;
    *screen_ccol = (పూర్ణాంకం)ccol + coloff;
    *screen_ecol = (పూర్ణాంకం)ecol + coloff;
    *screen_అడ్డువరుస = prop_win->w_winrow + 1
				 - (పూర్ణాంకం)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    స్థిర శూన్య
popup_hide_for_textprop(win_T *wp)
{
    ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	తిరిగి;
    wp->w_popup_flags |= POPF_HIDDEN;
    ఒకవేళ (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    స్థిర bool
popup_కనుగొను_prop_above_శిఖరం(
	win_T	    *wp,
	win_T	    *prop_win,
	పూర్ణాంకం	    max_వ్యాప్తి,
	textprop_T  *prop,
	linenr_T    *దొరికింది_lnum)
{
    linenr_T	మొదటి;

    ఒకవేళ (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	తిరిగి false;

    మొదటి = max_వ్యాప్తి > 0 ? prop_win->w_topline - max_వ్యాప్తి : 1;
    ఒకవేళ (మొదటి < 1)
	మొదటి = 1;
    తిరిగి కనుగొను_prop_లోపల_పంక్తులు(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, దొరికింది_lnum, మొదటి, prop_win->w_topline - 1);
}

/*
 * Compute and assign w_popup_topoff/bottomoff/leftclip/rightclip from the
 * host (textprop) window's content rectangle when POPF_CLIPWINDOW is set.
 * The popup's logical geometry (w_winrow, w_height, w_width) is preserved;
 * only the *off/clip fields record how much of each edge falls outside.
 * Returns true when the popup has scrolled completely past one of the host
 * edges, in which case the caller must hide it.
 */
    స్థిర bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    పూర్ణాంకం	    extra_h, extra_w;
    పూర్ణాంకం	    popup_శిఖరం, popup_bottom, popup_ఎడమ, popup_కుడి;
    పూర్ణాంకం	    మొత్తం_h, మొత్తం_w;

    ఒకవేళ (cw == ఖాళీ)
	తిరిగి false;

    extra_h = popup_శిఖరం_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_వెడల్పు(wp);

    popup_శిఖరం = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_ఎత్తు + extra_h;
    popup_ఎడమ = wp->w_wincol;
    popup_కుడి = wp->w_wincol + wp->w_వెడల్పు + extra_w;
    మొత్తం_h = wp->w_ఎత్తు + extra_h;
    మొత్తం_w = wp->w_వెడల్పు + extra_w;

    ఒకవేళ (popup_శిఖరం < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_శిఖరం;
    ఒకవేళ (popup_bottom > cw->w_winrow + cw->w_ఎత్తు)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_ఎత్తు);
    ఒకవేళ (popup_ఎడమ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_ఎడమ;
    ఒకవేళ (popup_కుడి > cw->w_wincol + cw->w_వెడల్పు)
	wp->w_popup_rightclip = popup_కుడి - (cw->w_wincol + cw->w_వెడల్పు);

    తిరిగి wp->w_popup_topoff >= మొత్తం_h
	|| wp->w_popup_bottomoff >= మొత్తం_h
	|| wp->w_popup_leftclip >= మొత్తం_w
	|| wp->w_popup_rightclip >= మొత్తం_w;
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
    స్థిర శూన్య
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    ఒకవేళ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_ఎత్తు -= cl->clip_శిఖరం_content + cl->clip_bot_content;
	ఒకవేళ (wp->w_ఎత్తు < 0)
	    wp->w_ఎత్తు = 0;
	ఒకవేళ (cl->clip_శిఖరం_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_శిఖరం_content;
    }
    ఒకవేళ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	ఒకవేళ (cl->clip_ఎడమ_content > 0 || cl->clip_కుడి_content > 0)
	    wp->w_p_wrap = FALSE;
	ఒకవేళ (cl->clip_కుడి_content > 0)
	{
	    wp->w_వెడల్పు -= cl->clip_కుడి_content;
	    ఒకవేళ (wp->w_వెడల్పు < 0)
		wp->w_వెడల్పు = 0;
	}
	ఒకవేళ (cl->clip_ఎడమ_content > 0)
	{
	    wp->w_leftcol += cl->clip_ఎడమ_content;
	    wp->w_wincol += cl->clip_ఎడమ_content;
	    wp->w_వెడల్పు -= cl->clip_ఎడమ_content;
	    ఒకవేళ (wp->w_వెడల్పు < 0)
		wp->w_వెడల్పు = 0;
	}
    }
}

#నిర్వచితమైతే FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    స్థిర శూన్య
popup_position_image(win_T *wp)
{
    linenr_T	    అడ్డువరుస;
    colnr_T	    నిలువు;

    పూర్ణాంకం iw, ih;

    // Visible cell width and height of popup itself (not image!)
    పూర్ణాంకం	visible_వెడల్పు;
    పూర్ణాంకం	visible_ఎత్తు;

    // Crop dimensions of image (in pixels).
    పూర్ణాంకం	crop_x, crop_y, crop_వెడల్పు, crop_ఎత్తు;

    popup_clip_T clip;

    ఒకవేళ (wp->w_popup_imagep == ఖాళీ || wp->w_popup_flags & POPF_HIDDEN)
	తిరిగి;

    image_placement_సమితి_enable(wp->w_popup_imagep, true);

    అడ్డువరుస = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    నిలువు = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    ఒకవేళ (wp->w_popup_imagep->img == ఖాళీ)
	// Just set the bounding box
	వెళ్ళు నిష్క్రమించు;

    visible_వెడల్పు =
	wp->w_వెడల్పు - clip.clip_ఎడమ_content - clip.clip_కుడి_content;
    visible_ఎత్తు =
	wp->w_ఎత్తు - clip.clip_శిఖరం_content - clip.clip_bot_content;

    ఒకవేళ (visible_వెడల్పు < 0 || visible_ఎత్తు < 0)
	తిరిగి;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_ఎడమ_content;
    crop_y = clip.clip_శిఖరం_content;
    crop_వెడల్పు = iw - crop_x - clip.clip_కుడి_content;
    crop_ఎత్తు = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    అడ్డువరుస -= wp->w_popup_border[0] - clip.eff_border[0];
    అడ్డువరుస -= wp->w_popup_padding[0] - clip.eff_padding[0];

    నిలువు -= wp->w_popup_border[3] - clip.eff_border[3];
    నిలువు -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    ఒకవేళ (crop_వెడల్పు > visible_వెడల్పు)
	crop_వెడల్పు = visible_వెడల్పు;
    ఒకవేళ (crop_ఎత్తు > visible_ఎత్తు)
	crop_ఎత్తు = visible_ఎత్తు;

    image_placement_సమితి_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_వెడల్పు, crop_ఎత్తు);
నిష్క్రమించు:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_సమితి_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_సమితి_position(wp->w_popup_imagep, అడ్డువరుస, నిలువు);
    image_placement_సమితి_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_ఎత్తు, clip.eff_వెడల్పు);
}
#అయితే_ముగింపు


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    స్థిర శూన్య
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    పూర్ణాంకం		wrapped = 0;
    పూర్ణాంకం		maxwidth;
    పూర్ణాంకం		maxwidth_no_scrollbar;
    పూర్ణాంకం		వెడల్పు_with_scrollbar = 0;
    పూర్ణాంకం		వాడిన_maxwidth = FALSE;
    పూర్ణాంకం		margin_వెడల్పు = 0;
    పూర్ణాంకం		maxspace;
    పూర్ణాంకం		center_vert = FALSE;
    పూర్ణాంకం		center_hor = FALSE;
    పూర్ణాంకం		allow_adjust_ఎడమ = !wp->w_popup_fixed;
    పూర్ణాంకం		శిఖరం_extra = popup_శిఖరం_extra(wp);
    పూర్ణాంకం		కుడి_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    పూర్ణాంకం		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    పూర్ణాంకం		ఎడమ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    పూర్ణాంకం		extra_ఎత్తు = శిఖరం_extra + bot_extra;
    పూర్ణాంకం		extra_వెడల్పు = ఎడమ_extra + కుడి_extra;
    పూర్ణాంకం		w_ఎత్తు_before_హద్దు;
    popup_layout_T org_layout;
    పూర్ణాంకం		minwidth, minheight;
    పూర్ణాంకం		maxheight = అడ్డువరుసలు;
    పూర్ణాంకం		wantline = wp->w_wantline;  // adjusted for textprop
    పూర్ణాంకం		wantcol = wp->w_wantcol;    // adjusted for textprop
    పూర్ణాంకం		use_wantcol = wantcol != 0;
    పూర్ణాంకం		adjust_ఎత్తు_for_శిఖరం_aligned = FALSE;

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
    ఒకవేళ (wp->w_popup_చివరి_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    ఒకవేళ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	పూర్ణాంకం	    screen_అడ్డువరుస;
	పూర్ణాంకం	    screen_scol;
	పూర్ణాంకం	    screen_ccol;
	పూర్ణాంకం	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_శిఖరం = false;
	ఒకవేళ (!కనుగొను_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    ఒకవేళ (popup_కనుగొను_prop_above_శిఖరం(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_శిఖరం = true;
	    లేకపోతే
	    {
		ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#నిర్వచితమైతే FEAT_IMAGE_POPUP
		    ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
			image_placement_clear(wp->w_popup_imagep);
#అయితే_ముగింపు
		    popup_hide_for_textprop(wp);
		    ఒకవేళ (wp->w_winrow + popup_ఎత్తు(wp) >= cmdline_అడ్డువరుస)
			clear_cmdline = TRUE;
		}
		తిరిగి;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.నిలువు = prop.tp_నిలువు;
	ఒకవేళ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.నిలువు += prop.tp_len - 1;
	ఒకవేళ (prop_above_శిఖరం)
	    popup_screenpos_above_శిఖరం(prop_win, &pos, prop_lnum, &screen_అడ్డువరుస,
				     &screen_scol, &screen_ccol, &screen_ecol);
	లేకపోతే
	    textpos2screenpos(prop_win, &pos, &screen_అడ్డువరుస,
				     &screen_scol, &screen_ccol, &screen_ecol);

	ఒకవేళ (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_వెడల్పు = 0;
	    తిరిగి;
	}
	ఒకవేళ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_అడ్డువరుస + wantline + 1;
	లేకపోతే
	    // above the text
	    wantline = screen_అడ్డువరుస + wantline - 1;
	center_vert = FALSE;
	ఒకవేళ (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	లేకపోతే
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    లేకపోతే
    {
	// If no line was specified default to vertical centering.
	ఒకవేళ (wantline == 0)
	    center_vert = TRUE;
	లేకపోతే ఒకవేళ (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	ఒకవేళ (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    ఒకవేళ (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    లేకపోతే
    {
	ఒకవేళ (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    ఒకవేళ (wp->w_winrow >= అడ్డువరుసలు)
		wp->w_winrow = అడ్డువరుసలు - 1;
	}
	ఒకవేళ (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_అడ్డువరుస
				    - wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - 1, 0);
	}

	ఒకవేళ (!use_wantcol)
	    center_hor = TRUE;
	లేకపోతే ఒకవేళ (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    ఒకవేళ (wp->w_wincol > firstwin->w_wincol + topframe->fr_వెడల్పు
						- ఎడమ_extra - కుడి_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_వెడల్పు
						- ఎడమ_extra - కుడి_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    ఒకవేళ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_వెడల్పు
					- wp->w_wincol - ఎడమ_extra - కుడి_extra;
    maxwidth = maxspace;
    ఒకవేళ (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_ఎడమ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    ఒకవేళ (wp->w_p_nu || wp->w_p_rnu)
	margin_వెడల్పు = సంఖ్య_వెడల్పు(wp) + 1;
#నిర్వచితమైతే FEAT_FOLDING
    margin_వెడల్పు += wp->w_p_fdc;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
    ఒకవేళ (signcolumn_on(wp))
	margin_వెడల్పు += 2;
#అయితే_ముగింపు
    ఒకవేళ (margin_వెడల్పు >= maxwidth)
	margin_వెడల్పు = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#నిర్వచితమైతే FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    ఒకవేళ (wp->w_బఫరు->b_పదం != ఖాళీ)
    {
	ఒకవేళ (minwidth == 0)
	    minwidth = 20;
	ఒకవేళ (minheight == 0)
	    minheight = 5;
    }
#అయితే_ముగింపు

    ఒకవేళ (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    లేకపోతే ఒకవేళ (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_అడ్డువరుస - 1;

    // start at the desired first line
    ఒకవేళ (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	ఒకవేళ (wp->w_firstline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    wp->w_firstline = 0;
	లేకపోతే
	    wp->w_topline = wp->w_firstline;
    }
    ఒకవేళ (wp->w_topline < 1)
	wp->w_topline = 1;
    లేకపోతే ఒకవేళ (wp->w_topline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	wp->w_topline = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_వెడల్పు = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	పూర్ణాంకం saved_w_వెడల్పు = wp->w_వెడల్పు;

	ఒకవేళ (wp->w_వెడల్పు < maxwidth)
	    wp->w_వెడల్పు = maxwidth;
	కొరకు (ln = 1; ln <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క; ++ln)
	{
	    పూర్ణాంకం len = linetabsize(wp, ln) + margin_వెడల్పు;

	    ఒకవేళ (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    ఒకవేళ (saved_w_వెడల్పు < len)
		saved_w_వెడల్పు = len;
	    ఒకవేళ (wp->w_maxwidth > 0 && saved_w_వెడల్పు >= wp->w_maxwidth)
		ఆపు;
	}
	wp->w_వెడల్పు = saved_w_వెడల్పు;
    }
    ఒకవేళ (wp->w_firstline < 0)
	lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
    లేకపోతే
	lnum = wp->w_topline;
    అయితే (lnum >= 1 && lnum <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	పూర్ణాంకం len;
	పూర్ణాంకం w_వెడల్పు = wp->w_వెడల్పు;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	ఒకవేళ (wp->w_వెడల్పు < maxwidth)
	    wp->w_వెడల్పు = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_వెడల్పు = w_వెడల్పు;

	ఒకవేళ (len + margin_వెడల్పు > maxwidth
		&& allow_adjust_ఎడమ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    పూర్ణాంకం జరుపు_by = len + margin_వెడల్పు - maxwidth;

	    ఒకవేళ (జరుపు_by > wp->w_wincol)
	    {
		పూర్ణాంకం truncate_జరుపు = జరుపు_by - wp->w_wincol;

		జరుపు_by -= truncate_జరుపు;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    ఒకవేళ (wp->w_maxwidth > 0 && maxwidth + జరుపు_by > wp->w_maxwidth)
		జరుపు_by = wp->w_maxwidth - maxwidth;

	    ఒకవేళ (జరుపు_by > 0)
	    {
		wp->w_wincol -= జరుపు_by;
		maxwidth += జరుపు_by;
	    }
	    wp->w_వెడల్పు = maxwidth;
	}
	ఒకవేళ (wp->w_p_wrap)
	{
	    అయితే (len + margin_వెడల్పు > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_వెడల్పు;
		wp->w_వెడల్పు = maxwidth;
		వాడిన_maxwidth = TRUE;
	    }
	}
	ఒకవేళ (wp->w_వెడల్పు < len + margin_వెడల్పు)
	{
	    wp->w_వెడల్పు = len + margin_వెడల్పు;
	    ఒకవేళ (wp->w_maxwidth > 0 && wp->w_వెడల్పు > wp->w_maxwidth)
		wp->w_వెడల్పు = wp->w_maxwidth;
	}

	ఒకవేళ (wp->w_firstline < 0)
	    --lnum;
	లేకపోతే
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	ఒకవేళ (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - lnum)
		       + wrapped >= maxheight)
	    ఆపు;
    }

    ఒకవేళ (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క);
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (wp->w_బఫరు->b_పదం != ఖాళీ && !పదం_is_ముగిసిన(wp->w_బఫరు))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#అయితే_ముగింపు
    maxwidth_no_scrollbar = maxwidth;
    ఒకవేళ (wp->w_has_scrollbar)
    {
	++కుడి_extra;
	++extra_వెడల్పు;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	ఒకవేళ (maxwidth + కుడి_extra >= maxspace
		&& (వాడిన_maxwidth || (minwidth > 0 && wp->w_వెడల్పు < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    ఒకవేళ (wp->w_popup_title != ఖాళీ && *wp->w_popup_title != NUL)
    {
	పూర్ణాంకం title_len = vim_strsize(wp->w_popup_title) + 2 - extra_వెడల్పు;

	ఒకవేళ (minwidth < title_len)
	    minwidth = title_len;
    }

    ఒకవేళ (minwidth > 0 && wp->w_వెడల్పు < minwidth)
	wp->w_వెడల్పు = minwidth;
    ఒకవేళ (wp->w_వెడల్పు > maxwidth)
    {
	ఒకవేళ (wp->w_వెడల్పు > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_వెడల్పు - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	ఒకవేళ (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    పూర్ణాంకం off = wp->w_వెడల్పు - maxwidth;
	    extra_వెడల్పు -= MIN(off, కుడి_extra);
	    wp->w_వెడల్పు = maxwidth_no_scrollbar;
	}
	లేకపోతే
	{
	    wp->w_వెడల్పు = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    వెడల్పు_with_scrollbar = maxwidth_no_scrollbar - కుడి_extra;
	}
    }
    ఒకవేళ (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_వెడల్పు - wp->w_వెడల్పు - extra_వెడల్పు) / 2;
	ఒకవేళ (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    లేకపోతే ఒకవేళ (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	పూర్ణాంకం leftoff = wantcol - (wp->w_వెడల్పు + extra_వెడల్పు);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	ఒకవేళ (leftoff >= 0)
	    wp->w_wincol = leftoff;
	లేకపోతే ఒకవేళ (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_వెడల్పు -= wp->w_leftcol;
	    ఒకవేళ (wp->w_వెడల్పు < 0)
		wp->w_వెడల్పు = 0;
	}
    }

    ఒకవేళ (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	పూర్ణాంకం want_నిలువు = 0;

	// try to show the right border and any scrollbar
	want_నిలువు = ఎడమ_extra + wp->w_వెడల్పు + కుడి_extra;
	ఒకవేళ (want_నిలువు > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_నిలువు >= firstwin->w_wincol + topframe->fr_వెడల్పు)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_వెడల్పు - want_నిలువు;
	    ఒకవేళ (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_ఎత్తు = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - wp->w_topline
								 + 1 + wrapped;
    ఒకవేళ (minheight > 0 && wp->w_ఎత్తు < minheight)
	wp->w_ఎత్తు = minheight;
    ఒకవేళ (maxheight > 0 && wp->w_ఎత్తు > maxheight)
	wp->w_ఎత్తు = maxheight;
    w_ఎత్తు_before_హద్దు = wp->w_ఎత్తు;
    ఒకవేళ (wp->w_ఎత్తు > అడ్డువరుసలు - wp->w_winrow)
	wp->w_ఎత్తు = అడ్డువరుసలు - wp->w_winrow;

    ఒకవేళ (center_vert)
    {
	wp->w_winrow = (అడ్డువరుసలు - wp->w_ఎత్తు - extra_ఎత్తు) / 2;
	ఒకవేళ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    లేకపోతే ఒకవేళ (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	ఒకవేళ ((wp->w_ఎత్తు + extra_ఎత్తు) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_ఎత్తు + extra_ఎత్తు);
	లేకపోతే ఒకవేళ (wantline * 2 >= అడ్డువరుసలు || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_ఎత్తు = wantline - extra_ఎత్తు;
	}
	లేకపోతే
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_ఎత్తు_for_శిఖరం_aligned = TRUE;
	}
    }
    లేకపోతే ఒకవేళ (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	పూర్ణాంకం check_ఎత్తు = (wp->w_popup_flags & POPF_INFO) ? wp->w_ఎత్తు
						    : w_ఎత్తు_before_హద్దు;
	ఒకవేళ (wp != popup_dragwin
		&& wantline + (check_ఎత్తు + extra_ఎత్తు) - 1 > అడ్డువరుసలు
		&& wantline * 2 > అడ్డువరుసలు
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_ఎత్తు = w_ఎత్తు_before_హద్దు;
	    wp->w_winrow = wantline - 2 - wp->w_ఎత్తు - extra_ఎత్తు;
	    ఒకవేళ (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_ఎత్తు + 2;
	    ఒకవేళ (wp->w_winrow < 0)
	    {
		wp->w_ఎత్తు += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	లేకపోతే
	{
	    wp->w_winrow = wantline - 1;
	    adjust_ఎత్తు_for_శిఖరం_aligned = TRUE;
	}
    }

    ఒకవేళ (adjust_ఎత్తు_for_శిఖరం_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_ఎత్తు + extra_ఎత్తు > అడ్డువరుసలు)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_ఎత్తు = అడ్డువరుసలు - wp->w_winrow - extra_ఎత్తు;
#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (wp->w_బఫరు->b_పదం == ఖాళీ || పదం_is_ముగిసిన(wp->w_బఫరు))
#అయితే_ముగింపు
	{
	    // The width already accounts for a scrollbar that was there.
	    ఒకవేళ (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		ఒకవేళ (వెడల్పు_with_scrollbar > 0)
		    wp->w_వెడల్పు = వెడల్పు_with_scrollbar;
		లేకపోతే
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_వెడల్పు;
	    }
	}
    }

    // make sure w_winrow is valid
    ఒకవేళ (wp->w_winrow >= అడ్డువరుసలు)
	wp->w_winrow = అడ్డువరుసలు - 1;
    లేకపోతే ఒకవేళ (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    ఒకవేళ (wp->w_wincol + wp->w_వెడల్పు + extra_వెడల్పు
				    > firstwin->w_wincol + topframe->fr_వెడల్పు)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_వెడల్పు
						- wp->w_వెడల్పు - extra_వెడల్పు;
    ఒకవేళ (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    ఒకవేళ (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    ఒకవేళ (wp->w_wincol + wp->w_వెడల్పు + extra_వెడల్పు
				    > firstwin->w_wincol + topframe->fr_వెడల్పు)
    {
	పూర్ణాంకం avail = firstwin->w_wincol + topframe->fr_వెడల్పు
						- wp->w_wincol - extra_వెడల్పు;
	wp->w_వెడల్పు = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    ఒకవేళ (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	ఒకవేళ (wp->w_winrow + wp->w_ఎత్తు + extra_ఎత్తు > అడ్డువరుసలు)
	    wp->w_winrow = అడ్డువరుసలు - wp->w_ఎత్తు - extra_ఎత్తు;
	ఒకవేళ (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	ఒకవేళ (wp->w_winrow + wp->w_ఎత్తు + extra_ఎత్తు > అడ్డువరుసలు)
	{
	    పూర్ణాంకం avail = అడ్డువరుసలు - wp->w_winrow - extra_ఎత్తు;
	    wp->w_ఎత్తు = avail > 0 ? avail : 0;
	}
    }

    ఒకవేళ (wp->w_ఎత్తు != org_layout.ఎత్తు)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    ఒకవేళ (popup_compute_clipwindow_offsets(wp))
    {
#నిర్వచితమైతే FEAT_IMAGE_POPUP
	ఒకవేళ (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
		image_placement_clear(wp->w_popup_imagep);
#అయితే_ముగింపు

	popup_hide_for_textprop(wp);
	తిరిగి;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_చివరి_changedtick = CHANGEDTICK(wp->w_బఫరు);
    ఒకవేళ (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_బఫరు);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_వెడల్పు = wp->w_popup_prop_win->w_వెడల్పు;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_ఎత్తు;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    ఒకవేళ (popup_layout_మారింది(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	ఒకవేళ (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_ముసుగు_refresh = TRUE;
    }

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    popup_position_image(wp);
#అయితే_ముగింపు
}

రకనిర్వచనం గణన
{
    TYPE_సాధారణ,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_సందేశం_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    స్థిర పూర్ణాంకం
popup_is_notification(create_type_T type)
{
    తిరిగి type == TYPE_NOTIFICATION || type == TYPE_సందేశం_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    స్థిర శూన్య
popup_సమితి_బఫరు_పాఠ్యం(buf_T *buf, typval_T పాఠ్యం)
{
    పూర్ణాంకం	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    కొరకు (lnum = buf->b_ml.ml_పంక్తి_లెక్క; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_బఫరు;

    // Add text to the buffer.
    ఒకవేళ (పాఠ్యం.v_type == VAR_STRING)
    {
	char_u *s = పాఠ్యం.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == ఖాళీ ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    లేకపోతే
    {
	list_T *l = పాఠ్యం.vval.v_list;

	ఒకవేళ (l != ఖాళీ && l->lv_len > 0)
	{
	    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
		emsg(_(e_using_సంఖ్య_as_string));
	    లేకపోతే ఒకవేళ (l->lv_మొదటి->li_tv.v_type == VAR_STRING)
		// list of strings
		కూడు_popup_అక్షరశ్రేణులు(buf, l);
	    లేకపోతే
		// list of dictionaries
		కూడు_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_పంక్తి_లెక్క);
    curbuf = curwin->w_బఫరు;
}

#నిర్వచించు సమితి_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    చేయి {						    \
	ఒకవేళ (wp != ఖాళీ)					    \
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
    } అయితే (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    స్థిర పూర్ణాంకం
parse_popup_option(win_T *wp, పూర్ణాంకం is_preview)
{
    char_u *p =
#నిర్వచితమైతే FEAT_QUICKFIX
	!is_preview ? p_cpp :
#అయితే_ముగింపు
	p_pvp;
    పూర్ణాంకం	    border_enabled = FALSE;

    ఒకవేళ (wp != ఖాళీ)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    కొరకు ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	పూర్ణాంకం	x;

	e = vim_strchr(p, ':');
	ఒకవేళ (e == ఖాళీ || e[1] == NUL)
	    తిరిగి FAIL;

	p = vim_strchr(e, ',');
	ఒకవేళ (p == ఖాళీ)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	ఒకవేళ (STRNCMP(s, "height:", 7) == 0)
	{
	    ఒకవేళ (dig != p)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		ఒకవేళ (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "width:", 6) == 0)
	{
	    ఒకవేళ (dig != p)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		ఒకవేళ (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "highlight:", 10) == 0)
	{
	    ఒకవేళ (wp != ఖాళీ)
	    {
		అక్షరం *errmsg;
		పూర్ణాంకం c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		ఒకవేళ (errmsg == ఖాళీ)
		    సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		లేకపోతే
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    ఒకవేళ (*arg == NUL || *arg == ',')
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		కొరకు (పూర్ణాంకం i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    పూర్ణాంకం		i;
	    పూర్ణాంకం		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    పూర్ణాంకం		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    ఒకవేళ (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		ఒకవేళ (wp != ఖాళీ)
		{
		    కొరకు (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    సమితి_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		కొనసాగు;
	    }

	    token = vim_strnsave(arg, token_len);
	    ఒకవేళ (token == ఖాళీ)
		తిరిగి FAIL;

	    ఒకవేళ ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		ఒకవేళ (STRCMP(token, "single") == 0)
		    సమితి_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		లేకపోతే ఒకవేళ (STRCMP(token, "double") == 0)
		    సమితి_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		లేకపోతే ఒకవేళ (STRCMP(token, "round") == 0)
		    సమితి_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		లేకపోతే ఒకవేళ (STRCMP(token, "on") == 0)
		    సమితి_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		లేకపోతే ఒకవేళ (STRCMP(token, "ascii") == 0)
		    సమితి_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		లేకపోతే ఒకవేళ (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    పూర్ణాంకం		బహిర్గతం[8];
		    పూర్ణాంకం		failed = FALSE;

		    సమితి_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    కొరకు (i = 0; i < 8 && !failed; i++)
		    {
			ఒకవేళ (*q == NUL)
			    failed = TRUE;
			లేకపోతే
			{
			    బహిర్గతం[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    ఒకవేళ (i < 7)
			    {
				ఒకవేళ (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    ఒకవేళ (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			తిరిగి FAIL;
		    }
		    సమితి_BORDER_CHARS(బహిర్గతం[0], బహిర్గతం[1], బహిర్గతం[2], బహిర్గతం[3], బహిర్గతం[4],
			    బహిర్గతం[5], బహిర్గతం[6], బహిర్గతం[7]);
		}
	    }
	    లేకపోతే
	    {
		vim_free(token);
		తిరిగి FAIL;
	    }

	    ఒకవేళ (wp != ఖాళీ)
	    {
		కొరకు (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    పూర్ణాంకం		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    పూర్ణాంకం		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ఒకవేళ (!on && !off)
		తిరిగి FAIL;
	    on = on && mouse_has(MOUSE_చొప్పించు) && (border_enabled || is_preview);
	    ఒకవేళ (wp != ఖాళీ)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    పూర్ణాంకం		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    పూర్ణాంకం		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ఒకవేళ (!on && !off)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		ఒకవేళ (on && mouse_has(MOUSE_చొప్పించు))
		    wp->w_popup_flags |= POPF_RESIZE;
		లేకపోతే
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    పూర్ణాంకం		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    పూర్ణాంకం		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    ఒకవేళ (!on && !off)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    పూర్ణాంకం		అంశం = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    పూర్ణాంకం		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    ఒకవేళ (!menu && !అంశం)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	లేకపోతే ఒకవేళ (STRNCMP(s, "opacity:", 8) == 0)
	{
	    ఒకవేళ (dig != p || x < 0 || x > 100)
		తిరిగి FAIL;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		ఒకవేళ (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		లేకపోతే
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	లేకపోతే
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    పూర్ణాంకం
parse_previewpopup(win_T *wp)
{
    తిరిగి parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    పూర్ణాంకం
parse_completepopup(win_T *wp)
{
    తిరిగి parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    శూన్య
popup_సమితి_wantpos_cursor(win_T *wp, పూర్ణాంకం వెడల్పు, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    ఒకవేళ (d != ఖాళీ)
	ppt = get_pos_నమోదు(d, FALSE);

    setcursor_mayforce(TRUE);
    ఒకవేళ (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    లేకపోతే
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	ఒకవేళ (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    ఒకవేళ (wp->w_wantcol > Columns - వెడల్పు)
    {
	wp->w_wantcol = Columns - వెడల్పు;
	ఒకవేళ (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    శూన్య
popup_సమితి_wantpos_rowcol(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    wp->w_wantline = అడ్డువరుస;
    wp->w_wantcol = నిలువు;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    స్థిర శూన్య
కూడు_border_ఎడమ_కుడి_padding(win_T *wp)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#నిర్వచితమైతే FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    స్థిర పూర్ణాంకం
popup_terminal_exists(శూన్య)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_బఫరు->b_పదం != ఖాళీ)
	    తిరిగి TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	    ఒకవేళ (wp->w_బఫరు->b_పదం != ఖాళీ)
		తిరిగి TRUE;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    శూన్య
popup_redraw_all(శూన్య)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    పూర్ణాంకం
popup_need_redraw(శూన్య)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    తిరిగి TRUE;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Set the color for a notification window.
 */
    స్థిర శూన్య
popup_update_color(win_T *wp, create_type_T type)
{
    అక్షరం    *hiname = type == TYPE_సందేశం_WIN
				       ? "MessageWindow" : "PopupNotification";
    అక్షరం    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    ఒకవేళ (errmsg == ఖాళీ)
	సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    లేకపోతే
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    స్థిర win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = ఖాళీ;
    పూర్ణాంకం		tabnr = 0;
    పూర్ణాంకం		new_బఫరు;
    buf_T	*buf = ఖాళీ;
    dict_T	*d = ఖాళీ;
    పూర్ణాంకం		i;

    ఒకవేళ (check_secure())
	తిరిగి ఖాళీ;

    ఒకవేళ (argvars != ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script()
		&& (check_for_string_or_సంఖ్య_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    తిరిగి ఖాళీ;

	// Check that arguments look OK.
	ఒకవేళ (argvars[0].v_type == VAR_సంఖ్య)
	{
	    buf = buflist_findnr(argvars[0].vval.v_సంఖ్య);
	    ఒకవేళ (buf == ఖాళీ)
	    {
		semsg(_(e_బఫరు_nr_does_not_exist), argvars[0].vval.v_సంఖ్య);
		తిరిగి ఖాళీ;
	    }
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (buf->b_పదం != ఖాళీ && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_రెండవ_popup_with_terminal));
		తిరిగి ఖాళీ;
	    }
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != ఖాళీ)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != ఖాళీ))
	{
	    emsg(_(e_బఫరు_సంఖ్య_పాఠ్యం_or_list_required));
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    తిరిగి ఖాళీ;
	d = argvars[1].vval.v_dict;
    }

    ఒకవేళ (d != ఖాళీ)
    {
	ఒకవేళ (dict_has_కీలకం(d, "tabpage"))
	    tabnr = (పూర్ణాంకం)dict_get_సంఖ్య(d, "tabpage");
	లేకపోతే ఒకవేళ (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	లేకపోతే
	    tabnr = 0;
	ఒకవేళ (tabnr > 0)
	{
	    tp = కనుగొను_tabpage(tabnr);
	    ఒకవేళ (tp == ఖాళీ)
	    {
		semsg(_(e_tabpage_not_దొరికింది_nr), tabnr);
		తిరిగి ఖాళీ;
	    }
	}
    }
    లేకపోతే ఒకవేళ (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    ఒకవేళ (buf != ఖాళీ && buf->b_locked_విభజించు)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_కిటికీ_గమ్యం_a_closing_బఫరు));
	తిరిగి ఖాళీ;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (rettv != ఖాళీ)
	rettv->vval.v_సంఖ్య = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    ఒకవేళ (buf != ఖాళీ)
    {
	// use existing buffer
	new_బఫరు = FALSE;
	win_init_popup_win(wp, buf);
	సమితి_local_options_default(wp, FALSE);
	మార్పిడి_exists_action = SEA_READONLY;
	బఫరు_ensure_loaded(buf);
	మార్పిడి_exists_action = SEA_NONE;
    }
    లేకపోతే
    {
	// create a new buffer associated with the popup
	new_బఫరు = TRUE;
	buf = buflist_new(ఖాళీ, ఖాళీ, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	ఒకవేళ (buf == ఖాళీ)
	{
	    win_free_popup(wp);
	    తిరిగి ఖాళీ;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	సమితి_local_options_default(wp, TRUE);
	సమితి_string_option_ప్రత్యక్ష_లోపల_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	సమితి_string_option_ప్రత్యక్ష_లోపల_buf(buf, (char_u *)"bufhidden", -1,
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

    ఒకవేళ (tp != ఖాళీ)
    {
	// popup on specified tab page
	wp->w_తదుపరి = tp->tp_మొదటి_popupwin;
	tp->tp_మొదటి_popupwin = wp;
    }
    లేకపోతే ఒకవేళ (tabnr == 0)
    {
	// popup on current tab page
	wp->w_తదుపరి = curtab->tp_మొదటి_popupwin;
	curtab->tp_మొదటి_popupwin = wp;
    }
    లేకపోతే // (tabnr < 0)
    {
	win_T *prev = మొదటి_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	ఒకవేళ (మొదటి_popupwin == ఖాళీ)
	    మొదటి_popupwin = wp;
	లేకపోతే
	{
	    అయితే (prev->w_తదుపరి != ఖాళీ)
		prev = prev->w_తదుపరి;
	    prev->w_తదుపరి = wp;
	}
#నిర్వచితమైతే FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#అయితే_ముగింపు
    }

    ఒకవేళ (new_బఫరు && argvars != ఖాళీ)
	popup_సమితి_బఫరు_పాఠ్యం(buf, argvars[0]);

    ఒకవేళ (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    ఒకవేళ (type == TYPE_ATCURSOR)
    {
	popup_సమితి_wantpos_cursor(wp, 0, d);
	సమితి_moved_విలువలు(wp);
	సమితి_moved_columns(wp, కనుగొను_STRING);
    }

    ఒకవేళ (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_అడ్డువరుస;
	ఒకవేళ (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_నిలువు + 1;
	సమితి_mousemoved_విలువలు(wp);
	సమితి_mousemoved_columns(wp, కనుగొను_IDENT + కనుగొను_STRING + కనుగొను_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    ఒకవేళ (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	పూర్ణాంకం	ఎత్తు = buf->b_ml.ml_పంక్తి_లెక్క + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	కొరకు (twp = మొదటి_popupwin; twp != ఖాళీ; twp = nextwin)
	{
	    nextwin = twp->w_తదుపరి;
	    ఒకవేళ (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + ఎత్తు
		    && twp->w_winrow + popup_ఎత్తు(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_ఎత్తు(twp) + 1;
		nextwin = మొదటి_popupwin;
	    }
	}
	ఒకవేళ (wp->w_wantline + ఎత్తు > అడ్డువరుసలు)
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
	కొరకు (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    ఒకవేళ (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	కూడు_border_ఎడమ_కుడి_padding(wp);
    }

    ఒకవేళ (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	ఒకవేళ (callback.cb_name != ఖాళీ)
	{
	    సమితి_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    కొరకు (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    కొరకు (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    ఒకవేళ (type == TYPE_PREVIEW)
    {
	ఒకవేళ (mouse_has(MOUSE_చొప్పించు))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	కొరకు (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_సమితి_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#నిర్వచితమైతే FEAT_QUICKFIX
    ఒకవేళ (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	ఒకవేళ (mouse_has(MOUSE_చొప్పించు))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	కూడు_border_ఎడమ_కుడి_padding(wp);
	parse_completepopup(wp);
    }
#అయితే_ముగింపు

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_బహుళకం = బహుళకం_ALL;

    ఒకవేళ (d != ఖాళీ)
    {
	// Deal with options.
	ఒకవేళ (apply_options(wp, d, TRUE) == FAIL)
	{
	    (శూన్య)popup_close(wp->w_id, FALSE);
	    తిరిగి ఖాళీ;
	}
    }

#నిర్వచితమైతే FEAT_TIMERS
    ఒకవేళ (popup_is_notification(type) && wp->w_popup_timer == ఖాళీ)
	popup_కూడు_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#అయితే_ముగింపు

    popup_adjust_position(wp);

    wp->w_vsep_వెడల్పు = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_ముసుగు_refresh = TRUE;

#నిర్వచితమైతే FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    ఒకవేళ (buf->b_పదం != ఖాళీ)
	win_enter(wp, FALSE);
#అయితే_ముగింపు

    తిరిగి wp;
}

/*
 * popup_clear()
 */
    శూన్య
f_popup_clear(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం force = FALSE;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	force = (పూర్ణాంకం)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    శూన్య
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_సాధారణ);
}

/*
 * popup_atcursor({text}, {options})
 */
    శూన్య
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    శూన్య
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    స్థిర శూన్య
invoke_popup_callback(win_T *wp, typval_T *ఫలితం)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = (varnumber_T)wp->w_id;

    ఒకవేళ (ఫలితం != ఖాళీ && ఫలితం->v_type != VAR_UNKNOWN)
	copy_tv(ఫలితం, &argv[1]);
    లేకపోతే
    {
	argv[1].v_type = VAR_సంఖ్య;
	argv[1].vval.v_సంఖ్య = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    ఒకవేళ (ఫలితం != ఖాళీ)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    స్థిర శూన్య
వెనుక_గమ్యం_prevwin(win_T *wp)
{
    ఒకవేళ (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    లేకపోతే
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    స్థిర శూన్య
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    పూర్ణాంకం id;

    ఒకవేళ (!win_valid(wp))
       తిరిగి;

    id = wp->w_id;

#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (wp == curwin && curbuf->b_పదం != ఖాళీ)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    ఒకవేళ (owp != curwin && owp->w_బఫరు->b_పదం != ఖాళీ)
		ఆపు;
	ఒకవేళ (owp != ఖాళీ)
	    win_enter(owp, FALSE);
	లేకపోతే
	{
	    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, owp)
		ఒకవేళ (owp != curwin && owp->w_బఫరు->b_పదం != ఖాళీ)
		    ఆపు;
	    ఒకవేళ (owp != ఖాళీ)
		win_enter(owp, FALSE);
	    లేకపోతే
		వెనుక_గమ్యం_prevwin(wp);
	}
    }
#అయితే_ముగింపు

    // Just in case a check higher up is missing.
    ఒకవేళ (wp == curwin && ERROR_IF_POPUP_కిటికీ)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	తిరిగి;
    }

    CHECK_CURBUF;
    ఒకవేళ (wp->w_close_cb.cb_name != ఖాళీ)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    శూన్య
popup_close_with_retval(win_T *wp, పూర్ణాంకం retval)
{
    typval_T res;

    res.v_type = VAR_సంఖ్య;
    res.vval.v_సంఖ్య = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    శూన్య
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    స్థిర శూన్య
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    ఒకవేళ (wp != mouse_wp
	    && wp->w_popup_mouse_అడ్డువరుస != 0
	    && (wp->w_popup_mouse_అడ్డువరుస != mouse_అడ్డువరుస
		|| mouse_నిలువు < wp->w_popup_mouse_mincol
		|| mouse_నిలువు > wp->w_popup_mouse_maxcol))
    {
	// Careful: this makes "wp" invalid.
	popup_close_with_retval(wp, -2);
    }
}

/*
 * Called when the mouse moved: may close a popup with "mousemoved".
 */
    శూన్య
popup_handle_mouse_moved(శూన్య)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    పూర్ణాంకం	    అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం	    నిలువు = mouse_నిలువు;

    // find the window where the mouse is in
    mouse_wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);

    కొరకు (wp = మొదటి_popupwin; wp != ఖాళీ; wp = nextwp)
    {
	nextwp = wp->w_తదుపరి;
	check_mouse_moved(wp, mouse_wp);
    }
    కొరకు (wp = curtab->tp_మొదటి_popupwin; wp != ఖాళీ; wp = nextwp)
    {
	nextwp = wp->w_తదుపరి;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    స్థిర శూన్య
filter_handle_drag(win_T *wp, పూర్ణాంకం c, typval_T *rettv)
{
    పూర్ణాంకం	అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం	నిలువు = mouse_నిలువు;

    ఒకవేళ ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_కీలకం(c)
	    && (wp == popup_dragwin
			  || wp == mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_సంఖ్య = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    శూన్య
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		id;
    win_T	*wp;
    char_u	*కీలకం;
    typval_T	res;
    పూర్ణాంకం		c;
    linenr_T	old_lnum;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    id = tv_get_సంఖ్య(&argvars[0]);
    wp = win_id2wp(id);
    కీలకం = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    c = *కీలకం;
    ఒకవేళ (c == K_SPECIAL && కీలకం[1] != NUL)
	c = గమ్యం_SPECIAL(కీలకం[1], కీలకం[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_TRUE;
    res.v_type = VAR_సంఖ్య;

    old_lnum = wp->w_cursor.lnum;
    ఒకవేళ (c == 'k' || c == 'K' || c == K_పైకి || c == Ctrl_P)
    {
	ఒకవేళ (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	లేకపోతే
	    wp->w_cursor.lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
    }
    ఒకవేళ (c == 'j' || c == 'J' || c == K_కిందికి || c == Ctrl_N)
    {
	ఒకవేళ (wp->w_cursor.lnum < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    ++wp->w_cursor.lnum;
	లేకపోతే
	    wp->w_cursor.lnum = 1;
    }
    ఒకవేళ (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	తిరిగి;
    }

    ఒకవేళ (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_సంఖ్య = -1;
	popup_close_and_callback(wp, &res);
	తిరిగి;
    }
    ఒకవేళ (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_సంఖ్య = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	తిరిగి;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    శూన్య
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		id;
    win_T	*wp;
    char_u	*కీలకం;
    typval_T	res;
    పూర్ణాంకం		c;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    id = tv_get_సంఖ్య(&argvars[0]);
    wp = win_id2wp(id);
    కీలకం = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    c = *కీలకం;
    ఒకవేళ (c == CAR && need_wait_return)
	తిరిగి;
    ఒకవేళ (c == K_SPECIAL && కీలకం[1] != NUL)
	c = గమ్యం_SPECIAL(కీలకం[1], కీలకం[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_TRUE;

    ఒకవేళ (c == 'y' || c == 'Y')
	res.vval.v_సంఖ్య = 1;
    లేకపోతే ఒకవేళ (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_సంఖ్య = 0;
    లేకపోతే
    {
	filter_handle_drag(wp, c, rettv);
	తిరిగి;
    }

    // Invoke callback
    res.v_type = VAR_సంఖ్య;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    శూన్య
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    శూన్య
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    శూన్య
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    స్థిర win_T *
కనుగొను_popup_win(పూర్ణాంకం id)
{
    win_T *wp = win_id2wp(id);

    ఒకవేళ (wp != ఖాళీ && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_కిటికీ_nr_is_not_popup_కిటికీ), id);
	తిరిగి ఖాళీ;
    }
    తిరిగి wp;
}

/*
 * popup_close({id})
 */
    శూన్య
f_popup_close(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం		id;
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    ఒకవేళ (
#నిర్వచితమైతే FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_పదం == ఖాళీ &&
#అయితే_ముగింపు
	    ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp != ఖాళీ)
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
    స్థిర శూన్య
popup_clear_ముసుగు_for(win_T *wp)
{
    పూర్ణాంకం r, c;
    పూర్ణాంకం అడ్డువరుస_ప్రారంభం, నిలువు_ప్రారంభం, అడ్డువరుస_చివర, నిలువు_చివర;

    ఒకవేళ (popup_ముసుగు == ఖాళీ || !popup_visible)
	తిరిగి;

    అడ్డువరుస_ప్రారంభం = MAX(wp->w_winrow, 0);
    నిలువు_ప్రారంభం = MAX(wp->w_wincol, 0);
    అడ్డువరుస_చివర = MIN(wp->w_winrow + popup_ఎత్తు(wp), (పూర్ణాంకం)screen_అడ్డువరుసలు);
    నిలువు_చివర = MIN(wp->w_wincol + popup_వెడల్పు(wp), (పూర్ణాంకం)screen_Columns);

    కొరకు (r = అడ్డువరుస_ప్రారంభం; r < అడ్డువరుస_చివర; ++r)
	కొరకు (c = నిలువు_ప్రారంభం; c < నిలువు_చివర; ++c)
	    popup_ముసుగు[r * screen_Columns + c] = 0;
}

    శూన్య
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    పూర్ణాంకం			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (error_if_పదం_popup_కిటికీ())
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	తిరిగి;

    popup_save_area(wp, &old_area);

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	image_placement_clear(wp->w_popup_imagep);
#అయితే_ముగింపు

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    ఒకవేళ (wp->w_winrow + popup_ఎత్తు(wp) >= cmdline_అడ్డువరుస)
	clear_cmdline = TRUE;

    ఒకవేళ (was_visible)
	popup_clear_ముసుగు_for(wp);

    ఒకవేళ (old_area.చురుకైన)
	popup_redraw_exposed_area(&old_area);
    లేకపోతే
	redraw_all_later(UPD_NOT_VALID);

    స్థితి_redraw_all();
    ఒకవేళ (!old_area.చురుకైన)
	popup_ముసుగు_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    శూన్య
f_popup_hide(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం		id;
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    శూన్య
popup_show(win_T *wp)
{
    bool popup_చురుకైన;

    ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	తిరిగి;

    popup_చురుకైన = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    ఒకవేళ (popup_చురుకైన)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_పంక్తులు_valid = 0;
	ఒకవేళ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    లేకపోతే
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_ముసుగు_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    శూన్య
f_popup_show(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం		id;
    win_T	*wp;

    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#నిర్వచితమైతే FEAT_QUICKFIX
    ఒకవేళ (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#అయితే_ముగింపు

    rettv->vval.v_సంఖ్య = 0;
}

/*
 * popup_settext({id}, {text})
 */
    శూన్య
f_popup_settext(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం		id;
    win_T	*wp;
    popup_area_T	old_area;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    popup_save_area(wp, &old_area);

    ఒకవేళ (check_for_string_or_list_arg(argvars, 1) == FAIL)
	తిరిగి;

    popup_సమితి_బఫరు_పాఠ్యం(wp->w_బఫరు, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_పంక్తులు_valid = 0;
    ఒకవేళ (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    ఒకవేళ (popup_area_మారింది(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    శూన్య
f_popup_setbuf(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_FALSE;

    ఒకవేళ (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_బఫరు_arg(argvars, 1) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    buf = tv_get_buf_from_arg(&argvars[1]);

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (buf->b_పదం != ఖాళీ && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_రెండవ_popup_with_terminal));
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (wp->w_బఫరు != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_బఫరు->b_nwindows--;
	win_init_popup_win(wp, buf);
	సమితి_local_options_default(wp, FALSE);
	మార్పిడి_exists_action = SEA_READONLY;
	బఫరు_ensure_loaded(buf);
	మార్పిడి_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	ఒకవేళ (popup_area_మారింది(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_సంఖ్య = VVAL_TRUE;
}

    స్థిర శూన్య
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    పూర్ణాంకం			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    ఒకవేళ (wp->w_popup_imagep != ఖాళీ)
	image_placement_free(wp->w_popup_imagep);
#అయితే_ముగింపు

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_బఫరు->b_locked = FALSE;
    ఒకవేళ (wp->w_winrow + popup_ఎత్తు(wp) >= cmdline_అడ్డువరుస)
	clear_cmdline = TRUE;

    ఒకవేళ (was_visible)
	popup_clear_ముసుగు_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    ఒకవేళ (wp == సందేశం_win_2)
	సందేశం_win_2 = ఖాళీ;
#అయితే_ముగింపు

    ఒకవేళ (!old_area.చురుకైన)
	redraw_all_later(UPD_NOT_VALID);
    స్థితి_redraw_all();
    ఒకవేళ (!old_area.చురుకైన)
	popup_ముసుగు_refresh = TRUE;
}

    స్థిర శూన్య
error_for_popup_కిటికీ(శూన్య)
{
    emsg(_(e_not_allowed_లోపల_popup_కిటికీ));
}

    పూర్ణాంకం
error_if_popup_కిటికీ(పూర్ణాంకం also_with_పదం వాడని)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    ఒకవేళ (WIN_IS_POPUP(curwin)
#నిర్వచితమైతే FEAT_TERMINAL
	    && (also_with_పదం || curbuf->b_పదం == ఖాళీ)
#అయితే_ముగింపు
	    )
    {
	error_for_popup_కిటికీ();
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    పూర్ణాంకం
popup_close(పూర్ణాంకం id, పూర్ణాంకం force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = ఖాళీ;

    // go through global popups
    కొరకు (wp = మొదటి_popupwin; wp != ఖాళీ; prev = wp, wp = wp->w_తదుపరి)
	ఒకవేళ (wp->w_id == id)
	{
	    ఒకవేళ (wp == curwin)
	    {
		ఒకవేళ (!force)
		{
		    error_for_popup_కిటికీ();
		    తిరిగి FAIL;
		}
		వెనుక_గమ్యం_prevwin(wp);
	    }
	    ఒకవేళ (prev == ఖాళీ)
		మొదటి_popupwin = wp->w_తదుపరి;
	    లేకపోతే
		prev->w_తదుపరి = wp->w_తదుపరి;
#నిర్వచితమైతే FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#అయితే_ముగింపు
	    popup_free(wp);
	    తిరిగి OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	ఒకవేళ (popup_close_tabpage(tp, id, force) == OK)
	    తిరిగి OK;
    తిరిగి FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    పూర్ణాంకం
popup_close_tabpage(tabpage_T *tp, పూర్ణాంకం id, పూర్ణాంకం force)
{
    win_T	*wp;
    win_T	**మూలం = &tp->tp_మొదటి_popupwin;
    win_T	*prev = ఖాళీ;

    కొరకు (wp = *మూలం; wp != ఖాళీ; prev = wp, wp = wp->w_తదుపరి)
	ఒకవేళ (wp->w_id == id)
	{
	    ఒకవేళ (wp == curwin)
	    {
		ఒకవేళ (!force)
		{
		    error_for_popup_కిటికీ();
		    తిరిగి FAIL;
		}
		వెనుక_గమ్యం_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    ఒకవేళ (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    ఒకవేళ (prev == ఖాళీ)
		*మూలం = wp->w_తదుపరి;
	    లేకపోతే
		prev->w_తదుపరి = wp->w_తదుపరి;
#నిర్వచితమైతే FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#అయితే_ముగింపు
	    popup_free(wp);
	    తిరిగి OK;
	}
    తిరిగి FAIL;
}

    శూన్య
close_all_popups(పూర్ణాంకం force)
{
    ఒకవేళ (!force && ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    అయితే (మొదటి_popupwin != ఖాళీ)
	ఒకవేళ (popup_close(మొదటి_popupwin->w_id, force) == FAIL)
	    తిరిగి;
    అయితే (curtab->tp_మొదటి_popupwin != ఖాళీ)
	ఒకవేళ (popup_close(curtab->tp_మొదటి_popupwin->w_id, force) == FAIL)
	    తిరిగి;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    స్థిర శూన్య
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->చురుకైన = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->ఎత్తు = popup_ఎత్తు(wp);
    area->వెడల్పు = popup_వెడల్పు(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    స్థిర శూన్య
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    పూర్ణాంకం i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    కొరకు (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    స్థిర bool
popup_style_మారింది(win_T *wp, popup_style_snapshot_T *style)
{
    పూర్ణాంకం i;

    ఒకవేళ (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	తిరిగి true;
    కొరకు (i = 0; i < 4; i++)
	ఒకవేళ (style->border_highlight[i] != wp->w_border_highlight[i])
	    తిరిగి true;
    తిరిగి false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    స్థిర శూన్య
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->వెడల్పు = wp->w_వెడల్పు;
    layout->ఎత్తు = wp->w_ఎత్తు;
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
    స్థిర bool
popup_layout_మారింది(win_T *wp, popup_layout_T *layout)
{
    తిరిగి layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->వెడల్పు != wp->w_వెడల్పు
	|| layout->ఎత్తు != wp->w_ఎత్తు
	|| layout->has_scrollbar != wp->w_has_scrollbar
	|| layout->topoff != wp->w_popup_topoff
	|| layout->bottomoff != wp->w_popup_bottomoff
	|| layout->leftclip != wp->w_popup_leftclip
	|| layout->rightclip != wp->w_popup_rightclip;
}

/*
 * Return true when the popup no longer covers the saved area.
 */
    స్థిర bool
popup_area_మారింది(win_T *wp, popup_area_T *area)
{
    తిరిగి area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->ఎత్తు != popup_ఎత్తు(wp)
	|| area->వెడల్పు != popup_వెడల్పు(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    స్థిర శూన్య
mark_overlapped_opacity_popup(win_T *wp, పూర్ణాంకం area_శిఖరం, పూర్ణాంకం area_bot,
	పూర్ణాంకం area_ఎడమ, పూర్ణాంకం area_కుడి, పూర్ణాంకం zindex)
{
    ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_ఎత్తు(wp) <= area_శిఖరం
	    || wp->w_wincol >= area_కుడి
	    || wp->w_wincol + popup_వెడల్పు(wp) - wp->w_popup_leftoff
							      <= area_ఎడమ)
	తిరిగి;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_పంక్తులు_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    స్థిర శూన్య
redraw_overlapped_opacity_popups(పూర్ణాంకం winrow, పూర్ణాంకం wincol, పూర్ణాంకం ఎత్తు, పూర్ణాంకం వెడల్పు,
	పూర్ణాంకం leftoff, పూర్ణాంకం zindex)
{
    win_T	*wp;
    పూర్ణాంకం		area_శిఖరం = winrow;
    పూర్ణాంకం		area_bot = winrow + ఎత్తు;
    పూర్ణాంకం		area_ఎడమ = wincol;
    పూర్ణాంకం		area_కుడి = wincol + వెడల్పు - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_శిఖరం, area_bot, area_ఎడమ,
		area_కుడి, zindex);
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_శిఖరం, area_bot, area_ఎడమ,
		area_కుడి, zindex);

    ఒకవేళ (must_redraw < UPD_VALID)
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
    స్థిర శూన్య
refill_pum_bg_అడ్డువరుస_from_బఫరు(పూర్ణాంకం r, పూర్ణాంకం ఎడమ_2, పూర్ణాంకం కుడి_2)
{
    పూర్ణాంకం		పంక్తి_cp = r;
    పూర్ణాంకం		నిలువు_cp = ఎడమ_2;
    win_T	*wp;
    linenr_T	lnum;
    పూర్ణాంకం		అడ్డువరుస_for_lnum;
    పూర్ణాంకం		నిలువు_for_lnum = 0;
    char_u	*పంక్తి;
    char_u	*p;
    పూర్ణాంకం		win_పాఠ్యం_నిలువు;
    పూర్ణాంకం		screen_నిలువు;
    పూర్ణాంకం		soff_పునాది = (r - pum_bg_శిఖరం) * pum_bg_నిలువుల_సంఖ్య;
    పూర్ణాంకం		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    కొరకు (c = ఎడమ_2; c < కుడి_2; ++c)
    {
	pum_bg_పంక్తులు[soff_పునాది + c] = ' ';
	ఒకవేళ (pum_bg_attrs != ఖాళీ)
	    pum_bg_attrs[soff_పునాది + c] = 0;
	ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ)
	    pum_bg_పంక్తులు_UC[soff_పునాది + c] = 0;
	ఒకవేళ (enc_utf8)
	{
	    పూర్ణాంకం k;
	    కొరకు (k = 0; k < MAX_MCO; ++k)
		ఒకవేళ (pum_bg_పంక్తులు_C[k] != ఖాళీ)
		    pum_bg_పంక్తులు_C[k][soff_పునాది + c] = 0;
	}
    }

    wp = mouse_కనుగొను_win(&పంక్తి_cp, &నిలువు_cp, IGNORE_POPUP);
    ఒకవేళ (wp == ఖాళీ || పంక్తి_cp < 0 || పంక్తి_cp >= wp->w_ఎత్తు)
	తిరిగి;
    ఒకవేళ (wp->w_బఫరు == ఖాళీ || wp->w_బఫరు->b_ml.ml_mfp == ఖాళీ)
	తిరిగి;

    // Compute the buffer line for this screen row.
    అడ్డువరుస_for_lnum = పంక్తి_cp;
    ఒకవేళ (mouse_comp_pos(wp, &అడ్డువరుస_for_lnum, &నిలువు_for_lnum, &lnum, ఖాళీ))
	తిరిగి;	// past end of buffer
    ఒకవేళ (lnum < 1 || lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    ఒకవేళ (పంక్తి == ఖాళీ)
	తిరిగి;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_పాఠ్యం_నిలువు = wp->w_wincol + win_నిలువు_off(wp);
    ఒకవేళ (!wp->w_p_wrap)
	win_పాఠ్యం_నిలువు -= wp->w_leftcol;
    screen_నిలువు = win_పాఠ్యం_నిలువు;
    p = పంక్తి;
    అయితే (*p != NUL && screen_నిలువు < కుడి_2)
    {
	పూర్ణాంకం char_cells;
	పూర్ణాంకం byte_లెక్క;
	పూర్ణాంకం soff = soff_పునాది + screen_నిలువు;
	పూర్ణాంకం లోపల_range = (screen_నిలువు >= ఎడమ_2 && screen_నిలువు < కుడి_2);

	ఒకవేళ (*p == '\t')
	{
	    పూర్ణాంకం ts = (పూర్ణాంకం)wp->w_బఫరు->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_నిలువు - win_పాఠ్యం_నిలువు) % ts) : 1;
	    byte_లెక్క = 1;
	    ఒకవేళ (లోపల_range)
	    {
		pum_bg_పంక్తులు[soff] = ' ';
		ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ)
		    pum_bg_పంక్తులు_UC[soff] = 0;
	    }
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_లెక్క = mb_ptr2len(p);
	    ఒకవేళ (లోపల_range)
	    {
		pum_bg_పంక్తులు[soff] = *p;
		ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ)
		    pum_bg_పంక్తులు_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	లేకపోతే
	{
	    char_cells = 1;
	    byte_లెక్క = 1;
	    ఒకవేళ (లోపల_range)
	    {
		pum_bg_పంక్తులు[soff] = (*p < 0x20) ? ' ' : *p;
		ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ)
		    pum_bg_పంక్తులు_UC[soff] = 0;
	    }
	}
	ఒకవేళ (లోపల_range && pum_bg_attrs != ఖాళీ)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_లెక్క;
	screen_నిలువు += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    స్థిర శూన్య
popup_redraw_exposed_area(popup_area_T *area)
{
    ఒకవేళ (!area->చురుకైన)
	తిరిగి;

    redraw_under_popup_area(area->winrow, area->wincol, area->ఎత్తు,
	    area->వెడల్పు, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->ఎత్తు, area->వెడల్పు, area->leftoff, area->zindex);

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
    ఒకవేళ (pum_bg_పంక్తులు != ఖాళీ
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->ఎత్తు > pum_bg_శిఖరం)
    {
	పూర్ణాంకం శిఖరం = MAX(area->winrow, pum_bg_శిఖరం);
	పూర్ణాంకం bot = MIN(area->winrow + area->ఎత్తు, pum_bg_bot);
	పూర్ణాంకం ఎడమ_2 = MAX(area->wincol, 0);
	పూర్ణాంకం కుడి_2 = MIN(area->wincol + area->వెడల్పు, pum_bg_నిలువుల_సంఖ్య);
	పూర్ణాంకం r;

	కొరకు (r = శిఖరం; r < bot; ++r)
	    refill_pum_bg_అడ్డువరుస_from_బఫరు(r, ఎడమ_2, కుడి_2);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    స్థిర శూన్య
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->పంక్తులు);
    vim_free(saved_screen->attrs);
    vim_free(saved_screen->linesuc);
    CLEAR_సూచిక(saved_screen);
}

/*
 * Save the screen area that opacity padding may need to blend against.
 * On entry "saved_screen" must be zero-initialised.  When the snapshot is
 * unavailable (no opacity popup, no padding, allocation failure) "lines" is
 * left NULL and callers should fall back to plain screen_fill().  The caller
 * must always release the snapshot with popup_free_saved_screen().
 */
    స్థిర శూన్య
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    ఒకవేళ (screen_opacity_popup == ఖాళీ
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	తిరిగి;

    saved_screen->ప్రారంభం_అడ్డువరుస = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->ప్రారంభం_నిలువు = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->అడ్డువరుసలు_2 = wp->w_popup_padding[0] + wp->w_ఎత్తు
						    + wp->w_popup_padding[2];
    saved_screen->నిలువుల_సంఖ్య = wp->w_popup_padding[3] + wp->w_వెడల్పు
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    ఒకవేళ (saved_screen->ప్రారంభం_నిలువు > 0)
    {
	--saved_screen->ప్రారంభం_నిలువు;
	++saved_screen->నిలువుల_సంఖ్య;
    }

    saved_screen->పంక్తులు = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->అడ్డువరుసలు_2 * saved_screen->నిలువుల_సంఖ్య);
    saved_screen->attrs = ALLOC_CLEAR_MULT(పూర్ణాంకం,
				  saved_screen->అడ్డువరుసలు_2 * saved_screen->నిలువుల_సంఖ్య);
    ఒకవేళ (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->అడ్డువరుసలు_2 * saved_screen->నిలువుల_సంఖ్య);

    ఒకవేళ (saved_screen->పంక్తులు == ఖాళీ || saved_screen->attrs == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం r = 0; r < saved_screen->అడ్డువరుసలు_2; r++)
    {
	పూర్ణాంకం screen_అడ్డువరుస = saved_screen->ప్రారంభం_అడ్డువరుస + r;

	ఒకవేళ (screen_అడ్డువరుస >= 0 && screen_అడ్డువరుస < screen_అడ్డువరుసలు)
	    కొరకు (పూర్ణాంకం c = 0; c < saved_screen->నిలువుల_సంఖ్య; c++)
	    {
		పూర్ణాంకం screen_నిలువు = saved_screen->ప్రారంభం_నిలువు + c;

		ఒకవేళ (screen_నిలువు >= 0 && screen_నిలువు < screen_Columns)
		{
		    పూర్ణాంకం off = పంక్తి_స్థానభ్రంశం[screen_అడ్డువరుస] + screen_నిలువు;
		    పూర్ణాంకం save_off = r * saved_screen->నిలువుల_సంఖ్య + c;

		    saved_screen->పంక్తులు[save_off] = Screen_పంక్తులు[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    ఒకవేళ (enc_utf8 && saved_screen->linesuc != ఖాళీ)
			saved_screen->linesuc[save_off] = Screen_పంక్తులు_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    స్థిర శూన్య
redraw_under_popup_area(పూర్ణాంకం winrow, పూర్ణాంకం wincol, పూర్ణాంకం ఎత్తు, పూర్ణాంకం వెడల్పు, పూర్ణాంకం leftoff)
{
    పూర్ణాంకం	    r;

    కొరకు (r = winrow; r < winrow + ఎత్తు && r < screen_అడ్డువరుసలు; ++r)
    {
	పూర్ణాంకం	    c;
	win_T	    *prev_twp = ఖాళీ;

	ఒకవేళ (r >= cmdline_అడ్డువరుస)
	{
	    clear_cmdline = TRUE;
	    కొనసాగు;
	}

	కొరకు (c = wincol; c < wincol + వెడల్పు - leftoff && c < screen_Columns; ++c)
	{
	    పూర్ణాంకం	    పంక్తి_cp = r;
	    పూర్ణాంకం	    నిలువు_cp = c;
	    win_T   *twp;

	    twp = mouse_కనుగొను_win(&పంక్తి_cp, &నిలువు_cp, IGNORE_POPUP);
	    ఒకవేళ (twp != ఖాళీ && twp != prev_twp)
	    {
		prev_twp = twp;
		ఒకవేళ (పంక్తి_cp < twp->w_ఎత్తు)
		{
		    linenr_T lnum;

		    (శూన్య)mouse_comp_pos(twp, &పంక్తి_cp, &నిలువు_cp, &lnum, ఖాళీ);
		    redrawWinline(twp, lnum);
		}
		లేకపోతే ఒకవేళ (పంక్తి_cp == twp->w_ఎత్తు)
		    twp->w_redr_స్థితి = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    శూన్య
f_popup_move(typval_T *argvars, typval_T *rettv వాడని)
{
    dict_T	*dict;
    పూర్ణాంకం		id;
    win_T	*wp;
    popup_area_T	old_area;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;  // invalid {id}

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	తిరిగి;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    ఒకవేళ (wp->w_winrow + wp->w_ఎత్తు >= cmdline_అడ్డువరుస)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    ఒకవేళ (popup_area_మారింది(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    శూన్య
f_popup_setoptions(typval_T *argvars, typval_T *rettv వాడని)
{
    dict_T	*dict;
    పూర్ణాంకం		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    పూర్ణాంకం		need_redraw = FALSE;
    పూర్ణాంకం		need_reposition = FALSE;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;  // invalid {id}

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	తిరిగి;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (శూన్య)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    ఒకవేళ (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    ఒకవేళ (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    లేకపోతే ఒకవేళ (popup_style_మారింది(wp, &old_style))
	need_redraw = TRUE;

    ఒకవేళ (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    ఒకవేళ (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_ముసుగు_refresh = TRUE;
    }
    లేకపోతే ఒకవేళ (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_పంక్తులు_valid = 0;
	ఒకవేళ (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    ఒకవేళ (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_ముసుగు_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    ఒకవేళ (popup_area_మారింది(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    శూన్య
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    పూర్ణాంకం		id;
    win_T	*wp;
    పూర్ణాంకం		శిఖరం_extra;
    పూర్ణాంకం		ఎడమ_extra;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;  // invalid {id}
    శిఖరం_extra = popup_శిఖరం_extra(wp);
    ఎడమ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_తాళం_కొలత(&dict->dv_hashtab, 11);

    dict_కూడు_సంఖ్య(dict, "line", wp->w_winrow + 1);
    dict_కూడు_సంఖ్య(dict, "col", wp->w_wincol + 1);
    dict_కూడు_సంఖ్య(dict, "width", wp->w_వెడల్పు + ఎడమ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_కూడు_సంఖ్య(dict, "height", wp->w_ఎత్తు + శిఖరం_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_కూడు_సంఖ్య(dict, "core_line", wp->w_winrow + 1 + శిఖరం_extra);
    dict_కూడు_సంఖ్య(dict, "core_col", wp->w_wincol + 1 + ఎడమ_extra);
    dict_కూడు_సంఖ్య(dict, "core_width", wp->w_వెడల్పు);
    dict_కూడు_సంఖ్య(dict, "core_height", wp->w_ఎత్తు);

    dict_కూడు_సంఖ్య(dict, "scrollbar", wp->w_has_scrollbar);
    dict_కూడు_సంఖ్య(dict, "firstline", wp->w_topline);
    dict_కూడు_సంఖ్య(dict, "lastline", wp->w_botline - 1);
    dict_కూడు_సంఖ్య(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    శూన్య
f_popup_list(typval_T *argvars వాడని, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    FOR_ALL_POPUPWINS(wp)
	list_append_సంఖ్య(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	    list_append_సంఖ్య(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    శూన్య
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    అడ్డువరుస = tv_get_సంఖ్య(&argvars[0]) - 1;
    నిలువు = tv_get_సంఖ్య(&argvars[1]) - 1;
    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
    ఒకవేళ (wp != ఖాళీ && WIN_IS_POPUP(wp))
	rettv->vval.v_సంఖ్య = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    స్థిర శూన్య
get_padding_border(dict_T *dict, పూర్ణాంకం *array, అక్షరం *name)
{
    list_T  *list;
    పూర్ణాంకం	    i;

    ఒకవేళ (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	తిరిగి;

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    ఒకవేళ (dict_కూడు_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	తిరిగి;
    }
    ఒకవేళ (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	కొరకు (i = 0; i < 4; ++i)
	    list_append_సంఖ్య(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    స్థిర శూన్య
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < 4; ++i)
	ఒకవేళ (wp->w_border_highlight[i] != ఖాళీ)
	    ఆపు;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    ఒకవేళ (i == 4 && !wp->w_border_highlight_isset)
	తిరిగి;

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    ఒకవేళ (dict_కూడు_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	తిరిగి;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    ఒకవేళ (i == 4)
	తిరిగి;
    కొరకు (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    స్థిర శూన్య
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    పూర్ణాంకం	    i;
    char_u  buf[NUMBUFLEN];
    పూర్ణాంకం	    len;

    కొరకు (i = 0; i < 8; ++i)
	ఒకవేళ (wp->w_border_char[i] != 0)
	    ఆపు;
    ఒకవేళ (i == 8)
	తిరిగి;

    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    ఒకవేళ (dict_కూడు_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	తిరిగి;
    }
    కొరకు (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    స్థిర శూన్య
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    ఒకవేళ (list != ఖాళీ)
    {
	ఒకవేళ (dict_కూడు_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	లేకపోతే
	{
	    list_append_సంఖ్య(list, wp->w_popup_lnum);
	    list_append_సంఖ్య(list, wp->w_popup_mincol);
	    list_append_సంఖ్య(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    ఒకవేళ (list == ఖాళీ)
	తిరిగి;

    ఒకవేళ (dict_కూడు_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	తిరిగి;
    }
    list_append_సంఖ్య(list, wp->w_popup_mouse_అడ్డువరుస);
    list_append_సంఖ్య(list, wp->w_popup_mouse_mincol);
    list_append_సంఖ్య(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    శూన్య
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    పూర్ణాంకం		id;
    win_T	*wp;
    tabpage_T	*tp;
    పూర్ణాంకం		i;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = కనుగొను_popup_win(id);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    dict = rettv->vval.v_dict;
    dict_కూడు_సంఖ్య(dict, "line", wp->w_wantline);
    dict_కూడు_సంఖ్య(dict, "col", wp->w_wantcol);
    dict_కూడు_సంఖ్య(dict, "minwidth", wp->w_minwidth);
    dict_కూడు_సంఖ్య(dict, "minheight", wp->w_minheight);
    dict_కూడు_సంఖ్య(dict, "maxheight", wp->w_maxheight);
    dict_కూడు_సంఖ్య(dict, "maxwidth", wp->w_maxwidth);
    dict_కూడు_సంఖ్య(dict, "firstline", wp->w_firstline);
    dict_కూడు_సంఖ్య(dict, "scrollbar", wp->w_want_scrollbar);
    dict_కూడు_సంఖ్య(dict, "zindex", wp->w_zindex);
    dict_కూడు_సంఖ్య(dict, "fixed", wp->w_popup_fixed);
    ఒకవేళ (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = పాఠ్యం_prop_type_by_id(
		wp->w_popup_prop_win->w_బఫరు,
		wp->w_popup_prop_type);

	ఒకవేళ (pt != ఖాళీ)
	    dict_కూడు_string(dict, "textprop", pt->pt_name);
	dict_కూడు_సంఖ్య(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_కూడు_సంఖ్య(dict, "textpropid", wp->w_popup_prop_id);
    }

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    ఒకవేళ (wp->w_popup_imagep != ఖాళీ && wp->w_popup_imagep->img != ఖాళీ)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	ఒకవేళ (dict_కూడు_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#అయితే_ముగింపు

    dict_కూడు_string(dict, "title", wp->w_popup_title);
    dict_కూడు_సంఖ్య(dict, "wrap", wp->w_p_wrap);
    dict_కూడు_సంఖ్య(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_కూడు_సంఖ్య(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_కూడు_సంఖ్య(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_కూడు_సంఖ్య(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_కూడు_సంఖ్య(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_కూడు_సంఖ్య(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_కూడు_సంఖ్య(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_కూడు_సంఖ్య(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_కూడు_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_కూడు_string(dict, "highlights", wp->w_p_whl);
    ఒకవేళ (wp->w_scrollbar_highlight != ఖాళీ)
	dict_కూడు_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    ఒకవేళ (wp->w_thumb_highlight != ఖాళీ)
	dict_కూడు_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_లోపల_TAB(tp, twp)
	    ఒకవేళ (twp->w_id == id)
		ఆపు;
	ఒకవేళ (twp != ఖాళీ)
	    ఆపు;
	++i;
    }
    ఒకవేళ (tp == ఖాళీ)
	i = -1;  // must be global
    లేకపోతే ఒకవేళ (tp == curtab)
	i = 0;
    dict_కూడు_సంఖ్య(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    ఒకవేళ (wp->w_filter_cb.cb_name != ఖాళీ)
	dict_కూడు_callback(dict, "filter", &wp->w_filter_cb);
    ఒకవేళ (wp->w_close_cb.cb_name != ఖాళీ)
	dict_కూడు_callback(dict, "callback", &wp->w_close_cb);

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(poppos_entries); ++i)
	ఒకవేళ (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_కూడు_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(పూర్ణాంకం)poppos_entries[i].pp_name.నిడివి);
	    ఆపు;
	}

    ఒకవేళ (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_కూడు_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    లేకపోతే ఒకవేళ (wp->w_popup_close == POPCLOSE_CLICK)
	dict_కూడు_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    లేకపోతే
	dict_కూడు_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#ఒకవేళ defined(FEAT_TIMERS)
    dict_కూడు_సంఖ్య(dict, "time", wp->w_popup_timer != ఖాళీ
	    ?  (దీర్ఘ)wp->w_popup_timer->tr_interval : 0L);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    పూర్ణాంకం
error_if_పదం_popup_కిటికీ(శూన్య)
{
    ఒకవేళ (WIN_IS_POPUP(curwin) && curbuf->b_పదం != ఖాళీ
					   && పదం_పని_running(curbuf->b_పదం))
    {
	emsg(_(e_not_allowed_for_terminal_లోపల_popup_కిటికీ));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    శూన్య
popup_reset_handled(పూర్ణాంకం handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
కనుగొను_తదుపరి_popup(పూర్ణాంకం అత్యల్ప, పూర్ణాంకం handled_flag)
{
    win_T   *wp;
    win_T   *దొరికింది_wp;
    పూర్ణాంకం	    దొరికింది_zindex;

    దొరికింది_zindex = అత్యల్ప ? గరిష్ఠ_పూర్ణాంకం : 0;
    దొరికింది_wp = ఖాళీ;
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (అత్యల్ప ? wp->w_zindex < దొరికింది_zindex
		    : wp->w_zindex > దొరికింది_zindex))
	{
	    దొరికింది_zindex = wp->w_zindex;
	    దొరికింది_wp = wp;
	}
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (అత్యల్ప ? wp->w_zindex < దొరికింది_zindex
		    : wp->w_zindex > దొరికింది_zindex))
	{
	    దొరికింది_zindex = wp->w_zindex;
	    దొరికింది_wp = wp;
	}

    ఒకవేళ (దొరికింది_wp != ఖాళీ)
	దొరికింది_wp->w_popup_handled |= handled_flag;
    తిరిగి దొరికింది_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    స్థిర పూర్ణాంకం
invoke_popup_filter(win_T *wp, పూర్ణాంకం c)
{
    పూర్ణాంకం		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    పూర్ణాంకం		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    ఒకవేళ (c == Ctrl_C)
    {
	పూర్ణాంకం save_got_int = got_int;
	పూర్ణాంకం was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	ఒకవేళ (was_curwin && wp == curwin)
	    తిరిగి -1;
	తిరిగి TRUE;
    }

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_గమ్యం_buf(c, mod_ముసుగు, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    ఒకవేళ (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    లేకపోతే
    {
	ఒకవేళ (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	ఒకవేళ (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    ఒకవేళ (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	లేకపోతే
	{
	    ఒకవేళ (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    తిరిగి res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    పూర్ణాంకం
popup_do_filter(పూర్ణాంకం c)
{
    స్థిర పూర్ణాంకం	recursive = FALSE;
    పూర్ణాంకం		res = FALSE;
    win_T	*wp;
    పూర్ణాంకం		save_కీలకం_Typed = కీలకం_Typed;
    పూర్ణాంకం		state;
    పూర్ణాంకం		was_must_redraw = must_redraw;

#నిర్వచితమైతే FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    ఒకవేళ (popup_is_popup(curwin) && curbuf->b_పదం != ఖాళీ)
	తిరిగి FALSE;
#అయితే_ముగింపు

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE
	    || State == బహుళకం_CONFIRM || State == బహుళకం_EXTERNCMD)
	తిరిగి FALSE;

    ఒకవేళ (recursive)
	తిరిగి FALSE;
    recursive = TRUE;

    ఒకవేళ (c == K_LEFTMOUSE)
    {
	పూర్ణాంకం అడ్డువరుస = mouse_అడ్డువరుస;
	పూర్ణాంకం నిలువు = mouse_నిలువు;

	wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
	ఒకవేళ (wp != ఖాళీ && popup_close_if_on_X(wp, అడ్డువరుస, నిలువు))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    అయితే (res == FALSE
		     && (wp = కనుగొను_తదుపరి_popup(FALSE, POPUP_HANDLED_2)) != ఖాళీ)
	ఒకవేళ (wp->w_filter_cb.cb_name != ఖాళీ
		&& (wp->w_filter_బహుళకం & state) != 0)
	    res = invoke_popup_filter(wp, c);

    ఒకవేళ (must_redraw > was_must_redraw)
    {
	పూర్ణాంకం save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    కీలకం_Typed = save_కీలకం_Typed;

    // When interrupted return FALSE to avoid looping.
    తిరిగి res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    పూర్ణాంకం
popup_no_mapping(శూన్య)
{
    పూర్ణాంకం	    విడత;
    win_T   *wp;

    కొరకు (విడత = 1; విడత <= 2; ++విడత)
	కొరకు (wp = విడత == 1 ? మొదటి_popupwin : curtab->tp_మొదటి_popupwin;
						   wp != ఖాళీ; wp = wp->w_తదుపరి)
	    ఒకవేళ (wp->w_filter_cb.cb_name != ఖాళీ
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    శూన్య
popup_check_cursor_pos(శూన్య)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    అయితే ((wp = కనుగొను_తదుపరి_popup(TRUE, POPUP_HANDLED_3)) != ఖాళీ)
	ఒకవేళ (wp->w_popup_curwin != ఖాళీ
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.నిలువు < wp->w_popup_mincol
		    || curwin->w_cursor.నిలువు > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    స్థిర శూన్య
popup_update_ముసుగు(win_T *wp, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    పూర్ణాంకం		అడ్డువరుస, నిలువు;

    ఒకవేళ (wp->w_popup_ముసుగు == ఖాళీ || వెడల్పు == 0 || ఎత్తు == 0)
    {
	VIM_CLEAR(wp->w_popup_ముసుగు_cells);
	తిరిగి;
    }
    ఒకవేళ (wp->w_popup_ముసుగు_cells != ఖాళీ
	    && wp->w_popup_ముసుగు_ఎత్తు == ఎత్తు
	    && wp->w_popup_ముసుగు_వెడల్పు == వెడల్పు)
	తిరిగి;  // cache is still valid

    vim_free(wp->w_popup_ముసుగు_cells);
    ఒకవేళ (వెడల్పు > 0 && (size_t)ఎత్తు > కొలత_MAX / (size_t)వెడల్పు)
    {
	wp->w_popup_ముసుగు_cells = ఖాళీ;
	తిరిగి;
    }
    wp->w_popup_ముసుగు_cells = alloc_clear((size_t)వెడల్పు * ఎత్తు);
    ఒకవేళ (wp->w_popup_ముసుగు_cells == ఖాళీ)
	తిరిగి;
    cells = wp->w_popup_ముసుగు_cells;

    FOR_ALL_LIST_వస్తువులు(wp->w_popup_ముసుగు, lio)
    {
	పూర్ణాంకం నిలువుల_సంఖ్య, cole;
	పూర్ణాంకం పంక్తులు, linee;

	li = lio->li_tv.vval.v_list->lv_మొదటి;
	నిలువుల_సంఖ్య = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (నిలువుల_సంఖ్య < 0)
	    నిలువుల_సంఖ్య = వెడల్పు + నిలువుల_సంఖ్య + 1;
	ఒకవేళ (నిలువుల_సంఖ్య <= 0)
	    నిలువుల_సంఖ్య = 1;
	li = li->li_తదుపరి;
	cole = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (cole < 0)
	    cole = వెడల్పు + cole + 1;
	ఒకవేళ (cole > వెడల్పు)
	    cole = వెడల్పు;
	li = li->li_తదుపరి;
	పంక్తులు = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (పంక్తులు < 0)
	    పంక్తులు = ఎత్తు + పంక్తులు + 1;
	ఒకవేళ (పంక్తులు <= 0)
	    పంక్తులు = 1;
	li = li->li_తదుపరి;
	linee = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (linee < 0)
	    linee = ఎత్తు + linee + 1;
	ఒకవేళ (linee > ఎత్తు)
	    linee = ఎత్తు;

	కొరకు (అడ్డువరుస = పంక్తులు - 1; అడ్డువరుస < linee; ++అడ్డువరుస)
	    కొరకు (నిలువు = నిలువుల_సంఖ్య - 1; నిలువు < cole; ++నిలువు)
		cells[అడ్డువరుస * వెడల్పు + నిలువు] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    స్థిర పూర్ణాంకం
popup_masked(win_T *wp, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, పూర్ణాంకం screencol, పూర్ణాంకం screenline)
{
    పూర్ణాంకం నిలువు = screencol - wp->w_wincol + wp->w_popup_leftoff;
    పూర్ణాంకం పంక్తి = screenline - wp->w_winrow;

    తిరిగి నిలువు >= 0 && నిలువు < వెడల్పు
	    && పంక్తి >= 0 && పంక్తి < ఎత్తు
	    && wp->w_popup_ముసుగు_cells[పంక్తి * వెడల్పు + నిలువు];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    స్థిర శూన్య
update_popup_transparent(win_T *wp, పూర్ణాంకం val)
{
    ఒకవేళ (wp->w_popup_ముసుగు == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం		వెడల్పు = popup_వెడల్పు(wp);
    పూర్ణాంకం		ఎత్తు = popup_ఎత్తు(wp);
    listitem_T	*lio, *li;
    పూర్ణాంకం		నిలువుల_సంఖ్య, cole;
    పూర్ణాంకం		పంక్తులు, linee;
    పూర్ణాంకం		నిలువు, పంక్తి;

    FOR_ALL_LIST_వస్తువులు(wp->w_popup_ముసుగు, lio)
    {
	li = lio->li_tv.vval.v_list->lv_మొదటి;
	నిలువుల_సంఖ్య = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (నిలువుల_సంఖ్య < 0)
	    నిలువుల_సంఖ్య = వెడల్పు + నిలువుల_సంఖ్య + 1;
	li = li->li_తదుపరి;
	cole = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (cole < 0)
	    cole = వెడల్పు + cole + 1;
	li = li->li_తదుపరి;
	పంక్తులు = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (పంక్తులు < 0)
	    పంక్తులు = ఎత్తు + పంక్తులు + 1;
	li = li->li_తదుపరి;
	linee = tv_get_సంఖ్య(&li->li_tv);
	ఒకవేళ (linee < 0)
	    linee = ఎత్తు + linee + 1;

	--నిలువుల_సంఖ్య;
	నిలువుల_సంఖ్య -= wp->w_popup_leftoff;
	ఒకవేళ (నిలువుల_సంఖ్య < 0)
	    నిలువుల_సంఖ్య = 0;
	cole -= wp->w_popup_leftoff;
	--పంక్తులు;
	ఒకవేళ (పంక్తులు < 0)
	    పంక్తులు = 0;
	కొరకు (పంక్తి = పంక్తులు; పంక్తి < linee
		&& పంక్తి + wp->w_winrow < screen_అడ్డువరుసలు; ++పంక్తి)
	    కొరకు (నిలువు = నిలువుల_సంఖ్య; నిలువు < cole
		    && నిలువు + wp->w_wincol < screen_Columns; ++నిలువు)
		popup_transparent[(పంక్తి + wp->w_winrow) * screen_Columns
		    + నిలువు + wp->w_wincol] = val;
    }
}

/*
 * Only called when popup window "wp" is hidden: If the window is positioned
 * next to a text property, and it is now visible, then  unhide the popup.
 * We don't check if visible popups become hidden, that is done in
 * popup_adjust_position().
 * Return TRUE if the popup became unhidden.
 */
    స్థిర పూర్ణాంకం
check_popup_unhidden(win_T *wp)
{
    ఒకవేళ (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    దొరికింది = false;

	ఒకవేళ ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    తిరిగి FALSE;
	ఒకవేళ (కనుగొను_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    దొరికింది = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	లేకపోతే ఒకవేళ (popup_కనుగొను_prop_above_శిఖరం(wp, wp->w_popup_prop_win,
					    popup_ఎత్తు(wp), &prop, &lnum))
	    దొరికింది = true;
	ఒకవేళ (దొరికింది)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    స్థిర పూర్ణాంకం
popup_need_position_adjust(win_T *wp)
{
    ఒకవేళ (wp->w_popup_చివరి_changedtick != CHANGEDTICK(wp->w_బఫరు))
	తిరిగి TRUE;
    ఒకవేళ (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_బఫరు)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_వెడల్పు != wp->w_popup_prop_win->w_వెడల్పు
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_ఎత్తు))
	తిరిగి TRUE;

    // May need to adjust the width if the cursor moved.
    తిరిగి wp->w_cursor.lnum != wp->w_popup_చివరి_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
స్థిర పొట్టి *opacity_zindex = ఖాళీ;
స్థిర పూర్ణాంకం    opacity_zindex_అడ్డువరుసలు = 0;
స్థిర పూర్ణాంకం    opacity_zindex_నిలువుల_సంఖ్య = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    స్థిర శూన్య
popup_mark_opacity_zindex(win_T *wp)
{
    పూర్ణాంకం	    వెడల్పు;
    పూర్ణాంకం	    ఎత్తు;
    పూర్ణాంకం	    r, c;

    ఒకవేళ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	తిరిగి;

    వెడల్పు = popup_వెడల్పు(wp);
    ఎత్తు = popup_ఎత్తు(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    కొరకు (r = wp->w_winrow;
		       r < wp->w_winrow + ఎత్తు - wp->w_popup_topoff
						&& r < screen_అడ్డువరుసలు; ++r)
	కొరకు (c = wp->w_wincol;
		 c < wp->w_wincol + వెడల్పు - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    పూర్ణాంకం off = r * screen_Columns + c;
	    ఒకవేళ (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    స్థిర శూన్య
redraw_win_under_opacity_popup(win_T *wp)
{
    పూర్ణాంకం	    ఎత్తు;
    పూర్ణాంకం	    వెడల్పు;
    పూర్ణాంకం	    r;

    ఒకవేళ (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	తిరిగి;

    ఎత్తు = popup_ఎత్తు(wp);
    వెడల్పు = popup_వెడల్పు(wp);
    కొరకు (r = wp->w_winrow;
		       r < wp->w_winrow + ఎత్తు && r < screen_అడ్డువరుసలు; ++r)
    {
	పూర్ణాంకం	    నిలువు;
	win_T	    *prev_twp = ఖాళీ;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	కొరకు (నిలువు = wp->w_wincol;
		నిలువు < wp->w_wincol + వెడల్పు && నిలువు < screen_Columns; ++నిలువు)
	{
	    పూర్ణాంకం	    పంక్తి_cp = r;
	    పూర్ణాంకం	    నిలువు_cp = నిలువు;
	    win_T   *twp;

	    twp = mouse_కనుగొను_win(&పంక్తి_cp, &నిలువు_cp, IGNORE_POPUP);
	    ఒకవేళ (twp != ఖాళీ && twp != prev_twp)
	    {
		prev_twp = twp;
		ఒకవేళ (పంక్తి_cp < twp->w_ఎత్తు)
		{
		    linenr_T lnum;

#నిర్వచితమైతే FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    ఒకవేళ (పదం_do_update_కిటికీ(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    లేకపోతే
#అయితే_ముగింపు
		    {
			(శూన్య)mouse_comp_pos(twp, &పంక్తి_cp, &నిలువు_cp, &lnum,
									ఖాళీ);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		లేకపోతే ఒకవేళ (పంక్తి_cp == twp->w_ఎత్తు)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_స్థితి = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    పూర్ణాంకం
popup_is_under_opacity(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (opacity_zindex == ఖాళీ
	    || అడ్డువరుస < 0 || అడ్డువరుస >= opacity_zindex_అడ్డువరుసలు
	    || నిలువు < 0 || నిలువు >= opacity_zindex_నిలువుల_సంఖ్య)
	తిరిగి FALSE;
    తిరిగి opacity_zindex[అడ్డువరుస * opacity_zindex_నిలువుల_సంఖ్య + నిలువు] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    పూర్ణాంకం
popup_is_over_opacity(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& అడ్డువరుస >= wp->w_winrow
		&& అడ్డువరుస < wp->w_winrow + popup_ఎత్తు(wp)
		&& నిలువు >= wp->w_wincol
		&& నిలువు < wp->w_wincol + popup_వెడల్పు(wp))
	    తిరిగి TRUE;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& అడ్డువరుస >= wp->w_winrow
		&& అడ్డువరుస < wp->w_winrow + popup_ఎత్తు(wp)
		&& నిలువు >= wp->w_wincol
		&& నిలువు < wp->w_wincol + popup_వెడల్పు(wp))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    పూర్ణాంకం
popup_is_under_opacity_range(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం ప్రారంభం_నిలువు, పూర్ణాంకం చివర_నిలువు)
{
    పూర్ణాంకం నిలువు;

    ఒకవేళ (opacity_zindex == ఖాళీ
	    || అడ్డువరుస < 0 || అడ్డువరుస >= opacity_zindex_అడ్డువరుసలు)
	తిరిగి FALSE;
    కొరకు (నిలువు = ప్రారంభం_నిలువు; నిలువు < చివర_నిలువు && నిలువు < opacity_zindex_నిలువుల_సంఖ్య; ++నిలువు)
	ఒకవేళ (opacity_zindex[అడ్డువరుస * opacity_zindex_నిలువుల_సంఖ్య + నిలువు] > screen_zindex)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    శూన్య
may_update_popup_ముసుగు(పూర్ణాంకం type)
{
    win_T	*wp;
    పొట్టి	*ముసుగు;
    పూర్ణాంకం		పంక్తి, నిలువు;
    పూర్ణాంకం		redraw_all_popups = FALSE;
    పూర్ణాంకం		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    ఒకవేళ (popup_ముసుగు_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_ముసుగు_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ముసుగు_refresh |= check_popup_unhidden(wp);
	లేకపోతే ఒకవేళ (popup_need_position_adjust(wp))
	    popup_ముసుగు_refresh = TRUE;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ముసుగు_refresh |= check_popup_unhidden(wp);
	లేకపోతే ఒకవేళ (popup_need_position_adjust(wp))
	    popup_ముసుగు_refresh = TRUE;

    // Force background windows to redraw rows under opacity popups.
    // Opacity popups don't participate in popup_mask, so their area
    // wouldn't normally be redrawn.  Without this, ScreenAttrs retains
    // blended values from the previous cycle, causing blend accumulation.
    // This must run every cycle, not just when popup_mask_refresh is set.
    //
    // Also build the opacity_zindex array used by screen_char() to suppress
    // output for cells under opacity popups during background draw.
    {
	పూర్ణాంకం has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	ఒకవేళ (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_అడ్డువరుసలు = 0;
	    opacity_zindex_నిలువుల_సంఖ్య = 0;
	}
	లేకపోతే
	{
	    ఒకవేళ (opacity_zindex_అడ్డువరుసలు != screen_అడ్డువరుసలు
		    || opacity_zindex_నిలువుల_సంఖ్య != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(పొట్టి,
					screen_అడ్డువరుసలు * screen_Columns);
		opacity_zindex_అడ్డువరుసలు = screen_అడ్డువరుసలు;
		opacity_zindex_నిలువుల_సంఖ్య = screen_Columns;
	    }
	    ఒకవేళ (opacity_zindex != ఖాళీ)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_అడ్డువరుసలు * screen_Columns * పరిమాణం(పొట్టి));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    ఒకవేళ (!popup_ముసుగు_refresh)
	తిరిగి;

    // Need to update the mask, something has changed.
    popup_ముసుగు_refresh = FALSE;
    popup_ముసుగు_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    ఒకవేళ (redrawing_all_win)
	ముసుగు = popup_ముసుగు;
    లేకపోతే
	ముసుగు = popup_ముసుగు_తదుపరి;
    vim_memset(ముసుగు, 0, (size_t)screen_అడ్డువరుసలు * screen_Columns * పరిమాణం(పొట్టి));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    అయితే ((wp = కనుగొను_తదుపరి_popup(TRUE, POPUP_HANDLED_4)) != ఖాళీ)
    {
	పూర్ణాంకం వెడల్పు;
	పూర్ణాంకం ఎత్తు;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	ఒకవేళ (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    ఒకవేళ (wp->w_popup_flags & POPF_HIDDEN)
		కొనసాగు;
	}

	వెడల్పు = popup_వెడల్పు(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	ఒకవేళ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    ఎత్తు = cl.eff_ఎత్తు;
	}
	లేకపోతే
	    ఎత్తు = popup_ఎత్తు(wp);
	popup_update_ముసుగు(wp, వెడల్పు, ఎత్తు);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    కొనసాగు;

	{
	    పూర్ణాంకం ముసుగు_ప్రారంభం = wp->w_winrow;
	    పూర్ణాంకం ముసుగు_చివర = ముసుగు_ప్రారంభం + ఎత్తు;
	    పూర్ణాంకం ముసుగు_నిలువు_ప్రారంభం = wp->w_wincol + wp->w_popup_leftclip;
	    పూర్ణాంకం ముసుగు_నిలువు_చివర = wp->w_wincol + వెడల్పు - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    కొరకు (పంక్తి = ముసుగు_ప్రారంభం;
			    పంక్తి < ముసుగు_చివర && పంక్తి < screen_అడ్డువరుసలు; ++పంక్తి)
		కొరకు (నిలువు = ముసుగు_నిలువు_ప్రారంభం;
		     నిలువు < ముసుగు_నిలువు_చివర && నిలువు < screen_Columns; ++నిలువు)
		    ఒకవేళ (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(పంక్తి, నిలువు, FALSE))
			ముసుగు[పంక్తి * screen_Columns + నిలువు] = POPUPMENU_ZINDEX;
		    లేకపోతే ఒకవేళ (wp->w_popup_ముసుగు_cells == ఖాళీ
				    || !popup_masked(wp, వెడల్పు, ఎత్తు, నిలువు, పంక్తి))
			ముసుగు[పంక్తి * screen_Columns + నిలువు] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    ఒకవేళ (ముసుగు == popup_ముసుగు_తదుపరి)
    {
	పూర్ణాంకం	    *plines_cache = ALLOC_CLEAR_MULT(పూర్ణాంకం, అడ్డువరుసలు);
	win_T	    *prev_wp = ఖాళీ;

	కొరకు (పంక్తి = 0; పంక్తి < screen_అడ్డువరుసలు; ++పంక్తి)
	{
	    పూర్ణాంకం	    నిలువు_పూర్తి = 0;

	    కొరకు (నిలువు = 0; నిలువు < screen_Columns; ++నిలువు)
	    {
		పూర్ణాంకం off = పంక్తి * screen_Columns + నిలువు;

		ఒకవేళ (popup_ముసుగు[off] != popup_ముసుగు_తదుపరి[off])
		{
		    popup_ముసుగు[off] = popup_ముసుగు_తదుపరి[off];

		    ఒకవేళ (పంక్తి >= cmdline_అడ్డువరుస)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			ఒకవేళ (!సందేశం_scrolled && popup_ముసుగు_తదుపరి[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    లేకపోతే ఒకవేళ (నిలువు >= నిలువు_పూర్తి)
		    {
			linenr_T	lnum;
			పూర్ణాంకం		పంక్తి_cp = పంక్తి;
			పూర్ణాంకం		నిలువు_cp = నిలువు;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_కనుగొను_win(&పంక్తి_cp, &నిలువు_cp, IGNORE_POPUP);
			ఒకవేళ (wp != ఖాళీ)
			{
#ఒకవేళ defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    ఒకవేళ (bt_terminal(wp->w_బఫరు))
				redraw_win_later(wp, UPD_NOT_VALID);
			    లేకపోతే
#అయితే_ముగింపు
			    {
				ఒకవేళ (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   పరిమాణం(పూర్ణాంకం) * అడ్డువరుసలు);
				    prev_wp = wp;
				}

				ఒకవేళ (పంక్తి_cp >= wp->w_ఎత్తు)
				    // In (or below) status line
				    wp->w_redr_స్థితి = true;
				లేకపోతే
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (శూన్య)mouse_comp_pos(wp, &పంక్తి_cp, &నిలువు_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    నిలువు_పూర్తి = wp->w_wincol + wp->w_వెడల్పు - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_ఉపయోగిస్తుంది_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    శూన్య
may_update_popup_position(శూన్య)
{
    ఒకవేళ (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#నిర్వచితమైతే FEAT_PROP_POPUP
స్థిర schar_T *పునాది_screenlines = ఖాళీ;
స్థిర పూర్ణాంకం *పునాది_screenattrs = ఖాళీ;
స్థిర u8char_T *పునాది_screenlinesuc = ఖాళీ;
స్థిర పూర్ణాంకం పునాది_screen_అడ్డువరుసలు = 0;
స్థిర పూర్ణాంకం పునాది_screen_నిలువుల_సంఖ్య = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    పూర్ణాంకం
popup_get_పునాది_screen_cell(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, schar_T *linep, పూర్ణాంకం *attrp,
							  u8char_T *ucp)
{
    ఒకవేళ (పునాది_screenlines == ఖాళీ || పునాది_screenattrs == ఖాళీ)
	తిరిగి FALSE;
    ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0 || అడ్డువరుస >= పునాది_screen_అడ్డువరుసలు
						     || నిలువు >= పునాది_screen_నిలువుల_సంఖ్య)
	తిరిగి FALSE;

    పూర్ణాంకం off = అడ్డువరుస * పునాది_screen_నిలువుల_సంఖ్య + నిలువు;
    ఒకవేళ (linep != ఖాళీ)
	*linep = పునాది_screenlines[off];
    ఒకవేళ (attrp != ఖాళీ)
	*attrp = పునాది_screenattrs[off];
    ఒకవేళ (ucp != ఖాళీ)
    {
	ఒకవేళ (enc_utf8 && పునాది_screenlinesuc != ఖాళీ)
	    *ucp = పునాది_screenlinesuc[off];
	లేకపోతే
	    *ucp = 0;
    }
    తిరిగి TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    శూన్య
popup_సమితి_పునాది_screen_cell(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, schar_T పంక్తి, పూర్ణాంకం attr, u8char_T uc)
{
    ఒకవేళ (పునాది_screenlines == ఖాళీ || పునాది_screenattrs == ఖాళీ)
	తిరిగి;
    ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0 || అడ్డువరుస >= పునాది_screen_అడ్డువరుసలు
						     || నిలువు >= పునాది_screen_నిలువుల_సంఖ్య)
	తిరిగి;

    పూర్ణాంకం off = అడ్డువరుస * పునాది_screen_నిలువుల_సంఖ్య + నిలువు;
    పునాది_screenlines[off] = పంక్తి;
    పునాది_screenattrs[off] = attr;
    ఒకవేళ (enc_utf8 && పునాది_screenlinesuc != ఖాళీ)
	పునాది_screenlinesuc[off] = uc;
}
#అయితే_ముగింపు

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
స్థిర శూన్య
draw_opacity_padding_cell(
	పూర్ణాంకం		అడ్డువరుస,
	పూర్ణాంకం		నిలువు,
	popup_saved_screen_T *saved_screen,
	పూర్ణాంకం		pad_ప్రారంభం_నిలువు,
	పూర్ణాంకం		pad_చివర_నిలువు)
{
    ఒకవేళ (పంక్తి_స్థానభ్రంశం == ఖాళీ || Screen_పంక్తులు == ఖాళీ || ScreenAttrs == ఖాళీ
	    || (enc_utf8 && Screen_పంక్తులు_UC == ఖాళీ))
	తిరిగి;

    పూర్ణాంకం off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు;
    పూర్ణాంకం r = అడ్డువరుస - saved_screen->ప్రారంభం_అడ్డువరుస;
    పూర్ణాంకం c = నిలువు - saved_screen->ప్రారంభం_నిలువు;

    ఒకవేళ (r >= 0 && r < saved_screen->అడ్డువరుసలు_2 && c >= 0 && c < saved_screen->నిలువుల_సంఖ్య)
    {
	పూర్ణాంకం save_off = r * saved_screen->నిలువుల_సంఖ్య + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	ఒకవేళ (enc_utf8 && saved_screen->linesuc != ఖాళీ)
	{
	    పూర్ణాంకం పునాది_నిలువు = నిలువు - 1;
	    పూర్ణాంకం పునాది_off = off - 1;
	    పూర్ణాంకం పునాది_save_off = save_off - 1;
	    పూర్ణాంకం wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    ఒకవేళ (పునాది_off >= 0)
	    {
		ఒకవేళ (Screen_పంక్తులు_UC[పునాది_off] != 0
			&& utf_char2cells(Screen_పంక్తులు_UC[పునాది_off]) == 2
			&& Screen_పంక్తులు[off] == 0)
		    wide_prev = TRUE;
	    }
	    ఒకవేళ (!wide_prev && save_off > 0)
	    {
		ఒకవేళ (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->పంక్తులు[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    ఒకవేళ (wide_prev && పునాది_నిలువు >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		ఒకవేళ (పునాది_నిలువు < pad_ప్రారంభం_నిలువు)
		{
		    ఒకవేళ (Screen_పంక్తులు_UC[పునాది_off] != 0
			    && utf_char2cells(Screen_పంక్తులు_UC[పునాది_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_పంక్తులు[పునాది_off] = ' ';
			Screen_పంక్తులు_UC[పునాది_off] = 0;
			ScreenAttrs[పునాది_off] = saved_screen->attrs[పునాది_save_off];
			popup_సమితి_పునాది_screen_cell(అడ్డువరుస, పునాది_నిలువు,
				Screen_పంక్తులు[పునాది_off],
				ScreenAttrs[పునాది_off],
				Screen_పంక్తులు_UC[పునాది_off]);
			screen_char(పునాది_off, అడ్డువరుస, పునాది_నిలువు);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_పంక్తులు[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[పునాది_save_off];
			ఒకవేళ (enc_utf8)
			    Screen_పంక్తులు_UC[off] = 0;
			పూర్ణాంకం popup_attr_val =
					get_win_attr(screen_opacity_popup);
			పూర్ణాంకం blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_సమితి_పునాది_screen_cell(అడ్డువరుస, నిలువు,
				Screen_పంక్తులు[off], ScreenAttrs[off],
				Screen_పంక్తులు_UC[off]);
			screen_char(off, అడ్డువరుస, నిలువు);
			తిరిగి;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    ఒకవేళ (పునాది_save_off >= 0
			    && saved_screen->linesuc[పునాది_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[పునాది_save_off]) == 2
			    && Screen_పంక్తులు[పునాది_off] == ' '
			    && Screen_పంక్తులు_UC[పునాది_off] == 0)
		    {
			పూర్ణాంకం popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			పూర్ణాంకం blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_పంక్తులు[పునాది_off] =
				    saved_screen->పంక్తులు[పునాది_save_off];
			Screen_పంక్తులు_UC[పునాది_off] =
				    saved_screen->linesuc[పునాది_save_off];
			ScreenAttrs[పునాది_off] =
				    saved_screen->attrs[పునాది_save_off];
			ScreenAttrs[పునాది_off] = hl_blend_attr(
				    ScreenAttrs[పునాది_off],
				    popup_attr_val, blend, TRUE);

			Screen_పంక్తులు[off] = 0;
			Screen_పంక్తులు_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[పునాది_off];

			popup_సమితి_పునాది_screen_cell(అడ్డువరుస, పునాది_నిలువు,
				    Screen_పంక్తులు[పునాది_off],
				    ScreenAttrs[పునాది_off],
				    Screen_పంక్తులు_UC[పునాది_off]);
			popup_సమితి_పునాది_screen_cell(అడ్డువరుస, నిలువు,
				    Screen_పంక్తులు[off],
				    ScreenAttrs[off],
				    Screen_పంక్తులు_UC[off]);
			screen_char(పునాది_off, అడ్డువరుస, పునాది_నిలువు);
			తిరిగి;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_పంక్తులు[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[పునాది_save_off];
		    ఒకవేళ (enc_utf8)
			Screen_పంక్తులు_UC[off] = 0;
		    పూర్ణాంకం popup_attr_val = get_win_attr(screen_opacity_popup);
		    పూర్ణాంకం blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_సమితి_పునాది_screen_cell(అడ్డువరుస, నిలువు, Screen_పంక్తులు[off],
					       ScreenAttrs[off], Screen_పంక్తులు_UC[off]);
		    screen_char(off, అడ్డువరుస, నిలువు);
		    తిరిగి;
		}

		// Base cell is inside the saved area, redraw the wide char.
		ఒకవేళ (save_off > 0)
		{
		    Screen_పంక్తులు[పునాది_off] = saved_screen->పంక్తులు[పునాది_save_off];
		    ScreenAttrs[పునాది_off] = saved_screen->attrs[పునాది_save_off];
		    Screen_పంక్తులు[off] = saved_screen->పంక్తులు[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_పంక్తులు_UC[పునాది_off] = saved_screen->linesuc[పునాది_save_off];
		    Screen_పంక్తులు_UC[off] = saved_screen->linesuc[save_off];

		    పూర్ణాంకం popup_attr_val = get_win_attr(screen_opacity_popup);
		    పూర్ణాంకం blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[పునాది_off] = hl_blend_attr(ScreenAttrs[పునాది_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[పునాది_off];
		    popup_సమితి_పునాది_screen_cell(అడ్డువరుస, పునాది_నిలువు, Screen_పంక్తులు[పునాది_off],
					       ScreenAttrs[పునాది_off], Screen_పంక్తులు_UC[పునాది_off]);
		    popup_సమితి_పునాది_screen_cell(అడ్డువరుస, నిలువు, Screen_పంక్తులు[off],
					       ScreenAttrs[off], Screen_పంక్తులు_UC[off]);
		    screen_char(పునాది_off, అడ్డువరుస, పునాది_నిలువు);
		}
		తిరిగి;
	    }
	}
	Screen_పంక్తులు[off] = saved_screen->పంక్తులు[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	ఒకవేళ (enc_utf8)
	    Screen_పంక్తులు_UC[off] = saved_screen->linesuc != ఖాళీ
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0
		&& utf_char2cells(Screen_పంక్తులు_UC[off]) == 2
		&& నిలువు + 1 >= pad_చివర_నిలువు)
	{
	    Screen_పంక్తులు[off] = ' ';
	    Screen_పంక్తులు_UC[off] = 0;
	}

	పూర్ణాంకం popup_attr_val = get_win_attr(screen_opacity_popup);
	పూర్ణాంకం blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_సమితి_పునాది_screen_cell(అడ్డువరుస, నిలువు, Screen_పంక్తులు[off],
				   ScreenAttrs[off], Screen_పంక్తులు_UC[off]);
	screen_char(off, అడ్డువరుస, నిలువు);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
స్థిర శూన్య
fill_opacity_padding(
	పూర్ణాంకం		ప్రారంభం_అడ్డువరుస,
	పూర్ణాంకం		చివర_అడ్డువరుస,
	పూర్ణాంకం		ప్రారంభం_నిలువు,
	పూర్ణాంకం		చివర_నిలువు,
	popup_saved_screen_T *saved_screen)
{
    కొరకు (పూర్ణాంకం pad_అడ్డువరుస = ప్రారంభం_అడ్డువరుస; pad_అడ్డువరుస < చివర_అడ్డువరుస; pad_అడ్డువరుస++)
	కొరకు (పూర్ణాంకం pad_నిలువు = ప్రారంభం_నిలువు; pad_నిలువు < చివర_నిలువు; pad_నిలువు++)
	    draw_opacity_padding_cell(pad_అడ్డువరుస, pad_నిలువు, saved_screen,
		    ప్రారంభం_నిలువు, చివర_నిలువు);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    శూన్య
update_popups(శూన్య (*win_update)(win_T *wp))
{
    win_T   *wp;
    పూర్ణాంకం	    శిఖరం_off;
    పూర్ణాంకం	    ఎడమ_extra;
    పూర్ణాంకం	    మొత్తం_వెడల్పు;
    పూర్ణాంకం	    మొత్తం_ఎత్తు_2;
    పూర్ణాంకం	    శిఖరం_padding;
    పూర్ణాంకం	    popup_attr;
    పూర్ణాంకం	    border_attr[4];
    పూర్ణాంకం	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    పూర్ణాంకం	    అడ్డువరుస;
    పూర్ణాంకం	    wincol;
    పూర్ణాంకం	    padcol = 0;
    పూర్ణాంకం	    padendcol = 0;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    sb_thumb_శిఖరం = 0;
    పూర్ణాంకం	    sb_thumb_ఎత్తు = 0;
    పూర్ణాంకం	    attr_scroll = 0;
    పూర్ణాంకం	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (పునాది_screenlines != ఖాళీ)
    {
	vim_free(పునాది_screenlines);
	పునాది_screenlines = ఖాళీ;
    }
    ఒకవేళ (పునాది_screenattrs != ఖాళీ)
    {
	vim_free(పునాది_screenattrs);
	పునాది_screenattrs = ఖాళీ;
    }
    ఒకవేళ (పునాది_screenlinesuc != ఖాళీ)
    {
	vim_free(పునాది_screenlinesuc);
	పునాది_screenlinesuc = ఖాళీ;
    }
    పునాది_screen_అడ్డువరుసలు = 0;
    పునాది_screen_నిలువుల_సంఖ్య = 0;
#అయితే_ముగింపు

    అయితే ((wp = కనుగొను_తదుపరి_popup(TRUE, POPUP_HANDLED_5)) != ఖాళీ)
    {
	పూర్ణాంకం	    title_len = 0;
	పూర్ణాంకం	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	లేకపోతే
	    screen_opacity_popup = ఖాళీ;

#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (screen_opacity_popup != ఖాళీ)
	{
	    ఒకవేళ (పునాది_screenlines != ఖాళీ)
	    {
		vim_free(పునాది_screenlines);
		పునాది_screenlines = ఖాళీ;
	    }
	    ఒకవేళ (పునాది_screenattrs != ఖాళీ)
	    {
		vim_free(పునాది_screenattrs);
		పునాది_screenattrs = ఖాళీ;
	    }
	    ఒకవేళ (పునాది_screenlinesuc != ఖాళీ)
	    {
		vim_free(పునాది_screenlinesuc);
		పునాది_screenlinesuc = ఖాళీ;
	    }

	    పునాది_screen_అడ్డువరుసలు = screen_అడ్డువరుసలు;
	    పునాది_screen_నిలువుల_సంఖ్య = screen_Columns;
	    పునాది_screenlines = ALLOC_MULT(schar_T,
				    పునాది_screen_అడ్డువరుసలు * పునాది_screen_నిలువుల_సంఖ్య);
	    పునాది_screenattrs = ALLOC_MULT(పూర్ణాంకం,
				    పునాది_screen_అడ్డువరుసలు * పునాది_screen_నిలువుల_సంఖ్య);
	    ఒకవేళ (enc_utf8)
		పునాది_screenlinesuc = ALLOC_MULT(u8char_T,
				    పునాది_screen_అడ్డువరుసలు * పునాది_screen_నిలువుల_సంఖ్య);

	    ఒకవేళ (పునాది_screenlines != ఖాళీ && పునాది_screenattrs != ఖాళీ)
	    {
		కొరకు (పూర్ణాంకం r = 0; r < పునాది_screen_అడ్డువరుసలు; r++)
		{
		    పూర్ణాంకం off = పంక్తి_స్థానభ్రంశం[r];
		    పూర్ణాంకం పునాది_off = r * పునాది_screen_నిలువుల_సంఖ్య;
		    కొరకు (పూర్ణాంకం c = 0; c < పునాది_screen_నిలువుల_సంఖ్య; c++)
		    {
			పునాది_screenlines[పునాది_off + c] = Screen_పంక్తులు[off + c];
			పునాది_screenattrs[పునాది_off + c] = ScreenAttrs[off + c];
			ఒకవేళ (enc_utf8 && పునాది_screenlinesuc != ఖాళీ)
			    పునాది_screenlinesuc[పునాది_off + c] =
					Screen_పంక్తులు_UC[off + c];
		    }
		}
	    }
	}
#అయితే_ముగింపు

	// Save background ScreenLines for padding opacity before win_update()
	// overwrites them.
	popup_saved_screen_T saved_screen = { 0 };

	popup_save_padding_screen(wp, &saved_screen);

	// Set flags in popup_transparent[] for masked cells.
	update_popup_transparent(wp, 1);

	// adjust w_winrow and w_wincol for border and padding, since
	// win_update() doesn't handle them.
	శిఖరం_off = popup_శిఖరం_extra(wp);
	ఎడమ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	ఒకవేళ (wp->w_wincol + ఎడమ_extra < 0)
	    ఎడమ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(శిఖరం_off - wp->w_popup_topoff, 0);
	wp->w_wincol += ఎడమ_extra;

	// Draw the popup text, unless it's off screen.
	ఒకవేళ (wp->w_winrow < screen_అడ్డువరుసలు && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    ఒకవేళ (wp->w_popup_చివరి_curline != wp->w_cursor.lnum)
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
	    ఒకవేళ (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    లేకపోతే ఒకవేళ (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(శిఖరం_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= ఎడమ_extra;

	// Add offset for border and padding if not done already.
	ఒకవేళ ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += ఎడమ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	ఒకవేళ ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += శిఖరం_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	ఒకవేళ (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    మొత్తం_వెడల్పు = cl.eff_వెడల్పు;
	లేకపోతే
	    మొత్తం_వెడల్పు = popup_వెడల్పు(wp) - wp->w_popup_rightoff;
	ఒకవేళ (మొత్తం_వెడల్పు < 0)
	    మొత్తం_వెడల్పు = 0;
	ఒకవేళ (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    మొత్తం_ఎత్తు_2 = cl.eff_ఎత్తు;
	లేకపోతే
	    మొత్తం_ఎత్తు_2 = popup_ఎత్తు(wp);
	popup_attr = get_win_attr(wp);

	ఒకవేళ (wp->w_winrow + మొత్తం_ఎత్తు_2 > cmdline_అడ్డువరుస)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	లేకపోతే
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	ఒకవేళ (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	లేకపోతే
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    కొరకు (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    ఒకవేళ (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	కొరకు (i = 0; i < 8; ++i)
	    ఒకవేళ (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	కొరకు (i = 0; i < 4; ++i)
	{
	    ఒకవేళ (wp->w_border_highlight[i] != ఖాళీ)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    లేకపోతే ఒకవేళ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    లేకపోతే
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	ఒకవేళ ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	ఒకవేళ (wp->w_popup_title != ఖాళీ)
	{
	    పూర్ణాంకం	    title_attr;

	    ఒకవేళ (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != ఖాళీ)
		title_attr = border_attr[0];
	    లేకపోతే ఒకవేళ (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    లేకపోతే
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    ఒకవేళ (title_len > మొత్తం_వెడల్పు - 2)
	    {
		పూర్ణాంకం	title_byte_len = (పూర్ణాంకం)STRLEN(wp->w_popup_title);
		char_u  *title_పాఠ్యం = alloc(title_byte_len + 1);

		ఒకవేళ (title_పాఠ్యం != ఖాళీ)
		{
		    trunc_string(wp->w_popup_title, title_పాఠ్యం,
					  మొత్తం_వెడల్పు - 2, title_byte_len + 1);
		    screen_puts(title_పాఠ్యం, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_పాఠ్యం);
		}

		title_len = మొత్తం_వెడల్పు - 2;
	    }
	    లేకపోతే
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	శిఖరం_padding = cl.eff_padding[0];
	ఒకవేళ (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    ఒకవేళ (title_len > 0)
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, title_wincol,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			title_wincol + title_len, wincol + మొత్తం_వెడల్పు,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    లేకపోతే
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + మొత్తం_వెడల్పు,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    ఒకవేళ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + మొత్తం_వెడల్పు - 1, border_attr[1]);
	    }
	}
	లేకపోతే ఒకవేళ (cl.eff_padding[0] == 0 && popup_శిఖరం_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    శిఖరం_padding = 1;

	ఒకవేళ (శిఖరం_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + మొత్తం_వెడల్పు - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    ఒకవేళ (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	ఒకవేళ (శిఖరం_padding > 0)
	{
	    అడ్డువరుస = wp->w_winrow + cl.eff_border[0];
	    ఒకవేళ (title_len > 0 && అడ్డువరుస == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		ఒకవేళ (screen_opacity_popup != ఖాళీ && saved_screen.పంక్తులు != ఖాళీ)
		{
		    // Left of title
		    fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		లేకపోతే
		{
		    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		అడ్డువరుస += 1;
		శిఖరం_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    ఒకవేళ (screen_opacity_popup != ఖాళీ && saved_screen.పంక్తులు != ఖాళీ)
	    {
		fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + శిఖరం_padding, padcol, padendcol,
			&saved_screen);
	    }
	    లేకపోతే
	    {
		screen_fill(అడ్డువరుస, అడ్డువరుస + శిఖరం_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	ఒకవేళ (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	    పూర్ణాంకం		ఎత్తు = wp->w_ఎత్తు;
	    పూర్ణాంకం		చివరి;

	    sb_thumb_ఎత్తు = ((linenr_T)ఎత్తు * ఎత్తు + linecount / 2)
								   / linecount;
	    ఒకవేళ (wp->w_topline > 1 && sb_thumb_ఎత్తు == ఎత్తు)
		--sb_thumb_ఎత్తు;  // scrolled, no full thumb
	    ఒకవేళ (sb_thumb_ఎత్తు == 0)
		sb_thumb_ఎత్తు = 1;
	    ఒకవేళ (linecount <= wp->w_ఎత్తు || wp->w_ఎత్తు == 0)
		// it just fits, avoid divide by zero
		sb_thumb_శిఖరం = 0;
	    లేకపోతే
		sb_thumb_శిఖరం = (wp->w_topline - 1
				+ (linecount / wp->w_ఎత్తు) / 2)
				* (wp->w_ఎత్తు - sb_thumb_ఎత్తు)
						  / (linecount - wp->w_ఎత్తు);
	    ఒకవేళ (wp->w_topline > 1 && sb_thumb_శిఖరం == 0 && ఎత్తు > 1)
		sb_thumb_శిఖరం = 1;  // show it's scrolled
	    చివరి = మొత్తం_ఎత్తు_2 - శిఖరం_off - wp->w_popup_border[2];
	    ఒకవేళ (sb_thumb_శిఖరం >= చివరి)
		// show at least one character
		sb_thumb_శిఖరం = చివరి - 1;

	    ఒకవేళ (wp->w_scrollbar_highlight != ఖాళీ)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    లేకపోతే
		attr_scroll = highlight_attr[HLF_PSB];
	    ఒకవేళ (wp->w_thumb_highlight != ఖాళీ)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    లేకపోతే
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	కొరకు (i = cl.eff_border[0]; i < మొత్తం_ఎత్తు_2 - cl.eff_border[2]; ++i)
	{
	    పూర్ణాంకం	pad_ఎడమ;
	    // left and right padding only needed next to the body
	    పూర్ణాంకం do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < మొత్తం_ఎత్తు_2 - cl.eff_border[2] - cl.eff_padding[2];

	    అడ్డువరుస = wp->w_winrow + i;

	    // left border
	    ఒకవేళ (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, అడ్డువరుస, wincol, border_attr[3]);
	    }
	    ఒకవేళ (do_padding && cl.eff_padding[3] > 0)
	    {
		పూర్ణాంకం నిలువు = wincol + wp->w_popup_border[3];

		// left padding
		pad_ఎడమ = wp->w_popup_padding[3];
		ఒకవేళ (నిలువు < 0)
		{
		    pad_ఎడమ += నిలువు;
		    నిలువు = 0;
		}
		ఒకవేళ (pad_ఎడమ > 0)
		{
		    ఒకవేళ (screen_opacity_popup != ఖాళీ && saved_screen.పంక్తులు != ఖాళీ)
			fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + 1, నిలువు, నిలువు + pad_ఎడమ,
				&saved_screen);
		    లేకపోతే
			screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు, నిలువు + pad_ఎడమ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    ఒకవేళ (wp->w_has_scrollbar)
	    {
		పూర్ణాంకం పంక్తి = i - శిఖరం_off;
		పూర్ణాంకం scroll_నిలువు = wp->w_wincol + మొత్తం_వెడల్పు - 1
						       - wp->w_popup_border[1];

		ఒకవేళ (పంక్తి >= 0 && పంక్తి < wp->w_ఎత్తు)
		    screen_putchar(' ', అడ్డువరుస, scroll_నిలువు,
			    పంక్తి >= sb_thumb_శిఖరం
				       && పంక్తి < sb_thumb_శిఖరం + sb_thumb_ఎత్తు
						  ? attr_thumb : attr_scroll);
		లేకపోతే
		    screen_putchar(' ', అడ్డువరుస, scroll_నిలువు, popup_attr);
	    }
	    // right border
	    ఒకవేళ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, అడ్డువరుస, wincol + మొత్తం_వెడల్పు - 1, border_attr[1]);
	    }
	    // right padding
	    ఒకవేళ (do_padding && cl.eff_padding[1] > 0)
	    {
		పూర్ణాంకం pad_నిలువు_ప్రారంభం = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_వెడల్పు;
		పూర్ణాంకం pad_నిలువు_చివర = pad_నిలువు_ప్రారంభం + wp->w_popup_padding[1];

		ఒకవేళ (screen_opacity_popup != ఖాళీ && saved_screen.పంక్తులు != ఖాళీ)
		    fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + 1, pad_నిలువు_ప్రారంభం, pad_నిలువు_చివర,
			    &saved_screen);
		లేకపోతే
		    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, pad_నిలువు_ప్రారంభం, pad_నిలువు_చివర,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	ఒకవేళ (wp->w_popup_shadow)
	{
	    పూర్ణాంకం నిలువు = wincol + మొత్తం_వెడల్పు;
	    కొరకు (i = 0; i < మొత్తం_ఎత్తు_2; ++i)
	    {
		అడ్డువరుస = wp->w_winrow + i + 1;
		put_shadow_char(అడ్డువరుస, నిలువు);
		put_shadow_char(అడ్డువరుస, నిలువు + 1);
	    }
	}

	ఒకవేళ (cl.eff_padding[2] > 0)
	{
	    // bottom padding -- sits right after the visible content rows.
	    // Derive the row from total_height so it always lands inside the
	    // popup's drawn extent, including the corner case where the top
	    // clip consumes more rows than the content itself (so the visible
	    // content height is zero).  A formula based on
	    // w_height - clip_top_content - clip_bot_content can go negative
	    // there and would draw the padding above w_winrow.
	    అడ్డువరుస = wp->w_winrow + మొత్తం_ఎత్తు_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    ఒకవేళ (screen_opacity_popup != ఖాళీ && saved_screen.పంక్తులు != ఖాళీ)
		fill_opacity_padding(అడ్డువరుస, అడ్డువరుస + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    లేకపోతే
		screen_fill(అడ్డువరుస, అడ్డువరుస + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	ఒకవేళ (cl.eff_border[2] > 0)
	{
	    // bottom border
	    అడ్డువరుస = wp->w_winrow + మొత్తం_ఎత్తు_2 - 1;
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + మొత్తం_వెడల్పు,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    ఒకవేళ (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, అడ్డువరుస, wincol + మొత్తం_వెడల్పు - 1, border_attr[2]);
	    }
	}

	ఒకవేళ (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    అడ్డువరుస = wp->w_winrow + మొత్తం_ఎత్తు_2;
	    కొరకు (పూర్ణాంకం నిలువు = 2 + (wincol < 0 ? 0 : wincol);
		    నిలువు < wincol + మొత్తం_వెడల్పు; నిలువు++)
		put_shadow_char(అడ్డువరుస, నిలువు);
	}

	ఒకవేళ (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + మొత్తం_వెడల్పు - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = ఖాళీ;

	// Back to the normal zindex.
	screen_zindex = 0;

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	// if this was the message window popup may start the timer now
	may_ప్రారంభం_సందేశం_win_timer(wp);
#అయితే_ముగింపు

	ఒకవేళ (override_success)
	    తీయి_highlight_overrides();

#నిర్వచితమైతే FEAT_IMAGE_POPUP
	popup_position_image(wp);
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_PROP_POPUP
    VIM_CLEAR(పునాది_screenlines);
    VIM_CLEAR(పునాది_screenattrs);
    VIM_CLEAR(పునాది_screenlinesuc);
    పునాది_screen_అడ్డువరుసలు = 0;
    పునాది_screen_నిలువుల_సంఖ్య = 0;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
    // In case win_update() called start_search_hl().
    చివర_శోధన_hl();
#అయితే_ముగింపు
}

/*
 * Mark references in callbacks of one popup window.
 */
    స్థిర పూర్ణాంకం
సమితి_ref_లోపల_one_popup(win_T *wp, పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    typval_T	tv;

    ఒకవేళ (wp->w_close_cb.cb_partial != ఖాళీ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
    }
    ఒకవేళ (wp->w_filter_cb.cb_partial != ఖాళీ)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
    }
    abort = abort || సమితి_ref_లోపల_list(wp->w_popup_ముసుగు, copyID);
    తిరిగి abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_popups(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    కొరకు (wp = మొదటి_popupwin; !abort && wp != ఖాళీ; wp = wp->w_తదుపరి)
	abort = abort || సమితి_ref_లోపల_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	కొరకు (wp = tp->tp_మొదటి_popupwin; !abort && wp != ఖాళీ; wp = wp->w_తదుపరి)
	    abort = abort || సమితి_ref_లోపల_one_popup(wp, copyID);
	ఒకవేళ (abort)
	    ఆపు;
    }
    తిరిగి abort;
}

    పూర్ణాంకం
popup_is_popup(win_T *wp)
{
    తిరిగి wp->w_popup_flags != 0;
}

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_కనుగొను_preview_కిటికీ(శూన్య)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_p_pvw)
	    తిరిగి wp;
    తిరిగి ఖాళీ;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_కనుగొను_info_కిటికీ(శూన్య)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_popup_flags & POPF_INFO)
	    తిరిగి wp;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(పూర్ణాంకం dir, bool by_పుట)
{
#నిర్వచితమైతే FEAT_QUICKFIX
    win_T	*wp = popup_కనుగొను_info_కిటికీ();
    పూర్ణాంకం		by;
    linenr_T	new_topline;

    ఒకవేళ (wp == ఖాళీ)
	తిరిగి false;

    by = by_పుట ? (wp->w_ఎత్తు > 2 ? wp->w_ఎత్తు - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    ఒకవేళ (new_topline < 1)
	new_topline = 1;
    ఒకవేళ (new_topline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	new_topline = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (new_topline != wp->w_topline)
    {
	సమితి_topline(wp, new_topline);
	popup_సమితి_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    తిరిగి true;
#లేకపోతే
    తిరిగి false;
#అయితే_ముగింపు
}

    శూన్య
f_popup_findecho(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
    rettv->vval.v_సంఖ్య = సందేశం_win_2 == ఖాళీ ? 0 : సందేశం_win_2->w_id;
#లేకపోతే
    rettv->vval.v_సంఖ్య = 0;
#అయితే_ముగింపు
}

    శూన్య
f_popup_findinfo(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితమైతే FEAT_QUICKFIX
    win_T   *wp = popup_కనుగొను_info_కిటికీ();

    rettv->vval.v_సంఖ్య = wp == ఖాళీ ? 0 : wp->w_id;
#లేకపోతే
    rettv->vval.v_సంఖ్య = 0;
#అయితే_ముగింపు
}

    శూన్య
f_popup_findpreview(typval_T *argvars వాడని, typval_T *rettv)
{
#నిర్వచితమైతే FEAT_QUICKFIX
    win_T   *wp = popup_కనుగొను_preview_కిటికీ();

    rettv->vval.v_సంఖ్య = wp == ఖాళీ ? 0 : wp->w_id;
#లేకపోతే
    rettv->vval.v_సంఖ్య = 0;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    పూర్ణాంకం
popup_create_preview_కిటికీ(పూర్ణాంకం info)
{
    win_T *wp = popup_create(ఖాళీ, ఖాళీ, info ? TYPE_INFO : TYPE_PREVIEW);

    ఒకవేళ (wp == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (info)
	wp->w_popup_flags |= POPF_INFO;
    లేకపోతే
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    ఒకవేళ (wp->w_minwidth > 0)
	wp->w_వెడల్పు = wp->w_minwidth;
    లేకపోతే ఒకవేళ (wp->w_maxwidth > 0)
	wp->w_వెడల్పు = wp->w_maxwidth;
    లేకపోతే
	wp->w_వెడల్పు = curwin->w_వెడల్పు;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_బఫరు->b_locked = FALSE;

    win_enter(wp, FALSE);
    తిరిగి OK;
}

/*
 * Close any preview popup.
 */
    శూన్య
popup_close_preview(శూన్య)
{
    win_T *wp = popup_కనుగొను_preview_కిటికీ();

    ఒకవేళ (wp != ఖాళీ)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    శూన్య
popup_hide_info(శూన్య)
{
    win_T *wp = popup_కనుగొను_info_కిటికీ();

    ఒకవేళ (wp != ఖాళీ)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    శూన్య
popup_close_info(శూన్య)
{
    win_T *wp = popup_కనుగొను_info_కిటికీ();

    ఒకవేళ (wp != ఖాళీ)
	popup_close_with_retval(wp, -1);
}
#అయితే_ముగింపు

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    పూర్ణాంకం
popup_overlaps_cmdline(శూన్య)
{
    తిరిగి popup_on_cmdline;
}

#ఒకవేళ defined(HAS_సందేశం_కిటికీ)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_సందేశం_win(శూన్య)
{
    ఒకవేళ (సందేశం_win_2 != ఖాళీ)
	తిరిగి సందేశం_win_2;

    పూర్ణాంకం i;

    సందేశం_win_2 = popup_create(ఖాళీ, ఖాళీ, TYPE_సందేశం_WIN);

    ఒకవేళ (సందేశం_win_2 == ఖాళీ)
	తిరిగి ఖాళీ;

    // use the full screen width
    సందేశం_win_2->w_వెడల్పు = Columns;

    // position at bottom of screen
    సందేశం_win_2->w_popup_pos = POPPOS_BOTTOM;
    సందేశం_win_2->w_wantcol = 1;
    సందేశం_win_2->w_minwidth = 9999;
    సందేశం_win_2->w_firstline = -1;

    // no padding, border at the top
    కొరకు (i = 0; i < 4; ++i)
	సందేశం_win_2->w_popup_padding[i] = 0;
    కొరకు (i = 1; i < 4; ++i)
	సందేశం_win_2->w_popup_border[i] = 0;

    ఒకవేళ (సందేశం_win_2->w_popup_timer != ఖాళీ)
	సందేశం_win_2->w_popup_timer->tr_keep = TRUE;
    తిరిగి సందేశం_win_2;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    శూన్య
popup_show_సందేశం_win(శూన్య)
{
    ఒకవేళ (సందేశం_win_2 == ఖాళీ)
	తిరిగి;

    ఒకవేళ ((సందేశం_win_2->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(సందేశం_win_2, TYPE_సందేశం_WIN);
	popup_show(సందేశం_win_2);
    }
    ప్రారంభం_సందేశం_win_timer = TRUE;
}

    స్థిర శూన్య
may_ప్రారంభం_సందేశం_win_timer(win_T *wp)
{
    ఒకవేళ (wp == సందేశం_win_2 && ప్రారంభం_సందేశం_win_timer)
    {
	ఒకవేళ (సందేశం_win_2->w_popup_timer != ఖాళీ)
	{
	    సందేశం_win_2->w_popup_timer->tr_interval = సందేశం_win_సమయం;
	    timer_ప్రారంభం(సందేశం_win_2->w_popup_timer);
	    సందేశం_win_సమయం = 3000;
	}
	ప్రారంభం_సందేశం_win_timer = FALSE;
    }
}

    పూర్ణాంకం
popup_సందేశం_win_visible(శూన్య)
{
    తిరిగి సందేశం_win_2 != ఖాళీ
	&& (సందేశం_win_2->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    శూన్య
popup_hide_సందేశం_win(శూన్య)
{
    ఒకవేళ (సందేశం_win_2 != ఖాళీ)
	popup_hide(సందేశం_win_2);
}

// Values saved in start_echowindow() and restored in end_echowindow()
స్థిర పూర్ణాంకం save_సందేశం_didout = FALSE;
స్థిర పూర్ణాంకం save_సందేశం_నిలువు = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
స్థిర పూర్ణాంకం ew_సందేశం_didout = FALSE;
స్థిర పూర్ణాంకం ew_సందేశం_నిలువు = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    శూన్య
ప్రారంభం_echowindow(పూర్ణాంకం సమయం_sec)
{
    లోపల_echowindow = TRUE;
    save_సందేశం_didout = సందేశం_didout;
    save_సందేశం_నిలువు = సందేశం_నిలువు;
    సందేశం_didout = ew_సందేశం_didout;
    సందేశం_నిలువు = ew_సందేశం_నిలువు;
    ఒకవేళ (సమయం_sec != 0)
	సందేశం_win_సమయం = సమయం_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    శూన్య
చివర_echowindow(శూన్య)
{
    లోపల_echowindow = FALSE;

    ఒకవేళ ((State & బహుళకం_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_సందేశం_didout = TRUE;
    ew_సందేశం_నిలువు = సందేశం_నిలువు == 0 ? 1 : సందేశం_నిలువు;
    సందేశం_didout = save_సందేశం_didout;
    సందేశం_నిలువు = save_సందేశం_నిలువు;
}
#అయితే_ముగింపు

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    పూర్ణాంకం
popup_win_closed(win_T *win)
{
    పూర్ణాంకం	    విడత;
    win_T   *wp;
    win_T   *తదుపరి;
    పూర్ణాంకం	    ret = FALSE;

    కొరకు (విడత = 1; విడత <= 2; ++విడత)
	కొరకు (wp = విడత == 1 ? మొదటి_popupwin : curtab->tp_మొదటి_popupwin;
							 wp != ఖాళీ; wp = తదుపరి)
	{
	    తదుపరి = wp->w_తదుపరి;
	    ఒకవేళ (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    తిరిగి ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    శూన్య
popup_సమితి_title(win_T *wp)
{
    ఒకవేళ (wp->w_బఫరు->b_fname == ఖాళీ)
	తిరిగి;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_బఫరు, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_బఫరు->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    ఒకవేళ (wp->w_popup_title != ఖాళీ)
	vim_snprintf((అక్షరం *)wp->w_popup_title, len, " %s ",
		wp->w_బఫరు->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    శూన్య
popup_update_preview_title(శూన్య)
{
    win_T *wp = popup_కనుగొను_preview_కిటికీ();

    ఒకవేళ (wp != ఖాళీ)
	popup_సమితి_title(wp);
}
#అయితే_ముగింపు

#అయితే_ముగింపు // FEAT_PROP_POPUP
