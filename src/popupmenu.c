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
#अन्तर्भाव "vim.h"

स्थिर pumitem_T *pum_array = रिक्त;	// items of displayed pum
स्थिर पूर्णांक pum_size;			// nr of items in "pum_array"
स्थिर पूर्णांक pum_selected;		// index of selected item or -1
स्थिर पूर्णांक pum_first = 0;		// index of top item

स्थिर पूर्णांक call_update_screen = FALSE;
स्थिर पूर्णांक pum_in_cmdline = FALSE;

स्थिर पूर्णांक pum_height;			// nr of displayed pum items
स्थिर पूर्णांक pum_width;			// width of displayed pum items
स्थिर पूर्णांक pum_base_width;		// width of pum items base
स्थिर पूर्णांक pum_kind_width;		// width of pum items kind column
स्थिर पूर्णांक pum_extra_width;		// width of extra stuff
स्थिर पूर्णांक pum_scrollbar;		// TRUE when scrollbar present
#ifdef FEAT_RIGHTLEFT
स्थिर पूर्णांक pum_rl;			// TRUE when pum is drawn 'rightleft'
#endif

स्थिर पूर्णांक pum_row;			// top row of pum
स्थिर पूर्णांक pum_col;			// left column of pum

स्थिर win_T *pum_window = रिक्त;
स्थिर पूर्णांक pum_win_row;
स्थिर पूर्णांक pum_win_height;
स्थिर पूर्णांक pum_win_col;
स्थिर पूर्णांक pum_win_wcol;
स्थिर पूर्णांक pum_win_width;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
स्थिर पूर्णांक pum_pretend_not_visible = FALSE;

// Border support
स्थिर पूर्णांक pum_border = 0;
स्थिर पूर्णांक pum_margin = 0;	// margin of 1 cell on left and right
स्थिर पूर्णांक pum_shadow = 0;

#ifdef FEAT_IMAGE_POPUP
स्थिर image_placement_T *pum_imagep = रिक्त;
#endif

// Border characters
स्थिर संरचना {
    पूर्णांक शीर्ष;
    पूर्णांक उजवा;
    पूर्णांक bottom;
    पूर्णांक डावा;
    पूर्णांक शीर्ष_डावा;
    पूर्णांक शीर्ष_उजवा;
    पूर्णांक bottom_उजवा;
    पूर्णांक bottom_डावा;
} pum_border_chars;

स्थिर पूर्णांक pum_set_selected(पूर्णांक n, पूर्णांक repeat);
स्थिर शून्य pum_draw_border(शून्य);
स्थिर शून्य pum_draw_shadow(शून्य);
स्थिर शून्य compute_margins(पूर्णांक *उजवा_margin, पूर्णांक *डावा_margin, पूर्णांक *डावा_padding);

#define PUM_DEF_HEIGHT 10

#define DEFINE_PUM_SETTER(feature) \
	शून्य \
    pum_set_##feature(पूर्णांक enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    स्थिर शून्य
