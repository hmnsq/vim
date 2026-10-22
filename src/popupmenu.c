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
#અન્તર્ભાવ "vim.h"

સ્થિર pumitem_T *pum_array = ખાલી;	// items of displayed pum
સ્થિર પૂર્ણાંક pum_size;			// nr of items in "pum_array"
સ્થિર પૂર્ણાંક pum_selected;		// index of selected item or -1
સ્થિર પૂર્ણાંક pum_first = 0;		// index of top item

સ્થિર પૂર્ણાંક call_update_screen = FALSE;
સ્થિર પૂર્ણાંક pum_in_cmdline = FALSE;

સ્થિર પૂર્ણાંક pum_height;			// nr of displayed pum items
સ્થિર પૂર્ણાંક pum_width;			// width of displayed pum items
સ્થિર પૂર્ણાંક pum_base_width;		// width of pum items base
સ્થિર પૂર્ણાંક pum_kind_width;		// width of pum items kind column
સ્થિર પૂર્ણાંક pum_extra_width;		// width of extra stuff
સ્થિર પૂર્ણાંક pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
સ્થિર પૂર્ણાંક pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

સ્થિર પૂર્ણાંક pum_row;			// top row of pum
સ્થિર પૂર્ણાંક pum_col;			// left column of pum

સ્થિર win_T *pum_window = ખાલી;
સ્થિર પૂર્ણાંક pum_win_row;
સ્થિર પૂર્ણાંક pum_win_height;
સ્થિર પૂર્ણાંક pum_win_col;
સ્થિર પૂર્ણાંક pum_win_wcol;
સ્થિર પૂર્ણાંક pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
સ્થિર પૂર્ણાંક pum_pretend_not_visible = FALSE;

// Border support
સ્થિર પૂર્ણાંક pum_border = 0;
સ્થિર પૂર્ણાંક pum_margin = 0;	// margin of 1 cell on left and right
સ્થિર પૂર્ણાંક pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
સ્થિર image_placement_T *pum_imagep = ખાલી;
#endif

// Border characters
સ્થિર સંરચના {
    પૂર્ણાંક ટોચ;
    પૂર્ણાંક જમણું;
    પૂર્ણાંક bottom;
    પૂર્ણાંક ડાબું;
    પૂર્ણાંક ટોચ_ડાબું;
    પૂર્ણાંક ટોચ_જમણું;
    પૂર્ણાંક bottom_જમણું;
    પૂર્ણાંક bottom_ડાબું;
} pum_border_chars;

