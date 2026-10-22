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
#അന്തര്ഭാവ "vim.h"

സ്ഥിതം pumitem_T *pum_array = ശൂന്യ;	// items of displayed pum
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_size;			// nr of items in "pum_array"
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_selected;		// index of selected item or -1
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_first = 0;		// index of top item

സ്ഥിതം പൂർണ്ണസംഖ്യ call_update_screen = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_in_cmdline = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ pum_height;			// nr of displayed pum items
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_width;			// width of displayed pum items
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_base_width;		// width of pum items base
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_kind_width;		// width of pum items kind column
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_extra_width;		// width of extra stuff
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ pum_row;			// top row of pum
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_col;			// left column of pum

സ്ഥിതം win_T *pum_window = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_win_row;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_win_height;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_win_col;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_win_wcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_pretend_not_visible = FALSE;

// Border support
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_border = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_margin = 0;	// margin of 1 cell on left and right
സ്ഥിതം പൂർണ്ണസംഖ്യ pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
സ്ഥിതം image_placement_T *pum_imagep = ശൂന്യ;
#endif

// Border characters
സ്ഥിതം ഘടന {
    പൂർണ്ണസംഖ്യ മുകൾ;
    പൂർണ്ണസംഖ്യ വലത്;
    പൂർണ്ണസംഖ്യ bottom;
    പൂർണ്ണസംഖ്യ ഇടത്;
    പൂർണ്ണസംഖ്യ മുകൾ_ഇടത്;
    പൂർണ്ണസംഖ്യ മുകൾ_വലത്;
    പൂർണ്ണസംഖ്യ bottom_വലത്;
    പൂർണ്ണസംഖ്യ bottom_ഇടത്;
} pum_border_chars;

