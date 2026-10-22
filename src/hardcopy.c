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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#എങ്കിൽ defined(FEAT_PRINTER)

#അന്തര്ഭാവ "hardcopy.h"
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
സ്ഥിതം സ്ഥിരം long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

സ്ഥിതം സ്ഥിരം long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

സ്ഥിതം പൂർണ്ണസംഖ്യ		current_syn_id;
#endif

പൂർണ്ണസംഖ്യ	prt_curr_italic;
പൂർണ്ണസംഖ്യ	prt_curr_bold;
പൂർണ്ണസംഖ്യ	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
പൂർണ്ണസംഖ്യ	prt_page_എണ്ണം;

/*
 * These values determine the print position on a page.
 */
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ		lead_spaces;	    // remaining spaces for a TAB
    പൂർണ്ണസംഖ്യ		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_വരി;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    പൂർണ്ണസംഖ്യ		ff;		    // seen form feed character
} prt_pos_T;

#എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

സ്ഥിതം അക്ഷരം *parse_list_options(char_u *option_str, option_table_T *table, പൂർണ്ണസംഖ്യ table_size);

സ്ഥിതം colnr_T hardcopy_വരി(prt_settings_T *psettings, പൂർണ്ണസംഖ്യ page_വരി, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    അക്ഷരം *
parse_printoptions(optset_T *args UNUSED)
{
    തിരികെ parse_list_options(p_popt, printer_opts, OPT_PRINT_അക്കം_OPTIONS);
}

#എങ്കിൽ defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    അക്ഷരം *
parse_printmbfont(optset_T *args UNUSED)
{
    തിരികെ parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_അക്കം_OPTIONS);
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
    സ്ഥിതം അക്ഷരം *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    പൂർണ്ണസംഖ്യ			table_size)
{
    option_table_T *old_opts;
    അക്ഷരം	*ret = ശൂന്യ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		idx = 0;		// init for GCC
    പൂർണ്ണസംഖ്യ		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    എങ്കിൽ (old_opts == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    ആകുമ്പോൾ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	എങ്കിൽ (colonp == ശൂന്യ)
	{
	    ret = e_missing_colon_3;
	    നിർത്തുക;
	}
	commap = vim_strchr(stringp, ',');
	എങ്കിൽ (commap == ശൂന്യ)
	    commap = option_str + STRLEN(option_str);

	len = (പൂർണ്ണസംഖ്യ)(colonp - stringp);

	വേണ്ടി (idx = 0; idx < table_size; ++idx)
	    എങ്കിൽ (STRNICMP(stringp, table[idx].name, len) == 0)
		നിർത്തുക;

	എങ്കിൽ (idx == table_size)
	{
	    ret = e_illegal_component;
	    നിർത്തുക;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	എങ്കിൽ (table[idx].hasnum)
	{
	    എങ്കിൽ (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		നിർത്തുക;
	    }

	    table[idx].സംഖ്യ = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (പൂർണ്ണസംഖ്യ)(commap - p);

	stringp = commap;
	എങ്കിൽ (*stringp == ',')
	    ++stringp;
    }

    എങ്കിൽ (ret != ശൂന്യ)
    {
	// Restore old options in case of error
	വേണ്ടി (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    തിരികെ ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    സ്ഥിതം long_u
darken_rgb(long_u rgb)
{
    തിരികെ	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    സ്ഥിതം long_u
prt_get_term_color(പൂർണ്ണസംഖ്യ colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    എങ്കിൽ (t_colors > 8)
	തിരികെ cterm_color_16[colorindex % 16];
    തിരികെ cterm_color_8[colorindex % 8];
}

    സ്ഥിതം ശൂന്യം
prt_get_attr(
    പൂർണ്ണസംഖ്യ			hl_id,
    prt_text_attr_T	*pattr,
    പൂർണ്ണസംഖ്യ			modec)
{
    പൂർണ്ണസംഖ്യ     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    അക്ഷരം    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ശൂന്യ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ശൂന്യ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ശൂന്യ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ശൂന്യ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	എങ്കിൽ (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    അല്ലെങ്കിൽ
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (അക്ഷരം *)highlight_color(hl_id, (char_u *)"fg", modec);
	എങ്കിൽ (color == ശൂന്യ)
	    colorindex = 0;
	അല്ലെങ്കിൽ
	    colorindex = atoi(color);

	എങ്കിൽ (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	അല്ലെങ്കിൽ
	    fg_color = PRCOLOR_BLACK;
    }

    എങ്കിൽ (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    അല്ലെങ്കിൽ എങ്കിൽ (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    സ്ഥിതം ശൂന്യം
prt_set_fg(long_u fg)
{
    എങ്കിൽ (fg == prt_curr_fg)
	തിരികെ;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    സ്ഥിതം ശൂന്യം
prt_set_bg(long_u bg)
{
    എങ്കിൽ (bg == prt_curr_bg)
	തിരികെ;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    സ്ഥിതം ശൂന്യം
prt_set_font(പൂർണ്ണസംഖ്യ bold, പൂർണ്ണസംഖ്യ italic, പൂർണ്ണസംഖ്യ underline)
{
    എങ്കിൽ (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	തിരികെ;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    സ്ഥിതം ശൂന്യം
prt_വരി_സംഖ്യ(
    prt_settings_T *psettings,
    പൂർണ്ണസംഖ്യ		page_വരി,
    linenr_T	lnum)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->സംഖ്യ.fg_color);
    prt_set_bg(psettings->സംഖ്യ.bg_color);
    prt_set_font(psettings->സംഖ്യ.bold, psettings->സംഖ്യ.italic, psettings->സംഖ്യ.underline);
    mch_print_start_വരി(TRUE, page_വരി);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((അക്ഷരം *)tbuf, "%6ld", (long)lnum);
    വേണ്ടി (i = 0; i < 6; i++)
	(ശൂന്യം)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    എങ്കിൽ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
prt_header_height(ശൂന്യം)
{
    എങ്കിൽ (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	തിരികെ printer_opts[OPT_PRINT_HEADERHEIGHT].സംഖ്യ;
    തിരികെ 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    പൂർണ്ണസംഖ്യ
prt_use_സംഖ്യ(ശൂന്യം)
{
    തിരികെ (printer_opts[OPT_PRINT_സംഖ്യ].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_സംഖ്യ].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    പൂർണ്ണസംഖ്യ
prt_get_unit(പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ		u = PRT_UNIT_NONE;
    പൂർണ്ണസംഖ്യ		i;
    സ്ഥിതം അക്ഷരം *(units[4]) = PRT_UNIT_NAMES;

    എങ്കിൽ (!printer_opts[idx].present)
	തിരികെ PRT_UNIT_NONE;

    വേണ്ടി (i = 0; i < 4; ++i)
	എങ്കിൽ (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    നിർത്തുക;
	}
    തിരികെ u;
}

/*
 * Print the page header.
 */
    സ്ഥിതം ശൂന്യം
prt_header(
    prt_settings_T  *psettings,
    പൂർണ്ണസംഖ്യ		pagenum,
    linenr_T	lnum UNUSED)
{
    പൂർണ്ണസംഖ്യ		width = psettings->chars_per_വരി;
    പൂർണ്ണസംഖ്യ		page_വരി;
    char_u	*tbuf;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		l;

    // Also use the space for the line number.
    എങ്കിൽ (prt_use_സംഖ്യ())
	width += PRINT_സംഖ്യ_WIDTH;

    tbuf = alloc(width + IOSIZE);
    എങ്കിൽ (tbuf == ശൂന്യ)
	തിരികെ;

#ifdef FEAT_STL_OPT
    എങ്കിൽ (*p_header != NUL)
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
	printer_page_അക്കം = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, ശൂന്യ, ശൂന്യ,
			 ശൂന്യ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    അല്ലെങ്കിൽ
#endif
	sprintf((അക്ഷരം *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_വരി = 0 - prt_header_height();
    mch_print_start_വരി(TRUE, page_വരി);
    വേണ്ടി (p = tbuf; *p != NUL; )
    {
	എങ്കിൽ (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_വരി;
	    എങ്കിൽ (page_വരി >= 0) // out of room in header
		നിർത്തുക;
	    mch_print_start_വരി(TRUE, page_വരി);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    എങ്കിൽ (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    അല്ലെങ്കിൽ
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
    ശൂന്യം
prt_message(char_u *s)
{
    screen_fill((പൂർണ്ണസംഖ്യ)Rows - 1, (പൂർണ്ണസംഖ്യ)Rows, 0, (പൂർണ്ണസംഖ്യ)Columns, ' ', ' ', 0);
    screen_puts(s, (പൂർണ്ണസംഖ്യ)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    ശൂന്യം
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    പൂർണ്ണസംഖ്യ			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    പൂർണ്ണസംഖ്യ			page_വരി;
    പൂർണ്ണസംഖ്യ			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_സന്ദേശം = TRUE;
#endif

#എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    എങ്കിൽ (*eap->arg == '>')
    {
	അക്ഷരം	*errormsg = ശൂന്യ;

	// Expand things like "%.ps".
	എങ്കിൽ (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    എങ്കിൽ (errormsg != ശൂന്യ)
		emsg(errormsg);
	    തിരികെ;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    എങ്കിൽ (mch_print_init(&settings,
			curbuf->b_fname == ശൂന്യ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ശൂന്യ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	തിരികെ;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    എങ്കിൽ (gui.in_use)
	settings.modec = 'g';
    അല്ലെങ്കിൽ
# endif
	എങ്കിൽ (t_colors > 1)
	    settings.modec = 'c';
	അല്ലെങ്കിൽ
	    settings.modec = 't';

    എങ്കിൽ (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    അല്ലെങ്കിൽ
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.സംഖ്യ.fg_color = PRCOLOR_BLACK;
    settings.സംഖ്യ.bg_color = PRCOLOR_WHITE;
    settings.സംഖ്യ.bold = FALSE;
    settings.സംഖ്യ.italic = TRUE;
    settings.സംഖ്യ.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    എങ്കിൽ (prt_use_സംഖ്യ() && settings.do_syntax)
    {
	പൂർണ്ണസംഖ്യ		id;

	id = syn_name2id((char_u *)"LineNr");
	എങ്കിൽ (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.സംഖ്യ, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    വേണ്ടി (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    എങ്കിൽ (bytes_to_print == 0)
    {
	സന്ദേശം(_("No text to be printed"));
	പോകുക print_fail_no_begin;
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

    എങ്കിൽ (!mch_print_begin(&settings))
	പോകുക print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_എണ്ണം = 0;
    വേണ്ടി (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	പൂർണ്ണസംഖ്യ		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_വരി = eap->line1;
	prtpos = page_prtpos;

	എങ്കിൽ (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    എങ്കിൽ (!mch_print_begin(&settings))
		പോകുക print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	വേണ്ടി (prt_page_എണ്ണം = 0; prtpos.file_വരി <= eap->line2; ++prt_page_എണ്ണം)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    വേണ്ടി (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		വേണ്ടി (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    എങ്കിൽ (got_int || settings.user_abort)
			പോകുക print_fail;

		    sprintf((അക്ഷരം *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_എണ്ണം + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (പൂർണ്ണസംഖ്യ)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (പൂർണ്ണസംഖ്യ)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    എങ്കിൽ (!mch_print_begin_page(IObuff))
			പോകുക print_fail;

		    എങ്കിൽ (settings.n_collated_copies > 1)
			sprintf((അക്ഷരം *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    എങ്കിൽ (prt_header_height() > 0)
			prt_header(&settings, prt_page_എണ്ണം + 1 + side,
							prtpos.file_വരി);

		    വേണ്ടി (page_വരി = 0; page_വരി < settings.വരികൾ_per_page;
								  ++page_വരി)
		    {
			prtpos.column = hardcopy_വരി(&settings,
							   page_വരി, &prtpos);
			എങ്കിൽ (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_വരി)));
			    എങ്കിൽ (++prtpos.file_വരി > eap->line2)
				നിർത്തുക; // reached the end
			}
			അല്ലെങ്കിൽ എങ്കിൽ (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    നിർത്തുക;
			}
		    }

		    എങ്കിൽ (!mch_print_end_page())
			പോകുക print_fail;
		    എങ്കിൽ (prtpos.file_വരി > eap->line2)
			നിർത്തുക; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		എങ്കിൽ (prtpos.file_വരി > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    എങ്കിൽ (!mch_print_blank_page())
			പോകുക print_fail;
		}
	    }
	    എങ്കിൽ (settings.duplex && prtpos.file_വരി <= eap->line2)
		++prt_page_എണ്ണം;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    എങ്കിൽ (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_സന്ദേശം
#endif
		))
    {
	sprintf((അക്ഷരം *)IObuff, "%s", _("Printing aborted"));
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
    സ്ഥിതം colnr_T
hardcopy_വരി(
    prt_settings_T	*psettings,
    പൂർണ്ണസംഖ്യ			page_വരി,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		need_break = FALSE;
    പൂർണ്ണസംഖ്യ		outputlen;
    പൂർണ്ണസംഖ്യ		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    പൂർണ്ണസംഖ്യ		id;
#endif

    എങ്കിൽ (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	എങ്കിൽ (!ppos->ff && prt_use_സംഖ്യ())
	    prt_വരി_സംഖ്യ(psettings, page_വരി, ppos->file_വരി);
	ppos->ff = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_വരി(0, page_വരി);
    വരി = ml_get(ppos->file_വരി);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    വേണ്ടി (col = ppos->column; വരി[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	എങ്കിൽ (has_mbyte && (outputlen = (*mb_ptr2len)(വരി + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	എങ്കിൽ (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_വരി, col, 1, ശൂന്യ, FALSE);
	    എങ്കിൽ (id > 0)
		id = syn_get_final_id(id);
	    അല്ലെങ്കിൽ
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    വരി = ml_get(ppos->file_വരി);

	    എങ്കിൽ (id != current_syn_id)
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
	എങ്കിൽ (വരി[col] == TAB || tab_spaces != 0)
	{
	    എങ്കിൽ (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
		tab_spaces = (പൂർണ്ണസംഖ്യ)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    ആകുമ്പോൾ (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		എങ്കിൽ (need_break)
		    നിർത്തുക;
	    }
	    // Keep the TAB if we didn't finish it.
	    എങ്കിൽ (need_break && tab_spaces > 0)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (വരി[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	അല്ലെങ്കിൽ
	{
	    need_break = mch_print_text_out(വരി + col, outputlen);
	    എങ്കിൽ (has_mbyte)
		print_pos += (*mb_ptr2cells)(വരി + col);
	    അല്ലെങ്കിൽ
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (പൂർണ്ണസംഖ്യ)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    എങ്കിൽ (!ppos->ff
	    && (വരി[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	തിരികെ 0;
    തിരികെ col;
}

#endif //FEAT_PRINTER