સ્થિર પૂર્ણાંક pum_set_selected(પૂર્ણાંક n, પૂર્ણાંક repeat);
સ્થિર શૂન્ય pum_draw_border(શૂન્ય);
સ્થિર શૂન્ય pum_draw_shadow(શૂન્ય);
સ્થિર શૂન્ય compute_margins(પૂર્ણાંક *જમણું_margin, પૂર્ણાંક *ડાબું_margin, પૂર્ણાંક *ડાબું_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	શૂન્ય \
    pum_set_##feature(પૂર્ણાંક enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    સ્થિર શૂન્ય
pum_compute_size(શૂન્ય)
{
    પૂર્ણાંક	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    માટે (પૂર્ણાંક i = 0; i < pum_size; ++i)
    {
	જો (pum_array[i].pum_text != ખાલી)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    જો (pum_base_width < w)
		pum_base_width = w;
	}
	જો (pum_array[i].pum_kind != ખાલી)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    જો (pum_kind_width < w)
		pum_kind_width = w;
	}
	જો (pum_array[i].pum_extra != ખાલી)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    જો (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    સ્થિર શૂન્ય
pum_compute_vertical_placement(
	પૂર્ણાંક size,
	પૂર્ણાંક above_row,
	પૂર્ણાંક below_row)
{
    પૂર્ણાંક context_લીટીઓ;
    પૂર્ણાંક cline_visible_offset;
    પૂર્ણાંક extra_height = 2 * pum_border + pum_shadow;
    પૂર્ણાંક extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    જો (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    જો (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	જો (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_લીટીઓ = 0;
	નહીંતર
	    // Leave two lines of context if possible
	    context_લીટીઓ = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	જો (pum_win_row >= size + context_લીટીઓ + extra_height)
	{
	    pum_row = pum_win_row - size - context_લીટીઓ - extra_offset;
	    pum_height = size;
	}
	નહીંતર
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_લીટીઓ - extra_height;
	}
	જો (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    નહીંતર
    {
	// pum below "pum_win_row"
	જો (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_લીટીઓ = 0;
	નહીંતર
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_લીટીઓ = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_લીટીઓ + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	જો (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#જો defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    જો (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    સ્થિર પૂર્ણાંક
set_pum_width_aligned_with_cursor(પૂર્ણાંક width, પૂર્ણાંક available_width)
{
    પૂર્ણાંક end_padding = TRUE;

    જો (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    જો (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    પાછા available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    સ્થિર શૂન્ય
pum_compute_horizontal_placement(પૂર્ણાંક cursor_col)
{
    પૂર્ણાંક desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    પૂર્ણાંક available_width;
    પૂર્ણાંક col_offset;
    પૂર્ણાંક trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    નહીંતર
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    જો (set_pum_width_aligned_with_cursor(desired_width, available_width))
	પાછા;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    જો (available_width > p_pw)
    {
	pum_width = available_width;
	પાછા;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    જો (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	નહીંતર
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	પાછા;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
	pum_col = Columns - 1 - col_offset;
    નહીંતર
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
    શૂન્ય
pum_display(
	pumitem_T   *array,
	પૂર્ણાંક	    size,
	પૂર્ણાંક	    selected,   // index of initially selected item, -1 if
				// out of range
	પૂર્ણાંક	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    પૂર્ણાંક	    cursor_col;
    પૂર્ણાંક	    above_row;
    પૂર્ણાંક	    below_row;
    પૂર્ણાંક	    redo_ગણતરી = 0;
#જો defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    કરો
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = ખાલી;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	જો (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	નહીંતર
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#જો defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    જો (pvwin->w_p_pvw)
		તોડો;
	જો (pvwin != ખાલી)
	{
	    જો (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    નહીંતર જો (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	જો (pum_height < 1 || (pum_height == 1 && size > 1))
	    પાછા;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	જો (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	નહીંતર
	{
	    પૂર્ણાંક wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    જો (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    નહીંતર
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    જો (curwin->w_p_cole > 0 && conceal_cursor_લીટી(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		જો (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    જો (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    નહીંતર
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
    } જ્યારે (pum_set_selected(selected, redo_ગણતરી) && ++redo_ગણતરી <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    શૂન્ય
pum_call_update_screen(શૂન્ય)
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
    પૂર્ણાંક
pum_under_menu(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક only_redrawing)
{
    પૂર્ણાંક	extra_ડાબું = pum_border + (pum_margin && pum_border ? 1 : 0);
    પૂર્ણાંક	extra_જમણું = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    પૂર્ણાંક	extra_above = pum_border;
    પૂર્ણાંક	extra_below = pum_border + (pum_shadow ? 1 : 0);
    પૂર્ણાંક	ટોચ = pum_row - extra_above;
    પૂર્ણાંક	bot = pum_row + pum_height + extra_below;
    પૂર્ણાંક	ડાબું = pum_col - 1 - extra_ડાબું;
    પૂર્ણાંક	જમણું = pum_col + pum_width + pum_scrollbar + extra_જમણું;

    જો (!((!only_redrawing || pum_will_redraw)
	    && row >= ટોચ && row < bot && col >= ડાબું && col < જમણું))
	પાછા FALSE;

    જો (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	જો (only_redrawing)
	{
	    જો (col >= જમણું - 2 || row == bot - 1)
		પાછા FALSE;
	}
	નહીંતર
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    પૂર્ણાંક	    જમણું_margin, ડાબું_margin, ડાબું_padding;

	    compute_margins(&જમણું_margin, &ડાબું_margin, &ડાબું_padding);
	    જો (row == ટોચ && col >= જમણું - 2)
		પાછા FALSE;
	    જો (row == bot - 1 && col < pum_col + 2 - ડાબું_padding - pum_border
		    - ડાબું_margin)
		પાછા FALSE;
	}
    }
    પાછા TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    સ્થિર પૂર્ણાંક *
pum_compute_text_attrs(char_u *text, hlf_T hlf, પૂર્ણાંક user_hlattr)
{
    પૂર્ણાંક		i;
    size_t	leader_len;
    પૂર્ણાંક		char_cells;
    પૂર્ણાંક		new_attr;
    char_u	*ptr = text;
    પૂર્ણાંક		cell_idx = 0;
    garray_T	*ga = ખાલી;
    પૂર્ણાંક		*attrs = ખાલી;
    char_u	*leader = ખાલી;
    પૂર્ણાંક		in_fuzzy;
    પૂર્ણાંક		matched_len = -1;
    int_u	char_pos = 0;
    પૂર્ણાંક		is_select = FALSE;

    જો (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	પાછા ખાલી;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    જો (leader == ખાલી || *leader == NUL)
	પાછા ખાલી;

    attrs = ALLOC_MULT(પૂર્ણાંક, vim_strsize(text));
    જો (attrs == ખાલી)
	પાછા ખાલી;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    જો (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	જો (!ga)
	{
	    vim_free(attrs);
	    પાછા ખાલી;
	}
    }

    જ્યારે (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	જો (ga != ખાલી)
	{
	    // Handle fuzzy matching
	    માટે (i = 0; i < ga->ga_len; i++)
	    {
		જો (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    તોડો;
		}
	    }
	}
	નહીંતર
	{
	    જો (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (પૂર્ણાંક)leader_len;
	    જો (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	જો (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	માટે (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    જો (ga != ખાલી)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    પાછા attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    સ્થિર શૂન્ય
pum_screen_puts_with_attrs(
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    પૂર્ણાંક		cells UNUSED,
    char_u	*text,
    પૂર્ણાંક		textlen,
    પૂર્ણાંક		*attrs)
{
    પૂર્ણાંક		col_start = col;
    char_u	*ptr = text;
    પૂર્ણાંક		char_len;
    પૂર્ણાંક		attr;

    // Render text with proper attributes
    જ્યારે (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	નહીંતર
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    સ્થિર અંતઃરેખ char_u *
pum_get_item(પૂર્ણાંક index, પૂર્ણાંક type)
{
    પસંદગી(type)
    {
	સ્થિતિ CPT_ABBR: પાછા pum_array[index].pum_text;
	સ્થિતિ CPT_KIND: પાછા pum_array[index].pum_kind;
	સ્થિતિ CPT_MENU: પાછા pum_array[index].pum_extra;
    }
    પાછા ખાલી;
}

    સ્થિર અંતઃરેખ પૂર્ણાંક
pum_user_attr_combine(પૂર્ણાંક idx, પૂર્ણાંક type, પૂર્ણાંક attr)
{
    પૂર્ણાંક user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    પાછા user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    સ્થિર પૂર્ણાંક
pum_display_rtl_text(
	પૂર્ણાંક	row,
	પૂર્ણાંક     col,
	char_u  *st,
	પૂર્ણાંક     attr,
	પૂર્ણાંક     *attrs,
	પૂર્ણાંક     width,
	પૂર્ણાંક     width_limit,
	પૂર્ણાંક     totwidth,
	પૂર્ણાંક     next_isempty,
	પૂર્ણાંક	selected)
{
    char_u  *rt = ખાલી;
    પૂર્ણાંક     cells = 0;
    પૂર્ણાંક     over_cell = 0;
    પૂર્ણાંક     truncated = FALSE;
    પૂર્ણાંક     pad = next_isempty ? 0 : 2;
    પૂર્ણાંક     remaining = 0;
    પૂર્ણાંક	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    પૂર્ણાંક	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    જો (st == ખાલી)
	પાછા col;

    rt = reverse_text(st);
    જો (rt == ખાલી)
    {
	VIM_CLEAR(st);
	પાછા col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    જો (cells > width_limit)
    {
	કરો
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} જ્યારે (cells > width_limit);

	જો (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    જો (truncated)
    {
	char_u  *orig_rt = rt;
	પૂર્ણાંક     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	જો (cells > remaining)
	{
	    જ્યારે (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (પૂર્ણાંક)STRLEN(orig_rt);
	જો (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	જો (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    જો (attrs == ખાલી)
	screen_puts_len(rt, (પૂર્ણાંક)STRLEN(rt), row, col - cells + 1, attr);
    નહીંતર
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(પૂર્ણાંક)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    પાછા col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    સ્થિર પૂર્ણાંક
pum_display_ltr_text(
	પૂર્ણાંક     row,
	પૂર્ણાંક     col,
	char_u  *st,
	પૂર્ણાંક     attr,
	પૂર્ણાંક     *attrs,
	પૂર્ણાંક     width,        // width already calculated in outer loop
	પૂર્ણાંક     width_limit,
	પૂર્ણાંક     totwidth,
	પૂર્ણાંક     next_isempty,
	પૂર્ણાંક	selected)
{
    પૂર્ણાંક     size = 0;
    પૂર્ણાંક     cells = 0;
    char_u  *st_end = ખાલી;
    પૂર્ણાંક     over_cell = 0;
    પૂર્ણાંક     pad = next_isempty ? 0 : 2;
    પૂર્ણાંક     truncated = FALSE;
    પૂર્ણાંક     remaining = 0;
    પૂર્ણાંક	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    પૂર્ણાંક	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    જો (st == ખાલી)
	પાછા col;

    size = (પૂર્ણાંક)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    જ્યારે (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	જો (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	નહીંતર
	    --cells;
    }

    // truncated
    જો (truncated)
    {
	remaining = width_limit - totwidth - 1;
	જો (cells > remaining)
	{
	    st_end = st + size;
	    જ્યારે (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	જો (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    જો (attrs == ખાલી)
	screen_puts_len(st, size, row, col, attr);
    નહીંતર
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    જો (truncated)
    {
	જો (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    પાછા col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    સ્થિર પૂર્ણાંક
pum_process_item(
	પૂર્ણાંક     row,
	પૂર્ણાંક     col,
	પૂર્ણાંક     idx,
	પૂર્ણાંક     j,         // Current position in order array
	પૂર્ણાંક     *order,    // Order array
	hlf_T   hlf,
	પૂર્ણાંક     attr,
	પૂર્ણાંક     *totwidth_ptr,
	પૂર્ણાંક     next_isempty)
{
    પૂર્ણાંક     item_type = order[j];
    char_u  *s = ખાલી;
    char_u  *p = pum_get_item(idx, item_type);
    પૂર્ણાંક     width = 0;  // item width
    પૂર્ણાંક     w;		// char width
    પૂર્ણાંક	    selected = idx == pum_selected;

    માટે ( ; ; MB_PTR_ADV(p))
    {
	જો (s == ખાલી)
	    s = p;
	w = ptr2cells(p);
	જો (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    ચાલુ;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	પૂર્ણાંક     *attrs = ખાલી;
	પૂર્ણાંક     saved = *p;

	જો (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	જો (saved != NUL)
	    *p = saved;

	જો (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	નહીંતર
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	જો (attrs != ખાલી)
	    VIM_CLEAR(attrs);

	જો (*p != TAB)
	    તોડો;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	નહીંતર
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = ખાલી;  // start text at next char
	width = 0;
    }

    પાછા col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    સ્થિર શૂન્ય
pum_draw_scrollbar(
	પૂર્ણાંક	row,
	પૂર્ણાંક	i,
	પૂર્ણાંક	thumb_pos,
	પૂર્ણાંક	thumb_height)
{
    જો (pum_scrollbar <= 0)
	પાછા;

    પૂર્ણાંક attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    નહીંતર
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    સ્થિર અંતઃરેખ શૂન્ય
pum_align_order(પૂર્ણાંક *order)
{
    પૂર્ણાંક is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

સ્થિર શૂન્ય pum_free_bg(શૂન્ય);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    શૂન્ય
pum_opacity_changed(શૂન્ય)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    જો (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    સ્થિર શૂન્ય
pum_free_bg(શૂન્ય)
{
    પૂર્ણાંક k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_લીટીઓ);
    VIM_CLEAR(pum_bg_લીટીઓ_UC);
    માટે (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_લીટીઓ_C[k]);
    pum_bg_ટોચ = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    શૂન્ય
pum_redraw(શૂન્ય)
{
    પૂર્ણાંક		row = pum_row;
    પૂર્ણાંક		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		idx;
    char_u	*p = ખાલી;
    પૂર્ણાંક		totwidth;
    પૂર્ણાંક		thumb_pos = 0;
    પૂર્ણાંક		thumb_height = 1;
    પૂર્ણાંક		item_type;
    પૂર્ણાંક		order[3];
    પૂર્ણાંક		next_isempty = FALSE;
    પૂર્ણાંક		n;
    પૂર્ણાંક		વસ્તુઓ_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    પૂર્ણાંક		basic_width;  // first item width
    પૂર્ણાંક		last_isabbr = FALSE;
    પૂર્ણાંક		orig_attr = -1;
    પૂર્ણાંક		scroll_range = pum_size - pum_height;
    bool	override_success;
    પૂર્ણાંક		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ધકેલો_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    જો (pum_imagep == ખાલી)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ખાલી, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    જો (pum_imagep != ખાલી)
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

    જો (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	જો (opacity_active && pum_bg_લીટીઓ != ખાલી
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	જો (opacity_active && pum_bg_લીટીઓ != ખાલી)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	નહીંતર જો (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    જો (Screen_લીટીઓ != ખાલી && ScreenAttrs != ખાલી)
	    {
		પૂર્ણાંક	save_ટોચ, save_bot, save_ncells, k;

		pum_free_bg();
		save_ટોચ = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		જો (save_ટોચ < 0)
		    save_ટોચ = 0;
		જો (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_ટોચ = save_ટોચ;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		જો (save_ટોચ < save_bot)
		{
		    save_ncells = (save_bot - save_ટોચ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_લીટીઓ = LALLOC_MULT(schar_T, save_ncells);
		    જો (enc_utf8)
		    {
			pum_bg_લીટીઓ_UC = LALLOC_MULT(u8char_T, save_ncells);
			માટે (k = 0; k < MAX_MCO; ++k)
			    pum_bg_લીટીઓ_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    જો (pum_bg_attrs != ખાલી && pum_bg_લીટીઓ != ખાલી)
		    {
			માટે (પૂર્ણાંક r = save_ટોચ; r < save_bot; ++r)
			{
			    પૂર્ણાંક soff = (r - save_ટોચ) * screen_Columns;
			    પૂર્ણાંક loff = લીટી_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * કદ(sattr_T));
			    mch_memmove(pum_bg_લીટીઓ + soff,
				    Screen_લીટીઓ + loff,
				    screen_Columns * કદ(schar_T));
			    જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી
						&& Screen_લીટીઓ_UC != ખાલી)
			    {
				mch_memmove(pum_bg_લીટીઓ_UC + soff,
					Screen_લીટીઓ_UC + loff,
					screen_Columns * કદ(u8char_T));
				માટે (k = 0; k < MAX_MCO; ++k)
				    જો (pum_bg_લીટીઓ_C[k] != ખાલી
						    && Screen_લીટીઓ_C[k] != ખાલી)
					mch_memmove(pum_bg_લીટીઓ_C[k] + soff,
						Screen_લીટીઓ_C[k] + loff,
						screen_Columns
						    * કદ(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	નહીંતર
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

    જો (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	જો (thumb_height == 0)
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
    જો (pum_border)
	pum_draw_border();
    જો (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    જો (opacity_active)
	screen_pum_blend = 100 - (પૂર્ણાંક)p_po;

    માટે (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	{
	    જો (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	નહીંતર
#endif
	    જો (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = વસ્તુઓ_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	માટે (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    જો (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    જો (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == ખાલી;
	    નહીંતર
		next_isempty = TRUE;

	    જો (p != ખાલી)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    જો (j > 0)
		n = વસ્તુઓ_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    નહીંતર
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    જો (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == ખાલી)))
		    || basic_width + n >= pum_width)
		તોડો;
#ifdef FEAT_RIGHTLEFT
	    જો (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    નહીંતર
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	નહીંતર
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

    જો (override_success)
	કાઢો_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    શૂન્ય
pum_position_info_popup(win_T *wp)
{
    પૂર્ણાંક col = pum_col + pum_width + pum_scrollbar + 1;
    પૂર્ણાંક row = pum_row;
    પૂર્ણાંક botpos = POPPOS_BOTLEFT;
    પૂર્ણાંક	used_maxwidth_opt = FALSE;
    પૂર્ણાંક	જમણું_margin, ડાબું_margin, dummy;
    પૂર્ણાંક ડાબું_extra_width, જમણું_extra_width;

    (શૂન્ય)compute_margins(&જમણું_margin, &ડાબું_margin, &dummy);

    ડાબું_extra_width = pum_border + ડાબું_margin;
    જમણું_extra_width = pum_border + જમણું_margin + (pum_shadow ? 2 : 0);
    col += જમણું_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    જો (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - ડાબું_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - ડાબું_extra_width;
    }
    નહીંતર
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    જો (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_ટોચ_extra(wp);
    જો (wp->w_popup_flags & POPF_INFO_MENU)
    {
	જો (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	નહીંતર
	    // menu below cursor line, align with top
	    row += 1;
    }
    નહીંતર
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    જો (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    નહીંતર
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
    સ્થિર પૂર્ણાંક
pum_set_selected(પૂર્ણાંક n, પૂર્ણાંક repeat UNUSED)
{
    પૂર્ણાંક	    resized = FALSE;
    પૂર્ણાંક	    context = pum_height / 2;
    પૂર્ણાંક	    scroll_offset;
#ifdef FEAT_QUICKFIX
    પૂર્ણાંક	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    પૂર્ણાંક	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    જો (pum_selected >= 0 && pum_selected < pum_size)
    {
	જો (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    જો (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		જો (pum_first < 0)
		    pum_first = 0;
		નહીંતર જો (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    નહીંતર
		pum_first = pum_selected;
	}
	નહીંતર જો (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    જો (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    નહીંતર
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	જો (pum_height > 2)
	{
	    જો (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    નહીંતર જો (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#જો defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	જો (pum_array[pum_selected].pum_info != ખાલી
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# નહીંતર
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    જો (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    નહીંતર જો (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    નહીંતર
		use_popup = USEPOPUP_NONE;
	    જો (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    જો (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    જો (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    જો (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		પૂર્ણાંક	res = OK;

		જો (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ખાલી
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    જ્યારે (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		નહીંતર
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ખાલી, ખાલી, ખાલી, ECMD_ONE, 0, ખાલી);
		    --no_u_sync;
		    જો (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_કિંમત_give_err((char_u *)"swf",
							  0L, ખાલી, OPT_LOCAL);
			set_option_કિંમત_give_err((char_u *)"bl",
							  0L, ખાલી, OPT_LOCAL);
			set_option_કિંમત_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_કિંમત_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_કિંમત_give_err((char_u *)"diff",
							  0L, ખાલી, OPT_LOCAL);
		    }
		}
		જો (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    માટે (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			જો (e == ખાલી)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    તોડો;
			}
			*e = NUL;
			ml_append(lnum++, p, (પૂર્ણાંક)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_લીટી_ગણતરી);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    જો (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			જો (curwin->w_height < lnum)
			{
			    win_setheight((પૂર્ણાંક)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    જો (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    નહીંતર જો (curwin->w_topline > curbuf->b_ml.ml_લીટી_ગણતરી)
			curwin->w_topline = curbuf->b_ml.ml_લીટી_ગણતરી;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    જો (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			જો (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    જો ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			પૂર્ણાંક save_redr_status;

			જો (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			જો (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			જો (resized && win_valid(curwin_save))
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

			જો (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    જો (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    જો (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    જો (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    જો (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    પાછા resized;
}

/*
 * Undisplay the popup menu (later).
 */
    શૂન્ય
pum_undisplay(શૂન્ય)
{
#ifdef FEAT_IMAGE_POPUP
    જો (pum_imagep != ખાલી)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ખાલી;
    }
#endif
    pum_free_bg();
    pum_array = ખાલી;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    જો (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    શૂન્ય
pum_clear(શૂન્ય)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    પૂર્ણાંક
pum_visible(શૂન્ય)
{
    પાછા !pum_pretend_not_visible && pum_array != ખાલી;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    સ્થિર પૂર્ણાંક
pum_in_same_position(શૂન્ય)
{
    પૂર્ણાંક	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    પાછા pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    પૂર્ણાંક
pum_redraw_in_same_position(શૂન્ય)
{
    જો (!pum_visible() || pum_will_redraw)
	પાછા FALSE;  // nothing to do

    પાછા pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    શૂન્ય
pum_may_redraw(શૂન્ય)
{
    pumitem_T	*array = pum_array;
    પૂર્ણાંક		len = pum_size;
    પૂર્ણાંક		selected = pum_selected;

    જો (!pum_visible() || pum_will_redraw)
	પાછા;  // nothing to do

    જો (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    નહીંતર
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
    પૂર્ણાંક
pum_get_height(શૂન્ય)
{
    પાછા pum_height;
}

#જો defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    શૂન્ય
pum_set_event_info(dict_T *dict)
{
    જો (!pum_visible())
	પાછા;
    (શૂન્ય)dict_add_સંખ્યા(dict, "height", pum_height);
    (શૂન્ય)dict_add_સંખ્યા(dict, "width", pum_width);
    (શૂન્ય)dict_add_સંખ્યા(dict, "row", pum_row);
    (શૂન્ય)dict_add_સંખ્યા(dict, "col", pum_col);
    (શૂન્ય)dict_add_સંખ્યા(dict, "size", pum_size);
    (શૂન્ય)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#જો defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    સ્થિર શૂન્ય
pum_position_at_mouse(પૂર્ણાંક min_width)
{
    પૂર્ણાંક extra_height = pum_border * 2 + pum_shadow;
    પૂર્ણાંક extra_offset = pum_border + pum_shadow;
    પૂર્ણાંક trailing_overhead = 0;
    પૂર્ણાંક col_offset = 0;
    પૂર્ણાંક available_width;

    જો (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	જો (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	જો (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    નહીંતર
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	જો (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	જો (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	નહીંતર
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    નહીંતર
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	જો (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	નહીંતર
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = ખાલી;
}

#endif

#જો defined(FEAT_BEVAL_TERM)
સ્થિર pumitem_T *balloon_array = ખાલી;
સ્થિર પૂર્ણાંક balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

પ્રકારવ્યાખ્યા સંરચના {
    char_u	*start;
    પૂર્ણાંક		bytelen;
    પૂર્ણાંક		cells;
    પૂર્ણાંક		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    પૂર્ણાંક
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    પૂર્ણાંક		quoted = FALSE;
    પૂર્ણાંક		height;
    પૂર્ણાંક		લીટી;
    પૂર્ણાંક		item_idx;
    પૂર્ણાંક		indent = 0;
    પૂર્ણાંક		max_cells = 0;
    પૂર્ણાંક		max_height = Rows / 2 - 1;
    પૂર્ણાંક		long_item_ગણતરી = 0;
    પૂર્ણાંક		split_long_વસ્તુઓ = FALSE;

    ga_init2(&ga, કદ(balpart_T), 20);
    p = mesg;

    જ્યારે (*p != NUL)
    {
	જો (ga_grow(&ga, 1) == FAIL)
	    જાઓ failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	જ્યારે (*p != NUL)
	{
	    જો (*p == '"')
		quoted = !quoted;
	    નહીંતર જો (*p == '\n')
		તોડો;
	    નહીંતર જો (*p == '\\' && p[1] != NUL)
		++p;
	    નહીંતર જો (!quoted)
	    {
		જો ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    જો (*p == '{')
			++indent;
		    નહીંતર જો (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    તોડો;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	જો (*p == '\n')
	    ++p;
	જો (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_ગણતરી += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    જો (long_item_ગણતરી > 0 && height + long_item_ગણતરી <= max_height)
    {
	split_long_વસ્તુઓ = TRUE;
	height += long_item_ગણતરી;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    જો (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    જો (*array == ખાલી)
	જાઓ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    માટે (લીટી = 1, item_idx = 0; લીટી < height - 1; ++item_idx)
    {
	પૂર્ણાંક	skip;
	પૂર્ણાંક	thislen;
	પૂર્ણાંક	copylen;
	પૂર્ણાંક	ind;
	પૂર્ણાંક	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	જો (item->bytelen == 0)
	    (*array)[લીટી++].pum_text = vim_strsave((char_u *)"");
	નહીંતર
	    માટે (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		જો (split_long_વસ્તુઓ && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    માટે (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			જો ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    તોડો;
		    thislen = p - (item->start + skip);
		}
		નહીંતર
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		જો (p == ખાલી)
		{
		    માટે (લીટી = 0; લીટી <= height - 1; ++લીટી)
			vim_free((*array)[લીટી].pum_text);
		    vim_free(*array);
		    જાઓ failed;
		}
		માટે (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		માટે (copylen = thislen; copylen > 0; --copylen)
		    જો (item->start[skip + copylen - 1] != ' ')
			તોડો;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[લીટી].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++લીટી;
	    }
    }
    ga_clear(&ga);
    પાછા height;

failed:
    ga_clear(&ga);
    પાછા 0;
}

    શૂન્ય
ui_remove_balloon(શૂન્ય)
{
    જો (balloon_array == ખાલી)
	પાછા;

    pum_undisplay();
    જ્યારે (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    શૂન્ય
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    જો (mesg == ખાલી && list == ખાલી)
    {
	pum_undisplay();
	પાછા;
    }
    જો (list != ખાલી)
    {
	listitem_T  *li;
	પૂર્ણાંક	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	જો (balloon_array == ખાલી)
	    પાછા;
	CHECK_LIST_MATERIALIZE(list);
	માટે (idx = 0, li = list->lv_first; li != ખાલી; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == ખાલી ? (char_u *)"" : text);
	}
    }
    નહીંતર
	balloon_arraysize = split_message(mesg, &balloon_array);

    જો (balloon_arraysize <= 0)
	પાછા;

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
    શૂન્ય
ui_may_remove_balloon(શૂન્ય)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#જો defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    સ્થિર શૂન્ય
pum_select_mouse_pos(શૂન્ય)
{
    પૂર્ણાંક idx = mouse_row - pum_row;

    જો (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    નહીંતર જો (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    સ્થિર શૂન્ય
pum_execute_menu(vimmenu_T *menu, પૂર્ણાંક mode)
{
    vimmenu_T   *mp;
    પૂર્ણાંક		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	જો ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    તોડો;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    શૂન્ય
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    પૂર્ણાંક		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    પૂર્ણાંક		save_bevalterm = p_bevalterm;
# endif
    પૂર્ણાંક		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	જો (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    જો (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	પાછા;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    જો (array == ખાલી)
	પાછા;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = ખાલી;

	// Make a copy of the text, the menu may be redefined in a callback.
	જો (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	નહીંતર જો (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	જો (s != ખાલી)
	{
	    s = vim_strsave(s);
	    જો (s != ખાલી)
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

    માટે (;;)
    {
	પૂર્ણાંક	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	જો (c == ESC || c == Ctrl_C || pum_array == ખાલી)
	    તોડો;
	નહીંતર જો (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    તોડો;
	}
	નહીંતર જો (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    જ્યારે (pum_selected > 0)
	    {
		--pum_selected;
		જો (*array[pum_selected].pum_text != NUL)
		    તોડો;
	    }
	}
	નહીંતર જો (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    જ્યારે (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		જો (*array[pum_selected].pum_text != NUL)
		    તોડો;
	    }
	}
	નહીંતર જો (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    તોડો;
	}
	નહીંતર જો (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	નહીંતર જો (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    જો (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		તોડો;
	    }
	    જો (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		તોડો;
	}
    }

    માટે (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    શૂન્ય
pum_make_popup(char_u *path_name, પૂર્ણાંક use_mouse_pos)
{
    vimmenu_T *menu;

    જો (!use_mouse_pos)
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
    જો (menu != ખાલી)
	pum_show_popupmenu(menu);
}
#endif

    સ્થિર પૂર્ણાંક
is_singlewidth(પૂર્ણાંક c)
{
    જો (c >= 0 && c < 0x80)
	પાછા TRUE;
    જો (utf_char2cells(c) == 1)
	પાછા TRUE;
    પાછા FALSE;
}

/*
 * Set border characters for popup menu.
 */
    શૂન્ય
pum_set_border_chars(
	પૂર્ણાંક ટોચ,
	પૂર્ણાંક જમણું,
	પૂર્ણાંક bottom,
	પૂર્ણાંક ડાબું,
	પૂર્ણાંક ટોચ_ડાબું,
	પૂર્ણાંક ટોચ_જમણું,
	પૂર્ણાંક bottom_જમણું,
	પૂર્ણાંક bottom_ડાબું)
{
    જો (!is_singlewidth(ટોચ) || !is_singlewidth(જમણું)
	    || !is_singlewidth(bottom) || !is_singlewidth(ડાબું)
	    || !is_singlewidth(ટોચ_ડાબું) || !is_singlewidth(ટોચ_જમણું)
	    || !is_singlewidth(bottom_જમણું) || !is_singlewidth(bottom_ડાબું))
	પાછા;
    pum_border_chars.ટોચ = ટોચ;
    pum_border_chars.જમણું = જમણું;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ડાબું = ડાબું;
    pum_border_chars.ટોચ_ડાબું = ટોચ_ડાબું;
    pum_border_chars.ટોચ_જમણું = ટોચ_જમણું;
    pum_border_chars.bottom_જમણું = bottom_જમણું;
    pum_border_chars.bottom_ડાબું = bottom_ડાબું;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    સ્થિર શૂન્ય
compute_margins(પૂર્ણાંક *જમણું_margin, પૂર્ણાંક *ડાબું_margin, પૂર્ણાંક *ડાબું_padding)
{
    *જમણું_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	*ડાબું_padding = (pum_col < Columns - 1 - pum_border);
	*ડાબું_margin = *જમણું_margin && (pum_col < Columns - 3);
	પાછા;
    }
#endif
    *ડાબું_padding = (pum_col > pum_border);
    *ડાબું_margin = *જમણું_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    સ્થિર શૂન્ય
pum_draw_border(શૂન્ય)
{
    પૂર્ણાંક	ટોચ_row = pum_row - 1;
    પૂર્ણાંક	bottom_row = pum_row + pum_height;
    પૂર્ણાંક	col;
    પૂર્ણાંક	attr = highlight_attr[HLF_PMB];
    પૂર્ણાંક	જમણું_margin, ડાબું_margin, ડાબું_padding;
    પૂર્ણાંક	inner_width;

    જો (!pum_border)
	પાછા;

    (શૂન્ય)compute_margins(&જમણું_margin, &ડાબું_margin, &ડાબું_padding);

    inner_width = pum_width + pum_scrollbar + ડાબું_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
	col = pum_col + ડાબું_padding + 1;
    નહીંતર
#endif
	col = pum_col - ડાબું_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	screen_putchar(pum_border_chars.ટોચ_જમણું, ટોચ_row, col, attr);
	જો (ડાબું_margin)
	    screen_putchar(' ', ટોચ_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_જમણું, bottom_row, col, attr);
	જો (ડાબું_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    નહીંતર
#endif
    {
	screen_putchar(pum_border_chars.ટોચ_ડાબું, ટોચ_row, col, attr);
	જો (ડાબું_margin)
	    screen_putchar(' ', ટોચ_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_ડાબું, bottom_row, col, attr);
	જો (ડાબું_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    માટે (પૂર્ણાંક i = 0; i < inner_width; i++)
    {
	પૂર્ણાંક cur_col;
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    cur_col = col - 1 - i;
	નહીંતર
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.ટોચ, ટોચ_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	screen_putchar(pum_border_chars.ટોચ_ડાબું, ટોચ_row,
		col - inner_width - 1, attr);
	જો (જમણું_margin)
	    screen_putchar(' ', ટોચ_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_ડાબું, bottom_row,
		col - inner_width - 1, attr);
	જો (જમણું_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    નહીંતર
#endif
    {
	screen_putchar(pum_border_chars.ટોચ_જમણું, ટોચ_row,
		col + inner_width + 1, attr);
	જો (જમણું_margin)
	    screen_putchar(' ', ટોચ_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_જમણું, bottom_row,
		col + inner_width + 1, attr);
	જો (જમણું_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    માટે (પૂર્ણાંક i = 0; i < pum_height; i++)
    {
	પૂર્ણાંક row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	{
	    screen_putchar(pum_border_chars.ડાબું, row, col, attr);
	    જો (ડાબું_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.જમણું, row,
		    col - inner_width - 1, attr);
	    જો (જમણું_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	નહીંતર
#endif
	{
	    screen_putchar(pum_border_chars.ડાબું, row, col, attr);
	    જો (ડાબું_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.જમણું, row,
		    col + inner_width + 1, attr);
	    જો (જમણું_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    શૂન્ય
put_shadow_char(પૂર્ણાંક row, પૂર્ણાંક col)
{
    char_u  buf[MB_MAXBYTES + 1];
    પૂર્ણાંક	    shadow_attr = highlight_attr[HLF_PMS];
    પૂર્ણાંક	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    સ્થિર શૂન્ય
pum_draw_shadow(શૂન્ય)
{
    પૂર્ણાંક	row, col, next_col;
    પૂર્ણાંક	width, offset;
    પૂર્ણાંક	inner_width;
    પૂર્ણાંક	જમણું_margin, ડાબું_margin, ડાબું_padding;

    જો (!pum_shadow)
	પાછા;

    (શૂન્ય)compute_margins(&જમણું_margin, &ડાબું_margin, &ડાબું_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + જમણું_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    જો (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    નહીંતર
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    માટે (પૂર્ણાંક i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + ડાબું_padding + pum_border + ડાબું_margin;
    offset = 2 - ડાબું_padding - pum_border - ડાબું_margin;

    માટે (પૂર્ણાંક i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	જો (pum_rl)
	    col = pum_col - offset - i;
	નહીંતર
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
