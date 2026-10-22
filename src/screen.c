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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	screen_attr = 0;

ಸ್ಥಿರ ಶೂನ್ಯ screen_char_2(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screenclear2(ಪೂರ್ಣಾಂಕ doclear);
ಸ್ಥಿರ ಶೂನ್ಯ lineclear(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ lineinvalid(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಗಲ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win_do_ಸಾಲುಗಳು(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಸಾಲು_ಎಣಿಕೆ, ಪೂರ್ಣಾಂಕ mayclear, ಪೂರ್ಣಾಂಕ del, ಪೂರ್ಣಾಂಕ clear_attr);
ಸ್ಥಿರ ಶೂನ್ಯ win_rest_invalid(win_T *wp);
ಸ್ಥಿರ ಶೂನ್ಯ ಸಂದೇಶ_pos_ಬಹುಲಕ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ recording_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ attr);

// Ugly global: overrule attribute used by screen_char()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screen_char_attr = 0;

#ಆದರೆ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    ಪೂರ್ಣಾಂಕ
conceal_cursor_ಸಾಲು(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (*wp->w_p_cocu == NUL)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (get_real_state() & ಬಹುಲಕ_VISUAL)
	c = 'v';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	c = 'i';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	c = 'n';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	c = 'c';
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ vim_strchr(wp->w_p_cocu, c) != ಖಾಲಿ;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    ಶೂನ್ಯ
conceal_check_cursor_ಸಾಲು(ಪೂರ್ಣಾಂಕ was_concealed)
{
    ಆದರೆ (curwin->w_p_cole <= 0 || conceal_cursor_ಸಾಲು(curwin) == was_concealed)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ wcol = curwin->w_wcol;

    need_cursor_ಸಾಲು_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ಆದರೆ (!was_concealed)
	curwin->w_wcol = wcol;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    ಪೂರ್ಣಾಂಕ
get_win_attr(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ಆದರೆ (win_attr != 0)
    {
	ಆದರೆ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	ಇಲ್ಲದಿದ್ದರೆ
	    win_attr = 0;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(wp))
    {
	ಆದರೆ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	ಇಲ್ಲದಿದ್ದರೆ
	    win_attr = HL_ATTR(HLF_ತೆಗೆ);    // Popup
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
    ಹಿಂತಿರುಗಿ win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
screen_fill_ಕೊನೆ(
	win_T *wp,
	ಪೂರ್ಣಾಂಕ	c1,
	ಪೂರ್ಣಾಂಕ	c2,
	ಪೂರ್ಣಾಂಕ	off,
	ಪೂರ್ಣಾಂಕ	ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	endrow,
	ಪೂರ್ಣಾಂಕ	attr)
{
    ಪೂರ್ಣಾಂಕ	    nn = off + ಅಗಲ;

    ಆದರೆ (nn > wp->w_ಅಗಲ)
	nn = wp->w_ಅಗಲ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (ಪೂರ್ಣಾಂಕ)wp->w_wincol + nn,
		c1, c2, attr);
    ಹಿಂತಿರುಗಿ nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    ಶೂನ್ಯ
win_draw_ಕೊನೆ(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		c1,
    ಪೂರ್ಣಾಂಕ		c2,
    ಪೂರ್ಣಾಂಕ		draw_margin,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		endrow,
    hlf_T	hl)
{
    ಪೂರ್ಣಾಂಕ		n = 0;
    ಪೂರ್ಣಾಂಕ		attr = HL_ATTR(hl);
    ಪೂರ್ಣಾಂಕ		win_attr = get_win_attr(wp);
    bool	override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ಆದರೆ (draw_margin)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಪೂರ್ಣಾಂಕ	fdc = compute_foldcolumn(wp, 0);

	ಆದರೆ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_ಕೊನೆ(wp, ' ', ' ', n, fdc,
		      ಅಡ್ಡಸಾಲು, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	ಆದರೆ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_ಕೊನೆ(wp, ' ', ' ', n, 2,
		      ಅಡ್ಡಸಾಲು, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == ಖಾಲಿ)
	    // draw the number column
	    n = screen_fill_ಕೊನೆ(wp, ' ', ' ', n, ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1,
		       ಅಡ್ಡಸಾಲು, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp), c1, c2, attr);
    }

    ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(wp, ಅಡ್ಡಸಾಲು);

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    ಪೂರ್ಣಾಂಕ
compute_foldcolumn(win_T *wp, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    ಪೂರ್ಣಾಂಕ n = wp->w_ಅಗಲ - (ಲಂಬಸಾಲು + wmw);

    ಹಿಂತಿರುಗಿ MIN(wp->w_p_fdc, n);
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
    ಪೂರ್ಣಾಂಕ		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		level;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_level;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2;
    ಪೂರ್ಣಾಂಕ		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    ಪೂರ್ಣಾಂಕ		ಚಿಹ್ನೆ = 0;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    ಖಾಲಿಗಳು_2 = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    ಮೊದಲ_level = level - fdc - closed + 1 + ಖಾಲಿಗಳು_2;
    ಆದರೆ (ಮೊದಲ_level < 1)
	ಮೊದಲ_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    ಗಾಗಿ (i = 0; i < n; i++)
    {
	ಆದರೆ (win_foldinfo.fi_lnum == lnum
		&& ಮೊದಲ_level + i >= win_foldinfo.fi_ಕೆಳಗಿನ_level)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldopen;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_level == 1)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldsep;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_fill_chars.foldinner != NUL)
	    ಚಿಹ್ನೆ = wp->w_fill_chars.foldinner;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_level + i <= 9)
	    ಚಿಹ್ನೆ = '0' + ಮೊದಲ_level + i;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಚಿಹ್ನೆ = '>';

	len = utf_char2bytes(ಚಿಹ್ನೆ, &p[byte_counter]);
	byte_counter += len;
	ಆದರೆ (ಮೊದಲ_level + i >= level)
	{
	    i++;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (closed)
    {
	ಆದರೆ (ಚಿಹ್ನೆ != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ಆದರೆ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	ಚಿಹ್ನೆ = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(ಚಿಹ್ನೆ, &p[byte_counter]);
	byte_counter += len;
    }

    ಹಿಂತಿರುಗಿ MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
comp_char_differs(ಪೂರ್ಣಾಂಕ off_from, ಪೂರ್ಣಾಂಕ off_ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
    {
	ಆದರೆ (Screen_ಸಾಲುಗಳು_C[i][off_from] != Screen_ಸಾಲುಗಳು_C[i][off_ಗಮ್ಯ])
	    ಹಿಂತಿರುಗಿ TRUE;
	ಆದರೆ (Screen_ಸಾಲುಗಳು_C[i][off_from] == 0)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
char_needs_redraw(ಪೂರ್ಣಾಂಕ off_from, ಪೂರ್ಣಾಂಕ off_ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0
	    && ((Screen_ಸಾಲುಗಳು[off_from] != Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_ಗಮ್ಯ])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_ಗಮ್ಯ]
			: (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 1 && Screen_ಸಾಲುಗಳು[off_from + 1]
						 != Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ + 1])))
		|| (enc_utf8
		    && (Screen_ಸಾಲುಗಳು_UC[off_from] != Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ]
			|| (Screen_ಸಾಲುಗಳು_UC[off_from] != 0
			    && comp_char_differs(off_from, off_ಗಮ್ಯ))
			|| ((*mb_off2cells)(off_from, off_from + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) > 1
			    && Screen_ಸಾಲುಗಳು[off_from + 1]
						!= Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ + 1])))))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    ಪೂರ್ಣಾಂಕ
screen_get_current_ಸಾಲು_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
blocked_by_popup(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ off;

    ಆದರೆ (!popup_visible)
	ಹಿಂತಿರುಗಿ FALSE;
    off = ಅಡ್ಡಸಾಲು * screen_Columns + ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ popup_ಮುಖವಾಡ[off] > screen_zindex || popup_transparent[off];
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    ಶೂನ್ಯ
reset_screen_attr(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_for_popup(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ಆದರೆ (pum_under_menu(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, TRUE)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & ಬಹುಲಕ_CMDLINE) == 0
	    && pum_under_menu(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, FALSE))
	ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (pum_visible()
	    && (State & ಬಹುಲಕ_CMDLINE) == 0
	    && pum_under_menu(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, FALSE))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (blocked_by_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
ಸ್ಥಿರ char_u	*suppressed_cells = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	suppressed_ಅಡ್ಡಸಾಲುಗಳು = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
mark_suppressed_cell(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (suppressed_cells == ಖಾಲಿ
	    || suppressed_ಅಡ್ಡಸಾಲುಗಳು != screen_ಅಡ್ಡಸಾಲುಗಳು
	    || suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_ಅಡ್ಡಸಾಲುಗಳು * screen_Columns);
	ಆದರೆ (suppressed_cells == ಖಾಲಿ)
	{
	    suppressed_ಅಡ್ಡಸಾಲುಗಳು = 0;
	    suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
	    ಹಿಂತಿರುಗಿ;
	}
	suppressed_ಅಡ್ಡಸಾಲುಗಳು = screen_ಅಡ್ಡಸಾಲುಗಳು;
	suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen_Columns;
    }
    suppressed_cells[ಅಡ್ಡಸಾಲು * suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು] = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unmark_suppressed_cell(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (suppressed_cells != ಖಾಲಿ
	    && ಅಡ್ಡಸಾಲು >= 0 && ಅಡ್ಡಸಾಲು < suppressed_ಅಡ್ಡಸಾಲುಗಳು
	    && ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು < suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	suppressed_cells[ಅಡ್ಡಸಾಲು * suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು] = FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_suppressed_cell(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಹಿಂತಿರುಗಿ suppressed_cells != ಖಾಲಿ
	&& ಅಡ್ಡಸಾಲು >= 0 && ಅಡ್ಡಸಾಲು < suppressed_ಅಡ್ಡಸಾಲುಗಳು
	&& ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು < suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ
	&& suppressed_cells[ಅಡ್ಡಸಾಲು * suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು];
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		popup_attr;	// get_win_attr(screen_opacity_popup)
    ಪೂರ್ಣಾಂಕ		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
popup_ತಳ_attr_or(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ fallback)
{
    ಪೂರ್ಣಾಂಕ		ತಳ = fallback;

    popup_get_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಖಾಲಿ, &ತಳ, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ ತಳ;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
popup_blend_with_ತಳ(
	ಸ್ಥಿರಾಂಕ popup_opacity_T	*po,
	ಪೂರ್ಣಾಂಕ			char_attr,
	ಪೂರ್ಣಾಂಕ			ತಳ_attr,
	ಪೂರ್ಣಾಂಕ			blend_fg)
{
    ಪೂರ್ಣಾಂಕ		combined = hl_combine_attr(po->popup_attr, char_attr);

    ಹಿಂತಿರುಗಿ hl_blend_attr(ತಳ_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_wide_char_opacity_attrs(
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ col2,
	sattr_T *attr1, sattr_T *attr2)
{
    ಪೂರ್ಣಾಂಕ		bg1, bg2;
    ಪೂರ್ಣಾಂಕ		base1 = 0;
    ಪೂರ್ಣಾಂಕ		base2 = 0;
    attrentry_T	*ae;

    ಆದರೆ (*attr1 == *attr2)
	ಹಿಂತಿರುಗಿ;

    popup_get_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, col1, ಖಾಲಿ, &base1, ಖಾಲಿ);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != ಖಾಲಿ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ಇಲ್ಲದಿದ್ದರೆ
    bg1 = (ae != ಖಾಲಿ && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_ತಳ_screen_cell(ಅಡ್ಡಸಾಲು, col2, ಖಾಲಿ, &base2, ಖಾಲಿ);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != ಖಾಲಿ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# ಇಲ್ಲದಿದ್ದರೆ
    bg2 = (ae != ಖಾಲಿ && ae->ae_u.cterm.bg_color != 0);
# endif

    ಆದರೆ (bg1 && !bg2)
	*attr1 = *attr2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!bg1 && bg2)
	*attr2 = *attr1;
}
#ಆದರೆ_ಅಂತ್ಯ

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
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	coloff,
	ಪೂರ್ಣಾಂಕ	endcol,
	ಪೂರ್ಣಾಂಕ	clear_ಅಗಲ,
	colnr_T	ಕೊನೆಯ_vcol,
	ಪೂರ್ಣಾಂಕ	flags ಬಳಸದ)
{
    ಚಿಹ್ನೆರಹಿತ	    off_from;
    ಚಿಹ್ನೆರಹಿತ	    off_ಗಮ್ಯ;
    ಚಿಹ್ನೆರಹಿತ	    max_off_from;
    ಚಿಹ್ನೆರಹಿತ	    max_off_ಗಮ್ಯ;
    ಪೂರ್ಣಾಂಕ		    ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		    hl;
    ಪೂರ್ಣಾಂಕ		    force = FALSE;	// force update rest of the line
    ಪೂರ್ಣಾಂಕ		    redraw_this		// bool: does character need redraw?
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#ಆದರೆ_ಅಂತ್ಯ
				;
    ಪೂರ್ಣಾಂಕ		    redraw_ಮುಂದಿನ;	// redraw_this for next character
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಪೂರ್ಣಾಂಕ		    ಬದಲಾಯಿತು_this;	// TRUE if character changed
    ಪೂರ್ಣಾಂಕ		    ಬದಲಾಯಿತು_ಮುಂದಿನ;	// TRUE if next character changed
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		    clear_ಮುಂದಿನ = FALSE;
    ಪೂರ್ಣಾಂಕ		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    ಪೂರ್ಣಾಂಕ		    drawing_opacity_popup =
				screen_opacity_popup != ಖಾಲಿ
				&& (flags & SLF_POPUP);
    CLEAR_ಕ್ಷೇತ್ರ(po);
    ಆದರೆ (drawing_opacity_popup)
	popup_opacity_init(&po);
#ಆದರೆ_ಅಂತ್ಯ

    // Check for illegal row and col, just in case.
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಆದರೆ (endcol > Columns)
	endcol = Columns;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    clip_may_clear_selection(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ

    off_from = (ಚಿಹ್ನೆರಹಿತ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    off_ಗಮ್ಯ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_ಗಮ್ಯ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + screen_Columns;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ಆದರೆ (clear_ಅಗಲ > 0)
	{
	    ಪೂರ್ಣಾಂಕ clear_ಆರಂಭ = ಲಂಬಸಾಲು;

	    ಆಗ (ಲಂಬಸಾಲು <= endcol && Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] == ' '
		    && ScreenAttrs[off_ಗಮ್ಯ] == 0
				  && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] == 0))
	    {
		++off_ಗಮ್ಯ;
		++ಲಂಬಸಾಲು;
	    }
	    ಆದರೆ (ಲಂಬಸಾಲು <= endcol)
		screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = endcol; i >= clear_ಆರಂಭ; i--)
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_ಗಮ್ಯ + (i - ಲಂಬಸಾಲು)] =
		    (flags & SLF_INC_VCOL) ? ++ಕೊನೆಯ_vcol : ಕೊನೆಯ_vcol;
	}
	ಲಂಬಸಾಲು = endcol + 1;
	off_ಗಮ್ಯ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು + coloff;
	off_from += ಲಂಬಸಾಲು;
	endcol = (clear_ಅಗಲ > 0 ? clear_ಅಗಲ : -clear_ಅಗಲ);
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_RIGHTLEFT

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    ಆದರೆ (coloff > 0 && enc_utf8
		   && Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] == 0
		   && Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ - 1] != 0
		   && (*mb_char2cells)(Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ - 1]) > 1)
    {
	Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ - 1] = ' ';
	Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	ಆದರೆ (screen_opacity_popup == ಖಾಲಿ)
# endif
	    screen_char(off_ಗಮ್ಯ - 1, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff - 1);
    }
#ಆದರೆ_ಅಂತ್ಯ

    redraw_ಮುಂದಿನ = char_needs_redraw(off_from, off_ಗಮ್ಯ, endcol - ಲಂಬಸಾಲು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಬದಲಾಯಿತು_ಮುಂದಿನ = redraw_ಮುಂದಿನ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (ಲಂಬಸಾಲು < endcol)
    {
	ಆದರೆ (has_mbyte && (ಲಂಬಸಾಲು + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	ಇಲ್ಲದಿದ್ದರೆ
	    char_cells = 1;

	redraw_this = redraw_ಮುಂದಿನ;
	redraw_ಮುಂದಿನ = force || char_needs_redraw(off_from + char_cells,
			      off_ಗಮ್ಯ + char_cells, endcol - ಲಂಬಸಾಲು - char_cells);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ಆದರೆ (!redraw_this && is_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff))
	    redraw_this = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	ಬದಲಾಯಿತು_this = ಬದಲಾಯಿತು_ಮುಂದಿನ;
	ಬದಲಾಯಿತು_ಮುಂದಿನ = redraw_ಮುಂದಿನ;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	ಆದರೆ (redraw_ಮುಂದಿನ && gui.ಒಳ_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_ಗಮ್ಯ + char_cells];
	    ಆದರೆ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ಆದರೆ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
	// skip the second cell for double-width characters.
	ಆದರೆ (redraw_this && char_cells == 2 && skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ಆದರೆ (redraw_this && skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff))
	    redraw_this = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	ಪೂರ್ಣಾಂಕ opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ಆದರೆ (drawing_opacity_popup
		&& Screen_ಸಾಲುಗಳು[off_from] == ' '
		&& (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_from] == 0))
	{
	    ಪೂರ್ಣಾಂಕ popup_src_attr = ScreenAttrs[off_from];
	    ಪೂರ್ಣಾಂಕ bg_is_ಖಾಲಿಗಳು = Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] == 0
				&& (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ಆದರೆ (bg_is_ಖಾಲಿಗಳು
		    && off_ಗಮ್ಯ > 0
		    && enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ - 1] != 0
		    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] != 0
		    || (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] != 0))
	    {
		ಪೂರ್ಣಾಂಕ bg_char_cells = (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] != 0)
				? utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ]) : 1;

		ಆದರೆ (bg_char_cells == 2)
		{
		    ಆದರೆ (ಲಂಬಸಾಲು + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_ಗಮ್ಯ + 1 >= max_off_ಗಮ್ಯ)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			ಪೂರ್ಣಾಂಕ ತಳ_attr;
			Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = ' ';
			ಆದರೆ (enc_utf8)
			    Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] = 0;
			ತಳ_attr = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff,
							     ScreenAttrs[off_ಗಮ್ಯ]);
			ScreenAttrs[off_ಗಮ್ಯ] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, ತಳ_attr, TRUE);
			screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			ಹೋಗು skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    ಆದರೆ (!(Screen_ಸಾಲುಗಳು[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_from + 1] == 0)))
			ಹೋಗು skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		ಪೂರ್ಣಾಂಕ ತಳ_attr = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff,
							     ScreenAttrs[off_ಗಮ್ಯ]);
		ScreenAttrs[off_ಗಮ್ಯ] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, ತಳ_attr, TRUE);
		screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ಆದರೆ (bg_char_cells == 2)
		{
		    ಪೂರ್ಣಾಂಕ ತಳ_attr2 = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff + 1,
							 ScreenAttrs[off_ಗಮ್ಯ + 1]);
		    ScreenAttrs[off_ಗಮ್ಯ + 1] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, ತಳ_attr2, TRUE);
		    ಆದರೆ (po.blend == 100)
			resolve_wide_char_opacity_attrs(ಅಡ್ಡಸಾಲು,
				ಲಂಬಸಾಲು + coloff, ಲಂಬಸಾಲು + coloff + 1,
				&ScreenAttrs[off_ಗಮ್ಯ], &ScreenAttrs[off_ಗಮ್ಯ + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bg_is_ಖಾಲಿಗಳು)
	    {
		ಪೂರ್ಣಾಂಕ ತಳ_attr;
		Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = ' ';
		ತಳ_attr = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff,
							     ScreenAttrs[off_ಗಮ್ಯ]);
		ScreenAttrs[off_ಗಮ್ಯ] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, ತಳ_attr, TRUE);
		screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (redraw_this)
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
	    ಆದರೆ (       p_wiv
		    && !force
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		    && ScreenAttrs[off_ಗಮ್ಯ] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_ಗಮ್ಯ])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
		ಹೊರಹೋಗು_str(T_CE);		// clear rest of this screen line
		screen_ಆರಂಭ();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_ಮುಂದಿನ = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		ಆದರೆ (ಲಂಬಸಾಲು + coloff > 0 && ScreenAttrs[off_ಗಮ್ಯ - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_ಗಮ್ಯ - 1];
		    ಪದ_windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
		    screen_stop_highlight();
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ಆದರೆ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ಆದರೆ (char_cells == 1
			&& ಲಂಬಸಾಲು + 1 < endcol
			&& (*mb_off2cells)(off_ಗಮ್ಯ, max_off_ಗಮ್ಯ) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ + 1] = 0;
		    redraw_ಮುಂದಿನ = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (char_cells == 2
			&& ಲಂಬಸಾಲು + 2 < endcol
			&& (*mb_off2cells)(off_ಗಮ್ಯ, max_off_ಗಮ್ಯ) == 1
			&& (*mb_off2cells)(off_ಗಮ್ಯ + 1, max_off_ಗಮ್ಯ) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ + 2] = 0;
		    redraw_ಮುಂದಿನ = TRUE;
		}

		ಆದರೆ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_ಗಮ್ಯ] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ಆದರೆ (has_mbyte && ಲಂಬಸಾಲು + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_ಗಮ್ಯ, max_off_ಗಮ್ಯ) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_ಗಮ್ಯ, max_off_ಗಮ್ಯ) == 1
			    && (*mb_off2cells)(off_ಗಮ್ಯ + 1, max_off_ಗಮ್ಯ) > 1)))
		clear_ಮುಂದಿನ = TRUE;

	    Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = Screen_ಸಾಲುಗಳು[off_from];
	    ಆದರೆ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] = Screen_ಸಾಲುಗಳು_UC[off_from];
		ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[off_from] != 0)
		{
		    ಪೂರ್ಣಾಂಕ	    i;

		    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
			Screen_ಸಾಲುಗಳು_C[i][off_ಗಮ್ಯ] = Screen_ಸಾಲುಗಳು_C[i][off_from];
		}
	    }
	    ಆದರೆ (char_cells == 2)
	    {
		Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ + 1] = Screen_ಸಾಲುಗಳು[off_from + 1];
		ಆದರೆ (enc_utf8)
		    Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ + 1] = 0;
	    }

