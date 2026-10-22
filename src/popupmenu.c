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
#ضمن "vim.h"

ساكن pumitem_T *pum_array = عدم;	// items of displayed pum
ساكن صحيح pum_size;			// nr of items in "pum_array"
ساكن صحيح pum_selected;		// index of selected item or -1
ساكن صحيح pum_first = 0;		// index of top item

ساكن صحيح call_update_screen = FALSE;
ساكن صحيح pum_in_cmdline = FALSE;

ساكن صحيح pum_height;			// nr of displayed pum items
ساكن صحيح pum_width;			// width of displayed pum items
ساكن صحيح pum_base_width;		// width of pum items base
ساكن صحيح pum_kind_width;		// width of pum items kind column
ساكن صحيح pum_extra_width;		// width of extra stuff
ساكن صحيح pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
ساكن صحيح pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

ساكن صحيح pum_row;			// top row of pum
ساكن صحيح pum_col;			// left column of pum

ساكن win_T *pum_window = عدم;
ساكن صحيح pum_win_row;
ساكن صحيح pum_win_height;
ساكن صحيح pum_win_col;
ساكن صحيح pum_win_wcol;
ساكن صحيح pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ساكن صحيح pum_pretend_not_visible = FALSE;

// Border support
ساكن صحيح pum_border = 0;
ساكن صحيح pum_margin = 0;	// margin of 1 cell on left and right
ساكن صحيح pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
ساكن image_placement_T *pum_imagep = عدم;
#endif

// Border characters
ساكن بنية {
    صحيح اتس;
    صحيح کانن;
    صحيح bottom;
    صحيح کيري;
    صحيح اتس_کيري;
    صحيح اتس_کانن;
    صحيح bottom_کانن;
    صحيح bottom_کيري;
} pum_border_chars;

