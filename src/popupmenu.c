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
ساكن صحيح pum_سائيز;			// nr of items in "pum_array"
ساكن صحيح pum_selected;		// index of selected item or -1
ساكن صحيح pum_پهريون = 0;		// index of top item

ساكن صحيح call_update_screen = FALSE;
ساكن صحيح pum_اندريون_cmdline = FALSE;

ساكن صحيح pum_اوچائي;			// nr of displayed pum items
ساكن صحيح pum_ويڪر;			// width of displayed pum items
ساكن صحيح pum_بنياد_ويڪر;		// width of pum items base
ساكن صحيح pum_قسم_ويڪر;		// width of pum items kind column
ساكن صحيح pum_extra_ويڪر;		// width of extra stuff
ساكن صحيح pum_scrollbar;		// TRUE when scrollbar present
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
ساكن صحيح pum_rl;			// TRUE when pum is drawn 'rightleft'
#جيڪڏهن_پورو

ساكن صحيح pum_صف;			// top row of pum
ساكن صحيح pum_کانو;			// left column of pum

ساكن win_T *pum_دري = عدم;
ساكن صحيح pum_win_صف;
ساكن صحيح pum_win_اوچائي;
ساكن صحيح pum_win_کانو;
ساكن صحيح pum_win_wcol;
ساكن صحيح pum_win_ويڪر;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ساكن صحيح pum_pretend_not_visible = FALSE;

// Border support
ساكن صحيح pum_border = 0;
ساكن صحيح pum_margin = 0;	// margin of 1 cell on left and right
ساكن صحيح pum_shadow = 0;

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_POPUP
ساكن image_placement_T *pum_imagep = عدم;
#جيڪڏهن_پورو

// Border characters
ساكن بنية {
    صحيح چوٽي;
    صحيح ساڄو;
    صحيح bottom;
    صحيح کاٻو;
    صحيح چوٽي_کاٻو;
    صحيح چوٽي_ساڄو;
    صحيح bottom_ساڄو;
    صحيح bottom_کاٻو;
} pum_border_chars;

ساكن صحيح pum_سيٽ_selected(صحيح n, صحيح repeat);
ساكن فراغ pum_draw_border(فراغ);
ساكن فراغ pum_draw_shadow(فراغ);
ساكن فراغ compute_margins(صحيح *ساڄو_margin, صحيح *کاٻو_margin, صحيح *کاٻو_padding);

#وصف_ڪريو PUM_DEF_اوچائي 10

#وصف_ڪريو DEFINE_PUM_SETTER(feature) \
	فراغ \
    pum_سيٽ##feature(صحيح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ساكن فراغ
