/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * hardcopy.c: printing to paper
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ defined(FEAT_PRINTER)

#ਅਨ੍ਤਰ੍ਭਾਵ "hardcopy.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#ਜੇ_ਅੰਤ

/*
 * To implement printing on a platform, the following functions must be
 * defined:
 *
 * int mch_print_init(prt_settings_T *psettings, char_u *jobname, int forceit)
 * Called once.  Code should display printer dialogue (if appropriate) and
 * determine printer font and margin settings.  Reset has_color if the printer
 * doesn't support colors at all.
 * Returns FAIL to abort.
 *
 * int mch_print_begin(prt_settings_T *settings)
 * Called to start the print job.
 * Return FALSE to abort.
 *
 * int mch_print_begin_page(char_u *msg)
 * Called at the start of each page.
 * "msg" indicates the progress of the print job, can be NULL.
 * Return FALSE to abort.
 *
 * int mch_print_end_page()
 * Called at the end of each page.
 * Return FALSE to abort.
 *
 * int mch_print_blank_page()
 * Called to generate a blank page for collated, duplex, multiple copy
 * document.  Return FALSE to abort.
 *
 * void mch_print_end(prt_settings_T *psettings)
 * Called at normal end of print job.
 *
 * void mch_print_cleanup()
 * Called if print job ends normally or is abandoned. Free any memory, close
 * devices and handles.  Also called when mch_print_begin() fails, but not
 * when mch_print_init() fails.
 *
 * void mch_print_set_font(int Bold, int Italic, int Underline);
 * Called whenever the font style changes.
 *
 * void mch_print_set_bg(long_u bgcol);
 * Called to set the background color for the following text. Parameter is an
 * RGB value.
 *
 * void mch_print_set_fg(long_u fgcol);
 * Called to set the foreground color for the following text. Parameter is an
 * RGB value.
 *
 * mch_print_start_line(int margin, int page_line)
 * Sets the current position at the start of line "page_line".
 * If margin is TRUE start in the left margin (for header and line number).
 *
 * int mch_print_text_out(char_u *p, int len);
 * Output one character of text p[len] at the current position.
 * Return TRUE if there is no room for another character in the same line.
 *
 * Note that the generic code has no idea of margins. The machine code should
 * simply make the page look smaller!  The header and the line numbers are
 * printed in the margin.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
ਸਥਿਰ ਅਚਲ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ਸਥਿਰ ਅਚਲ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ਸਥਿਰ ਪੂਰਨ		current_syn_id;
#ਜੇ_ਅੰਤ

ਪੂਰਨ	prt_curr_italic;
ਪੂਰਨ	prt_curr_bold;
ਪੂਰਨ	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
ਪੂਰਨ	prt_ਪੰਨਾ_ਗਿਣਤੀ;

/*
 * These values determine the print position on a page.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		lead_spaces;	    // remaining spaces for a TAB
    ਪੂਰਨ		print_pos;	    // virtual column for computing TABs
    colnr_T	ਕਾਲਮ;		    // byte column
    linenr_T	file_ਸਤਰ;	    // line nr in the buffer
    long_u	ਬਾਈਟਾਂ_printed;	    // bytes printed so far
    ਪੂਰਨ		ff;		    // seen form feed character
} prt_pos_T;

#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN] = {
    {"A4",		595.0,  842.0},
    {"letter",	612.0,  792.0},
    {"10x14",	720.0, 1008.0},
    {"A3",		842.0, 1191.0},
    {"A5",		420.0,  595.0},
    {"B4",		729.0, 1032.0},
    {"B5",		516.0,  729.0},
    {"executive",	522.0,  756.0},
    {"folio",	595.0,  935.0},
    {"ledger",	1224.0,  792.0},
    {"legal",	612.0, 1008.0},
    {"quarto",	610.0,  780.0},
    {"statement",	396.0,  612.0},
    {"tabloid",	792.0, 1224.0}
};
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *parse_list_options(char_u *option_str, option_ਸਾਰਣੀ_T *ਸਾਰਣੀ, ਪੂਰਨ ਸਾਰਣੀ_ਮਾਪ_2);

ਸਥਿਰ colnr_T hardcopy_ਸਤਰ(prt_settings_T *psettings, ਪੂਰਨ ਪੰਨਾ_ਸਤਰ, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    ਅੱਖਰ *
parse_printoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ parse_list_options(p_popt, printer_opts, OPT_PRINT_ਅੰਕ_OPTIONS);
}

#ਜੇ defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    ਅੱਖਰ *
parse_printmbfont(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_ਅੰਕ_OPTIONS);
}
#ਜੇ_ਅੰਤ

/*
 * Parse a list of options in the form
 * option:value,option:value,option:value
 *
 * "value" can start with a number which is parsed out, e.g.  margin:12mm
 *
 * Returns an error message for an illegal option, NULL otherwise.
 * Only used for the printer at the moment...
 */
    ਸਥਿਰ ਅੱਖਰ *
