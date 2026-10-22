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
ساکن صحیح	screen_attr = 0;

ساکن خالی screen_char_2(غیرعلامتی off, صحیح صف, صحیح ستون);
ساکن صحیح screenclear2(صحیح doclear);
ساکن خالی lineclear(غیرعلامتی off, صحیح چوڑائی, صحیح attr);
ساکن خالی lineinvalid(غیرعلامتی off, صحیح چوڑائی);
ساکن صحیح win_do_سطریں(win_T *wp, صحیح صف, صحیح سطر_گنتی, صحیح mayclear, صحیح del, صحیح clear_attr);
ساکن خالی win_rest_invalid(win_T *wp);
ساکن خالی پیغام_pos_منوال(خالی);
ساکن خالی recording_منوال(صحیح attr);

// Ugly global: overrule attribute used by screen_char()
ساکن صحیح screen_char_attr = 0;

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
	واپس FALSE;
    اگر (get_real_state() & منوال_VISUAL)
	c = 'v';
    ورنہ اگر (State & منوال_داخل_کرو)
	c = 'i';
    ورنہ اگر (State & منوال_معمول)
	c = 'n';
    ورنہ اگر (State & منوال_CMDLINE)
	c = 'c';
    ورنہ
	واپس FALSE;
    واپس vim_strchr(wp->w_p_cocu, c) != تہی;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    خالی
