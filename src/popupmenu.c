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
#شامل "vim.h"

ایستا pumitem_T *pum_array = پوچ;	// items of displayed pum
ایستا صحیح pum_size;			// nr of items in "pum_array"
ایستا صحیح pum_selected;		// index of selected item or -1
ایستا صحیح pum_first = 0;		// index of top item

ایستا صحیح call_update_screen = FALSE;
ایستا صحیح pum_in_cmdline = FALSE;

ایستا صحیح pum_height;			// nr of displayed pum items
ایستا صحیح pum_width;			// width of displayed pum items
ایستا صحیح pum_base_width;		// width of pum items base
ایستا صحیح pum_kind_width;		// width of pum items kind column
ایستا صحیح pum_extra_width;		// width of extra stuff
ایستا صحیح pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
ایستا صحیح pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

ایستا صحیح pum_row;			// top row of pum
ایستا صحیح pum_col;			// left column of pum

ایستا win_T *pum_window = پوچ;
ایستا صحیح pum_win_row;
ایستا صحیح pum_win_height;
ایستا صحیح pum_win_col;
ایستا صحیح pum_win_wcol;
ایستا صحیح pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ایستا صحیح pum_pretend_not_visible = FALSE;

// Border support
ایستا صحیح pum_border = 0;
ایستا صحیح pum_margin = 0;	// margin of 1 cell on left and right
ایستا صحیح pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
ایستا image_placement_T *pum_imagep = پوچ;
#endif

// Border characters
ایستا ساختار {
    صحیح بالاترین;
    صحیح راست;
    صحیح bottom;
    صحیح چپ;
    صحیح بالاترین_چپ;
    صحیح بالاترین_راست;
    صحیح bottom_راست;
    صحیح bottom_چپ;
} pum_border_chars;