parse_list_options(
    char_u		*option_str,
    option_ਸਾਰਣੀ_T	*ਸਾਰਣੀ,
    ਪੂਰਨ			ਸਾਰਣੀ_ਮਾਪ_2)
{
    option_ਸਾਰਣੀ_T *old_opts;
    ਅੱਖਰ	*ret = ਨਿਰਾ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    ਪੂਰਨ		idx = 0;		// init for GCC
    ਪੂਰਨ		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_ਸਾਰਣੀ_T, ਸਾਰਣੀ_ਮਾਪ_2);
    ਜੇ (old_opts == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (idx = 0; idx < ਸਾਰਣੀ_ਮਾਪ_2; ++idx)
    {
	old_opts[idx] = ਸਾਰਣੀ[idx];
	ਸਾਰਣੀ[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    ਜਦੋਂ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	ਜੇ (colonp == ਨਿਰਾ)
	{
	    ret = e_missing_colon_3;
	    ਰੋਕੋ;
	}
	commap = vim_strchr(stringp, ',');
	ਜੇ (commap == ਨਿਰਾ)
	    commap = option_str + STRLEN(option_str);

	len = (ਪੂਰਨ)(colonp - stringp);

	ਲਈ (idx = 0; idx < ਸਾਰਣੀ_ਮਾਪ_2; ++idx)
	    ਜੇ (STRNICMP(stringp, ਸਾਰਣੀ[idx].name, len) == 0)
		ਰੋਕੋ;

	ਜੇ (idx == ਸਾਰਣੀ_ਮਾਪ_2)
	{
	    ret = e_illegal_component;
	    ਰੋਕੋ;
	}
	p = colonp + 1;
	ਸਾਰਣੀ[idx].present = TRUE;

	ਜੇ (ਸਾਰਣੀ[idx].hasnum)
	{
	    ਜੇ (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ਰੋਕੋ;
	    }

	    ਸਾਰਣੀ[idx].ਸੰਖਿਆ = getdigits(&p); // advances p
	}

	ਸਾਰਣੀ[idx].string = p;
	ਸਾਰਣੀ[idx].ਲੜੀ_ਲੰਬਾਈ = (ਪੂਰਨ)(commap - p);

	stringp = commap;
	ਜੇ (*stringp == ',')
	    ++stringp;
    }

    ਜੇ (ret != ਨਿਰਾ)
    {
	// Restore old options in case of error
	ਲਈ (idx = 0; idx < ਸਾਰਣੀ_ਮਾਪ_2; ++idx)
	    ਸਾਰਣੀ[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ਵਾਪਸ ret;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ਸਥਿਰ long_u
darken_rgb(long_u rgb)
{
    ਵਾਪਸ	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ਸਥਿਰ long_u
prt_get_ਪਦ_color(ਪੂਰਨ colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    ਜੇ (t_colors > 8)
	ਵਾਪਸ cterm_color_16[colorindex % 16];
    ਵਾਪਸ cterm_color_8[colorindex % 8];
}

    ਸਥਿਰ ਖਾਲੀ
prt_get_attr(
    ਪੂਰਨ			hl_id,
    prt_ਪਾਠ_attr_T	*pattr,
    ਪੂਰਨ			modec)
{
    ਪੂਰਨ     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    ਅੱਖਰ    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ਨਿਰਾ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ਨਿਰਾ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ਨਿਰਾ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ਨਿਰਾ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	ਜੇ (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (ਅੱਖਰ *)highlight_color(hl_id, (char_u *)"fg", modec);
	ਜੇ (color == ਨਿਰਾ)
	    colorindex = 0;
	ਨਹੀਂਤਾਂ
	    colorindex = atoi(color);

	ਜੇ (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_ਪਦ_color(colorindex);
	ਨਹੀਂਤਾਂ
	    fg_color = PRCOLOR_BLACK;
    }

    ਜੇ (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    ਨਹੀਂਤਾਂ ਜੇ (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#ਜੇ_ਅੰਤ // FEAT_SYN_HL

    ਸਥਿਰ ਖਾਲੀ
prt_ਸੈੱਟ_fg(long_u fg)
{
    ਜੇ (fg == prt_curr_fg)
	ਵਾਪਸ;

    prt_curr_fg = fg;
    mch_print_ਸੈੱਟ_fg(fg);
}

    ਸਥਿਰ ਖਾਲੀ
prt_ਸੈੱਟ_bg(long_u bg)
{
    ਜੇ (bg == prt_curr_bg)
	ਵਾਪਸ;

    prt_curr_bg = bg;
    mch_print_ਸੈੱਟ_bg(bg);
}

    ਸਥਿਰ ਖਾਲੀ
prt_ਸੈੱਟ_font(ਪੂਰਨ bold, ਪੂਰਨ italic, ਪੂਰਨ underline)
{
    ਜੇ (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ਵਾਪਸ;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_ਸੈੱਟ_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_ਸਤਰ_ਸੰਖਿਆ(
    prt_settings_T *psettings,
    ਪੂਰਨ		ਪੰਨਾ_ਸਤਰ,
    linenr_T	lnum)
{
    ਪੂਰਨ		i;
    char_u	tbuf[20];

    prt_ਸੈੱਟ_fg(psettings->ਸੰਖਿਆ.fg_color);
    prt_ਸੈੱਟ_bg(psettings->ਸੰਖਿਆ.bg_color);
    prt_ਸੈੱਟ_font(psettings->ਸੰਖਿਆ.bold, psettings->ਸੰਖਿਆ.italic, psettings->ਸੰਖਿਆ.underline);
    mch_print_ਸ਼ੁਰੂ_ਸਤਰ(TRUE, ਪੰਨਾ_ਸਤਰ);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((ਅੱਖਰ *)tbuf, "%6ld", (ਲੰਮਾ)lnum);
    ਲਈ (i = 0; i < 6; i++)
	(ਖਾਲੀ)mch_print_ਪਾਠ_ਬਾਹਰਲਾ(&tbuf[i], 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	// Set colors and font back to normal.
	prt_ਸੈੱਟ_fg(PRCOLOR_BLACK);
	prt_ਸੈੱਟ_bg(PRCOLOR_WHITE);
	prt_ਸੈੱਟ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    ਪੂਰਨ
prt_ਸਿਰਲੇਖ_ਉਚਾਈ(ਖਾਲੀ)
{
    ਜੇ (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ਵਾਪਸ printer_opts[OPT_PRINT_HEADERHEIGHT].ਸੰਖਿਆ;
    ਵਾਪਸ 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    ਪੂਰਨ
prt_use_ਸੰਖਿਆ(ਖਾਲੀ)
{
    ਵਾਪਸ (printer_opts[OPT_PRINT_ਸੰਖਿਆ].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_ਸੰਖਿਆ].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    ਪੂਰਨ
prt_get_unit(ਪੂਰਨ idx)
{
    ਪੂਰਨ		u = PRT_UNIT_NONE;
    ਪੂਰਨ		i;
    ਸਥਿਰ ਅੱਖਰ *(units[4]) = PRT_UNIT_ਨਾਂ;

    ਜੇ (!printer_opts[idx].present)
	ਵਾਪਸ PRT_UNIT_NONE;

    ਲਈ (i = 0; i < 4; ++i)
	ਜੇ (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ਰੋਕੋ;
	}
    ਵਾਪਸ u;
}

/*
 * Print the page header.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_ਸਿਰਲੇਖ(
    prt_settings_T  *psettings,
    ਪੂਰਨ		pagenum,
    linenr_T	lnum ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ਚੌੜਾਈ = psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ;
    ਪੂਰਨ		ਪੰਨਾ_ਸਤਰ;
    char_u	*tbuf;
    char_u	*p;
    ਪੂਰਨ		l;

    // Also use the space for the line number.
    ਜੇ (prt_use_ਸੰਖਿਆ())
	ਚੌੜਾਈ += PRINT_ਸੰਖਿਆ_ਚੌੜਾਈ;

    tbuf = alloc(ਚੌੜਾਈ + IOSIZE);
    ਜੇ (tbuf == ਨਿਰਾ)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    ਜੇ (*p_ਸਿਰਲੇਖ != NUL)
    {
	linenr_T	tmp_lnum, tmp_topline, tmp_botline;

	/*
	 * Need to (temporarily) set current line number and first/last line
	 * number on the 'window'.  Since we don't know how long the page is,
	 * set the first and current line number to the top line, and guess
	 * that the page length is 64.
	 */
	tmp_lnum = curwin->w_cursor.lnum;
	tmp_topline = curwin->w_topline;
	tmp_botline = curwin->w_botline;
	curwin->w_cursor.lnum = lnum;
	curwin->w_topline = lnum;
	curwin->w_botline = lnum + 63;
	printer_ਪੰਨਾ_ਅੰਕ = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(ਚੌੜਾਈ + IOSIZE), p_ਸਿਰਲੇਖ,
			 (char_u *)"printheader", 0, ' ', ਚੌੜਾਈ, ਨਿਰਾ, ਨਿਰਾ,
			 ਨਿਰਾ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	sprintf((ਅੱਖਰ *)tbuf, _("Page %d"), pagenum);

    prt_ਸੈੱਟ_fg(PRCOLOR_BLACK);
    prt_ਸੈੱਟ_bg(PRCOLOR_WHITE);
    prt_ਸੈੱਟ_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    ਪੰਨਾ_ਸਤਰ = 0 - prt_ਸਿਰਲੇਖ_ਉਚਾਈ();
    mch_print_ਸ਼ੁਰੂ_ਸਤਰ(TRUE, ਪੰਨਾ_ਸਤਰ);
    ਲਈ (p = tbuf; *p != NUL; )
    {
	ਜੇ (mch_print_ਪਾਠ_ਬਾਹਰਲਾ(p, (l = (*mb_ptr2len)(p))))
	{
	    ++ਪੰਨਾ_ਸਤਰ;
	    ਜੇ (ਪੰਨਾ_ਸਤਰ >= 0) // out of room in header
		ਰੋਕੋ;
	    mch_print_ਸ਼ੁਰੂ_ਸਤਰ(TRUE, ਪੰਨਾ_ਸਤਰ);
	}
	p += l;
    }

    vim_free(tbuf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	// Set colors and font back to normal.
	prt_ਸੈੱਟ_fg(PRCOLOR_BLACK);
	prt_ਸੈੱਟ_bg(PRCOLOR_WHITE);
	prt_ਸੈੱਟ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    ਖਾਲੀ
prt_ਸੁਨੇਹਾ(char_u *s)
{
    screen_fill((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, (ਪੂਰਨ)Columns, ' ', ' ', 0);
    screen_puts(s, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, 0, HL_ATTR(HLF_R));
    ਬਾਹਰਲਾ_flush();
}

    ਖਾਲੀ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    ਪੂਰਨ			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_print = 0;
    ਪੂਰਨ			ਪੰਨਾ_ਸਤਰ;
    ਪੂਰਨ			jobsplit;

    CLEAR_ਖੇਤਰ(settings);
    settings.has_color = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO
    settings.user_abort_ਸੁਨੇਹਾ = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ਜੇ (*eap->arg == '>')
    {
	ਅੱਖਰ	*errormsg = ਨਿਰਾ;

	// Expand things like "%.ps".
	ਜੇ (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    ਜੇ (errormsg != ਨਿਰਾ)
		emsg(errormsg);
	    ਵਾਪਸ;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*eap->arg != NUL)
	settings.arguments = eap->arg;
#ਜੇ_ਅੰਤ

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    ਜੇ (mch_print_init(&settings,
			curbuf->b_fname == ਨਿਰਾ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ਨਿਰਾ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# ifdef  FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	settings.modec = 'g';
    ਨਹੀਂਤਾਂ
# endif
	ਜੇ (t_colors > 1)
	    settings.modec = 'c';
	ਨਹੀਂਤਾਂ
	    settings.modec = 't';

    ਜੇ (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    ਨਹੀਂਤਾਂ
	settings.do_syntax = settings.has_color;
#ਜੇ_ਅੰਤ

    // Set up printing attributes for line numbers
    settings.ਸੰਖਿਆ.fg_color = PRCOLOR_BLACK;
    settings.ਸੰਖਿਆ.bg_color = PRCOLOR_WHITE;
    settings.ਸੰਖਿਆ.bold = FALSE;
    settings.ਸੰਖਿਆ.italic = TRUE;
    settings.ਸੰਖਿਆ.underline = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    ਜੇ (prt_use_ਸੰਖਿਆ() && settings.do_syntax)
    {
	ਪੂਰਨ		id;

	id = syn_name2id((char_u *)"LineNr");
	ਜੇ (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.ਸੰਖਿਆ, settings.modec);
    }
#ਜੇ_ਅੰਤ

    /*
     * Estimate the total lines to be printed
     */
    ਲਈ (lnum = eap->line1; lnum <= eap->line2; lnum++)
	ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    ਜੇ (ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_print == 0)
    {
	ਸੁਨੇਹਾ(_("No text to be printed"));
	ਜਾਓ print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_ਸੈੱਟ_fg(PRCOLOR_BLACK);
    prt_ਸੈੱਟ_bg(PRCOLOR_WHITE);
    prt_ਸੈੱਟ_font(FALSE, FALSE, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    current_syn_id = -1;
#ਜੇ_ਅੰਤ

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    ਜੇ (!mch_print_begin(&settings))
	ਜਾਓ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_ਪੰਨਾ_ਗਿਣਤੀ = 0;
    ਲਈ (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	ਪੰਨਾ_prtpos;	// print position at page start
	ਪੂਰਨ		side;

	CLEAR_ਖੇਤਰ(ਪੰਨਾ_prtpos);
	ਪੰਨਾ_prtpos.file_ਸਤਰ = eap->line1;
	prtpos = ਪੰਨਾ_prtpos;

	ਜੇ (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_ਸਿਰਾ(&settings);
	    ਜੇ (!mch_print_begin(&settings))
		ਜਾਓ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	ਲਈ (prt_ਪੰਨਾ_ਗਿਣਤੀ = 0; prtpos.file_ਸਤਰ <= eap->line2; ++prt_ਪੰਨਾ_ਗਿਣਤੀ)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    ਲਈ (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = ਪੰਨਾ_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		ਲਈ (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    ਜੇ (got_int || settings.user_abort)
			ਜਾਓ print_fail;

		    sprintf((ਅੱਖਰ *)IObuff, _("Printing page %d (%d%%)"),
			    prt_ਪੰਨਾ_ਗਿਣਤੀ + 1 + side,
			    prtpos.ਬਾਈਟਾਂ_printed > 1000000
				? (ਪੂਰਨ)(prtpos.ਬਾਈਟਾਂ_printed /
						       (ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_print / 100))
				: (ਪੂਰਨ)((prtpos.ਬਾਈਟਾਂ_printed * 100)
							   / ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_print));
		    ਜੇ (!mch_print_begin_ਪੰਨਾ(IObuff))
			ਜਾਓ print_fail;

		    ਜੇ (settings.n_collated_copies > 1)
			sprintf((ਅੱਖਰ *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_ਸੁਨੇਹਾ(IObuff);

		    /*
		     * Output header if required
		     */
		    ਜੇ (prt_ਸਿਰਲੇਖ_ਉਚਾਈ() > 0)
			prt_ਸਿਰਲੇਖ(&settings, prt_ਪੰਨਾ_ਗਿਣਤੀ + 1 + side,
							prtpos.file_ਸਤਰ);

		    ਲਈ (ਪੰਨਾ_ਸਤਰ = 0; ਪੰਨਾ_ਸਤਰ < settings.ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ;
								  ++ਪੰਨਾ_ਸਤਰ)
		    {
			prtpos.ਕਾਲਮ = hardcopy_ਸਤਰ(&settings,
							   ਪੰਨਾ_ਸਤਰ, &prtpos);
			ਜੇ (prtpos.ਕਾਲਮ == 0)
			{
			    // finished a file line
			    prtpos.ਬਾਈਟਾਂ_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_ਸਤਰ)));
			    ਜੇ (++prtpos.file_ਸਤਰ > eap->line2)
				ਰੋਕੋ; // reached the end
			}
			ਨਹੀਂਤਾਂ ਜੇ (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ਰੋਕੋ;
			}
		    }

		    ਜੇ (!mch_print_ਸਿਰਾ_ਪੰਨਾ())
			ਜਾਓ print_fail;
		    ਜੇ (prtpos.file_ਸਤਰ > eap->line2)
			ਰੋਕੋ; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		ਜੇ (prtpos.file_ਸਤਰ > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    ਜੇ (!mch_print_blank_ਪੰਨਾ())
			ਜਾਓ print_fail;
		}
	    }
	    ਜੇ (settings.duplex && prtpos.file_ਸਤਰ <= eap->line2)
		++prt_ਪੰਨਾ_ਗਿਣਤੀ;

	    // Remember the position where the next page starts.
	    ਪੰਨਾ_prtpos = prtpos;
	}

	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_ਸੁਨੇਹਾ(IObuff);
    }

print_fail:
    ਜੇ (got_int || (settings.user_abort
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO
		&& settings.user_abort_ਸੁਨੇਹਾ
#ਜੇ_ਅੰਤ
		))
    {
	sprintf((ਅੱਖਰ *)IObuff, "%s", _("Printing aborted"));
	prt_ਸੁਨੇਹਾ(IObuff);
    }
    mch_print_ਸਿਰਾ(&settings);

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    ਸਥਿਰ colnr_T
hardcopy_ਸਤਰ(
    prt_settings_T	*psettings,
    ਪੂਰਨ			ਪੰਨਾ_ਸਤਰ,
    prt_pos_T		*ppos)
{
    colnr_T	ਖਾਨਾ;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		need_break = FALSE;
    ਪੂਰਨ		outputlen;
    ਪੂਰਨ		tab_spaces;
    long_u	print_pos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    prt_ਪਾਠ_attr_T attr;
    ਪੂਰਨ		id;
#ਜੇ_ਅੰਤ

    ਜੇ (ppos->ਕਾਲਮ == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	ਜੇ (!ppos->ff && prt_use_ਸੰਖਿਆ())
	    prt_ਸਤਰ_ਸੰਖਿਆ(psettings, ਪੰਨਾ_ਸਤਰ, ppos->file_ਸਤਰ);
	ppos->ff = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_ਸ਼ੁਰੂ_ਸਤਰ(0, ਪੰਨਾ_ਸਤਰ);
    ਸਤਰ = ml_get(ppos->file_ਸਤਰ);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    ਲਈ (ਖਾਨਾ = ppos->ਕਾਲਮ; ਸਤਰ[ਖਾਨਾ] != NUL && !need_break; ਖਾਨਾ += outputlen)
    {
	outputlen = 1;
	ਜੇ (has_mbyte && (outputlen = (*mb_ptr2len)(ਸਤਰ + ਖਾਨਾ)) < 1)
	    outputlen = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	ਜੇ (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_ਸਤਰ, ਖਾਨਾ, 1, ਨਿਰਾ, FALSE);
	    ਜੇ (id > 0)
		id = syn_get_final_id(id);
	    ਨਹੀਂਤਾਂ
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    ਸਤਰ = ml_get(ppos->file_ਸਤਰ);

	    ਜੇ (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_ਸੈੱਟ_font(attr.bold, attr.italic, attr.underline);
		prt_ਸੈੱਟ_fg(attr.fg_color);
		prt_ਸੈੱਟ_bg(attr.bg_color);
	    }
	}
#ਜੇ_ਅੰਤ

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	ਜੇ (ਸਤਰ[ਖਾਨਾ] == TAB || tab_spaces != 0)
	{
	    ਜੇ (tab_spaces == 0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
		tab_spaces = (ਪੂਰਨ)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#ਜੇ_ਅੰਤ

	    ਜਦੋਂ (tab_spaces > 0)
	    {
		need_break = mch_print_ਪਾਠ_ਬਾਹਰਲਾ((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		ਜੇ (need_break)
		    ਰੋਕੋ;
	    }
	    // Keep the TAB if we didn't finish it.
	    ਜੇ (need_break && tab_spaces > 0)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ[ਖਾਨਾ] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    need_break = mch_print_ਪਾਠ_ਬਾਹਰਲਾ(ਸਤਰ + ਖਾਨਾ, outputlen);
	    ਜੇ (has_mbyte)
		print_pos += (*mb_ptr2cells)(ਸਤਰ + ਖਾਨਾ);
	    ਨਹੀਂਤਾਂ
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (ਪੂਰਨ)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    ਜੇ (!ppos->ff
	    && (ਸਤਰ[ਖਾਨਾ] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ਵਾਪਸ 0;
    ਵਾਪਸ ਖਾਨਾ;
}

#ਜੇ_ਅੰਤ //FEAT_PRINTER
