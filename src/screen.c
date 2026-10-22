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

#అన్తర్భావ "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
స్థిర పూర్ణాంకం	screen_attr = 0;

స్థిర శూన్య screen_char_2(గుర్తులేని off, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు);
స్థిర పూర్ణాంకం screenclear2(పూర్ణాంకం doclear);
స్థిర శూన్య lineclear(గుర్తులేని off, పూర్ణాంకం వెడల్పు, పూర్ణాంకం attr);
స్థిర శూన్య lineinvalid(గుర్తులేని off, పూర్ణాంకం వెడల్పు);
స్థిర పూర్ణాంకం win_do_పంక్తులు(win_T *wp, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం పంక్తి_లెక్క, పూర్ణాంకం mayclear, పూర్ణాంకం del, పూర్ణాంకం clear_attr);
స్థిర శూన్య win_rest_invalid(win_T *wp);
స్థిర శూన్య సందేశం_pos_బహుళకం(శూన్య);
స్థిర శూన్య recording_బహుళకం(పూర్ణాంకం attr);

// Ugly global: overrule attribute used by screen_char()
స్థిర పూర్ణాంకం screen_char_attr = 0;

#ఒకవేళ defined(FEAT_CONCEAL)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    పూర్ణాంకం
conceal_cursor_పంక్తి(win_T *wp)
{
    పూర్ణాంకం		c;

    ఒకవేళ (*wp->w_p_cocu == NUL)
	తిరిగి FALSE;
    ఒకవేళ (get_real_state() & బహుళకం_VISUAL)
	c = 'v';
    లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
	c = 'i';
    లేకపోతే ఒకవేళ (State & బహుళకం_సాధారణ)
	c = 'n';
    లేకపోతే ఒకవేళ (State & బహుళకం_CMDLINE)
	c = 'c';
    లేకపోతే
	తిరిగి FALSE;
    తిరిగి vim_strchr(wp->w_p_cocu, c) != ఖాళీ;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 * To be called after changing the state, "was_concealed" is the value of
 * "conceal_cursor_line()" before the change.
 * "
 */
    శూన్య
conceal_check_cursor_పంక్తి(పూర్ణాంకం was_concealed)
{
    ఒకవేళ (curwin->w_p_cole <= 0 || conceal_cursor_పంక్తి(curwin) == was_concealed)
	తిరిగి;

    పూర్ణాంకం wcol = curwin->w_wcol;

    need_cursor_పంక్తి_redraw = TRUE;
    // Need to recompute cursor column, e.g., when starting Visual mode
    // without concealing.
    curs_columns(TRUE);

    // When concealing now w_wcol will be computed wrong, keep the previous
    // value, it will be updated in win_line().
    ఒకవేళ (!was_concealed)
	curwin->w_wcol = wcol;
}
#అయితే_ముగింపు

/*
 * Get HLF_WIN attribute for window "wp".  If not set and "wp" is a popup window
 * then get the "Pmenu" highlight attribute.
 */
    పూర్ణాంకం
get_win_attr(win_T *wp)
{
    పూర్ణాంకం	    win_attr = wp->w_hlfwin_id;
    bool    override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

    ఒకవేళ (win_attr != 0)
    {
	ఒకవేళ (win_attr != -1)
	    win_attr = syn_id2attr(win_attr);
	లేకపోతే
	    win_attr = 0;
    }
#నిర్వచితమైతే FEAT_PROP_POPUP
    లేకపోతే ఒకవేళ (WIN_IS_POPUP(wp))
    {
	ఒకవేళ (wp->w_popup_flags & POPF_INFO)
	    win_attr = HL_ATTR(HLF_PSI);    // PmenuSel
	లేకపోతే
	    win_attr = HL_ATTR(HLF_తీయి);    // Popup
    }
#అయితే_ముగింపు

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
    తిరిగి win_attr;
}

/*
 * Call screen_fill() with the columns adjusted for 'rightleft' if needed.
 * Return the new offset.
 */
    స్థిర పూర్ణాంకం
screen_fill_చివర(
	win_T *wp,
	పూర్ణాంకం	c1,
	పూర్ణాంకం	c2,
	పూర్ణాంకం	off,
	పూర్ణాంకం	వెడల్పు,
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	endrow,
	పూర్ణాంకం	attr)
{
    పూర్ణాంకం	    nn = off + వెడల్పు;

    ఒకవేళ (nn > wp->w_వెడల్పు)
	nn = wp->w_వెడల్పు;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - nn, (పూర్ణాంకం)W_ENDCOL(wp) - off,
		c1, c2, attr);
    }
    లేకపోతే
#అయితే_ముగింపు
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + endrow,
		wp->w_wincol + off, (పూర్ణాంకం)wp->w_wincol + nn,
		c1, c2, attr);
    తిరిగి nn;
}

/*
 * Clear lines near the end the window and mark the unused lines with "c1".
 * use "c2" as the filler character.
 * When "draw_margin" is TRUE then draw the sign, fold and number columns.
 */
    శూన్య
win_draw_చివర(
    win_T	*wp,
    పూర్ణాంకం		c1,
    పూర్ణాంకం		c2,
    పూర్ణాంకం		draw_margin,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		endrow,
    hlf_T	hl)
{
    పూర్ణాంకం		n = 0;
    పూర్ణాంకం		attr = HL_ATTR(hl);
    పూర్ణాంకం		win_attr = get_win_attr(wp);
    bool	override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

    attr = hl_combine_attr(win_attr, attr);

    ఒకవేళ (draw_margin)
    {
#నిర్వచితమైతే FEAT_FOLDING
	పూర్ణాంకం	fdc = compute_foldcolumn(wp, 0);

	ఒకవేళ (fdc > 0)
	    // draw the fold column
	    n = screen_fill_చివర(wp, ' ', ' ', n, fdc,
		      అడ్డువరుస, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_FC)));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	ఒకవేళ (signcolumn_on(wp))
	    // draw the sign column
	    n = screen_fill_చివర(wp, ' ', ' ', n, 2,
		      అడ్డువరుస, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_SC)));
#అయితే_ముగింపు
	ఒకవేళ ((wp->w_p_nu || wp->w_p_rnu)
				  && vim_strchr(p_cpo, CPO_NUMCOL) == ఖాళీ)
	    // draw the number column
	    n = screen_fill_చివర(wp, ' ', ' ', n, సంఖ్య_వెడల్పు(wp) + 1,
		       అడ్డువరుస, endrow, hl_combine_attr(win_attr, HL_ATTR(HLF_N)));
    }

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + endrow,
		wp->w_wincol, W_ENDCOL(wp) - 1 - n, c2, c2, attr);
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - n, W_ENDCOL(wp) - n, c1, c2, attr);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + endrow,
		wp->w_wincol + n, (పూర్ణాంకం)W_ENDCOL(wp), c1, c2, attr);
    }

    సమితి_ఖాళీలు_అడ్డువరుసలు(wp, అడ్డువరుస);

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    పూర్ణాంకం
compute_foldcolumn(win_T *wp, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    పూర్ణాంకం n = wp->w_వెడల్పు - (నిలువు + wmw);

    తిరిగి MIN(wp->w_p_fdc, n);
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
    పూర్ణాంకం		closed,		// TRUE of FALSE
    linenr_T	lnum)		// current line number
{
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		level;
    పూర్ణాంకం		మొదటి_level;
    పూర్ణాంకం		ఖాళీలు_2;
    పూర్ణాంకం		fdc = compute_foldcolumn(wp, 0);
    size_t	byte_counter = 0;
    పూర్ణాంకం		చిహ్నం = 0;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		n;

    // Init to all spaces.
    vim_memset(p, ' ', MAX_MCO * fdc + 1);

    level = win_foldinfo.fi_level;
    ఖాళీలు_2 = (fdc == 1) ? 0 : 1;

    // If the column is too narrow, we start at the lowest level that
    // fits and use numbers to indicate the depth.
    మొదటి_level = level - fdc - closed + 1 + ఖాళీలు_2;
    ఒకవేళ (మొదటి_level < 1)
	మొదటి_level = 1;

    n = MIN(fdc, level);		// evaluate this once
    కొరకు (i = 0; i < n; i++)
    {
	ఒకవేళ (win_foldinfo.fi_lnum == lnum
		&& మొదటి_level + i >= win_foldinfo.fi_దిగువ_level)
	    చిహ్నం = wp->w_fill_chars.foldopen;
	లేకపోతే ఒకవేళ (మొదటి_level == 1)
	    చిహ్నం = wp->w_fill_chars.foldsep;
	లేకపోతే ఒకవేళ (wp->w_fill_chars.foldinner != NUL)
	    చిహ్నం = wp->w_fill_chars.foldinner;
	లేకపోతే ఒకవేళ (మొదటి_level + i <= 9)
	    చిహ్నం = '0' + మొదటి_level + i;
	లేకపోతే
	    చిహ్నం = '>';

	len = utf_char2bytes(చిహ్నం, &p[byte_counter]);
	byte_counter += len;
	ఒకవేళ (మొదటి_level + i >= level)
	{
	    i++;
	    ఆపు;
	}
    }

    ఒకవేళ (closed)
    {
	ఒకవేళ (చిహ్నం != 0)
	{
	    // rollback length and the character
	    byte_counter -= len;
	    ఒకవేళ (len > 1)
		// for a multibyte character, erase all the bytes
		vim_memset(p + byte_counter, ' ', len);
	}
	చిహ్నం = wp->w_fill_chars.foldclosed;
	len = utf_char2bytes(చిహ్నం, &p[byte_counter]);
	byte_counter += len;
    }

    తిరిగి MAX(byte_counter + (fdc - i), (size_t)fdc);
}
#అయితే_ముగింపు // FEAT_FOLDING

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    స్థిర పూర్ణాంకం
comp_char_differs(పూర్ణాంకం off_from, పూర్ణాంకం off_గమ్యం)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < Screen_mco; ++i)
    {
	ఒకవేళ (Screen_పంక్తులు_C[i][off_from] != Screen_పంక్తులు_C[i][off_గమ్యం])
	    తిరిగి TRUE;
	ఒకవేళ (Screen_పంక్తులు_C[i][off_from] == 0)
	    ఆపు;
    }
    తిరిగి FALSE;
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    స్థిర పూర్ణాంకం
char_needs_redraw(పూర్ణాంకం off_from, పూర్ణాంకం off_గమ్యం, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    ఒకవేళ (నిలువుల_సంఖ్య > 0
	    && ((Screen_పంక్తులు[off_from] != Screen_పంక్తులు[off_గమ్యం]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_గమ్యం])
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(Screen_పంక్తులు[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && Screen_పంక్తులు[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_గమ్యం]
			: (నిలువుల_సంఖ్య > 1 && Screen_పంక్తులు[off_from + 1]
						 != Screen_పంక్తులు[off_గమ్యం + 1])))
		|| (enc_utf8
		    && (Screen_పంక్తులు_UC[off_from] != Screen_పంక్తులు_UC[off_గమ్యం]
			|| (Screen_పంక్తులు_UC[off_from] != 0
			    && comp_char_differs(off_from, off_గమ్యం))
			|| ((*mb_off2cells)(off_from, off_from + నిలువుల_సంఖ్య) > 1
			    && Screen_పంక్తులు[off_from + 1]
						!= Screen_పంక్తులు[off_గమ్యం + 1])))))
	తిరిగి TRUE;
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Return the index in ScreenLines[] for the current screen line.
 */
    పూర్ణాంకం
