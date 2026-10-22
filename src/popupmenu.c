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
ساكن صحيح pum_اندازه;			// nr of items in "pum_array"
ساكن صحيح pum_selected;		// index of selected item or -1
ساكن صحيح pum_اولی = 0;		// index of top item

ساكن صحيح call_update_screen = FALSE;
ساكن صحيح pum_درون_cmdline = FALSE;

ساكن صحيح pum_ارتفاع;			// nr of displayed pum items
ساكن صحيح pum_عرض;			// width of displayed pum items
ساكن صحيح pum_پایه_عرض;		// width of pum items base
ساكن صحيح pum_نوع_عرض;		// width of pum items kind column
ساكن صحيح pum_extra_عرض;		// width of extra stuff
ساكن صحيح pum_scrollbar;		// TRUE when scrollbar present
#اگر_تعریف_شده FEAT_RIGHTLEFT
ساكن صحيح pum_rl;			// TRUE when pum is drawn 'rightleft'
#پایان_اگر

ساكن صحيح pum_ردیف;			// top row of pum
ساكن صحيح pum_ستون;			// left column of pum

ساكن win_T *pum_پنجره = عدم;
ساكن صحيح pum_win_ردیف;
ساكن صحيح pum_win_ارتفاع;
ساكن صحيح pum_win_ستون;
ساكن صحيح pum_win_wcol;
ساكن صحيح pum_win_عرض;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ساكن صحيح pum_pretend_not_visible = FALSE;

// Border support
ساكن صحيح pum_border = 0;
ساكن صحيح pum_margin = 0;	// margin of 1 cell on left and right
ساكن صحيح pum_shadow = 0;

#اگر_تعریف_شده FEAT_IMAGE_POPUP
ساكن image_placement_T *pum_imagep = عدم;
#پایان_اگر

// Border characters
ساكن بنية {
    صحيح بالاترین;
    صحيح راست_2;
    صحيح bottom;
    صحيح چپ_2;
    صحيح بالاترین_چپ;
    صحيح بالاترین_راست;
    صحيح bottom_راست;
    صحيح bottom_چپ;
} pum_border_chars;

ساكن صحيح pum_مجموعه_selected(صحيح n, صحيح repeat);
ساكن فراغ pum_draw_border(فراغ);
ساكن فراغ pum_draw_shadow(فراغ);
ساكن فراغ compute_margins(صحيح *راست_margin, صحيح *چپ_margin, صحيح *چپ_padding);

#تعریف_کن PUM_DEF_ارتفاع 10

#تعریف_کن DEFINE_PUM_SETTER(feature) \
	فراغ \
    pum_مجموعه##feature(صحيح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ساكن فراغ