pum_compute_size(शून्य)
{
    पूर्णांक	w;

    // Compute the width of the widest match and the widest extra.
    pum_base_width = 0;
    pum_kind_width = 0;
    pum_extra_width = 0;
    साठी (पूर्णांक i = 0; i < pum_size; ++i)
    {
	जर (pum_array[i].pum_text != रिक्त)
	{
	    w = vim_strsize(pum_array[i].pum_text);
	    जर (pum_base_width < w)
		pum_base_width = w;
	}
	जर (pum_array[i].pum_kind != रिक्त)
	{
	    w = vim_strsize(pum_array[i].pum_kind) + 1;
	    जर (pum_kind_width < w)
		pum_kind_width = w;
	}
	जर (pum_array[i].pum_extra != रिक्त)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    जर (pum_extra_width < w)
		pum_extra_width = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    स्थिर शून्य
pum_compute_vertical_placement(
	पूर्णांक size,
	पूर्णांक above_row,
	पूर्णांक below_row)
{
    पूर्णांक context_ओळी;
    पूर्णांक cline_visible_offset;
    पूर्णांक extra_height = 2 * pum_border + pum_shadow;
    पूर्णांक extra_offset = pum_border + pum_shadow;

    pum_height = MIN(size, PUM_DEF_HEIGHT);
    जर (p_ph > 0 && pum_height > p_ph)
	pum_height = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    जर (pum_win_row + 2 >= below_row - pum_height - extra_height
	    && pum_win_row - above_row > (below_row - above_row) / 2)
    {
	// pum above "pum_win_row"
	जर (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ओळी = 0;
	अन्यथा
	    // Leave two lines of context if possible
	    context_ओळी = MIN(2, curwin->w_wrow - curwin->w_cline_row);

	जर (pum_win_row >= size + context_ओळी + extra_height)
	{
	    pum_row = pum_win_row - size - context_ओळी - extra_offset;
	    pum_height = size;
	}
	अन्यथा
	{
	    pum_row = pum_border;
	    pum_height = pum_win_row - context_ओळी - extra_height;
	}
	जर (p_ph > 0 && pum_height > p_ph)
	{
	    pum_row += pum_height - p_ph;
	    pum_height = p_ph;
	}
    }
    अन्यथा
    {
	// pum below "pum_win_row"
	जर (State & MODE_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ओळी = 0;
	अन्यथा
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_offset = curwin->w_cline_row +
		curwin->w_cline_height - curwin->w_wrow;
	    context_ओळी = MIN(3, cline_visible_offset);
	}

	pum_row = pum_win_row + context_ओळी + pum_border;
	pum_height = MIN(below_row - pum_row - extra_offset, size);
	जर (p_ph > 0 && pum_height > p_ph)
	    pum_height = p_ph;
    }

#जर defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    जर (above_row > 0 && pum_row < above_row && pum_height > above_row)
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
    स्थिर पूर्णांक
set_pum_width_aligned_with_cursor(पूर्णांक width, पूर्णांक available_width)
{
    पूर्णांक end_padding = TRUE;

    जर (width < p_pw)
    {
	width = p_pw;
	end_padding = FALSE;
    }
    जर (p_pmw > 0 && width >= p_pmw)
    {
	width = p_pmw;
	end_padding = FALSE;
    }

    pum_width = width + (end_padding && width >= p_pw ? 1 : 0);
    परत available_width >= pum_width;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    स्थिर शून्य
pum_compute_horizontal_placement(पूर्णांक cursor_col)
{
    पूर्णांक desired_width = pum_base_width + pum_kind_width + pum_extra_width;
    पूर्णांक available_width;
    पूर्णांक col_offset;
    पूर्णांक trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	col_offset = pum_border && (cursor_col >= Columns - 2)
	    ? (3 - Columns + cursor_col) : 0;
	available_width = cursor_col + 1;
	pum_col = cursor_col - col_offset;
    }
    अन्यथा
#endif
    {
	col_offset = pum_border && (cursor_col <= 1) ? (2 - cursor_col) : 0;
	available_width = Columns - cursor_col;
	pum_col = cursor_col + col_offset;
    }

    available_width -= trailing_overhead + col_offset;

    // Case 1: Align pum with "cursor_col"
    जर (set_pum_width_aligned_with_cursor(desired_width, available_width))
	परत;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    जर (available_width > p_pw)
    {
	pum_width = available_width;
	परत;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    available_width = Columns - trailing_overhead - col_offset;

    जर (available_width > p_pw)
    {
	pum_width = p_pw;  // Truncate beyond 'pum_width'
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    pum_col = pum_width + trailing_overhead;
	अन्यथा
#endif
	    pum_col = Columns - pum_width - trailing_overhead;
	परत;
    }

    // Case 3: Not enough room anywhere, use what we have
#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
	pum_col = Columns - 1 - col_offset;
    अन्यथा
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
    शून्य
pum_display(
	pumitem_T   *array,
	पूर्णांक	    size,
	पूर्णांक	    selected,   // index of initially selected item, -1 if
				// out of range
	पूर्णांक	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    पूर्णांक	    cursor_col;
    पूर्णांक	    above_row;
    पूर्णांक	    below_row;
    पूर्णांक	    redo_मोजणी = 0;
#जर defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#endif

#ifdef FEAT_RIGHTLEFT
    pum_rl = (State & MODE_CMDLINE) == 0 && curwin->w_p_rl;
#endif

    करा
    {
	above_row = 0;
	below_row = cmdline_row;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_col();
	pum_array = रिक्त;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_window = curwin;
	जर (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_row = cmdline_row;
	अन्यथा
	    pum_win_row = curwin->w_wrow + W_WINROW(curwin);
	pum_win_height = curwin->w_height;
	pum_win_col = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_width = curwin->w_width;

#जर defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    जर (pvwin->w_p_pvw)
		थांबा;
	जर (pvwin != रिक्त)
	{
	    जर (W_WINROW(pvwin) < W_WINROW(curwin))
		above_row = W_WINROW(pvwin) + pvwin->w_height;
	    अन्यथा जर (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_height)
		below_row = W_WINROW(pvwin);
	}
#endif

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(size, above_row, below_row);

	// don't display when we only have room for one line
	जर (pum_height < 1 || (pum_height == 1 && size > 1))
	    परत;

	pum_array = array;
	pum_size = size;
	pum_compute_size();

	// Calculate cursor column
	जर (State & MODE_CMDLINE)
	    // cmdline completion popup menu
	    cursor_col = cmdline_compl_startcol();
	अन्यथा
	{
	    पूर्णांक wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    जर (curwin->w_width > 0)
		wcol %= curwin->w_width;
	    अन्यथा
		wcol = 0;
#ifdef FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    जर (curwin->w_p_cole > 0 && conceal_cursor_ओळ(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		जर (wcol < 0)
		    wcol = 0;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    जर (pum_rl)
		cursor_col = curwin->w_wincol + curwin->w_width - wcol - 1;
	    अन्यथा
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
    } पर्यंत (pum_set_selected(selected, redo_मोजणी) && ++redo_मोजणी <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    शून्य
pum_call_update_screen(शून्य)
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
    पूर्णांक
pum_under_menu(पूर्णांक row, पूर्णांक col, पूर्णांक only_redrawing)
{
    पूर्णांक	extra_डावा = pum_border + (pum_margin && pum_border ? 1 : 0);
    पूर्णांक	extra_उजवा = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    पूर्णांक	extra_above = pum_border;
    पूर्णांक	extra_below = pum_border + (pum_shadow ? 1 : 0);
    पूर्णांक	शीर्ष = pum_row - extra_above;
    पूर्णांक	bot = pum_row + pum_height + extra_below;
    पूर्णांक	डावा = pum_col - 1 - extra_डावा;
    पूर्णांक	उजवा = pum_col + pum_width + pum_scrollbar + extra_उजवा;

    जर (!((!only_redrawing || pum_will_redraw)
	    && row >= शीर्ष && row < bot && col >= डावा && col < उजवा))
	परत FALSE;

    जर (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	जर (only_redrawing)
	{
	    जर (col >= उजवा - 2 || row == bot - 1)
		परत FALSE;
	}
	अन्यथा
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    पूर्णांक	    उजवा_margin, डावा_margin, डावा_padding;

	    compute_margins(&उजवा_margin, &डावा_margin, &डावा_padding);
	    जर (row == शीर्ष && col >= उजवा - 2)
		परत FALSE;
	    जर (row == bot - 1 && col < pum_col + 2 - डावा_padding - pum_border
		    - डावा_margin)
		परत FALSE;
	}
    }
    परत TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    स्थिर पूर्णांक *
pum_compute_text_attrs(char_u *text, hlf_T hlf, पूर्णांक user_hlattr)
{
    पूर्णांक		i;
    माप_प्रकार	leader_len;
    पूर्णांक		char_cells;
    पूर्णांक		new_attr;
    char_u	*ptr = text;
    पूर्णांक		cell_idx = 0;
    garray_T	*ga = रिक्त;
    पूर्णांक		*attrs = रिक्त;
    char_u	*leader = रिक्त;
    पूर्णांक		in_fuzzy;
    पूर्णांक		matched_len = -1;
    int_u	char_pos = 0;
    पूर्णांक		is_select = FALSE;

    जर (*text == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	परत रिक्त;

    is_select = hlf == HLF_PSI;
    leader = (State & MODE_CMDLINE) ? cmdline_compl_pattern()
							  : ins_compl_leader();
    जर (leader == रिक्त || *leader == NUL)
	परत रिक्त;

    attrs = ALLOC_MULT(पूर्णांक, vim_strsize(text));
    जर (attrs == रिक्त)
	परत रिक्त;

    in_fuzzy = (State & MODE_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    जर (in_fuzzy)
    {
	ga = fuzzy_match_str_with_pos(text, leader);
	जर (!ga)
	{
	    vim_free(attrs);
	    परत रिक्त;
	}
    }

    पर्यंत (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	जर (ga != रिक्त)
	{
	    // Handle fuzzy matching
	    साठी (i = 0; i < ga->ga_len; i++)
	    {
		जर (char_pos == ((int_u *)ga->ga_data)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    थांबा;
		}
	    }
	}
	अन्यथा
	{
	    जर (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (पूर्णांक)leader_len;
	    जर (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	जर (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	साठी (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    जर (ga != रिक्त)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    परत attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    स्थिर शून्य
pum_screen_puts_with_attrs(
    पूर्णांक		row,
    पूर्णांक		col,
    पूर्णांक		cells UNUSED,
    char_u	*text,
    पूर्णांक		textlen,
    पूर्णांक		*attrs)
{
    पूर्णांक		col_start = col;
    char_u	*ptr = text;
    पूर्णांक		char_len;
    पूर्णांक		attr;

    // Render text with proper attributes
    पर्यंत (*ptr != NUL && ptr < text + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    attr = attrs[col_start + cells - col - 1];
	अन्यथा
#endif
	    attr = attrs[col - col_start];
	screen_puts_len(ptr, char_len, row, col, attr);
	col += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    स्थिर ओळीत char_u *
pum_get_item(पूर्णांक index, पूर्णांक type)
{
    निवड(type)
    {
	स्थिती CPT_ABBR: परत pum_array[index].pum_text;
	स्थिती CPT_KIND: परत pum_array[index].pum_kind;
	स्थिती CPT_MENU: परत pum_array[index].pum_extra;
    }
    परत रिक्त;
}

    स्थिर ओळीत पूर्णांक
pum_user_attr_combine(पूर्णांक idx, पूर्णांक type, पूर्णांक attr)
{
    पूर्णांक user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_kind_hlattr,
    };

    परत user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ifdef FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    स्थिर पूर्णांक
pum_display_rtl_text(
	पूर्णांक	row,
	पूर्णांक     col,
	char_u  *st,
	पूर्णांक     attr,
	पूर्णांक     *attrs,
	पूर्णांक     width,
	पूर्णांक     width_limit,
	पूर्णांक     totwidth,
	पूर्णांक     next_isempty,
	पूर्णांक	selected)
{
    char_u  *rt = रिक्त;
    पूर्णांक     cells = 0;
    पूर्णांक     over_cell = 0;
    पूर्णांक     truncated = FALSE;
    पूर्णांक     pad = next_isempty ? 0 : 2;
    पूर्णांक     remaining = 0;
    पूर्णांक	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    पूर्णांक	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    जर (st == रिक्त)
	परत col;

    rt = reverse_text(st);
    जर (rt == रिक्त)
    {
	VIM_CLEAR(st);
	परत col;
    }

    char_u *rt_start = rt;
    cells = mb_string2cells(rt, -1);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    जर (cells > width_limit)
    {
	करा
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} पर्यंत (cells > width_limit);

	जर (cells < width_limit)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    जर (truncated)
    {
	char_u  *orig_rt = rt;
	पूर्णांक     size = 0;

	remaining = width_limit - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	जर (cells > remaining)
	{
	    पर्यंत (cells > remaining)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	size = (पूर्णांक)STRLEN(orig_rt);
	जर (cells < remaining)
	    over_cell = remaining - cells;

	cells = mb_string2cells(orig_rt, size);
	width = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, row, col - width + 1, trunc_attr);

	जर (over_cell > 0)
	    screen_fill(row, row + 1, col - width + 2,
		    col - width + 2 + over_cell, ' ', ' ', attr);
    }

    जर (attrs == रिक्त)
	screen_puts_len(rt, (पूर्णांक)STRLEN(rt), row, col - cells + 1, attr);
    अन्यथा
	pum_screen_puts_with_attrs(row, col - cells + 1, cells, rt,
		(पूर्णांक)STRLEN(rt), attrs);

    vim_free(rt_start);
    VIM_CLEAR(st);
    परत col - width;
}
#endif

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    स्थिर पूर्णांक
pum_display_ltr_text(
	पूर्णांक     row,
	पूर्णांक     col,
	char_u  *st,
	पूर्णांक     attr,
	पूर्णांक     *attrs,
	पूर्णांक     width,        // width already calculated in outer loop
	पूर्णांक     width_limit,
	पूर्णांक     totwidth,
	पूर्णांक     next_isempty,
	पूर्णांक	selected)
{
    पूर्णांक     size = 0;
    पूर्णांक     cells = 0;
    char_u  *st_end = रिक्त;
    पूर्णांक     over_cell = 0;
    पूर्णांक     pad = next_isempty ? 0 : 2;
    पूर्णांक     truncated = FALSE;
    पूर्णांक     remaining = 0;
    पूर्णांक	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    पूर्णांक	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    जर (st == रिक्त)
	परत col;

    size = (पूर्णांक)STRLEN(st);
    cells = (*mb_string2cells)(st, size);
    truncated = width_limit - totwidth < cells + pad;

    // only draw the text that fits
    पर्यंत (size > 0 && col + cells > width_limit + pum_col)
    {
	--size;
	जर (has_mbyte)
	{
	    size -= (*mb_head_off)(st, st + size);
	    cells -= (*mb_ptr2cells)(st + size);
	}
	अन्यथा
	    --cells;
    }

    // truncated
    जर (truncated)
    {
	remaining = width_limit - totwidth - 1;
	जर (cells > remaining)
	{
	    st_end = st + size;
	    पर्यंत (st_end > st && cells > remaining)
	    {
		MB_PTR_BACK(st, st_end);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_end) : 1;
	    }
	    size = st_end - st;
	}

	जर (cells < remaining)
	    over_cell = remaining - cells;
	cells = mb_string2cells(st, size);
	width = cells + over_cell + 1;
    }

    जर (attrs == रिक्त)
	screen_puts_len(st, size, row, col, attr);
    अन्यथा
	pum_screen_puts_with_attrs(row, col, cells, st, size, attrs);

    जर (truncated)
    {
	जर (over_cell > 0)
	    screen_fill(row, row + 1, col + cells,
		    col + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, row, col + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    परत col + width;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    स्थिर पूर्णांक
pum_process_item(
	पूर्णांक     row,
	पूर्णांक     col,
	पूर्णांक     idx,
	पूर्णांक     j,         // Current position in order array
	पूर्णांक     *order,    // Order array
	hlf_T   hlf,
	पूर्णांक     attr,
	पूर्णांक     *totwidth_ptr,
	पूर्णांक     next_isempty)
{
    पूर्णांक     item_type = order[j];
    char_u  *s = रिक्त;
    char_u  *p = pum_get_item(idx, item_type);
    पूर्णांक     width = 0;  // item width
    पूर्णांक     w;		// char width
    पूर्णांक	    selected = idx == pum_selected;

    साठी ( ; ; MB_PTR_ADV(p))
    {
	जर (s == रिक्त)
	    s = p;
	w = ptr2cells(p);
	जर (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_width)
	{
	    width += w;
	    सुरूठेवा;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	पूर्णांक     *attrs = रिक्त;
	पूर्णांक     saved = *p;

	जर (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	जर (saved != NUL)
	    *p = saved;

	जर (item_type == CPT_ABBR)
	    attrs = pum_compute_text_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    col = pum_display_rtl_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);
	अन्यथा
#endif
	    col = pum_display_ltr_text(row, col, st, attr, attrs,
		    width, pum_width, *totwidth_ptr, next_isempty, selected);

	जर (attrs != रिक्त)
	    VIM_CLEAR(attrs);

	जर (*p != TAB)
	    थांबा;

	// Display two spaces for a Tab.
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, row, col - 1, attr);
	    col -= 2;
	}
	अन्यथा
#endif
	{
	    screen_puts_len((char_u *)"  ", 2, row, col, attr);
	    col += 2;
	}
	*totwidth_ptr += 2;
	s = रिक्त;  // start text at next char
	width = 0;
    }

    परत col;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    स्थिर शून्य
pum_draw_scrollbar(
	पूर्णांक	row,
	पूर्णांक	i,
	पूर्णांक	thumb_pos,
	पूर्णांक	thumb_height)
{
    जर (pum_scrollbar <= 0)
	परत;

    पूर्णांक attr = (i >= thumb_pos && i < thumb_pos + thumb_height) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
	screen_putchar(' ', row, pum_col - pum_width, attr);
    अन्यथा
#endif
	screen_putchar(' ', row, pum_col + pum_width, attr);
}

    स्थिर ओळीत शून्य
pum_align_order(पूर्णांक *order)
{
    पूर्णांक is_default = cia_flags == 0;
    order[0] = is_default ? CPT_ABBR : cia_flags / 100;
    order[1] = is_default ? CPT_KIND : (cia_flags / 10) % 10;
    order[2] = is_default ? CPT_MENU : cia_flags % 10;
}

स्थिर शून्य pum_free_bg(शून्य);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    शून्य
pum_opacity_changed(शून्य)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    जर (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    स्थिर शून्य
pum_free_bg(शून्य)
{
    पूर्णांक k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_ओळी);
    VIM_CLEAR(pum_bg_ओळी_UC);
    साठी (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_ओळी_C[k]);
    pum_bg_शीर्ष = 0;
    pum_bg_bot = 0;
    pum_bg_cols = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    शून्य
pum_redraw(शून्य)
{
    पूर्णांक		row = pum_row;
    पूर्णांक		col;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    पूर्णांक		attr;
    पूर्णांक		i, j;
    पूर्णांक		idx;
    char_u	*p = रिक्त;
    पूर्णांक		totwidth;
    पूर्णांक		thumb_pos = 0;
    पूर्णांक		thumb_height = 1;
    पूर्णांक		item_type;
    पूर्णांक		order[3];
    पूर्णांक		next_isempty = FALSE;
    पूर्णांक		n;
    पूर्णांक		वस्तुसंच_width_array[3] = { pum_base_width, pum_kind_width,
							    pum_extra_width };
    पूर्णांक		basic_width;  // first item width
    पूर्णांक		last_isabbr = FALSE;
    पूर्णांक		orig_attr = -1;
    पूर्णांक		scroll_range = pum_size - pum_height;
    bool	override_success;
    पूर्णांक		opacity_active = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ढकला_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    जर (pum_imagep == रिक्त)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(रिक्त, अच_पूर्णांक_उच्च,
		IMAGE_PLACEMENT_USE_PUM, true);
    जर (pum_imagep != रिक्त)
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

    जर (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	जर (opacity_active && pum_bg_ओळी != रिक्त
		&& (pum_bg_cols != screen_Columns
		    || pum_bg_bot > screen_Rows))
	    pum_free_bg();

	जर (opacity_active && pum_bg_ओळी != रिक्त)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	अन्यथा जर (opacity_active)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    जर (Screen_ओळी != रिक्त && ScreenAttrs != रिक्त)
	    {
		पूर्णांक	save_शीर्ष, save_bot, save_ncells, k;

		pum_free_bg();
		save_शीर्ष = pum_row - pum_border;
		save_bot = pum_row + pum_height + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		जर (save_शीर्ष < 0)
		    save_शीर्ष = 0;
		जर (save_bot > screen_Rows)
		    save_bot = screen_Rows;
		pum_bg_शीर्ष = save_शीर्ष;
		pum_bg_bot = save_bot;
		pum_bg_cols = screen_Columns;
		जर (save_शीर्ष < save_bot)
		{
		    save_ncells = (save_bot - save_शीर्ष) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_ओळी = LALLOC_MULT(schar_T, save_ncells);
		    जर (enc_utf8)
		    {
			pum_bg_ओळी_UC = LALLOC_MULT(u8char_T, save_ncells);
			साठी (k = 0; k < MAX_MCO; ++k)
			    pum_bg_ओळी_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    जर (pum_bg_attrs != रिक्त && pum_bg_ओळी != रिक्त)
		    {
			साठी (पूर्णांक r = save_शीर्ष; r < save_bot; ++r)
			{
			    पूर्णांक soff = (r - save_शीर्ष) * screen_Columns;
			    पूर्णांक loff = ओळ_Offset[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * आकार(sattr_T));
			    mch_memmove(pum_bg_ओळी + soff,
				    Screen_ओळी + loff,
				    screen_Columns * आकार(schar_T));
			    जर (enc_utf8 && pum_bg_ओळी_UC != रिक्त
						&& Screen_ओळी_UC != रिक्त)
			    {
				mch_memmove(pum_bg_ओळी_UC + soff,
					Screen_ओळी_UC + loff,
					screen_Columns * आकार(u8char_T));
				साठी (k = 0; k < MAX_MCO; ++k)
				    जर (pum_bg_ओळी_C[k] != रिक्त
						    && Screen_ओळी_C[k] != रिक्त)
					mch_memmove(pum_bg_ओळी_C[k] + soff,
						Screen_ओळी_C[k] + loff,
						screen_Columns
						    * आकार(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	अन्यथा
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

    जर (pum_scrollbar)
    {
	thumb_height = pum_height * pum_height / pum_size;
	जर (thumb_height == 0)
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
    जर (pum_border)
	pum_draw_border();
    जर (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    जर (opacity_active)
	screen_pum_blend = 100 - (पूर्णांक)p_po;

    साठी (i = 0; i < pum_height; ++i)
    {
	idx = i + pum_first;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	{
	    जर (pum_col < curwin->w_wincol + curwin->w_width - 1
							    - pum_border)
		screen_putchar(' ', row, pum_col + 1, attr);
	}
	अन्यथा
#endif
	    जर (pum_col > pum_border)
		screen_putchar(' ', row, pum_col - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	col = pum_col;
	totwidth = 0;
	pum_align_order(order);
	basic_width = वस्तुसंच_width_array[order[0]];
	last_isabbr = order[2] == CPT_ABBR;
	साठी (j = 0; j < 3; ++j)
	{
	    item_type = order[j];
	    hlf = hlfs[item_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    जर (item_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, item_type, attr);
	    p = pum_get_item(idx, item_type);

	    जर (j + 1 < 3)
		next_isempty = pum_get_item(idx, order[j + 1]) == रिक्त;
	    अन्यथा
		next_isempty = TRUE;

	    जर (p != रिक्त)
		// Process and display the item
		col = pum_process_item(row, col, idx, j, order, hlf, attr,
						    &totwidth, next_isempty);

	    जर (j > 0)
		n = वस्तुसंच_width_array[order[1]] + (last_isabbr ? 0 : 1);
	    अन्यथा
		n = order[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    जर (j == 2
		    || (next_isempty && (j == 1 || (j == 0
				&& pum_get_item(idx, order[j + 2]) == रिक्त)))
		    || basic_width + n >= pum_width)
		थांबा;
#ifdef FEAT_RIGHTLEFT
	    जर (pum_rl)
	    {
		screen_fill(row, row + 1, pum_col - basic_width - n + 1,
						col + 1, ' ', ' ', orig_attr);
		col = pum_col - basic_width - n;
	    }
	    अन्यथा
#endif
	    {
		screen_fill(row, row + 1, col, pum_col + basic_width + n,
							' ', ' ', orig_attr);
		col = pum_col + basic_width + n;
	    }
	    totwidth = basic_width + n;
	}

#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    screen_fill(row, row + 1, pum_col - pum_width + 1, col + 1,
							    ' ', ' ', orig_attr);
	अन्यथा
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

    जर (override_success)
	काढा_highlight_overrides();

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
}

#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    शून्य
pum_position_info_popup(win_T *wp)
{
    पूर्णांक col = pum_col + pum_width + pum_scrollbar + 1;
    पूर्णांक row = pum_row;
    पूर्णांक botpos = POPPOS_BOTLEFT;
    पूर्णांक	used_maxwidth_opt = FALSE;
    पूर्णांक	उजवा_margin, डावा_margin, dummy;
    पूर्णांक डावा_extra_width, उजवा_extra_width;

    (शून्य)compute_margins(&उजवा_margin, &डावा_margin, &dummy);

    डावा_extra_width = pum_border + डावा_margin;
    उजवा_extra_width = pum_border + उजवा_margin + (pum_shadow ? 2 : 0);
    col += उजवा_extra_width;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    जर (Columns - col < 20 && Columns - col < pum_col)
    {
	col = pum_col - 1 - डावा_extra_width;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_col - 1 - डावा_extra_width;
    }
    अन्यथा
	wp->w_maxwidth = Columns - col + 1;
    wp->w_maxwidth -= popup_extra_width(wp);
    जर (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	used_maxwidth_opt = TRUE;
    }

    row -= popup_शीर्ष_extra(wp);
    जर (wp->w_popup_flags & POPF_INFO_MENU)
    {
	जर (pum_row < pum_win_row)
	{
	    // menu above cursor line, align with bottom
	    row += pum_height;
	    wp->w_popup_pos = botpos;
	}
	अन्यथा
	    // menu below cursor line, align with top
	    row += 1;
    }
    अन्यथा
	// align with the selected item
	row += pum_selected - pum_first + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    जर (wp->w_maxwidth < 10 && !used_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    अन्यथा
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
    स्थिर पूर्णांक
pum_set_selected(पूर्णांक n, पूर्णांक repeat UNUSED)
{
    पूर्णांक	    resized = FALSE;
    पूर्णांक	    context = pum_height / 2;
    पूर्णांक	    scroll_offset;
#ifdef FEAT_QUICKFIX
    पूर्णांक	    prev_selected = pum_selected;
    अचिन्हित	cur_cot_flags = get_cot_flags();
#endif
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    पूर्णांक	    has_info = FALSE;
#endif

    pum_selected = n;
    scroll_offset = pum_selected - pum_height;

    जर (pum_selected >= 0 && pum_selected < pum_size)
    {
	जर (pum_first > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    जर (pum_first > pum_selected - 2)
	    {
		pum_first -= pum_height - 2;
		जर (pum_first < 0)
		    pum_first = 0;
		अन्यथा जर (pum_first > pum_selected)
		    pum_first = pum_selected;
	    }
	    अन्यथा
		pum_first = pum_selected;
	}
	अन्यथा जर (pum_first < scroll_offset + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    जर (pum_first < scroll_offset + 3)
		pum_first = MAX(pum_first + pum_height - 2, scroll_offset + 1);
	    अन्यथा
		pum_first = scroll_offset + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	जर (pum_height > 2)
	{
	    जर (pum_first > pum_selected - context)
		pum_first = MAX(pum_selected - context, 0);  // scroll down
	    अन्यथा जर (pum_first < pum_selected + context - pum_height + 1)
		pum_first = pum_selected + context - pum_height + 1;  // up
	}
	// adjust for the number of lines displayed
	pum_first = MIN(pum_first, pum_size - pum_height);

#जर defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	जर (pum_array[pum_selected].pum_info != रिक्त
		&& Rows > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# अन्यथा
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    जर (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    अन्यथा जर (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_NORMAL;
	    अन्यथा
		use_popup = USEPOPUP_NONE;
	    जर (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		block_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    जर (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    जर (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    जर (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		पूर्णांक	res = OK;

		जर (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == रिक्त
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    पर्यंत (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		अन्यथा
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE, 0, रिक्त);
		    --no_u_sync;
		    जर (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			set_option_मूल्य_give_err((char_u *)"swf",
							  0L, रिक्त, OPT_LOCAL);
			set_option_मूल्य_give_err((char_u *)"bl",
							  0L, रिक्त, OPT_LOCAL);
			set_option_मूल्य_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			set_option_मूल्य_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			set_option_मूल्य_give_err((char_u *)"diff",
							  0L, रिक्त, OPT_LOCAL);
		    }
		}
		जर (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    साठी (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			जर (e == रिक्त)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    थांबा;
			}
			*e = NUL;
			ml_append(lnum++, p, (पूर्णांक)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    जर (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			जर (curwin->w_height < lnum)
			{
			    win_setheight((पूर्णांक)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_changed = 0;
		    curbuf->b_p_ma = FALSE;
		    जर (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    अन्यथा जर (curwin->w_topline > curbuf->b_ml.ml_ओळ_मोजणी)
			curwin->w_topline = curbuf->b_ml.ml_ओळ_मोजणी;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.col = 0;
# ifdef FEAT_PROP_POPUP
		    जर (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			जर (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    जर ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			पूर्णांक save_redr_status;

			जर (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			जर (ins_compl_active() && !resized)
			    curwin->w_redr_status = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			जर (resized && win_valid(curwin_save))
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

			जर (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    जर (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    जर (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    जर (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#endif
    }
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    जर (!has_info)
	// hide any popup info window
	popup_hide_info();
#endif

    परत resized;
}

/*
 * Undisplay the popup menu (later).
 */
    शून्य
pum_undisplay(शून्य)
{
#ifdef FEAT_IMAGE_POPUP
    जर (pum_imagep != रिक्त)
    {
	image_placement_free(pum_imagep);
	pum_imagep = रिक्त;
    }
#endif
    pum_free_bg();
    pum_array = रिक्त;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    जर (pum_in_cmdline)
    {
	clear_cmdline = TRUE;
	pum_in_cmdline = FALSE;
    }
    status_redraw_all();
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#endif
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    शून्य
pum_clear(शून्य)
{
    pum_first = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    पूर्णांक
pum_visible(शून्य)
{
    परत !pum_pretend_not_visible && pum_array != रिक्त;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    स्थिर पूर्णांक
pum_in_same_position(शून्य)
{
    पूर्णांक	    row = (State & MODE_CMDLINE)
			? cmdline_row
			: curwin->w_wrow + W_WINROW(curwin);

    परत pum_window != curwin
	    || (pum_win_row == row
		&& pum_win_height == curwin->w_height
		&& pum_win_col == curwin->w_wincol
		&& pum_win_width == curwin->w_width);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    पूर्णांक
pum_redraw_in_same_position(शून्य)
{
    जर (!pum_visible() || pum_will_redraw)
	परत FALSE;  // nothing to do

    परत pum_in_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    शून्य
pum_may_redraw(शून्य)
{
    pumitem_T	*array = pum_array;
    पूर्णांक		len = pum_size;
    पूर्णांक		selected = pum_selected;

    जर (!pum_visible() || pum_will_redraw)
	परत;  // nothing to do

    जर (pum_in_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    अन्यथा
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
    पूर्णांक
pum_get_height(शून्य)
{
    परत pum_height;
}

#जर defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    शून्य
pum_set_event_info(dict_T *dict)
{
    जर (!pum_visible())
	परत;
    (शून्य)dict_add_संख्या(dict, "height", pum_height);
    (शून्य)dict_add_संख्या(dict, "width", pum_width);
    (शून्य)dict_add_संख्या(dict, "row", pum_row);
    (शून्य)dict_add_संख्या(dict, "col", pum_col);
    (शून्य)dict_add_संख्या(dict, "size", pum_size);
    (शून्य)dict_add_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#endif

#जर defined(FEAT_BEVAL_TERM) || defined(FEAT_TERM_POPUP_MENU)
    स्थिर शून्य
pum_position_at_mouse(पूर्णांक min_width)
{
    पूर्णांक extra_height = pum_border * 2 + pum_shadow;
    पूर्णांक extra_offset = pum_border + pum_shadow;
    पूर्णांक trailing_overhead = 0;
    पूर्णांक col_offset = 0;
    पूर्णांक available_width;

    जर (Rows - mouse_row - extra_height > pum_size
	    || Rows - mouse_row > mouse_row)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_row = mouse_row + 1 + pum_border;
	जर (pum_height > Rows - pum_row - extra_height)
	    pum_height = Rows - pum_row - extra_height;
	जर (pum_row + pum_height + extra_offset >= cmdline_row)
	    pum_in_cmdline = TRUE;
    }
    अन्यथा
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_row = mouse_row - pum_size - extra_offset;
	जर (pum_row < pum_border)
	{
	    pum_height += pum_row - pum_border;
	    pum_row = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	col_offset = (mouse_col == Columns - 1) && pum_border;
	available_width = mouse_col + 1 - trailing_overhead - col_offset;
	जर (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col - col_offset;
	अन्यथा
	    // Not enough space, left align with window.
	    pum_col = MIN(pum_base_width, min_width) - 1 + trailing_overhead;
	pum_width = pum_col + 1 - trailing_overhead;
    }
    अन्यथा
# endif
    {
	col_offset = (mouse_col == 0) && pum_border;
	available_width = Columns - mouse_col - trailing_overhead - col_offset;
	जर (available_width >= pum_base_width || available_width > min_width)
	    // Enough space to show at mouse column.
	    pum_col = mouse_col + col_offset;
	अन्यथा
	    // Not enough space, right align with window.
	    pum_col = Columns - MIN(pum_base_width, min_width)
		- trailing_overhead;
	pum_width = Columns - pum_col - trailing_overhead;
    }

    pum_width = MIN(pum_width, pum_base_width + 1);
    // Do not redraw at cursor position.
    pum_window = रिक्त;
}

#endif

#जर defined(FEAT_BEVAL_TERM)
स्थिर pumitem_T *balloon_array = रिक्त;
स्थिर पूर्णांक balloon_arraysize;

# define BALLOON_MIN_WIDTH 50
# define BALLOON_MIN_HEIGHT 10

प्रकारव्याख्या संरचना {
    char_u	*start;
    पूर्णांक		bytelen;
    पूर्णांक		cells;
    पूर्णांक		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    पूर्णांक
split_message(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*item;
    पूर्णांक		quoted = FALSE;
    पूर्णांक		height;
    पूर्णांक		ओळ;
    पूर्णांक		item_idx;
    पूर्णांक		indent = 0;
    पूर्णांक		max_cells = 0;
    पूर्णांक		max_height = Rows / 2 - 1;
    पूर्णांक		long_item_मोजणी = 0;
    पूर्णांक		split_long_वस्तुसंच = FALSE;

    ga_init2(&ga, आकार(balpart_T), 20);
    p = mesg;

    पर्यंत (*p != NUL)
    {
	जर (ga_grow(&ga, 1) == FAIL)
	    जा failed;
	item = ((balpart_T *)ga.ga_data) + ga.ga_len;
	item->start = p;
	item->indent = indent;
	item->cells = indent * 2;
	++ga.ga_len;
	पर्यंत (*p != NUL)
	{
	    जर (*p == '"')
		quoted = !quoted;
	    अन्यथा जर (*p == '\n')
		थांबा;
	    अन्यथा जर (*p == '\\' && p[1] != NUL)
		++p;
	    अन्यथा जर (!quoted)
	    {
		जर ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    जर (*p == '{')
			++indent;
		    अन्यथा जर (*p == '}' && indent > 0)
			--indent;
		    ++item->cells;
		    p = skipwhite(p + 1);
		    थांबा;
		}
	    }
	    item->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	item->bytelen = p - item->start;
	जर (*p == '\n')
	    ++p;
	जर (item->cells > max_cells)
	    max_cells = item->cells;
	long_item_मोजणी += (item->cells - 1) / BALLOON_MIN_WIDTH;
    }

    height = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    जर (long_item_मोजणी > 0 && height + long_item_मोजणी <= max_height)
    {
	split_long_वस्तुसंच = TRUE;
	height += long_item_मोजणी;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    जर (height > max_height)
	height = max_height;
    *array = ALLOC_CLEAR_MULT(pumitem_T, height);
    जर (*array == रिक्त)
	जा failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_text = vim_strsave((char_u *)"");
    (*array + height - 1)->pum_text = vim_strsave((char_u *)"");

    साठी (ओळ = 1, item_idx = 0; ओळ < height - 1; ++item_idx)
    {
	पूर्णांक	skip;
	पूर्णांक	thislen;
	पूर्णांक	copylen;
	पूर्णांक	ind;
	पूर्णांक	cells;

	item = ((balpart_T *)ga.ga_data) + item_idx;
	जर (item->bytelen == 0)
	    (*array)[ओळ++].pum_text = vim_strsave((char_u *)"");
	अन्यथा
	    साठी (skip = 0; skip < item->bytelen; skip += thislen)
	    {
		जर (split_long_वस्तुसंच && item->cells >= BALLOON_MIN_WIDTH)
		{
		    cells = item->indent * 2;
		    साठी (p = item->start + skip;
			    p < item->start + item->bytelen;
							    p += mb_ptr2len(p))
			जर ((cells += ptr2cells(p)) > BALLOON_MIN_WIDTH)
			    थांबा;
		    thislen = p - (item->start + skip);
		}
		अन्यथा
		    thislen = item->bytelen;

		// put indent at the start
		p = alloc(thislen + item->indent * 2 + 1);
		जर (p == रिक्त)
		{
		    साठी (ओळ = 0; ओळ <= height - 1; ++ओळ)
			vim_free((*array)[ओळ].pum_text);
		    vim_free(*array);
		    जा failed;
		}
		साठी (ind = 0; ind < item->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		साठी (copylen = thislen; copylen > 0; --copylen)
		    जर (item->start[skip + copylen - 1] != ' ')
			थांबा;

		vim_strncpy(p + ind, item->start + skip, copylen);
		(*array)[ओळ].pum_text = p;
		item->indent = 0;  // wrapped line has no indent
		++ओळ;
	    }
    }
    ga_clear(&ga);
    परत height;

failed:
    ga_clear(&ga);
    परत 0;
}

    शून्य
ui_remove_balloon(शून्य)
{
    जर (balloon_array == रिक्त)
	परत;

    pum_undisplay();
    पर्यंत (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_text);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    शून्य
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    जर (mesg == रिक्त && list == रिक्त)
    {
	pum_undisplay();
	परत;
    }
    जर (list != रिक्त)
    {
	listitem_T  *li;
	पूर्णांक	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	जर (balloon_array == रिक्त)
	    परत;
	CHECK_LIST_MATERIALIZE(list);
	साठी (idx = 0, li = list->lv_first; li != रिक्त; li = li->li_next, ++idx)
	{
	    char_u *text = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_text = vim_strsave(
					   text == रिक्त ? (char_u *)"" : text);
	}
    }
    अन्यथा
	balloon_arraysize = split_message(mesg, &balloon_array);

    जर (balloon_arraysize <= 0)
	परत;

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
    शून्य
ui_may_remove_balloon(शून्य)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#endif

#जर defined(FEAT_TERM_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    स्थिर शून्य
pum_select_mouse_pos(शून्य)
{
    पूर्णांक idx = mouse_row - pum_row;

    जर (idx < 0 || idx >= pum_height)
	pum_selected = -1;
    अन्यथा जर (*pum_array[idx].pum_text != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    स्थिर शून्य
pum_execute_menu(vimmenu_T *menu, पूर्णांक mode)
{
    vimmenu_T   *mp;
    पूर्णांक		idx = 0;
    exarg_T	ea;

    FOR_ALL_CHILD_MENUS(menu, mp)
	जर ((mp->modes & mp->enabled & mode) && idx++ == pum_selected)
	{
	    CLEAR_FIELD(ea);
	    execute_menu(&ea, mp, -1);
	    थांबा;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    शून्य
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    पूर्णांक		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_TERM
    पूर्णांक		save_bevalterm = p_bevalterm;
# endif
    पूर्णांक		mode;

    pum_undisplay();
    pum_size = 0;
    mode = get_menu_mode_flag();

    FOR_ALL_CHILD_MENUS(menu, mp)
	जर (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & mode))
	    ++pum_size;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    जर (pum_size <= 0)
    {
	emsg(_(e_menu_only_exists_in_another_mode));
	परत;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_size);
    जर (array == रिक्त)
	परत;

    FOR_ALL_CHILD_MENUS(menu, mp)
    {
	char_u *s = रिक्त;

	// Make a copy of the text, the menu may be redefined in a callback.
	जर (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	अन्यथा जर (mp->modes & mp->enabled & mode)
	    s = mp->dname;
	जर (s != रिक्त)
	{
	    s = vim_strsave(s);
	    जर (s != रिक्त)
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

    साठी (;;)
    {
	पूर्णांक	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	out_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	जर (c == ESC || c == Ctrl_C || pum_array == रिक्त)
	    थांबा;
	अन्यथा जर (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, mode);
	    थांबा;
	}
	अन्यथा जर (c == 'k' || c == K_UP || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    पर्यंत (pum_selected > 0)
	    {
		--pum_selected;
		जर (*array[pum_selected].pum_text != NUL)
		    थांबा;
	    }
	}
	अन्यथा जर (c == 'j' || c == K_DOWN || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    पर्यंत (pum_selected < pum_size - 1)
	    {
		++pum_selected;
		जर (*array[pum_selected].pum_text != NUL)
		    थांबा;
	    }
	}
	अन्यथा जर (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    थांबा;
	}
	अन्यथा जर (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	अन्यथा जर (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    जर (pum_selected >= 0)
	    {
		pum_execute_menu(menu, mode);
		थांबा;
	    }
	    जर (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		थांबा;
	}
    }

    साठी (idx = 0; idx < pum_size; ++idx)
	vim_free(array[idx].pum_text);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_TERM
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    शून्य
pum_make_popup(char_u *path_name, पूर्णांक use_mouse_pos)
{
    vimmenu_T *menu;

    जर (!use_mouse_pos)
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
    जर (menu != रिक्त)
	pum_show_popupmenu(menu);
}
#endif

    स्थिर पूर्णांक
is_singlewidth(पूर्णांक c)
{
    जर (c >= 0 && c < 0x80)
	परत TRUE;
    जर (utf_char2cells(c) == 1)
	परत TRUE;
    परत FALSE;
}

/*
 * Set border characters for popup menu.
 */
    शून्य
pum_set_border_chars(
	पूर्णांक शीर्ष,
	पूर्णांक उजवा,
	पूर्णांक bottom,
	पूर्णांक डावा,
	पूर्णांक शीर्ष_डावा,
	पूर्णांक शीर्ष_उजवा,
	पूर्णांक bottom_उजवा,
	पूर्णांक bottom_डावा)
{
    जर (!is_singlewidth(शीर्ष) || !is_singlewidth(उजवा)
	    || !is_singlewidth(bottom) || !is_singlewidth(डावा)
	    || !is_singlewidth(शीर्ष_डावा) || !is_singlewidth(शीर्ष_उजवा)
	    || !is_singlewidth(bottom_उजवा) || !is_singlewidth(bottom_डावा))
	परत;
    pum_border_chars.शीर्ष = शीर्ष;
    pum_border_chars.उजवा = उजवा;
    pum_border_chars.bottom = bottom;
    pum_border_chars.डावा = डावा;
    pum_border_chars.शीर्ष_डावा = शीर्ष_डावा;
    pum_border_chars.शीर्ष_उजवा = शीर्ष_उजवा;
    pum_border_chars.bottom_उजवा = bottom_उजवा;
    pum_border_chars.bottom_डावा = bottom_डावा;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    स्थिर शून्य
compute_margins(पूर्णांक *उजवा_margin, पूर्णांक *डावा_margin, पूर्णांक *डावा_padding)
{
    *उजवा_margin = pum_margin && pum_border;

#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	*डावा_padding = (pum_col < Columns - 1 - pum_border);
	*डावा_margin = *उजवा_margin && (pum_col < Columns - 3);
	परत;
    }
#endif
    *डावा_padding = (pum_col > pum_border);
    *डावा_margin = *उजवा_margin && (pum_col > 2);
}

/*
 * Draw the border around the popup menu
 */
    स्थिर शून्य
pum_draw_border(शून्य)
{
    पूर्णांक	शीर्ष_row = pum_row - 1;
    पूर्णांक	bottom_row = pum_row + pum_height;
    पूर्णांक	col;
    पूर्णांक	attr = highlight_attr[HLF_PMB];
    पूर्णांक	उजवा_margin, डावा_margin, डावा_padding;
    पूर्णांक	inner_width;

    जर (!pum_border)
	परत;

    (शून्य)compute_margins(&उजवा_margin, &डावा_margin, &डावा_padding);

    inner_width = pum_width + pum_scrollbar + डावा_padding;

    // Draw border left to right or right to left (for RTL)
#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
	col = pum_col + डावा_padding + 1;
    अन्यथा
#endif
	col = pum_col - डावा_padding - 1;

    // left corners (right corners for RTL)
#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	screen_putchar(pum_border_chars.शीर्ष_उजवा, शीर्ष_row, col, attr);
	जर (डावा_margin)
	    screen_putchar(' ', शीर्ष_row, col + 1, attr);
	screen_putchar(pum_border_chars.bottom_उजवा, bottom_row, col, attr);
	जर (डावा_margin)
	    screen_putchar(' ', bottom_row, col + 1, attr);
    }
    अन्यथा
#endif
    {
	screen_putchar(pum_border_chars.शीर्ष_डावा, शीर्ष_row, col, attr);
	जर (डावा_margin)
	    screen_putchar(' ', शीर्ष_row, col - 1, attr);
	screen_putchar(pum_border_chars.bottom_डावा, bottom_row, col, attr);
	जर (डावा_margin)
	    screen_putchar(' ', bottom_row, col - 1, attr);
    }

    // Top horizontal lines
    साठी (पूर्णांक i = 0; i < inner_width; i++)
    {
	पूर्णांक cur_col;
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    cur_col = col - 1 - i;
	अन्यथा
#endif
	    cur_col = col + 1 + i;
	screen_putchar(pum_border_chars.शीर्ष, शीर्ष_row, cur_col, attr);
	screen_putchar(pum_border_chars.bottom, bottom_row, cur_col, attr);
    }

    // right corners (left corners for RTL)
#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	screen_putchar(pum_border_chars.शीर्ष_डावा, शीर्ष_row,
		col - inner_width - 1, attr);
	जर (उजवा_margin)
	    screen_putchar(' ', शीर्ष_row, col - inner_width - 2, attr);
	screen_putchar(pum_border_chars.bottom_डावा, bottom_row,
		col - inner_width - 1, attr);
	जर (उजवा_margin)
	    screen_putchar(' ', bottom_row, col - inner_width - 2, attr);
    }
    अन्यथा
#endif
    {
	screen_putchar(pum_border_chars.शीर्ष_उजवा, शीर्ष_row,
		col + inner_width + 1, attr);
	जर (उजवा_margin)
	    screen_putchar(' ', शीर्ष_row, col + inner_width + 2, attr);
	screen_putchar(pum_border_chars.bottom_उजवा, bottom_row,
		col + inner_width + 1, attr);
	जर (उजवा_margin)
	    screen_putchar(' ', bottom_row, col + inner_width + 2, attr);
    }

    // Draw side borders
    साठी (पूर्णांक i = 0; i < pum_height; i++)
    {
	पूर्णांक row = pum_row + i;
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	{
	    screen_putchar(pum_border_chars.डावा, row, col, attr);
	    जर (डावा_margin)
		screen_putchar(' ', row, col + 1, attr);
	    screen_putchar(pum_border_chars.उजवा, row,
		    col - inner_width - 1, attr);
	    जर (उजवा_margin)
		screen_putchar(' ', row, col - inner_width - 2, attr);
	}
	अन्यथा
#endif
	{
	    screen_putchar(pum_border_chars.डावा, row, col, attr);
	    जर (डावा_margin)
		screen_putchar(' ', row, col - 1, attr);
	    screen_putchar(pum_border_chars.उजवा, row,
		    col + inner_width + 1, attr);
	    जर (उजवा_margin)
		screen_putchar(' ', row, col + inner_width + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    शून्य
put_shadow_char(पूर्णांक row, पूर्णांक col)
{
    char_u  buf[MB_MAXBYTES + 1];
    पूर्णांक	    shadow_attr = highlight_attr[HLF_PMS];
    पूर्णांक	    char_attr, new_attr;

    screen_getbytes(row, col, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), row, col, new_attr);
}

/*
 * Draw the shadow
 */
    स्थिर शून्य
pum_draw_shadow(शून्य)
{
    पूर्णांक	row, col, next_col;
    पूर्णांक	width, offset;
    पूर्णांक	inner_width;
    पूर्णांक	उजवा_margin, डावा_margin, डावा_padding;

    जर (!pum_shadow)
	परत;

    (शून्य)compute_margins(&उजवा_margin, &डावा_margin, &डावा_padding);
    inner_width = pum_width + pum_scrollbar + pum_border + उजवा_margin;

    // Draw right side shadow (left for RTL)
#ifdef FEAT_RIGHTLEFT
    जर (pum_rl)
    {
	col = pum_col - inner_width;
	next_col = col - 1;
    }
    अन्यथा
#endif
    {
	col = pum_col + inner_width;
	next_col = col + 1;
    }

    साठी (पूर्णांक i = 0; i < pum_height + pum_border; i++)
    {
	row = pum_row + 1 + i - pum_border;
	put_shadow_char(row, col);
	put_shadow_char(row, next_col);
    }

    // Bottom shadow
    row = pum_row + pum_height + pum_border;
    width = inner_width + डावा_padding + pum_border + डावा_margin;
    offset = 2 - डावा_padding - pum_border - डावा_margin;

    साठी (पूर्णांक i = 0; i < width; i++)
    {
#ifdef FEAT_RIGHTLEFT
	जर (pum_rl)
	    col = pum_col - offset - i;
	अन्यथा
#endif
	    col = pum_col + offset + i;
	put_shadow_char(row, col);
    }
}
