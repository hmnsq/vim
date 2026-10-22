/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * popupmenu.c: Popup menu (PUM)
 */
#include "vim.h"

static pumitem_T *pum_array = NULL;	// items of displayed pum
static số_nguyên pum_size;			// nr of items in "pum_array"
static số_nguyên pum_selected;		// index of selected item or -1
static số_nguyên pum_first = 0;		// index of top item

static số_nguyên call_update_screen = FALSE;
static số_nguyên pum_in_cmdline = FALSE;

static số_nguyên pum_height;			// nr of displayed pum items
static số_nguyên pum_width;			// width of displayed pum items
static số_nguyên pum_base_width;		// width of pum items base
static số_nguyên pum_kind_width;		// width of pum items kind column
static số_nguyên pum_extra_width;		// width of extra stuff
static số_nguyên pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
static số_nguyên pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

static số_nguyên pum_row;			// top row of pum
static số_nguyên pum_col;			// left column of pum

static win_T *pum_window = NULL;
static số_nguyên pum_win_row;
static số_nguyên pum_win_height;
static số_nguyên pum_win_col;
static số_nguyên pum_win_wcol;
static số_nguyên pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
static số_nguyên pum_pretend_not_visible = FALSE;

// Border support
static số_nguyên pum_border = 0;
static số_nguyên pum_margin = 0;	// margin of 1 cell on left and right
static số_nguyên pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
static image_placement_T *pum_imagep = NULL;
#endif

// Border characters
static struct {
    số_nguyên top;
    số_nguyên right;
    số_nguyên bottom;
    số_nguyên left;
    số_nguyên top_left;
    số_nguyên top_right;
    số_nguyên bottom_right;
    số_nguyên bottom_left;
} pum_border_chars;

