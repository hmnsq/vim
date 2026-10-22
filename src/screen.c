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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
ਸਥਿਰ ਪੂਰਨ	screen_attr = 0;

ਸਥਿਰ ਖਾਲੀ screen_char_2(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ);
ਸਥਿਰ ਪੂਰਨ screenclear2(ਪੂਰਨ doclear);
ਸਥਿਰ ਖਾਲੀ lineclear(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ lineinvalid(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਚੌੜਾਈ);
ਸਥਿਰ ਪੂਰਨ win_do_ਸਤਰਾਂ(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਸਤਰ_ਗਿਣਤੀ, ਪੂਰਨ mayclear, ਪੂਰਨ del, ਪੂਰਨ clear_attr);
ਸਥਿਰ ਖਾਲੀ win_rest_invalid(win_T *wp);
ਸਥਿਰ ਖਾਲੀ ਸੁਨੇਹਾ_pos_ਬਹੁਲਕ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ recording_ਬਹੁਲਕ(ਪੂਰਨ attr);

// Ugly global: overrule attribute used by screen_char()
ਸਥਿਰ ਪੂਰਨ screen_char_attr = 0;

#ਜੇ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    ਪੂਰਨ
conceal_cursor_ਸਤਰ(win_T *wp)
{
    ਪੂਰਨ		c;

    ਜੇ (*wp->w_p_cocu == NUL)
	ਵਾਪਸ FALSE;
    ਜੇ (get_real_state() & ਬਹੁਲਕ_VISUAL)
	c = 'v';
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	c = 'i';
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	c = 'n';
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	c = 'c';
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;
    ਵਾਪਸ vim_strchr(wp->w_p_cocu, c) != ਨਿਰਾ;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    ਖਾਲੀ
conceal_check_cursor_ਸਤਰ(ਪੂਰਨ was_concealed)
{
    ਜੇ (curwin->w_p_cole <= 0 || conceal_cursor_ਸਤਰ(curwin) == was_concealed)
	ਵਾਪਸ;

    ਪੂਰਨ wcol = curwin->w_wcol;

    need_cursor_ਸਤਰ_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ਜੇ (!was_concealed)
	curwin->w_wcol = wcol;
}
#ਜੇ_ਅੰਤ

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    ਪੂਰਨ
get_win_attr(win_T *wp)
{
    ਪੂਰਨ	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ਜੇ (win_attr != 0)
    {
	ਜੇ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	ਨਹੀਂਤਾਂ
	    win_attr = 0;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(wp))
    {
	ਜੇ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	ਨਹੀਂਤਾਂ
	    win_attr = HL_ATTR(HLF_ਕੱਢੋ);    // Popup
    }
#ਜੇ_ਅੰਤ

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
    ਵਾਪਸ win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ਸਥਿਰ ਪੂਰਨ
screen_fill_ਸਿਰਾ(
	win_T *wp,
	ਪੂਰਨ	c1,
	ਪੂਰਨ	c2,
	ਪੂਰਨ	off,
	ਪੂਰਨ	ਚੌੜਾਈ,
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	endrow,
	ਪੂਰਨ	attr)
{
    ਪੂਰਨ	    nn = off + ਚੌੜਾਈ;

    ਜੇ (nn > wp->w_ਚੌੜਾਈ)
	nn = wp->w_ਚੌੜਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (ਪੂਰਨ)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (ਪੂਰਨ)wp->w_wincol + nn,
		c1, c2, attr);
    ਵਾਪਸ nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    ਖਾਲੀ
win_draw_ਸਿਰਾ(
    win_T	*wp,
    ਪੂਰਨ		c1,
    ਪੂਰਨ		c2,
    ਪੂਰਨ		draw_margin,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		endrow,
    hlf_T	hl)
{
    ਪੂਰਨ		n = 0;
    ਪੂਰਨ		attr = HL_ATTR(hl);
    ਪੂਰਨ		win_attr = get_win_attr(wp);
    bool	override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ਜੇ (draw_margin)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਪੂਰਨ	fdc = compute_foldcolumn(wp, 0);

	ਜੇ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_ਸਿਰਾ(wp, ' ', ' ', n, fdc,
		      ਪੰਕਤੀ, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	ਜੇ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_ਸਿਰਾ(wp, ' ', ' ', n, 2,
		      ਪੰਕਤੀ, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#ਜੇ_ਅੰਤ
	ਜੇ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == ਨਿਰਾ)
	    // draw the number column
	    n = screen_fill_ਸਿਰਾ(wp, ' ', ' ', n, ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1,
		       ਪੰਕਤੀ, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (ਪੂਰਨ)W_ENDCOL(wp), c1, c2, attr);
    }

    ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(wp, ਪੰਕਤੀ);

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    ਪੂਰਨ
compute_foldcolumn(win_T *wp, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    ਪੂਰਨ n = wp->w_ਚੌੜਾਈ - (ਖਾਨਾ + wmw);

    ਵਾਪਸ MIN(wp->w_p_fdc, n);
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
    ਪੂਰਨ		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		level;
    ਪੂਰਨ		ਪਹਿਲਾ_level;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2;
    ਪੂਰਨ		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    ਪੂਰਨ		ਚਿੰਨ੍ਹ = 0;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    ਖਾਲੀ_ਥਾਵਾਂ_2 = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    ਪਹਿਲਾ_level = level - fdc - closed + 1 + ਖਾਲੀ_ਥਾਵਾਂ_2;
    ਜੇ (ਪਹਿਲਾ_level < 1)
	ਪਹਿਲਾ_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    ਲਈ (i = 0; i < n; i++)
    {
	ਜੇ (win_foldinfo.fi_lnum == lnum
		&& ਪਹਿਲਾ_level + i >= win_foldinfo.fi_ਨੀਵਾਂ_level)
	    ਚਿੰਨ੍ਹ = wp->w_fill_chars.foldopen;
	ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_level == 1)
	    ਚਿੰਨ੍ਹ = wp->w_fill_chars.foldsep;
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_fill_chars.foldinner != NUL)
	    ਚਿੰਨ੍ਹ = wp->w_fill_chars.foldinner;
	ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_level + i <= 9)
	    ਚਿੰਨ੍ਹ = '0' + ਪਹਿਲਾ_level + i;
	ਨਹੀਂਤਾਂ
	    ਚਿੰਨ੍ਹ = '>';

	len = utf_char2bytes(ਚਿੰਨ੍ਹ, &p[byte_counter]);
	byte_counter += len;
	ਜੇ (ਪਹਿਲਾ_level + i >= level)
	{
	    i++;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (closed)
    {
	ਜੇ (ਚਿੰਨ੍ਹ != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ਜੇ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	ਚਿੰਨ੍ਹ = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(ਚਿੰਨ੍ਹ, &p[byte_counter]);
	byte_counter += len;
    }

    ਵਾਪਸ MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#ਜੇ_ਅੰਤ // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ਸਥਿਰ ਪੂਰਨ
comp_char_differs(ਪੂਰਨ off_from, ਪੂਰਨ off_ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < Screen_mco; ++i)
    {
	ਜੇ (Screen_ਸਤਰਾਂ_C[i][off_from] != Screen_ਸਤਰਾਂ_C[i][off_ਮੰਜ਼ਿਲ])
	    ਵਾਪਸ TRUE;
	ਜੇ (Screen_ਸਤਰਾਂ_C[i][off_from] == 0)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ਸਥਿਰ ਪੂਰਨ
char_needs_redraw(ਪੂਰਨ off_from, ਪੂਰਨ off_ਮੰਜ਼ਿਲ, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ > 0
	    && ((Screen_ਸਤਰਾਂ[off_from] != Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_ਮੰਜ਼ਿਲ])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_ਸਤਰਾਂ[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_ਸਤਰਾਂ[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_ਮੰਜ਼ਿਲ]
			: (ਖਾਨੇ_ਗਿਣਤੀ > 1 && Screen_ਸਤਰਾਂ[off_from + 1]
						 != Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ + 1])))
		|| (enc_utf8
		    && (Screen_ਸਤਰਾਂ_UC[off_from] != Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ]
			|| (Screen_ਸਤਰਾਂ_UC[off_from] != 0
			    && comp_char_differs(off_from, off_ਮੰਜ਼ਿਲ))
			|| ((*mb_off2cells)(off_from, off_from + ਖਾਨੇ_ਗਿਣਤੀ) > 1
			    && Screen_ਸਤਰਾਂ[off_from + 1]
						!= Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ + 1])))))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    ਪੂਰਨ
screen_get_current_ਸਤਰ_off(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ਸਥਿਰ ਪੂਰਨ
blocked_by_popup(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ off;

    ਜੇ (!popup_visible)
	ਵਾਪਸ FALSE;
    off = ਪੰਕਤੀ * screen_Columns + ਖਾਨਾ;
    ਵਾਪਸ popup_ਨਕਾਬ[off] > screen_zindex || popup_transparent[off];
}
#ਜੇ_ਅੰਤ

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    ਖਾਲੀ
reset_screen_attr(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ਸਥਿਰ ਪੂਰਨ
skip_for_popup(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ਜੇ (pum_under_menu(ਪੰਕਤੀ, ਖਾਨਾ, TRUE)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & ਬਹੁਲਕ_CMDLINE) == 0
	    && pum_under_menu(ਪੰਕਤੀ, ਖਾਨਾ, FALSE))
	ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
    ਜੇ (pum_visible()
	    && (State & ਬਹੁਲਕ_CMDLINE) == 0
	    && pum_under_menu(ਪੰਕਤੀ, ਖਾਨਾ, FALSE))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (blocked_by_popup(ਪੰਕਤੀ, ਖਾਨਾ))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ਸਥਿਰ char_u	*suppressed_cells = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	suppressed_ਪੰਕਤੀਆਂ = 0;
ਸਥਿਰ ਪੂਰਨ	suppressed_ਖਾਨੇ_ਗਿਣਤੀ = 0;

    ਸਥਿਰ ਖਾਲੀ
mark_suppressed_cell(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (suppressed_cells == ਨਿਰਾ
	    || suppressed_ਪੰਕਤੀਆਂ != screen_ਪੰਕਤੀਆਂ
	    || suppressed_ਖਾਨੇ_ਗਿਣਤੀ != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_ਪੰਕਤੀਆਂ * screen_Columns);
	ਜੇ (suppressed_cells == ਨਿਰਾ)
	{
	    suppressed_ਪੰਕਤੀਆਂ = 0;
	    suppressed_ਖਾਨੇ_ਗਿਣਤੀ = 0;
	    ਵਾਪਸ;
	}
	suppressed_ਪੰਕਤੀਆਂ = screen_ਪੰਕਤੀਆਂ;
	suppressed_ਖਾਨੇ_ਗਿਣਤੀ = screen_Columns;
    }
    suppressed_cells[ਪੰਕਤੀ * suppressed_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ] = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
unmark_suppressed_cell(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (suppressed_cells != ਨਿਰਾ
	    && ਪੰਕਤੀ >= 0 && ਪੰਕਤੀ < suppressed_ਪੰਕਤੀਆਂ
	    && ਖਾਨਾ >= 0 && ਖਾਨਾ < suppressed_ਖਾਨੇ_ਗਿਣਤੀ)
	suppressed_cells[ਪੰਕਤੀ * suppressed_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ] = FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
is_suppressed_cell(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਵਾਪਸ suppressed_cells != ਨਿਰਾ
	&& ਪੰਕਤੀ >= 0 && ਪੰਕਤੀ < suppressed_ਪੰਕਤੀਆਂ
	&& ਖਾਨਾ >= 0 && ਖਾਨਾ < suppressed_ਖਾਨੇ_ਗਿਣਤੀ
	&& suppressed_cells[ਪੰਕਤੀ * suppressed_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ];
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		popup_attr;	// get_win_attr(screen_opacity_popup)
    ਪੂਰਨ		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
popup_ਅਧਾਰ_attr_or(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ fallback)
{
    ਪੂਰਨ		ਅਧਾਰ = fallback;

    popup_get_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, ਖਾਨਾ, ਨਿਰਾ, &ਅਧਾਰ, ਨਿਰਾ);
    ਵਾਪਸ ਅਧਾਰ;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
popup_blend_with_ਅਧਾਰ(
	ਅਚਲ popup_opacity_T	*po,
	ਪੂਰਨ			char_attr,
	ਪੂਰਨ			ਅਧਾਰ_attr,
	ਪੂਰਨ			blend_fg)
{
    ਪੂਰਨ		combined = hl_combine_attr(po->popup_attr, char_attr);

    ਵਾਪਸ hl_blend_attr(ਅਧਾਰ_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ਸਥਿਰ ਖਾਲੀ
resolve_wide_char_opacity_attrs(
	ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ col1, ਪੂਰਨ col2,
	sattr_T *attr1, sattr_T *attr2)
{
    ਪੂਰਨ		bg1, bg2;
    ਪੂਰਨ		base1 = 0;
    ਪੂਰਨ		base2 = 0;
    attrentry_T	*ae;

    ਜੇ (*attr1 == *attr2)
	ਵਾਪਸ;

    popup_get_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, col1, ਨਿਰਾ, &base1, ਨਿਰਾ);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != ਨਿਰਾ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ਨਹੀਂਤਾਂ
    bg1 = (ae != ਨਿਰਾ && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_ਅਧਾਰ_screen_cell(ਪੰਕਤੀ, col2, ਨਿਰਾ, &base2, ਨਿਰਾ);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != ਨਿਰਾ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ਨਹੀਂਤਾਂ
    bg2 = (ae != ਨਿਰਾ && ae->ae_u.cterm.bg_color != 0);
# endif

    ਜੇ (bg1 && !bg2)
	*attr1 = *attr2;
    ਨਹੀਂਤਾਂ ਜੇ (!bg1 && bg2)
	*attr2 = *attr1;
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ
screen_ਸਤਰ(
	win_T	*wp,
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	coloff,
	ਪੂਰਨ	endcol,
	ਪੂਰਨ	clear_ਚੌੜਾਈ,
	colnr_T	ਆਖਰੀ_vcol,
	ਪੂਰਨ	flags ਅਣਵਰਤਿਆ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    off_from;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    off_ਮੰਜ਼ਿਲ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    max_off_from;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    max_off_ਮੰਜ਼ਿਲ;
    ਪੂਰਨ		    ਖਾਨਾ = 0;
    ਪੂਰਨ		    hl;
    ਪੂਰਨ		    force = FALSE;	// force update rest of the line
    ਪੂਰਨ		    redraw_this		// bool: does character need redraw?
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#ਜੇ_ਅੰਤ
				;
    ਪੂਰਨ		    redraw_ਅਗਲਾ;	// redraw_this for next character
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਪੂਰਨ		    ਬਦਲਿਆ_this;	// TRUE if character changed
    ਪੂਰਨ		    ਬਦਲਿਆ_ਅਗਲਾ;	// TRUE if next character changed
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		    clear_ਅਗਲਾ = FALSE;
    ਪੂਰਨ		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    ਪੂਰਨ		    drawing_opacity_popup =
				screen_opacity_popup != ਨਿਰਾ
				&& (flags & SLF_POPUP);
    CLEAR_ਖੇਤਰ(po);
    ਜੇ (drawing_opacity_popup)
	popup_opacity_init(&po);
#ਜੇ_ਅੰਤ

    // Check for illegal row and col, just in case.
    ਜੇ (ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    ਜੇ (endcol > Columns)
	endcol = Columns;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    clip_may_clear_selection(ਪੰਕਤੀ, ਪੰਕਤੀ);
#ਜੇ_ਅੰਤ

    off_from = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
    off_ਮੰਜ਼ਿਲ = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_ਮੰਜ਼ਿਲ = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + screen_Columns;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ਜੇ (clear_ਚੌੜਾਈ > 0)
	{
	    ਪੂਰਨ clear_ਸ਼ੁਰੂ = ਖਾਨਾ;

	    ਜਦੋਂ (ਖਾਨਾ <= endcol && Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] == ' '
		    && ScreenAttrs[off_ਮੰਜ਼ਿਲ] == 0
				  && (!enc_utf8 || Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] == 0))
	    {
		++off_ਮੰਜ਼ਿਲ;
		++ਖਾਨਾ;
	    }
	    ਜੇ (ਖਾਨਾ <= endcol)
		screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    ਲਈ (ਪੂਰਨ i = endcol; i >= clear_ਸ਼ੁਰੂ; i--)
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_ਮੰਜ਼ਿਲ + (i - ਖਾਨਾ)] =
		    (flags & SLF_INC_VCOL) ? ++ਆਖਰੀ_vcol : ਆਖਰੀ_vcol;
	}
	ਖਾਨਾ = endcol + 1;
	off_ਮੰਜ਼ਿਲ = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ + coloff;
	off_from += ਖਾਨਾ;
	endcol = (clear_ਚੌੜਾਈ > 0 ? clear_ਚੌੜਾਈ : -clear_ਚੌੜਾਈ);
    }
#ਜੇ_ਅੰਤ // FEAT_RIGHTLEFT

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    ਜੇ (coloff > 0 && enc_utf8
		   && Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] == 0
		   && Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ - 1] != 0
		   && (*mb_char2cells)(Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ - 1]) > 1)
    {
	Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ - 1] = ' ';
	Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	ਜੇ (screen_opacity_popup == ਨਿਰਾ)
# endif
	    screen_char(off_ਮੰਜ਼ਿਲ - 1, ਪੰਕਤੀ, ਖਾਨਾ + coloff - 1);
    }
#ਜੇ_ਅੰਤ

    redraw_ਅਗਲਾ = char_needs_redraw(off_from, off_ਮੰਜ਼ਿਲ, endcol - ਖਾਨਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਬਦਲਿਆ_ਅਗਲਾ = redraw_ਅਗਲਾ;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (ਖਾਨਾ < endcol)
    {
	ਜੇ (has_mbyte && (ਖਾਨਾ + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	ਨਹੀਂਤਾਂ
	    char_cells = 1;

	redraw_this = redraw_ਅਗਲਾ;
	redraw_ਅਗਲਾ = force || char_needs_redraw(off_from + char_cells,
			      off_ਮੰਜ਼ਿਲ + char_cells, endcol - ਖਾਨਾ - char_cells);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ਜੇ (!redraw_this && is_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + coloff))
	    redraw_this = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	ਬਦਲਿਆ_this = ਬਦਲਿਆ_ਅਗਲਾ;
	ਬਦਲਿਆ_ਅਗਲਾ = redraw_ਅਗਲਾ;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	ਜੇ (redraw_ਅਗਲਾ && gui.ਵਿੱਚ_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_ਮੰਜ਼ਿਲ + char_cells];
	    ਜੇ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ਜੇ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#ਜੇ_ਅੰਤ
	// skip the second cell for double-width characters.
	ਜੇ (redraw_this && char_cells == 2 && skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ਜੇ (redraw_this && skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ + coloff))
	    redraw_this = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	ਪੂਰਨ opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ਜੇ (drawing_opacity_popup
		&& Screen_ਸਤਰਾਂ[off_from] == ' '
		&& (!enc_utf8 || Screen_ਸਤਰਾਂ_UC[off_from] == 0))
	{
	    ਪੂਰਨ popup_src_attr = ScreenAttrs[off_from];
	    ਪੂਰਨ bg_is_ਖਾਲੀ_ਥਾਵਾਂ = Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] == 0
				&& (!enc_utf8 || Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ਜੇ (bg_is_ਖਾਲੀ_ਥਾਵਾਂ
		    && off_ਮੰਜ਼ਿਲ > 0
		    && enc_utf8 && Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ - 1] != 0
		    && utf_char2cells(Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    ਨਹੀਂਤਾਂ ਜੇ (Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] != 0
		    || (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] != 0))
	    {
		ਪੂਰਨ bg_char_cells = (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] != 0)
				? utf_char2cells(Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ]) : 1;

		ਜੇ (bg_char_cells == 2)
		{
		    ਜੇ (ਖਾਨਾ + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_ਮੰਜ਼ਿਲ + 1 >= max_off_ਮੰਜ਼ਿਲ)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			ਪੂਰਨ ਅਧਾਰ_attr;
			Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = ' ';
			ਜੇ (enc_utf8)
			    Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] = 0;
			ਅਧਾਰ_attr = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, ਖਾਨਾ + coloff,
							     ScreenAttrs[off_ਮੰਜ਼ਿਲ]);
			ScreenAttrs[off_ਮੰਜ਼ਿਲ] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, ਅਧਾਰ_attr, TRUE);
			screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			ਜਾਓ skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    ਜੇ (!(Screen_ਸਤਰਾਂ[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_ਸਤਰਾਂ_UC[off_from + 1] == 0)))
			ਜਾਓ skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		ਪੂਰਨ ਅਧਾਰ_attr = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, ਖਾਨਾ + coloff,
							     ScreenAttrs[off_ਮੰਜ਼ਿਲ]);
		ScreenAttrs[off_ਮੰਜ਼ਿਲ] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, ਅਧਾਰ_attr, TRUE);
		screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ਜੇ (bg_char_cells == 2)
		{
		    ਪੂਰਨ ਅਧਾਰ_attr2 = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, ਖਾਨਾ + coloff + 1,
							 ScreenAttrs[off_ਮੰਜ਼ਿਲ + 1]);
		    ScreenAttrs[off_ਮੰਜ਼ਿਲ + 1] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, ਅਧਾਰ_attr2, TRUE);
		    ਜੇ (po.blend == 100)
			resolve_wide_char_opacity_attrs(ਪੰਕਤੀ,
				ਖਾਨਾ + coloff, ਖਾਨਾ + coloff + 1,
				&ScreenAttrs[off_ਮੰਜ਼ਿਲ], &ScreenAttrs[off_ਮੰਜ਼ਿਲ + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    ਨਹੀਂਤਾਂ ਜੇ (bg_is_ਖਾਲੀ_ਥਾਵਾਂ)
	    {
		ਪੂਰਨ ਅਧਾਰ_attr;
		Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = ' ';
		ਅਧਾਰ_attr = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, ਖਾਨਾ + coloff,
							     ScreenAttrs[off_ਮੰਜ਼ਿਲ]);
		ScreenAttrs[off_ਮੰਜ਼ਿਲ] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, ਅਧਾਰ_attr, TRUE);
		screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#ਜੇ_ਅੰਤ

	ਜੇ (redraw_this)
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
	    ਜੇ (       p_wiv
		    && !force
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		    && ScreenAttrs[off_ਮੰਜ਼ਿਲ] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_ਮੰਜ਼ਿਲ])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(ਪੰਕਤੀ, ਖਾਨਾ + coloff);
		ਬਾਹਰਲਾ_str(T_CE);		// clear rest of this screen line
		screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_ਅਗਲਾ = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		ਜੇ (ਖਾਨਾ + coloff > 0 && ScreenAttrs[off_ਮੰਜ਼ਿਲ - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_ਮੰਜ਼ਿਲ - 1];
		    ਪਦ_windgoto(ਪੰਕਤੀ, ਖਾਨਾ + coloff);
		    screen_stop_highlight();
		}
		ਨਹੀਂਤਾਂ
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ਜੇ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ਜੇ (char_cells == 1
			&& ਖਾਨਾ + 1 < endcol
			&& (*mb_off2cells)(off_ਮੰਜ਼ਿਲ, max_off_ਮੰਜ਼ਿਲ) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ + 1] = 0;
		    redraw_ਅਗਲਾ = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (char_cells == 2
			&& ਖਾਨਾ + 2 < endcol
			&& (*mb_off2cells)(off_ਮੰਜ਼ਿਲ, max_off_ਮੰਜ਼ਿਲ) == 1
			&& (*mb_off2cells)(off_ਮੰਜ਼ਿਲ + 1, max_off_ਮੰਜ਼ਿਲ) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ + 2] = 0;
		    redraw_ਅਗਲਾ = TRUE;
		}

		ਜੇ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_ਮੰਜ਼ਿਲ] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ਜੇ (has_mbyte && ਖਾਨਾ + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_ਮੰਜ਼ਿਲ, max_off_ਮੰਜ਼ਿਲ) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_ਮੰਜ਼ਿਲ, max_off_ਮੰਜ਼ਿਲ) == 1
			    && (*mb_off2cells)(off_ਮੰਜ਼ਿਲ + 1, max_off_ਮੰਜ਼ਿਲ) > 1)))
		clear_ਅਗਲਾ = TRUE;

	    Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = Screen_ਸਤਰਾਂ[off_from];
	    ਜੇ (enc_utf8)
	    {
		Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] = Screen_ਸਤਰਾਂ_UC[off_from];
		ਜੇ (Screen_ਸਤਰਾਂ_UC[off_from] != 0)
		{
		    ਪੂਰਨ	    i;

		    ਲਈ (i = 0; i < Screen_mco; ++i)
			Screen_ਸਤਰਾਂ_C[i][off_ਮੰਜ਼ਿਲ] = Screen_ਸਤਰਾਂ_C[i][off_from];
		}
	    }
	    ਜੇ (char_cells == 2)
	    {
		Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ + 1] = Screen_ਸਤਰਾਂ[off_from + 1];
		ਜੇ (enc_utf8)
		    Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ + 1] = 0;
	    }

