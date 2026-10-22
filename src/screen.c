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
static цео	screen_attr = 0;

static void screen_char_2(unsigned off, цео row, цео col);
static цео screenclear2(цео doclear);
static void lineclear(unsigned off, цео width, цео attr);
static void lineinvalid(unsigned off, цео width);
static цео win_do_lines(win_T *wp, цео row, цео line_count, цео mayclear, цео del, цео clear_attr);
static void win_rest_invalid(win_T *wp);
static void msg_pos_mode(void);
static void recording_mode(цео attr);

// Ugly global: overrule attribute used by screen_char()
static цео screen_char_attr = 0;

#ако defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    цео
conceal_cursor_line(win_T *wp)
{
    цео		c;

    ако (*wp->w_p_cocu == NUL)
	врати FALSE;
    ако (get_real_state() & MODE_VISUAL)
	c = 'v';
    инако ако (State & MODE_INSERT)
	c = 'i';
    инако ако (State & MODE_NORMAL)
	c = 'n';
    инако ако (State & MODE_CMDLINE)
	c = 'c';
    инако
	врати FALSE;
    врати vim_strchr(wp->w_p_cocu, c) != NULL;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    void
conceal_check_cursor_line(цео was_concealed)
{
    ако (curwin->w_p_cole <= 0 || conceal_cursor_line(curwin) == was_concealed)
	врати;

    цео wcol = curwin->w_wcol;

    need_cursor_line_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ако (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    цео
get_win_attr(win_T *wp)
{
    цео	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ако (win_attr != 0)
    {
	ако (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	инако
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    инако ако (WIN_IS_POPUP(wp))
    {
	ако (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	инако
	    win_attr = HL_ATTR(HLF_POP);    // Popup
    }
#endif

    ако (override_success)
	pop_highlight_overrides();
    врати win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    static цео
screen_fill_end(
	win_T *wp,
	цео	c1,
	цео	c2,
	цео	off,
	цео	width,
	цео	row,
	цео	endrow,
	цео	attr)
{
    цео	    nn = off + width;

    ако (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    ако (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (цео)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    инако
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (цео)wp->w_wincol + nn,
		c1, c2, attr);
    врати nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    void
win_draw_end(
    win_T	*wp,
    цео		c1,
    цео		c2,
    цео		draw_margin,
    цео		row,
    цео		endrow,
    hlf_T	hl)
{
    цео		n = 0;
    цео		attr = HL_ATTR(hl);
    цео		win_attr = get_win_attr(wp);
    bool	override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ако (draw_margin)
    {
#ifdef FEAT_FOLDING
	цео	fdc = compute_foldcolumn(wp, 0);

	ако (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	ако (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	ако ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == NULL)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, number_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    ако (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    инако
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (цео)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    ако (override_success)
	pop_highlight_overrides();
}

#ако defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    цео
compute_foldcolumn(win_T *wp, цео col)
{
    цео wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    цео n = wp->w_width - (col + wmw);

    врати MIN(wp->w_p_fdc, n);
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
    цео		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    цео		i = 0;
    цео		level;
    цео		first_level;
    цео		empty;
    цео		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    цео		symbol = 0;
    цео		len = 0;
    цео		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    ако (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    за (i = 0; i < n; i++)
    {
	ако (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_low_level)
	    symbol = wp->w_fill_chars.foldopen;
	инако ако (first_level == 1)
	    symbol = wp->w_fill_chars.foldsep;
	инако ако (wp->w_fill_chars.foldinner != NUL)
	    symbol = wp->w_fill_chars.foldinner;
	инако ако (first_level + i <= 9)
	    symbol = '0' + first_level + i;
	инако
	    symbol = '>';

	len = utf_char2bytes(symbol, &p[byte_counter]);
	byte_counter += len;
	ако (first_level + i >= level)
	{
	    i++;
	    break;
	}
    }

    ако (closed)
    {
	ако (symbol != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ако (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	symbol = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(symbol, &p[byte_counter]);
	byte_counter += len;
    }

    врати MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    static цео
comp_char_differs(цео off_from, цео off_to)
{
    цео	    i;

    за (i = 0; i < Screen_mco; ++i)
    {
	ако (ScreenLinesC[i][off_from] != ScreenLinesC[i][off_to])
	    врати TRUE;
	ако (ScreenLinesC[i][off_from] == 0)
	    break;
    }
    врати FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    static цео
char_needs_redraw(цео off_from, цео off_to, цео cols)
{
    ако (cols > 0
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
	врати TRUE;
    врати FALSE;
}

#ако defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    цео
screen_get_current_line_off(void)
{
    врати (цео)(current_ScreenLine - ScreenLines);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    static цео
blocked_by_popup(цео row, цео col)
{
    цео off;

    ако (!popup_visible)
	врати FALSE;
    off = row * screen_Columns + col;
    врати popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    void
reset_screen_attr(void)
{
#ifdef FEAT_GUI
    ако (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    инако
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    static цео
skip_for_popup(цео row, цео col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ако (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	врати TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    ако (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	врати TRUE;
#инако
    ако (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	врати TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    ако (blocked_by_popup(row, col))
	врати TRUE;
#endif
    врати FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
static char_u	*suppressed_cells = NULL;
static цео	suppressed_rows = 0;
static цео	suppressed_cols = 0;

    static void
mark_suppressed_cell(цео row, цео col)
{
    ако (suppressed_cells == NULL
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	ако (suppressed_cells == NULL)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    врати;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    static void
unmark_suppressed_cell(цео row, цео col)
{
    ако (suppressed_cells != NULL
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    static цео
is_suppressed_cell(цео row, цео col)
{
    врати suppressed_cells != NULL
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
typedef struct {
    цео		popup_attr;	// get_win_attr(screen_opacity_popup)
    цео		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    static inline void
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    static inline цео
popup_base_attr_or(цео row, цео col, цео fallback)
{
    цео		base = fallback;

    popup_get_base_screen_cell(row, col, NULL, &base, NULL);
    врати base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    static inline цео
popup_blend_with_base(
	const popup_opacity_T	*po,
	цео			char_attr,
	цео			base_attr,
	цео			blend_fg)
{
    цео		combined = hl_combine_attr(po->popup_attr, char_attr);

    врати hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    static void
resolve_wide_char_opacity_attrs(
	цео row, цео col1, цео col2,
	sattr_T *attr1, sattr_T *attr2)
{
    цео		bg1, bg2;
    цео		base1 = 0;
    цео		base2 = 0;
    attrentry_T	*ae;

    ако (*attr1 == *attr2)
	врати;

    popup_get_base_screen_cell(row, col1, NULL, &base1, NULL);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != NULL && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# инако
    bg1 = (ae != NULL && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, NULL, &base2, NULL);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != NULL && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# инако
    bg2 = (ae != NULL && ae->ae_u.cterm.bg_color != 0);
# endif

    ако (bg1 && !bg2)
	*attr1 = *attr2;
    инако ако (!bg1 && bg2)
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
	цео	row,
	цео	coloff,
	цео	endcol,
	цео	clear_width,
	colnr_T	last_vcol,
	цео	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    цео		    col = 0;
    цео		    hl;
    цео		    force = FALSE;	// force update rest of the line
    цео		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    цео		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    цео		    changed_this;	// TRUE if character changed
    цео		    changed_next;	// TRUE if next character changed
#endif
    цео		    clear_next = FALSE;
    цео		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    цео		    drawing_opacity_popup =
				screen_opacity_popup != NULL
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    ако (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    ако (row >= Rows)
	row = Rows - 1;
    ако (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_ScreenLine - ScreenLines);
    off_to = LineOffset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = LineOffset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    ако (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ако (clear_width > 0)
	{
	    цео clear_start = col;

	    док (col <= endcol && ScreenLines[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || ScreenLinesUC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    ако (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    за (цео i = endcol; i >= clear_start; i--)
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
    ако (coloff > 0 && enc_utf8
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
	ако (screen_opacity_popup == NULL)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    док (col < endcol)
    {
	ако (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	инако
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ако (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	ако (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    ако (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ако (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	ако (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ако (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	цео opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ако (drawing_opacity_popup
		&& ScreenLines[off_from] == ' '
		&& (!enc_utf8 || ScreenLinesUC[off_from] == 0))
	{
	    цео popup_src_attr = ScreenAttrs[off_from];
	    цео bg_is_empty = ScreenLines[off_to] == 0
				&& (!enc_utf8 || ScreenLinesUC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ако (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && ScreenLinesUC[off_to - 1] != 0
		    && utf_char2cells(ScreenLinesUC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    инако ако (ScreenLines[off_to] != 0
		    || (enc_utf8 && ScreenLinesUC[off_to] != 0))
	    {
		цео bg_char_cells = (enc_utf8 && ScreenLinesUC[off_to] != 0)
				? utf_char2cells(ScreenLinesUC[off_to]) : 1;

		ако (bg_char_cells == 2)
		{
		    ако (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			цео base_attr;
			ScreenLines[off_to] = ' ';
			ако (enc_utf8)
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
		    ако (!(ScreenLines[off_from + 1] == ' '
			    && (!enc_utf8 || ScreenLinesUC[off_from + 1] == 0)))
			goto skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		цео base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ако (bg_char_cells == 2)
		{
		    цео base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    ако (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    инако ако (bg_is_empty)
	    {
		цео base_attr;
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

	ако (redraw_this)
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
	    ако (       p_wiv
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
		ако (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		инако
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ако (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ако (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    ScreenLines[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		инако ако (char_cells == 2
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

		ако (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ако (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    ScreenLines[off_to] = ScreenLines[off_from];
	    ако (enc_utf8)
	    {
		ScreenLinesUC[off_to] = ScreenLinesUC[off_from];
		ако (ScreenLinesUC[off_from] != 0)
		{
		    цео	    i;

		    за (i = 0; i < Screen_mco; ++i)
			ScreenLinesC[i][off_to] = ScreenLinesC[i][off_from];
		}
	    }
	    ако (char_cells == 2)
	    {
		ScreenLines[off_to + 1] = ScreenLines[off_from + 1];
		ако (enc_utf8)
		    ScreenLinesUC[off_to + 1] = 0;
	    }

#ако defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ако (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ако defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		ако (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ако (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ако (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ако (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ако (drawing_opacity_popup && po.blend > 0)
	    {
		цео popup_src_attr = ScreenAttrs[off_from];
		цео scol1 = col + coloff;
		цео base1 = popup_base_attr_or(row, scol1, 0);

		ако (char_cells == 2)
		{
		    цео base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    цео over1 = popup_is_over_opacity(row, scol1);
		    цео over2 = popup_is_over_opacity(row, scol1 + 1);
		    ако (over1 != over2)
		    {
			ако (over1)
			    base1 = base2;
			инако
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    инако
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ако (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    ако (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    инако
		screen_char(off_to, row, col + coloff);
	}
	инако ако (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    ако (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    инако ако (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	ако (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    ако (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    ако (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	ScreenLines[off_to] = ' ';
	ако (enc_utf8)
	    ScreenLinesUC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	ако (screen_opacity_popup == NULL)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    ако (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	цео startCol = col;
#endif

	// blank out the rest of the line
	док (col < clear_width && ScreenLines[off_to] == ' '
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
	ако (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ако (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		ако (hl > HL_ALL || (hl & HL_BOLD))
		{
		    цео prev_cells = 1;

		    ако (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = ScreenLines[off_to - 1] == 0 ? 2 : 1;
		    инако ако (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = LineOffset[row];
			unsigned max_off = LineOffset[row] + screen_Columns;

			док (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ако (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			ако (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			инако
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    док (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    ако (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ако (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    ако (!skip_for_popup(row, col + coloff))
	    {
		цео c;

		c = fillchar_vsep(&hl, wp, row);
		ако (ScreenLines[off_to] != (schar_T)c
			|| (enc_utf8 && (цео)ScreenLinesUC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    ScreenLines[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    ако (enc_utf8)
		    {
			ако (c >= 0x80)
			{
			    ScreenLinesUC[off_to] = c;
			    ScreenLinesC[0][off_to] = 0;
			    ScreenLines[off_to] = 0x80; // avoid storing zero
			}
			инако
			    ScreenLinesUC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	инако
	    LineWraps[row] = FALSE;
    }

    ако (override_success)
	pop_highlight_overrides();
}

#ако defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    void
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    цео		t;

    за (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
draw_vsep_win(win_T *wp, цео row)
{
    ако (!wp->w_vsep_width)
	врати;

    цео content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    цео start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    за (цео r = start_row; r < content_end; ++r)
    {
	цео hl;
	цео c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ако (wp->w_status_height != 0)
    {
	цео hl;
	цео c;
	ако (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	инако
	    c = fillchar_vsep(&hl, wp, content_end);
	за (цео r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    цео
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    док (fr->fr_parent != NULL)
    {
	ако (fr->fr_parent->fr_layout == FR_COL)
	{
	    ако (fr->fr_next != NULL)
		break;
	}
	инако
	{
	    ако (fr->fr_next != NULL)
		врати TRUE;
	}
	fr = fr->fr_parent;
    }
    врати FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    цео
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    цео		len)	    // length of buffer
{
    char_u	*p;
    цео		plen;

    ако (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	врати 0;

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
    ако (p == NULL || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	ако (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	инако
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((char *)buf, len, (char *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    ако (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    врати plen;
}

#ако defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    void
win_redr_custom(
    win_T	*wp,
    цео		draw_ruler)	// TRUE or FALSE
{
    static цео	entered = FALSE;
    цео		attr;
    цео		curattr;
    цео		row;
    цео		col = 0;
    цео		maxwidth;
    цео		width;
    цео		n;
    цео		len;
    цео		stlh_cnt;
    цео		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    цео		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    цео		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ако (entered)
	врати;
    entered = TRUE;

    // setup environment for the task at hand
    ако (wp == NULL)
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
    инако
    {
	override_success = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	цео in_status_line = wp->w_status_height != 0;
	maxwidth = in_status_line ? wp->w_width : cmdline_width;

	ако (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ако (*stl == '%')
	    {
		ако (*++stl == '-')
		    stl++;
		ако (atoi((char *)stl))
		    док (VIM_ISDIGIT(*stl))
			stl++;
		ако (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    ако (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    ако (!in_status_line)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	инако
	{
	    opt_name = (char_u *)"statusline";
	    ако (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    инако
		stl = p_stl;
	}

	ако (in_status_line)
	    col += wp->w_wincol;
	инако
	    col += cmdline_col_off;
    }

    ако (maxwidth <= 0)
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
    цео col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = NULL;
    цео			*out_count = NULL;
    цео			region_base_col = 0;
    ако (!draw_ruler)
    {
	ако (wp != NULL)
	{
	    out_regions = &wp->w_stl_click;
	    out_count = &wp->w_stl_click_count;
	    region_base_col = wp->w_wincol;
	}
	инако
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_count = &tabline_stl_click_count;
	    region_base_col = firstwin->w_wincol;
	}

	за (n = 0; n < *out_count; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_count = 0;
    }

    цео carry_hl = 0;
    за (цео i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, sizeof(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	ако (p != NULL)
	{
	    len = vim_snprintf((char *)buf, sizeof(buf), "%s", p);
	    vim_free(p);
	}
	инако
	    len = (цео)STRLEN(buf);

	// fill up with "fillchar"
	док (width < maxwidth && len < (цео)sizeof(buf) - 1)
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
	за (n = 0; hltab[n].start != NULL; n++)
	{
	    len = (цео)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    ако (hltab[n].userhl == 0)
		curattr = attr;
	    инако ако (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    инако ако (wp != NULL && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    инако ако (wp != NULL && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    инако ако (wp != NULL && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    инако
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ако (out_regions != NULL)
	{
	    цео click_count = 0;
	    за (n = 0; clicktab[n].start != NULL; n++)
		click_count++;

	    ако (click_count > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			sizeof(stl_click_region_T)
				    * (*out_count + click_count));
		ако (new_arr != NULL)
		{
		    char_u  *cur_funcname = NULL;
		    цео	    cur_minwid = 0;
		    цео	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    за (n = 0; clicktab[n].start != NULL; n++)
		    {
			len += vim_strnsize(p,
					    (цео)(clicktab[n].start - p));
			p = clicktab[n].start;

			ако (cur_funcname != NULL)
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
		    ако (cur_funcname != NULL)
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
	за (n = 0; clicktab[n].start != NULL; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ако (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ако (stl != NULL)
	vim_free(stl_tmp);

    ако (wp == NULL)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	за (n = 0; tabtab[n].start != NULL; n++)
	{
	    len += vim_strnsize(p, (цео)(tabtab[n].start - p));
	    док (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	док (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    ако (override_success)
	pop_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    void
screen_putchar(цео c, цео row, цео col, цео attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ако (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    инако
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
screen_getbytes(цео row, цео col, char_u *bytes, цео *attrp)
{
    unsigned off;

    // safety check
    ако (ScreenLines == NULL || row >= screen_Rows || col >= screen_Columns)
	врати;

    off = LineOffset[row] + col;
    ако (attrp != NULL)
	*attrp = ScreenAttrs[off];
    bytes[0] = ScreenLines[off];
    bytes[1] = NUL;

    ако (enc_utf8 && ScreenLinesUC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    инако ако (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
    {
	bytes[0] = ScreenLines[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    инако ако (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
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
    static цео
screen_comp_differs(цео off, цео *u8cc)
{
    цео	    i;

    за (i = 0; i < Screen_mco; ++i)
    {
	ако (ScreenLinesC[i][off] != (u8char_T)u8cc[i])
	    врати TRUE;
	ако (u8cc[i] == 0)
	    break;
    }
    врати FALSE;
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
    цео		row,
    цео		col,
    цео		attr)
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
    цео		textlen,
    цео		row,
    цео		col,
    цео		attr_arg)
{
    цео		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    цео		len = textlen;
    цео		c;
    unsigned	max_off;
    цео		mbyte_blen = 1;
    цео		mbyte_cells = 1;
    цео		u8c = 0;
    цео		u8cc[MAX_MCO];
    цео		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    цео		prev_c = 0;		// previous Arabic character
    цео		pc, nc, nc1;
    цео		pcc[MAX_MCO];
#endif
    цео		force_redraw_this;
    цео		force_redraw_next = FALSE;
    цео		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ако (ScreenLines == NULL
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	врати;
    off = LineOffset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ако (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ако (!skip_for_popup(row, col - 1))
	{
	    ScreenLines[off - 1] = ' ';
	    ако (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    инако ако (enc_utf8)
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
    док (col < screen_Columns
	    && (len < 0 || (цео)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ако (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (цео)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ако (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    инако ако (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    инако	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (цео)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		ако (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ако (len >= 0 && (цео)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    инако
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(цео)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		инако
		    prev_c = u8c;
#endif
		ако (col + mbyte_cells > screen_Columns)
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
	цео cell_attr = attr;
	ако (screen_pum_blend > 0 && pum_bg_attrs != NULL
		&& row >= pum_bg_top && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    цео soff = (row - pum_bg_top) * pum_bg_cols + col;
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

	ако ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#ако defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ако (need_redraw && ScreenLines[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ако defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		цео	n = ScreenAttrs[off];

		ако (n > HL_ALL)
		    n = syn_attr2attr(n);
		ако (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ако (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ако (clear_next_cell)
		clear_next_cell = FALSE;
	    инако ако (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ако (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		ScreenLines[off + mbyte_blen] = 0;
	    ScreenLines[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    ако (enc_utf8)
	    {
		ако (c < 0x80 && u8cc[0] == 0)
		    ScreenLinesUC[off] = 0;
		инако
		{
		    цео	    i;

		    ScreenLinesUC[off] = u8c;
		    за (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][off] = u8cc[i];
			ако (u8cc[i] == 0)
			    break;
		    }
		}
		ако (mbyte_cells == 2)
		{
		    ScreenLines[off + 1] = 0;
		    ScreenLinesUC[off + 1] = 0;
		    за (цео ci = 0; ci < Screen_mco; ++ci)
			ScreenLinesC[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    инако ако (mbyte_cells == 2)
	    {
		ScreenLines[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    инако ако (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    инако
		screen_char(off, row, col);
	}
	ако (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    ако (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	инако
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ако (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	ако (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	инако
	    screen_char(off, row, col);
    }
}

#ако defined(FEAT_SEARCH_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    void
start_search_hl(void)
{
    ако (!p_hls || no_hlsearch)
	врати;

    end_search_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_search_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    void
end_search_hl(void)
{
    ако (screen_search_hl.rm.regprog == NULL)
	врати;

    vim_regfree(screen_search_hl.rm.regprog);
    screen_search_hl.rm.regprog = NULL;
}
#endif

    static void
screen_start_highlight(цео attr)
{
    attrentry_T *aep = NULL;

    screen_attr = attr;
    ако (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	врати;

#ifdef FEAT_GUI
    ако (gui.in_use)
    {
	char	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	врати;
    }
#endif

    ако (attr > HL_ALL)				// special HL attr.
    {
	ако (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	инако
	    aep = syn_term_attr2entry(attr);
	ако (aep == NULL)	    // did ":syntax clear"
	    attr = 0;
	инако
	    attr = aep->ae_attr;
    }
#ако defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ако (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	цео	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ако (p_tgc)
	{
	    ако (aep == NULL || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    ако (aep == NULL || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	инако ако (t_colors >= 256)
	{
	    ако (aep == NULL || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    ако (aep == NULL || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    ако ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    инако ако (aep != NULL && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    ако ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    ако ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    ако ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    ако ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    ако ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    ако (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    ако ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    ако ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    ако ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ако (aep != NULL)
    {
	ако (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ако (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ако (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	инако
#endif
	    ако (t_colors > 1)
	    {
		ако (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ако (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ако (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	инако
#endif
	    ако (t_colors > 1)
	    {
		ако (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ако (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ако (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	инако
#endif
	    ако (t_colors > 1)
	    {
		ако (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ако (!IS_CTERM)
	{
	    ако (aep->ae_u.term.start != NULL)
		out_str(aep->ae_u.term.start);
	}
    }
}

    void
screen_stop_highlight(void)
{
    цео	    do_ME = FALSE;	    // output T_ME code
#ако defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    цео	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    ако (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	ако (gui.in_use)
	{
	    char	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	инако
#endif
	{
	    цео is_under;

	    ако (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ако (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ако (aep != NULL && ((
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
#ако defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ако (use_vtp())
		    {
			ако (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ако (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			ако (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    инако
		    {
			// Process FG and BG at once.
			ако (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		инако
		{
		    aep = syn_term_attr2entry(screen_attr);
		    ако (aep != NULL && aep->ae_u.term.stop != NULL)
		    {
			ако (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			инако
			    out_str(aep->ae_u.term.stop);
		    }
		}
		ако (aep == NULL)	    // did ":syntax clear"
		    screen_attr = 0;
		инако
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ако (screen_attr & HL_STANDOUT)
	    {
		ако (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		инако
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ако (is_under && *T_UCE != NUL)
	    {
		ако (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		инако
		    out_str(T_UCE);
	    }
	    ако ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ако (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		инако
		    out_str(T_UE);
	    }
	    ако (screen_attr & HL_ITALIC)
	    {
		ако (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		инако
		    out_str(T_CZR);
	    }
	    ако (screen_attr & HL_STRIKETHROUGH)
	    {
		ако (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		инако
		    out_str(T_STE);
	    }
	    ако (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    ако (p_tgc)
	    {
		ако (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		ако (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		ако (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    инако
#endif
	    {
		ако (t_colors > 1)
		{
		    // set Normal cterm colors
		    ако (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    ако (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    ако (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    ако (cterm_normal_fg_bold)
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
    ако (!IS_CTERM)
	врати;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    ако (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#инако
	ако (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    ако (cterm_normal_fg_bold)
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
screen_char(unsigned off, цео row, цео col)
{
    цео		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ако (row >= screen_Rows || col >= screen_Columns)
	врати;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ако (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ако (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    цео off2 = off + 1;
	    ScreenLines[off2] = ' ';
	    ScreenLinesUC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	ако (enc_utf8 && ScreenLinesUC[off] != 0
		&& utf_char2cells(ScreenLinesUC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	врати;
    }
    ако (enc_utf8 && ScreenLinesUC[off] != 0
	    && utf_char2cells(ScreenLinesUC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	врати;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ако (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	врати;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ако (screen_char_attr != 0)
	attr = screen_char_attr;
    инако
	attr = ScreenAttrs[off];
    ако (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    ако (enc_utf8 && ScreenLinesUC[off] != 0
	    && utf_char2cells(ScreenLinesUC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    ако (screen_attr != attr)
	screen_start_highlight(attr);

    ако (enc_utf8 && ScreenLinesUC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ако (
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
	инако ако (utf_ambiguous_width(ScreenLinesUC[off]))
	{
	    ако (*p_ambw == 'd'
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
	инако ако (utf_char2cells(ScreenLinesUC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    инако
    {
	out_flush_check();
	out_char(ScreenLines[off]);
	// double-byte character in single-width cell
	ако (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
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
screen_char_2(unsigned off, цео row, цео col)
{
    // Check for illegal values (could be wrong when screen was resized).
    ако (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	врати;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ако (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	врати;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ако (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	врати;
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
    цео		row,
    цео		col,
    цео		height,
    цео		width,
    цео		invert,
    цео		force)
{
    цео		r, c;
    цео		off;
    цео		max_off;

    // Can't use ScreenLines unless initialized
    ако (ScreenLines == NULL)
	врати;

    ако (invert)
	screen_char_attr = HL_INVERSE;
    за (r = row; r < row + height; ++r)
    {
	off = LineOffset[r];
	max_off = off + screen_Columns;
	за (c = col; c < col + width; ++c)
	{
	    ако (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ако (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    инако
	    {
		ако (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ако (utf_off2cells(off + c, max_off) > 1)
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
redraw_block(цео row, цео end, win_T *wp)
{
    цео		col;
    цео		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    ако (wp == NULL)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    инако
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    void
space_to_screenline(цео off, цео attr)
{
    ScreenLines[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    ако (enc_utf8)
	ScreenLinesUC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    void
screen_fill(
	цео	start_row,
	цео	end_row,
	цео	start_col,
	цео	end_col,
	цео	c1,
	цео	c2,
	цео	attr)
{
    цео	    row;
    цео	    col;
    цео	    off;
    цео	    end_off;
    цео	    did_delete;
    цео	    c;
    цео	    norm_term;
#ако defined(FEAT_GUI) || defined(UNIX)
    цео	    force_next = FALSE;
#endif

    ако (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    ако (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    ако (ScreenLines == NULL
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	врати;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    за (row = start_row; row < end_row; ++row)
    {
	ако (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ако (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		цео left_attr = ScreenAttrs[LineOffset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, left_attr);
	    }
	    ако (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		цео right_attr = ScreenAttrs[LineOffset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, right_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ако (c2 == ' '
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
	    ако (c1 != ' ')			// don't clear first char
		++col;

	    off = LineOffset[row] + col;
	    end_off = LineOffset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    ако (enc_utf8)
		док (off < end_off && ScreenLines[off] == ' '
			  && ScreenAttrs[off] == 0 && ScreenLinesUC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (цео)(off - LineOffset[row]))
#endif
			  )
		    ++off;
	    инако
		док (off < end_off && ScreenLines[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (цео)(off - LineOffset[row]))
#endif
			  )
		    ++off;
	    ако (off < end_off)		// something to be cleared
	    {
		col = off - LineOffset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		док (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (цео)(off - LineOffset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = LineOffset[row] + start_col;
	c = c1;
	за (col = start_col; col < end_col; ++col)
	{
	    ако ((ScreenLines[off] != c
		    || (enc_utf8 && (цео)ScreenLinesUC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ако defined(FEAT_GUI) || defined(UNIX)
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
		ако (screen_opacity_popup != NULL && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ако (ScreenLines[off] == 0
			    && off > 0
			    && enc_utf8 && ScreenLinesUC[off - 1] != 0
			    && utf_char2cells(ScreenLinesUC[off - 1]) == 2)
			goto next_col;
		}
		ако (screen_opacity_popup != NULL && c == ' '
						       && ScreenLines[off] != 0)
		{
		    цео bg_char_cells = 1;
		    ако (enc_utf8 && ScreenLinesUC[off] != 0)
			bg_char_cells = utf_char2cells(ScreenLinesUC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ако (bg_char_cells == 2)
		    {
			ако (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    goto skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ако (c2 != ' ')
			    goto skip_opacity_fill;
		    }

		    цео popup_attr = get_win_attr(screen_opacity_popup);
		    цео blend = screen_opacity_popup->w_popup_blend;
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
		ако (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != NULL
			&& row >= pum_bg_top && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    цео soff = (row - pum_bg_top) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ако (enc_utf8 && pum_bg_linesUC != NULL
			    && col > start_col
			    && pum_bg_linesUC[soff] == 0
			    && pum_bg_linesUC[soff - 1] != 0
			    && utf_char2cells(pum_bg_linesUC[soff - 1]) == 2)
		    {
			ScreenLines[off] = 0;
			ако (ScreenLinesUC != NULL)
			    ScreenLinesUC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			goto next_col;
		    }

		    цео underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ако (enc_utf8 && pum_bg_linesUC != NULL
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
			ако (ScreenLinesUC != NULL)
			    ScreenLinesUC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			goto next_col;
		    }

		    // Restore underlying character so text shows through.
		    ScreenLines[off] = pum_bg_lines[soff];
		    ако (enc_utf8 && pum_bg_linesUC != NULL
						&& ScreenLinesUC != NULL)
		    {
			цео k;
			ScreenLinesUC[off] = pum_bg_linesUC[soff];
			за (k = 0; k < MAX_MCO; ++k)
			    ако (pum_bg_linesC[k] != NULL
						    && ScreenLinesC[k] != NULL)
				ScreenLinesC[k][off] = pum_bg_linesC[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    goto next_col;
		}
#ако defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ако (
# ifdef FEAT_GUI
			gui.in_use
# endif
# ако defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    ако (ScreenLines[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    инако
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ако (gui.directx_enabled)
		    force_next = TRUE;
#endif
		ScreenLines[off] = c;
		ако (enc_utf8)
		{
		    ако (c >= 0x80)
		    {
			ScreenLinesUC[off] = c;
			ScreenLinesC[0][off] = 0;
			ScreenLines[off] = 0x80; // avoid storing zero
		    }
		    инако
			ScreenLinesUC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ако (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    ако (col == start_col)
	    {
		ако (did_delete)
		    break;
		c = c2;
	    }
	}
	ако (end_col == Columns)
	    LineWraps[row] = FALSE;
	ако (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ако (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ако (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    void
check_for_delay(цео check_msg_scroll)
{
    ако ((emsg_on_display || (check_msg_scroll && msg_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ако (check_msg_scroll)
	    msg_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    static void
clear_TabPageIdxs(void)
{
    цео		scol;

    за (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    цео
screen_valid(цео doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    врати (ScreenLines != NULL);
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
screenalloc(цео doclear)
{
    цео		    new_row, old_row;
#ifdef FEAT_GUI
    цео		    old_Rows;
#endif
    win_T	    *wp;
    цео		    outofmem = FALSE;
    цео		    len;
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
    char	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    static цео	    entered = FALSE;		// avoid recursiveness
    static цео	    done_outofmem_msg = FALSE;	// did outofmem message
    цео		    retry_count = 0;
    цео		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ако ((ScreenLines != NULL
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (ScreenLinesUC != NULL)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != NULL)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && ScreenLines == NULL))
	врати;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ако (entered)
	врати;
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
    за (цео i = 0; i < AUCMD_WIN_COUNT; ++i)
	ако (aucmd_win[i].auc_win != NULL)
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
    vim_memset(new_ScreenLinesC, 0, sizeof(u8char_T *) * MAX_MCO);
    ако (enc_utf8)
    {
	new_ScreenLinesUC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	за (цео i = 0; i < p_mco; ++i)
	    new_ScreenLinesC[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    ако (enc_dbcs == DBCS_JPNU)
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
    new_popup_transparent = LALLOC_MULT(char, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ако (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
	    goto give_up;
	}
    }
    за (цео i = 0; i < AUCMD_WIN_COUNT; ++i)
	ако (aucmd_win[i].auc_win != NULL
		&& aucmd_win[i].auc_win->w_lines == NULL
		&& win_alloc_lines(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    break;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ако (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
	    goto give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ако (win_alloc_lines(wp) == FAIL)
	    {
		outofmem = TRUE;
		goto give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    за (цео i = 0; i < p_mco; ++i)
	ако (new_ScreenLinesC[i] == NULL)
	{
	    found_null = TRUE;
	    break;
	}
    ако (new_ScreenLines == NULL
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
	ако (ScreenLines != NULL || !done_outofmem_msg)
	{
	    // guess the size
	    do_outofmem_msg((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_msg = TRUE;
	}
	VIM_CLEAR(new_ScreenLines);
	VIM_CLEAR(new_ScreenLinesUC);
	за (цео i = 0; i < p_mco; ++i)
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
    инако
    {
	done_outofmem_msg = FALSE;

	за (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_LineOffset[new_row] = new_row * Columns;
	    new_LineWraps[new_row] = FALSE;

	    (void)vim_memset(new_ScreenLines + new_row * Columns,
				  ' ', (size_t)Columns * sizeof(schar_T));
	    ако (enc_utf8)
	    {
		(void)vim_memset(new_ScreenLinesUC + new_row * Columns,
				   0, (size_t)Columns * sizeof(u8char_T));
		за (цео i = 0; i < p_mco; ++i)
		    (void)vim_memset(new_ScreenLinesC[i]
						      + new_row * Columns,
				   0, (size_t)Columns * sizeof(u8char_T));
	    }
	    ако (enc_dbcs == DBCS_JPNU)
		(void)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * sizeof(schar_T));
	    (void)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * sizeof(sattr_T));
	    (void)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * sizeof(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ако (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		ако (old_row >= 0 && ScreenLines != NULL)
		{
		    ако (screen_Columns < Columns)
			len = screen_Columns;
		    инако
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ако (!(enc_utf8 && ScreenLinesUC == NULL)
						       && p_mco == Screen_mco)
			mch_memmove(new_ScreenLines + new_LineOffset[new_row],
				ScreenLines + LineOffset[old_row],
				(size_t)len * sizeof(schar_T));
		    ако (enc_utf8 && ScreenLinesUC != NULL
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_ScreenLinesUC + new_LineOffset[new_row],
				ScreenLinesUC + LineOffset[old_row],
				(size_t)len * sizeof(u8char_T));
			за (цео i = 0; i < p_mco; ++i)
			    mch_memmove(new_ScreenLinesC[i]
						    + new_LineOffset[new_row],
				ScreenLinesC[i] + LineOffset[old_row],
				(size_t)len * sizeof(u8char_T));
		    }
		    ако (enc_dbcs == DBCS_JPNU && ScreenLines2 != NULL)
			mch_memmove(new_ScreenLines2 + new_LineOffset[new_row],
				ScreenLines2 + LineOffset[old_row],
				(size_t)len * sizeof(schar_T));
		    mch_memmove(new_ScreenAttrs + new_LineOffset[new_row],
			    ScreenAttrs + LineOffset[old_row],
			    (size_t)len * sizeof(sattr_T));
		    mch_memmove(new_ScreenCols + new_LineOffset[new_row],
			    ScreenCols + LineOffset[old_row],
			    (size_t)len * sizeof(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_ScreenLine = new_ScreenLines + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * sizeof(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * sizeof(char));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    ScreenLines = new_ScreenLines;
    ScreenLinesUC = new_ScreenLinesUC;
    за (цео i = 0; i < p_mco; ++i)
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
    ако (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ако (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    инако ако (gui.in_use
	    && !gui.starting
	    && ScreenLines != NULL
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (цео)Rows - 1, (цео)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ако (msg_row >= Rows)		// Rows got smaller
	    msg_row = Rows - 1;		// put cursor at last row
	инако ако (Rows > old_Rows)	// Rows got bigger
	    msg_row += Rows - old_Rows; // put cursor in same place
	ако (msg_col >= cmdline_width)	// cmdline_width got smaller
	    msg_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    ако (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ако (starting == 0 && ++retry_count <= 3)
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
    цео		i;

    VIM_CLEAR(ScreenLinesUC);
    за (i = 0; i < Screen_mco; ++i)
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
    цео
screenclear(void)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    врати screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    void
redraw_as_cleared(void)
{
    screenclear2(FALSE);
}

    static цео
screenclear2(цео doclear)
{
    цео	    i;
    цео	    did_clear = FALSE;

    ако (starting == NO_SCREEN || ScreenLines == NULL
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	врати FALSE;

#ifdef FEAT_GUI
    ако (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    за (i = 0; i < Rows; ++i)
    {
	lineclear(LineOffset[i], (цео)Columns, 0);
	LineWraps[i] = FALSE;
    }

    ако (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ако (suppressed_cells != NULL)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    инако
    {
	// can't clear the screen, mark all chars with invalid attributes
	за (i = 0; i < Rows; ++i)
	    lineinvalid(LineOffset[i], (цео)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ако defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ако (must_redraw == UPD_CLEAR)	// no need to clear again
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

    врати did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    static void
lineclear(unsigned off, цео width, цео attr)
{
    (void)vim_memset(ScreenLines + off, ' ', (size_t)width * sizeof(schar_T));
    ако (enc_utf8)
	(void)vim_memset(ScreenLinesUC + off, 0,
					  (size_t)width * sizeof(u8char_T));
    (void)vim_memset(ScreenAttrs + off, attr, (size_t)width * sizeof(sattr_T));
    (void)vim_memset(ScreenCols + off, -1, (size_t)width * sizeof(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    static void
lineinvalid(unsigned off, цео width)
{
    (void)vim_memset(ScreenAttrs + off, -1, (size_t)width * sizeof(sattr_T));
    (void)vim_memset(ScreenCols + off, -1, (size_t)width * sizeof(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    void
line_was_clobbered(цео screen_lnum)
{
    lineinvalid(LineOffset[screen_lnum], (цео)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    static void
linecopy(цео to, цео from, win_T *wp)
{
    unsigned	off_to = LineOffset[to] + wp->w_wincol;
    unsigned	off_from = LineOffset[from] + wp->w_wincol;

    mch_memmove(ScreenLines + off_to, ScreenLines + off_from,
	    wp->w_width * sizeof(schar_T));
    ако (enc_utf8)
    {
	цео	i;

	mch_memmove(ScreenLinesUC + off_to, ScreenLinesUC + off_from,
		wp->w_width * sizeof(u8char_T));
	за (i = 0; i < p_mco; ++i)
	    mch_memmove(ScreenLinesC[i] + off_to, ScreenLinesC[i] + off_from,
		    wp->w_width * sizeof(u8char_T));
    }
    ако (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * sizeof(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * sizeof(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * sizeof(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    цео
can_clear(char_u *p)
{
    врати (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#инако
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
windgoto(цео row, цео col)
{
    sattr_T	    *p;
    цео		    i;
    цео		    plan;
    цео		    cost;
    цео		    wouldbe_col;
    цео		    noinvcurs;
    char_u	    *bs;
    цео		    goto_cost;
    цео		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ако (ScreenLines == NULL)
	врати;
    ако (col == screen_cur_col && row == screen_cur_row)
	врати;

    // Check for valid position.
    ако (row < 0)	// window without text lines?
	row = 0;
    ако (row >= screen_Rows)
	row = screen_Rows - 1;
    ако (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ако (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    инако
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
    ако (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = NULL;			    // init for GCC
	attr = screen_attr;
	ако (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ако (*T_LE)
		bs = T_LE;		    // "cursor left"
	    инако
		bs = T_BC;		    // "backspace character (old)
	    ако (*bs)
		cost = (screen_cur_col - col) * (цео)STRLEN(bs);
	    инако
		cost = 999;
	    ако (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    инако
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    ако (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	инако ако (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    ако (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	инако
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
	ако (i > 0)
	    cost += i;
	ако (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + LineOffset[row] + wouldbe_col;
	    док (i && *p++ == attr)
		--i;
	    ако (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ако (*--p == 0)
		{
		    cost += noinvcurs;
		    док (i && *p++ == 0)
			--i;
		}
		ако (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    ако (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		за (i = wouldbe_col; i < col; ++i)
		    ако (ScreenLinesUC[LineOffset[row] + i] != 0)
		    {
			cost = 999;
			break;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ако (cost < 999)
		за (i = wouldbe_col; i < col; ++i)
		    ако (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			break;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ако (cost < goto_cost)
	{
	    ако (plan == PLAN_LE)
	    {
		ако (noinvcurs)
		    screen_stop_highlight();
		док (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    инако ако (plan == PLAN_CR)
	    {
		ако (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    инако ако (plan == PLAN_NL)
	    {
		ако (noinvcurs)
		    screen_stop_highlight();
		док (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    ако (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ако (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    док (i-- > 0)
			out_char(*T_ND);
		}
		инако
		{
		    цео	off;

		    off = LineOffset[row] + screen_cur_col;
		    док (i-- > 0)
		    {
			ако (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(ScreenLines[off]);
			ако (enc_dbcs == DBCS_JPNU
				&& ScreenLines[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    инако
	cost = 999;

    ако (cost >= goto_cost)
    {
	ако (noinvcurs)
	    screen_stop_highlight();
	ако (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_right(col - screen_cur_col);
	инако
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
    цео
cursor_screen_cells(void)
{
    врати has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    void
setcursor_mayforce(цео force)
{
    ако (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((цео)curwin->w_width - curwin->w_wcol
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
    цео
win_ins_lines(
    win_T	*wp,
    цео		row,
    цео		line_count,
    цео		invalid,
    цео		mayclear)
{
    цео		did_delete;
    цео		nextrow;
    цео		lastrow;
    цео		retval;

    ако (invalid)
	wp->w_lines_valid = 0;

    // with only a few lines it's not worth the effort
    ако (wp->w_height < 5)
	врати FAIL;

    // with the popup menu visible this might not work correctly
    ако (pum_visible())
	врати FAIL;

    ако (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, FALSE, 0);
    ако (retval != MAYBE)
	врати retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ако (wp->w_next != NULL || wp->w_status_height)
    {
	ако (screen_del_lines(0, W_WINROW(wp) + wp->w_height - line_count,
				  line_count, (цео)Rows, FALSE, 0, NULL) == OK)
	    did_delete = TRUE;
	инако ако (wp->w_next)
	    врати FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ако (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + line_count;
	ако (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - line_count, lastrow - line_count,
		  wp->w_wincol, (цео)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ако (screen_ins_lines(0, W_WINROW(wp) + row, line_count, (цео)Rows, 0, NULL)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ако (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	врати FAIL;
    }

    врати OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    цео
win_del_lines(
    win_T	*wp,
    цео		row,
    цео		line_count,
    цео		invalid,
    цео		mayclear,
    цео		clear_attr)	    // for clearing lines
{
    цео		retval;

    ако (invalid)
	wp->w_lines_valid = 0;

    ако (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, TRUE, clear_attr);
    ако (retval != MAYBE)
	врати retval;

    ако (screen_del_lines(0, W_WINROW(wp) + row, line_count,
				   (цео)Rows, FALSE, clear_attr, NULL) == FAIL)
	врати FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ако (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	ако (screen_ins_lines(0, W_WINROW(wp) + wp->w_height - line_count,
			      line_count, (цео)Rows, clear_attr, NULL) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    инако
	redraw_cmdline = TRUE;
    врати OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    static цео
win_do_lines(
    win_T	*wp,
    цео		row,
    цео		line_count,
    цео		mayclear,
    цео		del,
    цео		clear_attr)
{
    цео		retval;

    ако (!redrawing() || line_count <= 0)
	врати FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ако (no_win_do_lines_ins && !del)
	врати FAIL;

    // only a few lines left: redraw is faster
    ако (mayclear && Rows - line_count < 5
	    && wp->w_width == topframe->fr_width)
    {
	ако (!no_win_do_lines_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	врати FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ако (popup_visible)
	врати FAIL;
#endif

    // Delete all remaining lines
    ако (row + line_count >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (цео)W_ENDCOL(wp), ' ', ' ', 0);
	врати OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ако (!no_win_do_lines_ins)
	clear_cmdline = TRUE;

#ако defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ако (tabpanel_width() > 0)
	врати FAIL;
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
    ако (scroll_region || wp->w_width != topframe->fr_width)
    {
	ако (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	ако (del)
	    retval = screen_del_lines(W_WINROW(wp) + row, 0, line_count,
				    wp->w_height - row, FALSE, clear_attr, wp);
	инако
	    retval = screen_ins_lines(W_WINROW(wp) + row, 0, line_count,
					   wp->w_height - row, clear_attr, wp);
	ако (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	врати retval;
    }

    ако (wp->w_next != NULL && p_tf) // don't delete/insert on fast terminal
	врати FAIL;

    врати MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    static void
win_rest_invalid(win_T *wp)
{
    док (wp != NULL)
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
    цео
screen_ins_lines(
    цео		off,
    цео		row,
    цео		line_count,
    цео		end,
    цео		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    цео		i;
    цео		j;
    unsigned	temp;
    цео		cursor_row;
    цео		cursor_col = 0;
    цео		type;
    цео		result_empty;
    цео		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ако (!screen_valid(TRUE)
	     || line_count <= 0 || line_count > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	врати FAIL;

#ifdef FEAT_PROP_POPUP
    ако (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, line_count);
# endif
	врати FAIL;
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
    ако (wp != NULL && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ако (line_count > 3)
	    врати FAIL;
	type = USE_REDRAW;
    }
    инако ако (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    инако ако (*T_CAL != NUL && (line_count > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    инако ако (*T_CDL != NUL && result_empty && (line_count > 1 || !can_ce))
	type = USE_T_CDL;
    инако ако (*T_AL != NUL)
	type = USE_T_AL;
    инако ако (can_ce && result_empty)
	type = USE_T_CE;
    инако ако (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    инако ако (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    инако
	врати FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ако (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	врати screen_del_lines(off, row, line_count, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ако (*T_DB)
	screen_del_lines(off, end - line_count, line_count, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ако (off + row > 0 || (wp != NULL && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    инако
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

    ако (wp != NULL && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ако (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    инако
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    за (i = 0; i < line_count; ++i)
    {
	ако (wp != NULL && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    док ((j -= line_count) >= row)
		linecopy(j + line_count, j, wp);
	    j += line_count;
	    ако (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    инако
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	инако
	{
	    j = end - 1 - i;
	    temp = LineOffset[j];
	    док ((j -= line_count) >= row)
	    {
		LineOffset[j + line_count] = LineOffset[j];
		LineWraps[j + line_count] = LineWraps[j];
	    }
	    LineOffset[j + line_count] = temp;
	    LineWraps[j + line_count] = FALSE;
	    ако (can_clear((char_u *)" "))
		lineclear(temp, (цео)Columns, clear_attr);
	    инако
		lineinvalid(temp, (цео)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    ако (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ако (type == USE_REDRAW)
	redraw_block(row, end, wp);
    инако ако (type == USE_T_CAL)
    {
	term_append_lines(line_count);
	screen_start();		// don't know where cursor is now
    }
    инако
    {
	за (i = 0; i < line_count; i++)
	{
	    ако (type == USE_T_AL)
	    {
		ако (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    инако  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ако (type == USE_T_SR && *T_DA)
    {
	за (i = 0; i < line_count; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#ако defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ако (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    врати OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    цео
screen_del_lines(
    цео		off,
    цео		row,
    цео		line_count,
    цео		end,
    цео		force,		// even when line_count > p_ttyscroll
    цео		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    цео		j;
    цео		i;
    unsigned	temp;
    цео		cursor_row;
    цео		cursor_col = 0;
    цео		cursor_end;
    цео		result_empty;	// result is empty until end of region
    цео		can_delete;	// deleting line codes can be used
    цео		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ако (!screen_valid(TRUE)
	    || line_count <= 0
	    || (!force && line_count > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	врати FAIL;

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
    ако (wp != NULL && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ако (line_count > 3)
	    врати FAIL;
	type = USE_REDRAW;
    }
    инако ако (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    инако ако (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    line_count == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    инако ако (*T_CDL != NUL && line_count > 1 && can_delete)
	type = USE_T_CDL;
    инако ако (can_clear(T_CE) && result_empty
	    && (wp == NULL || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    инако ако (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    инако ако (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    инако
	врати FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ако (off + row > 0 || (wp != NULL && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    инако
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

    ако (wp != NULL && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ако (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    инако
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
    за (i = 0; i < line_count; ++i)
    {
	ако (wp != NULL && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    док ((j += line_count) <= end - 1)
		linecopy(j - line_count, j, wp);
	    j -= line_count;
	    ако (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    инако
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	инако
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = LineOffset[j];
	    док ((j += line_count) <= end - 1)
	    {
		LineOffset[j - line_count] = LineOffset[j];
		LineWraps[j - line_count] = LineWraps[j];
	    }
	    LineOffset[j - line_count] = temp;
	    LineWraps[j - line_count] = FALSE;
	    ако (can_clear((char_u *)" "))
		lineclear(temp, (цео)Columns, clear_attr);
	    инако
		lineinvalid(temp, (цео)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    ако (screen_attr != clear_attr)
	screen_stop_highlight();
    ако (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ако (type == USE_REDRAW)
	redraw_block(row, end, wp);
    инако ако (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    инако ако (type == USE_T_CDL)
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
    инако ако (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	за (i = line_count; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    инако
    {
	за (i = line_count; --i >= 0; )
	{
	    ако (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    инако // type == USE_T_CE
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
    ако (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	за (i = line_count; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#ако defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ако (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    врати OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    цео
skip_showmode(void)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ако (global_busy
	    || msg_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !KeyTyped))
    {
	redraw_mode = TRUE;		// show mode later
	врати TRUE;
    }
    врати FALSE;
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
    цео
showmode(void)
{
    цео		need_clear;
    цео		length = 0;
    цео		do_mode;
    цео		attr;
    цео		nwr_save;
    цео		sub_attr;
    цео		show_ruler_with_pum = FALSE;

    do_mode = p_smd && msg_silent == 0
	    && ((State & MODE_INSERT)
		|| restart_edit != NUL
		|| VIsual_active);
    ако (do_mode || reg_recording != 0)
    {
	ако (skip_showmode())
	    врати 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ако (clear_cmdline && cmdline_row < Rows - 1)
	    msg_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	msg_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ако (do_mode)
	{
	    msg_puts_attr("--", attr);
#ако defined(FEAT_XIM)
	    ако (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# инако
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		msg_puts_attr(" IM", attr);
# инако
		msg_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    ако (edit_submode != NULL && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - msg_row) * cmdline_width - 3;
		ако (edit_submode_extra != NULL)
		    length -= vim_strsize(edit_submode_extra);
		ако (length > 0)
		{
		    ако (edit_submode_pre != NULL)
			length -= vim_strsize(edit_submode_pre);
		    ако (length - vim_strsize(edit_submode) > 0)
		    {
			ако (edit_submode_pre != NULL)
			    msg_puts_attr((char *)edit_submode_pre, attr);
			msg_puts_attr((char *)edit_submode, attr);
		    }
		    ако (edit_submode_extra != NULL)
		    {
			msg_puts_attr(" ", attr);  // add a space in between
			ако ((цео)edit_submode_highl < (цео)HLF_COUNT)
			    sub_attr = HL_ATTR(edit_submode_highl);
			инако
			    sub_attr = attr;
			msg_puts_attr((char *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    инако
	    {
		ако (State & VREPLACE_FLAG)
		    msg_puts_attr(_(" VREPLACE"), attr);
		инако ако (State & REPLACE_FLAG)
		    msg_puts_attr(_(" REPLACE"), attr);
		инако ако (State & MODE_INSERT)
		{
#ifdef FEAT_RIGHTLEFT
		    ако (p_ri)
			msg_puts_attr(_(" REVERSE"), attr);
#endif
		    msg_puts_attr(_(" INSERT"), attr);
		}
		инако ако (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    msg_puts_attr(_(" (insert)"), attr);
		инако ако (restart_edit == 'R')
		    msg_puts_attr(_(" (replace)"), attr);
		инако ако (restart_edit == 'V')
		    msg_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		ако (p_hkmap)
		    msg_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		ако (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ако (curwin->w_p_arab)
			msg_puts_attr(_(" Arabic"), attr);
		    инако
# endif
			ако (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    msg_puts_attr((char *)NameBuff, attr);
		}
#endif
		ако ((State & MODE_INSERT) && p_paste)
		    msg_puts_attr(_(" (paste)"), attr);

		ако (VIsual_active)
		{
		    char *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    switch ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			case 0:	p = N_(" VISUAL"); break;
			case 1: p = N_(" VISUAL LINE"); break;
			case 2: p = N_(" VISUAL BLOCK"); break;
			case 4: p = N_(" SELECT"); break;
			case 5: p = N_(" SELECT LINE"); break;
			default: p = N_(" SELECT BLOCK"); break;
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
	ако (reg_recording != 0
		&& edit_submode == NULL)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	ако (need_clear || clear_cmdline || redraw_mode)
	    msg_clr_eos();
	msg_didout = FALSE;		// overwrite this message
	length = msg_col;
	msg_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    инако ако (clear_cmdline && msg_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	msg_clr_cmdline();
    инако ако (redraw_mode)
    {
	msg_pos_mode();
	msg_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ако (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ако (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    врати length;
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
unshowmode(цео force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ако (!redrawing() || (!force && char_avail() && !KeyTyped))
	redraw_cmdline = TRUE;		// delete mode later
    инако
	clearmode();
}

/*
 * Clear the mode message.
 */
    void
clearmode(void)
{
    цео save_msg_row = msg_row;
    цео save_msg_col = msg_col;

    msg_pos_mode();
    ако (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    msg_clr_eos();

    msg_col = save_msg_col;
    msg_row = save_msg_row;
}

    static void
recording_mode(цео attr)
{
    msg_puts_attr(_("recording"), attr);
    ако (shortmess(SHM_RECORDING))
	врати;

    char s[4];

    sprintf(s, " @%c", reg_recording);
    msg_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    static цео
mouse_has_any(void)
{
    врати mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_INSERT)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    void
draw_tabline(void)
{
    цео		tabcount = 0;
    tabpage_T	*tp;
    цео		tabwidth;
    цео		col = 0;
    цео		scol = 0;
    цео		attr;
    win_T	*wp;
    win_T	*cwp;
    цео		wincount;
    цео		modified;
    цео		c;
    цео		len;
    цео		attr_sel;
    цео		attr_nosel = 0;
    цео		attr_fill = 0;
    char_u	*p;
    цео		room;
    цео		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#ако defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    ако (ScreenLines == NULL)
	врати;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ако (gui_use_tabline())
    {
	gui_update_tabline();
	врати;
    }
#endif

    ако (tabline_height() < 1)
	врати;

#ако defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    ако (*p_tal != NUL)
	win_redr_custom(NULL, FALSE);
    инако
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	ако (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	за (tp = first_tabpage; tp != NULL && col < Columns - 4;
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

	    ако (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ако (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    ако (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    ако (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    инако
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    за (wincount = 0; wp != NULL; wp = wp->w_next, ++wincount)
		ако (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    ако (modified || wincount > 1)
	    {
		ако (wincount > 1)
		{
		    len = vim_snprintf((char *)NameBuff, MAXPATHL, "%d", wincount);
		    ако (col + len >= Columns - 3)
			break;
		    screen_puts_len(NameBuff, len, 0, col,
#ако defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#инако
					 attr
#endif
					       );
		    col += len;
		}
		ако (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    ако (room > 0)
	    {
		цео n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ако (has_mbyte)
		    док (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		инако ако (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		ако (len > n)
		    len = n;

		screen_puts_len(p, (цео)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    док (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    ако (override_success)
		pop_highlight_overrides();
	}

	ако (use_sep_chars)
	    c = '_';
	инако
	    c = ' ';
	screen_fill(0, 1, col, (цео)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ако (p_sc && *p_sloc == 't')
	{
	    цео	n = (цео)Columns - col - (tabcount > 1) * 3;
	    цео	width = MIN(10, n);

	    ако (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (цео)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ако (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (цео)Columns - 1, attr_nosel);
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
    ако (buf_spname(buf) != NULL)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    инако
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    цео
fillchar_status(цео *attr, win_T *wp)
{
    цео	    fill;
    bool    override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    ако (bt_terminal(wp->w_buffer))
    {
	ако (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	инако
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    инако
#endif
    ако (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    инако
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ако (override_success)
	pop_highlight_overrides();
    врати fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    static bool
vsep_row_is_curwin(win_T *wp, цео row)
{
    ако (wp == curwin)
	врати true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ако (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	врати true;

    врати false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    цео
fillchar_vsep(цео *attr, win_T *wp, цео row)
{
    bool override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ако (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    инако
	*attr = HL_ATTR(HLF_CNC);
    ако (override_success)
	pop_highlight_overrides();

    ако (*attr == 0 && wp->w_fill_chars.vert == ' ')
	врати '|';
    инако
	врати wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    цео
redrawing(void)
{
#ifdef FEAT_EVAL
    ако (disable_redraw_for_testing)
	врати 0;
    инако
#endif
	врати ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !KeyTyped && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    цео
messaging(void)
{
    врати (!(p_lz && char_avail() && !KeyTyped));
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
    цео last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    ако (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#инако
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	ако (!last_has_status)
	    sc_col = ru_col;
    }
    ако (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	ако (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    ако (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    ако (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#ако defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    цео
number_width(win_T *wp)
{
    цео		n;
    linenr_T	lnum;

    ако (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    инако
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_line_count;

    ако (lnum == wp->w_nrwidth_line_count && wp->w_nuw_cached == wp->w_p_nuw)
	врати wp->w_nrwidth_width;
    wp->w_nrwidth_line_count = lnum;

    n = 0;
    do
    {
	lnum /= 10;
	++n;
    } док (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ако (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ако (n < 2 && get_first_valid_sign(wp) != NULL
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    врати n;
}
#endif

#ако defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    цео
screen_screencol(void)
{
    врати screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    цео
screen_screenrow(void)
{
    врати screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    static цео
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ако (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T num = 0;
	цео	    bytes;
	цео	    n;

	за (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ако (n < 0)
		врати 0;
	    num = num * 256 + n;
	}
	*p += 2;
	врати num;
    }
    врати mb_ptr2char_adv(p);
}

struct charstab
{
    цео	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

static fill_chars_T fill_chars;
static struct charstab filltab[] =
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
#ако defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
static lcs_chars_T lcs_chars;
static struct charstab lcstab[] =
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
#инако
    CHARSTAB_ENTRY(NULL,		    "conceal"),
#endif
    CHARSTAB_ENTRY(NULL,		    "multispace"),
    CHARSTAB_ENTRY(NULL,		    "leadmultispace")
};

    static char *
field_value_err(char *errbuf, size_t errbuflen, char *fmt, char_u *field)
{
    ако (errbuf == NULL)
	врати "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    врати errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    static char *
set_chars_option(win_T *wp, char_u *value, цео is_listchars, цео apply,
						char *errbuf, size_t errbuflen)
{
    цео	    round, i, entries;
    char_u  *p, *s;
    цео	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = NULL;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = NULL; // Last occurrence of "leadmultispace:"
    цео	    multispace_len = 0;	      // Length of lcs-multispace string
    цео	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    struct charstab *tab;

    ако (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	ако (wp->w_p_lcs[0] == NUL)
	    value = p_lcs;  // local value is empty, use the global value
    }
    инако
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	ако (wp->w_p_fcs[0] == NUL)
	    value = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    за (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	цео has_tab = FALSE, has_leadtab = FALSE;

	ако (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ако (is_listchars)
	    {
		за (i = 0; i < entries; ++i)
		    ако (tab[i].cp != NULL)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ако (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(цео, multispace_len + 1);
		    ако (lcs_chars.multispace != NULL)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		инако
		    lcs_chars.multispace = NULL;

		ако (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(цео, lead_multispace_len + 1);
		    ако (lcs_chars.leadmultispace != NULL)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		инако
		    lcs_chars.leadmultispace = NULL;
	    }
	    инако
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
#ако defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = value;
	док (*p)
	{
	    за (i = 0; i < entries; ++i)
	    {
		ако (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    continue;

		s = p + tab[i].name.length + 1;

		ако (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ако (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			док (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ако (char2cells(c1) > 1)
				врати field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ако (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    врати field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    инако
		    {
			цео multispace_pos = 0;

			док (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ако (p == last_multispace && lcs_chars.multispace != NULL)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    break;
		}

		ако (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ако (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			док (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ако (char2cells(c1) > 1)
				врати field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ако (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    врати field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    инако
		    {
			цео multispace_pos = 0;

			док (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ако (p == last_lmultispace && lcs_chars.leadmultispace != NULL)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    break;
		}

		c2 = c3 = 0;
		ако (*s == NUL)
		    врати field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ако (char2cells(c1) > 1)
		    врати field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		ако (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ако (*s == NUL)
			врати field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ако (char2cells(c2) > 1)
			врати field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    ако (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ако (char2cells(c3) > 1)
			    врати field_value_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    ако (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    инако  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ако (*s == ',' || *s == NUL)
		{
		    ако (round > 0)
		    {
			ако (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			инако ако (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			инако ако (tab[i].cp != NULL)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    break;
		}
		инако
		    врати field_value_err(errbuf, errbuflen,
				    e_wrong_number_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    ако (i == entries)
		врати e_invalid_argument;

	    ако (*p == ',')
		++p;
	}

	ако (is_listchars && has_leadtab && !has_tab)
	    врати e_leadtab_requires_tab;
    }

    ако (apply)
    {
	ако (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	инако
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    врати NULL;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    char *
set_fillchars_option(win_T *wp, char_u *val, цео apply, char *errbuf,
							      size_t errbuflen)
{
    врати set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    char *
set_listchars_option(win_T *wp, char_u *val, цео apply, char *errbuf,
							      size_t errbuflen)
{
    врати set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, цео idx)
{
    ако (idx < 0 || idx >= (цео)ARRAY_LENGTH(filltab))
	врати NULL;

    врати filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, цео idx)
{
    ако (idx < 0 || idx >= (цео)ARRAY_LENGTH(lcstab))
	врати NULL;

    врати lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    char *
check_chars_options(void)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ако (set_listchars_option(curwin, p_lcs, FALSE, NULL, 0) != NULL)
	врати e_conflicts_with_value_of_listchars;
    ако (set_fillchars_option(curwin, p_fcs, FALSE, NULL, 0) != NULL)
	врати e_conflicts_with_value_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ако (set_listchars_option(wp, wp->w_p_lcs, FALSE, NULL, 0) != NULL)
	    врати e_conflicts_with_value_of_listchars;
	ако (set_fillchars_option(wp, wp->w_p_fcs, FALSE, NULL, 0) != NULL)
	    врати e_conflicts_with_value_of_fillchars;
    }
    врати NULL;
}