#ಆದರೆ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ಆದರೆ (
# ifdef FEAT_GUI
		    gui.ಒಳ_use
# endif
# ಆದರೆ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    ಪದ_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_ಗಮ್ಯ];
		ಆದರೆ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ಆದರೆ (hl & HL_BOLD)
		    redraw_ಮುಂದಿನ = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ಆದರೆ (gui.ಒಳ_use && ಬದಲಾಯಿತು_this)
		redraw_ಮುಂದಿನ = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ಆದರೆ (gui.directx_enabled && ಬದಲಾಯಿತು_this)
		redraw_this = TRUE;
# endif
#ಆದರೆ_ಅಂತ್ಯ

	    ScreenAttrs[off_ಗಮ್ಯ] = ScreenAttrs[off_from];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ಆದರೆ (drawing_opacity_popup && po.blend > 0)
	    {
		ಪೂರ್ಣಾಂಕ popup_src_attr = ScreenAttrs[off_from];
		ಪೂರ್ಣಾಂಕ scol1 = ಲಂಬಸಾಲು + coloff;
		ಪೂರ್ಣಾಂಕ base1 = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, scol1, 0);

		ಆದರೆ (char_cells == 2)
		{
		    ಪೂರ್ಣಾಂಕ base2 = popup_ತಳ_attr_or(ಅಡ್ಡಸಾಲು, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    ಪೂರ್ಣಾಂಕ over1 = popup_is_over_opacity(ಅಡ್ಡಸಾಲು, scol1);
		    ಪೂರ್ಣಾಂಕ over2 = popup_is_over_opacity(ಅಡ್ಡಸಾಲು, scol1 + 1);
		    ಆದರೆ (over1 != over2)
		    {
			ಆದರೆ (over1)
			    base1 = base2;
			ಇಲ್ಲದಿದ್ದರೆ
			    base2 = base1;
		    }
		    ScreenAttrs[off_ಗಮ್ಯ + 1] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_ಗಮ್ಯ] = popup_blend_with_ತಳ(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ಆದರೆ (char_cells == 2)
		ScreenAttrs[off_ಗಮ್ಯ + 1] = ScreenAttrs[off_ಗಮ್ಯ];

	    ಆದರೆ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
	    ಇಲ್ಲದಿದ್ದರೆ
		screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (  p_wiv
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		&& ಲಂಬಸಾಲು + coloff > 0 && off_ಗಮ್ಯ > 0)
	{
	    ಆದರೆ (ScreenAttrs[off_ಗಮ್ಯ] == ScreenAttrs[off_ಗಮ್ಯ - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (screen_attr != 0)
		screen_stop_highlight();
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (!opacity_blank)
#ಆದರೆ_ಅಂತ್ಯ
	{
	    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_ಗಮ್ಯ] = Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_from];
	    ಆದರೆ (char_cells == 2)
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_ಗಮ್ಯ + 1] = Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_from + 1];
	}

	off_ಗಮ್ಯ += char_cells;
	off_from += char_cells;
	ಲಂಬಸಾಲು += char_cells;
    }

    ಆದರೆ (clear_ಮುಂದಿನ && !skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = ' ';
	ಆದರೆ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (screen_opacity_popup == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
	    screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
    }

    ಆದರೆ (clear_ಅಗಲ > 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#ಆದರೆ_ಅಂತ್ಯ
				   )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು_2 = ಲಂಬಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

	// blank out the rest of the line
	ಆಗ (ಲಂಬಸಾಲು < clear_ಅಗಲ && Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] == ' '
						  && ScreenAttrs[off_ಗಮ್ಯ] == 0
				  && (!enc_utf8 || Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] == 0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
				  && !is_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff)
