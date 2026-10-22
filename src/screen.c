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
स्थिर पूर्णांक	screen_attr = 0;

स्थिर शून्य screen_char_2(अचिन्हित off, पूर्णांक row, पूर्णांक col);
स्थिर पूर्णांक screenclear2(पूर्णांक doclear);
स्थिर शून्य lineclear(अचिन्हित off, पूर्णांक width, पूर्णांक attr);
स्थिर शून्य lineinvalid(अचिन्हित off, पूर्णांक width);
स्थिर पूर्णांक win_do_ओळी(win_T *wp, पूर्णांक row, पूर्णांक ओळ_मोजणी, पूर्णांक mayclear, पूर्णांक del, पूर्णांक clear_attr);
स्थिर शून्य win_rest_invalid(win_T *wp);
स्थिर शून्य संदेश_pos_mode(शून्य);
स्थिर शून्य recording_mode(पूर्णांक attr);

// Ugly global: overrule attribute used by screen_char()
स्थिर पूर्णांक screen_char_attr = 0;

#जर defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    पूर्णांक
conceal_cursor_ओळ(win_T *wp)
{
    पूर्णांक		c;

    जर (*wp->w_p_cocu == NUL)
	परत FALSE;
    जर (get_real_state() & MODE_VISUAL)
	c = 'v';
    अन्यथा जर (State & MODE_घाला)
	c = 'i';
    अन्यथा जर (State & MODE_NORMAL)
	c = 'n';
    अन्यथा जर (State & MODE_CMDLINE)
	c = 'c';
    अन्यथा
	परत FALSE;
    परत vim_strchr(wp->w_p_cocu, c) != रिक्त;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    शून्य
conceal_check_cursor_ओळ(पूर्णांक was_concealed)
{
    जर (curwin->w_p_cole <= 0 || conceal_cursor_ओळ(curwin) == was_concealed)
	परत;

    पूर्णांक wcol = curwin->w_wcol;

    need_cursor_ओळ_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    जर (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    पूर्णांक
get_win_attr(win_T *wp)
{
    पूर्णांक	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);

    जर (win_attr != 0)
    {
	जर (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	अन्यथा
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    अन्यथा जर (WIN_IS_POPUP(wp))
    {
	जर (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	अन्यथा
	    win_attr = HL_ATTR(HLF_काढा);    // Popup
    }
#endif

    जर (override_success)
	काढा_highlight_overrides();
    परत win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    स्थिर पूर्णांक
screen_fill_end(
	win_T *wp,
	पूर्णांक	c1,
	पूर्णांक	c2,
	पूर्णांक	off,
	पूर्णांक	width,
	पूर्णांक	row,
	पूर्णांक	endrow,
	पूर्णांक	attr)
{
    पूर्णांक	    nn = off + width;

    जर (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    जर (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (पूर्णांक)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    अन्यथा
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (पूर्णांक)wp->w_wincol + nn,
		c1, c2, attr);
    परत nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    शून्य
win_draw_end(
    win_T	*wp,
    पूर्णांक		c1,
    पूर्णांक		c2,
    पूर्णांक		draw_margin,
    पूर्णांक		row,
    पूर्णांक		endrow,
    hlf_T	hl)
{
    पूर्णांक		n = 0;
    पूर्णांक		attr = HL_ATTR(hl);
    पूर्णांक		win_attr = get_win_attr(wp);
    bool	override_success =
	ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    जर (draw_margin)
    {
#ifdef FEAT_FOLDING
	पूर्णांक	fdc = compute_foldcolumn(wp, 0);

	जर (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	जर (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	जर ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == रिक्त)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, संख्या_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    जर (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    अन्यथा
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (पूर्णांक)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    जर (override_success)
	काढा_highlight_overrides();
}

#जर defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    पूर्णांक
compute_foldcolumn(win_T *wp, पूर्णांक col)
{
    पूर्णांक wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    पूर्णांक n = wp->w_width - (col + wmw);

    परत MIN(wp->w_p_fdc, n);
}

/*
 * Fill the foldcolumn at "p" for window "wp".
 * Only to be called when 'foldcolumn' > 0.
 * Returns the number of bytes stored in 'p'. When non-multibyte characters are
 * used for the fold column markers, this is equal to 'fdc' setting. Otherwise,
 * this will be greater than 'fdc'.
 */
    माप_प्रकार
fill_foldcolumn(
    char_u	*p,
    win_T	*wp,
    पूर्णांक		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    पूर्णांक		i = 0;
    पूर्णांक		level;
    पूर्णांक		first_level;
    पूर्णांक		empty;
    पूर्णांक		fdc = compute_foldcolumn(wp, 0);
    माप_प्रकार	byte_counter = 0;
    पूर्णांक		चिन्ह = 0;
    पूर्णांक		len = 0;
    पूर्णांक		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    जर (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    साठी (i = 0; i < n; i++)
    {
	जर (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_खालचा_level)
	    चिन्ह = wp->w_fill_chars.foldopen;
	अन्यथा जर (first_level == 1)
	    चिन्ह = wp->w_fill_chars.foldsep;
	अन्यथा जर (wp->w_fill_chars.foldinner != NUL)
	    चिन्ह = wp->w_fill_chars.foldinner;
	अन्यथा जर (first_level + i <= 9)
	    चिन्ह = '0' + first_level + i;
	अन्यथा
	    चिन्ह = '>';

	len = utf_char2bytes(चिन्ह, &p[byte_counter]);
	byte_counter += len;
	जर (first_level + i >= level)
	{
	    i++;
	    थांबा;
	}
    }

    जर (closed)
    {
	जर (चिन्ह != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    जर (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	चिन्ह = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(चिन्ह, &p[byte_counter]);
	byte_counter += len;
    }

    परत MAX(byte_counter + (fdc - i), (माप_प्रकार)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    स्थिर पूर्णांक
comp_char_differs(पूर्णांक off_from, पूर्णांक off_to)
{
    पूर्णांक	    i;

    साठी (i = 0; i < Screen_mco; ++i)
    {
	जर (Screen_ओळी_C[i][off_from] != Screen_ओळी_C[i][off_to])
	    परत TRUE;
	जर (Screen_ओळी_C[i][off_from] == 0)
	    थांबा;
    }
    परत FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    स्थिर पूर्णांक
char_needs_redraw(पूर्णांक off_from, पूर्णांक off_to, पूर्णांक cols)
{
    जर (cols > 0
	    && ((Screen_ओळी[off_from] != Screen_ओळी[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_ओळी[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_ओळी[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_ओळी[off_from + 1]
						 != Screen_ओळी[off_to + 1])))
		|| (enc_utf8
		    && (Screen_ओळी_UC[off_from] != Screen_ओळी_UC[off_to]
			|| (Screen_ओळी_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_ओळी[off_from + 1]
						!= Screen_ओळी[off_to + 1])))))
	परत TRUE;
    परत FALSE;
}

#जर defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    पूर्णांक
screen_get_current_ओळ_off(शून्य)
{
    परत (पूर्णांक)(current_Screen_ओळ - Screen_ओळी);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    स्थिर पूर्णांक
blocked_by_popup(पूर्णांक row, पूर्णांक col)
{
    पूर्णांक off;

    जर (!popup_visible)
	परत FALSE;
    off = row * screen_Columns + col;
    परत popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    शून्य
reset_screen_attr(शून्य)
{
#ifdef FEAT_GUI
    जर (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    अन्यथा
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    स्थिर पूर्णांक
skip_for_popup(पूर्णांक row, पूर्णांक col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    जर (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	परत TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    जर (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	परत TRUE;
#अन्यथा
    जर (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	परत TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    जर (blocked_by_popup(row, col))
	परत TRUE;
#endif
    परत FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
स्थिर char_u	*suppressed_cells = रिक्त;
स्थिर पूर्णांक	suppressed_rows = 0;
स्थिर पूर्णांक	suppressed_cols = 0;

    स्थिर शून्य
mark_suppressed_cell(पूर्णांक row, पूर्णांक col)
{
    जर (suppressed_cells == रिक्त
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (माप_प्रकार)screen_Rows * screen_Columns);
	जर (suppressed_cells == रिक्त)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    परत;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    स्थिर शून्य
unmark_suppressed_cell(पूर्णांक row, पूर्णांक col)
{
    जर (suppressed_cells != रिक्त
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    स्थिर पूर्णांक
is_suppressed_cell(पूर्णांक row, पूर्णांक col)
{
    परत suppressed_cells != रिक्त
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
प्रकारव्याख्या संरचना {
    पूर्णांक		popup_attr;	// get_win_attr(screen_opacity_popup)
    पूर्णांक		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    स्थिर ओळीत शून्य
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    स्थिर ओळीत पूर्णांक
popup_base_attr_or(पूर्णांक row, पूर्णांक col, पूर्णांक fallback)
{
    पूर्णांक		base = fallback;

    popup_get_base_screen_cell(row, col, रिक्त, &base, रिक्त);
    परत base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    स्थिर ओळीत पूर्णांक
popup_blend_with_base(
	स्थिरांक popup_opacity_T	*po,
	पूर्णांक			char_attr,
	पूर्णांक			base_attr,
	पूर्णांक			blend_fg)
{
    पूर्णांक		combined = hl_combine_attr(po->popup_attr, char_attr);

    परत hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    स्थिर शून्य
resolve_wide_char_opacity_attrs(
	पूर्णांक row, पूर्णांक col1, पूर्णांक col2,
	sattr_T *attr1, sattr_T *attr2)
{
    पूर्णांक		bg1, bg2;
    पूर्णांक		base1 = 0;
    पूर्णांक		base2 = 0;
    attrentry_T	*ae;

    जर (*attr1 == *attr2)
	परत;

    popup_get_base_screen_cell(row, col1, रिक्त, &base1, रिक्त);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != रिक्त && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# अन्यथा
    bg1 = (ae != रिक्त && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, रिक्त, &base2, रिक्त);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != रिक्त && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# अन्यथा
    bg2 = (ae != रिक्त && ae->ae_u.cterm.bg_color != 0);
# endif

    जर (bg1 && !bg2)
	*attr1 = *attr2;
    अन्यथा जर (!bg1 && bg2)
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
screen_ओळ(
	win_T	*wp,
	पूर्णांक	row,
	पूर्णांक	coloff,
	पूर्णांक	endcol,
	पूर्णांक	clear_width,
	colnr_T	last_vcol,
	पूर्णांक	flags UNUSED)
{
    अचिन्हित	    off_from;
    अचिन्हित	    off_to;
    अचिन्हित	    max_off_from;
    अचिन्हित	    max_off_to;
    पूर्णांक		    col = 0;
    पूर्णांक		    hl;
    पूर्णांक		    force = FALSE;	// force update rest of the line
    पूर्णांक		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    पूर्णांक		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    पूर्णांक		    changed_this;	// TRUE if character changed
    पूर्णांक		    changed_next;	// TRUE if next character changed
#endif
    पूर्णांक		    clear_next = FALSE;
    पूर्णांक		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    पूर्णांक		    drawing_opacity_popup =
				screen_opacity_popup != रिक्त
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    जर (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    जर (row >= Rows)
	row = Rows - 1;
    जर (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (अचिन्हित)(current_Screen_ओळ - Screen_ओळी);
    off_to = ओळ_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = ओळ_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    जर (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	जर (clear_width > 0)
	{
	    पूर्णांक clear_start = col;

	    पर्यंत (col <= endcol && Screen_ओळी[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_ओळी_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    जर (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    साठी (पूर्णांक i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = ओळ_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    जर (coloff > 0 && enc_utf8
		   && Screen_ओळी[off_to] == 0
		   && Screen_ओळी_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_ओळी_UC[off_to - 1]) > 1)
    {
	Screen_ओळी[off_to - 1] = ' ';
	Screen_ओळी_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	जर (screen_opacity_popup == रिक्त)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    पर्यंत (col < endcol)
    {
	जर (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	अन्यथा
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	जर (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	जर (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    जर (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    जर (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	जर (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	जर (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	पूर्णांक opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	जर (drawing_opacity_popup
		&& Screen_ओळी[off_from] == ' '
		&& (!enc_utf8 || Screen_ओळी_UC[off_from] == 0))
	{
	    पूर्णांक popup_src_attr = ScreenAttrs[off_from];
	    पूर्णांक bg_is_empty = Screen_ओळी[off_to] == 0
				&& (!enc_utf8 || Screen_ओळी_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    जर (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_ओळी_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_ओळी_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    अन्यथा जर (Screen_ओळी[off_to] != 0
		    || (enc_utf8 && Screen_ओळी_UC[off_to] != 0))
	    {
		पूर्णांक bg_char_cells = (enc_utf8 && Screen_ओळी_UC[off_to] != 0)
				? utf_char2cells(Screen_ओळी_UC[off_to]) : 1;

		जर (bg_char_cells == 2)
		{
		    जर (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			पूर्णांक base_attr;
			Screen_ओळी[off_to] = ' ';
			जर (enc_utf8)
			    Screen_ओळी_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			जा skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    जर (!(Screen_ओळी[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_ओळी_UC[off_from + 1] == 0)))
			जा skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		पूर्णांक base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		जर (bg_char_cells == 2)
		{
		    पूर्णांक base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    जर (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    अन्यथा जर (bg_is_empty)
	    {
		पूर्णांक base_attr;
		Screen_ओळी[off_to] = ' ';
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

	जर (redraw_this)
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
	    जर (       p_wiv
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
		जर (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		अन्यथा
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    जर (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		जर (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_ओळी[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		अन्यथा जर (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_ओळी[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		जर (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    जर (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_ओळी[off_to] = Screen_ओळी[off_from];
	    जर (enc_utf8)
	    {
		Screen_ओळी_UC[off_to] = Screen_ओळी_UC[off_from];
		जर (Screen_ओळी_UC[off_from] != 0)
		{
		    पूर्णांक	    i;

		    साठी (i = 0; i < Screen_mco; ++i)
			Screen_ओळी_C[i][off_to] = Screen_ओळी_C[i][off_from];
		}
	    }
	    जर (char_cells == 2)
	    {
		Screen_ओळी[off_to + 1] = Screen_ओळी[off_from + 1];
		जर (enc_utf8)
		    Screen_ओळी_UC[off_to + 1] = 0;
	    }

#जर defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    जर (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# जर defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		जर (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		जर (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    जर (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    जर (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    जर (drawing_opacity_popup && po.blend > 0)
	    {
		पूर्णांक popup_src_attr = ScreenAttrs[off_from];
		पूर्णांक scol1 = col + coloff;
		पूर्णांक base1 = popup_base_attr_or(row, scol1, 0);

		जर (char_cells == 2)
		{
		    पूर्णांक base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    पूर्णांक over1 = popup_is_over_opacity(row, scol1);
		    पूर्णांक over2 = popup_is_over_opacity(row, scol1 + 1);
		    जर (over1 != over2)
		    {
			जर (over1)
			    base1 = base2;
			अन्यथा
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    अन्यथा
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    जर (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    जर (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    अन्यथा
		screen_char(off_to, row, col + coloff);
	}
	अन्यथा जर (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    जर (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    अन्यथा जर (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	जर (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    जर (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    जर (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_ओळी[off_to] = ' ';
	जर (enc_utf8)
	    Screen_ओळी_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	जर (screen_opacity_popup == रिक्त)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    जर (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	पूर्णांक startCol = col;
#endif

	// blank out the rest of the line
	पर्यंत (col < clear_width && Screen_ओळी[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_ओळी_UC[off_to] == 0)
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
	जर (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    जर (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		जर (hl > HL_ALL || (hl & HL_BOLD))
		{
		    पूर्णांक prev_cells = 1;

		    जर (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_ओळी[off_to - 1] == 0 ? 2 : 1;
		    अन्यथा जर (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			अचिन्हित off = ओळ_Offset[row];
			अचिन्हित max_off = ओळ_Offset[row] + screen_Columns;

			पर्यंत (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    जर (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			जर (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			अन्यथा
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    पर्यंत (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    जर (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	जर (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    जर (!skip_for_popup(row, col + coloff))
	    {
		पूर्णांक c;

		c = fillchar_vsep(&hl, wp, row);
		जर (Screen_ओळी[off_to] != (schar_T)c
			|| (enc_utf8 && (पूर्णांक)Screen_ओळी_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_ओळी[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    जर (enc_utf8)
		    {
			जर (c >= 0x80)
			{
			    Screen_ओळी_UC[off_to] = c;
			    Screen_ओळी_C[0][off_to] = 0;
			    Screen_ओळी[off_to] = 0x80; // avoid storing zero
			}
			अन्यथा
			    Screen_ओळी_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	अन्यथा
	    ओळ_Wraps[row] = FALSE;
    }

    जर (override_success)
	काढा_highlight_overrides();
}

#जर defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    शून्य
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    पूर्णांक		t;

    साठी (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
draw_vsep_win(win_T *wp, पूर्णांक row)
{
    जर (!wp->w_vsep_width)
	परत;

    पूर्णांक content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    पूर्णांक start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    साठी (पूर्णांक r = start_row; r < content_end; ++r)
    {
	पूर्णांक hl;
	पूर्णांक c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    जर (wp->w_status_height != 0)
    {
	पूर्णांक hl;
	पूर्णांक c;
	जर (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	अन्यथा
	    c = fillchar_vsep(&hl, wp, content_end);
	साठी (पूर्णांक r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    पूर्णांक
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    पर्यंत (fr->fr_parent != रिक्त)
    {
	जर (fr->fr_parent->fr_layout == FR_COL)
	{
	    जर (fr->fr_next != रिक्त)
		थांबा;
	}
	अन्यथा
	{
	    जर (fr->fr_next != रिक्त)
		परत TRUE;
	}
	fr = fr->fr_parent;
    }
    परत FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    पूर्णांक
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    पूर्णांक		len)	    // length of buffer
{
    char_u	*p;
    पूर्णांक		plen;

    जर (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	परत 0;

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
    जर (p == रिक्त || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	जर (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	अन्यथा
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((अक्षर *)buf, len, (अक्षर *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    जर (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    परत plen;
}

#जर defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    शून्य
win_redr_custom(
    win_T	*wp,
    पूर्णांक		draw_ruler)	// TRUE or FALSE
{
    स्थिर पूर्णांक	entered = FALSE;
    पूर्णांक		attr;
    पूर्णांक		curattr;
    पूर्णांक		row;
    पूर्णांक		col = 0;
    पूर्णांक		maxwidth;
    पूर्णांक		width;
    पूर्णांक		n;
    पूर्णांक		len;
    पूर्णांक		stlh_cnt;
    पूर्णांक		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    पूर्णांक		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    पूर्णांक		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    जर (entered)
	परत;
    entered = TRUE;

    // setup environment for the task at hand
    जर (wp == रिक्त)
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
    अन्यथा
    {
	override_success = ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	पूर्णांक in_status_ओळ = wp->w_status_height != 0;
	maxwidth = in_status_ओळ ? wp->w_width : cmdline_width;

	जर (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    जर (*stl == '%')
	    {
		जर (*++stl == '-')
		    stl++;
		जर (म_से_प((अक्षर *)stl))
		    पर्यंत (VIM_ISDIGIT(*stl))
			stl++;
		जर (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    जर (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    जर (!in_status_ओळ)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	अन्यथा
	{
	    opt_name = (char_u *)"statusline";
	    जर (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    अन्यथा
		stl = p_stl;
	}

	जर (in_status_ओळ)
	    col += wp->w_wincol;
	अन्यथा
	    col += cmdline_col_off;
    }

    जर (maxwidth <= 0)
	जा theend;

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
    पूर्णांक col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = रिक्त;
    पूर्णांक			*out_मोजणी = रिक्त;
    पूर्णांक			region_base_col = 0;
    जर (!draw_ruler)
    {
	जर (wp != रिक्त)
	{
	    out_regions = &wp->w_stl_click;
	    out_मोजणी = &wp->w_stl_click_मोजणी;
	    region_base_col = wp->w_wincol;
	}
	अन्यथा
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_मोजणी = &tabline_stl_click_मोजणी;
	    region_base_col = firstwin->w_wincol;
	}

	साठी (n = 0; n < *out_मोजणी; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_मोजणी = 0;
    }

    पूर्णांक carry_hl = 0;
    साठी (पूर्णांक i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, आकार(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	जर (p != रिक्त)
	{
	    len = vim_snprintf((अक्षर *)buf, आकार(buf), "%s", p);
	    vim_free(p);
	}
	अन्यथा
	    len = (पूर्णांक)STRLEN(buf);

	// fill up with "fillchar"
	पर्यंत (width < maxwidth && len < (पूर्णांक)आकार(buf) - 1)
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
	साठी (n = 0; hltab[n].start != रिक्त; n++)
	{
	    len = (पूर्णांक)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    जर (hltab[n].userhl == 0)
		curattr = attr;
	    अन्यथा जर (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    अन्यथा जर (wp != रिक्त && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    अन्यथा जर (wp != रिक्त && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    अन्यथा जर (wp != रिक्त && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    अन्यथा
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	जर (out_regions != रिक्त)
	{
	    पूर्णांक click_मोजणी = 0;
	    साठी (n = 0; clicktab[n].start != रिक्त; n++)
		click_मोजणी++;

	    जर (click_मोजणी > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			आकार(stl_click_region_T)
				    * (*out_मोजणी + click_मोजणी));
		जर (new_arr != रिक्त)
		{
		    char_u  *cur_funcname = रिक्त;
		    पूर्णांक	    cur_minwid = 0;
		    पूर्णांक	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    साठी (n = 0; clicktab[n].start != रिक्त; n++)
		    {
			len += vim_strnsize(p,
					    (पूर्णांक)(clicktab[n].start - p));
			p = clicktab[n].start;

			जर (cur_funcname != रिक्त)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_मोजणी];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_मोजणी)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    जर (cur_funcname != रिक्त)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_मोजणी];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_मोजणी)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	साठी (n = 0; clicktab[n].start != रिक्त; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    जर (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    जर (stl != रिक्त)
	vim_free(stl_tmp);

    जर (wp == रिक्त)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	साठी (n = 0; tabtab[n].start != रिक्त; n++)
	{
	    len += vim_strnsize(p, (पूर्णांक)(tabtab[n].start - p));
	    पर्यंत (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	पर्यंत (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    जर (override_success)
	काढा_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    शून्य
screen_putchar(पूर्णांक c, पूर्णांक row, पूर्णांक col, पूर्णांक attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    जर (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    अन्यथा
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
screen_getbytes(पूर्णांक row, पूर्णांक col, char_u *bytes, पूर्णांक *attrp)
{
    अचिन्हित off;

    // safety check
    जर (Screen_ओळी == रिक्त || row >= screen_Rows || col >= screen_Columns)
	परत;

    off = ओळ_Offset[row] + col;
    जर (attrp != रिक्त)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_ओळी[off];
    bytes[1] = NUL;

    जर (enc_utf8 && Screen_ओळी_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    अन्यथा जर (enc_dbcs == DBCS_JPNU && Screen_ओळी[off] == 0x8e)
    {
	bytes[0] = Screen_ओळी[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    अन्यथा जर (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_ओळी[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    स्थिर पूर्णांक
screen_comp_differs(पूर्णांक off, पूर्णांक *u8cc)
{
    पूर्णांक	    i;

    साठी (i = 0; i < Screen_mco; ++i)
    {
	जर (Screen_ओळी_C[i][off] != (u8char_T)u8cc[i])
	    परत TRUE;
	जर (u8cc[i] == 0)
	    थांबा;
    }
    परत FALSE;
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
    पूर्णांक		row,
    पूर्णांक		col,
    पूर्णांक		attr)
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
    पूर्णांक		textlen,
    पूर्णांक		row,
    पूर्णांक		col,
    पूर्णांक		attr_arg)
{
    पूर्णांक		attr = attr_arg;
    अचिन्हित	off;
    char_u	*ptr = text;
    पूर्णांक		len = textlen;
    पूर्णांक		c;
    अचिन्हित	max_off;
    पूर्णांक		mbyte_blen = 1;
    पूर्णांक		mbyte_cells = 1;
    पूर्णांक		u8c = 0;
    पूर्णांक		u8cc[MAX_MCO];
    पूर्णांक		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    पूर्णांक		prev_c = 0;		// previous Arabic character
    पूर्णांक		pc, nc, nc1;
    पूर्णांक		pcc[MAX_MCO];
#endif
    पूर्णांक		force_redraw_this;
    पूर्णांक		force_redraw_next = FALSE;
    पूर्णांक		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    जर (Screen_ओळी == रिक्त
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	परत;
    off = ओळ_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    जर (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	जर (!skip_for_popup(row, col - 1))
	{
	    Screen_ओळी[off - 1] = ' ';
	    जर (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    अन्यथा जर (enc_utf8)
	    {
		Screen_ओळी_UC[off - 1] = 0;
		Screen_ओळी_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = ओळ_Offset[row] + screen_Columns;
    पर्यंत (col < screen_Columns
	    && (len < 0 || (पूर्णांक)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	जर (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (पूर्णांक)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    जर (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    अन्यथा जर (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    अन्यथा	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (पूर्णांक)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		जर (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    जर (len >= 0 && (पूर्णांक)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    अन्यथा
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(पूर्णांक)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		अन्यथा
		    prev_c = u8c;
#endif
		जर (col + mbyte_cells > screen_Columns)
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
	पूर्णांक cell_attr = attr;
	जर (screen_pum_blend > 0 && pum_bg_attrs != रिक्त
		&& row >= pum_bg_शीर्ष && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    पूर्णांक soff = (row - pum_bg_शीर्ष) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_ओळी[off] != c
		|| (mbyte_cells == 2
		    && Screen_ओळी[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_ओळी_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_ओळी_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	जर ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#जर defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    जर (need_redraw && Screen_ओळी[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# जर defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		पूर्णांक	n = ScreenAttrs[off];

		जर (n > HL_ALL)
		    n = syn_attr2attr(n);
		जर (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    जर (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    जर (clear_next_cell)
		clear_next_cell = FALSE;
	    अन्यथा जर (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    जर (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_ओळी[off + mbyte_blen] = 0;
	    Screen_ओळी[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    जर (enc_utf8)
	    {
		जर (c < 0x80 && u8cc[0] == 0)
		    Screen_ओळी_UC[off] = 0;
		अन्यथा
		{
		    पूर्णांक	    i;

		    Screen_ओळी_UC[off] = u8c;
		    साठी (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ओळी_C[i][off] = u8cc[i];
			जर (u8cc[i] == 0)
			    थांबा;
		    }
		}
		जर (mbyte_cells == 2)
		{
		    Screen_ओळी[off + 1] = 0;
		    Screen_ओळी_UC[off + 1] = 0;
		    साठी (पूर्णांक ci = 0; ci < Screen_mco; ++ci)
			Screen_ओळी_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    अन्यथा जर (mbyte_cells == 2)
	    {
		Screen_ओळी[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    अन्यथा जर (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    अन्यथा
		screen_char(off, row, col);
	}
	जर (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    जर (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	अन्यथा
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    जर (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	जर (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	अन्यथा
	    screen_char(off, row, col);
    }
}

#जर defined(FEAT_शोध_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    शून्य
start_शोध_hl(शून्य)
{
    जर (!p_hls || no_hlsearch)
	परत;

    end_शोध_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_शोध_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    शून्य
end_शोध_hl(शून्य)
{
    जर (screen_शोध_hl.rm.regprog == रिक्त)
	परत;

    vim_regfree(screen_शोध_hl.rm.regprog);
    screen_शोध_hl.rm.regprog = रिक्त;
}
#endif

    स्थिर शून्य
screen_start_highlight(पूर्णांक attr)
{
    attrentry_T *aep = रिक्त;

    screen_attr = attr;
    जर (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	परत;

#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	अक्षर	buf[20];

	// The GUI handles this internally.
	प्र_लिखो(buf, "\033|%dh", attr);
	OUT_STR(buf);
	परत;
    }
#endif

    जर (attr > HL_ALL)				// special HL attr.
    {
	जर (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	अन्यथा
	    aep = syn_term_attr2entry(attr);
	जर (aep == रिक्त)	    // did ":syntax clear"
	    attr = 0;
	अन्यथा
	    attr = aep->ae_attr;
    }
#जर defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    जर (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	पूर्णांक	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	जर (p_tgc)
	{
	    जर (aep == रिक्त || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    जर (aep == रिक्त || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	अन्यथा जर (t_colors >= 256)
	{
	    जर (aep == रिक्त || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    जर (aep == रिक्त || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    जर ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    अन्यथा जर (aep != रिक्त && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    जर ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    जर ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    जर ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    जर ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    जर ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    जर (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    जर ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    जर ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    जर ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    जर (aep != रिक्त)
    {
	जर (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	जर (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    जर (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	अन्यथा
#endif
	    जर (t_colors > 1)
	    {
		जर (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	जर (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    जर (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	अन्यथा
#endif
	    जर (t_colors > 1)
	    {
		जर (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	जर (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    जर (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	अन्यथा
#endif
	    जर (t_colors > 1)
	    {
		जर (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	जर (!IS_CTERM)
	{
	    जर (aep->ae_u.term.start != रिक्त)
		out_str(aep->ae_u.term.start);
	}
    }
}

    शून्य
screen_stop_highlight(शून्य)
{
    पूर्णांक	    do_ME = FALSE;	    // output T_ME code
#जर defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    पूर्णांक	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    जर (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	जर (gui.in_use)
	{
	    अक्षर	buf[20];

	    // use internal GUI code
	    प्र_लिखो(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	अन्यथा
#endif
	{
	    पूर्णांक is_under;

	    जर (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		जर (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    जर (aep != रिक्त && ((
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
#जर defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    जर (use_vtp())
		    {
			जर (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			जर (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			जर (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    अन्यथा
		    {
			// Process FG and BG at once.
			जर (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		अन्यथा
		{
		    aep = syn_term_attr2entry(screen_attr);
		    जर (aep != रिक्त && aep->ae_u.term.stop != रिक्त)
		    {
			जर (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			अन्यथा
			    out_str(aep->ae_u.term.stop);
		    }
		}
		जर (aep == रिक्त)	    // did ":syntax clear"
		    screen_attr = 0;
		अन्यथा
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    जर (screen_attr & HL_STANDOUT)
	    {
		जर (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		अन्यथा
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    जर (is_under && *T_UCE != NUL)
	    {
		जर (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		अन्यथा
		    out_str(T_UCE);
	    }
	    जर ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		जर (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		अन्यथा
		    out_str(T_UE);
	    }
	    जर (screen_attr & HL_ITALIC)
	    {
		जर (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		अन्यथा
		    out_str(T_CZR);
	    }
	    जर (screen_attr & HL_STRIKETHROUGH)
	    {
		जर (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		अन्यथा
		    out_str(T_STE);
	    }
	    जर (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    जर (p_tgc)
	    {
		जर (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		जर (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		जर (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    अन्यथा
#endif
	    {
		जर (t_colors > 1)
		{
		    // set Normal cterm colors
		    जर (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    जर (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    जर (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    जर (cterm_normal_fg_bold)
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
    जर (!IS_CTERM)
	परत;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    जर (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#अन्यथा
	जर (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    जर (cterm_normal_fg_bold)
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
screen_char(अचिन्हित off, पूर्णांक row, पूर्णांक col)
{
    पूर्णांक		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    जर (row >= screen_Rows || col >= screen_Columns)
	परत;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    जर (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	जर (enc_utf8 && Screen_ओळी_UC[off] != 0
		&& utf_char2cells(Screen_ओळी_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    पूर्णांक off2 = off + 1;
	    Screen_ओळी[off2] = ' ';
	    Screen_ओळी_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	जर (enc_utf8 && Screen_ओळी_UC[off] != 0
		&& utf_char2cells(Screen_ओळी_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	परत;
    }
    जर (enc_utf8 && Screen_ओळी_UC[off] != 0
	    && utf_char2cells(Screen_ओळी_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	परत;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    जर (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	परत;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    जर (screen_char_attr != 0)
	attr = screen_char_attr;
    अन्यथा
	attr = ScreenAttrs[off];
    जर (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    जर (enc_utf8 && Screen_ओळी_UC[off] != 0
	    && utf_char2cells(Screen_ओळी_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    जर (screen_attr != attr)
	screen_start_highlight(attr);

    जर (enc_utf8 && Screen_ओळी_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	जर (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_ओळी_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	अन्यथा जर (utf_ambiguous_width(Screen_ओळी_UC[off]))
	{
	    जर (*p_ambw == 'd'
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
	अन्यथा जर (utf_char2cells(Screen_ओळी_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    अन्यथा
    {
	out_flush_check();
	out_char(Screen_ओळी[off]);
	// double-byte character in single-width cell
	जर (enc_dbcs == DBCS_JPNU && Screen_ओळी[off] == 0x8e)
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
screen_char_2(अचिन्हित off, पूर्णांक row, पूर्णांक col)
{
    // Check for illegal values (could be wrong when screen was resized).
    जर (off + 1 >= (अचिन्हित)(screen_Rows * screen_Columns))
	परत;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    जर (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	परत;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    जर (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	परत;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_ओळी[off + 1]);
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
    पूर्णांक		row,
    पूर्णांक		col,
    पूर्णांक		height,
    पूर्णांक		width,
    पूर्णांक		invert,
    पूर्णांक		force)
{
    पूर्णांक		r, c;
    पूर्णांक		off;
    पूर्णांक		max_off;

    // Can't use ScreenLines unless initialized
    जर (Screen_ओळी == रिक्त)
	परत;

    जर (invert)
	screen_char_attr = HL_INVERSE;
    साठी (r = row; r < row + height; ++r)
    {
	off = ओळ_Offset[r];
	max_off = off + screen_Columns;
	साठी (c = col; c < col + width; ++c)
	{
	    जर (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		जर (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    अन्यथा
	    {
		जर (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		जर (utf_off2cells(off + c, max_off) > 1)
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
redraw_block(पूर्णांक row, पूर्णांक end, win_T *wp)
{
    पूर्णांक		col;
    पूर्णांक		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    जर (wp == रिक्त)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    अन्यथा
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    शून्य
space_to_screenline(पूर्णांक off, पूर्णांक attr)
{
    Screen_ओळी[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    जर (enc_utf8)
	Screen_ओळी_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    शून्य
screen_fill(
	पूर्णांक	start_row,
	पूर्णांक	end_row,
	पूर्णांक	start_col,
	पूर्णांक	end_col,
	पूर्णांक	c1,
	पूर्णांक	c2,
	पूर्णांक	attr)
{
    पूर्णांक	    row;
    पूर्णांक	    col;
    पूर्णांक	    off;
    पूर्णांक	    end_off;
    पूर्णांक	    did_delete;
    पूर्णांक	    c;
    पूर्णांक	    norm_term;
#जर defined(FEAT_GUI) || defined(UNIX)
    पूर्णांक	    force_next = FALSE;
#endif

    जर (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    जर (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    जर (Screen_ओळी == रिक्त
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	परत;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    साठी (row = start_row; row < end_row; ++row)
    {
	जर (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    जर (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		पूर्णांक डावा_attr = ScreenAttrs[ओळ_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, डावा_attr);
	    }
	    जर (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		पूर्णांक उजवा_attr = ScreenAttrs[ओळ_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, उजवा_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	जर (c2 == ' '
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
	    जर (c1 != ' ')			// don't clear first char
		++col;

	    off = ओळ_Offset[row] + col;
	    end_off = ओळ_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    जर (enc_utf8)
		पर्यंत (off < end_off && Screen_ओळी[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_ओळी_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (पूर्णांक)(off - ओळ_Offset[row]))
#endif
			  )
		    ++off;
	    अन्यथा
		पर्यंत (off < end_off && Screen_ओळी[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (पूर्णांक)(off - ओळ_Offset[row]))
#endif
			  )
		    ++off;
	    जर (off < end_off)		// something to be cleared
	    {
		col = off - ओळ_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		पर्यंत (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (पूर्णांक)(off - ओळ_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = ओळ_Offset[row] + start_col;
	c = c1;
	साठी (col = start_col; col < end_col; ++col)
	{
	    जर ((Screen_ओळी[off] != c
		    || (enc_utf8 && (पूर्णांक)Screen_ओळी_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#जर defined(FEAT_GUI) || defined(UNIX)
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
		जर (screen_opacity_popup != रिक्त && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    जर (Screen_ओळी[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_ओळी_UC[off - 1] != 0
			    && utf_char2cells(Screen_ओळी_UC[off - 1]) == 2)
			जा next_col;
		}
		जर (screen_opacity_popup != रिक्त && c == ' '
						       && Screen_ओळी[off] != 0)
		{
		    पूर्णांक bg_char_cells = 1;
		    जर (enc_utf8 && Screen_ओळी_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_ओळी_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    जर (bg_char_cells == 2)
		    {
			जर (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    जा skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			जर (c2 != ' ')
			    जा skip_opacity_fill;
		    }

		    पूर्णांक popup_attr = get_win_attr(screen_opacity_popup);
		    पूर्णांक blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    जा next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		जर (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != रिक्त
			&& row >= pum_bg_शीर्ष && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    पूर्णांक soff = (row - pum_bg_शीर्ष) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    जर (enc_utf8 && pum_bg_ओळी_UC != रिक्त
			    && col > start_col
			    && pum_bg_ओळी_UC[soff] == 0
			    && pum_bg_ओळी_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_ओळी_UC[soff - 1]) == 2)
		    {
			Screen_ओळी[off] = 0;
			जर (Screen_ओळी_UC != रिक्त)
			    Screen_ओळी_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			जा next_col;
		    }

		    पूर्णांक underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    जर (enc_utf8 && pum_bg_ओळी_UC != रिक्त
			    && ((pum_bg_ओळी_UC[soff] != 0
				    && utf_char2cells(pum_bg_ओळी_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_ओळी_UC[soff] == 0
				    && col > 0
				    && pum_bg_ओळी_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_ओळी_UC[soff - 1])
								       == 2)))
		    {
			Screen_ओळी[off] = ' ';
			जर (Screen_ओळी_UC != रिक्त)
			    Screen_ओळी_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			जा next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_ओळी[off] = pum_bg_ओळी[soff];
		    जर (enc_utf8 && pum_bg_ओळी_UC != रिक्त
						&& Screen_ओळी_UC != रिक्त)
		    {
			पूर्णांक k;
			Screen_ओळी_UC[off] = pum_bg_ओळी_UC[soff];
			साठी (k = 0; k < MAX_MCO; ++k)
			    जर (pum_bg_ओळी_C[k] != रिक्त
						    && Screen_ओळी_C[k] != रिक्त)
				Screen_ओळी_C[k][off] = pum_bg_ओळी_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    जा next_col;
		}
#जर defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		जर (
# ifdef FEAT_GUI
			gui.in_use
# endif
# जर defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    जर (Screen_ओळी[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    अन्यथा
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		जर (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_ओळी[off] = c;
		जर (enc_utf8)
		{
		    जर (c >= 0x80)
		    {
			Screen_ओळी_UC[off] = c;
			Screen_ओळी_C[0][off] = 0;
			Screen_ओळी[off] = 0x80; // avoid storing zero
		    }
		    अन्यथा
			Screen_ओळी_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		जर (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    जर (col == start_col)
	    {
		जर (did_delete)
		    थांबा;
		c = c2;
	    }
	}
	जर (end_col == Columns)
	    ओळ_Wraps[row] = FALSE;
	जर (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    जर (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    जर (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    शून्य
check_for_delay(पूर्णांक check_संदेश_scroll)
{
    जर ((emsg_on_display || (check_संदेश_scroll && संदेश_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	जर (check_संदेश_scroll)
	    संदेश_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    स्थिर शून्य
clear_TabPageIdxs(शून्य)
{
    पूर्णांक		scol;

    साठी (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    पूर्णांक
screen_valid(पूर्णांक doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    परत (Screen_ओळी != रिक्त);
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
screenalloc(पूर्णांक doclear)
{
    पूर्णांक		    new_row, old_row;
#ifdef FEAT_GUI
    पूर्णांक		    old_Rows;
#endif
    win_T	    *wp;
    पूर्णांक		    outofmem = FALSE;
    पूर्णांक		    len;
    schar_T	    *new_Screen_ओळी;
    u8char_T	    *new_Screen_ओळी_UC = रिक्त;
    u8char_T	    *new_Screen_ओळी_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = रिक्त;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    अचिन्हित	    *new_ओळ_Offset;
    char_u	    *new_ओळ_Wraps;
    लघु	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    लघु	    *new_popup_mask;
    लघु	    *new_popup_mask_next;
    अक्षर	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    स्थिर पूर्णांक	    entered = FALSE;		// avoid recursiveness
    स्थिर पूर्णांक	    done_outofmem_संदेश = FALSE;	// did outofmem message
    पूर्णांक		    retry_मोजणी = 0;
    पूर्णांक		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    जर ((Screen_ओळी != रिक्त
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_ओळी_UC != रिक्त)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != रिक्त)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_ओळी == रिक्त))
	परत;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    जर (entered)
	परत;
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
    साठी (पूर्णांक i = 0; i < AUCMD_WIN_मोजणी; ++i)
	जर (aucmd_win[i].auc_win != रिक्त)
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

    new_Screen_ओळी = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_ओळी_C, 0, आकार(u8char_T *) * MAX_MCO);
    जर (enc_utf8)
    {
	new_Screen_ओळी_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	साठी (पूर्णांक i = 0; i < p_mco; ++i)
	    new_Screen_ओळी_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    जर (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_ओळ_Offset = LALLOC_MULT(अचिन्हित, Rows);
    new_ओळ_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(लघु, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(लघु, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(लघु, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(अक्षर, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जर (win_alloc_ओळी(wp) == FAIL)
	{
	    outofmem = TRUE;
	    जा give_up;
	}
    }
    साठी (पूर्णांक i = 0; i < AUCMD_WIN_मोजणी; ++i)
	जर (aucmd_win[i].auc_win != रिक्त
		&& aucmd_win[i].auc_win->w_ओळी == रिक्त
		&& win_alloc_ओळी(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    थांबा;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	जर (win_alloc_ओळी(wp) == FAIL)
	{
	    outofmem = TRUE;
	    जा give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    जर (win_alloc_ओळी(wp) == FAIL)
	    {
		outofmem = TRUE;
		जा give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    साठी (पूर्णांक i = 0; i < p_mco; ++i)
	जर (new_Screen_ओळी_C[i] == रिक्त)
	{
	    found_null = TRUE;
	    थांबा;
	}
    जर (new_Screen_ओळी == रिक्त
	    || (enc_utf8 && (new_Screen_ओळी_UC == रिक्त || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == रिक्त)
	    || new_ScreenAttrs == रिक्त
	    || new_ScreenCols == रिक्त
	    || new_ओळ_Offset == रिक्त
	    || new_ओळ_Wraps == रिक्त
	    || new_TabPageIdxs == रिक्त
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == रिक्त
	    || new_popup_mask_next == रिक्त
	    || new_popup_transparent == रिक्त
#endif
	    || outofmem)
    {
	जर (Screen_ओळी != रिक्त || !done_outofmem_संदेश)
	{
	    // guess the size
	    do_outofmem_संदेश((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_संदेश = TRUE;
	}
	VIM_CLEAR(new_Screen_ओळी);
	VIM_CLEAR(new_Screen_ओळी_UC);
	साठी (पूर्णांक i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_ओळी_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_ओळ_Offset);
	VIM_CLEAR(new_ओळ_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    अन्यथा
    {
	done_outofmem_संदेश = FALSE;

	साठी (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_ओळ_Offset[new_row] = new_row * Columns;
	    new_ओळ_Wraps[new_row] = FALSE;

	    (शून्य)vim_memset(new_Screen_ओळी + new_row * Columns,
				  ' ', (माप_प्रकार)Columns * आकार(schar_T));
	    जर (enc_utf8)
	    {
		(शून्य)vim_memset(new_Screen_ओळी_UC + new_row * Columns,
				   0, (माप_प्रकार)Columns * आकार(u8char_T));
		साठी (पूर्णांक i = 0; i < p_mco; ++i)
		    (शून्य)vim_memset(new_Screen_ओळी_C[i]
						      + new_row * Columns,
				   0, (माप_प्रकार)Columns * आकार(u8char_T));
	    }
	    जर (enc_dbcs == DBCS_JPNU)
		(शून्य)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (माप_प्रकार)Columns * आकार(schar_T));
	    (शून्य)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (माप_प्रकार)Columns * आकार(sattr_T));
	    (शून्य)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (माप_प्रकार)Columns * आकार(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    जर (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		जर (old_row >= 0 && Screen_ओळी != रिक्त)
		{
		    जर (screen_Columns < Columns)
			len = screen_Columns;
		    अन्यथा
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    जर (!(enc_utf8 && Screen_ओळी_UC == रिक्त)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_ओळी + new_ओळ_Offset[new_row],
				Screen_ओळी + ओळ_Offset[old_row],
				(माप_प्रकार)len * आकार(schar_T));
		    जर (enc_utf8 && Screen_ओळी_UC != रिक्त
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_ओळी_UC + new_ओळ_Offset[new_row],
				Screen_ओळी_UC + ओळ_Offset[old_row],
				(माप_प्रकार)len * आकार(u8char_T));
			साठी (पूर्णांक i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_ओळी_C[i]
						    + new_ओळ_Offset[new_row],
				Screen_ओळी_C[i] + ओळ_Offset[old_row],
				(माप_प्रकार)len * आकार(u8char_T));
		    }
		    जर (enc_dbcs == DBCS_JPNU && ScreenLines2 != रिक्त)
			mch_memmove(new_ScreenLines2 + new_ओळ_Offset[new_row],
				ScreenLines2 + ओळ_Offset[old_row],
				(माप_प्रकार)len * आकार(schar_T));
		    mch_memmove(new_ScreenAttrs + new_ओळ_Offset[new_row],
			    ScreenAttrs + ओळ_Offset[old_row],
			    (माप_प्रकार)len * आकार(sattr_T));
		    mch_memmove(new_ScreenCols + new_ओळ_Offset[new_row],
			    ScreenCols + ओळ_Offset[old_row],
			    (माप_प्रकार)len * आकार(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_ओळ = new_Screen_ओळी + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * आकार(लघु));
	vim_memset(new_popup_transparent, 0, Rows * Columns * आकार(अक्षर));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_ओळी = new_Screen_ओळी;
    Screen_ओळी_UC = new_Screen_ओळी_UC;
    साठी (पूर्णांक i = 0; i < p_mco; ++i)
	Screen_ओळी_C[i] = new_Screen_ओळी_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    ओळ_Offset = new_ओळ_Offset;
    ओळ_Wraps = new_ओळ_Wraps;
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
    जर (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    जर (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    अन्यथा जर (gui.in_use
	    && !gui.starting
	    && Screen_ओळी != रिक्त
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (पूर्णांक)Rows - 1, (पूर्णांक)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	जर (संदेश_row >= Rows)		// Rows got smaller
	    संदेश_row = Rows - 1;		// put cursor at last row
	अन्यथा जर (Rows > old_Rows)	// Rows got bigger
	    संदेश_row += Rows - old_Rows; // put cursor in same place
	जर (संदेश_col >= cmdline_width)	// cmdline_width got smaller
	    संदेश_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    जर (starting == 0 && ++retry_मोजणी <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, रिक्त, रिक्त, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	जा retry;
    }
}

    शून्य
free_screenlines(शून्य)
{
    पूर्णांक		i;

    VIM_CLEAR(Screen_ओळी_UC);
    साठी (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_ओळी_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_ओळी);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(ओळ_Offset);
    VIM_CLEAR(ओळ_Wraps);
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
    पूर्णांक
screenclear(शून्य)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    परत screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    शून्य
redraw_as_cleared(शून्य)
{
    screenclear2(FALSE);
}

    स्थिर पूर्णांक
screenclear2(पूर्णांक doclear)
{
    पूर्णांक	    i;
    पूर्णांक	    did_clear = FALSE;

    जर (starting == NO_SCREEN || Screen_ओळी == रिक्त
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	परत FALSE;

#ifdef FEAT_GUI
    जर (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    साठी (i = 0; i < Rows; ++i)
    {
	lineclear(ओळ_Offset[i], (पूर्णांक)Columns, 0);
	ओळ_Wraps[i] = FALSE;
    }

    जर (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	जर (suppressed_cells != रिक्त)
	    vim_memset(suppressed_cells, 0,
			       (माप_प्रकार)suppressed_rows * suppressed_cols);
#endif
    }
    अन्यथा
    {
	// can't clear the screen, mark all chars with invalid attributes
	साठी (i = 0; i < Rows; ++i)
	    lineinvalid(ओळ_Offset[i], (पूर्णांक)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    जर (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    संदेश_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    संदेश_row = cmdline_row;	// put cursor on last line for messages
    संदेश_col = 0;
    screen_start();		// don't know where cursor is now
    संदेश_didany = FALSE;
    संदेश_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    परत did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    स्थिर शून्य
lineclear(अचिन्हित off, पूर्णांक width, पूर्णांक attr)
{
    (शून्य)vim_memset(Screen_ओळी + off, ' ', (माप_प्रकार)width * आकार(schar_T));
    जर (enc_utf8)
	(शून्य)vim_memset(Screen_ओळी_UC + off, 0,
					  (माप_प्रकार)width * आकार(u8char_T));
    (शून्य)vim_memset(ScreenAttrs + off, attr, (माप_प्रकार)width * आकार(sattr_T));
    (शून्य)vim_memset(ScreenCols + off, -1, (माप_प्रकार)width * आकार(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    स्थिर शून्य
lineinvalid(अचिन्हित off, पूर्णांक width)
{
    (शून्य)vim_memset(ScreenAttrs + off, -1, (माप_प्रकार)width * आकार(sattr_T));
    (शून्य)vim_memset(ScreenCols + off, -1, (माप_प्रकार)width * आकार(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    शून्य
ओळ_was_clobbered(पूर्णांक screen_lnum)
{
    lineinvalid(ओळ_Offset[screen_lnum], (पूर्णांक)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    स्थिर शून्य
linecopy(पूर्णांक to, पूर्णांक from, win_T *wp)
{
    अचिन्हित	off_to = ओळ_Offset[to] + wp->w_wincol;
    अचिन्हित	off_from = ओळ_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_ओळी + off_to, Screen_ओळी + off_from,
	    wp->w_width * आकार(schar_T));
    जर (enc_utf8)
    {
	पूर्णांक	i;

	mch_memmove(Screen_ओळी_UC + off_to, Screen_ओळी_UC + off_from,
		wp->w_width * आकार(u8char_T));
	साठी (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_ओळी_C[i] + off_to, Screen_ओळी_C[i] + off_from,
		    wp->w_width * आकार(u8char_T));
    }
    जर (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * आकार(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * आकार(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * आकार(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    पूर्णांक
can_clear(char_u *p)
{
    परत (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#अन्यथा
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
windgoto(पूर्णांक row, पूर्णांक col)
{
    sattr_T	    *p;
    पूर्णांक		    i;
    पूर्णांक		    plan;
    पूर्णांक		    cost;
    पूर्णांक		    wouldbe_col;
    पूर्णांक		    noinvcurs;
    char_u	    *bs;
    पूर्णांक		    goto_cost;
    पूर्णांक		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    जर (Screen_ओळी == रिक्त)
	परत;
    जर (col == screen_cur_col && row == screen_cur_row)
	परत;

    // Check for valid position.
    जर (row < 0)	// window without text lines?
	row = 0;
    जर (row >= screen_Rows)
	row = screen_Rows - 1;
    जर (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    जर (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    अन्यथा
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
    जर (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = रिक्त;			    // init for GCC
	attr = screen_attr;
	जर (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    जर (*T_LE)
		bs = T_LE;		    // "cursor left"
	    अन्यथा
		bs = T_BC;		    // "backspace character (old)
	    जर (*bs)
		cost = (screen_cur_col - col) * (पूर्णांक)STRLEN(bs);
	    अन्यथा
		cost = 999;
	    जर (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    अन्यथा
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    जर (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	अन्यथा जर (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    जर (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	अन्यथा
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
	जर (i > 0)
	    cost += i;
	जर (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + ओळ_Offset[row] + wouldbe_col;
	    पर्यंत (i && *p++ == attr)
		--i;
	    जर (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		जर (*--p == 0)
		{
		    cost += noinvcurs;
		    पर्यंत (i && *p++ == 0)
			--i;
		}
		जर (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    जर (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		साठी (i = wouldbe_col; i < col; ++i)
		    जर (Screen_ओळी_UC[ओळ_Offset[row] + i] != 0)
		    {
			cost = 999;
			थांबा;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    जर (cost < 999)
		साठी (i = wouldbe_col; i < col; ++i)
		    जर (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			थांबा;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	जर (cost < goto_cost)
	{
	    जर (plan == PLAN_LE)
	    {
		जर (noinvcurs)
		    screen_stop_highlight();
		पर्यंत (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    अन्यथा जर (plan == PLAN_CR)
	    {
		जर (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    अन्यथा जर (plan == PLAN_NL)
	    {
		जर (noinvcurs)
		    screen_stop_highlight();
		पर्यंत (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    जर (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		जर (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    पर्यंत (i-- > 0)
			out_char(*T_ND);
		}
		अन्यथा
		{
		    पूर्णांक	off;

		    off = ओळ_Offset[row] + screen_cur_col;
		    पर्यंत (i-- > 0)
		    {
			जर (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_ओळी[off]);
			जर (enc_dbcs == DBCS_JPNU
				&& Screen_ओळी[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    अन्यथा
	cost = 999;

    जर (cost >= goto_cost)
    {
	जर (noinvcurs)
	    screen_stop_highlight();
	जर (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_उजवा(col - screen_cur_col);
	अन्यथा
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
    पूर्णांक
cursor_screen_cells(शून्य)
{
    परत has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    शून्य
setcursor_mayforce(पूर्णांक force)
{
    जर (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((पूर्णांक)curwin->w_width - curwin->w_wcol
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
    पूर्णांक
win_ins_ओळी(
    win_T	*wp,
    पूर्णांक		row,
    पूर्णांक		ओळ_मोजणी,
    पूर्णांक		invalid,
    पूर्णांक		mayclear)
{
    पूर्णांक		did_delete;
    पूर्णांक		nextrow;
    पूर्णांक		lastrow;
    पूर्णांक		retval;

    जर (invalid)
	wp->w_ओळी_valid = 0;

    // with only a few lines it's not worth the effort
    जर (wp->w_height < 5)
	परत FAIL;

    // with the popup menu visible this might not work correctly
    जर (pum_visible())
	परत FAIL;

    जर (ओळ_मोजणी > wp->w_height - row)
	ओळ_मोजणी = wp->w_height - row;

    retval = win_do_ओळी(wp, row, ओळ_मोजणी, mayclear, FALSE, 0);
    जर (retval != MAYBE)
	परत retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    जर (wp->w_next != रिक्त || wp->w_status_height)
    {
	जर (screen_del_ओळी(0, W_WINROW(wp) + wp->w_height - ओळ_मोजणी,
				  ओळ_मोजणी, (पूर्णांक)Rows, FALSE, 0, रिक्त) == OK)
	    did_delete = TRUE;
	अन्यथा जर (wp->w_next)
	    परत FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    जर (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + ओळ_मोजणी;
	जर (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - ओळ_मोजणी, lastrow - ओळ_मोजणी,
		  wp->w_wincol, (पूर्णांक)W_ENDCOL(wp), ' ', ' ', 0);
    }

    जर (screen_ins_ओळी(0, W_WINROW(wp) + row, ओळ_मोजणी, (पूर्णांक)Rows, 0, रिक्त)
								      == FAIL)
    {
	// deletion will have messed up other windows
	जर (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	परत FAIL;
    }

    परत OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    पूर्णांक
win_del_ओळी(
    win_T	*wp,
    पूर्णांक		row,
    पूर्णांक		ओळ_मोजणी,
    पूर्णांक		invalid,
    पूर्णांक		mayclear,
    पूर्णांक		clear_attr)	    // for clearing lines
{
    पूर्णांक		retval;

    जर (invalid)
	wp->w_ओळी_valid = 0;

    जर (ओळ_मोजणी > wp->w_height - row)
	ओळ_मोजणी = wp->w_height - row;

    retval = win_do_ओळी(wp, row, ओळ_मोजणी, mayclear, TRUE, clear_attr);
    जर (retval != MAYBE)
	परत retval;

    जर (screen_del_ओळी(0, W_WINROW(wp) + row, ओळ_मोजणी,
				   (पूर्णांक)Rows, FALSE, clear_attr, रिक्त) == FAIL)
	परत FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    जर (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	जर (screen_ins_ओळी(0, W_WINROW(wp) + wp->w_height - ओळ_मोजणी,
			      ओळ_मोजणी, (पूर्णांक)Rows, clear_attr, रिक्त) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    अन्यथा
	redraw_cmdline = TRUE;
    परत OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    स्थिर पूर्णांक
win_do_ओळी(
    win_T	*wp,
    पूर्णांक		row,
    पूर्णांक		ओळ_मोजणी,
    पूर्णांक		mayclear,
    पूर्णांक		del,
    पूर्णांक		clear_attr)
{
    पूर्णांक		retval;

    जर (!redrawing() || ओळ_मोजणी <= 0)
	परत FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    जर (no_win_do_ओळी_ins && !del)
	परत FAIL;

    // only a few lines left: redraw is faster
    जर (mayclear && Rows - ओळ_मोजणी < 5
	    && wp->w_width == topframe->fr_width)
    {
	जर (!no_win_do_ओळी_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	परत FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    जर (popup_visible)
	परत FAIL;
#endif

    // Delete all remaining lines
    जर (row + ओळ_मोजणी >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (पूर्णांक)W_ENDCOL(wp), ' ', ' ', 0);
	परत OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    जर (!no_win_do_ओळी_ins)
	clear_cmdline = TRUE;

#जर defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    जर (tabpanel_width() > 0)
	परत FAIL;
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
    जर (scroll_region || wp->w_width != topframe->fr_width)
    {
	जर (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	जर (del)
	    retval = screen_del_ओळी(W_WINROW(wp) + row, 0, ओळ_मोजणी,
				    wp->w_height - row, FALSE, clear_attr, wp);
	अन्यथा
	    retval = screen_ins_ओळी(W_WINROW(wp) + row, 0, ओळ_मोजणी,
					   wp->w_height - row, clear_attr, wp);
	जर (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	परत retval;
    }

    जर (wp->w_next != रिक्त && p_tf) // don't delete/insert on fast terminal
	परत FAIL;

    परत MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    स्थिर शून्य
win_rest_invalid(win_T *wp)
{
    पर्यंत (wp != रिक्त)
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
    पूर्णांक
screen_ins_ओळी(
    पूर्णांक		off,
    पूर्णांक		row,
    पूर्णांक		ओळ_मोजणी,
    पूर्णांक		end,
    पूर्णांक		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    पूर्णांक		i;
    पूर्णांक		j;
    अचिन्हित	temp;
    पूर्णांक		cursor_row;
    पूर्णांक		cursor_col = 0;
    पूर्णांक		type;
    पूर्णांक		result_empty;
    पूर्णांक		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    जर (!screen_valid(TRUE)
	     || ओळ_मोजणी <= 0 || ओळ_मोजणी > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	परत FAIL;

#ifdef FEAT_PROP_POPUP
    जर (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, ओळ_मोजणी);
# endif
	परत FAIL;
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
    result_empty = (row + ओळ_मोजणी >= end);
    जर (wp != रिक्त && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	जर (ओळ_मोजणी > 3)
	    परत FAIL;
	type = USE_REDRAW;
    }
    अन्यथा जर (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    अन्यथा जर (*T_CAL != NUL && (ओळ_मोजणी > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    अन्यथा जर (*T_CDL != NUL && result_empty && (ओळ_मोजणी > 1 || !can_ce))
	type = USE_T_CDL;
    अन्यथा जर (*T_AL != NUL)
	type = USE_T_AL;
    अन्यथा जर (can_ce && result_empty)
	type = USE_T_CE;
    अन्यथा जर (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    अन्यथा जर (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    अन्यथा
	परत FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    जर (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	परत screen_del_ओळी(off, row, ओळ_मोजणी, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    जर (*T_DB)
	screen_del_ओळी(off, end - ओळ_मोजणी, ओळ_मोजणी, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    जर (off + row > 0 || (wp != रिक्त && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    अन्यथा
	clip_scroll_selection(-ओळ_मोजणी);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, ओळ_मोजणी);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    जर (wp != रिक्त && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    जर (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    अन्यथा
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    साठी (i = 0; i < ओळ_मोजणी; ++i)
    {
	जर (wp != रिक्त && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    पर्यंत ((j -= ओळ_मोजणी) >= row)
		linecopy(j + ओळ_मोजणी, j, wp);
	    j += ओळ_मोजणी;
	    जर (can_clear((char_u *)" "))
		lineclear(ओळ_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    अन्यथा
		lineinvalid(ओळ_Offset[j] + wp->w_wincol, wp->w_width);
	    ओळ_Wraps[j] = FALSE;
	}
	अन्यथा
	{
	    j = end - 1 - i;
	    temp = ओळ_Offset[j];
	    पर्यंत ((j -= ओळ_मोजणी) >= row)
	    {
		ओळ_Offset[j + ओळ_मोजणी] = ओळ_Offset[j];
		ओळ_Wraps[j + ओळ_मोजणी] = ओळ_Wraps[j];
	    }
	    ओळ_Offset[j + ओळ_मोजणी] = temp;
	    ओळ_Wraps[j + ओळ_मोजणी] = FALSE;
	    जर (can_clear((char_u *)" "))
		lineclear(temp, (पूर्णांक)Columns, clear_attr);
	    अन्यथा
		lineinvalid(temp, (पूर्णांक)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    जर (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    जर (type == USE_REDRAW)
	redraw_block(row, end, wp);
    अन्यथा जर (type == USE_T_CAL)
    {
	term_append_ओळी(ओळ_मोजणी);
	screen_start();		// don't know where cursor is now
    }
    अन्यथा
    {
	साठी (i = 0; i < ओळ_मोजणी; i++)
	{
	    जर (type == USE_T_AL)
	    {
		जर (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    अन्यथा  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    जर (type == USE_T_SR && *T_DA)
    {
	साठी (i = 0; i < ओळ_मोजणी; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    जर (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    परत OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    पूर्णांक
screen_del_ओळी(
    पूर्णांक		off,
    पूर्णांक		row,
    पूर्णांक		ओळ_मोजणी,
    पूर्णांक		end,
    पूर्णांक		force,		// even when line_count > p_ttyscroll
    पूर्णांक		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    पूर्णांक		j;
    पूर्णांक		i;
    अचिन्हित	temp;
    पूर्णांक		cursor_row;
    पूर्णांक		cursor_col = 0;
    पूर्णांक		cursor_end;
    पूर्णांक		result_empty;	// result is empty until end of region
    पूर्णांक		can_delete;	// deleting line codes can be used
    पूर्णांक		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    जर (!screen_valid(TRUE)
	    || ओळ_मोजणी <= 0
	    || (!force && ओळ_मोजणी > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	परत FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + ओळ_मोजणी >= end;

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
    जर (wp != रिक्त && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	जर (ओळ_मोजणी > 3)
	    परत FAIL;
	type = USE_REDRAW;
    }
    अन्यथा जर (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    अन्यथा जर (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    ओळ_मोजणी == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    अन्यथा जर (*T_CDL != NUL && ओळ_मोजणी > 1 && can_delete)
	type = USE_T_CDL;
    अन्यथा जर (can_clear(T_CE) && result_empty
	    && (wp == रिक्त || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    अन्यथा जर (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    अन्यथा जर (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    अन्यथा
	परत FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    जर (off + row > 0 || (wp != रिक्त && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    अन्यथा
	clip_scroll_selection(ओळ_मोजणी);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -ओळ_मोजणी);
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

    जर (wp != रिक्त && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    जर (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    अन्यथा
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
    साठी (i = 0; i < ओळ_मोजणी; ++i)
    {
	जर (wp != रिक्त && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    पर्यंत ((j += ओळ_मोजणी) <= end - 1)
		linecopy(j - ओळ_मोजणी, j, wp);
	    j -= ओळ_मोजणी;
	    जर (can_clear((char_u *)" "))
		lineclear(ओळ_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    अन्यथा
		lineinvalid(ओळ_Offset[j] + wp->w_wincol, wp->w_width);
	    ओळ_Wraps[j] = FALSE;
	}
	अन्यथा
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = ओळ_Offset[j];
	    पर्यंत ((j += ओळ_मोजणी) <= end - 1)
	    {
		ओळ_Offset[j - ओळ_मोजणी] = ओळ_Offset[j];
		ओळ_Wraps[j - ओळ_मोजणी] = ओळ_Wraps[j];
	    }
	    ओळ_Offset[j - ओळ_मोजणी] = temp;
	    ओळ_Wraps[j - ओळ_मोजणी] = FALSE;
	    जर (can_clear((char_u *)" "))
		lineclear(temp, (पूर्णांक)Columns, clear_attr);
	    अन्यथा
		lineinvalid(temp, (पूर्णांक)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    जर (screen_attr != clear_attr)
	screen_stop_highlight();
    जर (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    जर (type == USE_REDRAW)
	redraw_block(row, end, wp);
    अन्यथा जर (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    अन्यथा जर (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_ओळी(ओळ_मोजणी);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    अन्यथा जर (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	साठी (i = ओळ_मोजणी; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    अन्यथा
    {
	साठी (i = ओळ_मोजणी; --i >= 0; )
	{
	    जर (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    अन्यथा // type == USE_T_CE
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
    जर (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	साठी (i = ओळ_मोजणी; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    जर (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    परत OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    पूर्णांक
skip_showmode(शून्य)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    जर (global_busy
	    || संदेश_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !किल्ली_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	परत TRUE;
    }
    परत FALSE;
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
    पूर्णांक
showmode(शून्य)
{
    पूर्णांक		need_clear;
    पूर्णांक		length = 0;
    पूर्णांक		do_mode;
    पूर्णांक		attr;
    पूर्णांक		nwr_save;
    पूर्णांक		sub_attr;
    पूर्णांक		show_ruler_with_pum = FALSE;

    do_mode = p_smd && संदेश_silent == 0
	    && ((State & MODE_घाला)
		|| restart_edit != NUL
		|| VIsual_active);
    जर (do_mode || reg_recording != 0)
    {
	जर (skip_showmode())
	    परत 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	जर (clear_cmdline && cmdline_row < Rows - 1)
	    संदेश_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	संदेश_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	जर (do_mode)
	{
	    संदेश_puts_attr("--", attr);
#जर defined(FEAT_XIM)
	    जर (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# अन्यथा
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		संदेश_puts_attr(" IM", attr);
# अन्यथा
		संदेश_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    जर (edit_submode != रिक्त && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - संदेश_row) * cmdline_width - 3;
		जर (edit_submode_extra != रिक्त)
		    length -= vim_strsize(edit_submode_extra);
		जर (length > 0)
		{
		    जर (edit_submode_pre != रिक्त)
			length -= vim_strsize(edit_submode_pre);
		    जर (length - vim_strsize(edit_submode) > 0)
		    {
			जर (edit_submode_pre != रिक्त)
			    संदेश_puts_attr((अक्षर *)edit_submode_pre, attr);
			संदेश_puts_attr((अक्षर *)edit_submode, attr);
		    }
		    जर (edit_submode_extra != रिक्त)
		    {
			संदेश_puts_attr(" ", attr);  // add a space in between
			जर ((पूर्णांक)edit_submode_highl < (पूर्णांक)HLF_मोजणी)
			    sub_attr = HL_ATTR(edit_submode_highl);
			अन्यथा
			    sub_attr = attr;
			संदेश_puts_attr((अक्षर *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    अन्यथा
	    {
		जर (State & VREPLACE_FLAG)
		    संदेश_puts_attr(_(" VREPLACE"), attr);
		अन्यथा जर (State & REPLACE_FLAG)
		    संदेश_puts_attr(_(" REPLACE"), attr);
		अन्यथा जर (State & MODE_घाला)
		{
#ifdef FEAT_RIGHTLEFT
		    जर (p_ri)
			संदेश_puts_attr(_(" REVERSE"), attr);
#endif
		    संदेश_puts_attr(_(" INSERT"), attr);
		}
		अन्यथा जर (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    संदेश_puts_attr(_(" (insert)"), attr);
		अन्यथा जर (restart_edit == 'R')
		    संदेश_puts_attr(_(" (replace)"), attr);
		अन्यथा जर (restart_edit == 'V')
		    संदेश_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		जर (p_hkmap)
		    संदेश_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		जर (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    जर (curwin->w_p_arab)
			संदेश_puts_attr(_(" Arabic"), attr);
		    अन्यथा
# endif
			जर (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    संदेश_puts_attr((अक्षर *)NameBuff, attr);
		}
#endif
		जर ((State & MODE_घाला) && p_paste)
		    संदेश_puts_attr(_(" (paste)"), attr);

		जर (VIsual_active)
		{
		    अक्षर *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    निवड ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			स्थिती 0:	p = N_(" VISUAL"); थांबा;
			स्थिती 1: p = N_(" VISUAL LINE"); थांबा;
			स्थिती 2: p = N_(" VISUAL BLOCK"); थांबा;
			स्थिती 4: p = N_(" SELECT"); थांबा;
			स्थिती 5: p = N_(" SELECT LINE"); थांबा;
			पूर्वनिर्धारित: p = N_(" SELECT BLOCK"); थांबा;
		    }
		    संदेश_puts_attr(_(p), attr);
		}
		संदेश_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	जर (reg_recording != 0
		&& edit_submode == रिक्त)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	जर (need_clear || clear_cmdline || redraw_mode)
	    संदेश_clr_eos();
	संदेश_didout = FALSE;		// overwrite this message
	length = संदेश_col;
	संदेश_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    अन्यथा जर (clear_cmdline && संदेश_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	संदेश_clr_cmdline();
    अन्यथा जर (redraw_mode)
    {
	संदेश_pos_mode();
	संदेश_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    जर (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    जर (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    परत length;
}

/*
 * Position for a mode message.
 */
    स्थिर शून्य
संदेश_pos_mode(शून्य)
{
    संदेश_col = 0;
    संदेश_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    शून्य
unshowmode(पूर्णांक force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    जर (!redrawing() || (!force && char_avail() && !किल्ली_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    अन्यथा
	clearmode();
}

/*
 * Clear the mode message.
 */
    शून्य
clearmode(शून्य)
{
    पूर्णांक save_संदेश_row = संदेश_row;
    पूर्णांक save_संदेश_col = संदेश_col;

    संदेश_pos_mode();
    जर (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    संदेश_clr_eos();

    संदेश_col = save_संदेश_col;
    संदेश_row = save_संदेश_row;
}

    स्थिर शून्य
recording_mode(पूर्णांक attr)
{
    संदेश_puts_attr(_("recording"), attr);
    जर (shortmess(SHM_RECORDING))
	परत;

    अक्षर s[4];

    प्र_लिखो(s, " @%c", reg_recording);
    संदेश_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    स्थिर पूर्णांक
mouse_has_any(शून्य)
{
    परत mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_घाला)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    शून्य
draw_tabline(शून्य)
{
    पूर्णांक		tabcount = 0;
    tabpage_T	*tp;
    पूर्णांक		tabwidth;
    पूर्णांक		col = 0;
    पूर्णांक		scol = 0;
    पूर्णांक		attr;
    win_T	*wp;
    win_T	*cwp;
    पूर्णांक		wincount;
    पूर्णांक		modified;
    पूर्णांक		c;
    पूर्णांक		len;
    पूर्णांक		attr_sel;
    पूर्णांक		attr_nosel = 0;
    पूर्णांक		attr_fill = 0;
    char_u	*p;
    पूर्णांक		room;
    पूर्णांक		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#जर defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    जर (Screen_ओळी == रिक्त)
	परत;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    जर (gui_use_tabline())
    {
	gui_update_tabline();
	परत;
    }
#endif

    जर (tabline_height() < 1)
	परत;

#जर defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    जर (*p_tal != NUL)
	win_redr_custom(रिक्त, FALSE);
    अन्यथा
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	जर (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	साठी (tp = first_tabpage; tp != रिक्त && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = ढकला_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    जर (tp->tp_topframe == topframe)
		attr = attr_sel;
	    जर (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    जर (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    जर (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    अन्यथा
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    साठी (wincount = 0; wp != रिक्त; wp = wp->w_next, ++wincount)
		जर (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    जर (modified || wincount > 1)
	    {
		जर (wincount > 1)
		{
		    len = vim_snprintf((अक्षर *)NameBuff, MAXPATHL, "%d", wincount);
		    जर (col + len >= Columns - 3)
			थांबा;
		    screen_puts_len(NameBuff, len, 0, col,
#जर defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#अन्यथा
					 attr
#endif
					       );
		    col += len;
		}
		जर (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    जर (room > 0)
	    {
		पूर्णांक n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		जर (has_mbyte)
		    पर्यंत (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		अन्यथा जर (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		जर (len > n)
		    len = n;

		screen_puts_len(p, (पूर्णांक)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    पर्यंत (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    जर (override_success)
		काढा_highlight_overrides();
	}

	जर (use_sep_chars)
	    c = '_';
	अन्यथा
	    c = ' ';
	screen_fill(0, 1, col, (पूर्णांक)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	जर (p_sc && *p_sloc == 't')
	{
	    पूर्णांक	n = (पूर्णांक)Columns - col - (tabcount > 1) * 3;
	    पूर्णांक	width = MIN(10, n);

	    जर (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (पूर्णांक)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	जर (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (पूर्णांक)Columns - 1, attr_nosel);
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
    जर (buf_spname(buf) != रिक्त)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    अन्यथा
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    पूर्णांक
fillchar_status(पूर्णांक *attr, win_T *wp)
{
    पूर्णांक	    fill;
    bool    override_success =
	ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    जर (bt_terminal(wp->w_buffer))
    {
	जर (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	अन्यथा
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    अन्यथा
#endif
    जर (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    अन्यथा
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    जर (override_success)
	काढा_highlight_overrides();
    परत fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    स्थिर bool
vsep_row_is_curwin(win_T *wp, पूर्णांक row)
{
    जर (wp == curwin)
	परत true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    जर (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	परत true;

    परत false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    पूर्णांक
fillchar_vsep(पूर्णांक *attr, win_T *wp, पूर्णांक row)
{
    bool override_success =
	ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);
    जर (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    अन्यथा
	*attr = HL_ATTR(HLF_CNC);
    जर (override_success)
	काढा_highlight_overrides();

    जर (*attr == 0 && wp->w_fill_chars.vert == ' ')
	परत '|';
    अन्यथा
	परत wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    पूर्णांक
redrawing(शून्य)
{
#ifdef FEAT_EVAL
    जर (disable_redraw_for_testing)
	परत 0;
    अन्यथा
#endif
	परत ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !किल्ली_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    पूर्णांक
messaging(शून्य)
{
    परत (!(p_lz && char_avail() && !किल्ली_Typed));
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
    पूर्णांक last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    जर (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#अन्यथा
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	जर (!last_has_status)
	    sc_col = ru_col;
    }
    जर (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	जर (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    जर (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    जर (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#जर defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    पूर्णांक
संख्या_width(win_T *wp)
{
    पूर्णांक		n;
    linenr_T	lnum;

    जर (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    अन्यथा
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;

    जर (lnum == wp->w_nrwidth_ओळ_मोजणी && wp->w_nuw_cached == wp->w_p_nuw)
	परत wp->w_nrwidth_width;
    wp->w_nrwidth_ओळ_मोजणी = lnum;

    n = 0;
    करा
    {
	lnum /= 10;
	++n;
    } पर्यंत (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    जर (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    जर (n < 2 && get_first_valid_sign(wp) != रिक्त
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    परत n;
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    पूर्णांक
screen_screencol(शून्य)
{
    परत screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    पूर्णांक
screen_screenrow(शून्य)
{
    परत screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    स्थिर पूर्णांक
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    जर (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T अंक = 0;
	पूर्णांक	    bytes;
	पूर्णांक	    n;

	साठी (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    जर (n < 0)
		परत 0;
	    अंक = अंक * 256 + n;
	}
	*p += 2;
	परत अंक;
    }
    परत mb_ptr2char_adv(p);
}

संरचना charstab
{
    पूर्णांक	    *cp;
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
#जर defined(FEAT_TABPANEL)
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
#अन्यथा
    CHARSTAB_ENTRY(रिक्त,		    "conceal"),
#endif
    CHARSTAB_ENTRY(रिक्त,		    "multispace"),
    CHARSTAB_ENTRY(रिक्त,		    "leadmultispace")
};

    स्थिर अक्षर *
field_मूल्य_err(अक्षर *errbuf, माप_प्रकार errbuflen, अक्षर *fmt, char_u *field)
{
    जर (errbuf == रिक्त)
	परत "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    परत errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    स्थिर अक्षर *
set_chars_option(win_T *wp, char_u *मूल्य, पूर्णांक is_listchars, पूर्णांक apply,
						अक्षर *errbuf, माप_प्रकार errbuflen)
{
    पूर्णांक	    round, i, entries;
    char_u  *p, *s;
    पूर्णांक	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = रिक्त;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = रिक्त; // Last occurrence of "leadmultispace:"
    पूर्णांक	    multispace_len = 0;	      // Length of lcs-multispace string
    पूर्णांक	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    संरचना charstab *tab;

    जर (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	जर (wp->w_p_lcs[0] == NUL)
	    मूल्य = p_lcs;  // local value is empty, use the global value
    }
    अन्यथा
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	जर (wp->w_p_fcs[0] == NUL)
	    मूल्य = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    साठी (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	पूर्णांक has_tab = FALSE, has_leadtab = FALSE;

	जर (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    जर (is_listchars)
	    {
		साठी (i = 0; i < entries; ++i)
		    जर (tab[i].cp != रिक्त)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		जर (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(पूर्णांक, multispace_len + 1);
		    जर (lcs_chars.multispace != रिक्त)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		अन्यथा
		    lcs_chars.multispace = रिक्त;

		जर (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(पूर्णांक, lead_multispace_len + 1);
		    जर (lcs_chars.leadmultispace != रिक्त)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		अन्यथा
		    lcs_chars.leadmultispace = रिक्त;
	    }
	    अन्यथा
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
#जर defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = मूल्य;
	पर्यंत (*p)
	{
	    साठी (i = 0; i < entries; ++i)
	    {
		जर (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    सुरूठेवा;

		s = p + tab[i].name.length + 1;

		जर (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    जर (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			पर्यंत (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जर (char2cells(c1) > 1)
				परत field_मूल्य_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			जर (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    परत field_मूल्य_err(errbuf, errbuflen,
				    e_wrong_संख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    अन्यथा
		    {
			पूर्णांक multispace_pos = 0;

			पर्यंत (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जर (p == last_multispace && lcs_chars.multispace != रिक्त)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    थांबा;
		}

		जर (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    जर (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			पर्यंत (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जर (char2cells(c1) > 1)
				परत field_मूल्य_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			जर (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    परत field_मूल्य_err(errbuf, errbuflen,
				    e_wrong_संख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    अन्यथा
		    {
			पूर्णांक multispace_pos = 0;

			पर्यंत (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    जर (p == last_lmultispace && lcs_chars.leadmultispace != रिक्त)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    थांबा;
		}

		c2 = c3 = 0;
		जर (*s == NUL)
		    परत field_मूल्य_err(errbuf, errbuflen,
				    e_wrong_संख्या_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		जर (char2cells(c1) > 1)
		    परत field_मूल्य_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		जर (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    जर (*s == NUL)
			परत field_मूल्य_err(errbuf, errbuflen,
				    e_wrong_संख्या_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    जर (char2cells(c2) > 1)
			परत field_मूल्य_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    जर (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			जर (char2cells(c3) > 1)
			    परत field_मूल्य_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    जर (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    अन्यथा  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		जर (*s == ',' || *s == NUL)
		{
		    जर (round > 0)
		    {
			जर (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			अन्यथा जर (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			अन्यथा जर (tab[i].cp != रिक्त)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    थांबा;
		}
		अन्यथा
		    परत field_मूल्य_err(errbuf, errbuflen,
				    e_wrong_संख्या_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    जर (i == entries)
		परत e_invalid_argument;

	    जर (*p == ',')
		++p;
	}

	जर (is_listchars && has_leadtab && !has_tab)
	    परत e_leadtab_requires_tab;
    }

    जर (apply)
    {
	जर (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	अन्यथा
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    परत रिक्त;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    अक्षर *
set_fillchars_option(win_T *wp, char_u *val, पूर्णांक apply, अक्षर *errbuf,
							      माप_प्रकार errbuflen)
{
    परत set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    अक्षर *
set_listchars_option(win_T *wp, char_u *val, पूर्णांक apply, अक्षर *errbuf,
							      माप_प्रकार errbuflen)
{
    परत set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(filltab))
	परत रिक्त;

    परत filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(lcstab))
	परत रिक्त;

    परत lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    अक्षर *
check_chars_options(शून्य)
{
    tabpage_T   *tp;
    win_T	    *wp;

    जर (set_listchars_option(curwin, p_lcs, FALSE, रिक्त, 0) != रिक्त)
	परत e_conflicts_with_मूल्य_of_listchars;
    जर (set_fillchars_option(curwin, p_fcs, FALSE, रिक्त, 0) != रिक्त)
	परत e_conflicts_with_मूल्य_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	जर (set_listchars_option(wp, wp->w_p_lcs, FALSE, रिक्त, 0) != रिक्त)
	    परत e_conflicts_with_मूल्य_of_listchars;
	जर (set_fillchars_option(wp, wp->w_p_fcs, FALSE, रिक्त, 0) != रिक्त)
	    परत e_conflicts_with_मूल्य_of_fillchars;
    }
    परत रिक्त;
}
