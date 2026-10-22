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
ساكن صحيح pum_ناپ;			// nr of items in "pum_array"
ساكن صحيح pum_selected;		// index of selected item or -1
ساكن صحيح pum_پہلا = 0;		// index of top item

ساكن صحيح call_update_screen = FALSE;
ساكن صحيح pum_وچ_cmdline = FALSE;

ساكن صحيح pum_اچائی;			// nr of displayed pum items
ساكن صحيح pum_چوڑائی;			// width of displayed pum items
ساكن صحيح pum_ادھار_چوڑائی;		// width of pum items base
ساكن صحيح pum_قسم_چوڑائی;		// width of pum items kind column
ساكن صحيح pum_extra_چوڑائی;		// width of extra stuff
ساكن صحيح pum_scrollbar;		// TRUE when scrollbar present
#جے_تعریف_شدہ FEAT_RIGHTLEFT
ساكن صحيح pum_rl;			// TRUE when pum is drawn 'rightleft'
#جے_انت

ساكن صحيح pum_صف;			// top row of pum
ساكن صحيح pum_خانہ;			// left column of pum

ساكن win_T *pum_کھڑکی = عدم;
ساكن صحيح pum_win_صف;
ساكن صحيح pum_win_اچائی;
ساكن صحيح pum_win_خانہ;
ساكن صحيح pum_win_wcol;
ساكن صحيح pum_win_چوڑائی;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ساكن صحيح pum_pretend_not_visible = FALSE;

// Border support
ساكن صحيح pum_border = 0;
ساكن صحيح pum_margin = 0;	// margin of 1 cell on left and right
ساكن صحيح pum_shadow = 0;

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
ساكن image_placement_T *pum_imagep = عدم;
#جے_انت

// Border characters
ساكن بنية {
    صحيح چوٹی;
    صحيح دایاں;
    صحيح bottom;
    صحيح بایاں;
    صحيح چوٹی_بایاں;
    صحيح چوٹی_دایاں;
    صحيح bottom_دایاں;
    صحيح bottom_بایاں;
} pum_border_chars;

ساكن صحيح pum_سیٹ_selected(صحيح n, صحيح repeat);
ساكن فراغ pum_draw_border(فراغ);
ساكن فراغ pum_draw_shadow(فراغ);
ساكن فراغ compute_margins(صحيح *دایاں_margin, صحيح *بایاں_margin, صحيح *بایاں_padding);

#تعریف_کرو PUM_DEF_اچائی 10

#تعریف_کرو DEFINE_PUM_SETTER(feature) \
	فراغ \
    pum_سیٹ##feature(صحيح enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ساكن فراغ
pum_compute_ناپ(فراغ)
{
    صحيح	w;

    // Compute the width of the widest match and the widest extra.
    pum_ادھار_چوڑائی = 0;
    pum_قسم_چوڑائی = 0;
    pum_extra_چوڑائی = 0;
    لكل (صحيح i = 0; i < pum_ناپ; ++i)
    {
	إذا (pum_array[i].pum_متن != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_متن);
	    إذا (pum_ادھار_چوڑائی < w)
		pum_ادھار_چوڑائی = w;
	}
	إذا (pum_array[i].pum_قسم != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_قسم) + 1;
	    إذا (pum_قسم_چوڑائی < w)
		pum_قسم_چوڑائی = w;
	}
	إذا (pum_array[i].pum_extra != عدم)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    إذا (pum_extra_چوڑائی < w)
		pum_extra_چوڑائی = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ساكن فراغ