#ਜੇ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ਜੇ (
# ifdef FEAT_GUI
		    gui.ਵਿੱਚ_use
# endif
# ਜੇ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    ਪਦ_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_ਮੰਜ਼ਿਲ];
		ਜੇ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ਜੇ (hl & HL_BOLD)
		    redraw_ਅਗਲਾ = TRUE;
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ਜੇ (gui.ਵਿੱਚ_use && ਬਦਲਿਆ_this)
		redraw_ਅਗਲਾ = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ਜੇ (gui.directx_enabled && ਬਦਲਿਆ_this)
		redraw_this = TRUE;
# endif
#ਜੇ_ਅੰਤ

	    ScreenAttrs[off_ਮੰਜ਼ਿਲ] = ScreenAttrs[off_from];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ਜੇ (drawing_opacity_popup && po.blend > 0)
	    {
		ਪੂਰਨ popup_src_attr = ScreenAttrs[off_from];
		ਪੂਰਨ scol1 = ਖਾਨਾ + coloff;
		ਪੂਰਨ base1 = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, scol1, 0);

		ਜੇ (char_cells == 2)
		{
		    ਪੂਰਨ base2 = popup_ਅਧਾਰ_attr_or(ਪੰਕਤੀ, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    ਪੂਰਨ over1 = popup_is_over_opacity(ਪੰਕਤੀ, scol1);
		    ਪੂਰਨ over2 = popup_is_over_opacity(ਪੰਕਤੀ, scol1 + 1);
		    ਜੇ (over1 != over2)
		    {
			ਜੇ (over1)
			    base1 = base2;
			ਨਹੀਂਤਾਂ
			    base2 = base1;
		    }
		    ScreenAttrs[off_ਮੰਜ਼ਿਲ + 1] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_ਮੰਜ਼ਿਲ] = popup_blend_with_ਅਧਾਰ(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ਜੇ (char_cells == 2)
		ScreenAttrs[off_ਮੰਜ਼ਿਲ + 1] = ScreenAttrs[off_ਮੰਜ਼ਿਲ];

	    ਜੇ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
	    ਨਹੀਂਤਾਂ
		screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
	}
	ਨਹੀਂਤਾਂ ਜੇ (  p_wiv
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		&& ਖਾਨਾ + coloff > 0 && off_ਮੰਜ਼ਿਲ > 0)
	{
	    ਜੇ (ScreenAttrs[off_ਮੰਜ਼ਿਲ] == ScreenAttrs[off_ਮੰਜ਼ਿਲ - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (screen_attr != 0)
		screen_stop_highlight();
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (!opacity_blank)
#ਜੇ_ਅੰਤ
	{
	    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_ਮੰਜ਼ਿਲ] = Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_from];
	    ਜੇ (char_cells == 2)
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_ਮੰਜ਼ਿਲ + 1] = Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_from + 1];
	}

	off_ਮੰਜ਼ਿਲ += char_cells;
	off_from += char_cells;
	ਖਾਨਾ += char_cells;
    }

    ਜੇ (clear_ਅਗਲਾ && !skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = ' ';
	ਜੇ (enc_utf8)
	    Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (screen_opacity_popup == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
	    screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
    }

    ਜੇ (clear_ਚੌੜਾਈ > 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#ਜੇ_ਅੰਤ
				   )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ_2 = ਖਾਨਾ;
#ਜੇ_ਅੰਤ

	// blank out the rest of the line
	ਜਦੋਂ (ਖਾਨਾ < clear_ਚੌੜਾਈ && Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] == ' '
						  && ScreenAttrs[off_ਮੰਜ਼ਿਲ] == 0
				  && (!enc_utf8 || Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] == 0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
				  && !is_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + coloff)
#ਜੇ_ਅੰਤ
				  )
	{
	    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_ਮੰਜ਼ਿਲ] =
			      (flags & SLF_INC_VCOL) ? ++ਆਖਰੀ_vcol : ਆਖਰੀ_vcol;
	    ++off_ਮੰਜ਼ਿਲ;
	    ++ਖਾਨਾ;
	}
	ਜੇ (ਖਾਨਾ < clear_ਚੌੜਾਈ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ਜੇ (gui.ਵਿੱਚ_use && (ਖਾਨਾ > ਸ਼ੁਰੂ_ਖਾਨਾ_2 || !redraw_this))
	    {
		hl = ScreenAttrs[off_ਮੰਜ਼ਿਲ];
		ਜੇ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    ਪੂਰਨ prev_cells = 1;

		    ਜੇ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ - 1] == 0 ? 2 : 1;
		    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			ਬਿਨਾਨਿਸ਼ਾਨੀ off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ];
			ਬਿਨਾਨਿਸ਼ਾਨੀ max_off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + screen_Columns;

			ਜਦੋਂ (off < off_ਮੰਜ਼ਿਲ)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ਜੇ (!skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ + coloff - prev_cells))
		    {
			ਜੇ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_ਮੰਜ਼ਿਲ - prev_cells, ਪੰਕਤੀ,
						   ਖਾਨਾ + coloff - prev_cells);
			ਨਹੀਂਤਾਂ
			    screen_char(off_ਮੰਜ਼ਿਲ - prev_cells, ਪੰਕਤੀ,
						   ਖਾਨਾ + coloff - prev_cells);
		    }
		}
	    }
#ਜੇ_ਅੰਤ
	    screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, ਖਾਨਾ + coloff, clear_ਚੌੜਾਈ + coloff,
								 ' ', ' ', 0);
	    ਜਦੋਂ (ਖਾਨਾ < clear_ਚੌੜਾਈ)
	    {
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[off_ਮੰਜ਼ਿਲ++]
			    = (flags & SLF_INC_VCOL) ? ++ਆਖਰੀ_vcol : ਆਖਰੀ_vcol;
		++ਖਾਨਾ;
	    }
	}
    }

    ਜੇ (clear_ਚੌੜਾਈ > 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#ਜੇ_ਅੰਤ
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ਜੇ (coloff + ਖਾਨਾ < firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
	{
	    ਜੇ (!skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ + coloff))
	    {
		ਪੂਰਨ c;

		c = fillchar_vsep(&hl, wp, ਪੰਕਤੀ);
		ਜੇ (Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] != (schar_T)c
			|| (enc_utf8 && (ਪੂਰਨ)Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_ਮੰਜ਼ਿਲ] != hl)
		{
		    Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = c;
		    ScreenAttrs[off_ਮੰਜ਼ਿਲ] = hl;
		    ਜੇ (enc_utf8)
		    {
			ਜੇ (c >= 0x80)
			{
			    Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] = c;
			    Screen_ਸਤਰਾਂ_C[0][off_ਮੰਜ਼ਿਲ] = 0;
			    Screen_ਸਤਰਾਂ[off_ਮੰਜ਼ਿਲ] = 0x80; // avoid storing zero
			}
			ਨਹੀਂਤਾਂ
			    Screen_ਸਤਰਾਂ_UC[off_ਮੰਜ਼ਿਲ] = 0;
		    }
		    screen_char(off_ਮੰਜ਼ਿਲ, ਪੰਕਤੀ, ਖਾਨਾ + coloff);
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਸਤਰ_Wraps[ਪੰਕਤੀ] = FALSE;
    }

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
}

