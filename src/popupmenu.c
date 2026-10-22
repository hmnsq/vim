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

স্থির pumitem_T *pum_array = ফাঁকা;	// items of displayed pum
স্থির পূর্ণসংখ্যা pum_আকার;			// nr of items in "pum_array"
স্থির পূর্ণসংখ্যা pum_selected;		// index of selected item or -1
স্থির পূর্ণসংখ্যা pum_first = 0;		// index of top item

স্থির পূর্ণসংখ্যা call_update_screen = FALSE;
স্থির পূর্ণসংখ্যা pum_in_cmdline = FALSE;

স্থির পূর্ণসংখ্যা pum_height;			// nr of displayed pum items
স্থির পূর্ণসংখ্যা pum_width;			// width of displayed pum items
স্থির পূর্ণসংখ্যা pum_base_width;		// width of pum items base
স্থির পূর্ণসংখ্যা pum_kind_width;		// width of pum items kind column
স্থির পূর্ণসংখ্যা pum_extra_width;		// width of extra stuff
স্থির পূর্ণসংখ্যা pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
স্থির পূর্ণসংখ্যা pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

স্থির পূর্ণসংখ্যা pum_row;			// top row of pum
স্থির পূর্ণসংখ্যা pum_col;			// left column of pum

স্থির win_T *pum_window = ফাঁকা;
স্থির পূর্ণসংখ্যা pum_win_row;
স্থির পূর্ণসংখ্যা pum_win_height;
স্থির পূর্ণসংখ্যা pum_win_col;
স্থির পূর্ণসংখ্যা pum_win_wcol;
স্থির পূর্ণসংখ্যা pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
স্থির পূর্ণসংখ্যা pum_pretend_not_visible = FALSE;

// Border support
স্থির পূর্ণসংখ্যা pum_border = 0;
স্থির পূর্ণসংখ্যা pum_margin = 0;	// margin of 1 cell on left and right
স্থির পূর্ণসংখ্যা pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
স্থির image_placement_T *pum_imagep = ফাঁকা;
#endif

// Border characters
স্থির কাঠামো {
    পূর্ণসংখ্যা শীর্ষ_2;
    পূর্ণসংখ্যা ডান;
    পূর্ণসংখ্যা bottom;
    পূর্ণসংখ্যা বাম;
    পূর্ণসংখ্যা শীর্ষ_বাম;
    পূর্ণসংখ্যা শীর্ষ_ডান;
    পূর্ণসংখ্যা bottom_ডান;
    পূর্ণসংখ্যা bottom_বাম;
} pum_border_chars;

