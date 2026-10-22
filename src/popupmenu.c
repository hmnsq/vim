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
#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ pumitem_T *pum_array = ಖಾಲಿ;	// items of displayed pum
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಗಾತ್ರ;			// nr of items in "pum_array"
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_selected;		// index of selected item or -1
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಮೊದಲ = 0;		// index of top item

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ call_update_screen = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಒಳ_cmdline = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಎತ್ತರ;			// nr of displayed pum items
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಅಗಲ;			// width of displayed pum items
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ತಳ_ಅಗಲ;		// width of pum items base
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಬಗೆ_ಅಗಲ;		// width of pum items kind column
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_extra_ಅಗಲ;		// width of extra stuff
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_scrollbar;		// TRUE when scrollbar present
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_rl;			// TRUE when pum is drawn 'rightleft'
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಅಡ್ಡಸಾಲು;			// top row of pum
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಲಂಬಸಾಲು;			// left column of pum

ಸ್ಥಿರ win_T *pum_ಕಿಟಕಿ = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_win_ಅಡ್ಡಸಾಲು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_win_ಎತ್ತರ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_win_ಲಂಬಸಾಲು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_win_wcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_win_ಅಗಲ;

// Some parts are not updated when a popup menu is visible.  Setting this flag
// makes pum_visible() return FALSE even when there is a popup menu.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_pretend_not_visible = FALSE;

// Border support
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_border = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_margin = 0;	// margin of 1 cell on left and right
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_shadow = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
ಸ್ಥಿರ image_placement_T *pum_imagep = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

// Border characters
ಸ್ಥಿರ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ತುದಿ;
    ಪೂರ್ಣಾಂಕ ಬಲ_2;
    ಪೂರ್ಣಾಂಕ bottom;
    ಪೂರ್ಣಾಂಕ ಎಡ_2;
    ಪೂರ್ಣಾಂಕ ತುದಿ_ಎಡ;
    ಪೂರ್ಣಾಂಕ ತುದಿ_ಬಲ;
    ಪೂರ್ಣಾಂಕ bottom_ಬಲ;
    ಪೂರ್ಣಾಂಕ bottom_ಎಡ;
} pum_border_chars;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pum_ಗಣ_selected(ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ repeat);
ಸ್ಥಿರ ಶೂನ್ಯ pum_draw_border(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ pum_draw_shadow(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ compute_margins(ಪೂರ್ಣಾಂಕ *ಬಲ_margin, ಪೂರ್ಣಾಂಕ *ಎಡ_margin, ಪೂರ್ಣಾಂಕ *ಎಡ_padding);

#ವ್ಯಾಖ್ಯಾನಿಸು PUM_DEF_ಎತ್ತರ 10

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_PUM_SETTER(feature) \
	ಶೂನ್ಯ \
    pum_ಗಣ##feature(ಪೂರ್ಣಾಂಕ enable) \
    { \
	pum_##feature = enable ? 1 : 0; \
    }

DEFINE_PUM_SETTER(border)
DEFINE_PUM_SETTER(shadow)
DEFINE_PUM_SETTER(margin)

    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	w;

    // Compute the width of the widest match and the widest extra.
    pum_ತಳ_ಅಗಲ = 0;
    pum_ಬಗೆ_ಅಗಲ = 0;
    pum_extra_ಅಗಲ = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pum_ಗಾತ್ರ; ++i)
    {
	ಆದರೆ (pum_array[i].pum_ಪಠ್ಯ != ಖಾಲಿ)
	{
	    w = vim_strsize(pum_array[i].pum_ಪಠ್ಯ);
	    ಆದರೆ (pum_ತಳ_ಅಗಲ < w)
		pum_ತಳ_ಅಗಲ = w;
	}
	ಆದರೆ (pum_array[i].pum_ಬಗೆ != ಖಾಲಿ)
	{
	    w = vim_strsize(pum_array[i].pum_ಬಗೆ) + 1;
	    ಆದರೆ (pum_ಬಗೆ_ಅಗಲ < w)
		pum_ಬಗೆ_ಅಗಲ = w;
	}
	ಆದರೆ (pum_array[i].pum_extra != ಖಾಲಿ)
	{
	    w = vim_strsize(pum_array[i].pum_extra) + 1;
	    ಆದರೆ (pum_extra_ಅಗಲ < w)
		pum_extra_ಅಗಲ = w;
	}
    }
}

/*
 * Calculate vertical placement for popup menu.
 * Sets pum_row and pum_height based on available space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_vertical_placement(
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2,
	ಪೂರ್ಣಾಂಕ above_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ context_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ cline_visible_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ extra_ಎತ್ತರ = 2 * pum_border + pum_shadow;
    ಪೂರ್ಣಾಂಕ extra_ಸ್ಥಾನಾಂತರ = pum_border + pum_shadow;

    pum_ಎತ್ತರ = MIN(ಗಾತ್ರ_2, PUM_DEF_ಎತ್ತರ);
    ಆದರೆ (p_ph > 0 && pum_ಎತ್ತರ > p_ph)
	pum_ಎತ್ತರ = p_ph;

    // Put the pum below "pum_win_row" if possible.  If there are few lines
    // decide on where there is more room.
    ಆದರೆ (pum_win_ಅಡ್ಡಸಾಲು + 2 >= ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು - pum_ಎತ್ತರ - extra_ಎತ್ತರ
	    && pum_win_ಅಡ್ಡಸಾಲು - above_ಅಡ್ಡಸಾಲು > (ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು - above_ಅಡ್ಡಸಾಲು) / 2)
    {
	// pum above "pum_win_row"
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ಸಾಲುಗಳು = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Leave two lines of context if possible
	    context_ಸಾಲುಗಳು = MIN(2, curwin->w_wrow - curwin->w_cline_ಅಡ್ಡಸಾಲು);

	ಆದರೆ (pum_win_ಅಡ್ಡಸಾಲು >= ಗಾತ್ರ_2 + context_ಸಾಲುಗಳು + extra_ಎತ್ತರ)
	{
	    pum_ಅಡ್ಡಸಾಲು = pum_win_ಅಡ್ಡಸಾಲು - ಗಾತ್ರ_2 - context_ಸಾಲುಗಳು - extra_ಸ್ಥಾನಾಂತರ;
	    pum_ಎತ್ತರ = ಗಾತ್ರ_2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pum_ಅಡ್ಡಸಾಲು = pum_border;
	    pum_ಎತ್ತರ = pum_win_ಅಡ್ಡಸಾಲು - context_ಸಾಲುಗಳು - extra_ಎತ್ತರ;
	}
	ಆದರೆ (p_ph > 0 && pum_ಎತ್ತರ > p_ph)
	{
	    pum_ಅಡ್ಡಸಾಲು += pum_ಎತ್ತರ - p_ph;
	    pum_ಎತ್ತರ = p_ph;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// pum below "pum_win_row"
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    // for cmdline pum, no need for context lines
	    context_ಸಾಲುಗಳು = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Leave three lines of context if possible
	    validate_cheight();
	    cline_visible_ಸ್ಥಾನಾಂತರ = curwin->w_cline_ಅಡ್ಡಸಾಲು +
		curwin->w_cline_ಎತ್ತರ - curwin->w_wrow;
	    context_ಸಾಲುಗಳು = MIN(3, cline_visible_ಸ್ಥಾನಾಂತರ);
	}

	pum_ಅಡ್ಡಸಾಲು = pum_win_ಅಡ್ಡಸಾಲು + context_ಸಾಲುಗಳು + pum_border;
	pum_ಎತ್ತರ = MIN(ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು - pum_ಅಡ್ಡಸಾಲು - extra_ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2);
	ಆದರೆ (p_ph > 0 && pum_ಎತ್ತರ > p_ph)
	    pum_ಎತ್ತರ = p_ph;
    }

#ಆದರೆ defined(FEAT_QUICKFIX)
    // If there is a preview window above avoid drawing over it.
    ಆದರೆ (above_ಅಡ್ಡಸಾಲು > 0 && pum_ಅಡ್ಡಸಾಲು < above_ಅಡ್ಡಸಾಲು && pum_ಎತ್ತರ > above_ಅಡ್ಡಸಾಲು)
    {
	pum_ಅಡ್ಡಸಾಲು = above_ಅಡ್ಡಸಾಲು;
	pum_ಎತ್ತರ = pum_win_ಅಡ್ಡಸಾಲು - above_ಅಡ್ಡಸಾಲು;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Try to set 'pum_width' so that it fits within available_width.
 * Returns TRUE if pum_width was successfully set, FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_pum_ಅಗಲ_aligned_with_cursor(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಲಭ್ಯ_ಅಗಲ)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_padding = TRUE;

    ಆದರೆ (ಅಗಲ < p_pw)
    {
	ಅಗಲ = p_pw;
	ಕೊನೆ_padding = FALSE;
    }
    ಆದರೆ (p_pmw > 0 && ಅಗಲ >= p_pmw)
    {
	ಅಗಲ = p_pmw;
	ಕೊನೆ_padding = FALSE;
    }

    pum_ಅಗಲ = ಅಗಲ + (ಕೊನೆ_padding && ಅಗಲ >= p_pw ? 1 : 0);
    ಹಿಂತಿರುಗಿ ಲಭ್ಯ_ಅಗಲ >= pum_ಅಗಲ;
}

/*
 * Calculate horizontal placement for popup menu. Sets pum_col and pum_width
 * based on cursor position and available space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_compute_horizontal_placement(ಪೂರ್ಣಾಂಕ cursor_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ desired_ಅಗಲ = pum_ತಳ_ಅಗಲ + pum_ಬಗೆ_ಅಗಲ + pum_extra_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಲಭ್ಯ_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = pum_border && (cursor_ಲಂಬಸಾಲು >= Columns - 2)
	    ? (3 - Columns + cursor_ಲಂಬಸಾಲು) : 0;
	ಲಭ್ಯ_ಅಗಲ = cursor_ಲಂಬಸಾಲು + 1;
	pum_ಲಂಬಸಾಲು = cursor_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = pum_border && (cursor_ಲಂಬಸಾಲು <= 1) ? (2 - cursor_ಲಂಬಸಾಲು) : 0;
	ಲಭ್ಯ_ಅಗಲ = Columns - cursor_ಲಂಬಸಾಲು;
	pum_ಲಂಬಸಾಲು = cursor_ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    }

    ಲಭ್ಯ_ಅಗಲ -= trailing_overhead + ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;

    // Case 1: Align pum with "cursor_col"
    ಆದರೆ (ಗಣ_pum_ಅಗಲ_aligned_with_cursor(desired_ಅಗಲ, ಲಭ್ಯ_ಅಗಲ))
	ಹಿಂತಿರುಗಿ;
    // Show the pum truncated, provided it is at least as wide as 'pum_width'
    ಆದರೆ (ಲಭ್ಯ_ಅಗಲ > p_pw)
    {
	pum_ಅಗಲ = ಲಭ್ಯ_ಅಗಲ;
	ಹಿಂತಿರುಗಿ;
    }

    // Case 2: Truncated pum is no longer aligned with "cursor_col"
    ಲಭ್ಯ_ಅಗಲ = Columns - trailing_overhead - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (ಲಭ್ಯ_ಅಗಲ > p_pw)
    {
	pum_ಅಗಲ = p_pw;  // Truncate beyond 'pum_width'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    pum_ಲಂಬಸಾಲು = pum_ಅಗಲ + trailing_overhead;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    pum_ಲಂಬಸಾಲು = Columns - pum_ಅಗಲ - trailing_overhead;
	ಹಿಂತಿರುಗಿ;
    }

    // Case 3: Not enough room anywhere, use what we have
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
	pum_ಲಂಬಸಾಲು = Columns - 1 - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	pum_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
    pum_ಅಗಲ = Columns - trailing_overhead;
}

/*
 * Show the popup menu with items "array[size]".
 * "array" must remain valid until pum_undisplay() is called!
 * When possible the leftmost character is aligned with cursor column.
 * The menu appears above the screen line "row" or at "row" + "height" - 1.
 */
    ಶೂನ್ಯ
