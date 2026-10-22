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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ଯଦି defined(FEAT_PRINTER)

#ଅନ୍ତର୍ଭାଵ "hardcopy.h"
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
ସ୍ଥିର ଅଚଳ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ସ୍ଥିର ଅଚଳ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_syn_id;
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_italic;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_bold;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_page_ଗଣନା;

/*
 * These values determine the print position on a page.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lead_spaces;	    // remaining spaces for a TAB
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_ଧାଡ଼ି;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ff;		    // seen form feed character
} prt_pos_T;

#ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

ସ୍ଥିର ଅକ୍ଷର *parse_list_options(char_u *option_str, option_table_T *table, ପୂର୍ଣ୍ଣସଂଖ୍ୟା table_size);

ସ୍ଥିର colnr_T hardcopy_ଧାଡ଼ି(prt_settings_T *psettings, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଧାଡ଼ି, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    ଅକ୍ଷର *
parse_printoptions(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ parse_list_options(p_popt, printer_opts, OPT_PRINT_ଅଙ୍କ_OPTIONS);
}

#ଯଦି defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    ଅକ୍ଷର *
parse_printmbfont(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_ଅଙ୍କ_OPTIONS);
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
    ସ୍ଥିର ଅକ୍ଷର *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			table_size)
{
    option_table_T *old_opts;
    ଅକ୍ଷର	*ret = ଖାଲି;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;		// init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    ଯଦି (old_opts == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    ଯେତେବେଳେ (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	ଯଦି (colonp == ଖାଲି)
	{
	    ret = e_missing_colon_3;
	    ଭାଙ୍ଗ;
	}
	commap = vim_strchr(stringp, ',');
	ଯଦି (commap == ଖାଲି)
	    commap = option_str + STRLEN(option_str);

	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(colonp - stringp);

	ପାଇଁ (idx = 0; idx < table_size; ++idx)
	    ଯଦି (STRNICMP(stringp, table[idx].name, len) == 0)
		ଭାଙ୍ଗ;

	ଯଦି (idx == table_size)
	{
	    ret = e_illegal_component;
	    ଭାଙ୍ଗ;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	ଯଦି (table[idx].hasnum)
	{
	    ଯଦି (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		ଭାଙ୍ଗ;
	    }

	    table[idx].ସଂଖ୍ୟା = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(commap - p);

	stringp = commap;
	ଯଦି (*stringp == ',')
	    ++stringp;
    }

    ଯଦି (ret != ଖାଲି)
    {
	// Restore old options in case of error
	ପାଇଁ (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ଫେରନ୍ତୁ ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ସ୍ଥିର long_u
darken_rgb(long_u rgb)
{
    ଫେରନ୍ତୁ	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ସ୍ଥିର long_u
prt_get_term_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    ଯଦି (t_colors > 8)
	ଫେରନ୍ତୁ cterm_color_16[colorindex % 16];
    ଫେରନ୍ତୁ cterm_color_8[colorindex % 8];
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_get_attr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			hl_id,
    prt_text_attr_T	*pattr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			modec)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    ଅକ୍ଷର    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ଖାଲି);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ଖାଲି);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ଖାଲି);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ଖାଲି);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	ଯଦି (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    ଅନ୍ୟଥା
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (ଅକ୍ଷର *)highlight_color(hl_id, (char_u *)"fg", modec);
	ଯଦି (color == ଖାଲି)
	    colorindex = 0;
	ଅନ୍ୟଥା
	    colorindex = atoi(color);

	ଯଦି (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	ଅନ୍ୟଥା
	    fg_color = PRCOLOR_BLACK;
    }

    ଯଦି (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    ଅନ୍ୟଥା ଯଦି (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    ସ୍ଥିର ଶୂନ୍ୟ
prt_set_fg(long_u fg)
{
    ଯଦି (fg == prt_curr_fg)
	ଫେରନ୍ତୁ;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_set_bg(long_u bg)
{
    ଯଦି (bg == prt_curr_bg)
	ଫେରନ୍ତୁ;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_set_font(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold, ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline)
{
    ଯଦି (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ଫେରନ୍ତୁ;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_ଧାଡ଼ି_ସଂଖ୍ୟା(
    prt_settings_T *psettings,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		page_ଧାଡ଼ି,
    linenr_T	lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->ସଂଖ୍ୟା.fg_color);
    prt_set_bg(psettings->ସଂଖ୍ୟା.bg_color);
    prt_set_font(psettings->ସଂଖ୍ୟା.bold, psettings->ସଂଖ୍ୟା.italic, psettings->ସଂଖ୍ୟା.underline);
    mch_print_start_ଧାଡ଼ି(TRUE, page_ଧାଡ଼ି);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((ଅକ୍ଷର *)tbuf, "%6ld", (long)lnum);
    ପାଇଁ (i = 0; i < 6; i++)
	(ଶୂନ୍ୟ)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    ଯଦି (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_header_height(ଶୂନ୍ୟ)
{
    ଯଦି (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ଫେରନ୍ତୁ printer_opts[OPT_PRINT_HEADERHEIGHT].ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_use_ସଂଖ୍ୟା(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (printer_opts[OPT_PRINT_ସଂଖ୍ୟା].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_ସଂଖ୍ୟା].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_get_unit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u = PRT_UNIT_NONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ସ୍ଥିର ଅକ୍ଷର *(units[4]) = PRT_UNIT_NAMES;

    ଯଦି (!printer_opts[idx].present)
	ଫେରନ୍ତୁ PRT_UNIT_NONE;

    ପାଇଁ (i = 0; i < 4; ++i)
	ଯଦି (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    ଭାଙ୍ଗ;
	}
    ଫେରନ୍ତୁ u;
}

/*
 * Print the page header.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_header(
    prt_settings_T  *psettings,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pagenum,
    linenr_T	lnum UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width = psettings->chars_per_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		page_ଧାଡ଼ି;
    char_u	*tbuf;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    // Also use the space for the line number.
    ଯଦି (prt_use_ସଂଖ୍ୟା())
	width += PRINT_ସଂଖ୍ୟା_WIDTH;

    tbuf = alloc(width + IOSIZE);
    ଯଦି (tbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

#ifdef FEAT_STL_OPT
    ଯଦି (*p_header != NUL)
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
	printer_page_ଅଙ୍କ = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, ଖାଲି, ଖାଲି,
			 ଖାଲି);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    ଅନ୍ୟଥା
#endif
	sprintf((ଅକ୍ଷର *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_ଧାଡ଼ି = 0 - prt_header_height();
    mch_print_start_ଧାଡ଼ି(TRUE, page_ଧାଡ଼ି);
    ପାଇଁ (p = tbuf; *p != NUL; )
    {
	ଯଦି (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_ଧାଡ଼ି;
	    ଯଦି (page_ଧାଡ଼ି >= 0) // out of room in header
		ଭାଙ୍ଗ;
	    mch_print_start_ଧାଡ଼ି(TRUE, page_ଧାଡ଼ି);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    ଯଦି (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
prt_message(char_u *s)
{
    screen_fill((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, ' ', ' ', 0);
    screen_puts(s, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    ଶୂନ୍ୟ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			page_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_ସନ୍ଦେଶ = TRUE;
#endif

#ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ଯଦି (*eap->arg == '>')
    {
	ଅକ୍ଷର	*errormsg = ଖାଲି;

	// Expand things like "%.ps".
	ଯଦି (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    ଯଦି (errormsg != ଖାଲି)
		emsg(errormsg);
	    ଫେରନ୍ତୁ;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    ଅନ୍ୟଥା ଯଦି (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    ଯଦି (mch_print_init(&settings,
			curbuf->b_fname == ଖାଲି
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ଖାଲି
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ଫେରନ୍ତୁ;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    ଯଦି (gui.in_use)
	settings.modec = 'g';
    ଅନ୍ୟଥା
# endif
	ଯଦି (t_colors > 1)
	    settings.modec = 'c';
	ଅନ୍ୟଥା
	    settings.modec = 't';

    ଯଦି (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    ଅନ୍ୟଥା ଯଦି (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    ଅନ୍ୟଥା
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.ସଂଖ୍ୟା.fg_color = PRCOLOR_BLACK;
    settings.ସଂଖ୍ୟା.bg_color = PRCOLOR_WHITE;
    settings.ସଂଖ୍ୟା.bold = FALSE;
    settings.ସଂଖ୍ୟା.italic = TRUE;
    settings.ସଂଖ୍ୟା.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    ଯଦି (prt_use_ସଂଖ୍ୟା() && settings.do_syntax)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;

	id = syn_name2id((char_u *)"LineNr");
	ଯଦି (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.ସଂଖ୍ୟା, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    ପାଇଁ (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    ଯଦି (bytes_to_print == 0)
    {
	ସନ୍ଦେଶ(_("No text to be printed"));
	ଯାଅ print_fail_no_begin;
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

    ଯଦି (!mch_print_begin(&settings))
	ଯାଅ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_ଗଣନା = 0;
    ପାଇଁ (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_ଧାଡ଼ି = eap->line1;
	prtpos = page_prtpos;

	ଯଦି (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    ଯଦି (!mch_print_begin(&settings))
		ଯାଅ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	ପାଇଁ (prt_page_ଗଣନା = 0; prtpos.file_ଧାଡ଼ି <= eap->line2; ++prt_page_ଗଣନା)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    ପାଇଁ (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		ପାଇଁ (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    ଯଦି (got_int || settings.user_abort)
			ଯାଅ print_fail;

		    sprintf((ଅକ୍ଷର *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_ଗଣନା + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    ଯଦି (!mch_print_begin_page(IObuff))
			ଯାଅ print_fail;

		    ଯଦି (settings.n_collated_copies > 1)
			sprintf((ଅକ୍ଷର *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    ଯଦି (prt_header_height() > 0)
			prt_header(&settings, prt_page_ଗଣନା + 1 + side,
							prtpos.file_ଧାଡ଼ି);

		    ପାଇଁ (page_ଧାଡ଼ି = 0; page_ଧାଡ଼ି < settings.ଧାଡ଼ିଗୁଡ଼ିକ_per_page;
								  ++page_ଧାଡ଼ି)
		    {
			prtpos.column = hardcopy_ଧାଡ଼ି(&settings,
							   page_ଧାଡ଼ି, &prtpos);
			ଯଦି (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_ଧାଡ଼ି)));
			    ଯଦି (++prtpos.file_ଧାଡ଼ି > eap->line2)
				ଭାଙ୍ଗ; // reached the end
			}
			ଅନ୍ୟଥା ଯଦି (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    ଭାଙ୍ଗ;
			}
		    }

		    ଯଦି (!mch_print_end_page())
			ଯାଅ print_fail;
		    ଯଦି (prtpos.file_ଧାଡ଼ି > eap->line2)
			ଭାଙ୍ଗ; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		ଯଦି (prtpos.file_ଧାଡ଼ି > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    ଯଦି (!mch_print_blank_page())
			ଯାଅ print_fail;
		}
	    }
	    ଯଦି (settings.duplex && prtpos.file_ଧାଡ଼ି <= eap->line2)
		++prt_page_ଗଣନା;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    ଯଦି (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_ସନ୍ଦେଶ
#endif
		))
    {
	sprintf((ଅକ୍ଷର *)IObuff, "%s", _("Printing aborted"));
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
    ସ୍ଥିର colnr_T
hardcopy_ଧାଡ଼ି(
    prt_settings_T	*psettings,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			page_ଧାଡ଼ି,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_break = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		outputlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
#endif

    ଯଦି (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	ଯଦି (!ppos->ff && prt_use_ସଂଖ୍ୟା())
	    prt_ଧାଡ଼ି_ସଂଖ୍ୟା(psettings, page_ଧାଡ଼ି, ppos->file_ଧାଡ଼ି);
	ppos->ff = FALSE;
    }
    ଅନ୍ୟଥା
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_ଧାଡ଼ି(0, page_ଧାଡ଼ି);
    ଧାଡ଼ି = ml_get(ppos->file_ଧାଡ଼ି);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    ପାଇଁ (col = ppos->column; ଧାଡ଼ି[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	ଯଦି (has_mbyte && (outputlen = (*mb_ptr2len)(ଧାଡ଼ି + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	ଯଦି (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_ଧାଡ଼ି, col, 1, ଖାଲି, FALSE);
	    ଯଦି (id > 0)
		id = syn_get_final_id(id);
	    ଅନ୍ୟଥା
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    ଧାଡ଼ି = ml_get(ppos->file_ଧାଡ଼ି);

	    ଯଦି (id != current_syn_id)
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
	ଯଦି (ଧାଡ଼ି[col] == TAB || tab_spaces != 0)
	{
	    ଯଦି (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
		tab_spaces = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    ଯେତେବେଳେ (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		ଯଦି (need_break)
		    ଭାଙ୍ଗ;
	    }
	    // Keep the TAB if we didn't finish it.
	    ଯଦି (need_break && tab_spaces > 0)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	ଅନ୍ୟଥା
	{
	    need_break = mch_print_text_out(ଧାଡ଼ି + col, outputlen);
	    ଯଦି (has_mbyte)
		print_pos += (*mb_ptr2cells)(ଧାଡ଼ି + col);
	    ଅନ୍ୟଥା
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    ଯଦି (!ppos->ff
	    && (ଧାଡ଼ି[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ col;
}

#endif //FEAT_PRINTER