pum_compute_سائيز(فراغ)
{
    صحيح	w;

    // Compute the width of the widest match and the widest extra.
    pum_بنياد_ويڪر = 0;
    pum_قسم_ويڪر = 0;
    pum_extra_ويڪر = 0;
    لكل (صحيح i = 0; i < pum_سائيز; ++i)
    {
	إذا (pum_array[i].pum_متن != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_متن);
	    إذا (pum_بنياد_ويڪر < w)
		pum_بنياد_ويڪر = w;
	}
	إذا (pum_array[i].pum_قسم != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_قسم) + 1;
	    إذا (pum_قسم_ويڪر < w)
		pum_قسم_ويڪر = w;
	}
	إذا (pum_array[i].pum_extra != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    إذا (pum_extra_ويڪر < w)
		pum_extra_ويڪر = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ساكن فراغ
pum_compute_vertical_placement(
	صحيح سائيز,
	صحيح above_صف,
	صحيح هيٺ_صف)
{
    صحيح context_سٽون;
    صحيح cline_visible_وٿي;
    صحيح extra_اوچائي = 2 * pum_border + pum_shadow;
    صحيح extra_وٿي = pum_border + pum_shadow;

    pum_اوچائي = MIN(سائيز, PUM_DEF_اوچائي);
    إذا (p_ph > 0 && pum_اوچائي > p_ph)
	pum_اوچائي = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    إذا (pum_win_صف + 2 >= هيٺ_صف - pum_اوچائي - extra_اوچائي
	    && pum_win_صف - above_صف > (هيٺ_صف - above_صف) / 2)
    {
	// pum above "pum_win_row"
	إذا (State & گهڻائي_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سٽون = 0;
	وإلا
	    // Leave two lines of context if possible
	    context_سٽون = MIN(2, curwin->w_wrow - curwin->w_cline_صف);

	إذا (pum_win_صف >= سائيز + context_سٽون + extra_اوچائي)
	{
	    pum_صف = pum_win_صف - سائيز - context_سٽون - extra_وٿي;
	    pum_اوچائي = سائيز;
	}
	وإلا
	{
	    pum_صف = pum_border;
	    pum_اوچائي = pum_win_صف - context_سٽون - extra_اوچائي;
	}
	إذا (p_ph > 0 && pum_اوچائي > p_ph)
	{
	    pum_صف += pum_اوچائي - p_ph;
	    pum_اوچائي = p_ph;
	}
    }
    وإلا
    {
	// pum below "pum_win_row"
	إذا (State & گهڻائي_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سٽون = 0;
	وإلا
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_وٿي = curwin->w_cline_صف +
		curwin->w_cline_اوچائي - curwin->w_wrow;
	    context_سٽون = MIN(3, cline_visible_وٿي);
	}

	pum_صف = pum_win_صف + context_سٽون + pum_border;
	pum_اوچائي = MIN(هيٺ_صف - pum_صف - extra_وٿي, سائيز);
	إذا (p_ph > 0 && pum_اوچائي > p_ph)
	    pum_اوچائي = p_ph;
    }

#إذا defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    إذا (above_صف > 0 && pum_صف < above_صف && pum_اوچائي > above_صف)
    {
	pum_صف = above_صف;
	pum_اوچائي = pum_win_صف - above_صف;
    }
#جيڪڏهن_پورو
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ساكن صحيح
سيٽ_pum_ويڪر_aligned_with_cursor(صحيح ويڪر, صحيح موجود_ويڪر)
{
    صحيح پڇاڙي_padding = TRUE;

    إذا (ويڪر < p_pw)
    {
	ويڪر = p_pw;
	پڇاڙي_padding = FALSE;
    }
    إذا (p_pmw > 0 && ويڪر >= p_pmw)
    {
	ويڪر = p_pmw;
	پڇاڙي_padding = FALSE;
    }

    pum_ويڪر = ويڪر + (پڇاڙي_padding && ويڪر >= p_pw ? 1 : 0);
    أرجع موجود_ويڪر >= pum_ويڪر;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ساكن فراغ
pum_compute_horizontal_placement(صحيح cursor_کانو)
{
    صحيح desired_ويڪر = pum_بنياد_ويڪر + pum_قسم_ويڪر + pum_extra_ويڪر;
    صحيح موجود_ويڪر;
    صحيح کانو_وٿي;
    صحيح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	کانو_وٿي = pum_border && (cursor_کانو >= Columns - 2)
	    ? (3 - Columns + cursor_کانو) : 0;
	موجود_ويڪر = cursor_کانو + 1;
	pum_کانو = cursor_کانو - کانو_وٿي;
    }
    وإلا
#جيڪڏهن_پورو
    {
	کانو_وٿي = pum_border && (cursor_کانو <= 1) ? (2 - cursor_کانو) : 0;
	موجود_ويڪر = Columns - cursor_کانو;
	pum_کانو = cursor_کانو + کانو_وٿي;
    }

    موجود_ويڪر -= trailing_overhead + کانو_وٿي;

    // Case 1: Align pum with "cursor_col"
    إذا (سيٽ_pum_ويڪر_aligned_with_cursor(desired_ويڪر, موجود_ويڪر))
	أرجع;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    إذا (موجود_ويڪر > p_pw)
    {
	pum_ويڪر = موجود_ويڪر;
	أرجع;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    موجود_ويڪر = Columns - trailing_overhead - کانو_وٿي;

    إذا (موجود_ويڪر > p_pw)
    {
	pum_ويڪر = p_pw;  // Truncate beyond 'pum_width'
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    pum_کانو = pum_ويڪر + trailing_overhead;
	وإلا
#جيڪڏهن_پورو
	    pum_کانو = Columns - pum_ويڪر - trailing_overhead;
	أرجع;
    }

    // Case 3: Not enough room anywhere, use what we have
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
	pum_کانو = Columns - 1 - کانو_وٿي;
    وإلا
#جيڪڏهن_پورو
	pum_کانو = کانو_وٿي;
    pum_ويڪر = Columns - trailing_overhead;
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
	صحيح	    سائيز,
	صحيح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحيح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحيح	    cursor_کانو;
    صحيح	    above_صف;
    صحيح	    هيٺ_صف;
    صحيح	    redo_ڳڻپ = 0;
#إذا defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    pum_rl = (State & گهڻائي_CMDLINE) == 0 && curwin->w_p_rl;
#جيڪڏهن_پورو

    افعل
    {
	above_صف = 0;
	هيٺ_صف = cmdline_صف;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_کانو();
	pum_array = عدم;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_دري = curwin;
	إذا (State & گهڻائي_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_صف = cmdline_صف;
	وإلا
	    pum_win_صف = curwin->w_wrow + W_WINROW(curwin);
	pum_win_اوچائي = curwin->w_اوچائي;
	pum_win_کانو = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_ويڪر = curwin->w_ويڪر;

#إذا defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    إذا (pvwin->w_p_pvw)
		اكسر;
	إذا (pvwin != عدم)
	{
	    إذا (W_WINROW(pvwin) < W_WINROW(curwin))
		above_صف = W_WINROW(pvwin) + pvwin->w_اوچائي;
	    وإلا إذا (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_اوچائي)
		هيٺ_صف = W_WINROW(pvwin);
	}
#جيڪڏهن_پورو

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(سائيز, above_صف, هيٺ_صف);

	// don't display when we only have room for one line
	إذا (pum_اوچائي < 1 || (pum_اوچائي == 1 && سائيز > 1))
	    أرجع;

	pum_array = array;
	pum_سائيز = سائيز;
	pum_compute_سائيز();

	// Calculate cursor column
	إذا (State & گهڻائي_CMDLINE)
	    // cmdline completion popup menu
	    cursor_کانو = cmdline_compl_startcol();
	وإلا
	{
	    صحيح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    إذا (curwin->w_ويڪر > 0)
		wcol %= curwin->w_ويڪر;
	    وإلا
		wcol = 0;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    إذا (curwin->w_p_cole > 0 && conceal_cursor_سٽ(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		إذا (wcol < 0)
		    wcol = 0;
	    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    إذا (pum_rl)
		cursor_کانو = curwin->w_wincol + curwin->w_ويڪر - wcol - 1;
	    وإلا
#جيڪڏهن_پورو
		cursor_کانو = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_اوچائي < سائيز) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_کانو);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } طالما (pum_سيٽ_selected(selected, redo_ڳڻپ) && ++redo_ڳڻپ <= 2);

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
pum_under_menu(صحيح صف, صحيح کانو, صحيح only_redrawing)
{
    صحيح	extra_کاٻو = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحيح	extra_ساڄو = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحيح	extra_above = pum_border;
    صحيح	extra_هيٺ = pum_border + (pum_shadow ? 1 : 0);
    صحيح	چوٽي = pum_صف - extra_above;
    صحيح	bot = pum_صف + pum_اوچائي + extra_هيٺ;
    صحيح	کاٻو = pum_کانو - 1 - extra_کاٻو;
    صحيح	ساڄو = pum_کانو + pum_ويڪر + pum_scrollbar + extra_ساڄو;

    إذا (!((!only_redrawing || pum_will_redraw)
	    && صف >= چوٽي && صف < bot && کانو >= کاٻو && کانو < ساڄو))
	أرجع FALSE;

    إذا (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	إذا (only_redrawing)
	{
	    إذا (کانو >= ساڄو - 2 || صف == bot - 1)
		أرجع FALSE;
	}
	وإلا
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحيح	    ساڄو_margin, کاٻو_margin, کاٻو_padding;

	    compute_margins(&ساڄو_margin, &کاٻو_margin, &کاٻو_padding);
	    إذا (صف == چوٽي && کانو >= ساڄو - 2)
		أرجع FALSE;
	    إذا (صف == bot - 1 && کانو < pum_کانو + 2 - کاٻو_padding - pum_border
		    - کاٻو_margin)
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
    صحيح		اندريون_fuzzy;
    صحيح		matched_len = -1;
    int_u	char_pos = 0;
    صحيح		is_select = FALSE;

    إذا (*متن == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	أرجع عدم;

    is_select = hlf == HLF_PSI;
    leader = (State & گهڻائي_CMDLINE) ? cmdline_compl_نمونو()
							  : ins_compl_leader();
    إذا (leader == عدم || *leader == NUL)
	أرجع عدم;

    attrs = ALLOC_MULT(صحيح, vim_strsize(متن));
    إذا (attrs == عدم)
	أرجع عدم;

    اندريون_fuzzy = (State & گهڻائي_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    إذا (اندريون_fuzzy)
    {
	ga = fuzzy_ميلاپ_str_with_pos(متن, leader);
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
		إذا (char_pos == ((int_u *)ga->ga_معلومات)[i])
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
    صحيح		صف,
    صحيح		کانو,
    صحيح		cells اڻ_استعمال_ٿيل,
    char_u	*متن,
    صحيح		textlen,
    صحيح		*attrs)
{
    صحيح		کانو_شروعات = کانو;
    char_u	*ptr = متن;
    صحيح		char_len;
    صحيح		attr;

    // Render text with proper attributes
    طالما (*ptr != NUL && ptr < متن + textlen)
    {
	char_len = mb_ptr2len(ptr);
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    attr = attrs[کانو_شروعات + cells - کانو - 1];
	وإلا
#جيڪڏهن_پورو
	    attr = attrs[کانو - کانو_شروعات];
	screen_puts_len(ptr, char_len, صف, کانو, attr);
	کانو += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ساكن مضمن char_u *
pum_get_شي(صحيح اشاريو, صحيح type)
{
    بدل(type)
    {
	حالة CPT_ABBR: أرجع pum_array[اشاريو].pum_متن;
	حالة CPT_قسم: أرجع pum_array[اشاريو].pum_قسم;
	حالة CPT_MENU: أرجع pum_array[اشاريو].pum_extra;
    }
    أرجع عدم;
}

    ساكن مضمن صحيح
pum_user_attr_combine(صحيح idx, صحيح type, صحيح attr)
{
    صحيح user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_قسم_hlattr,
    };

    أرجع user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_rtl_متن(
	صحيح	صف,
	صحيح     کانو,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     ويڪر,
	صحيح     ويڪر_حد_بندي,
	صحيح     totwidth,
	صحيح     ايندڙ_isempty,
	صحيح	selected)
{
    char_u  *rt = عدم;
    صحيح     cells = 0;
    صحيح     over_cell = 0;
    صحيح     truncated = FALSE;
    صحيح     pad = ايندڙ_isempty ? 0 : 2;
    صحيح     بچيل = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    إذا (st == عدم)
	أرجع کانو;

    rt = ابتو_متن(st);
    إذا (rt == عدم)
    {
	VIM_CLEAR(st);
	أرجع کانو;
    }

    char_u *rt_شروعات = rt;
    cells = mb_string2cells(rt, -1);
    truncated = ويڪر_حد_بندي - totwidth < cells + pad;

    // only draw the text that fits
    إذا (cells > ويڪر_حد_بندي)
    {
	افعل
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} طالما (cells > ويڪر_حد_بندي);

	إذا (cells < ويڪر_حد_بندي)
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
	صحيح     سائيز = 0;

	بچيل = ويڪر_حد_بندي - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	إذا (cells > بچيل)
	{
	    طالما (cells > بچيل)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	سائيز = (صحيح)STRLEN(orig_rt);
	إذا (cells < بچيل)
	    over_cell = بچيل - cells;

	cells = mb_string2cells(orig_rt, سائيز);
	ويڪر = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, صف, کانو - ويڪر + 1, trunc_attr);

	إذا (over_cell > 0)
	    screen_fill(صف, صف + 1, کانو - ويڪر + 2,
		    کانو - ويڪر + 2 + over_cell, ' ', ' ', attr);
    }

    إذا (attrs == عدم)
	screen_puts_len(rt, (صحيح)STRLEN(rt), صف, کانو - cells + 1, attr);
    وإلا
	pum_screen_puts_with_attrs(صف, کانو - cells + 1, cells, rt,
		(صحيح)STRLEN(rt), attrs);

    vim_free(rt_شروعات);
    VIM_CLEAR(st);
    أرجع کانو - ويڪر;
}
#جيڪڏهن_پورو

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_ltr_متن(
	صحيح     صف,
	صحيح     کانو,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     ويڪر,        // width already calculated in outer loop
	صحيح     ويڪر_حد_بندي,
	صحيح     totwidth,
	صحيح     ايندڙ_isempty,
	صحيح	selected)
{
    صحيح     سائيز = 0;
    صحيح     cells = 0;
    char_u  *st_پڇاڙي = عدم;
    صحيح     over_cell = 0;
    صحيح     pad = ايندڙ_isempty ? 0 : 2;
    صحيح     truncated = FALSE;
    صحيح     بچيل = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    إذا (st == عدم)
	أرجع کانو;

    سائيز = (صحيح)STRLEN(st);
    cells = (*mb_string2cells)(st, سائيز);
    truncated = ويڪر_حد_بندي - totwidth < cells + pad;

    // only draw the text that fits
    طالما (سائيز > 0 && کانو + cells > ويڪر_حد_بندي + pum_کانو)
    {
	--سائيز;
	إذا (has_mbyte)
	{
	    سائيز -= (*mb_سر_off)(st, st + سائيز);
	    cells -= (*mb_ptr2cells)(st + سائيز);
	}
	وإلا
	    --cells;
    }

    // truncated
    إذا (truncated)
    {
	بچيل = ويڪر_حد_بندي - totwidth - 1;
	إذا (cells > بچيل)
	{
	    st_پڇاڙي = st + سائيز;
	    طالما (st_پڇاڙي > st && cells > بچيل)
	    {
		MB_PTR_پٺ(st, st_پڇاڙي);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_پڇاڙي) : 1;
	    }
	    سائيز = st_پڇاڙي - st;
	}

	إذا (cells < بچيل)
	    over_cell = بچيل - cells;
	cells = mb_string2cells(st, سائيز);
	ويڪر = cells + over_cell + 1;
    }

    إذا (attrs == عدم)
	screen_puts_len(st, سائيز, صف, کانو, attr);
    وإلا
	pum_screen_puts_with_attrs(صف, کانو, cells, st, سائيز, attrs);

    إذا (truncated)
    {
	إذا (over_cell > 0)
	    screen_fill(صف, صف + 1, کانو + cells,
		    کانو + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, صف, کانو + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    أرجع کانو + ويڪر;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ساكن صحيح
pum_process_شي(
	صحيح     صف,
	صحيح     کانو,
	صحيح     idx,
	صحيح     j,         // Current position in order array
	صحيح     *ترتيب,    // Order array
	hlf_T   hlf,
	صحيح     attr,
	صحيح     *totwidth_ptr,
	صحيح     ايندڙ_isempty)
{
    صحيح     شي_type = ترتيب[j];
    char_u  *s = عدم;
    char_u  *p = pum_get_شي(idx, شي_type);
    صحيح     ويڪر = 0;  // item width
    صحيح     w;		// char width
    صحيح	    selected = idx == pum_selected;

    لكل ( ; ; MB_PTR_ADV(p))
    {
	إذا (s == عدم)
	    s = p;
	w = ptr2cells(p);
	إذا (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_ويڪر)
	{
	    ويڪر += w;
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

	إذا (شي_type == CPT_ABBR)
	    attrs = pum_compute_متن_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    کانو = pum_display_rtl_متن(صف, کانو, st, attr, attrs,
		    ويڪر, pum_ويڪر, *totwidth_ptr, ايندڙ_isempty, selected);
	وإلا
#جيڪڏهن_پورو
	    کانو = pum_display_ltr_متن(صف, کانو, st, attr, attrs,
		    ويڪر, pum_ويڪر, *totwidth_ptr, ايندڙ_isempty, selected);

	إذا (attrs != عدم)
	    VIM_CLEAR(attrs);

	إذا (*p != TAB)
	    اكسر;

	// Display two spaces for a Tab.
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, صف, کانو - 1, attr);
	    کانو -= 2;
	}
	وإلا
#جيڪڏهن_پورو
	{
	    screen_puts_len((char_u *)"  ", 2, صف, کانو, attr);
	    کانو += 2;
	}
	*totwidth_ptr += 2;
	s = عدم;  // start text at next char
	ويڪر = 0;
    }

    أرجع کانو;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ساكن فراغ
pum_draw_scrollbar(
	صحيح	صف,
	صحيح	i,
	صحيح	thumb_pos,
	صحيح	thumb_اوچائي)
{
    إذا (pum_scrollbar <= 0)
	أرجع;

    صحيح attr = (i >= thumb_pos && i < thumb_pos + thumb_اوچائي) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
	screen_putchar(' ', صف, pum_کانو - pum_ويڪر, attr);
    وإلا
#جيڪڏهن_پورو
	screen_putchar(' ', صف, pum_کانو + pum_ويڪر, attr);
}

    ساكن مضمن فراغ
pum_align_ترتيب(صحيح *ترتيب)
{
    صحيح is_default = cia_flags == 0;
    ترتيب[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ترتيب[1] = is_default ? CPT_قسم : (cia_flags / 10) % 10;
    ترتيب[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ساكن فراغ pum_free_bg(فراغ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    فراغ
pum_opacity_بدليو(فراغ)
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
    VIM_CLEAR(pum_bg_سٽون);
    VIM_CLEAR(pum_bg_سٽون_UC);
    لكل (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_سٽون_C[k]);
    pum_bg_چوٽي = 0;
    pum_bg_bot = 0;
    pum_bg_کانن_ڳڻپ = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    فراغ
pum_redraw(فراغ)
{
    صحيح		صف = pum_صف;
    صحيح		کانو;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحيح		attr;
    صحيح		i, j;
    صحيح		idx;
    char_u	*p = عدم;
    صحيح		totwidth;
    صحيح		thumb_pos = 0;
    صحيح		thumb_اوچائي = 1;
    صحيح		شي_type;
    صحيح		ترتيب[3];
    صحيح		ايندڙ_isempty = FALSE;
    صحيح		n;
    صحيح		شيون_ويڪر_array[3] = { pum_بنياد_ويڪر, pum_قسم_ويڪر,
							    pum_extra_ويڪر };
    صحيح		basic_ويڪر;  // first item width
    صحيح		آخري_isabbr = FALSE;
    صحيح		orig_attr = -1;
    صحيح		scroll_range = pum_سائيز - pum_اوچائي;
    bool	override_success;
    صحيح		opacity_سرگرم = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ڌڪو_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    رڪن_سيٽ_sync_output(رڪن_SYNC_OUTPUT_ENABLE);

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    إذا (pum_imagep == عدم)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(عدم, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    إذا (pum_imagep != عدم)
    {
	image_placement_سيٽ_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_سيٽ_position(pum_imagep, pum_صف, pum_کانو);
	image_placement_سيٽ_bounding_box(pum_imagep, pum_صف, pum_کانو,
		pum_اوچائي, pum_ويڪر);
	image_placement_سيٽ_enable(pum_imagep, true);
    }
#جيڪڏهن_پورو

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
	إذا (opacity_سرگرم && pum_bg_سٽون != عدم
		&& (pum_bg_کانن_ڳڻپ != screen_Columns
		    || pum_bg_bot > screen_صفون))
	    pum_free_bg();

	إذا (opacity_سرگرم && pum_bg_سٽون != عدم)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	وإلا إذا (opacity_سرگرم)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    إذا (Screen_سٽون != عدم && ScreenAttrs != عدم)
	    {
		صحيح	save_چوٽي, save_bot, save_ncells, k;

		pum_free_bg();
		save_چوٽي = pum_صف - pum_border;
		save_bot = pum_صف + pum_اوچائي + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		إذا (save_چوٽي < 0)
		    save_چوٽي = 0;
		إذا (save_bot > screen_صفون)
		    save_bot = screen_صفون;
		pum_bg_چوٽي = save_چوٽي;
		pum_bg_bot = save_bot;
		pum_bg_کانن_ڳڻپ = screen_Columns;
		إذا (save_چوٽي < save_bot)
		{
		    save_ncells = (save_bot - save_چوٽي) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_سٽون = LALLOC_MULT(schar_T, save_ncells);
		    إذا (enc_utf8)
		    {
			pum_bg_سٽون_UC = LALLOC_MULT(u8char_T, save_ncells);
			لكل (k = 0; k < MAX_MCO; ++k)
			    pum_bg_سٽون_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    إذا (pum_bg_attrs != عدم && pum_bg_سٽون != عدم)
		    {
			لكل (صحيح r = save_چوٽي; r < save_bot; ++r)
			{
			    صحيح soff = (r - save_چوٽي) * screen_Columns;
			    صحيح loff = سٽ_وٿي[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * حجم(sattr_T));
			    mch_memmove(pum_bg_سٽون + soff,
				    Screen_سٽون + loff,
				    screen_Columns * حجم(schar_T));
			    إذا (enc_utf8 && pum_bg_سٽون_UC != عدم
						&& Screen_سٽون_UC != عدم)
			    {
				mch_memmove(pum_bg_سٽون_UC + soff,
					Screen_سٽون_UC + loff,
					screen_Columns * حجم(u8char_T));
				لكل (k = 0; k < MAX_MCO; ++k)
				    إذا (pum_bg_سٽون_C[k] != عدم
						    && Screen_سٽون_C[k] != عدم)
					mch_memmove(pum_bg_سٽون_C[k] + soff,
						Screen_سٽون_C[k] + loff,
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
    pum_پهريون = MIN(pum_پهريون, scroll_range);

    إذا (pum_scrollbar)
    {
	thumb_اوچائي = pum_اوچائي * pum_اوچائي / pum_سائيز;
	إذا (thumb_اوچائي == 0)
	    thumb_اوچائي = 1;
	thumb_pos = (pum_پهريون * (pum_اوچائي - thumb_اوچائي)
			    + scroll_range / 2) / scroll_range;
    }

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#جيڪڏهن_پورو

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    إذا (pum_border)
	pum_draw_border();
    إذا (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    إذا (opacity_سرگرم)
	screen_pum_blend = 100 - (صحيح)p_po;

    لكل (i = 0; i < pum_اوچائي; ++i)
    {
	idx = i + pum_پهريون;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    إذا (pum_کانو < curwin->w_wincol + curwin->w_ويڪر - 1
							    - pum_border)
		screen_putchar(' ', صف, pum_کانو + 1, attr);
	}
	وإلا
#جيڪڏهن_پورو
	    إذا (pum_کانو > pum_border)
		screen_putchar(' ', صف, pum_کانو - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	کانو = pum_کانو;
	totwidth = 0;
	pum_align_ترتيب(ترتيب);
	basic_ويڪر = شيون_ويڪر_array[ترتيب[0]];
	آخري_isabbr = ترتيب[2] == CPT_ABBR;
	لكل (j = 0; j < 3; ++j)
	{
	    شي_type = ترتيب[j];
	    hlf = hlfs[شي_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    إذا (شي_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, شي_type, attr);
	    p = pum_get_شي(idx, شي_type);

	    إذا (j + 1 < 3)
		ايندڙ_isempty = pum_get_شي(idx, ترتيب[j + 1]) == عدم;
	    وإلا
		ايندڙ_isempty = TRUE;

	    إذا (p != عدم)
		// Process and display the item
		کانو = pum_process_شي(صف, کانو, idx, j, ترتيب, hlf, attr,
						    &totwidth, ايندڙ_isempty);

	    إذا (j > 0)
		n = شيون_ويڪر_array[ترتيب[1]] + (آخري_isabbr ? 0 : 1);
	    وإلا
		n = ترتيب[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    إذا (j == 2
		    || (ايندڙ_isempty && (j == 1 || (j == 0
				&& pum_get_شي(idx, ترتيب[j + 2]) == عدم)))
		    || basic_ويڪر + n >= pum_ويڪر)
		اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    إذا (pum_rl)
	    {
		screen_fill(صف, صف + 1, pum_کانو - basic_ويڪر - n + 1,
						کانو + 1, ' ', ' ', orig_attr);
		کانو = pum_کانو - basic_ويڪر - n;
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		screen_fill(صف, صف + 1, کانو, pum_کانو + basic_ويڪر + n,
							' ', ' ', orig_attr);
		کانو = pum_کانو + basic_ويڪر + n;
	    }
	    totwidth = basic_ويڪر + n;
	}

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    screen_fill(صف, صف + 1, pum_کانو - pum_ويڪر + 1, کانو + 1,
							    ' ', ' ', orig_attr);
	وإلا
#جيڪڏهن_پورو
	    screen_fill(صف, صف + 1, کانو, pum_کانو + pum_ويڪر, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(صف, i, thumb_pos, thumb_اوچائي);

	++صف;
    }

    screen_pum_blend = 0;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    screen_zindex = 0;
#جيڪڏهن_پورو

    إذا (override_success)
	ڪڍو_highlight_overrides();

    رڪن_سيٽ_sync_output(رڪن_SYNC_OUTPUT_DISABLE);
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    فراغ
pum_position_info_popup(win_T *wp)
{
    صحيح کانو = pum_کانو + pum_ويڪر + pum_scrollbar + 1;
    صحيح صف = pum_صف;
    صحيح botpos = POPPOS_BOTLEFT;
    صحيح	استعمال_ٿيل_maxwidth_opt = FALSE;
    صحيح	ساڄو_margin, کاٻو_margin, dummy;
    صحيح کاٻو_extra_ويڪر, ساڄو_extra_ويڪر;

    (فراغ)compute_margins(&ساڄو_margin, &کاٻو_margin, &dummy);

    کاٻو_extra_ويڪر = pum_border + کاٻو_margin;
    ساڄو_extra_ويڪر = pum_border + ساڄو_margin + (pum_shadow ? 2 : 0);
    کانو += ساڄو_extra_ويڪر;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    إذا (Columns - کانو < 20 && Columns - کانو < pum_کانو)
    {
	کانو = pum_کانو - 1 - کاٻو_extra_ويڪر;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_کانو - 1 - کاٻو_extra_ويڪر;
    }
    وإلا
	wp->w_maxwidth = Columns - کانو + 1;
    wp->w_maxwidth -= popup_extra_ويڪر(wp);
    إذا (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	استعمال_ٿيل_maxwidth_opt = TRUE;
    }

    صف -= popup_چوٽي_extra(wp);
    إذا (wp->w_popup_flags & POPF_INFO_MENU)
    {
	إذا (pum_صف < pum_win_صف)
	{
	    // menu above cursor line, align with bottom
	    صف += pum_اوچائي;
	    wp->w_popup_pos = botpos;
	}
	وإلا
	    // menu below cursor line, align with top
	    صف += 1;
    }
    وإلا
	// align with the selected item
	صف += pum_selected - pum_پهريون + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (wp->w_maxwidth < 10 && !استعمال_ٿيل_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    وإلا
	popup_سيٽ_wantpos_rowcol(wp, صف, کانو);
}
#جيڪڏهن_پورو

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
pum_سيٽ_selected(صحيح n, صحيح repeat اڻ_استعمال_ٿيل)
{
    صحيح	    resized = FALSE;
    صحيح	    context = pum_اوچائي / 2;
    صحيح	    scroll_وٿي;
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    صحيح	    prev_selected = pum_selected;
    بنانشان	cur_cot_flags = get_cot_flags();
#جيڪڏهن_پورو
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحيح	    has_info = FALSE;
#جيڪڏهن_پورو

    pum_selected = n;
    scroll_وٿي = pum_selected - pum_اوچائي;

    إذا (pum_selected >= 0 && pum_selected < pum_سائيز)
    {
	إذا (pum_پهريون > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    إذا (pum_پهريون > pum_selected - 2)
	    {
		pum_پهريون -= pum_اوچائي - 2;
		إذا (pum_پهريون < 0)
		    pum_پهريون = 0;
		وإلا إذا (pum_پهريون > pum_selected)
		    pum_پهريون = pum_selected;
	    }
	    وإلا
		pum_پهريون = pum_selected;
	}
	وإلا إذا (pum_پهريون < scroll_وٿي + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    إذا (pum_پهريون < scroll_وٿي + 3)
		pum_پهريون = MAX(pum_پهريون + pum_اوچائي - 2, scroll_وٿي + 1);
	    وإلا
		pum_پهريون = scroll_وٿي + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	إذا (pum_اوچائي > 2)
	{
	    إذا (pum_پهريون > pum_selected - context)
		pum_پهريون = MAX(pum_selected - context, 0);  // scroll down
	    وإلا إذا (pum_پهريون < pum_selected + context - pum_اوچائي + 1)
		pum_پهريون = pum_selected + context - pum_اوچائي + 1;  //مٿي
	}
	// adjust for the number of lines displayed
	pum_پهريون = MIN(pum_پهريون, pum_سائيز - pum_اوچائي);

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
		&& صفون > 10
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
		use_popup = USEPOPUP_عام;
	    وإلا
		use_popup = USEPOPUP_NONE;
	    إذا (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		بلاڪ_autocmds();
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
			سيٽ_option_قدر_give_err((char_u *)"swf",
							  0L, عدم, OPT_LOCAL);
			سيٽ_option_قدر_give_err((char_u *)"bl",
							  0L, عدم, OPT_LOCAL);
			سيٽ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			سيٽ_option_قدر_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			سيٽ_option_قدر_give_err((char_u *)"diff",
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
		    ml_delete(curbuf->b_ml.ml_سٽ_ڳڻپ);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    إذا (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			إذا (curwin->w_اوچائي < lnum)
			{
			    win_setheight((صحيح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_بدليو = 0;
		    curbuf->b_p_ma = FALSE;
		    إذا (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    وإلا إذا (curwin->w_topline > curbuf->b_ml.ml_سٽ_ڳڻپ)
			curwin->w_topline = curbuf->b_ml.ml_سٽ_ڳڻپ;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.کانو = 0;
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
			صحيح save_redr_حالت;

			إذا (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			إذا (ins_compl_سرگرم() && !resized)
			    curwin->w_redr_حالت = false;

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
			save_redr_حالت = curwin_save->w_redr_حالت;
			curwin_save->w_redr_حالت = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_حالت = save_redr_حالت;

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
#جيڪڏهن_پورو
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    إذا (!has_info)
	// hide any popup info window
	popup_hide_info();
#جيڪڏهن_پورو

    أرجع resized;
}

/*
 * Undisplay the popup menu (later).
 */
    فراغ
pum_undisplay(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_POPUP
    إذا (pum_imagep != عدم)
    {
	image_placement_free(pum_imagep);
	pum_imagep = عدم;
    }
#جيڪڏهن_پورو
    pum_free_bg();
    pum_array = عدم;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جيڪڏهن_پورو
    إذا (pum_اندريون_cmdline)
    {
	clear_cmdline = TRUE;
	pum_اندريون_cmdline = FALSE;
    }
    حالت_redraw_all();
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#جيڪڏهن_پورو
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    فراغ
pum_clear(فراغ)
{
    pum_پهريون = 0;
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
pum_اندريون_same_position(فراغ)
{
    صحيح	    صف = (State & گهڻائي_CMDLINE)
			? cmdline_صف
			: curwin->w_wrow + W_WINROW(curwin);

    أرجع pum_دري != curwin
	    || (pum_win_صف == صف
		&& pum_win_اوچائي == curwin->w_اوچائي
		&& pum_win_کانو == curwin->w_wincol
		&& pum_win_ويڪر == curwin->w_ويڪر);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحيح
pum_redraw_اندريون_same_position(فراغ)
{
    إذا (!pum_visible() || pum_will_redraw)
	أرجع FALSE;  // nothing to do

    أرجع pum_اندريون_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    فراغ
pum_may_redraw(فراغ)
{
    pumitem_T	*array = pum_array;
    صحيح		len = pum_سائيز;
    صحيح		selected = pum_selected;

    إذا (!pum_visible() || pum_will_redraw)
	أرجع;  // nothing to do

    إذا (pum_اندريون_same_position())
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
pum_get_اوچائي(فراغ)
{
    أرجع pum_اوچائي;
}

#إذا defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    فراغ
pum_سيٽ_event_info(dict_T *dict)
{
    إذا (!pum_visible())
	أرجع;
    (فراغ)dict_جوڙيو_عدد(dict, "height", pum_اوچائي);
    (فراغ)dict_جوڙيو_عدد(dict, "width", pum_ويڪر);
    (فراغ)dict_جوڙيو_عدد(dict, "row", pum_صف);
    (فراغ)dict_جوڙيو_عدد(dict, "col", pum_کانو);
    (فراغ)dict_جوڙيو_عدد(dict, "size", pum_سائيز);
    (فراغ)dict_جوڙيو_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_BEVAL_رڪن) || defined(FEAT_رڪن_POPUP_MENU)
    ساكن فراغ
pum_position_at_mouse(صحيح min_ويڪر)
{
    صحيح extra_اوچائي = pum_border * 2 + pum_shadow;
    صحيح extra_وٿي = pum_border + pum_shadow;
    صحيح trailing_overhead = 0;
    صحيح کانو_وٿي = 0;
    صحيح موجود_ويڪر;

    إذا (صفون - mouse_صف - extra_اوچائي > pum_سائيز
	    || صفون - mouse_صف > mouse_صف)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_صف = mouse_صف + 1 + pum_border;
	إذا (pum_اوچائي > صفون - pum_صف - extra_اوچائي)
	    pum_اوچائي = صفون - pum_صف - extra_اوچائي;
	إذا (pum_صف + pum_اوچائي + extra_وٿي >= cmdline_صف)
	    pum_اندريون_cmdline = TRUE;
    }
    وإلا
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_صف = mouse_صف - pum_سائيز - extra_وٿي;
	إذا (pum_صف < pum_border)
	{
	    pum_اوچائي += pum_صف - pum_border;
	    pum_صف = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	کانو_وٿي = (mouse_کانو == Columns - 1) && pum_border;
	موجود_ويڪر = mouse_کانو + 1 - trailing_overhead - کانو_وٿي;
	إذا (موجود_ويڪر >= pum_بنياد_ويڪر || موجود_ويڪر > min_ويڪر)
	    // Enough space to show at mouse column.
	    pum_کانو = mouse_کانو - کانو_وٿي;
	وإلا
	    // Not enough space, left align with window.
	    pum_کانو = MIN(pum_بنياد_ويڪر, min_ويڪر) - 1 + trailing_overhead;
	pum_ويڪر = pum_کانو + 1 - trailing_overhead;
    }
    وإلا
# endif
    {
	کانو_وٿي = (mouse_کانو == 0) && pum_border;
	موجود_ويڪر = Columns - mouse_کانو - trailing_overhead - کانو_وٿي;
	إذا (موجود_ويڪر >= pum_بنياد_ويڪر || موجود_ويڪر > min_ويڪر)
	    // Enough space to show at mouse column.
	    pum_کانو = mouse_کانو + کانو_وٿي;
	وإلا
	    // Not enough space, right align with window.
	    pum_کانو = Columns - MIN(pum_بنياد_ويڪر, min_ويڪر)
		- trailing_overhead;
	pum_ويڪر = Columns - pum_کانو - trailing_overhead;
    }

    pum_ويڪر = MIN(pum_ويڪر, pum_بنياد_ويڪر + 1);
    // Do not redraw at cursor position.
    pum_دري = عدم;
}

#جيڪڏهن_پورو

#إذا defined(FEAT_BEVAL_رڪن)
ساكن pumitem_T *balloon_array = عدم;
ساكن صحيح balloon_arraysize;

# define BALLOON_MIN_ويڪر 50
# define BALLOON_MIN_اوچائي 10

تعريفنوع بنية {
    char_u	*شروعات;
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
ورهايو_پيغام(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*شي;
    صحيح		quoted = FALSE;
    صحيح		اوچائي;
    صحيح		سٽ;
    صحيح		شي_idx;
    صحيح		indent = 0;
    صحيح		max_cells = 0;
    صحيح		max_اوچائي_2 = صفون / 2 - 1;
    صحيح		long_شي_ڳڻپ = 0;
    صحيح		ورهايو_long_شيون = FALSE;

    ga_init2(&ga, حجم(balpart_T), 20);
    p = mesg;

    طالما (*p != NUL)
    {
	إذا (ga_grow(&ga, 1) == FAIL)
	    اذهب failed;
	شي = ((balpart_T *)ga.ga_معلومات) + ga.ga_len;
	شي->شروعات = p;
	شي->indent = indent;
	شي->cells = indent * 2;
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
		    ++شي->cells;
		    p = skipwhite(p + 1);
		    اكسر;
		}
	    }
	    شي->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	شي->bytelen = p - شي->شروعات;
	إذا (*p == '\n')
	    ++p;
	إذا (شي->cells > max_cells)
	    max_cells = شي->cells;
	long_شي_ڳڻپ += (شي->cells - 1) / BALLOON_MIN_ويڪر;
    }

    اوچائي = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    إذا (long_شي_ڳڻپ > 0 && اوچائي + long_شي_ڳڻپ <= max_اوچائي_2)
    {
	ورهايو_long_شيون = TRUE;
	اوچائي += long_شي_ڳڻپ;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    إذا (اوچائي > max_اوچائي_2)
	اوچائي = max_اوچائي_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, اوچائي);
    إذا (*array == عدم)
	اذهب failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_متن = vim_strsave((char_u *)"");
    (*array + اوچائي - 1)->pum_متن = vim_strsave((char_u *)"");

    لكل (سٽ = 1, شي_idx = 0; سٽ < اوچائي - 1; ++شي_idx)
    {
	صحيح	skip;
	صحيح	thislen;
	صحيح	copylen;
	صحيح	ind;
	صحيح	cells;

	شي = ((balpart_T *)ga.ga_معلومات) + شي_idx;
	إذا (شي->bytelen == 0)
	    (*array)[سٽ++].pum_متن = vim_strsave((char_u *)"");
	وإلا
	    لكل (skip = 0; skip < شي->bytelen; skip += thislen)
	    {
		إذا (ورهايو_long_شيون && شي->cells >= BALLOON_MIN_ويڪر)
		{
		    cells = شي->indent * 2;
		    لكل (p = شي->شروعات + skip;
			    p < شي->شروعات + شي->bytelen;
							    p += mb_ptr2len(p))
			إذا ((cells += ptr2cells(p)) > BALLOON_MIN_ويڪر)
			    اكسر;
		    thislen = p - (شي->شروعات + skip);
		}
		وإلا
		    thislen = شي->bytelen;

		// put indent at the start
		p = alloc(thislen + شي->indent * 2 + 1);
		إذا (p == عدم)
		{
		    لكل (سٽ = 0; سٽ <= اوچائي - 1; ++سٽ)
			vim_free((*array)[سٽ].pum_متن);
		    vim_free(*array);
		    اذهب failed;
		}
		لكل (ind = 0; ind < شي->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		لكل (copylen = thislen; copylen > 0; --copylen)
		    إذا (شي->شروعات[skip + copylen - 1] != ' ')
			اكسر;

		vim_strncpy(p + ind, شي->شروعات + skip, copylen);
		(*array)[سٽ].pum_متن = p;
		شي->indent = 0;  // wrapped line has no indent
		++سٽ;
	    }
    }
    ga_clear(&ga);
    أرجع اوچائي;

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
	لكل (idx = 0, li = list->lv_پهريون; li != عدم; li = li->li_ايندڙ, ++idx)
	{
	    char_u *متن = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_متن = vim_strsave(
					   متن == عدم ? (char_u *)"" : متن);
	}
    }
    وإلا
	balloon_arraysize = ورهايو_پيغام(mesg, &balloon_array);

    إذا (balloon_arraysize <= 0)
	أرجع;

    pum_array = balloon_array;
    pum_سائيز = balloon_arraysize;
    pum_compute_سائيز();
    pum_scrollbar = 0;
    pum_اوچائي = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_ويڪر);
    pum_selected = -1;
    pum_پهريون = 0;
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
#جيڪڏهن_پورو

#إذا defined(FEAT_رڪن_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ساكن فراغ
pum_select_mouse_pos(فراغ)
{
    صحيح idx = mouse_صف - pum_صف;

    إذا (idx < 0 || idx >= pum_اوچائي)
	pum_selected = -1;
    وإلا إذا (*pum_array[idx].pum_متن != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ساكن فراغ
pum_execute_menu(vimmenu_T *menu, صحيح گهڻائي)
{
    vimmenu_T   *mp;
    صحيح		idx = 0;
    exarg_T	ea;

    FOR_ALL_ٻار_MENUS(menu, mp)
	إذا ((mp->modes & mp->enabled & گهڻائي) && idx++ == pum_selected)
	{
	    CLEAR_ميدان(ea);
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
# ifdef FEAT_BEVAL_رڪن
    صحيح		save_bevalterm = p_bevalterm;
# endif
    صحيح		گهڻائي;

    pum_undisplay();
    pum_سائيز = 0;
    گهڻائي = get_menu_گهڻائي_flag();

    FOR_ALL_ٻار_MENUS(menu, mp)
	إذا (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & گهڻائي))
	    ++pum_سائيز;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    إذا (pum_سائيز <= 0)
    {
	emsg(_(e_menu_only_exists_اندريون_another_گهڻائي));
	أرجع;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_سائيز);
    إذا (array == عدم)
	أرجع;

    FOR_ALL_ٻار_MENUS(menu, mp)
    {
	char_u *s = عدم;

	// Make a copy of the text, the menu may be redefined in a callback.
	إذا (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	وإلا إذا (mp->modes & mp->enabled & گهڻائي)
	    s = mp->dname;
	إذا (s != عدم)
	{
	    s = vim_strsave(s);
	    إذا (s != عدم)
		array[idx++].pum_متن = s;
	}
    }

    pum_array = array;
    pum_compute_سائيز();
    pum_scrollbar = 0;
    pum_اوچائي = pum_سائيز;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_پهريون = 0;
# ifdef FEAT_BEVAL_رڪن
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    لكل (;;)
    {
	صحيح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	ٻاهريون_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	إذا (c == ESC || c == Ctrl_C || pum_array == عدم)
	    اكسر;
	وإلا إذا (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, گهڻائي);
	    اكسر;
	}
	وإلا إذا (c == 'k' || c == K_مٿي || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    طالما (pum_selected > 0)
	    {
		--pum_selected;
		إذا (*array[pum_selected].pum_متن != NUL)
		    اكسر;
	    }
	}
	وإلا إذا (c == 'j' || c == K_هيٺ_ڏانهن || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    طالما (pum_selected < pum_سائيز - 1)
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
		pum_execute_menu(menu, گهڻائي);
		اكسر;
	    }
	    إذا (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		اكسر;
	}
    }

    لكل (idx = 0; idx < pum_سائيز; ++idx)
	vim_free(array[idx].pum_متن);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_رڪن
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    فراغ
pum_make_popup(char_u *رستو_name, صحيح use_mouse_pos)
{
    vimmenu_T *menu;

    إذا (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_صف = W_WINROW(curwin) + curwin->w_wrow;
	mouse_کانو =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_ڳوليو_menu(رستو_name);
    إذا (menu != عدم)
	pum_show_popupmenu(menu);
}
#جيڪڏهن_پورو

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
pum_سيٽ_border_chars(
	صحيح چوٽي,
	صحيح ساڄو,
	صحيح bottom,
	صحيح کاٻو,
	صحيح چوٽي_کاٻو,
	صحيح چوٽي_ساڄو,
	صحيح bottom_ساڄو,
	صحيح bottom_کاٻو)
{
    إذا (!is_singlewidth(چوٽي) || !is_singlewidth(ساڄو)
	    || !is_singlewidth(bottom) || !is_singlewidth(کاٻو)
	    || !is_singlewidth(چوٽي_کاٻو) || !is_singlewidth(چوٽي_ساڄو)
	    || !is_singlewidth(bottom_ساڄو) || !is_singlewidth(bottom_کاٻو))
	أرجع;
    pum_border_chars.چوٽي = چوٽي;
    pum_border_chars.ساڄو = ساڄو;
    pum_border_chars.bottom = bottom;
    pum_border_chars.کاٻو = کاٻو;
    pum_border_chars.چوٽي_کاٻو = چوٽي_کاٻو;
    pum_border_chars.چوٽي_ساڄو = چوٽي_ساڄو;
    pum_border_chars.bottom_ساڄو = bottom_ساڄو;
    pum_border_chars.bottom_کاٻو = bottom_کاٻو;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ساكن فراغ
compute_margins(صحيح *ساڄو_margin, صحيح *کاٻو_margin, صحيح *کاٻو_padding)
{
    *ساڄو_margin = pum_margin && pum_border;

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	*کاٻو_padding = (pum_کانو < Columns - 1 - pum_border);
	*کاٻو_margin = *ساڄو_margin && (pum_کانو < Columns - 3);
	أرجع;
    }
#جيڪڏهن_پورو
    *کاٻو_padding = (pum_کانو > pum_border);
    *کاٻو_margin = *ساڄو_margin && (pum_کانو > 2);
}

/*
 * Draw the border around the popup menu
 */
    ساكن فراغ
pum_draw_border(فراغ)
{
    صحيح	چوٽي_صف = pum_صف - 1;
    صحيح	bottom_صف = pum_صف + pum_اوچائي;
    صحيح	کانو;
    صحيح	attr = highlight_attr[HLF_PMB];
    صحيح	ساڄو_margin, کاٻو_margin, کاٻو_padding;
    صحيح	inner_ويڪر;

    إذا (!pum_border)
	أرجع;

    (فراغ)compute_margins(&ساڄو_margin, &کاٻو_margin, &کاٻو_padding);

    inner_ويڪر = pum_ويڪر + pum_scrollbar + کاٻو_padding;

    // Draw border left to right or right to left (for RTL)
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
	کانو = pum_کانو + کاٻو_padding + 1;
    وإلا
#جيڪڏهن_پورو
	کانو = pum_کانو - کاٻو_padding - 1;

    // left corners (right corners for RTL)
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٽي_ساڄو, چوٽي_صف, کانو, attr);
	إذا (کاٻو_margin)
	    screen_putchar(' ', چوٽي_صف, کانو + 1, attr);
	screen_putchar(pum_border_chars.bottom_ساڄو, bottom_صف, کانو, attr);
	إذا (کاٻو_margin)
	    screen_putchar(' ', bottom_صف, کانو + 1, attr);
    }
    وإلا
#جيڪڏهن_پورو
    {
	screen_putchar(pum_border_chars.چوٽي_کاٻو, چوٽي_صف, کانو, attr);
	إذا (کاٻو_margin)
	    screen_putchar(' ', چوٽي_صف, کانو - 1, attr);
	screen_putchar(pum_border_chars.bottom_کاٻو, bottom_صف, کانو, attr);
	إذا (کاٻو_margin)
	    screen_putchar(' ', bottom_صف, کانو - 1, attr);
    }

    // Top horizontal lines
    لكل (صحيح i = 0; i < inner_ويڪر; i++)
    {
	صحيح cur_کانو;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    cur_کانو = کانو - 1 - i;
	وإلا
#جيڪڏهن_پورو
	    cur_کانو = کانو + 1 + i;
	screen_putchar(pum_border_chars.چوٽي, چوٽي_صف, cur_کانو, attr);
	screen_putchar(pum_border_chars.bottom, bottom_صف, cur_کانو, attr);
    }

    // right corners (left corners for RTL)
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٽي_کاٻو, چوٽي_صف,
		کانو - inner_ويڪر - 1, attr);
	إذا (ساڄو_margin)
	    screen_putchar(' ', چوٽي_صف, کانو - inner_ويڪر - 2, attr);
	screen_putchar(pum_border_chars.bottom_کاٻو, bottom_صف,
		کانو - inner_ويڪر - 1, attr);
	إذا (ساڄو_margin)
	    screen_putchar(' ', bottom_صف, کانو - inner_ويڪر - 2, attr);
    }
    وإلا
#جيڪڏهن_پورو
    {
	screen_putchar(pum_border_chars.چوٽي_ساڄو, چوٽي_صف,
		کانو + inner_ويڪر + 1, attr);
	إذا (ساڄو_margin)
	    screen_putchar(' ', چوٽي_صف, کانو + inner_ويڪر + 2, attr);
	screen_putchar(pum_border_chars.bottom_ساڄو, bottom_صف,
		کانو + inner_ويڪر + 1, attr);
	إذا (ساڄو_margin)
	    screen_putchar(' ', bottom_صف, کانو + inner_ويڪر + 2, attr);
    }

    // Draw side borders
    لكل (صحيح i = 0; i < pum_اوچائي; i++)
    {
	صحيح صف = pum_صف + i;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_putchar(pum_border_chars.کاٻو, صف, کانو, attr);
	    إذا (کاٻو_margin)
		screen_putchar(' ', صف, کانو + 1, attr);
	    screen_putchar(pum_border_chars.ساڄو, صف,
		    کانو - inner_ويڪر - 1, attr);
	    إذا (ساڄو_margin)
		screen_putchar(' ', صف, کانو - inner_ويڪر - 2, attr);
	}
	وإلا
#جيڪڏهن_پورو
	{
	    screen_putchar(pum_border_chars.کاٻو, صف, کانو, attr);
	    إذا (کاٻو_margin)
		screen_putchar(' ', صف, کانو - 1, attr);
	    screen_putchar(pum_border_chars.ساڄو, صف,
		    کانو + inner_ويڪر + 1, attr);
	    إذا (ساڄو_margin)
		screen_putchar(' ', صف, کانو + inner_ويڪر + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    فراغ
put_shadow_char(صحيح صف, صحيح کانو)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحيح	    shadow_attr = highlight_attr[HLF_PMS];
    صحيح	    char_attr, new_attr;

    screen_getbytes(صف, کانو, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), صف, کانو, new_attr);
}

/*
 * Draw the shadow
 */
    ساكن فراغ
pum_draw_shadow(فراغ)
{
    صحيح	صف, کانو, ايندڙ_کانو;
    صحيح	ويڪر, وٿي;
    صحيح	inner_ويڪر;
    صحيح	ساڄو_margin, کاٻو_margin, کاٻو_padding;

    إذا (!pum_shadow)
	أرجع;

    (فراغ)compute_margins(&ساڄو_margin, &کاٻو_margin, &کاٻو_padding);
    inner_ويڪر = pum_ويڪر + pum_scrollbar + pum_border + ساڄو_margin;

    // Draw right side shadow (left for RTL)
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	کانو = pum_کانو - inner_ويڪر;
	ايندڙ_کانو = کانو - 1;
    }
    وإلا
#جيڪڏهن_پورو
    {
	کانو = pum_کانو + inner_ويڪر;
	ايندڙ_کانو = کانو + 1;
    }

    لكل (صحيح i = 0; i < pum_اوچائي + pum_border; i++)
    {
	صف = pum_صف + 1 + i - pum_border;
	put_shadow_char(صف, کانو);
	put_shadow_char(صف, ايندڙ_کانو);
    }

    // Bottom shadow
    صف = pum_صف + pum_اوچائي + pum_border;
    ويڪر = inner_ويڪر + کاٻو_padding + pum_border + کاٻو_margin;
    وٿي = 2 - کاٻو_padding - pum_border - کاٻو_margin;

    لكل (صحيح i = 0; i < ويڪر; i++)
    {
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (pum_rl)
	    کانو = pum_کانو - وٿي - i;
	وإلا
#جيڪڏهن_پورو
	    کانو = pum_کانو + وٿي + i;
	put_shadow_char(صف, کانو);
    }
}
