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
#অন্তর্ভাব "vim.h"

স্থির pumitem_T *pum_array = রিক্ত;	// items of displayed pum
স্থির পূর্ণাঙ্ক pum_size;			// nr of items in "pum_array"
স্থির পূর্ণাঙ্ক pum_selected;		// index of selected item or -1
স্থির পূর্ণাঙ্ক pum_first = 0;		// index of top item

স্থির পূর্ণাঙ্ক call_update_screen = FALSE;
স্থির পূর্ণাঙ্ক pum_in_cmdline = FALSE;

স্থির পূর্ণাঙ্ক pum_height;			// nr of displayed pum items
স্থির পূর্ণাঙ্ক pum_width;			// width of displayed pum items
স্থির পূর্ণাঙ্ক pum_base_width;		// width of pum items base
স্থির পূর্ণাঙ্ক pum_kind_width;		// width of pum items kind column
স্থির পূর্ণাঙ্ক pum_extra_width;		// width of extra stuff
স্থির পূর্ণাঙ্ক pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
স্থির পূর্ণাঙ্ক pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

স্থির পূর্ণাঙ্ক pum_row;			// top row of pum
স্থির পূর্ণাঙ্ক pum_col;			// left column of pum

স্থির win_T *pum_window = রিক্ত;
স্থির পূর্ণাঙ্ক pum_win_row;
স্থির পূর্ণাঙ্ক pum_win_height;
স্থির পূর্ণাঙ্ক pum_win_col;
স্থির পূর্ণাঙ্ক pum_win_wcol;
স্থির পূর্ণাঙ্ক pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
স্থির পূর্ণাঙ্ক pum_pretend_not_visible = FALSE;

// Border support
স্থির পূর্ণাঙ্ক pum_border = 0;
স্থির পূর্ণাঙ্ক pum_margin = 0;	// margin of 1 cell on left and right
স্থির পূর্ণাঙ্ক pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
স্থির image_placement_T *pum_imagep = রিক্ত;
#endif

// Border characters
স্থির সংরচনা {
    পূর্ণাঙ্ক শীর্ষ;
    পূর্ণাঙ্ক ডান;
    পূর্ণাঙ্ক bottom;
    পূর্ণাঙ্ক বাম;
    পূর্ণাঙ্ক শীর্ষ_বাম;
    পূর্ণাঙ্ক শীর্ষ_ডান;
    পূর্ণাঙ্ক bottom_ডান;
    পূর্ণাঙ্ক bottom_বাম;
} pum_border_chars;

