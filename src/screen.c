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

#ضمن "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
ساكن صحيح	screen_attr = 0;

ساكن فراغ screen_char_2(بنانشان off, صحيح صف, صحيح کانو);
ساكن صحيح screenclear2(صحيح doclear);
ساكن فراغ lineclear(بنانشان off, صحيح ويڪر, صحيح attr);
ساكن فراغ lineinvalid(بنانشان off, صحيح ويڪر);
ساكن صحيح win_do_سٽون(win_T *wp, صحيح صف, صحيح سٽ_ڳڻپ, صحيح mayclear, صحيح del, صحيح clear_attr);
ساكن فراغ win_rest_invalid(win_T *wp);
ساكن فراغ نياپو_pos_گهڻائي(فراغ);
ساكن فراغ recording_گهڻائي(صحيح attr);

// Ugly global: overrule attribute used by screen_char()
ساكن صحيح screen_char_attr = 0;

#إذا defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    صحيح
conceal_cursor_سٽ(win_T *wp)
{
    صحيح		c;

    إذا (*wp->w_p_cocu == NUL)
	أرجع FALSE;
    إذا (get_real_state() & گهڻائي_VISUAL)
	c = 'v';
    وإلا إذا (State & گهڻائي_وجهو)
	c = 'i';
    وإلا إذا (State & گهڻائي_عام)
	c = 'n';
    وإلا إذا (State & گهڻائي_CMDLINE)
	c = 'c';
    وإلا
	أرجع FALSE;
    أرجع vim_strchr(wp->w_p_cocu, c) != عدم;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    فراغ
conceal_check_cursor_سٽ(صحيح was_concealed)
{
    إذا (curwin->w_p_cole <= 0 || conceal_cursor_سٽ(curwin) == was_concealed)
	أرجع;

    صحيح wcol = curwin->w_wcol;

    need_cursor_سٽ_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    إذا (!was_concealed)
	curwin->w_wcol = wcol;
}
#جيڪڏهن_پورو

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    صحيح
get_win_attr(win_T *wp)
{
    صحيح	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);

    إذا (win_attr != 0)
    {
	إذا (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	وإلا
	    win_attr = 0;
    }
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    وإلا إذا (WIN_IS_POPUP(wp))
    {
	إذا (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	وإلا
	    win_attr = HL_ATTR(HLF_ڪڍو);    // Popup
    }
#جيڪڏهن_پورو

    إذا (override_success)
	ڪڍو_highlight_overrides();
    أرجع win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ساكن صحيح
screen_fill_پڇاڙي(
	win_T *wp,
	صحيح	c1,
	صحيح	c2,
	صحيح	off,
	صحيح	ويڪر,
	صحيح	صف,
	صحيح	endrow,
	صحيح	attr)
{
    صحيح	    nn = off + ويڪر;

    إذا (nn > wp->w_ويڪر)
	nn = wp->w_ويڪر;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (صحيح)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    وإلا
#جيڪڏهن_پورو
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (صحيح)wp->w_wincol + nn,
		c1, c2, attr);
    أرجع nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    فراغ
win_draw_پڇاڙي(
    win_T	*wp,
    صحيح		c1,
    صحيح		c2,
    صحيح		draw_margin,
    صحيح		صف,
    صحيح		endrow,
    hlf_T	hl)
{
    صحيح		n = 0;
    صحيح		attr = HL_ATTR(hl);
    صحيح		win_attr = get_win_attr(wp);
    bool	override_success =
	ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    إذا (draw_margin)
    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	صحيح	fdc = compute_foldcolumn(wp, 0);

	إذا (fdc > 0)
	    // draw the fold column
	    n = screen_fill_پڇاڙي(wp, ' ', ' ', n, fdc,
		      صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	إذا (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_پڇاڙي(wp, ' ', ' ', n, 2,
		      صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#جيڪڏهن_پورو
	إذا ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == عدم)
	    // draw the number column
	    n = screen_fill_پڇاڙي(wp, ' ', ' ', n, عدد_ويڪر(wp) + 1,
		       صف, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    وإلا
#جيڪڏهن_پورو
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (صحيح)W_ENDCOL(wp), c1, c2, attr);
    }

    سيٽ_خالي_جايون_صفون(wp, صف);

    إذا (override_success)
	ڪڍو_highlight_overrides();
}

#إذا defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    صحيح
compute_foldcolumn(win_T *wp, صحيح کانو)
{
    صحيح wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    صحيح n = wp->w_ويڪر - (کانو + wmw);

    أرجع MIN(wp->w_p_fdc, n);
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
    صحيح		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    صحيح		i = 0;
    صحيح		level;
    صحيح		پهريون_level;
    صحيح		خالي_جايون_2;
    صحيح		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    صحيح		نشان = 0;
    صحيح		len = 0;
    صحيح		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    خالي_جايون_2 = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    پهريون_level = level - fdc - closed + 1 + خالي_جايون_2;
    إذا (پهريون_level < 1)
	پهريون_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    لكل (i = 0; i < n; i++)
    {
	إذا (win_foldinfo.fi_lnum == lnum
		&& پهريون_level + i >= win_foldinfo.fi_هيٺيون_level)
	    نشان = wp->w_fill_chars.foldopen;
	وإلا إذا (پهريون_level == 1)
	    نشان = wp->w_fill_chars.foldsep;
	وإلا إذا (wp->w_fill_chars.foldinner != NUL)
	    نشان = wp->w_fill_chars.foldinner;
	وإلا إذا (پهريون_level + i <= 9)
	    نشان = '0' + پهريون_level + i;
	وإلا
	    نشان = '>';

	len = utf_char2bytes(نشان, &p[byte_counter]);
	byte_counter += len;
	إذا (پهريون_level + i >= level)
	{
	    i++;
	    اكسر;
	}
    }

    إذا (closed)
    {
	إذا (نشان != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    إذا (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	نشان = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(نشان, &p[byte_counter]);
	byte_counter += len;
    }

    أرجع MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#جيڪڏهن_پورو // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ساكن صحيح
comp_char_differs(صحيح off_from, صحيح off_منزل)
{
    صحيح	    i;

    لكل (i = 0; i < Screen_mco; ++i)
    {
	إذا (Screen_سٽون_C[i][off_from] != Screen_سٽون_C[i][off_منزل])
	    أرجع TRUE;
	إذا (Screen_سٽون_C[i][off_from] == 0)
	    اكسر;
    }
    أرجع FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ساكن صحيح
char_needs_redraw(صحيح off_from, صحيح off_منزل, صحيح کانن_ڳڻپ)
{
    إذا (کانن_ڳڻپ > 0
	    && ((Screen_سٽون[off_from] != Screen_سٽون[off_منزل]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_منزل])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_سٽون[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_سٽون[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_منزل]
			: (کانن_ڳڻپ > 1 && Screen_سٽون[off_from + 1]
						 != Screen_سٽون[off_منزل + 1])))
		|| (enc_utf8
		    && (Screen_سٽون_UC[off_from] != Screen_سٽون_UC[off_منزل]
			|| (Screen_سٽون_UC[off_from] != 0
			    && comp_char_differs(off_from, off_منزل))
			|| ((*mb_off2cells)(off_from, off_from + کانن_ڳڻپ) > 1
			    && Screen_سٽون[off_from + 1]
						!= Screen_سٽون[off_منزل + 1])))))
	أرجع TRUE;
    أرجع FALSE;
}

#إذا defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    صحيح
screen_get_current_سٽ_off(فراغ)
{
    أرجع (صحيح)(current_Screen_سٽ - Screen_سٽون);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ساكن صحيح
blocked_by_popup(صحيح صف, صحيح کانو)
{
    صحيح off;

    إذا (!popup_visible)
	أرجع FALSE;
    off = صف * screen_Columns + کانو;
    أرجع popup_نقاب[off] > screen_zindex || popup_transparent[off];
}
#جيڪڏهن_پورو

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    فراغ
reset_screen_attr(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    وإلا
#جيڪڏهن_پورو
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ساكن صحيح
skip_for_popup(صحيح صف, صحيح کانو)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    إذا (pum_under_menu(صف, کانو, TRUE)
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#جيڪڏهن_پورو
	    )
	أرجع TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & گهڻائي_CMDLINE) == 0
	    && pum_under_menu(صف, کانو, FALSE))
	أرجع TRUE;
#وإلا
    إذا (pum_visible()
	    && (State & گهڻائي_CMDLINE) == 0
	    && pum_under_menu(صف, کانو, FALSE))
	أرجع TRUE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (blocked_by_popup(صف, کانو))
	أرجع TRUE;
#جيڪڏهن_پورو
    أرجع FALSE;
}

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ساكن char_u	*suppressed_cells = عدم;
ساكن صحيح	suppressed_صفون = 0;
ساكن صحيح	suppressed_کانن_ڳڻپ = 0;

    ساكن فراغ
mark_suppressed_cell(صحيح صف, صحيح کانو)
{
    إذا (suppressed_cells == عدم
	    || suppressed_صفون != screen_صفون
	    || suppressed_کانن_ڳڻپ != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_صفون * screen_Columns);
	إذا (suppressed_cells == عدم)
	{
	    suppressed_صفون = 0;
	    suppressed_کانن_ڳڻپ = 0;
	    أرجع;
	}
	suppressed_صفون = screen_صفون;
	suppressed_کانن_ڳڻپ = screen_Columns;
    }
    suppressed_cells[صف * suppressed_کانن_ڳڻپ + کانو] = TRUE;
}

    ساكن فراغ
unmark_suppressed_cell(صحيح صف, صحيح کانو)
{
    إذا (suppressed_cells != عدم
	    && صف >= 0 && صف < suppressed_صفون
	    && کانو >= 0 && کانو < suppressed_کانن_ڳڻپ)
	suppressed_cells[صف * suppressed_کانن_ڳڻپ + کانو] = FALSE;
}

    ساكن صحيح
is_suppressed_cell(صحيح صف, صحيح کانو)
{
    أرجع suppressed_cells != عدم
	&& صف >= 0 && صف < suppressed_صفون
	&& کانو >= 0 && کانو < suppressed_کانن_ڳڻپ
	&& suppressed_cells[صف * suppressed_کانن_ڳڻپ + کانو];
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
تعريفنوع بنية {
    صحيح		popup_attr;	// get_win_attr(screen_opacity_popup)
    صحيح		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ساكن مضمن فراغ
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ساكن مضمن صحيح
popup_بنياد_attr_or(صحيح صف, صحيح کانو, صحيح fallback)
{
    صحيح		بنياد = fallback;

    popup_get_بنياد_screen_cell(صف, کانو, عدم, &بنياد, عدم);
    أرجع بنياد;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ساكن مضمن صحيح
popup_blend_with_بنياد(
	ثابت popup_opacity_T	*po,
	صحيح			char_attr,
	صحيح			بنياد_attr,
	صحيح			blend_fg)
{
    صحيح		combined = hl_combine_attr(po->popup_attr, char_attr);

    أرجع hl_blend_attr(بنياد_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ساكن فراغ
resolve_wide_char_opacity_attrs(
	صحيح صف, صحيح col1, صحيح col2,
	sattr_T *attr1, sattr_T *attr2)
{
    صحيح		bg1, bg2;
    صحيح		base1 = 0;
    صحيح		base2 = 0;
    attrentry_T	*ae;

    إذا (*attr1 == *attr2)
	أرجع;

    popup_get_بنياد_screen_cell(صف, col1, عدم, &base1, عدم);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != عدم && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# وإلا
    bg1 = (ae != عدم && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_بنياد_screen_cell(صف, col2, عدم, &base2, عدم);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != عدم && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# وإلا
    bg2 = (ae != عدم && ae->ae_u.cterm.bg_color != 0);
# endif

    إذا (bg1 && !bg2)
	*attr1 = *attr2;
    وإلا إذا (!bg1 && bg2)
	*attr2 = *attr1;
}
#جيڪڏهن_پورو

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
    فراغ
screen_سٽ(
	win_T	*wp,
	صحيح	صف,
	صحيح	coloff,
	صحيح	endcol,
	صحيح	clear_ويڪر,
	colnr_T	آخري_vcol,
	صحيح	flags اڻ_استعمال_ٿيل)
{
    بنانشان	    off_from;
    بنانشان	    off_منزل;
    بنانشان	    max_off_from;
    بنانشان	    max_off_منزل;
    صحيح		    کانو = 0;
    صحيح		    hl;
    صحيح		    force = FALSE;	// force update rest of the line
    صحيح		    redraw_this		// bool: does character need redraw?
#جيڪڏهن_وصف_ٿيل FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#جيڪڏهن_پورو
				;
    صحيح		    redraw_ايندڙ;	// redraw_this for next character
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
    صحيح		    بدليو_this;	// TRUE if character changed
    صحيح		    بدليو_ايندڙ;	// TRUE if next character changed
#جيڪڏهن_پورو
    صحيح		    clear_ايندڙ = FALSE;
    صحيح		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    صحيح		    drawing_opacity_popup =
				screen_opacity_popup != عدم
				&& (flags & SLF_POPUP);
    CLEAR_ميدان(po);
    إذا (drawing_opacity_popup)
	popup_opacity_init(&po);
#جيڪڏهن_پورو

    // Check for illegal row and col, just in case.
    إذا (صف >= صفون)
	صف = صفون - 1;
    إذا (endcol > Columns)
	endcol = Columns;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    clip_may_clear_selection(صف, صف);
#جيڪڏهن_پورو

    off_from = (بنانشان)(current_Screen_سٽ - Screen_سٽون);
    off_منزل = سٽ_وٿي[صف] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_منزل = سٽ_وٿي[صف] + screen_Columns;

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	إذا (clear_ويڪر > 0)
	{
	    صحيح clear_شروعات = کانو;

	    طالما (کانو <= endcol && Screen_سٽون[off_منزل] == ' '
		    && ScreenAttrs[off_منزل] == 0
				  && (!enc_utf8 || Screen_سٽون_UC[off_منزل] == 0))
	    {
		++off_منزل;
		++کانو;
	    }
	    إذا (کانو <= endcol)
		screen_fill(صف, صف + 1, کانو + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    لكل (صحيح i = endcol; i >= clear_شروعات; i--)
		Screen_کانن_ڳڻپ[off_منزل + (i - کانو)] =
		    (flags & SLF_INC_VCOL) ? ++آخري_vcol : آخري_vcol;
	}
	کانو = endcol + 1;
	off_منزل = سٽ_وٿي[صف] + کانو + coloff;
	off_from += کانو;
	endcol = (clear_ويڪر > 0 ? clear_ويڪر : -clear_ويڪر);
    }
#جيڪڏهن_پورو // FEAT_RIGHTLEFT

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    إذا (coloff > 0 && enc_utf8
		   && Screen_سٽون[off_منزل] == 0
		   && Screen_سٽون_UC[off_منزل - 1] != 0
		   && (*mb_char2cells)(Screen_سٽون_UC[off_منزل - 1]) > 1)
    {
	Screen_سٽون[off_منزل - 1] = ' ';
	Screen_سٽون_UC[off_منزل - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	إذا (screen_opacity_popup == عدم)
# endif
	    screen_char(off_منزل - 1, صف, کانو + coloff - 1);
    }
#جيڪڏهن_پورو

    redraw_ايندڙ = char_needs_redraw(off_from, off_منزل, endcol - کانو);
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
    بدليو_ايندڙ = redraw_ايندڙ;
#جيڪڏهن_پورو

    طالما (کانو < endcol)
    {
	إذا (has_mbyte && (کانو + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	وإلا
	    char_cells = 1;

	redraw_this = redraw_ايندڙ;
	redraw_ايندڙ = force || char_needs_redraw(off_from + char_cells,
			      off_منزل + char_cells, endcol - کانو - char_cells);

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	إذا (!redraw_this && is_suppressed_cell(صف, کانو + coloff))
	    redraw_this = TRUE;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	بدليو_this = بدليو_ايندڙ;
	بدليو_ايندڙ = redraw_ايندڙ;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	إذا (redraw_ايندڙ && gui.اندريون_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_منزل + char_cells];
	    إذا (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    إذا (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#جيڪڏهن_پورو
	// skip the second cell for double-width characters.
	إذا (redraw_this && char_cells == 2 && skip_for_popup(صف, کانو + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	إذا (redraw_this && skip_for_popup(صف, کانو + coloff))
	    redraw_this = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	صحيح opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	إذا (drawing_opacity_popup
		&& Screen_سٽون[off_from] == ' '
		&& (!enc_utf8 || Screen_سٽون_UC[off_from] == 0))
	{
	    صحيح popup_src_attr = ScreenAttrs[off_from];
	    صحيح bg_is_خالي_جايون = Screen_سٽون[off_منزل] == 0
				&& (!enc_utf8 || Screen_سٽون_UC[off_منزل] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    إذا (bg_is_خالي_جايون
		    && off_منزل > 0
		    && enc_utf8 && Screen_سٽون_UC[off_منزل - 1] != 0
		    && utf_char2cells(Screen_سٽون_UC[off_منزل - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    وإلا إذا (Screen_سٽون[off_منزل] != 0
		    || (enc_utf8 && Screen_سٽون_UC[off_منزل] != 0))
	    {
		صحيح bg_char_cells = (enc_utf8 && Screen_سٽون_UC[off_منزل] != 0)
				? utf_char2cells(Screen_سٽون_UC[off_منزل]) : 1;

		إذا (bg_char_cells == 2)
		{
		    إذا (کانو + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_منزل + 1 >= max_off_منزل)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			صحيح بنياد_attr;
			Screen_سٽون[off_منزل] = ' ';
			إذا (enc_utf8)
			    Screen_سٽون_UC[off_منزل] = 0;
			بنياد_attr = popup_بنياد_attr_or(صف, کانو + coloff,
							     ScreenAttrs[off_منزل]);
			ScreenAttrs[off_منزل] = popup_blend_with_بنياد(&po,
					    popup_src_attr, بنياد_attr, TRUE);
			screen_char(off_منزل, صف, کانو + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			اذهب skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    إذا (!(Screen_سٽون[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_سٽون_UC[off_from + 1] == 0)))
			اذهب skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		صحيح بنياد_attr = popup_بنياد_attr_or(صف, کانو + coloff,
							     ScreenAttrs[off_منزل]);
		ScreenAttrs[off_منزل] = popup_blend_with_بنياد(&po,
					    popup_src_attr, بنياد_attr, TRUE);
		screen_char(off_منزل, صف, کانو + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		إذا (bg_char_cells == 2)
		{
		    صحيح بنياد_attr2 = popup_بنياد_attr_or(صف, کانو + coloff + 1,
							 ScreenAttrs[off_منزل + 1]);
		    ScreenAttrs[off_منزل + 1] = popup_blend_with_بنياد(&po,
					    popup_src_attr, بنياد_attr2, TRUE);
		    إذا (po.blend == 100)
			resolve_wide_char_opacity_attrs(صف,
				کانو + coloff, کانو + coloff + 1,
				&ScreenAttrs[off_منزل], &ScreenAttrs[off_منزل + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    وإلا إذا (bg_is_خالي_جايون)
	    {
		صحيح بنياد_attr;
		Screen_سٽون[off_منزل] = ' ';
		بنياد_attr = popup_بنياد_attr_or(صف, کانو + coloff,
							     ScreenAttrs[off_منزل]);
		ScreenAttrs[off_منزل] = popup_blend_with_بنياد(&po,
					    popup_src_attr, بنياد_attr, TRUE);
		screen_char(off_منزل, صف, کانو + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#جيڪڏهن_پورو

	إذا (redraw_this)
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
	    إذا (       p_wiv
		    && !force
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		    && !gui.اندريون_use
#جيڪڏهن_پورو
		    && ScreenAttrs[off_منزل] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_منزل])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(صف, کانو + coloff);
		ٻاهريون_str(T_CE);		// clear rest of this screen line
		screen_شروعات();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_ايندڙ = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		إذا (کانو + coloff > 0 && ScreenAttrs[off_منزل - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_منزل - 1];
		    رڪن_windgoto(صف, کانو + coloff);
		    screen_stop_highlight();
		}
		وإلا
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    إذا (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		إذا (char_cells == 1
			&& کانو + 1 < endcol
			&& (*mb_off2cells)(off_منزل, max_off_منزل) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_سٽون[off_منزل + 1] = 0;
		    redraw_ايندڙ = TRUE;
		}
		وإلا إذا (char_cells == 2
			&& کانو + 2 < endcol
			&& (*mb_off2cells)(off_منزل, max_off_منزل) == 1
			&& (*mb_off2cells)(off_منزل + 1, max_off_منزل) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_سٽون[off_منزل + 2] = 0;
		    redraw_ايندڙ = TRUE;
		}

		إذا (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_منزل] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    إذا (has_mbyte && کانو + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_منزل, max_off_منزل) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_منزل, max_off_منزل) == 1
			    && (*mb_off2cells)(off_منزل + 1, max_off_منزل) > 1)))
		clear_ايندڙ = TRUE;

	    Screen_سٽون[off_منزل] = Screen_سٽون[off_from];
	    إذا (enc_utf8)
	    {
		Screen_سٽون_UC[off_منزل] = Screen_سٽون_UC[off_from];
		إذا (Screen_سٽون_UC[off_from] != 0)
		{
		    صحيح	    i;

		    لكل (i = 0; i < Screen_mco; ++i)
			Screen_سٽون_C[i][off_منزل] = Screen_سٽون_C[i][off_from];
		}
	    }
	    إذا (char_cells == 2)
	    {
		Screen_سٽون[off_منزل + 1] = Screen_سٽون[off_from + 1];
		إذا (enc_utf8)
		    Screen_سٽون_UC[off_منزل + 1] = 0;
	    }

#إذا defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    إذا (
# ifdef FEAT_GUI
		    gui.اندريون_use
# endif
# إذا defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    رڪن_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_منزل];
		إذا (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		إذا (hl & HL_BOLD)
		    redraw_ايندڙ = TRUE;
	    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    إذا (gui.اندريون_use && بدليو_this)
		redraw_ايندڙ = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    إذا (gui.directx_enabled && بدليو_this)
		redraw_this = TRUE;
# endif
#جيڪڏهن_پورو

	    ScreenAttrs[off_منزل] = ScreenAttrs[off_from];
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    إذا (drawing_opacity_popup && po.blend > 0)
	    {
		صحيح popup_src_attr = ScreenAttrs[off_from];
		صحيح scol1 = کانو + coloff;
		صحيح base1 = popup_بنياد_attr_or(صف, scol1, 0);

		إذا (char_cells == 2)
		{
		    صحيح base2 = popup_بنياد_attr_or(صف, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    صحيح over1 = popup_is_over_opacity(صف, scol1);
		    صحيح over2 = popup_is_over_opacity(صف, scol1 + 1);
		    إذا (over1 != over2)
		    {
			إذا (over1)
			    base1 = base2;
			وإلا
			    base2 = base1;
		    }
		    ScreenAttrs[off_منزل + 1] = popup_blend_with_بنياد(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_منزل] = popup_blend_with_بنياد(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    وإلا
#جيڪڏهن_پورو
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    إذا (char_cells == 2)
		ScreenAttrs[off_منزل + 1] = ScreenAttrs[off_منزل];

	    إذا (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_منزل, صف, کانو + coloff);
	    وإلا
		screen_char(off_منزل, صف, کانو + coloff);
	}
	وإلا إذا (  p_wiv
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		&& !gui.اندريون_use
#جيڪڏهن_پورو
		&& کانو + coloff > 0 && off_منزل > 0)
	{
	    إذا (ScreenAttrs[off_منزل] == ScreenAttrs[off_منزل - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    وإلا إذا (screen_attr != 0)
		screen_stop_highlight();
	}

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	إذا (!opacity_blank)
#جيڪڏهن_پورو
	{
	    Screen_کانن_ڳڻپ[off_منزل] = Screen_کانن_ڳڻپ[off_from];
	    إذا (char_cells == 2)
		Screen_کانن_ڳڻپ[off_منزل + 1] = Screen_کانن_ڳڻپ[off_from + 1];
	}

	off_منزل += char_cells;
	off_from += char_cells;
	کانو += char_cells;
    }

    إذا (clear_ايندڙ && !skip_for_popup(صف, کانو + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_سٽون[off_منزل] = ' ';
	إذا (enc_utf8)
	    Screen_سٽون_UC[off_منزل] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	إذا (screen_opacity_popup == عدم)
#جيڪڏهن_پورو
	    screen_char(off_منزل, صف, کانو + coloff);
    }

    إذا (clear_ويڪر > 0
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#جيڪڏهن_پورو
				   )
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	صحيح شروعات_کانو_2 = کانو;
#جيڪڏهن_پورو

	// blank out the rest of the line
	طالما (کانو < clear_ويڪر && Screen_سٽون[off_منزل] == ' '
						  && ScreenAttrs[off_منزل] == 0
				  && (!enc_utf8 || Screen_سٽون_UC[off_منزل] == 0)
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
				  && !is_suppressed_cell(صف, کانو + coloff)
#جيڪڏهن_پورو
				  )
	{
	    Screen_کانن_ڳڻپ[off_منزل] =
			      (flags & SLF_INC_VCOL) ? ++آخري_vcol : آخري_vcol;
	    ++off_منزل;
	    ++کانو;
	}
	إذا (کانو < clear_ويڪر)
	{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    إذا (gui.اندريون_use && (کانو > شروعات_کانو_2 || !redraw_this))
	    {
		hl = ScreenAttrs[off_منزل];
		إذا (hl > HL_ALL || (hl & HL_BOLD))
		{
		    صحيح prev_cells = 1;

		    إذا (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_سٽون[off_منزل - 1] == 0 ? 2 : 1;
		    وإلا إذا (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			بنانشان off = سٽ_وٿي[صف];
			بنانشان max_off = سٽ_وٿي[صف] + screen_Columns;

			طالما (off < off_منزل)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    إذا (!skip_for_popup(صف, کانو + coloff - prev_cells))
		    {
			إذا (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_منزل - prev_cells, صف,
						   کانو + coloff - prev_cells);
			وإلا
			    screen_char(off_منزل - prev_cells, صف,
						   کانو + coloff - prev_cells);
		    }
		}
	    }
#جيڪڏهن_پورو
	    screen_fill(صف, صف + 1, کانو + coloff, clear_ويڪر + coloff,
								 ' ', ' ', 0);
	    طالما (کانو < clear_ويڪر)
	    {
		Screen_کانن_ڳڻپ[off_منزل++]
			    = (flags & SLF_INC_VCOL) ? ++آخري_vcol : آخري_vcol;
		++کانو;
	    }
	}
    }

    إذا (clear_ويڪر > 0
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#جيڪڏهن_پورو
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	إذا (coloff + کانو < firstwin->w_wincol + topframe->fr_ويڪر)
	{
	    إذا (!skip_for_popup(صف, کانو + coloff))
	    {
		صحيح c;

		c = fillchar_vsep(&hl, wp, صف);
		إذا (Screen_سٽون[off_منزل] != (schar_T)c
			|| (enc_utf8 && (صحيح)Screen_سٽون_UC[off_منزل]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_منزل] != hl)
		{
		    Screen_سٽون[off_منزل] = c;
		    ScreenAttrs[off_منزل] = hl;
		    إذا (enc_utf8)
		    {
			إذا (c >= 0x80)
			{
			    Screen_سٽون_UC[off_منزل] = c;
			    Screen_سٽون_C[0][off_منزل] = 0;
			    Screen_سٽون[off_منزل] = 0x80; // avoid storing zero
			}
			وإلا
			    Screen_سٽون_UC[off_منزل] = 0;
		    }
		    screen_char(off_منزل, صف, کانو + coloff);
		}
	    }
	}
	وإلا
	    سٽ_Wraps[صف] = FALSE;
    }

    إذا (override_success)
	ڪڍو_highlight_overrides();
}

#إذا defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    فراغ
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    صحيح		t;

    لكل (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#جيڪڏهن_پورو

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    فراغ
draw_vsep_win(win_T *wp, صحيح صف)
{
    إذا (!wp->w_vsep_ويڪر)
	أرجع;

    صحيح content_پڇاڙي = W_WINROW(wp) + wp->w_اوچائي;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    صحيح شروعات_صف = (صف == 0) ? wp->w_winrow : W_WINROW(wp) + صف;
    لكل (صحيح r = شروعات_صف; r < content_پڇاڙي; ++r)
    {
	صحيح hl;
	صحيح c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    إذا (wp->w_حالت_اوچائي != 0)
    {
	صحيح hl;
	صحيح c;
	إذا (stl_connected(wp))
	    c = fillchar_حالت(&hl, wp);
	وإلا
	    c = fillchar_vsep(&hl, wp, content_پڇاڙي);
	لكل (صحيح r = content_پڇاڙي; r < content_پڇاڙي + wp->w_حالت_اوچائي; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    صحيح
stl_connected(win_T *wp)
{
    چوکٺو_T	*fr;

    fr = wp->w_چوکٺو;
    طالما (fr->fr_والد != عدم)
    {
	إذا (fr->fr_والد->fr_layout == FR_کانو)
	{
	    إذا (fr->fr_ايندڙ != عدم)
		اكسر;
	}
	وإلا
	{
	    إذا (fr->fr_ايندڙ != عدم)
		أرجع TRUE;
	}
	fr = fr->fr_والد;
    }
    أرجع FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    صحيح
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    صحيح		len)	    // length of buffer
{
    char_u	*p;
    صحيح		plen;

    إذا (wp->w_بفر->b_p_iminsert != B_IMODE_LMAP)
	أرجع 0;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	منزل_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_بفر;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_منزل_string(منزل_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    إذا (p == عدم || *p == NUL)
#جيڪڏهن_پورو
    {
#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
	إذا (wp->w_بفر->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_بفر->b_p_keymap;
	وإلا
#جيڪڏهن_پورو
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((حرف *)buf, len, (حرف *)fmt, p);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    vim_free(s);
#جيڪڏهن_پورو
    إذا (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    أرجع plen;
}

#إذا defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    فراغ
win_redr_custom(
    win_T	*wp,
    صحيح		draw_ruler)	// TRUE or FALSE
{
    ساكن صحيح	entered = FALSE;
    صحيح		attr;
    صحيح		curattr;
    صحيح		صف;
    صحيح		کانو = 0;
    صحيح		maxwidth;
    صحيح		ويڪر;
    صحيح		n;
    صحيح		len;
    صحيح		stlh_cnt;
    صحيح		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    صحيح		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    صحيح		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    إذا (entered)
	أرجع;
    entered = TRUE;

    // setup environment for the task at hand
    إذا (wp == عدم)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	صف = 0;
	کانو = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_ويڪر;
	opt_name = (char_u *)"tabline";
    }
    وإلا
    {
	override_success = ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	صف = statusline_صف(wp);
	fillchar = fillchar_حالت(&attr, wp);
	صحيح اندريون_حالت_سٽ_2 = wp->w_حالت_اوچائي != 0;
	maxwidth = اندريون_حالت_سٽ_2 ? wp->w_ويڪر : cmdline_ويڪر;

	إذا (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    إذا (*stl == '%')
	    {
		إذا (*++stl == '-')
		    stl++;
		إذا (atoi((حرف *)stl))
		    طالما (VIM_ISDIGIT(*stl))
			stl++;
		إذا (*stl++ != '(')
		    stl = p_ruf;
	    }
	    کانو = ru_کانو - (cmdline_ويڪر - maxwidth);
	    إذا (کانو < (maxwidth + 1) / 2)
		کانو = (maxwidth + 1) / 2;
	    maxwidth -= کانو;
	    إذا (!اندريون_حالت_سٽ_2)
	    {
		صف = صفون - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	وإلا
	{
	    opt_name = (char_u *)"statusline";
	    إذا (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    وإلا
		stl = p_stl;
	}

	إذا (اندريون_حالت_سٽ_2)
	    کانو += wp->w_wincol;
	وإلا
	    کانو += cmdline_کانو_off;
    }

    إذا (maxwidth <= 0)
	اذهب theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == عدم ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_حالت_اوچائي;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == عدم) ? (char_u *)"" : stl;
    صحيح کانو_save = کانو;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **ٻاهريون_regions = عدم;
    صحيح			*ٻاهريون_ڳڻپ = عدم;
    صحيح			region_بنياد_کانو = 0;
    إذا (!draw_ruler)
    {
	إذا (wp != عدم)
	{
	    ٻاهريون_regions = &wp->w_stl_click;
	    ٻاهريون_ڳڻپ = &wp->w_stl_click_ڳڻپ;
	    region_بنياد_کانو = wp->w_wincol;
	}
	وإلا
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    ٻاهريون_regions = &tabline_stl_click;
	    ٻاهريون_ڳڻپ = &tabline_stl_click_ڳڻپ;
	    region_بنياد_کانو = firstwin->w_wincol;
	}

	لكل (n = 0; n < *ٻاهريون_ڳڻپ; n++)
	    vim_free((*ٻاهريون_regions)[n].funcname);
	VIM_CLEAR(*ٻاهريون_regions);
	*ٻاهريون_ڳڻپ = 0;
    }

    صحيح حاصل_hl = 0;
    لكل (صحيح i = 0; i < stlh_cnt; i++)
    {
	کانو = کانو_save;
	buf[0] = NUL;
	ويڪر = build_stl_str_hl_mline(ewp, buf, حجم(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&حاصل_hl);

	// Make all characters printable.
	p = transstr(buf);
	إذا (p != عدم)
	{
	    len = vim_snprintf((حرف *)buf, حجم(buf), "%s", p);
	    vim_free(p);
	}
	وإلا
	    len = (صحيح)STRLEN(buf);

	// fill up with "fillchar"
	طالما (ويڪر < maxwidth && len < (صحيح)حجم(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++ويڪر;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	لكل (n = 0; hltab[n].شروعات != عدم; n++)
	{
	    len = (صحيح)(hltab[n].شروعات - p);
	    screen_puts_len(p, len, صف + i, کانو, curattr);
	    کانو += vim_strnsize(p, len);
	    p = hltab[n].شروعات;

	    إذا (hltab[n].userhl == 0)
		curattr = attr;
	    وإلا إذا (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    وإلا إذا (wp != عدم && wp != curwin && bt_terminal(wp->w_بفر)
						    && wp->w_حالت_اوچائي != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    وإلا إذا (wp != عدم && bt_terminal(wp->w_بفر)
						    && wp->w_حالت_اوچائي != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    وإلا إذا (wp != عدم && wp != curwin && wp->w_حالت_اوچائي != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    وإلا
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, صف + i, کانو, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	إذا (ٻاهريون_regions != عدم)
	{
	    صحيح click_ڳڻپ = 0;
	    لكل (n = 0; clicktab[n].شروعات != عدم; n++)
		click_ڳڻپ++;

	    إذا (click_ڳڻپ > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*ٻاهريون_regions,
			حجم(stl_click_region_T)
				    * (*ٻاهريون_ڳڻپ + click_ڳڻپ));
		إذا (new_arr != عدم)
		{
		    char_u  *cur_funcname = عدم;
		    صحيح	    cur_minwid = 0;
		    صحيح	    region_شروعات = region_بنياد_کانو;

		    *ٻاهريون_regions = new_arr;

		    len = 0;
		    p = buf;
		    لكل (n = 0; clicktab[n].شروعات != عدم; n++)
		    {
			len += vim_strnsize(p,
					    (صحيح)(clicktab[n].شروعات - p));
			p = clicktab[n].شروعات;

			إذا (cur_funcname != عدم)
			{
			    stl_click_region_T *r =
					&(*ٻاهريون_regions)[*ٻاهريون_ڳڻپ];
			    r->صف = صف + i;
			    r->کانو_شروعات = region_شروعات;
			    r->کانو_پڇاڙي = region_بنياد_کانو + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*ٻاهريون_ڳڻپ)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_شروعات = region_بنياد_کانو + len;
		    }

		    // Close final region if it extends to the end.
		    إذا (cur_funcname != عدم)
		    {
			stl_click_region_T *r =
					&(*ٻاهريون_regions)[*ٻاهريون_ڳڻپ];
			r->صف = صف + i;
			r->کانو_شروعات = region_شروعات;
			r->کانو_پڇاڙي = region_بنياد_کانو + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*ٻاهريون_ڳڻپ)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	لكل (n = 0; clicktab[n].شروعات != عدم; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    إذا (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    إذا (stl != عدم)
	vim_free(stl_tmp);

    إذا (wp == عدم)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	کانو = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	لكل (n = 0; tabtab[n].شروعات != عدم; n++)
	{
	    len += vim_strnsize(p, (صحيح)(tabtab[n].شروعات - p));
	    طالما (کانو < len)
		Tab_صفحو_Idxs[کانو++] = fillchar;
	    p = tabtab[n].شروعات;
	    fillchar = tabtab[n].userhl;
	}
	طالما (کانو < firstwin->w_wincol + topframe->fr_ويڪر)
	    Tab_صفحو_Idxs[کانو++] = fillchar;
    }

theend:
    إذا (override_success)
	ڪڍو_highlight_overrides();
    entered = FALSE;
}

#جيڪڏهن_پورو // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    فراغ
screen_putchar(صحيح c, صحيح صف, صحيح کانو, صحيح attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    إذا (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    وإلا
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, صف, کانو, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    فراغ
screen_getbytes(صحيح صف, صحيح کانو, char_u *بائيٽون, صحيح *attrp)
{
    بنانشان off;

    // safety check
    إذا (Screen_سٽون == عدم || صف >= screen_صفون || کانو >= screen_Columns)
	أرجع;

    off = سٽ_وٿي[صف] + کانو;
    إذا (attrp != عدم)
	*attrp = ScreenAttrs[off];
    بائيٽون[0] = Screen_سٽون[off];
    بائيٽون[1] = NUL;

    إذا (enc_utf8 && Screen_سٽون_UC[off] != 0)
	بائيٽون[utfc_char2bytes(off, بائيٽون)] = NUL;
    وإلا إذا (enc_dbcs == DBCS_JPNU && Screen_سٽون[off] == 0x8e)
    {
	بائيٽون[0] = Screen_سٽون[off];
	بائيٽون[1] = ScreenLines2[off];
	بائيٽون[2] = NUL;
    }
    وإلا إذا (enc_dbcs && MB_BYTE2LEN(بائيٽون[0]) > 1)
    {
	بائيٽون[1] = Screen_سٽون[off + 1];
	بائيٽون[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ساكن صحيح
screen_comp_differs(صحيح off, صحيح *u8cc)
{
    صحيح	    i;

    لكل (i = 0; i < Screen_mco; ++i)
    {
	إذا (Screen_سٽون_C[i][off] != (u8char_T)u8cc[i])
	    أرجع TRUE;
	إذا (u8cc[i] == 0)
	    اكسر;
    }
    أرجع FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    فراغ
screen_puts(
    char_u	*متن,
    صحيح		صف,
    صحيح		کانو,
    صحيح		attr)
{
    screen_puts_len(متن, -1, صف, کانو, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    فراغ
screen_puts_len(
    char_u	*متن,
    صحيح		textlen,
    صحيح		صف,
    صحيح		کانو,
    صحيح		attr_arg)
{
    صحيح		attr = attr_arg;
    بنانشان	off;
    char_u	*ptr = متن;
    صحيح		len = textlen;
    صحيح		c;
    بنانشان	max_off;
    صحيح		mbyte_blen = 1;
    صحيح		mbyte_cells = 1;
    صحيح		u8c = 0;
    صحيح		u8cc[MAX_MCO];
    صحيح		clear_ايندڙ_cell = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
    صحيح		prev_c = 0;		// previous Arabic character
    صحيح		pc, nc, nc1;
    صحيح		pcc[MAX_MCO];
#جيڪڏهن_پورو
    صحيح		force_redraw_this;
    صحيح		force_redraw_ايندڙ = FALSE;
    صحيح		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    إذا (Screen_سٽون == عدم
	    || صف >= screen_صفون || صف < 0
	    || کانو >= screen_Columns || کانو < 0)
	أرجع;
    off = سٽ_وٿي[صف] + کانو;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    إذا (has_mbyte && کانو > 0 && کانو < screen_Columns
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    && !gui.اندريون_use
#جيڪڏهن_پورو
	    && mb_fix_کانو(کانو, صف) != کانو)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	إذا (!skip_for_popup(صف, کانو - 1))
	{
	    Screen_سٽون[off - 1] = ' ';
	    إذا (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    وإلا إذا (enc_utf8)
	    {
		Screen_سٽون_UC[off - 1] = 0;
		Screen_سٽون_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, صف, کانو - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_ايندڙ = TRUE;
    }

    max_off = سٽ_وٿي[صف] + screen_Columns;
    طالما (کانو < screen_Columns
	    && (len < 0 || (صحيح)(ptr - متن) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	إذا (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (صحيح)((متن + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    إذا (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    وإلا إذا (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    وإلا	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (صحيح)((متن + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
		إذا (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    إذا (len >= 0 && (صحيح)(ptr - متن) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    وإلا
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(صحيح)((متن + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		وإلا
		    prev_c = u8c;
#جيڪڏهن_پورو
		إذا (کانو + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_ايندڙ;
	force_redraw_ايندڙ = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	صحيح cell_attr = attr;
	إذا (screen_pum_blend > 0 && pum_bg_attrs != عدم
		&& صف >= pum_bg_چوٽي && صف < pum_bg_bot
		&& کانو < pum_bg_کانن_ڳڻپ)
	{
	    صحيح soff = (صف - pum_bg_چوٽي) * pum_bg_کانن_ڳڻپ + کانو;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_سٽون[off] != c
		|| (mbyte_cells == 2
		    && Screen_سٽون[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_سٽون_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_سٽون_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_سرگرم
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(صف, کانو)
#جيڪڏهن_پورو
		;

	إذا ((need_redraw || force_redraw_this) && !skip_for_popup(صف, کانو))
	{
#إذا defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    إذا (need_redraw && Screen_سٽون[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.اندريون_use
# endif
# إذا defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    رڪن_is_xterm
# endif
		    ))
	    {
		صحيح	n = ScreenAttrs[off];

		إذا (n > HL_ALL)
		    n = syn_attr2attr(n);
		إذا (n & HL_BOLD)
		    force_redraw_ايندڙ = TRUE;
	    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    إذا (gui.directx_enabled && need_redraw)
		force_redraw_ايندڙ = TRUE;
#جيڪڏهن_پورو
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    إذا (clear_ايندڙ_cell)
		clear_ايندڙ_cell = FALSE;
	    وإلا إذا (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= متن + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_ايندڙ_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    إذا (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_سٽون[off + mbyte_blen] = 0;
	    Screen_سٽون[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    Screen_کانن_ڳڻپ[off] = -1;
	    إذا (enc_utf8)
	    {
		إذا (c < 0x80 && u8cc[0] == 0)
		    Screen_سٽون_UC[off] = 0;
		وإلا
		{
		    صحيح	    i;

		    Screen_سٽون_UC[off] = u8c;
		    لكل (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سٽون_C[i][off] = u8cc[i];
			إذا (u8cc[i] == 0)
			    اكسر;
		    }
		}
		إذا (mbyte_cells == 2)
		{
		    Screen_سٽون[off + 1] = 0;
		    Screen_سٽون_UC[off + 1] = 0;
		    لكل (صحيح ci = 0; ci < Screen_mco; ++ci)
			Screen_سٽون_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    Screen_کانن_ڳڻپ[off + 1] = -1;
		}
		screen_char(off, صف, کانو);
	    }
	    وإلا إذا (mbyte_cells == 2)
	    {
		Screen_سٽون[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		Screen_کانن_ڳڻپ[off + 1] = -1;
		screen_char_2(off, صف, کانو);
	    }
	    وإلا إذا (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, صف, کانو);
	    }
	    وإلا
		screen_char(off, صف, کانو);
	}
	إذا (has_mbyte)
	{
	    off += mbyte_cells;
	    کانو += mbyte_cells;
	    ptr += mbyte_blen;
	    إذا (clear_ايندڙ_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	وإلا
	{
	    ++off;
	    ++کانو;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    إذا (force_redraw_ايندڙ && کانو < screen_Columns && !skip_for_popup(صف, کانو))
    {
	إذا (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, صف, کانو);
	وإلا
	    screen_char(off, صف, کانو);
    }
}

#إذا defined(FEAT_ڳولا_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    فراغ
شروعات_ڳولا_hl(فراغ)
{
    إذا (!p_hls || no_hlsearch)
	أرجع;

    پڇاڙي_ڳولا_hl();  // just in case it wasn't called before
    آخري_pat_prog(&screen_ڳولا_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    فراغ
پڇاڙي_ڳولا_hl(فراغ)
{
    إذا (screen_ڳولا_hl.rm.regprog == عدم)
	أرجع;

    vim_regfree(screen_ڳولا_hl.rm.regprog);
    screen_ڳولا_hl.rm.regprog = عدم;
}
#جيڪڏهن_پورو

    ساكن فراغ
screen_شروعات_highlight(صحيح attr)
{
    attrentry_T *aep = عدم;

    screen_attr = attr;
    إذا (!ڀريل_screen
#جيڪڏهن_وصف_ٿيل MSWIN
	    || !termcap_سرگرم
#جيڪڏهن_پورو
       )
	أرجع;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
    {
	حرف	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	ٻاهريون_STR(buf);
	أرجع;
    }
#جيڪڏهن_پورو

    إذا (attr > HL_ALL)				// special HL attr.
    {
	إذا (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	وإلا
	    aep = syn_رڪن_attr2entry(attr);
	إذا (aep == عدم)	    // did ":syntax clear"
	    attr = 0;
	وإلا
	    attr = aep->ae_attr;
    }
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    إذا (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	صحيح	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	إذا (p_tgc)
	{
	    إذا (aep == عدم || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		رڪن_fg_rgb_color(defguifg);
	    إذا (aep == عدم || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		رڪن_bg_rgb_color(defguibg);
	}
	وإلا إذا (t_colors >= 256)
	{
	    إذا (aep == عدم || aep->ae_u.cterm.fg_color == 0)
		رڪن_fg_color(defctermfg);
	    إذا (aep == عدم || aep->ae_u.cterm.bg_color == 0)
		رڪن_bg_color(defctermbg);
	}
    }
#جيڪڏهن_پورو
    إذا ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	ٻاهريون_str(T_MD);
    وإلا إذا (aep != عدم && cterm_عام_fg_bold && (
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#جيڪڏهن_پورو
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	ٻاهريون_str(T_ME);
    إذا ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	ٻاهريون_str(T_SO);
    إذا ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	ٻاهريون_str(T_UCS);
    إذا ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	ٻاهريون_str(T_USS);
    إذا ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	ٻاهريون_str(T_DS);
    إذا ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	ٻاهريون_str(T_CDS);
    إذا (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	ٻاهريون_str(T_US);
    إذا ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	ٻاهريون_str(T_CZH);
    إذا ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	ٻاهريون_str(T_MR);
    إذا ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	ٻاهريون_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    إذا (aep != عدم)
    {
	إذا (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		رڪن_font(aep->ae_u.cterm.font);
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	إذا (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    إذا (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		رڪن_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	وإلا
#جيڪڏهن_پورو
	    إذا (t_colors > 1)
	    {
		إذا (aep->ae_u.cterm.fg_color)
		    رڪن_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
	إذا (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    إذا (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		رڪن_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	وإلا
#جيڪڏهن_پورو
	    إذا (t_colors > 1)
	    {
		إذا (aep->ae_u.cterm.bg_color)
		    رڪن_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
	إذا (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    إذا (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		رڪن_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	وإلا
#جيڪڏهن_پورو
	    إذا (t_colors > 1)
	    {
		إذا (aep->ae_u.cterm.ul_color)
		    رڪن_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	إذا (!IS_CTERM)
	{
	    إذا (aep->ae_u.رڪن.شروعات != عدم)
		ٻاهريون_str(aep->ae_u.رڪن.شروعات);
	}
    }
}

    فراغ
screen_stop_highlight(فراغ)
{
    صحيح	    do_ME = FALSE;	    // output T_ME code
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    صحيح	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#جيڪڏهن_پورو

    إذا (screen_attr != 0
#جيڪڏهن_وصف_ٿيل MSWIN
			&& termcap_سرگرم
#جيڪڏهن_پورو
					   )
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	{
	    حرف	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    ٻاهريون_STR(buf);
	}
	وإلا
#جيڪڏهن_پورو
	{
	    صحيح is_under;

	    إذا (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		إذا (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    إذا (aep != عدم && ((
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#جيڪڏهن_پورو
				aep->ae_u.cterm.fg_color) || (
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#جيڪڏهن_پورو
				aep->ae_u.cterm.bg_color)))
			do_ME = TRUE;
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    إذا (use_vtp())
		    {
			إذا (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			إذا (!do_ME && do_ME_fg)
			    ٻاهريون_str((char_u *)"\033|39m"); // restore FG
			إذا (!do_ME && do_ME_bg)
			    ٻاهريون_str((char_u *)"\033|49m"); // restore BG
		    }
		    وإلا
		    {
			// Process FG and BG at once.
			إذا (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#جيڪڏهن_پورو
		}
		وإلا
		{
		    aep = syn_رڪن_attr2entry(screen_attr);
		    إذا (aep != عدم && aep->ae_u.رڪن.stop != عدم)
		    {
			إذا (STRCMP(aep->ae_u.رڪن.stop, T_ME) == 0)
			    do_ME = TRUE;
			وإلا
			    ٻاهريون_str(aep->ae_u.رڪن.stop);
		    }
		}
		إذا (aep == عدم)	    // did ":syntax clear"
		    screen_attr = 0;
		وإلا
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    إذا (screen_attr & HL_STANDOUT)
	    {
		إذا (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		وإلا
		    ٻاهريون_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    إذا (is_under && *T_UCE != NUL)
	    {
		إذا (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		وإلا
		    ٻاهريون_str(T_UCE);
	    }
	    إذا ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		إذا (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		وإلا
		    ٻاهريون_str(T_UE);
	    }
	    إذا (screen_attr & HL_ITALIC)
	    {
		إذا (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		وإلا
		    ٻاهريون_str(T_CZR);
	    }
	    إذا (screen_attr & HL_STRIKETHROUGH)
	    {
		إذا (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		وإلا
		    ٻاهريون_str(T_STE);
	    }
	    إذا (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		ٻاهريون_str(T_ME);

#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
	    إذا (p_tgc)
	    {
		إذا (cterm_عام_fg_gui_color != INVALCOLOR)
		    رڪن_fg_rgb_color(cterm_عام_fg_gui_color);
		إذا (cterm_عام_bg_gui_color != INVALCOLOR)
		    رڪن_bg_rgb_color(cterm_عام_bg_gui_color);
		إذا (cterm_عام_ul_gui_color != INVALCOLOR)
		    رڪن_ul_rgb_color(cterm_عام_ul_gui_color);
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		إذا (t_colors > 1)
		{
		    // set Normal cterm colors
		    إذا (cterm_عام_fg_color != 0)
			رڪن_fg_color(cterm_عام_fg_color - 1);
		    إذا (cterm_عام_bg_color != 0)
			رڪن_bg_color(cterm_عام_bg_color - 1);
		    إذا (cterm_عام_ul_color != 0)
			رڪن_ul_color(cterm_عام_ul_color - 1);
		    إذا (cterm_عام_fg_bold)
			ٻاهريون_str(T_MD);
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
    فراغ
reset_cterm_colors(فراغ)
{
    إذا (!IS_CTERM)
	أرجع;

    // set Normal cterm colors
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
    إذا (p_tgc ? (cterm_عام_fg_gui_color != INVALCOLOR
		|| cterm_عام_bg_gui_color != INVALCOLOR)
	    : (cterm_عام_fg_color > 0 || cterm_عام_bg_color > 0))
#وإلا
	إذا (cterm_عام_fg_color > 0 || cterm_عام_bg_color > 0)
#جيڪڏهن_پورو
	{
	    ٻاهريون_str(T_OP);
	    screen_attr = -1;
	}
    إذا (cterm_عام_fg_bold)
    {
	ٻاهريون_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    فراغ
screen_char(بنانشان off, صحيح صف, صحيح کانو)
{
    صحيح		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    إذا (صف >= screen_صفون || کانو >= screen_Columns)
	أرجع;

#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
    mark_dirty_region_for_images(صف, کانو, 1, 1);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    إذا (popup_is_under_opacity(صف, کانو))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	إذا (enc_utf8 && Screen_سٽون_UC[off] != 0
		&& utf_char2cells(Screen_سٽون_UC[off]) == 2
		&& کانو + 1 < screen_Columns
		&& !popup_is_under_opacity(صف, کانو + 1))
	{
	    صحيح off2 = off + 1;
	    Screen_سٽون[off2] = ' ';
	    Screen_سٽون_UC[off2] = 0;
	    screen_char(off2, صف, کانو + 1);
	}
	mark_suppressed_cell(صف, کانو);
	إذا (enc_utf8 && Screen_سٽون_UC[off] != 0
		&& utf_char2cells(Screen_سٽون_UC[off]) == 2
		&& کانو + 1 < screen_Columns
		&& popup_is_under_opacity(صف, کانو + 1))
	    mark_suppressed_cell(صف, کانو + 1);
	screen_cur_کانو = 9999;
	أرجع;
    }
    إذا (enc_utf8 && Screen_سٽون_UC[off] != 0
	    && utf_char2cells(Screen_سٽون_UC[off]) == 2
	    && کانو + 1 < screen_Columns
	    && popup_is_under_opacity(صف, کانو + 1))
    {
	mark_suppressed_cell(صف, کانو);
	mark_suppressed_cell(صف, کانو + 1);
	screen_cur_کانو = 9999;
	أرجع;
    }
#جيڪڏهن_پورو

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    إذا (*T_XN == NUL
	    && صف == screen_صفون - 1 && کانو == screen_Columns - 1
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#جيڪڏهن_پورو
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_کانن_ڳڻپ[off] = -1;
	أرجع;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    إذا (screen_char_attr != 0)
	attr = screen_char_attr;
    وإلا
	attr = ScreenAttrs[off];
    إذا (screen_attr != attr)
	screen_stop_highlight();

    windgoto(صف, کانو);

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(صف, کانو);
    إذا (enc_utf8 && Screen_سٽون_UC[off] != 0
	    && utf_char2cells(Screen_سٽون_UC[off]) == 2)
	unmark_suppressed_cell(صف, کانو + 1);
#جيڪڏهن_پورو

    إذا (screen_attr != attr)
	screen_شروعات_highlight(attr);

    إذا (enc_utf8 && Screen_سٽون_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	إذا (
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    !gui.اندريون_use &&
#جيڪڏهن_پورو
	    get_cellwidth(Screen_سٽون_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    ٻاهريون_str((char_u *)"  ");
	    رڪن_windgoto(صف, کانو);
	    screen_cur_کانو = 9999;
	}
	وإلا إذا (utf_ambiguous_ويڪر(Screen_سٽون_UC[off]))
	{
	    إذا (*p_ambw == 'd'
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		    && !gui.اندريون_use
#جيڪڏهن_پورو
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		ٻاهريون_str((char_u *)"  ");
		رڪن_windgoto(صف, کانو);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_کانو = 9999;
	}
	وإلا إذا (utf_char2cells(Screen_سٽون_UC[off]) > 1)
	    ++screen_cur_کانو;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	ٻاهريون_str(buf);
    }
    وإلا
    {
	ٻاهريون_flush_check();
	ٻاهريون_char(Screen_سٽون[off]);
	// double-byte character in single-width cell
	إذا (enc_dbcs == DBCS_JPNU && Screen_سٽون[off] == 0x8e)
	    ٻاهريون_char(ScreenLines2[off]);
    }

    screen_cur_کانو++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    ساكن فراغ
screen_char_2(بنانشان off, صحيح صف, صحيح کانو)
{
    // Check for illegal values (could be wrong when screen was resized).
    إذا (off + 1 >= (بنانشان)(screen_صفون * screen_Columns))
	أرجع;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    إذا (صف == screen_صفون - 1 && کانو >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_کانن_ڳڻپ[off] = -1;
	أرجع;
    }

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    إذا (popup_is_under_opacity(صف, کانو))
    {
	mark_suppressed_cell(صف, کانو);
	mark_suppressed_cell(صف, کانو + 1);
	screen_cur_کانو = 9999;
	أرجع;
    }
#جيڪڏهن_پورو

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, صف, کانو);
    ٻاهريون_char(Screen_سٽون[off + 1]);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    unmark_suppressed_cell(صف, کانو + 1);
#جيڪڏهن_پورو
    ++screen_cur_کانو;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    فراغ
screen_draw_rectangle(
    صحيح		صف,
    صحيح		کانو,
    صحيح		اوچائي,
    صحيح		ويڪر,
    صحيح		invert,
    صحيح		force)
{
    صحيح		r, c;
    صحيح		off;
    صحيح		max_off;

    // Can't use ScreenLines unless initialized
    إذا (Screen_سٽون == عدم)
	أرجع;

    إذا (invert)
	screen_char_attr = HL_INVERSE;
    لكل (r = صف; r < صف + اوچائي; ++r)
    {
	off = سٽ_وٿي[r];
	max_off = off + screen_Columns;
	لكل (c = کانو; c < کانو + ويڪر; ++c)
	{
	    إذا (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		إذا (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    وإلا
	    {
		إذا (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		إذا (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    ساكن فراغ
redraw_بلاڪ(صحيح صف, صحيح پڇاڙي, win_T *wp)
{
    صحيح		کانو;
    صحيح		ويڪر;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    clip_may_clear_selection(صف, پڇاڙي - 1);
#جيڪڏهن_پورو

    إذا (wp == عدم)
    {
	کانو = firstwin->w_wincol;
	ويڪر = topframe->fr_ويڪر;
    }
    وإلا
    {
	کانو = wp->w_wincol;
	ويڪر = wp->w_ويڪر;
    }
    screen_draw_rectangle(صف, کانو, پڇاڙي - صف, ويڪر, FALSE, FALSE);
}

    فراغ
space_منزل_screenline(صحيح off, صحيح attr)
{
    Screen_سٽون[off] = ' ';
    ScreenAttrs[off] = attr;
    Screen_کانن_ڳڻپ[off] = -1;
    إذا (enc_utf8)
	Screen_سٽون_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    فراغ
screen_fill(
	صحيح	شروعات_صف,
	صحيح	پڇاڙي_صف,
	صحيح	شروعات_کانو,
	صحيح	پڇاڙي_کانو,
	صحيح	c1,
	صحيح	c2,
	صحيح	attr)
{
    صحيح	    صف;
    صحيح	    کانو;
    صحيح	    off;
    صحيح	    پڇاڙي_off;
    صحيح	    did_delete;
    صحيح	    c;
    صحيح	    norm_رڪن;
#إذا defined(FEAT_GUI) || defined(UNIX)
    صحيح	    force_ايندڙ = FALSE;
#جيڪڏهن_پورو

    إذا (پڇاڙي_صف > screen_صفون)		// safety check
	پڇاڙي_صف = screen_صفون;
    إذا (پڇاڙي_کانو > screen_Columns)	// safety check
	پڇاڙي_کانو = screen_Columns;
    إذا (Screen_سٽون == عدم
	    || شروعات_صف < 0		// should not happen
	    || شروعات_صف >= پڇاڙي_صف
	    || شروعات_کانو >= پڇاڙي_کانو)	// nothing to do
	أرجع;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_رڪن = (
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    !gui.اندريون_use &&
#جيڪڏهن_پورو
	    !IS_CTERM);
    لكل (صف = شروعات_صف; صف < پڇاڙي_صف; ++صف)
    {
	إذا (has_mbyte
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		&& !gui.اندريون_use
#جيڪڏهن_پورو
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    إذا (شروعات_کانو > 0 && mb_fix_کانو(شروعات_کانو, صف) != شروعات_کانو)
	    {
		// start_col is the right half, clear the left half
		صحيح کاٻو_attr = ScreenAttrs[سٽ_وٿي[صف] + شروعات_کانو - 1];
		screen_puts_len((char_u *)" ", 1, صف, شروعات_کانو - 1, کاٻو_attr);
	    }
	    إذا (پڇاڙي_کانو < screen_Columns && mb_fix_کانو(پڇاڙي_کانو, صف) != پڇاڙي_کانو)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		صحيح ساڄو_attr = ScreenAttrs[سٽ_وٿي[صف] + پڇاڙي_کانو];
		screen_puts_len((char_u *)" ", 1, صف, پڇاڙي_کانو, ساڄو_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	إذا (c2 == ' '
		&& پڇاڙي_کانو == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_رڪن
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(صف,
						شروعات_کانو, پڇاڙي_کانو)
#جيڪڏهن_پورو
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    کانو = شروعات_کانو;
	    إذا (c1 != ' ')			// don't clear first char
		++کانو;

	    off = سٽ_وٿي[صف] + کانو;
	    پڇاڙي_off = سٽ_وٿي[صف] + پڇاڙي_کانو;

	    // skip blanks (used often, keep it fast!)
	    إذا (enc_utf8)
		طالما (off < پڇاڙي_off && Screen_سٽون[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_سٽون_UC[off] == 0
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			  && !is_suppressed_cell(صف,
					      (صحيح)(off - سٽ_وٿي[صف]))
#جيڪڏهن_پورو
			  )
		    ++off;
	    وإلا
		طالما (off < پڇاڙي_off && Screen_سٽون[off] == ' '
						     && ScreenAttrs[off] == 0
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			  && !is_suppressed_cell(صف,
					      (صحيح)(off - سٽ_وٿي[صف]))
#جيڪڏهن_پورو
			  )
		    ++off;
	    إذا (off < پڇاڙي_off)		// something to be cleared
	    {
		کانو = off - سٽ_وٿي[صف];
		screen_stop_highlight();
		رڪن_windgoto(صف, کانو);// clear rest of this screen line
		ٻاهريون_str(T_CE);
		screen_شروعات();		// don't know where cursor is now
		کانو = پڇاڙي_کانو - کانو;
		طالما (کانو--)		// clear chars in ScreenLines
		{
		    space_منزل_screenline(off, 0);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(صف, (صحيح)(off - سٽ_وٿي[صف]));
#جيڪڏهن_پورو
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = سٽ_وٿي[صف] + شروعات_کانو;
	c = c1;
	لكل (کانو = شروعات_کانو; کانو < پڇاڙي_کانو; ++کانو)
	{
	    إذا ((Screen_سٽون[off] != c
		    || (enc_utf8 && (صحيح)Screen_سٽون_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#إذا defined(FEAT_GUI) || defined(UNIX)
		    || force_ايندڙ
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(صف, کانو)
#جيڪڏهن_پورو
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(صف, کانو))
	    {
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		إذا (screen_opacity_popup != عدم && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    إذا (Screen_سٽون[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_سٽون_UC[off - 1] != 0
			    && utf_char2cells(Screen_سٽون_UC[off - 1]) == 2)
			اذهب ايندڙ_کانو;
		}
		إذا (screen_opacity_popup != عدم && c == ' '
						       && Screen_سٽون[off] != 0)
		{
		    صحيح bg_char_cells = 1;
		    إذا (enc_utf8 && Screen_سٽون_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_سٽون_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    إذا (bg_char_cells == 2)
		    {
			إذا (کانو + 1 >= پڇاڙي_کانو)
			    // At the edge, skip wide char.
			    اذهب skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			إذا (c2 != ' ')
			    اذهب skip_opacity_fill;
		    }

		    صحيح popup_attr = get_win_attr(screen_opacity_popup);
		    صحيح blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, صف, کانو);
		    اذهب ايندڙ_کانو;
		}
skip_opacity_fill:
#جيڪڏهن_پورو
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		إذا (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != عدم
			&& صف >= pum_bg_چوٽي && صف < pum_bg_bot
			&& کانو < pum_bg_کانن_ڳڻپ)
		{
		    صحيح soff = (صف - pum_bg_چوٽي) * pum_bg_کانن_ڳڻپ + کانو;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    إذا (enc_utf8 && pum_bg_سٽون_UC != عدم
			    && کانو > شروعات_کانو
			    && pum_bg_سٽون_UC[soff] == 0
			    && pum_bg_سٽون_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_سٽون_UC[soff - 1]) == 2)
		    {
			Screen_سٽون[off] = 0;
			إذا (Screen_سٽون_UC != عدم)
			    Screen_سٽون_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			اذهب ايندڙ_کانو;
		    }

		    صحيح underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    إذا (enc_utf8 && pum_bg_سٽون_UC != عدم
			    && ((pum_bg_سٽون_UC[soff] != 0
				    && utf_char2cells(pum_bg_سٽون_UC[soff]) == 2
				    && کانو + 1 >= پڇاڙي_کانو)
				|| (کانو == شروعات_کانو
				    && pum_bg_سٽون_UC[soff] == 0
				    && کانو > 0
				    && pum_bg_سٽون_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_سٽون_UC[soff - 1])
								       == 2)))
		    {
			Screen_سٽون[off] = ' ';
			إذا (Screen_سٽون_UC != عدم)
			    Screen_سٽون_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, صف, کانو);
			اذهب ايندڙ_کانو;
		    }

		    // Restore underlying character so text shows through.
		    Screen_سٽون[off] = pum_bg_سٽون[soff];
		    إذا (enc_utf8 && pum_bg_سٽون_UC != عدم
						&& Screen_سٽون_UC != عدم)
		    {
			صحيح k;
			Screen_سٽون_UC[off] = pum_bg_سٽون_UC[soff];
			لكل (k = 0; k < MAX_MCO; ++k)
			    إذا (pum_bg_سٽون_C[k] != عدم
						    && Screen_سٽون_C[k] != عدم)
				Screen_سٽون_C[k][off] = pum_bg_سٽون_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, صف, کانو);
		    اذهب ايندڙ_کانو;
		}
#إذا defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		إذا (
# ifdef FEAT_GUI
			gui.اندريون_use
# endif
# إذا defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			رڪن_is_xterm
# endif
		   )
		{
		    إذا (Screen_سٽون[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_ايندڙ = TRUE;
		    وإلا
			force_ايندڙ = FALSE;
		}
#جيڪڏهن_پورو // FEAT_GUI || defined(UNIX)
#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		إذا (gui.directx_enabled)
		    force_ايندڙ = TRUE;
#جيڪڏهن_پورو
		Screen_سٽون[off] = c;
		إذا (enc_utf8)
		{
		    إذا (c >= 0x80)
		    {
			Screen_سٽون_UC[off] = c;
			Screen_سٽون_C[0][off] = 0;
			Screen_سٽون[off] = 0x80; // avoid storing zero
		    }
		    وإلا
			Screen_سٽون_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		إذا (!did_delete || c != ' ')
		    screen_char(off, صف, کانو);
	    }
ايندڙ_کانو:
	    Screen_کانن_ڳڻپ[off] = -1;
	    ++off;
	    إذا (کانو == شروعات_کانو)
	    {
		إذا (did_delete)
		    اكسر;
		c = c2;
	    }
	}
	إذا (پڇاڙي_کانو == Columns)
	    سٽ_Wraps[صف] = FALSE;
	إذا (صف == صفون - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    إذا (((شروعات_کانو == 0 && پڇاڙي_کانو == Columns)
			|| (شروعات_کانو == cmdline_کانو_off
			&& پڇاڙي_کانو == cmdline_کانو_off + cmdline_ويڪر))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#جيڪڏهن_پورو
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    إذا (شروعات_کانو == 0 || شروعات_کانو == cmdline_کانو_off)
		گهڻائي_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    فراغ
check_for_delay(صحيح check_نياپو_scroll)
{
    إذا ((emsg_on_display || (check_نياپو_scroll && نياپو_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !اندريون_assert_fails)
    {
	ٻاهريون_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	إذا (check_نياپو_scroll)
	    نياپو_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ساكن فراغ
clear_Tab_صفحو_Idxs(فراغ)
{
    صحيح		scol;

    لكل (scol = 0; scol < Columns; ++scol)
	Tab_صفحو_Idxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    صحيح
screen_valid(صحيح doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    أرجع (Screen_سٽون != عدم);
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
    فراغ
screenalloc(صحيح doclear)
{
    صحيح		    new_صف, old_صف;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    صحيح		    old_صفون_2;
#جيڪڏهن_پورو
    win_T	    *wp;
    صحيح		    outofmem = FALSE;
    صحيح		    len;
    schar_T	    *new_Screen_سٽون;
    u8char_T	    *new_Screen_سٽون_UC = عدم;
    u8char_T	    *new_Screen_سٽون_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = عدم;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_Screen_کانن_ڳڻپ;
    بنانشان	    *new_سٽ_وٿي;
    char_u	    *new_سٽ_Wraps;
    ننڍو	    *new_Tab_صفحو_Idxs;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    ننڍو	    *new_popup_نقاب;
    ننڍو	    *new_popup_نقاب_ايندڙ;
    حرف	    *new_popup_transparent;
#جيڪڏهن_پورو
    tabpage_T	    *tp;
    ساكن صحيح	    entered = FALSE;		// avoid recursiveness
    ساكن صحيح	    پورو_outofmem_نياپو = FALSE;	// did outofmem message
    صحيح		    retry_ڳڻپ = 0;
    صحيح		    مليو_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    إذا ((Screen_سٽون != عدم
		&& صفون == screen_صفون
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_سٽون_UC != عدم)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != عدم)
		&& p_mco == Screen_mco)
	    || صفون == 0
	    || Columns == 0
	    || (!ڀريل_screen && Screen_سٽون == عدم))
	أرجع;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    إذا (entered)
	أرجع;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_تالو_screen();  // be safe, put it here
#جيڪڏهن_پورو

    comp_کانو();		// recompute columns for shown command and ruler

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
    لكل (صحيح i = 0; i < AUCMD_WIN_ڳڻپ; ++i)
	إذا (aucmd_win[i].auc_win != عدم)
	    win_free_lsize(aucmd_win[i].auc_win);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندريون_TAB(tp, wp)
	    win_free_lsize(wp);
#جيڪڏهن_پورو

    new_Screen_سٽون = LALLOC_MULT(schar_T, (صفون + 1) * Columns);
    vim_memset(new_Screen_سٽون_C, 0, حجم(u8char_T *) * MAX_MCO);
    إذا (enc_utf8)
    {
	new_Screen_سٽون_UC = LALLOC_MULT(u8char_T, (صفون + 1) * Columns);
	لكل (صحيح i = 0; i < p_mco; ++i)
	    new_Screen_سٽون_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (صفون + 1) * Columns);
    }
    إذا (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (صفون + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (صفون + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_Screen_کانن_ڳڻپ = LALLOC_CLEAR_MULT(colnr_T, (صفون + 1) * Columns);
    new_سٽ_وٿي = LALLOC_MULT(بنانشان, صفون);
    new_سٽ_Wraps = LALLOC_MULT(char_u, صفون);
    new_Tab_صفحو_Idxs = LALLOC_MULT(ننڍو, Columns);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    new_popup_نقاب = LALLOC_MULT(ننڍو, صفون * Columns);
    new_popup_نقاب_ايندڙ = LALLOC_MULT(ننڍو, صفون * Columns);
    new_popup_transparent = LALLOC_MULT(حرف, صفون * Columns);
#جيڪڏهن_پورو

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (win_alloc_سٽون(wp) == FAIL)
	{
	    outofmem = TRUE;
	    اذهب give_مٿي;
	}
    }
    لكل (صحيح i = 0; i < AUCMD_WIN_ڳڻپ; ++i)
	إذا (aucmd_win[i].auc_win != عدم
		&& aucmd_win[i].auc_win->w_سٽون == عدم
		&& win_alloc_سٽون(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    اكسر;
	}
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	إذا (win_alloc_سٽون(wp) == FAIL)
	{
	    outofmem = TRUE;
	    اذهب give_مٿي;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندريون_TAB(tp, wp)
	    إذا (win_alloc_سٽون(wp) == FAIL)
	    {
		outofmem = TRUE;
		اذهب give_مٿي;
	    }
#جيڪڏهن_پورو

give_مٿي:
    مليو_null = FALSE;
    لكل (صحيح i = 0; i < p_mco; ++i)
	إذا (new_Screen_سٽون_C[i] == عدم)
	{
	    مليو_null = TRUE;
	    اكسر;
	}
    إذا (new_Screen_سٽون == عدم
	    || (enc_utf8 && (new_Screen_سٽون_UC == عدم || مليو_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == عدم)
	    || new_ScreenAttrs == عدم
	    || new_Screen_کانن_ڳڻپ == عدم
	    || new_سٽ_وٿي == عدم
	    || new_سٽ_Wraps == عدم
	    || new_Tab_صفحو_Idxs == عدم
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    || new_popup_نقاب == عدم
	    || new_popup_نقاب_ايندڙ == عدم
	    || new_popup_transparent == عدم
#جيڪڏهن_پورو
	    || outofmem)
    {
	إذا (Screen_سٽون != عدم || !پورو_outofmem_نياپو)
	{
	    // guess the size
	    do_outofmem_نياپو((long_u)((صفون + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    پورو_outofmem_نياپو = TRUE;
	}
	VIM_CLEAR(new_Screen_سٽون);
	VIM_CLEAR(new_Screen_سٽون_UC);
	لكل (صحيح i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_سٽون_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_Screen_کانن_ڳڻپ);
	VIM_CLEAR(new_سٽ_وٿي);
	VIM_CLEAR(new_سٽ_Wraps);
	VIM_CLEAR(new_Tab_صفحو_Idxs);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_نقاب);
	VIM_CLEAR(new_popup_نقاب_ايندڙ);
	VIM_CLEAR(new_popup_transparent);
#جيڪڏهن_پورو
    }
    وإلا
    {
	پورو_outofmem_نياپو = FALSE;

	لكل (new_صف = 0; new_صف < صفون; ++new_صف)
	{
	    new_سٽ_وٿي[new_صف] = new_صف * Columns;
	    new_سٽ_Wraps[new_صف] = FALSE;

	    (فراغ)vim_memset(new_Screen_سٽون + new_صف * Columns,
				  ' ', (size_t)Columns * حجم(schar_T));
	    إذا (enc_utf8)
	    {
		(فراغ)vim_memset(new_Screen_سٽون_UC + new_صف * Columns,
				   0, (size_t)Columns * حجم(u8char_T));
		لكل (صحيح i = 0; i < p_mco; ++i)
		    (فراغ)vim_memset(new_Screen_سٽون_C[i]
						      + new_صف * Columns,
				   0, (size_t)Columns * حجم(u8char_T));
	    }
	    إذا (enc_dbcs == DBCS_JPNU)
		(فراغ)vim_memset(new_ScreenLines2 + new_صف * Columns,
				   0, (size_t)Columns * حجم(schar_T));
	    (فراغ)vim_memset(new_ScreenAttrs + new_صف * Columns,
				    0, (size_t)Columns * حجم(sattr_T));
	    (فراغ)vim_memset(new_Screen_کانن_ڳڻپ + new_صف * Columns,
				    0, (size_t)Columns * حجم(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    إذا (!doclear)
	    {
		old_صف = new_صف + (screen_صفون - صفون);
		إذا (old_صف >= 0 && Screen_سٽون != عدم)
		{
		    إذا (screen_Columns < Columns)
			len = screen_Columns;
		    وإلا
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    إذا (!(enc_utf8 && Screen_سٽون_UC == عدم)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_سٽون + new_سٽ_وٿي[new_صف],
				Screen_سٽون + سٽ_وٿي[old_صف],
				(size_t)len * حجم(schar_T));
		    إذا (enc_utf8 && Screen_سٽون_UC != عدم
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_سٽون_UC + new_سٽ_وٿي[new_صف],
				Screen_سٽون_UC + سٽ_وٿي[old_صف],
				(size_t)len * حجم(u8char_T));
			لكل (صحيح i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_سٽون_C[i]
						    + new_سٽ_وٿي[new_صف],
				Screen_سٽون_C[i] + سٽ_وٿي[old_صف],
				(size_t)len * حجم(u8char_T));
		    }
		    إذا (enc_dbcs == DBCS_JPNU && ScreenLines2 != عدم)
			mch_memmove(new_ScreenLines2 + new_سٽ_وٿي[new_صف],
				ScreenLines2 + سٽ_وٿي[old_صف],
				(size_t)len * حجم(schar_T));
		    mch_memmove(new_ScreenAttrs + new_سٽ_وٿي[new_صف],
			    ScreenAttrs + سٽ_وٿي[old_صف],
			    (size_t)len * حجم(sattr_T));
		    mch_memmove(new_Screen_کانن_ڳڻپ + new_سٽ_وٿي[new_صف],
			    Screen_کانن_ڳڻپ + سٽ_وٿي[old_صف],
			    (size_t)len * حجم(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_سٽ = new_Screen_سٽون + صفون * Columns;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	vim_memset(new_popup_نقاب, 0, صفون * Columns * حجم(ننڍو));
	vim_memset(new_popup_transparent, 0, صفون * Columns * حجم(حرف));
#جيڪڏهن_پورو
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_سٽون = new_Screen_سٽون;
    Screen_سٽون_UC = new_Screen_سٽون_UC;
    لكل (صحيح i = 0; i < p_mco; ++i)
	Screen_سٽون_C[i] = new_Screen_سٽون_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    Screen_کانن_ڳڻپ = new_Screen_کانن_ڳڻپ;
    سٽ_وٿي = new_سٽ_وٿي;
    سٽ_Wraps = new_سٽ_Wraps;
    Tab_صفحو_Idxs = new_Tab_صفحو_Idxs;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    popup_نقاب = new_popup_نقاب;
    popup_نقاب_ايندڙ = new_popup_نقاب_ايندڙ;
    popup_transparent = new_popup_transparent;
    popup_نقاب_refresh = TRUE;
#جيڪڏهن_پورو

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    old_صفون_2 = screen_صفون;
#جيڪڏهن_پورو
    screen_صفون = صفون;
    screen_Columns = Columns;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Cursor position may now be out of bounds after resize
    إذا (gui.اندريون_use && (gui.cursor_صف >= screen_صفون
		|| gui.cursor_کانو >= screen_Columns))
	gui.cursor_is_valid = false;
#جيڪڏهن_پورو

    سيٽ_must_redraw(UPD_CLEAR);	// need to clear the screen later
    إذا (doclear)
	screenclear2(TRUE);
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    وإلا إذا (gui.اندريون_use
	    && !gui.starting
	    && Screen_سٽون != عدم
	    && old_صفون_2 != صفون)
    {
	gui_redraw_بلاڪ(0, 0, (صحيح)صفون - 1, (صحيح)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	إذا (نياپو_صف >= صفون)		// Rows got smaller
	    نياپو_صف = صفون - 1;		// put cursor at last row
	وإلا إذا (صفون > old_صفون_2)	// Rows got bigger
	    نياپو_صف += صفون - old_صفون_2; // put cursor in same place
	إذا (نياپو_کانو >= cmdline_ويڪر)	// cmdline_width got smaller
	    نياپو_کانو = cmdline_ويڪر - 1;    // put cursor at last cmdline column
    }
#جيڪڏهن_پورو
    clear_Tab_صفحو_Idxs();

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_unlock_screen();
#جيڪڏهن_پورو

    entered = FALSE;
    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    إذا (starting == 0 && ++retry_ڳڻپ <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, عدم, عدم, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	اذهب retry;
    }
}

    فراغ
free_screenlines(فراغ)
{
    صحيح		i;

    VIM_CLEAR(Screen_سٽون_UC);
    لكل (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_سٽون_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_سٽون);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(Screen_کانن_ڳڻپ);
    VIM_CLEAR(سٽ_وٿي);
    VIM_CLEAR(سٽ_Wraps);
    VIM_CLEAR(Tab_صفحو_Idxs);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    VIM_CLEAR(popup_نقاب);
    VIM_CLEAR(popup_نقاب_ايندڙ);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_صفون = 0;
    suppressed_کانن_ڳڻپ = 0;
#جيڪڏهن_پورو
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    صحيح
screenclear(فراغ)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    أرجع screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    فراغ
redraw_as_cleared(فراغ)
{
    screenclear2(FALSE);
}

    ساكن صحيح
screenclear2(صحيح doclear)
{
    صحيح	    i;
    صحيح	    did_clear = FALSE;

    إذا (starting == NO_SCREEN || Screen_سٽون == عدم
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    || (gui.اندريون_use && gui.starting)
#جيڪڏهن_پورو
	    )
	أرجع FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (!gui.اندريون_use)
#جيڪڏهن_پورو
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#جيڪڏهن_پورو

    // blank out ScreenLines
    لكل (i = 0; i < صفون; ++i)
    {
	lineclear(سٽ_وٿي[i], (صحيح)Columns, 0);
	سٽ_Wraps[i] = FALSE;
    }

    إذا (doclear && can_clear(T_CL))
    {
	ٻاهريون_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	گهڻائي_displayed = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	إذا (suppressed_cells != عدم)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_صفون * suppressed_کانن_ڳڻپ);
#جيڪڏهن_پورو
    }
    وإلا
    {
	// can't clear the screen, mark all chars with invalid attributes
	لكل (i = 0; i < صفون; ++i)
	    lineinvalid(سٽ_وٿي[i], (صحيح)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جيڪڏهن_پورو
    إذا (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    نياپو_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#جيڪڏهن_پورو
    نياپو_صف = cmdline_صف;	// put cursor on last line for messages
    نياپو_کانو = 0;
    screen_شروعات();		// don't know where cursor is now
    نياپو_didany = FALSE;
    نياپو_didout = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
    سرڪايو_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#جيڪڏهن_پورو

    أرجع did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ساكن فراغ
lineclear(بنانشان off, صحيح ويڪر, صحيح attr)
{
    (فراغ)vim_memset(Screen_سٽون + off, ' ', (size_t)ويڪر * حجم(schar_T));
    إذا (enc_utf8)
	(فراغ)vim_memset(Screen_سٽون_UC + off, 0,
					  (size_t)ويڪر * حجم(u8char_T));
    (فراغ)vim_memset(ScreenAttrs + off, attr, (size_t)ويڪر * حجم(sattr_T));
    (فراغ)vim_memset(Screen_کانن_ڳڻپ + off, -1, (size_t)ويڪر * حجم(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ساكن فراغ
lineinvalid(بنانشان off, صحيح ويڪر)
{
    (فراغ)vim_memset(ScreenAttrs + off, -1, (size_t)ويڪر * حجم(sattr_T));
    (فراغ)vim_memset(Screen_کانن_ڳڻپ + off, -1, (size_t)ويڪر * حجم(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    فراغ
سٽ_was_clobbered(صحيح screen_lnum)
{
    lineinvalid(سٽ_وٿي[screen_lnum], (صحيح)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ساكن فراغ
linecopy(صحيح منزل, صحيح from, win_T *wp)
{
    بنانشان	off_منزل = سٽ_وٿي[منزل] + wp->w_wincol;
    بنانشان	off_from = سٽ_وٿي[from] + wp->w_wincol;

    mch_memmove(Screen_سٽون + off_منزل, Screen_سٽون + off_from,
	    wp->w_ويڪر * حجم(schar_T));
    إذا (enc_utf8)
    {
	صحيح	i;

	mch_memmove(Screen_سٽون_UC + off_منزل, Screen_سٽون_UC + off_from,
		wp->w_ويڪر * حجم(u8char_T));
	لكل (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_سٽون_C[i] + off_منزل, Screen_سٽون_C[i] + off_from,
		    wp->w_ويڪر * حجم(u8char_T));
    }
    إذا (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_منزل, ScreenLines2 + off_from,
		wp->w_ويڪر * حجم(schar_T));
    mch_memmove(ScreenAttrs + off_منزل, ScreenAttrs + off_from,
	    wp->w_ويڪر * حجم(sattr_T));
    mch_memmove(Screen_کانن_ڳڻپ + off_منزل, Screen_کانن_ڳڻپ + off_from,
	    wp->w_ويڪر * حجم(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    صحيح
can_clear(char_u *p)
{
    أرجع (*p != NUL && (t_colors <= 1
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		|| gui.اندريون_use
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_عام_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_عام_bg_color == 0)
#وإلا
		|| cterm_عام_bg_color == 0
#جيڪڏهن_پورو
		|| *T_UT != NUL)
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#جيڪڏهن_پورو
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    فراغ
screen_شروعات(فراغ)
{
    screen_cur_صف = screen_cur_کانو = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    فراغ
windgoto(صحيح صف, صحيح کانو)
{
    sattr_T	    *p;
    صحيح		    i;
    صحيح		    plan;
    صحيح		    لاڳت;
    صحيح		    wouldbe_کانو;
    صحيح		    noinvcurs;
    char_u	    *bs;
    صحيح		    goto_لاڳت;
    صحيح		    attr;

#وصف_ڪريو GOTO_لاڳت   7	// assume a term_windgoto() takes about 7 chars
#وصف_ڪريو HIGHL_لاڳت  5	// assume unhighlight takes 5 chars

#وصف_ڪريو PLAN_LE	    1
#وصف_ڪريو PLAN_CR	    2
#وصف_ڪريو PLAN_NL	    3
#وصف_ڪريو PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    إذا (Screen_سٽون == عدم)
	أرجع;
    إذا (کانو == screen_cur_کانو && صف == screen_cur_صف)
	أرجع;

    // Check for valid position.
    إذا (صف < 0)	// window without text lines?
	صف = 0;
    إذا (صف >= screen_صفون)
	صف = screen_صفون - 1;
    إذا (کانو >= screen_Columns)
	کانو = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    إذا (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_لاڳت;
    وإلا
	noinvcurs = 0;
    goto_لاڳت = GOTO_لاڳت + noinvcurs;

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
    إذا (صف >= screen_cur_صف && screen_cur_کانو < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = عدم;			    // init for GCC
	attr = screen_attr;
	إذا (صف == screen_cur_صف && کانو < screen_cur_کانو)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    إذا (*T_LE)
		bs = T_LE;		    // "cursor left"
	    وإلا
		bs = T_BC;		    // "backspace character (old)
	    إذا (*bs)
		لاڳت = (screen_cur_کانو - کانو) * (صحيح)STRLEN(bs);
	    وإلا
		لاڳت = 999;
	    إذا (کانو + 1 < لاڳت)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_کانو = 0;
		لاڳت = 1;		    // CR is just one character
	    }
	    وإلا
	    {
		plan = PLAN_LE;
		wouldbe_کانو = کانو;
	    }
	    إذا (noinvcurs)		    // will stop highlighting
	    {
		لاڳت += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	وإلا إذا (صف > screen_cur_صف)
	{
	    plan = PLAN_NL;
	    wouldbe_کانو = 0;
	    لاڳت = (صف - screen_cur_صف) * 2;  // CR LF
	    إذا (noinvcurs)		    // will stop highlighting
	    {
		لاڳت += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	وإلا
	{
	    plan = PLAN_WRITE;
	    wouldbe_کانو = screen_cur_کانو;
	    لاڳت = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = کانو - wouldbe_کانو;
	إذا (i > 0)
	    لاڳت += i;
	إذا (لاڳت < goto_لاڳت && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + سٽ_وٿي[صف] + wouldbe_کانو;
	    طالما (i && *p++ == attr)
		--i;
	    إذا (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		إذا (*--p == 0)
		{
		    لاڳت += noinvcurs;
		    طالما (i && *p++ == 0)
			--i;
		}
		إذا (i != 0)
		    لاڳت = 999;	// different attributes, don't do it
	    }
	    إذا (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		لكل (i = wouldbe_کانو; i < کانو; ++i)
		    إذا (Screen_سٽون_UC[سٽ_وٿي[صف] + i] != 0)
		    {
			لاڳت = 999;
			اكسر;
		    }
	    }
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    إذا (لاڳت < 999)
		لكل (i = wouldbe_کانو; i < کانو; ++i)
		    إذا (popup_is_under_opacity(صف, i))
		    {
			لاڳت = 999;
			اكسر;
		    }
#جيڪڏهن_پورو
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	إذا (لاڳت < goto_لاڳت)
	{
	    إذا (plan == PLAN_LE)
	    {
		إذا (noinvcurs)
		    screen_stop_highlight();
		طالما (screen_cur_کانو > کانو)
		{
		    ٻاهريون_str(bs);
		    --screen_cur_کانو;
		}
	    }
	    وإلا إذا (plan == PLAN_CR)
	    {
		إذا (noinvcurs)
		    screen_stop_highlight();
		ٻاهريون_char('\r');
		screen_cur_کانو = 0;
	    }
	    وإلا إذا (plan == PLAN_NL)
	    {
		إذا (noinvcurs)
		    screen_stop_highlight();
		طالما (screen_cur_صف < صف)
		{
		    ٻاهريون_char('\n');
		    ++screen_cur_صف;
		}
		screen_cur_کانو = 0;
	    }

	    i = کانو - screen_cur_کانو;
	    إذا (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		إذا (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    طالما (i-- > 0)
			ٻاهريون_char(*T_ND);
		}
		وإلا
		{
		    صحيح	off;

		    off = سٽ_وٿي[صف] + screen_cur_کانو;
		    طالما (i-- > 0)
		    {
			إذا (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			ٻاهريون_flush_check();
			ٻاهريون_char(Screen_سٽون[off]);
			إذا (enc_dbcs == DBCS_JPNU
				&& Screen_سٽون[off] == 0x8e)
			    ٻاهريون_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    وإلا
	لاڳت = 999;

    إذا (لاڳت >= goto_لاڳت)
    {
	إذا (noinvcurs)
	    screen_stop_highlight();
	إذا (صف == screen_cur_صف && (کانو > screen_cur_کانو)
		&& *T_CRI != NUL)
	    رڪن_cursor_ساڄو(کانو - screen_cur_کانو);
	وإلا
	    رڪن_windgoto(صف, کانو);
    }
    screen_cur_صف = صف;
    screen_cur_کانو = کانو;
}

/*
 * Set cursor to its position in the current window.
 */
    فراغ
setcursor(فراغ)
{
    setcursor_mayforce(FALSE);
}

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    صحيح
cursor_screen_cells(فراغ)
{
    أرجع has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#جيڪڏهن_پورو

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    فراغ
setcursor_mayforce(صحيح force)
{
    إذا (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((صحيح)curwin->w_ويڪر - curwin->w_wcol
					    - cursor_screen_cells()) :
#جيڪڏهن_پورو
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
    صحيح
win_ins_سٽون(
    win_T	*wp,
    صحيح		صف,
    صحيح		سٽ_ڳڻپ,
    صحيح		invalid,
    صحيح		mayclear)
{
    صحيح		did_delete;
    صحيح		nextrow;
    صحيح		lastrow;
    صحيح		retval;

    إذا (invalid)
	wp->w_سٽون_valid = 0;

    // with only a few lines it's not worth the effort
    إذا (wp->w_اوچائي < 5)
	أرجع FAIL;

    // with the popup menu visible this might not work correctly
    إذا (pum_visible())
	أرجع FAIL;

    إذا (سٽ_ڳڻپ > wp->w_اوچائي - صف)
	سٽ_ڳڻپ = wp->w_اوچائي - صف;

    retval = win_do_سٽون(wp, صف, سٽ_ڳڻپ, mayclear, FALSE, 0);
    إذا (retval != MAYBE)
	أرجع retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    إذا (wp->w_ايندڙ != عدم || wp->w_حالت_اوچائي)
    {
	إذا (screen_del_سٽون(0, W_WINROW(wp) + wp->w_اوچائي - سٽ_ڳڻپ,
				  سٽ_ڳڻپ, (صحيح)صفون, FALSE, 0, عدم) == OK)
	    did_delete = TRUE;
	وإلا إذا (wp->w_ايندڙ)
	    أرجع FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    إذا (!did_delete)
    {
	wp->w_redr_حالت = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_اوچائي + wp->w_حالت_اوچائي;
	lastrow = nextrow + سٽ_ڳڻپ;
	إذا (lastrow > صفون)
	    lastrow = صفون;
	screen_fill(nextrow - سٽ_ڳڻپ, lastrow - سٽ_ڳڻپ,
		  wp->w_wincol, (صحيح)W_ENDCOL(wp), ' ', ' ', 0);
    }

    إذا (screen_ins_سٽون(0, W_WINROW(wp) + صف, سٽ_ڳڻپ, (صحيح)صفون, 0, عدم)
								      == FAIL)
    {
	// deletion will have messed up other windows
	إذا (did_delete)
	{
	    wp->w_redr_حالت = true;
	    win_rest_invalid(W_ايندڙ(wp));
	}
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    صحيح
win_del_سٽون(
    win_T	*wp,
    صحيح		صف,
    صحيح		سٽ_ڳڻپ,
    صحيح		invalid,
    صحيح		mayclear,
    صحيح		clear_attr)	    // for clearing lines
{
    صحيح		retval;

    إذا (invalid)
	wp->w_سٽون_valid = 0;

    إذا (سٽ_ڳڻپ > wp->w_اوچائي - صف)
	سٽ_ڳڻپ = wp->w_اوچائي - صف;

    retval = win_do_سٽون(wp, صف, سٽ_ڳڻپ, mayclear, TRUE, clear_attr);
    إذا (retval != MAYBE)
	أرجع retval;

    إذا (screen_del_سٽون(0, W_WINROW(wp) + صف, سٽ_ڳڻپ,
				   (صحيح)صفون, FALSE, clear_attr, عدم) == FAIL)
	أرجع FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    إذا (wp->w_ايندڙ || wp->w_حالت_اوچائي || cmdline_صف < صفون - 1)
    {
	إذا (screen_ins_سٽون(0, W_WINROW(wp) + wp->w_اوچائي - سٽ_ڳڻپ,
			      سٽ_ڳڻپ, (صحيح)صفون, clear_attr, عدم) == FAIL)
	{
	    wp->w_redr_حالت = true;
	    win_rest_invalid(wp->w_ايندڙ);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    وإلا
	redraw_cmdline = TRUE;
    أرجع OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ساكن صحيح
win_do_سٽون(
    win_T	*wp,
    صحيح		صف,
    صحيح		سٽ_ڳڻپ,
    صحيح		mayclear,
    صحيح		del,
    صحيح		clear_attr)
{
    صحيح		retval;

    إذا (!redrawing() || سٽ_ڳڻپ <= 0)
	أرجع FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    إذا (no_win_do_سٽون_ins && !del)
	أرجع FAIL;

    // only a few lines left: redraw is faster
    إذا (mayclear && صفون - سٽ_ڳڻپ < 5
	    && wp->w_ويڪر == topframe->fr_ويڪر)
    {
	إذا (!no_win_do_سٽون_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	أرجع FAIL;
    }

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    إذا (popup_visible)
	أرجع FAIL;
#جيڪڏهن_پورو

    // Delete all remaining lines
    إذا (صف + سٽ_ڳڻپ >= wp->w_اوچائي)
    {
	screen_fill(W_WINROW(wp) + صف, W_WINROW(wp) + wp->w_اوچائي,
		wp->w_wincol, (صحيح)W_ENDCOL(wp), ' ', ' ', 0);
	أرجع OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    إذا (!no_win_do_سٽون_ins)
	clear_cmdline = TRUE;

#إذا defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    إذا (tabpanel_ويڪر() > 0)
	أرجع FAIL;
#جيڪڏهن_پورو

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    إذا (scroll_region || wp->w_ويڪر != topframe->fr_ويڪر)
    {
	إذا (scroll_region && (wp->w_ويڪر == topframe->fr_ويڪر
		    || *T_CSV != NUL))
	    scroll_region_سيٽ(wp, صف);
	إذا (del)
	    retval = screen_del_سٽون(W_WINROW(wp) + صف, 0, سٽ_ڳڻپ,
				    wp->w_اوچائي - صف, FALSE, clear_attr, wp);
	وإلا
	    retval = screen_ins_سٽون(W_WINROW(wp) + صف, 0, سٽ_ڳڻپ,
					   wp->w_اوچائي - صف, clear_attr, wp);
	إذا (scroll_region && (wp->w_ويڪر == topframe->fr_ويڪر
		    || *T_CSV != NUL))
	    scroll_region_reset();
	أرجع retval;
    }

    إذا (wp->w_ايندڙ != عدم && p_tf) // don't delete/insert on fast terminal
	أرجع FAIL;

    أرجع MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ساكن فراغ
win_rest_invalid(win_T *wp)
{
    طالما (wp != عدم)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_حالت = true;
	wp = wp->w_ايندڙ;
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
#وصف_ڪريو USE_T_CAL   1
#وصف_ڪريو USE_T_CDL   2
#وصف_ڪريو USE_T_AL    3
#وصف_ڪريو USE_T_CE    4
#وصف_ڪريو USE_T_DL    5
#وصف_ڪريو USE_T_SR    6
#وصف_ڪريو USE_NL	    7
#وصف_ڪريو USE_T_CD    8
#وصف_ڪريو USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    صحيح
screen_ins_سٽون(
    صحيح		off,
    صحيح		صف,
    صحيح		سٽ_ڳڻپ,
    صحيح		پڇاڙي,
    صحيح		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    صحيح		i;
    صحيح		j;
    بنانشان	عارضي;
    صحيح		cursor_صف;
    صحيح		cursor_کانو = 0;
    صحيح		type;
    صحيح		نتيجو_خالي_جايون;
    صحيح		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    إذا (!screen_valid(TRUE)
	     || سٽ_ڳڻپ <= 0 || سٽ_ڳڻپ > p_ttyscroll
	     || پڇاڙي > صفون
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	     || (clip_تارو.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#جيڪڏهن_پورو
	     )
	أرجع FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	سرڪايو_image_placements(off + صف, off + پڇاڙي, سٽ_ڳڻپ);
# endif
	أرجع FAIL;
    }
#جيڪڏهن_پورو

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
    نتيجو_خالي_جايون = (صف + سٽ_ڳڻپ >= پڇاڙي);
    إذا (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	إذا (سٽ_ڳڻپ > 3)
	    أرجع FAIL;
	type = USE_REDRAW;
    }
    وإلا إذا (can_clear(T_CD) && نتيجو_خالي_جايون)
	type = USE_T_CD;
    وإلا إذا (*T_CAL != NUL && (سٽ_ڳڻپ > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    وإلا إذا (*T_CDL != NUL && نتيجو_خالي_جايون && (سٽ_ڳڻپ > 1 || !can_ce))
	type = USE_T_CDL;
    وإلا إذا (*T_AL != NUL)
	type = USE_T_AL;
    وإلا إذا (can_ce && نتيجو_خالي_جايون)
	type = USE_T_CE;
    وإلا إذا (*T_DL != NUL && نتيجو_خالي_جايون)
	type = USE_T_DL;
    وإلا إذا (*T_SR != NUL && صف == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    وإلا
	أرجع FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    إذا (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	أرجع screen_del_سٽون(off, صف, سٽ_ڳڻپ, پڇاڙي, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    إذا (*T_DB)
	screen_del_سٽون(off, پڇاڙي - سٽ_ڳڻپ, سٽ_ڳڻپ, پڇاڙي, FALSE, 0, wp);

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    إذا (off + صف > 0 || (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر))
	clip_clear_selection(&clip_تارو);
    وإلا
	clip_scroll_selection(-سٽ_ڳڻپ);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
    سرڪايو_image_placements(off + صف, off + پڇاڙي, سٽ_ڳڻپ);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_تالو_screen();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(صف + off <= gui.cursor_صف);
#جيڪڏهن_پورو

    إذا (wp != عدم && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_کانو = wp->w_wincol;

    إذا (*T_CCS != NUL)	   // cursor relative to region
	cursor_صف = صف;
    وإلا
	cursor_صف = صف + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    صف += off;
    پڇاڙي += off;
    لكل (i = 0; i < سٽ_ڳڻپ; ++i)
    {
	إذا (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر)
	{
	    // need to copy part of a line
	    j = پڇاڙي - 1 - i;
	    طالما ((j -= سٽ_ڳڻپ) >= صف)
		linecopy(j + سٽ_ڳڻپ, j, wp);
	    j += سٽ_ڳڻپ;
	    إذا (can_clear((char_u *)" "))
		lineclear(سٽ_وٿي[j] + wp->w_wincol,
			wp->w_ويڪر, clear_attr);
	    وإلا
		lineinvalid(سٽ_وٿي[j] + wp->w_wincol, wp->w_ويڪر);
	    سٽ_Wraps[j] = FALSE;
	}
	وإلا
	{
	    j = پڇاڙي - 1 - i;
	    عارضي = سٽ_وٿي[j];
	    طالما ((j -= سٽ_ڳڻپ) >= صف)
	    {
		سٽ_وٿي[j + سٽ_ڳڻپ] = سٽ_وٿي[j];
		سٽ_Wraps[j + سٽ_ڳڻپ] = سٽ_Wraps[j];
	    }
	    سٽ_وٿي[j + سٽ_ڳڻپ] = عارضي;
	    سٽ_Wraps[j + سٽ_ڳڻپ] = FALSE;
	    إذا (can_clear((char_u *)" "))
		lineclear(عارضي, (صحيح)Columns, clear_attr);
	    وإلا
		lineinvalid(عارضي, (صحيح)Columns);
	}
    }

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_unlock_screen();
#جيڪڏهن_پورو

    screen_stop_highlight();
    windgoto(cursor_صف, cursor_کانو);
    إذا (clear_attr != 0)
	screen_شروعات_highlight(clear_attr);

    // redraw the characters
    إذا (type == USE_REDRAW)
	redraw_بلاڪ(صف, پڇاڙي, wp);
    وإلا إذا (type == USE_T_CAL)
    {
	رڪن_append_سٽون(سٽ_ڳڻپ);
	screen_شروعات();		// don't know where cursor is now
    }
    وإلا
    {
	لكل (i = 0; i < سٽ_ڳڻپ; i++)
	{
	    إذا (type == USE_T_AL)
	    {
		إذا (i && cursor_صف != 0)
		    windgoto(cursor_صف, cursor_کانو);
		ٻاهريون_str(T_AL);
	    }
	    وإلا  // type == USE_T_SR
		ٻاهريون_str(T_SR);
	    screen_شروعات();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    إذا (type == USE_T_SR && *T_DA)
    {
	لكل (i = 0; i < سٽ_ڳڻپ; ++i)
	{
	    windgoto(off + i, cursor_کانو);
	    ٻاهريون_str(T_CE);
	    screen_شروعات();	    // don't know where cursor is now
	}
    }

#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    gui_can_update_cursor();
    إذا (gui.اندريون_use)
	ٻاهريون_flush();	// always flush after a scroll
#جيڪڏهن_پورو
    أرجع OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    صحيح
screen_del_سٽون(
    صحيح		off,
    صحيح		صف,
    صحيح		سٽ_ڳڻپ,
    صحيح		پڇاڙي,
    صحيح		force,		// even when line_count > p_ttyscroll
    صحيح		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    صحيح		j;
    صحيح		i;
    بنانشان	عارضي;
    صحيح		cursor_صف;
    صحيح		cursor_کانو = 0;
    صحيح		cursor_پڇاڙي;
    صحيح		نتيجو_خالي_جايون;	// result is empty until end of region
    صحيح		can_delete;	// deleting line codes can be used
    صحيح		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    إذا (!screen_valid(TRUE)
	    || سٽ_ڳڻپ <= 0
	    || (!force && سٽ_ڳڻپ > p_ttyscroll)
	    || پڇاڙي > صفون
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	    || (clip_تارو.state != SELECT_CLEARED && redrawing_for_callback > 0)
#جيڪڏهن_پورو
       )
	أرجع FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    نتيجو_خالي_جايون = صف + سٽ_ڳڻپ >= پڇاڙي;

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
    إذا (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	إذا (سٽ_ڳڻپ > 3)
	    أرجع FAIL;
	type = USE_REDRAW;
    }
    وإلا إذا (can_clear(T_CD) && نتيجو_خالي_جايون)
	type = USE_T_CD;
    وإلا إذا (صف == 0 && (
#جيڪڏهن_وصف_نه_ٿيل AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    سٽ_ڳڻپ == 1 ||
#جيڪڏهن_پورو
						*T_CDL == NUL))
	type = USE_NL;
    وإلا إذا (*T_CDL != NUL && سٽ_ڳڻپ > 1 && can_delete)
	type = USE_T_CDL;
    وإلا إذا (can_clear(T_CE) && نتيجو_خالي_جايون
	    && (wp == عدم || wp->w_ويڪر == topframe->fr_ويڪر))
	type = USE_T_CE;
    وإلا إذا (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    وإلا إذا (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    وإلا
	أرجع FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    إذا (off + صف > 0 || (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر))
	clip_clear_selection(&clip_تارو);
    وإلا
	clip_scroll_selection(سٽ_ڳڻپ);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
    سرڪايو_image_placements(off + صف, off + پڇاڙي, -سٽ_ڳڻپ);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_تالو_screen();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_صف >= صف + off
						&& gui.cursor_صف < پڇاڙي + off);
#جيڪڏهن_پورو

    إذا (wp != عدم && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_کانو = wp->w_wincol;

    إذا (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_صف = صف;
	cursor_پڇاڙي = پڇاڙي;
    }
    وإلا
    {
	cursor_صف = صف + off;
	cursor_پڇاڙي = پڇاڙي + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    صف += off;
    پڇاڙي += off;
    لكل (i = 0; i < سٽ_ڳڻپ; ++i)
    {
	إذا (wp != عدم && wp->w_ويڪر != topframe->fr_ويڪر)
	{
	    // need to copy part of a line
	    j = صف + i;
	    طالما ((j += سٽ_ڳڻپ) <= پڇاڙي - 1)
		linecopy(j - سٽ_ڳڻپ, j, wp);
	    j -= سٽ_ڳڻپ;
	    إذا (can_clear((char_u *)" "))
		lineclear(سٽ_وٿي[j] + wp->w_wincol,
			wp->w_ويڪر, clear_attr);
	    وإلا
		lineinvalid(سٽ_وٿي[j] + wp->w_wincol, wp->w_ويڪر);
	    سٽ_Wraps[j] = FALSE;
	}
	وإلا
	{
	    // whole width, moving the line pointers is faster
	    j = صف + i;
	    عارضي = سٽ_وٿي[j];
	    طالما ((j += سٽ_ڳڻپ) <= پڇاڙي - 1)
	    {
		سٽ_وٿي[j - سٽ_ڳڻپ] = سٽ_وٿي[j];
		سٽ_Wraps[j - سٽ_ڳڻپ] = سٽ_Wraps[j];
	    }
	    سٽ_وٿي[j - سٽ_ڳڻپ] = عارضي;
	    سٽ_Wraps[j - سٽ_ڳڻپ] = FALSE;
	    إذا (can_clear((char_u *)" "))
		lineclear(عارضي, (صحيح)Columns, clear_attr);
	    وإلا
		lineinvalid(عارضي, (صحيح)Columns);
	}
    }

#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    vim_unlock_screen();
#جيڪڏهن_پورو

    إذا (screen_attr != clear_attr)
	screen_stop_highlight();
    إذا (clear_attr != 0)
	screen_شروعات_highlight(clear_attr);

    // redraw the characters
    إذا (type == USE_REDRAW)
	redraw_بلاڪ(صف, پڇاڙي, wp);
    وإلا إذا (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_صف, cursor_کانو);
	ٻاهريون_str(T_CD);
	screen_شروعات();			// don't know where cursor is now
    }
    وإلا إذا (type == USE_T_CDL)
    {
	windgoto(cursor_صف, cursor_کانو);
	رڪن_delete_سٽون(سٽ_ڳڻپ);
	screen_شروعات();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    وإلا إذا (type == USE_NL)
    {
	windgoto(cursor_پڇاڙي - 1, cursor_کانو);
	لكل (i = سٽ_ڳڻپ; --i >= 0; )
	    ٻاهريون_char('\n');		// cursor will remain on same line
    }
    وإلا
    {
	لكل (i = سٽ_ڳڻپ; --i >= 0; )
	{
	    إذا (type == USE_T_DL)
	    {
		windgoto(cursor_صف, cursor_کانو);
		ٻاهريون_str(T_DL);		// delete a line
	    }
	    وإلا // type == USE_T_CE
	    {
		windgoto(cursor_صف + i, cursor_کانو);
		ٻاهريون_str(T_CE);		// erase a line
	    }
	    screen_شروعات();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    إذا (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	لكل (i = سٽ_ڳڻپ; i > 0; --i)
	{
	    windgoto(cursor_پڇاڙي - i, cursor_کانو);
	    ٻاهريون_str(T_CE);		// erase a line
	    screen_شروعات();		// don't know where cursor is now
	}
    }

#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    gui_can_update_cursor();
    إذا (gui.اندريون_use)
	ٻاهريون_flush();	// always flush after a scroll
#جيڪڏهن_پورو

    أرجع OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    صحيح
skip_showmode(فراغ)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    إذا (global_busy
	    || نياپو_silent != 0
	    || !redrawing()
	    || ((!stuff_خالي_جايون() || typebuf.tb_len > 0 || using_script())
		&& !ڪنجي_Typed))
    {
	redraw_گهڻائي = TRUE;		// show mode later
	أرجع TRUE;
    }
    أرجع FALSE;
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
    صحيح
showmode(فراغ)
{
    صحيح		need_clear;
    صحيح		ڊيگهائي = 0;
    صحيح		do_گهڻائي;
    صحيح		attr;
    صحيح		nwr_save;
    صحيح		sub_attr;
    صحيح		show_ruler_with_pum = FALSE;

    do_گهڻائي = p_smd && نياپو_silent == 0
	    && ((State & گهڻائي_وجهو)
		|| restart_edit != NUL
		|| VIsual_سرگرم);
    إذا (do_گهڻائي || reg_recording != 0)
    {
	إذا (skip_showmode())
	    أرجع 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	إذا (clear_cmdline && cmdline_صف < صفون - 1)
	    نياپو_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	نياپو_pos_گهڻائي();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	إذا (do_گهڻائي)
	{
	    نياپو_puts_attr("--", attr);
#إذا defined(FEAT_XIM)
	    إذا (
# ifdef FEAT_GUI_GTK
		    preedit_get_حالت()
# وإلا
		    im_get_حالت()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		نياپو_puts_attr(" IM", attr);
# وإلا
		نياپو_puts_attr(" XIM", attr);
# endif
#جيڪڏهن_پورو
	    // CTRL-X in Insert mode
	    إذا (edit_submode != عدم && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		ڊيگهائي = (صفون - نياپو_صف) * cmdline_ويڪر - 3;
		إذا (edit_submode_extra != عدم)
		    ڊيگهائي -= vim_strsize(edit_submode_extra);
		إذا (ڊيگهائي > 0)
		{
		    إذا (edit_submode_pre != عدم)
			ڊيگهائي -= vim_strsize(edit_submode_pre);
		    إذا (ڊيگهائي - vim_strsize(edit_submode) > 0)
		    {
			إذا (edit_submode_pre != عدم)
			    نياپو_puts_attr((حرف *)edit_submode_pre, attr);
			نياپو_puts_attr((حرف *)edit_submode, attr);
		    }
		    إذا (edit_submode_extra != عدم)
		    {
			نياپو_puts_attr(" ", attr);  // add a space in between
			إذا ((صحيح)edit_submode_highl < (صحيح)HLF_ڳڻپ)
			    sub_attr = HL_ATTR(edit_submode_highl);
			وإلا
			    sub_attr = attr;
			نياپو_puts_attr((حرف *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    وإلا
	    {
		إذا (State & VREPLACE_FLAG)
		    نياپو_puts_attr(_(" VREPLACE"), attr);
		وإلا إذا (State & REPLACE_FLAG)
		    نياپو_puts_attr(_(" REPLACE"), attr);
		وإلا إذا (State & گهڻائي_وجهو)
		{
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		    إذا (p_ri)
			نياپو_puts_attr(_(" REVERSE"), attr);
#جيڪڏهن_پورو
		    نياپو_puts_attr(_(" INSERT"), attr);
		}
		وإلا إذا (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    نياپو_puts_attr(_(" (insert)"), attr);
		وإلا إذا (restart_edit == 'R')
		    نياپو_puts_attr(_(" (replace)"), attr);
		وإلا إذا (restart_edit == 'V')
		    نياپو_puts_attr(_(" (vreplace)"), attr);
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
		إذا (p_hkmap)
		    نياپو_puts_attr(_(" Hebrew"), attr);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
		إذا (State & گهڻائي_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    إذا (curwin->w_p_arab)
			نياپو_puts_attr(_(" Arabic"), attr);
		    وإلا
# endif
			إذا (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    نياپو_puts_attr((حرف *)NameBuff, attr);
		}
#جيڪڏهن_پورو
		إذا ((State & گهڻائي_وجهو) && p_paste)
		    نياپو_puts_attr(_(" (paste)"), attr);

		إذا (VIsual_سرگرم)
		{
		    حرف *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    بدل ((VIsual_select ? 4 : 0)
			    + (VIsual_گهڻائي == Ctrl_V) * 2
			    + (VIsual_گهڻائي == 'V'))
		    {
			حالة 0:	p = N_(" VISUAL"); اكسر;
			حالة 1: p = N_(" VISUAL LINE"); اكسر;
			حالة 2: p = N_(" VISUAL BLOCK"); اكسر;
			حالة 4: p = N_(" SELECT"); اكسر;
			حالة 5: p = N_(" SELECT LINE"); اكسر;
			افتراضي: p = N_(" SELECT BLOCK"); اكسر;
		    }
		    نياپو_puts_attr(_(p), attr);
		}
		نياپو_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	إذا (reg_recording != 0
		&& edit_submode == عدم)    // otherwise it gets too long
	{
	    recording_گهڻائي(attr);
	    need_clear = TRUE;
	}

	گهڻائي_displayed = TRUE;
	إذا (need_clear || clear_cmdline || redraw_گهڻائي)
	    نياپو_clr_eos();
	نياپو_didout = FALSE;		// overwrite this message
	ڊيگهائي = نياپو_کانو;
	نياپو_کانو = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    وإلا إذا (clear_cmdline && نياپو_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	نياپو_clr_cmdline();
    وإلا إذا (redraw_گهڻائي)
    {
	نياپو_pos_گهڻائي();
	نياپو_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    إذا (VIsual_سرگرم)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    إذا (redrawing() && lastwin->w_حالت_اوچائي == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_گهڻائي = FALSE;
    clear_cmdline = FALSE;

    أرجع ڊيگهائي;
}

/*
 * Position for a mode message.
 */
    ساكن فراغ
نياپو_pos_گهڻائي(فراغ)
{
    نياپو_کانو = 0;
    نياپو_صف = صفون - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    فراغ
unshowmode(صحيح force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    إذا (!redrawing() || (!force && char_avail() && !ڪنجي_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    وإلا
	clearmode();
}

/*
 * Clear the mode message.
 */
    فراغ
clearmode(فراغ)
{
    صحيح save_نياپو_صف = نياپو_صف;
    صحيح save_نياپو_کانو = نياپو_کانو;

    نياپو_pos_گهڻائي();
    إذا (reg_recording != 0)
	recording_گهڻائي(HL_ATTR(HLF_CM));
    نياپو_clr_eos();

    نياپو_کانو = save_نياپو_کانو;
    نياپو_صف = save_نياپو_صف;
}

    ساكن فراغ
recording_گهڻائي(صحيح attr)
{
    نياپو_puts_attr(_("recording"), attr);
    إذا (shortmess(SHM_RECORDING))
	أرجع;

    حرف s[4];

    sprintf(s, " @%c", reg_recording);
    نياپو_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ساكن صحيح
mouse_has_any(فراغ)
{
    أرجع mouse_has(MOUSE_عام) || mouse_has(MOUSE_وجهو)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    فراغ
draw_tabline(فراغ)
{
    صحيح		tabcount = 0;
    tabpage_T	*tp;
    صحيح		tabwidth;
    صحيح		کانو = 0;
    صحيح		scol = 0;
    صحيح		attr;
    win_T	*wp;
    win_T	*cwp;
    صحيح		wincount;
    صحيح		modified;
    صحيح		c;
    صحيح		len;
    صحيح		attr_sel;
    صحيح		attr_nosel = 0;
    صحيح		attr_fill = 0;
    char_u	*p;
    صحيح		گنجائش;
    صحيح		use_sep_chars = (t_colors < 8
#جيڪڏهن_وصف_ٿيل FEAT_GUI
					    && !gui.اندريون_use
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
					    && !p_tgc
#جيڪڏهن_پورو
					    );
    bool	override_success;

#إذا defined(FEAT_TABPANEL)
    کانو = firstwin->w_wincol;
#جيڪڏهن_پورو

    إذا (Screen_سٽون == عدم)
	أرجع;
    redraw_tabline = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    إذا (gui_use_tabline())
    {
	gui_update_tabline();
	أرجع;
    }
#جيڪڏهن_پورو

    إذا (tabline_اوچائي() < 1)
	أرجع;

#إذا defined(FEAT_STL_OPT)
    clear_Tab_صفحو_Idxs();

    // Use the 'tabline' option if it's set.
    إذا (*p_tal != NUL)
	win_redr_custom(عدم, FALSE);
    وإلا
#جيڪڏهن_پورو
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_ويڪر - 1 + tabcount / 2) / tabcount;
	إذا (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	لكل (tp = پهريون_tabpage; tp != عدم && کانو < Columns - 4;
							     tp = tp->tp_ايندڙ)
	{
	    scol = کانو;

	    override_success = ڌڪو_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    إذا (tp->tp_topframe == topframe)
		attr = attr_sel;
	    إذا (use_sep_chars && کانو > 0)
		screen_putchar('|', 0, کانو++, attr);

	    إذا (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, کانو++, attr);

	    إذا (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    وإلا
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    لكل (wincount = 0; wp != عدم; wp = wp->w_ايندڙ, ++wincount)
		إذا (buf_Is_بدليو(wp->w_بفر))
		    modified = TRUE;
	    إذا (modified || wincount > 1)
	    {
		إذا (wincount > 1)
		{
		    len = vim_snprintf((حرف *)NameBuff, MAXPATHL, "%d", wincount);
		    إذا (کانو + len >= Columns - 3)
			اكسر;
		    screen_puts_len(NameBuff, len, 0, کانو,
#إذا defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#وإلا
					 attr
#جيڪڏهن_پورو
					       );
		    کانو += len;
		}
		إذا (modified)
		    screen_puts_len((char_u *)"+", 1, 0, کانو++, attr);
		screen_putchar(' ', 0, کانو++, attr);
	    }

	    گنجائش = scol - کانو + tabwidth - 1;
	    إذا (گنجائش > 0)
	    {
		صحيح n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_بفر);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		إذا (has_mbyte)
		    طالما (len > گنجائش)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		وإلا إذا (len > گنجائش)
		{
		    p += len - گنجائش;
		    len = گنجائش;
		}
		n = Columns - کانو - 1;
		إذا (len > n)
		    len = n;

		screen_puts_len(p, (صحيح)STRLEN(p), 0, کانو, attr);
		کانو += len;
	    }
	    screen_putchar(' ', 0, کانو++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    طالما (scol < کانو)
		Tab_صفحو_Idxs[scol++] = tabcount;

	    إذا (override_success)
		ڪڍو_highlight_overrides();
	}

	إذا (use_sep_chars)
	    c = '_';
	وإلا
	    c = ' ';
	screen_fill(0, 1, کانو, (صحيح)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	إذا (p_sc && *p_sloc == 't')
	{
	    صحيح	n = (صحيح)Columns - کانو - (tabcount > 1) * 3;
	    صحيح	ويڪر = MIN(10, n);

	    إذا (ويڪر > 0)
		screen_puts_len(showcmd_buf, ويڪر, 0, (صحيح)Columns
			    - ويڪر - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	إذا (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (صحيح)Columns - 1, attr_nosel);
	    Tab_صفحو_Idxs[Columns - 1] = -999;
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
    فراغ
get_trans_bufname(buf_T *buf)
{
    إذا (buf_spname(buf) != عدم)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    وإلا
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    صحيح
fillchar_حالت(صحيح *attr, win_T *wp)
{
    صحيح	    fill;
    bool    override_success =
	ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    إذا (bt_terminal(wp->w_بفر))
    {
	إذا (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	وإلا
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    وإلا
#جيڪڏهن_پورو
    إذا (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    وإلا
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    إذا (override_success)
	ڪڍو_highlight_overrides();
    أرجع fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ساكن bool
vsep_صف_is_curwin(win_T *wp, صحيح صف)
{
    إذا (wp == curwin)
	أرجع true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    إذا (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_ويڪر
	    && صف >= curwin->w_winrow
	    && صف < W_WINROW(curwin) + curwin->w_اوچائي
						   + curwin->w_حالت_اوچائي)
	أرجع true;

    أرجع false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    صحيح
fillchar_vsep(صحيح *attr, win_T *wp, صحيح صف)
{
    bool override_success =
	ڌڪو_highlight_overrides(wp->w_hl, wp->w_hl_len);
    إذا (vsep_صف_is_curwin(wp, صف))
	*attr = HL_ATTR(HLF_C);
    وإلا
	*attr = HL_ATTR(HLF_CNC);
    إذا (override_success)
	ڪڍو_highlight_overrides();

    إذا (*attr == 0 && wp->w_fill_chars.vert == ' ')
	أرجع '|';
    وإلا
	أرجع wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    صحيح
redrawing(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (disable_redraw_for_testing)
	أرجع 0;
    وإلا
#جيڪڏهن_پورو
	أرجع ((RedrawingDisabled == 0
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#جيڪڏهن_پورو
		) && !(p_lz && char_avail() && !ڪنجي_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    صحيح
messaging(فراغ)
{
    أرجع (!(p_lz && char_avail() && !ڪنجي_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#وصف_ڪريو کانو_RULER 17	    // columns needed by standard ruler

    فراغ
comp_کانو(فراغ)
{
    صحيح آخري_has_حالت = آخري_stl_اوچائي(FALSE) > 0;

    sc_کانو = 0;
    ru_کانو = 0;
    إذا (p_ru)
    {
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
	ru_کانو = (ru_wid ? ru_wid : کانو_RULER) + 1;
#وإلا
	ru_کانو = کانو_RULER + 1;
#جيڪڏهن_پورو
	// no last status line, adjust sc_col
	إذا (!آخري_has_حالت)
	    sc_کانو = ru_کانو;
    }
    إذا (p_sc && *p_sloc == 'l')
    {
	sc_کانو += SHOWCMD_کانن_ڳڻپ;
	إذا (!p_ru || آخري_has_حالت)	    // no need for separating space
	    ++sc_کانو;
    }
    sc_کانو = cmdline_ويڪر - sc_کانو;
    ru_کانو = cmdline_ويڪر - ru_کانو;
    إذا (sc_کانو <= 0)		// screen too narrow, will become a mess
	sc_کانو = 1;
    إذا (ru_کانو <= 0)
	ru_کانو = 1;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    سيٽ_vim_var_nr(VV_ECHOSPACE, sc_کانو - 1);
#جيڪڏهن_پورو
}

#إذا defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    صحيح
عدد_ويڪر(win_T *wp)
{
    صحيح		n;
    linenr_T	lnum;

    إذا (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_اوچائي;
    وإلا
	// cursor line shows absolute line number
	lnum = wp->w_بفر->b_ml.ml_سٽ_ڳڻپ;

    إذا (lnum == wp->w_nrwidth_سٽ_ڳڻپ && wp->w_nuw_cached == wp->w_p_nuw)
	أرجع wp->w_nrwidth_ويڪر;
    wp->w_nrwidth_سٽ_ڳڻپ = lnum;

    n = 0;
    افعل
    {
	lnum /= 10;
	++n;
    } طالما (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    إذا (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    إذا (n < 2 && get_پهريون_valid_sign(wp) != عدم
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_ويڪر = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    أرجع n;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    صحيح
screen_screencol(فراغ)
{
    أرجع screen_cur_کانو;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    صحيح
screen_screenrow(فراغ)
{
    أرجع screen_cur_صف;
}
#جيڪڏهن_پورو

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ساكن صحيح
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    إذا (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T انگ = 0;
	صحيح	    بائيٽون;
	صحيح	    n;

	لكل (بائيٽون = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; بائيٽون > 0; --بائيٽون)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    إذا (n < 0)
		أرجع 0;
	    انگ = انگ * 256 + n;
	}
	*p += 2;
	أرجع انگ;
    }
    أرجع mb_ptr2char_adv(p);
}

بنية charstab
{
    صحيح	    *cp;
    string_T	name;
};

#وصف_ڪريو CHARSTAB_داخلا(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ساكن fill_chars_T fill_chars;
ساكن بنية charstab filltab[] =
{
    CHARSTAB_داخلا(&fill_chars.stl,	    "stl"),
    CHARSTAB_داخلا(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_داخلا(&fill_chars.vert,	    "vert"),
    CHARSTAB_داخلا(&fill_chars.fold,	    "fold"),
    CHARSTAB_داخلا(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_داخلا(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_داخلا(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_داخلا(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_داخلا(&fill_chars.diff,	    "diff"),
    CHARSTAB_داخلا(&fill_chars.eob,	    "eob"),
    CHARSTAB_داخلا(&fill_chars.lastline,    "lastline"),
#إذا defined(FEAT_TABPANEL)
    CHARSTAB_داخلا(&fill_chars.tpl_vert,    "tpl_vert"),
#جيڪڏهن_پورو
    CHARSTAB_داخلا(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_داخلا(&fill_chars.truncrl,	    "truncrl"),
};
ساكن lcs_chars_T lcs_chars;
ساكن بنية charstab lcstab[] =
{
    CHARSTAB_داخلا(&lcs_chars.eol,	    "eol"),
    CHARSTAB_داخلا(&lcs_chars.ext,	    "extends"),
    CHARSTAB_داخلا(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_داخلا(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_داخلا(&lcs_chars.space,	    "space"),
    CHARSTAB_داخلا(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_داخلا(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_داخلا(&lcs_chars.trail,	    "trail"),
    CHARSTAB_داخلا(&lcs_chars.lead,	    "lead"),
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    CHARSTAB_داخلا(&lcs_chars.conceal,	    "conceal"),
#وإلا
    CHARSTAB_داخلا(عدم,		    "conceal"),
#جيڪڏهن_پورو
    CHARSTAB_داخلا(عدم,		    "multispace"),
    CHARSTAB_داخلا(عدم,		    "leadmultispace")
};

    ساكن حرف *
ميدان_قدر_err(حرف *errbuf, size_t errbuflen, حرف *fmt, char_u *ميدان)
{
    إذا (errbuf == عدم)
	أرجع "";
    vim_snprintf(errbuf, errbuflen, _(fmt), ميدان);
    أرجع errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ساكن حرف *
سيٽ_chars_option(win_T *wp, char_u *قدر, صحيح is_listchars, صحيح apply,
						حرف *errbuf, size_t errbuflen)
{
    صحيح	    ڦيرو, i, entries;
    char_u  *p, *s;
    صحيح	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *آخري_multispace = عدم;  // Last occurrence of "multispace:"
    char_u  *آخري_lmultispace = عدم; // Last occurrence of "leadmultispace:"
    صحيح	    multispace_len = 0;	      // Length of lcs-multispace string
    صحيح	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    بنية charstab *tab;

    إذا (is_listchars)
    {
	tab = lcstab;
	CLEAR_ميدان(lcs_chars);
	entries = ARRAY_ڊيگهائي(lcstab);
	إذا (wp->w_p_lcs[0] == NUL)
	    قدر = p_lcs;  // local value is empty, use the global value
    }
    وإلا
    {
	tab = filltab;
	entries = ARRAY_ڊيگهائي(filltab);
	إذا (wp->w_p_fcs[0] == NUL)
	    قدر = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    لكل (ڦيرو = 0; ڦيرو <= (apply ? 1 : 0); ++ڦيرو)
    {
	صحيح has_tab = FALSE, has_leadtab = FALSE;

	إذا (ڦيرو > 0)
	{
	    // After checking that the value is valid: set defaults.
	    إذا (is_listchars)
	    {
		لكل (i = 0; i < entries; ++i)
		    إذا (tab[i].cp != عدم)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		إذا (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(صحيح, multispace_len + 1);
		    إذا (lcs_chars.multispace != عدم)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		وإلا
		    lcs_chars.multispace = عدم;

		إذا (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(صحيح, lead_multispace_len + 1);
		    إذا (lcs_chars.leadmultispace != عدم)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		وإلا
		    lcs_chars.leadmultispace = عدم;
	    }
	    وإلا
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
#إذا defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#جيڪڏهن_پورو
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = قدر;
	طالما (*p)
	{
	    لكل (i = 0; i < entries; ++i)
	    {
		إذا (!(STRNCMP(p, tab[i].name.string, tab[i].name.ڊيگهائي) == 0 && p[tab[i].name.ڊيگهائي] == ':'))
		    استمر;

		s = p + tab[i].name.ڊيگهائي + 1;

		إذا (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    إذا (ڦيرو == 0)
		    {
			// Get length of lcs-multispace string in first round
			آخري_multispace = p;
			multispace_len = 0;
			طالما (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    إذا (char2cells(c1) > 1)
				أرجع ميدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_ويڪر_for_ميدان_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			إذا (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    أرجع ميدان_قدر_err(errbuf, errbuflen,
				    e_wrong_عدد_جو_characters_for_ميدان_str,
				    tab[i].name.string);
		    }
		    وإلا
		    {
			صحيح multispace_pos = 0;

			طالما (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    إذا (p == آخري_multispace && lcs_chars.multispace != عدم)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    اكسر;
		}

		إذا (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    إذا (ڦيرو == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			//ڦيرو
			آخري_lmultispace = p;
			lead_multispace_len = 0;
			طالما (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    إذا (char2cells(c1) > 1)
				أرجع ميدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_ويڪر_for_ميدان_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			إذا (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    أرجع ميدان_قدر_err(errbuf, errbuflen,
				    e_wrong_عدد_جو_characters_for_ميدان_str,
				    tab[i].name.string);
		    }
		    وإلا
		    {
			صحيح multispace_pos = 0;

			طالما (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    إذا (p == آخري_lmultispace && lcs_chars.leadmultispace != عدم)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    اكسر;
		}

		c2 = c3 = 0;
		إذا (*s == NUL)
		    أرجع ميدان_قدر_err(errbuf, errbuflen,
				    e_wrong_عدد_جو_characters_for_ميدان_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		إذا (char2cells(c1) > 1)
		    أرجع ميدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_ويڪر_for_ميدان_str,
					 tab[i].name.string);
		إذا (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    إذا (*s == NUL)
			أرجع ميدان_قدر_err(errbuf, errbuflen,
				    e_wrong_عدد_جو_characters_for_ميدان_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    إذا (char2cells(c2) > 1)
			أرجع ميدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_ويڪر_for_ميدان_str,
					 tab[i].name.string);
		    إذا (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			إذا (char2cells(c3) > 1)
			    أرجع ميدان_قدر_err(errbuf, errbuflen,
					 e_wrong_character_ويڪر_for_ميدان_str,
					 tab[i].name.string);
		    }
		    إذا (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    وإلا  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		إذا (*s == ',' || *s == NUL)
		{
		    إذا (ڦيرو > 0)
		    {
			إذا (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			وإلا إذا (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			وإلا إذا (tab[i].cp != عدم)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    اكسر;
		}
		وإلا
		    أرجع ميدان_قدر_err(errbuf, errbuflen,
				    e_wrong_عدد_جو_characters_for_ميدان_str,
				    tab[i].name.string);
	    }

	    إذا (i == entries)
		أرجع e_invalid_دليل;

	    إذا (*p == ',')
		++p;
	}

	إذا (is_listchars && has_leadtab && !has_tab)
	    أرجع e_leadtab_requires_tab;
    }

    إذا (apply)
    {
	إذا (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	وإلا
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    أرجع عدم;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    حرف *
سيٽ_fillchars_option(win_T *wp, char_u *val, صحيح apply, حرف *errbuf,
							      size_t errbuflen)
{
    أرجع سيٽ_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    حرف *
سيٽ_listchars_option(win_T *wp, char_u *val, صحيح apply, حرف *errbuf,
							      size_t errbuflen)
{
    أرجع سيٽ_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(filltab))
	أرجع عدم;

    أرجع filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(lcstab))
	أرجع عدم;

    أرجع lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    حرف *
check_chars_options(فراغ)
{
    tabpage_T   *tp;
    win_T	    *wp;

    إذا (سيٽ_listchars_option(curwin, p_lcs, FALSE, عدم, 0) != عدم)
	أرجع e_conflicts_with_قدر_جو_listchars;
    إذا (سيٽ_fillchars_option(curwin, p_fcs, FALSE, عدم, 0) != عدم)
	أرجع e_conflicts_with_قدر_جو_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (سيٽ_listchars_option(wp, wp->w_p_lcs, FALSE, عدم, 0) != عدم)
	    أرجع e_conflicts_with_قدر_جو_listchars;
	إذا (سيٽ_fillchars_option(wp, wp->w_p_fcs, FALSE, عدم, 0) != عدم)
	    أرجع e_conflicts_with_قدر_جو_fillchars;
    }
    أرجع عدم;
}
