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

#অন্তর্ভাব "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
স্থির পূর্ণাঙ্ক	screen_attr = 0;

স্থির শূন্য screen_char_2(unsigned off, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col);
স্থির পূর্ণাঙ্ক screenclear2(পূর্ণাঙ্ক doclear);
স্থির শূন্য lineclear(unsigned off, পূর্ণাঙ্ক width, পূর্ণাঙ্ক attr);
স্থির শূন্য lineinvalid(unsigned off, পূর্ণাঙ্ক width);
স্থির পূর্ণাঙ্ক win_do_সারিগুলি(win_T *wp, পূর্ণাঙ্ক row, পূর্ণাঙ্ক সারি_গুনতি, পূর্ণাঙ্ক mayclear, পূর্ণাঙ্ক del, পূর্ণাঙ্ক clear_attr);
স্থির শূন্য win_rest_invalid(win_T *wp);
স্থির শূন্য বার্তা_pos_mode(শূন্য);
স্থির শূন্য recording_mode(পূর্ণাঙ্ক attr);

// Ugly global: overrule attribute used by screen_char()
স্থির পূর্ণাঙ্ক screen_char_attr = 0;

#যদি defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    পূর্ণাঙ্ক
conceal_cursor_সারি(win_T *wp)
{
    পূর্ণাঙ্ক		c;

    যদি (*wp->w_p_cocu == NUL)
	প্রত্যাগম FALSE;
    যদি (get_real_state() & MODE_VISUAL)
	c = 'v';
    অন্যথা যদি (State & MODE_ঢোকাও)
	c = 'i';
    অন্যথা যদি (State & MODE_NORMAL)
	c = 'n';
    অন্যথা যদি (State & MODE_CMDLINE)
	c = 'c';
    অন্যথা
	প্রত্যাগম FALSE;
    প্রত্যাগম vim_strchr(wp->w_p_cocu, c) != রিক্ত;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    শূন্য
conceal_check_cursor_সারি(পূর্ণাঙ্ক was_concealed)
{
    যদি (curwin->w_p_cole <= 0 || conceal_cursor_সারি(curwin) == was_concealed)
	প্রত্যাগম;

    পূর্ণাঙ্ক wcol = curwin->w_wcol;

    need_cursor_সারি_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    যদি (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    পূর্ণাঙ্ক
get_win_attr(win_T *wp)
{
    পূর্ণাঙ্ক	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

    যদি (win_attr != 0)
    {
	যদি (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	অন্যথা
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    অন্যথা যদি (WIN_IS_POPUP(wp))
    {
	যদি (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	অন্যথা
	    win_attr = HL_ATTR(HLF_বের_করো);    // Popup
    }
#endif

    যদি (override_success)
	বের_করো_highlight_overrides();
    প্রত্যাগম win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    স্থির পূর্ণাঙ্ক
screen_fill_end(
	win_T *wp,
	পূর্ণাঙ্ক	c1,
	পূর্ণাঙ্ক	c2,
	পূর্ণাঙ্ক	off,
	পূর্ণাঙ্ক	width,
	পূর্ণাঙ্ক	row,
	পূর্ণাঙ্ক	endrow,
	পূর্ণাঙ্ক	attr)
{
    পূর্ণাঙ্ক	    nn = off + width;

    যদি (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    যদি (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (পূর্ণাঙ্ক)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    অন্যথা
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (পূর্ণাঙ্ক)wp->w_wincol + nn,
		c1, c2, attr);
    প্রত্যাগম nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    শূন্য
win_draw_end(
    win_T	*wp,
    পূর্ণাঙ্ক		c1,
    পূর্ণাঙ্ক		c2,
    পূর্ণাঙ্ক		draw_margin,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		endrow,
    hlf_T	hl)
{
    পূর্ণাঙ্ক		n = 0;
    পূর্ণাঙ্ক		attr = HL_ATTR(hl);
    পূর্ণাঙ্ক		win_attr = get_win_attr(wp);
    bool	override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    যদি (draw_margin)
    {
#ifdef FEAT_FOLDING
	পূর্ণাঙ্ক	fdc = compute_foldcolumn(wp, 0);

	যদি (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	যদি (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	যদি ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == রিক্ত)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, সংখ্যা_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    যদি (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    অন্যথা
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (পূর্ণাঙ্ক)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    যদি (override_success)
	বের_করো_highlight_overrides();
}

#যদি defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    পূর্ণাঙ্ক
compute_foldcolumn(win_T *wp, পূর্ণাঙ্ক col)
{
    পূর্ণাঙ্ক wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    পূর্ণাঙ্ক n = wp->w_width - (col + wmw);

    প্রত্যাগম MIN(wp->w_p_fdc, n);
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
    পূর্ণাঙ্ক		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    পূর্ণাঙ্ক		i = 0;
    পূর্ণাঙ্ক		level;
    পূর্ণাঙ্ক		first_level;
    পূর্ণাঙ্ক		empty;
    পূর্ণাঙ্ক		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    পূর্ণাঙ্ক		প্রতীক = 0;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    যদি (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    প্রতি (i = 0; i < n; i++)
    {
	যদি (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_নিচের_level)
	    প্রতীক = wp->w_fill_chars.foldopen;
	অন্যথা যদি (first_level == 1)
	    প্রতীক = wp->w_fill_chars.foldsep;
	অন্যথা যদি (wp->w_fill_chars.foldinner != NUL)
	    প্রতীক = wp->w_fill_chars.foldinner;
	অন্যথা যদি (first_level + i <= 9)
	    প্রতীক = '0' + first_level + i;
	অন্যথা
	    প্রতীক = '>';

	len = utf_char2bytes(প্রতীক, &p[byte_counter]);
	byte_counter += len;
	যদি (first_level + i >= level)
	{
	    i++;
	    ভঙ্গ;
	}
    }

    যদি (closed)
    {
	যদি (প্রতীক != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    যদি (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	প্রতীক = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(প্রতীক, &p[byte_counter]);
	byte_counter += len;
    }

    প্রত্যাগম MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    স্থির পূর্ণাঙ্ক
comp_char_differs(পূর্ণাঙ্ক off_from, পূর্ণাঙ্ক off_to)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < Screen_mco; ++i)
    {
	যদি (Screen_সারিগুলি_C[i][off_from] != Screen_সারিগুলি_C[i][off_to])
	    প্রত্যাগম TRUE;
	যদি (Screen_সারিগুলি_C[i][off_from] == 0)
	    ভঙ্গ;
    }
    প্রত্যাগম FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    স্থির পূর্ণাঙ্ক
char_needs_redraw(পূর্ণাঙ্ক off_from, পূর্ণাঙ্ক off_to, পূর্ণাঙ্ক cols)
{
    যদি (cols > 0
	    && ((Screen_সারিগুলি[off_from] != Screen_সারিগুলি[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_সারিগুলি[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_সারিগুলি[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_সারিগুলি[off_from + 1]
						 != Screen_সারিগুলি[off_to + 1])))
		|| (enc_utf8
		    && (Screen_সারিগুলি_UC[off_from] != Screen_সারিগুলি_UC[off_to]
			|| (Screen_সারিগুলি_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_সারিগুলি[off_from + 1]
						!= Screen_সারিগুলি[off_to + 1])))))
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

#যদি defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    পূর্ণাঙ্ক
screen_get_current_সারি_off(শূন্য)
{
    প্রত্যাগম (পূর্ণাঙ্ক)(current_Screen_সারি - Screen_সারিগুলি);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    স্থির পূর্ণাঙ্ক
blocked_by_popup(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    পূর্ণাঙ্ক off;

    যদি (!popup_visible)
	প্রত্যাগম FALSE;
    off = row * screen_Columns + col;
    প্রত্যাগম popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    শূন্য
reset_screen_attr(শূন্য)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    অন্যথা
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    স্থির পূর্ণাঙ্ক
skip_for_popup(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    যদি (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	প্রত্যাগম TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    যদি (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	প্রত্যাগম TRUE;
#অন্যথা
    যদি (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	প্রত্যাগম TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    যদি (blocked_by_popup(row, col))
	প্রত্যাগম TRUE;
#endif
    প্রত্যাগম FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
স্থির char_u	*suppressed_cells = রিক্ত;
স্থির পূর্ণাঙ্ক	suppressed_rows = 0;
স্থির পূর্ণাঙ্ক	suppressed_cols = 0;

    স্থির শূন্য
mark_suppressed_cell(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (suppressed_cells == রিক্ত
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	যদি (suppressed_cells == রিক্ত)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    প্রত্যাগম;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    স্থির শূন্য
unmark_suppressed_cell(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    যদি (suppressed_cells != রিক্ত
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    স্থির পূর্ণাঙ্ক
is_suppressed_cell(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    প্রত্যাগম suppressed_cells != রিক্ত
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
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		popup_attr;	// get_win_attr(screen_opacity_popup)
    পূর্ণাঙ্ক		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    স্থির অন্তঃস্থ শূন্য
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    স্থির অন্তঃস্থ পূর্ণাঙ্ক
popup_base_attr_or(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক fallback)
{
    পূর্ণাঙ্ক		base = fallback;

    popup_get_base_screen_cell(row, col, রিক্ত, &base, রিক্ত);
    প্রত্যাগম base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    স্থির অন্তঃস্থ পূর্ণাঙ্ক
popup_blend_with_base(
	ধ্রুব popup_opacity_T	*po,
	পূর্ণাঙ্ক			char_attr,
	পূর্ণাঙ্ক			base_attr,
	পূর্ণাঙ্ক			blend_fg)
{
    পূর্ণাঙ্ক		combined = hl_combine_attr(po->popup_attr, char_attr);

    প্রত্যাগম hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    স্থির শূন্য
resolve_wide_char_opacity_attrs(
	পূর্ণাঙ্ক row, পূর্ণাঙ্ক col1, পূর্ণাঙ্ক col2,
	sattr_T *attr1, sattr_T *attr2)
{
    পূর্ণাঙ্ক		bg1, bg2;
    পূর্ণাঙ্ক		base1 = 0;
    পূর্ণাঙ্ক		base2 = 0;
    attrentry_T	*ae;

    যদি (*attr1 == *attr2)
	প্রত্যাগম;

    popup_get_base_screen_cell(row, col1, রিক্ত, &base1, রিক্ত);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != রিক্ত && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# অন্যথা
    bg1 = (ae != রিক্ত && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, রিক্ত, &base2, রিক্ত);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != রিক্ত && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# অন্যথা
    bg2 = (ae != রিক্ত && ae->ae_u.cterm.bg_color != 0);
# endif

    যদি (bg1 && !bg2)
	*attr1 = *attr2;
    অন্যথা যদি (!bg1 && bg2)
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
    শূন্য
screen_সারি(
	win_T	*wp,
	পূর্ণাঙ্ক	row,
	পূর্ণাঙ্ক	coloff,
	পূর্ণাঙ্ক	endcol,
	পূর্ণাঙ্ক	clear_width,
	colnr_T	last_vcol,
	পূর্ণাঙ্ক	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    পূর্ণাঙ্ক		    col = 0;
    পূর্ণাঙ্ক		    hl;
    পূর্ণাঙ্ক		    force = FALSE;	// force update rest of the line
    পূর্ণাঙ্ক		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    পূর্ণাঙ্ক		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    পূর্ণাঙ্ক		    changed_this;	// TRUE if character changed
    পূর্ণাঙ্ক		    changed_next;	// TRUE if next character changed
#endif
    পূর্ণাঙ্ক		    clear_next = FALSE;
    পূর্ণাঙ্ক		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    পূর্ণাঙ্ক		    drawing_opacity_popup =
				screen_opacity_popup != রিক্ত
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    যদি (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    যদি (row >= Rows)
	row = Rows - 1;
    যদি (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_Screen_সারি - Screen_সারিগুলি);
    off_to = সারি_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = সারি_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    যদি (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	যদি (clear_width > 0)
	{
	    পূর্ণাঙ্ক clear_start = col;

	    যাবত্ (col <= endcol && Screen_সারিগুলি[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_সারিগুলি_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    যদি (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    প্রতি (পূর্ণাঙ্ক i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = সারি_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    যদি (coloff > 0 && enc_utf8
		   && Screen_সারিগুলি[off_to] == 0
		   && Screen_সারিগুলি_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_সারিগুলি_UC[off_to - 1]) > 1)
    {
	Screen_সারিগুলি[off_to - 1] = ' ';
	Screen_সারিগুলি_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	যদি (screen_opacity_popup == রিক্ত)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    যাবত্ (col < endcol)
    {
	যদি (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	অন্যথা
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	যদি (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	যদি (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    যদি (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    যদি (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	যদি (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	যদি (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	পূর্ণাঙ্ক opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	যদি (drawing_opacity_popup
		&& Screen_সারিগুলি[off_from] == ' '
		&& (!enc_utf8 || Screen_সারিগুলি_UC[off_from] == 0))
	{
	    পূর্ণাঙ্ক popup_src_attr = ScreenAttrs[off_from];
	    পূর্ণাঙ্ক bg_is_empty = Screen_সারিগুলি[off_to] == 0
				&& (!enc_utf8 || Screen_সারিগুলি_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    যদি (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_সারিগুলি_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_সারিগুলি_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    অন্যথা যদি (Screen_সারিগুলি[off_to] != 0
		    || (enc_utf8 && Screen_সারিগুলি_UC[off_to] != 0))
	    {
		পূর্ণাঙ্ক bg_char_cells = (enc_utf8 && Screen_সারিগুলি_UC[off_to] != 0)
				? utf_char2cells(Screen_সারিগুলি_UC[off_to]) : 1;

		যদি (bg_char_cells == 2)
		{
		    যদি (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			পূর্ণাঙ্ক base_attr;
			Screen_সারিগুলি[off_to] = ' ';
			যদি (enc_utf8)
			    Screen_সারিগুলি_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			গচ্ছ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    যদি (!(Screen_সারিগুলি[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_সারিগুলি_UC[off_from + 1] == 0)))
			গচ্ছ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		পূর্ণাঙ্ক base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		যদি (bg_char_cells == 2)
		{
		    পূর্ণাঙ্ক base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    যদি (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    অন্যথা যদি (bg_is_empty)
	    {
		পূর্ণাঙ্ক base_attr;
		Screen_সারিগুলি[off_to] = ' ';
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

	যদি (redraw_this)
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
	    যদি (       p_wiv
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
		যদি (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		অন্যথা
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    যদি (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		যদি (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_সারিগুলি[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		অন্যথা যদি (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_সারিগুলি[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		যদি (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    যদি (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_সারিগুলি[off_to] = Screen_সারিগুলি[off_from];
	    যদি (enc_utf8)
	    {
		Screen_সারিগুলি_UC[off_to] = Screen_সারিগুলি_UC[off_from];
		যদি (Screen_সারিগুলি_UC[off_from] != 0)
		{
		    পূর্ণাঙ্ক	    i;

		    প্রতি (i = 0; i < Screen_mco; ++i)
			Screen_সারিগুলি_C[i][off_to] = Screen_সারিগুলি_C[i][off_from];
		}
	    }
	    যদি (char_cells == 2)
	    {
		Screen_সারিগুলি[off_to + 1] = Screen_সারিগুলি[off_from + 1];
		যদি (enc_utf8)
		    Screen_সারিগুলি_UC[off_to + 1] = 0;
	    }

#যদি defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    যদি (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# যদি defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		যদি (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		যদি (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    যদি (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    যদি (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    যদি (drawing_opacity_popup && po.blend > 0)
	    {
		পূর্ণাঙ্ক popup_src_attr = ScreenAttrs[off_from];
		পূর্ণাঙ্ক scol1 = col + coloff;
		পূর্ণাঙ্ক base1 = popup_base_attr_or(row, scol1, 0);

		যদি (char_cells == 2)
		{
		    পূর্ণাঙ্ক base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    পূর্ণাঙ্ক over1 = popup_is_over_opacity(row, scol1);
		    পূর্ণাঙ্ক over2 = popup_is_over_opacity(row, scol1 + 1);
		    যদি (over1 != over2)
		    {
			যদি (over1)
			    base1 = base2;
			অন্যথা
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    অন্যথা
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    যদি (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    যদি (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    অন্যথা
		screen_char(off_to, row, col + coloff);
	}
	অন্যথা যদি (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    যদি (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    অন্যথা যদি (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	যদি (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    যদি (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    যদি (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_সারিগুলি[off_to] = ' ';
	যদি (enc_utf8)
	    Screen_সারিগুলি_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	যদি (screen_opacity_popup == রিক্ত)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    যদি (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	পূর্ণাঙ্ক startCol = col;
#endif

	// blank out the rest of the line
	যাবত্ (col < clear_width && Screen_সারিগুলি[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_সারিগুলি_UC[off_to] == 0)
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
	যদি (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    যদি (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		যদি (hl > HL_ALL || (hl & HL_BOLD))
		{
		    পূর্ণাঙ্ক prev_cells = 1;

		    যদি (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_সারিগুলি[off_to - 1] == 0 ? 2 : 1;
		    অন্যথা যদি (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = সারি_Offset[row];
			unsigned max_off = সারি_Offset[row] + screen_Columns;

			যাবত্ (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    যদি (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			যদি (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			অন্যথা
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    যাবত্ (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    যদি (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	যদি (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    যদি (!skip_for_popup(row, col + coloff))
	    {
		পূর্ণাঙ্ক c;

		c = fillchar_vsep(&hl, wp, row);
		যদি (Screen_সারিগুলি[off_to] != (schar_T)c
			|| (enc_utf8 && (পূর্ণাঙ্ক)Screen_সারিগুলি_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_সারিগুলি[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    যদি (enc_utf8)
		    {
			যদি (c >= 0x80)
			{
			    Screen_সারিগুলি_UC[off_to] = c;
			    Screen_সারিগুলি_C[0][off_to] = 0;
			    Screen_সারিগুলি[off_to] = 0x80; // avoid storing zero
			}
			অন্যথা
			    Screen_সারিগুলি_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	অন্যথা
	    সারি_Wraps[row] = FALSE;
    }

    যদি (override_success)
	বের_করো_highlight_overrides();
}

#যদি defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    শূন্য
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    পূর্ণাঙ্ক		t;

    প্রতি (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
    শূন্য
draw_vsep_win(win_T *wp, পূর্ণাঙ্ক row)
{
    যদি (!wp->w_vsep_width)
	প্রত্যাগম;

    পূর্ণাঙ্ক content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    পূর্ণাঙ্ক start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    প্রতি (পূর্ণাঙ্ক r = start_row; r < content_end; ++r)
    {
	পূর্ণাঙ্ক hl;
	পূর্ণাঙ্ক c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    যদি (wp->w_status_height != 0)
    {
	পূর্ণাঙ্ক hl;
	পূর্ণাঙ্ক c;
	যদি (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	অন্যথা
	    c = fillchar_vsep(&hl, wp, content_end);
	প্রতি (পূর্ণাঙ্ক r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    পূর্ণাঙ্ক
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    যাবত্ (fr->fr_parent != রিক্ত)
    {
	যদি (fr->fr_parent->fr_layout == FR_COL)
	{
	    যদি (fr->fr_next != রিক্ত)
		ভঙ্গ;
	}
	অন্যথা
	{
	    যদি (fr->fr_next != রিক্ত)
		প্রত্যাগম TRUE;
	}
	fr = fr->fr_parent;
    }
    প্রত্যাগম FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    পূর্ণাঙ্ক
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    পূর্ণাঙ্ক		len)	    // length of buffer
{
    char_u	*p;
    পূর্ণাঙ্ক		plen;

    যদি (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	প্রত্যাগম 0;

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
    যদি (p == রিক্ত || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	যদি (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	অন্যথা
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((বর্ণ *)buf, len, (বর্ণ *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    যদি (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    প্রত্যাগম plen;
}

#যদি defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    শূন্য
win_redr_custom(
    win_T	*wp,
    পূর্ণাঙ্ক		draw_ruler)	// TRUE or FALSE
{
    স্থির পূর্ণাঙ্ক	entered = FALSE;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		curattr;
    পূর্ণাঙ্ক		row;
    পূর্ণাঙ্ক		col = 0;
    পূর্ণাঙ্ক		maxwidth;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		stlh_cnt;
    পূর্ণাঙ্ক		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    পূর্ণাঙ্ক		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    পূর্ণাঙ্ক		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    যদি (entered)
	প্রত্যাগম;
    entered = TRUE;

    // setup environment for the task at hand
    যদি (wp == রিক্ত)
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
    অন্যথা
    {
	override_success = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	পূর্ণাঙ্ক in_status_সারি = wp->w_status_height != 0;
	maxwidth = in_status_সারি ? wp->w_width : cmdline_width;

	যদি (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    যদি (*stl == '%')
	    {
		যদি (*++stl == '-')
		    stl++;
		যদি (atoi((বর্ণ *)stl))
		    যাবত্ (VIM_ISDIGIT(*stl))
			stl++;
		যদি (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    যদি (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    যদি (!in_status_সারি)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	অন্যথা
	{
	    opt_name = (char_u *)"statusline";
	    যদি (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    অন্যথা
		stl = p_stl;
	}

	যদি (in_status_সারি)
	    col += wp->w_wincol;
	অন্যথা
	    col += cmdline_col_off;
    }

    যদি (maxwidth <= 0)
	গচ্ছ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == রিক্ত ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == রিক্ত) ? (char_u *)"" : stl;
    পূর্ণাঙ্ক col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = রিক্ত;
    পূর্ণাঙ্ক			*out_গুনতি = রিক্ত;
    পূর্ণাঙ্ক			region_base_col = 0;
    যদি (!draw_ruler)
    {
	যদি (wp != রিক্ত)
	{
	    out_regions = &wp->w_stl_click;
	    out_গুনতি = &wp->w_stl_click_গুনতি;
	    region_base_col = wp->w_wincol;
	}
	অন্যথা
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_গুনতি = &tabline_stl_click_গুনতি;
	    region_base_col = firstwin->w_wincol;
	}

	প্রতি (n = 0; n < *out_গুনতি; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_গুনতি = 0;
    }

    পূর্ণাঙ্ক carry_hl = 0;
    প্রতি (পূর্ণাঙ্ক i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, পরিমাণ(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	যদি (p != রিক্ত)
	{
	    len = vim_snprintf((বর্ণ *)buf, পরিমাণ(buf), "%s", p);
	    vim_free(p);
	}
	অন্যথা
	    len = (পূর্ণাঙ্ক)STRLEN(buf);

	// fill up with "fillchar"
	যাবত্ (width < maxwidth && len < (পূর্ণাঙ্ক)পরিমাণ(buf) - 1)
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
	প্রতি (n = 0; hltab[n].start != রিক্ত; n++)
	{
	    len = (পূর্ণাঙ্ক)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    যদি (hltab[n].userhl == 0)
		curattr = attr;
	    অন্যথা যদি (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    অন্যথা যদি (wp != রিক্ত && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    অন্যথা যদি (wp != রিক্ত && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    অন্যথা যদি (wp != রিক্ত && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    অন্যথা
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	যদি (out_regions != রিক্ত)
	{
	    পূর্ণাঙ্ক click_গুনতি = 0;
	    প্রতি (n = 0; clicktab[n].start != রিক্ত; n++)
		click_গুনতি++;

	    যদি (click_গুনতি > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			পরিমাণ(stl_click_region_T)
				    * (*out_গুনতি + click_গুনতি));
		যদি (new_arr != রিক্ত)
		{
		    char_u  *cur_funcname = রিক্ত;
		    পূর্ণাঙ্ক	    cur_minwid = 0;
		    পূর্ণাঙ্ক	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    প্রতি (n = 0; clicktab[n].start != রিক্ত; n++)
		    {
			len += vim_strnsize(p,
					    (পূর্ণাঙ্ক)(clicktab[n].start - p));
			p = clicktab[n].start;

			যদি (cur_funcname != রিক্ত)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_গুনতি];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_গুনতি)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    যদি (cur_funcname != রিক্ত)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_গুনতি];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_গুনতি)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	প্রতি (n = 0; clicktab[n].start != রিক্ত; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    যদি (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    যদি (stl != রিক্ত)
	vim_free(stl_tmp);

    যদি (wp == রিক্ত)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	প্রতি (n = 0; tabtab[n].start != রিক্ত; n++)
	{
	    len += vim_strnsize(p, (পূর্ণাঙ্ক)(tabtab[n].start - p));
	    যাবত্ (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	যাবত্ (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    যদি (override_success)
	বের_করো_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    শূন্য
screen_putchar(পূর্ণাঙ্ক c, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    যদি (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    অন্যথা
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
    শূন্য
screen_getbytes(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, char_u *bytes, পূর্ণাঙ্ক *attrp)
{
    unsigned off;

    // safety check
    যদি (Screen_সারিগুলি == রিক্ত || row >= screen_Rows || col >= screen_Columns)
	প্রত্যাগম;

    off = সারি_Offset[row] + col;
    যদি (attrp != রিক্ত)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_সারিগুলি[off];
    bytes[1] = NUL;

    যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    অন্যথা যদি (enc_dbcs == DBCS_JPNU && Screen_সারিগুলি[off] == 0x8e)
    {
	bytes[0] = Screen_সারিগুলি[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    অন্যথা যদি (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_সারিগুলি[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    স্থির পূর্ণাঙ্ক
screen_comp_differs(পূর্ণাঙ্ক off, পূর্ণাঙ্ক *u8cc)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < Screen_mco; ++i)
    {
	যদি (Screen_সারিগুলি_C[i][off] != (u8char_T)u8cc[i])
	    প্রত্যাগম TRUE;
	যদি (u8cc[i] == 0)
	    ভঙ্গ;
    }
    প্রত্যাগম FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    শূন্য
screen_puts(
    char_u	*text,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    শূন্য
screen_puts_len(
    char_u	*text,
    পূর্ণাঙ্ক		textlen,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		attr_arg)
{
    পূর্ণাঙ্ক		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    পূর্ণাঙ্ক		len = textlen;
    পূর্ণাঙ্ক		c;
    unsigned	max_off;
    পূর্ণাঙ্ক		mbyte_blen = 1;
    পূর্ণাঙ্ক		mbyte_cells = 1;
    পূর্ণাঙ্ক		u8c = 0;
    পূর্ণাঙ্ক		u8cc[MAX_MCO];
    পূর্ণাঙ্ক		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    পূর্ণাঙ্ক		prev_c = 0;		// previous Arabic character
    পূর্ণাঙ্ক		pc, nc, nc1;
    পূর্ণাঙ্ক		pcc[MAX_MCO];
#endif
    পূর্ণাঙ্ক		force_redraw_this;
    পূর্ণাঙ্ক		force_redraw_next = FALSE;
    পূর্ণাঙ্ক		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    যদি (Screen_সারিগুলি == রিক্ত
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	প্রত্যাগম;
    off = সারি_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    যদি (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	যদি (!skip_for_popup(row, col - 1))
	{
	    Screen_সারিগুলি[off - 1] = ' ';
	    যদি (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    অন্যথা যদি (enc_utf8)
	    {
		Screen_সারিগুলি_UC[off - 1] = 0;
		Screen_সারিগুলি_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = সারি_Offset[row] + screen_Columns;
    যাবত্ (col < screen_Columns
	    && (len < 0 || (পূর্ণাঙ্ক)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	যদি (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (পূর্ণাঙ্ক)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    যদি (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    অন্যথা যদি (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    অন্যথা	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (পূর্ণাঙ্ক)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		যদি (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    যদি (len >= 0 && (পূর্ণাঙ্ক)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    অন্যথা
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(পূর্ণাঙ্ক)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		অন্যথা
		    prev_c = u8c;
#endif
		যদি (col + mbyte_cells > screen_Columns)
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
	পূর্ণাঙ্ক cell_attr = attr;
	যদি (screen_pum_blend > 0 && pum_bg_attrs != রিক্ত
		&& row >= pum_bg_শীর্ষ && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    পূর্ণাঙ্ক soff = (row - pum_bg_শীর্ষ) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_সারিগুলি[off] != c
		|| (mbyte_cells == 2
		    && Screen_সারিগুলি[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_সারিগুলি_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_সারিগুলি_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	যদি ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#যদি defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    যদি (need_redraw && Screen_সারিগুলি[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# যদি defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		পূর্ণাঙ্ক	n = ScreenAttrs[off];

		যদি (n > HL_ALL)
		    n = syn_attr2attr(n);
		যদি (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    যদি (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    যদি (clear_next_cell)
		clear_next_cell = FALSE;
	    অন্যথা যদি (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    যদি (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_সারিগুলি[off + mbyte_blen] = 0;
	    Screen_সারিগুলি[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    যদি (enc_utf8)
	    {
		যদি (c < 0x80 && u8cc[0] == 0)
		    Screen_সারিগুলি_UC[off] = 0;
		অন্যথা
		{
		    পূর্ণাঙ্ক	    i;

		    Screen_সারিগুলি_UC[off] = u8c;
		    প্রতি (i = 0; i < Screen_mco; ++i)
		    {
			Screen_সারিগুলি_C[i][off] = u8cc[i];
			যদি (u8cc[i] == 0)
			    ভঙ্গ;
		    }
		}
		যদি (mbyte_cells == 2)
		{
		    Screen_সারিগুলি[off + 1] = 0;
		    Screen_সারিগুলি_UC[off + 1] = 0;
		    প্রতি (পূর্ণাঙ্ক ci = 0; ci < Screen_mco; ++ci)
			Screen_সারিগুলি_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    অন্যথা যদি (mbyte_cells == 2)
	    {
		Screen_সারিগুলি[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    অন্যথা যদি (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    অন্যথা
		screen_char(off, row, col);
	}
	যদি (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    যদি (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	অন্যথা
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    যদি (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	যদি (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	অন্যথা
	    screen_char(off, row, col);
    }
}

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    শূন্য
start_অনুসন্ধান_hl(শূন্য)
{
    যদি (!p_hls || no_hlsearch)
	প্রত্যাগম;

    end_অনুসন্ধান_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_অনুসন্ধান_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    শূন্য
end_অনুসন্ধান_hl(শূন্য)
{
    যদি (screen_অনুসন্ধান_hl.rm.regprog == রিক্ত)
	প্রত্যাগম;

    vim_regfree(screen_অনুসন্ধান_hl.rm.regprog);
    screen_অনুসন্ধান_hl.rm.regprog = রিক্ত;
}
#endif

    স্থির শূন্য
screen_start_highlight(পূর্ণাঙ্ক attr)
{
    attrentry_T *aep = রিক্ত;

    screen_attr = attr;
    যদি (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	প্রত্যাগম;

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	বর্ণ	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	প্রত্যাগম;
    }
#endif

    যদি (attr > HL_ALL)				// special HL attr.
    {
	যদি (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	অন্যথা
	    aep = syn_term_attr2entry(attr);
	যদি (aep == রিক্ত)	    // did ":syntax clear"
	    attr = 0;
	অন্যথা
	    attr = aep->ae_attr;
    }
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    যদি (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	পূর্ণাঙ্ক	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	যদি (p_tgc)
	{
	    যদি (aep == রিক্ত || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    যদি (aep == রিক্ত || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	অন্যথা যদি (t_colors >= 256)
	{
	    যদি (aep == রিক্ত || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    যদি (aep == রিক্ত || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    যদি ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    অন্যথা যদি (aep != রিক্ত && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    যদি ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    যদি ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    যদি ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    যদি ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    যদি ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    যদি (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    যদি ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    যদি ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    যদি ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    যদি (aep != রিক্ত)
    {
	যদি (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	যদি (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    যদি (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	অন্যথা
#endif
	    যদি (t_colors > 1)
	    {
		যদি (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	যদি (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    যদি (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	অন্যথা
#endif
	    যদি (t_colors > 1)
	    {
		যদি (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	যদি (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    যদি (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	অন্যথা
#endif
	    যদি (t_colors > 1)
	    {
		যদি (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	যদি (!IS_CTERM)
	{
	    যদি (aep->ae_u.term.start != রিক্ত)
		out_str(aep->ae_u.term.start);
	}
    }
}

    শূন্য
screen_stop_highlight(শূন্য)
{
    পূর্ণাঙ্ক	    do_ME = FALSE;	    // output T_ME code
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    পূর্ণাঙ্ক	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    যদি (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	যদি (gui.in_use)
	{
	    বর্ণ	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	অন্যথা
#endif
	{
	    পূর্ণাঙ্ক is_under;

	    যদি (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		যদি (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    যদি (aep != রিক্ত && ((
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
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    যদি (use_vtp())
		    {
			যদি (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			যদি (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			যদি (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    অন্যথা
		    {
			// Process FG and BG at once.
			যদি (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		অন্যথা
		{
		    aep = syn_term_attr2entry(screen_attr);
		    যদি (aep != রিক্ত && aep->ae_u.term.stop != রিক্ত)
		    {
			যদি (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			অন্যথা
			    out_str(aep->ae_u.term.stop);
		    }
		}
		যদি (aep == রিক্ত)	    // did ":syntax clear"
		    screen_attr = 0;
		অন্যথা
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    যদি (screen_attr & HL_STANDOUT)
	    {
		যদি (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		অন্যথা
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    যদি (is_under && *T_UCE != NUL)
	    {
		যদি (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		অন্যথা
		    out_str(T_UCE);
	    }
	    যদি ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		যদি (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		অন্যথা
		    out_str(T_UE);
	    }
	    যদি (screen_attr & HL_ITALIC)
	    {
		যদি (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		অন্যথা
		    out_str(T_CZR);
	    }
	    যদি (screen_attr & HL_STRIKETHROUGH)
	    {
		যদি (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		অন্যথা
		    out_str(T_STE);
	    }
	    যদি (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    যদি (p_tgc)
	    {
		যদি (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		যদি (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		যদি (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    অন্যথা
#endif
	    {
		যদি (t_colors > 1)
		{
		    // set Normal cterm colors
		    যদি (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    যদি (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    যদি (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    যদি (cterm_normal_fg_bold)
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
    শূন্য
reset_cterm_colors(শূন্য)
{
    যদি (!IS_CTERM)
	প্রত্যাগম;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    যদি (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#অন্যথা
	যদি (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    যদি (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    শূন্য
screen_char(unsigned off, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    পূর্ণাঙ্ক		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    যদি (row >= screen_Rows || col >= screen_Columns)
	প্রত্যাগম;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    যদি (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0
		&& utf_char2cells(Screen_সারিগুলি_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    পূর্ণাঙ্ক off2 = off + 1;
	    Screen_সারিগুলি[off2] = ' ';
	    Screen_সারিগুলি_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0
		&& utf_char2cells(Screen_সারিগুলি_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	প্রত্যাগম;
    }
    যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0
	    && utf_char2cells(Screen_সারিগুলি_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	প্রত্যাগম;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    যদি (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	প্রত্যাগম;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    যদি (screen_char_attr != 0)
	attr = screen_char_attr;
    অন্যথা
	attr = ScreenAttrs[off];
    যদি (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0
	    && utf_char2cells(Screen_সারিগুলি_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    যদি (screen_attr != attr)
	screen_start_highlight(attr);

    যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	যদি (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_সারিগুলি_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	অন্যথা যদি (utf_ambiguous_width(Screen_সারিগুলি_UC[off]))
	{
	    যদি (*p_ambw == 'd'
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
	অন্যথা যদি (utf_char2cells(Screen_সারিগুলি_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    অন্যথা
    {
	out_flush_check();
	out_char(Screen_সারিগুলি[off]);
	// double-byte character in single-width cell
	যদি (enc_dbcs == DBCS_JPNU && Screen_সারিগুলি[off] == 0x8e)
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
    স্থির শূন্য
screen_char_2(unsigned off, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    // Check for illegal values (could be wrong when screen was resized).
    যদি (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	প্রত্যাগম;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    যদি (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	প্রত্যাগম;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    যদি (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	প্রত্যাগম;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_সারিগুলি[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    শূন্য
screen_draw_rectangle(
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		height,
    পূর্ণাঙ্ক		width,
    পূর্ণাঙ্ক		invert,
    পূর্ণাঙ্ক		force)
{
    পূর্ণাঙ্ক		r, c;
    পূর্ণাঙ্ক		off;
    পূর্ণাঙ্ক		max_off;

    // Can't use ScreenLines unless initialized
    যদি (Screen_সারিগুলি == রিক্ত)
	প্রত্যাগম;

    যদি (invert)
	screen_char_attr = HL_INVERSE;
    প্রতি (r = row; r < row + height; ++r)
    {
	off = সারি_Offset[r];
	max_off = off + screen_Columns;
	প্রতি (c = col; c < col + width; ++c)
	{
	    যদি (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		যদি (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    অন্যথা
	    {
		যদি (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		যদি (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    স্থির শূন্য
redraw_block(পূর্ণাঙ্ক row, পূর্ণাঙ্ক end, win_T *wp)
{
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    যদি (wp == রিক্ত)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    অন্যথা
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    শূন্য
space_to_screenline(পূর্ণাঙ্ক off, পূর্ণাঙ্ক attr)
{
    Screen_সারিগুলি[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    যদি (enc_utf8)
	Screen_সারিগুলি_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    শূন্য
screen_fill(
	পূর্ণাঙ্ক	start_row,
	পূর্ণাঙ্ক	end_row,
	পূর্ণাঙ্ক	start_col,
	পূর্ণাঙ্ক	end_col,
	পূর্ণাঙ্ক	c1,
	পূর্ণাঙ্ক	c2,
	পূর্ণাঙ্ক	attr)
{
    পূর্ণাঙ্ক	    row;
    পূর্ণাঙ্ক	    col;
    পূর্ণাঙ্ক	    off;
    পূর্ণাঙ্ক	    end_off;
    পূর্ণাঙ্ক	    did_delete;
    পূর্ণাঙ্ক	    c;
    পূর্ণাঙ্ক	    norm_term;
#যদি defined(FEAT_GUI) || defined(UNIX)
    পূর্ণাঙ্ক	    force_next = FALSE;
#endif

    যদি (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    যদি (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    যদি (Screen_সারিগুলি == রিক্ত
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	প্রত্যাগম;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    প্রতি (row = start_row; row < end_row; ++row)
    {
	যদি (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    যদি (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		পূর্ণাঙ্ক বাম_attr = ScreenAttrs[সারি_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, বাম_attr);
	    }
	    যদি (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		পূর্ণাঙ্ক ডান_attr = ScreenAttrs[সারি_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, ডান_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	যদি (c2 == ' '
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
	    যদি (c1 != ' ')			// don't clear first char
		++col;

	    off = সারি_Offset[row] + col;
	    end_off = সারি_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    যদি (enc_utf8)
		যাবত্ (off < end_off && Screen_সারিগুলি[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_সারিগুলি_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (পূর্ণাঙ্ক)(off - সারি_Offset[row]))
#endif
			  )
		    ++off;
	    অন্যথা
		যাবত্ (off < end_off && Screen_সারিগুলি[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (পূর্ণাঙ্ক)(off - সারি_Offset[row]))
#endif
			  )
		    ++off;
	    যদি (off < end_off)		// something to be cleared
	    {
		col = off - সারি_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		যাবত্ (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (পূর্ণাঙ্ক)(off - সারি_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = সারি_Offset[row] + start_col;
	c = c1;
	প্রতি (col = start_col; col < end_col; ++col)
	{
	    যদি ((Screen_সারিগুলি[off] != c
		    || (enc_utf8 && (পূর্ণাঙ্ক)Screen_সারিগুলি_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#যদি defined(FEAT_GUI) || defined(UNIX)
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
		যদি (screen_opacity_popup != রিক্ত && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    যদি (Screen_সারিগুলি[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_সারিগুলি_UC[off - 1] != 0
			    && utf_char2cells(Screen_সারিগুলি_UC[off - 1]) == 2)
			গচ্ছ next_col;
		}
		যদি (screen_opacity_popup != রিক্ত && c == ' '
						       && Screen_সারিগুলি[off] != 0)
		{
		    পূর্ণাঙ্ক bg_char_cells = 1;
		    যদি (enc_utf8 && Screen_সারিগুলি_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_সারিগুলি_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    যদি (bg_char_cells == 2)
		    {
			যদি (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    গচ্ছ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			যদি (c2 != ' ')
			    গচ্ছ skip_opacity_fill;
		    }

		    পূর্ণাঙ্ক popup_attr = get_win_attr(screen_opacity_popup);
		    পূর্ণাঙ্ক blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    গচ্ছ next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		যদি (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != রিক্ত
			&& row >= pum_bg_শীর্ষ && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    পূর্ণাঙ্ক soff = (row - pum_bg_শীর্ষ) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    যদি (enc_utf8 && pum_bg_সারিগুলি_UC != রিক্ত
			    && col > start_col
			    && pum_bg_সারিগুলি_UC[soff] == 0
			    && pum_bg_সারিগুলি_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_সারিগুলি_UC[soff - 1]) == 2)
		    {
			Screen_সারিগুলি[off] = 0;
			যদি (Screen_সারিগুলি_UC != রিক্ত)
			    Screen_সারিগুলি_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			গচ্ছ next_col;
		    }

		    পূর্ণাঙ্ক underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    যদি (enc_utf8 && pum_bg_সারিগুলি_UC != রিক্ত
			    && ((pum_bg_সারিগুলি_UC[soff] != 0
				    && utf_char2cells(pum_bg_সারিগুলি_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_সারিগুলি_UC[soff] == 0
				    && col > 0
				    && pum_bg_সারিগুলি_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_সারিগুলি_UC[soff - 1])
								       == 2)))
		    {
			Screen_সারিগুলি[off] = ' ';
			যদি (Screen_সারিগুলি_UC != রিক্ত)
			    Screen_সারিগুলি_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			গচ্ছ next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_সারিগুলি[off] = pum_bg_সারিগুলি[soff];
		    যদি (enc_utf8 && pum_bg_সারিগুলি_UC != রিক্ত
						&& Screen_সারিগুলি_UC != রিক্ত)
		    {
			পূর্ণাঙ্ক k;
			Screen_সারিগুলি_UC[off] = pum_bg_সারিগুলি_UC[soff];
			প্রতি (k = 0; k < MAX_MCO; ++k)
			    যদি (pum_bg_সারিগুলি_C[k] != রিক্ত
						    && Screen_সারিগুলি_C[k] != রিক্ত)
				Screen_সারিগুলি_C[k][off] = pum_bg_সারিগুলি_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    গচ্ছ next_col;
		}
#যদি defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		যদি (
# ifdef FEAT_GUI
			gui.in_use
# endif
# যদি defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    যদি (Screen_সারিগুলি[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    অন্যথা
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		যদি (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_সারিগুলি[off] = c;
		যদি (enc_utf8)
		{
		    যদি (c >= 0x80)
		    {
			Screen_সারিগুলি_UC[off] = c;
			Screen_সারিগুলি_C[0][off] = 0;
			Screen_সারিগুলি[off] = 0x80; // avoid storing zero
		    }
		    অন্যথা
			Screen_সারিগুলি_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		যদি (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    যদি (col == start_col)
	    {
		যদি (did_delete)
		    ভঙ্গ;
		c = c2;
	    }
	}
	যদি (end_col == Columns)
	    সারি_Wraps[row] = FALSE;
	যদি (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    যদি (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    যদি (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    শূন্য
check_for_delay(পূর্ণাঙ্ক check_বার্তা_scroll)
{
    যদি ((emsg_on_display || (check_বার্তা_scroll && বার্তা_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	যদি (check_বার্তা_scroll)
	    বার্তা_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    স্থির শূন্য
clear_TabPageIdxs(শূন্য)
{
    পূর্ণাঙ্ক		scol;

    প্রতি (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    পূর্ণাঙ্ক
screen_valid(পূর্ণাঙ্ক doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    প্রত্যাগম (Screen_সারিগুলি != রিক্ত);
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
    শূন্য
screenalloc(পূর্ণাঙ্ক doclear)
{
    পূর্ণাঙ্ক		    new_row, old_row;
#ifdef FEAT_GUI
    পূর্ণাঙ্ক		    old_Rows;
#endif
    win_T	    *wp;
    পূর্ণাঙ্ক		    outofmem = FALSE;
    পূর্ণাঙ্ক		    len;
    schar_T	    *new_Screen_সারিগুলি;
    u8char_T	    *new_Screen_সারিগুলি_UC = রিক্ত;
    u8char_T	    *new_Screen_সারিগুলি_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = রিক্ত;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_সারি_Offset;
    char_u	    *new_সারি_Wraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    বর্ণ	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    স্থির পূর্ণাঙ্ক	    entered = FALSE;		// avoid recursiveness
    স্থির পূর্ণাঙ্ক	    done_outofmem_বার্তা = FALSE;	// did outofmem message
    পূর্ণাঙ্ক		    retry_গুনতি = 0;
    পূর্ণাঙ্ক		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    যদি ((Screen_সারিগুলি != রিক্ত
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_সারিগুলি_UC != রিক্ত)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != রিক্ত)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_সারিগুলি == রিক্ত))
	প্রত্যাগম;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    যদি (entered)
	প্রত্যাগম;
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
    প্রতি (পূর্ণাঙ্ক i = 0; i < AUCMD_WIN_গুনতি; ++i)
	যদি (aucmd_win[i].auc_win != রিক্ত)
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

    new_Screen_সারিগুলি = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_সারিগুলি_C, 0, পরিমাণ(u8char_T *) * MAX_MCO);
    যদি (enc_utf8)
    {
	new_Screen_সারিগুলি_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
	    new_Screen_সারিগুলি_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    যদি (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_সারি_Offset = LALLOC_MULT(unsigned, Rows);
    new_সারি_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(বর্ণ, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	যদি (win_alloc_সারিগুলি(wp) == FAIL)
	{
	    outofmem = TRUE;
	    গচ্ছ give_up;
	}
    }
    প্রতি (পূর্ণাঙ্ক i = 0; i < AUCMD_WIN_গুনতি; ++i)
	যদি (aucmd_win[i].auc_win != রিক্ত
		&& aucmd_win[i].auc_win->w_সারিগুলি == রিক্ত
		&& win_alloc_সারিগুলি(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    ভঙ্গ;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	যদি (win_alloc_সারিগুলি(wp) == FAIL)
	{
	    outofmem = TRUE;
	    গচ্ছ give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    যদি (win_alloc_সারিগুলি(wp) == FAIL)
	    {
		outofmem = TRUE;
		গচ্ছ give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
	যদি (new_Screen_সারিগুলি_C[i] == রিক্ত)
	{
	    found_null = TRUE;
	    ভঙ্গ;
	}
    যদি (new_Screen_সারিগুলি == রিক্ত
	    || (enc_utf8 && (new_Screen_সারিগুলি_UC == রিক্ত || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == রিক্ত)
	    || new_ScreenAttrs == রিক্ত
	    || new_ScreenCols == রিক্ত
	    || new_সারি_Offset == রিক্ত
	    || new_সারি_Wraps == রিক্ত
	    || new_TabPageIdxs == রিক্ত
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == রিক্ত
	    || new_popup_mask_next == রিক্ত
	    || new_popup_transparent == রিক্ত
#endif
	    || outofmem)
    {
	যদি (Screen_সারিগুলি != রিক্ত || !done_outofmem_বার্তা)
	{
	    // guess the size
	    do_outofmem_বার্তা((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_বার্তা = TRUE;
	}
	VIM_CLEAR(new_Screen_সারিগুলি);
	VIM_CLEAR(new_Screen_সারিগুলি_UC);
	প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_সারিগুলি_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_সারি_Offset);
	VIM_CLEAR(new_সারি_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    অন্যথা
    {
	done_outofmem_বার্তা = FALSE;

	প্রতি (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_সারি_Offset[new_row] = new_row * Columns;
	    new_সারি_Wraps[new_row] = FALSE;

	    (শূন্য)vim_memset(new_Screen_সারিগুলি + new_row * Columns,
				  ' ', (size_t)Columns * পরিমাণ(schar_T));
	    যদি (enc_utf8)
	    {
		(শূন্য)vim_memset(new_Screen_সারিগুলি_UC + new_row * Columns,
				   0, (size_t)Columns * পরিমাণ(u8char_T));
		প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
		    (শূন্য)vim_memset(new_Screen_সারিগুলি_C[i]
						      + new_row * Columns,
				   0, (size_t)Columns * পরিমাণ(u8char_T));
	    }
	    যদি (enc_dbcs == DBCS_JPNU)
		(শূন্য)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * পরিমাণ(schar_T));
	    (শূন্য)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * পরিমাণ(sattr_T));
	    (শূন্য)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * পরিমাণ(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    যদি (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		যদি (old_row >= 0 && Screen_সারিগুলি != রিক্ত)
		{
		    যদি (screen_Columns < Columns)
			len = screen_Columns;
		    অন্যথা
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    যদি (!(enc_utf8 && Screen_সারিগুলি_UC == রিক্ত)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_সারিগুলি + new_সারি_Offset[new_row],
				Screen_সারিগুলি + সারি_Offset[old_row],
				(size_t)len * পরিমাণ(schar_T));
		    যদি (enc_utf8 && Screen_সারিগুলি_UC != রিক্ত
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_সারিগুলি_UC + new_সারি_Offset[new_row],
				Screen_সারিগুলি_UC + সারি_Offset[old_row],
				(size_t)len * পরিমাণ(u8char_T));
			প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_সারিগুলি_C[i]
						    + new_সারি_Offset[new_row],
				Screen_সারিগুলি_C[i] + সারি_Offset[old_row],
				(size_t)len * পরিমাণ(u8char_T));
		    }
		    যদি (enc_dbcs == DBCS_JPNU && ScreenLines2 != রিক্ত)
			mch_memmove(new_ScreenLines2 + new_সারি_Offset[new_row],
				ScreenLines2 + সারি_Offset[old_row],
				(size_t)len * পরিমাণ(schar_T));
		    mch_memmove(new_ScreenAttrs + new_সারি_Offset[new_row],
			    ScreenAttrs + সারি_Offset[old_row],
			    (size_t)len * পরিমাণ(sattr_T));
		    mch_memmove(new_ScreenCols + new_সারি_Offset[new_row],
			    ScreenCols + সারি_Offset[old_row],
			    (size_t)len * পরিমাণ(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_সারি = new_Screen_সারিগুলি + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * পরিমাণ(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * পরিমাণ(বর্ণ));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_সারিগুলি = new_Screen_সারিগুলি;
    Screen_সারিগুলি_UC = new_Screen_সারিগুলি_UC;
    প্রতি (পূর্ণাঙ্ক i = 0; i < p_mco; ++i)
	Screen_সারিগুলি_C[i] = new_Screen_সারিগুলি_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    সারি_Offset = new_সারি_Offset;
    সারি_Wraps = new_সারি_Wraps;
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
    যদি (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    যদি (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    অন্যথা যদি (gui.in_use
	    && !gui.starting
	    && Screen_সারিগুলি != রিক্ত
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	যদি (বার্তা_row >= Rows)		// Rows got smaller
	    বার্তা_row = Rows - 1;		// put cursor at last row
	অন্যথা যদি (Rows > old_Rows)	// Rows got bigger
	    বার্তা_row += Rows - old_Rows; // put cursor in same place
	যদি (বার্তা_col >= cmdline_width)	// cmdline_width got smaller
	    বার্তা_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    যদি (starting == 0 && ++retry_গুনতি <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, রিক্ত, রিক্ত, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	গচ্ছ retry;
    }
}

    শূন্য
free_screenlines(শূন্য)
{
    পূর্ণাঙ্ক		i;

    VIM_CLEAR(Screen_সারিগুলি_UC);
    প্রতি (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_সারিগুলি_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_সারিগুলি);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(সারি_Offset);
    VIM_CLEAR(সারি_Wraps);
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
    পূর্ণাঙ্ক
screenclear(শূন্য)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    প্রত্যাগম screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    শূন্য
redraw_as_cleared(শূন্য)
{
    screenclear2(FALSE);
}

    স্থির পূর্ণাঙ্ক
screenclear2(পূর্ণাঙ্ক doclear)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    did_clear = FALSE;

    যদি (starting == NO_SCREEN || Screen_সারিগুলি == রিক্ত
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	প্রত্যাগম FALSE;

#ifdef FEAT_GUI
    যদি (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    প্রতি (i = 0; i < Rows; ++i)
    {
	lineclear(সারি_Offset[i], (পূর্ণাঙ্ক)Columns, 0);
	সারি_Wraps[i] = FALSE;
    }

    যদি (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	যদি (suppressed_cells != রিক্ত)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    অন্যথা
    {
	// can't clear the screen, mark all chars with invalid attributes
	প্রতি (i = 0; i < Rows; ++i)
	    lineinvalid(সারি_Offset[i], (পূর্ণাঙ্ক)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    যদি (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    বার্তা_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    বার্তা_row = cmdline_row;	// put cursor on last line for messages
    বার্তা_col = 0;
    screen_start();		// don't know where cursor is now
    বার্তা_didany = FALSE;
    বার্তা_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    প্রত্যাগম did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    স্থির শূন্য
lineclear(unsigned off, পূর্ণাঙ্ক width, পূর্ণাঙ্ক attr)
{
    (শূন্য)vim_memset(Screen_সারিগুলি + off, ' ', (size_t)width * পরিমাণ(schar_T));
    যদি (enc_utf8)
	(শূন্য)vim_memset(Screen_সারিগুলি_UC + off, 0,
					  (size_t)width * পরিমাণ(u8char_T));
    (শূন্য)vim_memset(ScreenAttrs + off, attr, (size_t)width * পরিমাণ(sattr_T));
    (শূন্য)vim_memset(ScreenCols + off, -1, (size_t)width * পরিমাণ(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    স্থির শূন্য
lineinvalid(unsigned off, পূর্ণাঙ্ক width)
{
    (শূন্য)vim_memset(ScreenAttrs + off, -1, (size_t)width * পরিমাণ(sattr_T));
    (শূন্য)vim_memset(ScreenCols + off, -1, (size_t)width * পরিমাণ(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    শূন্য
সারি_was_clobbered(পূর্ণাঙ্ক screen_lnum)
{
    lineinvalid(সারি_Offset[screen_lnum], (পূর্ণাঙ্ক)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    স্থির শূন্য
linecopy(পূর্ণাঙ্ক to, পূর্ণাঙ্ক from, win_T *wp)
{
    unsigned	off_to = সারি_Offset[to] + wp->w_wincol;
    unsigned	off_from = সারি_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_সারিগুলি + off_to, Screen_সারিগুলি + off_from,
	    wp->w_width * পরিমাণ(schar_T));
    যদি (enc_utf8)
    {
	পূর্ণাঙ্ক	i;

	mch_memmove(Screen_সারিগুলি_UC + off_to, Screen_সারিগুলি_UC + off_from,
		wp->w_width * পরিমাণ(u8char_T));
	প্রতি (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_সারিগুলি_C[i] + off_to, Screen_সারিগুলি_C[i] + off_from,
		    wp->w_width * পরিমাণ(u8char_T));
    }
    যদি (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * পরিমাণ(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * পরিমাণ(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * পরিমাণ(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    পূর্ণাঙ্ক
can_clear(char_u *p)
{
    প্রত্যাগম (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#অন্যথা
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
    শূন্য
screen_start(শূন্য)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    শূন্য
windgoto(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    sattr_T	    *p;
    পূর্ণাঙ্ক		    i;
    পূর্ণাঙ্ক		    plan;
    পূর্ণাঙ্ক		    cost;
    পূর্ণাঙ্ক		    wouldbe_col;
    পূর্ণাঙ্ক		    noinvcurs;
    char_u	    *bs;
    পূর্ণাঙ্ক		    goto_cost;
    পূর্ণাঙ্ক		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    যদি (Screen_সারিগুলি == রিক্ত)
	প্রত্যাগম;
    যদি (col == screen_cur_col && row == screen_cur_row)
	প্রত্যাগম;

    // Check for valid position.
    যদি (row < 0)	// window without text lines?
	row = 0;
    যদি (row >= screen_Rows)
	row = screen_Rows - 1;
    যদি (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    যদি (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    অন্যথা
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
    যদি (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = রিক্ত;			    // init for GCC
	attr = screen_attr;
	যদি (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    যদি (*T_LE)
		bs = T_LE;		    // "cursor left"
	    অন্যথা
		bs = T_BC;		    // "backspace character (old)
	    যদি (*bs)
		cost = (screen_cur_col - col) * (পূর্ণাঙ্ক)STRLEN(bs);
	    অন্যথা
		cost = 999;
	    যদি (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    অন্যথা
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    যদি (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	অন্যথা যদি (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    যদি (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	অন্যথা
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
	যদি (i > 0)
	    cost += i;
	যদি (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + সারি_Offset[row] + wouldbe_col;
	    যাবত্ (i && *p++ == attr)
		--i;
	    যদি (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		যদি (*--p == 0)
		{
		    cost += noinvcurs;
		    যাবত্ (i && *p++ == 0)
			--i;
		}
		যদি (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    যদি (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		প্রতি (i = wouldbe_col; i < col; ++i)
		    যদি (Screen_সারিগুলি_UC[সারি_Offset[row] + i] != 0)
		    {
			cost = 999;
			ভঙ্গ;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    যদি (cost < 999)
		প্রতি (i = wouldbe_col; i < col; ++i)
		    যদি (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			ভঙ্গ;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	যদি (cost < goto_cost)
	{
	    যদি (plan == PLAN_LE)
	    {
		যদি (noinvcurs)
		    screen_stop_highlight();
		যাবত্ (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    অন্যথা যদি (plan == PLAN_CR)
	    {
		যদি (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    অন্যথা যদি (plan == PLAN_NL)
	    {
		যদি (noinvcurs)
		    screen_stop_highlight();
		যাবত্ (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    যদি (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		যদি (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    যাবত্ (i-- > 0)
			out_char(*T_ND);
		}
		অন্যথা
		{
		    পূর্ণাঙ্ক	off;

		    off = সারি_Offset[row] + screen_cur_col;
		    যাবত্ (i-- > 0)
		    {
			যদি (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_সারিগুলি[off]);
			যদি (enc_dbcs == DBCS_JPNU
				&& Screen_সারিগুলি[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    অন্যথা
	cost = 999;

    যদি (cost >= goto_cost)
    {
	যদি (noinvcurs)
	    screen_stop_highlight();
	যদি (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_ডান(col - screen_cur_col);
	অন্যথা
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    শূন্য
setcursor(শূন্য)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    পূর্ণাঙ্ক
cursor_screen_cells(শূন্য)
{
    প্রত্যাগম has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    শূন্য
setcursor_mayforce(পূর্ণাঙ্ক force)
{
    যদি (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((পূর্ণাঙ্ক)curwin->w_width - curwin->w_wcol
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
    পূর্ণাঙ্ক
win_ins_সারিগুলি(
    win_T	*wp,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		সারি_গুনতি,
    পূর্ণাঙ্ক		invalid,
    পূর্ণাঙ্ক		mayclear)
{
    পূর্ণাঙ্ক		did_delete;
    পূর্ণাঙ্ক		nextrow;
    পূর্ণাঙ্ক		lastrow;
    পূর্ণাঙ্ক		retval;

    যদি (invalid)
	wp->w_সারিগুলি_valid = 0;

    // with only a few lines it's not worth the effort
    যদি (wp->w_height < 5)
	প্রত্যাগম FAIL;

    // with the popup menu visible this might not work correctly
    যদি (pum_visible())
	প্রত্যাগম FAIL;

    যদি (সারি_গুনতি > wp->w_height - row)
	সারি_গুনতি = wp->w_height - row;

    retval = win_do_সারিগুলি(wp, row, সারি_গুনতি, mayclear, FALSE, 0);
    যদি (retval != MAYBE)
	প্রত্যাগম retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    যদি (wp->w_next != রিক্ত || wp->w_status_height)
    {
	যদি (screen_del_সারিগুলি(0, W_WINROW(wp) + wp->w_height - সারি_গুনতি,
				  সারি_গুনতি, (পূর্ণাঙ্ক)Rows, FALSE, 0, রিক্ত) == OK)
	    did_delete = TRUE;
	অন্যথা যদি (wp->w_next)
	    প্রত্যাগম FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    যদি (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + সারি_গুনতি;
	যদি (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - সারি_গুনতি, lastrow - সারি_গুনতি,
		  wp->w_wincol, (পূর্ণাঙ্ক)W_ENDCOL(wp), ' ', ' ', 0);
    }

    যদি (screen_ins_সারিগুলি(0, W_WINROW(wp) + row, সারি_গুনতি, (পূর্ণাঙ্ক)Rows, 0, রিক্ত)
								      == FAIL)
    {
	// deletion will have messed up other windows
	যদি (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    পূর্ণাঙ্ক
win_del_সারিগুলি(
    win_T	*wp,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		সারি_গুনতি,
    পূর্ণাঙ্ক		invalid,
    পূর্ণাঙ্ক		mayclear,
    পূর্ণাঙ্ক		clear_attr)	    // for clearing lines
{
    পূর্ণাঙ্ক		retval;

    যদি (invalid)
	wp->w_সারিগুলি_valid = 0;

    যদি (সারি_গুনতি > wp->w_height - row)
	সারি_গুনতি = wp->w_height - row;

    retval = win_do_সারিগুলি(wp, row, সারি_গুনতি, mayclear, TRUE, clear_attr);
    যদি (retval != MAYBE)
	প্রত্যাগম retval;

    যদি (screen_del_সারিগুলি(0, W_WINROW(wp) + row, সারি_গুনতি,
				   (পূর্ণাঙ্ক)Rows, FALSE, clear_attr, রিক্ত) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    যদি (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	যদি (screen_ins_সারিগুলি(0, W_WINROW(wp) + wp->w_height - সারি_গুনতি,
			      সারি_গুনতি, (পূর্ণাঙ্ক)Rows, clear_attr, রিক্ত) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    অন্যথা
	redraw_cmdline = TRUE;
    প্রত্যাগম OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    স্থির পূর্ণাঙ্ক
win_do_সারিগুলি(
    win_T	*wp,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		সারি_গুনতি,
    পূর্ণাঙ্ক		mayclear,
    পূর্ণাঙ্ক		del,
    পূর্ণাঙ্ক		clear_attr)
{
    পূর্ণাঙ্ক		retval;

    যদি (!redrawing() || সারি_গুনতি <= 0)
	প্রত্যাগম FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    যদি (no_win_do_সারিগুলি_ins && !del)
	প্রত্যাগম FAIL;

    // only a few lines left: redraw is faster
    যদি (mayclear && Rows - সারি_গুনতি < 5
	    && wp->w_width == topframe->fr_width)
    {
	যদি (!no_win_do_সারিগুলি_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	প্রত্যাগম FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    যদি (popup_visible)
	প্রত্যাগম FAIL;
#endif

    // Delete all remaining lines
    যদি (row + সারি_গুনতি >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (পূর্ণাঙ্ক)W_ENDCOL(wp), ' ', ' ', 0);
	প্রত্যাগম OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    যদি (!no_win_do_সারিগুলি_ins)
	clear_cmdline = TRUE;

#যদি defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    যদি (tabpanel_width() > 0)
	প্রত্যাগম FAIL;
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
    যদি (scroll_region || wp->w_width != topframe->fr_width)
    {
	যদি (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	যদি (del)
	    retval = screen_del_সারিগুলি(W_WINROW(wp) + row, 0, সারি_গুনতি,
				    wp->w_height - row, FALSE, clear_attr, wp);
	অন্যথা
	    retval = screen_ins_সারিগুলি(W_WINROW(wp) + row, 0, সারি_গুনতি,
					   wp->w_height - row, clear_attr, wp);
	যদি (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	প্রত্যাগম retval;
    }

    যদি (wp->w_next != রিক্ত && p_tf) // don't delete/insert on fast terminal
	প্রত্যাগম FAIL;

    প্রত্যাগম MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    স্থির শূন্য
win_rest_invalid(win_T *wp)
{
    যাবত্ (wp != রিক্ত)
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
    পূর্ণাঙ্ক
screen_ins_সারিগুলি(
    পূর্ণাঙ্ক		off,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		সারি_গুনতি,
    পূর্ণাঙ্ক		end,
    পূর্ণাঙ্ক		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;
    unsigned	temp;
    পূর্ণাঙ্ক		cursor_row;
    পূর্ণাঙ্ক		cursor_col = 0;
    পূর্ণাঙ্ক		type;
    পূর্ণাঙ্ক		result_empty;
    পূর্ণাঙ্ক		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    যদি (!screen_valid(TRUE)
	     || সারি_গুনতি <= 0 || সারি_গুনতি > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	প্রত্যাগম FAIL;

#ifdef FEAT_PROP_POPUP
    যদি (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, সারি_গুনতি);
# endif
	প্রত্যাগম FAIL;
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
    result_empty = (row + সারি_গুনতি >= end);
    যদি (wp != রিক্ত && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	যদি (সারি_গুনতি > 3)
	    প্রত্যাগম FAIL;
	type = USE_REDRAW;
    }
    অন্যথা যদি (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    অন্যথা যদি (*T_CAL != NUL && (সারি_গুনতি > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    অন্যথা যদি (*T_CDL != NUL && result_empty && (সারি_গুনতি > 1 || !can_ce))
	type = USE_T_CDL;
    অন্যথা যদি (*T_AL != NUL)
	type = USE_T_AL;
    অন্যথা যদি (can_ce && result_empty)
	type = USE_T_CE;
    অন্যথা যদি (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    অন্যথা যদি (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    অন্যথা
	প্রত্যাগম FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    যদি (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	প্রত্যাগম screen_del_সারিগুলি(off, row, সারি_গুনতি, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    যদি (*T_DB)
	screen_del_সারিগুলি(off, end - সারি_গুনতি, সারি_গুনতি, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    যদি (off + row > 0 || (wp != রিক্ত && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    অন্যথা
	clip_scroll_selection(-সারি_গুনতি);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, সারি_গুনতি);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    যদি (wp != রিক্ত && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    যদি (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    অন্যথা
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    প্রতি (i = 0; i < সারি_গুনতি; ++i)
    {
	যদি (wp != রিক্ত && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    যাবত্ ((j -= সারি_গুনতি) >= row)
		linecopy(j + সারি_গুনতি, j, wp);
	    j += সারি_গুনতি;
	    যদি (can_clear((char_u *)" "))
		lineclear(সারি_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    অন্যথা
		lineinvalid(সারি_Offset[j] + wp->w_wincol, wp->w_width);
	    সারি_Wraps[j] = FALSE;
	}
	অন্যথা
	{
	    j = end - 1 - i;
	    temp = সারি_Offset[j];
	    যাবত্ ((j -= সারি_গুনতি) >= row)
	    {
		সারি_Offset[j + সারি_গুনতি] = সারি_Offset[j];
		সারি_Wraps[j + সারি_গুনতি] = সারি_Wraps[j];
	    }
	    সারি_Offset[j + সারি_গুনতি] = temp;
	    সারি_Wraps[j + সারি_গুনতি] = FALSE;
	    যদি (can_clear((char_u *)" "))
		lineclear(temp, (পূর্ণাঙ্ক)Columns, clear_attr);
	    অন্যথা
		lineinvalid(temp, (পূর্ণাঙ্ক)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    যদি (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    যদি (type == USE_REDRAW)
	redraw_block(row, end, wp);
    অন্যথা যদি (type == USE_T_CAL)
    {
	term_append_সারিগুলি(সারি_গুনতি);
	screen_start();		// don't know where cursor is now
    }
    অন্যথা
    {
	প্রতি (i = 0; i < সারি_গুনতি; i++)
	{
	    যদি (type == USE_T_AL)
	    {
		যদি (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    অন্যথা  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    যদি (type == USE_T_SR && *T_DA)
    {
	প্রতি (i = 0; i < সারি_গুনতি; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    যদি (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    প্রত্যাগম OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    পূর্ণাঙ্ক
screen_del_সারিগুলি(
    পূর্ণাঙ্ক		off,
    পূর্ণাঙ্ক		row,
    পূর্ণাঙ্ক		সারি_গুনতি,
    পূর্ণাঙ্ক		end,
    পূর্ণাঙ্ক		force,		// even when line_count > p_ttyscroll
    পূর্ণাঙ্ক		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    পূর্ণাঙ্ক		j;
    পূর্ণাঙ্ক		i;
    unsigned	temp;
    পূর্ণাঙ্ক		cursor_row;
    পূর্ণাঙ্ক		cursor_col = 0;
    পূর্ণাঙ্ক		cursor_end;
    পূর্ণাঙ্ক		result_empty;	// result is empty until end of region
    পূর্ণাঙ্ক		can_delete;	// deleting line codes can be used
    পূর্ণাঙ্ক		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    যদি (!screen_valid(TRUE)
	    || সারি_গুনতি <= 0
	    || (!force && সারি_গুনতি > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	প্রত্যাগম FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + সারি_গুনতি >= end;

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
    যদি (wp != রিক্ত && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	যদি (সারি_গুনতি > 3)
	    প্রত্যাগম FAIL;
	type = USE_REDRAW;
    }
    অন্যথা যদি (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    অন্যথা যদি (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    সারি_গুনতি == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    অন্যথা যদি (*T_CDL != NUL && সারি_গুনতি > 1 && can_delete)
	type = USE_T_CDL;
    অন্যথা যদি (can_clear(T_CE) && result_empty
	    && (wp == রিক্ত || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    অন্যথা যদি (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    অন্যথা যদি (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    অন্যথা
	প্রত্যাগম FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    যদি (off + row > 0 || (wp != রিক্ত && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    অন্যথা
	clip_scroll_selection(সারি_গুনতি);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -সারি_গুনতি);
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

    যদি (wp != রিক্ত && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    যদি (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    অন্যথা
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
    প্রতি (i = 0; i < সারি_গুনতি; ++i)
    {
	যদি (wp != রিক্ত && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    যাবত্ ((j += সারি_গুনতি) <= end - 1)
		linecopy(j - সারি_গুনতি, j, wp);
	    j -= সারি_গুনতি;
	    যদি (can_clear((char_u *)" "))
		lineclear(সারি_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    অন্যথা
		lineinvalid(সারি_Offset[j] + wp->w_wincol, wp->w_width);
	    সারি_Wraps[j] = FALSE;
	}
	অন্যথা
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = সারি_Offset[j];
	    যাবত্ ((j += সারি_গুনতি) <= end - 1)
	    {
		সারি_Offset[j - সারি_গুনতি] = সারি_Offset[j];
		সারি_Wraps[j - সারি_গুনতি] = সারি_Wraps[j];
	    }
	    সারি_Offset[j - সারি_গুনতি] = temp;
	    সারি_Wraps[j - সারি_গুনতি] = FALSE;
	    যদি (can_clear((char_u *)" "))
		lineclear(temp, (পূর্ণাঙ্ক)Columns, clear_attr);
	    অন্যথা
		lineinvalid(temp, (পূর্ণাঙ্ক)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    যদি (screen_attr != clear_attr)
	screen_stop_highlight();
    যদি (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    যদি (type == USE_REDRAW)
	redraw_block(row, end, wp);
    অন্যথা যদি (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    অন্যথা যদি (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_সারিগুলি(সারি_গুনতি);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    অন্যথা যদি (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	প্রতি (i = সারি_গুনতি; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    অন্যথা
    {
	প্রতি (i = সারি_গুনতি; --i >= 0; )
	{
	    যদি (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    অন্যথা // type == USE_T_CE
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
    যদি (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	প্রতি (i = সারি_গুনতি; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    যদি (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    প্রত্যাগম OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    পূর্ণাঙ্ক
skip_showmode(শূন্য)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    যদি (global_busy
	    || বার্তা_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !চাবি_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
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
    পূর্ণাঙ্ক
showmode(শূন্য)
{
    পূর্ণাঙ্ক		need_clear;
    পূর্ণাঙ্ক		length = 0;
    পূর্ণাঙ্ক		do_mode;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		nwr_save;
    পূর্ণাঙ্ক		sub_attr;
    পূর্ণাঙ্ক		show_ruler_with_pum = FALSE;

    do_mode = p_smd && বার্তা_silent == 0
	    && ((State & MODE_ঢোকাও)
		|| restart_edit != NUL
		|| VIsual_active);
    যদি (do_mode || reg_recording != 0)
    {
	যদি (skip_showmode())
	    প্রত্যাগম 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	যদি (clear_cmdline && cmdline_row < Rows - 1)
	    বার্তা_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	বার্তা_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	যদি (do_mode)
	{
	    বার্তা_puts_attr("--", attr);
#যদি defined(FEAT_XIM)
	    যদি (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# অন্যথা
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		বার্তা_puts_attr(" IM", attr);
# অন্যথা
		বার্তা_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    যদি (edit_submode != রিক্ত && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - বার্তা_row) * cmdline_width - 3;
		যদি (edit_submode_extra != রিক্ত)
		    length -= vim_strsize(edit_submode_extra);
		যদি (length > 0)
		{
		    যদি (edit_submode_pre != রিক্ত)
			length -= vim_strsize(edit_submode_pre);
		    যদি (length - vim_strsize(edit_submode) > 0)
		    {
			যদি (edit_submode_pre != রিক্ত)
			    বার্তা_puts_attr((বর্ণ *)edit_submode_pre, attr);
			বার্তা_puts_attr((বর্ণ *)edit_submode, attr);
		    }
		    যদি (edit_submode_extra != রিক্ত)
		    {
			বার্তা_puts_attr(" ", attr);  // add a space in between
			যদি ((পূর্ণাঙ্ক)edit_submode_highl < (পূর্ণাঙ্ক)HLF_গুনতি)
			    sub_attr = HL_ATTR(edit_submode_highl);
			অন্যথা
			    sub_attr = attr;
			বার্তা_puts_attr((বর্ণ *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    অন্যথা
	    {
		যদি (State & VREPLACE_FLAG)
		    বার্তা_puts_attr(_(" VREPLACE"), attr);
		অন্যথা যদি (State & REPLACE_FLAG)
		    বার্তা_puts_attr(_(" REPLACE"), attr);
		অন্যথা যদি (State & MODE_ঢোকাও)
		{
#ifdef FEAT_RIGHTLEFT
		    যদি (p_ri)
			বার্তা_puts_attr(_(" REVERSE"), attr);
#endif
		    বার্তা_puts_attr(_(" INSERT"), attr);
		}
		অন্যথা যদি (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    বার্তা_puts_attr(_(" (insert)"), attr);
		অন্যথা যদি (restart_edit == 'R')
		    বার্তা_puts_attr(_(" (replace)"), attr);
		অন্যথা যদি (restart_edit == 'V')
		    বার্তা_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		যদি (p_hkmap)
		    বার্তা_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		যদি (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    যদি (curwin->w_p_arab)
			বার্তা_puts_attr(_(" Arabic"), attr);
		    অন্যথা
# endif
			যদি (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    বার্তা_puts_attr((বর্ণ *)NameBuff, attr);
		}
#endif
		যদি ((State & MODE_ঢোকাও) && p_paste)
		    বার্তা_puts_attr(_(" (paste)"), attr);

		যদি (VIsual_active)
		{
		    বর্ণ *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    বিকল্প ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			পক্ষ 0:	p = N_(" VISUAL"); ভঙ্গ;
			পক্ষ 1: p = N_(" VISUAL LINE"); ভঙ্গ;
			পক্ষ 2: p = N_(" VISUAL BLOCK"); ভঙ্গ;
			পক্ষ 4: p = N_(" SELECT"); ভঙ্গ;
			পক্ষ 5: p = N_(" SELECT LINE"); ভঙ্গ;
			স্বভাব: p = N_(" SELECT BLOCK"); ভঙ্গ;
		    }
		    বার্তা_puts_attr(_(p), attr);
		}
		বার্তা_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	যদি (reg_recording != 0
		&& edit_submode == রিক্ত)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	যদি (need_clear || clear_cmdline || redraw_mode)
	    বার্তা_clr_eos();
	বার্তা_didout = FALSE;		// overwrite this message
	length = বার্তা_col;
	বার্তা_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    অন্যথা যদি (clear_cmdline && বার্তা_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	বার্তা_clr_cmdline();
    অন্যথা যদি (redraw_mode)
    {
	বার্তা_pos_mode();
	বার্তা_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    যদি (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    যদি (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    প্রত্যাগম length;
}

/*
 * Position for a mode message.
 */
    স্থির শূন্য
বার্তা_pos_mode(শূন্য)
{
    বার্তা_col = 0;
    বার্তা_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    শূন্য
unshowmode(পূর্ণাঙ্ক force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    যদি (!redrawing() || (!force && char_avail() && !চাবি_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    অন্যথা
	clearmode();
}

/*
 * Clear the mode message.
 */
    শূন্য
clearmode(শূন্য)
{
    পূর্ণাঙ্ক save_বার্তা_row = বার্তা_row;
    পূর্ণাঙ্ক save_বার্তা_col = বার্তা_col;

    বার্তা_pos_mode();
    যদি (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    বার্তা_clr_eos();

    বার্তা_col = save_বার্তা_col;
    বার্তা_row = save_বার্তা_row;
}

    স্থির শূন্য
recording_mode(পূর্ণাঙ্ক attr)
{
    বার্তা_puts_attr(_("recording"), attr);
    যদি (shortmess(SHM_RECORDING))
	প্রত্যাগম;

    বর্ণ s[4];

    sprintf(s, " @%c", reg_recording);
    বার্তা_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    স্থির পূর্ণাঙ্ক
mouse_has_any(শূন্য)
{
    প্রত্যাগম mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_ঢোকাও)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    শূন্য
draw_tabline(শূন্য)
{
    পূর্ণাঙ্ক		tabcount = 0;
    tabpage_T	*tp;
    পূর্ণাঙ্ক		tabwidth;
    পূর্ণাঙ্ক		col = 0;
    পূর্ণাঙ্ক		scol = 0;
    পূর্ণাঙ্ক		attr;
    win_T	*wp;
    win_T	*cwp;
    পূর্ণাঙ্ক		wincount;
    পূর্ণাঙ্ক		modified;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		attr_sel;
    পূর্ণাঙ্ক		attr_nosel = 0;
    পূর্ণাঙ্ক		attr_fill = 0;
    char_u	*p;
    পূর্ণাঙ্ক		room;
    পূর্ণাঙ্ক		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#যদি defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    যদি (Screen_সারিগুলি == রিক্ত)
	প্রত্যাগম;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    যদি (gui_use_tabline())
    {
	gui_update_tabline();
	প্রত্যাগম;
    }
#endif

    যদি (tabline_height() < 1)
	প্রত্যাগম;

#যদি defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    যদি (*p_tal != NUL)
	win_redr_custom(রিক্ত, FALSE);
    অন্যথা
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	যদি (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	প্রতি (tp = first_tabpage; tp != রিক্ত && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = ঠেলো_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    যদি (tp->tp_topframe == topframe)
		attr = attr_sel;
	    যদি (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    যদি (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    যদি (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    অন্যথা
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    প্রতি (wincount = 0; wp != রিক্ত; wp = wp->w_next, ++wincount)
		যদি (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    যদি (modified || wincount > 1)
	    {
		যদি (wincount > 1)
		{
		    len = vim_snprintf((বর্ণ *)NameBuff, MAXPATHL, "%d", wincount);
		    যদি (col + len >= Columns - 3)
			ভঙ্গ;
		    screen_puts_len(NameBuff, len, 0, col,
#যদি defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#অন্যথা
					 attr
#endif
					       );
		    col += len;
		}
		যদি (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    যদি (room > 0)
	    {
		পূর্ণাঙ্ক n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		যদি (has_mbyte)
		    যাবত্ (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		অন্যথা যদি (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		যদি (len > n)
		    len = n;

		screen_puts_len(p, (পূর্ণাঙ্ক)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    যাবত্ (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    যদি (override_success)
		বের_করো_highlight_overrides();
	}

	যদি (use_sep_chars)
	    c = '_';
	অন্যথা
	    c = ' ';
	screen_fill(0, 1, col, (পূর্ণাঙ্ক)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	যদি (p_sc && *p_sloc == 't')
	{
	    পূর্ণাঙ্ক	n = (পূর্ণাঙ্ক)Columns - col - (tabcount > 1) * 3;
	    পূর্ণাঙ্ক	width = MIN(10, n);

	    যদি (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (পূর্ণাঙ্ক)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	যদি (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (পূর্ণাঙ্ক)Columns - 1, attr_nosel);
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
    শূন্য
get_trans_bufname(buf_T *buf)
{
    যদি (buf_spname(buf) != রিক্ত)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    অন্যথা
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    পূর্ণাঙ্ক
fillchar_status(পূর্ণাঙ্ক *attr, win_T *wp)
{
    পূর্ণাঙ্ক	    fill;
    bool    override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    যদি (bt_terminal(wp->w_buffer))
    {
	যদি (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	অন্যথা
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    অন্যথা
#endif
    যদি (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    অন্যথা
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    যদি (override_success)
	বের_করো_highlight_overrides();
    প্রত্যাগম fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    স্থির bool
vsep_row_is_curwin(win_T *wp, পূর্ণাঙ্ক row)
{
    যদি (wp == curwin)
	প্রত্যাগম true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    যদি (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	প্রত্যাগম true;

    প্রত্যাগম false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    পূর্ণাঙ্ক
fillchar_vsep(পূর্ণাঙ্ক *attr, win_T *wp, পূর্ণাঙ্ক row)
{
    bool override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);
    যদি (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    অন্যথা
	*attr = HL_ATTR(HLF_CNC);
    যদি (override_success)
	বের_করো_highlight_overrides();

    যদি (*attr == 0 && wp->w_fill_chars.vert == ' ')
	প্রত্যাগম '|';
    অন্যথা
	প্রত্যাগম wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    পূর্ণাঙ্ক
redrawing(শূন্য)
{
#ifdef FEAT_EVAL
    যদি (disable_redraw_for_testing)
	প্রত্যাগম 0;
    অন্যথা
#endif
	প্রত্যাগম ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !চাবি_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    পূর্ণাঙ্ক
messaging(শূন্য)
{
    প্রত্যাগম (!(p_lz && char_avail() && !চাবি_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    শূন্য
comp_col(শূন্য)
{
    পূর্ণাঙ্ক last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    যদি (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#অন্যথা
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	যদি (!last_has_status)
	    sc_col = ru_col;
    }
    যদি (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	যদি (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    যদি (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    যদি (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#যদি defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    পূর্ণাঙ্ক
সংখ্যা_width(win_T *wp)
{
    পূর্ণাঙ্ক		n;
    linenr_T	lnum;

    যদি (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    অন্যথা
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_সারি_গুনতি;

    যদি (lnum == wp->w_nrwidth_সারি_গুনতি && wp->w_nuw_cached == wp->w_p_nuw)
	প্রত্যাগম wp->w_nrwidth_width;
    wp->w_nrwidth_সারি_গুনতি = lnum;

    n = 0;
    কুরু
    {
	lnum /= 10;
	++n;
    } যাবত্ (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    যদি (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    যদি (n < 2 && get_first_valid_sign(wp) != রিক্ত
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    প্রত্যাগম n;
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    পূর্ণাঙ্ক
screen_screencol(শূন্য)
{
    প্রত্যাগম screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    পূর্ণাঙ্ক
screen_screenrow(শূন্য)
{
    প্রত্যাগম screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    স্থির পূর্ণাঙ্ক
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    যদি (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T অঙ্ক = 0;
	পূর্ণাঙ্ক	    bytes;
	পূর্ণাঙ্ক	    n;

	প্রতি (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    যদি (n < 0)
		প্রত্যাগম 0;
	    অঙ্ক = অঙ্ক * 256 + n;
	}
	*p += 2;
	প্রত্যাগম অঙ্ক;
    }
    প্রত্যাগম mb_ptr2char_adv(p);
}

সংরচনা charstab
{
    পূর্ণাঙ্ক	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

স্থির fill_chars_T fill_chars;
স্থির সংরচনা charstab filltab[] =
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
#যদি defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
স্থির lcs_chars_T lcs_chars;
স্থির সংরচনা charstab lcstab[] =
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
#অন্যথা
    CHARSTAB_ENTRY(রিক্ত,		    "conceal"),
#endif
    CHARSTAB_ENTRY(রিক্ত,		    "multispace"),
    CHARSTAB_ENTRY(রিক্ত,		    "leadmultispace")
};

    স্থির বর্ণ *
field_মান_err(বর্ণ *errbuf, size_t errbuflen, বর্ণ *fmt, char_u *field)
{
    যদি (errbuf == রিক্ত)
	প্রত্যাগম "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    প্রত্যাগম errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    স্থির বর্ণ *
set_chars_option(win_T *wp, char_u *মান, পূর্ণাঙ্ক is_listchars, পূর্ণাঙ্ক apply,
						বর্ণ *errbuf, size_t errbuflen)
{
    পূর্ণাঙ্ক	    round, i, entries;
    char_u  *p, *s;
    পূর্ণাঙ্ক	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = রিক্ত;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = রিক্ত; // Last occurrence of "leadmultispace:"
    পূর্ণাঙ্ক	    multispace_len = 0;	      // Length of lcs-multispace string
    পূর্ণাঙ্ক	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    সংরচনা charstab *tab;

    যদি (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	যদি (wp->w_p_lcs[0] == NUL)
	    মান = p_lcs;  // local value is empty, use the global value
    }
    অন্যথা
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	যদি (wp->w_p_fcs[0] == NUL)
	    মান = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    প্রতি (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	পূর্ণাঙ্ক has_tab = FALSE, has_leadtab = FALSE;

	যদি (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    যদি (is_listchars)
	    {
		প্রতি (i = 0; i < entries; ++i)
		    যদি (tab[i].cp != রিক্ত)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		যদি (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(পূর্ণাঙ্ক, multispace_len + 1);
		    যদি (lcs_chars.multispace != রিক্ত)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		অন্যথা
		    lcs_chars.multispace = রিক্ত;

		যদি (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(পূর্ণাঙ্ক, lead_multispace_len + 1);
		    যদি (lcs_chars.leadmultispace != রিক্ত)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		অন্যথা
		    lcs_chars.leadmultispace = রিক্ত;
	    }
	    অন্যথা
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
#যদি defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = মান;
	যাবত্ (*p)
	{
	    প্রতি (i = 0; i < entries; ++i)
	    {
		যদি (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    অনুবর্ত;

		s = p + tab[i].name.length + 1;

		যদি (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    যদি (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			যাবত্ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    যদি (char2cells(c1) > 1)
				প্রত্যাগম field_মান_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			যদি (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    প্রত্যাগম field_মান_err(errbuf, errbuflen,
				    e_wrong_সংখ্যা_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    অন্যথা
		    {
			পূর্ণাঙ্ক multispace_pos = 0;

			যাবত্ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    যদি (p == last_multispace && lcs_chars.multispace != রিক্ত)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ভঙ্গ;
		}

		যদি (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    যদি (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			যাবত্ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    যদি (char2cells(c1) > 1)
				প্রত্যাগম field_মান_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			যদি (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    প্রত্যাগম field_মান_err(errbuf, errbuflen,
				    e_wrong_সংখ্যা_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    অন্যথা
		    {
			পূর্ণাঙ্ক multispace_pos = 0;

			যাবত্ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    যদি (p == last_lmultispace && lcs_chars.leadmultispace != রিক্ত)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ভঙ্গ;
		}

		c2 = c3 = 0;
		যদি (*s == NUL)
		    প্রত্যাগম field_মান_err(errbuf, errbuflen,
				    e_wrong_সংখ্যা_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		যদি (char2cells(c1) > 1)
		    প্রত্যাগম field_মান_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		যদি (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    যদি (*s == NUL)
			প্রত্যাগম field_মান_err(errbuf, errbuflen,
				    e_wrong_সংখ্যা_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    যদি (char2cells(c2) > 1)
			প্রত্যাগম field_মান_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    যদি (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			যদি (char2cells(c3) > 1)
			    প্রত্যাগম field_মান_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    যদি (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    অন্যথা  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		যদি (*s == ',' || *s == NUL)
		{
		    যদি (round > 0)
		    {
			যদি (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			অন্যথা যদি (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			অন্যথা যদি (tab[i].cp != রিক্ত)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    ভঙ্গ;
		}
		অন্যথা
		    প্রত্যাগম field_মান_err(errbuf, errbuflen,
				    e_wrong_সংখ্যা_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    যদি (i == entries)
		প্রত্যাগম e_invalid_argument;

	    যদি (*p == ',')
		++p;
	}

	যদি (is_listchars && has_leadtab && !has_tab)
	    প্রত্যাগম e_leadtab_requires_tab;
    }

    যদি (apply)
    {
	যদি (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	অন্যথা
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    প্রত্যাগম রিক্ত;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    বর্ণ *
set_fillchars_option(win_T *wp, char_u *val, পূর্ণাঙ্ক apply, বর্ণ *errbuf,
							      size_t errbuflen)
{
    প্রত্যাগম set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    বর্ণ *
set_listchars_option(win_T *wp, char_u *val, পূর্ণাঙ্ক apply, বর্ণ *errbuf,
							      size_t errbuflen)
{
    প্রত্যাগম set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(filltab))
	প্রত্যাগম রিক্ত;

    প্রত্যাগম filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(lcstab))
	প্রত্যাগম রিক্ত;

    প্রত্যাগম lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    বর্ণ *
check_chars_options(শূন্য)
{
    tabpage_T   *tp;
    win_T	    *wp;

    যদি (set_listchars_option(curwin, p_lcs, FALSE, রিক্ত, 0) != রিক্ত)
	প্রত্যাগম e_conflicts_with_মান_of_listchars;
    যদি (set_fillchars_option(curwin, p_fcs, FALSE, রিক্ত, 0) != রিক্ত)
	প্রত্যাগম e_conflicts_with_মান_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	যদি (set_listchars_option(wp, wp->w_p_lcs, FALSE, রিক্ত, 0) != রিক্ত)
	    প্রত্যাগম e_conflicts_with_মান_of_listchars;
	যদি (set_fillchars_option(wp, wp->w_p_fcs, FALSE, রিক্ত, 0) != রিক্ত)
	    প্রত্যাগম e_conflicts_with_মান_of_fillchars;
    }
    প্রত্যাগম রিক্ত;
}
