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
static ຈຳນວນເຕັມ pum_size;			// nr of items in "pum_array"
static ຈຳນວນເຕັມ pum_selected;		// index of selected item or -1
static ຈຳນວນເຕັມ pum_first = 0;		// index of top item

static ຈຳນວນເຕັມ call_update_screen = FALSE;
static ຈຳນວນເຕັມ pum_in_cmdline = FALSE;

static ຈຳນວນເຕັມ pum_height;			// nr of displayed pum items
static ຈຳນວນເຕັມ pum_width;			// width of displayed pum items
static ຈຳນວນເຕັມ pum_base_width;		// width of pum items base
static ຈຳນວນເຕັມ pum_kind_width;		// width of pum items kind column
static ຈຳນວນເຕັມ pum_extra_width;		// width of extra stuff
static ຈຳນວນເຕັມ pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
static ຈຳນວນເຕັມ pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

static ຈຳນວນເຕັມ pum_row;			// top row of pum
static ຈຳນວນເຕັມ pum_col;			// left column of pum

static win_T *pum_window = NULL;
static ຈຳນວນເຕັມ pum_win_row;
static ຈຳນວນເຕັມ pum_win_height;
static ຈຳນວນເຕັມ pum_win_col;
static ຈຳນວນເຕັມ pum_win_wcol;
static ຈຳນວນເຕັມ pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
static ຈຳນວນເຕັມ pum_pretend_not_visible = FALSE;

// Border support
static ຈຳນວນເຕັມ pum_border = 0;
static ຈຳນວນເຕັມ pum_margin = 0;	// margin of 1 cell on left and right
static ຈຳນວນເຕັມ pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
static image_placement_T *pum_imagep = NULL;
#endif

// Border characters
static sa_nracanaa {
    ຈຳນວນເຕັມ top;
    ຈຳນວນເຕັມ right;
    ຈຳນວນເຕັມ bottom;
    ຈຳນວນເຕັມ left;
    ຈຳນວນເຕັມ top_left;
    ຈຳນວນເຕັມ top_right;
    ຈຳນວນເຕັມ bottom_right;
    ຈຳນວນເຕັມ bottom_left;
} pum_border_chars;

