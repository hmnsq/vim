/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * screen.c: Lower level code for displaying on the screen.
 *
 * Output to the screen (console, terminal emulator or GUI window) is minimized
 * by remembering what is already on the screen, and only updating the parts
 * that changed.
 *
 * ScreenLines[off]  Contains a copy of the whole screen, as it is currently
 *		     displayed (excluding text written by external commands).
 * ScreenAttrs[off]  Contains the associated attributes.
 * ScreenCols[off]   Contains the virtual columns in the line. -1 means not
 *		     available or before buffer text.
 *
 * LineOffset[row]   Contains the offset into ScreenLines*[], ScreenAttrs[]
 *		     and ScreenCols[] for each line.
 * LineWraps[row]    Flag for each line whether it wraps to the next line.
 *
 * For double-byte characters, two consecutive bytes in ScreenLines[] can form
 * one character which occupies two display cells.
 * For UTF-8 a multi-byte character is converted to Unicode and stored in
 * ScreenLinesUC[].  ScreenLines[] contains the first byte only.  For an ASCII
 * character without composing chars ScreenLinesUC[] will be 0 and
 * ScreenLinesC[][] is not used.  When the character occupies two display
 * cells the next byte in ScreenLines[] is 0.
 * ScreenLinesC[][] contain up to 'maxcombine' composing characters
 * (drawn on top of the first character).  There is 0 after the last one used.
 * ScreenLines2[] is only used for euc-jp to store the second byte if the
 * first byte is 0x8e (single-width character).
 *
 * The screen_*() functions write to the screen and handle updating
 * ScreenLines[].
 */

#include "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
static puur_naa_nka	screen_attr = 0;

static void screen_char_2(unsigned off, puur_naa_nka row, puur_naa_nka col);
static puur_naa_nka screenclear2(puur_naa_nka doclear);
static void lineclear(unsigned off, puur_naa_nka width, puur_naa_nka attr);
static void lineinvalid(unsigned off, puur_naa_nka width);
static puur_naa_nka win_do_lines(win_T *wp, puur_naa_nka row, puur_naa_nka line_count, puur_naa_nka mayclear, puur_naa_nka del, puur_naa_nka clear_attr);
static void win_rest_invalid(win_T *wp);
static void msg_pos_mode(void);
static void recording_mode(puur_naa_nka attr);

// Ugly global: overrule attribute used by screen_char()
static puur_naa_nka screen_char_attr = 0;

#ᮚᮦᮔ᮪ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    puur_naa_nka
conceal_cursor_line(win_T *wp)
{
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (*wp->w_p_cocu == NUL)
	ᮝᮜᮤ FALSE;
    ᮚᮦᮔ᮪ (get_real_state() & MODE_VISUAL)
	c = 'v';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State & MODE_INSERT)
	c = 'i';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State & MODE_NORMAL)
	c = 'n';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State & MODE_CMDLINE)
	c = 'c';
    ᮜᮤᮚ
	ᮝᮜᮤ FALSE;
    ᮝᮜᮤ vim_strchr(wp->w_p_cocu, c) != NULL;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    void
