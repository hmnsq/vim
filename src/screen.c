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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	screen_attr = 0;

ಸ್ಥಿರ ಶೂನ್ಯ screen_char_2(unsigned off, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screenclear2(ಪೂರ್ಣಾಙ್ಕ doclear);
ಸ್ಥಿರ ಶೂನ್ಯ lineclear(unsigned off, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ lineinvalid(unsigned off, ಪೂರ್ಣಾಙ್ಕ width);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win_do_ಸಾಲುಗಳು(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ mayclear, ಪೂರ್ಣಾಙ್ಕ del, ಪೂರ್ಣಾಙ್ಕ clear_attr);
ಸ್ಥಿರ ಶೂನ್ಯ win_rest_invalid(win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_pos_mode(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ recording_mode(ಪೂರ್ಣಾಙ್ಕ attr);

// Ugly global: overrule attribute used by screen_char()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screen_char_attr = 0;

#ಯದಿ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    ಪೂರ್ಣಾಙ್ಕ
conceal_cursor_ಸಾಲು(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (*wp->w_p_cocu == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (get_real_state() & MODE_VISUAL)
	c = 'v';
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
	c = 'i';
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_NORMAL)
	c = 'n';
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_CMDLINE)
	c = 'c';
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ vim_strchr(wp->w_p_cocu, c) != ರಿಕ್ತ;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    ಶೂನ್ಯ
conceal_check_cursor_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ was_concealed)
{
    ಯದಿ (curwin->w_p_cole <= 0 || conceal_cursor_ಸಾಲು(curwin) == was_concealed)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ wcol = curwin->w_wcol;

    need_cursor_ಸಾಲು_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ಯದಿ (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_win_attr(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ಯದಿ (win_attr != 0)
    {
	ಯದಿ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	ಅನ್ಯಥಾ
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(wp))
    {
	ಯದಿ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	ಅನ್ಯಥಾ
	    win_attr = HL_ATTR(HLF_ತೆಗೆ);    // Popup
    }
#endif

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
    ಪ್ರತ್ಯಾಗಮ win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
screen_fill_end(
	win_T *wp,
	ಪೂರ್ಣಾಙ್ಕ	c1,
	ಪೂರ್ಣಾಙ್ಕ	c2,
	ಪೂರ್ಣಾಙ್ಕ	off,
	ಪೂರ್ಣಾಙ್ಕ	width,
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	endrow,
	ಪೂರ್ಣಾಙ್ಕ	attr)
{
    ಪೂರ್ಣಾಙ್ಕ	    nn = off + width;

    ಯದಿ (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    ಅನ್ಯಥಾ
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (ಪೂರ್ಣಾಙ್ಕ)wp->w_wincol + nn,
		c1, c2, attr);
    ಪ್ರತ್ಯಾಗಮ nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    ಶೂನ್ಯ
win_draw_end(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		c1,
    ಪೂರ್ಣಾಙ್ಕ		c2,
    ಪೂರ್ಣಾಙ್ಕ		draw_margin,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		endrow,
    hlf_T	hl)
{
    ಪೂರ್ಣಾಙ್ಕ		n = 0;
    ಪೂರ್ಣಾಙ್ಕ		attr = HL_ATTR(hl);
    ಪೂರ್ಣಾಙ್ಕ		win_attr = get_win_attr(wp);
    bool	override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ಯದಿ (draw_margin)
    {
#ifdef FEAT_FOLDING
	ಪೂರ್ಣಾಙ್ಕ	fdc = compute_foldcolumn(wp, 0);

	ಯದಿ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	ಯದಿ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	ಯದಿ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == ರಿಕ್ತ)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, ಸಂಖ್ಯೆ_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    ಅನ್ಯಥಾ
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    ಪೂರ್ಣಾಙ್ಕ
compute_foldcolumn(win_T *wp, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    ಪೂರ್ಣಾಙ್ಕ n = wp->w_width - (col + wmw);

    ಪ್ರತ್ಯಾಗಮ MIN(wp->w_p_fdc, n);
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
    ಪೂರ್ಣಾಙ್ಕ		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		level;
    ಪೂರ್ಣಾಙ್ಕ		first_level;
    ಪೂರ್ಣಾಙ್ಕ		empty;
    ಪೂರ್ಣಾಙ್ಕ		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಚಿಹ್ನೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    ಯದಿ (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    ಪ್ರತಿ (i = 0; i < n; i++)
    {
	ಯದಿ (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_ಕೆಳಗಿನ_level)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldopen;
	ಅನ್ಯಥಾ ಯದಿ (first_level == 1)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldsep;
	ಅನ್ಯಥಾ ಯದಿ (wp->w_fill_chars.foldinner != NUL)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldinner;
	ಅನ್ಯಥಾ ಯದಿ (first_level + i <= 9)
	    ಚಿಹ್ನೆ = '0' + first_level + i;
	ಅನ್ಯಥಾ
	    ಚಿಹ್ನೆ = '>';

	len = utf_char2bytes(ಚಿಹ್ನೆ, &p[byte_counter]);
	byte_counter += len;
	ಯದಿ (first_level + i >= level)
	{
	    i++;
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (closed)
    {
	ಯದಿ (ಚಿಹ್ನೆ != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ಯದಿ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	ಚಿಹ್ನೆ = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(ಚಿಹ್ನೆ, &p[byte_counter]);
	byte_counter += len;
    }

    ಪ್ರತ್ಯಾಗಮ MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
comp_char_differs(ಪೂರ್ಣಾಙ್ಕ off_from, ಪೂರ್ಣಾಙ್ಕ off_to)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
    {
	ಯದಿ (Screen_ಸಾಲುಗಳು_C[i][off_from] != Screen_ಸಾಲುಗಳು_C[i][off_to])
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಯದಿ (Screen_ಸಾಲುಗಳು_C[i][off_from] == 0)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
char_needs_redraw(ಪೂರ್ಣಾಙ್ಕ off_from, ಪೂರ್ಣಾಙ್ಕ off_to, ಪೂರ್ಣಾಙ್ಕ cols)
{
    ಯದಿ (cols > 0
	    && ((Screen_ಸಾಲುಗಳು[off_from] != Screen_ಸಾಲುಗಳು[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_ಸಾಲುಗಳು[off_from + 1]
						 != Screen_ಸಾಲುಗಳು[off_to + 1])))
		|| (enc_utf8
		    && (Screen_ಸಾಲುಗಳು_UC[off_from] != Screen_ಸಾಲುಗಳು_UC[off_to]
			|| (Screen_ಸಾಲುಗಳು_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_ಸಾಲುಗಳು[off_from + 1]
						!= Screen_ಸಾಲುಗಳು[off_to + 1])))))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    ಪೂರ್ಣಾಙ್ಕ
screen_get_current_ಸಾಲು_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
blocked_by_popup(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ off;

    ಯದಿ (!popup_visible)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    off = row * screen_Columns + col;
    ಪ್ರತ್ಯಾಗಮ popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    ಶೂನ್ಯ
reset_screen_attr(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    ಅನ್ಯಥಾ
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_for_popup(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ಯದಿ (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    ಯದಿ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
    ಯದಿ (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    ಯದಿ (blocked_by_popup(row, col))
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ಸ್ಥಿರ char_u	*suppressed_cells = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	suppressed_rows = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	suppressed_cols = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
mark_suppressed_cell(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (suppressed_cells == ರಿಕ್ತ
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	ಯದಿ (suppressed_cells == ರಿಕ್ತ)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unmark_suppressed_cell(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (suppressed_cells != ರಿಕ್ತ
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_suppressed_cell(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪ್ರತ್ಯಾಗಮ suppressed_cells != ರಿಕ್ತ
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		popup_attr;	// get_win_attr(screen_opacity_popup)
    ಪೂರ್ಣಾಙ್ಕ		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
popup_base_attr_or(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ fallback)
{
    ಪೂರ್ಣಾಙ್ಕ		base = fallback;

    popup_get_base_screen_cell(row, col, ರಿಕ್ತ, &base, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
popup_blend_with_base(
	ಧ್ರುವ popup_opacity_T	*po,
	ಪೂರ್ಣಾಙ್ಕ			char_attr,
	ಪೂರ್ಣಾಙ್ಕ			base_attr,
	ಪೂರ್ಣಾಙ್ಕ			blend_fg)
{
    ಪೂರ್ಣಾಙ್ಕ		combined = hl_combine_attr(po->popup_attr, char_attr);

    ಪ್ರತ್ಯಾಗಮ hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_wide_char_opacity_attrs(
	ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ col2,
	sattr_T *attr1, sattr_T *attr2)
{
    ಪೂರ್ಣಾಙ್ಕ		bg1, bg2;
    ಪೂರ್ಣಾಙ್ಕ		base1 = 0;
    ಪೂರ್ಣಾಙ್ಕ		base2 = 0;
    attrentry_T	*ae;

    ಯದಿ (*attr1 == *attr2)
	ಪ್ರತ್ಯಾಗಮ;

    popup_get_base_screen_cell(row, col1, ರಿಕ್ತ, &base1, ರಿಕ್ತ);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != ರಿಕ್ತ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ಅನ್ಯಥಾ
    bg1 = (ae != ರಿಕ್ತ && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, ರಿಕ್ತ, &base2, ರಿಕ್ತ);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != ರಿಕ್ತ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ಅನ್ಯಥಾ
    bg2 = (ae != ರಿಕ್ತ && ae->ae_u.cterm.bg_color != 0);
# endif

    ಯದಿ (bg1 && !bg2)
	*attr1 = *attr2;
    ಅನ್ಯಥಾ ಯದಿ (!bg1 && bg2)
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
    ಶೂನ್ಯ
screen_ಸಾಲು(
	win_T	*wp,
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	coloff,
	ಪೂರ್ಣಾಙ್ಕ	endcol,
	ಪೂರ್ಣಾಙ್ಕ	clear_width,
	colnr_T	last_vcol,
	ಪೂರ್ಣಾಙ್ಕ	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    ಪೂರ್ಣಾಙ್ಕ		    col = 0;
    ಪೂರ್ಣಾಙ್ಕ		    hl;
    ಪೂರ್ಣಾಙ್ಕ		    force = FALSE;	// force update rest of the line
    ಪೂರ್ಣಾಙ್ಕ		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    ಪೂರ್ಣಾಙ್ಕ		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    ಪೂರ್ಣಾಙ್ಕ		    changed_this;	// TRUE if character changed
    ಪೂರ್ಣಾಙ್ಕ		    changed_next;	// TRUE if next character changed
#endif
    ಪೂರ್ಣಾಙ್ಕ		    clear_next = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    ಪೂರ್ಣಾಙ್ಕ		    drawing_opacity_popup =
				screen_opacity_popup != ರಿಕ್ತ
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    ಯದಿ (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    ಯದಿ (row >= Rows)
	row = Rows - 1;
    ಯದಿ (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    off_to = ಸಾಲು_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = ಸಾಲು_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ಯದಿ (clear_width > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ clear_start = col;

	    ಯಾವತ್ (col <= endcol && Screen_ಸಾಲುಗಳು[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    ಯದಿ (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = ಸಾಲು_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    ಯದಿ (coloff > 0 && enc_utf8
		   && Screen_ಸಾಲುಗಳು[off_to] == 0
		   && Screen_ಸಾಲುಗಳು_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_ಸಾಲುಗಳು_UC[off_to - 1]) > 1)
    {
	Screen_ಸಾಲುಗಳು[off_to - 1] = ' ';
	Screen_ಸಾಲುಗಳು_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	ಯದಿ (screen_opacity_popup == ರಿಕ್ತ)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    ಯಾವತ್ (col < endcol)
    {
	ಯದಿ (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	ಅನ್ಯಥಾ
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ಯದಿ (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	ಯದಿ (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    ಯದಿ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ಯದಿ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	ಯದಿ (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ಯದಿ (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	ಪೂರ್ಣಾಙ್ಕ opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ಯದಿ (drawing_opacity_popup
		&& Screen_ಸಾಲುಗಳು[off_from] == ' '
		&& (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_from] == 0))
	{
	    ಪೂರ್ಣಾಙ್ಕ popup_src_attr = ScreenAttrs[off_from];
	    ಪೂರ್ಣಾಙ್ಕ bg_is_empty = Screen_ಸಾಲುಗಳು[off_to] == 0
				&& (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ಯದಿ (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    ಅನ್ಯಥಾ ಯದಿ (Screen_ಸಾಲುಗಳು[off_to] != 0
		    || (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_to] != 0))
	    {
		ಪೂರ್ಣಾಙ್ಕ bg_char_cells = (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_to] != 0)
				? utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off_to]) : 1;

		ಯದಿ (bg_char_cells == 2)
		{
		    ಯದಿ (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			ಪೂರ್ಣಾಙ್ಕ base_attr;
			Screen_ಸಾಲುಗಳು[off_to] = ' ';
			ಯದಿ (enc_utf8)
			    Screen_ಸಾಲುಗಳು_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			ಗಚ್ಛ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    ಯದಿ (!(Screen_ಸಾಲುಗಳು[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_from + 1] == 0)))
			ಗಚ್ಛ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		ಪೂರ್ಣಾಙ್ಕ base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ಯದಿ (bg_char_cells == 2)
		{
		    ಪೂರ್ಣಾಙ್ಕ base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    ಯದಿ (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    ಅನ್ಯಥಾ ಯದಿ (bg_is_empty)
	    {
		ಪೂರ್ಣಾಙ್ಕ base_attr;
		Screen_ಸಾಲುಗಳು[off_to] = ' ';
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

	ಯದಿ (redraw_this)
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
	    ಯದಿ (       p_wiv
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
		ಯದಿ (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		ಅನ್ಯಥಾ
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ಯದಿ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ಯದಿ (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_ಸಾಲುಗಳು[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		ಅನ್ಯಥಾ ಯದಿ (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_ಸಾಲುಗಳು[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		ಯದಿ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ಯದಿ (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_ಸಾಲುಗಳು[off_to] = Screen_ಸಾಲುಗಳು[off_from];
	    ಯದಿ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off_to] = Screen_ಸಾಲುಗಳು_UC[off_from];
		ಯದಿ (Screen_ಸಾಲುಗಳು_UC[off_from] != 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    i;

		    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
			Screen_ಸಾಲುಗಳು_C[i][off_to] = Screen_ಸಾಲುಗಳು_C[i][off_from];
		}
	    }
	    ಯದಿ (char_cells == 2)
	    {
		Screen_ಸಾಲುಗಳು[off_to + 1] = Screen_ಸಾಲುಗಳು[off_from + 1];
		ಯದಿ (enc_utf8)
		    Screen_ಸಾಲುಗಳು_UC[off_to + 1] = 0;
	    }

#ಯದಿ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ಯದಿ (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ಯದಿ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		ಯದಿ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ಯದಿ (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ಯದಿ (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ಯದಿ (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ಯದಿ (drawing_opacity_popup && po.blend > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ popup_src_attr = ScreenAttrs[off_from];
		ಪೂರ್ಣಾಙ್ಕ scol1 = col + coloff;
		ಪೂರ್ಣಾಙ್ಕ base1 = popup_base_attr_or(row, scol1, 0);

		ಯದಿ (char_cells == 2)
		{
		    ಪೂರ್ಣಾಙ್ಕ base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    ಪೂರ್ಣಾಙ್ಕ over1 = popup_is_over_opacity(row, scol1);
		    ಪೂರ್ಣಾಙ್ಕ over2 = popup_is_over_opacity(row, scol1 + 1);
		    ಯದಿ (over1 != over2)
		    {
			ಯದಿ (over1)
			    base1 = base2;
			ಅನ್ಯಥಾ
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    ಅನ್ಯಥಾ
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ಯದಿ (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    ಯದಿ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    ಅನ್ಯಥಾ
		screen_char(off_to, row, col + coloff);
	}
	ಅನ್ಯಥಾ ಯದಿ (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    ಯದಿ (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	ಯದಿ (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    ಯದಿ (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    ಯದಿ (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_ಸಾಲುಗಳು[off_to] = ' ';
	ಯದಿ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	ಯದಿ (screen_opacity_popup == ರಿಕ್ತ)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    ಯದಿ (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	ಪೂರ್ಣಾಙ್ಕ startCol = col;
#endif

	// blank out the rest of the line
	ಯಾವತ್ (col < clear_width && Screen_ಸಾಲುಗಳು[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_to] == 0)
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
	ಯದಿ (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ಯದಿ (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		ಯದಿ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    ಪೂರ್ಣಾಙ್ಕ prev_cells = 1;

		    ಯದಿ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_ಸಾಲುಗಳು[off_to - 1] == 0 ? 2 : 1;
		    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = ಸಾಲು_Offset[row];
			unsigned max_off = ಸಾಲು_Offset[row] + screen_Columns;

			ಯಾವತ್ (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ಯದಿ (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			ಯದಿ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			ಅನ್ಯಥಾ
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    ಯಾವತ್ (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    ಯದಿ (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ಯದಿ (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    ಯದಿ (!skip_for_popup(row, col + coloff))
	    {
		ಪೂರ್ಣಾಙ್ಕ c;

		c = fillchar_vsep(&hl, wp, row);
		ಯದಿ (Screen_ಸಾಲುಗಳು[off_to] != (schar_T)c
			|| (enc_utf8 && (ಪೂರ್ಣಾಙ್ಕ)Screen_ಸಾಲುಗಳು_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_ಸಾಲುಗಳು[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    ಯದಿ (enc_utf8)
		    {
			ಯದಿ (c >= 0x80)
			{
			    Screen_ಸಾಲುಗಳು_UC[off_to] = c;
			    Screen_ಸಾಲುಗಳು_C[0][off_to] = 0;
			    Screen_ಸಾಲುಗಳು[off_to] = 0x80; // avoid storing zero
			}
			ಅನ್ಯಥಾ
			    Screen_ಸಾಲುಗಳು_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	ಅನ್ಯಥಾ
	    ಸಾಲು_Wraps[row] = FALSE;
    }

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಯದಿ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    ಶೂನ್ಯ
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಙ್ಕ		t;

    ಪ್ರತಿ (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
    ಶೂನ್ಯ
draw_vsep_win(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row)
{
    ಯದಿ (!wp->w_vsep_width)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    ಪೂರ್ಣಾಙ್ಕ start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = start_row; r < content_end; ++r)
    {
	ಪೂರ್ಣಾಙ್ಕ hl;
	ಪೂರ್ಣಾಙ್ಕ c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ಯದಿ (wp->w_status_height != 0)
    {
	ಪೂರ್ಣಾಙ್ಕ hl;
	ಪೂರ್ಣಾಙ್ಕ c;
	ಯದಿ (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	ಅನ್ಯಥಾ
	    c = fillchar_vsep(&hl, wp, content_end);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    ಪೂರ್ಣಾಙ್ಕ
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    ಯಾವತ್ (fr->fr_parent != ರಿಕ್ತ)
    {
	ಯದಿ (fr->fr_parent->fr_layout == FR_COL)
	{
	    ಯದಿ (fr->fr_next != ರಿಕ್ತ)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fr->fr_next != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	fr = fr->fr_parent;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    ಪೂರ್ಣಾಙ್ಕ		len)	    // length of buffer
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		plen;

    ಯದಿ (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	ಪ್ರತ್ಯಾಗಮ 0;

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
    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	ಯದಿ (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	ಅನ್ಯಥಾ
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((ವರ್ಣ *)buf, len, (ವರ್ಣ *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    ಯದಿ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    ಪ್ರತ್ಯಾಗಮ plen;
}

#ಯದಿ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    ಶೂನ್ಯ
win_redr_custom(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		draw_ruler)	// TRUE or FALSE
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		curattr;
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;
    ಪೂರ್ಣಾಙ್ಕ		maxwidth;
    ಪೂರ್ಣಾಙ್ಕ		width;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		stlh_cnt;
    ಪೂರ್ಣಾಙ್ಕ		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    ಪೂರ್ಣಾಙ್ಕ		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    ಪೂರ್ಣಾಙ್ಕ		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ಯದಿ (entered)
	ಪ್ರತ್ಯಾಗಮ;
    entered = TRUE;

    // setup environment for the task at hand
    ಯದಿ (wp == ರಿಕ್ತ)
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
    ಅನ್ಯಥಾ
    {
	override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	ಪೂರ್ಣಾಙ್ಕ in_status_ಸಾಲು = wp->w_status_height != 0;
	maxwidth = in_status_ಸಾಲು ? wp->w_width : cmdline_width;

	ಯದಿ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ಯದಿ (*stl == '%')
	    {
		ಯದಿ (*++stl == '-')
		    stl++;
		ಯದಿ (atoi((ವರ್ಣ *)stl))
		    ಯಾವತ್ (VIM_ISDIGIT(*stl))
			stl++;
		ಯದಿ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    ಯದಿ (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    ಯದಿ (!in_status_ಸಾಲು)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    opt_name = (char_u *)"statusline";
	    ಯದಿ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    ಅನ್ಯಥಾ
		stl = p_stl;
	}

	ಯದಿ (in_status_ಸಾಲು)
	    col += wp->w_wincol;
	ಅನ್ಯಥಾ
	    col += cmdline_col_off;
    }

    ಯದಿ (maxwidth <= 0)
	ಗಚ್ಛ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == ರಿಕ್ತ ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == ರಿಕ್ತ) ? (char_u *)"" : stl;
    ಪೂರ್ಣಾಙ್ಕ col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			*out_ಎಣಿಕೆ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			region_base_col = 0;
    ಯದಿ (!draw_ruler)
    {
	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    out_regions = &wp->w_stl_click;
	    out_ಎಣಿಕೆ = &wp->w_stl_click_ಎಣಿಕೆ;
	    region_base_col = wp->w_wincol;
	}
	ಅನ್ಯಥಾ
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_ಎಣಿಕೆ = &tabline_stl_click_ಎಣಿಕೆ;
	    region_base_col = firstwin->w_wincol;
	}

	ಪ್ರತಿ (n = 0; n < *out_ಎಣಿಕೆ; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_ಎಣಿಕೆ = 0;
    }

    ಪೂರ್ಣಾಙ್ಕ carry_hl = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, ಪರಿಮಾಣ(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    len = vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), "%s", p);
	    vim_free(p);
	}
	ಅನ್ಯಥಾ
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf);

	// fill up with "fillchar"
	ಯಾವತ್ (width < maxwidth && len < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf) - 1)
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
	ಪ್ರತಿ (n = 0; hltab[n].start != ರಿಕ್ತ; n++)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    ಯದಿ (hltab[n].userhl == 0)
		curattr = attr;
	    ಅನ್ಯಥಾ ಯದಿ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    ಅನ್ಯಥಾ ಯದಿ (wp != ರಿಕ್ತ && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    ಅನ್ಯಥಾ ಯದಿ (wp != ರಿಕ್ತ && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    ಅನ್ಯಥಾ ಯದಿ (wp != ರಿಕ್ತ && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    ಅನ್ಯಥಾ
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ಯದಿ (out_regions != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ click_ಎಣಿಕೆ = 0;
	    ಪ್ರತಿ (n = 0; clicktab[n].start != ರಿಕ್ತ; n++)
		click_ಎಣಿಕೆ++;

	    ಯದಿ (click_ಎಣಿಕೆ > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			ಪರಿಮಾಣ(stl_click_region_T)
				    * (*out_ಎಣಿಕೆ + click_ಎಣಿಕೆ));
		ಯದಿ (new_arr != ರಿಕ್ತ)
		{
		    char_u  *cur_funcname = ರಿಕ್ತ;
		    ಪೂರ್ಣಾಙ್ಕ	    cur_minwid = 0;
		    ಪೂರ್ಣಾಙ್ಕ	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    ಪ್ರತಿ (n = 0; clicktab[n].start != ರಿಕ್ತ; n++)
		    {
			len += vim_strnsize(p,
					    (ಪೂರ್ಣಾಙ್ಕ)(clicktab[n].start - p));
			p = clicktab[n].start;

			ಯದಿ (cur_funcname != ರಿಕ್ತ)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_ಎಣಿಕೆ];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_ಎಣಿಕೆ)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    ಯದಿ (cur_funcname != ರಿಕ್ತ)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_ಎಣಿಕೆ];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_ಎಣಿಕೆ)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	ಪ್ರತಿ (n = 0; clicktab[n].start != ರಿಕ್ತ; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ಯದಿ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ಯದಿ (stl != ರಿಕ್ತ)
	vim_free(stl_tmp);

    ಯದಿ (wp == ರಿಕ್ತ)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	ಪ್ರತಿ (n = 0; tabtab[n].start != ರಿಕ್ತ; n++)
	{
	    len += vim_strnsize(p, (ಪೂರ್ಣಾಙ್ಕ)(tabtab[n].start - p));
	    ಯಾವತ್ (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	ಯಾವತ್ (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    ಶೂನ್ಯ
screen_putchar(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ಯದಿ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
screen_getbytes(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *bytes, ಪೂರ್ಣಾಙ್ಕ *attrp)
{
    unsigned off;

    // safety check
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ || row >= screen_Rows || col >= screen_Columns)
	ಪ್ರತ್ಯಾಗಮ;

    off = ಸಾಲು_Offset[row] + col;
    ಯದಿ (attrp != ರಿಕ್ತ)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_ಸಾಲುಗಳು[off];
    bytes[1] = NUL;

    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
    {
	bytes[0] = Screen_ಸಾಲುಗಳು[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_ಸಾಲುಗಳು[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
screen_comp_differs(ಪೂರ್ಣಾಙ್ಕ off, ಪೂರ್ಣಾಙ್ಕ *u8cc)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
    {
	ಯದಿ (Screen_ಸಾಲುಗಳು_C[i][off] != (u8char_T)u8cc[i])
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಯದಿ (u8cc[i] == 0)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    ಶೂನ್ಯ
screen_puts(
    char_u	*text,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    ಶೂನ್ಯ
screen_puts_len(
    char_u	*text,
    ಪೂರ್ಣಾಙ್ಕ		textlen,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		attr_arg)
{
    ಪೂರ್ಣಾಙ್ಕ		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    ಪೂರ್ಣಾಙ್ಕ		len = textlen;
    ಪೂರ್ಣಾಙ್ಕ		c;
    unsigned	max_off;
    ಪೂರ್ಣಾಙ್ಕ		mbyte_blen = 1;
    ಪೂರ್ಣಾಙ್ಕ		mbyte_cells = 1;
    ಪೂರ್ಣಾಙ್ಕ		u8c = 0;
    ಪೂರ್ಣಾಙ್ಕ		u8cc[MAX_MCO];
    ಪೂರ್ಣಾಙ್ಕ		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    ಪೂರ್ಣಾಙ್ಕ		prev_c = 0;		// previous Arabic character
    ಪೂರ್ಣಾಙ್ಕ		pc, nc, nc1;
    ಪೂರ್ಣಾಙ್ಕ		pcc[MAX_MCO];
#endif
    ಪೂರ್ಣಾಙ್ಕ		force_redraw_this;
    ಪೂರ್ಣಾಙ್ಕ		force_redraw_next = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	ಪ್ರತ್ಯಾಗಮ;
    off = ಸಾಲು_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ಯದಿ (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ಯದಿ (!skip_for_popup(row, col - 1))
	{
	    Screen_ಸಾಲುಗಳು[off - 1] = ' ';
	    ಯದಿ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off - 1] = 0;
		Screen_ಸಾಲುಗಳು_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = ಸಾಲು_Offset[row] + screen_Columns;
    ಯಾವತ್ (col < screen_Columns
	    && (len < 0 || (ಪೂರ್ಣಾಙ್ಕ)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ಯದಿ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (ಪೂರ್ಣಾಙ್ಕ)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ಯದಿ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    ಅನ್ಯಥಾ	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (ಪೂರ್ಣಾಙ್ಕ)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		ಯದಿ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ಯದಿ (len >= 0 && (ಪೂರ್ಣಾಙ್ಕ)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    ಅನ್ಯಥಾ
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(ಪೂರ್ಣಾಙ್ಕ)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		ಅನ್ಯಥಾ
		    prev_c = u8c;
#endif
		ಯದಿ (col + mbyte_cells > screen_Columns)
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
	ಪೂರ್ಣಾಙ್ಕ cell_attr = attr;
	ಯದಿ (screen_pum_blend > 0 && pum_bg_attrs != ರಿಕ್ತ
		&& row >= pum_bg_ತುದಿ && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    ಪೂರ್ಣಾಙ್ಕ soff = (row - pum_bg_ತುದಿ) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_ಸಾಲುಗಳು[off] != c
		|| (mbyte_cells == 2
		    && Screen_ಸಾಲುಗಳು[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_ಸಾಲುಗಳು_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_ಸಾಲುಗಳು_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	ಯದಿ ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#ಯದಿ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ಯದಿ (need_redraw && Screen_ಸಾಲುಗಳು[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# ಯದಿ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		ಪೂರ್ಣಾಙ್ಕ	n = ScreenAttrs[off];

		ಯದಿ (n > HL_ALL)
		    n = syn_attr2attr(n);
		ಯದಿ (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ಯದಿ (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ಯದಿ (clear_next_cell)
		clear_next_cell = FALSE;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ಯದಿ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_ಸಾಲುಗಳು[off + mbyte_blen] = 0;
	    Screen_ಸಾಲುಗಳು[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    ಯದಿ (enc_utf8)
	    {
		ಯದಿ (c < 0x80 && u8cc[0] == 0)
		    Screen_ಸಾಲುಗಳು_UC[off] = 0;
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ	    i;

		    Screen_ಸಾಲುಗಳು_UC[off] = u8c;
		    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][off] = u8cc[i];
			ಯದಿ (u8cc[i] == 0)
			    ಭಙ್ಗ;
		    }
		}
		ಯದಿ (mbyte_cells == 2)
		{
		    Screen_ಸಾಲುಗಳು[off + 1] = 0;
		    Screen_ಸಾಲುಗಳು_UC[off + 1] = 0;
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ci = 0; ci < Screen_mco; ++ci)
			Screen_ಸಾಲುಗಳು_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mbyte_cells == 2)
	    {
		Screen_ಸಾಲುಗಳು[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    ಅನ್ಯಥಾ
		screen_char(off, row, col);
	}
	ಯದಿ (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    ಯದಿ (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ಯದಿ (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	ಯದಿ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	ಅನ್ಯಥಾ
	    screen_char(off, row, col);
    }
}

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    ಶೂನ್ಯ
start_ಹುಡುಕು_hl(ಶೂನ್ಯ)
{
    ಯದಿ (!p_hls || no_hlsearch)
	ಪ್ರತ್ಯಾಗಮ;

    end_ಹುಡುಕು_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_ಹುಡುಕು_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    ಶೂನ್ಯ
end_ಹುಡುಕು_hl(ಶೂನ್ಯ)
{
    ಯದಿ (screen_ಹುಡುಕು_hl.rm.regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_regfree(screen_ಹುಡುಕು_hl.rm.regprog);
    screen_ಹುಡುಕು_hl.rm.regprog = ರಿಕ್ತ;
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
screen_start_highlight(ಪೂರ್ಣಾಙ್ಕ attr)
{
    attrentry_T *aep = ರಿಕ್ತ;

    screen_attr = attr;
    ಯದಿ (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	ವರ್ಣ	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (attr > HL_ALL)				// special HL attr.
    {
	ಯದಿ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ಅನ್ಯಥಾ
	    aep = syn_term_attr2entry(attr);
	ಯದಿ (aep == ರಿಕ್ತ)	    // did ":syntax clear"
	    attr = 0;
	ಅನ್ಯಥಾ
	    attr = aep->ae_attr;
    }
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಯದಿ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	ಪೂರ್ಣಾಙ್ಕ	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ಯದಿ (p_tgc)
	{
	    ಯದಿ (aep == ರಿಕ್ತ || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    ಯದಿ (aep == ರಿಕ್ತ || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	ಅನ್ಯಥಾ ಯದಿ (t_colors >= 256)
	{
	    ಯದಿ (aep == ರಿಕ್ತ || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    ಯದಿ (aep == ರಿಕ್ತ || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    ಯದಿ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    ಅನ್ಯಥಾ ಯದಿ (aep != ರಿಕ್ತ && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    ಯದಿ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    ಯದಿ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    ಯದಿ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    ಯದಿ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    ಯದಿ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    ಯದಿ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    ಯದಿ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    ಯದಿ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    ಯದಿ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ಯದಿ (aep != ರಿಕ್ತ)
    {
	ಯದಿ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ಯದಿ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ಯದಿ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	ಅನ್ಯಥಾ
#endif
	    ಯದಿ (t_colors > 1)
	    {
		ಯದಿ (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ಯದಿ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ಯದಿ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	ಅನ್ಯಥಾ
#endif
	    ಯದಿ (t_colors > 1)
	    {
		ಯದಿ (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	ಯದಿ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ಯದಿ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	ಅನ್ಯಥಾ
#endif
	    ಯದಿ (t_colors > 1)
	    {
		ಯದಿ (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ಯದಿ (!IS_CTERM)
	{
	    ಯದಿ (aep->ae_u.term.start != ರಿಕ್ತ)
		out_str(aep->ae_u.term.start);
	}
    }
}

    ಶೂನ್ಯ
screen_stop_highlight(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    do_ME = FALSE;	    // output T_ME code
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಙ್ಕ	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    ಯದಿ (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	{
	    ವರ್ಣ	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	ಅನ್ಯಥಾ
#endif
	{
	    ಪೂರ್ಣಾಙ್ಕ is_under;

	    ಯದಿ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ಯದಿ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ಯದಿ (aep != ರಿಕ್ತ && ((
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
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ಯದಿ (use_vtp())
		    {
			ಯದಿ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ಯದಿ (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			ಯದಿ (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    ಅನ್ಯಥಾ
		    {
			// Process FG and BG at once.
			ಯದಿ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		ಅನ್ಯಥಾ
		{
		    aep = syn_term_attr2entry(screen_attr);
		    ಯದಿ (aep != ರಿಕ್ತ && aep->ae_u.term.stop != ರಿಕ್ತ)
		    {
			ಯದಿ (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			ಅನ್ಯಥಾ
			    out_str(aep->ae_u.term.stop);
		    }
		}
		ಯದಿ (aep == ರಿಕ್ತ)	    // did ":syntax clear"
		    screen_attr = 0;
		ಅನ್ಯಥಾ
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ಯದಿ (screen_attr & HL_STANDOUT)
	    {
		ಯದಿ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		ಅನ್ಯಥಾ
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ಯದಿ (is_under && *T_UCE != NUL)
	    {
		ಯದಿ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		ಅನ್ಯಥಾ
		    out_str(T_UCE);
	    }
	    ಯದಿ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ಯದಿ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		ಅನ್ಯಥಾ
		    out_str(T_UE);
	    }
	    ಯದಿ (screen_attr & HL_ITALIC)
	    {
		ಯದಿ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		ಅನ್ಯಥಾ
		    out_str(T_CZR);
	    }
	    ಯದಿ (screen_attr & HL_STRIKETHROUGH)
	    {
		ಯದಿ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		ಅನ್ಯಥಾ
		    out_str(T_STE);
	    }
	    ಯದಿ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    ಯದಿ (p_tgc)
	    {
		ಯದಿ (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		ಯದಿ (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		ಯದಿ (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		ಯದಿ (t_colors > 1)
		{
		    // set Normal cterm colors
		    ಯದಿ (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    ಯದಿ (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    ಯದಿ (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    ಯದಿ (cterm_normal_fg_bold)
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
    ಶೂನ್ಯ
reset_cterm_colors(ಶೂನ್ಯ)
{
    ಯದಿ (!IS_CTERM)
	ಪ್ರತ್ಯಾಗಮ;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    ಯದಿ (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#ಅನ್ಯಥಾ
	ಯದಿ (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    ಯದಿ (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    ಶೂನ್ಯ
screen_char(unsigned off, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ಯದಿ (row >= screen_Rows || col >= screen_Columns)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ಯದಿ (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    ಪೂರ್ಣಾಙ್ಕ off2 = off + 1;
	    Screen_ಸಾಲುಗಳು[off2] = ' ';
	    Screen_ಸಾಲುಗಳು_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
	    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ಯದಿ (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ಯದಿ (screen_char_attr != 0)
	attr = screen_char_attr;
    ಅನ್ಯಥಾ
	attr = ScreenAttrs[off];
    ಯದಿ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
	    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    ಯದಿ (screen_attr != attr)
	screen_start_highlight(attr);

    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ಯದಿ (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_ಸಾಲುಗಳು_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	ಅನ್ಯಥಾ ಯದಿ (utf_ambiguous_width(Screen_ಸಾಲುಗಳು_UC[off]))
	{
	    ಯದಿ (*p_ambw == 'd'
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
	ಅನ್ಯಥಾ ಯದಿ (utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    ಅನ್ಯಥಾ
    {
	out_flush_check();
	out_char(Screen_ಸಾಲುಗಳು[off]);
	// double-byte character in single-width cell
	ಯದಿ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_char_2(unsigned off, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    // Check for illegal values (could be wrong when screen was resized).
    ಯದಿ (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	ಪ್ರತ್ಯಾಗಮ;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ಯದಿ (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ಯದಿ (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_ಸಾಲುಗಳು[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    ಶೂನ್ಯ
screen_draw_rectangle(
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		height,
    ಪೂರ್ಣಾಙ್ಕ		width,
    ಪೂರ್ಣಾಙ್ಕ		invert,
    ಪೂರ್ಣಾಙ್ಕ		force)
{
    ಪೂರ್ಣಾಙ್ಕ		r, c;
    ಪೂರ್ಣಾಙ್ಕ		off;
    ಪೂರ್ಣಾಙ್ಕ		max_off;

    // Can't use ScreenLines unless initialized
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (invert)
	screen_char_attr = HL_INVERSE;
    ಪ್ರತಿ (r = row; r < row + height; ++r)
    {
	off = ಸಾಲು_Offset[r];
	max_off = off + screen_Columns;
	ಪ್ರತಿ (c = col; c < col + width; ++c)
	{
	    ಯದಿ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ಯದಿ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ಯದಿ (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_block(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ end, win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    ಯದಿ (wp == ರಿಕ್ತ)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    ಅನ್ಯಥಾ
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    ಶೂನ್ಯ
space_to_screenline(ಪೂರ್ಣಾಙ್ಕ off, ಪೂರ್ಣಾಙ್ಕ attr)
{
    Screen_ಸಾಲುಗಳು[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    ಯದಿ (enc_utf8)
	Screen_ಸಾಲುಗಳು_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    ಶೂನ್ಯ
screen_fill(
	ಪೂರ್ಣಾಙ್ಕ	start_row,
	ಪೂರ್ಣಾಙ್ಕ	end_row,
	ಪೂರ್ಣಾಙ್ಕ	start_col,
	ಪೂರ್ಣಾಙ್ಕ	end_col,
	ಪೂರ್ಣಾಙ್ಕ	c1,
	ಪೂರ್ಣಾಙ್ಕ	c2,
	ಪೂರ್ಣಾಙ್ಕ	attr)
{
    ಪೂರ್ಣಾಙ್ಕ	    row;
    ಪೂರ್ಣಾಙ್ಕ	    col;
    ಪೂರ್ಣಾಙ್ಕ	    off;
    ಪೂರ್ಣಾಙ್ಕ	    end_off;
    ಪೂರ್ಣಾಙ್ಕ	    did_delete;
    ಪೂರ್ಣಾಙ್ಕ	    c;
    ಪೂರ್ಣಾಙ್ಕ	    norm_term;
#ಯದಿ defined(FEAT_GUI) || defined(UNIX)
    ಪೂರ್ಣಾಙ್ಕ	    force_next = FALSE;
#endif

    ಯದಿ (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    ಯದಿ (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    ಪ್ರತಿ (row = start_row; row < end_row; ++row)
    {
	ಯದಿ (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ಯದಿ (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		ಪೂರ್ಣಾಙ್ಕ ಎಡ_attr = ScreenAttrs[ಸಾಲು_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, ಎಡ_attr);
	    }
	    ಯದಿ (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		ಪೂರ್ಣಾಙ್ಕ ಬಲ_attr = ScreenAttrs[ಸಾಲು_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, ಬಲ_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ಯದಿ (c2 == ' '
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
	    ಯದಿ (c1 != ' ')			// don't clear first char
		++col;

	    off = ಸಾಲು_Offset[row] + col;
	    end_off = ಸಾಲು_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    ಯದಿ (enc_utf8)
		ಯಾವತ್ (off < end_off && Screen_ಸಾಲುಗಳು[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_ಸಾಲುಗಳು_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (ಪೂರ್ಣಾಙ್ಕ)(off - ಸಾಲು_Offset[row]))
#endif
			  )
		    ++off;
	    ಅನ್ಯಥಾ
		ಯಾವತ್ (off < end_off && Screen_ಸಾಲುಗಳು[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (ಪೂರ್ಣಾಙ್ಕ)(off - ಸಾಲು_Offset[row]))
#endif
			  )
		    ++off;
	    ಯದಿ (off < end_off)		// something to be cleared
	    {
		col = off - ಸಾಲು_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		ಯಾವತ್ (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (ಪೂರ್ಣಾಙ್ಕ)(off - ಸಾಲು_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = ಸಾಲು_Offset[row] + start_col;
	c = c1;
	ಪ್ರತಿ (col = start_col; col < end_col; ++col)
	{
	    ಯದಿ ((Screen_ಸಾಲುಗಳು[off] != c
		    || (enc_utf8 && (ಪೂರ್ಣಾಙ್ಕ)Screen_ಸಾಲುಗಳು_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ಯದಿ defined(FEAT_GUI) || defined(UNIX)
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
		ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ಯದಿ (Screen_ಸಾಲುಗಳು[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off - 1] != 0
			    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off - 1]) == 2)
			ಗಚ್ಛ next_col;
		}
		ಯದಿ (screen_opacity_popup != ರಿಕ್ತ && c == ' '
						       && Screen_ಸಾಲುಗಳು[off] != 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ bg_char_cells = 1;
		    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ಯದಿ (bg_char_cells == 2)
		    {
			ಯದಿ (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    ಗಚ್ಛ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ಯದಿ (c2 != ' ')
			    ಗಚ್ಛ skip_opacity_fill;
		    }

		    ಪೂರ್ಣಾಙ್ಕ popup_attr = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಙ್ಕ blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    ಗಚ್ಛ next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		ಯದಿ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != ರಿಕ್ತ
			&& row >= pum_bg_ತುದಿ && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    ಪೂರ್ಣಾಙ್ಕ soff = (row - pum_bg_ತುದಿ) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ
			    && col > start_col
			    && pum_bg_ಸಾಲುಗಳು_UC[soff] == 0
			    && pum_bg_ಸಾಲುಗಳು_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff - 1]) == 2)
		    {
			Screen_ಸಾಲುಗಳು[off] = 0;
			ಯದಿ (Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			ಗಚ್ಛ next_col;
		    }

		    ಪೂರ್ಣಾಙ್ಕ underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ
			    && ((pum_bg_ಸಾಲುಗಳು_UC[soff] != 0
				    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_ಸಾಲುಗಳು_UC[soff] == 0
				    && col > 0
				    && pum_bg_ಸಾಲುಗಳು_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff - 1])
								       == 2)))
		    {
			Screen_ಸಾಲುಗಳು[off] = ' ';
			ಯದಿ (Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			ಗಚ್ಛ next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_ಸಾಲುಗಳು[off] = pum_bg_ಸಾಲುಗಳು[soff];
		    ಯದಿ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ರಿಕ್ತ
						&& Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
		    {
			ಪೂರ್ಣಾಙ್ಕ k;
			Screen_ಸಾಲುಗಳು_UC[off] = pum_bg_ಸಾಲುಗಳು_UC[soff];
			ಪ್ರತಿ (k = 0; k < MAX_MCO; ++k)
			    ಯದಿ (pum_bg_ಸಾಲುಗಳು_C[k] != ರಿಕ್ತ
						    && Screen_ಸಾಲುಗಳು_C[k] != ರಿಕ್ತ)
				Screen_ಸಾಲುಗಳು_C[k][off] = pum_bg_ಸಾಲುಗಳು_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    ಗಚ್ಛ next_col;
		}
#ಯದಿ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ಯದಿ (
# ifdef FEAT_GUI
			gui.in_use
# endif
# ಯದಿ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    ಯದಿ (Screen_ಸಾಲುಗಳು[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    ಅನ್ಯಥಾ
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ಯದಿ (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_ಸಾಲುಗಳು[off] = c;
		ಯದಿ (enc_utf8)
		{
		    ಯದಿ (c >= 0x80)
		    {
			Screen_ಸಾಲುಗಳು_UC[off] = c;
			Screen_ಸಾಲುಗಳು_C[0][off] = 0;
			Screen_ಸಾಲುಗಳು[off] = 0x80; // avoid storing zero
		    }
		    ಅನ್ಯಥಾ
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ಯದಿ (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    ಯದಿ (col == start_col)
	    {
		ಯದಿ (did_delete)
		    ಭಙ್ಗ;
		c = c2;
	    }
	}
	ಯದಿ (end_col == Columns)
	    ಸಾಲು_Wraps[row] = FALSE;
	ಯದಿ (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ಯದಿ (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ಯದಿ (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    ಶೂನ್ಯ
check_for_delay(ಪೂರ್ಣಾಙ್ಕ check_ಸಂದೇಶ_scroll)
{
    ಯದಿ ((emsg_on_display || (check_ಸಂದೇಶ_scroll && ಸಂದೇಶ_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ಯದಿ (check_ಸಂದೇಶ_scroll)
	    ಸಂದೇಶ_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_TabPageIdxs(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		scol;

    ಪ್ರತಿ (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    ಪೂರ್ಣಾಙ್ಕ
screen_valid(ಪೂರ್ಣಾಙ್ಕ doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    ಪ್ರತ್ಯಾಗಮ (Screen_ಸಾಲುಗಳು != ರಿಕ್ತ);
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
    ಶೂನ್ಯ
screenalloc(ಪೂರ್ಣಾಙ್ಕ doclear)
{
    ಪೂರ್ಣಾಙ್ಕ		    new_row, old_row;
#ifdef FEAT_GUI
    ಪೂರ್ಣಾಙ್ಕ		    old_Rows;
#endif
    win_T	    *wp;
    ಪೂರ್ಣಾಙ್ಕ		    outofmem = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    len;
    schar_T	    *new_Screen_ಸಾಲುಗಳು;
    u8char_T	    *new_Screen_ಸಾಲುಗಳು_UC = ರಿಕ್ತ;
    u8char_T	    *new_Screen_ಸಾಲುಗಳು_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = ರಿಕ್ತ;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_ಸಾಲು_Offset;
    char_u	    *new_ಸಾಲು_Wraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    ವರ್ಣ	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    entered = FALSE;		// avoid recursiveness
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    done_outofmem_ಸಂದೇಶ = FALSE;	// did outofmem message
    ಪೂರ್ಣಾಙ್ಕ		    retry_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ಯದಿ ((Screen_ಸಾಲುಗಳು != ರಿಕ್ತ
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != ರಿಕ್ತ)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_ಸಾಲುಗಳು == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ಯದಿ (entered)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಯದಿ (aucmd_win[i].auc_win != ರಿಕ್ತ)
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

    new_Screen_ಸಾಲುಗಳು = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_ಸಾಲುಗಳು_C, 0, ಪರಿಮಾಣ(u8char_T *) * MAX_MCO);
    ಯದಿ (enc_utf8)
    {
	new_Screen_ಸಾಲುಗಳು_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
	    new_Screen_ಸಾಲುಗಳು_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    ಯದಿ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_ಸಾಲು_Offset = LALLOC_MULT(unsigned, Rows);
    new_ಸಾಲು_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(ವರ್ಣ, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಯದಿ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ಗಚ್ಛ give_up;
	}
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಯದಿ (aucmd_win[i].auc_win != ರಿಕ್ತ
		&& aucmd_win[i].auc_win->w_ಸಾಲುಗಳು == ರಿಕ್ತ
		&& win_alloc_ಸಾಲುಗಳು(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    ಭಙ್ಗ;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ಗಚ್ಛ give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    ಯದಿ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	    {
		outofmem = TRUE;
		ಗಚ್ಛ give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
	ಯದಿ (new_Screen_ಸಾಲುಗಳು_C[i] == ರಿಕ್ತ)
	{
	    found_null = TRUE;
	    ಭಙ್ಗ;
	}
    ಯದಿ (new_Screen_ಸಾಲುಗಳು == ರಿಕ್ತ
	    || (enc_utf8 && (new_Screen_ಸಾಲುಗಳು_UC == ರಿಕ್ತ || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == ರಿಕ್ತ)
	    || new_ScreenAttrs == ರಿಕ್ತ
	    || new_ScreenCols == ರಿಕ್ತ
	    || new_ಸಾಲು_Offset == ರಿಕ್ತ
	    || new_ಸಾಲು_Wraps == ರಿಕ್ತ
	    || new_TabPageIdxs == ರಿಕ್ತ
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == ರಿಕ್ತ
	    || new_popup_mask_next == ರಿಕ್ತ
	    || new_popup_transparent == ರಿಕ್ತ
#endif
	    || outofmem)
    {
	ಯದಿ (Screen_ಸಾಲುಗಳು != ರಿಕ್ತ || !done_outofmem_ಸಂದೇಶ)
	{
	    // guess the size
	    do_outofmem_ಸಂದೇಶ((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_ಸಂದೇಶ = TRUE;
	}
	VIM_CLEAR(new_Screen_ಸಾಲುಗಳು);
	VIM_CLEAR(new_Screen_ಸಾಲುಗಳು_UC);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_ಸಾಲುಗಳು_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_ಸಾಲು_Offset);
	VIM_CLEAR(new_ಸಾಲು_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    ಅನ್ಯಥಾ
    {
	done_outofmem_ಸಂದೇಶ = FALSE;

	ಪ್ರತಿ (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_ಸಾಲು_Offset[new_row] = new_row * Columns;
	    new_ಸಾಲು_Wraps[new_row] = FALSE;

	    (ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು + new_row * Columns,
				  ' ', (size_t)Columns * ಪರಿಮಾಣ(schar_T));
	    ಯದಿ (enc_utf8)
	    {
		(ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು_UC + new_row * Columns,
				   0, (size_t)Columns * ಪರಿಮಾಣ(u8char_T));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
		    (ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು_C[i]
						      + new_row * Columns,
				   0, (size_t)Columns * ಪರಿಮಾಣ(u8char_T));
	    }
	    ಯದಿ (enc_dbcs == DBCS_JPNU)
		(ಶೂನ್ಯ)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * ಪರಿಮಾಣ(schar_T));
	    (ಶೂನ್ಯ)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * ಪರಿಮಾಣ(sattr_T));
	    (ಶೂನ್ಯ)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * ಪರಿಮಾಣ(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ಯದಿ (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		ಯದಿ (old_row >= 0 && Screen_ಸಾಲುಗಳು != ರಿಕ್ತ)
		{
		    ಯದಿ (screen_Columns < Columns)
			len = screen_Columns;
		    ಅನ್ಯಥಾ
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ಯದಿ (!(enc_utf8 && Screen_ಸಾಲುಗಳು_UC == ರಿಕ್ತ)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_ಸಾಲುಗಳು + new_ಸಾಲು_Offset[new_row],
				Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[old_row],
				(size_t)len * ಪರಿಮಾಣ(schar_T));
		    ಯದಿ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC != ರಿಕ್ತ
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_ಸಾಲುಗಳು_UC + new_ಸಾಲು_Offset[new_row],
				Screen_ಸಾಲುಗಳು_UC + ಸಾಲು_Offset[old_row],
				(size_t)len * ಪರಿಮಾಣ(u8char_T));
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_ಸಾಲುಗಳು_C[i]
						    + new_ಸಾಲು_Offset[new_row],
				Screen_ಸಾಲುಗಳು_C[i] + ಸಾಲು_Offset[old_row],
				(size_t)len * ಪರಿಮಾಣ(u8char_T));
		    }
		    ಯದಿ (enc_dbcs == DBCS_JPNU && ScreenLines2 != ರಿಕ್ತ)
			mch_memmove(new_ScreenLines2 + new_ಸಾಲು_Offset[new_row],
				ScreenLines2 + ಸಾಲು_Offset[old_row],
				(size_t)len * ಪರಿಮಾಣ(schar_T));
		    mch_memmove(new_ScreenAttrs + new_ಸಾಲು_Offset[new_row],
			    ScreenAttrs + ಸಾಲು_Offset[old_row],
			    (size_t)len * ಪರಿಮಾಣ(sattr_T));
		    mch_memmove(new_ScreenCols + new_ಸಾಲು_Offset[new_row],
			    ScreenCols + ಸಾಲು_Offset[old_row],
			    (size_t)len * ಪರಿಮಾಣ(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_ಸಾಲು = new_Screen_ಸಾಲುಗಳು + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * ಪರಿಮಾಣ(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * ಪರಿಮಾಣ(ವರ್ಣ));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_ಸಾಲುಗಳು = new_Screen_ಸಾಲುಗಳು;
    Screen_ಸಾಲುಗಳು_UC = new_Screen_ಸಾಲುಗಳು_UC;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < p_mco; ++i)
	Screen_ಸಾಲುಗಳು_C[i] = new_Screen_ಸಾಲುಗಳು_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    ಸಾಲು_Offset = new_ಸಾಲು_Offset;
    ಸಾಲು_Wraps = new_ಸಾಲು_Wraps;
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
    ಯದಿ (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ಯದಿ (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    ಅನ್ಯಥಾ ಯದಿ (gui.in_use
	    && !gui.starting
	    && Screen_ಸಾಲುಗಳು != ರಿಕ್ತ
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ಯದಿ (ಸಂದೇಶ_row >= Rows)		// Rows got smaller
	    ಸಂದೇಶ_row = Rows - 1;		// put cursor at last row
	ಅನ್ಯಥಾ ಯದಿ (Rows > old_Rows)	// Rows got bigger
	    ಸಂದೇಶ_row += Rows - old_Rows; // put cursor in same place
	ಯದಿ (ಸಂದೇಶ_col >= cmdline_width)	// cmdline_width got smaller
	    ಸಂದೇಶ_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ಯದಿ (starting == 0 && ++retry_ಎಣಿಕೆ <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	ಗಚ್ಛ retry;
    }
}

    ಶೂನ್ಯ
free_screenlines(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    VIM_CLEAR(Screen_ಸಾಲುಗಳು_UC);
    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_ಸಾಲುಗಳು_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_ಸಾಲುಗಳು);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(ಸಾಲು_Offset);
    VIM_CLEAR(ಸಾಲು_Wraps);
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
    ಪೂರ್ಣಾಙ್ಕ
screenclear(ಶೂನ್ಯ)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    ಪ್ರತ್ಯಾಗಮ screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    ಶೂನ್ಯ
redraw_as_cleared(ಶೂನ್ಯ)
{
    screenclear2(FALSE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
screenclear2(ಪೂರ್ಣಾಙ್ಕ doclear)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    did_clear = FALSE;

    ಯದಿ (starting == NO_SCREEN || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ FALSE;

#ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    ಪ್ರತಿ (i = 0; i < Rows; ++i)
    {
	lineclear(ಸಾಲು_Offset[i], (ಪೂರ್ಣಾಙ್ಕ)Columns, 0);
	ಸಾಲು_Wraps[i] = FALSE;
    }

    ಯದಿ (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ಯದಿ (suppressed_cells != ರಿಕ್ತ)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    ಅನ್ಯಥಾ
    {
	// can't clear the screen, mark all chars with invalid attributes
	ಪ್ರತಿ (i = 0; i < Rows; ++i)
	    lineinvalid(ಸಾಲು_Offset[i], (ಪೂರ್ಣಾಙ್ಕ)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    ಯದಿ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    ಸಂದೇಶ_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    ಸಂದೇಶ_row = cmdline_row;	// put cursor on last line for messages
    ಸಂದೇಶ_col = 0;
    screen_start();		// don't know where cursor is now
    ಸಂದೇಶ_didany = FALSE;
    ಸಂದೇಶ_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    ಪ್ರತ್ಯಾಗಮ did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
lineclear(unsigned off, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ attr)
{
    (ಶೂನ್ಯ)vim_memset(Screen_ಸಾಲುಗಳು + off, ' ', (size_t)width * ಪರಿಮಾಣ(schar_T));
    ಯದಿ (enc_utf8)
	(ಶೂನ್ಯ)vim_memset(Screen_ಸಾಲುಗಳು_UC + off, 0,
					  (size_t)width * ಪರಿಮಾಣ(u8char_T));
    (ಶೂನ್ಯ)vim_memset(ScreenAttrs + off, attr, (size_t)width * ಪರಿಮಾಣ(sattr_T));
    (ಶೂನ್ಯ)vim_memset(ScreenCols + off, -1, (size_t)width * ಪರಿಮಾಣ(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
lineinvalid(unsigned off, ಪೂರ್ಣಾಙ್ಕ width)
{
    (ಶೂನ್ಯ)vim_memset(ScreenAttrs + off, -1, (size_t)width * ಪರಿಮಾಣ(sattr_T));
    (ಶೂನ್ಯ)vim_memset(ScreenCols + off, -1, (size_t)width * ಪರಿಮಾಣ(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    ಶೂನ್ಯ
ಸಾಲು_was_clobbered(ಪೂರ್ಣಾಙ್ಕ screen_lnum)
{
    lineinvalid(ಸಾಲು_Offset[screen_lnum], (ಪೂರ್ಣಾಙ್ಕ)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
linecopy(ಪೂರ್ಣಾಙ್ಕ to, ಪೂರ್ಣಾಙ್ಕ from, win_T *wp)
{
    unsigned	off_to = ಸಾಲು_Offset[to] + wp->w_wincol;
    unsigned	off_from = ಸಾಲು_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_ಸಾಲುಗಳು + off_to, Screen_ಸಾಲುಗಳು + off_from,
	    wp->w_width * ಪರಿಮಾಣ(schar_T));
    ಯದಿ (enc_utf8)
    {
	ಪೂರ್ಣಾಙ್ಕ	i;

	mch_memmove(Screen_ಸಾಲುಗಳು_UC + off_to, Screen_ಸಾಲುಗಳು_UC + off_from,
		wp->w_width * ಪರಿಮಾಣ(u8char_T));
	ಪ್ರತಿ (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_ಸಾಲುಗಳು_C[i] + off_to, Screen_ಸಾಲುಗಳು_C[i] + off_from,
		    wp->w_width * ಪರಿಮಾಣ(u8char_T));
    }
    ಯದಿ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * ಪರಿಮಾಣ(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * ಪರಿಮಾಣ(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * ಪರಿಮಾಣ(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    ಪೂರ್ಣಾಙ್ಕ
can_clear(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
screen_start(ಶೂನ್ಯ)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    ಶೂನ್ಯ
windgoto(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    sattr_T	    *p;
    ಪೂರ್ಣಾಙ್ಕ		    i;
    ಪೂರ್ಣಾಙ್ಕ		    plan;
    ಪೂರ್ಣಾಙ್ಕ		    cost;
    ಪೂರ್ಣಾಙ್ಕ		    wouldbe_col;
    ಪೂರ್ಣಾಙ್ಕ		    noinvcurs;
    char_u	    *bs;
    ಪೂರ್ಣಾಙ್ಕ		    goto_cost;
    ಪೂರ್ಣಾಙ್ಕ		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (col == screen_cur_col && row == screen_cur_row)
	ಪ್ರತ್ಯಾಗಮ;

    // Check for valid position.
    ಯದಿ (row < 0)	// window without text lines?
	row = 0;
    ಯದಿ (row >= screen_Rows)
	row = screen_Rows - 1;
    ಯದಿ (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ಯದಿ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    ಅನ್ಯಥಾ
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
    ಯದಿ (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = ರಿಕ್ತ;			    // init for GCC
	attr = screen_attr;
	ಯದಿ (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ಯದಿ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    ಅನ್ಯಥಾ
		bs = T_BC;		    // "backspace character (old)
	    ಯದಿ (*bs)
		cost = (screen_cur_col - col) * (ಪೂರ್ಣಾಙ್ಕ)STRLEN(bs);
	    ಅನ್ಯಥಾ
		cost = 999;
	    ಯದಿ (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    ಅನ್ಯಥಾ
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    ಯದಿ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	ಅನ್ಯಥಾ ಯದಿ (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    ಯದಿ (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	ಅನ್ಯಥಾ
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
	ಯದಿ (i > 0)
	    cost += i;
	ಯದಿ (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + ಸಾಲು_Offset[row] + wouldbe_col;
	    ಯಾವತ್ (i && *p++ == attr)
		--i;
	    ಯದಿ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ಯದಿ (*--p == 0)
		{
		    cost += noinvcurs;
		    ಯಾವತ್ (i && *p++ == 0)
			--i;
		}
		ಯದಿ (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    ಯದಿ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		ಪ್ರತಿ (i = wouldbe_col; i < col; ++i)
		    ಯದಿ (Screen_ಸಾಲುಗಳು_UC[ಸಾಲು_Offset[row] + i] != 0)
		    {
			cost = 999;
			ಭಙ್ಗ;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ಯದಿ (cost < 999)
		ಪ್ರತಿ (i = wouldbe_col; i < col; ++i)
		    ಯದಿ (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			ಭಙ್ಗ;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ಯದಿ (cost < goto_cost)
	{
	    ಯದಿ (plan == PLAN_LE)
	    {
		ಯದಿ (noinvcurs)
		    screen_stop_highlight();
		ಯಾವತ್ (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (plan == PLAN_CR)
	    {
		ಯದಿ (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (plan == PLAN_NL)
	    {
		ಯದಿ (noinvcurs)
		    screen_stop_highlight();
		ಯಾವತ್ (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    ಯದಿ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ಯದಿ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    ಯಾವತ್ (i-- > 0)
			out_char(*T_ND);
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ	off;

		    off = ಸಾಲು_Offset[row] + screen_cur_col;
		    ಯಾವತ್ (i-- > 0)
		    {
			ಯದಿ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_ಸಾಲುಗಳು[off]);
			ಯದಿ (enc_dbcs == DBCS_JPNU
				&& Screen_ಸಾಲುಗಳು[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    ಅನ್ಯಥಾ
	cost = 999;

    ಯದಿ (cost >= goto_cost)
    {
	ಯದಿ (noinvcurs)
	    screen_stop_highlight();
	ಯದಿ (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_ಬಲ(col - screen_cur_col);
	ಅನ್ಯಥಾ
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    ಶೂನ್ಯ
setcursor(ಶೂನ್ಯ)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    ಪೂರ್ಣಾಙ್ಕ
cursor_screen_cells(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    ಶೂನ್ಯ
setcursor_mayforce(ಪೂರ್ಣಾಙ್ಕ force)
{
    ಯದಿ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((ಪೂರ್ಣಾಙ್ಕ)curwin->w_width - curwin->w_wcol
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
    ಪೂರ್ಣಾಙ್ಕ
win_ins_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		invalid,
    ಪೂರ್ಣಾಙ್ಕ		mayclear)
{
    ಪೂರ್ಣಾಙ್ಕ		did_delete;
    ಪೂರ್ಣಾಙ್ಕ		nextrow;
    ಪೂರ್ಣಾಙ್ಕ		lastrow;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    ಯದಿ (invalid)
	wp->w_ಸಾಲುಗಳು_valid = 0;

    // with only a few lines it's not worth the effort
    ಯದಿ (wp->w_height < 5)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // with the popup menu visible this might not work correctly
    ಯದಿ (pum_visible())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > wp->w_height - row)
	ಸಾಲು_ಎಣಿಕೆ = wp->w_height - row;

    retval = win_do_ಸಾಲುಗಳು(wp, row, ಸಾಲು_ಎಣಿಕೆ, mayclear, FALSE, 0);
    ಯದಿ (retval != MAYBE)
	ಪ್ರತ್ಯಾಗಮ retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ಯದಿ (wp->w_next != ರಿಕ್ತ || wp->w_status_height)
    {
	ಯದಿ (screen_del_ಸಾಲುಗಳು(0, W_WINROW(wp) + wp->w_height - ಸಾಲು_ಎಣಿಕೆ,
				  ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಙ್ಕ)Rows, FALSE, 0, ರಿಕ್ತ) == OK)
	    did_delete = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (wp->w_next)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ಯದಿ (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + ಸಾಲು_ಎಣಿಕೆ;
	ಯದಿ (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - ಸಾಲು_ಎಣಿಕೆ, lastrow - ಸಾಲು_ಎಣಿಕೆ,
		  wp->w_wincol, (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ಯದಿ (screen_ins_ಸಾಲುಗಳು(0, W_WINROW(wp) + row, ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, ರಿಕ್ತ)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ಯದಿ (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_del_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		invalid,
    ಪೂರ್ಣಾಙ್ಕ		mayclear,
    ಪೂರ್ಣಾಙ್ಕ		clear_attr)	    // for clearing lines
{
    ಪೂರ್ಣಾಙ್ಕ		retval;

    ಯದಿ (invalid)
	wp->w_ಸಾಲುಗಳು_valid = 0;

    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > wp->w_height - row)
	ಸಾಲು_ಎಣಿಕೆ = wp->w_height - row;

    retval = win_do_ಸಾಲುಗಳು(wp, row, ಸಾಲು_ಎಣಿಕೆ, mayclear, TRUE, clear_attr);
    ಯದಿ (retval != MAYBE)
	ಪ್ರತ್ಯಾಗಮ retval;

    ಯದಿ (screen_del_ಸಾಲುಗಳು(0, W_WINROW(wp) + row, ಸಾಲು_ಎಣಿಕೆ,
				   (ಪೂರ್ಣಾಙ್ಕ)Rows, FALSE, clear_attr, ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ಯದಿ (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	ಯದಿ (screen_ins_ಸಾಲುಗಳು(0, W_WINROW(wp) + wp->w_height - ಸಾಲು_ಎಣಿಕೆ,
			      ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಙ್ಕ)Rows, clear_attr, ರಿಕ್ತ) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    ಅನ್ಯಥಾ
	redraw_cmdline = TRUE;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win_do_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		mayclear,
    ಪೂರ್ಣಾಙ್ಕ		del,
    ಪೂರ್ಣಾಙ್ಕ		clear_attr)
{
    ಪೂರ್ಣಾಙ್ಕ		retval;

    ಯದಿ (!redrawing() || ಸಾಲು_ಎಣಿಕೆ <= 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ಯದಿ (no_win_do_ಸಾಲುಗಳು_ins && !del)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // only a few lines left: redraw is faster
    ಯದಿ (mayclear && Rows - ಸಾಲು_ಎಣಿಕೆ < 5
	    && wp->w_width == topframe->fr_width)
    {
	ಯದಿ (!no_win_do_ಸಾಲುಗಳು_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ಯದಿ (popup_visible)
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

    // Delete all remaining lines
    ಯದಿ (row + ಸಾಲು_ಎಣಿಕೆ >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp), ' ', ' ', 0);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ಯದಿ (!no_win_do_ಸಾಲುಗಳು_ins)
	clear_cmdline = TRUE;

#ಯದಿ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ಯದಿ (tabpanel_width() > 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
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
    ಯದಿ (scroll_region || wp->w_width != topframe->fr_width)
    {
	ಯದಿ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	ಯದಿ (del)
	    retval = screen_del_ಸಾಲುಗಳು(W_WINROW(wp) + row, 0, ಸಾಲು_ಎಣಿಕೆ,
				    wp->w_height - row, FALSE, clear_attr, wp);
	ಅನ್ಯಥಾ
	    retval = screen_ins_ಸಾಲುಗಳು(W_WINROW(wp) + row, 0, ಸಾಲು_ಎಣಿಕೆ,
					   wp->w_height - row, clear_attr, wp);
	ಯದಿ (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	ಪ್ರತ್ಯಾಗಮ retval;
    }

    ಯದಿ (wp->w_next != ರಿಕ್ತ && p_tf) // don't delete/insert on fast terminal
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_rest_invalid(win_T *wp)
{
    ಯಾವತ್ (wp != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ
screen_ins_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಙ್ಕ		off,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		end,
    ಪೂರ್ಣಾಙ್ಕ		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;
    unsigned	temp;
    ಪೂರ್ಣಾಙ್ಕ		cursor_row;
    ಪೂರ್ಣಾಙ್ಕ		cursor_col = 0;
    ಪೂರ್ಣಾಙ್ಕ		type;
    ಪೂರ್ಣಾಙ್ಕ		result_empty;
    ಪೂರ್ಣಾಙ್ಕ		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ಯದಿ (!screen_valid(TRUE)
	     || ಸಾಲು_ಎಣಿಕೆ <= 0 || ಸಾಲು_ಎಣಿಕೆ > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_PROP_POPUP
    ಯದಿ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, ಸಾಲು_ಎಣಿಕೆ);
# endif
	ಪ್ರತ್ಯಾಗಮ FAIL;
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
    result_empty = (row + ಸಾಲು_ಎಣಿಕೆ >= end);
    ಯದಿ (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > 3)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	type = USE_REDRAW;
    }
    ಅನ್ಯಥಾ ಯದಿ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    ಅನ್ಯಥಾ ಯದಿ (*T_CAL != NUL && (ಸಾಲು_ಎಣಿಕೆ > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    ಅನ್ಯಥಾ ಯದಿ (*T_CDL != NUL && result_empty && (ಸಾಲು_ಎಣಿಕೆ > 1 || !can_ce))
	type = USE_T_CDL;
    ಅನ್ಯಥಾ ಯದಿ (*T_AL != NUL)
	type = USE_T_AL;
    ಅನ್ಯಥಾ ಯದಿ (can_ce && result_empty)
	type = USE_T_CE;
    ಅನ್ಯಥಾ ಯದಿ (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    ಅನ್ಯಥಾ ಯದಿ (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ಯದಿ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	ಪ್ರತ್ಯಾಗಮ screen_del_ಸಾಲುಗಳು(off, row, ಸಾಲು_ಎಣಿಕೆ, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ಯದಿ (*T_DB)
	screen_del_ಸಾಲುಗಳು(off, end - ಸಾಲು_ಎಣಿಕೆ, ಸಾಲು_ಎಣಿಕೆ, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ಯದಿ (off + row > 0 || (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    ಅನ್ಯಥಾ
	clip_scroll_selection(-ಸಾಲು_ಎಣಿಕೆ);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, ಸಾಲು_ಎಣಿಕೆ);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    ಯದಿ (wp != ರಿಕ್ತ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ಯದಿ (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    ಅನ್ಯಥಾ
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    ಪ್ರತಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    ಯಾವತ್ ((j -= ಸಾಲು_ಎಣಿಕೆ) >= row)
		linecopy(j + ಸಾಲು_ಎಣಿಕೆ, j, wp);
	    j += ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (can_clear((char_u *)" "))
		lineclear(ಸಾಲು_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    ಅನ್ಯಥಾ
		lineinvalid(ಸಾಲು_Offset[j] + wp->w_wincol, wp->w_width);
	    ಸಾಲು_Wraps[j] = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    j = end - 1 - i;
	    temp = ಸಾಲು_Offset[j];
	    ಯಾವತ್ ((j -= ಸಾಲು_ಎಣಿಕೆ) >= row)
	    {
		ಸಾಲು_Offset[j + ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Offset[j];
		ಸಾಲು_Wraps[j + ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Wraps[j];
	    }
	    ಸಾಲು_Offset[j + ಸಾಲು_ಎಣಿಕೆ] = temp;
	    ಸಾಲು_Wraps[j + ಸಾಲು_ಎಣಿಕೆ] = FALSE;
	    ಯದಿ (can_clear((char_u *)" "))
		lineclear(temp, (ಪೂರ್ಣಾಙ್ಕ)Columns, clear_attr);
	    ಅನ್ಯಥಾ
		lineinvalid(temp, (ಪೂರ್ಣಾಙ್ಕ)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    ಯದಿ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ಯದಿ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    ಅನ್ಯಥಾ ಯದಿ (type == USE_T_CAL)
    {
	term_append_ಸಾಲುಗಳು(ಸಾಲು_ಎಣಿಕೆ);
	screen_start();		// don't know where cursor is now
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (type == USE_T_AL)
	    {
		ಯದಿ (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    ಅನ್ಯಥಾ  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ಯದಿ (type == USE_T_SR && *T_DA)
    {
	ಪ್ರತಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ಯದಿ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ಪೂರ್ಣಾಙ್ಕ
screen_del_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಙ್ಕ		off,
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		end,
    ಪೂರ್ಣಾಙ್ಕ		force,		// even when line_count > p_ttyscroll
    ಪೂರ್ಣಾಙ್ಕ		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    ಪೂರ್ಣಾಙ್ಕ		j;
    ಪೂರ್ಣಾಙ್ಕ		i;
    unsigned	temp;
    ಪೂರ್ಣಾಙ್ಕ		cursor_row;
    ಪೂರ್ಣಾಙ್ಕ		cursor_col = 0;
    ಪೂರ್ಣಾಙ್ಕ		cursor_end;
    ಪೂರ್ಣಾಙ್ಕ		result_empty;	// result is empty until end of region
    ಪೂರ್ಣಾಙ್ಕ		can_delete;	// deleting line codes can be used
    ಪೂರ್ಣಾಙ್ಕ		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ಯದಿ (!screen_valid(TRUE)
	    || ಸಾಲು_ಎಣಿಕೆ <= 0
	    || (!force && ಸಾಲು_ಎಣಿಕೆ > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + ಸಾಲು_ಎಣಿಕೆ >= end;

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
    ಯದಿ (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > 3)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	type = USE_REDRAW;
    }
    ಅನ್ಯಥಾ ಯದಿ (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    ಅನ್ಯಥಾ ಯದಿ (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    ಸಾಲು_ಎಣಿಕೆ == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    ಅನ್ಯಥಾ ಯದಿ (*T_CDL != NUL && ಸಾಲು_ಎಣಿಕೆ > 1 && can_delete)
	type = USE_T_CDL;
    ಅನ್ಯಥಾ ಯದಿ (can_clear(T_CE) && result_empty
	    && (wp == ರಿಕ್ತ || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    ಅನ್ಯಥಾ ಯದಿ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    ಅನ್ಯಥಾ ಯದಿ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ಯದಿ (off + row > 0 || (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    ಅನ್ಯಥಾ
	clip_scroll_selection(ಸಾಲು_ಎಣಿಕೆ);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -ಸಾಲು_ಎಣಿಕೆ);
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

    ಯದಿ (wp != ರಿಕ್ತ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    ಯದಿ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    ಅನ್ಯಥಾ
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
    ಪ್ರತಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (wp != ರಿಕ್ತ && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    ಯಾವತ್ ((j += ಸಾಲು_ಎಣಿಕೆ) <= end - 1)
		linecopy(j - ಸಾಲು_ಎಣಿಕೆ, j, wp);
	    j -= ಸಾಲು_ಎಣಿಕೆ;
	    ಯದಿ (can_clear((char_u *)" "))
		lineclear(ಸಾಲು_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    ಅನ್ಯಥಾ
		lineinvalid(ಸಾಲು_Offset[j] + wp->w_wincol, wp->w_width);
	    ಸಾಲು_Wraps[j] = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = ಸಾಲು_Offset[j];
	    ಯಾವತ್ ((j += ಸಾಲು_ಎಣಿಕೆ) <= end - 1)
	    {
		ಸಾಲು_Offset[j - ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Offset[j];
		ಸಾಲು_Wraps[j - ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Wraps[j];
	    }
	    ಸಾಲು_Offset[j - ಸಾಲು_ಎಣಿಕೆ] = temp;
	    ಸಾಲು_Wraps[j - ಸಾಲು_ಎಣಿಕೆ] = FALSE;
	    ಯದಿ (can_clear((char_u *)" "))
		lineclear(temp, (ಪೂರ್ಣಾಙ್ಕ)Columns, clear_attr);
	    ಅನ್ಯಥಾ
		lineinvalid(temp, (ಪೂರ್ಣಾಙ್ಕ)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    ಯದಿ (screen_attr != clear_attr)
	screen_stop_highlight();
    ಯದಿ (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    ಯದಿ (type == USE_REDRAW)
	redraw_block(row, end, wp);
    ಅನ್ಯಥಾ ಯದಿ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    ಅನ್ಯಥಾ ಯದಿ (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_ಸಾಲುಗಳು(ಸಾಲು_ಎಣಿಕೆ);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    ಅನ್ಯಥಾ ಯದಿ (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	ಪ್ರತಿ (i = ಸಾಲು_ಎಣಿಕೆ; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = ಸಾಲು_ಎಣಿಕೆ; --i >= 0; )
	{
	    ಯದಿ (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    ಅನ್ಯಥಾ // type == USE_T_CE
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
    ಯದಿ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	ಪ್ರತಿ (i = ಸಾಲು_ಎಣಿಕೆ; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    ಯದಿ (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    ಪೂರ್ಣಾಙ್ಕ
skip_showmode(ಶೂನ್ಯ)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ಯದಿ (global_busy
	    || ಸಂದೇಶ_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !ಕೀಲಿ_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
showmode(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		need_clear;
    ಪೂರ್ಣಾಙ್ಕ		length = 0;
    ಪೂರ್ಣಾಙ್ಕ		do_mode;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		nwr_save;
    ಪೂರ್ಣಾಙ್ಕ		sub_attr;
    ಪೂರ್ಣಾಙ್ಕ		show_ruler_with_pum = FALSE;

    do_mode = p_smd && ಸಂದೇಶ_silent == 0
	    && ((State & MODE_ಸೇರಿಸು)
		|| restart_edit != NUL
		|| VIsual_active);
    ಯದಿ (do_mode || reg_recording != 0)
    {
	ಯದಿ (skip_showmode())
	    ಪ್ರತ್ಯಾಗಮ 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ಯದಿ (clear_cmdline && cmdline_row < Rows - 1)
	    ಸಂದೇಶ_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	ಸಂದೇಶ_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ಯದಿ (do_mode)
	{
	    ಸಂದೇಶ_puts_attr("--", attr);
#ಯದಿ defined(FEAT_XIM)
	    ಯದಿ (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# ಅನ್ಯಥಾ
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		ಸಂದೇಶ_puts_attr(" IM", attr);
# ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    ಯದಿ (edit_submode != ರಿಕ್ತ && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - ಸಂದೇಶ_row) * cmdline_width - 3;
		ಯದಿ (edit_submode_extra != ರಿಕ್ತ)
		    length -= vim_strsize(edit_submode_extra);
		ಯದಿ (length > 0)
		{
		    ಯದಿ (edit_submode_pre != ರಿಕ್ತ)
			length -= vim_strsize(edit_submode_pre);
		    ಯದಿ (length - vim_strsize(edit_submode) > 0)
		    {
			ಯದಿ (edit_submode_pre != ರಿಕ್ತ)
			    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)edit_submode_pre, attr);
			ಸಂದೇಶ_puts_attr((ವರ್ಣ *)edit_submode, attr);
		    }
		    ಯದಿ (edit_submode_extra != ರಿಕ್ತ)
		    {
			ಸಂದೇಶ_puts_attr(" ", attr);  // add a space in between
			ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)edit_submode_highl < (ಪೂರ್ಣಾಙ್ಕ)HLF_ಎಣಿಕೆ)
			    sub_attr = HL_ATTR(edit_submode_highl);
			ಅನ್ಯಥಾ
			    sub_attr = attr;
			ಸಂದೇಶ_puts_attr((ವರ್ಣ *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (State & VREPLACE_FLAG)
		    ಸಂದೇಶ_puts_attr(_(" VREPLACE"), attr);
		ಅನ್ಯಥಾ ಯದಿ (State & REPLACE_FLAG)
		    ಸಂದೇಶ_puts_attr(_(" REPLACE"), attr);
		ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
		{
#ifdef FEAT_RIGHTLEFT
		    ಯದಿ (p_ri)
			ಸಂದೇಶ_puts_attr(_(" REVERSE"), attr);
#endif
		    ಸಂದೇಶ_puts_attr(_(" INSERT"), attr);
		}
		ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    ಸಂದೇಶ_puts_attr(_(" (insert)"), attr);
		ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'R')
		    ಸಂದೇಶ_puts_attr(_(" (replace)"), attr);
		ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'V')
		    ಸಂದೇಶ_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		ಯದಿ (p_hkmap)
		    ಸಂದೇಶ_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		ಯದಿ (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ಯದಿ (curwin->w_p_arab)
			ಸಂದೇಶ_puts_attr(_(" Arabic"), attr);
		    ಅನ್ಯಥಾ
# endif
			ಯದಿ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)NameBuff, attr);
		}
#endif
		ಯದಿ ((State & MODE_ಸೇರಿಸು) && p_paste)
		    ಸಂದೇಶ_puts_attr(_(" (paste)"), attr);

		ಯದಿ (VIsual_active)
		{
		    ವರ್ಣ *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    ವಿಕಲ್ಪ ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			ಪಕ್ಷ 0:	p = N_(" VISUAL"); ಭಙ್ಗ;
			ಪಕ್ಷ 1: p = N_(" VISUAL LINE"); ಭಙ್ಗ;
			ಪಕ್ಷ 2: p = N_(" VISUAL BLOCK"); ಭಙ್ಗ;
			ಪಕ್ಷ 4: p = N_(" SELECT"); ಭಙ್ಗ;
			ಪಕ್ಷ 5: p = N_(" SELECT LINE"); ಭಙ್ಗ;
			ಸ್ವಭಾವ: p = N_(" SELECT BLOCK"); ಭಙ್ಗ;
		    }
		    ಸಂದೇಶ_puts_attr(_(p), attr);
		}
		ಸಂದೇಶ_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	ಯದಿ (reg_recording != 0
		&& edit_submode == ರಿಕ್ತ)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	ಯದಿ (need_clear || clear_cmdline || redraw_mode)
	    ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_didout = FALSE;		// overwrite this message
	length = ಸಂದೇಶ_col;
	ಸಂದೇಶ_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    ಅನ್ಯಥಾ ಯದಿ (clear_cmdline && ಸಂದೇಶ_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	ಸಂದೇಶ_clr_cmdline();
    ಅನ್ಯಥಾ ಯದಿ (redraw_mode)
    {
	ಸಂದೇಶ_pos_mode();
	ಸಂದೇಶ_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ಯದಿ (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ಯದಿ (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    ಪ್ರತ್ಯಾಗಮ length;
}

/*
 * Position for a mode message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_pos_mode(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_col = 0;
    ಸಂದೇಶ_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    ಶೂನ್ಯ
unshowmode(ಪೂರ್ಣಾಙ್ಕ force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ಯದಿ (!redrawing() || (!force && char_avail() && !ಕೀಲಿ_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    ಅನ್ಯಥಾ
	clearmode();
}

/*
 * Clear the mode message.
 */
    ಶೂನ್ಯ
clearmode(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ save_ಸಂದೇಶ_row = ಸಂದೇಶ_row;
    ಪೂರ್ಣಾಙ್ಕ save_ಸಂದೇಶ_col = ಸಂದೇಶ_col;

    ಸಂದೇಶ_pos_mode();
    ಯದಿ (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    ಸಂದೇಶ_clr_eos();

    ಸಂದೇಶ_col = save_ಸಂದೇಶ_col;
    ಸಂದೇಶ_row = save_ಸಂದೇಶ_row;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
recording_mode(ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಸಂದೇಶ_puts_attr(_("recording"), attr);
    ಯದಿ (shortmess(SHM_RECORDING))
	ಪ್ರತ್ಯಾಗಮ;

    ವರ್ಣ s[4];

    sprintf(s, " @%c", reg_recording);
    ಸಂದೇಶ_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mouse_has_any(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_ಸೇರಿಸು)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    ಶೂನ್ಯ
draw_tabline(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		tabcount = 0;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		tabwidth;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;
    ಪೂರ್ಣಾಙ್ಕ		scol = 0;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    win_T	*wp;
    win_T	*cwp;
    ಪೂರ್ಣಾಙ್ಕ		wincount;
    ಪೂರ್ಣಾಙ್ಕ		modified;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		attr_sel;
    ಪೂರ್ಣಾಙ್ಕ		attr_nosel = 0;
    ಪೂರ್ಣಾಙ್ಕ		attr_fill = 0;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		room;
    ಪೂರ್ಣಾಙ್ಕ		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#ಯದಿ defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ಯದಿ (gui_use_tabline())
    {
	gui_update_tabline();
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (tabline_height() < 1)
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    ಯದಿ (*p_tal != NUL)
	win_redr_custom(ರಿಕ್ತ, FALSE);
    ಅನ್ಯಥಾ
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	ಯದಿ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = ತಳ್ಳು_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    ಯದಿ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ಯದಿ (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    ಯದಿ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    ಯದಿ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    ಅನ್ಯಥಾ
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    ಪ್ರತಿ (wincount = 0; wp != ರಿಕ್ತ; wp = wp->w_next, ++wincount)
		ಯದಿ (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    ಯದಿ (modified || wincount > 1)
	    {
		ಯದಿ (wincount > 1)
		{
		    len = vim_snprintf((ವರ್ಣ *)NameBuff, MAXPATHL, "%d", wincount);
		    ಯದಿ (col + len >= Columns - 3)
			ಭಙ್ಗ;
		    screen_puts_len(NameBuff, len, 0, col,
#ಯದಿ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#ಅನ್ಯಥಾ
					 attr
#endif
					       );
		    col += len;
		}
		ಯದಿ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    ಯದಿ (room > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ಯದಿ (has_mbyte)
		    ಯಾವತ್ (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		ಅನ್ಯಥಾ ಯದಿ (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		ಯದಿ (len > n)
		    len = n;

		screen_puts_len(p, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    ಯಾವತ್ (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    ಯದಿ (override_success)
		ತೆಗೆ_highlight_overrides();
	}

	ಯದಿ (use_sep_chars)
	    c = '_';
	ಅನ್ಯಥಾ
	    c = ' ';
	screen_fill(0, 1, col, (ಪೂರ್ಣಾಙ್ಕ)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ಯದಿ (p_sc && *p_sloc == 't')
	{
	    ಪೂರ್ಣಾಙ್ಕ	n = (ಪೂರ್ಣಾಙ್ಕ)Columns - col - (tabcount > 1) * 3;
	    ಪೂರ್ಣಾಙ್ಕ	width = MIN(10, n);

	    ಯದಿ (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (ಪೂರ್ಣಾಙ್ಕ)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ಯದಿ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (ಪೂರ್ಣಾಙ್ಕ)Columns - 1, attr_nosel);
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
    ಶೂನ್ಯ
get_trans_bufname(buf_T *buf)
{
    ಯದಿ (buf_spname(buf) != ರಿಕ್ತ)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    ಅನ್ಯಥಾ
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    ಪೂರ್ಣಾಙ್ಕ
fillchar_status(ಪೂರ್ಣಾಙ್ಕ *attr, win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ	    fill;
    bool    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    ಯದಿ (bt_terminal(wp->w_buffer))
    {
	ಯದಿ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	ಅನ್ಯಥಾ
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    ಅನ್ಯಥಾ
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
    ಪ್ರತ್ಯಾಗಮ fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ಸ್ಥಿರ bool
vsep_row_is_curwin(win_T *wp, ಪೂರ್ಣಾಙ್ಕ row)
{
    ಯದಿ (wp == curwin)
	ಪ್ರತ್ಯಾಗಮ true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ಯದಿ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	ಪ್ರತ್ಯಾಗಮ true;

    ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
fillchar_vsep(ಪೂರ್ಣಾಙ್ಕ *attr, win_T *wp, ಪೂರ್ಣಾಙ್ಕ row)
{
    bool override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ಯದಿ (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    ಅನ್ಯಥಾ
	*attr = HL_ATTR(HLF_CNC);
    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();

    ಯದಿ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	ಪ್ರತ್ಯಾಗಮ '|';
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
redrawing(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಯದಿ (disable_redraw_for_testing)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ
#endif
	ಪ್ರತ್ಯಾಗಮ ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !ಕೀಲಿ_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
messaging(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (!(p_lz && char_avail() && !ಕೀಲಿ_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    ಶೂನ್ಯ
comp_col(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    ಯದಿ (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#ಅನ್ಯಥಾ
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	ಯದಿ (!last_has_status)
	    sc_col = ru_col;
    }
    ಯದಿ (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	ಯದಿ (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    ಯದಿ (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    ಯದಿ (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸಂಖ್ಯೆ_width(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    linenr_T	lnum;

    ಯದಿ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    ಅನ್ಯಥಾ
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (lnum == wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ && wp->w_nuw_cached == wp->w_p_nuw)
	ಪ್ರತ್ಯಾಗಮ wp->w_nrwidth_width;
    wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = lnum;

    n = 0;
    ಕುರು
    {
	lnum /= 10;
	++n;
    } ಯಾವತ್ (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ಯದಿ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ಯದಿ (n < 2 && get_first_valid_sign(wp) != ರಿಕ್ತ
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    ಪ್ರತ್ಯಾಗಮ n;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    ಪೂರ್ಣಾಙ್ಕ
screen_screencol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    ಪೂರ್ಣಾಙ್ಕ
screen_screenrow(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ಯದಿ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T ಅಂಕೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ	    bytes;
	ಪೂರ್ಣಾಙ್ಕ	    n;

	ಪ್ರತಿ (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ಯದಿ (n < 0)
		ಪ್ರತ್ಯಾಗಮ 0;
	    ಅಂಕೆ = ಅಂಕೆ * 256 + n;
	}
	*p += 2;
	ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ;
    }
    ಪ್ರತ್ಯಾಗಮ mb_ptr2char_adv(p);
}

ಸಂರಚನಾ charstab
{
    ಪೂರ್ಣಾಙ್ಕ	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ಸ್ಥಿರ fill_chars_T fill_chars;
ಸ್ಥಿರ ಸಂರಚನಾ charstab filltab[] =
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
#ಯದಿ defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
ಸ್ಥಿರ lcs_chars_T lcs_chars;
ಸ್ಥಿರ ಸಂರಚನಾ charstab lcstab[] =
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
#ಅನ್ಯಥಾ
    CHARSTAB_ENTRY(ರಿಕ್ತ,		    "conceal"),
#endif
    CHARSTAB_ENTRY(ರಿಕ್ತ,		    "multispace"),
    CHARSTAB_ENTRY(ರಿಕ್ತ,		    "leadmultispace")
};

    ಸ್ಥಿರ ವರ್ಣ *
field_ಮೌಲ್ಯ_err(ವರ್ಣ *errbuf, size_t errbuflen, ವರ್ಣ *fmt, char_u *field)
{
    ಯದಿ (errbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    ಪ್ರತ್ಯಾಗಮ errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ಸ್ಥಿರ ವರ್ಣ *
set_chars_option(win_T *wp, char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ is_listchars, ಪೂರ್ಣಾಙ್ಕ apply,
						ವರ್ಣ *errbuf, size_t errbuflen)
{
    ಪೂರ್ಣಾಙ್ಕ	    round, i, entries;
    char_u  *p, *s;
    ಪೂರ್ಣಾಙ್ಕ	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = ರಿಕ್ತ;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = ರಿಕ್ತ; // Last occurrence of "leadmultispace:"
    ಪೂರ್ಣಾಙ್ಕ	    multispace_len = 0;	      // Length of lcs-multispace string
    ಪೂರ್ಣಾಙ್ಕ	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    ಸಂರಚನಾ charstab *tab;

    ಯದಿ (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	ಯದಿ (wp->w_p_lcs[0] == NUL)
	    ಮೌಲ್ಯ = p_lcs;  // local value is empty, use the global value
    }
    ಅನ್ಯಥಾ
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	ಯದಿ (wp->w_p_fcs[0] == NUL)
	    ಮೌಲ್ಯ = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    ಪ್ರತಿ (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	ಪೂರ್ಣಾಙ್ಕ has_tab = FALSE, has_leadtab = FALSE;

	ಯದಿ (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ಯದಿ (is_listchars)
	    {
		ಪ್ರತಿ (i = 0; i < entries; ++i)
		    ಯದಿ (tab[i].cp != ರಿಕ್ತ)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ಯದಿ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, multispace_len + 1);
		    ಯದಿ (lcs_chars.multispace != ರಿಕ್ತ)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		ಅನ್ಯಥಾ
		    lcs_chars.multispace = ರಿಕ್ತ;

		ಯದಿ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, lead_multispace_len + 1);
		    ಯದಿ (lcs_chars.leadmultispace != ರಿಕ್ತ)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		ಅನ್ಯಥಾ
		    lcs_chars.leadmultispace = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
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
#ಯದಿ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = ಮೌಲ್ಯ;
	ಯಾವತ್ (*p)
	{
	    ಪ್ರತಿ (i = 0; i < entries; ++i)
	    {
		ಯದಿ (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    ಅನುವರ್ತ;

		s = p + tab[i].name.length + 1;

		ಯದಿ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ಯದಿ (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			ಯಾವತ್ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಯದಿ (char2cells(c1) > 1)
				ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ಯದಿ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ multispace_pos = 0;

			ಯಾವತ್ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಯದಿ (p == last_multispace && lcs_chars.multispace != ರಿಕ್ತ)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ಭಙ್ಗ;
		}

		ಯದಿ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ಯದಿ (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			ಯಾವತ್ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಯದಿ (char2cells(c1) > 1)
				ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ಯದಿ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ multispace_pos = 0;

			ಯಾವತ್ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಯದಿ (p == last_lmultispace && lcs_chars.leadmultispace != ರಿಕ್ತ)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ಭಙ್ಗ;
		}

		c2 = c3 = 0;
		ಯದಿ (*s == NUL)
		    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ಯದಿ (char2cells(c1) > 1)
		    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		ಯದಿ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ಯದಿ (*s == NUL)
			ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ಯದಿ (char2cells(c2) > 1)
			ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    ಯದಿ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ಯದಿ (char2cells(c3) > 1)
			    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    ಯದಿ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    ಅನ್ಯಥಾ  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ಯದಿ (*s == ',' || *s == NUL)
		{
		    ಯದಿ (round > 0)
		    {
			ಯದಿ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			ಅನ್ಯಥಾ ಯದಿ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			ಅನ್ಯಥಾ ಯದಿ (tab[i].cp != ರಿಕ್ತ)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    ಪ್ರತ್ಯಾಗಮ field_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    ಯದಿ (i == entries)
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

	    ಯದಿ (*p == ',')
		++p;
	}

	ಯದಿ (is_listchars && has_leadtab && !has_tab)
	    ಪ್ರತ್ಯಾಗಮ e_leadtab_requires_tab;
    }

    ಯದಿ (apply)
    {
	ಯದಿ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    ವರ್ಣ *
set_fillchars_option(win_T *wp, char_u *val, ಪೂರ್ಣಾಙ್ಕ apply, ವರ್ಣ *errbuf,
							      size_t errbuflen)
{
    ಪ್ರತ್ಯಾಗಮ set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    ವರ್ಣ *
set_listchars_option(win_T *wp, char_u *val, ಪೂರ್ಣಾಙ್ಕ apply, ವರ್ಣ *errbuf,
							      size_t errbuflen)
{
    ಪ್ರತ್ಯಾಗಮ set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(filltab))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(lcstab))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    ವರ್ಣ *
check_chars_options(ಶೂನ್ಯ)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ಯದಿ (set_listchars_option(curwin, p_lcs, FALSE, ರಿಕ್ತ, 0) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ e_conflicts_with_ಮೌಲ್ಯ_of_listchars;
    ಯದಿ (set_fillchars_option(curwin, p_fcs, FALSE, ರಿಕ್ತ, 0) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ e_conflicts_with_ಮೌಲ್ಯ_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಯದಿ (set_listchars_option(wp, wp->w_p_lcs, FALSE, ರಿಕ್ತ, 0) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ e_conflicts_with_ಮೌಲ್ಯ_of_listchars;
	ಯದಿ (set_fillchars_option(wp, wp->w_p_fcs, FALSE, ರಿಕ್ತ, 0) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ e_conflicts_with_ಮೌಲ್ಯ_of_fillchars;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