ایستا صحیح pum_set_selected(صحیح n, صحیح repeat);
ایستا تهی pum_draw_border(تهی);
ایستا تهی pum_draw_shadow(تهی);
ایستا تهی compute_margins(صحیح *راست_margin, صحیح *چپ_margin, صحیح *چپ_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	تهی \
    pum_set_##feature(صحیح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ایستا تهی
pum_compute_size(تهی)
{
    صحیح	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    برای (صحیح i = 0; i < pum_size; ++i)
    {
	اگر (pum_array[i].pum_text != پوچ)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    اگر (pum_base_width < w)
		pum_base_width = w;
	}
	اگر (pum_array[i].pum_kind != پوچ)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    اگر (pum_kind_width < w)
		pum_kind_width = w;
	}
	اگر (pum_array[i].pum_extra != پوچ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    اگر (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ایستا تهی
pum_compute_vertical_placement(
	صحیح size,
	صحیح above_row,
	صحیح below_row)
{
    صحیح context_سطرها;
    صحیح cline_visible_offset;
    صحیح extra_height = 2 * pum_border + pum_shadow;
    صحیح extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    اگر (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    اگر (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	اگر (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطرها = 0;
	وگرنه
	    // Leave two lines of context if possible
	    context_سطرها = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	اگر (pum_win_row >= size + context_سطرها + extra_height)
	{
	    pum_row = pum_win_row - size - context_سطرها - extra_offset;
	    pum_height = size;
	}
	وگرنه
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_سطرها - extra_height;
	}
	اگر (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    وگرنه
    {
	// pum below "pum_win_row"
	اگر (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطرها = 0;
	وگرنه
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_سطرها = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_سطرها + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	اگر (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#اگر defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    اگر (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    ایستا صحیح
set_pum_width_aligned_with_cursor(صحیح width, صحیح available_width)
{
    صحیح end_padding = TRUE;

    اگر (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    اگر (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    بازگشت available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ایستا تهی
pum_compute_horizontal_placement(صحیح cursor_col)
{
    صحیح desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    صحیح available_width;
    صحیح col_offset;
    صحیح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    وگرنه
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    اگر (set_pum_width_aligned_with_cursor(desired_width, available_width))
	بازگشت;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    اگر (available_width > p_pw)
    {
	pum_width = available_width;
	بازگشت;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    اگر (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	وگرنه
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	بازگشت;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
	pum_col = Columns - 1 - col_offset;
    وگرنه
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
    تهی
pum_display(
	pumitem_T   *array,
	صحیح	    size,
	صحیح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحیح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحیح	    cursor_col;
    صحیح	    above_row;
    صحیح	    below_row;
    صحیح	    redo_شمار = 0;
#اگر defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    انجامبده
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = پوچ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	اگر (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	وگرنه
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#اگر defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    اگر (pvwin->w_p_pvw)
		بشکن;
	اگر (pvwin != پوچ)
	{
	    اگر (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    وگرنه اگر (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	اگر (pum_height < 1 || (pum_height == 1 && size > 1))
	    بازگشت;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	اگر (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	وگرنه
	{
	    صحیح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    اگر (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    وگرنه
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    اگر (curwin->w_p_cole > 0 && conceal_cursor_سطر(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		اگر (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    اگر (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    وگرنه
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
    } وقتی (pum_set_selected(selected, redo_شمار) && ++redo_شمار <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    تهی
pum_call_update_screen(تهی)
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
    صحیح
pum_under_menu(صحیح row, صحیح col, صحیح only_redrawing)
{
    صحیح	extra_چپ = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحیح	extra_راست = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحیح	extra_above = pum_border;
    صحیح	extra_below = pum_border + (pum_shadow ? 1 : 0);
    صحیح	بالاترین = pum_row - extra_above;
    صحیح	bot = pum_row + pum_height + extra_below;
    صحیح	چپ = pum_col - 1 - extra_چپ;
    صحیح	راست = pum_col + pum_width + pum_scrollbar + extra_راست;

    اگر (!((!only_redrawing || pum_will_redraw)
	    && row >= بالاترین && row < bot && col >= چپ && col < راست))
	بازگشت FALSE;

    اگر (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	اگر (only_redrawing)
	{
	    اگر (col >= راست - 2 || row == bot - 1)
		بازگشت FALSE;
	}
	وگرنه
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحیح	    راست_margin, چپ_margin, چپ_padding;

	    compute_margins(&راست_margin, &چپ_margin, &چپ_padding);
	    اگر (row == بالاترین && col >= راست - 2)
		بازگشت FALSE;
	    اگر (row == bot - 1 && col < pum_col + 2 - چپ_padding - pum_border
		    - چپ_margin)
		بازگشت FALSE;
	}
    }
    بازگشت TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ایستا صحیح *
pum_compute_text_attrs(char_u *text, hlf_T hlf, صحیح user_hlattr)
{
    صحیح		i;
    size_t	leader_len;
    صحیح		char_cells;
    صحیح		new_attr;
    char_u	*ptr = text;
    صحیح		cell_idx = 0;
    garray_T	*ga = پوچ;
    صحیح		*attrs = پوچ;
    char_u	*leader = پوچ;
    صحیح		in_fuzzy;
    صحیح		matched_len = -1;
    int_u	char_pos = 0;
    صحیح		is_select = FALSE;

    اگر (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	بازگشت پوچ;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    اگر (leader == پوچ || *leader == NUL)
	بازگشت پوچ;

    attrs = ALLOC_MULT(صحیح, vim_strsize(text));
    اگر (attrs == پوچ)
	بازگشت پوچ;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    اگر (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	اگر (!ga)
	{
	    vim_free(attrs);
	    بازگشت پوچ;
	}
    }

    وقتی (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	اگر (ga != پوچ)
	{
	    // Handle fuzzy matching
	    برای (i = 0; i < ga->ga_len; i++)
	    {
		اگر (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    بشکن;
		}
	    }
	}
	وگرنه
	{
	    اگر (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (صحیح)leader_len;
	    اگر (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	اگر (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	برای (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    اگر (ga != پوچ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    بازگشت attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ایستا تهی
pum_screen_puts_with_attrs(
    صحیح		row,
    صحیح		col,
    صحیح		cells UNUSED,
    char_u	*text,
    صحیح		textlen,
    صحیح		*attrs)
{
    صحیح		col_start = col;
    char_u	*ptr = text;
    صحیح		char_len;
    صحیح		attr;

    // Render text with proper attributes
    وقتی (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	وگرنه
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ایستا درونخطی char_u *
pum_get_item(صحیح index, صحیح type)
{
    انتخاب(type)
    {
	حالت CPT_ABBR: بازگشت pum_array[index].pum_text;
	حالت CPT_KIND: بازگشت pum_array[index].pum_kind;
	حالت CPT_MENU: بازگشت pum_array[index].pum_extra;
    }
    بازگشت پوچ;
}

    ایستا درونخطی صحیح
pum_user_attr_combine(صحیح idx, صحیح type, صحیح attr)
{
    صحیح user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    بازگشت user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ایستا صحیح
pum_display_rtl_text(
	صحیح	row,
	صحیح     col,
	char_u  *st,
	صحیح     attr,
	صحیح     *attrs,
	صحیح     width,
	صحیح     width_limit,
	صحیح     totwidth,
	صحیح     next_isempty,
	صحیح	selected)
{
    char_u  *rt = پوچ;
    صحیح     cells = 0;
    صحیح     over_cell = 0;
    صحیح     truncated = FALSE;
    صحیح     pad = next_isempty ? 0 : 2;
    صحیح     remaining = 0;
    صحیح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحیح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    اگر (st == پوچ)
	بازگشت col;

    rt = reverse_text(st);
    اگر (rt == پوچ)
    {
	VIM_CLEAR(st);
	بازگشت col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    اگر (cells > width_limit)
    {
	انجامبده
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} وقتی (cells > width_limit);

	اگر (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    اگر (truncated)
    {
	char_u  *orig_rt = rt;
	صحیح     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	اگر (cells > remaining)
	{
	    وقتی (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (صحیح)STRLEN(orig_rt);
	اگر (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	اگر (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    اگر (attrs == پوچ)
	screen_puts_len(rt, (صحیح)STRLEN(rt), row, col - cells + 1, attr);
    وگرنه
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(صحیح)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    بازگشت col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ایستا صحیح
pum_display_ltr_text(
	صحیح     row,
	صحیح     col,
	char_u  *st,
	صحیح     attr,
	صحیح     *attrs,
	صحیح     width,        // width already calculated in outer loop
	صحیح     width_limit,
	صحیح     totwidth,
	صحیح     next_isempty,
	صحیح	selected)
{
    صحیح     size = 0;
    صحیح     cells = 0;
    char_u  *st_end = پوچ;
    صحیح     over_cell = 0;
    صحیح     pad = next_isempty ? 0 : 2;
    صحیح     truncated = FALSE;
    صحیح     remaining = 0;
    صحیح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحیح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    اگر (st == پوچ)
	بازگشت col;

    size = (صحیح)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    وقتی (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	اگر (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	وگرنه
	    --cells;
    }

    // truncated
    اگر (truncated)
    {
	remaining = width_limit - totwidth - 1;
	اگر (cells > remaining)
	{
	    st_end = st + size;
	    وقتی (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	اگر (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    اگر (attrs == پوچ)
	screen_puts_len(st, size, row, col, attr);
    وگرنه
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    اگر (truncated)
    {
	اگر (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    بازگشت col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ایستا صحیح
pum_process_item(
	صحیح     row,
	صحیح     col,
	صحیح     idx,
	صحیح     j,         // Current position in order array
	صحیح     *order,    // Order array
	hlf_T   hlf,
	صحیح     attr,
	صحیح     *totwidth_ptr,
	صحیح     next_isempty)
{
    صحیح     item_type = order[j];
    char_u  *s = پوچ;
    char_u  *p = pum_get_item(idx, item_type);
    صحیح     width = 0;  // item width
    صحیح     w;		// char width
    صحیح	    selected = idx == pum_selected;

    برای ( ; ; MB_PTR_ADV(p))
    {
	اگر (s == پوچ)
	    s = p;
	w = ptr2cells(p);
	اگر (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    ادامه;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	صحیح     *attrs = پوچ;
	صحیح     saved = *p;

	اگر (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	اگر (saved != NUL)
	    *p = saved;

	اگر (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	وگرنه
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	اگر (attrs != پوچ)
	    VIM_CLEAR(attrs);

	اگر (*p != TAB)
	    بشکن;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	وگرنه
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = پوچ;  // start text at next char
	width = 0;
    }

    بازگشت col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ایستا تهی
pum_draw_scrollbar(
	صحیح	row,
	صحیح	i,
	صحیح	thumb_pos,
	صحیح	thumb_height)
{
    اگر (pum_scrollbar <= 0)
	بازگشت;

    صحیح attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    وگرنه
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    ایستا درونخطی تهی
pum_align_order(صحیح *order)
{
    صحیح is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ایستا تهی pum_free_bg(تهی);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    تهی
pum_opacity_changed(تهی)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    اگر (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ایستا تهی
pum_free_bg(تهی)
{
    صحیح k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_سطرها);
    VIM_CLEAR(pum_bg_سطرها_UC);
    برای (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_سطرها_C[k]);
    pum_bg_بالاترین = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    تهی
pum_redraw(تهی)
{
    صحیح		row = pum_row;
    صحیح		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحیح		attr;
    صحیح		i, j;
    صحیح		idx;
    char_u	*p = پوچ;
    صحیح		totwidth;
    صحیح		thumb_pos = 0;
    صحیح		thumb_height = 1;
    صحیح		item_type;
    صحیح		order[3];
    صحیح		next_isempty = FALSE;
    صحیح		n;
    صحیح		اقلام_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    صحیح		basic_width;  // first item width
    صحیح		last_isabbr = FALSE;
    صحیح		orig_attr = -1;
    صحیح		scroll_range = pum_size - pum_height;
    bool	override_success;
    صحیح		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = افزودن_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    اگر (pum_imagep == پوچ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(پوچ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    اگر (pum_imagep != پوچ)
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

    اگر (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	اگر (opacity_active && pum_bg_سطرها != پوچ
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	اگر (opacity_active && pum_bg_سطرها != پوچ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	وگرنه اگر (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    اگر (Screen_سطرها != پوچ && ScreenAttrs != پوچ)
	    {
		صحیح	save_بالاترین, save_bot, save_ncells, k;

		pum_free_bg();
		save_بالاترین = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		اگر (save_بالاترین < 0)
		    save_بالاترین = 0;
		اگر (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_بالاترین = save_بالاترین;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		اگر (save_بالاترین < save_bot)
		{
		    save_ncells = (save_bot - save_بالاترین) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_سطرها = LALLOC_MULT(schar_T, save_ncells);
		    اگر (enc_utf8)
		    {
			pum_bg_سطرها_UC = LALLOC_MULT(u8char_T, save_ncells);
			برای (k = 0; k < MAX_MCO; ++k)
			    pum_bg_سطرها_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    اگر (pum_bg_attrs != پوچ && pum_bg_سطرها != پوچ)
		    {
			برای (صحیح r = save_بالاترین; r < save_bot; ++r)
			{
			    صحیح soff = (r - save_بالاترین) * screen_Columns;
			    صحیح loff = سطر_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * اندازه(sattr_T));
			    mch_memmove(pum_bg_سطرها + soff,
				    Screen_سطرها + loff,
				    screen_Columns * اندازه(schar_T));
			    اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ
						&& Screen_سطرها_UC != پوچ)
			    {
				mch_memmove(pum_bg_سطرها_UC + soff,
					Screen_سطرها_UC + loff,
					screen_Columns * اندازه(u8char_T));
				برای (k = 0; k < MAX_MCO; ++k)
				    اگر (pum_bg_سطرها_C[k] != پوچ
						    && Screen_سطرها_C[k] != پوچ)
					mch_memmove(pum_bg_سطرها_C[k] + soff,
						Screen_سطرها_C[k] + loff,
						screen_Columns
						    * اندازه(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	وگرنه
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

    اگر (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	اگر (thumb_height == 0)
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
    اگر (pum_border)
	pum_draw_border();
    اگر (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    اگر (opacity_active)
	screen_pum_blend = 100 - (صحیح)p_po;

    برای (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    اگر (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	وگرنه
#endif
	    اگر (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = اقلام_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	برای (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    اگر (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    اگر (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == پوچ;
	    وگرنه
		next_isempty = TRUE;

	    اگر (p != پوچ)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    اگر (j > 0)
		n = اقلام_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    وگرنه
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    اگر (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == پوچ)))
		    || basic_width + n >= pum_width)
		بشکن;
#ifdef FEAT_RIGHTLEFT
	    اگر (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    وگرنه
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	وگرنه
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

    اگر (override_success)
	برداشت_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    تهی
pum_position_info_popup(win_T *wp)
{
    صحیح col = pum_col + pum_width + pum_scrollbar + 1;
    صحیح row = pum_row;
    صحیح botpos = POPPOS_BOTLEFT;
    صحیح	used_maxwidth_opt = FALSE;
    صحیح	راست_margin, چپ_margin, dummy;
    صحیح چپ_extra_width, راست_extra_width;

    (تهی)compute_margins(&راست_margin, &چپ_margin, &dummy);

    چپ_extra_width = pum_border + چپ_margin;
    راست_extra_width = pum_border + راست_margin + (pum_shadow ? 2 : 0);
    col += راست_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    اگر (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - چپ_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - چپ_extra_width;
    }
    وگرنه
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    اگر (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_بالاترین_extra(wp);
    اگر (wp->w_popup_flags & POPF_INFO_MENU)
    {
	اگر (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	وگرنه
	    // menu below cursor line, align with top
	    row += 1;
    }
    وگرنه
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    اگر (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    وگرنه
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
    ایستا صحیح
pum_set_selected(صحیح n, صحیح repeat UNUSED)
{
    صحیح	    resized = FALSE;
    صحیح	    context = pum_height / 2;
    صحیح	    scroll_offset;
#ifdef FEAT_QUICKFIX
    صحیح	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحیح	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    اگر (pum_selected >= 0 && pum_selected < pum_size)
    {
	اگر (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    اگر (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		اگر (pum_first < 0)
		    pum_first = 0;
		وگرنه اگر (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    وگرنه
		pum_first = pum_selected;
	}
	وگرنه اگر (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    اگر (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    وگرنه
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	اگر (pum_height > 2)
	{
	    اگر (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    وگرنه اگر (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#اگر defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	اگر (pum_array[pum_selected].pum_info != پوچ
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# وگرنه
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    اگر (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    وگرنه اگر (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    وگرنه
		use_popup = USEPOPUP_NONE;
	    اگر (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    اگر (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    اگر (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    اگر (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		صحیح	res = OK;

		اگر (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == پوچ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    وقتی (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		وگرنه
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, پوچ, پوچ, پوچ, ECMD_ONE, 0, پوچ);
		    --no_u_sync;
		    اگر (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_مقدار_give_err((char_u *)"swf",
							  0L, پوچ, OPT_LOCAL);
			set_option_مقدار_give_err((char_u *)"bl",
							  0L, پوچ, OPT_LOCAL);
			set_option_مقدار_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_مقدار_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_مقدار_give_err((char_u *)"diff",
							  0L, پوچ, OPT_LOCAL);
		    }
		}
		اگر (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    برای (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			اگر (e == پوچ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    بشکن;
			}
			*e = NUL;
			ml_append(lnum++, p, (صحیح)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_سطر_شمار);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    اگر (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			اگر (curwin->w_height < lnum)
			{
			    win_setheight((صحیح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    اگر (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    وگرنه اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_شمار)
			curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    اگر (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			اگر (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    اگر ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			صحیح save_redr_status;

			اگر (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			اگر (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			اگر (resized && win_valid(curwin_save))
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

			اگر (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    اگر (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    اگر (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    اگر (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    اگر (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    بازگشت resized;
}

/*
 * Undisplay the popup menu (later).
 */
    تهی
pum_undisplay(تهی)
{
#ifdef FEAT_IMAGE_POPUP
    اگر (pum_imagep != پوچ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = پوچ;
    }
#endif
    pum_free_bg();
    pum_array = پوچ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    اگر (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    تهی
pum_clear(تهی)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    صحیح
pum_visible(تهی)
{
    بازگشت !pum_pretend_not_visible && pum_array != پوچ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ایستا صحیح
pum_in_same_position(تهی)
{
    صحیح	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    بازگشت pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحیح
pum_redraw_in_same_position(تهی)
{
    اگر (!pum_visible() || pum_will_redraw)
	بازگشت FALSE;  // nothing to do

    بازگشت pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    تهی
pum_may_redraw(تهی)
{
    pumitem_T	*array = pum_array;
    صحیح		len = pum_size;
    صحیح		selected = pum_selected;

    اگر (!pum_visible() || pum_will_redraw)
	بازگشت;  // nothing to do

    اگر (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    وگرنه
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
    صحیح
pum_get_height(تهی)
{
    بازگشت pum_height;
}

#اگر defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    تهی
pum_set_event_info(dict_T *dict)
{
    اگر (!pum_visible())
	بازگشت;
    (تهی)dict_add_عدد(dict, "height", pum_height);
    (تهی)dict_add_عدد(dict, "width", pum_width);
    (تهی)dict_add_عدد(dict, "row", pum_row);
    (تهی)dict_add_عدد(dict, "col", pum_col);
    (تهی)dict_add_عدد(dict, "size", pum_size);
    (تهی)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#اگر defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    ایستا تهی
pum_position_at_mouse(صحیح min_width)
{
    صحیح extra_height = pum_border * 2 + pum_shadow;
    صحیح extra_offset = pum_border + pum_shadow;
    صحیح trailing_overhead = 0;
    صحیح col_offset = 0;
    صحیح available_width;

    اگر (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	اگر (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	اگر (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    وگرنه
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	اگر (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	اگر (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	وگرنه
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    وگرنه
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	اگر (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	وگرنه
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = پوچ;
}

#endif

#اگر defined(FEAT_BEVAL_TERM)
ایستا pumitem_T *balloon_array = پوچ;
ایستا صحیح balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

تعریفنوع ساختار {
    char_u	*start;
    صحیح		bytelen;
    صحیح		cells;
    صحیح		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    صحیح
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    صحیح		quoted = FALSE;
    صحیح		height;
    صحیح		سطر;
    صحیح		item_idx;
    صحیح		indent = 0;
    صحیح		max_cells = 0;
    صحیح		max_height = Rows / 2 - 1;
    صحیح		long_item_شمار = 0;
    صحیح		split_long_اقلام = FALSE;

    ga_init2(&ga, اندازه(balpart_T), 20);
    p = mesg;

    وقتی (*p != NUL)
    {
	اگر (ga_grow(&ga, 1) == FAIL)
	    برو failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	وقتی (*p != NUL)
	{
	    اگر (*p == '"')
		quoted = !quoted;
	    وگرنه اگر (*p == '\n')
		بشکن;
	    وگرنه اگر (*p == '\\' && p[1] != NUL)
		++p;
	    وگرنه اگر (!quoted)
	    {
		اگر ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    اگر (*p == '{')
			++indent;
		    وگرنه اگر (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    بشکن;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	اگر (*p == '\n')
	    ++p;
	اگر (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_شمار += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    اگر (long_item_شمار > 0 && height + long_item_شمار <= max_height)
    {
	split_long_اقلام = TRUE;
	height += long_item_شمار;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    اگر (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    اگر (*array == پوچ)
	برو failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    برای (سطر = 1, item_idx = 0; سطر < height - 1; ++item_idx)
    {
	صحیح	skip;
	صحیح	thislen;
	صحیح	copylen;
	صحیح	ind;
	صحیح	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	اگر (item->bytelen == 0)
	    (*array)[سطر++].pum_text = vim_strsave((char_u *)"");
	وگرنه
	    برای (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		اگر (split_long_اقلام && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    برای (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			اگر ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    بشکن;
		    thislen = p - (item->start + skip);
		}
		وگرنه
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		اگر (p == پوچ)
		{
		    برای (سطر = 0; سطر <= height - 1; ++سطر)
			vim_free((*array)[سطر].pum_text);
		    vim_free(*array);
		    برو failed;
		}
		برای (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		برای (copylen = thislen; copylen > 0; --copylen)
		    اگر (item->start[skip + copylen - 1] != ' ')
			بشکن;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[سطر].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++سطر;
	    }
    }
    ga_clear(&ga);
    بازگشت height;

failed:
    ga_clear(&ga);
    بازگشت 0;
}

    تهی
ui_remove_balloon(تهی)
{
    اگر (balloon_array == پوچ)
	بازگشت;

    pum_undisplay();
    وقتی (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    تهی
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    اگر (mesg == پوچ && list == پوچ)
    {
	pum_undisplay();
	بازگشت;
    }
    اگر (list != پوچ)
    {
	listitem_T  *li;
	صحیح	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	اگر (balloon_array == پوچ)
	    بازگشت;
	CHECK_LIST_MATERIALIZE(list);
	برای (idx = 0, li = list->lv_first; li != پوچ; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == پوچ ? (char_u *)"" : text);
	}
    }
    وگرنه
	balloon_arraysize = split_message(mesg, &balloon_array);

    اگر (balloon_arraysize <= 0)
	بازگشت;

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
    تهی
ui_may_remove_balloon(تهی)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#اگر defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ایستا تهی
pum_select_mouse_pos(تهی)
{
    صحیح idx = mouse_row - pum_row;

    اگر (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    وگرنه اگر (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ایستا تهی
pum_execute_menu(vimmenu_T *menu, صحیح mode)
{
    vimmenu_T   *mp;
    صحیح		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	اگر ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    بشکن;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    تهی
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    صحیح		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    صحیح		save_bevalterm = p_bevalterm;
# endif
    صحیح		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	اگر (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    اگر (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	بازگشت;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    اگر (array == پوچ)
	بازگشت;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = پوچ;

	// Make a copy of the text, the menu may be redefined in a callback.
	اگر (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	وگرنه اگر (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	اگر (s != پوچ)
	{
	    s = vim_strsave(s);
	    اگر (s != پوچ)
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

    برای (;;)
    {
	صحیح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	اگر (c == ESC || c == Ctrl_C || pum_array == پوچ)
	    بشکن;
	وگرنه اگر (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    بشکن;
	}
	وگرنه اگر (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    وقتی (pum_selected > 0)
	    {
		--pum_selected;
		اگر (*array[pum_selected].pum_text != NUL)
		    بشکن;
	    }
	}
	وگرنه اگر (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    وقتی (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		اگر (*array[pum_selected].pum_text != NUL)
		    بشکن;
	    }
	}
	وگرنه اگر (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    بشکن;
	}
	وگرنه اگر (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	وگرنه اگر (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    اگر (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		بشکن;
	    }
	    اگر (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		بشکن;
	}
    }

    برای (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    تهی
pum_make_popup(char_u *path_name, صحیح use_mouse_pos)
{
    vimmenu_T *menu;

    اگر (!use_mouse_pos)
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
    اگر (menu != پوچ)
	pum_show_popupmenu(menu);
}
#endif

    ایستا صحیح
is_singlewidth(صحیح c)
{
    اگر (c >= 0 && c < 0x80)
	بازگشت TRUE;
    اگر (utf_char2cells(c) == 1)
	بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Set border characters for popup menu.
 */
    تهی
pum_set_border_chars(
	صحیح بالاترین,
	صحیح راست,
	صحیح bottom,
	صحیح چپ,
	صحیح بالاترین_چپ,
	صحیح بالاترین_راست,
	صحیح bottom_راست,
	صحیح bottom_چپ)
{
    اگر (!is_singlewidth(بالاترین) || !is_singlewidth(راست)
	    || !is_singlewidth(bottom) || !is_singlewidth(چپ)
	    || !is_singlewidth(بالاترین_چپ) || !is_singlewidth(بالاترین_راست)
	    || !is_singlewidth(bottom_راست) || !is_singlewidth(bottom_چپ))
	بازگشت;
    pum_border_chars.بالاترین = بالاترین;
    pum_border_chars.راست = راست;
    pum_border_chars.bottom = bottom;
    pum_border_chars.چپ = چپ;
    pum_border_chars.بالاترین_چپ = بالاترین_چپ;
    pum_border_chars.بالاترین_راست = بالاترین_راست;
    pum_border_chars.bottom_راست = bottom_راست;
    pum_border_chars.bottom_چپ = bottom_چپ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ایستا تهی
compute_margins(صحیح *راست_margin, صحیح *چپ_margin, صحیح *چپ_padding)
{
    *راست_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	*چپ_padding = (pum_col < Columns - 1 - pum_border);
	*چپ_margin = *راست_margin && (pum_col < Columns - 3);
	بازگشت;
    }
#endif
    *چپ_padding = (pum_col > pum_border);
    *چپ_margin = *راست_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    ایستا تهی
pum_draw_border(تهی)
{
    صحیح	بالاترین_row = pum_row - 1;
    صحیح	bottom_row = pum_row + pum_height;
    صحیح	col;
    صحیح	attr = highlight_attr[HLF_PMB];
    صحیح	راست_margin, چپ_margin, چپ_padding;
    صحیح	inner_width;

    اگر (!pum_border)
	بازگشت;

    (تهی)compute_margins(&راست_margin, &چپ_margin, &چپ_padding);

    inner_width = pum_width + pum_scrollbar + چپ_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
	col = pum_col + چپ_padding + 1;
    وگرنه
#endif
	col = pum_col - چپ_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	screen_putchar(pum_border_chars.بالاترین_راست, بالاترین_row, col, attr);
	اگر (چپ_margin)
	    screen_putchar(' ', بالاترین_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_راست, bottom_row, col, attr);
	اگر (چپ_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    وگرنه
#endif
    {
	screen_putchar(pum_border_chars.بالاترین_چپ, بالاترین_row, col, attr);
	اگر (چپ_margin)
	    screen_putchar(' ', بالاترین_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_چپ, bottom_row, col, attr);
	اگر (چپ_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    برای (صحیح i = 0; i < inner_width; i++)
    {
	صحیح cur_col;
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    cur_col = col - 1 - i;
	وگرنه
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.بالاترین, بالاترین_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	screen_putchar(pum_border_chars.بالاترین_چپ, بالاترین_row,
		col - inner_width - 1, attr);
	اگر (راست_margin)
	    screen_putchar(' ', بالاترین_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_چپ, bottom_row,
		col - inner_width - 1, attr);
	اگر (راست_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    وگرنه
#endif
    {
	screen_putchar(pum_border_chars.بالاترین_راست, بالاترین_row,
		col + inner_width + 1, attr);
	اگر (راست_margin)
	    screen_putchar(' ', بالاترین_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_راست, bottom_row,
		col + inner_width + 1, attr);
	اگر (راست_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    برای (صحیح i = 0; i < pum_height; i++)
    {
	صحیح row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    screen_putchar(pum_border_chars.چپ, row, col, attr);
	    اگر (چپ_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.راست, row,
		    col - inner_width - 1, attr);
	    اگر (راست_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	وگرنه
#endif
	{
	    screen_putchar(pum_border_chars.چپ, row, col, attr);
	    اگر (چپ_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.راست, row,
		    col + inner_width + 1, attr);
	    اگر (راست_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    تهی
put_shadow_char(صحیح row, صحیح col)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحیح	    shadow_attr = highlight_attr[HLF_PMS];
    صحیح	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    ایستا تهی
pum_draw_shadow(تهی)
{
    صحیح	row, col, next_col;
    صحیح	width, offset;
    صحیح	inner_width;
    صحیح	راست_margin, چپ_margin, چپ_padding;

    اگر (!pum_shadow)
	بازگشت;

    (تهی)compute_margins(&راست_margin, &چپ_margin, &چپ_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + راست_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    وگرنه
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    برای (صحیح i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + چپ_padding + pum_border + چپ_margin;
    offset = 2 - چپ_padding - pum_border - چپ_margin;

    برای (صحیح i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	اگر (pum_rl)
	    col = pum_col - offset - i;
	وگرنه
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