conceal_check_cursor_line(puur_naa_nka was_concealed)
{
    ᮚᮦᮔ᮪ (curwin->w_p_cole <= 0 || conceal_cursor_line(curwin) == was_concealed)
	ᮝᮜᮤ;

    puur_naa_nka wcol = curwin->w_wcol;

    need_cursor_line_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ᮚᮦᮔ᮪ (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    puur_naa_nka
get_win_attr(win_T *wp)
{
    puur_naa_nka	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ᮚᮦᮔ᮪ (win_attr != 0)
    {
	ᮚᮦᮔ᮪ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	ᮜᮤᮚ
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    ᮜᮤᮚ ᮚᮦᮔ᮪ (WIN_IS_POPUP(wp))
    {
	ᮚᮦᮔ᮪ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	ᮜᮤᮚ
	    win_attr = HL_ATTR(HLF_POP);    // Popup
    }
#endif

    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();
    ᮝᮜᮤ win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    static puur_naa_nka
screen_fill_end(
	win_T *wp,
	puur_naa_nka	c1,
	puur_naa_nka	c2,
	puur_naa_nka	off,
	puur_naa_nka	width,
	puur_naa_nka	row,
	puur_naa_nka	endrow,
	puur_naa_nka	attr)
{
    puur_naa_nka	    nn = off + width;

    ᮚᮦᮔ᮪ (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    ᮚᮦᮔ᮪ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (puur_naa_nka)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    ᮜᮤᮚ
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (puur_naa_nka)wp->w_wincol + nn,
		c1, c2, attr);
    ᮝᮜᮤ nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    void
win_draw_end(
    win_T	*wp,
    puur_naa_nka		c1,
    puur_naa_nka		c2,
    puur_naa_nka		draw_margin,
    puur_naa_nka		row,
    puur_naa_nka		endrow,
    hlf_T	hl)
{
    puur_naa_nka		n = 0;
    puur_naa_nka		attr = HL_ATTR(hl);
    puur_naa_nka		win_attr = get_win_attr(wp);
    bool	override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ᮚᮦᮔ᮪ (draw_margin)
    {
#ifdef FEAT_FOLDING
	puur_naa_nka	fdc = compute_foldcolumn(wp, 0);

	ᮚᮦᮔ᮪ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	ᮚᮦᮔ᮪ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	ᮚᮦᮔ᮪ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == NULL)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, number_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    ᮚᮦᮔ᮪ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    ᮜᮤᮚ
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (puur_naa_nka)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();
}

#ᮚᮦᮔ᮪ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    puur_naa_nka
compute_foldcolumn(win_T *wp, puur_naa_nka col)
{
    puur_naa_nka wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    puur_naa_nka n = wp->w_width - (col + wmw);

    ᮝᮜᮤ MIN(wp->w_p_fdc, n);
}

/*
 * Fill the foldcolumn at "p" for window "wp".
 * Only to be called when 'foldcolumn' > 0.
 * Returns the number of bytes stored in 'p'. When non-multibyte characters are
 * used for the fold column markers, this is equal to 'fdc' setting. Otherwise,
 * this will be greater than 'fdc'.
 */
    size_t
fill_foldcolumn(
    char_u	*p,
    win_T	*wp,
    puur_naa_nka		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    puur_naa_nka		i = 0;
    puur_naa_nka		level;
    puur_naa_nka		first_level;
    puur_naa_nka		empty;
    puur_naa_nka		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    puur_naa_nka		symbol = 0;
    puur_naa_nka		len = 0;
    puur_naa_nka		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    ᮚᮦᮔ᮪ (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    for (i = 0; i < n; i++)
    {
	ᮚᮦᮔ᮪ (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_low_level)
	    symbol = wp->w_fill_chars.foldopen;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (first_level == 1)
	    symbol = wp->w_fill_chars.foldsep;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_fill_chars.foldinner != NUL)
	    symbol = wp->w_fill_chars.foldinner;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (first_level + i <= 9)
	    symbol = '0' + first_level + i;
	ᮜᮤᮚ
	    symbol = '>';

	len = utf_char2bytes(symbol, &p[byte_counter]);
	byte_counter += len;
	ᮚᮦᮔ᮪ (first_level + i >= level)
	{
	    i++;
	    bha_nga;
	}
    }

    ᮚᮦᮔ᮪ (closed)
    {
	ᮚᮦᮔ᮪ (symbol != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ᮚᮦᮔ᮪ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	symbol = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(symbol, &p[byte_counter]);
	byte_counter += len;
    }

    ᮝᮜᮤ MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    static puur_naa_nka
comp_char_differs(puur_naa_nka off_from, puur_naa_nka off_to)
{
    puur_naa_nka	    i;

    for (i = 0; i < Screen_mco; ++i)
    {
	ᮚᮦᮔ᮪ (ScreenLinesC[i][off_from] != ScreenLinesC[i][off_to])
	    ᮝᮜᮤ TRUE;
	ᮚᮦᮔ᮪ (ScreenLinesC[i][off_from] == 0)
	    bha_nga;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    static puur_naa_nka
char_needs_redraw(puur_naa_nka off_from, puur_naa_nka off_to, puur_naa_nka cols)
{
    ᮚᮦᮔ᮪ (cols > 0
	    && ((ScreenLines[off_from] != ScreenLines[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(ScreenLines[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && ScreenLines[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && ScreenLines[off_from + 1]
						 != ScreenLines[off_to + 1])))
		|| (enc_utf8
		    && (ScreenLinesUC[off_from] != ScreenLinesUC[off_to]
			|| (ScreenLinesUC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && ScreenLines[off_from + 1]
						!= ScreenLines[off_to + 1])))))
	ᮝᮜᮤ TRUE;
    ᮝᮜᮤ FALSE;
}

#ᮚᮦᮔ᮪ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    puur_naa_nka
screen_get_current_line_off(void)
{
    ᮝᮜᮤ (puur_naa_nka)(current_ScreenLine - ScreenLines);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    static puur_naa_nka
blocked_by_popup(puur_naa_nka row, puur_naa_nka col)
{
    puur_naa_nka off;

    ᮚᮦᮔ᮪ (!popup_visible)
	ᮝᮜᮤ FALSE;
    off = row * screen_Columns + col;
    ᮝᮜᮤ popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    void
reset_screen_attr(void)
{
#ifdef FEAT_GUI
    ᮚᮦᮔ᮪ (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    ᮜᮤᮚ
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    static puur_naa_nka
skip_for_popup(puur_naa_nka row, puur_naa_nka col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ᮚᮦᮔ᮪ (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	ᮝᮜᮤ TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    ᮚᮦᮔ᮪ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	ᮝᮜᮤ TRUE;
#ᮜᮤᮚ
    ᮚᮦᮔ᮪ (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	ᮝᮜᮤ TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    ᮚᮦᮔ᮪ (blocked_by_popup(row, col))
	ᮝᮜᮤ TRUE;
#endif
    ᮝᮜᮤ FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
static char_u	*suppressed_cells = NULL;
static puur_naa_nka	suppressed_rows = 0;
static puur_naa_nka	suppressed_cols = 0;

    static void
mark_suppressed_cell(puur_naa_nka row, puur_naa_nka col)
{
    ᮚᮦᮔ᮪ (suppressed_cells == NULL
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	ᮚᮦᮔ᮪ (suppressed_cells == NULL)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    ᮝᮜᮤ;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    static void
unmark_suppressed_cell(puur_naa_nka row, puur_naa_nka col)
{
    ᮚᮦᮔ᮪ (suppressed_cells != NULL
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    static puur_naa_nka
is_suppressed_cell(puur_naa_nka row, puur_naa_nka col)
{
    ᮝᮜᮤ suppressed_cells != NULL
	&& row >= 0 && row < suppressed_rows
	&& col >= 0 && col < suppressed_cols
	&& suppressed_cells[row * suppressed_cols + col];
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
typedef sa_nracanaa {
    puur_naa_nka		popup_attr;	// get_win_attr(screen_opacity_popup)
    puur_naa_nka		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    static anta_hstha void
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    static anta_hstha puur_naa_nka
popup_base_attr_or(puur_naa_nka row, puur_naa_nka col, puur_naa_nka fallback)
{
    puur_naa_nka		base = fallback;

    popup_get_base_screen_cell(row, col, NULL, &base, NULL);
    ᮝᮜᮤ base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    static anta_hstha puur_naa_nka
popup_blend_with_base(
	const popup_opacity_T	*po,
	puur_naa_nka			char_attr,
	puur_naa_nka			base_attr,
	puur_naa_nka			blend_fg)
{
    puur_naa_nka		combined = hl_combine_attr(po->popup_attr, char_attr);

    ᮝᮜᮤ hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    static void
resolve_wide_char_opacity_attrs(
	puur_naa_nka row, puur_naa_nka col1, puur_naa_nka col2,
	sattr_T *attr1, sattr_T *attr2)
{
    puur_naa_nka		bg1, bg2;
    puur_naa_nka		base1 = 0;
    puur_naa_nka		base2 = 0;
    attrentry_T	*ae;

    ᮚᮦᮔ᮪ (*attr1 == *attr2)
	ᮝᮜᮤ;

    popup_get_base_screen_cell(row, col1, NULL, &base1, NULL);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != NULL && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ᮜᮤᮚ
    bg1 = (ae != NULL && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, NULL, &base2, NULL);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != NULL && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ᮜᮤᮚ
    bg2 = (ae != NULL && ae->ae_u.cterm.bg_color != 0);
# endif

    ᮚᮦᮔ᮪ (bg1 && !bg2)
	*attr1 = *attr2;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (!bg1 && bg2)
	*attr2 = *attr1;
}
#endif

/*
 * Move one "cooked" screen line to the screen, but only the characters that
 * have actually changed.  Handle insert/delete character.
 * "coloff" gives the first column on the screen for this line.
 * "endcol" gives the columns where valid characters are.
 * "clear_width" is the width of the window.  It's > 0 if the rest of the line
 * needs to be cleared, negative otherwise.
 * "flags" can have bits:
 * SLF_POPUP	    popup window
 * SLF_RIGHTLEFT    rightleft window:
 *    When TRUE and "clear_width" > 0, clear columns 0 to "endcol"
 *    When FALSE and "clear_width" > 0, clear columns "endcol" to "clear_width"
 * SLF_INC_VCOL:
 *    When FALSE, use "last_vcol" for ScreenCols[] of the columns to clear.
 *    When TRUE, use an increasing sequence starting from "last_vcol + 1" for
 *    ScreenCols[] of the columns to clear.
 */
    void
screen_line(
	win_T	*wp,
	puur_naa_nka	row,
	puur_naa_nka	coloff,
	puur_naa_nka	endcol,
	puur_naa_nka	clear_width,
	colnr_T	last_vcol,
	puur_naa_nka	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    puur_naa_nka		    col = 0;
    puur_naa_nka		    hl;
    puur_naa_nka		    force = FALSE;	// force update rest of the line
    puur_naa_nka		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    puur_naa_nka		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    puur_naa_nka		    changed_this;	// TRUE if character changed
    puur_naa_nka		    changed_next;	// TRUE if next character changed
#endif
    puur_naa_nka		    clear_next = FALSE;
    puur_naa_nka		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    puur_naa_nka		    drawing_opacity_popup =
				screen_opacity_popup != NULL
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    ᮚᮦᮔ᮪ (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    ᮚᮦᮔ᮪ (row >= Rows)
	row = Rows - 1;
    ᮚᮦᮔ᮪ (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_ScreenLine - ScreenLines);
    off_to = LineOffset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = LineOffset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    ᮚᮦᮔ᮪ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ᮚᮦᮔ᮪ (clear_width > 0)
	{
	    puur_naa_nka clear_start = col;

	    ᮞᮨᮓᮨᮜ (col <= endcol && ScreenLines[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || ScreenLinesUC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    ᮚᮦᮔ᮪ (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    for (puur_naa_nka i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = LineOffset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    ᮚᮦᮔ᮪ (coloff > 0 && enc_utf8
		   && ScreenLines[off_to] == 0
		   && ScreenLinesUC[off_to - 1] != 0
		   && (*mb_char2cells)(ScreenLinesUC[off_to - 1]) > 1)
    {
	ScreenLines[off_to - 1] = ' ';
	ScreenLinesUC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	ᮚᮦᮔ᮪ (screen_opacity_popup == NULL)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    ᮞᮨᮓᮨᮜ (col < endcol)
    {
	ᮚᮦᮔ᮪ (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	ᮜᮤᮚ
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ᮚᮦᮔ᮪ (!redraw_this && is_suppressed_cell(row, col + coloff))
	    redraw_this = TRUE;
#endif

#ifdef FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	changed_this = changed_next;
	changed_next = redraw_next;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	ᮚᮦᮔ᮪ (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    ᮚᮦᮔ᮪ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ᮚᮦᮔ᮪ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	ᮚᮦᮔ᮪ (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ᮚᮦᮔ᮪ (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	puur_naa_nka opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ᮚᮦᮔ᮪ (drawing_opacity_popup
		&& ScreenLines[off_from] == ' '
		&& (!enc_utf8 || ScreenLinesUC[off_from] == 0))
	{
	    puur_naa_nka popup_src_attr = ScreenAttrs[off_from];
	    puur_naa_nka bg_is_empty = ScreenLines[off_to] == 0
				&& (!enc_utf8 || ScreenLinesUC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ᮚᮦᮔ᮪ (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && ScreenLinesUC[off_to - 1] != 0
		    && utf_char2cells(ScreenLinesUC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (ScreenLines[off_to] != 0
		    || (enc_utf8 && ScreenLinesUC[off_to] != 0))
	    {
		puur_naa_nka bg_char_cells = (enc_utf8 && ScreenLinesUC[off_to] != 0)
				? utf_char2cells(ScreenLinesUC[off_to]) : 1;

		ᮚᮦᮔ᮪ (bg_char_cells == 2)
		{
		    ᮚᮦᮔ᮪ (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			puur_naa_nka base_attr;
			ScreenLines[off_to] = ' ';
			ᮚᮦᮔ᮪ (enc_utf8)
			    ScreenLinesUC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			goto skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    ᮚᮦᮔ᮪ (!(ScreenLines[off_from + 1] == ' '
			    && (!enc_utf8 || ScreenLinesUC[off_from + 1] == 0)))
			goto skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		puur_naa_nka base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ᮚᮦᮔ᮪ (bg_char_cells == 2)
		{
		    puur_naa_nka base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    ᮚᮦᮔ᮪ (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (bg_is_empty)
	    {
		puur_naa_nka base_attr;
		ScreenLines[off_to] = ' ';
		base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#endif

	ᮚᮦᮔ᮪ (redraw_this)
	{
	    /*
	     * Special handling when 'xs' termcap flag set (hpterm):
	     * Attributes for characters are stored at the position where the
	     * cursor is when writing the highlighting code.  The
	     * start-highlighting code must be written with the cursor on the
	     * first highlighted character.  The stop-highlighting code must
	     * be written with the cursor just after the last highlighted
	     * character.
	     * Overwriting a character doesn't remove its highlighting.  Need
	     * to clear the rest of the line, and force redrawing it
	     * completely.
	     */
	    ᮚᮦᮔ᮪ (       p_wiv
		    && !force
#ifdef FEAT_GUI
		    && !gui.in_use
#endif
		    && ScreenAttrs[off_to] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_to])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(row, col + coloff);
		out_str(T_CE);		// clear rest of this screen line
		screen_start();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_next = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		ᮚᮦᮔ᮪ (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		ᮜᮤᮚ
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ᮚᮦᮔ᮪ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ᮚᮦᮔ᮪ (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    ScreenLines[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    ScreenLines[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ᮚᮦᮔ᮪ (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    ScreenLines[off_to] = ScreenLines[off_from];
	    ᮚᮦᮔ᮪ (enc_utf8)
	    {
		ScreenLinesUC[off_to] = ScreenLinesUC[off_from];
		ᮚᮦᮔ᮪ (ScreenLinesUC[off_from] != 0)
		{
		    puur_naa_nka	    i;

		    for (i = 0; i < Screen_mco; ++i)
			ScreenLinesC[i][off_to] = ScreenLinesC[i][off_from];
		}
	    }
	    ᮚᮦᮔ᮪ (char_cells == 2)
	    {
		ScreenLines[off_to + 1] = ScreenLines[off_from + 1];
		ᮚᮦᮔ᮪ (enc_utf8)
		    ScreenLinesUC[off_to + 1] = 0;
	    }

#ᮚᮦᮔ᮪ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ᮚᮦᮔ᮪ (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ᮚᮦᮔ᮪ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		ᮚᮦᮔ᮪ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ᮚᮦᮔ᮪ (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ᮚᮦᮔ᮪ (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ᮚᮦᮔ᮪ (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ᮚᮦᮔ᮪ (drawing_opacity_popup && po.blend > 0)
	    {
		puur_naa_nka popup_src_attr = ScreenAttrs[off_from];
		puur_naa_nka scol1 = col + coloff;
		puur_naa_nka base1 = popup_base_attr_or(row, scol1, 0);

		ᮚᮦᮔ᮪ (char_cells == 2)
		{
		    puur_naa_nka base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    puur_naa_nka over1 = popup_is_over_opacity(row, scol1);
		    puur_naa_nka over2 = popup_is_over_opacity(row, scol1 + 1);
		    ᮚᮦᮔ᮪ (over1 != over2)
		    {
			ᮚᮦᮔ᮪ (over1)
			    base1 = base2;
			ᮜᮤᮚ
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    ᮜᮤᮚ
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ᮚᮦᮔ᮪ (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    ᮚᮦᮔ᮪ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    ᮜᮤᮚ
		screen_char(off_to, row, col + coloff);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    ᮚᮦᮔ᮪ (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	ᮚᮦᮔ᮪ (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    ᮚᮦᮔ᮪ (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    ᮚᮦᮔ᮪ (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	ScreenLines[off_to] = ' ';
	ᮚᮦᮔ᮪ (enc_utf8)
	    ScreenLinesUC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	ᮚᮦᮔ᮪ (screen_opacity_popup == NULL)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    ᮚᮦᮔ᮪ (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	puur_naa_nka startCol = col;
#endif

	// blank out the rest of the line
	ᮞᮨᮓᮨᮜ (col < clear_width && ScreenLines[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || ScreenLinesUC[off_to] == 0)
#ifdef FEAT_PROP_POPUP
				  && !is_suppressed_cell(row, col + coloff)
#endif
				  )
	{
	    ScreenCols[off_to] =
			      (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	    ++off_to;
	    ++col;
	}
	ᮚᮦᮔ᮪ (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ᮚᮦᮔ᮪ (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		ᮚᮦᮔ᮪ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    puur_naa_nka prev_cells = 1;

		    ᮚᮦᮔ᮪ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = ScreenLines[off_to - 1] == 0 ? 2 : 1;
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = LineOffset[row];
			unsigned max_off = LineOffset[row] + screen_Columns;

			ᮞᮨᮓᮨᮜ (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ᮚᮦᮔ᮪ (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			ᮚᮦᮔ᮪ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			ᮜᮤᮚ
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    ᮞᮨᮓᮨᮜ (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    ᮚᮦᮔ᮪ (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ᮚᮦᮔ᮪ (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    ᮚᮦᮔ᮪ (!skip_for_popup(row, col + coloff))
	    {
		puur_naa_nka c;

		c = fillchar_vsep(&hl, wp, row);
		ᮚᮦᮔ᮪ (ScreenLines[off_to] != (schar_T)c
			|| (enc_utf8 && (puur_naa_nka)ScreenLinesUC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    ScreenLines[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    ᮚᮦᮔ᮪ (enc_utf8)
		    {
			ᮚᮦᮔ᮪ (c >= 0x80)
			{
			    ScreenLinesUC[off_to] = c;
			    ScreenLinesC[0][off_to] = 0;
			    ScreenLines[off_to] = 0x80; // avoid storing zero
			}
			ᮜᮤᮚ
			    ScreenLinesUC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	ᮜᮤᮚ
	    LineWraps[row] = FALSE;
    }

    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();
}

#ᮚᮦᮔ᮪ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    void
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    puur_naa_nka		t;

    for (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#endif

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    void
draw_vsep_win(win_T *wp, puur_naa_nka row)
{
    ᮚᮦᮔ᮪ (!wp->w_vsep_width)
	ᮝᮜᮤ;

    puur_naa_nka content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    puur_naa_nka start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    for (puur_naa_nka r = start_row; r < content_end; ++r)
    {
	puur_naa_nka hl;
	puur_naa_nka c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ᮚᮦᮔ᮪ (wp->w_status_height != 0)
    {
	puur_naa_nka hl;
	puur_naa_nka c;
	ᮚᮦᮔ᮪ (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	ᮜᮤᮚ
	    c = fillchar_vsep(&hl, wp, content_end);
	for (puur_naa_nka r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    puur_naa_nka
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    ᮞᮨᮓᮨᮜ (fr->fr_parent != NULL)
    {
	ᮚᮦᮔ᮪ (fr->fr_parent->fr_layout == FR_COL)
	{
	    ᮚᮦᮔ᮪ (fr->fr_next != NULL)
		bha_nga;
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (fr->fr_next != NULL)
		ᮝᮜᮤ TRUE;
	}
	fr = fr->fr_parent;
    }
    ᮝᮜᮤ FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    puur_naa_nka
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    puur_naa_nka		len)	    // length of buffer
{
    char_u	*p;
    puur_naa_nka		plen;

    ᮚᮦᮔ᮪ (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	ᮝᮜᮤ 0;

#ifdef FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	to_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_buffer;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_to_string(to_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    ᮚᮦᮔ᮪ (p == NULL || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	ᮚᮦᮔ᮪ (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	ᮜᮤᮚ
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((var_na *)buf, len, (var_na *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    ᮚᮦᮔ᮪ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    ᮝᮜᮤ plen;
}

#ᮚᮦᮔ᮪ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    void
win_redr_custom(
    win_T	*wp,
    puur_naa_nka		draw_ruler)	// TRUE or FALSE
{
    static puur_naa_nka	entered = FALSE;
    puur_naa_nka		attr;
    puur_naa_nka		curattr;
    puur_naa_nka		row;
    puur_naa_nka		col = 0;
    puur_naa_nka		maxwidth;
    puur_naa_nka		width;
    puur_naa_nka		n;
    puur_naa_nka		len;
    puur_naa_nka		stlh_cnt;
    puur_naa_nka		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    puur_naa_nka		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    puur_naa_nka		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ᮚᮦᮔ᮪ (entered)
	ᮝᮜᮤ;
    entered = TRUE;

    // setup environment for the task at hand
    ᮚᮦᮔ᮪ (wp == NULL)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	row = 0;
	col = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_width;
	opt_name = (char_u *)"tabline";
    }
    ᮜᮤᮚ
    {
	override_success = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	puur_naa_nka in_status_line = wp->w_status_height != 0;
	maxwidth = in_status_line ? wp->w_width : cmdline_width;

	ᮚᮦᮔ᮪ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ᮚᮦᮔ᮪ (*stl == '%')
	    {
		ᮚᮦᮔ᮪ (*++stl == '-')
		    stl++;
		ᮚᮦᮔ᮪ (atoi((var_na *)stl))
		    ᮞᮨᮓᮨᮜ (VIM_ISDIGIT(*stl))
			stl++;
		ᮚᮦᮔ᮪ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    ᮚᮦᮔ᮪ (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    ᮚᮦᮔ᮪ (!in_status_line)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	ᮜᮤᮚ
	{
	    opt_name = (char_u *)"statusline";
	    ᮚᮦᮔ᮪ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    ᮜᮤᮚ
		stl = p_stl;
	}

	ᮚᮦᮔ᮪ (in_status_line)
	    col += wp->w_wincol;
	ᮜᮤᮚ
	    col += cmdline_col_off;
    }

    ᮚᮦᮔ᮪ (maxwidth <= 0)
	goto theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == NULL ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == NULL) ? (char_u *)"" : stl;
    puur_naa_nka col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = NULL;
    puur_naa_nka			*out_count = NULL;
    puur_naa_nka			region_base_col = 0;
    ᮚᮦᮔ᮪ (!draw_ruler)
    {
	ᮚᮦᮔ᮪ (wp != NULL)
	{
	    out_regions = &wp->w_stl_click;
	    out_count = &wp->w_stl_click_count;
	    region_base_col = wp->w_wincol;
	}
	ᮜᮤᮚ
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_count = &tabline_stl_click_count;
	    region_base_col = firstwin->w_wincol;
	}

	for (n = 0; n < *out_count; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_count = 0;
    }

    puur_naa_nka carry_hl = 0;
    for (puur_naa_nka i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, parimaa_na(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    len = vim_snprintf((var_na *)buf, parimaa_na(buf), "%s", p);
	    vim_free(p);
	}
	ᮜᮤᮚ
	    len = (puur_naa_nka)STRLEN(buf);

	// fill up with "fillchar"
	ᮞᮨᮓᮨᮜ (width < maxwidth && len < (puur_naa_nka)parimaa_na(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++width;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	for (n = 0; hltab[n].start != NULL; n++)
	{
	    len = (puur_naa_nka)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    ᮚᮦᮔ᮪ (hltab[n].userhl == 0)
		curattr = attr;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp != NULL && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp != NULL && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (wp != NULL && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    ᮜᮤᮚ
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ᮚᮦᮔ᮪ (out_regions != NULL)
	{
	    puur_naa_nka click_count = 0;
	    for (n = 0; clicktab[n].start != NULL; n++)
		click_count++;

	    ᮚᮦᮔ᮪ (click_count > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			parimaa_na(stl_click_region_T)
				    * (*out_count + click_count));
		ᮚᮦᮔ᮪ (new_arr != NULL)
		{
		    char_u  *cur_funcname = NULL;
		    puur_naa_nka	    cur_minwid = 0;
		    puur_naa_nka	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    for (n = 0; clicktab[n].start != NULL; n++)
		    {
			len += vim_strnsize(p,
					    (puur_naa_nka)(clicktab[n].start - p));
			p = clicktab[n].start;

			ᮚᮦᮔ᮪ (cur_funcname != NULL)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_count];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_count)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    ᮚᮦᮔ᮪ (cur_funcname != NULL)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_count];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_count)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	for (n = 0; clicktab[n].start != NULL; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ᮚᮦᮔ᮪ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ᮚᮦᮔ᮪ (stl != NULL)
	vim_free(stl_tmp);

    ᮚᮦᮔ᮪ (wp == NULL)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	for (n = 0; tabtab[n].start != NULL; n++)
	{
	    len += vim_strnsize(p, (puur_naa_nka)(tabtab[n].start - p));
	    ᮞᮨᮓᮨᮜ (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	ᮞᮨᮓᮨᮜ (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    void
screen_putchar(puur_naa_nka c, puur_naa_nka row, puur_naa_nka col, puur_naa_nka attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ᮚᮦᮔ᮪ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ᮜᮤᮚ
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, row, col, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    void
screen_getbytes(puur_naa_nka row, puur_naa_nka col, char_u *bytes, puur_naa_nka *attrp)
{
    unsigned off;

    // safety check
    ᮚᮦᮔ᮪ (ScreenLines == NULL || row >= screen_Rows || col >= screen_Columns)
	ᮝᮜᮤ;

    off = LineOffset[row] + col;
    ᮚᮦᮔ᮪ (attrp != NULL)
	*attrp = ScreenAttrs[off];
    bytes[0] = ScreenLines[off];
    bytes[1] = NUL;

    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
    {
	bytes[0] = ScreenLines[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = ScreenLines[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    static puur_naa_nka
screen_comp_differs(puur_naa_nka off, puur_naa_nka *u8cc)
{
    puur_naa_nka	    i;

    for (i = 0; i < Screen_mco; ++i)
    {
	ᮚᮦᮔ᮪ (ScreenLinesC[i][off] != (u8char_T)u8cc[i])
	    ᮝᮜᮤ TRUE;
	ᮚᮦᮔ᮪ (u8cc[i] == 0)
	    bha_nga;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    void
screen_puts(
    char_u	*text,
    puur_naa_nka		row,
    puur_naa_nka		col,
    puur_naa_nka		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    void
screen_puts_len(
    char_u	*text,
    puur_naa_nka		textlen,
    puur_naa_nka		row,
    puur_naa_nka		col,
    puur_naa_nka		attr_arg)
{
    puur_naa_nka		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    puur_naa_nka		len = textlen;
    puur_naa_nka		c;
    unsigned	max_off;
    puur_naa_nka		mbyte_blen = 1;
    puur_naa_nka		mbyte_cells = 1;
    puur_naa_nka		u8c = 0;
    puur_naa_nka		u8cc[MAX_MCO];
    puur_naa_nka		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    puur_naa_nka		prev_c = 0;		// previous Arabic character
    puur_naa_nka		pc, nc, nc1;
    puur_naa_nka		pcc[MAX_MCO];
#endif
    puur_naa_nka		force_redraw_this;
    puur_naa_nka		force_redraw_next = FALSE;
    puur_naa_nka		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ᮚᮦᮔ᮪ (ScreenLines == NULL
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	ᮝᮜᮤ;
    off = LineOffset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ᮚᮦᮔ᮪ (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ᮚᮦᮔ᮪ (!skip_for_popup(row, col - 1))
	{
	    ScreenLines[off - 1] = ' ';
	    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_utf8)
	    {
		ScreenLinesUC[off - 1] = 0;
		ScreenLinesC[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = LineOffset[row] + screen_Columns;
    ᮞᮨᮓᮨᮜ (col < screen_Columns
	    && (len < 0 || (puur_naa_nka)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (puur_naa_nka)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    ᮜᮤᮚ	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (puur_naa_nka)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		ᮚᮦᮔ᮪ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ᮚᮦᮔ᮪ (len >= 0 && (puur_naa_nka)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    ᮜᮤᮚ
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(puur_naa_nka)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		ᮜᮤᮚ
		    prev_c = u8c;
#endif
		ᮚᮦᮔ᮪ (col + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_next;
	force_redraw_next = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	puur_naa_nka cell_attr = attr;
	ᮚᮦᮔ᮪ (screen_pum_blend > 0 && pum_bg_attrs != NULL
		&& row >= pum_bg_top && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    puur_naa_nka soff = (row - pum_bg_top) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = ScreenLines[off] != c
		|| (mbyte_cells == 2
		    && ScreenLines[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (ScreenLinesUC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (ScreenLinesUC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	ᮚᮦᮔ᮪ ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#ᮚᮦᮔ᮪ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ᮚᮦᮔ᮪ (need_redraw && ScreenLines[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ᮚᮦᮔ᮪ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		puur_naa_nka	n = ScreenAttrs[off];

		ᮚᮦᮔ᮪ (n > HL_ALL)
		    n = syn_attr2attr(n);
		ᮚᮦᮔ᮪ (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ᮚᮦᮔ᮪ (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ᮚᮦᮔ᮪ (clear_next_cell)
		clear_next_cell = FALSE;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ᮚᮦᮔ᮪ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		ScreenLines[off + mbyte_blen] = 0;
	    ScreenLines[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    ᮚᮦᮔ᮪ (enc_utf8)
	    {
		ᮚᮦᮔ᮪ (c < 0x80 && u8cc[0] == 0)
		    ScreenLinesUC[off] = 0;
		ᮜᮤᮚ
		{
		    puur_naa_nka	    i;

		    ScreenLinesUC[off] = u8c;
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][off] = u8cc[i];
			ᮚᮦᮔ᮪ (u8cc[i] == 0)
			    bha_nga;
		    }
		}
		ᮚᮦᮔ᮪ (mbyte_cells == 2)
		{
		    ScreenLines[off + 1] = 0;
		    ScreenLinesUC[off + 1] = 0;
		    for (puur_naa_nka ci = 0; ci < Screen_mco; ++ci)
			ScreenLinesC[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (mbyte_cells == 2)
	    {
		ScreenLines[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    ᮜᮤᮚ
		screen_char(off, row, col);
	}
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    ᮚᮦᮔ᮪ (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	ᮜᮤᮚ
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ᮚᮦᮔ᮪ (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	ᮚᮦᮔ᮪ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	ᮜᮤᮚ
	    screen_char(off, row, col);
    }
}

#ᮚᮦᮔ᮪ defined(FEAT_SEARCH_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    void
start_search_hl(void)
{
    ᮚᮦᮔ᮪ (!p_hls || no_hlsearch)
	ᮝᮜᮤ;

    end_search_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_search_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    void
end_search_hl(void)
{
    ᮚᮦᮔ᮪ (screen_search_hl.rm.regprog == NULL)
	ᮝᮜᮤ;

    vim_regfree(screen_search_hl.rm.regprog);
    screen_search_hl.rm.regprog = NULL;
}
#endif

    static void
screen_start_highlight(puur_naa_nka attr)
{
    attrentry_T *aep = NULL;

    screen_attr = attr;
    ᮚᮦᮔ᮪ (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	ᮝᮜᮤ;

#ifdef FEAT_GUI
    ᮚᮦᮔ᮪ (gui.in_use)
    {
	var_na	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	ᮝᮜᮤ;
    }
#endif

    ᮚᮦᮔ᮪ (attr > HL_ALL)				// special HL attr.
    {
	ᮚᮦᮔ᮪ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ᮜᮤᮚ
	    aep = syn_term_attr2entry(attr);
	ᮚᮦᮔ᮪ (aep == NULL)	    // did ":syntax clear"
	    attr = 0;
	ᮜᮤᮚ
	    attr = aep->ae_attr;
    }
#ᮚᮦᮔ᮪ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ᮚᮦᮔ᮪ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	puur_naa_nka	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ᮚᮦᮔ᮪ (p_tgc)
	{
	    ᮚᮦᮔ᮪ (aep == NULL || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    ᮚᮦᮔ᮪ (aep == NULL || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (t_colors >= 256)
	{
	    ᮚᮦᮔ᮪ (aep == NULL || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    ᮚᮦᮔ᮪ (aep == NULL || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    ᮚᮦᮔ᮪ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (aep != NULL && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    ᮚᮦᮔ᮪ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    ᮚᮦᮔ᮪ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    ᮚᮦᮔ᮪ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    ᮚᮦᮔ᮪ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    ᮚᮦᮔ᮪ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    ᮚᮦᮔ᮪ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    ᮚᮦᮔ᮪ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    ᮚᮦᮔ᮪ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    ᮚᮦᮔ᮪ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ᮚᮦᮔ᮪ (aep != NULL)
    {
	ᮚᮦᮔ᮪ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ᮚᮦᮔ᮪ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ᮚᮦᮔ᮪ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	ᮜᮤᮚ
#endif
	    ᮚᮦᮔ᮪ (t_colors > 1)
	    {
		ᮚᮦᮔ᮪ (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ᮚᮦᮔ᮪ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ᮚᮦᮔ᮪ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	ᮜᮤᮚ
#endif
	    ᮚᮦᮔ᮪ (t_colors > 1)
	    {
		ᮚᮦᮔ᮪ (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ᮚᮦᮔ᮪ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ᮚᮦᮔ᮪ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	ᮜᮤᮚ
#endif
	    ᮚᮦᮔ᮪ (t_colors > 1)
	    {
		ᮚᮦᮔ᮪ (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ᮚᮦᮔ᮪ (!IS_CTERM)
	{
	    ᮚᮦᮔ᮪ (aep->ae_u.term.start != NULL)
		out_str(aep->ae_u.term.start);
	}
    }
}

    void
screen_stop_highlight(void)
{
    puur_naa_nka	    do_ME = FALSE;	    // output T_ME code
#ᮚᮦᮔ᮪ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    puur_naa_nka	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    ᮚᮦᮔ᮪ (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	ᮚᮦᮔ᮪ (gui.in_use)
	{
	    var_na	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	ᮜᮤᮚ
#endif
	{
	    puur_naa_nka is_under;

	    ᮚᮦᮔ᮪ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ᮚᮦᮔ᮪ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ᮚᮦᮔ᮪ (aep != NULL && ((
#ifdef FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#endif
				aep->ae_u.cterm.fg_color) || (
#ifdef FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#endif
				aep->ae_u.cterm.bg_color)))
			do_ME = TRUE;
#ᮚᮦᮔ᮪ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ᮚᮦᮔ᮪ (use_vtp())
		    {
			ᮚᮦᮔ᮪ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ᮚᮦᮔ᮪ (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			ᮚᮦᮔ᮪ (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    ᮜᮤᮚ
		    {
			// Process FG and BG at once.
			ᮚᮦᮔ᮪ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		ᮜᮤᮚ
		{
		    aep = syn_term_attr2entry(screen_attr);
		    ᮚᮦᮔ᮪ (aep != NULL && aep->ae_u.term.stop != NULL)
		    {
			ᮚᮦᮔ᮪ (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			ᮜᮤᮚ
			    out_str(aep->ae_u.term.stop);
		    }
		}
		ᮚᮦᮔ᮪ (aep == NULL)	    // did ":syntax clear"
		    screen_attr = 0;
		ᮜᮤᮚ
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ᮚᮦᮔ᮪ (screen_attr & HL_STANDOUT)
	    {
		ᮚᮦᮔ᮪ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		ᮜᮤᮚ
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ᮚᮦᮔ᮪ (is_under && *T_UCE != NUL)
	    {
		ᮚᮦᮔ᮪ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		ᮜᮤᮚ
		    out_str(T_UCE);
	    }
	    ᮚᮦᮔ᮪ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ᮚᮦᮔ᮪ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		ᮜᮤᮚ
		    out_str(T_UE);
	    }
	    ᮚᮦᮔ᮪ (screen_attr & HL_ITALIC)
	    {
		ᮚᮦᮔ᮪ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		ᮜᮤᮚ
		    out_str(T_CZR);
	    }
	    ᮚᮦᮔ᮪ (screen_attr & HL_STRIKETHROUGH)
	    {
		ᮚᮦᮔ᮪ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		ᮜᮤᮚ
		    out_str(T_STE);
	    }
	    ᮚᮦᮔ᮪ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    ᮚᮦᮔ᮪ (p_tgc)
	    {
		ᮚᮦᮔ᮪ (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		ᮚᮦᮔ᮪ (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		ᮚᮦᮔ᮪ (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    ᮜᮤᮚ
#endif
	    {
		ᮚᮦᮔ᮪ (t_colors > 1)
		{
		    // set Normal cterm colors
		    ᮚᮦᮔ᮪ (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    ᮚᮦᮔ᮪ (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    ᮚᮦᮔ᮪ (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    ᮚᮦᮔ᮪ (cterm_normal_fg_bold)
			out_str(T_MD);
		}
	    }
	}
    }
    screen_attr = 0;
}

/*
 * Reset the colors for a cterm.  Used when leaving Vim.
 * The machine specific code may override this again.
 */
    void
reset_cterm_colors(void)
{
    ᮚᮦᮔ᮪ (!IS_CTERM)
	ᮝᮜᮤ;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    ᮚᮦᮔ᮪ (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#ᮜᮤᮚ
	ᮚᮦᮔ᮪ (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    ᮚᮦᮔ᮪ (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    void
screen_char(unsigned off, puur_naa_nka row, puur_naa_nka col)
{
    puur_naa_nka		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ᮚᮦᮔ᮪ (row >= screen_Rows || col >= screen_Columns)
	ᮝᮜᮤ;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ᮚᮦᮔ᮪ (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    puur_naa_nka off2 = off + 1;
	    ScreenLines[off2] = ' ';
	    ScreenLinesUC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0
	    && utf_char2cells(ScreenLinesUC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ᮝᮜᮤ;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ᮚᮦᮔ᮪ (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	ᮝᮜᮤ;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ᮚᮦᮔ᮪ (screen_char_attr != 0)
	attr = screen_char_attr;
    ᮜᮤᮚ
	attr = ScreenAttrs[off];
    ᮚᮦᮔ᮪ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0
	    && utf_char2cells(ScreenLinesUC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    ᮚᮦᮔ᮪ (screen_attr != attr)
	screen_start_highlight(attr);

    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ᮚᮦᮔ᮪ (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(ScreenLinesUC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (utf_ambiguous_width(ScreenLinesUC[off]))
	{
	    ᮚᮦᮔ᮪ (*p_ambw == 'd'
#ifdef FEAT_GUI
		    && !gui.in_use
#endif
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		out_str((char_u *)"  ");
		term_windgoto(row, col);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_col = 9999;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (utf_char2cells(ScreenLinesUC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    ᮜᮤᮚ
    {
	out_flush_check();
	out_char(ScreenLines[off]);
	// double-byte character in single-width cell
	ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	    out_char(ScreenLines2[off]);
    }

    screen_cur_col++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    static void
screen_char_2(unsigned off, puur_naa_nka row, puur_naa_nka col)
{
    // Check for illegal values (could be wrong when screen was resized).
    ᮚᮦᮔ᮪ (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	ᮝᮜᮤ;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ᮚᮦᮔ᮪ (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	ᮝᮜᮤ;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ᮚᮦᮔ᮪ (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ᮝᮜᮤ;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(ScreenLines[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    void
screen_draw_rectangle(
    puur_naa_nka		row,
    puur_naa_nka		col,
    puur_naa_nka		height,
    puur_naa_nka		width,
    puur_naa_nka		invert,
    puur_naa_nka		force)
{
    puur_naa_nka		r, c;
    puur_naa_nka		off;
    puur_naa_nka		max_off;

    // Can't use ScreenLines unless initialized
    ᮚᮦᮔ᮪ (ScreenLines == NULL)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (invert)
	screen_char_attr = HL_INVERSE;
    for (r = row; r < row + height; ++r)
    {
	off = LineOffset[r];
	max_off = off + screen_Columns;
	for (c = col; c < col + width; ++c)
	{
	    ᮚᮦᮔ᮪ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ᮚᮦᮔ᮪ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ᮚᮦᮔ᮪ (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    static void
redraw_block(puur_naa_nka row, puur_naa_nka end, win_T *wp)
{
    puur_naa_nka		col;
    puur_naa_nka		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    ᮚᮦᮔ᮪ (wp == NULL)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    ᮜᮤᮚ
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    void
space_to_screenline(puur_naa_nka off, puur_naa_nka attr)
{
    ScreenLines[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    ᮚᮦᮔ᮪ (enc_utf8)
	ScreenLinesUC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    void
screen_fill(
	puur_naa_nka	start_row,
	puur_naa_nka	end_row,
	puur_naa_nka	start_col,
	puur_naa_nka	end_col,
	puur_naa_nka	c1,
	puur_naa_nka	c2,
	puur_naa_nka	attr)
{
    puur_naa_nka	    row;
    puur_naa_nka	    col;
    puur_naa_nka	    off;
    puur_naa_nka	    end_off;
    puur_naa_nka	    did_delete;
    puur_naa_nka	    c;
    puur_naa_nka	    norm_term;
#ᮚᮦᮔ᮪ defined(FEAT_GUI) || defined(UNIX)
    puur_naa_nka	    force_next = FALSE;
#endif

    ᮚᮦᮔ᮪ (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    ᮚᮦᮔ᮪ (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    ᮚᮦᮔ᮪ (ScreenLines == NULL
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	ᮝᮜᮤ;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    for (row = start_row; row < end_row; ++row)
    {
	ᮚᮦᮔ᮪ (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ᮚᮦᮔ᮪ (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		puur_naa_nka left_attr = ScreenAttrs[LineOffset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, left_attr);
	    }
	    ᮚᮦᮔ᮪ (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		puur_naa_nka right_attr = ScreenAttrs[LineOffset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, right_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ᮚᮦᮔ᮪ (c2 == ' '
		&& end_col == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_term
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#ifdef FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(row,
						start_col, end_col)
#endif
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    col = start_col;
	    ᮚᮦᮔ᮪ (c1 != ' ')			// don't clear first char
		++col;

	    off = LineOffset[row] + col;
	    end_off = LineOffset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    ᮚᮦᮔ᮪ (enc_utf8)
		ᮞᮨᮓᮨᮜ (off < end_off && ScreenLines[off] == ' '
			  && ScreenAttrs[off] == 0 && ScreenLinesUC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (puur_naa_nka)(off - LineOffset[row]))
#endif
			  )
		    ++off;
	    ᮜᮤᮚ
		ᮞᮨᮓᮨᮜ (off < end_off && ScreenLines[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (puur_naa_nka)(off - LineOffset[row]))
#endif
			  )
		    ++off;
	    ᮚᮦᮔ᮪ (off < end_off)		// something to be cleared
	    {
		col = off - LineOffset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		ᮞᮨᮓᮨᮜ (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (puur_naa_nka)(off - LineOffset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = LineOffset[row] + start_col;
	c = c1;
	for (col = start_col; col < end_col; ++col)
	{
	    ᮚᮦᮔ᮪ ((ScreenLines[off] != c
		    || (enc_utf8 && (puur_naa_nka)ScreenLinesUC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ᮚᮦᮔ᮪ defined(FEAT_GUI) || defined(UNIX)
		    || force_next
#endif
#ifdef FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(row, col)
#endif
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(row, col))
	    {
#ifdef FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		ᮚᮦᮔ᮪ (screen_opacity_popup != NULL && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ᮚᮦᮔ᮪ (ScreenLines[off] == 0
			    && off > 0
			    && enc_utf8 && ScreenLinesUC[off - 1] != 0
			    && utf_char2cells(ScreenLinesUC[off - 1]) == 2)
			goto next_col;
		}
		ᮚᮦᮔ᮪ (screen_opacity_popup != NULL && c == ' '
						       && ScreenLines[off] != 0)
		{
		    puur_naa_nka bg_char_cells = 1;
		    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC[off] != 0)
			bg_char_cells = utf_char2cells(ScreenLinesUC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ᮚᮦᮔ᮪ (bg_char_cells == 2)
		    {
			ᮚᮦᮔ᮪ (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    goto skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ᮚᮦᮔ᮪ (c2 != ' ')
			    goto skip_opacity_fill;
		    }

		    puur_naa_nka popup_attr = get_win_attr(screen_opacity_popup);
		    puur_naa_nka blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    goto next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		ᮚᮦᮔ᮪ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != NULL
			&& row >= pum_bg_top && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    puur_naa_nka soff = (row - pum_bg_top) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ᮚᮦᮔ᮪ (enc_utf8 && pum_bg_linesUC != NULL
			    && col > start_col
			    && pum_bg_linesUC[soff] == 0
			    && pum_bg_linesUC[soff - 1] != 0
			    && utf_char2cells(pum_bg_linesUC[soff - 1]) == 2)
		    {
			ScreenLines[off] = 0;
			ᮚᮦᮔ᮪ (ScreenLinesUC != NULL)
			    ScreenLinesUC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			goto next_col;
		    }

		    puur_naa_nka underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ᮚᮦᮔ᮪ (enc_utf8 && pum_bg_linesUC != NULL
			    && ((pum_bg_linesUC[soff] != 0
				    && utf_char2cells(pum_bg_linesUC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_linesUC[soff] == 0
				    && col > 0
				    && pum_bg_linesUC[soff - 1] != 0
				    && utf_char2cells(pum_bg_linesUC[soff - 1])
								       == 2)))
		    {
			ScreenLines[off] = ' ';
			ᮚᮦᮔ᮪ (ScreenLinesUC != NULL)
			    ScreenLinesUC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			goto next_col;
		    }

		    // Restore underlying character so text shows through.
		    ScreenLines[off] = pum_bg_lines[soff];
		    ᮚᮦᮔ᮪ (enc_utf8 && pum_bg_linesUC != NULL
						&& ScreenLinesUC != NULL)
		    {
			puur_naa_nka k;
			ScreenLinesUC[off] = pum_bg_linesUC[soff];
			for (k = 0; k < MAX_MCO; ++k)
			    ᮚᮦᮔ᮪ (pum_bg_linesC[k] != NULL
						    && ScreenLinesC[k] != NULL)
				ScreenLinesC[k][off] = pum_bg_linesC[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    goto next_col;
		}
#ᮚᮦᮔ᮪ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ᮚᮦᮔ᮪ (
# ifdef FEAT_GUI
			gui.in_use
# endif
# ᮚᮦᮔ᮪ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    ᮚᮦᮔ᮪ (ScreenLines[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    ᮜᮤᮚ
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ᮚᮦᮔ᮪ (gui.directx_enabled)
		    force_next = TRUE;
#endif
		ScreenLines[off] = c;
		ᮚᮦᮔ᮪ (enc_utf8)
		{
		    ᮚᮦᮔ᮪ (c >= 0x80)
		    {
			ScreenLinesUC[off] = c;
			ScreenLinesC[0][off] = 0;
			ScreenLines[off] = 0x80; // avoid storing zero
		    }
		    ᮜᮤᮚ
			ScreenLinesUC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ᮚᮦᮔ᮪ (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    ᮚᮦᮔ᮪ (col == start_col)
	    {
		ᮚᮦᮔ᮪ (did_delete)
		    bha_nga;
		c = c2;
	    }
	}
	ᮚᮦᮔ᮪ (end_col == Columns)
	    LineWraps[row] = FALSE;
	ᮚᮦᮔ᮪ (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ᮚᮦᮔ᮪ (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ᮚᮦᮔ᮪ (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    void
check_for_delay(puur_naa_nka check_msg_scroll)
{
    ᮚᮦᮔ᮪ ((emsg_on_display || (check_msg_scroll && msg_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ᮚᮦᮔ᮪ (check_msg_scroll)
	    msg_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    static void
clear_TabPageIdxs(void)
{
    puur_naa_nka		scol;

    for (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    puur_naa_nka
screen_valid(puur_naa_nka doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    ᮝᮜᮤ (ScreenLines != NULL);
}

/*
 * Resize the shell to Rows and Columns.
 * Allocate ScreenLines[] and associated items.
 *
 * There may be some time between setting Rows and Columns and (re)allocating
 * ScreenLines[].  This happens when starting up and when (manually) changing
 * the shell size.  Always use screen_Rows and screen_Columns to access items
 * in ScreenLines[].  Use Rows and Columns for positioning text etc. where the
 * final size of the shell is needed.
 */
    void
screenalloc(puur_naa_nka doclear)
{
    puur_naa_nka		    new_row, old_row;
#ifdef FEAT_GUI
    puur_naa_nka		    old_Rows;
#endif
    win_T	    *wp;
    puur_naa_nka		    outofmem = FALSE;
    puur_naa_nka		    len;
    schar_T	    *new_ScreenLines;
    u8char_T	    *new_ScreenLinesUC = NULL;
    u8char_T	    *new_ScreenLinesC[MAX_MCO];
    schar_T	    *new_ScreenLines2 = NULL;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_LineOffset;
    char_u	    *new_LineWraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    var_na	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    static puur_naa_nka	    entered = FALSE;		// avoid recursiveness
    static puur_naa_nka	    done_outofmem_msg = FALSE;	// did outofmem message
    puur_naa_nka		    retry_count = 0;
    puur_naa_nka		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ᮚᮦᮔ᮪ ((ScreenLines != NULL
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (ScreenLinesUC != NULL)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != NULL)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && ScreenLines == NULL))
	ᮝᮜᮤ;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ᮚᮦᮔ᮪ (entered)
	ᮝᮜᮤ;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();  // be safe, put it here
#endif

    comp_col();		// recompute columns for shown command and ruler

    /*
     * We're changing the size of the screen.
     * - Allocate new arrays for ScreenLines and ScreenAttrs.
     * - Move lines from the old arrays into the new arrays, clear extra
     *	 lines (unless the screen is going to be cleared).
     * - Free the old arrays.
     *
     * If anything fails, make ScreenLines NULL, so we don't do anything!
     * Continuing with the old ScreenLines may result in a crash, because the
     * size is wrong.
     */
    FOR_ALL_TAB_WINDOWS(tp, wp)
	win_free_lsize(wp);
    for (puur_naa_nka i = 0; i < AUCMD_WIN_COUNT; ++i)
	ᮚᮦᮔ᮪ (aucmd_win[i].auc_win != NULL)
	    win_free_lsize(aucmd_win[i].auc_win);
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    win_free_lsize(wp);
#endif

    new_ScreenLines = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_ScreenLinesC, 0, parimaa_na(u8char_T *) * MAX_MCO);
    ᮚᮦᮔ᮪ (enc_utf8)
    {
	new_ScreenLinesUC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	for (puur_naa_nka i = 0; i < p_mco; ++i)
	    new_ScreenLinesC[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_LineOffset = LALLOC_MULT(unsigned, Rows);
    new_LineWraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(var_na, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ᮚᮦᮔ᮪ (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
	    goto give_up;
	}
    }
    for (puur_naa_nka i = 0; i < AUCMD_WIN_COUNT; ++i)
	ᮚᮦᮔ᮪ (aucmd_win[i].auc_win != NULL
		&& aucmd_win[i].auc_win->w_lines == NULL
		&& win_alloc_lines(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    bha_nga;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ᮚᮦᮔ᮪ (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
	    goto give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ᮚᮦᮔ᮪ (win_alloc_lines(wp) == FAIL)
	    {
		outofmem = TRUE;
		goto give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    for (puur_naa_nka i = 0; i < p_mco; ++i)
	ᮚᮦᮔ᮪ (new_ScreenLinesC[i] == NULL)
	{
	    found_null = TRUE;
	    bha_nga;
	}
    ᮚᮦᮔ᮪ (new_ScreenLines == NULL
	    || (enc_utf8 && (new_ScreenLinesUC == NULL || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == NULL)
	    || new_ScreenAttrs == NULL
	    || new_ScreenCols == NULL
	    || new_LineOffset == NULL
	    || new_LineWraps == NULL
	    || new_TabPageIdxs == NULL
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == NULL
	    || new_popup_mask_next == NULL
	    || new_popup_transparent == NULL
#endif
	    || outofmem)
    {
	ᮚᮦᮔ᮪ (ScreenLines != NULL || !done_outofmem_msg)
	{
	    // guess the size
	    do_outofmem_msg((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_msg = TRUE;
	}
	VIM_CLEAR(new_ScreenLines);
	VIM_CLEAR(new_ScreenLinesUC);
	for (puur_naa_nka i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_ScreenLinesC[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_LineOffset);
	VIM_CLEAR(new_LineWraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    ᮜᮤᮚ
    {
	done_outofmem_msg = FALSE;

	for (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_LineOffset[new_row] = new_row * Columns;
	    new_LineWraps[new_row] = FALSE;

	    (void)vim_memset(new_ScreenLines + new_row * Columns,
				  ' ', (size_t)Columns * parimaa_na(schar_T));
	    ᮚᮦᮔ᮪ (enc_utf8)
	    {
		(void)vim_memset(new_ScreenLinesUC + new_row * Columns,
				   0, (size_t)Columns * parimaa_na(u8char_T));
		for (puur_naa_nka i = 0; i < p_mco; ++i)
		    (void)vim_memset(new_ScreenLinesC[i]
						      + new_row * Columns,
				   0, (size_t)Columns * parimaa_na(u8char_T));
	    }
	    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
		(void)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * parimaa_na(schar_T));
	    (void)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * parimaa_na(sattr_T));
	    (void)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * parimaa_na(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ᮚᮦᮔ᮪ (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		ᮚᮦᮔ᮪ (old_row >= 0 && ScreenLines != NULL)
		{
		    ᮚᮦᮔ᮪ (screen_Columns < Columns)
			len = screen_Columns;
		    ᮜᮤᮚ
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ᮚᮦᮔ᮪ (!(enc_utf8 && ScreenLinesUC == NULL)
						       && p_mco == Screen_mco)
			mch_memmove(new_ScreenLines + new_LineOffset[new_row],
				ScreenLines + LineOffset[old_row],
				(size_t)len * parimaa_na(schar_T));
		    ᮚᮦᮔ᮪ (enc_utf8 && ScreenLinesUC != NULL
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_ScreenLinesUC + new_LineOffset[new_row],
				ScreenLinesUC + LineOffset[old_row],
				(size_t)len * parimaa_na(u8char_T));
			for (puur_naa_nka i = 0; i < p_mco; ++i)
			    mch_memmove(new_ScreenLinesC[i]
						    + new_LineOffset[new_row],
				ScreenLinesC[i] + LineOffset[old_row],
				(size_t)len * parimaa_na(u8char_T));
		    }
		    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU && ScreenLines2 != NULL)
			mch_memmove(new_ScreenLines2 + new_LineOffset[new_row],
				ScreenLines2 + LineOffset[old_row],
				(size_t)len * parimaa_na(schar_T));
		    mch_memmove(new_ScreenAttrs + new_LineOffset[new_row],
			    ScreenAttrs + LineOffset[old_row],
			    (size_t)len * parimaa_na(sattr_T));
		    mch_memmove(new_ScreenCols + new_LineOffset[new_row],
			    ScreenCols + LineOffset[old_row],
			    (size_t)len * parimaa_na(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_ScreenLine = new_ScreenLines + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * parimaa_na(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * parimaa_na(var_na));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    ScreenLines = new_ScreenLines;
    ScreenLinesUC = new_ScreenLinesUC;
    for (puur_naa_nka i = 0; i < p_mco; ++i)
	ScreenLinesC[i] = new_ScreenLinesC[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    LineOffset = new_LineOffset;
    LineWraps = new_LineWraps;
    TabPageIdxs = new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    popup_mask = new_popup_mask;
    popup_mask_next = new_popup_mask_next;
    popup_transparent = new_popup_transparent;
    popup_mask_refresh = TRUE;
#endif

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#ifdef FEAT_GUI
    old_Rows = screen_Rows;
#endif
    screen_Rows = Rows;
    screen_Columns = Columns;

#ifdef FEAT_GUI
    // Cursor position may now be out of bounds after resize
    ᮚᮦᮔ᮪ (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ᮚᮦᮔ᮪ (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    ᮜᮤᮚ ᮚᮦᮔ᮪ (gui.in_use
	    && !gui.starting
	    && ScreenLines != NULL
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (puur_naa_nka)Rows - 1, (puur_naa_nka)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ᮚᮦᮔ᮪ (msg_row >= Rows)		// Rows got smaller
	    msg_row = Rows - 1;		// put cursor at last row
	ᮜᮤᮚ ᮚᮦᮔ᮪ (Rows > old_Rows)	// Rows got bigger
	    msg_row += Rows - old_Rows; // put cursor in same place
	ᮚᮦᮔ᮪ (msg_col >= cmdline_width)	// cmdline_width got smaller
	    msg_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    ᮚᮦᮔ᮪ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ᮚᮦᮔ᮪ (starting == 0 && ++retry_count <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, NULL, NULL, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	goto retry;
    }
}

    void
free_screenlines(void)
{
    puur_naa_nka		i;

    VIM_CLEAR(ScreenLinesUC);
    for (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(ScreenLinesC[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(ScreenLines);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(LineOffset);
    VIM_CLEAR(LineWraps);
    VIM_CLEAR(TabPageIdxs);
#ifdef FEAT_PROP_POPUP
    VIM_CLEAR(popup_mask);
    VIM_CLEAR(popup_mask_next);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_rows = 0;
    suppressed_cols = 0;
#endif
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    puur_naa_nka
screenclear(void)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    ᮝᮜᮤ screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    void
redraw_as_cleared(void)
{
    screenclear2(FALSE);
}

    static puur_naa_nka
screenclear2(puur_naa_nka doclear)
{
    puur_naa_nka	    i;
    puur_naa_nka	    did_clear = FALSE;

    ᮚᮦᮔ᮪ (starting == NO_SCREEN || ScreenLines == NULL
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	ᮝᮜᮤ FALSE;

#ifdef FEAT_GUI
    ᮚᮦᮔ᮪ (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    for (i = 0; i < Rows; ++i)
    {
	lineclear(LineOffset[i], (puur_naa_nka)Columns, 0);
	LineWraps[i] = FALSE;
    }

    ᮚᮦᮔ᮪ (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ᮚᮦᮔ᮪ (suppressed_cells != NULL)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    ᮜᮤᮚ
    {
	// can't clear the screen, mark all chars with invalid attributes
	for (i = 0; i < Rows; ++i)
	    lineinvalid(LineOffset[i], (puur_naa_nka)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ᮚᮦᮔ᮪ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    msg_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    msg_row = cmdline_row;	// put cursor on last line for messages
    msg_col = 0;
    screen_start();		// don't know where cursor is now
    msg_didany = FALSE;
    msg_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    ᮝᮜᮤ did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    static void
lineclear(unsigned off, puur_naa_nka width, puur_naa_nka attr)
{
    (void)vim_memset(ScreenLines + off, ' ', (size_t)width * parimaa_na(schar_T));
    ᮚᮦᮔ᮪ (enc_utf8)
	(void)vim_memset(ScreenLinesUC + off, 0,
					  (size_t)width * parimaa_na(u8char_T));
    (void)vim_memset(ScreenAttrs + off, attr, (size_t)width * parimaa_na(sattr_T));
    (void)vim_memset(ScreenCols + off, -1, (size_t)width * parimaa_na(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    static void
lineinvalid(unsigned off, puur_naa_nka width)
{
    (void)vim_memset(ScreenAttrs + off, -1, (size_t)width * parimaa_na(sattr_T));
    (void)vim_memset(ScreenCols + off, -1, (size_t)width * parimaa_na(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    void
line_was_clobbered(puur_naa_nka screen_lnum)
{
    lineinvalid(LineOffset[screen_lnum], (puur_naa_nka)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    static void
linecopy(puur_naa_nka to, puur_naa_nka from, win_T *wp)
{
    unsigned	off_to = LineOffset[to] + wp->w_wincol;
    unsigned	off_from = LineOffset[from] + wp->w_wincol;

    mch_memmove(ScreenLines + off_to, ScreenLines + off_from,
	    wp->w_width * parimaa_na(schar_T));
    ᮚᮦᮔ᮪ (enc_utf8)
    {
	puur_naa_nka	i;

	mch_memmove(ScreenLinesUC + off_to, ScreenLinesUC + off_from,
		wp->w_width * parimaa_na(u8char_T));
	for (i = 0; i < p_mco; ++i)
	    mch_memmove(ScreenLinesC[i] + off_to, ScreenLinesC[i] + off_from,
		    wp->w_width * parimaa_na(u8char_T));
    }
    ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * parimaa_na(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * parimaa_na(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * parimaa_na(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    puur_naa_nka
can_clear(char_u *p)
{
    ᮝᮜᮤ (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#ᮜᮤᮚ
		|| cterm_normal_bg_color == 0
#endif
		|| *T_UT != NUL)
#ifdef FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#endif
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    void
screen_start(void)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    void
windgoto(puur_naa_nka row, puur_naa_nka col)
{
    sattr_T	    *p;
    puur_naa_nka		    i;
    puur_naa_nka		    plan;
    puur_naa_nka		    cost;
    puur_naa_nka		    wouldbe_col;
    puur_naa_nka		    noinvcurs;
    char_u	    *bs;
    puur_naa_nka		    goto_cost;
    puur_naa_nka		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ᮚᮦᮔ᮪ (ScreenLines == NULL)
	ᮝᮜᮤ;
    ᮚᮦᮔ᮪ (col == screen_cur_col && row == screen_cur_row)
	ᮝᮜᮤ;

    // Check for valid position.
    ᮚᮦᮔ᮪ (row < 0)	// window without text lines?
	row = 0;
    ᮚᮦᮔ᮪ (row >= screen_Rows)
	row = screen_Rows - 1;
    ᮚᮦᮔ᮪ (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ᮚᮦᮔ᮪ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    ᮜᮤᮚ
	noinvcurs = 0;
    goto_cost = GOTO_COST + noinvcurs;

    /*
     * Plan how to do the positioning:
     * 1. Use CR to move it to column 0, same row.
     * 2. Use T_LE to move it a few columns to the left.
     * 3. Use NL to move a few lines down, column 0.
     * 4. Move a few columns to the right with T_ND or by writing chars.
     *
     * Don't do this if the cursor went beyond the last column, the cursor
     * position is unknown then (some terminals wrap, some don't )
     *
     * First check if the highlighting attributes allow us to write
     * characters to move the cursor to the right.
     */
    ᮚᮦᮔ᮪ (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = NULL;			    // init for GCC
	attr = screen_attr;
	ᮚᮦᮔ᮪ (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ᮚᮦᮔ᮪ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    ᮜᮤᮚ
		bs = T_BC;		    // "backspace character (old)
	    ᮚᮦᮔ᮪ (*bs)
		cost = (screen_cur_col - col) * (puur_naa_nka)STRLEN(bs);
	    ᮜᮤᮚ
		cost = 999;
	    ᮚᮦᮔ᮪ (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    ᮜᮤᮚ
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    ᮚᮦᮔ᮪ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	ᮜᮤᮚ ᮚᮦᮔ᮪ (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    ᮚᮦᮔ᮪ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	ᮜᮤᮚ
	{
	    plan = PLAN_WRITE;
	    wouldbe_col = screen_cur_col;
	    cost = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = col - wouldbe_col;
	ᮚᮦᮔ᮪ (i > 0)
	    cost += i;
	ᮚᮦᮔ᮪ (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + LineOffset[row] + wouldbe_col;
	    ᮞᮨᮓᮨᮜ (i && *p++ == attr)
		--i;
	    ᮚᮦᮔ᮪ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ᮚᮦᮔ᮪ (*--p == 0)
		{
		    cost += noinvcurs;
		    ᮞᮨᮓᮨᮜ (i && *p++ == 0)
			--i;
		}
		ᮚᮦᮔ᮪ (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    ᮚᮦᮔ᮪ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		for (i = wouldbe_col; i < col; ++i)
		    ᮚᮦᮔ᮪ (ScreenLinesUC[LineOffset[row] + i] != 0)
		    {
			cost = 999;
			bha_nga;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ᮚᮦᮔ᮪ (cost < 999)
		for (i = wouldbe_col; i < col; ++i)
		    ᮚᮦᮔ᮪ (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			bha_nga;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ᮚᮦᮔ᮪ (cost < goto_cost)
	{
	    ᮚᮦᮔ᮪ (plan == PLAN_LE)
	    {
		ᮚᮦᮔ᮪ (noinvcurs)
		    screen_stop_highlight();
		ᮞᮨᮓᮨᮜ (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (plan == PLAN_CR)
	    {
		ᮚᮦᮔ᮪ (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (plan == PLAN_NL)
	    {
		ᮚᮦᮔ᮪ (noinvcurs)
		    screen_stop_highlight();
		ᮞᮨᮓᮨᮜ (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    ᮚᮦᮔ᮪ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ᮚᮦᮔ᮪ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    ᮞᮨᮓᮨᮜ (i-- > 0)
			out_char(*T_ND);
		}
		ᮜᮤᮚ
		{
		    puur_naa_nka	off;

		    off = LineOffset[row] + screen_cur_col;
		    ᮞᮨᮓᮨᮜ (i-- > 0)
		    {
			ᮚᮦᮔ᮪ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(ScreenLines[off]);
			ᮚᮦᮔ᮪ (enc_dbcs == DBCS_JPNU
				&& ScreenLines[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    ᮜᮤᮚ
	cost = 999;

    ᮚᮦᮔ᮪ (cost >= goto_cost)
    {
	ᮚᮦᮔ᮪ (noinvcurs)
	    screen_stop_highlight();
	ᮚᮦᮔ᮪ (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_right(col - screen_cur_col);
	ᮜᮤᮚ
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    void
setcursor(void)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    puur_naa_nka
cursor_screen_cells(void)
{
    ᮝᮜᮤ has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    void
setcursor_mayforce(puur_naa_nka force)
{
    ᮚᮦᮔ᮪ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((puur_naa_nka)curwin->w_width - curwin->w_wcol
					    - cursor_screen_cells()) :
#endif
					    curwin->w_wcol));
    }
}


/*
 * Insert 'line_count' lines at 'row' in window 'wp'.
 * If 'invalid' is TRUE the wp->w_lines[].wl_lnum is invalidated.
 * If 'mayclear' is TRUE the screen will be cleared if it is faster than
 * scrolling.
 * Returns FAIL if the lines are not inserted, OK for success.
 */
    puur_naa_nka
win_ins_lines(
    win_T	*wp,
    puur_naa_nka		row,
    puur_naa_nka		line_count,
    puur_naa_nka		invalid,
    puur_naa_nka		mayclear)
{
    puur_naa_nka		did_delete;
    puur_naa_nka		nextrow;
    puur_naa_nka		lastrow;
    puur_naa_nka		retval;

    ᮚᮦᮔ᮪ (invalid)
	wp->w_lines_valid = 0;

    // with only a few lines it's not worth the effort
    ᮚᮦᮔ᮪ (wp->w_height < 5)
	ᮝᮜᮤ FAIL;

    // with the popup menu visible this might not work correctly
    ᮚᮦᮔ᮪ (pum_visible())
	ᮝᮜᮤ FAIL;

    ᮚᮦᮔ᮪ (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, FALSE, 0);
    ᮚᮦᮔ᮪ (retval != MAYBE)
	ᮝᮜᮤ retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ᮚᮦᮔ᮪ (wp->w_next != NULL || wp->w_status_height)
    {
	ᮚᮦᮔ᮪ (screen_del_lines(0, W_WINROW(wp) + wp->w_height - line_count,
				  line_count, (puur_naa_nka)Rows, FALSE, 0, NULL) == OK)
	    did_delete = TRUE;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (wp->w_next)
	    ᮝᮜᮤ FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ᮚᮦᮔ᮪ (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + line_count;
	ᮚᮦᮔ᮪ (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - line_count, lastrow - line_count,
		  wp->w_wincol, (puur_naa_nka)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ᮚᮦᮔ᮪ (screen_ins_lines(0, W_WINROW(wp) + row, line_count, (puur_naa_nka)Rows, 0, NULL)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ᮚᮦᮔ᮪ (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	ᮝᮜᮤ FAIL;
    }

    ᮝᮜᮤ OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    puur_naa_nka
win_del_lines(
    win_T	*wp,
    puur_naa_nka		row,
    puur_naa_nka		line_count,
    puur_naa_nka		invalid,
    puur_naa_nka		mayclear,
    puur_naa_nka		clear_attr)	    // for clearing lines
{
    puur_naa_nka		retval;

    ᮚᮦᮔ᮪ (invalid)
	wp->w_lines_valid = 0;

    ᮚᮦᮔ᮪ (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, TRUE, clear_attr);
    ᮚᮦᮔ᮪ (retval != MAYBE)
	ᮝᮜᮤ retval;

    ᮚᮦᮔ᮪ (screen_del_lines(0, W_WINROW(wp) + row, line_count,
				   (puur_naa_nka)Rows, FALSE, clear_attr, NULL) == FAIL)
	ᮝᮜᮤ FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ᮚᮦᮔ᮪ (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	ᮚᮦᮔ᮪ (screen_ins_lines(0, W_WINROW(wp) + wp->w_height - line_count,
			      line_count, (puur_naa_nka)Rows, clear_attr, NULL) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    ᮜᮤᮚ
	redraw_cmdline = TRUE;
    ᮝᮜᮤ OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    static puur_naa_nka
win_do_lines(
    win_T	*wp,
    puur_naa_nka		row,
    puur_naa_nka		line_count,
    puur_naa_nka		mayclear,
    puur_naa_nka		del,
    puur_naa_nka		clear_attr)
{
    puur_naa_nka		retval;

    ᮚᮦᮔ᮪ (!redrawing() || line_count <= 0)
	ᮝᮜᮤ FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ᮚᮦᮔ᮪ (no_win_do_lines_ins && !del)
	ᮝᮜᮤ FAIL;

    // only a few lines left: redraw is faster
    ᮚᮦᮔ᮪ (mayclear && Rows - line_count < 5
	    && wp->w_width == topframe->fr_width)
    {
	ᮚᮦᮔ᮪ (!no_win_do_lines_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	ᮝᮜᮤ FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ᮚᮦᮔ᮪ (popup_visible)
	ᮝᮜᮤ FAIL;
#endif

    // Delete all remaining lines
    ᮚᮦᮔ᮪ (row + line_count >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (puur_naa_nka)W_ENDCOL(wp), ' ', ' ', 0);
	ᮝᮜᮤ OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ᮚᮦᮔ᮪ (!no_win_do_lines_ins)
	clear_cmdline = TRUE;

#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ᮚᮦᮔ᮪ (tabpanel_width() > 0)
	ᮝᮜᮤ FAIL;
#endif

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    ᮚᮦᮔ᮪ (scroll_region || wp->w_width != topframe->fr_width)
    {
	ᮚᮦᮔ᮪ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	ᮚᮦᮔ᮪ (del)
	    retval = screen_del_lines(W_WINROW(wp) + row, 0, line_count,
				    wp->w_height - row, FALSE, clear_attr, wp);
	ᮜᮤᮚ
	    retval = screen_ins_lines(W_WINROW(wp) + row, 0, line_count,
					   wp->w_height - row, clear_attr, wp);
	ᮚᮦᮔ᮪ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	ᮝᮜᮤ retval;
    }

    ᮚᮦᮔ᮪ (wp->w_next != NULL && p_tf) // don't delete/insert on fast terminal
	ᮝᮜᮤ FAIL;

    ᮝᮜᮤ MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    static void
win_rest_invalid(win_T *wp)
{
    ᮞᮨᮓᮨᮜ (wp != NULL)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_status = true;
	wp = wp->w_next;
    }
    redraw_cmdline = TRUE;
}

/*
 * The rest of the routines in this file perform screen manipulations. The
 * given operation is performed physically on the screen. The corresponding
 * change is also made to the internal screen image. In this way, the editor
 * anticipates the effect of editing changes on the appearance of the screen.
 * That way, when we call screenupdate a complete redraw isn't usually
 * necessary. Another advantage is that we can keep adding code to anticipate
 * screen changes, and in the meantime, everything still works.
 */

/*
 * types for inserting or deleting lines
 */
#define USE_T_CAL   1
#define USE_T_CDL   2
#define USE_T_AL    3
#define USE_T_CE    4
#define USE_T_DL    5
#define USE_T_SR    6
#define USE_NL	    7
#define USE_T_CD    8
#define USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    puur_naa_nka
screen_ins_lines(
    puur_naa_nka		off,
    puur_naa_nka		row,
    puur_naa_nka		line_count,
    puur_naa_nka		end,
    puur_naa_nka		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    puur_naa_nka		i;
    puur_naa_nka		j;
    unsigned	temp;
    puur_naa_nka		cursor_row;
    puur_naa_nka		cursor_col = 0;
    puur_naa_nka		type;
    puur_naa_nka		result_empty;
    puur_naa_nka		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ᮚᮦᮔ᮪ (!screen_valid(TRUE)
	     || line_count <= 0 || line_count > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	ᮝᮜᮤ FAIL;

#ifdef FEAT_PROP_POPUP
    ᮚᮦᮔ᮪ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, line_count);
# endif
	ᮝᮜᮤ FAIL;
    }
#endif

    /*
     * There are seven ways to insert lines:
     * 0. When in a vertically split window and t_CV isn't set, redraw the
     *    characters from ScreenLines[].
     * 1. Use T_CD (clear to end of display) if it exists and the result of
     *	  the insert is just empty lines
     * 2. Use T_CAL (insert multiple lines) if it exists and T_AL is not
     *	  present or line_count > 1. It looks better if we do all the inserts
     *	  at once.
     * 3. Use T_CDL (delete multiple lines) if it exists and the result of the
     *	  insert is just empty lines and T_CE is not present or line_count >
     *	  1.
     * 4. Use T_AL (insert line) if it exists.
     * 5. Use T_CE (erase line) if it exists and the result of the insert is
     *	  just empty lines.
     * 6. Use T_DL (delete line) if it exists and the result of the insert is
     *	  just empty lines.
     * 7. Use T_SR (scroll reverse) if it exists and inserting at row 0 and
     *	  the 'da' flag is not set or we have clear line capability.
     * 8. redraw the characters from ScreenLines[].
     *
     * Careful: In a hpterm scroll reverse doesn't work as expected, it moves
     * the scrollbar for the window. It does have insert line, use that if it
     * exists.
     */
    result_empty = (row + line_count >= end);
    ᮚᮦᮔ᮪ (wp != NULL && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ᮚᮦᮔ᮪ (line_count > 3)
	    ᮝᮜᮤ FAIL;
	type = USE_REDRAW;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_CAL != NUL && (line_count > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_CDL != NUL && result_empty && (line_count > 1 || !can_ce))
	type = USE_T_CDL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_AL != NUL)
	type = USE_T_AL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (can_ce && result_empty)
	type = USE_T_CE;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    ᮜᮤᮚ
	ᮝᮜᮤ FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ᮚᮦᮔ᮪ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	ᮝᮜᮤ screen_del_lines(off, row, line_count, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ᮚᮦᮔ᮪ (*T_DB)
	screen_del_lines(off, end - line_count, line_count, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ᮚᮦᮔ᮪ (off + row > 0 || (wp != NULL && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    ᮜᮤᮚ
	clip_scroll_selection(-line_count);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, line_count);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    ᮚᮦᮔ᮪ (wp != NULL && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ᮚᮦᮔ᮪ (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    ᮜᮤᮚ
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	ᮚᮦᮔ᮪ (wp != NULL && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    ᮞᮨᮓᮨᮜ ((j -= line_count) >= row)
		linecopy(j + line_count, j, wp);
	    j += line_count;
	    ᮚᮦᮔ᮪ (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    ᮜᮤᮚ
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	ᮜᮤᮚ
	{
	    j = end - 1 - i;
	    temp = LineOffset[j];
	    ᮞᮨᮓᮨᮜ ((j -= line_count) >= row)
	    {
		LineOffset[j + line_count] = LineOffset[j];
		LineWraps[j + line_count] = LineWraps[j];
	    }
	    LineOffset[j + line_count] = temp;
	    LineWraps[j + line_count] = FALSE;
	    ᮚᮦᮔ᮪ (can_clear((char_u *)" "))
		lineclear(temp, (puur_naa_nka)Columns, clear_attr);
	    ᮜᮤᮚ
		lineinvalid(temp, (puur_naa_nka)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    ᮚᮦᮔ᮪ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ᮚᮦᮔ᮪ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == USE_T_CAL)
    {
	term_append_lines(line_count);
	screen_start();		// don't know where cursor is now
    }
    ᮜᮤᮚ
    {
	for (i = 0; i < line_count; i++)
	{
	    ᮚᮦᮔ᮪ (type == USE_T_AL)
	    {
		ᮚᮦᮔ᮪ (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    ᮜᮤᮚ  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ᮚᮦᮔ᮪ (type == USE_T_SR && *T_DA)
    {
	for (i = 0; i < line_count; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ᮚᮦᮔ᮪ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    ᮝᮜᮤ OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    puur_naa_nka
screen_del_lines(
    puur_naa_nka		off,
    puur_naa_nka		row,
    puur_naa_nka		line_count,
    puur_naa_nka		end,
    puur_naa_nka		force,		// even when line_count > p_ttyscroll
    puur_naa_nka		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    puur_naa_nka		j;
    puur_naa_nka		i;
    unsigned	temp;
    puur_naa_nka		cursor_row;
    puur_naa_nka		cursor_col = 0;
    puur_naa_nka		cursor_end;
    puur_naa_nka		result_empty;	// result is empty until end of region
    puur_naa_nka		can_delete;	// deleting line codes can be used
    puur_naa_nka		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ᮚᮦᮔ᮪ (!screen_valid(TRUE)
	    || line_count <= 0
	    || (!force && line_count > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	ᮝᮜᮤ FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + line_count >= end;

    /*
     * We can delete lines only when 'db' flag not set or when 'ce' option
     * available.
     */
    can_delete = (*T_DB == NUL || can_clear(T_CE));

    /*
     * There are six ways to delete lines:
     * 0. When in a vertically split window and t_CV isn't set, redraw the
     *    characters from ScreenLines[].
     * 1. Use T_CD if it exists and the result is empty.
     * 2. Use newlines if row == 0 and count == 1 or T_CDL does not exist.
     * 3. Use T_CDL (delete multiple lines) if it exists and line_count > 1 or
     *	  none of the other ways work.
     * 4. Use T_CE (erase line) if the result is empty.
     * 5. Use T_DL (delete line) if it exists.
     * 6. redraw the characters from ScreenLines[].
     */
    ᮚᮦᮔ᮪ (wp != NULL && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ᮚᮦᮔ᮪ (line_count > 3)
	    ᮝᮜᮤ FAIL;
	type = USE_REDRAW;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    line_count == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_CDL != NUL && line_count > 1 && can_delete)
	type = USE_T_CDL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (can_clear(T_CE) && result_empty
	    && (wp == NULL || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    ᮜᮤᮚ
	ᮝᮜᮤ FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ᮚᮦᮔ᮪ (off + row > 0 || (wp != NULL && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    ᮜᮤᮚ
	clip_scroll_selection(line_count);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -line_count);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_row >= row + off
						&& gui.cursor_row < end + off);
#endif

    ᮚᮦᮔ᮪ (wp != NULL && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ᮚᮦᮔ᮪ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    ᮜᮤᮚ
    {
	cursor_row = row + off;
	cursor_end = end + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	ᮚᮦᮔ᮪ (wp != NULL && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    ᮞᮨᮓᮨᮜ ((j += line_count) <= end - 1)
		linecopy(j - line_count, j, wp);
	    j -= line_count;
	    ᮚᮦᮔ᮪ (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    ᮜᮤᮚ
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	ᮜᮤᮚ
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = LineOffset[j];
	    ᮞᮨᮓᮨᮜ ((j += line_count) <= end - 1)
	    {
		LineOffset[j - line_count] = LineOffset[j];
		LineWraps[j - line_count] = LineWraps[j];
	    }
	    LineOffset[j - line_count] = temp;
	    LineWraps[j - line_count] = FALSE;
	    ᮚᮦᮔ᮪ (can_clear((char_u *)" "))
		lineclear(temp, (puur_naa_nka)Columns, clear_attr);
	    ᮜᮤᮚ
		lineinvalid(temp, (puur_naa_nka)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    ᮚᮦᮔ᮪ (screen_attr != clear_attr)
	screen_stop_highlight();
    ᮚᮦᮔ᮪ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ᮚᮦᮔ᮪ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_lines(line_count);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	for (i = line_count; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    ᮜᮤᮚ
    {
	for (i = line_count; --i >= 0; )
	{
	    ᮚᮦᮔ᮪ (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    ᮜᮤᮚ // type == USE_T_CE
	    {
		windgoto(cursor_row + i, cursor_col);
		out_str(T_CE);		// erase a line
	    }
	    screen_start();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    ᮚᮦᮔ᮪ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	for (i = line_count; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ᮚᮦᮔ᮪ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    ᮝᮜᮤ OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    puur_naa_nka
skip_showmode(void)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ᮚᮦᮔ᮪ (global_busy
	    || msg_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !KeyTyped))
    {
	redraw_mode = TRUE;		// show mode later
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Show the current mode and ruler.
 *
 * If clear_cmdline is TRUE, clear the rest of the cmdline.
 * If clear_cmdline is FALSE there may be a message there that needs to be
 * cleared only if a mode is shown.
 * If redraw_mode is TRUE show or clear the mode.
 * Return the length of the message (0 if no message).
 */
    puur_naa_nka
showmode(void)
{
    puur_naa_nka		need_clear;
    puur_naa_nka		length = 0;
    puur_naa_nka		do_mode;
    puur_naa_nka		attr;
    puur_naa_nka		nwr_save;
    puur_naa_nka		sub_attr;
    puur_naa_nka		show_ruler_with_pum = FALSE;

    do_mode = p_smd && msg_silent == 0
	    && ((State & MODE_INSERT)
		|| restart_edit != NUL
		|| VIsual_active);
    ᮚᮦᮔ᮪ (do_mode || reg_recording != 0)
    {
	ᮚᮦᮔ᮪ (skip_showmode())
	    ᮝᮜᮤ 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ᮚᮦᮔ᮪ (clear_cmdline && cmdline_row < Rows - 1)
	    msg_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	msg_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ᮚᮦᮔ᮪ (do_mode)
	{
	    msg_puts_attr("--", attr);
#ᮚᮦᮔ᮪ defined(FEAT_XIM)
	    ᮚᮦᮔ᮪ (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# ᮜᮤᮚ
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		msg_puts_attr(" IM", attr);
# ᮜᮤᮚ
		msg_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    ᮚᮦᮔ᮪ (edit_submode != NULL && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - msg_row) * cmdline_width - 3;
		ᮚᮦᮔ᮪ (edit_submode_extra != NULL)
		    length -= vim_strsize(edit_submode_extra);
		ᮚᮦᮔ᮪ (length > 0)
		{
		    ᮚᮦᮔ᮪ (edit_submode_pre != NULL)
			length -= vim_strsize(edit_submode_pre);
		    ᮚᮦᮔ᮪ (length - vim_strsize(edit_submode) > 0)
		    {
			ᮚᮦᮔ᮪ (edit_submode_pre != NULL)
			    msg_puts_attr((var_na *)edit_submode_pre, attr);
			msg_puts_attr((var_na *)edit_submode, attr);
		    }
		    ᮚᮦᮔ᮪ (edit_submode_extra != NULL)
		    {
			msg_puts_attr(" ", attr);  // add a space in between
			ᮚᮦᮔ᮪ ((puur_naa_nka)edit_submode_highl < (puur_naa_nka)HLF_COUNT)
			    sub_attr = HL_ATTR(edit_submode_highl);
			ᮜᮤᮚ
			    sub_attr = attr;
			msg_puts_attr((var_na *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (State & VREPLACE_FLAG)
		    msg_puts_attr(_(" VREPLACE"), attr);
		ᮜᮤᮚ ᮚᮦᮔ᮪ (State & REPLACE_FLAG)
		    msg_puts_attr(_(" REPLACE"), attr);
		ᮜᮤᮚ ᮚᮦᮔ᮪ (State & MODE_INSERT)
		{
#ifdef FEAT_RIGHTLEFT
		    ᮚᮦᮔ᮪ (p_ri)
			msg_puts_attr(_(" REVERSE"), attr);
#endif
		    msg_puts_attr(_(" INSERT"), attr);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    msg_puts_attr(_(" (insert)"), attr);
		ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'R')
		    msg_puts_attr(_(" (replace)"), attr);
		ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'V')
		    msg_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		ᮚᮦᮔ᮪ (p_hkmap)
		    msg_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		ᮚᮦᮔ᮪ (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ᮚᮦᮔ᮪ (curwin->w_p_arab)
			msg_puts_attr(_(" Arabic"), attr);
		    ᮜᮤᮚ
# endif
			ᮚᮦᮔ᮪ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    msg_puts_attr((var_na *)NameBuff, attr);
		}
#endif
		ᮚᮦᮔ᮪ ((State & MODE_INSERT) && p_paste)
		    msg_puts_attr(_(" (paste)"), attr);

		ᮚᮦᮔ᮪ (VIsual_active)
		{
		    var_na *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    switch ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			pak_sa 0:	p = N_(" VISUAL"); bha_nga;
			pak_sa 1: p = N_(" VISUAL LINE"); bha_nga;
			pak_sa 2: p = N_(" VISUAL BLOCK"); bha_nga;
			pak_sa 4: p = N_(" SELECT"); bha_nga;
			pak_sa 5: p = N_(" SELECT LINE"); bha_nga;
			default: p = N_(" SELECT BLOCK"); bha_nga;
		    }
		    msg_puts_attr(_(p), attr);
		}
		msg_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	ᮚᮦᮔ᮪ (reg_recording != 0
		&& edit_submode == NULL)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	ᮚᮦᮔ᮪ (need_clear || clear_cmdline || redraw_mode)
	    msg_clr_eos();
	msg_didout = FALSE;		// overwrite this message
	length = msg_col;
	msg_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (clear_cmdline && msg_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	msg_clr_cmdline();
    ᮜᮤᮚ ᮚᮦᮔ᮪ (redraw_mode)
    {
	msg_pos_mode();
	msg_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ᮚᮦᮔ᮪ (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ᮚᮦᮔ᮪ (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    ᮝᮜᮤ length;
}

/*
 * Position for a mode message.
 */
    static void
msg_pos_mode(void)
{
    msg_col = 0;
    msg_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    void
unshowmode(puur_naa_nka force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ᮚᮦᮔ᮪ (!redrawing() || (!force && char_avail() && !KeyTyped))
	redraw_cmdline = TRUE;		// delete mode later
    ᮜᮤᮚ
	clearmode();
}

/*
 * Clear the mode message.
 */
    void
clearmode(void)
{
    puur_naa_nka save_msg_row = msg_row;
    puur_naa_nka save_msg_col = msg_col;

    msg_pos_mode();
    ᮚᮦᮔ᮪ (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    msg_clr_eos();

    msg_col = save_msg_col;
    msg_row = save_msg_row;
}

    static void
recording_mode(puur_naa_nka attr)
{
    msg_puts_attr(_("recording"), attr);
    ᮚᮦᮔ᮪ (shortmess(SHM_RECORDING))
	ᮝᮜᮤ;

    var_na s[4];

    sprintf(s, " @%c", reg_recording);
    msg_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    static puur_naa_nka
mouse_has_any(void)
{
    ᮝᮜᮤ mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_INSERT)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    void
draw_tabline(void)
{
    puur_naa_nka		tabcount = 0;
    tabpage_T	*tp;
    puur_naa_nka		tabwidth;
    puur_naa_nka		col = 0;
    puur_naa_nka		scol = 0;
    puur_naa_nka		attr;
    win_T	*wp;
    win_T	*cwp;
    puur_naa_nka		wincount;
    puur_naa_nka		modified;
    puur_naa_nka		c;
    puur_naa_nka		len;
    puur_naa_nka		attr_sel;
    puur_naa_nka		attr_nosel = 0;
    puur_naa_nka		attr_fill = 0;
    char_u	*p;
    puur_naa_nka		room;
    puur_naa_nka		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    ᮚᮦᮔ᮪ (ScreenLines == NULL)
	ᮝᮜᮤ;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ᮚᮦᮔ᮪ (gui_use_tabline())
    {
	gui_update_tabline();
	ᮝᮜᮤ;
    }
#endif

    ᮚᮦᮔ᮪ (tabline_height() < 1)
	ᮝᮜᮤ;

#ᮚᮦᮔ᮪ defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    ᮚᮦᮔ᮪ (*p_tal != NUL)
	win_redr_custom(NULL, FALSE);
    ᮜᮤᮚ
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	ᮚᮦᮔ᮪ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	for (tp = first_tabpage; tp != NULL && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = push_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    ᮚᮦᮔ᮪ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ᮚᮦᮔ᮪ (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    ᮚᮦᮔ᮪ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    ᮚᮦᮔ᮪ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    ᮜᮤᮚ
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    for (wincount = 0; wp != NULL; wp = wp->w_next, ++wincount)
		ᮚᮦᮔ᮪ (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    ᮚᮦᮔ᮪ (modified || wincount > 1)
	    {
		ᮚᮦᮔ᮪ (wincount > 1)
		{
		    len = vim_snprintf((var_na *)NameBuff, MAXPATHL, "%d", wincount);
		    ᮚᮦᮔ᮪ (col + len >= Columns - 3)
			bha_nga;
		    screen_puts_len(NameBuff, len, 0, col,
#ᮚᮦᮔ᮪ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#ᮜᮤᮚ
					 attr
#endif
					       );
		    col += len;
		}
		ᮚᮦᮔ᮪ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    ᮚᮦᮔ᮪ (room > 0)
	    {
		puur_naa_nka n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ᮚᮦᮔ᮪ (has_mbyte)
		    ᮞᮨᮓᮨᮜ (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		ᮜᮤᮚ ᮚᮦᮔ᮪ (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		ᮚᮦᮔ᮪ (len > n)
		    len = n;

		screen_puts_len(p, (puur_naa_nka)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    ᮞᮨᮓᮨᮜ (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    ᮚᮦᮔ᮪ (override_success)
		pop_highlight_overrides();
	}

	ᮚᮦᮔ᮪ (use_sep_chars)
	    c = '_';
	ᮜᮤᮚ
	    c = ' ';
	screen_fill(0, 1, col, (puur_naa_nka)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ᮚᮦᮔ᮪ (p_sc && *p_sloc == 't')
	{
	    puur_naa_nka	n = (puur_naa_nka)Columns - col - (tabcount > 1) * 3;
	    puur_naa_nka	width = MIN(10, n);

	    ᮚᮦᮔ᮪ (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (puur_naa_nka)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ᮚᮦᮔ᮪ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (puur_naa_nka)Columns - 1, attr_nosel);
	    TabPageIdxs[Columns - 1] = -999;
	}
    }

    // Reset the flag here again, in case evaluating 'tabline' causes it to be
    // set.
    redraw_tabline = FALSE;
}

/*
 * Get buffer name for "buf" into NameBuff[].
 * Takes care of special buffer names and translates special characters.
 */
    void
get_trans_bufname(buf_T *buf)
{
    ᮚᮦᮔ᮪ (buf_spname(buf) != NULL)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    ᮜᮤᮚ
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    puur_naa_nka
fillchar_status(puur_naa_nka *attr, win_T *wp)
{
    puur_naa_nka	    fill;
    bool    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    ᮚᮦᮔ᮪ (bt_terminal(wp->w_buffer))
    {
	ᮚᮦᮔ᮪ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	ᮜᮤᮚ
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    ᮜᮤᮚ
#endif
    ᮚᮦᮔ᮪ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    ᮜᮤᮚ
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();
    ᮝᮜᮤ fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    static bool
vsep_row_is_curwin(win_T *wp, puur_naa_nka row)
{
    ᮚᮦᮔ᮪ (wp == curwin)
	ᮝᮜᮤ true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ᮚᮦᮔ᮪ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	ᮝᮜᮤ true;

    ᮝᮜᮤ false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    puur_naa_nka
fillchar_vsep(puur_naa_nka *attr, win_T *wp, puur_naa_nka row)
{
    bool override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ᮚᮦᮔ᮪ (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    ᮜᮤᮚ
	*attr = HL_ATTR(HLF_CNC);
    ᮚᮦᮔ᮪ (override_success)
	pop_highlight_overrides();

    ᮚᮦᮔ᮪ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	ᮝᮜᮤ '|';
    ᮜᮤᮚ
	ᮝᮜᮤ wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    puur_naa_nka
redrawing(void)
{
#ifdef FEAT_EVAL
    ᮚᮦᮔ᮪ (disable_redraw_for_testing)
	ᮝᮜᮤ 0;
    ᮜᮤᮚ
#endif
	ᮝᮜᮤ ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !KeyTyped && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    puur_naa_nka
messaging(void)
{
    ᮝᮜᮤ (!(p_lz && char_avail() && !KeyTyped));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    void
comp_col(void)
{
    puur_naa_nka last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    ᮚᮦᮔ᮪ (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#ᮜᮤᮚ
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	ᮚᮦᮔ᮪ (!last_has_status)
	    sc_col = ru_col;
    }
    ᮚᮦᮔ᮪ (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	ᮚᮦᮔ᮪ (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    ᮚᮦᮔ᮪ (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    ᮚᮦᮔ᮪ (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#ᮚᮦᮔ᮪ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    puur_naa_nka
number_width(win_T *wp)
{
    puur_naa_nka		n;
    linenr_T	lnum;

    ᮚᮦᮔ᮪ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    ᮜᮤᮚ
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_line_count;

    ᮚᮦᮔ᮪ (lnum == wp->w_nrwidth_line_count && wp->w_nuw_cached == wp->w_p_nuw)
	ᮝᮜᮤ wp->w_nrwidth_width;
    wp->w_nrwidth_line_count = lnum;

    n = 0;
    do
    {
	lnum /= 10;
	++n;
    } ᮞᮨᮓᮨᮜ (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ᮚᮦᮔ᮪ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ᮚᮦᮔ᮪ (n < 2 && get_first_valid_sign(wp) != NULL
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    ᮝᮜᮤ n;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    puur_naa_nka
screen_screencol(void)
{
    ᮝᮜᮤ screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    puur_naa_nka
screen_screenrow(void)
{
    ᮝᮜᮤ screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    static puur_naa_nka
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ᮚᮦᮔ᮪ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T num = 0;
	puur_naa_nka	    bytes;
	puur_naa_nka	    n;

	for (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ᮚᮦᮔ᮪ (n < 0)
		ᮝᮜᮤ 0;
	    num = num * 256 + n;
	}
	*p += 2;
	ᮝᮜᮤ num;
    }
    ᮝᮜᮤ mb_ptr2char_adv(p);
}

sa_nracanaa charstab
{
    puur_naa_nka	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

static fill_chars_T fill_chars;
static sa_nracanaa charstab filltab[] =
{
    CHARSTAB_ENTRY(&fill_chars.stl,	    "stl"),
    CHARSTAB_ENTRY(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_ENTRY(&fill_chars.vert,	    "vert"),
    CHARSTAB_ENTRY(&fill_chars.fold,	    "fold"),
    CHARSTAB_ENTRY(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_ENTRY(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_ENTRY(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_ENTRY(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_ENTRY(&fill_chars.diff,	    "diff"),
    CHARSTAB_ENTRY(&fill_chars.eob,	    "eob"),
    CHARSTAB_ENTRY(&fill_chars.lastline,    "lastline"),
#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
static lcs_chars_T lcs_chars;
static sa_nracanaa charstab lcstab[] =
{
    CHARSTAB_ENTRY(&lcs_chars.eol,	    "eol"),
    CHARSTAB_ENTRY(&lcs_chars.ext,	    "extends"),
    CHARSTAB_ENTRY(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_ENTRY(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_ENTRY(&lcs_chars.space,	    "space"),
    CHARSTAB_ENTRY(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_ENTRY(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_ENTRY(&lcs_chars.trail,	    "trail"),
    CHARSTAB_ENTRY(&lcs_chars.lead,	    "lead"),
#ifdef FEAT_CONCEAL
    CHARSTAB_ENTRY(&lcs_chars.conceal,	    "conceal"),
#ᮜᮤᮚ
    CHARSTAB_ENTRY(NULL,		    "conceal"),
#endif
    CHARSTAB_ENTRY(NULL,		    "multispace"),
    CHARSTAB_ENTRY(NULL,		    "leadmultispace")
};

    static var_na *
field_value_err(var_na *errbuf, size_t errbuflen, var_na *fmt, char_u *field)
{
    ᮚᮦᮔ᮪ (errbuf == NULL)
	ᮝᮜᮤ "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    ᮝᮜᮤ errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    static var_na *
set_chars_option(win_T *wp, char_u *value, puur_naa_nka is_listchars, puur_naa_nka apply,
						var_na *errbuf, size_t errbuflen)
{
    puur_naa_nka	    round, i, entries;
    char_u  *p, *s;
    puur_naa_nka	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = NULL;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = NULL; // Last occurrence of "leadmultispace:"
    puur_naa_nka	    multispace_len = 0;	      // Length of lcs-multispace string
    puur_naa_nka	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    sa_nracanaa charstab *tab;

    ᮚᮦᮔ᮪ (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	ᮚᮦᮔ᮪ (wp->w_p_lcs[0] == NUL)
	    value = p_lcs;  // local value is empty, use the global value
    }
    ᮜᮤᮚ
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	ᮚᮦᮔ᮪ (wp->w_p_fcs[0] == NUL)
	    value = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    for (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	puur_naa_nka has_tab = FALSE, has_leadtab = FALSE;

	ᮚᮦᮔ᮪ (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ᮚᮦᮔ᮪ (is_listchars)
	    {
		for (i = 0; i < entries; ++i)
		    ᮚᮦᮔ᮪ (tab[i].cp != NULL)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ᮚᮦᮔ᮪ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(puur_naa_nka, multispace_len + 1);
		    ᮚᮦᮔ᮪ (lcs_chars.multispace != NULL)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		ᮜᮤᮚ
		    lcs_chars.multispace = NULL;

		ᮚᮦᮔ᮪ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(puur_naa_nka, lead_multispace_len + 1);
		    ᮚᮦᮔ᮪ (lcs_chars.leadmultispace != NULL)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		ᮜᮤᮚ
		    lcs_chars.leadmultispace = NULL;
	    }
	    ᮜᮤᮚ
	    {
		fill_chars.stl = ' ';
		fill_chars.stlnc = ' ';
		fill_chars.vert = '|';
		fill_chars.fold = '-';
		fill_chars.foldopen = '-';
		fill_chars.foldclosed = '+';
		fill_chars.foldsep = '|';
		fill_chars.foldinner = NUL;
		fill_chars.diff = '-';
		fill_chars.eob = '~';
		fill_chars.lastline = '@';
#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = value;
	ᮞᮨᮓᮨᮜ (*p)
	{
	    for (i = 0; i < entries; ++i)
	    {
		ᮚᮦᮔ᮪ (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    continue;

		s = p + tab[i].name.length + 1;

		ᮚᮦᮔ᮪ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ᮚᮦᮔ᮪ (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			ᮞᮨᮓᮨᮜ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ᮚᮦᮔ᮪ (char2cells(c1) > 1)
				ᮝᮜᮤ field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ᮚᮦᮔ᮪ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    ᮜᮤᮚ
		    {
			puur_naa_nka multispace_pos = 0;

			ᮞᮨᮓᮨᮜ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ᮚᮦᮔ᮪ (p == last_multispace && lcs_chars.multispace != NULL)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    bha_nga;
		}

		ᮚᮦᮔ᮪ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ᮚᮦᮔ᮪ (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			ᮞᮨᮓᮨᮜ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ᮚᮦᮔ᮪ (char2cells(c1) > 1)
				ᮝᮜᮤ field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ᮚᮦᮔ᮪ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    ᮜᮤᮚ
		    {
			puur_naa_nka multispace_pos = 0;

			ᮞᮨᮓᮨᮜ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ᮚᮦᮔ᮪ (p == last_lmultispace && lcs_chars.leadmultispace != NULL)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    bha_nga;
		}

		c2 = c3 = 0;
		ᮚᮦᮔ᮪ (*s == NUL)
		    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ᮚᮦᮔ᮪ (char2cells(c1) > 1)
		    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		ᮚᮦᮔ᮪ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ᮚᮦᮔ᮪ (*s == NUL)
			ᮝᮜᮤ field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ᮚᮦᮔ᮪ (char2cells(c2) > 1)
			ᮝᮜᮤ field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    ᮚᮦᮔ᮪ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ᮚᮦᮔ᮪ (char2cells(c3) > 1)
			    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    ᮚᮦᮔ᮪ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    ᮜᮤᮚ  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ᮚᮦᮔ᮪ (*s == ',' || *s == NUL)
		{
		    ᮚᮦᮔ᮪ (round > 0)
		    {
			ᮚᮦᮔ᮪ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			ᮜᮤᮚ ᮚᮦᮔ᮪ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			ᮜᮤᮚ ᮚᮦᮔ᮪ (tab[i].cp != NULL)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    bha_nga;
		}
		ᮜᮤᮚ
		    ᮝᮜᮤ field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    ᮚᮦᮔ᮪ (i == entries)
		ᮝᮜᮤ e_invalid_argument;

	    ᮚᮦᮔ᮪ (*p == ',')
		++p;
	}

	ᮚᮦᮔ᮪ (is_listchars && has_leadtab && !has_tab)
	    ᮝᮜᮤ e_leadtab_requires_tab;
    }

    ᮚᮦᮔ᮪ (apply)
    {
	ᮚᮦᮔ᮪ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	ᮜᮤᮚ
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    ᮝᮜᮤ NULL;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    var_na *
set_fillchars_option(win_T *wp, char_u *val, puur_naa_nka apply, var_na *errbuf,
							      size_t errbuflen)
{
    ᮝᮜᮤ set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    var_na *
set_listchars_option(win_T *wp, char_u *val, puur_naa_nka apply, var_na *errbuf,
							      size_t errbuflen)
{
    ᮝᮜᮤ set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᮚᮦᮔ᮪ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(filltab))
	ᮝᮜᮤ NULL;

    ᮝᮜᮤ filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᮚᮦᮔ᮪ (idx < 0 || idx >= (puur_naa_nka)ARRAY_LENGTH(lcstab))
	ᮝᮜᮤ NULL;

    ᮝᮜᮤ lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    var_na *
check_chars_options(void)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ᮚᮦᮔ᮪ (set_listchars_option(curwin, p_lcs, FALSE, NULL, 0) != NULL)
	ᮝᮜᮤ e_conflicts_with_value_of_listchars;
    ᮚᮦᮔ᮪ (set_fillchars_option(curwin, p_fcs, FALSE, NULL, 0) != NULL)
	ᮝᮜᮤ e_conflicts_with_value_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ᮚᮦᮔ᮪ (set_listchars_option(wp, wp->w_p_lcs, FALSE, NULL, 0) != NULL)
	    ᮝᮜᮤ e_conflicts_with_value_of_listchars;
	ᮚᮦᮔ᮪ (set_fillchars_option(wp, wp->w_p_fcs, FALSE, NULL, 0) != NULL)
	    ᮝᮜᮤ e_conflicts_with_value_of_fillchars;
    }
    ᮝᮜᮤ NULL;
}