screen_get_current_పంక్తి_off(శూన్య)
{
    తిరిగి (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
/*
 * Return TRUE if this position has a higher level popup or this cell is
 * transparent in the current popup.
 */
    స్థిర పూర్ణాంకం
blocked_by_popup(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం off;

    ఒకవేళ (!popup_visible)
	తిరిగి FALSE;
    off = అడ్డువరుస * screen_Columns + నిలువు;
    తిరిగి popup_ముసుగు[off] > screen_zindex || popup_transparent[off];
}
#అయితే_ముగింపు

/*
 * Reset the highlighting.  Used before clearing the screen.
 */
    శూన్య
reset_screen_attr(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	// Use a code that will reset gui.highlight_mask in
	// gui_stop_highlight().
	screen_attr = HL_ALL + 1;
    లేకపోతే
#అయితే_ముగింపు
	// Use attributes that is very unlikely to appear in text.
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE | HL_STRIKETHROUGH;
}

/*
 * Return TRUE if the character at "row" / "col" is under the popup menu and it
 * will be redrawn soon or it is under another popup.
 */
    స్థిర పూర్ణాంకం
skip_for_popup(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    // Popup windows with zindex higher than POPUPMENU_ZINDEX go on top.
    ఒకవేళ (pum_under_menu(అడ్డువరుస, నిలువు, TRUE)
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && screen_zindex <= POPUPMENU_ZINDEX
#అయితే_ముగింపు
	    )
	తిరిగి TRUE;
    // Protect cells under the pum from background draws (vsep, status line).
    // Excluded for wildmenu pum (MODE_CMDLINE): pum_row can be stale while
    // the cmdline grows.
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (screen_zindex < POPUPMENU_ZINDEX
	    && pum_visible()
	    && (State & బహుళకం_CMDLINE) == 0
	    && pum_under_menu(అడ్డువరుస, నిలువు, FALSE))
	తిరిగి TRUE;
#లేకపోతే
    ఒకవేళ (pum_visible()
	    && (State & బహుళకం_CMDLINE) == 0
	    && pum_under_menu(అడ్డువరుస, నిలువు, FALSE))
	తిరిగి TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (blocked_by_popup(అడ్డువరుస, నిలువు))
	తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి FALSE;
}

#నిర్వచితమైతే FEAT_PROP_POPUP
// Cells where ScreenLines[] was updated but terminal output was suppressed
// because the cell was under an opacity popup.  For these cells the terminal
// no longer matches ScreenLines[], so the "cell unchanged, skip output"
// optimization must not be applied until they are output again.
స్థిర char_u	*suppressed_cells = ఖాళీ;
స్థిర పూర్ణాంకం	suppressed_అడ్డువరుసలు = 0;
స్థిర పూర్ణాంకం	suppressed_నిలువుల_సంఖ్య = 0;

    స్థిర శూన్య
mark_suppressed_cell(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (suppressed_cells == ఖాళీ
	    || suppressed_అడ్డువరుసలు != screen_అడ్డువరుసలు
	    || suppressed_నిలువుల_సంఖ్య != screen_Columns)
    {
	vim_free(suppressed_cells);
	suppressed_cells = alloc_clear(
				   (size_t)screen_అడ్డువరుసలు * screen_Columns);
	ఒకవేళ (suppressed_cells == ఖాళీ)
	{
	    suppressed_అడ్డువరుసలు = 0;
	    suppressed_నిలువుల_సంఖ్య = 0;
	    తిరిగి;
	}
	suppressed_అడ్డువరుసలు = screen_అడ్డువరుసలు;
	suppressed_నిలువుల_సంఖ్య = screen_Columns;
    }
    suppressed_cells[అడ్డువరుస * suppressed_నిలువుల_సంఖ్య + నిలువు] = TRUE;
}

    స్థిర శూన్య
unmark_suppressed_cell(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (suppressed_cells != ఖాళీ
	    && అడ్డువరుస >= 0 && అడ్డువరుస < suppressed_అడ్డువరుసలు
	    && నిలువు >= 0 && నిలువు < suppressed_నిలువుల_సంఖ్య)
	suppressed_cells[అడ్డువరుస * suppressed_నిలువుల_సంఖ్య + నిలువు] = FALSE;
}

    స్థిర పూర్ణాంకం
is_suppressed_cell(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    తిరిగి suppressed_cells != ఖాళీ
	&& అడ్డువరుస >= 0 && అడ్డువరుస < suppressed_అడ్డువరుసలు
	&& నిలువు >= 0 && నిలువు < suppressed_నిలువుల_సంఖ్య
	&& suppressed_cells[అడ్డువరుస * suppressed_నిలువుల_సంఖ్య + నిలువు];
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
/*
 * Cached attributes of the current opacity popup, computed once per
 * screen_line() to avoid recomputing them inside the per-cell loop.
 */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		popup_attr;	// get_win_attr(screen_opacity_popup)
    పూర్ణాంకం		blend;		// screen_opacity_popup->w_popup_blend
} popup_opacity_T;

    స్థిర అంతఃవరుస శూన్య
popup_opacity_init(popup_opacity_T *po)
{
    po->popup_attr = get_win_attr(screen_opacity_popup);
    po->blend = screen_opacity_popup->w_popup_blend;
}

/*
 * Read the underlying base screen attribute at (row, col), falling back
 * to "fallback" when the position is outside base_screenattrs.
 */
    స్థిర అంతఃవరుస పూర్ణాంకం
popup_పునాది_attr_or(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం fallback)
{
    పూర్ణాంకం		పునాది = fallback;

    popup_get_పునాది_screen_cell(అడ్డువరుస, నిలువు, ఖాళీ, &పునాది, ఖాళీ);
    తిరిగి పునాది;
}

/*
 * Blend "char_attr" through the cached opacity popup over "base_attr".
 * "blend_fg" is TRUE when both fg and bg should blend (space cells), FALSE
 * when only bg should blend so the popup fg is preserved (text cells).
 */
    స్థిర అంతఃవరుస పూర్ణాంకం
popup_blend_with_పునాది(
	స్థిరరాశి popup_opacity_T	*po,
	పూర్ణాంకం			char_attr,
	పూర్ణాంకం			పునాది_attr,
	పూర్ణాంకం			blend_fg)
{
    పూర్ణాంకం		combined = hl_combine_attr(po->popup_attr, char_attr);

    తిరిగి hl_blend_attr(పునాది_attr, combined, po->blend, blend_fg);
}

/*
 * For a double-wide character at a popup boundary with opacity:0
 * (blend==100), the two cells may have different underlying attrs.
 * Pick the one without a background color to prevent color leaking.
 */
    స్థిర శూన్య
resolve_wide_char_opacity_attrs(
	పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం col1, పూర్ణాంకం col2,
	sattr_T *attr1, sattr_T *attr2)
{
    పూర్ణాంకం		bg1, bg2;
    పూర్ణాంకం		base1 = 0;
    పూర్ణాంకం		base2 = 0;
    attrentry_T	*ae;

    ఒకవేళ (*attr1 == *attr2)
	తిరిగి;

    popup_get_పునాది_screen_cell(అడ్డువరుస, col1, ఖాళీ, &base1, ఖాళీ);
    ae = syn_cterm_attr2entry(base1);
# ifdef FEAT_TERMGUICOLORS
    bg1 = (ae != ఖాళీ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# లేకపోతే
    bg1 = (ae != ఖాళీ && ae->ae_u.cterm.bg_color != 0);
# endif

    popup_get_పునాది_screen_cell(అడ్డువరుస, col2, ఖాళీ, &base2, ఖాళీ);
    ae = syn_cterm_attr2entry(base2);
# ifdef FEAT_TERMGUICOLORS
    bg2 = (ae != ఖాళీ && !COLOR_INVALID(ae->ae_u.cterm.bg_rgb));
# లేకపోతే
    bg2 = (ae != ఖాళీ && ae->ae_u.cterm.bg_color != 0);
# endif

    ఒకవేళ (bg1 && !bg2)
	*attr1 = *attr2;
    లేకపోతే ఒకవేళ (!bg1 && bg2)
	*attr2 = *attr1;
}
#అయితే_ముగింపు

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
    శూన్య
screen_పంక్తి(
	win_T	*wp,
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	coloff,
	పూర్ణాంకం	endcol,
	పూర్ణాంకం	clear_వెడల్పు,
	colnr_T	చివరి_vcol,
	పూర్ణాంకం	flags వాడని)
{
    గుర్తులేని	    off_from;
    గుర్తులేని	    off_గమ్యం;
    గుర్తులేని	    max_off_from;
    గుర్తులేని	    max_off_గమ్యం;
    పూర్ణాంకం		    నిలువు = 0;
    పూర్ణాంకం		    hl;
    పూర్ణాంకం		    force = FALSE;	// force update rest of the line
    పూర్ణాంకం		    redraw_this		// bool: does character need redraw?
#నిర్వచితమైతే FEAT_GUI
				= TRUE	// For GUI when while-loop empty
#అయితే_ముగింపు
				;
    పూర్ణాంకం		    redraw_తదుపరి;	// redraw_this for next character
#నిర్వచితమైతే FEAT_GUI_MSWIN
    పూర్ణాంకం		    మారింది_this;	// TRUE if character changed
    పూర్ణాంకం		    మారింది_తదుపరి;	// TRUE if next character changed
#అయితే_ముగింపు
    పూర్ణాంకం		    clear_తదుపరి = FALSE;
    పూర్ణాంకం		    char_cells;		// 1: normal char
					// 2: occupies two display cells
    bool	    override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);
#నిర్వచితమైతే FEAT_PROP_POPUP
    popup_opacity_T po;			// cached when drawing an opacity popup
    పూర్ణాంకం		    drawing_opacity_popup =
				screen_opacity_popup != ఖాళీ
				&& (flags & SLF_POPUP);
    CLEAR_క్షేత్రం(po);
    ఒకవేళ (drawing_opacity_popup)
	popup_opacity_init(&po);
#అయితే_ముగింపు

    // Check for illegal row and col, just in case.
    ఒకవేళ (అడ్డువరుస >= అడ్డువరుసలు)
	అడ్డువరుస = అడ్డువరుసలు - 1;
    ఒకవేళ (endcol > Columns)
	endcol = Columns;

#నిర్వచితమైతే FEAT_CLIPBOARD
    clip_may_clear_selection(అడ్డువరుస, అడ్డువరుస);
#అయితే_ముగింపు

    off_from = (గుర్తులేని)(current_Screen_పంక్తి - Screen_పంక్తులు);
    off_గమ్యం = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + coloff;
    max_off_from = off_from + screen_Columns;
    max_off_గమ్యం = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + screen_Columns;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (flags & SLF_RIGHTLEFT)
    {
	// Clear rest first, because it's left of the text.
	ఒకవేళ (clear_వెడల్పు > 0)
	{
	    పూర్ణాంకం clear_ప్రారంభం = నిలువు;

	    అయితే (నిలువు <= endcol && Screen_పంక్తులు[off_గమ్యం] == ' '
		    && ScreenAttrs[off_గమ్యం] == 0
				  && (!enc_utf8 || Screen_పంక్తులు_UC[off_గమ్యం] == 0))
	    {
		++off_గమ్యం;
		++నిలువు;
	    }
	    ఒకవేళ (నిలువు <= endcol)
		screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);

	    కొరకు (పూర్ణాంకం i = endcol; i >= clear_ప్రారంభం; i--)
		Screen_నిలువుల_సంఖ్య[off_గమ్యం + (i - నిలువు)] =
		    (flags & SLF_INC_VCOL) ? ++చివరి_vcol : చివరి_vcol;
	}
	నిలువు = endcol + 1;
	off_గమ్యం = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు + coloff;
	off_from += నిలువు;
	endcol = (clear_వెడల్పు > 0 ? clear_వెడల్పు : -clear_వెడల్పు);
    }
#అయితే_ముగింపు // FEAT_RIGHTLEFT

#నిర్వచితమైతే FEAT_PROP_POPUP
    // First char of a popup window may go on top of the right half of a
    // double-wide character. Clear the left half to avoid it getting the popup
    // window background color.
    ఒకవేళ (coloff > 0 && enc_utf8
		   && Screen_పంక్తులు[off_గమ్యం] == 0
		   && Screen_పంక్తులు_UC[off_గమ్యం - 1] != 0
		   && (*mb_char2cells)(Screen_పంక్తులు_UC[off_గమ్యం - 1]) > 1)
    {
	Screen_పంక్తులు[off_గమ్యం - 1] = ' ';
	Screen_పంక్తులు_UC[off_గమ్యం - 1] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background draw already output this cell, and outputting
	// a space here would briefly erase it causing flicker.
# ifdef FEAT_PROP_POPUP
	ఒకవేళ (screen_opacity_popup == ఖాళీ)
# endif
	    screen_char(off_గమ్యం - 1, అడ్డువరుస, నిలువు + coloff - 1);
    }
#అయితే_ముగింపు

    redraw_తదుపరి = char_needs_redraw(off_from, off_గమ్యం, endcol - నిలువు);
#నిర్వచితమైతే FEAT_GUI_MSWIN
    మారింది_తదుపరి = redraw_తదుపరి;
#అయితే_ముగింపు

    అయితే (నిలువు < endcol)
    {
	ఒకవేళ (has_mbyte && (నిలువు + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	లేకపోతే
	    char_cells = 1;

	redraw_this = redraw_తదుపరి;
	redraw_తదుపరి = force || char_needs_redraw(off_from + char_cells,
			      off_గమ్యం + char_cells, endcol - నిలువు - char_cells);

#నిర్వచితమైతే FEAT_PROP_POPUP
	// A cell whose output was suppressed under an opacity popup does not
	// match the terminal even when ScreenLines[] is unchanged.
	ఒకవేళ (!redraw_this && is_suppressed_cell(అడ్డువరుస, నిలువు + coloff))
	    redraw_this = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
# ifdef FEAT_GUI_MSWIN
	మారింది_this = మారింది_తదుపరి;
	మారింది_తదుపరి = redraw_తదుపరి;
# endif
	// If the next character was bold, then redraw the current character to
	// remove any pixels that might have spilt over into us.  This only
	// happens in the GUI.
	// With MS-Windows antialiasing may also cause pixels to spill over
	// from a previous character, no matter attributes, always redraw if a
	// character changed.
	ఒకవేళ (redraw_తదుపరి && gui.లోపల_use)
	{
# ifndef FEAT_GUI_MSWIN
	    hl = ScreenAttrs[off_గమ్యం + char_cells];
	    ఒకవేళ (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    ఒకవేళ (hl & HL_BOLD)
# endif
		redraw_this = TRUE;
	}
#అయితే_ముగింపు
	// skip the second cell for double-width characters.
	ఒకవేళ (redraw_this && char_cells == 2 && skip_for_popup(అడ్డువరుస, నిలువు + coloff + 1))
	    redraw_this = FALSE;
	// Check if the character is occluded by a popup.
	ఒకవేళ (redraw_this && skip_for_popup(అడ్డువరుస, నిలువు + coloff))
	    redraw_this = FALSE;

#నిర్వచితమైతే FEAT_PROP_POPUP
	// For popup with opacity windows: if drawing a space, show the
	// underlying character with the popup's attributes blended in.
	పూర్ణాంకం opacity_blank = FALSE;
	// Popup with opacity: when drawing a space, blend the popup attrs
	// onto whatever is underneath instead of overwriting it.
	ఒకవేళ (drawing_opacity_popup
		&& Screen_పంక్తులు[off_from] == ' '
		&& (!enc_utf8 || Screen_పంక్తులు_UC[off_from] == 0))
	{
	    పూర్ణాంకం popup_src_attr = ScreenAttrs[off_from];
	    పూర్ణాంకం bg_is_ఖాళీలు = Screen_పంక్తులు[off_గమ్యం] == 0
				&& (!enc_utf8 || Screen_పంక్తులు_UC[off_గమ్యం] == 0);

	    // Case A: target cell is the second half of a wide char from a
	    // previous draw.  Don't overwrite it.
	    ఒకవేళ (bg_is_ఖాళీలు
		    && off_గమ్యం > 0
		    && enc_utf8 && Screen_పంక్తులు_UC[off_గమ్యం - 1] != 0
		    && utf_char2cells(Screen_పంక్తులు_UC[off_గమ్యం - 1]) == 2)
	    {
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	    // Case B: target cell holds a real character — blend popup attrs
	    // over it, preserving the underlying glyph.
	    లేకపోతే ఒకవేళ (Screen_పంక్తులు[off_గమ్యం] != 0
		    || (enc_utf8 && Screen_పంక్తులు_UC[off_గమ్యం] != 0))
	    {
		పూర్ణాంకం bg_char_cells = (enc_utf8 && Screen_పంక్తులు_UC[off_గమ్యం] != 0)
				? utf_char2cells(Screen_పంక్తులు_UC[off_గమ్యం]) : 1;

		ఒకవేళ (bg_char_cells == 2)
		{
		    ఒకవేళ (నిలువు + 1 >= endcol || off_from + 1 >= max_off_from
						       || off_గమ్యం + 1 >= max_off_గమ్యం)
		    {
			// Wide char doesn't fit at the edge.  Replace with a
			// blended space so opacity is still applied.
			పూర్ణాంకం పునాది_attr;
			Screen_పంక్తులు[off_గమ్యం] = ' ';
			ఒకవేళ (enc_utf8)
			    Screen_పంక్తులు_UC[off_గమ్యం] = 0;
			పునాది_attr = popup_పునాది_attr_or(అడ్డువరుస, నిలువు + coloff,
							     ScreenAttrs[off_గమ్యం]);
			ScreenAttrs[off_గమ్యం] = popup_blend_with_పునాది(&po,
					    popup_src_attr, పునాది_attr, TRUE);
			screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
			opacity_blank = TRUE;
			redraw_this = FALSE;
			వెళ్ళు skip_opacity;
		    }
		    // Wide bg char must be followed by a popup space, otherwise
		    // a popup glyph would corrupt its right half.
		    ఒకవేళ (!(Screen_పంక్తులు[off_from + 1] == ' '
			    && (!enc_utf8 || Screen_పంక్తులు_UC[off_from + 1] == 0)))
			వెళ్ళు skip_opacity;
		}

		// Keep the underlying character and blend the popup attrs over
		// it.  blend_fg=TRUE because we're drawing a space, so the
		// foreground (the underlying glyph) should also blend.
		opacity_blank = TRUE;
		పూర్ణాంకం పునాది_attr = popup_పునాది_attr_or(అడ్డువరుస, నిలువు + coloff,
							     ScreenAttrs[off_గమ్యం]);
		ScreenAttrs[off_గమ్యం] = popup_blend_with_పునాది(&po,
					    popup_src_attr, పునాది_attr, TRUE);
		screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
		// For wide bg char also blend the second cell; its base may be
		// outside the popup area.
		ఒకవేళ (bg_char_cells == 2)
		{
		    పూర్ణాంకం పునాది_attr2 = popup_పునాది_attr_or(అడ్డువరుస, నిలువు + coloff + 1,
							 ScreenAttrs[off_గమ్యం + 1]);
		    ScreenAttrs[off_గమ్యం + 1] = popup_blend_with_పునాది(&po,
					    popup_src_attr, పునాది_attr2, TRUE);
		    ఒకవేళ (po.blend == 100)
			resolve_wide_char_opacity_attrs(అడ్డువరుస,
				నిలువు + coloff, నిలువు + coloff + 1,
				&ScreenAttrs[off_గమ్యం], &ScreenAttrs[off_గమ్యం + 1]);
		}
		redraw_this = FALSE;
	    }
	    // Case C: popup space overlaps the second half of a destroyed wide
	    // character.  Blend so the cell matches its neighbors instead of
	    // appearing as a solid-colored gap.
	    లేకపోతే ఒకవేళ (bg_is_ఖాళీలు)
	    {
		పూర్ణాంకం పునాది_attr;
		Screen_పంక్తులు[off_గమ్యం] = ' ';
		పునాది_attr = popup_పునాది_attr_or(అడ్డువరుస, నిలువు + coloff,
							     ScreenAttrs[off_గమ్యం]);
		ScreenAttrs[off_గమ్యం] = popup_blend_with_పునాది(&po,
					    popup_src_attr, పునాది_attr, TRUE);
		screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
		opacity_blank = TRUE;
		redraw_this = FALSE;
	    }
	}
skip_opacity:
#అయితే_ముగింపు

	ఒకవేళ (redraw_this)
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
	    ఒకవేళ (       p_wiv
		    && !force
#నిర్వచితమైతే FEAT_GUI
		    && !gui.లోపల_use
#అయితే_ముగింపు
		    && ScreenAttrs[off_గమ్యం] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_గమ్యం])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(అడ్డువరుస, నిలువు + coloff);
		బహిర్గతం_str(T_CE);		// clear rest of this screen line
		screen_ప్రారంభం();		// don't know where cursor is now
		force = TRUE;		// force redraw of rest of the line
		redraw_తదుపరి = TRUE;	// or else next char would miss out

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		ఒకవేళ (నిలువు + coloff > 0 && ScreenAttrs[off_గమ్యం - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_గమ్యం - 1];
		    పదం_windgoto(అడ్డువరుస, నిలువు + coloff);
		    screen_stop_highlight();
		}
		లేకపోతే
		    screen_attr = 0;	    // highlighting has stopped
	    }
	    ఒకవేళ (enc_dbcs != 0)
	    {
		// Check if overwriting a double-byte with a single-byte or
		// the other way around requires another character to be
		// redrawn.  For UTF-8 this isn't needed, because comparing
		// ScreenLinesUC[] is sufficient.
		ఒకవేళ (char_cells == 1
			&& నిలువు + 1 < endcol
			&& (*mb_off2cells)(off_గమ్యం, max_off_గమ్యం) > 1)
		{
		    // Writing a single-cell character over a double-cell
		    // character: need to redraw the next cell.
		    Screen_పంక్తులు[off_గమ్యం + 1] = 0;
		    redraw_తదుపరి = TRUE;
		}
		లేకపోతే ఒకవేళ (char_cells == 2
			&& నిలువు + 2 < endcol
			&& (*mb_off2cells)(off_గమ్యం, max_off_గమ్యం) == 1
			&& (*mb_off2cells)(off_గమ్యం + 1, max_off_గమ్యం) > 1)
		{
		    // Writing the second half of a double-cell character over
		    // a double-cell character: need to redraw the second
		    // cell.
		    Screen_పంక్తులు[off_గమ్యం + 2] = 0;
		    redraw_తదుపరి = TRUE;
		}

		ఒకవేళ (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_గమ్యం] = ScreenLines2[off_from];
	    }
	    // When writing a single-width character over a double-width
	    // character and at the end of the redrawn text, need to clear out
	    // the right half of the old character.
	    // Also required when writing the right half of a double-width
	    // char over the left half of an existing one.
	    ఒకవేళ (has_mbyte && నిలువు + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_గమ్యం, max_off_గమ్యం) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_గమ్యం, max_off_గమ్యం) == 1
			    && (*mb_off2cells)(off_గమ్యం + 1, max_off_గమ్యం) > 1)))
		clear_తదుపరి = TRUE;

	    Screen_పంక్తులు[off_గమ్యం] = Screen_పంక్తులు[off_from];
	    ఒకవేళ (enc_utf8)
	    {
		Screen_పంక్తులు_UC[off_గమ్యం] = Screen_పంక్తులు_UC[off_from];
		ఒకవేళ (Screen_పంక్తులు_UC[off_from] != 0)
		{
		    పూర్ణాంకం	    i;

		    కొరకు (i = 0; i < Screen_mco; ++i)
			Screen_పంక్తులు_C[i][off_గమ్యం] = Screen_పంక్తులు_C[i][off_from];
		}
	    }
	    ఒకవేళ (char_cells == 2)
	    {
		Screen_పంక్తులు[off_గమ్యం + 1] = Screen_పంక్తులు[off_from + 1];
		ఒకవేళ (enc_utf8)
		    Screen_పంక్తులు_UC[off_గమ్యం + 1] = 0;
	    }

#ఒకవేళ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single column of pixels appear in the
	    // next character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ఒకవేళ (
# ifdef FEAT_GUI
		    gui.లోపల_use
# endif
# ఒకవేళ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    పదం_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_గమ్యం];
		ఒకవేళ (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		ఒకవేళ (hl & HL_BOLD)
		    redraw_తదుపరి = TRUE;
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
	    // MS-Windows antialiasing may spill over to the next character,
	    // redraw that one if this one changed, no matter attributes.
	    ఒకవేళ (gui.లోపల_use && మారింది_this)
		redraw_తదుపరి = TRUE;
# ifdef FEAT_DIRECTX
	    // DirectWrite subpixel rendering (especially with CFF/OTF
	    // fonts) can extend pixels beyond cell boundaries to the
	    // left.  Redraw the current character if the previous one
	    // changed.
	    ఒకవేళ (gui.directx_enabled && మారింది_this)
		redraw_this = TRUE;
# endif
#అయితే_ముగింపు

	    ScreenAttrs[off_గమ్యం] = ScreenAttrs[off_from];
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // For popup with opacity text: blend popup attrs over the underlying
	    // base.  blend_fg=FALSE so the popup foreground (the real glyph) is
	    // preserved while only the background blends.
	    ఒకవేళ (drawing_opacity_popup && po.blend > 0)
	    {
		పూర్ణాంకం popup_src_attr = ScreenAttrs[off_from];
		పూర్ణాంకం scol1 = నిలువు + coloff;
		పూర్ణాంకం base1 = popup_పునాది_attr_or(అడ్డువరుస, scol1, 0);

		ఒకవేళ (char_cells == 2)
		{
		    పూర్ణాంకం base2 = popup_పునాది_attr_or(అడ్డువరుస, scol1 + 1, 0);

		    // Terminals can't render different bg colors for the two
		    // halves of a wide char.  If one half is over a lower
		    // opacity popup and the other isn't, use the non-popup
		    // side for both halves to avoid color leaking.
		    పూర్ణాంకం over1 = popup_is_over_opacity(అడ్డువరుస, scol1);
		    పూర్ణాంకం over2 = popup_is_over_opacity(అడ్డువరుస, scol1 + 1);
		    ఒకవేళ (over1 != over2)
		    {
			ఒకవేళ (over1)
			    base1 = base2;
			లేకపోతే
			    base2 = base1;
		    }
		    ScreenAttrs[off_గమ్యం + 1] = popup_blend_with_పునాది(&po,
					    popup_src_attr, base2, FALSE);
		}
		ScreenAttrs[off_గమ్యం] = popup_blend_with_పునాది(&po,
					    popup_src_attr, base1, FALSE);
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    // For simplicity set the attributes of second half of a
	    // double-wide character equal to the first half.
	    ఒకవేళ (char_cells == 2)
		ScreenAttrs[off_గమ్యం + 1] = ScreenAttrs[off_గమ్యం];

	    ఒకవేళ (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
	    లేకపోతే
		screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
	}
	లేకపోతే ఒకవేళ (  p_wiv
#నిర్వచితమైతే FEAT_GUI
		&& !gui.లోపల_use
#అయితే_ముగింపు
		&& నిలువు + coloff > 0 && off_గమ్యం > 0)
	{
	    ఒకవేళ (ScreenAttrs[off_గమ్యం] == ScreenAttrs[off_గమ్యం - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    లేకపోతే ఒకవేళ (screen_attr != 0)
		screen_stop_highlight();
	}

#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (!opacity_blank)
#అయితే_ముగింపు
	{
	    Screen_నిలువుల_సంఖ్య[off_గమ్యం] = Screen_నిలువుల_సంఖ్య[off_from];
	    ఒకవేళ (char_cells == 2)
		Screen_నిలువుల_సంఖ్య[off_గమ్యం + 1] = Screen_నిలువుల_సంఖ్య[off_from + 1];
	}

	off_గమ్యం += char_cells;
	off_from += char_cells;
	నిలువు += char_cells;
    }

    ఒకవేళ (clear_తదుపరి && !skip_for_popup(అడ్డువరుస, నిలువు + coloff))
    {
	// Clear the second half of a double-wide character of which the left
	// half was overwritten with a single-wide character.
	Screen_పంక్తులు[off_గమ్యం] = ' ';
	ఒకవేళ (enc_utf8)
	    Screen_పంక్తులు_UC[off_గమ్యం] = 0;
	// Skip screen output when drawing an opacity popup: the
	// background already has this cell, outputting a space here
	// would briefly erase it causing flicker.
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (screen_opacity_popup == ఖాళీ)
#అయితే_ముగింపు
	    screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
    }

    ఒకవేళ (clear_వెడల్పు > 0
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    && !(flags & SLF_RIGHTLEFT)
#అయితే_ముగింపు
				   )
    {
#నిర్వచితమైతే FEAT_GUI
	పూర్ణాంకం ప్రారంభం_నిలువు_2 = నిలువు;
#అయితే_ముగింపు

	// blank out the rest of the line
	అయితే (నిలువు < clear_వెడల్పు && Screen_పంక్తులు[off_గమ్యం] == ' '
						  && ScreenAttrs[off_గమ్యం] == 0
				  && (!enc_utf8 || Screen_పంక్తులు_UC[off_గమ్యం] == 0)
#నిర్వచితమైతే FEAT_PROP_POPUP
				  && !is_suppressed_cell(అడ్డువరుస, నిలువు + coloff)
#అయితే_ముగింపు
				  )
	{
	    Screen_నిలువుల_సంఖ్య[off_గమ్యం] =
			      (flags & SLF_INC_VCOL) ? ++చివరి_vcol : చివరి_vcol;
	    ++off_గమ్యం;
	    ++నిలువు;
	}
	ఒకవేళ (నిలువు < clear_వెడల్పు)
	{
#నిర్వచితమైతే FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    ఒకవేళ (gui.లోపల_use && (నిలువు > ప్రారంభం_నిలువు_2 || !redraw_this))
	    {
		hl = ScreenAttrs[off_గమ్యం];
		ఒకవేళ (hl > HL_ALL || (hl & HL_BOLD))
		{
		    పూర్ణాంకం prev_cells = 1;

		    ఒకవేళ (enc_utf8)
			// for utf-8, ScreenLines[char_offset + 1] == 0 means
			// that its width is 2.
			prev_cells = Screen_పంక్తులు[off_గమ్యం - 1] == 0 ? 2 : 1;
		    లేకపోతే ఒకవేళ (enc_dbcs != 0)
		    {
			// find previous character by counting from first
			// column and get its width.
			గుర్తులేని off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస];
			గుర్తులేని max_off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + screen_Columns;

			అయితే (off < off_గమ్యం)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    ఒకవేళ (!skip_for_popup(అడ్డువరుస, నిలువు + coloff - prev_cells))
		    {
			ఒకవేళ (enc_dbcs != 0 && prev_cells > 1)
			    screen_char_2(off_గమ్యం - prev_cells, అడ్డువరుస,
						   నిలువు + coloff - prev_cells);
			లేకపోతే
			    screen_char(off_గమ్యం - prev_cells, అడ్డువరుస,
						   నిలువు + coloff - prev_cells);
		    }
		}
	    }
#అయితే_ముగింపు
	    screen_fill(అడ్డువరుస, అడ్డువరుస + 1, నిలువు + coloff, clear_వెడల్పు + coloff,
								 ' ', ' ', 0);
	    అయితే (నిలువు < clear_వెడల్పు)
	    {
		Screen_నిలువుల_సంఖ్య[off_గమ్యం++]
			    = (flags & SLF_INC_VCOL) ? ++చివరి_vcol : చివరి_vcol;
		++నిలువు;
	    }
	}
    }

    ఒకవేళ (clear_వెడల్పు > 0
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && !(flags & SLF_POPUP)  // no separator for popup window
#అయితే_ముగింపు
	    )
    {
	// For a window that has a right neighbor, draw the separator char
	// right of the window contents.  But not on top of a popup window.
	ఒకవేళ (coloff + నిలువు < firstwin->w_wincol + topframe->fr_వెడల్పు)
	{
	    ఒకవేళ (!skip_for_popup(అడ్డువరుస, నిలువు + coloff))
	    {
		పూర్ణాంకం c;

		c = fillchar_vsep(&hl, wp, అడ్డువరుస);
		ఒకవేళ (Screen_పంక్తులు[off_గమ్యం] != (schar_T)c
			|| (enc_utf8 && (పూర్ణాంకం)Screen_పంక్తులు_UC[off_గమ్యం]
							!= (c >= 0x80 ? c : 0))
			|| ScreenAttrs[off_గమ్యం] != hl)
		{
		    Screen_పంక్తులు[off_గమ్యం] = c;
		    ScreenAttrs[off_గమ్యం] = hl;
		    ఒకవేళ (enc_utf8)
		    {
			ఒకవేళ (c >= 0x80)
			{
			    Screen_పంక్తులు_UC[off_గమ్యం] = c;
			    Screen_పంక్తులు_C[0][off_గమ్యం] = 0;
			    Screen_పంక్తులు[off_గమ్యం] = 0x80; // avoid storing zero
			}
			లేకపోతే
			    Screen_పంక్తులు_UC[off_గమ్యం] = 0;
		    }
		    screen_char(off_గమ్యం, అడ్డువరుస, నిలువు + coloff);
		}
	    }
	}
	లేకపోతే
	    పంక్తి_Wraps[అడ్డువరుస] = FALSE;
    }

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
}

#ఒకవేళ defined(FEAT_RIGHTLEFT)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    శూన్య
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    పూర్ణాంకం		t;

    కొరకు (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#అయితే_ముగింపు

/*
 * Draw the vertical separator right of window "wp" starting with line "row".
 * Includes status line rows so the VertSplit/VertSplitNC highlight is
 * refreshed after a current window change even for non-current windows
 * whose status lines are not redrawn.
 */
    శూన్య
draw_vsep_win(win_T *wp, పూర్ణాంకం అడ్డువరుస)
{
    ఒకవేళ (!wp->w_vsep_వెడల్పు)
	తిరిగి;

    పూర్ణాంకం content_చివర = W_WINROW(wp) + wp->w_ఎత్తు;

    // Content rows: VertSplit/VertSplitNC based on adjacency.  Include the
    // WinBar row (above the content) when redrawing from the top so that
    // the separator highlight is updated on current-window changes.
    పూర్ణాంకం ప్రారంభం_అడ్డువరుస = (అడ్డువరుస == 0) ? wp->w_winrow : W_WINROW(wp) + అడ్డువరుస;
    కొరకు (పూర్ణాంకం r = ప్రారంభం_అడ్డువరుస; r < content_చివర; ++r)
    {
	పూర్ణాంకం hl;
	పూర్ణాంకం c = fillchar_vsep(&hl, wp, r);
	screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }

    // Status line rows: when the status line is connected with the right
    // neighbour the separator cell is the status line fillchar; otherwise
    // it is the vsep char.
    ఒకవేళ (wp->w_స్థితి_ఎత్తు != 0)
    {
	పూర్ణాంకం hl;
	పూర్ణాంకం c;
	ఒకవేళ (stl_connected(wp))
	    c = fillchar_స్థితి(&hl, wp);
	లేకపోతే
	    c = fillchar_vsep(&hl, wp, content_చివర);
	కొరకు (పూర్ణాంకం r = content_చివర; r < content_చివర + wp->w_స్థితి_ఎత్తు; ++r)
	    screen_fill(r, r + 1, W_ENDCOL(wp), W_ENDCOL(wp) + 1, c, ' ', hl);
    }
}

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    పూర్ణాంకం
stl_connected(win_T *wp)
{
    చట్రం_T	*fr;

    fr = wp->w_చట్రం;
    అయితే (fr->fr_మాతృ != ఖాళీ)
    {
	ఒకవేళ (fr->fr_మాతృ->fr_layout == FR_నిలువు)
	{
	    ఒకవేళ (fr->fr_తదుపరి != ఖాళీ)
		ఆపు;
	}
	లేకపోతే
	{
	    ఒకవేళ (fr->fr_తదుపరి != ఖాళీ)
		తిరిగి TRUE;
	}
	fr = fr->fr_మాతృ;
    }
    తిరిగి FALSE;
}


/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    పూర్ణాంకం
get_keymap_str(
    win_T	*wp,
    char_u	*fmt,	    // format string containing one %s item
    char_u	*buf,	    // buffer for the result
    పూర్ణాంకం		len)	    // length of buffer
{
    char_u	*p;
    పూర్ణాంకం		plen;

    ఒకవేళ (wp->w_బఫరు->b_p_iminsert != B_IMODE_LMAP)
	తిరిగి 0;

#నిర్వచితమైతే FEAT_EVAL
    buf_T	*old_curbuf = curbuf;
    win_T	*old_curwin = curwin;
    char_u	గమ్యం_evaluate[] = "b:keymap_name";
    char_u	*s;

    curbuf = wp->w_బఫరు;
    curwin = wp;
    ++emsg_skip;
    s = p = eval_గమ్యం_string(గమ్యం_evaluate, FALSE, FALSE);
    --emsg_skip;
    curbuf = old_curbuf;
    curwin = old_curwin;
    ఒకవేళ (p == ఖాళీ || *p == NUL)
#అయితే_ముగింపు
    {
#నిర్వచితమైతే FEAT_KEYMAP
	ఒకవేళ (wp->w_బఫరు->b_kmap_state & KEYMAP_LOADED)
	    p = wp->w_బఫరు->b_p_keymap;
	లేకపోతే
#అయితే_ముగింపు
	    p = (char_u *)"lang";
    }
    plen = vim_snprintf((అక్షరం *)buf, len, (అక్షరం *)fmt, p);
#నిర్వచితమైతే FEAT_EVAL
    vim_free(s);
#అయితే_ముగింపు
    ఒకవేళ (plen < 0 || plen > len - 1)
    {
	buf[0] = NUL;
	plen = 0;
    }

    తిరిగి plen;
}

#ఒకవేళ defined(FEAT_STL_OPT)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    శూన్య
win_redr_custom(
    win_T	*wp,
    పూర్ణాంకం		draw_ruler)	// TRUE or FALSE
{
    స్థిర పూర్ణాంకం	entered = FALSE;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		curattr;
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు = 0;
    పూర్ణాంకం		maxwidth;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		n;
    పూర్ణాంకం		len;
    పూర్ణాంకం		stlh_cnt;
    పూర్ణాంకం		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    char_u	*opt_name;
    పూర్ణాంకం		opt_scope = 0;
    stl_hlrec_T *hltab;
    stl_hlrec_T *tabtab;
    stl_clickrec_T *clicktab;
    win_T	*ewp;
    పూర్ణాంకం		p_crb_save;
    bool	override_success = false;

    // There is a tiny chance that this gets called recursively: When
    // redrawing a status line triggers redrawing the ruler or tabline.
    // Avoid trouble by not allowing recursion.
    ఒకవేళ (entered)
	తిరిగి;
    entered = TRUE;

    // setup environment for the task at hand
    ఒకవేళ (wp == ఖాళీ)
    {
	// Use 'tabline'.  Always at the first line of the screen.
	stl = p_tal;
	అడ్డువరుస = 0;
	నిలువు = firstwin->w_wincol;
	fillchar = ' ';
	attr = HL_ATTR(HLF_TPF);
	maxwidth = topframe->fr_వెడల్పు;
	opt_name = (char_u *)"tabline";
    }
    లేకపోతే
    {
	override_success = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

	అడ్డువరుస = statusline_అడ్డువరుస(wp);
	fillchar = fillchar_స్థితి(&attr, wp);
	పూర్ణాంకం లోపల_స్థితి_పంక్తి_2 = wp->w_స్థితి_ఎత్తు != 0;
	maxwidth = లోపల_స్థితి_పంక్తి_2 ? wp->w_వెడల్పు : cmdline_వెడల్పు;

	ఒకవేళ (draw_ruler)
	{
	    stl = p_ruf;
	    opt_name = (char_u *)"rulerformat";
	    // advance past any leading group spec - implicit in ru_col
	    ఒకవేళ (*stl == '%')
	    {
		ఒకవేళ (*++stl == '-')
		    stl++;
		ఒకవేళ (atoi((అక్షరం *)stl))
		    అయితే (VIM_ISDIGIT(*stl))
			stl++;
		ఒకవేళ (*stl++ != '(')
		    stl = p_ruf;
	    }
	    నిలువు = ru_నిలువు - (cmdline_వెడల్పు - maxwidth);
	    ఒకవేళ (నిలువు < (maxwidth + 1) / 2)
		నిలువు = (maxwidth + 1) / 2;
	    maxwidth -= నిలువు;
	    ఒకవేళ (!లోపల_స్థితి_పంక్తి_2)
	    {
		అడ్డువరుస = అడ్డువరుసలు - 1;
		--maxwidth;	// writing in last column may cause scrolling
		fillchar = ' ';
		attr = 0;
	    }
	}
	లేకపోతే
	{
	    opt_name = (char_u *)"statusline";
	    ఒకవేళ (*wp->w_p_stl != NUL)
	    {
		stl = wp->w_p_stl;
		opt_scope = OPT_LOCAL;
	    }
	    లేకపోతే
		stl = p_stl;
	}

	ఒకవేళ (లోపల_స్థితి_పంక్తి_2)
	    నిలువు += wp->w_wincol;
	లేకపోతే
	    నిలువు += cmdline_నిలువు_off;
    }

    ఒకవేళ (maxwidth <= 0)
	వెళ్ళు theend;

    // Temporarily reset 'cursorbind', we don't want a side effect from moving
    // the cursor away and back.
    ewp = wp == ఖాళీ ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;
    stlh_cnt = draw_ruler || stl == p_tal ? 1 : wp->w_స్థితి_ఎత్తు;

    // Make a copy, because the statusline may include a function call that
    // might change the option value and free the memory.
    stl = vim_strsave(stl);
    char_u *stl_tmp = (stl == ఖాళీ) ? (char_u *)"" : stl;
    పూర్ణాంకం నిలువు_save = నిలువు;

    // Determine where click regions for this draw will be stored, and reset
    // the destination so each row in the loop below can append.  This must
    // be done before the loop because for a multi-line statusline each row
    // contributes its own click regions.
    stl_click_region_T  **బహిర్గతం_regions = ఖాళీ;
    పూర్ణాంకం			*బహిర్గతం_లెక్క = ఖాళీ;
    పూర్ణాంకం			region_పునాది_నిలువు = 0;
    ఒకవేళ (!draw_ruler)
    {
	ఒకవేళ (wp != ఖాళీ)
	{
	    బహిర్గతం_regions = &wp->w_stl_click;
	    బహిర్గతం_లెక్క = &wp->w_stl_click_లెక్క;
	    region_పునాది_నిలువు = wp->w_wincol;
	}
	లేకపోతే
	{
	    // 'tabline': store regions in global state since there is no
	    // associated window.
	    బహిర్గతం_regions = &tabline_stl_click;
	    బహిర్గతం_లెక్క = &tabline_stl_click_లెక్క;
	    region_పునాది_నిలువు = firstwin->w_wincol;
	}

	కొరకు (n = 0; n < *బహిర్గతం_లెక్క; n++)
	    vim_free((*బహిర్గతం_regions)[n].funcname);
	VIM_CLEAR(*బహిర్గతం_regions);
	*బహిర్గతం_లెక్క = 0;
    }

    పూర్ణాంకం వాహకం_hl = 0;
    కొరకు (పూర్ణాంకం i = 0; i < stlh_cnt; i++)
    {
	నిలువు = నిలువు_save;
	buf[0] = NUL;
	వెడల్పు = build_stl_str_hl_mline(ewp, buf, పరిమాణం(buf),
			&stl_tmp,
			opt_name, opt_scope,
			fillchar, maxwidth, &hltab, &tabtab,
			&clicktab,
			&వాహకం_hl);

	// Make all characters printable.
	p = transstr(buf);
	ఒకవేళ (p != ఖాళీ)
	{
	    len = vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), "%s", p);
	    vim_free(p);
	}
	లేకపోతే
	    len = (పూర్ణాంకం)STRLEN(buf);

	// fill up with "fillchar"
	అయితే (వెడల్పు < maxwidth && len < (పూర్ణాంకం)పరిమాణం(buf) - 1)
	{
	    len += (*mb_char2bytes)(fillchar, buf + len);
	    ++వెడల్పు;
	}
	buf[len] = NUL;

	/*
	 * Draw each snippet with the specified highlighting.
	 */
	curattr = attr;
	p = buf;
	కొరకు (n = 0; hltab[n].ప్రారంభం != ఖాళీ; n++)
	{
	    len = (పూర్ణాంకం)(hltab[n].ప్రారంభం - p);
	    screen_puts_len(p, len, అడ్డువరుస + i, నిలువు, curattr);
	    నిలువు += vim_strnsize(p, len);
	    p = hltab[n].ప్రారంభం;

	    ఒకవేళ (hltab[n].userhl == 0)
		curattr = attr;
	    లేకపోతే ఒకవేళ (hltab[n].userhl < 0)
		curattr = syn_id2attr(-hltab[n].userhl);
# ifdef FEAT_TERMINAL
	    లేకపోతే ఒకవేళ (wp != ఖాళీ && wp != curwin && bt_terminal(wp->w_బఫరు)
						    && wp->w_స్థితి_ఎత్తు != 0)
		curattr = highlight_stltermnc[hltab[n].userhl - 1];
	    లేకపోతే ఒకవేళ (wp != ఖాళీ && bt_terminal(wp->w_బఫరు)
						    && wp->w_స్థితి_ఎత్తు != 0)
		curattr = highlight_stlterm[hltab[n].userhl - 1];
# endif
	    లేకపోతే ఒకవేళ (wp != ఖాళీ && wp != curwin && wp->w_స్థితి_ఎత్తు != 0)
		curattr = highlight_stlnc[hltab[n].userhl - 1];
	    లేకపోతే
		curattr = highlight_user[hltab[n].userhl - 1];
	}
	screen_puts(p, అడ్డువరుస + i, నిలువు, curattr);

	// Append click regions for this row.  clicktab reflects the line
	// just rendered, so each row of a multi-line statusline contributes
	// its own regions.
	ఒకవేళ (బహిర్గతం_regions != ఖాళీ)
	{
	    పూర్ణాంకం click_లెక్క = 0;
	    కొరకు (n = 0; clicktab[n].ప్రారంభం != ఖాళీ; n++)
		click_లెక్క++;

	    ఒకవేళ (click_లెక్క > 0)
	    {
		stl_click_region_T *new_arr = vim_realloc(*బహిర్గతం_regions,
			పరిమాణం(stl_click_region_T)
				    * (*బహిర్గతం_లెక్క + click_లెక్క));
		ఒకవేళ (new_arr != ఖాళీ)
		{
		    char_u  *cur_funcname = ఖాళీ;
		    పూర్ణాంకం	    cur_minwid = 0;
		    పూర్ణాంకం	    region_ప్రారంభం = region_పునాది_నిలువు;

		    *బహిర్గతం_regions = new_arr;

		    len = 0;
		    p = buf;
		    కొరకు (n = 0; clicktab[n].ప్రారంభం != ఖాళీ; n++)
		    {
			len += vim_strnsize(p,
					    (పూర్ణాంకం)(clicktab[n].ప్రారంభం - p));
			p = clicktab[n].ప్రారంభం;

			ఒకవేళ (cur_funcname != ఖాళీ)
			{
			    stl_click_region_T *r =
					&(*బహిర్గతం_regions)[*బహిర్గతం_లెక్క];
			    r->అడ్డువరుస = అడ్డువరుస + i;
			    r->నిలువు_ప్రారంభం = region_ప్రారంభం;
			    r->నిలువు_చివర = region_పునాది_నిలువు + len;
			    r->funcname = vim_strsave(cur_funcname);
			    r->minwid = cur_minwid;
			    r->tabnr = 0;
			    (*బహిర్గతం_లెక్క)++;
			}

			cur_funcname = clicktab[n].funcname;
			cur_minwid = clicktab[n].minwid;
			region_ప్రారంభం = region_పునాది_నిలువు + len;
		    }

		    // Close final region if it extends to the end.
		    ఒకవేళ (cur_funcname != ఖాళీ)
		    {
			stl_click_region_T *r =
					&(*బహిర్గతం_regions)[*బహిర్గతం_లెక్క];
			r->అడ్డువరుస = అడ్డువరుస + i;
			r->నిలువు_ప్రారంభం = region_ప్రారంభం;
			r->నిలువు_చివర = region_పునాది_నిలువు + maxwidth;
			r->funcname = vim_strsave(cur_funcname);
			r->minwid = cur_minwid;
			r->tabnr = 0;
			(*బహిర్గతం_లెక్క)++;
		    }
		}
	    }
	}

	// Free the funcname strings allocated by build_stl_str_hl_local()
	// for this line.  They have been copied into the region array above.
	కొరకు (n = 0; clicktab[n].ప్రారంభం != ఖాళీ; n++)
	    vim_free(clicktab[n].funcname);
    }
    ewp->w_p_crb = p_crb_save;

    ఒకవేళ (p_sc && STRCMP(opt_name, p_sloc) == 0)
	showcmd_update_clear_state();

    // Note: In the loop, build_stl_str_hl_mline() may replace stl_tmp with
    // a newly allocated buffer (when "%!" evaluation occurs), freeing the
    // original "stl" internally.  After the loop, stl_tmp must be freed
    // instead of stl, as it holds the current buffer ownership.
    ఒకవేళ (stl != ఖాళీ)
	vim_free(stl_tmp);

    ఒకవేళ (wp == ఖాళీ)
    {
	// Fill the TabPageIdxs[] array for clicking in the tab pagesline.
	నిలువు = firstwin->w_wincol;
	len = 0;
	p = buf;
	fillchar = 0;
	కొరకు (n = 0; tabtab[n].ప్రారంభం != ఖాళీ; n++)
	{
	    len += vim_strnsize(p, (పూర్ణాంకం)(tabtab[n].ప్రారంభం - p));
	    అయితే (నిలువు < len)
		Tab_పుట_Idxs[నిలువు++] = fillchar;
	    p = tabtab[n].ప్రారంభం;
	    fillchar = tabtab[n].userhl;
	}
	అయితే (నిలువు < firstwin->w_wincol + topframe->fr_వెడల్పు)
	    Tab_పుట_Idxs[నిలువు++] = fillchar;
    }

theend:
    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
    entered = FALSE;
}

#అయితే_ముగింపు // FEAT_STL_OPT

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    శూన్య
screen_putchar(పూర్ణాంకం c, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    ఒకవేళ (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    లేకపోతే
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, అడ్డువరుస, నిలువు, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes", which must
 * have a size of "MB_MAXBYTES + 1".
 * If "attrp" is not NULL, return the character's attribute in "*attrp".
 */
    శూన్య
screen_getbytes(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *బైట్లు, పూర్ణాంకం *attrp)
{
    గుర్తులేని off;

    // safety check
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ || అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు >= screen_Columns)
	తిరిగి;

    off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు;
    ఒకవేళ (attrp != ఖాళీ)
	*attrp = ScreenAttrs[off];
    బైట్లు[0] = Screen_పంక్తులు[off];
    బైట్లు[1] = NUL;

    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0)
	బైట్లు[utfc_char2bytes(off, బైట్లు)] = NUL;
    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU && Screen_పంక్తులు[off] == 0x8e)
    {
	బైట్లు[0] = Screen_పంక్తులు[off];
	బైట్లు[1] = ScreenLines2[off];
	బైట్లు[2] = NUL;
    }
    లేకపోతే ఒకవేళ (enc_dbcs && MB_BYTE2LEN(బైట్లు[0]) > 1)
    {
	బైట్లు[1] = Screen_పంక్తులు[off + 1];
	బైట్లు[2] = NUL;
    }
}

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    స్థిర పూర్ణాంకం
screen_comp_differs(పూర్ణాంకం off, పూర్ణాంకం *u8cc)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < Screen_mco; ++i)
    {
	ఒకవేళ (Screen_పంక్తులు_C[i][off] != (u8char_T)u8cc[i])
	    తిరిగి TRUE;
	ఒకవేళ (u8cc[i] == 0)
	    ఆపు;
    }
    తిరిగి FALSE;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    శూన్య
