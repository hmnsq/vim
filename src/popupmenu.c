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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ pumitem_T *pum_array = ਨਿਰਾ;	// items of displayed pum
ਸਥਿਰ ਪੂਰਨ pum_ਮਾਪ;			// nr of items in "pum_array"
ਸਥਿਰ ਪੂਰਨ pum_selected;		// index of selected item or -1
ਸਥਿਰ ਪੂਰਨ pum_ਪਹਿਲਾ = 0;		// index of top item

ਸਥਿਰ ਪੂਰਨ call_update_screen = FALSE;
ਸਥਿਰ ਪੂਰਨ pum_ਵਿੱਚ_cmdline = FALSE;

ਸਥਿਰ ਪੂਰਨ pum_ਉਚਾਈ;			// nr of displayed pum items
ਸਥਿਰ ਪੂਰਨ pum_ਚੌੜਾਈ;			// width of displayed pum items
ਸਥਿਰ ਪੂਰਨ pum_ਅਧਾਰ_ਚੌੜਾਈ;		// width of pum items base
ਸਥਿਰ ਪੂਰਨ pum_ਕਿਸਮ_ਚੌੜਾਈ;		// width of pum items kind column
ਸਥਿਰ ਪੂਰਨ pum_extra_ਚੌੜਾਈ;		// width of extra stuff
ਸਥਿਰ ਪੂਰਨ pum_scrollbar;		// TRUE when scrollbar present
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
ਸਥਿਰ ਪੂਰਨ pum_rl;			// TRUE when pum is drawn 'rightleft'
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ pum_ਪੰਕਤੀ;			// top row of pum
ਸਥਿਰ ਪੂਰਨ pum_ਖਾਨਾ;			// left column of pum

ਸਥਿਰ win_T *pum_ਖਿੜਕੀ = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ pum_win_ਪੰਕਤੀ;
ਸਥਿਰ ਪੂਰਨ pum_win_ਉਚਾਈ;
ਸਥਿਰ ਪੂਰਨ pum_win_ਖਾਨਾ;
ਸਥਿਰ ਪੂਰਨ pum_win_wcol;
ਸਥਿਰ ਪੂਰਨ pum_win_ਚੌੜਾਈ;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ਸਥਿਰ ਪੂਰਨ pum_pretend_not_visible = FALSE;

// Border support
ਸਥਿਰ ਪੂਰਨ pum_border = 0;
ਸਥਿਰ ਪੂਰਨ pum_margin = 0;	// margin of 1 cell on left and right
ਸਥਿਰ ਪੂਰਨ pum_shadow = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
ਸਥਿਰ image_placement_T *pum_imagep = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

// Border characters
ਸਥਿਰ ਬਣਤਰ {
    ਪੂਰਨ ਸਿਖਰ;
    ਪੂਰਨ ਸੱਜਾ;
    ਪੂਰਨ bottom;
    ਪੂਰਨ ਖੱਬਾ;
    ਪੂਰਨ ਸਿਖਰ_ਖੱਬਾ;
    ਪੂਰਨ ਸਿਖਰ_ਸੱਜਾ;
    ਪੂਰਨ bottom_ਸੱਜਾ;
    ਪੂਰਨ bottom_ਖੱਬਾ;
} pum_border_chars;

