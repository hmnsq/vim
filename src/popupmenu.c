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
#అన్తర్భావ "vim.h"

స్థిర pumitem_T *pum_array = ఖాళీ;	// items of displayed pum
స్థిర పూర్ణాంకం pum_కొలత;			// nr of items in "pum_array"
స్థిర పూర్ణాంకం pum_selected;		// index of selected item or -1
స్థిర పూర్ణాంకం pum_మొదటి = 0;		// index of top item

స్థిర పూర్ణాంకం call_update_screen = FALSE;
స్థిర పూర్ణాంకం pum_లోపల_cmdline = FALSE;

స్థిర పూర్ణాంకం pum_ఎత్తు;			// nr of displayed pum items
స్థిర పూర్ణాంకం pum_వెడల్పు;			// width of displayed pum items
స్థిర పూర్ణాంకం pum_పునాది_వెడల్పు;		// width of pum items base
స్థిర పూర్ణాంకం pum_రకం_వెడల్పు;		// width of pum items kind column
స్థిర పూర్ణాంకం pum_extra_వెడల్పు;		// width of extra stuff
స్థిర పూర్ణాంకం pum_scrollbar;		// TRUE when scrollbar present
#నిర్వచితమైతే FEAT_RIGHTLEFT
స్థిర పూర్ణాంకం pum_rl;			// TRUE when pum is drawn 'rightleft'
#అయితే_ముగింపు

స్థిర పూర్ణాంకం pum_అడ్డువరుస;			// top row of pum
స్థిర పూర్ణాంకం pum_నిలువు;			// left column of pum

స్థిర win_T *pum_కిటికీ = ఖాళీ;
స్థిర పూర్ణాంకం pum_win_అడ్డువరుస;
స్థిర పూర్ణాంకం pum_win_ఎత్తు;
స్థిర పూర్ణాంకం pum_win_నిలువు;
స్థిర పూర్ణాంకం pum_win_wcol;
స్థిర పూర్ణాంకం pum_win_వెడల్పు;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
స్థిర పూర్ణాంకం pum_pretend_not_visible = FALSE;

// Border support
స్థిర పూర్ణాంకం pum_border = 0;
స్థిర పూర్ణాంకం pum_margin = 0;	// margin of 1 cell on left and right
స్థిర పూర్ణాంకం pum_shadow = 0;

#నిర్వచితమైతే FEAT_IMAGE_POPUP
స్థిర image_placement_T *pum_imagep = ఖాళీ;
#అయితే_ముగింపు

// Border characters
స్థిర నిర్మాణం {
    పూర్ణాంకం శిఖరం;
    పూర్ణాంకం కుడి_2;
    పూర్ణాంకం bottom;
    పూర్ణాంకం ఎడమ_2;
    పూర్ణాంకం శిఖరం_ఎడమ;
    పూర్ణాంకం శిఖరం_కుడి;
    పూర్ణాంకం bottom_కుడి;
    పూర్ణాంకం bottom_ఎడమ;
} pum_border_chars;

స్థిర పూర్ణాంకం pum_సమితి_selected(పూర్ణాంకం n, పూర్ణాంకం repeat);
స్థిర శూన్య pum_draw_border(శూన్య);
స్థిర శూన్య pum_draw_shadow(శూన్య);
స్థిర శూన్య compute_margins(పూర్ణాంకం *కుడి_margin, పూర్ణాంకం *ఎడమ_margin, పూర్ణాంకం *ఎడమ_padding);

#నిర్వచించు PUM_DEF_ఎత్తు 10

#నిర్వచించు DEFINE_PUM_SETTER(feature) \
	శూన్య \
    pum_సమితి##feature(పూర్ణాంకం enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    స్థిర శూన్య
