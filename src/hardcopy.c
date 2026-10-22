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

#ಆದರೆ defined(FEAT_PRINTER)

#ಅನ್ತರ್ಭಾವ "hardcopy.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		current_syn_id;
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ	prt_curr_italic;
ಪೂರ್ಣಾಂಕ	prt_curr_bold;
ಪೂರ್ಣಾಂಕ	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
ಪೂರ್ಣಾಂಕ	prt_ಪುಟ_ಎಣಿಕೆ;

/*
 * These values determine the print position on a page.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		lead_spaces;	    // remaining spaces for a TAB
    ಪೂರ್ಣಾಂಕ		print_pos;	    // virtual column for computing TABs
    colnr_T	ಸ್ತಂಭ;		    // byte column
    linenr_T	file_ಸಾಲು;	    // line nr in the buffer
    long_u	ಬೈಟುಗಳು_printed;	    // bytes printed so far
    ಪೂರ್ಣಾಂಕ		ff;		    // seen form feed character
} prt_pos_T;

#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ *parse_list_options(char_u *option_str, option_ಕೋಷ್ಟಕ_T *ಕೋಷ್ಟಕ, ಪೂರ್ಣಾಂಕ ಕೋಷ್ಟಕ_ಗಾತ್ರ_2);

ಸ್ಥಿರ colnr_T hardcopy_ಸಾಲು(prt_settings_T *psettings, ಪೂರ್ಣಾಂಕ ಪುಟ_ಸಾಲು, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    ಅಕ್ಷರ *
parse_printoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ parse_list_options(p_popt, printer_opts, OPT_PRINT_ಅಂಕೆ_OPTIONS);
}

#ಆದರೆ defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    ಅಕ್ಷರ *
parse_printmbfont(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_ಅಂಕೆ_OPTIONS);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Parse a list of options in the form
 * option:value,option:value,option:value
 *
 * "value" can start with a number which is parsed out, e.g.  margin:12mm
 *
 * Returns an error message for an illegal option, NULL otherwise.
 * Only used for the printer at the moment...
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
parse_list_options(
    char_u		*option_str,
    option_ಕೋಷ್ಟಕ_T	*ಕೋಷ್ಟಕ,
    ಪೂರ್ಣಾಂಕ			ಕೋಷ್ಟಕ_ಗಾತ್ರ_2)
{
    option_ಕೋಷ್ಟಕ_T *old_opts;
    ಅಕ್ಷರ	*ret = ಖಾಲಿ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		idx = 0;		// init for GCC
    ಪೂರ್ಣಾಂಕ		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_ಕೋಷ್ಟಕ_T, ಕೋಷ್ಟಕ_ಗಾತ್ರ_2);
    ಆದರೆ (old_opts == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (idx = 0; idx < ಕೋಷ್ಟಕ_ಗಾತ್ರ_2; ++idx)
    {
	old_opts[idx] = ಕೋಷ್ಟಕ[idx];
	ಕೋಷ್ಟಕ[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    ಆಗ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	ಆದರೆ (colonp == ಖಾಲಿ)
	{
	    ret = e_missing_colon_3;
	    ನಿಲ್ಲಿಸು;
	}
	commap = vim_strchr(stringp, ',');
	ಆದರೆ (commap == ಖಾಲಿ)
	    commap = option_str + STRLEN(option_str);

	len = (ಪೂರ್ಣಾಂಕ)(colonp - stringp);

	ಗಾಗಿ (idx = 0; idx < ಕೋಷ್ಟಕ_ಗಾತ್ರ_2; ++idx)
	    ಆದರೆ (STRNICMP(stringp, ಕೋಷ್ಟಕ[idx].name, len) == 0)
		ನಿಲ್ಲಿಸು;

	ಆದರೆ (idx == ಕೋಷ್ಟಕ_ಗಾತ್ರ_2)
	{
	    ret = e_illegal_component;
	    ನಿಲ್ಲಿಸು;
	}
	p = colonp + 1;
	ಕೋಷ್ಟಕ[idx].present = TRUE;

	ಆದರೆ (ಕೋಷ್ಟಕ[idx].hasnum)
	{
	    ಆದರೆ (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ನಿಲ್ಲಿಸು;
	    }

	    ಕೋಷ್ಟಕ[idx].ಸಂಖ್ಯೆ = getdigits(&p); // advances p
	}

	ಕೋಷ್ಟಕ[idx].string = p;
	ಕೋಷ್ಟಕ[idx].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ = (ಪೂರ್ಣಾಂಕ)(commap - p);

	stringp = commap;
	ಆದರೆ (*stringp == ',')
	    ++stringp;
    }

    ಆದರೆ (ret != ಖಾಲಿ)
    {
	// Restore old options in case of error
	ಗಾಗಿ (idx = 0; idx < ಕೋಷ್ಟಕ_ಗಾತ್ರ_2; ++idx)
	    ಕೋಷ್ಟಕ[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ಹಿಂತಿರುಗಿ ret;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ಸ್ಥಿರ long_u
darken_rgb(long_u rgb)
{
    ಹಿಂತಿರುಗಿ	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ಸ್ಥಿರ long_u
prt_get_ಪದ_color(ಪೂರ್ಣಾಂಕ colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    ಆದರೆ (t_colors > 8)
	ಹಿಂತಿರುಗಿ cterm_color_16[colorindex % 16];
    ಹಿಂತಿರುಗಿ cterm_color_8[colorindex % 8];
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_get_attr(
    ಪೂರ್ಣಾಂಕ			hl_id,
    prt_ಪಠ್ಯ_attr_T	*pattr,
    ಪೂರ್ಣಾಂಕ			modec)
{
    ಪೂರ್ಣಾಂಕ     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    ಅಕ್ಷರ    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ಖಾಲಿ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ಖಾಲಿ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ಖಾಲಿ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ಖಾಲಿ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (USE_24_ಬಿಟ್)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	ಆದರೆ (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (ಅಕ್ಷರ *)highlight_color(hl_id, (char_u *)"fg", modec);
	ಆದರೆ (color == ಖಾಲಿ)
	    colorindex = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    colorindex = atoi(color);

	ಆದರೆ (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_ಪದ_color(colorindex);
	ಇಲ್ಲದಿದ್ದರೆ
	    fg_color = PRCOLOR_BLACK;
    }

    ಆದರೆ (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_SYN_HL

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಗಣ_fg(long_u fg)
{
    ಆದರೆ (fg == prt_curr_fg)
	ಹಿಂತಿರುಗಿ;

    prt_curr_fg = fg;
    mch_print_ಗಣ_fg(fg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಗಣ_bg(long_u bg)
{
    ಆದರೆ (bg == prt_curr_bg)
	ಹಿಂತಿರುಗಿ;

    prt_curr_bg = bg;
    mch_print_ಗಣ_bg(bg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಗಣ_font(ಪೂರ್ಣಾಂಕ bold, ಪೂರ್ಣಾಂಕ italic, ಪೂರ್ಣಾಂಕ underline)
{
    ಆದರೆ (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ಹಿಂತಿರುಗಿ;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_ಗಣ_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಸಾಲು_ಸಂಖ್ಯೆ(
    prt_settings_T *psettings,
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಸಾಲು,
    linenr_T	lnum)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	tbuf[20];

    prt_ಗಣ_fg(psettings->ಸಂಖ್ಯೆ.fg_color);
    prt_ಗಣ_bg(psettings->ಸಂಖ್ಯೆ.bg_color);
    prt_ಗಣ_font(psettings->ಸಂಖ್ಯೆ.bold, psettings->ಸಂಖ್ಯೆ.italic, psettings->ಸಂಖ್ಯೆ.underline);
    mch_print_ಆರಂಭ_ಸಾಲು(TRUE, ಪುಟ_ಸಾಲು);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((ಅಕ್ಷರ *)tbuf, "%6ld", (ದೀರ್ಘ)lnum);
    ಗಾಗಿ (i = 0; i < 6; i++)
	(ಶೂನ್ಯ)mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(&tbuf[i], 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Set colors and font back to normal.
	prt_ಗಣ_fg(PRCOLOR_BLACK);
	prt_ಗಣ_bg(PRCOLOR_WHITE);
	prt_ಗಣ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    ಪೂರ್ಣಾಂಕ
prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಆದರೆ (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ಹಿಂತಿರುಗಿ printer_opts[OPT_PRINT_HEADERHEIGHT].ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    ಪೂರ್ಣಾಂಕ
prt_use_ಸಂಖ್ಯೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (printer_opts[OPT_PRINT_ಸಂಖ್ಯೆ].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_ಸಂಖ್ಯೆ].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    ಪೂರ್ಣಾಂಕ
prt_get_unit(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ		u = PRT_UNIT_NONE;
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಅಕ್ಷರ *(units[4]) = PRT_UNIT_ಹೆಸರುಗಳು;

    ಆದರೆ (!printer_opts[idx].present)
	ಹಿಂತಿರುಗಿ PRT_UNIT_NONE;

    ಗಾಗಿ (i = 0; i < 4; ++i)
	ಆದರೆ (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ನಿಲ್ಲಿಸು;
	}
    ಹಿಂತಿರುಗಿ u;
}

/*
 * Print the page header.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಶೀರ್ಷಿಕೆ(
    prt_settings_T  *psettings,
    ಪೂರ್ಣಾಂಕ		pagenum,
    linenr_T	lnum ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ಅಗಲ = psettings->chars_ಪ್ರತಿ_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಸಾಲು;
    char_u	*tbuf;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		l;

    // Also use the space for the line number.
    ಆದರೆ (prt_use_ಸಂಖ್ಯೆ())
	ಅಗಲ += PRINT_ಸಂಖ್ಯೆ_ಅಗಲ;

    tbuf = alloc(ಅಗಲ + IOSIZE);
    ಆದರೆ (tbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    ಆದರೆ (*p_ಶೀರ್ಷಿಕೆ != NUL)
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
	printer_ಪುಟ_ಅಂಕೆ = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(ಅಗಲ + IOSIZE), p_ಶೀರ್ಷಿಕೆ,
			 (char_u *)"printheader", 0, ' ', ಅಗಲ, ಖಾಲಿ, ಖಾಲಿ,
			 ಖಾಲಿ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	sprintf((ಅಕ್ಷರ *)tbuf, _("Page %d"), pagenum);

    prt_ಗಣ_fg(PRCOLOR_BLACK);
    prt_ಗಣ_bg(PRCOLOR_WHITE);
    prt_ಗಣ_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    ಪುಟ_ಸಾಲು = 0 - prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();
    mch_print_ಆರಂಭ_ಸಾಲು(TRUE, ಪುಟ_ಸಾಲು);
    ಗಾಗಿ (p = tbuf; *p != NUL; )
    {
	ಆದರೆ (mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(p, (l = (*mb_ptr2len)(p))))
	{
	    ++ಪುಟ_ಸಾಲು;
	    ಆದರೆ (ಪುಟ_ಸಾಲು >= 0) // out of room in header
		ನಿಲ್ಲಿಸು;
	    mch_print_ಆರಂಭ_ಸಾಲು(TRUE, ಪುಟ_ಸಾಲು);
	}
	p += l;
    }

    vim_free(tbuf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Set colors and font back to normal.
	prt_ಗಣ_fg(PRCOLOR_BLACK);
	prt_ಗಣ_bg(PRCOLOR_WHITE);
	prt_ಗಣ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    ಶೂನ್ಯ
prt_ಸಂದೇಶ(char_u *s)
{
    screen_fill((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, (ಪೂರ್ಣಾಂಕ)Columns, ' ', ' ', 0);
    screen_puts(s, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, 0, HL_ATTR(HLF_R));
    ಹೊರಹೋಗು_flush();
}

    ಶೂನ್ಯ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    ಪೂರ್ಣಾಂಕ			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		ಬೈಟುಗಳು_ಗಮ್ಯ_print = 0;
    ಪೂರ್ಣಾಂಕ			ಪುಟ_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ			jobsplit;

    CLEAR_ಕ್ಷೇತ್ರ(settings);
    settings.has_color = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO
    settings.user_abort_ಸಂದೇಶ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ಆದರೆ (*eap->arg == '>')
    {
	ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;

	// Expand things like "%.ps".
	ಆದರೆ (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    ಆದರೆ (errormsg != ಖಾಲಿ)
		emsg(errormsg);
	    ಹಿಂತಿರುಗಿ;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->arg != NUL)
	settings.arguments = eap->arg;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    ಆದರೆ (mch_print_init(&settings,
			curbuf->b_fname == ಖಾಲಿ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ಖಾಲಿ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
# ifdef  FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	settings.modec = 'g';
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಆದರೆ (t_colors > 1)
	    settings.modec = 'c';
	ಇಲ್ಲದಿದ್ದರೆ
	    settings.modec = 't';

    ಆದರೆ (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    ಇಲ್ಲದಿದ್ದರೆ
	settings.do_syntax = settings.has_color;
#ಆದರೆ_ಅಂತ್ಯ

    // Set up printing attributes for line numbers
    settings.ಸಂಖ್ಯೆ.fg_color = PRCOLOR_BLACK;
    settings.ಸಂಖ್ಯೆ.bg_color = PRCOLOR_WHITE;
    settings.ಸಂಖ್ಯೆ.bold = FALSE;
    settings.ಸಂಖ್ಯೆ.italic = TRUE;
    settings.ಸಂಖ್ಯೆ.underline = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    ಆದರೆ (prt_use_ಸಂಖ್ಯೆ() && settings.do_syntax)
    {
	ಪೂರ್ಣಾಂಕ		id;

	id = syn_name2id((char_u *)"LineNr");
	ಆದರೆ (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.ಸಂಖ್ಯೆ, settings.modec);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Estimate the total lines to be printed
     */
    ಗಾಗಿ (lnum = eap->line1; lnum <= eap->line2; lnum++)
	ಬೈಟುಗಳು_ಗಮ್ಯ_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    ಆದರೆ (ಬೈಟುಗಳು_ಗಮ್ಯ_print == 0)
    {
	ಸಂದೇಶ(_("No text to be printed"));
	ಹೋಗು print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_ಗಣ_fg(PRCOLOR_BLACK);
    prt_ಗಣ_bg(PRCOLOR_WHITE);
    prt_ಗಣ_font(FALSE, FALSE, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    current_syn_id = -1;
#ಆದರೆ_ಅಂತ್ಯ

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    ಆದರೆ (!mch_print_begin(&settings))
	ಹೋಗು print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_ಪುಟ_ಎಣಿಕೆ = 0;
    ಗಾಗಿ (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	ಪುಟ_prtpos;	// print position at page start
	ಪೂರ್ಣಾಂಕ		side;

	CLEAR_ಕ್ಷೇತ್ರ(ಪುಟ_prtpos);
	ಪುಟ_prtpos.file_ಸಾಲು = eap->line1;
	prtpos = ಪುಟ_prtpos;

	ಆದರೆ (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_ಕೊನೆ(&settings);
	    ಆದರೆ (!mch_print_begin(&settings))
		ಹೋಗು print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	ಗಾಗಿ (prt_ಪುಟ_ಎಣಿಕೆ = 0; prtpos.file_ಸಾಲು <= eap->line2; ++prt_ಪುಟ_ಎಣಿಕೆ)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    ಗಾಗಿ (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = ಪುಟ_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		ಗಾಗಿ (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    ಆದರೆ (got_int || settings.user_abort)
			ಹೋಗು print_fail;

		    sprintf((ಅಕ್ಷರ *)IObuff, _("Printing page %d (%d%%)"),
			    prt_ಪುಟ_ಎಣಿಕೆ + 1 + side,
			    prtpos.ಬೈಟುಗಳು_printed > 1000000
				? (ಪೂರ್ಣಾಂಕ)(prtpos.ಬೈಟುಗಳು_printed /
						       (ಬೈಟುಗಳು_ಗಮ್ಯ_print / 100))
				: (ಪೂರ್ಣಾಂಕ)((prtpos.ಬೈಟುಗಳು_printed * 100)
							   / ಬೈಟುಗಳು_ಗಮ್ಯ_print));
		    ಆದರೆ (!mch_print_begin_ಪುಟ(IObuff))
			ಹೋಗು print_fail;

		    ಆದರೆ (settings.n_collated_copies > 1)
			sprintf((ಅಕ್ಷರ *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_ಸಂದೇಶ(IObuff);

		    /*
		     * Output header if required
		     */
		    ಆದರೆ (prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ() > 0)
			prt_ಶೀರ್ಷಿಕೆ(&settings, prt_ಪುಟ_ಎಣಿಕೆ + 1 + side,
							prtpos.file_ಸಾಲು);

		    ಗಾಗಿ (ಪುಟ_ಸಾಲು = 0; ಪುಟ_ಸಾಲು < settings.ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ;
								  ++ಪುಟ_ಸಾಲು)
		    {
			prtpos.ಸ್ತಂಭ = hardcopy_ಸಾಲು(&settings,
							   ಪುಟ_ಸಾಲು, &prtpos);
			ಆದರೆ (prtpos.ಸ್ತಂಭ == 0)
			{
			    // finished a file line
			    prtpos.ಬೈಟುಗಳು_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_ಸಾಲು)));
			    ಆದರೆ (++prtpos.file_ಸಾಲು > eap->line2)
				ನಿಲ್ಲಿಸು; // reached the end
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ನಿಲ್ಲಿಸು;
			}
		    }

		    ಆದರೆ (!mch_print_ಕೊನೆ_ಪುಟ())
			ಹೋಗು print_fail;
		    ಆದರೆ (prtpos.file_ಸಾಲು > eap->line2)
			ನಿಲ್ಲಿಸು; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		ಆದರೆ (prtpos.file_ಸಾಲು > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    ಆದರೆ (!mch_print_blank_ಪುಟ())
			ಹೋಗು print_fail;
		}
	    }
	    ಆದರೆ (settings.duplex && prtpos.file_ಸಾಲು <= eap->line2)
		++prt_ಪುಟ_ಎಣಿಕೆ;

	    // Remember the position where the next page starts.
	    ಪುಟ_prtpos = prtpos;
	}

	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_ಸಂದೇಶ(IObuff);
    }

print_fail:
    ಆದರೆ (got_int || (settings.user_abort
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO
		&& settings.user_abort_ಸಂದೇಶ
#ಆದರೆ_ಅಂತ್ಯ
		))
    {
	sprintf((ಅಕ್ಷರ *)IObuff, "%s", _("Printing aborted"));
	prt_ಸಂದೇಶ(IObuff);
    }
    mch_print_ಕೊನೆ(&settings);

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
    ಪೂರ್ಣಾಂಕ			ಪುಟ_ಸಾಲು,
    prt_pos_T		*ppos)
{
    colnr_T	ಲಂಬಸಾಲು;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		need_break = FALSE;
    ಪೂರ್ಣಾಂಕ		outputlen;
    ಪೂರ್ಣಾಂಕ		tab_spaces;
    long_u	print_pos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    prt_ಪಠ್ಯ_attr_T attr;
    ಪೂರ್ಣಾಂಕ		id;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ppos->ಸ್ತಂಭ == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	ಆದರೆ (!ppos->ff && prt_use_ಸಂಖ್ಯೆ())
	    prt_ಸಾಲು_ಸಂಖ್ಯೆ(psettings, ಪುಟ_ಸಾಲು, ppos->file_ಸಾಲು);
	ppos->ff = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_ಆರಂಭ_ಸಾಲು(0, ಪುಟ_ಸಾಲು);
    ಸಾಲು = ml_get(ppos->file_ಸಾಲು);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    ಗಾಗಿ (ಲಂಬಸಾಲು = ppos->ಸ್ತಂಭ; ಸಾಲು[ಲಂಬಸಾಲು] != NUL && !need_break; ಲಂಬಸಾಲು += outputlen)
    {
	outputlen = 1;
	ಆದರೆ (has_mbyte && (outputlen = (*mb_ptr2len)(ಸಾಲು + ಲಂಬಸಾಲು)) < 1)
	    outputlen = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	ಆದರೆ (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_ಸಾಲು, ಲಂಬಸಾಲು, 1, ಖಾಲಿ, FALSE);
	    ಆದರೆ (id > 0)
		id = syn_get_final_id(id);
	    ಇಲ್ಲದಿದ್ದರೆ
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    ಸಾಲು = ml_get(ppos->file_ಸಾಲು);

	    ಆದರೆ (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_ಗಣ_font(attr.bold, attr.italic, attr.underline);
		prt_ಗಣ_fg(attr.fg_color);
		prt_ಗಣ_bg(attr.bg_color);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು] == TAB || tab_spaces != 0)
	{
	    ಆದರೆ (tab_spaces == 0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
		tab_spaces = (ಪೂರ್ಣಾಂಕ)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#ಆದರೆ_ಅಂತ್ಯ

	    ಆಗ (tab_spaces > 0)
	    {
		need_break = mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		ಆದರೆ (need_break)
		    ನಿಲ್ಲಿಸು;
	    }
	    // Keep the TAB if we didn't finish it.
	    ಆದರೆ (need_break && tab_spaces > 0)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    need_break = mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(ಸಾಲು + ಲಂಬಸಾಲು, outputlen);
	    ಆದರೆ (has_mbyte)
		print_pos += (*mb_ptr2cells)(ಸಾಲು + ಲಂಬಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (ಪೂರ್ಣಾಂಕ)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    ಆದರೆ (!ppos->ff
	    && (ಸಾಲು[ಲಂಬಸಾಲು] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

#ಆದರೆ_ಅಂತ್ಯ //FEAT_PRINTER
