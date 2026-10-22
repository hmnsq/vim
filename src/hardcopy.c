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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#ఒకవేళ defined(FEAT_PRINTER)

#అన్తర్భావ "hardcopy.h"
#నిర్వచితమైతే FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#అయితే_ముగింపు

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

#నిర్వచితమైతే FEAT_SYN_HL
స్థిర స్థిరరాశి long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

స్థిర స్థిరరాశి long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

స్థిర పూర్ణాంకం		current_syn_id;
#అయితే_ముగింపు

పూర్ణాంకం	prt_curr_italic;
పూర్ణాంకం	prt_curr_bold;
పూర్ణాంకం	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
పూర్ణాంకం	prt_పుట_లెక్క;

/*
 * These values determine the print position on a page.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		lead_spaces;	    // remaining spaces for a TAB
    పూర్ణాంకం		print_pos;	    // virtual column for computing TABs
    colnr_T	స్తంభం;		    // byte column
    linenr_T	file_పంక్తి;	    // line nr in the buffer
    long_u	బైట్లు_printed;	    // bytes printed so far
    పూర్ణాంకం		ff;		    // seen form feed character
} prt_pos_T;

#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
#అయితే_ముగింపు

స్థిర అక్షరం *parse_list_options(char_u *option_str, option_పట్టిక_T *పట్టిక, పూర్ణాంకం పట్టిక_కొలత_2);

స్థిర colnr_T hardcopy_పంక్తి(prt_settings_T *psettings, పూర్ణాంకం పుట_పంక్తి, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    అక్షరం *
parse_printoptions(optset_T *వాదనలు వాడని)
{
    తిరిగి parse_list_options(p_popt, printer_opts, OPT_PRINT_అంకె_OPTIONS);
}

#ఒకవేళ defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    అక్షరం *
parse_printmbfont(optset_T *వాదనలు వాడని)
{
    తిరిగి parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_అంకె_OPTIONS);
}
#అయితే_ముగింపు

/*
 * Parse a list of options in the form
 * option:value,option:value,option:value
 *
 * "value" can start with a number which is parsed out, e.g.  margin:12mm
 *
 * Returns an error message for an illegal option, NULL otherwise.
 * Only used for the printer at the moment...
 */
    స్థిర అక్షరం *
