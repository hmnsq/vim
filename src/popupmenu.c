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
#அந்தர்பாவ "vim.h"

நிலையான pumitem_T *pum_array = வெற்று;	// items of displayed pum
நிலையான முழு pum_size;			// nr of items in "pum_array"
நிலையான முழு pum_selected;		// index of selected item or -1
நிலையான முழு pum_first = 0;		// index of top item

நிலையான முழு call_update_screen = FALSE;
நிலையான முழு pum_in_cmdline = FALSE;

நிலையான முழு pum_height;			// nr of displayed pum items
நிலையான முழு pum_width;			// width of displayed pum items
நிலையான முழு pum_base_width;		// width of pum items base
நிலையான முழு pum_kind_width;		// width of pum items kind column
நிலையான முழு pum_extra_width;		// width of extra stuff
நிலையான முழு pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
நிலையான முழு pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

நிலையான முழு pum_row;			// top row of pum
நிலையான முழு pum_col;			// left column of pum

நிலையான win_T *pum_window = வெற்று;
நிலையான முழு pum_win_row;
நிலையான முழு pum_win_height;
நிலையான முழு pum_win_col;
நிலையான முழு pum_win_wcol;
நிலையான முழு pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
நிலையான முழு pum_pretend_not_visible = FALSE;

// Border support
நிலையான முழு pum_border = 0;
நிலையான முழு pum_margin = 0;	// margin of 1 cell on left and right
நிலையான முழு pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
நிலையான image_placement_T *pum_imagep = வெற்று;
#endif

// Border characters
நிலையான கட்டமைப்பு {
    முழு உச்சி;
    முழு வலது;
    முழு bottom;
    முழு இடது;
    முழு உச்சி_இடது;
    முழு உச்சி_வலது;
    முழு bottom_வலது;
    முழு bottom_இடது;
} pum_border_chars;