ਸਥਿਰ ਪੂਰਨ pum_ਸੈੱਟ_selected(ਪੂਰਨ n, ਪੂਰਨ repeat);
ਸਥਿਰ ਖਾਲੀ pum_draw_border(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ pum_draw_shadow(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ compute_margins(ਪੂਰਨ *ਸੱਜਾ_margin, ਪੂਰਨ *ਖੱਬਾ_margin, ਪੂਰਨ *ਖੱਬਾ_padding);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUM_DEF_ਉਚਾਈ 10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_PUM_SETTER(feature) \
	ਖਾਲੀ \
    pum_ਸੈੱਟ##feature(ਪੂਰਨ enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ਸਥਿਰ ਖਾਲੀ
pum_compute_ਮਾਪ(ਖਾਲੀ)
{
    ਪੂਰਨ	w;

    // Compute the width of the widest match and the widest extra.
    pum_ਅਧਾਰ_ਚੌੜਾਈ = 0;
    pum_ਕਿਸਮ_ਚੌੜਾਈ = 0;
    pum_extra_ਚੌੜਾਈ = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < pum_ਮਾਪ; ++i)
    {
	ਜੇ (pum_array[i].pum_ਪਾਠ != ਨਿਰਾ)
	{
	    w = vim_strsize(pum_array[i].pum_ਪਾਠ);
	    ਜੇ (pum_ਅਧਾਰ_ਚੌੜਾਈ < w)
		pum_ਅਧਾਰ_ਚੌੜਾਈ = w;
	}
	ਜੇ (pum_array[i].pum_ਕਿਸਮ != ਨਿਰਾ)
	{
	    w = vim_strsize(pum_array[i].pum_ਕਿਸਮ) + 1;
	    ਜੇ (pum_ਕਿਸਮ_ਚੌੜਾਈ < w)
		pum_ਕਿਸਮ_ਚੌੜਾਈ = w;
	}
	ਜੇ (pum_array[i].pum_extra != ਨਿਰਾ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ਜੇ (pum_extra_ਚੌੜਾਈ < w)
		pum_extra_ਚੌੜਾਈ = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_compute_vertical_placement(
	ਪੂਰਨ ਮਾਪ,
	ਪੂਰਨ above_ਪੰਕਤੀ,
	ਪੂਰਨ ਹੇਠਾਂ_ਪੰਕਤੀ)
{
    ਪੂਰਨ context_ਸਤਰਾਂ;
    ਪੂਰਨ cline_visible_ਵਿੱਥ;
    ਪੂਰਨ extra_ਉਚਾਈ = 2 * pum_border + pum_shadow;
    ਪੂਰਨ extra_ਵਿੱਥ = pum_border + pum_shadow;

    pum_ਉਚਾਈ = MIN(ਮਾਪ, PUM_DEF_ਉਚਾਈ);
    ਜੇ (p_ph > 0 && pum_ਉਚਾਈ > p_ph)
	pum_ਉਚਾਈ = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ਜੇ (pum_win_ਪੰਕਤੀ + 2 >= ਹੇਠਾਂ_ਪੰਕਤੀ - pum_ਉਚਾਈ - extra_ਉਚਾਈ
	    && pum_win_ਪੰਕਤੀ - above_ਪੰਕਤੀ > (ਹੇਠਾਂ_ਪੰਕਤੀ - above_ਪੰਕਤੀ) / 2)
    {
	// pum above "pum_win_row"
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ਸਤਰਾਂ = 0;
	ਨਹੀਂਤਾਂ
	    // Leave two lines of context if possible
	    context_ਸਤਰਾਂ = MIN(2, curwin->w_wrow - curwin->w_cline_ਪੰਕਤੀ);

	ਜੇ (pum_win_ਪੰਕਤੀ >= ਮਾਪ + context_ਸਤਰਾਂ + extra_ਉਚਾਈ)
	{
	    pum_ਪੰਕਤੀ = pum_win_ਪੰਕਤੀ - ਮਾਪ - context_ਸਤਰਾਂ - extra_ਵਿੱਥ;
	    pum_ਉਚਾਈ = ਮਾਪ;
	}
	ਨਹੀਂਤਾਂ
	{
	    pum_ਪੰਕਤੀ = pum_border;
	    pum_ਉਚਾਈ = pum_win_ਪੰਕਤੀ - context_ਸਤਰਾਂ - extra_ਉਚਾਈ;
	}
	ਜੇ (p_ph > 0 && pum_ਉਚਾਈ > p_ph)
	{
	    pum_ਪੰਕਤੀ += pum_ਉਚਾਈ - p_ph;
	    pum_ਉਚਾਈ = p_ph;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// pum below "pum_win_row"
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ਸਤਰਾਂ = 0;
	ਨਹੀਂਤਾਂ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_ਵਿੱਥ = curwin->w_cline_ਪੰਕਤੀ +
		curwin->w_cline_ਉਚਾਈ - curwin->w_wrow;
	    context_ਸਤਰਾਂ = MIN(3, cline_visible_ਵਿੱਥ);
	}

	pum_ਪੰਕਤੀ = pum_win_ਪੰਕਤੀ + context_ਸਤਰਾਂ + pum_border;
	pum_ਉਚਾਈ = MIN(ਹੇਠਾਂ_ਪੰਕਤੀ - pum_ਪੰਕਤੀ - extra_ਵਿੱਥ, ਮਾਪ);
	ਜੇ (p_ph > 0 && pum_ਉਚਾਈ > p_ph)
	    pum_ਉਚਾਈ = p_ph;
    }

#ਜੇ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ਜੇ (above_ਪੰਕਤੀ > 0 && pum_ਪੰਕਤੀ < above_ਪੰਕਤੀ && pum_ਉਚਾਈ > above_ਪੰਕਤੀ)
    {
	pum_ਪੰਕਤੀ = above_ਪੰਕਤੀ;
	pum_ਉਚਾਈ = pum_win_ਪੰਕਤੀ - above_ਪੰਕਤੀ;
    }
#ਜੇ_ਅੰਤ
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_pum_ਚੌੜਾਈ_aligned_with_cursor(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਪਲਬਧ_ਚੌੜਾਈ)
{
    ਪੂਰਨ ਸਿਰਾ_padding = TRUE;

    ਜੇ (ਚੌੜਾਈ < p_pw)
    {
	ਚੌੜਾਈ = p_pw;
	ਸਿਰਾ_padding = FALSE;
    }
    ਜੇ (p_pmw > 0 && ਚੌੜਾਈ >= p_pmw)
    {
	ਚੌੜਾਈ = p_pmw;
	ਸਿਰਾ_padding = FALSE;
    }

    pum_ਚੌੜਾਈ = ਚੌੜਾਈ + (ਸਿਰਾ_padding && ਚੌੜਾਈ >= p_pw ? 1 : 0);
    ਵਾਪਸ ਉਪਲਬਧ_ਚੌੜਾਈ >= pum_ਚੌੜਾਈ;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_compute_horizontal_placement(ਪੂਰਨ cursor_ਖਾਨਾ)
{
    ਪੂਰਨ desired_ਚੌੜਾਈ = pum_ਅਧਾਰ_ਚੌੜਾਈ + pum_ਕਿਸਮ_ਚੌੜਾਈ + pum_extra_ਚੌੜਾਈ;
    ਪੂਰਨ ਉਪਲਬਧ_ਚੌੜਾਈ;
    ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ;
    ਪੂਰਨ trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	ਖਾਨਾ_ਵਿੱਥ = pum_border && (cursor_ਖਾਨਾ >= Columns - 2)
	    ? (3 - Columns + cursor_ਖਾਨਾ) : 0;
	ਉਪਲਬਧ_ਚੌੜਾਈ = cursor_ਖਾਨਾ + 1;
	pum_ਖਾਨਾ = cursor_ਖਾਨਾ - ਖਾਨਾ_ਵਿੱਥ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਖਾਨਾ_ਵਿੱਥ = pum_border && (cursor_ਖਾਨਾ <= 1) ? (2 - cursor_ਖਾਨਾ) : 0;
	ਉਪਲਬਧ_ਚੌੜਾਈ = Columns - cursor_ਖਾਨਾ;
	pum_ਖਾਨਾ = cursor_ਖਾਨਾ + ਖਾਨਾ_ਵਿੱਥ;
    }

    ਉਪਲਬਧ_ਚੌੜਾਈ -= trailing_overhead + ਖਾਨਾ_ਵਿੱਥ;

    // Case 1: Align pum with "cursor_col"
    ਜੇ (ਸੈੱਟ_pum_ਚੌੜਾਈ_aligned_with_cursor(desired_ਚੌੜਾਈ, ਉਪਲਬਧ_ਚੌੜਾਈ))
	ਵਾਪਸ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ਜੇ (ਉਪਲਬਧ_ਚੌੜਾਈ > p_pw)
    {
	pum_ਚੌੜਾਈ = ਉਪਲਬਧ_ਚੌੜਾਈ;
	ਵਾਪਸ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    ਉਪਲਬਧ_ਚੌੜਾਈ = Columns - trailing_overhead - ਖਾਨਾ_ਵਿੱਥ;

    ਜੇ (ਉਪਲਬਧ_ਚੌੜਾਈ > p_pw)
    {
	pum_ਚੌੜਾਈ = p_pw;  // Truncate beyond 'pum_width'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    pum_ਖਾਨਾ = pum_ਚੌੜਾਈ + trailing_overhead;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    pum_ਖਾਨਾ = Columns - pum_ਚੌੜਾਈ - trailing_overhead;
	ਵਾਪਸ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
	pum_ਖਾਨਾ = Columns - 1 - ਖਾਨਾ_ਵਿੱਥ;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	pum_ਖਾਨਾ = ਖਾਨਾ_ਵਿੱਥ;
    pum_ਚੌੜਾਈ = Columns - trailing_overhead;
}

/*
 * Show the popup menu with items "array[size]".
 * "array" must remain valid until pum_undisplay() is called!
 * When possible the leftmost character is aligned with cursor column.
 * The menu appears above the screen line "row" or at "row" + "height" - 1.
 */
    ਖਾਲੀ
pum_display(
	pumitem_T   *array,
	ਪੂਰਨ	    ਮਾਪ,
	ਪੂਰਨ	    selected,   // index of initially selected item, -1 if
				// out of range
	ਪੂਰਨ	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    ਪੂਰਨ	    cursor_ਖਾਨਾ;
    ਪੂਰਨ	    above_ਪੰਕਤੀ;
    ਪੂਰਨ	    ਹੇਠਾਂ_ਪੰਕਤੀ;
    ਪੂਰਨ	    redo_ਗਿਣਤੀ = 0;
#ਜੇ defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    pum_rl = (State & ਬਹੁਲਕ_CMDLINE) == 0 && curwin->w_p_rl;
#ਜੇ_ਅੰਤ

    ਕਰੋ
    {
	above_ਪੰਕਤੀ = 0;
	ਹੇਠਾਂ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_ਖਾਨਾ();
	pum_array = ਨਿਰਾ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_ਖਿੜਕੀ = curwin;
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
	ਨਹੀਂਤਾਂ
	    pum_win_ਪੰਕਤੀ = curwin->w_wrow + W_WINROW(curwin);
	pum_win_ਉਚਾਈ = curwin->w_ਉਚਾਈ;
	pum_win_ਖਾਨਾ = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_ਚੌੜਾਈ = curwin->w_ਚੌੜਾਈ;

#ਜੇ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ਜੇ (pvwin->w_p_pvw)
		ਰੋਕੋ;
	ਜੇ (pvwin != ਨਿਰਾ)
	{
	    ਜੇ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_ਪੰਕਤੀ = W_WINROW(pvwin) + pvwin->w_ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_ਉਚਾਈ)
		ਹੇਠਾਂ_ਪੰਕਤੀ = W_WINROW(pvwin);
	}
#ਜੇ_ਅੰਤ

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(ਮਾਪ, above_ਪੰਕਤੀ, ਹੇਠਾਂ_ਪੰਕਤੀ);

	// don't display when we only have room for one line
	ਜੇ (pum_ਉਚਾਈ < 1 || (pum_ਉਚਾਈ == 1 && ਮਾਪ > 1))
	    ਵਾਪਸ;

	pum_array = array;
	pum_ਮਾਪ = ਮਾਪ;
	pum_compute_ਮਾਪ();

	// Calculate cursor column
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    // cmdline completion popup menu
	    cursor_ਖਾਨਾ = cmdline_compl_startcol();
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ਜੇ (curwin->w_ਚੌੜਾਈ > 0)
		wcol %= curwin->w_ਚੌੜਾਈ;
	    ਨਹੀਂਤਾਂ
		wcol = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ਜੇ (curwin->w_p_cole > 0 && conceal_cursor_ਸਤਰ(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ਜੇ (wcol < 0)
		    wcol = 0;
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (pum_rl)
		cursor_ਖਾਨਾ = curwin->w_wincol + curwin->w_ਚੌੜਾਈ - wcol - 1;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		cursor_ਖਾਨਾ = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_ਉਚਾਈ < ਮਾਪ) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_ਖਾਨਾ);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } ਜਦੋਂ (pum_ਸੈੱਟ_selected(selected, redo_ਗਿਣਤੀ) && ++redo_ਗਿਣਤੀ <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    ਖਾਲੀ
pum_call_update_screen(ਖਾਲੀ)
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
    ਪੂਰਨ
pum_under_menu(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ only_redrawing)
{
    ਪੂਰਨ	extra_ਖੱਬਾ = pum_border + (pum_margin && pum_border ? 1 : 0);
    ਪੂਰਨ	extra_ਸੱਜਾ = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    ਪੂਰਨ	extra_above = pum_border;
    ਪੂਰਨ	extra_ਹੇਠਾਂ = pum_border + (pum_shadow ? 1 : 0);
    ਪੂਰਨ	ਸਿਖਰ = pum_ਪੰਕਤੀ - extra_above;
    ਪੂਰਨ	bot = pum_ਪੰਕਤੀ + pum_ਉਚਾਈ + extra_ਹੇਠਾਂ;
    ਪੂਰਨ	ਖੱਬਾ = pum_ਖਾਨਾ - 1 - extra_ਖੱਬਾ;
    ਪੂਰਨ	ਸੱਜਾ = pum_ਖਾਨਾ + pum_ਚੌੜਾਈ + pum_scrollbar + extra_ਸੱਜਾ;

    ਜੇ (!((!only_redrawing || pum_will_redraw)
	    && ਪੰਕਤੀ >= ਸਿਖਰ && ਪੰਕਤੀ < bot && ਖਾਨਾ >= ਖੱਬਾ && ਖਾਨਾ < ਸੱਜਾ))
	ਵਾਪਸ FALSE;

    ਜੇ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ਜੇ (only_redrawing)
	{
	    ਜੇ (ਖਾਨਾ >= ਸੱਜਾ - 2 || ਪੰਕਤੀ == bot - 1)
		ਵਾਪਸ FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    ਪੂਰਨ	    ਸੱਜਾ_margin, ਖੱਬਾ_margin, ਖੱਬਾ_padding;

	    compute_margins(&ਸੱਜਾ_margin, &ਖੱਬਾ_margin, &ਖੱਬਾ_padding);
	    ਜੇ (ਪੰਕਤੀ == ਸਿਖਰ && ਖਾਨਾ >= ਸੱਜਾ - 2)
		ਵਾਪਸ FALSE;
	    ਜੇ (ਪੰਕਤੀ == bot - 1 && ਖਾਨਾ < pum_ਖਾਨਾ + 2 - ਖੱਬਾ_padding - pum_border
		    - ਖੱਬਾ_margin)
		ਵਾਪਸ FALSE;
	}
    }
    ਵਾਪਸ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ਸਥਿਰ ਪੂਰਨ *
pum_compute_ਪਾਠ_attrs(char_u *ਪਾਠ, hlf_T hlf, ਪੂਰਨ user_hlattr)
{
    ਪੂਰਨ		i;
    size_t	leader_len;
    ਪੂਰਨ		char_cells;
    ਪੂਰਨ		new_attr;
    char_u	*ptr = ਪਾਠ;
    ਪੂਰਨ		cell_idx = 0;
    garray_T	*ga = ਨਿਰਾ;
    ਪੂਰਨ		*attrs = ਨਿਰਾ;
    char_u	*leader = ਨਿਰਾ;
    ਪੂਰਨ		ਵਿੱਚ_fuzzy;
    ਪੂਰਨ		matched_len = -1;
    int_u	char_pos = 0;
    ਪੂਰਨ		is_select = FALSE;

    ਜੇ (*ਪਾਠ == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ਵਾਪਸ ਨਿਰਾ;

    is_select = hlf == HLF_PSI;
    leader = (State & ਬਹੁਲਕ_CMDLINE) ? cmdline_compl_ਨਮੂਨਾ()
							  : ins_compl_leader();
    ਜੇ (leader == ਨਿਰਾ || *leader == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    attrs = ALLOC_MULT(ਪੂਰਨ, vim_strsize(ਪਾਠ));
    ਜੇ (attrs == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਿੱਚ_fuzzy = (State & ਬਹੁਲਕ_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ਜੇ (ਵਿੱਚ_fuzzy)
    {
	ga = fuzzy_ਮੇਲ_str_with_pos(ਪਾਠ, leader);
	ਜੇ (!ga)
	{
	    vim_free(attrs);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    ਜਦੋਂ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ਜੇ (ga != ਨਿਰਾ)
	{
	    // Handle fuzzy matching
	    ਲਈ (i = 0; i < ga->ga_len; i++)
	    {
		ਜੇ (char_pos == ((int_u *)ga->ga_ਅੰਕੜੇ)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (ਪੂਰਨ)leader_len;
	    ਜੇ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ਜੇ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	ਲਈ (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ਜੇ (ga != ਨਿਰਾ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ਵਾਪਸ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_screen_puts_with_attrs(
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		cells ਅਣਵਰਤਿਆ,
    char_u	*ਪਾਠ,
    ਪੂਰਨ		textlen,
    ਪੂਰਨ		*attrs)
{
    ਪੂਰਨ		ਖਾਨਾ_ਸ਼ੁਰੂ = ਖਾਨਾ;
    char_u	*ptr = ਪਾਠ;
    ਪੂਰਨ		char_len;
    ਪੂਰਨ		attr;

    // Render text with proper attributes
    ਜਦੋਂ (*ptr != NUL && ptr < ਪਾਠ + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    attr = attrs[ਖਾਨਾ_ਸ਼ੁਰੂ + cells - ਖਾਨਾ - 1];
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    attr = attrs[ਖਾਨਾ - ਖਾਨਾ_ਸ਼ੁਰੂ];
	screen_puts_len(ptr, char_len, ਪੰਕਤੀ, ਖਾਨਾ, attr);
	ਖਾਨਾ += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ਸਥਿਰ ਸਤਰਅੰਦਰ char_u *
pum_get_ਵਸਤੂ(ਪੂਰਨ ਸੂਚਕਾਂਕ, ਪੂਰਨ type)
{
    ਚੋਣ(type)
    {
	ਹਾਲਤ CPT_ABBR: ਵਾਪਸ pum_array[ਸੂਚਕਾਂਕ].pum_ਪਾਠ;
	ਹਾਲਤ CPT_ਕਿਸਮ: ਵਾਪਸ pum_array[ਸੂਚਕਾਂਕ].pum_ਕਿਸਮ;
	ਹਾਲਤ CPT_MENU: ਵਾਪਸ pum_array[ਸੂਚਕਾਂਕ].pum_extra;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
pum_user_attr_combine(ਪੂਰਨ idx, ਪੂਰਨ type, ਪੂਰਨ attr)
{
    ਪੂਰਨ user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_ਕਿਸਮ_hlattr,
    };

    ਵਾਪਸ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ਸਥਿਰ ਪੂਰਨ
pum_display_rtl_ਪਾਠ(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ     ਖਾਨਾ,
	char_u  *st,
	ਪੂਰਨ     attr,
	ਪੂਰਨ     *attrs,
	ਪੂਰਨ     ਚੌੜਾਈ,
	ਪੂਰਨ     ਚੌੜਾਈ_ਹੱਦਬੰਦੀ,
	ਪੂਰਨ     totwidth,
	ਪੂਰਨ     ਅਗਲਾ_isempty,
	ਪੂਰਨ	selected)
{
    char_u  *rt = ਨਿਰਾ;
    ਪੂਰਨ     cells = 0;
    ਪੂਰਨ     over_cell = 0;
    ਪੂਰਨ     truncated = FALSE;
    ਪੂਰਨ     pad = ਅਗਲਾ_isempty ? 0 : 2;
    ਪੂਰਨ     ਬਚਿਆ = 0;
    ਪੂਰਨ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ਪੂਰਨ	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ਜੇ (st == ਨਿਰਾ)
	ਵਾਪਸ ਖਾਨਾ;

    rt = ਉਲਟ_ਪਾਠ(st);
    ਜੇ (rt == ਨਿਰਾ)
    {
	VIM_CLEAR(st);
	ਵਾਪਸ ਖਾਨਾ;
    }

    char_u *rt_ਸ਼ੁਰੂ = rt;
    cells = mb_string2cells(rt, -1);
    truncated = ਚੌੜਾਈ_ਹੱਦਬੰਦੀ - totwidth < cells + pad;

    // only draw the text that fits
    ਜੇ (cells > ਚੌੜਾਈ_ਹੱਦਬੰਦੀ)
    {
	ਕਰੋ
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ਜਦੋਂ (cells > ਚੌੜਾਈ_ਹੱਦਬੰਦੀ);

	ਜੇ (cells < ਚੌੜਾਈ_ਹੱਦਬੰਦੀ)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ਜੇ (truncated)
    {
	char_u  *orig_rt = rt;
	ਪੂਰਨ     ਮਾਪ = 0;

	ਬਚਿਆ = ਚੌੜਾਈ_ਹੱਦਬੰਦੀ - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ਜੇ (cells > ਬਚਿਆ)
	{
	    ਜਦੋਂ (cells > ਬਚਿਆ)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	ਮਾਪ = (ਪੂਰਨ)STRLEN(orig_rt);
	ਜੇ (cells < ਬਚਿਆ)
	    over_cell = ਬਚਿਆ - cells;

	cells = mb_string2cells(orig_rt, ਮਾਪ);
	ਚੌੜਾਈ = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, ਪੰਕਤੀ, ਖਾਨਾ - ਚੌੜਾਈ + 1, trunc_attr);

	ਜੇ (over_cell > 0)
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ - ਚੌੜਾਈ + 2,
		    ਖਾਨਾ - ਚੌੜਾਈ + 2 + over_cell, ' ', ' ', attr);
    }

    ਜੇ (attrs == ਨਿਰਾ)
	screen_puts_len(rt, (ਪੂਰਨ)STRLEN(rt), ਪੰਕਤੀ, ਖਾਨਾ - cells + 1, attr);
    ਨਹੀਂਤਾਂ
	pum_screen_puts_with_attrs(ਪੰਕਤੀ, ਖਾਨਾ - cells + 1, cells, rt,
		(ਪੂਰਨ)STRLEN(rt), attrs);

    vim_free(rt_ਸ਼ੁਰੂ);
    VIM_CLEAR(st);
    ਵਾਪਸ ਖਾਨਾ - ਚੌੜਾਈ;
}
#ਜੇ_ਅੰਤ

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ਸਥਿਰ ਪੂਰਨ
pum_display_ltr_ਪਾਠ(
	ਪੂਰਨ     ਪੰਕਤੀ,
	ਪੂਰਨ     ਖਾਨਾ,
	char_u  *st,
	ਪੂਰਨ     attr,
	ਪੂਰਨ     *attrs,
	ਪੂਰਨ     ਚੌੜਾਈ,        // width already calculated in outer loop
	ਪੂਰਨ     ਚੌੜਾਈ_ਹੱਦਬੰਦੀ,
	ਪੂਰਨ     totwidth,
	ਪੂਰਨ     ਅਗਲਾ_isempty,
	ਪੂਰਨ	selected)
{
    ਪੂਰਨ     ਮਾਪ = 0;
    ਪੂਰਨ     cells = 0;
    char_u  *st_ਸਿਰਾ = ਨਿਰਾ;
    ਪੂਰਨ     over_cell = 0;
    ਪੂਰਨ     pad = ਅਗਲਾ_isempty ? 0 : 2;
    ਪੂਰਨ     truncated = FALSE;
    ਪੂਰਨ     ਬਚਿਆ = 0;
    ਪੂਰਨ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ਪੂਰਨ	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ਜੇ (st == ਨਿਰਾ)
	ਵਾਪਸ ਖਾਨਾ;

    ਮਾਪ = (ਪੂਰਨ)STRLEN(st);
    cells = (*mb_string2cells)(st, ਮਾਪ);
    truncated = ਚੌੜਾਈ_ਹੱਦਬੰਦੀ - totwidth < cells + pad;

    // only draw the text that fits
    ਜਦੋਂ (ਮਾਪ > 0 && ਖਾਨਾ + cells > ਚੌੜਾਈ_ਹੱਦਬੰਦੀ + pum_ਖਾਨਾ)
    {
	--ਮਾਪ;
	ਜੇ (has_mbyte)
	{
	    ਮਾਪ -= (*mb_ਸਿਰ_off)(st, st + ਮਾਪ);
	    cells -= (*mb_ptr2cells)(st + ਮਾਪ);
	}
	ਨਹੀਂਤਾਂ
	    --cells;
    }

    // truncated
    ਜੇ (truncated)
    {
	ਬਚਿਆ = ਚੌੜਾਈ_ਹੱਦਬੰਦੀ - totwidth - 1;
	ਜੇ (cells > ਬਚਿਆ)
	{
	    st_ਸਿਰਾ = st + ਮਾਪ;
	    ਜਦੋਂ (st_ਸਿਰਾ > st && cells > ਬਚਿਆ)
	    {
		MB_PTR_ਪਿੱਠ(st, st_ਸਿਰਾ);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_ਸਿਰਾ) : 1;
	    }
	    ਮਾਪ = st_ਸਿਰਾ - st;
	}

	ਜੇ (cells < ਬਚਿਆ)
	    over_cell = ਬਚਿਆ - cells;
	cells = mb_string2cells(st, ਮਾਪ);
	ਚੌੜਾਈ = cells + over_cell + 1;
    }

    ਜੇ (attrs == ਨਿਰਾ)
	screen_puts_len(st, ਮਾਪ, ਪੰਕਤੀ, ਖਾਨਾ, attr);
    ਨਹੀਂਤਾਂ
	pum_screen_puts_with_attrs(ਪੰਕਤੀ, ਖਾਨਾ, cells, st, ਮਾਪ, attrs);

    ਜੇ (truncated)
    {
	ਜੇ (over_cell > 0)
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ + cells,
		    ਖਾਨਾ + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, ਪੰਕਤੀ, ਖਾਨਾ + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ਵਾਪਸ ਖਾਨਾ + ਚੌੜਾਈ;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ਸਥਿਰ ਪੂਰਨ
pum_process_ਵਸਤੂ(
	ਪੂਰਨ     ਪੰਕਤੀ,
	ਪੂਰਨ     ਖਾਨਾ,
	ਪੂਰਨ     idx,
	ਪੂਰਨ     j,         // Current position in order array
	ਪੂਰਨ     *ਕ੍ਰਮ,    // Order array
	hlf_T   hlf,
	ਪੂਰਨ     attr,
	ਪੂਰਨ     *totwidth_ptr,
	ਪੂਰਨ     ਅਗਲਾ_isempty)
{
    ਪੂਰਨ     ਵਸਤੂ_type = ਕ੍ਰਮ[j];
    char_u  *s = ਨਿਰਾ;
    char_u  *p = pum_get_ਵਸਤੂ(idx, ਵਸਤੂ_type);
    ਪੂਰਨ     ਚੌੜਾਈ = 0;  // item width
    ਪੂਰਨ     w;		// char width
    ਪੂਰਨ	    selected = idx == pum_selected;

    ਲਈ ( ; ; MB_PTR_ADV(p))
    {
	ਜੇ (s == ਨਿਰਾ)
	    s = p;
	w = ptr2cells(p);
	ਜੇ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_ਚੌੜਾਈ)
	{
	    ਚੌੜਾਈ += w;
	    ਜਾਰੀ;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	ਪੂਰਨ     *attrs = ਨਿਰਾ;
	ਪੂਰਨ     saved = *p;

	ਜੇ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ਜੇ (saved != NUL)
	    *p = saved;

	ਜੇ (ਵਸਤੂ_type == CPT_ABBR)
	    attrs = pum_compute_ਪਾਠ_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    ਖਾਨਾ = pum_display_rtl_ਪਾਠ(ਪੰਕਤੀ, ਖਾਨਾ, st, attr, attrs,
		    ਚੌੜਾਈ, pum_ਚੌੜਾਈ, *totwidth_ptr, ਅਗਲਾ_isempty, selected);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਖਾਨਾ = pum_display_ltr_ਪਾਠ(ਪੰਕਤੀ, ਖਾਨਾ, st, attr, attrs,
		    ਚੌੜਾਈ, pum_ਚੌੜਾਈ, *totwidth_ptr, ਅਗਲਾ_isempty, selected);

	ਜੇ (attrs != ਨਿਰਾ)
	    VIM_CLEAR(attrs);

	ਜੇ (*p != TAB)
	    ਰੋਕੋ;

	// Display two spaces for a Tab.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, ਪੰਕਤੀ, ਖਾਨਾ - 1, attr);
	    ਖਾਨਾ -= 2;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    screen_puts_len((char_u *)"  ", 2, ਪੰਕਤੀ, ਖਾਨਾ, attr);
	    ਖਾਨਾ += 2;
	}
	*totwidth_ptr += 2;
	s = ਨਿਰਾ;  // start text at next char
	ਚੌੜਾਈ = 0;
    }

    ਵਾਪਸ ਖਾਨਾ;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_draw_scrollbar(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	i,
	ਪੂਰਨ	thumb_pos,
	ਪੂਰਨ	thumb_ਉਚਾਈ)
{
    ਜੇ (pum_scrollbar <= 0)
	ਵਾਪਸ;

    ਪੂਰਨ attr = (i >= thumb_pos && i < thumb_pos + thumb_ਉਚਾਈ) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
	screen_putchar(' ', ਪੰਕਤੀ, pum_ਖਾਨਾ - pum_ਚੌੜਾਈ, attr);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	screen_putchar(' ', ਪੰਕਤੀ, pum_ਖਾਨਾ + pum_ਚੌੜਾਈ, attr);
}

    ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
pum_align_ਕ੍ਰਮ(ਪੂਰਨ *ਕ੍ਰਮ)
{
    ਪੂਰਨ is_default = cia_flags == 0;
    ਕ੍ਰਮ[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ਕ੍ਰਮ[1] = is_default ? CPT_ਕਿਸਮ : (cia_flags / 10) % 10;
    ਕ੍ਰਮ[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ਸਥਿਰ ਖਾਲੀ pum_free_bg(ਖਾਲੀ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    ਖਾਲੀ
pum_opacity_ਬਦਲਿਆ(ਖਾਲੀ)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    ਜੇ (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ਸਥਿਰ ਖਾਲੀ
pum_free_bg(ਖਾਲੀ)
{
    ਪੂਰਨ k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_ਸਤਰਾਂ);
    VIM_CLEAR(pum_bg_ਸਤਰਾਂ_UC);
    ਲਈ (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_ਸਤਰਾਂ_C[k]);
    pum_bg_ਸਿਖਰ = 0;
    pum_bg_bot = 0;
    pum_bg_ਖਾਨੇ_ਗਿਣਤੀ = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    ਖਾਲੀ
pum_redraw(ਖਾਲੀ)
{
    ਪੂਰਨ		ਪੰਕਤੀ = pum_ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		idx;
    char_u	*p = ਨਿਰਾ;
    ਪੂਰਨ		totwidth;
    ਪੂਰਨ		thumb_pos = 0;
    ਪੂਰਨ		thumb_ਉਚਾਈ = 1;
    ਪੂਰਨ		ਵਸਤੂ_type;
    ਪੂਰਨ		ਕ੍ਰਮ[3];
    ਪੂਰਨ		ਅਗਲਾ_isempty = FALSE;
    ਪੂਰਨ		n;
    ਪੂਰਨ		ਚੀਜ਼ਾਂ_ਚੌੜਾਈ_array[3] = { pum_ਅਧਾਰ_ਚੌੜਾਈ, pum_ਕਿਸਮ_ਚੌੜਾਈ,
							    pum_extra_ਚੌੜਾਈ };
    ਪੂਰਨ		basic_ਚੌੜਾਈ;  // first item width
    ਪੂਰਨ		ਆਖਰੀ_isabbr = FALSE;
    ਪੂਰਨ		orig_attr = -1;
    ਪੂਰਨ		scroll_range = pum_ਮਾਪ - pum_ਉਚਾਈ;
    bool	override_success;
    ਪੂਰਨ		opacity_ਸਰਗਰਮ = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ਧੱਕੋ_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_ENABLE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ਜੇ (pum_imagep == ਨਿਰਾ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ਨਿਰਾ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ਜੇ (pum_imagep != ਨਿਰਾ)
    {
	image_placement_ਸੈੱਟ_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_ਸੈੱਟ_position(pum_imagep, pum_ਪੰਕਤੀ, pum_ਖਾਨਾ);
	image_placement_ਸੈੱਟ_bounding_box(pum_imagep, pum_ਪੰਕਤੀ, pum_ਖਾਨਾ,
		pum_ਉਚਾਈ, pum_ਚੌੜਾਈ);
	image_placement_ਸੈੱਟ_enable(pum_imagep, true);
    }
#ਜੇ_ਅੰਤ

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

    ਜੇ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ਜੇ (opacity_ਸਰਗਰਮ && pum_bg_ਸਤਰਾਂ != ਨਿਰਾ
		&& (pum_bg_ਖਾਨੇ_ਗਿਣਤੀ != screen_Columns
		    || pum_bg_bot > screen_ਪੰਕਤੀਆਂ))
	    pum_free_bg();

	ਜੇ (opacity_ਸਰਗਰਮ && pum_bg_ਸਤਰਾਂ != ਨਿਰਾ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (opacity_ਸਰਗਰਮ)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ਜੇ (Screen_ਸਤਰਾਂ != ਨਿਰਾ && ScreenAttrs != ਨਿਰਾ)
	    {
		ਪੂਰਨ	save_ਸਿਖਰ, save_bot, save_ncells, k;

		pum_free_bg();
		save_ਸਿਖਰ = pum_ਪੰਕਤੀ - pum_border;
		save_bot = pum_ਪੰਕਤੀ + pum_ਉਚਾਈ + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ਜੇ (save_ਸਿਖਰ < 0)
		    save_ਸਿਖਰ = 0;
		ਜੇ (save_bot > screen_ਪੰਕਤੀਆਂ)
		    save_bot = screen_ਪੰਕਤੀਆਂ;
		pum_bg_ਸਿਖਰ = save_ਸਿਖਰ;
		pum_bg_bot = save_bot;
		pum_bg_ਖਾਨੇ_ਗਿਣਤੀ = screen_Columns;
		ਜੇ (save_ਸਿਖਰ < save_bot)
		{
		    save_ncells = (save_bot - save_ਸਿਖਰ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_ਸਤਰਾਂ = LALLOC_MULT(schar_T, save_ncells);
		    ਜੇ (enc_utf8)
		    {
			pum_bg_ਸਤਰਾਂ_UC = LALLOC_MULT(u8char_T, save_ncells);
			ਲਈ (k = 0; k < MAX_MCO; ++k)
			    pum_bg_ਸਤਰਾਂ_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ਜੇ (pum_bg_attrs != ਨਿਰਾ && pum_bg_ਸਤਰਾਂ != ਨਿਰਾ)
		    {
			ਲਈ (ਪੂਰਨ r = save_ਸਿਖਰ; r < save_bot; ++r)
			{
			    ਪੂਰਨ soff = (r - save_ਸਿਖਰ) * screen_Columns;
			    ਪੂਰਨ loff = ਸਤਰ_ਵਿੱਥ[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * ਆਕਾਰ(sattr_T));
			    mch_memmove(pum_bg_ਸਤਰਾਂ + soff,
				    Screen_ਸਤਰਾਂ + loff,
				    screen_Columns * ਆਕਾਰ(schar_T));
			    ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ
						&& Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ)
			    {
				mch_memmove(pum_bg_ਸਤਰਾਂ_UC + soff,
					Screen_ਸਤਰਾਂ_UC + loff,
					screen_Columns * ਆਕਾਰ(u8char_T));
				ਲਈ (k = 0; k < MAX_MCO; ++k)
				    ਜੇ (pum_bg_ਸਤਰਾਂ_C[k] != ਨਿਰਾ
						    && Screen_ਸਤਰਾਂ_C[k] != ਨਿਰਾ)
					mch_memmove(pum_bg_ਸਤਰਾਂ_C[k] + soff,
						Screen_ਸਤਰਾਂ_C[k] + loff,
						screen_Columns
						    * ਆਕਾਰ(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Do not redraw in pum_may_redraw() and don't draw in the area
	    // where the popup menu will be.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
    }

    // never display more than we have
    pum_ਪਹਿਲਾ = MIN(pum_ਪਹਿਲਾ, scroll_range);

    ਜੇ (pum_scrollbar)
    {
	thumb_ਉਚਾਈ = pum_ਉਚਾਈ * pum_ਉਚਾਈ / pum_ਮਾਪ;
	ਜੇ (thumb_ਉਚਾਈ == 0)
	    thumb_ਉਚਾਈ = 1;
	thumb_pos = (pum_ਪਹਿਲਾ * (pum_ਉਚਾਈ - thumb_ਉਚਾਈ)
			    + scroll_range / 2) / scroll_range;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#ਜੇ_ਅੰਤ

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    ਜੇ (pum_border)
	pum_draw_border();
    ਜੇ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ਜੇ (opacity_ਸਰਗਰਮ)
	screen_pum_blend = 100 - (ਪੂਰਨ)p_po;

    ਲਈ (i = 0; i < pum_ਉਚਾਈ; ++i)
    {
	idx = i + pum_ਪਹਿਲਾ;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	{
	    ਜੇ (pum_ਖਾਨਾ < curwin->w_wincol + curwin->w_ਚੌੜਾਈ - 1
							    - pum_border)
		screen_putchar(' ', ਪੰਕਤੀ, pum_ਖਾਨਾ + 1, attr);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (pum_ਖਾਨਾ > pum_border)
		screen_putchar(' ', ਪੰਕਤੀ, pum_ਖਾਨਾ - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	ਖਾਨਾ = pum_ਖਾਨਾ;
	totwidth = 0;
	pum_align_ਕ੍ਰਮ(ਕ੍ਰਮ);
	basic_ਚੌੜਾਈ = ਚੀਜ਼ਾਂ_ਚੌੜਾਈ_array[ਕ੍ਰਮ[0]];
	ਆਖਰੀ_isabbr = ਕ੍ਰਮ[2] == CPT_ABBR;
	ਲਈ (j = 0; j < 3; ++j)
	{
	    ਵਸਤੂ_type = ਕ੍ਰਮ[j];
	    hlf = hlfs[ਵਸਤੂ_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ਜੇ (ਵਸਤੂ_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, ਵਸਤੂ_type, attr);
	    p = pum_get_ਵਸਤੂ(idx, ਵਸਤੂ_type);

	    ਜੇ (j + 1 < 3)
		ਅਗਲਾ_isempty = pum_get_ਵਸਤੂ(idx, ਕ੍ਰਮ[j + 1]) == ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		ਅਗਲਾ_isempty = TRUE;

	    ਜੇ (p != ਨਿਰਾ)
		// Process and display the item
		ਖਾਨਾ = pum_process_ਵਸਤੂ(ਪੰਕਤੀ, ਖਾਨਾ, idx, j, ਕ੍ਰਮ, hlf, attr,
						    &totwidth, ਅਗਲਾ_isempty);

	    ਜੇ (j > 0)
		n = ਚੀਜ਼ਾਂ_ਚੌੜਾਈ_array[ਕ੍ਰਮ[1]] + (ਆਖਰੀ_isabbr ? 0 : 1);
	    ਨਹੀਂਤਾਂ
		n = ਕ੍ਰਮ[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ਜੇ (j == 2
		    || (ਅਗਲਾ_isempty && (j == 1 || (j == 0
				&& pum_get_ਵਸਤੂ(idx, ਕ੍ਰਮ[j + 2]) == ਨਿਰਾ)))
		    || basic_ਚੌੜਾਈ + n >= pum_ਚੌੜਾਈ)
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (pum_rl)
	    {
		screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, pum_ਖਾਨਾ - basic_ਚੌੜਾਈ - n + 1,
						ਖਾਨਾ + 1, ' ', ' ', orig_attr);
		ਖਾਨਾ = pum_ਖਾਨਾ - basic_ਚੌੜਾਈ - n;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ, pum_ਖਾਨਾ + basic_ਚੌੜਾਈ + n,
							' ', ' ', orig_attr);
		ਖਾਨਾ = pum_ਖਾਨਾ + basic_ਚੌੜਾਈ + n;
	    }
	    totwidth = basic_ਚੌੜਾਈ + n;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, pum_ਖਾਨਾ - pum_ਚੌੜਾਈ + 1, ਖਾਨਾ + 1,
							    ' ', ' ', orig_attr);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ, pum_ਖਾਨਾ + pum_ਚੌੜਾਈ, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(ਪੰਕਤੀ, i, thumb_pos, thumb_ਉਚਾਈ);

	++ਪੰਕਤੀ;
    }

    screen_pum_blend = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    screen_zindex = 0;
#ਜੇ_ਅੰਤ

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();

    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_DISABLE);
}

#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    ਖਾਲੀ
pum_position_info_popup(win_T *wp)
{
    ਪੂਰਨ ਖਾਨਾ = pum_ਖਾਨਾ + pum_ਚੌੜਾਈ + pum_scrollbar + 1;
    ਪੂਰਨ ਪੰਕਤੀ = pum_ਪੰਕਤੀ;
    ਪੂਰਨ botpos = POPPOS_BOTLEFT;
    ਪੂਰਨ	ਵਰਤਿਆ_maxwidth_opt = FALSE;
    ਪੂਰਨ	ਸੱਜਾ_margin, ਖੱਬਾ_margin, dummy;
    ਪੂਰਨ ਖੱਬਾ_extra_ਚੌੜਾਈ, ਸੱਜਾ_extra_ਚੌੜਾਈ;

    (ਖਾਲੀ)compute_margins(&ਸੱਜਾ_margin, &ਖੱਬਾ_margin, &dummy);

    ਖੱਬਾ_extra_ਚੌੜਾਈ = pum_border + ਖੱਬਾ_margin;
    ਸੱਜਾ_extra_ਚੌੜਾਈ = pum_border + ਸੱਜਾ_margin + (pum_shadow ? 2 : 0);
    ਖਾਨਾ += ਸੱਜਾ_extra_ਚੌੜਾਈ;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ਜੇ (Columns - ਖਾਨਾ < 20 && Columns - ਖਾਨਾ < pum_ਖਾਨਾ)
    {
	ਖਾਨਾ = pum_ਖਾਨਾ - 1 - ਖੱਬਾ_extra_ਚੌੜਾਈ;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_ਖਾਨਾ - 1 - ਖੱਬਾ_extra_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
	wp->w_maxwidth = Columns - ਖਾਨਾ + 1;
    wp->w_maxwidth -= popup_extra_ਚੌੜਾਈ(wp);
    ਜੇ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	ਵਰਤਿਆ_maxwidth_opt = TRUE;
    }

    ਪੰਕਤੀ -= popup_ਸਿਖਰ_extra(wp);
    ਜੇ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ਜੇ (pum_ਪੰਕਤੀ < pum_win_ਪੰਕਤੀ)
	{
	    // menu above cursor line, align with bottom
	    ਪੰਕਤੀ += pum_ਉਚਾਈ;
	    wp->w_popup_pos = botpos;
	}
	ਨਹੀਂਤਾਂ
	    // menu below cursor line, align with top
	    ਪੰਕਤੀ += 1;
    }
    ਨਹੀਂਤਾਂ
	// align with the selected item
	ਪੰਕਤੀ += pum_selected - pum_ਪਹਿਲਾ + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ਜੇ (wp->w_maxwidth < 10 && !ਵਰਤਿਆ_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ਨਹੀਂਤਾਂ
	popup_ਸੈੱਟ_wantpos_rowcol(wp, ਪੰਕਤੀ, ਖਾਨਾ);
}
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ ਪੂਰਨ
pum_ਸੈੱਟ_selected(ਪੂਰਨ n, ਪੂਰਨ repeat ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    resized = FALSE;
    ਪੂਰਨ	    context = pum_ਉਚਾਈ / 2;
    ਪੂਰਨ	    scroll_ਵਿੱਥ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਪੂਰਨ	    prev_selected = pum_selected;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	cur_cot_flags = get_cot_flags();
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ਪੂਰਨ	    has_info = FALSE;
#ਜੇ_ਅੰਤ

    pum_selected = n;
    scroll_ਵਿੱਥ = pum_selected - pum_ਉਚਾਈ;

    ਜੇ (pum_selected >= 0 && pum_selected < pum_ਮਾਪ)
    {
	ਜੇ (pum_ਪਹਿਲਾ > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ਜੇ (pum_ਪਹਿਲਾ > pum_selected - 2)
	    {
		pum_ਪਹਿਲਾ -= pum_ਉਚਾਈ - 2;
		ਜੇ (pum_ਪਹਿਲਾ < 0)
		    pum_ਪਹਿਲਾ = 0;
		ਨਹੀਂਤਾਂ ਜੇ (pum_ਪਹਿਲਾ > pum_selected)
		    pum_ਪਹਿਲਾ = pum_selected;
	    }
	    ਨਹੀਂਤਾਂ
		pum_ਪਹਿਲਾ = pum_selected;
	}
	ਨਹੀਂਤਾਂ ਜੇ (pum_ਪਹਿਲਾ < scroll_ਵਿੱਥ + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ਜੇ (pum_ਪਹਿਲਾ < scroll_ਵਿੱਥ + 3)
		pum_ਪਹਿਲਾ = MAX(pum_ਪਹਿਲਾ + pum_ਉਚਾਈ - 2, scroll_ਵਿੱਥ + 1);
	    ਨਹੀਂਤਾਂ
		pum_ਪਹਿਲਾ = scroll_ਵਿੱਥ + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ਜੇ (pum_ਉਚਾਈ > 2)
	{
	    ਜੇ (pum_ਪਹਿਲਾ > pum_selected - context)
		pum_ਪਹਿਲਾ = MAX(pum_selected - context, 0);  // scroll down
	    ਨਹੀਂਤਾਂ ਜੇ (pum_ਪਹਿਲਾ < pum_selected + context - pum_ਉਚਾਈ + 1)
		pum_ਪਹਿਲਾ = pum_selected + context - pum_ਉਚਾਈ + 1;  //ਉੱਪਰ
	}
	// adjust for the number of lines displayed
	pum_ਪਹਿਲਾ = MIN(pum_ਪਹਿਲਾ, pum_ਮਾਪ - pum_ਉਚਾਈ);

#ਜੇ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ਜੇ (pum_array[pum_selected].pum_info != ਨਿਰਾ
		&& ਪੰਕਤੀਆਂ > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ਨਹੀਂਤਾਂ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ਜੇ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ਨਹੀਂਤਾਂ ਜੇ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_ਸਾਧਾਰਨ;
	    ਨਹੀਂਤਾਂ
		use_popup = USEPOPUP_NONE;
	    ਜੇ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		ਖੰਡ_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ਜੇ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ਜੇ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ਜੇ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		ਪੂਰਨ	res = OK;

		ਜੇ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ਨਿਰਾ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ਜਦੋਂ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE, 0, ਨਿਰਾ);
		    --no_u_sync;
		    ਜੇ (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"swf",
							  0L, ਨਿਰਾ, OPT_LOCAL);
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bl",
							  0L, ਨਿਰਾ, OPT_LOCAL);
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"diff",
							  0L, ਨਿਰਾ, OPT_LOCAL);
		    }
		}
		ਜੇ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    ਲਈ (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ਜੇ (e == ਨਿਰਾ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ਰੋਕੋ;
			}
			*e = NUL;
			ml_append(lnum++, p, (ਪੂਰਨ)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ਜੇ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ਜੇ (curwin->w_ਉਚਾਈ < lnum)
			{
			    win_setheight((ਪੂਰਨ)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_ਬਦਲਿਆ = 0;
		    curbuf->b_p_ma = FALSE;
		    ਜੇ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			curwin->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.ਖਾਨਾ = 0;
# ifdef FEAT_PROP_POPUP
		    ਜੇ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ਜੇ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ਜੇ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			ਪੂਰਨ save_redr_ਸਥਿਤੀ;

			ਜੇ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ਜੇ (ins_compl_ਸਰਗਰਮ() && !resized)
			    curwin->w_redr_ਸਥਿਤੀ = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ਜੇ (resized && win_valid(curwin_save))
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
			save_redr_ਸਥਿਤੀ = curwin_save->w_redr_ਸਥਿਤੀ;
			curwin_save->w_redr_ਸਥਿਤੀ = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_ਸਥਿਤੀ = save_redr_ਸਥਿਤੀ;

			ਜੇ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ਜੇ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ਜੇ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ਜੇ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#ਜੇ_ਅੰਤ
    }
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ਜੇ (!has_info)
	// hide any popup info window
	popup_hide_info();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    ਖਾਲੀ
pum_undisplay(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_POPUP
    ਜੇ (pum_imagep != ਨਿਰਾ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ਨਿਰਾ;
    }
#ਜੇ_ਅੰਤ
    pum_free_bg();
    pum_array = ਨਿਰਾ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (pum_ਵਿੱਚ_cmdline)
    {
	clear_cmdline = TRUE;
	pum_ਵਿੱਚ_cmdline = FALSE;
    }
    ਸਥਿਤੀ_redraw_all();
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#ਜੇ_ਅੰਤ
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    ਖਾਲੀ
pum_clear(ਖਾਲੀ)
{
    pum_ਪਹਿਲਾ = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    ਪੂਰਨ
pum_visible(ਖਾਲੀ)
{
    ਵਾਪਸ !pum_pretend_not_visible && pum_array != ਨਿਰਾ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ਸਥਿਰ ਪੂਰਨ
pum_ਵਿੱਚ_same_position(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਪੰਕਤੀ = (State & ਬਹੁਲਕ_CMDLINE)
			? cmdline_ਪੰਕਤੀ
			: curwin->w_wrow + W_WINROW(curwin);

    ਵਾਪਸ pum_ਖਿੜਕੀ != curwin
	    || (pum_win_ਪੰਕਤੀ == ਪੰਕਤੀ
		&& pum_win_ਉਚਾਈ == curwin->w_ਉਚਾਈ
		&& pum_win_ਖਾਨਾ == curwin->w_wincol
		&& pum_win_ਚੌੜਾਈ == curwin->w_ਚੌੜਾਈ);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    ਪੂਰਨ
pum_redraw_ਵਿੱਚ_same_position(ਖਾਲੀ)
{
    ਜੇ (!pum_visible() || pum_will_redraw)
	ਵਾਪਸ FALSE;  // nothing to do

    ਵਾਪਸ pum_ਵਿੱਚ_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    ਖਾਲੀ
pum_may_redraw(ਖਾਲੀ)
{
    pumitem_T	*array = pum_array;
    ਪੂਰਨ		len = pum_ਮਾਪ;
    ਪੂਰਨ		selected = pum_selected;

    ਜੇ (!pum_visible() || pum_will_redraw)
	ਵਾਪਸ;  // nothing to do

    ਜੇ (pum_ਵਿੱਚ_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ਨਹੀਂਤਾਂ
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
    ਪੂਰਨ
pum_get_ਉਚਾਈ(ਖਾਲੀ)
{
    ਵਾਪਸ pum_ਉਚਾਈ;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    ਖਾਲੀ
pum_ਸੈੱਟ_event_info(dict_T *dict)
{
    ਜੇ (!pum_visible())
	ਵਾਪਸ;
    (ਖਾਲੀ)dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "height", pum_ਉਚਾਈ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "width", pum_ਚੌੜਾਈ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "row", pum_ਪੰਕਤੀ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", pum_ਖਾਨਾ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "size", pum_ਮਾਪ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL_ਪਦ) || defined(FEAT_ਪਦ_POPUP_MENU)
    ਸਥਿਰ ਖਾਲੀ
pum_position_at_mouse(ਪੂਰਨ min_ਚੌੜਾਈ)
{
    ਪੂਰਨ extra_ਉਚਾਈ = pum_border * 2 + pum_shadow;
    ਪੂਰਨ extra_ਵਿੱਥ = pum_border + pum_shadow;
    ਪੂਰਨ trailing_overhead = 0;
    ਪੂਰਨ ਖਾਨਾ_ਵਿੱਥ = 0;
    ਪੂਰਨ ਉਪਲਬਧ_ਚੌੜਾਈ;

    ਜੇ (ਪੰਕਤੀਆਂ - mouse_ਪੰਕਤੀ - extra_ਉਚਾਈ > pum_ਮਾਪ
	    || ਪੰਕਤੀਆਂ - mouse_ਪੰਕਤੀ > mouse_ਪੰਕਤੀ)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ + 1 + pum_border;
	ਜੇ (pum_ਉਚਾਈ > ਪੰਕਤੀਆਂ - pum_ਪੰਕਤੀ - extra_ਉਚਾਈ)
	    pum_ਉਚਾਈ = ਪੰਕਤੀਆਂ - pum_ਪੰਕਤੀ - extra_ਉਚਾਈ;
	ਜੇ (pum_ਪੰਕਤੀ + pum_ਉਚਾਈ + extra_ਵਿੱਥ >= cmdline_ਪੰਕਤੀ)
	    pum_ਵਿੱਚ_cmdline = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ - pum_ਮਾਪ - extra_ਵਿੱਥ;
	ਜੇ (pum_ਪੰਕਤੀ < pum_border)
	{
	    pum_ਉਚਾਈ += pum_ਪੰਕਤੀ - pum_border;
	    pum_ਪੰਕਤੀ = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	ਖਾਨਾ_ਵਿੱਥ = (mouse_ਖਾਨਾ == Columns - 1) && pum_border;
	ਉਪਲਬਧ_ਚੌੜਾਈ = mouse_ਖਾਨਾ + 1 - trailing_overhead - ਖਾਨਾ_ਵਿੱਥ;
	ਜੇ (ਉਪਲਬਧ_ਚੌੜਾਈ >= pum_ਅਧਾਰ_ਚੌੜਾਈ || ਉਪਲਬਧ_ਚੌੜਾਈ > min_ਚੌੜਾਈ)
	    // Enough space to show at mouse column.
	    pum_ਖਾਨਾ = mouse_ਖਾਨਾ - ਖਾਨਾ_ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    // Not enough space, left align with window.
	    pum_ਖਾਨਾ = MIN(pum_ਅਧਾਰ_ਚੌੜਾਈ, min_ਚੌੜਾਈ) - 1 + trailing_overhead;
	pum_ਚੌੜਾਈ = pum_ਖਾਨਾ + 1 - trailing_overhead;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	ਖਾਨਾ_ਵਿੱਥ = (mouse_ਖਾਨਾ == 0) && pum_border;
	ਉਪਲਬਧ_ਚੌੜਾਈ = Columns - mouse_ਖਾਨਾ - trailing_overhead - ਖਾਨਾ_ਵਿੱਥ;
	ਜੇ (ਉਪਲਬਧ_ਚੌੜਾਈ >= pum_ਅਧਾਰ_ਚੌੜਾਈ || ਉਪਲਬਧ_ਚੌੜਾਈ > min_ਚੌੜਾਈ)
	    // Enough space to show at mouse column.
	    pum_ਖਾਨਾ = mouse_ਖਾਨਾ + ਖਾਨਾ_ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    // Not enough space, right align with window.
	    pum_ਖਾਨਾ = Columns - MIN(pum_ਅਧਾਰ_ਚੌੜਾਈ, min_ਚੌੜਾਈ)
		- trailing_overhead;
	pum_ਚੌੜਾਈ = Columns - pum_ਖਾਨਾ - trailing_overhead;
    }

    pum_ਚੌੜਾਈ = MIN(pum_ਚੌੜਾਈ, pum_ਅਧਾਰ_ਚੌੜਾਈ + 1);
    // Do not redraw at cursor position.
    pum_ਖਿੜਕੀ = ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL_ਪਦ)
ਸਥਿਰ pumitem_T *balloon_array = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ balloon_arraysize;

# define BALLOON_MIN_ਚੌੜਾਈ 50
# define BALLOON_MIN_ਉਚਾਈ 10

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*ਸ਼ੁਰੂ;
    ਪੂਰਨ		bytelen;
    ਪੂਰਨ		cells;
    ਪੂਰਨ		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    ਪੂਰਨ
ਵੰਡੋ_ਸੁਨੇਹਾ(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*ਵਸਤੂ;
    ਪੂਰਨ		quoted = FALSE;
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		ਸਤਰ;
    ਪੂਰਨ		ਵਸਤੂ_idx;
    ਪੂਰਨ		indent = 0;
    ਪੂਰਨ		max_cells = 0;
    ਪੂਰਨ		max_ਉਚਾਈ_2 = ਪੰਕਤੀਆਂ / 2 - 1;
    ਪੂਰਨ		long_ਵਸਤੂ_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		ਵੰਡੋ_long_ਚੀਜ਼ਾਂ = FALSE;

    ga_init2(&ga, ਆਕਾਰ(balpart_T), 20);
    p = mesg;

    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (ga_grow(&ga, 1) == FAIL)
	    ਜਾਓ failed;
	ਵਸਤੂ = ((balpart_T *)ga.ga_ਅੰਕੜੇ) + ga.ga_len;
	ਵਸਤੂ->ਸ਼ੁਰੂ = p;
	ਵਸਤੂ->indent = indent;
	ਵਸਤੂ->cells = indent * 2;
	++ga.ga_len;
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ (*p == '"')
		quoted = !quoted;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '\n')
		ਰੋਕੋ;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' && p[1] != NUL)
		++p;
	    ਨਹੀਂਤਾਂ ਜੇ (!quoted)
	    {
		ਜੇ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ਜੇ (*p == '{')
			++indent;
		    ਨਹੀਂਤਾਂ ਜੇ (*p == '}' && indent > 0)
			--indent;
		    ++ਵਸਤੂ->cells;
		    p = skipwhite(p + 1);
		    ਰੋਕੋ;
		}
	    }
	    ਵਸਤੂ->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	ਵਸਤੂ->bytelen = p - ਵਸਤੂ->ਸ਼ੁਰੂ;
	ਜੇ (*p == '\n')
	    ++p;
	ਜੇ (ਵਸਤੂ->cells > max_cells)
	    max_cells = ਵਸਤੂ->cells;
	long_ਵਸਤੂ_ਗਿਣਤੀ += (ਵਸਤੂ->cells - 1) / BALLOON_MIN_ਚੌੜਾਈ;
    }

    ਉਚਾਈ = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ਜੇ (long_ਵਸਤੂ_ਗਿਣਤੀ > 0 && ਉਚਾਈ + long_ਵਸਤੂ_ਗਿਣਤੀ <= max_ਉਚਾਈ_2)
    {
	ਵੰਡੋ_long_ਚੀਜ਼ਾਂ = TRUE;
	ਉਚਾਈ += long_ਵਸਤੂ_ਗਿਣਤੀ;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ਜੇ (ਉਚਾਈ > max_ਉਚਾਈ_2)
	ਉਚਾਈ = max_ਉਚਾਈ_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, ਉਚਾਈ);
    ਜੇ (*array == ਨਿਰਾ)
	ਜਾਓ failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_ਪਾਠ = vim_strsave((char_u *)"");
    (*array + ਉਚਾਈ - 1)->pum_ਪਾਠ = vim_strsave((char_u *)"");

    ਲਈ (ਸਤਰ = 1, ਵਸਤੂ_idx = 0; ਸਤਰ < ਉਚਾਈ - 1; ++ਵਸਤੂ_idx)
    {
	ਪੂਰਨ	skip;
	ਪੂਰਨ	thislen;
	ਪੂਰਨ	copylen;
	ਪੂਰਨ	ind;
	ਪੂਰਨ	cells;

	ਵਸਤੂ = ((balpart_T *)ga.ga_ਅੰਕੜੇ) + ਵਸਤੂ_idx;
	ਜੇ (ਵਸਤੂ->bytelen == 0)
	    (*array)[ਸਤਰ++].pum_ਪਾਠ = vim_strsave((char_u *)"");
	ਨਹੀਂਤਾਂ
	    ਲਈ (skip = 0; skip < ਵਸਤੂ->bytelen; skip += thislen)
	    {
		ਜੇ (ਵੰਡੋ_long_ਚੀਜ਼ਾਂ && ਵਸਤੂ->cells >= BALLOON_MIN_ਚੌੜਾਈ)
		{
		    cells = ਵਸਤੂ->indent * 2;
		    ਲਈ (p = ਵਸਤੂ->ਸ਼ੁਰੂ + skip;
			    p < ਵਸਤੂ->ਸ਼ੁਰੂ + ਵਸਤੂ->bytelen;
							    p += mb_ptr2len(p))
			ਜੇ ((cells += ptr2cells(p)) > BALLOON_MIN_ਚੌੜਾਈ)
			    ਰੋਕੋ;
		    thislen = p - (ਵਸਤੂ->ਸ਼ੁਰੂ + skip);
		}
		ਨਹੀਂਤਾਂ
		    thislen = ਵਸਤੂ->bytelen;

		// put indent at the start
		p = alloc(thislen + ਵਸਤੂ->indent * 2 + 1);
		ਜੇ (p == ਨਿਰਾ)
		{
		    ਲਈ (ਸਤਰ = 0; ਸਤਰ <= ਉਚਾਈ - 1; ++ਸਤਰ)
			vim_free((*array)[ਸਤਰ].pum_ਪਾਠ);
		    vim_free(*array);
		    ਜਾਓ failed;
		}
		ਲਈ (ind = 0; ind < ਵਸਤੂ->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		ਲਈ (copylen = thislen; copylen > 0; --copylen)
		    ਜੇ (ਵਸਤੂ->ਸ਼ੁਰੂ[skip + copylen - 1] != ' ')
			ਰੋਕੋ;

		vim_strncpy(p + ind, ਵਸਤੂ->ਸ਼ੁਰੂ + skip, copylen);
		(*array)[ਸਤਰ].pum_ਪਾਠ = p;
		ਵਸਤੂ->indent = 0;  // wrapped line has no indent
		++ਸਤਰ;
	    }
    }
    ga_clear(&ga);
    ਵਾਪਸ ਉਚਾਈ;

failed:
    ga_clear(&ga);
    ਵਾਪਸ 0;
}

    ਖਾਲੀ
ui_remove_balloon(ਖਾਲੀ)
{
    ਜੇ (balloon_array == ਨਿਰਾ)
	ਵਾਪਸ;

    pum_undisplay();
    ਜਦੋਂ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_ਪਾਠ);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    ਖਾਲੀ
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    ਜੇ (mesg == ਨਿਰਾ && list == ਨਿਰਾ)
    {
	pum_undisplay();
	ਵਾਪਸ;
    }
    ਜੇ (list != ਨਿਰਾ)
    {
	listitem_T  *li;
	ਪੂਰਨ	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ਜੇ (balloon_array == ਨਿਰਾ)
	    ਵਾਪਸ;
	CHECK_LIST_MATERIALIZE(list);
	ਲਈ (idx = 0, li = list->lv_ਪਹਿਲਾ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ, ++idx)
	{
	    char_u *ਪਾਠ = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_ਪਾਠ = vim_strsave(
					   ਪਾਠ == ਨਿਰਾ ? (char_u *)"" : ਪਾਠ);
	}
    }
    ਨਹੀਂਤਾਂ
	balloon_arraysize = ਵੰਡੋ_ਸੁਨੇਹਾ(mesg, &balloon_array);

    ਜੇ (balloon_arraysize <= 0)
	ਵਾਪਸ;

    pum_array = balloon_array;
    pum_ਮਾਪ = balloon_arraysize;
    pum_compute_ਮਾਪ();
    pum_scrollbar = 0;
    pum_ਉਚਾਈ = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_ਚੌੜਾਈ);
    pum_selected = -1;
    pum_ਪਹਿਲਾ = 0;
    pum_redraw();
}

/*
 * Called when the mouse moved, may remove any displayed balloon.
 */
    ਖਾਲੀ
ui_may_remove_balloon(ਖਾਲੀ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਪਦ_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_select_mouse_pos(ਖਾਲੀ)
{
    ਪੂਰਨ idx = mouse_ਪੰਕਤੀ - pum_ਪੰਕਤੀ;

    ਜੇ (idx < 0 || idx >= pum_ਉਚਾਈ)
	pum_selected = -1;
    ਨਹੀਂਤਾਂ ਜੇ (*pum_array[idx].pum_ਪਾਠ != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ਸਥਿਰ ਖਾਲੀ
pum_execute_menu(vimmenu_T *menu, ਪੂਰਨ ਬਹੁਲਕ)
{
    vimmenu_T   *mp;
    ਪੂਰਨ		idx = 0;
    exarg_T	ea;

    FOR_ALL_ਬੱਚਾ_MENUS(menu, mp)
	ਜੇ ((mp->modes & mp->enabled & ਬਹੁਲਕ) && idx++ == pum_selected)
	{
	    CLEAR_ਖੇਤਰ(ea);
	    execute_menu(&ea, mp, -1);
	    ਰੋਕੋ;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    ਖਾਲੀ
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    ਪੂਰਨ		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_ਪਦ
    ਪੂਰਨ		save_bevalterm = p_bevalterm;
# endif
    ਪੂਰਨ		ਬਹੁਲਕ;

    pum_undisplay();
    pum_ਮਾਪ = 0;
    ਬਹੁਲਕ = get_menu_ਬਹੁਲਕ_flag();

    FOR_ALL_ਬੱਚਾ_MENUS(menu, mp)
	ਜੇ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & ਬਹੁਲਕ))
	    ++pum_ਮਾਪ;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ਜੇ (pum_ਮਾਪ <= 0)
    {
	emsg(_(e_menu_only_exists_ਵਿੱਚ_another_ਬਹੁਲਕ));
	ਵਾਪਸ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_ਮਾਪ);
    ਜੇ (array == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_ਬੱਚਾ_MENUS(menu, mp)
    {
	char_u *s = ਨਿਰਾ;

	// Make a copy of the text, the menu may be redefined in a callback.
	ਜੇ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ਨਹੀਂਤਾਂ ਜੇ (mp->modes & mp->enabled & ਬਹੁਲਕ)
	    s = mp->dname;
	ਜੇ (s != ਨਿਰਾ)
	{
	    s = vim_strsave(s);
	    ਜੇ (s != ਨਿਰਾ)
		array[idx++].pum_ਪਾਠ = s;
	}
    }

    pum_array = array;
    pum_compute_ਮਾਪ();
    pum_scrollbar = 0;
    pum_ਉਚਾਈ = pum_ਮਾਪ;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_ਪਹਿਲਾ = 0;
# ifdef FEAT_BEVAL_ਪਦ
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    ਲਈ (;;)
    {
	ਪੂਰਨ	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	ਬਾਹਰਲਾ_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ਜੇ (c == ESC || c == Ctrl_C || pum_array == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, ਬਹੁਲਕ);
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == 'k' || c == K_ਉੱਪਰ || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ਜਦੋਂ (pum_selected > 0)
	    {
		--pum_selected;
		ਜੇ (*array[pum_selected].pum_ਪਾਠ != NUL)
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == 'j' || c == K_ਥੱਲੇ || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ਜਦੋਂ (pum_selected < pum_ਮਾਪ - 1)
	    {
		++pum_selected;
		ਜੇ (*array[pum_selected].pum_ਪਾਠ != NUL)
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ਜੇ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, ਬਹੁਲਕ);
		ਰੋਕੋ;
	    }
	    ਜੇ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ਰੋਕੋ;
	}
    }

    ਲਈ (idx = 0; idx < pum_ਮਾਪ; ++idx)
	vim_free(array[idx].pum_ਪਾਠ);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_ਪਦ
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    ਖਾਲੀ
pum_make_popup(char_u *ਰਸਤਾ_name, ਪੂਰਨ use_mouse_pos)
{
    vimmenu_T *menu;

    ਜੇ (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_ਪੰਕਤੀ = W_WINROW(curwin) + curwin->w_wrow;
	mouse_ਖਾਨਾ =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_ਲੱਭੋ_menu(ਰਸਤਾ_name);
    ਜੇ (menu != ਨਿਰਾ)
	pum_show_popupmenu(menu);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
is_singlewidth(ਪੂਰਨ c)
{
    ਜੇ (c >= 0 && c < 0x80)
	ਵਾਪਸ TRUE;
    ਜੇ (utf_char2cells(c) == 1)
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    ਖਾਲੀ
pum_ਸੈੱਟ_border_chars(
	ਪੂਰਨ ਸਿਖਰ,
	ਪੂਰਨ ਸੱਜਾ,
	ਪੂਰਨ bottom,
	ਪੂਰਨ ਖੱਬਾ,
	ਪੂਰਨ ਸਿਖਰ_ਖੱਬਾ,
	ਪੂਰਨ ਸਿਖਰ_ਸੱਜਾ,
	ਪੂਰਨ bottom_ਸੱਜਾ,
	ਪੂਰਨ bottom_ਖੱਬਾ)
{
    ਜੇ (!is_singlewidth(ਸਿਖਰ) || !is_singlewidth(ਸੱਜਾ)
	    || !is_singlewidth(bottom) || !is_singlewidth(ਖੱਬਾ)
	    || !is_singlewidth(ਸਿਖਰ_ਖੱਬਾ) || !is_singlewidth(ਸਿਖਰ_ਸੱਜਾ)
	    || !is_singlewidth(bottom_ਸੱਜਾ) || !is_singlewidth(bottom_ਖੱਬਾ))
	ਵਾਪਸ;
    pum_border_chars.ਸਿਖਰ = ਸਿਖਰ;
    pum_border_chars.ਸੱਜਾ = ਸੱਜਾ;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ਖੱਬਾ = ਖੱਬਾ;
    pum_border_chars.ਸਿਖਰ_ਖੱਬਾ = ਸਿਖਰ_ਖੱਬਾ;
    pum_border_chars.ਸਿਖਰ_ਸੱਜਾ = ਸਿਖਰ_ਸੱਜਾ;
    pum_border_chars.bottom_ਸੱਜਾ = bottom_ਸੱਜਾ;
    pum_border_chars.bottom_ਖੱਬਾ = bottom_ਖੱਬਾ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
compute_margins(ਪੂਰਨ *ਸੱਜਾ_margin, ਪੂਰਨ *ਖੱਬਾ_margin, ਪੂਰਨ *ਖੱਬਾ_padding)
{
    *ਸੱਜਾ_margin = pum_margin && pum_border;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	*ਖੱਬਾ_padding = (pum_ਖਾਨਾ < Columns - 1 - pum_border);
	*ਖੱਬਾ_margin = *ਸੱਜਾ_margin && (pum_ਖਾਨਾ < Columns - 3);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    *ਖੱਬਾ_padding = (pum_ਖਾਨਾ > pum_border);
    *ਖੱਬਾ_margin = *ਸੱਜਾ_margin && (pum_ਖਾਨਾ > 2);
}

/*
 * Draw the border around the popup menu
 */
    ਸਥਿਰ ਖਾਲੀ
pum_draw_border(ਖਾਲੀ)
{
    ਪੂਰਨ	ਸਿਖਰ_ਪੰਕਤੀ = pum_ਪੰਕਤੀ - 1;
    ਪੂਰਨ	bottom_ਪੰਕਤੀ = pum_ਪੰਕਤੀ + pum_ਉਚਾਈ;
    ਪੂਰਨ	ਖਾਨਾ;
    ਪੂਰਨ	attr = highlight_attr[HLF_PMB];
    ਪੂਰਨ	ਸੱਜਾ_margin, ਖੱਬਾ_margin, ਖੱਬਾ_padding;
    ਪੂਰਨ	inner_ਚੌੜਾਈ;

    ਜੇ (!pum_border)
	ਵਾਪਸ;

    (ਖਾਲੀ)compute_margins(&ਸੱਜਾ_margin, &ਖੱਬਾ_margin, &ਖੱਬਾ_padding);

    inner_ਚੌੜਾਈ = pum_ਚੌੜਾਈ + pum_scrollbar + ਖੱਬਾ_padding;

    // Draw border left to right or right to left (for RTL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
	ਖਾਨਾ = pum_ਖਾਨਾ + ਖੱਬਾ_padding + 1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਖਾਨਾ = pum_ਖਾਨਾ - ਖੱਬਾ_padding - 1;

    // left corners (right corners for RTL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	screen_putchar(pum_border_chars.ਸਿਖਰ_ਸੱਜਾ, ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ, attr);
	ਜੇ (ਖੱਬਾ_margin)
	    screen_putchar(' ', ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ + 1, attr);
	screen_putchar(pum_border_chars.bottom_ਸੱਜਾ, bottom_ਪੰਕਤੀ, ਖਾਨਾ, attr);
	ਜੇ (ਖੱਬਾ_margin)
	    screen_putchar(' ', bottom_ਪੰਕਤੀ, ਖਾਨਾ + 1, attr);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	screen_putchar(pum_border_chars.ਸਿਖਰ_ਖੱਬਾ, ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ, attr);
	ਜੇ (ਖੱਬਾ_margin)
	    screen_putchar(' ', ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ - 1, attr);
	screen_putchar(pum_border_chars.bottom_ਖੱਬਾ, bottom_ਪੰਕਤੀ, ਖਾਨਾ, attr);
	ਜੇ (ਖੱਬਾ_margin)
	    screen_putchar(' ', bottom_ਪੰਕਤੀ, ਖਾਨਾ - 1, attr);
    }

    // Top horizontal lines
    ਲਈ (ਪੂਰਨ i = 0; i < inner_ਚੌੜਾਈ; i++)
    {
	ਪੂਰਨ cur_ਖਾਨਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    cur_ਖਾਨਾ = ਖਾਨਾ - 1 - i;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    cur_ਖਾਨਾ = ਖਾਨਾ + 1 + i;
	screen_putchar(pum_border_chars.ਸਿਖਰ, ਸਿਖਰ_ਪੰਕਤੀ, cur_ਖਾਨਾ, attr);
	screen_putchar(pum_border_chars.bottom, bottom_ਪੰਕਤੀ, cur_ਖਾਨਾ, attr);
    }

    // right corners (left corners for RTL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	screen_putchar(pum_border_chars.ਸਿਖਰ_ਖੱਬਾ, ਸਿਖਰ_ਪੰਕਤੀ,
		ਖਾਨਾ - inner_ਚੌੜਾਈ - 1, attr);
	ਜੇ (ਸੱਜਾ_margin)
	    screen_putchar(' ', ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ - inner_ਚੌੜਾਈ - 2, attr);
	screen_putchar(pum_border_chars.bottom_ਖੱਬਾ, bottom_ਪੰਕਤੀ,
		ਖਾਨਾ - inner_ਚੌੜਾਈ - 1, attr);
	ਜੇ (ਸੱਜਾ_margin)
	    screen_putchar(' ', bottom_ਪੰਕਤੀ, ਖਾਨਾ - inner_ਚੌੜਾਈ - 2, attr);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	screen_putchar(pum_border_chars.ਸਿਖਰ_ਸੱਜਾ, ਸਿਖਰ_ਪੰਕਤੀ,
		ਖਾਨਾ + inner_ਚੌੜਾਈ + 1, attr);
	ਜੇ (ਸੱਜਾ_margin)
	    screen_putchar(' ', ਸਿਖਰ_ਪੰਕਤੀ, ਖਾਨਾ + inner_ਚੌੜਾਈ + 2, attr);
	screen_putchar(pum_border_chars.bottom_ਸੱਜਾ, bottom_ਪੰਕਤੀ,
		ਖਾਨਾ + inner_ਚੌੜਾਈ + 1, attr);
	ਜੇ (ਸੱਜਾ_margin)
	    screen_putchar(' ', bottom_ਪੰਕਤੀ, ਖਾਨਾ + inner_ਚੌੜਾਈ + 2, attr);
    }

    // Draw side borders
    ਲਈ (ਪੂਰਨ i = 0; i < pum_ਉਚਾਈ; i++)
    {
	ਪੂਰਨ ਪੰਕਤੀ = pum_ਪੰਕਤੀ + i;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	{
	    screen_putchar(pum_border_chars.ਖੱਬਾ, ਪੰਕਤੀ, ਖਾਨਾ, attr);
	    ਜੇ (ਖੱਬਾ_margin)
		screen_putchar(' ', ਪੰਕਤੀ, ਖਾਨਾ + 1, attr);
	    screen_putchar(pum_border_chars.ਸੱਜਾ, ਪੰਕਤੀ,
		    ਖਾਨਾ - inner_ਚੌੜਾਈ - 1, attr);
	    ਜੇ (ਸੱਜਾ_margin)
		screen_putchar(' ', ਪੰਕਤੀ, ਖਾਨਾ - inner_ਚੌੜਾਈ - 2, attr);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    screen_putchar(pum_border_chars.ਖੱਬਾ, ਪੰਕਤੀ, ਖਾਨਾ, attr);
	    ਜੇ (ਖੱਬਾ_margin)
		screen_putchar(' ', ਪੰਕਤੀ, ਖਾਨਾ - 1, attr);
	    screen_putchar(pum_border_chars.ਸੱਜਾ, ਪੰਕਤੀ,
		    ਖਾਨਾ + inner_ਚੌੜਾਈ + 1, attr);
	    ਜੇ (ਸੱਜਾ_margin)
		screen_putchar(' ', ਪੰਕਤੀ, ਖਾਨਾ + inner_ਚੌੜਾਈ + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    ਖਾਲੀ
put_shadow_char(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    char_u  buf[MB_MAXBYTES + 1];
    ਪੂਰਨ	    shadow_attr = highlight_attr[HLF_PMS];
    ਪੂਰਨ	    char_attr, new_attr;

    screen_getbytes(ਪੰਕਤੀ, ਖਾਨਾ, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), ਪੰਕਤੀ, ਖਾਨਾ, new_attr);
}

/*
 * Draw the shadow
 */
    ਸਥਿਰ ਖਾਲੀ
pum_draw_shadow(ਖਾਲੀ)
{
    ਪੂਰਨ	ਪੰਕਤੀ, ਖਾਨਾ, ਅਗਲਾ_ਖਾਨਾ;
    ਪੂਰਨ	ਚੌੜਾਈ, ਵਿੱਥ;
    ਪੂਰਨ	inner_ਚੌੜਾਈ;
    ਪੂਰਨ	ਸੱਜਾ_margin, ਖੱਬਾ_margin, ਖੱਬਾ_padding;

    ਜੇ (!pum_shadow)
	ਵਾਪਸ;

    (ਖਾਲੀ)compute_margins(&ਸੱਜਾ_margin, &ਖੱਬਾ_margin, &ਖੱਬਾ_padding);
    inner_ਚੌੜਾਈ = pum_ਚੌੜਾਈ + pum_scrollbar + pum_border + ਸੱਜਾ_margin;

    // Draw right side shadow (left for RTL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (pum_rl)
    {
	ਖਾਨਾ = pum_ਖਾਨਾ - inner_ਚੌੜਾਈ;
	ਅਗਲਾ_ਖਾਨਾ = ਖਾਨਾ - 1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਖਾਨਾ = pum_ਖਾਨਾ + inner_ਚੌੜਾਈ;
	ਅਗਲਾ_ਖਾਨਾ = ਖਾਨਾ + 1;
    }

    ਲਈ (ਪੂਰਨ i = 0; i < pum_ਉਚਾਈ + pum_border; i++)
    {
	ਪੰਕਤੀ = pum_ਪੰਕਤੀ + 1 + i - pum_border;
	put_shadow_char(ਪੰਕਤੀ, ਖਾਨਾ);
	put_shadow_char(ਪੰਕਤੀ, ਅਗਲਾ_ਖਾਨਾ);
    }

    // Bottom shadow
    ਪੰਕਤੀ = pum_ਪੰਕਤੀ + pum_ਉਚਾਈ + pum_border;
    ਚੌੜਾਈ = inner_ਚੌੜਾਈ + ਖੱਬਾ_padding + pum_border + ਖੱਬਾ_margin;
    ਵਿੱਥ = 2 - ਖੱਬਾ_padding - pum_border - ਖੱਬਾ_margin;

    ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (pum_rl)
	    ਖਾਨਾ = pum_ਖਾਨਾ - ਵਿੱਥ - i;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਖਾਨਾ = pum_ਖਾਨਾ + ਵਿੱਥ + i;
	put_shadow_char(ਪੰਕਤੀ, ਖਾਨਾ);
    }
}
