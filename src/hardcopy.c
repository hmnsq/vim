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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "version.h"

#যদি defined(FEAT_PRINTER)

#অন্তর্ভাৱ "hardcopy.h"
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
স্থির ধ্রুৱ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

স্থির ধ্রুৱ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

স্থির পূর্ণাঙ্ক		current_syn_id;
#endif

পূর্ণাঙ্ক	prt_curr_italic;
পূর্ণাঙ্ক	prt_curr_bold;
পূর্ণাঙ্ক	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
পূর্ণাঙ্ক	prt_page_গুনতি;

/*
 * These values determine the print position on a page.
 */
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		lead_spaces;	    // remaining spaces for a TAB
    পূর্ণাঙ্ক		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_সাৰি;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    পূর্ণাঙ্ক		ff;		    // seen form feed character
} prt_pos_T;

#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

স্থির ৱর্ণ *parse_list_options(char_u *option_str, option_table_T *table, পূর্ণাঙ্ক table_size);

স্থির colnr_T hardcopy_সাৰি(prt_settings_T *psettings, পূর্ণাঙ্ক page_সাৰি, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    ৱর্ণ *
parse_printoptions(optset_T *args UNUSED)
{
    ঘূৰাই parse_list_options(p_popt, printer_opts, OPT_PRINT_অঙ্ক_OPTIONS);
}

#যদি defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    ৱর্ণ *
parse_printmbfont(optset_T *args UNUSED)
{
    ঘূৰাই parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_অঙ্ক_OPTIONS);
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
    স্থির ৱর্ণ *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    পূর্ণাঙ্ক			table_size)
{
    option_table_T *old_opts;
    ৱর্ণ	*ret = রিক্ত;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    পূর্ণাঙ্ক		idx = 0;		// init for GCC
    পূর্ণাঙ্ক		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    যদি (old_opts == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    যেতিয়ালৈ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	যদি (colonp == রিক্ত)
	{
	    ret = e_missing_colon_3;
	    ভঙ্গ;
	}
	commap = vim_strchr(stringp, ',');
	যদি (commap == রিক্ত)
	    commap = option_str + STRLEN(option_str);

	len = (পূর্ণাঙ্ক)(colonp - stringp);

	বাবে (idx = 0; idx < table_size; ++idx)
	    যদি (STRNICMP(stringp, table[idx].name, len) == 0)
		ভঙ্গ;

	যদি (idx == table_size)
	{
	    ret = e_illegal_component;
	    ভঙ্গ;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	যদি (table[idx].hasnum)
	{
	    যদি (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ভঙ্গ;
	    }

	    table[idx].সংখ্যা = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (পূর্ণাঙ্ক)(commap - p);

	stringp = commap;
	যদি (*stringp == ',')
	    ++stringp;
    }

    যদি (ret != রিক্ত)
    {
	// Restore old options in case of error
	বাবে (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ঘূৰাই ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    স্থির long_u
darken_rgb(long_u rgb)
{
    ঘূৰাই	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    স্থির long_u
prt_get_term_color(পূর্ণাঙ্ক colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    যদি (t_colors > 8)
	ঘূৰাই cterm_color_16[colorindex % 16];
    ঘূৰাই cterm_color_8[colorindex % 8];
}

    স্থির শূন্য
prt_get_attr(
    পূর্ণাঙ্ক			hl_id,
    prt_text_attr_T	*pattr,
    পূর্ণাঙ্ক			modec)
{
    পূর্ণাঙ্ক     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    ৱর্ণ    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != রিক্ত);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != রিক্ত);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != রিক্ত);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != রিক্ত);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	যদি (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    অন্যথা
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (ৱর্ণ *)highlight_color(hl_id, (char_u *)"fg", modec);
	যদি (color == রিক্ত)
	    colorindex = 0;
	অন্যথা
	    colorindex = atoi(color);

	যদি (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	অন্যথা
	    fg_color = PRCOLOR_BLACK;
    }

    যদি (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    অন্যথা যদি (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    স্থির শূন্য
prt_set_fg(long_u fg)
{
    যদি (fg == prt_curr_fg)
	ঘূৰাই;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    স্থির শূন্য
prt_set_bg(long_u bg)
{
    যদি (bg == prt_curr_bg)
	ঘূৰাই;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    স্থির শূন্য
prt_set_font(পূর্ণাঙ্ক bold, পূর্ণাঙ্ক italic, পূর্ণাঙ্ক underline)
{
    যদি (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ঘূৰাই;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    স্থির শূন্য
prt_সাৰি_সংখ্যা(
    prt_settings_T *psettings,
    পূর্ণাঙ্ক		page_সাৰি,
    linenr_T	lnum)
{
    পূর্ণাঙ্ক		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->সংখ্যা.fg_color);
    prt_set_bg(psettings->সংখ্যা.bg_color);
    prt_set_font(psettings->সংখ্যা.bold, psettings->সংখ্যা.italic, psettings->সংখ্যা.underline);
    mch_print_start_সাৰি(TRUE, page_সাৰি);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((ৱর্ণ *)tbuf, "%6ld", (long)lnum);
    বাবে (i = 0; i < 6; i++)
	(শূন্য)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    যদি (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    অন্যথা
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
    পূর্ণাঙ্ক
prt_header_height(শূন্য)
{
    যদি (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ঘূৰাই printer_opts[OPT_PRINT_HEADERHEIGHT].সংখ্যা;
    ঘূৰাই 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    পূর্ণাঙ্ক
prt_use_সংখ্যা(শূন্য)
{
    ঘূৰাই (printer_opts[OPT_PRINT_সংখ্যা].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_সংখ্যা].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    পূর্ণাঙ্ক
prt_get_unit(পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক		u = PRT_UNIT_NONE;
    পূর্ণাঙ্ক		i;
    স্থির ৱর্ণ *(units[4]) = PRT_UNIT_NAMES;

    যদি (!printer_opts[idx].present)
	ঘূৰাই PRT_UNIT_NONE;

    বাবে (i = 0; i < 4; ++i)
	যদি (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ভঙ্গ;
	}
    ঘূৰাই u;
}

/*
 * Print the page header.
 */
    স্থির শূন্য
prt_header(
    prt_settings_T  *psettings,
    পূর্ণাঙ্ক		pagenum,
    linenr_T	lnum UNUSED)
{
    পূর্ণাঙ্ক		width = psettings->chars_per_সাৰি;
    পূর্ণাঙ্ক		page_সাৰি;
    char_u	*tbuf;
    char_u	*p;
    পূর্ণাঙ্ক		l;

    // Also use the space for the line number.
    যদি (prt_use_সংখ্যা())
	width += PRINT_সংখ্যা_WIDTH;

    tbuf = alloc(width + IOSIZE);
    যদি (tbuf == রিক্ত)
	ঘূৰাই;

#ifdef FEAT_STL_OPT
    যদি (*p_header != NUL)
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
	printer_page_অঙ্ক = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, রিক্ত, রিক্ত,
			 রিক্ত);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    অন্যথা
#endif
	sprintf((ৱর্ণ *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_সাৰি = 0 - prt_header_height();
    mch_print_start_সাৰি(TRUE, page_সাৰি);
    বাবে (p = tbuf; *p != NUL; )
    {
	যদি (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_সাৰি;
	    যদি (page_সাৰি >= 0) // out of room in header
		ভঙ্গ;
	    mch_print_start_সাৰি(TRUE, page_সাৰি);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    যদি (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    অন্যথা
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
    শূন্য
prt_message(char_u *s)
{
    screen_fill((পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Rows, 0, (পূর্ণাঙ্ক)Columns, ' ', ' ', 0);
    screen_puts(s, (পূর্ণাঙ্ক)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    শূন্য
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    পূর্ণাঙ্ক			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    পূর্ণাঙ্ক			page_সাৰি;
    পূর্ণাঙ্ক			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_বাৰ্তা = TRUE;
#endif

#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    যদি (*eap->arg == '>')
    {
	ৱর্ণ	*errormsg = রিক্ত;

	// Expand things like "%.ps".
	যদি (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    যদি (errormsg != রিক্ত)
		emsg(errormsg);
	    ঘূৰাই;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    অন্যথা যদি (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    যদি (mch_print_init(&settings,
			curbuf->b_fname == রিক্ত
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == রিক্ত
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ঘূৰাই;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    যদি (gui.in_use)
	settings.modec = 'g';
    অন্যথা
# endif
	যদি (t_colors > 1)
	    settings.modec = 'c';
	অন্যথা
	    settings.modec = 't';

    যদি (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    অন্যথা যদি (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    অন্যথা
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.সংখ্যা.fg_color = PRCOLOR_BLACK;
    settings.সংখ্যা.bg_color = PRCOLOR_WHITE;
    settings.সংখ্যা.bold = FALSE;
    settings.সংখ্যা.italic = TRUE;
    settings.সংখ্যা.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    যদি (prt_use_সংখ্যা() && settings.do_syntax)
    {
	পূর্ণাঙ্ক		id;

	id = syn_name2id((char_u *)"LineNr");
	যদি (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.সংখ্যা, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    বাবে (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    যদি (bytes_to_print == 0)
    {
	বাৰ্তা(_("No text to be printed"));
	গচ্ছ print_fail_no_begin;
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

    যদি (!mch_print_begin(&settings))
	গচ্ছ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_গুনতি = 0;
    বাবে (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	পূর্ণাঙ্ক		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_সাৰি = eap->line1;
	prtpos = page_prtpos;

	যদি (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    যদি (!mch_print_begin(&settings))
		গচ্ছ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	বাবে (prt_page_গুনতি = 0; prtpos.file_সাৰি <= eap->line2; ++prt_page_গুনতি)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    বাবে (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		বাবে (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    যদি (got_int || settings.user_abort)
			গচ্ছ print_fail;

		    sprintf((ৱর্ণ *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_গুনতি + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (পূর্ণাঙ্ক)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (পূর্ণাঙ্ক)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    যদি (!mch_print_begin_page(IObuff))
			গচ্ছ print_fail;

		    যদি (settings.n_collated_copies > 1)
			sprintf((ৱর্ণ *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    যদি (prt_header_height() > 0)
			prt_header(&settings, prt_page_গুনতি + 1 + side,
							prtpos.file_সাৰি);

		    বাবে (page_সাৰি = 0; page_সাৰি < settings.সাৰিগুলি_per_page;
								  ++page_সাৰি)
		    {
			prtpos.column = hardcopy_সাৰি(&settings,
							   page_সাৰি, &prtpos);
			যদি (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_সাৰি)));
			    যদি (++prtpos.file_সাৰি > eap->line2)
				ভঙ্গ; // reached the end
			}
			অন্যথা যদি (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ভঙ্গ;
			}
		    }

		    যদি (!mch_print_end_page())
			গচ্ছ print_fail;
		    যদি (prtpos.file_সাৰি > eap->line2)
			ভঙ্গ; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		যদি (prtpos.file_সাৰি > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    যদি (!mch_print_blank_page())
			গচ্ছ print_fail;
		}
	    }
	    যদি (settings.duplex && prtpos.file_সাৰি <= eap->line2)
		++prt_page_গুনতি;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    যদি (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_বাৰ্তা
#endif
		))
    {
	sprintf((ৱর্ণ *)IObuff, "%s", _("Printing aborted"));
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
    স্থির colnr_T
hardcopy_সাৰি(
    prt_settings_T	*psettings,
    পূর্ণাঙ্ক			page_সাৰি,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*সাৰি;
    পূর্ণাঙ্ক		need_break = FALSE;
    পূর্ণাঙ্ক		outputlen;
    পূর্ণাঙ্ক		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    পূর্ণাঙ্ক		id;
#endif

    যদি (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	যদি (!ppos->ff && prt_use_সংখ্যা())
	    prt_সাৰি_সংখ্যা(psettings, page_সাৰি, ppos->file_সাৰি);
	ppos->ff = FALSE;
    }
    অন্যথা
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_সাৰি(0, page_সাৰি);
    সাৰি = ml_get(ppos->file_সাৰি);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    বাবে (col = ppos->column; সাৰি[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	যদি (has_mbyte && (outputlen = (*mb_ptr2len)(সাৰি + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	যদি (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_সাৰি, col, 1, রিক্ত, FALSE);
	    যদি (id > 0)
		id = syn_get_final_id(id);
	    অন্যথা
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    সাৰি = ml_get(ppos->file_সাৰি);

	    যদি (id != current_syn_id)
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
	যদি (সাৰি[col] == TAB || tab_spaces != 0)
	{
	    যদি (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#অন্যথা
		tab_spaces = (পূর্ণাঙ্ক)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    যেতিয়ালৈ (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		যদি (need_break)
		    ভঙ্গ;
	    }
	    // Keep the TAB if we didn't finish it.
	    যদি (need_break && tab_spaces > 0)
		ভঙ্গ;
	}
	অন্যথা যদি (সাৰি[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	অন্যথা
	{
	    need_break = mch_print_text_out(সাৰি + col, outputlen);
	    যদি (has_mbyte)
		print_pos += (*mb_ptr2cells)(সাৰি + col);
	    অন্যথা
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (পূর্ণাঙ্ক)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    যদি (!ppos->ff
	    && (সাৰি[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ঘূৰাই 0;
    ঘূৰাই col;
}

#endif //FEAT_PRINTER
