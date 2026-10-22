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

#અન્તર્ભાવ "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
સ્થિર પૂર્ણાંક	screen_attr = 0;

સ્થિર શૂન્ય screen_char_2(unsigned off, પૂર્ણાંક row, પૂર્ણાંક col);
સ્થિર પૂર્ણાંક screenclear2(પૂર્ણાંક doclear);
સ્થિર શૂન્ય lineclear(unsigned off, પૂર્ણાંક width, પૂર્ણાંક attr);
સ્થિર શૂન્ય lineinvalid(unsigned off, પૂર્ણાંક width);
સ્થિર પૂર્ણાંક win_do_લીટીઓ(win_T *wp, પૂર્ણાંક row, પૂર્ણાંક લીટી_ગણતરી, પૂર્ણાંક mayclear, પૂર્ણાંક del, પૂર્ણાંક clear_attr);
સ્થિર શૂન્ય win_rest_invalid(win_T *wp);
સ્થિર શૂન્ય સંદેશ_pos_mode(શૂન્ય);
સ્થિર શૂન્ય recording_mode(પૂર્ણાંક attr);

// Ugly global: overrule attribute used by screen_char()
સ્થિર પૂર્ણાંક screen_char_attr = 0;

#જો defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    પૂર્ણાંક
conceal_cursor_લીટી(win_T *wp)
{
    પૂર્ણાંક		c;

    જો (*wp->w_p_cocu == NUL)
	પાછા FALSE;
    જો (get_real_state() & MODE_VISUAL)
	c = 'v';
    નહીંતર જો (State & MODE_ઉમેરો)
	c = 'i';
    નહીંતર જો (State & MODE_NORMAL)
	c = 'n';
    નહીંતર જો (State & MODE_CMDLINE)
	c = 'c';
    નહીંતર
	પાછા FALSE;
    પાછા vim_strchr(wp->w_p_cocu, c) != ખાલી;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    શૂન્ય
conceal_check_cursor_લીટી(પૂર્ણાંક was_concealed)
{
    જો (curwin->w_p_cole <= 0 || conceal_cursor_લીટી(curwin) == was_concealed)
	પાછા;

    પૂર્ણાંક wcol = curwin->w_wcol;

    need_cursor_લીટી_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    જો (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    પૂર્ણાંક
get_win_attr(win_T *wp)
{
    પૂર્ણાંક	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

    જો (win_attr != 0)
    {
	જો (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	નહીંતર
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    નહીંતર જો (WIN_IS_POPUP(wp))
    {
	જો (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	નહીંતર
	    win_attr = HL_ATTR(HLF_કાઢો);    // Popup
    }
#endif

    જો (override_success)
	કાઢો_highlight_overrides();
    પાછા win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    સ્થિર પૂર્ણાંક
screen_fill_end(
	win_T *wp,
	પૂર્ણાંક	c1,
	પૂર્ણાંક	c2,
	પૂર્ણાંક	off,
	પૂર્ણાંક	width,
	પૂર્ણાંક	row,
	પૂર્ણાંક	endrow,
	પૂર્ણાંક	attr)
{
    પૂર્ણાંક	    nn = off + width;

    જો (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    જો (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (પૂર્ણાંક)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    નહીંતર
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (પૂર્ણાંક)wp->w_wincol + nn,
		c1, c2, attr);
    પાછા nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    શૂન્ય
win_draw_end(
    win_T	*wp,
    પૂર્ણાંક		c1,
    પૂર્ણાંક		c2,
    પૂર્ણાંક		draw_margin,
    પૂર્ણાંક		row,
    પૂર્ણાંક		endrow,
    hlf_T	hl)
{
    પૂર્ણાંક		n = 0;
    પૂર્ણાંક		attr = HL_ATTR(hl);
    પૂર્ણાંક		win_attr = get_win_attr(wp);
    bool	override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    જો (draw_margin)
    {
#ifdef FEAT_FOLDING
	પૂર્ણાંક	fdc = compute_foldcolumn(wp, 0);

	જો (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	જો (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	જો ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == ખાલી)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, સંખ્યા_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    જો (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    નહીંતર
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (પૂર્ણાંક)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    જો (override_success)
	કાઢો_highlight_overrides();
}

#જો defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    પૂર્ણાંક
compute_foldcolumn(win_T *wp, પૂર્ણાંક col)
{
    પૂર્ણાંક wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    પૂર્ણાંક n = wp->w_width - (col + wmw);

    પાછા MIN(wp->w_p_fdc, n);
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
    પૂર્ણાંક		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    પૂર્ણાંક		i = 0;
    પૂર્ણાંક		level;
    પૂર્ણાંક		first_level;
    પૂર્ણાંક		empty;
    પૂર્ણાંક		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    પૂર્ણાંક		પ્રતીક = 0;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    જો (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    માટે (i = 0; i < n; i++)
    {
	જો (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_નીચલું_level)
	    પ્રતીક = wp->w_fill_chars.foldopen;
	નહીંતર જો (first_level == 1)
	    પ્રતીક = wp->w_fill_chars.foldsep;
	નહીંતર જો (wp->w_fill_chars.foldinner != NUL)
	    પ્રતીક = wp->w_fill_chars.foldinner;
	નહીંતર જો (first_level + i <= 9)
	    પ્રતીક = '0' + first_level + i;
	નહીંતર
	    પ્રતીક = '>';

	len = utf_char2bytes(પ્રતીક, &p[byte_counter]);
	byte_counter += len;
	જો (first_level + i >= level)
	{
	    i++;
	    તોડો;
	}
    }

    જો (closed)
    {
	જો (પ્રતીક != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    જો (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	પ્રતીક = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(પ્રતીક, &p[byte_counter]);
	byte_counter += len;
    }

    પાછા MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    સ્થિર પૂર્ણાંક
comp_char_differs(પૂર્ણાંક off_from, પૂર્ણાંક off_to)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < Screen_mco; ++i)
    {
	જો (Screen_લીટીઓ_C[i][off_from] != Screen_લીટીઓ_C[i][off_to])
	    પાછા TRUE;
	જો (Screen_લીટીઓ_C[i][off_from] == 0)
	    તોડો;
    }
    પાછા FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    સ્થિર પૂર્ણાંક
char_needs_redraw(પૂર્ણાંક off_from, પૂર્ણાંક off_to, પૂર્ણાંક cols)
{
    જો (cols > 0
	    && ((Screen_લીટીઓ[off_from] != Screen_લીટીઓ[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_લીટીઓ[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_લીટીઓ[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_લીટીઓ[off_from + 1]
						 != Screen_લીટીઓ[off_to + 1])))
		|| (enc_utf8
		    && (Screen_લીટીઓ_UC[off_from] != Screen_લીટીઓ_UC[off_to]
			|| (Screen_લીટીઓ_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_લીટીઓ[off_from + 1]
						!= Screen_લીટીઓ[off_to + 1])))))
	પાછા TRUE;
    પાછા FALSE;
}

#જો defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    પૂર્ણાંક
screen_get_current_લીટી_off(શૂન્ય)
{
    પાછા (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    સ્થિર પૂર્ણાંક
blocked_by_popup(પૂર્ણાંક row, પૂર્ણાંક col)
{
    પૂર્ણાંક off;

    જો (!popup_visible)
	પાછા FALSE;
    off = row * screen_Columns + col;
    પાછા popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    શૂન્ય
reset_screen_attr(શૂન્ય)
{
#ifdef FEAT_GUI
    જો (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    નહીંતર
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    સ્થિર પૂર્ણાંક
skip_for_popup(પૂર્ણાંક row, પૂર્ણાંક col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    જો (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	પાછા TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    જો (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	પાછા TRUE;
#નહીંતર
    જો (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	પાછા TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    જો (blocked_by_popup(row, col))
	પાછા TRUE;
#endif
    પાછા FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
સ્થિર char_u	*suppressed_cells = ખાલી;
સ્થિર પૂર્ણાંક	suppressed_rows = 0;
સ્થિર પૂર્ણાંક	suppressed_cols = 0;

    સ્થિર શૂન્ય
mark_suppressed_cell(પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (suppressed_cells == ખાલી
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	જો (suppressed_cells == ખાલી)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    પાછા;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    સ્થિર શૂન્ય
unmark_suppressed_cell(પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (suppressed_cells != ખાલી
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    સ્થિર પૂર્ણાંક
is_suppressed_cell(પૂર્ણાંક row, પૂર્ણાંક col)
{
    પાછા suppressed_cells != ખાલી
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
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		popup_attr;	// get_win_attr(screen_opacity_popup)
    પૂર્ણાંક		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    સ્થિર અંતઃરેખ શૂન્ય
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    સ્થિર અંતઃરેખ પૂર્ણાંક
popup_base_attr_or(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક fallback)
{
    પૂર્ણાંક		base = fallback;

    popup_get_base_screen_cell(row, col, ખાલી, &base, ખાલી);
    પાછા base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    સ્થિર અંતઃરેખ પૂર્ણાંક
popup_blend_with_base(
	અચલ popup_opacity_T	*po,
	પૂર્ણાંક			char_attr,
	પૂર્ણાંક			base_attr,
	પૂર્ણાંક			blend_fg)
{
    પૂર્ણાંક		combined = hl_combine_attr(po->popup_attr, char_attr);

    પાછા hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    સ્થિર શૂન્ય
resolve_wide_char_opacity_attrs(
	પૂર્ણાંક row, પૂર્ણાંક col1, પૂર્ણાંક col2,
	sattr_T *attr1, sattr_T *attr2)
{
    પૂર્ણાંક		bg1, bg2;
    પૂર્ણાંક		base1 = 0;
    પૂર્ણાંક		base2 = 0;
    attrentry_T	*ae;

    જો (*attr1 == *attr2)
	પાછા;

    popup_get_base_screen_cell(row, col1, ખાલી, &base1, ખાલી);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != ખાલી && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# નહીંતર
    bg1 = (ae != ખાલી && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, ખાલી, &base2, ખાલી);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != ખાલી && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# નહીંતર
    bg2 = (ae != ખાલી && ae->ae_u.cterm.bg_color != 0);
# endif

    જો (bg1 && !bg2)
	*attr1 = *attr2;
    નહીંતર જો (!bg1 && bg2)
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
    શૂન્ય
screen_લીટી(
	win_T	*wp,
	પૂર્ણાંક	row,
	પૂર્ણાંક	coloff,
	પૂર્ણાંક	endcol,
	પૂર્ણાંક	clear_width,
	colnr_T	last_vcol,
	પૂર્ણાંક	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    પૂર્ણાંક		    col = 0;
    પૂર્ણાંક		    hl;
    પૂર્ણાંક		    force = FALSE;	// force update rest of the line
    પૂર્ણાંક		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    પૂર્ણાંક		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    પૂર્ણાંક		    changed_this;	// TRUE if character changed
    પૂર્ણાંક		    changed_next;	// TRUE if next character changed
#endif
    પૂર્ણાંક		    clear_next = FALSE;
    પૂર્ણાંક		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    પૂર્ણાંક		    drawing_opacity_popup =
				screen_opacity_popup != ખાલી
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    જો (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    જો (row >= Rows)
	row = Rows - 1;
    જો (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_Screen_લીટી - Screen_લીટીઓ);
    off_to = લીટી_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = લીટી_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    જો (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	જો (clear_width > 0)
	{
	    પૂર્ણાંક clear_start = col;

	    જ્યારે (col <= endcol && Screen_લીટીઓ[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_લીટીઓ_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    જો (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    માટે (પૂર્ણાંક i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = લીટી_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    જો (coloff > 0 && enc_utf8
		   && Screen_લીટીઓ[off_to] == 0
		   && Screen_લીટીઓ_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_લીટીઓ_UC[off_to - 1]) > 1)
    {
	Screen_લીટીઓ[off_to - 1] = ' ';
	Screen_લીટીઓ_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	જો (screen_opacity_popup == ખાલી)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    જ્યારે (col < endcol)
    {
	જો (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	નહીંતર
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	જો (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	જો (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    જો (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    જો (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	જો (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	જો (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	પૂર્ણાંક opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	જો (drawing_opacity_popup
		&& Screen_લીટીઓ[off_from] == ' '
		&& (!enc_utf8 || Screen_લીટીઓ_UC[off_from] == 0))
	{
	    પૂર્ણાંક popup_src_attr = ScreenAttrs[off_from];
	    પૂર્ણાંક bg_is_empty = Screen_લીટીઓ[off_to] == 0
				&& (!enc_utf8 || Screen_લીટીઓ_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    જો (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_લીટીઓ_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_લીટીઓ_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    નહીંતર જો (Screen_લીટીઓ[off_to] != 0
		    || (enc_utf8 && Screen_લીટીઓ_UC[off_to] != 0))
	    {
		પૂર્ણાંક bg_char_cells = (enc_utf8 && Screen_લીટીઓ_UC[off_to] != 0)
				? utf_char2cells(Screen_લીટીઓ_UC[off_to]) : 1;

		જો (bg_char_cells == 2)
		{
		    જો (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			પૂર્ણાંક base_attr;
			Screen_લીટીઓ[off_to] = ' ';
			જો (enc_utf8)
			    Screen_લીટીઓ_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			જાઓ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    જો (!(Screen_લીટીઓ[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_લીટીઓ_UC[off_from + 1] == 0)))
			જાઓ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		પૂર્ણાંક base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		જો (bg_char_cells == 2)
		{
		    પૂર્ણાંક base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    જો (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    નહીંતર જો (bg_is_empty)
	    {
		પૂર્ણાંક base_attr;
		Screen_લીટીઓ[off_to] = ' ';
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

	જો (redraw_this)
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
	    જો (       p_wiv
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
		જો (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		નહીંતર
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    જો (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		જો (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_લીટીઓ[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		નહીંતર જો (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_લીટીઓ[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		જો (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    જો (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_લીટીઓ[off_to] = Screen_લીટીઓ[off_from];
	    જો (enc_utf8)
	    {
		Screen_લીટીઓ_UC[off_to] = Screen_લીટીઓ_UC[off_from];
		જો (Screen_લીટીઓ_UC[off_from] != 0)
		{
		    પૂર્ણાંક	    i;

		    માટે (i = 0; i < Screen_mco; ++i)
			Screen_લીટીઓ_C[i][off_to] = Screen_લીટીઓ_C[i][off_from];
		}
	    }
	    જો (char_cells == 2)
	    {
		Screen_લીટીઓ[off_to + 1] = Screen_લીટીઓ[off_from + 1];
		જો (enc_utf8)
		    Screen_લીટીઓ_UC[off_to + 1] = 0;
	    }

#જો defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    જો (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# જો defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		જો (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		જો (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    જો (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    જો (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    જો (drawing_opacity_popup && po.blend > 0)
	    {
		પૂર્ણાંક popup_src_attr = ScreenAttrs[off_from];
		પૂર્ણાંક scol1 = col + coloff;
		પૂર્ણાંક base1 = popup_base_attr_or(row, scol1, 0);

		જો (char_cells == 2)
		{
		    પૂર્ણાંક base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    પૂર્ણાંક over1 = popup_is_over_opacity(row, scol1);
		    પૂર્ણાંક over2 = popup_is_over_opacity(row, scol1 + 1);
		    જો (over1 != over2)
		    {
			જો (over1)
			    base1 = base2;
			નહીંતર
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    નહીંતર
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    જો (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    જો (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    નહીંતર
		screen_char(off_to, row, col + coloff);
	}
	નહીંતર જો (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    જો (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    નહીંતર જો (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	જો (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    જો (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    જો (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_લીટીઓ[off_to] = ' ';
	જો (enc_utf8)
	    Screen_લીટીઓ_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	જો (screen_opacity_popup == ખાલી)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    જો (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	પૂર્ણાંક startCol = col;
#endif

	// blank out the rest of the line
	જ્યારે (col < clear_width && Screen_લીટીઓ[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_લીટીઓ_UC[off_to] == 0)
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
	જો (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    જો (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		જો (hl > HL_ALL || (hl & HL_BOLD))
		{
		    પૂર્ણાંક prev_cells = 1;

		    જો (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_લીટીઓ[off_to - 1] == 0 ? 2 : 1;
		    નહીંતર જો (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = લીટી_Offset[row];
			unsigned max_off = લીટી_Offset[row] + screen_Columns;

			જ્યારે (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    જો (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			જો (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			નહીંતર
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    જ્યારે (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    જો (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	જો (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    જો (!skip_for_popup(row, col + coloff))
	    {
		પૂર્ણાંક c;

		c = fillchar_vsep(&hl, wp, row);
		જો (Screen_લીટીઓ[off_to] != (schar_T)c
			|| (enc_utf8 && (પૂર્ણાંક)Screen_લીટીઓ_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_લીટીઓ[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    જો (enc_utf8)
		    {
			જો (c >= 0x80)
			{
			    Screen_લીટીઓ_UC[off_to] = c;
			    Screen_લીટીઓ_C[0][off_to] = 0;
			    Screen_લીટીઓ[off_to] = 0x80; // avoid storing zero
			}
			નહીંતર
			    Screen_લીટીઓ_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	નહીંતર
	    લીટી_Wraps[row] = FALSE;
    }

    જો (override_success)
	કાઢો_highlight_overrides();
}

#જો defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    શૂન્ય
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    પૂર્ણાંક		t;

    માટે (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
    શૂન્ય
draw_vsep_win(win_T *wp, પૂર્ણાંક row)
{
    જો (!wp->w_vsep_width)
	પાછા;

    પૂર્ણાંક content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    પૂર્ણાંક start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    માટે (પૂર્ણાંક r = start_row; r < content_end; ++r)
    {
	પૂર્ણાંક hl;
	પૂર્ણાંક c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    જો (wp->w_status_height != 0)
    {
	પૂર્ણાંક hl;
	પૂર્ણાંક c;
	જો (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	નહીંતર
	    c = fillchar_vsep(&hl, wp, content_end);
	માટે (પૂર્ણાંક r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    પૂર્ણાંક
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    જ્યારે (fr->fr_parent != ખાલી)
    {
	જો (fr->fr_parent->fr_layout == FR_COL)
	{
	    જો (fr->fr_next != ખાલી)
		તોડો;
	}
	નહીંતર
	{
	    જો (fr->fr_next != ખાલી)
		પાછા TRUE;
	}
	fr = fr->fr_parent;
    }
    પાછા FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    પૂર્ણાંક
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    પૂર્ણાંક		len)	    // length of buffer
{
    char_u	*p;
    પૂર્ણાંક		plen;

    જો (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	પાછા 0;

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
    જો (p == ખાલી || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	જો (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	નહીંતર
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((અક્ષર *)buf, len, (અક્ષર *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    જો (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    પાછા plen;
}

#જો defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    શૂન્ય
win_redr_custom(
    win_T	*wp,
    પૂર્ણાંક		draw_ruler)	// TRUE or FALSE
{
    સ્થિર પૂર્ણાંક	entered = FALSE;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		curattr;
    પૂર્ણાંક		row;
    પૂર્ણાંક		col = 0;
    પૂર્ણાંક		maxwidth;
    પૂર્ણાંક		width;
    પૂર્ણાંક		n;
    પૂર્ણાંક		len;
    પૂર્ણાંક		stlh_cnt;
    પૂર્ણાંક		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    પૂર્ણાંક		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    પૂર્ણાંક		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    જો (entered)
	પાછા;
    entered = TRUE;

    // setup environment for the task at hand
    જો (wp == ખાલી)
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
    નહીંતર
    {
	override_success = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	પૂર્ણાંક in_status_લીટી = wp->w_status_height != 0;
	maxwidth = in_status_લીટી ? wp->w_width : cmdline_width;

	જો (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    જો (*stl == '%')
	    {
		જો (*++stl == '-')
		    stl++;
		જો (atoi((અક્ષર *)stl))
		    જ્યારે (VIM_ISDIGIT(*stl))
			stl++;
		જો (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    જો (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    જો (!in_status_લીટી)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	નહીંતર
	{
	    opt_name = (char_u *)"statusline";
	    જો (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    નહીંતર
		stl = p_stl;
	}

	જો (in_status_લીટી)
	    col += wp->w_wincol;
	નહીંતર
	    col += cmdline_col_off;
    }

    જો (maxwidth <= 0)
	જાઓ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == ખાલી ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == ખાલી) ? (char_u *)"" : stl;
    પૂર્ણાંક col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = ખાલી;
    પૂર્ણાંક			*out_ગણતરી = ખાલી;
    પૂર્ણાંક			region_base_col = 0;
    જો (!draw_ruler)
    {
	જો (wp != ખાલી)
	{
	    out_regions = &wp->w_stl_click;
	    out_ગણતરી = &wp->w_stl_click_ગણતરી;
	    region_base_col = wp->w_wincol;
	}
	નહીંતર
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_ગણતરી = &tabline_stl_click_ગણતરી;
	    region_base_col = firstwin->w_wincol;
	}

	માટે (n = 0; n < *out_ગણતરી; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_ગણતરી = 0;
    }

    પૂર્ણાંક carry_hl = 0;
    માટે (પૂર્ણાંક i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, કદ(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	જો (p != ખાલી)
	{
	    len = vim_snprintf((અક્ષર *)buf, કદ(buf), "%s", p);
	    vim_free(p);
	}
	નહીંતર
	    len = (પૂર્ણાંક)STRLEN(buf);

	// fill up with "fillchar"
	જ્યારે (width < maxwidth && len < (પૂર્ણાંક)કદ(buf) - 1)
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
	માટે (n = 0; hltab[n].start != ખાલી; n++)
	{
	    len = (પૂર્ણાંક)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    જો (hltab[n].userhl == 0)
		curattr = attr;
	    નહીંતર જો (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    નહીંતર જો (wp != ખાલી && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    નહીંતર જો (wp != ખાલી && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    નહીંતર જો (wp != ખાલી && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    નહીંતર
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	જો (out_regions != ખાલી)
	{
	    પૂર્ણાંક click_ગણતરી = 0;
	    માટે (n = 0; clicktab[n].start != ખાલી; n++)
		click_ગણતરી++;

	    જો (click_ગણતરી > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			કદ(stl_click_region_T)
				    * (*out_ગણતરી + click_ગણતરી));
		જો (new_arr != ખાલી)
		{
		    char_u  *cur_funcname = ખાલી;
		    પૂર્ણાંક	    cur_minwid = 0;
		    પૂર્ણાંક	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    માટે (n = 0; clicktab[n].start != ખાલી; n++)
		    {
			len += vim_strnsize(p,
					    (પૂર્ણાંક)(clicktab[n].start - p));
			p = clicktab[n].start;

			જો (cur_funcname != ખાલી)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_ગણતરી];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_ગણતરી)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    જો (cur_funcname != ખાલી)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_ગણતરી];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_ગણતરી)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	માટે (n = 0; clicktab[n].start != ખાલી; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    જો (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    જો (stl != ખાલી)
	vim_free(stl_tmp);

    જો (wp == ખાલી)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	માટે (n = 0; tabtab[n].start != ખાલી; n++)
	{
	    len += vim_strnsize(p, (પૂર્ણાંક)(tabtab[n].start - p));
	    જ્યારે (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	જ્યારે (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    જો (override_success)
	કાઢો_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    શૂન્ય
screen_putchar(પૂર્ણાંક c, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    જો (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    નહીંતર
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
    શૂન્ય
screen_getbytes(પૂર્ણાંક row, પૂર્ણાંક col, char_u *bytes, પૂર્ણાંક *attrp)
{
    unsigned off;

    // safety check
    જો (Screen_લીટીઓ == ખાલી || row >= screen_Rows || col >= screen_Columns)
	પાછા;

    off = લીટી_Offset[row] + col;
    જો (attrp != ખાલી)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_લીટીઓ[off];
    bytes[1] = NUL;

    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    નહીંતર જો (enc_dbcs == DBCS_JPNU && Screen_લીટીઓ[off] == 0x8e)
    {
	bytes[0] = Screen_લીટીઓ[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    નહીંતર જો (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_લીટીઓ[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    સ્થિર પૂર્ણાંક
screen_comp_differs(પૂર્ણાંક off, પૂર્ણાંક *u8cc)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < Screen_mco; ++i)
    {
	જો (Screen_લીટીઓ_C[i][off] != (u8char_T)u8cc[i])
	    પાછા TRUE;
	જો (u8cc[i] == 0)
	    તોડો;
    }
    પાછા FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    શૂન્ય
screen_puts(
    char_u	*text,
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    પૂર્ણાંક		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    શૂન્ય
screen_puts_len(
    char_u	*text,
    પૂર્ણાંક		textlen,
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    પૂર્ણાંક		attr_arg)
{
    પૂર્ણાંક		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    પૂર્ણાંક		len = textlen;
    પૂર્ણાંક		c;
    unsigned	max_off;
    પૂર્ણાંક		mbyte_blen = 1;
    પૂર્ણાંક		mbyte_cells = 1;
    પૂર્ણાંક		u8c = 0;
    પૂર્ણાંક		u8cc[MAX_MCO];
    પૂર્ણાંક		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    પૂર્ણાંક		prev_c = 0;		// previous Arabic character
    પૂર્ણાંક		pc, nc, nc1;
    પૂર્ણાંક		pcc[MAX_MCO];
#endif
    પૂર્ણાંક		force_redraw_this;
    પૂર્ણાંક		force_redraw_next = FALSE;
    પૂર્ણાંક		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    જો (Screen_લીટીઓ == ખાલી
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	પાછા;
    off = લીટી_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    જો (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	જો (!skip_for_popup(row, col - 1))
	{
	    Screen_લીટીઓ[off - 1] = ' ';
	    જો (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    નહીંતર જો (enc_utf8)
	    {
		Screen_લીટીઓ_UC[off - 1] = 0;
		Screen_લીટીઓ_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = લીટી_Offset[row] + screen_Columns;
    જ્યારે (col < screen_Columns
	    && (len < 0 || (પૂર્ણાંક)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	જો (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (પૂર્ણાંક)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    જો (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    નહીંતર જો (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    નહીંતર	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (પૂર્ણાંક)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		જો (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    જો (len >= 0 && (પૂર્ણાંક)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    નહીંતર
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(પૂર્ણાંક)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		નહીંતર
		    prev_c = u8c;
#endif
		જો (col + mbyte_cells > screen_Columns)
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
	પૂર્ણાંક cell_attr = attr;
	જો (screen_pum_blend > 0 && pum_bg_attrs != ખાલી
		&& row >= pum_bg_ટોચ && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    પૂર્ણાંક soff = (row - pum_bg_ટોચ) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_લીટીઓ[off] != c
		|| (mbyte_cells == 2
		    && Screen_લીટીઓ[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_લીટીઓ_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_લીટીઓ_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	જો ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#જો defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    જો (need_redraw && Screen_લીટીઓ[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# જો defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		પૂર્ણાંક	n = ScreenAttrs[off];

		જો (n > HL_ALL)
		    n = syn_attr2attr(n);
		જો (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    જો (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    જો (clear_next_cell)
		clear_next_cell = FALSE;
	    નહીંતર જો (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    જો (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_લીટીઓ[off + mbyte_blen] = 0;
	    Screen_લીટીઓ[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    જો (enc_utf8)
	    {
		જો (c < 0x80 && u8cc[0] == 0)
		    Screen_લીટીઓ_UC[off] = 0;
		નહીંતર
		{
		    પૂર્ણાંક	    i;

		    Screen_લીટીઓ_UC[off] = u8c;
		    માટે (i = 0; i < Screen_mco; ++i)
		    {
			Screen_લીટીઓ_C[i][off] = u8cc[i];
			જો (u8cc[i] == 0)
			    તોડો;
		    }
		}
		જો (mbyte_cells == 2)
		{
		    Screen_લીટીઓ[off + 1] = 0;
		    Screen_લીટીઓ_UC[off + 1] = 0;
		    માટે (પૂર્ણાંક ci = 0; ci < Screen_mco; ++ci)
			Screen_લીટીઓ_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    નહીંતર જો (mbyte_cells == 2)
	    {
		Screen_લીટીઓ[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    નહીંતર જો (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    નહીંતર
		screen_char(off, row, col);
	}
	જો (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    જો (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	નહીંતર
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    જો (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	જો (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	નહીંતર
	    screen_char(off, row, col);
    }
}

#જો defined(FEAT_શોધ_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    શૂન્ય
start_શોધ_hl(શૂન્ય)
{
    જો (!p_hls || no_hlsearch)
	પાછા;

    end_શોધ_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_શોધ_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    શૂન્ય
end_શોધ_hl(શૂન્ય)
{
    જો (screen_શોધ_hl.rm.regprog == ખાલી)
	પાછા;

    vim_regfree(screen_શોધ_hl.rm.regprog);
    screen_શોધ_hl.rm.regprog = ખાલી;
}
#endif

    સ્થિર શૂન્ય
screen_start_highlight(પૂર્ણાંક attr)
{
    attrentry_T *aep = ખાલી;

    screen_attr = attr;
    જો (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	પાછા;

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	અક્ષર	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	પાછા;
    }
#endif

    જો (attr > HL_ALL)				// special HL attr.
    {
	જો (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	નહીંતર
	    aep = syn_term_attr2entry(attr);
	જો (aep == ખાલી)	    // did ":syntax clear"
	    attr = 0;
	નહીંતર
	    attr = aep->ae_attr;
    }
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    જો (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	પૂર્ણાંક	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	જો (p_tgc)
	{
	    જો (aep == ખાલી || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    જો (aep == ખાલી || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	નહીંતર જો (t_colors >= 256)
	{
	    જો (aep == ખાલી || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    જો (aep == ખાલી || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    જો ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    નહીંતર જો (aep != ખાલી && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    જો ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    જો ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    જો ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    જો ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    જો ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    જો (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    જો ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    જો ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    જો ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    જો (aep != ખાલી)
    {
	જો (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	જો (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    જો (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	નહીંતર
#endif
	    જો (t_colors > 1)
	    {
		જો (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	જો (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    જો (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	નહીંતર
#endif
	    જો (t_colors > 1)
	    {
		જો (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	જો (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    જો (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	નહીંતર
#endif
	    જો (t_colors > 1)
	    {
		જો (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	જો (!IS_CTERM)
	{
	    જો (aep->ae_u.term.start != ખાલી)
		out_str(aep->ae_u.term.start);
	}
    }
}

    શૂન્ય
screen_stop_highlight(શૂન્ય)
{
    પૂર્ણાંક	    do_ME = FALSE;	    // output T_ME code
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    પૂર્ણાંક	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    જો (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	જો (gui.in_use)
	{
	    અક્ષર	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	નહીંતર
#endif
	{
	    પૂર્ણાંક is_under;

	    જો (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		જો (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    જો (aep != ખાલી && ((
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
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    જો (use_vtp())
		    {
			જો (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			જો (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			જો (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    નહીંતર
		    {
			// Process FG and BG at once.
			જો (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		નહીંતર
		{
		    aep = syn_term_attr2entry(screen_attr);
		    જો (aep != ખાલી && aep->ae_u.term.stop != ખાલી)
		    {
			જો (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			નહીંતર
			    out_str(aep->ae_u.term.stop);
		    }
		}
		જો (aep == ખાલી)	    // did ":syntax clear"
		    screen_attr = 0;
		નહીંતર
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    જો (screen_attr & HL_STANDOUT)
	    {
		જો (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		નહીંતર
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    જો (is_under && *T_UCE != NUL)
	    {
		જો (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		નહીંતર
		    out_str(T_UCE);
	    }
	    જો ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		જો (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		નહીંતર
		    out_str(T_UE);
	    }
	    જો (screen_attr & HL_ITALIC)
	    {
		જો (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		નહીંતર
		    out_str(T_CZR);
	    }
	    જો (screen_attr & HL_STRIKETHROUGH)
	    {
		જો (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		નહીંતર
		    out_str(T_STE);
	    }
	    જો (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    જો (p_tgc)
	    {
		જો (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		જો (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		જો (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    નહીંતર
#endif
	    {
		જો (t_colors > 1)
		{
		    // set Normal cterm colors
		    જો (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    જો (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    જો (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    જો (cterm_normal_fg_bold)
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
    શૂન્ય
reset_cterm_colors(શૂન્ય)
{
    જો (!IS_CTERM)
	પાછા;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    જો (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#નહીંતર
	જો (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    જો (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    શૂન્ય
screen_char(unsigned off, પૂર્ણાંક row, પૂર્ણાંક col)
{
    પૂર્ણાંક		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    જો (row >= screen_Rows || col >= screen_Columns)
	પાછા;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    જો (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0
		&& utf_char2cells(Screen_લીટીઓ_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    પૂર્ણાંક off2 = off + 1;
	    Screen_લીટીઓ[off2] = ' ';
	    Screen_લીટીઓ_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0
		&& utf_char2cells(Screen_લીટીઓ_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	પાછા;
    }
    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0
	    && utf_char2cells(Screen_લીટીઓ_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	પાછા;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    જો (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	પાછા;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    જો (screen_char_attr != 0)
	attr = screen_char_attr;
    નહીંતર
	attr = ScreenAttrs[off];
    જો (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0
	    && utf_char2cells(Screen_લીટીઓ_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    જો (screen_attr != attr)
	screen_start_highlight(attr);

    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	જો (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_લીટીઓ_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	નહીંતર જો (utf_ambiguous_width(Screen_લીટીઓ_UC[off]))
	{
	    જો (*p_ambw == 'd'
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
	નહીંતર જો (utf_char2cells(Screen_લીટીઓ_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    નહીંતર
    {
	out_flush_check();
	out_char(Screen_લીટીઓ[off]);
	// double-byte character in single-width cell
	જો (enc_dbcs == DBCS_JPNU && Screen_લીટીઓ[off] == 0x8e)
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
    સ્થિર શૂન્ય
screen_char_2(unsigned off, પૂર્ણાંક row, પૂર્ણાંક col)
{
    // Check for illegal values (could be wrong when screen was resized).
    જો (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	પાછા;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    જો (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	પાછા;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    જો (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	પાછા;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_લીટીઓ[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    શૂન્ય
screen_draw_rectangle(
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    પૂર્ણાંક		height,
    પૂર્ણાંક		width,
    પૂર્ણાંક		invert,
    પૂર્ણાંક		force)
{
    પૂર્ણાંક		r, c;
    પૂર્ણાંક		off;
    પૂર્ણાંક		max_off;

    // Can't use ScreenLines unless initialized
    જો (Screen_લીટીઓ == ખાલી)
	પાછા;

    જો (invert)
	screen_char_attr = HL_INVERSE;
    માટે (r = row; r < row + height; ++r)
    {
	off = લીટી_Offset[r];
	max_off = off + screen_Columns;
	માટે (c = col; c < col + width; ++c)
	{
	    જો (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		જો (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    નહીંતર
	    {
		જો (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		જો (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    સ્થિર શૂન્ય
redraw_block(પૂર્ણાંક row, પૂર્ણાંક end, win_T *wp)
{
    પૂર્ણાંક		col;
    પૂર્ણાંક		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    જો (wp == ખાલી)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    નહીંતર
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    શૂન્ય
space_to_screenline(પૂર્ણાંક off, પૂર્ણાંક attr)
{
    Screen_લીટીઓ[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    જો (enc_utf8)
	Screen_લીટીઓ_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    શૂન્ય
screen_fill(
	પૂર્ણાંક	start_row,
	પૂર્ણાંક	end_row,
	પૂર્ણાંક	start_col,
	પૂર્ણાંક	end_col,
	પૂર્ણાંક	c1,
	પૂર્ણાંક	c2,
	પૂર્ણાંક	attr)
{
    પૂર્ણાંક	    row;
    પૂર્ણાંક	    col;
    પૂર્ણાંક	    off;
    પૂર્ણાંક	    end_off;
    પૂર્ણાંક	    did_delete;
    પૂર્ણાંક	    c;
    પૂર્ણાંક	    norm_term;
#જો defined(FEAT_GUI) || defined(UNIX)
    પૂર્ણાંક	    force_next = FALSE;
#endif

    જો (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    જો (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    જો (Screen_લીટીઓ == ખાલી
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	પાછા;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    માટે (row = start_row; row < end_row; ++row)
    {
	જો (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    જો (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		પૂર્ણાંક ડાબું_attr = ScreenAttrs[લીટી_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, ડાબું_attr);
	    }
	    જો (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		પૂર્ણાંક જમણું_attr = ScreenAttrs[લીટી_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, જમણું_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	જો (c2 == ' '
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
	    જો (c1 != ' ')			// don't clear first char
		++col;

	    off = લીટી_Offset[row] + col;
	    end_off = લીટી_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    જો (enc_utf8)
		જ્યારે (off < end_off && Screen_લીટીઓ[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_લીટીઓ_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (પૂર્ણાંક)(off - લીટી_Offset[row]))
#endif
			  )
		    ++off;
	    નહીંતર
		જ્યારે (off < end_off && Screen_લીટીઓ[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (પૂર્ણાંક)(off - લીટી_Offset[row]))
#endif
			  )
		    ++off;
	    જો (off < end_off)		// something to be cleared
	    {
		col = off - લીટી_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		જ્યારે (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (પૂર્ણાંક)(off - લીટી_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = લીટી_Offset[row] + start_col;
	c = c1;
	માટે (col = start_col; col < end_col; ++col)
	{
	    જો ((Screen_લીટીઓ[off] != c
		    || (enc_utf8 && (પૂર્ણાંક)Screen_લીટીઓ_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#જો defined(FEAT_GUI) || defined(UNIX)
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
		જો (screen_opacity_popup != ખાલી && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    જો (Screen_લીટીઓ[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_લીટીઓ_UC[off - 1] != 0
			    && utf_char2cells(Screen_લીટીઓ_UC[off - 1]) == 2)
			જાઓ next_col;
		}
		જો (screen_opacity_popup != ખાલી && c == ' '
						       && Screen_લીટીઓ[off] != 0)
		{
		    પૂર્ણાંક bg_char_cells = 1;
		    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_લીટીઓ_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    જો (bg_char_cells == 2)
		    {
			જો (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    જાઓ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			જો (c2 != ' ')
			    જાઓ skip_opacity_fill;
		    }

		    પૂર્ણાંક popup_attr = get_win_attr(screen_opacity_popup);
		    પૂર્ણાંક blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    જાઓ next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		જો (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != ખાલી
			&& row >= pum_bg_ટોચ && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    પૂર્ણાંક soff = (row - pum_bg_ટોચ) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી
			    && col > start_col
			    && pum_bg_લીટીઓ_UC[soff] == 0
			    && pum_bg_લીટીઓ_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_લીટીઓ_UC[soff - 1]) == 2)
		    {
			Screen_લીટીઓ[off] = 0;
			જો (Screen_લીટીઓ_UC != ખાલી)
			    Screen_લીટીઓ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			જાઓ next_col;
		    }

		    પૂર્ણાંક underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી
			    && ((pum_bg_લીટીઓ_UC[soff] != 0
				    && utf_char2cells(pum_bg_લીટીઓ_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_લીટીઓ_UC[soff] == 0
				    && col > 0
				    && pum_bg_લીટીઓ_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_લીટીઓ_UC[soff - 1])
								       == 2)))
		    {
			Screen_લીટીઓ[off] = ' ';
			જો (Screen_લીટીઓ_UC != ખાલી)
			    Screen_લીટીઓ_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			જાઓ next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_લીટીઓ[off] = pum_bg_લીટીઓ[soff];
		    જો (enc_utf8 && pum_bg_લીટીઓ_UC != ખાલી
						&& Screen_લીટીઓ_UC != ખાલી)
		    {
			પૂર્ણાંક k;
			Screen_લીટીઓ_UC[off] = pum_bg_લીટીઓ_UC[soff];
			માટે (k = 0; k < MAX_MCO; ++k)
			    જો (pum_bg_લીટીઓ_C[k] != ખાલી
						    && Screen_લીટીઓ_C[k] != ખાલી)
				Screen_લીટીઓ_C[k][off] = pum_bg_લીટીઓ_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    જાઓ next_col;
		}
#જો defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		જો (
# ifdef FEAT_GUI
			gui.in_use
# endif
# જો defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    જો (Screen_લીટીઓ[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    નહીંતર
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		જો (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_લીટીઓ[off] = c;
		જો (enc_utf8)
		{
		    જો (c >= 0x80)
		    {
			Screen_લીટીઓ_UC[off] = c;
			Screen_લીટીઓ_C[0][off] = 0;
			Screen_લીટીઓ[off] = 0x80; // avoid storing zero
		    }
		    નહીંતર
			Screen_લીટીઓ_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		જો (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    જો (col == start_col)
	    {
		જો (did_delete)
		    તોડો;
		c = c2;
	    }
	}
	જો (end_col == Columns)
	    લીટી_Wraps[row] = FALSE;
	જો (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    જો (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    જો (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    શૂન્ય
check_for_delay(પૂર્ણાંક check_સંદેશ_scroll)
{
    જો ((emsg_on_display || (check_સંદેશ_scroll && સંદેશ_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	જો (check_સંદેશ_scroll)
	    સંદેશ_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    સ્થિર શૂન્ય
clear_TabPageIdxs(શૂન્ય)
{
    પૂર્ણાંક		scol;

    માટે (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    પૂર્ણાંક
screen_valid(પૂર્ણાંક doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    પાછા (Screen_લીટીઓ != ખાલી);
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
    શૂન્ય
screenalloc(પૂર્ણાંક doclear)
{
    પૂર્ણાંક		    new_row, old_row;
#ifdef FEAT_GUI
    પૂર્ણાંક		    old_Rows;
#endif
    win_T	    *wp;
    પૂર્ણાંક		    outofmem = FALSE;
    પૂર્ણાંક		    len;
    schar_T	    *new_Screen_લીટીઓ;
    u8char_T	    *new_Screen_લીટીઓ_UC = ખાલી;
    u8char_T	    *new_Screen_લીટીઓ_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = ખાલી;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_લીટી_Offset;
    char_u	    *new_લીટી_Wraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    અક્ષર	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    સ્થિર પૂર્ણાંક	    entered = FALSE;		// avoid recursiveness
    સ્થિર પૂર્ણાંક	    done_outofmem_સંદેશ = FALSE;	// did outofmem message
    પૂર્ણાંક		    retry_ગણતરી = 0;
    પૂર્ણાંક		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    જો ((Screen_લીટીઓ != ખાલી
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_લીટીઓ_UC != ખાલી)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != ખાલી)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_લીટીઓ == ખાલી))
	પાછા;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    જો (entered)
	પાછા;
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
    માટે (પૂર્ણાંક i = 0; i < AUCMD_WIN_ગણતરી; ++i)
	જો (aucmd_win[i].auc_win != ખાલી)
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

    new_Screen_લીટીઓ = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_લીટીઓ_C, 0, કદ(u8char_T *) * MAX_MCO);
    જો (enc_utf8)
    {
	new_Screen_લીટીઓ_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
	    new_Screen_લીટીઓ_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    જો (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_લીટી_Offset = LALLOC_MULT(unsigned, Rows);
    new_લીટી_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(અક્ષર, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	જો (win_alloc_લીટીઓ(wp) == FAIL)
	{
	    outofmem = TRUE;
	    જાઓ give_up;
	}
    }
    માટે (પૂર્ણાંક i = 0; i < AUCMD_WIN_ગણતરી; ++i)
	જો (aucmd_win[i].auc_win != ખાલી
		&& aucmd_win[i].auc_win->w_લીટીઓ == ખાલી
		&& win_alloc_લીટીઓ(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    તોડો;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	જો (win_alloc_લીટીઓ(wp) == FAIL)
	{
	    outofmem = TRUE;
	    જાઓ give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    જો (win_alloc_લીટીઓ(wp) == FAIL)
	    {
		outofmem = TRUE;
		જાઓ give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
	જો (new_Screen_લીટીઓ_C[i] == ખાલી)
	{
	    found_null = TRUE;
	    તોડો;
	}
    જો (new_Screen_લીટીઓ == ખાલી
	    || (enc_utf8 && (new_Screen_લીટીઓ_UC == ખાલી || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == ખાલી)
	    || new_ScreenAttrs == ખાલી
	    || new_ScreenCols == ખાલી
	    || new_લીટી_Offset == ખાલી
	    || new_લીટી_Wraps == ખાલી
	    || new_TabPageIdxs == ખાલી
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == ખાલી
	    || new_popup_mask_next == ખાલી
	    || new_popup_transparent == ખાલી
#endif
	    || outofmem)
    {
	જો (Screen_લીટીઓ != ખાલી || !done_outofmem_સંદેશ)
	{
	    // guess the size
	    do_outofmem_સંદેશ((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_સંદેશ = TRUE;
	}
	VIM_CLEAR(new_Screen_લીટીઓ);
	VIM_CLEAR(new_Screen_લીટીઓ_UC);
	માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_લીટીઓ_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_લીટી_Offset);
	VIM_CLEAR(new_લીટી_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    નહીંતર
    {
	done_outofmem_સંદેશ = FALSE;

	માટે (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_લીટી_Offset[new_row] = new_row * Columns;
	    new_લીટી_Wraps[new_row] = FALSE;

	    (શૂન્ય)vim_memset(new_Screen_લીટીઓ + new_row * Columns,
				  ' ', (size_t)Columns * કદ(schar_T));
	    જો (enc_utf8)
	    {
		(શૂન્ય)vim_memset(new_Screen_લીટીઓ_UC + new_row * Columns,
				   0, (size_t)Columns * કદ(u8char_T));
		માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
		    (શૂન્ય)vim_memset(new_Screen_લીટીઓ_C[i]
						      + new_row * Columns,
				   0, (size_t)Columns * કદ(u8char_T));
	    }
	    જો (enc_dbcs == DBCS_JPNU)
		(શૂન્ય)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * કદ(schar_T));
	    (શૂન્ય)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * કદ(sattr_T));
	    (શૂન્ય)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * કદ(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    જો (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		જો (old_row >= 0 && Screen_લીટીઓ != ખાલી)
		{
		    જો (screen_Columns < Columns)
			len = screen_Columns;
		    નહીંતર
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    જો (!(enc_utf8 && Screen_લીટીઓ_UC == ખાલી)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_લીટીઓ + new_લીટી_Offset[new_row],
				Screen_લીટીઓ + લીટી_Offset[old_row],
				(size_t)len * કદ(schar_T));
		    જો (enc_utf8 && Screen_લીટીઓ_UC != ખાલી
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_લીટીઓ_UC + new_લીટી_Offset[new_row],
				Screen_લીટીઓ_UC + લીટી_Offset[old_row],
				(size_t)len * કદ(u8char_T));
			માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_લીટીઓ_C[i]
						    + new_લીટી_Offset[new_row],
				Screen_લીટીઓ_C[i] + લીટી_Offset[old_row],
				(size_t)len * કદ(u8char_T));
		    }
		    જો (enc_dbcs == DBCS_JPNU && ScreenLines2 != ખાલી)
			mch_memmove(new_ScreenLines2 + new_લીટી_Offset[new_row],
				ScreenLines2 + લીટી_Offset[old_row],
				(size_t)len * કદ(schar_T));
		    mch_memmove(new_ScreenAttrs + new_લીટી_Offset[new_row],
			    ScreenAttrs + લીટી_Offset[old_row],
			    (size_t)len * કદ(sattr_T));
		    mch_memmove(new_ScreenCols + new_લીટી_Offset[new_row],
			    ScreenCols + લીટી_Offset[old_row],
			    (size_t)len * કદ(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_લીટી = new_Screen_લીટીઓ + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * કદ(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * કદ(અક્ષર));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_લીટીઓ = new_Screen_લીટીઓ;
    Screen_લીટીઓ_UC = new_Screen_લીટીઓ_UC;
    માટે (પૂર્ણાંક i = 0; i < p_mco; ++i)
	Screen_લીટીઓ_C[i] = new_Screen_લીટીઓ_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    લીટી_Offset = new_લીટી_Offset;
    લીટી_Wraps = new_લીટી_Wraps;
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
    જો (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    જો (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    નહીંતર જો (gui.in_use
	    && !gui.starting
	    && Screen_લીટીઓ != ખાલી
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	જો (સંદેશ_row >= Rows)		// Rows got smaller
	    સંદેશ_row = Rows - 1;		// put cursor at last row
	નહીંતર જો (Rows > old_Rows)	// Rows got bigger
	    સંદેશ_row += Rows - old_Rows; // put cursor in same place
	જો (સંદેશ_col >= cmdline_width)	// cmdline_width got smaller
	    સંદેશ_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    જો (starting == 0 && ++retry_ગણતરી <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, ખાલી, ખાલી, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	જાઓ retry;
    }
}

    શૂન્ય
free_screenlines(શૂન્ય)
{
    પૂર્ણાંક		i;

    VIM_CLEAR(Screen_લીટીઓ_UC);
    માટે (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_લીટીઓ_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_લીટીઓ);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(લીટી_Offset);
    VIM_CLEAR(લીટી_Wraps);
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
    પૂર્ણાંક
screenclear(શૂન્ય)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    પાછા screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    શૂન્ય
redraw_as_cleared(શૂન્ય)
{
    screenclear2(FALSE);
}

    સ્થિર પૂર્ણાંક
screenclear2(પૂર્ણાંક doclear)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    did_clear = FALSE;

    જો (starting == NO_SCREEN || Screen_લીટીઓ == ખાલી
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	પાછા FALSE;

#ifdef FEAT_GUI
    જો (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    માટે (i = 0; i < Rows; ++i)
    {
	lineclear(લીટી_Offset[i], (પૂર્ણાંક)Columns, 0);
	લીટી_Wraps[i] = FALSE;
    }

    જો (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	જો (suppressed_cells != ખાલી)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    નહીંતર
    {
	// can't clear the screen, mark all chars with invalid attributes
	માટે (i = 0; i < Rows; ++i)
	    lineinvalid(લીટી_Offset[i], (પૂર્ણાંક)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    જો (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    સંદેશ_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    સંદેશ_row = cmdline_row;	// put cursor on last line for messages
    સંદેશ_col = 0;
    screen_start();		// don't know where cursor is now
    સંદેશ_didany = FALSE;
    સંદેશ_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    પાછા did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    સ્થિર શૂન્ય
lineclear(unsigned off, પૂર્ણાંક width, પૂર્ણાંક attr)
{
    (શૂન્ય)vim_memset(Screen_લીટીઓ + off, ' ', (size_t)width * કદ(schar_T));
    જો (enc_utf8)
	(શૂન્ય)vim_memset(Screen_લીટીઓ_UC + off, 0,
					  (size_t)width * કદ(u8char_T));
    (શૂન્ય)vim_memset(ScreenAttrs + off, attr, (size_t)width * કદ(sattr_T));
    (શૂન્ય)vim_memset(ScreenCols + off, -1, (size_t)width * કદ(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    સ્થિર શૂન્ય
lineinvalid(unsigned off, પૂર્ણાંક width)
{
    (શૂન્ય)vim_memset(ScreenAttrs + off, -1, (size_t)width * કદ(sattr_T));
    (શૂન્ય)vim_memset(ScreenCols + off, -1, (size_t)width * કદ(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    શૂન્ય
લીટી_was_clobbered(પૂર્ણાંક screen_lnum)
{
    lineinvalid(લીટી_Offset[screen_lnum], (પૂર્ણાંક)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    સ્થિર શૂન્ય
linecopy(પૂર્ણાંક to, પૂર્ણાંક from, win_T *wp)
{
    unsigned	off_to = લીટી_Offset[to] + wp->w_wincol;
    unsigned	off_from = લીટી_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_લીટીઓ + off_to, Screen_લીટીઓ + off_from,
	    wp->w_width * કદ(schar_T));
    જો (enc_utf8)
    {
	પૂર્ણાંક	i;

	mch_memmove(Screen_લીટીઓ_UC + off_to, Screen_લીટીઓ_UC + off_from,
		wp->w_width * કદ(u8char_T));
	માટે (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_લીટીઓ_C[i] + off_to, Screen_લીટીઓ_C[i] + off_from,
		    wp->w_width * કદ(u8char_T));
    }
    જો (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * કદ(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * કદ(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * કદ(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    પૂર્ણાંક
can_clear(char_u *p)
{
    પાછા (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#નહીંતર
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
    શૂન્ય
screen_start(શૂન્ય)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    શૂન્ય
windgoto(પૂર્ણાંક row, પૂર્ણાંક col)
{
    sattr_T	    *p;
    પૂર્ણાંક		    i;
    પૂર્ણાંક		    plan;
    પૂર્ણાંક		    cost;
    પૂર્ણાંક		    wouldbe_col;
    પૂર્ણાંક		    noinvcurs;
    char_u	    *bs;
    પૂર્ણાંક		    goto_cost;
    પૂર્ણાંક		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    જો (Screen_લીટીઓ == ખાલી)
	પાછા;
    જો (col == screen_cur_col && row == screen_cur_row)
	પાછા;

    // Check for valid position.
    જો (row < 0)	// window without text lines?
	row = 0;
    જો (row >= screen_Rows)
	row = screen_Rows - 1;
    જો (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    જો (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    નહીંતર
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
    જો (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = ખાલી;			    // init for GCC
	attr = screen_attr;
	જો (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    જો (*T_LE)
		bs = T_LE;		    // "cursor left"
	    નહીંતર
		bs = T_BC;		    // "backspace character (old)
	    જો (*bs)
		cost = (screen_cur_col - col) * (પૂર્ણાંક)STRLEN(bs);
	    નહીંતર
		cost = 999;
	    જો (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    નહીંતર
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    જો (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	નહીંતર જો (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    જો (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	નહીંતર
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
	જો (i > 0)
	    cost += i;
	જો (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + લીટી_Offset[row] + wouldbe_col;
	    જ્યારે (i && *p++ == attr)
		--i;
	    જો (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		જો (*--p == 0)
		{
		    cost += noinvcurs;
		    જ્યારે (i && *p++ == 0)
			--i;
		}
		જો (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    જો (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		માટે (i = wouldbe_col; i < col; ++i)
		    જો (Screen_લીટીઓ_UC[લીટી_Offset[row] + i] != 0)
		    {
			cost = 999;
			તોડો;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    જો (cost < 999)
		માટે (i = wouldbe_col; i < col; ++i)
		    જો (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			તોડો;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	જો (cost < goto_cost)
	{
	    જો (plan == PLAN_LE)
	    {
		જો (noinvcurs)
		    screen_stop_highlight();
		જ્યારે (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    નહીંતર જો (plan == PLAN_CR)
	    {
		જો (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    નહીંતર જો (plan == PLAN_NL)
	    {
		જો (noinvcurs)
		    screen_stop_highlight();
		જ્યારે (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    જો (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		જો (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    જ્યારે (i-- > 0)
			out_char(*T_ND);
		}
		નહીંતર
		{
		    પૂર્ણાંક	off;

		    off = લીટી_Offset[row] + screen_cur_col;
		    જ્યારે (i-- > 0)
		    {
			જો (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_લીટીઓ[off]);
			જો (enc_dbcs == DBCS_JPNU
				&& Screen_લીટીઓ[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    નહીંતર
	cost = 999;

    જો (cost >= goto_cost)
    {
	જો (noinvcurs)
	    screen_stop_highlight();
	જો (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_જમણું(col - screen_cur_col);
	નહીંતર
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    શૂન્ય
setcursor(શૂન્ય)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    પૂર્ણાંક
cursor_screen_cells(શૂન્ય)
{
    પાછા has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    શૂન્ય
setcursor_mayforce(પૂર્ણાંક force)
{
    જો (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((પૂર્ણાંક)curwin->w_width - curwin->w_wcol
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
    પૂર્ણાંક
win_ins_લીટીઓ(
    win_T	*wp,
    પૂર્ણાંક		row,
    પૂર્ણાંક		લીટી_ગણતરી,
    પૂર્ણાંક		invalid,
    પૂર્ણાંક		mayclear)
{
    પૂર્ણાંક		did_delete;
    પૂર્ણાંક		nextrow;
    પૂર્ણાંક		lastrow;
    પૂર્ણાંક		retval;

    જો (invalid)
	wp->w_લીટીઓ_valid = 0;

    // with only a few lines it's not worth the effort
    જો (wp->w_height < 5)
	પાછા FAIL;

    // with the popup menu visible this might not work correctly
    જો (pum_visible())
	પાછા FAIL;

    જો (લીટી_ગણતરી > wp->w_height - row)
	લીટી_ગણતરી = wp->w_height - row;

    retval = win_do_લીટીઓ(wp, row, લીટી_ગણતરી, mayclear, FALSE, 0);
    જો (retval != MAYBE)
	પાછા retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    જો (wp->w_next != ખાલી || wp->w_status_height)
    {
	જો (screen_del_લીટીઓ(0, W_WINROW(wp) + wp->w_height - લીટી_ગણતરી,
				  લીટી_ગણતરી, (પૂર્ણાંક)Rows, FALSE, 0, ખાલી) == OK)
	    did_delete = TRUE;
	નહીંતર જો (wp->w_next)
	    પાછા FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    જો (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + લીટી_ગણતરી;
	જો (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - લીટી_ગણતરી, lastrow - લીટી_ગણતરી,
		  wp->w_wincol, (પૂર્ણાંક)W_ENDCOL(wp), ' ', ' ', 0);
    }

    જો (screen_ins_લીટીઓ(0, W_WINROW(wp) + row, લીટી_ગણતરી, (પૂર્ણાંક)Rows, 0, ખાલી)
								      == FAIL)
    {
	// deletion will have messed up other windows
	જો (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    પૂર્ણાંક
win_del_લીટીઓ(
    win_T	*wp,
    પૂર્ણાંક		row,
    પૂર્ણાંક		લીટી_ગણતરી,
    પૂર્ણાંક		invalid,
    પૂર્ણાંક		mayclear,
    પૂર્ણાંક		clear_attr)	    // for clearing lines
{
    પૂર્ણાંક		retval;

    જો (invalid)
	wp->w_લીટીઓ_valid = 0;

    જો (લીટી_ગણતરી > wp->w_height - row)
	લીટી_ગણતરી = wp->w_height - row;

    retval = win_do_લીટીઓ(wp, row, લીટી_ગણતરી, mayclear, TRUE, clear_attr);
    જો (retval != MAYBE)
	પાછા retval;

    જો (screen_del_લીટીઓ(0, W_WINROW(wp) + row, લીટી_ગણતરી,
				   (પૂર્ણાંક)Rows, FALSE, clear_attr, ખાલી) == FAIL)
	પાછા FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    જો (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	જો (screen_ins_લીટીઓ(0, W_WINROW(wp) + wp->w_height - લીટી_ગણતરી,
			      લીટી_ગણતરી, (પૂર્ણાંક)Rows, clear_attr, ખાલી) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    નહીંતર
	redraw_cmdline = TRUE;
    પાછા OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    સ્થિર પૂર્ણાંક
win_do_લીટીઓ(
    win_T	*wp,
    પૂર્ણાંક		row,
    પૂર્ણાંક		લીટી_ગણતરી,
    પૂર્ણાંક		mayclear,
    પૂર્ણાંક		del,
    પૂર્ણાંક		clear_attr)
{
    પૂર્ણાંક		retval;

    જો (!redrawing() || લીટી_ગણતરી <= 0)
	પાછા FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    જો (no_win_do_લીટીઓ_ins && !del)
	પાછા FAIL;

    // only a few lines left: redraw is faster
    જો (mayclear && Rows - લીટી_ગણતરી < 5
	    && wp->w_width == topframe->fr_width)
    {
	જો (!no_win_do_લીટીઓ_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	પાછા FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    જો (popup_visible)
	પાછા FAIL;
#endif

    // Delete all remaining lines
    જો (row + લીટી_ગણતરી >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (પૂર્ણાંક)W_ENDCOL(wp), ' ', ' ', 0);
	પાછા OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    જો (!no_win_do_લીટીઓ_ins)
	clear_cmdline = TRUE;

#જો defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    જો (tabpanel_width() > 0)
	પાછા FAIL;
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
    જો (scroll_region || wp->w_width != topframe->fr_width)
    {
	જો (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	જો (del)
	    retval = screen_del_લીટીઓ(W_WINROW(wp) + row, 0, લીટી_ગણતરી,
				    wp->w_height - row, FALSE, clear_attr, wp);
	નહીંતર
	    retval = screen_ins_લીટીઓ(W_WINROW(wp) + row, 0, લીટી_ગણતરી,
					   wp->w_height - row, clear_attr, wp);
	જો (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	પાછા retval;
    }

    જો (wp->w_next != ખાલી && p_tf) // don't delete/insert on fast terminal
	પાછા FAIL;

    પાછા MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    સ્થિર શૂન્ય
win_rest_invalid(win_T *wp)
{
    જ્યારે (wp != ખાલી)
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
    પૂર્ણાંક
screen_ins_લીટીઓ(
    પૂર્ણાંક		off,
    પૂર્ણાંક		row,
    પૂર્ણાંક		લીટી_ગણતરી,
    પૂર્ણાંક		end,
    પૂર્ણાંક		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    unsigned	temp;
    પૂર્ણાંક		cursor_row;
    પૂર્ણાંક		cursor_col = 0;
    પૂર્ણાંક		type;
    પૂર્ણાંક		result_empty;
    પૂર્ણાંક		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    જો (!screen_valid(TRUE)
	     || લીટી_ગણતરી <= 0 || લીટી_ગણતરી > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	પાછા FAIL;

#ifdef FEAT_PROP_POPUP
    જો (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, લીટી_ગણતરી);
# endif
	પાછા FAIL;
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
    result_empty = (row + લીટી_ગણતરી >= end);
    જો (wp != ખાલી && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	જો (લીટી_ગણતરી > 3)
	    પાછા FAIL;
	type = USE_REDRAW;
    }
    નહીંતર જો (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    નહીંતર જો (*T_CAL != NUL && (લીટી_ગણતરી > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    નહીંતર જો (*T_CDL != NUL && result_empty && (લીટી_ગણતરી > 1 || !can_ce))
	type = USE_T_CDL;
    નહીંતર જો (*T_AL != NUL)
	type = USE_T_AL;
    નહીંતર જો (can_ce && result_empty)
	type = USE_T_CE;
    નહીંતર જો (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    નહીંતર જો (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    નહીંતર
	પાછા FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    જો (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	પાછા screen_del_લીટીઓ(off, row, લીટી_ગણતરી, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    જો (*T_DB)
	screen_del_લીટીઓ(off, end - લીટી_ગણતરી, લીટી_ગણતરી, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    જો (off + row > 0 || (wp != ખાલી && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    નહીંતર
	clip_scroll_selection(-લીટી_ગણતરી);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, લીટી_ગણતરી);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    જો (wp != ખાલી && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    જો (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    નહીંતર
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    માટે (i = 0; i < લીટી_ગણતરી; ++i)
    {
	જો (wp != ખાલી && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    જ્યારે ((j -= લીટી_ગણતરી) >= row)
		linecopy(j + લીટી_ગણતરી, j, wp);
	    j += લીટી_ગણતરી;
	    જો (can_clear((char_u *)" "))
		lineclear(લીટી_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    નહીંતર
		lineinvalid(લીટી_Offset[j] + wp->w_wincol, wp->w_width);
	    લીટી_Wraps[j] = FALSE;
	}
	નહીંતર
	{
	    j = end - 1 - i;
	    temp = લીટી_Offset[j];
	    જ્યારે ((j -= લીટી_ગણતરી) >= row)
	    {
		લીટી_Offset[j + લીટી_ગણતરી] = લીટી_Offset[j];
		લીટી_Wraps[j + લીટી_ગણતરી] = લીટી_Wraps[j];
	    }
	    લીટી_Offset[j + લીટી_ગણતરી] = temp;
	    લીટી_Wraps[j + લીટી_ગણતરી] = FALSE;
	    જો (can_clear((char_u *)" "))
		lineclear(temp, (પૂર્ણાંક)Columns, clear_attr);
	    નહીંતર
		lineinvalid(temp, (પૂર્ણાંક)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    જો (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    જો (type == USE_REDRAW)
	redraw_block(row, end, wp);
    નહીંતર જો (type == USE_T_CAL)
    {
	term_append_લીટીઓ(લીટી_ગણતરી);
	screen_start();		// don't know where cursor is now
    }
    નહીંતર
    {
	માટે (i = 0; i < લીટી_ગણતરી; i++)
	{
	    જો (type == USE_T_AL)
	    {
		જો (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    નહીંતર  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    જો (type == USE_T_SR && *T_DA)
    {
	માટે (i = 0; i < લીટી_ગણતરી; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    જો (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    પાછા OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    પૂર્ણાંક
screen_del_લીટીઓ(
    પૂર્ણાંક		off,
    પૂર્ણાંક		row,
    પૂર્ણાંક		લીટી_ગણતરી,
    પૂર્ણાંક		end,
    પૂર્ણાંક		force,		// even when line_count > p_ttyscroll
    પૂર્ણાંક		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    પૂર્ણાંક		j;
    પૂર્ણાંક		i;
    unsigned	temp;
    પૂર્ણાંક		cursor_row;
    પૂર્ણાંક		cursor_col = 0;
    પૂર્ણાંક		cursor_end;
    પૂર્ણાંક		result_empty;	// result is empty until end of region
    પૂર્ણાંક		can_delete;	// deleting line codes can be used
    પૂર્ણાંક		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    જો (!screen_valid(TRUE)
	    || લીટી_ગણતરી <= 0
	    || (!force && લીટી_ગણતરી > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	પાછા FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + લીટી_ગણતરી >= end;

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
    જો (wp != ખાલી && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	જો (લીટી_ગણતરી > 3)
	    પાછા FAIL;
	type = USE_REDRAW;
    }
    નહીંતર જો (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    નહીંતર જો (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    લીટી_ગણતરી == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    નહીંતર જો (*T_CDL != NUL && લીટી_ગણતરી > 1 && can_delete)
	type = USE_T_CDL;
    નહીંતર જો (can_clear(T_CE) && result_empty
	    && (wp == ખાલી || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    નહીંતર જો (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    નહીંતર જો (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    નહીંતર
	પાછા FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    જો (off + row > 0 || (wp != ખાલી && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    નહીંતર
	clip_scroll_selection(લીટી_ગણતરી);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -લીટી_ગણતરી);
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

    જો (wp != ખાલી && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    જો (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    નહીંતર
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
    માટે (i = 0; i < લીટી_ગણતરી; ++i)
    {
	જો (wp != ખાલી && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    જ્યારે ((j += લીટી_ગણતરી) <= end - 1)
		linecopy(j - લીટી_ગણતરી, j, wp);
	    j -= લીટી_ગણતરી;
	    જો (can_clear((char_u *)" "))
		lineclear(લીટી_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    નહીંતર
		lineinvalid(લીટી_Offset[j] + wp->w_wincol, wp->w_width);
	    લીટી_Wraps[j] = FALSE;
	}
	નહીંતર
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = લીટી_Offset[j];
	    જ્યારે ((j += લીટી_ગણતરી) <= end - 1)
	    {
		લીટી_Offset[j - લીટી_ગણતરી] = લીટી_Offset[j];
		લીટી_Wraps[j - લીટી_ગણતરી] = લીટી_Wraps[j];
	    }
	    લીટી_Offset[j - લીટી_ગણતરી] = temp;
	    લીટી_Wraps[j - લીટી_ગણતરી] = FALSE;
	    જો (can_clear((char_u *)" "))
		lineclear(temp, (પૂર્ણાંક)Columns, clear_attr);
	    નહીંતર
		lineinvalid(temp, (પૂર્ણાંક)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    જો (screen_attr != clear_attr)
	screen_stop_highlight();
    જો (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    જો (type == USE_REDRAW)
	redraw_block(row, end, wp);
    નહીંતર જો (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    નહીંતર જો (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_લીટીઓ(લીટી_ગણતરી);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    નહીંતર જો (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	માટે (i = લીટી_ગણતરી; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    નહીંતર
    {
	માટે (i = લીટી_ગણતરી; --i >= 0; )
	{
	    જો (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    નહીંતર // type == USE_T_CE
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
    જો (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	માટે (i = લીટી_ગણતરી; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    જો (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    પાછા OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    પૂર્ણાંક
skip_showmode(શૂન્ય)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    જો (global_busy
	    || સંદેશ_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !ચાવી_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	પાછા TRUE;
    }
    પાછા FALSE;
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
    પૂર્ણાંક
showmode(શૂન્ય)
{
    પૂર્ણાંક		need_clear;
    પૂર્ણાંક		length = 0;
    પૂર્ણાંક		do_mode;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		nwr_save;
    પૂર્ણાંક		sub_attr;
    પૂર્ણાંક		show_ruler_with_pum = FALSE;

    do_mode = p_smd && સંદેશ_silent == 0
	    && ((State & MODE_ઉમેરો)
		|| restart_edit != NUL
		|| VIsual_active);
    જો (do_mode || reg_recording != 0)
    {
	જો (skip_showmode())
	    પાછા 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	જો (clear_cmdline && cmdline_row < Rows - 1)
	    સંદેશ_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	સંદેશ_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	જો (do_mode)
	{
	    સંદેશ_puts_attr("--", attr);
#જો defined(FEAT_XIM)
	    જો (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# નહીંતર
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		સંદેશ_puts_attr(" IM", attr);
# નહીંતર
		સંદેશ_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    જો (edit_submode != ખાલી && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - સંદેશ_row) * cmdline_width - 3;
		જો (edit_submode_extra != ખાલી)
		    length -= vim_strsize(edit_submode_extra);
		જો (length > 0)
		{
		    જો (edit_submode_pre != ખાલી)
			length -= vim_strsize(edit_submode_pre);
		    જો (length - vim_strsize(edit_submode) > 0)
		    {
			જો (edit_submode_pre != ખાલી)
			    સંદેશ_puts_attr((અક્ષર *)edit_submode_pre, attr);
			સંદેશ_puts_attr((અક્ષર *)edit_submode, attr);
		    }
		    જો (edit_submode_extra != ખાલી)
		    {
			સંદેશ_puts_attr(" ", attr);  // add a space in between
			જો ((પૂર્ણાંક)edit_submode_highl < (પૂર્ણાંક)HLF_ગણતરી)
			    sub_attr = HL_ATTR(edit_submode_highl);
			નહીંતર
			    sub_attr = attr;
			સંદેશ_puts_attr((અક્ષર *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    નહીંતર
	    {
		જો (State & VREPLACE_FLAG)
		    સંદેશ_puts_attr(_(" VREPLACE"), attr);
		નહીંતર જો (State & REPLACE_FLAG)
		    સંદેશ_puts_attr(_(" REPLACE"), attr);
		નહીંતર જો (State & MODE_ઉમેરો)
		{
#ifdef FEAT_RIGHTLEFT
		    જો (p_ri)
			સંદેશ_puts_attr(_(" REVERSE"), attr);
#endif
		    સંદેશ_puts_attr(_(" INSERT"), attr);
		}
		નહીંતર જો (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    સંદેશ_puts_attr(_(" (insert)"), attr);
		નહીંતર જો (restart_edit == 'R')
		    સંદેશ_puts_attr(_(" (replace)"), attr);
		નહીંતર જો (restart_edit == 'V')
		    સંદેશ_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		જો (p_hkmap)
		    સંદેશ_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		જો (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    જો (curwin->w_p_arab)
			સંદેશ_puts_attr(_(" Arabic"), attr);
		    નહીંતર
# endif
			જો (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    સંદેશ_puts_attr((અક્ષર *)NameBuff, attr);
		}
#endif
		જો ((State & MODE_ઉમેરો) && p_paste)
		    સંદેશ_puts_attr(_(" (paste)"), attr);

		જો (VIsual_active)
		{
		    અક્ષર *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    પસંદગી ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			સ્થિતિ 0:	p = N_(" VISUAL"); તોડો;
			સ્થિતિ 1: p = N_(" VISUAL LINE"); તોડો;
			સ્થિતિ 2: p = N_(" VISUAL BLOCK"); તોડો;
			સ્થિતિ 4: p = N_(" SELECT"); તોડો;
			સ્થિતિ 5: p = N_(" SELECT LINE"); તોડો;
			મૂળભૂત: p = N_(" SELECT BLOCK"); તોડો;
		    }
		    સંદેશ_puts_attr(_(p), attr);
		}
		સંદેશ_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	જો (reg_recording != 0
		&& edit_submode == ખાલી)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	જો (need_clear || clear_cmdline || redraw_mode)
	    સંદેશ_clr_eos();
	સંદેશ_didout = FALSE;		// overwrite this message
	length = સંદેશ_col;
	સંદેશ_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    નહીંતર જો (clear_cmdline && સંદેશ_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	સંદેશ_clr_cmdline();
    નહીંતર જો (redraw_mode)
    {
	સંદેશ_pos_mode();
	સંદેશ_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    જો (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    જો (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    પાછા length;
}

/*
 * Position for a mode message.
 */
    સ્થિર શૂન્ય
સંદેશ_pos_mode(શૂન્ય)
{
    સંદેશ_col = 0;
    સંદેશ_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    શૂન્ય
unshowmode(પૂર્ણાંક force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    જો (!redrawing() || (!force && char_avail() && !ચાવી_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    નહીંતર
	clearmode();
}

/*
 * Clear the mode message.
 */
    શૂન્ય
clearmode(શૂન્ય)
{
    પૂર્ણાંક save_સંદેશ_row = સંદેશ_row;
    પૂર્ણાંક save_સંદેશ_col = સંદેશ_col;

    સંદેશ_pos_mode();
    જો (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    સંદેશ_clr_eos();

    સંદેશ_col = save_સંદેશ_col;
    સંદેશ_row = save_સંદેશ_row;
}

    સ્થિર શૂન્ય
recording_mode(પૂર્ણાંક attr)
{
    સંદેશ_puts_attr(_("recording"), attr);
    જો (shortmess(SHM_RECORDING))
	પાછા;

    અક્ષર s[4];

    sprintf(s, " @%c", reg_recording);
    સંદેશ_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    સ્થિર પૂર્ણાંક
mouse_has_any(શૂન્ય)
{
    પાછા mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_ઉમેરો)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    શૂન્ય
draw_tabline(શૂન્ય)
{
    પૂર્ણાંક		tabcount = 0;
    tabpage_T	*tp;
    પૂર્ણાંક		tabwidth;
    પૂર્ણાંક		col = 0;
    પૂર્ણાંક		scol = 0;
    પૂર્ણાંક		attr;
    win_T	*wp;
    win_T	*cwp;
    પૂર્ણાંક		wincount;
    પૂર્ણાંક		modified;
    પૂર્ણાંક		c;
    પૂર્ણાંક		len;
    પૂર્ણાંક		attr_sel;
    પૂર્ણાંક		attr_nosel = 0;
    પૂર્ણાંક		attr_fill = 0;
    char_u	*p;
    પૂર્ણાંક		room;
    પૂર્ણાંક		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#જો defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    જો (Screen_લીટીઓ == ખાલી)
	પાછા;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    જો (gui_use_tabline())
    {
	gui_update_tabline();
	પાછા;
    }
#endif

    જો (tabline_height() < 1)
	પાછા;

#જો defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    જો (*p_tal != NUL)
	win_redr_custom(ખાલી, FALSE);
    નહીંતર
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	જો (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	માટે (tp = first_tabpage; tp != ખાલી && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = ધકેલો_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    જો (tp->tp_topframe == topframe)
		attr = attr_sel;
	    જો (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    જો (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    જો (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    નહીંતર
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    માટે (wincount = 0; wp != ખાલી; wp = wp->w_next, ++wincount)
		જો (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    જો (modified || wincount > 1)
	    {
		જો (wincount > 1)
		{
		    len = vim_snprintf((અક્ષર *)NameBuff, MAXPATHL, "%d", wincount);
		    જો (col + len >= Columns - 3)
			તોડો;
		    screen_puts_len(NameBuff, len, 0, col,
#જો defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#નહીંતર
					 attr
#endif
					       );
		    col += len;
		}
		જો (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    જો (room > 0)
	    {
		પૂર્ણાંક n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		જો (has_mbyte)
		    જ્યારે (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		નહીંતર જો (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		જો (len > n)
		    len = n;

		screen_puts_len(p, (પૂર્ણાંક)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    જ્યારે (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    જો (override_success)
		કાઢો_highlight_overrides();
	}

	જો (use_sep_chars)
	    c = '_';
	નહીંતર
	    c = ' ';
	screen_fill(0, 1, col, (પૂર્ણાંક)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	જો (p_sc && *p_sloc == 't')
	{
	    પૂર્ણાંક	n = (પૂર્ણાંક)Columns - col - (tabcount > 1) * 3;
	    પૂર્ણાંક	width = MIN(10, n);

	    જો (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (પૂર્ણાંક)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	જો (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (પૂર્ણાંક)Columns - 1, attr_nosel);
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
    શૂન્ય
get_trans_bufname(buf_T *buf)
{
    જો (buf_spname(buf) != ખાલી)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    નહીંતર
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    પૂર્ણાંક
fillchar_status(પૂર્ણાંક *attr, win_T *wp)
{
    પૂર્ણાંક	    fill;
    bool    override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    જો (bt_terminal(wp->w_buffer))
    {
	જો (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	નહીંતર
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    નહીંતર
#endif
    જો (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    નહીંતર
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    જો (override_success)
	કાઢો_highlight_overrides();
    પાછા fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    સ્થિર bool
vsep_row_is_curwin(win_T *wp, પૂર્ણાંક row)
{
    જો (wp == curwin)
	પાછા true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    જો (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	પાછા true;

    પાછા false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    પૂર્ણાંક
fillchar_vsep(પૂર્ણાંક *attr, win_T *wp, પૂર્ણાંક row)
{
    bool override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);
    જો (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    નહીંતર
	*attr = HL_ATTR(HLF_CNC);
    જો (override_success)
	કાઢો_highlight_overrides();

    જો (*attr == 0 && wp->w_fill_chars.vert == ' ')
	પાછા '|';
    નહીંતર
	પાછા wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    પૂર્ણાંક
redrawing(શૂન્ય)
{
#ifdef FEAT_EVAL
    જો (disable_redraw_for_testing)
	પાછા 0;
    નહીંતર
#endif
	પાછા ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !ચાવી_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    પૂર્ણાંક
messaging(શૂન્ય)
{
    પાછા (!(p_lz && char_avail() && !ચાવી_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    શૂન્ય
comp_col(શૂન્ય)
{
    પૂર્ણાંક last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    જો (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#નહીંતર
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	જો (!last_has_status)
	    sc_col = ru_col;
    }
    જો (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	જો (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    જો (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    જો (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#જો defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    પૂર્ણાંક
સંખ્યા_width(win_T *wp)
{
    પૂર્ણાંક		n;
    linenr_T	lnum;

    જો (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    નહીંતર
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;

    જો (lnum == wp->w_nrwidth_લીટી_ગણતરી && wp->w_nuw_cached == wp->w_p_nuw)
	પાછા wp->w_nrwidth_width;
    wp->w_nrwidth_લીટી_ગણતરી = lnum;

    n = 0;
    કરો
    {
	lnum /= 10;
	++n;
    } જ્યારે (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    જો (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    જો (n < 2 && get_first_valid_sign(wp) != ખાલી
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    પાછા n;
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    પૂર્ણાંક
screen_screencol(શૂન્ય)
{
    પાછા screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    પૂર્ણાંક
screen_screenrow(શૂન્ય)
{
    પાછા screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    સ્થિર પૂર્ણાંક
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    જો (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T અંક = 0;
	પૂર્ણાંક	    bytes;
	પૂર્ણાંક	    n;

	માટે (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    જો (n < 0)
		પાછા 0;
	    અંક = અંક * 256 + n;
	}
	*p += 2;
	પાછા અંક;
    }
    પાછા mb_ptr2char_adv(p);
}

સંરચના charstab
{
    પૂર્ણાંક	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

સ્થિર fill_chars_T fill_chars;
સ્થિર સંરચના charstab filltab[] =
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
#જો defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
સ્થિર lcs_chars_T lcs_chars;
સ્થિર સંરચના charstab lcstab[] =
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
#નહીંતર
    CHARSTAB_ENTRY(ખાલી,		    "conceal"),
#endif
    CHARSTAB_ENTRY(ખાલી,		    "multispace"),
    CHARSTAB_ENTRY(ખાલી,		    "leadmultispace")
};

    સ્થિર અક્ષર *
field_કિંમત_err(અક્ષર *errbuf, size_t errbuflen, અક્ષર *fmt, char_u *field)
{
    જો (errbuf == ખાલી)
	પાછા "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    પાછા errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    સ્થિર અક્ષર *
set_chars_option(win_T *wp, char_u *કિંમત, પૂર્ણાંક is_listchars, પૂર્ણાંક apply,
						અક્ષર *errbuf, size_t errbuflen)
{
    પૂર્ણાંક	    round, i, entries;
    char_u  *p, *s;
    પૂર્ણાંક	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = ખાલી;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = ખાલી; // Last occurrence of "leadmultispace:"
    પૂર્ણાંક	    multispace_len = 0;	      // Length of lcs-multispace string
    પૂર્ણાંક	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    સંરચના charstab *tab;

    જો (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	જો (wp->w_p_lcs[0] == NUL)
	    કિંમત = p_lcs;  // local value is empty, use the global value
    }
    નહીંતર
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	જો (wp->w_p_fcs[0] == NUL)
	    કિંમત = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    માટે (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	પૂર્ણાંક has_tab = FALSE, has_leadtab = FALSE;

	જો (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    જો (is_listchars)
	    {
		માટે (i = 0; i < entries; ++i)
		    જો (tab[i].cp != ખાલી)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		જો (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(પૂર્ણાંક, multispace_len + 1);
		    જો (lcs_chars.multispace != ખાલી)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		નહીંતર
		    lcs_chars.multispace = ખાલી;

		જો (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(પૂર્ણાંક, lead_multispace_len + 1);
		    જો (lcs_chars.leadmultispace != ખાલી)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		નહીંતર
		    lcs_chars.leadmultispace = ખાલી;
	    }
	    નહીંતર
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
#જો defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = કિંમત;
	જ્યારે (*p)
	{
	    માટે (i = 0; i < entries; ++i)
	    {
		જો (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    ચાલુ;

		s = p + tab[i].name.length + 1;

		જો (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    જો (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			જ્યારે (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    જો (char2cells(c1) > 1)
				પાછા field_કિંમત_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			જો (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    પાછા field_કિંમત_err(errbuf, errbuflen,
				    e_wrong_સંખ્યા_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    નહીંતર
		    {
			પૂર્ણાંક multispace_pos = 0;

			જ્યારે (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    જો (p == last_multispace && lcs_chars.multispace != ખાલી)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    તોડો;
		}

		જો (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    જો (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			જ્યારે (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    જો (char2cells(c1) > 1)
				પાછા field_કિંમત_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			જો (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    પાછા field_કિંમત_err(errbuf, errbuflen,
				    e_wrong_સંખ્યા_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    નહીંતર
		    {
			પૂર્ણાંક multispace_pos = 0;

			જ્યારે (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    જો (p == last_lmultispace && lcs_chars.leadmultispace != ખાલી)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    તોડો;
		}

		c2 = c3 = 0;
		જો (*s == NUL)
		    પાછા field_કિંમત_err(errbuf, errbuflen,
				    e_wrong_સંખ્યા_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		જો (char2cells(c1) > 1)
		    પાછા field_કિંમત_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		જો (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    જો (*s == NUL)
			પાછા field_કિંમત_err(errbuf, errbuflen,
				    e_wrong_સંખ્યા_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    જો (char2cells(c2) > 1)
			પાછા field_કિંમત_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    જો (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			જો (char2cells(c3) > 1)
			    પાછા field_કિંમત_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    જો (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    નહીંતર  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		જો (*s == ',' || *s == NUL)
		{
		    જો (round > 0)
		    {
			જો (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			નહીંતર જો (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			નહીંતર જો (tab[i].cp != ખાલી)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    તોડો;
		}
		નહીંતર
		    પાછા field_કિંમત_err(errbuf, errbuflen,
				    e_wrong_સંખ્યા_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    જો (i == entries)
		પાછા e_invalid_argument;

	    જો (*p == ',')
		++p;
	}

	જો (is_listchars && has_leadtab && !has_tab)
	    પાછા e_leadtab_requires_tab;
    }

    જો (apply)
    {
	જો (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	નહીંતર
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    પાછા ખાલી;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    અક્ષર *
set_fillchars_option(win_T *wp, char_u *val, પૂર્ણાંક apply, અક્ષર *errbuf,
							      size_t errbuflen)
{
    પાછા set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    અક્ષર *
set_listchars_option(win_T *wp, char_u *val, પૂર્ણાંક apply, અક્ષર *errbuf,
							      size_t errbuflen)
{
    પાછા set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(filltab))
	પાછા ખાલી;

    પાછા filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(lcstab))
	પાછા ખાલી;

    પાછા lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    અક્ષર *
check_chars_options(શૂન્ય)
{
    tabpage_T   *tp;
    win_T	    *wp;

    જો (set_listchars_option(curwin, p_lcs, FALSE, ખાલી, 0) != ખાલી)
	પાછા e_conflicts_with_કિંમત_of_listchars;
    જો (set_fillchars_option(curwin, p_fcs, FALSE, ખાલી, 0) != ખાલી)
	પાછા e_conflicts_with_કિંમત_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	જો (set_listchars_option(wp, wp->w_p_lcs, FALSE, ખાલી, 0) != ખાલી)
	    પાછા e_conflicts_with_કિંમત_of_listchars;
	જો (set_fillchars_option(wp, wp->w_p_fcs, FALSE, ખાલી, 0) != ખાલી)
	    પાછા e_conflicts_with_કિંમત_of_fillchars;
    }
    પાછા ખાલી;
}