স্থির পূর্ণসংখ্যা pum_set_selected(পূর্ণসংখ্যা n, পূর্ণসংখ্যা repeat);
স্থির শূন্য pum_draw_border(শূন্য);
স্থির শূন্য pum_draw_shadow(শূন্য);
স্থির শূন্য compute_margins(পূর্ণসংখ্যা *ডান_margin, পূর্ণসংখ্যা *বাম_margin, পূর্ণসংখ্যা *বাম_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	শূন্য \
    pum_set_##feature(পূর্ণসংখ্যা enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    স্থির শূন্য
pum_compute_আকার(শূন্য)
{
    পূর্ণসংখ্যা	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    জন্য (পূর্ণসংখ্যা i = 0; i < pum_আকার; ++i)
    {
	যদি (pum_array[i].pum_text != ফাঁকা)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    যদি (pum_base_width < w)
		pum_base_width = w;
	}
	যদি (pum_array[i].pum_kind != ফাঁকা)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    যদি (pum_kind_width < w)
		pum_kind_width = w;
	}
	যদি (pum_array[i].pum_extra != ফাঁকা)
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
	পূর্ণসংখ্যা আকার_2,
	পূর্ণসংখ্যা above_row,
	পূর্ণসংখ্যা below_row)
{
    পূর্ণসংখ্যা context_সারিগুলি;
    পূর্ণসংখ্যা cline_visible_offset;
    পূর্ণসংখ্যা extra_height = 2 * pum_border + pum_shadow;
    পূর্ণসংখ্যা extra_offset = pum_border + pum_shadow;

    pum_height = MIN(আকার_2, PUM_DEF_HEIGHT);
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

	যদি (pum_win_row >= আকার_2 + context_সারিগুলি + extra_height)
	{
	    pum_row = pum_win_row - আকার_2 - context_সারিগুলি - extra_offset;
	    pum_height = আকার_2;
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
	pum_height = MIN(below_row - pum_row - extra_offset, আকার_2);
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
    স্থির পূর্ণসংখ্যা
set_pum_width_aligned_with_cursor(পূর্ণসংখ্যা width, পূর্ণসংখ্যা available_width)
{
    পূর্ণসংখ্যা end_padding = TRUE;

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
    ফেরত available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    স্থির শূন্য
pum_compute_horizontal_placement(পূর্ণসংখ্যা cursor_col)
{
    পূর্ণসংখ্যা desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    পূর্ণসংখ্যা available_width;
    পূর্ণসংখ্যা col_offset;
    পূর্ণসংখ্যা trailing_overhead = pum_scrollbar + pum_border
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
	ফেরত;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    যদি (available_width > p_pw)
    {
	pum_width = available_width;
	ফেরত;
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
	ফেরত;
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
	পূর্ণসংখ্যা	    আকার_2,
	পূর্ণসংখ্যা	    selected,   // index of initially selected item, -1 if
				// out of range
	পূর্ণসংখ্যা	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    পূর্ণসংখ্যা	    cursor_col;
    পূর্ণসংখ্যা	    above_row;
    পূর্ণসংখ্যা	    below_row;
    পূর্ণসংখ্যা	    redo_গুনতি = 0;
#যদি defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    করো
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = ফাঁকা;

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
		বিরতি;
	যদি (pvwin != ফাঁকা)
	{
	    যদি (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    অন্যথা যদি (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(আকার_2, above_row, below_row);

	// don't display when we only have room for one line
	যদি (pum_height < 1 || (pum_height == 1 && আকার_2 > 1))
	    ফেরত;

	pum_array = array;
	pum_আকার = আকার_2;
	pum_compute_আকার();

	// Calculate cursor column
	যদি (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	অন্যথা
	{
	    পূর্ণসংখ্যা wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
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
	pum_scrollbar = (pum_height < আকার_2) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_col);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } যখন (pum_set_selected(selected, redo_গুনতি) && ++redo_গুনতি <= 2);

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
    পূর্ণসংখ্যা
pum_under_menu(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, পূর্ণসংখ্যা only_redrawing)
{
    পূর্ণসংখ্যা	extra_বাম = pum_border + (pum_margin && pum_border ? 1 : 0);
    পূর্ণসংখ্যা	extra_ডান = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    পূর্ণসংখ্যা	extra_above = pum_border;
    পূর্ণসংখ্যা	extra_below = pum_border + (pum_shadow ? 1 : 0);
    পূর্ণসংখ্যা	শীর্ষ_2 = pum_row - extra_above;
    পূর্ণসংখ্যা	bot = pum_row + pum_height + extra_below;
    পূর্ণসংখ্যা	বাম = pum_col - 1 - extra_বাম;
    পূর্ণসংখ্যা	ডান = pum_col + pum_width + pum_scrollbar + extra_ডান;

    যদি (!((!only_redrawing || pum_will_redraw)
	    && row >= শীর্ষ_2 && row < bot && col >= বাম && col < ডান))
	ফেরত FALSE;

    যদি (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	যদি (only_redrawing)
	{
	    যদি (col >= ডান - 2 || row == bot - 1)
		ফেরত FALSE;
	}
	অন্যথা
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    পূর্ণসংখ্যা	    ডান_margin, বাম_margin, বাম_padding;

	    compute_margins(&ডান_margin, &বাম_margin, &বাম_padding);
	    যদি (row == শীর্ষ_2 && col >= ডান - 2)
		ফেরত FALSE;
	    যদি (row == bot - 1 && col < pum_col + 2 - বাম_padding - pum_border
		    - বাম_margin)
		ফেরত FALSE;
	}
    }
    ফেরত TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    স্থির পূর্ণসংখ্যা *
pum_compute_text_attrs(char_u *text, hlf_T hlf, পূর্ণসংখ্যা user_hlattr)
{
    পূর্ণসংখ্যা		i;
    size_t	leader_len;
    পূর্ণসংখ্যা		char_cells;
    পূর্ণসংখ্যা		new_attr;
    char_u	*ptr = text;
    পূর্ণসংখ্যা		cell_idx = 0;
    garray_T	*ga = ফাঁকা;
    পূর্ণসংখ্যা		*attrs = ফাঁকা;
    char_u	*leader = ফাঁকা;
    পূর্ণসংখ্যা		in_fuzzy;
    পূর্ণসংখ্যা		matched_len = -1;
    int_u	char_pos = 0;
    পূর্ণসংখ্যা		is_select = FALSE;

    যদি (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ফেরত ফাঁকা;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    যদি (leader == ফাঁকা || *leader == NUL)
	ফেরত ফাঁকা;

    attrs = ALLOC_MULT(পূর্ণসংখ্যা, vim_strsize(text));
    যদি (attrs == ফাঁকা)
	ফেরত ফাঁকা;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    যদি (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	যদি (!ga)
	{
	    vim_free(attrs);
	    ফেরত ফাঁকা;
	}
    }

    যখন (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	যদি (ga != ফাঁকা)
	{
	    // Handle fuzzy matching
	    জন্য (i = 0; i < ga->ga_len; i++)
	    {
		যদি (char_pos == ((int_u *)ga->ga_তথ্য)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    বিরতি;
		}
	    }
	}
	অন্যথা
	{
	    যদি (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (পূর্ণসংখ্যা)leader_len;
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
	জন্য (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    যদি (ga != ফাঁকা)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ফেরত attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    স্থির শূন্য
pum_screen_puts_with_attrs(
    পূর্ণসংখ্যা		row,
    পূর্ণসংখ্যা		col,
    পূর্ণসংখ্যা		cells UNUSED,
    char_u	*text,
    পূর্ণসংখ্যা		textlen,
    পূর্ণসংখ্যা		*attrs)
{
    পূর্ণসংখ্যা		col_start = col;
    char_u	*ptr = text;
    পূর্ণসংখ্যা		char_len;
    পূর্ণসংখ্যা		attr;

    // Render text with proper attributes
    যখন (*ptr != NUL && ptr < text + textlen)
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

    স্থির অন্তঃরেখ char_u *
pum_get_item(পূর্ণসংখ্যা সূচক, পূর্ণসংখ্যা type)
{
    নির্বাচন(type)
    {
	ক্ষেত্র CPT_ABBR: ফেরত pum_array[সূচক].pum_text;
	ক্ষেত্র CPT_KIND: ফেরত pum_array[সূচক].pum_kind;
	ক্ষেত্র CPT_MENU: ফেরত pum_array[সূচক].pum_extra;
    }
    ফেরত ফাঁকা;
}

    স্থির অন্তঃরেখ পূর্ণসংখ্যা
pum_user_attr_combine(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা type, পূর্ণসংখ্যা attr)
{
    পূর্ণসংখ্যা user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    ফেরত user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    স্থির পূর্ণসংখ্যা
pum_display_rtl_text(
	পূর্ণসংখ্যা	row,
	পূর্ণসংখ্যা     col,
	char_u  *st,
	পূর্ণসংখ্যা     attr,
	পূর্ণসংখ্যা     *attrs,
	পূর্ণসংখ্যা     width,
	পূর্ণসংখ্যা     width_limit,
	পূর্ণসংখ্যা     totwidth,
	পূর্ণসংখ্যা     পরবর্তী_isempty,
	পূর্ণসংখ্যা	selected)
{
    char_u  *rt = ফাঁকা;
    পূর্ণসংখ্যা     cells = 0;
    পূর্ণসংখ্যা     over_cell = 0;
    পূর্ণসংখ্যা     truncated = FALSE;
    পূর্ণসংখ্যা     pad = পরবর্তী_isempty ? 0 : 2;
    পূর্ণসংখ্যা     remaining = 0;
    পূর্ণসংখ্যা	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    পূর্ণসংখ্যা	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    যদি (st == ফাঁকা)
	ফেরত col;

    rt = reverse_text(st);
    যদি (rt == ফাঁকা)
    {
	VIM_CLEAR(st);
	ফেরত col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    যদি (cells > width_limit)
    {
	করো
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} যখন (cells > width_limit);

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
	পূর্ণসংখ্যা     আকার_2 = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	যদি (cells > remaining)
	{
	    যখন (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	আকার_2 = (পূর্ণসংখ্যা)STRLEN(orig_rt);
	যদি (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, আকার_2);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	যদি (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    যদি (attrs == ফাঁকা)
	screen_puts_len(rt, (পূর্ণসংখ্যা)STRLEN(rt), row, col - cells + 1, attr);
    অন্যথা
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(পূর্ণসংখ্যা)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    ফেরত col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    স্থির পূর্ণসংখ্যা
pum_display_ltr_text(
	পূর্ণসংখ্যা     row,
	পূর্ণসংখ্যা     col,
	char_u  *st,
	পূর্ণসংখ্যা     attr,
	পূর্ণসংখ্যা     *attrs,
	পূর্ণসংখ্যা     width,        // width already calculated in outer loop
	পূর্ণসংখ্যা     width_limit,
	পূর্ণসংখ্যা     totwidth,
	পূর্ণসংখ্যা     পরবর্তী_isempty,
	পূর্ণসংখ্যা	selected)
{
    পূর্ণসংখ্যা     আকার_2 = 0;
    পূর্ণসংখ্যা     cells = 0;
    char_u  *st_end = ফাঁকা;
    পূর্ণসংখ্যা     over_cell = 0;
    পূর্ণসংখ্যা     pad = পরবর্তী_isempty ? 0 : 2;
    পূর্ণসংখ্যা     truncated = FALSE;
    পূর্ণসংখ্যা     remaining = 0;
    পূর্ণসংখ্যা	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    পূর্ণসংখ্যা	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    যদি (st == ফাঁকা)
	ফেরত col;

    আকার_2 = (পূর্ণসংখ্যা)STRLEN(st);
    cells = (*mb_string2cells)(st, আকার_2);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    যখন (আকার_2 > 0 && col + cells > width_limit + pum_col)
    {
	--আকার_2;
	যদি (has_mbyte)
	{
	    আকার_2 -= (*mb_শীর্ষ_off)(st, st + আকার_2);
	    cells -= (*mb_ptr2cells)(st + আকার_2);
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
	    st_end = st + আকার_2;
	    যখন (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    আকার_2 = st_end - st;
	}

	যদি (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, আকার_2);
	width = cells + over_cell + 1;
    }

    যদি (attrs == ফাঁকা)
	screen_puts_len(st, আকার_2, row, col, attr);
    অন্যথা
	pum_screen_puts_with_attrs(row, col, cells, st, আকার_2, attrs);

    যদি (truncated)
    {
	যদি (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ফেরত col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    স্থির পূর্ণসংখ্যা
pum_process_item(
	পূর্ণসংখ্যা     row,
	পূর্ণসংখ্যা     col,
	পূর্ণসংখ্যা     idx,
	পূর্ণসংখ্যা     j,         // Current position in order array
	পূর্ণসংখ্যা     *order,    // Order array
	hlf_T   hlf,
	পূর্ণসংখ্যা     attr,
	পূর্ণসংখ্যা     *totwidth_ptr,
	পূর্ণসংখ্যা     পরবর্তী_isempty)
{
    পূর্ণসংখ্যা     item_type = order[j];
    char_u  *s = ফাঁকা;
    char_u  *p = pum_get_item(idx, item_type);
    পূর্ণসংখ্যা     width = 0;  // item width
    পূর্ণসংখ্যা     w;		// char width
    পূর্ণসংখ্যা	    selected = idx == pum_selected;

    জন্য ( ; ; MB_PTR_ADV(p))
    {
	যদি (s == ফাঁকা)
	    s = p;
	w = ptr2cells(p);
	যদি (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    চালু;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	পূর্ণসংখ্যা     *attrs = ফাঁকা;
	পূর্ণসংখ্যা     saved = *p;

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
		    width, pum_width, *totwidth_ptr, পরবর্তী_isempty, selected);
	অন্যথা
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, পরবর্তী_isempty, selected);

	যদি (attrs != ফাঁকা)
	    VIM_CLEAR(attrs);

	যদি (*p != TAB)
	    বিরতি;

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
	s = ফাঁকা;  // start text at next char
	width = 0;
    }

    ফেরত col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    স্থির শূন্য
pum_draw_scrollbar(
	পূর্ণসংখ্যা	row,
	পূর্ণসংখ্যা	i,
	পূর্ণসংখ্যা	thumb_pos,
	পূর্ণসংখ্যা	thumb_height)
{
    যদি (pum_scrollbar <= 0)
	ফেরত;

    পূর্ণসংখ্যা attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    অন্যথা
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    স্থির অন্তঃরেখ শূন্য
pum_align_order(পূর্ণসংখ্যা *order)
{
    পূর্ণসংখ্যা is_default = cia_flags == 0;
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
    পূর্ণসংখ্যা k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_সারিগুলি);
    VIM_CLEAR(pum_bg_সারিগুলি_UC);
    জন্য (k = 0; k < MAX_MCO; ++k)
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
    পূর্ণসংখ্যা		row = pum_row;
    পূর্ণসংখ্যা		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    পূর্ণসংখ্যা		attr;
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		idx;
    char_u	*p = ফাঁকা;
    পূর্ণসংখ্যা		totwidth;
    পূর্ণসংখ্যা		thumb_pos = 0;
    পূর্ণসংখ্যা		thumb_height = 1;
    পূর্ণসংখ্যা		item_type;
    পূর্ণসংখ্যা		order[3];
    পূর্ণসংখ্যা		পরবর্তী_isempty = FALSE;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		বস্তুসমূহ_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    পূর্ণসংখ্যা		basic_width;  // first item width
    পূর্ণসংখ্যা		last_isabbr = FALSE;
    পূর্ণসংখ্যা		orig_attr = -1;
    পূর্ণসংখ্যা		scroll_range = pum_আকার - pum_height;
    bool	override_success;
    পূর্ণসংখ্যা		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ঠেলো_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    যদি (pum_imagep == ফাঁকা)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ফাঁকা, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    যদি (pum_imagep != ফাঁকা)
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
	যদি (opacity_active && pum_bg_সারিগুলি != ফাঁকা
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	যদি (opacity_active && pum_bg_সারিগুলি != ফাঁকা)
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
	    যদি (Screen_সারিগুলি != ফাঁকা && ScreenAttrs != ফাঁকা)
	    {
		পূর্ণসংখ্যা	save_শীর্ষ, save_bot, save_ncells, k;

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
			জন্য (k = 0; k < MAX_MCO; ++k)
			    pum_bg_সারিগুলি_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    যদি (pum_bg_attrs != ফাঁকা && pum_bg_সারিগুলি != ফাঁকা)
		    {
			জন্য (পূর্ণসংখ্যা r = save_শীর্ষ; r < save_bot; ++r)
			{
			    পূর্ণসংখ্যা soff = (r - save_শীর্ষ) * screen_Columns;
			    পূর্ণসংখ্যা loff = সারি_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * আকার(sattr_T));
			    mch_memmove(pum_bg_সারিগুলি + soff,
				    Screen_সারিগুলি + loff,
				    screen_Columns * আকার(schar_T));
			    যদি (enc_utf8 && pum_bg_সারিগুলি_UC != ফাঁকা
						&& Screen_সারিগুলি_UC != ফাঁকা)
			    {
				mch_memmove(pum_bg_সারিগুলি_UC + soff,
					Screen_সারিগুলি_UC + loff,
					screen_Columns * আকার(u8char_T));
				জন্য (k = 0; k < MAX_MCO; ++k)
				    যদি (pum_bg_সারিগুলি_C[k] != ফাঁকা
						    && Screen_সারিগুলি_C[k] != ফাঁকা)
					mch_memmove(pum_bg_সারিগুলি_C[k] + soff,
						Screen_সারিগুলি_C[k] + loff,
						screen_Columns
						    * আকার(u8char_T));
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
	thumb_height = pum_height * pum_height / pum_আকার;
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
	screen_pum_blend = 100 - (পূর্ণসংখ্যা)p_po;

    জন্য (i = 0; i < pum_height; ++i)
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
	জন্য (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    যদি (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    যদি (j + 1 < 3)
		পরবর্তী_isempty = pum_get_item(idx, order[j + 1]) == ফাঁকা;
	    অন্যথা
		পরবর্তী_isempty = TRUE;

	    যদি (p != ফাঁকা)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, পরবর্তী_isempty);

	    যদি (j > 0)
		n = বস্তুসমূহ_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    অন্যথা
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    যদি (j == 2
		    || (পরবর্তী_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == ফাঁকা)))
		    || basic_width + n >= pum_width)
		বিরতি;
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
    পূর্ণসংখ্যা col = pum_col + pum_width + pum_scrollbar + 1;
    পূর্ণসংখ্যা row = pum_row;
    পূর্ণসংখ্যা botpos = POPPOS_BOTLEFT;
    পূর্ণসংখ্যা	used_maxwidth_opt = FALSE;
    পূর্ণসংখ্যা	ডান_margin, বাম_margin, dummy;
    পূর্ণসংখ্যা বাম_extra_width, ডান_extra_width;

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
    স্থির পূর্ণসংখ্যা
pum_set_selected(পূর্ণসংখ্যা n, পূর্ণসংখ্যা repeat UNUSED)
{
    পূর্ণসংখ্যা	    resized = FALSE;
    পূর্ণসংখ্যা	    context = pum_height / 2;
    পূর্ণসংখ্যা	    scroll_offset;
#ifdef FEAT_QUICKFIX
    পূর্ণসংখ্যা	    পূর্ব_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    পূর্ণসংখ্যা	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    যদি (pum_selected >= 0 && pum_selected < pum_আকার)
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
	pum_first = MIN(pum_first, pum_আকার - pum_height);

#যদি defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	যদি (pum_array[pum_selected].pum_info != ফাঁকা
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
		পূর্ণসংখ্যা	res = OK;

		যদি (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ফাঁকা
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    যখন (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		অন্যথা
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ফাঁকা, ফাঁকা, ফাঁকা, ECMD_ONE, 0, ফাঁকা);
		    --no_u_sync;
		    যদি (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_মান_give_err((char_u *)"swf",
							  0L, ফাঁকা, OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bl",
							  0L, ফাঁকা, OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_মান_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_মান_give_err((char_u *)"diff",
							  0L, ফাঁকা, OPT_LOCAL);
		    }
		}
		যদি (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    জন্য (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			যদি (e == ফাঁকা)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    বিরতি;
			}
			*e = NUL;
			ml_append(lnum++, p, (পূর্ণসংখ্যা)(e - p + 1), FALSE);
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
			    win_setheight((পূর্ণসংখ্যা)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    যদি (pum_selected != পূর্ব_selected)
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
			পূর্ণসংখ্যা save_redr_status;

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

    ফেরত resized;
}

/*
 * Undisplay the popup menu (later).
 */
    শূন্য
pum_undisplay(শূন্য)
{
#ifdef FEAT_IMAGE_POPUP
    যদি (pum_imagep != ফাঁকা)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ফাঁকা;
    }
#endif
    pum_free_bg();
    pum_array = ফাঁকা;
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
    পূর্ণসংখ্যা
pum_visible(শূন্য)
{
    ফেরত !pum_pretend_not_visible && pum_array != ফাঁকা;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    স্থির পূর্ণসংখ্যা
pum_in_same_position(শূন্য)
{
    পূর্ণসংখ্যা	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    ফেরত pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    পূর্ণসংখ্যা
pum_redraw_in_same_position(শূন্য)
{
    যদি (!pum_visible() || pum_will_redraw)
	ফেরত FALSE;  // nothing to do

    ফেরত pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    শূন্য
pum_may_redraw(শূন্য)
{
    pumitem_T	*array = pum_array;
    পূর্ণসংখ্যা		len = pum_আকার;
    পূর্ণসংখ্যা		selected = pum_selected;

    যদি (!pum_visible() || pum_will_redraw)
	ফেরত;  // nothing to do

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
    পূর্ণসংখ্যা
pum_get_height(শূন্য)
{
    ফেরত pum_height;
}

#যদি defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    শূন্য
pum_set_event_info(dict_T *dict)
{
    যদি (!pum_visible())
	ফেরত;
    (শূন্য)dict_add_সংখ্যা(dict, "height", pum_height);
    (শূন্য)dict_add_সংখ্যা(dict, "width", pum_width);
    (শূন্য)dict_add_সংখ্যা(dict, "row", pum_row);
    (শূন্য)dict_add_সংখ্যা(dict, "col", pum_col);
    (শূন্য)dict_add_সংখ্যা(dict, "size", pum_আকার);
    (শূন্য)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#যদি defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    স্থির শূন্য
pum_position_at_mouse(পূর্ণসংখ্যা min_width)
{
    পূর্ণসংখ্যা extra_height = pum_border * 2 + pum_shadow;
    পূর্ণসংখ্যা extra_offset = pum_border + pum_shadow;
    পূর্ণসংখ্যা trailing_overhead = 0;
    পূর্ণসংখ্যা col_offset = 0;
    পূর্ণসংখ্যা available_width;

    যদি (Rows - mouse_row - extra_height > pum_আকার
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
	pum_row = mouse_row - pum_আকার - extra_offset;
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
    pum_window = ফাঁকা;
}

#endif

#যদি defined(FEAT_BEVAL_TERM)
স্থির pumitem_T *balloon_array = ফাঁকা;
স্থির পূর্ণসংখ্যা balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

প্রকারলেখ কাঠামো {
    char_u	*start;
    পূর্ণসংখ্যা		bytelen;
    পূর্ণসংখ্যা		cells;
    পূর্ণসংখ্যা		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    পূর্ণসংখ্যা
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    পূর্ণসংখ্যা		quoted = FALSE;
    পূর্ণসংখ্যা		height;
    পূর্ণসংখ্যা		সারি;
    পূর্ণসংখ্যা		item_idx;
    পূর্ণসংখ্যা		indent = 0;
    পূর্ণসংখ্যা		max_cells = 0;
    পূর্ণসংখ্যা		max_height = Rows / 2 - 1;
    পূর্ণসংখ্যা		long_item_গুনতি = 0;
    পূর্ণসংখ্যা		split_long_বস্তুসমূহ = FALSE;

    ga_init2(&ga, আকার(balpart_T), 20);
    p = mesg;

    যখন (*p != NUL)
    {
	যদি (ga_grow(&ga, 1) == FAIL)
	    যাও failed;
	item = ((balpart_T *)ga.ga_তথ্য) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	যখন (*p != NUL)
	{
	    যদি (*p == '"')
		quoted = !quoted;
	    অন্যথা যদি (*p == '\n')
		বিরতি;
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
		    বিরতি;
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
    যদি (*array == ফাঁকা)
	যাও failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    জন্য (সারি = 1, item_idx = 0; সারি < height - 1; ++item_idx)
    {
	পূর্ণসংখ্যা	skip;
	পূর্ণসংখ্যা	thislen;
	পূর্ণসংখ্যা	copylen;
	পূর্ণসংখ্যা	ind;
	পূর্ণসংখ্যা	cells;

	item = ((balpart_T *)ga.ga_তথ্য) + item_idx;
	যদি (item->bytelen == 0)
	    (*array)[সারি++].pum_text = vim_strsave((char_u *)"");
	অন্যথা
	    জন্য (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		যদি (split_long_বস্তুসমূহ && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    জন্য (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			যদি ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    বিরতি;
		    thislen = p - (item->start + skip);
		}
		অন্যথা
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		যদি (p == ফাঁকা)
		{
		    জন্য (সারি = 0; সারি <= height - 1; ++সারি)
			vim_free((*array)[সারি].pum_text);
		    vim_free(*array);
		    যাও failed;
		}
		জন্য (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		জন্য (copylen = thislen; copylen > 0; --copylen)
		    যদি (item->start[skip + copylen - 1] != ' ')
			বিরতি;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[সারি].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++সারি;
	    }
    }
    ga_clear(&ga);
    ফেরত height;

failed:
    ga_clear(&ga);
    ফেরত 0;
}

    শূন্য
ui_remove_balloon(শূন্য)
{
    যদি (balloon_array == ফাঁকা)
	ফেরত;

    pum_undisplay();
    যখন (balloon_arraysize > 0)
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

    যদি (mesg == ফাঁকা && list == ফাঁকা)
    {
	pum_undisplay();
	ফেরত;
    }
    যদি (list != ফাঁকা)
    {
	listitem_T  *li;
	পূর্ণসংখ্যা	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	যদি (balloon_array == ফাঁকা)
	    ফেরত;
	CHECK_LIST_MATERIALIZE(list);
	জন্য (idx = 0, li = list->lv_first; li != ফাঁকা; li = li->li_পরবর্তী, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == ফাঁকা ? (char_u *)"" : text);
	}
    }
    অন্যথা
	balloon_arraysize = split_message(mesg, &balloon_array);

    যদি (balloon_arraysize <= 0)
	ফেরত;

    pum_array = balloon_array;
    pum_আকার = balloon_arraysize;
    pum_compute_আকার();
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
    পূর্ণসংখ্যা idx = mouse_row - pum_row;

    যদি (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    অন্যথা যদি (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    স্থির শূন্য
pum_execute_menu(vimmenu_T *menu, পূর্ণসংখ্যা mode)
{
    vimmenu_T   *mp;
    পূর্ণসংখ্যা		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	যদি ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    বিরতি;
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
    পূর্ণসংখ্যা		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    পূর্ণসংখ্যা		save_bevalterm = p_bevalterm;
# endif
    পূর্ণসংখ্যা		mode;

    pum_undisplay();
    pum_আকার = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	যদি (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_আকার;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    যদি (pum_আকার <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	ফেরত;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_আকার);
    যদি (array == ফাঁকা)
	ফেরত;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = ফাঁকা;

	// Make a copy of the text, the menu may be redefined in a callback.
	যদি (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	অন্যথা যদি (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	যদি (s != ফাঁকা)
	{
	    s = vim_strsave(s);
	    যদি (s != ফাঁকা)
		array[idx++].pum_text = s;
	}
    }

    pum_array = array;
    pum_compute_আকার();
    pum_scrollbar = 0;
    pum_height = pum_আকার;
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

    জন্য (;;)
    {
	পূর্ণসংখ্যা	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	যদি (c == ESC || c == Ctrl_C || pum_array == ফাঁকা)
	    বিরতি;
	অন্যথা যদি (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    বিরতি;
	}
	অন্যথা যদি (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    যখন (pum_selected > 0)
	    {
		--pum_selected;
		যদি (*array[pum_selected].pum_text != NUL)
		    বিরতি;
	    }
	}
	অন্যথা যদি (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    যখন (pum_selected < pum_আকার - 1)
	    {
		++pum_selected;
		যদি (*array[pum_selected].pum_text != NUL)
		    বিরতি;
	    }
	}
	অন্যথা যদি (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    বিরতি;
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
		বিরতি;
	    }
	    যদি (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		বিরতি;
	}
    }

    জন্য (idx = 0; idx < pum_আকার; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    শূন্য
pum_make_popup(char_u *path_name, পূর্ণসংখ্যা use_mouse_pos)
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
    যদি (menu != ফাঁকা)
	pum_show_popupmenu(menu);
}
#endif

    স্থির পূর্ণসংখ্যা
is_singlewidth(পূর্ণসংখ্যা c)
{
    যদি (c >= 0 && c < 0x80)
	ফেরত TRUE;
    যদি (utf_char2cells(c) == 1)
	ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Set border characters for popup menu.
 */
    শূন্য
pum_set_border_chars(
	পূর্ণসংখ্যা শীর্ষ_2,
	পূর্ণসংখ্যা ডান,
	পূর্ণসংখ্যা bottom,
	পূর্ণসংখ্যা বাম,
	পূর্ণসংখ্যা শীর্ষ_বাম,
	পূর্ণসংখ্যা শীর্ষ_ডান,
	পূর্ণসংখ্যা bottom_ডান,
	পূর্ণসংখ্যা bottom_বাম)
{
    যদি (!is_singlewidth(শীর্ষ_2) || !is_singlewidth(ডান)
	    || !is_singlewidth(bottom) || !is_singlewidth(বাম)
	    || !is_singlewidth(শীর্ষ_বাম) || !is_singlewidth(শীর্ষ_ডান)
	    || !is_singlewidth(bottom_ডান) || !is_singlewidth(bottom_বাম))
	ফেরত;
    pum_border_chars.শীর্ষ_2 = শীর্ষ_2;
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
compute_margins(পূর্ণসংখ্যা *ডান_margin, পূর্ণসংখ্যা *বাম_margin, পূর্ণসংখ্যা *বাম_padding)
{
    *ডান_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	*বাম_padding = (pum_col < Columns - 1 - pum_border);
	*বাম_margin = *ডান_margin && (pum_col < Columns - 3);
	ফেরত;
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
    পূর্ণসংখ্যা	শীর্ষ_row = pum_row - 1;
    পূর্ণসংখ্যা	bottom_row = pum_row + pum_height;
    পূর্ণসংখ্যা	col;
    পূর্ণসংখ্যা	attr = highlight_attr[HLF_PMB];
    পূর্ণসংখ্যা	ডান_margin, বাম_margin, বাম_padding;
    পূর্ণসংখ্যা	inner_width;

    যদি (!pum_border)
	ফেরত;

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
    জন্য (পূর্ণসংখ্যা i = 0; i < inner_width; i++)
    {
	পূর্ণসংখ্যা cur_col;
#ifdef FEAT_RIGHTLEFT
	যদি (pum_rl)
	    cur_col = col - 1 - i;
	অন্যথা
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.শীর্ষ_2, শীর্ষ_row, cur_col, attr);
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
    জন্য (পূর্ণসংখ্যা i = 0; i < pum_height; i++)
    {
	পূর্ণসংখ্যা row = pum_row + i;
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
put_shadow_char(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    char_u  buf[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা	    shadow_attr = highlight_attr[HLF_PMS];
    পূর্ণসংখ্যা	    char_attr, new_attr;

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
    পূর্ণসংখ্যা	row, col, পরবর্তী_col;
    পূর্ণসংখ্যা	width, offset;
    পূর্ণসংখ্যা	inner_width;
    পূর্ণসংখ্যা	ডান_margin, বাম_margin, বাম_padding;

    যদি (!pum_shadow)
	ফেরত;

    (শূন্য)compute_margins(&ডান_margin, &বাম_margin, &বাম_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + ডান_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    যদি (pum_rl)
    {
	col = pum_col - inner_width;
	পরবর্তী_col = col - 1;
    }
    অন্যথা
#endif
    {
	col = pum_col + inner_width;
	পরবর্তী_col = col + 1;
    }

    জন্য (পূর্ণসংখ্যা i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, পরবর্তী_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + বাম_padding + pum_border + বাম_margin;
    offset = 2 - বাম_padding - pum_border - বাম_margin;

    জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
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
