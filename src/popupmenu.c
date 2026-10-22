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

ساکن pumitem_T *pum_array = تہی;	// items of displayed pum
ساکن صحیح pum_سائز;			// nr of items in "pum_array"
ساکن صحیح pum_selected;		// index of selected item or -1
ساکن صحیح pum_پہلا = 0;		// index of top item

ساکن صحیح call_update_screen = FALSE;
ساکن صحیح pum_اندر_cmdline = FALSE;

ساکن صحیح pum_اونچائی;			// nr of displayed pum items
ساکن صحیح pum_چوڑائی;			// width of displayed pum items
ساکن صحیح pum_بنیاد_چوڑائی;		// width of pum items base
ساکن صحیح pum_قسم_چوڑائی;		// width of pum items kind column
ساکن صحیح pum_extra_چوڑائی;		// width of extra stuff
ساکن صحیح pum_scrollbar;		// TRUE when scrollbar present
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
ساکن صحیح pum_rl;			// TRUE when pum is drawn 'rightleft'
#اگر_ختم

ساکن صحیح pum_صف;			// top row of pum
ساکن صحیح pum_ستون;			// left column of pum

ساکن win_T *pum_کھڑکی = تہی;
ساکن صحیح pum_win_صف;
ساکن صحیح pum_win_اونچائی;
ساکن صحیح pum_win_ستون;
ساکن صحیح pum_win_wcol;
ساکن صحیح pum_win_چوڑائی;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ساکن صحیح pum_pretend_not_visible = FALSE;

// Border support
ساکن صحیح pum_border = 0;
ساکن صحیح pum_margin = 0;	// margin of 1 cell on left and right
ساکن صحیح pum_shadow = 0;

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
ساکن image_placement_T *pum_imagep = تہی;
#اگر_ختم

// Border characters
ساکن ساخت {
    صحیح چوٹی;
    صحیح دایاں;
    صحیح bottom;
    صحیح بایاں;
    صحیح چوٹی_بایاں;
    صحیح چوٹی_دایاں;
    صحیح bottom_دایاں;
    صحیح bottom_بایاں;
} pum_border_chars;

ساکن صحیح pum_سیٹ_selected(صحیح n, صحیح repeat);
ساکن خالی pum_draw_border(خالی);
ساکن خالی pum_draw_shadow(خالی);
ساکن خالی compute_margins(صحیح *دایاں_margin, صحیح *بایاں_margin, صحیح *بایاں_padding);

#تعریف_کرو PUM_DEF_اونچائی 10

#تعریف_کرو DEFINE_PUM_SETTER(feature) \
	خالی \
    pum_سیٹ##feature(صحیح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ساکن خالی