screen_puts(
    char_u	*పాఠ్యం,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		attr)
{
    screen_puts_len(పాఠ్యం, -1, అడ్డువరుస, నిలువు, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    శూన్య
screen_puts_len(
    char_u	*పాఠ్యం,
    పూర్ణాంకం		textlen,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		attr_arg)
{
    పూర్ణాంకం		attr = attr_arg;
    గుర్తులేని	off;
    char_u	*ptr = పాఠ్యం;
    పూర్ణాంకం		len = textlen;
    పూర్ణాంకం		c;
    గుర్తులేని	max_off;
    పూర్ణాంకం		mbyte_blen = 1;
    పూర్ణాంకం		mbyte_cells = 1;
    పూర్ణాంకం		u8c = 0;
    పూర్ణాంకం		u8cc[MAX_MCO];
    పూర్ణాంకం		clear_తదుపరి_cell = FALSE;
#నిర్వచితమైతే FEAT_ARABIC
    పూర్ణాంకం		prev_c = 0;		// previous Arabic character
    పూర్ణాంకం		pc, nc, nc1;
    పూర్ణాంకం		pcc[MAX_MCO];
#అయితే_ముగింపు
    పూర్ణాంకం		force_redraw_this;
    పూర్ణాంకం		force_redraw_తదుపరి = FALSE;
    పూర్ణాంకం		need_redraw;

    // Safety check. The check for negative row and column is to fix issue
    // #4102. TODO: find out why row/col could be negative.
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ
	    || అడ్డువరుస >= screen_అడ్డువరుసలు || అడ్డువరుస < 0
	    || నిలువు >= screen_Columns || నిలువు < 0)
	తిరిగి;
    off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు;

    // When drawing over the right half of a double-wide char clear out the
    // left half.  Only needed in a terminal.
    ఒకవేళ (has_mbyte && నిలువు > 0 && నిలువు < screen_Columns
#నిర్వచితమైతే FEAT_GUI
	    && !gui.లోపల_use
#అయితే_ముగింపు
	    && mb_fix_నిలువు(నిలువు, అడ్డువరుస) != నిలువు)
    {
	// Keep the original attribute to preserve background color
	// when drawing over popup windows.
	ఒకవేళ (!skip_for_popup(అడ్డువరుస, నిలువు - 1))
	{
	    Screen_పంక్తులు[off - 1] = ' ';
	    ఒకవేళ (enc_dbcs == DBCS_JPNU)
		ScreenAttrs[off - 1] = 0;
	    లేకపోతే ఒకవేళ (enc_utf8)
	    {
		Screen_పంక్తులు_UC[off - 1] = 0;
		Screen_పంక్తులు_C[0][off - 1] = 0;
	    }
	    // redraw the previous cell, make it empty
	    screen_char(off - 1, అడ్డువరుస, నిలువు - 1);
	}
	// force the cell at "col" to be redrawn
	force_redraw_తదుపరి = TRUE;
    }

    max_off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + screen_Columns;
    అయితే (నిలువు < screen_Columns
	    && (len < 0 || (పూర్ణాంకం)(ptr - పాఠ్యం) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
	// check if this is the first byte of a multibyte
	ఒకవేళ (has_mbyte)
	{
	    mbyte_blen = enc_utf8 && len > 0
			     ? utfc_ptr2len_len(ptr, (పూర్ణాంకం)((పాఠ్యం + len) - ptr))
			     : (*mb_ptr2len)(ptr);
	    ఒకవేళ (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    లేకపోతే ఒకవేళ (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    లేకపోతే	// enc_utf8
	    {
		u8c = len >= 0
		      ? utfc_ptr2char_len(ptr, u8cc, (పూర్ణాంకం)((పాఠ్యం + len) - ptr))
		      : utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
#నిర్వచితమైతే FEAT_ARABIC
		ఒకవేళ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    // Do Arabic shaping.
		    ఒకవేళ (len >= 0 && (పూర్ణాంకం)(ptr - పాఠ్యం) + mbyte_blen >= len)
		    {
			// Past end of string to be displayed.
			nc = NUL;
			nc1 = NUL;
		    }
		    లేకపోతే
		    {
			nc = len >= 0
				 ? utfc_ptr2char_len(ptr + mbyte_blen, pcc,
					(పూర్ణాంకం)((పాఠ్యం + len) - ptr - mbyte_blen))
				 : utfc_ptr2char(ptr + mbyte_blen, pcc);
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		లేకపోతే
		    prev_c = u8c;
#అయితే_ముగింపు
		ఒకవేళ (నిలువు + mbyte_cells > screen_Columns)
		{
		    // Only 1 cell left, but character requires 2 cells:
		    // display a '>' in the last column to avoid wrapping.
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}

	force_redraw_this = force_redraw_తదుపరి;
	force_redraw_తదుపరి = FALSE;

	// When drawing pum text with opacity, blend the popup bg with the
	// saved underlying bg so text cells match the padding cells that
	// use hl_pum_blend_attr.  Without this, popup text appears on a
	// solid bg while padding shows a blended bg, creating a visible
	// seam.
	పూర్ణాంకం cell_attr = attr;
	ఒకవేళ (screen_pum_blend > 0 && pum_bg_attrs != ఖాళీ
		&& అడ్డువరుస >= pum_bg_శిఖరం && అడ్డువరుస < pum_bg_bot
		&& నిలువు < pum_bg_నిలువుల_సంఖ్య)
	{
	    పూర్ణాంకం soff = (అడ్డువరుస - pum_bg_శిఖరం) * pum_bg_నిలువుల_సంఖ్య + నిలువు;
	    cell_attr = hl_blend_attr(pum_bg_attrs[soff], attr,
					    screen_pum_blend, FALSE);
	}

	need_redraw = Screen_పంక్తులు[off] != c
		|| (mbyte_cells == 2
		    && Screen_పంక్తులు[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (Screen_పంక్తులు_UC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (Screen_పంక్తులు_UC[off] != 0
					  && screen_comp_differs(off, u8cc))))
		|| ScreenAttrs[off] != cell_attr
		|| exmode_చురుకైన
#నిర్వచితమైతే FEAT_PROP_POPUP
		// Output was suppressed under an opacity popup: the terminal
		// does not match ScreenLines[] even when unchanged.
		|| is_suppressed_cell(అడ్డువరుస, నిలువు)
#అయితే_ముగింపు
		;

	ఒకవేళ ((need_redraw || force_redraw_this) && !skip_for_popup(అడ్డువరుస, నిలువు))
	{
#ఒకవేళ defined(FEAT_GUI) || defined(UNIX)
	    // The bold trick makes a single row of pixels appear in the next
	    // character.  When a bold character is removed, the next
	    // character should be redrawn too.  This happens for our own GUI
	    // and for some xterms.
	    ఒకవేళ (need_redraw && Screen_పంక్తులు[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.లోపల_use
# endif
# ఒకవేళ defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    పదం_is_xterm
# endif
		    ))
	    {
		పూర్ణాంకం	n = ScreenAttrs[off];

		ఒకవేళ (n > HL_ALL)
		    n = syn_attr2attr(n);
		ఒకవేళ (n & HL_BOLD)
		    force_redraw_తదుపరి = TRUE;
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIRECTX
	    // DirectWrite subpixel rendering can extend pixels beyond
	    // cell boundaries.  Redraw the next character too.
	    ఒకవేళ (gui.directx_enabled && need_redraw)
		force_redraw_తదుపరి = TRUE;
#అయితే_ముగింపు
	    // When at the end of the text and overwriting a two-cell
	    // character with a one-cell character, need to clear the next
	    // cell.  Also when overwriting the left half of a two-cell char
	    // with the right half of a two-cell char.  Do this only once
	    // (mb_off2cells() may return 2 on the right half).
	    ఒకవేళ (clear_తదుపరి_cell)
		clear_తదుపరి_cell = FALSE;
	    లేకపోతే ఒకవేళ (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= పాఠ్యం + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_తదుపరి_cell = TRUE;

	    // Make sure we never leave a second byte of a double-byte behind,
	    // it confuses mb_off2cells().
	    ఒకవేళ (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		Screen_పంక్తులు[off + mbyte_blen] = 0;
	    Screen_పంక్తులు[off] = c;
	    ScreenAttrs[off] = cell_attr;
	    Screen_నిలువుల_సంఖ్య[off] = -1;
	    ఒకవేళ (enc_utf8)
	    {
		ఒకవేళ (c < 0x80 && u8cc[0] == 0)
		    Screen_పంక్తులు_UC[off] = 0;
		లేకపోతే
		{
		    పూర్ణాంకం	    i;

		    Screen_పంక్తులు_UC[off] = u8c;
		    కొరకు (i = 0; i < Screen_mco; ++i)
		    {
			Screen_పంక్తులు_C[i][off] = u8cc[i];
			ఒకవేళ (u8cc[i] == 0)
			    ఆపు;
		    }
		}
		ఒకవేళ (mbyte_cells == 2)
		{
		    Screen_పంక్తులు[off + 1] = 0;
		    Screen_పంక్తులు_UC[off + 1] = 0;
		    కొరకు (పూర్ణాంకం ci = 0; ci < Screen_mco; ++ci)
			Screen_పంక్తులు_C[ci][off + 1] = 0;
		    ScreenAttrs[off + 1] = cell_attr;
		    Screen_నిలువుల_సంఖ్య[off + 1] = -1;
		}
		screen_char(off, అడ్డువరుస, నిలువు);
	    }
	    లేకపోతే ఒకవేళ (mbyte_cells == 2)
	    {
		Screen_పంక్తులు[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = cell_attr;
		Screen_నిలువుల_సంఖ్య[off + 1] = -1;
		screen_char_2(off, అడ్డువరుస, నిలువు);
	    }
	    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, అడ్డువరుస, నిలువు);
	    }
	    లేకపోతే
		screen_char(off, అడ్డువరుస, నిలువు);
	}
	ఒకవేళ (has_mbyte)
	{
	    off += mbyte_cells;
	    నిలువు += mbyte_cells;
	    ptr += mbyte_blen;
	    ఒకవేళ (clear_తదుపరి_cell)
	    {
		// This only happens at the end, display one space next.
		// Keep the attribute from before.
		ptr = (char_u *)" ";
		len = -1;
		attr = ScreenAttrs[off];
	    }
	}
	లేకపోతే
	{
	    ++off;
	    ++నిలువు;
	    ++ptr;
	}
    }

    // If we detected the next character needs to be redrawn, but the text
    // doesn't extend up to there, update the character here.
    ఒకవేళ (force_redraw_తదుపరి && నిలువు < screen_Columns && !skip_for_popup(అడ్డువరుస, నిలువు))
    {
	ఒకవేళ (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, అడ్డువరుస, నిలువు);
	లేకపోతే
	    screen_char(off, అడ్డువరుస, నిలువు);
    }
}

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
/*
 * Prepare for 'hlsearch' highlighting.
 */
    శూన్య
ప్రారంభం_శోధన_hl(శూన్య)
{
    ఒకవేళ (!p_hls || no_hlsearch)
	తిరిగి;

    చివర_శోధన_hl();  // just in case it wasn't called before
    చివరి_pat_prog(&screen_శోధన_hl.rm);
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    శూన్య
చివర_శోధన_hl(శూన్య)
{
    ఒకవేళ (screen_శోధన_hl.rm.regprog == ఖాళీ)
	తిరిగి;

    vim_regfree(screen_శోధన_hl.rm.regprog);
    screen_శోధన_hl.rm.regprog = ఖాళీ;
}
#అయితే_ముగింపు

    స్థిర శూన్య
screen_ప్రారంభం_highlight(పూర్ణాంకం attr)
{
    attrentry_T *aep = ఖాళీ;

    screen_attr = attr;
    ఒకవేళ (!నిండిన_screen
#నిర్వచితమైతే MSWIN
	    || !termcap_చురుకైన
#అయితే_ముగింపు
       )
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	అక్షరం	buf[20];

	// The GUI handles this internally.
	sprintf(buf, "\033|%dh", attr);
	బహిర్గతం_STR(buf);
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (attr > HL_ALL)				// special HL attr.
    {
	ఒకవేళ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	లేకపోతే
	    aep = syn_పదం_attr2entry(attr);
	ఒకవేళ (aep == ఖాళీ)	    // did ":syntax clear"
	    attr = 0;
	లేకపోతే
	    attr = aep->ae_attr;
    }
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (use_vtp())
    {
	guicolor_T  defguifg, defguibg;
	పూర్ణాంకం	    defctermfg, defctermbg;

	// If FG and BG are unset, the color is undefined when
	// BOLD+INVERSE. Use Normal as the default value.
	get_default_console_color(&defctermfg, &defctermbg, &defguifg,
		&defguibg);

	ఒకవేళ (p_tgc)
	{
	    ఒకవేళ (aep == ఖాళీ || COLOR_INVALID(aep->ae_u.cterm.fg_rgb))
		పదం_fg_rgb_color(defguifg);
	    ఒకవేళ (aep == ఖాళీ || COLOR_INVALID(aep->ae_u.cterm.bg_rgb))
		పదం_bg_rgb_color(defguibg);
	}
	లేకపోతే ఒకవేళ (t_colors >= 256)
	{
	    ఒకవేళ (aep == ఖాళీ || aep->ae_u.cterm.fg_color == 0)
		పదం_fg_color(defctermfg);
	    ఒకవేళ (aep == ఖాళీ || aep->ae_u.cterm.bg_color == 0)
		పదం_bg_color(defctermbg);
	}
    }
#అయితే_ముగింపు
    ఒకవేళ ((attr & HL_BOLD) && *T_MD != NUL)	// bold
	బహిర్గతం_str(T_MD);
    లేకపోతే ఒకవేళ (aep != ఖాళీ && cterm_సాధారణ_fg_bold && (
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
		? aep->ae_u.cterm.fg_rgb != INVALCOLOR
		:
#అయితే_ముగింపు
		t_colors > 1 && aep->ae_u.cterm.fg_color))
	// If the Normal FG color has BOLD attribute and the new HL
	// has a FG color defined, clear BOLD.
	బహిర్గతం_str(T_ME);
    ఒకవేళ ((attr & HL_STANDOUT) && *T_SO != NUL)	// standout
	బహిర్గతం_str(T_SO);
    ఒకవేళ ((attr & HL_UNDERCURL) && *T_UCS != NUL) // undercurl
	బహిర్గతం_str(T_UCS);
    ఒకవేళ ((attr & HL_UNDERDOUBLE) && *T_USS != NUL) // double underline
	బహిర్గతం_str(T_USS);
    ఒకవేళ ((attr & HL_UNDERDOTTED) && *T_DS != NUL) // dotted underline
	బహిర్గతం_str(T_DS);
    ఒకవేళ ((attr & HL_UNDERDASHED) && *T_CDS != NUL) // dashed underline
	బహిర్గతం_str(T_CDS);
    ఒకవేళ (((attr & HL_UNDERLINE)	    // underline or undercurl, etc.
		|| ((attr & HL_UNDERCURL) && *T_UCS == NUL)
		|| ((attr & HL_UNDERDOUBLE) && *T_USS == NUL)
		|| ((attr & HL_UNDERDOTTED) && *T_DS == NUL)
		|| ((attr & HL_UNDERDASHED) && *T_CDS == NUL))
	    && *T_US != NUL)
	బహిర్గతం_str(T_US);
    ఒకవేళ ((attr & HL_ITALIC) && *T_CZH != NUL)	// italic
	బహిర్గతం_str(T_CZH);
    ఒకవేళ ((attr & HL_INVERSE) && *T_MR != NUL)	// inverse (reverse)
	బహిర్గతం_str(T_MR);
    ఒకవేళ ((attr & HL_STRIKETHROUGH) && *T_STS != NUL)	// strike
	బహిర్గతం_str(T_STS);

    /*
     * Output the color or start string after bold etc., in case the
     * bold etc. override the color setting.
     */
    ఒకవేళ (aep != ఖాళీ)
    {
	ఒకవేళ (aep->ae_u.cterm.font > 0 && aep->ae_u.cterm.font < 12)
		పదం_font(aep->ae_u.cterm.font);
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	// When 'termguicolors' is set but fg or bg is unset,
	// fall back to the cterm colors.   This helps for SpellBad,
	// where the GUI uses a red undercurl.
	ఒకవేళ (p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR)
	{
	    ఒకవేళ (aep->ae_u.cterm.fg_rgb != INVALCOLOR)
		పదం_fg_rgb_color(aep->ae_u.cterm.fg_rgb);
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (t_colors > 1)
	    {
		ఒకవేళ (aep->ae_u.cterm.fg_color)
		    పదం_fg_color(aep->ae_u.cterm.fg_color - 1);
	    }
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	ఒకవేళ (p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR)
	{
	    ఒకవేళ (aep->ae_u.cterm.bg_rgb != INVALCOLOR)
		పదం_bg_rgb_color(aep->ae_u.cterm.bg_rgb);
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (t_colors > 1)
	    {
		ఒకవేళ (aep->ae_u.cterm.bg_color)
		    పదం_bg_color(aep->ae_u.cterm.bg_color - 1);
	    }
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	ఒకవేళ (p_tgc && aep->ae_u.cterm.ul_rgb != CTERMCOLOR)
	{
	    ఒకవేళ (aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		పదం_ul_rgb_color(aep->ae_u.cterm.ul_rgb);
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (t_colors > 1)
	    {
		ఒకవేళ (aep->ae_u.cterm.ul_color)
		    పదం_ul_color(aep->ae_u.cterm.ul_color - 1);
	    }

	ఒకవేళ (!IS_CTERM)
	{
	    ఒకవేళ (aep->ae_u.పదం.ప్రారంభం != ఖాళీ)
		బహిర్గతం_str(aep->ae_u.పదం.ప్రారంభం);
	}
    }
}

    శూన్య
screen_stop_highlight(శూన్య)
{
    పూర్ణాంకం	    do_ME = FALSE;	    // output T_ME code
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    పూర్ణాంకం	    do_ME_fg = FALSE, do_ME_bg = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (screen_attr != 0
#నిర్వచితమైతే MSWIN
			&& termcap_చురుకైన
#అయితే_ముగింపు
					   )
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    అక్షరం	buf[20];

	    // use internal GUI code
	    sprintf(buf, "\033|%dH", screen_attr);
	    బహిర్గతం_STR(buf);
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    పూర్ణాంకం is_under;

	    ఒకవేళ (screen_attr > HL_ALL)			// special HL attr.
	    {
		attrentry_T *aep;

		ఒకవేళ (IS_CTERM)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    ఒకవేళ (aep != ఖాళీ && ((
#నిర్వచితమైతే FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.fg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.fg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_fg = TRUE) : (do_ME_fg = FALSE)
# endif
				:
#అయితే_ముగింపు
				aep->ae_u.cterm.fg_color) || (
#నిర్వచితమైతే FEAT_TERMGUICOLORS
			    p_tgc && aep->ae_u.cterm.bg_rgb != CTERMCOLOR
				? aep->ae_u.cterm.bg_rgb != INVALCOLOR
# ifdef FEAT_VTP
				    ? !(do_ME_bg = TRUE) : (do_ME_bg = FALSE)
# endif
				:
#అయితే_ముగింపు
				aep->ae_u.cterm.bg_color)))
			do_ME = TRUE;
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    ఒకవేళ (use_vtp())
		    {
			ఒకవేళ (do_ME_fg && do_ME_bg)
			    do_ME = TRUE;

			// FG and BG cannot be separated in T_ME, which is not
			// efficient.
			ఒకవేళ (!do_ME && do_ME_fg)
			    బహిర్గతం_str((char_u *)"\033|39m"); // restore FG
			ఒకవేళ (!do_ME && do_ME_bg)
			    బహిర్గతం_str((char_u *)"\033|49m"); // restore BG
		    }
		    లేకపోతే
		    {
			// Process FG and BG at once.
			ఒకవేళ (!do_ME)
			    do_ME = do_ME_fg | do_ME_bg;
		    }
#అయితే_ముగింపు
		}
		లేకపోతే
		{
		    aep = syn_పదం_attr2entry(screen_attr);
		    ఒకవేళ (aep != ఖాళీ && aep->ae_u.పదం.stop != ఖాళీ)
		    {
			ఒకవేళ (STRCMP(aep->ae_u.పదం.stop, T_ME) == 0)
			    do_ME = TRUE;
			లేకపోతే
			    బహిర్గతం_str(aep->ae_u.పదం.stop);
		    }
		}
		ఒకవేళ (aep == ఖాళీ)	    // did ":syntax clear"
		    screen_attr = 0;
		లేకపోతే
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    ఒకవేళ (screen_attr & HL_STANDOUT)
	    {
		ఒకవేళ (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		లేకపోతే
		    బహిర్గతం_str(T_SE);
	    }
	    is_under = (screen_attr & (HL_UNDERCURL
			  | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED));
	    ఒకవేళ (is_under && *T_UCE != NUL)
	    {
		ఒకవేళ (STRCMP(T_UCE, T_ME) == 0)
		    do_ME = TRUE;
		లేకపోతే
		    బహిర్గతం_str(T_UCE);
	    }
	    ఒకవేళ ((screen_attr & HL_UNDERLINE) || (is_under && *T_UCE == NUL))
	    {
		ఒకవేళ (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		లేకపోతే
		    బహిర్గతం_str(T_UE);
	    }
	    ఒకవేళ (screen_attr & HL_ITALIC)
	    {
		ఒకవేళ (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		లేకపోతే
		    బహిర్గతం_str(T_CZR);
	    }
	    ఒకవేళ (screen_attr & HL_STRIKETHROUGH)
	    {
		ఒకవేళ (STRCMP(T_STE, T_ME) == 0)
		    do_ME = TRUE;
		లేకపోతే
		    బహిర్గతం_str(T_STE);
	    }
	    ఒకవేళ (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		బహిర్గతం_str(T_ME);

#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    ఒకవేళ (p_tgc)
	    {
		ఒకవేళ (cterm_సాధారణ_fg_gui_color != INVALCOLOR)
		    పదం_fg_rgb_color(cterm_సాధారణ_fg_gui_color);
		ఒకవేళ (cterm_సాధారణ_bg_gui_color != INVALCOLOR)
		    పదం_bg_rgb_color(cterm_సాధారణ_bg_gui_color);
		ఒకవేళ (cterm_సాధారణ_ul_gui_color != INVALCOLOR)
		    పదం_ul_rgb_color(cterm_సాధారణ_ul_gui_color);
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		ఒకవేళ (t_colors > 1)
		{
		    // set Normal cterm colors
		    ఒకవేళ (cterm_సాధారణ_fg_color != 0)
			పదం_fg_color(cterm_సాధారణ_fg_color - 1);
		    ఒకవేళ (cterm_సాధారణ_bg_color != 0)
			పదం_bg_color(cterm_సాధారణ_bg_color - 1);
		    ఒకవేళ (cterm_సాధారణ_ul_color != 0)
			పదం_ul_color(cterm_సాధారణ_ul_color - 1);
		    ఒకవేళ (cterm_సాధారణ_fg_bold)
			బహిర్గతం_str(T_MD);
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
    శూన్య
reset_cterm_colors(శూన్య)
{
    ఒకవేళ (!IS_CTERM)
	తిరిగి;

    // set Normal cterm colors
#నిర్వచితమైతే FEAT_TERMGUICOLORS
    ఒకవేళ (p_tgc ? (cterm_సాధారణ_fg_gui_color != INVALCOLOR
		|| cterm_సాధారణ_bg_gui_color != INVALCOLOR)
	    : (cterm_సాధారణ_fg_color > 0 || cterm_సాధారణ_bg_color > 0))
#లేకపోతే
	ఒకవేళ (cterm_సాధారణ_fg_color > 0 || cterm_సాధారణ_bg_color > 0)
#అయితే_ముగింపు
	{
	    బహిర్గతం_str(T_OP);
	    screen_attr = -1;
	}
    ఒకవేళ (cterm_సాధారణ_fg_bold)
    {
	బహిర్గతం_str(T_ME);
	screen_attr = -1;
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    శూన్య
screen_char(గుర్తులేని off, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		attr;

    // Check for illegal values, just in case (could happen just after
    // resizing).
    ఒకవేళ (అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు >= screen_Columns)
	తిరిగి;

#నిర్వచితమైతే FEAT_IMAGE
    mark_dirty_region_for_images(అడ్డువరుస, నిలువు, 1, 1);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    // If this cell is under a higher-zindex opacity popup, suppress
    // output to prevent flicker.  The higher popup's redraw will
    // output the final blended result.
    // Also suppress if this is a wide character whose second cell
    // is under an opacity popup.
    ఒకవేళ (popup_is_under_opacity(అడ్డువరుస, నిలువు))
    {
	// If this is a wide character whose left half is under an opacity
	// popup but right half is not, clear the right half so the old
	// blended value doesn't remain as a ghost after popup_move().
	ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0
		&& utf_char2cells(Screen_పంక్తులు_UC[off]) == 2
		&& నిలువు + 1 < screen_Columns
		&& !popup_is_under_opacity(అడ్డువరుస, నిలువు + 1))
	{
	    పూర్ణాంకం off2 = off + 1;
	    Screen_పంక్తులు[off2] = ' ';
	    Screen_పంక్తులు_UC[off2] = 0;
	    screen_char(off2, అడ్డువరుస, నిలువు + 1);
	}
	mark_suppressed_cell(అడ్డువరుస, నిలువు);
	ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0
		&& utf_char2cells(Screen_పంక్తులు_UC[off]) == 2
		&& నిలువు + 1 < screen_Columns
		&& popup_is_under_opacity(అడ్డువరుస, నిలువు + 1))
	    mark_suppressed_cell(అడ్డువరుస, నిలువు + 1);
	screen_cur_నిలువు = 9999;
	తిరిగి;
    }
    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0
	    && utf_char2cells(Screen_పంక్తులు_UC[off]) == 2
	    && నిలువు + 1 < screen_Columns
	    && popup_is_under_opacity(అడ్డువరుస, నిలువు + 1))
    {
	mark_suppressed_cell(అడ్డువరుస, నిలువు);
	mark_suppressed_cell(అడ్డువరుస, నిలువు + 1);
	screen_cur_నిలువు = 9999;
	తిరిగి;
    }
#అయితే_ముగింపు

    // Outputting a character in the last cell on the screen may scroll the
    // screen up.  Only do it when the "xn" termcap property is set, otherwise
    // mark the character invalid (update it when scrolled up).
    ఒకవేళ (*T_XN == NUL
	    && అడ్డువరుస == screen_అడ్డువరుసలు - 1 && నిలువు == screen_Columns - 1
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    // account for first command-line character in rightleft mode
	    && !cmdmsg_rl
#అయితే_ముగింపు
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_నిలువుల_సంఖ్య[off] = -1;
	తిరిగి;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    ఒకవేళ (screen_char_attr != 0)
	attr = screen_char_attr;
    లేకపోతే
	attr = ScreenAttrs[off];
    ఒకవేళ (screen_attr != attr)
	screen_stop_highlight();

    windgoto(అడ్డువరుస, నిలువు);

#నిర్వచితమైతే FEAT_PROP_POPUP
    // The cell is output below, the terminal matches ScreenLines again.
    unmark_suppressed_cell(అడ్డువరుస, నిలువు);
    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0
	    && utf_char2cells(Screen_పంక్తులు_UC[off]) == 2)
	unmark_suppressed_cell(అడ్డువరుస, నిలువు + 1);
#అయితే_ముగింపు

    ఒకవేళ (screen_attr != attr)
	screen_ప్రారంభం_highlight(attr);

    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	ఒకవేళ (
#నిర్వచితమైతే FEAT_GUI
	    !gui.లోపల_use &&
#అయితే_ముగింపు
	    get_cellwidth(Screen_పంక్తులు_UC[off]) > 1
	    )
	{
	    // If the width is set to 2 with setcellwidths()
	    // clear the two screen cells. If the character is actually
	    // single width it won't change the second cell.
	    బహిర్గతం_str((char_u *)"  ");
	    పదం_windgoto(అడ్డువరుస, నిలువు);
	    screen_cur_నిలువు = 9999;
	}
	లేకపోతే ఒకవేళ (utf_ambiguous_వెడల్పు(Screen_పంక్తులు_UC[off]))
	{
	    ఒకవేళ (*p_ambw == 'd'
#నిర్వచితమైతే FEAT_GUI
		    && !gui.లోపల_use
#అయితే_ముగింపు
		    )
	    {
		// Clear the two screen cells. If the character is actually
		// single width it won't change the second cell.
		బహిర్గతం_str((char_u *)"  ");
		పదం_windgoto(అడ్డువరుస, నిలువు);
	    }
	    // not sure where the cursor is after drawing the ambiguous width
	    // character
	    screen_cur_నిలువు = 9999;
	}
	లేకపోతే ఒకవేళ (utf_char2cells(Screen_పంక్తులు_UC[off]) > 1)
	    ++screen_cur_నిలువు;

	// Convert the UTF-8 character to bytes and write it.
	buf[utfc_char2bytes(off, buf)] = NUL;
	బహిర్గతం_str(buf);
    }
    లేకపోతే
    {
	బహిర్గతం_flush_check();
	బహిర్గతం_char(Screen_పంక్తులు[off]);
	// double-byte character in single-width cell
	ఒకవేళ (enc_dbcs == DBCS_JPNU && Screen_పంక్తులు[off] == 0x8e)
	    బహిర్గతం_char(ScreenLines2[off]);
    }

    screen_cur_నిలువు++;
}

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    స్థిర శూన్య
screen_char_2(గుర్తులేని off, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    // Check for illegal values (could be wrong when screen was resized).
    ఒకవేళ (off + 1 >= (గుర్తులేని)(screen_అడ్డువరుసలు * screen_Columns))
	తిరిగి;

    // Outputting the last character on the screen may scroll the screen up.
    // Don't to it!  Mark the character invalid (update it when scrolled up)
    ఒకవేళ (అడ్డువరుస == screen_అడ్డువరుసలు - 1 && నిలువు >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	Screen_నిలువుల_సంఖ్య[off] = -1;
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_PROP_POPUP
    // If under a higher-zindex opacity popup, suppress output.
    ఒకవేళ (popup_is_under_opacity(అడ్డువరుస, నిలువు))
    {
	mark_suppressed_cell(అడ్డువరుస, నిలువు);
	mark_suppressed_cell(అడ్డువరుస, నిలువు + 1);
	screen_cur_నిలువు = 9999;
	తిరిగి;
    }
#అయితే_ముగింపు

    // Output the first byte normally (positions the cursor), then write the
    // second byte directly.
    screen_char(off, అడ్డువరుస, నిలువు);
    బహిర్గతం_char(Screen_పంక్తులు[off + 1]);
#నిర్వచితమైతే FEAT_PROP_POPUP
    unmark_suppressed_cell(అడ్డువరుస, నిలువు + 1);
#అయితే_ముగింపు
    ++screen_cur_నిలువు;
}

/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    శూన్య
screen_draw_rectangle(
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		ఎత్తు,
    పూర్ణాంకం		వెడల్పు,
    పూర్ణాంకం		invert,
    పూర్ణాంకం		force)
{
    పూర్ణాంకం		r, c;
    పూర్ణాంకం		off;
    పూర్ణాంకం		max_off;

    // Can't use ScreenLines unless initialized
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    ఒకవేళ (invert)
	screen_char_attr = HL_INVERSE;
    కొరకు (r = అడ్డువరుస; r < అడ్డువరుస + ఎత్తు; ++r)
    {
	off = పంక్తి_స్థానభ్రంశం[r];
	max_off = off + screen_Columns;
	కొరకు (c = నిలువు; c < నిలువు + వెడల్పు; ++c)
	{
	    ఒకవేళ (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		ఒకవేళ (force || !skip_for_popup(r, c))
		    screen_char_2(off + c, r, c);
		++c;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (force || !skip_for_popup(r, c))
		    screen_char(off + c, r, c);
		ఒకవేళ (utf_off2cells(off + c, max_off) > 1)
		    ++c;
	    }
	}
    }
    screen_char_attr = 0;
}

/*
 * Redraw the characters for a vertically split window.
 */
    స్థిర శూన్య
redraw_ఖండం(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం చివర, win_T *wp)
{
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		వెడల్పు;

#నిర్వచితమైతే FEAT_CLIPBOARD
    clip_may_clear_selection(అడ్డువరుస, చివర - 1);
#అయితే_ముగింపు

    ఒకవేళ (wp == ఖాళీ)
    {
	నిలువు = firstwin->w_wincol;
	వెడల్పు = topframe->fr_వెడల్పు;
    }
    లేకపోతే
    {
	నిలువు = wp->w_wincol;
	వెడల్పు = wp->w_వెడల్పు;
    }
    screen_draw_rectangle(అడ్డువరుస, నిలువు, చివర - అడ్డువరుస, వెడల్పు, FALSE, FALSE);
}

    శూన్య
space_గమ్యం_screenline(పూర్ణాంకం off, పూర్ణాంకం attr)
{
    Screen_పంక్తులు[off] = ' ';
    ScreenAttrs[off] = attr;
    Screen_నిలువుల_సంఖ్య[off] = -1;
    ఒకవేళ (enc_utf8)
	Screen_పంక్తులు_UC[off] = 0;
}

/*
 * Fill the screen from "start_row" to "end_row" (exclusive), from "start_col"
 * to "end_col" (exclusive) with character "c1" in first column followed by
 * "c2" in the other columns.  Use attributes "attr".
 */
    శూన్య
screen_fill(
	పూర్ణాంకం	ప్రారంభం_అడ్డువరుస,
	పూర్ణాంకం	చివర_అడ్డువరుస,
	పూర్ణాంకం	ప్రారంభం_నిలువు,
	పూర్ణాంకం	చివర_నిలువు,
	పూర్ణాంకం	c1,
	పూర్ణాంకం	c2,
	పూర్ణాంకం	attr)
{
    పూర్ణాంకం	    అడ్డువరుస;
    పూర్ణాంకం	    నిలువు;
    పూర్ణాంకం	    off;
    పూర్ణాంకం	    చివర_off;
    పూర్ణాంకం	    did_delete;
    పూర్ణాంకం	    c;
    పూర్ణాంకం	    norm_పదం;
#ఒకవేళ defined(FEAT_GUI) || defined(UNIX)
    పూర్ణాంకం	    force_తదుపరి = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (చివర_అడ్డువరుస > screen_అడ్డువరుసలు)		// safety check
	చివర_అడ్డువరుస = screen_అడ్డువరుసలు;
    ఒకవేళ (చివర_నిలువు > screen_Columns)	// safety check
	చివర_నిలువు = screen_Columns;
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ
	    || ప్రారంభం_అడ్డువరుస < 0		// should not happen
	    || ప్రారంభం_అడ్డువరుస >= చివర_అడ్డువరుస
	    || ప్రారంభం_నిలువు >= చివర_నిలువు)	// nothing to do
	తిరిగి;

    // it's a "normal" terminal when not in a GUI or cterm
    norm_పదం = (
#నిర్వచితమైతే FEAT_GUI
	    !gui.లోపల_use &&
#అయితే_ముగింపు
	    !IS_CTERM);
    కొరకు (అడ్డువరుస = ప్రారంభం_అడ్డువరుస; అడ్డువరుస < చివర_అడ్డువరుస; ++అడ్డువరుస)
    {
	ఒకవేళ (has_mbyte
#నిర్వచితమైతే FEAT_GUI
		&& !gui.లోపల_use
#అయితే_ముగింపు
	   )
	{
	    // When drawing over the right half of a double-wide char clear
	    // out the left half.  When drawing over the left half of a
	    // double wide-char clear out the right half.  Only needed in a
	    // terminal.
	    ఒకవేళ (ప్రారంభం_నిలువు > 0 && mb_fix_నిలువు(ప్రారంభం_నిలువు, అడ్డువరుస) != ప్రారంభం_నిలువు)
	    {
		// start_col is the right half, clear the left half
		పూర్ణాంకం ఎడమ_attr = ScreenAttrs[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + ప్రారంభం_నిలువు - 1];
		screen_puts_len((char_u *)" ", 1, అడ్డువరుస, ప్రారంభం_నిలువు - 1, ఎడమ_attr);
	    }
	    ఒకవేళ (చివర_నిలువు < screen_Columns && mb_fix_నిలువు(చివర_నిలువు, అడ్డువరుస) != చివర_నిలువు)
	    {
		// end_col is the right half of a wide char, preserve its attribute
		పూర్ణాంకం కుడి_attr = ScreenAttrs[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + చివర_నిలువు];
		screen_puts_len((char_u *)" ", 1, అడ్డువరుస, చివర_నిలువు, కుడి_attr);
	    }
	}
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	ఒకవేళ (c2 == ' '
		&& చివర_నిలువు == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_పదం
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0)))
#నిర్వచితమైతే FEAT_PROP_POPUP
		// Do not use T_CE optimization if any cell in the
		// range is under an opacity popup.  The clear-to-eol
		// command would erase the popup area on screen.
		&& !popup_is_under_opacity_range(అడ్డువరుస,
						ప్రారంభం_నిలువు, చివర_నిలువు)
#అయితే_ముగింపు
		)
	{
	    /*
	     * check if we really need to clear something
	     */
	    నిలువు = ప్రారంభం_నిలువు;
	    ఒకవేళ (c1 != ' ')			// don't clear first char
		++నిలువు;

	    off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + నిలువు;
	    చివర_off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + చివర_నిలువు;

	    // skip blanks (used often, keep it fast!)
	    ఒకవేళ (enc_utf8)
		అయితే (off < చివర_off && Screen_పంక్తులు[off] == ' '
			  && ScreenAttrs[off] == 0 && Screen_పంక్తులు_UC[off] == 0
#నిర్వచితమైతే FEAT_PROP_POPUP
			  && !is_suppressed_cell(అడ్డువరుస,
					      (పూర్ణాంకం)(off - పంక్తి_స్థానభ్రంశం[అడ్డువరుస]))
#అయితే_ముగింపు
			  )
		    ++off;
	    లేకపోతే
		అయితే (off < చివర_off && Screen_పంక్తులు[off] == ' '
						     && ScreenAttrs[off] == 0
#నిర్వచితమైతే FEAT_PROP_POPUP
			  && !is_suppressed_cell(అడ్డువరుస,
					      (పూర్ణాంకం)(off - పంక్తి_స్థానభ్రంశం[అడ్డువరుస]))
#అయితే_ముగింపు
			  )
		    ++off;
	    ఒకవేళ (off < చివర_off)		// something to be cleared
	    {
		నిలువు = off - పంక్తి_స్థానభ్రంశం[అడ్డువరుస];
		screen_stop_highlight();
		పదం_windgoto(అడ్డువరుస, నిలువు);// clear rest of this screen line
		బహిర్గతం_str(T_CE);
		screen_ప్రారంభం();		// don't know where cursor is now
		నిలువు = చివర_నిలువు - నిలువు;
		అయితే (నిలువు--)		// clear chars in ScreenLines
		{
		    space_గమ్యం_screenline(off, 0);
#నిర్వచితమైతే FEAT_PROP_POPUP
		    // T_CE cleared the terminal cell, it matches again.
		    unmark_suppressed_cell(అడ్డువరుస, (పూర్ణాంకం)(off - పంక్తి_స్థానభ్రంశం[అడ్డువరుస]));
#అయితే_ముగింపు
		    ++off;
		}
	    }
	    did_delete = TRUE;		// the chars are cleared now
	}

	off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + ప్రారంభం_నిలువు;
	c = c1;
	కొరకు (నిలువు = ప్రారంభం_నిలువు; నిలువు < చివర_నిలువు; ++నిలువు)
	{
	    ఒకవేళ ((Screen_పంక్తులు[off] != c
		    || (enc_utf8 && (పూర్ణాంకం)Screen_పంక్తులు_UC[off]
						       != (c >= 0x80 ? c : 0))
		    || ScreenAttrs[off] != attr
		    || must_redraw == UPD_CLEAR  // screen clear pending
#ఒకవేళ defined(FEAT_GUI) || defined(UNIX)
		    || force_తదుపరి
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
		    // Output was suppressed under an opacity popup: the
		    // terminal does not match ScreenLines[] even when
		    // unchanged.
		    || is_suppressed_cell(అడ్డువరుస, నిలువు)
#అయితే_ముగింపు
		    )
		    // Skip if under a(nother) popup.
		    && !skip_for_popup(అడ్డువరుస, నిలువు))
	    {
#నిర్వచితమైతే FEAT_PROP_POPUP
		// For popup with opacity: show underlying character with
		// popup's background color applied.
		ఒకవేళ (screen_opacity_popup != ఖాళీ && c == ' ')
		{
		    // Skip if background is the second cell of a wide character.
		    // Check if previous cell is a wide character.
		    ఒకవేళ (Screen_పంక్తులు[off] == 0
			    && off > 0
			    && enc_utf8 && Screen_పంక్తులు_UC[off - 1] != 0
			    && utf_char2cells(Screen_పంక్తులు_UC[off - 1]) == 2)
			వెళ్ళు తదుపరి_నిలువు;
		}
		ఒకవేళ (screen_opacity_popup != ఖాళీ && c == ' '
						       && Screen_పంక్తులు[off] != 0)
		{
		    పూర్ణాంకం bg_char_cells = 1;
		    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0)
			bg_char_cells = utf_char2cells(Screen_పంక్తులు_UC[off]);

		    // For wide background character, check if the next cell
		    // is also being filled with space.  If not, the wide char
		    // would be partially covered, so don't show it.
		    ఒకవేళ (bg_char_cells == 2)
		    {
			ఒకవేళ (నిలువు + 1 >= చివర_నిలువు)
			    // At the edge, skip wide char.
			    వెళ్ళు skip_opacity_fill;
			// In screen_fill, we're filling with 'c' which is ' '.
			// The next cell will also be filled with c2 (usually ' ').
			// If c2 is not space, skip the wide char.
			ఒకవేళ (c2 != ' ')
			    వెళ్ళు skip_opacity_fill;
		    }

		    పూర్ణాంకం popup_attr = get_win_attr(screen_opacity_popup);
		    పూర్ణాంకం blend = screen_opacity_popup->w_popup_blend;
		    // Blend both foreground and background for padding area
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
							popup_attr, blend, TRUE);
		    screen_char(off, అడ్డువరుస, నిలువు);
		    వెళ్ళు తదుపరి_నిలువు;
		}
skip_opacity_fill:
#అయితే_ముగింపు
		// For pum opacity: blend pum background with underlying.
		// Only for space cells; text cells are handled normally.
		ఒకవేళ (screen_pum_blend > 0 && c == ' '
			&& pum_bg_attrs != ఖాళీ
			&& అడ్డువరుస >= pum_bg_శిఖరం && అడ్డువరుస < pum_bg_bot
			&& నిలువు < pum_bg_నిలువుల_సంఖ్య)
		{
		    పూర్ణాంకం soff = (అడ్డువరుస - pum_bg_శిఖరం) * pum_bg_నిలువుల_సంఖ్య + నిలువు;

		    // Skip the trailing cell of a wide background char: its
		    // leading cell already emitted the full wide glyph via
		    // screen_char(); drawing here would clobber the right half.
		    // Only applies when the previous cell was actually processed
		    // in this fill range -- if the fill starts here (col ==
		    // start_col), the "wide lead" is outside our range (e.g.,
		    // popup text wrote a narrow cell there), so we fall through
		    // to the regular blend path which renders a blended space.
		    ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ
			    && నిలువు > ప్రారంభం_నిలువు
			    && pum_bg_పంక్తులు_UC[soff] == 0
			    && pum_bg_పంక్తులు_UC[soff - 1] != 0
			    && utf_char2cells(pum_bg_పంక్తులు_UC[soff - 1]) == 2)
		    {
			Screen_పంక్తులు[off] = 0;
			ఒకవేళ (Screen_పంక్తులు_UC != ఖాళీ)
			    Screen_పంక్తులు_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[off - 1];
			వెళ్ళు తదుపరి_నిలువు;
		    }

		    పూర్ణాంకం underlying_attr = pum_bg_attrs[soff];

		    // If restoring the leading cell of a wide background char
		    // would extend past the end of the fill range (e.g. into a
		    // popup border cell on the right edge), render a blended
		    // space instead so the border at col+1 is preserved.
		    // Likewise, if this cell is the trailing half of a wide
		    // background char whose leading cell is outside the fill
		    // range (e.g., popup text wrote a narrow cell there),
		    // restoring the trailing alone would emit a stray NUL.
		    ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ
			    && ((pum_bg_పంక్తులు_UC[soff] != 0
				    && utf_char2cells(pum_bg_పంక్తులు_UC[soff]) == 2
				    && నిలువు + 1 >= చివర_నిలువు)
				|| (నిలువు == ప్రారంభం_నిలువు
				    && pum_bg_పంక్తులు_UC[soff] == 0
				    && నిలువు > 0
				    && pum_bg_పంక్తులు_UC[soff - 1] != 0
				    && utf_char2cells(pum_bg_పంక్తులు_UC[soff - 1])
								       == 2)))
		    {
			Screen_పంక్తులు[off] = ' ';
			ఒకవేళ (Screen_పంక్తులు_UC != ఖాళీ)
			    Screen_పంక్తులు_UC[off] = 0;
			ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
						attr, screen_pum_blend);
			screen_char(off, అడ్డువరుస, నిలువు);
			వెళ్ళు తదుపరి_నిలువు;
		    }

		    // Restore underlying character so text shows through.
		    Screen_పంక్తులు[off] = pum_bg_పంక్తులు[soff];
		    ఒకవేళ (enc_utf8 && pum_bg_పంక్తులు_UC != ఖాళీ
						&& Screen_పంక్తులు_UC != ఖాళీ)
		    {
			పూర్ణాంకం k;
			Screen_పంక్తులు_UC[off] = pum_bg_పంక్తులు_UC[soff];
			కొరకు (k = 0; k < MAX_MCO; ++k)
			    ఒకవేళ (pum_bg_పంక్తులు_C[k] != ఖాళీ
						    && Screen_పంక్తులు_C[k] != ఖాళీ)
				Screen_పంక్తులు_C[k][off] = pum_bg_పంక్తులు_C[k][soff];
		    }
		    // Keep underlying fg, blend bg only.
		    ScreenAttrs[off] = hl_pum_blend_attr(underlying_attr,
					    attr, screen_pum_blend);
		    screen_char(off, అడ్డువరుస, నిలువు);
		    వెళ్ళు తదుపరి_నిలువు;
		}
#ఒకవేళ defined(FEAT_GUI) || defined(UNIX)
		// The bold trick may make a single row of pixels appear in
		// the next character.  When a bold character is removed, the
		// next character should be redrawn too.  This happens for our
		// own GUI and for some xterms.
		ఒకవేళ (
# ifdef FEAT_GUI
			gui.లోపల_use
# endif
# ఒకవేళ defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			పదం_is_xterm
# endif
		   )
		{
		    ఒకవేళ (Screen_పంక్తులు[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_తదుపరి = TRUE;
		    లేకపోతే
			force_తదుపరి = FALSE;
		}
#అయితే_ముగింపు // FEAT_GUI || defined(UNIX)
#నిర్వచితమైతే FEAT_DIRECTX
		// DirectWrite subpixel rendering can extend pixels
		// beyond cell boundaries.  Redraw the next character.
		ఒకవేళ (gui.directx_enabled)
		    force_తదుపరి = TRUE;
#అయితే_ముగింపు
		Screen_పంక్తులు[off] = c;
		ఒకవేళ (enc_utf8)
		{
		    ఒకవేళ (c >= 0x80)
		    {
			Screen_పంక్తులు_UC[off] = c;
			Screen_పంక్తులు_C[0][off] = 0;
			Screen_పంక్తులు[off] = 0x80; // avoid storing zero
		    }
		    లేకపోతే
			Screen_పంక్తులు_UC[off] = 0;
		}
		ScreenAttrs[off] = attr;
		ఒకవేళ (!did_delete || c != ' ')
		    screen_char(off, అడ్డువరుస, నిలువు);
	    }
తదుపరి_నిలువు:
	    Screen_నిలువుల_సంఖ్య[off] = -1;
	    ++off;
	    ఒకవేళ (నిలువు == ప్రారంభం_నిలువు)
	    {
		ఒకవేళ (did_delete)
		    ఆపు;
		c = c2;
	    }
	}
	ఒకవేళ (చివర_నిలువు == Columns)
	    పంక్తి_Wraps[అడ్డువరుస] = FALSE;
	ఒకవేళ (అడ్డువరుస == అడ్డువరుసలు - 1)		// overwritten the command line
	{
	    redraw_cmdline = TRUE;
	    ఒకవేళ (((ప్రారంభం_నిలువు == 0 && చివర_నిలువు == Columns)
			|| (ప్రారంభం_నిలువు == cmdline_నిలువు_off
			&& చివర_నిలువు == cmdline_నిలువు_off + cmdline_వెడల్పు))
		    && c1 == ' ' && c2 == ' ' && attr == 0
#నిర్వచితమైతే FEAT_PROP_POPUP
		    && !popup_overlaps_cmdline()
#అయితే_ముగింపు
		    )
		clear_cmdline = FALSE;	// command line has been cleared
	    ఒకవేళ (ప్రారంభం_నిలువు == 0 || ప్రారంభం_నిలువు == cmdline_నిలువు_off)
		బహుళకం_displayed = FALSE; // mode cleared or overwritten
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    శూన్య
check_for_delay(పూర్ణాంకం check_సందేశం_scroll)
{
    ఒకవేళ ((emsg_on_display || (check_సందేశం_scroll && సందేశం_scroll))
	    && !did_wait_return
	    && emsg_silent == 0
	    && !లోపల_assert_fails)
    {
	బహిర్గతం_flush();
	ui_delay(1006L, TRUE);
	emsg_on_display = FALSE;
	ఒకవేళ (check_సందేశం_scroll)
	    సందేశం_scroll = FALSE;
    }
}

/*
 * Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect.
 */
    స్థిర శూన్య
clear_Tab_పుట_Idxs(శూన్య)
{
    పూర్ణాంకం		scol;

    కొరకు (scol = 0; scol < Columns; ++scol)
	Tab_పుట_Idxs[scol] = 0;
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    పూర్ణాంకం
screen_valid(పూర్ణాంకం doclear)
{
    screenalloc(doclear);	   // allocate screen buffers if size changed
    తిరిగి (Screen_పంక్తులు != ఖాళీ);
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
    శూన్య
screenalloc(పూర్ణాంకం doclear)
{
    పూర్ణాంకం		    new_అడ్డువరుస, old_అడ్డువరుస;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		    old_అడ్డువరుసలు_2;
#అయితే_ముగింపు
    win_T	    *wp;
    పూర్ణాంకం		    outofmem = FALSE;
    పూర్ణాంకం		    len;
    schar_T	    *new_Screen_పంక్తులు;
    u8char_T	    *new_Screen_పంక్తులు_UC = ఖాళీ;
    u8char_T	    *new_Screen_పంక్తులు_C[MAX_MCO];
    schar_T	    *new_ScreenLines2 = ఖాళీ;
    sattr_T	    *new_ScreenAttrs;
    colnr_T	    *new_Screen_నిలువుల_సంఖ్య;
    గుర్తులేని	    *new_పంక్తి_స్థానభ్రంశం;
    char_u	    *new_పంక్తి_Wraps;
    పొట్టి	    *new_Tab_పుట_Idxs;
#నిర్వచితమైతే FEAT_PROP_POPUP
    పొట్టి	    *new_popup_ముసుగు;
    పొట్టి	    *new_popup_ముసుగు_తదుపరి;
    అక్షరం	    *new_popup_transparent;
#అయితే_ముగింపు
    tabpage_T	    *tp;
    స్థిర పూర్ణాంకం	    entered = FALSE;		// avoid recursiveness
    స్థిర పూర్ణాంకం	    పూర్తి_outofmem_సందేశం = FALSE;	// did outofmem message
    పూర్ణాంకం		    retry_లెక్క = 0;
    పూర్ణాంకం		    దొరికింది_null;

retry:
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    ఒకవేళ ((Screen_పంక్తులు != ఖాళీ
		&& అడ్డువరుసలు == screen_అడ్డువరుసలు
		&& Columns == screen_Columns
		&& enc_utf8 == (Screen_పంక్తులు_UC != ఖాళీ)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != ఖాళీ)
		&& p_mco == Screen_mco)
	    || అడ్డువరుసలు == 0
	    || Columns == 0
	    || (!నిండిన_screen && Screen_పంక్తులు == ఖాళీ))
	తిరిగి;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    ఒకవేళ (entered)
	తిరిగి;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    // fit the windows in the new sized shell

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_తాళం_screen();  // be safe, put it here
#అయితే_ముగింపు

    comp_నిలువు();		// recompute columns for shown command and ruler

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
    కొరకు (పూర్ణాంకం i = 0; i < AUCMD_WIN_లెక్క; ++i)
	ఒకవేళ (aucmd_win[i].auc_win != ఖాళీ)
	    win_free_lsize(aucmd_win[i].auc_win);
#నిర్వచితమైతే FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	win_free_lsize(wp);
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	    win_free_lsize(wp);
#అయితే_ముగింపు

    new_Screen_పంక్తులు = LALLOC_MULT(schar_T, (అడ్డువరుసలు + 1) * Columns);
    vim_memset(new_Screen_పంక్తులు_C, 0, పరిమాణం(u8char_T *) * MAX_MCO);
    ఒకవేళ (enc_utf8)
    {
	new_Screen_పంక్తులు_UC = LALLOC_MULT(u8char_T, (అడ్డువరుసలు + 1) * Columns);
	కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
	    new_Screen_పంక్తులు_C[i] = LALLOC_CLEAR_MULT(u8char_T,
							 (అడ్డువరుసలు + 1) * Columns);
    }
    ఒకవేళ (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = LALLOC_MULT(schar_T, (అడ్డువరుసలు + 1) * Columns);
    new_ScreenAttrs = LALLOC_MULT(sattr_T, (అడ్డువరుసలు + 1) * Columns);
    // Clear ScreenCols to avoid a warning for uninitialized memory in
    // jump_to_mouse().
    new_Screen_నిలువుల_సంఖ్య = LALLOC_CLEAR_MULT(colnr_T, (అడ్డువరుసలు + 1) * Columns);
    new_పంక్తి_స్థానభ్రంశం = LALLOC_MULT(గుర్తులేని, అడ్డువరుసలు);
    new_పంక్తి_Wraps = LALLOC_MULT(char_u, అడ్డువరుసలు);
    new_Tab_పుట_Idxs = LALLOC_MULT(పొట్టి, Columns);
#నిర్వచితమైతే FEAT_PROP_POPUP
    new_popup_ముసుగు = LALLOC_MULT(పొట్టి, అడ్డువరుసలు * Columns);
    new_popup_ముసుగు_తదుపరి = LALLOC_MULT(పొట్టి, అడ్డువరుసలు * Columns);
    new_popup_transparent = LALLOC_MULT(అక్షరం, అడ్డువరుసలు * Columns);
#అయితే_ముగింపు

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ఒకవేళ (win_alloc_పంక్తులు(wp) == FAIL)
	{
	    outofmem = TRUE;
	    వెళ్ళు give_పైకి;
	}
    }
    కొరకు (పూర్ణాంకం i = 0; i < AUCMD_WIN_లెక్క; ++i)
	ఒకవేళ (aucmd_win[i].auc_win != ఖాళీ
		&& aucmd_win[i].auc_win->w_పంక్తులు == ఖాళీ
		&& win_alloc_పంక్తులు(aucmd_win[i].auc_win) == FAIL)
	{
	    outofmem = TRUE;
	    ఆపు;
	}
#నిర్వచితమైతే FEAT_PROP_POPUP
    // global popup windows
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (win_alloc_పంక్తులు(wp) == FAIL)
	{
	    outofmem = TRUE;
	    వెళ్ళు give_పైకి;
	}
    // tab-local popup windows
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	    ఒకవేళ (win_alloc_పంక్తులు(wp) == FAIL)
	    {
		outofmem = TRUE;
		వెళ్ళు give_పైకి;
	    }
#అయితే_ముగింపు

give_పైకి:
    దొరికింది_null = FALSE;
    కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
	ఒకవేళ (new_Screen_పంక్తులు_C[i] == ఖాళీ)
	{
	    దొరికింది_null = TRUE;
	    ఆపు;
	}
    ఒకవేళ (new_Screen_పంక్తులు == ఖాళీ
	    || (enc_utf8 && (new_Screen_పంక్తులు_UC == ఖాళీ || దొరికింది_null))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == ఖాళీ)
	    || new_ScreenAttrs == ఖాళీ
	    || new_Screen_నిలువుల_సంఖ్య == ఖాళీ
	    || new_పంక్తి_స్థానభ్రంశం == ఖాళీ
	    || new_పంక్తి_Wraps == ఖాళీ
	    || new_Tab_పుట_Idxs == ఖాళీ
#నిర్వచితమైతే FEAT_PROP_POPUP
	    || new_popup_ముసుగు == ఖాళీ
	    || new_popup_ముసుగు_తదుపరి == ఖాళీ
	    || new_popup_transparent == ఖాళీ
#అయితే_ముగింపు
	    || outofmem)
    {
	ఒకవేళ (Screen_పంక్తులు != ఖాళీ || !పూర్తి_outofmem_సందేశం)
	{
	    // guess the size
	    do_outofmem_సందేశం((long_u)((అడ్డువరుసలు + 1) * Columns));

	    // Remember we did this to avoid getting outofmem messages over
	    // and over again.
	    పూర్తి_outofmem_సందేశం = TRUE;
	}
	VIM_CLEAR(new_Screen_పంక్తులు);
	VIM_CLEAR(new_Screen_పంక్తులు_UC);
	కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
	    VIM_CLEAR(new_Screen_పంక్తులు_C[i]);
	VIM_CLEAR(new_ScreenLines2);
	VIM_CLEAR(new_ScreenAttrs);
	VIM_CLEAR(new_Screen_నిలువుల_సంఖ్య);
	VIM_CLEAR(new_పంక్తి_స్థానభ్రంశం);
	VIM_CLEAR(new_పంక్తి_Wraps);
	VIM_CLEAR(new_Tab_పుట_Idxs);
#నిర్వచితమైతే FEAT_PROP_POPUP
	VIM_CLEAR(new_popup_ముసుగు);
	VIM_CLEAR(new_popup_ముసుగు_తదుపరి);
	VIM_CLEAR(new_popup_transparent);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	పూర్తి_outofmem_సందేశం = FALSE;

	కొరకు (new_అడ్డువరుస = 0; new_అడ్డువరుస < అడ్డువరుసలు; ++new_అడ్డువరుస)
	{
	    new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస] = new_అడ్డువరుస * Columns;
	    new_పంక్తి_Wraps[new_అడ్డువరుస] = FALSE;

	    (శూన్య)vim_memset(new_Screen_పంక్తులు + new_అడ్డువరుస * Columns,
				  ' ', (size_t)Columns * పరిమాణం(schar_T));
	    ఒకవేళ (enc_utf8)
	    {
		(శూన్య)vim_memset(new_Screen_పంక్తులు_UC + new_అడ్డువరుస * Columns,
				   0, (size_t)Columns * పరిమాణం(u8char_T));
		కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
		    (శూన్య)vim_memset(new_Screen_పంక్తులు_C[i]
						      + new_అడ్డువరుస * Columns,
				   0, (size_t)Columns * పరిమాణం(u8char_T));
	    }
	    ఒకవేళ (enc_dbcs == DBCS_JPNU)
		(శూన్య)vim_memset(new_ScreenLines2 + new_అడ్డువరుస * Columns,
				   0, (size_t)Columns * పరిమాణం(schar_T));
	    (శూన్య)vim_memset(new_ScreenAttrs + new_అడ్డువరుస * Columns,
				    0, (size_t)Columns * పరిమాణం(sattr_T));
	    (శూన్య)vim_memset(new_Screen_నిలువుల_సంఖ్య + new_అడ్డువరుస * Columns,
				    0, (size_t)Columns * పరిమాణం(colnr_T));

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    ఒకవేళ (!doclear)
	    {
		old_అడ్డువరుస = new_అడ్డువరుస + (screen_అడ్డువరుసలు - అడ్డువరుసలు);
		ఒకవేళ (old_అడ్డువరుస >= 0 && Screen_పంక్తులు != ఖాళీ)
		{
		    ఒకవేళ (screen_Columns < Columns)
			len = screen_Columns;
		    లేకపోతే
			len = Columns;
		    // When switching to utf-8 don't copy characters, they
		    // may be invalid now.  Also when p_mco changes.
		    ఒకవేళ (!(enc_utf8 && Screen_పంక్తులు_UC == ఖాళీ)
						       && p_mco == Screen_mco)
			mch_memmove(new_Screen_పంక్తులు + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
				Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
				(size_t)len * పరిమాణం(schar_T));
		    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC != ఖాళీ
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_Screen_పంక్తులు_UC + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
				Screen_పంక్తులు_UC + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
				(size_t)len * పరిమాణం(u8char_T));
			కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
			    mch_memmove(new_Screen_పంక్తులు_C[i]
						    + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
				Screen_పంక్తులు_C[i] + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
				(size_t)len * పరిమాణం(u8char_T));
		    }
		    ఒకవేళ (enc_dbcs == DBCS_JPNU && ScreenLines2 != ఖాళీ)
			mch_memmove(new_ScreenLines2 + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
				ScreenLines2 + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
				(size_t)len * పరిమాణం(schar_T));
		    mch_memmove(new_ScreenAttrs + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
			    ScreenAttrs + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
			    (size_t)len * పరిమాణం(sattr_T));
		    mch_memmove(new_Screen_నిలువుల_సంఖ్య + new_పంక్తి_స్థానభ్రంశం[new_అడ్డువరుస],
			    Screen_నిలువుల_సంఖ్య + పంక్తి_స్థానభ్రంశం[old_అడ్డువరుస],
			    (size_t)len * పరిమాణం(colnr_T));
		}
	    }
	}
	// Use the last line of the screen for the current line.
	current_Screen_పంక్తి = new_Screen_పంక్తులు + అడ్డువరుసలు * Columns;

#నిర్వచితమైతే FEAT_PROP_POPUP
	vim_memset(new_popup_ముసుగు, 0, అడ్డువరుసలు * Columns * పరిమాణం(పొట్టి));
	vim_memset(new_popup_transparent, 0, అడ్డువరుసలు * Columns * పరిమాణం(అక్షరం));
#అయితే_ముగింపు
    }

    free_screenlines();

    // NOTE: this may result in all pointers to become NULL.
    Screen_పంక్తులు = new_Screen_పంక్తులు;
    Screen_పంక్తులు_UC = new_Screen_పంక్తులు_UC;
    కొరకు (పూర్ణాంకం i = 0; i < p_mco; ++i)
	Screen_పంక్తులు_C[i] = new_Screen_పంక్తులు_C[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
    ScreenAttrs = new_ScreenAttrs;
    Screen_నిలువుల_సంఖ్య = new_Screen_నిలువుల_సంఖ్య;
    పంక్తి_స్థానభ్రంశం = new_పంక్తి_స్థానభ్రంశం;
    పంక్తి_Wraps = new_పంక్తి_Wraps;
    Tab_పుట_Idxs = new_Tab_పుట_Idxs;
#నిర్వచితమైతే FEAT_PROP_POPUP
    popup_ముసుగు = new_popup_ముసుగు;
    popup_ముసుగు_తదుపరి = new_popup_ముసుగు_తదుపరి;
    popup_transparent = new_popup_transparent;
    popup_ముసుగు_refresh = TRUE;
#అయితే_ముగింపు

    // It's important that screen_Rows and screen_Columns reflect the actual
    // size of ScreenLines[].  Set them before calling anything.
#నిర్వచితమైతే FEAT_GUI
    old_అడ్డువరుసలు_2 = screen_అడ్డువరుసలు;
#అయితే_ముగింపు
    screen_అడ్డువరుసలు = అడ్డువరుసలు;
    screen_Columns = Columns;

#నిర్వచితమైతే FEAT_GUI
    // Cursor position may now be out of bounds after resize
    ఒకవేళ (gui.లోపల_use && (gui.cursor_అడ్డువరుస >= screen_అడ్డువరుసలు
		|| gui.cursor_నిలువు >= screen_Columns))
	gui.cursor_is_valid = false;
#అయితే_ముగింపు

    సమితి_must_redraw(UPD_CLEAR);	// need to clear the screen later
    ఒకవేళ (doclear)
	screenclear2(TRUE);
#నిర్వచితమైతే FEAT_GUI
    లేకపోతే ఒకవేళ (gui.లోపల_use
	    && !gui.starting
	    && Screen_పంక్తులు != ఖాళీ
	    && old_అడ్డువరుసలు_2 != అడ్డువరుసలు)
    {
	gui_redraw_ఖండం(0, 0, (పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)Columns - 1, 0);

	// Adjust the position of the cursor, for when executing an external
	// command.
	ఒకవేళ (సందేశం_అడ్డువరుస >= అడ్డువరుసలు)		// Rows got smaller
	    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;		// put cursor at last row
	లేకపోతే ఒకవేళ (అడ్డువరుసలు > old_అడ్డువరుసలు_2)	// Rows got bigger
	    సందేశం_అడ్డువరుస += అడ్డువరుసలు - old_అడ్డువరుసలు_2; // put cursor in same place
	ఒకవేళ (సందేశం_నిలువు >= cmdline_వెడల్పు)	// cmdline_width got smaller
	    సందేశం_నిలువు = cmdline_వెడల్పు - 1;    // put cursor at last cmdline column
    }
#అయితే_ముగింపు
    clear_Tab_పుట_Idxs();

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_unlock_screen();
#అయితే_ముగింపు

    entered = FALSE;
    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    ఒకవేళ (starting == 0 && ++retry_లెక్క <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, ఖాళీ, ఖాళీ, FALSE, curbuf);
	// A shell resize also changes window sizes; trigger WinResized (and
	// WinScrolled) now instead of waiting for the next command.
	may_trigger_win_scrolled_resized();
	// In rare cases, autocommands may have altered Rows or Columns,
	// jump back to check if we need to allocate the screen again.
	వెళ్ళు retry;
    }
}

    శూన్య
free_screenlines(శూన్య)
{
    పూర్ణాంకం		i;

    VIM_CLEAR(Screen_పంక్తులు_UC);
    కొరకు (i = 0; i < Screen_mco; ++i)
	VIM_CLEAR(Screen_పంక్తులు_C[i]);
    VIM_CLEAR(ScreenLines2);
    VIM_CLEAR(Screen_పంక్తులు);
    VIM_CLEAR(ScreenAttrs);
    VIM_CLEAR(Screen_నిలువుల_సంఖ్య);
    VIM_CLEAR(పంక్తి_స్థానభ్రంశం);
    VIM_CLEAR(పంక్తి_Wraps);
    VIM_CLEAR(Tab_పుట_Idxs);
#నిర్వచితమైతే FEAT_PROP_POPUP
    VIM_CLEAR(popup_ముసుగు);
    VIM_CLEAR(popup_ముసుగు_తదుపరి);
    VIM_CLEAR(popup_transparent);
    VIM_CLEAR(suppressed_cells);
    suppressed_అడ్డువరుసలు = 0;
    suppressed_నిలువుల_సంఖ్య = 0;
#అయితే_ముగింపు
}

/*
 * Clear the screen.
 * May delay if there is something the user should read.
 * Allocated the screen for resizing if needed.
 * Returns TRUE when the screen was actually cleared, FALSE if all display
 * cells were marked for updating.
 */
    పూర్ణాంకం
screenclear(శూన్య)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);		    // allocate screen buffers if size changed
    తిరిగి screenclear2(TRUE);	    // clear the screen
}

/*
 * Do not clear the screen but mark everything for redraw.
 */
    శూన్య
redraw_as_cleared(శూన్య)
{
    screenclear2(FALSE);
}

    స్థిర పూర్ణాంకం
screenclear2(పూర్ణాంకం doclear)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    did_clear = FALSE;

    ఒకవేళ (starting == NO_SCREEN || Screen_పంక్తులు == ఖాళీ
#నిర్వచితమైతే FEAT_GUI
	    || (gui.లోపల_use && gui.starting)
#అయితే_ముగింపు
	    )
	తిరిగి FALSE;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
	screen_attr = -1;	// force setting the Normal colors
    screen_stop_highlight();	// don't want highlighting here

#నిర్వచితమైతే FEAT_CLIPBOARD
    // disable selection without redrawing it
    clip_scroll_selection(9999);
#అయితే_ముగింపు

    // blank out ScreenLines
    కొరకు (i = 0; i < అడ్డువరుసలు; ++i)
    {
	lineclear(పంక్తి_స్థానభ్రంశం[i], (పూర్ణాంకం)Columns, 0);
	పంక్తి_Wraps[i] = FALSE;
    }

    ఒకవేళ (doclear && can_clear(T_CL))
    {
	బహిర్గతం_str(T_CL);		// clear the display
	did_clear = TRUE;
	clear_cmdline = FALSE;
	బహుళకం_displayed = FALSE;
#నిర్వచితమైతే FEAT_PROP_POPUP
	// The display was cleared, the terminal matches ScreenLines again.
	ఒకవేళ (suppressed_cells != ఖాళీ)
	    vim_memset(suppressed_cells, 0,
			       (size_t)suppressed_అడ్డువరుసలు * suppressed_నిలువుల_సంఖ్య);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// can't clear the screen, mark all chars with invalid attributes
	కొరకు (i = 0; i < అడ్డువరుసలు; ++i)
	    lineinvalid(పంక్తి_స్థానభ్రంశం[i], (పూర్ణాంకం)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	// can use contents of ScreenLines now

    win_rest_invalid(firstwin);	// redraw all regular windows
    redraw_cmdline = TRUE;
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (must_redraw == UPD_CLEAR)	// no need to clear again
	must_redraw = UPD_NOT_VALID;
    సందేశం_scrolled = 0;		// compute_cmdrow() uses this
    compute_cmdrow();
#నిర్వచితమైతే FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
#అయితే_ముగింపు
    సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;	// put cursor on last line for messages
    సందేశం_నిలువు = 0;
    screen_ప్రారంభం();		// don't know where cursor is now
    సందేశం_didany = FALSE;
    సందేశం_didout = FALSE;
#నిర్వచితమైతే FEAT_IMAGE
    జరుపు_reset_image_placements(); // Reset "row_off" for every placement
    clear_all_image_placements();
#అయితే_ముగింపు

    తిరిగి did_clear;
}

/*
 * Clear one line in ScreenLines.
 */
    స్థిర శూన్య
lineclear(గుర్తులేని off, పూర్ణాంకం వెడల్పు, పూర్ణాంకం attr)
{
    (శూన్య)vim_memset(Screen_పంక్తులు + off, ' ', (size_t)వెడల్పు * పరిమాణం(schar_T));
    ఒకవేళ (enc_utf8)
	(శూన్య)vim_memset(Screen_పంక్తులు_UC + off, 0,
					  (size_t)వెడల్పు * పరిమాణం(u8char_T));
    (శూన్య)vim_memset(ScreenAttrs + off, attr, (size_t)వెడల్పు * పరిమాణం(sattr_T));
    (శూన్య)vim_memset(Screen_నిలువుల_సంఖ్య + off, -1, (size_t)వెడల్పు * పరిమాణం(colnr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    స్థిర శూన్య
lineinvalid(గుర్తులేని off, పూర్ణాంకం వెడల్పు)
{
    (శూన్య)vim_memset(ScreenAttrs + off, -1, (size_t)వెడల్పు * పరిమాణం(sattr_T));
    (శూన్య)vim_memset(Screen_నిలువుల_సంఖ్య + off, -1, (size_t)వెడల్పు * పరిమాణం(colnr_T));
}

/*
 * To be called when characters were sent to the terminal directly, outputting
 * test on "screen_lnum".
 */
    శూన్య
పంక్తి_was_clobbered(పూర్ణాంకం screen_lnum)
{
    lineinvalid(పంక్తి_స్థానభ్రంశం[screen_lnum], (పూర్ణాంకం)Columns);
}

/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    స్థిర శూన్య
linecopy(పూర్ణాంకం గమ్యం, పూర్ణాంకం from, win_T *wp)
{
    గుర్తులేని	off_గమ్యం = పంక్తి_స్థానభ్రంశం[గమ్యం] + wp->w_wincol;
    గుర్తులేని	off_from = పంక్తి_స్థానభ్రంశం[from] + wp->w_wincol;

    mch_memmove(Screen_పంక్తులు + off_గమ్యం, Screen_పంక్తులు + off_from,
	    wp->w_వెడల్పు * పరిమాణం(schar_T));
    ఒకవేళ (enc_utf8)
    {
	పూర్ణాంకం	i;

	mch_memmove(Screen_పంక్తులు_UC + off_గమ్యం, Screen_పంక్తులు_UC + off_from,
		wp->w_వెడల్పు * పరిమాణం(u8char_T));
	కొరకు (i = 0; i < p_mco; ++i)
	    mch_memmove(Screen_పంక్తులు_C[i] + off_గమ్యం, Screen_పంక్తులు_C[i] + off_from,
		    wp->w_వెడల్పు * పరిమాణం(u8char_T));
    }
    ఒకవేళ (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_గమ్యం, ScreenLines2 + off_from,
		wp->w_వెడల్పు * పరిమాణం(schar_T));
    mch_memmove(ScreenAttrs + off_గమ్యం, ScreenAttrs + off_from,
	    wp->w_వెడల్పు * పరిమాణం(sattr_T));
    mch_memmove(Screen_నిలువుల_సంఖ్య + off_గమ్యం, Screen_నిలువుల_సంఖ్య + off_from,
	    wp->w_వెడల్పు * పరిమాణం(colnr_T));
}

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 * Don't clear to end-of-line when there are popups, it may cause flicker.
 */
    పూర్ణాంకం
can_clear(char_u *p)
{
    తిరిగి (*p != NUL && (t_colors <= 1
#నిర్వచితమైతే FEAT_GUI
		|| gui.లోపల_use
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		|| (p_tgc && cterm_సాధారణ_bg_gui_color == INVALCOLOR)
		|| (!p_tgc && cterm_సాధారణ_bg_color == 0)
#లేకపోతే
		|| cterm_సాధారణ_bg_color == 0
#అయితే_ముగింపు
		|| *T_UT != NUL)
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && !(p == T_CE && popup_visible)
#అయితే_ముగింపు
	    );
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    శూన్య
screen_ప్రారంభం(శూన్య)
{
    screen_cur_అడ్డువరుస = screen_cur_నిలువు = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    శూన్య
windgoto(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    sattr_T	    *p;
    పూర్ణాంకం		    i;
    పూర్ణాంకం		    plan;
    పూర్ణాంకం		    ఖర్చు;
    పూర్ణాంకం		    wouldbe_నిలువు;
    పూర్ణాంకం		    noinvcurs;
    char_u	    *bs;
    పూర్ణాంకం		    goto_ఖర్చు;
    పూర్ణాంకం		    attr;

#నిర్వచించు GOTO_ఖర్చు   7	// assume a term_windgoto() takes about 7 chars
#నిర్వచించు HIGHL_ఖర్చు  5	// assume unhighlight takes 5 chars

#నిర్వచించు PLAN_LE	    1
#నిర్వచించు PLAN_CR	    2
#నిర్వచించు PLAN_NL	    3
#నిర్వచించు PLAN_WRITE  4
    // Can't use ScreenLines unless initialized
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ)
	తిరిగి;
    ఒకవేళ (నిలువు == screen_cur_నిలువు && అడ్డువరుస == screen_cur_అడ్డువరుస)
	తిరిగి;

    // Check for valid position.
    ఒకవేళ (అడ్డువరుస < 0)	// window without text lines?
	అడ్డువరుస = 0;
    ఒకవేళ (అడ్డువరుస >= screen_అడ్డువరుసలు)
	అడ్డువరుస = screen_అడ్డువరుసలు - 1;
    ఒకవేళ (నిలువు >= screen_Columns)
	నిలువు = screen_Columns - 1;

    // check if no cursor movement is allowed in highlight mode
    ఒకవేళ (screen_attr && *T_MS == NUL)
	noinvcurs = HIGHL_ఖర్చు;
    లేకపోతే
	noinvcurs = 0;
    goto_ఖర్చు = GOTO_ఖర్చు + noinvcurs;

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
    ఒకవేళ (అడ్డువరుస >= screen_cur_అడ్డువరుస && screen_cur_నిలువు < Columns)
    {
	/*
	 * If the cursor is in the same row, bigger col, we can use CR
	 * or T_LE.
	 */
	bs = ఖాళీ;			    // init for GCC
	attr = screen_attr;
	ఒకవేళ (అడ్డువరుస == screen_cur_అడ్డువరుస && నిలువు < screen_cur_నిలువు)
	{
	    // "le" is preferred over "bc", because "bc" is obsolete
	    ఒకవేళ (*T_LE)
		bs = T_LE;		    // "cursor left"
	    లేకపోతే
		bs = T_BC;		    // "backspace character (old)
	    ఒకవేళ (*bs)
		ఖర్చు = (screen_cur_నిలువు - నిలువు) * (పూర్ణాంకం)STRLEN(bs);
	    లేకపోతే
		ఖర్చు = 999;
	    ఒకవేళ (నిలువు + 1 < ఖర్చు)	    // using CR is less characters
	    {
		plan = PLAN_CR;
		wouldbe_నిలువు = 0;
		ఖర్చు = 1;		    // CR is just one character
	    }
	    లేకపోతే
	    {
		plan = PLAN_LE;
		wouldbe_నిలువు = నిలువు;
	    }
	    ఒకవేళ (noinvcurs)		    // will stop highlighting
	    {
		ఖర్చు += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is above where we want to be, we can use CR LF.
	 */
	లేకపోతే ఒకవేళ (అడ్డువరుస > screen_cur_అడ్డువరుస)
	{
	    plan = PLAN_NL;
	    wouldbe_నిలువు = 0;
	    ఖర్చు = (అడ్డువరుస - screen_cur_అడ్డువరుస) * 2;  // CR LF
	    ఒకవేళ (noinvcurs)		    // will stop highlighting
	    {
		ఖర్చు += noinvcurs;
		attr = 0;
	    }
	}

	/*
	 * If the cursor is in the same row, smaller col, just use write.
	 */
	లేకపోతే
	{
	    plan = PLAN_WRITE;
	    wouldbe_నిలువు = screen_cur_నిలువు;
	    ఖర్చు = 0;
	}

	/*
	 * Check if any characters that need to be written have the
	 * correct attributes.  Also avoid UTF-8 characters.
	 */
	i = నిలువు - wouldbe_నిలువు;
	ఒకవేళ (i > 0)
	    ఖర్చు += i;
	ఒకవేళ (ఖర్చు < goto_ఖర్చు && i > 0)
	{
	    /*
	     * Check if the attributes are correct without additionally
	     * stopping highlighting.
	     */
	    p = ScreenAttrs + పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + wouldbe_నిలువు;
	    అయితే (i && *p++ == attr)
		--i;
	    ఒకవేళ (i != 0)
	    {
		/*
		 * Try if it works when highlighting is stopped here.
		 */
		ఒకవేళ (*--p == 0)
		{
		    ఖర్చు += noinvcurs;
		    అయితే (i && *p++ == 0)
			--i;
		}
		ఒకవేళ (i != 0)
		    ఖర్చు = 999;	// different attributes, don't do it
	    }
	    ఒకవేళ (enc_utf8)
	    {
		// Don't use an UTF-8 char for positioning, it's slow.
		కొరకు (i = wouldbe_నిలువు; i < నిలువు; ++i)
		    ఒకవేళ (Screen_పంక్తులు_UC[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + i] != 0)
		    {
			ఖర్చు = 999;
			ఆపు;
		    }
	    }
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // Don't output characters over opacity popup cells, it
	    // would show unblended background values.
	    ఒకవేళ (ఖర్చు < 999)
		కొరకు (i = wouldbe_నిలువు; i < నిలువు; ++i)
		    ఒకవేళ (popup_is_under_opacity(అడ్డువరుస, i))
		    {
			ఖర్చు = 999;
			ఆపు;
		    }
#అయితే_ముగింపు
	}

	/*
	 * We can do it without term_windgoto()!
	 */
	ఒకవేళ (ఖర్చు < goto_ఖర్చు)
	{
	    ఒకవేళ (plan == PLAN_LE)
	    {
		ఒకవేళ (noinvcurs)
		    screen_stop_highlight();
		అయితే (screen_cur_నిలువు > నిలువు)
		{
		    బహిర్గతం_str(bs);
		    --screen_cur_నిలువు;
		}
	    }
	    లేకపోతే ఒకవేళ (plan == PLAN_CR)
	    {
		ఒకవేళ (noinvcurs)
		    screen_stop_highlight();
		బహిర్గతం_char('\r');
		screen_cur_నిలువు = 0;
	    }
	    లేకపోతే ఒకవేళ (plan == PLAN_NL)
	    {
		ఒకవేళ (noinvcurs)
		    screen_stop_highlight();
		అయితే (screen_cur_అడ్డువరుస < అడ్డువరుస)
		{
		    బహిర్గతం_char('\n');
		    ++screen_cur_అడ్డువరుస;
		}
		screen_cur_నిలువు = 0;
	    }

	    i = నిలువు - screen_cur_నిలువు;
	    ఒకవేళ (i > 0)
	    {
		/*
		 * Use cursor-right if it's one character only.  Avoids
		 * removing a line of pixels from the last bold char, when
		 * using the bold trick in the GUI.
		 */
		ఒకవేళ (T_ND[0] != NUL && T_ND[1] == NUL)
		{
		    అయితే (i-- > 0)
			బహిర్గతం_char(*T_ND);
		}
		లేకపోతే
		{
		    పూర్ణాంకం	off;

		    off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + screen_cur_నిలువు;
		    అయితే (i-- > 0)
		    {
			ఒకవేళ (ScreenAttrs[off] != screen_attr)
			    screen_stop_highlight();
			బహిర్గతం_flush_check();
			బహిర్గతం_char(Screen_పంక్తులు[off]);
			ఒకవేళ (enc_dbcs == DBCS_JPNU
				&& Screen_పంక్తులు[off] == 0x8e)
			    బహిర్గతం_char(ScreenLines2[off]);
			++off;
		    }
		}
	    }
	}
    }
    లేకపోతే
	ఖర్చు = 999;

    ఒకవేళ (ఖర్చు >= goto_ఖర్చు)
    {
	ఒకవేళ (noinvcurs)
	    screen_stop_highlight();
	ఒకవేళ (అడ్డువరుస == screen_cur_అడ్డువరుస && (నిలువు > screen_cur_నిలువు)
		&& *T_CRI != NUL)
	    పదం_cursor_కుడి(నిలువు - screen_cur_నిలువు);
	లేకపోతే
	    పదం_windgoto(అడ్డువరుస, నిలువు);
    }
    screen_cur_అడ్డువరుస = అడ్డువరుస;
    screen_cur_నిలువు = నిలువు;
}

/*
 * Set cursor to its position in the current window.
 */
    శూన్య
setcursor(శూన్య)
{
    setcursor_mayforce(FALSE);
}

#నిర్వచితమైతే FEAT_RIGHTLEFT
/*
 * Return the number of screen cells used by the character under the cursor
 * in the current window.
 */
    పూర్ణాంకం
cursor_screen_cells(శూన్య)
{
    తిరిగి has_mbyte && (*mb_ptr2cells)(ml_get_cursor()) == 2
				  && vim_isprintc(gchar_cursor()) ? 2 : 1;
}
#అయితే_ముగింపు

/*
 * Set cursor to its position in the current window.
 * When "force" is TRUE also when not redrawing.
 */
    శూన్య
setcursor_mayforce(పూర్ణాంకం force)
{
    ఒకవేళ (force || redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		curwin->w_wincol + (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		// With 'rightleft' set and the cursor on a double-wide
		// character, position it on the leftmost column.
		curwin->w_p_rl ? ((పూర్ణాంకం)curwin->w_వెడల్పు - curwin->w_wcol
					    - cursor_screen_cells()) :
#అయితే_ముగింపు
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
    పూర్ణాంకం
win_ins_పంక్తులు(
    win_T	*wp,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		పంక్తి_లెక్క,
    పూర్ణాంకం		invalid,
    పూర్ణాంకం		mayclear)
{
    పూర్ణాంకం		did_delete;
    పూర్ణాంకం		nextrow;
    పూర్ణాంకం		lastrow;
    పూర్ణాంకం		retval;

    ఒకవేళ (invalid)
	wp->w_పంక్తులు_valid = 0;

    // with only a few lines it's not worth the effort
    ఒకవేళ (wp->w_ఎత్తు < 5)
	తిరిగి FAIL;

    // with the popup menu visible this might not work correctly
    ఒకవేళ (pum_visible())
	తిరిగి FAIL;

    ఒకవేళ (పంక్తి_లెక్క > wp->w_ఎత్తు - అడ్డువరుస)
	పంక్తి_లెక్క = wp->w_ఎత్తు - అడ్డువరుస;

    retval = win_do_పంక్తులు(wp, అడ్డువరుస, పంక్తి_లెక్క, mayclear, FALSE, 0);
    ఒకవేళ (retval != MAYBE)
	తిరిగి retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to
     * avoid messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    ఒకవేళ (wp->w_తదుపరి != ఖాళీ || wp->w_స్థితి_ఎత్తు)
    {
	ఒకవేళ (screen_del_పంక్తులు(0, W_WINROW(wp) + wp->w_ఎత్తు - పంక్తి_లెక్క,
				  పంక్తి_లెక్క, (పూర్ణాంకం)అడ్డువరుసలు, FALSE, 0, ఖాళీ) == OK)
	    did_delete = TRUE;
	లేకపోతే ఒకవేళ (wp->w_తదుపరి)
	    తిరిగి FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    ఒకవేళ (!did_delete)
    {
	wp->w_redr_స్థితి = true;
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు;
	lastrow = nextrow + పంక్తి_లెక్క;
	ఒకవేళ (lastrow > అడ్డువరుసలు)
	    lastrow = అడ్డువరుసలు;
	screen_fill(nextrow - పంక్తి_లెక్క, lastrow - పంక్తి_లెక్క,
		  wp->w_wincol, (పూర్ణాంకం)W_ENDCOL(wp), ' ', ' ', 0);
    }

    ఒకవేళ (screen_ins_పంక్తులు(0, W_WINROW(wp) + అడ్డువరుస, పంక్తి_లెక్క, (పూర్ణాంకం)అడ్డువరుసలు, 0, ఖాళీ)
								      == FAIL)
    {
	// deletion will have messed up other windows
	ఒకవేళ (did_delete)
	{
	    wp->w_redr_స్థితి = true;
	    win_rest_invalid(W_తదుపరి(wp));
	}
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Delete "line_count" window lines at "row" in window "wp".
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    పూర్ణాంకం
win_del_పంక్తులు(
    win_T	*wp,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		పంక్తి_లెక్క,
    పూర్ణాంకం		invalid,
    పూర్ణాంకం		mayclear,
    పూర్ణాంకం		clear_attr)	    // for clearing lines
{
    పూర్ణాంకం		retval;

    ఒకవేళ (invalid)
	wp->w_పంక్తులు_valid = 0;

    ఒకవేళ (పంక్తి_లెక్క > wp->w_ఎత్తు - అడ్డువరుస)
	పంక్తి_లెక్క = wp->w_ఎత్తు - అడ్డువరుస;

    retval = win_do_పంక్తులు(wp, అడ్డువరుస, పంక్తి_లెక్క, mayclear, TRUE, clear_attr);
    ఒకవేళ (retval != MAYBE)
	తిరిగి retval;

    ఒకవేళ (screen_del_పంక్తులు(0, W_WINROW(wp) + అడ్డువరుస, పంక్తి_లెక్క,
				   (పూర్ణాంకం)అడ్డువరుసలు, FALSE, clear_attr, ఖాళీ) == FAIL)
	తిరిగి FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    ఒకవేళ (wp->w_తదుపరి || wp->w_స్థితి_ఎత్తు || cmdline_అడ్డువరుస < అడ్డువరుసలు - 1)
    {
	ఒకవేళ (screen_ins_పంక్తులు(0, W_WINROW(wp) + wp->w_ఎత్తు - పంక్తి_లెక్క,
			      పంక్తి_లెక్క, (పూర్ణాంకం)అడ్డువరుసలు, clear_attr, ఖాళీ) == FAIL)
	{
	    wp->w_redr_స్థితి = true;
	    win_rest_invalid(wp->w_తదుపరి);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    లేకపోతే
	redraw_cmdline = TRUE;
    తిరిగి OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    స్థిర పూర్ణాంకం
win_do_పంక్తులు(
    win_T	*wp,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		పంక్తి_లెక్క,
    పూర్ణాంకం		mayclear,
    పూర్ణాంకం		del,
    పూర్ణాంకం		clear_attr)
{
    పూర్ణాంకం		retval;

    ఒకవేళ (!redrawing() || పంక్తి_లెక్క <= 0)
	తిరిగి FAIL;

    // When inserting lines would result in loss of command output, just redraw
    // the lines.
    ఒకవేళ (no_win_do_పంక్తులు_ins && !del)
	తిరిగి FAIL;

    // only a few lines left: redraw is faster
    ఒకవేళ (mayclear && అడ్డువరుసలు - పంక్తి_లెక్క < 5
	    && wp->w_వెడల్పు == topframe->fr_వెడల్పు)
    {
	ఒకవేళ (!no_win_do_పంక్తులు_ins)
	    redraw_as_cleared();    // don't clear the screen to avoid flicker
	తిరిగి FAIL;
    }

#నిర్వచితమైతే FEAT_PROP_POPUP
    // this doesn't work when there are popups visible
    ఒకవేళ (popup_visible)
	తిరిగి FAIL;
#అయితే_ముగింపు

    // Delete all remaining lines
    ఒకవేళ (అడ్డువరుస + పంక్తి_లెక్క >= wp->w_ఎత్తు)
    {
	screen_fill(W_WINROW(wp) + అడ్డువరుస, W_WINROW(wp) + wp->w_ఎత్తు,
		wp->w_wincol, (పూర్ణాంకం)W_ENDCOL(wp), ' ', ' ', 0);
	తిరిగి OK;
    }

    /*
     * When scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     * Don't do this when avoiding to insert lines.
     */
    ఒకవేళ (!no_win_do_పంక్తులు_ins)
	clear_cmdline = TRUE;

#ఒకవేళ defined(FEAT_TABPANEL)
    // Terminal scroll operations affect the full screen width, which would
    // corrupt the vertical tabpanel area and cause flicker.
    ఒకవేళ (tabpanel_వెడల్పు() > 0)
	తిరిగి FAIL;
#అయితే_ముగింపు

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    ఒకవేళ (scroll_region || wp->w_వెడల్పు != topframe->fr_వెడల్పు)
    {
	ఒకవేళ (scroll_region && (wp->w_వెడల్పు == topframe->fr_వెడల్పు
		    || *T_CSV != NUL))
	    scroll_region_సమితి(wp, అడ్డువరుస);
	ఒకవేళ (del)
	    retval = screen_del_పంక్తులు(W_WINROW(wp) + అడ్డువరుస, 0, పంక్తి_లెక్క,
				    wp->w_ఎత్తు - అడ్డువరుస, FALSE, clear_attr, wp);
	లేకపోతే
	    retval = screen_ins_పంక్తులు(W_WINROW(wp) + అడ్డువరుస, 0, పంక్తి_లెక్క,
					   wp->w_ఎత్తు - అడ్డువరుస, clear_attr, wp);
	ఒకవేళ (scroll_region && (wp->w_వెడల్పు == topframe->fr_వెడల్పు
		    || *T_CSV != NUL))
	    scroll_region_reset();
	తిరిగి retval;
    }

    ఒకవేళ (wp->w_తదుపరి != ఖాళీ && p_tf) // don't delete/insert on fast terminal
	తిరిగి FAIL;

    తిరిగి MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    స్థిర శూన్య
win_rest_invalid(win_T *wp)
{
    అయితే (wp != ఖాళీ)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	wp->w_redr_స్థితి = true;
	wp = wp->w_తదుపరి;
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
#నిర్వచించు USE_T_CAL   1
#నిర్వచించు USE_T_CDL   2
#నిర్వచించు USE_T_AL    3
#నిర్వచించు USE_T_CE    4
#నిర్వచించు USE_T_DL    5
#నిర్వచించు USE_T_SR    6
#నిర్వచించు USE_NL	    7
#నిర్వచించు USE_T_CD    8
#నిర్వచించు USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    పూర్ణాంకం
screen_ins_పంక్తులు(
    పూర్ణాంకం		off,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		పంక్తి_లెక్క,
    పూర్ణాంకం		చివర,
    పూర్ణాంకం		clear_attr,
    win_T	*wp)	    // NULL or window to use width from
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    గుర్తులేని	తాత్కాలిక;
    పూర్ణాంకం		cursor_అడ్డువరుస;
    పూర్ణాంకం		cursor_నిలువు = 0;
    పూర్ణాంకం		type;
    పూర్ణాంకం		ఫలితం_ఖాళీలు;
    పూర్ణాంకం		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     * - there is a popup window
     */
    ఒకవేళ (!screen_valid(TRUE)
	     || పంక్తి_లెక్క <= 0 || పంక్తి_లెక్క > p_ttyscroll
	     || చివర > అడ్డువరుసలు
#నిర్వచితమైతే FEAT_CLIPBOARD
	     || (clip_నక్షత్రం.state != SELECT_CLEARED
						 && redrawing_for_callback > 0)
#అయితే_ముగింపు
	     )
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_visible)
    {
# ifdef FEAT_IMAGE
	// Always make sure images are correctly shifted
	జరుపు_image_placements(off + అడ్డువరుస, off + చివర, పంక్తి_లెక్క);
# endif
	తిరిగి FAIL;
    }
#అయితే_ముగింపు

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
    ఫలితం_ఖాళీలు = (అడ్డువరుస + పంక్తి_లెక్క >= చివర);
    ఒకవేళ (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ఒకవేళ (పంక్తి_లెక్క > 3)
	    తిరిగి FAIL;
	type = USE_REDRAW;
    }
    లేకపోతే ఒకవేళ (can_clear(T_CD) && ఫలితం_ఖాళీలు)
	type = USE_T_CD;
    లేకపోతే ఒకవేళ (*T_CAL != NUL && (పంక్తి_లెక్క > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    లేకపోతే ఒకవేళ (*T_CDL != NUL && ఫలితం_ఖాళీలు && (పంక్తి_లెక్క > 1 || !can_ce))
	type = USE_T_CDL;
    లేకపోతే ఒకవేళ (*T_AL != NUL)
	type = USE_T_AL;
    లేకపోతే ఒకవేళ (can_ce && ఫలితం_ఖాళీలు)
	type = USE_T_CE;
    లేకపోతే ఒకవేళ (*T_DL != NUL && ఫలితం_ఖాళీలు)
	type = USE_T_DL;
    లేకపోతే ఒకవేళ (*T_SR != NUL && అడ్డువరుస == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    లేకపోతే
	తిరిగి FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    ఒకవేళ (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	తిరిగి screen_del_పంక్తులు(off, అడ్డువరుస, పంక్తి_లెక్క, చివర, FALSE, 0, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    ఒకవేళ (*T_DB)
	screen_del_పంక్తులు(off, చివర - పంక్తి_లెక్క, పంక్తి_లెక్క, చివర, FALSE, 0, wp);

#నిర్వచితమైతే FEAT_CLIPBOARD
    // Remove a modeless selection when inserting lines halfway the screen
    // or not the full width of the screen.
    ఒకవేళ (off + అడ్డువరుస > 0 || (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు))
	clip_clear_selection(&clip_నక్షత్రం);
    లేకపోతే
	clip_scroll_selection(-పంక్తి_లెక్క);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
    జరుపు_image_placements(off + అడ్డువరుస, off + చివర, పంక్తి_లెక్క);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_తాళం_screen();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(అడ్డువరుస + off <= gui.cursor_అడ్డువరుస);
#అయితే_ముగింపు

    ఒకవేళ (wp != ఖాళీ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_నిలువు = wp->w_wincol;

    ఒకవేళ (*T_CCS != NUL)	   // cursor relative to region
	cursor_అడ్డువరుస = అడ్డువరుస;
    లేకపోతే
	cursor_అడ్డువరుస = అడ్డువరుస + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    అడ్డువరుస += off;
    చివర += off;
    కొరకు (i = 0; i < పంక్తి_లెక్క; ++i)
    {
	ఒకవేళ (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు)
	{
	    // need to copy part of a line
	    j = చివర - 1 - i;
	    అయితే ((j -= పంక్తి_లెక్క) >= అడ్డువరుస)
		linecopy(j + పంక్తి_లెక్క, j, wp);
	    j += పంక్తి_లెక్క;
	    ఒకవేళ (can_clear((char_u *)" "))
		lineclear(పంక్తి_స్థానభ్రంశం[j] + wp->w_wincol,
			wp->w_వెడల్పు, clear_attr);
	    లేకపోతే
		lineinvalid(పంక్తి_స్థానభ్రంశం[j] + wp->w_wincol, wp->w_వెడల్పు);
	    పంక్తి_Wraps[j] = FALSE;
	}
	లేకపోతే
	{
	    j = చివర - 1 - i;
	    తాత్కాలిక = పంక్తి_స్థానభ్రంశం[j];
	    అయితే ((j -= పంక్తి_లెక్క) >= అడ్డువరుస)
	    {
		పంక్తి_స్థానభ్రంశం[j + పంక్తి_లెక్క] = పంక్తి_స్థానభ్రంశం[j];
		పంక్తి_Wraps[j + పంక్తి_లెక్క] = పంక్తి_Wraps[j];
	    }
	    పంక్తి_స్థానభ్రంశం[j + పంక్తి_లెక్క] = తాత్కాలిక;
	    పంక్తి_Wraps[j + పంక్తి_లెక్క] = FALSE;
	    ఒకవేళ (can_clear((char_u *)" "))
		lineclear(తాత్కాలిక, (పూర్ణాంకం)Columns, clear_attr);
	    లేకపోతే
		lineinvalid(తాత్కాలిక, (పూర్ణాంకం)Columns);
	}
    }

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_unlock_screen();
#అయితే_ముగింపు

    screen_stop_highlight();
    windgoto(cursor_అడ్డువరుస, cursor_నిలువు);
    ఒకవేళ (clear_attr != 0)
	screen_ప్రారంభం_highlight(clear_attr);

    // redraw the characters
    ఒకవేళ (type == USE_REDRAW)
	redraw_ఖండం(అడ్డువరుస, చివర, wp);
    లేకపోతే ఒకవేళ (type == USE_T_CAL)
    {
	పదం_append_పంక్తులు(పంక్తి_లెక్క);
	screen_ప్రారంభం();		// don't know where cursor is now
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < పంక్తి_లెక్క; i++)
	{
	    ఒకవేళ (type == USE_T_AL)
	    {
		ఒకవేళ (i && cursor_అడ్డువరుస != 0)
		    windgoto(cursor_అడ్డువరుస, cursor_నిలువు);
		బహిర్గతం_str(T_AL);
	    }
	    లేకపోతే  // type == USE_T_SR
		బహిర్గతం_str(T_SR);
	    screen_ప్రారంభం();	    // don't know where cursor is now
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    ఒకవేళ (type == USE_T_SR && *T_DA)
    {
	కొరకు (i = 0; i < పంక్తి_లెక్క; ++i)
	{
	    windgoto(off + i, cursor_నిలువు);
	    బహిర్గతం_str(T_CE);
	    screen_ప్రారంభం();	    // don't know where cursor is now
	}
    }

#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    gui_can_update_cursor();
    ఒకవేళ (gui.లోపల_use)
	బహిర్గతం_flush();	// always flush after a scroll
#అయితే_ముగింపు
    తిరిగి OK;
}

/*
 * Delete lines on the screen and update ScreenLines[].
 * "end" is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used "off" is the offset from the top for the region.
 * "row" and "end" are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    పూర్ణాంకం
screen_del_పంక్తులు(
    పూర్ణాంకం		off,
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		పంక్తి_లెక్క,
    పూర్ణాంకం		చివర,
    పూర్ణాంకం		force,		// even when line_count > p_ttyscroll
    పూర్ణాంకం		clear_attr,	// used for clearing lines
    win_T	*wp)		// NULL or window to use width from
{
    పూర్ణాంకం		j;
    పూర్ణాంకం		i;
    గుర్తులేని	తాత్కాలిక;
    పూర్ణాంకం		cursor_అడ్డువరుస;
    పూర్ణాంకం		cursor_నిలువు = 0;
    పూర్ణాంకం		cursor_చివర;
    పూర్ణాంకం		ఫలితం_ఖాళీలు;	// result is empty until end of region
    పూర్ణాంకం		can_delete;	// deleting line codes can be used
    పూర్ణాంకం		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     * - "end" is more than "Rows" (safety check, should not happen)
     * - redrawing for a callback and there is a modeless selection
     */
    ఒకవేళ (!screen_valid(TRUE)
	    || పంక్తి_లెక్క <= 0
	    || (!force && పంక్తి_లెక్క > p_ttyscroll)
	    || చివర > అడ్డువరుసలు
#నిర్వచితమైతే FEAT_CLIPBOARD
	    || (clip_నక్షత్రం.state != SELECT_CLEARED && redrawing_for_callback > 0)
#అయితే_ముగింపు
       )
	తిరిగి FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    ఫలితం_ఖాళీలు = అడ్డువరుస + పంక్తి_లెక్క >= చివర;

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
    ఒకవేళ (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు && *T_CSV == NUL)
    {
	// Avoid that lines are first cleared here and then redrawn, which
	// results in many characters updated twice.  This happens with CTRL-F
	// in a vertically split window.  With line-by-line scrolling
	// USE_REDRAW should be faster.
	ఒకవేళ (పంక్తి_లెక్క > 3)
	    తిరిగి FAIL;
	type = USE_REDRAW;
    }
    లేకపోతే ఒకవేళ (can_clear(T_CD) && ఫలితం_ఖాళీలు)
	type = USE_T_CD;
    లేకపోతే ఒకవేళ (అడ్డువరుస == 0 && (
#నిర్వచితం_కాకపోతే AMIGA
	// On the Amiga, somehow '\n' on the last line doesn't always scroll
	// up, so use delete-line command
			    పంక్తి_లెక్క == 1 ||
#అయితే_ముగింపు
						*T_CDL == NUL))
	type = USE_NL;
    లేకపోతే ఒకవేళ (*T_CDL != NUL && పంక్తి_లెక్క > 1 && can_delete)
	type = USE_T_CDL;
    లేకపోతే ఒకవేళ (can_clear(T_CE) && ఫలితం_ఖాళీలు
	    && (wp == ఖాళీ || wp->w_వెడల్పు == topframe->fr_వెడల్పు))
	type = USE_T_CE;
    లేకపోతే ఒకవేళ (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    లేకపోతే ఒకవేళ (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    లేకపోతే
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_CLIPBOARD
    // Remove a modeless selection when deleting lines halfway the screen or
    // not the full width of the screen.
    ఒకవేళ (off + అడ్డువరుస > 0 || (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు))
	clip_clear_selection(&clip_నక్షత్రం);
    లేకపోతే
	clip_scroll_selection(పంక్తి_లెక్క);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
    జరుపు_image_placements(off + అడ్డువరుస, off + చివర, -పంక్తి_లెక్క);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_తాళం_screen();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    // Don't update the GUI cursor here, ScreenLines[] is invalid until the
    // scrolling is actually carried out.
    gui_dont_update_cursor(gui.cursor_అడ్డువరుస >= అడ్డువరుస + off
						&& gui.cursor_అడ్డువరుస < చివర + off);
#అయితే_ముగింపు

    ఒకవేళ (wp != ఖాళీ && wp->w_wincol != 0 && *T_CSV != NUL && *T_CCS == NUL)
	cursor_నిలువు = wp->w_wincol;

    ఒకవేళ (*T_CCS != NUL)	    // cursor relative to region
    {
	cursor_అడ్డువరుస = అడ్డువరుస;
	cursor_చివర = చివర;
    }
    లేకపోతే
    {
	cursor_అడ్డువరుస = అడ్డువరుస + off;
	cursor_చివర = చివర + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    అడ్డువరుస += off;
    చివర += off;
    కొరకు (i = 0; i < పంక్తి_లెక్క; ++i)
    {
	ఒకవేళ (wp != ఖాళీ && wp->w_వెడల్పు != topframe->fr_వెడల్పు)
	{
	    // need to copy part of a line
	    j = అడ్డువరుస + i;
	    అయితే ((j += పంక్తి_లెక్క) <= చివర - 1)
		linecopy(j - పంక్తి_లెక్క, j, wp);
	    j -= పంక్తి_లెక్క;
	    ఒకవేళ (can_clear((char_u *)" "))
		lineclear(పంక్తి_స్థానభ్రంశం[j] + wp->w_wincol,
			wp->w_వెడల్పు, clear_attr);
	    లేకపోతే
		lineinvalid(పంక్తి_స్థానభ్రంశం[j] + wp->w_wincol, wp->w_వెడల్పు);
	    పంక్తి_Wraps[j] = FALSE;
	}
	లేకపోతే
	{
	    // whole width, moving the line pointers is faster
	    j = అడ్డువరుస + i;
	    తాత్కాలిక = పంక్తి_స్థానభ్రంశం[j];
	    అయితే ((j += పంక్తి_లెక్క) <= చివర - 1)
	    {
		పంక్తి_స్థానభ్రంశం[j - పంక్తి_లెక్క] = పంక్తి_స్థానభ్రంశం[j];
		పంక్తి_Wraps[j - పంక్తి_లెక్క] = పంక్తి_Wraps[j];
	    }
	    పంక్తి_స్థానభ్రంశం[j - పంక్తి_లెక్క] = తాత్కాలిక;
	    పంక్తి_Wraps[j - పంక్తి_లెక్క] = FALSE;
	    ఒకవేళ (can_clear((char_u *)" "))
		lineclear(తాత్కాలిక, (పూర్ణాంకం)Columns, clear_attr);
	    లేకపోతే
		lineinvalid(తాత్కాలిక, (పూర్ణాంకం)Columns);
	}
    }

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_unlock_screen();
#అయితే_ముగింపు

    ఒకవేళ (screen_attr != clear_attr)
	screen_stop_highlight();
    ఒకవేళ (clear_attr != 0)
	screen_ప్రారంభం_highlight(clear_attr);

    // redraw the characters
    ఒకవేళ (type == USE_REDRAW)
	redraw_ఖండం(అడ్డువరుస, చివర, wp);
    లేకపోతే ఒకవేళ (type == USE_T_CD)	// delete the lines
    {
	windgoto(cursor_అడ్డువరుస, cursor_నిలువు);
	బహిర్గతం_str(T_CD);
	screen_ప్రారంభం();			// don't know where cursor is now
    }
    లేకపోతే ఒకవేళ (type == USE_T_CDL)
    {
	windgoto(cursor_అడ్డువరుస, cursor_నిలువు);
	పదం_delete_పంక్తులు(పంక్తి_లెక్క);
	screen_ప్రారంభం();			// don't know where cursor is now
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    లేకపోతే ఒకవేళ (type == USE_NL)
    {
	windgoto(cursor_చివర - 1, cursor_నిలువు);
	కొరకు (i = పంక్తి_లెక్క; --i >= 0; )
	    బహిర్గతం_char('\n');		// cursor will remain on same line
    }
    లేకపోతే
    {
	కొరకు (i = పంక్తి_లెక్క; --i >= 0; )
	{
	    ఒకవేళ (type == USE_T_DL)
	    {
		windgoto(cursor_అడ్డువరుస, cursor_నిలువు);
		బహిర్గతం_str(T_DL);		// delete a line
	    }
	    లేకపోతే // type == USE_T_CE
	    {
		windgoto(cursor_అడ్డువరుస + i, cursor_నిలువు);
		బహిర్గతం_str(T_CE);		// erase a line
	    }
	    screen_ప్రారంభం();		// don't know where cursor is now
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    ఒకవేళ (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	కొరకు (i = పంక్తి_లెక్క; i > 0; --i)
	{
	    windgoto(cursor_చివర - i, cursor_నిలువు);
	    బహిర్గతం_str(T_CE);		// erase a line
	    screen_ప్రారంభం();		// don't know where cursor is now
	}
    }

#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    gui_can_update_cursor();
    ఒకవేళ (gui.లోపల_use)
	బహిర్గతం_flush();	// always flush after a scroll
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Return TRUE when postponing displaying the mode message: when not redrawing
 * or inside a mapping or a script.
 */
    పూర్ణాంకం
skip_showmode(శూన్య)
{
    // Check the stuff buffer, typeahead buffer and script input for pending
    // characters, instead of char_avail() which also reads raw terminal input
    // and may pick up terminal response sequences (e.g. t_RV response),
    // falsely preventing the mode from being shown.
    ఒకవేళ (global_busy
	    || సందేశం_silent != 0
	    || !redrawing()
	    || ((!stuff_ఖాళీలు() || typebuf.tb_len > 0 || using_script())
		&& !కీలకం_Typed))
    {
	redraw_బహుళకం = TRUE;		// show mode later
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
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
    పూర్ణాంకం
showmode(శూన్య)
{
    పూర్ణాంకం		need_clear;
    పూర్ణాంకం		నిడివి = 0;
    పూర్ణాంకం		do_బహుళకం;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		nwr_save;
    పూర్ణాంకం		sub_attr;
    పూర్ణాంకం		show_ruler_with_pum = FALSE;

    do_బహుళకం = p_smd && సందేశం_silent == 0
	    && ((State & బహుళకం_చొప్పించు)
		|| restart_edit != NUL
		|| VIsual_చురుకైన);
    ఒకవేళ (do_బహుళకం || reg_recording != 0)
    {
	ఒకవేళ (skip_showmode())
	    తిరిగి 0;		// show mode later

	nwr_save = need_wait_return;

	// wait a bit before overwriting an important message
	check_for_delay(FALSE);

	// if the cmdline is more than one line high, erase top lines
	need_clear = clear_cmdline;
	ఒకవేళ (clear_cmdline && cmdline_అడ్డువరుస < అడ్డువరుసలు - 1)
	    సందేశం_clr_cmdline();			// will reset clear_cmdline

	// Position on the last line in the window, column 0
	సందేశం_pos_బహుళకం();
	cursor_off();
	attr = HL_ATTR(HLF_CM);			// Highlight mode
	ఒకవేళ (do_బహుళకం)
	{
	    సందేశం_puts_attr("--", attr);
#ఒకవేళ defined(FEAT_XIM)
	    ఒకవేళ (
# ifdef FEAT_GUI_GTK
		    preedit_get_స్థితి()
# లేకపోతే
		    im_get_స్థితి()
# endif
	       )
# ifdef FEAT_GUI_GTK // most of the time, it's not XIM being used
		సందేశం_puts_attr(" IM", attr);
# లేకపోతే
		సందేశం_puts_attr(" XIM", attr);
# endif
#అయితే_ముగింపు
	    // CTRL-X in Insert mode
	    ఒకవేళ (edit_submode != ఖాళీ && !shortmess(SHM_COMPLETIONMENU))
	    {
		// These messages can get long, avoid a wrap in a narrow
		// window.  Prefer showing edit_submode_extra.
		నిడివి = (అడ్డువరుసలు - సందేశం_అడ్డువరుస) * cmdline_వెడల్పు - 3;
		ఒకవేళ (edit_submode_extra != ఖాళీ)
		    నిడివి -= vim_strsize(edit_submode_extra);
		ఒకవేళ (నిడివి > 0)
		{
		    ఒకవేళ (edit_submode_pre != ఖాళీ)
			నిడివి -= vim_strsize(edit_submode_pre);
		    ఒకవేళ (నిడివి - vim_strsize(edit_submode) > 0)
		    {
			ఒకవేళ (edit_submode_pre != ఖాళీ)
			    సందేశం_puts_attr((అక్షరం *)edit_submode_pre, attr);
			సందేశం_puts_attr((అక్షరం *)edit_submode, attr);
		    }
		    ఒకవేళ (edit_submode_extra != ఖాళీ)
		    {
			సందేశం_puts_attr(" ", attr);  // add a space in between
			ఒకవేళ ((పూర్ణాంకం)edit_submode_highl < (పూర్ణాంకం)HLF_లెక్క)
			    sub_attr = HL_ATTR(edit_submode_highl);
			లేకపోతే
			    sub_attr = attr;
			సందేశం_puts_attr((అక్షరం *)edit_submode_extra, sub_attr);
		    }
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (State & VREPLACE_FLAG)
		    సందేశం_puts_attr(_(" VREPLACE"), attr);
		లేకపోతే ఒకవేళ (State & REPLACE_FLAG)
		    సందేశం_puts_attr(_(" REPLACE"), attr);
		లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
		{
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    ఒకవేళ (p_ri)
			సందేశం_puts_attr(_(" REVERSE"), attr);
#అయితే_ముగింపు
		    సందేశం_puts_attr(_(" INSERT"), attr);
		}
		లేకపోతే ఒకవేళ (restart_edit == 'I' || restart_edit == 'i' ||
			restart_edit == 'a' || restart_edit == 'A')
		    సందేశం_puts_attr(_(" (insert)"), attr);
		లేకపోతే ఒకవేళ (restart_edit == 'R')
		    సందేశం_puts_attr(_(" (replace)"), attr);
		లేకపోతే ఒకవేళ (restart_edit == 'V')
		    సందేశం_puts_attr(_(" (vreplace)"), attr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (p_hkmap)
		    సందేశం_puts_attr(_(" Hebrew"), attr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
		ఒకవేళ (State & బహుళకం_LANGMAP)
		{
# ifdef FEAT_ARABIC
		    ఒకవేళ (curwin->w_p_arab)
			సందేశం_puts_attr(_(" Arabic"), attr);
		    లేకపోతే
# endif
			ఒకవేళ (get_keymap_str(curwin, (char_u *)" (%s)",
							   NameBuff, MAXPATHL) > 0)
			    సందేశం_puts_attr((అక్షరం *)NameBuff, attr);
		}
#అయితే_ముగింపు
		ఒకవేళ ((State & బహుళకం_చొప్పించు) && p_paste)
		    సందేశం_puts_attr(_(" (paste)"), attr);

		ఒకవేళ (VIsual_చురుకైన)
		{
		    అక్షరం *p;

		    // Don't concatenate separate words to avoid translation
		    // problems.
		    ఎంపిక ((VIsual_select ? 4 : 0)
			    + (VIsual_బహుళకం == Ctrl_V) * 2
			    + (VIsual_బహుళకం == 'V'))
		    {
			స్థితి 0:	p = N_(" VISUAL"); ఆపు;
			స్థితి 1: p = N_(" VISUAL LINE"); ఆపు;
			స్థితి 2: p = N_(" VISUAL BLOCK"); ఆపు;
			స్థితి 4: p = N_(" SELECT"); ఆపు;
			స్థితి 5: p = N_(" SELECT LINE"); ఆపు;
			అప్రమేయ: p = N_(" SELECT BLOCK"); ఆపు;
		    }
		    సందేశం_puts_attr(_(p), attr);
		}
		సందేశం_puts_attr(" --", attr);
		// Ensure ruler is shown when a popup is visible and only the mode name
		// is displayed. Without this, the ruler may disappear during insert-mode
		// completion when 'shortmess' includes 'c'.
		show_ruler_with_pum = TRUE;
	    }

	    need_clear = TRUE;
	}
	ఒకవేళ (reg_recording != 0
		&& edit_submode == ఖాళీ)    // otherwise it gets too long
	{
	    recording_బహుళకం(attr);
	    need_clear = TRUE;
	}

	బహుళకం_displayed = TRUE;
	ఒకవేళ (need_clear || clear_cmdline || redraw_బహుళకం)
	    సందేశం_clr_eos();
	సందేశం_didout = FALSE;		// overwrite this message
	నిడివి = సందేశం_నిలువు;
	సందేశం_నిలువు = 0;
	need_wait_return = nwr_save;	// never ask for hit-return for this
    }
    లేకపోతే ఒకవేళ (clear_cmdline && సందేశం_silent == 0)
	// Clear the whole command line.  Will reset "clear_cmdline".
	సందేశం_clr_cmdline();
    లేకపోతే ఒకవేళ (redraw_బహుళకం)
    {
	సందేశం_pos_బహుళకం();
	సందేశం_clr_eos();
    }

    // In Visual mode the size of the selected area must be redrawn.
    ఒకవేళ (VIsual_చురుకైన)
	clear_showcmd();

    // If the last window has no status line, the ruler is after the mode
    // message and must be redrawn
    ఒకవేళ (redrawing() && lastwin->w_స్థితి_ఎత్తు == 0)
	win_redr_ruler(lastwin, TRUE, show_ruler_with_pum);

    redraw_cmdline = FALSE;
    redraw_బహుళకం = FALSE;
    clear_cmdline = FALSE;

    తిరిగి నిడివి;
}

/*
 * Position for a mode message.
 */
    స్థిర శూన్య
సందేశం_pos_బహుళకం(శూన్య)
{
    సందేశం_నిలువు = 0;
    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    శూన్య
unshowmode(పూర్ణాంకం force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    ఒకవేళ (!redrawing() || (!force && char_avail() && !కీలకం_Typed))
	redraw_cmdline = TRUE;		// delete mode later
    లేకపోతే
	clearmode();
}

/*
 * Clear the mode message.
 */
    శూన్య
clearmode(శూన్య)
{
    పూర్ణాంకం save_సందేశం_అడ్డువరుస = సందేశం_అడ్డువరుస;
    పూర్ణాంకం save_సందేశం_నిలువు = సందేశం_నిలువు;

    సందేశం_pos_బహుళకం();
    ఒకవేళ (reg_recording != 0)
	recording_బహుళకం(HL_ATTR(HLF_CM));
    సందేశం_clr_eos();

    సందేశం_నిలువు = save_సందేశం_నిలువు;
    సందేశం_అడ్డువరుస = save_సందేశం_అడ్డువరుస;
}

    స్థిర శూన్య
recording_బహుళకం(పూర్ణాంకం attr)
{
    సందేశం_puts_attr(_("recording"), attr);
    ఒకవేళ (shortmess(SHM_RECORDING))
	తిరిగి;

    అక్షరం s[4];

    sprintf(s, " @%c", reg_recording);
    సందేశం_puts_attr(s, attr);
}

/*
 * Return TRUE if mouse is enabled.
 */
    స్థిర పూర్ణాంకం
mouse_has_any(శూన్య)
{
    తిరిగి mouse_has(MOUSE_సాధారణ) || mouse_has(MOUSE_చొప్పించు)
	|| mouse_has(MOUSE_VISUAL);
}

/*
 * Draw the tab pages line at the top of the Vim window.
 */
    శూన్య
draw_tabline(శూన్య)
{
    పూర్ణాంకం		tabcount = 0;
    tabpage_T	*tp;
    పూర్ణాంకం		tabwidth;
    పూర్ణాంకం		నిలువు = 0;
    పూర్ణాంకం		scol = 0;
    పూర్ణాంకం		attr;
    win_T	*wp;
    win_T	*cwp;
    పూర్ణాంకం		wincount;
    పూర్ణాంకం		modified;
    పూర్ణాంకం		c;
    పూర్ణాంకం		len;
    పూర్ణాంకం		attr_sel;
    పూర్ణాంకం		attr_nosel = 0;
    పూర్ణాంకం		attr_fill = 0;
    char_u	*p;
    పూర్ణాంకం		స్థలం;
    పూర్ణాంకం		use_sep_chars = (t_colors < 8
#నిర్వచితమైతే FEAT_GUI
					    && !gui.లోపల_use
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMGUICOLORS
					    && !p_tgc
#అయితే_ముగింపు
					    );
    bool	override_success;

#ఒకవేళ defined(FEAT_TABPANEL)
    నిలువు = firstwin->w_wincol;
#అయితే_ముగింపు

    ఒకవేళ (Screen_పంక్తులు == ఖాళీ)
	తిరిగి;
    redraw_tabline = FALSE;

#నిర్వచితమైతే FEAT_GUI_TABLINE
    // Take care of a GUI tabline.
    ఒకవేళ (gui_use_tabline())
    {
	gui_update_tabline();
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (tabline_ఎత్తు() < 1)
	తిరిగి;

#ఒకవేళ defined(FEAT_STL_OPT)
    clear_Tab_పుట_Idxs();

    // Use the 'tabline' option if it's set.
    ఒకవేళ (*p_tal != NUL)
	win_redr_custom(ఖాళీ, FALSE);
    లేకపోతే
#అయితే_ముగింపు
    {
	FOR_ALL_TABPAGES(tp)
	    ++tabcount;

	tabwidth = (topframe->fr_వెడల్పు - 1 + tabcount / 2) / tabcount;
	ఒకవేళ (tabwidth < 6)
	    tabwidth = 6;

	tabcount = 0;
	కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ && నిలువు < Columns - 4;
							     tp = tp->tp_తదుపరి)
	{
	    scol = నిలువు;

	    override_success = నెట్టు_highlight_overrides(
		    tp->tp_curwin->w_hl, tp->tp_curwin->w_hl_len);

	    // Update them each time since highlight override might change them.
	    attr_sel = HL_ATTR(HLF_TPS);
	    attr_nosel = HL_ATTR(HLF_TP);
	    attr_fill = HL_ATTR(HLF_TPF);
	    attr = attr_nosel;

	    ఒకవేళ (tp->tp_topframe == topframe)
		attr = attr_sel;
	    ఒకవేళ (use_sep_chars && నిలువు > 0)
		screen_putchar('|', 0, నిలువు++, attr);

	    ఒకవేళ (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, నిలువు++, attr);

	    ఒకవేళ (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    లేకపోతే
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    కొరకు (wincount = 0; wp != ఖాళీ; wp = wp->w_తదుపరి, ++wincount)
		ఒకవేళ (buf_Is_మారింది(wp->w_బఫరు))
		    modified = TRUE;
	    ఒకవేళ (modified || wincount > 1)
	    {
		ఒకవేళ (wincount > 1)
		{
		    len = vim_snprintf((అక్షరం *)NameBuff, MAXPATHL, "%d", wincount);
		    ఒకవేళ (నిలువు + len >= Columns - 3)
			ఆపు;
		    screen_puts_len(NameBuff, len, 0, నిలువు,
#ఒకవేళ defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, HL_ATTR(HLF_T))
#లేకపోతే
					 attr
#అయితే_ముగింపు
					       );
		    నిలువు += len;
		}
		ఒకవేళ (modified)
		    screen_puts_len((char_u *)"+", 1, 0, నిలువు++, attr);
		screen_putchar(' ', 0, నిలువు++, attr);
	    }

	    స్థలం = scol - నిలువు + tabwidth - 1;
	    ఒకవేళ (స్థలం > 0)
	    {
		పూర్ణాంకం n;

		// Get buffer name in NameBuff[]
		get_trans_bufname(cwp->w_బఫరు);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
		ఒకవేళ (has_mbyte)
		    అయితే (len > స్థలం)
		    {
			len -= ptr2cells(p);
			MB_PTR_ADV(p);
		    }
		లేకపోతే ఒకవేళ (len > స్థలం)
		{
		    p += len - స్థలం;
		    len = స్థలం;
		}
		n = Columns - నిలువు - 1;
		ఒకవేళ (len > n)
		    len = n;

		screen_puts_len(p, (పూర్ణాంకం)STRLEN(p), 0, నిలువు, attr);
		నిలువు += len;
	    }
	    screen_putchar(' ', 0, నిలువు++, attr);

	    // Store the tab page number in TabPageIdxs[], so that
	    // jump_to_mouse() knows where each one is.
	    ++tabcount;
	    అయితే (scol < నిలువు)
		Tab_పుట_Idxs[scol++] = tabcount;

	    ఒకవేళ (override_success)
		తీయి_highlight_overrides();
	}

	ఒకవేళ (use_sep_chars)
	    c = '_';
	లేకపోతే
	    c = ' ';
	screen_fill(0, 1, నిలువు, (పూర్ణాంకం)Columns, c, c, attr_fill);

	// Draw the 'showcmd' information if 'showcmdloc' == "tabline".
	ఒకవేళ (p_sc && *p_sloc == 't')
	{
	    పూర్ణాంకం	n = (పూర్ణాంకం)Columns - నిలువు - (tabcount > 1) * 3;
	    పూర్ణాంకం	వెడల్పు = MIN(10, n);

	    ఒకవేళ (వెడల్పు > 0)
		screen_puts_len(showcmd_buf, వెడల్పు, 0, (పూర్ణాంకం)Columns
			    - వెడల్పు - (tabcount > 1) * 2, attr_nosel);
	    showcmd_update_clear_state();
	}

	// Put an "X" for closing the current tab if there are several.
	ఒకవేళ (tabcount > 1 && mouse_has_any())
	{
	    screen_putchar('X', 0, (పూర్ణాంకం)Columns - 1, attr_nosel);
	    Tab_పుట_Idxs[Columns - 1] = -999;
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
    శూన్య
get_trans_bufname(buf_T *buf)
{
    ఒకవేళ (buf_spname(buf) != ఖాళీ)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    లేకపోతే
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}

/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    పూర్ణాంకం
fillchar_స్థితి(పూర్ణాంకం *attr, win_T *wp)
{
    పూర్ణాంకం	    fill;
    bool    override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (bt_terminal(wp->w_బఫరు))
    {
	ఒకవేళ (wp == curwin)
	{
	    *attr = HL_ATTR(HLF_ST);
	    fill = wp->w_fill_chars.stl;
	}
	లేకపోతే
	{
	    *attr = HL_ATTR(HLF_STNC);
	    fill = wp->w_fill_chars.stlnc;
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (wp == curwin)
    {
	*attr = HL_ATTR(HLF_S);
	fill = wp->w_fill_chars.stl;
    }
    లేకపోతే
    {
	*attr = HL_ATTR(HLF_SNC);
	fill = wp->w_fill_chars.stlnc;
    }

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
    తిరిగి fill;
}

/*
 * Return true if the vertical separator of "wp" at screen row "row" is
 * adjacent to the current window.  The separator is owned by "wp" and drawn
 * at its right edge.
 */
    స్థిర bool
vsep_అడ్డువరుస_is_curwin(win_T *wp, పూర్ణాంకం అడ్డువరుస)
{
    ఒకవేళ (wp == curwin)
	తిరిగి true;

    // Check if curwin is immediately to the right of wp's separator and
    // "row" is within curwin's row range (winbar + content + status line).
    ఒకవేళ (curwin->w_wincol == W_ENDCOL(wp) + wp->w_vsep_వెడల్పు
	    && అడ్డువరుస >= curwin->w_winrow
	    && అడ్డువరుస < W_WINROW(curwin) + curwin->w_ఎత్తు
						   + curwin->w_స్థితి_ఎత్తు)
	తిరిగి true;

    తిరిగి false;
}

/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 * Use VertSplit when the separator is adjacent to the current window,
 * VertSplitNC otherwise.
 */
    పూర్ణాంకం
fillchar_vsep(పూర్ణాంకం *attr, win_T *wp, పూర్ణాంకం అడ్డువరుస)
{
    bool override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ఒకవేళ (vsep_అడ్డువరుస_is_curwin(wp, అడ్డువరుస))
	*attr = HL_ATTR(HLF_C);
    లేకపోతే
	*attr = HL_ATTR(HLF_CNC);
    ఒకవేళ (override_success)
	తీయి_highlight_overrides();

    ఒకవేళ (*attr == 0 && wp->w_fill_chars.vert == ' ')
	తిరిగి '|';
    లేకపోతే
	తిరిగి wp->w_fill_chars.vert;
}

/*
 * Return TRUE if redrawing should currently be done.
 */
    పూర్ణాంకం
redrawing(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (disable_redraw_for_testing)
	తిరిగి 0;
    లేకపోతే
#అయితే_ముగింపు
	తిరిగి ((RedrawingDisabled == 0
#నిర్వచితమైతే FEAT_EVAL
		    || ignore_redraw_flag_for_testing
#అయితే_ముగింపు
		) && !(p_lz && char_avail() && !కీలకం_Typed && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    పూర్ణాంకం
messaging(శూన్య)
{
    తిరిగి (!(p_lz && char_avail() && !కీలకం_Typed));
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#నిర్వచించు నిలువు_RULER 17	    // columns needed by standard ruler

    శూన్య
comp_నిలువు(శూన్య)
{
    పూర్ణాంకం చివరి_has_స్థితి = చివరి_stl_ఎత్తు(FALSE) > 0;

    sc_నిలువు = 0;
    ru_నిలువు = 0;
    ఒకవేళ (p_ru)
    {
#నిర్వచితమైతే FEAT_STL_OPT
	ru_నిలువు = (ru_wid ? ru_wid : నిలువు_RULER) + 1;
#లేకపోతే
	ru_నిలువు = నిలువు_RULER + 1;
#అయితే_ముగింపు
	// no last status line, adjust sc_col
	ఒకవేళ (!చివరి_has_స్థితి)
	    sc_నిలువు = ru_నిలువు;
    }
    ఒకవేళ (p_sc && *p_sloc == 'l')
    {
	sc_నిలువు += SHOWCMD_నిలువుల_సంఖ్య;
	ఒకవేళ (!p_ru || చివరి_has_స్థితి)	    // no need for separating space
	    ++sc_నిలువు;
    }
    sc_నిలువు = cmdline_వెడల్పు - sc_నిలువు;
    ru_నిలువు = cmdline_వెడల్పు - ru_నిలువు;
    ఒకవేళ (sc_నిలువు <= 0)		// screen too narrow, will become a mess
	sc_నిలువు = 1;
    ఒకవేళ (ru_నిలువు <= 0)
	ru_నిలువు = 1;
#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_nr(VV_ECHOSPACE, sc_నిలువు - 1);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    పూర్ణాంకం
సంఖ్య_వెడల్పు(win_T *wp)
{
    పూర్ణాంకం		n;
    linenr_T	lnum;

    ఒకవేళ (wp->w_p_rnu && !wp->w_p_nu)
	// cursor line shows "0"
	lnum = wp->w_ఎత్తు;
    లేకపోతే
	// cursor line shows absolute line number
	lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (lnum == wp->w_nrwidth_పంక్తి_లెక్క && wp->w_nuw_cached == wp->w_p_nuw)
	తిరిగి wp->w_nrwidth_వెడల్పు;
    wp->w_nrwidth_పంక్తి_లెక్క = lnum;

    n = 0;
    చేయి
    {
	lnum /= 10;
	++n;
    } అయితే (lnum > 0);

    // 'numberwidth' gives the minimal width plus one
    ఒకవేళ (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

# ifdef FEAT_SIGNS
    // If 'signcolumn' is set to 'number' and there is a sign to display, then
    // the minimal width for the number column is 2.
    ఒకవేళ (n < 2 && get_మొదటి_valid_sign(wp) != ఖాళీ
	    && (*wp->w_p_scl == 'n' && *(wp->w_p_scl + 1) == 'u'))
	n = 2;
# endif

    wp->w_nrwidth_వెడల్పు = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    తిరిగి n;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    పూర్ణాంకం
screen_screencol(శూన్య)
{
    తిరిగి screen_cur_నిలువు;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    పూర్ణాంకం
screen_screenrow(శూన్య)
{
    తిరిగి screen_cur_అడ్డువరుస;
}
#అయితే_ముగింపు

/*
 * Calls mb_ptr2char_adv(p) and returns the character.
 * If "p" starts with "\x", "\u" or "\U" the hex or unicode value is used.
 */
    స్థిర పూర్ణాంకం
get_encoded_char_adv(char_u **p)
{
    char_u *s = *p;

    ఒకవేళ (s[0] == '\\' && (s[1] == 'x' || s[1] == 'u' || s[1] == 'U'))
    {
	varnumber_T అంకె = 0;
	పూర్ణాంకం	    బైట్లు;
	పూర్ణాంకం	    n;

	కొరకు (బైట్లు = s[1] == 'x' ? 1 : s[1] == 'u' ? 2 : 4; బైట్లు > 0; --బైట్లు)
	{
	    *p += 2;
	    n = hexhex2nr(*p);
	    ఒకవేళ (n < 0)
		తిరిగి 0;
	    అంకె = అంకె * 256 + n;
	}
	*p += 2;
	తిరిగి అంకె;
    }
    తిరిగి mb_ptr2char_adv(p);
}

నిర్మాణం charstab
{
    పూర్ణాంకం	    *cp;
    string_T	name;
};

#నిర్వచించు CHARSTAB_నమోదు(cp, name) \
    {(cp), {(char_u *)(name), STRLEN_LITERAL(name)}}

స్థిర fill_chars_T fill_chars;
స్థిర నిర్మాణం charstab filltab[] =
{
    CHARSTAB_నమోదు(&fill_chars.stl,	    "stl"),
    CHARSTAB_నమోదు(&fill_chars.stlnc,	    "stlnc"),
    CHARSTAB_నమోదు(&fill_chars.vert,	    "vert"),
    CHARSTAB_నమోదు(&fill_chars.fold,	    "fold"),
    CHARSTAB_నమోదు(&fill_chars.foldopen,    "foldopen"),
    CHARSTAB_నమోదు(&fill_chars.foldclosed,  "foldclose"),
    CHARSTAB_నమోదు(&fill_chars.foldsep,	    "foldsep"),
    CHARSTAB_నమోదు(&fill_chars.foldinner,   "foldinner"),
    CHARSTAB_నమోదు(&fill_chars.diff,	    "diff"),
    CHARSTAB_నమోదు(&fill_chars.eob,	    "eob"),
    CHARSTAB_నమోదు(&fill_chars.lastline,    "lastline"),
#ఒకవేళ defined(FEAT_TABPANEL)
    CHARSTAB_నమోదు(&fill_chars.tpl_vert,    "tpl_vert"),
#అయితే_ముగింపు
    CHARSTAB_నమోదు(&fill_chars.trunc,	    "trunc"),
    CHARSTAB_నమోదు(&fill_chars.truncrl,	    "truncrl"),
};
స్థిర lcs_chars_T lcs_chars;
స్థిర నిర్మాణం charstab lcstab[] =
{
    CHARSTAB_నమోదు(&lcs_chars.eol,	    "eol"),
    CHARSTAB_నమోదు(&lcs_chars.ext,	    "extends"),
    CHARSTAB_నమోదు(&lcs_chars.nbsp,	    "nbsp"),
    CHARSTAB_నమోదు(&lcs_chars.prec,	    "precedes"),
    CHARSTAB_నమోదు(&lcs_chars.space,	    "space"),
    CHARSTAB_నమోదు(&lcs_chars.tab2,	    "tab"),
    CHARSTAB_నమోదు(&lcs_chars.leadtab2,	    "leadtab"),
    CHARSTAB_నమోదు(&lcs_chars.trail,	    "trail"),
    CHARSTAB_నమోదు(&lcs_chars.lead,	    "lead"),
#నిర్వచితమైతే FEAT_CONCEAL
    CHARSTAB_నమోదు(&lcs_chars.conceal,	    "conceal"),
#లేకపోతే
    CHARSTAB_నమోదు(ఖాళీ,		    "conceal"),
#అయితే_ముగింపు
    CHARSTAB_నమోదు(ఖాళీ,		    "multispace"),
    CHARSTAB_నమోదు(ఖాళీ,		    "leadmultispace")
};

    స్థిర అక్షరం *
క్షేత్రం_విలువ_err(అక్షరం *errbuf, size_t errbuflen, అక్షరం *fmt, char_u *క్షేత్రం)
{
    ఒకవేళ (errbuf == ఖాళీ)
	తిరిగి "";
    vim_snprintf(errbuf, errbuflen, _(fmt), క్షేత్రం);
    తిరిగి errbuf;
}

/*
 * Handle setting 'listchars' or 'fillchars'.
 * "value" points to either the global or the window-local value.
 * "is_listchars" is TRUE for "listchars" and FALSE for "fillchars".
 * When "apply" is FALSE do not store the flags, only check for errors.
 * Assume monocell characters.
 * Returns error message, NULL if it's OK.
 */
    స్థిర అక్షరం *
సమితి_chars_option(win_T *wp, char_u *విలువ, పూర్ణాంకం is_listchars, పూర్ణాంకం apply,
						అక్షరం *errbuf, size_t errbuflen)
{
    పూర్ణాంకం	    విడత, i, entries;
    char_u  *p, *s;
    పూర్ణాంకం	    c1 = 0, c2 = 0, c3 = 0;
    char_u  *చివరి_multispace = ఖాళీ;  // Last occurrence of "multispace:"
    char_u  *చివరి_lmultispace = ఖాళీ; // Last occurrence of "leadmultispace:"
    పూర్ణాంకం	    multispace_len = 0;	      // Length of lcs-multispace string
    పూర్ణాంకం	    lead_multispace_len = 0;  // Length of lcs-leadmultispace string

    నిర్మాణం charstab *tab;

    ఒకవేళ (is_listchars)
    {
	tab = lcstab;
	CLEAR_క్షేత్రం(lcs_chars);
	entries = ARRAY_నిడివి(lcstab);
	ఒకవేళ (wp->w_p_lcs[0] == NUL)
	    విలువ = p_lcs;  // local value is empty, use the global value
    }
    లేకపోతే
    {
	tab = filltab;
	entries = ARRAY_నిడివి(filltab);
	ఒకవేళ (wp->w_p_fcs[0] == NUL)
	    విలువ = p_fcs;  // local value is empty, use the global value
    }

    // first round: check for valid value, second round: assign values
    కొరకు (విడత = 0; విడత <= (apply ? 1 : 0); ++విడత)
    {
	పూర్ణాంకం has_tab = FALSE, has_leadtab = FALSE;

	ఒకవేళ (విడత > 0)
	{
	    // After checking that the value is valid: set defaults.
	    ఒకవేళ (is_listchars)
	    {
		కొరకు (i = 0; i < entries; ++i)
		    ఒకవేళ (tab[i].cp != ఖాళీ)
			*(tab[i].cp) = NUL;
		lcs_chars.tab1 = NUL;
		lcs_chars.tab3 = NUL;
		lcs_chars.leadtab1 = NUL;
		lcs_chars.leadtab3 = NUL;

		ఒకవేళ (multispace_len > 0)
		{
		    lcs_chars.multispace = ALLOC_MULT(పూర్ణాంకం, multispace_len + 1);
		    ఒకవేళ (lcs_chars.multispace != ఖాళీ)
			lcs_chars.multispace[multispace_len] = NUL;
		}
		లేకపోతే
		    lcs_chars.multispace = ఖాళీ;

		ఒకవేళ (lead_multispace_len > 0)
		{
		    lcs_chars.leadmultispace =
				      ALLOC_MULT(పూర్ణాంకం, lead_multispace_len + 1);
		    ఒకవేళ (lcs_chars.leadmultispace != ఖాళీ)
			lcs_chars.leadmultispace[lead_multispace_len] = NUL;
		}
		లేకపోతే
		    lcs_chars.leadmultispace = ఖాళీ;
	    }
	    లేకపోతే
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
#ఒకవేళ defined(FEAT_TABPANEL)
		fill_chars.tpl_vert = '|';
#అయితే_ముగింపు
		fill_chars.trunc = '>';
		fill_chars.truncrl = '<';
	    }
	}
	p = విలువ;
	అయితే (*p)
	{
	    కొరకు (i = 0; i < entries; ++i)
	    {
		ఒకవేళ (!(STRNCMP(p, tab[i].name.string, tab[i].name.నిడివి) == 0 && p[tab[i].name.నిడివి] == ':'))
		    కొనసాగు;

		s = p + tab[i].name.నిడివి + 1;

		ఒకవేళ (is_listchars && STRCMP(tab[i].name.string, "multispace") == 0)
		{
		    ఒకవేళ (విడత == 0)
		    {
			// Get length of lcs-multispace string in first round
			చివరి_multispace = p;
			multispace_len = 0;
			అయితే (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ఒకవేళ (char2cells(c1) > 1)
				తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
					 e_wrong_character_వెడల్పు_for_క్షేత్రం_str,
					 tab[i].name.string);
			    ++multispace_len;
			}
			ఒకవేళ (multispace_len == 0)
			    // lcs-multispace cannot be an empty string
			    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
				    e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str,
				    tab[i].name.string);
		    }
		    లేకపోతే
		    {
			పూర్ణాంకం multispace_pos = 0;

			అయితే (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ఒకవేళ (p == చివరి_multispace && lcs_chars.multispace != ఖాళీ)
				lcs_chars.multispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ఆపు;
		}

		ఒకవేళ (is_listchars && STRCMP(tab[i].name.string, "leadmultispace") == 0)
		{
		    ఒకవేళ (విడత == 0)
		    {
			// Get length of lcs-leadmultispace string in first
			//విడత
			చివరి_lmultispace = p;
			lead_multispace_len = 0;
			అయితే (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ఒకవేళ (char2cells(c1) > 1)
				తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
					 e_wrong_character_వెడల్పు_for_క్షేత్రం_str,
					 tab[i].name.string);
			    ++lead_multispace_len;
			}
			ఒకవేళ (lead_multispace_len == 0)
			    // lcs-leadmultispace cannot be an empty string
			    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
				    e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str,
				    tab[i].name.string);
		    }
		    లేకపోతే
		    {
			పూర్ణాంకం multispace_pos = 0;

			అయితే (*s != NUL && *s != ',')
			{
			    c1 = get_encoded_char_adv(&s);
			    ఒకవేళ (p == చివరి_lmultispace && lcs_chars.leadmultispace != ఖాళీ)
				lcs_chars.leadmultispace[multispace_pos++] = c1;
			}
		    }
		    p = s;
		    ఆపు;
		}

		c2 = c3 = 0;
		ఒకవేళ (*s == NUL)
		    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
				    e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str,
				    tab[i].name.string);
		c1 = get_encoded_char_adv(&s);
		ఒకవేళ (char2cells(c1) > 1)
		    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
					 e_wrong_character_వెడల్పు_for_క్షేత్రం_str,
					 tab[i].name.string);
		ఒకవేళ (tab[i].cp == &lcs_chars.tab2 ||
		    tab[i].cp == &lcs_chars.leadtab2)
		{
		    ఒకవేళ (*s == NUL)
			తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
				    e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str,
				    tab[i].name.string);
		    c2 = get_encoded_char_adv(&s);
		    ఒకవేళ (char2cells(c2) > 1)
			తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
					 e_wrong_character_వెడల్పు_for_క్షేత్రం_str,
					 tab[i].name.string);
		    ఒకవేళ (!(*s == ',' || *s == NUL))
		    {
			c3 = get_encoded_char_adv(&s);
			ఒకవేళ (char2cells(c3) > 1)
			    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
					 e_wrong_character_వెడల్పు_for_క్షేత్రం_str,
					 tab[i].name.string);
		    }
		    ఒకవేళ (tab[i].cp == &lcs_chars.tab2)
			has_tab = TRUE;
		    లేకపోతే  // tab[i].cp == &lcs_chars.leadtab2
			has_leadtab = TRUE;
		}

		ఒకవేళ (*s == ',' || *s == NUL)
		{
		    ఒకవేళ (విడత > 0)
		    {
			ఒకవేళ (tab[i].cp == &lcs_chars.tab2)
			{
			    lcs_chars.tab1 = c1;
			    lcs_chars.tab2 = c2;
			    lcs_chars.tab3 = c3;
			}
			లేకపోతే ఒకవేళ (tab[i].cp == &lcs_chars.leadtab2)
			{
			    lcs_chars.leadtab1 = c1;
			    lcs_chars.leadtab2 = c2;
			    lcs_chars.leadtab3 = c3;
			}
			లేకపోతే ఒకవేళ (tab[i].cp != ఖాళీ)
			    *(tab[i].cp) = c1;
		    }
		    p = s;
		    ఆపు;
		}
		లేకపోతే
		    తిరిగి క్షేత్రం_విలువ_err(errbuf, errbuflen,
				    e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str,
				    tab[i].name.string);
	    }

	    ఒకవేళ (i == entries)
		తిరిగి e_invalid_వాదం;

	    ఒకవేళ (*p == ',')
		++p;
	}

	ఒకవేళ (is_listchars && has_leadtab && !has_tab)
	    తిరిగి e_leadtab_requires_tab;
    }

    ఒకవేళ (apply)
    {
	ఒకవేళ (is_listchars)
	{
	    vim_free(wp->w_lcs_chars.multispace);
	    vim_free(wp->w_lcs_chars.leadmultispace);
	    wp->w_lcs_chars = lcs_chars;
	}
	లేకపోతే
	{
	    wp->w_fill_chars = fill_chars;
	}
    }

    తిరిగి ఖాళీ;	// no error
}

/*
 * Handle the new value of 'fillchars'.
 */
    అక్షరం *
సమితి_fillchars_option(win_T *wp, char_u *val, పూర్ణాంకం apply, అక్షరం *errbuf,
							      size_t errbuflen)
{
    తిరిగి సమితి_chars_option(wp, val, FALSE, apply, errbuf, errbuflen);
}

/*
 * Handle the new value of 'listchars'.
 */
    అక్షరం *
సమితి_listchars_option(win_T *wp, char_u *val, పూర్ణాంకం apply, అక్షరం *errbuf,
							      size_t errbuflen)
{
    తిరిగి సమితి_chars_option(wp, val, TRUE, apply, errbuf, errbuflen);
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'fillchars' option.
 */
    char_u *
get_fillchars_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(filltab))
	తిరిగి ఖాళీ;

    తిరిగి filltab[idx].name.string;
}

/*
 * Function given to ExpandGeneric() to obtain possible arguments of the
 * 'listchars' option.
 */
    char_u *
get_listchars_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(lcstab))
	తిరిగి ఖాళీ;

    తిరిగి lcstab[idx].name.string;
}

/*
 * Check all global and local values of 'listchars' and 'fillchars'.
 * Return an untranslated error messages if any of them is invalid, NULL
 * otherwise.
 */
    అక్షరం *
check_chars_options(శూన్య)
{
    tabpage_T   *tp;
    win_T	    *wp;

    ఒకవేళ (సమితి_listchars_option(curwin, p_lcs, FALSE, ఖాళీ, 0) != ఖాళీ)
	తిరిగి e_conflicts_with_విలువ_యొక్క_listchars;
    ఒకవేళ (సమితి_fillchars_option(curwin, p_fcs, FALSE, ఖాళీ, 0) != ఖాళీ)
	తిరిగి e_conflicts_with_విలువ_యొక్క_fillchars;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ఒకవేళ (సమితి_listchars_option(wp, wp->w_p_lcs, FALSE, ఖాళీ, 0) != ఖాళీ)
	    తిరిగి e_conflicts_with_విలువ_యొక్క_listchars;
	ఒకవేళ (సమితి_fillchars_option(wp, wp->w_p_fcs, FALSE, ఖాళీ, 0) != ఖాళీ)
	    తిరిగి e_conflicts_with_విలువ_యొక్క_fillchars;
    }
    తిరిగి ఖాళీ;
}