pum_compute_కొలత(శూన్య)
{
    పూర్ణాంకం	w;

    // Compute the width of the widest match and the widest extra.
    pum_పునాది_వెడల్పు = 0;
    pum_రకం_వెడల్పు = 0;
    pum_extra_వెడల్పు = 0;
    కొరకు (పూర్ణాంకం i = 0; i < pum_కొలత; ++i)
    {
	ఒకవేళ (pum_array[i].pum_పాఠ్యం != ఖాళీ)
	{
	    w = vim_strsize(pum_array[i].pum_పాఠ్యం);
	    ఒకవేళ (pum_పునాది_వెడల్పు < w)
		pum_పునాది_వెడల్పు = w;
	}
	ఒకవేళ (pum_array[i].pum_రకం != ఖాళీ)
	{
	    w = vim_strsize(pum_array[i].pum_రకం) + 1;
	    ఒకవేళ (pum_రకం_వెడల్పు < w)
		pum_రకం_వెడల్పు = w;
	}
	ఒకవేళ (pum_array[i].pum_extra != ఖాళీ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ఒకవేళ (pum_extra_వెడల్పు < w)
		pum_extra_వెడల్పు = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    స్థిర శూన్య
pum_compute_vertical_placement(
	పూర్ణాంకం కొలత,
	పూర్ణాంకం above_అడ్డువరుస,
	పూర్ణాంకం క్రింద_అడ్డువరుస)
{
    పూర్ణాంకం context_పంక్తులు;
    పూర్ణాంకం cline_visible_స్థానభ్రంశం;
    పూర్ణాంకం extra_ఎత్తు = 2 * pum_border + pum_shadow;
    పూర్ణాంకం extra_స్థానభ్రంశం = pum_border + pum_shadow;

    pum_ఎత్తు = MIN(కొలత, PUM_DEF_ఎత్తు);
    ఒకవేళ (p_ph > 0 && pum_ఎత్తు > p_ph)
	pum_ఎత్తు = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ఒకవేళ (pum_win_అడ్డువరుస + 2 >= క్రింద_అడ్డువరుస - pum_ఎత్తు - extra_ఎత్తు
	    && pum_win_అడ్డువరుస - above_అడ్డువరుస > (క్రింద_అడ్డువరుస - above_అడ్డువరుస) / 2)
    {
	// pum above "pum_win_row"
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_పంక్తులు = 0;
	లేకపోతే
	    // Leave two lines of context if possible
	    context_పంక్తులు = MIN(2, curwin->w_wrow - curwin->w_cline_అడ్డువరుస);

	ఒకవేళ (pum_win_అడ్డువరుస >= కొలత + context_పంక్తులు + extra_ఎత్తు)
	{
	    pum_అడ్డువరుస = pum_win_అడ్డువరుస - కొలత - context_పంక్తులు - extra_స్థానభ్రంశం;
	    pum_ఎత్తు = కొలత;
	}
	లేకపోతే
	{
	    pum_అడ్డువరుస = pum_border;
	    pum_ఎత్తు = pum_win_అడ్డువరుస - context_పంక్తులు - extra_ఎత్తు;
	}
	ఒకవేళ (p_ph > 0 && pum_ఎత్తు > p_ph)
	{
	    pum_అడ్డువరుస += pum_ఎత్తు - p_ph;
	    pum_ఎత్తు = p_ph;
	}
    }
    లేకపోతే
    {
	// pum below "pum_win_row"
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_పంక్తులు = 0;
	లేకపోతే
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_స్థానభ్రంశం = curwin->w_cline_అడ్డువరుస +
		curwin->w_cline_ఎత్తు - curwin->w_wrow;
	    context_పంక్తులు = MIN(3, cline_visible_స్థానభ్రంశం);
	}

	pum_అడ్డువరుస = pum_win_అడ్డువరుస + context_పంక్తులు + pum_border;
	pum_ఎత్తు = MIN(క్రింద_అడ్డువరుస - pum_అడ్డువరుస - extra_స్థానభ్రంశం, కొలత);
	ఒకవేళ (p_ph > 0 && pum_ఎత్తు > p_ph)
	    pum_ఎత్తు = p_ph;
    }

#ఒకవేళ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ఒకవేళ (above_అడ్డువరుస > 0 && pum_అడ్డువరుస < above_అడ్డువరుస && pum_ఎత్తు > above_అడ్డువరుస)
    {
	pum_అడ్డువరుస = above_అడ్డువరుస;
	pum_ఎత్తు = pum_win_అడ్డువరుస - above_అడ్డువరుస;
    }
#అయితే_ముగింపు
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    స్థిర పూర్ణాంకం
సమితి_pum_వెడల్పు_aligned_with_cursor(పూర్ణాంకం వెడల్పు, పూర్ణాంకం అందుబాటు_వెడల్పు)
{
    పూర్ణాంకం చివర_padding = TRUE;

    ఒకవేళ (వెడల్పు < p_pw)
    {
	వెడల్పు = p_pw;
	చివర_padding = FALSE;
    }
    ఒకవేళ (p_pmw > 0 && వెడల్పు >= p_pmw)
    {
	వెడల్పు = p_pmw;
	చివర_padding = FALSE;
    }

    pum_వెడల్పు = వెడల్పు + (చివర_padding && వెడల్పు >= p_pw ? 1 : 0);
    తిరిగి అందుబాటు_వెడల్పు >= pum_వెడల్పు;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    స్థిర శూన్య
pum_compute_horizontal_placement(పూర్ణాంకం cursor_నిలువు)
{
    పూర్ణాంకం desired_వెడల్పు = pum_పునాది_వెడల్పు + pum_రకం_వెడల్పు + pum_extra_వెడల్పు;
    పూర్ణాంకం అందుబాటు_వెడల్పు;
    పూర్ణాంకం నిలువు_స్థానభ్రంశం;
    పూర్ణాంకం trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	నిలువు_స్థానభ్రంశం = pum_border && (cursor_నిలువు >= Columns - 2)
	    ? (3 - Columns + cursor_నిలువు) : 0;
	అందుబాటు_వెడల్పు = cursor_నిలువు + 1;
	pum_నిలువు = cursor_నిలువు - నిలువు_స్థానభ్రంశం;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	నిలువు_స్థానభ్రంశం = pum_border && (cursor_నిలువు <= 1) ? (2 - cursor_నిలువు) : 0;
	అందుబాటు_వెడల్పు = Columns - cursor_నిలువు;
	pum_నిలువు = cursor_నిలువు + నిలువు_స్థానభ్రంశం;
    }

    అందుబాటు_వెడల్పు -= trailing_overhead + నిలువు_స్థానభ్రంశం;

    // Case 1: Align pum with "cursor_col"
    ఒకవేళ (సమితి_pum_వెడల్పు_aligned_with_cursor(desired_వెడల్పు, అందుబాటు_వెడల్పు))
	తిరిగి;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ఒకవేళ (అందుబాటు_వెడల్పు > p_pw)
    {
	pum_వెడల్పు = అందుబాటు_వెడల్పు;
	తిరిగి;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    అందుబాటు_వెడల్పు = Columns - trailing_overhead - నిలువు_స్థానభ్రంశం;

    ఒకవేళ (అందుబాటు_వెడల్పు > p_pw)
    {
	pum_వెడల్పు = p_pw;  // Truncate beyond 'pum_width'
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    pum_నిలువు = pum_వెడల్పు + trailing_overhead;
	లేకపోతే
#అయితే_ముగింపు
	    pum_నిలువు = Columns - pum_వెడల్పు - trailing_overhead;
	తిరిగి;
    }

    // Case 3: Not enough room anywhere, use what we have
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
	pum_నిలువు = Columns - 1 - నిలువు_స్థానభ్రంశం;
    లేకపోతే
#అయితే_ముగింపు
	pum_నిలువు = నిలువు_స్థానభ్రంశం;
    pum_వెడల్పు = Columns - trailing_overhead;
}

/*
 * Show the popup menu with items "array[size]".
 * "array" must remain valid until pum_undisplay() is called!
 * When possible the leftmost character is aligned with cursor column.
 * The menu appears above the screen line "row" or at "row" + "height" - 1.
 */
    శూన్య
pum_display(
	pumitem_T   *array,
	పూర్ణాంకం	    కొలత,
	పూర్ణాంకం	    selected,   // index of initially selected item, -1 if
				// out of range
	పూర్ణాంకం	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    పూర్ణాంకం	    cursor_నిలువు;
    పూర్ణాంకం	    above_అడ్డువరుస;
    పూర్ణాంకం	    క్రింద_అడ్డువరుస;
    పూర్ణాంకం	    redo_లెక్క = 0;
#ఒకవేళ defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RIGHTLEFT
    pum_rl = (State & బహుళకం_CMDLINE) == 0 && curwin->w_p_rl;
#అయితే_ముగింపు

    చేయి
    {
	above_అడ్డువరుస = 0;
	క్రింద_అడ్డువరుస = cmdline_అడ్డువరుస;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_నిలువు();
	pum_array = ఖాళీ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_కిటికీ = curwin;
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_అడ్డువరుస = cmdline_అడ్డువరుస;
	లేకపోతే
	    pum_win_అడ్డువరుస = curwin->w_wrow + W_WINROW(curwin);
	pum_win_ఎత్తు = curwin->w_ఎత్తు;
	pum_win_నిలువు = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_వెడల్పు = curwin->w_వెడల్పు;

#ఒకవేళ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ఒకవేళ (pvwin->w_p_pvw)
		ఆపు;
	ఒకవేళ (pvwin != ఖాళీ)
	{
	    ఒకవేళ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_అడ్డువరుస = W_WINROW(pvwin) + pvwin->w_ఎత్తు;
	    లేకపోతే ఒకవేళ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_ఎత్తు)
		క్రింద_అడ్డువరుస = W_WINROW(pvwin);
	}
#అయితే_ముగింపు

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(కొలత, above_అడ్డువరుస, క్రింద_అడ్డువరుస);

	// don't display when we only have room for one line
	ఒకవేళ (pum_ఎత్తు < 1 || (pum_ఎత్తు == 1 && కొలత > 1))
	    తిరిగి;

	pum_array = array;
	pum_కొలత = కొలత;
	pum_compute_కొలత();

	// Calculate cursor column
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    // cmdline completion popup menu
	    cursor_నిలువు = cmdline_compl_startcol();
	లేకపోతే
	{
	    పూర్ణాంకం wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ఒకవేళ (curwin->w_వెడల్పు > 0)
		wcol %= curwin->w_వెడల్పు;
	    లేకపోతే
		wcol = 0;
#నిర్వచితమైతే FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ఒకవేళ (curwin->w_p_cole > 0 && conceal_cursor_పంక్తి(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ఒకవేళ (wcol < 0)
		    wcol = 0;
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (pum_rl)
		cursor_నిలువు = curwin->w_wincol + curwin->w_వెడల్పు - wcol - 1;
	    లేకపోతే
#అయితే_ముగింపు
		cursor_నిలువు = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_ఎత్తు < కొలత) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_నిలువు);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } అయితే (pum_సమితి_selected(selected, redo_లెక్క) && ++redo_లెక్క <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    శూన్య
pum_call_update_screen(శూన్య)
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
    పూర్ణాంకం
pum_under_menu(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం only_redrawing)
{
    పూర్ణాంకం	extra_ఎడమ = pum_border + (pum_margin && pum_border ? 1 : 0);
    పూర్ణాంకం	extra_కుడి = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    పూర్ణాంకం	extra_above = pum_border;
    పూర్ణాంకం	extra_క్రింద = pum_border + (pum_shadow ? 1 : 0);
    పూర్ణాంకం	శిఖరం = pum_అడ్డువరుస - extra_above;
    పూర్ణాంకం	bot = pum_అడ్డువరుస + pum_ఎత్తు + extra_క్రింద;
    పూర్ణాంకం	ఎడమ_2 = pum_నిలువు - 1 - extra_ఎడమ;
    పూర్ణాంకం	కుడి_2 = pum_నిలువు + pum_వెడల్పు + pum_scrollbar + extra_కుడి;

    ఒకవేళ (!((!only_redrawing || pum_will_redraw)
	    && అడ్డువరుస >= శిఖరం && అడ్డువరుస < bot && నిలువు >= ఎడమ_2 && నిలువు < కుడి_2))
	తిరిగి FALSE;

    ఒకవేళ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ఒకవేళ (only_redrawing)
	{
	    ఒకవేళ (నిలువు >= కుడి_2 - 2 || అడ్డువరుస == bot - 1)
		తిరిగి FALSE;
	}
	లేకపోతే
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    పూర్ణాంకం	    కుడి_margin, ఎడమ_margin, ఎడమ_padding;

	    compute_margins(&కుడి_margin, &ఎడమ_margin, &ఎడమ_padding);
	    ఒకవేళ (అడ్డువరుస == శిఖరం && నిలువు >= కుడి_2 - 2)
		తిరిగి FALSE;
	    ఒకవేళ (అడ్డువరుస == bot - 1 && నిలువు < pum_నిలువు + 2 - ఎడమ_padding - pum_border
		    - ఎడమ_margin)
		తిరిగి FALSE;
	}
    }
    తిరిగి TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    స్థిర పూర్ణాంకం *
pum_compute_పాఠ్యం_attrs(char_u *పాఠ్యం, hlf_T hlf, పూర్ణాంకం user_hlattr)
{
    పూర్ణాంకం		i;
    size_t	leader_len;
    పూర్ణాంకం		char_cells;
    పూర్ణాంకం		new_attr;
    char_u	*ptr = పాఠ్యం;
    పూర్ణాంకం		cell_idx = 0;
    garray_T	*ga = ఖాళీ;
    పూర్ణాంకం		*attrs = ఖాళీ;
    char_u	*leader = ఖాళీ;
    పూర్ణాంకం		లోపల_fuzzy;
    పూర్ణాంకం		matched_len = -1;
    int_u	char_pos = 0;
    పూర్ణాంకం		is_select = FALSE;

    ఒకవేళ (*పాఠ్యం == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	తిరిగి ఖాళీ;

    is_select = hlf == HLF_PSI;
    leader = (State & బహుళకం_CMDLINE) ? cmdline_compl_నమూనా()
							  : ins_compl_leader();
    ఒకవేళ (leader == ఖాళీ || *leader == NUL)
	తిరిగి ఖాళీ;

    attrs = ALLOC_MULT(పూర్ణాంకం, vim_strsize(పాఠ్యం));
    ఒకవేళ (attrs == ఖాళీ)
	తిరిగి ఖాళీ;

    లోపల_fuzzy = (State & బహుళకం_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ఒకవేళ (లోపల_fuzzy)
    {
	ga = fuzzy_సరిపోలిక_str_with_pos(పాఠ్యం, leader);
	ఒకవేళ (!ga)
	{
	    vim_free(attrs);
	    తిరిగి ఖాళీ;
	}
    }

    అయితే (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ఒకవేళ (ga != ఖాళీ)
	{
	    // Handle fuzzy matching
	    కొరకు (i = 0; i < ga->ga_len; i++)
	    {
		ఒకవేళ (char_pos == ((int_u *)ga->ga_దత్తాంశం)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ఆపు;
		}
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (పూర్ణాంకం)leader_len;
	    ఒకవేళ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ఒకవేళ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	కొరకు (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ఒకవేళ (ga != ఖాళీ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    తిరిగి attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    స్థిర శూన్య
pum_screen_puts_with_attrs(
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		cells వాడని,
    char_u	*పాఠ్యం,
    పూర్ణాంకం		textlen,
    పూర్ణాంకం		*attrs)
{
    పూర్ణాంకం		నిలువు_ప్రారంభం = నిలువు;
    char_u	*ptr = పాఠ్యం;
    పూర్ణాంకం		char_len;
    పూర్ణాంకం		attr;

    // Render text with proper attributes
    అయితే (*ptr != NUL && ptr < పాఠ్యం + textlen)
    {
	char_len = mb_ptr2len(ptr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    attr = attrs[నిలువు_ప్రారంభం + cells - నిలువు - 1];
	లేకపోతే
#అయితే_ముగింపు
	    attr = attrs[నిలువు - నిలువు_ప్రారంభం];
	screen_puts_len(ptr, char_len, అడ్డువరుస, నిలువు, attr);
	నిలువు += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    స్థిర అంతఃవరుస char_u *
pum_get_అంశం(పూర్ణాంకం సూచీ, పూర్ణాంకం type)
{
    ఎంపిక(type)
    {
	స్థితి CPT_ABBR: తిరిగి pum_array[సూచీ].pum_పాఠ్యం;
	స్థితి CPT_రకం: తిరిగి pum_array[సూచీ].pum_రకం;
	స్థితి CPT_MENU: తిరిగి pum_array[సూచీ].pum_extra;
    }
    తిరిగి ఖాళీ;
}

    స్థిర అంతఃవరుస పూర్ణాంకం
pum_user_attr_combine(పూర్ణాంకం idx, పూర్ణాంకం type, పూర్ణాంకం attr)
{
    పూర్ణాంకం user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_రకం_hlattr,
    };

    తిరిగి user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#నిర్వచితమైతే FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    స్థిర పూర్ణాంకం
pum_display_rtl_పాఠ్యం(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం     నిలువు,
	char_u  *st,
	పూర్ణాంకం     attr,
	పూర్ణాంకం     *attrs,
	పూర్ణాంకం     వెడల్పు,
	పూర్ణాంకం     వెడల్పు_హద్దు,
	పూర్ణాంకం     totwidth,
	పూర్ణాంకం     తదుపరి_isempty,
	పూర్ణాంకం	selected)
{
    char_u  *rt = ఖాళీ;
    పూర్ణాంకం     cells = 0;
    పూర్ణాంకం     over_cell = 0;
    పూర్ణాంకం     truncated = FALSE;
    పూర్ణాంకం     pad = తదుపరి_isempty ? 0 : 2;
    పూర్ణాంకం     మిగిలిన = 0;
    పూర్ణాంకం	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    పూర్ణాంకం	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ఒకవేళ (st == ఖాళీ)
	తిరిగి నిలువు;

    rt = తిరగేయి_పాఠ్యం(st);
    ఒకవేళ (rt == ఖాళీ)
    {
	VIM_CLEAR(st);
	తిరిగి నిలువు;
    }

    char_u *rt_ప్రారంభం = rt;
    cells = mb_string2cells(rt, -1);
    truncated = వెడల్పు_హద్దు - totwidth < cells + pad;

    // only draw the text that fits
    ఒకవేళ (cells > వెడల్పు_హద్దు)
    {
	చేయి
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} అయితే (cells > వెడల్పు_హద్దు);

	ఒకవేళ (cells < వెడల్పు_హద్దు)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ఒకవేళ (truncated)
    {
	char_u  *orig_rt = rt;
	పూర్ణాంకం     కొలత = 0;

	మిగిలిన = వెడల్పు_హద్దు - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ఒకవేళ (cells > మిగిలిన)
	{
	    అయితే (cells > మిగిలిన)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	కొలత = (పూర్ణాంకం)STRLEN(orig_rt);
	ఒకవేళ (cells < మిగిలిన)
	    over_cell = మిగిలిన - cells;

	cells = mb_string2cells(orig_rt, కొలత);
	వెడల్పు = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, అడ్డువరుస, నిలువు - వెడల్పు + 1, trunc_attr);

	ఒకవేళ (over_cell > 0)
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు - వెడల్పు + 2,
		    నిలువు - వెడల్పు + 2 + over_cell, ' ', ' ', attr);
    }

    ఒకవేళ (attrs == ఖాళీ)
	screen_puts_len(rt, (పూర్ణాంకం)STRLEN(rt), అడ్డువరుస, నిలువు - cells + 1, attr);
    లేకపోతే
	pum_screen_puts_with_attrs(అడ్డువరుస, నిలువు - cells + 1, cells, rt,
		(పూర్ణాంకం)STRLEN(rt), attrs);

    vim_free(rt_ప్రారంభం);
    VIM_CLEAR(st);
    తిరిగి నిలువు - వెడల్పు;
}
#అయితే_ముగింపు

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    స్థిర పూర్ణాంకం
pum_display_ltr_పాఠ్యం(
	పూర్ణాంకం     అడ్డువరుస,
	పూర్ణాంకం     నిలువు,
	char_u  *st,
	పూర్ణాంకం     attr,
	పూర్ణాంకం     *attrs,
	పూర్ణాంకం     వెడల్పు,        // width already calculated in outer loop
	పూర్ణాంకం     వెడల్పు_హద్దు,
	పూర్ణాంకం     totwidth,
	పూర్ణాంకం     తదుపరి_isempty,
	పూర్ణాంకం	selected)
{
    పూర్ణాంకం     కొలత = 0;
    పూర్ణాంకం     cells = 0;
    char_u  *st_చివర = ఖాళీ;
    పూర్ణాంకం     over_cell = 0;
    పూర్ణాంకం     pad = తదుపరి_isempty ? 0 : 2;
    పూర్ణాంకం     truncated = FALSE;
    పూర్ణాంకం     మిగిలిన = 0;
    పూర్ణాంకం	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    పూర్ణాంకం	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ఒకవేళ (st == ఖాళీ)
	తిరిగి నిలువు;

    కొలత = (పూర్ణాంకం)STRLEN(st);
    cells = (*mb_string2cells)(st, కొలత);
    truncated = వెడల్పు_హద్దు - totwidth < cells + pad;

    // only draw the text that fits
    అయితే (కొలత > 0 && నిలువు + cells > వెడల్పు_హద్దు + pum_నిలువు)
    {
	--కొలత;
	ఒకవేళ (has_mbyte)
	{
	    కొలత -= (*mb_తల_off)(st, st + కొలత);
	    cells -= (*mb_ptr2cells)(st + కొలత);
	}
	లేకపోతే
	    --cells;
    }

    // truncated
    ఒకవేళ (truncated)
    {
	మిగిలిన = వెడల్పు_హద్దు - totwidth - 1;
	ఒకవేళ (cells > మిగిలిన)
	{
	    st_చివర = st + కొలత;
	    అయితే (st_చివర > st && cells > మిగిలిన)
	    {
		MB_PTR_వెనుక(st, st_చివర);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_చివర) : 1;
	    }
	    కొలత = st_చివర - st;
	}

	ఒకవేళ (cells < మిగిలిన)
	    over_cell = మిగిలిన - cells;
	cells = mb_string2cells(st, కొలత);
	వెడల్పు = cells + over_cell + 1;
    }

    ఒకవేళ (attrs == ఖాళీ)
	screen_puts_len(st, కొలత, అడ్డువరుస, నిలువు, attr);
    లేకపోతే
	pum_screen_puts_with_attrs(అడ్డువరుస, నిలువు, cells, st, కొలత, attrs);

    ఒకవేళ (truncated)
    {
	ఒకవేళ (over_cell > 0)
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు + cells,
		    నిలువు + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, అడ్డువరుస, నిలువు + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    తిరిగి నిలువు + వెడల్పు;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    స్థిర పూర్ణాంకం
pum_process_అంశం(
	పూర్ణాంకం     అడ్డువరుస,
	పూర్ణాంకం     నిలువు,
	పూర్ణాంకం     idx,
	పూర్ణాంకం     j,         // Current position in order array
	పూర్ణాంకం     *క్రమం,    // Order array
	hlf_T   hlf,
	పూర్ణాంకం     attr,
	పూర్ణాంకం     *totwidth_ptr,
	పూర్ణాంకం     తదుపరి_isempty)
{
    పూర్ణాంకం     అంశం_type = క్రమం[j];
    char_u  *s = ఖాళీ;
    char_u  *p = pum_get_అంశం(idx, అంశం_type);
    పూర్ణాంకం     వెడల్పు = 0;  // item width
    పూర్ణాంకం     w;		// char width
    పూర్ణాంకం	    selected = idx == pum_selected;

    కొరకు ( ; ; MB_PTR_ADV(p))
    {
	ఒకవేళ (s == ఖాళీ)
	    s = p;
	w = ptr2cells(p);
	ఒకవేళ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_వెడల్పు)
	{
	    వెడల్పు += w;
	    కొనసాగు;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	పూర్ణాంకం     *attrs = ఖాళీ;
	పూర్ణాంకం     saved = *p;

	ఒకవేళ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ఒకవేళ (saved != NUL)
	    *p = saved;

	ఒకవేళ (అంశం_type == CPT_ABBR)
	    attrs = pum_compute_పాఠ్యం_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    నిలువు = pum_display_rtl_పాఠ్యం(అడ్డువరుస, నిలువు, st, attr, attrs,
		    వెడల్పు, pum_వెడల్పు, *totwidth_ptr, తదుపరి_isempty, selected);
	లేకపోతే
#అయితే_ముగింపు
	    నిలువు = pum_display_ltr_పాఠ్యం(అడ్డువరుస, నిలువు, st, attr, attrs,
		    వెడల్పు, pum_వెడల్పు, *totwidth_ptr, తదుపరి_isempty, selected);

	ఒకవేళ (attrs != ఖాళీ)
	    VIM_CLEAR(attrs);

	ఒకవేళ (*p != TAB)
	    ఆపు;

	// Display two spaces for a Tab.
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, అడ్డువరుస, నిలువు - 1, attr);
	    నిలువు -= 2;
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    screen_puts_len((char_u *)"  ", 2, అడ్డువరుస, నిలువు, attr);
	    నిలువు += 2;
	}
	*totwidth_ptr += 2;
	s = ఖాళీ;  // start text at next char
	వెడల్పు = 0;
    }

    తిరిగి నిలువు;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    స్థిర శూన్య
pum_draw_scrollbar(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	i,
	పూర్ణాంకం	thumb_pos,
	పూర్ణాంకం	thumb_ఎత్తు)
{
    ఒకవేళ (pum_scrollbar <= 0)
	తిరిగి;

    పూర్ణాంకం attr = (i >= thumb_pos && i < thumb_pos + thumb_ఎత్తు) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
	screen_putchar(' ', అడ్డువరుస, pum_నిలువు - pum_వెడల్పు, attr);
    లేకపోతే
#అయితే_ముగింపు
	screen_putchar(' ', అడ్డువరుస, pum_నిలువు + pum_వెడల్పు, attr);
}

    స్థిర అంతఃవరుస శూన్య
pum_align_క్రమం(పూర్ణాంకం *క్రమం)
{
    పూర్ణాంకం is_default = cia_flags == 0;
    క్రమం[0] = is_default ? CPT_ABBR : cia_flags / 100;
    క్రమం[1] = is_default ? CPT_రకం : (cia_flags / 10) % 10;
    క్రమం[2] = is_default ? CPT_MENU : cia_flags % 10;
}

స్థిర శూన్య pum_free_bg(శూన్య);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    శూన్య
pum_opacity_మారింది(శూన్య)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    ఒకవేళ (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    స్థిర శూన్య
pum_free_bg(శూన్య)
{
    పూర్ణాంకం k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_పంక్తులు);
    VIM_CLEAR(pum_bg_పంక్తులు_UC);
    కొరకు (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_పంక్తులు_C[k]);
    pum_bg_శిఖరం = 0;
    pum_bg_bot = 0;
    pum_bg_నిలువుల_సంఖ్య = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    శూన్య
pum_redraw(శూన్య)
{
    పూర్ణాంకం		అడ్డువరుస = pum_అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		idx;
    char_u	*p = ఖాళీ;
    పూర్ణాంకం		totwidth;
    పూర్ణాంకం		thumb_pos = 0;
    పూర్ణాంకం		thumb_ఎత్తు = 1;
    పూర్ణాంకం		అంశం_type;
    పూర్ణాంకం		క్రమం[3];
    పూర్ణాంకం		తదుపరి_isempty = FALSE;
    పూర్ణాంకం		n;
    పూర్ణాంకం		వస్తువులు_వెడల్పు_array[3] = { pum_పునాది_వెడల్పు, pum_రకం_వెడల్పు,
							    pum_extra_వెడల్పు };
    పూర్ణాంకం		basic_వెడల్పు;  // first item width
    పూర్ణాంకం		చివరి_isabbr = FALSE;
    పూర్ణాంకం		orig_attr = -1;
    పూర్ణాంకం		scroll_range = pum_కొలత - pum_ఎత్తు;
    bool	override_success;
    పూర్ణాంకం		opacity_చురుకైన = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = నెట్టు_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_ENABLE);

#నిర్వచితమైతే FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ఒకవేళ (pum_imagep == ఖాళీ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ఖాళీ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ఒకవేళ (pum_imagep != ఖాళీ)
    {
	image_placement_సమితి_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_సమితి_position(pum_imagep, pum_అడ్డువరుస, pum_నిలువు);
	image_placement_సమితి_bounding_box(pum_imagep, pum_అడ్డువరుస, pum_నిలువు,
		pum_ఎత్తు, pum_వెడల్పు);
	image_placement_సమితి_enable(pum_imagep, true);
    }
#అయితే_ముగింపు

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

    ఒకవేళ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ఒకవేళ (opacity_చురుకైన && pum_bg_పంక్తులు != ఖాళీ
		&& (pum_bg_నిలువుల_సంఖ్య != screen_Columns
		    || pum_bg_bot > screen_అడ్డువరుసలు))
	    pum_free_bg();

	ఒకవేళ (opacity_చురుకైన && pum_bg_పంక్తులు != ఖాళీ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	లేకపోతే ఒకవేళ (opacity_చురుకైన)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ఒకవేళ (Screen_పంక్తులు != ఖాళీ && ScreenAttrs != ఖాళీ)
	    {
		పూర్ణాంకం	save_శిఖరం, save_bot, save_ncells, k;

		pum_free_bg();
		save_శిఖరం = pum_అడ్డువరుస - pum_border;
		save_bot = pum_అడ్డువరుస + pum_ఎత్తు + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ఒకవేళ (save_శిఖరం < 0)
		    save_శిఖరం = 0;
		ఒకవేళ (save_bot > screen_అడ్డువరుసలు)
		    save_bot = screen_అడ్డువరుసలు;
		pum_bg_శిఖరం = save_శిఖరం;
		pum_bg_bot = save_bot;
		pum_bg_నిలువుల_సంఖ్య = screen_Columns;
		ఒకవేళ (save_శిఖరం < save_bot)
		{
		    save_ncells = (save_bot - save_శిఖరం) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_పంక్తులు = LALLOC_MULT(schar_T, save_ncells);
		    ఒకవేళ (enc_utf8)
		    {
			pum_bg_పంక్తులు_UC = LALLOC_MULT(u8char_T, save_ncells);
			కొరకు (k = 0; k < MAX_MCO; ++k)
			    pum_bg_పంక్తులు_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ఒకవేళ (pum_bg_attrs != ఖాళీ && pum_bg_పంక్తులు != ఖాళీ)
		    {
			కొరకు (పూర్ణాంకం r = save_శిఖరం; r < save_bot; ++r)
			{
			    పూర్ణాంకం soff = (r - save_శిఖరం) * screen_Columns;
			    పూర్ణాంకం loff = పంక్తి_స్థానభ్రంశం[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * పరిమాణం(sattr_T));
			    mch_memmove(pum_bg_పంక్తులు + soff,
				    Screen_పంక్తులు + loff,
				    screen_Columns * పరిమాణం(schar_T));
			    ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ
						&& Screen_పంక్తులు_UC != ఖాళీ)
			    {
				mch_memmove(pum_bg_పంక్తులు_UC + soff,
					Screen_పంక్తులు_UC + loff,
					screen_Columns * పరిమాణం(u8char_T));
				కొరకు (k = 0; k < MAX_MCO; ++k)
				    ఒకవేళ (pum_bg_పంక్తులు_C[k] != ఖాళీ
						    && Screen_పంక్తులు_C[k] != ఖాళీ)
					mch_memmove(pum_bg_పంక్తులు_C[k] + soff,
						Screen_పంక్తులు_C[k] + loff,
						screen_Columns
						    * పరిమాణం(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	లేకపోతే
	{
	    // Do not redraw in pum_may_redraw() and don't draw in the area
	    // where the popup menu will be.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
    }

    // never display more than we have
    pum_మొదటి = MIN(pum_మొదటి, scroll_range);

    ఒకవేళ (pum_scrollbar)
    {
	thumb_ఎత్తు = pum_ఎత్తు * pum_ఎత్తు / pum_కొలత;
	ఒకవేళ (thumb_ఎత్తు == 0)
	    thumb_ఎత్తు = 1;
	thumb_pos = (pum_మొదటి * (pum_ఎత్తు - thumb_ఎత్తు)
			    + scroll_range / 2) / scroll_range;
    }

#నిర్వచితమైతే FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#అయితే_ముగింపు

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    ఒకవేళ (pum_border)
	pum_draw_border();
    ఒకవేళ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ఒకవేళ (opacity_చురుకైన)
	screen_pum_blend = 100 - (పూర్ణాంకం)p_po;

    కొరకు (i = 0; i < pum_ఎత్తు; ++i)
    {
	idx = i + pum_మొదటి;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	{
	    ఒకవేళ (pum_నిలువు < curwin->w_wincol + curwin->w_వెడల్పు - 1
							    - pum_border)
		screen_putchar(' ', అడ్డువరుస, pum_నిలువు + 1, attr);
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (pum_నిలువు > pum_border)
		screen_putchar(' ', అడ్డువరుస, pum_నిలువు - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	నిలువు = pum_నిలువు;
	totwidth = 0;
	pum_align_క్రమం(క్రమం);
	basic_వెడల్పు = వస్తువులు_వెడల్పు_array[క్రమం[0]];
	చివరి_isabbr = క్రమం[2] == CPT_ABBR;
	కొరకు (j = 0; j < 3; ++j)
	{
	    అంశం_type = క్రమం[j];
	    hlf = hlfs[అంశం_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ఒకవేళ (అంశం_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, అంశం_type, attr);
	    p = pum_get_అంశం(idx, అంశం_type);

	    ఒకవేళ (j + 1 < 3)
		తదుపరి_isempty = pum_get_అంశం(idx, క్రమం[j + 1]) == ఖాళీ;
	    లేకపోతే
		తదుపరి_isempty = TRUE;

	    ఒకవేళ (p != ఖాళీ)
		// Process and display the item
		నిలువు = pum_process_అంశం(అడ్డువరుస, నిలువు, idx, j, క్రమం, hlf, attr,
						    &totwidth, తదుపరి_isempty);

	    ఒకవేళ (j > 0)
		n = వస్తువులు_వెడల్పు_array[క్రమం[1]] + (చివరి_isabbr ? 0 : 1);
	    లేకపోతే
		n = క్రమం[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ఒకవేళ (j == 2
		    || (తదుపరి_isempty && (j == 1 || (j == 0
				&& pum_get_అంశం(idx, క్రమం[j + 2]) == ఖాళీ)))
		    || basic_వెడల్పు + n >= pum_వెడల్పు)
		ఆపు;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (pum_rl)
	    {
		screen_fill(అడ్డువరుస, అడ్డువరుస + 1, pum_నిలువు - basic_వెడల్పు - n + 1,
						నిలువు + 1, ' ', ' ', orig_attr);
		నిలువు = pum_నిలువు - basic_వెడల్పు - n;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు, pum_నిలువు + basic_వెడల్పు + n,
							' ', ' ', orig_attr);
		నిలువు = pum_నిలువు + basic_వెడల్పు + n;
	    }
	    totwidth = basic_వెడల్పు + n;
	}

#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, pum_నిలువు - pum_వెడల్పు + 1, నిలువు + 1,
							    ' ', ' ', orig_attr);
	లేకపోతే
#అయితే_ముగింపు
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు, pum_నిలువు + pum_వెడల్పు, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(అడ్డువరుస, i, thumb_pos, thumb_ఎత్తు);

	++అడ్డువరుస;
    }

    screen_pum_blend = 0;

#నిర్వచితమైతే FEAT_PROP_POPUP
    screen_zindex = 0;
#అయితే_ముగింపు

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();

    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_DISABLE);
}

#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    శూన్య
pum_position_info_popup(win_T *wp)
{
    పూర్ణాంకం నిలువు = pum_నిలువు + pum_వెడల్పు + pum_scrollbar + 1;
    పూర్ణాంకం అడ్డువరుస = pum_అడ్డువరుస;
    పూర్ణాంకం botpos = POPPOS_BOTLEFT;
    పూర్ణాంకం	వాడిన_maxwidth_opt = FALSE;
    పూర్ణాంకం	కుడి_margin, ఎడమ_margin, dummy;
    పూర్ణాంకం ఎడమ_extra_వెడల్పు, కుడి_extra_వెడల్పు;

    (శూన్య)compute_margins(&కుడి_margin, &ఎడమ_margin, &dummy);

    ఎడమ_extra_వెడల్పు = pum_border + ఎడమ_margin;
    కుడి_extra_వెడల్పు = pum_border + కుడి_margin + (pum_shadow ? 2 : 0);
    నిలువు += కుడి_extra_వెడల్పు;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ఒకవేళ (Columns - నిలువు < 20 && Columns - నిలువు < pum_నిలువు)
    {
	నిలువు = pum_నిలువు - 1 - ఎడమ_extra_వెడల్పు;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_నిలువు - 1 - ఎడమ_extra_వెడల్పు;
    }
    లేకపోతే
	wp->w_maxwidth = Columns - నిలువు + 1;
    wp->w_maxwidth -= popup_extra_వెడల్పు(wp);
    ఒకవేళ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	వాడిన_maxwidth_opt = TRUE;
    }

    అడ్డువరుస -= popup_శిఖరం_extra(wp);
    ఒకవేళ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ఒకవేళ (pum_అడ్డువరుస < pum_win_అడ్డువరుస)
	{
	    // menu above cursor line, align with bottom
	    అడ్డువరుస += pum_ఎత్తు;
	    wp->w_popup_pos = botpos;
	}
	లేకపోతే
	    // menu below cursor line, align with top
	    అడ్డువరుస += 1;
    }
    లేకపోతే
	// align with the selected item
	అడ్డువరుస += pum_selected - pum_మొదటి + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ఒకవేళ (wp->w_maxwidth < 10 && !వాడిన_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    లేకపోతే
	popup_సమితి_wantpos_rowcol(wp, అడ్డువరుస, నిలువు);
}
#అయితే_ముగింపు

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
    స్థిర పూర్ణాంకం
pum_సమితి_selected(పూర్ణాంకం n, పూర్ణాంకం repeat వాడని)
{
    పూర్ణాంకం	    resized = FALSE;
    పూర్ణాంకం	    context = pum_ఎత్తు / 2;
    పూర్ణాంకం	    scroll_స్థానభ్రంశం;
#నిర్వచితమైతే FEAT_QUICKFIX
    పూర్ణాంకం	    prev_selected = pum_selected;
    గుర్తులేని	cur_cot_flags = get_cot_flags();
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    పూర్ణాంకం	    has_info = FALSE;
#అయితే_ముగింపు

    pum_selected = n;
    scroll_స్థానభ్రంశం = pum_selected - pum_ఎత్తు;

    ఒకవేళ (pum_selected >= 0 && pum_selected < pum_కొలత)
    {
	ఒకవేళ (pum_మొదటి > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ఒకవేళ (pum_మొదటి > pum_selected - 2)
	    {
		pum_మొదటి -= pum_ఎత్తు - 2;
		ఒకవేళ (pum_మొదటి < 0)
		    pum_మొదటి = 0;
		లేకపోతే ఒకవేళ (pum_మొదటి > pum_selected)
		    pum_మొదటి = pum_selected;
	    }
	    లేకపోతే
		pum_మొదటి = pum_selected;
	}
	లేకపోతే ఒకవేళ (pum_మొదటి < scroll_స్థానభ్రంశం + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ఒకవేళ (pum_మొదటి < scroll_స్థానభ్రంశం + 3)
		pum_మొదటి = MAX(pum_మొదటి + pum_ఎత్తు - 2, scroll_స్థానభ్రంశం + 1);
	    లేకపోతే
		pum_మొదటి = scroll_స్థానభ్రంశం + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ఒకవేళ (pum_ఎత్తు > 2)
	{
	    ఒకవేళ (pum_మొదటి > pum_selected - context)
		pum_మొదటి = MAX(pum_selected - context, 0);  // scroll down
	    లేకపోతే ఒకవేళ (pum_మొదటి < pum_selected + context - pum_ఎత్తు + 1)
		pum_మొదటి = pum_selected + context - pum_ఎత్తు + 1;  //పైకి
	}
	// adjust for the number of lines displayed
	pum_మొదటి = MIN(pum_మొదటి, pum_కొలత - pum_ఎత్తు);

#ఒకవేళ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ఒకవేళ (pum_array[pum_selected].pum_info != ఖాళీ
		&& అడ్డువరుసలు > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# లేకపోతే
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ఒకవేళ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    లేకపోతే ఒకవేళ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_సాధారణ;
	    లేకపోతే
		use_popup = USEPOPUP_NONE;
	    ఒకవేళ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		ఖండం_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ఒకవేళ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ఒకవేళ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ఒకవేళ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		పూర్ణాంకం	res = OK;

		ఒకవేళ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ఖాళీ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    అయితే (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		లేకపోతే
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_ONE, 0, ఖాళీ);
		    --no_u_sync;
		    ఒకవేళ (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			సమితి_option_విలువ_give_err((char_u *)"swf",
							  0L, ఖాళీ, OPT_LOCAL);
			సమితి_option_విలువ_give_err((char_u *)"bl",
							  0L, ఖాళీ, OPT_LOCAL);
			సమితి_option_విలువ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			సమితి_option_విలువ_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			సమితి_option_విలువ_give_err((char_u *)"diff",
							  0L, ఖాళీ, OPT_LOCAL);
		    }
		}
		ఒకవేళ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    కొరకు (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ఒకవేళ (e == ఖాళీ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ఆపు;
			}
			*e = NUL;
			ml_append(lnum++, p, (పూర్ణాంకం)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_పంక్తి_లెక్క);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ఒకవేళ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ఒకవేళ (curwin->w_ఎత్తు < lnum)
			{
			    win_setheight((పూర్ణాంకం)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_మారింది = 0;
		    curbuf->b_p_ma = FALSE;
		    ఒకవేళ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    లేకపోతే ఒకవేళ (curwin->w_topline > curbuf->b_ml.ml_పంక్తి_లెక్క)
			curwin->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.నిలువు = 0;
# ifdef FEAT_PROP_POPUP
		    ఒకవేళ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ఒకవేళ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ఒకవేళ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			పూర్ణాంకం save_redr_స్థితి;

			ఒకవేళ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ఒకవేళ (ins_compl_చురుకైన() && !resized)
			    curwin->w_redr_స్థితి = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ఒకవేళ (resized && win_valid(curwin_save))
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
			save_redr_స్థితి = curwin_save->w_redr_స్థితి;
			curwin_save->w_redr_స్థితి = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_స్థితి = save_redr_స్థితి;

			ఒకవేళ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ఒకవేళ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ఒకవేళ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ఒకవేళ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#అయితే_ముగింపు
    }
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ఒకవేళ (!has_info)
	// hide any popup info window
	popup_hide_info();
#అయితే_ముగింపు

    తిరిగి resized;
}

/*
 * Undisplay the popup menu (later).
 */
    శూన్య
pum_undisplay(శూన్య)
{
#నిర్వచితమైతే FEAT_IMAGE_POPUP
    ఒకవేళ (pum_imagep != ఖాళీ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ఖాళీ;
    }
#అయితే_ముగింపు
    pum_free_bg();
    pum_array = ఖాళీ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (pum_లోపల_cmdline)
    {
	clear_cmdline = TRUE;
	pum_లోపల_cmdline = FALSE;
    }
    స్థితి_redraw_all();
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#అయితే_ముగింపు
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    శూన్య
pum_clear(శూన్య)
{
    pum_మొదటి = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    పూర్ణాంకం
pum_visible(శూన్య)
{
    తిరిగి !pum_pretend_not_visible && pum_array != ఖాళీ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    స్థిర పూర్ణాంకం
pum_లోపల_same_position(శూన్య)
{
    పూర్ణాంకం	    అడ్డువరుస = (State & బహుళకం_CMDLINE)
			? cmdline_అడ్డువరుస
			: curwin->w_wrow + W_WINROW(curwin);

    తిరిగి pum_కిటికీ != curwin
	    || (pum_win_అడ్డువరుస == అడ్డువరుస
		&& pum_win_ఎత్తు == curwin->w_ఎత్తు
		&& pum_win_నిలువు == curwin->w_wincol
		&& pum_win_వెడల్పు == curwin->w_వెడల్పు);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    పూర్ణాంకం
pum_redraw_లోపల_same_position(శూన్య)
{
    ఒకవేళ (!pum_visible() || pum_will_redraw)
	తిరిగి FALSE;  // nothing to do

    తిరిగి pum_లోపల_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    శూన్య
pum_may_redraw(శూన్య)
{
    pumitem_T	*array = pum_array;
    పూర్ణాంకం		len = pum_కొలత;
    పూర్ణాంకం		selected = pum_selected;

    ఒకవేళ (!pum_visible() || pum_will_redraw)
	తిరిగి;  // nothing to do

    ఒకవేళ (pum_లోపల_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    లేకపోతే
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
    పూర్ణాంకం
pum_get_ఎత్తు(శూన్య)
{
    తిరిగి pum_ఎత్తు;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    శూన్య
pum_సమితి_event_info(dict_T *dict)
{
    ఒకవేళ (!pum_visible())
	తిరిగి;
    (శూన్య)dict_కూడు_సంఖ్య(dict, "height", pum_ఎత్తు);
    (శూన్య)dict_కూడు_సంఖ్య(dict, "width", pum_వెడల్పు);
    (శూన్య)dict_కూడు_సంఖ్య(dict, "row", pum_అడ్డువరుస);
    (శూన్య)dict_కూడు_సంఖ్య(dict, "col", pum_నిలువు);
    (శూన్య)dict_కూడు_సంఖ్య(dict, "size", pum_కొలత);
    (శూన్య)dict_కూడు_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL_పదం) || defined(FEAT_పదం_POPUP_MENU)
    స్థిర శూన్య
pum_position_at_mouse(పూర్ణాంకం min_వెడల్పు)
{
    పూర్ణాంకం extra_ఎత్తు = pum_border * 2 + pum_shadow;
    పూర్ణాంకం extra_స్థానభ్రంశం = pum_border + pum_shadow;
    పూర్ణాంకం trailing_overhead = 0;
    పూర్ణాంకం నిలువు_స్థానభ్రంశం = 0;
    పూర్ణాంకం అందుబాటు_వెడల్పు;

    ఒకవేళ (అడ్డువరుసలు - mouse_అడ్డువరుస - extra_ఎత్తు > pum_కొలత
	    || అడ్డువరుసలు - mouse_అడ్డువరుస > mouse_అడ్డువరుస)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_అడ్డువరుస = mouse_అడ్డువరుస + 1 + pum_border;
	ఒకవేళ (pum_ఎత్తు > అడ్డువరుసలు - pum_అడ్డువరుస - extra_ఎత్తు)
	    pum_ఎత్తు = అడ్డువరుసలు - pum_అడ్డువరుస - extra_ఎత్తు;
	ఒకవేళ (pum_అడ్డువరుస + pum_ఎత్తు + extra_స్థానభ్రంశం >= cmdline_అడ్డువరుస)
	    pum_లోపల_cmdline = TRUE;
    }
    లేకపోతే
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_అడ్డువరుస = mouse_అడ్డువరుస - pum_కొలత - extra_స్థానభ్రంశం;
	ఒకవేళ (pum_అడ్డువరుస < pum_border)
	{
	    pum_ఎత్తు += pum_అడ్డువరుస - pum_border;
	    pum_అడ్డువరుస = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	నిలువు_స్థానభ్రంశం = (mouse_నిలువు == Columns - 1) && pum_border;
	అందుబాటు_వెడల్పు = mouse_నిలువు + 1 - trailing_overhead - నిలువు_స్థానభ్రంశం;
	ఒకవేళ (అందుబాటు_వెడల్పు >= pum_పునాది_వెడల్పు || అందుబాటు_వెడల్పు > min_వెడల్పు)
	    // Enough space to show at mouse column.
	    pum_నిలువు = mouse_నిలువు - నిలువు_స్థానభ్రంశం;
	లేకపోతే
	    // Not enough space, left align with window.
	    pum_నిలువు = MIN(pum_పునాది_వెడల్పు, min_వెడల్పు) - 1 + trailing_overhead;
	pum_వెడల్పు = pum_నిలువు + 1 - trailing_overhead;
    }
    లేకపోతే
# endif
    {
	నిలువు_స్థానభ్రంశం = (mouse_నిలువు == 0) && pum_border;
	అందుబాటు_వెడల్పు = Columns - mouse_నిలువు - trailing_overhead - నిలువు_స్థానభ్రంశం;
	ఒకవేళ (అందుబాటు_వెడల్పు >= pum_పునాది_వెడల్పు || అందుబాటు_వెడల్పు > min_వెడల్పు)
	    // Enough space to show at mouse column.
	    pum_నిలువు = mouse_నిలువు + నిలువు_స్థానభ్రంశం;
	లేకపోతే
	    // Not enough space, right align with window.
	    pum_నిలువు = Columns - MIN(pum_పునాది_వెడల్పు, min_వెడల్పు)
		- trailing_overhead;
	pum_వెడల్పు = Columns - pum_నిలువు - trailing_overhead;
    }

    pum_వెడల్పు = MIN(pum_వెడల్పు, pum_పునాది_వెడల్పు + 1);
    // Do not redraw at cursor position.
    pum_కిటికీ = ఖాళీ;
}

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL_పదం)
స్థిర pumitem_T *balloon_array = ఖాళీ;
స్థిర పూర్ణాంకం balloon_arraysize;

# define BALLOON_MIN_వెడల్పు 50
# define BALLOON_MIN_ఎత్తు 10

రకనిర్వచనం నిర్మాణం {
    char_u	*ప్రారంభం;
    పూర్ణాంకం		bytelen;
    పూర్ణాంకం		cells;
    పూర్ణాంకం		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    పూర్ణాంకం
విభజించు_సందేశం(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*అంశం;
    పూర్ణాంకం		quoted = FALSE;
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		పంక్తి;
    పూర్ణాంకం		అంశం_idx;
    పూర్ణాంకం		indent = 0;
    పూర్ణాంకం		max_cells = 0;
    పూర్ణాంకం		max_ఎత్తు_2 = అడ్డువరుసలు / 2 - 1;
    పూర్ణాంకం		long_అంశం_లెక్క = 0;
    పూర్ణాంకం		విభజించు_long_వస్తువులు = FALSE;

    ga_init2(&ga, పరిమాణం(balpart_T), 20);
    p = mesg;

    అయితే (*p != NUL)
    {
	ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    వెళ్ళు failed;
	అంశం = ((balpart_T *)ga.ga_దత్తాంశం) + ga.ga_len;
	అంశం->ప్రారంభం = p;
	అంశం->indent = indent;
	అంశం->cells = indent * 2;
	++ga.ga_len;
	అయితే (*p != NUL)
	{
	    ఒకవేళ (*p == '"')
		quoted = !quoted;
	    లేకపోతే ఒకవేళ (*p == '\n')
		ఆపు;
	    లేకపోతే ఒకవేళ (*p == '\\' && p[1] != NUL)
		++p;
	    లేకపోతే ఒకవేళ (!quoted)
	    {
		ఒకవేళ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ఒకవేళ (*p == '{')
			++indent;
		    లేకపోతే ఒకవేళ (*p == '}' && indent > 0)
			--indent;
		    ++అంశం->cells;
		    p = skipwhite(p + 1);
		    ఆపు;
		}
	    }
	    అంశం->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	అంశం->bytelen = p - అంశం->ప్రారంభం;
	ఒకవేళ (*p == '\n')
	    ++p;
	ఒకవేళ (అంశం->cells > max_cells)
	    max_cells = అంశం->cells;
	long_అంశం_లెక్క += (అంశం->cells - 1) / BALLOON_MIN_వెడల్పు;
    }

    ఎత్తు = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ఒకవేళ (long_అంశం_లెక్క > 0 && ఎత్తు + long_అంశం_లెక్క <= max_ఎత్తు_2)
    {
	విభజించు_long_వస్తువులు = TRUE;
	ఎత్తు += long_అంశం_లెక్క;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ఒకవేళ (ఎత్తు > max_ఎత్తు_2)
	ఎత్తు = max_ఎత్తు_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, ఎత్తు);
    ఒకవేళ (*array == ఖాళీ)
	వెళ్ళు failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_పాఠ్యం = vim_strsave((char_u *)"");
    (*array + ఎత్తు - 1)->pum_పాఠ్యం = vim_strsave((char_u *)"");

    కొరకు (పంక్తి = 1, అంశం_idx = 0; పంక్తి < ఎత్తు - 1; ++అంశం_idx)
    {
	పూర్ణాంకం	skip;
	పూర్ణాంకం	thislen;
	పూర్ణాంకం	copylen;
	పూర్ణాంకం	ind;
	పూర్ణాంకం	cells;

	అంశం = ((balpart_T *)ga.ga_దత్తాంశం) + అంశం_idx;
	ఒకవేళ (అంశం->bytelen == 0)
	    (*array)[పంక్తి++].pum_పాఠ్యం = vim_strsave((char_u *)"");
	లేకపోతే
	    కొరకు (skip = 0; skip < అంశం->bytelen; skip += thislen)
	    {
		ఒకవేళ (విభజించు_long_వస్తువులు && అంశం->cells >= BALLOON_MIN_వెడల్పు)
		{
		    cells = అంశం->indent * 2;
		    కొరకు (p = అంశం->ప్రారంభం + skip;
			    p < అంశం->ప్రారంభం + అంశం->bytelen;
							    p += mb_ptr2len(p))
			ఒకవేళ ((cells += ptr2cells(p)) > BALLOON_MIN_వెడల్పు)
			    ఆపు;
		    thislen = p - (అంశం->ప్రారంభం + skip);
		}
		లేకపోతే
		    thislen = అంశం->bytelen;

		// put indent at the start
		p = alloc(thislen + అంశం->indent * 2 + 1);
		ఒకవేళ (p == ఖాళీ)
		{
		    కొరకు (పంక్తి = 0; పంక్తి <= ఎత్తు - 1; ++పంక్తి)
			vim_free((*array)[పంక్తి].pum_పాఠ్యం);
		    vim_free(*array);
		    వెళ్ళు failed;
		}
		కొరకు (ind = 0; ind < అంశం->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		కొరకు (copylen = thislen; copylen > 0; --copylen)
		    ఒకవేళ (అంశం->ప్రారంభం[skip + copylen - 1] != ' ')
			ఆపు;

		vim_strncpy(p + ind, అంశం->ప్రారంభం + skip, copylen);
		(*array)[పంక్తి].pum_పాఠ్యం = p;
		అంశం->indent = 0;  // wrapped line has no indent
		++పంక్తి;
	    }
    }
    ga_clear(&ga);
    తిరిగి ఎత్తు;

failed:
    ga_clear(&ga);
    తిరిగి 0;
}

    శూన్య
ui_remove_balloon(శూన్య)
{
    ఒకవేళ (balloon_array == ఖాళీ)
	తిరిగి;

    pum_undisplay();
    అయితే (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_పాఠ్యం);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    శూన్య
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    ఒకవేళ (mesg == ఖాళీ && list == ఖాళీ)
    {
	pum_undisplay();
	తిరిగి;
    }
    ఒకవేళ (list != ఖాళీ)
    {
	listitem_T  *li;
	పూర్ణాంకం	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ఒకవేళ (balloon_array == ఖాళీ)
	    తిరిగి;
	CHECK_LIST_MATERIALIZE(list);
	కొరకు (idx = 0, li = list->lv_మొదటి; li != ఖాళీ; li = li->li_తదుపరి, ++idx)
	{
	    char_u *పాఠ్యం = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_పాఠ్యం = vim_strsave(
					   పాఠ్యం == ఖాళీ ? (char_u *)"" : పాఠ్యం);
	}
    }
    లేకపోతే
	balloon_arraysize = విభజించు_సందేశం(mesg, &balloon_array);

    ఒకవేళ (balloon_arraysize <= 0)
	తిరిగి;

    pum_array = balloon_array;
    pum_కొలత = balloon_arraysize;
    pum_compute_కొలత();
    pum_scrollbar = 0;
    pum_ఎత్తు = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_వెడల్పు);
    pum_selected = -1;
    pum_మొదటి = 0;
    pum_redraw();
}

/*
 * Called when the mouse moved, may remove any displayed balloon.
 */
    శూన్య
ui_may_remove_balloon(శూన్య)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పదం_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    స్థిర శూన్య
pum_select_mouse_pos(శూన్య)
{
    పూర్ణాంకం idx = mouse_అడ్డువరుస - pum_అడ్డువరుస;

    ఒకవేళ (idx < 0 || idx >= pum_ఎత్తు)
	pum_selected = -1;
    లేకపోతే ఒకవేళ (*pum_array[idx].pum_పాఠ్యం != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    స్థిర శూన్య
pum_execute_menu(vimmenu_T *menu, పూర్ణాంకం బహుళకం)
{
    vimmenu_T   *mp;
    పూర్ణాంకం		idx = 0;
    exarg_T	ea;

    FOR_ALL_బిడ్డ_MENUS(menu, mp)
	ఒకవేళ ((mp->modes & mp->enabled & బహుళకం) && idx++ == pum_selected)
	{
	    CLEAR_క్షేత్రం(ea);
	    execute_menu(&ea, mp, -1);
	    ఆపు;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    శూన్య
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    పూర్ణాంకం		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_పదం
    పూర్ణాంకం		save_bevalterm = p_bevalterm;
# endif
    పూర్ణాంకం		బహుళకం;

    pum_undisplay();
    pum_కొలత = 0;
    బహుళకం = get_menu_బహుళకం_flag();

    FOR_ALL_బిడ్డ_MENUS(menu, mp)
	ఒకవేళ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & బహుళకం))
	    ++pum_కొలత;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ఒకవేళ (pum_కొలత <= 0)
    {
	emsg(_(e_menu_only_exists_లోపల_another_బహుళకం));
	తిరిగి;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_కొలత);
    ఒకవేళ (array == ఖాళీ)
	తిరిగి;

    FOR_ALL_బిడ్డ_MENUS(menu, mp)
    {
	char_u *s = ఖాళీ;

	// Make a copy of the text, the menu may be redefined in a callback.
	ఒకవేళ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	లేకపోతే ఒకవేళ (mp->modes & mp->enabled & బహుళకం)
	    s = mp->dname;
	ఒకవేళ (s != ఖాళీ)
	{
	    s = vim_strsave(s);
	    ఒకవేళ (s != ఖాళీ)
		array[idx++].pum_పాఠ్యం = s;
	}
    }

    pum_array = array;
    pum_compute_కొలత();
    pum_scrollbar = 0;
    pum_ఎత్తు = pum_కొలత;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_మొదటి = 0;
# ifdef FEAT_BEVAL_పదం
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    కొరకు (;;)
    {
	పూర్ణాంకం	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	బహిర్గతం_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ఒకవేళ (c == ESC || c == Ctrl_C || pum_array == ఖాళీ)
	    ఆపు;
	లేకపోతే ఒకవేళ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, బహుళకం);
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (c == 'k' || c == K_పైకి || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    అయితే (pum_selected > 0)
	    {
		--pum_selected;
		ఒకవేళ (*array[pum_selected].pum_పాఠ్యం != NUL)
		    ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (c == 'j' || c == K_కిందికి || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    అయితే (pum_selected < pum_కొలత - 1)
	    {
		++pum_selected;
		ఒకవేళ (*array[pum_selected].pum_పాఠ్యం != NUL)
		    ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	లేకపోతే ఒకవేళ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ఒకవేళ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, బహుళకం);
		ఆపు;
	    }
	    ఒకవేళ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ఆపు;
	}
    }

    కొరకు (idx = 0; idx < pum_కొలత; ++idx)
	vim_free(array[idx].pum_పాఠ్యం);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_పదం
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    శూన్య
pum_make_popup(char_u *మార్గం_name, పూర్ణాంకం use_mouse_pos)
{
    vimmenu_T *menu;

    ఒకవేళ (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_అడ్డువరుస = W_WINROW(curwin) + curwin->w_wrow;
	mouse_నిలువు =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_కనుగొను_menu(మార్గం_name);
    ఒకవేళ (menu != ఖాళీ)
	pum_show_popupmenu(menu);
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
is_singlewidth(పూర్ణాంకం c)
{
    ఒకవేళ (c >= 0 && c < 0x80)
	తిరిగి TRUE;
    ఒకవేళ (utf_char2cells(c) == 1)
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Set border characters for popup menu.
 */
    శూన్య
pum_సమితి_border_chars(
	పూర్ణాంకం శిఖరం,
	పూర్ణాంకం కుడి_2,
	పూర్ణాంకం bottom,
	పూర్ణాంకం ఎడమ_2,
	పూర్ణాంకం శిఖరం_ఎడమ,
	పూర్ణాంకం శిఖరం_కుడి,
	పూర్ణాంకం bottom_కుడి,
	పూర్ణాంకం bottom_ఎడమ)
{
    ఒకవేళ (!is_singlewidth(శిఖరం) || !is_singlewidth(కుడి_2)
	    || !is_singlewidth(bottom) || !is_singlewidth(ఎడమ_2)
	    || !is_singlewidth(శిఖరం_ఎడమ) || !is_singlewidth(శిఖరం_కుడి)
	    || !is_singlewidth(bottom_కుడి) || !is_singlewidth(bottom_ఎడమ))
	తిరిగి;
    pum_border_chars.శిఖరం = శిఖరం;
    pum_border_chars.కుడి_2 = కుడి_2;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ఎడమ_2 = ఎడమ_2;
    pum_border_chars.శిఖరం_ఎడమ = శిఖరం_ఎడమ;
    pum_border_chars.శిఖరం_కుడి = శిఖరం_కుడి;
    pum_border_chars.bottom_కుడి = bottom_కుడి;
    pum_border_chars.bottom_ఎడమ = bottom_ఎడమ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    స్థిర శూన్య
compute_margins(పూర్ణాంకం *కుడి_margin, పూర్ణాంకం *ఎడమ_margin, పూర్ణాంకం *ఎడమ_padding)
{
    *కుడి_margin = pum_margin && pum_border;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	*ఎడమ_padding = (pum_నిలువు < Columns - 1 - pum_border);
	*ఎడమ_margin = *కుడి_margin && (pum_నిలువు < Columns - 3);
	తిరిగి;
    }
#అయితే_ముగింపు
    *ఎడమ_padding = (pum_నిలువు > pum_border);
    *ఎడమ_margin = *కుడి_margin && (pum_నిలువు > 2);
}

/*
 * Draw the border around the popup menu
 */
    స్థిర శూన్య
pum_draw_border(శూన్య)
{
    పూర్ణాంకం	శిఖరం_అడ్డువరుస = pum_అడ్డువరుస - 1;
    పూర్ణాంకం	bottom_అడ్డువరుస = pum_అడ్డువరుస + pum_ఎత్తు;
    పూర్ణాంకం	నిలువు;
    పూర్ణాంకం	attr = highlight_attr[HLF_PMB];
    పూర్ణాంకం	కుడి_margin, ఎడమ_margin, ఎడమ_padding;
    పూర్ణాంకం	inner_వెడల్పు;

    ఒకవేళ (!pum_border)
	తిరిగి;

    (శూన్య)compute_margins(&కుడి_margin, &ఎడమ_margin, &ఎడమ_padding);

    inner_వెడల్పు = pum_వెడల్పు + pum_scrollbar + ఎడమ_padding;

    // Draw border left to right or right to left (for RTL)
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
	నిలువు = pum_నిలువు + ఎడమ_padding + 1;
    లేకపోతే
#అయితే_ముగింపు
	నిలువు = pum_నిలువు - ఎడమ_padding - 1;

    // left corners (right corners for RTL)
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	screen_putchar(pum_border_chars.శిఖరం_కుడి, శిఖరం_అడ్డువరుస, నిలువు, attr);
	ఒకవేళ (ఎడమ_margin)
	    screen_putchar(' ', శిఖరం_అడ్డువరుస, నిలువు + 1, attr);
	screen_putchar(pum_border_chars.bottom_కుడి, bottom_అడ్డువరుస, నిలువు, attr);
	ఒకవేళ (ఎడమ_margin)
	    screen_putchar(' ', bottom_అడ్డువరుస, నిలువు + 1, attr);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	screen_putchar(pum_border_chars.శిఖరం_ఎడమ, శిఖరం_అడ్డువరుస, నిలువు, attr);
	ఒకవేళ (ఎడమ_margin)
	    screen_putchar(' ', శిఖరం_అడ్డువరుస, నిలువు - 1, attr);
	screen_putchar(pum_border_chars.bottom_ఎడమ, bottom_అడ్డువరుస, నిలువు, attr);
	ఒకవేళ (ఎడమ_margin)
	    screen_putchar(' ', bottom_అడ్డువరుస, నిలువు - 1, attr);
    }

    // Top horizontal lines
    కొరకు (పూర్ణాంకం i = 0; i < inner_వెడల్పు; i++)
    {
	పూర్ణాంకం cur_నిలువు;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    cur_నిలువు = నిలువు - 1 - i;
	లేకపోతే
#అయితే_ముగింపు
	    cur_నిలువు = నిలువు + 1 + i;
	screen_putchar(pum_border_chars.శిఖరం, శిఖరం_అడ్డువరుస, cur_నిలువు, attr);
	screen_putchar(pum_border_chars.bottom, bottom_అడ్డువరుస, cur_నిలువు, attr);
    }

    // right corners (left corners for RTL)
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	screen_putchar(pum_border_chars.శిఖరం_ఎడమ, శిఖరం_అడ్డువరుస,
		నిలువు - inner_వెడల్పు - 1, attr);
	ఒకవేళ (కుడి_margin)
	    screen_putchar(' ', శిఖరం_అడ్డువరుస, నిలువు - inner_వెడల్పు - 2, attr);
	screen_putchar(pum_border_chars.bottom_ఎడమ, bottom_అడ్డువరుస,
		నిలువు - inner_వెడల్పు - 1, attr);
	ఒకవేళ (కుడి_margin)
	    screen_putchar(' ', bottom_అడ్డువరుస, నిలువు - inner_వెడల్పు - 2, attr);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	screen_putchar(pum_border_chars.శిఖరం_కుడి, శిఖరం_అడ్డువరుస,
		నిలువు + inner_వెడల్పు + 1, attr);
	ఒకవేళ (కుడి_margin)
	    screen_putchar(' ', శిఖరం_అడ్డువరుస, నిలువు + inner_వెడల్పు + 2, attr);
	screen_putchar(pum_border_chars.bottom_కుడి, bottom_అడ్డువరుస,
		నిలువు + inner_వెడల్పు + 1, attr);
	ఒకవేళ (కుడి_margin)
	    screen_putchar(' ', bottom_అడ్డువరుస, నిలువు + inner_వెడల్పు + 2, attr);
    }

    // Draw side borders
    కొరకు (పూర్ణాంకం i = 0; i < pum_ఎత్తు; i++)
    {
	పూర్ణాంకం అడ్డువరుస = pum_అడ్డువరుస + i;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	{
	    screen_putchar(pum_border_chars.ఎడమ_2, అడ్డువరుస, నిలువు, attr);
	    ఒకవేళ (ఎడమ_margin)
		screen_putchar(' ', అడ్డువరుస, నిలువు + 1, attr);
	    screen_putchar(pum_border_chars.కుడి_2, అడ్డువరుస,
		    నిలువు - inner_వెడల్పు - 1, attr);
	    ఒకవేళ (కుడి_margin)
		screen_putchar(' ', అడ్డువరుస, నిలువు - inner_వెడల్పు - 2, attr);
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    screen_putchar(pum_border_chars.ఎడమ_2, అడ్డువరుస, నిలువు, attr);
	    ఒకవేళ (ఎడమ_margin)
		screen_putchar(' ', అడ్డువరుస, నిలువు - 1, attr);
	    screen_putchar(pum_border_chars.కుడి_2, అడ్డువరుస,
		    నిలువు + inner_వెడల్పు + 1, attr);
	    ఒకవేళ (కుడి_margin)
		screen_putchar(' ', అడ్డువరుస, నిలువు + inner_వెడల్పు + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    శూన్య
put_shadow_char(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    char_u  buf[MB_MAXBYTES + 1];
    పూర్ణాంకం	    shadow_attr = highlight_attr[HLF_PMS];
    పూర్ణాంకం	    char_attr, new_attr;

    screen_getbytes(అడ్డువరుస, నిలువు, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), అడ్డువరుస, నిలువు, new_attr);
}

/*
 * Draw the shadow
 */
    స్థిర శూన్య
pum_draw_shadow(శూన్య)
{
    పూర్ణాంకం	అడ్డువరుస, నిలువు, తదుపరి_నిలువు;
    పూర్ణాంకం	వెడల్పు, స్థానభ్రంశం;
    పూర్ణాంకం	inner_వెడల్పు;
    పూర్ణాంకం	కుడి_margin, ఎడమ_margin, ఎడమ_padding;

    ఒకవేళ (!pum_shadow)
	తిరిగి;

    (శూన్య)compute_margins(&కుడి_margin, &ఎడమ_margin, &ఎడమ_padding);
    inner_వెడల్పు = pum_వెడల్పు + pum_scrollbar + pum_border + కుడి_margin;

    // Draw right side shadow (left for RTL)
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (pum_rl)
    {
	నిలువు = pum_నిలువు - inner_వెడల్పు;
	తదుపరి_నిలువు = నిలువు - 1;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	నిలువు = pum_నిలువు + inner_వెడల్పు;
	తదుపరి_నిలువు = నిలువు + 1;
    }

    కొరకు (పూర్ణాంకం i = 0; i < pum_ఎత్తు + pum_border; i++)
    {
	అడ్డువరుస = pum_అడ్డువరుస + 1 + i - pum_border;
	put_shadow_char(అడ్డువరుస, నిలువు);
	put_shadow_char(అడ్డువరుస, తదుపరి_నిలువు);
    }

    // Bottom shadow
    అడ్డువరుస = pum_అడ్డువరుస + pum_ఎత్తు + pum_border;
    వెడల్పు = inner_వెడల్పు + ఎడమ_padding + pum_border + ఎడమ_margin;
    స్థానభ్రంశం = 2 - ఎడమ_padding - pum_border - ఎడమ_margin;

    కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (pum_rl)
	    నిలువు = pum_నిలువు - స్థానభ్రంశం - i;
	లేకపోతే
#అయితే_ముగింపు
	    నిలువు = pum_నిలువు + స్థానభ్రంశం + i;
	put_shadow_char(అడ్డువరుస, నిలువు);
    }
}