സ്ഥിതം പൂർണ്ണസംഖ്യ pum_set_selected(പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ repeat);
സ്ഥിതം ശൂന്യം pum_draw_border(ശൂന്യം);
സ്ഥിതം ശൂന്യം pum_draw_shadow(ശൂന്യം);
സ്ഥിതം ശൂന്യം compute_margins(പൂർണ്ണസംഖ്യ *വലത്_margin, പൂർണ്ണസംഖ്യ *ഇടത്_margin, പൂർണ്ണസംഖ്യ *ഇടത്_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	ശൂന്യം \
    pum_set_##feature(പൂർണ്ണസംഖ്യ enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    സ്ഥിതം ശൂന്യം
pum_compute_size(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pum_size; ++i)
    {
	എങ്കിൽ (pum_array[i].pum_text != ശൂന്യ)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    എങ്കിൽ (pum_base_width < w)
		pum_base_width = w;
	}
	എങ്കിൽ (pum_array[i].pum_kind != ശൂന്യ)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    എങ്കിൽ (pum_kind_width < w)
		pum_kind_width = w;
	}
	എങ്കിൽ (pum_array[i].pum_extra != ശൂന്യ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    എങ്കിൽ (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    സ്ഥിതം ശൂന്യം
pum_compute_vertical_placement(
	പൂർണ്ണസംഖ്യ size,
	പൂർണ്ണസംഖ്യ above_row,
	പൂർണ്ണസംഖ്യ below_row)
{
    പൂർണ്ണസംഖ്യ context_വരികൾ;
    പൂർണ്ണസംഖ്യ cline_visible_offset;
    പൂർണ്ണസംഖ്യ extra_height = 2 * pum_border + pum_shadow;
    പൂർണ്ണസംഖ്യ extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    എങ്കിൽ (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    എങ്കിൽ (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	എങ്കിൽ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_വരികൾ = 0;
	അല്ലെങ്കിൽ
	    // Leave two lines of context if possible
	    context_വരികൾ = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	എങ്കിൽ (pum_win_row >= size + context_വരികൾ + extra_height)
	{
	    pum_row = pum_win_row - size - context_വരികൾ - extra_offset;
	    pum_height = size;
	}
	അല്ലെങ്കിൽ
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_വരികൾ - extra_height;
	}
	എങ്കിൽ (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    അല്ലെങ്കിൽ
    {
	// pum below "pum_win_row"
	എങ്കിൽ (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_വരികൾ = 0;
	അല്ലെങ്കിൽ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_വരികൾ = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_വരികൾ + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	എങ്കിൽ (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#എങ്കിൽ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    എങ്കിൽ (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_pum_width_aligned_with_cursor(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ available_width)
{
    പൂർണ്ണസംഖ്യ end_padding = TRUE;

    എങ്കിൽ (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    എങ്കിൽ (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    തിരികെ available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    സ്ഥിതം ശൂന്യം
pum_compute_horizontal_placement(പൂർണ്ണസംഖ്യ cursor_col)
{
    പൂർണ്ണസംഖ്യ desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    പൂർണ്ണസംഖ്യ available_width;
    പൂർണ്ണസംഖ്യ col_offset;
    പൂർണ്ണസംഖ്യ trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    അല്ലെങ്കിൽ
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    എങ്കിൽ (set_pum_width_aligned_with_cursor(desired_width, available_width))
	തിരികെ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    എങ്കിൽ (available_width > p_pw)
    {
	pum_width = available_width;
	തിരികെ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    എങ്കിൽ (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	അല്ലെങ്കിൽ
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	തിരികെ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
	pum_col = Columns - 1 - col_offset;
    അല്ലെങ്കിൽ
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
    ശൂന്യം
pum_display(
	pumitem_T   *array,
	പൂർണ്ണസംഖ്യ	    size,
	പൂർണ്ണസംഖ്യ	    selected,   // index of initially selected item, -1 if
				// out of range
	പൂർണ്ണസംഖ്യ	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    പൂർണ്ണസംഖ്യ	    cursor_col;
    പൂർണ്ണസംഖ്യ	    above_row;
    പൂർണ്ണസംഖ്യ	    below_row;
    പൂർണ്ണസംഖ്യ	    redo_എണ്ണം = 0;
#എങ്കിൽ defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    ചെയ്യുക
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = ശൂന്യ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	എങ്കിൽ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	അല്ലെങ്കിൽ
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#എങ്കിൽ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    എങ്കിൽ (pvwin->w_p_pvw)
		നിർത്തുക;
	എങ്കിൽ (pvwin != ശൂന്യ)
	{
	    എങ്കിൽ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    അല്ലെങ്കിൽ എങ്കിൽ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	എങ്കിൽ (pum_height < 1 || (pum_height == 1 && size > 1))
	    തിരികെ;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	എങ്കിൽ (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    എങ്കിൽ (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    അല്ലെങ്കിൽ
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    എങ്കിൽ (curwin->w_p_cole > 0 && conceal_cursor_വരി(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		എങ്കിൽ (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    അല്ലെങ്കിൽ
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
    } ആകുമ്പോൾ (pum_set_selected(selected, redo_എണ്ണം) && ++redo_എണ്ണം <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    ശൂന്യം
pum_call_update_screen(ശൂന്യം)
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
    പൂർണ്ണസംഖ്യ
pum_under_menu(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ only_redrawing)
{
    പൂർണ്ണസംഖ്യ	extra_ഇടത് = pum_border + (pum_margin && pum_border ? 1 : 0);
    പൂർണ്ണസംഖ്യ	extra_വലത് = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    പൂർണ്ണസംഖ്യ	extra_above = pum_border;
    പൂർണ്ണസംഖ്യ	extra_below = pum_border + (pum_shadow ? 1 : 0);
    പൂർണ്ണസംഖ്യ	മുകൾ = pum_row - extra_above;
    പൂർണ്ണസംഖ്യ	bot = pum_row + pum_height + extra_below;
    പൂർണ്ണസംഖ്യ	ഇടത് = pum_col - 1 - extra_ഇടത്;
    പൂർണ്ണസംഖ്യ	വലത് = pum_col + pum_width + pum_scrollbar + extra_വലത്;

    എങ്കിൽ (!((!only_redrawing || pum_will_redraw)
	    && row >= മുകൾ && row < bot && col >= ഇടത് && col < വലത്))
	തിരികെ FALSE;

    എങ്കിൽ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	എങ്കിൽ (only_redrawing)
	{
	    എങ്കിൽ (col >= വലത് - 2 || row == bot - 1)
		തിരികെ FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    പൂർണ്ണസംഖ്യ	    വലത്_margin, ഇടത്_margin, ഇടത്_padding;

	    compute_margins(&വലത്_margin, &ഇടത്_margin, &ഇടത്_padding);
	    എങ്കിൽ (row == മുകൾ && col >= വലത് - 2)
		തിരികെ FALSE;
	    എങ്കിൽ (row == bot - 1 && col < pum_col + 2 - ഇടത്_padding - pum_border
		    - ഇടത്_margin)
		തിരികെ FALSE;
	}
    }
    തിരികെ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ *
pum_compute_text_attrs(char_u *text, hlf_T hlf, പൂർണ്ണസംഖ്യ user_hlattr)
{
    പൂർണ്ണസംഖ്യ		i;
    size_t	leader_len;
    പൂർണ്ണസംഖ്യ		char_cells;
    പൂർണ്ണസംഖ്യ		new_attr;
    char_u	*ptr = text;
    പൂർണ്ണസംഖ്യ		cell_idx = 0;
    garray_T	*ga = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		*attrs = ശൂന്യ;
    char_u	*leader = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		in_fuzzy;
    പൂർണ്ണസംഖ്യ		matched_len = -1;
    int_u	char_pos = 0;
    പൂർണ്ണസംഖ്യ		is_select = FALSE;

    എങ്കിൽ (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	തിരികെ ശൂന്യ;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    എങ്കിൽ (leader == ശൂന്യ || *leader == NUL)
	തിരികെ ശൂന്യ;

    attrs = ALLOC_MULT(പൂർണ്ണസംഖ്യ, vim_strsize(text));
    എങ്കിൽ (attrs == ശൂന്യ)
	തിരികെ ശൂന്യ;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    എങ്കിൽ (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	എങ്കിൽ (!ga)
	{
	    vim_free(attrs);
	    തിരികെ ശൂന്യ;
	}
    }

    ആകുമ്പോൾ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	എങ്കിൽ (ga != ശൂന്യ)
	{
	    // Handle fuzzy matching
	    വേണ്ടി (i = 0; i < ga->ga_len; i++)
	    {
		എങ്കിൽ (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (പൂർണ്ണസംഖ്യ)leader_len;
	    എങ്കിൽ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	എങ്കിൽ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	വേണ്ടി (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    എങ്കിൽ (ga != ശൂന്യ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    തിരികെ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    സ്ഥിതം ശൂന്യം
pum_screen_puts_with_attrs(
    പൂർണ്ണസംഖ്യ		row,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		cells UNUSED,
    char_u	*text,
    പൂർണ്ണസംഖ്യ		textlen,
    പൂർണ്ണസംഖ്യ		*attrs)
{
    പൂർണ്ണസംഖ്യ		col_start = col;
    char_u	*ptr = text;
    പൂർണ്ണസംഖ്യ		char_len;
    പൂർണ്ണസംഖ്യ		attr;

    // Render text with proper attributes
    ആകുമ്പോൾ (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	അല്ലെങ്കിൽ
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    സ്ഥിതം വരിയിൽ char_u *
pum_get_item(പൂർണ്ണസംഖ്യ index, പൂർണ്ണസംഖ്യ type)
{
    തിരഞ്ഞെടുപ്പ്(type)
    {
	അവസ്ഥ CPT_ABBR: തിരികെ pum_array[index].pum_text;
	അവസ്ഥ CPT_KIND: തിരികെ pum_array[index].pum_kind;
	അവസ്ഥ CPT_MENU: തിരികെ pum_array[index].pum_extra;
    }
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ
pum_user_attr_combine(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ attr)
{
    പൂർണ്ണസംഖ്യ user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    തിരികെ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_display_rtl_text(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ     col,
	char_u  *st,
	പൂർണ്ണസംഖ്യ     attr,
	പൂർണ്ണസംഖ്യ     *attrs,
	പൂർണ്ണസംഖ്യ     width,
	പൂർണ്ണസംഖ്യ     width_limit,
	പൂർണ്ണസംഖ്യ     totwidth,
	പൂർണ്ണസംഖ്യ     next_isempty,
	പൂർണ്ണസംഖ്യ	selected)
{
    char_u  *rt = ശൂന്യ;
    പൂർണ്ണസംഖ്യ     cells = 0;
    പൂർണ്ണസംഖ്യ     over_cell = 0;
    പൂർണ്ണസംഖ്യ     truncated = FALSE;
    പൂർണ്ണസംഖ്യ     pad = next_isempty ? 0 : 2;
    പൂർണ്ണസംഖ്യ     remaining = 0;
    പൂർണ്ണസംഖ്യ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    പൂർണ്ണസംഖ്യ	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    എങ്കിൽ (st == ശൂന്യ)
	തിരികെ col;

    rt = reverse_text(st);
    എങ്കിൽ (rt == ശൂന്യ)
    {
	VIM_CLEAR(st);
	തിരികെ col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    എങ്കിൽ (cells > width_limit)
    {
	ചെയ്യുക
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ആകുമ്പോൾ (cells > width_limit);

	എങ്കിൽ (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    എങ്കിൽ (truncated)
    {
	char_u  *orig_rt = rt;
	പൂർണ്ണസംഖ്യ     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	എങ്കിൽ (cells > remaining)
	{
	    ആകുമ്പോൾ (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (പൂർണ്ണസംഖ്യ)STRLEN(orig_rt);
	എങ്കിൽ (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	എങ്കിൽ (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    എങ്കിൽ (attrs == ശൂന്യ)
	screen_puts_len(rt, (പൂർണ്ണസംഖ്യ)STRLEN(rt), row, col - cells + 1, attr);
    അല്ലെങ്കിൽ
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(പൂർണ്ണസംഖ്യ)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    തിരികെ col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_display_ltr_text(
	പൂർണ്ണസംഖ്യ     row,
	പൂർണ്ണസംഖ്യ     col,
	char_u  *st,
	പൂർണ്ണസംഖ്യ     attr,
	പൂർണ്ണസംഖ്യ     *attrs,
	പൂർണ്ണസംഖ്യ     width,        // width already calculated in outer loop
	പൂർണ്ണസംഖ്യ     width_limit,
	പൂർണ്ണസംഖ്യ     totwidth,
	പൂർണ്ണസംഖ്യ     next_isempty,
	പൂർണ്ണസംഖ്യ	selected)
{
    പൂർണ്ണസംഖ്യ     size = 0;
    പൂർണ്ണസംഖ്യ     cells = 0;
    char_u  *st_end = ശൂന്യ;
    പൂർണ്ണസംഖ്യ     over_cell = 0;
    പൂർണ്ണസംഖ്യ     pad = next_isempty ? 0 : 2;
    പൂർണ്ണസംഖ്യ     truncated = FALSE;
    പൂർണ്ണസംഖ്യ     remaining = 0;
    പൂർണ്ണസംഖ്യ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    പൂർണ്ണസംഖ്യ	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    എങ്കിൽ (st == ശൂന്യ)
	തിരികെ col;

    size = (പൂർണ്ണസംഖ്യ)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    ആകുമ്പോൾ (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	എങ്കിൽ (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	അല്ലെങ്കിൽ
	    --cells;
    }

    // truncated
    എങ്കിൽ (truncated)
    {
	remaining = width_limit - totwidth - 1;
	എങ്കിൽ (cells > remaining)
	{
	    st_end = st + size;
	    ആകുമ്പോൾ (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	എങ്കിൽ (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    എങ്കിൽ (attrs == ശൂന്യ)
	screen_puts_len(st, size, row, col, attr);
    അല്ലെങ്കിൽ
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    എങ്കിൽ (truncated)
    {
	എങ്കിൽ (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    തിരികെ col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_process_item(
	പൂർണ്ണസംഖ്യ     row,
	പൂർണ്ണസംഖ്യ     col,
	പൂർണ്ണസംഖ്യ     idx,
	പൂർണ്ണസംഖ്യ     j,         // Current position in order array
	പൂർണ്ണസംഖ്യ     *order,    // Order array
	hlf_T   hlf,
	പൂർണ്ണസംഖ്യ     attr,
	പൂർണ്ണസംഖ്യ     *totwidth_ptr,
	പൂർണ്ണസംഖ്യ     next_isempty)
{
    പൂർണ്ണസംഖ്യ     item_type = order[j];
    char_u  *s = ശൂന്യ;
    char_u  *p = pum_get_item(idx, item_type);
    പൂർണ്ണസംഖ്യ     width = 0;  // item width
    പൂർണ്ണസംഖ്യ     w;		// char width
    പൂർണ്ണസംഖ്യ	    selected = idx == pum_selected;

    വേണ്ടി ( ; ; MB_PTR_ADV(p))
    {
	എങ്കിൽ (s == ശൂന്യ)
	    s = p;
	w = ptr2cells(p);
	എങ്കിൽ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    തുടരുക;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	പൂർണ്ണസംഖ്യ     *attrs = ശൂന്യ;
	പൂർണ്ണസംഖ്യ     saved = *p;

	എങ്കിൽ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	എങ്കിൽ (saved != NUL)
	    *p = saved;

	എങ്കിൽ (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	അല്ലെങ്കിൽ
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	എങ്കിൽ (attrs != ശൂന്യ)
	    VIM_CLEAR(attrs);

	എങ്കിൽ (*p != TAB)
	    നിർത്തുക;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	അല്ലെങ്കിൽ
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = ശൂന്യ;  // start text at next char
	width = 0;
    }

    തിരികെ col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    സ്ഥിതം ശൂന്യം
pum_draw_scrollbar(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ	i,
	പൂർണ്ണസംഖ്യ	thumb_pos,
	പൂർണ്ണസംഖ്യ	thumb_height)
{
    എങ്കിൽ (pum_scrollbar <= 0)
	തിരികെ;

    പൂർണ്ണസംഖ്യ attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    അല്ലെങ്കിൽ
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    സ്ഥിതം വരിയിൽ ശൂന്യം
pum_align_order(പൂർണ്ണസംഖ്യ *order)
{
    പൂർണ്ണസംഖ്യ is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

സ്ഥിതം ശൂന്യം pum_free_bg(ശൂന്യം);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    ശൂന്യം
pum_opacity_changed(ശൂന്യം)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    എങ്കിൽ (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    സ്ഥിതം ശൂന്യം
pum_free_bg(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_വരികൾ);
    VIM_CLEAR(pum_bg_വരികൾ_UC);
    വേണ്ടി (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_വരികൾ_C[k]);
    pum_bg_മുകൾ = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    ശൂന്യം
pum_redraw(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		row = pum_row;
    പൂർണ്ണസംഖ്യ		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    പൂർണ്ണസംഖ്യ		attr;
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		idx;
    char_u	*p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		totwidth;
    പൂർണ്ണസംഖ്യ		thumb_pos = 0;
    പൂർണ്ണസംഖ്യ		thumb_height = 1;
    പൂർണ്ണസംഖ്യ		item_type;
    പൂർണ്ണസംഖ്യ		order[3];
    പൂർണ്ണസംഖ്യ		next_isempty = FALSE;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		ഇനങ്ങൾ_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    പൂർണ്ണസംഖ്യ		basic_width;  // first item width
    പൂർണ്ണസംഖ്യ		last_isabbr = FALSE;
    പൂർണ്ണസംഖ്യ		orig_attr = -1;
    പൂർണ്ണസംഖ്യ		scroll_range = pum_size - pum_height;
    bool	override_success;
    പൂർണ്ണസംഖ്യ		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = തള്ളുക_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    എങ്കിൽ (pum_imagep == ശൂന്യ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ശൂന്യ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    എങ്കിൽ (pum_imagep != ശൂന്യ)
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

    എങ്കിൽ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	എങ്കിൽ (opacity_active && pum_bg_വരികൾ != ശൂന്യ
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	എങ്കിൽ (opacity_active && pum_bg_വരികൾ != ശൂന്യ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    എങ്കിൽ (Screen_വരികൾ != ശൂന്യ && ScreenAttrs != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ	save_മുകൾ, save_bot, save_ncells, k;

		pum_free_bg();
		save_മുകൾ = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		എങ്കിൽ (save_മുകൾ < 0)
		    save_മുകൾ = 0;
		എങ്കിൽ (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_മുകൾ = save_മുകൾ;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		എങ്കിൽ (save_മുകൾ < save_bot)
		{
		    save_ncells = (save_bot - save_മുകൾ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_വരികൾ = LALLOC_MULT(schar_T, save_ncells);
		    എങ്കിൽ (enc_utf8)
		    {
			pum_bg_വരികൾ_UC = LALLOC_MULT(u8char_T, save_ncells);
			വേണ്ടി (k = 0; k < MAX_MCO; ++k)
			    pum_bg_വരികൾ_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    എങ്കിൽ (pum_bg_attrs != ശൂന്യ && pum_bg_വരികൾ != ശൂന്യ)
		    {
			വേണ്ടി (പൂർണ്ണസംഖ്യ r = save_മുകൾ; r < save_bot; ++r)
			{
			    പൂർണ്ണസംഖ്യ soff = (r - save_മുകൾ) * screen_Columns;
			    പൂർണ്ണസംഖ്യ loff = വരി_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * വലുപ്പം(sattr_T));
			    mch_memmove(pum_bg_വരികൾ + soff,
				    Screen_വരികൾ + loff,
				    screen_Columns * വലുപ്പം(schar_T));
			    എങ്കിൽ (enc_utf8 && pum_bg_വരികൾ_UC != ശൂന്യ
						&& Screen_വരികൾ_UC != ശൂന്യ)
			    {
				mch_memmove(pum_bg_വരികൾ_UC + soff,
					Screen_വരികൾ_UC + loff,
					screen_Columns * വലുപ്പം(u8char_T));
				വേണ്ടി (k = 0; k < MAX_MCO; ++k)
				    എങ്കിൽ (pum_bg_വരികൾ_C[k] != ശൂന്യ
						    && Screen_വരികൾ_C[k] != ശൂന്യ)
					mch_memmove(pum_bg_വരികൾ_C[k] + soff,
						Screen_വരികൾ_C[k] + loff,
						screen_Columns
						    * വലുപ്പം(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	അല്ലെങ്കിൽ
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

    എങ്കിൽ (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	എങ്കിൽ (thumb_height == 0)
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
    എങ്കിൽ (pum_border)
	pum_draw_border();
    എങ്കിൽ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    എങ്കിൽ (opacity_active)
	screen_pum_blend = 100 - (പൂർണ്ണസംഖ്യ)p_po;

    വേണ്ടി (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	{
	    എങ്കിൽ (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = ഇനങ്ങൾ_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	വേണ്ടി (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    എങ്കിൽ (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    എങ്കിൽ (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == ശൂന്യ;
	    അല്ലെങ്കിൽ
		next_isempty = TRUE;

	    എങ്കിൽ (p != ശൂന്യ)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    എങ്കിൽ (j > 0)
		n = ഇനങ്ങൾ_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    അല്ലെങ്കിൽ
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    എങ്കിൽ (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == ശൂന്യ)))
		    || basic_width + n >= pum_width)
		നിർത്തുക;
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	അല്ലെങ്കിൽ
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

    എങ്കിൽ (override_success)
	എടുക്കുക_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    ശൂന്യം
pum_position_info_popup(win_T *wp)
{
    പൂർണ്ണസംഖ്യ col = pum_col + pum_width + pum_scrollbar + 1;
    പൂർണ്ണസംഖ്യ row = pum_row;
    പൂർണ്ണസംഖ്യ botpos = POPPOS_BOTLEFT;
    പൂർണ്ണസംഖ്യ	used_maxwidth_opt = FALSE;
    പൂർണ്ണസംഖ്യ	വലത്_margin, ഇടത്_margin, dummy;
    പൂർണ്ണസംഖ്യ ഇടത്_extra_width, വലത്_extra_width;

    (ശൂന്യം)compute_margins(&വലത്_margin, &ഇടത്_margin, &dummy);

    ഇടത്_extra_width = pum_border + ഇടത്_margin;
    വലത്_extra_width = pum_border + വലത്_margin + (pum_shadow ? 2 : 0);
    col += വലത്_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    എങ്കിൽ (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - ഇടത്_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - ഇടത്_extra_width;
    }
    അല്ലെങ്കിൽ
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    എങ്കിൽ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_മുകൾ_extra(wp);
    എങ്കിൽ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	എങ്കിൽ (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	അല്ലെങ്കിൽ
	    // menu below cursor line, align with top
	    row += 1;
    }
    അല്ലെങ്കിൽ
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    എങ്കിൽ (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_set_selected(പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ repeat UNUSED)
{
    പൂർണ്ണസംഖ്യ	    resized = FALSE;
    പൂർണ്ണസംഖ്യ	    context = pum_height / 2;
    പൂർണ്ണസംഖ്യ	    scroll_offset;
#ifdef FEAT_QUICKFIX
    പൂർണ്ണസംഖ്യ	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    പൂർണ്ണസംഖ്യ	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    എങ്കിൽ (pum_selected >= 0 && pum_selected < pum_size)
    {
	എങ്കിൽ (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    എങ്കിൽ (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		എങ്കിൽ (pum_first < 0)
		    pum_first = 0;
		അല്ലെങ്കിൽ എങ്കിൽ (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    അല്ലെങ്കിൽ
		pum_first = pum_selected;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    എങ്കിൽ (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    അല്ലെങ്കിൽ
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	എങ്കിൽ (pum_height > 2)
	{
	    എങ്കിൽ (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    അല്ലെങ്കിൽ എങ്കിൽ (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#എങ്കിൽ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	എങ്കിൽ (pum_array[pum_selected].pum_info != ശൂന്യ
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# അല്ലെങ്കിൽ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    എങ്കിൽ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    അല്ലെങ്കിൽ എങ്കിൽ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    അല്ലെങ്കിൽ
		use_popup = USEPOPUP_NONE;
	    എങ്കിൽ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    എങ്കിൽ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    എങ്കിൽ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    എങ്കിൽ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		പൂർണ്ണസംഖ്യ	res = OK;

		എങ്കിൽ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ശൂന്യ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ആകുമ്പോൾ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		അല്ലെങ്കിൽ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_ONE, 0, ശൂന്യ);
		    --no_u_sync;
		    എങ്കിൽ (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_മൂല്യം_give_err((char_u *)"swf",
							  0L, ശൂന്യ, OPT_LOCAL);
			set_option_മൂല്യം_give_err((char_u *)"bl",
							  0L, ശൂന്യ, OPT_LOCAL);
			set_option_മൂല്യം_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_മൂല്യം_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_മൂല്യം_give_err((char_u *)"diff",
							  0L, ശൂന്യ, OPT_LOCAL);
		    }
		}
		എങ്കിൽ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    വേണ്ടി (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			എങ്കിൽ (e == ശൂന്യ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    നിർത്തുക;
			}
			*e = NUL;
			ml_append(lnum++, p, (പൂർണ്ണസംഖ്യ)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_വരി_എണ്ണം);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    എങ്കിൽ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			എങ്കിൽ (curwin->w_height < lnum)
			{
			    win_setheight((പൂർണ്ണസംഖ്യ)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    എങ്കിൽ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_topline > curbuf->b_ml.ml_വരി_എണ്ണം)
			curwin->w_topline = curbuf->b_ml.ml_വരി_എണ്ണം;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    എങ്കിൽ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			എങ്കിൽ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    എങ്കിൽ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			പൂർണ്ണസംഖ്യ save_redr_status;

			എങ്കിൽ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			എങ്കിൽ (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			എങ്കിൽ (resized && win_valid(curwin_save))
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

			എങ്കിൽ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    എങ്കിൽ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    എങ്കിൽ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    എങ്കിൽ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    എങ്കിൽ (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    തിരികെ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    ശൂന്യം
pum_undisplay(ശൂന്യം)
{
#ifdef FEAT_IMAGE_POPUP
    എങ്കിൽ (pum_imagep != ശൂന്യ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ശൂന്യ;
    }
#endif
    pum_free_bg();
    pum_array = ശൂന്യ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    എങ്കിൽ (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    ശൂന്യം
pum_clear(ശൂന്യം)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    പൂർണ്ണസംഖ്യ
pum_visible(ശൂന്യം)
{
    തിരികെ !pum_pretend_not_visible && pum_array != ശൂന്യ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pum_in_same_position(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    തിരികെ pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    പൂർണ്ണസംഖ്യ
pum_redraw_in_same_position(ശൂന്യം)
{
    എങ്കിൽ (!pum_visible() || pum_will_redraw)
	തിരികെ FALSE;  // nothing to do

    തിരികെ pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    ശൂന്യം
pum_may_redraw(ശൂന്യം)
{
    pumitem_T	*array = pum_array;
    പൂർണ്ണസംഖ്യ		len = pum_size;
    പൂർണ്ണസംഖ്യ		selected = pum_selected;

    എങ്കിൽ (!pum_visible() || pum_will_redraw)
	തിരികെ;  // nothing to do

    എങ്കിൽ (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
pum_get_height(ശൂന്യം)
{
    തിരികെ pum_height;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    ശൂന്യം
pum_set_event_info(dict_T *dict)
{
    എങ്കിൽ (!pum_visible())
	തിരികെ;
    (ശൂന്യം)dict_add_സംഖ്യ(dict, "height", pum_height);
    (ശൂന്യം)dict_add_സംഖ്യ(dict, "width", pum_width);
    (ശൂന്യം)dict_add_സംഖ്യ(dict, "row", pum_row);
    (ശൂന്യം)dict_add_സംഖ്യ(dict, "col", pum_col);
    (ശൂന്യം)dict_add_സംഖ്യ(dict, "size", pum_size);
    (ശൂന്യം)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#എങ്കിൽ defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    സ്ഥിതം ശൂന്യം
pum_position_at_mouse(പൂർണ്ണസംഖ്യ min_width)
{
    പൂർണ്ണസംഖ്യ extra_height = pum_border * 2 + pum_shadow;
    പൂർണ്ണസംഖ്യ extra_offset = pum_border + pum_shadow;
    പൂർണ്ണസംഖ്യ trailing_overhead = 0;
    പൂർണ്ണസംഖ്യ col_offset = 0;
    പൂർണ്ണസംഖ്യ available_width;

    എങ്കിൽ (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	എങ്കിൽ (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	എങ്കിൽ (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	എങ്കിൽ (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	എങ്കിൽ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	അല്ലെങ്കിൽ
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    അല്ലെങ്കിൽ
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	എങ്കിൽ (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	അല്ലെങ്കിൽ
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = ശൂന്യ;
}

#endif

#എങ്കിൽ defined(FEAT_BEVAL_TERM)
സ്ഥിതം pumitem_T *balloon_array = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

തരനിർവചനം ഘടന {
    char_u	*start;
    പൂർണ്ണസംഖ്യ		bytelen;
    പൂർണ്ണസംഖ്യ		cells;
    പൂർണ്ണസംഖ്യ		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    പൂർണ്ണസംഖ്യ
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    പൂർണ്ണസംഖ്യ		quoted = FALSE;
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		വരി;
    പൂർണ്ണസംഖ്യ		item_idx;
    പൂർണ്ണസംഖ്യ		indent = 0;
    പൂർണ്ണസംഖ്യ		max_cells = 0;
    പൂർണ്ണസംഖ്യ		max_height = Rows / 2 - 1;
    പൂർണ്ണസംഖ്യ		long_item_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		split_long_ഇനങ്ങൾ = FALSE;

    ga_init2(&ga, വലുപ്പം(balpart_T), 20);
    p = mesg;

    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    പോകുക failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	ആകുമ്പോൾ (*p != NUL)
	{
	    എങ്കിൽ (*p == '"')
		quoted = !quoted;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '\n')
		നിർത്തുക;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' && p[1] != NUL)
		++p;
	    അല്ലെങ്കിൽ എങ്കിൽ (!quoted)
	    {
		എങ്കിൽ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    എങ്കിൽ (*p == '{')
			++indent;
		    അല്ലെങ്കിൽ എങ്കിൽ (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    നിർത്തുക;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	എങ്കിൽ (*p == '\n')
	    ++p;
	എങ്കിൽ (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_എണ്ണം += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    എങ്കിൽ (long_item_എണ്ണം > 0 && height + long_item_എണ്ണം <= max_height)
    {
	split_long_ഇനങ്ങൾ = TRUE;
	height += long_item_എണ്ണം;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    എങ്കിൽ (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    എങ്കിൽ (*array == ശൂന്യ)
	പോകുക failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    വേണ്ടി (വരി = 1, item_idx = 0; വരി < height - 1; ++item_idx)
    {
	പൂർണ്ണസംഖ്യ	skip;
	പൂർണ്ണസംഖ്യ	thislen;
	പൂർണ്ണസംഖ്യ	copylen;
	പൂർണ്ണസംഖ്യ	ind;
	പൂർണ്ണസംഖ്യ	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	എങ്കിൽ (item->bytelen == 0)
	    (*array)[വരി++].pum_text = vim_strsave((char_u *)"");
	അല്ലെങ്കിൽ
	    വേണ്ടി (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		എങ്കിൽ (split_long_ഇനങ്ങൾ && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    വേണ്ടി (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			എങ്കിൽ ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    നിർത്തുക;
		    thislen = p - (item->start + skip);
		}
		അല്ലെങ്കിൽ
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		എങ്കിൽ (p == ശൂന്യ)
		{
		    വേണ്ടി (വരി = 0; വരി <= height - 1; ++വരി)
			vim_free((*array)[വരി].pum_text);
		    vim_free(*array);
		    പോകുക failed;
		}
		വേണ്ടി (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		വേണ്ടി (copylen = thislen; copylen > 0; --copylen)
		    എങ്കിൽ (item->start[skip + copylen - 1] != ' ')
			നിർത്തുക;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[വരി].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++വരി;
	    }
    }
    ga_clear(&ga);
    തിരികെ height;

failed:
    ga_clear(&ga);
    തിരികെ 0;
}

    ശൂന്യം
ui_remove_balloon(ശൂന്യം)
{
    എങ്കിൽ (balloon_array == ശൂന്യ)
	തിരികെ;

    pum_undisplay();
    ആകുമ്പോൾ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    ശൂന്യം
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    എങ്കിൽ (mesg == ശൂന്യ && list == ശൂന്യ)
    {
	pum_undisplay();
	തിരികെ;
    }
    എങ്കിൽ (list != ശൂന്യ)
    {
	listitem_T  *li;
	പൂർണ്ണസംഖ്യ	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	എങ്കിൽ (balloon_array == ശൂന്യ)
	    തിരികെ;
	CHECK_LIST_MATERIALIZE(list);
	വേണ്ടി (idx = 0, li = list->lv_first; li != ശൂന്യ; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == ശൂന്യ ? (char_u *)"" : text);
	}
    }
    അല്ലെങ്കിൽ
	balloon_arraysize = split_message(mesg, &balloon_array);

    എങ്കിൽ (balloon_arraysize <= 0)
	തിരികെ;

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
    ശൂന്യം
ui_may_remove_balloon(ശൂന്യം)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#എങ്കിൽ defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    സ്ഥിതം ശൂന്യം
pum_select_mouse_pos(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ idx = mouse_row - pum_row;

    എങ്കിൽ (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    അല്ലെങ്കിൽ എങ്കിൽ (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    സ്ഥിതം ശൂന്യം
pum_execute_menu(vimmenu_T *menu, പൂർണ്ണസംഖ്യ mode)
{
    vimmenu_T   *mp;
    പൂർണ്ണസംഖ്യ		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	എങ്കിൽ ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    നിർത്തുക;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    ശൂന്യം
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    പൂർണ്ണസംഖ്യ		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    പൂർണ്ണസംഖ്യ		save_bevalterm = p_bevalterm;
# endif
    പൂർണ്ണസംഖ്യ		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	എങ്കിൽ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    എങ്കിൽ (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	തിരികെ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    എങ്കിൽ (array == ശൂന്യ)
	തിരികെ;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = ശൂന്യ;

	// Make a copy of the text, the menu may be redefined in a callback.
	എങ്കിൽ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	അല്ലെങ്കിൽ എങ്കിൽ (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	എങ്കിൽ (s != ശൂന്യ)
	{
	    s = vim_strsave(s);
	    എങ്കിൽ (s != ശൂന്യ)
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

    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	എങ്കിൽ (c == ESC || c == Ctrl_C || pum_array == ശൂന്യ)
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ആകുമ്പോൾ (pum_selected > 0)
	    {
		--pum_selected;
		എങ്കിൽ (*array[pum_selected].pum_text != NUL)
		    നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ആകുമ്പോൾ (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		എങ്കിൽ (*array[pum_selected].pum_text != NUL)
		    നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    എങ്കിൽ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		നിർത്തുക;
	    }
	    എങ്കിൽ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		നിർത്തുക;
	}
    }

    വേണ്ടി (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    ശൂന്യം
pum_make_popup(char_u *path_name, പൂർണ്ണസംഖ്യ use_mouse_pos)
{
    vimmenu_T *menu;

    എങ്കിൽ (!use_mouse_pos)
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
    എങ്കിൽ (menu != ശൂന്യ)
	pum_show_popupmenu(menu);
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_singlewidth(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c >= 0 && c < 0x80)
	തിരികെ TRUE;
    എങ്കിൽ (utf_char2cells(c) == 1)
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    ശൂന്യം
pum_set_border_chars(
	പൂർണ്ണസംഖ്യ മുകൾ,
	പൂർണ്ണസംഖ്യ വലത്,
	പൂർണ്ണസംഖ്യ bottom,
	പൂർണ്ണസംഖ്യ ഇടത്,
	പൂർണ്ണസംഖ്യ മുകൾ_ഇടത്,
	പൂർണ്ണസംഖ്യ മുകൾ_വലത്,
	പൂർണ്ണസംഖ്യ bottom_വലത്,
	പൂർണ്ണസംഖ്യ bottom_ഇടത്)
{
    എങ്കിൽ (!is_singlewidth(മുകൾ) || !is_singlewidth(വലത്)
	    || !is_singlewidth(bottom) || !is_singlewidth(ഇടത്)
	    || !is_singlewidth(മുകൾ_ഇടത്) || !is_singlewidth(മുകൾ_വലത്)
	    || !is_singlewidth(bottom_വലത്) || !is_singlewidth(bottom_ഇടത്))
	തിരികെ;
    pum_border_chars.മുകൾ = മുകൾ;
    pum_border_chars.വലത് = വലത്;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ഇടത് = ഇടത്;
    pum_border_chars.മുകൾ_ഇടത് = മുകൾ_ഇടത്;
    pum_border_chars.മുകൾ_വലത് = മുകൾ_വലത്;
    pum_border_chars.bottom_വലത് = bottom_വലത്;
    pum_border_chars.bottom_ഇടത് = bottom_ഇടത്;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    സ്ഥിതം ശൂന്യം
compute_margins(പൂർണ്ണസംഖ്യ *വലത്_margin, പൂർണ്ണസംഖ്യ *ഇടത്_margin, പൂർണ്ണസംഖ്യ *ഇടത്_padding)
{
    *വലത്_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	*ഇടത്_padding = (pum_col < Columns - 1 - pum_border);
	*ഇടത്_margin = *വലത്_margin && (pum_col < Columns - 3);
	തിരികെ;
    }
#endif
    *ഇടത്_padding = (pum_col > pum_border);
    *ഇടത്_margin = *വലത്_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    സ്ഥിതം ശൂന്യം
pum_draw_border(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	മുകൾ_row = pum_row - 1;
    പൂർണ്ണസംഖ്യ	bottom_row = pum_row + pum_height;
    പൂർണ്ണസംഖ്യ	col;
    പൂർണ്ണസംഖ്യ	attr = highlight_attr[HLF_PMB];
    പൂർണ്ണസംഖ്യ	വലത്_margin, ഇടത്_margin, ഇടത്_padding;
    പൂർണ്ണസംഖ്യ	inner_width;

    എങ്കിൽ (!pum_border)
	തിരികെ;

    (ശൂന്യം)compute_margins(&വലത്_margin, &ഇടത്_margin, &ഇടത്_padding);

    inner_width = pum_width + pum_scrollbar + ഇടത്_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
	col = pum_col + ഇടത്_padding + 1;
    അല്ലെങ്കിൽ
#endif
	col = pum_col - ഇടത്_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	screen_putchar(pum_border_chars.മുകൾ_വലത്, മുകൾ_row, col, attr);
	എങ്കിൽ (ഇടത്_margin)
	    screen_putchar(' ', മുകൾ_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_വലത്, bottom_row, col, attr);
	എങ്കിൽ (ഇടത്_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    അല്ലെങ്കിൽ
#endif
    {
	screen_putchar(pum_border_chars.മുകൾ_ഇടത്, മുകൾ_row, col, attr);
	എങ്കിൽ (ഇടത്_margin)
	    screen_putchar(' ', മുകൾ_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_ഇടത്, bottom_row, col, attr);
	എങ്കിൽ (ഇടത്_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < inner_width; i++)
    {
	പൂർണ്ണസംഖ്യ cur_col;
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    cur_col = col - 1 - i;
	അല്ലെങ്കിൽ
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.മുകൾ, മുകൾ_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	screen_putchar(pum_border_chars.മുകൾ_ഇടത്, മുകൾ_row,
		col - inner_width - 1, attr);
	എങ്കിൽ (വലത്_margin)
	    screen_putchar(' ', മുകൾ_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_ഇടത്, bottom_row,
		col - inner_width - 1, attr);
	എങ്കിൽ (വലത്_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    അല്ലെങ്കിൽ
#endif
    {
	screen_putchar(pum_border_chars.മുകൾ_വലത്, മുകൾ_row,
		col + inner_width + 1, attr);
	എങ്കിൽ (വലത്_margin)
	    screen_putchar(' ', മുകൾ_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_വലത്, bottom_row,
		col + inner_width + 1, attr);
	എങ്കിൽ (വലത്_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pum_height; i++)
    {
	പൂർണ്ണസംഖ്യ row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	{
	    screen_putchar(pum_border_chars.ഇടത്, row, col, attr);
	    എങ്കിൽ (ഇടത്_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.വലത്, row,
		    col - inner_width - 1, attr);
	    എങ്കിൽ (വലത്_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	അല്ലെങ്കിൽ
#endif
	{
	    screen_putchar(pum_border_chars.ഇടത്, row, col, attr);
	    എങ്കിൽ (ഇടത്_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.വലത്, row,
		    col + inner_width + 1, attr);
	    എങ്കിൽ (വലത്_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    ശൂന്യം
put_shadow_char(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    char_u  buf[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ	    shadow_attr = highlight_attr[HLF_PMS];
    പൂർണ്ണസംഖ്യ	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    സ്ഥിതം ശൂന്യം
pum_draw_shadow(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	row, col, next_col;
    പൂർണ്ണസംഖ്യ	width, offset;
    പൂർണ്ണസംഖ്യ	inner_width;
    പൂർണ്ണസംഖ്യ	വലത്_margin, ഇടത്_margin, ഇടത്_padding;

    എങ്കിൽ (!pum_shadow)
	തിരികെ;

    (ശൂന്യം)compute_margins(&വലത്_margin, &ഇടത്_margin, &ഇടത്_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + വലത്_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    അല്ലെങ്കിൽ
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + ഇടത്_padding + pum_border + ഇടത്_margin;
    offset = 2 - ഇടത്_padding - pum_border - ഇടത്_margin;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (pum_rl)
	    col = pum_col - offset - i;
	അല്ലെങ്കിൽ
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