#ਜੇ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    ਖਾਲੀ
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    ਪੂਰਨ		t;

    ਲਈ (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    ਖਾਲੀ
draw_vsep_win(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ)
{
    ਜੇ (!wp->w_vsep_ਚੌੜਾਈ)
	ਵਾਪਸ;

    ਪੂਰਨ content_ਸਿਰਾ = W_WINROW(wp) + wp->w_ਉਚਾਈ;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    ਪੂਰਨ ਸ਼ੁਰੂ_ਪੰਕਤੀ = (ਪੰਕਤੀ == 0) ? wp->w_winrow : W_WINROW(wp) + ਪੰਕਤੀ;
    ਲਈ (ਪੂਰਨ r = ਸ਼ੁਰੂ_ਪੰਕਤੀ; r < content_ਸਿਰਾ; ++r)
    {
	ਪੂਰਨ hl;
	ਪੂਰਨ c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
    {
	ਪੂਰਨ hl;
	ਪੂਰਨ c;
	ਜੇ (stl_connected(wp))
	    c = fillchar_ਸਥਿਤੀ(&hl, wp);
	ਨਹੀਂਤਾਂ
	    c = fillchar_vsep(&hl, wp, content_ਸਿਰਾ);
	ਲਈ (ਪੂਰਨ r = content_ਸਿਰਾ; r < content_ਸਿਰਾ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    ਪੂਰਨ
stl_connected(win_T *wp)
{
    ਚੌਖਟਾ_T	*fr;

    fr = wp->w_ਚੌਖਟਾ;
    ਜਦੋਂ (fr->fr_ਮਾਪਾ != ਨਿਰਾ)
    {
	ਜੇ (fr->fr_ਮਾਪਾ->fr_layout == FR_ਖਾਨਾ)
	{
	    ਜੇ (fr->fr_ਅਗਲਾ != ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fr->fr_ਅਗਲਾ != ਨਿਰਾ)
		ਵਾਪਸ TRUE;
	}
	fr = fr->fr_ਮਾਪਾ;
    }
    ਵਾਪਸ FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    ਪੂਰਨ
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    ਪੂਰਨ		len)	    // length of buffer
{
    char_u	*p;
    ਪੂਰਨ		plen;

    ਜੇ (wp->w_ਬਫਰ->b_p_iminsert != B_IMODE_LMAP)
	ਵਾਪਸ 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	ਮੰਜ਼ਿਲ_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_ਬਫਰ;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_ਮੰਜ਼ਿਲ_string(ਮੰਜ਼ਿਲ_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    ਜੇ (p == ਨਿਰਾ || *p == NUL)
#ਜੇ_ਅੰਤ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	ਜੇ (wp->w_ਬਫਰ->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_ਬਫਰ->b_p_keymap;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((ਅੱਖਰ *)buf, len, (ਅੱਖਰ *)fmt, p);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    vim_free(s);
#ਜੇ_ਅੰਤ
    ਜੇ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    ਵਾਪਸ plen;
}

#ਜੇ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    ਖਾਲੀ
win_redr_custom(
    win_T	*wp,
    ਪੂਰਨ		draw_ruler)	// TRUE or FALSE
{
    ਸਥਿਰ ਪੂਰਨ	entered = FALSE;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		curattr;
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ = 0;
    ਪੂਰਨ		maxwidth;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		n;
    ਪੂਰਨ		len;
    ਪੂਰਨ		stlh_cnt;
    ਪੂਰਨ		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    ਪੂਰਨ		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    ਪੂਰਨ		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ਜੇ (entered)
	ਵਾਪਸ;
    entered = TRUE;

    // setup environment for the task at hand
    ਜੇ (wp == ਨਿਰਾ)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	ਪੰਕਤੀ = 0;
	ਖਾਨਾ = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_ਚੌੜਾਈ;
	opt_name = (char_u *)"tabline";
    }
    ਨਹੀਂਤਾਂ
    {
	override_success = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ਪੰਕਤੀ = statusline_ਪੰਕਤੀ(wp);
	fillchar = fillchar_ਸਥਿਤੀ(&attr, wp);
	ਪੂਰਨ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2 = wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0;
	maxwidth = ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2 ? wp->w_ਚੌੜਾਈ : cmdline_ਚੌੜਾਈ;

	ਜੇ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ਜੇ (*stl == '%')
	    {
		ਜੇ (*++stl == '-')
		    stl++;
		ਜੇ (atoi((ਅੱਖਰ *)stl))
		    ਜਦੋਂ (VIM_ISDIGIT(*stl))
			stl++;
		ਜੇ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    ਖਾਨਾ = ru_ਖਾਨਾ - (cmdline_ਚੌੜਾਈ - maxwidth);
	    ਜੇ (ਖਾਨਾ < (maxwidth + 1) / 2)
		ਖਾਨਾ = (maxwidth + 1) / 2;
	    maxwidth -= ਖਾਨਾ;
	    ਜੇ (!ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2)
	    {
		ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    opt_name = (char_u *)"statusline";
	    ਜੇ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    ਨਹੀਂਤਾਂ
		stl = p_stl;
	}

	ਜੇ (ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2)
	    ਖਾਨਾ += wp->w_wincol;
	ਨਹੀਂਤਾਂ
	    ਖਾਨਾ += cmdline_ਖਾਨਾ_off;
    }

    ਜੇ (maxwidth <= 0)
	ਜਾਓ theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == ਨਿਰਾ ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_ਸਥਿਤੀ_ਉਚਾਈ;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == ਨਿਰਾ) ? (char_u *)"" : stl;
    ਪੂਰਨ ਖਾਨਾ_save = ਖਾਨਾ;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **ਬਾਹਰਲਾ_regions = ਨਿਰਾ;
    ਪੂਰਨ			*ਬਾਹਰਲਾ_ਗਿਣਤੀ = ਨਿਰਾ;
    ਪੂਰਨ			region_ਅਧਾਰ_ਖਾਨਾ = 0;
    ਜੇ (!draw_ruler)
    {
	ਜੇ (wp != ਨਿਰਾ)
	{
	    ਬਾਹਰਲਾ_regions = &wp->w_stl_click;
	    ਬਾਹਰਲਾ_ਗਿਣਤੀ = &wp->w_stl_click_ਗਿਣਤੀ;
	    region_ਅਧਾਰ_ਖਾਨਾ = wp->w_wincol;
	}
	ਨਹੀਂਤਾਂ
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    ਬਾਹਰਲਾ_regions = &tabline_stl_click;
	    ਬਾਹਰਲਾ_ਗਿਣਤੀ = &tabline_stl_click_ਗਿਣਤੀ;
	    region_ਅਧਾਰ_ਖਾਨਾ = firstwin->w_wincol;
	}

	ਲਈ (n = 0; n < *ਬਾਹਰਲਾ_ਗਿਣਤੀ; n++)
	    vim_free((*ਬਾਹਰਲਾ_regions)[n].funcname);
	VIM_CLEAR(*ਬਾਹਰਲਾ_regions);
	*ਬਾਹਰਲਾ_ਗਿਣਤੀ = 0;
    }

    ਪੂਰਨ ਹਾਸਲ_hl = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < stlh_cnt; i++)
    {
	ਖਾਨਾ = ਖਾਨਾ_save;
	buf[0] = NUL;
	ਚੌੜਾਈ = build_stl_str_hl_mline(ewp, buf, ਆਕਾਰ(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&ਹਾਸਲ_hl);

	// Make all characters printable.
	p = transstr(buf);
	ਜੇ (p != ਨਿਰਾ)
	{
	    len = vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), "%s", p);
	    vim_free(p);
	}
	ਨਹੀਂਤਾਂ
	    len = (ਪੂਰਨ)STRLEN(buf);

	// fill up with "fillchar"
	ਜਦੋਂ (ਚੌੜਾਈ < maxwidth && len < (ਪੂਰਨ)ਆਕਾਰ(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++ਚੌੜਾਈ;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	ਲਈ (n = 0; hltab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
	{
	    len = (ਪੂਰਨ)(hltab[n].ਸ਼ੁਰੂ - p);
	    screen_puts_len(p, len, ਪੰਕਤੀ + i, ਖਾਨਾ, curattr);
	    ਖਾਨਾ += vim_strnsize(p, len);
	    p = hltab[n].ਸ਼ੁਰੂ;

	    ਜੇ (hltab[n].userhl == 0)
		curattr = attr;
	    ਨਹੀਂਤਾਂ ਜੇ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    ਨਹੀਂਤਾਂ ਜੇ (wp != ਨਿਰਾ && wp != curwin && bt_terminal(wp->w_ਬਫਰ)
						    && wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    ਨਹੀਂਤਾਂ ਜੇ (wp != ਨਿਰਾ && bt_terminal(wp->w_ਬਫਰ)
						    && wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    ਨਹੀਂਤਾਂ ਜੇ (wp != ਨਿਰਾ && wp != curwin && wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    ਨਹੀਂਤਾਂ
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, ਪੰਕਤੀ + i, ਖਾਨਾ, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ਜੇ (ਬਾਹਰਲਾ_regions != ਨਿਰਾ)
	{
	    ਪੂਰਨ click_ਗਿਣਤੀ = 0;
	    ਲਈ (n = 0; clicktab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
		click_ਗਿਣਤੀ++;

	    ਜੇ (click_ਗਿਣਤੀ > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*ਬਾਹਰਲਾ_regions,
			ਆਕਾਰ(stl_click_region_T)
				    * (*ਬਾਹਰਲਾ_ਗਿਣਤੀ + click_ਗਿਣਤੀ));
		ਜੇ (new_arr != ਨਿਰਾ)
		{
		    char_u  *cur_funcname = ਨਿਰਾ;
		    ਪੂਰਨ	    cur_minwid = 0;
		    ਪੂਰਨ	    region_ਸ਼ੁਰੂ = region_ਅਧਾਰ_ਖਾਨਾ;

		    *ਬਾਹਰਲਾ_regions = new_arr;

		    len = 0;
		    p = buf;
		    ਲਈ (n = 0; clicktab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
		    {
			len += vim_strnsize(p,
					    (ਪੂਰਨ)(clicktab[n].ਸ਼ੁਰੂ - p));
			p = clicktab[n].ਸ਼ੁਰੂ;

			ਜੇ (cur_funcname != ਨਿਰਾ)
			{
			    stl_click_region_T *r =
					&(*ਬਾਹਰਲਾ_regions)[*ਬਾਹਰਲਾ_ਗਿਣਤੀ];
			    r->ਪੰਕਤੀ = ਪੰਕਤੀ + i;
			    r->ਖਾਨਾ_ਸ਼ੁਰੂ = region_ਸ਼ੁਰੂ;
			    r->ਖਾਨਾ_ਸਿਰਾ = region_ਅਧਾਰ_ਖਾਨਾ + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*ਬਾਹਰਲਾ_ਗਿਣਤੀ)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_ਸ਼ੁਰੂ = region_ਅਧਾਰ_ਖਾਨਾ + len;
		    }

		    // Close final region if it extends to the end.
		    ਜੇ (cur_funcname != ਨਿਰਾ)
		    {
			stl_click_region_T *r =
					&(*ਬਾਹਰਲਾ_regions)[*ਬਾਹਰਲਾ_ਗਿਣਤੀ];
			r->ਪੰਕਤੀ = ਪੰਕਤੀ + i;
			r->ਖਾਨਾ_ਸ਼ੁਰੂ = region_ਸ਼ੁਰੂ;
			r->ਖਾਨਾ_ਸਿਰਾ = region_ਅਧਾਰ_ਖਾਨਾ + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*ਬਾਹਰਲਾ_ਗਿਣਤੀ)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	ਲਈ (n = 0; clicktab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ਜੇ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ਜੇ (stl != ਨਿਰਾ)
	vim_free(stl_tmp);

    ਜੇ (wp == ਨਿਰਾ)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	ਖਾਨਾ = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	ਲਈ (n = 0; tabtab[n].ਸ਼ੁਰੂ != ਨਿਰਾ; n++)
	{
	    len += vim_strnsize(p, (ਪੂਰਨ)(tabtab[n].ਸ਼ੁਰੂ - p));
	    ਜਦੋਂ (ਖਾਨਾ < len)
		Tab_ਪੰਨਾ_Idxs[ਖਾਨਾ++] = fillchar;
	    p = tabtab[n].ਸ਼ੁਰੂ;
	    fillchar = tabtab[n].userhl;
	}
	ਜਦੋਂ (ਖਾਨਾ < firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
	    Tab_ਪੰਨਾ_Idxs[ਖਾਨਾ++] = fillchar;
    }

theend:
    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
    entered = FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    ਖਾਲੀ
screen_putchar(ਪੂਰਨ c, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ਜੇ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ਨਹੀਂਤਾਂ
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, ਪੰਕਤੀ, ਖਾਨਾ, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    ਖਾਲੀ
screen_getbytes(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *ਬਾਈਟਾਂ, ਪੂਰਨ *attrp)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ off;

    // safety check
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ;

    off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ;
    ਜੇ (attrp != ਨਿਰਾ)
	*attrp = ScreenAttrs[off];
    ਬਾਈਟਾਂ[0] = Screen_ਸਤਰਾਂ[off];
    ਬਾਈਟਾਂ[1] = NUL;

    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0)
	ਬਾਈਟਾਂ[utfc_char2bytes(off, ਬਾਈਟਾਂ)] = NUL;
    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU && Screen_ਸਤਰਾਂ[off] == 0x8e)
    {
	ਬਾਈਟਾਂ[0] = Screen_ਸਤਰਾਂ[off];
	ਬਾਈਟਾਂ[1] = ScreenLines2[off];
	ਬਾਈਟਾਂ[2] = NUL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs && MB_BYTE2LEN(ਬਾਈਟਾਂ[0]) > 1)
    {
	ਬਾਈਟਾਂ[1] = Screen_ਸਤਰਾਂ[off + 1];
	ਬਾਈਟਾਂ[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ਸਥਿਰ ਪੂਰਨ
screen_comp_differs(ਪੂਰਨ off, ਪੂਰਨ *u8cc)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < Screen_mco; ++i)
    {
	ਜੇ (Screen_ਸਤਰਾਂ_C[i][off] != (u8char_T)u8cc[i])
	    ਵਾਪਸ TRUE;
	ਜੇ (u8cc[i] == 0)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    ਖਾਲੀ
screen_puts(
    char_u	*ਪਾਠ,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		attr)
{
    screen_puts_len(ਪਾਠ, -1, ਪੰਕਤੀ, ਖਾਨਾ, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    ਖਾਲੀ
screen_puts_len(
    char_u	*ਪਾਠ,
    ਪੂਰਨ		textlen,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		attr_arg)
{
    ਪੂਰਨ		attr = attr_arg;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	off;
    char_u	*ptr = ਪਾਠ;
    ਪੂਰਨ		len = textlen;
    ਪੂਰਨ		c;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	max_off;
    ਪੂਰਨ		mbyte_blen = 1;
    ਪੂਰਨ		mbyte_cells = 1;
    ਪੂਰਨ		u8c = 0;
    ਪੂਰਨ		u8cc[MAX_MCO];
    ਪੂਰਨ		clear_ਅਗਲਾ_cell = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    ਪੂਰਨ		prev_c = 0;		// previous Arabic character
    ਪੂਰਨ		pc, nc, nc1;
    ਪੂਰਨ		pcc[MAX_MCO];
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		force_redraw_this;
    ਪੂਰਨ		force_redraw_ਅਗਲਾ = FALSE;
    ਪੂਰਨ		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ
	    || ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਪੰਕਤੀ < 0
	    || ਖਾਨਾ >= screen_Columns || ਖਾਨਾ < 0)
	ਵਾਪਸ;
    off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ਜੇ (has_mbyte && ਖਾਨਾ > 0 && ਖਾਨਾ < screen_Columns
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
	    && mb_fix_ਖਾਨਾ(ਖਾਨਾ, ਪੰਕਤੀ) != ਖਾਨਾ)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ਜੇ (!skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ - 1))
	{
	    Screen_ਸਤਰਾਂ[off - 1] = ' ';
	    ਜੇ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	    {
		Screen_ਸਤਰਾਂ_UC[off - 1] = 0;
		Screen_ਸਤਰਾਂ_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, ਪੰਕਤੀ, ਖਾਨਾ - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_ਅਗਲਾ = TRUE;
    }

    max_off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + screen_Columns;
    ਜਦੋਂ (ਖਾਨਾ < screen_Columns
	    && (len < 0 || (ਪੂਰਨ)(ptr - ਪਾਠ) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ਜੇ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (ਪੂਰਨ)((ਪਾਠ + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ਜੇ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    ਨਹੀਂਤਾਂ	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (ਪੂਰਨ)((ਪਾਠ + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
		ਜੇ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ਜੇ (len >= 0 && (ਪੂਰਨ)(ptr - ਪਾਠ) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(ਪੂਰਨ)((ਪਾਠ + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		ਨਹੀਂਤਾਂ
		    prev_c = u8c;
#ਜੇ_ਅੰਤ
		ਜੇ (ਖਾਨਾ + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_ਅਗਲਾ;
	force_redraw_ਅਗਲਾ = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	ਪੂਰਨ cell_attr = attr;
	ਜੇ (screen_pum_blend > 0 && pum_bg_attrs != ਨਿਰਾ
		&& ਪੰਕਤੀ >= pum_bg_ਸਿਖਰ && ਪੰਕਤੀ < pum_bg_bot
		&& ਖਾਨਾ < pum_bg_ਖਾਨੇ_ਗਿਣਤੀ)
	{
	    ਪੂਰਨ soff = (ਪੰਕਤੀ - pum_bg_ਸਿਖਰ) * pum_bg_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_ਸਤਰਾਂ[off] != c
		|| (mbyte_cells == 2
		    && Screen_ਸਤਰਾਂ[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_ਸਤਰਾਂ_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_ਸਤਰਾਂ_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_ਸਰਗਰਮ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ)
#ਜੇ_ਅੰਤ
		;

	ਜੇ ((need_redraw || force_redraw_this) && !skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ))
	{
#ਜੇ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ਜੇ (need_redraw && Screen_ਸਤਰਾਂ[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.ਵਿੱਚ_use
# endif
# ਜੇ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    ਪਦ_is_xterm
# endif
		    ))
	    {
		ਪੂਰਨ	n = ScreenAttrs[off];

		ਜੇ (n > HL_ALL)
		    n = syn_attr2attr(n);
		ਜੇ (n & HL_BOLD)
		    force_redraw_ਅਗਲਾ = TRUE;
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ਜੇ (gui.directx_enabled && need_redraw)
		force_redraw_ਅਗਲਾ = TRUE;
#ਜੇ_ਅੰਤ
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ਜੇ (clear_ਅਗਲਾ_cell)
		clear_ਅਗਲਾ_cell = FALSE;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= ਪਾਠ + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_ਅਗਲਾ_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ਜੇ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_ਸਤਰਾਂ[off + mbyte_blen] = 0;
	    Screen_ਸਤਰਾਂ[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off] = -1;
	    ਜੇ (enc_utf8)
	    {
		ਜੇ (c < 0x80 && u8cc[0] == 0)
		    Screen_ਸਤਰਾਂ_UC[off] = 0;
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ	    i;

		    Screen_ਸਤਰਾਂ_UC[off] = u8c;
		    ਲਈ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ਸਤਰਾਂ_C[i][off] = u8cc[i];
			ਜੇ (u8cc[i] == 0)
			    ਰੋਕੋ;
		    }
		}
		ਜੇ (mbyte_cells == 2)
		{
		    Screen_ਸਤਰਾਂ[off + 1] = 0;
		    Screen_ਸਤਰਾਂ_UC[off + 1] = 0;
		    ਲਈ (ਪੂਰਨ ci = 0; ci < Screen_mco; ++ci)
			Screen_ਸਤਰਾਂ_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off + 1] = -1;
		}
		screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (mbyte_cells == 2)
	    {
		Screen_ਸਤਰਾਂ[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		Screen_ਖਾਨੇ_ਗਿਣਤੀ[off + 1] = -1;
		screen_char_2(off, ਪੰਕਤੀ, ਖਾਨਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
	    }
	    ਨਹੀਂਤਾਂ
		screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
	}
	ਜੇ (has_mbyte)
	{
	    off += mbyte_cells;
	    ਖਾਨਾ += mbyte_cells;
	    ptr += mbyte_blen;
	    ਜੇ (clear_ਅਗਲਾ_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ++off;
	    ++ਖਾਨਾ;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ਜੇ (force_redraw_ਅਗਲਾ && ਖਾਨਾ < screen_Columns && !skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ))
    {
	ਜੇ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, ਪੰਕਤੀ, ਖਾਨਾ);
	ਨਹੀਂਤਾਂ
	    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
    }
}

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    ਖਾਲੀ
ਸ਼ੁਰੂ_ਖੋਜ_hl(ਖਾਲੀ)
{
    ਜੇ (!p_hls || no_hlsearch)
	ਵਾਪਸ;

    ਸਿਰਾ_ਖੋਜ_hl();  // just in case it wasn't called before
    ਆਖਰੀ_pat_prog(&screen_ਖੋਜ_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    ਖਾਲੀ
ਸਿਰਾ_ਖੋਜ_hl(ਖਾਲੀ)
{
    ਜੇ (screen_ਖੋਜ_hl.rm.regprog == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_regfree(screen_ਖੋਜ_hl.rm.regprog);
    screen_ਖੋਜ_hl.rm.regprog = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
screen_ਸ਼ੁਰੂ_highlight(ਪੂਰਨ attr)
{
    attrentry_T *aep = ਨਿਰਾ;

    screen_attr = attr;
    ਜੇ (!ਭਰਿਆ_screen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    || !termcap_ਸਰਗਰਮ
#ਜੇ_ਅੰਤ
       )
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਅੱਖਰ	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	ਬਾਹਰਲਾ_STR(buf);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (attr > HL_ALL)				// special HL attr.
    {
	ਜੇ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ਨਹੀਂਤਾਂ
	    aep = syn_ਪਦ_attr2entry(attr);
	ਜੇ (aep == ਨਿਰਾ)	    // did ":syntax clear"
	    attr = 0;
	ਨਹੀਂਤਾਂ
	    attr = aep->ae_attr;
    }
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ਜੇ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	ਪੂਰਨ	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ਜੇ (p_tgc)
	{
	    ਜੇ (aep == ਨਿਰਾ || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		ਪਦ_fg_rgb_color(defguifg);
	    ਜੇ (aep == ਨਿਰਾ || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		ਪਦ_bg_rgb_color(defguibg);
	}
	ਨਹੀਂਤਾਂ ਜੇ (t_colors >= 256)
	{
	    ਜੇ (aep == ਨਿਰਾ || aep->ae_u.cterm.fg_color == 0)
		ਪਦ_fg_color(defctermfg);
	    ਜੇ (aep == ਨਿਰਾ || aep->ae_u.cterm.bg_color == 0)
		ਪਦ_bg_color(defctermbg);
	}
    }
#ਜੇ_ਅੰਤ
    ਜੇ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	ਬਾਹਰਲਾ_str(T_MD);
    ਨਹੀਂਤਾਂ ਜੇ (aep != ਨਿਰਾ && cterm_ਸਾਧਾਰਨ_fg_bold && (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#ਜੇ_ਅੰਤ
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	ਬਾਹਰਲਾ_str(T_ME);
    ਜੇ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	ਬਾਹਰਲਾ_str(T_SO);
    ਜੇ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	ਬਾਹਰਲਾ_str(T_UCS);
    ਜੇ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	ਬਾਹਰਲਾ_str(T_USS);
    ਜੇ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	ਬਾਹਰਲਾ_str(T_DS);
    ਜੇ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	ਬਾਹਰਲਾ_str(T_CDS);
    ਜੇ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	ਬਾਹਰਲਾ_str(T_US);
    ਜੇ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	ਬਾਹਰਲਾ_str(T_CZH);
    ਜੇ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	ਬਾਹਰਲਾ_str(T_MR);
    ਜੇ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	ਬਾਹਰਲਾ_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ਜੇ (aep != ਨਿਰਾ)
    {
	ਜੇ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		ਪਦ_font(aep->ae_u.cterm.font);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ਜੇ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ਜੇ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		ਪਦ_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (t_colors > 1)
	    {
		ਜੇ (aep->ae_u.cterm.fg_color)
		    ਪਦ_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	ਜੇ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ਜੇ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		ਪਦ_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (t_colors > 1)
	    {
		ਜੇ (aep->ae_u.cterm.bg_color)
		    ਪਦ_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	ਜੇ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ਜੇ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		ਪਦ_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (t_colors > 1)
	    {
		ਜੇ (aep->ae_u.cterm.ul_color)
		    ਪਦ_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ਜੇ (!IS_CTERM)
	{
	    ਜੇ (aep->ae_u.ਪਦ.ਸ਼ੁਰੂ != ਨਿਰਾ)
		ਬਾਹਰਲਾ_str(aep->ae_u.ਪਦ.ਸ਼ੁਰੂ);
	}
    }
}

    ਖਾਲੀ
screen_stop_highlight(ਖਾਲੀ)
{
    ਪੂਰਨ	    do_ME = FALSE;	    // output T_ME code
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ਪੂਰਨ	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (screen_attr != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
			&& termcap_ਸਰਗਰਮ
#ਜੇ_ਅੰਤ
					   )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    ਅੱਖਰ	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    ਬਾਹਰਲਾ_STR(buf);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    ਪੂਰਨ is_under;

	    ਜੇ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ਜੇ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ਜੇ (aep != ਨਿਰਾ && ((
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#ਜੇ_ਅੰਤ
				aep->ae_u.cterm.fg_color) || (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#ਜੇ_ਅੰਤ
				aep->ae_u.cterm.bg_color)))
			do_ME = TRUE;
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ਜੇ (use_vtp())
		    {
			ਜੇ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ਜੇ (!do_ME && do_ME_fg)
			    ਬਾਹਰਲਾ_str((char_u *)"\033|39m"); // restore FG
			ਜੇ (!do_ME && do_ME_bg)
			    ਬਾਹਰਲਾ_str((char_u *)"\033|49m"); // restore BG
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Process FG and BG at once.
			ਜੇ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ
		{
		    aep = syn_ਪਦ_attr2entry(screen_attr);
		    ਜੇ (aep != ਨਿਰਾ && aep->ae_u.ਪਦ.stop != ਨਿਰਾ)
		    {
			ਜੇ (STRCMP(aep->ae_u.ਪਦ.stop, T_ME) == 0)
			    do_ME = TRUE;
			ਨਹੀਂਤਾਂ
			    ਬਾਹਰਲਾ_str(aep->ae_u.ਪਦ.stop);
		    }
		}
		ਜੇ (aep == ਨਿਰਾ)	    // did ":syntax clear"
		    screen_attr = 0;
		ਨਹੀਂਤਾਂ
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ਜੇ (screen_attr & HL_STANDOUT)
	    {
		ਜੇ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		ਨਹੀਂਤਾਂ
		    ਬਾਹਰਲਾ_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ਜੇ (is_under && *T_UCE != NUL)
	    {
		ਜੇ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		ਨਹੀਂਤਾਂ
		    ਬਾਹਰਲਾ_str(T_UCE);
	    }
	    ਜੇ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ਜੇ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		ਨਹੀਂਤਾਂ
		    ਬਾਹਰਲਾ_str(T_UE);
	    }
	    ਜੇ (screen_attr & HL_ITALIC)
	    {
		ਜੇ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		ਨਹੀਂਤਾਂ
		    ਬਾਹਰਲਾ_str(T_CZR);
	    }
	    ਜੇ (screen_attr & HL_STRIKETHROUGH)
	    {
		ਜੇ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		ਨਹੀਂਤਾਂ
		    ਬਾਹਰਲਾ_str(T_STE);
	    }
	    ਜੇ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		ਬਾਹਰਲਾ_str(T_ME);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    ਜੇ (p_tgc)
	    {
		ਜੇ (cterm_ਸਾਧਾਰਨ_fg_gui_color != INVALCOLOR)
		    ਪਦ_fg_rgb_color(cterm_ਸਾਧਾਰਨ_fg_gui_color);
		ਜੇ (cterm_ਸਾਧਾਰਨ_bg_gui_color != INVALCOLOR)
		    ਪਦ_bg_rgb_color(cterm_ਸਾਧਾਰਨ_bg_gui_color);
		ਜੇ (cterm_ਸਾਧਾਰਨ_ul_gui_color != INVALCOLOR)
		    ਪਦ_ul_rgb_color(cterm_ਸਾਧਾਰਨ_ul_gui_color);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		ਜੇ (t_colors > 1)
		{
		    // set Normal cterm colors
		    ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color != 0)
			ਪਦ_fg_color(cterm_ਸਾਧਾਰਨ_fg_color - 1);
		    ਜੇ (cterm_ਸਾਧਾਰਨ_bg_color != 0)
			ਪਦ_bg_color(cterm_ਸਾਧਾਰਨ_bg_color - 1);
		    ਜੇ (cterm_ਸਾਧਾਰਨ_ul_color != 0)
			ਪਦ_ul_color(cterm_ਸਾਧਾਰਨ_ul_color - 1);
		    ਜੇ (cterm_ਸਾਧਾਰਨ_fg_bold)
			ਬਾਹਰਲਾ_str(T_MD);
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
    ਖਾਲੀ
reset_cterm_colors(ਖਾਲੀ)
{
    ਜੇ (!IS_CTERM)
	ਵਾਪਸ;

    // set Normal cterm colors
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
    ਜੇ (p_tgc ? (cterm_ਸਾਧਾਰਨ_fg_gui_color != INVALCOLOR
		|| cterm_ਸਾਧਾਰਨ_bg_gui_color != INVALCOLOR)
	    : (cterm_ਸਾਧਾਰਨ_fg_color > 0 || cterm_ਸਾਧਾਰਨ_bg_color > 0))
#ਨਹੀਂਤਾਂ
	ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color > 0 || cterm_ਸਾਧਾਰਨ_bg_color > 0)
#ਜੇ_ਅੰਤ
	{
	    ਬਾਹਰਲਾ_str(T_OP);
	    screen_attr = -1;
	}
    ਜੇ (cterm_ਸਾਧਾਰਨ_fg_bold)
    {
	ਬਾਹਰਲਾ_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    ਖਾਲੀ
screen_char(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ਜੇ (ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    mark_dirty_region_for_images(ਪੰਕਤੀ, ਖਾਨਾ, 1, 1);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ਜੇ (popup_is_under_opacity(ਪੰਕਤੀ, ਖਾਨਾ))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
		&& utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) == 2
		&& ਖਾਨਾ + 1 < screen_Columns
		&& !popup_is_under_opacity(ਪੰਕਤੀ, ਖਾਨਾ + 1))
	{
	    ਪੂਰਨ off2 = off + 1;
	    Screen_ਸਤਰਾਂ[off2] = ' ';
	    Screen_ਸਤਰਾਂ_UC[off2] = 0;
	    screen_char(off2, ਪੰਕਤੀ, ਖਾਨਾ + 1);
	}
	mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ);
	ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
		&& utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) == 2
		&& ਖਾਨਾ + 1 < screen_Columns
		&& popup_is_under_opacity(ਪੰਕਤੀ, ਖਾਨਾ + 1))
	    mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + 1);
	screen_cur_ਖਾਨਾ = 9999;
	ਵਾਪਸ;
    }
    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
	    && utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) == 2
	    && ਖਾਨਾ + 1 < screen_Columns
	    && popup_is_under_opacity(ਪੰਕਤੀ, ਖਾਨਾ + 1))
    {
	mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ);
	mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + 1);
	screen_cur_ਖਾਨਾ = 9999;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ਜੇ (*T_XN == NUL
	    && ਪੰਕਤੀ == screen_ਪੰਕਤੀਆਂ - 1 && ਖਾਨਾ == screen_Columns - 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#ਜੇ_ਅੰਤ
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ਖਾਨੇ_ਗਿਣਤੀ[off] = -1;
	ਵਾਪਸ;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ਜੇ (screen_char_attr != 0)
	attr = screen_char_attr;
    ਨਹੀਂਤਾਂ
	attr = ScreenAttrs[off];
    ਜੇ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(ਪੰਕਤੀ, ਖਾਨਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ);
    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
	    && utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) == 2)
	unmark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + 1);
#ਜੇ_ਅੰਤ

    ਜੇ (screen_attr != attr)
	screen_ਸ਼ੁਰੂ_highlight(attr);

    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    !gui.ਵਿੱਚ_use &&
#ਜੇ_ਅੰਤ
	    get_cellwidth(Screen_ਸਤਰਾਂ_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    ਬਾਹਰਲਾ_str((char_u *)"  ");
	    ਪਦ_windgoto(ਪੰਕਤੀ, ਖਾਨਾ);
	    screen_cur_ਖਾਨਾ = 9999;
	}
	ਨਹੀਂਤਾਂ ਜੇ (utf_ambiguous_ਚੌੜਾਈ(Screen_ਸਤਰਾਂ_UC[off]))
	{
	    ਜੇ (*p_ambw == 'd'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		ਬਾਹਰਲਾ_str((char_u *)"  ");
		ਪਦ_windgoto(ਪੰਕਤੀ, ਖਾਨਾ);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_ਖਾਨਾ = 9999;
	}
	ਨਹੀਂਤਾਂ ਜੇ (utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]) > 1)
	    ++screen_cur_ਖਾਨਾ;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	ਬਾਹਰਲਾ_str(buf);
    }
    ਨਹੀਂਤਾਂ
    {
	ਬਾਹਰਲਾ_flush_check();
	ਬਾਹਰਲਾ_char(Screen_ਸਤਰਾਂ[off]);
	// double-byte character in single-width cell
	ਜੇ (enc_dbcs == DBCS_JPNU && Screen_ਸਤਰਾਂ[off] == 0x8e)
	    ਬਾਹਰਲਾ_char(ScreenLines2[off]);
    }

    screen_cur_ਖਾਨਾ++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    ਸਥਿਰ ਖਾਲੀ
screen_char_2(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    // Check for illegal values (could be wrong when screen was resized).
    ਜੇ (off + 1 >= (ਬਿਨਾਨਿਸ਼ਾਨੀ)(screen_ਪੰਕਤੀਆਂ * screen_Columns))
	ਵਾਪਸ;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ਜੇ (ਪੰਕਤੀ == screen_ਪੰਕਤੀਆਂ - 1 && ਖਾਨਾ >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ਖਾਨੇ_ਗਿਣਤੀ[off] = -1;
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ਜੇ (popup_is_under_opacity(ਪੰਕਤੀ, ਖਾਨਾ))
    {
	mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ);
	mark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + 1);
	screen_cur_ਖਾਨਾ = 9999;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
    ਬਾਹਰਲਾ_char(Screen_ਸਤਰਾਂ[off + 1]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    unmark_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ + 1);
#ਜੇ_ਅੰਤ
    ++screen_cur_ਖਾਨਾ;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    ਖਾਲੀ
screen_draw_rectangle(
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		ਉਚਾਈ,
    ਪੂਰਨ		ਚੌੜਾਈ,
    ਪੂਰਨ		invert,
    ਪੂਰਨ		force)
{
    ਪੂਰਨ		r, c;
    ਪੂਰਨ		off;
    ਪੂਰਨ		max_off;

    // Can't use ScreenLines unless initialized
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (invert)
	screen_char_attr = HL_INVERSE;
    ਲਈ (r = ਪੰਕਤੀ; r < ਪੰਕਤੀ + ਉਚਾਈ; ++r)
    {
	off = ਸਤਰ_ਵਿੱਥ[r];
	max_off = off + screen_Columns;
	ਲਈ (c = ਖਾਨਾ; c < ਖਾਨਾ + ਚੌੜਾਈ; ++c)
	{
	    ਜੇ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ਜੇ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ਜੇ (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_ਖੰਡ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਸਿਰਾ, win_T *wp)
{
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		ਚੌੜਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    clip_may_clear_selection(ਪੰਕਤੀ, ਸਿਰਾ - 1);
#ਜੇ_ਅੰਤ

    ਜੇ (wp == ਨਿਰਾ)
    {
	ਖਾਨਾ = firstwin->w_wincol;
	ਚੌੜਾਈ = topframe->fr_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਖਾਨਾ = wp->w_wincol;
	ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;
    }
    screen_draw_rectangle(ਪੰਕਤੀ, ਖਾਨਾ, ਸਿਰਾ - ਪੰਕਤੀ, ਚੌੜਾਈ, FALSE, FALSE);
}

    ਖਾਲੀ
space_ਮੰਜ਼ਿਲ_screenline(ਪੂਰਨ off, ਪੂਰਨ attr)
{
    Screen_ਸਤਰਾਂ[off] = ' ';
    ScreenAttrs[off] = attr;
    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off] = -1;
    ਜੇ (enc_utf8)
	Screen_ਸਤਰਾਂ_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    ਖਾਲੀ
screen_fill(
	ਪੂਰਨ	ਸ਼ੁਰੂ_ਪੰਕਤੀ,
	ਪੂਰਨ	ਸਿਰਾ_ਪੰਕਤੀ,
	ਪੂਰਨ	ਸ਼ੁਰੂ_ਖਾਨਾ,
	ਪੂਰਨ	ਸਿਰਾ_ਖਾਨਾ,
	ਪੂਰਨ	c1,
	ਪੂਰਨ	c2,
	ਪੂਰਨ	attr)
{
    ਪੂਰਨ	    ਪੰਕਤੀ;
    ਪੂਰਨ	    ਖਾਨਾ;
    ਪੂਰਨ	    off;
    ਪੂਰਨ	    ਸਿਰਾ_off;
    ਪੂਰਨ	    did_delete;
    ਪੂਰਨ	    c;
    ਪੂਰਨ	    norm_ਪਦ;
#ਜੇ defined(FEAT_GUI) || defined(UNIX)
    ਪੂਰਨ	    force_ਅਗਲਾ = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (ਸਿਰਾ_ਪੰਕਤੀ > screen_ਪੰਕਤੀਆਂ)		// safety check
	ਸਿਰਾ_ਪੰਕਤੀ = screen_ਪੰਕਤੀਆਂ;
    ਜੇ (ਸਿਰਾ_ਖਾਨਾ > screen_Columns)	// safety check
	ਸਿਰਾ_ਖਾਨਾ = screen_Columns;
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ
	    || ਸ਼ੁਰੂ_ਪੰਕਤੀ < 0		// should not happen
	    || ਸ਼ੁਰੂ_ਪੰਕਤੀ >= ਸਿਰਾ_ਪੰਕਤੀ
	    || ਸ਼ੁਰੂ_ਖਾਨਾ >= ਸਿਰਾ_ਖਾਨਾ)	// nothing to do
	ਵਾਪਸ;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_ਪਦ = (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    !gui.ਵਿੱਚ_use &&
#ਜੇ_ਅੰਤ
	    !IS_CTERM);
    ਲਈ (ਪੰਕਤੀ = ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < ਸਿਰਾ_ਪੰਕਤੀ; ++ਪੰਕਤੀ)
    {
	ਜੇ (has_mbyte
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ > 0 && mb_fix_ਖਾਨਾ(ਸ਼ੁਰੂ_ਖਾਨਾ, ਪੰਕਤੀ) != ਸ਼ੁਰੂ_ਖਾਨਾ)
	    {
		// start_col is the right half, clear the left half
		ਪੂਰਨ ਖੱਬਾ_attr = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਸ਼ੁਰੂ_ਖਾਨਾ - 1];
		screen_puts_len((char_u *)" ", 1, ਪੰਕਤੀ, ਸ਼ੁਰੂ_ਖਾਨਾ - 1, ਖੱਬਾ_attr);
	    }
	    ਜੇ (ਸਿਰਾ_ਖਾਨਾ < screen_Columns && mb_fix_ਖਾਨਾ(ਸਿਰਾ_ਖਾਨਾ, ਪੰਕਤੀ) != ਸਿਰਾ_ਖਾਨਾ)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		ਪੂਰਨ ਸੱਜਾ_attr = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਸਿਰਾ_ਖਾਨਾ];
		screen_puts_len((char_u *)" ", 1, ਪੰਕਤੀ, ਸਿਰਾ_ਖਾਨਾ, ਸੱਜਾ_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ਜੇ (c2 == ' '
		&& ਸਿਰਾ_ਖਾਨਾ == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_ਪਦ
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(ਪੰਕਤੀ,
						ਸ਼ੁਰੂ_ਖਾਨਾ, ਸਿਰਾ_ਖਾਨਾ)
#ਜੇ_ਅੰਤ
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ;
	    ਜੇ (c1 != ' ')			// don't clear first char
		++ਖਾਨਾ;

	    off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਖਾਨਾ;
	    ਸਿਰਾ_off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਸਿਰਾ_ਖਾਨਾ;

	    // skip blanks (used often, keep it fast!)
	    ਜੇ (enc_utf8)
		ਜਦੋਂ (off < ਸਿਰਾ_off && Screen_ਸਤਰਾਂ[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_ਸਤਰਾਂ_UC[off] == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			  && !is_suppressed_cell(ਪੰਕਤੀ,
					      (ਪੂਰਨ)(off - ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ]))
#ਜੇ_ਅੰਤ
			  )
		    ++off;
	    ਨਹੀਂਤਾਂ
		ਜਦੋਂ (off < ਸਿਰਾ_off && Screen_ਸਤਰਾਂ[off] == ' '
						     && ScreenAttrs[off] == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			  && !is_suppressed_cell(ਪੰਕਤੀ,
					      (ਪੂਰਨ)(off - ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ]))
#ਜੇ_ਅੰਤ
			  )
		    ++off;
	    ਜੇ (off < ਸਿਰਾ_off)		// something to be cleared
	    {
		ਖਾਨਾ = off - ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ];
		screen_stop_highlight();
		ਪਦ_windgoto(ਪੰਕਤੀ, ਖਾਨਾ);// clear rest of this screen line
		ਬਾਹਰਲਾ_str(T_CE);
		screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
		ਖਾਨਾ = ਸਿਰਾ_ਖਾਨਾ - ਖਾਨਾ;
		ਜਦੋਂ (ਖਾਨਾ--)		// clear chars in ScreenLines
		{
		    space_ਮੰਜ਼ਿਲ_screenline(off, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(ਪੰਕਤੀ, (ਪੂਰਨ)(off - ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ]));
#ਜੇ_ਅੰਤ
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਸ਼ੁਰੂ_ਖਾਨਾ;
	c = c1;
	ਲਈ (ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ < ਸਿਰਾ_ਖਾਨਾ; ++ਖਾਨਾ)
	{
	    ਜੇ ((Screen_ਸਤਰਾਂ[off] != c
		    || (enc_utf8 && (ਪੂਰਨ)Screen_ਸਤਰਾਂ_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ਜੇ defined(FEAT_GUI) || defined(UNIX)
		    || force_ਅਗਲਾ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(ਪੰਕਤੀ, ਖਾਨਾ)
#ਜੇ_ਅੰਤ
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(ਪੰਕਤੀ, ਖਾਨਾ))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		ਜੇ (screen_opacity_popup != ਨਿਰਾ && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ਜੇ (Screen_ਸਤਰਾਂ[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_ਸਤਰਾਂ_UC[off - 1] != 0
			    && utf_char2cells(Screen_ਸਤਰਾਂ_UC[off - 1]) == 2)
			ਜਾਓ ਅਗਲਾ_ਖਾਨਾ;
		}
		ਜੇ (screen_opacity_popup != ਨਿਰਾ && c == ' '
						       && Screen_ਸਤਰਾਂ[off] != 0)
		{
		    ਪੂਰਨ bg_char_cells = 1;
		    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_ਸਤਰਾਂ_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ਜੇ (bg_char_cells == 2)
		    {
			ਜੇ (ਖਾਨਾ + 1 >= ਸਿਰਾ_ਖਾਨਾ)
			    // At the edge, skip wide char.
			    ਜਾਓ skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ਜੇ (c2 != ' ')
			    ਜਾਓ skip_opacity_fill;
		    }

		    ਪੂਰਨ popup_attr = get_win_attr(screen_opacity_popup);
		    ਪੂਰਨ blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
		    ਜਾਓ ਅਗਲਾ_ਖਾਨਾ;
		}
skip_opacity_fill:
#ਜੇ_ਅੰਤ
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		ਜੇ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != ਨਿਰਾ
			&& ਪੰਕਤੀ >= pum_bg_ਸਿਖਰ && ਪੰਕਤੀ < pum_bg_bot
			&& ਖਾਨਾ < pum_bg_ਖਾਨੇ_ਗਿਣਤੀ)
		{
		    ਪੂਰਨ soff = (ਪੰਕਤੀ - pum_bg_ਸਿਖਰ) * pum_bg_ਖਾਨੇ_ਗਿਣਤੀ + ਖਾਨਾ;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ
			    && ਖਾਨਾ > ਸ਼ੁਰੂ_ਖਾਨਾ
			    && pum_bg_ਸਤਰਾਂ_UC[soff] == 0
			    && pum_bg_ਸਤਰਾਂ_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_ਸਤਰਾਂ_UC[soff - 1]) == 2)
		    {
			Screen_ਸਤਰਾਂ[off] = 0;
			ਜੇ (Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ)
			    Screen_ਸਤਰਾਂ_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			ਜਾਓ ਅਗਲਾ_ਖਾਨਾ;
		    }

		    ਪੂਰਨ underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ
			    && ((pum_bg_ਸਤਰਾਂ_UC[soff] != 0
				    && utf_char2cells(pum_bg_ਸਤਰਾਂ_UC[soff]) == 2
				    && ਖਾਨਾ + 1 >= ਸਿਰਾ_ਖਾਨਾ)
				|| (ਖਾਨਾ == ਸ਼ੁਰੂ_ਖਾਨਾ
				    && pum_bg_ਸਤਰਾਂ_UC[soff] == 0
				    && ਖਾਨਾ > 0
				    && pum_bg_ਸਤਰਾਂ_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_ਸਤਰਾਂ_UC[soff - 1])
								       == 2)))
		    {
			Screen_ਸਤਰਾਂ[off] = ' ';
			ਜੇ (Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ)
			    Screen_ਸਤਰਾਂ_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
			ਜਾਓ ਅਗਲਾ_ਖਾਨਾ;
		    }

		    // Restore underlying character so text shows through.
		    Screen_ਸਤਰਾਂ[off] = pum_bg_ਸਤਰਾਂ[soff];
		    ਜੇ (enc_utf8 && pum_bg_ਸਤਰਾਂ_UC != ਨਿਰਾ
						&& Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ)
		    {
			ਪੂਰਨ k;
			Screen_ਸਤਰਾਂ_UC[off] = pum_bg_ਸਤਰਾਂ_UC[soff];
			ਲਈ (k = 0; k < MAX_MCO; ++k)
			    ਜੇ (pum_bg_ਸਤਰਾਂ_C[k] != ਨਿਰਾ
						    && Screen_ਸਤਰਾਂ_C[k] != ਨਿਰਾ)
				Screen_ਸਤਰਾਂ_C[k][off] = pum_bg_ਸਤਰਾਂ_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
		    ਜਾਓ ਅਗਲਾ_ਖਾਨਾ;
		}
#ਜੇ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ਜੇ (
# ifdef FEAT_GUI
			gui.ਵਿੱਚ_use
# endif
# ਜੇ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			ਪਦ_is_xterm
# endif
		   )
		{
		    ਜੇ (Screen_ਸਤਰਾਂ[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_ਅਗਲਾ = TRUE;
		    ਨਹੀਂਤਾਂ
			force_ਅਗਲਾ = FALSE;
		}
#ਜੇ_ਅੰਤ // FEAT_GUI || defined(UNIX)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ਜੇ (gui.directx_enabled)
		    force_ਅਗਲਾ = TRUE;
#ਜੇ_ਅੰਤ
		Screen_ਸਤਰਾਂ[off] = c;
		ਜੇ (enc_utf8)
		{
		    ਜੇ (c >= 0x80)
		    {
			Screen_ਸਤਰਾਂ_UC[off] = c;
			Screen_ਸਤਰਾਂ_C[0][off] = 0;
			Screen_ਸਤਰਾਂ[off] = 0x80; // avoid storing zero
		    }
		    ਨਹੀਂਤਾਂ
			Screen_ਸਤਰਾਂ_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ਜੇ (!did_delete || c != ' ')
		    screen_char(off, ਪੰਕਤੀ, ਖਾਨਾ);
	    }
ਅਗਲਾ_ਖਾਨਾ:
	    Screen_ਖਾਨੇ_ਗਿਣਤੀ[off] = -1;
	    ++off;
	    ਜੇ (ਖਾਨਾ == ਸ਼ੁਰੂ_ਖਾਨਾ)
	    {
		ਜੇ (did_delete)
		    ਰੋਕੋ;
		c = c2;
	    }
	}
	ਜੇ (ਸਿਰਾ_ਖਾਨਾ == Columns)
	    ਸਤਰ_Wraps[ਪੰਕਤੀ] = FALSE;
	ਜੇ (ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ਜੇ (((ਸ਼ੁਰੂ_ਖਾਨਾ == 0 && ਸਿਰਾ_ਖਾਨਾ == Columns)
			|| (ਸ਼ੁਰੂ_ਖਾਨਾ == cmdline_ਖਾਨਾ_off
			&& ਸਿਰਾ_ਖਾਨਾ == cmdline_ਖਾਨਾ_off + cmdline_ਚੌੜਾਈ))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#ਜੇ_ਅੰਤ
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ == 0 || ਸ਼ੁਰੂ_ਖਾਨਾ == cmdline_ਖਾਨਾ_off)
		ਬਹੁਲਕ_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    ਖਾਲੀ
check_for_delay(ਪੂਰਨ check_ਸੁਨੇਹਾ_scroll)
{
    ਜੇ ((emsg_on_display || (check_ਸੁਨੇਹਾ_scroll && ਸੁਨੇਹਾ_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !ਵਿੱਚ_assert_fails)
    {
	ਬਾਹਰਲਾ_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ਜੇ (check_ਸੁਨੇਹਾ_scroll)
	    ਸੁਨੇਹਾ_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_Tab_ਪੰਨਾ_Idxs(ਖਾਲੀ)
{
    ਪੂਰਨ		scol;

    ਲਈ (scol = 0; scol < Columns; ++scol)
	Tab_ਪੰਨਾ_Idxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    ਪੂਰਨ
screen_valid(ਪੂਰਨ doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    ਵਾਪਸ (Screen_ਸਤਰਾਂ != ਨਿਰਾ);
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
    ਖਾਲੀ
screenalloc(ਪੂਰਨ doclear)
{
    ਪੂਰਨ		    new_ਪੰਕਤੀ, old_ਪੰਕਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		    old_ਪੰਕਤੀਆਂ_2;
#ਜੇ_ਅੰਤ
    win_T	    *wp;
    ਪੂਰਨ		    outofmem = FALSE;
    ਪੂਰਨ		    len;
    schar_T	    *new_Screen_ਸਤਰਾਂ;
    u8char_T	    *new_Screen_ਸਤਰਾਂ_UC = ਨਿਰਾ;
    u8char_T	    *new_Screen_ਸਤਰਾਂ_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = ਨਿਰਾ;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_Screen_ਖਾਨੇ_ਗਿਣਤੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	    *new_ਸਤਰ_ਵਿੱਥ;
    char_u	    *new_ਸਤਰ_Wraps;
    ਛੋਟਾ	    *new_Tab_ਪੰਨਾ_Idxs;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਛੋਟਾ	    *new_popup_ਨਕਾਬ;
    ਛੋਟਾ	    *new_popup_ਨਕਾਬ_ਅਗਲਾ;
    ਅੱਖਰ	    *new_popup_transparent;
#ਜੇ_ਅੰਤ
    tabpage_T	    *tp;
    ਸਥਿਰ ਪੂਰਨ	    entered = FALSE;		// avoid recursiveness
    ਸਥਿਰ ਪੂਰਨ	    ਪੂਰਾ_outofmem_ਸੁਨੇਹਾ = FALSE;	// did outofmem message
    ਪੂਰਨ		    retry_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		    ਮਿਲਿਆ_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ਜੇ ((Screen_ਸਤਰਾਂ != ਨਿਰਾ
		&& ਪੰਕਤੀਆਂ == screen_ਪੰਕਤੀਆਂ
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != ਨਿਰਾ)
		&& p_mco == Screen_mco)
	    || ਪੰਕਤੀਆਂ == 0
	    || Columns == 0
	    || (!ਭਰਿਆ_screen && Screen_ਸਤਰਾਂ == ਨਿਰਾ))
	ਵਾਪਸ;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ਜੇ (entered)
	ਵਾਪਸ;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_ਤਾਲਾ_screen();  // be safe, put it here
#ਜੇ_ਅੰਤ

    comp_ਖਾਨਾ();		// recompute columns for shown command and ruler

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
    ਲਈ (ਪੂਰਨ i = 0; i < AUCMD_WIN_ਗਿਣਤੀ; ++i)
	ਜੇ (aucmd_win[i].auc_win != ਨਿਰਾ)
	    win_free_lsize(aucmd_win[i].auc_win);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	    win_free_lsize(wp);
#ਜੇ_ਅੰਤ

    new_Screen_ਸਤਰਾਂ = LALLOC_MULT(schar_T, (ਪੰਕਤੀਆਂ + 1) * Columns);
    vim_memset(new_Screen_ਸਤਰਾਂ_C, 0, ਆਕਾਰ(u8char_T *) * MAX_MCO);
    ਜੇ (enc_utf8)
    {
	new_Screen_ਸਤਰਾਂ_UC = LALLOC_MULT(u8char_T, (ਪੰਕਤੀਆਂ + 1) * Columns);
	ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
	    new_Screen_ਸਤਰਾਂ_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (ਪੰਕਤੀਆਂ + 1) * Columns);
    }
    ਜੇ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (ਪੰਕਤੀਆਂ + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (ਪੰਕਤੀਆਂ + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_Screen_ਖਾਨੇ_ਗਿਣਤੀ = LALLOC_CLEAR_MULT(colnr_T, (ਪੰਕਤੀਆਂ + 1) * Columns);
    new_ਸਤਰ_ਵਿੱਥ = LALLOC_MULT(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਪੰਕਤੀਆਂ);
    new_ਸਤਰ_Wraps = LALLOC_MULT(char_u, ਪੰਕਤੀਆਂ);
    new_Tab_ਪੰਨਾ_Idxs = LALLOC_MULT(ਛੋਟਾ, Columns);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    new_popup_ਨਕਾਬ = LALLOC_MULT(ਛੋਟਾ, ਪੰਕਤੀਆਂ * Columns);
    new_popup_ਨਕਾਬ_ਅਗਲਾ = LALLOC_MULT(ਛੋਟਾ, ਪੰਕਤੀਆਂ * Columns);
    new_popup_transparent = LALLOC_MULT(ਅੱਖਰ, ਪੰਕਤੀਆਂ * Columns);
#ਜੇ_ਅੰਤ

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ਜੇ (win_alloc_ਸਤਰਾਂ(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ਜਾਓ give_ਉੱਪਰ;
	}
    }
    ਲਈ (ਪੂਰਨ i = 0; i < AUCMD_WIN_ਗਿਣਤੀ; ++i)
	ਜੇ (aucmd_win[i].auc_win != ਨਿਰਾ
		&& aucmd_win[i].auc_win->w_ਸਤਰਾਂ == ਨਿਰਾ
		&& win_alloc_ਸਤਰਾਂ(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    ਰੋਕੋ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ਜੇ (win_alloc_ਸਤਰਾਂ(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ਜਾਓ give_ਉੱਪਰ;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	    ਜੇ (win_alloc_ਸਤਰਾਂ(wp) == FAIL)
	    {
		outofmem = TRUE;
		ਜਾਓ give_ਉੱਪਰ;
	    }
#ਜੇ_ਅੰਤ

give_ਉੱਪਰ:
    ਮਿਲਿਆ_null = FALSE;
    ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
	ਜੇ (new_Screen_ਸਤਰਾਂ_C[i] == ਨਿਰਾ)
	{
	    ਮਿਲਿਆ_null = TRUE;
	    ਰੋਕੋ;
	}
    ਜੇ (new_Screen_ਸਤਰਾਂ == ਨਿਰਾ
	    || (enc_utf8 && (new_Screen_ਸਤਰਾਂ_UC == ਨਿਰਾ || ਮਿਲਿਆ_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == ਨਿਰਾ)
	    || new_ScreenAttrs == ਨਿਰਾ
	    || new_Screen_ਖਾਨੇ_ਗਿਣਤੀ == ਨਿਰਾ
	    || new_ਸਤਰ_ਵਿੱਥ == ਨਿਰਾ
	    || new_ਸਤਰ_Wraps == ਨਿਰਾ
	    || new_Tab_ਪੰਨਾ_Idxs == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    || new_popup_ਨਕਾਬ == ਨਿਰਾ
	    || new_popup_ਨਕਾਬ_ਅਗਲਾ == ਨਿਰਾ
	    || new_popup_transparent == ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    || outofmem)
    {
	ਜੇ (Screen_ਸਤਰਾਂ != ਨਿਰਾ || !ਪੂਰਾ_outofmem_ਸੁਨੇਹਾ)
	{
	    // guess the size
	    do_outofmem_ਸੁਨੇਹਾ((long_u)((ਪੰਕਤੀਆਂ + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    ਪੂਰਾ_outofmem_ਸੁਨੇਹਾ = TRUE;
	}
	VIM_CLEAR(new_Screen_ਸਤਰਾਂ);
	VIM_CLEAR(new_Screen_ਸਤਰਾਂ_UC);
	ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_ਸਤਰਾਂ_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_Screen_ਖਾਨੇ_ਗਿਣਤੀ);
	VIM_CLEAR(new_ਸਤਰ_ਵਿੱਥ);
	VIM_CLEAR(new_ਸਤਰ_Wraps);
	VIM_CLEAR(new_Tab_ਪੰਨਾ_Idxs);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_ਨਕਾਬ);
	VIM_CLEAR(new_popup_ਨਕਾਬ_ਅਗਲਾ);
	VIM_CLEAR(new_popup_transparent);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਾ_outofmem_ਸੁਨੇਹਾ = FALSE;

	ਲਈ (new_ਪੰਕਤੀ = 0; new_ਪੰਕਤੀ < ਪੰਕਤੀਆਂ; ++new_ਪੰਕਤੀ)
	{
	    new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ] = new_ਪੰਕਤੀ * Columns;
	    new_ਸਤਰ_Wraps[new_ਪੰਕਤੀ] = FALSE;

	    (ਖਾਲੀ)vim_memset(new_Screen_ਸਤਰਾਂ + new_ਪੰਕਤੀ * Columns,
				  ' ', (size_t)Columns * ਆਕਾਰ(schar_T));
	    ਜੇ (enc_utf8)
	    {
		(ਖਾਲੀ)vim_memset(new_Screen_ਸਤਰਾਂ_UC + new_ਪੰਕਤੀ * Columns,
				   0, (size_t)Columns * ਆਕਾਰ(u8char_T));
		ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
		    (ਖਾਲੀ)vim_memset(new_Screen_ਸਤਰਾਂ_C[i]
						      + new_ਪੰਕਤੀ * Columns,
				   0, (size_t)Columns * ਆਕਾਰ(u8char_T));
	    }
	    ਜੇ (enc_dbcs == DBCS_JPNU)
		(ਖਾਲੀ)vim_memset(new_ScreenLines2 + new_ਪੰਕਤੀ * Columns,
				   0, (size_t)Columns * ਆਕਾਰ(schar_T));
	    (ਖਾਲੀ)vim_memset(new_ScreenAttrs + new_ਪੰਕਤੀ * Columns,
				    0, (size_t)Columns * ਆਕਾਰ(sattr_T));
	    (ਖਾਲੀ)vim_memset(new_Screen_ਖਾਨੇ_ਗਿਣਤੀ + new_ਪੰਕਤੀ * Columns,
				    0, (size_t)Columns * ਆਕਾਰ(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ਜੇ (!doclear)
	    {
		old_ਪੰਕਤੀ = new_ਪੰਕਤੀ + (screen_ਪੰਕਤੀਆਂ - ਪੰਕਤੀਆਂ);
		ਜੇ (old_ਪੰਕਤੀ >= 0 && Screen_ਸਤਰਾਂ != ਨਿਰਾ)
		{
		    ਜੇ (screen_Columns < Columns)
			len = screen_Columns;
		    ਨਹੀਂਤਾਂ
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ਜੇ (!(enc_utf8 && Screen_ਸਤਰਾਂ_UC == ਨਿਰਾ)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_ਸਤਰਾਂ + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
				Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
				(size_t)len * ਆਕਾਰ(schar_T));
		    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC != ਨਿਰਾ
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_ਸਤਰਾਂ_UC + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
				Screen_ਸਤਰਾਂ_UC + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
				(size_t)len * ਆਕਾਰ(u8char_T));
			ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_ਸਤਰਾਂ_C[i]
						    + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
				Screen_ਸਤਰਾਂ_C[i] + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
				(size_t)len * ਆਕਾਰ(u8char_T));
		    }
		    ਜੇ (enc_dbcs == DBCS_JPNU && ScreenLines2 != ਨਿਰਾ)
			mch_memmove(new_ScreenLines2 + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
				ScreenLines2 + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
				(size_t)len * ਆਕਾਰ(schar_T));
		    mch_memmove(new_ScreenAttrs + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
			    ScreenAttrs + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
			    (size_t)len * ਆਕਾਰ(sattr_T));
		    mch_memmove(new_Screen_ਖਾਨੇ_ਗਿਣਤੀ + new_ਸਤਰ_ਵਿੱਥ[new_ਪੰਕਤੀ],
			    Screen_ਖਾਨੇ_ਗਿਣਤੀ + ਸਤਰ_ਵਿੱਥ[old_ਪੰਕਤੀ],
			    (size_t)len * ਆਕਾਰ(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_ਸਤਰ = new_Screen_ਸਤਰਾਂ + ਪੰਕਤੀਆਂ * Columns;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	vim_memset(new_popup_ਨਕਾਬ, 0, ਪੰਕਤੀਆਂ * Columns * ਆਕਾਰ(ਛੋਟਾ));
	vim_memset(new_popup_transparent, 0, ਪੰਕਤੀਆਂ * Columns * ਆਕਾਰ(ਅੱਖਰ));
#ਜੇ_ਅੰਤ
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_ਸਤਰਾਂ = new_Screen_ਸਤਰਾਂ;
    Screen_ਸਤਰਾਂ_UC = new_Screen_ਸਤਰਾਂ_UC;
    ਲਈ (ਪੂਰਨ i = 0; i < p_mco; ++i)
	Screen_ਸਤਰਾਂ_C[i] = new_Screen_ਸਤਰਾਂ_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    Screen_ਖਾਨੇ_ਗਿਣਤੀ = new_Screen_ਖਾਨੇ_ਗਿਣਤੀ;
    ਸਤਰ_ਵਿੱਥ = new_ਸਤਰ_ਵਿੱਥ;
    ਸਤਰ_Wraps = new_ਸਤਰ_Wraps;
    Tab_ਪੰਨਾ_Idxs = new_Tab_ਪੰਨਾ_Idxs;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    popup_ਨਕਾਬ = new_popup_ਨਕਾਬ;
    popup_ਨਕਾਬ_ਅਗਲਾ = new_popup_ਨਕਾਬ_ਅਗਲਾ;
    popup_transparent = new_popup_transparent;
    popup_ਨਕਾਬ_refresh = TRUE;
#ਜੇ_ਅੰਤ

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    old_ਪੰਕਤੀਆਂ_2 = screen_ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ
    screen_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
    screen_Columns = Columns;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Cursor position may now be out of bounds after resize
    ਜੇ (gui.ਵਿੱਚ_use && (gui.cursor_ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ
		|| gui.cursor_ਖਾਨਾ >= screen_Columns))
	gui.cursor_is_valid = false;
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ਜੇ (doclear)
	screenclear2(TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਨਹੀਂਤਾਂ ਜੇ (gui.ਵਿੱਚ_use
	    && !gui.starting
	    && Screen_ਸਤਰਾਂ != ਨਿਰਾ
	    && old_ਪੰਕਤੀਆਂ_2 != ਪੰਕਤੀਆਂ)
    {
	gui_redraw_ਖੰਡ(0, 0, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)		// Rows got smaller
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;		// put cursor at last row
	ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀਆਂ > old_ਪੰਕਤੀਆਂ_2)	// Rows got bigger
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ += ਪੰਕਤੀਆਂ - old_ਪੰਕਤੀਆਂ_2; // put cursor in same place
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= cmdline_ਚੌੜਾਈ)	// cmdline_width got smaller
	    ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;    // put cursor at last cmdline column
    }
#ਜੇ_ਅੰਤ
    clear_Tab_ਪੰਨਾ_Idxs();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ਜੇ_ਅੰਤ

    entered = FALSE;
    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ਜੇ (starting == 0 && ++retry_ਗਿਣਤੀ <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	ਜਾਓ retry;
    }
}

    ਖਾਲੀ
free_screenlines(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    VIM_CLEAR(Screen_ਸਤਰਾਂ_UC);
    ਲਈ (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_ਸਤਰਾਂ_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_ਸਤਰਾਂ);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(Screen_ਖਾਨੇ_ਗਿਣਤੀ);
    VIM_CLEAR(ਸਤਰ_ਵਿੱਥ);
    VIM_CLEAR(ਸਤਰ_Wraps);
    VIM_CLEAR(Tab_ਪੰਨਾ_Idxs);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    VIM_CLEAR(popup_ਨਕਾਬ);
    VIM_CLEAR(popup_ਨਕਾਬ_ਅਗਲਾ);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_ਪੰਕਤੀਆਂ = 0;
    suppressed_ਖਾਨੇ_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    ਪੂਰਨ
screenclear(ਖਾਲੀ)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    ਵਾਪਸ screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    ਖਾਲੀ
redraw_as_cleared(ਖਾਲੀ)
{
    screenclear2(FALSE);
}

    ਸਥਿਰ ਪੂਰਨ
screenclear2(ਪੂਰਨ doclear)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    did_clear = FALSE;

    ਜੇ (starting == NO_SCREEN || Screen_ਸਤਰਾਂ == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    || (gui.ਵਿੱਚ_use && gui.starting)
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#ਜੇ_ਅੰਤ

    // blank out ScreenLines
    ਲਈ (i = 0; i < ਪੰਕਤੀਆਂ; ++i)
    {
	lineclear(ਸਤਰ_ਵਿੱਥ[i], (ਪੂਰਨ)Columns, 0);
	ਸਤਰ_Wraps[i] = FALSE;
    }

    ਜੇ (doclear && can_clear(T_CL))
    {
	ਬਾਹਰਲਾ_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	ਬਹੁਲਕ_displayed = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ਜੇ (suppressed_cells != ਨਿਰਾ)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_ਪੰਕਤੀਆਂ * suppressed_ਖਾਨੇ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// can't clear the screen, mark all chars with invalid attributes
	ਲਈ (i = 0; i < ਪੰਕਤੀਆਂ; ++i)
	    lineinvalid(ਸਤਰ_ਵਿੱਥ[i], (ਪੂਰਨ)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    ਸੁਨੇਹਾ_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#ਜੇ_ਅੰਤ
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;	// put cursor on last line for messages
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
    ਸੁਨੇਹਾ_didany = FALSE;
    ਸੁਨੇਹਾ_didout = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    ਖਿਸਕਾਓ_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ਸਥਿਰ ਖਾਲੀ
lineclear(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ attr)
{
    (ਖਾਲੀ)vim_memset(Screen_ਸਤਰਾਂ + off, ' ', (size_t)ਚੌੜਾਈ * ਆਕਾਰ(schar_T));
    ਜੇ (enc_utf8)
	(ਖਾਲੀ)vim_memset(Screen_ਸਤਰਾਂ_UC + off, 0,
					  (size_t)ਚੌੜਾਈ * ਆਕਾਰ(u8char_T));
    (ਖਾਲੀ)vim_memset(ScreenAttrs + off, attr, (size_t)ਚੌੜਾਈ * ਆਕਾਰ(sattr_T));
    (ਖਾਲੀ)vim_memset(Screen_ਖਾਨੇ_ਗਿਣਤੀ + off, -1, (size_t)ਚੌੜਾਈ * ਆਕਾਰ(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ਸਥਿਰ ਖਾਲੀ
lineinvalid(ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਪੂਰਨ ਚੌੜਾਈ)
{
    (ਖਾਲੀ)vim_memset(ScreenAttrs + off, -1, (size_t)ਚੌੜਾਈ * ਆਕਾਰ(sattr_T));
    (ਖਾਲੀ)vim_memset(Screen_ਖਾਨੇ_ਗਿਣਤੀ + off, -1, (size_t)ਚੌੜਾਈ * ਆਕਾਰ(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    ਖਾਲੀ
ਸਤਰ_was_clobbered(ਪੂਰਨ screen_lnum)
{
    lineinvalid(ਸਤਰ_ਵਿੱਥ[screen_lnum], (ਪੂਰਨ)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ਸਥਿਰ ਖਾਲੀ
linecopy(ਪੂਰਨ ਮੰਜ਼ਿਲ, ਪੂਰਨ from, win_T *wp)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ	off_ਮੰਜ਼ਿਲ = ਸਤਰ_ਵਿੱਥ[ਮੰਜ਼ਿਲ] + wp->w_wincol;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	off_from = ਸਤਰ_ਵਿੱਥ[from] + wp->w_wincol;

    mch_memmove(Screen_ਸਤਰਾਂ + off_ਮੰਜ਼ਿਲ, Screen_ਸਤਰਾਂ + off_from,
	    wp->w_ਚੌੜਾਈ * ਆਕਾਰ(schar_T));
    ਜੇ (enc_utf8)
    {
	ਪੂਰਨ	i;

	mch_memmove(Screen_ਸਤਰਾਂ_UC + off_ਮੰਜ਼ਿਲ, Screen_ਸਤਰਾਂ_UC + off_from,
		wp->w_ਚੌੜਾਈ * ਆਕਾਰ(u8char_T));
	ਲਈ (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_ਸਤਰਾਂ_C[i] + off_ਮੰਜ਼ਿਲ, Screen_ਸਤਰਾਂ_C[i] + off_from,
		    wp->w_ਚੌੜਾਈ * ਆਕਾਰ(u8char_T));
    }
    ਜੇ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_ਮੰਜ਼ਿਲ, ScreenLines2 + off_from,
		wp->w_ਚੌੜਾਈ * ਆਕਾਰ(schar_T));
    mch_memmove(ScreenAttrs + off_ਮੰਜ਼ਿਲ, ScreenAttrs + off_from,
	    wp->w_ਚੌੜਾਈ * ਆਕਾਰ(sattr_T));
    mch_memmove(Screen_ਖਾਨੇ_ਗਿਣਤੀ + off_ਮੰਜ਼ਿਲ, Screen_ਖਾਨੇ_ਗਿਣਤੀ + off_from,
	    wp->w_ਚੌੜਾਈ * ਆਕਾਰ(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    ਪੂਰਨ
can_clear(char_u *p)
{
    ਵਾਪਸ (*p != NUL && (t_colors <= 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		|| gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_ਸਾਧਾਰਨ_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_ਸਾਧਾਰਨ_bg_color == 0)
#ਨਹੀਂਤਾਂ
		|| cterm_ਸਾਧਾਰਨ_bg_color == 0
#ਜੇ_ਅੰਤ
		|| *T_UT != NUL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#ਜੇ_ਅੰਤ
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    ਖਾਲੀ
screen_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    screen_cur_ਪੰਕਤੀ = screen_cur_ਖਾਨਾ = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    ਖਾਲੀ
windgoto(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    sattr_T	    *p;
    ਪੂਰਨ		    i;
    ਪੂਰਨ		    plan;
    ਪੂਰਨ		    ਲਾਗਤ;
    ਪੂਰਨ		    wouldbe_ਖਾਨਾ;
    ਪੂਰਨ		    noinvcurs;
    char_u	    *bs;
    ਪੂਰਨ		    goto_ਲਾਗਤ;
    ਪੂਰਨ		    attr;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOTO_ਲਾਗਤ   7	// assume a term_windgoto() takes about 7 chars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIGHL_ਲਾਗਤ  5	// assume unhighlight takes 5 chars

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAN_LE	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAN_CR	    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAN_NL	    3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (ਖਾਨਾ == screen_cur_ਖਾਨਾ && ਪੰਕਤੀ == screen_cur_ਪੰਕਤੀ)
	ਵਾਪਸ;

    // Check for valid position.
    ਜੇ (ਪੰਕਤੀ < 0)	// window without text lines?
	ਪੰਕਤੀ = 0;
    ਜੇ (ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ)
	ਪੰਕਤੀ = screen_ਪੰਕਤੀਆਂ - 1;
    ਜੇ (ਖਾਨਾ >= screen_Columns)
	ਖਾਨਾ = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ਜੇ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_ਲਾਗਤ;
    ਨਹੀਂਤਾਂ
	noinvcurs = 0;
    goto_ਲਾਗਤ = GOTO_ਲਾਗਤ + noinvcurs;

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
    ਜੇ (ਪੰਕਤੀ >= screen_cur_ਪੰਕਤੀ && screen_cur_ਖਾਨਾ < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = ਨਿਰਾ;			    // init for GCC
	attr = screen_attr;
	ਜੇ (ਪੰਕਤੀ == screen_cur_ਪੰਕਤੀ && ਖਾਨਾ < screen_cur_ਖਾਨਾ)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ਜੇ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    ਨਹੀਂਤਾਂ
		bs = T_BC;		    // "backspace character (old)
	    ਜੇ (*bs)
		ਲਾਗਤ = (screen_cur_ਖਾਨਾ - ਖਾਨਾ) * (ਪੂਰਨ)STRLEN(bs);
	    ਨਹੀਂਤਾਂ
		ਲਾਗਤ = 999;
	    ਜੇ (ਖਾਨਾ + 1 < ਲਾਗਤ)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_ਖਾਨਾ = 0;
		ਲਾਗਤ = 1;		    // CR is just one character
	    }
	    ਨਹੀਂਤਾਂ
	    {
		plan = PLAN_LE;
		wouldbe_ਖਾਨਾ = ਖਾਨਾ;
	    }
	    ਜੇ (noinvcurs)		    // will stop highlighting
	    {
		ਲਾਗਤ += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀ > screen_cur_ਪੰਕਤੀ)
	{
	    plan = PLAN_NL;
	    wouldbe_ਖਾਨਾ = 0;
	    ਲਾਗਤ = (ਪੰਕਤੀ - screen_cur_ਪੰਕਤੀ) * 2;  // CR LF
	    ਜੇ (noinvcurs)		    // will stop highlighting
	    {
		ਲਾਗਤ += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	ਨਹੀਂਤਾਂ
	{
	    plan = PLAN_WRITE;
	    wouldbe_ਖਾਨਾ = screen_cur_ਖਾਨਾ;
	    ਲਾਗਤ = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = ਖਾਨਾ - wouldbe_ਖਾਨਾ;
	ਜੇ (i > 0)
	    ਲਾਗਤ += i;
	ਜੇ (ਲਾਗਤ < goto_ਲਾਗਤ && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + wouldbe_ਖਾਨਾ;
	    ਜਦੋਂ (i && *p++ == attr)
		--i;
	    ਜੇ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ਜੇ (*--p == 0)
		{
		    ਲਾਗਤ += noinvcurs;
		    ਜਦੋਂ (i && *p++ == 0)
			--i;
		}
		ਜੇ (i != 0)
		    ਲਾਗਤ = 999;	// different attributes, don't do it
	    }
	    ਜੇ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		ਲਈ (i = wouldbe_ਖਾਨਾ; i < ਖਾਨਾ; ++i)
		    ਜੇ (Screen_ਸਤਰਾਂ_UC[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + i] != 0)
		    {
			ਲਾਗਤ = 999;
			ਰੋਕੋ;
		    }
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ਜੇ (ਲਾਗਤ < 999)
		ਲਈ (i = wouldbe_ਖਾਨਾ; i < ਖਾਨਾ; ++i)
		    ਜੇ (popup_is_under_opacity(ਪੰਕਤੀ, i))
		    {
			ਲਾਗਤ = 999;
			ਰੋਕੋ;
		    }
#ਜੇ_ਅੰਤ
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ਜੇ (ਲਾਗਤ < goto_ਲਾਗਤ)
	{
	    ਜੇ (plan == PLAN_LE)
	    {
		ਜੇ (noinvcurs)
		    screen_stop_highlight();
		ਜਦੋਂ (screen_cur_ਖਾਨਾ > ਖਾਨਾ)
		{
		    ਬਾਹਰਲਾ_str(bs);
		    --screen_cur_ਖਾਨਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (plan == PLAN_CR)
	    {
		ਜੇ (noinvcurs)
		    screen_stop_highlight();
		ਬਾਹਰਲਾ_char('\r');
		screen_cur_ਖਾਨਾ = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (plan == PLAN_NL)
	    {
		ਜੇ (noinvcurs)
		    screen_stop_highlight();
		ਜਦੋਂ (screen_cur_ਪੰਕਤੀ < ਪੰਕਤੀ)
		{
		    ਬਾਹਰਲਾ_char('\n');
		    ++screen_cur_ਪੰਕਤੀ;
		}
		screen_cur_ਖਾਨਾ = 0;
	    }

	    i = ਖਾਨਾ - screen_cur_ਖਾਨਾ;
	    ਜੇ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ਜੇ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    ਜਦੋਂ (i-- > 0)
			ਬਾਹਰਲਾ_char(*T_ND);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ	off;

		    off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + screen_cur_ਖਾਨਾ;
		    ਜਦੋਂ (i-- > 0)
		    {
			ਜੇ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			ਬਾਹਰਲਾ_flush_check();
			ਬਾਹਰਲਾ_char(Screen_ਸਤਰਾਂ[off]);
			ਜੇ (enc_dbcs == DBCS_JPNU
				&& Screen_ਸਤਰਾਂ[off] == 0x8e)
			    ਬਾਹਰਲਾ_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	ਲਾਗਤ = 999;

    ਜੇ (ਲਾਗਤ >= goto_ਲਾਗਤ)
    {
	ਜੇ (noinvcurs)
	    screen_stop_highlight();
	ਜੇ (ਪੰਕਤੀ == screen_cur_ਪੰਕਤੀ && (ਖਾਨਾ > screen_cur_ਖਾਨਾ)
		&& *T_CRI != NUL)
	    ਪਦ_cursor_ਸੱਜਾ(ਖਾਨਾ - screen_cur_ਖਾਨਾ);
	ਨਹੀਂਤਾਂ
	    ਪਦ_windgoto(ਪੰਕਤੀ, ਖਾਨਾ);
    }
    screen_cur_ਪੰਕਤੀ = ਪੰਕਤੀ;
    screen_cur_ਖਾਨਾ = ਖਾਨਾ;
}

/*
 * Set cursor to its position in the current window.
 */
    ਖਾਲੀ
setcursor(ਖਾਲੀ)
{
    setcursor_mayforce(FALSE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    ਪੂਰਨ
cursor_screen_cells(ਖਾਲੀ)
{
    ਵਾਪਸ has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#ਜੇ_ਅੰਤ

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    ਖਾਲੀ
setcursor_mayforce(ਪੂਰਨ force)
{
    ਜੇ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((ਪੂਰਨ)curwin->w_ਚੌੜਾਈ - curwin->w_wcol
					    - cursor_screen_cells()) :
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
win_ins_ਸਤਰਾਂ(
    win_T	*wp,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		invalid,
    ਪੂਰਨ		mayclear)
{
    ਪੂਰਨ		did_delete;
    ਪੂਰਨ		nextrow;
    ਪੂਰਨ		lastrow;
    ਪੂਰਨ		retval;

    ਜੇ (invalid)
	wp->w_ਸਤਰਾਂ_valid = 0;

    // with only a few lines it's not worth the effort
    ਜੇ (wp->w_ਉਚਾਈ < 5)
	ਵਾਪਸ FAIL;

    // with the popup menu visible this might not work correctly
    ਜੇ (pum_visible())
	ਵਾਪਸ FAIL;

    ਜੇ (ਸਤਰ_ਗਿਣਤੀ > wp->w_ਉਚਾਈ - ਪੰਕਤੀ)
	ਸਤਰ_ਗਿਣਤੀ = wp->w_ਉਚਾਈ - ਪੰਕਤੀ;

    retval = win_do_ਸਤਰਾਂ(wp, ਪੰਕਤੀ, ਸਤਰ_ਗਿਣਤੀ, mayclear, FALSE, 0);
    ਜੇ (retval != MAYBE)
	ਵਾਪਸ retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ਜੇ (wp->w_ਅਗਲਾ != ਨਿਰਾ || wp->w_ਸਥਿਤੀ_ਉਚਾਈ)
    {
	ਜੇ (screen_del_ਸਤਰਾਂ(0, W_WINROW(wp) + wp->w_ਉਚਾਈ - ਸਤਰ_ਗਿਣਤੀ,
				  ਸਤਰ_ਗਿਣਤੀ, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, FALSE, 0, ਨਿਰਾ) == OK)
	    did_delete = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਅਗਲਾ)
	    ਵਾਪਸ FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ਜੇ (!did_delete)
    {
	wp->w_redr_ਸਥਿਤੀ = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ;
	lastrow = nextrow + ਸਤਰ_ਗਿਣਤੀ;
	ਜੇ (lastrow > ਪੰਕਤੀਆਂ)
	    lastrow = ਪੰਕਤੀਆਂ;
	screen_fill(nextrow - ਸਤਰ_ਗਿਣਤੀ, lastrow - ਸਤਰ_ਗਿਣਤੀ,
		  wp->w_wincol, (ਪੂਰਨ)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ਜੇ (screen_ins_ਸਤਰਾਂ(0, W_WINROW(wp) + ਪੰਕਤੀ, ਸਤਰ_ਗਿਣਤੀ, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, ਨਿਰਾ)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ਜੇ (did_delete)
	{
	    wp->w_redr_ਸਥਿਤੀ = true;
	    win_rest_invalid(W_ਅਗਲਾ(wp));
	}
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ਪੂਰਨ
win_del_ਸਤਰਾਂ(
    win_T	*wp,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		invalid,
    ਪੂਰਨ		mayclear,
    ਪੂਰਨ		clear_attr)	    // for clearing lines
{
    ਪੂਰਨ		retval;

    ਜੇ (invalid)
	wp->w_ਸਤਰਾਂ_valid = 0;

    ਜੇ (ਸਤਰ_ਗਿਣਤੀ > wp->w_ਉਚਾਈ - ਪੰਕਤੀ)
	ਸਤਰ_ਗਿਣਤੀ = wp->w_ਉਚਾਈ - ਪੰਕਤੀ;

    retval = win_do_ਸਤਰਾਂ(wp, ਪੰਕਤੀ, ਸਤਰ_ਗਿਣਤੀ, mayclear, TRUE, clear_attr);
    ਜੇ (retval != MAYBE)
	ਵਾਪਸ retval;

    ਜੇ (screen_del_ਸਤਰਾਂ(0, W_WINROW(wp) + ਪੰਕਤੀ, ਸਤਰ_ਗਿਣਤੀ,
				   (ਪੂਰਨ)ਪੰਕਤੀਆਂ, FALSE, clear_attr, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ਜੇ (wp->w_ਅਗਲਾ || wp->w_ਸਥਿਤੀ_ਉਚਾਈ || cmdline_ਪੰਕਤੀ < ਪੰਕਤੀਆਂ - 1)
    {
	ਜੇ (screen_ins_ਸਤਰਾਂ(0, W_WINROW(wp) + wp->w_ਉਚਾਈ - ਸਤਰ_ਗਿਣਤੀ,
			      ਸਤਰ_ਗਿਣਤੀ, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, clear_attr, ਨਿਰਾ) == FAIL)
	{
	    wp->w_redr_ਸਥਿਤੀ = true;
	    win_rest_invalid(wp->w_ਅਗਲਾ);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    ਨਹੀਂਤਾਂ
	redraw_cmdline = TRUE;
    ਵਾਪਸ OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ਸਥਿਰ ਪੂਰਨ
win_do_ਸਤਰਾਂ(
    win_T	*wp,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		mayclear,
    ਪੂਰਨ		del,
    ਪੂਰਨ		clear_attr)
{
    ਪੂਰਨ		retval;

    ਜੇ (!redrawing() || ਸਤਰ_ਗਿਣਤੀ <= 0)
	ਵਾਪਸ FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ਜੇ (no_win_do_ਸਤਰਾਂ_ins && !del)
	ਵਾਪਸ FAIL;

    // only a few lines left: redraw is faster
    ਜੇ (mayclear && ਪੰਕਤੀਆਂ - ਸਤਰ_ਗਿਣਤੀ < 5
	    && wp->w_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ)
    {
	ਜੇ (!no_win_do_ਸਤਰਾਂ_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ਜੇ (popup_visible)
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    // Delete all remaining lines
    ਜੇ (ਪੰਕਤੀ + ਸਤਰ_ਗਿਣਤੀ >= wp->w_ਉਚਾਈ)
    {
	screen_fill(W_WINROW(wp) + ਪੰਕਤੀ, W_WINROW(wp) + wp->w_ਉਚਾਈ,
		wp->w_wincol, (ਪੂਰਨ)W_ENDCOL(wp), ' ', ' ', 0);
	ਵਾਪਸ OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ਜੇ (!no_win_do_ਸਤਰਾਂ_ins)
	clear_cmdline = TRUE;

#ਜੇ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ਜੇ (tabpanel_ਚੌੜਾਈ() > 0)
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    ਜੇ (scroll_region || wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ)
    {
	ਜੇ (scroll_region && (wp->w_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ
		    || *T_CSV != NUL))
	    scroll_region_ਸੈੱਟ(wp, ਪੰਕਤੀ);
	ਜੇ (del)
	    retval = screen_del_ਸਤਰਾਂ(W_WINROW(wp) + ਪੰਕਤੀ, 0, ਸਤਰ_ਗਿਣਤੀ,
				    wp->w_ਉਚਾਈ - ਪੰਕਤੀ, FALSE, clear_attr, wp);
	ਨਹੀਂਤਾਂ
	    retval = screen_ins_ਸਤਰਾਂ(W_WINROW(wp) + ਪੰਕਤੀ, 0, ਸਤਰ_ਗਿਣਤੀ,
					   wp->w_ਉਚਾਈ - ਪੰਕਤੀ, clear_attr, wp);
	ਜੇ (scroll_region && (wp->w_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ
		    || *T_CSV != NUL))
	    scroll_region_reset();
	ਵਾਪਸ retval;
    }

    ਜੇ (wp->w_ਅਗਲਾ != ਨਿਰਾ && p_tf) // don't delete/insert on fast terminal
	ਵਾਪਸ FAIL;

    ਵਾਪਸ MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ਸਥਿਰ ਖਾਲੀ
win_rest_invalid(win_T *wp)
{
    ਜਦੋਂ (wp != ਨਿਰਾ)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_ਸਥਿਤੀ = true;
	wp = wp->w_ਅਗਲਾ;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_CAL   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_CDL   2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_AL    3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_CE    4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_DL    5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_SR    6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_NL	    7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_T_CD    8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    ਪੂਰਨ
screen_ins_ਸਤਰਾਂ(
    ਪੂਰਨ		off,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		ਸਿਰਾ,
    ਪੂਰਨ		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਆਰਜ਼ੀ;
    ਪੂਰਨ		cursor_ਪੰਕਤੀ;
    ਪੂਰਨ		cursor_ਖਾਨਾ = 0;
    ਪੂਰਨ		type;
    ਪੂਰਨ		ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ;
    ਪੂਰਨ		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ਜੇ (!screen_valid(TRUE)
	     || ਸਤਰ_ਗਿਣਤੀ <= 0 || ਸਤਰ_ਗਿਣਤੀ > p_ttyscroll
	     || ਸਿਰਾ > ਪੰਕਤੀਆਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	     || (clip_ਤਾਰਾ.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#ਜੇ_ਅੰਤ
	     )
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	ਖਿਸਕਾਓ_image_placements(off + ਪੰਕਤੀ, off + ਸਿਰਾ, ਸਤਰ_ਗਿਣਤੀ);
# endif
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

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
    ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ = (ਪੰਕਤੀ + ਸਤਰ_ਗਿਣਤੀ >= ਸਿਰਾ);
    ਜੇ (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ਜੇ (ਸਤਰ_ਗਿਣਤੀ > 3)
	    ਵਾਪਸ FAIL;
	type = USE_REDRAW;
    }
    ਨਹੀਂਤਾਂ ਜੇ (can_clear(T_CD) && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ)
	type = USE_T_CD;
    ਨਹੀਂਤਾਂ ਜੇ (*T_CAL != NUL && (ਸਤਰ_ਗਿਣਤੀ > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    ਨਹੀਂਤਾਂ ਜੇ (*T_CDL != NUL && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ && (ਸਤਰ_ਗਿਣਤੀ > 1 || !can_ce))
	type = USE_T_CDL;
    ਨਹੀਂਤਾਂ ਜੇ (*T_AL != NUL)
	type = USE_T_AL;
    ਨਹੀਂਤਾਂ ਜੇ (can_ce && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ)
	type = USE_T_CE;
    ਨਹੀਂਤਾਂ ਜੇ (*T_DL != NUL && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ)
	type = USE_T_DL;
    ਨਹੀਂਤਾਂ ਜੇ (*T_SR != NUL && ਪੰਕਤੀ == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ਜੇ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	ਵਾਪਸ screen_del_ਸਤਰਾਂ(off, ਪੰਕਤੀ, ਸਤਰ_ਗਿਣਤੀ, ਸਿਰਾ, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ਜੇ (*T_DB)
	screen_del_ਸਤਰਾਂ(off, ਸਿਰਾ - ਸਤਰ_ਗਿਣਤੀ, ਸਤਰ_ਗਿਣਤੀ, ਸਿਰਾ, FALSE, 0, wp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ਜੇ (off + ਪੰਕਤੀ > 0 || (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ))
	clip_clear_selection(&clip_ਤਾਰਾ);
    ਨਹੀਂਤਾਂ
	clip_scroll_selection(-ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    ਖਿਸਕਾਓ_image_placements(off + ਪੰਕਤੀ, off + ਸਿਰਾ, ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_ਤਾਲਾ_screen();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(ਪੰਕਤੀ + off <= gui.cursor_ਪੰਕਤੀ);
#ਜੇ_ਅੰਤ

    ਜੇ (wp != ਨਿਰਾ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ਖਾਨਾ = wp->w_wincol;

    ਜੇ (*T_CCS != NUL)	   // cursor relative to region
	cursor_ਪੰਕਤੀ = ਪੰਕਤੀ;
    ਨਹੀਂਤਾਂ
	cursor_ਪੰਕਤੀ = ਪੰਕਤੀ + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    ਪੰਕਤੀ += off;
    ਸਿਰਾ += off;
    ਲਈ (i = 0; i < ਸਤਰ_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ)
	{
	    // need to copy part of a line
	    j = ਸਿਰਾ - 1 - i;
	    ਜਦੋਂ ((j -= ਸਤਰ_ਗਿਣਤੀ) >= ਪੰਕਤੀ)
		linecopy(j + ਸਤਰ_ਗਿਣਤੀ, j, wp);
	    j += ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (can_clear((char_u *)" "))
		lineclear(ਸਤਰ_ਵਿੱਥ[j] + wp->w_wincol,
			wp->w_ਚੌੜਾਈ, clear_attr);
	    ਨਹੀਂਤਾਂ
		lineinvalid(ਸਤਰ_ਵਿੱਥ[j] + wp->w_wincol, wp->w_ਚੌੜਾਈ);
	    ਸਤਰ_Wraps[j] = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    j = ਸਿਰਾ - 1 - i;
	    ਆਰਜ਼ੀ = ਸਤਰ_ਵਿੱਥ[j];
	    ਜਦੋਂ ((j -= ਸਤਰ_ਗਿਣਤੀ) >= ਪੰਕਤੀ)
	    {
		ਸਤਰ_ਵਿੱਥ[j + ਸਤਰ_ਗਿਣਤੀ] = ਸਤਰ_ਵਿੱਥ[j];
		ਸਤਰ_Wraps[j + ਸਤਰ_ਗਿਣਤੀ] = ਸਤਰ_Wraps[j];
	    }
	    ਸਤਰ_ਵਿੱਥ[j + ਸਤਰ_ਗਿਣਤੀ] = ਆਰਜ਼ੀ;
	    ਸਤਰ_Wraps[j + ਸਤਰ_ਗਿਣਤੀ] = FALSE;
	    ਜੇ (can_clear((char_u *)" "))
		lineclear(ਆਰਜ਼ੀ, (ਪੂਰਨ)Columns, clear_attr);
	    ਨਹੀਂਤਾਂ
		lineinvalid(ਆਰਜ਼ੀ, (ਪੂਰਨ)Columns);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ਜੇ_ਅੰਤ

    screen_stop_highlight();
    windgoto(cursor_ਪੰਕਤੀ, cursor_ਖਾਨਾ);
    ਜੇ (clear_attr != 0)
	screen_ਸ਼ੁਰੂ_highlight(clear_attr);

    // redraw the characters
    ਜੇ (type == USE_REDRAW)
	redraw_ਖੰਡ(ਪੰਕਤੀ, ਸਿਰਾ, wp);
    ਨਹੀਂਤਾਂ ਜੇ (type == USE_T_CAL)
    {
	ਪਦ_append_ਸਤਰਾਂ(ਸਤਰ_ਗਿਣਤੀ);
	screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < ਸਤਰ_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (type == USE_T_AL)
	    {
		ਜੇ (i && cursor_ਪੰਕਤੀ != 0)
		    windgoto(cursor_ਪੰਕਤੀ, cursor_ਖਾਨਾ);
		ਬਾਹਰਲਾ_str(T_AL);
	    }
	    ਨਹੀਂਤਾਂ  // type == USE_T_SR
		ਬਾਹਰਲਾ_str(T_SR);
	    screen_ਸ਼ੁਰੂ();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ਜੇ (type == USE_T_SR && *T_DA)
    {
	ਲਈ (i = 0; i < ਸਤਰ_ਗਿਣਤੀ; ++i)
	{
	    windgoto(off + i, cursor_ਖਾਨਾ);
	    ਬਾਹਰਲਾ_str(T_CE);
	    screen_ਸ਼ੁਰੂ();	    // don't know where cursor is now
	}
    }

#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    gui_can_update_cursor();
    ਜੇ (gui.ਵਿੱਚ_use)
	ਬਾਹਰਲਾ_flush();	// always flush after a scroll
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ਪੂਰਨ
screen_del_ਸਤਰਾਂ(
    ਪੂਰਨ		off,
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		ਸਿਰਾ,
    ਪੂਰਨ		force,		// even when line_count > p_ttyscroll
    ਪੂਰਨ		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    ਪੂਰਨ		j;
    ਪੂਰਨ		i;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਆਰਜ਼ੀ;
    ਪੂਰਨ		cursor_ਪੰਕਤੀ;
    ਪੂਰਨ		cursor_ਖਾਨਾ = 0;
    ਪੂਰਨ		cursor_ਸਿਰਾ;
    ਪੂਰਨ		ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ;	// result is empty until end of region
    ਪੂਰਨ		can_delete;	// deleting line codes can be used
    ਪੂਰਨ		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ਜੇ (!screen_valid(TRUE)
	    || ਸਤਰ_ਗਿਣਤੀ <= 0
	    || (!force && ਸਤਰ_ਗਿਣਤੀ > p_ttyscroll)
	    || ਸਿਰਾ > ਪੰਕਤੀਆਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	    || (clip_ਤਾਰਾ.state != SELECT_CLEARED && redrawing_for_callback > 0)
#ਜੇ_ਅੰਤ
       )
	ਵਾਪਸ FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ = ਪੰਕਤੀ + ਸਤਰ_ਗਿਣਤੀ >= ਸਿਰਾ;

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
    ਜੇ (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ਜੇ (ਸਤਰ_ਗਿਣਤੀ > 3)
	    ਵਾਪਸ FAIL;
	type = USE_REDRAW;
    }
    ਨਹੀਂਤਾਂ ਜੇ (can_clear(T_CD) && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ)
	type = USE_T_CD;
    ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀ == 0 && (
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    ਸਤਰ_ਗਿਣਤੀ == 1 ||
#ਜੇ_ਅੰਤ
						*T_CDL == NUL))
	type = USE_NL;
    ਨਹੀਂਤਾਂ ਜੇ (*T_CDL != NUL && ਸਤਰ_ਗਿਣਤੀ > 1 && can_delete)
	type = USE_T_CDL;
    ਨਹੀਂਤਾਂ ਜੇ (can_clear(T_CE) && ਨਤੀਜਾ_ਖਾਲੀ_ਥਾਵਾਂ
	    && (wp == ਨਿਰਾ || wp->w_ਚੌੜਾਈ == topframe->fr_ਚੌੜਾਈ))
	type = USE_T_CE;
    ਨਹੀਂਤਾਂ ਜੇ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    ਨਹੀਂਤਾਂ ਜੇ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ਜੇ (off + ਪੰਕਤੀ > 0 || (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ))
	clip_clear_selection(&clip_ਤਾਰਾ);
    ਨਹੀਂਤਾਂ
	clip_scroll_selection(ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    ਖਿਸਕਾਓ_image_placements(off + ਪੰਕਤੀ, off + ਸਿਰਾ, -ਸਤਰ_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_ਤਾਲਾ_screen();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_ਪੰਕਤੀ >= ਪੰਕਤੀ + off
						&& gui.cursor_ਪੰਕਤੀ < ਸਿਰਾ + off);
#ਜੇ_ਅੰਤ

    ਜੇ (wp != ਨਿਰਾ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ਖਾਨਾ = wp->w_wincol;

    ਜੇ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_ਪੰਕਤੀ = ਪੰਕਤੀ;
	cursor_ਸਿਰਾ = ਸਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	cursor_ਪੰਕਤੀ = ਪੰਕਤੀ + off;
	cursor_ਸਿਰਾ = ਸਿਰਾ + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    ਪੰਕਤੀ += off;
    ਸਿਰਾ += off;
    ਲਈ (i = 0; i < ਸਤਰ_ਗਿਣਤੀ; ++i)
    {
	ਜੇ (wp != ਨਿਰਾ && wp->w_ਚੌੜਾਈ != topframe->fr_ਚੌੜਾਈ)
	{
	    // need to copy part of a line
	    j = ਪੰਕਤੀ + i;
	    ਜਦੋਂ ((j += ਸਤਰ_ਗਿਣਤੀ) <= ਸਿਰਾ - 1)
		linecopy(j - ਸਤਰ_ਗਿਣਤੀ, j, wp);
	    j -= ਸਤਰ_ਗਿਣਤੀ;
	    ਜੇ (can_clear((char_u *)" "))
		lineclear(ਸਤਰ_ਵਿੱਥ[j] + wp->w_wincol,
			wp->w_ਚੌੜਾਈ, clear_attr);
	    ਨਹੀਂਤਾਂ
		lineinvalid(ਸਤਰ_ਵਿੱਥ[j] + wp->w_wincol, wp->w_ਚੌੜਾਈ);
	    ਸਤਰ_Wraps[j] = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // whole width, moving the line pointers is faster
	    j = ਪੰਕਤੀ + i;
	    ਆਰਜ਼ੀ = ਸਤਰ_ਵਿੱਥ[j];
	    ਜਦੋਂ ((j += ਸਤਰ_ਗਿਣਤੀ) <= ਸਿਰਾ - 1)
	    {
		ਸਤਰ_ਵਿੱਥ[j - ਸਤਰ_ਗਿਣਤੀ] = ਸਤਰ_ਵਿੱਥ[j];
		ਸਤਰ_Wraps[j - ਸਤਰ_ਗਿਣਤੀ] = ਸਤਰ_Wraps[j];
	    }
	    ਸਤਰ_ਵਿੱਥ[j - ਸਤਰ_ਗਿਣਤੀ] = ਆਰਜ਼ੀ;
	    ਸਤਰ_Wraps[j - ਸਤਰ_ਗਿਣਤੀ] = FALSE;
	    ਜੇ (can_clear((char_u *)" "))
		lineclear(ਆਰਜ਼ੀ, (ਪੂਰਨ)Columns, clear_attr);
	    ਨਹੀਂਤਾਂ
		lineinvalid(ਆਰਜ਼ੀ, (ਪੂਰਨ)Columns);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ਜੇ_ਅੰਤ

    ਜੇ (screen_attr != clear_attr)
	screen_stop_highlight();
    ਜੇ (clear_attr != 0)
	screen_ਸ਼ੁਰੂ_highlight(clear_attr);

    // redraw the characters
    ਜੇ (type == USE_REDRAW)
	redraw_ਖੰਡ(ਪੰਕਤੀ, ਸਿਰਾ, wp);
    ਨਹੀਂਤਾਂ ਜੇ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_ਪੰਕਤੀ, cursor_ਖਾਨਾ);
	ਬਾਹਰਲਾ_str(T_CD);
	screen_ਸ਼ੁਰੂ();			// don't know where cursor is now
    }
    ਨਹੀਂਤਾਂ ਜੇ (type == USE_T_CDL)
    {
	windgoto(cursor_ਪੰਕਤੀ, cursor_ਖਾਨਾ);
	ਪਦ_delete_ਸਤਰਾਂ(ਸਤਰ_ਗਿਣਤੀ);
	screen_ਸ਼ੁਰੂ();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    ਨਹੀਂਤਾਂ ਜੇ (type == USE_NL)
    {
	windgoto(cursor_ਸਿਰਾ - 1, cursor_ਖਾਨਾ);
	ਲਈ (i = ਸਤਰ_ਗਿਣਤੀ; --i >= 0; )
	    ਬਾਹਰਲਾ_char('\n');		// cursor will remain on same line
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = ਸਤਰ_ਗਿਣਤੀ; --i >= 0; )
	{
	    ਜੇ (type == USE_T_DL)
	    {
		windgoto(cursor_ਪੰਕਤੀ, cursor_ਖਾਨਾ);
		ਬਾਹਰਲਾ_str(T_DL);		// delete a line
	    }
	    ਨਹੀਂਤਾਂ // type == USE_T_CE
	    {
		windgoto(cursor_ਪੰਕਤੀ + i, cursor_ਖਾਨਾ);
		ਬਾਹਰਲਾ_str(T_CE);		// erase a line
	    }
	    screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    ਜੇ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	ਲਈ (i = ਸਤਰ_ਗਿਣਤੀ; i > 0; --i)
	{
	    windgoto(cursor_ਸਿਰਾ - i, cursor_ਖਾਨਾ);
	    ਬਾਹਰਲਾ_str(T_CE);		// erase a line
	    screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
	}
    }

#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    gui_can_update_cursor();
    ਜੇ (gui.ਵਿੱਚ_use)
	ਬਾਹਰਲਾ_flush();	// always flush after a scroll
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    ਪੂਰਨ
skip_showmode(ਖਾਲੀ)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ਜੇ (global_busy
	    || ਸੁਨੇਹਾ_silent != 0
	    || !redrawing()
	    || ((!stuff_ਖਾਲੀ_ਥਾਵਾਂ() || typebuf.tb_len > 0 || using_script())
		&& !ਕੁੰਜੀ_Typed))
    {
	redraw_ਬਹੁਲਕ = TRUE;		// show mode later
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
showmode(ਖਾਲੀ)
{
    ਪੂਰਨ		need_clear;
    ਪੂਰਨ		ਲੰਮਾਈ = 0;
    ਪੂਰਨ		do_ਬਹੁਲਕ;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		nwr_save;
    ਪੂਰਨ		sub_attr;
    ਪੂਰਨ		show_ruler_with_pum = FALSE;

    do_ਬਹੁਲਕ = p_smd && ਸੁਨੇਹਾ_silent == 0
	    && ((State & ਬਹੁਲਕ_ਪਾਓ)
		|| restart_edit != NUL
		|| VIsual_ਸਰਗਰਮ);
    ਜੇ (do_ਬਹੁਲਕ || reg_recording != 0)
    {
	ਜੇ (skip_showmode())
	    ਵਾਪਸ 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ਜੇ (clear_cmdline && cmdline_ਪੰਕਤੀ < ਪੰਕਤੀਆਂ - 1)
	    ਸੁਨੇਹਾ_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	ਸੁਨੇਹਾ_pos_ਬਹੁਲਕ();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ਜੇ (do_ਬਹੁਲਕ)
	{
	    ਸੁਨੇਹਾ_puts_attr("--", attr);
#ਜੇ defined(FEAT_XIM)
	    ਜੇ (
# ifdef FEAT_GUI_GTK
		    preedit_get_ਸਥਿਤੀ()
# ਨਹੀਂਤਾਂ
		    im_get_ਸਥਿਤੀ()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		ਸੁਨੇਹਾ_puts_attr(" IM", attr);
# ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts_attr(" XIM", attr);
# endif
#ਜੇ_ਅੰਤ
	    // CTRL-X in Insert mode
	    ਜੇ (edit_submode != ਨਿਰਾ && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		ਲੰਮਾਈ = (ਪੰਕਤੀਆਂ - ਸੁਨੇਹਾ_ਪੰਕਤੀ) * cmdline_ਚੌੜਾਈ - 3;
		ਜੇ (edit_submode_extra != ਨਿਰਾ)
		    ਲੰਮਾਈ -= vim_strsize(edit_submode_extra);
		ਜੇ (ਲੰਮਾਈ > 0)
		{
		    ਜੇ (edit_submode_pre != ਨਿਰਾ)
			ਲੰਮਾਈ -= vim_strsize(edit_submode_pre);
		    ਜੇ (ਲੰਮਾਈ - vim_strsize(edit_submode) > 0)
		    {
			ਜੇ (edit_submode_pre != ਨਿਰਾ)
			    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)edit_submode_pre, attr);
			ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)edit_submode, attr);
		    }
		    ਜੇ (edit_submode_extra != ਨਿਰਾ)
		    {
			ਸੁਨੇਹਾ_puts_attr(" ", attr);  // add a space in between
			ਜੇ ((ਪੂਰਨ)edit_submode_highl < (ਪੂਰਨ)HLF_ਗਿਣਤੀ)
			    sub_attr = HL_ATTR(edit_submode_highl);
			ਨਹੀਂਤਾਂ
			    sub_attr = attr;
			ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (State & VREPLACE_FLAG)
		    ਸੁਨੇਹਾ_puts_attr(_(" VREPLACE"), attr);
		ਨਹੀਂਤਾਂ ਜੇ (State & REPLACE_FLAG)
		    ਸੁਨੇਹਾ_puts_attr(_(" REPLACE"), attr);
		ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    ਜੇ (p_ri)
			ਸੁਨੇਹਾ_puts_attr(_(" REVERSE"), attr);
#ਜੇ_ਅੰਤ
		    ਸੁਨੇਹਾ_puts_attr(_(" INSERT"), attr);
		}
		ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    ਸੁਨੇਹਾ_puts_attr(_(" (insert)"), attr);
		ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'R')
		    ਸੁਨੇਹਾ_puts_attr(_(" (replace)"), attr);
		ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'V')
		    ਸੁਨੇਹਾ_puts_attr(_(" (vreplace)"), attr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (p_hkmap)
		    ਸੁਨੇਹਾ_puts_attr(_(" Hebrew"), attr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
		ਜੇ (State & ਬਹੁਲਕ_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ਜੇ (curwin->w_p_arab)
			ਸੁਨੇਹਾ_puts_attr(_(" Arabic"), attr);
		    ਨਹੀਂਤਾਂ
# endif
			ਜੇ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)NameBuff, attr);
		}
#ਜੇ_ਅੰਤ
		ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && p_paste)
		    ਸੁਨੇਹਾ_puts_attr(_(" (paste)"), attr);

		ਜੇ (VIsual_ਸਰਗਰਮ)
		{
		    ਅੱਖਰ *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    ਚੋਣ ((VIsual_select ? 4 : 0)
			    + (VIsual_ਬਹੁਲਕ == Ctrl_V) * 2
			    + (VIsual_ਬਹੁਲਕ == 'V'))
		    {
			ਹਾਲਤ 0:	p = N_(" VISUAL"); ਰੋਕੋ;
			ਹਾਲਤ 1: p = N_(" VISUAL LINE"); ਰੋਕੋ;
			ਹਾਲਤ 2: p = N_(" VISUAL BLOCK"); ਰੋਕੋ;
			ਹਾਲਤ 4: p = N_(" SELECT"); ਰੋਕੋ;
			ਹਾਲਤ 5: p = N_(" SELECT LINE"); ਰੋਕੋ;
			ਮੂਲ: p = N_(" SELECT BLOCK"); ਰੋਕੋ;
		    }
		    ਸੁਨੇਹਾ_puts_attr(_(p), attr);
		}
		ਸੁਨੇਹਾ_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	ਜੇ (reg_recording != 0
		&& edit_submode == ਨਿਰਾ)    // otherwise it gets too long
	{
	    recording_ਬਹੁਲਕ(attr);
	    need_clear = TRUE;
	}

	ਬਹੁਲਕ_displayed = TRUE;
	ਜੇ (need_clear || clear_cmdline || redraw_ਬਹੁਲਕ)
	    ਸੁਨੇਹਾ_clr_eos();
	ਸੁਨੇਹਾ_didout = FALSE;		// overwrite this message
	ਲੰਮਾਈ = ਸੁਨੇਹਾ_ਖਾਨਾ;
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    ਨਹੀਂਤਾਂ ਜੇ (clear_cmdline && ਸੁਨੇਹਾ_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	ਸੁਨੇਹਾ_clr_cmdline();
    ਨਹੀਂਤਾਂ ਜੇ (redraw_ਬਹੁਲਕ)
    {
	ਸੁਨੇਹਾ_pos_ਬਹੁਲਕ();
	ਸੁਨੇਹਾ_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ਜੇ (VIsual_ਸਰਗਰਮ)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ਜੇ (redrawing() && lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_ਬਹੁਲਕ = FALSE;
    clear_cmdline = FALSE;

    ਵਾਪਸ ਲੰਮਾਈ;
}

/*
 * Position for a mode message.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_pos_ਬਹੁਲਕ(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    ਖਾਲੀ
unshowmode(ਪੂਰਨ force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ਜੇ (!redrawing() || (!force && char_avail() && !ਕੁੰਜੀ_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    ਨਹੀਂਤਾਂ
	clearmode();
}

/*
 * Clear the mode message.
 */
    ਖਾਲੀ
clearmode(ਖਾਲੀ)
{
    ਪੂਰਨ save_ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    ਪੂਰਨ save_ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;

    ਸੁਨੇਹਾ_pos_ਬਹੁਲਕ();
    ਜੇ (reg_recording != 0)
	recording_ਬਹੁਲਕ(HL_ATTR(HLF_CM));
    ਸੁਨੇਹਾ_clr_eos();

    ਸੁਨੇਹਾ_ਖਾਨਾ = save_ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਸੁਨੇਹਾ_ਪੰਕਤੀ = save_ਸੁਨੇਹਾ_ਪੰਕਤੀ;
}

    ਸਥਿਰ ਖਾਲੀ
recording_ਬਹੁਲਕ(ਪੂਰਨ attr)
{
    ਸੁਨੇਹਾ_puts_attr(_("recording"), attr);
    ਜੇ (shortmess(SHM_RECORDING))
	ਵਾਪਸ;

    ਅੱਖਰ s[4];

    sprintf(s, " @%c", reg_recording);
    ਸੁਨੇਹਾ_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ਸਥਿਰ ਪੂਰਨ
mouse_has_any(ਖਾਲੀ)
{
    ਵਾਪਸ mouse_has(MOUSE_ਸਾਧਾਰਨ) || mouse_has(MOUSE_ਪਾਓ)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    ਖਾਲੀ
draw_tabline(ਖਾਲੀ)
{
    ਪੂਰਨ		tabcount = 0;
    tabpage_T	*tp;
    ਪੂਰਨ		tabwidth;
    ਪੂਰਨ		ਖਾਨਾ = 0;
    ਪੂਰਨ		scol = 0;
    ਪੂਰਨ		attr;
    win_T	*wp;
    win_T	*cwp;
    ਪੂਰਨ		wincount;
    ਪੂਰਨ		modified;
    ਪੂਰਨ		c;
    ਪੂਰਨ		len;
    ਪੂਰਨ		attr_sel;
    ਪੂਰਨ		attr_nosel = 0;
    ਪੂਰਨ		attr_fill = 0;
    char_u	*p;
    ਪੂਰਨ		ਗੁੰਜਾਇਸ਼;
    ਪੂਰਨ		use_sep_chars = (t_colors < 8
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
					    && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
					    && !p_tgc
#ਜੇ_ਅੰਤ
					    );
    bool	override_success;

#ਜੇ defined(FEAT_TABPANEL)
    ਖਾਨਾ = firstwin->w_wincol;
#ਜੇ_ਅੰਤ

    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;
    redraw_tabline = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ਜੇ (gui_use_tabline())
    {
	gui_update_tabline();
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (tabline_ਉਚਾਈ() < 1)
	ਵਾਪਸ;

#ਜੇ defined(FEAT_STL_OPT)
    clear_Tab_ਪੰਨਾ_Idxs();

    // Use the 'tabline' option if it's set.
    ਜੇ (*p_tal != NUL)
	win_redr_custom(ਨਿਰਾ, FALSE);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_ਚੌੜਾਈ - 1 + tabcount / 2) / tabcount;
	ਜੇ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ && ਖਾਨਾ < Columns - 4;
							     tp = tp->tp_ਅਗਲਾ)
	{
	    scol = ਖਾਨਾ;

	    override_success = ਧੱਕੋ_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    ਜੇ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ਜੇ (use_sep_chars && ਖਾਨਾ > 0)
		screen_putchar('|', 0, ਖਾਨਾ++, attr);

	    ਜੇ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, ਖਾਨਾ++, attr);

	    ਜੇ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    ਲਈ (wincount = 0; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ, ++wincount)
		ਜੇ (buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ))
		    modified = TRUE;
	    ਜੇ (modified || wincount > 1)
	    {
		ਜੇ (wincount > 1)
		{
		    len = vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL, "%d", wincount);
		    ਜੇ (ਖਾਨਾ + len >= Columns - 3)
			ਰੋਕੋ;
		    screen_puts_len(NameBuff, len, 0, ਖਾਨਾ,
#ਜੇ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#ਨਹੀਂਤਾਂ
					 attr
#ਜੇ_ਅੰਤ
					       );
		    ਖਾਨਾ += len;
		}
		ਜੇ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, ਖਾਨਾ++, attr);
		screen_putchar(' ', 0, ਖਾਨਾ++, attr);
	    }

	    ਗੁੰਜਾਇਸ਼ = scol - ਖਾਨਾ + tabwidth - 1;
	    ਜੇ (ਗੁੰਜਾਇਸ਼ > 0)
	    {
		ਪੂਰਨ n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_ਬਫਰ);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ਜੇ (has_mbyte)
		    ਜਦੋਂ (len > ਗੁੰਜਾਇਸ਼)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		ਨਹੀਂਤਾਂ ਜੇ (len > ਗੁੰਜਾਇਸ਼)
		{
		    p += len - ਗੁੰਜਾਇਸ਼;
		    len = ਗੁੰਜਾਇਸ਼;
		}
		n = Columns - ਖਾਨਾ - 1;
		ਜੇ (len > n)
		    len = n;

		screen_puts_len(p, (ਪੂਰਨ)STRLEN(p), 0, ਖਾਨਾ, attr);
		ਖਾਨਾ += len;
	    }
	    screen_putchar(' ', 0, ਖਾਨਾ++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    ਜਦੋਂ (scol < ਖਾਨਾ)
		Tab_ਪੰਨਾ_Idxs[scol++] = tabcount;

	    ਜੇ (override_success)
		ਕੱਢੋ_highlight_overrides();
	}

	ਜੇ (use_sep_chars)
	    c = '_';
	ਨਹੀਂਤਾਂ
	    c = ' ';
	screen_fill(0, 1, ਖਾਨਾ, (ਪੂਰਨ)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ਜੇ (p_sc && *p_sloc == 't')
	{
	    ਪੂਰਨ	n = (ਪੂਰਨ)Columns - ਖਾਨਾ - (tabcount > 1) * 3;
	    ਪੂਰਨ	ਚੌੜਾਈ = MIN(10, n);

	    ਜੇ (ਚੌੜਾਈ > 0)
		screen_puts_len(showcmd_buf, ਚੌੜਾਈ, 0, (ਪੂਰਨ)Columns
			    - ਚੌੜਾਈ - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ਜੇ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (ਪੂਰਨ)Columns - 1, attr_nosel);
	    Tab_ਪੰਨਾ_Idxs[Columns - 1] = -999;
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
    ਖਾਲੀ
get_trans_bufname(buf_T *buf)
{
    ਜੇ (buf_spname(buf) != ਨਿਰਾ)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    ਨਹੀਂਤਾਂ
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    ਪੂਰਨ
fillchar_ਸਥਿਤੀ(ਪੂਰਨ *attr, win_T *wp)
{
    ਪੂਰਨ	    fill;
    bool    override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (bt_terminal(wp->w_ਬਫਰ))
    {
	ਜੇ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	ਨਹੀਂਤਾਂ
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    ਨਹੀਂਤਾਂ
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
    ਵਾਪਸ fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ਸਥਿਰ bool
vsep_ਪੰਕਤੀ_is_curwin(win_T *wp, ਪੂਰਨ ਪੰਕਤੀ)
{
    ਜੇ (wp == curwin)
	ਵਾਪਸ true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ਜੇ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_ਚੌੜਾਈ
	    && ਪੰਕਤੀ >= curwin->w_winrow
	    && ਪੰਕਤੀ < W_WINROW(curwin) + curwin->w_ਉਚਾਈ
						   + curwin->w_ਸਥਿਤੀ_ਉਚਾਈ)
	ਵਾਪਸ true;

    ਵਾਪਸ false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    ਪੂਰਨ
fillchar_vsep(ਪੂਰਨ *attr, win_T *wp, ਪੂਰਨ ਪੰਕਤੀ)
{
    bool override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ਜੇ (vsep_ਪੰਕਤੀ_is_curwin(wp, ਪੰਕਤੀ))
	*attr = HL_ATTR(HLF_C);
    ਨਹੀਂਤਾਂ
	*attr = HL_ATTR(HLF_CNC);
    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();

    ਜੇ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	ਵਾਪਸ '|';
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    ਪੂਰਨ
redrawing(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (disable_redraw_for_testing)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ((RedrawingDisabled == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#ਜੇ_ਅੰਤ
		) && !(p_lz && char_avail() && !ਕੁੰਜੀ_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    ਪੂਰਨ
messaging(ਖਾਲੀ)
{
    ਵਾਪਸ (!(p_lz && char_avail() && !ਕੁੰਜੀ_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਨਾ_RULER 17	    // columns needed by standard ruler

    ਖਾਲੀ
comp_ਖਾਨਾ(ਖਾਲੀ)
{
    ਪੂਰਨ ਆਖਰੀ_has_ਸਥਿਤੀ = ਆਖਰੀ_stl_ਉਚਾਈ(FALSE) > 0;

    sc_ਖਾਨਾ = 0;
    ru_ਖਾਨਾ = 0;
    ਜੇ (p_ru)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	ru_ਖਾਨਾ = (ru_wid ? ru_wid : ਖਾਨਾ_RULER) + 1;
#ਨਹੀਂਤਾਂ
	ru_ਖਾਨਾ = ਖਾਨਾ_RULER + 1;
#ਜੇ_ਅੰਤ
	// no last status line, adjust sc_col
	ਜੇ (!ਆਖਰੀ_has_ਸਥਿਤੀ)
	    sc_ਖਾਨਾ = ru_ਖਾਨਾ;
    }
    ਜੇ (p_sc && *p_sloc == 'l')
    {
	sc_ਖਾਨਾ += SHOWCMD_ਖਾਨੇ_ਗਿਣਤੀ;
	ਜੇ (!p_ru || ਆਖਰੀ_has_ਸਥਿਤੀ)	    // no need for separating space
	    ++sc_ਖਾਨਾ;
    }
    sc_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - sc_ਖਾਨਾ;
    ru_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - ru_ਖਾਨਾ;
    ਜੇ (sc_ਖਾਨਾ <= 0)		// screen too narrow, will become a mess
	sc_ਖਾਨਾ = 1;
    ਜੇ (ru_ਖਾਨਾ <= 0)
	ru_ਖਾਨਾ = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_ECHOSPACE, sc_ਖਾਨਾ - 1);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    ਪੂਰਨ
ਸੰਖਿਆ_ਚੌੜਾਈ(win_T *wp)
{
    ਪੂਰਨ		n;
    linenr_T	lnum;

    ਜੇ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_ਉਚਾਈ;
    ਨਹੀਂਤਾਂ
	// cursor line shows absolute line number
	lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (lnum == wp->w_nrwidth_ਸਤਰ_ਗਿਣਤੀ && wp->w_nuw_cached == wp->w_p_nuw)
	ਵਾਪਸ wp->w_nrwidth_ਚੌੜਾਈ;
    wp->w_nrwidth_ਸਤਰ_ਗਿਣਤੀ = lnum;

    n = 0;
    ਕਰੋ
    {
	lnum /= 10;
	++n;
    } ਜਦੋਂ (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ਜੇ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ਜੇ (n < 2 && get_ਪਹਿਲਾ_valid_sign(wp) != ਨਿਰਾ
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_ਚੌੜਾਈ = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    ਵਾਪਸ n;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    ਪੂਰਨ
screen_screencol(ਖਾਲੀ)
{
    ਵਾਪਸ screen_cur_ਖਾਨਾ;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    ਪੂਰਨ
screen_screenrow(ਖਾਲੀ)
{
    ਵਾਪਸ screen_cur_ਪੰਕਤੀ;
}
#ਜੇ_ਅੰਤ

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ਸਥਿਰ ਪੂਰਨ
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ਜੇ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T ਅੰਕ = 0;
	ਪੂਰਨ	    ਬਾਈਟਾਂ;
	ਪੂਰਨ	    n;

	ਲਈ (ਬਾਈਟਾਂ = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; ਬਾਈਟਾਂ > 0; --ਬਾਈਟਾਂ)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ਜੇ (n < 0)
		ਵਾਪਸ 0;
	    ਅੰਕ = ਅੰਕ * 256 + n;
	}
	*p += 2;
	ਵਾਪਸ ਅੰਕ;
    }
    ਵਾਪਸ mb_ptr2char_adv(p);
}

ਬਣਤਰ charstab
{
    ਪੂਰਨ	    *cp;
    string_T	name;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHARSTAB_ਇੰਦਰਾਜ(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ਸਥਿਰ fill_chars_T fill_chars;
ਸਥਿਰ ਬਣਤਰ charstab filltab[] =
{
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.stl,	    "stl"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.vert,	    "vert"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.fold,	    "fold"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.diff,	    "diff"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.eob,	    "eob"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.lastline,    "lastline"),
#ਜੇ defined(FEAT_TABPANEL)
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.tpl_vert,    "tpl_vert"),
#ਜੇ_ਅੰਤ
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ਇੰਦਰਾਜ(&fill_chars.truncrl,	    "truncrl"),
};
ਸਥਿਰ lcs_chars_T lcs_chars;
ਸਥਿਰ ਬਣਤਰ charstab lcstab[] =
{
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.eol,	    "eol"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.ext,	    "extends"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.space,	    "space"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.trail,	    "trail"),
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.lead,	    "lead"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    CHARSTAB_ਇੰਦਰਾਜ(&lcs_chars.conceal,	    "conceal"),
#ਨਹੀਂਤਾਂ
    CHARSTAB_ਇੰਦਰਾਜ(ਨਿਰਾ,		    "conceal"),
#ਜੇ_ਅੰਤ
    CHARSTAB_ਇੰਦਰਾਜ(ਨਿਰਾ,		    "multispace"),
    CHARSTAB_ਇੰਦਰਾਜ(ਨਿਰਾ,		    "leadmultispace")
};

    ਸਥਿਰ ਅੱਖਰ *
ਖੇਤਰ_ਮੁੱਲ_err(ਅੱਖਰ *errbuf, size_t errbuflen, ਅੱਖਰ *fmt, char_u *ਖੇਤਰ)
{
    ਜੇ (errbuf == ਨਿਰਾ)
	ਵਾਪਸ "";
    vim_snprintf(errbuf, errbuflen, _(fmt), ਖੇਤਰ);
    ਵਾਪਸ errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ਸਥਿਰ ਅੱਖਰ *
ਸੈੱਟ_chars_option(win_T *wp, char_u *ਮੁੱਲ, ਪੂਰਨ is_listchars, ਪੂਰਨ apply,
						ਅੱਖਰ *errbuf, size_t errbuflen)
{
    ਪੂਰਨ	    ਗੇੜਾ, i, entries;
    char_u  *p, *s;
    ਪੂਰਨ	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *ਆਖਰੀ_multispace = ਨਿਰਾ;  // Last occurrence of "multispace:"
    char_u  *ਆਖਰੀ_lmultispace = ਨਿਰਾ; // Last occurrence of "leadmultispace:"
    ਪੂਰਨ	    multispace_len = 0;	      // Length of lcs-multispace string
    ਪੂਰਨ	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    ਬਣਤਰ charstab *tab;

    ਜੇ (is_listchars)
    {
	tab = lcstab;
	CLEAR_ਖੇਤਰ(lcs_chars);
	entries = ARRAY_ਲੰਮਾਈ(lcstab);
	ਜੇ (wp->w_p_lcs[0] == NUL)
	    ਮੁੱਲ = p_lcs;  // local value is empty, use the global value
    }
    ਨਹੀਂਤਾਂ
    {
	tab = filltab;
	entries = ARRAY_ਲੰਮਾਈ(filltab);
	ਜੇ (wp->w_p_fcs[0] == NUL)
	    ਮੁੱਲ = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    ਲਈ (ਗੇੜਾ = 0; ਗੇੜਾ <= (apply ? 1 : 0); ++ਗੇੜਾ)
    {
	ਪੂਰਨ has_tab = FALSE, has_leadtab = FALSE;

	ਜੇ (ਗੇੜਾ > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ਜੇ (is_listchars)
	    {
		ਲਈ (i = 0; i < entries; ++i)
		    ਜੇ (tab[i].cp != ਨਿਰਾ)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ਜੇ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(ਪੂਰਨ, multispace_len + 1);
		    ਜੇ (lcs_chars.multispace != ਨਿਰਾ)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		ਨਹੀਂਤਾਂ
		    lcs_chars.multispace = ਨਿਰਾ;

		ਜੇ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(ਪੂਰਨ, lead_multispace_len + 1);
		    ਜੇ (lcs_chars.leadmultispace != ਨਿਰਾ)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		ਨਹੀਂਤਾਂ
		    lcs_chars.leadmultispace = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
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
#ਜੇ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#ਜੇ_ਅੰਤ
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = ਮੁੱਲ;
	ਜਦੋਂ (*p)
	{
	    ਲਈ (i = 0; i < entries; ++i)
	    {
		ਜੇ (!(STRNCMP(p, tab[i].name.string, tab[i].name.ਲੰਮਾਈ) == 0 && p[tab[i].name.ਲੰਮਾਈ] == ':'))
		    ਜਾਰੀ;

		s = p + tab[i].name.ਲੰਮਾਈ + 1;

		ਜੇ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ਜੇ (ਗੇੜਾ == 0)
		    {
			// Get length of lcs-multispace string in first round
			ਆਖਰੀ_multispace = p;
			multispace_len = 0;
			ਜਦੋਂ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ਜੇ (char2cells(c1) > 1)
				ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
					 e_wrong_character_ਚੌੜਾਈ_for_ਖੇਤਰ_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ਜੇ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
				    e_wrong_ਸੰਖਿਆ_ਦਾ_characters_for_ਖੇਤਰ_str,
				    tab[i].name.string);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ multispace_pos = 0;

			ਜਦੋਂ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ਜੇ (p == ਆਖਰੀ_multispace && lcs_chars.multispace != ਨਿਰਾ)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ਰੋਕੋ;
		}

		ਜੇ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ਜੇ (ਗੇੜਾ == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			//ਗੇੜਾ
			ਆਖਰੀ_lmultispace = p;
			lead_multispace_len = 0;
			ਜਦੋਂ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ਜੇ (char2cells(c1) > 1)
				ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
					 e_wrong_character_ਚੌੜਾਈ_for_ਖੇਤਰ_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ਜੇ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
				    e_wrong_ਸੰਖਿਆ_ਦਾ_characters_for_ਖੇਤਰ_str,
				    tab[i].name.string);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ multispace_pos = 0;

			ਜਦੋਂ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ਜੇ (p == ਆਖਰੀ_lmultispace && lcs_chars.leadmultispace != ਨਿਰਾ)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ਰੋਕੋ;
		}

		c2 = c3 = 0;
		ਜੇ (*s == NUL)
		    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
				    e_wrong_ਸੰਖਿਆ_ਦਾ_characters_for_ਖੇਤਰ_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ਜੇ (char2cells(c1) > 1)
		    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
					 e_wrong_character_ਚੌੜਾਈ_for_ਖੇਤਰ_str,
					 tab[i].name.string);
		ਜੇ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ਜੇ (*s == NUL)
			ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
				    e_wrong_ਸੰਖਿਆ_ਦਾ_characters_for_ਖੇਤਰ_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ਜੇ (char2cells(c2) > 1)
			ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
					 e_wrong_character_ਚੌੜਾਈ_for_ਖੇਤਰ_str,
					 tab[i].name.string);
		    ਜੇ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ਜੇ (char2cells(c3) > 1)
			    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
					 e_wrong_character_ਚੌੜਾਈ_for_ਖੇਤਰ_str,
					 tab[i].name.string);
		    }
		    ਜੇ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    ਨਹੀਂਤਾਂ  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ਜੇ (*s == ',' || *s == NUL)
		{
		    ਜੇ (ਗੇੜਾ > 0)
		    {
			ਜੇ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			ਨਹੀਂਤਾਂ ਜੇ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			ਨਹੀਂਤਾਂ ਜੇ (tab[i].cp != ਨਿਰਾ)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    ਵਾਪਸ ਖੇਤਰ_ਮੁੱਲ_err(errbuf, errbuflen,
				    e_wrong_ਸੰਖਿਆ_ਦਾ_characters_for_ਖੇਤਰ_str,
				    tab[i].name.string);
	    }

	    ਜੇ (i == entries)
		ਵਾਪਸ e_invalid_ਦਲੀਲ;

	    ਜੇ (*p == ',')
		++p;
	}

	ਜੇ (is_listchars && has_leadtab && !has_tab)
	    ਵਾਪਸ e_leadtab_requires_tab;
    }

    ਜੇ (apply)
    {
	ਜੇ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    ਵਾਪਸ ਨਿਰਾ;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    ਅੱਖਰ *
ਸੈੱਟ_fillchars_option(win_T *wp, char_u *val, ਪੂਰਨ apply, ਅੱਖਰ *errbuf,
							      size_t errbuflen)
{
    ਵਾਪਸ ਸੈੱਟ_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    ਅੱਖਰ *
ਸੈੱਟ_listchars_option(win_T *wp, char_u *val, ਪੂਰਨ apply, ਅੱਖਰ *errbuf,
							      size_t errbuflen)
{
    ਵਾਪਸ ਸੈੱਟ_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(filltab))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(lcstab))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    ਅੱਖਰ *
check_chars_options(ਖਾਲੀ)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ਜੇ (ਸੈੱਟ_listchars_option(curwin, p_lcs, FALSE, ਨਿਰਾ, 0) != ਨਿਰਾ)
	ਵਾਪਸ e_conflicts_with_ਮੁੱਲ_ਦਾ_listchars;
    ਜੇ (ਸੈੱਟ_fillchars_option(curwin, p_fcs, FALSE, ਨਿਰਾ, 0) != ਨਿਰਾ)
	ਵਾਪਸ e_conflicts_with_ਮੁੱਲ_ਦਾ_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ਜੇ (ਸੈੱਟ_listchars_option(wp, wp->w_p_lcs, FALSE, ਨਿਰਾ, 0) != ਨਿਰਾ)
	    ਵਾਪਸ e_conflicts_with_ਮੁੱਲ_ਦਾ_listchars;
	ਜੇ (ਸੈੱਟ_fillchars_option(wp, wp->w_p_fcs, FALSE, ਨਿਰਾ, 0) != ਨਿਰਾ)
	    ਵਾਪਸ e_conflicts_with_ਮੁੱਲ_ਦਾ_fillchars;
    }
    ਵਾਪਸ ਨਿਰਾ;
}