pum_display(
	pumitem_T   *array,
	ಪೂರ್ಣಾಂಕ	    ಗಾತ್ರ_2,
	ಪೂರ್ಣಾಂಕ	    selected,   // index of initially selected item, -1 if
				// out of range
	ಪೂರ್ಣಾಂಕ	    pum_wcol)   // screen column to align the menu to, or -1
				// to use the cursor column
{
    ಪೂರ್ಣಾಂಕ	    cursor_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    above_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    redo_ಎಣಿಕೆ = 0;
#ಆದರೆ defined(FEAT_QUICKFIX)
    win_T   *pvwin;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    pum_rl = (State & ಬಹುಲಕ_CMDLINE) == 0 && curwin->w_p_rl;
#ಆದರೆ_ಅಂತ್ಯ

    ಮಾಡು
    {
	above_ಅಡ್ಡಸಾಲು = 0;
	ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;

	// Pretend the pum is already there to avoid that must_redraw is set
	// when 'cuc' is on.
	pum_array = (pumitem_T *)1;
	validate_cursor_ಲಂಬಸಾಲು();
	pum_array = ಖಾಲಿ;

	// Remember the essential parts of the window position and size, so we
	// can decide when to reposition the popup menu.
	pum_ಕಿಟಕಿ = curwin;
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    // cmdline completion popup menu
	    pum_win_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ
	    pum_win_ಅಡ್ಡಸಾಲು = curwin->w_wrow + W_WINROW(curwin);
	pum_win_ಎತ್ತರ = curwin->w_ಎತ್ತರ;
	pum_win_ಲಂಬಸಾಲು = curwin->w_wincol;
	pum_win_wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	pum_win_ಅಗಲ = curwin->w_ಅಗಲ;

#ಆದರೆ defined(FEAT_QUICKFIX)
	FOR_ALL_WINDOWS(pvwin)
	    ಆದರೆ (pvwin->w_p_pvw)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (pvwin != ಖಾಲಿ)
	{
	    ಆದರೆ (W_WINROW(pvwin) < W_WINROW(curwin))
		above_ಅಡ್ಡಸಾಲು = W_WINROW(pvwin) + pvwin->w_ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (W_WINROW(pvwin) > W_WINROW(curwin) + curwin->w_ಎತ್ತರ)
		ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು = W_WINROW(pvwin);
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Figure out the vertical size and position of the pum.
	pum_compute_vertical_placement(ಗಾತ್ರ_2, above_ಅಡ್ಡಸಾಲು, ಕೆಳಗೆ_ಅಡ್ಡಸಾಲು);

	// don't display when we only have room for one line
	ಆದರೆ (pum_ಎತ್ತರ < 1 || (pum_ಎತ್ತರ == 1 && ಗಾತ್ರ_2 > 1))
	    ಹಿಂತಿರುಗಿ;

	pum_array = array;
	pum_ಗಾತ್ರ = ಗಾತ್ರ_2;
	pum_compute_ಗಾತ್ರ();

	// Calculate cursor column
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    // cmdline completion popup menu
	    cursor_ಲಂಬಸಾಲು = cmdline_compl_startcol();
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ wcol = pum_wcol >= 0 ? pum_wcol : curwin->w_wcol;
	    // w_wcol includes virtual text "above".
	    ಆದರೆ (curwin->w_ಅಗಲ > 0)
		wcol %= curwin->w_ಅಗಲ;
	    ಇಲ್ಲದಿದ್ದರೆ
		wcol = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    // w_wcol does not account for text concealed before the cursor;
	    // shift by the offset win_line() recorded for the cursor line so the
	    // menu lines up with the visible text.
	    ಆದರೆ (curwin->w_p_cole > 0 && conceal_cursor_ಸಾಲು(curwin))
	    {
		wcol -= curwin->w_wcol_conceal_off;
		ಆದರೆ (wcol < 0)
		    wcol = 0;
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (pum_rl)
		cursor_ಲಂಬಸಾಲು = curwin->w_wincol + curwin->w_ಅಗಲ - wcol - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		cursor_ಲಂಬಸಾಲು = curwin->w_wincol + wcol;
	}

	// if there are more items than room we need a scrollbar
	pum_scrollbar = (pum_ಎತ್ತರ < ಗಾತ್ರ_2) ? 1 : 0;

	// Figure out the horizontal size and position of the pum.
	pum_compute_horizontal_placement(cursor_ಲಂಬಸಾಲು);

	// Set selected item and redraw.  If the window size changed need to
	// redo the positioning.  Limit this to two times, when there is not
	// much room the window size will keep changing.
    } ಆಗ (pum_ಗಣ_selected(selected, redo_ಎಣಿಕೆ) && ++redo_ಎಣಿಕೆ <= 2);

    pum_redraw();
}

/*
 * Set a flag that when pum_redraw() is called it first calls update_screen().
 * This will avoid clearing and redrawing the popup menu, prevent flicker.
 */
    ಶೂನ್ಯ
pum_call_update_screen(ಶೂನ್ಯ)
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
    ಪೂರ್ಣಾಂಕ
pum_under_menu(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ only_redrawing)
{
    ಪೂರ್ಣಾಂಕ	extra_ಎಡ = pum_border + (pum_margin && pum_border ? 1 : 0);
    ಪೂರ್ಣಾಂಕ	extra_ಬಲ = pum_border + (pum_margin && pum_border ? 1 : 0)
						+ (pum_shadow ? 2 : 0);
    ಪೂರ್ಣಾಂಕ	extra_above = pum_border;
    ಪೂರ್ಣಾಂಕ	extra_ಕೆಳಗೆ = pum_border + (pum_shadow ? 1 : 0);
    ಪೂರ್ಣಾಂಕ	ತುದಿ = pum_ಅಡ್ಡಸಾಲು - extra_above;
    ಪೂರ್ಣಾಂಕ	bot = pum_ಅಡ್ಡಸಾಲು + pum_ಎತ್ತರ + extra_ಕೆಳಗೆ;
    ಪೂರ್ಣಾಂಕ	ಎಡ_2 = pum_ಲಂಬಸಾಲು - 1 - extra_ಎಡ;
    ಪೂರ್ಣಾಂಕ	ಬಲ_2 = pum_ಲಂಬಸಾಲು + pum_ಅಗಲ + pum_scrollbar + extra_ಬಲ;

    ಆದರೆ (!((!only_redrawing || pum_will_redraw)
	    && ಅಡ್ಡಸಾಲು >= ತುದಿ && ಅಡ್ಡಸಾಲು < bot && ಲಂಬಸಾಲು >= ಎಡ_2 && ಲಂಬಸಾಲು < ಬಲ_2))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (pum_shadow)
    {
	// The shadow recolors the cells underneath.  When the menu will be
	// redrawn, leave the shadow cells unprotected so the window refreshes
	// them first, clearing stale content left by a larger menu.
	ಆದರೆ (only_redrawing)
	{
	    ಆದರೆ (ಲಂಬಸಾಲು >= ಬಲ_2 - 2 || ಅಡ್ಡಸಾಲು == bot - 1)
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Menu stays as-is: exclude only the corner cells the shadow never
	    // draws.
	    ಪೂರ್ಣಾಂಕ	    ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;

	    compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);
	    ಆದರೆ (ಅಡ್ಡಸಾಲು == ತುದಿ && ಲಂಬಸಾಲು >= ಬಲ_2 - 2)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (ಅಡ್ಡಸಾಲು == bot - 1 && ಲಂಬಸಾಲು < pum_ಲಂಬಸಾಲು + 2 - ಎಡ_padding - pum_border
		    - ಎಡ_margin)
		ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Computes attributes of text on the popup menu.
 * Returns attributes for every cell, or NULL if all attributes are the same.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *
pum_compute_ಪಠ್ಯ_attrs(char_u *ಪಠ್ಯ, hlf_T hlf, ಪೂರ್ಣಾಂಕ user_hlattr)
{
    ಪೂರ್ಣಾಂಕ		i;
    size_t	leader_len;
    ಪೂರ್ಣಾಂಕ		char_cells;
    ಪೂರ್ಣಾಂಕ		new_attr;
    char_u	*ptr = ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		cell_idx = 0;
    garray_T	*ga = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		*attrs = ಖಾಲಿ;
    char_u	*leader = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಒಳ_fuzzy;
    ಪೂರ್ಣಾಂಕ		matched_len = -1;
    int_u	char_pos = 0;
    ಪೂರ್ಣಾಂಕ		is_select = FALSE;

    ಆದರೆ (*ಪಠ್ಯ == NUL || (hlf != HLF_PSI && hlf != HLF_PNI)
	    || (highlight_attr[HLF_PMSI] == highlight_attr[HLF_PSI]
		&& highlight_attr[HLF_PMNI] == highlight_attr[HLF_PNI]))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    is_select = hlf == HLF_PSI;
    leader = (State & ಬಹುಲಕ_CMDLINE) ? cmdline_compl_ನಮೂನೆ()
							  : ins_compl_leader();
    ಆದರೆ (leader == ಖಾಲಿ || *leader == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    attrs = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, vim_strsize(ಪಠ್ಯ));
    ಆದರೆ (attrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಒಳ_fuzzy = (State & ಬಹುಲಕ_CMDLINE) ? cmdline_compl_is_fuzzy()
					  : (get_cot_flags() & COT_FUZZY) != 0;
    leader_len = STRLEN(leader);

    ಆದರೆ (ಒಳ_fuzzy)
    {
	ga = fuzzy_ಹೊಂದಿಕೆ_str_with_pos(ಪಠ್ಯ, leader);
	ಆದರೆ (!ga)
	{
	    vim_free(attrs);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    ಆಗ (*ptr != NUL)
    {
	new_attr = highlight_attr[hlf];

	ಆದರೆ (ga != ಖಾಲಿ)
	{
	    // Handle fuzzy matching
	    ಗಾಗಿ (i = 0; i < ga->ga_len; i++)
	    {
		ಆದರೆ (char_pos == ((int_u *)ga->ga_ದತ್ತಾಂಶ)[i])
		{
		    new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		    new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (matched_len < 0 && MB_STRNICMP(ptr, leader, leader_len) == 0)
		matched_len = (ಪೂರ್ಣಾಂಕ)leader_len;
	    ಆದರೆ (matched_len > 0)
	    {
		new_attr = highlight_attr[is_select ? HLF_PMSI : HLF_PMNI];
		new_attr = hl_combine_attr(highlight_attr[hlf], new_attr);
		matched_len--;
	    }
	}

	new_attr = hl_combine_attr(highlight_attr[HLF_PNI], new_attr);
	ಆದರೆ (user_hlattr > 0)
	    new_attr = hl_combine_attr(new_attr, user_hlattr);

	char_cells = mb_ptr2cells(ptr);
	ಗಾಗಿ (i = 0; i < char_cells; i++)
	    attrs[cell_idx + i] = new_attr;
	cell_idx += char_cells;

	MB_PTR_ADV(ptr);
	char_pos++;
    }

    ಆದರೆ (ga != ಖಾಲಿ)
    {
	ga_clear(ga);
	vim_free(ga);
    }
    ಹಿಂತಿರುಗಿ attrs;
}

/*
 * Displays text on the popup menu with specific attributes.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_screen_puts_with_attrs(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		cells ಬಳಸದ,
    char_u	*ಪಠ್ಯ,
    ಪೂರ್ಣಾಂಕ		textlen,
    ಪೂರ್ಣಾಂಕ		*attrs)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಆರಂಭ = ಲಂಬಸಾಲು;
    char_u	*ptr = ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		char_len;
    ಪೂರ್ಣಾಂಕ		attr;

    // Render text with proper attributes
    ಆಗ (*ptr != NUL && ptr < ಪಠ್ಯ + textlen)
    {
	char_len = mb_ptr2len(ptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    attr = attrs[ಲಂಬಸಾಲು_ಆರಂಭ + cells - ಲಂಬಸಾಲು - 1];
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    attr = attrs[ಲಂಬಸಾಲು - ಲಂಬಸಾಲು_ಆರಂಭ];
	screen_puts_len(ptr, char_len, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	ಲಂಬಸಾಲು += mb_ptr2cells(ptr);
	ptr += char_len;
    }
}

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ char_u *
pum_get_ಅಂಶ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ type)
{
    ಆಯ್ಕೆ(type)
    {
	ಸ್ಥಿತಿ CPT_ABBR: ಹಿಂತಿರುಗಿ pum_array[ಸೂಚ್ಯಂಕ].pum_ಪಠ್ಯ;
	ಸ್ಥಿತಿ CPT_ಬಗೆ: ಹಿಂತಿರುಗಿ pum_array[ಸೂಚ್ಯಂಕ].pum_ಬಗೆ;
	ಸ್ಥಿತಿ CPT_MENU: ಹಿಂತಿರುಗಿ pum_array[ಸೂಚ್ಯಂಕ].pum_extra;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
pum_user_attr_combine(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ user_attr[] = {
	pum_array[idx].pum_user_abbr_hlattr,
	pum_array[idx].pum_user_ಬಗೆ_hlattr,
    };

    ಹಿಂತಿರುಗಿ user_attr[type] > 0 ? hl_combine_attr(attr, user_attr[type]) : attr;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
/*
 * Display RTL text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_display_rtl_ಪಠ್ಯ(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ     ಲಂಬಸಾಲು,
	char_u  *st,
	ಪೂರ್ಣಾಂಕ     attr,
	ಪೂರ್ಣಾಂಕ     *attrs,
	ಪೂರ್ಣಾಂಕ     ಅಗಲ,
	ಪೂರ್ಣಾಂಕ     ಅಗಲ_ಎಲ್ಲೆ,
	ಪೂರ್ಣಾಂಕ     totwidth,
	ಪೂರ್ಣಾಂಕ     ಮುಂದಿನ_isempty,
	ಪೂರ್ಣಾಂಕ	selected)
{
    char_u  *rt = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ     cells = 0;
    ಪೂರ್ಣಾಂಕ     over_cell = 0;
    ಪೂರ್ಣಾಂಕ     truncated = FALSE;
    ಪೂರ್ಣಾಂಕ     pad = ಮುಂದಿನ_isempty ? 0 : 2;
    ಪೂರ್ಣಾಂಕ     ಉಳಿದ = 0;
    ಪೂರ್ಣಾಂಕ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ಪೂರ್ಣಾಂಕ	    truncrl = curwin->w_fill_chars.truncrl != NUL
					? curwin->w_fill_chars.truncrl : '<';

    ಆದರೆ (st == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;

    rt = ಹಿಮ್ಮುಖ_ಪಠ್ಯ(st);
    ಆದರೆ (rt == ಖಾಲಿ)
    {
	VIM_CLEAR(st);
	ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
    }

    char_u *rt_ಆರಂಭ = rt;
    cells = mb_string2cells(rt, -1);
    truncated = ಅಗಲ_ಎಲ್ಲೆ - totwidth < cells + pad;

    // only draw the text that fits
    ಆದರೆ (cells > ಅಗಲ_ಎಲ್ಲೆ)
    {
	ಮಾಡು
	{
	    cells -= has_mbyte ? (*mb_ptr2cells)(rt) : 1;
	    MB_PTR_ADV(rt);
	} ಆಗ (cells > ಅಗಲ_ಎಲ್ಲೆ);

	ಆದರೆ (cells < ಅಗಲ_ಎಲ್ಲೆ)
	{
	    // Most left character requires 2-cells
	    // but only 1 cell is available on screen.
	    // Put a '<' on the left of the pum item.
	    *(--rt) = '<';
	    cells++;
	}
    }

    ಆದರೆ (truncated)
    {
	char_u  *orig_rt = rt;
	ಪೂರ್ಣಾಂಕ     ಗಾತ್ರ_2 = 0;

	ಉಳಿದ = ಅಗಲ_ಎಲ್ಲೆ - totwidth - 1;
	cells = mb_string2cells(rt, -1);
	ಆದರೆ (cells > ಉಳಿದ)
	{
	    ಆಗ (cells > ಉಳಿದ)
	    {
		MB_PTR_ADV(orig_rt);
		cells -= has_mbyte ? (*mb_ptr2cells)(orig_rt) : 1;
	    }
	}
	ಗಾತ್ರ_2 = (ಪೂರ್ಣಾಂಕ)STRLEN(orig_rt);
	ಆದರೆ (cells < ಉಳಿದ)
	    over_cell = ಉಳಿದ - cells;

	cells = mb_string2cells(orig_rt, ಗಾತ್ರ_2);
	ಅಗಲ = cells + over_cell + 1;
	rt = orig_rt;

	screen_putchar(truncrl, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - ಅಗಲ + 1, trunc_attr);

	ಆದರೆ (over_cell > 0)
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು - ಅಗಲ + 2,
		    ಲಂಬಸಾಲು - ಅಗಲ + 2 + over_cell, ' ', ' ', attr);
    }

    ಆದರೆ (attrs == ಖಾಲಿ)
	screen_puts_len(rt, (ಪೂರ್ಣಾಂಕ)STRLEN(rt), ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - cells + 1, attr);
    ಇಲ್ಲದಿದ್ದರೆ
	pum_screen_puts_with_attrs(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - cells + 1, cells, rt,
		(ಪೂರ್ಣಾಂಕ)STRLEN(rt), attrs);

    vim_free(rt_ಆರಂಭ);
    VIM_CLEAR(st);
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು - ಅಗಲ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Display LTR text with proper attributes in the popup menu.
 * Returns the adjusted column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_display_ltr_ಪಠ್ಯ(
	ಪೂರ್ಣಾಂಕ     ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ     ಲಂಬಸಾಲು,
	char_u  *st,
	ಪೂರ್ಣಾಂಕ     attr,
	ಪೂರ್ಣಾಂಕ     *attrs,
	ಪೂರ್ಣಾಂಕ     ಅಗಲ,        // width already calculated in outer loop
	ಪೂರ್ಣಾಂಕ     ಅಗಲ_ಎಲ್ಲೆ,
	ಪೂರ್ಣಾಂಕ     totwidth,
	ಪೂರ್ಣಾಂಕ     ಮುಂದಿನ_isempty,
	ಪೂರ್ಣಾಂಕ	selected)
{
    ಪೂರ್ಣಾಂಕ     ಗಾತ್ರ_2 = 0;
    ಪೂರ್ಣಾಂಕ     cells = 0;
    char_u  *st_ಕೊನೆ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ     over_cell = 0;
    ಪೂರ್ಣಾಂಕ     pad = ಮುಂದಿನ_isempty ? 0 : 2;
    ಪೂರ್ಣಾಂಕ     truncated = FALSE;
    ಪೂರ್ಣಾಂಕ     ಉಳಿದ = 0;
    ಪೂರ್ಣಾಂಕ	    trunc_attr = highlight_attr[selected ? HLF_PSI : HLF_PNI];
    ಪೂರ್ಣಾಂಕ	    trunc = curwin->w_fill_chars.trunc != NUL
					    ? curwin->w_fill_chars.trunc : '>';

    ಆದರೆ (st == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;

    ಗಾತ್ರ_2 = (ಪೂರ್ಣಾಂಕ)STRLEN(st);
    cells = (*mb_string2cells)(st, ಗಾತ್ರ_2);
    truncated = ಅಗಲ_ಎಲ್ಲೆ - totwidth < cells + pad;

    // only draw the text that fits
    ಆಗ (ಗಾತ್ರ_2 > 0 && ಲಂಬಸಾಲು + cells > ಅಗಲ_ಎಲ್ಲೆ + pum_ಲಂಬಸಾಲು)
    {
	--ಗಾತ್ರ_2;
	ಆದರೆ (has_mbyte)
	{
	    ಗಾತ್ರ_2 -= (*mb_ತಲೆ_off)(st, st + ಗಾತ್ರ_2);
	    cells -= (*mb_ptr2cells)(st + ಗಾತ್ರ_2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    --cells;
    }

    // truncated
    ಆದರೆ (truncated)
    {
	ಉಳಿದ = ಅಗಲ_ಎಲ್ಲೆ - totwidth - 1;
	ಆದರೆ (cells > ಉಳಿದ)
	{
	    st_ಕೊನೆ = st + ಗಾತ್ರ_2;
	    ಆಗ (st_ಕೊನೆ > st && cells > ಉಳಿದ)
	    {
		MB_PTR_ಹಿಂಭಾಗ(st, st_ಕೊನೆ);
		cells -= has_mbyte ? (*mb_ptr2cells)(st_ಕೊನೆ) : 1;
	    }
	    ಗಾತ್ರ_2 = st_ಕೊನೆ - st;
	}

	ಆದರೆ (cells < ಉಳಿದ)
	    over_cell = ಉಳಿದ - cells;
	cells = mb_string2cells(st, ಗಾತ್ರ_2);
	ಅಗಲ = cells + over_cell + 1;
    }

    ಆದರೆ (attrs == ಖಾಲಿ)
	screen_puts_len(st, ಗಾತ್ರ_2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
    ಇಲ್ಲದಿದ್ದರೆ
	pum_screen_puts_with_attrs(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, cells, st, ಗಾತ್ರ_2, attrs);

    ಆದರೆ (truncated)
    {
	ಆದರೆ (over_cell > 0)
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು + cells,
		    ಲಂಬಸಾಲು + cells + over_cell, ' ', ' ', attr);

	screen_putchar(trunc, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + cells + over_cell, trunc_attr);
    }

    VIM_CLEAR(st);
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು + ಅಗಲ;
}


/*
 * Process and display a single popup menu item (text/kind/extra).
 * Returns the new column position after drawing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_process_ಅಂಶ(
	ಪೂರ್ಣಾಂಕ     ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ     ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ     idx,
	ಪೂರ್ಣಾಂಕ     j,         // Current position in order array
	ಪೂರ್ಣಾಂಕ     *ಕ್ರಮ,    // Order array
	hlf_T   hlf,
	ಪೂರ್ಣಾಂಕ     attr,
	ಪೂರ್ಣಾಂಕ     *totwidth_ptr,
	ಪೂರ್ಣಾಂಕ     ಮುಂದಿನ_isempty)
{
    ಪೂರ್ಣಾಂಕ     ಅಂಶ_type = ಕ್ರಮ[j];
    char_u  *s = ಖಾಲಿ;
    char_u  *p = pum_get_ಅಂಶ(idx, ಅಂಶ_type);
    ಪೂರ್ಣಾಂಕ     ಅಗಲ = 0;  // item width
    ಪೂರ್ಣಾಂಕ     w;		// char width
    ಪೂರ್ಣಾಂಕ	    selected = idx == pum_selected;

    ಗಾಗಿ ( ; ; MB_PTR_ADV(p))
    {
	ಆದರೆ (s == ಖಾಲಿ)
	    s = p;
	w = ptr2cells(p);
	ಆದರೆ (*p != NUL && *p != TAB && *totwidth_ptr + w <= pum_ಅಗಲ)
	{
	    ಅಗಲ += w;
	    ಮುಂದುವರಿ;
	}

	// Display the text that fits or comes before a Tab.
	// First convert it to printable characters.
	char_u  *st;
	ಪೂರ್ಣಾಂಕ     *attrs = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ     saved = *p;

	ಆದರೆ (saved != NUL)
	    *p = NUL;
	st = transstr(s);
	ಆದರೆ (saved != NUL)
	    *p = saved;

	ಆದರೆ (ಅಂಶ_type == CPT_ABBR)
	    attrs = pum_compute_ಪಠ್ಯ_attrs(st, hlf,
		      pum_array[idx].pum_user_abbr_hlattr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    ಲಂಬಸಾಲು = pum_display_rtl_ಪಠ್ಯ(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, st, attr, attrs,
		    ಅಗಲ, pum_ಅಗಲ, *totwidth_ptr, ಮುಂದಿನ_isempty, selected);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಲಂಬಸಾಲು = pum_display_ltr_ಪಠ್ಯ(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, st, attr, attrs,
		    ಅಗಲ, pum_ಅಗಲ, *totwidth_ptr, ಮುಂದಿನ_isempty, selected);

	ಆದರೆ (attrs != ಖಾಲಿ)
	    VIM_CLEAR(attrs);

	ಆದರೆ (*p != TAB)
	    ನಿಲ್ಲಿಸು;

	// Display two spaces for a Tab.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	{
	    screen_puts_len((char_u *)"  ", 2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1, attr);
	    ಲಂಬಸಾಲು -= 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    screen_puts_len((char_u *)"  ", 2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	    ಲಂಬಸಾಲು += 2;
	}
	*totwidth_ptr += 2;
	s = ಖಾಲಿ;  // start text at next char
	ಅಗಲ = 0;
    }

    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

/*
 * Draw the scrollbar for the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_scrollbar(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	i,
	ಪೂರ್ಣಾಂಕ	thumb_pos,
	ಪೂರ್ಣಾಂಕ	thumb_ಎತ್ತರ)
{
    ಆದರೆ (pum_scrollbar <= 0)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ attr = (i >= thumb_pos && i < thumb_pos + thumb_ಎತ್ತರ) ?
			highlight_attr[HLF_PST] : highlight_attr[HLF_PSB];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
	screen_putchar(' ', ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು - pum_ಅಗಲ, attr);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	screen_putchar(' ', ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು + pum_ಅಗಲ, attr);
}

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
pum_align_ಕ್ರಮ(ಪೂರ್ಣಾಂಕ *ಕ್ರಮ)
{
    ಪೂರ್ಣಾಂಕ is_default = cia_flags == 0;
    ಕ್ರಮ[0] = is_default ? CPT_ABBR : cia_flags / 100;
    ಕ್ರಮ[1] = is_default ? CPT_ಬಗೆ : (cia_flags / 10) % 10;
    ಕ್ರಮ[2] = is_default ? CPT_MENU : cia_flags % 10;
}

ಸ್ಥಿರ ಶೂನ್ಯ pum_free_bg(ಶೂನ್ಯ);

/*
 * Called when the pum opacity value has changed.
 * Invalidates cached background and triggers redraw if pum is visible.
 */
    ಶೂನ್ಯ
pum_opacity_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    // Invalidate cached background so it gets re-saved.
    pum_free_bg();

    ಆದರೆ (pum_visible())
    {
	// Force full screen clear so ScreenAttrs doesn't retain
	// stale blended values from the previous pumopacity.
	redraw_all_later(UPD_CLEAR);
	call_update_screen = TRUE;
	pum_redraw();
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
pum_free_bg(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ k;
    VIM_CLEAR(pum_bg_attrs);
    VIM_CLEAR(pum_bg_ಸಾಲುಗಳು);
    VIM_CLEAR(pum_bg_ಸಾಲುಗಳು_UC);
    ಗಾಗಿ (k = 0; k < MAX_MCO; ++k)
	VIM_CLEAR(pum_bg_ಸಾಲುಗಳು_C[k]);
    pum_bg_ತುದಿ = 0;
    pum_bg_bot = 0;
    pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
}

/*
 * Redraw the popup menu, using "pum_first" and "pum_selected".
 */
    ಶೂನ್ಯ
pum_redraw(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    hlf_T	*hlfs; // array used for highlights
    hlf_T	hlf;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		totwidth;
    ಪೂರ್ಣಾಂಕ		thumb_pos = 0;
    ಪೂರ್ಣಾಂಕ		thumb_ಎತ್ತರ = 1;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_type;
    ಪೂರ್ಣಾಂಕ		ಕ್ರಮ[3];
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_isempty = FALSE;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		ವಸ್ತುಗಳು_ಅಗಲ_array[3] = { pum_ತಳ_ಅಗಲ, pum_ಬಗೆ_ಅಗಲ,
							    pum_extra_ಅಗಲ };
    ಪೂರ್ಣಾಂಕ		basic_ಅಗಲ;  // first item width
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_isabbr = FALSE;
    ಪೂರ್ಣಾಂಕ		orig_attr = -1;
    ಪೂರ್ಣಾಂಕ		scroll_range = pum_ಗಾತ್ರ - pum_ಎತ್ತರ;
    bool	override_success;
    ಪೂರ್ಣಾಂಕ		opacity_ಸಕ್ರಿಯ = (p_po > 0 && p_po < 100);

    // Use current window for highlight overrides when using 'winhighlight'
    override_success = ತಳ್ಳು_highlight_overrides(curwin->w_hl, curwin->w_hl_len);

    // Batch the underlying screen update and the pum drawing into a single
    // synchronized output frame to avoid flicker.
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_ENABLE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    // Create a image placement so that other images are clipped correctly
    ಆದರೆ (pum_imagep == ಖಾಲಿ)
	// Make pom always be on top of any image placement with same zindex.
	pum_imagep = image_placement_new(ಖಾಲಿ, UINT_MAX,
		IMAGE_PLACEMENT_USE_PUM, true);
    ಆದರೆ (pum_imagep != ಖಾಲಿ)
    {
	image_placement_ಗಣ_zindex(pum_imagep, POPUPMENU_ZINDEX);
	image_placement_ಗಣ_position(pum_imagep, pum_ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು);
	image_placement_ಗಣ_bounding_box(pum_imagep, pum_ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು,
		pum_ಎತ್ತರ, pum_ಅಗಲ);
	image_placement_ಗಣ_enable(pum_imagep, true);
    }
#ಆದರೆ_ಅಂತ್ಯ

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

    ಆದರೆ (call_update_screen)
    {
	call_update_screen = FALSE;
	// Invalidate cached background if screen size changed (e.g.
	// after window resize).
	ಆದರೆ (opacity_ಸಕ್ರಿಯ && pum_bg_ಸಾಲುಗಳು != ಖಾಲಿ
		&& (pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != screen_Columns
		    || pum_bg_bot > screen_ಅಡ್ಡಸಾಲುಗಳು))
	    pum_free_bg();

	ಆದರೆ (opacity_ಸಕ್ರಿಯ && pum_bg_ಸಾಲುಗಳು != ಖಾಲಿ)
	{
	    // Already have saved background; skip update_screen to avoid
	    // flickering.  Just do a normal pum_will_redraw update.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opacity_ಸಕ್ರಿಯ)
	{
	    // For opacity: draw background including the area under the
	    // pum, then save it.
	    pum_pretend_not_visible = TRUE;
	    update_screen(0);
	    pum_pretend_not_visible = FALSE;

	    // Save background to static buffers.
	    ಆದರೆ (Screen_ಸಾಲುಗಳು != ಖಾಲಿ && ScreenAttrs != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ	save_ತುದಿ, save_bot, save_ncells, k;

		pum_free_bg();
		save_ತುದಿ = pum_ಅಡ್ಡಸಾಲು - pum_border;
		save_bot = pum_ಅಡ್ಡಸಾಲು + pum_ಎತ್ತರ + pum_border
					    + (pum_shadow ? 1 : 0) + 1;
		ಆದರೆ (save_ತುದಿ < 0)
		    save_ತುದಿ = 0;
		ಆದರೆ (save_bot > screen_ಅಡ್ಡಸಾಲುಗಳು)
		    save_bot = screen_ಅಡ್ಡಸಾಲುಗಳು;
		pum_bg_ತುದಿ = save_ತುದಿ;
		pum_bg_bot = save_bot;
		pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen_Columns;
		ಆದರೆ (save_ತುದಿ < save_bot)
		{
		    save_ncells = (save_bot - save_ತುದಿ) * screen_Columns;
		    pum_bg_attrs = LALLOC_MULT(sattr_T, save_ncells);
		    pum_bg_ಸಾಲುಗಳು = LALLOC_MULT(schar_T, save_ncells);
		    ಆದರೆ (enc_utf8)
		    {
			pum_bg_ಸಾಲುಗಳು_UC = LALLOC_MULT(u8char_T, save_ncells);
			ಗಾಗಿ (k = 0; k < MAX_MCO; ++k)
			    pum_bg_ಸಾಲುಗಳು_C[k] = LALLOC_MULT(u8char_T,
							    save_ncells);
		    }
		    ಆದರೆ (pum_bg_attrs != ಖಾಲಿ && pum_bg_ಸಾಲುಗಳು != ಖಾಲಿ)
		    {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = save_ತುದಿ; r < save_bot; ++r)
			{
			    ಪೂರ್ಣಾಂಕ soff = (r - save_ತುದಿ) * screen_Columns;
			    ಪೂರ್ಣಾಂಕ loff = ಸಾಲು_ಸ್ಥಾನಾಂತರ[r];

			    mch_memmove(pum_bg_attrs + soff,
				    ScreenAttrs + loff,
				    screen_Columns * ಗಾತ್ರ(sattr_T));
			    mch_memmove(pum_bg_ಸಾಲುಗಳು + soff,
				    Screen_ಸಾಲುಗಳು + loff,
				    screen_Columns * ಗಾತ್ರ(schar_T));
			    ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ
						&& Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
			    {
				mch_memmove(pum_bg_ಸಾಲುಗಳು_UC + soff,
					Screen_ಸಾಲುಗಳು_UC + loff,
					screen_Columns * ಗಾತ್ರ(u8char_T));
				ಗಾಗಿ (k = 0; k < MAX_MCO; ++k)
				    ಆದರೆ (pum_bg_ಸಾಲುಗಳು_C[k] != ಖಾಲಿ
						    && Screen_ಸಾಲುಗಳು_C[k] != ಖಾಲಿ)
					mch_memmove(pum_bg_ಸಾಲುಗಳು_C[k] + soff,
						Screen_ಸಾಲುಗಳು_C[k] + loff,
						screen_Columns
						    * ಗಾತ್ರ(u8char_T));
			    }
			}
		    }
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Do not redraw in pum_may_redraw() and don't draw in the area
	    // where the popup menu will be.
	    pum_will_redraw = TRUE;
	    update_screen(0);
	    pum_will_redraw = FALSE;
	}
    }

    // never display more than we have
    pum_ಮೊದಲ = MIN(pum_ಮೊದಲ, scroll_range);

    ಆದರೆ (pum_scrollbar)
    {
	thumb_ಎತ್ತರ = pum_ಎತ್ತರ * pum_ಎತ್ತರ / pum_ಗಾತ್ರ;
	ಆದರೆ (thumb_ಎತ್ತರ == 0)
	    thumb_ಎತ್ತರ = 1;
	thumb_pos = (pum_ಮೊದಲ * (pum_ಎತ್ತರ - thumb_ಎತ್ತರ)
			    + scroll_range / 2) / scroll_range;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // The popup menu is drawn over popup menus with zindex under
    // POPUPMENU_ZINDEX.
    screen_zindex = POPUPMENU_ZINDEX;
#ಆದರೆ_ಅಂತ್ಯ

    // Draw border and shadow first if enabled, before setting blend
    // so that border/shadow characters are drawn without opacity.
    ಆದರೆ (pum_border)
	pum_draw_border();
    ಆದರೆ (pum_shadow)
	pum_draw_shadow();

    // Set blend for screen_puts_len / screen_fill to use.
    // Only the pum content area should be blended, not border/shadow.
    ಆದರೆ (opacity_ಸಕ್ರಿಯ)
	screen_pum_blend = 100 - (ಪೂರ್ಣಾಂಕ)p_po;

    ಗಾಗಿ (i = 0; i < pum_ಎತ್ತರ; ++i)
    {
	idx = i + pum_ಮೊದಲ;
	hlfs = (idx == pum_selected) ? hlfsSel : hlfsNorm;
	hlf = hlfs[0]; // start with "word" highlight
	attr = highlight_attr[hlf];

	// prepend a space if there is room
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	{
	    ಆದರೆ (pum_ಲಂಬಸಾಲು < curwin->w_wincol + curwin->w_ಅಗಲ - 1
							    - pum_border)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು + 1, attr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (pum_ಲಂಬಸಾಲು > pum_border)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, pum_ಲಂಬಸಾಲು - 1, attr);

	// Display each entry, use two spaces for a Tab.
	// Do this 3 times and order from p_cia
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು;
	totwidth = 0;
	pum_align_ಕ್ರಮ(ಕ್ರಮ);
	basic_ಅಗಲ = ವಸ್ತುಗಳು_ಅಗಲ_array[ಕ್ರಮ[0]];
	ಕೊನೆಯ_isabbr = ಕ್ರಮ[2] == CPT_ABBR;
	ಗಾಗಿ (j = 0; j < 3; ++j)
	{
	    ಅಂಶ_type = ಕ್ರಮ[j];
	    hlf = hlfs[ಅಂಶ_type];
	    attr = highlight_attr[hlf];
	    orig_attr = attr;
	    ಆದರೆ (ಅಂಶ_type < 2)  // try combine attr with user custom
		attr = pum_user_attr_combine(idx, ಅಂಶ_type, attr);
	    p = pum_get_ಅಂಶ(idx, ಅಂಶ_type);

	    ಆದರೆ (j + 1 < 3)
		ಮುಂದಿನ_isempty = pum_get_ಅಂಶ(idx, ಕ್ರಮ[j + 1]) == ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಮುಂದಿನ_isempty = TRUE;

	    ಆದರೆ (p != ಖಾಲಿ)
		// Process and display the item
		ಲಂಬಸಾಲು = pum_process_ಅಂಶ(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, idx, j, ಕ್ರಮ, hlf, attr,
						    &totwidth, ಮುಂದಿನ_isempty);

	    ಆದರೆ (j > 0)
		n = ವಸ್ತುಗಳು_ಅಗಲ_array[ಕ್ರಮ[1]] + (ಕೊನೆಯ_isabbr ? 0 : 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		n = ಕ್ರಮ[j] == CPT_ABBR ? 1 : 0;

	    // Stop when there is nothing more to display.
	    ಆದರೆ (j == 2
		    || (ಮುಂದಿನ_isempty && (j == 1 || (j == 0
				&& pum_get_ಅಂಶ(idx, ಕ್ರಮ[j + 2]) == ಖಾಲಿ)))
		    || basic_ಅಗಲ + n >= pum_ಅಗಲ)
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (pum_rl)
	    {
		screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, pum_ಲಂಬಸಾಲು - basic_ಅಗಲ - n + 1,
						ಲಂಬಸಾಲು + 1, ' ', ' ', orig_attr);
		ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು - basic_ಅಗಲ - n;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು, pum_ಲಂಬಸಾಲು + basic_ಅಗಲ + n,
							' ', ' ', orig_attr);
		ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು + basic_ಅಗಲ + n;
	    }
	    totwidth = basic_ಅಗಲ + n;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, pum_ಲಂಬಸಾಲು - pum_ಅಗಲ + 1, ಲಂಬಸಾಲು + 1,
							    ' ', ' ', orig_attr);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು, pum_ಲಂಬಸಾಲು + pum_ಅಗಲ, ' ', ' ',
								orig_attr);
	pum_draw_scrollbar(ಅಡ್ಡಸಾಲು, i, thumb_pos, thumb_ಎತ್ತರ);

	++ಅಡ್ಡಸಾಲು;
    }

    screen_pum_blend = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    screen_zindex = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();

    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_DISABLE);
}

#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * Position the info popup relative to the popup menu item.
 */
    ಶೂನ್ಯ
pum_position_info_popup(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು + pum_ಅಗಲ + pum_scrollbar + 1;
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ botpos = POPPOS_BOTLEFT;
    ಪೂರ್ಣಾಂಕ	ಬಳಸಿದ_maxwidth_opt = FALSE;
    ಪೂರ್ಣಾಂಕ	ಬಲ_margin, ಎಡ_margin, dummy;
    ಪೂರ್ಣಾಂಕ ಎಡ_extra_ಅಗಲ, ಬಲ_extra_ಅಗಲ;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &dummy);

    ಎಡ_extra_ಅಗಲ = pum_border + ಎಡ_margin;
    ಬಲ_extra_ಅಗಲ = pum_border + ಬಲ_margin + (pum_shadow ? 2 : 0);
    ಲಂಬಸಾಲು += ಬಲ_extra_ಅಗಲ;

    wp->w_popup_pos = POPPOS_TOPLEFT;
    ಆದರೆ (Columns - ಲಂಬಸಾಲು < 20 && Columns - ಲಂಬಸಾಲು < pum_ಲಂಬಸಾಲು)
    {
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು - 1 - ಎಡ_extra_ಅಗಲ;
	wp->w_popup_pos = POPPOS_TOPRIGHT;
	botpos = POPPOS_BOTRIGHT;
	wp->w_maxwidth = pum_ಲಂಬಸಾಲು - 1 - ಎಡ_extra_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wp->w_maxwidth = Columns - ಲಂಬಸಾಲು + 1;
    wp->w_maxwidth -= popup_extra_ಅಗಲ(wp);
    ಆದರೆ (wp->w_maxwidth_opt > 0 && wp->w_maxwidth > wp->w_maxwidth_opt)
    {
	// option value overrules computed value
	wp->w_maxwidth = wp->w_maxwidth_opt;
	ಬಳಸಿದ_maxwidth_opt = TRUE;
    }

    ಅಡ್ಡಸಾಲು -= popup_ತುದಿ_extra(wp);
    ಆದರೆ (wp->w_popup_flags & POPF_INFO_MENU)
    {
	ಆದರೆ (pum_ಅಡ್ಡಸಾಲು < pum_win_ಅಡ್ಡಸಾಲು)
	{
	    // menu above cursor line, align with bottom
	    ಅಡ್ಡಸಾಲು += pum_ಎತ್ತರ;
	    wp->w_popup_pos = botpos;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // menu below cursor line, align with top
	    ಅಡ್ಡಸಾಲು += 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// align with the selected item
	ಅಡ್ಡಸಾಲು += pum_selected - pum_ಮೊದಲ + 1;

    wp->w_popup_flags &= ~POPF_HIDDEN;
    ಆದರೆ (wp->w_maxwidth < 10 && !ಬಳಸಿದ_maxwidth_opt)
	// The popup is not going to fit or will overlap with the cursor
	// position, hide the popup.
	wp->w_popup_flags |= POPF_HIDDEN;
    ಇಲ್ಲದಿದ್ದರೆ
	popup_ಗಣ_wantpos_rowcol(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_ಗಣ_selected(ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ repeat ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    resized = FALSE;
    ಪೂರ್ಣಾಂಕ	    context = pum_ಎತ್ತರ / 2;
    ಪೂರ್ಣಾಂಕ	    scroll_ಸ್ಥಾನಾಂತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಪೂರ್ಣಾಂಕ	    prev_selected = pum_selected;
    ಚಿಹ್ನೆರಹಿತ	cur_cot_flags = get_cot_flags();
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ಪೂರ್ಣಾಂಕ	    has_info = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    pum_selected = n;
    scroll_ಸ್ಥಾನಾಂತರ = pum_selected - pum_ಎತ್ತರ;

    ಆದರೆ (pum_selected >= 0 && pum_selected < pum_ಗಾತ್ರ)
    {
	ಆದರೆ (pum_ಮೊದಲ > pum_selected - 4)
	{
	    // scroll down; when we did a jump it's probably a PageUp then
	    // scroll a whole page
	    ಆದರೆ (pum_ಮೊದಲ > pum_selected - 2)
	    {
		pum_ಮೊದಲ -= pum_ಎತ್ತರ - 2;
		ಆದರೆ (pum_ಮೊದಲ < 0)
		    pum_ಮೊದಲ = 0;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pum_ಮೊದಲ > pum_selected)
		    pum_ಮೊದಲ = pum_selected;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		pum_ಮೊದಲ = pum_selected;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pum_ಮೊದಲ < scroll_ಸ್ಥಾನಾಂತರ + 5)
	{
	    // scroll up; when we did a jump it's probably a PageDown then
	    // scroll a whole page
	    ಆದರೆ (pum_ಮೊದಲ < scroll_ಸ್ಥಾನಾಂತರ + 3)
		pum_ಮೊದಲ = MAX(pum_ಮೊದಲ + pum_ಎತ್ತರ - 2, scroll_ಸ್ಥಾನಾಂತರ + 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		pum_ಮೊದಲ = scroll_ಸ್ಥಾನಾಂತರ + 1;
	}

	// Give a few lines of context when possible.
	context = MIN(context, 3);
	ಆದರೆ (pum_ಎತ್ತರ > 2)
	{
	    ಆದರೆ (pum_ಮೊದಲ > pum_selected - context)
		pum_ಮೊದಲ = MAX(pum_selected - context, 0);  // scroll down
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pum_ಮೊದಲ < pum_selected + context - pum_ಎತ್ತರ + 1)
		pum_ಮೊದಲ = pum_selected + context - pum_ಎತ್ತರ + 1;  //ಮೇಲೆ
	}
	// adjust for the number of lines displayed
	pum_ಮೊದಲ = MIN(pum_ಮೊದಲ, pum_ಗಾತ್ರ - pum_ಎತ್ತರ);

#ಆದರೆ defined(FEAT_QUICKFIX)
	/*
	 * Show extra info in the preview window if there is something and
	 * 'completeopt' contains "preview" or "popup" or "popuphidden".
	 * Skip this when tried twice already.
	 * Skip this also when there is not much room.
	 * Skip this for command-window when 'completeopt' contains "preview".
	 * NOTE: Be very careful not to sync undo!
	 */
	ಆದರೆ (pum_array[pum_selected].pum_info != ಖಾಲಿ
		&& ಅಡ್ಡಸಾಲುಗಳು > 10
		&& repeat <= 1
		&& (cur_cot_flags & COT_ANY_PREVIEW)
		&& !((cur_cot_flags & COT_PREVIEW) && cmdwin_type != 0))
	{
	    win_T	*curwin_save = curwin;
	    tabpage_T   *curtab_save = curtab;
# ifdef FEAT_PROP_POPUP
	    use_popup_T	use_popup;
# ಇಲ್ಲದಿದ್ದರೆ
#  define use_popup USEPOPUP_NONE
# endif
# ifdef FEAT_PROP_POPUP
	    has_info = TRUE;
	    ಆದರೆ (cur_cot_flags & COT_POPUPHIDDEN)
		use_popup = USEPOPUP_HIDDEN;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_cot_flags & COT_POPUP)
		use_popup = USEPOPUP_ಸಾಮಾನ್ಯ;
	    ಇಲ್ಲದಿದ್ದರೆ
		use_popup = USEPOPUP_NONE;
	    ಆದರೆ (use_popup != USEPOPUP_NONE)
		// don't use WinEnter or WinLeave autocommands for the info
		// popup
		ಖಂಡ_autocmds();
# endif
	    // Open a preview window and set "curwin" to it.
	    // 3 lines by default, prefer 'previewheight' if set and smaller.
	    g_do_tagpreview = 3;
	    ಆದರೆ (p_pvh > 0 && p_pvh < g_do_tagpreview)
		g_do_tagpreview = p_pvh;
	    ++RedrawingDisabled;
	    // Prevent undo sync here, if an autocommand syncs undo weird
	    // things can happen to the undo tree.
	    ++no_u_sync;
	    resized = prepare_tagpreview(FALSE, FALSE, use_popup);
	    --no_u_sync;
	    ಆದರೆ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    g_do_tagpreview = 0;

	    ಆದರೆ (curwin->w_p_pvw
# ifdef FEAT_PROP_POPUP
		    || (curwin->w_popup_flags & POPF_INFO)
# endif
		    )
	    {
		ಪೂರ್ಣಾಂಕ	res = OK;

		ಆದರೆ (!resized
			&& curbuf->b_nwindows == 1
			&& curbuf->b_fname == ಖಾಲಿ
			&& bt_nofile(curbuf)
			&& curbuf->b_p_bh[0] == 'w')
		{
		    // Already a "wipeout" buffer, make it empty.
		    ಆಗ (!BUFEMPTY())
			ml_delete((linenr_T)1);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Don't want to sync undo in the current buffer.
		    ++no_u_sync;
		    res = do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE, 0, ಖಾಲಿ);
		    --no_u_sync;
		    ಆದರೆ (res == OK)
		    {
			// Edit a new, empty buffer. Set options for a "wipeout"
			// buffer.
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"swf",
							  0L, ಖಾಲಿ, OPT_LOCAL);
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bl",
							  0L, ಖಾಲಿ, OPT_LOCAL);
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bh",
					      0L, (char_u *)"wipe", OPT_LOCAL);
			ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"diff",
							  0L, ಖಾಲಿ, OPT_LOCAL);
		    }
		}
		ಆದರೆ (res == OK)
		{
		    char_u	*p, *e;
		    linenr_T	lnum = 0;

		    ಗಾಗಿ (p = pum_array[pum_selected].pum_info; *p != NUL; )
		    {
			e = vim_strchr(p, '\n');
			ಆದರೆ (e == ಖಾಲಿ)
			{
			    ml_append(lnum++, p, 0, FALSE);
			    ನಿಲ್ಲಿಸು;
			}
			*e = NUL;
			ml_append(lnum++, p, (ಪೂರ್ಣಾಂಕ)(e - p + 1), FALSE);
			*e = '\n';
			p = e + 1;
		    }
		    // delete the empty last line
		    ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

		    // Increase the height of the preview window to show the
		    // text, but no more than 'previewheight' lines.
		    ಆದರೆ (repeat == 0 && use_popup == USEPOPUP_NONE)
		    {
			lnum = MIN(lnum, p_pvh);
			ಆದರೆ (curwin->w_ಎತ್ತರ < lnum)
			{
			    win_setheight((ಪೂರ್ಣಾಂಕ)lnum);
			    resized = TRUE;
			}
		    }

		    curbuf->b_ಬದಲಾಯಿತು = 0;
		    curbuf->b_p_ma = FALSE;
		    ಆದರೆ (pum_selected != prev_selected)
		    {
# ifdef FEAT_PROP_POPUP
			curwin->w_firstline = 0;
# endif
			curwin->w_topline = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		    curwin->w_cursor.lnum = curwin->w_topline;
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
# ifdef FEAT_PROP_POPUP
		    ಆದರೆ (use_popup != USEPOPUP_NONE)
		    {
			pum_position_info_popup(curwin);
			ಆದರೆ (win_valid(curwin_save))
			    redraw_win_later(curwin_save, UPD_SOME_VALID);
		    }
# endif
		    ಆದರೆ ((curwin != curwin_save && win_valid(curwin_save))
			    || (curtab != curtab_save
						&& valid_tabpage(curtab_save)))
		    {
			ಪೂರ್ಣಾಂಕ save_redr_ಸ್ಥಿತಿ;

			ಆದರೆ (curtab != curtab_save && valid_tabpage(curtab_save))
			    goto_tabpage_tp(curtab_save, FALSE, FALSE);

			// When the first completion is done and the preview
			// window is not resized, skip the preview window's
			// status line redrawing.
			ಆದರೆ (ins_compl_ಸಕ್ರಿಯ() && !resized)
			    curwin->w_redr_ಸ್ಥಿತಿ = false;

			// Return cursor to where we were
			validate_cursor();
			redraw_later(UPD_SOME_VALID);

			// When the preview window was resized we need to
			// update the view on the buffer.  Only go back to
			// the window when needed, otherwise it will always be
			// redrawn.
			ಆದರೆ (resized && win_valid(curwin_save))
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
			save_redr_ಸ್ಥಿತಿ = curwin_save->w_redr_ಸ್ಥಿತಿ;
			curwin_save->w_redr_ಸ್ಥಿತಿ = false;
			update_screen(0);
			pum_pretend_not_visible = FALSE;
			pum_will_redraw = FALSE;
			curwin_save->w_redr_ಸ್ಥಿತಿ = save_redr_ಸ್ಥಿತಿ;

			ಆದರೆ (!resized && win_valid(curwin_save))
			{
# ifdef FEAT_PROP_POPUP
			    win_T *wp = curwin;
# endif
			    ++no_u_sync;
			    win_enter(curwin_save, TRUE);
			    --no_u_sync;
# ifdef FEAT_PROP_POPUP
			    ಆದರೆ (use_popup == USEPOPUP_HIDDEN && win_valid(wp))
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
# ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	    ಆದರೆ (WIN_IS_POPUP(curwin))
		// can't keep focus in a popup window
		win_enter(firstwin, TRUE);
# endif
# ifdef FEAT_PROP_POPUP
	    ಆದರೆ (use_popup != USEPOPUP_NONE)
		unblock_autocmds();
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    ಆದರೆ (!has_info)
	// hide any popup info window
	popup_hide_info();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ resized;
}

/*
 * Undisplay the popup menu (later).
 */
    ಶೂನ್ಯ
pum_undisplay(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_POPUP
    ಆದರೆ (pum_imagep != ಖಾಲಿ)
    {
	image_placement_free(pum_imagep);
	pum_imagep = ಖಾಲಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    pum_free_bg();
    pum_array = ಖಾಲಿ;
    redraw_all_later(UPD_NOT_VALID);
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (pum_ಒಳ_cmdline)
    {
	clear_cmdline = TRUE;
	pum_ಒಳ_cmdline = FALSE;
    }
    ಸ್ಥಿತಿ_redraw_all();
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    // hide any popup info window
    popup_hide_info();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Clear the popup menu.  Currently only resets the offset to the first
 * displayed item.
 */
    ಶೂನ್ಯ
pum_clear(ಶೂನ್ಯ)
{
    pum_ಮೊದಲ = 0;
}

/*
 * Return TRUE if the popup menu is displayed. Used to avoid some redrawing
 * that could overwrite it.  Overruled when "pum_pretend_not_visible" is set,
 * used to redraw the status lines.
 */
    ಪೂರ್ಣಾಂಕ
pum_visible(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ !pum_pretend_not_visible && pum_array != ಖಾಲಿ;
}

/*
 * Return TRUE if the popup can be redrawn in the same position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pum_ಒಳ_same_position(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು = (State & ಬಹುಲಕ_CMDLINE)
			? cmdline_ಅಡ್ಡಸಾಲು
			: curwin->w_wrow + W_WINROW(curwin);

    ಹಿಂತಿರುಗಿ pum_ಕಿಟಕಿ != curwin
	    || (pum_win_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲು
		&& pum_win_ಎತ್ತರ == curwin->w_ಎತ್ತರ
		&& pum_win_ಲಂಬಸಾಲು == curwin->w_wincol
		&& pum_win_ಅಗಲ == curwin->w_ಅಗಲ);
}

/*
 * Return TRUE when pum_may_redraw() will call pum_redraw().
 * This means that the pum area should not be overwritten to avoid flicker.
 */
    ಪೂರ್ಣಾಂಕ
pum_redraw_ಒಳ_same_position(ಶೂನ್ಯ)
{
    ಆದರೆ (!pum_visible() || pum_will_redraw)
	ಹಿಂತಿರುಗಿ FALSE;  // nothing to do

    ಹಿಂತಿರುಗಿ pum_ಒಳ_same_position();
}

/*
 * Reposition the popup menu to adjust for window layout changes.
 */
    ಶೂನ್ಯ
pum_may_redraw(ಶೂನ್ಯ)
{
    pumitem_T	*array = pum_array;
    ಪೂರ್ಣಾಂಕ		len = pum_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		selected = pum_selected;

    ಆದರೆ (!pum_visible() || pum_will_redraw)
	ಹಿಂತಿರುಗಿ;  // nothing to do

    ಆದರೆ (pum_ಒಳ_same_position())
    {
	pum_redraw();  // Redraw window in same position
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ
pum_get_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pum_ಎತ್ತರ;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Add size information about the pum to "dict".
 */
    ಶೂನ್ಯ
pum_ಗಣ_event_info(dict_T *dict)
{
    ಆದರೆ (!pum_visible())
	ಹಿಂತಿರುಗಿ;
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "height", pum_ಎತ್ತರ);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "width", pum_ಅಗಲ);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "row", pum_ಅಡ್ಡಸಾಲು);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", pum_ಲಂಬಸಾಲು);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "size", pum_ಗಾತ್ರ);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_bool(dict, "scrollbar",
				       pum_scrollbar ? VVAL_TRUE : VVAL_FALSE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_ಪದ) || defined(FEAT_ಪದ_POPUP_MENU)
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_position_at_mouse(ಪೂರ್ಣಾಂಕ min_ಅಗಲ)
{
    ಪೂರ್ಣಾಂಕ extra_ಎತ್ತರ = pum_border * 2 + pum_shadow;
    ಪೂರ್ಣಾಂಕ extra_ಸ್ಥಾನಾಂತರ = pum_border + pum_shadow;
    ಪೂರ್ಣಾಂಕ trailing_overhead = 0;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = 0;
    ಪೂರ್ಣಾಂಕ ಲಭ್ಯ_ಅಗಲ;

    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು - mouse_ಅಡ್ಡಸಾಲು - extra_ಎತ್ತರ > pum_ಗಾತ್ರ
	    || ಅಡ್ಡಸಾಲುಗಳು - mouse_ಅಡ್ಡಸಾಲು > mouse_ಅಡ್ಡಸಾಲು)
    {
	// Enough space below the mouse row,
	// or there is more space below the mouse row than above.
	pum_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು + 1 + pum_border;
	ಆದರೆ (pum_ಎತ್ತರ > ಅಡ್ಡಸಾಲುಗಳು - pum_ಅಡ್ಡಸಾಲು - extra_ಎತ್ತರ)
	    pum_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು - pum_ಅಡ್ಡಸಾಲು - extra_ಎತ್ತರ;
	ಆದರೆ (pum_ಅಡ್ಡಸಾಲು + pum_ಎತ್ತರ + extra_ಸ್ಥಾನಾಂತರ >= cmdline_ಅಡ್ಡಸಾಲು)
	    pum_ಒಳ_cmdline = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Show above the mouse row, reduce height if it does not fit.
	pum_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು - pum_ಗಾತ್ರ - extra_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (pum_ಅಡ್ಡಸಾಲು < pum_border)
	{
	    pum_ಎತ್ತರ += pum_ಅಡ್ಡಸಾಲು - pum_border;
	    pum_ಅಡ್ಡಸಾಲು = pum_border;
	}
    }

    trailing_overhead = pum_scrollbar + pum_border
	+ (pum_margin && pum_border ? 1 : 0) + (pum_shadow ? 2 : 0);

# ifdef FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = (mouse_ಲಂಬಸಾಲು == Columns - 1) && pum_border;
	ಲಭ್ಯ_ಅಗಲ = mouse_ಲಂಬಸಾಲು + 1 - trailing_overhead - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (ಲಭ್ಯ_ಅಗಲ >= pum_ತಳ_ಅಗಲ || ಲಭ್ಯ_ಅಗಲ > min_ಅಗಲ)
	    // Enough space to show at mouse column.
	    pum_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Not enough space, left align with window.
	    pum_ಲಂಬಸಾಲು = MIN(pum_ತಳ_ಅಗಲ, min_ಅಗಲ) - 1 + trailing_overhead;
	pum_ಅಗಲ = pum_ಲಂಬಸಾಲು + 1 - trailing_overhead;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ = (mouse_ಲಂಬಸಾಲು == 0) && pum_border;
	ಲಭ್ಯ_ಅಗಲ = Columns - mouse_ಲಂಬಸಾಲು - trailing_overhead - ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (ಲಭ್ಯ_ಅಗಲ >= pum_ತಳ_ಅಗಲ || ಲಭ್ಯ_ಅಗಲ > min_ಅಗಲ)
	    // Enough space to show at mouse column.
	    pum_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು + ಲಂಬಸಾಲು_ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Not enough space, right align with window.
	    pum_ಲಂಬಸಾಲು = Columns - MIN(pum_ತಳ_ಅಗಲ, min_ಅಗಲ)
		- trailing_overhead;
	pum_ಅಗಲ = Columns - pum_ಲಂಬಸಾಲು - trailing_overhead;
    }

    pum_ಅಗಲ = MIN(pum_ಅಗಲ, pum_ತಳ_ಅಗಲ + 1);
    // Do not redraw at cursor position.
    pum_ಕಿಟಕಿ = ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_ಪದ)
ಸ್ಥಿರ pumitem_T *balloon_array = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ balloon_arraysize;

# define BALLOON_MIN_ಅಗಲ 50
# define BALLOON_MIN_ಎತ್ತರ 10

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		bytelen;
    ಪೂರ್ಣಾಂಕ		cells;
    ಪೂರ್ಣಾಂಕ		indent;
} balpart_T;

/*
 * Split a string into parts to display in the balloon.
 * Aimed at output from gdb.  Attempts to split at white space, preserve quoted
 * strings and make a struct look good.
 * Resulting array is stored in "array" and returns the size of the array.
 */
    ಪೂರ್ಣಾಂಕ
ವಿಭಜಿಸು_ಸಂದೇಶ(char_u *mesg, pumitem_T **array)
{
    garray_T	ga;
    char_u	*p;
    balpart_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		quoted = FALSE;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_idx;
    ಪೂರ್ಣಾಂಕ		indent = 0;
    ಪೂರ್ಣಾಂಕ		max_cells = 0;
    ಪೂರ್ಣಾಂಕ		max_ಎತ್ತರ_2 = ಅಡ್ಡಸಾಲುಗಳು / 2 - 1;
    ಪೂರ್ಣಾಂಕ		long_ಅಂಶ_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು_long_ವಸ್ತುಗಳು = FALSE;

    ga_init2(&ga, ಗಾತ್ರ(balpart_T), 20);
    p = mesg;

    ಆಗ (*p != NUL)
    {
	ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    ಹೋಗು failed;
	ಅಂಶ = ((balpart_T *)ga.ga_ದತ್ತಾಂಶ) + ga.ga_len;
	ಅಂಶ->ಆರಂಭ = p;
	ಅಂಶ->indent = indent;
	ಅಂಶ->cells = indent * 2;
	++ga.ga_len;
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (*p == '"')
		quoted = !quoted;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\n')
		ನಿಲ್ಲಿಸು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\' && p[1] != NUL)
		++p;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!quoted)
	    {
		ಆದರೆ ((*p == ',' && p[1] == ' ') || *p == '{' || *p == '}')
		{
		    // Looks like a good point to break.
		    ಆದರೆ (*p == '{')
			++indent;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '}' && indent > 0)
			--indent;
		    ++ಅಂಶ->cells;
		    p = skipwhite(p + 1);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಅಂಶ->cells += ptr2cells(p);
	    p += mb_ptr2len(p);
	}
	ಅಂಶ->bytelen = p - ಅಂಶ->ಆರಂಭ;
	ಆದರೆ (*p == '\n')
	    ++p;
	ಆದರೆ (ಅಂಶ->cells > max_cells)
	    max_cells = ಅಂಶ->cells;
	long_ಅಂಶ_ಎಣಿಕೆ += (ಅಂಶ->cells - 1) / BALLOON_MIN_ಅಗಲ;
    }

    ಎತ್ತರ = 2 + ga.ga_len;

    // If there are long items and the height is below the limit: split lines
    ಆದರೆ (long_ಅಂಶ_ಎಣಿಕೆ > 0 && ಎತ್ತರ + long_ಅಂಶ_ಎಣಿಕೆ <= max_ಎತ್ತರ_2)
    {
	ವಿಭಜಿಸು_long_ವಸ್ತುಗಳು = TRUE;
	ಎತ್ತರ += long_ಅಂಶ_ಎಣಿಕೆ;
    }

    // Limit to half the window height, it has to fit above or below the mouse
    // position.
    ಆದರೆ (ಎತ್ತರ > max_ಎತ್ತರ_2)
	ಎತ್ತರ = max_ಎತ್ತರ_2;
    *array = ALLOC_CLEAR_MULT(pumitem_T, ಎತ್ತರ);
    ಆದರೆ (*array == ಖಾಲಿ)
	ಹೋಗು failed;

    // Add an empty line above and below, looks better.
    (*array)->pum_ಪಠ್ಯ = vim_strsave((char_u *)"");
    (*array + ಎತ್ತರ - 1)->pum_ಪಠ್ಯ = vim_strsave((char_u *)"");

    ಗಾಗಿ (ಸಾಲು = 1, ಅಂಶ_idx = 0; ಸಾಲು < ಎತ್ತರ - 1; ++ಅಂಶ_idx)
    {
	ಪೂರ್ಣಾಂಕ	skip;
	ಪೂರ್ಣಾಂಕ	thislen;
	ಪೂರ್ಣಾಂಕ	copylen;
	ಪೂರ್ಣಾಂಕ	ind;
	ಪೂರ್ಣಾಂಕ	cells;

	ಅಂಶ = ((balpart_T *)ga.ga_ದತ್ತಾಂಶ) + ಅಂಶ_idx;
	ಆದರೆ (ಅಂಶ->bytelen == 0)
	    (*array)[ಸಾಲು++].pum_ಪಠ್ಯ = vim_strsave((char_u *)"");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾಗಿ (skip = 0; skip < ಅಂಶ->bytelen; skip += thislen)
	    {
		ಆದರೆ (ವಿಭಜಿಸು_long_ವಸ್ತುಗಳು && ಅಂಶ->cells >= BALLOON_MIN_ಅಗಲ)
		{
		    cells = ಅಂಶ->indent * 2;
		    ಗಾಗಿ (p = ಅಂಶ->ಆರಂಭ + skip;
			    p < ಅಂಶ->ಆರಂಭ + ಅಂಶ->bytelen;
							    p += mb_ptr2len(p))
			ಆದರೆ ((cells += ptr2cells(p)) > BALLOON_MIN_ಅಗಲ)
			    ನಿಲ್ಲಿಸು;
		    thislen = p - (ಅಂಶ->ಆರಂಭ + skip);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    thislen = ಅಂಶ->bytelen;

		// put indent at the start
		p = alloc(thislen + ಅಂಶ->indent * 2 + 1);
		ಆದರೆ (p == ಖಾಲಿ)
		{
		    ಗಾಗಿ (ಸಾಲು = 0; ಸಾಲು <= ಎತ್ತರ - 1; ++ಸಾಲು)
			vim_free((*array)[ಸಾಲು].pum_ಪಠ್ಯ);
		    vim_free(*array);
		    ಹೋಗು failed;
		}
		ಗಾಗಿ (ind = 0; ind < ಅಂಶ->indent * 2; ++ind)
		    p[ind] = ' ';

		// exclude spaces at the end of the string
		ಗಾಗಿ (copylen = thislen; copylen > 0; --copylen)
		    ಆದರೆ (ಅಂಶ->ಆರಂಭ[skip + copylen - 1] != ' ')
			ನಿಲ್ಲಿಸು;

		vim_strncpy(p + ind, ಅಂಶ->ಆರಂಭ + skip, copylen);
		(*array)[ಸಾಲು].pum_ಪಠ್ಯ = p;
		ಅಂಶ->indent = 0;  // wrapped line has no indent
		++ಸಾಲು;
	    }
    }
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ ಎತ್ತರ;

failed:
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ 0;
}

    ಶೂನ್ಯ
ui_remove_balloon(ಶೂನ್ಯ)
{
    ಆದರೆ (balloon_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pum_undisplay();
    ಆಗ (balloon_arraysize > 0)
	vim_free(balloon_array[--balloon_arraysize].pum_ಪಠ್ಯ);
    VIM_CLEAR(balloon_array);
}

/*
 * Terminal version of a balloon, uses the popup menu code.
 */
    ಶೂನ್ಯ
ui_post_balloon(char_u *mesg, list_T *list)
{
    ui_remove_balloon();

    ಆದರೆ (mesg == ಖಾಲಿ && list == ಖಾಲಿ)
    {
	pum_undisplay();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (list != ಖಾಲಿ)
    {
	listitem_T  *li;
	ಪೂರ್ಣಾಂಕ	    idx;

	balloon_arraysize = list->lv_len;
	balloon_array = ALLOC_CLEAR_MULT(pumitem_T, list->lv_len);
	ಆದರೆ (balloon_array == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	CHECK_LIST_MATERIALIZE(list);
	ಗಾಗಿ (idx = 0, li = list->lv_ಮೊದಲ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ, ++idx)
	{
	    char_u *ಪಠ್ಯ = tv_get_string_chk(&li->li_tv);

	    balloon_array[idx].pum_ಪಠ್ಯ = vim_strsave(
					   ಪಠ್ಯ == ಖಾಲಿ ? (char_u *)"" : ಪಠ್ಯ);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	balloon_arraysize = ವಿಭಜಿಸು_ಸಂದೇಶ(mesg, &balloon_array);

    ಆದರೆ (balloon_arraysize <= 0)
	ಹಿಂತಿರುಗಿ;

    pum_array = balloon_array;
    pum_ಗಾತ್ರ = balloon_arraysize;
    pum_compute_ಗಾತ್ರ();
    pum_scrollbar = 0;
    pum_ಎತ್ತರ = balloon_arraysize;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif

    pum_position_at_mouse(BALLOON_MIN_ಅಗಲ);
    pum_selected = -1;
    pum_ಮೊದಲ = 0;
    pum_redraw();
}

/*
 * Called when the mouse moved, may remove any displayed balloon.
 */
    ಶೂನ್ಯ
ui_may_remove_balloon(ಶೂನ್ಯ)
{
    // For now: remove the balloon whenever the mouse moves to another screen
    // cell.
    ui_remove_balloon();
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಪದ_POPUP_MENU)
/*
 * Select the pum entry at the mouse position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_select_mouse_pos(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ idx = mouse_ಅಡ್ಡಸಾಲು - pum_ಅಡ್ಡಸಾಲು;

    ಆದರೆ (idx < 0 || idx >= pum_ಎತ್ತರ)
	pum_selected = -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*pum_array[idx].pum_ಪಠ್ಯ != NUL)
	pum_selected = idx;
}

/*
 * Execute the currently selected popup menu item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_execute_menu(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    vimmenu_T   *mp;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    exarg_T	ea;

    FOR_ALL_ಮಗು_MENUS(menu, mp)
	ಆದರೆ ((mp->modes & mp->enabled & ಬಹುಲಕ) && idx++ == pum_selected)
	{
	    CLEAR_ಕ್ಷೇತ್ರ(ea);
	    execute_menu(&ea, mp, -1);
	    ನಿಲ್ಲಿಸು;
	}
}

/*
 * Open the terminal version of the popup menu and don't return until it is
 * closed.
 */
    ಶೂನ್ಯ
pum_show_popupmenu(vimmenu_T *menu)
{
    vimmenu_T   *mp;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    pumitem_T	*array;
# ifdef FEAT_BEVAL_ಪದ
    ಪೂರ್ಣಾಂಕ		save_bevalterm = p_bevalterm;
# endif
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;

    pum_undisplay();
    pum_ಗಾತ್ರ = 0;
    ಬಹುಲಕ = get_menu_ಬಹುಲಕ_flag();

    FOR_ALL_ಮಗು_MENUS(menu, mp)
	ಆದರೆ (menu_is_separator(mp->dname)
		|| (mp->modes & mp->enabled & ಬಹುಲಕ))
	    ++pum_ಗಾತ್ರ;

    // When there are only Terminal mode menus, using "popup Edit" results in
    // pum_size being zero.
    ಆದರೆ (pum_ಗಾತ್ರ <= 0)
    {
	emsg(_(e_menu_only_exists_ಒಳ_another_ಬಹುಲಕ));
	ಹಿಂತಿರುಗಿ;
    }

    array = ALLOC_CLEAR_MULT(pumitem_T, pum_ಗಾತ್ರ);
    ಆದರೆ (array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_ಮಗು_MENUS(menu, mp)
    {
	char_u *s = ಖಾಲಿ;

	// Make a copy of the text, the menu may be redefined in a callback.
	ಆದರೆ (menu_is_separator(mp->dname))
	    s = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mp->modes & mp->enabled & ಬಹುಲಕ)
	    s = mp->dname;
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    s = vim_strsave(s);
	    ಆದರೆ (s != ಖಾಲಿ)
		array[idx++].pum_ಪಠ್ಯ = s;
	}
    }

    pum_array = array;
    pum_compute_ಗಾತ್ರ();
    pum_scrollbar = 0;
    pum_ಎತ್ತರ = pum_ಗಾತ್ರ;
# ifdef FEAT_RIGHTLEFT
    pum_rl = curwin->w_p_rl;
# endif
    pum_position_at_mouse(20);

    pum_selected = -1;
    pum_ಮೊದಲ = 0;
# ifdef FEAT_BEVAL_ಪದ
    p_bevalterm = TRUE;  // track mouse movement
    mch_setmouse(TRUE);
# endif

    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	c;

	pum_redraw();
	setcursor_mayforce(TRUE);
	ಹೊರಹೋಗು_flush();

	c = vgetc();

	// Bail out when typing Esc, CTRL-C or some callback or <expr> mapping
	// closed the popup menu.
	ಆದರೆ (c == ESC || c == Ctrl_C || pum_array == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CAR || c == NL)
	{
	    // enter: select current item, if any, and close
	    pum_execute_menu(menu, ಬಹುಲಕ);
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'k' || c == K_ಮೇಲೆ || c == K_MOUSEUP)
	{
	    // cursor up: select previous item
	    ಆಗ (pum_selected > 0)
	    {
		--pum_selected;
		ಆದರೆ (*array[pum_selected].pum_ಪಠ್ಯ != NUL)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'j' || c == K_ಕೆಳಕ್ಕೆ || c == K_MOUSEDOWN)
	{
	    // cursor down: select next item
	    ಆಗ (pum_selected < pum_ಗಾತ್ರ - 1)
	    {
		++pum_selected;
		ಆದರೆ (*array[pum_selected].pum_ಪಠ್ಯ != NUL)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_RIGHTMOUSE)
	{
	    // Right mouse down: reposition the menu.
	    vungetc(c);
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_LEFTDRAG || c == K_RIGHTDRAG || c == K_MOUSEMOVE)
	{
	    // mouse moved: select item in the mouse row
	    pum_select_mouse_pos();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM || c == K_RIGHTRELEASE)
	{
	    // left mouse click: select clicked item, if any, and close;
	    // right mouse release: select clicked item, close if any
	    pum_select_mouse_pos();
	    ಆದರೆ (pum_selected >= 0)
	    {
		pum_execute_menu(menu, ಬಹುಲಕ);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (c == K_LEFTMOUSE || c == K_LEFTMOUSE_NM)
		ನಿಲ್ಲಿಸು;
	}
    }

    ಗಾಗಿ (idx = 0; idx < pum_ಗಾತ್ರ; ++idx)
	vim_free(array[idx].pum_ಪಠ್ಯ);
    vim_free(array);
    pum_undisplay();
# ifdef FEAT_BEVAL_ಪದ
    p_bevalterm = save_bevalterm;
    mch_setmouse(TRUE);
# endif
}

    ಶೂನ್ಯ
pum_make_popup(char_u *ಮಾರ್ಗ_name, ಪೂರ್ಣಾಂಕ use_mouse_pos)
{
    vimmenu_T *menu;

    ಆದರೆ (!use_mouse_pos)
    {
	// Hack: set mouse position at the cursor so that the menu pops up
	// around there.
	mouse_ಅಡ್ಡಸಾಲು = W_WINROW(curwin) + curwin->w_wrow;
	mouse_ಲಂಬಸಾಲು =
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? W_ENDCOL(curwin) - curwin->w_wcol - 1 :
# endif
	    curwin->w_wincol + curwin->w_wcol;
    }

    menu = gui_ಕಂಡುಹಿಡಿ_menu(ಮಾರ್ಗ_name);
    ಆದರೆ (menu != ಖಾಲಿ)
	pum_show_popupmenu(menu);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_singlewidth(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c >= 0 && c < 0x80)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (utf_char2cells(c) == 1)
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set border characters for popup menu.
 */
    ಶೂನ್ಯ
pum_ಗಣ_border_chars(
	ಪೂರ್ಣಾಂಕ ತುದಿ,
	ಪೂರ್ಣಾಂಕ ಬಲ_2,
	ಪೂರ್ಣಾಂಕ bottom,
	ಪೂರ್ಣಾಂಕ ಎಡ_2,
	ಪೂರ್ಣಾಂಕ ತುದಿ_ಎಡ,
	ಪೂರ್ಣಾಂಕ ತುದಿ_ಬಲ,
	ಪೂರ್ಣಾಂಕ bottom_ಬಲ,
	ಪೂರ್ಣಾಂಕ bottom_ಎಡ)
{
    ಆದರೆ (!is_singlewidth(ತುದಿ) || !is_singlewidth(ಬಲ_2)
	    || !is_singlewidth(bottom) || !is_singlewidth(ಎಡ_2)
	    || !is_singlewidth(ತುದಿ_ಎಡ) || !is_singlewidth(ತುದಿ_ಬಲ)
	    || !is_singlewidth(bottom_ಬಲ) || !is_singlewidth(bottom_ಎಡ))
	ಹಿಂತಿರುಗಿ;
    pum_border_chars.ತುದಿ = ತುದಿ;
    pum_border_chars.ಬಲ_2 = ಬಲ_2;
    pum_border_chars.bottom = bottom;
    pum_border_chars.ಎಡ_2 = ಎಡ_2;
    pum_border_chars.ತುದಿ_ಎಡ = ತುದಿ_ಎಡ;
    pum_border_chars.ತುದಿ_ಬಲ = ತುದಿ_ಬಲ;
    pum_border_chars.bottom_ಬಲ = bottom_ಬಲ;
    pum_border_chars.bottom_ಎಡ = bottom_ಎಡ;
    pum_border = 1;
}

/*
 * Compute margins for the popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compute_margins(ಪೂರ್ಣಾಂಕ *ಬಲ_margin, ಪೂರ್ಣಾಂಕ *ಎಡ_margin, ಪೂರ್ಣಾಂಕ *ಎಡ_padding)
{
    *ಬಲ_margin = pum_margin && pum_border;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	*ಎಡ_padding = (pum_ಲಂಬಸಾಲು < Columns - 1 - pum_border);
	*ಎಡ_margin = *ಬಲ_margin && (pum_ಲಂಬಸಾಲು < Columns - 3);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    *ಎಡ_padding = (pum_ಲಂಬಸಾಲು > pum_border);
    *ಎಡ_margin = *ಬಲ_margin && (pum_ಲಂಬಸಾಲು > 2);
}

/*
 * Draw the border around the popup menu
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_border(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	ತುದಿ_ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು - 1;
    ಪೂರ್ಣಾಂಕ	bottom_ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು + pum_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	attr = highlight_attr[HLF_PMB];
    ಪೂರ್ಣಾಂಕ	ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;
    ಪೂರ್ಣಾಂಕ	inner_ಅಗಲ;

    ಆದರೆ (!pum_border)
	ಹಿಂತಿರುಗಿ;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);

    inner_ಅಗಲ = pum_ಅಗಲ + pum_scrollbar + ಎಡ_padding;

    // Draw border left to right or right to left (for RTL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು + ಎಡ_padding + 1;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು - ಎಡ_padding - 1;

    // left corners (right corners for RTL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಬಲ, ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	ಆದರೆ (ಎಡ_margin)
	    screen_putchar(' ', ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1, attr);
	screen_putchar(pum_border_chars.bottom_ಬಲ, bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	ಆದರೆ (ಎಡ_margin)
	    screen_putchar(' ', bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1, attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಎಡ, ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	ಆದರೆ (ಎಡ_margin)
	    screen_putchar(' ', ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1, attr);
	screen_putchar(pum_border_chars.bottom_ಎಡ, bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	ಆದರೆ (ಎಡ_margin)
	    screen_putchar(' ', bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1, attr);
    }

    // Top horizontal lines
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < inner_ಅಗಲ; i++)
    {
	ಪೂರ್ಣಾಂಕ cur_ಲಂಬಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    cur_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1 - i;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    cur_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + 1 + i;
	screen_putchar(pum_border_chars.ತುದಿ, ತುದಿ_ಅಡ್ಡಸಾಲು, cur_ಲಂಬಸಾಲು, attr);
	screen_putchar(pum_border_chars.bottom, bottom_ಅಡ್ಡಸಾಲು, cur_ಲಂಬಸಾಲು, attr);
    }

    // right corners (left corners for RTL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಎಡ, ತುದಿ_ಅಡ್ಡಸಾಲು,
		ಲಂಬಸಾಲು - inner_ಅಗಲ - 1, attr);
	ಆದರೆ (ಬಲ_margin)
	    screen_putchar(' ', ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - inner_ಅಗಲ - 2, attr);
	screen_putchar(pum_border_chars.bottom_ಎಡ, bottom_ಅಡ್ಡಸಾಲು,
		ಲಂಬಸಾಲು - inner_ಅಗಲ - 1, attr);
	ಆದರೆ (ಬಲ_margin)
	    screen_putchar(' ', bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - inner_ಅಗಲ - 2, attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	screen_putchar(pum_border_chars.ತುದಿ_ಬಲ, ತುದಿ_ಅಡ್ಡಸಾಲು,
		ಲಂಬಸಾಲು + inner_ಅಗಲ + 1, attr);
	ಆದರೆ (ಬಲ_margin)
	    screen_putchar(' ', ತುದಿ_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + inner_ಅಗಲ + 2, attr);
	screen_putchar(pum_border_chars.bottom_ಬಲ, bottom_ಅಡ್ಡಸಾಲು,
		ಲಂಬಸಾಲು + inner_ಅಗಲ + 1, attr);
	ಆದರೆ (ಬಲ_margin)
	    screen_putchar(' ', bottom_ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + inner_ಅಗಲ + 2, attr);
    }

    // Draw side borders
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pum_ಎತ್ತರ; i++)
    {
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು + i;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	{
	    screen_putchar(pum_border_chars.ಎಡ_2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	    ಆದರೆ (ಎಡ_margin)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1, attr);
	    screen_putchar(pum_border_chars.ಬಲ_2, ಅಡ್ಡಸಾಲು,
		    ಲಂಬಸಾಲು - inner_ಅಗಲ - 1, attr);
	    ಆದರೆ (ಬಲ_margin)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - inner_ಅಗಲ - 2, attr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    screen_putchar(pum_border_chars.ಎಡ_2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
	    ಆದರೆ (ಎಡ_margin)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1, attr);
	    screen_putchar(pum_border_chars.ಬಲ_2, ಅಡ್ಡಸಾಲು,
		    ಲಂಬಸಾಲು + inner_ಅಗಲ + 1, attr);
	    ಆದರೆ (ಬಲ_margin)
		screen_putchar(' ', ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + inner_ಅಗಲ + 2, attr);
	}
    }
}

/*
 * Get the underlying character and redraw with shadow highlight.
 * Preserve bold, italic, underline, and reverse text underneath the shadow.
 */
    ಶೂನ್ಯ
put_shadow_char(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    char_u  buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ	    shadow_attr = highlight_attr[HLF_PMS];
    ಪೂರ್ಣಾಂಕ	    char_attr, new_attr;

    screen_getbytes(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, buf, &char_attr);
    new_attr = hl_combine_attr(char_attr, shadow_attr);
    screen_putchar((*mb_ptr2char)(buf), ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, new_attr);
}

/*
 * Draw the shadow
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pum_draw_shadow(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಮುಂದಿನ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	ಅಗಲ, ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ	inner_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	ಬಲ_margin, ಎಡ_margin, ಎಡ_padding;

    ಆದರೆ (!pum_shadow)
	ಹಿಂತಿರುಗಿ;

    (ಶೂನ್ಯ)compute_margins(&ಬಲ_margin, &ಎಡ_margin, &ಎಡ_padding);
    inner_ಅಗಲ = pum_ಅಗಲ + pum_scrollbar + pum_border + ಬಲ_margin;

    // Draw right side shadow (left for RTL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (pum_rl)
    {
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು - inner_ಅಗಲ;
	ಮುಂದಿನ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು + inner_ಅಗಲ;
	ಮುಂದಿನ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + 1;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < pum_ಎತ್ತರ + pum_border; i++)
    {
	ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು + 1 + i - pum_border;
	put_shadow_char(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	put_shadow_char(ಅಡ್ಡಸಾಲು, ಮುಂದಿನ_ಲಂಬಸಾಲು);
    }

    // Bottom shadow
    ಅಡ್ಡಸಾಲು = pum_ಅಡ್ಡಸಾಲು + pum_ಎತ್ತರ + pum_border;
    ಅಗಲ = inner_ಅಗಲ + ಎಡ_padding + pum_border + ಎಡ_margin;
    ಸ್ಥಾನಾಂತರ = 2 - ಎಡ_padding - pum_border - ಎಡ_margin;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (pum_rl)
	    ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು - ಸ್ಥಾನಾಂತರ - i;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಲಂಬಸಾಲು = pum_ಲಂಬಸಾಲು + ಸ್ಥಾನಾಂತರ + i;
	put_shadow_char(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    }
}