static số_nguyên pum_set_selected(số_nguyên n, số_nguyên repeat);
static void pum_draw_border(void);
static void pum_draw_shadow(void);
static void compute_margins(số_nguyên *right_margin, số_nguyên *left_margin, số_nguyên *left_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	void \
    pum_set_##feature(số_nguyên enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    static void
pum_compute_size(void)
{
    số_nguyên	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    cho (số_nguyên i = 0; i < pum_size; ++i)
    {
	nếu (pum_array[i].pum_text != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    nếu (pum_base_width < w)
		pum_base_width = w;
	}
	nếu (pum_array[i].pum_kind != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    nếu (pum_kind_width < w)
		pum_kind_width = w;
	}
	nếu (pum_array[i].pum_extra != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    nếu (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    static void
pum_compute_vertical_placement(
	số_nguyên size,
	số_nguyên above_row,
	số_nguyên below_row)
{
    số_nguyên context_lines;
    số_nguyên cline_visible_offset;
    số_nguyên extra_height = 2 * pum_border + pum_shadow;
    số_nguyên extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    nếu (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    nếu (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	nếu (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_lines = 0;
	không_thì
	    // Leave two lines of context if possible
	    context_lines = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	nếu (pum_win_row >= size + context_lines + extra_height)
	{
	    pum_row = pum_win_row - size - context_lines - extra_offset;
	    pum_height = size;
	}
	không_thì
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_lines - extra_height;
	}
	nếu (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    không_thì
    {
	// pum below "pum_win_row"
	nếu (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_lines = 0;
	không_thì
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_lines = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_lines + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	nếu (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#nếu defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    nếu (above_row > 0 && pum_row < above_row && pum_height > above_row)
    {
	pum_row = above_row;
	pum_height = pum_win_row - above_row;
    }
#endif
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    static số_nguyên
set_pum_width_aligned_with_cursor(số_nguyên width, số_nguyên available_width)
{
    số_nguyên end_padding = TRUE;

    nếu (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    nếu (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    trả_về available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    static void
pum_compute_horizontal_placement(số_nguyên cursor_col)
{
    số_nguyên desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    số_nguyên available_width;
    số_nguyên col_offset;
    số_nguyên trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    không_thì
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    nếu (set_pum_width_aligned_with_cursor(desired_width, available_width))
	trả_về;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    nếu (available_width > p_pw)
    {
	pum_width = available_width;
	trả_về;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    nếu (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	không_thì
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	trả_về;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
	pum_col = Columns - 1 - col_offset;
    không_thì
#endif
	pum_col = col_offset;
    pum_width = Columns - trailing_overhead;
}

/*
 * Show the popup menu with items "array[size]".
 * "array" must remain valid until pum_undisplay() is called!
 * When possible the leftmost character is aligned with cursor column.
 * The menu appears above the screen line "row" or at "row" + "height" - 1.
 */
    void
pum_display(
	pumitem_T   *array,
	số_nguyên	    size,
	số_nguyên	    selected,   // index of initially selected item, -1 if
				// out of range
	số_nguyên	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    số_nguyên	    cursor_col;
    số_nguyên	    above_row;
    số_nguyên	    below_row;
    số_nguyên	    redo_count = 0;
#nếu defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    do
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = NULL;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	nếu (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	không_thì
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#nếu defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    nếu (pvwin->w_p_pvw)
		break;
	nếu (pvwin != NULL)
	{
	    nếu (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    không_thì nếu (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	nếu (pum_height < 1 || (pum_height == 1 && size > 1))
	    trả_về;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	nếu (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	không_thì
	{
	    số_nguyên wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    nếu (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    không_thì
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    nếu (curwin->w_p_cole > 0 && conceal_cursor_line(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		nếu (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    nếu (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    không_thì
#endif
		cursor_col = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_height < size) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_col);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } trong_khi (pum_set_selected(selected, redo_count) && ++redo_count <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    void
pum_call_update_screen(void)
{
    call_update_screen = TRUE;

    // Update the cursor position to be able to compute the popup menu
    // position.  The cursor line length may have changed because of the
    // inserted completion.
    curwin->w_valid &= ~(VALID_CROW|VALID_CHEIGHT);
    validate_cursor();
}

/*
 * Return TRUE if we are going to redraw the popup menu and the screen position
 * "row"/"col" is under the popup menu.
 */
    số_nguyên
pum_under_menu(số_nguyên row, số_nguyên col, số_nguyên only_redrawing)
{
    số_nguyên	extra_left = pum_border + (pum_margin && pum_border ? 1 : 0);
    số_nguyên	extra_right = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    số_nguyên	extra_above = pum_border;
    số_nguyên	extra_below = pum_border + (pum_shadow ? 1 : 0);
    số_nguyên	top = pum_row - extra_above;
    số_nguyên	bot = pum_row + pum_height + extra_below;
    số_nguyên	left = pum_col - 1 - extra_left;
    số_nguyên	right = pum_col + pum_width + pum_scrollbar + extra_right;

    nếu (!((!only_redrawing || pum_will_redraw)
	    && row >= top && row < bot && col >= left && col < right))
	trả_về FALSE;

    nếu (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	nếu (only_redrawing)
	{
	    nếu (col >= right - 2 || row == bot - 1)
		trả_về FALSE;
	}
	không_thì
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    số_nguyên	    right_margin, left_margin, left_padding;

	    compute_margins(&right_margin, &left_margin, &left_padding);
	    nếu (row == top && col >= right - 2)
		trả_về FALSE;
	    nếu (row == bot - 1 && col < pum_col + 2 - left_padding - pum_border
		    - left_margin)
		trả_về FALSE;
	}
    }
    trả_về TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    static số_nguyên *
pum_compute_text_attrs(char_u *text, hlf_T hlf, số_nguyên user_hlattr)
{
    số_nguyên		i;
    size_t	leader_len;
    số_nguyên		char_cells;
    số_nguyên		new_attr;
    char_u	*ptr = text;
    số_nguyên		cell_idx = 0;
    garray_T	*ga = NULL;
    số_nguyên		*attrs = NULL;
    char_u	*leader = NULL;
    số_nguyên		in_fuzzy;
    số_nguyên		matched_len = -1;
    int_u	char_pos = 0;
    số_nguyên		is_select = FALSE;

    nếu (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	trả_về NULL;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    nếu (leader == NULL || *leader == NUL)
	trả_về NULL;

    attrs = ALLOC_MULT(số_nguyên, vim_strsize(text));
    nếu (attrs == NULL)
	trả_về NULL;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    nếu (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	nếu (!ga)
	{
	    vim_free(attrs);
	    trả_về NULL;
	}
    }

    trong_khi (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	nếu (ga != NULL)
	{
	    // Handle fuzzy matching
	    cho (i = 0; i < ga->ga_len; i++)
	    {
		nếu (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    break;
		}
	    }
	}
	không_thì
	{
	    nếu (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (số_nguyên)leader_len;
	    nếu (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	nếu (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	cho (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    nếu (ga != NULL)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    trả_về attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    static void
pum_screen_puts_with_attrs(
    số_nguyên		row,
    số_nguyên		col,
    số_nguyên		cells UNUSED,
    char_u	*text,
    số_nguyên		textlen,
    số_nguyên		*attrs)
{
    số_nguyên		col_start = col;
    char_u	*ptr = text;
    số_nguyên		char_len;
    số_nguyên		attr;

    // Render text with proper attributes
    trong_khi (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	không_thì
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    static inline char_u *
pum_get_item(số_nguyên index, số_nguyên type)
{
    switch(type)
    {
	case CPT_ABBR: trả_về pum_array[index].pum_text;
	case CPT_KIND: trả_về pum_array[index].pum_kind;
	case CPT_MENU: trả_về pum_array[index].pum_extra;
    }
    trả_về NULL;
}

    static inline số_nguyên
pum_user_attr_combine(số_nguyên idx, số_nguyên type, số_nguyên attr)
{
    số_nguyên user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    trả_về user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    static số_nguyên
pum_display_rtl_text(
	số_nguyên	row,
	số_nguyên     col,
	char_u  *st,
	số_nguyên     attr,
	số_nguyên     *attrs,
	số_nguyên     width,
	số_nguyên     width_limit,
	số_nguyên     totwidth,
	số_nguyên     next_isempty,
	số_nguyên	selected)
{
    char_u  *rt = NULL;
    số_nguyên     cells = 0;
    số_nguyên     over_cell = 0;
    số_nguyên     truncated = FALSE;
    số_nguyên     pad = next_isempty ? 0 : 2;
    số_nguyên     remaining = 0;
    số_nguyên	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    số_nguyên	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    nếu (st == NULL)
	trả_về col;

    rt = reverse_text(st);
    nếu (rt == NULL)
    {
	VIM_CLEAR(st);
	trả_về col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    nếu (cells > width_limit)
    {
	do
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} trong_khi (cells > width_limit);

	nếu (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    nếu (truncated)
    {
	char_u  *orig_rt = rt;
	số_nguyên     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	nếu (cells > remaining)
	{
	    trong_khi (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (số_nguyên)STRLEN(orig_rt);
	nếu (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	nếu (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    nếu (attrs == NULL)
	screen_puts_len(rt, (số_nguyên)STRLEN(rt), row, col - cells + 1, attr);
    không_thì
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(số_nguyên)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    trả_về col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    static số_nguyên
pum_display_ltr_text(
	số_nguyên     row,
	số_nguyên     col,
	char_u  *st,
	số_nguyên     attr,
	số_nguyên     *attrs,
	số_nguyên     width,        // width already calculated in outer loop
	số_nguyên     width_limit,
	số_nguyên     totwidth,
	số_nguyên     next_isempty,
	số_nguyên	selected)
{
    số_nguyên     size = 0;
    số_nguyên     cells = 0;
    char_u  *st_end = NULL;
    số_nguyên     over_cell = 0;
    số_nguyên     pad = next_isempty ? 0 : 2;
    số_nguyên     truncated = FALSE;
    số_nguyên     remaining = 0;
    số_nguyên	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    số_nguyên	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    nếu (st == NULL)
	trả_về col;

    size = (số_nguyên)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    trong_khi (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	nếu (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	không_thì
	    --cells;
    }

    // truncated
    nếu (truncated)
    {
	remaining = width_limit - totwidth - 1;
	nếu (cells > remaining)
	{
	    st_end = st + size;
	    trong_khi (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	nếu (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    nếu (attrs == NULL)
	screen_puts_len(st, size, row, col, attr);
    không_thì
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    nếu (truncated)
    {
	nếu (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    trả_về col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    static số_nguyên
pum_process_item(
	số_nguyên     row,
	số_nguyên     col,
	số_nguyên     idx,
	số_nguyên     j,         // Current position in order array
	số_nguyên     *order,    // Order array
	hlf_T   hlf,
	số_nguyên     attr,
	số_nguyên     *totwidth_ptr,
	số_nguyên     next_isempty)
{
    số_nguyên     item_type = order[j];
    char_u  *s = NULL;
    char_u  *p = pum_get_item(idx, item_type);
    số_nguyên     width = 0;  // item width
    số_nguyên     w;		// char width
    số_nguyên	    selected = idx == pum_selected;

    cho ( ; ; MB_PTR_ADV(p))
    {
	nếu (s == NULL)
	    s = p;
	w = ptr2cells(p);
	nếu (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    continue;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	số_nguyên     *attrs = NULL;
	số_nguyên     saved = *p;

	nếu (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	nếu (saved != NUL)
	    *p = saved;

	nếu (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	không_thì
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	nếu (attrs != NULL)
	    VIM_CLEAR(attrs);

	nếu (*p != TAB)
	    break;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	không_thì
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = NULL;  // start text at next char
	width = 0;
    }

    trả_về col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    static void
pum_draw_scrollbar(
	số_nguyên	row,
	số_nguyên	i,
	số_nguyên	thumb_pos,
	số_nguyên	thumb_height)
{
    nếu (pum_scrollbar <= 0)
	trả_về;

    số_nguyên attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    không_thì
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    static inline void
pum_align_order(số_nguyên *order)
{
    số_nguyên is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

static void pum_free_bg(void);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    void
pum_opacity_changed(void)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    nếu (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    static void
pum_free_bg(void)
{
    số_nguyên k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_lines);
    VIM_CLEAR(pum_bg_linesUC);
    cho (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_linesC[k]);
    pum_bg_top = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    void
pum_redraw(void)
{
    số_nguyên		row = pum_row;
    số_nguyên		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    số_nguyên		attr;
    số_nguyên		i, j;
    số_nguyên		idx;
    char_u	*p = NULL;
    số_nguyên		totwidth;
    số_nguyên		thumb_pos = 0;
    số_nguyên		thumb_height = 1;
    số_nguyên		item_type;
    số_nguyên		order[3];
    số_nguyên		next_isempty = FALSE;
    số_nguyên		n;
    số_nguyên		items_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    số_nguyên		basic_width;  // first item width
    số_nguyên		last_isabbr = FALSE;
    số_nguyên		orig_attr = -1;
    số_nguyên		scroll_range = pum_size - pum_height;
    bool	override_success;
    số_nguyên		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = push_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    nếu (pum_imagep == NULL)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(NULL, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    nếu (pum_imagep != NULL)
    {
	image_placement_set_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_set_position(pum_imagep, pum_row, pum_col);
	image_placement_set_bounding_box(pum_imagep, pum_row, pum_col,
		pum_height, pum_width);
	image_placement_set_enable(pum_imagep, true);
    }
#endif

    hlf_T	hlfsNorm[3];
    hlf_T	hlfsSel[3];
    // "word"/"abbr"
    hlfsNorm[0] = HLF_PNI;
    hlfsSel[0] = HLF_PSI;
    // "kind"
    hlfsNorm[1] = HLF_PNK;
    hlfsSel[1] = HLF_PSK;
    // "extra text"
    hlfsNorm[2] = HLF_PNX;
    hlfsSel[2] = HLF_PSX;

    nếu (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	nếu (opacity_active && pum_bg_lines != NULL
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	nếu (opacity_active && pum_bg_lines != NULL)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	không_thì nếu (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    nếu (ScreenLines != NULL && ScreenAttrs != NULL)
	    {
		số_nguyên	save_top, save_bot, save_ncells, k;

		pum_free_bg();
		save_top = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		nếu (save_top < 0)
		    save_top = 0;
		nếu (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_top = save_top;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		nếu (save_top < save_bot)
		{
		    save_ncells = (save_bot - save_top) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_lines = LALLOC_MULT(schar_T, save_ncells);
		    nếu (enc_utf8)
		    {
			pum_bg_linesUC = LALLOC_MULT(u8char_T, save_ncells);
			cho (k = 0; k < MAX_MCO; ++k)
			    pum_bg_linesC[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    nếu (pum_bg_attrs != NULL && pum_bg_lines != NULL)
		    {
			cho (số_nguyên r = save_top; r < save_bot; ++r)
			{
			    số_nguyên soff = (r - save_top) * screen_Columns;
			    số_nguyên loff = LineOffset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * sizeof(sattr_T));
			    mch_memmove(pum_bg_lines + soff,
				    ScreenLines + loff,
				    screen_Columns * sizeof(schar_T));
			    nếu (enc_utf8 && pum_bg_linesUC != NULL
						&& ScreenLinesUC != NULL)
			    {
				mch_memmove(pum_bg_linesUC + soff,
					ScreenLinesUC + loff,
					screen_Columns * sizeof(u8char_T));
				cho (k = 0; k < MAX_MCO; ++k)
				    nếu (pum_bg_linesC[k] != NULL
						    && ScreenLinesC[k] != NULL)
					mch_memmove(pum_bg_linesC[k] + soff,
						ScreenLinesC[k] + loff,
						screen_Columns
						    * sizeof(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	không_thì
	{
	    // Do not redraw in pum_may_redraw() and don't draw in the area
	    // where the popup menu will be.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
    }

    // never display more than we have
    pum_first = MIN(pum_first, scroll_range);

    nếu (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	nếu (thumb_height == 0)
	    thumb_height = 1;
	thumb_pos = (pum_first * (pum_height - thumb_height)
			    + scroll_range / 2) / scroll_range;
    }

#ifdef FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#endif

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    nếu (pum_border)
	pum_draw_border();
    nếu (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    nếu (opacity_active)
	screen_pum_blend = 100 - (số_nguyên)p_po;

    cho (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	{
	    nếu (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	không_thì
#endif
	    nếu (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = items_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	cho (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    nếu (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    nếu (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == NULL;
	    không_thì
		next_isempty = TRUE;

	    nếu (p != NULL)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    nếu (j > 0)
		n = items_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    không_thì
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    nếu (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == NULL)))
		    || basic_width + n >= pum_width)
		break;
#ifdef FEAT_RIGHTLEFT
	    nếu (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    không_thì
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	không_thì
#endif
	    screen_fill(row, row + 1, col, pum_col + pum_width, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(row, i, thumb_pos, thumb_height);

	++row;
    }

    screen_pum_blend = 0;

#ifdef FEAT_PROP_POPUP
    screen_zindex = 0;
#endif

    nếu (override_success)
	pop_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#nếu defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    void
pum_position_info_popup(win_T *wp)
{
    số_nguyên col = pum_col + pum_width + pum_scrollbar + 1;
    số_nguyên row = pum_row;
    số_nguyên botpos = POPPOS_BOTLEFT;
    số_nguyên	used_maxwidth_opt = FALSE;
    số_nguyên	right_margin, left_margin, dummy;
    số_nguyên left_extra_width, right_extra_width;

    (void)compute_margins(&right_margin, &left_margin, &dummy);

    left_extra_width = pum_border + left_margin;
    right_extra_width = pum_border + right_margin + (pum_shadow ? 2 : 0);
    col += right_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    nếu (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - left_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - left_extra_width;
    }
    không_thì
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    nếu (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_top_extra(wp);
    nếu (wp->w_popup_flags & POPF_INFO_MENU)
    {
	nếu (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	không_thì
	    // menu below cursor line, align with top
	    row += 1;
    }
    không_thì
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    nếu (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    không_thì
	popup_set_wantpos_rowcol(wp, row, col);
}
#endif

/*
 * Set the index of the currently selected item.  The menu will scroll when
 * necessary.  When "n" is out of range don't scroll.
 * This may be repeated when the preview window is used:
 * "repeat" == 0: open preview window normally
 * "repeat" == 1: open preview window but don't set the size
 * "repeat" == 2: don't open preview window
 * Returns TRUE when the window was resized and the location of the popup menu
 * must be recomputed.
 */
    static số_nguyên
pum_set_selected(số_nguyên n, số_nguyên repeat UNUSED)
{
    số_nguyên	    resized = FALSE;
    số_nguyên	    context = pum_height / 2;
    số_nguyên	    scroll_offset;
#ifdef FEAT_QUICKFIX
    số_nguyên	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#nếu defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    số_nguyên	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    nếu (pum_selected >= 0 && pum_selected < pum_size)
    {
	nếu (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    nếu (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		nếu (pum_first < 0)
		    pum_first = 0;
		không_thì nếu (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    không_thì
		pum_first = pum_selected;
	}
	không_thì nếu (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    nếu (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    không_thì
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	nếu (pum_height > 2)
	{
	    nếu (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    không_thì nếu (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#nếu defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	nếu (pum_array[pum_selected].pum_info != NULL
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# không_thì
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    nếu (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    không_thì nếu (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    không_thì
		use_popup = USEPOPUP_NONE;
	    nếu (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    nếu (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    nếu (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    nếu (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		số_nguyên	res = OK;

		nếu (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == NULL
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    trong_khi (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		không_thì
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, 0, NULL);
		    --no_u_sync;
		    nếu (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_value_give_err((char_u *)"swf",
							  0L, NULL, OPT_LOCAL);
			set_option_value_give_err((char_u *)"bl",
							  0L, NULL, OPT_LOCAL);
			set_option_value_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_value_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_value_give_err((char_u *)"diff",
							  0L, NULL, OPT_LOCAL);
		    }
		}
		nếu (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    cho (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			nếu (e == NULL)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    break;
			}
			*e = NUL;
			ml_append(lnum++, p, (số_nguyên)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_line_count);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    nếu (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			nếu (curwin->w_height < lnum)
			{
			    win_setheight((số_nguyên)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    nếu (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    không_thì nếu (curwin->w_topline > curbuf->b_ml.ml_line_count)
			curwin->w_topline = curbuf->b_ml.ml_line_count;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    nếu (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			nếu (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    nếu ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			số_nguyên save_redr_status;

			nếu (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			nếu (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			nếu (resized && win_valid(curwin_save))
			{
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
			    update_topline();
			}

			// Update the screen before drawing the popup menu.
			// Enable updating the status lines.
			pum_pretend_not_visible = TRUE;

			// But don't draw text at the new popup menu position,
			// it causes flicker.  When resizing we need to draw
			// anyway, the position may change later.
			// Also do not redraw the status line of the original
			// current window here, to avoid it gets drawn with
			// StatusLineNC for a moment and cause flicker.
			pum_will_redraw = !resized;
			save_redr_status = curwin_save->w_redr_status;
			curwin_save->w_redr_status = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_status = save_redr_status;

			nếu (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    nếu (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
				popup_hide(wp);
# endif
			}

			// May need to update the screen again when there are
			// autocommands involved.
			pum_pretend_not_visible = TRUE;
			pum_will_redraw = !resized;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			call_update_screen = FALSE;
		    }
		}
	    }
# nếu defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    nếu (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    nếu (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#nếu defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    nếu (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    trả_về resized;
}

/*
 * Undisplay the popup menu (later).
 */
    void
pum_undisplay(void)
{
#ifdef FEAT_IMAGE_POPUP
    nếu (pum_imagep != NULL)
    {
	image_placement_free(pum_imagep);
	pum_imagep = NULL;
    }
#endif
    pum_free_bg();
    pum_array = NULL;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#nếu defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    nếu (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#nếu defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    void
pum_clear(void)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    số_nguyên
pum_visible(void)
{
    trả_về !pum_pretend_not_visible && pum_array != NULL;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    static số_nguyên
pum_in_same_position(void)
{
    số_nguyên	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    trả_về pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    số_nguyên
pum_redraw_in_same_position(void)
{
    nếu (!pum_visible() || pum_will_redraw)
	trả_về FALSE;  // nothing to do

    trả_về pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    void
pum_may_redraw(void)
{
    pumitem_T	*array = pum_array;
    số_nguyên		len = pum_size;
    số_nguyên		selected = pum_selected;

    nếu (!pum_visible() || pum_will_redraw)
	trả_về;  // nothing to do

    nếu (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    không_thì
    {
	// Window layout changed, recompute the position.
	// Use the remembered w_wcol value, the cursor may have moved when a
	// completion was inserted, but we want the menu in the same position.
	pum_undisplay();
	pum_display(array, len, selected, pum_win_wcol);
    }
}

/*
 * Return the height of the popup menu, the number of entries visible.
 * Only valid when pum_visible() returns TRUE!
 */
    số_nguyên
pum_get_height(void)
{
    trả_về pum_height;
}

#nếu defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    void
pum_set_event_info(dict_T *dict)
{
    nếu (!pum_visible())
	trả_về;
    (void)dict_add_number(dict, "height", pum_height);
    (void)dict_add_number(dict, "width", pum_width);
    (void)dict_add_number(dict, "row", pum_row);
    (void)dict_add_number(dict, "col", pum_col);
    (void)dict_add_number(dict, "size", pum_size);
    (void)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#nếu defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    static void
pum_position_at_mouse(số_nguyên min_width)
{
    số_nguyên extra_height = pum_border * 2 + pum_shadow;
    số_nguyên extra_offset = pum_border + pum_shadow;
    số_nguyên trailing_overhead = 0;
    số_nguyên col_offset = 0;
    số_nguyên available_width;

    nếu (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	nếu (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	nếu (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    không_thì
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	nếu (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	nếu (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	không_thì
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    không_thì
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	nếu (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	không_thì
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = NULL;
}

#endif

#nếu defined(FEAT_BEVAL_TERM)
static pumitem_T *balloon_array = NULL;
static số_nguyên balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

typedef struct {
    char_u	*start;
    số_nguyên		bytelen;
    số_nguyên		cells;
    số_nguyên		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    số_nguyên
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    số_nguyên		quoted = FALSE;
    số_nguyên		height;
    số_nguyên		line;
    số_nguyên		item_idx;
    số_nguyên		indent = 0;
    số_nguyên		max_cells = 0;
    số_nguyên		max_height = Rows / 2 - 1;
    số_nguyên		long_item_count = 0;
    số_nguyên		split_long_items = FALSE;

    ga_init2(&ga, sizeof(balpart_T), 20);
    p = mesg;

    trong_khi (*p != NUL)
    {
	nếu (ga_grow(&ga, 1) == FAIL)
	    goto failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	trong_khi (*p != NUL)
	{
	    nếu (*p == '"')
		quoted = !quoted;
	    không_thì nếu (*p == '\n')
		break;
	    không_thì nếu (*p == '\\' && p[1] != NUL)
		++p;
	    không_thì nếu (!quoted)
	    {
		nếu ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    nếu (*p == '{')
			++indent;
		    không_thì nếu (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    break;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	nếu (*p == '\n')
	    ++p;
	nếu (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_count += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    nếu (long_item_count > 0 && height + long_item_count <= max_height)
    {
	split_long_items = TRUE;
	height += long_item_count;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    nếu (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    nếu (*array == NULL)
	goto failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    cho (line = 1, item_idx = 0; line < height - 1; ++item_idx)
    {
	số_nguyên	skip;
	số_nguyên	thislen;
	số_nguyên	copylen;
	số_nguyên	ind;
	số_nguyên	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	nếu (item->bytelen == 0)
	    (*array)[line++].pum_text = vim_strsave((char_u *)"");
	không_thì
	    cho (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		nếu (split_long_items && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    cho (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			nếu ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    break;
		    thislen = p - (item->start + skip);
		}
		không_thì
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		nếu (p == NULL)
		{
		    cho (line = 0; line <= height - 1; ++line)
			vim_free((*array)[line].pum_text);
		    vim_free(*array);
		    goto failed;
		}
		cho (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		cho (copylen = thislen; copylen > 0; --copylen)
		    nếu (item->start[skip + copylen - 1] != ' ')
			break;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[line].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++line;
	    }
    }
    ga_clear(&ga);
    trả_về height;

failed:
    ga_clear(&ga);
    trả_về 0;
}

    void
ui_remove_balloon(void)
{
    nếu (balloon_array == NULL)
	trả_về;

    pum_undisplay();
    trong_khi (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    void
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    nếu (mesg == NULL && list == NULL)
    {
	pum_undisplay();
	trả_về;
    }
    nếu (list != NULL)
    {
	listitem_T  *li;
	số_nguyên	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	nếu (balloon_array == NULL)
	    trả_về;
	CHECK_LIST_MATERIALIZE(list);
	cho (idx = 0, li = list->lv_first; li != NULL; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == NULL ? (char_u *)"" : text);
	}
    }
    không_thì
	balloon_arraysize = split_message(mesg, &balloon_array);

    nếu (balloon_arraysize <= 0)
	trả_về;

    pum_array = balloon_array;
    pum_size = balloon_arraysize;
    pum_compute_size();
    pum_scrollbar = 0;
    pum_height = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_WIDTH);
    pum_selected = -1;
    pum_first = 0;
    pum_redraw();
}

/*
 * Called when the mouse moved, may remove any displayed balloon.
 */
    void
ui_may_remove_balloon(void)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#nếu defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    static void
pum_select_mouse_pos(void)
{
    số_nguyên idx = mouse_row - pum_row;

    nếu (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    không_thì nếu (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    static void
pum_execute_menu(vimmenu_T *menu, số_nguyên mode)
{
    vimmenu_T   *mp;
    số_nguyên		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	nếu ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    break;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    void
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    số_nguyên		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    số_nguyên		save_bevalterm = p_bevalterm;
# endif
    số_nguyên		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	nếu (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    nếu (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	trả_về;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    nếu (array == NULL)
	trả_về;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = NULL;

	// Make a copy of the text, the menu may be redefined in a callback.
	nếu (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	không_thì nếu (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	nếu (s != NULL)
	{
	    s = vim_strsave(s);
	    nếu (s != NULL)
		array[idx++].pum_text = s;
	}
    }

    pum_array = array;
    pum_compute_size();
    pum_scrollbar = 0;
    pum_height = pum_size;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_first = 0;
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    cho (;;)
    {
	số_nguyên	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	nếu (c == ESC || c == Ctrl_C || pum_array == NULL)
	    break;
	không_thì nếu (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    break;
	}
	không_thì nếu (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    trong_khi (pum_selected > 0)
	    {
		--pum_selected;
		nếu (*array[pum_selected].pum_text != NUL)
		    break;
	    }
	}
	không_thì nếu (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    trong_khi (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		nếu (*array[pum_selected].pum_text != NUL)
		    break;
	    }
	}
	không_thì nếu (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    break;
	}
	không_thì nếu (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	không_thì nếu (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    nếu (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		break;
	    }
	    nếu (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		break;
	}
    }

    cho (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    void
pum_make_popup(char_u *path_name, số_nguyên use_mouse_pos)
{
    vimmenu_T *menu;

    nếu (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_row = W_WINROW(curwin) + curwin->w_wrow;
	mouse_col =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_find_menu(path_name);
    nếu (menu != NULL)
	pum_show_popupmenu(menu);
}
#endif

    static số_nguyên
is_singlewidth(số_nguyên c)
{
    nếu (c >= 0 && c < 0x80)
	trả_về TRUE;
    nếu (utf_char2cells(c) == 1)
	trả_về TRUE;
    trả_về FALSE;
}

/*
 * Set border characters for popup menu.
 */
    void
pum_set_border_chars(
	số_nguyên top,
	số_nguyên right,
	số_nguyên bottom,
	số_nguyên left,
	số_nguyên top_left,
	số_nguyên top_right,
	số_nguyên bottom_right,
	số_nguyên bottom_left)
{
    nếu (!is_singlewidth(top) || !is_singlewidth(right)
	    || !is_singlewidth(bottom) || !is_singlewidth(left)
	    || !is_singlewidth(top_left) || !is_singlewidth(top_right)
	    || !is_singlewidth(bottom_right) || !is_singlewidth(bottom_left))
	trả_về;
    pum_border_chars.top = top;
    pum_border_chars.right = right;
    pum_border_chars.bottom = bottom;
    pum_border_chars.left = left;
    pum_border_chars.top_left = top_left;
    pum_border_chars.top_right = top_right;
    pum_border_chars.bottom_right = bottom_right;
    pum_border_chars.bottom_left = bottom_left;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    static void
compute_margins(số_nguyên *right_margin, số_nguyên *left_margin, số_nguyên *left_padding)
{
    *right_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	*left_padding = (pum_col < Columns - 1 - pum_border);
	*left_margin = *right_margin && (pum_col < Columns - 3);
	trả_về;
    }
#endif
    *left_padding = (pum_col > pum_border);
    *left_margin = *right_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    static void
pum_draw_border(void)
{
    số_nguyên	top_row = pum_row - 1;
    số_nguyên	bottom_row = pum_row + pum_height;
    số_nguyên	col;
    số_nguyên	attr = highlight_attr[HLF_PMB];
    số_nguyên	right_margin, left_margin, left_padding;
    số_nguyên	inner_width;

    nếu (!pum_border)
	trả_về;

    (void)compute_margins(&right_margin, &left_margin, &left_padding);

    inner_width = pum_width + pum_scrollbar + left_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
	col = pum_col + left_padding + 1;
    không_thì
#endif
	col = pum_col - left_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	screen_putchar(pum_border_chars.top_right, top_row, col, attr);
	nếu (left_margin)
	    screen_putchar(' ', top_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_right, bottom_row, col, attr);
	nếu (left_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    không_thì
#endif
    {
	screen_putchar(pum_border_chars.top_left, top_row, col, attr);
	nếu (left_margin)
	    screen_putchar(' ', top_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_left, bottom_row, col, attr);
	nếu (left_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    cho (số_nguyên i = 0; i < inner_width; i++)
    {
	số_nguyên cur_col;
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    cur_col = col - 1 - i;
	không_thì
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.top, top_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	screen_putchar(pum_border_chars.top_left, top_row,
		col - inner_width - 1, attr);
	nếu (right_margin)
	    screen_putchar(' ', top_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_left, bottom_row,
		col - inner_width - 1, attr);
	nếu (right_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    không_thì
#endif
    {
	screen_putchar(pum_border_chars.top_right, top_row,
		col + inner_width + 1, attr);
	nếu (right_margin)
	    screen_putchar(' ', top_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_right, bottom_row,
		col + inner_width + 1, attr);
	nếu (right_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    cho (số_nguyên i = 0; i < pum_height; i++)
    {
	số_nguyên row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	{
	    screen_putchar(pum_border_chars.left, row, col, attr);
	    nếu (left_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.right, row,
		    col - inner_width - 1, attr);
	    nếu (right_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	không_thì
#endif
	{
	    screen_putchar(pum_border_chars.left, row, col, attr);
	    nếu (left_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.right, row,
		    col + inner_width + 1, attr);
	    nếu (right_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    void
put_shadow_char(số_nguyên row, số_nguyên col)
{
    char_u  buf[MB_MAXBYTES + 1];
    số_nguyên	    shadow_attr = highlight_attr[HLF_PMS];
    số_nguyên	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    static void
pum_draw_shadow(void)
{
    số_nguyên	row, col, next_col;
    số_nguyên	width, offset;
    số_nguyên	inner_width;
    số_nguyên	right_margin, left_margin, left_padding;

    nếu (!pum_shadow)
	trả_về;

    (void)compute_margins(&right_margin, &left_margin, &left_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + right_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    nếu (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    không_thì
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    cho (số_nguyên i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + left_padding + pum_border + left_margin;
    offset = 2 - left_padding - pum_border - left_margin;

    cho (số_nguyên i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	nếu (pum_rl)
	    col = pum_col - offset - i;
	không_thì
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