static ຈຳນວນເຕັມ pum_set_selected(ຈຳນວນເຕັມ n, ຈຳນວນເຕັມ repeat);
static void pum_draw_border(void);
static void pum_draw_shadow(void);
static void compute_margins(ຈຳນວນເຕັມ *right_margin, ຈຳນວນເຕັມ *left_margin, ຈຳນວນເຕັມ *left_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	void \
    pum_set_##feature(ຈຳນວນເຕັມ enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    static void
pum_compute_size(void)
{
    ຈຳນວນເຕັມ	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pum_size; ++i)
    {
	ຖ້າ (pum_array[i].pum_text != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    ຖ້າ (pum_base_width < w)
		pum_base_width = w;
	}
	ຖ້າ (pum_array[i].pum_kind != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    ຖ້າ (pum_kind_width < w)
		pum_kind_width = w;
	}
	ຖ້າ (pum_array[i].pum_extra != NULL)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ຖ້າ (pum_extra_width < w)
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
	ຈຳນວນເຕັມ size,
	ຈຳນວນເຕັມ above_row,
	ຈຳນວນເຕັມ below_row)
{
    ຈຳນວນເຕັມ context_lines;
    ຈຳນວນເຕັມ cline_visible_offset;
    ຈຳນວນເຕັມ extra_height = 2 * pum_border + pum_shadow;
    ຈຳນວນເຕັມ extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    ຖ້າ (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ຖ້າ (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	ຖ້າ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_lines = 0;
	ບໍ່ດັ່ງນັ້ນ
	    // Leave two lines of context if possible
	    context_lines = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	ຖ້າ (pum_win_row >= size + context_lines + extra_height)
	{
	    pum_row = pum_win_row - size - context_lines - extra_offset;
	    pum_height = size;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_lines - extra_height;
	}
	ຖ້າ (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// pum below "pum_win_row"
	ຖ້າ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_lines = 0;
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_lines = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_lines + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	ຖ້າ (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#ຖ້າ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ຖ້າ (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    static ຈຳນວນເຕັມ
set_pum_width_aligned_with_cursor(ຈຳນວນເຕັມ width, ຈຳນວນເຕັມ available_width)
{
    ຈຳນວນເຕັມ end_padding = TRUE;

    ຖ້າ (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    ຖ້າ (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    ສົ່ງຄືນ available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    static void
pum_compute_horizontal_placement(ຈຳນວນເຕັມ cursor_col)
{
    ຈຳນວນເຕັມ desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    ຈຳນວນເຕັມ available_width;
    ຈຳນວນເຕັມ col_offset;
    ຈຳນວນເຕັມ trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    ຖ້າ (set_pum_width_aligned_with_cursor(desired_width, available_width))
	ສົ່ງຄືນ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ຖ້າ (available_width > p_pw)
    {
	pum_width = available_width;
	ສົ່ງຄືນ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    ຖ້າ (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	ບໍ່ດັ່ງນັ້ນ
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	ສົ່ງຄືນ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
	pum_col = Columns - 1 - col_offset;
    ບໍ່ດັ່ງນັ້ນ
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
	ຈຳນວນເຕັມ	    size,
	ຈຳນວນເຕັມ	    selected,   // index of initially selected item, -1 if
				// out of range
	ຈຳນວນເຕັມ	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    ຈຳນວນເຕັມ	    cursor_col;
    ຈຳນວນເຕັມ	    above_row;
    ຈຳນວນເຕັມ	    below_row;
    ຈຳນວນເຕັມ	    redo_count = 0;
#ຖ້າ defined(FEAT_QUICKFIX)
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
	ຖ້າ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	ບໍ່ດັ່ງນັ້ນ
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#ຖ້າ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ຖ້າ (pvwin->w_p_pvw)
		bha_nga;
	ຖ້າ (pvwin != NULL)
	{
	    ຖ້າ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	ຖ້າ (pum_height < 1 || (pum_height == 1 && size > 1))
	    ສົ່ງຄືນ;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	ຖ້າ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຈຳນວນເຕັມ wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ຖ້າ (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    ບໍ່ດັ່ງນັ້ນ
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ຖ້າ (curwin->w_p_cole > 0 && conceal_cursor_line(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ຖ້າ (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    ຖ້າ (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    ບໍ່ດັ່ງນັ້ນ
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
    } ໃນຂະນະທີ່ (pum_set_selected(selected, redo_count) && ++redo_count <= 2);

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
    ຈຳນວນເຕັມ
pum_under_menu(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col, ຈຳນວນເຕັມ only_redrawing)
{
    ຈຳນວນເຕັມ	extra_left = pum_border + (pum_margin && pum_border ? 1 : 0);
    ຈຳນວນເຕັມ	extra_right = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    ຈຳນວນເຕັມ	extra_above = pum_border;
    ຈຳນວນເຕັມ	extra_below = pum_border + (pum_shadow ? 1 : 0);
    ຈຳນວນເຕັມ	top = pum_row - extra_above;
    ຈຳນວນເຕັມ	bot = pum_row + pum_height + extra_below;
    ຈຳນວນເຕັມ	left = pum_col - 1 - extra_left;
    ຈຳນວນເຕັມ	right = pum_col + pum_width + pum_scrollbar + extra_right;

    ຖ້າ (!((!only_redrawing || pum_will_redraw)
	    && row >= top && row < bot && col >= left && col < right))
	ສົ່ງຄືນ FALSE;

    ຖ້າ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ຖ້າ (only_redrawing)
	{
	    ຖ້າ (col >= right - 2 || row == bot - 1)
		ສົ່ງຄືນ FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    ຈຳນວນເຕັມ	    right_margin, left_margin, left_padding;

	    compute_margins(&right_margin, &left_margin, &left_padding);
	    ຖ້າ (row == top && col >= right - 2)
		ສົ່ງຄືນ FALSE;
	    ຖ້າ (row == bot - 1 && col < pum_col + 2 - left_padding - pum_border
		    - left_margin)
		ສົ່ງຄືນ FALSE;
	}
    }
    ສົ່ງຄືນ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    static ຈຳນວນເຕັມ *
pum_compute_text_attrs(char_u *text, hlf_T hlf, ຈຳນວນເຕັມ user_hlattr)
{
    ຈຳນວນເຕັມ		i;
    size_t	leader_len;
    ຈຳນວນເຕັມ		char_cells;
    ຈຳນວນເຕັມ		new_attr;
    char_u	*ptr = text;
    ຈຳນວນເຕັມ		cell_idx = 0;
    garray_T	*ga = NULL;
    ຈຳນວນເຕັມ		*attrs = NULL;
    char_u	*leader = NULL;
    ຈຳນວນເຕັມ		in_fuzzy;
    ຈຳນວນເຕັມ		matched_len = -1;
    int_u	char_pos = 0;
    ຈຳນວນເຕັມ		is_select = FALSE;

    ຖ້າ (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ສົ່ງຄືນ NULL;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    ຖ້າ (leader == NULL || *leader == NUL)
	ສົ່ງຄືນ NULL;

    attrs = ALLOC_MULT(ຈຳນວນເຕັມ, vim_strsize(text));
    ຖ້າ (attrs == NULL)
	ສົ່ງຄືນ NULL;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ຖ້າ (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	ຖ້າ (!ga)
	{
	    vim_free(attrs);
	    ສົ່ງຄືນ NULL;
	}
    }

    ໃນຂະນະທີ່ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ຖ້າ (ga != NULL)
	{
	    // Handle fuzzy matching
	    ສຳລັບ (i = 0; i < ga->ga_len; i++)
	    {
		ຖ້າ (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    bha_nga;
		}
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (ຈຳນວນເຕັມ)leader_len;
	    ຖ້າ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ຖ້າ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	ສຳລັບ (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ຖ້າ (ga != NULL)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ສົ່ງຄືນ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    static void
pum_screen_puts_with_attrs(
    ຈຳນວນເຕັມ		row,
    ຈຳນວນເຕັມ		col,
    ຈຳນວນເຕັມ		cells UNUSED,
    char_u	*text,
    ຈຳນວນເຕັມ		textlen,
    ຈຳນວນເຕັມ		*attrs)
{
    ຈຳນວນເຕັມ		col_start = col;
    char_u	*ptr = text;
    ຈຳນວນເຕັມ		char_len;
    ຈຳນວນເຕັມ		attr;

    // Render text with proper attributes
    ໃນຂະນະທີ່ (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	ບໍ່ດັ່ງນັ້ນ
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    static anta_hstha char_u *
pum_get_item(ຈຳນວນເຕັມ index, ຈຳນວນເຕັມ type)
{
    switch(type)
    {
	pak_sa CPT_ABBR: ສົ່ງຄືນ pum_array[index].pum_text;
	pak_sa CPT_KIND: ສົ່ງຄືນ pum_array[index].pum_kind;
	pak_sa CPT_MENU: ສົ່ງຄືນ pum_array[index].pum_extra;
    }
    ສົ່ງຄືນ NULL;
}

    static anta_hstha ຈຳນວນເຕັມ
pum_user_attr_combine(ຈຳນວນເຕັມ idx, ຈຳນວນເຕັມ type, ຈຳນວນເຕັມ attr)
{
    ຈຳນວນເຕັມ user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    ສົ່ງຄືນ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    static ຈຳນວນເຕັມ
pum_display_rtl_text(
	ຈຳນວນເຕັມ	row,
	ຈຳນວນເຕັມ     col,
	char_u  *st,
	ຈຳນວນເຕັມ     attr,
	ຈຳນວນເຕັມ     *attrs,
	ຈຳນວນເຕັມ     width,
	ຈຳນວນເຕັມ     width_limit,
	ຈຳນວນເຕັມ     totwidth,
	ຈຳນວນເຕັມ     next_isempty,
	ຈຳນວນເຕັມ	selected)
{
    char_u  *rt = NULL;
    ຈຳນວນເຕັມ     cells = 0;
    ຈຳນວນເຕັມ     over_cell = 0;
    ຈຳນວນເຕັມ     truncated = FALSE;
    ຈຳນວນເຕັມ     pad = next_isempty ? 0 : 2;
    ຈຳນວນເຕັມ     remaining = 0;
    ຈຳນວນເຕັມ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ຈຳນວນເຕັມ	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ຖ້າ (st == NULL)
	ສົ່ງຄືນ col;

    rt = reverse_text(st);
    ຖ້າ (rt == NULL)
    {
	VIM_CLEAR(st);
	ສົ່ງຄືນ col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ຖ້າ (cells > width_limit)
    {
	do
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ໃນຂະນະທີ່ (cells > width_limit);

	ຖ້າ (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ຖ້າ (truncated)
    {
	char_u  *orig_rt = rt;
	ຈຳນວນເຕັມ     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ຖ້າ (cells > remaining)
	{
	    ໃນຂະນະທີ່ (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (ຈຳນວນເຕັມ)STRLEN(orig_rt);
	ຖ້າ (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	ຖ້າ (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    ຖ້າ (attrs == NULL)
	screen_puts_len(rt, (ຈຳນວນເຕັມ)STRLEN(rt), row, col - cells + 1, attr);
    ບໍ່ດັ່ງນັ້ນ
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(ຈຳນວນເຕັມ)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    ສົ່ງຄືນ col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    static ຈຳນວນເຕັມ
pum_display_ltr_text(
	ຈຳນວນເຕັມ     row,
	ຈຳນວນເຕັມ     col,
	char_u  *st,
	ຈຳນວນເຕັມ     attr,
	ຈຳນວນເຕັມ     *attrs,
	ຈຳນວນເຕັມ     width,        // width already calculated in outer loop
	ຈຳນວນເຕັມ     width_limit,
	ຈຳນວນເຕັມ     totwidth,
	ຈຳນວນເຕັມ     next_isempty,
	ຈຳນວນເຕັມ	selected)
{
    ຈຳນວນເຕັມ     size = 0;
    ຈຳນວນເຕັມ     cells = 0;
    char_u  *st_end = NULL;
    ຈຳນວນເຕັມ     over_cell = 0;
    ຈຳນວນເຕັມ     pad = next_isempty ? 0 : 2;
    ຈຳນວນເຕັມ     truncated = FALSE;
    ຈຳນວນເຕັມ     remaining = 0;
    ຈຳນວນເຕັມ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ຈຳນວນເຕັມ	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ຖ້າ (st == NULL)
	ສົ່ງຄືນ col;

    size = (ຈຳນວນເຕັມ)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ໃນຂະນະທີ່ (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	ຖ້າ (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	ບໍ່ດັ່ງນັ້ນ
	    --cells;
    }

    // truncated
    ຖ້າ (truncated)
    {
	remaining = width_limit - totwidth - 1;
	ຖ້າ (cells > remaining)
	{
	    st_end = st + size;
	    ໃນຂະນະທີ່ (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	ຖ້າ (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    ຖ້າ (attrs == NULL)
	screen_puts_len(st, size, row, col, attr);
    ບໍ່ດັ່ງນັ້ນ
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    ຖ້າ (truncated)
    {
	ຖ້າ (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ສົ່ງຄືນ col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    static ຈຳນວນເຕັມ
pum_process_item(
	ຈຳນວນເຕັມ     row,
	ຈຳນວນເຕັມ     col,
	ຈຳນວນເຕັມ     idx,
	ຈຳນວນເຕັມ     j,         // Current position in order array
	ຈຳນວນເຕັມ     *order,    // Order array
	hlf_T   hlf,
	ຈຳນວນເຕັມ     attr,
	ຈຳນວນເຕັມ     *totwidth_ptr,
	ຈຳນວນເຕັມ     next_isempty)
{
    ຈຳນວນເຕັມ     item_type = order[j];
    char_u  *s = NULL;
    char_u  *p = pum_get_item(idx, item_type);
    ຈຳນວນເຕັມ     width = 0;  // item width
    ຈຳນວນເຕັມ     w;		// char width
    ຈຳນວນເຕັມ	    selected = idx == pum_selected;

    ສຳລັບ ( ; ; MB_PTR_ADV(p))
    {
	ຖ້າ (s == NULL)
	    s = p;
	w = ptr2cells(p);
	ຖ້າ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    continue;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	ຈຳນວນເຕັມ     *attrs = NULL;
	ຈຳນວນເຕັມ     saved = *p;

	ຖ້າ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ຖ້າ (saved != NUL)
	    *p = saved;

	ຖ້າ (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	ບໍ່ດັ່ງນັ້ນ
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	ຖ້າ (attrs != NULL)
	    VIM_CLEAR(attrs);

	ຖ້າ (*p != TAB)
	    bha_nga;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	ບໍ່ດັ່ງນັ້ນ
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = NULL;  // start text at next char
	width = 0;
    }

    ສົ່ງຄືນ col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    static void
pum_draw_scrollbar(
	ຈຳນວນເຕັມ	row,
	ຈຳນວນເຕັມ	i,
	ຈຳນວນເຕັມ	thumb_pos,
	ຈຳນວນເຕັມ	thumb_height)
{
    ຖ້າ (pum_scrollbar <= 0)
	ສົ່ງຄືນ;

    ຈຳນວນເຕັມ attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    ບໍ່ດັ່ງນັ້ນ
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    static anta_hstha void
pum_align_order(ຈຳນວນເຕັມ *order)
{
    ຈຳນວນເຕັມ is_default = cia_flags == 0;
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

    ຖ້າ (pum_visible())
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
    ຈຳນວນເຕັມ k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_lines);
    VIM_CLEAR(pum_bg_linesUC);
    ສຳລັບ (k = 0; k < MAX_MCO; ++k)
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
    ຈຳນວນເຕັມ		row = pum_row;
    ຈຳນວນເຕັມ		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    ຈຳນວນເຕັມ		attr;
    ຈຳນວນເຕັມ		i, j;
    ຈຳນວນເຕັມ		idx;
    char_u	*p = NULL;
    ຈຳນວນເຕັມ		totwidth;
    ຈຳນວນເຕັມ		thumb_pos = 0;
    ຈຳນວນເຕັມ		thumb_height = 1;
    ຈຳນວນເຕັມ		item_type;
    ຈຳນວນເຕັມ		order[3];
    ຈຳນວນເຕັມ		next_isempty = FALSE;
    ຈຳນວນເຕັມ		n;
    ຈຳນວນເຕັມ		items_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    ຈຳນວນເຕັມ		basic_width;  // first item width
    ຈຳນວນເຕັມ		last_isabbr = FALSE;
    ຈຳນວນເຕັມ		orig_attr = -1;
    ຈຳນວນເຕັມ		scroll_range = pum_size - pum_height;
    bool	override_success;
    ຈຳນວນເຕັມ		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = push_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ຖ້າ (pum_imagep == NULL)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(NULL, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ຖ້າ (pum_imagep != NULL)
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

    ຖ້າ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ຖ້າ (opacity_active && pum_bg_lines != NULL
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	ຖ້າ (opacity_active && pum_bg_lines != NULL)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ຖ້າ (ScreenLines != NULL && ScreenAttrs != NULL)
	    {
		ຈຳນວນເຕັມ	save_top, save_bot, save_ncells, k;

		pum_free_bg();
		save_top = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ຖ້າ (save_top < 0)
		    save_top = 0;
		ຖ້າ (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_top = save_top;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		ຖ້າ (save_top < save_bot)
		{
		    save_ncells = (save_bot - save_top) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_lines = LALLOC_MULT(schar_T, save_ncells);
		    ຖ້າ (enc_utf8)
		    {
			pum_bg_linesUC = LALLOC_MULT(u8char_T, save_ncells);
			ສຳລັບ (k = 0; k < MAX_MCO; ++k)
			    pum_bg_linesC[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ຖ້າ (pum_bg_attrs != NULL && pum_bg_lines != NULL)
		    {
			ສຳລັບ (ຈຳນວນເຕັມ r = save_top; r < save_bot; ++r)
			{
			    ຈຳນວນເຕັມ soff = (r - save_top) * screen_Columns;
			    ຈຳນວນເຕັມ loff = LineOffset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * parimaa_na(sattr_T));
			    mch_memmove(pum_bg_lines + soff,
				    ScreenLines + loff,
				    screen_Columns * parimaa_na(schar_T));
			    ຖ້າ (enc_utf8 && pum_bg_linesUC != NULL
						&& ScreenLinesUC != NULL)
			    {
				mch_memmove(pum_bg_linesUC + soff,
					ScreenLinesUC + loff,
					screen_Columns * parimaa_na(u8char_T));
				ສຳລັບ (k = 0; k < MAX_MCO; ++k)
				    ຖ້າ (pum_bg_linesC[k] != NULL
						    && ScreenLinesC[k] != NULL)
					mch_memmove(pum_bg_linesC[k] + soff,
						ScreenLinesC[k] + loff,
						screen_Columns
						    * parimaa_na(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
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

    ຖ້າ (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	ຖ້າ (thumb_height == 0)
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
    ຖ້າ (pum_border)
	pum_draw_border();
    ຖ້າ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ຖ້າ (opacity_active)
	screen_pum_blend = 100 - (ຈຳນວນເຕັມ)p_po;

    ສຳລັບ (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	{
	    ຖ້າ (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	ບໍ່ດັ່ງນັ້ນ
#endif
	    ຖ້າ (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = items_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	ສຳລັບ (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ຖ້າ (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    ຖ້າ (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == NULL;
	    ບໍ່ດັ່ງນັ້ນ
		next_isempty = TRUE;

	    ຖ້າ (p != NULL)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    ຖ້າ (j > 0)
		n = items_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    ບໍ່ດັ່ງນັ້ນ
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ຖ້າ (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == NULL)))
		    || basic_width + n >= pum_width)
		bha_nga;
#ifdef FEAT_RIGHTLEFT
	    ຖ້າ (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    ບໍ່ດັ່ງນັ້ນ
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	ບໍ່ດັ່ງນັ້ນ
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

    ຖ້າ (override_success)
	pop_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    void
pum_position_info_popup(win_T *wp)
{
    ຈຳນວນເຕັມ col = pum_col + pum_width + pum_scrollbar + 1;
    ຈຳນວນເຕັມ row = pum_row;
    ຈຳນວນເຕັມ botpos = POPPOS_BOTLEFT;
    ຈຳນວນເຕັມ	used_maxwidth_opt = FALSE;
    ຈຳນວນເຕັມ	right_margin, left_margin, dummy;
    ຈຳນວນເຕັມ left_extra_width, right_extra_width;

    (void)compute_margins(&right_margin, &left_margin, &dummy);

    left_extra_width = pum_border + left_margin;
    right_extra_width = pum_border + right_margin + (pum_shadow ? 2 : 0);
    col += right_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ຖ້າ (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - left_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - left_extra_width;
    }
    ບໍ່ດັ່ງນັ້ນ
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    ຖ້າ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_top_extra(wp);
    ຖ້າ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ຖ້າ (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	ບໍ່ດັ່ງນັ້ນ
	    // menu below cursor line, align with top
	    row += 1;
    }
    ບໍ່ດັ່ງນັ້ນ
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ຖ້າ (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ບໍ່ດັ່ງນັ້ນ
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
    static ຈຳນວນເຕັມ
pum_set_selected(ຈຳນວນເຕັມ n, ຈຳນວນເຕັມ repeat UNUSED)
{
    ຈຳນວນເຕັມ	    resized = FALSE;
    ຈຳນວນເຕັມ	    context = pum_height / 2;
    ຈຳນວນເຕັມ	    scroll_offset;
#ifdef FEAT_QUICKFIX
    ຈຳນວນເຕັມ	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ຈຳນວນເຕັມ	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    ຖ້າ (pum_selected >= 0 && pum_selected < pum_size)
    {
	ຖ້າ (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ຖ້າ (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		ຖ້າ (pum_first < 0)
		    pum_first = 0;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    ບໍ່ດັ່ງນັ້ນ
		pum_first = pum_selected;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ຖ້າ (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    ບໍ່ດັ່ງນັ້ນ
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ຖ້າ (pum_height > 2)
	{
	    ຖ້າ (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#ຖ້າ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ຖ້າ (pum_array[pum_selected].pum_info != NULL
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ບໍ່ດັ່ງນັ້ນ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ຖ້າ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    ບໍ່ດັ່ງນັ້ນ
		use_popup = USEPOPUP_NONE;
	    ຖ້າ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ຖ້າ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ຖ້າ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ຖ້າ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		ຈຳນວນເຕັມ	res = OK;

		ຖ້າ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == NULL
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ໃນຂະນະທີ່ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, 0, NULL);
		    --no_u_sync;
		    ຖ້າ (res == OK)
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
		ຖ້າ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    ສຳລັບ (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ຖ້າ (e == NULL)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    bha_nga;
			}
			*e = NUL;
			ml_append(lnum++, p, (ຈຳນວນເຕັມ)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_line_count);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ຖ້າ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ຖ້າ (curwin->w_height < lnum)
			{
			    win_setheight((ຈຳນວນເຕັມ)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    ຖ້າ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (curwin->w_topline > curbuf->b_ml.ml_line_count)
			curwin->w_topline = curbuf->b_ml.ml_line_count;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    ຖ້າ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ຖ້າ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ຖ້າ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			ຈຳນວນເຕັມ save_redr_status;

			ຖ້າ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ຖ້າ (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ຖ້າ (resized && win_valid(curwin_save))
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

			ຖ້າ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ຖ້າ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ຖ້າ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ຖ້າ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ຖ້າ (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    ສົ່ງຄືນ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    void
pum_undisplay(void)
{
#ifdef FEAT_IMAGE_POPUP
    ຖ້າ (pum_imagep != NULL)
    {
	image_placement_free(pum_imagep);
	pum_imagep = NULL;
    }
#endif
    pum_free_bg();
    pum_array = NULL;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ຖ້າ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ຖ້າ (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
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
    ຈຳນວນເຕັມ
pum_visible(void)
{
    ສົ່ງຄືນ !pum_pretend_not_visible && pum_array != NULL;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    static ຈຳນວນເຕັມ
pum_in_same_position(void)
{
    ຈຳນວນເຕັມ	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    ສົ່ງຄືນ pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    ຈຳນວນເຕັມ
pum_redraw_in_same_position(void)
{
    ຖ້າ (!pum_visible() || pum_will_redraw)
	ສົ່ງຄືນ FALSE;  // nothing to do

    ສົ່ງຄືນ pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    void
pum_may_redraw(void)
{
    pumitem_T	*array = pum_array;
    ຈຳນວນເຕັມ		len = pum_size;
    ຈຳນວນເຕັມ		selected = pum_selected;

    ຖ້າ (!pum_visible() || pum_will_redraw)
	ສົ່ງຄືນ;  // nothing to do

    ຖ້າ (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ບໍ່ດັ່ງນັ້ນ
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
    ຈຳນວນເຕັມ
pum_get_height(void)
{
    ສົ່ງຄືນ pum_height;
}

#ຖ້າ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    void
pum_set_event_info(dict_T *dict)
{
    ຖ້າ (!pum_visible())
	ສົ່ງຄືນ;
    (void)dict_add_number(dict, "height", pum_height);
    (void)dict_add_number(dict, "width", pum_width);
    (void)dict_add_number(dict, "row", pum_row);
    (void)dict_add_number(dict, "col", pum_col);
    (void)dict_add_number(dict, "size", pum_size);
    (void)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#ຖ້າ defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    static void
pum_position_at_mouse(ຈຳນວນເຕັມ min_width)
{
    ຈຳນວນເຕັມ extra_height = pum_border * 2 + pum_shadow;
    ຈຳນວນເຕັມ extra_offset = pum_border + pum_shadow;
    ຈຳນວນເຕັມ trailing_overhead = 0;
    ຈຳນວນເຕັມ col_offset = 0;
    ຈຳນວນເຕັມ available_width;

    ຖ້າ (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	ຖ້າ (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	ຖ້າ (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	ຖ້າ (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	ຖ້າ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	ບໍ່ດັ່ງນັ້ນ
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    ບໍ່ດັ່ງນັ້ນ
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	ຖ້າ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	ບໍ່ດັ່ງນັ້ນ
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

#ຖ້າ defined(FEAT_BEVAL_TERM)
static pumitem_T *balloon_array = NULL;
static ຈຳນວນເຕັມ balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

typedef sa_nracanaa {
    char_u	*start;
    ຈຳນວນເຕັມ		bytelen;
    ຈຳນວນເຕັມ		cells;
    ຈຳນວນເຕັມ		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    ຈຳນວນເຕັມ
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    ຈຳນວນເຕັມ		quoted = FALSE;
    ຈຳນວນເຕັມ		height;
    ຈຳນວນເຕັມ		line;
    ຈຳນວນເຕັມ		item_idx;
    ຈຳນວນເຕັມ		indent = 0;
    ຈຳນວນເຕັມ		max_cells = 0;
    ຈຳນວນເຕັມ		max_height = Rows / 2 - 1;
    ຈຳນວນເຕັມ		long_item_count = 0;
    ຈຳນວນເຕັມ		split_long_items = FALSE;

    ga_init2(&ga, parimaa_na(balpart_T), 20);
    p = mesg;

    ໃນຂະນະທີ່ (*p != NUL)
    {
	ຖ້າ (ga_grow(&ga, 1) == FAIL)
	    goto failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	ໃນຂະນະທີ່ (*p != NUL)
	{
	    ຖ້າ (*p == '"')
		quoted = !quoted;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '\n')
		bha_nga;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '\\' && p[1] != NUL)
		++p;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!quoted)
	    {
		ຖ້າ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ຖ້າ (*p == '{')
			++indent;
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    bha_nga;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	ຖ້າ (*p == '\n')
	    ++p;
	ຖ້າ (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_count += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ຖ້າ (long_item_count > 0 && height + long_item_count <= max_height)
    {
	split_long_items = TRUE;
	height += long_item_count;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ຖ້າ (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    ຖ້າ (*array == NULL)
	goto failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    ສຳລັບ (line = 1, item_idx = 0; line < height - 1; ++item_idx)
    {
	ຈຳນວນເຕັມ	skip;
	ຈຳນວນເຕັມ	thislen;
	ຈຳນວນເຕັມ	copylen;
	ຈຳນວນເຕັມ	ind;
	ຈຳນວນເຕັມ	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	ຖ້າ (item->bytelen == 0)
	    (*array)[line++].pum_text = vim_strsave((char_u *)"");
	ບໍ່ດັ່ງນັ້ນ
	    ສຳລັບ (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		ຖ້າ (split_long_items && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    ສຳລັບ (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			ຖ້າ ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    bha_nga;
		    thislen = p - (item->start + skip);
		}
		ບໍ່ດັ່ງນັ້ນ
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		ຖ້າ (p == NULL)
		{
		    ສຳລັບ (line = 0; line <= height - 1; ++line)
			vim_free((*array)[line].pum_text);
		    vim_free(*array);
		    goto failed;
		}
		ສຳລັບ (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		ສຳລັບ (copylen = thislen; copylen > 0; --copylen)
		    ຖ້າ (item->start[skip + copylen - 1] != ' ')
			bha_nga;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[line].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++line;
	    }
    }
    ga_clear(&ga);
    ສົ່ງຄືນ height;

failed:
    ga_clear(&ga);
    ສົ່ງຄືນ 0;
}

    void
ui_remove_balloon(void)
{
    ຖ້າ (balloon_array == NULL)
	ສົ່ງຄືນ;

    pum_undisplay();
    ໃນຂະນະທີ່ (balloon_arraysize > 0)
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

    ຖ້າ (mesg == NULL && list == NULL)
    {
	pum_undisplay();
	ສົ່ງຄືນ;
    }
    ຖ້າ (list != NULL)
    {
	listitem_T  *li;
	ຈຳນວນເຕັມ	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ຖ້າ (balloon_array == NULL)
	    ສົ່ງຄືນ;
	CHECK_LIST_MATERIALIZE(list);
	ສຳລັບ (idx = 0, li = list->lv_first; li != NULL; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == NULL ? (char_u *)"" : text);
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	balloon_arraysize = split_message(mesg, &balloon_array);

    ຖ້າ (balloon_arraysize <= 0)
	ສົ່ງຄືນ;

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

#ຖ້າ defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    static void
pum_select_mouse_pos(void)
{
    ຈຳນວນເຕັມ idx = mouse_row - pum_row;

    ຖ້າ (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    static void
pum_execute_menu(vimmenu_T *menu, ຈຳນວນເຕັມ mode)
{
    vimmenu_T   *mp;
    ຈຳນວນເຕັມ		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	ຖ້າ ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    bha_nga;
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
    ຈຳນວນເຕັມ		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    ຈຳນວນເຕັມ		save_bevalterm = p_bevalterm;
# endif
    ຈຳນວນເຕັມ		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	ຖ້າ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ຖ້າ (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	ສົ່ງຄືນ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    ຖ້າ (array == NULL)
	ສົ່ງຄືນ;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = NULL;

	// Make a copy of the text, the menu may be redefined in a callback.
	ຖ້າ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	ຖ້າ (s != NULL)
	{
	    s = vim_strsave(s);
	    ຖ້າ (s != NULL)
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

    ສຳລັບ (;;)
    {
	ຈຳນວນເຕັມ	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ຖ້າ (c == ESC || c == Ctrl_C || pum_array == NULL)
	    bha_nga;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    bha_nga;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ໃນຂະນະທີ່ (pum_selected > 0)
	    {
		--pum_selected;
		ຖ້າ (*array[pum_selected].pum_text != NUL)
		    bha_nga;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ໃນຂະນະທີ່ (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		ຖ້າ (*array[pum_selected].pum_text != NUL)
		    bha_nga;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    bha_nga;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ຖ້າ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		bha_nga;
	    }
	    ຖ້າ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		bha_nga;
	}
    }

    ສຳລັບ (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    void
pum_make_popup(char_u *path_name, ຈຳນວນເຕັມ use_mouse_pos)
{
    vimmenu_T *menu;

    ຖ້າ (!use_mouse_pos)
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
    ຖ້າ (menu != NULL)
	pum_show_popupmenu(menu);
}
#endif

    static ຈຳນວນເຕັມ
is_singlewidth(ຈຳນວນເຕັມ c)
{
    ຖ້າ (c >= 0 && c < 0x80)
	ສົ່ງຄືນ TRUE;
    ຖ້າ (utf_char2cells(c) == 1)
	ສົ່ງຄືນ TRUE;
    ສົ່ງຄືນ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    void
pum_set_border_chars(
	ຈຳນວນເຕັມ top,
	ຈຳນວນເຕັມ right,
	ຈຳນວນເຕັມ bottom,
	ຈຳນວນເຕັມ left,
	ຈຳນວນເຕັມ top_left,
	ຈຳນວນເຕັມ top_right,
	ຈຳນວນເຕັມ bottom_right,
	ຈຳນວນເຕັມ bottom_left)
{
    ຖ້າ (!is_singlewidth(top) || !is_singlewidth(right)
	    || !is_singlewidth(bottom) || !is_singlewidth(left)
	    || !is_singlewidth(top_left) || !is_singlewidth(top_right)
	    || !is_singlewidth(bottom_right) || !is_singlewidth(bottom_left))
	ສົ່ງຄືນ;
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
compute_margins(ຈຳນວນເຕັມ *right_margin, ຈຳນວນເຕັມ *left_margin, ຈຳນວນເຕັມ *left_padding)
{
    *right_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	*left_padding = (pum_col < Columns - 1 - pum_border);
	*left_margin = *right_margin && (pum_col < Columns - 3);
	ສົ່ງຄືນ;
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
    ຈຳນວນເຕັມ	top_row = pum_row - 1;
    ຈຳນວນເຕັມ	bottom_row = pum_row + pum_height;
    ຈຳນວນເຕັມ	col;
    ຈຳນວນເຕັມ	attr = highlight_attr[HLF_PMB];
    ຈຳນວນເຕັມ	right_margin, left_margin, left_padding;
    ຈຳນວນເຕັມ	inner_width;

    ຖ້າ (!pum_border)
	ສົ່ງຄືນ;

    (void)compute_margins(&right_margin, &left_margin, &left_padding);

    inner_width = pum_width + pum_scrollbar + left_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
	col = pum_col + left_padding + 1;
    ບໍ່ດັ່ງນັ້ນ
#endif
	col = pum_col - left_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	screen_putchar(pum_border_chars.top_right, top_row, col, attr);
	ຖ້າ (left_margin)
	    screen_putchar(' ', top_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_right, bottom_row, col, attr);
	ຖ້າ (left_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
	screen_putchar(pum_border_chars.top_left, top_row, col, attr);
	ຖ້າ (left_margin)
	    screen_putchar(' ', top_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_left, bottom_row, col, attr);
	ຖ້າ (left_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < inner_width; i++)
    {
	ຈຳນວນເຕັມ cur_col;
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    cur_col = col - 1 - i;
	ບໍ່ດັ່ງນັ້ນ
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.top, top_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	screen_putchar(pum_border_chars.top_left, top_row,
		col - inner_width - 1, attr);
	ຖ້າ (right_margin)
	    screen_putchar(' ', top_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_left, bottom_row,
		col - inner_width - 1, attr);
	ຖ້າ (right_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
	screen_putchar(pum_border_chars.top_right, top_row,
		col + inner_width + 1, attr);
	ຖ້າ (right_margin)
	    screen_putchar(' ', top_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_right, bottom_row,
		col + inner_width + 1, attr);
	ຖ້າ (right_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pum_height; i++)
    {
	ຈຳນວນເຕັມ row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	{
	    screen_putchar(pum_border_chars.left, row, col, attr);
	    ຖ້າ (left_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.right, row,
		    col - inner_width - 1, attr);
	    ຖ້າ (right_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	ບໍ່ດັ່ງນັ້ນ
#endif
	{
	    screen_putchar(pum_border_chars.left, row, col, attr);
	    ຖ້າ (left_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.right, row,
		    col + inner_width + 1, attr);
	    ຖ້າ (right_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    void
put_shadow_char(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col)
{
    char_u  buf[MB_MAXBYTES + 1];
    ຈຳນວນເຕັມ	    shadow_attr = highlight_attr[HLF_PMS];
    ຈຳນວນເຕັມ	    char_attr, new_attr;

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
    ຈຳນວນເຕັມ	row, col, next_col;
    ຈຳນວນເຕັມ	width, offset;
    ຈຳນວນເຕັມ	inner_width;
    ຈຳນວນເຕັມ	right_margin, left_margin, left_padding;

    ຖ້າ (!pum_shadow)
	ສົ່ງຄືນ;

    (void)compute_margins(&right_margin, &left_margin, &left_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + right_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    ຖ້າ (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + left_padding + pum_border + left_margin;
    offset = 2 - left_padding - pum_border - left_margin;

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	ຖ້າ (pum_rl)
	    col = pum_col - offset - i;
	ບໍ່ດັ່ງນັ້ນ
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