স্থির পূর্ণাঙ্ক pum_set_selected(পূর্ণাঙ্ক n, পূর্ণাঙ্ক repeat);
স্থির শূন্য pum_draw_border(শূন্য);
স্থির শূন্য pum_draw_shadow(শূন্য);
স্থির শূন্য compute_margins(পূর্ণাঙ্ক *ডান_margin, পূর্ণাঙ্ক *বাম_margin, পূর্ণাঙ্ক *বাম_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	শূন্য \
    pum_set_##feature(পূর্ণাঙ্ক enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    স্থির শূন্য
pum_compute_size(শূন্য)
{
    পূর্ণাঙ্ক	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    প্রতি (পূর্ণাঙ্ক i = 0; i < pum_size; ++i)
    {
	যদি (pum_array[i].pum_text != রিক্ত)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    যদি (pum_base_width < w)
		pum_base_width = w;
	}
	যদি (pum_array[i].pum_kind != রিক্ত)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    যদি (pum_kind_width < w)
		pum_kind_width = w;
	}
	যদি (pum_array[i].pum_extra != রিক্ত)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    যদি (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    স্থির শূন্য
pum_compute_vertical_placement(
	পূর্ণাঙ্ক size,
	পূর্ণাঙ্ক above_row,
	পূর্ণাঙ্ক below_row)
{
    পূর্ণাঙ্ক context_সারিগুলি;
    পূর্ণাঙ্ক cline_visible_offset;
    পূর্ণাঙ্ক extra_height = 2 * pum_border + pum_shadow;
    পূর্ণাঙ্ক extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    যদি (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    যদি (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	যদি (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_সারিগুলি = 0;
	অন্যথা
	    // Leave two lines of context if possible
	    context_সারিগুলি = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	যদি (pum_win_row >= size + context_সারিগুলি + extra_height)
	{
	    pum_row = pum_win_row - size - context_সারিগুলি - extra_offset;
	    pum_height = size;
	}
	অন্যথা
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_সারিগুলি - extra_height;
	}
	যদি (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    অন্যথা
    {
	// pum below "pum_win_row"
	যদি (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_সারিগুলি = 0;
	অন্যথা
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_সারিগুলি = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_সারিগুলি + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	যদি (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#যদি defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    যদি (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    স্থির পূর্ণাঙ্ক
set_pum_width_aligned_with_cursor(পূর্ণাঙ্ক width, পূর্ণাঙ্ক available_width)
{
    পূর্ণাঙ্ক end_padding = TRUE;

    যদি (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    যদি (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    প্রত্যাগম available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    স্থির শূন্য
pum_compute_horizontal_placement(পূর্ণাঙ্ক cursor_col)
{
    পূর্ণাঙ্ক desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    পূর্ণাঙ্ক available_width;
    পূর্ণাঙ্ক col_offset;
    পূর্ণাঙ্ক trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    অন্যথা
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    যদি (set_pum_width_aligned_with_cursor(desired_width, available_width))
	প্রত্যাগম;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    যদি (available_width > p_pw)
    {
	pum_width = available_width;
	প্রত্যাগম;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    যদি (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	অন্যথা
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	প্রত্যাগম;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
	pum_col = Columns - 1 - col_offset;
    অন্যথা
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
    শূন্য
pum_display(
	pumitem_T   *array,
	পূর্ণাঙ্ক	    size,
	পূর্ণাঙ্ক	    selected,   // index of initially selected item, -1 if
				// out of range
	পূর্ণাঙ্ক	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    পূর্ণাঙ্ক	    cursor_col;
    পূর্ণাঙ্ক	    above_row;
    পূর্ণাঙ্ক	    below_row;
    পূর্ণাঙ্ক	    redo_গুনতি = 0;
#যদি defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    কুরু
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = রিক্ত;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	যদি (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	অন্যথা
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#যদি defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    যদি (pvwin->w_p_pvw)
		ভঙ্গ;
	যদি (pvwin != রিক্ত)
	{
	    যদি (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    অন্যথা যদি (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	যদি (pum_height < 1 || (pum_height == 1 && size > 1))
	    প্রত্যাগম;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	যদি (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	অন্যথা
	{
	    পূর্ণাঙ্ক wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    যদি (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    অন্যথা
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    যদি (curwin->w_p_cole > 0 && conceal_cursor_সারি(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		যদি (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    যদি (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    অন্যথা
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
    } যাবত্ (pum_set_selected(selected, redo_গুনতি) && ++redo_গুনতি <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    শূন্য
pum_call_update_screen(শূন্য)
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
    পূর্ণাঙ্ক
pum_under_menu(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক only_redrawing)
{
    পূর্ণাঙ্ক	extra_বাম = pum_border + (pum_margin && pum_border ? 1 : 0);
    পূর্ণাঙ্ক	extra_ডান = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    পূর্ণাঙ্ক	extra_above = pum_border;
    পূর্ণাঙ্ক	extra_below = pum_border + (pum_shadow ? 1 : 0);
    পূর্ণাঙ্ক	শীর্ষ = pum_row - extra_above;
    পূর্ণাঙ্ক	bot = pum_row + pum_height + extra_below;
    পূর্ণাঙ্ক	বাম = pum_col - 1 - extra_বাম;
    পূর্ণাঙ্ক	ডান = pum_col + pum_width + pum_scrollbar + extra_ডান;

    যদি (!((!only_redrawing || pum_will_redraw)
	    && row >= শীর্ষ && row < bot && col >= বাম && col < ডান))
	প্রত্যাগম FALSE;

    যদি (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	যদি (only_redrawing)
	{
	    যদি (col >= ডান - 2 || row == bot - 1)
		প্রত্যাগম FALSE;
	}
	অন্যথা
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    পূর্ণাঙ্ক	    ডান_margin, বাম_margin, বাম_padding;

	    compute_margins(&ডান_margin, &বাম_margin, &বাম_padding);
	    যদি (row == শীর্ষ && col >= ডান - 2)
		প্রত্যাগম FALSE;
	    যদি (row == bot - 1 && col < pum_col + 2 - বাম_padding - pum_border
		    - বাম_margin)
		প্রত্যাগম FALSE;
	}
    }
    প্রত্যাগম TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    স্থির পূর্ণাঙ্ক *
pum_compute_text_attrs(char_u *text, hlf_T hlf, পূর্ণাঙ্ক user_hlattr)
{
    পূর্ণাঙ্ক		i;
    size_t	leader_len;
    পূর্ণাঙ্ক		char_cells;
    পূর্ণাঙ্ক		new_attr;
    char_u	*ptr = text;
    পূর্ণাঙ্ক		cell_idx = 0;
    garray_T	*ga = রিক্ত;
    পূর্ণাঙ্ক		*attrs = রিক্ত;
    char_u	*leader = রিক্ত;
    পূর্ণাঙ্ক		in_fuzzy;
    পূর্ণাঙ্ক		matched_len = -1;
    int_u	char_pos = 0;
    পূর্ণাঙ্ক		is_select = FALSE;

    যদি (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	প্রত্যাগম রিক্ত;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    যদি (leader == রিক্ত || *leader == NUL)
	প্রত্যাগম রিক্ত;

    attrs = ALLOC_MULT(পূর্ণাঙ্ক, vim_strsize(text));
    যদি (attrs == রিক্ত)
	প্রত্যাগম রিক্ত;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    যদি (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	যদি (!ga)
	{
	    vim_free(attrs);
	    প্রত্যাগম রিক্ত;
	}
    }

    যাবত্ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	যদি (ga != রিক্ত)
	{
	    // Handle fuzzy matching
	    প্রতি (i = 0; i < ga->ga_len; i++)
	    {
		যদি (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ভঙ্গ;
		}
	    }
	}
	অন্যথা
	{
	    যদি (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (পূর্ণাঙ্ক)leader_len;
	    যদি (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	যদি (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	প্রতি (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    যদি (ga != রিক্ত)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    প্রত্যাগম attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    স্থির শূন্য
pum_screen_puts_with_attrs(
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		cells UNUSED,
    char_u	*text,
    পূর্ণাঙ্ক		textlen,
    পূর্ণাঙ্ক		*attrs)
{
    পূর্ণাঙ্ক		col_start = col;
    char_u	*ptr = text;
    পূর্ণাঙ্ক		char_len;
    পূর্ণাঙ্ক		attr;

    // Render text with proper attributes
    যাবত্ (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	অন্যথা
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    স্থির অন্তঃস্থ char_u *
pum_get_item(পূর্ণাঙ্ক index, পূর্ণাঙ্ক type)
{
    বিকল্প(type)
    {
	পক্ষ CPT_ABBR: প্রত্যাগম pum_array[index].pum_text;
	পক্ষ CPT_KIND: প্রত্যাগম pum_array[index].pum_kind;
	পক্ষ CPT_MENU: প্রত্যাগম pum_array[index].pum_extra;
    }
    প্রত্যাগম রিক্ত;
}

    স্থির অন্তঃস্থ পূর্ণাঙ্ক
pum_user_attr_combine(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক type, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    প্রত্যাগম user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    স্থির পূর্ণাঙ্ক
pum_display_rtl_text(
	পূর্ণাঙ্ক	row,
	পূর্ণাঙ্ক     col,
	char_u  *st,
	পূর্ণাঙ্ক     attr,
	পূর্ণাঙ্ক     *attrs,
	পূর্ণাঙ্ক     width,
	পূর্ণাঙ্ক     width_limit,
	পূর্ণাঙ্ক     totwidth,
	পূর্ণাঙ্ক     next_isempty,
	পূর্ণাঙ্ক	selected)
{
    char_u  *rt = রিক্ত;
    পূর্ণাঙ্ক     cells = 0;
    পূর্ণাঙ্ক     over_cell = 0;
    পূর্ণাঙ্ক     truncated = FALSE;
    পূর্ণাঙ্ক     pad = next_isempty ? 0 : 2;
    পূর্ণাঙ্ক     remaining = 0;
    পূর্ণাঙ্ক	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    পূর্ণাঙ্ক	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    যদি (st == রিক্ত)
	প্রত্যাগম col;

    rt = reverse_text(st);
    যদি (rt == রিক্ত)
    {
	VIM_CLEAR(st);
	প্রত্যাগম col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    যদি (cells > width_limit)
    {
	কুরু
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} যাবত্ (cells > width_limit);

	যদি (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    যদি (truncated)
    {
	char_u  *orig_rt = rt;
	পূর্ণাঙ্ক     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	যদি (cells > remaining)
	{
	    যাবত্ (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (পূর্ণাঙ্ক)STRLEN(orig_rt);
	যদি (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	যদি (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    যদি (attrs == রিক্ত)
	screen_puts_len(rt, (পূর্ণাঙ্ক)STRLEN(rt), row, col - cells + 1, attr);
    অন্যথা
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(পূর্ণাঙ্ক)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    প্রত্যাগম col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    স্থির পূর্ণাঙ্ক
pum_display_ltr_text(
	পূর্ণাঙ্ক     row,
	পূর্ণাঙ্ক     col,
	char_u  *st,
	পূর্ণাঙ্ক     attr,
	পূর্ণাঙ্ক     *attrs,
	পূর্ণাঙ্ক     width,        // width already calculated in outer loop
	পূর্ণাঙ্ক     width_limit,
	পূর্ণাঙ্ক     totwidth,
	পূর্ণাঙ্ক     next_isempty,
	পূর্ণাঙ্ক	selected)
{
    পূর্ণাঙ্ক     size = 0;
    পূর্ণাঙ্ক     cells = 0;
    char_u  *st_end = রিক্ত;
    পূর্ণাঙ্ক     over_cell = 0;
    পূর্ণাঙ্ক     pad = next_isempty ? 0 : 2;
    পূর্ণাঙ্ক     truncated = FALSE;
    পূর্ণাঙ্ক     remaining = 0;
    পূর্ণাঙ্ক	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    পূর্ণাঙ্ক	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    যদি (st == রিক্ত)
	প্রত্যাগম col;

    size = (পূর্ণাঙ্ক)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    যাবত্ (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	যদি (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	অন্যথা
	    --cells;
    }

    // truncated
    যদি (truncated)
    {
	remaining = width_limit - totwidth - 1;
	যদি (cells > remaining)
	{
	    st_end = st + size;
	    যাবত্ (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	যদি (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    যদি (attrs == রিক্ত)
	screen_puts_len(st, size, row, col, attr);
    অন্যথা
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    যদি (truncated)
    {
	যদি (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    প্রত্যাগম col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    স্থির পূর্ণাঙ্ক
pum_process_item(
	পূর্ণাঙ্ক     row,
	পূর্ণাঙ্ক     col,
	পূর্ণাঙ্ক     idx,
	পূর্ণাঙ্ক     j,         // Current position in order array
	পূর্ণাঙ্ক     *order,    // Order array
	hlf_T   hlf,
	পূর্ণাঙ্ক     attr,
	পূর্ণাঙ্ক     *totwidth_ptr,
	পূর্ণাঙ্ক     next_isempty)
{
    পূর্ণাঙ্ক     item_type = order[j];
    char_u  *s = রিক্ত;
    char_u  *p = pum_get_item(idx, item_type);
    পূর্ণাঙ্ক     width = 0;  // item width
    পূর্ণাঙ্ক     w;		// char width
    পূর্ণাঙ্ক	    selected = idx == pum_selected;

    প্রতি ( ; ; MB_PTR_ADV(p))
    {
	যদি (s == রিক্ত)
	    s = p;
	w = ptr2cells(p);
	যদি (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    অনুবর্ত;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	পূর্ণাঙ্ক     *attrs = রিক্ত;
	পূর্ণাঙ্ক     saved = *p;

	যদি (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	যদি (saved != NUL)
	    *p = saved;

	যদি (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	অন্যথা
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	যদি (attrs != রিক্ত)
	    VIM_CLEAR(attrs);

	যদি (*p != TAB)
	    ভঙ্গ;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	অন্যথা
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = রিক্ত;  // start text at next char
	width = 0;
    }

    প্রত্যাগম col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    স্থির শূন্য
pum_draw_scrollbar(
	পূর্ণাঙ্ক	row,
	পূর্ণাঙ্ক	i,
	পূর্ণাঙ্ক	thumb_pos,
	পূর্ণাঙ্ক	thumb_height)
{
    যদি (pum_scrollbar <= 0)
	প্রত্যাগম;

    পূর্ণাঙ্ক attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    অন্যথা
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    স্থির অন্তঃস্থ শূন্য
pum_align_order(পূর্ণাঙ্ক *order)
{
    পূর্ণাঙ্ক is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

স্থির শূন্য pum_free_bg(শূন্য);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    শূন্য
pum_opacity_changed(শূন্য)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    যদি (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    স্থির শূন্য
pum_free_bg(শূন্য)
{
    পূর্ণাঙ্ক k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_সারিগুলি);
    VIM_CLEAR(pum_bg_সারিগুলি_UC);
    প্রতি (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_সারিগুলি_C[k]);
    pum_bg_শীর্ষ = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    শূন্য
pum_redraw(শূন্য)
{
    পূর্ণাঙ্ক		row = pum_row;
    পূর্ণাঙ্ক		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		idx;
    char_u	*p = রিক্ত;
    পূর্ণাঙ্ক		totwidth;
    পূর্ণাঙ্ক		thumb_pos = 0;
    পূর্ণাঙ্ক		thumb_height = 1;
    পূর্ণাঙ্ক		item_type;
    পূর্ণাঙ্ক		order[3];
    পূর্ণাঙ্ক		next_isempty = FALSE;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		বস্তুসমূহ_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    পূর্ণাঙ্ক		basic_width;  // first item width
    পূর্ণাঙ্ক		last_isabbr = FALSE;
    পূর্ণাঙ্ক		orig_attr = -1;
    পূর্ণাঙ্ক		scroll_range = pum_size - pum_height;
    bool	override_success;
    পূর্ণাঙ্ক		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ঠেলো_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    যদি (pum_imagep == রিক্ত)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(রিক্ত, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    যদি (pum_imagep != রিক্ত)
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

    যদি (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	যদি (opacity_active && pum_bg_সারিগুলি != রিক্ত
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	যদি (opacity_active && pum_bg_সারিগুলি != রিক্ত)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	অন্যথা যদি (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    যদি (Screen_সারিগুলি != রিক্ত && ScreenAttrs != রিক্ত)
	    {
		পূর্ণাঙ্ক	save_শীর্ষ, save_bot, save_ncells, k;

		pum_free_bg();
		save_শীর্ষ = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		যদি (save_শীর্ষ < 0)
		    save_শীর্ষ = 0;
		যদি (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_শীর্ষ = save_শীর্ষ;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		যদি (save_শীর্ষ < save_bot)
		{
		    save_ncells = (save_bot - save_শীর্ষ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_সারিগুলি = LALLOC_MULT(schar_T, save_ncells);
		    যদি (enc_utf8)
		    {
			pum_bg_সারিগুলি_UC = LALLOC_MULT(u8char_T, save_ncells);
			প্রতি (k = 0; k < MAX_MCO; ++k)
			    pum_bg_সারিগুলি_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    যদি (pum_bg_attrs != রিক্ত && pum_bg_সারিগুলি != রিক্ত)
		    {
			প্রতি (পূর্ণাঙ্ক r = save_শীর্ষ; r < save_bot; ++r)
			{
			    পূর্ণাঙ্ক soff = (r - save_শীর্ষ) * screen_Columns;
			    পূর্ণাঙ্ক loff = সারি_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * পরিমাণ(sattr_T));
			    mch_memmove(pum_bg_সারিগুলি + soff,
				    Screen_সারিগুলি + loff,
				    screen_Columns * পরিমাণ(schar_T));
			    যদি (enc_utf8 && pum_bg_সারিগুলি_UC != রিক্ত
						&& Screen_সারিগুলি_UC != রিক্ত)
			    {
				mch_memmove(pum_bg_সারিগুলি_UC + soff,
					Screen_সারিগুলি_UC + loff,
					screen_Columns * পরিমাণ(u8char_T));
				প্রতি (k = 0; k < MAX_MCO; ++k)
				    যদি (pum_bg_সারিগুলি_C[k] != রিক্ত
						    && Screen_সারিগুলি_C[k] != রিক্ত)
					mch_memmove(pum_bg_সারিগুলি_C[k] + soff,
						Screen_সারিগুলি_C[k] + loff,
						screen_Columns
						    * পরিমাণ(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	অন্যথা
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

    যদি (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	যদি (thumb_height == 0)
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
    যদি (pum_border)
	pum_draw_border();
    যদি (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    যদি (opacity_active)
	screen_pum_blend = 100 - (পূর্ণাঙ্ক)p_po;

    প্রতি (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	{
	    যদি (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	অন্যথা
#endif
	    যদি (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = বস্তুসমূহ_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	প্রতি (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    যদি (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    যদি (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == রিক্ত;
	    অন্যথা
		next_isempty = TRUE;

	    যদি (p != রিক্ত)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    যদি (j > 0)
		n = বস্তুসমূহ_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    অন্যথা
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    যদি (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == রিক্ত)))
		    || basic_width + n >= pum_width)
		ভঙ্গ;
#ifdef FEAT_RIGHTLEFT
	    যদি (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    অন্যথা
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	অন্যথা
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

    যদি (override_success)
	বের_করো_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    শূন্য
pum_position_info_popup(win_T *wp)
{
    পূর্ণাঙ্ক col = pum_col + pum_width + pum_scrollbar + 1;
    পূর্ণাঙ্ক row = pum_row;
    পূর্ণাঙ্ক botpos = POPPOS_BOTLEFT;
    পূর্ণাঙ্ক	used_maxwidth_opt = FALSE;
    পূর্ণাঙ্ক	ডান_margin, বাম_margin, dummy;
    পূর্ণাঙ্ক বাম_extra_width, ডান_extra_width;

    (শূন্য)compute_margins(&ডান_margin, &বাম_margin, &dummy);

    বাম_extra_width = pum_border + বাম_margin;
    ডান_extra_width = pum_border + ডান_margin + (pum_shadow ? 2 : 0);
    col += ডান_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    যদি (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - বাম_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - বাম_extra_width;
    }
    অন্যথা
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    যদি (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_শীর্ষ_extra(wp);
    যদি (wp->w_popup_flags & POPF_INFO_MENU)
    {
	যদি (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	অন্যথা
	    // menu below cursor line, align with top
	    row += 1;
    }
    অন্যথা
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    যদি (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    অন্যথা
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
    স্থির পূর্ণাঙ্ক
pum_set_selected(পূর্ণাঙ্ক n, পূর্ণাঙ্ক repeat UNUSED)
{
    পূর্ণাঙ্ক	    resized = FALSE;
    পূর্ণাঙ্ক	    context = pum_height / 2;
    পূর্ণাঙ্ক	    scroll_offset;
#ifdef FEAT_QUICKFIX
    পূর্ণাঙ্ক	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    পূর্ণাঙ্ক	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    যদি (pum_selected >= 0 && pum_selected < pum_size)
    {
	যদি (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    যদি (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		যদি (pum_first < 0)
		    pum_first = 0;
		অন্যথা যদি (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    অন্যথা
		pum_first = pum_selected;
	}
	অন্যথা যদি (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    যদি (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    অন্যথা
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	যদি (pum_height > 2)
	{
	    যদি (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    অন্যথা যদি (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#যদি defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	যদি (pum_array[pum_selected].pum_info != রিক্ত
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# অন্যথা
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    যদি (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    অন্যথা যদি (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    অন্যথা
		use_popup = USEPOPUP_NONE;
	    যদি (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    যদি (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    যদি (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    যদি (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		পূর্ণাঙ্ক	res = OK;

		যদি (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == রিক্ত
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    যাবত্ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		অন্যথা
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, রিক্ত, রিক্ত, রিক্ত, ECMD_ONE, 0, রিক্ত);
		    --no_u_sync;
		    যদি (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_মান_give_err((char_u *)"swf",
							  0L, রিক্ত, OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bl",
							  0L, রিক্ত, OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_মান_give_err((char_u *)"diff",
							  0L, রিক্ত, OPT_LOCAL);
		    }
		}
		যদি (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    প্রতি (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			যদি (e == রিক্ত)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ভঙ্গ;
			}
			*e = NUL;
			ml_append(lnum++, p, (পূর্ণাঙ্ক)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_সারি_গুনতি);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    যদি (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			যদি (curwin->w_height < lnum)
			{
			    win_setheight((পূর্ণাঙ্ক)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    যদি (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    অন্যথা যদি (curwin->w_topline > curbuf->b_ml.ml_সারি_গুনতি)
			curwin->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    যদি (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			যদি (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    যদি ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			পূর্ণাঙ্ক save_redr_status;

			যদি (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			যদি (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			যদি (resized && win_valid(curwin_save))
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

			যদি (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    যদি (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    যদি (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    যদি (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    যদি (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    প্রত্যাগম resized;
}

/*
 * Undisplay the popup menu (later).
 */
    শূন্য
pum_undisplay(শূন্য)
{
#ifdef FEAT_IMAGE_POPUP
    যদি (pum_imagep != রিক্ত)
    {
	image_placement_free(pum_imagep);
	pum_imagep = রিক্ত;
    }
#endif
    pum_free_bg();
    pum_array = রিক্ত;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    যদি (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    শূন্য
pum_clear(শূন্য)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    পূর্ণাঙ্ক
pum_visible(শূন্য)
{
    প্রত্যাগম !pum_pretend_not_visible && pum_array != রিক্ত;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    স্থির পূর্ণাঙ্ক
pum_in_same_position(শূন্য)
{
    পূর্ণাঙ্ক	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    প্রত্যাগম pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    পূর্ণাঙ্ক
pum_redraw_in_same_position(শূন্য)
{
    যদি (!pum_visible() || pum_will_redraw)
	প্রত্যাগম FALSE;  // nothing to do

    প্রত্যাগম pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    শূন্য
pum_may_redraw(শূন্য)
{
    pumitem_T	*array = pum_array;
    পূর্ণাঙ্ক		len = pum_size;
    পূর্ণাঙ্ক		selected = pum_selected;

    যদি (!pum_visible() || pum_will_redraw)
	প্রত্যাগম;  // nothing to do

    যদি (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    অন্যথা
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
    পূর্ণাঙ্ক
pum_get_height(শূন্য)
{
    প্রত্যাগম pum_height;
}

#যদি defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    শূন্য
pum_set_event_info(dict_T *dict)
{
    যদি (!pum_visible())
	প্রত্যাগম;
    (শূন্য)dict_add_সংখ্যা(dict, "height", pum_height);
    (শূন্য)dict_add_সংখ্যা(dict, "width", pum_width);
    (শূন্য)dict_add_সংখ্যা(dict, "row", pum_row);
    (শূন্য)dict_add_সংখ্যা(dict, "col", pum_col);
    (শূন্য)dict_add_সংখ্যা(dict, "size", pum_size);
    (শূন্য)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#যদি defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    স্থির শূন্য
pum_position_at_mouse(পূর্ণাঙ্ক min_width)
{
    পূর্ণাঙ্ক extra_height = pum_border * 2 + pum_shadow;
    পূর্ণাঙ্ক extra_offset = pum_border + pum_shadow;
    পূর্ণাঙ্ক trailing_overhead = 0;
    পূর্ণাঙ্ক col_offset = 0;
    পূর্ণাঙ্ক available_width;

    যদি (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	যদি (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	যদি (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    অন্যথা
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	যদি (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	যদি (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	অন্যথা
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    অন্যথা
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	যদি (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	অন্যথা
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = রিক্ত;
}

#endif

#যদি defined(FEAT_BEVAL_TERM)
স্থির pumitem_T *balloon_array = রিক্ত;
স্থির পূর্ণাঙ্ক balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

প্রকারনির্দেশ সংরচনা {
    char_u	*start;
    পূর্ণাঙ্ক		bytelen;
    পূর্ণাঙ্ক		cells;
    পূর্ণাঙ্ক		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    পূর্ণাঙ্ক
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    পূর্ণাঙ্ক		quoted = FALSE;
    পূর্ণাঙ্ক		height;
    পূর্ণাঙ্ক		সারি;
    পূর্ণাঙ্ক		item_idx;
    পূর্ণাঙ্ক		indent = 0;
    পূর্ণাঙ্ক		max_cells = 0;
    পূর্ণাঙ্ক		max_height = Rows / 2 - 1;
    পূর্ণাঙ্ক		long_item_গুনতি = 0;
    পূর্ণাঙ্ক		split_long_বস্তুসমূহ = FALSE;

    ga_init2(&ga, পরিমাণ(balpart_T), 20);
    p = mesg;

    যাবত্ (*p != NUL)
    {
	যদি (ga_grow(&ga, 1) == FAIL)
	    গচ্ছ failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	যাবত্ (*p != NUL)
	{
	    যদি (*p == '"')
		quoted = !quoted;
	    অন্যথা যদি (*p == '\n')
		ভঙ্গ;
	    অন্যথা যদি (*p == '\\' && p[1] != NUL)
		++p;
	    অন্যথা যদি (!quoted)
	    {
		যদি ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    যদি (*p == '{')
			++indent;
		    অন্যথা যদি (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    ভঙ্গ;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	যদি (*p == '\n')
	    ++p;
	যদি (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_গুনতি += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    যদি (long_item_গুনতি > 0 && height + long_item_গুনতি <= max_height)
    {
	split_long_বস্তুসমূহ = TRUE;
	height += long_item_গুনতি;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    যদি (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    যদি (*array == রিক্ত)
	গচ্ছ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    প্রতি (সারি = 1, item_idx = 0; সারি < height - 1; ++item_idx)
    {
	পূর্ণাঙ্ক	skip;
	পূর্ণাঙ্ক	thislen;
	পূর্ণাঙ্ক	copylen;
	পূর্ণাঙ্ক	ind;
	পূর্ণাঙ্ক	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	যদি (item->bytelen == 0)
	    (*array)[সারি++].pum_text = vim_strsave((char_u *)"");
	অন্যথা
	    প্রতি (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		যদি (split_long_বস্তুসমূহ && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    প্রতি (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			যদি ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    ভঙ্গ;
		    thislen = p - (item->start + skip);
		}
		অন্যথা
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		যদি (p == রিক্ত)
		{
		    প্রতি (সারি = 0; সারি <= height - 1; ++সারি)
			vim_free((*array)[সারি].pum_text);
		    vim_free(*array);
		    গচ্ছ failed;
		}
		প্রতি (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		প্রতি (copylen = thislen; copylen > 0; --copylen)
		    যদি (item->start[skip + copylen - 1] != ' ')
			ভঙ্গ;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[সারি].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++সারি;
	    }
    }
    ga_clear(&ga);
    প্রত্যাগম height;

failed:
    ga_clear(&ga);
    প্রত্যাগম 0;
}

    শূন্য
ui_remove_balloon(শূন্য)
{
    যদি (balloon_array == রিক্ত)
	প্রত্যাগম;

    pum_undisplay();
    যাবত্ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    শূন্য
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    যদি (mesg == রিক্ত && list == রিক্ত)
    {
	pum_undisplay();
	প্রত্যাগম;
    }
    যদি (list != রিক্ত)
    {
	listitem_T  *li;
	পূর্ণাঙ্ক	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	যদি (balloon_array == রিক্ত)
	    প্রত্যাগম;
	CHECK_LIST_MATERIALIZE(list);
	প্রতি (idx = 0, li = list->lv_first; li != রিক্ত; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == রিক্ত ? (char_u *)"" : text);
	}
    }
    অন্যথা
	balloon_arraysize = split_message(mesg, &balloon_array);

    যদি (balloon_arraysize <= 0)
	প্রত্যাগম;

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
    শূন্য
ui_may_remove_balloon(শূন্য)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#যদি defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    স্থির শূন্য
pum_select_mouse_pos(শূন্য)
{
    পূর্ণাঙ্ক idx = mouse_row - pum_row;

    যদি (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    অন্যথা যদি (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    স্থির শূন্য
pum_execute_menu(vimmenu_T *menu, পূর্ণাঙ্ক mode)
{
    vimmenu_T   *mp;
    পূর্ণাঙ্ক		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	যদি ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    ভঙ্গ;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    শূন্য
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    পূর্ণাঙ্ক		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    পূর্ণাঙ্ক		save_bevalterm = p_bevalterm;
# endif
    পূর্ণাঙ্ক		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	যদি (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    যদি (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	প্রত্যাগম;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    যদি (array == রিক্ত)
	প্রত্যাগম;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = রিক্ত;

	// Make a copy of the text, the menu may be redefined in a callback.
	যদি (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	অন্যথা যদি (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	যদি (s != রিক্ত)
	{
	    s = vim_strsave(s);
	    যদি (s != রিক্ত)
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

    প্রতি (;;)
    {
	পূর্ণাঙ্ক	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	যদি (c == ESC || c == Ctrl_C || pum_array == রিক্ত)
	    ভঙ্গ;
	অন্যথা যদি (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    ভঙ্গ;
	}
	অন্যথা যদি (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    যাবত্ (pum_selected > 0)
	    {
		--pum_selected;
		যদি (*array[pum_selected].pum_text != NUL)
		    ভঙ্গ;
	    }
	}
	অন্যথা যদি (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    যাবত্ (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		যদি (*array[pum_selected].pum_text != NUL)
		    ভঙ্গ;
	    }
	}
	অন্যথা যদি (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ভঙ্গ;
	}
	অন্যথা যদি (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	অন্যথা যদি (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    যদি (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		ভঙ্গ;
	    }
	    যদি (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ভঙ্গ;
	}
    }

    প্রতি (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    শূন্য
pum_make_popup(char_u *path_name, পূর্ণাঙ্ক use_mouse_pos)
{
    vimmenu_T *menu;

    যদি (!use_mouse_pos)
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
    যদি (menu != রিক্ত)
	pum_show_popupmenu(menu);
}
#endif

    স্থির পূর্ণাঙ্ক
is_singlewidth(পূর্ণাঙ্ক c)
{
    যদি (c >= 0 && c < 0x80)
	প্রত্যাগম TRUE;
    যদি (utf_char2cells(c) == 1)
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Set border characters for popup menu.
 */
    শূন্য
pum_set_border_chars(
	পূর্ণাঙ্ক শীর্ষ,
	পূর্ণাঙ্ক ডান,
	পূর্ণাঙ্ক bottom,
	পূর্ণাঙ্ক বাম,
	পূর্ণাঙ্ক শীর্ষ_বাম,
	পূর্ণাঙ্ক শীর্ষ_ডান,
	পূর্ণাঙ্ক bottom_ডান,
	পূর্ণাঙ্ক bottom_বাম)
{
    যদি (!is_singlewidth(শীর্ষ) || !is_singlewidth(ডান)
	    || !is_singlewidth(bottom) || !is_singlewidth(বাম)
	    || !is_singlewidth(শীর্ষ_বাম) || !is_singlewidth(শীর্ষ_ডান)
	    || !is_singlewidth(bottom_ডান) || !is_singlewidth(bottom_বাম))
	প্রত্যাগম;
    pum_border_chars.শীর্ষ = শীর্ষ;
    pum_border_chars.ডান = ডান;
    pum_border_chars.bottom = bottom;
    pum_border_chars.বাম = বাম;
    pum_border_chars.শীর্ষ_বাম = শীর্ষ_বাম;
    pum_border_chars.শীর্ষ_ডান = শীর্ষ_ডান;
    pum_border_chars.bottom_ডান = bottom_ডান;
    pum_border_chars.bottom_বাম = bottom_বাম;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    স্থির শূন্য
compute_margins(পূর্ণাঙ্ক *ডান_margin, পূর্ণাঙ্ক *বাম_margin, পূর্ণাঙ্ক *বাম_padding)
{
    *ডান_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	*বাম_padding = (pum_col < Columns - 1 - pum_border);
	*বাম_margin = *ডান_margin && (pum_col < Columns - 3);
	প্রত্যাগম;
    }
#endif
    *বাম_padding = (pum_col > pum_border);
    *বাম_margin = *ডান_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    স্থির শূন্য
pum_draw_border(শূন্য)
{
    পূর্ণাঙ্ক	শীর্ষ_row = pum_row - 1;
    পূর্ণাঙ্ক	bottom_row = pum_row + pum_height;
    পূর্ণাঙ্ক	col;
    পূর্ণাঙ্ক	attr = highlight_attr[HLF_PMB];
    পূর্ণাঙ্ক	ডান_margin, বাম_margin, বাম_padding;
    পূর্ণাঙ্ক	inner_width;

    যদি (!pum_border)
	প্রত্যাগম;

    (শূন্য)compute_margins(&ডান_margin, &বাম_margin, &বাম_padding);

    inner_width = pum_width + pum_scrollbar + বাম_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
	col = pum_col + বাম_padding + 1;
    অন্যথা
#endif
	col = pum_col - বাম_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	screen_putchar(pum_border_chars.শীর্ষ_ডান, শীর্ষ_row, col, attr);
	যদি (বাম_margin)
	    screen_putchar(' ', শীর্ষ_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_ডান, bottom_row, col, attr);
	যদি (বাম_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    অন্যথা
#endif
    {
	screen_putchar(pum_border_chars.শীর্ষ_বাম, শীর্ষ_row, col, attr);
	যদি (বাম_margin)
	    screen_putchar(' ', শীর্ষ_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_বাম, bottom_row, col, attr);
	যদি (বাম_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    প্রতি (পূর্ণাঙ্ক i = 0; i < inner_width; i++)
    {
	পূর্ণাঙ্ক cur_col;
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    cur_col = col - 1 - i;
	অন্যথা
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.শীর্ষ, শীর্ষ_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	screen_putchar(pum_border_chars.শীর্ষ_বাম, শীর্ষ_row,
		col - inner_width - 1, attr);
	যদি (ডান_margin)
	    screen_putchar(' ', শীর্ষ_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_বাম, bottom_row,
		col - inner_width - 1, attr);
	যদি (ডান_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    অন্যথা
#endif
    {
	screen_putchar(pum_border_chars.শীর্ষ_ডান, শীর্ষ_row,
		col + inner_width + 1, attr);
	যদি (ডান_margin)
	    screen_putchar(' ', শীর্ষ_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_ডান, bottom_row,
		col + inner_width + 1, attr);
	যদি (ডান_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    প্রতি (পূর্ণাঙ্ক i = 0; i < pum_height; i++)
    {
	পূর্ণাঙ্ক row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	{
	    screen_putchar(pum_border_chars.বাম, row, col, attr);
	    যদি (বাম_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.ডান, row,
		    col - inner_width - 1, attr);
	    যদি (ডান_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	অন্যথা
#endif
	{
	    screen_putchar(pum_border_chars.বাম, row, col, attr);
	    যদি (বাম_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.ডান, row,
		    col + inner_width + 1, attr);
	    যদি (ডান_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    শূন্য
put_shadow_char(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    char_u  buf[MB_MAXBYTES + 1];
    পূর্ণাঙ্ক	    shadow_attr = highlight_attr[HLF_PMS];
    পূর্ণাঙ্ক	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    স্থির শূন্য
pum_draw_shadow(শূন্য)
{
    পূর্ণাঙ্ক	row, col, next_col;
    পূর্ণাঙ্ক	width, offset;
    পূর্ণাঙ্ক	inner_width;
    পূর্ণাঙ্ক	ডান_margin, বাম_margin, বাম_padding;

    যদি (!pum_shadow)
	প্রত্যাগম;

    (শূন্য)compute_margins(&ডান_margin, &বাম_margin, &বাম_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + ডান_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    অন্যথা
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    প্রতি (পূর্ণাঙ্ক i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + বাম_padding + pum_border + বাম_margin;
    offset = 2 - বাম_padding - pum_border - বাম_margin;

    প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    col = pum_col - offset - i;
	অন্যথা
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