நிலையான முழு pum_set_selected(முழு n, முழு repeat);
நிலையான வெறுமை pum_draw_border(வெறுமை);
நிலையான வெறுமை pum_draw_shadow(வெறுமை);
நிலையான வெறுமை compute_margins(முழு *வலது_margin, முழு *இடது_margin, முழு *இடது_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	வெறுமை \
    pum_set_##feature(முழு enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    நிலையான வெறுமை
pum_compute_size(வெறுமை)
{
    முழு	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    க்கு (முழு i = 0; i < pum_size; ++i)
    {
	எனில் (pum_array[i].pum_text != வெற்று)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    எனில் (pum_base_width < w)
		pum_base_width = w;
	}
	எனில் (pum_array[i].pum_kind != வெற்று)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    எனில் (pum_kind_width < w)
		pum_kind_width = w;
	}
	எனில் (pum_array[i].pum_extra != வெற்று)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    எனில் (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    நிலையான வெறுமை
pum_compute_vertical_placement(
	முழு size,
	முழு above_row,
	முழு below_row)
{
    முழு context_வரிகள்;
    முழு cline_visible_offset;
    முழு extra_height = 2 * pum_border + pum_shadow;
    முழு extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    எனில் (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    எனில் (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	எனில் (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_வரிகள் = 0;
	இல்லையெனில்
	    // Leave two lines of context if possible
	    context_வரிகள் = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	எனில் (pum_win_row >= size + context_வரிகள் + extra_height)
	{
	    pum_row = pum_win_row - size - context_வரிகள் - extra_offset;
	    pum_height = size;
	}
	இல்லையெனில்
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_வரிகள் - extra_height;
	}
	எனில் (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    இல்லையெனில்
    {
	// pum below "pum_win_row"
	எனில் (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_வரிகள் = 0;
	இல்லையெனில்
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_வரிகள் = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_வரிகள் + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	எனில் (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#எனில் defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    எனில் (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    நிலையான முழு
set_pum_width_aligned_with_cursor(முழு width, முழு available_width)
{
    முழு end_padding = TRUE;

    எனில் (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    எனில் (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    திரும்பு available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    நிலையான வெறுமை
pum_compute_horizontal_placement(முழு cursor_col)
{
    முழு desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    முழு available_width;
    முழு col_offset;
    முழு trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    இல்லையெனில்
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    எனில் (set_pum_width_aligned_with_cursor(desired_width, available_width))
	திரும்பு;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    எனில் (available_width > p_pw)
    {
	pum_width = available_width;
	திரும்பு;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    எனில் (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	இல்லையெனில்
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	திரும்பு;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
	pum_col = Columns - 1 - col_offset;
    இல்லையெனில்
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
    வெறுமை
pum_display(
	pumitem_T   *array,
	முழு	    size,
	முழு	    selected,   // index of initially selected item, -1 if
				// out of range
	முழு	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    முழு	    cursor_col;
    முழு	    above_row;
    முழு	    below_row;
    முழு	    redo_மொத்தம் = 0;
#எனில் defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    செய்
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = வெற்று;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	எனில் (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	இல்லையெனில்
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#எனில் defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    எனில் (pvwin->w_p_pvw)
		நிறுத்து;
	எனில் (pvwin != வெற்று)
	{
	    எனில் (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    இல்லையெனில் எனில் (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	எனில் (pum_height < 1 || (pum_height == 1 && size > 1))
	    திரும்பு;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	எனில் (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	இல்லையெனில்
	{
	    முழு wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    எனில் (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    இல்லையெனில்
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    எனில் (curwin->w_p_cole > 0 && conceal_cursor_வரி(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		எனில் (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    எனில் (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    இல்லையெனில்
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
    } போது (pum_set_selected(selected, redo_மொத்தம்) && ++redo_மொத்தம் <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    வெறுமை
pum_call_update_screen(வெறுமை)
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
    முழு
pum_under_menu(முழு row, முழு col, முழு only_redrawing)
{
    முழு	extra_இடது = pum_border + (pum_margin && pum_border ? 1 : 0);
    முழு	extra_வலது = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    முழு	extra_above = pum_border;
    முழு	extra_below = pum_border + (pum_shadow ? 1 : 0);
    முழு	உச்சி = pum_row - extra_above;
    முழு	bot = pum_row + pum_height + extra_below;
    முழு	இடது = pum_col - 1 - extra_இடது;
    முழு	வலது = pum_col + pum_width + pum_scrollbar + extra_வலது;

    எனில் (!((!only_redrawing || pum_will_redraw)
	    && row >= உச்சி && row < bot && col >= இடது && col < வலது))
	திரும்பு FALSE;

    எனில் (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	எனில் (only_redrawing)
	{
	    எனில் (col >= வலது - 2 || row == bot - 1)
		திரும்பு FALSE;
	}
	இல்லையெனில்
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    முழு	    வலது_margin, இடது_margin, இடது_padding;

	    compute_margins(&வலது_margin, &இடது_margin, &இடது_padding);
	    எனில் (row == உச்சி && col >= வலது - 2)
		திரும்பு FALSE;
	    எனில் (row == bot - 1 && col < pum_col + 2 - இடது_padding - pum_border
		    - இடது_margin)
		திரும்பு FALSE;
	}
    }
    திரும்பு TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    நிலையான முழு *
pum_compute_text_attrs(char_u *text, hlf_T hlf, முழு user_hlattr)
{
    முழு		i;
    size_t	leader_len;
    முழு		char_cells;
    முழு		new_attr;
    char_u	*ptr = text;
    முழு		cell_idx = 0;
    garray_T	*ga = வெற்று;
    முழு		*attrs = வெற்று;
    char_u	*leader = வெற்று;
    முழு		in_fuzzy;
    முழு		matched_len = -1;
    int_u	char_pos = 0;
    முழு		is_select = FALSE;

    எனில் (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	திரும்பு வெற்று;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    எனில் (leader == வெற்று || *leader == NUL)
	திரும்பு வெற்று;

    attrs = ALLOC_MULT(முழு, vim_strsize(text));
    எனில் (attrs == வெற்று)
	திரும்பு வெற்று;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    எனில் (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	எனில் (!ga)
	{
	    vim_free(attrs);
	    திரும்பு வெற்று;
	}
    }

    போது (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	எனில் (ga != வெற்று)
	{
	    // Handle fuzzy matching
	    க்கு (i = 0; i < ga->ga_len; i++)
	    {
		எனில் (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில்
	{
	    எனில் (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (முழு)leader_len;
	    எனில் (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	எனில் (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	க்கு (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    எனில் (ga != வெற்று)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    திரும்பு attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    நிலையான வெறுமை
pum_screen_puts_with_attrs(
    முழு		row,
    முழு		col,
    முழு		cells UNUSED,
    char_u	*text,
    முழு		textlen,
    முழு		*attrs)
{
    முழு		col_start = col;
    char_u	*ptr = text;
    முழு		char_len;
    முழு		attr;

    // Render text with proper attributes
    போது (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	இல்லையெனில்
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    நிலையான உள்வரி char_u *
pum_get_item(முழு index, முழு type)
{
    தேர்வு(type)
    {
	நிலை CPT_ABBR: திரும்பு pum_array[index].pum_text;
	நிலை CPT_KIND: திரும்பு pum_array[index].pum_kind;
	நிலை CPT_MENU: திரும்பு pum_array[index].pum_extra;
    }
    திரும்பு வெற்று;
}

    நிலையான உள்வரி முழு
pum_user_attr_combine(முழு idx, முழு type, முழு attr)
{
    முழு user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    திரும்பு user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    நிலையான முழு
pum_display_rtl_text(
	முழு	row,
	முழு     col,
	char_u  *st,
	முழு     attr,
	முழு     *attrs,
	முழு     width,
	முழு     width_limit,
	முழு     totwidth,
	முழு     next_isempty,
	முழு	selected)
{
    char_u  *rt = வெற்று;
    முழு     cells = 0;
    முழு     over_cell = 0;
    முழு     truncated = FALSE;
    முழு     pad = next_isempty ? 0 : 2;
    முழு     remaining = 0;
    முழு	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    முழு	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    எனில் (st == வெற்று)
	திரும்பு col;

    rt = reverse_text(st);
    எனில் (rt == வெற்று)
    {
	VIM_CLEAR(st);
	திரும்பு col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    எனில் (cells > width_limit)
    {
	செய்
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} போது (cells > width_limit);

	எனில் (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    எனில் (truncated)
    {
	char_u  *orig_rt = rt;
	முழு     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	எனில் (cells > remaining)
	{
	    போது (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (முழு)STRLEN(orig_rt);
	எனில் (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	எனில் (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    எனில் (attrs == வெற்று)
	screen_puts_len(rt, (முழு)STRLEN(rt), row, col - cells + 1, attr);
    இல்லையெனில்
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(முழு)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    திரும்பு col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    நிலையான முழு
pum_display_ltr_text(
	முழு     row,
	முழு     col,
	char_u  *st,
	முழு     attr,
	முழு     *attrs,
	முழு     width,        // width already calculated in outer loop
	முழு     width_limit,
	முழு     totwidth,
	முழு     next_isempty,
	முழு	selected)
{
    முழு     size = 0;
    முழு     cells = 0;
    char_u  *st_end = வெற்று;
    முழு     over_cell = 0;
    முழு     pad = next_isempty ? 0 : 2;
    முழு     truncated = FALSE;
    முழு     remaining = 0;
    முழு	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    முழு	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    எனில் (st == வெற்று)
	திரும்பு col;

    size = (முழு)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    போது (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	எனில் (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	இல்லையெனில்
	    --cells;
    }

    // truncated
    எனில் (truncated)
    {
	remaining = width_limit - totwidth - 1;
	எனில் (cells > remaining)
	{
	    st_end = st + size;
	    போது (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	எனில் (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    எனில் (attrs == வெற்று)
	screen_puts_len(st, size, row, col, attr);
    இல்லையெனில்
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    எனில் (truncated)
    {
	எனில் (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    திரும்பு col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    நிலையான முழு
pum_process_item(
	முழு     row,
	முழு     col,
	முழு     idx,
	முழு     j,         // Current position in order array
	முழு     *order,    // Order array
	hlf_T   hlf,
	முழு     attr,
	முழு     *totwidth_ptr,
	முழு     next_isempty)
{
    முழு     item_type = order[j];
    char_u  *s = வெற்று;
    char_u  *p = pum_get_item(idx, item_type);
    முழு     width = 0;  // item width
    முழு     w;		// char width
    முழு	    selected = idx == pum_selected;

    க்கு ( ; ; MB_PTR_ADV(p))
    {
	எனில் (s == வெற்று)
	    s = p;
	w = ptr2cells(p);
	எனில் (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    தொடர்;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	முழு     *attrs = வெற்று;
	முழு     saved = *p;

	எனில் (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	எனில் (saved != NUL)
	    *p = saved;

	எனில் (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	இல்லையெனில்
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	எனில் (attrs != வெற்று)
	    VIM_CLEAR(attrs);

	எனில் (*p != TAB)
	    நிறுத்து;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	இல்லையெனில்
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = வெற்று;  // start text at next char
	width = 0;
    }

    திரும்பு col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    நிலையான வெறுமை
pum_draw_scrollbar(
	முழு	row,
	முழு	i,
	முழு	thumb_pos,
	முழு	thumb_height)
{
    எனில் (pum_scrollbar <= 0)
	திரும்பு;

    முழு attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    இல்லையெனில்
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    நிலையான உள்வரி வெறுமை
pum_align_order(முழு *order)
{
    முழு is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

நிலையான வெறுமை pum_free_bg(வெறுமை);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    வெறுமை
pum_opacity_changed(வெறுமை)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    எனில் (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    நிலையான வெறுமை
pum_free_bg(வெறுமை)
{
    முழு k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_வரிகள்);
    VIM_CLEAR(pum_bg_வரிகள்_UC);
    க்கு (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_வரிகள்_C[k]);
    pum_bg_உச்சி = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    வெறுமை
pum_redraw(வெறுமை)
{
    முழு		row = pum_row;
    முழு		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    முழு		attr;
    முழு		i, j;
    முழு		idx;
    char_u	*p = வெற்று;
    முழு		totwidth;
    முழு		thumb_pos = 0;
    முழு		thumb_height = 1;
    முழு		item_type;
    முழு		order[3];
    முழு		next_isempty = FALSE;
    முழு		n;
    முழு		பொருட்கள்_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    முழு		basic_width;  // first item width
    முழு		last_isabbr = FALSE;
    முழு		orig_attr = -1;
    முழு		scroll_range = pum_size - pum_height;
    bool	override_success;
    முழு		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = தள்ளு_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    எனில் (pum_imagep == வெற்று)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(வெற்று, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    எனில் (pum_imagep != வெற்று)
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

    எனில் (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	எனில் (opacity_active && pum_bg_வரிகள் != வெற்று
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	எனில் (opacity_active && pum_bg_வரிகள் != வெற்று)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	இல்லையெனில் எனில் (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    எனில் (Screen_வரிகள் != வெற்று && ScreenAttrs != வெற்று)
	    {
		முழு	save_உச்சி, save_bot, save_ncells, k;

		pum_free_bg();
		save_உச்சி = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		எனில் (save_உச்சி < 0)
		    save_உச்சி = 0;
		எனில் (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_உச்சி = save_உச்சி;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		எனில் (save_உச்சி < save_bot)
		{
		    save_ncells = (save_bot - save_உச்சி) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_வரிகள் = LALLOC_MULT(schar_T, save_ncells);
		    எனில் (enc_utf8)
		    {
			pum_bg_வரிகள்_UC = LALLOC_MULT(u8char_T, save_ncells);
			க்கு (k = 0; k < MAX_MCO; ++k)
			    pum_bg_வரிகள்_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    எனில் (pum_bg_attrs != வெற்று && pum_bg_வரிகள் != வெற்று)
		    {
			க்கு (முழு r = save_உச்சி; r < save_bot; ++r)
			{
			    முழு soff = (r - save_உச்சி) * screen_Columns;
			    முழு loff = வரி_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * அளவு(sattr_T));
			    mch_memmove(pum_bg_வரிகள் + soff,
				    Screen_வரிகள் + loff,
				    screen_Columns * அளவு(schar_T));
			    எனில் (enc_utf8 && pum_bg_வரிகள்_UC != வெற்று
						&& Screen_வரிகள்_UC != வெற்று)
			    {
				mch_memmove(pum_bg_வரிகள்_UC + soff,
					Screen_வரிகள்_UC + loff,
					screen_Columns * அளவு(u8char_T));
				க்கு (k = 0; k < MAX_MCO; ++k)
				    எனில் (pum_bg_வரிகள்_C[k] != வெற்று
						    && Screen_வரிகள்_C[k] != வெற்று)
					mch_memmove(pum_bg_வரிகள்_C[k] + soff,
						Screen_வரிகள்_C[k] + loff,
						screen_Columns
						    * அளவு(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	இல்லையெனில்
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

    எனில் (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	எனில் (thumb_height == 0)
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
    எனில் (pum_border)
	pum_draw_border();
    எனில் (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    எனில் (opacity_active)
	screen_pum_blend = 100 - (முழு)p_po;

    க்கு (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	{
	    எனில் (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	இல்லையெனில்
#endif
	    எனில் (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = பொருட்கள்_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	க்கு (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    எனில் (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    எனில் (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == வெற்று;
	    இல்லையெனில்
		next_isempty = TRUE;

	    எனில் (p != வெற்று)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    எனில் (j > 0)
		n = பொருட்கள்_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    இல்லையெனில்
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    எனில் (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == வெற்று)))
		    || basic_width + n >= pum_width)
		நிறுத்து;
#ifdef FEAT_RIGHTLEFT
	    எனில் (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    இல்லையெனில்
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	இல்லையெனில்
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

    எனில் (override_success)
	எடு_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    வெறுமை
pum_position_info_popup(win_T *wp)
{
    முழு col = pum_col + pum_width + pum_scrollbar + 1;
    முழு row = pum_row;
    முழு botpos = POPPOS_BOTLEFT;
    முழு	used_maxwidth_opt = FALSE;
    முழு	வலது_margin, இடது_margin, dummy;
    முழு இடது_extra_width, வலது_extra_width;

    (வெறுமை)compute_margins(&வலது_margin, &இடது_margin, &dummy);

    இடது_extra_width = pum_border + இடது_margin;
    வலது_extra_width = pum_border + வலது_margin + (pum_shadow ? 2 : 0);
    col += வலது_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    எனில் (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - இடது_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - இடது_extra_width;
    }
    இல்லையெனில்
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    எனில் (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_உச்சி_extra(wp);
    எனில் (wp->w_popup_flags & POPF_INFO_MENU)
    {
	எனில் (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	இல்லையெனில்
	    // menu below cursor line, align with top
	    row += 1;
    }
    இல்லையெனில்
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    எனில் (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    இல்லையெனில்
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
    நிலையான முழு
pum_set_selected(முழு n, முழு repeat UNUSED)
{
    முழு	    resized = FALSE;
    முழு	    context = pum_height / 2;
    முழு	    scroll_offset;
#ifdef FEAT_QUICKFIX
    முழு	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    முழு	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    எனில் (pum_selected >= 0 && pum_selected < pum_size)
    {
	எனில் (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    எனில் (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		எனில் (pum_first < 0)
		    pum_first = 0;
		இல்லையெனில் எனில் (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    இல்லையெனில்
		pum_first = pum_selected;
	}
	இல்லையெனில் எனில் (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    எனில் (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    இல்லையெனில்
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	எனில் (pum_height > 2)
	{
	    எனில் (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    இல்லையெனில் எனில் (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#எனில் defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	எனில் (pum_array[pum_selected].pum_info != வெற்று
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# இல்லையெனில்
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    எனில் (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    இல்லையெனில் எனில் (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    இல்லையெனில்
		use_popup = USEPOPUP_NONE;
	    எனில் (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    எனில் (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    எனில் (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    எனில் (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		முழு	res = OK;

		எனில் (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == வெற்று
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    போது (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		இல்லையெனில்
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, வெற்று, வெற்று, வெற்று, ECMD_ONE, 0, வெற்று);
		    --no_u_sync;
		    எனில் (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_மதிப்பு_give_err((char_u *)"swf",
							  0L, வெற்று, OPT_LOCAL);
			set_option_மதிப்பு_give_err((char_u *)"bl",
							  0L, வெற்று, OPT_LOCAL);
			set_option_மதிப்பு_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_மதிப்பு_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_மதிப்பு_give_err((char_u *)"diff",
							  0L, வெற்று, OPT_LOCAL);
		    }
		}
		எனில் (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    க்கு (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			எனில் (e == வெற்று)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    நிறுத்து;
			}
			*e = NUL;
			ml_append(lnum++, p, (முழு)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_வரி_மொத்தம்);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    எனில் (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			எனில் (curwin->w_height < lnum)
			{
			    win_setheight((முழு)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    எனில் (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    இல்லையெனில் எனில் (curwin->w_topline > curbuf->b_ml.ml_வரி_மொத்தம்)
			curwin->w_topline = curbuf->b_ml.ml_வரி_மொத்தம்;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    எனில் (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			எனில் (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    எனில் ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			முழு save_redr_status;

			எனில் (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			எனில் (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			எனில் (resized && win_valid(curwin_save))
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

			எனில் (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    எனில் (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    எனில் (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    எனில் (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    எனில் (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    திரும்பு resized;
}

/*
 * Undisplay the popup menu (later).
 */
    வெறுமை
pum_undisplay(வெறுமை)
{
#ifdef FEAT_IMAGE_POPUP
    எனில் (pum_imagep != வெற்று)
    {
	image_placement_free(pum_imagep);
	pum_imagep = வெற்று;
    }
#endif
    pum_free_bg();
    pum_array = வெற்று;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    எனில் (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    வெறுமை
pum_clear(வெறுமை)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    முழு
pum_visible(வெறுமை)
{
    திரும்பு !pum_pretend_not_visible && pum_array != வெற்று;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    நிலையான முழு
pum_in_same_position(வெறுமை)
{
    முழு	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    திரும்பு pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    முழு
pum_redraw_in_same_position(வெறுமை)
{
    எனில் (!pum_visible() || pum_will_redraw)
	திரும்பு FALSE;  // nothing to do

    திரும்பு pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    வெறுமை
pum_may_redraw(வெறுமை)
{
    pumitem_T	*array = pum_array;
    முழு		len = pum_size;
    முழு		selected = pum_selected;

    எனில் (!pum_visible() || pum_will_redraw)
	திரும்பு;  // nothing to do

    எனில் (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    இல்லையெனில்
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
    முழு
pum_get_height(வெறுமை)
{
    திரும்பு pum_height;
}

#எனில் defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    வெறுமை
pum_set_event_info(dict_T *dict)
{
    எனில் (!pum_visible())
	திரும்பு;
    (வெறுமை)dict_add_எண்(dict, "height", pum_height);
    (வெறுமை)dict_add_எண்(dict, "width", pum_width);
    (வெறுமை)dict_add_எண்(dict, "row", pum_row);
    (வெறுமை)dict_add_எண்(dict, "col", pum_col);
    (வெறுமை)dict_add_எண்(dict, "size", pum_size);
    (வெறுமை)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#எனில் defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    நிலையான வெறுமை
pum_position_at_mouse(முழு min_width)
{
    முழு extra_height = pum_border * 2 + pum_shadow;
    முழு extra_offset = pum_border + pum_shadow;
    முழு trailing_overhead = 0;
    முழு col_offset = 0;
    முழு available_width;

    எனில் (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	எனில் (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	எனில் (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    இல்லையெனில்
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	எனில் (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	எனில் (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	இல்லையெனில்
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    இல்லையெனில்
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	எனில் (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	இல்லையெனில்
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = வெற்று;
}

#endif

#எனில் defined(FEAT_BEVAL_TERM)
நிலையான pumitem_T *balloon_array = வெற்று;
நிலையான முழு balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

வகைவரை கட்டமைப்பு {
    char_u	*start;
    முழு		bytelen;
    முழு		cells;
    முழு		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    முழு
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    முழு		quoted = FALSE;
    முழு		height;
    முழு		வரி;
    முழு		item_idx;
    முழு		indent = 0;
    முழு		max_cells = 0;
    முழு		max_height = Rows / 2 - 1;
    முழு		long_item_மொத்தம் = 0;
    முழு		split_long_பொருட்கள் = FALSE;

    ga_init2(&ga, அளவு(balpart_T), 20);
    p = mesg;

    போது (*p != NUL)
    {
	எனில் (ga_grow(&ga, 1) == FAIL)
	    செல் failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	போது (*p != NUL)
	{
	    எனில் (*p == '"')
		quoted = !quoted;
	    இல்லையெனில் எனில் (*p == '\n')
		நிறுத்து;
	    இல்லையெனில் எனில் (*p == '\\' && p[1] != NUL)
		++p;
	    இல்லையெனில் எனில் (!quoted)
	    {
		எனில் ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    எனில் (*p == '{')
			++indent;
		    இல்லையெனில் எனில் (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    நிறுத்து;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	எனில் (*p == '\n')
	    ++p;
	எனில் (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_மொத்தம் += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    எனில் (long_item_மொத்தம் > 0 && height + long_item_மொத்தம் <= max_height)
    {
	split_long_பொருட்கள் = TRUE;
	height += long_item_மொத்தம்;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    எனில் (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    எனில் (*array == வெற்று)
	செல் failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    க்கு (வரி = 1, item_idx = 0; வரி < height - 1; ++item_idx)
    {
	முழு	skip;
	முழு	thislen;
	முழு	copylen;
	முழு	ind;
	முழு	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	எனில் (item->bytelen == 0)
	    (*array)[வரி++].pum_text = vim_strsave((char_u *)"");
	இல்லையெனில்
	    க்கு (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		எனில் (split_long_பொருட்கள் && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    க்கு (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			எனில் ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    நிறுத்து;
		    thislen = p - (item->start + skip);
		}
		இல்லையெனில்
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		எனில் (p == வெற்று)
		{
		    க்கு (வரி = 0; வரி <= height - 1; ++வரி)
			vim_free((*array)[வரி].pum_text);
		    vim_free(*array);
		    செல் failed;
		}
		க்கு (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		க்கு (copylen = thislen; copylen > 0; --copylen)
		    எனில் (item->start[skip + copylen - 1] != ' ')
			நிறுத்து;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[வரி].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++வரி;
	    }
    }
    ga_clear(&ga);
    திரும்பு height;

failed:
    ga_clear(&ga);
    திரும்பு 0;
}

    வெறுமை
ui_remove_balloon(வெறுமை)
{
    எனில் (balloon_array == வெற்று)
	திரும்பு;

    pum_undisplay();
    போது (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    வெறுமை
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    எனில் (mesg == வெற்று && list == வெற்று)
    {
	pum_undisplay();
	திரும்பு;
    }
    எனில் (list != வெற்று)
    {
	listitem_T  *li;
	முழு	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	எனில் (balloon_array == வெற்று)
	    திரும்பு;
	CHECK_LIST_MATERIALIZE(list);
	க்கு (idx = 0, li = list->lv_first; li != வெற்று; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == வெற்று ? (char_u *)"" : text);
	}
    }
    இல்லையெனில்
	balloon_arraysize = split_message(mesg, &balloon_array);

    எனில் (balloon_arraysize <= 0)
	திரும்பு;

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
    வெறுமை
ui_may_remove_balloon(வெறுமை)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#எனில் defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    நிலையான வெறுமை
pum_select_mouse_pos(வெறுமை)
{
    முழு idx = mouse_row - pum_row;

    எனில் (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    இல்லையெனில் எனில் (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    நிலையான வெறுமை
pum_execute_menu(vimmenu_T *menu, முழு mode)
{
    vimmenu_T   *mp;
    முழு		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	எனில் ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    நிறுத்து;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    வெறுமை
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    முழு		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    முழு		save_bevalterm = p_bevalterm;
# endif
    முழு		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	எனில் (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    எனில் (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	திரும்பு;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    எனில் (array == வெற்று)
	திரும்பு;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = வெற்று;

	// Make a copy of the text, the menu may be redefined in a callback.
	எனில் (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	இல்லையெனில் எனில் (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	எனில் (s != வெற்று)
	{
	    s = vim_strsave(s);
	    எனில் (s != வெற்று)
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

    க்கு (;;)
    {
	முழு	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	எனில் (c == ESC || c == Ctrl_C || pum_array == வெற்று)
	    நிறுத்து;
	இல்லையெனில் எனில் (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    போது (pum_selected > 0)
	    {
		--pum_selected;
		எனில் (*array[pum_selected].pum_text != NUL)
		    நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    போது (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		எனில் (*array[pum_selected].pum_text != NUL)
		    நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	இல்லையெனில் எனில் (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    எனில் (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		நிறுத்து;
	    }
	    எனில் (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		நிறுத்து;
	}
    }

    க்கு (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    வெறுமை
pum_make_popup(char_u *path_name, முழு use_mouse_pos)
{
    vimmenu_T *menu;

    எனில் (!use_mouse_pos)
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
    எனில் (menu != வெற்று)
	pum_show_popupmenu(menu);
}
#endif

    நிலையான முழு
is_singlewidth(முழு c)
{
    எனில் (c >= 0 && c < 0x80)
	திரும்பு TRUE;
    எனில் (utf_char2cells(c) == 1)
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Set border characters for popup menu.
 */
    வெறுமை
pum_set_border_chars(
	முழு உச்சி,
	முழு வலது,
	முழு bottom,
	முழு இடது,
	முழு உச்சி_இடது,
	முழு உச்சி_வலது,
	முழு bottom_வலது,
	முழு bottom_இடது)
{
    எனில் (!is_singlewidth(உச்சி) || !is_singlewidth(வலது)
	    || !is_singlewidth(bottom) || !is_singlewidth(இடது)
	    || !is_singlewidth(உச்சி_இடது) || !is_singlewidth(உச்சி_வலது)
	    || !is_singlewidth(bottom_வலது) || !is_singlewidth(bottom_இடது))
	திரும்பு;
    pum_border_chars.உச்சி = உச்சி;
    pum_border_chars.வலது = வலது;
    pum_border_chars.bottom = bottom;
    pum_border_chars.இடது = இடது;
    pum_border_chars.உச்சி_இடது = உச்சி_இடது;
    pum_border_chars.உச்சி_வலது = உச்சி_வலது;
    pum_border_chars.bottom_வலது = bottom_வலது;
    pum_border_chars.bottom_இடது = bottom_இடது;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    நிலையான வெறுமை
compute_margins(முழு *வலது_margin, முழு *இடது_margin, முழு *இடது_padding)
{
    *வலது_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	*இடது_padding = (pum_col < Columns - 1 - pum_border);
	*இடது_margin = *வலது_margin && (pum_col < Columns - 3);
	திரும்பு;
    }
#endif
    *இடது_padding = (pum_col > pum_border);
    *இடது_margin = *வலது_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    நிலையான வெறுமை
pum_draw_border(வெறுமை)
{
    முழு	உச்சி_row = pum_row - 1;
    முழு	bottom_row = pum_row + pum_height;
    முழு	col;
    முழு	attr = highlight_attr[HLF_PMB];
    முழு	வலது_margin, இடது_margin, இடது_padding;
    முழு	inner_width;

    எனில் (!pum_border)
	திரும்பு;

    (வெறுமை)compute_margins(&வலது_margin, &இடது_margin, &இடது_padding);

    inner_width = pum_width + pum_scrollbar + இடது_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
	col = pum_col + இடது_padding + 1;
    இல்லையெனில்
#endif
	col = pum_col - இடது_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	screen_putchar(pum_border_chars.உச்சி_வலது, உச்சி_row, col, attr);
	எனில் (இடது_margin)
	    screen_putchar(' ', உச்சி_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_வலது, bottom_row, col, attr);
	எனில் (இடது_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    இல்லையெனில்
#endif
    {
	screen_putchar(pum_border_chars.உச்சி_இடது, உச்சி_row, col, attr);
	எனில் (இடது_margin)
	    screen_putchar(' ', உச்சி_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_இடது, bottom_row, col, attr);
	எனில் (இடது_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    க்கு (முழு i = 0; i < inner_width; i++)
    {
	முழு cur_col;
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    cur_col = col - 1 - i;
	இல்லையெனில்
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.உச்சி, உச்சி_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	screen_putchar(pum_border_chars.உச்சி_இடது, உச்சி_row,
		col - inner_width - 1, attr);
	எனில் (வலது_margin)
	    screen_putchar(' ', உச்சி_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_இடது, bottom_row,
		col - inner_width - 1, attr);
	எனில் (வலது_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    இல்லையெனில்
#endif
    {
	screen_putchar(pum_border_chars.உச்சி_வலது, உச்சி_row,
		col + inner_width + 1, attr);
	எனில் (வலது_margin)
	    screen_putchar(' ', உச்சி_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_வலது, bottom_row,
		col + inner_width + 1, attr);
	எனில் (வலது_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    க்கு (முழு i = 0; i < pum_height; i++)
    {
	முழு row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	{
	    screen_putchar(pum_border_chars.இடது, row, col, attr);
	    எனில் (இடது_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.வலது, row,
		    col - inner_width - 1, attr);
	    எனில் (வலது_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	இல்லையெனில்
#endif
	{
	    screen_putchar(pum_border_chars.இடது, row, col, attr);
	    எனில் (இடது_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.வலது, row,
		    col + inner_width + 1, attr);
	    எனில் (வலது_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    வெறுமை
put_shadow_char(முழு row, முழு col)
{
    char_u  buf[MB_MAXBYTES + 1];
    முழு	    shadow_attr = highlight_attr[HLF_PMS];
    முழு	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    நிலையான வெறுமை
pum_draw_shadow(வெறுமை)
{
    முழு	row, col, next_col;
    முழு	width, offset;
    முழு	inner_width;
    முழு	வலது_margin, இடது_margin, இடது_padding;

    எனில் (!pum_shadow)
	திரும்பு;

    (வெறுமை)compute_margins(&வலது_margin, &இடது_margin, &இடது_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + வலது_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    எனில் (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    இல்லையெனில்
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    க்கு (முழு i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + இடது_padding + pum_border + இடது_margin;
    offset = 2 - இடது_padding - pum_border - இடது_margin;

    க்கு (முழு i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	எனில் (pum_rl)
	    col = pum_col - offset - i;
	இல்லையெனில்
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