pum_compute_vertical_placement(
	صحيح ناپ,
	صحيح above_صف,
	صحيح ہیٹھاں_صف)
{
    صحيح context_سطریں;
    صحيح cline_visible_وتھ;
    صحيح extra_اچائی = 2 * pum_border + pum_shadow;
    صحيح extra_وتھ = pum_border + pum_shadow;

    pum_اچائی = MIN(ناپ, PUM_DEF_اچائی);
    إذا (p_ph > 0 && pum_اچائی > p_ph)
	pum_اچائی = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    إذا (pum_win_صف + 2 >= ہیٹھاں_صف - pum_اچائی - extra_اچائی
	    && pum_win_صف - above_صف > (ہیٹھاں_صف - above_صف) / 2)
    {
	// pum above "pum_win_row"
	إذا (State & منوال_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطریں = 0;
	وإلا
	    // Leave two lines of context if possible
	    context_سطریں = MIN(2, curwin->w_wrow - curwin->w_cline_صف);

	إذا (pum_win_صف >= ناپ + context_سطریں + extra_اچائی)
	{
	    pum_صف = pum_win_صف - ناپ - context_سطریں - extra_وتھ;
	    pum_اچائی = ناپ;
	}
	وإلا
	{
	    pum_صف = pum_border;
	    pum_اچائی = pum_win_صف - context_سطریں - extra_اچائی;
	}
	إذا (p_ph > 0 && pum_اچائی > p_ph)
	{
	    pum_صف += pum_اچائی - p_ph;
	    pum_اچائی = p_ph;
	}
    }
    وإلا
    {
	// pum below "pum_win_row"
	إذا (State & منوال_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_سطریں = 0;
	وإلا
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_وتھ = curwin->w_cline_صف +
		curwin->w_cline_اچائی - curwin->w_wrow;
	    context_سطریں = MIN(3, cline_visible_وتھ);
	}

	pum_صف = pum_win_صف + context_سطریں + pum_border;
	pum_اچائی = MIN(ہیٹھاں_صف - pum_صف - extra_وتھ, ناپ);
	إذا (p_ph > 0 && pum_اچائی > p_ph)
	    pum_اچائی = p_ph;
    }

#إذا defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    إذا (above_صف > 0 && pum_صف < above_صف && pum_اچائی > above_صف)
    {
	pum_صف = above_صف;
	pum_اچائی = pum_win_صف - above_صف;
    }
#جے_انت
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ساكن صحيح
سیٹ_pum_چوڑائی_aligned_with_cursor(صحيح چوڑائی, صحيح دستیاب_چوڑائی)
{
    صحيح سرا_padding = TRUE;

    إذا (چوڑائی < p_pw)
    {
	چوڑائی = p_pw;
	سرا_padding = FALSE;
    }
    إذا (p_pmw > 0 && چوڑائی >= p_pmw)
    {
	چوڑائی = p_pmw;
	سرا_padding = FALSE;
    }

    pum_چوڑائی = چوڑائی + (سرا_padding && چوڑائی >= p_pw ? 1 : 0);
    أرجع دستیاب_چوڑائی >= pum_چوڑائی;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ساكن فراغ
pum_compute_horizontal_placement(صحيح cursor_خانہ)
{
    صحيح desired_چوڑائی = pum_ادھار_چوڑائی + pum_قسم_چوڑائی + pum_extra_چوڑائی;
    صحيح دستیاب_چوڑائی;
    صحيح خانہ_وتھ;
    صحيح trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	خانہ_وتھ = pum_border && (cursor_خانہ >= Columns - 2)
	    ? (3 - Columns + cursor_خانہ) : 0;
	دستیاب_چوڑائی = cursor_خانہ + 1;
	pum_خانہ = cursor_خانہ - خانہ_وتھ;
    }
    وإلا
#جے_انت
    {
	خانہ_وتھ = pum_border && (cursor_خانہ <= 1) ? (2 - cursor_خانہ) : 0;
	دستیاب_چوڑائی = Columns - cursor_خانہ;
	pum_خانہ = cursor_خانہ + خانہ_وتھ;
    }

    دستیاب_چوڑائی -= trailing_overhead + خانہ_وتھ;

    // Case 1: Align pum with "cursor_col"
    إذا (سیٹ_pum_چوڑائی_aligned_with_cursor(desired_چوڑائی, دستیاب_چوڑائی))
	أرجع;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    إذا (دستیاب_چوڑائی > p_pw)
    {
	pum_چوڑائی = دستیاب_چوڑائی;
	أرجع;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    دستیاب_چوڑائی = Columns - trailing_overhead - خانہ_وتھ;

    إذا (دستیاب_چوڑائی > p_pw)
    {
	pum_چوڑائی = p_pw;  // Truncate beyond 'pum_width'
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    pum_خانہ = pum_چوڑائی + trailing_overhead;
	وإلا
#جے_انت
	    pum_خانہ = Columns - pum_چوڑائی - trailing_overhead;
	أرجع;
    }

    // Case 3: Not enough room anywhere, use what we have
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
	pum_خانہ = Columns - 1 - خانہ_وتھ;
    وإلا
#جے_انت
	pum_خانہ = خانہ_وتھ;
    pum_چوڑائی = Columns - trailing_overhead;
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
	صحيح	    ناپ,
	صحيح	    selected,   // index of initially selected item, -1 if
				// out of range
	صحيح	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    صحيح	    cursor_خانہ;
    صحيح	    above_صف;
    صحيح	    ہیٹھاں_صف;
    صحيح	    redo_گنتی = 0;
#إذا defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#جے_انت

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    pum_rl = (State & منوال_CMDLINE) == 0 && curwin->w_p_rl;
#جے_انت

    افعل
    {
	above_صف = 0;
	ہیٹھاں_صف = cmdline_صف;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_خانہ();
	pum_array = عدم;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_کھڑکی = curwin;
	إذا (State & منوال_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_صف = cmdline_صف;
	وإلا
	    pum_win_صف = curwin->w_wrow + W_WINROW(curwin);
	pum_win_اچائی = curwin->w_اچائی;
	pum_win_خانہ = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_چوڑائی = curwin->w_چوڑائی;

#إذا defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    إذا (pvwin->w_p_pvw)
		اكسر;
	إذا (pvwin != عدم)
	{
	    إذا (W_WINROW(pvwin) < W_WINROW(curwin))
		above_صف = W_WINROW(pvwin) + pvwin->w_اچائی;
	    وإلا إذا (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_اچائی)
		ہیٹھاں_صف = W_WINROW(pvwin);
	}
#جے_انت

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(ناپ, above_صف, ہیٹھاں_صف);

	// don't display when we only have room for one line
	إذا (pum_اچائی < 1 || (pum_اچائی == 1 && ناپ > 1))
	    أرجع;

	pum_array = array;
	pum_ناپ = ناپ;
	pum_compute_ناپ();

	// Calculate cursor column
	إذا (State & منوال_CMDLINE)
	    // cmdline completion popup menu
	    cursor_خانہ = cmdline_compl_startcol();
	وإلا
	{
	    صحيح wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    إذا (curwin->w_چوڑائی > 0)
		wcol %= curwin->w_چوڑائی;
	    وإلا
		wcol = 0;
#جے_تعریف_شدہ FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    إذا (curwin->w_p_cole > 0 && conceal_cursor_سطر(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		إذا (wcol < 0)
		    wcol = 0;
	    }
#جے_انت
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    إذا (pum_rl)
		cursor_خانہ = curwin->w_wincol + curwin->w_چوڑائی - wcol - 1;
	    وإلا
#جے_انت
		cursor_خانہ = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_اچائی < ناپ) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_خانہ);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } طالما (pum_سیٹ_selected(selected, redo_گنتی) && ++redo_گنتی <= 2);

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
pum_under_menu(صحيح صف, صحيح خانہ, صحيح only_redrawing)
{
    صحيح	extra_بایاں = pum_border + (pum_margin && pum_border ? 1 : 0);
    صحيح	extra_دایاں = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    صحيح	extra_above = pum_border;
    صحيح	extra_ہیٹھاں = pum_border + (pum_shadow ? 1 : 0);
    صحيح	چوٹی = pum_صف - extra_above;
    صحيح	bot = pum_صف + pum_اچائی + extra_ہیٹھاں;
    صحيح	بایاں = pum_خانہ - 1 - extra_بایاں;
    صحيح	دایاں = pum_خانہ + pum_چوڑائی + pum_scrollbar + extra_دایاں;

    إذا (!((!only_redrawing || pum_will_redraw)
	    && صف >= چوٹی && صف < bot && خانہ >= بایاں && خانہ < دایاں))
	أرجع FALSE;

    إذا (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	إذا (only_redrawing)
	{
	    إذا (خانہ >= دایاں - 2 || صف == bot - 1)
		أرجع FALSE;
	}
	وإلا
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    صحيح	    دایاں_margin, بایاں_margin, بایاں_padding;

	    compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);
	    إذا (صف == چوٹی && خانہ >= دایاں - 2)
		أرجع FALSE;
	    إذا (صف == bot - 1 && خانہ < pum_خانہ + 2 - بایاں_padding - pum_border
		    - بایاں_margin)
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
    صحيح		وچ_fuzzy;
    صحيح		matched_len = -1;
    int_u	char_pos = 0;
    صحيح		is_select = FALSE;

    إذا (*متن == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	أرجع عدم;

    is_select = hlf == HLF_PSI;
    leader = (State & منوال_CMDLINE) ? cmdline_compl_نمونہ()
							  : ins_compl_leader();
    إذا (leader == عدم || *leader == NUL)
	أرجع عدم;

    attrs = ALLOC_MULT(صحيح, vim_strsize(متن));
    إذا (attrs == عدم)
	أرجع عدم;

    وچ_fuzzy = (State & منوال_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    إذا (وچ_fuzzy)
    {
	ga = fuzzy_میل_str_with_pos(متن, leader);
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
		إذا (char_pos == ((int_u *)ga->ga_انکڑے)[i])
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
    صحيح		خانہ,
    صحيح		cells بن_ورتیا,
    char_u	*متن,
    صحيح		textlen,
    صحيح		*attrs)
{
    صحيح		خانہ_شروع = خانہ;
    char_u	*ptr = متن;
    صحيح		char_len;
    صحيح		attr;

    // Render text with proper attributes
    طالما (*ptr != NUL && ptr < متن + textlen)
    {
	char_len = mb_ptr2len(ptr);
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    attr = attrs[خانہ_شروع + cells - خانہ - 1];
	وإلا
#جے_انت
	    attr = attrs[خانہ - خانہ_شروع];
	screen_puts_len(ptr, char_len, صف, خانہ, attr);
	خانہ += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ساكن مضمن char_u *
pum_get_شے(صحيح اشاریہ, صحيح type)
{
    بدل(type)
    {
	حالة CPT_ABBR: أرجع pum_array[اشاریہ].pum_متن;
	حالة CPT_قسم: أرجع pum_array[اشاریہ].pum_قسم;
	حالة CPT_MENU: أرجع pum_array[اشاریہ].pum_extra;
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

#جے_تعریف_شدہ FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_rtl_متن(
	صحيح	صف,
	صحيح     خانہ,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     چوڑائی,
	صحيح     چوڑائی_حد_بندی,
	صحيح     totwidth,
	صحيح     اگلا_isempty,
	صحيح	selected)
{
    char_u  *rt = عدم;
    صحيح     cells = 0;
    صحيح     over_cell = 0;
    صحيح     truncated = FALSE;
    صحيح     pad = اگلا_isempty ? 0 : 2;
    صحيح     بچیا = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    إذا (st == عدم)
	أرجع خانہ;

    rt = الٹ_متن(st);
    إذا (rt == عدم)
    {
	VIM_CLEAR(st);
	أرجع خانہ;
    }

    char_u *rt_شروع = rt;
    cells = mb_string2cells(rt, -1);
    truncated = چوڑائی_حد_بندی - totwidth < cells + pad;

    // only draw the text that fits
    إذا (cells > چوڑائی_حد_بندی)
    {
	افعل
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} طالما (cells > چوڑائی_حد_بندی);

	إذا (cells < چوڑائی_حد_بندی)
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
	صحيح     ناپ = 0;

	بچیا = چوڑائی_حد_بندی - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	إذا (cells > بچیا)
	{
	    طالما (cells > بچیا)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	ناپ = (صحيح)STRLEN(orig_rt);
	إذا (cells < بچیا)
	    over_cell = بچیا - cells;

	cells = mb_string2cells(orig_rt, ناپ);
	چوڑائی = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, صف, خانہ - چوڑائی + 1, trunc_attr);

	إذا (over_cell > 0)
	    screen_fill(صف, صف + 1, خانہ - چوڑائی + 2,
		    خانہ - چوڑائی + 2 + over_cell, ' ', ' ', attr);
    }

    إذا (attrs == عدم)
	screen_puts_len(rt, (صحيح)STRLEN(rt), صف, خانہ - cells + 1, attr);
    وإلا
	pum_screen_puts_with_attrs(صف, خانہ - cells + 1, cells, rt,
		(صحيح)STRLEN(rt), attrs);

    vim_free(rt_شروع);
    VIM_CLEAR(st);
    أرجع خانہ - چوڑائی;
}
#جے_انت

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ساكن صحيح
pum_display_ltr_متن(
	صحيح     صف,
	صحيح     خانہ,
	char_u  *st,
	صحيح     attr,
	صحيح     *attrs,
	صحيح     چوڑائی,        // width already calculated in outer loop
	صحيح     چوڑائی_حد_بندی,
	صحيح     totwidth,
	صحيح     اگلا_isempty,
	صحيح	selected)
{
    صحيح     ناپ = 0;
    صحيح     cells = 0;
    char_u  *st_سرا = عدم;
    صحيح     over_cell = 0;
    صحيح     pad = اگلا_isempty ? 0 : 2;
    صحيح     truncated = FALSE;
    صحيح     بچیا = 0;
    صحيح	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    صحيح	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    إذا (st == عدم)
	أرجع خانہ;

    ناپ = (صحيح)STRLEN(st);
    cells = (*mb_string2cells)(st, ناپ);
    truncated = چوڑائی_حد_بندی - totwidth < cells + pad;

    // only draw the text that fits
    طالما (ناپ > 0 && خانہ + cells > چوڑائی_حد_بندی + pum_خانہ)
    {
	--ناپ;
	إذا (has_mbyte)
	{
	    ناپ -= (*mb_سر_off)(st, st + ناپ);
	    cells -= (*mb_ptr2cells)(st + ناپ);
	}
	وإلا
	    --cells;
    }

    // truncated
    إذا (truncated)
    {
	بچیا = چوڑائی_حد_بندی - totwidth - 1;
	إذا (cells > بچیا)
	{
	    st_سرا = st + ناپ;
	    طالما (st_سرا > st && cells > بچیا)
	    {
		MB_PTR_پٹھ(st, st_سرا);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_سرا) : 1;
	    }
	    ناپ = st_سرا - st;
	}

	إذا (cells < بچیا)
	    over_cell = بچیا - cells;
	cells = mb_string2cells(st, ناپ);
	چوڑائی = cells + over_cell + 1;
    }

    إذا (attrs == عدم)
	screen_puts_len(st, ناپ, صف, خانہ, attr);
    وإلا
	pum_screen_puts_with_attrs(صف, خانہ, cells, st, ناپ, attrs);

    إذا (truncated)
    {
	إذا (over_cell > 0)
	    screen_fill(صف, صف + 1, خانہ + cells,
		    خانہ + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, صف, خانہ + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    أرجع خانہ + چوڑائی;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ساكن صحيح
pum_process_شے(
	صحيح     صف,
	صحيح     خانہ,
	صحيح     idx,
	صحيح     j,         // Current position in order array
	صحيح     *ترتیب,    // Order array
	hlf_T   hlf,
	صحيح     attr,
	صحيح     *totwidth_ptr,
	صحيح     اگلا_isempty)
{
    صحيح     شے_type = ترتیب[j];
    char_u  *s = عدم;
    char_u  *p = pum_get_شے(idx, شے_type);
    صحيح     چوڑائی = 0;  // item width
    صحيح     w;		// char width
    صحيح	    selected = idx == pum_selected;

    لكل ( ; ; MB_PTR_ADV(p))
    {
	إذا (s == عدم)
	    s = p;
	w = ptr2cells(p);
	إذا (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_چوڑائی)
	{
	    چوڑائی += w;
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

	إذا (شے_type == CPT_ABBR)
	    attrs = pum_compute_متن_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    خانہ = pum_display_rtl_متن(صف, خانہ, st, attr, attrs,
		    چوڑائی, pum_چوڑائی, *totwidth_ptr, اگلا_isempty, selected);
	وإلا
#جے_انت
	    خانہ = pum_display_ltr_متن(صف, خانہ, st, attr, attrs,
		    چوڑائی, pum_چوڑائی, *totwidth_ptr, اگلا_isempty, selected);

	إذا (attrs != عدم)
	    VIM_CLEAR(attrs);

	إذا (*p != TAB)
	    اكسر;

	// Display two spaces for a Tab.
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, صف, خانہ - 1, attr);
	    خانہ -= 2;
	}
	وإلا
#جے_انت
	{
	    screen_puts_len((char_u *)"  ", 2, صف, خانہ, attr);
	    خانہ += 2;
	}
	*totwidth_ptr += 2;
	s = عدم;  // start text at next char
	چوڑائی = 0;
    }

    أرجع خانہ;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ساكن فراغ
pum_draw_scrollbar(
	صحيح	صف,
	صحيح	i,
	صحيح	thumb_pos,
	صحيح	thumb_اچائی)
{
    إذا (pum_scrollbar <= 0)
	أرجع;

    صحيح attr = (i >= thumb_pos && i < thumb_pos + thumb_اچائی) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
	screen_putchar(' ', صف, pum_خانہ - pum_چوڑائی, attr);
    وإلا
#جے_انت
	screen_putchar(' ', صف, pum_خانہ + pum_چوڑائی, attr);
}

    ساكن مضمن فراغ
pum_align_ترتیب(صحيح *ترتیب)
{
    صحيح is_default = cia_flags == 0;
    ترتیب[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ترتیب[1] = is_default ? CPT_قسم : (cia_flags / 10) % 10;
    ترتیب[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ساكن فراغ pum_free_bg(فراغ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    فراغ
pum_opacity_بدلیا(فراغ)
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
    VIM_CLEAR(pum_bg_سطریں);
    VIM_CLEAR(pum_bg_سطریں_UC);
    لكل (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_سطریں_C[k]);
    pum_bg_چوٹی = 0;
    pum_bg_bot = 0;
    pum_bg_خانے_گنتی = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    فراغ
pum_redraw(فراغ)
{
    صحيح		صف = pum_صف;
    صحيح		خانہ;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    صحيح		attr;
    صحيح		i, j;
    صحيح		idx;
    char_u	*p = عدم;
    صحيح		totwidth;
    صحيح		thumb_pos = 0;
    صحيح		thumb_اچائی = 1;
    صحيح		شے_type;
    صحيح		ترتیب[3];
    صحيح		اگلا_isempty = FALSE;
    صحيح		n;
    صحيح		اشیاء_چوڑائی_array[3] = { pum_ادھار_چوڑائی, pum_قسم_چوڑائی,
							    pum_extra_چوڑائی };
    صحيح		basic_چوڑائی;  // first item width
    صحيح		آخری_isabbr = FALSE;
    صحيح		orig_attr = -1;
    صحيح		scroll_range = pum_ناپ - pum_اچائی;
    bool	override_success;
    صحيح		opacity_سرگرم = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ڈالو_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    إذا (pum_imagep == عدم)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(عدم, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    إذا (pum_imagep != عدم)
    {
	image_placement_سیٹ_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_سیٹ_position(pum_imagep, pum_صف, pum_خانہ);
	image_placement_سیٹ_bounding_box(pum_imagep, pum_صف, pum_خانہ,
		pum_اچائی, pum_چوڑائی);
	image_placement_سیٹ_enable(pum_imagep, true);
    }
#جے_انت

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
	إذا (opacity_سرگرم && pum_bg_سطریں != عدم
		&& (pum_bg_خانے_گنتی != screen_Columns
		    || pum_bg_bot > screen_صفاں))
	    pum_free_bg();

	إذا (opacity_سرگرم && pum_bg_سطریں != عدم)
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
	    إذا (Screen_سطریں != عدم && ScreenAttrs != عدم)
	    {
		صحيح	save_چوٹی, save_bot, save_ncells, k;

		pum_free_bg();
		save_چوٹی = pum_صف - pum_border;
		save_bot = pum_صف + pum_اچائی + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		إذا (save_چوٹی < 0)
		    save_چوٹی = 0;
		إذا (save_bot > screen_صفاں)
		    save_bot = screen_صفاں;
		pum_bg_چوٹی = save_چوٹی;
		pum_bg_bot = save_bot;
		pum_bg_خانے_گنتی = screen_Columns;
		إذا (save_چوٹی < save_bot)
		{
		    save_ncells = (save_bot - save_چوٹی) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_سطریں = LALLOC_MULT(schar_T, save_ncells);
		    إذا (enc_utf8)
		    {
			pum_bg_سطریں_UC = LALLOC_MULT(u8char_T, save_ncells);
			لكل (k = 0; k < MAX_MCO; ++k)
			    pum_bg_سطریں_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    إذا (pum_bg_attrs != عدم && pum_bg_سطریں != عدم)
		    {
			لكل (صحيح r = save_چوٹی; r < save_bot; ++r)
			{
			    صحيح soff = (r - save_چوٹی) * screen_Columns;
			    صحيح loff = سطر_وتھ[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * حجم(sattr_T));
			    mch_memmove(pum_bg_سطریں + soff,
				    Screen_سطریں + loff,
				    screen_Columns * حجم(schar_T));
			    إذا (enc_utf8 && pum_bg_سطریں_UC != عدم
						&& Screen_سطریں_UC != عدم)
			    {
				mch_memmove(pum_bg_سطریں_UC + soff,
					Screen_سطریں_UC + loff,
					screen_Columns * حجم(u8char_T));
				لكل (k = 0; k < MAX_MCO; ++k)
				    إذا (pum_bg_سطریں_C[k] != عدم
						    && Screen_سطریں_C[k] != عدم)
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
    pum_پہلا = MIN(pum_پہلا, scroll_range);

    إذا (pum_scrollbar)
    {
	thumb_اچائی = pum_اچائی * pum_اچائی / pum_ناپ;
	إذا (thumb_اچائی == 0)
	    thumb_اچائی = 1;
	thumb_pos = (pum_پہلا * (pum_اچائی - thumb_اچائی)
			    + scroll_range / 2) / scroll_range;
    }

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#جے_انت

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

    لكل (i = 0; i < pum_اچائی; ++i)
    {
	idx = i + pum_پہلا;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    إذا (pum_خانہ < curwin->w_wincol + curwin->w_چوڑائی - 1
							    - pum_border)
		screen_putchar(' ', صف, pum_خانہ + 1, attr);
	}
	وإلا
#جے_انت
	    إذا (pum_خانہ > pum_border)
		screen_putchar(' ', صف, pum_خانہ - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	خانہ = pum_خانہ;
	totwidth = 0;
	pum_align_ترتیب(ترتیب);
	basic_چوڑائی = اشیاء_چوڑائی_array[ترتیب[0]];
	آخری_isabbr = ترتیب[2] == CPT_ABBR;
	لكل (j = 0; j < 3; ++j)
	{
	    شے_type = ترتیب[j];
	    hlf = hlfs[شے_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    إذا (شے_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, شے_type, attr);
	    p = pum_get_شے(idx, شے_type);

	    إذا (j + 1 < 3)
		اگلا_isempty = pum_get_شے(idx, ترتیب[j + 1]) == عدم;
	    وإلا
		اگلا_isempty = TRUE;

	    إذا (p != عدم)
		// Process and display the item
		خانہ = pum_process_شے(صف, خانہ, idx, j, ترتیب, hlf, attr,
						    &totwidth, اگلا_isempty);

	    إذا (j > 0)
		n = اشیاء_چوڑائی_array[ترتیب[1]] + (آخری_isabbr ? 0 : 1);
	    وإلا
		n = ترتیب[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    إذا (j == 2
		    || (اگلا_isempty && (j == 1 || (j == 0
				&& pum_get_شے(idx, ترتیب[j + 2]) == عدم)))
		    || basic_چوڑائی + n >= pum_چوڑائی)
		اكسر;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    إذا (pum_rl)
	    {
		screen_fill(صف, صف + 1, pum_خانہ - basic_چوڑائی - n + 1,
						خانہ + 1, ' ', ' ', orig_attr);
		خانہ = pum_خانہ - basic_چوڑائی - n;
	    }
	    وإلا
#جے_انت
	    {
		screen_fill(صف, صف + 1, خانہ, pum_خانہ + basic_چوڑائی + n,
							' ', ' ', orig_attr);
		خانہ = pum_خانہ + basic_چوڑائی + n;
	    }
	    totwidth = basic_چوڑائی + n;
	}

#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    screen_fill(صف, صف + 1, pum_خانہ - pum_چوڑائی + 1, خانہ + 1,
							    ' ', ' ', orig_attr);
	وإلا
#جے_انت
	    screen_fill(صف, صف + 1, خانہ, pum_خانہ + pum_چوڑائی, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(صف, i, thumb_pos, thumb_اچائی);

	++صف;
    }

    screen_pum_blend = 0;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    screen_zindex = 0;
#جے_انت

    إذا (override_success)
	نکالو_highlight_overrides();

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    فراغ
pum_position_info_popup(win_T *wp)
{
    صحيح خانہ = pum_خانہ + pum_چوڑائی + pum_scrollbar + 1;
    صحيح صف = pum_صف;
    صحيح botpos = POPPOS_BOTLEFT;
    صحيح	ورتیا_maxwidth_opt = FALSE;
    صحيح	دایاں_margin, بایاں_margin, dummy;
    صحيح بایاں_extra_چوڑائی, دایاں_extra_چوڑائی;

    (فراغ)compute_margins(&دایاں_margin, &بایاں_margin, &dummy);

    بایاں_extra_چوڑائی = pum_border + بایاں_margin;
    دایاں_extra_چوڑائی = pum_border + دایاں_margin + (pum_shadow ? 2 : 0);
    خانہ += دایاں_extra_چوڑائی;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    إذا (Columns - خانہ < 20 && Columns - خانہ < pum_خانہ)
    {
	خانہ = pum_خانہ - 1 - بایاں_extra_چوڑائی;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_خانہ - 1 - بایاں_extra_چوڑائی;
    }
    وإلا
	wp->w_maxwidth = Columns - خانہ + 1;
    wp->w_maxwidth -= popup_extra_چوڑائی(wp);
    إذا (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	ورتیا_maxwidth_opt = TRUE;
    }

    صف -= popup_چوٹی_extra(wp);
    إذا (wp->w_popup_flags & POPF_INFO_MENU)
    {
	إذا (pum_صف < pum_win_صف)
	{
	    // menu above cursor line, align with bottom
	    صف += pum_اچائی;
	    wp->w_popup_pos = botpos;
	}
	وإلا
	    // menu below cursor line, align with top
	    صف += 1;
    }
    وإلا
	// align with the selected item
	صف += pum_selected - pum_پہلا + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (wp->w_maxwidth < 10 && !ورتیا_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    وإلا
	popup_سیٹ_wantpos_rowcol(wp, صف, خانہ);
}
#جے_انت

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
pum_سیٹ_selected(صحيح n, صحيح repeat بن_ورتیا)
{
    صحيح	    resized = FALSE;
    صحيح	    context = pum_اچائی / 2;
    صحيح	    scroll_وتھ;
#جے_تعریف_شدہ FEAT_QUICKFIX
    صحيح	    prev_selected = pum_selected;
    بنانشانی	cur_cot_flags = get_cot_flags();
#جے_انت
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    صحيح	    has_info = FALSE;
#جے_انت

    pum_selected = n;
    scroll_وتھ = pum_selected - pum_اچائی;

    إذا (pum_selected >= 0 && pum_selected < pum_ناپ)
    {
	إذا (pum_پہلا > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    إذا (pum_پہلا > pum_selected - 2)
	    {
		pum_پہلا -= pum_اچائی - 2;
		إذا (pum_پہلا < 0)
		    pum_پہلا = 0;
		وإلا إذا (pum_پہلا > pum_selected)
		    pum_پہلا = pum_selected;
	    }
	    وإلا
		pum_پہلا = pum_selected;
	}
	وإلا إذا (pum_پہلا < scroll_وتھ + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    إذا (pum_پہلا < scroll_وتھ + 3)
		pum_پہلا = MAX(pum_پہلا + pum_اچائی - 2, scroll_وتھ + 1);
	    وإلا
		pum_پہلا = scroll_وتھ + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	إذا (pum_اچائی > 2)
	{
	    إذا (pum_پہلا > pum_selected - context)
		pum_پہلا = MAX(pum_selected - context, 0);  // scroll down
	    وإلا إذا (pum_پہلا < pum_selected + context - pum_اچائی + 1)
		pum_پہلا = pum_selected + context - pum_اچائی + 1;  //اتے
	}
	// adjust for the number of lines displayed
	pum_پہلا = MIN(pum_پہلا, pum_ناپ - pum_اچائی);

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
		&& صفاں > 10
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
		بلاک_autocmds();
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
			سیٹ_option_قدر_give_err((char_u *)"swf",
							  0L, عدم, OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bl",
							  0L, عدم, OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			سیٹ_option_قدر_give_err((char_u *)"diff",
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
		    ml_delete(curbuf->b_ml.ml_سطر_گنتی);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    إذا (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			إذا (curwin->w_اچائی < lnum)
			{
			    win_setheight((صحيح)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_بدلیا = 0;
		    curbuf->b_p_ma = FALSE;
		    إذا (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    وإلا إذا (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
			curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.خانہ = 0;
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
#جے_انت
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    إذا (!has_info)
	// hide any popup info window
	popup_hide_info();
#جے_انت

    أرجع resized;
}

/*
 * Undisplay the popup menu (later).
 */
    فراغ
pum_undisplay(فراغ)
{
#جے_تعریف_شدہ FEAT_IMAGE_POPUP
    إذا (pum_imagep != عدم)
    {
	image_placement_free(pum_imagep);
	pum_imagep = عدم;
    }
#جے_انت
    pum_free_bg();
    pum_array = عدم;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
    إذا (pum_وچ_cmdline)
    {
	clear_cmdline = TRUE;
	pum_وچ_cmdline = FALSE;
    }
    حالت_redraw_all();
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#جے_انت
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    فراغ
pum_clear(فراغ)
{
    pum_پہلا = 0;
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
pum_وچ_same_position(فراغ)
{
    صحيح	    صف = (State & منوال_CMDLINE)
			? cmdline_صف
			: curwin->w_wrow + W_WINROW(curwin);

    أرجع pum_کھڑکی != curwin
	    || (pum_win_صف == صف
		&& pum_win_اچائی == curwin->w_اچائی
		&& pum_win_خانہ == curwin->w_wincol
		&& pum_win_چوڑائی == curwin->w_چوڑائی);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    صحيح
pum_redraw_وچ_same_position(فراغ)
{
    إذا (!pum_visible() || pum_will_redraw)
	أرجع FALSE;  // nothing to do

    أرجع pum_وچ_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    فراغ
pum_may_redraw(فراغ)
{
    pumitem_T	*array = pum_array;
    صحيح		len = pum_ناپ;
    صحيح		selected = pum_selected;

    إذا (!pum_visible() || pum_will_redraw)
	أرجع;  // nothing to do

    إذا (pum_وچ_same_position())
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
pum_get_اچائی(فراغ)
{
    أرجع pum_اچائی;
}

#إذا defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    فراغ
pum_سیٹ_event_info(dict_T *dict)
{
    إذا (!pum_visible())
	أرجع;
    (فراغ)dict_جوڑو_نمبر(dict, "height", pum_اچائی);
    (فراغ)dict_جوڑو_نمبر(dict, "width", pum_چوڑائی);
    (فراغ)dict_جوڑو_نمبر(dict, "row", pum_صف);
    (فراغ)dict_جوڑو_نمبر(dict, "col", pum_خانہ);
    (فراغ)dict_جوڑو_نمبر(dict, "size", pum_ناپ);
    (فراغ)dict_جوڑو_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#جے_انت

#إذا defined(FEAT_BEVAL_رکن) || defined(FEAT_رکن_POPUP_MENU)
    ساكن فراغ
pum_position_at_mouse(صحيح min_چوڑائی)
{
    صحيح extra_اچائی = pum_border * 2 + pum_shadow;
    صحيح extra_وتھ = pum_border + pum_shadow;
    صحيح trailing_overhead = 0;
    صحيح خانہ_وتھ = 0;
    صحيح دستیاب_چوڑائی;

    إذا (صفاں - mouse_صف - extra_اچائی > pum_ناپ
	    || صفاں - mouse_صف > mouse_صف)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_صف = mouse_صف + 1 + pum_border;
	إذا (pum_اچائی > صفاں - pum_صف - extra_اچائی)
	    pum_اچائی = صفاں - pum_صف - extra_اچائی;
	إذا (pum_صف + pum_اچائی + extra_وتھ >= cmdline_صف)
	    pum_وچ_cmdline = TRUE;
    }
    وإلا
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_صف = mouse_صف - pum_ناپ - extra_وتھ;
	إذا (pum_صف < pum_border)
	{
	    pum_اچائی += pum_صف - pum_border;
	    pum_صف = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	خانہ_وتھ = (mouse_خانہ == Columns - 1) && pum_border;
	دستیاب_چوڑائی = mouse_خانہ + 1 - trailing_overhead - خانہ_وتھ;
	إذا (دستیاب_چوڑائی >= pum_ادھار_چوڑائی || دستیاب_چوڑائی > min_چوڑائی)
	    // Enough space to show at mouse column.
	    pum_خانہ = mouse_خانہ - خانہ_وتھ;
	وإلا
	    // Not enough space, left align with window.
	    pum_خانہ = MIN(pum_ادھار_چوڑائی, min_چوڑائی) - 1 + trailing_overhead;
	pum_چوڑائی = pum_خانہ + 1 - trailing_overhead;
    }
    وإلا
# endif
    {
	خانہ_وتھ = (mouse_خانہ == 0) && pum_border;
	دستیاب_چوڑائی = Columns - mouse_خانہ - trailing_overhead - خانہ_وتھ;
	إذا (دستیاب_چوڑائی >= pum_ادھار_چوڑائی || دستیاب_چوڑائی > min_چوڑائی)
	    // Enough space to show at mouse column.
	    pum_خانہ = mouse_خانہ + خانہ_وتھ;
	وإلا
	    // Not enough space, right align with window.
	    pum_خانہ = Columns - MIN(pum_ادھار_چوڑائی, min_چوڑائی)
		- trailing_overhead;
	pum_چوڑائی = Columns - pum_خانہ - trailing_overhead;
    }

    pum_چوڑائی = MIN(pum_چوڑائی, pum_ادھار_چوڑائی + 1);
    // Do not redraw at cursor position.
    pum_کھڑکی = عدم;
}

#جے_انت

#إذا defined(FEAT_BEVAL_رکن)
ساكن pumitem_T *balloon_array = عدم;
ساكن صحيح balloon_arraysize;

# define BALLOON_MIN_چوڑائی 50
# define BALLOON_MIN_اچائی 10

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
ونڈو_سنیہا(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*شے;
    صحيح		quoted = FALSE;
    صحيح		اچائی;
    صحيح		سطر;
    صحيح		شے_idx;
    صحيح		indent = 0;
    صحيح		max_cells = 0;
    صحيح		max_اچائی_2 = صفاں / 2 - 1;
    صحيح		long_شے_گنتی = 0;
    صحيح		ونڈو_long_اشیاء = FALSE;

    ga_init2(&ga, حجم(balpart_T), 20);
    p = mesg;

    طالما (*p != NUL)
    {
	إذا (ga_grow(&ga, 1) == FAIL)
	    اذهب failed;
	شے = ((balpart_T *)ga.ga_انکڑے) + ga.ga_len;
	شے->شروع = p;
	شے->indent = indent;
	شے->cells = indent * 2;
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
		    ++شے->cells;
		    p = skipwhite(p + 1);
		    اكسر;
		}
	    }
	    شے->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	شے->bytelen = p - شے->شروع;
	إذا (*p == '\n')
	    ++p;
	إذا (شے->cells > max_cells)
	    max_cells = شے->cells;
	long_شے_گنتی += (شے->cells - 1) / BALLOON_MIN_چوڑائی;
    }

    اچائی = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    إذا (long_شے_گنتی > 0 && اچائی + long_شے_گنتی <= max_اچائی_2)
    {
	ونڈو_long_اشیاء = TRUE;
	اچائی += long_شے_گنتی;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    إذا (اچائی > max_اچائی_2)
	اچائی = max_اچائی_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, اچائی);
    إذا (*array == عدم)
	اذهب failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_متن = vim_strsave((char_u *)"");
    (*array + اچائی - 1)->pum_متن = vim_strsave((char_u *)"");

    لكل (سطر = 1, شے_idx = 0; سطر < اچائی - 1; ++شے_idx)
    {
	صحيح	skip;
	صحيح	thislen;
	صحيح	copylen;
	صحيح	ind;
	صحيح	cells;

	شے = ((balpart_T *)ga.ga_انکڑے) + شے_idx;
	إذا (شے->bytelen == 0)
	    (*array)[سطر++].pum_متن = vim_strsave((char_u *)"");
	وإلا
	    لكل (skip = 0; skip < شے->bytelen; skip += thislen)
	    {
		إذا (ونڈو_long_اشیاء && شے->cells >= BALLOON_MIN_چوڑائی)
		{
		    cells = شے->indent * 2;
		    لكل (p = شے->شروع + skip;
			    p < شے->شروع + شے->bytelen;
							    p += mb_ptr2len(p))
			إذا ((cells += ptr2cells(p)) > BALLOON_MIN_چوڑائی)
			    اكسر;
		    thislen = p - (شے->شروع + skip);
		}
		وإلا
		    thislen = شے->bytelen;

		// put indent at the start
		p = alloc(thislen + شے->indent * 2 + 1);
		إذا (p == عدم)
		{
		    لكل (سطر = 0; سطر <= اچائی - 1; ++سطر)
			vim_free((*array)[سطر].pum_متن);
		    vim_free(*array);
		    اذهب failed;
		}
		لكل (ind = 0; ind < شے->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		لكل (copylen = thislen; copylen > 0; --copylen)
		    إذا (شے->شروع[skip + copylen - 1] != ' ')
			اكسر;

		vim_strncpy(p + ind, شے->شروع + skip, copylen);
		(*array)[سطر].pum_متن = p;
		شے->indent = 0;  // wrapped line has no indent
		++سطر;
	    }
    }
    ga_clear(&ga);
    أرجع اچائی;

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
	لكل (idx = 0, li = list->lv_پہلا; li != عدم; li = li->li_اگلا, ++idx)
	{
	    char_u *متن = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_متن = vim_strsave(
					   متن == عدم ? (char_u *)"" : متن);
	}
    }
    وإلا
	balloon_arraysize = ونڈو_سنیہا(mesg, &balloon_array);

    إذا (balloon_arraysize <= 0)
	أرجع;

    pum_array = balloon_array;
    pum_ناپ = balloon_arraysize;
    pum_compute_ناپ();
    pum_scrollbar = 0;
    pum_اچائی = balloon_arraysize;
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
    فراغ
ui_may_remove_balloon(فراغ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#جے_انت

#إذا defined(FEAT_رکن_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ساكن فراغ
pum_select_mouse_pos(فراغ)
{
    صحيح idx = mouse_صف - pum_صف;

    إذا (idx < 0 || idx >= pum_اچائی)
	pum_selected = -1;
    وإلا إذا (*pum_array[idx].pum_متن != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ساكن فراغ
pum_execute_menu(vimmenu_T *menu, صحيح منوال)
{
    vimmenu_T   *mp;
    صحيح		idx = 0;
    exarg_T	ea;

    FOR_ALL_بچہ_MENUS(menu, mp)
	إذا ((mp->modes & mp->enabled & منوال) && idx++ == pum_selected)
	{
	    CLEAR_میدان(ea);
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
# ifdef FEAT_BEVAL_رکن
    صحيح		save_bevalterm = p_bevalterm;
# endif
    صحيح		منوال;

    pum_undisplay();
    pum_ناپ = 0;
    منوال = get_menu_منوال_flag();

    FOR_ALL_بچہ_MENUS(menu, mp)
	إذا (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & منوال))
	    ++pum_ناپ;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    إذا (pum_ناپ <= 0)
    {
	emsg(_(e_menu_only_exists_وچ_another_منوال));
	أرجع;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_ناپ);
    إذا (array == عدم)
	أرجع;

    FOR_ALL_بچہ_MENUS(menu, mp)
    {
	char_u *s = عدم;

	// Make a copy of the text, the menu may be redefined in a callback.
	إذا (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	وإلا إذا (mp->modes & mp->enabled & منوال)
	    s = mp->dname;
	إذا (s != عدم)
	{
	    s = vim_strsave(s);
	    إذا (s != عدم)
		array[idx++].pum_متن = s;
	}
    }

    pum_array = array;
    pum_compute_ناپ();
    pum_scrollbar = 0;
    pum_اچائی = pum_ناپ;
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

    لكل (;;)
    {
	صحيح	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	باہرلا_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	إذا (c == ESC || c == Ctrl_C || pum_array == عدم)
	    اكسر;
	وإلا إذا (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, منوال);
	    اكسر;
	}
	وإلا إذا (c == 'k' || c == K_اتے || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    طالما (pum_selected > 0)
	    {
		--pum_selected;
		إذا (*array[pum_selected].pum_متن != NUL)
		    اكسر;
	    }
	}
	وإلا إذا (c == 'j' || c == K_تھلے || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    طالما (pum_selected < pum_ناپ - 1)
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
		pum_execute_menu(menu, منوال);
		اكسر;
	    }
	    إذا (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		اكسر;
	}
    }

    لكل (idx = 0; idx < pum_ناپ; ++idx)
	vim_free(array[idx].pum_متن);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_رکن
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    فراغ
pum_make_popup(char_u *رستہ_name, صحيح use_mouse_pos)
{
    vimmenu_T *menu;

    إذا (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_صف = W_WINROW(curwin) + curwin->w_wrow;
	mouse_خانہ =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_لبھو_menu(رستہ_name);
    إذا (menu != عدم)
	pum_show_popupmenu(menu);
}
#جے_انت

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
pum_سیٹ_border_chars(
	صحيح چوٹی,
	صحيح دایاں,
	صحيح bottom,
	صحيح بایاں,
	صحيح چوٹی_بایاں,
	صحيح چوٹی_دایاں,
	صحيح bottom_دایاں,
	صحيح bottom_بایاں)
{
    إذا (!is_singlewidth(چوٹی) || !is_singlewidth(دایاں)
	    || !is_singlewidth(bottom) || !is_singlewidth(بایاں)
	    || !is_singlewidth(چوٹی_بایاں) || !is_singlewidth(چوٹی_دایاں)
	    || !is_singlewidth(bottom_دایاں) || !is_singlewidth(bottom_بایاں))
	أرجع;
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
    ساكن فراغ
compute_margins(صحيح *دایاں_margin, صحيح *بایاں_margin, صحيح *بایاں_padding)
{
    *دایاں_margin = pum_margin && pum_border;

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	*بایاں_padding = (pum_خانہ < Columns - 1 - pum_border);
	*بایاں_margin = *دایاں_margin && (pum_خانہ < Columns - 3);
	أرجع;
    }
#جے_انت
    *بایاں_padding = (pum_خانہ > pum_border);
    *بایاں_margin = *دایاں_margin && (pum_خانہ > 2);
}

/*
 * Draw the border around the popup menu
 */
    ساكن فراغ
pum_draw_border(فراغ)
{
    صحيح	چوٹی_صف = pum_صف - 1;
    صحيح	bottom_صف = pum_صف + pum_اچائی;
    صحيح	خانہ;
    صحيح	attr = highlight_attr[HLF_PMB];
    صحيح	دایاں_margin, بایاں_margin, بایاں_padding;
    صحيح	inner_چوڑائی;

    إذا (!pum_border)
	أرجع;

    (فراغ)compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);

    inner_چوڑائی = pum_چوڑائی + pum_scrollbar + بایاں_padding;

    // Draw border left to right or right to left (for RTL)
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
	خانہ = pum_خانہ + بایاں_padding + 1;
    وإلا
#جے_انت
	خانہ = pum_خانہ - بایاں_padding - 1;

    // left corners (right corners for RTL)
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٹی_دایاں, چوٹی_صف, خانہ, attr);
	إذا (بایاں_margin)
	    screen_putchar(' ', چوٹی_صف, خانہ + 1, attr);
	screen_putchar(pum_border_chars.bottom_دایاں, bottom_صف, خانہ, attr);
	إذا (بایاں_margin)
	    screen_putchar(' ', bottom_صف, خانہ + 1, attr);
    }
    وإلا
#جے_انت
    {
	screen_putchar(pum_border_chars.چوٹی_بایاں, چوٹی_صف, خانہ, attr);
	إذا (بایاں_margin)
	    screen_putchar(' ', چوٹی_صف, خانہ - 1, attr);
	screen_putchar(pum_border_chars.bottom_بایاں, bottom_صف, خانہ, attr);
	إذا (بایاں_margin)
	    screen_putchar(' ', bottom_صف, خانہ - 1, attr);
    }

    // Top horizontal lines
    لكل (صحيح i = 0; i < inner_چوڑائی; i++)
    {
	صحيح cur_خانہ;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    cur_خانہ = خانہ - 1 - i;
	وإلا
#جے_انت
	    cur_خانہ = خانہ + 1 + i;
	screen_putchar(pum_border_chars.چوٹی, چوٹی_صف, cur_خانہ, attr);
	screen_putchar(pum_border_chars.bottom, bottom_صف, cur_خانہ, attr);
    }

    // right corners (left corners for RTL)
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	screen_putchar(pum_border_chars.چوٹی_بایاں, چوٹی_صف,
		خانہ - inner_چوڑائی - 1, attr);
	إذا (دایاں_margin)
	    screen_putchar(' ', چوٹی_صف, خانہ - inner_چوڑائی - 2, attr);
	screen_putchar(pum_border_chars.bottom_بایاں, bottom_صف,
		خانہ - inner_چوڑائی - 1, attr);
	إذا (دایاں_margin)
	    screen_putchar(' ', bottom_صف, خانہ - inner_چوڑائی - 2, attr);
    }
    وإلا
#جے_انت
    {
	screen_putchar(pum_border_chars.چوٹی_دایاں, چوٹی_صف,
		خانہ + inner_چوڑائی + 1, attr);
	إذا (دایاں_margin)
	    screen_putchar(' ', چوٹی_صف, خانہ + inner_چوڑائی + 2, attr);
	screen_putchar(pum_border_chars.bottom_دایاں, bottom_صف,
		خانہ + inner_چوڑائی + 1, attr);
	إذا (دایاں_margin)
	    screen_putchar(' ', bottom_صف, خانہ + inner_چوڑائی + 2, attr);
    }

    // Draw side borders
    لكل (صحيح i = 0; i < pum_اچائی; i++)
    {
	صحيح صف = pum_صف + i;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	{
	    screen_putchar(pum_border_chars.بایاں, صف, خانہ, attr);
	    إذا (بایاں_margin)
		screen_putchar(' ', صف, خانہ + 1, attr);
	    screen_putchar(pum_border_chars.دایاں, صف,
		    خانہ - inner_چوڑائی - 1, attr);
	    إذا (دایاں_margin)
		screen_putchar(' ', صف, خانہ - inner_چوڑائی - 2, attr);
	}
	وإلا
#جے_انت
	{
	    screen_putchar(pum_border_chars.بایاں, صف, خانہ, attr);
	    إذا (بایاں_margin)
		screen_putchar(' ', صف, خانہ - 1, attr);
	    screen_putchar(pum_border_chars.دایاں, صف,
		    خانہ + inner_چوڑائی + 1, attr);
	    إذا (دایاں_margin)
		screen_putchar(' ', صف, خانہ + inner_چوڑائی + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    فراغ
put_shadow_char(صحيح صف, صحيح خانہ)
{
    char_u  buf[MB_MAXBYTES + 1];
    صحيح	    shadow_attr = highlight_attr[HLF_PMS];
    صحيح	    char_attr, new_attr;

    screen_getbytes(صف, خانہ, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), صف, خانہ, new_attr);
}

/*
 * Draw the shadow
 */
    ساكن فراغ
pum_draw_shadow(فراغ)
{
    صحيح	صف, خانہ, اگلا_خانہ;
    صحيح	چوڑائی, وتھ;
    صحيح	inner_چوڑائی;
    صحيح	دایاں_margin, بایاں_margin, بایاں_padding;

    إذا (!pum_shadow)
	أرجع;

    (فراغ)compute_margins(&دایاں_margin, &بایاں_margin, &بایاں_padding);
    inner_چوڑائی = pum_چوڑائی + pum_scrollbar + pum_border + دایاں_margin;

    // Draw right side shadow (left for RTL)
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (pum_rl)
    {
	خانہ = pum_خانہ - inner_چوڑائی;
	اگلا_خانہ = خانہ - 1;
    }
    وإلا
#جے_انت
    {
	خانہ = pum_خانہ + inner_چوڑائی;
	اگلا_خانہ = خانہ + 1;
    }

    لكل (صحيح i = 0; i < pum_اچائی + pum_border; i++)
    {
	صف = pum_صف + 1 + i - pum_border;
	put_shadow_char(صف, خانہ);
	put_shadow_char(صف, اگلا_خانہ);
    }

    // Bottom shadow
    صف = pum_صف + pum_اچائی + pum_border;
    چوڑائی = inner_چوڑائی + بایاں_padding + pum_border + بایاں_margin;
    وتھ = 2 - بایاں_padding - pum_border - بایاں_margin;

    لكل (صحيح i = 0; i < چوڑائی; i++)
    {
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (pum_rl)
	    خانہ = pum_خانہ - وتھ - i;
	وإلا
#جے_انت
	    خانہ = pum_خانہ + وتھ + i;
	put_shadow_char(صف, خانہ);
    }
}
