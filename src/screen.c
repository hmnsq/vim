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

#अन्तर्भाव "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
स्थिर पूर्णाङ्क	screen_attr = 0;

स्थिर शून्य screen_char_2(unsigned off, पूर्णाङ्क row, पूर्णाङ्क col);
स्थिर पूर्णाङ्क screenclear2(पूर्णाङ्क doclear);
स्थिर शून्य lineclear(unsigned off, पूर्णाङ्क width, पूर्णाङ्क attr);
स्थिर शून्य lineinvalid(unsigned off, पूर्णाङ्क width);
स्थिर पूर्णाङ्क win_do_पङ्क्तयः(win_T *wp, पूर्णाङ्क row, पूर्णाङ्क पङ्क्तिः_गणनम्, पूर्णाङ्क mayclear, पूर्णाङ्क del, पूर्णाङ्क clear_attr);
स्थिर शून्य win_rest_invalid(win_T *wp);
स्थिर शून्य सन्देशः_pos_mode(शून्य);
स्थिर शून्य recording_mode(पूर्णाङ्क attr);

// Ugly global: overrule attribute used by screen_char()
स्थिर पूर्णाङ्क screen_char_attr = 0;

#जइ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    पूर्णाङ्क
conceal_cursor_पङ्क्तिः(win_T *wp)
{
    पूर्णाङ्क		c;

    जइ (*wp->w_p_cocu == NUL)
	प्रत्यागम FALSE;
    जइ (get_real_state() & MODE_VISUAL)
	c = 'v';
    अण्णहा जइ (State & MODE_निवेशय)
	c = 'i';
    अण्णहा जइ (State & MODE_NORMAL)
	c = 'n';
    अण्णहा जइ (State & MODE_CMDLINE)
	c = 'c';
    अण्णहा
	प्रत्यागम FALSE;
    प्रत्यागम vim_strchr(wp->w_p_cocu, c) != रिक्त;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    शून्य
conceal_check_cursor_पङ्क्तिः(पूर्णाङ्क was_concealed)
{
    जइ (curwin->w_p_cole <= 0 || conceal_cursor_पङ्क्तिः(curwin) == was_concealed)
	प्रत्यागम;

    पूर्णाङ्क wcol = curwin->w_wcol;

    need_cursor_पङ्क्तिः_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    जइ (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    पूर्णाङ्क
get_win_attr(win_T *wp)
{
    पूर्णाङ्क	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);

    जइ (win_attr != 0)
    {
	जइ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	अण्णहा
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    अण्णहा जइ (WIN_IS_POPUP(wp))
    {
	जइ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	अण्णहा
	    win_attr = HL_ATTR(HLF_निष्कासय);    // Popup
    }
#endif

    जइ (override_success)
	निष्कासय_highlight_overrides();
    प्रत्यागम win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    स्थिर पूर्णाङ्क
screen_fill_end(
	win_T *wp,
	पूर्णाङ्क	c1,
	पूर्णाङ्क	c2,
	पूर्णाङ्क	off,
	पूर्णाङ्क	width,
	पूर्णाङ्क	row,
	पूर्णाङ्क	endrow,
	पूर्णाङ्क	attr)
{
    पूर्णाङ्क	    nn = off + width;

    जइ (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    जइ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (पूर्णाङ्क)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    अण्णहा
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (पूर्णाङ्क)wp->w_wincol + nn,
		c1, c2, attr);
    प्रत्यागम nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    शून्य
win_draw_end(
    win_T	*wp,
    पूर्णाङ्क		c1,
    पूर्णाङ्क		c2,
    पूर्णाङ्क		draw_margin,
    पूर्णाङ्क		row,
    पूर्णाङ्क		endrow,
    hlf_T	hl)
{
    पूर्णाङ्क		n = 0;
    पूर्णाङ्क		attr = HL_ATTR(hl);
    पूर्णाङ्क		win_attr = get_win_attr(wp);
    bool	override_success =
	प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    जइ (draw_margin)
    {
#ifdef FEAT_FOLDING
	पूर्णाङ्क	fdc = compute_foldcolumn(wp, 0);

	जइ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	जइ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	जइ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == रिक्त)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, सङ्ख्या_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    जइ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    अण्णहा
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (पूर्णाङ्क)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    जइ (override_success)
	निष्कासय_highlight_overrides();
}

#जइ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    पूर्णाङ्क
compute_foldcolumn(win_T *wp, पूर्णाङ्क col)
{
    पूर्णाङ्क wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    पूर्णाङ्क n = wp->w_width - (col + wmw);

    प्रत्यागम MIN(wp->w_p_fdc, n);
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
    पूर्णाङ्क		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    पूर्णाङ्क		i = 0;
    पूर्णाङ्क		level;
    पूर्णाङ्क		first_level;
    पूर्णाङ्क		empty;
    पूर्णाङ्क		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    पूर्णाङ्क		चिह्नम् = 0;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    जइ (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    प्रति (i = 0; i < n; i++)
    {
	जइ (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_नीचः_level)
	    चिह्नम् = wp->w_fill_chars.foldopen;
	अण्णहा जइ (first_level == 1)
	    चिह्नम् = wp->w_fill_chars.foldsep;
	अण्णहा जइ (wp->w_fill_chars.foldinner != NUL)
	    चिह्नम् = wp->w_fill_chars.foldinner;
	अण्णहा जइ (first_level + i <= 9)
	    चिह्नम् = '0' + first_level + i;
	अण्णहा
	    चिह्नम् = '>';

	len = utf_char2bytes(चिह्नम्, &p[byte_counter]);
	byte_counter += len;
	जइ (first_level + i >= level)
	{
	    i++;
	    भङ्ग;
	}
    }

    जइ (closed)
    {
	जइ (चिह्नम् != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    जइ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	चिह्नम् = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(चिह्नम्, &p[byte_counter]);
	byte_counter += len;
    }

    प्रत्यागम MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    स्थिर पूर्णाङ्क
comp_char_differs(पूर्णाङ्क off_from, पूर्णाङ्क off_to)
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < Screen_mco; ++i)
    {
	जइ (Screen_पङ्क्तयः_C[i][off_from] != Screen_पङ्क्तयः_C[i][off_to])
	    प्रत्यागम TRUE;
	जइ (Screen_पङ्क्तयः_C[i][off_from] == 0)
	    भङ्ग;
    }
    प्रत्यागम FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    स्थिर पूर्णाङ्क
char_needs_redraw(पूर्णाङ्क off_from, पूर्णाङ्क off_to, पूर्णाङ्क cols)
{
    जइ (cols > 0
	    && ((Screen_पङ्क्तयः[off_from] != Screen_पङ्क्तयः[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_पङ्क्तयः[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_पङ्क्तयः[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_पङ्क्तयः[off_from + 1]
						 != Screen_पङ्क्तयः[off_to + 1])))
		|| (enc_utf8
		    && (Screen_पङ्क्तयः_UC[off_from] != Screen_पङ्क्तयः_UC[off_to]
			|| (Screen_पङ्क्तयः_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_पङ्क्तयः[off_from + 1]
						!= Screen_पङ्क्तयः[off_to + 1])))))
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#जइ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    पूर्णाङ्क
screen_get_current_पङ्क्तिः_off(शून्य)
{
    प्रत्यागम (पूर्णाङ्क)(current_Screen_पङ्क्तिः - Screen_पङ्क्तयः);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    स्थिर पूर्णाङ्क
blocked_by_popup(पूर्णाङ्क row, पूर्णाङ्क col)
{
    पूर्णाङ्क off;

    जइ (!popup_visible)
	प्रत्यागम FALSE;
    off = row * screen_Columns + col;
    प्रत्यागम popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    शून्य
reset_screen_attr(शून्य)
{
#ifdef FEAT_GUI
    जइ (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    अण्णहा
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    स्थिर पूर्णाङ्क
skip_for_popup(पूर्णाङ्क row, पूर्णाङ्क col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    जइ (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	प्रत्यागम TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    जइ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	प्रत्यागम TRUE;
#अण्णहा
    जइ (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	प्रत्यागम TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    जइ (blocked_by_popup(row, col))
	प्रत्यागम TRUE;
#endif
    प्रत्यागम FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
स्थिर char_u	*suppressed_cells = रिक्त;
स्थिर पूर्णाङ्क	suppressed_rows = 0;
स्थिर पूर्णाङ्क	suppressed_cols = 0;

    स्थिर शून्य
mark_suppressed_cell(पूर्णाङ्क row, पूर्णाङ्क col)
{
    जइ (suppressed_cells == रिक्त
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	जइ (suppressed_cells == रिक्त)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    प्रत्यागम;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    स्थिर शून्य
unmark_suppressed_cell(पूर्णाङ्क row, पूर्णाङ्क col)
{
    जइ (suppressed_cells != रिक्त
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    स्थिर पूर्णाङ्क
is_suppressed_cell(पूर्णाङ्क row, पूर्णाङ्क col)
{
    प्रत्यागम suppressed_cells != रिक्त
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
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		popup_attr;	// get_win_attr(screen_opacity_popup)
    पूर्णाङ्क		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    स्थिर अन्तःस्थ शून्य
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    स्थिर अन्तःस्थ पूर्णाङ्क
popup_base_attr_or(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क fallback)
{
    पूर्णाङ्क		base = fallback;

    popup_get_base_screen_cell(row, col, रिक्त, &base, रिक्त);
    प्रत्यागम base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    स्थिर अन्तःस्थ पूर्णाङ्क
popup_blend_with_base(
	ध्रुव popup_opacity_T	*po,
	पूर्णाङ्क			char_attr,
	पूर्णाङ्क			base_attr,
	पूर्णाङ्क			blend_fg)
{
    पूर्णाङ्क		combined = hl_combine_attr(po->popup_attr, char_attr);

    प्रत्यागम hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    स्थिर शून्य
resolve_wide_char_opacity_attrs(
	पूर्णाङ्क row, पूर्णाङ्क col1, पूर्णाङ्क col2,
	sattr_T *attr1, sattr_T *attr2)
{
    पूर्णाङ्क		bg1, bg2;
    पूर्णाङ्क		base1 = 0;
    पूर्णाङ्क		base2 = 0;
    attrentry_T	*ae;

    जइ (*attr1 == *attr2)
	प्रत्यागम;

    popup_get_base_screen_cell(row, col1, रिक्त, &base1, रिक्त);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != रिक्त && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# अण्णहा
    bg1 = (ae != रिक्त && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, रिक्त, &base2, रिक्त);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != रिक्त && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# अण्णहा
    bg2 = (ae != रिक्त && ae->ae_u.cterm.bg_color != 0);
# endif

    जइ (bg1 && !bg2)
	*attr1 = *attr2;
    अण्णहा जइ (!bg1 && bg2)
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
    शून्य
screen_पङ्क्तिः(
	win_T	*wp,
	पूर्णाङ्क	row,
	पूर्णाङ्क	coloff,
	पूर्णाङ्क	endcol,
	पूर्णाङ्क	clear_width,
	colnr_T	last_vcol,
	पूर्णाङ्क	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    पूर्णाङ्क		    col = 0;
    पूर्णाङ्क		    hl;
    पूर्णाङ्क		    force = FALSE;	// force update rest of the line
    पूर्णाङ्क		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    पूर्णाङ्क		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    पूर्णाङ्क		    changed_this;	// TRUE if character changed
    पूर्णाङ्क		    changed_next;	// TRUE if next character changed
#endif
    पूर्णाङ्क		    clear_next = FALSE;
    पूर्णाङ्क		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    पूर्णाङ्क		    drawing_opacity_popup =
				screen_opacity_popup != रिक्त
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    जइ (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    जइ (row >= Rows)
	row = Rows - 1;
    जइ (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_Screen_पङ्क्तिः - Screen_पङ्क्तयः);
    off_to = पङ्क्तिः_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = पङ्क्तिः_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    जइ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	जइ (clear_width > 0)
	{
	    पूर्णाङ्क clear_start = col;

	    जाव (col <= endcol && Screen_पङ्क्तयः[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_पङ्क्तयः_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    जइ (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    प्रति (पूर्णाङ्क i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = पङ्क्तिः_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    जइ (coloff > 0 && enc_utf8
		   && Screen_पङ्क्तयः[off_to] == 0
		   && Screen_पङ्क्तयः_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_पङ्क्तयः_UC[off_to - 1]) > 1)
    {
	Screen_पङ्क्तयः[off_to - 1] = ' ';
	Screen_पङ्क्तयः_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	जइ (screen_opacity_popup == रिक्त)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    जाव (col < endcol)
    {
	जइ (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	अण्णहा
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	जइ (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	जइ (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    जइ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    जइ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	जइ (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	जइ (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	पूर्णाङ्क opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	जइ (drawing_opacity_popup
		&& Screen_पङ्क्तयः[off_from] == ' '
		&& (!enc_utf8 || Screen_पङ्क्तयः_UC[off_from] == 0))
	{
	    पूर्णाङ्क popup_src_attr = ScreenAttrs[off_from];
	    पूर्णाङ्क bg_is_empty = Screen_पङ्क्तयः[off_to] == 0
				&& (!enc_utf8 || Screen_पङ्क्तयः_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    जइ (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_पङ्क्तयः_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_पङ्क्तयः_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    अण्णहा जइ (Screen_पङ्क्तयः[off_to] != 0
		    || (enc_utf8 && Screen_पङ्क्तयः_UC[off_to] != 0))
	    {
		पूर्णाङ्क bg_char_cells = (enc_utf8 && Screen_पङ्क्तयः_UC[off_to] != 0)
				? utf_char2cells(Screen_पङ्क्तयः_UC[off_to]) : 1;

		जइ (bg_char_cells == 2)
		{
		    जइ (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			पूर्णाङ्क base_attr;
			Screen_पङ्क्तयः[off_to] = ' ';
			जइ (enc_utf8)
			    Screen_पङ्क्तयः_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			गच्छ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    जइ (!(Screen_पङ्क्तयः[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_पङ्क्तयः_UC[off_from + 1] == 0)))
			गच्छ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		पूर्णाङ्क base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		जइ (bg_char_cells == 2)
		{
		    पूर्णाङ्क base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    जइ (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    अण्णहा जइ (bg_is_empty)
	    {
		पूर्णाङ्क base_attr;
		Screen_पङ्क्तयः[off_to] = ' ';
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

	जइ (redraw_this)
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
	    जइ (       p_wiv
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
		जइ (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		अण्णहा
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    जइ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		जइ (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_पङ्क्तयः[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		अण्णहा जइ (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_पङ्क्तयः[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		जइ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    जइ (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_पङ्क्तयः[off_to] = Screen_पङ्क्तयः[off_from];
	    जइ (enc_utf8)
	    {
		Screen_पङ्क्तयः_UC[off_to] = Screen_पङ्क्तयः_UC[off_from];
		जइ (Screen_पङ्क्तयः_UC[off_from] != 0)
		{
		    पूर्णाङ्क	    i;

		    प्रति (i = 0; i < Screen_mco; ++i)
			Screen_पङ्क्तयः_C[i][off_to] = Screen_पङ्क्तयः_C[i][off_from];
		}
	    }
	    जइ (char_cells == 2)
	    {
		Screen_पङ्क्तयः[off_to + 1] = Screen_पङ्क्तयः[off_from + 1];
		जइ (enc_utf8)
		    Screen_पङ्क्तयः_UC[off_to + 1] = 0;
	    }

#जइ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    जइ (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# जइ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		जइ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		जइ (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    जइ (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    जइ (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    जइ (drawing_opacity_popup && po.blend > 0)
	    {
		पूर्णाङ्क popup_src_attr = ScreenAttrs[off_from];
		पूर्णाङ्क scol1 = col + coloff;
		पूर्णाङ्क base1 = popup_base_attr_or(row, scol1, 0);

		जइ (char_cells == 2)
		{
		    पूर्णाङ्क base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    पूर्णाङ्क over1 = popup_is_over_opacity(row, scol1);
		    पूर्णाङ्क over2 = popup_is_over_opacity(row, scol1 + 1);
		    जइ (over1 != over2)
		    {
			जइ (over1)
			    base1 = base2;
			अण्णहा
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    अण्णहा
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    जइ (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    जइ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    अण्णहा
		screen_char(off_to, row, col + coloff);
	}
	अण्णहा जइ (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    जइ (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    अण्णहा जइ (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	जइ (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    जइ (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    जइ (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_पङ्क्तयः[off_to] = ' ';
	जइ (enc_utf8)
	    Screen_पङ्क्तयः_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	जइ (screen_opacity_popup == रिक्त)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    जइ (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	पूर्णाङ्क startCol = col;
#endif

	// blank out the rest of the line
	जाव (col < clear_width && Screen_पङ्क्तयः[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_पङ्क्तयः_UC[off_to] == 0)
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
	जइ (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    जइ (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		जइ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    पूर्णाङ्क prev_cells = 1;

		    जइ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_पङ्क्तयः[off_to - 1] == 0 ? 2 : 1;
		    अण्णहा जइ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = पङ्क्तिः_Offset[row];
			unsigned max_off = पङ्क्तिः_Offset[row] + screen_Columns;

			जाव (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    जइ (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			जइ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			अण्णहा
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    जाव (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    जइ (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	जइ (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    जइ (!skip_for_popup(row, col + coloff))
	    {
		पूर्णाङ्क c;

		c = fillchar_vsep(&hl, wp, row);
		जइ (Screen_पङ्क्तयः[off_to] != (schar_T)c
			|| (enc_utf8 && (पूर्णाङ्क)Screen_पङ्क्तयः_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_पङ्क्तयः[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    जइ (enc_utf8)
		    {
			जइ (c >= 0x80)
			{
			    Screen_पङ्क्तयः_UC[off_to] = c;
			    Screen_पङ्क्तयः_C[0][off_to] = 0;
			    Screen_पङ्क्तयः[off_to] = 0x80; // avoid storing zero
			}
			अण्णहा
			    Screen_पङ्क्तयः_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	अण्णहा
	    पङ्क्तिः_Wraps[row] = FALSE;
    }

    जइ (override_success)
	निष्कासय_highlight_overrides();
}

#जइ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    शून्य
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    पूर्णाङ्क		t;

    प्रति (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
    शून्य
draw_vsep_win(win_T *wp, पूर्णाङ्क row)
{
    जइ (!wp->w_vsep_width)
	प्रत्यागम;

    पूर्णाङ्क content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    पूर्णाङ्क start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    प्रति (पूर्णाङ्क r = start_row; r < content_end; ++r)
    {
	पूर्णाङ्क hl;
	पूर्णाङ्क c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    जइ (wp->w_status_height != 0)
    {
	पूर्णाङ्क hl;
	पूर्णाङ्क c;
	जइ (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	अण्णहा
	    c = fillchar_vsep(&hl, wp, content_end);
	प्रति (पूर्णाङ्क r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    पूर्णाङ्क
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    जाव (fr->fr_parent != रिक्त)
    {
	जइ (fr->fr_parent->fr_layout == FR_COL)
	{
	    जइ (fr->fr_next != रिक्त)
		भङ्ग;
	}
	अण्णहा
	{
	    जइ (fr->fr_next != रिक्त)
		प्रत्यागम TRUE;
	}
	fr = fr->fr_parent;
    }
    प्रत्यागम FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    पूर्णाङ्क
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    पूर्णाङ्क		len)	    // length of buffer
{
    char_u	*p;
    पूर्णाङ्क		plen;

    जइ (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	प्रत्यागम 0;

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
    जइ (p == रिक्त || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	जइ (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	अण्णहा
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((वर्ण *)buf, len, (वर्ण *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    जइ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    प्रत्यागम plen;
}

#जइ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    शून्य
win_redr_custom(
    win_T	*wp,
    पूर्णाङ्क		draw_ruler)	// TRUE or FALSE
{
    स्थिर पूर्णाङ्क	entered = FALSE;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		curattr;
    पूर्णाङ्क		row;
    पूर्णाङ्क		col = 0;
    पूर्णाङ्क		maxwidth;
    पूर्णाङ्क		width;
    पूर्णाङ्क		n;
    पूर्णाङ्क		len;
    पूर्णाङ्क		stlh_cnt;
    पूर्णाङ्क		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    पूर्णाङ्क		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    पूर्णाङ्क		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    जइ (entered)
	प्रत्यागम;
    entered = TRUE;

    // setup environment for the task at hand
    जइ (wp == रिक्त)
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
    अण्णहा
    {
	override_success = प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	पूर्णाङ्क in_status_पङ्क्तिः = wp->w_status_height != 0;
	maxwidth = in_status_पङ्क्तिः ? wp->w_width : cmdline_width;

	जइ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    जइ (*stl == '%')
	    {
		जइ (*++stl == '-')
		    stl++;
		जइ (atoi((वर्ण *)stl))
		    जाव (VIM_ISDIGIT(*stl))
			stl++;
		जइ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    जइ (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    जइ (!in_status_पङ्क्तिः)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	अण्णहा
	{
	    opt_name = (char_u *)"statusline";
	    जइ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    अण्णहा
		stl = p_stl;
	}

	जइ (in_status_पङ्क्तिः)
	    col += wp->w_wincol;
	अण्णहा
	    col += cmdline_col_off;
    }

    जइ (maxwidth <= 0)
	गच्छ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == रिक्त ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == रिक्त) ? (char_u *)"" : stl;
    पूर्णाङ्क col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = रिक्त;
    पूर्णाङ्क			*out_गणनम् = रिक्त;
    पूर्णाङ्क			region_base_col = 0;
    जइ (!draw_ruler)
    {
	जइ (wp != रिक्त)
	{
	    out_regions = &wp->w_stl_click;
	    out_गणनम् = &wp->w_stl_click_गणनम्;
	    region_base_col = wp->w_wincol;
	}
	अण्णहा
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_गणनम् = &tabline_stl_click_गणनम्;
	    region_base_col = firstwin->w_wincol;
	}

	प्रति (n = 0; n < *out_गणनम्; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_गणनम् = 0;
    }

    पूर्णाङ्क carry_hl = 0;
    प्रति (पूर्णाङ्क i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, परिमाण(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	जइ (p != रिक्त)
	{
	    len = vim_snprintf((वर्ण *)buf, परिमाण(buf), "%s", p);
	    vim_free(p);
	}
	अण्णहा
	    len = (पूर्णाङ्क)STRLEN(buf);

	// fill up with "fillchar"
	जाव (width < maxwidth && len < (पूर्णाङ्क)परिमाण(buf) - 1)
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
	प्रति (n = 0; hltab[n].start != रिक्त; n++)
	{
	    len = (पूर्णाङ्क)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    जइ (hltab[n].userhl == 0)
		curattr = attr;
	    अण्णहा जइ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    अण्णहा जइ (wp != रिक्त && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    अण्णहा जइ (wp != रिक्त && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    अण्णहा जइ (wp != रिक्त && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    अण्णहा
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	जइ (out_regions != रिक्त)
	{
	    पूर्णाङ्क click_गणनम् = 0;
	    प्रति (n = 0; clicktab[n].start != रिक्त; n++)
		click_गणनम्++;

	    जइ (click_गणनम् > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			परिमाण(stl_click_region_T)
				    * (*out_गणनम् + click_गणनम्));
		जइ (new_arr != रिक्त)
		{
		    char_u  *cur_funcname = रिक्त;
		    पूर्णाङ्क	    cur_minwid = 0;
		    पूर्णाङ्क	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    प्रति (n = 0; clicktab[n].start != रिक्त; n++)
		    {
			len += vim_strnsize(p,
					    (पूर्णाङ्क)(clicktab[n].start - p));
			p = clicktab[n].start;

			जइ (cur_funcname != रिक्त)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_गणनम्];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_गणनम्)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    जइ (cur_funcname != रिक्त)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_गणनम्];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_गणनम्)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	प्रति (n = 0; clicktab[n].start != रिक्त; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    जइ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    जइ (stl != रिक्त)
	vim_free(stl_tmp);

    जइ (wp == रिक्त)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	प्रति (n = 0; tabtab[n].start != रिक्त; n++)
	{
	    len += vim_strnsize(p, (पूर्णाङ्क)(tabtab[n].start - p));
	    जाव (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	जाव (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    जइ (override_success)
	निष्कासय_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    शून्य
screen_putchar(पूर्णाङ्क c, पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    जइ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    अण्णहा
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
    शून्य
screen_getbytes(पूर्णाङ्क row, पूर्णाङ्क col, char_u *bytes, पूर्णाङ्क *attrp)
{
    unsigned off;

    // safety check
    जइ (Screen_पङ्क्तयः == रिक्त || row >= screen_Rows || col >= screen_Columns)
	प्रत्यागम;

    off = पङ्क्तिः_Offset[row] + col;
    जइ (attrp != रिक्त)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_पङ्क्तयः[off];
    bytes[1] = NUL;

    जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    अण्णहा जइ (enc_dbcs == DBCS_JPNU && Screen_पङ्क्तयः[off] == 0x8e)
    {
	bytes[0] = Screen_पङ्क्तयः[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    अण्णहा जइ (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_पङ्क्तयः[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    स्थिर पूर्णाङ्क
screen_comp_differs(पूर्णाङ्क off, पूर्णाङ्क *u8cc)
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < Screen_mco; ++i)
    {
	जइ (Screen_पङ्क्तयः_C[i][off] != (u8char_T)u8cc[i])
	    प्रत्यागम TRUE;
	जइ (u8cc[i] == 0)
	    भङ्ग;
    }
    प्रत्यागम FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    शून्य
screen_puts(
    char_u	*text,
    पूर्णाङ्क		row,
    पूर्णाङ्क		col,
    पूर्णाङ्क		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    शून्य
screen_puts_len(
    char_u	*text,
    पूर्णाङ्क		textlen,
    पूर्णाङ्क		row,
    पूर्णाङ्क		col,
    पूर्णाङ्क		attr_arg)
{
    पूर्णाङ्क		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    पूर्णाङ्क		len = textlen;
    पूर्णाङ्क		c;
    unsigned	max_off;
    पूर्णाङ्क		mbyte_blen = 1;
    पूर्णाङ्क		mbyte_cells = 1;
    पूर्णाङ्क		u8c = 0;
    पूर्णाङ्क		u8cc[MAX_MCO];
    पूर्णाङ्क		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    पूर्णाङ्क		prev_c = 0;		// previous Arabic character
    पूर्णाङ्क		pc, nc, nc1;
    पूर्णाङ्क		pcc[MAX_MCO];
#endif
    पूर्णाङ्क		force_redraw_this;
    पूर्णाङ्क		force_redraw_next = FALSE;
    पूर्णाङ्क		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    जइ (Screen_पङ्क्तयः == रिक्त
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	प्रत्यागम;
    off = पङ्क्तिः_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    जइ (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	जइ (!skip_for_popup(row, col - 1))
	{
	    Screen_पङ्क्तयः[off - 1] = ' ';
	    जइ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    अण्णहा जइ (enc_utf8)
	    {
		Screen_पङ्क्तयः_UC[off - 1] = 0;
		Screen_पङ्क्तयः_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = पङ्क्तिः_Offset[row] + screen_Columns;
    जाव (col < screen_Columns
	    && (len < 0 || (पूर्णाङ्क)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	जइ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (पूर्णाङ्क)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    जइ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    अण्णहा जइ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    अण्णहा	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (पूर्णाङ्क)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		जइ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    जइ (len >= 0 && (पूर्णाङ्क)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    अण्णहा
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(पूर्णाङ्क)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		अण्णहा
		    prev_c = u8c;
#endif
		जइ (col + mbyte_cells > screen_Columns)
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
	पूर्णाङ्क cell_attr = attr;
	जइ (screen_pum_blend > 0 && pum_bg_attrs != रिक्त
		&& row >= pum_bg_शिखरम् && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    पूर्णाङ्क soff = (row - pum_bg_शिखरम्) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_पङ्क्तयः[off] != c
		|| (mbyte_cells == 2
		    && Screen_पङ्क्तयः[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_पङ्क्तयः_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_पङ्क्तयः_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	जइ ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#जइ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    जइ (need_redraw && Screen_पङ्क्तयः[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# जइ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		पूर्णाङ्क	n = ScreenAttrs[off];

		जइ (n > HL_ALL)
		    n = syn_attr2attr(n);
		जइ (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    जइ (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    जइ (clear_next_cell)
		clear_next_cell = FALSE;
	    अण्णहा जइ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    जइ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_पङ्क्तयः[off + mbyte_blen] = 0;
	    Screen_पङ्क्तयः[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    जइ (enc_utf8)
	    {
		जइ (c < 0x80 && u8cc[0] == 0)
		    Screen_पङ्क्तयः_UC[off] = 0;
		अण्णहा
		{
		    पूर्णाङ्क	    i;

		    Screen_पङ्क्तयः_UC[off] = u8c;
		    प्रति (i = 0; i < Screen_mco; ++i)
		    {
			Screen_पङ्क्तयः_C[i][off] = u8cc[i];
			जइ (u8cc[i] == 0)
			    भङ्ग;
		    }
		}
		जइ (mbyte_cells == 2)
		{
		    Screen_पङ्क्तयः[off + 1] = 0;
		    Screen_पङ्क्तयः_UC[off + 1] = 0;
		    प्रति (पूर्णाङ्क ci = 0; ci < Screen_mco; ++ci)
			Screen_पङ्क्तयः_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    अण्णहा जइ (mbyte_cells == 2)
	    {
		Screen_पङ्क्तयः[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    अण्णहा जइ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    अण्णहा
		screen_char(off, row, col);
	}
	जइ (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    जइ (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	अण्णहा
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    जइ (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	जइ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	अण्णहा
	    screen_char(off, row, col);
    }
}

#जइ defined(FEAT_अन्वेषणम्_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    शून्य
start_अन्वेषणम्_hl(शून्य)
{
    जइ (!p_hls || no_hlsearch)
	प्रत्यागम;

    end_अन्वेषणम्_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_अन्वेषणम्_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    शून्य
end_अन्वेषणम्_hl(शून्य)
{
    जइ (screen_अन्वेषणम्_hl.rm.regprog == रिक्त)
	प्रत्यागम;

    vim_regfree(screen_अन्वेषणम्_hl.rm.regprog);
    screen_अन्वेषणम्_hl.rm.regprog = रिक्त;
}
#endif

    स्थिर शून्य
screen_start_highlight(पूर्णाङ्क attr)
{
    attrentry_T *aep = रिक्त;

    screen_attr = attr;
    जइ (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	प्रत्यागम;

#ifdef FEAT_GUI
    जइ (gui.in_use)
    {
	वर्ण	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	प्रत्यागम;
    }
#endif

    जइ (attr > HL_ALL)				// special HL attr.
    {
	जइ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	अण्णहा
	    aep = syn_term_attr2entry(attr);
	जइ (aep == रिक्त)	    // did ":syntax clear"
	    attr = 0;
	अण्णहा
	    attr = aep->ae_attr;
    }
#जइ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    जइ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	पूर्णाङ्क	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	जइ (p_tgc)
	{
	    जइ (aep == रिक्त || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    जइ (aep == रिक्त || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	अण्णहा जइ (t_colors >= 256)
	{
	    जइ (aep == रिक्त || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    जइ (aep == रिक्त || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    जइ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    अण्णहा जइ (aep != रिक्त && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    जइ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    जइ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    जइ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    जइ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    जइ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    जइ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    जइ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    जइ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    जइ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    जइ (aep != रिक्त)
    {
	जइ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	जइ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    जइ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	अण्णहा
#endif
	    जइ (t_colors > 1)
	    {
		जइ (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	जइ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    जइ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	अण्णहा
#endif
	    जइ (t_colors > 1)
	    {
		जइ (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	जइ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    जइ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	अण्णहा
#endif
	    जइ (t_colors > 1)
	    {
		जइ (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	जइ (!IS_CTERM)
	{
	    जइ (aep->ae_u.term.start != रिक्त)
		out_str(aep->ae_u.term.start);
	}
    }
}

    शून्य
screen_stop_highlight(शून्य)
{
    पूर्णाङ्क	    do_ME = FALSE;	    // output T_ME code
#जइ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    पूर्णाङ्क	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    जइ (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	जइ (gui.in_use)
	{
	    वर्ण	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	अण्णहा
#endif
	{
	    पूर्णाङ्क is_under;

	    जइ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		जइ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    जइ (aep != रिक्त && ((
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
#जइ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    जइ (use_vtp())
		    {
			जइ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			जइ (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			जइ (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    अण्णहा
		    {
			// Process FG and BG at once.
			जइ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		अण्णहा
		{
		    aep = syn_term_attr2entry(screen_attr);
		    जइ (aep != रिक्त && aep->ae_u.term.stop != रिक्त)
		    {
			जइ (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			अण्णहा
			    out_str(aep->ae_u.term.stop);
		    }
		}
		जइ (aep == रिक्त)	    // did ":syntax clear"
		    screen_attr = 0;
		अण्णहा
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    जइ (screen_attr & HL_STANDOUT)
	    {
		जइ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		अण्णहा
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    जइ (is_under && *T_UCE != NUL)
	    {
		जइ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		अण्णहा
		    out_str(T_UCE);
	    }
	    जइ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		जइ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		अण्णहा
		    out_str(T_UE);
	    }
	    जइ (screen_attr & HL_ITALIC)
	    {
		जइ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		अण्णहा
		    out_str(T_CZR);
	    }
	    जइ (screen_attr & HL_STRIKETHROUGH)
	    {
		जइ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		अण्णहा
		    out_str(T_STE);
	    }
	    जइ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    जइ (p_tgc)
	    {
		जइ (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		जइ (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		जइ (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    अण्णहा
#endif
	    {
		जइ (t_colors > 1)
		{
		    // set Normal cterm colors
		    जइ (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    जइ (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    जइ (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    जइ (cterm_normal_fg_bold)
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
    शून्य
reset_cterm_colors(शून्य)
{
    जइ (!IS_CTERM)
	प्रत्यागम;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    जइ (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#अण्णहा
	जइ (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    जइ (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    शून्य
screen_char(unsigned off, पूर्णाङ्क row, पूर्णाङ्क col)
{
    पूर्णाङ्क		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    जइ (row >= screen_Rows || col >= screen_Columns)
	प्रत्यागम;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    जइ (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0
		&& utf_char2cells(Screen_पङ्क्तयः_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    पूर्णाङ्क off2 = off + 1;
	    Screen_पङ्क्तयः[off2] = ' ';
	    Screen_पङ्क्तयः_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0
		&& utf_char2cells(Screen_पङ्क्तयः_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	प्रत्यागम;
    }
    जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0
	    && utf_char2cells(Screen_पङ्क्तयः_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	प्रत्यागम;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    जइ (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	प्रत्यागम;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    जइ (screen_char_attr != 0)
	attr = screen_char_attr;
    अण्णहा
	attr = ScreenAttrs[off];
    जइ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0
	    && utf_char2cells(Screen_पङ्क्तयः_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    जइ (screen_attr != attr)
	screen_start_highlight(attr);

    जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	जइ (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_पङ्क्तयः_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	अण्णहा जइ (utf_ambiguous_width(Screen_पङ्क्तयः_UC[off]))
	{
	    जइ (*p_ambw == 'd'
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
	अण्णहा जइ (utf_char2cells(Screen_पङ्क्तयः_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    अण्णहा
    {
	out_flush_check();
	out_char(Screen_पङ्क्तयः[off]);
	// double-byte character in single-width cell
	जइ (enc_dbcs == DBCS_JPNU && Screen_पङ्क्तयः[off] == 0x8e)
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
    स्थिर शून्य
screen_char_2(unsigned off, पूर्णाङ्क row, पूर्णाङ्क col)
{
    // Check for illegal values (could be wrong when screen was resized).
    जइ (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	प्रत्यागम;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    जइ (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	प्रत्यागम;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    जइ (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	प्रत्यागम;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_पङ्क्तयः[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    शून्य
screen_draw_rectangle(
    पूर्णाङ्क		row,
    पूर्णाङ्क		col,
    पूर्णाङ्क		height,
    पूर्णाङ्क		width,
    पूर्णाङ्क		invert,
    पूर्णाङ्क		force)
{
    पूर्णाङ्क		r, c;
    पूर्णाङ्क		off;
    पूर्णाङ्क		max_off;

    // Can't use ScreenLines unless initialized
    जइ (Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम;

    जइ (invert)
	screen_char_attr = HL_INVERSE;
    प्रति (r = row; r < row + height; ++r)
    {
	off = पङ्क्तिः_Offset[r];
	max_off = off + screen_Columns;
	प्रति (c = col; c < col + width; ++c)
	{
	    जइ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		जइ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    अण्णहा
	    {
		जइ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		जइ (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    स्थिर शून्य
redraw_block(पूर्णाङ्क row, पूर्णाङ्क end, win_T *wp)
{
    पूर्णाङ्क		col;
    पूर्णाङ्क		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    जइ (wp == रिक्त)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    अण्णहा
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    शून्य
space_to_screenline(पूर्णाङ्क off, पूर्णाङ्क attr)
{
    Screen_पङ्क्तयः[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    जइ (enc_utf8)
	Screen_पङ्क्तयः_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    शून्य
screen_fill(
	पूर्णाङ्क	start_row,
	पूर्णाङ्क	end_row,
	पूर्णाङ्क	start_col,
	पूर्णाङ्क	end_col,
	पूर्णाङ्क	c1,
	पूर्णाङ्क	c2,
	पूर्णाङ्क	attr)
{
    पूर्णाङ्क	    row;
    पूर्णाङ्क	    col;
    पूर्णाङ्क	    off;
    पूर्णाङ्क	    end_off;
    पूर्णाङ्क	    did_delete;
    पूर्णाङ्क	    c;
    पूर्णाङ्क	    norm_term;
#जइ defined(FEAT_GUI) || defined(UNIX)
    पूर्णाङ्क	    force_next = FALSE;
#endif

    जइ (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    जइ (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    जइ (Screen_पङ्क्तयः == रिक्त
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	प्रत्यागम;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    प्रति (row = start_row; row < end_row; ++row)
    {
	जइ (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    जइ (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		पूर्णाङ्क वामः_attr = ScreenAttrs[पङ्क्तिः_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, वामः_attr);
	    }
	    जइ (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		पूर्णाङ्क दक्षिणः_attr = ScreenAttrs[पङ्क्तिः_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, दक्षिणः_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	जइ (c2 == ' '
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
	    जइ (c1 != ' ')			// don't clear first char
		++col;

	    off = पङ्क्तिः_Offset[row] + col;
	    end_off = पङ्क्तिः_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    जइ (enc_utf8)
		जाव (off < end_off && Screen_पङ्क्तयः[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_पङ्क्तयः_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (पूर्णाङ्क)(off - पङ्क्तिः_Offset[row]))
#endif
			  )
		    ++off;
	    अण्णहा
		जाव (off < end_off && Screen_पङ्क्तयः[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (पूर्णाङ्क)(off - पङ्क्तिः_Offset[row]))
#endif
			  )
		    ++off;
	    जइ (off < end_off)		// something to be cleared
	    {
		col = off - पङ्क्तिः_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		जाव (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (पूर्णाङ्क)(off - पङ्क्तिः_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = पङ्क्तिः_Offset[row] + start_col;
	c = c1;
	प्रति (col = start_col; col < end_col; ++col)
	{
	    जइ ((Screen_पङ्क्तयः[off] != c
		    || (enc_utf8 && (पूर्णाङ्क)Screen_पङ्क्तयः_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#जइ defined(FEAT_GUI) || defined(UNIX)
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
		जइ (screen_opacity_popup != रिक्त && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    जइ (Screen_पङ्क्तयः[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_पङ्क्तयः_UC[off - 1] != 0
			    && utf_char2cells(Screen_पङ्क्तयः_UC[off - 1]) == 2)
			गच्छ next_col;
		}
		जइ (screen_opacity_popup != रिक्त && c == ' '
						       && Screen_पङ्क्तयः[off] != 0)
		{
		    पूर्णाङ्क bg_char_cells = 1;
		    जइ (enc_utf8 && Screen_पङ्क्तयः_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_पङ्क्तयः_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    जइ (bg_char_cells == 2)
		    {
			जइ (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    गच्छ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			जइ (c2 != ' ')
			    गच्छ skip_opacity_fill;
		    }

		    पूर्णाङ्क popup_attr = get_win_attr(screen_opacity_popup);
		    पूर्णाङ्क blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    गच्छ next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		जइ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != रिक्त
			&& row >= pum_bg_शिखरम् && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    पूर्णाङ्क soff = (row - pum_bg_शिखरम्) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    जइ (enc_utf8 && pum_bg_पङ्क्तयः_UC != रिक्त
			    && col > start_col
			    && pum_bg_पङ्क्तयः_UC[soff] == 0
			    && pum_bg_पङ्क्तयः_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_पङ्क्तयः_UC[soff - 1]) == 2)
		    {
			Screen_पङ्क्तयः[off] = 0;
			जइ (Screen_पङ्क्तयः_UC != रिक्त)
			    Screen_पङ्क्तयः_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			गच्छ next_col;
		    }

		    पूर्णाङ्क underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    जइ (enc_utf8 && pum_bg_पङ्क्तयः_UC != रिक्त
			    && ((pum_bg_पङ्क्तयः_UC[soff] != 0
				    && utf_char2cells(pum_bg_पङ्क्तयः_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_पङ्क्तयः_UC[soff] == 0
				    && col > 0
				    && pum_bg_पङ्क्तयः_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_पङ्क्तयः_UC[soff - 1])
								       == 2)))
		    {
			Screen_पङ्क्तयः[off] = ' ';
			जइ (Screen_पङ्क्तयः_UC != रिक्त)
			    Screen_पङ्क्तयः_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			गच्छ next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_पङ्क्तयः[off] = pum_bg_पङ्क्तयः[soff];
		    जइ (enc_utf8 && pum_bg_पङ्क्तयः_UC != रिक्त
						&& Screen_पङ्क्तयः_UC != रिक्त)
		    {
			पूर्णाङ्क k;
			Screen_पङ्क्तयः_UC[off] = pum_bg_पङ्क्तयः_UC[soff];
			प्रति (k = 0; k < MAX_MCO; ++k)
			    जइ (pum_bg_पङ्क्तयः_C[k] != रिक्त
						    && Screen_पङ्क्तयः_C[k] != रिक्त)
				Screen_पङ्क्तयः_C[k][off] = pum_bg_पङ्क्तयः_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    गच्छ next_col;
		}
#जइ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		जइ (
# ifdef FEAT_GUI
			gui.in_use
# endif
# जइ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    जइ (Screen_पङ्क्तयः[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    अण्णहा
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		जइ (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_पङ्क्तयः[off] = c;
		जइ (enc_utf8)
		{
		    जइ (c >= 0x80)
		    {
			Screen_पङ्क्तयः_UC[off] = c;
			Screen_पङ्क्तयः_C[0][off] = 0;
			Screen_पङ्क्तयः[off] = 0x80; // avoid storing zero
		    }
		    अण्णहा
			Screen_पङ्क्तयः_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		जइ (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    जइ (col == start_col)
	    {
		जइ (did_delete)
		    भङ्ग;
		c = c2;
	    }
	}
	जइ (end_col == Columns)
	    पङ्क्तिः_Wraps[row] = FALSE;
	जइ (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    जइ (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    जइ (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    शून्य
check_for_delay(पूर्णाङ्क check_सन्देशः_scroll)
{
    जइ ((emsg_on_display || (check_सन्देशः_scroll && सन्देशः_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	जइ (check_सन्देशः_scroll)
	    सन्देशः_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    स्थिर शून्य
clear_TabPageIdxs(शून्य)
{
    पूर्णाङ्क		scol;

    प्रति (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    पूर्णाङ्क
screen_valid(पूर्णाङ्क doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    प्रत्यागम (Screen_पङ्क्तयः != रिक्त);
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
    शून्य
screenalloc(पूर्णाङ्क doclear)
{
    पूर्णाङ्क		    new_row, old_row;
#ifdef FEAT_GUI
    पूर्णाङ्क		    old_Rows;
#endif
    win_T	    *wp;
    पूर्णाङ्क		    outofmem = FALSE;
    पूर्णाङ्क		    len;
    schar_T	    *new_Screen_पङ्क्तयः;
    u8char_T	    *new_Screen_पङ्क्तयः_UC = रिक्त;
    u8char_T	    *new_Screen_पङ्क्तयः_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = रिक्त;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_पङ्क्तिः_Offset;
    char_u	    *new_पङ्क्तिः_Wraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    वर्ण	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    स्थिर पूर्णाङ्क	    entered = FALSE;		// avoid recursiveness
    स्थिर पूर्णाङ्क	    done_outofmem_सन्देशः = FALSE;	// did outofmem message
    पूर्णाङ्क		    retry_गणनम् = 0;
    पूर्णाङ्क		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    जइ ((Screen_पङ्क्तयः != रिक्त
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_पङ्क्तयः_UC != रिक्त)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != रिक्त)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_पङ्क्तयः == रिक्त))
	प्रत्यागम;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    जइ (entered)
	प्रत्यागम;
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
    प्रति (पूर्णाङ्क i = 0; i < AUCMD_WIN_गणनम्; ++i)
	जइ (aucmd_win[i].auc_win != रिक्त)
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

    new_Screen_पङ्क्तयः = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_पङ्क्तयः_C, 0, परिमाण(u8char_T *) * MAX_MCO);
    जइ (enc_utf8)
    {
	new_Screen_पङ्क्तयः_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
	    new_Screen_पङ्क्तयः_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    जइ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_पङ्क्तिः_Offset = LALLOC_MULT(unsigned, Rows);
    new_पङ्क्तिः_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(वर्ण, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जइ (win_alloc_पङ्क्तयः(wp) == FAIL)
	{
	    outofmem = TRUE;
	    गच्छ give_up;
	}
    }
    प्रति (पूर्णाङ्क i = 0; i < AUCMD_WIN_गणनम्; ++i)
	जइ (aucmd_win[i].auc_win != रिक्त
		&& aucmd_win[i].auc_win->w_पङ्क्तयः == रिक्त
		&& win_alloc_पङ्क्तयः(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    भङ्ग;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	जइ (win_alloc_पङ्क्तयः(wp) == FAIL)
	{
	    outofmem = TRUE;
	    गच्छ give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    जइ (win_alloc_पङ्क्तयः(wp) == FAIL)
	    {
		outofmem = TRUE;
		गच्छ give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
	जइ (new_Screen_पङ्क्तयः_C[i] == रिक्त)
	{
	    found_null = TRUE;
	    भङ्ग;
	}
    जइ (new_Screen_पङ्क्तयः == रिक्त
	    || (enc_utf8 && (new_Screen_पङ्क्तयः_UC == रिक्त || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == रिक्त)
	    || new_ScreenAttrs == रिक्त
	    || new_ScreenCols == रिक्त
	    || new_पङ्क्तिः_Offset == रिक्त
	    || new_पङ्क्तिः_Wraps == रिक्त
	    || new_TabPageIdxs == रिक्त
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == रिक्त
	    || new_popup_mask_next == रिक्त
	    || new_popup_transparent == रिक्त
#endif
	    || outofmem)
    {
	जइ (Screen_पङ्क्तयः != रिक्त || !done_outofmem_सन्देशः)
	{
	    // guess the size
	    do_outofmem_सन्देशः((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_सन्देशः = TRUE;
	}
	VIM_CLEAR(new_Screen_पङ्क्तयः);
	VIM_CLEAR(new_Screen_पङ्क्तयः_UC);
	प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_पङ्क्तयः_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_पङ्क्तिः_Offset);
	VIM_CLEAR(new_पङ्क्तिः_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    अण्णहा
    {
	done_outofmem_सन्देशः = FALSE;

	प्रति (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_पङ्क्तिः_Offset[new_row] = new_row * Columns;
	    new_पङ्क्तिः_Wraps[new_row] = FALSE;

	    (शून्य)vim_memset(new_Screen_पङ्क्तयः + new_row * Columns,
				  ' ', (size_t)Columns * परिमाण(schar_T));
	    जइ (enc_utf8)
	    {
		(शून्य)vim_memset(new_Screen_पङ्क्तयः_UC + new_row * Columns,
				   0, (size_t)Columns * परिमाण(u8char_T));
		प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
		    (शून्य)vim_memset(new_Screen_पङ्क्तयः_C[i]
						      + new_row * Columns,
				   0, (size_t)Columns * परिमाण(u8char_T));
	    }
	    जइ (enc_dbcs == DBCS_JPNU)
		(शून्य)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * परिमाण(schar_T));
	    (शून्य)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * परिमाण(sattr_T));
	    (शून्य)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * परिमाण(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    जइ (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		जइ (old_row >= 0 && Screen_पङ्क्तयः != रिक्त)
		{
		    जइ (screen_Columns < Columns)
			len = screen_Columns;
		    अण्णहा
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    जइ (!(enc_utf8 && Screen_पङ्क्तयः_UC == रिक्त)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_पङ्क्तयः + new_पङ्क्तिः_Offset[new_row],
				Screen_पङ्क्तयः + पङ्क्तिः_Offset[old_row],
				(size_t)len * परिमाण(schar_T));
		    जइ (enc_utf8 && Screen_पङ्क्तयः_UC != रिक्त
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_पङ्क्तयः_UC + new_पङ्क्तिः_Offset[new_row],
				Screen_पङ्क्तयः_UC + पङ्क्तिः_Offset[old_row],
				(size_t)len * परिमाण(u8char_T));
			प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_पङ्क्तयः_C[i]
						    + new_पङ्क्तिः_Offset[new_row],
				Screen_पङ्क्तयः_C[i] + पङ्क्तिः_Offset[old_row],
				(size_t)len * परिमाण(u8char_T));
		    }
		    जइ (enc_dbcs == DBCS_JPNU && ScreenLines2 != रिक्त)
			mch_memmove(new_ScreenLines2 + new_पङ्क्तिः_Offset[new_row],
				ScreenLines2 + पङ्क्तिः_Offset[old_row],
				(size_t)len * परिमाण(schar_T));
		    mch_memmove(new_ScreenAttrs + new_पङ्क्तिः_Offset[new_row],
			    ScreenAttrs + पङ्क्तिः_Offset[old_row],
			    (size_t)len * परिमाण(sattr_T));
		    mch_memmove(new_ScreenCols + new_पङ्क्तिः_Offset[new_row],
			    ScreenCols + पङ्क्तिः_Offset[old_row],
			    (size_t)len * परिमाण(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_पङ्क्तिः = new_Screen_पङ्क्तयः + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * परिमाण(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * परिमाण(वर्ण));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_पङ्क्तयः = new_Screen_पङ्क्तयः;
    Screen_पङ्क्तयः_UC = new_Screen_पङ्क्तयः_UC;
    प्रति (पूर्णाङ्क i = 0; i < p_mco; ++i)
	Screen_पङ्क्तयः_C[i] = new_Screen_पङ्क्तयः_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    पङ्क्तिः_Offset = new_पङ्क्तिः_Offset;
    पङ्क्तिः_Wraps = new_पङ्क्तिः_Wraps;
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
    जइ (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    जइ (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    अण्णहा जइ (gui.in_use
	    && !gui.starting
	    && Screen_पङ्क्तयः != रिक्त
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	जइ (सन्देशः_row >= Rows)		// Rows got smaller
	    सन्देशः_row = Rows - 1;		// put cursor at last row
	अण्णहा जइ (Rows > old_Rows)	// Rows got bigger
	    सन्देशः_row += Rows - old_Rows; // put cursor in same place
	जइ (सन्देशः_col >= cmdline_width)	// cmdline_width got smaller
	    सन्देशः_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    जइ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    जइ (starting == 0 && ++retry_गणनम् <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, रिक्त, रिक्त, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	गच्छ retry;
    }
}

    शून्य
free_screenlines(शून्य)
{
    पूर्णाङ्क		i;

    VIM_CLEAR(Screen_पङ्क्तयः_UC);
    प्रति (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_पङ्क्तयः_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_पङ्क्तयः);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(पङ्क्तिः_Offset);
    VIM_CLEAR(पङ्क्तिः_Wraps);
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
    पूर्णाङ्क
screenclear(शून्य)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    प्रत्यागम screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    शून्य
redraw_as_cleared(शून्य)
{
    screenclear2(FALSE);
}

    स्थिर पूर्णाङ्क
screenclear2(पूर्णाङ्क doclear)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    did_clear = FALSE;

    जइ (starting == NO_SCREEN || Screen_पङ्क्तयः == रिक्त
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	प्रत्यागम FALSE;

#ifdef FEAT_GUI
    जइ (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    प्रति (i = 0; i < Rows; ++i)
    {
	lineclear(पङ्क्तिः_Offset[i], (पूर्णाङ्क)Columns, 0);
	पङ्क्तिः_Wraps[i] = FALSE;
    }

    जइ (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	जइ (suppressed_cells != रिक्त)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    अण्णहा
    {
	// can't clear the screen, mark all chars with invalid attributes
	प्रति (i = 0; i < Rows; ++i)
	    lineinvalid(पङ्क्तिः_Offset[i], (पूर्णाङ्क)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#जइ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    जइ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    सन्देशः_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    सन्देशः_row = cmdline_row;	// put cursor on last line for messages
    सन्देशः_col = 0;
    screen_start();		// don't know where cursor is now
    सन्देशः_didany = FALSE;
    सन्देशः_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    प्रत्यागम did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    स्थिर शून्य
lineclear(unsigned off, पूर्णाङ्क width, पूर्णाङ्क attr)
{
    (शून्य)vim_memset(Screen_पङ्क्तयः + off, ' ', (size_t)width * परिमाण(schar_T));
    जइ (enc_utf8)
	(शून्य)vim_memset(Screen_पङ्क्तयः_UC + off, 0,
					  (size_t)width * परिमाण(u8char_T));
    (शून्य)vim_memset(ScreenAttrs + off, attr, (size_t)width * परिमाण(sattr_T));
    (शून्य)vim_memset(ScreenCols + off, -1, (size_t)width * परिमाण(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    स्थिर शून्य
lineinvalid(unsigned off, पूर्णाङ्क width)
{
    (शून्य)vim_memset(ScreenAttrs + off, -1, (size_t)width * परिमाण(sattr_T));
    (शून्य)vim_memset(ScreenCols + off, -1, (size_t)width * परिमाण(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    शून्य
पङ्क्तिः_was_clobbered(पूर्णाङ्क screen_lnum)
{
    lineinvalid(पङ्क्तिः_Offset[screen_lnum], (पूर्णाङ्क)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    स्थिर शून्य
linecopy(पूर्णाङ्क to, पूर्णाङ्क from, win_T *wp)
{
    unsigned	off_to = पङ्क्तिः_Offset[to] + wp->w_wincol;
    unsigned	off_from = पङ्क्तिः_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_पङ्क्तयः + off_to, Screen_पङ्क्तयः + off_from,
	    wp->w_width * परिमाण(schar_T));
    जइ (enc_utf8)
    {
	पूर्णाङ्क	i;

	mch_memmove(Screen_पङ्क्तयः_UC + off_to, Screen_पङ्क्तयः_UC + off_from,
		wp->w_width * परिमाण(u8char_T));
	प्रति (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_पङ्क्तयः_C[i] + off_to, Screen_पङ्क्तयः_C[i] + off_from,
		    wp->w_width * परिमाण(u8char_T));
    }
    जइ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * परिमाण(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * परिमाण(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * परिमाण(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    पूर्णाङ्क
can_clear(char_u *p)
{
    प्रत्यागम (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#अण्णहा
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
    शून्य
screen_start(शून्य)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    शून्य
windgoto(पूर्णाङ्क row, पूर्णाङ्क col)
{
    sattr_T	    *p;
    पूर्णाङ्क		    i;
    पूर्णाङ्क		    plan;
    पूर्णाङ्क		    cost;
    पूर्णाङ्क		    wouldbe_col;
    पूर्णाङ्क		    noinvcurs;
    char_u	    *bs;
    पूर्णाङ्क		    goto_cost;
    पूर्णाङ्क		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    जइ (Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम;
    जइ (col == screen_cur_col && row == screen_cur_row)
	प्रत्यागम;

    // Check for valid position.
    जइ (row < 0)	// window without text lines?
	row = 0;
    जइ (row >= screen_Rows)
	row = screen_Rows - 1;
    जइ (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    जइ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    अण्णहा
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
    जइ (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = रिक्त;			    // init for GCC
	attr = screen_attr;
	जइ (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    जइ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    अण्णहा
		bs = T_BC;		    // "backspace character (old)
	    जइ (*bs)
		cost = (screen_cur_col - col) * (पूर्णाङ्क)STRLEN(bs);
	    अण्णहा
		cost = 999;
	    जइ (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    अण्णहा
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    जइ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	अण्णहा जइ (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    जइ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	अण्णहा
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
	जइ (i > 0)
	    cost += i;
	जइ (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + पङ्क्तिः_Offset[row] + wouldbe_col;
	    जाव (i && *p++ == attr)
		--i;
	    जइ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		जइ (*--p == 0)
		{
		    cost += noinvcurs;
		    जाव (i && *p++ == 0)
			--i;
		}
		जइ (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    जइ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		प्रति (i = wouldbe_col; i < col; ++i)
		    जइ (Screen_पङ्क्तयः_UC[पङ्क्तिः_Offset[row] + i] != 0)
		    {
			cost = 999;
			भङ्ग;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    जइ (cost < 999)
		प्रति (i = wouldbe_col; i < col; ++i)
		    जइ (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			भङ्ग;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	जइ (cost < goto_cost)
	{
	    जइ (plan == PLAN_LE)
	    {
		जइ (noinvcurs)
		    screen_stop_highlight();
		जाव (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    अण्णहा जइ (plan == PLAN_CR)
	    {
		जइ (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    अण्णहा जइ (plan == PLAN_NL)
	    {
		जइ (noinvcurs)
		    screen_stop_highlight();
		जाव (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    जइ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		जइ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    जाव (i-- > 0)
			out_char(*T_ND);
		}
		अण्णहा
		{
		    पूर्णाङ्क	off;

		    off = पङ्क्तिः_Offset[row] + screen_cur_col;
		    जाव (i-- > 0)
		    {
			जइ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_पङ्क्तयः[off]);
			जइ (enc_dbcs == DBCS_JPNU
				&& Screen_पङ्क्तयः[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    अण्णहा
	cost = 999;

    जइ (cost >= goto_cost)
    {
	जइ (noinvcurs)
	    screen_stop_highlight();
	जइ (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_दक्षिणः(col - screen_cur_col);
	अण्णहा
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    शून्य
setcursor(शून्य)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    पूर्णाङ्क
cursor_screen_cells(शून्य)
{
    प्रत्यागम has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    शून्य
setcursor_mayforce(पूर्णाङ्क force)
{
    जइ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((पूर्णाङ्क)curwin->w_width - curwin->w_wcol
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
    पूर्णाङ्क
win_ins_पङ्क्तयः(
    win_T	*wp,
    पूर्णाङ्क		row,
    पूर्णाङ्क		पङ्क्तिः_गणनम्,
    पूर्णाङ्क		invalid,
    पूर्णाङ्क		mayclear)
{
    पूर्णाङ्क		did_delete;
    पूर्णाङ्क		nextrow;
    पूर्णाङ्क		lastrow;
    पूर्णाङ्क		retval;

    जइ (invalid)
	wp->w_पङ्क्तयः_valid = 0;

    // with only a few lines it's not worth the effort
    जइ (wp->w_height < 5)
	प्रत्यागम FAIL;

    // with the popup menu visible this might not work correctly
    जइ (pum_visible())
	प्रत्यागम FAIL;

    जइ (पङ्क्तिः_गणनम् > wp->w_height - row)
	पङ्क्तिः_गणनम् = wp->w_height - row;

    retval = win_do_पङ्क्तयः(wp, row, पङ्क्तिः_गणनम्, mayclear, FALSE, 0);
    जइ (retval != MAYBE)
	प्रत्यागम retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    जइ (wp->w_next != रिक्त || wp->w_status_height)
    {
	जइ (screen_del_पङ्क्तयः(0, W_WINROW(wp) + wp->w_height - पङ्क्तिः_गणनम्,
				  पङ्क्तिः_गणनम्, (पूर्णाङ्क)Rows, FALSE, 0, रिक्त) == OK)
	    did_delete = TRUE;
	अण्णहा जइ (wp->w_next)
	    प्रत्यागम FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    जइ (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + पङ्क्तिः_गणनम्;
	जइ (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - पङ्क्तिः_गणनम्, lastrow - पङ्क्तिः_गणनम्,
		  wp->w_wincol, (पूर्णाङ्क)W_ENDCOL(wp), ' ', ' ', 0);
    }

    जइ (screen_ins_पङ्क्तयः(0, W_WINROW(wp) + row, पङ्क्तिः_गणनम्, (पूर्णाङ्क)Rows, 0, रिक्त)
								      == FAIL)
    {
	// deletion will have messed up other windows
	जइ (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    पूर्णाङ्क
win_del_पङ्क्तयः(
    win_T	*wp,
    पूर्णाङ्क		row,
    पूर्णाङ्क		पङ्क्तिः_गणनम्,
    पूर्णाङ्क		invalid,
    पूर्णाङ्क		mayclear,
    पूर्णाङ्क		clear_attr)	    // for clearing lines
{
    पूर्णाङ्क		retval;

    जइ (invalid)
	wp->w_पङ्क्तयः_valid = 0;

    जइ (पङ्क्तिः_गणनम् > wp->w_height - row)
	पङ्क्तिः_गणनम् = wp->w_height - row;

    retval = win_do_पङ्क्तयः(wp, row, पङ्क्तिः_गणनम्, mayclear, TRUE, clear_attr);
    जइ (retval != MAYBE)
	प्रत्यागम retval;

    जइ (screen_del_पङ्क्तयः(0, W_WINROW(wp) + row, पङ्क्तिः_गणनम्,
				   (पूर्णाङ्क)Rows, FALSE, clear_attr, रिक्त) == FAIL)
	प्रत्यागम FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    जइ (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	जइ (screen_ins_पङ्क्तयः(0, W_WINROW(wp) + wp->w_height - पङ्क्तिः_गणनम्,
			      पङ्क्तिः_गणनम्, (पूर्णाङ्क)Rows, clear_attr, रिक्त) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    अण्णहा
	redraw_cmdline = TRUE;
    प्रत्यागम OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    स्थिर पूर्णाङ्क
win_do_पङ्क्तयः(
    win_T	*wp,
    पूर्णाङ्क		row,
    पूर्णाङ्क		पङ्क्तिः_गणनम्,
    पूर्णाङ्क		mayclear,
    पूर्णाङ्क		del,
    पूर्णाङ्क		clear_attr)
{
    पूर्णाङ्क		retval;

    जइ (!redrawing() || पङ्क्तिः_गणनम् <= 0)
	प्रत्यागम FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    जइ (no_win_do_पङ्क्तयः_ins && !del)
	प्रत्यागम FAIL;

    // only a few lines left: redraw is faster
    जइ (mayclear && Rows - पङ्क्तिः_गणनम् < 5
	    && wp->w_width == topframe->fr_width)
    {
	जइ (!no_win_do_पङ्क्तयः_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	प्रत्यागम FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    जइ (popup_visible)
	प्रत्यागम FAIL;
#endif

    // Delete all remaining lines
    जइ (row + पङ्क्तिः_गणनम् >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (पूर्णाङ्क)W_ENDCOL(wp), ' ', ' ', 0);
	प्रत्यागम OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    जइ (!no_win_do_पङ्क्तयः_ins)
	clear_cmdline = TRUE;

#जइ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    जइ (tabpanel_width() > 0)
	प्रत्यागम FAIL;
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
    जइ (scroll_region || wp->w_width != topframe->fr_width)
    {
	जइ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	जइ (del)
	    retval = screen_del_पङ्क्तयः(W_WINROW(wp) + row, 0, पङ्क्तिः_गणनम्,
				    wp->w_height - row, FALSE, clear_attr, wp);
	अण्णहा
	    retval = screen_ins_पङ्क्तयः(W_WINROW(wp) + row, 0, पङ्क्तिः_गणनम्,
					   wp->w_height - row, clear_attr, wp);
	जइ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	प्रत्यागम retval;
    }

    जइ (wp->w_next != रिक्त && p_tf) // don't delete/insert on fast terminal
	प्रत्यागम FAIL;

    प्रत्यागम MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    स्थिर शून्य
win_rest_invalid(win_T *wp)
{
    जाव (wp != रिक्त)
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
    पूर्णाङ्क
screen_ins_पङ्क्तयः(
    पूर्णाङ्क		off,
    पूर्णाङ्क		row,
    पूर्णाङ्क		पङ्क्तिः_गणनम्,
    पूर्णाङ्क		end,
    पूर्णाङ्क		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;
    unsigned	temp;
    पूर्णाङ्क		cursor_row;
    पूर्णाङ्क		cursor_col = 0;
    पूर्णाङ्क		type;
    पूर्णाङ्क		result_empty;
    पूर्णाङ्क		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    जइ (!screen_valid(TRUE)
	     || पङ्क्तिः_गणनम् <= 0 || पङ्क्तिः_गणनम् > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	प्रत्यागम FAIL;

#ifdef FEAT_PROP_POPUP
    जइ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, पङ्क्तिः_गणनम्);
# endif
	प्रत्यागम FAIL;
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
    result_empty = (row + पङ्क्तिः_गणनम् >= end);
    जइ (wp != रिक्त && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	जइ (पङ्क्तिः_गणनम् > 3)
	    प्रत्यागम FAIL;
	type = USE_REDRAW;
    }
    अण्णहा जइ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    अण्णहा जइ (*T_CAL != NUL && (पङ्क्तिः_गणनम् > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    अण्णहा जइ (*T_CDL != NUL && result_empty && (पङ्क्तिः_गणनम् > 1 || !can_ce))
	type = USE_T_CDL;
    अण्णहा जइ (*T_AL != NUL)
	type = USE_T_AL;
    अण्णहा जइ (can_ce && result_empty)
	type = USE_T_CE;
    अण्णहा जइ (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    अण्णहा जइ (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    अण्णहा
	प्रत्यागम FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    जइ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	प्रत्यागम screen_del_पङ्क्तयः(off, row, पङ्क्तिः_गणनम्, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    जइ (*T_DB)
	screen_del_पङ्क्तयः(off, end - पङ्क्तिः_गणनम्, पङ्क्तिः_गणनम्, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    जइ (off + row > 0 || (wp != रिक्त && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    अण्णहा
	clip_scroll_selection(-पङ्क्तिः_गणनम्);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, पङ्क्तिः_गणनम्);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    जइ (wp != रिक्त && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    जइ (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    अण्णहा
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    प्रति (i = 0; i < पङ्क्तिः_गणनम्; ++i)
    {
	जइ (wp != रिक्त && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    जाव ((j -= पङ्क्तिः_गणनम्) >= row)
		linecopy(j + पङ्क्तिः_गणनम्, j, wp);
	    j += पङ्क्तिः_गणनम्;
	    जइ (can_clear((char_u *)" "))
		lineclear(पङ्क्तिः_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    अण्णहा
		lineinvalid(पङ्क्तिः_Offset[j] + wp->w_wincol, wp->w_width);
	    पङ्क्तिः_Wraps[j] = FALSE;
	}
	अण्णहा
	{
	    j = end - 1 - i;
	    temp = पङ्क्तिः_Offset[j];
	    जाव ((j -= पङ्क्तिः_गणनम्) >= row)
	    {
		पङ्क्तिः_Offset[j + पङ्क्तिः_गणनम्] = पङ्क्तिः_Offset[j];
		पङ्क्तिः_Wraps[j + पङ्क्तिः_गणनम्] = पङ्क्तिः_Wraps[j];
	    }
	    पङ्क्तिः_Offset[j + पङ्क्तिः_गणनम्] = temp;
	    पङ्क्तिः_Wraps[j + पङ्क्तिः_गणनम्] = FALSE;
	    जइ (can_clear((char_u *)" "))
		lineclear(temp, (पूर्णाङ्क)Columns, clear_attr);
	    अण्णहा
		lineinvalid(temp, (पूर्णाङ्क)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    जइ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    जइ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    अण्णहा जइ (type == USE_T_CAL)
    {
	term_append_पङ्क्तयः(पङ्क्तिः_गणनम्);
	screen_start();		// don't know where cursor is now
    }
    अण्णहा
    {
	प्रति (i = 0; i < पङ्क्तिः_गणनम्; i++)
	{
	    जइ (type == USE_T_AL)
	    {
		जइ (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    अण्णहा  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    जइ (type == USE_T_SR && *T_DA)
    {
	प्रति (i = 0; i < पङ्क्तिः_गणनम्; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#जइ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    जइ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    प्रत्यागम OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    पूर्णाङ्क
screen_del_पङ्क्तयः(
    पूर्णाङ्क		off,
    पूर्णाङ्क		row,
    पूर्णाङ्क		पङ्क्तिः_गणनम्,
    पूर्णाङ्क		end,
    पूर्णाङ्क		force,		// even when line_count > p_ttyscroll
    पूर्णाङ्क		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    पूर्णाङ्क		j;
    पूर्णाङ्क		i;
    unsigned	temp;
    पूर्णाङ्क		cursor_row;
    पूर्णाङ्क		cursor_col = 0;
    पूर्णाङ्क		cursor_end;
    पूर्णाङ्क		result_empty;	// result is empty until end of region
    पूर्णाङ्क		can_delete;	// deleting line codes can be used
    पूर्णाङ्क		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    जइ (!screen_valid(TRUE)
	    || पङ्क्तिः_गणनम् <= 0
	    || (!force && पङ्क्तिः_गणनम् > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	प्रत्यागम FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + पङ्क्तिः_गणनम् >= end;

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
    जइ (wp != रिक्त && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	जइ (पङ्क्तिः_गणनम् > 3)
	    प्रत्यागम FAIL;
	type = USE_REDRAW;
    }
    अण्णहा जइ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    अण्णहा जइ (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    पङ्क्तिः_गणनम् == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    अण्णहा जइ (*T_CDL != NUL && पङ्क्तिः_गणनम् > 1 && can_delete)
	type = USE_T_CDL;
    अण्णहा जइ (can_clear(T_CE) && result_empty
	    && (wp == रिक्त || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    अण्णहा जइ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    अण्णहा जइ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    अण्णहा
	प्रत्यागम FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    जइ (off + row > 0 || (wp != रिक्त && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    अण्णहा
	clip_scroll_selection(पङ्क्तिः_गणनम्);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -पङ्क्तिः_गणनम्);
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

    जइ (wp != रिक्त && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    जइ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    अण्णहा
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
    प्रति (i = 0; i < पङ्क्तिः_गणनम्; ++i)
    {
	जइ (wp != रिक्त && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    जाव ((j += पङ्क्तिः_गणनम्) <= end - 1)
		linecopy(j - पङ्क्तिः_गणनम्, j, wp);
	    j -= पङ्क्तिः_गणनम्;
	    जइ (can_clear((char_u *)" "))
		lineclear(पङ्क्तिः_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    अण्णहा
		lineinvalid(पङ्क्तिः_Offset[j] + wp->w_wincol, wp->w_width);
	    पङ्क्तिः_Wraps[j] = FALSE;
	}
	अण्णहा
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = पङ्क्तिः_Offset[j];
	    जाव ((j += पङ्क्तिः_गणनम्) <= end - 1)
	    {
		पङ्क्तिः_Offset[j - पङ्क्तिः_गणनम्] = पङ्क्तिः_Offset[j];
		पङ्क्तिः_Wraps[j - पङ्क्तिः_गणनम्] = पङ्क्तिः_Wraps[j];
	    }
	    पङ्क्तिः_Offset[j - पङ्क्तिः_गणनम्] = temp;
	    पङ्क्तिः_Wraps[j - पङ्क्तिः_गणनम्] = FALSE;
	    जइ (can_clear((char_u *)" "))
		lineclear(temp, (पूर्णाङ्क)Columns, clear_attr);
	    अण्णहा
		lineinvalid(temp, (पूर्णाङ्क)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    जइ (screen_attr != clear_attr)
	screen_stop_highlight();
    जइ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    जइ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    अण्णहा जइ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    अण्णहा जइ (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_पङ्क्तयः(पङ्क्तिः_गणनम्);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    अण्णहा जइ (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	प्रति (i = पङ्क्तिः_गणनम्; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    अण्णहा
    {
	प्रति (i = पङ्क्तिः_गणनम्; --i >= 0; )
	{
	    जइ (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    अण्णहा // type == USE_T_CE
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
    जइ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	प्रति (i = पङ्क्तिः_गणनम्; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#जइ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    जइ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    प्रत्यागम OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    पूर्णाङ्क
skip_showmode(शून्य)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    जइ (global_busy
	    || सन्देशः_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !कुञ्चिका_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    पूर्णाङ्क
showmode(शून्य)
{
    पूर्णाङ्क		need_clear;
    पूर्णाङ्क		length = 0;
    पूर्णाङ्क		do_mode;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		nwr_save;
    पूर्णाङ्क		sub_attr;
    पूर्णाङ्क		show_ruler_with_pum = FALSE;

    do_mode = p_smd && सन्देशः_silent == 0
	    && ((State & MODE_निवेशय)
		|| restart_edit != NUL
		|| VIsual_active);
    जइ (do_mode || reg_recording != 0)
    {
	जइ (skip_showmode())
	    प्रत्यागम 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	जइ (clear_cmdline && cmdline_row < Rows - 1)
	    सन्देशः_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	सन्देशः_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	जइ (do_mode)
	{
	    सन्देशः_puts_attr("--", attr);
#जइ defined(FEAT_XIM)
	    जइ (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# अण्णहा
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		सन्देशः_puts_attr(" IM", attr);
# अण्णहा
		सन्देशः_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    जइ (edit_submode != रिक्त && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - सन्देशः_row) * cmdline_width - 3;
		जइ (edit_submode_extra != रिक्त)
		    length -= vim_strsize(edit_submode_extra);
		जइ (length > 0)
		{
		    जइ (edit_submode_pre != रिक्त)
			length -= vim_strsize(edit_submode_pre);
		    जइ (length - vim_strsize(edit_submode) > 0)
		    {
			जइ (edit_submode_pre != रिक्त)
			    सन्देशः_puts_attr((वर्ण *)edit_submode_pre, attr);
			सन्देशः_puts_attr((वर्ण *)edit_submode, attr);
		    }
		    जइ (edit_submode_extra != रिक्त)
		    {
			सन्देशः_puts_attr(" ", attr);  // add a space in between
			जइ ((पूर्णाङ्क)edit_submode_highl < (पूर्णाङ्क)HLF_गणनम्)
			    sub_attr = HL_ATTR(edit_submode_highl);
			अण्णहा
			    sub_attr = attr;
			सन्देशः_puts_attr((वर्ण *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    अण्णहा
	    {
		जइ (State & VREPLACE_FLAG)
		    सन्देशः_puts_attr(_(" VREPLACE"), attr);
		अण्णहा जइ (State & REPLACE_FLAG)
		    सन्देशः_puts_attr(_(" REPLACE"), attr);
		अण्णहा जइ (State & MODE_निवेशय)
		{
#ifdef FEAT_RIGHTLEFT
		    जइ (p_ri)
			सन्देशः_puts_attr(_(" REVERSE"), attr);
#endif
		    सन्देशः_puts_attr(_(" INSERT"), attr);
		}
		अण्णहा जइ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    सन्देशः_puts_attr(_(" (insert)"), attr);
		अण्णहा जइ (restart_edit == 'R')
		    सन्देशः_puts_attr(_(" (replace)"), attr);
		अण्णहा जइ (restart_edit == 'V')
		    सन्देशः_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		जइ (p_hkmap)
		    सन्देशः_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		जइ (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    जइ (curwin->w_p_arab)
			सन्देशः_puts_attr(_(" Arabic"), attr);
		    अण्णहा
# endif
			जइ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    सन्देशः_puts_attr((वर्ण *)NameBuff, attr);
		}
#endif
		जइ ((State & MODE_निवेशय) && p_paste)
		    सन्देशः_puts_attr(_(" (paste)"), attr);

		जइ (VIsual_active)
		{
		    वर्ण *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    विकल्प ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			पक्ष 0:	p = N_(" VISUAL"); भङ्ग;
			पक्ष 1: p = N_(" VISUAL LINE"); भङ्ग;
			पक्ष 2: p = N_(" VISUAL BLOCK"); भङ्ग;
			पक्ष 4: p = N_(" SELECT"); भङ्ग;
			पक्ष 5: p = N_(" SELECT LINE"); भङ्ग;
			स्वभाव: p = N_(" SELECT BLOCK"); भङ्ग;
		    }
		    सन्देशः_puts_attr(_(p), attr);
		}
		सन्देशः_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	जइ (reg_recording != 0
		&& edit_submode == रिक्त)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	जइ (need_clear || clear_cmdline || redraw_mode)
	    सन्देशः_clr_eos();
	सन्देशः_didout = FALSE;		// overwrite this message
	length = सन्देशः_col;
	सन्देशः_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    अण्णहा जइ (clear_cmdline && सन्देशः_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	सन्देशः_clr_cmdline();
    अण्णहा जइ (redraw_mode)
    {
	सन्देशः_pos_mode();
	सन्देशः_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    जइ (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    जइ (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    प्रत्यागम length;
}

/*
 * Position for a mode message.
 */
    स्थिर शून्य
सन्देशः_pos_mode(शून्य)
{
    सन्देशः_col = 0;
    सन्देशः_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    शून्य
unshowmode(पूर्णाङ्क force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    जइ (!redrawing() || (!force && char_avail() && !कुञ्चिका_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    अण्णहा
	clearmode();
}

/*
 * Clear the mode message.
 */
    शून्य
clearmode(शून्य)
{
    पूर्णाङ्क save_सन्देशः_row = सन्देशः_row;
    पूर्णाङ्क save_सन्देशः_col = सन्देशः_col;

    सन्देशः_pos_mode();
    जइ (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    सन्देशः_clr_eos();

    सन्देशः_col = save_सन्देशः_col;
    सन्देशः_row = save_सन्देशः_row;
}

    स्थिर शून्य
recording_mode(पूर्णाङ्क attr)
{
    सन्देशः_puts_attr(_("recording"), attr);
    जइ (shortmess(SHM_RECORDING))
	प्रत्यागम;

    वर्ण s[4];

    sprintf(s, " @%c", reg_recording);
    सन्देशः_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    स्थिर पूर्णाङ्क
mouse_has_any(शून्य)
{
    प्रत्यागम mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_निवेशय)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    शून्य
draw_tabline(शून्य)
{
    पूर्णाङ्क		tabcount = 0;
    tabpage_T	*tp;
    पूर्णाङ्क		tabwidth;
    पूर्णाङ्क		col = 0;
    पूर्णाङ्क		scol = 0;
    पूर्णाङ्क		attr;
    win_T	*wp;
    win_T	*cwp;
    पूर्णाङ्क		wincount;
    पूर्णाङ्क		modified;
    पूर्णाङ्क		c;
    पूर्णाङ्क		len;
    पूर्णाङ्क		attr_sel;
    पूर्णाङ्क		attr_nosel = 0;
    पूर्णाङ्क		attr_fill = 0;
    char_u	*p;
    पूर्णाङ्क		room;
    पूर्णाङ्क		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#जइ defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    जइ (Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    जइ (gui_use_tabline())
    {
	gui_update_tabline();
	प्रत्यागम;
    }
#endif

    जइ (tabline_height() < 1)
	प्रत्यागम;

#जइ defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    जइ (*p_tal != NUL)
	win_redr_custom(रिक्त, FALSE);
    अण्णहा
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	जइ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	प्रति (tp = first_tabpage; tp != रिक्त && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = प्रक्षिप_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    जइ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    जइ (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    जइ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    जइ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    अण्णहा
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    प्रति (wincount = 0; wp != रिक्त; wp = wp->w_next, ++wincount)
		जइ (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    जइ (modified || wincount > 1)
	    {
		जइ (wincount > 1)
		{
		    len = vim_snprintf((वर्ण *)NameBuff, MAXPATHL, "%d", wincount);
		    जइ (col + len >= Columns - 3)
			भङ्ग;
		    screen_puts_len(NameBuff, len, 0, col,
#जइ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#अण्णहा
					 attr
#endif
					       );
		    col += len;
		}
		जइ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    जइ (room > 0)
	    {
		पूर्णाङ्क n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		जइ (has_mbyte)
		    जाव (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		अण्णहा जइ (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		जइ (len > n)
		    len = n;

		screen_puts_len(p, (पूर्णाङ्क)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    जाव (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    जइ (override_success)
		निष्कासय_highlight_overrides();
	}

	जइ (use_sep_chars)
	    c = '_';
	अण्णहा
	    c = ' ';
	screen_fill(0, 1, col, (पूर्णाङ्क)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	जइ (p_sc && *p_sloc == 't')
	{
	    पूर्णाङ्क	n = (पूर्णाङ्क)Columns - col - (tabcount > 1) * 3;
	    पूर्णाङ्क	width = MIN(10, n);

	    जइ (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (पूर्णाङ्क)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	जइ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (पूर्णाङ्क)Columns - 1, attr_nosel);
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
    शून्य
get_trans_bufname(buf_T *buf)
{
    जइ (buf_spname(buf) != रिक्त)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    अण्णहा
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    पूर्णाङ्क
fillchar_status(पूर्णाङ्क *attr, win_T *wp)
{
    पूर्णाङ्क	    fill;
    bool    override_success =
	प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    जइ (bt_terminal(wp->w_buffer))
    {
	जइ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	अण्णहा
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    अण्णहा
#endif
    जइ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    अण्णहा
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    जइ (override_success)
	निष्कासय_highlight_overrides();
    प्रत्यागम fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    स्थिर bool
vsep_row_is_curwin(win_T *wp, पूर्णाङ्क row)
{
    जइ (wp == curwin)
	प्रत्यागम true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    जइ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	प्रत्यागम true;

    प्रत्यागम false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    पूर्णाङ्क
fillchar_vsep(पूर्णाङ्क *attr, win_T *wp, पूर्णाङ्क row)
{
    bool override_success =
	प्रक्षिप_highlight_overrides(wp->w_hl, wp->w_hl_len);
    जइ (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    अण्णहा
	*attr = HL_ATTR(HLF_CNC);
    जइ (override_success)
	निष्कासय_highlight_overrides();

    जइ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	प्रत्यागम '|';
    अण्णहा
	प्रत्यागम wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    पूर्णाङ्क
redrawing(शून्य)
{
#ifdef FEAT_EVAL
    जइ (disable_redraw_for_testing)
	प्रत्यागम 0;
    अण्णहा
#endif
	प्रत्यागम ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !कुञ्चिका_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    पूर्णाङ्क
messaging(शून्य)
{
    प्रत्यागम (!(p_lz && char_avail() && !कुञ्चिका_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    शून्य
comp_col(शून्य)
{
    पूर्णाङ्क last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    जइ (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#अण्णहा
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	जइ (!last_has_status)
	    sc_col = ru_col;
    }
    जइ (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	जइ (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    जइ (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    जइ (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#जइ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    पूर्णाङ्क
सङ्ख्या_width(win_T *wp)
{
    पूर्णाङ्क		n;
    linenr_T	lnum;

    जइ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    अण्णहा
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;

    जइ (lnum == wp->w_nrwidth_पङ्क्तिः_गणनम् && wp->w_nuw_cached == wp->w_p_nuw)
	प्रत्यागम wp->w_nrwidth_width;
    wp->w_nrwidth_पङ्क्तिः_गणनम् = lnum;

    n = 0;
    करेहि
    {
	lnum /= 10;
	++n;
    } जाव (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    जइ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    जइ (n < 2 && get_first_valid_sign(wp) != रिक्त
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    प्रत्यागम n;
}
#endif

#जइ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    पूर्णाङ्क
screen_screencol(शून्य)
{
    प्रत्यागम screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    पूर्णाङ्क
screen_screenrow(शून्य)
{
    प्रत्यागम screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    स्थिर पूर्णाङ्क
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    जइ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T अङ्कः = 0;
	पूर्णाङ्क	    bytes;
	पूर्णाङ्क	    n;

	प्रति (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    जइ (n < 0)
		प्रत्यागम 0;
	    अङ्कः = अङ्कः * 256 + n;
	}
	*p += 2;
	प्रत्यागम अङ्कः;
    }
    प्रत्यागम mb_ptr2char_adv(p);
}

संरचना charstab
{
    पूर्णाङ्क	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

स्थिर fill_chars_T fill_chars;
स्थिर संरचना charstab filltab[] =
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
#जइ defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
स्थिर lcs_chars_T lcs_chars;
स्थिर संरचना charstab lcstab[] =
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
#अण्णहा
    CHARSTAB_ENTRY(रिक्त,		    "conceal"),
#endif
    CHARSTAB_ENTRY(रिक्त,		    "multispace"),
    CHARSTAB_ENTRY(रिक्त,		    "leadmultispace")
};

    स्थिर वर्ण *
field_मानम्_err(वर्ण *errbuf, size_t errbuflen, वर्ण *fmt, char_u *field)
{
    जइ (errbuf == रिक्त)
	प्रत्यागम "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    प्रत्यागम errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    स्थिर वर्ण *
set_chars_option(win_T *wp, char_u *मानम्, पूर्णाङ्क is_listchars, पूर्णाङ्क apply,
						वर्ण *errbuf, size_t errbuflen)
{
    पूर्णाङ्क	    round, i, entries;
    char_u  *p, *s;
    पूर्णाङ्क	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = रिक्त;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = रिक्त; // Last occurrence of "leadmultispace:"
    पूर्णाङ्क	    multispace_len = 0;	      // Length of lcs-multispace string
    पूर्णाङ्क	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    संरचना charstab *tab;

    जइ (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	जइ (wp->w_p_lcs[0] == NUL)
	    मानम् = p_lcs;  // local value is empty, use the global value
    }
    अण्णहा
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	जइ (wp->w_p_fcs[0] == NUL)
	    मानम् = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    प्रति (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	पूर्णाङ्क has_tab = FALSE, has_leadtab = FALSE;

	जइ (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    जइ (is_listchars)
	    {
		प्रति (i = 0; i < entries; ++i)
		    जइ (tab[i].cp != रिक्त)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		जइ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(पूर्णाङ्क, multispace_len + 1);
		    जइ (lcs_chars.multispace != रिक्त)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		अण्णहा
		    lcs_chars.multispace = रिक्त;

		जइ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(पूर्णाङ्क, lead_multispace_len + 1);
		    जइ (lcs_chars.leadmultispace != रिक्त)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		अण्णहा
		    lcs_chars.leadmultispace = रिक्त;
	    }
	    अण्णहा
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
#जइ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = मानम्;
	जाव (*p)
	{
	    प्रति (i = 0; i < entries; ++i)
	    {
		जइ (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    अनुवर्त;

		s = p + tab[i].name.length + 1;

		जइ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    जइ (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			जाव (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जइ (char2cells(c1) > 1)
				प्रत्यागम field_मानम्_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			जइ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
				    e_wrong_सङ्ख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    अण्णहा
		    {
			पूर्णाङ्क multispace_pos = 0;

			जाव (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जइ (p == last_multispace && lcs_chars.multispace != रिक्त)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    भङ्ग;
		}

		जइ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    जइ (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			जाव (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जइ (char2cells(c1) > 1)
				प्रत्यागम field_मानम्_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			जइ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
				    e_wrong_सङ्ख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    अण्णहा
		    {
			पूर्णाङ्क multispace_pos = 0;

			जाव (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जइ (p == last_lmultispace && lcs_chars.leadmultispace != रिक्त)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    भङ्ग;
		}

		c2 = c3 = 0;
		जइ (*s == NUL)
		    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
				    e_wrong_सङ्ख्या_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		जइ (char2cells(c1) > 1)
		    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		जइ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    जइ (*s == NUL)
			प्रत्यागम field_मानम्_err(errbuf, errbuflen,
				    e_wrong_सङ्ख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    जइ (char2cells(c2) > 1)
			प्रत्यागम field_मानम्_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    जइ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			जइ (char2cells(c3) > 1)
			    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    जइ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    अण्णहा  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		जइ (*s == ',' || *s == NUL)
		{
		    जइ (round > 0)
		    {
			जइ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			अण्णहा जइ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			अण्णहा जइ (tab[i].cp != रिक्त)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    भङ्ग;
		}
		अण्णहा
		    प्रत्यागम field_मानम्_err(errbuf, errbuflen,
				    e_wrong_सङ्ख्या_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    जइ (i == entries)
		प्रत्यागम e_invalid_argument;

	    जइ (*p == ',')
		++p;
	}

	जइ (is_listchars && has_leadtab && !has_tab)
	    प्रत्यागम e_leadtab_requires_tab;
    }

    जइ (apply)
    {
	जइ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	अण्णहा
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    प्रत्यागम रिक्त;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    वर्ण *
set_fillchars_option(win_T *wp, char_u *val, पूर्णाङ्क apply, वर्ण *errbuf,
							      size_t errbuflen)
{
    प्रत्यागम set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    वर्ण *
set_listchars_option(win_T *wp, char_u *val, पूर्णाङ्क apply, वर्ण *errbuf,
							      size_t errbuflen)
{
    प्रत्यागम set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    जइ (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(filltab))
	प्रत्यागम रिक्त;

    प्रत्यागम filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    जइ (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(lcstab))
	प्रत्यागम रिक्त;

    प्रत्यागम lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    वर्ण *
check_chars_options(शून्य)
{
    tabpage_T   *tp;
    win_T	    *wp;

    जइ (set_listchars_option(curwin, p_lcs, FALSE, रिक्त, 0) != रिक्त)
	प्रत्यागम e_conflicts_with_मानम्_of_listchars;
    जइ (set_fillchars_option(curwin, p_fcs, FALSE, रिक्त, 0) != रिक्त)
	प्रत्यागम e_conflicts_with_मानम्_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जइ (set_listchars_option(wp, wp->w_p_lcs, FALSE, रिक्त, 0) != रिक्त)
	    प्रत्यागम e_conflicts_with_मानम्_of_listchars;
	जइ (set_fillchars_option(wp, wp->w_p_fcs, FALSE, रिक्त, 0) != रिक्त)
	    प्रत्यागम e_conflicts_with_मानम्_of_fillchars;
    }
    प्रत्यागम रिक्त;
}