#ಆದರೆ_ಅಂತ್ಯ
				  )
	{
	    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_ಗಮ್ಯ] =
			      (flags & SLF_INC_VCOL) ? ++ಕೊನೆಯ_vcol : ಕೊನೆಯ_vcol;
	    ++off_ಗಮ್ಯ;
	    ++ಲಂಬಸಾಲು;
	}
	ಆದರೆ (ಲಂಬಸಾಲು < clear_ಅಗಲ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ಆದರೆ (gui.ಒಳ_use && (ಲಂಬಸಾಲು > ಆರಂಭ_ಲಂಬಸಾಲು_2 || !redraw_this))
	    {
		hl = ScreenAttrs[off_ಗಮ್ಯ];
		ಆದರೆ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    ಪೂರ್ಣಾಂಕ prev_cells = 1;

		    ಆದರೆ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ - 1] == 0 ? 2 : 1;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			ಚಿಹ್ನೆರಹಿತ off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು];
			ಚಿಹ್ನೆರಹಿತ max_off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + screen_Columns;

			ಆಗ (off < off_ಗಮ್ಯ)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ಆದರೆ (!skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff - prev_cells))
		    {
			ಆದರೆ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_ಗಮ್ಯ - prev_cells, ಅಡ್ಡಸಾಲು,
						   ಲಂಬಸಾಲು + coloff - prev_cells);
			ಇಲ್ಲದಿದ್ದರೆ
			    screen_char(off_ಗಮ್ಯ - prev_cells, ಅಡ್ಡಸಾಲು,
						   ಲಂಬಸಾಲು + coloff - prev_cells);
		    }
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, ಲಂಬಸಾಲು + coloff, clear_ಅಗಲ + coloff,
								 ' ', ' ', 0);
	    ಆಗ (ಲಂಬಸಾಲು < clear_ಅಗಲ)
	    {
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off_ಗಮ್ಯ++]
			    = (flags & SLF_INC_VCOL) ? ++ಕೊನೆಯ_vcol : ಕೊನೆಯ_vcol;
		++ಲಂಬಸಾಲು;
	    }
	}
    }

    ಆದರೆ (clear_ಅಗಲ > 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ಆದರೆ (coloff + ಲಂಬಸಾಲು < firstwin->w_wincol + topframe->fr_ಅಗಲ)
	{
	    ಆದರೆ (!skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff))
	    {
		ಪೂರ್ಣಾಂಕ c;

		c = fillchar_vsep(&hl, wp, ಅಡ್ಡಸಾಲು);
		ಆದರೆ (Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] != (schar_T)c
			|| (enc_utf8 && (ಪೂರ್ಣಾಂಕ)Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_ಗಮ್ಯ] != hl)
		{
		    Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = c;
		    ScreenAttrs[off_ಗಮ್ಯ] = hl;
		    ಆದರೆ (enc_utf8)
		    {
			ಆದರೆ (c >= 0x80)
			{
			    Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] = c;
			    Screen_ಸಾಲುಗಳು_C[0][off_ಗಮ್ಯ] = 0;
			    Screen_ಸಾಲುಗಳು[off_ಗಮ್ಯ] = 0x80; // avoid storing zero
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    Screen_ಸಾಲುಗಳು_UC[off_ಗಮ್ಯ] = 0;
		    }
		    screen_char(off_ಗಮ್ಯ, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + coloff);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಾಲು_Wraps[ಅಡ್ಡಸಾಲು] = FALSE;
    }

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಆದರೆ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    ಶೂನ್ಯ
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಂಕ		t;

    ಗಾಗಿ (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    ಶೂನ್ಯ
draw_vsep_win(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಆದರೆ (!wp->w_vsep_ಅಗಲ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ content_ಕೊನೆ = W_WINROW(wp) + wp->w_ಎತ್ತರ;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಅಡ್ಡಸಾಲು = (ಅಡ್ಡಸಾಲು == 0) ? wp->w_winrow : W_WINROW(wp) + ಅಡ್ಡಸಾಲು;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = ಆರಂಭ_ಅಡ್ಡಸಾಲು; r < content_ಕೊನೆ; ++r)
    {
	ಪೂರ್ಣಾಂಕ hl;
	ಪೂರ್ಣಾಂಕ c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
    {
	ಪೂರ್ಣಾಂಕ hl;
	ಪೂರ್ಣಾಂಕ c;
	ಆದರೆ (stl_connected(wp))
	    c = fillchar_ಸ್ಥಿತಿ(&hl, wp);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = fillchar_vsep(&hl, wp, content_ಕೊನೆ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = content_ಕೊನೆ; r < content_ಕೊನೆ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    ಪೂರ್ಣಾಂಕ
stl_connected(win_T *wp)
{
    ಚೌಕಟ್ಟು_T	*fr;

    fr = wp->w_ಚೌಕಟ್ಟು;
    ಆಗ (fr->fr_ಪೋಷಕ != ಖಾಲಿ)
    {
	ಆದರೆ (fr->fr_ಪೋಷಕ->fr_layout == FR_ಲಂಬಸಾಲು)
	{
	    ಆದರೆ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fr->fr_ಮುಂದಿನ != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE;
	}
	fr = fr->fr_ಪೋಷಕ;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    ಪೂರ್ಣಾಂಕ
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    ಪೂರ್ಣಾಂಕ		len)	    // length of buffer
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		plen;

    ಆದರೆ (wp->w_ಬಫರ್->b_p_iminsert != B_IMODE_LMAP)
	ಹಿಂತಿರುಗಿ 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	ಗಮ್ಯ_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_ಬಫರ್;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_ಗಮ್ಯ_string(ಗಮ್ಯ_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
#ಆದರೆ_ಅಂತ್ಯ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	ಆದರೆ (wp->w_ಬಫರ್->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_ಬಫರ್->b_p_keymap;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((ಅಕ್ಷರ *)buf, len, (ಅಕ್ಷರ *)fmt, p);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    vim_free(s);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    ಹಿಂತಿರುಗಿ plen;
}

#ಆದರೆ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    ಶೂನ್ಯ
win_redr_custom(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		draw_ruler)	// TRUE or FALSE
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = FALSE;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		curattr;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		maxwidth;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		stlh_cnt;
    ಪೂರ್ಣಾಂಕ		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    ಪೂರ್ಣಾಂಕ		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    ಪೂರ್ಣಾಂಕ		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ;
    entered = TRUE;

    // setup environment for the task at hand
    ಆದರೆ (wp == ಖಾಲಿ)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	ಅಡ್ಡಸಾಲು = 0;
	ಲಂಬಸಾಲು = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_ಅಗಲ;
	opt_name = (char_u *)"tabline";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ಅಡ್ಡಸಾಲು = statusline_ಅಡ್ಡಸಾಲು(wp);
	fillchar = fillchar_ಸ್ಥಿತಿ(&attr, wp);
	ಪೂರ್ಣಾಂಕ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2 = wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0;
	maxwidth = ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2 ? wp->w_ಅಗಲ : cmdline_ಅಗಲ;

	ಆದರೆ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ಆದರೆ (*stl == '%')
	    {
		ಆದರೆ (*++stl == '-')
		    stl++;
		ಆದರೆ (atoi((ಅಕ್ಷರ *)stl))
		    ಆಗ (VIM_ISDIGIT(*stl))
			stl++;
		ಆದರೆ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    ಲಂಬಸಾಲು = ru_ಲಂಬಸಾಲು - (cmdline_ಅಗಲ - maxwidth);
	    ಆದರೆ (ಲಂಬಸಾಲು < (maxwidth + 1) / 2)
		ಲಂಬಸಾಲು = (maxwidth + 1) / 2;
	    maxwidth -= ಲಂಬಸಾಲು;
	    ಆದರೆ (!ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2)
	    {
		ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    opt_name = (char_u *)"statusline";
	    ಆದರೆ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		stl = p_stl;
	}

	ಆದರೆ (ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2)
	    ಲಂಬಸಾಲು += wp->w_wincol;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಲಂಬಸಾಲು += cmdline_ಲಂಬಸಾಲು_off;
    }

    ಆದರೆ (maxwidth <= 0)
	ಹೋಗು theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == ಖಾಲಿ ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == ಖಾಲಿ) ? (char_u *)"" : stl;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_save = ಲಂಬಸಾಲು;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **ಹೊರಹೋಗು_regions = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			*ಹೊರಹೋಗು_ಎಣಿಕೆ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			region_ತಳ_ಲಂಬಸಾಲು = 0;
    ಆದರೆ (!draw_ruler)
    {
	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    ಹೊರಹೋಗು_regions = &wp->w_stl_click;
	    ಹೊರಹೋಗು_ಎಣಿಕೆ = &wp->w_stl_click_ಎಣಿಕೆ;
	    region_ತಳ_ಲಂಬಸಾಲು = wp->w_wincol;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    ಹೊರಹೋಗು_regions = &tabline_stl_click;
	    ಹೊರಹೋಗು_ಎಣಿಕೆ = &tabline_stl_click_ಎಣಿಕೆ;
	    region_ತಳ_ಲಂಬಸಾಲು = firstwin->w_wincol;
	}

	ಗಾಗಿ (n = 0; n < *ಹೊರಹೋಗು_ಎಣಿಕೆ; n++)
	    vim_free((*ಹೊರಹೋಗು_regions)[n].funcname);
	VIM_CLEAR(*ಹೊರಹೋಗು_regions);
	*ಹೊರಹೋಗು_ಎಣಿಕೆ = 0;
    }

    ಪೂರ್ಣಾಂಕ ವಾಹಕ_hl = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < stlh_cnt; i++)
    {
	ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_save;
	buf[0] = NUL;
	ಅಗಲ = build_stl_str_hl_mline(ewp, buf, ಗಾತ್ರ(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&ವಾಹಕ_hl);

	// Make all characters printable.
	p = transstr(buf);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    len = vim_snprintf((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf), "%s", p);
	    vim_free(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(buf);

	// fill up with "fillchar"
	ಆಗ (ಅಗಲ < maxwidth && len < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++ಅಗಲ;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	ಗಾಗಿ (n = 0; hltab[n].ಆರಂಭ != ಖಾಲಿ; n++)
	{
	    len = (ಪೂರ್ಣಾಂಕ)(hltab[n].ಆರಂಭ - p);
	    screen_puts_len(p, len, ಅಡ್ಡಸಾಲು + i, ಲಂಬಸಾಲು, curattr);
	    ಲಂಬಸಾಲು += vim_strnsize(p, len);
	    p = hltab[n].ಆರಂಭ;

	    ಆದರೆ (hltab[n].userhl == 0)
		curattr = attr;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp != ಖಾಲಿ && wp != curwin && bt_terminal(wp->w_ಬಫರ್)
						    && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp != ಖಾಲಿ && bt_terminal(wp->w_ಬಫರ್)
						    && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp != ಖಾಲಿ && wp != curwin && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    ಇಲ್ಲದಿದ್ದರೆ
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, ಅಡ್ಡಸಾಲು + i, ಲಂಬಸಾಲು, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ಆದರೆ (ಹೊರಹೋಗು_regions != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ click_ಎಣಿಕೆ = 0;
	    ಗಾಗಿ (n = 0; clicktab[n].ಆರಂಭ != ಖಾಲಿ; n++)
		click_ಎಣಿಕೆ++;

	    ಆದರೆ (click_ಎಣಿಕೆ > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*ಹೊರಹೋಗು_regions,
			ಗಾತ್ರ(stl_click_region_T)
				    * (*ಹೊರಹೋಗು_ಎಣಿಕೆ + click_ಎಣಿಕೆ));
		ಆದರೆ (new_arr != ಖಾಲಿ)
		{
		    char_u  *cur_funcname = ಖಾಲಿ;
		    ಪೂರ್ಣಾಂಕ	    cur_minwid = 0;
		    ಪೂರ್ಣಾಂಕ	    region_ಆರಂಭ = region_ತಳ_ಲಂಬಸಾಲು;

		    *ಹೊರಹೋಗು_regions = new_arr;

		    len = 0;
		    p = buf;
		    ಗಾಗಿ (n = 0; clicktab[n].ಆರಂಭ != ಖಾಲಿ; n++)
		    {
			len += vim_strnsize(p,
					    (ಪೂರ್ಣಾಂಕ)(clicktab[n].ಆರಂಭ - p));
			p = clicktab[n].ಆರಂಭ;

			ಆದರೆ (cur_funcname != ಖಾಲಿ)
			{
			    stl_click_region_T *r =
					&(*ಹೊರಹೋಗು_regions)[*ಹೊರಹೋಗು_ಎಣಿಕೆ];
			    r->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು + i;
			    r->ಲಂಬಸಾಲು_ಆರಂಭ = region_ಆರಂಭ;
			    r->ಲಂಬಸಾಲು_ಕೊನೆ = region_ತಳ_ಲಂಬಸಾಲು + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*ಹೊರಹೋಗು_ಎಣಿಕೆ)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_ಆರಂಭ = region_ತಳ_ಲಂಬಸಾಲು + len;
		    }

		    // Close final region if it extends to the end.
		    ಆದರೆ (cur_funcname != ಖಾಲಿ)
		    {
			stl_click_region_T *r =
					&(*ಹೊರಹೋಗು_regions)[*ಹೊರಹೋಗು_ಎಣಿಕೆ];
			r->ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು + i;
			r->ಲಂಬಸಾಲು_ಆರಂಭ = region_ಆರಂಭ;
			r->ಲಂಬಸಾಲು_ಕೊನೆ = region_ತಳ_ಲಂಬಸಾಲು + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*ಹೊರಹೋಗು_ಎಣಿಕೆ)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	ಗಾಗಿ (n = 0; clicktab[n].ಆರಂಭ != ಖಾಲಿ; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ಆದರೆ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ಆದರೆ (stl != ಖಾಲಿ)
	vim_free(stl_tmp);

    ಆದರೆ (wp == ಖಾಲಿ)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	ಲಂಬಸಾಲು = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	ಗಾಗಿ (n = 0; tabtab[n].ಆರಂಭ != ಖಾಲಿ; n++)
	{
	    len += vim_strnsize(p, (ಪೂರ್ಣಾಂಕ)(tabtab[n].ಆರಂಭ - p));
	    ಆಗ (ಲಂಬಸಾಲು < len)
		Tab_ಪುಟ_Idxs[ಲಂಬಸಾಲು++] = fillchar;
	    p = tabtab[n].ಆರಂಭ;
	    fillchar = tabtab[n].userhl;
	}
	ಆಗ (ಲಂಬಸಾಲು < firstwin->w_wincol + topframe->fr_ಅಗಲ)
	    Tab_ಪುಟ_Idxs[ಲಂಬಸಾಲು++] = fillchar;
    }

theend:
    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
    entered = FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    ಶೂನ್ಯ
screen_putchar(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ಆದರೆ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    ಶೂನ್ಯ
screen_getbytes(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ *attrp)
{
    ಚಿಹ್ನೆರಹಿತ off;

    // safety check
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ;

    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು;
    ಆದರೆ (attrp != ಖಾಲಿ)
	*attrp = ScreenAttrs[off];
    ಬೈಟುಗಳು[0] = Screen_ಸಾಲುಗಳು[off];
    ಬೈಟುಗಳು[1] = NUL;

    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
	ಬೈಟುಗಳು[utfc_char2bytes(off, ಬೈಟುಗಳು)] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
    {
	ಬೈಟುಗಳು[0] = Screen_ಸಾಲುಗಳು[off];
	ಬೈಟುಗಳು[1] = ScreenLines2[off];
	ಬೈಟುಗಳು[2] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs && MB_BYTE2LEN(ಬೈಟುಗಳು[0]) > 1)
    {
	ಬೈಟುಗಳು[1] = Screen_ಸಾಲುಗಳು[off + 1];
	ಬೈಟುಗಳು[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
screen_comp_differs(ಪೂರ್ಣಾಂಕ off, ಪೂರ್ಣಾಂಕ *u8cc)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
    {
	ಆದರೆ (Screen_ಸಾಲುಗಳು_C[i][off] != (u8char_T)u8cc[i])
	    ಹಿಂತಿರುಗಿ TRUE;
	ಆದರೆ (u8cc[i] == 0)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    ಶೂನ್ಯ
screen_puts(
    char_u	*ಪಠ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		attr)
{
    screen_puts_len(ಪಠ್ಯ, -1, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    ಶೂನ್ಯ
screen_puts_len(
    char_u	*ಪಠ್ಯ,
    ಪೂರ್ಣಾಂಕ		textlen,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		attr_arg)
{
    ಪೂರ್ಣಾಂಕ		attr = attr_arg;
    ಚಿಹ್ನೆರಹಿತ	off;
    char_u	*ptr = ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		len = textlen;
    ಪೂರ್ಣಾಂಕ		c;
    ಚಿಹ್ನೆರಹಿತ	max_off;
    ಪೂರ್ಣಾಂಕ		mbyte_blen = 1;
    ಪೂರ್ಣಾಂಕ		mbyte_cells = 1;
    ಪೂರ್ಣಾಂಕ		u8c = 0;
    ಪೂರ್ಣಾಂಕ		u8cc[MAX_MCO];
    ಪೂರ್ಣಾಂಕ		clear_ಮುಂದಿನ_cell = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    ಪೂರ್ಣಾಂಕ		prev_c = 0;		// previous Arabic character
    ಪೂರ್ಣಾಂಕ		pc, nc, nc1;
    ಪೂರ್ಣಾಂಕ		pcc[MAX_MCO];
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		force_redraw_this;
    ಪೂರ್ಣಾಂಕ		force_redraw_ಮುಂದಿನ = FALSE;
    ಪೂರ್ಣಾಂಕ		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ
	    || ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಅಡ್ಡಸಾಲು < 0
	    || ಲಂಬಸಾಲು >= screen_Columns || ಲಂಬಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ;
    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ಆದರೆ (has_mbyte && ಲಂಬಸಾಲು > 0 && ಲಂಬಸಾಲು < screen_Columns
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
	    && mb_fix_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು) != ಲಂಬಸಾಲು)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ಆದರೆ (!skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1))
	{
	    Screen_ಸಾಲುಗಳು[off - 1] = ' ';
	    ಆದರೆ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off - 1] = 0;
		Screen_ಸಾಲುಗಳು_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_ಮುಂದಿನ = TRUE;
    }

    max_off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + screen_Columns;
    ಆಗ (ಲಂಬಸಾಲು < screen_Columns
	    && (len < 0 || (ಪೂರ್ಣಾಂಕ)(ptr - ಪಠ್ಯ) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ಆದರೆ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (ಪೂರ್ಣಾಂಕ)((ಪಠ್ಯ + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ಆದರೆ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    ಇಲ್ಲದಿದ್ದರೆ	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (ಪೂರ್ಣಾಂಕ)((ಪಠ್ಯ + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
		ಆದರೆ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ಆದರೆ (len >= 0 && (ಪೂರ್ಣಾಂಕ)(ptr - ಪಠ್ಯ) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(ಪೂರ್ಣಾಂಕ)((ಪಠ್ಯ + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    prev_c = u8c;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ಲಂಬಸಾಲು + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_ಮುಂದಿನ;
	force_redraw_ಮುಂದಿನ = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	ಪೂರ್ಣಾಂಕ cell_attr = attr;
	ಆದರೆ (screen_pum_blend > 0 && pum_bg_attrs != ಖಾಲಿ
		&& ಅಡ್ಡಸಾಲು >= pum_bg_ತುದಿ && ಅಡ್ಡಸಾಲು < pum_bg_bot
		&& ಲಂಬಸಾಲು < pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	{
	    ಪೂರ್ಣಾಂಕ soff = (ಅಡ್ಡಸಾಲು - pum_bg_ತುದಿ) * pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು;
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
		|| exmode_ಸಕ್ರಿಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು)
#ಆದರೆ_ಅಂತ್ಯ
		;

	ಆದರೆ ((need_redraw || force_redraw_this) && !skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	{
#ಆದರೆ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ಆದರೆ (need_redraw && Screen_ಸಾಲುಗಳು[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.ಒಳ_use
# endif
# ಆದರೆ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    ಪದ_is_xterm
# endif
		    ))
	    {
		ಪೂರ್ಣಾಂಕ	n = ScreenAttrs[off];

		ಆದರೆ (n > HL_ALL)
		    n = syn_attr2attr(n);
		ಆದರೆ (n & HL_BOLD)
		    force_redraw_ಮುಂದಿನ = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ಆದರೆ (gui.directx_enabled && need_redraw)
		force_redraw_ಮುಂದಿನ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ಆದರೆ (clear_ಮುಂದಿನ_cell)
		clear_ಮುಂದಿನ_cell = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= ಪಠ್ಯ + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_ಮುಂದಿನ_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ಆದರೆ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_ಸಾಲುಗಳು[off + mbyte_blen] = 0;
	    Screen_ಸಾಲುಗಳು[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off] = -1;
	    ಆದರೆ (enc_utf8)
	    {
		ಆದರೆ (c < 0x80 && u8cc[0] == 0)
		    Screen_ಸಾಲುಗಳು_UC[off] = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ	    i;

		    Screen_ಸಾಲುಗಳು_UC[off] = u8c;
		    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][off] = u8cc[i];
			ಆದರೆ (u8cc[i] == 0)
			    ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (mbyte_cells == 2)
		{
		    Screen_ಸಾಲುಗಳು[off + 1] = 0;
		    Screen_ಸಾಲುಗಳು_UC[off + 1] = 0;
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ci = 0; ci < Screen_mco; ++ci)
			Screen_ಸಾಲುಗಳು_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off + 1] = -1;
		}
		screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mbyte_cells == 2)
	    {
		Screen_ಸಾಲುಗಳು[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off + 1] = -1;
		screen_char_2(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	}
	ಆದರೆ (has_mbyte)
	{
	    off += mbyte_cells;
	    ಲಂಬಸಾಲು += mbyte_cells;
	    ptr += mbyte_blen;
	    ಆದರೆ (clear_ಮುಂದಿನ_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++off;
	    ++ಲಂಬಸಾಲು;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ಆದರೆ (force_redraw_ಮುಂದಿನ && ಲಂಬಸಾಲು < screen_Columns && !skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
    {
	ಆದರೆ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ
	    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    }
}

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    ಶೂನ್ಯ
ಆರಂಭ_ಹುಡುಕು_hl(ಶೂನ್ಯ)
{
    ಆದರೆ (!p_hls || no_hlsearch)
	ಹಿಂತಿರುಗಿ;

    ಕೊನೆ_ಹುಡುಕು_hl();  // just in case it wasn't called before
    ಕೊನೆಯ_pat_prog(&screen_ಹುಡುಕು_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    ಶೂನ್ಯ
ಕೊನೆ_ಹುಡುಕು_hl(ಶೂನ್ಯ)
{
    ಆದರೆ (screen_ಹುಡುಕು_hl.rm.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_regfree(screen_ಹುಡುಕು_hl.rm.regprog);
    screen_ಹುಡುಕು_hl.rm.regprog = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
screen_ಆರಂಭ_highlight(ಪೂರ್ಣಾಂಕ attr)
{
    attrentry_T *aep = ಖಾಲಿ;

    screen_attr = attr;
    ಆದರೆ (!ತುಂಬಿದ_screen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    || !termcap_ಸಕ್ರಿಯ
#ಆದರೆ_ಅಂತ್ಯ
       )
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಅಕ್ಷರ	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	ಹೊರಹೋಗು_STR(buf);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (attr > HL_ALL)				// special HL attr.
    {
	ಆದರೆ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ಇಲ್ಲದಿದ್ದರೆ
	    aep = syn_ಪದ_attr2entry(attr);
	ಆದರೆ (aep == ಖಾಲಿ)	    // did ":syntax clear"
	    attr = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    attr = aep->ae_attr;
    }
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	ಪೂರ್ಣಾಂಕ	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ಆದರೆ (p_tgc)
	{
	    ಆದರೆ (aep == ಖಾಲಿ || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		ಪದ_fg_rgb_color(defguifg);
	    ಆದರೆ (aep == ಖಾಲಿ || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		ಪದ_bg_rgb_color(defguibg);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors >= 256)
	{
	    ಆದರೆ (aep == ಖಾಲಿ || aep->ae_u.cterm.fg_color == 0)
		ಪದ_fg_color(defctermfg);
	    ಆದರೆ (aep == ಖಾಲಿ || aep->ae_u.cterm.bg_color == 0)
		ಪದ_bg_color(defctermbg);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	ಹೊರಹೋಗು_str(T_MD);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aep != ಖಾಲಿ && cterm_ಸಾಮಾನ್ಯ_fg_bold && (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#ಆದರೆ_ಅಂತ್ಯ
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	ಹೊರಹೋಗು_str(T_ME);
    ಆದರೆ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	ಹೊರಹೋಗು_str(T_SO);
    ಆದರೆ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	ಹೊರಹೋಗು_str(T_UCS);
    ಆದರೆ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	ಹೊರಹೋಗು_str(T_USS);
    ಆದರೆ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	ಹೊರಹೋಗು_str(T_DS);
    ಆದರೆ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	ಹೊರಹೋಗು_str(T_CDS);
    ಆದರೆ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	ಹೊರಹೋಗು_str(T_US);
    ಆದರೆ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	ಹೊರಹೋಗು_str(T_CZH);
    ಆದರೆ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	ಹೊರಹೋಗು_str(T_MR);
    ಆದರೆ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	ಹೊರಹೋಗು_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ಆದರೆ (aep != ಖಾಲಿ)
    {
	ಆದರೆ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		ಪದ_font(aep->ae_u.cterm.font);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ಆದರೆ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ಆದರೆ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		ಪದ_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (t_colors > 1)
	    {
		ಆದರೆ (aep->ae_u.cterm.fg_color)
		    ಪದ_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	ಆದರೆ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ಆದರೆ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		ಪದ_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (t_colors > 1)
	    {
		ಆದರೆ (aep->ae_u.cterm.bg_color)
		    ಪದ_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	ಆದರೆ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ಆದರೆ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		ಪದ_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (t_colors > 1)
	    {
		ಆದರೆ (aep->ae_u.cterm.ul_color)
		    ಪದ_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ಆದರೆ (!IS_CTERM)
	{
	    ಆದರೆ (aep->ae_u.ಪದ.ಆರಂಭ != ಖಾಲಿ)
		ಹೊರಹೋಗು_str(aep->ae_u.ಪದ.ಆರಂಭ);
	}
    }
}

    ಶೂನ್ಯ
screen_stop_highlight(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    do_ME = FALSE;	    // output T_ME code
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಂಕ	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (screen_attr != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
			&& termcap_ಸಕ್ರಿಯ
#ಆದರೆ_ಅಂತ್ಯ
					   )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    ಅಕ್ಷರ	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    ಹೊರಹೋಗು_STR(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಪೂರ್ಣಾಂಕ is_under;

	    ಆದರೆ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ಆದರೆ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ಆದರೆ (aep != ಖಾಲಿ && ((
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#ಆದರೆ_ಅಂತ್ಯ
				aep->ae_u.cterm.fg_color) || (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#ಆದರೆ_ಅಂತ್ಯ
				aep->ae_u.cterm.bg_color)))
			do_ME = TRUE;
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ಆದರೆ (use_vtp())
		    {
			ಆದರೆ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ಆದರೆ (!do_ME && do_ME_fg)
			    ಹೊರಹೋಗು_str((char_u *)"\033|39m"); // restore FG
			ಆದರೆ (!do_ME && do_ME_bg)
			    ಹೊರಹೋಗು_str((char_u *)"\033|49m"); // restore BG
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Process FG and BG at once.
			ಆದರೆ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    aep = syn_ಪದ_attr2entry(screen_attr);
		    ಆದರೆ (aep != ಖಾಲಿ && aep->ae_u.ಪದ.stop != ಖಾಲಿ)
		    {
			ಆದರೆ (STRCMP(aep->ae_u.ಪದ.stop, T_ME) == 0)
			    do_ME = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			    ಹೊರಹೋಗು_str(aep->ae_u.ಪದ.stop);
		    }
		}
		ಆದರೆ (aep == ಖಾಲಿ)	    // did ":syntax clear"
		    screen_attr = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ಆದರೆ (screen_attr & HL_STANDOUT)
	    {
		ಆದರೆ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊರಹೋಗು_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ಆದರೆ (is_under && *T_UCE != NUL)
	    {
		ಆದರೆ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊರಹೋಗು_str(T_UCE);
	    }
	    ಆದರೆ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ಆದರೆ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊರಹೋಗು_str(T_UE);
	    }
	    ಆದರೆ (screen_attr & HL_ITALIC)
	    {
		ಆದರೆ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊರಹೋಗು_str(T_CZR);
	    }
	    ಆದರೆ (screen_attr & HL_STRIKETHROUGH)
	    {
		ಆದರೆ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹೊರಹೋಗು_str(T_STE);
	    }
	    ಆದರೆ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		ಹೊರಹೋಗು_str(T_ME);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    ಆದರೆ (p_tgc)
	    {
		ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_gui_color != INVALCOLOR)
		    ಪದ_fg_rgb_color(cterm_ಸಾಮಾನ್ಯ_fg_gui_color);
		ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_gui_color != INVALCOLOR)
		    ಪದ_bg_rgb_color(cterm_ಸಾಮಾನ್ಯ_bg_gui_color);
		ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_ul_gui_color != INVALCOLOR)
		    ಪದ_ul_rgb_color(cterm_ಸಾಮಾನ್ಯ_ul_gui_color);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಆದರೆ (t_colors > 1)
		{
		    // set Normal cterm colors
		    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color != 0)
			ಪದ_fg_color(cterm_ಸಾಮಾನ್ಯ_fg_color - 1);
		    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_color != 0)
			ಪದ_bg_color(cterm_ಸಾಮಾನ್ಯ_bg_color - 1);
		    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_ul_color != 0)
			ಪದ_ul_color(cterm_ಸಾಮಾನ್ಯ_ul_color - 1);
		    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_bold)
			ಹೊರಹೋಗು_str(T_MD);
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
    ಆದರೆ (!IS_CTERM)
	ಹಿಂತಿರುಗಿ;

    // set Normal cterm colors
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
    ಆದರೆ (p_tgc ? (cterm_ಸಾಮಾನ್ಯ_fg_gui_color != INVALCOLOR
		|| cterm_ಸಾಮಾನ್ಯ_bg_gui_color != INVALCOLOR)
	    : (cterm_ಸಾಮಾನ್ಯ_fg_color > 0 || cterm_ಸಾಮಾನ್ಯ_bg_color > 0))
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color > 0 || cterm_ಸಾಮಾನ್ಯ_bg_color > 0)
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಹೊರಹೋಗು_str(T_OP);
	    screen_attr = -1;
	}
    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_bold)
    {
	ಹೊರಹೋಗು_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    ಶೂನ್ಯ
screen_char(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    mark_dirty_region_for_images(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, 1, 1);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ಆದರೆ (popup_is_under_opacity(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& ಲಂಬಸಾಲು + 1 < screen_Columns
		&& !popup_is_under_opacity(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1))
	{
	    ಪೂರ್ಣಾಂಕ off2 = off + 1;
	    Screen_ಸಾಲುಗಳು[off2] = ' ';
	    Screen_ಸಾಲುಗಳು_UC[off2] = 0;
	    screen_char(off2, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
	}
	mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
		&& utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
		&& ಲಂಬಸಾಲು + 1 < screen_Columns
		&& popup_is_under_opacity(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1))
	    mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
	screen_cur_ಲಂಬಸಾಲು = 9999;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
	    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2
	    && ಲಂಬಸಾಲು + 1 < screen_Columns
	    && popup_is_under_opacity(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1))
    {
	mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
	screen_cur_ಲಂಬಸಾಲು = 9999;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ಆದರೆ (*T_XN == NUL
	    && ಅಡ್ಡಸಾಲು == screen_ಅಡ್ಡಸಾಲುಗಳು - 1 && ಲಂಬಸಾಲು == screen_Columns - 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off] = -1;
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ಆದರೆ (screen_char_attr != 0)
	attr = screen_char_attr;
    ಇಲ್ಲದಿದ್ದರೆ
	attr = ScreenAttrs[off];
    ಆದರೆ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
	    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) == 2)
	unmark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (screen_attr != attr)
	screen_ಆರಂಭ_highlight(attr);

    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    !gui.ಒಳ_use &&
#ಆದರೆ_ಅಂತ್ಯ
	    get_cellwidth(Screen_ಸಾಲುಗಳು_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    ಹೊರಹೋಗು_str((char_u *)"  ");
	    ಪದ_windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    screen_cur_ಲಂಬಸಾಲು = 9999;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (utf_ambiguous_ಅಗಲ(Screen_ಸಾಲುಗಳು_UC[off]))
	{
	    ಆದರೆ (*p_ambw == 'd'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		ಹೊರಹೋಗು_str((char_u *)"  ");
		ಪದ_windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_ಲಂಬಸಾಲು = 9999;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]) > 1)
	    ++screen_cur_ಲಂಬಸಾಲು;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	ಹೊರಹೋಗು_str(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಹೊರಹೋಗು_flush_check();
	ಹೊರಹೋಗು_char(Screen_ಸಾಲುಗಳು[off]);
	// double-byte character in single-width cell
	ಆದರೆ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
	    ಹೊರಹೋಗು_char(ScreenLines2[off]);
    }

    screen_cur_ಲಂಬಸಾಲು++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
screen_char_2(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    // Check for illegal values (could be wrong when screen was resized).
    ಆದರೆ (off + 1 >= (ಚಿಹ್ನೆರಹಿತ)(screen_ಅಡ್ಡಸಾಲುಗಳು * screen_Columns))
	ಹಿಂತಿರುಗಿ;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ಆದರೆ (ಅಡ್ಡಸಾಲು == screen_ಅಡ್ಡಸಾಲುಗಳು - 1 && ಲಂಬಸಾಲು >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off] = -1;
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ಆದರೆ (popup_is_under_opacity(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
    {
	mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	mark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
	screen_cur_ಲಂಬಸಾಲು = 9999;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    ಹೊರಹೋಗು_char(Screen_ಸಾಲುಗಳು[off + 1]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    unmark_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + 1);
#ಆದರೆ_ಅಂತ್ಯ
    ++screen_cur_ಲಂಬಸಾಲು;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    ಶೂನ್ಯ
screen_draw_rectangle(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ,
    ಪೂರ್ಣಾಂಕ		ಅಗಲ,
    ಪೂರ್ಣಾಂಕ		invert,
    ಪೂರ್ಣಾಂಕ		force)
{
    ಪೂರ್ಣಾಂಕ		r, c;
    ಪೂರ್ಣಾಂಕ		off;
    ಪೂರ್ಣಾಂಕ		max_off;

    // Can't use ScreenLines unless initialized
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (invert)
	screen_char_attr = HL_INVERSE;
    ಗಾಗಿ (r = ಅಡ್ಡಸಾಲು; r < ಅಡ್ಡಸಾಲು + ಎತ್ತರ; ++r)
    {
	off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[r];
	max_off = off + screen_Columns;
	ಗಾಗಿ (c = ಲಂಬಸಾಲು; c < ಲಂಬಸಾಲು + ಅಗಲ; ++c)
	{
	    ಆದರೆ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ಆದರೆ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ಆದರೆ (utf_off2cells(off + c, max_off) > 1)
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
redraw_ಖಂಡ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಕೊನೆ, win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    clip_may_clear_selection(ಅಡ್ಡಸಾಲು, ಕೊನೆ - 1);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp == ಖಾಲಿ)
    {
	ಲಂಬಸಾಲು = firstwin->w_wincol;
	ಅಗಲ = topframe->fr_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಲಂಬಸಾಲು = wp->w_wincol;
	ಅಗಲ = wp->w_ಅಗಲ;
    }
    screen_draw_rectangle(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಕೊನೆ - ಅಡ್ಡಸಾಲು, ಅಗಲ, FALSE, FALSE);
}

    ಶೂನ್ಯ
space_ಗಮ್ಯ_screenline(ಪೂರ್ಣಾಂಕ off, ಪೂರ್ಣಾಂಕ attr)
{
    Screen_ಸಾಲುಗಳು[off] = ' ';
    ScreenAttrs[off] = attr;
    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off] = -1;
    ಆದರೆ (enc_utf8)
	Screen_ಸಾಲುಗಳು_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    ಶೂನ್ಯ
screen_fill(
	ಪೂರ್ಣಾಂಕ	ಆರಂಭ_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಕೊನೆ_ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಆರಂಭ_ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಕೊನೆ_ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	c1,
	ಪೂರ್ಣಾಂಕ	c2,
	ಪೂರ್ಣಾಂಕ	attr)
{
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    off;
    ಪೂರ್ಣಾಂಕ	    ಕೊನೆ_off;
    ಪೂರ್ಣಾಂಕ	    did_delete;
    ಪೂರ್ಣಾಂಕ	    c;
    ಪೂರ್ಣಾಂಕ	    norm_ಪದ;
#ಆದರೆ defined(FEAT_GUI) || defined(UNIX)
    ಪೂರ್ಣಾಂಕ	    force_ಮುಂದಿನ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಕೊನೆ_ಅಡ್ಡಸಾಲು > screen_ಅಡ್ಡಸಾಲುಗಳು)		// safety check
	ಕೊನೆ_ಅಡ್ಡಸಾಲು = screen_ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು > screen_Columns)	// safety check
	ಕೊನೆ_ಲಂಬಸಾಲು = screen_Columns;
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ
	    || ಆರಂಭ_ಅಡ್ಡಸಾಲು < 0		// should not happen
	    || ಆರಂಭ_ಅಡ್ಡಸಾಲು >= ಕೊನೆ_ಅಡ್ಡಸಾಲು
	    || ಆರಂಭ_ಲಂಬಸಾಲು >= ಕೊನೆ_ಲಂಬಸಾಲು)	// nothing to do
	ಹಿಂತಿರುಗಿ;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_ಪದ = (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    !gui.ಒಳ_use &&
#ಆದರೆ_ಅಂತ್ಯ
	    !IS_CTERM);
    ಗಾಗಿ (ಅಡ್ಡಸಾಲು = ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < ಕೊನೆ_ಅಡ್ಡಸಾಲು; ++ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (has_mbyte
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು > 0 && mb_fix_ಲಂಬಸಾಲು(ಆರಂಭ_ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು) != ಆರಂಭ_ಲಂಬಸಾಲು)
	    {
		// start_col is the right half, clear the left half
		ಪೂರ್ಣಾಂಕ ಎಡ_attr = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಆರಂಭ_ಲಂಬಸಾಲು - 1];
		screen_puts_len((char_u *)" ", 1, ಅಡ್ಡಸಾಲು, ಆರಂಭ_ಲಂಬಸಾಲು - 1, ಎಡ_attr);
	    }
	    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು < screen_Columns && mb_fix_ಲಂಬಸಾಲು(ಕೊನೆ_ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು) != ಕೊನೆ_ಲಂಬಸಾಲು)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		ಪೂರ್ಣಾಂಕ ಬಲ_attr = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಕೊನೆ_ಲಂಬಸಾಲು];
		screen_puts_len((char_u *)" ", 1, ಅಡ್ಡಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು, ಬಲ_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ಆದರೆ (c2 == ' '
		&& ಕೊನೆ_ಲಂಬಸಾಲು == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_ಪದ
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(ಅಡ್ಡಸಾಲು,
						ಆರಂಭ_ಲಂಬಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು)
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು;
	    ಆದರೆ (c1 != ' ')			// don't clear first char
		++ಲಂಬಸಾಲು;

	    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಲಂಬಸಾಲು;
	    ಕೊನೆ_off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಕೊನೆ_ಲಂಬಸಾಲು;

	    // skip blanks (used often, keep it fast!)
	    ಆದರೆ (enc_utf8)
		ಆಗ (off < ಕೊನೆ_off && Screen_ಸಾಲುಗಳು[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_ಸಾಲುಗಳು_UC[off] == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			  && !is_suppressed_cell(ಅಡ್ಡಸಾಲು,
					      (ಪೂರ್ಣಾಂಕ)(off - ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು]))
#ಆದರೆ_ಅಂತ್ಯ
			  )
		    ++off;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆಗ (off < ಕೊನೆ_off && Screen_ಸಾಲುಗಳು[off] == ' '
						     && ScreenAttrs[off] == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			  && !is_suppressed_cell(ಅಡ್ಡಸಾಲು,
					      (ಪೂರ್ಣಾಂಕ)(off - ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು]))
#ಆದರೆ_ಅಂತ್ಯ
			  )
		    ++off;
	    ಆದರೆ (off < ಕೊನೆ_off)		// something to be cleared
	    {
		ಲಂಬಸಾಲು = off - ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು];
		screen_stop_highlight();
		ಪದ_windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);// clear rest of this screen line
		ಹೊರಹೋಗು_str(T_CE);
		screen_ಆರಂಭ();		// don't know where cursor is now
		ಲಂಬಸಾಲು = ಕೊನೆ_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು;
		ಆಗ (ಲಂಬಸಾಲು--)		// clear chars in ScreenLines
		{
		    space_ಗಮ್ಯ_screenline(off, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(ಅಡ್ಡಸಾಲು, (ಪೂರ್ಣಾಂಕ)(off - ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು]));
#ಆದರೆ_ಅಂತ್ಯ
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಆರಂಭ_ಲಂಬಸಾಲು;
	c = c1;
	ಗಾಗಿ (ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು < ಕೊನೆ_ಲಂಬಸಾಲು; ++ಲಂಬಸಾಲು)
	{
	    ಆದರೆ ((Screen_ಸಾಲುಗಳು[off] != c
		    || (enc_utf8 && (ಪೂರ್ಣಾಂಕ)Screen_ಸಾಲುಗಳು_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ಆದರೆ defined(FEAT_GUI) || defined(UNIX)
		    || force_ಮುಂದಿನ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು)
#ಆದರೆ_ಅಂತ್ಯ
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ಆದರೆ (Screen_ಸಾಲುಗಳು[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off - 1] != 0
			    && utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off - 1]) == 2)
			ಹೋಗು ಮುಂದಿನ_ಲಂಬಸಾಲು;
		}
		ಆದರೆ (screen_opacity_popup != ಖಾಲಿ && c == ' '
						       && Screen_ಸಾಲುಗಳು[off] != 0)
		{
		    ಪೂರ್ಣಾಂಕ bg_char_cells = 1;
		    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_ಸಾಲುಗಳು_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ಆದರೆ (bg_char_cells == 2)
		    {
			ಆದರೆ (ಲಂಬಸಾಲು + 1 >= ಕೊನೆ_ಲಂಬಸಾಲು)
			    // At the edge, skip wide char.
			    ಹೋಗು skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ಆದರೆ (c2 != ' ')
			    ಹೋಗು skip_opacity_fill;
		    }

		    ಪೂರ್ಣಾಂಕ popup_attr = get_win_attr(screen_opacity_popup);
		    ಪೂರ್ಣಾಂಕ blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
		    ಹೋಗು ಮುಂದಿನ_ಲಂಬಸಾಲು;
		}
skip_opacity_fill:
#ಆದರೆ_ಅಂತ್ಯ
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		ಆದರೆ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != ಖಾಲಿ
			&& ಅಡ್ಡಸಾಲು >= pum_bg_ತುದಿ && ಅಡ್ಡಸಾಲು < pum_bg_bot
			&& ಲಂಬಸಾಲು < pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
		{
		    ಪೂರ್ಣಾಂಕ soff = (ಅಡ್ಡಸಾಲು - pum_bg_ತುದಿ) * pum_bg_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಲಂಬಸಾಲು;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ
			    && ಲಂಬಸಾಲು > ಆರಂಭ_ಲಂಬಸಾಲು
			    && pum_bg_ಸಾಲುಗಳು_UC[soff] == 0
			    && pum_bg_ಸಾಲುಗಳು_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff - 1]) == 2)
		    {
			Screen_ಸಾಲುಗಳು[off] = 0;
			ಆದರೆ (Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			ಹೋಗು ಮುಂದಿನ_ಲಂಬಸಾಲು;
		    }

		    ಪೂರ್ಣಾಂಕ underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ
			    && ((pum_bg_ಸಾಲುಗಳು_UC[soff] != 0
				    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff]) == 2
				    && ಲಂಬಸಾಲು + 1 >= ಕೊನೆ_ಲಂಬಸಾಲು)
				|| (ಲಂಬಸಾಲು == ಆರಂಭ_ಲಂಬಸಾಲು
				    && pum_bg_ಸಾಲುಗಳು_UC[soff] == 0
				    && ಲಂಬಸಾಲು > 0
				    && pum_bg_ಸಾಲುಗಳು_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_ಸಾಲುಗಳು_UC[soff - 1])
								       == 2)))
		    {
			Screen_ಸಾಲುಗಳು[off] = ' ';
			ಆದರೆ (Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
			    Screen_ಸಾಲುಗಳು_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
			ಹೋಗು ಮುಂದಿನ_ಲಂಬಸಾಲು;
		    }

		    // Restore underlying character so text shows through.
		    Screen_ಸಾಲುಗಳು[off] = pum_bg_ಸಾಲುಗಳು[soff];
		    ಆದರೆ (enc_utf8 && pum_bg_ಸಾಲುಗಳು_UC != ಖಾಲಿ
						&& Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
		    {
			ಪೂರ್ಣಾಂಕ k;
			Screen_ಸಾಲುಗಳು_UC[off] = pum_bg_ಸಾಲುಗಳು_UC[soff];
			ಗಾಗಿ (k = 0; k < MAX_MCO; ++k)
			    ಆದರೆ (pum_bg_ಸಾಲುಗಳು_C[k] != ಖಾಲಿ
						    && Screen_ಸಾಲುಗಳು_C[k] != ಖಾಲಿ)
				Screen_ಸಾಲುಗಳು_C[k][off] = pum_bg_ಸಾಲುಗಳು_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
		    ಹೋಗು ಮುಂದಿನ_ಲಂಬಸಾಲು;
		}
#ಆದರೆ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ಆದರೆ (
# ifdef FEAT_GUI
			gui.ಒಳ_use
# endif
# ಆದರೆ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			ಪದ_is_xterm
# endif
		   )
		{
		    ಆದರೆ (Screen_ಸಾಲುಗಳು[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_ಮುಂದಿನ = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
			force_ಮುಂದಿನ = FALSE;
		}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI || defined(UNIX)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ಆದರೆ (gui.directx_enabled)
		    force_ಮುಂದಿನ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		Screen_ಸಾಲುಗಳು[off] = c;
		ಆದರೆ (enc_utf8)
		{
		    ಆದರೆ (c >= 0x80)
		    {
			Screen_ಸಾಲುಗಳು_UC[off] = c;
			Screen_ಸಾಲುಗಳು_C[0][off] = 0;
			Screen_ಸಾಲುಗಳು[off] = 0x80; // avoid storing zero
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			Screen_ಸಾಲುಗಳು_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ಆದರೆ (!did_delete || c != ' ')
		    screen_char(off, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
	    }
ಮುಂದಿನ_ಲಂಬಸಾಲು:
	    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off] = -1;
	    ++off;
	    ಆದರೆ (ಲಂಬಸಾಲು == ಆರಂಭ_ಲಂಬಸಾಲು)
	    {
		ಆದರೆ (did_delete)
		    ನಿಲ್ಲಿಸು;
		c = c2;
	    }
	}
	ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು == Columns)
	    ಸಾಲು_Wraps[ಅಡ್ಡಸಾಲು] = FALSE;
	ಆದರೆ (ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ಆದರೆ (((ಆರಂಭ_ಲಂಬಸಾಲು == 0 && ಕೊನೆ_ಲಂಬಸಾಲು == Columns)
			|| (ಆರಂಭ_ಲಂಬಸಾಲು == cmdline_ಲಂಬಸಾಲು_off
			&& ಕೊನೆ_ಲಂಬಸಾಲು == cmdline_ಲಂಬಸಾಲು_off + cmdline_ಅಗಲ))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#ಆದರೆ_ಅಂತ್ಯ
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು == 0 || ಆರಂಭ_ಲಂಬಸಾಲು == cmdline_ಲಂಬಸಾಲು_off)
		ಬಹುಲಕ_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    ಶೂನ್ಯ
check_for_delay(ಪೂರ್ಣಾಂಕ check_ಸಂದೇಶ_scroll)
{
    ಆದರೆ ((emsg_on_display || (check_ಸಂದೇಶ_scroll && ಸಂದೇಶ_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !ಒಳ_assert_fails)
    {
	ಹೊರಹೋಗು_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ಆದರೆ (check_ಸಂದೇಶ_scroll)
	    ಸಂದೇಶ_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_Tab_ಪುಟ_Idxs(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		scol;

    ಗಾಗಿ (scol = 0; scol < Columns; ++scol)
	Tab_ಪುಟ_Idxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    ಪೂರ್ಣಾಂಕ
screen_valid(ಪೂರ್ಣಾಂಕ doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    ಹಿಂತಿರುಗಿ (Screen_ಸಾಲುಗಳು != ಖಾಲಿ);
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
screenalloc(ಪೂರ್ಣಾಂಕ doclear)
{
    ಪೂರ್ಣಾಂಕ		    new_ಅಡ್ಡಸಾಲು, old_ಅಡ್ಡಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		    old_ಅಡ್ಡಸಾಲುಗಳು_2;
#ಆದರೆ_ಅಂತ್ಯ
    win_T	    *wp;
    ಪೂರ್ಣಾಂಕ		    outofmem = FALSE;
    ಪೂರ್ಣಾಂಕ		    len;
    schar_T	    *new_Screen_ಸಾಲುಗಳು;
    u8char_T	    *new_Screen_ಸಾಲುಗಳು_UC = ಖಾಲಿ;
    u8char_T	    *new_Screen_ಸಾಲುಗಳು_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = ಖಾಲಿ;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಚಿಹ್ನೆರಹಿತ	    *new_ಸಾಲು_ಸ್ಥಾನಾಂತರ;
    char_u	    *new_ಸಾಲು_Wraps;
    ಗಿಡ್ಡ	    *new_Tab_ಪುಟ_Idxs;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಗಿಡ್ಡ	    *new_popup_ಮುಖವಾಡ;
    ಗಿಡ್ಡ	    *new_popup_ಮುಖವಾಡ_ಮುಂದಿನ;
    ಅಕ್ಷರ	    *new_popup_transparent;
#ಆದರೆ_ಅಂತ್ಯ
    tabpage_T	    *tp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    entered = FALSE;		// avoid recursiveness
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಮುಗಿದ_outofmem_ಸಂದೇಶ = FALSE;	// did outofmem message
    ಪೂರ್ಣಾಂಕ		    retry_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		    ಸಿಕ್ಕಿತು_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ಆದರೆ ((Screen_ಸಾಲುಗಳು != ಖಾಲಿ
		&& ಅಡ್ಡಸಾಲುಗಳು == screen_ಅಡ್ಡಸಾಲುಗಳು
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != ಖಾಲಿ)
		&& p_mco == Screen_mco)
	    || ಅಡ್ಡಸಾಲುಗಳು == 0
	    || Columns == 0
	    || (!ತುಂಬಿದ_screen && Screen_ಸಾಲುಗಳು == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_ಬೀಗ_screen();  // be safe, put it here
#ಆದರೆ_ಅಂತ್ಯ

    comp_ಲಂಬಸಾಲು();		// recompute columns for shown command and ruler

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
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಆದರೆ (aucmd_win[i].auc_win != ಖಾಲಿ)
	    win_free_lsize(aucmd_win[i].auc_win);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	    win_free_lsize(wp);
#ಆದರೆ_ಅಂತ್ಯ

    new_Screen_ಸಾಲುಗಳು = LALLOC_MULT(schar_T, (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
    vim_memset(new_Screen_ಸಾಲುಗಳು_C, 0, ಗಾತ್ರ(u8char_T *) * MAX_MCO);
    ಆದರೆ (enc_utf8)
    {
	new_Screen_ಸಾಲುಗಳು_UC = LALLOC_MULT(u8char_T, (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
	    new_Screen_ಸಾಲುಗಳು_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
    }
    ಆದರೆ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = LALLOC_CLEAR_MULT(colnr_T, (ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns);
    new_ಸಾಲು_ಸ್ಥಾನಾಂತರ = LALLOC_MULT(ಚಿಹ್ನೆರಹಿತ, ಅಡ್ಡಸಾಲುಗಳು);
    new_ಸಾಲು_Wraps = LALLOC_MULT(char_u, ಅಡ್ಡಸಾಲುಗಳು);
    new_Tab_ಪುಟ_Idxs = LALLOC_MULT(ಗಿಡ್ಡ, Columns);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    new_popup_ಮುಖವಾಡ = LALLOC_MULT(ಗಿಡ್ಡ, ಅಡ್ಡಸಾಲುಗಳು * Columns);
    new_popup_ಮುಖವಾಡ_ಮುಂದಿನ = LALLOC_MULT(ಗಿಡ್ಡ, ಅಡ್ಡಸಾಲುಗಳು * Columns);
    new_popup_transparent = LALLOC_MULT(ಅಕ್ಷರ, ಅಡ್ಡಸಾಲುಗಳು * Columns);
#ಆದರೆ_ಅಂತ್ಯ

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಆದರೆ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ಹೋಗು give_ಮೇಲೆ;
	}
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಆದರೆ (aucmd_win[i].auc_win != ಖಾಲಿ
		&& aucmd_win[i].auc_win->w_ಸಾಲುಗಳು == ಖಾಲಿ
		&& win_alloc_ಸಾಲುಗಳು(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	{
	    outofmem = TRUE;
	    ಹೋಗು give_ಮೇಲೆ;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	    ಆದರೆ (win_alloc_ಸಾಲುಗಳು(wp) == FAIL)
	    {
		outofmem = TRUE;
		ಹೋಗು give_ಮೇಲೆ;
	    }
#ಆದರೆ_ಅಂತ್ಯ

give_ಮೇಲೆ:
    ಸಿಕ್ಕಿತು_null = FALSE;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
	ಆದರೆ (new_Screen_ಸಾಲುಗಳು_C[i] == ಖಾಲಿ)
	{
	    ಸಿಕ್ಕಿತು_null = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    ಆದರೆ (new_Screen_ಸಾಲುಗಳು == ಖಾಲಿ
	    || (enc_utf8 && (new_Screen_ಸಾಲುಗಳು_UC == ಖಾಲಿ || ಸಿಕ್ಕಿತು_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == ಖಾಲಿ)
	    || new_ScreenAttrs == ಖಾಲಿ
	    || new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == ಖಾಲಿ
	    || new_ಸಾಲು_ಸ್ಥಾನಾಂತರ == ಖಾಲಿ
	    || new_ಸಾಲು_Wraps == ಖಾಲಿ
	    || new_Tab_ಪುಟ_Idxs == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    || new_popup_ಮುಖವಾಡ == ಖಾಲಿ
	    || new_popup_ಮುಖವಾಡ_ಮುಂದಿನ == ಖಾಲಿ
	    || new_popup_transparent == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    || outofmem)
    {
	ಆದರೆ (Screen_ಸಾಲುಗಳು != ಖಾಲಿ || !ಮುಗಿದ_outofmem_ಸಂದೇಶ)
	{
	    // guess the size
	    do_outofmem_ಸಂದೇಶ((long_u)((ಅಡ್ಡಸಾಲುಗಳು + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    ಮುಗಿದ_outofmem_ಸಂದೇಶ = TRUE;
	}
	VIM_CLEAR(new_Screen_ಸಾಲುಗಳು);
	VIM_CLEAR(new_Screen_ಸಾಲುಗಳು_UC);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_ಸಾಲುಗಳು_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	VIM_CLEAR(new_ಸಾಲು_ಸ್ಥಾನಾಂತರ);
	VIM_CLEAR(new_ಸಾಲು_Wraps);
	VIM_CLEAR(new_Tab_ಪುಟ_Idxs);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_ಮುಖವಾಡ);
	VIM_CLEAR(new_popup_ಮುಖವಾಡ_ಮುಂದಿನ);
	VIM_CLEAR(new_popup_transparent);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಮುಗಿದ_outofmem_ಸಂದೇಶ = FALSE;

	ಗಾಗಿ (new_ಅಡ್ಡಸಾಲು = 0; new_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು; ++new_ಅಡ್ಡಸಾಲು)
	{
	    new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು] = new_ಅಡ್ಡಸಾಲು * Columns;
	    new_ಸಾಲು_Wraps[new_ಅಡ್ಡಸಾಲು] = FALSE;

	    (ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು + new_ಅಡ್ಡಸಾಲು * Columns,
				  ' ', (size_t)Columns * ಗಾತ್ರ(schar_T));
	    ಆದರೆ (enc_utf8)
	    {
		(ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು_UC + new_ಅಡ್ಡಸಾಲು * Columns,
				   0, (size_t)Columns * ಗಾತ್ರ(u8char_T));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
		    (ಶೂನ್ಯ)vim_memset(new_Screen_ಸಾಲುಗಳು_C[i]
						      + new_ಅಡ್ಡಸಾಲು * Columns,
				   0, (size_t)Columns * ಗಾತ್ರ(u8char_T));
	    }
	    ಆದರೆ (enc_dbcs == DBCS_JPNU)
		(ಶೂನ್ಯ)vim_memset(new_ScreenLines2 + new_ಅಡ್ಡಸಾಲು * Columns,
				   0, (size_t)Columns * ಗಾತ್ರ(schar_T));
	    (ಶೂನ್ಯ)vim_memset(new_ScreenAttrs + new_ಅಡ್ಡಸಾಲು * Columns,
				    0, (size_t)Columns * ಗಾತ್ರ(sattr_T));
	    (ಶೂನ್ಯ)vim_memset(new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + new_ಅಡ್ಡಸಾಲು * Columns,
				    0, (size_t)Columns * ಗಾತ್ರ(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ಆದರೆ (!doclear)
	    {
		old_ಅಡ್ಡಸಾಲು = new_ಅಡ್ಡಸಾಲು + (screen_ಅಡ್ಡಸಾಲುಗಳು - ಅಡ್ಡಸಾಲುಗಳು);
		ಆದರೆ (old_ಅಡ್ಡಸಾಲು >= 0 && Screen_ಸಾಲುಗಳು != ಖಾಲಿ)
		{
		    ಆದರೆ (screen_Columns < Columns)
			len = screen_Columns;
		    ಇಲ್ಲದಿದ್ದರೆ
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ಆದರೆ (!(enc_utf8 && Screen_ಸಾಲುಗಳು_UC == ಖಾಲಿ)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_ಸಾಲುಗಳು + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
				Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
				(size_t)len * ಗಾತ್ರ(schar_T));
		    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC != ಖಾಲಿ
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_ಸಾಲುಗಳು_UC + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
				Screen_ಸಾಲುಗಳು_UC + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
				(size_t)len * ಗಾತ್ರ(u8char_T));
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_ಸಾಲುಗಳು_C[i]
						    + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
				Screen_ಸಾಲುಗಳು_C[i] + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
				(size_t)len * ಗಾತ್ರ(u8char_T));
		    }
		    ಆದರೆ (enc_dbcs == DBCS_JPNU && ScreenLines2 != ಖಾಲಿ)
			mch_memmove(new_ScreenLines2 + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
				ScreenLines2 + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
				(size_t)len * ಗಾತ್ರ(schar_T));
		    mch_memmove(new_ScreenAttrs + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
			    ScreenAttrs + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
			    (size_t)len * ಗಾತ್ರ(sattr_T));
		    mch_memmove(new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + new_ಸಾಲು_ಸ್ಥಾನಾಂತರ[new_ಅಡ್ಡಸಾಲು],
			    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + ಸಾಲು_ಸ್ಥಾನಾಂತರ[old_ಅಡ್ಡಸಾಲು],
			    (size_t)len * ಗಾತ್ರ(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_ಸಾಲು = new_Screen_ಸಾಲುಗಳು + ಅಡ್ಡಸಾಲುಗಳು * Columns;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	vim_memset(new_popup_ಮುಖವಾಡ, 0, ಅಡ್ಡಸಾಲುಗಳು * Columns * ಗಾತ್ರ(ಗಿಡ್ಡ));
	vim_memset(new_popup_transparent, 0, ಅಡ್ಡಸಾಲುಗಳು * Columns * ಗಾತ್ರ(ಅಕ್ಷರ));
#ಆದರೆ_ಅಂತ್ಯ
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_ಸಾಲುಗಳು = new_Screen_ಸಾಲುಗಳು;
    Screen_ಸಾಲುಗಳು_UC = new_Screen_ಸಾಲುಗಳು_UC;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < p_mco; ++i)
	Screen_ಸಾಲುಗಳು_C[i] = new_Screen_ಸಾಲುಗಳು_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = new_Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಸಾಲು_ಸ್ಥಾನಾಂತರ = new_ಸಾಲು_ಸ್ಥಾನಾಂತರ;
    ಸಾಲು_Wraps = new_ಸಾಲು_Wraps;
    Tab_ಪುಟ_Idxs = new_Tab_ಪುಟ_Idxs;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    popup_ಮುಖವಾಡ = new_popup_ಮುಖವಾಡ;
    popup_ಮುಖವಾಡ_ಮುಂದಿನ = new_popup_ಮುಖವಾಡ_ಮುಂದಿನ;
    popup_transparent = new_popup_transparent;
    popup_ಮುಖವಾಡ_refresh = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    old_ಅಡ್ಡಸಾಲುಗಳು_2 = screen_ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    screen_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
    screen_Columns = Columns;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Cursor position may now be out of bounds after resize
    ಆದರೆ (gui.ಒಳ_use && (gui.cursor_ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು
		|| gui.cursor_ಲಂಬಸಾಲು >= screen_Columns))
	gui.cursor_is_valid = false;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ಆದರೆ (doclear)
	screenclear2(TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.ಒಳ_use
	    && !gui.starting
	    && Screen_ಸಾಲುಗಳು != ಖಾಲಿ
	    && old_ಅಡ್ಡಸಾಲುಗಳು_2 != ಅಡ್ಡಸಾಲುಗಳು)
    {
	gui_redraw_ಖಂಡ(0, 0, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)		// Rows got smaller
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;		// put cursor at last row
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು > old_ಅಡ್ಡಸಾಲುಗಳು_2)	// Rows got bigger
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು += ಅಡ್ಡಸಾಲುಗಳು - old_ಅಡ್ಡಸಾಲುಗಳು_2; // put cursor in same place
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= cmdline_ಅಗಲ)	// cmdline_width got smaller
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;    // put cursor at last cmdline column
    }
#ಆದರೆ_ಅಂತ್ಯ
    clear_Tab_ಪುಟ_Idxs();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ಆದರೆ_ಅಂತ್ಯ

    entered = FALSE;
    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ಆದರೆ (starting == 0 && ++retry_ಎಣಿಕೆ <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	ಹೋಗು retry;
    }
}

    ಶೂನ್ಯ
free_screenlines(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    VIM_CLEAR(Screen_ಸಾಲುಗಳು_UC);
    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_ಸಾಲುಗಳು_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_ಸಾಲುಗಳು);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    VIM_CLEAR(ಸಾಲು_ಸ್ಥಾನಾಂತರ);
    VIM_CLEAR(ಸಾಲು_Wraps);
    VIM_CLEAR(Tab_ಪುಟ_Idxs);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    VIM_CLEAR(popup_ಮುಖವಾಡ);
    VIM_CLEAR(popup_ಮುಖವಾಡ_ಮುಂದಿನ);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_ಅಡ್ಡಸಾಲುಗಳು = 0;
    suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    ಪೂರ್ಣಾಂಕ
screenclear(ಶೂನ್ಯ)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    ಹಿಂತಿರುಗಿ screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    ಶೂನ್ಯ
redraw_as_cleared(ಶೂನ್ಯ)
{
    screenclear2(FALSE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
screenclear2(ಪೂರ್ಣಾಂಕ doclear)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    did_clear = FALSE;

    ಆದರೆ (starting == NO_SCREEN || Screen_ಸಾಲುಗಳು == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    || (gui.ಒಳ_use && gui.starting)
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#ಆದರೆ_ಅಂತ್ಯ

    // blank out ScreenLines
    ಗಾಗಿ (i = 0; i < ಅಡ್ಡಸಾಲುಗಳು; ++i)
    {
	lineclear(ಸಾಲು_ಸ್ಥಾನಾಂತರ[i], (ಪೂರ್ಣಾಂಕ)Columns, 0);
	ಸಾಲು_Wraps[i] = FALSE;
    }

    ಆದರೆ (doclear && can_clear(T_CL))
    {
	ಹೊರಹೋಗು_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	ಬಹುಲಕ_displayed = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ಆದರೆ (suppressed_cells != ಖಾಲಿ)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_ಅಡ್ಡಸಾಲುಗಳು * suppressed_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// can't clear the screen, mark all chars with invalid attributes
	ಗಾಗಿ (i = 0; i < ಅಡ್ಡಸಾಲುಗಳು; ++i)
	    lineinvalid(ಸಾಲು_ಸ್ಥಾನಾಂತರ[i], (ಪೂರ್ಣಾಂಕ)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    ಸಂದೇಶ_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#ಆದರೆ_ಅಂತ್ಯ
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;	// put cursor on last line for messages
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    screen_ಆರಂಭ();		// don't know where cursor is now
    ಸಂದೇಶ_didany = FALSE;
    ಸಂದೇಶ_didout = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    ಸರಿಸು_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
lineclear(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ attr)
{
    (ಶೂನ್ಯ)vim_memset(Screen_ಸಾಲುಗಳು + off, ' ', (size_t)ಅಗಲ * ಗಾತ್ರ(schar_T));
    ಆದರೆ (enc_utf8)
	(ಶೂನ್ಯ)vim_memset(Screen_ಸಾಲುಗಳು_UC + off, 0,
					  (size_t)ಅಗಲ * ಗಾತ್ರ(u8char_T));
    (ಶೂನ್ಯ)vim_memset(ScreenAttrs + off, attr, (size_t)ಅಗಲ * ಗಾತ್ರ(sattr_T));
    (ಶೂನ್ಯ)vim_memset(Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + off, -1, (size_t)ಅಗಲ * ಗಾತ್ರ(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
lineinvalid(ಚಿಹ್ನೆರಹಿತ off, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    (ಶೂನ್ಯ)vim_memset(ScreenAttrs + off, -1, (size_t)ಅಗಲ * ಗಾತ್ರ(sattr_T));
    (ಶೂನ್ಯ)vim_memset(Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + off, -1, (size_t)ಅಗಲ * ಗಾತ್ರ(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    ಶೂನ್ಯ
ಸಾಲು_was_clobbered(ಪೂರ್ಣಾಂಕ screen_lnum)
{
    lineinvalid(ಸಾಲು_ಸ್ಥಾನಾಂತರ[screen_lnum], (ಪೂರ್ಣಾಂಕ)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
linecopy(ಪೂರ್ಣಾಂಕ ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ from, win_T *wp)
{
    ಚಿಹ್ನೆರಹಿತ	off_ಗಮ್ಯ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಗಮ್ಯ] + wp->w_wincol;
    ಚಿಹ್ನೆರಹಿತ	off_from = ಸಾಲು_ಸ್ಥಾನಾಂತರ[from] + wp->w_wincol;

    mch_memmove(Screen_ಸಾಲುಗಳು + off_ಗಮ್ಯ, Screen_ಸಾಲುಗಳು + off_from,
	    wp->w_ಅಗಲ * ಗಾತ್ರ(schar_T));
    ಆದರೆ (enc_utf8)
    {
	ಪೂರ್ಣಾಂಕ	i;

	mch_memmove(Screen_ಸಾಲುಗಳು_UC + off_ಗಮ್ಯ, Screen_ಸಾಲುಗಳು_UC + off_from,
		wp->w_ಅಗಲ * ಗಾತ್ರ(u8char_T));
	ಗಾಗಿ (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_ಸಾಲುಗಳು_C[i] + off_ಗಮ್ಯ, Screen_ಸಾಲುಗಳು_C[i] + off_from,
		    wp->w_ಅಗಲ * ಗಾತ್ರ(u8char_T));
    }
    ಆದರೆ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_ಗಮ್ಯ, ScreenLines2 + off_from,
		wp->w_ಅಗಲ * ಗಾತ್ರ(schar_T));
    mch_memmove(ScreenAttrs + off_ಗಮ್ಯ, ScreenAttrs + off_from,
	    wp->w_ಅಗಲ * ಗಾತ್ರ(sattr_T));
    mch_memmove(Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + off_ಗಮ್ಯ, Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + off_from,
	    wp->w_ಅಗಲ * ಗಾತ್ರ(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    ಪೂರ್ಣಾಂಕ
can_clear(char_u *p)
{
    ಹಿಂತಿರುಗಿ (*p != NUL && (t_colors <= 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		|| gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_ಸಾಮಾನ್ಯ_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_ಸಾಮಾನ್ಯ_bg_color == 0)
#ಇಲ್ಲದಿದ್ದರೆ
		|| cterm_ಸಾಮಾನ್ಯ_bg_color == 0
#ಆದರೆ_ಅಂತ್ಯ
		|| *T_UT != NUL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#ಆದರೆ_ಅಂತ್ಯ
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    ಶೂನ್ಯ
screen_ಆರಂಭ(ಶೂನ್ಯ)
{
    screen_cur_ಅಡ್ಡಸಾಲು = screen_cur_ಲಂಬಸಾಲು = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    ಶೂನ್ಯ
windgoto(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    sattr_T	    *p;
    ಪೂರ್ಣಾಂಕ		    i;
    ಪೂರ್ಣಾಂಕ		    plan;
    ಪೂರ್ಣಾಂಕ		    ವೆಚ್ಚ;
    ಪೂರ್ಣಾಂಕ		    wouldbe_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		    noinvcurs;
    char_u	    *bs;
    ಪೂರ್ಣಾಂಕ		    goto_ವೆಚ್ಚ;
    ಪೂರ್ಣಾಂಕ		    attr;

#ವ್ಯಾಖ್ಯಾನಿಸು GOTO_ವೆಚ್ಚ   7	// assume a term_windgoto() takes about 7 chars
#ವ್ಯಾಖ್ಯಾನಿಸು HIGHL_ವೆಚ್ಚ  5	// assume unhighlight takes 5 chars

#ವ್ಯಾಖ್ಯಾನಿಸು PLAN_LE	    1
#ವ್ಯಾಖ್ಯಾನಿಸು PLAN_CR	    2
#ವ್ಯಾಖ್ಯಾನಿಸು PLAN_NL	    3
#ವ್ಯಾಖ್ಯಾನಿಸು PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಲಂಬಸಾಲು == screen_cur_ಲಂಬಸಾಲು && ಅಡ್ಡಸಾಲು == screen_cur_ಅಡ್ಡಸಾಲು)
	ಹಿಂತಿರುಗಿ;

    // Check for valid position.
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0)	// window without text lines?
	ಅಡ್ಡಸಾಲು = 0;
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು)
	ಅಡ್ಡಸಾಲು = screen_ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಆದರೆ (ಲಂಬಸಾಲು >= screen_Columns)
	ಲಂಬಸಾಲು = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ಆದರೆ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_ವೆಚ್ಚ;
    ಇಲ್ಲದಿದ್ದರೆ
	noinvcurs = 0;
    goto_ವೆಚ್ಚ = GOTO_ವೆಚ್ಚ + noinvcurs;

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
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_cur_ಅಡ್ಡಸಾಲು && screen_cur_ಲಂಬಸಾಲು < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = ಖಾಲಿ;			    // init for GCC
	attr = screen_attr;
	ಆದರೆ (ಅಡ್ಡಸಾಲು == screen_cur_ಅಡ್ಡಸಾಲು && ಲಂಬಸಾಲು < screen_cur_ಲಂಬಸಾಲು)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ಆದರೆ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    ಇಲ್ಲದಿದ್ದರೆ
		bs = T_BC;		    // "backspace character (old)
	    ಆದರೆ (*bs)
		ವೆಚ್ಚ = (screen_cur_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು) * (ಪೂರ್ಣಾಂಕ)STRLEN(bs);
	    ಇಲ್ಲದಿದ್ದರೆ
		ವೆಚ್ಚ = 999;
	    ಆದರೆ (ಲಂಬಸಾಲು + 1 < ವೆಚ್ಚ)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_ಲಂಬಸಾಲು = 0;
		ವೆಚ್ಚ = 1;		    // CR is just one character
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		plan = PLAN_LE;
		wouldbe_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    }
	    ಆದರೆ (noinvcurs)		    // will stop highlighting
	    {
		ವೆಚ್ಚ += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲು > screen_cur_ಅಡ್ಡಸಾಲು)
	{
	    plan = PLAN_NL;
	    wouldbe_ಲಂಬಸಾಲು = 0;
	    ವೆಚ್ಚ = (ಅಡ್ಡಸಾಲು - screen_cur_ಅಡ್ಡಸಾಲು) * 2;  // CR LF
	    ಆದರೆ (noinvcurs)		    // will stop highlighting
	    {
		ವೆಚ್ಚ += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    plan = PLAN_WRITE;
	    wouldbe_ಲಂಬಸಾಲು = screen_cur_ಲಂಬಸಾಲು;
	    ವೆಚ್ಚ = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = ಲಂಬಸಾಲು - wouldbe_ಲಂಬಸಾಲು;
	ಆದರೆ (i > 0)
	    ವೆಚ್ಚ += i;
	ಆದರೆ (ವೆಚ್ಚ < goto_ವೆಚ್ಚ && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + wouldbe_ಲಂಬಸಾಲು;
	    ಆಗ (i && *p++ == attr)
		--i;
	    ಆದರೆ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ಆದರೆ (*--p == 0)
		{
		    ವೆಚ್ಚ += noinvcurs;
		    ಆಗ (i && *p++ == 0)
			--i;
		}
		ಆದರೆ (i != 0)
		    ವೆಚ್ಚ = 999;	// different attributes, don't do it
	    }
	    ಆದರೆ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		ಗಾಗಿ (i = wouldbe_ಲಂಬಸಾಲು; i < ಲಂಬಸಾಲು; ++i)
		    ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + i] != 0)
		    {
			ವೆಚ್ಚ = 999;
			ನಿಲ್ಲಿಸು;
		    }
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ಆದರೆ (ವೆಚ್ಚ < 999)
		ಗಾಗಿ (i = wouldbe_ಲಂಬಸಾಲು; i < ಲಂಬಸಾಲು; ++i)
		    ಆದರೆ (popup_is_under_opacity(ಅಡ್ಡಸಾಲು, i))
		    {
			ವೆಚ್ಚ = 999;
			ನಿಲ್ಲಿಸು;
		    }
#ಆದರೆ_ಅಂತ್ಯ
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ಆದರೆ (ವೆಚ್ಚ < goto_ವೆಚ್ಚ)
	{
	    ಆದರೆ (plan == PLAN_LE)
	    {
		ಆದರೆ (noinvcurs)
		    screen_stop_highlight();
		ಆಗ (screen_cur_ಲಂಬಸಾಲು > ಲಂಬಸಾಲು)
		{
		    ಹೊರಹೋಗು_str(bs);
		    --screen_cur_ಲಂಬಸಾಲು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (plan == PLAN_CR)
	    {
		ಆದರೆ (noinvcurs)
		    screen_stop_highlight();
		ಹೊರಹೋಗು_char('\r');
		screen_cur_ಲಂಬಸಾಲು = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (plan == PLAN_NL)
	    {
		ಆದರೆ (noinvcurs)
		    screen_stop_highlight();
		ಆಗ (screen_cur_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲು)
		{
		    ಹೊರಹೋಗು_char('\n');
		    ++screen_cur_ಅಡ್ಡಸಾಲು;
		}
		screen_cur_ಲಂಬಸಾಲು = 0;
	    }

	    i = ಲಂಬಸಾಲು - screen_cur_ಲಂಬಸಾಲು;
	    ಆದರೆ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ಆದರೆ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    ಆಗ (i-- > 0)
			ಹೊರಹೋಗು_char(*T_ND);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ	off;

		    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + screen_cur_ಲಂಬಸಾಲು;
		    ಆಗ (i-- > 0)
		    {
			ಆದರೆ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			ಹೊರಹೋಗು_flush_check();
			ಹೊರಹೋಗು_char(Screen_ಸಾಲುಗಳು[off]);
			ಆದರೆ (enc_dbcs == DBCS_JPNU
				&& Screen_ಸಾಲುಗಳು[off] == 0x8e)
			    ಹೊರಹೋಗು_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ವೆಚ್ಚ = 999;

    ಆದರೆ (ವೆಚ್ಚ >= goto_ವೆಚ್ಚ)
    {
	ಆದರೆ (noinvcurs)
	    screen_stop_highlight();
	ಆದರೆ (ಅಡ್ಡಸಾಲು == screen_cur_ಅಡ್ಡಸಾಲು && (ಲಂಬಸಾಲು > screen_cur_ಲಂಬಸಾಲು)
		&& *T_CRI != NUL)
	    ಪದ_cursor_ಬಲ(ಲಂಬಸಾಲು - screen_cur_ಲಂಬಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪದ_windgoto(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
    }
    screen_cur_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    screen_cur_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
}

/*
 * Set cursor to its position in the current window.
 */
    ಶೂನ್ಯ
setcursor(ಶೂನ್ಯ)
{
    setcursor_mayforce(FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    ಪೂರ್ಣಾಂಕ
cursor_screen_cells(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    ಶೂನ್ಯ
setcursor_mayforce(ಪೂರ್ಣಾಂಕ force)
{
    ಆದರೆ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((ಪೂರ್ಣಾಂಕ)curwin->w_ಅಗಲ - curwin->w_wcol
					    - cursor_screen_cells()) :
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
win_ins_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		invalid,
    ಪೂರ್ಣಾಂಕ		mayclear)
{
    ಪೂರ್ಣಾಂಕ		did_delete;
    ಪೂರ್ಣಾಂಕ		nextrow;
    ಪೂರ್ಣಾಂಕ		lastrow;
    ಪೂರ್ಣಾಂಕ		retval;

    ಆದರೆ (invalid)
	wp->w_ಸಾಲುಗಳು_valid = 0;

    // with only a few lines it's not worth the effort
    ಆದರೆ (wp->w_ಎತ್ತರ < 5)
	ಹಿಂತಿರುಗಿ FAIL;

    // with the popup menu visible this might not work correctly
    ಆದರೆ (pum_visible())
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು)
	ಸಾಲು_ಎಣಿಕೆ = wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು;

    retval = win_do_ಸಾಲುಗಳು(wp, ಅಡ್ಡಸಾಲು, ಸಾಲು_ಎಣಿಕೆ, mayclear, FALSE, 0);
    ಆದರೆ (retval != MAYBE)
	ಹಿಂತಿರುಗಿ retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ಆದರೆ (wp->w_ಮುಂದಿನ != ಖಾಲಿ || wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
    {
	ಆದರೆ (screen_del_ಸಾಲುಗಳು(0, W_WINROW(wp) + wp->w_ಎತ್ತರ - ಸಾಲು_ಎಣಿಕೆ,
				  ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, FALSE, 0, ಖಾಲಿ) == OK)
	    did_delete = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಮುಂದಿನ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ಆದರೆ (!did_delete)
    {
	wp->w_redr_ಸ್ಥಿತಿ = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	lastrow = nextrow + ಸಾಲು_ಎಣಿಕೆ;
	ಆದರೆ (lastrow > ಅಡ್ಡಸಾಲುಗಳು)
	    lastrow = ಅಡ್ಡಸಾಲುಗಳು;
	screen_fill(nextrow - ಸಾಲು_ಎಣಿಕೆ, lastrow - ಸಾಲು_ಎಣಿಕೆ,
		  wp->w_wincol, (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ಆದರೆ (screen_ins_ಸಾಲುಗಳು(0, W_WINROW(wp) + ಅಡ್ಡಸಾಲು, ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, ಖಾಲಿ)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ಆದರೆ (did_delete)
	{
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	    win_rest_invalid(W_ಮುಂದಿನ(wp));
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ಪೂರ್ಣಾಂಕ
win_del_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		invalid,
    ಪೂರ್ಣಾಂಕ		mayclear,
    ಪೂರ್ಣಾಂಕ		clear_attr)	    // for clearing lines
{
    ಪೂರ್ಣಾಂಕ		retval;

    ಆದರೆ (invalid)
	wp->w_ಸಾಲುಗಳು_valid = 0;

    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು)
	ಸಾಲು_ಎಣಿಕೆ = wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು;

    retval = win_do_ಸಾಲುಗಳು(wp, ಅಡ್ಡಸಾಲು, ಸಾಲು_ಎಣಿಕೆ, mayclear, TRUE, clear_attr);
    ಆದರೆ (retval != MAYBE)
	ಹಿಂತಿರುಗಿ retval;

    ಆದರೆ (screen_del_ಸಾಲುಗಳು(0, W_WINROW(wp) + ಅಡ್ಡಸಾಲು, ಸಾಲು_ಎಣಿಕೆ,
				   (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, FALSE, clear_attr, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ಆದರೆ (wp->w_ಮುಂದಿನ || wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ || cmdline_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು - 1)
    {
	ಆದರೆ (screen_ins_ಸಾಲುಗಳು(0, W_WINROW(wp) + wp->w_ಎತ್ತರ - ಸಾಲು_ಎಣಿಕೆ,
			      ಸಾಲು_ಎಣಿಕೆ, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, clear_attr, ಖಾಲಿ) == FAIL)
	{
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	    win_rest_invalid(wp->w_ಮುಂದಿನ);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    ಇಲ್ಲದಿದ್ದರೆ
	redraw_cmdline = TRUE;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_do_ಸಾಲುಗಳು(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		mayclear,
    ಪೂರ್ಣಾಂಕ		del,
    ಪೂರ್ಣಾಂಕ		clear_attr)
{
    ಪೂರ್ಣಾಂಕ		retval;

    ಆದರೆ (!redrawing() || ಸಾಲು_ಎಣಿಕೆ <= 0)
	ಹಿಂತಿರುಗಿ FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ಆದರೆ (no_win_do_ಸಾಲುಗಳು_ins && !del)
	ಹಿಂತಿರುಗಿ FAIL;

    // only a few lines left: redraw is faster
    ಆದರೆ (mayclear && ಅಡ್ಡಸಾಲುಗಳು - ಸಾಲು_ಎಣಿಕೆ < 5
	    && wp->w_ಅಗಲ == topframe->fr_ಅಗಲ)
    {
	ಆದರೆ (!no_win_do_ಸಾಲುಗಳು_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ಆದರೆ (popup_visible)
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    // Delete all remaining lines
    ಆದರೆ (ಅಡ್ಡಸಾಲು + ಸಾಲು_ಎಣಿಕೆ >= wp->w_ಎತ್ತರ)
    {
	screen_fill(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, W_WINROW(wp) + wp->w_ಎತ್ತರ,
		wp->w_wincol, (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp), ' ', ' ', 0);
	ಹಿಂತಿರುಗಿ OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ಆದರೆ (!no_win_do_ಸಾಲುಗಳು_ins)
	clear_cmdline = TRUE;

#ಆದರೆ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ಆದರೆ (tabpanel_ಅಗಲ() > 0)
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    ಆದರೆ (scroll_region || wp->w_ಅಗಲ != topframe->fr_ಅಗಲ)
    {
	ಆದರೆ (scroll_region && (wp->w_ಅಗಲ == topframe->fr_ಅಗಲ
		    || *T_CSV != NUL))
	    scroll_region_ಗಣ(wp, ಅಡ್ಡಸಾಲು);
	ಆದರೆ (del)
	    retval = screen_del_ಸಾಲುಗಳು(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, 0, ಸಾಲು_ಎಣಿಕೆ,
				    wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು, FALSE, clear_attr, wp);
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = screen_ins_ಸಾಲುಗಳು(W_WINROW(wp) + ಅಡ್ಡಸಾಲು, 0, ಸಾಲು_ಎಣಿಕೆ,
					   wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು, clear_attr, wp);
	ಆದರೆ (scroll_region && (wp->w_ಅಗಲ == topframe->fr_ಅಗಲ
		    || *T_CSV != NUL))
	    scroll_region_reset();
	ಹಿಂತಿರುಗಿ retval;
    }

    ಆದರೆ (wp->w_ಮುಂದಿನ != ಖಾಲಿ && p_tf) // don't delete/insert on fast terminal
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_rest_invalid(win_T *wp)
{
    ಆಗ (wp != ಖಾಲಿ)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_ಸ್ಥಿತಿ = true;
	wp = wp->w_ಮುಂದಿನ;
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
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_CAL   1
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_CDL   2
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_AL    3
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_CE    4
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_DL    5
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_SR    6
#ವ್ಯಾಖ್ಯಾನಿಸು USE_NL	    7
#ವ್ಯಾಖ್ಯಾನಿಸು USE_T_CD    8
#ವ್ಯಾಖ್ಯಾನಿಸು USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    ಪೂರ್ಣಾಂಕ
screen_ins_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಂಕ		off,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    ಚಿಹ್ನೆರಹಿತ	ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ		cursor_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		cursor_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		type;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು;
    ಪೂರ್ಣಾಂಕ		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ಆದರೆ (!screen_valid(TRUE)
	     || ಸಾಲು_ಎಣಿಕೆ <= 0 || ಸಾಲು_ಎಣಿಕೆ > p_ttyscroll
	     || ಕೊನೆ > ಅಡ್ಡಸಾಲುಗಳು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	     || (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#ಆದರೆ_ಅಂತ್ಯ
	     )
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	ಸರಿಸು_image_placements(off + ಅಡ್ಡಸಾಲು, off + ಕೊನೆ, ಸಾಲು_ಎಣಿಕೆ);
# endif
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

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
    ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು = (ಅಡ್ಡಸಾಲು + ಸಾಲು_ಎಣಿಕೆ >= ಕೊನೆ);
    ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > 3)
	    ಹಿಂತಿರುಗಿ FAIL;
	type = USE_REDRAW;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_clear(T_CD) && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು)
	type = USE_T_CD;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CAL != NUL && (ಸಾಲು_ಎಣಿಕೆ > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CDL != NUL && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು && (ಸಾಲು_ಎಣಿಕೆ > 1 || !can_ce))
	type = USE_T_CDL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_AL != NUL)
	type = USE_T_AL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_ce && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು)
	type = USE_T_CE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_DL != NUL && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು)
	type = USE_T_DL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_SR != NUL && ಅಡ್ಡಸಾಲು == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ಆದರೆ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	ಹಿಂತಿರುಗಿ screen_del_ಸಾಲುಗಳು(off, ಅಡ್ಡಸಾಲು, ಸಾಲು_ಎಣಿಕೆ, ಕೊನೆ, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ಆದರೆ (*T_DB)
	screen_del_ಸಾಲುಗಳು(off, ಕೊನೆ - ಸಾಲು_ಎಣಿಕೆ, ಸಾಲು_ಎಣಿಕೆ, ಕೊನೆ, FALSE, 0, wp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ಆದರೆ (off + ಅಡ್ಡಸಾಲು > 0 || (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ))
	clip_clear_selection(&clip_ನಕ್ಷತ್ರ);
    ಇಲ್ಲದಿದ್ದರೆ
	clip_scroll_selection(-ಸಾಲು_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    ಸರಿಸು_image_placements(off + ಅಡ್ಡಸಾಲು, off + ಕೊನೆ, ಸಾಲು_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_ಬೀಗ_screen();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(ಅಡ್ಡಸಾಲು + off <= gui.cursor_ಅಡ್ಡಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp != ಖಾಲಿ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ಲಂಬಸಾಲು = wp->w_wincol;

    ಆದರೆ (*T_CCS != NUL)	   // cursor relative to region
	cursor_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	cursor_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    ಅಡ್ಡಸಾಲು += off;
    ಕೊನೆ += off;
    ಗಾಗಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ)
	{
	    // need to copy part of a line
	    j = ಕೊನೆ - 1 - i;
	    ಆಗ ((j -= ಸಾಲು_ಎಣಿಕೆ) >= ಅಡ್ಡಸಾಲು)
		linecopy(j + ಸಾಲು_ಎಣಿಕೆ, j, wp);
	    j += ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (can_clear((char_u *)" "))
		lineclear(ಸಾಲು_ಸ್ಥಾನಾಂತರ[j] + wp->w_wincol,
			wp->w_ಅಗಲ, clear_attr);
	    ಇಲ್ಲದಿದ್ದರೆ
		lineinvalid(ಸಾಲು_ಸ್ಥಾನಾಂತರ[j] + wp->w_wincol, wp->w_ಅಗಲ);
	    ಸಾಲು_Wraps[j] = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    j = ಕೊನೆ - 1 - i;
	    ತಾತ್ಕಾಲಿಕ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[j];
	    ಆಗ ((j -= ಸಾಲು_ಎಣಿಕೆ) >= ಅಡ್ಡಸಾಲು)
	    {
		ಸಾಲು_ಸ್ಥಾನಾಂತರ[j + ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_ಸ್ಥಾನಾಂತರ[j];
		ಸಾಲು_Wraps[j + ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Wraps[j];
	    }
	    ಸಾಲು_ಸ್ಥಾನಾಂತರ[j + ಸಾಲು_ಎಣಿಕೆ] = ತಾತ್ಕಾಲಿಕ;
	    ಸಾಲು_Wraps[j + ಸಾಲು_ಎಣಿಕೆ] = FALSE;
	    ಆದರೆ (can_clear((char_u *)" "))
		lineclear(ತಾತ್ಕಾಲಿಕ, (ಪೂರ್ಣಾಂಕ)Columns, clear_attr);
	    ಇಲ್ಲದಿದ್ದರೆ
		lineinvalid(ತಾತ್ಕಾಲಿಕ, (ಪೂರ್ಣಾಂಕ)Columns);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ಆದರೆ_ಅಂತ್ಯ

    screen_stop_highlight();
    windgoto(cursor_ಅಡ್ಡಸಾಲು, cursor_ಲಂಬಸಾಲು);
    ಆದರೆ (clear_attr != 0)
	screen_ಆರಂಭ_highlight(clear_attr);

    // redraw the characters
    ಆದರೆ (type == USE_REDRAW)
	redraw_ಖಂಡ(ಅಡ್ಡಸಾಲು, ಕೊನೆ, wp);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == USE_T_CAL)
    {
	ಪದ_append_ಸಾಲುಗಳು(ಸಾಲು_ಎಣಿಕೆ);
	screen_ಆರಂಭ();		// don't know where cursor is now
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (type == USE_T_AL)
	    {
		ಆದರೆ (i && cursor_ಅಡ್ಡಸಾಲು != 0)
		    windgoto(cursor_ಅಡ್ಡಸಾಲು, cursor_ಲಂಬಸಾಲು);
		ಹೊರಹೋಗು_str(T_AL);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ  // type == USE_T_SR
		ಹೊರಹೋಗು_str(T_SR);
	    screen_ಆರಂಭ();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ಆದರೆ (type == USE_T_SR && *T_DA)
    {
	ಗಾಗಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
	{
	    windgoto(off + i, cursor_ಲಂಬಸಾಲು);
	    ಹೊರಹೋಗು_str(T_CE);
	    screen_ಆರಂಭ();	    // don't know where cursor is now
	}
    }

#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    gui_can_update_cursor();
    ಆದರೆ (gui.ಒಳ_use)
	ಹೊರಹೋಗು_flush();	// always flush after a scroll
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    ಪೂರ್ಣಾಂಕ
screen_del_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಂಕ		off,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		force,		// even when line_count > p_ttyscroll
    ಪೂರ್ಣಾಂಕ		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		i;
    ಚಿಹ್ನೆರಹಿತ	ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ		cursor_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		cursor_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		cursor_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು;	// result is empty until end of region
    ಪೂರ್ಣಾಂಕ		can_delete;	// deleting line codes can be used
    ಪೂರ್ಣಾಂಕ		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ಆದರೆ (!screen_valid(TRUE)
	    || ಸಾಲು_ಎಣಿಕೆ <= 0
	    || (!force && ಸಾಲು_ಎಣಿಕೆ > p_ttyscroll)
	    || ಕೊನೆ > ಅಡ್ಡಸಾಲುಗಳು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	    || (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED && redrawing_for_callback > 0)
#ಆದರೆ_ಅಂತ್ಯ
       )
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು = ಅಡ್ಡಸಾಲು + ಸಾಲು_ಎಣಿಕೆ >= ಕೊನೆ;

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
    ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > 3)
	    ಹಿಂತಿರುಗಿ FAIL;
	type = USE_REDRAW;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_clear(T_CD) && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು)
	type = USE_T_CD;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲು == 0 && (
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    ಸಾಲು_ಎಣಿಕೆ == 1 ||
#ಆದರೆ_ಅಂತ್ಯ
						*T_CDL == NUL))
	type = USE_NL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CDL != NUL && ಸಾಲು_ಎಣಿಕೆ > 1 && can_delete)
	type = USE_T_CDL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_clear(T_CE) && ಫಲಿತಾಂಶ_ಖಾಲಿಗಳು
	    && (wp == ಖಾಲಿ || wp->w_ಅಗಲ == topframe->fr_ಅಗಲ))
	type = USE_T_CE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ಆದರೆ (off + ಅಡ್ಡಸಾಲು > 0 || (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ))
	clip_clear_selection(&clip_ನಕ್ಷತ್ರ);
    ಇಲ್ಲದಿದ್ದರೆ
	clip_scroll_selection(ಸಾಲು_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    ಸರಿಸು_image_placements(off + ಅಡ್ಡಸಾಲು, off + ಕೊನೆ, -ಸಾಲು_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_ಬೀಗ_screen();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲು + off
						&& gui.cursor_ಅಡ್ಡಸಾಲು < ಕೊನೆ + off);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp != ಖಾಲಿ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_ಲಂಬಸಾಲು = wp->w_wincol;

    ಆದರೆ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	cursor_ಕೊನೆ = ಕೊನೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cursor_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು + off;
	cursor_ಕೊನೆ = ಕೊನೆ + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    ಅಡ್ಡಸಾಲು += off;
    ಕೊನೆ += off;
    ಗಾಗಿ (i = 0; i < ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಅಗಲ != topframe->fr_ಅಗಲ)
	{
	    // need to copy part of a line
	    j = ಅಡ್ಡಸಾಲು + i;
	    ಆಗ ((j += ಸಾಲು_ಎಣಿಕೆ) <= ಕೊನೆ - 1)
		linecopy(j - ಸಾಲು_ಎಣಿಕೆ, j, wp);
	    j -= ಸಾಲು_ಎಣಿಕೆ;
	    ಆದರೆ (can_clear((char_u *)" "))
		lineclear(ಸಾಲು_ಸ್ಥಾನಾಂತರ[j] + wp->w_wincol,
			wp->w_ಅಗಲ, clear_attr);
	    ಇಲ್ಲದಿದ್ದರೆ
		lineinvalid(ಸಾಲು_ಸ್ಥಾನಾಂತರ[j] + wp->w_wincol, wp->w_ಅಗಲ);
	    ಸಾಲು_Wraps[j] = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // whole width, moving the line pointers is faster
	    j = ಅಡ್ಡಸಾಲು + i;
	    ತಾತ್ಕಾಲಿಕ = ಸಾಲು_ಸ್ಥಾನಾಂತರ[j];
	    ಆಗ ((j += ಸಾಲು_ಎಣಿಕೆ) <= ಕೊನೆ - 1)
	    {
		ಸಾಲು_ಸ್ಥಾನಾಂತರ[j - ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_ಸ್ಥಾನಾಂತರ[j];
		ಸಾಲು_Wraps[j - ಸಾಲು_ಎಣಿಕೆ] = ಸಾಲು_Wraps[j];
	    }
	    ಸಾಲು_ಸ್ಥಾನಾಂತರ[j - ಸಾಲು_ಎಣಿಕೆ] = ತಾತ್ಕಾಲಿಕ;
	    ಸಾಲು_Wraps[j - ಸಾಲು_ಎಣಿಕೆ] = FALSE;
	    ಆದರೆ (can_clear((char_u *)" "))
		lineclear(ತಾತ್ಕಾಲಿಕ, (ಪೂರ್ಣಾಂಕ)Columns, clear_attr);
	    ಇಲ್ಲದಿದ್ದರೆ
		lineinvalid(ತಾತ್ಕಾಲಿಕ, (ಪೂರ್ಣಾಂಕ)Columns);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (screen_attr != clear_attr)
	screen_stop_highlight();
    ಆದರೆ (clear_attr != 0)
	screen_ಆರಂಭ_highlight(clear_attr);

    // redraw the characters
    ಆದರೆ (type == USE_REDRAW)
	redraw_ಖಂಡ(ಅಡ್ಡಸಾಲು, ಕೊನೆ, wp);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_ಅಡ್ಡಸಾಲು, cursor_ಲಂಬಸಾಲು);
	ಹೊರಹೋಗು_str(T_CD);
	screen_ಆರಂಭ();			// don't know where cursor is now
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == USE_T_CDL)
    {
	windgoto(cursor_ಅಡ್ಡಸಾಲು, cursor_ಲಂಬಸಾಲು);
	ಪದ_delete_ಸಾಲುಗಳು(ಸಾಲು_ಎಣಿಕೆ);
	screen_ಆರಂಭ();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == USE_NL)
    {
	windgoto(cursor_ಕೊನೆ - 1, cursor_ಲಂಬಸಾಲು);
	ಗಾಗಿ (i = ಸಾಲು_ಎಣಿಕೆ; --i >= 0; )
	    ಹೊರಹೋಗು_char('\n');		// cursor will remain on same line
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = ಸಾಲು_ಎಣಿಕೆ; --i >= 0; )
	{
	    ಆದರೆ (type == USE_T_DL)
	    {
		windgoto(cursor_ಅಡ್ಡಸಾಲು, cursor_ಲಂಬಸಾಲು);
		ಹೊರಹೋಗು_str(T_DL);		// delete a line
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // type == USE_T_CE
	    {
		windgoto(cursor_ಅಡ್ಡಸಾಲು + i, cursor_ಲಂಬಸಾಲು);
		ಹೊರಹೋಗು_str(T_CE);		// erase a line
	    }
	    screen_ಆರಂಭ();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    ಆದರೆ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	ಗಾಗಿ (i = ಸಾಲು_ಎಣಿಕೆ; i > 0; --i)
	{
	    windgoto(cursor_ಕೊನೆ - i, cursor_ಲಂಬಸಾಲು);
	    ಹೊರಹೋಗು_str(T_CE);		// erase a line
	    screen_ಆರಂಭ();		// don't know where cursor is now
	}
    }

#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    gui_can_update_cursor();
    ಆದರೆ (gui.ಒಳ_use)
	ಹೊರಹೋಗು_flush();	// always flush after a scroll
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    ಪೂರ್ಣಾಂಕ
skip_showmode(ಶೂನ್ಯ)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ಆದರೆ (global_busy
	    || ಸಂದೇಶ_silent != 0
	    || !redrawing()
	    || ((!stuff_ಖಾಲಿಗಳು() || typebuf.tb_len > 0 || using_script())
		&& !ಕೀಲಿ_Typed))
    {
	redraw_ಬಹುಲಕ = TRUE;		// show mode later
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ
showmode(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		need_clear;
    ಪೂರ್ಣಾಂಕ		ಉದ್ದಳತೆ = 0;
    ಪೂರ್ಣಾಂಕ		do_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		nwr_save;
    ಪೂರ್ಣಾಂಕ		sub_attr;
    ಪೂರ್ಣಾಂಕ		show_ruler_with_pum = FALSE;

    do_ಬಹುಲಕ = p_smd && ಸಂದೇಶ_silent == 0
	    && ((State & ಬಹುಲಕ_ಸೇರಿಸು)
		|| restart_edit != NUL
		|| VIsual_ಸಕ್ರಿಯ);
    ಆದರೆ (do_ಬಹುಲಕ || reg_recording != 0)
    {
	ಆದರೆ (skip_showmode())
	    ಹಿಂತಿರುಗಿ 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ಆದರೆ (clear_cmdline && cmdline_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು - 1)
	    ಸಂದೇಶ_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	ಸಂದೇಶ_pos_ಬಹುಲಕ();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ಆದರೆ (do_ಬಹುಲಕ)
	{
	    ಸಂದೇಶ_puts_attr("--", attr);
#ಆದರೆ defined(FEAT_XIM)
	    ಆದರೆ (
# ifdef FEAT_GUI_GTK
		    preedit_get_ಸ್ಥಿತಿ()
# ಇಲ್ಲದಿದ್ದರೆ
		    im_get_ಸ್ಥಿತಿ()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		ಸಂದೇಶ_puts_attr(" IM", attr);
# ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts_attr(" XIM", attr);
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    // CTRL-X in Insert mode
	    ಆದರೆ (edit_submode != ಖಾಲಿ && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		ಉದ್ದಳತೆ = (ಅಡ್ಡಸಾಲುಗಳು - ಸಂದೇಶ_ಅಡ್ಡಸಾಲು) * cmdline_ಅಗಲ - 3;
		ಆದರೆ (edit_submode_extra != ಖಾಲಿ)
		    ಉದ್ದಳತೆ -= vim_strsize(edit_submode_extra);
		ಆದರೆ (ಉದ್ದಳತೆ > 0)
		{
		    ಆದರೆ (edit_submode_pre != ಖಾಲಿ)
			ಉದ್ದಳತೆ -= vim_strsize(edit_submode_pre);
		    ಆದರೆ (ಉದ್ದಳತೆ - vim_strsize(edit_submode) > 0)
		    {
			ಆದರೆ (edit_submode_pre != ಖಾಲಿ)
			    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)edit_submode_pre, attr);
			ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)edit_submode, attr);
		    }
		    ಆದರೆ (edit_submode_extra != ಖಾಲಿ)
		    {
			ಸಂದೇಶ_puts_attr(" ", attr);  // add a space in between
			ಆದರೆ ((ಪೂರ್ಣಾಂಕ)edit_submode_highl < (ಪೂರ್ಣಾಂಕ)HLF_ಎಣಿಕೆ)
			    sub_attr = HL_ATTR(edit_submode_highl);
			ಇಲ್ಲದಿದ್ದರೆ
			    sub_attr = attr;
			ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (State & VREPLACE_FLAG)
		    ಸಂದೇಶ_puts_attr(_(" VREPLACE"), attr);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & REPLACE_FLAG)
		    ಸಂದೇಶ_puts_attr(_(" REPLACE"), attr);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    ಆದರೆ (p_ri)
			ಸಂದೇಶ_puts_attr(_(" REVERSE"), attr);
#ಆದರೆ_ಅಂತ್ಯ
		    ಸಂದೇಶ_puts_attr(_(" INSERT"), attr);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    ಸಂದೇಶ_puts_attr(_(" (insert)"), attr);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'R')
		    ಸಂದೇಶ_puts_attr(_(" (replace)"), attr);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'V')
		    ಸಂದೇಶ_puts_attr(_(" (vreplace)"), attr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (p_hkmap)
		    ಸಂದೇಶ_puts_attr(_(" Hebrew"), attr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
		ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ಆದರೆ (curwin->w_p_arab)
			ಸಂದೇಶ_puts_attr(_(" Arabic"), attr);
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
			ಆದರೆ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)NameBuff, attr);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && p_paste)
		    ಸಂದೇಶ_puts_attr(_(" (paste)"), attr);

		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		{
		    ಅಕ್ಷರ *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    ಆಯ್ಕೆ ((VIsual_select ? 4 : 0)
			    + (VIsual_ಬಹುಲಕ == Ctrl_V) * 2
			    + (VIsual_ಬಹುಲಕ == 'V'))
		    {
			ಸ್ಥಿತಿ 0:	p = N_(" VISUAL"); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 1: p = N_(" VISUAL LINE"); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 2: p = N_(" VISUAL BLOCK"); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 4: p = N_(" SELECT"); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 5: p = N_(" SELECT LINE"); ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ: p = N_(" SELECT BLOCK"); ನಿಲ್ಲಿಸು;
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
	ಆದರೆ (reg_recording != 0
		&& edit_submode == ಖಾಲಿ)    // otherwise it gets too long
	{
	    recording_ಬಹುಲಕ(attr);
	    need_clear = TRUE;
	}

	ಬಹುಲಕ_displayed = TRUE;
	ಆದರೆ (need_clear || clear_cmdline || redraw_ಬಹುಲಕ)
	    ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_didout = FALSE;		// overwrite this message
	ಉದ್ದಳತೆ = ಸಂದೇಶ_ಲಂಬಸಾಲು;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clear_cmdline && ಸಂದೇಶ_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	ಸಂದೇಶ_clr_cmdline();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redraw_ಬಹುಲಕ)
    {
	ಸಂದೇಶ_pos_ಬಹುಲಕ();
	ಸಂದೇಶ_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ಆದರೆ (redrawing() && lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_ಬಹುಲಕ = FALSE;
    clear_cmdline = FALSE;

    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;
}

/*
 * Position for a mode message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_pos_ಬಹುಲಕ(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    ಶೂನ್ಯ
unshowmode(ಪೂರ್ಣಾಂಕ force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ಆದರೆ (!redrawing() || (!force && char_avail() && !ಕೀಲಿ_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    ಇಲ್ಲದಿದ್ದರೆ
	clearmode();
}

/*
 * Clear the mode message.
 */
    ಶೂನ್ಯ
clearmode(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ save_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ save_ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;

    ಸಂದೇಶ_pos_ಬಹುಲಕ();
    ಆದರೆ (reg_recording != 0)
	recording_ಬಹುಲಕ(HL_ATTR(HLF_CM));
    ಸಂದೇಶ_clr_eos();

    ಸಂದೇಶ_ಲಂಬಸಾಲು = save_ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = save_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
recording_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ attr)
{
    ಸಂದೇಶ_puts_attr(_("recording"), attr);
    ಆದರೆ (shortmess(SHM_RECORDING))
	ಹಿಂತಿರುಗಿ;

    ಅಕ್ಷರ s[4];

    sprintf(s, " @%c", reg_recording);
    ಸಂದೇಶ_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mouse_has_any(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ mouse_has(MOUSE_ಸಾಮಾನ್ಯ) || mouse_has(MOUSE_ಸೇರಿಸು)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    ಶೂನ್ಯ
draw_tabline(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		tabcount = 0;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		tabwidth;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		scol = 0;
    ಪೂರ್ಣಾಂಕ		attr;
    win_T	*wp;
    win_T	*cwp;
    ಪೂರ್ಣಾಂಕ		wincount;
    ಪೂರ್ಣಾಂಕ		modified;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		attr_sel;
    ಪೂರ್ಣಾಂಕ		attr_nosel = 0;
    ಪೂರ್ಣಾಂಕ		attr_fill = 0;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಳ;
    ಪೂರ್ಣಾಂಕ		use_sep_chars = (t_colors < 8
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
					    && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
					    && !p_tgc
#ಆದರೆ_ಅಂತ್ಯ
					    );
    bool	override_success;

#ಆದರೆ defined(FEAT_TABPANEL)
    ಲಂಬಸಾಲು = firstwin->w_wincol;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    redraw_tabline = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ಆದರೆ (gui_use_tabline())
    {
	gui_update_tabline();
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (tabline_ಎತ್ತರ() < 1)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(FEAT_STL_OPT)
    clear_Tab_ಪುಟ_Idxs();

    // Use the 'tabline' option if it's set.
    ಆದರೆ (*p_tal != NUL)
	win_redr_custom(ಖಾಲಿ, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_ಅಗಲ - 1 + tabcount / 2) / tabcount;
	ಆದರೆ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ && ಲಂಬಸಾಲು < Columns - 4;
							     tp = tp->tp_ಮುಂದಿನ)
	{
	    scol = ಲಂಬಸಾಲು;

	    override_success = ತಳ್ಳು_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    ಆದರೆ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ಆದರೆ (use_sep_chars && ಲಂಬಸಾಲು > 0)
		screen_putchar('|', 0, ಲಂಬಸಾಲು++, attr);

	    ಆದರೆ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, ಲಂಬಸಾಲು++, attr);

	    ಆದರೆ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    ಗಾಗಿ (wincount = 0; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ, ++wincount)
		ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್))
		    modified = TRUE;
	    ಆದರೆ (modified || wincount > 1)
	    {
		ಆದರೆ (wincount > 1)
		{
		    len = vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL, "%d", wincount);
		    ಆದರೆ (ಲಂಬಸಾಲು + len >= Columns - 3)
			ನಿಲ್ಲಿಸು;
		    screen_puts_len(NameBuff, len, 0, ಲಂಬಸಾಲು,
#ಆದರೆ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#ಇಲ್ಲದಿದ್ದರೆ
					 attr
#ಆದರೆ_ಅಂತ್ಯ
					       );
		    ಲಂಬಸಾಲು += len;
		}
		ಆದರೆ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, ಲಂಬಸಾಲು++, attr);
		screen_putchar(' ', 0, ಲಂಬಸಾಲು++, attr);
	    }

	    ಸ್ಥಳ = scol - ಲಂಬಸಾಲು + tabwidth - 1;
	    ಆದರೆ (ಸ್ಥಳ > 0)
	    {
		ಪೂರ್ಣಾಂಕ n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_ಬಫರ್);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ಆದರೆ (has_mbyte)
		    ಆಗ (len > ಸ್ಥಳ)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > ಸ್ಥಳ)
		{
		    p += len - ಸ್ಥಳ;
		    len = ಸ್ಥಳ;
		}
		n = Columns - ಲಂಬಸಾಲು - 1;
		ಆದರೆ (len > n)
		    len = n;

		screen_puts_len(p, (ಪೂರ್ಣಾಂಕ)STRLEN(p), 0, ಲಂಬಸಾಲು, attr);
		ಲಂಬಸಾಲು += len;
	    }
	    screen_putchar(' ', 0, ಲಂಬಸಾಲು++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    ಆಗ (scol < ಲಂಬಸಾಲು)
		Tab_ಪುಟ_Idxs[scol++] = tabcount;

	    ಆದರೆ (override_success)
		ತೆಗೆ_highlight_overrides();
	}

	ಆದರೆ (use_sep_chars)
	    c = '_';
	ಇಲ್ಲದಿದ್ದರೆ
	    c = ' ';
	screen_fill(0, 1, ಲಂಬಸಾಲು, (ಪೂರ್ಣಾಂಕ)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ಆದರೆ (p_sc && *p_sloc == 't')
	{
	    ಪೂರ್ಣಾಂಕ	n = (ಪೂರ್ಣಾಂಕ)Columns - ಲಂಬಸಾಲು - (tabcount > 1) * 3;
	    ಪೂರ್ಣಾಂಕ	ಅಗಲ = MIN(10, n);

	    ಆದರೆ (ಅಗಲ > 0)
		screen_puts_len(showcmd_buf, ಅಗಲ, 0, (ಪೂರ್ಣಾಂಕ)Columns
			    - ಅಗಲ - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ಆದರೆ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (ಪೂರ್ಣಾಂಕ)Columns - 1, attr_nosel);
	    Tab_ಪುಟ_Idxs[Columns - 1] = -999;
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
    ಆದರೆ (buf_spname(buf) != ಖಾಲಿ)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    ಇಲ್ಲದಿದ್ದರೆ
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    ಪೂರ್ಣಾಂಕ
fillchar_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ *attr, win_T *wp)
{
    ಪೂರ್ಣಾಂಕ	    fill;
    bool    override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (bt_terminal(wp->w_ಬಫರ್))
    {
	ಆದರೆ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
    ಹಿಂತಿರುಗಿ fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    ಸ್ಥಿರ bool
vsep_ಅಡ್ಡಸಾಲು_is_curwin(win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಆದರೆ (wp == curwin)
	ಹಿಂತಿರುಗಿ true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ಆದರೆ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_ಅಗಲ
	    && ಅಡ್ಡಸಾಲು >= curwin->w_winrow
	    && ಅಡ್ಡಸಾಲು < W_WINROW(curwin) + curwin->w_ಎತ್ತರ
						   + curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ true;

    ಹಿಂತಿರುಗಿ false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    ಪೂರ್ಣಾಂಕ
fillchar_vsep(ಪೂರ್ಣಾಂಕ *attr, win_T *wp, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    bool override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ಆದರೆ (vsep_ಅಡ್ಡಸಾಲು_is_curwin(wp, ಅಡ್ಡಸಾಲು))
	*attr = HL_ATTR(HLF_C);
    ಇಲ್ಲದಿದ್ದರೆ
	*attr = HL_ATTR(HLF_CNC);
    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();

    ಆದರೆ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	ಹಿಂತಿರುಗಿ '|';
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    ಪೂರ್ಣಾಂಕ
redrawing(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (disable_redraw_for_testing)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ((RedrawingDisabled == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#ಆದರೆ_ಅಂತ್ಯ
		) && !(p_lz && char_avail() && !ಕೀಲಿ_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    ಪೂರ್ಣಾಂಕ
messaging(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (!(p_lz && char_avail() && !ಕೀಲಿ_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು ಲಂಬಸಾಲು_RULER 17	    // columns needed by standard ruler

    ಶೂನ್ಯ
comp_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_has_ಸ್ಥಿತಿ = ಕೊನೆಯ_stl_ಎತ್ತರ(FALSE) > 0;

    sc_ಲಂಬಸಾಲು = 0;
    ru_ಲಂಬಸಾಲು = 0;
    ಆದರೆ (p_ru)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	ru_ಲಂಬಸಾಲು = (ru_wid ? ru_wid : ಲಂಬಸಾಲು_RULER) + 1;
#ಇಲ್ಲದಿದ್ದರೆ
	ru_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_RULER + 1;
#ಆದರೆ_ಅಂತ್ಯ
	// no last status line, adjust sc_col
	ಆದರೆ (!ಕೊನೆಯ_has_ಸ್ಥಿತಿ)
	    sc_ಲಂಬಸಾಲು = ru_ಲಂಬಸಾಲು;
    }
    ಆದರೆ (p_sc && *p_sloc == 'l')
    {
	sc_ಲಂಬಸಾಲು += SHOWCMD_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	ಆದರೆ (!p_ru || ಕೊನೆಯ_has_ಸ್ಥಿತಿ)	    // no need for separating space
	    ++sc_ಲಂಬಸಾಲು;
    }
    sc_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - sc_ಲಂಬಸಾಲು;
    ru_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - ru_ಲಂಬಸಾಲು;
    ಆದರೆ (sc_ಲಂಬಸಾಲು <= 0)		// screen too narrow, will become a mess
	sc_ಲಂಬಸಾಲು = 1;
    ಆದರೆ (ru_ಲಂಬಸಾಲು <= 0)
	ru_ಲಂಬಸಾಲು = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_nr(VV_ECHOSPACE, sc_ಲಂಬಸಾಲು - 1);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    ಪೂರ್ಣಾಂಕ
ಸಂಖ್ಯೆ_ಅಗಲ(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		n;
    linenr_T	lnum;

    ಆದರೆ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_ಎತ್ತರ;
    ಇಲ್ಲದಿದ್ದರೆ
	// cursor line shows absolute line number
	lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (lnum == wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ && wp->w_nuw_cached == wp->w_p_nuw)
	ಹಿಂತಿರುಗಿ wp->w_nrwidth_ಅಗಲ;
    wp->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = lnum;

    n = 0;
    ಮಾಡು
    {
	lnum /= 10;
	++n;
    } ಆಗ (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ಆದರೆ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ಆದರೆ (n < 2 && get_ಮೊದಲ_valid_sign(wp) != ಖಾಲಿ
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_ಅಗಲ = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    ಹಿಂತಿರುಗಿ n;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    ಪೂರ್ಣಾಂಕ
screen_screencol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ screen_cur_ಲಂಬಸಾಲು;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    ಪೂರ್ಣಾಂಕ
screen_screenrow(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ screen_cur_ಅಡ್ಡಸಾಲು;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ಆದರೆ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T ಅಂಕೆ = 0;
	ಪೂರ್ಣಾಂಕ	    ಬೈಟುಗಳು;
	ಪೂರ್ಣಾಂಕ	    n;

	ಗಾಗಿ (ಬೈಟುಗಳು = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; ಬೈಟುಗಳು > 0; --ಬೈಟುಗಳು)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ಆದರೆ (n < 0)
		ಹಿಂತಿರುಗಿ 0;
	    ಅಂಕೆ = ಅಂಕೆ * 256 + n;
	}
	*p += 2;
	ಹಿಂತಿರುಗಿ ಅಂಕೆ;
    }
    ಹಿಂತಿರುಗಿ mb_ptr2char_adv(p);
}

ರಚನೆ charstab
{
    ಪೂರ್ಣಾಂಕ	    *cp;
    string_T	name;
};

#ವ್ಯಾಖ್ಯಾನಿಸು CHARSTAB_ನಮೂದು(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

ಸ್ಥಿರ fill_chars_T fill_chars;
ಸ್ಥಿರ ರಚನೆ charstab filltab[] =
{
    CHARSTAB_ನಮೂದು(&fill_chars.stl,	    "stl"),
    CHARSTAB_ನಮೂದು(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_ನಮೂದು(&fill_chars.vert,	    "vert"),
    CHARSTAB_ನಮೂದು(&fill_chars.fold,	    "fold"),
    CHARSTAB_ನಮೂದು(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_ನಮೂದು(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_ನಮೂದು(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_ನಮೂದು(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_ನಮೂದು(&fill_chars.diff,	    "diff"),
    CHARSTAB_ನಮೂದು(&fill_chars.eob,	    "eob"),
    CHARSTAB_ನಮೂದು(&fill_chars.lastline,    "lastline"),
#ಆದರೆ defined(FEAT_TABPANEL)
    CHARSTAB_ನಮೂದು(&fill_chars.tpl_vert,    "tpl_vert"),
#ಆದರೆ_ಅಂತ್ಯ
    CHARSTAB_ನಮೂದು(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_ನಮೂದು(&fill_chars.truncrl,	    "truncrl"),
};
ಸ್ಥಿರ lcs_chars_T lcs_chars;
ಸ್ಥಿರ ರಚನೆ charstab lcstab[] =
{
    CHARSTAB_ನಮೂದು(&lcs_chars.eol,	    "eol"),
    CHARSTAB_ನಮೂದು(&lcs_chars.ext,	    "extends"),
    CHARSTAB_ನಮೂದು(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_ನಮೂದು(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_ನಮೂದು(&lcs_chars.space,	    "space"),
    CHARSTAB_ನಮೂದು(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_ನಮೂದು(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_ನಮೂದು(&lcs_chars.trail,	    "trail"),
    CHARSTAB_ನಮೂದು(&lcs_chars.lead,	    "lead"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    CHARSTAB_ನಮೂದು(&lcs_chars.conceal,	    "conceal"),
#ಇಲ್ಲದಿದ್ದರೆ
    CHARSTAB_ನಮೂದು(ಖಾಲಿ,		    "conceal"),
#ಆದರೆ_ಅಂತ್ಯ
    CHARSTAB_ನಮೂದು(ಖಾಲಿ,		    "multispace"),
    CHARSTAB_ನಮೂದು(ಖಾಲಿ,		    "leadmultispace")
};

    ಸ್ಥಿರ ಅಕ್ಷರ *
ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(ಅಕ್ಷರ *errbuf, size_t errbuflen, ಅಕ್ಷರ *fmt, char_u *ಕ್ಷೇತ್ರ)
{
    ಆದರೆ (errbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ "";
    vim_snprintf(errbuf, errbuflen, _(fmt), ಕ್ಷೇತ್ರ);
    ಹಿಂತಿರುಗಿ errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
ಗಣ_chars_option(win_T *wp, char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ is_listchars, ಪೂರ್ಣಾಂಕ apply,
						ಅಕ್ಷರ *errbuf, size_t errbuflen)
{
    ಪೂರ್ಣಾಂಕ	    ಸುತ್ತು, i, entries;
    char_u  *p, *s;
    ಪೂರ್ಣಾಂಕ	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *ಕೊನೆಯ_multispace = ಖಾಲಿ;  // Last occurrence of "multispace:"
    char_u  *ಕೊನೆಯ_lmultispace = ಖಾಲಿ; // Last occurrence of "leadmultispace:"
    ಪೂರ್ಣಾಂಕ	    multispace_len = 0;	      // Length of lcs-multispace string
    ಪೂರ್ಣಾಂಕ	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    ರಚನೆ charstab *tab;

    ಆದರೆ (is_listchars)
    {
	tab = lcstab;
	CLEAR_ಕ್ಷೇತ್ರ(lcs_chars);
	entries = ARRAY_ಉದ್ದಳತೆ(lcstab);
	ಆದರೆ (wp->w_p_lcs[0] == NUL)
	    ಮೌಲ್ಯ = p_lcs;  // local value is empty, use the global value
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tab = filltab;
	entries = ARRAY_ಉದ್ದಳತೆ(filltab);
	ಆದರೆ (wp->w_p_fcs[0] == NUL)
	    ಮೌಲ್ಯ = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    ಗಾಗಿ (ಸುತ್ತು = 0; ಸುತ್ತು <= (apply ? 1 : 0); ++ಸುತ್ತು)
    {
	ಪೂರ್ಣಾಂಕ has_tab = FALSE, has_leadtab = FALSE;

	ಆದರೆ (ಸುತ್ತು > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ಆದರೆ (is_listchars)
	    {
		ಗಾಗಿ (i = 0; i < entries; ++i)
		    ಆದರೆ (tab[i].cp != ಖಾಲಿ)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ಆದರೆ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, multispace_len + 1);
		    ಆದರೆ (lcs_chars.multispace != ಖಾಲಿ)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    lcs_chars.multispace = ಖಾಲಿ;

		ಆದರೆ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(ಪೂರ್ಣಾಂಕ, lead_multispace_len + 1);
		    ಆದರೆ (lcs_chars.leadmultispace != ಖಾಲಿ)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    lcs_chars.leadmultispace = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
#ಆದರೆ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#ಆದರೆ_ಅಂತ್ಯ
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = ಮೌಲ್ಯ;
	ಆಗ (*p)
	{
	    ಗಾಗಿ (i = 0; i < entries; ++i)
	    {
		ಆದರೆ (!(STRNCMP(p, tab[i].name.string, tab[i].name.ಉದ್ದಳತೆ) == 0 && p[tab[i].name.ಉದ್ದಳತೆ] == ':'))
		    ಮುಂದುವರಿ;

		s = p + tab[i].name.ಉದ್ದಳತೆ + 1;

		ಆದರೆ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ಆದರೆ (ಸುತ್ತು == 0)
		    {
			// Get length of lcs-multispace string in first round
			ಕೊನೆಯ_multispace = p;
			multispace_len = 0;
			ಆಗ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಆದರೆ (char2cells(c1) > 1)
				ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ಆದರೆ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str,
				    tab[i].name.string);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ multispace_pos = 0;

			ಆಗ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಆದರೆ (p == ಕೊನೆಯ_multispace && lcs_chars.multispace != ಖಾಲಿ)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ಆದರೆ (ಸುತ್ತು == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			//ಸುತ್ತು
			ಕೊನೆಯ_lmultispace = p;
			lead_multispace_len = 0;
			ಆಗ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಆದರೆ (char2cells(c1) > 1)
				ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ಆದರೆ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str,
				    tab[i].name.string);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ multispace_pos = 0;

			ಆಗ (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ಆದರೆ (p == ಕೊನೆಯ_lmultispace && lcs_chars.leadmultispace != ಖಾಲಿ)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ನಿಲ್ಲಿಸು;
		}

		c2 = c3 = 0;
		ಆದರೆ (*s == NUL)
		    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ಆದರೆ (char2cells(c1) > 1)
		    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str,
					 tab[i].name.string);
		ಆದರೆ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ಆದರೆ (*s == NUL)
			ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ಆದರೆ (char2cells(c2) > 1)
			ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str,
					 tab[i].name.string);
		    ಆದರೆ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ಆದರೆ (char2cells(c3) > 1)
			    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
					 e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str,
					 tab[i].name.string);
		    }
		    ಆದರೆ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ಆದರೆ (*s == ',' || *s == NUL)
		{
		    ಆದರೆ (ಸುತ್ತು > 0)
		    {
			ಆದರೆ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tab[i].cp != ಖಾಲಿ)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_ಮೌಲ್ಯ_err(errbuf, errbuflen,
				    e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str,
				    tab[i].name.string);
	    }

	    ಆದರೆ (i == entries)
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

	    ಆದರೆ (*p == ',')
		++p;
	}

	ಆದರೆ (is_listchars && has_leadtab && !has_tab)
	    ಹಿಂತಿರುಗಿ e_leadtab_requires_tab;
    }

    ಆದರೆ (apply)
    {
	ಆದರೆ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    ಅಕ್ಷರ *
ಗಣ_fillchars_option(win_T *wp, char_u *val, ಪೂರ್ಣಾಂಕ apply, ಅಕ್ಷರ *errbuf,
							      size_t errbuflen)
{
    ಹಿಂತಿರುಗಿ ಗಣ_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    ಅಕ್ಷರ *
ಗಣ_listchars_option(win_T *wp, char_u *val, ಪೂರ್ಣಾಂಕ apply, ಅಕ್ಷರ *errbuf,
							      size_t errbuflen)
{
    ಹಿಂತಿರುಗಿ ಗಣ_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(filltab))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(lcstab))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    ಅಕ್ಷರ *
check_chars_options(ಶೂನ್ಯ)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ಆದರೆ (ಗಣ_listchars_option(curwin, p_lcs, FALSE, ಖಾಲಿ, 0) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ e_conflicts_with_ಮೌಲ್ಯ_ದ_listchars;
    ಆದರೆ (ಗಣ_fillchars_option(curwin, p_fcs, FALSE, ಖಾಲಿ, 0) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ e_conflicts_with_ಮೌಲ್ಯ_ದ_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಆದರೆ (ಗಣ_listchars_option(wp, wp->w_p_lcs, FALSE, ಖಾಲಿ, 0) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ e_conflicts_with_ಮೌಲ್ಯ_ದ_listchars;
	ಆದರೆ (ಗಣ_fillchars_option(wp, wp->w_p_fcs, FALSE, ಖಾಲಿ, 0) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ e_conflicts_with_ಮೌಲ್ಯ_ದ_fillchars;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