parse_list_options(
    char_u		*option_str,
    option_పట్టిక_T	*పట్టిక,
    పూర్ణాంకం			పట్టిక_కొలత_2)
{
    option_పట్టిక_T *old_opts;
    అక్షరం	*ret = ఖాళీ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    పూర్ణాంకం		idx = 0;		// init for GCC
    పూర్ణాంకం		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_పట్టిక_T, పట్టిక_కొలత_2);
    ఒకవేళ (old_opts == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (idx = 0; idx < పట్టిక_కొలత_2; ++idx)
    {
	old_opts[idx] = పట్టిక[idx];
	పట్టిక[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    అయితే (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	ఒకవేళ (colonp == ఖాళీ)
	{
	    ret = e_missing_colon_3;
	    ఆపు;
	}
	commap = vim_strchr(stringp, ',');
	ఒకవేళ (commap == ఖాళీ)
	    commap = option_str + STRLEN(option_str);

	len = (పూర్ణాంకం)(colonp - stringp);

	కొరకు (idx = 0; idx < పట్టిక_కొలత_2; ++idx)
	    ఒకవేళ (STRNICMP(stringp, పట్టిక[idx].name, len) == 0)
		ఆపు;

	ఒకవేళ (idx == పట్టిక_కొలత_2)
	{
	    ret = e_illegal_component;
	    ఆపు;
	}
	p = colonp + 1;
	పట్టిక[idx].present = TRUE;

	ఒకవేళ (పట్టిక[idx].hasnum)
	{
	    ఒకవేళ (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ఆపు;
	    }

	    పట్టిక[idx].సంఖ్య = getdigits(&p); // advances p
	}

	పట్టిక[idx].string = p;
	పట్టిక[idx].అక్షరశ్రేణి_పొడవు = (పూర్ణాంకం)(commap - p);

	stringp = commap;
	ఒకవేళ (*stringp == ',')
	    ++stringp;
    }

    ఒకవేళ (ret != ఖాళీ)
    {
	// Restore old options in case of error
	కొరకు (idx = 0; idx < పట్టిక_కొలత_2; ++idx)
	    పట్టిక[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    తిరిగి ret;
}


#నిర్వచితమైతే FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    స్థిర long_u
darken_rgb(long_u rgb)
{
    తిరిగి	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    స్థిర long_u
prt_get_పదం_color(పూర్ణాంకం colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    ఒకవేళ (t_colors > 8)
	తిరిగి cterm_color_16[colorindex % 16];
    తిరిగి cterm_color_8[colorindex % 8];
}

    స్థిర శూన్య
prt_get_attr(
    పూర్ణాంకం			hl_id,
    prt_పాఠ్యం_attr_T	*pattr,
    పూర్ణాంకం			modec)
{
    పూర్ణాంకం     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    అక్షరం    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ఖాళీ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ఖాళీ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ఖాళీ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ఖాళీ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (USE_24_బిట్టు)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	ఒకవేళ (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    లేకపోతే
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (అక్షరం *)highlight_color(hl_id, (char_u *)"fg", modec);
	ఒకవేళ (color == ఖాళీ)
	    colorindex = 0;
	లేకపోతే
	    colorindex = atoi(color);

	ఒకవేళ (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_పదం_color(colorindex);
	లేకపోతే
	    fg_color = PRCOLOR_BLACK;
    }

    ఒకవేళ (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    లేకపోతే ఒకవేళ (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#అయితే_ముగింపు // FEAT_SYN_HL

    స్థిర శూన్య
prt_సమితి_fg(long_u fg)
{
    ఒకవేళ (fg == prt_curr_fg)
	తిరిగి;

    prt_curr_fg = fg;
    mch_print_సమితి_fg(fg);
}

    స్థిర శూన్య
prt_సమితి_bg(long_u bg)
{
    ఒకవేళ (bg == prt_curr_bg)
	తిరిగి;

    prt_curr_bg = bg;
    mch_print_సమితి_bg(bg);
}

    స్థిర శూన్య
prt_సమితి_font(పూర్ణాంకం bold, పూర్ణాంకం italic, పూర్ణాంకం underline)
{
    ఒకవేళ (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	తిరిగి;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_సమితి_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    స్థిర శూన్య
prt_పంక్తి_సంఖ్య(
    prt_settings_T *psettings,
    పూర్ణాంకం		పుట_పంక్తి,
    linenr_T	lnum)
{
    పూర్ణాంకం		i;
    char_u	tbuf[20];

    prt_సమితి_fg(psettings->సంఖ్య.fg_color);
    prt_సమితి_bg(psettings->సంఖ్య.bg_color);
    prt_సమితి_font(psettings->సంఖ్య.bold, psettings->సంఖ్య.italic, psettings->సంఖ్య.underline);
    mch_print_ప్రారంభం_పంక్తి(TRUE, పుట_పంక్తి);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((అక్షరం *)tbuf, "%6ld", (దీర్ఘ)lnum);
    కొరకు (i = 0; i < 6; i++)
	(శూన్య)mch_print_పాఠ్యం_బహిర్గతం(&tbuf[i], 1);

#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    లేకపోతే
#అయితే_ముగింపు
    {
	// Set colors and font back to normal.
	prt_సమితి_fg(PRCOLOR_BLACK);
	prt_సమితి_bg(PRCOLOR_WHITE);
	prt_సమితి_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    పూర్ణాంకం
prt_శీర్షిక_ఎత్తు(శూన్య)
{
    ఒకవేళ (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	తిరిగి printer_opts[OPT_PRINT_HEADERHEIGHT].సంఖ్య;
    తిరిగి 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    పూర్ణాంకం
prt_use_సంఖ్య(శూన్య)
{
    తిరిగి (printer_opts[OPT_PRINT_సంఖ్య].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_సంఖ్య].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    పూర్ణాంకం
prt_get_unit(పూర్ణాంకం idx)
{
    పూర్ణాంకం		u = PRT_UNIT_NONE;
    పూర్ణాంకం		i;
    స్థిర అక్షరం *(units[4]) = PRT_UNIT_పేర్లు;

    ఒకవేళ (!printer_opts[idx].present)
	తిరిగి PRT_UNIT_NONE;

    కొరకు (i = 0; i < 4; ++i)
	ఒకవేళ (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ఆపు;
	}
    తిరిగి u;
}

/*
 * Print the page header.
 */
    స్థిర శూన్య
prt_శీర్షిక(
    prt_settings_T  *psettings,
    పూర్ణాంకం		pagenum,
    linenr_T	lnum వాడని)
{
    పూర్ణాంకం		వెడల్పు = psettings->chars_చొప్పున_పంక్తి;
    పూర్ణాంకం		పుట_పంక్తి;
    char_u	*tbuf;
    char_u	*p;
    పూర్ణాంకం		l;

    // Also use the space for the line number.
    ఒకవేళ (prt_use_సంఖ్య())
	వెడల్పు += PRINT_సంఖ్య_వెడల్పు;

    tbuf = alloc(వెడల్పు + IOSIZE);
    ఒకవేళ (tbuf == ఖాళీ)
	తిరిగి;

#నిర్వచితమైతే FEAT_STL_OPT
    ఒకవేళ (*p_శీర్షిక != NUL)
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
	printer_పుట_అంకె = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(వెడల్పు + IOSIZE), p_శీర్షిక,
			 (char_u *)"printheader", 0, ' ', వెడల్పు, ఖాళీ, ఖాళీ,
			 ఖాళీ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    లేకపోతే
#అయితే_ముగింపు
	sprintf((అక్షరం *)tbuf, _("Page %d"), pagenum);

    prt_సమితి_fg(PRCOLOR_BLACK);
    prt_సమితి_bg(PRCOLOR_WHITE);
    prt_సమితి_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    పుట_పంక్తి = 0 - prt_శీర్షిక_ఎత్తు();
    mch_print_ప్రారంభం_పంక్తి(TRUE, పుట_పంక్తి);
    కొరకు (p = tbuf; *p != NUL; )
    {
	ఒకవేళ (mch_print_పాఠ్యం_బహిర్గతం(p, (l = (*mb_ptr2len)(p))))
	{
	    ++పుట_పంక్తి;
	    ఒకవేళ (పుట_పంక్తి >= 0) // out of room in header
		ఆపు;
	    mch_print_ప్రారంభం_పంక్తి(TRUE, పుట_పంక్తి);
	}
	p += l;
    }

    vim_free(tbuf);

#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    లేకపోతే
#అయితే_ముగింపు
    {
	// Set colors and font back to normal.
	prt_సమితి_fg(PRCOLOR_BLACK);
	prt_సమితి_bg(PRCOLOR_WHITE);
	prt_సమితి_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    శూన్య
prt_సందేశం(char_u *s)
{
    screen_fill((పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)అడ్డువరుసలు, 0, (పూర్ణాంకం)Columns, ' ', ' ', 0);
    screen_puts(s, (పూర్ణాంకం)అడ్డువరుసలు - 1, 0, HL_ATTR(HLF_R));
    బహిర్గతం_flush();
}

    శూన్య
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    పూర్ణాంకం			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		బైట్లు_గమ్యం_print = 0;
    పూర్ణాంకం			పుట_పంక్తి;
    పూర్ణాంకం			jobsplit;

    CLEAR_క్షేత్రం(settings);
    settings.has_color = TRUE;
#నిర్వచితమైతే FEAT_PRINT_PANGO
    settings.user_abort_సందేశం = TRUE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ఒకవేళ (*eap->arg == '>')
    {
	అక్షరం	*errormsg = ఖాళీ;

	// Expand things like "%.ps".
	ఒకవేళ (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    ఒకవేళ (errormsg != ఖాళీ)
		emsg(errormsg);
	    తిరిగి;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    లేకపోతే ఒకవేళ (*eap->arg != NUL)
	settings.arguments = eap->arg;
#అయితే_ముగింపు

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    ఒకవేళ (mch_print_init(&settings,
			curbuf->b_fname == ఖాళీ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ఖాళీ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	తిరిగి;

#నిర్వచితమైతే FEAT_SYN_HL
# ifdef  FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	settings.modec = 'g';
    లేకపోతే
# endif
	ఒకవేళ (t_colors > 1)
	    settings.modec = 'c';
	లేకపోతే
	    settings.modec = 't';

    ఒకవేళ (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    లేకపోతే ఒకవేళ (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    లేకపోతే
	settings.do_syntax = settings.has_color;
#అయితే_ముగింపు

    // Set up printing attributes for line numbers
    settings.సంఖ్య.fg_color = PRCOLOR_BLACK;
    settings.సంఖ్య.bg_color = PRCOLOR_WHITE;
    settings.సంఖ్య.bold = FALSE;
    settings.సంఖ్య.italic = TRUE;
    settings.సంఖ్య.underline = FALSE;
#నిర్వచితమైతే FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    ఒకవేళ (prt_use_సంఖ్య() && settings.do_syntax)
    {
	పూర్ణాంకం		id;

	id = syn_name2id((char_u *)"LineNr");
	ఒకవేళ (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.సంఖ్య, settings.modec);
    }
#అయితే_ముగింపు

    /*
     * Estimate the total lines to be printed
     */
    కొరకు (lnum = eap->line1; lnum <= eap->line2; lnum++)
	బైట్లు_గమ్యం_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    ఒకవేళ (బైట్లు_గమ్యం_print == 0)
    {
	సందేశం(_("No text to be printed"));
	వెళ్ళు print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_సమితి_fg(PRCOLOR_BLACK);
    prt_సమితి_bg(PRCOLOR_WHITE);
    prt_సమితి_font(FALSE, FALSE, FALSE);
#నిర్వచితమైతే FEAT_SYN_HL
    current_syn_id = -1;
#అయితే_ముగింపు

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    ఒకవేళ (!mch_print_begin(&settings))
	వెళ్ళు print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_పుట_లెక్క = 0;
    కొరకు (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	పుట_prtpos;	// print position at page start
	పూర్ణాంకం		side;

	CLEAR_క్షేత్రం(పుట_prtpos);
	పుట_prtpos.file_పంక్తి = eap->line1;
	prtpos = పుట_prtpos;

	ఒకవేళ (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_చివర(&settings);
	    ఒకవేళ (!mch_print_begin(&settings))
		వెళ్ళు print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	కొరకు (prt_పుట_లెక్క = 0; prtpos.file_పంక్తి <= eap->line2; ++prt_పుట_లెక్క)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    కొరకు (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = పుట_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		కొరకు (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    ఒకవేళ (got_int || settings.user_abort)
			వెళ్ళు print_fail;

		    sprintf((అక్షరం *)IObuff, _("Printing page %d (%d%%)"),
			    prt_పుట_లెక్క + 1 + side,
			    prtpos.బైట్లు_printed > 1000000
				? (పూర్ణాంకం)(prtpos.బైట్లు_printed /
						       (బైట్లు_గమ్యం_print / 100))
				: (పూర్ణాంకం)((prtpos.బైట్లు_printed * 100)
							   / బైట్లు_గమ్యం_print));
		    ఒకవేళ (!mch_print_begin_పుట(IObuff))
			వెళ్ళు print_fail;

		    ఒకవేళ (settings.n_collated_copies > 1)
			sprintf((అక్షరం *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_సందేశం(IObuff);

		    /*
		     * Output header if required
		     */
		    ఒకవేళ (prt_శీర్షిక_ఎత్తు() > 0)
			prt_శీర్షిక(&settings, prt_పుట_లెక్క + 1 + side,
							prtpos.file_పంక్తి);

		    కొరకు (పుట_పంక్తి = 0; పుట_పంక్తి < settings.పంక్తులు_చొప్పున_పుట;
								  ++పుట_పంక్తి)
		    {
			prtpos.స్తంభం = hardcopy_పంక్తి(&settings,
							   పుట_పంక్తి, &prtpos);
			ఒకవేళ (prtpos.స్తంభం == 0)
			{
			    // finished a file line
			    prtpos.బైట్లు_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_పంక్తి)));
			    ఒకవేళ (++prtpos.file_పంక్తి > eap->line2)
				ఆపు; // reached the end
			}
			లేకపోతే ఒకవేళ (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ఆపు;
			}
		    }

		    ఒకవేళ (!mch_print_చివర_పుట())
			వెళ్ళు print_fail;
		    ఒకవేళ (prtpos.file_పంక్తి > eap->line2)
			ఆపు; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		ఒకవేళ (prtpos.file_పంక్తి > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    ఒకవేళ (!mch_print_blank_పుట())
			వెళ్ళు print_fail;
		}
	    }
	    ఒకవేళ (settings.duplex && prtpos.file_పంక్తి <= eap->line2)
		++prt_పుట_లెక్క;

	    // Remember the position where the next page starts.
	    పుట_prtpos = prtpos;
	}

	vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_సందేశం(IObuff);
    }

print_fail:
    ఒకవేళ (got_int || (settings.user_abort
#నిర్వచితమైతే FEAT_PRINT_PANGO
		&& settings.user_abort_సందేశం
#అయితే_ముగింపు
		))
    {
	sprintf((అక్షరం *)IObuff, "%s", _("Printing aborted"));
	prt_సందేశం(IObuff);
    }
    mch_print_చివర(&settings);

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    స్థిర colnr_T
hardcopy_పంక్తి(
    prt_settings_T	*psettings,
    పూర్ణాంకం			పుట_పంక్తి,
    prt_pos_T		*ppos)
{
    colnr_T	నిలువు;
    char_u	*పంక్తి;
    పూర్ణాంకం		need_break = FALSE;
    పూర్ణాంకం		outputlen;
    పూర్ణాంకం		tab_spaces;
    long_u	print_pos;
#నిర్వచితమైతే FEAT_SYN_HL
    prt_పాఠ్యం_attr_T attr;
    పూర్ణాంకం		id;
#అయితే_ముగింపు

    ఒకవేళ (ppos->స్తంభం == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	ఒకవేళ (!ppos->ff && prt_use_సంఖ్య())
	    prt_పంక్తి_సంఖ్య(psettings, పుట_పంక్తి, ppos->file_పంక్తి);
	ppos->ff = FALSE;
    }
    లేకపోతే
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_ప్రారంభం_పంక్తి(0, పుట_పంక్తి);
    పంక్తి = ml_get(ppos->file_పంక్తి);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    కొరకు (నిలువు = ppos->స్తంభం; పంక్తి[నిలువు] != NUL && !need_break; నిలువు += outputlen)
    {
	outputlen = 1;
	ఒకవేళ (has_mbyte && (outputlen = (*mb_ptr2len)(పంక్తి + నిలువు)) < 1)
	    outputlen = 1;
#నిర్వచితమైతే FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	ఒకవేళ (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_పంక్తి, నిలువు, 1, ఖాళీ, FALSE);
	    ఒకవేళ (id > 0)
		id = syn_get_final_id(id);
	    లేకపోతే
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    పంక్తి = ml_get(ppos->file_పంక్తి);

	    ఒకవేళ (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_సమితి_font(attr.bold, attr.italic, attr.underline);
		prt_సమితి_fg(attr.fg_color);
		prt_సమితి_bg(attr.bg_color);
	    }
	}
#అయితే_ముగింపు

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	ఒకవేళ (పంక్తి[నిలువు] == TAB || tab_spaces != 0)
	{
	    ఒకవేళ (tab_spaces == 0)
#నిర్వచితమైతే FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#లేకపోతే
		tab_spaces = (పూర్ణాంకం)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#అయితే_ముగింపు

	    అయితే (tab_spaces > 0)
	    {
		need_break = mch_print_పాఠ్యం_బహిర్గతం((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		ఒకవేళ (need_break)
		    ఆపు;
	    }
	    // Keep the TAB if we didn't finish it.
	    ఒకవేళ (need_break && tab_spaces > 0)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (పంక్తి[నిలువు] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	లేకపోతే
	{
	    need_break = mch_print_పాఠ్యం_బహిర్గతం(పంక్తి + నిలువు, outputlen);
	    ఒకవేళ (has_mbyte)
		print_pos += (*mb_ptr2cells)(పంక్తి + నిలువు);
	    లేకపోతే
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (పూర్ణాంకం)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    ఒకవేళ (!ppos->ff
	    && (పంక్తి[నిలువు] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	తిరిగి 0;
    తిరిగి నిలువు;
}

#అయితే_ముగింపు //FEAT_PRINTER
