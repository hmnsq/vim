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
#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ pumitem_T *pum_array = ರಿಕ್ತ;	// items of displayed pum
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_size;			// nr of items in "pum_array"
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_selected;		// index of selected item or -1
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_first = 0;		// index of top item

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ call_update_screen = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_in_cmdline = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_height;			// nr of displayed pum items
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_width;			// width of displayed pum items
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_base_width;		// width of pum items base
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_kind_width;		// width of pum items kind column
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_extra_width;		// width of extra stuff
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_row;			// top row of pum
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_col;			// left column of pum

ಸ್ಥಿರ win_T *pum_window = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_win_row;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_win_height;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_win_col;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_win_wcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_pretend_not_visible = FALSE;

// Border support
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_border = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_margin = 0;	// margin of 1 cell on left and right
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
ಸ್ಥಿರ image_placement_T *pum_imagep = ರಿಕ್ತ;
#endif

// Border characters
ಸ್ಥಿರ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ bottom;
    ಪೂರ್ಣಾಙ್ಕ ಎಡ;
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ_ಎಡ;
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ_ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ bottom_ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ bottom_ಎಡ;
} pum_border_chars;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pum_set_selected(ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ repeat);
ಸ್ಥಿರ ಶೂನ್ಯ pum_draw_border(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ pum_draw_shadow(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ compute_margins(ಪೂರ್ಣಾಙ್ಕ *ಬಲ_margin, ಪೂರ್ಣಾಙ್ಕ *ಎಡ_margin, ಪೂರ್ಣಾಙ್ಕ *ಎಡ_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	ಶೂನ್ಯ \
    pum_set_##feature(ಪೂರ್ಣಾಙ್ಕ enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_size(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pum_size; ++i)
    {
	ಯದಿ (pum_array[i].pum_text != ರಿಕ್ತ)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    ಯದಿ (pum_base_width < w)
		pum_base_width = w;
	}
	ಯದಿ (pum_array[i].pum_kind != ರಿಕ್ತ)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    ಯದಿ (pum_kind_width < w)
		pum_kind_width = w;
	}
	ಯದಿ (pum_array[i].pum_extra != ರಿಕ್ತ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ಯದಿ (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_vertical_placement(
	ಪೂರ್ಣಾಙ್ಕ size,
	ಪೂರ್ಣಾಙ್ಕ above_row,
	ಪೂರ್ಣಾಙ್ಕ below_row)
{
    ಪೂರ್ಣಾಙ್ಕ context_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ cline_visible_offset;
    ಪೂರ್ಣಾಙ್ಕ extra_height = 2 * pum_border + pum_shadow;
    ಪೂರ್ಣಾಙ್ಕ extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    ಯದಿ (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ಯದಿ (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	ಯದಿ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ಸಾಲುಗಳು = 0;
	ಅನ್ಯಥಾ
	    // Leave two lines of context if possible
	    context_ಸಾಲುಗಳು = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	ಯದಿ (pum_win_row >= size + context_ಸಾಲುಗಳು + extra_height)
	{
	    pum_row = pum_win_row - size - context_ಸಾಲುಗಳು - extra_offset;
	    pum_height = size;
	}
	ಅನ್ಯಥಾ
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_ಸಾಲುಗಳು - extra_height;
	}
	ಯದಿ (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    ಅನ್ಯಥಾ
    {
	// pum below "pum_win_row"
	ಯದಿ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ಸಾಲುಗಳು = 0;
	ಅನ್ಯಥಾ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_ಸಾಲುಗಳು = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_ಸಾಲುಗಳು + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	ಯದಿ (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#ಯದಿ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ಯದಿ (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_pum_width_aligned_with_cursor(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ available_width)
{
    ಪೂರ್ಣಾಙ್ಕ end_padding = TRUE;

    ಯದಿ (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    ಯದಿ (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    ಪ್ರತ್ಯಾಗಮ available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_horizontal_placement(ಪೂರ್ಣಾಙ್ಕ cursor_col)
{
    ಪೂರ್ಣಾಙ್ಕ desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    ಪೂರ್ಣಾಙ್ಕ available_width;
    ಪೂರ್ಣಾಙ್ಕ col_offset;
    ಪೂರ್ಣಾಙ್ಕ trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    ಅನ್ಯಥಾ
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    ಯದಿ (set_pum_width_aligned_with_cursor(desired_width, available_width))
	ಪ್ರತ್ಯಾಗಮ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ಯದಿ (available_width > p_pw)
    {
	pum_width = available_width;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    ಯದಿ (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	ಅನ್ಯಥಾ
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
	pum_col = Columns - 1 - col_offset;
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
pum_display(
	pumitem_T   *array,
	ಪೂರ್ಣಾಙ್ಕ	    size,
	ಪೂರ್ಣಾಙ್ಕ	    selected,   // index of initially selected item, -1 if
				// out of range
	ಪೂರ್ಣಾಙ್ಕ	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    ಪೂರ್ಣಾಙ್ಕ	    cursor_col;
    ಪೂರ್ಣಾಙ್ಕ	    above_row;
    ಪೂರ್ಣಾಙ್ಕ	    below_row;
    ಪೂರ್ಣಾಙ್ಕ	    redo_ಎಣಿಕೆ = 0;
#ಯದಿ defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    ಕುರು
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = ರಿಕ್ತ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	ಯದಿ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	ಅನ್ಯಥಾ
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#ಯದಿ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ಯದಿ (pvwin->w_p_pvw)
		ಭಙ್ಗ;
	ಯದಿ (pvwin != ರಿಕ್ತ)
	{
	    ಯದಿ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    ಅನ್ಯಥಾ ಯದಿ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	ಯದಿ (pum_height < 1 || (pum_height == 1 && size > 1))
	    ಪ್ರತ್ಯಾಗಮ;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	ಯದಿ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ಯದಿ (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    ಅನ್ಯಥಾ
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ಯದಿ (curwin->w_p_cole > 0 && conceal_cursor_ಸಾಲು(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ಯದಿ (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    ಯದಿ (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    ಅನ್ಯಥಾ
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
    } ಯಾವತ್ (pum_set_selected(selected, redo_ಎಣಿಕೆ) && ++redo_ಎಣಿಕೆ <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    ಶೂನ್ಯ
pum_call_update_screen(ಶೂನ್ಯ)
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
    ಪೂರ್ಣಾಙ್ಕ
pum_under_menu(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ only_redrawing)
{
    ಪೂರ್ಣಾಙ್ಕ	extra_ಎಡ = pum_border + (pum_margin && pum_border ? 1 : 0);
    ಪೂರ್ಣಾಙ್ಕ	extra_ಬಲ = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    ಪೂರ್ಣಾಙ್ಕ	extra_above = pum_border;
    ಪೂರ್ಣಾಙ್ಕ	extra_below = pum_border + (pum_shadow ? 1 : 0);
    ಪೂರ್ಣಾಙ್ಕ	ತುದಿ = pum_row - extra_above;
    ಪೂರ್ಣಾಙ್ಕ	bot = pum_row + pum_height + extra_below;
    ಪೂರ್ಣಾಙ್ಕ	ಎಡ = pum_col - 1 - extra_ಎಡ;
    ಪೂರ್ಣಾಙ್ಕ	ಬಲ = pum_col + pum_width + pum_scrollbar + extra_ಬಲ;

    ಯದಿ (!((!only_redrawing || pum_will_redraw)
	    && row >= ತುದಿ && row < bot && col >= ಎಡ && col < ಬಲ))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ಯದಿ (only_redrawing)
	{
	    ಯದಿ (col >= ಬಲ - 2 || row == bot - 1)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    ಪೂರ್ಣಾಙ್ಕ	    ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;

	    compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);
	    ಯದಿ (row == ತುದಿ && col >= ಬಲ - 2)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (row == bot - 1 && col < pum_col + 2 - ಎಡ_padding - pum_border
		    - ಎಡ_margin)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *
pum_compute_text_attrs(char_u *text, hlf_T hlf, ಪೂರ್ಣಾಙ್ಕ user_hlattr)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    size_t	leader_len;
    ಪೂರ್ಣಾಙ್ಕ		char_cells;
    ಪೂರ್ಣಾಙ್ಕ		new_attr;
    char_u	*ptr = text;
    ಪೂರ್ಣಾಙ್ಕ		cell_idx = 0;
    garray_T	*ga = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		*attrs = ರಿಕ್ತ;
    char_u	*leader = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		in_fuzzy;
    ಪೂರ್ಣಾಙ್ಕ		matched_len = -1;
    int_u	char_pos = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_select = FALSE;

    ಯದಿ (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    ಯದಿ (leader == ರಿಕ್ತ || *leader == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    attrs = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, vim_strsize(text));
    ಯದಿ (attrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ಯದಿ (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	ಯದಿ (!ga)
	{
	    vim_free(attrs);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    ಯಾವತ್ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ಯದಿ (ga != ರಿಕ್ತ)
	{
	    // Handle fuzzy matching
	    ಪ್ರತಿ (i = 0; i < ga->ga_len; i++)
	    {
		ಯದಿ (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (ಪೂರ್ಣಾಙ್ಕ)leader_len;
	    ಯದಿ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ಯದಿ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	ಪ್ರತಿ (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ಯದಿ (ga != ರಿಕ್ತ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ಪ್ರತ್ಯಾಗಮ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_screen_puts_with_attrs(
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		cells UNUSED,
    char_u	*text,
    ಪೂರ್ಣಾಙ್ಕ		textlen,
    ಪೂರ್ಣಾಙ್ಕ		*attrs)
{
    ಪೂರ್ಣಾಙ್ಕ		col_start = col;
    char_u	*ptr = text;
    ಪೂರ್ಣಾಙ್ಕ		char_len;
    ಪೂರ್ಣಾಙ್ಕ		attr;

    // Render text with proper attributes
    ಯಾವತ್ (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	ಅನ್ಯಥಾ
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ char_u *
pum_get_item(ಪೂರ್ಣಾಙ್ಕ index, ಪೂರ್ಣಾಙ್ಕ type)
{
    ವಿಕಲ್ಪ(type)
    {
	ಪಕ್ಷ CPT_ABBR: ಪ್ರತ್ಯಾಗಮ pum_array[index].pum_text;
	ಪಕ್ಷ CPT_KIND: ಪ್ರತ್ಯಾಗಮ pum_array[index].pum_kind;
	ಪಕ್ಷ CPT_MENU: ಪ್ರತ್ಯಾಗಮ pum_array[index].pum_extra;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
pum_user_attr_combine(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ type, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    ಪ್ರತ್ಯಾಗಮ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_display_rtl_text(
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ     col,
	char_u  *st,
	ಪೂರ್ಣಾಙ್ಕ     attr,
	ಪೂರ್ಣಾಙ್ಕ     *attrs,
	ಪೂರ್ಣಾಙ್ಕ     width,
	ಪೂರ್ಣಾಙ್ಕ     width_limit,
	ಪೂರ್ಣಾಙ್ಕ     totwidth,
	ಪೂರ್ಣಾಙ್ಕ     next_isempty,
	ಪೂರ್ಣಾಙ್ಕ	selected)
{
    char_u  *rt = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ     cells = 0;
    ಪೂರ್ಣಾಙ್ಕ     over_cell = 0;
    ಪೂರ್ಣಾಙ್ಕ     truncated = FALSE;
    ಪೂರ್ಣಾಙ್ಕ     pad = next_isempty ? 0 : 2;
    ಪೂರ್ಣಾಙ್ಕ     remaining = 0;
    ಪೂರ್ಣಾಙ್ಕ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ಪೂರ್ಣಾಙ್ಕ	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ಯದಿ (st == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ col;

    rt = reverse_text(st);
    ಯದಿ (rt == ರಿಕ್ತ)
    {
	VIM_CLEAR(st);
	ಪ್ರತ್ಯಾಗಮ col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ಯದಿ (cells > width_limit)
    {
	ಕುರು
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ಯಾವತ್ (cells > width_limit);

	ಯದಿ (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ಯದಿ (truncated)
    {
	char_u  *orig_rt = rt;
	ಪೂರ್ಣಾಙ್ಕ     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ಯದಿ (cells > remaining)
	{
	    ಯಾವತ್ (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(orig_rt);
	ಯದಿ (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	ಯದಿ (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    ಯದಿ (attrs == ರಿಕ್ತ)
	screen_puts_len(rt, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(rt), row, col - cells + 1, attr);
    ಅನ್ಯಥಾ
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(ಪೂರ್ಣಾಙ್ಕ)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    ಪ್ರತ್ಯಾಗಮ col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_display_ltr_text(
	ಪೂರ್ಣಾಙ್ಕ     row,
	ಪೂರ್ಣಾಙ್ಕ     col,
	char_u  *st,
	ಪೂರ್ಣಾಙ್ಕ     attr,
	ಪೂರ್ಣಾಙ್ಕ     *attrs,
	ಪೂರ್ಣಾಙ್ಕ     width,        // width already calculated in outer loop
	ಪೂರ್ಣಾಙ್ಕ     width_limit,
	ಪೂರ್ಣಾಙ್ಕ     totwidth,
	ಪೂರ್ಣಾಙ್ಕ     next_isempty,
	ಪೂರ್ಣಾಙ್ಕ	selected)
{
    ಪೂರ್ಣಾಙ್ಕ     size = 0;
    ಪೂರ್ಣಾಙ್ಕ     cells = 0;
    char_u  *st_end = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ     over_cell = 0;
    ಪೂರ್ಣಾಙ್ಕ     pad = next_isempty ? 0 : 2;
    ಪೂರ್ಣಾಙ್ಕ     truncated = FALSE;
    ಪೂರ್ಣಾಙ್ಕ     remaining = 0;
    ಪೂರ್ಣಾಙ್ಕ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ಪೂರ್ಣಾಙ್ಕ	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ಯದಿ (st == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ col;

    size = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ಯಾವತ್ (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	ಯದಿ (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	ಅನ್ಯಥಾ
	    --cells;
    }

    // truncated
    ಯದಿ (truncated)
    {
	remaining = width_limit - totwidth - 1;
	ಯದಿ (cells > remaining)
	{
	    st_end = st + size;
	    ಯಾವತ್ (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	ಯದಿ (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    ಯದಿ (attrs == ರಿಕ್ತ)
	screen_puts_len(st, size, row, col, attr);
    ಅನ್ಯಥಾ
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    ಯದಿ (truncated)
    {
	ಯದಿ (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ಪ್ರತ್ಯಾಗಮ col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_process_item(
	ಪೂರ್ಣಾಙ್ಕ     row,
	ಪೂರ್ಣಾಙ್ಕ     col,
	ಪೂರ್ಣಾಙ್ಕ     idx,
	ಪೂರ್ಣಾಙ್ಕ     j,         // Current position in order array
	ಪೂರ್ಣಾಙ್ಕ     *order,    // Order array
	hlf_T   hlf,
	ಪೂರ್ಣಾಙ್ಕ     attr,
	ಪೂರ್ಣಾಙ್ಕ     *totwidth_ptr,
	ಪೂರ್ಣಾಙ್ಕ     next_isempty)
{
    ಪೂರ್ಣಾಙ್ಕ     item_type = order[j];
    char_u  *s = ರಿಕ್ತ;
    char_u  *p = pum_get_item(idx, item_type);
    ಪೂರ್ಣಾಙ್ಕ     width = 0;  // item width
    ಪೂರ್ಣಾಙ್ಕ     w;		// char width
    ಪೂರ್ಣಾಙ್ಕ	    selected = idx == pum_selected;

    ಪ್ರತಿ ( ; ; MB_PTR_ADV(p))
    {
	ಯದಿ (s == ರಿಕ್ತ)
	    s = p;
	w = ptr2cells(p);
	ಯದಿ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    ಅನುವರ್ತ;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	ಪೂರ್ಣಾಙ್ಕ     *attrs = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ     saved = *p;

	ಯದಿ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ಯದಿ (saved != NUL)
	    *p = saved;

	ಯದಿ (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	ಅನ್ಯಥಾ
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	ಯದಿ (attrs != ರಿಕ್ತ)
	    VIM_CLEAR(attrs);

	ಯದಿ (*p != TAB)
	    ಭಙ್ಗ;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	ಅನ್ಯಥಾ
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = ರಿಕ್ತ;  // start text at next char
	width = 0;
    }

    ಪ್ರತ್ಯಾಗಮ col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_scrollbar(
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	i,
	ಪೂರ್ಣಾಙ್ಕ	thumb_pos,
	ಪೂರ್ಣಾಙ್ಕ	thumb_height)
{
    ಯದಿ (pum_scrollbar <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    ಅನ್ಯಥಾ
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
pum_align_order(ಪೂರ್ಣಾಙ್ಕ *order)
{
    ಪೂರ್ಣಾಙ್ಕ is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ಸ್ಥಿರ ಶೂನ್ಯ pum_free_bg(ಶೂನ್ಯ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    ಶೂನ್ಯ
pum_opacity_changed(ಶೂನ್ಯ)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    ಯದಿ (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
pum_free_bg(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_ಸಾಲುಗಳು);
    VIM_CLEAR(pum_bg_ಸಾಲುಗಳು_UC);
    ಪ್ರತಿ (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_ಸಾಲುಗಳು_C[k]);
    pum_bg_ತುದಿ = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    ಶೂನ್ಯ
pum_redraw(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		row = pum_row;
    ಪೂರ್ಣಾಙ್ಕ		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*p = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		totwidth;
    ಪೂರ್ಣಾಙ್ಕ		thumb_pos = 0;
    ಪೂರ್ಣಾಙ್ಕ		thumb_height = 1;
    ಪೂರ್ಣಾಙ್ಕ		item_type;
    ಪೂರ್ಣಾಙ್ಕ		order[3];
    ಪೂರ್ಣಾಙ್ಕ		next_isempty = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		ವಸ್ತುಗಳು_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    ಪೂರ್ಣಾಙ್ಕ		basic_width;  // first item width
    ಪೂರ್ಣಾಙ್ಕ		last_isabbr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		orig_attr = -1;
    ಪೂರ್ಣಾಙ್ಕ		scroll_range = pum_size - pum_height;
    bool	override_success;
    ಪೂರ್ಣಾಙ್ಕ		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ತಳ್ಳು_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ಯದಿ (pum_imagep == ರಿಕ್ತ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ರಿಕ್ತ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ಯದಿ (pum_imagep != ರಿಕ್ತ)
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

    ಯದಿ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ಯದಿ (opacity_active && pum_bg_ಸಾಲುಗಳು != ರಿಕ್ತ
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	ಯದಿ (opacity_active && pum_bg_ಸಾಲುಗಳು != ರಿಕ್ತ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ಯದಿ (Screen_ಸಾಲುಗಳು != ರಿಕ್ತ && ScreenAttrs != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ	save_ತುದಿ, save_bot, save_ncells, k;

		pum_free_bg();
		save_ತುದಿ = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ಯದಿ (save_ತುದಿ < 0)
		    save_ತುದಿ = 0;
		ಯದಿ (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_ತುದಿ = save_ತುದಿ;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		ಯದಿ (save_ತುದಿ < save_bot)
		{
		    save_ncells = (save_bot - save_ತುದಿ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_ಸಾಲುಗಳು = LALLOC_MULT(schar_T, save_ncells);
		    ಯದಿ (enc_utf8)
		    {
			pum_bg_ಸಾಲುಗಳು_UC = LALLOC_MULT(u8char_T, save_ncells);
			ಪ್ರತಿ (k = 0; k < MAX_MCO; ++k)
			    pum_bg_ಸಾಲುಗಳು_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ಯದಿ (pum_bg_attrs != ರಿಕ್ತ && pum_bg_ಸಾಲುಗಳು != ರಿಕ್ತ)
		    {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = save_ತುದಿ; r < save_bot; ++r)
			{
			    ಪೂರ್ಣಾಙ್ಕ soff = (r - save_ತುದಿ) * screen_Columns;
			    ಪೂರ್ಣಾಙ್ಕ loff = ಸಾಲು_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * ಪರಿಮಾಣ(sattr_T));
			    mch_memmove(pum_bg_ಸಾಲುಗಳು + soff,
				    Screen_ಸಾಲುಗಳು + loff,
				    screen_Columns * ಪರಿಮಾಣ(schar_T));
			    ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ
						&& Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
			    {
				mch_memmove(pum_bg_ಸಾಲುಗಳು_UC + soff,
					Screen_ಸಾಲುಗಳು_UC + loff,
					screen_Columns * ಪರಿಮಾಣ(u8char_T));
				ಪ್ರತಿ (k = 0; k < MAX_MCO; ++k)
				    ಯದಿ (pum_bg_ಸಾಲುಗಳು_C[k] != ರಿಕ್ತ
						    && Screen_ಸಾಲುಗಳು_C[k] != ರಿಕ್ತ)
					mch_memmove(pum_bg_ಸಾಲುಗಳು_C[k] + soff,
						Screen_ಸಾಲುಗಳು_C[k] + loff,
						screen_Columns
						    * ಪರಿಮಾಣ(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ಅನ್ಯಥಾ
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

    ಯದಿ (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	ಯದಿ (thumb_height == 0)
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
    ಯದಿ (pum_border)
	pum_draw_border();
    ಯದಿ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ಯದಿ (opacity_active)
	screen_pum_blend = 100 - (ಪೂರ್ಣಾಙ್ಕ)p_po;

    ಪ್ರತಿ (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	{
	    ಯದಿ (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	ಅನ್ಯಥಾ
#endif
	    ಯದಿ (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = ವಸ್ತುಗಳು_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	ಪ್ರತಿ (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ಯದಿ (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    ಯದಿ (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		next_isempty = TRUE;

	    ಯದಿ (p != ರಿಕ್ತ)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    ಯದಿ (j > 0)
		n = ವಸ್ತುಗಳು_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    ಅನ್ಯಥಾ
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ಯದಿ (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == ರಿಕ್ತ)))
		    || basic_width + n >= pum_width)
		ಭಙ್ಗ;
#ifdef FEAT_RIGHTLEFT
	    ಯದಿ (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	ಅನ್ಯಥಾ
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

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    ಶೂನ್ಯ
pum_position_info_popup(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ col = pum_col + pum_width + pum_scrollbar + 1;
    ಪೂರ್ಣಾಙ್ಕ row = pum_row;
    ಪೂರ್ಣಾಙ್ಕ botpos = POPPOS_BOTLEFT;
    ಪೂರ್ಣಾಙ್ಕ	used_maxwidth_opt = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	ಬಲ_margin, ಎಡ_margin, dummy;
    ಪೂರ್ಣಾಙ್ಕ ಎಡ_extra_width, ಬಲ_extra_width;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &dummy);

    ಎಡ_extra_width = pum_border + ಎಡ_margin;
    ಬಲ_extra_width = pum_border + ಬಲ_margin + (pum_shadow ? 2 : 0);
    col += ಬಲ_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ಯದಿ (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - ಎಡ_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - ಎಡ_extra_width;
    }
    ಅನ್ಯಥಾ
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    ಯದಿ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_ತುದಿ_extra(wp);
    ಯದಿ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ಯದಿ (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	ಅನ್ಯಥಾ
	    // menu below cursor line, align with top
	    row += 1;
    }
    ಅನ್ಯಥಾ
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ಯದಿ (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_set_selected(ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ repeat UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	    resized = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    context = pum_height / 2;
    ಪೂರ್ಣಾಙ್ಕ	    scroll_offset;
#ifdef FEAT_QUICKFIX
    ಪೂರ್ಣಾಙ್ಕ	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ಪೂರ್ಣಾಙ್ಕ	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    ಯದಿ (pum_selected >= 0 && pum_selected < pum_size)
    {
	ಯದಿ (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ಯದಿ (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		ಯದಿ (pum_first < 0)
		    pum_first = 0;
		ಅನ್ಯಥಾ ಯದಿ (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    ಅನ್ಯಥಾ
		pum_first = pum_selected;
	}
	ಅನ್ಯಥಾ ಯದಿ (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ಯದಿ (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    ಅನ್ಯಥಾ
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ಯದಿ (pum_height > 2)
	{
	    ಯದಿ (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    ಅನ್ಯಥಾ ಯದಿ (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#ಯದಿ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ಯದಿ (pum_array[pum_selected].pum_info != ರಿಕ್ತ
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ಅನ್ಯಥಾ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ಯದಿ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ಅನ್ಯಥಾ ಯದಿ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    ಅನ್ಯಥಾ
		use_popup = USEPOPUP_NONE;
	    ಯದಿ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ಯದಿ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ಯದಿ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ಯದಿ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		ಪೂರ್ಣಾಙ್ಕ	res = OK;

		ಯದಿ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ರಿಕ್ತ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ಯಾವತ್ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ಅನ್ಯಥಾ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_ONE, 0, ರಿಕ್ತ);
		    --no_u_sync;
		    ಯದಿ (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_ಮೌಲ್ಯ_give_err((char_u *)"swf",
							  0L, ರಿಕ್ತ, OPT_LOCAL);
			set_option_ಮೌಲ್ಯ_give_err((char_u *)"bl",
							  0L, ರಿಕ್ತ, OPT_LOCAL);
			set_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_ಮೌಲ್ಯ_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_ಮೌಲ್ಯ_give_err((char_u *)"diff",
							  0L, ರಿಕ್ತ, OPT_LOCAL);
		    }
		}
		ಯದಿ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    ಪ್ರತಿ (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ಯದಿ (e == ರಿಕ್ತ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ಭಙ್ಗ;
			}
			*e = NUL;
			ml_append(lnum++, p, (ಪೂರ್ಣಾಙ್ಕ)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ಯದಿ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ಯದಿ (curwin->w_height < lnum)
			{
			    win_setheight((ಪೂರ್ಣಾಙ್ಕ)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    ಯದಿ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    ಯದಿ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ಯದಿ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ಯದಿ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			ಪೂರ್ಣಾಙ್ಕ save_redr_status;

			ಯದಿ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ಯದಿ (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ಯದಿ (resized && win_valid(curwin_save))
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

			ಯದಿ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ಯದಿ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ಯದಿ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ಯದಿ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ಯದಿ (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    ಪ್ರತ್ಯಾಗಮ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    ಶೂನ್ಯ
pum_undisplay(ಶೂನ್ಯ)
{
#ifdef FEAT_IMAGE_POPUP
    ಯದಿ (pum_imagep != ರಿಕ್ತ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ರಿಕ್ತ;
    }
#endif
    pum_free_bg();
    pum_array = ರಿಕ್ತ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ಯದಿ (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    ಶೂನ್ಯ
pum_clear(ಶೂನ್ಯ)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    ಪೂರ್ಣಾಙ್ಕ
pum_visible(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ !pum_pretend_not_visible && pum_array != ರಿಕ್ತ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pum_in_same_position(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    ಪ್ರತ್ಯಾಗಮ pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    ಪೂರ್ಣಾಙ್ಕ
pum_redraw_in_same_position(ಶೂನ್ಯ)
{
    ಯದಿ (!pum_visible() || pum_will_redraw)
	ಪ್ರತ್ಯಾಗಮ FALSE;  // nothing to do

    ಪ್ರತ್ಯಾಗಮ pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    ಶೂನ್ಯ
pum_may_redraw(ಶೂನ್ಯ)
{
    pumitem_T	*array = pum_array;
    ಪೂರ್ಣಾಙ್ಕ		len = pum_size;
    ಪೂರ್ಣಾಙ್ಕ		selected = pum_selected;

    ಯದಿ (!pum_visible() || pum_will_redraw)
	ಪ್ರತ್ಯಾಗಮ;  // nothing to do

    ಯದಿ (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
pum_get_height(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ pum_height;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    ಶೂನ್ಯ
pum_set_event_info(dict_T *dict)
{
    ಯದಿ (!pum_visible())
	ಪ್ರತ್ಯಾಗಮ;
    (ಶೂನ್ಯ)dict_add_ಸಂಖ್ಯೆ(dict, "height", pum_height);
    (ಶೂನ್ಯ)dict_add_ಸಂಖ್ಯೆ(dict, "width", pum_width);
    (ಶೂನ್ಯ)dict_add_ಸಂಖ್ಯೆ(dict, "row", pum_row);
    (ಶೂನ್ಯ)dict_add_ಸಂಖ್ಯೆ(dict, "col", pum_col);
    (ಶೂನ್ಯ)dict_add_ಸಂಖ್ಯೆ(dict, "size", pum_size);
    (ಶೂನ್ಯ)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#ಯದಿ defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_position_at_mouse(ಪೂರ್ಣಾಙ್ಕ min_width)
{
    ಪೂರ್ಣಾಙ್ಕ extra_height = pum_border * 2 + pum_shadow;
    ಪೂರ್ಣಾಙ್ಕ extra_offset = pum_border + pum_shadow;
    ಪೂರ್ಣಾಙ್ಕ trailing_overhead = 0;
    ಪೂರ್ಣಾಙ್ಕ col_offset = 0;
    ಪೂರ್ಣಾಙ್ಕ available_width;

    ಯದಿ (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	ಯದಿ (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	ಯದಿ (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	ಯದಿ (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	ಯದಿ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	ಅನ್ಯಥಾ
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    ಅನ್ಯಥಾ
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	ಯದಿ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	ಅನ್ಯಥಾ
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = ರಿಕ್ತ;
}

#endif

#ಯದಿ defined(FEAT_BEVAL_TERM)
ಸ್ಥಿರ pumitem_T *balloon_array = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*start;
    ಪೂರ್ಣಾಙ್ಕ		bytelen;
    ಪೂರ್ಣಾಙ್ಕ		cells;
    ಪೂರ್ಣಾಙ್ಕ		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    ಪೂರ್ಣಾಙ್ಕ
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		quoted = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		height;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		item_idx;
    ಪೂರ್ಣಾಙ್ಕ		indent = 0;
    ಪೂರ್ಣಾಙ್ಕ		max_cells = 0;
    ಪೂರ್ಣಾಙ್ಕ		max_height = Rows / 2 - 1;
    ಪೂರ್ಣಾಙ್ಕ		long_item_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		split_long_ವಸ್ತುಗಳು = FALSE;

    ga_init2(&ga, ಪರಿಮಾಣ(balpart_T), 20);
    p = mesg;

    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    ಗಚ್ಛ failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	ಯಾವತ್ (*p != NUL)
	{
	    ಯದಿ (*p == '"')
		quoted = !quoted;
	    ಅನ್ಯಥಾ ಯದಿ (*p == '\n')
		ಭಙ್ಗ;
	    ಅನ್ಯಥಾ ಯದಿ (*p == '\\' && p[1] != NUL)
		++p;
	    ಅನ್ಯಥಾ ಯದಿ (!quoted)
	    {
		ಯದಿ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ಯದಿ (*p == '{')
			++indent;
		    ಅನ್ಯಥಾ ಯದಿ (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    ಭಙ್ಗ;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	ಯದಿ (*p == '\n')
	    ++p;
	ಯದಿ (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_ಎಣಿಕೆ += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ಯದಿ (long_item_ಎಣಿಕೆ > 0 && height + long_item_ಎಣಿಕೆ <= max_height)
    {
	split_long_ವಸ್ತುಗಳು = TRUE;
	height += long_item_ಎಣಿಕೆ;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ಯದಿ (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    ಯದಿ (*array == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    ಪ್ರತಿ (ಸಾಲು = 1, item_idx = 0; ಸಾಲು < height - 1; ++item_idx)
    {
	ಪೂರ್ಣಾಙ್ಕ	skip;
	ಪೂರ್ಣಾಙ್ಕ	thislen;
	ಪೂರ್ಣಾಙ್ಕ	copylen;
	ಪೂರ್ಣಾಙ್ಕ	ind;
	ಪೂರ್ಣಾಙ್ಕ	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	ಯದಿ (item->bytelen == 0)
	    (*array)[ಸಾಲು++].pum_text = vim_strsave((char_u *)"");
	ಅನ್ಯಥಾ
	    ಪ್ರತಿ (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		ಯದಿ (split_long_ವಸ್ತುಗಳು && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    ಪ್ರತಿ (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			ಯದಿ ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    ಭಙ್ಗ;
		    thislen = p - (item->start + skip);
		}
		ಅನ್ಯಥಾ
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		ಯದಿ (p == ರಿಕ್ತ)
		{
		    ಪ್ರತಿ (ಸಾಲು = 0; ಸಾಲು <= height - 1; ++ಸಾಲು)
			vim_free((*array)[ಸಾಲು].pum_text);
		    vim_free(*array);
		    ಗಚ್ಛ failed;
		}
		ಪ್ರತಿ (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		ಪ್ರತಿ (copylen = thislen; copylen > 0; --copylen)
		    ಯದಿ (item->start[skip + copylen - 1] != ' ')
			ಭಙ್ಗ;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[ಸಾಲು].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++ಸಾಲು;
	    }
    }
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ height;

failed:
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಶೂನ್ಯ
ui_remove_balloon(ಶೂನ್ಯ)
{
    ಯದಿ (balloon_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pum_undisplay();
    ಯಾವತ್ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    ಶೂನ್ಯ
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    ಯದಿ (mesg == ರಿಕ್ತ && list == ರಿಕ್ತ)
    {
	pum_undisplay();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (list != ರಿಕ್ತ)
    {
	listitem_T  *li;
	ಪೂರ್ಣಾಙ್ಕ	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ಯದಿ (balloon_array == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	CHECK_LIST_MATERIALIZE(list);
	ಪ್ರತಿ (idx = 0, li = list->lv_first; li != ರಿಕ್ತ; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == ರಿಕ್ತ ? (char_u *)"" : text);
	}
    }
    ಅನ್ಯಥಾ
	balloon_arraysize = split_message(mesg, &balloon_array);

    ಯದಿ (balloon_arraysize <= 0)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಶೂನ್ಯ
ui_may_remove_balloon(ಶೂನ್ಯ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#ಯದಿ defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_select_mouse_pos(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ idx = mouse_row - pum_row;

    ಯದಿ (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    ಅನ್ಯಥಾ ಯದಿ (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_execute_menu(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ mode)
{
    vimmenu_T   *mp;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	ಯದಿ ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    ಭಙ್ಗ;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    ಶೂನ್ಯ
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    ಪೂರ್ಣಾಙ್ಕ		save_bevalterm = p_bevalterm;
# endif
    ಪೂರ್ಣಾಙ್ಕ		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	ಯದಿ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ಯದಿ (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	ಪ್ರತ್ಯಾಗಮ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    ಯದಿ (array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = ರಿಕ್ತ;

	// Make a copy of the text, the menu may be redefined in a callback.
	ಯದಿ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ಅನ್ಯಥಾ ಯದಿ (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	ಯದಿ (s != ರಿಕ್ತ)
	{
	    s = vim_strsave(s);
	    ಯದಿ (s != ರಿಕ್ತ)
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

    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ಯದಿ (c == ESC || c == Ctrl_C || pum_array == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ ಯದಿ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ಯಾವತ್ (pum_selected > 0)
	    {
		--pum_selected;
		ಯದಿ (*array[pum_selected].pum_text != NUL)
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ಯಾವತ್ (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		ಯದಿ (*array[pum_selected].pum_text != NUL)
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ಅನ್ಯಥಾ ಯದಿ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ಯದಿ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ಭಙ್ಗ;
	}
    }

    ಪ್ರತಿ (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    ಶೂನ್ಯ
pum_make_popup(char_u *path_name, ಪೂರ್ಣಾಙ್ಕ use_mouse_pos)
{
    vimmenu_T *menu;

    ಯದಿ (!use_mouse_pos)
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
    ಯದಿ (menu != ರಿಕ್ತ)
	pum_show_popupmenu(menu);
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_singlewidth(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c >= 0 && c < 0x80)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (utf_char2cells(c) == 1)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    ಶೂನ್ಯ
pum_set_border_chars(
	ಪೂರ್ಣಾಙ್ಕ ತುದಿ,
	ಪೂರ್ಣಾಙ್ಕ ಬಲ,
	ಪೂರ್ಣಾಙ್ಕ bottom,
	ಪೂರ್ಣಾಙ್ಕ ಎಡ,
	ಪೂರ್ಣಾಙ್ಕ ತುದಿ_ಎಡ,
	ಪೂರ್ಣಾಙ್ಕ ತುದಿ_ಬಲ,
	ಪೂರ್ಣಾಙ್ಕ bottom_ಬಲ,
	ಪೂರ್ಣಾಙ್ಕ bottom_ಎಡ)
{
    ಯದಿ (!is_singlewidth(ತುದಿ) || !is_singlewidth(ಬಲ)
	    || !is_singlewidth(bottom) || !is_singlewidth(ಎಡ)
	    || !is_singlewidth(ತುದಿ_ಎಡ) || !is_singlewidth(ತುದಿ_ಬಲ)
	    || !is_singlewidth(bottom_ಬಲ) || !is_singlewidth(bottom_ಎಡ))
	ಪ್ರತ್ಯಾಗಮ;
    pum_border_chars.ತುದಿ = ತುದಿ;
    pum_border_chars.ಬಲ = ಬಲ;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ಎಡ = ಎಡ;
    pum_border_chars.ತುದಿ_ಎಡ = ತುದಿ_ಎಡ;
    pum_border_chars.ತುದಿ_ಬಲ = ತುದಿ_ಬಲ;
    pum_border_chars.bottom_ಬಲ = bottom_ಬಲ;
    pum_border_chars.bottom_ಎಡ = bottom_ಎಡ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compute_margins(ಪೂರ್ಣಾಙ್ಕ *ಬಲ_margin, ಪೂರ್ಣಾಙ್ಕ *ಎಡ_margin, ಪೂರ್ಣಾಙ್ಕ *ಎಡ_padding)
{
    *ಬಲ_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	*ಎಡ_padding = (pum_col < Columns - 1 - pum_border);
	*ಎಡ_margin = *ಬಲ_margin && (pum_col < Columns - 3);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    *ಎಡ_padding = (pum_col > pum_border);
    *ಎಡ_margin = *ಬಲ_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_border(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	ತುದಿ_row = pum_row - 1;
    ಪೂರ್ಣಾಙ್ಕ	bottom_row = pum_row + pum_height;
    ಪೂರ್ಣಾಙ್ಕ	col;
    ಪೂರ್ಣಾಙ್ಕ	attr = highlight_attr[HLF_PMB];
    ಪೂರ್ಣಾಙ್ಕ	ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;
    ಪೂರ್ಣಾಙ್ಕ	inner_width;

    ಯದಿ (!pum_border)
	ಪ್ರತ್ಯಾಗಮ;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);

    inner_width = pum_width + pum_scrollbar + ಎಡ_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
	col = pum_col + ಎಡ_padding + 1;
    ಅನ್ಯಥಾ
#endif
	col = pum_col - ಎಡ_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಬಲ, ತುದಿ_row, col, attr);
	ಯದಿ (ಎಡ_margin)
	    screen_putchar(' ', ತುದಿ_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_ಬಲ, bottom_row, col, attr);
	ಯದಿ (ಎಡ_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    ಅನ್ಯಥಾ
#endif
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಎಡ, ತುದಿ_row, col, attr);
	ಯದಿ (ಎಡ_margin)
	    screen_putchar(' ', ತುದಿ_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_ಎಡ, bottom_row, col, attr);
	ಯದಿ (ಎಡ_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < inner_width; i++)
    {
	ಪೂರ್ಣಾಙ್ಕ cur_col;
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    cur_col = col - 1 - i;
	ಅನ್ಯಥಾ
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.ತುದಿ, ತುದಿ_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಎಡ, ತುದಿ_row,
		col - inner_width - 1, attr);
	ಯದಿ (ಬಲ_margin)
	    screen_putchar(' ', ತುದಿ_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_ಎಡ, bottom_row,
		col - inner_width - 1, attr);
	ಯದಿ (ಬಲ_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    ಅನ್ಯಥಾ
#endif
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಬಲ, ತುದಿ_row,
		col + inner_width + 1, attr);
	ಯದಿ (ಬಲ_margin)
	    screen_putchar(' ', ತುದಿ_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_ಬಲ, bottom_row,
		col + inner_width + 1, attr);
	ಯದಿ (ಬಲ_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pum_height; i++)
    {
	ಪೂರ್ಣಾಙ್ಕ row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	{
	    screen_putchar(pum_border_chars.ಎಡ, row, col, attr);
	    ಯದಿ (ಎಡ_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.ಬಲ, row,
		    col - inner_width - 1, attr);
	    ಯದಿ (ಬಲ_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	ಅನ್ಯಥಾ
#endif
	{
	    screen_putchar(pum_border_chars.ಎಡ, row, col, attr);
	    ಯದಿ (ಎಡ_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.ಬಲ, row,
		    col + inner_width + 1, attr);
	    ಯದಿ (ಬಲ_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    ಶೂನ್ಯ
put_shadow_char(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    char_u  buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಙ್ಕ	    shadow_attr = highlight_attr[HLF_PMS];
    ಪೂರ್ಣಾಙ್ಕ	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_shadow(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	row, col, next_col;
    ಪೂರ್ಣಾಙ್ಕ	width, offset;
    ಪೂರ್ಣಾಙ್ಕ	inner_width;
    ಪೂರ್ಣಾಙ್ಕ	ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;

    ಯದಿ (!pum_shadow)
	ಪ್ರತ್ಯಾಗಮ;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + ಬಲ_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    ಅನ್ಯಥಾ
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + ಎಡ_padding + pum_border + ಎಡ_margin;
    offset = 2 - ಎಡ_padding - pum_border - ಎಡ_margin;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (pum_rl)
	    col = pum_col - offset - i;
	ಅನ್ಯಥಾ
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
