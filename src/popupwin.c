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

#include "vim.h"

#якщо defined(FEAT_PROP_POPUP)

typedef struct {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
typedef struct {
    ціле		active;
    ціле		winrow;
    ціле		wincol;
    ціле		height;
    ціле		width;
    ціле		leftoff;
    ціле		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
typedef struct {
    schar_T	*lines;
    ціле		*attrs;
    u8char_T	*linesuc;
    ціле		start_row;
    ціле		start_col;
    ціле		rows;
    ціле		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
typedef struct {
    linenr_T	firstline;
    ціле		blend;
    ціле		flags;
    ціле		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
typedef struct {
    ціле		winrow;
    ціле		wincol;
    ціле		width;
    ціле		height;
    ціле		leftcol;
    ціле		leftoff;
    ціле		has_scrollbar;
    ціле		topoff;
    ціле		bottomoff;
    ціле		leftclip;
    ціле		rightclip;
} popup_layout_T;

static poppos_entry_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#ifdef HAS_MESSAGE_WINDOW
// Window used for ":echowindow"
static win_T *message_win = NULL;

// Time used for the next ":echowindow" message in msec.
static ціле  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
static ціле    start_message_win_timer = FALSE;

static void may_start_message_win_timer(win_T *wp);
#endif

static ціле popup_on_cmdline = FALSE;

static void popup_adjust_position(win_T *wp);
static bool popup_area_changed(win_T *wp, popup_area_T *area);
static void popup_redraw_exposed_area(popup_area_T *area);
static void popup_save_area(win_T *wp, popup_area_T *area);
static void popup_free_saved_screen(popup_saved_screen_T *saved_screen);
static void popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
static bool popup_layout_changed(win_T *wp, popup_layout_T *layout);
static bool popup_style_changed(win_T *wp, popup_style_snapshot_T *style);
static void popup_save_style(win_T *wp, popup_style_snapshot_T *style);
static void popup_save_layout(win_T *wp, popup_layout_T *layout);
static void redraw_under_popup_area(ціле winrow, ціле wincol, ціле height,
	ціле width, ціле leftoff);
static void redraw_overlapped_opacity_popups(ціле winrow, ціле wincol,
	ціле height, ціле width, ціле leftoff, ціле zindex);
static void redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    static ціле
popup_options_one(dict_T *dict, char_u *key)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    ціле		n = 0;

    di = dict_find(dict, key, -1);
    якщо (di == NULL)
	повернути MAXCOL;

    val = tv_get_string(&di->di_tv);
    якщо (STRNCMP(val, "cursor", 6) != 0)
	повернути dict_get_number_check(dict, key);

    setcursor_mayforce(TRUE);
    s = val + 6;
    якщо (*s != NUL)
    {
	endp = s;
	якщо (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((char *)s, (char **)&endp, 10);
	якщо (endp != NULL && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    повернути 0;
	}
    }

    якщо (STRCMP(key, "line") == 0)
	n = screen_screenrow() + 1 + n;
    інакше // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    якщо (n == 0)
	n = -1;
    повернути n;
}

    static ціле
set_padding_border(dict_T *dict, ціле *array, char *name, ціле max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    якщо (di == NULL)
	повернути OK;

    якщо (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	повернути FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    ціле		i;
    ціле		nr;

    для (i = 0; i < 4; ++i)
	array[i] = 1;
    якщо (list == NULL)
	повернути OK;

    CHECK_LIST_MATERIALIZE(list);
    для (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (ціле)tv_get_number(&li->li_tv);
	якщо (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    повернути OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    static void
set_moved_values(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.col;
    wp->w_popup_maxcol = curwin->w_cursor.col;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    static void
set_moved_columns(win_T *wp, ціле flags)
{
    char_u	*ptr;
    ціле		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    ціле		mincol;

    якщо (len <= 0)
	повернути;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (ціле)(ptr - ml_get_curline());
    якщо (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	повернути;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    static void
set_mousemoved_values(win_T *wp)
{
    wp->w_popup_mouse_row = mouse_row;
    wp->w_popup_mouse_mincol = mouse_col;
    wp->w_popup_mouse_maxcol = mouse_col;
}

    static void
update_popup_uses_mouse_move(void)
{
    popup_uses_mouse_move = FALSE;
    якщо (!popup_visible)
	повернути;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	якщо (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    повернути;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    повернути;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    static void
set_mousemoved_columns(win_T *wp, ціле flags)
{
    win_T	*textwp;
    char_u	*text;
    ціле		col;
    pos_T	pos;
    colnr_T	mcol;

    якщо (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, NULL, &col) != OK)
	повернути;

    // convert text column to mouse column
    pos.col = col;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, NULL, NULL, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.col = col + (colnr_T)STRLEN(text) - 1;
    getvcol(textwp, &pos, NULL, NULL, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(text);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    ціле
popup_on_border(win_T *wp, ціле row, ціле col)
{
    повернути (row == 0 && wp->w_popup_border[0] > 0)
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
    ціле
popup_close_if_on_X(win_T *wp, ціле row, ціле col)
{
    якщо (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	повернути TRUE;
    }
    повернути FALSE;
}

// Values set when dragging a popup window starts.
static ціле drag_start_row;
static ціле drag_start_col;
static ціле drag_start_wantline;
static ціле drag_start_wantcol;
static ціле drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    void
popup_start_drag(win_T *wp, ціле row, ціле col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    якщо (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    інакше
	drag_start_wantline = wp->w_wantline;
    якщо (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    інакше
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    якщо (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    якщо (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	якщо (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	якщо (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    void
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    якщо (!win_valid_popup(wp))
	повернути;

    якщо ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	ціле width_inc = mouse_col - drag_start_col;
	ціле height_inc = mouse_row - drag_start_row;

	якщо (width_inc != 0)
	{
	    ціле width = wp->w_width + width_inc;

	    якщо (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	якщо (height_inc != 0)
	{
	    ціле height = wp->w_height + height_inc;

	    якщо (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	повернути;
    }

    якщо (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	повернути;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    якщо (wp->w_wantline < 1)
	wp->w_wantline = 1;
    якщо (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    якщо (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    якщо (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    void
popup_set_firstline(win_T *wp)
{
    ціле	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    поки (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    ціле
popup_is_in_scrollbar(win_T *wp, ціле row, ціле col)
{
    повернути wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    void
popup_handle_scrollbar_click(win_T *wp, ціле row, ціле col)
{
    якщо (!popup_is_in_scrollbar(wp, row, col))
	повернути;

    ціле	    height = popup_height(wp);
    ціле	    new_topline = wp->w_topline;

    якщо (row >= height / 2)
    {
	// Click in lower half, scroll down.
	якщо (wp->w_topline < wp->w_buffer->b_ml.ml_line_count)
	    ++new_topline;
    }
    інакше якщо (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    якщо (new_topline == wp->w_topline)
	повернути;

    set_topline(wp, new_topline);
    якщо (wp == curwin)
    {
	якщо (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	інакше якщо (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#якщо defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    static void
popup_add_timeout(win_T *wp, ціле time, ціле close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((char *)cbbuf, sizeof(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    якщо (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	повернути;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    якщо (cb.cb_name != NULL && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    static poppos_T
get_pos_entry(dict_T *d, ціле give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    ціле	    nr;

    якщо (str == NULL)
	повернути POPPOS_NONE;

    для (nr = 0; nr < (ціле)ARRAY_LENGTH(poppos_entries); ++nr)
	якщо (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    повернути poppos_entries[nr].pp_val;

    якщо (give_error)
	semsg(_(e_invalid_argument_str), str);
    повернути POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    static void
apply_move_options(win_T *wp, dict_T *d)
{
    ціле		nr;
    char_u	*str;
    dictitem_T	*di;

    якщо ((nr = dict_get_number_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    якщо ((nr = dict_get_number_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    якщо ((nr = dict_get_number_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    якщо ((nr = dict_get_number_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    якщо (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    якщо (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    якщо (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	якщо (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    якщо (str != NULL)
    {
	wp->w_popup_prop_type = 0;
	якщо (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    якщо (di != NULL)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		якщо (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    якщо (nr <= 0)
		nr = find_prop_type_id(str, NULL);
	    якщо (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    інакше
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    якщо (di != NULL)
	wp->w_popup_prop_id = dict_get_number(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    static void
handle_moved_argument(win_T *wp, dictitem_T *di, ціле mousemoved)
{
    якщо (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != NULL)
    {
	char_u  *s = di->di_tv.vval.v_string;
	ціле	flags = 0;

	якщо (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	інакше якщо (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	інакше якщо (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	інакше якщо (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	якщо (flags != 0)
	{
	    якщо (mousemoved)
		set_mousemoved_columns(wp, flags);
	    інакше
		set_moved_columns(wp, flags);
	}
    }
    інакше якщо (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != NULL
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	ціле	    mincol;
	ціле	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	якщо (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_number(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    якщо (mousemoved)
		wp->w_popup_mouse_row = nr;
	    інакше
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    якщо (nr == 0)
		wp->w_popup_curwin = NULL;
	}

	mincol = tv_get_number(&li->li_tv);
	maxcol = tv_get_number(&li->li_next->li_tv);
	якщо (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	інакше
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    інакше
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    static void
check_highlight(dict_T *dict, char *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    якщо (di == NULL)
	повернути;

    якщо (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_value_for_argument_str), name);
    інакше
    {
	str = tv_get_string(&di->di_tv);
	якщо (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    static void
popup_show_curline(win_T *wp)
{
    якщо (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    інакше якщо (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	якщо (wp->w_topline < 1)
	    wp->w_topline = 1;
	інакше якщо (wp->w_topline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_topline = wp->w_buffer->b_ml.ml_line_count;
	поки (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_line_count
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    якщо (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    static char_u *
popup_get_sign_name(win_T *wp)
{
    static char    buf[30];

    vim_snprintf(buf, sizeof(buf), "popup-%d", wp->w_id);
    повернути (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    static void
popup_highlight_curline(win_T *wp)
{
    ціле	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    якщо ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	якщо (!sign_exists_by_name(sign_name))
	{
	    char *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, NULL, (char_u *)linehl, NULL, NULL, NULL,
		    NULL, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    інакше
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    static ціле
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    ціле		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    якщо (di != NULL)
    {
	wp->w_firstline = dict_get_number(dict, "firstline");
	якщо (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    якщо (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    якщо (str != NULL)
    {
	char_u	*title = vim_strsave(str);

	якщо (title != NULL)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    якщо (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	інакше
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	інакше
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	інакше
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	інакше
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	інакше
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    якщо (di != NULL)
    {
	nr = dict_get_number(dict, "opacity");
	якщо (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	інакше якщо (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	інакше якщо (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	інакше
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    якщо (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    інакше
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    якщо (di != NULL && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != NULL)
    {
	image_T		    *img;
	image_T		    *cur_img = NULL;
	image_placement_T   *place;
	ціле		    cw, ch;

	якщо (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    якщо (wp->w_popup_imagep != NULL)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = NULL;
	    }
	    goto image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	якщо (wp->w_popup_imagep != NULL && wp->w_popup_imagep->img != NULL)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	якщо (img == NULL)
	    повернути FAIL;

	якщо (img == cur_img)
	    goto image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	якщо (place == NULL)
	{
	    image_unref(img);
	    повернути FAIL;
	}

	якщо (wp->w_popup_imagep != NULL)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	якщо (dict_find(dict, (char_u *)"minwidth", -1) == NULL)
	    wp->w_minwidth = cw;
	якщо (dict_find(dict, (char_u *)"maxwidth", -1) == NULL)
	    wp->w_maxwidth = cw;
	якщо (dict_find(dict, (char_u *)"minheight", -1) == NULL)
	    wp->w_minheight = ch;
	якщо (dict_find(dict, (char_u *)"maxheight", -1) == NULL)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    інакше
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	якщо (wp->w_popup_imagep != NULL)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(NULL, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	якщо (wp->w_popup_imagep != NULL)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    якщо (di != NULL)
    {
	ціле ok = TRUE;

	якщо (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != NULL)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    якщо (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    інакше якщо (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    інакше якщо (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    інакше
		ok = FALSE;
	}
	інакше
	    ok = FALSE;
	якщо (!ok)
	    semsg(_(e_invalid_value_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    якщо (str != NULL)
    {
	char *errmsg = update_wincolor(wp, str);

	якщо (errmsg == NULL)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	інакше
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    якщо (str != NULL)
    {
	char *errmsg = update_winhighlight(wp, str);

	якщо (errmsg == NULL)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	інакше
	    emsg(_(errmsg));
    }

    якщо (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	повернути FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    якщо (di != NULL)
    {
	якщо (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    повернути FAIL;
	}
	інакше
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ціле		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    якщо (list->lv_len == 0)
	    {
		для (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = NULL;
		}
	    }
	    інакше
	    {
		для (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    якщо (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			якщо (hl != NULL)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		якщо (list->lv_len == 1 && wp->w_border_highlight[0] != NULL)
		    для (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			якщо (hl != NULL)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    якщо (di != NULL)
    {
	якщо (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    повернути FAIL;
	}
	інакше
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    ціле		i;

	    якщо (list != NULL)
	    {
		CHECK_LIST_MATERIALIZE(list);
		для (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    якщо (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		якщо (list->lv_len == 1)
		    для (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		якщо (list->lv_len == 2)
		{
		    для (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    для (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    якщо (di != NULL)
    {
	wp->w_zindex = dict_get_number(dict, "zindex");
	якщо (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	якщо (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    якщо (di != NULL)
    {
	ціле ok = FALSE;

	якщо (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != NULL)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ITEMS(di->di_tv.vval.v_list, li)
	    {
		якщо (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == NULL
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    break;
		}
		інакше
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	якщо (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	інакше
	{
	    semsg(_(e_invalid_value_for_argument_str), "mask");
	    повернути FAIL;
	}
    }

#якщо defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_number(dict, "time");
    якщо (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    якщо (di != NULL)
    {
	set_moved_values(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    якщо (di != NULL)
    {
	set_mousemoved_values(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    якщо (nr != -1)
    {
	якщо (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	інакше
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    якщо (di != NULL)
    {
	callback_T	callback = get_callback(&di->di_tv);

	якщо (callback.cb_name != NULL)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    якщо (nr != -1)
    {
	якщо (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	інакше
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    якщо (str != NULL)
    {
	якщо (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	інакше
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    якщо (di == NULL)
	повернути OK;

    callback_T	callback = get_callback(&di->di_tv);
    якщо (callback.cb_name == NULL)
	повернути OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    повернути OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    static ціле
apply_options(win_T *wp, dict_T *dict, ціле create)
{
    ціле		nr;

    apply_move_options(wp, dict);

    якщо (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    якщо (apply_general_options(wp, dict) == FAIL)
	повернути FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    якщо (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    якщо (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	якщо (wp->w_firstline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_line_count;
	інакше якщо (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    якщо (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    повернути OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    static void
add_popup_strings(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_ITEMS(l, li)
	якщо (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == NULL ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    static void
add_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_ITEMS(l, li)
    {
	якщо (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    повернути;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == NULL ? NULL : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == NULL ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    для (li = l->lv_first; li != NULL; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	якщо (di != NULL)
	{
	    якщо (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		повернути;
	    }
	    plist = di->di_tv.vval.v_list;
	    якщо (plist != NULL)
	    {
		FOR_ALL_LIST_ITEMS(plist, pli)
		{
		    якщо (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			повернути;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    якщо (dict != NULL)
		    {
			ціле col = dict_get_number(dict, "col");

			prop_add_common(lnum, col, dict, buf, NULL);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    ціле
popup_top_extra(win_T *wp)
{
    ціле	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    якщо (extra == 0 && wp->w_popup_title != NULL && *wp->w_popup_title != NUL)
	повернути 1;
    повернути extra;
}

/*
 * Get the padding plus border at the left.
 */
    ціле
popup_left_extra(win_T *wp)
{
    повернути wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    ціле
popup_height(win_T *wp)
{
    повернути wp->w_height
	    + popup_top_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    ціле
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    повернути wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    ціле
popup_extra_width(win_T *wp)
{
    повернути wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    static win_T *
popup_get_clipwin(win_T *wp)
{
    якщо (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	повернути NULL;
    якщо (win_valid(wp->w_popup_prop_win))
	повернути wp->w_popup_prop_win;
    повернути NULL;
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
typedef struct {
    ціле top_extra;
    ціле bot_extra;
    ціле left_extra;
    ціле right_extra;

    ціле clip_top_content;
    ціле clip_bot_content;
    ціле clip_left_content;
    ціле clip_right_content;

    ціле eff_top_extra;
    ціле eff_bot_extra;
    ціле eff_left_extra;
    ціле eff_right_extra;

    ціле eff_border[4];
    ціле eff_padding[4];

    ціле eff_height;
    ціле eff_width;
} popup_clip_T;

    static void
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    ціле h, w;

    cl->top_extra = popup_top_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->left_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->right_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_top_content = wp->w_popup_topoff - cl->top_extra;
    якщо (cl->clip_top_content < 0)
	cl->clip_top_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    якщо (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_left_content = wp->w_popup_leftclip - cl->left_extra;
    якщо (cl->clip_left_content < 0)
	cl->clip_left_content = 0;
    cl->clip_right_content = wp->w_popup_rightclip - cl->right_extra;
    якщо (cl->clip_right_content < 0)
	cl->clip_right_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	ціле clip = wp->w_popup_topoff;
	ціле b = wp->w_popup_border[0];
	ціле p = wp->w_popup_padding[0];
	ціле rem;

	якщо (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	інакше
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	ціле clip = wp->w_popup_bottomoff;
	ціле b = wp->w_popup_border[2];
	ціле p = wp->w_popup_padding[2];
	ціле rem;

	якщо (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	інакше
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
	ціле excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	якщо (excess > 0)
	{
	    якщо (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		якщо (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		інакше
		    cl->eff_border[0] -= excess;
	    }
	    інакше
		cl->eff_padding[0] -= excess;
	}
    }
    {
	ціле excess = wp->w_popup_topoff - cl->top_extra - wp->w_height;
	якщо (excess > 0)
	{
	    якщо (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		якщо (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		інакше
		    cl->eff_border[2] -= excess;
	    }
	    інакше
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_top_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_left_extra = wp->w_popup_leftclip > 0 ? 0 : cl->left_extra;
    cl->eff_right_extra = wp->w_popup_rightclip > 0 ? 0 : cl->right_extra;

    h = wp->w_height - cl->clip_top_content - cl->clip_bot_content;
    якщо (h < 0)
	h = 0;
    cl->eff_height = cl->eff_top_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_left_content - cl->clip_right_content;
    якщо (w < 0)
	w = 0;
    cl->eff_width = cl->eff_left_extra + w + cl->eff_right_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
typedef struct {
    ціле		height;
    ціле		width;
    ціле		winrow;
    ціле		wincol;
    ціле		leftcol;
    ціле		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    static void
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

    static void
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
    static void
popup_screenpos_above_top(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	ціле	    *screen_row,
	ціле	    *screen_scol,
	ціле	    *screen_ccol,
	ціле	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    ціле	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (ціле)win_col_off(prop_win) - (ціле)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (ціле)scol + coloff;
    *screen_ccol = (ціле)ccol + coloff;
    *screen_ecol = (ціле)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (ціле)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    static void
popup_hide_for_textprop(win_T *wp)
{
    якщо ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	повернути;
    wp->w_popup_flags |= POPF_HIDDEN;
    якщо (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    static bool
popup_find_prop_above_top(
	win_T	    *wp,
	win_T	    *prop_win,
	ціле	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    якщо (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	повернути false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    якщо (first < 1)
	first = 1;
    повернути find_prop_in_lines(prop_win,
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
    static bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    ціле	    extra_h, extra_w;
    ціле	    popup_top, popup_bottom, popup_left, popup_right;
    ціле	    total_h, total_w;

    якщо (cw == NULL)
	повернути false;

    extra_h = popup_top_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_top = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_left = wp->w_wincol;
    popup_right = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    якщо (popup_top < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_top;
    якщо (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    якщо (popup_left < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_left;
    якщо (popup_right > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_right - (cw->w_wincol + cw->w_width);

    повернути wp->w_popup_topoff >= total_h
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
    static void
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    якщо (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_top_content + cl->clip_bot_content;
	якщо (wp->w_height < 0)
	    wp->w_height = 0;
	якщо (cl->clip_top_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_top_content;
    }
    якщо (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	якщо (cl->clip_left_content > 0 || cl->clip_right_content > 0)
	    wp->w_p_wrap = FALSE;
	якщо (cl->clip_right_content > 0)
	{
	    wp->w_width -= cl->clip_right_content;
	    якщо (wp->w_width < 0)
		wp->w_width = 0;
	}
	якщо (cl->clip_left_content > 0)
	{
	    wp->w_leftcol += cl->clip_left_content;
	    wp->w_wincol += cl->clip_left_content;
	    wp->w_width -= cl->clip_left_content;
	    якщо (wp->w_width < 0)
		wp->w_width = 0;
	}
    }
}

#ifdef FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    static void
popup_position_image(win_T *wp)
{
    linenr_T	    row;
    colnr_T	    col;

    ціле iw, ih;

    // Visible cell width and height of popup itself (not image!)
    ціле	visible_width;
    ціле	visible_height;

    // Crop dimensions of image (in pixels).
    ціле	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    якщо (wp->w_popup_imagep == NULL || wp->w_popup_flags & POPF_HIDDEN)
	повернути;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    якщо (wp->w_popup_imagep->img == NULL)
	// Just set the bounding box
	goto exit;

    visible_width =
	wp->w_width - clip.clip_left_content - clip.clip_right_content;
    visible_height =
	wp->w_height - clip.clip_top_content - clip.clip_bot_content;

    якщо (visible_width < 0 || visible_height < 0)
	повернути;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_left_content;
    crop_y = clip.clip_top_content;
    crop_width = iw - crop_x - clip.clip_right_content;
    crop_height = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    row -= wp->w_popup_border[0] - clip.eff_border[0];
    row -= wp->w_popup_padding[0] - clip.eff_padding[0];

    col -= wp->w_popup_border[3] - clip.eff_border[3];
    col -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    якщо (crop_width > visible_width)
	crop_width = visible_width;
    якщо (crop_height > visible_height)
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
    static void
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    ціле		wrapped = 0;
    ціле		maxwidth;
    ціле		maxwidth_no_scrollbar;
    ціле		width_with_scrollbar = 0;
    ціле		used_maxwidth = FALSE;
    ціле		margin_width = 0;
    ціле		maxspace;
    ціле		center_vert = FALSE;
    ціле		center_hor = FALSE;
    ціле		allow_adjust_left = !wp->w_popup_fixed;
    ціле		top_extra = popup_top_extra(wp);
    ціле		right_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    ціле		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    ціле		left_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    ціле		extra_height = top_extra + bot_extra;
    ціле		extra_width = left_extra + right_extra;
    ціле		w_height_before_limit;
    popup_layout_T org_layout;
    ціле		minwidth, minheight;
    ціле		maxheight = Rows;
    ціле		wantline = wp->w_wantline;  // adjusted for textprop
    ціле		wantcol = wp->w_wantcol;    // adjusted for textprop
    ціле		use_wantcol = wantcol != 0;
    ціле		adjust_height_for_top_aligned = FALSE;

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
    якщо (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    якщо (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	ціле	    screen_row;
	ціле	    screen_scol;
	ціле	    screen_ccol;
	ціле	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_top = false;
	якщо (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    якщо (popup_find_prop_above_top(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_top = true;
	    інакше
	    {
		якщо ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    якщо (wp->w_popup_imagep != NULL)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    якщо (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		повернути;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	якщо (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	якщо (prop_above_top)
	    popup_screenpos_above_top(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	інакше
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	якщо (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    повернути;
	}
	якщо (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	інакше
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	якщо (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	інакше
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    інакше
    {
	// If no line was specified default to vertical centering.
	якщо (wantline == 0)
	    center_vert = TRUE;
	інакше якщо (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	якщо (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    якщо (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    інакше
    {
	якщо (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    якщо (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	якщо (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_line_count - 1, 0);
	}

	якщо (!use_wantcol)
	    center_hor = TRUE;
	інакше якщо (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    якщо (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- left_extra - right_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- left_extra - right_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    якщо (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - left_extra - right_extra;
    maxwidth = maxspace;
    якщо (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_left = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    якщо (wp->w_p_nu || wp->w_p_rnu)
	margin_width = number_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    якщо (signcolumn_on(wp))
	margin_width += 2;
#endif
    якщо (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    якщо (wp->w_buffer->b_term != NULL)
    {
	якщо (minwidth == 0)
	    minwidth = 20;
	якщо (minheight == 0)
	    minheight = 5;
    }
#endif

    якщо (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    інакше якщо (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    якщо (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	якщо (wp->w_firstline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_firstline = 0;
	інакше
	    wp->w_topline = wp->w_firstline;
    }
    якщо (wp->w_topline < 1)
	wp->w_topline = 1;
    інакше якщо (wp->w_topline > wp->w_buffer->b_ml.ml_line_count)
	wp->w_topline = wp->w_buffer->b_ml.ml_line_count;

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
	ціле saved_w_width = wp->w_width;

	якщо (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	для (ln = 1; ln <= wp->w_buffer->b_ml.ml_line_count; ++ln)
	{
	    ціле len = linetabsize(wp, ln) + margin_width;

	    якщо (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    якщо (saved_w_width < len)
		saved_w_width = len;
	    якщо (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		break;
	}
	wp->w_width = saved_w_width;
    }
    якщо (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_line_count;
    інакше
	lnum = wp->w_topline;
    поки (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_line_count)
    {
	ціле len;
	ціле w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	якщо (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	якщо (len + margin_width > maxwidth
		&& allow_adjust_left
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    ціле shift_by = len + margin_width - maxwidth;

	    якщо (shift_by > wp->w_wincol)
	    {
		ціле truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    якщо (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    якщо (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	якщо (wp->w_p_wrap)
	{
	    поки (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	якщо (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    якщо (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	якщо (wp->w_firstline < 0)
	    --lnum;
	інакше
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	якщо (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_line_count - lnum)
		       + wrapped >= maxheight)
	    break;
    }

    якщо (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_line_count);
#ifdef FEAT_TERMINAL
    якщо (wp->w_buffer->b_term != NULL && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    якщо (wp->w_has_scrollbar)
    {
	++right_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	якщо (maxwidth + right_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    якщо (wp->w_popup_title != NULL && *wp->w_popup_title != NUL)
    {
	ціле title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	якщо (minwidth < title_len)
	    minwidth = title_len;
    }

    якщо (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    якщо (wp->w_width > maxwidth)
    {
	якщо (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	якщо (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    ціле off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, right_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	інакше
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - right_extra;
	}
    }
    якщо (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	якщо (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    інакше якщо (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	ціле leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	якщо (leftoff >= 0)
	    wp->w_wincol = leftoff;
	інакше якщо (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    якщо (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    якщо (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	ціле want_col = 0;

	// try to show the right border and any scrollbar
	want_col = left_extra + wp->w_width + right_extra;
	якщо (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    якщо (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_line_count - wp->w_topline
								 + 1 + wrapped;
    якщо (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    якщо (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    якщо (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    якщо (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	якщо (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    інакше якщо (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	якщо ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	інакше якщо (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	інакше
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_top_aligned = TRUE;
	}
    }
    інакше якщо (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	ціле check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	якщо (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    якщо (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    якщо (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	інакше
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_top_aligned = TRUE;
	}
    }

    якщо (adjust_height_for_top_aligned && wp->w_want_scrollbar
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
	якщо (wp->w_buffer->b_term == NULL || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    якщо (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		якщо (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		інакше
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    якщо (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    інакше якщо (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    якщо (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    якщо (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    якщо (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    якщо (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	ціле avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    якщо (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	якщо (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	якщо (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	якщо (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    ціле avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    якщо (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    якщо (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	якщо (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    якщо (wp->w_popup_imagep != NULL)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	повернути;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    якщо (win_valid(wp->w_popup_prop_win))
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
    якщо (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	якщо (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_mask_refresh = TRUE;
    }

#ifdef FEAT_IMAGE_POPUP
    popup_position_image(wp);
#endif
}

typedef enum
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
    static ціле
popup_is_notification(create_type_T type)
{
    повернути type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    static void
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    ціле	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    для (lnum = buf->b_ml.ml_line_count; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    якщо (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == NULL ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    інакше
    {
	list_T *l = text.vval.v_list;

	якщо (l != NULL && l->lv_len > 0)
	{
	    якщо (l->lv_first == &range_list_item)
		emsg(_(e_using_number_as_string));
	    інакше якщо (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    інакше
		// list of dictionaries
		add_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_line_count);
    curbuf = curwin->w_buffer;
}

#define SET_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    do {						    \
	якщо (wp != NULL)					    \
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
    } поки (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    static ціле
parse_popup_option(win_T *wp, ціле is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    ціле	    border_enabled = FALSE;

    якщо (wp != NULL)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    для ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	ціле	x;

	e = vim_strchr(p, ':');
	якщо (e == NULL || e[1] == NUL)
	    повернути FAIL;

	p = vim_strchr(e, ',');
	якщо (p == NULL)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	якщо (STRNCMP(s, "height:", 7) == 0)
	{
	    якщо (dig != p)
		повернути FAIL;
	    якщо (wp != NULL)
	    {
		якщо (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	інакше якщо (STRNCMP(s, "width:", 6) == 0)
	{
	    якщо (dig != p)
		повернути FAIL;
	    якщо (wp != NULL)
	    {
		якщо (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	інакше якщо (STRNCMP(s, "highlight:", 10) == 0)
	{
	    якщо (wp != NULL)
	    {
		char *errmsg;
		ціле c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		якщо (errmsg == NULL)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		інакше
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	інакше якщо (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    якщо (*arg == NUL || *arg == ',')
		повернути FAIL;
	    якщо (wp != NULL)
	    {
		для (ціле i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	інакше якщо (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ціле		i;
	    ціле		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    ціле		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    якщо (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		якщо (wp != NULL)
		{
		    для (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		continue;
	    }

	    token = vim_strnsave(arg, token_len);
	    якщо (token == NULL)
		повернути FAIL;

	    якщо ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		якщо (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		інакше якщо (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		інакше якщо (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		інакше якщо (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		інакше якщо (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		інакше якщо (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    ціле		out[8];
		    ціле		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    для (i = 0; i < 8 && !failed; i++)
		    {
			якщо (*q == NUL)
			    failed = TRUE;
			інакше
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    якщо (i < 7)
			    {
				якщо (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    якщо (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			повернути FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    інакше
	    {
		vim_free(token);
		повернути FAIL;
	    }

	    якщо (wp != NULL)
	    {
		для (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	інакше якщо (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    ціле		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ціле		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    якщо (!on && !off)
		повернути FAIL;
	    on = on && mouse_has(MOUSE_INSERT) && (border_enabled || is_preview);
	    якщо (wp != NULL)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	інакше якщо (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ціле		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ціле		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    якщо (!on && !off)
		повернути FAIL;
	    якщо (wp != NULL)
	    {
		якщо (on && mouse_has(MOUSE_INSERT))
		    wp->w_popup_flags |= POPF_RESIZE;
		інакше
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	інакше якщо (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    ціле		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    ціле		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    якщо (!on && !off)
		повернути FAIL;
	    якщо (wp != NULL)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	інакше якщо (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    ціле		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    ціле		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    якщо (!menu && !item)
		повернути FAIL;
	    якщо (wp != NULL && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	інакше якщо (STRNCMP(s, "opacity:", 8) == 0)
	{
	    якщо (dig != p || x < 0 || x > 100)
		повернути FAIL;
	    якщо (wp != NULL)
	    {
		якщо (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		інакше
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	інакше
	    повернути FAIL;
    }
    повернути OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ціле
parse_previewpopup(win_T *wp)
{
    повернути parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    ціле
parse_completepopup(win_T *wp)
{
    повернути parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    void
popup_set_wantpos_cursor(win_T *wp, ціле width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    якщо (d != NULL)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    якщо (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    інакше
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	якщо (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    якщо (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	якщо (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    void
popup_set_wantpos_rowcol(win_T *wp, ціле row, ціле col)
{
    wp->w_wantline = row;
    wp->w_wantcol = col;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    static void
add_border_left_right_padding(win_T *wp)
{
    ціле i;

    для (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    static ціле
popup_terminal_exists(void)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	якщо (wp->w_buffer->b_term != NULL)
	    повернути TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    якщо (wp->w_buffer->b_term != NULL)
		повернути TRUE;
    повернути FALSE;
}
#endif

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    void
popup_redraw_all(void)
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
    ціле
popup_need_redraw(void)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	якщо ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    повернути TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    повернути TRUE;
    повернути FALSE;
}

/*
 * Set the color for a notification window.
 */
    static void
popup_update_color(win_T *wp, create_type_T type)
{
    char    *hiname = type == TYPE_MESSAGE_WIN
				       ? "MessageWindow" : "PopupNotification";
    char    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    якщо (errmsg == NULL)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    інакше
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    static win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = NULL;
    ціле		tabnr = 0;
    ціле		new_buffer;
    buf_T	*buf = NULL;
    dict_T	*d = NULL;
    ціле		i;

    якщо (check_secure())
	повернути NULL;

    якщо (argvars != NULL)
    {
	якщо (in_vim9script()
		&& (check_for_string_or_number_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    повернути NULL;

	// Check that arguments look OK.
	якщо (argvars[0].v_type == VAR_NUMBER)
	{
	    buf = buflist_findnr(argvars[0].vval.v_number);
	    якщо (buf == NULL)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_number);
		повернути NULL;
	    }
#ifdef FEAT_TERMINAL
	    якщо (buf->b_term != NULL && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		повернути NULL;
	    }
#endif
	}
	інакше якщо (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != NULL)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != NULL))
	{
	    emsg(_(e_buffer_number_text_or_list_required));
	    повернути NULL;
	}
	якщо (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    повернути NULL;
	d = argvars[1].vval.v_dict;
    }

    якщо (d != NULL)
    {
	якщо (dict_has_key(d, "tabpage"))
	    tabnr = (ціле)dict_get_number(d, "tabpage");
	інакше якщо (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	інакше
	    tabnr = 0;
	якщо (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    якщо (tp == NULL)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		повернути NULL;
	    }
	}
    }
    інакше якщо (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    якщо (buf != NULL && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	повернути NULL;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    якщо (wp == NULL)
	повернути NULL;
    якщо (rettv != NULL)
	rettv->vval.v_number = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    якщо (buf != NULL)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    інакше
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(NULL, NULL, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	якщо (buf == NULL)
	{
	    win_free_popup(wp);
	    повернути NULL;
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

    якщо (tp != NULL)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    інакше якщо (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    інакше // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	якщо (first_popupwin == NULL)
	    first_popupwin = wp;
	інакше
	{
	    поки (prev->w_next != NULL)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    якщо (new_buffer && argvars != NULL)
	popup_set_buffer_text(buf, argvars[0]);

    якщо (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    якщо (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_values(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    якщо (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	якщо (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_col + 1;
	set_mousemoved_values(wp);
	set_mousemoved_columns(wp, FIND_IDENT + FIND_STRING + FIND_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    якщо (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	ціле	height = buf->b_ml.ml_line_count + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	для (twp = first_popupwin; twp != NULL; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    якщо (twp != wp
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
	якщо (wp->w_wantline + height > Rows)
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
	для (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    якщо (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_left_right_padding(wp);
    }

    якщо (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	якщо (callback.cb_name != NULL)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    для (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    для (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    якщо (type == TYPE_PREVIEW)
    {
	якщо (mouse_has(MOUSE_INSERT))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	для (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    якщо (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	якщо (mouse_has(MOUSE_INSERT))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	add_border_left_right_padding(wp);
	parse_completepopup(wp);
    }
#endif

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_mode = MODE_ALL;

    якщо (d != NULL)
    {
	// Deal with options.
	якщо (apply_options(wp, d, TRUE) == FAIL)
	{
	    (void)popup_close(wp->w_id, FALSE);
	    повернути NULL;
	}
    }

#ifdef FEAT_TIMERS
    якщо (popup_is_notification(type) && wp->w_popup_timer == NULL)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    якщо (buf->b_term != NULL)
	win_enter(wp, FALSE);
#endif

    повернути wp;
}

/*
 * popup_clear()
 */
    void
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле force = FALSE;

    якщо (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	повернути;

    якщо (argvars[0].v_type != VAR_UNKNOWN)
	force = (ціле)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    void
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NORMAL);
}

/*
 * popup_atcursor({text}, {options})
 */
    void
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    void
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    static void
invoke_popup_callback(win_T *wp, typval_T *result)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = (varnumber_T)wp->w_id;

    якщо (result != NULL && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    інакше
    {
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    якщо (result != NULL)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    static void
back_to_prevwin(win_T *wp)
{
    якщо (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    інакше
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    static void
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    ціле id;

    якщо (!win_valid(wp))
       повернути;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    якщо (wp == curwin && curbuf->b_term != NULL)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    якщо (owp != curwin && owp->w_buffer->b_term != NULL)
		break;
	якщо (owp != NULL)
	    win_enter(owp, FALSE);
	інакше
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		якщо (owp != curwin && owp->w_buffer->b_term != NULL)
		    break;
	    якщо (owp != NULL)
		win_enter(owp, FALSE);
	    інакше
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    якщо (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	повернути;
    }

    CHECK_CURBUF;
    якщо (wp->w_close_cb.cb_name != NULL)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    void
popup_close_with_retval(win_T *wp, ціле retval)
{
    typval_T res;

    res.v_type = VAR_NUMBER;
    res.vval.v_number = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    void
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    static void
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    якщо (wp != mouse_wp
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
    void
popup_handle_mouse_moved(void)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    ціле	    row = mouse_row;
    ціле	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    для (wp = first_popupwin; wp != NULL; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    для (wp = curtab->tp_first_popupwin; wp != NULL; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    static void
filter_handle_drag(win_T *wp, ціле c, typval_T *rettv)
{
    ціле	row = mouse_row;
    ціле	col = mouse_col;

    якщо ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_key(c)
	    && (wp == popup_dragwin
			  || wp == mouse_find_win(&row, &col, FIND_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_number = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    void
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    ціле		id;
    win_T	*wp;
    char_u	*key;
    typval_T	res;
    ціле		c;
    linenr_T	old_lnum;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	повернути;

    id = tv_get_number(&argvars[0]);
    wp = win_id2wp(id);
    key = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    якщо (wp == NULL)
	повернути;

    c = *key;
    якщо (c == K_SPECIAL && key[1] != NUL)
	c = TO_SPECIAL(key[1], key[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_TRUE;
    res.v_type = VAR_NUMBER;

    old_lnum = wp->w_cursor.lnum;
    якщо (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	якщо (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	інакше
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_line_count;
    }
    якщо (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	якщо (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_line_count)
	    ++wp->w_cursor.lnum;
	інакше
	    wp->w_cursor.lnum = 1;
    }
    якщо (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	повернути;
    }

    якщо (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_number = -1;
	popup_close_and_callback(wp, &res);
	повернути;
    }
    якщо (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_number = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	повернути;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    void
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    ціле		id;
    win_T	*wp;
    char_u	*key;
    typval_T	res;
    ціле		c;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	повернути;

    id = tv_get_number(&argvars[0]);
    wp = win_id2wp(id);
    key = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    якщо (wp == NULL)
	повернути;

    c = *key;
    якщо (c == CAR && need_wait_return)
	повернути;
    якщо (c == K_SPECIAL && key[1] != NUL)
	c = TO_SPECIAL(key[1], key[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_TRUE;

    якщо (c == 'y' || c == 'Y')
	res.vval.v_number = 1;
    інакше якщо (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_number = 0;
    інакше
    {
	filter_handle_drag(wp, c, rettv);
	повернути;
    }

    // Invoke callback
    res.v_type = VAR_NUMBER;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    void
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    void
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    void
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    static win_T *
find_popup_win(ціле id)
{
    win_T *wp = win_id2wp(id);

    якщо (wp != NULL && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	повернути NULL;
    }
    повернути wp;
}

/*
 * popup_close({id})
 */
    void
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле		id;
    win_T	*wp;

    якщо (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	повернути;

    id = (ціле)tv_get_number(argvars);
    якщо (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == NULL &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	повернути;

    wp = find_popup_win(id);
    якщо (wp != NULL)
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
    static void
popup_clear_mask_for(win_T *wp)
{
    ціле r, c;
    ціле row_start, col_start, row_end, col_end;

    якщо (popup_mask == NULL || !popup_visible)
	повернути;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (ціле)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (ціле)screen_Columns);

    для (r = row_start; r < row_end; ++r)
	для (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    void
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    ціле			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    якщо (error_if_term_popup_window())
	повернути;
#endif
    якщо ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	повернути;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    якщо (wp->w_popup_imagep != NULL)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    якщо (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    якщо (was_visible)
	popup_clear_mask_for(wp);

    якщо (old_area.active)
	popup_redraw_exposed_area(&old_area);
    інакше
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    якщо (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    void
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле		id;
    win_T	*wp;

    якщо (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    void
popup_show(win_T *wp)
{
    bool popup_active;

    якщо ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	повернути;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    якщо (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_lines_valid = 0;
	якщо (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    інакше
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    void
f_popup_show(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле		id;
    win_T	*wp;

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = -1;

    якщо (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    якщо (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#endif

    rettv->vval.v_number = 0;
}

/*
 * popup_settext({id}, {text})
 */
    void
f_popup_settext(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле		id;
    win_T	*wp;
    popup_area_T	old_area;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	повернути;

    id = (ціле)tv_get_number(&argvars[0]);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;

    popup_save_area(wp, &old_area);

    якщо (check_for_string_or_list_arg(argvars, 1) == FAIL)
	повернути;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_lines_valid = 0;
    якщо (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    якщо (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    void
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    ціле			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    якщо (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	повернути;

    id = (ціле)tv_get_number(&argvars[0]);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;

    buf = tv_get_buf_from_arg(&argvars[1]);

    якщо (buf == NULL)
	повернути;
#ifdef FEAT_TERMINAL
    якщо (buf->b_term != NULL && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	повернути;
    }
#endif

    якщо (wp->w_buffer != buf)
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

	якщо (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_number = VVAL_TRUE;
}

    static void
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    ціле			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    якщо (wp->w_popup_imagep != NULL)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    якщо (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    якщо (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    якщо (wp == message_win)
	message_win = NULL;
#endif

    якщо (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    якщо (!old_area.active)
	popup_mask_refresh = TRUE;
}

    static void
error_for_popup_window(void)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    ціле
error_if_popup_window(ціле also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    якщо (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == NULL)
#endif
	    )
    {
	error_for_popup_window();
	повернути TRUE;
    }
    повернути FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    ціле
popup_close(ціле id, ціле force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = NULL;

    // go through global popups
    для (wp = first_popupwin; wp != NULL; prev = wp, wp = wp->w_next)
	якщо (wp->w_id == id)
	{
	    якщо (wp == curwin)
	    {
		якщо (!force)
		{
		    error_for_popup_window();
		    повернути FAIL;
		}
		back_to_prevwin(wp);
	    }
	    якщо (prev == NULL)
		first_popupwin = wp->w_next;
	    інакше
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    повернути OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	якщо (popup_close_tabpage(tp, id, force) == OK)
	    повернути OK;
    повернути FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    ціле
popup_close_tabpage(tabpage_T *tp, ціле id, ціле force)
{
    win_T	*wp;
    win_T	**root = &tp->tp_first_popupwin;
    win_T	*prev = NULL;

    для (wp = *root; wp != NULL; prev = wp, wp = wp->w_next)
	якщо (wp->w_id == id)
	{
	    якщо (wp == curwin)
	    {
		якщо (!force)
		{
		    error_for_popup_window();
		    повернути FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    якщо (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    якщо (prev == NULL)
		*root = wp->w_next;
	    інакше
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    повернути OK;
	}
    повернути FAIL;
}

    void
close_all_popups(ціле force)
{
    якщо (!force && ERROR_IF_ANY_POPUP_WINDOW)
	повернути;
    поки (first_popupwin != NULL)
	якщо (popup_close(first_popupwin->w_id, force) == FAIL)
	    повернути;
    поки (curtab->tp_first_popupwin != NULL)
	якщо (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    повернути;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    static void
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
    static void
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    ціле i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    для (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    static bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    ціле i;

    якщо (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	повернути true;
    для (i = 0; i < 4; i++)
	якщо (style->border_highlight[i] != wp->w_border_highlight[i])
	    повернути true;
    повернути false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    static void
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
    static bool
popup_layout_changed(win_T *wp, popup_layout_T *layout)
{
    повернути layout->winrow != wp->w_winrow
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
    static bool
popup_area_changed(win_T *wp, popup_area_T *area)
{
    повернути area->winrow != wp->w_winrow
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
    static void
mark_overlapped_opacity_popup(win_T *wp, ціле area_top, ціле area_bot,
	ціле area_left, ціле area_right, ціле zindex)
{
    якщо ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_top
	    || wp->w_wincol >= area_right
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_left)
	повернути;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_lines_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    static void
redraw_overlapped_opacity_popups(ціле winrow, ціле wincol, ціле height, ціле width,
	ціле leftoff, ціле zindex)
{
    win_T	*wp;
    ціле		area_top = winrow;
    ціле		area_bot = winrow + height;
    ціле		area_left = wincol;
    ціле		area_right = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_top, area_bot, area_left,
		area_right, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_top, area_bot, area_left,
		area_right, zindex);

    якщо (must_redraw < UPD_VALID)
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
    static void
refill_pum_bg_row_from_buffer(ціле r, ціле left, ціле right)
{
    ціле		line_cp = r;
    ціле		col_cp = left;
    win_T	*wp;
    linenr_T	lnum;
    ціле		row_for_lnum;
    ціле		col_for_lnum = 0;
    char_u	*line;
    char_u	*p;
    ціле		win_text_col;
    ціле		screen_col;
    ціле		soff_base = (r - pum_bg_top) * pum_bg_cols;
    ціле		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    для (c = left; c < right; ++c)
    {
	pum_bg_lines[soff_base + c] = ' ';
	якщо (pum_bg_attrs != NULL)
	    pum_bg_attrs[soff_base + c] = 0;
	якщо (enc_utf8 && pum_bg_linesUC != NULL)
	    pum_bg_linesUC[soff_base + c] = 0;
	якщо (enc_utf8)
	{
	    ціле k;
	    для (k = 0; k < MAX_MCO; ++k)
		якщо (pum_bg_linesC[k] != NULL)
		    pum_bg_linesC[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
    якщо (wp == NULL || line_cp < 0 || line_cp >= wp->w_height)
	повернути;
    якщо (wp->w_buffer == NULL || wp->w_buffer->b_ml.ml_mfp == NULL)
	повернути;

    // Compute the buffer line for this screen row.
    row_for_lnum = line_cp;
    якщо (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, NULL))
	повернути;	// past end of buffer
    якщо (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_line_count)
	повернути;

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    якщо (line == NULL)
	повернути;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    якщо (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = line;
    поки (*p != NUL && screen_col < right)
    {
	ціле char_cells;
	ціле byte_count;
	ціле soff = soff_base + screen_col;
	ціле in_range = (screen_col >= left && screen_col < right);

	якщо (*p == '\t')
	{
	    ціле ts = (ціле)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_count = 1;
	    якщо (in_range)
	    {
		pum_bg_lines[soff] = ' ';
		якщо (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = 0;
	    }
	}
	інакше якщо (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_count = mb_ptr2len(p);
	    якщо (in_range)
	    {
		pum_bg_lines[soff] = *p;
		якщо (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	інакше
	{
	    char_cells = 1;
	    byte_count = 1;
	    якщо (in_range)
	    {
		pum_bg_lines[soff] = (*p < 0x20) ? ' ' : *p;
		якщо (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = 0;
	    }
	}
	якщо (in_range && pum_bg_attrs != NULL)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_count;
	screen_col += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    static void
popup_redraw_exposed_area(popup_area_T *area)
{
    якщо (!area->active)
	повернути;

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
    якщо (pum_bg_lines != NULL
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_top)
    {
	ціле top = MAX(area->winrow, pum_bg_top);
	ціле bot = MIN(area->winrow + area->height, pum_bg_bot);
	ціле left = MAX(area->wincol, 0);
	ціле right = MIN(area->wincol + area->width, pum_bg_cols);
	ціле r;

	для (r = top; r < bot; ++r)
	    refill_pum_bg_row_from_buffer(r, left, right);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    static void
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->lines);
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
    static void
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    якщо (screen_opacity_popup == NULL
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	повернути;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    якщо (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->lines = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(ціле,
				  saved_screen->rows * saved_screen->cols);
    якщо (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    якщо (saved_screen->lines == NULL || saved_screen->attrs == NULL)
	повернути;

    для (ціле r = 0; r < saved_screen->rows; r++)
    {
	ціле screen_row = saved_screen->start_row + r;

	якщо (screen_row >= 0 && screen_row < screen_Rows)
	    для (ціле c = 0; c < saved_screen->cols; c++)
	    {
		ціле screen_col = saved_screen->start_col + c;

		якщо (screen_col >= 0 && screen_col < screen_Columns)
		{
		    ціле off = LineOffset[screen_row] + screen_col;
		    ціле save_off = r * saved_screen->cols + c;

		    saved_screen->lines[save_off] = ScreenLines[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    якщо (enc_utf8 && saved_screen->linesuc != NULL)
			saved_screen->linesuc[save_off] = ScreenLinesUC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    static void
redraw_under_popup_area(ціле winrow, ціле wincol, ціле height, ціле width, ціле leftoff)
{
    ціле	    r;

    для (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	ціле	    c;
	win_T	    *prev_twp = NULL;

	якщо (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    continue;
	}

	для (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    ціле	    line_cp = r;
	    ціле	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
	    якщо (twp != NULL && twp != prev_twp)
	    {
		prev_twp = twp;
		якщо (line_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (void)mouse_comp_pos(twp, &line_cp, &col_cp, &lnum, NULL);
		    redrawWinline(twp, lnum);
		}
		інакше якщо (line_cp == twp->w_height)
		    twp->w_redr_status = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    void
f_popup_move(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ціле		id;
    win_T	*wp;
    popup_area_T	old_area;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;  // invalid {id}

    якщо (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	повернути;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    якщо (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    якщо (popup_area_changed(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    void
f_popup_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;
    ціле		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    ціле		need_redraw = FALSE;
    ціле		need_reposition = FALSE;

    якщо (check_secure())
	повернути;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;  // invalid {id}

    якщо (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	повернути;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (void)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    якщо (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_line_count)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    якщо (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    інакше якщо (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    якщо (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    якщо (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    інакше якщо (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_lines_valid = 0;
	якщо (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    якщо (old_style.blend != wp->w_popup_blend)
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

    якщо (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    void
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ціле		id;
    win_T	*wp;
    ціле		top_extra;
    ціле		left_extra;

    якщо (rettv_dict_alloc(rettv) == FAIL)
	повернути;

    якщо (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;  // invalid {id}
    top_extra = popup_top_extra(wp);
    left_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_lock_size(&dict->dv_hashtab, 11);

    dict_add_number(dict, "line", wp->w_winrow + 1);
    dict_add_number(dict, "col", wp->w_wincol + 1);
    dict_add_number(dict, "width", wp->w_width + left_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_add_number(dict, "height", wp->w_height + top_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_add_number(dict, "core_line", wp->w_winrow + 1 + top_extra);
    dict_add_number(dict, "core_col", wp->w_wincol + 1 + left_extra);
    dict_add_number(dict, "core_width", wp->w_width);
    dict_add_number(dict, "core_height", wp->w_height);

    dict_add_number(dict, "scrollbar", wp->w_has_scrollbar);
    dict_add_number(dict, "firstline", wp->w_topline);
    dict_add_number(dict, "lastline", wp->w_botline - 1);
    dict_add_number(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    void
f_popup_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    якщо (rettv_list_alloc(rettv) == FAIL)
	повернути;
    FOR_ALL_POPUPWINS(wp)
	list_append_number(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    list_append_number(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    void
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    ціле		row;
    ціле		col;
    win_T	*wp;

    якщо (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	повернути;

    row = tv_get_number(&argvars[0]) - 1;
    col = tv_get_number(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    якщо (wp != NULL && WIN_IS_POPUP(wp))
	rettv->vval.v_number = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    static void
get_padding_border(dict_T *dict, ціле *array, char *name)
{
    list_T  *list;
    ціле	    i;

    якщо (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	повернути;

    list = list_alloc();
    якщо (list == NULL)
	повернути;

    якщо (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	повернути;
    }
    якщо (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	для (i = 0; i < 4; ++i)
	    list_append_number(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    static void
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ціле	    i;

    для (i = 0; i < 4; ++i)
	якщо (wp->w_border_highlight[i] != NULL)
	    break;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    якщо (i == 4 && !wp->w_border_highlight_isset)
	повернути;

    list = list_alloc();
    якщо (list == NULL)
	повернути;

    якщо (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	повернути;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    якщо (i == 4)
	повернути;
    для (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    static void
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    ціле	    i;
    char_u  buf[NUMBUFLEN];
    ціле	    len;

    для (i = 0; i < 8; ++i)
	якщо (wp->w_border_char[i] != 0)
	    break;
    якщо (i == 8)
	повернути;

    list = list_alloc();
    якщо (list == NULL)
	повернути;

    якщо (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	повернути;
    }
    для (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    static void
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    якщо (list != NULL)
    {
	якщо (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	інакше
	{
	    list_append_number(list, wp->w_popup_lnum);
	    list_append_number(list, wp->w_popup_mincol);
	    list_append_number(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    якщо (list == NULL)
	повернути;

    якщо (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	повернути;
    }
    list_append_number(list, wp->w_popup_mouse_row);
    list_append_number(list, wp->w_popup_mouse_mincol);
    list_append_number(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    void
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    ціле		id;
    win_T	*wp;
    tabpage_T	*tp;
    ціле		i;

    якщо (rettv_dict_alloc(rettv) == FAIL)
	повернути;

    якщо (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	повернути;

    id = (ціле)tv_get_number(argvars);
    wp = find_popup_win(id);
    якщо (wp == NULL)
	повернути;

    dict = rettv->vval.v_dict;
    dict_add_number(dict, "line", wp->w_wantline);
    dict_add_number(dict, "col", wp->w_wantcol);
    dict_add_number(dict, "minwidth", wp->w_minwidth);
    dict_add_number(dict, "minheight", wp->w_minheight);
    dict_add_number(dict, "maxheight", wp->w_maxheight);
    dict_add_number(dict, "maxwidth", wp->w_maxwidth);
    dict_add_number(dict, "firstline", wp->w_firstline);
    dict_add_number(dict, "scrollbar", wp->w_want_scrollbar);
    dict_add_number(dict, "zindex", wp->w_zindex);
    dict_add_number(dict, "fixed", wp->w_popup_fixed);
    якщо (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	якщо (pt != NULL)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_number(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_number(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    якщо (wp->w_popup_imagep != NULL && wp->w_popup_imagep->img != NULL)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	якщо (dict_add_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#endif

    dict_add_string(dict, "title", wp->w_popup_title);
    dict_add_number(dict, "wrap", wp->w_p_wrap);
    dict_add_number(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_add_number(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_add_number(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_add_number(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_add_number(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_add_number(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_add_number(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_add_number(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_add_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_add_string(dict, "highlights", wp->w_p_whl);
    якщо (wp->w_scrollbar_highlight != NULL)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    якщо (wp->w_thumb_highlight != NULL)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    якщо (twp->w_id == id)
		break;
	якщо (twp != NULL)
	    break;
	++i;
    }
    якщо (tp == NULL)
	i = -1;  // must be global
    інакше якщо (tp == curtab)
	i = 0;
    dict_add_number(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    якщо (wp->w_filter_cb.cb_name != NULL)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    якщо (wp->w_close_cb.cb_name != NULL)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    для (i = 0; i < (ціле)ARRAY_LENGTH(poppos_entries); ++i)
	якщо (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(ціле)poppos_entries[i].pp_name.length);
	    break;
	}

    якщо (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    інакше якщо (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    інакше
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#якщо defined(FEAT_TIMERS)
    dict_add_number(dict, "time", wp->w_popup_timer != NULL
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#якщо defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    ціле
error_if_term_popup_window(void)
{
    якщо (WIN_IS_POPUP(curwin) && curbuf->b_term != NULL
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	повернути TRUE;
    }
    повернути FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    void
popup_reset_handled(ціле handled_flag)
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
find_next_popup(ціле lowest, ціле handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    ціле	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = NULL;
    FOR_ALL_POPUPWINS(wp)
	якщо ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    якщо (found_wp != NULL)
	found_wp->w_popup_handled |= handled_flag;
    повернути found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    static ціле
invoke_popup_filter(win_T *wp, ціле c)
{
    ціле		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    ціле		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    якщо (c == Ctrl_C)
    {
	ціле save_got_int = got_int;
	ціле was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	якщо (was_curwin && wp == curwin)
	    повернути -1;
	повернути TRUE;
    }

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_to_buf(c, mod_mask, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    якщо (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    інакше
    {
	якщо (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	якщо (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    якщо (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	інакше
	{
	    якщо (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    повернути res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    ціле
popup_do_filter(ціле c)
{
    static ціле	recursive = FALSE;
    ціле		res = FALSE;
    win_T	*wp;
    ціле		save_KeyTyped = KeyTyped;
    ціле		state;
    ціле		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    якщо (popup_is_popup(curwin) && curbuf->b_term != NULL)
	повернути FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    якщо (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	повернути FALSE;

    якщо (recursive)
	повернути FALSE;
    recursive = TRUE;

    якщо (c == K_LEFTMOUSE)
    {
	ціле row = mouse_row;
	ціле col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	якщо (wp != NULL && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    поки (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != NULL)
	якщо (wp->w_filter_cb.cb_name != NULL
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    якщо (must_redraw > was_must_redraw)
    {
	ціле save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    KeyTyped = save_KeyTyped;

    // When interrupted return FALSE to avoid looping.
    повернути res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    ціле
popup_no_mapping(void)
{
    ціле	    round;
    win_T   *wp;

    для (round = 1; round <= 2; ++round)
	для (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != NULL; wp = wp->w_next)
	    якщо (wp->w_filter_cb.cb_name != NULL
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		повернути TRUE;
    повернути FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    void
popup_check_cursor_pos(void)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    поки ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != NULL)
	якщо (wp->w_popup_curwin != NULL
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.col < wp->w_popup_mincol
		    || curwin->w_cursor.col > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    static void
popup_update_mask(win_T *wp, ціле width, ціле height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    ціле		row, col;

    якщо (wp->w_popup_mask == NULL || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	повернути;
    }
    якщо (wp->w_popup_mask_cells != NULL
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	повернути;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    якщо (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = NULL;
	повернути;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    якщо (wp->w_popup_mask_cells == NULL)
	повернути;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_ITEMS(wp->w_popup_mask, lio)
    {
	ціле cols, cole;
	ціле lines, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_number(&li->li_tv);
	якщо (cols < 0)
	    cols = width + cols + 1;
	якщо (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_number(&li->li_tv);
	якщо (cole < 0)
	    cole = width + cole + 1;
	якщо (cole > width)
	    cole = width;
	li = li->li_next;
	lines = tv_get_number(&li->li_tv);
	якщо (lines < 0)
	    lines = height + lines + 1;
	якщо (lines <= 0)
	    lines = 1;
	li = li->li_next;
	linee = tv_get_number(&li->li_tv);
	якщо (linee < 0)
	    linee = height + linee + 1;
	якщо (linee > height)
	    linee = height;

	для (row = lines - 1; row < linee; ++row)
	    для (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    static ціле
popup_masked(win_T *wp, ціле width, ціле height, ціле screencol, ціле screenline)
{
    ціле col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    ціле line = screenline - wp->w_winrow;

    повернути col >= 0 && col < width
	    && line >= 0 && line < height
	    && wp->w_popup_mask_cells[line * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    static void
update_popup_transparent(win_T *wp, ціле val)
{
    якщо (wp->w_popup_mask == NULL)
	повернути;

    ціле		width = popup_width(wp);
    ціле		height = popup_height(wp);
    listitem_T	*lio, *li;
    ціле		cols, cole;
    ціле		lines, linee;
    ціле		col, line;

    FOR_ALL_LIST_ITEMS(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_number(&li->li_tv);
	якщо (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_number(&li->li_tv);
	якщо (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	lines = tv_get_number(&li->li_tv);
	якщо (lines < 0)
	    lines = height + lines + 1;
	li = li->li_next;
	linee = tv_get_number(&li->li_tv);
	якщо (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	якщо (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--lines;
	якщо (lines < 0)
	    lines = 0;
	для (line = lines; line < linee
		&& line + wp->w_winrow < screen_Rows; ++line)
	    для (col = cols; col < cole
		    && col + wp->w_wincol < screen_Columns; ++col)
		popup_transparent[(line + wp->w_winrow) * screen_Columns
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
    static ціле
check_popup_unhidden(win_T *wp)
{
    якщо (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	якщо ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    повернути FALSE;
	якщо (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	інакше якщо (popup_find_prop_above_top(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	якщо (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    повернути TRUE;
	}
    }
    повернути FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    static ціле
popup_need_position_adjust(win_T *wp)
{
    якщо (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	повернути TRUE;
    якщо (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	повернути TRUE;

    // May need to adjust the width if the cursor moved.
    повернути wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
static short *opacity_zindex = NULL;
static ціле    opacity_zindex_rows = 0;
static ціле    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    static void
popup_mark_opacity_zindex(win_T *wp)
{
    ціле	    width;
    ціле	    height;
    ціле	    r, c;

    якщо (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	повернути;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    для (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	для (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    ціле off = r * screen_Columns + c;
	    якщо (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    static void
redraw_win_under_opacity_popup(win_T *wp)
{
    ціле	    height;
    ціле	    width;
    ціле	    r;

    якщо (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	повернути;

    height = popup_height(wp);
    width = popup_width(wp);
    для (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	ціле	    col;
	win_T	    *prev_twp = NULL;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	для (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    ціле	    line_cp = r;
	    ціле	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
	    якщо (twp != NULL && twp != prev_twp)
	    {
		prev_twp = twp;
		якщо (line_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    якщо (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    інакше
#endif
		    {
			(void)mouse_comp_pos(twp, &line_cp, &col_cp, &lnum,
									NULL);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		інакше якщо (line_cp == twp->w_height)
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
    ціле
popup_is_under_opacity(ціле row, ціле col)
{
    якщо (opacity_zindex == NULL
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	повернути FALSE;
    повернути opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    ціле
popup_is_over_opacity(ціле row, ціле col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	якщо ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    повернути TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    повернути TRUE;
    повернути FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    ціле
popup_is_under_opacity_range(ціле row, ціле start_col, ціле end_col)
{
    ціле col;

    якщо (opacity_zindex == NULL
	    || row < 0 || row >= opacity_zindex_rows)
	повернути FALSE;
    для (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	якщо (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    повернути TRUE;
    повернути FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    void
may_update_popup_mask(ціле type)
{
    win_T	*wp;
    short	*mask;
    ціле		line, col;
    ціле		redraw_all_popups = FALSE;
    ціле		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    якщо (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	якщо (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	інакше якщо (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	інакше якщо (popup_need_position_adjust(wp))
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
	ціле has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    якщо ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    якщо ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	якщо (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	інакше
	{
	    якщо (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    якщо (opacity_zindex != NULL)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_Rows * screen_Columns * sizeof(short));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    якщо (!popup_mask_refresh)
	повернути;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	якщо (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    якщо (redrawing_all_win)
	mask = popup_mask;
    інакше
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * sizeof(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    поки ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != NULL)
    {
	ціле width;
	ціле height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	якщо (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    якщо (wp->w_popup_flags & POPF_HIDDEN)
		continue;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	якщо (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	інакше
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	якщо ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    continue;

	{
	    ціле mask_start = wp->w_winrow;
	    ціле mask_end = mask_start + height;
	    ціле mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    ціле mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    для (line = mask_start;
			    line < mask_end && line < screen_Rows; ++line)
		для (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    якщо (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(line, col, FALSE))
			mask[line * screen_Columns + col] = POPUPMENU_ZINDEX;
		    інакше якщо (wp->w_popup_mask_cells == NULL
				    || !popup_masked(wp, width, height, col, line))
			mask[line * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    якщо (mask == popup_mask_next)
    {
	ціле	    *plines_cache = ALLOC_CLEAR_MULT(ціле, Rows);
	win_T	    *prev_wp = NULL;

	для (line = 0; line < screen_Rows; ++line)
	{
	    ціле	    col_done = 0;

	    для (col = 0; col < screen_Columns; ++col)
	    {
		ціле off = line * screen_Columns + col;

		якщо (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    якщо (line >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			якщо (!msg_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    інакше якщо (col >= col_done)
		    {
			linenr_T	lnum;
			ціле		line_cp = line;
			ціле		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
			якщо (wp != NULL)
			{
#якщо defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    якщо (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    інакше
#endif
			    {
				якщо (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   sizeof(ціле) * Rows);
				    prev_wp = wp;
				}

				якщо (line_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				інакше
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (void)mouse_comp_pos(wp, &line_cp, &col_cp,
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
    void
may_update_popup_position(void)
{
    якщо (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
static schar_T *base_screenlines = NULL;
static ціле *base_screenattrs = NULL;
static u8char_T *base_screenlinesuc = NULL;
static ціле base_screen_rows = 0;
static ціле base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    ціле
popup_get_base_screen_cell(ціле row, ціле col, schar_T *linep, ціле *attrp,
							  u8char_T *ucp)
{
    якщо (base_screenlines == NULL || base_screenattrs == NULL)
	повернути FALSE;
    якщо (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	повернути FALSE;

    ціле off = row * base_screen_cols + col;
    якщо (linep != NULL)
	*linep = base_screenlines[off];
    якщо (attrp != NULL)
	*attrp = base_screenattrs[off];
    якщо (ucp != NULL)
    {
	якщо (enc_utf8 && base_screenlinesuc != NULL)
	    *ucp = base_screenlinesuc[off];
	інакше
	    *ucp = 0;
    }
    повернути TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    void
popup_set_base_screen_cell(ціле row, ціле col, schar_T line, ціле attr, u8char_T uc)
{
    якщо (base_screenlines == NULL || base_screenattrs == NULL)
	повернути;
    якщо (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	повернути;

    ціле off = row * base_screen_cols + col;
    base_screenlines[off] = line;
    base_screenattrs[off] = attr;
    якщо (enc_utf8 && base_screenlinesuc != NULL)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
static void
draw_opacity_padding_cell(
	ціле		row,
	ціле		col,
	popup_saved_screen_T *saved_screen,
	ціле		pad_start_col,
	ціле		pad_end_col)
{
    якщо (LineOffset == NULL || ScreenLines == NULL || ScreenAttrs == NULL
	    || (enc_utf8 && ScreenLinesUC == NULL))
	повернути;

    ціле off = LineOffset[row] + col;
    ціле r = row - saved_screen->start_row;
    ціле c = col - saved_screen->start_col;

    якщо (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	ціле save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	якщо (enc_utf8 && saved_screen->linesuc != NULL)
	{
	    ціле base_col = col - 1;
	    ціле base_off = off - 1;
	    ціле base_save_off = save_off - 1;
	    ціле wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    якщо (base_off >= 0)
	    {
		якщо (ScreenLinesUC[base_off] != 0
			&& utf_char2cells(ScreenLinesUC[base_off]) == 2
			&& ScreenLines[off] == 0)
		    wide_prev = TRUE;
	    }
	    якщо (!wide_prev && save_off > 0)
	    {
		якщо (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->lines[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    якщо (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		якщо (base_col < pad_start_col)
		{
		    якщо (ScreenLinesUC[base_off] != 0
			    && utf_char2cells(ScreenLinesUC[base_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			ScreenLines[base_off] = ' ';
			ScreenLinesUC[base_off] = 0;
			ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
			popup_set_base_screen_cell(row, base_col,
				ScreenLines[base_off],
				ScreenAttrs[base_off],
				ScreenLinesUC[base_off]);
			screen_char(base_off, row, base_col);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			ScreenLines[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[base_save_off];
			якщо (enc_utf8)
			    ScreenLinesUC[off] = 0;
			ціле popup_attr_val =
					get_win_attr(screen_opacity_popup);
			ціле blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				ScreenLines[off], ScreenAttrs[off],
				ScreenLinesUC[off]);
			screen_char(off, row, col);
			повернути;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    якщо (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && ScreenLines[base_off] == ' '
			    && ScreenLinesUC[base_off] == 0)
		    {
			ціле popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			ціле blend =
				    screen_opacity_popup->w_popup_blend;

			ScreenLines[base_off] =
				    saved_screen->lines[base_save_off];
			ScreenLinesUC[base_off] =
				    saved_screen->linesuc[base_save_off];
			ScreenAttrs[base_off] =
				    saved_screen->attrs[base_save_off];
			ScreenAttrs[base_off] = hl_blend_attr(
				    ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);

			ScreenLines[off] = 0;
			ScreenLinesUC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[base_off];

			popup_set_base_screen_cell(row, base_col,
				    ScreenLines[base_off],
				    ScreenAttrs[base_off],
				    ScreenLinesUC[base_off]);
			popup_set_base_screen_cell(row, col,
				    ScreenLines[off],
				    ScreenAttrs[off],
				    ScreenLinesUC[off]);
			screen_char(base_off, row, base_col);
			повернути;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    ScreenLines[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    якщо (enc_utf8)
			ScreenLinesUC[off] = 0;
		    ціле popup_attr_val = get_win_attr(screen_opacity_popup);
		    ціле blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, ScreenLines[off],
					       ScreenAttrs[off], ScreenLinesUC[off]);
		    screen_char(off, row, col);
		    повернути;
		}

		// Base cell is inside the saved area, redraw the wide char.
		якщо (save_off > 0)
		{
		    ScreenLines[base_off] = saved_screen->lines[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    ScreenLines[off] = saved_screen->lines[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    ScreenLinesUC[base_off] = saved_screen->linesuc[base_save_off];
		    ScreenLinesUC[off] = saved_screen->linesuc[save_off];

		    ціле popup_attr_val = get_win_attr(screen_opacity_popup);
		    ціле blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, ScreenLines[base_off],
					       ScreenAttrs[base_off], ScreenLinesUC[base_off]);
		    popup_set_base_screen_cell(row, col, ScreenLines[off],
					       ScreenAttrs[off], ScreenLinesUC[off]);
		    screen_char(base_off, row, base_col);
		}
		повернути;
	    }
	}
	ScreenLines[off] = saved_screen->lines[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	якщо (enc_utf8)
	    ScreenLinesUC[off] = saved_screen->linesuc != NULL
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	якщо (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    ScreenLines[off] = ' ';
	    ScreenLinesUC[off] = 0;
	}

	ціле popup_attr_val = get_win_attr(screen_opacity_popup);
	ціле blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_set_base_screen_cell(row, col, ScreenLines[off],
				   ScreenAttrs[off], ScreenLinesUC[off]);
	screen_char(off, row, col);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
static void
fill_opacity_padding(
	ціле		start_row,
	ціле		end_row,
	ціле		start_col,
	ціле		end_col,
	popup_saved_screen_T *saved_screen)
{
    для (ціле pad_row = start_row; pad_row < end_row; pad_row++)
	для (ціле pad_col = start_col; pad_col < end_col; pad_col++)
	    draw_opacity_padding_cell(pad_row, pad_col, saved_screen,
		    start_col, end_col);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    void
update_popups(void (*win_update)(win_T *wp))
{
    win_T   *wp;
    ціле	    top_off;
    ціле	    left_extra;
    ціле	    total_width;
    ціле	    total_height;
    ціле	    top_padding;
    ціле	    popup_attr;
    ціле	    border_attr[4];
    ціле	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    ціле	    row;
    ціле	    wincol;
    ціле	    padcol = 0;
    ціле	    padendcol = 0;
    ціле	    i;
    ціле	    sb_thumb_top = 0;
    ціле	    sb_thumb_height = 0;
    ціле	    attr_scroll = 0;
    ціле	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    якщо (base_screenlines != NULL)
    {
	vim_free(base_screenlines);
	base_screenlines = NULL;
    }
    якщо (base_screenattrs != NULL)
    {
	vim_free(base_screenattrs);
	base_screenattrs = NULL;
    }
    якщо (base_screenlinesuc != NULL)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = NULL;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    поки ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != NULL)
    {
	ціле	    title_len = 0;
	ціле	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	якщо ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	інакше
	    screen_opacity_popup = NULL;

#ifdef FEAT_PROP_POPUP
	якщо (screen_opacity_popup != NULL)
	{
	    якщо (base_screenlines != NULL)
	    {
		vim_free(base_screenlines);
		base_screenlines = NULL;
	    }
	    якщо (base_screenattrs != NULL)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = NULL;
	    }
	    якщо (base_screenlinesuc != NULL)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = NULL;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(ціле,
				    base_screen_rows * base_screen_cols);
	    якщо (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    якщо (base_screenlines != NULL && base_screenattrs != NULL)
	    {
		для (ціле r = 0; r < base_screen_rows; r++)
		{
		    ціле off = LineOffset[r];
		    ціле base_off = r * base_screen_cols;
		    для (ціле c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = ScreenLines[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			якщо (enc_utf8 && base_screenlinesuc != NULL)
			    base_screenlinesuc[base_off + c] =
					ScreenLinesUC[off + c];
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
	top_off = popup_top_extra(wp);
	left_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	якщо (wp->w_wincol + left_extra < 0)
	    left_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(top_off - wp->w_popup_topoff, 0);
	wp->w_wincol += left_extra;

	// Draw the popup text, unless it's off screen.
	якщо (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    якщо (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    якщо (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    інакше якщо (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(top_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= left_extra;

	// Add offset for border and padding if not done already.
	якщо ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += left_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	якщо ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += top_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	якщо (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	інакше
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	якщо (total_width < 0)
	    total_width = 0;
	якщо (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	інакше
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	якщо (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	інакше
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	якщо (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	інакше
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    для (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    якщо (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	для (i = 0; i < 8; ++i)
	    якщо (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	для (i = 0; i < 4; ++i)
	{
	    якщо (wp->w_border_highlight[i] != NULL)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    інакше якщо (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    інакше
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    якщо ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	якщо ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	якщо (wp->w_popup_title != NULL)
	{
	    ціле	    title_attr;

	    якщо (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != NULL)
		title_attr = border_attr[0];
	    інакше якщо (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    інакше
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    якщо (title_len > total_width - 2)
	    {
		ціле	title_byte_len = (ціле)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		якщо (title_text != NULL)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    інакше
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	top_padding = cl.eff_padding[0];
	якщо (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    якщо (title_len > 0)
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
	    інакше
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    якщо (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	інакше якщо (cl.eff_padding[0] == 0 && popup_top_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    top_padding = 1;

	якщо (top_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    якщо (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	якщо (top_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    якщо (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		якщо (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		інакше
		{
		    screen_fill(row, row + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(row, row + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		row += 1;
		top_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    якщо (screen_opacity_popup != NULL && saved_screen.lines != NULL)
	    {
		fill_opacity_padding(row, row + top_padding, padcol, padendcol,
			&saved_screen);
	    }
	    інакше
	    {
		screen_fill(row, row + top_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	якщо (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_line_count;
	    ціле		height = wp->w_height;
	    ціле		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    якщо (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    якщо (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    якщо (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_top = 0;
	    інакше
		sb_thumb_top = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    якщо (wp->w_topline > 1 && sb_thumb_top == 0 && height > 1)
		sb_thumb_top = 1;  // show it's scrolled
	    last = total_height - top_off - wp->w_popup_border[2];
	    якщо (sb_thumb_top >= last)
		// show at least one character
		sb_thumb_top = last - 1;

	    якщо (wp->w_scrollbar_highlight != NULL)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    інакше
		attr_scroll = highlight_attr[HLF_PSB];
	    якщо (wp->w_thumb_highlight != NULL)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    інакше
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	для (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    ціле	pad_left;
	    // left and right padding only needed next to the body
	    ціле do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    якщо (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    якщо (do_padding && cl.eff_padding[3] > 0)
	    {
		ціле col = wincol + wp->w_popup_border[3];

		// left padding
		pad_left = wp->w_popup_padding[3];
		якщо (col < 0)
		{
		    pad_left += col;
		    col = 0;
		}
		якщо (pad_left > 0)
		{
		    якщо (screen_opacity_popup != NULL && saved_screen.lines != NULL)
			fill_opacity_padding(row, row + 1, col, col + pad_left,
				&saved_screen);
		    інакше
			screen_fill(row, row + 1, col, col + pad_left,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    якщо (wp->w_has_scrollbar)
	    {
		ціле line = i - top_off;
		ціле scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		якщо (line >= 0 && line < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    line >= sb_thumb_top
				       && line < sb_thumb_top + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		інакше
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    якщо (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    якщо (do_padding && cl.eff_padding[1] > 0)
	    {
		ціле pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		ціле pad_col_end = pad_col_start + wp->w_popup_padding[1];

		якщо (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		інакше
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	якщо (wp->w_popup_shadow)
	{
	    ціле col = wincol + total_width;
	    для (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	якщо (cl.eff_padding[2] > 0)
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
	    якщо (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    інакше
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	якщо (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    якщо (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	якщо (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    для (ціле col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	якщо (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + total_width - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = NULL;

	// Back to the normal zindex.
	screen_zindex = 0;

#ifdef HAS_MESSAGE_WINDOW
	// if this was the message window popup may start the timer now
	may_start_message_win_timer(wp);
#endif

	якщо (override_success)
	    pop_highlight_overrides();

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

#якщо defined(FEAT_SEARCH_EXTRA)
    // In case win_update() called start_search_hl().
    end_search_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    static ціле
set_ref_in_one_popup(win_T *wp, ціле copyID)
{
    ціле		abort = FALSE;
    typval_T	tv;

    якщо (wp->w_close_cb.cb_partial != NULL)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    якщо (wp->w_filter_cb.cb_partial != NULL)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    повернути abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    ціле
set_ref_in_popups(ціле copyID)
{
    ціле		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    для (wp = first_popupwin; !abort && wp != NULL; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	для (wp = tp->tp_first_popupwin; !abort && wp != NULL; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	якщо (abort)
	    break;
    }
    повернути abort;
}

    ціле
popup_is_popup(win_T *wp)
{
    повернути wp->w_popup_flags != 0;
}

#якщо defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_preview_window(void)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо (wp->w_p_pvw)
	    повернути wp;
    повернути NULL;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_find_info_window(void)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	якщо (wp->w_popup_flags & POPF_INFO)
	    повернути wp;
    повернути NULL;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(ціле dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    ціле		by;
    linenr_T	new_topline;

    якщо (wp == NULL)
	повернути false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    якщо (new_topline < 1)
	new_topline = 1;
    якщо (new_topline > wp->w_buffer->b_ml.ml_line_count)
	new_topline = wp->w_buffer->b_ml.ml_line_count;
    якщо (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    повернути true;
#інакше
    повернути false;
#endif
}

    void
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_number = message_win == NULL ? 0 : message_win->w_id;
#інакше
    rettv->vval.v_number = 0;
#endif
}

    void
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_number = wp == NULL ? 0 : wp->w_id;
#інакше
    rettv->vval.v_number = 0;
#endif
}

    void
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_number = wp == NULL ? 0 : wp->w_id;
#інакше
    rettv->vval.v_number = 0;
#endif
}

#якщо defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    ціле
popup_create_preview_window(ціле info)
{
    win_T *wp = popup_create(NULL, NULL, info ? TYPE_INFO : TYPE_PREVIEW);

    якщо (wp == NULL)
	повернути FAIL;
    якщо (info)
	wp->w_popup_flags |= POPF_INFO;
    інакше
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    якщо (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    інакше якщо (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    інакше
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    повернути OK;
}

/*
 * Close any preview popup.
 */
    void
popup_close_preview(void)
{
    win_T *wp = popup_find_preview_window();

    якщо (wp != NULL)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    void
popup_hide_info(void)
{
    win_T *wp = popup_find_info_window();

    якщо (wp != NULL)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	якщо (State & MODE_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    void
popup_close_info(void)
{
    win_T *wp = popup_find_info_window();

    якщо (wp != NULL)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    ціле
popup_overlaps_cmdline(void)
{
    повернути popup_on_cmdline;
}

#якщо defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(void)
{
    якщо (message_win != NULL)
	повернути message_win;

    ціле i;

    message_win = popup_create(NULL, NULL, TYPE_MESSAGE_WIN);

    якщо (message_win == NULL)
	повернути NULL;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    для (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    для (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    якщо (message_win->w_popup_timer != NULL)
	message_win->w_popup_timer->tr_keep = TRUE;
    повернути message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    void
popup_show_message_win(void)
{
    якщо (message_win == NULL)
	повернути;

    якщо ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(message_win, TYPE_MESSAGE_WIN);
	popup_show(message_win);
    }
    start_message_win_timer = TRUE;
}

    static void
may_start_message_win_timer(win_T *wp)
{
    якщо (wp == message_win && start_message_win_timer)
    {
	якщо (message_win->w_popup_timer != NULL)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    ціле
popup_message_win_visible(void)
{
    повернути message_win != NULL
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    void
popup_hide_message_win(void)
{
    якщо (message_win != NULL)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
static ціле save_msg_didout = FALSE;
static ціле save_msg_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
static ціле ew_msg_didout = FALSE;
static ціле ew_msg_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    void
start_echowindow(ціле time_sec)
{
    in_echowindow = TRUE;
    save_msg_didout = msg_didout;
    save_msg_col = msg_col;
    msg_didout = ew_msg_didout;
    msg_col = ew_msg_col;
    якщо (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    void
end_echowindow(void)
{
    in_echowindow = FALSE;

    якщо ((State & MODE_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_msg_didout = TRUE;
    ew_msg_col = msg_col == 0 ? 1 : msg_col;
    msg_didout = save_msg_didout;
    msg_col = save_msg_col;
}
#endif

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    ціле
popup_win_closed(win_T *win)
{
    ціле	    round;
    win_T   *wp;
    win_T   *next;
    ціле	    ret = FALSE;

    для (round = 1; round <= 2; ++round)
	для (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != NULL; wp = next)
	{
	    next = wp->w_next;
	    якщо (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    повернути ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    void
popup_set_title(win_T *wp)
{
    якщо (wp->w_buffer->b_fname == NULL)
	повернути;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    якщо (wp->w_popup_title != NULL)
	vim_snprintf((char *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#якщо defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    void
popup_update_preview_title(void)
{
    win_T *wp = popup_find_preview_window();

    якщо (wp != NULL)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