ساكن صحيح pum_set_selected(صحيح n, صحيح repeat);
ساكن فراغ pum_draw_border(فراغ);
ساكن فراغ pum_draw_shadow(فراغ);
ساكن فراغ compute_margins(صحيح *کانن_margin, صحيح *کيري_margin, صحيح *کيري_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	فراغ \
    pum_set_##feature(صحيح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ساكن فراغ
pum_compute_size(فراغ)
{
    صحيح	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    لكل (صحيح i = 0; i < pum_size; ++i)
    {
	إذا (pum_array[i].pum_text != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    إذا (pum_base_width < w)
		pum_base_width = w;
	}
	إذا (pum_array[i].pum_kind != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    إذا (pum_kind_width < w)
		pum_kind_width = w;
	}
	إذا (pum_array[i].pum_extra != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    إذا (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ساكن فراغ
pum_compute_vertical_placement(
	صحيح size,
	صحيح above_row,
	صحيح below_row)
{
    صحيح context_باريس_سموا;
    صحيح cline_visible_offset;
    صحيح extra_height = 2 * pum_border + pum_shadow;
    صحيح extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    إذا (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    إذا (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	إذا (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_باريس_سموا = 0;
	وإلا
	    // Leave two lines of context if possible
	    context_باريس_سموا = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	إذا (pum_win_row >= size + context_باريس_سموا + extra_height)
	{
	    pum_row = pum_win_row - size - context_باريس_سموا - extra_offset;
	    pum_height = size;
	}
	وإلا
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_باريس_سموا - extra_height;
	}
	إذا (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    وإلا
    {
	// pum below "pum_win_row"
	إذا (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_باريس_سموا = 0;
	وإلا
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_باريس_سموا = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_باريس_سموا + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	إذا (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#إذا defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    إذا (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    ساكن صحيح
set_pum_width_aligned_with_cursor(صحيح width, صحيح available_width)
{
    صحيح end_padding = TRUE;

    إذا (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    إذا (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    أرجع available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ساكن فراغ
pum_compute_horizontal_placement(صحيح cursor_col)
{
    صحيح desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    صحيح available_width;
    صحيح col_offset;
    صحيح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    وإلا
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    إذا (set_pum_width_aligned_with_cursor(desired_width, available_width))
	أرجع;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    إذا (available_width > p_pw)
    {
	pum_width = available_width;
	أرجع;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    إذا (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	وإلا
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	أرجع;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
	pum_col = Columns - 1 - col_offset;
    وإلا
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
    فراغ
pum_display(
	pumitem_T   *array,
	صحيح	    size,
	صحيح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحيح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحيح	    cursor_col;
    صحيح	    above_row;
    صحيح	    below_row;
    صحيح	    redo_کيراءن = 0;
#إذا defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    افعل
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = عدم;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	إذا (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	وإلا
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#إذا defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    إذا (pvwin->w_p_pvw)
		اكسر;
	إذا (pvwin != عدم)
	{
	    إذا (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    وإلا إذا (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	إذا (pum_height < 1 || (pum_height == 1 && size > 1))
	    أرجع;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	إذا (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	وإلا
	{
	    صحيح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    إذا (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    وإلا
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    إذا (curwin->w_p_cole > 0 && conceal_cursor_باريس(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		إذا (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    إذا (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    وإلا
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
    } طالما (pum_set_selected(selected, redo_کيراءن) && ++redo_کيراءن <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    فراغ
pum_call_update_screen(فراغ)
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
    صحيح
pum_under_menu(صحيح row, صحيح col, صحيح only_redrawing)
{
    صحيح	extra_کيري = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحيح	extra_کانن = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحيح	extra_above = pum_border;
    صحيح	extra_below = pum_border + (pum_shadow ? 1 : 0);
    صحيح	اتس = pum_row - extra_above;
    صحيح	bot = pum_row + pum_height + extra_below;
    صحيح	کيري = pum_col - 1 - extra_کيري;
    صحيح	کانن = pum_col + pum_width + pum_scrollbar + extra_کانن;

    إذا (!((!only_redrawing || pum_will_redraw)
	    && row >= اتس && row < bot && col >= کيري && col < کانن))
	أرجع FALSE;

    إذا (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	إذا (only_redrawing)
	{
	    إذا (col >= کانن - 2 || row == bot - 1)
		أرجع FALSE;
	}
	وإلا
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحيح	    کانن_margin, کيري_margin, کيري_padding;

	    compute_margins(&کانن_margin, &کيري_margin, &کيري_padding);
	    إذا (row == اتس && col >= کانن - 2)
		أرجع FALSE;
	    إذا (row == bot - 1 && col < pum_col + 2 - کيري_padding - pum_border
		    - کيري_margin)
		أرجع FALSE;
	}
    }
    أرجع TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ساكن صحيح *
pum_compute_text_attrs(char_u *text, hlf_T hlf, صحيح user_hlattr)
{
    صحيح		i;
    size_t	leader_len;
    صحيح		char_cells;
    صحيح		new_attr;
    char_u	*ptr = text;
    صحيح		cell_idx = 0;
    garray_T	*ga = عدم;
    صحيح		*attrs = عدم;
    char_u	*leader = عدم;
    صحيح		in_fuzzy;
    صحيح		matched_len = -1;
    int_u	char_pos = 0;
    صحيح		is_select = FALSE;

    إذا (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	أرجع عدم;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    إذا (leader == عدم || *leader == NUL)
	أرجع عدم;

    attrs = ALLOC_MULT(صحيح, vim_strsize(text));
    إذا (attrs == عدم)
	أرجع عدم;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    إذا (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	إذا (!ga)
	{
	    vim_free(attrs);
	    أرجع عدم;
	}
    }

    طالما (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	إذا (ga != عدم)
	{
	    // Handle fuzzy matching
	    لكل (i = 0; i < ga->ga_len; i++)
	    {
		إذا (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    اكسر;
		}
	    }
	}
	وإلا
	{
	    إذا (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (صحيح)leader_len;
	    إذا (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	إذا (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	لكل (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    إذا (ga != عدم)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    أرجع attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ساكن فراغ
pum_screen_puts_with_attrs(
    صحيح		row,
    صحيح		col,
    صحيح		cells UNUSED,
    char_u	*text,
    صحيح		textlen,
    صحيح		*attrs)
{
    صحيح		col_start = col;
    char_u	*ptr = text;
    صحيح		char_len;
    صحيح		attr;

    // Render text with proper attributes
    طالما (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	وإلا
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ساكن مضمن char_u *
pum_get_item(صحيح index, صحيح type)
{
    بدل(type)
    {
	حالة CPT_ABBR: أرجع pum_array[index].pum_text;
	حالة CPT_KIND: أرجع pum_array[index].pum_kind;
	حالة CPT_MENU: أرجع pum_array[index].pum_extra;
    }
    أرجع عدم;
}

    ساكن مضمن صحيح
pum_user_attr_combine(صحيح idx, صحيح type, صحيح attr)
{
    صحيح user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    أرجع user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_rtl_text(
	صحيح	row,
	صحيح     col,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     width,
	صحيح     width_limit,
	صحيح     totwidth,
	صحيح     next_isempty,
	صحيح	selected)
{
    char_u  *rt = عدم;
    صحيح     cells = 0;
    صحيح     over_cell = 0;
    صحيح     truncated = FALSE;
    صحيح     pad = next_isempty ? 0 : 2;
    صحيح     remaining = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    إذا (st == عدم)
	أرجع col;

    rt = reverse_text(st);
    إذا (rt == عدم)
    {
	VIM_CLEAR(st);
	أرجع col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    إذا (cells > width_limit)
    {
	افعل
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} طالما (cells > width_limit);

	إذا (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    إذا (truncated)
    {
	char_u  *orig_rt = rt;
	صحيح     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	إذا (cells > remaining)
	{
	    طالما (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (صحيح)STRLEN(orig_rt);
	إذا (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	إذا (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    إذا (attrs == عدم)
	screen_puts_len(rt, (صحيح)STRLEN(rt), row, col - cells + 1, attr);
    وإلا
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(صحيح)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    أرجع col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_ltr_text(
	صحيح     row,
	صحيح     col,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     width,        // width already calculated in outer loop
	صحيح     width_limit,
	صحيح     totwidth,
	صحيح     next_isempty,
	صحيح	selected)
{
    صحيح     size = 0;
    صحيح     cells = 0;
    char_u  *st_end = عدم;
    صحيح     over_cell = 0;
    صحيح     pad = next_isempty ? 0 : 2;
    صحيح     truncated = FALSE;
    صحيح     remaining = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    إذا (st == عدم)
	أرجع col;

    size = (صحيح)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    طالما (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	إذا (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	وإلا
	    --cells;
    }

    // truncated
    إذا (truncated)
    {
	remaining = width_limit - totwidth - 1;
	إذا (cells > remaining)
	{
	    st_end = st + size;
	    طالما (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	إذا (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    إذا (attrs == عدم)
	screen_puts_len(st, size, row, col, attr);
    وإلا
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    إذا (truncated)
    {
	إذا (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    أرجع col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ساكن صحيح
pum_process_item(
	صحيح     row,
	صحيح     col,
	صحيح     idx,
	صحيح     j,         // Current position in order array
	صحيح     *order,    // Order array
	hlf_T   hlf,
	صحيح     attr,
	صحيح     *totwidth_ptr,
	صحيح     next_isempty)
{
    صحيح     item_type = order[j];
    char_u  *s = عدم;
    char_u  *p = pum_get_item(idx, item_type);
    صحيح     width = 0;  // item width
    صحيح     w;		// char width
    صحيح	    selected = idx == pum_selected;

    لكل ( ; ; MB_PTR_ADV(p))
    {
	إذا (s == عدم)
	    s = p;
	w = ptr2cells(p);
	إذا (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    استمر;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	صحيح     *attrs = عدم;
	صحيح     saved = *p;

	إذا (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	إذا (saved != NUL)
	    *p = saved;

	إذا (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	وإلا
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	إذا (attrs != عدم)
	    VIM_CLEAR(attrs);

	إذا (*p != TAB)
	    اكسر;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	وإلا
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = عدم;  // start text at next char
	width = 0;
    }

    أرجع col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ساكن فراغ
pum_draw_scrollbar(
	صحيح	row,
	صحيح	i,
	صحيح	thumb_pos,
	صحيح	thumb_height)
{
    إذا (pum_scrollbar <= 0)
	أرجع;

    صحيح attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    وإلا
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    ساكن مضمن فراغ
pum_align_order(صحيح *order)
{
    صحيح is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ساكن فراغ pum_free_bg(فراغ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    فراغ
pum_opacity_changed(فراغ)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    إذا (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ساكن فراغ
pum_free_bg(فراغ)
{
    صحيح k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_باريس_سموا);
    VIM_CLEAR(pum_bg_باريس_سموا_UC);
    لكل (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_باريس_سموا_C[k]);
    pum_bg_اتس = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    فراغ
pum_redraw(فراغ)
{
    صحيح		row = pum_row;
    صحيح		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحيح		attr;
    صحيح		i, j;
    صحيح		idx;
    char_u	*p = عدم;
    صحيح		totwidth;
    صحيح		thumb_pos = 0;
    صحيح		thumb_height = 1;
    صحيح		item_type;
    صحيح		order[3];
    صحيح		next_isempty = FALSE;
    صحيح		n;
    صحيح		بارڠن_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    صحيح		basic_width;  // first item width
    صحيح		last_isabbr = FALSE;
    صحيح		orig_attr = -1;
    صحيح		scroll_range = pum_size - pum_height;
    bool	override_success;
    صحيح		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = تولق_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    إذا (pum_imagep == عدم)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(عدم, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    إذا (pum_imagep != عدم)
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

    إذا (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	إذا (opacity_active && pum_bg_باريس_سموا != عدم
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	إذا (opacity_active && pum_bg_باريس_سموا != عدم)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	وإلا إذا (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    إذا (Screen_باريس_سموا != عدم && ScreenAttrs != عدم)
	    {
		صحيح	save_اتس, save_bot, save_ncells, k;

		pum_free_bg();
		save_اتس = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		إذا (save_اتس < 0)
		    save_اتس = 0;
		إذا (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_اتس = save_اتس;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		إذا (save_اتس < save_bot)
		{
		    save_ncells = (save_bot - save_اتس) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_باريس_سموا = LALLOC_MULT(schar_T, save_ncells);
		    إذا (enc_utf8)
		    {
			pum_bg_باريس_سموا_UC = LALLOC_MULT(u8char_T, save_ncells);
			لكل (k = 0; k < MAX_MCO; ++k)
			    pum_bg_باريس_سموا_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    إذا (pum_bg_attrs != عدم && pum_bg_باريس_سموا != عدم)
		    {
			لكل (صحيح r = save_اتس; r < save_bot; ++r)
			{
			    صحيح soff = (r - save_اتس) * screen_Columns;
			    صحيح loff = باريس_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * حجم(sattr_T));
			    mch_memmove(pum_bg_باريس_سموا + soff,
				    Screen_باريس_سموا + loff,
				    screen_Columns * حجم(schar_T));
			    إذا (enc_utf8 && pum_bg_باريس_سموا_UC != عدم
						&& Screen_باريس_سموا_UC != عدم)
			    {
				mch_memmove(pum_bg_باريس_سموا_UC + soff,
					Screen_باريس_سموا_UC + loff,
					screen_Columns * حجم(u8char_T));
				لكل (k = 0; k < MAX_MCO; ++k)
				    إذا (pum_bg_باريس_سموا_C[k] != عدم
						    && Screen_باريس_سموا_C[k] != عدم)
					mch_memmove(pum_bg_باريس_سموا_C[k] + soff,
						Screen_باريس_سموا_C[k] + loff,
						screen_Columns
						    * حجم(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	وإلا
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

    إذا (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	إذا (thumb_height == 0)
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
    إذا (pum_border)
	pum_draw_border();
    إذا (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    إذا (opacity_active)
	screen_pum_blend = 100 - (صحيح)p_po;

    لكل (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    إذا (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	وإلا
#endif
	    إذا (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = بارڠن_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	لكل (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    إذا (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    إذا (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == عدم;
	    وإلا
		next_isempty = TRUE;

	    إذا (p != عدم)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    إذا (j > 0)
		n = بارڠن_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    وإلا
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    إذا (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == عدم)))
		    || basic_width + n >= pum_width)
		اكسر;
#ifdef FEAT_RIGHTLEFT
	    إذا (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    وإلا
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	وإلا
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

    إذا (override_success)
	کلوار_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    فراغ
pum_position_info_popup(win_T *wp)
{
    صحيح col = pum_col + pum_width + pum_scrollbar + 1;
    صحيح row = pum_row;
    صحيح botpos = POPPOS_BOTLEFT;
    صحيح	used_maxwidth_opt = FALSE;
    صحيح	کانن_margin, کيري_margin, dummy;
    صحيح کيري_extra_width, کانن_extra_width;

    (فراغ)compute_margins(&کانن_margin, &کيري_margin, &dummy);

    کيري_extra_width = pum_border + کيري_margin;
    کانن_extra_width = pum_border + کانن_margin + (pum_shadow ? 2 : 0);
    col += کانن_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    إذا (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - کيري_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - کيري_extra_width;
    }
    وإلا
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    إذا (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_اتس_extra(wp);
    إذا (wp->w_popup_flags & POPF_INFO_MENU)
    {
	إذا (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	وإلا
	    // menu below cursor line, align with top
	    row += 1;
    }
    وإلا
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    وإلا
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
    ساكن صحيح
pum_set_selected(صحيح n, صحيح repeat UNUSED)
{
    صحيح	    resized = FALSE;
    صحيح	    context = pum_height / 2;
    صحيح	    scroll_offset;
#ifdef FEAT_QUICKFIX
    صحيح	    prev_selected = pum_selected;
    unsigned	cur_cot_flags = get_cot_flags();
#endif
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحيح	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    إذا (pum_selected >= 0 && pum_selected < pum_size)
    {
	إذا (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    إذا (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		إذا (pum_first < 0)
		    pum_first = 0;
		وإلا إذا (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    وإلا
		pum_first = pum_selected;
	}
	وإلا إذا (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    إذا (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    وإلا
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	إذا (pum_height > 2)
	{
	    إذا (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    وإلا إذا (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#إذا defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	إذا (pum_array[pum_selected].pum_info != عدم
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# وإلا
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    إذا (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    وإلا إذا (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    وإلا
		use_popup = USEPOPUP_NONE;
	    إذا (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    إذا (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    إذا (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    إذا (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		صحيح	res = OK;

		إذا (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == عدم
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    طالما (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		وإلا
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, عدم, عدم, عدم, ECMD_ONE, 0, عدم);
		    --no_u_sync;
		    إذا (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_نيلاي_give_err((char_u *)"swf",
							  0L, عدم, OPT_LOCAL);
			set_option_نيلاي_give_err((char_u *)"bl",
							  0L, عدم, OPT_LOCAL);
			set_option_نيلاي_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_نيلاي_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_نيلاي_give_err((char_u *)"diff",
							  0L, عدم, OPT_LOCAL);
		    }
		}
		إذا (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    لكل (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			إذا (e == عدم)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    اكسر;
			}
			*e = NUL;
			ml_append(lnum++, p, (صحيح)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_باريس_کيراءن);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    إذا (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			إذا (curwin->w_height < lnum)
			{
			    win_setheight((صحيح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    إذا (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    وإلا إذا (curwin->w_topline > curbuf->b_ml.ml_باريس_کيراءن)
			curwin->w_topline = curbuf->b_ml.ml_باريس_کيراءن;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    إذا (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			إذا (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    إذا ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			صحيح save_redr_status;

			إذا (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			إذا (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			إذا (resized && win_valid(curwin_save))
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

			إذا (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    إذا (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    إذا (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    إذا (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    إذا (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    أرجع resized;
}

/*
 * Undisplay the popup menu (later).
 */
    فراغ
pum_undisplay(فراغ)
{
#ifdef FEAT_IMAGE_POPUP
    إذا (pum_imagep != عدم)
    {
	image_placement_free(pum_imagep);
	pum_imagep = عدم;
    }
#endif
    pum_free_bg();
    pum_array = عدم;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    إذا (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    فراغ
pum_clear(فراغ)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    صحيح
pum_visible(فراغ)
{
    أرجع !pum_pretend_not_visible && pum_array != عدم;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ساكن صحيح
pum_in_same_position(فراغ)
{
    صحيح	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    أرجع pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحيح
pum_redraw_in_same_position(فراغ)
{
    إذا (!pum_visible() || pum_will_redraw)
	أرجع FALSE;  // nothing to do

    أرجع pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    فراغ
pum_may_redraw(فراغ)
{
    pumitem_T	*array = pum_array;
    صحيح		len = pum_size;
    صحيح		selected = pum_selected;

    إذا (!pum_visible() || pum_will_redraw)
	أرجع;  // nothing to do

    إذا (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    وإلا
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
    صحيح
pum_get_height(فراغ)
{
    أرجع pum_height;
}

#إذا defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    فراغ
pum_set_event_info(dict_T *dict)
{
    إذا (!pum_visible())
	أرجع;
    (فراغ)dict_add_نومبور(dict, "height", pum_height);
    (فراغ)dict_add_نومبور(dict, "width", pum_width);
    (فراغ)dict_add_نومبور(dict, "row", pum_row);
    (فراغ)dict_add_نومبور(dict, "col", pum_col);
    (فراغ)dict_add_نومبور(dict, "size", pum_size);
    (فراغ)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#إذا defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    ساكن فراغ
pum_position_at_mouse(صحيح min_width)
{
    صحيح extra_height = pum_border * 2 + pum_shadow;
    صحيح extra_offset = pum_border + pum_shadow;
    صحيح trailing_overhead = 0;
    صحيح col_offset = 0;
    صحيح available_width;

    إذا (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	إذا (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	إذا (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    وإلا
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	إذا (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	إذا (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	وإلا
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    وإلا
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	إذا (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	وإلا
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = عدم;
}

#endif

#إذا defined(FEAT_BEVAL_TERM)
ساكن pumitem_T *balloon_array = عدم;
ساكن صحيح balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

تعريفنوع بنية {
    char_u	*start;
    صحيح		bytelen;
    صحيح		cells;
    صحيح		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    صحيح
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    صحيح		quoted = FALSE;
    صحيح		height;
    صحيح		باريس;
    صحيح		item_idx;
    صحيح		indent = 0;
    صحيح		max_cells = 0;
    صحيح		max_height = Rows / 2 - 1;
    صحيح		long_item_کيراءن = 0;
    صحيح		split_long_بارڠن = FALSE;

    ga_init2(&ga, حجم(balpart_T), 20);
    p = mesg;

    طالما (*p != NUL)
    {
	إذا (ga_grow(&ga, 1) == FAIL)
	    اذهب failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	طالما (*p != NUL)
	{
	    إذا (*p == '"')
		quoted = !quoted;
	    وإلا إذا (*p == '\n')
		اكسر;
	    وإلا إذا (*p == '\\' && p[1] != NUL)
		++p;
	    وإلا إذا (!quoted)
	    {
		إذا ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    إذا (*p == '{')
			++indent;
		    وإلا إذا (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    اكسر;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	إذا (*p == '\n')
	    ++p;
	إذا (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_کيراءن += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    إذا (long_item_کيراءن > 0 && height + long_item_کيراءن <= max_height)
    {
	split_long_بارڠن = TRUE;
	height += long_item_کيراءن;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    إذا (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    إذا (*array == عدم)
	اذهب failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    لكل (باريس = 1, item_idx = 0; باريس < height - 1; ++item_idx)
    {
	صحيح	skip;
	صحيح	thislen;
	صحيح	copylen;
	صحيح	ind;
	صحيح	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	إذا (item->bytelen == 0)
	    (*array)[باريس++].pum_text = vim_strsave((char_u *)"");
	وإلا
	    لكل (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		إذا (split_long_بارڠن && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    لكل (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			إذا ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    اكسر;
		    thislen = p - (item->start + skip);
		}
		وإلا
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		إذا (p == عدم)
		{
		    لكل (باريس = 0; باريس <= height - 1; ++باريس)
			vim_free((*array)[باريس].pum_text);
		    vim_free(*array);
		    اذهب failed;
		}
		لكل (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		لكل (copylen = thislen; copylen > 0; --copylen)
		    إذا (item->start[skip + copylen - 1] != ' ')
			اكسر;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[باريس].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++باريس;
	    }
    }
    ga_clear(&ga);
    أرجع height;

failed:
    ga_clear(&ga);
    أرجع 0;
}

    فراغ
ui_remove_balloon(فراغ)
{
    إذا (balloon_array == عدم)
	أرجع;

    pum_undisplay();
    طالما (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    فراغ
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    إذا (mesg == عدم && list == عدم)
    {
	pum_undisplay();
	أرجع;
    }
    إذا (list != عدم)
    {
	listitem_T  *li;
	صحيح	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	إذا (balloon_array == عدم)
	    أرجع;
	CHECK_LIST_MATERIALIZE(list);
	لكل (idx = 0, li = list->lv_first; li != عدم; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == عدم ? (char_u *)"" : text);
	}
    }
    وإلا
	balloon_arraysize = split_message(mesg, &balloon_array);

    إذا (balloon_arraysize <= 0)
	أرجع;

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
    فراغ
ui_may_remove_balloon(فراغ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#إذا defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ساكن فراغ
pum_select_mouse_pos(فراغ)
{
    صحيح idx = mouse_row - pum_row;

    إذا (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    وإلا إذا (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ساكن فراغ
pum_execute_menu(vimmenu_T *menu, صحيح mode)
{
    vimmenu_T   *mp;
    صحيح		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	إذا ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    اكسر;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    فراغ
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    صحيح		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    صحيح		save_bevalterm = p_bevalterm;
# endif
    صحيح		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	إذا (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    إذا (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	أرجع;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    إذا (array == عدم)
	أرجع;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = عدم;

	// Make a copy of the text, the menu may be redefined in a callback.
	إذا (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	وإلا إذا (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	إذا (s != عدم)
	{
	    s = vim_strsave(s);
	    إذا (s != عدم)
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

    لكل (;;)
    {
	صحيح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	إذا (c == ESC || c == Ctrl_C || pum_array == عدم)
	    اكسر;
	وإلا إذا (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    اكسر;
	}
	وإلا إذا (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    طالما (pum_selected > 0)
	    {
		--pum_selected;
		إذا (*array[pum_selected].pum_text != NUL)
		    اكسر;
	    }
	}
	وإلا إذا (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    طالما (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		إذا (*array[pum_selected].pum_text != NUL)
		    اكسر;
	    }
	}
	وإلا إذا (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    اكسر;
	}
	وإلا إذا (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	وإلا إذا (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    إذا (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		اكسر;
	    }
	    إذا (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		اكسر;
	}
    }

    لكل (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    فراغ
pum_make_popup(char_u *path_name, صحيح use_mouse_pos)
{
    vimmenu_T *menu;

    إذا (!use_mouse_pos)
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
    إذا (menu != عدم)
	pum_show_popupmenu(menu);
}
#endif

    ساكن صحيح
is_singlewidth(صحيح c)
{
    إذا (c >= 0 && c < 0x80)
	أرجع TRUE;
    إذا (utf_char2cells(c) == 1)
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Set border characters for popup menu.
 */
    فراغ
pum_set_border_chars(
	صحيح اتس,
	صحيح کانن,
	صحيح bottom,
	صحيح کيري,
	صحيح اتس_کيري,
	صحيح اتس_کانن,
	صحيح bottom_کانن,
	صحيح bottom_کيري)
{
    إذا (!is_singlewidth(اتس) || !is_singlewidth(کانن)
	    || !is_singlewidth(bottom) || !is_singlewidth(کيري)
	    || !is_singlewidth(اتس_کيري) || !is_singlewidth(اتس_کانن)
	    || !is_singlewidth(bottom_کانن) || !is_singlewidth(bottom_کيري))
	أرجع;
    pum_border_chars.اتس = اتس;
    pum_border_chars.کانن = کانن;
    pum_border_chars.bottom = bottom;
    pum_border_chars.کيري = کيري;
    pum_border_chars.اتس_کيري = اتس_کيري;
    pum_border_chars.اتس_کانن = اتس_کانن;
    pum_border_chars.bottom_کانن = bottom_کانن;
    pum_border_chars.bottom_کيري = bottom_کيري;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ساكن فراغ
compute_margins(صحيح *کانن_margin, صحيح *کيري_margin, صحيح *کيري_padding)
{
    *کانن_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	*کيري_padding = (pum_col < Columns - 1 - pum_border);
	*کيري_margin = *کانن_margin && (pum_col < Columns - 3);
	أرجع;
    }
#endif
    *کيري_padding = (pum_col > pum_border);
    *کيري_margin = *کانن_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    ساكن فراغ
pum_draw_border(فراغ)
{
    صحيح	اتس_row = pum_row - 1;
    صحيح	bottom_row = pum_row + pum_height;
    صحيح	col;
    صحيح	attr = highlight_attr[HLF_PMB];
    صحيح	کانن_margin, کيري_margin, کيري_padding;
    صحيح	inner_width;

    إذا (!pum_border)
	أرجع;

    (فراغ)compute_margins(&کانن_margin, &کيري_margin, &کيري_padding);

    inner_width = pum_width + pum_scrollbar + کيري_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
	col = pum_col + کيري_padding + 1;
    وإلا
#endif
	col = pum_col - کيري_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.اتس_کانن, اتس_row, col, attr);
	إذا (کيري_margin)
	    screen_putchar(' ', اتس_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_کانن, bottom_row, col, attr);
	إذا (کيري_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    وإلا
#endif
    {
	screen_putchar(pum_border_chars.اتس_کيري, اتس_row, col, attr);
	إذا (کيري_margin)
	    screen_putchar(' ', اتس_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_کيري, bottom_row, col, attr);
	إذا (کيري_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    لكل (صحيح i = 0; i < inner_width; i++)
    {
	صحيح cur_col;
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    cur_col = col - 1 - i;
	وإلا
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.اتس, اتس_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.اتس_کيري, اتس_row,
		col - inner_width - 1, attr);
	إذا (کانن_margin)
	    screen_putchar(' ', اتس_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_کيري, bottom_row,
		col - inner_width - 1, attr);
	إذا (کانن_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    وإلا
#endif
    {
	screen_putchar(pum_border_chars.اتس_کانن, اتس_row,
		col + inner_width + 1, attr);
	إذا (کانن_margin)
	    screen_putchar(' ', اتس_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_کانن, bottom_row,
		col + inner_width + 1, attr);
	إذا (کانن_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    لكل (صحيح i = 0; i < pum_height; i++)
    {
	صحيح row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_putchar(pum_border_chars.کيري, row, col, attr);
	    إذا (کيري_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.کانن, row,
		    col - inner_width - 1, attr);
	    إذا (کانن_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	وإلا
#endif
	{
	    screen_putchar(pum_border_chars.کيري, row, col, attr);
	    إذا (کيري_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.کانن, row,
		    col + inner_width + 1, attr);
	    إذا (کانن_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    فراغ
put_shadow_char(صحيح row, صحيح col)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحيح	    shadow_attr = highlight_attr[HLF_PMS];
    صحيح	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    ساكن فراغ
pum_draw_shadow(فراغ)
{
    صحيح	row, col, next_col;
    صحيح	width, offset;
    صحيح	inner_width;
    صحيح	کانن_margin, کيري_margin, کيري_padding;

    إذا (!pum_shadow)
	أرجع;

    (فراغ)compute_margins(&کانن_margin, &کيري_margin, &کيري_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + کانن_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    وإلا
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    لكل (صحيح i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + کيري_padding + pum_border + کيري_margin;
    offset = 2 - کيري_padding - pum_border - کيري_margin;

    لكل (صحيح i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	إذا (pum_rl)
	    col = pum_col - offset - i;
	وإلا
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
