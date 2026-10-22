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
#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର pumitem_T *pum_array = ଖାଲି;	// items of displayed pum
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_size;			// nr of items in "pum_array"
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_selected;		// index of selected item or -1
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_first = 0;		// index of top item

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା call_update_screen = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_in_cmdline = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_height;			// nr of displayed pum items
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_width;			// width of displayed pum items
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_base_width;		// width of pum items base
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_kind_width;		// width of pum items kind column
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_extra_width;		// width of extra stuff
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_row;			// top row of pum
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_col;			// left column of pum

ସ୍ଥିର win_T *pum_window = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_win_row;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_win_height;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_win_col;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_win_wcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_pretend_not_visible = FALSE;

// Border support
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_border = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_margin = 0;	// margin of 1 cell on left and right
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
ସ୍ଥିର image_placement_T *pum_imagep = ଖାଲି;
#endif

// Border characters
ସ୍ଥିର ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_ବାମ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom_ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom_ବାମ;
} pum_border_chars;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pum_set_selected(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeat);
ସ୍ଥିର ଶୂନ୍ୟ pum_draw_border(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ pum_draw_shadow(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ compute_margins(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଡାହାଣ_margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ବାମ_margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ବାମ_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	ଶୂନ୍ୟ \
    pum_set_##feature(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ସ୍ଥିର ଶୂନ୍ୟ
pum_compute_size(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pum_size; ++i)
    {
	ଯଦି (pum_array[i].pum_text != ଖାଲି)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    ଯଦି (pum_base_width < w)
		pum_base_width = w;
	}
	ଯଦି (pum_array[i].pum_kind != ଖାଲି)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    ଯଦି (pum_kind_width < w)
		pum_kind_width = w;
	}
	ଯଦି (pum_array[i].pum_extra != ଖାଲି)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ଯଦି (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_compute_vertical_placement(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା above_row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା below_row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା context_ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cline_visible_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_height = 2 * pum_border + pum_shadow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    ଯଦି (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ଯଦି (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	ଯଦି (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
	ଅନ୍ୟଥା
	    // Leave two lines of context if possible
	    context_ଧାଡ଼ିଗୁଡ଼ିକ = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	ଯଦି (pum_win_row >= size + context_ଧାଡ଼ିଗୁଡ଼ିକ + extra_height)
	{
	    pum_row = pum_win_row - size - context_ଧାଡ଼ିଗୁଡ଼ିକ - extra_offset;
	    pum_height = size;
	}
	ଅନ୍ୟଥା
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_ଧାଡ଼ିଗୁଡ଼ିକ - extra_height;
	}
	ଯଦି (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    ଅନ୍ୟଥା
    {
	// pum below "pum_win_row"
	ଯଦି (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
	ଅନ୍ୟଥା
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_ଧାଡ଼ିଗୁଡ଼ିକ = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_ଧାଡ଼ିଗୁଡ଼ିକ + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	ଯଦି (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#ଯଦି defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ଯଦି (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_pum_width_aligned_with_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା available_width)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_padding = TRUE;

    ଯଦି (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    ଯଦି (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    ଫେରନ୍ତୁ available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_compute_horizontal_placement(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cursor_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା available_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    ଅନ୍ୟଥା
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    ଯଦି (set_pum_width_aligned_with_cursor(desired_width, available_width))
	ଫେରନ୍ତୁ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ଯଦି (available_width > p_pw)
    {
	pum_width = available_width;
	ଫେରନ୍ତୁ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    ଯଦି (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	ଅନ୍ୟଥା
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	ଫେରନ୍ତୁ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
	pum_col = Columns - 1 - col_offset;
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
pum_display(
	pumitem_T   *array,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    size,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    selected,   // index of initially selected item, -1 if
				// out of range
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cursor_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    above_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    below_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    redo_ଗଣନା = 0;
#ଯଦି defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    କର
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = ଖାଲି;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	ଯଦି (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	ଅନ୍ୟଥା
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#ଯଦି defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ଯଦି (pvwin->w_p_pvw)
		ଭାଙ୍ଗ;
	ଯଦି (pvwin != ଖାଲି)
	{
	    ଯଦି (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    ଅନ୍ୟଥା ଯଦି (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	ଯଦି (pum_height < 1 || (pum_height == 1 && size > 1))
	    ଫେରନ୍ତୁ;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	ଯଦି (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ଯଦି (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    ଅନ୍ୟଥା
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ଯଦି (curwin->w_p_cole > 0 && conceal_cursor_ଧାଡ଼ି(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ଯଦି (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    ଅନ୍ୟଥା
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
    } ଯେତେବେଳେ (pum_set_selected(selected, redo_ଗଣନା) && ++redo_ଗଣନା <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    ଶୂନ୍ୟ
pum_call_update_screen(ଶୂନ୍ୟ)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_under_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା only_redrawing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_ବାମ = pum_border + (pum_margin && pum_border ? 1 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_ଡାହାଣ = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_above = pum_border;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_below = pum_border + (pum_shadow ? 1 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଶୀର୍ଷ = pum_row - extra_above;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bot = pum_row + pum_height + extra_below;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ବାମ = pum_col - 1 - extra_ବାମ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଡାହାଣ = pum_col + pum_width + pum_scrollbar + extra_ଡାହାଣ;

    ଯଦି (!((!only_redrawing || pum_will_redraw)
	    && row >= ଶୀର୍ଷ && row < bot && col >= ବାମ && col < ଡାହାଣ))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ଯଦି (only_redrawing)
	{
	    ଯଦି (col >= ଡାହାଣ - 2 || row == bot - 1)
		ଫେରନ୍ତୁ FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଡାହାଣ_margin, ବାମ_margin, ବାମ_padding;

	    compute_margins(&ଡାହାଣ_margin, &ବାମ_margin, &ବାମ_padding);
	    ଯଦି (row == ଶୀର୍ଷ && col >= ଡାହାଣ - 2)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (row == bot - 1 && col < pum_col + 2 - ବାମ_padding - pum_border
		    - ବାମ_margin)
		ଫେରନ୍ତୁ FALSE;
	}
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
pum_compute_text_attrs(char_u *text, hlf_T hlf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_hlattr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    size_t	leader_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		char_cells;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_attr;
    char_u	*ptr = text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cell_idx = 0;
    garray_T	*ga = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*attrs = ଖାଲି;
    char_u	*leader = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_fuzzy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matched_len = -1;
    int_u	char_pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_select = FALSE;

    ଯଦି (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ଫେରନ୍ତୁ ଖାଲି;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    ଯଦି (leader == ଖାଲି || *leader == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    attrs = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, vim_strsize(text));
    ଯଦି (attrs == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ଯଦି (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	ଯଦି (!ga)
	{
	    vim_free(attrs);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    ଯେତେବେଳେ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ଯଦି (ga != ଖାଲି)
	{
	    // Handle fuzzy matching
	    ପାଇଁ (i = 0; i < ga->ga_len; i++)
	    {
		ଯଦି (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)leader_len;
	    ଯଦି (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ଯଦି (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	ପାଇଁ (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ଯଦି (ga != ଖାଲି)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ଫେରନ୍ତୁ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_screen_puts_with_attrs(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cells UNUSED,
    char_u	*text,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		textlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*attrs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_start = col;
    char_u	*ptr = text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		char_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;

    // Render text with proper attributes
    ଯେତେବେଳେ (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	ଅନ୍ୟଥା
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ସ୍ଥିର ଧାଡ଼ିଭିତରେ char_u *
pum_get_item(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ବାଛ(type)
    {
	ସ୍ଥିତି CPT_ABBR: ଫେରନ୍ତୁ pum_array[index].pum_text;
	ସ୍ଥିତି CPT_KIND: ଫେରନ୍ତୁ pum_array[index].pum_kind;
	ସ୍ଥିତି CPT_MENU: ଫେରନ୍ତୁ pum_array[index].pum_extra;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_user_attr_combine(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    ଫେରନ୍ତୁ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_display_rtl_text(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     col,
	char_u  *st,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     attr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     *attrs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width_limit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     totwidth,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     next_isempty,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	selected)
{
    char_u  *rt = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     cells = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     over_cell = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     truncated = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     pad = next_isempty ? 0 : 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     remaining = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ଯଦି (st == ଖାଲି)
	ଫେରନ୍ତୁ col;

    rt = reverse_text(st);
    ଯଦି (rt == ଖାଲି)
    {
	VIM_CLEAR(st);
	ଫେରନ୍ତୁ col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ଯଦି (cells > width_limit)
    {
	କର
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ଯେତେବେଳେ (cells > width_limit);

	ଯଦି (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ଯଦି (truncated)
    {
	char_u  *orig_rt = rt;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ଯଦି (cells > remaining)
	{
	    ଯେତେବେଳେ (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(orig_rt);
	ଯଦି (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	ଯଦି (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    ଯଦି (attrs == ଖାଲି)
	screen_puts_len(rt, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(rt), row, col - cells + 1, attr);
    ଅନ୍ୟଥା
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    ଫେରନ୍ତୁ col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_display_ltr_text(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     col,
	char_u  *st,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     attr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     *attrs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width,        // width already calculated in outer loop
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width_limit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     totwidth,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     next_isempty,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	selected)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     cells = 0;
    char_u  *st_end = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     over_cell = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     pad = next_isempty ? 0 : 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     truncated = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     remaining = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ଯଦି (st == ଖାଲି)
	ଫେରନ୍ତୁ col;

    size = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ଯେତେବେଳେ (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	ଯଦି (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	ଅନ୍ୟଥା
	    --cells;
    }

    // truncated
    ଯଦି (truncated)
    {
	remaining = width_limit - totwidth - 1;
	ଯଦି (cells > remaining)
	{
	    st_end = st + size;
	    ଯେତେବେଳେ (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	ଯଦି (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    ଯଦି (attrs == ଖାଲି)
	screen_puts_len(st, size, row, col, attr);
    ଅନ୍ୟଥା
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    ଯଦି (truncated)
    {
	ଯଦି (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ଫେରନ୍ତୁ col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_process_item(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     j,         // Current position in order array
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     *order,    // Order array
	hlf_T   hlf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     attr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     *totwidth_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     next_isempty)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     item_type = order[j];
    char_u  *s = ଖାଲି;
    char_u  *p = pum_get_item(idx, item_type);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width = 0;  // item width
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     w;		// char width
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    selected = idx == pum_selected;

    ପାଇଁ ( ; ; MB_PTR_ADV(p))
    {
	ଯଦି (s == ଖାଲି)
	    s = p;
	w = ptr2cells(p);
	ଯଦି (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    ଜାରି;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     *attrs = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     saved = *p;

	ଯଦି (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ଯଦି (saved != NUL)
	    *p = saved;

	ଯଦି (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	ଅନ୍ୟଥା
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	ଯଦି (attrs != ଖାଲି)
	    VIM_CLEAR(attrs);

	ଯଦି (*p != TAB)
	    ଭାଙ୍ଗ;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	ଅନ୍ୟଥା
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = ଖାଲି;  // start text at next char
	width = 0;
    }

    ଫେରନ୍ତୁ col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_draw_scrollbar(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	thumb_pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	thumb_height)
{
    ଯଦି (pum_scrollbar <= 0)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    ଅନ୍ୟଥା
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
pum_align_order(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *order)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ସ୍ଥିର ଶୂନ୍ୟ pum_free_bg(ଶୂନ୍ୟ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    ଶୂନ୍ୟ
pum_opacity_changed(ଶୂନ୍ୟ)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    ଯଦି (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
pum_free_bg(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ);
    VIM_CLEAR(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC);
    ପାଇଁ (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k]);
    pum_bg_ଶୀର୍ଷ = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    ଶୂନ୍ୟ
pum_redraw(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = pum_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    char_u	*p = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		totwidth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thumb_pos = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		thumb_height = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		order[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_isempty = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ବସ୍ତୁଗୁଡ଼ିକ_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		basic_width;  // first item width
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_isabbr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_attr = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scroll_range = pum_size - pum_height;
    bool	override_success;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ଠେଲ_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ଯଦି (pum_imagep == ଖାଲି)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ଖାଲି, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ଯଦି (pum_imagep != ଖାଲି)
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

    ଯଦି (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ଯଦି (opacity_active && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	ଯଦି (opacity_active && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି && ScreenAttrs != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_ଶୀର୍ଷ, save_bot, save_ncells, k;

		pum_free_bg();
		save_ଶୀର୍ଷ = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ଯଦି (save_ଶୀର୍ଷ < 0)
		    save_ଶୀର୍ଷ = 0;
		ଯଦି (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_ଶୀର୍ଷ = save_ଶୀର୍ଷ;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		ଯଦି (save_ଶୀର୍ଷ < save_bot)
		{
		    save_ncells = (save_bot - save_ଶୀର୍ଷ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ = LALLOC_MULT(schar_T, save_ncells);
		    ଯଦି (enc_utf8)
		    {
			pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC = LALLOC_MULT(u8char_T, save_ncells);
			ପାଇଁ (k = 0; k < MAX_MCO; ++k)
			    pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ଯଦି (pum_bg_attrs != ଖାଲି && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
		    {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = save_ଶୀର୍ଷ; r < save_bot; ++r)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା soff = (r - save_ଶୀର୍ଷ) * screen_Columns;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା loff = ଧାଡ଼ି_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * ଆକାର(sattr_T));
			    mch_memmove(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ + soff,
				    Screen_ଧାଡ଼ିଗୁଡ଼ିକ + loff,
				    screen_Columns * ଆକାର(schar_T));
			    ଯଦି (enc_utf8 && pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି
						&& Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC != ଖାଲି)
			    {
				mch_memmove(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_UC + soff,
					Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC + loff,
					screen_Columns * ଆକାର(u8char_T));
				ପାଇଁ (k = 0; k < MAX_MCO; ++k)
				    ଯଦି (pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] != ଖାଲି
						    && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] != ଖାଲି)
					mch_memmove(pum_bg_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] + soff,
						Screen_ଧାଡ଼ିଗୁଡ଼ିକ_C[k] + loff,
						screen_Columns
						    * ଆକାର(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ଅନ୍ୟଥା
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

    ଯଦି (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	ଯଦି (thumb_height == 0)
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
    ଯଦି (pum_border)
	pum_draw_border();
    ଯଦି (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ଯଦି (opacity_active)
	screen_pum_blend = 100 - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_po;

    ପାଇଁ (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	{
	    ଯଦି (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	ଅନ୍ୟଥା
#endif
	    ଯଦି (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = ବସ୍ତୁଗୁଡ଼ିକ_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	ପାଇଁ (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ଯଦି (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    ଯଦି (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == ଖାଲି;
	    ଅନ୍ୟଥା
		next_isempty = TRUE;

	    ଯଦି (p != ଖାଲି)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    ଯଦି (j > 0)
		n = ବସ୍ତୁଗୁଡ଼ିକ_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    ଅନ୍ୟଥା
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ଯଦି (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == ଖାଲି)))
		    || basic_width + n >= pum_width)
		ଭାଙ୍ଗ;
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	ଅନ୍ୟଥା
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

    ଯଦି (override_success)
	କାଢ଼_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    ଶୂନ୍ୟ
pum_position_info_popup(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = pum_col + pum_width + pum_scrollbar + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = pum_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା botpos = POPPOS_BOTLEFT;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	used_maxwidth_opt = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଡାହାଣ_margin, ବାମ_margin, dummy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ_extra_width, ଡାହାଣ_extra_width;

    (ଶୂନ୍ୟ)compute_margins(&ଡାହାଣ_margin, &ବାମ_margin, &dummy);

    ବାମ_extra_width = pum_border + ବାମ_margin;
    ଡାହାଣ_extra_width = pum_border + ଡାହାଣ_margin + (pum_shadow ? 2 : 0);
    col += ଡାହାଣ_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ଯଦି (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - ବାମ_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - ବାମ_extra_width;
    }
    ଅନ୍ୟଥା
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    ଯଦି (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_ଶୀର୍ଷ_extra(wp);
    ଯଦି (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ଯଦି (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	ଅନ୍ୟଥା
	    // menu below cursor line, align with top
	    row += 1;
    }
    ଅନ୍ୟଥା
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ଯଦି (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_set_selected(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeat UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    resized = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    context = pum_height / 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scroll_offset;
#ifdef FEAT_QUICKFIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    ଯଦି (pum_selected >= 0 && pum_selected < pum_size)
    {
	ଯଦି (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ଯଦି (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		ଯଦି (pum_first < 0)
		    pum_first = 0;
		ଅନ୍ୟଥା ଯଦି (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    ଅନ୍ୟଥା
		pum_first = pum_selected;
	}
	ଅନ୍ୟଥା ଯଦି (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ଯଦି (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    ଅନ୍ୟଥା
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ଯଦି (pum_height > 2)
	{
	    ଯଦି (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    ଅନ୍ୟଥା ଯଦି (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#ଯଦି defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ଯଦି (pum_array[pum_selected].pum_info != ଖାଲି
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ଅନ୍ୟଥା
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ଯଦି (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ଅନ୍ୟଥା ଯଦି (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    ଅନ୍ୟଥା
		use_popup = USEPOPUP_NONE;
	    ଯଦି (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ଯଦି (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ଯଦି (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ଯଦି (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	res = OK;

		ଯଦି (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ଖାଲି
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ଯେତେବେଳେ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ଅନ୍ୟଥା
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_ONE, 0, ଖାଲି);
		    --no_u_sync;
		    ଯଦି (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_ମୂଲ୍ୟ_give_err((char_u *)"swf",
							  0L, ଖାଲି, OPT_LOCAL);
			set_option_ମୂଲ୍ୟ_give_err((char_u *)"bl",
							  0L, ଖାଲି, OPT_LOCAL);
			set_option_ମୂଲ୍ୟ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_ମୂଲ୍ୟ_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_ମୂଲ୍ୟ_give_err((char_u *)"diff",
							  0L, ଖାଲି, OPT_LOCAL);
		    }
		}
		ଯଦି (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    ପାଇଁ (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ଯଦି (e == ଖାଲି)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ଭାଙ୍ଗ;
			}
			*e = NUL;
			ml_append(lnum++, p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ଯଦି (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ଯଦି (curwin->w_height < lnum)
			{
			    win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    ଯଦି (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ଅନ୍ୟଥା ଯଦି (curwin->w_topline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			curwin->w_topline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    ଯଦି (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ଯଦି (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ଯଦି ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_redr_status;

			ଯଦି (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ଯଦି (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ଯଦି (resized && win_valid(curwin_save))
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

			ଯଦି (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ଯଦି (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ଯଦି (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ଯଦି (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ଯଦି (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    ଫେରନ୍ତୁ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    ଶୂନ୍ୟ
pum_undisplay(ଶୂନ୍ୟ)
{
#ifdef FEAT_IMAGE_POPUP
    ଯଦି (pum_imagep != ଖାଲି)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ଖାଲି;
    }
#endif
    pum_free_bg();
    pum_array = ଖାଲି;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ଯଦି (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    ଶୂନ୍ୟ
pum_clear(ଶୂନ୍ୟ)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_visible(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ !pum_pretend_not_visible && pum_array != ଖାଲି;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_in_same_position(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    ଫେରନ୍ତୁ pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_redraw_in_same_position(ଶୂନ୍ୟ)
{
    ଯଦି (!pum_visible() || pum_will_redraw)
	ଫେରନ୍ତୁ FALSE;  // nothing to do

    ଫେରନ୍ତୁ pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    ଶୂନ୍ୟ
pum_may_redraw(ଶୂନ୍ୟ)
{
    pumitem_T	*array = pum_array;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = pum_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		selected = pum_selected;

    ଯଦି (!pum_visible() || pum_will_redraw)
	ଫେରନ୍ତୁ;  // nothing to do

    ଯଦି (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pum_get_height(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ pum_height;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    ଶୂନ୍ୟ
pum_set_event_info(dict_T *dict)
{
    ଯଦି (!pum_visible())
	ଫେରନ୍ତୁ;
    (ଶୂନ୍ୟ)dict_add_ସଂଖ୍ୟା(dict, "height", pum_height);
    (ଶୂନ୍ୟ)dict_add_ସଂଖ୍ୟା(dict, "width", pum_width);
    (ଶୂନ୍ୟ)dict_add_ସଂଖ୍ୟା(dict, "row", pum_row);
    (ଶୂନ୍ୟ)dict_add_ସଂଖ୍ୟା(dict, "col", pum_col);
    (ଶୂନ୍ୟ)dict_add_ସଂଖ୍ୟା(dict, "size", pum_size);
    (ଶୂନ୍ୟ)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#ଯଦି defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    ସ୍ଥିର ଶୂନ୍ୟ
pum_position_at_mouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_width)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_height = pum_border * 2 + pum_shadow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_offset = pum_border + pum_shadow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା trailing_overhead = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_offset = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା available_width;

    ଯଦି (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	ଯଦି (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	ଯଦି (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	ଯଦି (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	ଯଦି (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	ଅନ୍ୟଥା
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    ଅନ୍ୟଥା
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	ଯଦି (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	ଅନ୍ୟଥା
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = ଖାଲି;
}

#endif

#ଯଦି defined(FEAT_BEVAL_TERM)
ସ୍ଥିର pumitem_T *balloon_array = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bytelen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cells;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quoted = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_cells = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_height = Rows / 2 - 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		long_item_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		split_long_ବସ୍ତୁଗୁଡ଼ିକ = FALSE;

    ga_init2(&ga, ଆକାର(balpart_T), 20);
    p = mesg;

    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (ga_grow(&ga, 1) == FAIL)
	    ଯାଅ failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	ଯେତେବେଳେ (*p != NUL)
	{
	    ଯଦି (*p == '"')
		quoted = !quoted;
	    ଅନ୍ୟଥା ଯଦି (*p == '\n')
		ଭାଙ୍ଗ;
	    ଅନ୍ୟଥା ଯଦି (*p == '\\' && p[1] != NUL)
		++p;
	    ଅନ୍ୟଥା ଯଦି (!quoted)
	    {
		ଯଦି ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ଯଦି (*p == '{')
			++indent;
		    ଅନ୍ୟଥା ଯଦି (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    ଭାଙ୍ଗ;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	ଯଦି (*p == '\n')
	    ++p;
	ଯଦି (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_ଗଣନା += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ଯଦି (long_item_ଗଣନା > 0 && height + long_item_ଗଣନା <= max_height)
    {
	split_long_ବସ୍ତୁଗୁଡ଼ିକ = TRUE;
	height += long_item_ଗଣନା;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ଯଦି (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    ଯଦି (*array == ଖାଲି)
	ଯାଅ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    ପାଇଁ (ଧାଡ଼ି = 1, item_idx = 0; ଧାଡ଼ି < height - 1; ++item_idx)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	skip;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	thislen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	copylen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ind;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	ଯଦି (item->bytelen == 0)
	    (*array)[ଧାଡ଼ି++].pum_text = vim_strsave((char_u *)"");
	ଅନ୍ୟଥା
	    ପାଇଁ (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		ଯଦି (split_long_ବସ୍ତୁଗୁଡ଼ିକ && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    ପାଇଁ (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			ଯଦି ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    ଭାଙ୍ଗ;
		    thislen = p - (item->start + skip);
		}
		ଅନ୍ୟଥା
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		ଯଦି (p == ଖାଲି)
		{
		    ପାଇଁ (ଧାଡ଼ି = 0; ଧାଡ଼ି <= height - 1; ++ଧାଡ଼ି)
			vim_free((*array)[ଧାଡ଼ି].pum_text);
		    vim_free(*array);
		    ଯାଅ failed;
		}
		ପାଇଁ (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		ପାଇଁ (copylen = thislen; copylen > 0; --copylen)
		    ଯଦି (item->start[skip + copylen - 1] != ' ')
			ଭାଙ୍ଗ;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[ଧାଡ଼ି].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++ଧାଡ଼ି;
	    }
    }
    ga_clear(&ga);
    ଫେରନ୍ତୁ height;

failed:
    ga_clear(&ga);
    ଫେରନ୍ତୁ 0;
}

    ଶୂନ୍ୟ
ui_remove_balloon(ଶୂନ୍ୟ)
{
    ଯଦି (balloon_array == ଖାଲି)
	ଫେରନ୍ତୁ;

    pum_undisplay();
    ଯେତେବେଳେ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    ଶୂନ୍ୟ
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    ଯଦି (mesg == ଖାଲି && list == ଖାଲି)
    {
	pum_undisplay();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (list != ଖାଲି)
    {
	listitem_T  *li;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ଯଦି (balloon_array == ଖାଲି)
	    ଫେରନ୍ତୁ;
	CHECK_LIST_MATERIALIZE(list);
	ପାଇଁ (idx = 0, li = list->lv_first; li != ଖାଲି; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == ଖାଲି ? (char_u *)"" : text);
	}
    }
    ଅନ୍ୟଥା
	balloon_arraysize = split_message(mesg, &balloon_array);

    ଯଦି (balloon_arraysize <= 0)
	ଫେରନ୍ତୁ;

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
    ଶୂନ୍ୟ
ui_may_remove_balloon(ଶୂନ୍ୟ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#ଯଦି defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_select_mouse_pos(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = mouse_row - pum_row;

    ଯଦି (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    ଅନ୍ୟଥା ଯଦି (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_execute_menu(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    vimmenu_T   *mp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	ଯଦି ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    ଭାଙ୍ଗ;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    ଶୂନ୍ୟ
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_bevalterm = p_bevalterm;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	ଯଦି (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ଯଦି (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	ଫେରନ୍ତୁ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    ଯଦି (array == ଖାଲି)
	ଫେରନ୍ତୁ;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = ଖାଲି;

	// Make a copy of the text, the menu may be redefined in a callback.
	ଯଦି (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ଅନ୍ୟଥା ଯଦି (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	ଯଦି (s != ଖାଲି)
	{
	    s = vim_strsave(s);
	    ଯଦି (s != ଖାଲି)
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

    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ଯଦି (c == ESC || c == Ctrl_C || pum_array == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା ଯଦି (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ଯେତେବେଳେ (pum_selected > 0)
	    {
		--pum_selected;
		ଯଦି (*array[pum_selected].pum_text != NUL)
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ଯେତେବେଳେ (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		ଯଦି (*array[pum_selected].pum_text != NUL)
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ଅନ୍ୟଥା ଯଦି (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ଯଦି (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ଭାଙ୍ଗ;
	}
    }

    ପାଇଁ (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    ଶୂନ୍ୟ
pum_make_popup(char_u *path_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_mouse_pos)
{
    vimmenu_T *menu;

    ଯଦି (!use_mouse_pos)
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
    ଯଦି (menu != ଖାଲି)
	pum_show_popupmenu(menu);
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_singlewidth(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c >= 0 && c < 0x80)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (utf_char2cells(c) == 1)
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    ଶୂନ୍ୟ
pum_set_border_chars(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_ବାମ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_ଡାହାଣ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom_ଡାହାଣ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bottom_ବାମ)
{
    ଯଦି (!is_singlewidth(ଶୀର୍ଷ) || !is_singlewidth(ଡାହାଣ)
	    || !is_singlewidth(bottom) || !is_singlewidth(ବାମ)
	    || !is_singlewidth(ଶୀର୍ଷ_ବାମ) || !is_singlewidth(ଶୀର୍ଷ_ଡାହାଣ)
	    || !is_singlewidth(bottom_ଡାହାଣ) || !is_singlewidth(bottom_ବାମ))
	ଫେରନ୍ତୁ;
    pum_border_chars.ଶୀର୍ଷ = ଶୀର୍ଷ;
    pum_border_chars.ଡାହାଣ = ଡାହାଣ;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ବାମ = ବାମ;
    pum_border_chars.ଶୀର୍ଷ_ବାମ = ଶୀର୍ଷ_ବାମ;
    pum_border_chars.ଶୀର୍ଷ_ଡାହାଣ = ଶୀର୍ଷ_ଡାହାଣ;
    pum_border_chars.bottom_ଡାହାଣ = bottom_ଡାହାଣ;
    pum_border_chars.bottom_ବାମ = bottom_ବାମ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compute_margins(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଡାହାଣ_margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ବାମ_margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ବାମ_padding)
{
    *ଡାହାଣ_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	*ବାମ_padding = (pum_col < Columns - 1 - pum_border);
	*ବାମ_margin = *ଡାହାଣ_margin && (pum_col < Columns - 3);
	ଫେରନ୍ତୁ;
    }
#endif
    *ବାମ_padding = (pum_col > pum_border);
    *ବାମ_margin = *ଡାହାଣ_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_draw_border(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଶୀର୍ଷ_row = pum_row - 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bottom_row = pum_row + pum_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	attr = highlight_attr[HLF_PMB];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଡାହାଣ_margin, ବାମ_margin, ବାମ_padding;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	inner_width;

    ଯଦି (!pum_border)
	ଫେରନ୍ତୁ;

    (ଶୂନ୍ୟ)compute_margins(&ଡାହାଣ_margin, &ବାମ_margin, &ବାମ_padding);

    inner_width = pum_width + pum_scrollbar + ବାମ_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
	col = pum_col + ବାମ_padding + 1;
    ଅନ୍ୟଥା
#endif
	col = pum_col - ବାମ_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	screen_putchar(pum_border_chars.ଶୀର୍ଷ_ଡାହାଣ, ଶୀର୍ଷ_row, col, attr);
	ଯଦି (ବାମ_margin)
	    screen_putchar(' ', ଶୀର୍ଷ_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_ଡାହାଣ, bottom_row, col, attr);
	ଯଦି (ବାମ_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    ଅନ୍ୟଥା
#endif
    {
	screen_putchar(pum_border_chars.ଶୀର୍ଷ_ବାମ, ଶୀର୍ଷ_row, col, attr);
	ଯଦି (ବାମ_margin)
	    screen_putchar(' ', ଶୀର୍ଷ_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_ବାମ, bottom_row, col, attr);
	ଯଦି (ବାମ_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < inner_width; i++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_col;
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    cur_col = col - 1 - i;
	ଅନ୍ୟଥା
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.ଶୀର୍ଷ, ଶୀର୍ଷ_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	screen_putchar(pum_border_chars.ଶୀର୍ଷ_ବାମ, ଶୀର୍ଷ_row,
		col - inner_width - 1, attr);
	ଯଦି (ଡାହାଣ_margin)
	    screen_putchar(' ', ଶୀର୍ଷ_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_ବାମ, bottom_row,
		col - inner_width - 1, attr);
	ଯଦି (ଡାହାଣ_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    ଅନ୍ୟଥା
#endif
    {
	screen_putchar(pum_border_chars.ଶୀର୍ଷ_ଡାହାଣ, ଶୀର୍ଷ_row,
		col + inner_width + 1, attr);
	ଯଦି (ଡାହାଣ_margin)
	    screen_putchar(' ', ଶୀର୍ଷ_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_ଡାହାଣ, bottom_row,
		col + inner_width + 1, attr);
	ଯଦି (ଡାହାଣ_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pum_height; i++)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	{
	    screen_putchar(pum_border_chars.ବାମ, row, col, attr);
	    ଯଦି (ବାମ_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.ଡାହାଣ, row,
		    col - inner_width - 1, attr);
	    ଯଦି (ଡାହାଣ_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	ଅନ୍ୟଥା
#endif
	{
	    screen_putchar(pum_border_chars.ବାମ, row, col, attr);
	    ଯଦି (ବାମ_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.ଡାହାଣ, row,
		    col + inner_width + 1, attr);
	    ଯଦି (ଡାହାଣ_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    ଶୂନ୍ୟ
put_shadow_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    char_u  buf[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    shadow_attr = highlight_attr[HLF_PMS];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    ସ୍ଥିର ଶୂନ୍ୟ
pum_draw_shadow(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row, col, next_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width, offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	inner_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଡାହାଣ_margin, ବାମ_margin, ବାମ_padding;

    ଯଦି (!pum_shadow)
	ଫେରନ୍ତୁ;

    (ଶୂନ୍ୟ)compute_margins(&ଡାହାଣ_margin, &ବାମ_margin, &ବାମ_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + ଡାହାଣ_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    ଯଦି (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    ଅନ୍ୟଥା
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + ବାମ_padding + pum_border + ବାମ_margin;
    offset = 2 - ବାମ_padding - pum_border - ବାମ_margin;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	ଯଦି (pum_rl)
	    col = pum_col - offset - i;
	ଅନ୍ୟଥା
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