pum_compute_اندازه(فراغ)
{
    صحيح	w;

    // Compute the width of the widest match and the widest extra.
    pum_پایه_عرض = 0;
    pum_نوع_عرض = 0;
    pum_extra_عرض = 0;
    لكل (صحيح i = 0; i < pum_اندازه; ++i)
    {
	إذا (pum_array[i].pum_متن != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_متن);
	    إذا (pum_پایه_عرض < w)
		pum_پایه_عرض = w;
	}
	إذا (pum_array[i].pum_نوع != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_نوع) + 1;
	    إذا (pum_نوع_عرض < w)
		pum_نوع_عرض = w;
	}
	إذا (pum_array[i].pum_extra != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    إذا (pum_extra_عرض < w)
		pum_extra_عرض = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ساكن فراغ
pum_compute_vertical_placement(
	صحيح اندازه,
	صحيح above_ردیف,
	صحيح زیر_ردیف)
{
    صحيح context_سطرها;
    صحيح cline_visible_جابه_جایی;
    صحيح extra_ارتفاع = 2 * pum_border + pum_shadow;
    صحيح extra_جابه_جایی = pum_border + pum_shadow;

    pum_ارتفاع = MIN(اندازه, PUM_DEF_ارتفاع);
    إذا (p_ph > 0 && pum_ارتفاع > p_ph)
	pum_ارتفاع = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    إذا (pum_win_ردیف + 2 >= زیر_ردیف - pum_ارتفاع - extra_ارتفاع
	    && pum_win_ردیف - above_ردیف > (زیر_ردیف - above_ردیف) / 2)
    {
	// pum above "pum_win_row"
	إذا (State & نما_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطرها = 0;
	وإلا
	    // Leave two lines of context if possible
	    context_سطرها = MIN(2, curwin->w_wrow - curwin->w_cline_ردیف);

	إذا (pum_win_ردیف >= اندازه + context_سطرها + extra_ارتفاع)
	{
	    pum_ردیف = pum_win_ردیف - اندازه - context_سطرها - extra_جابه_جایی;
	    pum_ارتفاع = اندازه;
	}
	وإلا
	{
	    pum_ردیف = pum_border;
	    pum_ارتفاع = pum_win_ردیف - context_سطرها - extra_ارتفاع;
	}
	إذا (p_ph > 0 && pum_ارتفاع > p_ph)
	{
	    pum_ردیف += pum_ارتفاع - p_ph;
	    pum_ارتفاع = p_ph;
	}
    }
    وإلا
    {
	// pum below "pum_win_row"
	إذا (State & نما_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطرها = 0;
	وإلا
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_جابه_جایی = curwin->w_cline_ردیف +
		curwin->w_cline_ارتفاع - curwin->w_wrow;
	    context_سطرها = MIN(3, cline_visible_جابه_جایی);
	}

	pum_ردیف = pum_win_ردیف + context_سطرها + pum_border;
	pum_ارتفاع = MIN(زیر_ردیف - pum_ردیف - extra_جابه_جایی, اندازه);
	إذا (p_ph > 0 && pum_ارتفاع > p_ph)
	    pum_ارتفاع = p_ph;
    }

#إذا defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    إذا (above_ردیف > 0 && pum_ردیف < above_ردیف && pum_ارتفاع > above_ردیف)
    {
	pum_ردیف = above_ردیف;
	pum_ارتفاع = pum_win_ردیف - above_ردیف;
    }
#پایان_اگر
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ساكن صحيح
مجموعه_pum_عرض_aligned_with_cursor(صحيح عرض, صحيح موجود_عرض)
{
    صحيح انتها_padding = TRUE;

    إذا (عرض < p_pw)
    {
	عرض = p_pw;
	انتها_padding = FALSE;
    }
    إذا (p_pmw > 0 && عرض >= p_pmw)
    {
	عرض = p_pmw;
	انتها_padding = FALSE;
    }

    pum_عرض = عرض + (انتها_padding && عرض >= p_pw ? 1 : 0);
    أرجع موجود_عرض >= pum_عرض;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ساكن فراغ
pum_compute_horizontal_placement(صحيح cursor_ستون)
{
    صحيح desired_عرض = pum_پایه_عرض + pum_نوع_عرض + pum_extra_عرض;
    صحيح موجود_عرض;
    صحيح ستون_جابه_جایی;
    صحيح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	ستون_جابه_جایی = pum_border && (cursor_ستون >= Columns - 2)
	    ? (3 - Columns + cursor_ستون) : 0;
	موجود_عرض = cursor_ستون + 1;
	pum_ستون = cursor_ستون - ستون_جابه_جایی;
    }
    وإلا
#پایان_اگر
    {
	ستون_جابه_جایی = pum_border && (cursor_ستون <= 1) ? (2 - cursor_ستون) : 0;
	موجود_عرض = Columns - cursor_ستون;
	pum_ستون = cursor_ستون + ستون_جابه_جایی;
    }

    موجود_عرض -= trailing_overhead + ستون_جابه_جایی;

    // Case 1: Align pum with "cursor_col"
    إذا (مجموعه_pum_عرض_aligned_with_cursor(desired_عرض, موجود_عرض))
	أرجع;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    إذا (موجود_عرض > p_pw)
    {
	pum_عرض = موجود_عرض;
	أرجع;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    موجود_عرض = Columns - trailing_overhead - ستون_جابه_جایی;

    إذا (موجود_عرض > p_pw)
    {
	pum_عرض = p_pw;  // Truncate beyond 'pum_width'
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    pum_ستون = pum_عرض + trailing_overhead;
	وإلا
#پایان_اگر
	    pum_ستون = Columns - pum_عرض - trailing_overhead;
	أرجع;
    }

    // Case 3: Not enough room anywhere, use what we have
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
	pum_ستون = Columns - 1 - ستون_جابه_جایی;
    وإلا
#پایان_اگر
	pum_ستون = ستون_جابه_جایی;
    pum_عرض = Columns - trailing_overhead;
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
	صحيح	    اندازه,
	صحيح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحيح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحيح	    cursor_ستون;
    صحيح	    above_ردیف;
    صحيح	    زیر_ردیف;
    صحيح	    redo_شمار = 0;
#إذا defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#پایان_اگر

#اگر_تعریف_شده FEAT_RIGHTLEFT
    pum_rl = (State & نما_CMDLINE) == 0 && curwin->w_p_rl;
#پایان_اگر

    افعل
    {
	above_ردیف = 0;
	زیر_ردیف = cmdline_ردیف;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_ستون();
	pum_array = عدم;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_پنجره = curwin;
	إذا (State & نما_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_ردیف = cmdline_ردیف;
	وإلا
	    pum_win_ردیف = curwin->w_wrow + W_WINROW(curwin);
	pum_win_ارتفاع = curwin->w_ارتفاع;
	pum_win_ستون = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_عرض = curwin->w_عرض;

#إذا defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    إذا (pvwin->w_p_pvw)
		اكسر;
	إذا (pvwin != عدم)
	{
	    إذا (W_WINROW(pvwin) < W_WINROW(curwin))
		above_ردیف = W_WINROW(pvwin) + pvwin->w_ارتفاع;
	    وإلا إذا (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_ارتفاع)
		زیر_ردیف = W_WINROW(pvwin);
	}
#پایان_اگر

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(اندازه, above_ردیف, زیر_ردیف);

	// don't display when we only have room for one line
	إذا (pum_ارتفاع < 1 || (pum_ارتفاع == 1 && اندازه > 1))
	    أرجع;

	pum_array = array;
	pum_اندازه = اندازه;
	pum_compute_اندازه();

	// Calculate cursor column
	إذا (State & نما_CMDLINE)
	    // cmdline completion popup menu
	    cursor_ستون = cmdline_compl_startcol();
	وإلا
	{
	    صحيح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    إذا (curwin->w_عرض > 0)
		wcol %= curwin->w_عرض;
	    وإلا
		wcol = 0;
#اگر_تعریف_شده FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    إذا (curwin->w_p_cole > 0 && conceal_cursor_سطر(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		إذا (wcol < 0)
		    wcol = 0;
	    }
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    إذا (pum_rl)
		cursor_ستون = curwin->w_wincol + curwin->w_عرض - wcol - 1;
	    وإلا
#پایان_اگر
		cursor_ستون = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_ارتفاع < اندازه) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_ستون);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } طالما (pum_مجموعه_selected(selected, redo_شمار) && ++redo_شمار <= 2);

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
pum_under_menu(صحيح ردیف, صحيح ستون, صحيح only_redrawing)
{
    صحيح	extra_چپ = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحيح	extra_راست = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحيح	extra_above = pum_border;
    صحيح	extra_زیر = pum_border + (pum_shadow ? 1 : 0);
    صحيح	بالاترین = pum_ردیف - extra_above;
    صحيح	bot = pum_ردیف + pum_ارتفاع + extra_زیر;
    صحيح	چپ_2 = pum_ستون - 1 - extra_چپ;
    صحيح	راست_2 = pum_ستون + pum_عرض + pum_scrollbar + extra_راست;

    إذا (!((!only_redrawing || pum_will_redraw)
	    && ردیف >= بالاترین && ردیف < bot && ستون >= چپ_2 && ستون < راست_2))
	أرجع FALSE;

    إذا (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	إذا (only_redrawing)
	{
	    إذا (ستون >= راست_2 - 2 || ردیف == bot - 1)
		أرجع FALSE;
	}
	وإلا
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحيح	    راست_margin, چپ_margin, چپ_padding;

	    compute_margins(&راست_margin, &چپ_margin, &چپ_padding);
	    إذا (ردیف == بالاترین && ستون >= راست_2 - 2)
		أرجع FALSE;
	    إذا (ردیف == bot - 1 && ستون < pum_ستون + 2 - چپ_padding - pum_border
		    - چپ_margin)
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
pum_compute_متن_attrs(char_u *متن, hlf_T hlf, صحيح user_hlattr)
{
    صحيح		i;
    size_t	leader_len;
    صحيح		char_cells;
    صحيح		new_attr;
    char_u	*ptr = متن;
    صحيح		cell_idx = 0;
    garray_T	*ga = عدم;
    صحيح		*attrs = عدم;
    char_u	*leader = عدم;
    صحيح		درون_fuzzy;
    صحيح		matched_len = -1;
    int_u	char_pos = 0;
    صحيح		is_select = FALSE;

    إذا (*متن == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	أرجع عدم;

    is_select = hlf == HLF_PSI;
    leader = (State & نما_CMDLINE) ? cmdline_compl_الگو()
							  : ins_compl_leader();
    إذا (leader == عدم || *leader == NUL)
	أرجع عدم;

    attrs = ALLOC_MULT(صحيح, vim_strsize(متن));
    إذا (attrs == عدم)
	أرجع عدم;

    درون_fuzzy = (State & نما_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    إذا (درون_fuzzy)
    {
	ga = fuzzy_تطابق_str_with_pos(متن, leader);
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
		إذا (char_pos == ((int_u *)ga->ga_داده)[i])
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
    صحيح		ردیف,
    صحيح		ستون,
    صحيح		cells استفادهنشده,
    char_u	*متن,
    صحيح		textlen,
    صحيح		*attrs)
{
    صحيح		ستون_شروع = ستون;
    char_u	*ptr = متن;
    صحيح		char_len;
    صحيح		attr;

    // Render text with proper attributes
    طالما (*ptr != NUL && ptr < متن + textlen)
    {
	char_len = mb_ptr2len(ptr);
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    attr = attrs[ستون_شروع + cells - ستون - 1];
	وإلا
#پایان_اگر
	    attr = attrs[ستون - ستون_شروع];
	screen_puts_len(ptr, char_len, ردیف, ستون, attr);
	ستون += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ساكن مضمن char_u *
pum_get_قلم(صحيح شاخص, صحيح type)
{
    بدل(type)
    {
	حالة CPT_ABBR: أرجع pum_array[شاخص].pum_متن;
	حالة CPT_نوع: أرجع pum_array[شاخص].pum_نوع;
	حالة CPT_MENU: أرجع pum_array[شاخص].pum_extra;
    }
    أرجع عدم;
}

    ساكن مضمن صحيح
pum_user_attr_combine(صحيح idx, صحيح type, صحيح attr)
{
    صحيح user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_نوع_hlattr,
    };

    أرجع user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#اگر_تعریف_شده FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_rtl_متن(
	صحيح	ردیف,
	صحيح     ستون,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     عرض,
	صحيح     عرض_حد,
	صحيح     totwidth,
	صحيح     بعدی_isempty,
	صحيح	selected)
{
    char_u  *rt = عدم;
    صحيح     cells = 0;
    صحيح     over_cell = 0;
    صحيح     truncated = FALSE;
    صحيح     pad = بعدی_isempty ? 0 : 2;
    صحيح     مانده = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    إذا (st == عدم)
	أرجع ستون;

    rt = معکوس_متن(st);
    إذا (rt == عدم)
    {
	VIM_CLEAR(st);
	أرجع ستون;
    }

    char_u *rt_شروع = rt;
    cells = mb_string2cells(rt, -1);
    truncated = عرض_حد - totwidth < cells + pad;

    // only draw the text that fits
    إذا (cells > عرض_حد)
    {
	افعل
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} طالما (cells > عرض_حد);

	إذا (cells < عرض_حد)
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
	صحيح     اندازه = 0;

	مانده = عرض_حد - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	إذا (cells > مانده)
	{
	    طالما (cells > مانده)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	اندازه = (صحيح)STRLEN(orig_rt);
	إذا (cells < مانده)
	    over_cell = مانده - cells;

	cells = mb_string2cells(orig_rt, اندازه);
	عرض = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, ردیف, ستون - عرض + 1, trunc_attr);

	إذا (over_cell > 0)
	    screen_fill(ردیف, ردیف + 1, ستون - عرض + 2,
		    ستون - عرض + 2 + over_cell, ' ', ' ', attr);
    }

    إذا (attrs == عدم)
	screen_puts_len(rt, (صحيح)STRLEN(rt), ردیف, ستون - cells + 1, attr);
    وإلا
	pum_screen_puts_with_attrs(ردیف, ستون - cells + 1, cells, rt,
		(صحيح)STRLEN(rt), attrs);

    vim_free(rt_شروع);
    VIM_CLEAR(st);
    أرجع ستون - عرض;
}
#پایان_اگر

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_ltr_متن(
	صحيح     ردیف,
	صحيح     ستون,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     عرض,        // width already calculated in outer loop
	صحيح     عرض_حد,
	صحيح     totwidth,
	صحيح     بعدی_isempty,
	صحيح	selected)
{
    صحيح     اندازه = 0;
    صحيح     cells = 0;
    char_u  *st_انتها = عدم;
    صحيح     over_cell = 0;
    صحيح     pad = بعدی_isempty ? 0 : 2;
    صحيح     truncated = FALSE;
    صحيح     مانده = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    إذا (st == عدم)
	أرجع ستون;

    اندازه = (صحيح)STRLEN(st);
    cells = (*mb_string2cells)(st, اندازه);
    truncated = عرض_حد - totwidth < cells + pad;

    // only draw the text that fits
    طالما (اندازه > 0 && ستون + cells > عرض_حد + pum_ستون)
    {
	--اندازه;
	إذا (has_mbyte)
	{
	    اندازه -= (*mb_سر_off)(st, st + اندازه);
	    cells -= (*mb_ptr2cells)(st + اندازه);
	}
	وإلا
	    --cells;
    }

    // truncated
    إذا (truncated)
    {
	مانده = عرض_حد - totwidth - 1;
	إذا (cells > مانده)
	{
	    st_انتها = st + اندازه;
	    طالما (st_انتها > st && cells > مانده)
	    {
		MB_PTR_پشت(st, st_انتها);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_انتها) : 1;
	    }
	    اندازه = st_انتها - st;
	}

	إذا (cells < مانده)
	    over_cell = مانده - cells;
	cells = mb_string2cells(st, اندازه);
	عرض = cells + over_cell + 1;
    }

    إذا (attrs == عدم)
	screen_puts_len(st, اندازه, ردیف, ستون, attr);
    وإلا
	pum_screen_puts_with_attrs(ردیف, ستون, cells, st, اندازه, attrs);

    إذا (truncated)
    {
	إذا (over_cell > 0)
	    screen_fill(ردیف, ردیف + 1, ستون + cells,
		    ستون + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, ردیف, ستون + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    أرجع ستون + عرض;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ساكن صحيح
pum_process_قلم(
	صحيح     ردیف,
	صحيح     ستون,
	صحيح     idx,
	صحيح     j,         // Current position in order array
	صحيح     *ترتیب,    // Order array
	hlf_T   hlf,
	صحيح     attr,
	صحيح     *totwidth_ptr,
	صحيح     بعدی_isempty)
{
    صحيح     قلم_type = ترتیب[j];
    char_u  *s = عدم;
    char_u  *p = pum_get_قلم(idx, قلم_type);
    صحيح     عرض = 0;  // item width
    صحيح     w;		// char width
    صحيح	    selected = idx == pum_selected;

    لكل ( ; ; MB_PTR_ADV(p))
    {
	إذا (s == عدم)
	    s = p;
	w = ptr2cells(p);
	إذا (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_عرض)
	{
	    عرض += w;
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

	إذا (قلم_type == CPT_ABBR)
	    attrs = pum_compute_متن_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    ستون = pum_display_rtl_متن(ردیف, ستون, st, attr, attrs,
		    عرض, pum_عرض, *totwidth_ptr, بعدی_isempty, selected);
	وإلا
#پایان_اگر
	    ستون = pum_display_ltr_متن(ردیف, ستون, st, attr, attrs,
		    عرض, pum_عرض, *totwidth_ptr, بعدی_isempty, selected);

	إذا (attrs != عدم)
	    VIM_CLEAR(attrs);

	إذا (*p != TAB)
	    اكسر;

	// Display two spaces for a Tab.
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, ردیف, ستون - 1, attr);
	    ستون -= 2;
	}
	وإلا
#پایان_اگر
	{
	    screen_puts_len((char_u *)"  ", 2, ردیف, ستون, attr);
	    ستون += 2;
	}
	*totwidth_ptr += 2;
	s = عدم;  // start text at next char
	عرض = 0;
    }

    أرجع ستون;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ساكن فراغ
pum_draw_scrollbar(
	صحيح	ردیف,
	صحيح	i,
	صحيح	thumb_pos,
	صحيح	thumb_ارتفاع)
{
    إذا (pum_scrollbar <= 0)
	أرجع;

    صحيح attr = (i >= thumb_pos && i < thumb_pos + thumb_ارتفاع) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
	screen_putchar(' ', ردیف, pum_ستون - pum_عرض, attr);
    وإلا
#پایان_اگر
	screen_putchar(' ', ردیف, pum_ستون + pum_عرض, attr);
}

    ساكن مضمن فراغ
pum_align_ترتیب(صحيح *ترتیب)
{
    صحيح is_default = cia_flags == 0;
    ترتیب[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ترتیب[1] = is_default ? CPT_نوع : (cia_flags / 10) % 10;
    ترتیب[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ساكن فراغ pum_free_bg(فراغ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    فراغ
pum_opacity_تغییریافته(فراغ)
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
    VIM_CLEAR(pum_bg_سطرها);
    VIM_CLEAR(pum_bg_سطرها_UC);
    لكل (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_سطرها_C[k]);
    pum_bg_بالاترین = 0;
    pum_bg_bot = 0;
    pum_bg_تعداد_ستون = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    فراغ
pum_redraw(فراغ)
{
    صحيح		ردیف = pum_ردیف;
    صحيح		ستون;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحيح		attr;
    صحيح		i, j;
    صحيح		idx;
    char_u	*p = عدم;
    صحيح		totwidth;
    صحيح		thumb_pos = 0;
    صحيح		thumb_ارتفاع = 1;
    صحيح		قلم_type;
    صحيح		ترتیب[3];
    صحيح		بعدی_isempty = FALSE;
    صحيح		n;
    صحيح		اقلام_عرض_array[3] = { pum_پایه_عرض, pum_نوع_عرض,
							    pum_extra_عرض };
    صحيح		basic_عرض;  // first item width
    صحيح		آخرین_isabbr = FALSE;
    صحيح		orig_attr = -1;
    صحيح		scroll_range = pum_اندازه - pum_ارتفاع;
    bool	override_success;
    صحيح		opacity_فعال = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = افزودن_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_ENABLE);

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    إذا (pum_imagep == عدم)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(عدم, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    إذا (pum_imagep != عدم)
    {
	image_placement_مجموعه_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_مجموعه_position(pum_imagep, pum_ردیف, pum_ستون);
	image_placement_مجموعه_bounding_box(pum_imagep, pum_ردیف, pum_ستون,
		pum_ارتفاع, pum_عرض);
	image_placement_مجموعه_enable(pum_imagep, true);
    }
#پایان_اگر

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
	إذا (opacity_فعال && pum_bg_سطرها != عدم
		&& (pum_bg_تعداد_ستون != screen_Columns
		    || pum_bg_bot > screen_ردیفها))
	    pum_free_bg();

	إذا (opacity_فعال && pum_bg_سطرها != عدم)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	وإلا إذا (opacity_فعال)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    إذا (Screen_سطرها != عدم && ScreenAttrs != عدم)
	    {
		صحيح	save_بالاترین, save_bot, save_ncells, k;

		pum_free_bg();
		save_بالاترین = pum_ردیف - pum_border;
		save_bot = pum_ردیف + pum_ارتفاع + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		إذا (save_بالاترین < 0)
		    save_بالاترین = 0;
		إذا (save_bot > screen_ردیفها)
		    save_bot = screen_ردیفها;
		pum_bg_بالاترین = save_بالاترین;
		pum_bg_bot = save_bot;
		pum_bg_تعداد_ستون = screen_Columns;
		إذا (save_بالاترین < save_bot)
		{
		    save_ncells = (save_bot - save_بالاترین) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_سطرها = LALLOC_MULT(schar_T, save_ncells);
		    إذا (enc_utf8)
		    {
			pum_bg_سطرها_UC = LALLOC_MULT(u8char_T, save_ncells);
			لكل (k = 0; k < MAX_MCO; ++k)
			    pum_bg_سطرها_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    إذا (pum_bg_attrs != عدم && pum_bg_سطرها != عدم)
		    {
			لكل (صحيح r = save_بالاترین; r < save_bot; ++r)
			{
			    صحيح soff = (r - save_بالاترین) * screen_Columns;
			    صحيح loff = سطر_جابه_جایی[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * حجم(sattr_T));
			    mch_memmove(pum_bg_سطرها + soff,
				    Screen_سطرها + loff,
				    screen_Columns * حجم(schar_T));
			    إذا (enc_utf8 && pum_bg_سطرها_UC != عدم
						&& Screen_سطرها_UC != عدم)
			    {
				mch_memmove(pum_bg_سطرها_UC + soff,
					Screen_سطرها_UC + loff,
					screen_Columns * حجم(u8char_T));
				لكل (k = 0; k < MAX_MCO; ++k)
				    إذا (pum_bg_سطرها_C[k] != عدم
						    && Screen_سطرها_C[k] != عدم)
					mch_memmove(pum_bg_سطرها_C[k] + soff,
						Screen_سطرها_C[k] + loff,
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
    pum_اولی = MIN(pum_اولی, scroll_range);

    إذا (pum_scrollbar)
    {
	thumb_ارتفاع = pum_ارتفاع * pum_ارتفاع / pum_اندازه;
	إذا (thumb_ارتفاع == 0)
	    thumb_ارتفاع = 1;
	thumb_pos = (pum_اولی * (pum_ارتفاع - thumb_ارتفاع)
			    + scroll_range / 2) / scroll_range;
    }

#اگر_تعریف_شده FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#پایان_اگر

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    إذا (pum_border)
	pum_draw_border();
    إذا (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    إذا (opacity_فعال)
	screen_pum_blend = 100 - (صحيح)p_po;

    لكل (i = 0; i < pum_ارتفاع; ++i)
    {
	idx = i + pum_اولی;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    إذا (pum_ستون < curwin->w_wincol + curwin->w_عرض - 1
							    - pum_border)
		screen_putchar(' ', ردیف, pum_ستون + 1, attr);
	}
	وإلا
#پایان_اگر
	    إذا (pum_ستون > pum_border)
		screen_putchar(' ', ردیف, pum_ستون - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	ستون = pum_ستون;
	totwidth = 0;
	pum_align_ترتیب(ترتیب);
	basic_عرض = اقلام_عرض_array[ترتیب[0]];
	آخرین_isabbr = ترتیب[2] == CPT_ABBR;
	لكل (j = 0; j < 3; ++j)
	{
	    قلم_type = ترتیب[j];
	    hlf = hlfs[قلم_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    إذا (قلم_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, قلم_type, attr);
	    p = pum_get_قلم(idx, قلم_type);

	    إذا (j + 1 < 3)
		بعدی_isempty = pum_get_قلم(idx, ترتیب[j + 1]) == عدم;
	    وإلا
		بعدی_isempty = TRUE;

	    إذا (p != عدم)
		// Process and display the item
		ستون = pum_process_قلم(ردیف, ستون, idx, j, ترتیب, hlf, attr,
						    &totwidth, بعدی_isempty);

	    إذا (j > 0)
		n = اقلام_عرض_array[ترتیب[1]] + (آخرین_isabbr ? 0 : 1);
	    وإلا
		n = ترتیب[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    إذا (j == 2
		    || (بعدی_isempty && (j == 1 || (j == 0
				&& pum_get_قلم(idx, ترتیب[j + 2]) == عدم)))
		    || basic_عرض + n >= pum_عرض)
		اكسر;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    إذا (pum_rl)
	    {
		screen_fill(ردیف, ردیف + 1, pum_ستون - basic_عرض - n + 1,
						ستون + 1, ' ', ' ', orig_attr);
		ستون = pum_ستون - basic_عرض - n;
	    }
	    وإلا
#پایان_اگر
	    {
		screen_fill(ردیف, ردیف + 1, ستون, pum_ستون + basic_عرض + n,
							' ', ' ', orig_attr);
		ستون = pum_ستون + basic_عرض + n;
	    }
	    totwidth = basic_عرض + n;
	}

#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    screen_fill(ردیف, ردیف + 1, pum_ستون - pum_عرض + 1, ستون + 1,
							    ' ', ' ', orig_attr);
	وإلا
#پایان_اگر
	    screen_fill(ردیف, ردیف + 1, ستون, pum_ستون + pum_عرض, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(ردیف, i, thumb_pos, thumb_ارتفاع);

	++ردیف;
    }

    screen_pum_blend = 0;

#اگر_تعریف_شده FEAT_PROP_POPUP
    screen_zindex = 0;
#پایان_اگر

    إذا (override_success)
	برداشت_highlight_overrides();

    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_DISABLE);
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    فراغ
pum_position_info_popup(win_T *wp)
{
    صحيح ستون = pum_ستون + pum_عرض + pum_scrollbar + 1;
    صحيح ردیف = pum_ردیف;
    صحيح botpos = POPPOS_BOTLEFT;
    صحيح	استفاده_شده_maxwidth_opt = FALSE;
    صحيح	راست_margin, چپ_margin, dummy;
    صحيح چپ_extra_عرض, راست_extra_عرض;

    (فراغ)compute_margins(&راست_margin, &چپ_margin, &dummy);

    چپ_extra_عرض = pum_border + چپ_margin;
    راست_extra_عرض = pum_border + راست_margin + (pum_shadow ? 2 : 0);
    ستون += راست_extra_عرض;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    إذا (Columns - ستون < 20 && Columns - ستون < pum_ستون)
    {
	ستون = pum_ستون - 1 - چپ_extra_عرض;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_ستون - 1 - چپ_extra_عرض;
    }
    وإلا
	wp->w_maxwidth = Columns - ستون + 1;
    wp->w_maxwidth -= popup_extra_عرض(wp);
    إذا (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	استفاده_شده_maxwidth_opt = TRUE;
    }

    ردیف -= popup_بالاترین_extra(wp);
    إذا (wp->w_popup_flags & POPF_INFO_MENU)
    {
	إذا (pum_ردیف < pum_win_ردیف)
	{
	    // menu above cursor line, align with bottom
	    ردیف += pum_ارتفاع;
	    wp->w_popup_pos = botpos;
	}
	وإلا
	    // menu below cursor line, align with top
	    ردیف += 1;
    }
    وإلا
	// align with the selected item
	ردیف += pum_selected - pum_اولی + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (wp->w_maxwidth < 10 && !استفاده_شده_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    وإلا
	popup_مجموعه_wantpos_rowcol(wp, ردیف, ستون);
}
#پایان_اگر

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
pum_مجموعه_selected(صحيح n, صحيح repeat استفادهنشده)
{
    صحيح	    resized = FALSE;
    صحيح	    context = pum_ارتفاع / 2;
    صحيح	    scroll_جابه_جایی;
#اگر_تعریف_شده FEAT_QUICKFIX
    صحيح	    prev_selected = pum_selected;
    بیعلامت	cur_cot_flags = get_cot_flags();
#پایان_اگر
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحيح	    has_info = FALSE;
#پایان_اگر

    pum_selected = n;
    scroll_جابه_جایی = pum_selected - pum_ارتفاع;

    إذا (pum_selected >= 0 && pum_selected < pum_اندازه)
    {
	إذا (pum_اولی > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    إذا (pum_اولی > pum_selected - 2)
	    {
		pum_اولی -= pum_ارتفاع - 2;
		إذا (pum_اولی < 0)
		    pum_اولی = 0;
		وإلا إذا (pum_اولی > pum_selected)
		    pum_اولی = pum_selected;
	    }
	    وإلا
		pum_اولی = pum_selected;
	}
	وإلا إذا (pum_اولی < scroll_جابه_جایی + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    إذا (pum_اولی < scroll_جابه_جایی + 3)
		pum_اولی = MAX(pum_اولی + pum_ارتفاع - 2, scroll_جابه_جایی + 1);
	    وإلا
		pum_اولی = scroll_جابه_جایی + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	إذا (pum_ارتفاع > 2)
	{
	    إذا (pum_اولی > pum_selected - context)
		pum_اولی = MAX(pum_selected - context, 0);  // scroll down
	    وإلا إذا (pum_اولی < pum_selected + context - pum_ارتفاع + 1)
		pum_اولی = pum_selected + context - pum_ارتفاع + 1;  //به_بالا
	}
	// adjust for the number of lines displayed
	pum_اولی = MIN(pum_اولی, pum_اندازه - pum_ارتفاع);

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
		&& ردیفها > 10
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
		use_popup = USEPOPUP_نرمال;
	    وإلا
		use_popup = USEPOPUP_NONE;
	    إذا (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		بلوک_autocmds();
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
			مجموعه_option_مقدار_give_err((char_u *)"swf",
							  0L, عدم, OPT_LOCAL);
			مجموعه_option_مقدار_give_err((char_u *)"bl",
							  0L, عدم, OPT_LOCAL);
			مجموعه_option_مقدار_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			مجموعه_option_مقدار_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			مجموعه_option_مقدار_give_err((char_u *)"diff",
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
		    ml_delete(curbuf->b_ml.ml_سطر_شمار);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    إذا (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			إذا (curwin->w_ارتفاع < lnum)
			{
			    win_setheight((صحيح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_تغییریافته = 0;
		    curbuf->b_p_ma = FALSE;
		    إذا (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    وإلا إذا (curwin->w_topline > curbuf->b_ml.ml_سطر_شمار)
			curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.ستون = 0;
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
			صحيح save_redr_وضعیت;

			إذا (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			إذا (ins_compl_فعال() && !resized)
			    curwin->w_redr_وضعیت = false;

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
			save_redr_وضعیت = curwin_save->w_redr_وضعیت;
			curwin_save->w_redr_وضعیت = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_وضعیت = save_redr_وضعیت;

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
#پایان_اگر
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    إذا (!has_info)
	// hide any popup info window
	popup_hide_info();
#پایان_اگر

    أرجع resized;
}

/*
 * Undisplay the popup menu (later).
 */
    فراغ
pum_undisplay(فراغ)
{
#اگر_تعریف_شده FEAT_IMAGE_POPUP
    إذا (pum_imagep != عدم)
    {
	image_placement_free(pum_imagep);
	pum_imagep = عدم;
    }
#پایان_اگر
    pum_free_bg();
    pum_array = عدم;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
    إذا (pum_درون_cmdline)
    {
	clear_cmdline = TRUE;
	pum_درون_cmdline = FALSE;
    }
    وضعیت_redraw_all();
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#پایان_اگر
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    فراغ
pum_clear(فراغ)
{
    pum_اولی = 0;
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
pum_درون_same_position(فراغ)
{
    صحيح	    ردیف = (State & نما_CMDLINE)
			? cmdline_ردیف
			: curwin->w_wrow + W_WINROW(curwin);

    أرجع pum_پنجره != curwin
	    || (pum_win_ردیف == ردیف
		&& pum_win_ارتفاع == curwin->w_ارتفاع
		&& pum_win_ستون == curwin->w_wincol
		&& pum_win_عرض == curwin->w_عرض);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحيح
pum_redraw_درون_same_position(فراغ)
{
    إذا (!pum_visible() || pum_will_redraw)
	أرجع FALSE;  // nothing to do

    أرجع pum_درون_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    فراغ
pum_may_redraw(فراغ)
{
    pumitem_T	*array = pum_array;
    صحيح		len = pum_اندازه;
    صحيح		selected = pum_selected;

    إذا (!pum_visible() || pum_will_redraw)
	أرجع;  // nothing to do

    إذا (pum_درون_same_position())
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
pum_get_ارتفاع(فراغ)
{
    أرجع pum_ارتفاع;
}

#إذا defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    فراغ
pum_مجموعه_event_info(dict_T *dict)
{
    إذا (!pum_visible())
	أرجع;
    (فراغ)dict_جمع_کن_عدد(dict, "height", pum_ارتفاع);
    (فراغ)dict_جمع_کن_عدد(dict, "width", pum_عرض);
    (فراغ)dict_جمع_کن_عدد(dict, "row", pum_ردیف);
    (فراغ)dict_جمع_کن_عدد(dict, "col", pum_ستون);
    (فراغ)dict_جمع_کن_عدد(dict, "size", pum_اندازه);
    (فراغ)dict_جمع_کن_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#پایان_اگر

#إذا defined(FEAT_BEVAL_جمله) || defined(FEAT_جمله_POPUP_MENU)
    ساكن فراغ
pum_position_at_mouse(صحيح min_عرض)
{
    صحيح extra_ارتفاع = pum_border * 2 + pum_shadow;
    صحيح extra_جابه_جایی = pum_border + pum_shadow;
    صحيح trailing_overhead = 0;
    صحيح ستون_جابه_جایی = 0;
    صحيح موجود_عرض;

    إذا (ردیفها - mouse_ردیف - extra_ارتفاع > pum_اندازه
	    || ردیفها - mouse_ردیف > mouse_ردیف)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_ردیف = mouse_ردیف + 1 + pum_border;
	إذا (pum_ارتفاع > ردیفها - pum_ردیف - extra_ارتفاع)
	    pum_ارتفاع = ردیفها - pum_ردیف - extra_ارتفاع;
	إذا (pum_ردیف + pum_ارتفاع + extra_جابه_جایی >= cmdline_ردیف)
	    pum_درون_cmdline = TRUE;
    }
    وإلا
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_ردیف = mouse_ردیف - pum_اندازه - extra_جابه_جایی;
	إذا (pum_ردیف < pum_border)
	{
	    pum_ارتفاع += pum_ردیف - pum_border;
	    pum_ردیف = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	ستون_جابه_جایی = (mouse_ستون == Columns - 1) && pum_border;
	موجود_عرض = mouse_ستون + 1 - trailing_overhead - ستون_جابه_جایی;
	إذا (موجود_عرض >= pum_پایه_عرض || موجود_عرض > min_عرض)
	    // Enough space to show at mouse column.
	    pum_ستون = mouse_ستون - ستون_جابه_جایی;
	وإلا
	    // Not enough space, left align with window.
	    pum_ستون = MIN(pum_پایه_عرض, min_عرض) - 1 + trailing_overhead;
	pum_عرض = pum_ستون + 1 - trailing_overhead;
    }
    وإلا
# endif
    {
	ستون_جابه_جایی = (mouse_ستون == 0) && pum_border;
	موجود_عرض = Columns - mouse_ستون - trailing_overhead - ستون_جابه_جایی;
	إذا (موجود_عرض >= pum_پایه_عرض || موجود_عرض > min_عرض)
	    // Enough space to show at mouse column.
	    pum_ستون = mouse_ستون + ستون_جابه_جایی;
	وإلا
	    // Not enough space, right align with window.
	    pum_ستون = Columns - MIN(pum_پایه_عرض, min_عرض)
		- trailing_overhead;
	pum_عرض = Columns - pum_ستون - trailing_overhead;
    }

    pum_عرض = MIN(pum_عرض, pum_پایه_عرض + 1);
    // Do not redraw at cursor position.
    pum_پنجره = عدم;
}

#پایان_اگر

#إذا defined(FEAT_BEVAL_جمله)
ساكن pumitem_T *balloon_array = عدم;
ساكن صحيح balloon_arraysize;

# define BALLOON_MIN_عرض 50
# define BALLOON_MIN_ارتفاع 10

تعريفنوع بنية {
    char_u	*شروع;
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
تقسیم_پیام(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*قلم;
    صحيح		quoted = FALSE;
    صحيح		ارتفاع;
    صحيح		سطر;
    صحيح		قلم_idx;
    صحيح		indent = 0;
    صحيح		max_cells = 0;
    صحيح		max_ارتفاع_2 = ردیفها / 2 - 1;
    صحيح		long_قلم_شمار = 0;
    صحيح		تقسیم_long_اقلام = FALSE;

    ga_init2(&ga, حجم(balpart_T), 20);
    p = mesg;

    طالما (*p != NUL)
    {
	إذا (ga_grow(&ga, 1) == FAIL)
	    اذهب failed;
	قلم = ((balpart_T *)ga.ga_داده) + ga.ga_len;
	قلم->شروع = p;
	قلم->indent = indent;
	قلم->cells = indent * 2;
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
		    ++قلم->cells;
		    p = skipwhite(p + 1);
		    اكسر;
		}
	    }
	    قلم->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	قلم->bytelen = p - قلم->شروع;
	إذا (*p == '\n')
	    ++p;
	إذا (قلم->cells > max_cells)
	    max_cells = قلم->cells;
	long_قلم_شمار += (قلم->cells - 1) / BALLOON_MIN_عرض;
    }

    ارتفاع = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    إذا (long_قلم_شمار > 0 && ارتفاع + long_قلم_شمار <= max_ارتفاع_2)
    {
	تقسیم_long_اقلام = TRUE;
	ارتفاع += long_قلم_شمار;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    إذا (ارتفاع > max_ارتفاع_2)
	ارتفاع = max_ارتفاع_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, ارتفاع);
    إذا (*array == عدم)
	اذهب failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_متن = vim_strsave((char_u *)"");
    (*array + ارتفاع - 1)->pum_متن = vim_strsave((char_u *)"");

    لكل (سطر = 1, قلم_idx = 0; سطر < ارتفاع - 1; ++قلم_idx)
    {
	صحيح	skip;
	صحيح	thislen;
	صحيح	copylen;
	صحيح	ind;
	صحيح	cells;

	قلم = ((balpart_T *)ga.ga_داده) + قلم_idx;
	إذا (قلم->bytelen == 0)
	    (*array)[سطر++].pum_متن = vim_strsave((char_u *)"");
	وإلا
	    لكل (skip = 0; skip < قلم->bytelen; skip += thislen)
	    {
		إذا (تقسیم_long_اقلام && قلم->cells >= BALLOON_MIN_عرض)
		{
		    cells = قلم->indent * 2;
		    لكل (p = قلم->شروع + skip;
			    p < قلم->شروع + قلم->bytelen;
							    p += mb_ptr2len(p))
			إذا ((cells += ptr2cells(p)) > BALLOON_MIN_عرض)
			    اكسر;
		    thislen = p - (قلم->شروع + skip);
		}
		وإلا
		    thislen = قلم->bytelen;

		// put indent at the start
		p = alloc(thislen + قلم->indent * 2 + 1);
		إذا (p == عدم)
		{
		    لكل (سطر = 0; سطر <= ارتفاع - 1; ++سطر)
			vim_free((*array)[سطر].pum_متن);
		    vim_free(*array);
		    اذهب failed;
		}
		لكل (ind = 0; ind < قلم->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		لكل (copylen = thislen; copylen > 0; --copylen)
		    إذا (قلم->شروع[skip + copylen - 1] != ' ')
			اكسر;

		vim_strncpy(p + ind, قلم->شروع + skip, copylen);
		(*array)[سطر].pum_متن = p;
		قلم->indent = 0;  // wrapped line has no indent
		++سطر;
	    }
    }
    ga_clear(&ga);
    أرجع ارتفاع;

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
	vim_free(balloon_array[--balloon_arraysize].pum_متن);
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
	لكل (idx = 0, li = list->lv_اولی; li != عدم; li = li->li_بعدی, ++idx)
	{
	    char_u *متن = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_متن = vim_strsave(
					   متن == عدم ? (char_u *)"" : متن);
	}
    }
    وإلا
	balloon_arraysize = تقسیم_پیام(mesg, &balloon_array);

    إذا (balloon_arraysize <= 0)
	أرجع;

    pum_array = balloon_array;
    pum_اندازه = balloon_arraysize;
    pum_compute_اندازه();
    pum_scrollbar = 0;
    pum_ارتفاع = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_عرض);
    pum_selected = -1;
    pum_اولی = 0;
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
#پایان_اگر

#إذا defined(FEAT_جمله_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ساكن فراغ
pum_select_mouse_pos(فراغ)
{
    صحيح idx = mouse_ردیف - pum_ردیف;

    إذا (idx < 0 || idx >= pum_ارتفاع)
	pum_selected = -1;
    وإلا إذا (*pum_array[idx].pum_متن != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ساكن فراغ
pum_execute_menu(vimmenu_T *menu, صحيح نما)
{
    vimmenu_T   *mp;
    صحيح		idx = 0;
    exarg_T	ea;

    FOR_ALL_فرزند_MENUS(menu, mp)
	إذا ((mp->modes & mp->enabled & نما) && idx++ == pum_selected)
	{
	    CLEAR_فیلد(ea);
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
# ifdef FEAT_BEVAL_جمله
    صحيح		save_bevalterm = p_bevalterm;
# endif
    صحيح		نما;

    pum_undisplay();
    pum_اندازه = 0;
    نما = get_menu_نما_flag();

    FOR_ALL_فرزند_MENUS(menu, mp)
	إذا (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & نما))
	    ++pum_اندازه;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    إذا (pum_اندازه <= 0)
    {
	emsg(_(e_menu_only_exists_درون_another_نما));
	أرجع;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_اندازه);
    إذا (array == عدم)
	أرجع;

    FOR_ALL_فرزند_MENUS(menu, mp)
    {
	char_u *s = عدم;

	// Make a copy of the text, the menu may be redefined in a callback.
	إذا (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	وإلا إذا (mp->modes & mp->enabled & نما)
	    s = mp->dname;
	إذا (s != عدم)
	{
	    s = vim_strsave(s);
	    إذا (s != عدم)
		array[idx++].pum_متن = s;
	}
    }

    pum_array = array;
    pum_compute_اندازه();
    pum_scrollbar = 0;
    pum_ارتفاع = pum_اندازه;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_اولی = 0;
# ifdef FEAT_BEVAL_جمله
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    لكل (;;)
    {
	صحيح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	بیرون_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	إذا (c == ESC || c == Ctrl_C || pum_array == عدم)
	    اكسر;
	وإلا إذا (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, نما);
	    اكسر;
	}
	وإلا إذا (c == 'k' || c == K_به_بالا || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    طالما (pum_selected > 0)
	    {
		--pum_selected;
		إذا (*array[pum_selected].pum_متن != NUL)
		    اكسر;
	    }
	}
	وإلا إذا (c == 'j' || c == K_پایین || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    طالما (pum_selected < pum_اندازه - 1)
	    {
		++pum_selected;
		إذا (*array[pum_selected].pum_متن != NUL)
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
		pum_execute_menu(menu, نما);
		اكسر;
	    }
	    إذا (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		اكسر;
	}
    }

    لكل (idx = 0; idx < pum_اندازه; ++idx)
	vim_free(array[idx].pum_متن);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_جمله
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    فراغ
pum_make_popup(char_u *مسیر_name, صحيح use_mouse_pos)
{
    vimmenu_T *menu;

    إذا (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_ردیف = W_WINROW(curwin) + curwin->w_wrow;
	mouse_ستون =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_پیدا_کن_menu(مسیر_name);
    إذا (menu != عدم)
	pum_show_popupmenu(menu);
}
#پایان_اگر

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
pum_مجموعه_border_chars(
	صحيح بالاترین,
	صحيح راست_2,
	صحيح bottom,
	صحيح چپ_2,
	صحيح بالاترین_چپ,
	صحيح بالاترین_راست,
	صحيح bottom_راست,
	صحيح bottom_چپ)
{
    إذا (!is_singlewidth(بالاترین) || !is_singlewidth(راست_2)
	    || !is_singlewidth(bottom) || !is_singlewidth(چپ_2)
	    || !is_singlewidth(بالاترین_چپ) || !is_singlewidth(بالاترین_راست)
	    || !is_singlewidth(bottom_راست) || !is_singlewidth(bottom_چپ))
	أرجع;
    pum_border_chars.بالاترین = بالاترین;
    pum_border_chars.راست_2 = راست_2;
    pum_border_chars.bottom = bottom;
    pum_border_chars.چپ_2 = چپ_2;
    pum_border_chars.بالاترین_چپ = بالاترین_چپ;
    pum_border_chars.بالاترین_راست = بالاترین_راست;
    pum_border_chars.bottom_راست = bottom_راست;
    pum_border_chars.bottom_چپ = bottom_چپ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ساكن فراغ
compute_margins(صحيح *راست_margin, صحيح *چپ_margin, صحيح *چپ_padding)
{
    *راست_margin = pum_margin && pum_border;

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	*چپ_padding = (pum_ستون < Columns - 1 - pum_border);
	*چپ_margin = *راست_margin && (pum_ستون < Columns - 3);
	أرجع;
    }
#پایان_اگر
    *چپ_padding = (pum_ستون > pum_border);
    *چپ_margin = *راست_margin && (pum_ستون > 2);
}

/*
 * Draw the border around the popup menu
 */
    ساكن فراغ
pum_draw_border(فراغ)
{
    صحيح	بالاترین_ردیف = pum_ردیف - 1;
    صحيح	bottom_ردیف = pum_ردیف + pum_ارتفاع;
    صحيح	ستون;
    صحيح	attr = highlight_attr[HLF_PMB];
    صحيح	راست_margin, چپ_margin, چپ_padding;
    صحيح	inner_عرض;

    إذا (!pum_border)
	أرجع;

    (فراغ)compute_margins(&راست_margin, &چپ_margin, &چپ_padding);

    inner_عرض = pum_عرض + pum_scrollbar + چپ_padding;

    // Draw border left to right or right to left (for RTL)
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
	ستون = pum_ستون + چپ_padding + 1;
    وإلا
#پایان_اگر
	ستون = pum_ستون - چپ_padding - 1;

    // left corners (right corners for RTL)
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.بالاترین_راست, بالاترین_ردیف, ستون, attr);
	إذا (چپ_margin)
	    screen_putchar(' ', بالاترین_ردیف, ستون + 1, attr);
	screen_putchar(pum_border_chars.bottom_راست, bottom_ردیف, ستون, attr);
	إذا (چپ_margin)
	    screen_putchar(' ', bottom_ردیف, ستون + 1, attr);
    }
    وإلا
#پایان_اگر
    {
	screen_putchar(pum_border_chars.بالاترین_چپ, بالاترین_ردیف, ستون, attr);
	إذا (چپ_margin)
	    screen_putchar(' ', بالاترین_ردیف, ستون - 1, attr);
	screen_putchar(pum_border_chars.bottom_چپ, bottom_ردیف, ستون, attr);
	إذا (چپ_margin)
	    screen_putchar(' ', bottom_ردیف, ستون - 1, attr);
    }

    // Top horizontal lines
    لكل (صحيح i = 0; i < inner_عرض; i++)
    {
	صحيح cur_ستون;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    cur_ستون = ستون - 1 - i;
	وإلا
#پایان_اگر
	    cur_ستون = ستون + 1 + i;
	screen_putchar(pum_border_chars.بالاترین, بالاترین_ردیف, cur_ستون, attr);
	screen_putchar(pum_border_chars.bottom, bottom_ردیف, cur_ستون, attr);
    }

    // right corners (left corners for RTL)
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.بالاترین_چپ, بالاترین_ردیف,
		ستون - inner_عرض - 1, attr);
	إذا (راست_margin)
	    screen_putchar(' ', بالاترین_ردیف, ستون - inner_عرض - 2, attr);
	screen_putchar(pum_border_chars.bottom_چپ, bottom_ردیف,
		ستون - inner_عرض - 1, attr);
	إذا (راست_margin)
	    screen_putchar(' ', bottom_ردیف, ستون - inner_عرض - 2, attr);
    }
    وإلا
#پایان_اگر
    {
	screen_putchar(pum_border_chars.بالاترین_راست, بالاترین_ردیف,
		ستون + inner_عرض + 1, attr);
	إذا (راست_margin)
	    screen_putchar(' ', بالاترین_ردیف, ستون + inner_عرض + 2, attr);
	screen_putchar(pum_border_chars.bottom_راست, bottom_ردیف,
		ستون + inner_عرض + 1, attr);
	إذا (راست_margin)
	    screen_putchar(' ', bottom_ردیف, ستون + inner_عرض + 2, attr);
    }

    // Draw side borders
    لكل (صحيح i = 0; i < pum_ارتفاع; i++)
    {
	صحيح ردیف = pum_ردیف + i;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_putchar(pum_border_chars.چپ_2, ردیف, ستون, attr);
	    إذا (چپ_margin)
		screen_putchar(' ', ردیف, ستون + 1, attr);
	    screen_putchar(pum_border_chars.راست_2, ردیف,
		    ستون - inner_عرض - 1, attr);
	    إذا (راست_margin)
		screen_putchar(' ', ردیف, ستون - inner_عرض - 2, attr);
	}
	وإلا
#پایان_اگر
	{
	    screen_putchar(pum_border_chars.چپ_2, ردیف, ستون, attr);
	    إذا (چپ_margin)
		screen_putchar(' ', ردیف, ستون - 1, attr);
	    screen_putchar(pum_border_chars.راست_2, ردیف,
		    ستون + inner_عرض + 1, attr);
	    إذا (راست_margin)
		screen_putchar(' ', ردیف, ستون + inner_عرض + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    فراغ
put_shadow_char(صحيح ردیف, صحيح ستون)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحيح	    shadow_attr = highlight_attr[HLF_PMS];
    صحيح	    char_attr, new_attr;

    screen_getbytes(ردیف, ستون, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), ردیف, ستون, new_attr);
}

/*
 * Draw the shadow
 */
    ساكن فراغ
pum_draw_shadow(فراغ)
{
    صحيح	ردیف, ستون, بعدی_ستون;
    صحيح	عرض, جابه_جایی;
    صحيح	inner_عرض;
    صحيح	راست_margin, چپ_margin, چپ_padding;

    إذا (!pum_shadow)
	أرجع;

    (فراغ)compute_margins(&راست_margin, &چپ_margin, &چپ_padding);
    inner_عرض = pum_عرض + pum_scrollbar + pum_border + راست_margin;

    // Draw right side shadow (left for RTL)
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	ستون = pum_ستون - inner_عرض;
	بعدی_ستون = ستون - 1;
    }
    وإلا
#پایان_اگر
    {
	ستون = pum_ستون + inner_عرض;
	بعدی_ستون = ستون + 1;
    }

    لكل (صحيح i = 0; i < pum_ارتفاع + pum_border; i++)
    {
	ردیف = pum_ردیف + 1 + i - pum_border;
	put_shadow_char(ردیف, ستون);
	put_shadow_char(ردیف, بعدی_ستون);
    }

    // Bottom shadow
    ردیف = pum_ردیف + pum_ارتفاع + pum_border;
    عرض = inner_عرض + چپ_padding + pum_border + چپ_margin;
    جابه_جایی = 2 - چپ_padding - pum_border - چپ_margin;

    لكل (صحيح i = 0; i < عرض; i++)
    {
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (pum_rl)
	    ستون = pum_ستون - جابه_جایی - i;
	وإلا
#پایان_اگر
	    ستون = pum_ستون + جابه_جایی + i;
	put_shadow_char(ردیف, ستون);
    }
}
