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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ಯದಿ defined(FEAT_PRINTER)

#ಅನ್ತರ್ಭಾವ "hardcopy.h"
#ifdef FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#endif

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

#ifdef FEAT_SYN_HL
ಸ್ಥಿರ ಧ್ರುವ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ಸ್ಥಿರ ಧ್ರುವ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		current_syn_id;
#endif

ಪೂರ್ಣಾಙ್ಕ	prt_curr_italic;
ಪೂರ್ಣಾಙ್ಕ	prt_curr_bold;
ಪೂರ್ಣಾಙ್ಕ	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
ಪೂರ್ಣಾಙ್ಕ	prt_page_ಎಣಿಕೆ;

/*
 * These values determine the print position on a page.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		lead_spaces;	    // remaining spaces for a TAB
    ಪೂರ್ಣಾಙ್ಕ		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_ಸಾಲು;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    ಪೂರ್ಣಾಙ್ಕ		ff;		    // seen form feed character
} prt_pos_T;

#ಯದಿ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
#endif

ಸ್ಥಿರ ವರ್ಣ *parse_list_options(char_u *option_str, option_table_T *table, ಪೂರ್ಣಾಙ್ಕ table_size);

ಸ್ಥಿರ colnr_T hardcopy_ಸಾಲು(prt_settings_T *psettings, ಪೂರ್ಣಾಙ್ಕ page_ಸಾಲು, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    ವರ್ಣ *
parse_printoptions(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ parse_list_options(p_popt, printer_opts, OPT_PRINT_ಅಂಕೆ_OPTIONS);
}

#ಯದಿ defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    ವರ್ಣ *
parse_printmbfont(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_ಅಂಕೆ_OPTIONS);
}
#endif

/*
 * Parse a list of options in the form
 * option:value,option:value,option:value
 *
 * "value" can start with a number which is parsed out, e.g.  margin:12mm
 *
 * Returns an error message for an illegal option, NULL otherwise.
 * Only used for the printer at the moment...
 */
    ಸ್ಥಿರ ವರ್ಣ *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    ಪೂರ್ಣಾಙ್ಕ			table_size)
{
    option_table_T *old_opts;
    ವರ್ಣ	*ret = ರಿಕ್ತ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;		// init for GCC
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    ಯದಿ (old_opts == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    ಯಾವತ್ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	ಯದಿ (colonp == ರಿಕ್ತ)
	{
	    ret = e_missing_colon_3;
	    ಭಙ್ಗ;
	}
	commap = vim_strchr(stringp, ',');
	ಯದಿ (commap == ರಿಕ್ತ)
	    commap = option_str + STRLEN(option_str);

	len = (ಪೂರ್ಣಾಙ್ಕ)(colonp - stringp);

	ಪ್ರತಿ (idx = 0; idx < table_size; ++idx)
	    ಯದಿ (STRNICMP(stringp, table[idx].name, len) == 0)
		ಭಙ್ಗ;

	ಯದಿ (idx == table_size)
	{
	    ret = e_illegal_component;
	    ಭಙ್ಗ;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	ಯದಿ (table[idx].hasnum)
	{
	    ಯದಿ (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ಭಙ್ಗ;
	    }

	    table[idx].ಸಂಖ್ಯೆ = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (ಪೂರ್ಣಾಙ್ಕ)(commap - p);

	stringp = commap;
	ಯದಿ (*stringp == ',')
	    ++stringp;
    }

    ಯದಿ (ret != ರಿಕ್ತ)
    {
	// Restore old options in case of error
	ಪ್ರತಿ (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ಪ್ರತ್ಯಾಗಮ ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ಸ್ಥಿರ long_u
darken_rgb(long_u rgb)
{
    ಪ್ರತ್ಯಾಗಮ	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ಸ್ಥಿರ long_u
prt_get_term_color(ಪೂರ್ಣಾಙ್ಕ colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    ಯದಿ (t_colors > 8)
	ಪ್ರತ್ಯಾಗಮ cterm_color_16[colorindex % 16];
    ಪ್ರತ್ಯಾಗಮ cterm_color_8[colorindex % 8];
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_get_attr(
    ಪೂರ್ಣಾಙ್ಕ			hl_id,
    prt_text_attr_T	*pattr,
    ಪೂರ್ಣಾಙ್ಕ			modec)
{
    ಪೂರ್ಣಾಙ್ಕ     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    ವರ್ಣ    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ರಿಕ್ತ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ರಿಕ್ತ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ರಿಕ್ತ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ರಿಕ್ತ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	ಯದಿ (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    ಅನ್ಯಥಾ
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (ವರ್ಣ *)highlight_color(hl_id, (char_u *)"fg", modec);
	ಯದಿ (color == ರಿಕ್ತ)
	    colorindex = 0;
	ಅನ್ಯಥಾ
	    colorindex = atoi(color);

	ಯದಿ (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	ಅನ್ಯಥಾ
	    fg_color = PRCOLOR_BLACK;
    }

    ಯದಿ (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    ಅನ್ಯಥಾ ಯದಿ (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_set_fg(long_u fg)
{
    ಯದಿ (fg == prt_curr_fg)
	ಪ್ರತ್ಯಾಗಮ;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_set_bg(long_u bg)
{
    ಯದಿ (bg == prt_curr_bg)
	ಪ್ರತ್ಯಾಗಮ;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_set_font(ಪೂರ್ಣಾಙ್ಕ bold, ಪೂರ್ಣಾಙ್ಕ italic, ಪೂರ್ಣಾಙ್ಕ underline)
{
    ಯದಿ (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ಪ್ರತ್ಯಾಗಮ;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಸಾಲು_ಸಂಖ್ಯೆ(
    prt_settings_T *psettings,
    ಪೂರ್ಣಾಙ್ಕ		page_ಸಾಲು,
    linenr_T	lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->ಸಂಖ್ಯೆ.fg_color);
    prt_set_bg(psettings->ಸಂಖ್ಯೆ.bg_color);
    prt_set_font(psettings->ಸಂಖ್ಯೆ.bold, psettings->ಸಂಖ್ಯೆ.italic, psettings->ಸಂಖ್ಯೆ.underline);
    mch_print_start_ಸಾಲು(TRUE, page_ಸಾಲು);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((ವರ್ಣ *)tbuf, "%6ld", (long)lnum);
    ಪ್ರತಿ (i = 0; i < 6; i++)
	(ಶೂನ್ಯ)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    ಯದಿ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ಅನ್ಯಥಾ
#endif
    {
	// Set colors and font back to normal.
	prt_set_fg(PRCOLOR_BLACK);
	prt_set_bg(PRCOLOR_WHITE);
	prt_set_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    ಪೂರ್ಣಾಙ್ಕ
prt_header_height(ಶೂನ್ಯ)
{
    ಯದಿ (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ಪ್ರತ್ಯಾಗಮ printer_opts[OPT_PRINT_HEADERHEIGHT].ಸಂಖ್ಯೆ;
    ಪ್ರತ್ಯಾಗಮ 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    ಪೂರ್ಣಾಙ್ಕ
prt_use_ಸಂಖ್ಯೆ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (printer_opts[OPT_PRINT_ಸಂಖ್ಯೆ].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_ಸಂಖ್ಯೆ].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    ಪೂರ್ಣಾಙ್ಕ
prt_get_unit(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ		u = PRT_UNIT_NONE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸ್ಥಿರ ವರ್ಣ *(units[4]) = PRT_UNIT_NAMES;

    ಯದಿ (!printer_opts[idx].present)
	ಪ್ರತ್ಯಾಗಮ PRT_UNIT_NONE;

    ಪ್ರತಿ (i = 0; i < 4; ++i)
	ಯದಿ (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ಭಙ್ಗ;
	}
    ಪ್ರತ್ಯಾಗಮ u;
}

/*
 * Print the page header.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_header(
    prt_settings_T  *psettings,
    ಪೂರ್ಣಾಙ್ಕ		pagenum,
    linenr_T	lnum UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		width = psettings->chars_per_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		page_ಸಾಲು;
    char_u	*tbuf;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		l;

    // Also use the space for the line number.
    ಯದಿ (prt_use_ಸಂಖ್ಯೆ())
	width += PRINT_ಸಂಖ್ಯೆ_WIDTH;

    tbuf = alloc(width + IOSIZE);
    ಯದಿ (tbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_STL_OPT
    ಯದಿ (*p_header != NUL)
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
	printer_page_ಅಂಕೆ = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, ರಿಕ್ತ, ರಿಕ್ತ,
			 ರಿಕ್ತ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    ಅನ್ಯಥಾ
#endif
	sprintf((ವರ್ಣ *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_ಸಾಲು = 0 - prt_header_height();
    mch_print_start_ಸಾಲು(TRUE, page_ಸಾಲು);
    ಪ್ರತಿ (p = tbuf; *p != NUL; )
    {
	ಯದಿ (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_ಸಾಲು;
	    ಯದಿ (page_ಸಾಲು >= 0) // out of room in header
		ಭಙ್ಗ;
	    mch_print_start_ಸಾಲು(TRUE, page_ಸಾಲು);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    ಯದಿ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ಅನ್ಯಥಾ
#endif
    {
	// Set colors and font back to normal.
	prt_set_fg(PRCOLOR_BLACK);
	prt_set_bg(PRCOLOR_WHITE);
	prt_set_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    ಶೂನ್ಯ
prt_message(char_u *s)
{
    screen_fill((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, (ಪೂರ್ಣಾಙ್ಕ)Columns, ' ', ' ', 0);
    screen_puts(s, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    ಶೂನ್ಯ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    ಪೂರ್ಣಾಙ್ಕ			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    ಪೂರ್ಣಾಙ್ಕ			page_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_ಸಂದೇಶ = TRUE;
#endif

#ಯದಿ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ಯದಿ (*eap->arg == '>')
    {
	ವರ್ಣ	*errormsg = ರಿಕ್ತ;

	// Expand things like "%.ps".
	ಯದಿ (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    ಯದಿ (errormsg != ರಿಕ್ತ)
		emsg(errormsg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    ಅನ್ಯಥಾ ಯದಿ (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    ಯದಿ (mch_print_init(&settings,
			curbuf->b_fname == ರಿಕ್ತ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ರಿಕ್ತ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    ಯದಿ (gui.in_use)
	settings.modec = 'g';
    ಅನ್ಯಥಾ
# endif
	ಯದಿ (t_colors > 1)
	    settings.modec = 'c';
	ಅನ್ಯಥಾ
	    settings.modec = 't';

    ಯದಿ (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    ಅನ್ಯಥಾ
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.ಸಂಖ್ಯೆ.fg_color = PRCOLOR_BLACK;
    settings.ಸಂಖ್ಯೆ.bg_color = PRCOLOR_WHITE;
    settings.ಸಂಖ್ಯೆ.bold = FALSE;
    settings.ಸಂಖ್ಯೆ.italic = TRUE;
    settings.ಸಂಖ್ಯೆ.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    ಯದಿ (prt_use_ಸಂಖ್ಯೆ() && settings.do_syntax)
    {
	ಪೂರ್ಣಾಙ್ಕ		id;

	id = syn_name2id((char_u *)"LineNr");
	ಯದಿ (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.ಸಂಖ್ಯೆ, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    ಪ್ರತಿ (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    ಯದಿ (bytes_to_print == 0)
    {
	ಸಂದೇಶ(_("No text to be printed"));
	ಗಚ್ಛ print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(FALSE, FALSE, FALSE);
#ifdef FEAT_SYN_HL
    current_syn_id = -1;
#endif

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    ಯದಿ (!mch_print_begin(&settings))
	ಗಚ್ಛ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_ಎಣಿಕೆ = 0;
    ಪ್ರತಿ (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	ಪೂರ್ಣಾಙ್ಕ		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_ಸಾಲು = eap->line1;
	prtpos = page_prtpos;

	ಯದಿ (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    ಯದಿ (!mch_print_begin(&settings))
		ಗಚ್ಛ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	ಪ್ರತಿ (prt_page_ಎಣಿಕೆ = 0; prtpos.file_ಸಾಲು <= eap->line2; ++prt_page_ಎಣಿಕೆ)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    ಪ್ರತಿ (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		ಪ್ರತಿ (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    ಯದಿ (got_int || settings.user_abort)
			ಗಚ್ಛ print_fail;

		    sprintf((ವರ್ಣ *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_ಎಣಿಕೆ + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (ಪೂರ್ಣಾಙ್ಕ)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (ಪೂರ್ಣಾಙ್ಕ)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    ಯದಿ (!mch_print_begin_page(IObuff))
			ಗಚ್ಛ print_fail;

		    ಯದಿ (settings.n_collated_copies > 1)
			sprintf((ವರ್ಣ *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    ಯದಿ (prt_header_height() > 0)
			prt_header(&settings, prt_page_ಎಣಿಕೆ + 1 + side,
							prtpos.file_ಸಾಲು);

		    ಪ್ರತಿ (page_ಸಾಲು = 0; page_ಸಾಲು < settings.ಸಾಲುಗಳು_per_page;
								  ++page_ಸಾಲು)
		    {
			prtpos.column = hardcopy_ಸಾಲು(&settings,
							   page_ಸಾಲು, &prtpos);
			ಯದಿ (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_ಸಾಲು)));
			    ಯದಿ (++prtpos.file_ಸಾಲು > eap->line2)
				ಭಙ್ಗ; // reached the end
			}
			ಅನ್ಯಥಾ ಯದಿ (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ಭಙ್ಗ;
			}
		    }

		    ಯದಿ (!mch_print_end_page())
			ಗಚ್ಛ print_fail;
		    ಯದಿ (prtpos.file_ಸಾಲು > eap->line2)
			ಭಙ್ಗ; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		ಯದಿ (prtpos.file_ಸಾಲು > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    ಯದಿ (!mch_print_blank_page())
			ಗಚ್ಛ print_fail;
		}
	    }
	    ಯದಿ (settings.duplex && prtpos.file_ಸಾಲು <= eap->line2)
		++prt_page_ಎಣಿಕೆ;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    ಯದಿ (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_ಸಂದೇಶ
#endif
		))
    {
	sprintf((ವರ್ಣ *)IObuff, "%s", _("Printing aborted"));
	prt_message(IObuff);
    }
    mch_print_end(&settings);

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    ಸ್ಥಿರ colnr_T
hardcopy_ಸಾಲು(
    prt_settings_T	*psettings,
    ಪೂರ್ಣಾಙ್ಕ			page_ಸಾಲು,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		need_break = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		outputlen;
    ಪೂರ್ಣಾಙ್ಕ		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    ಪೂರ್ಣಾಙ್ಕ		id;
#endif

    ಯದಿ (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	ಯದಿ (!ppos->ff && prt_use_ಸಂಖ್ಯೆ())
	    prt_ಸಾಲು_ಸಂಖ್ಯೆ(psettings, page_ಸಾಲು, ppos->file_ಸಾಲು);
	ppos->ff = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_ಸಾಲು(0, page_ಸಾಲು);
    ಸಾಲು = ml_get(ppos->file_ಸಾಲು);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    ಪ್ರತಿ (col = ppos->column; ಸಾಲು[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	ಯದಿ (has_mbyte && (outputlen = (*mb_ptr2len)(ಸಾಲು + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	ಯದಿ (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_ಸಾಲು, col, 1, ರಿಕ್ತ, FALSE);
	    ಯದಿ (id > 0)
		id = syn_get_final_id(id);
	    ಅನ್ಯಥಾ
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    ಸಾಲು = ml_get(ppos->file_ಸಾಲು);

	    ಯದಿ (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_set_font(attr.bold, attr.italic, attr.underline);
		prt_set_fg(attr.fg_color);
		prt_set_bg(attr.bg_color);
	    }
	}
#endif

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	ಯದಿ (ಸಾಲು[col] == TAB || tab_spaces != 0)
	{
	    ಯದಿ (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
		tab_spaces = (ಪೂರ್ಣಾಙ್ಕ)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    ಯಾವತ್ (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		ಯದಿ (need_break)
		    ಭಙ್ಗ;
	    }
	    // Keep the TAB if we didn't finish it.
	    ಯದಿ (need_break && tab_spaces > 0)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	ಅನ್ಯಥಾ
	{
	    need_break = mch_print_text_out(ಸಾಲು + col, outputlen);
	    ಯದಿ (has_mbyte)
		print_pos += (*mb_ptr2cells)(ಸಾಲು + col);
	    ಅನ್ಯಥಾ
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (ಪೂರ್ಣಾಙ್ಕ)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    ಯದಿ (!ppos->ff
	    && (ಸಾಲು[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ col;
}

#endif //FEAT_PRINTER
