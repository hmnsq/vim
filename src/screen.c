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

#شامل "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
ایستا صحیح	screen_attr = 0;

ایستا تهی screen_char_2(unsigned off, صحیح row, صحیح col);
ایستا صحیح screenclear2(صحیح doclear);
ایستا تهی lineclear(unsigned off, صحیح width, صحیح attr);
ایستا تهی lineinvalid(unsigned off, صحیح width);
ایستا صحیح win_do_سطرها(win_T *wp, صحیح row, صحیح سطر_شمار, صحیح mayclear, صحیح del, صحیح clear_attr);
ایستا تهی win_rest_invalid(win_T *wp);
ایستا تهی پیام_pos_mode(تهی);
ایستا تهی recording_mode(صحیح attr);

// Ugly global: overrule attribute used by screen_char()
ایستا صحیح screen_char_attr = 0;

#اگر defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    صحیح
conceal_cursor_سطر(win_T *wp)
{
    صحیح		c;

    اگر (*wp->w_p_cocu == NUL)
	بازگشت FALSE;
    اگر (get_real_state() & MODE_VISUAL)
	c = 'v';
    وگرنه اگر (State & MODE_درج)
	c = 'i';
    وگرنه اگر (State & MODE_NORMAL)
	c = 'n';
    وگرنه اگر (State & MODE_CMDLINE)
	c = 'c';
    وگرنه
	بازگشت FALSE;
    بازگشت vim_strchr(wp->w_p_cocu, c) != پوچ;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    تهی
conceal_check_cursor_سطر(صحیح was_concealed)
{
    اگر (curwin->w_p_cole <= 0 || conceal_cursor_سطر(curwin) == was_concealed)
	بازگشت;

    صحیح wcol = curwin->w_wcol;

    need_cursor_سطر_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    اگر (!was_concealed)
	curwin->w_wcol = wcol;
}
#endif

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    صحیح
get_win_attr(win_T *wp)
{
    صحیح	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

    اگر (win_attr != 0)
    {
	اگر (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	وگرنه
	    win_attr = 0;
    }
#ifdef FEAT_PROP_POPUP
    وگرنه اگر (WIN_IS_POPUP(wp))
    {
	اگر (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	وگرنه
	    win_attr = HL_ATTR(HLF_برداشت);    // Popup
    }
#endif

    اگر (override_success)
	برداشت_highlight_overrides();
    بازگشت win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ایستا صحیح
screen_fill_end(
	win_T *wp,
	صحیح	c1,
	صحیح	c2,
	صحیح	off,
	صحیح	width,
	صحیح	row,
	صحیح	endrow,
	صحیح	attr)
{
    صحیح	    nn = off + width;

    اگر (nn > wp->w_width)
	nn = wp->w_width;
#ifdef FEAT_RIGHTLEFT
    اگر (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (صحیح)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    وگرنه
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (صحیح)wp->w_wincol + nn,
		c1, c2, attr);
    بازگشت nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    تهی
win_draw_end(
    win_T	*wp,
    صحیح		c1,
    صحیح		c2,
    صحیح		draw_margin,
    صحیح		row,
    صحیح		endrow,
    hlf_T	hl)
{
    صحیح		n = 0;
    صحیح		attr = HL_ATTR(hl);
    صحیح		win_attr = get_win_attr(wp);
    bool	override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    اگر (draw_margin)
    {
#ifdef FEAT_FOLDING
	صحیح	fdc = compute_foldcolumn(wp, 0);

	اگر (fdc > 0)
	    // draw the fold column
	    n = screen_fill_end(wp, ' ', ' ', n, fdc,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#endif
#ifdef FEAT_SIGNS
	اگر (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_end(wp, ' ', ' ', n, 2,
		      row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#endif
	اگر ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == پوچ)
	    // draw the number column
	    n = screen_fill_end(wp, ' ', ' ', n, عدد_width(wp) + 1,
		       row, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ifdef FEAT_RIGHTLEFT
    اگر (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    وگرنه
#endif
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (صحیح)W_ENDCOL(wp), c1, c2, attr);
    }

    set_empty_rows(wp, row);

    اگر (override_success)
	برداشت_highlight_overrides();
}

#اگر defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    صحیح
compute_foldcolumn(win_T *wp, صحیح col)
{
    صحیح wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    صحیح n = wp->w_width - (col + wmw);

    بازگشت MIN(wp->w_p_fdc, n);
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
    صحیح		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    صحیح		i = 0;
    صحیح		level;
    صحیح		first_level;
    صحیح		empty;
    صحیح		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    صحیح		نماد = 0;
    صحیح		len = 0;
    صحیح		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    empty = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    first_level = level - fdc - closed + 1 + empty;
    اگر (first_level < 1)
	first_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    برای (i = 0; i < n; i++)
    {
	اگر (win_foldinfo.fi_lnum == lnum
		&& first_level + i >= win_foldinfo.fi_کم_level)
	    نماد = wp->w_fill_chars.foldopen;
	وگرنه اگر (first_level == 1)
	    نماد = wp->w_fill_chars.foldsep;
	وگرنه اگر (wp->w_fill_chars.foldinner != NUL)
	    نماد = wp->w_fill_chars.foldinner;
	وگرنه اگر (first_level + i <= 9)
	    نماد = '0' + first_level + i;
	وگرنه
	    نماد = '>';

	len = utf_char2bytes(نماد, &p[byte_counter]);
	byte_counter += len;
	اگر (first_level + i >= level)
	{
	    i++;
	    بشکن;
	}
    }

    اگر (closed)
    {
	اگر (نماد != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    اگر (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	نماد = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(نماد, &p[byte_counter]);
	byte_counter += len;
    }

    بازگشت MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#endif // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ایستا صحیح
comp_char_differs(صحیح off_from, صحیح off_to)
{
    صحیح	    i;

    برای (i = 0; i < Screen_mco; ++i)
    {
	اگر (Screen_سطرها_C[i][off_from] != Screen_سطرها_C[i][off_to])
	    بازگشت TRUE;
	اگر (Screen_سطرها_C[i][off_from] == 0)
	    بشکن;
    }
    بازگشت FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ایستا صحیح
char_needs_redraw(صحیح off_from, صحیح off_to, صحیح cols)
{
    اگر (cols > 0
	    && ((Screen_سطرها[off_from] != Screen_سطرها[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_سطرها[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_سطرها[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && Screen_سطرها[off_from + 1]
						 != Screen_سطرها[off_to + 1])))
		|| (enc_utf8
		    && (Screen_سطرها_UC[off_from] != Screen_سطرها_UC[off_to]
			|| (Screen_سطرها_UC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && Screen_سطرها[off_from + 1]
						!= Screen_سطرها[off_to + 1])))))
	بازگشت TRUE;
    بازگشت FALSE;
}

#اگر defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    صحیح
screen_get_current_سطر_off(تهی)
{
    بازگشت (صحیح)(current_Screen_سطر - Screen_سطرها);
}
#endif

#ifdef FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ایستا صحیح
blocked_by_popup(صحیح row, صحیح col)
{
    صحیح off;

    اگر (!popup_visible)
	بازگشت FALSE;
    off = row * screen_Columns + col;
    بازگشت popup_mask[off] > screen_zindex || popup_transparent[off];
}
#endif

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    تهی
reset_screen_attr(تهی)
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    وگرنه
#endif
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ایستا صحیح
skip_for_popup(صحیح row, صحیح col)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    اگر (pum_under_menu(row, col, TRUE)
#ifdef FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#endif
	    )
	بازگشت TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ifdef FEAT_PROP_POPUP
    اگر (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	بازگشت TRUE;
#وگرنه
    اگر (pum_visible()
	    && (State & MODE_CMDLINE) == 0
	    && pum_under_menu(row, col, FALSE))
	بازگشت TRUE;
#endif
#ifdef FEAT_PROP_POPUP
    اگر (blocked_by_popup(row, col))
	بازگشت TRUE;
#endif
    بازگشت FALSE;
}

#ifdef FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ایستا char_u	*suppressed_cells = پوچ;
ایستا صحیح	suppressed_rows = 0;
ایستا صحیح	suppressed_cols = 0;

    ایستا تهی
mark_suppressed_cell(صحیح row, صحیح col)
{
    اگر (suppressed_cells == پوچ
	    || suppressed_rows != screen_Rows
	    || suppressed_cols != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_Rows * screen_Columns);
	اگر (suppressed_cells == پوچ)
	{
	    suppressed_rows = 0;
	    suppressed_cols = 0;
	    بازگشت;
	}
	suppressed_rows = screen_Rows;
	suppressed_cols = screen_Columns;
    }
    suppressed_cells[row * suppressed_cols + col] = TRUE;
}

    ایستا تهی
unmark_suppressed_cell(صحیح row, صحیح col)
{
    اگر (suppressed_cells != پوچ
	    && row >= 0 && row < suppressed_rows
	    && col >= 0 && col < suppressed_cols)
	suppressed_cells[row * suppressed_cols + col] = FALSE;
}

    ایستا صحیح
is_suppressed_cell(صحیح row, صحیح col)
{
    بازگشت suppressed_cells != پوچ
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
تعریفنوع ساختار {
    صحیح		popup_attr;	// get_win_attr(screen_opacity_popup)
    صحیح		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ایستا درونخطی تهی
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ایستا درونخطی صحیح
popup_base_attr_or(صحیح row, صحیح col, صحیح fallback)
{
    صحیح		base = fallback;

    popup_get_base_screen_cell(row, col, پوچ, &base, پوچ);
    بازگشت base;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ایستا درونخطی صحیح
popup_blend_with_base(
	ثابت popup_opacity_T	*po,
	صحیح			char_attr,
	صحیح			base_attr,
	صحیح			blend_fg)
{
    صحیح		combined = hl_combine_attr(po->popup_attr, char_attr);

    بازگشت hl_blend_attr(base_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ایستا تهی
resolve_wide_char_opacity_attrs(
	صحیح row, صحیح col1, صحیح col2,
	sattr_T *attr1, sattr_T *attr2)
{
    صحیح		bg1, bg2;
    صحیح		base1 = 0;
    صحیح		base2 = 0;
    attrentry_T	*ae;

    اگر (*attr1 == *attr2)
	بازگشت;

    popup_get_base_screen_cell(row, col1, پوچ, &base1, پوچ);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != پوچ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# وگرنه
    bg1 = (ae != پوچ && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_base_screen_cell(row, col2, پوچ, &base2, پوچ);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != پوچ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# وگرنه
    bg2 = (ae != پوچ && ae->ae_u.cterm.bg_color != 0);
# endif

    اگر (bg1 && !bg2)
	*attr1 = *attr2;
    وگرنه اگر (!bg1 && bg2)
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
    تهی
screen_سطر(
	win_T	*wp,
	صحیح	row,
	صحیح	coloff,
	صحیح	endcol,
	صحیح	clear_width,
	colnr_T	last_vcol,
	صحیح	flags UNUSED)
{
    unsigned	    off_from;
    unsigned	    off_to;
    unsigned	    max_off_from;
    unsigned	    max_off_to;
    صحیح		    col = 0;
    صحیح		    hl;
    صحیح		    force = FALSE;	// force update rest of the line
    صحیح		    redraw_this		// bool: does character need redraw?
#ifdef FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#endif
				;
    صحیح		    redraw_next;	// redraw_this for next character
#ifdef FEAT_GUI_MSWIN
    صحیح		    changed_this;	// TRUE if character changed
    صحیح		    changed_next;	// TRUE if next character changed
#endif
    صحیح		    clear_next = FALSE;
    صحیح		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ifdef FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    صحیح		    drawing_opacity_popup =
				screen_opacity_popup != پوچ
				&& (flags & SLF_POPUP);
    CLEAR_FIELD(po);
    اگر (drawing_opacity_popup)
	popup_opacity_init(&po);
#endif

    // Check for illegal row and col, just in case.
    اگر (row >= Rows)
	row = Rows - 1;
    اگر (endcol > Columns)
	endcol = Columns;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
#endif

    off_from = (unsigned)(current_Screen_سطر - Screen_سطرها);
    off_to = سطر_Offset[row] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_to = سطر_Offset[row] + screen_Columns;

#ifdef FEAT_RIGHTLEFT
    اگر (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	اگر (clear_width > 0)
	{
	    صحیح clear_start = col;

	    وقتی (col <= endcol && Screen_سطرها[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_سطرها_UC[off_to] == 0))
	    {
		++off_to;
		++col;
	    }
	    اگر (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    برای (صحیح i = endcol; i >= clear_start; i--)
		ScreenCols[off_to + (i - col)] =
		    (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
	}
	col = endcol + 1;
	off_to = سطر_Offset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif // FEAT_RIGHTLEFT

#ifdef FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    اگر (coloff > 0 && enc_utf8
		   && Screen_سطرها[off_to] == 0
		   && Screen_سطرها_UC[off_to - 1] != 0
		   && (*mb_char2cells)(Screen_سطرها_UC[off_to - 1]) > 1)
    {
	Screen_سطرها[off_to - 1] = ' ';
	Screen_سطرها_UC[off_to - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	اگر (screen_opacity_popup == پوچ)
# endif
	    screen_char(off_to - 1, row, col + coloff - 1);
    }
#endif

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);
#ifdef FEAT_GUI_MSWIN
    changed_next = redraw_next;
#endif

    وقتی (col < endcol)
    {
	اگر (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	وگرنه
	    char_cells = 1;

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + char_cells,
			      off_to + char_cells, endcol - col - char_cells);

#ifdef FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	اگر (!redraw_this && is_suppressed_cell(row, col + coloff))
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
	اگر (redraw_next && gui.in_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_to + char_cells];
	    اگر (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    اگر (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#endif
	// skip the second cell for double-width characters.
	اگر (redraw_this && char_cells == 2 && skip_for_popup(row, col + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	اگر (redraw_this && skip_for_popup(row, col + coloff))
	    redraw_this = FALSE;

#ifdef FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	صحیح opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	اگر (drawing_opacity_popup
		&& Screen_سطرها[off_from] == ' '
		&& (!enc_utf8 || Screen_سطرها_UC[off_from] == 0))
	{
	    صحیح popup_src_attr = ScreenAttrs[off_from];
	    صحیح bg_is_empty = Screen_سطرها[off_to] == 0
				&& (!enc_utf8 || Screen_سطرها_UC[off_to] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    اگر (bg_is_empty
		    && off_to > 0
		    && enc_utf8 && Screen_سطرها_UC[off_to - 1] != 0
		    && utf_char2cells(Screen_سطرها_UC[off_to - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    وگرنه اگر (Screen_سطرها[off_to] != 0
		    || (enc_utf8 && Screen_سطرها_UC[off_to] != 0))
	    {
		صحیح bg_char_cells = (enc_utf8 && Screen_سطرها_UC[off_to] != 0)
				? utf_char2cells(Screen_سطرها_UC[off_to]) : 1;

		اگر (bg_char_cells == 2)
		{
		    اگر (col + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_to + 1 >= max_off_to)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			صحیح base_attr;
			Screen_سطرها[off_to] = ' ';
			اگر (enc_utf8)
			    Screen_سطرها_UC[off_to] = 0;
			base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
			ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
			screen_char(off_to, row, col + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			برو skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    اگر (!(Screen_سطرها[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_سطرها_UC[off_from + 1] == 0)))
			برو skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		صحیح base_attr = popup_base_attr_or(row, col + coloff,
							     ScreenAttrs[off_to]);
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr, TRUE);
		screen_char(off_to, row, col + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		اگر (bg_char_cells == 2)
		{
		    صحیح base_attr2 = popup_base_attr_or(row, col + coloff + 1,
							 ScreenAttrs[off_to + 1]);
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base_attr2, TRUE);
		    اگر (po.blend == 100)
			resolve_wide_char_opacity_attrs(row,
				col + coloff, col + coloff + 1,
				&ScreenAttrs[off_to], &ScreenAttrs[off_to + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    وگرنه اگر (bg_is_empty)
	    {
		صحیح base_attr;
		Screen_سطرها[off_to] = ' ';
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

	اگر (redraw_this)
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
	    اگر (       p_wiv
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
		اگر (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		وگرنه
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    اگر (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		اگر (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_سطرها[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		وگرنه اگر (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_سطرها[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		اگر (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    اگر (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;

	    Screen_سطرها[off_to] = Screen_سطرها[off_from];
	    اگر (enc_utf8)
	    {
		Screen_سطرها_UC[off_to] = Screen_سطرها_UC[off_from];
		اگر (Screen_سطرها_UC[off_from] != 0)
		{
		    صحیح	    i;

		    برای (i = 0; i < Screen_mco; ++i)
			Screen_سطرها_C[i][off_to] = Screen_سطرها_C[i][off_from];
		}
	    }
	    اگر (char_cells == 2)
	    {
		Screen_سطرها[off_to + 1] = Screen_سطرها[off_from + 1];
		اگر (enc_utf8)
		    Screen_سطرها_UC[off_to + 1] = 0;
	    }

#اگر defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    اگر (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		اگر (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		اگر (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    اگر (gui.in_use && changed_this)
		redraw_next = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    اگر (gui.directx_enabled && changed_this)
		redraw_this = TRUE;
# endif
#endif

	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    اگر (drawing_opacity_popup && po.blend > 0)
	    {
		صحیح popup_src_attr = ScreenAttrs[off_from];
		صحیح scol1 = col + coloff;
		صحیح base1 = popup_base_attr_or(row, scol1, 0);

		اگر (char_cells == 2)
		{
		    صحیح base2 = popup_base_attr_or(row, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    صحیح over1 = popup_is_over_opacity(row, scol1);
		    صحیح over2 = popup_is_over_opacity(row, scol1 + 1);
		    اگر (over1 != over2)
		    {
			اگر (over1)
			    base1 = base2;
			وگرنه
			    base2 = base1;
		    }
		    ScreenAttrs[off_to + 1] = popup_blend_with_base(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_to] = popup_blend_with_base(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    وگرنه
#endif
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    اگر (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_to];

	    اگر (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    وگرنه
		screen_char(off_to, row, col + coloff);
	}
	وگرنه اگر (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0 && off_to > 0)
	{
	    اگر (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    وگرنه اگر (screen_attr != 0)
		screen_stop_highlight();
	}

#ifdef FEAT_PROP_POPUP
	اگر (!opacity_blank)
#endif
	{
	    ScreenCols[off_to] = ScreenCols[off_from];
	    اگر (char_cells == 2)
		ScreenCols[off_to + 1] = ScreenCols[off_from + 1];
	}

	off_to += char_cells;
	off_from += char_cells;
	col += char_cells;
    }

    اگر (clear_next && !skip_for_popup(row, col + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_سطرها[off_to] = ' ';
	اگر (enc_utf8)
	    Screen_سطرها_UC[off_to] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ifdef FEAT_PROP_POPUP
	اگر (screen_opacity_popup == پوچ)
#endif
	    screen_char(off_to, row, col + coloff);
    }

    اگر (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#endif
				   )
    {
#ifdef FEAT_GUI
	صحیح startCol = col;
#endif

	// blank out the rest of the line
	وقتی (col < clear_width && Screen_سطرها[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
				  && (!enc_utf8 || Screen_سطرها_UC[off_to] == 0)
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
	اگر (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    اگر (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		اگر (hl > HL_ALL || (hl & HL_BOLD))
		{
		    صحیح prev_cells = 1;

		    اگر (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_سطرها[off_to - 1] == 0 ? 2 : 1;
		    وگرنه اگر (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			unsigned off = سطر_Offset[row];
			unsigned max_off = سطر_Offset[row] + screen_Columns;

			وقتی (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    اگر (!skip_for_popup(row, col + coloff - prev_cells))
		    {
			اگر (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
			وگرنه
			    screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    }
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
	    وقتی (col < clear_width)
	    {
		ScreenCols[off_to++]
			    = (flags & SLF_INC_VCOL) ? ++last_vcol : last_vcol;
		++col;
	    }
	}
    }

    اگر (clear_width > 0
#ifdef FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#endif
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	اگر (coloff + col < firstwin->w_wincol + topframe->fr_width)
	{
	    اگر (!skip_for_popup(row, col + coloff))
	    {
		صحیح c;

		c = fillchar_vsep(&hl, wp, row);
		اگر (Screen_سطرها[off_to] != (schar_T)c
			|| (enc_utf8 && (صحیح)Screen_سطرها_UC[off_to]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_to] != hl)
		{
		    Screen_سطرها[off_to] = c;
		    ScreenAttrs[off_to] = hl;
		    اگر (enc_utf8)
		    {
			اگر (c >= 0x80)
			{
			    Screen_سطرها_UC[off_to] = c;
			    Screen_سطرها_C[0][off_to] = 0;
			    Screen_سطرها[off_to] = 0x80; // avoid storing zero
			}
			وگرنه
			    Screen_سطرها_UC[off_to] = 0;
		    }
		    screen_char(off_to, row, col + coloff);
		}
	    }
	}
	وگرنه
	    سطر_Wraps[row] = FALSE;
    }

    اگر (override_success)
	برداشت_highlight_overrides();
}

#اگر defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    تهی
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    صحیح		t;

    برای (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
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
    تهی
draw_vsep_win(win_T *wp, صحیح row)
{
    اگر (!wp->w_vsep_width)
	بازگشت;

    صحیح content_end = W_WINROW(wp) + wp->w_height;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    صحیح start_row = (row == 0) ? wp->w_winrow : W_WINROW(wp) + row;
    برای (صحیح r = start_row; r < content_end; ++r)
    {
	صحیح hl;
	صحیح c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    اگر (wp->w_status_height != 0)
    {
	صحیح hl;
	صحیح c;
	اگر (stl_connected(wp))
	    c = fillchar_status(&hl, wp);
	وگرنه
	    c = fillchar_vsep(&hl, wp, content_end);
	برای (صحیح r = content_end; r < content_end + wp->w_status_height; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    صحیح
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    وقتی (fr->fr_parent != پوچ)
    {
	اگر (fr->fr_parent->fr_layout == FR_COL)
	{
	    اگر (fr->fr_next != پوچ)
		بشکن;
	}
	وگرنه
	{
	    اگر (fr->fr_next != پوچ)
		بازگشت TRUE;
	}
	fr = fr->fr_parent;
    }
    بازگشت FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    صحیح
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    صحیح		len)	    // length of buffer
{
    char_u	*p;
    صحیح		plen;

    اگر (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	بازگشت 0;

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
    اگر (p == پوچ || *p == NUL)
#endif
    {
#ifdef FEAT_KEYMAP
	اگر (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_buffer->b_p_keymap;
	وگرنه
#endif
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((نویسه *)buf, len, (نویسه *)fmt, p);
#ifdef FEAT_EVAL
    vim_free(s);
#endif
    اگر (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    بازگشت plen;
}

#اگر defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    تهی
win_redr_custom(
    win_T	*wp,
    صحیح		draw_ruler)	// TRUE or FALSE
{
    ایستا صحیح	entered = FALSE;
    صحیح		attr;
    صحیح		curattr;
    صحیح		row;
    صحیح		col = 0;
    صحیح		maxwidth;
    صحیح		width;
    صحیح		n;
    صحیح		len;
    صحیح		stlh_cnt;
    صحیح		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    صحیح		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    صحیح		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    اگر (entered)
	بازگشت;
    entered = TRUE;

    // setup environment for the task at hand
    اگر (wp == پوچ)
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
    وگرنه
    {
	override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	row = statusline_row(wp);
	fillchar = fillchar_status(&attr, wp);
	صحیح in_status_سطر = wp->w_status_height != 0;
	maxwidth = in_status_سطر ? wp->w_width : cmdline_width;

	اگر (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    اگر (*stl == '%')
	    {
		اگر (*++stl == '-')
		    stl++;
		اگر (atoi((نویسه *)stl))
		    وقتی (VIM_ISDIGIT(*stl))
			stl++;
		اگر (*stl++ != '(')
		    stl = p_ruf;
	    }
	    col = ru_col - (cmdline_width - maxwidth);
	    اگر (col < (maxwidth + 1) / 2)
		col = (maxwidth + 1) / 2;
	    maxwidth -= col;
	    اگر (!in_status_سطر)
	    {
		row = Rows - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	وگرنه
	{
	    opt_name = (char_u *)"statusline";
	    اگر (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    وگرنه
		stl = p_stl;
	}

	اگر (in_status_سطر)
	    col += wp->w_wincol;
	وگرنه
	    col += cmdline_col_off;
    }

    اگر (maxwidth <= 0)
	برو theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == پوچ ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_status_height;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == پوچ) ? (char_u *)"" : stl;
    صحیح col_save = col;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **out_regions = پوچ;
    صحیح			*out_شمار = پوچ;
    صحیح			region_base_col = 0;
    اگر (!draw_ruler)
    {
	اگر (wp != پوچ)
	{
	    out_regions = &wp->w_stl_click;
	    out_شمار = &wp->w_stl_click_شمار;
	    region_base_col = wp->w_wincol;
	}
	وگرنه
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    out_regions = &tabline_stl_click;
	    out_شمار = &tabline_stl_click_شمار;
	    region_base_col = firstwin->w_wincol;
	}

	برای (n = 0; n < *out_شمار; n++)
	    vim_free((*out_regions)[n].funcname);
	VIM_CLEAR(*out_regions);
	*out_شمار = 0;
    }

    صحیح carry_hl = 0;
    برای (صحیح i = 0; i < stlh_cnt; i++)
    {
	col = col_save;
	buf[0] = NUL;
	width = build_stl_str_hl_mline(ewp, buf, اندازه(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&carry_hl);

	// Make all characters printable.
	p = transstr(buf);
	اگر (p != پوچ)
	{
	    len = vim_snprintf((نویسه *)buf, اندازه(buf), "%s", p);
	    vim_free(p);
	}
	وگرنه
	    len = (صحیح)STRLEN(buf);

	// fill up with "fillchar"
	وقتی (width < maxwidth && len < (صحیح)اندازه(buf) - 1)
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
	برای (n = 0; hltab[n].start != پوچ; n++)
	{
	    len = (صحیح)(hltab[n].start - p);
	    screen_puts_len(p, len, row + i, col, curattr);
	    col += vim_strnsize(p, len);
	    p = hltab[n].start;

	    اگر (hltab[n].userhl == 0)
		curattr = attr;
	    وگرنه اگر (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    وگرنه اگر (wp != پوچ && wp != curwin && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    وگرنه اگر (wp != پوچ && bt_terminal(wp->w_buffer)
						    && wp->w_status_height != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    وگرنه اگر (wp != پوچ && wp != curwin && wp->w_status_height != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    وگرنه
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, row + i, col, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	اگر (out_regions != پوچ)
	{
	    صحیح click_شمار = 0;
	    برای (n = 0; clicktab[n].start != پوچ; n++)
		click_شمار++;

	    اگر (click_شمار > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*out_regions,
			اندازه(stl_click_region_T)
				    * (*out_شمار + click_شمار));
		اگر (new_arr != پوچ)
		{
		    char_u  *cur_funcname = پوچ;
		    صحیح	    cur_minwid = 0;
		    صحیح	    region_start = region_base_col;

		    *out_regions = new_arr;

		    len = 0;
		    p = buf;
		    برای (n = 0; clicktab[n].start != پوچ; n++)
		    {
			len += vim_strnsize(p,
					    (صحیح)(clicktab[n].start - p));
			p = clicktab[n].start;

			اگر (cur_funcname != پوچ)
			{
			    stl_click_region_T *r =
					&(*out_regions)[*out_شمار];
			    r->row = row + i;
			    r->col_start = region_start;
			    r->col_end = region_base_col + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*out_شمار)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_start = region_base_col + len;
		    }

		    // Close final region if it extends to the end.
		    اگر (cur_funcname != پوچ)
		    {
			stl_click_region_T *r =
					&(*out_regions)[*out_شمار];
			r->row = row + i;
			r->col_start = region_start;
			r->col_end = region_base_col + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*out_شمار)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	برای (n = 0; clicktab[n].start != پوچ; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    اگر (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    اگر (stl != پوچ)
	vim_free(stl_tmp);

    اگر (wp == پوچ)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	col = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	برای (n = 0; tabtab[n].start != پوچ; n++)
	{
	    len += vim_strnsize(p, (صحیح)(tabtab[n].start - p));
	    وقتی (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	وقتی (col < firstwin->w_wincol + topframe->fr_width)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    اگر (override_success)
	برداشت_highlight_overrides();
    entered = FALSE;
}

#endif // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    تهی
screen_putchar(صحیح c, صحیح row, صحیح col, صحیح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    اگر (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    وگرنه
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
    تهی
screen_getbytes(صحیح row, صحیح col, char_u *bytes, صحیح *attrp)
{
    unsigned off;

    // safety check
    اگر (Screen_سطرها == پوچ || row >= screen_Rows || col >= screen_Columns)
	بازگشت;

    off = سطر_Offset[row] + col;
    اگر (attrp != پوچ)
	*attrp = ScreenAttrs[off];
    bytes[0] = Screen_سطرها[off];
    bytes[1] = NUL;

    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0)
	bytes[utfc_char2bytes(off, bytes)] = NUL;
    وگرنه اگر (enc_dbcs == DBCS_JPNU && Screen_سطرها[off] == 0x8e)
    {
	bytes[0] = Screen_سطرها[off];
	bytes[1] = ScreenLines2[off];
	bytes[2] = NUL;
    }
    وگرنه اگر (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
    {
	bytes[1] = Screen_سطرها[off + 1];
	bytes[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ایستا صحیح
screen_comp_differs(صحیح off, صحیح *u8cc)
{
    صحیح	    i;

    برای (i = 0; i < Screen_mco; ++i)
    {
	اگر (Screen_سطرها_C[i][off] != (u8char_T)u8cc[i])
	    بازگشت TRUE;
	اگر (u8cc[i] == 0)
	    بشکن;
    }
    بازگشت FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    تهی
screen_puts(
    char_u	*text,
    صحیح		row,
    صحیح		col,
    صحیح		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    تهی
screen_puts_len(
    char_u	*text,
    صحیح		textlen,
    صحیح		row,
    صحیح		col,
    صحیح		attr_arg)
{
    صحیح		attr = attr_arg;
    unsigned	off;
    char_u	*ptr = text;
    صحیح		len = textlen;
    صحیح		c;
    unsigned	max_off;
    صحیح		mbyte_blen = 1;
    صحیح		mbyte_cells = 1;
    صحیح		u8c = 0;
    صحیح		u8cc[MAX_MCO];
    صحیح		clear_next_cell = FALSE;
#ifdef FEAT_ARABIC
    صحیح		prev_c = 0;		// previous Arabic character
    صحیح		pc, nc, nc1;
    صحیح		pcc[MAX_MCO];
#endif
    صحیح		force_redraw_this;
    صحیح		force_redraw_next = FALSE;
    صحیح		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    اگر (Screen_سطرها == پوچ
	    || row >= screen_Rows || row < 0
	    || col >= screen_Columns || col < 0)
	بازگشت;
    off = سطر_Offset[row] + col;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    اگر (has_mbyte && col > 0 && col < screen_Columns
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    && mb_fix_col(col, row) != col)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	اگر (!skip_for_popup(row, col - 1))
	{
	    Screen_سطرها[off - 1] = ' ';
	    اگر (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    وگرنه اگر (enc_utf8)
	    {
		Screen_سطرها_UC[off - 1] = 0;
		Screen_سطرها_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, row, col - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_next = TRUE;
    }

    max_off = سطر_Offset[row] + screen_Columns;
    وقتی (col < screen_Columns
	    && (len < 0 || (صحیح)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	اگر (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (صحیح)((text + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    اگر (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    وگرنه اگر (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    وگرنه	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (صحیح)((text + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ifdef FEAT_ARABIC
		اگر (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    اگر (len >= 0 && (صحیح)(ptr - text) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    وگرنه
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(صحیح)((text + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		وگرنه
		    prev_c = u8c;
#endif
		اگر (col + mbyte_cells > screen_Columns)
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
	صحیح cell_attr = attr;
	اگر (screen_pum_blend > 0 && pum_bg_attrs != پوچ
		&& row >= pum_bg_بالاترین && row < pum_bg_bot
		&& col < pum_bg_cols)
	{
	    صحیح soff = (row - pum_bg_بالاترین) * pum_bg_cols + col;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_سطرها[off] != c
		|| (mbyte_cells == 2
		    && Screen_سطرها[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_سطرها_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_سطرها_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_active
#ifdef FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(row, col)
#endif
		;

	اگر ((need_redraw || force_redraw_this) && !skip_for_popup(row, col))
	{
#اگر defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    اگر (need_redraw && Screen_سطرها[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		صحیح	n = ScreenAttrs[off];

		اگر (n > HL_ALL)
		    n = syn_attr2attr(n);
		اگر (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    اگر (gui.directx_enabled && need_redraw)
		force_redraw_next = TRUE;
#endif
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    اگر (clear_next_cell)
		clear_next_cell = FALSE;
	    وگرنه اگر (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    اگر (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_سطرها[off + mbyte_blen] = 0;
	    Screen_سطرها[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    ScreenCols[off] = -1;
	    اگر (enc_utf8)
	    {
		اگر (c < 0x80 && u8cc[0] == 0)
		    Screen_سطرها_UC[off] = 0;
		وگرنه
		{
		    صحیح	    i;

		    Screen_سطرها_UC[off] = u8c;
		    برای (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطرها_C[i][off] = u8cc[i];
			اگر (u8cc[i] == 0)
			    بشکن;
		    }
		}
		اگر (mbyte_cells == 2)
		{
		    Screen_سطرها[off + 1] = 0;
		    Screen_سطرها_UC[off + 1] = 0;
		    برای (صحیح ci = 0; ci < Screen_mco; ++ci)
			Screen_سطرها_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    ScreenCols[off + 1] = -1;
		}
		screen_char(off, row, col);
	    }
	    وگرنه اگر (mbyte_cells == 2)
	    {
		Screen_سطرها[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		ScreenCols[off + 1] = -1;
		screen_char_2(off, row, col);
	    }
	    وگرنه اگر (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    وگرنه
		screen_char(off, row, col);
	}
	اگر (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    اگر (clear_next_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	وگرنه
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    اگر (force_redraw_next && col < screen_Columns && !skip_for_popup(row, col))
    {
	اگر (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	وگرنه
	    screen_char(off, row, col);
    }
}

#اگر defined(FEAT_جستجو_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    تهی
start_جستجو_hl(تهی)
{
    اگر (!p_hls || no_hlsearch)
	بازگشت;

    end_جستجو_hl();  // just in case it wasn't called before
    last_pat_prog(&screen_جستجو_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    تهی
end_جستجو_hl(تهی)
{
    اگر (screen_جستجو_hl.rm.regprog == پوچ)
	بازگشت;

    vim_regfree(screen_جستجو_hl.rm.regprog);
    screen_جستجو_hl.rm.regprog = پوچ;
}
#endif

    ایستا تهی
screen_start_highlight(صحیح attr)
{
    attrentry_T *aep = پوچ;

    screen_attr = attr;
    اگر (!full_screen
#ifdef MSWIN
	    || !termcap_active
#endif
       )
	بازگشت;

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	نویسه	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	OUT_STR(buf);
	بازگشت;
    }
#endif

    اگر (attr > HL_ALL)				// special HL attr.
    {
	اگر (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	وگرنه
	    aep = syn_term_attr2entry(attr);
	اگر (aep == پوچ)	    // did ":syntax clear"
	    attr = 0;
	وگرنه
	    attr = aep->ae_attr;
    }
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    اگر (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	صحیح	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	اگر (p_tgc)
	{
	    اگر (aep == پوچ || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		term_fg_rgb_color(defguifg);
	    اگر (aep == پوچ || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		term_bg_rgb_color(defguibg);
	}
	وگرنه اگر (t_colors >= 256)
	{
	    اگر (aep == پوچ || aep->ae_u.cterm.fg_color == 0)
		term_fg_color(defctermfg);
	    اگر (aep == پوچ || aep->ae_u.cterm.bg_color == 0)
		term_bg_color(defctermbg);
	}
    }
#endif
    اگر ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	out_str(T_MD);
    وگرنه اگر (aep != پوچ && cterm_normal_fg_bold && (
#ifdef FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#endif
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	out_str(T_ME);
    اگر ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	out_str(T_SO);
    اگر ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	out_str(T_UCS);
    اگر ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	out_str(T_USS);
    اگر ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	out_str(T_DS);
    اگر ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	out_str(T_CDS);
    اگر (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	out_str(T_US);
    اگر ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	out_str(T_CZH);
    اگر ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	out_str(T_MR);
    اگر ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	out_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    اگر (aep != پوچ)
    {
	اگر (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		term_font(aep->ae_u.cterm.font);
#ifdef FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	اگر (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		term_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	وگرنه
#endif
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.fg_color)
		    term_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	اگر (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		term_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	وگرنه
#endif
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.bg_color)
		    term_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ifdef FEAT_TERMGUICOLORS
	اگر (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		term_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	وگرنه
#endif
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.ul_color)
		    term_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	اگر (!IS_CTERM)
	{
	    اگر (aep->ae_u.term.start != پوچ)
		out_str(aep->ae_u.term.start);
	}
    }
}

    تهی
screen_stop_highlight(تهی)
{
    صحیح	    do_ME = FALSE;	    // output T_ME code
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    صحیح	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#endif

    اگر (screen_attr != 0
#ifdef MSWIN
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	اگر (gui.in_use)
	{
	    نویسه	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    OUT_STR(buf);
	}
	وگرنه
#endif
	{
	    صحیح is_under;

	    اگر (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		اگر (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    اگر (aep != پوچ && ((
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
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    اگر (use_vtp())
		    {
			اگر (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			اگر (!do_ME && do_ME_fg)
			    out_str((char_u *)"\033|39m"); // restore FG
			اگر (!do_ME && do_ME_bg)
			    out_str((char_u *)"\033|49m"); // restore BG
		    }
		    وگرنه
		    {
			// Process FG and BG at once.
			اگر (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#endif
		}
		وگرنه
		{
		    aep = syn_term_attr2entry(screen_attr);
		    اگر (aep != پوچ && aep->ae_u.term.stop != پوچ)
		    {
			اگر (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			وگرنه
			    out_str(aep->ae_u.term.stop);
		    }
		}
		اگر (aep == پوچ)	    // did ":syntax clear"
		    screen_attr = 0;
		وگرنه
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    اگر (screen_attr & HL_STANDOUT)
	    {
		اگر (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		وگرنه
		    out_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    اگر (is_under && *T_UCE != NUL)
	    {
		اگر (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		وگرنه
		    out_str(T_UCE);
	    }
	    اگر ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		اگر (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		وگرنه
		    out_str(T_UE);
	    }
	    اگر (screen_attr & HL_ITALIC)
	    {
		اگر (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		وگرنه
		    out_str(T_CZR);
	    }
	    اگر (screen_attr & HL_STRIKETHROUGH)
	    {
		اگر (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		وگرنه
		    out_str(T_STE);
	    }
	    اگر (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

#ifdef FEAT_TERMGUICOLORS
	    اگر (p_tgc)
	    {
		اگر (cterm_normal_fg_gui_color != INVALCOLOR)
		    term_fg_rgb_color(cterm_normal_fg_gui_color);
		اگر (cterm_normal_bg_gui_color != INVALCOLOR)
		    term_bg_rgb_color(cterm_normal_bg_gui_color);
		اگر (cterm_normal_ul_gui_color != INVALCOLOR)
		    term_ul_rgb_color(cterm_normal_ul_gui_color);
	    }
	    وگرنه
#endif
	    {
		اگر (t_colors > 1)
		{
		    // set Normal cterm colors
		    اگر (cterm_normal_fg_color != 0)
			term_fg_color(cterm_normal_fg_color - 1);
		    اگر (cterm_normal_bg_color != 0)
			term_bg_color(cterm_normal_bg_color - 1);
		    اگر (cterm_normal_ul_color != 0)
			term_ul_color(cterm_normal_ul_color - 1);
		    اگر (cterm_normal_fg_bold)
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
    تهی
reset_cterm_colors(تهی)
{
    اگر (!IS_CTERM)
	بازگشت;

    // set Normal cterm colors
#ifdef FEAT_TERMGUICOLORS
    اگر (p_tgc ? (cterm_normal_fg_gui_color != INVALCOLOR
		|| cterm_normal_bg_gui_color != INVALCOLOR)
	    : (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0))
#وگرنه
	اگر (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
#endif
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
    اگر (cterm_normal_fg_bold)
    {
	out_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    تهی
screen_char(unsigned off, صحیح row, صحیح col)
{
    صحیح		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    اگر (row >= screen_Rows || col >= screen_Columns)
	بازگشت;

#ifdef FEAT_IMAGE
    mark_dirty_region_for_images(row, col, 1, 1);
#endif

#ifdef FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    اگر (popup_is_under_opacity(row, col))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	اگر (enc_utf8 && Screen_سطرها_UC[off] != 0
		&& utf_char2cells(Screen_سطرها_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& !popup_is_under_opacity(row, col + 1))
	{
	    صحیح off2 = off + 1;
	    Screen_سطرها[off2] = ' ';
	    Screen_سطرها_UC[off2] = 0;
	    screen_char(off2, row, col + 1);
	}
	mark_suppressed_cell(row, col);
	اگر (enc_utf8 && Screen_سطرها_UC[off] != 0
		&& utf_char2cells(Screen_سطرها_UC[off]) == 2
		&& col + 1 < screen_Columns
		&& popup_is_under_opacity(row, col + 1))
	    mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	بازگشت;
    }
    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0
	    && utf_char2cells(Screen_سطرها_UC[off]) == 2
	    && col + 1 < screen_Columns
	    && popup_is_under_opacity(row, col + 1))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	بازگشت;
    }
#endif

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    اگر (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	بازگشت;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    اگر (screen_char_attr != 0)
	attr = screen_char_attr;
    وگرنه
	attr = ScreenAttrs[off];
    اگر (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

#ifdef FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(row, col);
    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0
	    && utf_char2cells(Screen_سطرها_UC[off]) == 2)
	unmark_suppressed_cell(row, col + 1);
#endif

    اگر (screen_attr != attr)
	screen_start_highlight(attr);

    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	اگر (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    get_cellwidth(Screen_سطرها_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    out_str((char_u *)"  ");
	    term_windgoto(row, col);
	    screen_cur_col = 9999;
	}
	وگرنه اگر (utf_ambiguous_width(Screen_سطرها_UC[off]))
	{
	    اگر (*p_ambw == 'd'
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
	وگرنه اگر (utf_char2cells(Screen_سطرها_UC[off]) > 1)
	    ++screen_cur_col;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
    }
    وگرنه
    {
	out_flush_check();
	out_char(Screen_سطرها[off]);
	// double-byte character in single-width cell
	اگر (enc_dbcs == DBCS_JPNU && Screen_سطرها[off] == 0x8e)
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
    ایستا تهی
screen_char_2(unsigned off, صحیح row, صحیح col)
{
    // Check for illegal values (could be wrong when screen was resized).
    اگر (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	بازگشت;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    اگر (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	ScreenCols[off] = -1;
	بازگشت;
    }

#ifdef FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    اگر (popup_is_under_opacity(row, col))
    {
	mark_suppressed_cell(row, col);
	mark_suppressed_cell(row, col + 1);
	screen_cur_col = 9999;
	بازگشت;
    }
#endif

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, row, col);
    out_char(Screen_سطرها[off + 1]);
#ifdef FEAT_PROP_POPUP
    unmark_suppressed_cell(row, col + 1);
#endif
    ++screen_cur_col;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    تهی
screen_draw_rectangle(
    صحیح		row,
    صحیح		col,
    صحیح		height,
    صحیح		width,
    صحیح		invert,
    صحیح		force)
{
    صحیح		r, c;
    صحیح		off;
    صحیح		max_off;

    // Can't use ScreenLines unless initialized
    اگر (Screen_سطرها == پوچ)
	بازگشت;

    اگر (invert)
	screen_char_attr = HL_INVERSE;
    برای (r = row; r < row + height; ++r)
    {
	off = سطر_Offset[r];
	max_off = off + screen_Columns;
	برای (c = col; c < col + width; ++c)
	{
	    اگر (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		اگر (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    وگرنه
	    {
		اگر (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		اگر (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    ایستا تهی
redraw_block(صحیح row, صحیح end, win_T *wp)
{
    صحیح		col;
    صحیح		width;

#ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
#endif

    اگر (wp == پوچ)
    {
	col = firstwin->w_wincol;
	width = topframe->fr_width;
    }
    وگرنه
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE, FALSE);
}

    تهی
space_to_screenline(صحیح off, صحیح attr)
{
    Screen_سطرها[off] = ' ';
    ScreenAttrs[off] = attr;
    ScreenCols[off] = -1;
    اگر (enc_utf8)
	Screen_سطرها_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    تهی
screen_fill(
	صحیح	start_row,
	صحیح	end_row,
	صحیح	start_col,
	صحیح	end_col,
	صحیح	c1,
	صحیح	c2,
	صحیح	attr)
{
    صحیح	    row;
    صحیح	    col;
    صحیح	    off;
    صحیح	    end_off;
    صحیح	    did_delete;
    صحیح	    c;
    صحیح	    norm_term;
#اگر defined(FEAT_GUI) || defined(UNIX)
    صحیح	    force_next = FALSE;
#endif

    اگر (end_row > screen_Rows)		// safety check
	end_row = screen_Rows;
    اگر (end_col > screen_Columns)	// safety check
	end_col = screen_Columns;
    اگر (Screen_سطرها == پوچ
	    || start_row < 0		// should not happen
	    || start_row >= end_row
	    || start_col >= end_col)	// nothing to do
	بازگشت;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
	    !IS_CTERM);
    برای (row = start_row; row < end_row; ++row)
    {
	اگر (has_mbyte
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    اگر (start_col > 0 && mb_fix_col(start_col, row) != start_col)
	    {
		// start_col is the right half, clear the left half
		صحیح چپ_attr = ScreenAttrs[سطر_Offset[row] + start_col - 1];
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, چپ_attr);
	    }
	    اگر (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		صحیح راست_attr = ScreenAttrs[سطر_Offset[row] + end_col];
		screen_puts_len((char_u *)" ", 1, row, end_col, راست_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	اگر (c2 == ' '
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
	    اگر (c1 != ' ')			// don't clear first char
		++col;

	    off = سطر_Offset[row] + col;
	    end_off = سطر_Offset[row] + end_col;

	    // skip blanks (used often, keep it fast!)
	    اگر (enc_utf8)
		وقتی (off < end_off && Screen_سطرها[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_سطرها_UC[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (صحیح)(off - سطر_Offset[row]))
#endif
			  )
		    ++off;
	    وگرنه
		وقتی (off < end_off && Screen_سطرها[off] == ' '
						     && ScreenAttrs[off] == 0
#ifdef FEAT_PROP_POPUP
			  && !is_suppressed_cell(row,
					      (صحیح)(off - سطر_Offset[row]))
#endif
			  )
		    ++off;
	    اگر (off < end_off)		// something to be cleared
	    {
		col = off - سطر_Offset[row];
		screen_stop_highlight();
		term_windgoto(row, col);// clear rest of this screen line
		out_str(T_CE);
		screen_start();		// don't know where cursor is now
		col = end_col - col;
		وقتی (col--)		// clear chars in ScreenLines
		{
		    space_to_screenline(off, 0);
#ifdef FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(row, (صحیح)(off - سطر_Offset[row]));
#endif
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = سطر_Offset[row] + start_col;
	c = c1;
	برای (col = start_col; col < end_col; ++col)
	{
	    اگر ((Screen_سطرها[off] != c
		    || (enc_utf8 && (صحیح)Screen_سطرها_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#اگر defined(FEAT_GUI) || defined(UNIX)
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
		اگر (screen_opacity_popup != پوچ && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    اگر (Screen_سطرها[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_سطرها_UC[off - 1] != 0
			    && utf_char2cells(Screen_سطرها_UC[off - 1]) == 2)
			برو next_col;
		}
		اگر (screen_opacity_popup != پوچ && c == ' '
						       && Screen_سطرها[off] != 0)
		{
		    صحیح bg_char_cells = 1;
		    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_سطرها_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    اگر (bg_char_cells == 2)
		    {
			اگر (col + 1 >= end_col)
			    // At the edge, skip wide char.
			    برو skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			اگر (c2 != ' ')
			    برو skip_opacity_fill;
		    }

		    صحیح popup_attr = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, row, col);
		    برو next_col;
		}
skip_opacity_fill:
#endif
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		اگر (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != پوچ
			&& row >= pum_bg_بالاترین && row < pum_bg_bot
			&& col < pum_bg_cols)
		{
		    صحیح soff = (row - pum_bg_بالاترین) * pum_bg_cols + col;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ
			    && col > start_col
			    && pum_bg_سطرها_UC[soff] == 0
			    && pum_bg_سطرها_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_سطرها_UC[soff - 1]) == 2)
		    {
			Screen_سطرها[off] = 0;
			اگر (Screen_سطرها_UC != پوچ)
			    Screen_سطرها_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			برو next_col;
		    }

		    صحیح underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ
			    && ((pum_bg_سطرها_UC[soff] != 0
				    && utf_char2cells(pum_bg_سطرها_UC[soff]) == 2
				    && col + 1 >= end_col)
				|| (col == start_col
				    && pum_bg_سطرها_UC[soff] == 0
				    && col > 0
				    && pum_bg_سطرها_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_سطرها_UC[soff - 1])
								       == 2)))
		    {
			Screen_سطرها[off] = ' ';
			اگر (Screen_سطرها_UC != پوچ)
			    Screen_سطرها_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, row, col);
			برو next_col;
		    }

		    // Restore underlying character so text shows through.
		    Screen_سطرها[off] = pum_bg_سطرها[soff];
		    اگر (enc_utf8 && pum_bg_سطرها_UC != پوچ
						&& Screen_سطرها_UC != پوچ)
		    {
			صحیح k;
			Screen_سطرها_UC[off] = pum_bg_سطرها_UC[soff];
			برای (k = 0; k < MAX_MCO; ++k)
			    اگر (pum_bg_سطرها_C[k] != پوچ
						    && Screen_سطرها_C[k] != پوچ)
				Screen_سطرها_C[k][off] = pum_bg_سطرها_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, row, col);
		    برو next_col;
		}
#اگر defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		اگر (
# ifdef FEAT_GUI
			gui.in_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    اگر (Screen_سطرها[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    وگرنه
			force_next = FALSE;
		}
#endif // FEAT_GUI || defined(UNIX)
#ifdef FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		اگر (gui.directx_enabled)
		    force_next = TRUE;
#endif
		Screen_سطرها[off] = c;
		اگر (enc_utf8)
		{
		    اگر (c >= 0x80)
		    {
			Screen_سطرها_UC[off] = c;
			Screen_سطرها_C[0][off] = 0;
			Screen_سطرها[off] = 0x80; // avoid storing zero
		    }
		    وگرنه
			Screen_سطرها_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		اگر (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
next_col:
	    ScreenCols[off] = -1;
	    ++off;
	    اگر (col == start_col)
	    {
		اگر (did_delete)
		    بشکن;
		c = c2;
	    }
	}
	اگر (end_col == Columns)
	    سطر_Wraps[row] = FALSE;
	اگر (row == Rows - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    اگر (((start_col == 0 && end_col == Columns)
			|| (start_col == cmdline_col_off
			&& end_col == cmdline_col_off + cmdline_width))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ifdef FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#endif
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    اگر (start_col == 0 || start_col == cmdline_col_off)
		mode_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    تهی
check_for_delay(صحیح check_پیام_scroll)
{
    اگر ((emsg_on_display || (check_پیام_scroll && پیام_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !in_assert_fails)
    {
	out_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	اگر (check_پیام_scroll)
	    پیام_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ایستا تهی
clear_TabPageIdxs(تهی)
{
    صحیح		scol;

    برای (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    صحیح
screen_valid(صحیح doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    بازگشت (Screen_سطرها != پوچ);
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
    تهی
screenalloc(صحیح doclear)
{
    صحیح		    new_row, old_row;
#ifdef FEAT_GUI
    صحیح		    old_Rows;
#endif
    win_T	    *wp;
    صحیح		    outofmem = FALSE;
    صحیح		    len;
    schar_T	    *new_Screen_سطرها;
    u8char_T	    *new_Screen_سطرها_UC = پوچ;
    u8char_T	    *new_Screen_سطرها_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = پوچ;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_ScreenCols;
    unsigned	    *new_سطر_Offset;
    char_u	    *new_سطر_Wraps;
    short	    *new_TabPageIdxs;
#ifdef FEAT_PROP_POPUP
    short	    *new_popup_mask;
    short	    *new_popup_mask_next;
    نویسه	    *new_popup_transparent;
#endif
    tabpage_T	    *tp;
    ایستا صحیح	    entered = FALSE;		// avoid recursiveness
    ایستا صحیح	    done_outofmem_پیام = FALSE;	// did outofmem message
    صحیح		    retry_شمار = 0;
    صحیح		    found_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    اگر ((Screen_سطرها != پوچ
		&& Rows == screen_Rows
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_سطرها_UC != پوچ)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != پوچ)
		&& p_mco == Screen_mco)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && Screen_سطرها == پوچ))
	بازگشت;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    اگر (entered)
	بازگشت;
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
    برای (صحیح i = 0; i < AUCMD_WIN_شمار; ++i)
	اگر (aucmd_win[i].auc_win != پوچ)
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

    new_Screen_سطرها = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    vim_memset(new_Screen_سطرها_C, 0, اندازه(u8char_T *) * MAX_MCO);
    اگر (enc_utf8)
    {
	new_Screen_سطرها_UC = LALLOC_MULT(u8char_T, (Rows + 1) * Columns);
	برای (صحیح i = 0; i < p_mco; ++i)
	    new_Screen_سطرها_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (Rows + 1) * Columns);
    }
    اگر (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (Rows + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (Rows + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_ScreenCols = LALLOC_CLEAR_MULT(colnr_T, (Rows + 1) * Columns);
    new_سطر_Offset = LALLOC_MULT(unsigned, Rows);
    new_سطر_Wraps = LALLOC_MULT(char_u, Rows);
    new_TabPageIdxs = LALLOC_MULT(short, Columns);
#ifdef FEAT_PROP_POPUP
    new_popup_mask = LALLOC_MULT(short, Rows * Columns);
    new_popup_mask_next = LALLOC_MULT(short, Rows * Columns);
    new_popup_transparent = LALLOC_MULT(نویسه, Rows * Columns);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (win_alloc_سطرها(wp) == FAIL)
	{
	    outofmem = TRUE;
	    برو give_up;
	}
    }
    برای (صحیح i = 0; i < AUCMD_WIN_شمار; ++i)
	اگر (aucmd_win[i].auc_win != پوچ
		&& aucmd_win[i].auc_win->w_سطرها == پوچ
		&& win_alloc_سطرها(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    بشکن;
	}
#ifdef FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	اگر (win_alloc_سطرها(wp) == FAIL)
	{
	    outofmem = TRUE;
	    برو give_up;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    اگر (win_alloc_سطرها(wp) == FAIL)
	    {
		outofmem = TRUE;
		برو give_up;
	    }
#endif

give_up:
    found_null = FALSE;
    برای (صحیح i = 0; i < p_mco; ++i)
	اگر (new_Screen_سطرها_C[i] == پوچ)
	{
	    found_null = TRUE;
	    بشکن;
	}
    اگر (new_Screen_سطرها == پوچ
	    || (enc_utf8 && (new_Screen_سطرها_UC == پوچ || found_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == پوچ)
	    || new_ScreenAttrs == پوچ
	    || new_ScreenCols == پوچ
	    || new_سطر_Offset == پوچ
	    || new_سطر_Wraps == پوچ
	    || new_TabPageIdxs == پوچ
#ifdef FEAT_PROP_POPUP
	    || new_popup_mask == پوچ
	    || new_popup_mask_next == پوچ
	    || new_popup_transparent == پوچ
#endif
	    || outofmem)
    {
	اگر (Screen_سطرها != پوچ || !done_outofmem_پیام)
	{
	    // guess the size
	    do_outofmem_پیام((long_u)((Rows + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    done_outofmem_پیام = TRUE;
	}
	VIM_CLEAR(new_Screen_سطرها);
	VIM_CLEAR(new_Screen_سطرها_UC);
	برای (صحیح i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_سطرها_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_ScreenCols);
	VIM_CLEAR(new_سطر_Offset);
	VIM_CLEAR(new_سطر_Wraps);
	VIM_CLEAR(new_TabPageIdxs);
#ifdef FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_mask);
	VIM_CLEAR(new_popup_mask_next);
	VIM_CLEAR(new_popup_transparent);
#endif
    }
    وگرنه
    {
	done_outofmem_پیام = FALSE;

	برای (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_سطر_Offset[new_row] = new_row * Columns;
	    new_سطر_Wraps[new_row] = FALSE;

	    (تهی)vim_memset(new_Screen_سطرها + new_row * Columns,
				  ' ', (size_t)Columns * اندازه(schar_T));
	    اگر (enc_utf8)
	    {
		(تهی)vim_memset(new_Screen_سطرها_UC + new_row * Columns,
				   0, (size_t)Columns * اندازه(u8char_T));
		برای (صحیح i = 0; i < p_mco; ++i)
		    (تهی)vim_memset(new_Screen_سطرها_C[i]
						      + new_row * Columns,
				   0, (size_t)Columns * اندازه(u8char_T));
	    }
	    اگر (enc_dbcs == DBCS_JPNU)
		(تهی)vim_memset(new_ScreenLines2 + new_row * Columns,
				   0, (size_t)Columns * اندازه(schar_T));
	    (تهی)vim_memset(new_ScreenAttrs + new_row * Columns,
				    0, (size_t)Columns * اندازه(sattr_T));
	    (تهی)vim_memset(new_ScreenCols + new_row * Columns,
				    0, (size_t)Columns * اندازه(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    اگر (!doclear)
	    {
		old_row = new_row + (screen_Rows - Rows);
		اگر (old_row >= 0 && Screen_سطرها != پوچ)
		{
		    اگر (screen_Columns < Columns)
			len = screen_Columns;
		    وگرنه
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    اگر (!(enc_utf8 && Screen_سطرها_UC == پوچ)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_سطرها + new_سطر_Offset[new_row],
				Screen_سطرها + سطر_Offset[old_row],
				(size_t)len * اندازه(schar_T));
		    اگر (enc_utf8 && Screen_سطرها_UC != پوچ
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_سطرها_UC + new_سطر_Offset[new_row],
				Screen_سطرها_UC + سطر_Offset[old_row],
				(size_t)len * اندازه(u8char_T));
			برای (صحیح i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_سطرها_C[i]
						    + new_سطر_Offset[new_row],
				Screen_سطرها_C[i] + سطر_Offset[old_row],
				(size_t)len * اندازه(u8char_T));
		    }
		    اگر (enc_dbcs == DBCS_JPNU && ScreenLines2 != پوچ)
			mch_memmove(new_ScreenLines2 + new_سطر_Offset[new_row],
				ScreenLines2 + سطر_Offset[old_row],
				(size_t)len * اندازه(schar_T));
		    mch_memmove(new_ScreenAttrs + new_سطر_Offset[new_row],
			    ScreenAttrs + سطر_Offset[old_row],
			    (size_t)len * اندازه(sattr_T));
		    mch_memmove(new_ScreenCols + new_سطر_Offset[new_row],
			    ScreenCols + سطر_Offset[old_row],
			    (size_t)len * اندازه(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_سطر = new_Screen_سطرها + Rows * Columns;

#ifdef FEAT_PROP_POPUP
	vim_memset(new_popup_mask, 0, Rows * Columns * اندازه(short));
	vim_memset(new_popup_transparent, 0, Rows * Columns * اندازه(نویسه));
#endif
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_سطرها = new_Screen_سطرها;
    Screen_سطرها_UC = new_Screen_سطرها_UC;
    برای (صحیح i = 0; i < p_mco; ++i)
	Screen_سطرها_C[i] = new_Screen_سطرها_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    ScreenCols = new_ScreenCols;
    سطر_Offset = new_سطر_Offset;
    سطر_Wraps = new_سطر_Wraps;
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
    اگر (gui.in_use && (gui.cursor_row >= screen_Rows
		|| gui.cursor_col >= screen_Columns))
	gui.cursor_is_valid = false;
#endif

    set_must_redraw(UPD_CLEAR);	// need to clear the screen later
    اگر (doclear)
	screenclear2(TRUE);
#ifdef FEAT_GUI
    وگرنه اگر (gui.in_use
	    && !gui.starting
	    && Screen_سطرها != پوچ
	    && old_Rows != Rows)
    {
	gui_redraw_block(0, 0, (صحیح)Rows - 1, (صحیح)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	اگر (پیام_row >= Rows)		// Rows got smaller
	    پیام_row = Rows - 1;		// put cursor at last row
	وگرنه اگر (Rows > old_Rows)	// Rows got bigger
	    پیام_row += Rows - old_Rows; // put cursor in same place
	اگر (پیام_col >= cmdline_width)	// cmdline_width got smaller
	    پیام_col = cmdline_width - 1;    // put cursor at last cmdline column
    }
#endif
    clear_TabPageIdxs();

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    entered = FALSE;
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    اگر (starting == 0 && ++retry_شمار <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, پوچ, پوچ, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	برو retry;
    }
}

    تهی
free_screenlines(تهی)
{
    صحیح		i;

    VIM_CLEAR(Screen_سطرها_UC);
    برای (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_سطرها_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_سطرها);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(ScreenCols);
    VIM_CLEAR(سطر_Offset);
    VIM_CLEAR(سطر_Wraps);
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
    صحیح
screenclear(تهی)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    بازگشت screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    تهی
redraw_as_cleared(تهی)
{
    screenclear2(FALSE);
}

    ایستا صحیح
screenclear2(صحیح doclear)
{
    صحیح	    i;
    صحیح	    did_clear = FALSE;

    اگر (starting == NO_SCREEN || Screen_سطرها == پوچ
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	بازگشت FALSE;

#ifdef FEAT_GUI
    اگر (!gui.in_use)
#endif
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ifdef FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#endif

    // blank out ScreenLines
    برای (i = 0; i < Rows; ++i)
    {
	lineclear(سطر_Offset[i], (صحیح)Columns, 0);
	سطر_Wraps[i] = FALSE;
    }

    اگر (doclear && can_clear(T_CL))
    {
	out_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
#ifdef FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	اگر (suppressed_cells != پوچ)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_rows * suppressed_cols);
#endif
    }
    وگرنه
    {
	// can't clear the screen, mark all chars with invalid attributes
	برای (i = 0; i < Rows; ++i)
	    lineinvalid(سطر_Offset[i], (صحیح)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    اگر (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    پیام_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#endif
    پیام_row = cmdline_row;	// put cursor on last line for messages
    پیام_col = 0;
    screen_start();		// don't know where cursor is now
    پیام_didany = FALSE;
    پیام_didout = FALSE;
#ifdef FEAT_IMAGE
    shift_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#endif

    بازگشت did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ایستا تهی
lineclear(unsigned off, صحیح width, صحیح attr)
{
    (تهی)vim_memset(Screen_سطرها + off, ' ', (size_t)width * اندازه(schar_T));
    اگر (enc_utf8)
	(تهی)vim_memset(Screen_سطرها_UC + off, 0,
					  (size_t)width * اندازه(u8char_T));
    (تهی)vim_memset(ScreenAttrs + off, attr, (size_t)width * اندازه(sattr_T));
    (تهی)vim_memset(ScreenCols + off, -1, (size_t)width * اندازه(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ایستا تهی
lineinvalid(unsigned off, صحیح width)
{
    (تهی)vim_memset(ScreenAttrs + off, -1, (size_t)width * اندازه(sattr_T));
    (تهی)vim_memset(ScreenCols + off, -1, (size_t)width * اندازه(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    تهی
سطر_was_clobbered(صحیح screen_lnum)
{
    lineinvalid(سطر_Offset[screen_lnum], (صحیح)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ایستا تهی
linecopy(صحیح to, صحیح from, win_T *wp)
{
    unsigned	off_to = سطر_Offset[to] + wp->w_wincol;
    unsigned	off_from = سطر_Offset[from] + wp->w_wincol;

    mch_memmove(Screen_سطرها + off_to, Screen_سطرها + off_from,
	    wp->w_width * اندازه(schar_T));
    اگر (enc_utf8)
    {
	صحیح	i;

	mch_memmove(Screen_سطرها_UC + off_to, Screen_سطرها_UC + off_from,
		wp->w_width * اندازه(u8char_T));
	برای (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_سطرها_C[i] + off_to, Screen_سطرها_C[i] + off_from,
		    wp->w_width * اندازه(u8char_T));
    }
    اگر (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * اندازه(schar_T));
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * اندازه(sattr_T));
    mch_memmove(ScreenCols + off_to, ScreenCols + off_from,
	    wp->w_width * اندازه(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    صحیح
can_clear(char_u *p)
{
    بازگشت (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_normal_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_normal_bg_color == 0)
#وگرنه
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
    تهی
screen_start(تهی)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    تهی
windgoto(صحیح row, صحیح col)
{
    sattr_T	    *p;
    صحیح		    i;
    صحیح		    plan;
    صحیح		    cost;
    صحیح		    wouldbe_col;
    صحیح		    noinvcurs;
    char_u	    *bs;
    صحیح		    goto_cost;
    صحیح		    attr;

#define GOTO_COST   7	// assume a term_windgoto() takes about 7 chars
#define HIGHL_COST  5	// assume unhighlight takes 5 chars

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    اگر (Screen_سطرها == پوچ)
	بازگشت;
    اگر (col == screen_cur_col && row == screen_cur_row)
	بازگشت;

    // Check for valid position.
    اگر (row < 0)	// window without text lines?
	row = 0;
    اگر (row >= screen_Rows)
	row = screen_Rows - 1;
    اگر (col >= screen_Columns)
	col = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    اگر (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_COST;
    وگرنه
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
    اگر (row >= screen_cur_row && screen_cur_col < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = پوچ;			    // init for GCC
	attr = screen_attr;
	اگر (row == screen_cur_row && col < screen_cur_col)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    اگر (*T_LE)
		bs = T_LE;		    // "cursor left"
	    وگرنه
		bs = T_BC;		    // "backspace character (old)
	    اگر (*bs)
		cost = (screen_cur_col - col) * (صحیح)STRLEN(bs);
	    وگرنه
		cost = 999;
	    اگر (col + 1 < cost)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_col = 0;
		cost = 1;		    // CR is just one character
	    }
	    وگرنه
	    {
		plan = PLAN_LE;
		wouldbe_col = col;
	    }
	    اگر (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	وگرنه اگر (row > screen_cur_row)
	{
	    plan = PLAN_NL;
	    wouldbe_col = 0;
	    cost = (row - screen_cur_row) * 2;  // CR LF
	    اگر (noinvcurs)		    // will stop highlighting
	    {
		cost += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	وگرنه
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
	اگر (i > 0)
	    cost += i;
	اگر (cost < goto_cost && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + سطر_Offset[row] + wouldbe_col;
	    وقتی (i && *p++ == attr)
		--i;
	    اگر (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		اگر (*--p == 0)
		{
		    cost += noinvcurs;
		    وقتی (i && *p++ == 0)
			--i;
		}
		اگر (i != 0)
		    cost = 999;	// different attributes, don't do it
	    }
	    اگر (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		برای (i = wouldbe_col; i < col; ++i)
		    اگر (Screen_سطرها_UC[سطر_Offset[row] + i] != 0)
		    {
			cost = 999;
			بشکن;
		    }
	    }
#ifdef FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    اگر (cost < 999)
		برای (i = wouldbe_col; i < col; ++i)
		    اگر (popup_is_under_opacity(row, i))
		    {
			cost = 999;
			بشکن;
		    }
#endif
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	اگر (cost < goto_cost)
	{
	    اگر (plan == PLAN_LE)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		وقتی (screen_cur_col > col)
		{
		    out_str(bs);
		    --screen_cur_col;
		}
	    }
	    وگرنه اگر (plan == PLAN_CR)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		out_char('\r');
		screen_cur_col = 0;
	    }
	    وگرنه اگر (plan == PLAN_NL)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		وقتی (screen_cur_row < row)
		{
		    out_char('\n');
		    ++screen_cur_row;
		}
		screen_cur_col = 0;
	    }

	    i = col - screen_cur_col;
	    اگر (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		اگر (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    وقتی (i-- > 0)
			out_char(*T_ND);
		}
		وگرنه
		{
		    صحیح	off;

		    off = سطر_Offset[row] + screen_cur_col;
		    وقتی (i-- > 0)
		    {
			اگر (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			out_flush_check();
			out_char(Screen_سطرها[off]);
			اگر (enc_dbcs == DBCS_JPNU
				&& Screen_سطرها[off] == 0x8e)
			    out_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    وگرنه
	cost = 999;

    اگر (cost >= goto_cost)
    {
	اگر (noinvcurs)
	    screen_stop_highlight();
	اگر (row == screen_cur_row && (col > screen_cur_col)
		&& *T_CRI != NUL)
	    term_cursor_راست(col - screen_cur_col);
	وگرنه
	    term_windgoto(row, col);
    }
    screen_cur_row = row;
    screen_cur_col = col;
}

/*
 * Set cursor to its position in the current window.
 */
    تهی
setcursor(تهی)
{
    setcursor_mayforce(FALSE);
}

#ifdef FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    صحیح
cursor_screen_cells(تهی)
{
    بازگشت has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#endif

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    تهی
setcursor_mayforce(صحیح force)
{
    اگر (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ifdef FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((صحیح)curwin->w_width - curwin->w_wcol
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
    صحیح
win_ins_سطرها(
    win_T	*wp,
    صحیح		row,
    صحیح		سطر_شمار,
    صحیح		invalid,
    صحیح		mayclear)
{
    صحیح		did_delete;
    صحیح		nextrow;
    صحیح		lastrow;
    صحیح		retval;

    اگر (invalid)
	wp->w_سطرها_valid = 0;

    // with only a few lines it's not worth the effort
    اگر (wp->w_height < 5)
	بازگشت FAIL;

    // with the popup menu visible this might not work correctly
    اگر (pum_visible())
	بازگشت FAIL;

    اگر (سطر_شمار > wp->w_height - row)
	سطر_شمار = wp->w_height - row;

    retval = win_do_سطرها(wp, row, سطر_شمار, mayclear, FALSE, 0);
    اگر (retval != MAYBE)
	بازگشت retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    اگر (wp->w_next != پوچ || wp->w_status_height)
    {
	اگر (screen_del_سطرها(0, W_WINROW(wp) + wp->w_height - سطر_شمار,
				  سطر_شمار, (صحیح)Rows, FALSE, 0, پوچ) == OK)
	    did_delete = TRUE;
	وگرنه اگر (wp->w_next)
	    بازگشت FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    اگر (!did_delete)
    {
	wp->w_redr_status = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + wp->w_status_height;
	lastrow = nextrow + سطر_شمار;
	اگر (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - سطر_شمار, lastrow - سطر_شمار,
		  wp->w_wincol, (صحیح)W_ENDCOL(wp), ' ', ' ', 0);
    }

    اگر (screen_ins_سطرها(0, W_WINROW(wp) + row, سطر_شمار, (صحیح)Rows, 0, پوچ)
								      == FAIL)
    {
	// deletion will have messed up other windows
	اگر (did_delete)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(W_NEXT(wp));
	}
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    صحیح
win_del_سطرها(
    win_T	*wp,
    صحیح		row,
    صحیح		سطر_شمار,
    صحیح		invalid,
    صحیح		mayclear,
    صحیح		clear_attr)	    // for clearing lines
{
    صحیح		retval;

    اگر (invalid)
	wp->w_سطرها_valid = 0;

    اگر (سطر_شمار > wp->w_height - row)
	سطر_شمار = wp->w_height - row;

    retval = win_do_سطرها(wp, row, سطر_شمار, mayclear, TRUE, clear_attr);
    اگر (retval != MAYBE)
	بازگشت retval;

    اگر (screen_del_سطرها(0, W_WINROW(wp) + row, سطر_شمار,
				   (صحیح)Rows, FALSE, clear_attr, پوچ) == FAIL)
	بازگشت FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    اگر (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	اگر (screen_ins_سطرها(0, W_WINROW(wp) + wp->w_height - سطر_شمار,
			      سطر_شمار, (صحیح)Rows, clear_attr, پوچ) == FAIL)
	{
	    wp->w_redr_status = true;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    وگرنه
	redraw_cmdline = TRUE;
    بازگشت OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ایستا صحیح
win_do_سطرها(
    win_T	*wp,
    صحیح		row,
    صحیح		سطر_شمار,
    صحیح		mayclear,
    صحیح		del,
    صحیح		clear_attr)
{
    صحیح		retval;

    اگر (!redrawing() || سطر_شمار <= 0)
	بازگشت FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    اگر (no_win_do_سطرها_ins && !del)
	بازگشت FAIL;

    // only a few lines left: redraw is faster
    اگر (mayclear && Rows - سطر_شمار < 5
	    && wp->w_width == topframe->fr_width)
    {
	اگر (!no_win_do_سطرها_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	بازگشت FAIL;
    }

#ifdef FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    اگر (popup_visible)
	بازگشت FAIL;
#endif

    // Delete all remaining lines
    اگر (row + سطر_شمار >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		wp->w_wincol, (صحیح)W_ENDCOL(wp), ' ', ' ', 0);
	بازگشت OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    اگر (!no_win_do_سطرها_ins)
	clear_cmdline = TRUE;

#اگر defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    اگر (tabpanel_width() > 0)
	بازگشت FAIL;
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
    اگر (scroll_region || wp->w_width != topframe->fr_width)
    {
	اگر (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_set(wp, row);
	اگر (del)
	    retval = screen_del_سطرها(W_WINROW(wp) + row, 0, سطر_شمار,
				    wp->w_height - row, FALSE, clear_attr, wp);
	وگرنه
	    retval = screen_ins_سطرها(W_WINROW(wp) + row, 0, سطر_شمار,
					   wp->w_height - row, clear_attr, wp);
	اگر (scroll_region && (wp->w_width == topframe->fr_width
		    || *T_CSV != NUL))
	    scroll_region_reset();
	بازگشت retval;
    }

    اگر (wp->w_next != پوچ && p_tf) // don't delete/insert on fast terminal
	بازگشت FAIL;

    بازگشت MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ایستا تهی
win_rest_invalid(win_T *wp)
{
    وقتی (wp != پوچ)
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
    صحیح
screen_ins_سطرها(
    صحیح		off,
    صحیح		row,
    صحیح		سطر_شمار,
    صحیح		end,
    صحیح		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    صحیح		i;
    صحیح		j;
    unsigned	temp;
    صحیح		cursor_row;
    صحیح		cursor_col = 0;
    صحیح		type;
    صحیح		result_empty;
    صحیح		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    اگر (!screen_valid(TRUE)
	     || سطر_شمار <= 0 || سطر_شمار > p_ttyscroll
	     || end > Rows
#ifdef FEAT_CLIPBOARD
	     || (clip_star.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#endif
	     )
	بازگشت FAIL;

#ifdef FEAT_PROP_POPUP
    اگر (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	shift_image_placements(off + row, off + end, سطر_شمار);
# endif
	بازگشت FAIL;
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
    result_empty = (row + سطر_شمار >= end);
    اگر (wp != پوچ && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	اگر (سطر_شمار > 3)
	    بازگشت FAIL;
	type = USE_REDRAW;
    }
    وگرنه اگر (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    وگرنه اگر (*T_CAL != NUL && (سطر_شمار > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    وگرنه اگر (*T_CDL != NUL && result_empty && (سطر_شمار > 1 || !can_ce))
	type = USE_T_CDL;
    وگرنه اگر (*T_AL != NUL)
	type = USE_T_AL;
    وگرنه اگر (can_ce && result_empty)
	type = USE_T_CE;
    وگرنه اگر (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    وگرنه اگر (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    وگرنه
	بازگشت FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    اگر (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	بازگشت screen_del_سطرها(off, row, سطر_شمار, end, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    اگر (*T_DB)
	screen_del_سطرها(off, end - سطر_شمار, سطر_شمار, end, FALSE, 0, wp);

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    اگر (off + row > 0 || (wp != پوچ && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    وگرنه
	clip_scroll_selection(-سطر_شمار);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, سطر_شمار);
#endif

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

#ifdef FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(row + off <= gui.cursor_row);
#endif

    اگر (wp != پوچ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    اگر (*T_CCS != NUL)	   // cursor relative to region
	cursor_row = row;
    وگرنه
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    برای (i = 0; i < سطر_شمار; ++i)
    {
	اگر (wp != پوچ && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = end - 1 - i;
	    وقتی ((j -= سطر_شمار) >= row)
		linecopy(j + سطر_شمار, j, wp);
	    j += سطر_شمار;
	    اگر (can_clear((char_u *)" "))
		lineclear(سطر_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    وگرنه
		lineinvalid(سطر_Offset[j] + wp->w_wincol, wp->w_width);
	    سطر_Wraps[j] = FALSE;
	}
	وگرنه
	{
	    j = end - 1 - i;
	    temp = سطر_Offset[j];
	    وقتی ((j -= سطر_شمار) >= row)
	    {
		سطر_Offset[j + سطر_شمار] = سطر_Offset[j];
		سطر_Wraps[j + سطر_شمار] = سطر_Wraps[j];
	    }
	    سطر_Offset[j + سطر_شمار] = temp;
	    سطر_Wraps[j + سطر_شمار] = FALSE;
	    اگر (can_clear((char_u *)" "))
		lineclear(temp, (صحیح)Columns, clear_attr);
	    وگرنه
		lineinvalid(temp, (صحیح)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, cursor_col);
    اگر (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    اگر (type == USE_REDRAW)
	redraw_block(row, end, wp);
    وگرنه اگر (type == USE_T_CAL)
    {
	term_append_سطرها(سطر_شمار);
	screen_start();		// don't know where cursor is now
    }
    وگرنه
    {
	برای (i = 0; i < سطر_شمار; i++)
	{
	    اگر (type == USE_T_AL)
	    {
		اگر (i && cursor_row != 0)
		    windgoto(cursor_row, cursor_col);
		out_str(T_AL);
	    }
	    وگرنه  // type == USE_T_SR
		out_str(T_SR);
	    screen_start();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    اگر (type == USE_T_SR && *T_DA)
    {
	برای (i = 0; i < سطر_شمار; ++i)
	{
	    windgoto(off + i, cursor_col);
	    out_str(T_CE);
	    screen_start();	    // don't know where cursor is now
	}
    }

#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    اگر (gui.in_use)
	out_flush();	// always flush after a scroll
#endif
    بازگشت OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    صحیح
screen_del_سطرها(
    صحیح		off,
    صحیح		row,
    صحیح		سطر_شمار,
    صحیح		end,
    صحیح		force,		// even when line_count > p_ttyscroll
    صحیح		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    صحیح		j;
    صحیح		i;
    unsigned	temp;
    صحیح		cursor_row;
    صحیح		cursor_col = 0;
    صحیح		cursor_end;
    صحیح		result_empty;	// result is empty until end of region
    صحیح		can_delete;	// deleting line codes can be used
    صحیح		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    اگر (!screen_valid(TRUE)
	    || سطر_شمار <= 0
	    || (!force && سطر_شمار > p_ttyscroll)
	    || end > Rows
#ifdef FEAT_CLIPBOARD
	    || (clip_star.state != SELECT_CLEARED && redrawing_for_callback > 0)
#endif
       )
	بازگشت FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + سطر_شمار >= end;

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
    اگر (wp != پوچ && wp->w_width != topframe->fr_width && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	اگر (سطر_شمار > 3)
	    بازگشت FAIL;
	type = USE_REDRAW;
    }
    وگرنه اگر (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    وگرنه اگر (row == 0 && (
#ifndef AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    سطر_شمار == 1 ||
#endif
						*T_CDL == NUL))
	type = USE_NL;
    وگرنه اگر (*T_CDL != NUL && سطر_شمار > 1 && can_delete)
	type = USE_T_CDL;
    وگرنه اگر (can_clear(T_CE) && result_empty
	    && (wp == پوچ || wp->w_width == topframe->fr_width))
	type = USE_T_CE;
    وگرنه اگر (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    وگرنه اگر (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    وگرنه
	بازگشت FAIL;

#ifdef FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    اگر (off + row > 0 || (wp != پوچ && wp->w_width != topframe->fr_width))
	clip_clear_selection(&clip_star);
    وگرنه
	clip_scroll_selection(سطر_شمار);
#endif
#ifdef FEAT_IMAGE
    shift_image_placements(off + row, off + end, -سطر_شمار);
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

    اگر (wp != پوچ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_col = wp->w_wincol;

    اگر (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_row = row;
	cursor_end = end;
    }
    وگرنه
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
    برای (i = 0; i < سطر_شمار; ++i)
    {
	اگر (wp != پوچ && wp->w_width != topframe->fr_width)
	{
	    // need to copy part of a line
	    j = row + i;
	    وقتی ((j += سطر_شمار) <= end - 1)
		linecopy(j - سطر_شمار, j, wp);
	    j -= سطر_شمار;
	    اگر (can_clear((char_u *)" "))
		lineclear(سطر_Offset[j] + wp->w_wincol,
			wp->w_width, clear_attr);
	    وگرنه
		lineinvalid(سطر_Offset[j] + wp->w_wincol, wp->w_width);
	    سطر_Wraps[j] = FALSE;
	}
	وگرنه
	{
	    // whole width, moving the line pointers is faster
	    j = row + i;
	    temp = سطر_Offset[j];
	    وقتی ((j += سطر_شمار) <= end - 1)
	    {
		سطر_Offset[j - سطر_شمار] = سطر_Offset[j];
		سطر_Wraps[j - سطر_شمار] = سطر_Wraps[j];
	    }
	    سطر_Offset[j - سطر_شمار] = temp;
	    سطر_Wraps[j - سطر_شمار] = FALSE;
	    اگر (can_clear((char_u *)" "))
		lineclear(temp, (صحیح)Columns, clear_attr);
	    وگرنه
		lineinvalid(temp, (صحیح)Columns);
	}
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    اگر (screen_attr != clear_attr)
	screen_stop_highlight();
    اگر (clear_attr != 0)
	screen_start_highlight(clear_attr);

    // redraw the characters
    اگر (type == USE_REDRAW)
	redraw_block(row, end, wp);
    وگرنه اگر (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_row, cursor_col);
	out_str(T_CD);
	screen_start();			// don't know where cursor is now
    }
    وگرنه اگر (type == USE_T_CDL)
    {
	windgoto(cursor_row, cursor_col);
	term_delete_سطرها(سطر_شمار);
	screen_start();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    وگرنه اگر (type == USE_NL)
    {
	windgoto(cursor_end - 1, cursor_col);
	برای (i = سطر_شمار; --i >= 0; )
	    out_char('\n');		// cursor will remain on same line
    }
    وگرنه
    {
	برای (i = سطر_شمار; --i >= 0; )
	{
	    اگر (type == USE_T_DL)
	    {
		windgoto(cursor_row, cursor_col);
		out_str(T_DL);		// delete a line
	    }
	    وگرنه // type == USE_T_CE
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
    اگر (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	برای (i = سطر_شمار; i > 0; --i)
	{
	    windgoto(cursor_end - i, cursor_col);
	    out_str(T_CE);		// erase a line
	    screen_start();		// don't know where cursor is now
	}
    }

#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif

#ifdef FEAT_GUI
    gui_can_update_cursor();
    اگر (gui.in_use)
	out_flush();	// always flush after a scroll
#endif

    بازگشت OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    صحیح
skip_showmode(تهی)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    اگر (global_busy
	    || پیام_silent != 0
	    || !redrawing()
	    || ((!stuff_empty() || typebuf.tb_len > 0 || using_script())
		&& !کلید_Typed))
    {
	redraw_mode = TRUE;		// show mode later
	بازگشت TRUE;
    }
    بازگشت FALSE;
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
    صحیح
showmode(تهی)
{
    صحیح		need_clear;
    صحیح		length = 0;
    صحیح		do_mode;
    صحیح		attr;
    صحیح		nwr_save;
    صحیح		sub_attr;
    صحیح		show_ruler_with_pum = FALSE;

    do_mode = p_smd && پیام_silent == 0
	    && ((State & MODE_درج)
		|| restart_edit != NUL
		|| VIsual_active);
    اگر (do_mode || reg_recording != 0)
    {
	اگر (skip_showmode())
	    بازگشت 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	اگر (clear_cmdline && cmdline_row < Rows - 1)
	    پیام_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	پیام_pos_mode();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	اگر (do_mode)
	{
	    پیام_puts_attr("--", attr);
#اگر defined(FEAT_XIM)
	    اگر (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# وگرنه
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		پیام_puts_attr(" IM", attr);
# وگرنه
		پیام_puts_attr(" XIM", attr);
# endif
#endif
	    // CTRL-X in Insert mode
	    اگر (edit_submode != پوچ && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		length = (Rows - پیام_row) * cmdline_width - 3;
		اگر (edit_submode_extra != پوچ)
		    length -= vim_strsize(edit_submode_extra);
		اگر (length > 0)
		{
		    اگر (edit_submode_pre != پوچ)
			length -= vim_strsize(edit_submode_pre);
		    اگر (length - vim_strsize(edit_submode) > 0)
		    {
			اگر (edit_submode_pre != پوچ)
			    پیام_puts_attr((نویسه *)edit_submode_pre, attr);
			پیام_puts_attr((نویسه *)edit_submode, attr);
		    }
		    اگر (edit_submode_extra != پوچ)
		    {
			پیام_puts_attr(" ", attr);  // add a space in between
			اگر ((صحیح)edit_submode_highl < (صحیح)HLF_شمار)
			    sub_attr = HL_ATTR(edit_submode_highl);
			وگرنه
			    sub_attr = attr;
			پیام_puts_attr((نویسه *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    وگرنه
	    {
		اگر (State & VREPLACE_FLAG)
		    پیام_puts_attr(_(" VREPLACE"), attr);
		وگرنه اگر (State & REPLACE_FLAG)
		    پیام_puts_attr(_(" REPLACE"), attr);
		وگرنه اگر (State & MODE_درج)
		{
#ifdef FEAT_RIGHTLEFT
		    اگر (p_ri)
			پیام_puts_attr(_(" REVERSE"), attr);
#endif
		    پیام_puts_attr(_(" INSERT"), attr);
		}
		وگرنه اگر (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    پیام_puts_attr(_(" (insert)"), attr);
		وگرنه اگر (restart_edit == 'R')
		    پیام_puts_attr(_(" (replace)"), attr);
		وگرنه اگر (restart_edit == 'V')
		    پیام_puts_attr(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		اگر (p_hkmap)
		    پیام_puts_attr(_(" Hebrew"), attr);
#endif
#ifdef FEAT_KEYMAP
		اگر (State & MODE_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    اگر (curwin->w_p_arab)
			پیام_puts_attr(_(" Arabic"), attr);
		    وگرنه
# endif
			اگر (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    پیام_puts_attr((نویسه *)NameBuff, attr);
		}
#endif
		اگر ((State & MODE_درج) && p_paste)
		    پیام_puts_attr(_(" (paste)"), attr);

		اگر (VIsual_active)
		{
		    نویسه *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    انتخاب ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			حالت 0:	p = N_(" VISUAL"); بشکن;
			حالت 1: p = N_(" VISUAL LINE"); بشکن;
			حالت 2: p = N_(" VISUAL BLOCK"); بشکن;
			حالت 4: p = N_(" SELECT"); بشکن;
			حالت 5: p = N_(" SELECT LINE"); بشکن;
			پیشفرض: p = N_(" SELECT BLOCK"); بشکن;
		    }
		    پیام_puts_attr(_(p), attr);
		}
		پیام_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	اگر (reg_recording != 0
		&& edit_submode == پوچ)    // otherwise it gets too long
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	اگر (need_clear || clear_cmdline || redraw_mode)
	    پیام_clr_eos();
	پیام_didout = FALSE;		// overwrite this message
	length = پیام_col;
	پیام_col = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    وگرنه اگر (clear_cmdline && پیام_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	پیام_clr_cmdline();
    وگرنه اگر (redraw_mode)
    {
	پیام_pos_mode();
	پیام_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    اگر (VIsual_active)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    اگر (redrawing() && lastwin->w_status_height == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_mode = FALSE;
    clear_cmdline = FALSE;

    بازگشت length;
}

/*
 * Position for a mode message.
 */
    ایستا تهی
پیام_pos_mode(تهی)
{
    پیام_col = 0;
    پیام_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    تهی
unshowmode(صحیح force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    اگر (!redrawing() || (!force && char_avail() && !کلید_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    وگرنه
	clearmode();
}

/*
 * Clear the mode message.
 */
    تهی
clearmode(تهی)
{
    صحیح save_پیام_row = پیام_row;
    صحیح save_پیام_col = پیام_col;

    پیام_pos_mode();
    اگر (reg_recording != 0)
	recording_mode(HL_ATTR(HLF_CM));
    پیام_clr_eos();

    پیام_col = save_پیام_col;
    پیام_row = save_پیام_row;
}

    ایستا تهی
recording_mode(صحیح attr)
{
    پیام_puts_attr(_("recording"), attr);
    اگر (shortmess(SHM_RECORDING))
	بازگشت;

    نویسه s[4];

    sprintf(s, " @%c", reg_recording);
    پیام_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ایستا صحیح
mouse_has_any(تهی)
{
    بازگشت mouse_has(MOUSE_NORMAL) || mouse_has(MOUSE_درج)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    تهی
draw_tabline(تهی)
{
    صحیح		tabcount = 0;
    tabpage_T	*tp;
    صحیح		tabwidth;
    صحیح		col = 0;
    صحیح		scol = 0;
    صحیح		attr;
    win_T	*wp;
    win_T	*cwp;
    صحیح		wincount;
    صحیح		modified;
    صحیح		c;
    صحیح		len;
    صحیح		attr_sel;
    صحیح		attr_nosel = 0;
    صحیح		attr_fill = 0;
    char_u	*p;
    صحیح		room;
    صحیح		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
#ifdef FEAT_TERMGUICOLORS
					    && !p_tgc
#endif
					    );
    bool	override_success;

#اگر defined(FEAT_TABPANEL)
    col = firstwin->w_wincol;
#endif

    اگر (Screen_سطرها == پوچ)
	بازگشت;
    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    اگر (gui_use_tabline())
    {
	gui_update_tabline();
	بازگشت;
    }
#endif

    اگر (tabline_height() < 1)
	بازگشت;

#اگر defined(FEAT_STL_OPT)
    clear_TabPageIdxs();

    // Use the 'tabline' option if it's set.
    اگر (*p_tal != NUL)
	win_redr_custom(پوچ, FALSE);
    وگرنه
#endif
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_width - 1 + tabcount / 2) / tabcount;
	اگر (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	برای (tp = first_tabpage; tp != پوچ && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    override_success = افزودن_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    اگر (tp->tp_topframe == topframe)
		attr = attr_sel;
	    اگر (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    اگر (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    اگر (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    وگرنه
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    برای (wincount = 0; wp != پوچ; wp = wp->w_next, ++wincount)
		اگر (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    اگر (modified || wincount > 1)
	    {
		اگر (wincount > 1)
		{
		    len = vim_snprintf((نویسه *)NameBuff, MAXPATHL, "%d", wincount);
		    اگر (col + len >= Columns - 3)
			بشکن;
		    screen_puts_len(NameBuff, len, 0, col,
#اگر defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#وگرنه
					 attr
#endif
					       );
		    col += len;
		}
		اگر (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    اگر (room > 0)
	    {
		صحیح n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		اگر (has_mbyte)
		    وقتی (len > room)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		وگرنه اگر (len > room)
		{
		    p += len - room;
		    len = room;
		}
		n = Columns - col - 1;
		اگر (len > n)
		    len = n;

		screen_puts_len(p, (صحیح)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    وقتی (scol < col)
		TabPageIdxs[scol++] = tabcount;

	    اگر (override_success)
		برداشت_highlight_overrides();
	}

	اگر (use_sep_chars)
	    c = '_';
	وگرنه
	    c = ' ';
	screen_fill(0, 1, col, (صحیح)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	اگر (p_sc && *p_sloc == 't')
	{
	    صحیح	n = (صحیح)Columns - col - (tabcount > 1) * 3;
	    صحیح	width = MIN(10, n);

	    اگر (width > 0)
		screen_puts_len(showcmd_buf, width, 0, (صحیح)Columns
			    - width - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	اگر (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (صحیح)Columns - 1, attr_nosel);
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
    تهی
get_trans_bufname(buf_T *buf)
{
    اگر (buf_spname(buf) != پوچ)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    وگرنه
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    صحیح
fillchar_status(صحیح *attr, win_T *wp)
{
    صحیح	    fill;
    bool    override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ifdef FEAT_TERMINAL
    اگر (bt_terminal(wp->w_buffer))
    {
	اگر (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	وگرنه
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    وگرنه
#endif
    اگر (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    وگرنه
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    اگر (override_success)
	برداشت_highlight_overrides();
    بازگشت fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ایستا bool
vsep_row_is_curwin(win_T *wp, صحیح row)
{
    اگر (wp == curwin)
	بازگشت true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    اگر (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_width
	    && row >= curwin->w_winrow
	    && row < W_WINROW(curwin) + curwin->w_height
						   + curwin->w_status_height)
	بازگشت true;

    بازگشت false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    صحیح
fillchar_vsep(صحیح *attr, win_T *wp, صحیح row)
{
    bool override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
    اگر (vsep_row_is_curwin(wp, row))
	*attr = HL_ATTR(HLF_C);
    وگرنه
	*attr = HL_ATTR(HLF_CNC);
    اگر (override_success)
	برداشت_highlight_overrides();

    اگر (*attr == 0 && wp->w_fill_chars.vert == ' ')
	بازگشت '|';
    وگرنه
	بازگشت wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    صحیح
redrawing(تهی)
{
#ifdef FEAT_EVAL
    اگر (disable_redraw_for_testing)
	بازگشت 0;
    وگرنه
#endif
	بازگشت ((RedrawingDisabled == 0
#ifdef FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#endif
		) && !(p_lz && char_avail() && !کلید_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    صحیح
messaging(تهی)
{
    بازگشت (!(p_lz && char_avail() && !کلید_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    // columns needed by standard ruler

    تهی
comp_col(تهی)
{
    صحیح last_has_status = last_stl_height(FALSE) > 0;

    sc_col = 0;
    ru_col = 0;
    اگر (p_ru)
    {
#ifdef FEAT_STL_OPT
	ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
#وگرنه
	ru_col = COL_RULER + 1;
#endif
	// no last status line, adjust sc_col
	اگر (!last_has_status)
	    sc_col = ru_col;
    }
    اگر (p_sc && *p_sloc == 'l')
    {
	sc_col += SHOWCMD_COLS;
	اگر (!p_ru || last_has_status)	    // no need for separating space
	    ++sc_col;
    }
    sc_col = cmdline_width - sc_col;
    ru_col = cmdline_width - ru_col;
    اگر (sc_col <= 0)		// screen too narrow, will become a mess
	sc_col = 1;
    اگر (ru_col <= 0)
	ru_col = 1;
#ifdef FEAT_EVAL
    set_vim_var_nr(VV_ECHOSPACE, sc_col - 1);
#endif
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    صحیح
عدد_width(win_T *wp)
{
    صحیح		n;
    linenr_T	lnum;

    اگر (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_height;
    وگرنه
	// cursor line shows absolute line number
	lnum = wp->w_buffer->b_ml.ml_سطر_شمار;

    اگر (lnum == wp->w_nrwidth_سطر_شمار && wp->w_nuw_cached == wp->w_p_nuw)
	بازگشت wp->w_nrwidth_width;
    wp->w_nrwidth_سطر_شمار = lnum;

    n = 0;
    انجامبده
    {
	lnum /= 10;
	++n;
    } وقتی (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    اگر (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    اگر (n < 2 && get_first_valid_sign(wp) != پوچ
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    بازگشت n;
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    صحیح
screen_screencol(تهی)
{
    بازگشت screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    صحیح
screen_screenrow(تهی)
{
    بازگشت screen_cur_row;
}
#endif

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ایستا صحیح
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    اگر (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T رقم = 0;
	صحیح	    bytes;
	صحیح	    n;

	برای (bytes = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; bytes > 0; --bytes)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    اگر (n < 0)
		بازگشت 0;
	    رقم = رقم * 256 + n;
	}
	*p += 2;
	بازگشت رقم;
    }
    بازگشت mb_ptr2char_adv(p);
}

ساختار charstab
{
    صحیح	    *cp;
    string_T	name;
};

#define CHARSTAB_ENTRY(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ایستا fill_chars_T fill_chars;
ایستا ساختار charstab filltab[] =
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
#اگر defined(FEAT_TABPANEL)
    CHARSTAB_ENTRY(&fill_chars.tpl_vert,    "tpl_vert"),
#endif
    CHARSTAB_ENTRY(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ENTRY(&fill_chars.truncrl,	    "truncrl"),
};
ایستا lcs_chars_T lcs_chars;
ایستا ساختار charstab lcstab[] =
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
#وگرنه
    CHARSTAB_ENTRY(پوچ,		    "conceal"),
#endif
    CHARSTAB_ENTRY(پوچ,		    "multispace"),
    CHARSTAB_ENTRY(پوچ,		    "leadmultispace")
};

    ایستا نویسه *
field_مقدار_err(نویسه *errbuf, size_t errbuflen, نویسه *fmt, char_u *field)
{
    اگر (errbuf == پوچ)
	بازگشت "";
    vim_snprintf(errbuf, errbuflen, _(fmt), field);
    بازگشت errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ایستا نویسه *
set_chars_option(win_T *wp, char_u *مقدار, صحیح is_listchars, صحیح apply,
						نویسه *errbuf, size_t errbuflen)
{
    صحیح	    round, i, entries;
    char_u  *p, *s;
    صحیح	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *last_multispace = پوچ;  // Last occurrence of "multispace:"
    char_u  *last_lmultispace = پوچ; // Last occurrence of "leadmultispace:"
    صحیح	    multispace_len = 0;	      // Length of lcs-multispace string
    صحیح	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    ساختار charstab *tab;

    اگر (is_listchars)
    {
	tab = lcstab;
	CLEAR_FIELD(lcs_chars);
	entries = ARRAY_LENGTH(lcstab);
	اگر (wp->w_p_lcs[0] == NUL)
	    مقدار = p_lcs;  // local value is empty, use the global value
    }
    وگرنه
    {
	tab = filltab;
	entries = ARRAY_LENGTH(filltab);
	اگر (wp->w_p_fcs[0] == NUL)
	    مقدار = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    برای (round = 0; round <= (apply ? 1 : 0); ++round)
    {
	صحیح has_tab = FALSE, has_leadtab = FALSE;

	اگر (round > 0)
	{
	    // After checking that the value is valid: set defaults.
	    اگر (is_listchars)
	    {
		برای (i = 0; i < entries; ++i)
		    اگر (tab[i].cp != پوچ)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		اگر (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(صحیح, multispace_len + 1);
		    اگر (lcs_chars.multispace != پوچ)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		وگرنه
		    lcs_chars.multispace = پوچ;

		اگر (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(صحیح, lead_multispace_len + 1);
		    اگر (lcs_chars.leadmultispace != پوچ)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		وگرنه
		    lcs_chars.leadmultispace = پوچ;
	    }
	    وگرنه
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
#اگر defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#endif
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = مقدار;
	وقتی (*p)
	{
	    برای (i = 0; i < entries; ++i)
	    {
		اگر (!(STRNCMP(p, tab[i].name.string, tab[i].name.length) == 0 && p[tab[i].name.length] == ':'))
		    ادامه;

		s = p + tab[i].name.length + 1;

		اگر (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    اگر (round == 0)
		    {
			// Get length of lcs-multispace string in first round
			last_multispace = p;
			multispace_len = 0;
			وقتی (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (char2cells(c1) > 1)
				بازگشت field_مقدار_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			اگر (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    بازگشت field_مقدار_err(errbuf, errbuflen,
				    e_wrong_عدد_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    وگرنه
		    {
			صحیح multispace_pos = 0;

			وقتی (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (p == last_multispace && lcs_chars.multispace != پوچ)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    بشکن;
		}

		اگر (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    اگر (round == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			// round
			last_lmultispace = p;
			lead_multispace_len = 0;
			وقتی (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (char2cells(c1) > 1)
				بازگشت field_مقدار_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			اگر (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    بازگشت field_مقدار_err(errbuf, errbuflen,
				    e_wrong_عدد_of_characters_for_field_str,
				    tab[i].name.string);
		    }
		    وگرنه
		    {
			صحیح multispace_pos = 0;

			وقتی (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (p == last_lmultispace && lcs_chars.leadmultispace != پوچ)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    بشکن;
		}

		c2 = c3 = 0;
		اگر (*s == NUL)
		    بازگشت field_مقدار_err(errbuf, errbuflen,
				    e_wrong_عدد_of_characters_for_field_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		اگر (char2cells(c1) > 1)
		    بازگشت field_مقدار_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		اگر (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    اگر (*s == NUL)
			بازگشت field_مقدار_err(errbuf, errbuflen,
				    e_wrong_عدد_of_characters_for_field_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    اگر (char2cells(c2) > 1)
			بازگشت field_مقدار_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    اگر (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			اگر (char2cells(c3) > 1)
			    بازگشت field_مقدار_err(errbuf, errbuflen,
					 e_wrong_character_width_for_field_str,
					 tab[i].name.string);
		    }
		    اگر (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    وگرنه  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		اگر (*s == ',' || *s == NUL)
		{
		    اگر (round > 0)
		    {
			اگر (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			وگرنه اگر (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			وگرنه اگر (tab[i].cp != پوچ)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    بشکن;
		}
		وگرنه
		    بازگشت field_مقدار_err(errbuf, errbuflen,
				    e_wrong_عدد_of_characters_for_field_str,
				    tab[i].name.string);
	    }

	    اگر (i == entries)
		بازگشت e_invalid_argument;

	    اگر (*p == ',')
		++p;
	}

	اگر (is_listchars && has_leadtab && !has_tab)
	    بازگشت e_leadtab_requires_tab;
    }

    اگر (apply)
    {
	اگر (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	وگرنه
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    بازگشت پوچ;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    نویسه *
set_fillchars_option(win_T *wp, char_u *val, صحیح apply, نویسه *errbuf,
							      size_t errbuflen)
{
    بازگشت set_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    نویسه *
set_listchars_option(win_T *wp, char_u *val, صحیح apply, نویسه *errbuf,
							      size_t errbuflen)
{
    بازگشت set_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(filltab))
	بازگشت پوچ;

    بازگشت filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(lcstab))
	بازگشت پوچ;

    بازگشت lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    نویسه *
check_chars_options(تهی)
{
    tabpage_T   *tp;
    win_T	    *wp;

    اگر (set_listchars_option(curwin, p_lcs, FALSE, پوچ, 0) != پوچ)
	بازگشت e_conflicts_with_مقدار_of_listchars;
    اگر (set_fillchars_option(curwin, p_fcs, FALSE, پوچ, 0) != پوچ)
	بازگشت e_conflicts_with_مقدار_of_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (set_listchars_option(wp, wp->w_p_lcs, FALSE, پوچ, 0) != پوچ)
	    بازگشت e_conflicts_with_مقدار_of_listchars;
	اگر (set_fillchars_option(wp, wp->w_p_fcs, FALSE, پوچ, 0) != پوچ)
	    بازگشت e_conflicts_with_مقدار_of_fillchars;
    }
    بازگشت پوچ;
}