pum_compute_سائز(خالی)
{
    صحیح	w;

    // Compute the width of the widest match and the widest extra.
    pum_بنیاد_چوڑائی = 0;
    pum_قسم_چوڑائی = 0;
    pum_extra_چوڑائی = 0;
    برائے (صحیح i = 0; i < pum_سائز; ++i)
    {
	اگر (pum_array[i].pum_متن != تہی)
	{
	    w = vim_strsize(pum_array[i].pum_متن);
	    اگر (pum_بنیاد_چوڑائی < w)
		pum_بنیاد_چوڑائی = w;
	}
	اگر (pum_array[i].pum_قسم != تہی)
	{
	    w = vim_strsize(pum_array[i].pum_قسم) + 1;
	    اگر (pum_قسم_چوڑائی < w)
		pum_قسم_چوڑائی = w;
	}
	اگر (pum_array[i].pum_extra != تہی)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    اگر (pum_extra_چوڑائی < w)
		pum_extra_چوڑائی = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ساکن خالی
pum_compute_vertical_placement(
	صحیح سائز,
	صحیح above_صف,
	صحیح نیچے_صف)
{
    صحیح context_سطریں;
    صحیح cline_visible_انحراف;
    صحیح extra_اونچائی = 2 * pum_border + pum_shadow;
    صحیح extra_انحراف = pum_border + pum_shadow;

    pum_اونچائی = MIN(سائز, PUM_DEF_اونچائی);
    اگر (p_ph > 0 && pum_اونچائی > p_ph)
	pum_اونچائی = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    اگر (pum_win_صف + 2 >= نیچے_صف - pum_اونچائی - extra_اونچائی
	    && pum_win_صف - above_صف > (نیچے_صف - above_صف) / 2)
    {
	// pum above "pum_win_row"
	اگر (State & منوال_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطریں = 0;
	ورنہ
	    // Leave two lines of context if possible
	    context_سطریں = MIN(2, curwin->w_wrow - curwin->w_cline_صف);

	اگر (pum_win_صف >= سائز + context_سطریں + extra_اونچائی)
	{
	    pum_صف = pum_win_صف - سائز - context_سطریں - extra_انحراف;
	    pum_اونچائی = سائز;
	}
	ورنہ
	{
	    pum_صف = pum_border;
	    pum_اونچائی = pum_win_صف - context_سطریں - extra_اونچائی;
	}
	اگر (p_ph > 0 && pum_اونچائی > p_ph)
	{
	    pum_صف += pum_اونچائی - p_ph;
	    pum_اونچائی = p_ph;
	}
    }
    ورنہ
    {
	// pum below "pum_win_row"
	اگر (State & منوال_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطریں = 0;
	ورنہ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_انحراف = curwin->w_cline_صف +
		curwin->w_cline_اونچائی - curwin->w_wrow;
	    context_سطریں = MIN(3, cline_visible_انحراف);
	}

	pum_صف = pum_win_صف + context_سطریں + pum_border;
	pum_اونچائی = MIN(نیچے_صف - pum_صف - extra_انحراف, سائز);
	اگر (p_ph > 0 && pum_اونچائی > p_ph)
	    pum_اونچائی = p_ph;
    }

#اگر defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    اگر (above_صف > 0 && pum_صف < above_صف && pum_اونچائی > above_صف)
    {
	pum_صف = above_صف;
	pum_اونچائی = pum_win_صف - above_صف;
    }
#اگر_ختم
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ساکن صحیح
سیٹ_pum_چوڑائی_aligned_with_cursor(صحیح چوڑائی, صحیح دستیاب_چوڑائی)
{
    صحیح انتہا_padding = TRUE;

    اگر (چوڑائی < p_pw)
    {
	چوڑائی = p_pw;
	انتہا_padding = FALSE;
    }
    اگر (p_pmw > 0 && چوڑائی >= p_pmw)
    {
	چوڑائی = p_pmw;
	انتہا_padding = FALSE;
    }

    pum_چوڑائی = چوڑائی + (انتہا_padding && چوڑائی >= p_pw ? 1 : 0);
    واپس دستیاب_چوڑائی >= pum_چوڑائی;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ساکن خالی
pum_compute_horizontal_placement(صحیح cursor_ستون)
{
    صحیح desired_چوڑائی = pum_بنیاد_چوڑائی + pum_قسم_چوڑائی + pum_extra_چوڑائی;
    صحیح دستیاب_چوڑائی;
    صحیح ستون_انحراف;
    صحیح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	ستون_انحراف = pum_border && (cursor_ستون >= Columns - 2)
	    ? (3 - Columns + cursor_ستون) : 0;
	دستیاب_چوڑائی = cursor_ستون + 1;
	pum_ستون = cursor_ستون - ستون_انحراف;
    }
    ورنہ
#اگر_ختم
    {
	ستون_انحراف = pum_border && (cursor_ستون <= 1) ? (2 - cursor_ستون) : 0;
	دستیاب_چوڑائی = Columns - cursor_ستون;
	pum_ستون = cursor_ستون + ستون_انحراف;
    }

    دستیاب_چوڑائی -= trailing_overhead + ستون_انحراف;

    // Case 1: Align pum with "cursor_col"
    اگر (سیٹ_pum_چوڑائی_aligned_with_cursor(desired_چوڑائی, دستیاب_چوڑائی))
	واپس;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    اگر (دستیاب_چوڑائی > p_pw)
    {
	pum_چوڑائی = دستیاب_چوڑائی;
	واپس;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    دستیاب_چوڑائی = Columns - trailing_overhead - ستون_انحراف;

    اگر (دستیاب_چوڑائی > p_pw)
    {
	pum_چوڑائی = p_pw;  // Truncate beyond 'pum_width'
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    pum_ستون = pum_چوڑائی + trailing_overhead;
	ورنہ
#اگر_ختم
	    pum_ستون = Columns - pum_چوڑائی - trailing_overhead;
	واپس;
    }

    // Case 3: Not enough room anywhere, use what we have
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
	pum_ستون = Columns - 1 - ستون_انحراف;
    ورنہ
#اگر_ختم
	pum_ستون = ستون_انحراف;
    pum_چوڑائی = Columns - trailing_overhead;
}

/*
 * Show the popup menu with items "array[size]".
 * "array" must remain valid until pum_undisplay() is called!
 * When possible the leftmost character is aligned with cursor column.
 * The menu appears above the screen line "row" or at "row" + "height" - 1.
 */
    خالی
pum_display(
	pumitem_T   *array,
	صحیح	    سائز,
	صحیح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحیح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحیح	    cursor_ستون;
    صحیح	    above_صف;
    صحیح	    نیچے_صف;
    صحیح	    redo_گنتی = 0;
#اگر defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    pum_rl = (State & منوال_CMDLINE) == 0 && curwin->w_p_rl;
#اگر_ختم

    کرو
    {
	above_صف = 0;
	نیچے_صف = cmdline_صف;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_ستون();
	pum_array = تہی;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_کھڑکی = curwin;
	اگر (State & منوال_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_صف = cmdline_صف;
	ورنہ
	    pum_win_صف = curwin->w_wrow + W_WINROW(curwin);
	pum_win_اونچائی = curwin->w_اونچائی;
	pum_win_ستون = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_چوڑائی = curwin->w_چوڑائی;

#اگر defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    اگر (pvwin->w_p_pvw)
		توڑو;
	اگر (pvwin != تہی)
	{
	    اگر (W_WINROW(pvwin) < W_WINROW(curwin))
		above_صف = W_WINROW(pvwin) + pvwin->w_اونچائی;
	    ورنہ اگر (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_اونچائی)
		نیچے_صف = W_WINROW(pvwin);
	}
#اگر_ختم

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(سائز, above_صف, نیچے_صف);

	// don't display when we only have room for one line
	اگر (pum_اونچائی < 1 || (pum_اونچائی == 1 && سائز > 1))
	    واپس;

	pum_array = array;
	pum_سائز = سائز;
	pum_compute_سائز();

	// Calculate cursor column
	اگر (State & منوال_CMDLINE)
	    // cmdline completion popup menu
	    cursor_ستون = cmdline_compl_startcol();
	ورنہ
	{
	    صحیح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    اگر (curwin->w_چوڑائی > 0)
		wcol %= curwin->w_چوڑائی;
	    ورنہ
		wcol = 0;
#اگر_تعریف_شدہ FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    اگر (curwin->w_p_cole > 0 && conceal_cursor_سطر(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		اگر (wcol < 0)
		    wcol = 0;
	    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (pum_rl)
		cursor_ستون = curwin->w_wincol + curwin->w_چوڑائی - wcol - 1;
	    ورنہ
#اگر_ختم
		cursor_ستون = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_اونچائی < سائز) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_ستون);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } جب (pum_سیٹ_selected(selected, redo_گنتی) && ++redo_گنتی <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    خالی
pum_call_update_screen(خالی)
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
pum_under_menu(صحیح صف, صحیح ستون, صحیح only_redrawing)
{
    صحیح	extra_بایاں = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحیح	extra_دایاں = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحیح	extra_above = pum_border;
    صحیح	extra_نیچے = pum_border + (pum_shadow ? 1 : 0);
    صحیح	چوٹی = pum_صف - extra_above;
    صحیح	bot = pum_صف + pum_اونچائی + extra_نیچے;
    صحیح	بایاں = pum_ستون - 1 - extra_بایاں;
    صحیح	دایاں = pum_ستون + pum_چوڑائی + pum_scrollbar + extra_دایاں;

    اگر (!((!only_redrawing || pum_will_redraw)
	    && صف >= چوٹی && صف < bot && ستون >= بایاں && ستون < دایاں))
	واپس FALSE;

    اگر (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	اگر (only_redrawing)
	{
	    اگر (ستون >= دایاں - 2 || صف == bot - 1)
		واپس FALSE;
	}
	ورنہ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحیح	    دایاں_margin, بایاں_margin, بایاں_padding;

	    compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);
	    اگر (صف == چوٹی && ستون >= دایاں - 2)
		واپس FALSE;
	    اگر (صف == bot - 1 && ستون < pum_ستون + 2 - بایاں_padding - pum_border
		    - بایاں_margin)
		واپس FALSE;
	}
    }
    واپس TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ساکن صحیح *
pum_compute_متن_attrs(char_u *متن, hlf_T hlf, صحیح user_hlattr)
{
    صحیح		i;
    size_t	leader_len;
    صحیح		char_cells;
    صحیح		new_attr;
    char_u	*ptr = متن;
    صحیح		cell_idx = 0;
    garray_T	*ga = تہی;
    صحیح		*attrs = تہی;
    char_u	*leader = تہی;
    صحیح		اندر_fuzzy;
    صحیح		matched_len = -1;
    int_u	char_pos = 0;
    صحیح		is_select = FALSE;

    اگر (*متن == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	واپس تہی;

    is_select = hlf == HLF_PSI;
    leader = (State & منوال_CMDLINE) ? cmdline_compl_نمونہ()
							  : ins_compl_leader();
    اگر (leader == تہی || *leader == NUL)
	واپس تہی;

    attrs = ALLOC_MULT(صحیح, vim_strsize(متن));
    اگر (attrs == تہی)
	واپس تہی;

    اندر_fuzzy = (State & منوال_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    اگر (اندر_fuzzy)
    {
	ga = fuzzy_مطابقت_str_with_pos(متن, leader);
	اگر (!ga)
	{
	    vim_free(attrs);
	    واپس تہی;
	}
    }

    جب (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	اگر (ga != تہی)
	{
	    // Handle fuzzy matching
	    برائے (i = 0; i < ga->ga_len; i++)
	    {
		اگر (char_pos == ((int_u *)ga->ga_ڈیٹا)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    توڑو;
		}
	    }
	}
	ورنہ
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
	برائے (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    اگر (ga != تہی)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    واپس attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ساکن خالی
pum_screen_puts_with_attrs(
    صحیح		صف,
    صحیح		ستون,
    صحیح		cells غیر_مستعمل,
    char_u	*متن,
    صحیح		textlen,
    صحیح		*attrs)
{
    صحیح		ستون_آغاز = ستون;
    char_u	*ptr = متن;
    صحیح		char_len;
    صحیح		attr;

    // Render text with proper attributes
    جب (*ptr != NUL && ptr < متن + textlen)
    {
	char_len = mb_ptr2len(ptr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    attr = attrs[ستون_آغاز + cells - ستون - 1];
	ورنہ
#اگر_ختم
	    attr = attrs[ستون - ستون_آغاز];
	screen_puts_len(ptr, char_len, صف, ستون, attr);
	ستون += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ساکن اندرونی char_u *
pum_get_شے(صحیح اشاریہ, صحیح type)
{
    انتخاب(type)
    {
	حالت CPT_ABBR: واپس pum_array[اشاریہ].pum_متن;
	حالت CPT_قسم: واپس pum_array[اشاریہ].pum_قسم;
	حالت CPT_MENU: واپس pum_array[اشاریہ].pum_extra;
    }
    واپس تہی;
}

    ساکن اندرونی صحیح
pum_user_attr_combine(صحیح idx, صحیح type, صحیح attr)
{
    صحیح user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_قسم_hlattr,
    };

    واپس user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساکن صحیح
pum_display_rtl_متن(
	صحیح	صف,
	صحیح     ستون,
	char_u  *st,
	صحیح     attr,
	صحیح     *attrs,
	صحیح     چوڑائی,
	صحیح     چوڑائی_حد_بندی,
	صحیح     totwidth,
	صحیح     اگلا_isempty,
	صحیح	selected)
{
    char_u  *rt = تہی;
    صحیح     cells = 0;
    صحیح     over_cell = 0;
    صحیح     truncated = FALSE;
    صحیح     pad = اگلا_isempty ? 0 : 2;
    صحیح     بقایا = 0;
    صحیح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحیح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    اگر (st == تہی)
	واپس ستون;

    rt = الٹ_متن(st);
    اگر (rt == تہی)
    {
	VIM_CLEAR(st);
	واپس ستون;
    }

    char_u *rt_آغاز = rt;
    cells = mb_string2cells(rt, -1);
    truncated = چوڑائی_حد_بندی - totwidth < cells + pad;

    // only draw the text that fits
    اگر (cells > چوڑائی_حد_بندی)
    {
	کرو
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} جب (cells > چوڑائی_حد_بندی);

	اگر (cells < چوڑائی_حد_بندی)
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
	صحیح     سائز = 0;

	بقایا = چوڑائی_حد_بندی - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	اگر (cells > بقایا)
	{
	    جب (cells > بقایا)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	سائز = (صحیح)STRLEN(orig_rt);
	اگر (cells < بقایا)
	    over_cell = بقایا - cells;

	cells = mb_string2cells(orig_rt, سائز);
	چوڑائی = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, صف, ستون - چوڑائی + 1, trunc_attr);

	اگر (over_cell > 0)
	    screen_fill(صف, صف + 1, ستون - چوڑائی + 2,
		    ستون - چوڑائی + 2 + over_cell, ' ', ' ', attr);
    }

    اگر (attrs == تہی)
	screen_puts_len(rt, (صحیح)STRLEN(rt), صف, ستون - cells + 1, attr);
    ورنہ
	pum_screen_puts_with_attrs(صف, ستون - cells + 1, cells, rt,
		(صحیح)STRLEN(rt), attrs);

    vim_free(rt_آغاز);
    VIM_CLEAR(st);
    واپس ستون - چوڑائی;
}
#اگر_ختم

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساکن صحیح
pum_display_ltr_متن(
	صحیح     صف,
	صحیح     ستون,
	char_u  *st,
	صحیح     attr,
	صحیح     *attrs,
	صحیح     چوڑائی,        // width already calculated in outer loop
	صحیح     چوڑائی_حد_بندی,
	صحیح     totwidth,
	صحیح     اگلا_isempty,
	صحیح	selected)
{
    صحیح     سائز = 0;
    صحیح     cells = 0;
    char_u  *st_انتہا = تہی;
    صحیح     over_cell = 0;
    صحیح     pad = اگلا_isempty ? 0 : 2;
    صحیح     truncated = FALSE;
    صحیح     بقایا = 0;
    صحیح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحیح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    اگر (st == تہی)
	واپس ستون;

    سائز = (صحیح)STRLEN(st);
    cells = (*mb_string2cells)(st, سائز);
    truncated = چوڑائی_حد_بندی - totwidth < cells + pad;

    // only draw the text that fits
    جب (سائز > 0 && ستون + cells > چوڑائی_حد_بندی + pum_ستون)
    {
	--سائز;
	اگر (has_mbyte)
	{
	    سائز -= (*mb_سر_off)(st, st + سائز);
	    cells -= (*mb_ptr2cells)(st + سائز);
	}
	ورنہ
	    --cells;
    }

    // truncated
    اگر (truncated)
    {
	بقایا = چوڑائی_حد_بندی - totwidth - 1;
	اگر (cells > بقایا)
	{
	    st_انتہا = st + سائز;
	    جب (st_انتہا > st && cells > بقایا)
	    {
		MB_PTR_عقب(st, st_انتہا);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_انتہا) : 1;
	    }
	    سائز = st_انتہا - st;
	}

	اگر (cells < بقایا)
	    over_cell = بقایا - cells;
	cells = mb_string2cells(st, سائز);
	چوڑائی = cells + over_cell + 1;
    }

    اگر (attrs == تہی)
	screen_puts_len(st, سائز, صف, ستون, attr);
    ورنہ
	pum_screen_puts_with_attrs(صف, ستون, cells, st, سائز, attrs);

    اگر (truncated)
    {
	اگر (over_cell > 0)
	    screen_fill(صف, صف + 1, ستون + cells,
		    ستون + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, صف, ستون + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    واپس ستون + چوڑائی;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ساکن صحیح
pum_process_شے(
	صحیح     صف,
	صحیح     ستون,
	صحیح     idx,
	صحیح     j,         // Current position in order array
	صحیح     *ترتیب,    // Order array
	hlf_T   hlf,
	صحیح     attr,
	صحیح     *totwidth_ptr,
	صحیح     اگلا_isempty)
{
    صحیح     شے_type = ترتیب[j];
    char_u  *s = تہی;
    char_u  *p = pum_get_شے(idx, شے_type);
    صحیح     چوڑائی = 0;  // item width
    صحیح     w;		// char width
    صحیح	    selected = idx == pum_selected;

    برائے ( ; ; MB_PTR_ADV(p))
    {
	اگر (s == تہی)
	    s = p;
	w = ptr2cells(p);
	اگر (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_چوڑائی)
	{
	    چوڑائی += w;
	    جاری;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	صحیح     *attrs = تہی;
	صحیح     saved = *p;

	اگر (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	اگر (saved != NUL)
	    *p = saved;

	اگر (شے_type == CPT_ABBR)
	    attrs = pum_compute_متن_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    ستون = pum_display_rtl_متن(صف, ستون, st, attr, attrs,
		    چوڑائی, pum_چوڑائی, *totwidth_ptr, اگلا_isempty, selected);
	ورنہ
#اگر_ختم
	    ستون = pum_display_ltr_متن(صف, ستون, st, attr, attrs,
		    چوڑائی, pum_چوڑائی, *totwidth_ptr, اگلا_isempty, selected);

	اگر (attrs != تہی)
	    VIM_CLEAR(attrs);

	اگر (*p != TAB)
	    توڑو;

	// Display two spaces for a Tab.
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, صف, ستون - 1, attr);
	    ستون -= 2;
	}
	ورنہ
#اگر_ختم
	{
	    screen_puts_len((char_u *)"  ", 2, صف, ستون, attr);
	    ستون += 2;
	}
	*totwidth_ptr += 2;
	s = تہی;  // start text at next char
	چوڑائی = 0;
    }

    واپس ستون;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ساکن خالی
pum_draw_scrollbar(
	صحیح	صف,
	صحیح	i,
	صحیح	thumb_pos,
	صحیح	thumb_اونچائی)
{
    اگر (pum_scrollbar <= 0)
	واپس;

    صحیح attr = (i >= thumb_pos && i < thumb_pos + thumb_اونچائی) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
	screen_putchar(' ', صف, pum_ستون - pum_چوڑائی, attr);
    ورنہ
#اگر_ختم
	screen_putchar(' ', صف, pum_ستون + pum_چوڑائی, attr);
}

    ساکن اندرونی خالی
pum_align_ترتیب(صحیح *ترتیب)
{
    صحیح is_default = cia_flags == 0;
    ترتیب[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ترتیب[1] = is_default ? CPT_قسم : (cia_flags / 10) % 10;
    ترتیب[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ساکن خالی pum_free_bg(خالی);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    خالی
pum_opacity_بدلا(خالی)
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

    ساکن خالی
pum_free_bg(خالی)
{
    صحیح k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_سطریں);
    VIM_CLEAR(pum_bg_سطریں_UC);
    برائے (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_سطریں_C[k]);
    pum_bg_چوٹی = 0;
    pum_bg_bot = 0;
    pum_bg_ستون_تعداد = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    خالی
pum_redraw(خالی)
{
    صحیح		صف = pum_صف;
    صحیح		ستون;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحیح		attr;
    صحیح		i, j;
    صحیح		idx;
    char_u	*p = تہی;
    صحیح		totwidth;
    صحیح		thumb_pos = 0;
    صحیح		thumb_اونچائی = 1;
    صحیح		شے_type;
    صحیح		ترتیب[3];
    صحیح		اگلا_isempty = FALSE;
    صحیح		n;
    صحیح		اشیاء_چوڑائی_array[3] = { pum_بنیاد_چوڑائی, pum_قسم_چوڑائی,
							    pum_extra_چوڑائی };
    صحیح		basic_چوڑائی;  // first item width
    صحیح		آخری_isabbr = FALSE;
    صحیح		orig_attr = -1;
    صحیح		scroll_range = pum_سائز - pum_اونچائی;
    bool	override_success;
    صحیح		opacity_فعال = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ڈالو_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    اگر (pum_imagep == تہی)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(تہی, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    اگر (pum_imagep != تہی)
    {
	image_placement_سیٹ_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_سیٹ_position(pum_imagep, pum_صف, pum_ستون);
	image_placement_سیٹ_bounding_box(pum_imagep, pum_صف, pum_ستون,
		pum_اونچائی, pum_چوڑائی);
	image_placement_سیٹ_enable(pum_imagep, true);
    }
#اگر_ختم

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
	اگر (opacity_فعال && pum_bg_سطریں != تہی
		&& (pum_bg_ستون_تعداد != screen_Columns
		    || pum_bg_bot > screen_صفیں))
	    pum_free_bg();

	اگر (opacity_فعال && pum_bg_سطریں != تہی)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ورنہ اگر (opacity_فعال)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    اگر (Screen_سطریں != تہی && ScreenAttrs != تہی)
	    {
		صحیح	save_چوٹی, save_bot, save_ncells, k;

		pum_free_bg();
		save_چوٹی = pum_صف - pum_border;
		save_bot = pum_صف + pum_اونچائی + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		اگر (save_چوٹی < 0)
		    save_چوٹی = 0;
		اگر (save_bot > screen_صفیں)
		    save_bot = screen_صفیں;
		pum_bg_چوٹی = save_چوٹی;
		pum_bg_bot = save_bot;
		pum_bg_ستون_تعداد = screen_Columns;
		اگر (save_چوٹی < save_bot)
		{
		    save_ncells = (save_bot - save_چوٹی) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_سطریں = LALLOC_MULT(schar_T, save_ncells);
		    اگر (enc_utf8)
		    {
			pum_bg_سطریں_UC = LALLOC_MULT(u8char_T, save_ncells);
			برائے (k = 0; k < MAX_MCO; ++k)
			    pum_bg_سطریں_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    اگر (pum_bg_attrs != تہی && pum_bg_سطریں != تہی)
		    {
			برائے (صحیح r = save_چوٹی; r < save_bot; ++r)
			{
			    صحیح soff = (r - save_چوٹی) * screen_Columns;
			    صحیح loff = سطر_انحراف[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * حجم(sattr_T));
			    mch_memmove(pum_bg_سطریں + soff,
				    Screen_سطریں + loff,
				    screen_Columns * حجم(schar_T));
			    اگر (enc_utf8 && pum_bg_سطریں_UC != تہی
						&& Screen_سطریں_UC != تہی)
			    {
				mch_memmove(pum_bg_سطریں_UC + soff,
					Screen_سطریں_UC + loff,
					screen_Columns * حجم(u8char_T));
				برائے (k = 0; k < MAX_MCO; ++k)
				    اگر (pum_bg_سطریں_C[k] != تہی
						    && Screen_سطریں_C[k] != تہی)
					mch_memmove(pum_bg_سطریں_C[k] + soff,
						Screen_سطریں_C[k] + loff,
						screen_Columns
						    * حجم(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ورنہ
	{
	    // Do not redraw in pum_may_redraw() and don't draw in the area
	    // where the popup menu will be.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
    }

    // never display more than we have
    pum_پہلا = MIN(pum_پہلا, scroll_range);

    اگر (pum_scrollbar)
    {
	thumb_اونچائی = pum_اونچائی * pum_اونچائی / pum_سائز;
	اگر (thumb_اونچائی == 0)
	    thumb_اونچائی = 1;
	thumb_pos = (pum_پہلا * (pum_اونچائی - thumb_اونچائی)
			    + scroll_range / 2) / scroll_range;
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#اگر_ختم

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    اگر (pum_border)
	pum_draw_border();
    اگر (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    اگر (opacity_فعال)
	screen_pum_blend = 100 - (صحیح)p_po;

    برائے (i = 0; i < pum_اونچائی; ++i)
    {
	idx = i + pum_پہلا;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    اگر (pum_ستون < curwin->w_wincol + curwin->w_چوڑائی - 1
							    - pum_border)
		screen_putchar(' ', صف, pum_ستون + 1, attr);
	}
	ورنہ
#اگر_ختم
	    اگر (pum_ستون > pum_border)
		screen_putchar(' ', صف, pum_ستون - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	ستون = pum_ستون;
	totwidth = 0;
	pum_align_ترتیب(ترتیب);
	basic_چوڑائی = اشیاء_چوڑائی_array[ترتیب[0]];
	آخری_isabbr = ترتیب[2] == CPT_ABBR;
	برائے (j = 0; j < 3; ++j)
	{
	    شے_type = ترتیب[j];
	    hlf = hlfs[شے_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    اگر (شے_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, شے_type, attr);
	    p = pum_get_شے(idx, شے_type);

	    اگر (j + 1 < 3)
		اگلا_isempty = pum_get_شے(idx, ترتیب[j + 1]) == تہی;
	    ورنہ
		اگلا_isempty = TRUE;

	    اگر (p != تہی)
		// Process and display the item
		ستون = pum_process_شے(صف, ستون, idx, j, ترتیب, hlf, attr,
						    &totwidth, اگلا_isempty);

	    اگر (j > 0)
		n = اشیاء_چوڑائی_array[ترتیب[1]] + (آخری_isabbr ? 0 : 1);
	    ورنہ
		n = ترتیب[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    اگر (j == 2
		    || (اگلا_isempty && (j == 1 || (j == 0
				&& pum_get_شے(idx, ترتیب[j + 2]) == تہی)))
		    || basic_چوڑائی + n >= pum_چوڑائی)
		توڑو;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (pum_rl)
	    {
		screen_fill(صف, صف + 1, pum_ستون - basic_چوڑائی - n + 1,
						ستون + 1, ' ', ' ', orig_attr);
		ستون = pum_ستون - basic_چوڑائی - n;
	    }
	    ورنہ
#اگر_ختم
	    {
		screen_fill(صف, صف + 1, ستون, pum_ستون + basic_چوڑائی + n,
							' ', ' ', orig_attr);
		ستون = pum_ستون + basic_چوڑائی + n;
	    }
	    totwidth = basic_چوڑائی + n;
	}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    screen_fill(صف, صف + 1, pum_ستون - pum_چوڑائی + 1, ستون + 1,
							    ' ', ' ', orig_attr);
	ورنہ
#اگر_ختم
	    screen_fill(صف, صف + 1, ستون, pum_ستون + pum_چوڑائی, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(صف, i, thumb_pos, thumb_اونچائی);

	++صف;
    }

    screen_pum_blend = 0;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    screen_zindex = 0;
#اگر_ختم

    اگر (override_success)
	نکالو_highlight_overrides();

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
}

#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    خالی
pum_position_info_popup(win_T *wp)
{
    صحیح ستون = pum_ستون + pum_چوڑائی + pum_scrollbar + 1;
    صحیح صف = pum_صف;
    صحیح botpos = POPPOS_BOTLEFT;
    صحیح	مستعمل_maxwidth_opt = FALSE;
    صحیح	دایاں_margin, بایاں_margin, dummy;
    صحیح بایاں_extra_چوڑائی, دایاں_extra_چوڑائی;

    (خالی)compute_margins(&دایاں_margin, &بایاں_margin, &dummy);

    بایاں_extra_چوڑائی = pum_border + بایاں_margin;
    دایاں_extra_چوڑائی = pum_border + دایاں_margin + (pum_shadow ? 2 : 0);
    ستون += دایاں_extra_چوڑائی;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    اگر (Columns - ستون < 20 && Columns - ستون < pum_ستون)
    {
	ستون = pum_ستون - 1 - بایاں_extra_چوڑائی;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_ستون - 1 - بایاں_extra_چوڑائی;
    }
    ورنہ
	wp->w_maxwidth = Columns - ستون + 1;
    wp->w_maxwidth -= popup_extra_چوڑائی(wp);
    اگر (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	مستعمل_maxwidth_opt = TRUE;
    }

    صف -= popup_چوٹی_extra(wp);
    اگر (wp->w_popup_flags & POPF_INFO_MENU)
    {
	اگر (pum_صف < pum_win_صف)
	{
	    // menu above cursor line, align with bottom
	    صف += pum_اونچائی;
	    wp->w_popup_pos = botpos;
	}
	ورنہ
	    // menu below cursor line, align with top
	    صف += 1;
    }
    ورنہ
	// align with the selected item
	صف += pum_selected - pum_پہلا + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    اگر (wp->w_maxwidth < 10 && !مستعمل_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ورنہ
	popup_سیٹ_wantpos_rowcol(wp, صف, ستون);
}
#اگر_ختم

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
    ساکن صحیح
pum_سیٹ_selected(صحیح n, صحیح repeat غیر_مستعمل)
{
    صحیح	    resized = FALSE;
    صحیح	    context = pum_اونچائی / 2;
    صحیح	    scroll_انحراف;
#اگر_تعریف_شدہ FEAT_QUICKFIX
    صحیح	    پچھلا_selected = pum_selected;
    غیرعلامتی	cur_cot_flags = get_cot_flags();
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحیح	    has_info = FALSE;
#اگر_ختم

    pum_selected = n;
    scroll_انحراف = pum_selected - pum_اونچائی;

    اگر (pum_selected >= 0 && pum_selected < pum_سائز)
    {
	اگر (pum_پہلا > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    اگر (pum_پہلا > pum_selected - 2)
	    {
		pum_پہلا -= pum_اونچائی - 2;
		اگر (pum_پہلا < 0)
		    pum_پہلا = 0;
		ورنہ اگر (pum_پہلا > pum_selected)
		    pum_پہلا = pum_selected;
	    }
	    ورنہ
		pum_پہلا = pum_selected;
	}
	ورنہ اگر (pum_پہلا < scroll_انحراف + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    اگر (pum_پہلا < scroll_انحراف + 3)
		pum_پہلا = MAX(pum_پہلا + pum_اونچائی - 2, scroll_انحراف + 1);
	    ورنہ
		pum_پہلا = scroll_انحراف + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	اگر (pum_اونچائی > 2)
	{
	    اگر (pum_پہلا > pum_selected - context)
		pum_پہلا = MAX(pum_selected - context, 0);  // scroll down
	    ورنہ اگر (pum_پہلا < pum_selected + context - pum_اونچائی + 1)
		pum_پہلا = pum_selected + context - pum_اونچائی + 1;  //اوپر
	}
	// adjust for the number of lines displayed
	pum_پہلا = MIN(pum_پہلا, pum_سائز - pum_اونچائی);

#اگر defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	اگر (pum_array[pum_selected].pum_info != تہی
		&& صفیں > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ورنہ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    اگر (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ورنہ اگر (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_معمول;
	    ورنہ
		use_popup = USEPOPUP_NONE;
	    اگر (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		بلاک_autocmds();
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
			&& curbuf->b_fname == تہی
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    جب (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ورنہ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, تہی, تہی, تہی, ECMD_ONE, 0, تہی);
		    --no_u_sync;
		    اگر (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			سیٹ_option_قدر_give_err((char_u *)"swf",
							  0L, تہی, OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bl",
							  0L, تہی, OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"diff",
							  0L, تہی, OPT_LOCAL);
		    }
		}
		اگر (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    برائے (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			اگر (e == تہی)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    توڑو;
			}
			*e = NUL;
			ml_append(lnum++, p, (صحیح)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_سطر_گنتی);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    اگر (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			اگر (curwin->w_اونچائی < lnum)
			{
			    win_setheight((صحیح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_بدلا = 0;
		    curbuf->b_p_ma = FALSE;
		    اگر (pum_selected != پچھلا_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ورنہ اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
			curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.ستون = 0;
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
			صحیح save_redr_حالت;

			اگر (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			اگر (ins_compl_فعال() && !resized)
			    curwin->w_redr_حالت = false;

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
			save_redr_حالت = curwin_save->w_redr_حالت;
			curwin_save->w_redr_حالت = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_حالت = save_redr_حالت;

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
#اگر_ختم
    }
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    اگر (!has_info)
	// hide any popup info window
	popup_hide_info();
#اگر_ختم

    واپس resized;
}

/*
 * Undisplay the popup menu (later).
 */
    خالی
pum_undisplay(خالی)
{
#اگر_تعریف_شدہ FEAT_IMAGE_POPUP
    اگر (pum_imagep != تہی)
    {
	image_placement_free(pum_imagep);
	pum_imagep = تہی;
    }
#اگر_ختم
    pum_free_bg();
    pum_array = تہی;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
    اگر (pum_اندر_cmdline)
    {
	clear_cmdline = TRUE;
	pum_اندر_cmdline = FALSE;
    }
    حالت_redraw_all();
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#اگر_ختم
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    خالی
pum_clear(خالی)
{
    pum_پہلا = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    صحیح
pum_visible(خالی)
{
    واپس !pum_pretend_not_visible && pum_array != تہی;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ساکن صحیح
pum_اندر_same_position(خالی)
{
    صحیح	    صف = (State & منوال_CMDLINE)
			? cmdline_صف
			: curwin->w_wrow + W_WINROW(curwin);

    واپس pum_کھڑکی != curwin
	    || (pum_win_صف == صف
		&& pum_win_اونچائی == curwin->w_اونچائی
		&& pum_win_ستون == curwin->w_wincol
		&& pum_win_چوڑائی == curwin->w_چوڑائی);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحیح
pum_redraw_اندر_same_position(خالی)
{
    اگر (!pum_visible() || pum_will_redraw)
	واپس FALSE;  // nothing to do

    واپس pum_اندر_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    خالی
pum_may_redraw(خالی)
{
    pumitem_T	*array = pum_array;
    صحیح		len = pum_سائز;
    صحیح		selected = pum_selected;

    اگر (!pum_visible() || pum_will_redraw)
	واپس;  // nothing to do

    اگر (pum_اندر_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ورنہ
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
pum_get_اونچائی(خالی)
{
    واپس pum_اونچائی;
}

#اگر defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    خالی
pum_سیٹ_event_info(dict_T *dict)
{
    اگر (!pum_visible())
	واپس;
    (خالی)dict_جوڑو_نمبر(dict, "height", pum_اونچائی);
    (خالی)dict_جوڑو_نمبر(dict, "width", pum_چوڑائی);
    (خالی)dict_جوڑو_نمبر(dict, "row", pum_صف);
    (خالی)dict_جوڑو_نمبر(dict, "col", pum_ستون);
    (خالی)dict_جوڑو_نمبر(dict, "size", pum_سائز);
    (خالی)dict_جوڑو_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#اگر_ختم

#اگر defined(FEAT_BEVAL_رکن) || defined(FEAT_رکن_POPUP_MENU)
    ساکن خالی
pum_position_at_mouse(صحیح min_چوڑائی)
{
    صحیح extra_اونچائی = pum_border * 2 + pum_shadow;
    صحیح extra_انحراف = pum_border + pum_shadow;
    صحیح trailing_overhead = 0;
    صحیح ستون_انحراف = 0;
    صحیح دستیاب_چوڑائی;

    اگر (صفیں - mouse_صف - extra_اونچائی > pum_سائز
	    || صفیں - mouse_صف > mouse_صف)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_صف = mouse_صف + 1 + pum_border;
	اگر (pum_اونچائی > صفیں - pum_صف - extra_اونچائی)
	    pum_اونچائی = صفیں - pum_صف - extra_اونچائی;
	اگر (pum_صف + pum_اونچائی + extra_انحراف >= cmdline_صف)
	    pum_اندر_cmdline = TRUE;
    }
    ورنہ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_صف = mouse_صف - pum_سائز - extra_انحراف;
	اگر (pum_صف < pum_border)
	{
	    pum_اونچائی += pum_صف - pum_border;
	    pum_صف = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	ستون_انحراف = (mouse_ستون == Columns - 1) && pum_border;
	دستیاب_چوڑائی = mouse_ستون + 1 - trailing_overhead - ستون_انحراف;
	اگر (دستیاب_چوڑائی >= pum_بنیاد_چوڑائی || دستیاب_چوڑائی > min_چوڑائی)
	    // Enough space to show at mouse column.
	    pum_ستون = mouse_ستون - ستون_انحراف;
	ورنہ
	    // Not enough space, left align with window.
	    pum_ستون = MIN(pum_بنیاد_چوڑائی, min_چوڑائی) - 1 + trailing_overhead;
	pum_چوڑائی = pum_ستون + 1 - trailing_overhead;
    }
    ورنہ
# endif
    {
	ستون_انحراف = (mouse_ستون == 0) && pum_border;
	دستیاب_چوڑائی = Columns - mouse_ستون - trailing_overhead - ستون_انحراف;
	اگر (دستیاب_چوڑائی >= pum_بنیاد_چوڑائی || دستیاب_چوڑائی > min_چوڑائی)
	    // Enough space to show at mouse column.
	    pum_ستون = mouse_ستون + ستون_انحراف;
	ورنہ
	    // Not enough space, right align with window.
	    pum_ستون = Columns - MIN(pum_بنیاد_چوڑائی, min_چوڑائی)
		- trailing_overhead;
	pum_چوڑائی = Columns - pum_ستون - trailing_overhead;
    }

    pum_چوڑائی = MIN(pum_چوڑائی, pum_بنیاد_چوڑائی + 1);
    // Do not redraw at cursor position.
    pum_کھڑکی = تہی;
}

#اگر_ختم

#اگر defined(FEAT_BEVAL_رکن)
ساکن pumitem_T *balloon_array = تہی;
ساکن صحیح balloon_arraysize;

# define BALLOON_MIN_چوڑائی 50
# define BALLOON_MIN_اونچائی 10

قسمتعریف ساخت {
    char_u	*آغاز;
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
بانٹو_پیغام(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*شے;
    صحیح		quoted = FALSE;
    صحیح		اونچائی;
    صحیح		سطر;
    صحیح		شے_idx;
    صحیح		indent = 0;
    صحیح		max_cells = 0;
    صحیح		max_اونچائی_2 = صفیں / 2 - 1;
    صحیح		long_شے_گنتی = 0;
    صحیح		بانٹو_long_اشیاء = FALSE;

    ga_init2(&ga, حجم(balpart_T), 20);
    p = mesg;

    جب (*p != NUL)
    {
	اگر (ga_grow(&ga, 1) == FAIL)
	    جاؤ failed;
	شے = ((balpart_T *)ga.ga_ڈیٹا) + ga.ga_len;
	شے->آغاز = p;
	شے->indent = indent;
	شے->cells = indent * 2;
	++ga.ga_len;
	جب (*p != NUL)
	{
	    اگر (*p == '"')
		quoted = !quoted;
	    ورنہ اگر (*p == '\n')
		توڑو;
	    ورنہ اگر (*p == '\\' && p[1] != NUL)
		++p;
	    ورنہ اگر (!quoted)
	    {
		اگر ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    اگر (*p == '{')
			++indent;
		    ورنہ اگر (*p == '}' && indent > 0)
			--indent;
		    ++شے->cells;
		    p = skipwhite(p + 1);
		    توڑو;
		}
	    }
	    شے->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	شے->bytelen = p - شے->آغاز;
	اگر (*p == '\n')
	    ++p;
	اگر (شے->cells > max_cells)
	    max_cells = شے->cells;
	long_شے_گنتی += (شے->cells - 1) / BALLOON_MIN_چوڑائی;
    }

    اونچائی = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    اگر (long_شے_گنتی > 0 && اونچائی + long_شے_گنتی <= max_اونچائی_2)
    {
	بانٹو_long_اشیاء = TRUE;
	اونچائی += long_شے_گنتی;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    اگر (اونچائی > max_اونچائی_2)
	اونچائی = max_اونچائی_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, اونچائی);
    اگر (*array == تہی)
	جاؤ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_متن = vim_strsave((char_u *)"");
    (*array + اونچائی - 1)->pum_متن = vim_strsave((char_u *)"");

    برائے (سطر = 1, شے_idx = 0; سطر < اونچائی - 1; ++شے_idx)
    {
	صحیح	skip;
	صحیح	thislen;
	صحیح	copylen;
	صحیح	ind;
	صحیح	cells;

	شے = ((balpart_T *)ga.ga_ڈیٹا) + شے_idx;
	اگر (شے->bytelen == 0)
	    (*array)[سطر++].pum_متن = vim_strsave((char_u *)"");
	ورنہ
	    برائے (skip = 0; skip < شے->bytelen; skip += thislen)
	    {
		اگر (بانٹو_long_اشیاء && شے->cells >= BALLOON_MIN_چوڑائی)
		{
		    cells = شے->indent * 2;
		    برائے (p = شے->آغاز + skip;
			    p < شے->آغاز + شے->bytelen;
							    p += mb_ptr2len(p))
			اگر ((cells += ptr2cells(p)) > BALLOON_MIN_چوڑائی)
			    توڑو;
		    thislen = p - (شے->آغاز + skip);
		}
		ورنہ
		    thislen = شے->bytelen;

		// put indent at the start
		p = alloc(thislen + شے->indent * 2 + 1);
		اگر (p == تہی)
		{
		    برائے (سطر = 0; سطر <= اونچائی - 1; ++سطر)
			vim_free((*array)[سطر].pum_متن);
		    vim_free(*array);
		    جاؤ failed;
		}
		برائے (ind = 0; ind < شے->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		برائے (copylen = thislen; copylen > 0; --copylen)
		    اگر (شے->آغاز[skip + copylen - 1] != ' ')
			توڑو;

		vim_strncpy(p + ind, شے->آغاز + skip, copylen);
		(*array)[سطر].pum_متن = p;
		شے->indent = 0;  // wrapped line has no indent
		++سطر;
	    }
    }
    ga_clear(&ga);
    واپس اونچائی;

failed:
    ga_clear(&ga);
    واپس 0;
}

    خالی
ui_remove_balloon(خالی)
{
    اگر (balloon_array == تہی)
	واپس;

    pum_undisplay();
    جب (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_متن);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    خالی
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    اگر (mesg == تہی && list == تہی)
    {
	pum_undisplay();
	واپس;
    }
    اگر (list != تہی)
    {
	listitem_T  *li;
	صحیح	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	اگر (balloon_array == تہی)
	    واپس;
	CHECK_LIST_MATERIALIZE(list);
	برائے (idx = 0, li = list->lv_پہلا; li != تہی; li = li->li_اگلا, ++idx)
	{
	    char_u *متن = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_متن = vim_strsave(
					   متن == تہی ? (char_u *)"" : متن);
	}
    }
    ورنہ
	balloon_arraysize = بانٹو_پیغام(mesg, &balloon_array);

    اگر (balloon_arraysize <= 0)
	واپس;

    pum_array = balloon_array;
    pum_سائز = balloon_arraysize;
    pum_compute_سائز();
    pum_scrollbar = 0;
    pum_اونچائی = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_چوڑائی);
    pum_selected = -1;
    pum_پہلا = 0;
    pum_redraw();
}

/*
 * Called when the mouse moved, may remove any displayed balloon.
 */
    خالی
ui_may_remove_balloon(خالی)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#اگر_ختم

#اگر defined(FEAT_رکن_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ساکن خالی
pum_select_mouse_pos(خالی)
{
    صحیح idx = mouse_صف - pum_صف;

    اگر (idx < 0 || idx >= pum_اونچائی)
	pum_selected = -1;
    ورنہ اگر (*pum_array[idx].pum_متن != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ساکن خالی
pum_execute_menu(vimmenu_T *menu, صحیح منوال)
{
    vimmenu_T   *mp;
    صحیح		idx = 0;
    exarg_T	ea;

    FOR_ALL_بچہ_MENUS(menu, mp)
	اگر ((mp->modes & mp->enabled & منوال) && idx++ == pum_selected)
	{
	    CLEAR_میدان(ea);
	    execute_menu(&ea, mp, -1);
	    توڑو;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    خالی
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    صحیح		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_رکن
    صحیح		save_bevalterm = p_bevalterm;
# endif
    صحیح		منوال;

    pum_undisplay();
    pum_سائز = 0;
    منوال = get_menu_منوال_flag();

    FOR_ALL_بچہ_MENUS(menu, mp)
	اگر (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & منوال))
	    ++pum_سائز;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    اگر (pum_سائز <= 0)
    {
	emsg(_(e_menu_only_exists_اندر_another_منوال));
	واپس;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_سائز);
    اگر (array == تہی)
	واپس;

    FOR_ALL_بچہ_MENUS(menu, mp)
    {
	char_u *s = تہی;

	// Make a copy of the text, the menu may be redefined in a callback.
	اگر (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ورنہ اگر (mp->modes & mp->enabled & منوال)
	    s = mp->dname;
	اگر (s != تہی)
	{
	    s = vim_strsave(s);
	    اگر (s != تہی)
		array[idx++].pum_متن = s;
	}
    }

    pum_array = array;
    pum_compute_سائز();
    pum_scrollbar = 0;
    pum_اونچائی = pum_سائز;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_پہلا = 0;
# ifdef FEAT_BEVAL_رکن
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    برائے (;;)
    {
	صحیح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	برآمد_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	اگر (c == ESC || c == Ctrl_C || pum_array == تہی)
	    توڑو;
	ورنہ اگر (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, منوال);
	    توڑو;
	}
	ورنہ اگر (c == 'k' || c == K_اوپر || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    جب (pum_selected > 0)
	    {
		--pum_selected;
		اگر (*array[pum_selected].pum_متن != NUL)
		    توڑو;
	    }
	}
	ورنہ اگر (c == 'j' || c == K_نیچے_کو || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    جب (pum_selected < pum_سائز - 1)
	    {
		++pum_selected;
		اگر (*array[pum_selected].pum_متن != NUL)
		    توڑو;
	    }
	}
	ورنہ اگر (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    توڑو;
	}
	ورنہ اگر (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ورنہ اگر (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    اگر (pum_selected >= 0)
	    {
		pum_execute_menu(menu, منوال);
		توڑو;
	    }
	    اگر (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		توڑو;
	}
    }

    برائے (idx = 0; idx < pum_سائز; ++idx)
	vim_free(array[idx].pum_متن);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_رکن
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    خالی
pum_make_popup(char_u *راستہ_name, صحیح use_mouse_pos)
{
    vimmenu_T *menu;

    اگر (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_صف = W_WINROW(curwin) + curwin->w_wrow;
	mouse_ستون =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_ڈھونڈو_menu(راستہ_name);
    اگر (menu != تہی)
	pum_show_popupmenu(menu);
}
#اگر_ختم

    ساکن صحیح
is_singlewidth(صحیح c)
{
    اگر (c >= 0 && c < 0x80)
	واپس TRUE;
    اگر (utf_char2cells(c) == 1)
	واپس TRUE;
    واپس FALSE;
}

/*
 * Set border characters for popup menu.
 */
    خالی
pum_سیٹ_border_chars(
	صحیح چوٹی,
	صحیح دایاں,
	صحیح bottom,
	صحیح بایاں,
	صحیح چوٹی_بایاں,
	صحیح چوٹی_دایاں,
	صحیح bottom_دایاں,
	صحیح bottom_بایاں)
{
    اگر (!is_singlewidth(چوٹی) || !is_singlewidth(دایاں)
	    || !is_singlewidth(bottom) || !is_singlewidth(بایاں)
	    || !is_singlewidth(چوٹی_بایاں) || !is_singlewidth(چوٹی_دایاں)
	    || !is_singlewidth(bottom_دایاں) || !is_singlewidth(bottom_بایاں))
	واپس;
    pum_border_chars.چوٹی = چوٹی;
    pum_border_chars.دایاں = دایاں;
    pum_border_chars.bottom = bottom;
    pum_border_chars.بایاں = بایاں;
    pum_border_chars.چوٹی_بایاں = چوٹی_بایاں;
    pum_border_chars.چوٹی_دایاں = چوٹی_دایاں;
    pum_border_chars.bottom_دایاں = bottom_دایاں;
    pum_border_chars.bottom_بایاں = bottom_بایاں;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ساکن خالی
compute_margins(صحیح *دایاں_margin, صحیح *بایاں_margin, صحیح *بایاں_padding)
{
    *دایاں_margin = pum_margin && pum_border;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	*بایاں_padding = (pum_ستون < Columns - 1 - pum_border);
	*بایاں_margin = *دایاں_margin && (pum_ستون < Columns - 3);
	واپس;
    }
#اگر_ختم
    *بایاں_padding = (pum_ستون > pum_border);
    *بایاں_margin = *دایاں_margin && (pum_ستون > 2);
}

/*
 * Draw the border around the popup menu
 */
    ساکن خالی
pum_draw_border(خالی)
{
    صحیح	چوٹی_صف = pum_صف - 1;
    صحیح	bottom_صف = pum_صف + pum_اونچائی;
    صحیح	ستون;
    صحیح	attr = highlight_attr[HLF_PMB];
    صحیح	دایاں_margin, بایاں_margin, بایاں_padding;
    صحیح	inner_چوڑائی;

    اگر (!pum_border)
	واپس;

    (خالی)compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);

    inner_چوڑائی = pum_چوڑائی + pum_scrollbar + بایاں_padding;

    // Draw border left to right or right to left (for RTL)
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
	ستون = pum_ستون + بایاں_padding + 1;
    ورنہ
#اگر_ختم
	ستون = pum_ستون - بایاں_padding - 1;

    // left corners (right corners for RTL)
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٹی_دایاں, چوٹی_صف, ستون, attr);
	اگر (بایاں_margin)
	    screen_putchar(' ', چوٹی_صف, ستون + 1, attr);
	screen_putchar(pum_border_chars.bottom_دایاں, bottom_صف, ستون, attr);
	اگر (بایاں_margin)
	    screen_putchar(' ', bottom_صف, ستون + 1, attr);
    }
    ورنہ
#اگر_ختم
    {
	screen_putchar(pum_border_chars.چوٹی_بایاں, چوٹی_صف, ستون, attr);
	اگر (بایاں_margin)
	    screen_putchar(' ', چوٹی_صف, ستون - 1, attr);
	screen_putchar(pum_border_chars.bottom_بایاں, bottom_صف, ستون, attr);
	اگر (بایاں_margin)
	    screen_putchar(' ', bottom_صف, ستون - 1, attr);
    }

    // Top horizontal lines
    برائے (صحیح i = 0; i < inner_چوڑائی; i++)
    {
	صحیح cur_ستون;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    cur_ستون = ستون - 1 - i;
	ورنہ
#اگر_ختم
	    cur_ستون = ستون + 1 + i;
	screen_putchar(pum_border_chars.چوٹی, چوٹی_صف, cur_ستون, attr);
	screen_putchar(pum_border_chars.bottom, bottom_صف, cur_ستون, attr);
    }

    // right corners (left corners for RTL)
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٹی_بایاں, چوٹی_صف,
		ستون - inner_چوڑائی - 1, attr);
	اگر (دایاں_margin)
	    screen_putchar(' ', چوٹی_صف, ستون - inner_چوڑائی - 2, attr);
	screen_putchar(pum_border_chars.bottom_بایاں, bottom_صف,
		ستون - inner_چوڑائی - 1, attr);
	اگر (دایاں_margin)
	    screen_putchar(' ', bottom_صف, ستون - inner_چوڑائی - 2, attr);
    }
    ورنہ
#اگر_ختم
    {
	screen_putchar(pum_border_chars.چوٹی_دایاں, چوٹی_صف,
		ستون + inner_چوڑائی + 1, attr);
	اگر (دایاں_margin)
	    screen_putchar(' ', چوٹی_صف, ستون + inner_چوڑائی + 2, attr);
	screen_putchar(pum_border_chars.bottom_دایاں, bottom_صف,
		ستون + inner_چوڑائی + 1, attr);
	اگر (دایاں_margin)
	    screen_putchar(' ', bottom_صف, ستون + inner_چوڑائی + 2, attr);
    }

    // Draw side borders
    برائے (صحیح i = 0; i < pum_اونچائی; i++)
    {
	صحیح صف = pum_صف + i;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	{
	    screen_putchar(pum_border_chars.بایاں, صف, ستون, attr);
	    اگر (بایاں_margin)
		screen_putchar(' ', صف, ستون + 1, attr);
	    screen_putchar(pum_border_chars.دایاں, صف,
		    ستون - inner_چوڑائی - 1, attr);
	    اگر (دایاں_margin)
		screen_putchar(' ', صف, ستون - inner_چوڑائی - 2, attr);
	}
	ورنہ
#اگر_ختم
	{
	    screen_putchar(pum_border_chars.بایاں, صف, ستون, attr);
	    اگر (بایاں_margin)
		screen_putchar(' ', صف, ستون - 1, attr);
	    screen_putchar(pum_border_chars.دایاں, صف,
		    ستون + inner_چوڑائی + 1, attr);
	    اگر (دایاں_margin)
		screen_putchar(' ', صف, ستون + inner_چوڑائی + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    خالی
put_shadow_char(صحیح صف, صحیح ستون)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحیح	    shadow_attr = highlight_attr[HLF_PMS];
    صحیح	    char_attr, new_attr;

    screen_getbytes(صف, ستون, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), صف, ستون, new_attr);
}

/*
 * Draw the shadow
 */
    ساکن خالی
pum_draw_shadow(خالی)
{
    صحیح	صف, ستون, اگلا_ستون;
    صحیح	چوڑائی, انحراف;
    صحیح	inner_چوڑائی;
    صحیح	دایاں_margin, بایاں_margin, بایاں_padding;

    اگر (!pum_shadow)
	واپس;

    (خالی)compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);
    inner_چوڑائی = pum_چوڑائی + pum_scrollbar + pum_border + دایاں_margin;

    // Draw right side shadow (left for RTL)
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (pum_rl)
    {
	ستون = pum_ستون - inner_چوڑائی;
	اگلا_ستون = ستون - 1;
    }
    ورنہ
#اگر_ختم
    {
	ستون = pum_ستون + inner_چوڑائی;
	اگلا_ستون = ستون + 1;
    }

    برائے (صحیح i = 0; i < pum_اونچائی + pum_border; i++)
    {
	صف = pum_صف + 1 + i - pum_border;
	put_shadow_char(صف, ستون);
	put_shadow_char(صف, اگلا_ستون);
    }

    // Bottom shadow
    صف = pum_صف + pum_اونچائی + pum_border;
    چوڑائی = inner_چوڑائی + بایاں_padding + pum_border + بایاں_margin;
    انحراف = 2 - بایاں_padding - pum_border - بایاں_margin;

    برائے (صحیح i = 0; i < چوڑائی; i++)
    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (pum_rl)
	    ستون = pum_ستون - انحراف - i;
	ورنہ
#اگر_ختم
	    ستون = pum_ستون + انحراف + i;
	put_shadow_char(صف, ستون);
    }
}