conceal_check_cursor_سطر(صحیح was_concealed)
{
    اگر (curwin->w_p_cole <= 0 || conceal_cursor_سطر(curwin) == was_concealed)
	واپس;

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
#اگر_ختم

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    صحیح
get_win_attr(win_T *wp)
{
    صحیح	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

    اگر (win_attr != 0)
    {
	اگر (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	ورنہ
	    win_attr = 0;
    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    ورنہ اگر (WIN_IS_POPUP(wp))
    {
	اگر (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	ورنہ
	    win_attr = HL_ATTR(HLF_نکالو);    // Popup
    }
#اگر_ختم

    اگر (override_success)
	نکالو_highlight_overrides();
    واپس win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ساکن صحیح
screen_fill_انتہا(
	win_T *wp,
	صحیح	c1,
	صحیح	c2,
	صحیح	off,
	صحیح	چوڑائی,
	صحیح	صف,
	صحیح	endrow,
	صحیح	attr)
{
    صحیح	    nn = off + چوڑائی;

    اگر (nn > wp->w_چوڑائی)
	nn = wp->w_چوڑائی;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (صحیح)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    ورنہ
#اگر_ختم
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (صحیح)wp->w_wincol + nn,
		c1, c2, attr);
    واپس nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    خالی
win_draw_انتہا(
    win_T	*wp,
    صحیح		c1,
    صحیح		c2,
    صحیح		draw_margin,
    صحیح		صف,
    صحیح		endrow,
    hlf_T	hl)
{
    صحیح		n = 0;
    صحیح		attr = HL_ATTR(hl);
    صحیح		win_attr = get_win_attr(wp);
    bool	override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    اگر (draw_margin)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	صحیح	fdc = compute_foldcolumn(wp, 0);

	اگر (fdc > 0)
	    // draw the fold column
	    n = screen_fill_انتہا(wp, ' ', ' ', n, fdc,
		      صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	اگر (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_انتہا(wp, ' ', ' ', n, 2,
		      صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#اگر_ختم
	اگر ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == تہی)
	    // draw the number column
	    n = screen_fill_انتہا(wp, ' ', ' ', n, نمبر_چوڑائی(wp) + 1,
		       صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    ورنہ
#اگر_ختم
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (صحیح)W_ENDCOL(wp), c1, c2, attr);
    }

    سیٹ_خالی_خانے_صفیں(wp, صف);

    اگر (override_success)
	نکالو_highlight_overrides();
}

#اگر defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    صحیح
compute_foldcolumn(win_T *wp, صحیح ستون)
{
    صحیح wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    صحیح n = wp->w_چوڑائی - (ستون + wmw);

    واپس MIN(wp->w_p_fdc, n);
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
    صحیح		پہلا_level;
    صحیح		خالی_خانے_2;
    صحیح		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_شمار = 0;
    صحیح		علامت = 0;
    صحیح		len = 0;
    صحیح		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    خالی_خانے_2 = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    پہلا_level = level - fdc - closed + 1 + خالی_خانے_2;
    اگر (پہلا_level < 1)
	پہلا_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    برائے (i = 0; i < n; i++)
    {
	اگر (win_foldinfo.fi_lnum == lnum
		&& پہلا_level + i >= win_foldinfo.fi_کم_level)
	    علامت = wp->w_fill_chars.foldopen;
	ورنہ اگر (پہلا_level == 1)
	    علامت = wp->w_fill_chars.foldsep;
	ورنہ اگر (wp->w_fill_chars.foldinner != NUL)
	    علامت = wp->w_fill_chars.foldinner;
	ورنہ اگر (پہلا_level + i <= 9)
	    علامت = '0' + پہلا_level + i;
	ورنہ
	    علامت = '>';

	len = utf_char2bytes(علامت, &p[byte_شمار]);
	byte_شمار += len;
	اگر (پہلا_level + i >= level)
	{
	    i++;
	    توڑو;
	}
    }

    اگر (closed)
    {
	اگر (علامت != 0)
	{
	    // rollback length and the character
	    byte_شمار -= len;
	    اگر (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_شمار, ' ', len);
	}
	علامت = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(علامت, &p[byte_شمار]);
	byte_شمار += len;
    }

    واپس MAX(byte_شمار + (fdc - i), (size_t)fdc);
}
#اگر_ختم // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ساکن صحیح
comp_char_differs(صحیح off_from, صحیح off_مقصد)
{
    صحیح	    i;

    برائے (i = 0; i < Screen_mco; ++i)
    {
	اگر (Screen_سطریں_C[i][off_from] != Screen_سطریں_C[i][off_مقصد])
	    واپس TRUE;
	اگر (Screen_سطریں_C[i][off_from] == 0)
	    توڑو;
    }
    واپس FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ساکن صحیح
char_needs_redraw(صحیح off_from, صحیح off_مقصد, صحیح ستون_تعداد)
{
    اگر (ستون_تعداد > 0
	    && ((Screen_سطریں[off_from] != Screen_سطریں[off_مقصد]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_مقصد])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_سطریں[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_سطریں[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_مقصد]
			: (ستون_تعداد > 1 && Screen_سطریں[off_from + 1]
						 != Screen_سطریں[off_مقصد + 1])))
		|| (enc_utf8
		    && (Screen_سطریں_UC[off_from] != Screen_سطریں_UC[off_مقصد]
			|| (Screen_سطریں_UC[off_from] != 0
			    && comp_char_differs(off_from, off_مقصد))
			|| ((*mb_off2cells)(off_from, off_from + ستون_تعداد) > 1
			    && Screen_سطریں[off_from + 1]
						!= Screen_سطریں[off_مقصد + 1])))))
	واپس TRUE;
    واپس FALSE;
}

#اگر defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    صحیح
screen_get_current_سطر_off(خالی)
{
    واپس (صحیح)(current_Screen_سطر - Screen_سطریں);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ساکن صحیح
blocked_by_popup(صحیح صف, صحیح ستون)
{
    صحیح off;

    اگر (!popup_visible)
	واپس FALSE;
    off = صف * screen_Columns + ستون;
    واپس popup_نقاب[off] > screen_zindex || popup_transparent[off];
}
#اگر_ختم

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    خالی
reset_screen_attr(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    ورنہ
#اگر_ختم
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ساکن صحیح
skip_for_popup(صحیح صف, صحیح ستون)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    اگر (pum_under_menu(صف, ستون, TRUE)
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#اگر_ختم
	    )
	واپس TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & منوال_CMDLINE) == 0
	    && pum_under_menu(صف, ستون, FALSE))
	واپس TRUE;
#ورنہ
    اگر (pum_visible()
	    && (State & منوال_CMDLINE) == 0
	    && pum_under_menu(صف, ستون, FALSE))
	واپس TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (blocked_by_popup(صف, ستون))
	واپس TRUE;
#اگر_ختم
    واپس FALSE;
}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ساکن char_u	*suppressed_cells = تہی;
ساکن صحیح	suppressed_صفیں = 0;
ساکن صحیح	suppressed_ستون_تعداد = 0;

    ساکن خالی
mark_suppressed_cell(صحیح صف, صحیح ستون)
{
    اگر (suppressed_cells == تہی
	    || suppressed_صفیں != screen_صفیں
	    || suppressed_ستون_تعداد != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_صفیں * screen_Columns);
	اگر (suppressed_cells == تہی)
	{
	    suppressed_صفیں = 0;
	    suppressed_ستون_تعداد = 0;
	    واپس;
	}
	suppressed_صفیں = screen_صفیں;
	suppressed_ستون_تعداد = screen_Columns;
    }
    suppressed_cells[صف * suppressed_ستون_تعداد + ستون] = TRUE;
}

    ساکن خالی
unmark_suppressed_cell(صحیح صف, صحیح ستون)
{
    اگر (suppressed_cells != تہی
	    && صف >= 0 && صف < suppressed_صفیں
	    && ستون >= 0 && ستون < suppressed_ستون_تعداد)
	suppressed_cells[صف * suppressed_ستون_تعداد + ستون] = FALSE;
}

    ساکن صحیح
is_suppressed_cell(صحیح صف, صحیح ستون)
{
    واپس suppressed_cells != تہی
	&& صف >= 0 && صف < suppressed_صفیں
	&& ستون >= 0 && ستون < suppressed_ستون_تعداد
	&& suppressed_cells[صف * suppressed_ستون_تعداد + ستون];
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
قسمتعریف ساخت {
    صحیح		popup_attr;	// get_win_attr(screen_opacity_popup)
    صحیح		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ساکن اندرونی خالی
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ساکن اندرونی صحیح
popup_بنیاد_attr_or(صحیح صف, صحیح ستون, صحیح fallback)
{
    صحیح		بنیاد = fallback;

    popup_get_بنیاد_screen_cell(صف, ستون, تہی, &بنیاد, تہی);
    واپس بنیاد;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ساکن اندرونی صحیح
popup_blend_with_بنیاد(
	ثابت popup_opacity_T	*po,
	صحیح			char_attr,
	صحیح			بنیاد_attr,
	صحیح			blend_fg)
{
    صحیح		combined = hl_combine_attr(po->popup_attr, char_attr);

    واپس hl_blend_attr(بنیاد_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ساکن خالی
resolve_wide_char_opacity_attrs(
	صحیح صف, صحیح col1, صحیح col2,
	sattr_T *attr1, sattr_T *attr2)
{
    صحیح		bg1, bg2;
    صحیح		base1 = 0;
    صحیح		base2 = 0;
    attrentry_T	*ae;

    اگر (*attr1 == *attr2)
	واپس;

    popup_get_بنیاد_screen_cell(صف, col1, تہی, &base1, تہی);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != تہی && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ورنہ
    bg1 = (ae != تہی && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_بنیاد_screen_cell(صف, col2, تہی, &base2, تہی);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != تہی && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ورنہ
    bg2 = (ae != تہی && ae->ae_u.cterm.bg_color != 0);
# endif

    اگر (bg1 && !bg2)
	*attr1 = *attr2;
    ورنہ اگر (!bg1 && bg2)
	*attr2 = *attr1;
}
#اگر_ختم

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
    خالی
screen_سطر(
	win_T	*wp,
	صحیح	صف,
	صحیح	coloff,
	صحیح	endcol,
	صحیح	clear_چوڑائی,
	colnr_T	آخری_vcol,
	صحیح	flags غیر_مستعمل)
{
    غیرعلامتی	    off_from;
    غیرعلامتی	    off_مقصد;
    غیرعلامتی	    max_off_from;
    غیرعلامتی	    max_off_مقصد;
    صحیح		    ستون = 0;
    صحیح		    hl;
    صحیح		    force = FALSE;	// force update rest of the line
    صحیح		    redraw_this		// bool: does character need redraw?
#اگر_تعریف_شدہ FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#اگر_ختم
				;
    صحیح		    redraw_اگلا;	// redraw_this for next character
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    صحیح		    بدلا_this;	// TRUE if character changed
    صحیح		    بدلا_اگلا;	// TRUE if next character changed
#اگر_ختم
    صحیح		    clear_اگلا = FALSE;
    صحیح		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    صحیح		    drawing_opacity_popup =
				screen_opacity_popup != تہی
				&& (flags & SLF_POPUP);
    CLEAR_میدان(po);
    اگر (drawing_opacity_popup)
	popup_opacity_init(&po);
#اگر_ختم

    // Check for illegal row and col, just in case.
    اگر (صف >= صفیں)
	صف = صفیں - 1;
    اگر (endcol > Columns)
	endcol = Columns;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    clip_may_clear_selection(صف, صف);
#اگر_ختم

    off_from = (غیرعلامتی)(current_Screen_سطر - Screen_سطریں);
    off_مقصد = سطر_انحراف[صف] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_مقصد = سطر_انحراف[صف] + screen_Columns;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	اگر (clear_چوڑائی > 0)
	{
	    صحیح clear_آغاز = ستون;

	    جب (ستون <= endcol && Screen_سطریں[off_مقصد] == ' '
		    && ScreenAttrs[off_مقصد] == 0
				  && (!enc_utf8 || Screen_سطریں_UC[off_مقصد] == 0))
	    {
		++off_مقصد;
		++ستون;
	    }
	    اگر (ستون <= endcol)
		screen_fill(صف, صف + 1, ستون + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    برائے (صحیح i = endcol; i >= clear_آغاز; i--)
		Screen_ستون_تعداد[off_مقصد + (i - ستون)] =
		    (flags & SLF_INC_VCOL) ? ++آخری_vcol : آخری_vcol;
	}
	ستون = endcol + 1;
	off_مقصد = سطر_انحراف[صف] + ستون + coloff;
	off_from += ستون;
	endcol = (clear_چوڑائی > 0 ? clear_چوڑائی : -clear_چوڑائی);
    }
#اگر_ختم // FEAT_RIGHTLEFT

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    اگر (coloff > 0 && enc_utf8
		   && Screen_سطریں[off_مقصد] == 0
		   && Screen_سطریں_UC[off_مقصد - 1] != 0
		   && (*mb_char2cells)(Screen_سطریں_UC[off_مقصد - 1]) > 1)
    {
	Screen_سطریں[off_مقصد - 1] = ' ';
	Screen_سطریں_UC[off_مقصد - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	اگر (screen_opacity_popup == تہی)
# endif
	    screen_char(off_مقصد - 1, صف, ستون + coloff - 1);
    }
#اگر_ختم

    redraw_اگلا = char_needs_redraw(off_from, off_مقصد, endcol - ستون);
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    بدلا_اگلا = redraw_اگلا;
#اگر_ختم

    جب (ستون < endcol)
    {
	اگر (has_mbyte && (ستون + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	ورنہ
	    char_cells = 1;

	redraw_this = redraw_اگلا;
	redraw_اگلا = force || char_needs_redraw(off_from + char_cells,
			      off_مقصد + char_cells, endcol - ستون - char_cells);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	اگر (!redraw_this && is_suppressed_cell(صف, ستون + coloff))
	    redraw_this = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	بدلا_this = بدلا_اگلا;
	بدلا_اگلا = redraw_اگلا;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	اگر (redraw_اگلا && gui.اندر_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_مقصد + char_cells];
	    اگر (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    اگر (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#اگر_ختم
	// skip the second cell for double-width characters.
	اگر (redraw_this && char_cells == 2 && skip_for_popup(صف, ستون + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	اگر (redraw_this && skip_for_popup(صف, ستون + coloff))
	    redraw_this = FALSE;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	صحیح opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	اگر (drawing_opacity_popup
		&& Screen_سطریں[off_from] == ' '
		&& (!enc_utf8 || Screen_سطریں_UC[off_from] == 0))
	{
	    صحیح popup_src_attr = ScreenAttrs[off_from];
	    صحیح bg_is_خالی_خانے = Screen_سطریں[off_مقصد] == 0
				&& (!enc_utf8 || Screen_سطریں_UC[off_مقصد] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    اگر (bg_is_خالی_خانے
		    && off_مقصد > 0
		    && enc_utf8 && Screen_سطریں_UC[off_مقصد - 1] != 0
		    && utf_char2cells(Screen_سطریں_UC[off_مقصد - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    ورنہ اگر (Screen_سطریں[off_مقصد] != 0
		    || (enc_utf8 && Screen_سطریں_UC[off_مقصد] != 0))
	    {
		صحیح bg_char_cells = (enc_utf8 && Screen_سطریں_UC[off_مقصد] != 0)
				? utf_char2cells(Screen_سطریں_UC[off_مقصد]) : 1;

		اگر (bg_char_cells == 2)
		{
		    اگر (ستون + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_مقصد + 1 >= max_off_مقصد)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			صحیح بنیاد_attr;
			Screen_سطریں[off_مقصد] = ' ';
			اگر (enc_utf8)
			    Screen_سطریں_UC[off_مقصد] = 0;
			بنیاد_attr = popup_بنیاد_attr_or(صف, ستون + coloff,
							     ScreenAttrs[off_مقصد]);
			ScreenAttrs[off_مقصد] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, بنیاد_attr, TRUE);
			screen_char(off_مقصد, صف, ستون + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			جاؤ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    اگر (!(Screen_سطریں[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_سطریں_UC[off_from + 1] == 0)))
			جاؤ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		صحیح بنیاد_attr = popup_بنیاد_attr_or(صف, ستون + coloff,
							     ScreenAttrs[off_مقصد]);
		ScreenAttrs[off_مقصد] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, بنیاد_attr, TRUE);
		screen_char(off_مقصد, صف, ستون + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		اگر (bg_char_cells == 2)
		{
		    صحیح بنیاد_attr2 = popup_بنیاد_attr_or(صف, ستون + coloff + 1,
							 ScreenAttrs[off_مقصد + 1]);
		    ScreenAttrs[off_مقصد + 1] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, بنیاد_attr2, TRUE);
		    اگر (po.blend == 100)
			resolve_wide_char_opacity_attrs(صف,
				ستون + coloff, ستون + coloff + 1,
				&ScreenAttrs[off_مقصد], &ScreenAttrs[off_مقصد + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    ورنہ اگر (bg_is_خالی_خانے)
	    {
		صحیح بنیاد_attr;
		Screen_سطریں[off_مقصد] = ' ';
		بنیاد_attr = popup_بنیاد_attr_or(صف, ستون + coloff,
							     ScreenAttrs[off_مقصد]);
		ScreenAttrs[off_مقصد] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, بنیاد_attr, TRUE);
		screen_char(off_مقصد, صف, ستون + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#اگر_ختم

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
#اگر_تعریف_شدہ FEAT_GUI
		    && !gui.اندر_use
#اگر_ختم
		    && ScreenAttrs[off_مقصد] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_مقصد])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(صف, ستون + coloff);
		برآمد_str(T_CE);		// clear rest of this screen line
		screen_آغاز();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_اگلا = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		اگر (ستون + coloff > 0 && ScreenAttrs[off_مقصد - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_مقصد - 1];
		    رکن_windgoto(صف, ستون + coloff);
		    screen_stop_highlight();
		}
		ورنہ
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    اگر (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		اگر (char_cells == 1
			&& ستون + 1 < endcol
			&& (*mb_off2cells)(off_مقصد, max_off_مقصد) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_سطریں[off_مقصد + 1] = 0;
		    redraw_اگلا = TRUE;
		}
		ورنہ اگر (char_cells == 2
			&& ستون + 2 < endcol
			&& (*mb_off2cells)(off_مقصد, max_off_مقصد) == 1
			&& (*mb_off2cells)(off_مقصد + 1, max_off_مقصد) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_سطریں[off_مقصد + 2] = 0;
		    redraw_اگلا = TRUE;
		}

		اگر (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_مقصد] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    اگر (has_mbyte && ستون + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_مقصد, max_off_مقصد) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_مقصد, max_off_مقصد) == 1
			    && (*mb_off2cells)(off_مقصد + 1, max_off_مقصد) > 1)))
		clear_اگلا = TRUE;

	    Screen_سطریں[off_مقصد] = Screen_سطریں[off_from];
	    اگر (enc_utf8)
	    {
		Screen_سطریں_UC[off_مقصد] = Screen_سطریں_UC[off_from];
		اگر (Screen_سطریں_UC[off_from] != 0)
		{
		    صحیح	    i;

		    برائے (i = 0; i < Screen_mco; ++i)
			Screen_سطریں_C[i][off_مقصد] = Screen_سطریں_C[i][off_from];
		}
	    }
	    اگر (char_cells == 2)
	    {
		Screen_سطریں[off_مقصد + 1] = Screen_سطریں[off_from + 1];
		اگر (enc_utf8)
		    Screen_سطریں_UC[off_مقصد + 1] = 0;
	    }

#اگر defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    اگر (
# ifdef FEAT_GUI
		    gui.اندر_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    رکن_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_مقصد];
		اگر (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		اگر (hl & HL_BOLD)
		    redraw_اگلا = TRUE;
	    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    اگر (gui.اندر_use && بدلا_this)
		redraw_اگلا = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    اگر (gui.directx_enabled && بدلا_this)
		redraw_this = TRUE;
# endif
#اگر_ختم

	    ScreenAttrs[off_مقصد] = ScreenAttrs[off_from];
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    اگر (drawing_opacity_popup && po.blend > 0)
	    {
		صحیح popup_src_attr = ScreenAttrs[off_from];
		صحیح scol1 = ستون + coloff;
		صحیح base1 = popup_بنیاد_attr_or(صف, scol1, 0);

		اگر (char_cells == 2)
		{
		    صحیح base2 = popup_بنیاد_attr_or(صف, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    صحیح over1 = popup_is_over_opacity(صف, scol1);
		    صحیح over2 = popup_is_over_opacity(صف, scol1 + 1);
		    اگر (over1 != over2)
		    {
			اگر (over1)
			    base1 = base2;
			ورنہ
			    base2 = base1;
		    }
		    ScreenAttrs[off_مقصد + 1] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_مقصد] = popup_blend_with_بنیاد(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    ورنہ
#اگر_ختم
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    اگر (char_cells == 2)
		ScreenAttrs[off_مقصد + 1] = ScreenAttrs[off_مقصد];

	    اگر (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_مقصد, صف, ستون + coloff);
	    ورنہ
		screen_char(off_مقصد, صف, ستون + coloff);
	}
	ورنہ اگر (  p_wiv
#اگر_تعریف_شدہ FEAT_GUI
		&& !gui.اندر_use
#اگر_ختم
		&& ستون + coloff > 0 && off_مقصد > 0)
	{
	    اگر (ScreenAttrs[off_مقصد] == ScreenAttrs[off_مقصد - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    ورنہ اگر (screen_attr != 0)
		screen_stop_highlight();
	}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (!opacity_blank)
#اگر_ختم
	{
	    Screen_ستون_تعداد[off_مقصد] = Screen_ستون_تعداد[off_from];
	    اگر (char_cells == 2)
		Screen_ستون_تعداد[off_مقصد + 1] = Screen_ستون_تعداد[off_from + 1];
	}

	off_مقصد += char_cells;
	off_from += char_cells;
	ستون += char_cells;
    }

    اگر (clear_اگلا && !skip_for_popup(صف, ستون + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_سطریں[off_مقصد] = ' ';
	اگر (enc_utf8)
	    Screen_سطریں_UC[off_مقصد] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (screen_opacity_popup == تہی)
#اگر_ختم
	    screen_char(off_مقصد, صف, ستون + coloff);
    }

    اگر (clear_چوڑائی > 0
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#اگر_ختم
				   )
    {
#اگر_تعریف_شدہ FEAT_GUI
	صحیح آغاز_ستون_2 = ستون;
#اگر_ختم

	// blank out the rest of the line
	جب (ستون < clear_چوڑائی && Screen_سطریں[off_مقصد] == ' '
						  && ScreenAttrs[off_مقصد] == 0
				  && (!enc_utf8 || Screen_سطریں_UC[off_مقصد] == 0)
#اگر_تعریف_شدہ FEAT_PROP_POPUP
				  && !is_suppressed_cell(صف, ستون + coloff)
#اگر_ختم
				  )
	{
	    Screen_ستون_تعداد[off_مقصد] =
			      (flags & SLF_INC_VCOL) ? ++آخری_vcol : آخری_vcol;
	    ++off_مقصد;
	    ++ستون;
	}
	اگر (ستون < clear_چوڑائی)
	{
#اگر_تعریف_شدہ FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    اگر (gui.اندر_use && (ستون > آغاز_ستون_2 || !redraw_this))
	    {
		hl = ScreenAttrs[off_مقصد];
		اگر (hl > HL_ALL || (hl & HL_BOLD))
		{
		    صحیح پچھلا_cells = 1;

		    اگر (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			پچھلا_cells = Screen_سطریں[off_مقصد - 1] == 0 ? 2 : 1;
		    ورنہ اگر (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			غیرعلامتی off = سطر_انحراف[صف];
			غیرعلامتی max_off = سطر_انحراف[صف] + screen_Columns;

			جب (off < off_مقصد)
			{
			    پچھلا_cells = (*mb_off2cells)(off, max_off);
			    off += پچھلا_cells;
			}
		    }

		    اگر (!skip_for_popup(صف, ستون + coloff - پچھلا_cells))
		    {
			اگر (enc_dbcs != 0 && پچھلا_cells > 1)
			    screen_char_2(off_مقصد - پچھلا_cells, صف,
						   ستون + coloff - پچھلا_cells);
			ورنہ
			    screen_char(off_مقصد - پچھلا_cells, صف,
						   ستون + coloff - پچھلا_cells);
		    }
		}
	    }
#اگر_ختم
	    screen_fill(صف, صف + 1, ستون + coloff, clear_چوڑائی + coloff,
								 ' ', ' ', 0);
	    جب (ستون < clear_چوڑائی)
	    {
		Screen_ستون_تعداد[off_مقصد++]
			    = (flags & SLF_INC_VCOL) ? ++آخری_vcol : آخری_vcol;
		++ستون;
	    }
	}
    }

    اگر (clear_چوڑائی > 0
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#اگر_ختم
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	اگر (coloff + ستون < firstwin->w_wincol + topframe->fr_چوڑائی)
	{
	    اگر (!skip_for_popup(صف, ستون + coloff))
	    {
		صحیح c;

		c = fillchar_vsep(&hl, wp, صف);
		اگر (Screen_سطریں[off_مقصد] != (schar_T)c
			|| (enc_utf8 && (صحیح)Screen_سطریں_UC[off_مقصد]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_مقصد] != hl)
		{
		    Screen_سطریں[off_مقصد] = c;
		    ScreenAttrs[off_مقصد] = hl;
		    اگر (enc_utf8)
		    {
			اگر (c >= 0x80)
			{
			    Screen_سطریں_UC[off_مقصد] = c;
			    Screen_سطریں_C[0][off_مقصد] = 0;
			    Screen_سطریں[off_مقصد] = 0x80; // avoid storing zero
			}
			ورنہ
			    Screen_سطریں_UC[off_مقصد] = 0;
		    }
		    screen_char(off_مقصد, صف, ستون + coloff);
		}
	    }
	}
	ورنہ
	    سطر_Wraps[صف] = FALSE;
    }

    اگر (override_success)
	نکالو_highlight_overrides();
}

#اگر defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    خالی
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    صحیح		t;

    برائے (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#اگر_ختم

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    خالی
draw_vsep_win(win_T *wp, صحیح صف)
{
    اگر (!wp->w_vsep_چوڑائی)
	واپس;

    صحیح content_انتہا = W_WINROW(wp) + wp->w_اونچائی;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    صحیح آغاز_صف = (صف == 0) ? wp->w_winrow : W_WINROW(wp) + صف;
    برائے (صحیح r = آغاز_صف; r < content_انتہا; ++r)
    {
	صحیح hl;
	صحیح c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    اگر (wp->w_حالت_اونچائی != 0)
    {
	صحیح hl;
	صحیح c;
	اگر (stl_connected(wp))
	    c = fillchar_حالت(&hl, wp);
	ورنہ
	    c = fillchar_vsep(&hl, wp, content_انتہا);
	برائے (صحیح r = content_انتہا; r < content_انتہا + wp->w_حالت_اونچائی; ++r)
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
    چوکھٹا_T	*fr;

    fr = wp->w_چوکھٹا;
    جب (fr->fr_والد != تہی)
    {
	اگر (fr->fr_والد->fr_layout == FR_ستون)
	{
	    اگر (fr->fr_اگلا != تہی)
		توڑو;
	}
	ورنہ
	{
	    اگر (fr->fr_اگلا != تہی)
		واپس TRUE;
	}
	fr = fr->fr_والد;
    }
    واپس FALSE;
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

    اگر (wp->w_بفر->b_p_iminsert != B_IMODE_LMAP)
	واپس 0;

#اگر_تعریف_شدہ FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	مقصد_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_بفر;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_مقصد_string(مقصد_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    اگر (p == تہی || *p == NUL)
#اگر_ختم
    {
#اگر_تعریف_شدہ FEAT_KEYMAP
	اگر (wp->w_بفر->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_بفر->b_p_keymap;
	ورنہ
#اگر_ختم
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((حرف *)buf, len, (حرف *)fmt, p);
#اگر_تعریف_شدہ FEAT_EVAL
    vim_free(s);
#اگر_ختم
    اگر (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    واپس plen;
}

#اگر defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    خالی
win_redr_custom(
    win_T	*wp,
    صحیح		draw_ruler)	// TRUE or FALSE
{
    ساکن صحیح	entered = FALSE;
    صحیح		attr;
    صحیح		curattr;
    صحیح		صف;
    صحیح		ستون = 0;
    صحیح		maxwidth;
    صحیح		چوڑائی;
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
	واپس;
    entered = TRUE;

    // setup environment for the task at hand
    اگر (wp == تہی)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	صف = 0;
	ستون = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_چوڑائی;
	opt_name = (char_u *)"tabline";
    }
    ورنہ
    {
	override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	صف = statusline_صف(wp);
	fillchar = fillchar_حالت(&attr, wp);
	صحیح اندر_حالت_سطر_2 = wp->w_حالت_اونچائی != 0;
	maxwidth = اندر_حالت_سطر_2 ? wp->w_چوڑائی : cmdline_چوڑائی;

	اگر (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    اگر (*stl == '%')
	    {
		اگر (*++stl == '-')
		    stl++;
		اگر (atoi((حرف *)stl))
		    جب (VIM_ISDIGIT(*stl))
			stl++;
		اگر (*stl++ != '(')
		    stl = p_ruf;
	    }
	    ستون = ru_ستون - (cmdline_چوڑائی - maxwidth);
	    اگر (ستون < (maxwidth + 1) / 2)
		ستون = (maxwidth + 1) / 2;
	    maxwidth -= ستون;
	    اگر (!اندر_حالت_سطر_2)
	    {
		صف = صفیں - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	ورنہ
	{
	    opt_name = (char_u *)"statusline";
	    اگر (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    ورنہ
		stl = p_stl;
	}

	اگر (اندر_حالت_سطر_2)
	    ستون += wp->w_wincol;
	ورنہ
	    ستون += cmdline_ستون_off;
    }

    اگر (maxwidth <= 0)
	جاؤ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == تہی ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_حالت_اونچائی;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == تہی) ? (char_u *)"" : stl;
    صحیح ستون_save = ستون;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **برآمد_regions = تہی;
    صحیح			*برآمد_گنتی = تہی;
    صحیح			region_بنیاد_ستون = 0;
    اگر (!draw_ruler)
    {
	اگر (wp != تہی)
	{
	    برآمد_regions = &wp->w_stl_click;
	    برآمد_گنتی = &wp->w_stl_click_گنتی;
	    region_بنیاد_ستون = wp->w_wincol;
	}
	ورنہ
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    برآمد_regions = &tabline_stl_click;
	    برآمد_گنتی = &tabline_stl_click_گنتی;
	    region_بنیاد_ستون = firstwin->w_wincol;
	}

	برائے (n = 0; n < *برآمد_گنتی; n++)
	    vim_free((*برآمد_regions)[n].funcname);
	VIM_CLEAR(*برآمد_regions);
	*برآمد_گنتی = 0;
    }

    صحیح حاصل_hl = 0;
    برائے (صحیح i = 0; i < stlh_cnt; i++)
    {
	ستون = ستون_save;
	buf[0] = NUL;
	چوڑائی = build_stl_str_hl_mline(ewp, buf, حجم(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&حاصل_hl);

	// Make all characters printable.
	p = transstr(buf);
	اگر (p != تہی)
	{
	    len = vim_snprintf((حرف *)buf, حجم(buf), "%s", p);
	    vim_free(p);
	}
	ورنہ
	    len = (صحیح)STRLEN(buf);

	// fill up with "fillchar"
	جب (چوڑائی < maxwidth && len < (صحیح)حجم(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++چوڑائی;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	برائے (n = 0; hltab[n].آغاز != تہی; n++)
	{
	    len = (صحیح)(hltab[n].آغاز - p);
	    screen_puts_len(p, len, صف + i, ستون, curattr);
	    ستون += vim_strnsize(p, len);
	    p = hltab[n].آغاز;

	    اگر (hltab[n].userhl == 0)
		curattr = attr;
	    ورنہ اگر (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    ورنہ اگر (wp != تہی && wp != curwin && bt_terminal(wp->w_بفر)
						    && wp->w_حالت_اونچائی != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    ورنہ اگر (wp != تہی && bt_terminal(wp->w_بفر)
						    && wp->w_حالت_اونچائی != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    ورنہ اگر (wp != تہی && wp != curwin && wp->w_حالت_اونچائی != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    ورنہ
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, صف + i, ستون, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	اگر (برآمد_regions != تہی)
	{
	    صحیح click_گنتی = 0;
	    برائے (n = 0; clicktab[n].آغاز != تہی; n++)
		click_گنتی++;

	    اگر (click_گنتی > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*برآمد_regions,
			حجم(stl_click_region_T)
				    * (*برآمد_گنتی + click_گنتی));
		اگر (new_arr != تہی)
		{
		    char_u  *cur_funcname = تہی;
		    صحیح	    cur_minwid = 0;
		    صحیح	    region_آغاز = region_بنیاد_ستون;

		    *برآمد_regions = new_arr;

		    len = 0;
		    p = buf;
		    برائے (n = 0; clicktab[n].آغاز != تہی; n++)
		    {
			len += vim_strnsize(p,
					    (صحیح)(clicktab[n].آغاز - p));
			p = clicktab[n].آغاز;

			اگر (cur_funcname != تہی)
			{
			    stl_click_region_T *r =
					&(*برآمد_regions)[*برآمد_گنتی];
			    r->صف = صف + i;
			    r->ستون_آغاز = region_آغاز;
			    r->ستون_انتہا = region_بنیاد_ستون + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*برآمد_گنتی)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_آغاز = region_بنیاد_ستون + len;
		    }

		    // Close final region if it extends to the end.
		    اگر (cur_funcname != تہی)
		    {
			stl_click_region_T *r =
					&(*برآمد_regions)[*برآمد_گنتی];
			r->صف = صف + i;
			r->ستون_آغاز = region_آغاز;
			r->ستون_انتہا = region_بنیاد_ستون + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*برآمد_گنتی)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	برائے (n = 0; clicktab[n].آغاز != تہی; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    اگر (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    اگر (stl != تہی)
	vim_free(stl_tmp);

    اگر (wp == تہی)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	ستون = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	برائے (n = 0; tabtab[n].آغاز != تہی; n++)
	{
	    len += vim_strnsize(p, (صحیح)(tabtab[n].آغاز - p));
	    جب (ستون < len)
		Tab_صفحہ_Idxs[ستون++] = fillchar;
	    p = tabtab[n].آغاز;
	    fillchar = tabtab[n].userhl;
	}
	جب (ستون < firstwin->w_wincol + topframe->fr_چوڑائی)
	    Tab_صفحہ_Idxs[ستون++] = fillchar;
    }

theend:
    اگر (override_success)
	نکالو_highlight_overrides();
    entered = FALSE;
}

#اگر_ختم // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    خالی
screen_putchar(صحیح c, صحیح صف, صحیح ستون, صحیح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    اگر (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ورنہ
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, صف, ستون, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    خالی
screen_getbytes(صحیح صف, صحیح ستون, char_u *بائٹیں, صحیح *attrp)
{
    غیرعلامتی off;

    // safety check
    اگر (Screen_سطریں == تہی || صف >= screen_صفیں || ستون >= screen_Columns)
	واپس;

    off = سطر_انحراف[صف] + ستون;
    اگر (attrp != تہی)
	*attrp = ScreenAttrs[off];
    بائٹیں[0] = Screen_سطریں[off];
    بائٹیں[1] = NUL;

    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0)
	بائٹیں[utfc_char2bytes(off, بائٹیں)] = NUL;
    ورنہ اگر (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
    {
	بائٹیں[0] = Screen_سطریں[off];
	بائٹیں[1] = ScreenLines2[off];
	بائٹیں[2] = NUL;
    }
    ورنہ اگر (enc_dbcs && MB_BYTE2LEN(بائٹیں[0]) > 1)
    {
	بائٹیں[1] = Screen_سطریں[off + 1];
	بائٹیں[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ساکن صحیح
screen_comp_differs(صحیح off, صحیح *u8cc)
{
    صحیح	    i;

    برائے (i = 0; i < Screen_mco; ++i)
    {
	اگر (Screen_سطریں_C[i][off] != (u8char_T)u8cc[i])
	    واپس TRUE;
	اگر (u8cc[i] == 0)
	    توڑو;
    }
    واپس FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    خالی
screen_puts(
    char_u	*متن,
    صحیح		صف,
    صحیح		ستون,
    صحیح		attr)
{
    screen_puts_len(متن, -1, صف, ستون, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    خالی
screen_puts_len(
    char_u	*متن,
    صحیح		textlen,
    صحیح		صف,
    صحیح		ستون,
    صحیح		attr_arg)
{
    صحیح		attr = attr_arg;
    غیرعلامتی	off;
    char_u	*ptr = متن;
    صحیح		len = textlen;
    صحیح		c;
    غیرعلامتی	max_off;
    صحیح		mbyte_blen = 1;
    صحیح		mbyte_cells = 1;
    صحیح		u8c = 0;
    صحیح		u8cc[MAX_MCO];
    صحیح		clear_اگلا_cell = FALSE;
#اگر_تعریف_شدہ FEAT_ARABIC
    صحیح		پچھلا_c = 0;		// previous Arabic character
    صحیح		pc, nc, nc1;
    صحیح		pcc[MAX_MCO];
#اگر_ختم
    صحیح		force_redraw_this;
    صحیح		force_redraw_اگلا = FALSE;
    صحیح		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    اگر (Screen_سطریں == تہی
	    || صف >= screen_صفیں || صف < 0
	    || ستون >= screen_Columns || ستون < 0)
	واپس;
    off = سطر_انحراف[صف] + ستون;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    اگر (has_mbyte && ستون > 0 && ستون < screen_Columns
#اگر_تعریف_شدہ FEAT_GUI
	    && !gui.اندر_use
#اگر_ختم
	    && mb_fix_ستون(ستون, صف) != ستون)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	اگر (!skip_for_popup(صف, ستون - 1))
	{
	    Screen_سطریں[off - 1] = ' ';
	    اگر (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    ورنہ اگر (enc_utf8)
	    {
		Screen_سطریں_UC[off - 1] = 0;
		Screen_سطریں_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, صف, ستون - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_اگلا = TRUE;
    }

    max_off = سطر_انحراف[صف] + screen_Columns;
    جب (ستون < screen_Columns
	    && (len < 0 || (صحیح)(ptr - متن) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	اگر (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (صحیح)((متن + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    اگر (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    ورنہ اگر (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    ورنہ	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (صحیح)((متن + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#اگر_تعریف_شدہ FEAT_ARABIC
		اگر (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    اگر (len >= 0 && (صحیح)(ptr - متن) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    ورنہ
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(صحیح)((متن + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = پچھلا_c;
		    پچھلا_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		ورنہ
		    پچھلا_c = u8c;
#اگر_ختم
		اگر (ستون + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_اگلا;
	force_redraw_اگلا = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	صحیح cell_attr = attr;
	اگر (screen_pum_blend > 0 && pum_bg_attrs != تہی
		&& صف >= pum_bg_چوٹی && صف < pum_bg_bot
		&& ستون < pum_bg_ستون_تعداد)
	{
	    صحیح soff = (صف - pum_bg_چوٹی) * pum_bg_ستون_تعداد + ستون;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_سطریں[off] != c
		|| (mbyte_cells == 2
		    && Screen_سطریں[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_سطریں_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_سطریں_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_فعال
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(صف, ستون)
#اگر_ختم
		;

	اگر ((need_redraw || force_redraw_this) && !skip_for_popup(صف, ستون))
	{
#اگر defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    اگر (need_redraw && Screen_سطریں[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.اندر_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    رکن_is_xterm
# endif
		    ))
	    {
		صحیح	n = ScreenAttrs[off];

		اگر (n > HL_ALL)
		    n = syn_attr2attr(n);
		اگر (n & HL_BOLD)
		    force_redraw_اگلا = TRUE;
	    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    اگر (gui.directx_enabled && need_redraw)
		force_redraw_اگلا = TRUE;
#اگر_ختم
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    اگر (clear_اگلا_cell)
		clear_اگلا_cell = FALSE;
	    ورنہ اگر (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= متن + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_اگلا_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    اگر (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_سطریں[off + mbyte_blen] = 0;
	    Screen_سطریں[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    Screen_ستون_تعداد[off] = -1;
	    اگر (enc_utf8)
	    {
		اگر (c < 0x80 && u8cc[0] == 0)
		    Screen_سطریں_UC[off] = 0;
		ورنہ
		{
		    صحیح	    i;

		    Screen_سطریں_UC[off] = u8c;
		    برائے (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطریں_C[i][off] = u8cc[i];
			اگر (u8cc[i] == 0)
			    توڑو;
		    }
		}
		اگر (mbyte_cells == 2)
		{
		    Screen_سطریں[off + 1] = 0;
		    Screen_سطریں_UC[off + 1] = 0;
		    برائے (صحیح ci = 0; ci < Screen_mco; ++ci)
			Screen_سطریں_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    Screen_ستون_تعداد[off + 1] = -1;
		}
		screen_char(off, صف, ستون);
	    }
	    ورنہ اگر (mbyte_cells == 2)
	    {
		Screen_سطریں[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		Screen_ستون_تعداد[off + 1] = -1;
		screen_char_2(off, صف, ستون);
	    }
	    ورنہ اگر (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, صف, ستون);
	    }
	    ورنہ
		screen_char(off, صف, ستون);
	}
	اگر (has_mbyte)
	{
	    off += mbyte_cells;
	    ستون += mbyte_cells;
	    ptr += mbyte_blen;
	    اگر (clear_اگلا_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	ورنہ
	{
	    ++off;
	    ++ستون;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    اگر (force_redraw_اگلا && ستون < screen_Columns && !skip_for_popup(صف, ستون))
    {
	اگر (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, صف, ستون);
	ورنہ
	    screen_char(off, صف, ستون);
    }
}

#اگر defined(FEAT_تلاش_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    خالی
آغاز_تلاش_hl(خالی)
{
    اگر (!p_hls || no_hlsearch)
	واپس;

    انتہا_تلاش_hl();  // just in case it wasn't called before
    آخری_pat_prog(&screen_تلاش_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    خالی
انتہا_تلاش_hl(خالی)
{
    اگر (screen_تلاش_hl.rm.regprog == تہی)
	واپس;

    vim_regfree(screen_تلاش_hl.rm.regprog);
    screen_تلاش_hl.rm.regprog = تہی;
}
#اگر_ختم

    ساکن خالی
screen_آغاز_highlight(صحیح attr)
{
    attrentry_T *aep = تہی;

    screen_attr = attr;
    اگر (!بھرا_screen
#اگر_تعریف_شدہ MSWIN
	    || !termcap_فعال
#اگر_ختم
       )
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	حرف	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	برآمد_STR(buf);
	واپس;
    }
#اگر_ختم

    اگر (attr > HL_ALL)				// special HL attr.
    {
	اگر (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ورنہ
	    aep = syn_رکن_attr2entry(attr);
	اگر (aep == تہی)	    // did ":syntax clear"
	    attr = 0;
	ورنہ
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
	    اگر (aep == تہی || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		رکن_fg_rgb_color(defguifg);
	    اگر (aep == تہی || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		رکن_bg_rgb_color(defguibg);
	}
	ورنہ اگر (t_colors >= 256)
	{
	    اگر (aep == تہی || aep->ae_u.cterm.fg_color == 0)
		رکن_fg_color(defctermfg);
	    اگر (aep == تہی || aep->ae_u.cterm.bg_color == 0)
		رکن_bg_color(defctermbg);
	}
    }
#اگر_ختم
    اگر ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	برآمد_str(T_MD);
    ورنہ اگر (aep != تہی && cterm_معمول_fg_bold && (
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#اگر_ختم
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	برآمد_str(T_ME);
    اگر ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	برآمد_str(T_SO);
    اگر ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	برآمد_str(T_UCS);
    اگر ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	برآمد_str(T_USS);
    اگر ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	برآمد_str(T_DS);
    اگر ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	برآمد_str(T_CDS);
    اگر (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	برآمد_str(T_US);
    اگر ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	برآمد_str(T_CZH);
    اگر ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	برآمد_str(T_MR);
    اگر ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	برآمد_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    اگر (aep != تہی)
    {
	اگر (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		رکن_font(aep->ae_u.cterm.font);
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	اگر (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		رکن_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	ورنہ
#اگر_ختم
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.fg_color)
		    رکن_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	اگر (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		رکن_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	ورنہ
#اگر_ختم
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.bg_color)
		    رکن_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	اگر (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    اگر (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		رکن_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	ورنہ
#اگر_ختم
	    اگر (t_colors > 1)
	    {
		اگر (aep->ae_u.cterm.ul_color)
		    رکن_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	اگر (!IS_CTERM)
	{
	    اگر (aep->ae_u.رکن.آغاز != تہی)
		برآمد_str(aep->ae_u.رکن.آغاز);
	}
    }
}

    خالی
screen_stop_highlight(خالی)
{
    صحیح	    do_ME = FALSE;	    // output T_ME code
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    صحیح	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#اگر_ختم

    اگر (screen_attr != 0
#اگر_تعریف_شدہ MSWIN
			&& termcap_فعال
#اگر_ختم
					   )
    {
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    حرف	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    برآمد_STR(buf);
	}
	ورنہ
#اگر_ختم
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
		    اگر (aep != تہی && ((
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#اگر_ختم
				aep->ae_u.cterm.fg_color) || (
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#اگر_ختم
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
			    برآمد_str((char_u *)"\033|39m"); // restore FG
			اگر (!do_ME && do_ME_bg)
			    برآمد_str((char_u *)"\033|49m"); // restore BG
		    }
		    ورنہ
		    {
			// Process FG and BG at once.
			اگر (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#اگر_ختم
		}
		ورنہ
		{
		    aep = syn_رکن_attr2entry(screen_attr);
		    اگر (aep != تہی && aep->ae_u.رکن.stop != تہی)
		    {
			اگر (STRCMP(aep->ae_u.رکن.stop, T_ME) == 0)
			    do_ME = TRUE;
			ورنہ
			    برآمد_str(aep->ae_u.رکن.stop);
		    }
		}
		اگر (aep == تہی)	    // did ":syntax clear"
		    screen_attr = 0;
		ورنہ
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
		ورنہ
		    برآمد_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    اگر (is_under && *T_UCE != NUL)
	    {
		اگر (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		ورنہ
		    برآمد_str(T_UCE);
	    }
	    اگر ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		اگر (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		ورنہ
		    برآمد_str(T_UE);
	    }
	    اگر (screen_attr & HL_ITALIC)
	    {
		اگر (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		ورنہ
		    برآمد_str(T_CZR);
	    }
	    اگر (screen_attr & HL_STRIKETHROUGH)
	    {
		اگر (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		ورنہ
		    برآمد_str(T_STE);
	    }
	    اگر (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		برآمد_str(T_ME);

#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    اگر (p_tgc)
	    {
		اگر (cterm_معمول_fg_gui_color != INVALCOLOR)
		    رکن_fg_rgb_color(cterm_معمول_fg_gui_color);
		اگر (cterm_معمول_bg_gui_color != INVALCOLOR)
		    رکن_bg_rgb_color(cterm_معمول_bg_gui_color);
		اگر (cterm_معمول_ul_gui_color != INVALCOLOR)
		    رکن_ul_rgb_color(cterm_معمول_ul_gui_color);
	    }
	    ورنہ
#اگر_ختم
	    {
		اگر (t_colors > 1)
		{
		    // set Normal cterm colors
		    اگر (cterm_معمول_fg_color != 0)
			رکن_fg_color(cterm_معمول_fg_color - 1);
		    اگر (cterm_معمول_bg_color != 0)
			رکن_bg_color(cterm_معمول_bg_color - 1);
		    اگر (cterm_معمول_ul_color != 0)
			رکن_ul_color(cterm_معمول_ul_color - 1);
		    اگر (cterm_معمول_fg_bold)
			برآمد_str(T_MD);
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
    خالی
reset_cterm_colors(خالی)
{
    اگر (!IS_CTERM)
	واپس;

    // set Normal cterm colors
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
    اگر (p_tgc ? (cterm_معمول_fg_gui_color != INVALCOLOR
		|| cterm_معمول_bg_gui_color != INVALCOLOR)
	    : (cterm_معمول_fg_color > 0 || cterm_معمول_bg_color > 0))
#ورنہ
	اگر (cterm_معمول_fg_color > 0 || cterm_معمول_bg_color > 0)
#اگر_ختم
	{
	    برآمد_str(T_OP);
	    screen_attr = -1;
	}
    اگر (cterm_معمول_fg_bold)
    {
	برآمد_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    خالی
screen_char(غیرعلامتی off, صحیح صف, صحیح ستون)
{
    صحیح		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    اگر (صف >= screen_صفیں || ستون >= screen_Columns)
	واپس;

#اگر_تعریف_شدہ FEAT_IMAGE
    mark_dirty_region_for_images(صف, ستون, 1, 1);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    اگر (popup_is_under_opacity(صف, ستون))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	اگر (enc_utf8 && Screen_سطریں_UC[off] != 0
		&& utf_char2cells(Screen_سطریں_UC[off]) == 2
		&& ستون + 1 < screen_Columns
		&& !popup_is_under_opacity(صف, ستون + 1))
	{
	    صحیح off2 = off + 1;
	    Screen_سطریں[off2] = ' ';
	    Screen_سطریں_UC[off2] = 0;
	    screen_char(off2, صف, ستون + 1);
	}
	mark_suppressed_cell(صف, ستون);
	اگر (enc_utf8 && Screen_سطریں_UC[off] != 0
		&& utf_char2cells(Screen_سطریں_UC[off]) == 2
		&& ستون + 1 < screen_Columns
		&& popup_is_under_opacity(صف, ستون + 1))
	    mark_suppressed_cell(صف, ستون + 1);
	screen_cur_ستون = 9999;
	واپس;
    }
    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0
	    && utf_char2cells(Screen_سطریں_UC[off]) == 2
	    && ستون + 1 < screen_Columns
	    && popup_is_under_opacity(صف, ستون + 1))
    {
	mark_suppressed_cell(صف, ستون);
	mark_suppressed_cell(صف, ستون + 1);
	screen_cur_ستون = 9999;
	واپس;
    }
#اگر_ختم

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    اگر (*T_XN == NUL
	    && صف == screen_صفیں - 1 && ستون == screen_Columns - 1
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#اگر_ختم
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ستون_تعداد[off] = -1;
	واپس;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    اگر (screen_char_attr != 0)
	attr = screen_char_attr;
    ورنہ
	attr = ScreenAttrs[off];
    اگر (screen_attr != attr)
	screen_stop_highlight();

    windgoto(صف, ستون);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(صف, ستون);
    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0
	    && utf_char2cells(Screen_سطریں_UC[off]) == 2)
	unmark_suppressed_cell(صف, ستون + 1);
#اگر_ختم

    اگر (screen_attr != attr)
	screen_آغاز_highlight(attr);

    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	اگر (
#اگر_تعریف_شدہ FEAT_GUI
	    !gui.اندر_use &&
#اگر_ختم
	    get_cellwidth(Screen_سطریں_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    برآمد_str((char_u *)"  ");
	    رکن_windgoto(صف, ستون);
	    screen_cur_ستون = 9999;
	}
	ورنہ اگر (utf_ambiguous_چوڑائی(Screen_سطریں_UC[off]))
	{
	    اگر (*p_ambw == 'd'
#اگر_تعریف_شدہ FEAT_GUI
		    && !gui.اندر_use
#اگر_ختم
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		برآمد_str((char_u *)"  ");
		رکن_windgoto(صف, ستون);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_ستون = 9999;
	}
	ورنہ اگر (utf_char2cells(Screen_سطریں_UC[off]) > 1)
	    ++screen_cur_ستون;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	برآمد_str(buf);
    }
    ورنہ
    {
	برآمد_flush_check();
	برآمد_char(Screen_سطریں[off]);
	// double-byte character in single-width cell
	اگر (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
	    برآمد_char(ScreenLines2[off]);
    }

    screen_cur_ستون++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    ساکن خالی
screen_char_2(غیرعلامتی off, صحیح صف, صحیح ستون)
{
    // Check for illegal values (could be wrong when screen was resized).
    اگر (off + 1 >= (غیرعلامتی)(screen_صفیں * screen_Columns))
	واپس;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    اگر (صف == screen_صفیں - 1 && ستون >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ستون_تعداد[off] = -1;
	واپس;
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    اگر (popup_is_under_opacity(صف, ستون))
    {
	mark_suppressed_cell(صف, ستون);
	mark_suppressed_cell(صف, ستون + 1);
	screen_cur_ستون = 9999;
	واپس;
    }
#اگر_ختم

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, صف, ستون);
    برآمد_char(Screen_سطریں[off + 1]);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    unmark_suppressed_cell(صف, ستون + 1);
#اگر_ختم
    ++screen_cur_ستون;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    خالی
screen_draw_rectangle(
    صحیح		صف,
    صحیح		ستون,
    صحیح		اونچائی,
    صحیح		چوڑائی,
    صحیح		invert,
    صحیح		force)
{
    صحیح		r, c;
    صحیح		off;
    صحیح		max_off;

    // Can't use ScreenLines unless initialized
    اگر (Screen_سطریں == تہی)
	واپس;

    اگر (invert)
	screen_char_attr = HL_INVERSE;
    برائے (r = صف; r < صف + اونچائی; ++r)
    {
	off = سطر_انحراف[r];
	max_off = off + screen_Columns;
	برائے (c = ستون; c < ستون + چوڑائی; ++c)
	{
	    اگر (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		اگر (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    ورنہ
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
    ساکن خالی
redraw_بلاک(صحیح صف, صحیح انتہا, win_T *wp)
{
    صحیح		ستون;
    صحیح		چوڑائی;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    clip_may_clear_selection(صف, انتہا - 1);
#اگر_ختم

    اگر (wp == تہی)
    {
	ستون = firstwin->w_wincol;
	چوڑائی = topframe->fr_چوڑائی;
    }
    ورنہ
    {
	ستون = wp->w_wincol;
	چوڑائی = wp->w_چوڑائی;
    }
    screen_draw_rectangle(صف, ستون, انتہا - صف, چوڑائی, FALSE, FALSE);
}

    خالی
space_مقصد_screenline(صحیح off, صحیح attr)
{
    Screen_سطریں[off] = ' ';
    ScreenAttrs[off] = attr;
    Screen_ستون_تعداد[off] = -1;
    اگر (enc_utf8)
	Screen_سطریں_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    خالی
screen_fill(
	صحیح	آغاز_صف,
	صحیح	انتہا_صف,
	صحیح	آغاز_ستون,
	صحیح	انتہا_ستون,
	صحیح	c1,
	صحیح	c2,
	صحیح	attr)
{
    صحیح	    صف;
    صحیح	    ستون;
    صحیح	    off;
    صحیح	    انتہا_off;
    صحیح	    did_delete;
    صحیح	    c;
    صحیح	    norm_رکن;
#اگر defined(FEAT_GUI) || defined(UNIX)
    صحیح	    force_اگلا = FALSE;
#اگر_ختم

    اگر (انتہا_صف > screen_صفیں)		// safety check
	انتہا_صف = screen_صفیں;
    اگر (انتہا_ستون > screen_Columns)	// safety check
	انتہا_ستون = screen_Columns;
    اگر (Screen_سطریں == تہی
	    || آغاز_صف < 0		// should not happen
	    || آغاز_صف >= انتہا_صف
	    || آغاز_ستون >= انتہا_ستون)	// nothing to do
	واپس;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_رکن = (
#اگر_تعریف_شدہ FEAT_GUI
	    !gui.اندر_use &&
#اگر_ختم
	    !IS_CTERM);
    برائے (صف = آغاز_صف; صف < انتہا_صف; ++صف)
    {
	اگر (has_mbyte
#اگر_تعریف_شدہ FEAT_GUI
		&& !gui.اندر_use
#اگر_ختم
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    اگر (آغاز_ستون > 0 && mb_fix_ستون(آغاز_ستون, صف) != آغاز_ستون)
	    {
		// start_col is the right half, clear the left half
		صحیح بایاں_attr = ScreenAttrs[سطر_انحراف[صف] + آغاز_ستون - 1];
		screen_puts_len((char_u *)" ", 1, صف, آغاز_ستون - 1, بایاں_attr);
	    }
	    اگر (انتہا_ستون < screen_Columns && mb_fix_ستون(انتہا_ستون, صف) != انتہا_ستون)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		صحیح دایاں_attr = ScreenAttrs[سطر_انحراف[صف] + انتہا_ستون];
		screen_puts_len((char_u *)" ", 1, صف, انتہا_ستون, دایاں_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	اگر (c2 == ' '
		&& انتہا_ستون == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_رکن
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(صف,
						آغاز_ستون, انتہا_ستون)
#اگر_ختم
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    ستون = آغاز_ستون;
	    اگر (c1 != ' ')			// don't clear first char
		++ستون;

	    off = سطر_انحراف[صف] + ستون;
	    انتہا_off = سطر_انحراف[صف] + انتہا_ستون;

	    // skip blanks (used often, keep it fast!)
	    اگر (enc_utf8)
		جب (off < انتہا_off && Screen_سطریں[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_سطریں_UC[off] == 0
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			  && !is_suppressed_cell(صف,
					      (صحیح)(off - سطر_انحراف[صف]))
#اگر_ختم
			  )
		    ++off;
	    ورنہ
		جب (off < انتہا_off && Screen_سطریں[off] == ' '
						     && ScreenAttrs[off] == 0
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			  && !is_suppressed_cell(صف,
					      (صحیح)(off - سطر_انحراف[صف]))
#اگر_ختم
			  )
		    ++off;
	    اگر (off < انتہا_off)		// something to be cleared
	    {
		ستون = off - سطر_انحراف[صف];
		screen_stop_highlight();
		رکن_windgoto(صف, ستون);// clear rest of this screen line
		برآمد_str(T_CE);
		screen_آغاز();		// don't know where cursor is now
		ستون = انتہا_ستون - ستون;
		جب (ستون--)		// clear chars in ScreenLines
		{
		    space_مقصد_screenline(off, 0);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(صف, (صحیح)(off - سطر_انحراف[صف]));
#اگر_ختم
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = سطر_انحراف[صف] + آغاز_ستون;
	c = c1;
	برائے (ستون = آغاز_ستون; ستون < انتہا_ستون; ++ستون)
	{
	    اگر ((Screen_سطریں[off] != c
		    || (enc_utf8 && (صحیح)Screen_سطریں_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#اگر defined(FEAT_GUI) || defined(UNIX)
		    || force_اگلا
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(صف, ستون)
#اگر_ختم
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(صف, ستون))
	    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		اگر (screen_opacity_popup != تہی && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    اگر (Screen_سطریں[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_سطریں_UC[off - 1] != 0
			    && utf_char2cells(Screen_سطریں_UC[off - 1]) == 2)
			جاؤ اگلا_ستون;
		}
		اگر (screen_opacity_popup != تہی && c == ' '
						       && Screen_سطریں[off] != 0)
		{
		    صحیح bg_char_cells = 1;
		    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_سطریں_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    اگر (bg_char_cells == 2)
		    {
			اگر (ستون + 1 >= انتہا_ستون)
			    // At the edge, skip wide char.
			    جاؤ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			اگر (c2 != ' ')
			    جاؤ skip_opacity_fill;
		    }

		    صحیح popup_attr = get_win_attr(screen_opacity_popup);
		    صحیح blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, صف, ستون);
		    جاؤ اگلا_ستون;
		}
skip_opacity_fill:
#اگر_ختم
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		اگر (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != تہی
			&& صف >= pum_bg_چوٹی && صف < pum_bg_bot
			&& ستون < pum_bg_ستون_تعداد)
		{
		    صحیح soff = (صف - pum_bg_چوٹی) * pum_bg_ستون_تعداد + ستون;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    اگر (enc_utf8 && pum_bg_سطریں_UC != تہی
			    && ستون > آغاز_ستون
			    && pum_bg_سطریں_UC[soff] == 0
			    && pum_bg_سطریں_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_سطریں_UC[soff - 1]) == 2)
		    {
			Screen_سطریں[off] = 0;
			اگر (Screen_سطریں_UC != تہی)
			    Screen_سطریں_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			جاؤ اگلا_ستون;
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
		    اگر (enc_utf8 && pum_bg_سطریں_UC != تہی
			    && ((pum_bg_سطریں_UC[soff] != 0
				    && utf_char2cells(pum_bg_سطریں_UC[soff]) == 2
				    && ستون + 1 >= انتہا_ستون)
				|| (ستون == آغاز_ستون
				    && pum_bg_سطریں_UC[soff] == 0
				    && ستون > 0
				    && pum_bg_سطریں_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_سطریں_UC[soff - 1])
								       == 2)))
		    {
			Screen_سطریں[off] = ' ';
			اگر (Screen_سطریں_UC != تہی)
			    Screen_سطریں_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, صف, ستون);
			جاؤ اگلا_ستون;
		    }

		    // Restore underlying character so text shows through.
		    Screen_سطریں[off] = pum_bg_سطریں[soff];
		    اگر (enc_utf8 && pum_bg_سطریں_UC != تہی
						&& Screen_سطریں_UC != تہی)
		    {
			صحیح k;
			Screen_سطریں_UC[off] = pum_bg_سطریں_UC[soff];
			برائے (k = 0; k < MAX_MCO; ++k)
			    اگر (pum_bg_سطریں_C[k] != تہی
						    && Screen_سطریں_C[k] != تہی)
				Screen_سطریں_C[k][off] = pum_bg_سطریں_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, صف, ستون);
		    جاؤ اگلا_ستون;
		}
#اگر defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		اگر (
# ifdef FEAT_GUI
			gui.اندر_use
# endif
# اگر defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			رکن_is_xterm
# endif
		   )
		{
		    اگر (Screen_سطریں[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_اگلا = TRUE;
		    ورنہ
			force_اگلا = FALSE;
		}
#اگر_ختم // FEAT_GUI || defined(UNIX)
#اگر_تعریف_شدہ FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		اگر (gui.directx_enabled)
		    force_اگلا = TRUE;
#اگر_ختم
		Screen_سطریں[off] = c;
		اگر (enc_utf8)
		{
		    اگر (c >= 0x80)
		    {
			Screen_سطریں_UC[off] = c;
			Screen_سطریں_C[0][off] = 0;
			Screen_سطریں[off] = 0x80; // avoid storing zero
		    }
		    ورنہ
			Screen_سطریں_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		اگر (!did_delete || c != ' ')
		    screen_char(off, صف, ستون);
	    }
اگلا_ستون:
	    Screen_ستون_تعداد[off] = -1;
	    ++off;
	    اگر (ستون == آغاز_ستون)
	    {
		اگر (did_delete)
		    توڑو;
		c = c2;
	    }
	}
	اگر (انتہا_ستون == Columns)
	    سطر_Wraps[صف] = FALSE;
	اگر (صف == صفیں - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    اگر (((آغاز_ستون == 0 && انتہا_ستون == Columns)
			|| (آغاز_ستون == cmdline_ستون_off
			&& انتہا_ستون == cmdline_ستون_off + cmdline_چوڑائی))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#اگر_ختم
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    اگر (آغاز_ستون == 0 || آغاز_ستون == cmdline_ستون_off)
		منوال_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    خالی
check_for_delay(صحیح check_پیغام_scroll)
{
    اگر ((emsg_on_display || (check_پیغام_scroll && پیغام_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !اندر_assert_fails)
    {
	برآمد_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	اگر (check_پیغام_scroll)
	    پیغام_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ساکن خالی
clear_Tab_صفحہ_Idxs(خالی)
{
    صحیح		scol;

    برائے (scol = 0; scol < Columns; ++scol)
	Tab_صفحہ_Idxs[scol] = 0;
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
    واپس (Screen_سطریں != تہی);
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
    خالی
screenalloc(صحیح doclear)
{
    صحیح		    new_صف, old_صف;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		    old_صفیں_2;
#اگر_ختم
    win_T	    *wp;
    صحیح		    outofmem = FALSE;
    صحیح		    len;
    schar_T	    *new_Screen_سطریں;
    u8char_T	    *new_Screen_سطریں_UC = تہی;
    u8char_T	    *new_Screen_سطریں_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = تہی;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_Screen_ستون_تعداد;
    غیرعلامتی	    *new_سطر_انحراف;
    char_u	    *new_سطر_Wraps;
    مختصر	    *new_Tab_صفحہ_Idxs;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    مختصر	    *new_popup_نقاب;
    مختصر	    *new_popup_نقاب_اگلا;
    حرف	    *new_popup_transparent;
#اگر_ختم
    tabpage_T	    *tp;
    ساکن صحیح	    entered = FALSE;		// avoid recursiveness
    ساکن صحیح	    مکمل_outofmem_پیغام = FALSE;	// did outofmem message
    صحیح		    retry_گنتی = 0;
    صحیح		    ملا_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    اگر ((Screen_سطریں != تہی
		&& صفیں == screen_صفیں
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_سطریں_UC != تہی)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != تہی)
		&& p_mco == Screen_mco)
	    || صفیں == 0
	    || Columns == 0
	    || (!بھرا_screen && Screen_سطریں == تہی))
	واپس;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    اگر (entered)
	واپس;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_تالا_screen();  // be safe, put it here
#اگر_ختم

    comp_ستون();		// recompute columns for shown command and ruler

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
    برائے (صحیح i = 0; i < AUCMD_WIN_گنتی; ++i)
	اگر (aucmd_win[i].auc_win != تہی)
	    win_free_lsize(aucmd_win[i].auc_win);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	    win_free_lsize(wp);
#اگر_ختم

    new_Screen_سطریں = LALLOC_MULT(schar_T, (صفیں + 1) * Columns);
    vim_memset(new_Screen_سطریں_C, 0, حجم(u8char_T *) * MAX_MCO);
    اگر (enc_utf8)
    {
	new_Screen_سطریں_UC = LALLOC_MULT(u8char_T, (صفیں + 1) * Columns);
	برائے (صحیح i = 0; i < p_mco; ++i)
	    new_Screen_سطریں_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (صفیں + 1) * Columns);
    }
    اگر (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (صفیں + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (صفیں + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_Screen_ستون_تعداد = LALLOC_CLEAR_MULT(colnr_T, (صفیں + 1) * Columns);
    new_سطر_انحراف = LALLOC_MULT(غیرعلامتی, صفیں);
    new_سطر_Wraps = LALLOC_MULT(char_u, صفیں);
    new_Tab_صفحہ_Idxs = LALLOC_MULT(مختصر, Columns);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    new_popup_نقاب = LALLOC_MULT(مختصر, صفیں * Columns);
    new_popup_نقاب_اگلا = LALLOC_MULT(مختصر, صفیں * Columns);
    new_popup_transparent = LALLOC_MULT(حرف, صفیں * Columns);
#اگر_ختم

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (win_alloc_سطریں(wp) == FAIL)
	{
	    outofmem = TRUE;
	    جاؤ give_اوپر;
	}
    }
    برائے (صحیح i = 0; i < AUCMD_WIN_گنتی; ++i)
	اگر (aucmd_win[i].auc_win != تہی
		&& aucmd_win[i].auc_win->w_سطریں == تہی
		&& win_alloc_سطریں(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    توڑو;
	}
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	اگر (win_alloc_سطریں(wp) == FAIL)
	{
	    outofmem = TRUE;
	    جاؤ give_اوپر;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	    اگر (win_alloc_سطریں(wp) == FAIL)
	    {
		outofmem = TRUE;
		جاؤ give_اوپر;
	    }
#اگر_ختم

give_اوپر:
    ملا_null = FALSE;
    برائے (صحیح i = 0; i < p_mco; ++i)
	اگر (new_Screen_سطریں_C[i] == تہی)
	{
	    ملا_null = TRUE;
	    توڑو;
	}
    اگر (new_Screen_سطریں == تہی
	    || (enc_utf8 && (new_Screen_سطریں_UC == تہی || ملا_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == تہی)
	    || new_ScreenAttrs == تہی
	    || new_Screen_ستون_تعداد == تہی
	    || new_سطر_انحراف == تہی
	    || new_سطر_Wraps == تہی
	    || new_Tab_صفحہ_Idxs == تہی
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    || new_popup_نقاب == تہی
	    || new_popup_نقاب_اگلا == تہی
	    || new_popup_transparent == تہی
#اگر_ختم
	    || outofmem)
    {
	اگر (Screen_سطریں != تہی || !مکمل_outofmem_پیغام)
	{
	    // guess the size
	    do_outofmem_پیغام((long_u)((صفیں + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    مکمل_outofmem_پیغام = TRUE;
	}
	VIM_CLEAR(new_Screen_سطریں);
	VIM_CLEAR(new_Screen_سطریں_UC);
	برائے (صحیح i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_سطریں_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_Screen_ستون_تعداد);
	VIM_CLEAR(new_سطر_انحراف);
	VIM_CLEAR(new_سطر_Wraps);
	VIM_CLEAR(new_Tab_صفحہ_Idxs);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_نقاب);
	VIM_CLEAR(new_popup_نقاب_اگلا);
	VIM_CLEAR(new_popup_transparent);
#اگر_ختم
    }
    ورنہ
    {
	مکمل_outofmem_پیغام = FALSE;

	برائے (new_صف = 0; new_صف < صفیں; ++new_صف)
	{
	    new_سطر_انحراف[new_صف] = new_صف * Columns;
	    new_سطر_Wraps[new_صف] = FALSE;

	    (خالی)vim_memset(new_Screen_سطریں + new_صف * Columns,
				  ' ', (size_t)Columns * حجم(schar_T));
	    اگر (enc_utf8)
	    {
		(خالی)vim_memset(new_Screen_سطریں_UC + new_صف * Columns,
				   0, (size_t)Columns * حجم(u8char_T));
		برائے (صحیح i = 0; i < p_mco; ++i)
		    (خالی)vim_memset(new_Screen_سطریں_C[i]
						      + new_صف * Columns,
				   0, (size_t)Columns * حجم(u8char_T));
	    }
	    اگر (enc_dbcs == DBCS_JPNU)
		(خالی)vim_memset(new_ScreenLines2 + new_صف * Columns,
				   0, (size_t)Columns * حجم(schar_T));
	    (خالی)vim_memset(new_ScreenAttrs + new_صف * Columns,
				    0, (size_t)Columns * حجم(sattr_T));
	    (خالی)vim_memset(new_Screen_ستون_تعداد + new_صف * Columns,
				    0, (size_t)Columns * حجم(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    اگر (!doclear)
	    {
		old_صف = new_صف + (screen_صفیں - صفیں);
		اگر (old_صف >= 0 && Screen_سطریں != تہی)
		{
		    اگر (screen_Columns < Columns)
			len = screen_Columns;
		    ورنہ
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    اگر (!(enc_utf8 && Screen_سطریں_UC == تہی)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_سطریں + new_سطر_انحراف[new_صف],
				Screen_سطریں + سطر_انحراف[old_صف],
				(size_t)len * حجم(schar_T));
		    اگر (enc_utf8 && Screen_سطریں_UC != تہی
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_سطریں_UC + new_سطر_انحراف[new_صف],
				Screen_سطریں_UC + سطر_انحراف[old_صف],
				(size_t)len * حجم(u8char_T));
			برائے (صحیح i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_سطریں_C[i]
						    + new_سطر_انحراف[new_صف],
				Screen_سطریں_C[i] + سطر_انحراف[old_صف],
				(size_t)len * حجم(u8char_T));
		    }
		    اگر (enc_dbcs == DBCS_JPNU && ScreenLines2 != تہی)
			mch_memmove(new_ScreenLines2 + new_سطر_انحراف[new_صف],
				ScreenLines2 + سطر_انحراف[old_صف],
				(size_t)len * حجم(schar_T));
		    mch_memmove(new_ScreenAttrs + new_سطر_انحراف[new_صف],
			    ScreenAttrs + سطر_انحراف[old_صف],
			    (size_t)len * حجم(sattr_T));
		    mch_memmove(new_Screen_ستون_تعداد + new_سطر_انحراف[new_صف],
			    Screen_ستون_تعداد + سطر_انحراف[old_صف],
			    (size_t)len * حجم(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_سطر = new_Screen_سطریں + صفیں * Columns;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	vim_memset(new_popup_نقاب, 0, صفیں * Columns * حجم(مختصر));
	vim_memset(new_popup_transparent, 0, صفیں * Columns * حجم(حرف));
#اگر_ختم
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_سطریں = new_Screen_سطریں;
    Screen_سطریں_UC = new_Screen_سطریں_UC;
    برائے (صحیح i = 0; i < p_mco; ++i)
	Screen_سطریں_C[i] = new_Screen_سطریں_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    Screen_ستون_تعداد = new_Screen_ستون_تعداد;
    سطر_انحراف = new_سطر_انحراف;
    سطر_Wraps = new_سطر_Wraps;
    Tab_صفحہ_Idxs = new_Tab_صفحہ_Idxs;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    popup_نقاب = new_popup_نقاب;
    popup_نقاب_اگلا = new_popup_نقاب_اگلا;
    popup_transparent = new_popup_transparent;
    popup_نقاب_refresh = TRUE;
#اگر_ختم

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#اگر_تعریف_شدہ FEAT_GUI
    old_صفیں_2 = screen_صفیں;
#اگر_ختم
    screen_صفیں = صفیں;
    screen_Columns = Columns;

#اگر_تعریف_شدہ FEAT_GUI
    // Cursor position may now be out of bounds after resize
    اگر (gui.اندر_use && (gui.cursor_صف >= screen_صفیں
		|| gui.cursor_ستون >= screen_Columns))
	gui.cursor_is_valid = false;
#اگر_ختم

    سیٹ_must_redraw(UPD_CLEAR);	// need to clear the screen later
    اگر (doclear)
	screenclear2(TRUE);
#اگر_تعریف_شدہ FEAT_GUI
    ورنہ اگر (gui.اندر_use
	    && !gui.starting
	    && Screen_سطریں != تہی
	    && old_صفیں_2 != صفیں)
    {
	gui_redraw_بلاک(0, 0, (صحیح)صفیں - 1, (صحیح)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	اگر (پیغام_صف >= صفیں)		// Rows got smaller
	    پیغام_صف = صفیں - 1;		// put cursor at last row
	ورنہ اگر (صفیں > old_صفیں_2)	// Rows got bigger
	    پیغام_صف += صفیں - old_صفیں_2; // put cursor in same place
	اگر (پیغام_ستون >= cmdline_چوڑائی)	// cmdline_width got smaller
	    پیغام_ستون = cmdline_چوڑائی - 1;    // put cursor at last cmdline column
    }
#اگر_ختم
    clear_Tab_صفحہ_Idxs();

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_unlock_screen();
#اگر_ختم

    entered = FALSE;
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    اگر (starting == 0 && ++retry_گنتی <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, تہی, تہی, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	جاؤ retry;
    }
}

    خالی
free_screenlines(خالی)
{
    صحیح		i;

    VIM_CLEAR(Screen_سطریں_UC);
    برائے (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_سطریں_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_سطریں);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(Screen_ستون_تعداد);
    VIM_CLEAR(سطر_انحراف);
    VIM_CLEAR(سطر_Wraps);
    VIM_CLEAR(Tab_صفحہ_Idxs);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    VIM_CLEAR(popup_نقاب);
    VIM_CLEAR(popup_نقاب_اگلا);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_صفیں = 0;
    suppressed_ستون_تعداد = 0;
#اگر_ختم
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    صحیح
screenclear(خالی)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    واپس screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    خالی
redraw_as_cleared(خالی)
{
    screenclear2(FALSE);
}

    ساکن صحیح
screenclear2(صحیح doclear)
{
    صحیح	    i;
    صحیح	    did_clear = FALSE;

    اگر (starting == NO_SCREEN || Screen_سطریں == تہی
#اگر_تعریف_شدہ FEAT_GUI
	    || (gui.اندر_use && gui.starting)
#اگر_ختم
	    )
	واپس FALSE;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (!gui.اندر_use)
#اگر_ختم
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#اگر_ختم

    // blank out ScreenLines
    برائے (i = 0; i < صفیں; ++i)
    {
	lineclear(سطر_انحراف[i], (صحیح)Columns, 0);
	سطر_Wraps[i] = FALSE;
    }

    اگر (doclear && can_clear(T_CL))
    {
	برآمد_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	منوال_displayed = FALSE;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	اگر (suppressed_cells != تہی)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_صفیں * suppressed_ستون_تعداد);
#اگر_ختم
    }
    ورنہ
    {
	// can't clear the screen, mark all chars with invalid attributes
	برائے (i = 0; i < صفیں; ++i)
	    lineinvalid(سطر_انحراف[i], (صحیح)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
    اگر (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    پیغام_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#اگر_ختم
    پیغام_صف = cmdline_صف;	// put cursor on last line for messages
    پیغام_ستون = 0;
    screen_آغاز();		// don't know where cursor is now
    پیغام_didany = FALSE;
    پیغام_didout = FALSE;
#اگر_تعریف_شدہ FEAT_IMAGE
    سرکاؤ_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#اگر_ختم

    واپس did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ساکن خالی
lineclear(غیرعلامتی off, صحیح چوڑائی, صحیح attr)
{
    (خالی)vim_memset(Screen_سطریں + off, ' ', (size_t)چوڑائی * حجم(schar_T));
    اگر (enc_utf8)
	(خالی)vim_memset(Screen_سطریں_UC + off, 0,
					  (size_t)چوڑائی * حجم(u8char_T));
    (خالی)vim_memset(ScreenAttrs + off, attr, (size_t)چوڑائی * حجم(sattr_T));
    (خالی)vim_memset(Screen_ستون_تعداد + off, -1, (size_t)چوڑائی * حجم(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ساکن خالی
lineinvalid(غیرعلامتی off, صحیح چوڑائی)
{
    (خالی)vim_memset(ScreenAttrs + off, -1, (size_t)چوڑائی * حجم(sattr_T));
    (خالی)vim_memset(Screen_ستون_تعداد + off, -1, (size_t)چوڑائی * حجم(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    خالی
سطر_was_clobbered(صحیح screen_lnum)
{
    lineinvalid(سطر_انحراف[screen_lnum], (صحیح)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ساکن خالی
linecopy(صحیح مقصد, صحیح from, win_T *wp)
{
    غیرعلامتی	off_مقصد = سطر_انحراف[مقصد] + wp->w_wincol;
    غیرعلامتی	off_from = سطر_انحراف[from] + wp->w_wincol;

    mch_memmove(Screen_سطریں + off_مقصد, Screen_سطریں + off_from,
	    wp->w_چوڑائی * حجم(schar_T));
    اگر (enc_utf8)
    {
	صحیح	i;

	mch_memmove(Screen_سطریں_UC + off_مقصد, Screen_سطریں_UC + off_from,
		wp->w_چوڑائی * حجم(u8char_T));
	برائے (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_سطریں_C[i] + off_مقصد, Screen_سطریں_C[i] + off_from,
		    wp->w_چوڑائی * حجم(u8char_T));
    }
    اگر (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_مقصد, ScreenLines2 + off_from,
		wp->w_چوڑائی * حجم(schar_T));
    mch_memmove(ScreenAttrs + off_مقصد, ScreenAttrs + off_from,
	    wp->w_چوڑائی * حجم(sattr_T));
    mch_memmove(Screen_ستون_تعداد + off_مقصد, Screen_ستون_تعداد + off_from,
	    wp->w_چوڑائی * حجم(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    صحیح
can_clear(char_u *p)
{
    واپس (*p != NUL && (t_colors <= 1
#اگر_تعریف_شدہ FEAT_GUI
		|| gui.اندر_use
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_معمول_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_معمول_bg_color == 0)
#ورنہ
		|| cterm_معمول_bg_color == 0
#اگر_ختم
		|| *T_UT != NUL)
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#اگر_ختم
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    خالی
screen_آغاز(خالی)
{
    screen_cur_صف = screen_cur_ستون = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    خالی
windgoto(صحیح صف, صحیح ستون)
{
    sattr_T	    *p;
    صحیح		    i;
    صحیح		    plan;
    صحیح		    لاگت;
    صحیح		    wouldbe_ستون;
    صحیح		    noinvcurs;
    char_u	    *bs;
    صحیح		    goto_لاگت;
    صحیح		    attr;

#تعریف_کرو GOTO_لاگت   7	// assume a term_windgoto() takes about 7 chars
#تعریف_کرو HIGHL_لاگت  5	// assume unhighlight takes 5 chars

#تعریف_کرو PLAN_LE	    1
#تعریف_کرو PLAN_CR	    2
#تعریف_کرو PLAN_NL	    3
#تعریف_کرو PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    اگر (Screen_سطریں == تہی)
	واپس;
    اگر (ستون == screen_cur_ستون && صف == screen_cur_صف)
	واپس;

    // Check for valid position.
    اگر (صف < 0)	// window without text lines?
	صف = 0;
    اگر (صف >= screen_صفیں)
	صف = screen_صفیں - 1;
    اگر (ستون >= screen_Columns)
	ستون = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    اگر (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_لاگت;
    ورنہ
	noinvcurs = 0;
    goto_لاگت = GOTO_لاگت + noinvcurs;

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
    اگر (صف >= screen_cur_صف && screen_cur_ستون < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = تہی;			    // init for GCC
	attr = screen_attr;
	اگر (صف == screen_cur_صف && ستون < screen_cur_ستون)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    اگر (*T_LE)
		bs = T_LE;		    // "cursor left"
	    ورنہ
		bs = T_BC;		    // "backspace character (old)
	    اگر (*bs)
		لاگت = (screen_cur_ستون - ستون) * (صحیح)STRLEN(bs);
	    ورنہ
		لاگت = 999;
	    اگر (ستون + 1 < لاگت)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_ستون = 0;
		لاگت = 1;		    // CR is just one character
	    }
	    ورنہ
	    {
		plan = PLAN_LE;
		wouldbe_ستون = ستون;
	    }
	    اگر (noinvcurs)		    // will stop highlighting
	    {
		لاگت += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	ورنہ اگر (صف > screen_cur_صف)
	{
	    plan = PLAN_NL;
	    wouldbe_ستون = 0;
	    لاگت = (صف - screen_cur_صف) * 2;  // CR LF
	    اگر (noinvcurs)		    // will stop highlighting
	    {
		لاگت += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	ورنہ
	{
	    plan = PLAN_WRITE;
	    wouldbe_ستون = screen_cur_ستون;
	    لاگت = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = ستون - wouldbe_ستون;
	اگر (i > 0)
	    لاگت += i;
	اگر (لاگت < goto_لاگت && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + سطر_انحراف[صف] + wouldbe_ستون;
	    جب (i && *p++ == attr)
		--i;
	    اگر (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		اگر (*--p == 0)
		{
		    لاگت += noinvcurs;
		    جب (i && *p++ == 0)
			--i;
		}
		اگر (i != 0)
		    لاگت = 999;	// different attributes, don't do it
	    }
	    اگر (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		برائے (i = wouldbe_ستون; i < ستون; ++i)
		    اگر (Screen_سطریں_UC[سطر_انحراف[صف] + i] != 0)
		    {
			لاگت = 999;
			توڑو;
		    }
	    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    اگر (لاگت < 999)
		برائے (i = wouldbe_ستون; i < ستون; ++i)
		    اگر (popup_is_under_opacity(صف, i))
		    {
			لاگت = 999;
			توڑو;
		    }
#اگر_ختم
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	اگر (لاگت < goto_لاگت)
	{
	    اگر (plan == PLAN_LE)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		جب (screen_cur_ستون > ستون)
		{
		    برآمد_str(bs);
		    --screen_cur_ستون;
		}
	    }
	    ورنہ اگر (plan == PLAN_CR)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		برآمد_char('\r');
		screen_cur_ستون = 0;
	    }
	    ورنہ اگر (plan == PLAN_NL)
	    {
		اگر (noinvcurs)
		    screen_stop_highlight();
		جب (screen_cur_صف < صف)
		{
		    برآمد_char('\n');
		    ++screen_cur_صف;
		}
		screen_cur_ستون = 0;
	    }

	    i = ستون - screen_cur_ستون;
	    اگر (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		اگر (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    جب (i-- > 0)
			برآمد_char(*T_ND);
		}
		ورنہ
		{
		    صحیح	off;

		    off = سطر_انحراف[صف] + screen_cur_ستون;
		    جب (i-- > 0)
		    {
			اگر (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			برآمد_flush_check();
			برآمد_char(Screen_سطریں[off]);
			اگر (enc_dbcs == DBCS_JPNU
				&& Screen_سطریں[off] == 0x8e)
			    برآمد_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    ورنہ
	لاگت = 999;

    اگر (لاگت >= goto_لاگت)
    {
	اگر (noinvcurs)
	    screen_stop_highlight();
	اگر (صف == screen_cur_صف && (ستون > screen_cur_ستون)
		&& *T_CRI != NUL)
	    رکن_cursor_دایاں(ستون - screen_cur_ستون);
	ورنہ
	    رکن_windgoto(صف, ستون);
    }
    screen_cur_صف = صف;
    screen_cur_ستون = ستون;
}

/*
 * Set cursor to its position in the current window.
 */
    خالی
setcursor(خالی)
{
    setcursor_mayforce(FALSE);
}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    صحیح
cursor_screen_cells(خالی)
{
    واپس has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#اگر_ختم

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    خالی
setcursor_mayforce(صحیح force)
{
    اگر (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((صحیح)curwin->w_چوڑائی - curwin->w_wcol
					    - cursor_screen_cells()) :
#اگر_ختم
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
win_ins_سطریں(
    win_T	*wp,
    صحیح		صف,
    صحیح		سطر_گنتی,
    صحیح		invalid,
    صحیح		mayclear)
{
    صحیح		did_delete;
    صحیح		nextrow;
    صحیح		lastrow;
    صحیح		retval;

    اگر (invalid)
	wp->w_سطریں_valid = 0;

    // with only a few lines it's not worth the effort
    اگر (wp->w_اونچائی < 5)
	واپس FAIL;

    // with the popup menu visible this might not work correctly
    اگر (pum_visible())
	واپس FAIL;

    اگر (سطر_گنتی > wp->w_اونچائی - صف)
	سطر_گنتی = wp->w_اونچائی - صف;

    retval = win_do_سطریں(wp, صف, سطر_گنتی, mayclear, FALSE, 0);
    اگر (retval != MAYBE)
	واپس retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    اگر (wp->w_اگلا != تہی || wp->w_حالت_اونچائی)
    {
	اگر (screen_del_سطریں(0, W_WINROW(wp) + wp->w_اونچائی - سطر_گنتی,
				  سطر_گنتی, (صحیح)صفیں, FALSE, 0, تہی) == OK)
	    did_delete = TRUE;
	ورنہ اگر (wp->w_اگلا)
	    واپس FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    اگر (!did_delete)
    {
	wp->w_redr_حالت = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_اونچائی + wp->w_حالت_اونچائی;
	lastrow = nextrow + سطر_گنتی;
	اگر (lastrow > صفیں)
	    lastrow = صفیں;
	screen_fill(nextrow - سطر_گنتی, lastrow - سطر_گنتی,
		  wp->w_wincol, (صحیح)W_ENDCOL(wp), ' ', ' ', 0);
    }

    اگر (screen_ins_سطریں(0, W_WINROW(wp) + صف, سطر_گنتی, (صحیح)صفیں, 0, تہی)
								      == FAIL)
    {
	// deletion will have messed up other windows
	اگر (did_delete)
	{
	    wp->w_redr_حالت = true;
	    win_rest_invalid(W_اگلا(wp));
	}
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    صحیح
win_del_سطریں(
    win_T	*wp,
    صحیح		صف,
    صحیح		سطر_گنتی,
    صحیح		invalid,
    صحیح		mayclear,
    صحیح		clear_attr)	    // for clearing lines
{
    صحیح		retval;

    اگر (invalid)
	wp->w_سطریں_valid = 0;

    اگر (سطر_گنتی > wp->w_اونچائی - صف)
	سطر_گنتی = wp->w_اونچائی - صف;

    retval = win_do_سطریں(wp, صف, سطر_گنتی, mayclear, TRUE, clear_attr);
    اگر (retval != MAYBE)
	واپس retval;

    اگر (screen_del_سطریں(0, W_WINROW(wp) + صف, سطر_گنتی,
				   (صحیح)صفیں, FALSE, clear_attr, تہی) == FAIL)
	واپس FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    اگر (wp->w_اگلا || wp->w_حالت_اونچائی || cmdline_صف < صفیں - 1)
    {
	اگر (screen_ins_سطریں(0, W_WINROW(wp) + wp->w_اونچائی - سطر_گنتی,
			      سطر_گنتی, (صحیح)صفیں, clear_attr, تہی) == FAIL)
	{
	    wp->w_redr_حالت = true;
	    win_rest_invalid(wp->w_اگلا);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    ورنہ
	redraw_cmdline = TRUE;
    واپس OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ساکن صحیح
win_do_سطریں(
    win_T	*wp,
    صحیح		صف,
    صحیح		سطر_گنتی,
    صحیح		mayclear,
    صحیح		del,
    صحیح		clear_attr)
{
    صحیح		retval;

    اگر (!redrawing() || سطر_گنتی <= 0)
	واپس FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    اگر (no_win_do_سطریں_ins && !del)
	واپس FAIL;

    // only a few lines left: redraw is faster
    اگر (mayclear && صفیں - سطر_گنتی < 5
	    && wp->w_چوڑائی == topframe->fr_چوڑائی)
    {
	اگر (!no_win_do_سطریں_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    اگر (popup_visible)
	واپس FAIL;
#اگر_ختم

    // Delete all remaining lines
    اگر (صف + سطر_گنتی >= wp->w_اونچائی)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + wp->w_اونچائی,
		wp->w_wincol, (صحیح)W_ENDCOL(wp), ' ', ' ', 0);
	واپس OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    اگر (!no_win_do_سطریں_ins)
	clear_cmdline = TRUE;

#اگر defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    اگر (tabpanel_چوڑائی() > 0)
	واپس FAIL;
#اگر_ختم

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    اگر (scroll_region || wp->w_چوڑائی != topframe->fr_چوڑائی)
    {
	اگر (scroll_region && (wp->w_چوڑائی == topframe->fr_چوڑائی
		    || *T_CSV != NUL))
	    scroll_region_سیٹ(wp, صف);
	اگر (del)
	    retval = screen_del_سطریں(W_WINROW(wp) + صف, 0, سطر_گنتی,
				    wp->w_اونچائی - صف, FALSE, clear_attr, wp);
	ورنہ
	    retval = screen_ins_سطریں(W_WINROW(wp) + صف, 0, سطر_گنتی,
					   wp->w_اونچائی - صف, clear_attr, wp);
	اگر (scroll_region && (wp->w_چوڑائی == topframe->fr_چوڑائی
		    || *T_CSV != NUL))
	    scroll_region_reset();
	واپس retval;
    }

    اگر (wp->w_اگلا != تہی && p_tf) // don't delete/insert on fast terminal
	واپس FAIL;

    واپس MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ساکن خالی
win_rest_invalid(win_T *wp)
{
    جب (wp != تہی)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_حالت = true;
	wp = wp->w_اگلا;
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
#تعریف_کرو USE_T_CAL   1
#تعریف_کرو USE_T_CDL   2
#تعریف_کرو USE_T_AL    3
#تعریف_کرو USE_T_CE    4
#تعریف_کرو USE_T_DL    5
#تعریف_کرو USE_T_SR    6
#تعریف_کرو USE_NL	    7
#تعریف_کرو USE_T_CD    8
#تعریف_کرو USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    صحیح
screen_ins_سطریں(
    صحیح		off,
    صحیح		صف,
    صحیح		سطر_گنتی,
    صحیح		انتہا,
    صحیح		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    صحیح		i;
    صحیح		j;
    غیرعلامتی	عارضی;
    صحیح		cursor_صف;
    صحیح		cursor_ستون = 0;
    صحیح		type;
    صحیح		نتیجہ_خالی_خانے;
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
	     || سطر_گنتی <= 0 || سطر_گنتی > p_ttyscroll
	     || انتہا > صفیں
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	     || (clip_ستارہ.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#اگر_ختم
	     )
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	سرکاؤ_image_placements(off + صف, off + انتہا, سطر_گنتی);
# endif
	واپس FAIL;
    }
#اگر_ختم

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
    نتیجہ_خالی_خانے = (صف + سطر_گنتی >= انتہا);
    اگر (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	اگر (سطر_گنتی > 3)
	    واپس FAIL;
	type = USE_REDRAW;
    }
    ورنہ اگر (can_clear(T_CD) && نتیجہ_خالی_خانے)
	type = USE_T_CD;
    ورنہ اگر (*T_CAL != NUL && (سطر_گنتی > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    ورنہ اگر (*T_CDL != NUL && نتیجہ_خالی_خانے && (سطر_گنتی > 1 || !can_ce))
	type = USE_T_CDL;
    ورنہ اگر (*T_AL != NUL)
	type = USE_T_AL;
    ورنہ اگر (can_ce && نتیجہ_خالی_خانے)
	type = USE_T_CE;
    ورنہ اگر (*T_DL != NUL && نتیجہ_خالی_خانے)
	type = USE_T_DL;
    ورنہ اگر (*T_SR != NUL && صف == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    ورنہ
	واپس FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    اگر (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	واپس screen_del_سطریں(off, صف, سطر_گنتی, انتہا, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    اگر (*T_DB)
	screen_del_سطریں(off, انتہا - سطر_گنتی, سطر_گنتی, انتہا, FALSE, 0, wp);

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    اگر (off + صف > 0 || (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی))
	clip_clear_selection(&clip_ستارہ);
    ورنہ
	clip_scroll_selection(-سطر_گنتی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
    سرکاؤ_image_placements(off + صف, off + انتہا, سطر_گنتی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_تالا_screen();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(صف + off <= gui.cursor_صف);
#اگر_ختم

    اگر (wp != تہی && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ستون = wp->w_wincol;

    اگر (*T_CCS != NUL)	   // cursor relative to region
	cursor_صف = صف;
    ورنہ
	cursor_صف = صف + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    صف += off;
    انتہا += off;
    برائے (i = 0; i < سطر_گنتی; ++i)
    {
	اگر (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی)
	{
	    // need to copy part of a line
	    j = انتہا - 1 - i;
	    جب ((j -= سطر_گنتی) >= صف)
		linecopy(j + سطر_گنتی, j, wp);
	    j += سطر_گنتی;
	    اگر (can_clear((char_u *)" "))
		lineclear(سطر_انحراف[j] + wp->w_wincol,
			wp->w_چوڑائی, clear_attr);
	    ورنہ
		lineinvalid(سطر_انحراف[j] + wp->w_wincol, wp->w_چوڑائی);
	    سطر_Wraps[j] = FALSE;
	}
	ورنہ
	{
	    j = انتہا - 1 - i;
	    عارضی = سطر_انحراف[j];
	    جب ((j -= سطر_گنتی) >= صف)
	    {
		سطر_انحراف[j + سطر_گنتی] = سطر_انحراف[j];
		سطر_Wraps[j + سطر_گنتی] = سطر_Wraps[j];
	    }
	    سطر_انحراف[j + سطر_گنتی] = عارضی;
	    سطر_Wraps[j + سطر_گنتی] = FALSE;
	    اگر (can_clear((char_u *)" "))
		lineclear(عارضی, (صحیح)Columns, clear_attr);
	    ورنہ
		lineinvalid(عارضی, (صحیح)Columns);
	}
    }

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_unlock_screen();
#اگر_ختم

    screen_stop_highlight();
    windgoto(cursor_صف, cursor_ستون);
    اگر (clear_attr != 0)
	screen_آغاز_highlight(clear_attr);

    // redraw the characters
    اگر (type == USE_REDRAW)
	redraw_بلاک(صف, انتہا, wp);
    ورنہ اگر (type == USE_T_CAL)
    {
	رکن_append_سطریں(سطر_گنتی);
	screen_آغاز();		// don't know where cursor is now
    }
    ورنہ
    {
	برائے (i = 0; i < سطر_گنتی; i++)
	{
	    اگر (type == USE_T_AL)
	    {
		اگر (i && cursor_صف != 0)
		    windgoto(cursor_صف, cursor_ستون);
		برآمد_str(T_AL);
	    }
	    ورنہ  // type == USE_T_SR
		برآمد_str(T_SR);
	    screen_آغاز();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    اگر (type == USE_T_SR && *T_DA)
    {
	برائے (i = 0; i < سطر_گنتی; ++i)
	{
	    windgoto(off + i, cursor_ستون);
	    برآمد_str(T_CE);
	    screen_آغاز();	    // don't know where cursor is now
	}
    }

#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    gui_can_update_cursor();
    اگر (gui.اندر_use)
	برآمد_flush();	// always flush after a scroll
#اگر_ختم
    واپس OK;
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
screen_del_سطریں(
    صحیح		off,
    صحیح		صف,
    صحیح		سطر_گنتی,
    صحیح		انتہا,
    صحیح		force,		// even when line_count > p_ttyscroll
    صحیح		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    صحیح		j;
    صحیح		i;
    غیرعلامتی	عارضی;
    صحیح		cursor_صف;
    صحیح		cursor_ستون = 0;
    صحیح		cursor_انتہا;
    صحیح		نتیجہ_خالی_خانے;	// result is empty until end of region
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
	    || سطر_گنتی <= 0
	    || (!force && سطر_گنتی > p_ttyscroll)
	    || انتہا > صفیں
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	    || (clip_ستارہ.state != SELECT_CLEARED && redrawing_for_callback > 0)
#اگر_ختم
       )
	واپس FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    نتیجہ_خالی_خانے = صف + سطر_گنتی >= انتہا;

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
    اگر (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	اگر (سطر_گنتی > 3)
	    واپس FAIL;
	type = USE_REDRAW;
    }
    ورنہ اگر (can_clear(T_CD) && نتیجہ_خالی_خانے)
	type = USE_T_CD;
    ورنہ اگر (صف == 0 && (
#اگر_غیر_تعریف_شدہ AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    سطر_گنتی == 1 ||
#اگر_ختم
						*T_CDL == NUL))
	type = USE_NL;
    ورنہ اگر (*T_CDL != NUL && سطر_گنتی > 1 && can_delete)
	type = USE_T_CDL;
    ورنہ اگر (can_clear(T_CE) && نتیجہ_خالی_خانے
	    && (wp == تہی || wp->w_چوڑائی == topframe->fr_چوڑائی))
	type = USE_T_CE;
    ورنہ اگر (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    ورنہ اگر (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    ورنہ
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    اگر (off + صف > 0 || (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی))
	clip_clear_selection(&clip_ستارہ);
    ورنہ
	clip_scroll_selection(سطر_گنتی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
    سرکاؤ_image_placements(off + صف, off + انتہا, -سطر_گنتی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_تالا_screen();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_صف >= صف + off
						&& gui.cursor_صف < انتہا + off);
#اگر_ختم

    اگر (wp != تہی && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ستون = wp->w_wincol;

    اگر (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_صف = صف;
	cursor_انتہا = انتہا;
    }
    ورنہ
    {
	cursor_صف = صف + off;
	cursor_انتہا = انتہا + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    صف += off;
    انتہا += off;
    برائے (i = 0; i < سطر_گنتی; ++i)
    {
	اگر (wp != تہی && wp->w_چوڑائی != topframe->fr_چوڑائی)
	{
	    // need to copy part of a line
	    j = صف + i;
	    جب ((j += سطر_گنتی) <= انتہا - 1)
		linecopy(j - سطر_گنتی, j, wp);
	    j -= سطر_گنتی;
	    اگر (can_clear((char_u *)" "))
		lineclear(سطر_انحراف[j] + wp->w_wincol,
			wp->w_چوڑائی, clear_attr);
	    ورنہ
		lineinvalid(سطر_انحراف[j] + wp->w_wincol, wp->w_چوڑائی);
	    سطر_Wraps[j] = FALSE;
	}
	ورنہ
	{
	    // whole width, moving the line pointers is faster
	    j = صف + i;
	    عارضی = سطر_انحراف[j];
	    جب ((j += سطر_گنتی) <= انتہا - 1)
	    {
		سطر_انحراف[j - سطر_گنتی] = سطر_انحراف[j];
		سطر_Wraps[j - سطر_گنتی] = سطر_Wraps[j];
	    }
	    سطر_انحراف[j - سطر_گنتی] = عارضی;
	    سطر_Wraps[j - سطر_گنتی] = FALSE;
	    اگر (can_clear((char_u *)" "))
		lineclear(عارضی, (صحیح)Columns, clear_attr);
	    ورنہ
		lineinvalid(عارضی, (صحیح)Columns);
	}
    }

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_unlock_screen();
#اگر_ختم

    اگر (screen_attr != clear_attr)
	screen_stop_highlight();
    اگر (clear_attr != 0)
	screen_آغاز_highlight(clear_attr);

    // redraw the characters
    اگر (type == USE_REDRAW)
	redraw_بلاک(صف, انتہا, wp);
    ورنہ اگر (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_صف, cursor_ستون);
	برآمد_str(T_CD);
	screen_آغاز();			// don't know where cursor is now
    }
    ورنہ اگر (type == USE_T_CDL)
    {
	windgoto(cursor_صف, cursor_ستون);
	رکن_delete_سطریں(سطر_گنتی);
	screen_آغاز();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    ورنہ اگر (type == USE_NL)
    {
	windgoto(cursor_انتہا - 1, cursor_ستون);
	برائے (i = سطر_گنتی; --i >= 0; )
	    برآمد_char('\n');		// cursor will remain on same line
    }
    ورنہ
    {
	برائے (i = سطر_گنتی; --i >= 0; )
	{
	    اگر (type == USE_T_DL)
	    {
		windgoto(cursor_صف, cursor_ستون);
		برآمد_str(T_DL);		// delete a line
	    }
	    ورنہ // type == USE_T_CE
	    {
		windgoto(cursor_صف + i, cursor_ستون);
		برآمد_str(T_CE);		// erase a line
	    }
	    screen_آغاز();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    اگر (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	برائے (i = سطر_گنتی; i > 0; --i)
	{
	    windgoto(cursor_انتہا - i, cursor_ستون);
	    برآمد_str(T_CE);		// erase a line
	    screen_آغاز();		// don't know where cursor is now
	}
    }

#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    gui_can_update_cursor();
    اگر (gui.اندر_use)
	برآمد_flush();	// always flush after a scroll
#اگر_ختم

    واپس OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    صحیح
skip_showmode(خالی)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    اگر (global_busy
	    || پیغام_silent != 0
	    || !redrawing()
	    || ((!stuff_خالی_خانے() || typebuf.tb_len > 0 || using_script())
		&& !کلید_Typed))
    {
	redraw_منوال = TRUE;		// show mode later
	واپس TRUE;
    }
    واپس FALSE;
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
showmode(خالی)
{
    صحیح		need_clear;
    صحیح		لمبائی = 0;
    صحیح		do_منوال;
    صحیح		attr;
    صحیح		nwr_save;
    صحیح		sub_attr;
    صحیح		show_ruler_with_pum = FALSE;

    do_منوال = p_smd && پیغام_silent == 0
	    && ((State & منوال_داخل_کرو)
		|| restart_edit != NUL
		|| VIsual_فعال);
    اگر (do_منوال || reg_recording != 0)
    {
	اگر (skip_showmode())
	    واپس 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	اگر (clear_cmdline && cmdline_صف < صفیں - 1)
	    پیغام_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	پیغام_pos_منوال();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	اگر (do_منوال)
	{
	    پیغام_puts_attr("--", attr);
#اگر defined(FEAT_XIM)
	    اگر (
# ifdef FEAT_GUI_GTK
		    preedit_get_حالت()
# ورنہ
		    im_get_حالت()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		پیغام_puts_attr(" IM", attr);
# ورنہ
		پیغام_puts_attr(" XIM", attr);
# endif
#اگر_ختم
	    // CTRL-X in Insert mode
	    اگر (edit_submode != تہی && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		لمبائی = (صفیں - پیغام_صف) * cmdline_چوڑائی - 3;
		اگر (edit_submode_extra != تہی)
		    لمبائی -= vim_strsize(edit_submode_extra);
		اگر (لمبائی > 0)
		{
		    اگر (edit_submode_pre != تہی)
			لمبائی -= vim_strsize(edit_submode_pre);
		    اگر (لمبائی - vim_strsize(edit_submode) > 0)
		    {
			اگر (edit_submode_pre != تہی)
			    پیغام_puts_attr((حرف *)edit_submode_pre, attr);
			پیغام_puts_attr((حرف *)edit_submode, attr);
		    }
		    اگر (edit_submode_extra != تہی)
		    {
			پیغام_puts_attr(" ", attr);  // add a space in between
			اگر ((صحیح)edit_submode_highl < (صحیح)HLF_گنتی)
			    sub_attr = HL_ATTR(edit_submode_highl);
			ورنہ
			    sub_attr = attr;
			پیغام_puts_attr((حرف *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    ورنہ
	    {
		اگر (State & VREPLACE_FLAG)
		    پیغام_puts_attr(_(" VREPLACE"), attr);
		ورنہ اگر (State & REPLACE_FLAG)
		    پیغام_puts_attr(_(" REPLACE"), attr);
		ورنہ اگر (State & منوال_داخل_کرو)
		{
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    اگر (p_ri)
			پیغام_puts_attr(_(" REVERSE"), attr);
#اگر_ختم
		    پیغام_puts_attr(_(" INSERT"), attr);
		}
		ورنہ اگر (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    پیغام_puts_attr(_(" (insert)"), attr);
		ورنہ اگر (restart_edit == 'R')
		    پیغام_puts_attr(_(" (replace)"), attr);
		ورنہ اگر (restart_edit == 'V')
		    پیغام_puts_attr(_(" (vreplace)"), attr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (p_hkmap)
		    پیغام_puts_attr(_(" Hebrew"), attr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_KEYMAP
		اگر (State & منوال_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    اگر (curwin->w_p_arab)
			پیغام_puts_attr(_(" Arabic"), attr);
		    ورنہ
# endif
			اگر (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    پیغام_puts_attr((حرف *)NameBuff, attr);
		}
#اگر_ختم
		اگر ((State & منوال_داخل_کرو) && p_paste)
		    پیغام_puts_attr(_(" (paste)"), attr);

		اگر (VIsual_فعال)
		{
		    حرف *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    انتخاب ((VIsual_select ? 4 : 0)
			    + (VIsual_منوال == Ctrl_V) * 2
			    + (VIsual_منوال == 'V'))
		    {
			حالت 0:	p = N_(" VISUAL"); توڑو;
			حالت 1: p = N_(" VISUAL LINE"); توڑو;
			حالت 2: p = N_(" VISUAL BLOCK"); توڑو;
			حالت 4: p = N_(" SELECT"); توڑو;
			حالت 5: p = N_(" SELECT LINE"); توڑو;
			طےشدہ: p = N_(" SELECT BLOCK"); توڑو;
		    }
		    پیغام_puts_attr(_(p), attr);
		}
		پیغام_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	اگر (reg_recording != 0
		&& edit_submode == تہی)    // otherwise it gets too long
	{
	    recording_منوال(attr);
	    need_clear = TRUE;
	}

	منوال_displayed = TRUE;
	اگر (need_clear || clear_cmdline || redraw_منوال)
	    پیغام_clr_eos();
	پیغام_didout = FALSE;		// overwrite this message
	لمبائی = پیغام_ستون;
	پیغام_ستون = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    ورنہ اگر (clear_cmdline && پیغام_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	پیغام_clr_cmdline();
    ورنہ اگر (redraw_منوال)
    {
	پیغام_pos_منوال();
	پیغام_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    اگر (VIsual_فعال)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    اگر (redrawing() && lastwin->w_حالت_اونچائی == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_منوال = FALSE;
    clear_cmdline = FALSE;

    واپس لمبائی;
}

/*
 * Position for a mode message.
 */
    ساکن خالی
پیغام_pos_منوال(خالی)
{
    پیغام_ستون = 0;
    پیغام_صف = صفیں - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    خالی
unshowmode(صحیح force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    اگر (!redrawing() || (!force && char_avail() && !کلید_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    ورنہ
	clearmode();
}

/*
 * Clear the mode message.
 */
    خالی
clearmode(خالی)
{
    صحیح save_پیغام_صف = پیغام_صف;
    صحیح save_پیغام_ستون = پیغام_ستون;

    پیغام_pos_منوال();
    اگر (reg_recording != 0)
	recording_منوال(HL_ATTR(HLF_CM));
    پیغام_clr_eos();

    پیغام_ستون = save_پیغام_ستون;
    پیغام_صف = save_پیغام_صف;
}

    ساکن خالی
recording_منوال(صحیح attr)
{
    پیغام_puts_attr(_("recording"), attr);
    اگر (shortmess(SHM_RECORDING))
	واپس;

    حرف s[4];

    sprintf(s, " @%c", reg_recording);
    پیغام_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ساکن صحیح
mouse_has_any(خالی)
{
    واپس mouse_has(MOUSE_معمول) || mouse_has(MOUSE_داخل_کرو)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    خالی
draw_tabline(خالی)
{
    صحیح		tabcount = 0;
    tabpage_T	*tp;
    صحیح		tabwidth;
    صحیح		ستون = 0;
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
    صحیح		گنجائش;
    صحیح		use_sep_chars = (t_colors < 8
#اگر_تعریف_شدہ FEAT_GUI
					    && !gui.اندر_use
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
					    && !p_tgc
#اگر_ختم
					    );
    bool	override_success;

#اگر defined(FEAT_TABPANEL)
    ستون = firstwin->w_wincol;
#اگر_ختم

    اگر (Screen_سطریں == تہی)
	واپس;
    redraw_tabline = FALSE;

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    اگر (gui_use_tabline())
    {
	gui_update_tabline();
	واپس;
    }
#اگر_ختم

    اگر (tabline_اونچائی() < 1)
	واپس;

#اگر defined(FEAT_STL_OPT)
    clear_Tab_صفحہ_Idxs();

    // Use the 'tabline' option if it's set.
    اگر (*p_tal != NUL)
	win_redr_custom(تہی, FALSE);
    ورنہ
#اگر_ختم
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_چوڑائی - 1 + tabcount / 2) / tabcount;
	اگر (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	برائے (tp = پہلا_tabpage; tp != تہی && ستون < Columns - 4;
							     tp = tp->tp_اگلا)
	{
	    scol = ستون;

	    override_success = ڈالو_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    اگر (tp->tp_topframe == topframe)
		attr = attr_sel;
	    اگر (use_sep_chars && ستون > 0)
		screen_putchar('|', 0, ستون++, attr);

	    اگر (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, ستون++, attr);

	    اگر (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    ورنہ
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    برائے (wincount = 0; wp != تہی; wp = wp->w_اگلا, ++wincount)
		اگر (buf_Is_بدلا(wp->w_بفر))
		    modified = TRUE;
	    اگر (modified || wincount > 1)
	    {
		اگر (wincount > 1)
		{
		    len = vim_snprintf((حرف *)NameBuff, MAXPATHL, "%d", wincount);
		    اگر (ستون + len >= Columns - 3)
			توڑو;
		    screen_puts_len(NameBuff, len, 0, ستون,
#اگر defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#ورنہ
					 attr
#اگر_ختم
					       );
		    ستون += len;
		}
		اگر (modified)
		    screen_puts_len((char_u *)"+", 1, 0, ستون++, attr);
		screen_putchar(' ', 0, ستون++, attr);
	    }

	    گنجائش = scol - ستون + tabwidth - 1;
	    اگر (گنجائش > 0)
	    {
		صحیح n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_بفر);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		اگر (has_mbyte)
		    جب (len > گنجائش)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		ورنہ اگر (len > گنجائش)
		{
		    p += len - گنجائش;
		    len = گنجائش;
		}
		n = Columns - ستون - 1;
		اگر (len > n)
		    len = n;

		screen_puts_len(p, (صحیح)STRLEN(p), 0, ستون, attr);
		ستون += len;
	    }
	    screen_putchar(' ', 0, ستون++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    جب (scol < ستون)
		Tab_صفحہ_Idxs[scol++] = tabcount;

	    اگر (override_success)
		نکالو_highlight_overrides();
	}

	اگر (use_sep_chars)
	    c = '_';
	ورنہ
	    c = ' ';
	screen_fill(0, 1, ستون, (صحیح)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	اگر (p_sc && *p_sloc == 't')
	{
	    صحیح	n = (صحیح)Columns - ستون - (tabcount > 1) * 3;
	    صحیح	چوڑائی = MIN(10, n);

	    اگر (چوڑائی > 0)
		screen_puts_len(showcmd_buf, چوڑائی, 0, (صحیح)Columns
			    - چوڑائی - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	اگر (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (صحیح)Columns - 1, attr_nosel);
	    Tab_صفحہ_Idxs[Columns - 1] = -999;
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
    خالی
get_trans_bufname(buf_T *buf)
{
    اگر (buf_spname(buf) != تہی)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    ورنہ
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    صحیح
fillchar_حالت(صحیح *attr, win_T *wp)
{
    صحیح	    fill;
    bool    override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (bt_terminal(wp->w_بفر))
    {
	اگر (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	ورنہ
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    ورنہ
#اگر_ختم
    اگر (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    ورنہ
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    اگر (override_success)
	نکالو_highlight_overrides();
    واپس fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ساکن bool
vsep_صف_is_curwin(win_T *wp, صحیح صف)
{
    اگر (wp == curwin)
	واپس true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    اگر (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_چوڑائی
	    && صف >= curwin->w_winrow
	    && صف < W_WINROW(curwin) + curwin->w_اونچائی
						   + curwin->w_حالت_اونچائی)
	واپس true;

    واپس false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    صحیح
fillchar_vsep(صحیح *attr, win_T *wp, صحیح صف)
{
    bool override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
    اگر (vsep_صف_is_curwin(wp, صف))
	*attr = HL_ATTR(HLF_C);
    ورنہ
	*attr = HL_ATTR(HLF_CNC);
    اگر (override_success)
	نکالو_highlight_overrides();

    اگر (*attr == 0 && wp->w_fill_chars.vert == ' ')
	واپس '|';
    ورنہ
	واپس wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    صحیح
redrawing(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (disable_redraw_for_testing)
	واپس 0;
    ورنہ
#اگر_ختم
	واپس ((RedrawingDisabled == 0
#اگر_تعریف_شدہ FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#اگر_ختم
		) && !(p_lz && char_avail() && !کلید_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    صحیح
messaging(خالی)
{
    واپس (!(p_lz && char_avail() && !کلید_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#تعریف_کرو ستون_RULER 17	    // columns needed by standard ruler

    خالی
comp_ستون(خالی)
{
    صحیح آخری_has_حالت = آخری_stl_اونچائی(FALSE) > 0;

    sc_ستون = 0;
    ru_ستون = 0;
    اگر (p_ru)
    {
#اگر_تعریف_شدہ FEAT_STL_OPT
	ru_ستون = (ru_wid ? ru_wid : ستون_RULER) + 1;
#ورنہ
	ru_ستون = ستون_RULER + 1;
#اگر_ختم
	// no last status line, adjust sc_col
	اگر (!آخری_has_حالت)
	    sc_ستون = ru_ستون;
    }
    اگر (p_sc && *p_sloc == 'l')
    {
	sc_ستون += SHOWCMD_ستون_تعداد;
	اگر (!p_ru || آخری_has_حالت)	    // no need for separating space
	    ++sc_ستون;
    }
    sc_ستون = cmdline_چوڑائی - sc_ستون;
    ru_ستون = cmdline_چوڑائی - ru_ستون;
    اگر (sc_ستون <= 0)		// screen too narrow, will become a mess
	sc_ستون = 1;
    اگر (ru_ستون <= 0)
	ru_ستون = 1;
#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_nr(VV_ECHOSPACE, sc_ستون - 1);
#اگر_ختم
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    صحیح
نمبر_چوڑائی(win_T *wp)
{
    صحیح		n;
    linenr_T	lnum;

    اگر (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_اونچائی;
    ورنہ
	// cursor line shows absolute line number
	lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;

    اگر (lnum == wp->w_nrwidth_سطر_گنتی && wp->w_nuw_cached == wp->w_p_nuw)
	واپس wp->w_nrwidth_چوڑائی;
    wp->w_nrwidth_سطر_گنتی = lnum;

    n = 0;
    کرو
    {
	lnum /= 10;
	++n;
    } جب (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    اگر (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    اگر (n < 2 && get_پہلا_valid_sign(wp) != تہی
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_چوڑائی = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    واپس n;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    صحیح
screen_screencol(خالی)
{
    واپس screen_cur_ستون;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    صحیح
screen_screenrow(خالی)
{
    واپس screen_cur_صف;
}
#اگر_ختم

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ساکن صحیح
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    اگر (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T عدد = 0;
	صحیح	    بائٹیں;
	صحیح	    n;

	برائے (بائٹیں = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; بائٹیں > 0; --بائٹیں)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    اگر (n < 0)
		واپس 0;
	    عدد = عدد * 256 + n;
	}
	*p += 2;
	واپس عدد;
    }
    واپس mb_ptr2char_adv(p);
}

ساخت charstab
{
    صحیح	    *cp;
    string_T	name;
};

#تعریف_کرو CHARSTAB_اندراج(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ساکن fill_chars_T fill_chars;
ساکن ساخت charstab filltab[] =
{
    CHARSTAB_اندراج(&fill_chars.stl,	    "stl"),
    CHARSTAB_اندراج(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_اندراج(&fill_chars.vert,	    "vert"),
    CHARSTAB_اندراج(&fill_chars.fold,	    "fold"),
    CHARSTAB_اندراج(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_اندراج(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_اندراج(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_اندراج(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_اندراج(&fill_chars.diff,	    "diff"),
    CHARSTAB_اندراج(&fill_chars.eob,	    "eob"),
    CHARSTAB_اندراج(&fill_chars.lastline,    "lastline"),
#اگر defined(FEAT_TABPANEL)
    CHARSTAB_اندراج(&fill_chars.tpl_vert,    "tpl_vert"),
#اگر_ختم
    CHARSTAB_اندراج(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_اندراج(&fill_chars.truncrl,	    "truncrl"),
};
ساکن lcs_chars_T lcs_chars;
ساکن ساخت charstab lcstab[] =
{
    CHARSTAB_اندراج(&lcs_chars.eol,	    "eol"),
    CHARSTAB_اندراج(&lcs_chars.ext,	    "extends"),
    CHARSTAB_اندراج(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_اندراج(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_اندراج(&lcs_chars.space,	    "space"),
    CHARSTAB_اندراج(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_اندراج(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_اندراج(&lcs_chars.trail,	    "trail"),
    CHARSTAB_اندراج(&lcs_chars.lead,	    "lead"),
#اگر_تعریف_شدہ FEAT_CONCEAL
    CHARSTAB_اندراج(&lcs_chars.conceal,	    "conceal"),
#ورنہ
    CHARSTAB_اندراج(تہی,		    "conceal"),
#اگر_ختم
    CHARSTAB_اندراج(تہی,		    "multispace"),
    CHARSTAB_اندراج(تہی,		    "leadmultispace")
};

    ساکن حرف *
میدان_قدر_err(حرف *errbuf, size_t errbuflen, حرف *fmt, char_u *میدان)
{
    اگر (errbuf == تہی)
	واپس "";
    vim_snprintf(errbuf, errbuflen, _(fmt), میدان);
    واپس errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ساکن حرف *
سیٹ_chars_option(win_T *wp, char_u *قدر, صحیح is_listchars, صحیح apply,
						حرف *errbuf, size_t errbuflen)
{
    صحیح	    باری, i, entries;
    char_u  *p, *s;
    صحیح	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *آخری_multispace = تہی;  // Last occurrence of "multispace:"
    char_u  *آخری_lmultispace = تہی; // Last occurrence of "leadmultispace:"
    صحیح	    multispace_len = 0;	      // Length of lcs-multispace string
    صحیح	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    ساخت charstab *tab;

    اگر (is_listchars)
    {
	tab = lcstab;
	CLEAR_میدان(lcs_chars);
	entries = ARRAY_لمبائی(lcstab);
	اگر (wp->w_p_lcs[0] == NUL)
	    قدر = p_lcs;  // local value is empty, use the global value
    }
    ورنہ
    {
	tab = filltab;
	entries = ARRAY_لمبائی(filltab);
	اگر (wp->w_p_fcs[0] == NUL)
	    قدر = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    برائے (باری = 0; باری <= (apply ? 1 : 0); ++باری)
    {
	صحیح has_tab = FALSE, has_leadtab = FALSE;

	اگر (باری > 0)
	{
	    // After checking that the value is valid: set defaults.
	    اگر (is_listchars)
	    {
		برائے (i = 0; i < entries; ++i)
		    اگر (tab[i].cp != تہی)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		اگر (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(صحیح, multispace_len + 1);
		    اگر (lcs_chars.multispace != تہی)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		ورنہ
		    lcs_chars.multispace = تہی;

		اگر (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(صحیح, lead_multispace_len + 1);
		    اگر (lcs_chars.leadmultispace != تہی)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		ورنہ
		    lcs_chars.leadmultispace = تہی;
	    }
	    ورنہ
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
#اگر_ختم
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = قدر;
	جب (*p)
	{
	    برائے (i = 0; i < entries; ++i)
	    {
		اگر (!(STRNCMP(p, tab[i].name.string, tab[i].name.لمبائی) == 0 && p[tab[i].name.لمبائی] == ':'))
		    جاری;

		s = p + tab[i].name.لمبائی + 1;

		اگر (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    اگر (باری == 0)
		    {
			// Get length of lcs-multispace string in first round
			آخری_multispace = p;
			multispace_len = 0;
			جب (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (char2cells(c1) > 1)
				واپس میدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_چوڑائی_for_میدان_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			اگر (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    واپس میدان_قدر_err(errbuf, errbuflen,
				    e_wrong_نمبر_کا_characters_for_میدان_str,
				    tab[i].name.string);
		    }
		    ورنہ
		    {
			صحیح multispace_pos = 0;

			جب (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (p == آخری_multispace && lcs_chars.multispace != تہی)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    توڑو;
		}

		اگر (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    اگر (باری == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			//باری
			آخری_lmultispace = p;
			lead_multispace_len = 0;
			جب (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (char2cells(c1) > 1)
				واپس میدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_چوڑائی_for_میدان_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			اگر (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    واپس میدان_قدر_err(errbuf, errbuflen,
				    e_wrong_نمبر_کا_characters_for_میدان_str,
				    tab[i].name.string);
		    }
		    ورنہ
		    {
			صحیح multispace_pos = 0;

			جب (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    اگر (p == آخری_lmultispace && lcs_chars.leadmultispace != تہی)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    توڑو;
		}

		c2 = c3 = 0;
		اگر (*s == NUL)
		    واپس میدان_قدر_err(errbuf, errbuflen,
				    e_wrong_نمبر_کا_characters_for_میدان_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		اگر (char2cells(c1) > 1)
		    واپس میدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_چوڑائی_for_میدان_str,
					 tab[i].name.string);
		اگر (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    اگر (*s == NUL)
			واپس میدان_قدر_err(errbuf, errbuflen,
				    e_wrong_نمبر_کا_characters_for_میدان_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    اگر (char2cells(c2) > 1)
			واپس میدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_چوڑائی_for_میدان_str,
					 tab[i].name.string);
		    اگر (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			اگر (char2cells(c3) > 1)
			    واپس میدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_چوڑائی_for_میدان_str,
					 tab[i].name.string);
		    }
		    اگر (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    ورنہ  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		اگر (*s == ',' || *s == NUL)
		{
		    اگر (باری > 0)
		    {
			اگر (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			ورنہ اگر (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			ورنہ اگر (tab[i].cp != تہی)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    توڑو;
		}
		ورنہ
		    واپس میدان_قدر_err(errbuf, errbuflen,
				    e_wrong_نمبر_کا_characters_for_میدان_str,
				    tab[i].name.string);
	    }

	    اگر (i == entries)
		واپس e_invalid_دلیل;

	    اگر (*p == ',')
		++p;
	}

	اگر (is_listchars && has_leadtab && !has_tab)
	    واپس e_leadtab_requires_tab;
    }

    اگر (apply)
    {
	اگر (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	ورنہ
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    واپس تہی;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    حرف *
سیٹ_fillchars_option(win_T *wp, char_u *val, صحیح apply, حرف *errbuf,
							      size_t errbuflen)
{
    واپس سیٹ_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    حرف *
سیٹ_listchars_option(win_T *wp, char_u *val, صحیح apply, حرف *errbuf,
							      size_t errbuflen)
{
    واپس سیٹ_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(filltab))
	واپس تہی;

    واپس filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(lcstab))
	واپس تہی;

    واپس lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    حرف *
check_chars_options(خالی)
{
    tabpage_T   *tp;
    win_T	    *wp;

    اگر (سیٹ_listchars_option(curwin, p_lcs, FALSE, تہی, 0) != تہی)
	واپس e_conflicts_with_قدر_کا_listchars;
    اگر (سیٹ_fillchars_option(curwin, p_fcs, FALSE, تہی, 0) != تہی)
	واپس e_conflicts_with_قدر_کا_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (سیٹ_listchars_option(wp, wp->w_p_lcs, FALSE, تہی, 0) != تہی)
	    واپس e_conflicts_with_قدر_کا_listchars;
	اگر (سیٹ_fillchars_option(wp, wp->w_p_fcs, FALSE, تہی, 0) != تہی)
	    واپس e_conflicts_with_قدر_کا_fillchars;
    }
    واپس تہی;
}
