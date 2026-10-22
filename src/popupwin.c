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

#如果 defined(FEAT_PROP_POPUP)

typedef struct {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_entry_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
typedef struct {
    整数		active;
    整数		winrow;
    整数		wincol;
    整数		height;
    整数		width;
    整数		leftoff;
    整数		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
typedef struct {
    schar_T	*lines;
    整数		*attrs;
    u8char_T	*linesuc;
    整数		start_row;
    整数		start_col;
    整数		rows;
    整数		cols;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
typedef struct {
    linenr_T	firstline;
    整数		blend;
    整数		flags;
    整数		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
typedef struct {
    整数		winrow;
    整数		wincol;
    整数		width;
    整数		height;
    整数		leftcol;
    整数		leftoff;
    整数		has_scrollbar;
    整数		topoff;
    整数		bottomoff;
    整数		leftclip;
    整数		rightclip;
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
static 整数  message_win_time = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
static 整数    start_message_win_timer = FALSE;

static void may_start_message_win_timer(win_T *wp);
#endif

static 整数 popup_on_cmdline = FALSE;

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
static void redraw_under_popup_area(整数 winrow, 整数 wincol, 整数 height,
	整数 width, 整数 leftoff);
static void redraw_overlapped_opacity_popups(整数 winrow, 整数 wincol,
	整数 height, 整数 width, 整数 leftoff, 整数 zindex);
static void redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    static 整数
popup_options_one(dict_T *dict, char_u *key)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    整数		n = 0;

    di = dict_find(dict, key, -1);
    如果 (di == NULL)
	返回 MAXCOL;

    val = tv_get_string(&di->di_tv);
    如果 (STRNCMP(val, "cursor", 6) != 0)
	返回 dict_get_number_check(dict, key);

    setcursor_mayforce(TRUE);
    s = val + 6;
    如果 (*s != NUL)
    {
	endp = s;
	如果 (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((char *)s, (char **)&endp, 10);
	如果 (endp != NULL && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_expression_str), val);
	    返回 0;
	}
    }

    如果 (STRCMP(key, "line") == 0)
	n = screen_screenrow() + 1 + n;
    否则 // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    如果 (n == 0)
	n = -1;
    返回 n;
}

    static 整数
set_padding_border(dict_T *dict, 整数 *array, char *name, 整数 max_val)
{
    dictitem_T	*di;

    di = dict_find(dict, (char_u *)name, -1);
    如果 (di == NULL)
	返回 OK;

    如果 (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	返回 FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    整数		i;
    整数		nr;

    为 (i = 0; i < 4; ++i)
	array[i] = 1;
    如果 (list == NULL)
	返回 OK;

    CHECK_LIST_MATERIALIZE(list);
    为 (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
	    ++i, li = li->li_next)
    {
	nr = (整数)tv_get_number(&li->li_tv);
	如果 (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    返回 OK;
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
set_moved_columns(win_T *wp, 整数 flags)
{
    char_u	*ptr;
    整数		len = find_ident_under_cursor(&ptr, flags | FIND_NOERROR);
    整数		mincol;

    如果 (len <= 0)
	返回;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (整数)(ptr - ml_get_curline());
    如果 (curwin->w_cursor.col < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.col;
	wp->w_popup_maxcol = curwin->w_cursor.col;
	返回;
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
    如果 (!popup_visible)
	返回;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	如果 (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    返回;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	如果 (wp->w_popup_mouse_row != 0)
	{
	    popup_uses_mouse_move = TRUE;
	    返回;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    static void
set_mousemoved_columns(win_T *wp, 整数 flags)
{
    win_T	*textwp;
    char_u	*text;
    整数		col;
    pos_T	pos;
    colnr_T	mcol;

    如果 (find_word_under_cursor(mouse_row, mouse_col, TRUE, flags,
				  &textwp, &pos.lnum, &text, NULL, &col) != OK)
	返回;

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
    整数
popup_on_border(win_T *wp, 整数 row, 整数 col)
{
    返回 (row == 0 && wp->w_popup_border[0] > 0)
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
    整数
popup_close_if_on_X(win_T *wp, 整数 row, 整数 col)
{
    如果 (wp->w_popup_close == POPCLOSE_BUTTON
	    && row == 0 && col == popup_width(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	返回 TRUE;
    }
    返回 FALSE;
}

// Values set when dragging a popup window starts.
static 整数 drag_start_row;
static 整数 drag_start_col;
static 整数 drag_start_wantline;
static 整数 drag_start_wantcol;
static 整数 drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    void
popup_start_drag(win_T *wp, 整数 row, 整数 col)
{
    drag_start_row = mouse_row;
    drag_start_col = mouse_col;
    如果 (wp->w_wantline <= 0)
	drag_start_wantline = wp->w_winrow + 1;
    否则
	drag_start_wantline = wp->w_wantline;
    如果 (wp->w_wantcol == 0)
	drag_start_wantcol = wp->w_wincol + 1;
    否则
	drag_start_wantcol = wp->w_wantcol;

    // Stop centering the popup
    如果 (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && row == popup_height(wp) - 1
			    && col == popup_width(wp) - 1;

    如果 (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	如果 (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	如果 (wp->w_popup_pos == POPPOS_BOTLEFT)
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
    如果 (!win_valid_popup(wp))
	返回;

    如果 ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	整数 width_inc = mouse_col - drag_start_col;
	整数 height_inc = mouse_row - drag_start_row;

	如果 (width_inc != 0)
	{
	    整数 width = wp->w_width + width_inc;

	    如果 (width < 1)
		width = 1;
	    wp->w_minwidth = width;
	    wp->w_maxwidth = width;
	    drag_start_col = mouse_col;
	}

	如果 (height_inc != 0)
	{
	    整数 height = wp->w_height + height_inc;

	    如果 (height < 1)
		height = 1;
	    wp->w_minheight = height;
	    wp->w_maxheight = height;
	    drag_start_row = mouse_row;
	}

	popup_adjust_position(wp);
	返回;
    }

    如果 (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	返回;
    wp->w_wantline = drag_start_wantline + (mouse_row - drag_start_row);
    如果 (wp->w_wantline < 1)
	wp->w_wantline = 1;
    如果 (wp->w_wantline > Rows)
	wp->w_wantline = Rows;
    wp->w_wantcol = drag_start_wantcol + (mouse_col - drag_start_col);
    如果 (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    如果 (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    void
popup_set_firstline(win_T *wp)
{
    整数	    height = wp->w_height;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    当 (wp->w_firstline > 1 && wp->w_height < height)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    整数
popup_is_in_scrollbar(win_T *wp, 整数 row, 整数 col)
{
    返回 wp->w_has_scrollbar
	&& row >= wp->w_popup_border[0]
	&& row < popup_height(wp) - wp->w_popup_border[2]
	&& col == popup_width(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    void
popup_handle_scrollbar_click(win_T *wp, 整数 row, 整数 col)
{
    如果 (!popup_is_in_scrollbar(wp, row, col))
	返回;

    整数	    height = popup_height(wp);
    整数	    new_topline = wp->w_topline;

    如果 (row >= height / 2)
    {
	// Click in lower half, scroll down.
	如果 (wp->w_topline < wp->w_buffer->b_ml.ml_line_count)
	    ++new_topline;
    }
    否则 如果 (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    如果 (new_topline == wp->w_topline)
	返回;

    set_topline(wp, new_topline);
    如果 (wp == curwin)
    {
	如果 (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	否则 如果 (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_set_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#如果 defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    static void
popup_add_timeout(win_T *wp, 整数 time, 整数 close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((char *)cbbuf, sizeof(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    如果 (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	返回;

    wp->w_popup_timer = create_timer(time, 0);
    callback_T cb = get_callback(&tv);
    如果 (cb.cb_name != NULL && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#endif

    static poppos_T
get_pos_entry(dict_T *d, 整数 give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    整数	    nr;

    如果 (str == NULL)
	返回 POPPOS_NONE;

    为 (nr = 0; nr < (整数)ARRAY_LENGTH(poppos_entries); ++nr)
	如果 (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    返回 poppos_entries[nr].pp_val;

    如果 (give_error)
	semsg(_(e_invalid_argument_str), str);
    返回 POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    static void
apply_move_options(win_T *wp, dict_T *d)
{
    整数		nr;
    char_u	*str;
    dictitem_T	*di;

    如果 ((nr = dict_get_number_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    如果 ((nr = dict_get_number_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    如果 ((nr = dict_get_number_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    如果 ((nr = dict_get_number_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    如果 (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    如果 (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    如果 (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_entry(d, TRUE);

	如果 (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    如果 (str != NULL)
    {
	wp->w_popup_prop_type = 0;
	如果 (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_find(d, (char_u *)"textpropwin", -1);
	    如果 (di != NULL)
	    {
		wp->w_popup_prop_win = find_win_by_nr_or_id(&di->di_tv);
		如果 (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = find_prop_type_id(str, wp->w_popup_prop_win->w_buffer);
	    如果 (nr <= 0)
		nr = find_prop_type_id(str, NULL);
	    如果 (nr <= 0)
		semsg(_(e_invalid_argument_str), str);
	    否则
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_find(d, (char_u *)"textpropid", -1);
    如果 (di != NULL)
	wp->w_popup_prop_id = dict_get_number(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    static void
handle_moved_argument(win_T *wp, dictitem_T *di, 整数 mousemoved)
{
    如果 (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != NULL)
    {
	char_u  *s = di->di_tv.vval.v_string;
	整数	flags = 0;

	如果 (STRCMP(s, "word") == 0)
	    flags = FIND_IDENT | FIND_STRING;
	否则 如果 (STRCMP(s, "WORD") == 0)
	    flags = FIND_STRING;
	否则 如果 (STRCMP(s, "expr") == 0)
	    flags = FIND_IDENT | FIND_STRING | FIND_EVAL;
	否则 如果 (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_argument_str), s);
	如果 (flags != 0)
	{
	    如果 (mousemoved)
		set_mousemoved_columns(wp, flags);
	    否则
		set_moved_columns(wp, flags);
	}
    }
    否则 如果 (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != NULL
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	整数	    mincol;
	整数	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
	如果 (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_number(&l->lv_first->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    如果 (mousemoved)
		wp->w_popup_mouse_row = nr;
	    否则
		wp->w_popup_lnum = nr;
	    li = li->li_next;
	    如果 (nr == 0)
		wp->w_popup_curwin = NULL;
	}

	mincol = tv_get_number(&li->li_tv);
	maxcol = tv_get_number(&li->li_next->li_tv);
	如果 (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	否则
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    否则
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
}

    static void
check_highlight(dict_T *dict, char *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_find(dict, (char_u *)name, -1);
    如果 (di == NULL)
	返回;

    如果 (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_value_for_argument_str), name);
    否则
    {
	str = tv_get_string(&di->di_tv);
	如果 (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    static void
popup_show_curline(win_T *wp)
{
    如果 (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    否则 如果 (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_height + 1;
	如果 (wp->w_topline < 1)
	    wp->w_topline = 1;
	否则 如果 (wp->w_topline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_topline = wp->w_buffer->b_ml.ml_line_count;
	当 (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_buffer->b_ml.ml_line_count
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_height + 1) > wp->w_height)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    如果 (wp->w_firstline > 0)
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
    返回 (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    static void
popup_highlight_curline(win_T *wp)
{
    整数	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_buffer, (char_u *)"PopUpMenu");

    如果 ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	如果 (!sign_exists_by_name(sign_name))
	{
	    char *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, NULL, (char_u *)linehl, NULL, NULL, NULL,
		    NULL, SIGN_DEF_PRIO);
	}

	sign_place(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_buffer, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    否则
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_last_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    static 整数
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    整数		nr;
    char_u	*str;

#ifdef FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#endif

    // TODO: flip

    di = dict_find(dict, (char_u *)"firstline", -1);
    如果 (di != NULL)
    {
	wp->w_firstline = dict_get_number(dict, "firstline");
	如果 (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    如果 (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    如果 (str != NULL)
    {
	char_u	*title = vim_strsave(str);

	如果 (title != NULL)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    如果 (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	否则
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	否则
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	否则
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	否则
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	否则
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_find(dict, (char_u *)"opacity", -1);
    如果 (di != NULL)
    {
	nr = dict_get_number(dict, "opacity");
	如果 (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	否则 如果 (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	否则 如果 (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	否则
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#ifdef FEAT_IMAGE_POPUP
    如果 (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    否则
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_find(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    如果 (di != NULL && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != NULL)
    {
	image_T		    *img;
	image_T		    *cur_img = NULL;
	image_placement_T   *place;
	整数		    cw, ch;

	如果 (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    如果 (wp->w_popup_imagep != NULL)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = NULL;
	    }
	    goto image_done;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	如果 (wp->w_popup_imagep != NULL && wp->w_popup_imagep->img != NULL)
	    cur_img = wp->w_popup_imagep->img;

	img = add_image(di->di_tv.vval.v_dict, cur_img, true);
	如果 (img == NULL)
	    返回 FAIL;

	如果 (img == cur_img)
	    goto image_done;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	place = image_placement_new(img, wp->w_id, iplace_use, false);

	如果 (place == NULL)
	{
	    image_unref(img);
	    返回 FAIL;
	}

	如果 (wp->w_popup_imagep != NULL)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = place;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	如果 (dict_find(dict, (char_u *)"minwidth", -1) == NULL)
	    wp->w_minwidth = cw;
	如果 (dict_find(dict, (char_u *)"maxwidth", -1) == NULL)
	    wp->w_maxwidth = cw;
	如果 (dict_find(dict, (char_u *)"minheight", -1) == NULL)
	    wp->w_minheight = ch;
	如果 (dict_find(dict, (char_u *)"maxheight", -1) == NULL)
	    wp->w_maxheight = ch;
image_done:
	;
    }
    否则
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	如果 (wp->w_popup_imagep != NULL)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(NULL, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	如果 (wp->w_popup_imagep != NULL)
	    image_placement_set_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_height, wp->w_width);
    }
#endif

    di = dict_find(dict, (char_u *)"close", -1);
    如果 (di != NULL)
    {
	整数 ok = TRUE;

	如果 (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != NULL)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    如果 (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    否则 如果 (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    否则 如果 (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    否则
		ok = FALSE;
	}
	否则
	    ok = FALSE;
	如果 (!ok)
	    semsg(_(e_invalid_value_for_argument_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    如果 (str != NULL)
    {
	char *errmsg = update_wincolor(wp, str);

	如果 (errmsg == NULL)
	    set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	否则
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    如果 (str != NULL)
    {
	char *errmsg = update_winhighlight(wp, str);

	如果 (errmsg == NULL)
	    set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	否则
	    emsg(_(errmsg));
    }

    如果 (set_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	set_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	返回 FAIL;

    di = dict_find(dict, (char_u *)"borderhighlight", -1);
    如果 (di != NULL)
    {
	如果 (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == NULL)
	{
	    emsg(_(e_list_required));
	    返回 FAIL;
	}
	否则
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    整数		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    如果 (list->lv_len == 0)
	    {
		为 (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = NULL;
		}
	    }
	    否则
	    {
		为 (i = 0, li = list->lv_first; i < 4 && i < list->lv_len;
						     ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    如果 (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			如果 (hl != NULL)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		如果 (list->lv_len == 1 && wp->w_border_highlight[0] != NULL)
		    为 (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			如果 (hl != NULL)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_find(dict, (char_u *)"borderchars", -1);
    如果 (di != NULL)
    {
	如果 (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    返回 FAIL;
	}
	否则
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    整数		i;

	    如果 (list != NULL)
	    {
		CHECK_LIST_MATERIALIZE(list);
		为 (i = 0, li = list->lv_first; i < 8 && i < list->lv_len;
							 ++i, li = li->li_next)
		{
		    str = tv_get_string(&li->li_tv);
		    如果 (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		如果 (list->lv_len == 1)
		    为 (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		如果 (list->lv_len == 2)
		{
		    为 (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    为 (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_find(dict, (char_u *)"zindex", -1);
    如果 (di != NULL)
    {
	wp->w_zindex = dict_get_number(dict, "zindex");
	如果 (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	如果 (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_find(dict, (char_u *)"mask", -1);
    如果 (di != NULL)
    {
	整数 ok = FALSE;

	如果 (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != NULL)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_ITEMS(di->di_tv.vval.v_list, li)
	    {
		如果 (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == NULL
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    break;
		}
		否则
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	如果 (ok)
	{
	    wp->w_popup_mask = di->di_tv.vval.v_list;
	    ++wp->w_popup_mask->lv_refcount;
	    VIM_CLEAR(wp->w_popup_mask_cells);
	}
	否则
	{
	    semsg(_(e_invalid_value_for_argument_str), "mask");
	    返回 FAIL;
	}
    }

#如果 defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_number(dict, "time");
    如果 (nr > 0)
	popup_add_timeout(wp, nr, TRUE);
#endif

    di = dict_find(dict, (char_u *)"moved", -1);
    如果 (di != NULL)
    {
	set_moved_values(wp);
	handle_moved_argument(wp, di, FALSE);
    }

    di = dict_find(dict, (char_u *)"mousemoved", -1);
    如果 (di != NULL)
    {
	set_mousemoved_values(wp);
	handle_moved_argument(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    如果 (nr != -1)
    {
	如果 (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	否则
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_find(dict, (char_u *)"filter", -1);
    如果 (di != NULL)
    {
	callback_T	callback = get_callback(&di->di_tv);

	如果 (callback.cb_name != NULL)
	{
	    free_callback(&wp->w_filter_cb);
	    set_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    如果 (nr != -1)
    {
	如果 (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	否则
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    如果 (str != NULL)
    {
	如果 (STRCMP(str, "a") == 0)
	    wp->w_filter_mode = MODE_ALL;
	否则
	    wp->w_filter_mode = mode_str2flags(str);
    }

    di = dict_find(dict, (char_u *)"callback", -1);
    如果 (di == NULL)
	返回 OK;

    callback_T	callback = get_callback(&di->di_tv);
    如果 (callback.cb_name == NULL)
	返回 OK;

    free_callback(&wp->w_close_cb);
    set_callback(&wp->w_close_cb, &callback);
    返回 OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    static 整数
apply_options(win_T *wp, dict_T *dict, 整数 create)
{
    整数		nr;

    apply_move_options(wp, dict);

    如果 (create)
	set_string_option_direct_in_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    如果 (apply_general_options(wp, dict) == FAIL)
	返回 FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    如果 (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    如果 (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	如果 (wp->w_firstline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_line_count;
	否则 如果 (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_height)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    如果 (create)
	popup_mask_refresh = TRUE;
    popup_highlight_curline(wp);

    返回 OK;
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
	如果 (li->li_tv.v_type == VAR_STRING)
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
	如果 (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_argument_1_list_item_nr_dictionary_required), lnum + 1);
	    返回;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == NULL ? NULL : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == NULL ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    为 (li = l->lv_first; li != NULL; li = li->li_next, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_find(dict, (char_u *)"props", -1);
	如果 (di != NULL)
	{
	    如果 (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		返回;
	    }
	    plist = di->di_tv.vval.v_list;
	    如果 (plist != NULL)
	    {
		FOR_ALL_LIST_ITEMS(plist, pli)
		{
		    如果 (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			返回;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    如果 (dict != NULL)
		    {
			整数 col = dict_get_number(dict, "col");

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
    整数
popup_top_extra(win_T *wp)
{
    整数	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    如果 (extra == 0 && wp->w_popup_title != NULL && *wp->w_popup_title != NUL)
	返回 1;
    返回 extra;
}

/*
 * Get the padding plus border at the left.
 */
    整数
popup_left_extra(win_T *wp)
{
    返回 wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    整数
popup_height(win_T *wp)
{
    返回 wp->w_height
	    + popup_top_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    整数
popup_width(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    返回 wp->w_width
	    + popup_extra_width(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    整数
popup_extra_width(win_T *wp)
{
    返回 wp->w_popup_padding[3] + wp->w_popup_border[3]
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
    如果 (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	返回 NULL;
    如果 (win_valid(wp->w_popup_prop_win))
	返回 wp->w_popup_prop_win;
    返回 NULL;
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
    整数 top_extra;
    整数 bot_extra;
    整数 left_extra;
    整数 right_extra;

    整数 clip_top_content;
    整数 clip_bot_content;
    整数 clip_left_content;
    整数 clip_right_content;

    整数 eff_top_extra;
    整数 eff_bot_extra;
    整数 eff_left_extra;
    整数 eff_right_extra;

    整数 eff_border[4];
    整数 eff_padding[4];

    整数 eff_height;
    整数 eff_width;
} popup_clip_T;

    static void
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    整数 h, w;

    cl->top_extra = popup_top_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->left_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->right_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_top_content = wp->w_popup_topoff - cl->top_extra;
    如果 (cl->clip_top_content < 0)
	cl->clip_top_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    如果 (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_left_content = wp->w_popup_leftclip - cl->left_extra;
    如果 (cl->clip_left_content < 0)
	cl->clip_left_content = 0;
    cl->clip_right_content = wp->w_popup_rightclip - cl->right_extra;
    如果 (cl->clip_right_content < 0)
	cl->clip_right_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	整数 clip = wp->w_popup_topoff;
	整数 b = wp->w_popup_border[0];
	整数 p = wp->w_popup_padding[0];
	整数 rem;

	如果 (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	否则
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	整数 clip = wp->w_popup_bottomoff;
	整数 b = wp->w_popup_border[2];
	整数 p = wp->w_popup_padding[2];
	整数 rem;

	如果 (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	否则
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
	整数 excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_height;
	如果 (excess > 0)
	{
	    如果 (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		如果 (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		否则
		    cl->eff_border[0] -= excess;
	    }
	    否则
		cl->eff_padding[0] -= excess;
	}
    }
    {
	整数 excess = wp->w_popup_topoff - cl->top_extra - wp->w_height;
	如果 (excess > 0)
	{
	    如果 (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		如果 (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		否则
		    cl->eff_border[2] -= excess;
	    }
	    否则
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_top_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_left_extra = wp->w_popup_leftclip > 0 ? 0 : cl->left_extra;
    cl->eff_right_extra = wp->w_popup_rightclip > 0 ? 0 : cl->right_extra;

    h = wp->w_height - cl->clip_top_content - cl->clip_bot_content;
    如果 (h < 0)
	h = 0;
    cl->eff_height = cl->eff_top_extra + h + cl->eff_bot_extra;

    w = wp->w_width - cl->clip_left_content - cl->clip_right_content;
    如果 (w < 0)
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
    整数		height;
    整数		width;
    整数		winrow;
    整数		wincol;
    整数		leftcol;
    整数		p_wrap;
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
	整数	    *screen_row,
	整数	    *screen_scol,
	整数	    *screen_ccol,
	整数	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    整数	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (整数)win_col_off(prop_win) - (整数)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (整数)scol + coloff;
    *screen_ccol = (整数)ccol + coloff;
    *screen_ecol = (整数)ecol + coloff;
    *screen_row = prop_win->w_winrow + 1
				 - (整数)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    static void
popup_hide_for_textprop(win_T *wp)
{
    如果 ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	返回;
    wp->w_popup_flags |= POPF_HIDDEN;
    如果 (win_valid(wp->w_popup_prop_win))
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
	整数	    max_reach,
	textprop_T  *prop,
	linenr_T    *found_lnum)
{
    linenr_T	first;

    如果 (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	返回 false;

    first = max_reach > 0 ? prop_win->w_topline - max_reach : 1;
    如果 (first < 1)
	first = 1;
    返回 find_prop_in_lines(prop_win,
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
    整数	    extra_h, extra_w;
    整数	    popup_top, popup_bottom, popup_left, popup_right;
    整数	    total_h, total_w;

    如果 (cw == NULL)
	返回 false;

    extra_h = popup_top_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_width(wp);

    popup_top = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_height + extra_h;
    popup_left = wp->w_wincol;
    popup_right = wp->w_wincol + wp->w_width + extra_w;
    total_h = wp->w_height + extra_h;
    total_w = wp->w_width + extra_w;

    如果 (popup_top < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_top;
    如果 (popup_bottom > cw->w_winrow + cw->w_height)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_height);
    如果 (popup_left < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_left;
    如果 (popup_right > cw->w_wincol + cw->w_width)
	wp->w_popup_rightclip = popup_right - (cw->w_wincol + cw->w_width);

    返回 wp->w_popup_topoff >= total_h
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
    如果 (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_height -= cl->clip_top_content + cl->clip_bot_content;
	如果 (wp->w_height < 0)
	    wp->w_height = 0;
	如果 (cl->clip_top_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_top_content;
    }
    如果 (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	如果 (cl->clip_left_content > 0 || cl->clip_right_content > 0)
	    wp->w_p_wrap = FALSE;
	如果 (cl->clip_right_content > 0)
	{
	    wp->w_width -= cl->clip_right_content;
	    如果 (wp->w_width < 0)
		wp->w_width = 0;
	}
	如果 (cl->clip_left_content > 0)
	{
	    wp->w_leftcol += cl->clip_left_content;
	    wp->w_wincol += cl->clip_left_content;
	    wp->w_width -= cl->clip_left_content;
	    如果 (wp->w_width < 0)
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

    整数 iw, ih;

    // Visible cell width and height of popup itself (not image!)
    整数	visible_width;
    整数	visible_height;

    // Crop dimensions of image (in pixels).
    整数	crop_x, crop_y, crop_width, crop_height;

    popup_clip_T clip;

    如果 (wp->w_popup_imagep == NULL || wp->w_popup_flags & POPF_HIDDEN)
	返回;

    image_placement_set_enable(wp->w_popup_imagep, true);

    row = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    col = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    如果 (wp->w_popup_imagep->img == NULL)
	// Just set the bounding box
	goto exit;

    visible_width =
	wp->w_width - clip.clip_left_content - clip.clip_right_content;
    visible_height =
	wp->w_height - clip.clip_top_content - clip.clip_bot_content;

    如果 (visible_width < 0 || visible_height < 0)
	返回;

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
    如果 (crop_width > visible_width)
	crop_width = visible_width;
    如果 (crop_height > visible_height)
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
    整数		wrapped = 0;
    整数		maxwidth;
    整数		maxwidth_no_scrollbar;
    整数		width_with_scrollbar = 0;
    整数		used_maxwidth = FALSE;
    整数		margin_width = 0;
    整数		maxspace;
    整数		center_vert = FALSE;
    整数		center_hor = FALSE;
    整数		allow_adjust_left = !wp->w_popup_fixed;
    整数		top_extra = popup_top_extra(wp);
    整数		right_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    整数		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    整数		left_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    整数		extra_height = top_extra + bot_extra;
    整数		extra_width = left_extra + right_extra;
    整数		w_height_before_limit;
    popup_layout_T org_layout;
    整数		minwidth, minheight;
    整数		maxheight = Rows;
    整数		wantline = wp->w_wantline;  // adjusted for textprop
    整数		wantcol = wp->w_wantcol;    // adjusted for textprop
    整数		use_wantcol = wantcol != 0;
    整数		adjust_height_for_top_aligned = FALSE;

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
    如果 (wp->w_popup_last_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    如果 (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	整数	    screen_row;
	整数	    screen_scol;
	整数	    screen_ccol;
	整数	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_top = false;
	如果 (!find_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    如果 (popup_find_prop_above_top(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_top = true;
	    否则
	    {
		如果 ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#ifdef FEAT_IMAGE_POPUP
		    如果 (wp->w_popup_imagep != NULL)
			image_placement_clear(wp->w_popup_imagep);
#endif
		    popup_hide_for_textprop(wp);
		    如果 (wp->w_winrow + popup_height(wp) >= cmdline_row)
			clear_cmdline = TRUE;
		}
		返回;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.col = prop.tp_col;
	如果 (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.col += prop.tp_len - 1;
	如果 (prop_above_top)
	    popup_screenpos_above_top(prop_win, &pos, prop_lnum, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);
	否则
	    textpos2screenpos(prop_win, &pos, &screen_row,
				     &screen_scol, &screen_ccol, &screen_ecol);

	如果 (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_width = 0;
	    返回;
	}
	如果 (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_row + wantline + 1;
	否则
	    // above the text
	    wantline = screen_row + wantline - 1;
	center_vert = FALSE;
	如果 (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	否则
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    否则
    {
	// If no line was specified default to vertical centering.
	如果 (wantline == 0)
	    center_vert = TRUE;
	否则 如果 (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	如果 (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    如果 (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    否则
    {
	如果 (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    如果 (wp->w_winrow >= Rows)
		wp->w_winrow = Rows - 1;
	}
	如果 (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_row
				    - wp->w_buffer->b_ml.ml_line_count - 1, 0);
	}

	如果 (!use_wantcol)
	    center_hor = TRUE;
	否则 如果 (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    如果 (wp->w_wincol > firstwin->w_wincol + topframe->fr_width
						- left_extra - right_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- left_extra - right_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    如果 (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_width
					- wp->w_wincol - left_extra - right_extra;
    maxwidth = maxspace;
    如果 (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_left = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    如果 (wp->w_p_nu || wp->w_p_rnu)
	margin_width = number_width(wp) + 1;
#ifdef FEAT_FOLDING
    margin_width += wp->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
    如果 (signcolumn_on(wp))
	margin_width += 2;
#endif
    如果 (margin_width >= maxwidth)
	margin_width = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#ifdef FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    如果 (wp->w_buffer->b_term != NULL)
    {
	如果 (minwidth == 0)
	    minwidth = 20;
	如果 (minheight == 0)
	    minheight = 5;
    }
#endif

    如果 (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    否则 如果 (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_row - 1;

    // start at the desired first line
    如果 (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	如果 (wp->w_firstline > wp->w_buffer->b_ml.ml_line_count)
	    wp->w_firstline = 0;
	否则
	    wp->w_topline = wp->w_firstline;
    }
    如果 (wp->w_topline < 1)
	wp->w_topline = 1;
    否则 如果 (wp->w_topline > wp->w_buffer->b_ml.ml_line_count)
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
	整数 saved_w_width = wp->w_width;

	如果 (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	为 (ln = 1; ln <= wp->w_buffer->b_ml.ml_line_count; ++ln)
	{
	    整数 len = linetabsize(wp, ln) + margin_width;

	    如果 (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    如果 (saved_w_width < len)
		saved_w_width = len;
	    如果 (wp->w_maxwidth > 0 && saved_w_width >= wp->w_maxwidth)
		break;
	}
	wp->w_width = saved_w_width;
    }
    如果 (wp->w_firstline < 0)
	lnum = wp->w_buffer->b_ml.ml_line_count;
    否则
	lnum = wp->w_topline;
    当 (lnum >= 1 && lnum <= wp->w_buffer->b_ml.ml_line_count)
    {
	整数 len;
	整数 w_width = wp->w_width;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	如果 (wp->w_width < maxwidth)
	    wp->w_width = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_width = w_width;

	如果 (len + margin_width > maxwidth
		&& allow_adjust_left
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    整数 shift_by = len + margin_width - maxwidth;

	    如果 (shift_by > wp->w_wincol)
	    {
		整数 truncate_shift = shift_by - wp->w_wincol;

		shift_by -= truncate_shift;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    如果 (wp->w_maxwidth > 0 && maxwidth + shift_by > wp->w_maxwidth)
		shift_by = wp->w_maxwidth - maxwidth;

	    如果 (shift_by > 0)
	    {
		wp->w_wincol -= shift_by;
		maxwidth += shift_by;
	    }
	    wp->w_width = maxwidth;
	}
	如果 (wp->w_p_wrap)
	{
	    当 (len + margin_width > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_width;
		wp->w_width = maxwidth;
		used_maxwidth = TRUE;
	    }
	}
	如果 (wp->w_width < len + margin_width)
	{
	    wp->w_width = len + margin_width;
	    如果 (wp->w_maxwidth > 0 && wp->w_width > wp->w_maxwidth)
		wp->w_width = wp->w_maxwidth;
	}

	如果 (wp->w_firstline < 0)
	    --lnum;
	否则
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	如果 (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_buffer->b_ml.ml_line_count - lnum)
		       + wrapped >= maxheight)
	    break;
    }

    如果 (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_buffer->b_ml.ml_line_count);
#ifdef FEAT_TERMINAL
    如果 (wp->w_buffer->b_term != NULL && !term_is_finished(wp->w_buffer))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#endif
    maxwidth_no_scrollbar = maxwidth;
    如果 (wp->w_has_scrollbar)
    {
	++right_extra;
	++extra_width;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	如果 (maxwidth + right_extra >= maxspace
		&& (used_maxwidth || (minwidth > 0 && wp->w_width < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    如果 (wp->w_popup_title != NULL && *wp->w_popup_title != NUL)
    {
	整数 title_len = vim_strsize(wp->w_popup_title) + 2 - extra_width;

	如果 (minwidth < title_len)
	    minwidth = title_len;
    }

    如果 (minwidth > 0 && wp->w_width < minwidth)
	wp->w_width = minwidth;
    如果 (wp->w_width > maxwidth)
    {
	如果 (wp->w_width > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_width - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	如果 (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    整数 off = wp->w_width - maxwidth;
	    extra_width -= MIN(off, right_extra);
	    wp->w_width = maxwidth_no_scrollbar;
	}
	否则
	{
	    wp->w_width = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    width_with_scrollbar = maxwidth_no_scrollbar - right_extra;
	}
    }
    如果 (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_width - wp->w_width - extra_width) / 2;
	如果 (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    否则 如果 (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	整数 leftoff = wantcol - (wp->w_width + extra_width);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	如果 (leftoff >= 0)
	    wp->w_wincol = leftoff;
	否则 如果 (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_width -= wp->w_leftcol;
	    如果 (wp->w_width < 0)
		wp->w_width = 0;
	}
    }

    如果 (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	整数 want_col = 0;

	// try to show the right border and any scrollbar
	want_col = left_extra + wp->w_width + right_extra;
	如果 (want_col > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_col >= firstwin->w_wincol + topframe->fr_width)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_width - want_col;
	    如果 (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_height = wp->w_buffer->b_ml.ml_line_count - wp->w_topline
								 + 1 + wrapped;
    如果 (minheight > 0 && wp->w_height < minheight)
	wp->w_height = minheight;
    如果 (maxheight > 0 && wp->w_height > maxheight)
	wp->w_height = maxheight;
    w_height_before_limit = wp->w_height;
    如果 (wp->w_height > Rows - wp->w_winrow)
	wp->w_height = Rows - wp->w_winrow;

    如果 (center_vert)
    {
	wp->w_winrow = (Rows - wp->w_height - extra_height) / 2;
	如果 (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    否则 如果 (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	如果 ((wp->w_height + extra_height) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_height + extra_height);
	否则 如果 (wantline * 2 >= Rows || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_height = wantline - extra_height;
	}
	否则
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_height_for_top_aligned = TRUE;
	}
    }
    否则 如果 (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	整数 check_height = (wp->w_popup_flags & POPF_INFO) ? wp->w_height
						    : w_height_before_limit;
	如果 (wp != popup_dragwin
		&& wantline + (check_height + extra_height) - 1 > Rows
		&& wantline * 2 > Rows
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_height = w_height_before_limit;
	    wp->w_winrow = wantline - 2 - wp->w_height - extra_height;
	    如果 (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_height + 2;
	    如果 (wp->w_winrow < 0)
	    {
		wp->w_height += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	否则
	{
	    wp->w_winrow = wantline - 1;
	    adjust_height_for_top_aligned = TRUE;
	}
    }

    如果 (adjust_height_for_top_aligned && wp->w_want_scrollbar
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
	如果 (wp->w_buffer->b_term == NULL || term_is_finished(wp->w_buffer))
#endif
	{
	    // The width already accounts for a scrollbar that was there.
	    如果 (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		如果 (width_with_scrollbar > 0)
		    wp->w_width = width_with_scrollbar;
		否则
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_width;
	    }
	}
    }

    // make sure w_winrow is valid
    如果 (wp->w_winrow >= Rows)
	wp->w_winrow = Rows - 1;
    否则 如果 (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    如果 (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_width
						- wp->w_width - extra_width;
    如果 (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    如果 (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    如果 (wp->w_wincol + wp->w_width + extra_width
				    > firstwin->w_wincol + topframe->fr_width)
    {
	整数 avail = firstwin->w_wincol + topframe->fr_width
						- wp->w_wincol - extra_width;
	wp->w_width = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    如果 (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	如果 (wp->w_winrow + wp->w_height + extra_height > Rows)
	    wp->w_winrow = Rows - wp->w_height - extra_height;
	如果 (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	如果 (wp->w_winrow + wp->w_height + extra_height > Rows)
	{
	    整数 avail = Rows - wp->w_winrow - extra_height;
	    wp->w_height = avail > 0 ? avail : 0;
	}
    }

    如果 (wp->w_height != org_layout.height)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    如果 (popup_compute_clipwindow_offsets(wp))
    {
#ifdef FEAT_IMAGE_POPUP
	如果 (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    如果 (wp->w_popup_imagep != NULL)
		image_placement_clear(wp->w_popup_imagep);
#endif

	popup_hide_for_textprop(wp);
	返回;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_last_changedtick = CHANGEDTICK(wp->w_buffer);
    如果 (win_valid(wp->w_popup_prop_win))
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
    如果 (popup_layout_changed(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	如果 (wp->w_popup_flags & POPF_ON_CMDLINE)
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
    static 整数
popup_is_notification(create_type_T type)
{
    返回 type == TYPE_NOTIFICATION || type == TYPE_MESSAGE_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    static void
popup_set_buffer_text(buf_T *buf, typval_T text)
{
    整数	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    为 (lnum = buf->b_ml.ml_line_count; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_buffer;

    // Add text to the buffer.
    如果 (text.v_type == VAR_STRING)
    {
	char_u *s = text.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == NULL ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    否则
    {
	list_T *l = text.vval.v_list;

	如果 (l != NULL && l->lv_len > 0)
	{
	    如果 (l->lv_first == &range_list_item)
		emsg(_(e_using_number_as_string));
	    否则 如果 (l->lv_first->li_tv.v_type == VAR_STRING)
		// list of strings
		add_popup_strings(buf, l);
	    否则
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
	如果 (wp != NULL)					    \
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
    } 当 (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    static 整数
parse_popup_option(win_T *wp, 整数 is_preview)
{
    char_u *p =
#ifdef FEAT_QUICKFIX
	!is_preview ? p_cpp :
#endif
	p_pvp;
    整数	    border_enabled = FALSE;

    如果 (wp != NULL)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    为 ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	整数	x;

	e = vim_strchr(p, ':');
	如果 (e == NULL || e[1] == NUL)
	    返回 FAIL;

	p = vim_strchr(e, ',');
	如果 (p == NULL)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	如果 (STRNCMP(s, "height:", 7) == 0)
	{
	    如果 (dig != p)
		返回 FAIL;
	    如果 (wp != NULL)
	    {
		如果 (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	否则 如果 (STRNCMP(s, "width:", 6) == 0)
	{
	    如果 (dig != p)
		返回 FAIL;
	    如果 (wp != NULL)
	    {
		如果 (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	否则 如果 (STRNCMP(s, "highlight:", 10) == 0)
	{
	    如果 (wp != NULL)
	    {
		char *errmsg;
		整数 c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		如果 (errmsg == NULL)
		    set_string_option_direct_in_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		否则
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	否则 如果 (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    如果 (*arg == NUL || *arg == ',')
		返回 FAIL;
	    如果 (wp != NULL)
	    {
		为 (整数 i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	否则 如果 (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    整数		i;
	    整数		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    整数		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    如果 (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		如果 (wp != NULL)
		{
		    为 (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		continue;
	    }

	    token = vim_strnsave(arg, token_len);
	    如果 (token == NULL)
		返回 FAIL;

	    如果 ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		如果 (STRCMP(token, "single") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		否则 如果 (STRCMP(token, "double") == 0)
		    SET_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		否则 如果 (STRCMP(token, "round") == 0)
		    SET_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		否则 如果 (STRCMP(token, "on") == 0)
		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		否则 如果 (STRCMP(token, "ascii") == 0)
		    SET_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		否则 如果 (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    整数		out[8];
		    整数		failed = FALSE;

		    SET_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    为 (i = 0; i < 8 && !failed; i++)
		    {
			如果 (*q == NUL)
			    failed = TRUE;
			否则
			{
			    out[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    如果 (i < 7)
			    {
				如果 (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    如果 (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			返回 FAIL;
		    }
		    SET_BORDER_CHARS(out[0], out[1], out[2], out[3], out[4],
			    out[5], out[6], out[7]);
		}
	    }
	    否则
	    {
		vim_free(token);
		返回 FAIL;
	    }

	    如果 (wp != NULL)
	    {
		为 (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	否则 如果 (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    整数		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    整数		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    如果 (!on && !off)
		返回 FAIL;
	    on = on && mouse_has(MOUSE_INSERT) && (border_enabled || is_preview);
	    如果 (wp != NULL)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	否则 如果 (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    整数		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    整数		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    如果 (!on && !off)
		返回 FAIL;
	    如果 (wp != NULL)
	    {
		如果 (on && mouse_has(MOUSE_INSERT))
		    wp->w_popup_flags |= POPF_RESIZE;
		否则
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	否则 如果 (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    整数		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    整数		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    如果 (!on && !off)
		返回 FAIL;
	    如果 (wp != NULL)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	否则 如果 (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    整数		item = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    整数		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    如果 (!menu && !item)
		返回 FAIL;
	    如果 (wp != NULL && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	否则 如果 (STRNCMP(s, "opacity:", 8) == 0)
	{
	    如果 (dig != p || x < 0 || x > 100)
		返回 FAIL;
	    如果 (wp != NULL)
	    {
		如果 (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		否则
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	否则
	    返回 FAIL;
    }
    返回 OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    整数
parse_previewpopup(win_T *wp)
{
    返回 parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    整数
parse_completepopup(win_T *wp)
{
    返回 parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    void
popup_set_wantpos_cursor(win_T *wp, 整数 width, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    如果 (d != NULL)
	ppt = get_pos_entry(d, FALSE);

    setcursor_mayforce(TRUE);
    如果 (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    否则
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	如果 (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    如果 (wp->w_wantcol > Columns - width)
    {
	wp->w_wantcol = Columns - width;
	如果 (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    void
popup_set_wantpos_rowcol(win_T *wp, 整数 row, 整数 col)
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
    整数 i;

    为 (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#ifdef FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    static 整数
popup_terminal_exists(void)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	如果 (wp->w_buffer->b_term != NULL)
	    返回 TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    如果 (wp->w_buffer->b_term != NULL)
		返回 TRUE;
    返回 FALSE;
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
    整数
popup_need_redraw(void)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	如果 ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    返回 TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	如果 ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    返回 TRUE;
    返回 FALSE;
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
    如果 (errmsg == NULL)
	set_string_option_direct_in_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    否则
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
    整数		tabnr = 0;
    整数		new_buffer;
    buf_T	*buf = NULL;
    dict_T	*d = NULL;
    整数		i;

    如果 (check_secure())
	返回 NULL;

    如果 (argvars != NULL)
    {
	如果 (in_vim9script()
		&& (check_for_string_or_number_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    返回 NULL;

	// Check that arguments look OK.
	如果 (argvars[0].v_type == VAR_NUMBER)
	{
	    buf = buflist_findnr(argvars[0].vval.v_number);
	    如果 (buf == NULL)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[0].vval.v_number);
		返回 NULL;
	    }
#ifdef FEAT_TERMINAL
	    如果 (buf->b_term != NULL && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_second_popup_with_terminal));
		返回 NULL;
	    }
#endif
	}
	否则 如果 (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != NULL)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != NULL))
	{
	    emsg(_(e_buffer_number_text_or_list_required));
	    返回 NULL;
	}
	如果 (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    返回 NULL;
	d = argvars[1].vval.v_dict;
    }

    如果 (d != NULL)
    {
	如果 (dict_has_key(d, "tabpage"))
	    tabnr = (整数)dict_get_number(d, "tabpage");
	否则 如果 (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	否则
	    tabnr = 0;
	如果 (tabnr > 0)
	{
	    tp = find_tabpage(tabnr);
	    如果 (tp == NULL)
	    {
		semsg(_(e_tabpage_not_found_nr), tabnr);
		返回 NULL;
	    }
	}
    }
    否则 如果 (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    如果 (buf != NULL && buf->b_locked_split)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_window_to_a_closing_buffer));
	返回 NULL;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    如果 (wp == NULL)
	返回 NULL;
    如果 (rettv != NULL)
	rettv->vval.v_number = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    如果 (buf != NULL)
    {
	// use existing buffer
	new_buffer = FALSE;
	win_init_popup_win(wp, buf);
	set_local_options_default(wp, FALSE);
	swap_exists_action = SEA_READONLY;
	buffer_ensure_loaded(buf);
	swap_exists_action = SEA_NONE;
    }
    否则
    {
	// create a new buffer associated with the popup
	new_buffer = TRUE;
	buf = buflist_new(NULL, NULL, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	如果 (buf == NULL)
	{
	    win_free_popup(wp);
	    返回 NULL;
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

    如果 (tp != NULL)
    {
	// popup on specified tab page
	wp->w_next = tp->tp_first_popupwin;
	tp->tp_first_popupwin = wp;
    }
    否则 如果 (tabnr == 0)
    {
	// popup on current tab page
	wp->w_next = curtab->tp_first_popupwin;
	curtab->tp_first_popupwin = wp;
    }
    否则 // (tabnr < 0)
    {
	win_T *prev = first_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	如果 (first_popupwin == NULL)
	    first_popupwin = wp;
	否则
	{
	    当 (prev->w_next != NULL)
		prev = prev->w_next;
	    prev->w_next = wp;
	}
#ifdef FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#endif
    }

    如果 (new_buffer && argvars != NULL)
	popup_set_buffer_text(buf, argvars[0]);

    如果 (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    如果 (type == TYPE_ATCURSOR)
    {
	popup_set_wantpos_cursor(wp, 0, d);
	set_moved_values(wp);
	set_moved_columns(wp, FIND_STRING);
    }

    如果 (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_row;
	如果 (wp->w_wantline <= 0)  // mouse on first line
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

    如果 (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	整数	height = buf->b_ml.ml_line_count + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	为 (twp = first_popupwin; twp != NULL; twp = nextwin)
	{
	    nextwin = twp->w_next;
	    如果 (twp != wp
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
	如果 (wp->w_wantline + height > Rows)
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
	为 (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    如果 (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	add_border_left_right_padding(wp);
    }

    如果 (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	如果 (callback.cb_name != NULL)
	{
	    set_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    为 (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    为 (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    如果 (type == TYPE_PREVIEW)
    {
	如果 (mouse_has(MOUSE_INSERT))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	为 (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_set_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#ifdef FEAT_QUICKFIX
    如果 (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	如果 (mouse_has(MOUSE_INSERT))
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

    如果 (d != NULL)
    {
	// Deal with options.
	如果 (apply_options(wp, d, TRUE) == FAIL)
	{
	    (void)popup_close(wp->w_id, FALSE);
	    返回 NULL;
	}
    }

#ifdef FEAT_TIMERS
    如果 (popup_is_notification(type) && wp->w_popup_timer == NULL)
	popup_add_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#endif

    popup_adjust_position(wp);

    wp->w_vsep_width = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_mask_refresh = TRUE;

#ifdef FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    如果 (buf->b_term != NULL)
	win_enter(wp, FALSE);
#endif

    返回 wp;
}

/*
 * popup_clear()
 */
    void
f_popup_clear(typval_T *argvars, typval_T *rettv UNUSED)
{
    整数 force = FALSE;

    如果 (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	返回;

    如果 (argvars[0].v_type != VAR_UNKNOWN)
	force = (整数)tv_get_bool(&argvars[0]);
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

    如果 (result != NULL && result->v_type != VAR_UNKNOWN)
	copy_tv(result, &argv[1]);
    否则
    {
	argv[1].v_type = VAR_NUMBER;
	argv[1].vval.v_number = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    如果 (result != NULL)
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
    如果 (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    否则
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    static void
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    整数 id;

    如果 (!win_valid(wp))
       返回;

    id = wp->w_id;

#ifdef FEAT_TERMINAL
    如果 (wp == curwin && curbuf->b_term != NULL)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    如果 (owp != curwin && owp->w_buffer->b_term != NULL)
		break;
	如果 (owp != NULL)
	    win_enter(owp, FALSE);
	否则
	{
	    FOR_ALL_POPUPWINS_IN_TAB(curtab, owp)
		如果 (owp != curwin && owp->w_buffer->b_term != NULL)
		    break;
	    如果 (owp != NULL)
		win_enter(owp, FALSE);
	    否则
		back_to_prevwin(wp);
	}
    }
#endif

    // Just in case a check higher up is missing.
    如果 (wp == curwin && ERROR_IF_POPUP_WINDOW)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	返回;
    }

    CHECK_CURBUF;
    如果 (wp->w_close_cb.cb_name != NULL)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    void
popup_close_with_retval(win_T *wp, 整数 retval)
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
    如果 (wp != mouse_wp
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
    整数	    row = mouse_row;
    整数	    col = mouse_col;

    // find the window where the mouse is in
    mouse_wp = mouse_find_win(&row, &col, FIND_POPUP);

    为 (wp = first_popupwin; wp != NULL; wp = nextwp)
    {
	nextwp = wp->w_next;
	check_mouse_moved(wp, mouse_wp);
    }
    为 (wp = curtab->tp_first_popupwin; wp != NULL; wp = nextwp)
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
filter_handle_drag(win_T *wp, 整数 c, typval_T *rettv)
{
    整数	row = mouse_row;
    整数	col = mouse_col;

    如果 ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
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
    整数		id;
    win_T	*wp;
    char_u	*key;
    typval_T	res;
    整数		c;
    linenr_T	old_lnum;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	返回;

    id = tv_get_number(&argvars[0]);
    wp = win_id2wp(id);
    key = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    如果 (wp == NULL)
	返回;

    c = *key;
    如果 (c == K_SPECIAL && key[1] != NUL)
	c = TO_SPECIAL(key[1], key[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_TRUE;
    res.v_type = VAR_NUMBER;

    old_lnum = wp->w_cursor.lnum;
    如果 (c == 'k' || c == 'K' || c == K_UP || c == Ctrl_P)
    {
	如果 (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	否则
	    wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_line_count;
    }
    如果 (c == 'j' || c == 'J' || c == K_DOWN || c == Ctrl_N)
    {
	如果 (wp->w_cursor.lnum < wp->w_buffer->b_ml.ml_line_count)
	    ++wp->w_cursor.lnum;
	否则
	    wp->w_cursor.lnum = 1;
    }
    如果 (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	返回;
    }

    如果 (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_number = -1;
	popup_close_and_callback(wp, &res);
	返回;
    }
    如果 (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_number = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	返回;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    void
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    整数		id;
    win_T	*wp;
    char_u	*key;
    typval_T	res;
    整数		c;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	返回;

    id = tv_get_number(&argvars[0]);
    wp = win_id2wp(id);
    key = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    如果 (wp == NULL)
	返回;

    c = *key;
    如果 (c == CAR && need_wait_return)
	返回;
    如果 (c == K_SPECIAL && key[1] != NUL)
	c = TO_SPECIAL(key[1], key[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_TRUE;

    如果 (c == 'y' || c == 'Y')
	res.vval.v_number = 1;
    否则 如果 (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_number = 0;
    否则
    {
	filter_handle_drag(wp, c, rettv);
	返回;
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
find_popup_win(整数 id)
{
    win_T *wp = win_id2wp(id);

    如果 (wp != NULL && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_window_nr_is_not_popup_window), id);
	返回 NULL;
    }
    返回 wp;
}

/*
 * popup_close({id})
 */
    void
f_popup_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    整数		id;
    win_T	*wp;

    如果 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	返回;

    id = (整数)tv_get_number(argvars);
    如果 (
#ifdef FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_term == NULL &&
#endif
	    ERROR_IF_ANY_POPUP_WINDOW)
	返回;

    wp = find_popup_win(id);
    如果 (wp != NULL)
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
    整数 r, c;
    整数 row_start, col_start, row_end, col_end;

    如果 (popup_mask == NULL || !popup_visible)
	返回;

    row_start = MAX(wp->w_winrow, 0);
    col_start = MAX(wp->w_wincol, 0);
    row_end = MIN(wp->w_winrow + popup_height(wp), (整数)screen_Rows);
    col_end = MIN(wp->w_wincol + popup_width(wp), (整数)screen_Columns);

    为 (r = row_start; r < row_end; ++r)
	为 (c = col_start; c < col_end; ++c)
	    popup_mask[r * screen_Columns + c] = 0;
}

    void
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    整数			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#ifdef FEAT_TERMINAL
    如果 (error_if_term_popup_window())
	返回;
#endif
    如果 ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	返回;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    如果 (wp->w_popup_imagep != NULL)
	image_placement_clear(wp->w_popup_imagep);
#endif

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    如果 (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    如果 (was_visible)
	popup_clear_mask_for(wp);

    如果 (old_area.active)
	popup_redraw_exposed_area(&old_area);
    否则
	redraw_all_later(UPD_NOT_VALID);

    status_redraw_all();
    如果 (!old_area.active)
	popup_mask_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    void
f_popup_hide(typval_T *argvars, typval_T *rettv UNUSED)
{
    整数		id;
    win_T	*wp;

    如果 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    void
popup_show(win_T *wp)
{
    bool popup_active;

    如果 ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	返回;

    popup_active = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    如果 (popup_active)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_lines_valid = 0;
	如果 (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    否则
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
    整数		id;
    win_T	*wp;

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = -1;

    如果 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#ifdef FEAT_QUICKFIX
    如果 (wp->w_popup_flags & POPF_INFO)
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
    整数		id;
    win_T	*wp;
    popup_area_T	old_area;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	返回;

    id = (整数)tv_get_number(&argvars[0]);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;

    popup_save_area(wp, &old_area);

    如果 (check_for_string_or_list_arg(argvars, 1) == FAIL)
	返回;

    popup_set_buffer_text(wp->w_buffer, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_lines_valid = 0;
    如果 (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    如果 (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    void
f_popup_setbuf(typval_T *argvars, typval_T *rettv UNUSED)
{
    整数			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    如果 (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_buffer_arg(argvars, 1) == FAIL)
	返回;

    id = (整数)tv_get_number(&argvars[0]);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;

    buf = tv_get_buf_from_arg(&argvars[1]);

    如果 (buf == NULL)
	返回;
#ifdef FEAT_TERMINAL
    如果 (buf->b_term != NULL && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_second_popup_with_terminal));
	返回;
    }
#endif

    如果 (wp->w_buffer != buf)
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

	如果 (popup_area_changed(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_number = VVAL_TRUE;
}

    static void
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    整数			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#ifdef FEAT_IMAGE_POPUP
    如果 (wp->w_popup_imagep != NULL)
	image_placement_free(wp->w_popup_imagep);
#endif

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_buffer->b_locked = FALSE;
    如果 (wp->w_winrow + popup_height(wp) >= cmdline_row)
	clear_cmdline = TRUE;

    如果 (was_visible)
	popup_clear_mask_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#ifdef HAS_MESSAGE_WINDOW
    如果 (wp == message_win)
	message_win = NULL;
#endif

    如果 (!old_area.active)
	redraw_all_later(UPD_NOT_VALID);
    status_redraw_all();
    如果 (!old_area.active)
	popup_mask_refresh = TRUE;
}

    static void
error_for_popup_window(void)
{
    emsg(_(e_not_allowed_in_popup_window));
}

    整数
error_if_popup_window(整数 also_with_term UNUSED)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    如果 (WIN_IS_POPUP(curwin)
#ifdef FEAT_TERMINAL
	    && (also_with_term || curbuf->b_term == NULL)
#endif
	    )
    {
	error_for_popup_window();
	返回 TRUE;
    }
    返回 FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    整数
popup_close(整数 id, 整数 force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = NULL;

    // go through global popups
    为 (wp = first_popupwin; wp != NULL; prev = wp, wp = wp->w_next)
	如果 (wp->w_id == id)
	{
	    如果 (wp == curwin)
	    {
		如果 (!force)
		{
		    error_for_popup_window();
		    返回 FAIL;
		}
		back_to_prevwin(wp);
	    }
	    如果 (prev == NULL)
		first_popupwin = wp->w_next;
	    否则
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    返回 OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	如果 (popup_close_tabpage(tp, id, force) == OK)
	    返回 OK;
    返回 FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    整数
popup_close_tabpage(tabpage_T *tp, 整数 id, 整数 force)
{
    win_T	*wp;
    win_T	**root = &tp->tp_first_popupwin;
    win_T	*prev = NULL;

    为 (wp = *root; wp != NULL; prev = wp, wp = wp->w_next)
	如果 (wp->w_id == id)
	{
	    如果 (wp == curwin)
	    {
		如果 (!force)
		{
		    error_for_popup_window();
		    返回 FAIL;
		}
		back_to_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    如果 (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    如果 (prev == NULL)
		*root = wp->w_next;
	    否则
		prev->w_next = wp->w_next;
#ifdef FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#endif
	    popup_free(wp);
	    返回 OK;
	}
    返回 FAIL;
}

    void
close_all_popups(整数 force)
{
    如果 (!force && ERROR_IF_ANY_POPUP_WINDOW)
	返回;
    当 (first_popupwin != NULL)
	如果 (popup_close(first_popupwin->w_id, force) == FAIL)
	    返回;
    当 (curtab->tp_first_popupwin != NULL)
	如果 (popup_close(curtab->tp_first_popupwin->w_id, force) == FAIL)
	    返回;
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
    整数 i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    为 (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    static bool
popup_style_changed(win_T *wp, popup_style_snapshot_T *style)
{
    整数 i;

    如果 (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	返回 true;
    为 (i = 0; i < 4; i++)
	如果 (style->border_highlight[i] != wp->w_border_highlight[i])
	    返回 true;
    返回 false;
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
    返回 layout->winrow != wp->w_winrow
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
    返回 area->winrow != wp->w_winrow
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
mark_overlapped_opacity_popup(win_T *wp, 整数 area_top, 整数 area_bot,
	整数 area_left, 整数 area_right, 整数 zindex)
{
    如果 ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_height(wp) <= area_top
	    || wp->w_wincol >= area_right
	    || wp->w_wincol + popup_width(wp) - wp->w_popup_leftoff
							      <= area_left)
	返回;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_lines_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    static void
redraw_overlapped_opacity_popups(整数 winrow, 整数 wincol, 整数 height, 整数 width,
	整数 leftoff, 整数 zindex)
{
    win_T	*wp;
    整数		area_top = winrow;
    整数		area_bot = winrow + height;
    整数		area_left = wincol;
    整数		area_right = wincol + width - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_top, area_bot, area_left,
		area_right, zindex);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_top, area_bot, area_left,
		area_right, zindex);

    如果 (must_redraw < UPD_VALID)
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
refill_pum_bg_row_from_buffer(整数 r, 整数 left, 整数 right)
{
    整数		line_cp = r;
    整数		col_cp = left;
    win_T	*wp;
    linenr_T	lnum;
    整数		row_for_lnum;
    整数		col_for_lnum = 0;
    char_u	*line;
    char_u	*p;
    整数		win_text_col;
    整数		screen_col;
    整数		soff_base = (r - pum_bg_top) * pum_bg_cols;
    整数		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    为 (c = left; c < right; ++c)
    {
	pum_bg_lines[soff_base + c] = ' ';
	如果 (pum_bg_attrs != NULL)
	    pum_bg_attrs[soff_base + c] = 0;
	如果 (enc_utf8 && pum_bg_linesUC != NULL)
	    pum_bg_linesUC[soff_base + c] = 0;
	如果 (enc_utf8)
	{
	    整数 k;
	    为 (k = 0; k < MAX_MCO; ++k)
		如果 (pum_bg_linesC[k] != NULL)
		    pum_bg_linesC[k][soff_base + c] = 0;
	}
    }

    wp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
    如果 (wp == NULL || line_cp < 0 || line_cp >= wp->w_height)
	返回;
    如果 (wp->w_buffer == NULL || wp->w_buffer->b_ml.ml_mfp == NULL)
	返回;

    // Compute the buffer line for this screen row.
    row_for_lnum = line_cp;
    如果 (mouse_comp_pos(wp, &row_for_lnum, &col_for_lnum, &lnum, NULL))
	返回;	// past end of buffer
    如果 (lnum < 1 || lnum > wp->w_buffer->b_ml.ml_line_count)
	返回;

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    如果 (line == NULL)
	返回;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_text_col = wp->w_wincol + win_col_off(wp);
    如果 (!wp->w_p_wrap)
	win_text_col -= wp->w_leftcol;
    screen_col = win_text_col;
    p = line;
    当 (*p != NUL && screen_col < right)
    {
	整数 char_cells;
	整数 byte_count;
	整数 soff = soff_base + screen_col;
	整数 in_range = (screen_col >= left && screen_col < right);

	如果 (*p == '\t')
	{
	    整数 ts = (整数)wp->w_buffer->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_col - win_text_col) % ts) : 1;
	    byte_count = 1;
	    如果 (in_range)
	    {
		pum_bg_lines[soff] = ' ';
		如果 (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = 0;
	    }
	}
	否则 如果 (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_count = mb_ptr2len(p);
	    如果 (in_range)
	    {
		pum_bg_lines[soff] = *p;
		如果 (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	否则
	{
	    char_cells = 1;
	    byte_count = 1;
	    如果 (in_range)
	    {
		pum_bg_lines[soff] = (*p < 0x20) ? ' ' : *p;
		如果 (enc_utf8 && pum_bg_linesUC != NULL)
		    pum_bg_linesUC[soff] = 0;
	    }
	}
	如果 (in_range && pum_bg_attrs != NULL)
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
    如果 (!area->active)
	返回;

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
    如果 (pum_bg_lines != NULL
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->height > pum_bg_top)
    {
	整数 top = MAX(area->winrow, pum_bg_top);
	整数 bot = MIN(area->winrow + area->height, pum_bg_bot);
	整数 left = MAX(area->wincol, 0);
	整数 right = MIN(area->wincol + area->width, pum_bg_cols);
	整数 r;

	为 (r = top; r < bot; ++r)
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
    如果 (screen_opacity_popup == NULL
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	返回;

    saved_screen->start_row = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->start_col = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->rows = wp->w_popup_padding[0] + wp->w_height
						    + wp->w_popup_padding[2];
    saved_screen->cols = wp->w_popup_padding[3] + wp->w_width
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    如果 (saved_screen->start_col > 0)
    {
	--saved_screen->start_col;
	++saved_screen->cols;
    }

    saved_screen->lines = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->rows * saved_screen->cols);
    saved_screen->attrs = ALLOC_CLEAR_MULT(整数,
				  saved_screen->rows * saved_screen->cols);
    如果 (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->rows * saved_screen->cols);

    如果 (saved_screen->lines == NULL || saved_screen->attrs == NULL)
	返回;

    为 (整数 r = 0; r < saved_screen->rows; r++)
    {
	整数 screen_row = saved_screen->start_row + r;

	如果 (screen_row >= 0 && screen_row < screen_Rows)
	    为 (整数 c = 0; c < saved_screen->cols; c++)
	    {
		整数 screen_col = saved_screen->start_col + c;

		如果 (screen_col >= 0 && screen_col < screen_Columns)
		{
		    整数 off = LineOffset[screen_row] + screen_col;
		    整数 save_off = r * saved_screen->cols + c;

		    saved_screen->lines[save_off] = ScreenLines[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    如果 (enc_utf8 && saved_screen->linesuc != NULL)
			saved_screen->linesuc[save_off] = ScreenLinesUC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    static void
redraw_under_popup_area(整数 winrow, 整数 wincol, 整数 height, 整数 width, 整数 leftoff)
{
    整数	    r;

    为 (r = winrow; r < winrow + height && r < screen_Rows; ++r)
    {
	整数	    c;
	win_T	    *prev_twp = NULL;

	如果 (r >= cmdline_row)
	{
	    clear_cmdline = TRUE;
	    continue;
	}

	为 (c = wincol; c < wincol + width - leftoff && c < screen_Columns; ++c)
	{
	    整数	    line_cp = r;
	    整数	    col_cp = c;
	    win_T   *twp;

	    twp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
	    如果 (twp != NULL && twp != prev_twp)
	    {
		prev_twp = twp;
		如果 (line_cp < twp->w_height)
		{
		    linenr_T lnum;

		    (void)mouse_comp_pos(twp, &line_cp, &col_cp, &lnum, NULL);
		    redrawWinline(twp, lnum);
		}
		否则 如果 (line_cp == twp->w_height)
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
    整数		id;
    win_T	*wp;
    popup_area_T	old_area;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;  // invalid {id}

    如果 (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	返回;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    如果 (wp->w_winrow + wp->w_height >= cmdline_row)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    如果 (popup_area_changed(wp, &old_area))
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
    整数		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    整数		need_redraw = FALSE;
    整数		need_reposition = FALSE;

    如果 (check_secure())
	返回;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;  // invalid {id}

    如果 (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	返回;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (void)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    如果 (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_buffer->b_ml.ml_line_count)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    如果 (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    否则 如果 (popup_style_changed(wp, &old_style))
	need_redraw = TRUE;

    如果 (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    如果 (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_mask_refresh = TRUE;
    }
    否则 如果 (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_lines_valid = 0;
	如果 (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    如果 (old_style.blend != wp->w_popup_blend)
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

    如果 (popup_area_changed(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    void
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    整数		id;
    win_T	*wp;
    整数		top_extra;
    整数		left_extra;

    如果 (rettv_dict_alloc(rettv) == FAIL)
	返回;

    如果 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;  // invalid {id}
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

    如果 (rettv_list_alloc(rettv) == FAIL)
	返回;
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
    整数		row;
    整数		col;
    win_T	*wp;

    如果 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	返回;

    row = tv_get_number(&argvars[0]) - 1;
    col = tv_get_number(&argvars[1]) - 1;
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    如果 (wp != NULL && WIN_IS_POPUP(wp))
	rettv->vval.v_number = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    static void
get_padding_border(dict_T *dict, 整数 *array, char *name)
{
    list_T  *list;
    整数	    i;

    如果 (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	返回;

    list = list_alloc();
    如果 (list == NULL)
	返回;

    如果 (dict_add_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	返回;
    }
    如果 (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	为 (i = 0; i < 4; ++i)
	    list_append_number(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    static void
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    整数	    i;

    为 (i = 0; i < 4; ++i)
	如果 (wp->w_border_highlight[i] != NULL)
	    break;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    如果 (i == 4 && !wp->w_border_highlight_isset)
	返回;

    list = list_alloc();
    如果 (list == NULL)
	返回;

    如果 (dict_add_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	返回;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    如果 (i == 4)
	返回;
    为 (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    static void
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    整数	    i;
    char_u  buf[NUMBUFLEN];
    整数	    len;

    为 (i = 0; i < 8; ++i)
	如果 (wp->w_border_char[i] != 0)
	    break;
    如果 (i == 8)
	返回;

    list = list_alloc();
    如果 (list == NULL)
	返回;

    如果 (dict_add_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	返回;
    }
    为 (i = 0; i < 8; ++i)
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
    如果 (list != NULL)
    {
	如果 (dict_add_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	否则
	{
	    list_append_number(list, wp->w_popup_lnum);
	    list_append_number(list, wp->w_popup_mincol);
	    list_append_number(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    如果 (list == NULL)
	返回;

    如果 (dict_add_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	返回;
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
    整数		id;
    win_T	*wp;
    tabpage_T	*tp;
    整数		i;

    如果 (rettv_dict_alloc(rettv) == FAIL)
	返回;

    如果 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	返回;

    id = (整数)tv_get_number(argvars);
    wp = find_popup_win(id);
    如果 (wp == NULL)
	返回;

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
    如果 (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = text_prop_type_by_id(
		wp->w_popup_prop_win->w_buffer,
		wp->w_popup_prop_type);

	如果 (pt != NULL)
	    dict_add_string(dict, "textprop", pt->pt_name);
	dict_add_number(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_add_number(dict, "textpropid", wp->w_popup_prop_id);
    }

#ifdef FEAT_IMAGE_POPUP
    如果 (wp->w_popup_imagep != NULL && wp->w_popup_imagep->img != NULL)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	如果 (dict_add_dict(dict, "image", img_dict) == FAIL)
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
    如果 (wp->w_scrollbar_highlight != NULL)
	dict_add_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    如果 (wp->w_thumb_highlight != NULL)
	dict_add_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_IN_TAB(tp, twp)
	    如果 (twp->w_id == id)
		break;
	如果 (twp != NULL)
	    break;
	++i;
    }
    如果 (tp == NULL)
	i = -1;  // must be global
    否则 如果 (tp == curtab)
	i = 0;
    dict_add_number(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    如果 (wp->w_filter_cb.cb_name != NULL)
	dict_add_callback(dict, "filter", &wp->w_filter_cb);
    如果 (wp->w_close_cb.cb_name != NULL)
	dict_add_callback(dict, "callback", &wp->w_close_cb);

    为 (i = 0; i < (整数)ARRAY_LENGTH(poppos_entries); ++i)
	如果 (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_add_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(整数)poppos_entries[i].pp_name.length);
	    break;
	}

    如果 (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_add_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    否则 如果 (wp->w_popup_close == POPCLOSE_CLICK)
	dict_add_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    否则
	dict_add_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#如果 defined(FEAT_TIMERS)
    dict_add_number(dict, "time", wp->w_popup_timer != NULL
	    ?  (long)wp->w_popup_timer->tr_interval : 0L);
#endif
}

#如果 defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    整数
error_if_term_popup_window(void)
{
    如果 (WIN_IS_POPUP(curwin) && curbuf->b_term != NULL
					   && term_job_running(curbuf->b_term))
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	返回 TRUE;
    }
    返回 FALSE;
}
#endif

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    void
popup_reset_handled(整数 handled_flag)
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
find_next_popup(整数 lowest, 整数 handled_flag)
{
    win_T   *wp;
    win_T   *found_wp;
    整数	    found_zindex;

    found_zindex = lowest ? INT_MAX : 0;
    found_wp = NULL;
    FOR_ALL_POPUPWINS(wp)
	如果 ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	如果 ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (lowest ? wp->w_zindex < found_zindex
		    : wp->w_zindex > found_zindex))
	{
	    found_zindex = wp->w_zindex;
	    found_wp = wp;
	}

    如果 (found_wp != NULL)
	found_wp->w_popup_handled |= handled_flag;
    返回 found_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    static 整数
invoke_popup_filter(win_T *wp, 整数 c)
{
    整数		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    整数		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    如果 (c == Ctrl_C)
    {
	整数 save_got_int = got_int;
	整数 was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	如果 (was_curwin && wp == curwin)
	    返回 -1;
	返回 TRUE;
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
    如果 (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    否则
    {
	如果 (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	如果 (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    如果 (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	否则
	{
	    如果 (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    返回 res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    整数
popup_do_filter(整数 c)
{
    static 整数	recursive = FALSE;
    整数		res = FALSE;
    win_T	*wp;
    整数		save_KeyTyped = KeyTyped;
    整数		state;
    整数		was_must_redraw = must_redraw;

#ifdef FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    如果 (popup_is_popup(curwin) && curbuf->b_term != NULL)
	返回 FALSE;
#endif

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    如果 (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	返回 FALSE;

    如果 (recursive)
	返回 FALSE;
    recursive = TRUE;

    如果 (c == K_LEFTMOUSE)
    {
	整数 row = mouse_row;
	整数 col = mouse_col;

	wp = mouse_find_win(&row, &col, FIND_POPUP);
	如果 (wp != NULL && popup_close_if_on_X(wp, row, col))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    当 (res == FALSE
		     && (wp = find_next_popup(FALSE, POPUP_HANDLED_2)) != NULL)
	如果 (wp->w_filter_cb.cb_name != NULL
		&& (wp->w_filter_mode & state) != 0)
	    res = invoke_popup_filter(wp, c);

    如果 (must_redraw > was_must_redraw)
    {
	整数 save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    KeyTyped = save_KeyTyped;

    // When interrupted return FALSE to avoid looping.
    返回 res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    整数
popup_no_mapping(void)
{
    整数	    round;
    win_T   *wp;

    为 (round = 1; round <= 2; ++round)
	为 (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
						   wp != NULL; wp = wp->w_next)
	    如果 (wp->w_filter_cb.cb_name != NULL
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		返回 TRUE;
    返回 FALSE;
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
    当 ((wp = find_next_popup(TRUE, POPUP_HANDLED_3)) != NULL)
	如果 (wp->w_popup_curwin != NULL
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
popup_update_mask(win_T *wp, 整数 width, 整数 height)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    整数		row, col;

    如果 (wp->w_popup_mask == NULL || width == 0 || height == 0)
    {
	VIM_CLEAR(wp->w_popup_mask_cells);
	返回;
    }
    如果 (wp->w_popup_mask_cells != NULL
	    && wp->w_popup_mask_height == height
	    && wp->w_popup_mask_width == width)
	返回;  // cache is still valid

    vim_free(wp->w_popup_mask_cells);
    如果 (width > 0 && (size_t)height > SIZE_MAX / (size_t)width)
    {
	wp->w_popup_mask_cells = NULL;
	返回;
    }
    wp->w_popup_mask_cells = alloc_clear((size_t)width * height);
    如果 (wp->w_popup_mask_cells == NULL)
	返回;
    cells = wp->w_popup_mask_cells;

    FOR_ALL_LIST_ITEMS(wp->w_popup_mask, lio)
    {
	整数 cols, cole;
	整数 lines, linee;

	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_number(&li->li_tv);
	如果 (cols < 0)
	    cols = width + cols + 1;
	如果 (cols <= 0)
	    cols = 1;
	li = li->li_next;
	cole = tv_get_number(&li->li_tv);
	如果 (cole < 0)
	    cole = width + cole + 1;
	如果 (cole > width)
	    cole = width;
	li = li->li_next;
	lines = tv_get_number(&li->li_tv);
	如果 (lines < 0)
	    lines = height + lines + 1;
	如果 (lines <= 0)
	    lines = 1;
	li = li->li_next;
	linee = tv_get_number(&li->li_tv);
	如果 (linee < 0)
	    linee = height + linee + 1;
	如果 (linee > height)
	    linee = height;

	为 (row = lines - 1; row < linee; ++row)
	    为 (col = cols - 1; col < cole; ++col)
		cells[row * width + col] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    static 整数
popup_masked(win_T *wp, 整数 width, 整数 height, 整数 screencol, 整数 screenline)
{
    整数 col = screencol - wp->w_wincol + wp->w_popup_leftoff;
    整数 line = screenline - wp->w_winrow;

    返回 col >= 0 && col < width
	    && line >= 0 && line < height
	    && wp->w_popup_mask_cells[line * width + col];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    static void
update_popup_transparent(win_T *wp, 整数 val)
{
    如果 (wp->w_popup_mask == NULL)
	返回;

    整数		width = popup_width(wp);
    整数		height = popup_height(wp);
    listitem_T	*lio, *li;
    整数		cols, cole;
    整数		lines, linee;
    整数		col, line;

    FOR_ALL_LIST_ITEMS(wp->w_popup_mask, lio)
    {
	li = lio->li_tv.vval.v_list->lv_first;
	cols = tv_get_number(&li->li_tv);
	如果 (cols < 0)
	    cols = width + cols + 1;
	li = li->li_next;
	cole = tv_get_number(&li->li_tv);
	如果 (cole < 0)
	    cole = width + cole + 1;
	li = li->li_next;
	lines = tv_get_number(&li->li_tv);
	如果 (lines < 0)
	    lines = height + lines + 1;
	li = li->li_next;
	linee = tv_get_number(&li->li_tv);
	如果 (linee < 0)
	    linee = height + linee + 1;

	--cols;
	cols -= wp->w_popup_leftoff;
	如果 (cols < 0)
	    cols = 0;
	cole -= wp->w_popup_leftoff;
	--lines;
	如果 (lines < 0)
	    lines = 0;
	为 (line = lines; line < linee
		&& line + wp->w_winrow < screen_Rows; ++line)
	    为 (col = cols; col < cole
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
    static 整数
check_popup_unhidden(win_T *wp)
{
    如果 (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    found = false;

	如果 ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    返回 FALSE;
	如果 (find_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    found = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	否则 如果 (popup_find_prop_above_top(wp, wp->w_popup_prop_win,
					    popup_height(wp), &prop, &lnum))
	    found = true;
	如果 (found)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    返回 TRUE;
	}
    }
    返回 FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    static 整数
popup_need_position_adjust(win_T *wp)
{
    如果 (wp->w_popup_last_changedtick != CHANGEDTICK(wp->w_buffer))
	返回 TRUE;
    如果 (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_buffer)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_width != wp->w_popup_prop_win->w_width
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_height))
	返回 TRUE;

    // May need to adjust the width if the cursor moved.
    返回 wp->w_cursor.lnum != wp->w_popup_last_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
static short *opacity_zindex = NULL;
static 整数    opacity_zindex_rows = 0;
static 整数    opacity_zindex_cols = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    static void
popup_mark_opacity_zindex(win_T *wp)
{
    整数	    width;
    整数	    height;
    整数	    r, c;

    如果 (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	返回;

    width = popup_width(wp);
    height = popup_height(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    为 (r = wp->w_winrow;
		       r < wp->w_winrow + height - wp->w_popup_topoff
						&& r < screen_Rows; ++r)
	为 (c = wp->w_wincol;
		 c < wp->w_wincol + width - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    整数 off = r * screen_Columns + c;
	    如果 (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    static void
redraw_win_under_opacity_popup(win_T *wp)
{
    整数	    height;
    整数	    width;
    整数	    r;

    如果 (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	返回;

    height = popup_height(wp);
    width = popup_width(wp);
    为 (r = wp->w_winrow;
		       r < wp->w_winrow + height && r < screen_Rows; ++r)
    {
	整数	    col;
	win_T	    *prev_twp = NULL;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	为 (col = wp->w_wincol;
		col < wp->w_wincol + width && col < screen_Columns; ++col)
	{
	    整数	    line_cp = r;
	    整数	    col_cp = col;
	    win_T   *twp;

	    twp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
	    如果 (twp != NULL && twp != prev_twp)
	    {
		prev_twp = twp;
		如果 (line_cp < twp->w_height)
		{
		    linenr_T lnum;

#ifdef FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    如果 (term_do_update_window(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    否则
#endif
		    {
			(void)mouse_comp_pos(twp, &line_cp, &col_cp, &lnum,
									NULL);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		否则 如果 (line_cp == twp->w_height)
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
    整数
popup_is_under_opacity(整数 row, 整数 col)
{
    如果 (opacity_zindex == NULL
	    || row < 0 || row >= opacity_zindex_rows
	    || col < 0 || col >= opacity_zindex_cols)
	返回 FALSE;
    返回 opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    整数
popup_is_over_opacity(整数 row, 整数 col)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	如果 ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    返回 TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	如果 ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& row >= wp->w_winrow
		&& row < wp->w_winrow + popup_height(wp)
		&& col >= wp->w_wincol
		&& col < wp->w_wincol + popup_width(wp))
	    返回 TRUE;
    返回 FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    整数
popup_is_under_opacity_range(整数 row, 整数 start_col, 整数 end_col)
{
    整数 col;

    如果 (opacity_zindex == NULL
	    || row < 0 || row >= opacity_zindex_rows)
	返回 FALSE;
    为 (col = start_col; col < end_col && col < opacity_zindex_cols; ++col)
	如果 (opacity_zindex[row * opacity_zindex_cols + col] > screen_zindex)
	    返回 TRUE;
    返回 FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    void
may_update_popup_mask(整数 type)
{
    win_T	*wp;
    short	*mask;
    整数		line, col;
    整数		redraw_all_popups = FALSE;
    整数		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    如果 (popup_mask_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_mask_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	如果 (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	否则 如果 (popup_need_position_adjust(wp))
	    popup_mask_refresh = TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	如果 (wp->w_popup_flags & POPF_HIDDEN)
	    popup_mask_refresh |= check_popup_unhidden(wp);
	否则 如果 (popup_need_position_adjust(wp))
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
	整数 has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    如果 ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    如果 ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	如果 (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_rows = 0;
	    opacity_zindex_cols = 0;
	}
	否则
	{
	    如果 (opacity_zindex_rows != screen_Rows
		    || opacity_zindex_cols != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(short,
					screen_Rows * screen_Columns);
		opacity_zindex_rows = screen_Rows;
		opacity_zindex_cols = screen_Columns;
	    }
	    如果 (opacity_zindex != NULL)
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

    如果 (!popup_mask_refresh)
	返回;

    // Need to update the mask, something has changed.
    popup_mask_refresh = FALSE;
    popup_mask_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	如果 (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    如果 (redrawing_all_win)
	mask = popup_mask;
    否则
	mask = popup_mask_next;
    vim_memset(mask, 0, (size_t)screen_Rows * screen_Columns * sizeof(short));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    当 ((wp = find_next_popup(TRUE, POPUP_HANDLED_4)) != NULL)
    {
	整数 width;
	整数 height;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	如果 (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    如果 (wp->w_popup_flags & POPF_HIDDEN)
		continue;
	}

	width = popup_width(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	如果 (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    height = cl.eff_height;
	}
	否则
	    height = popup_height(wp);
	popup_update_mask(wp, width, height);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	如果 ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    continue;

	{
	    整数 mask_start = wp->w_winrow;
	    整数 mask_end = mask_start + height;
	    整数 mask_col_start = wp->w_wincol + wp->w_popup_leftclip;
	    整数 mask_col_end = wp->w_wincol + width - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    为 (line = mask_start;
			    line < mask_end && line < screen_Rows; ++line)
		为 (col = mask_col_start;
		     col < mask_col_end && col < screen_Columns; ++col)
		    如果 (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(line, col, FALSE))
			mask[line * screen_Columns + col] = POPUPMENU_ZINDEX;
		    否则 如果 (wp->w_popup_mask_cells == NULL
				    || !popup_masked(wp, width, height, col, line))
			mask[line * screen_Columns + col] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    如果 (mask == popup_mask_next)
    {
	整数	    *plines_cache = ALLOC_CLEAR_MULT(整数, Rows);
	win_T	    *prev_wp = NULL;

	为 (line = 0; line < screen_Rows; ++line)
	{
	    整数	    col_done = 0;

	    为 (col = 0; col < screen_Columns; ++col)
	    {
		整数 off = line * screen_Columns + col;

		如果 (popup_mask[off] != popup_mask_next[off])
		{
		    popup_mask[off] = popup_mask_next[off];

		    如果 (line >= cmdline_row)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			如果 (!msg_scrolled && popup_mask_next[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    否则 如果 (col >= col_done)
		    {
			linenr_T	lnum;
			整数		line_cp = line;
			整数		col_cp = col;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_find_win(&line_cp, &col_cp, IGNORE_POPUP);
			如果 (wp != NULL)
			{
#如果 defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    如果 (bt_terminal(wp->w_buffer))
				redraw_win_later(wp, UPD_NOT_VALID);
			    否则
#endif
			    {
				如果 (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   sizeof(整数) * Rows);
				    prev_wp = wp;
				}

				如果 (line_cp >= wp->w_height)
				    // In (or below) status line
				    wp->w_redr_status = true;
				否则
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
    如果 (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#ifdef FEAT_PROP_POPUP
static schar_T *base_screenlines = NULL;
static 整数 *base_screenattrs = NULL;
static u8char_T *base_screenlinesuc = NULL;
static 整数 base_screen_rows = 0;
static 整数 base_screen_cols = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    整数
popup_get_base_screen_cell(整数 row, 整数 col, schar_T *linep, 整数 *attrp,
							  u8char_T *ucp)
{
    如果 (base_screenlines == NULL || base_screenattrs == NULL)
	返回 FALSE;
    如果 (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	返回 FALSE;

    整数 off = row * base_screen_cols + col;
    如果 (linep != NULL)
	*linep = base_screenlines[off];
    如果 (attrp != NULL)
	*attrp = base_screenattrs[off];
    如果 (ucp != NULL)
    {
	如果 (enc_utf8 && base_screenlinesuc != NULL)
	    *ucp = base_screenlinesuc[off];
	否则
	    *ucp = 0;
    }
    返回 TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    void
popup_set_base_screen_cell(整数 row, 整数 col, schar_T line, 整数 attr, u8char_T uc)
{
    如果 (base_screenlines == NULL || base_screenattrs == NULL)
	返回;
    如果 (row < 0 || col < 0 || row >= base_screen_rows
						     || col >= base_screen_cols)
	返回;

    整数 off = row * base_screen_cols + col;
    base_screenlines[off] = line;
    base_screenattrs[off] = attr;
    如果 (enc_utf8 && base_screenlinesuc != NULL)
	base_screenlinesuc[off] = uc;
}
#endif

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
static void
draw_opacity_padding_cell(
	整数		row,
	整数		col,
	popup_saved_screen_T *saved_screen,
	整数		pad_start_col,
	整数		pad_end_col)
{
    如果 (LineOffset == NULL || ScreenLines == NULL || ScreenAttrs == NULL
	    || (enc_utf8 && ScreenLinesUC == NULL))
	返回;

    整数 off = LineOffset[row] + col;
    整数 r = row - saved_screen->start_row;
    整数 c = col - saved_screen->start_col;

    如果 (r >= 0 && r < saved_screen->rows && c >= 0 && c < saved_screen->cols)
    {
	整数 save_off = r * saved_screen->cols + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	如果 (enc_utf8 && saved_screen->linesuc != NULL)
	{
	    整数 base_col = col - 1;
	    整数 base_off = off - 1;
	    整数 base_save_off = save_off - 1;
	    整数 wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    如果 (base_off >= 0)
	    {
		如果 (ScreenLinesUC[base_off] != 0
			&& utf_char2cells(ScreenLinesUC[base_off]) == 2
			&& ScreenLines[off] == 0)
		    wide_prev = TRUE;
	    }
	    如果 (!wide_prev && save_off > 0)
	    {
		如果 (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->lines[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    如果 (wide_prev && base_col >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		如果 (base_col < pad_start_col)
		{
		    如果 (ScreenLinesUC[base_off] != 0
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
			如果 (enc_utf8)
			    ScreenLinesUC[off] = 0;
			整数 popup_attr_val =
					get_win_attr(screen_opacity_popup);
			整数 blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_set_base_screen_cell(row, col,
				ScreenLines[off], ScreenAttrs[off],
				ScreenLinesUC[off]);
			screen_char(off, row, col);
			返回;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    如果 (base_save_off >= 0
			    && saved_screen->linesuc[base_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[base_save_off]) == 2
			    && ScreenLines[base_off] == ' '
			    && ScreenLinesUC[base_off] == 0)
		    {
			整数 popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			整数 blend =
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
			返回;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    ScreenLines[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[base_save_off];
		    如果 (enc_utf8)
			ScreenLinesUC[off] = 0;
		    整数 popup_attr_val = get_win_attr(screen_opacity_popup);
		    整数 blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_set_base_screen_cell(row, col, ScreenLines[off],
					       ScreenAttrs[off], ScreenLinesUC[off]);
		    screen_char(off, row, col);
		    返回;
		}

		// Base cell is inside the saved area, redraw the wide char.
		如果 (save_off > 0)
		{
		    ScreenLines[base_off] = saved_screen->lines[base_save_off];
		    ScreenAttrs[base_off] = saved_screen->attrs[base_save_off];
		    ScreenLines[off] = saved_screen->lines[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    ScreenLinesUC[base_off] = saved_screen->linesuc[base_save_off];
		    ScreenLinesUC[off] = saved_screen->linesuc[save_off];

		    整数 popup_attr_val = get_win_attr(screen_opacity_popup);
		    整数 blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[base_off] = hl_blend_attr(ScreenAttrs[base_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[base_off];
		    popup_set_base_screen_cell(row, base_col, ScreenLines[base_off],
					       ScreenAttrs[base_off], ScreenLinesUC[base_off]);
		    popup_set_base_screen_cell(row, col, ScreenLines[off],
					       ScreenAttrs[off], ScreenLinesUC[off]);
		    screen_char(base_off, row, base_col);
		}
		返回;
	    }
	}
	ScreenLines[off] = saved_screen->lines[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	如果 (enc_utf8)
	    ScreenLinesUC[off] = saved_screen->linesuc != NULL
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	如果 (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 >= pad_end_col)
	{
	    ScreenLines[off] = ' ';
	    ScreenLinesUC[off] = 0;
	}

	整数 popup_attr_val = get_win_attr(screen_opacity_popup);
	整数 blend = screen_opacity_popup->w_popup_blend;
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
	整数		start_row,
	整数		end_row,
	整数		start_col,
	整数		end_col,
	popup_saved_screen_T *saved_screen)
{
    为 (整数 pad_row = start_row; pad_row < end_row; pad_row++)
	为 (整数 pad_col = start_col; pad_col < end_col; pad_col++)
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
    整数	    top_off;
    整数	    left_extra;
    整数	    total_width;
    整数	    total_height;
    整数	    top_padding;
    整数	    popup_attr;
    整数	    border_attr[4];
    整数	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    整数	    row;
    整数	    wincol;
    整数	    padcol = 0;
    整数	    padendcol = 0;
    整数	    i;
    整数	    sb_thumb_top = 0;
    整数	    sb_thumb_height = 0;
    整数	    attr_scroll = 0;
    整数	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#ifdef FEAT_PROP_POPUP
    如果 (base_screenlines != NULL)
    {
	vim_free(base_screenlines);
	base_screenlines = NULL;
    }
    如果 (base_screenattrs != NULL)
    {
	vim_free(base_screenattrs);
	base_screenattrs = NULL;
    }
    如果 (base_screenlinesuc != NULL)
    {
	vim_free(base_screenlinesuc);
	base_screenlinesuc = NULL;
    }
    base_screen_rows = 0;
    base_screen_cols = 0;
#endif

    当 ((wp = find_next_popup(TRUE, POPUP_HANDLED_5)) != NULL)
    {
	整数	    title_len = 0;
	整数	    title_wincol;
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
	如果 ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	否则
	    screen_opacity_popup = NULL;

#ifdef FEAT_PROP_POPUP
	如果 (screen_opacity_popup != NULL)
	{
	    如果 (base_screenlines != NULL)
	    {
		vim_free(base_screenlines);
		base_screenlines = NULL;
	    }
	    如果 (base_screenattrs != NULL)
	    {
		vim_free(base_screenattrs);
		base_screenattrs = NULL;
	    }
	    如果 (base_screenlinesuc != NULL)
	    {
		vim_free(base_screenlinesuc);
		base_screenlinesuc = NULL;
	    }

	    base_screen_rows = screen_Rows;
	    base_screen_cols = screen_Columns;
	    base_screenlines = ALLOC_MULT(schar_T,
				    base_screen_rows * base_screen_cols);
	    base_screenattrs = ALLOC_MULT(整数,
				    base_screen_rows * base_screen_cols);
	    如果 (enc_utf8)
		base_screenlinesuc = ALLOC_MULT(u8char_T,
				    base_screen_rows * base_screen_cols);

	    如果 (base_screenlines != NULL && base_screenattrs != NULL)
	    {
		为 (整数 r = 0; r < base_screen_rows; r++)
		{
		    整数 off = LineOffset[r];
		    整数 base_off = r * base_screen_cols;
		    为 (整数 c = 0; c < base_screen_cols; c++)
		    {
			base_screenlines[base_off + c] = ScreenLines[off + c];
			base_screenattrs[base_off + c] = ScreenAttrs[off + c];
			如果 (enc_utf8 && base_screenlinesuc != NULL)
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
	如果 (wp->w_wincol + left_extra < 0)
	    left_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(top_off - wp->w_popup_topoff, 0);
	wp->w_wincol += left_extra;

	// Draw the popup text, unless it's off screen.
	如果 (wp->w_winrow < screen_Rows && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    如果 (wp->w_popup_last_curline != wp->w_cursor.lnum)
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
	    如果 (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    否则 如果 (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(top_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= left_extra;

	// Add offset for border and padding if not done already.
	如果 ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += left_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	如果 ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
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
	如果 (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    total_width = cl.eff_width;
	否则
	    total_width = popup_width(wp) - wp->w_popup_rightoff;
	如果 (total_width < 0)
	    total_width = 0;
	如果 (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    total_height = cl.eff_height;
	否则
	    total_height = popup_height(wp);
	popup_attr = get_win_attr(wp);

	如果 (wp->w_winrow + total_height > cmdline_row)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	否则
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	如果 (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	否则
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    为 (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    如果 (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	为 (i = 0; i < 8; ++i)
	    如果 (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	为 (i = 0; i < 4; ++i)
	{
	    如果 (wp->w_border_highlight[i] != NULL)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    否则 如果 (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    否则
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    如果 ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	如果 ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	如果 (wp->w_popup_title != NULL)
	{
	    整数	    title_attr;

	    如果 (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != NULL)
		title_attr = border_attr[0];
	    否则 如果 (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    否则
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    如果 (title_len > total_width - 2)
	    {
		整数	title_byte_len = (整数)STRLEN(wp->w_popup_title);
		char_u  *title_text = alloc(title_byte_len + 1);

		如果 (title_text != NULL)
		{
		    trunc_string(wp->w_popup_title, title_text,
					  total_width - 2, title_byte_len + 1);
		    screen_puts(title_text, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_text);
		}

		title_len = total_width - 2;
	    }
	    否则
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	top_padding = cl.eff_padding[0];
	如果 (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    如果 (title_len > 0)
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
	    否则
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + total_width,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    如果 (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + total_width - 1, border_attr[1]);
	    }
	}
	否则 如果 (cl.eff_padding[0] == 0 && popup_top_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    top_padding = 1;

	如果 (top_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + total_width - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    如果 (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	如果 (top_padding > 0)
	{
	    row = wp->w_winrow + cl.eff_border[0];
	    如果 (title_len > 0 && row == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		如果 (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		{
		    // Left of title
		    fill_opacity_padding(row, row + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(row, row + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		否则
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
	    如果 (screen_opacity_popup != NULL && saved_screen.lines != NULL)
	    {
		fill_opacity_padding(row, row + top_padding, padcol, padendcol,
			&saved_screen);
	    }
	    否则
	    {
		screen_fill(row, row + top_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	如果 (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_buffer->b_ml.ml_line_count;
	    整数		height = wp->w_height;
	    整数		last;

	    sb_thumb_height = ((linenr_T)height * height + linecount / 2)
								   / linecount;
	    如果 (wp->w_topline > 1 && sb_thumb_height == height)
		--sb_thumb_height;  // scrolled, no full thumb
	    如果 (sb_thumb_height == 0)
		sb_thumb_height = 1;
	    如果 (linecount <= wp->w_height || wp->w_height == 0)
		// it just fits, avoid divide by zero
		sb_thumb_top = 0;
	    否则
		sb_thumb_top = (wp->w_topline - 1
				+ (linecount / wp->w_height) / 2)
				* (wp->w_height - sb_thumb_height)
						  / (linecount - wp->w_height);
	    如果 (wp->w_topline > 1 && sb_thumb_top == 0 && height > 1)
		sb_thumb_top = 1;  // show it's scrolled
	    last = total_height - top_off - wp->w_popup_border[2];
	    如果 (sb_thumb_top >= last)
		// show at least one character
		sb_thumb_top = last - 1;

	    如果 (wp->w_scrollbar_highlight != NULL)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    否则
		attr_scroll = highlight_attr[HLF_PSB];
	    如果 (wp->w_thumb_highlight != NULL)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    否则
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	为 (i = cl.eff_border[0]; i < total_height - cl.eff_border[2]; ++i)
	{
	    整数	pad_left;
	    // left and right padding only needed next to the body
	    整数 do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < total_height - cl.eff_border[2] - cl.eff_padding[2];

	    row = wp->w_winrow + i;

	    // left border
	    如果 (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, row, wincol, border_attr[3]);
	    }
	    如果 (do_padding && cl.eff_padding[3] > 0)
	    {
		整数 col = wincol + wp->w_popup_border[3];

		// left padding
		pad_left = wp->w_popup_padding[3];
		如果 (col < 0)
		{
		    pad_left += col;
		    col = 0;
		}
		如果 (pad_left > 0)
		{
		    如果 (screen_opacity_popup != NULL && saved_screen.lines != NULL)
			fill_opacity_padding(row, row + 1, col, col + pad_left,
				&saved_screen);
		    否则
			screen_fill(row, row + 1, col, col + pad_left,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    如果 (wp->w_has_scrollbar)
	    {
		整数 line = i - top_off;
		整数 scroll_col = wp->w_wincol + total_width - 1
						       - wp->w_popup_border[1];

		如果 (line >= 0 && line < wp->w_height)
		    screen_putchar(' ', row, scroll_col,
			    line >= sb_thumb_top
				       && line < sb_thumb_top + sb_thumb_height
						  ? attr_thumb : attr_scroll);
		否则
		    screen_putchar(' ', row, scroll_col, popup_attr);
	    }
	    // right border
	    如果 (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[1]);
	    }
	    // right padding
	    如果 (do_padding && cl.eff_padding[1] > 0)
	    {
		整数 pad_col_start = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_width;
		整数 pad_col_end = pad_col_start + wp->w_popup_padding[1];

		如果 (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		    fill_opacity_padding(row, row + 1, pad_col_start, pad_col_end,
			    &saved_screen);
		否则
		    screen_fill(row, row + 1, pad_col_start, pad_col_end,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	如果 (wp->w_popup_shadow)
	{
	    整数 col = wincol + total_width;
	    为 (i = 0; i < total_height; ++i)
	    {
		row = wp->w_winrow + i + 1;
		put_shadow_char(row, col);
		put_shadow_char(row, col + 1);
	    }
	}

	如果 (cl.eff_padding[2] > 0)
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
	    如果 (screen_opacity_popup != NULL && saved_screen.lines != NULL)
		fill_opacity_padding(row, row + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    否则
		screen_fill(row, row + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	如果 (cl.eff_border[2] > 0)
	{
	    // bottom border
	    row = wp->w_winrow + total_height - 1;
	    screen_fill(row, row + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + total_width,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    如果 (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, row, wincol + total_width - 1, border_attr[2]);
	    }
	}

	如果 (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    row = wp->w_winrow + total_height;
	    为 (整数 col = 2 + (wincol < 0 ? 0 : wincol);
		    col < wincol + total_width; col++)
		put_shadow_char(row, col);
	}

	如果 (wp->w_popup_close == POPCLOSE_BUTTON)
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

	如果 (override_success)
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

#如果 defined(FEAT_SEARCH_EXTRA)
    // In case win_update() called start_search_hl().
    end_search_hl();
#endif
}

/*
 * Mark references in callbacks of one popup window.
 */
    static 整数
set_ref_in_one_popup(win_T *wp, 整数 copyID)
{
    整数		abort = FALSE;
    typval_T	tv;

    如果 (wp->w_close_cb.cb_partial != NULL)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    如果 (wp->w_filter_cb.cb_partial != NULL)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    abort = abort || set_ref_in_list(wp->w_popup_mask, copyID);
    返回 abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    整数
set_ref_in_popups(整数 copyID)
{
    整数		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    为 (wp = first_popupwin; !abort && wp != NULL; wp = wp->w_next)
	abort = abort || set_ref_in_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	为 (wp = tp->tp_first_popupwin; !abort && wp != NULL; wp = wp->w_next)
	    abort = abort || set_ref_in_one_popup(wp, copyID);
	如果 (abort)
	    break;
    }
    返回 abort;
}

    整数
popup_is_popup(win_T *wp)
{
    返回 wp->w_popup_flags != 0;
}

#如果 defined(FEAT_QUICKFIX)
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
	如果 (wp->w_p_pvw)
	    返回 wp;
    返回 NULL;
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
	如果 (wp->w_popup_flags & POPF_INFO)
	    返回 wp;
    返回 NULL;
}
#endif

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(整数 dir, bool by_page)
{
#ifdef FEAT_QUICKFIX
    win_T	*wp = popup_find_info_window();
    整数		by;
    linenr_T	new_topline;

    如果 (wp == NULL)
	返回 false;

    by = by_page ? (wp->w_height > 2 ? wp->w_height - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    如果 (new_topline < 1)
	new_topline = 1;
    如果 (new_topline > wp->w_buffer->b_ml.ml_line_count)
	new_topline = wp->w_buffer->b_ml.ml_line_count;
    如果 (new_topline != wp->w_topline)
    {
	set_topline(wp, new_topline);
	popup_set_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    返回 true;
#否则
    返回 false;
#endif
}

    void
f_popup_findecho(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef HAS_MESSAGE_WINDOW
    rettv->vval.v_number = message_win == NULL ? 0 : message_win->w_id;
#否则
    rettv->vval.v_number = 0;
#endif
}

    void
f_popup_findinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_info_window();

    rettv->vval.v_number = wp == NULL ? 0 : wp->w_id;
#否则
    rettv->vval.v_number = 0;
#endif
}

    void
f_popup_findpreview(typval_T *argvars UNUSED, typval_T *rettv)
{
#ifdef FEAT_QUICKFIX
    win_T   *wp = popup_find_preview_window();

    rettv->vval.v_number = wp == NULL ? 0 : wp->w_id;
#否则
    rettv->vval.v_number = 0;
#endif
}

#如果 defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    整数
popup_create_preview_window(整数 info)
{
    win_T *wp = popup_create(NULL, NULL, info ? TYPE_INFO : TYPE_PREVIEW);

    如果 (wp == NULL)
	返回 FAIL;
    如果 (info)
	wp->w_popup_flags |= POPF_INFO;
    否则
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    如果 (wp->w_minwidth > 0)
	wp->w_width = wp->w_minwidth;
    否则 如果 (wp->w_maxwidth > 0)
	wp->w_width = wp->w_maxwidth;
    否则
	wp->w_width = curwin->w_width;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_buffer->b_locked = FALSE;

    win_enter(wp, FALSE);
    返回 OK;
}

/*
 * Close any preview popup.
 */
    void
popup_close_preview(void)
{
    win_T *wp = popup_find_preview_window();

    如果 (wp != NULL)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    void
popup_hide_info(void)
{
    win_T *wp = popup_find_info_window();

    如果 (wp != NULL)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	如果 (State & MODE_CMDLINE)
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

    如果 (wp != NULL)
	popup_close_with_retval(wp, -1);
}
#endif

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    整数
popup_overlaps_cmdline(void)
{
    返回 popup_on_cmdline;
}

#如果 defined(HAS_MESSAGE_WINDOW)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_message_win(void)
{
    如果 (message_win != NULL)
	返回 message_win;

    整数 i;

    message_win = popup_create(NULL, NULL, TYPE_MESSAGE_WIN);

    如果 (message_win == NULL)
	返回 NULL;

    // use the full screen width
    message_win->w_width = Columns;

    // position at bottom of screen
    message_win->w_popup_pos = POPPOS_BOTTOM;
    message_win->w_wantcol = 1;
    message_win->w_minwidth = 9999;
    message_win->w_firstline = -1;

    // no padding, border at the top
    为 (i = 0; i < 4; ++i)
	message_win->w_popup_padding[i] = 0;
    为 (i = 1; i < 4; ++i)
	message_win->w_popup_border[i] = 0;

    如果 (message_win->w_popup_timer != NULL)
	message_win->w_popup_timer->tr_keep = TRUE;
    返回 message_win;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    void
popup_show_message_win(void)
{
    如果 (message_win == NULL)
	返回;

    如果 ((message_win->w_popup_flags & POPF_HIDDEN) != 0)
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
    如果 (wp == message_win && start_message_win_timer)
    {
	如果 (message_win->w_popup_timer != NULL)
	{
	    message_win->w_popup_timer->tr_interval = message_win_time;
	    timer_start(message_win->w_popup_timer);
	    message_win_time = 3000;
	}
	start_message_win_timer = FALSE;
    }
}

    整数
popup_message_win_visible(void)
{
    返回 message_win != NULL
	&& (message_win->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    void
popup_hide_message_win(void)
{
    如果 (message_win != NULL)
	popup_hide(message_win);
}

// Values saved in start_echowindow() and restored in end_echowindow()
static 整数 save_msg_didout = FALSE;
static 整数 save_msg_col = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
static 整数 ew_msg_didout = FALSE;
static 整数 ew_msg_col = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    void
start_echowindow(整数 time_sec)
{
    in_echowindow = TRUE;
    save_msg_didout = msg_didout;
    save_msg_col = msg_col;
    msg_didout = ew_msg_didout;
    msg_col = ew_msg_col;
    如果 (time_sec != 0)
	message_win_time = time_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    void
end_echowindow(void)
{
    in_echowindow = FALSE;

    如果 ((State & MODE_HITRETURN) == 0)
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
    整数
popup_win_closed(win_T *win)
{
    整数	    round;
    win_T   *wp;
    win_T   *next;
    整数	    ret = FALSE;

    为 (round = 1; round <= 2; ++round)
	为 (wp = round == 1 ? first_popupwin : curtab->tp_first_popupwin;
							 wp != NULL; wp = next)
	{
	    next = wp->w_next;
	    如果 (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    返回 ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    void
popup_set_title(win_T *wp)
{
    如果 (wp->w_buffer->b_fname == NULL)
	返回;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_buffer, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_buffer->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    如果 (wp->w_popup_title != NULL)
	vim_snprintf((char *)wp->w_popup_title, len, " %s ",
		wp->w_buffer->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#如果 defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    void
popup_update_preview_title(void)
{
    win_T *wp = popup_find_preview_window();

    如果 (wp != NULL)
	popup_set_title(wp);
}
#endif

#endif // FEAT_PROP_POPUP
