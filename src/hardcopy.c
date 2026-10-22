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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#எனில் defined(FEAT_PRINTER)

#அந்தர்பாவ "hardcopy.h"
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
நிலையான மாறிலி long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

நிலையான மாறிலி long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

நிலையான முழு		current_syn_id;
#endif

முழு	prt_curr_italic;
முழு	prt_curr_bold;
முழு	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
முழு	prt_page_மொத்தம்;

/*
 * These values determine the print position on a page.
 */
வகைவரை கட்டமைப்பு
{
    முழு		lead_spaces;	    // remaining spaces for a TAB
    முழு		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_வரி;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    முழு		ff;		    // seen form feed character
} prt_pos_T;

#எனில் defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

நிலையான எழுத்து *parse_list_options(char_u *option_str, option_table_T *table, முழு table_size);

நிலையான colnr_T hardcopy_வரி(prt_settings_T *psettings, முழு page_வரி, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    எழுத்து *
parse_printoptions(optset_T *args UNUSED)
{
    திரும்பு parse_list_options(p_popt, printer_opts, OPT_PRINT_இலக்கம்_OPTIONS);
}

#எனில் defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    எழுத்து *
parse_printmbfont(optset_T *args UNUSED)
{
    திரும்பு parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_இலக்கம்_OPTIONS);
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
    நிலையான எழுத்து *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    முழு			table_size)
{
    option_table_T *old_opts;
    எழுத்து	*ret = வெற்று;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    முழு		idx = 0;		// init for GCC
    முழு		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    எனில் (old_opts == வெற்று)
	திரும்பு வெற்று;

    க்கு (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    போது (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	எனில் (colonp == வெற்று)
	{
	    ret = e_missing_colon_3;
	    நிறுத்து;
	}
	commap = vim_strchr(stringp, ',');
	எனில் (commap == வெற்று)
	    commap = option_str + STRLEN(option_str);

	len = (முழு)(colonp - stringp);

	க்கு (idx = 0; idx < table_size; ++idx)
	    எனில் (STRNICMP(stringp, table[idx].name, len) == 0)
		நிறுத்து;

	எனில் (idx == table_size)
	{
	    ret = e_illegal_component;
	    நிறுத்து;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	எனில் (table[idx].hasnum)
	{
	    எனில் (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		நிறுத்து;
	    }

	    table[idx].எண் = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (முழு)(commap - p);

	stringp = commap;
	எனில் (*stringp == ',')
	    ++stringp;
    }

    எனில் (ret != வெற்று)
    {
	// Restore old options in case of error
	க்கு (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    திரும்பு ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    நிலையான long_u
darken_rgb(long_u rgb)
{
    திரும்பு	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    நிலையான long_u
prt_get_term_color(முழு colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    எனில் (t_colors > 8)
	திரும்பு cterm_color_16[colorindex % 16];
    திரும்பு cterm_color_8[colorindex % 8];
}

    நிலையான வெறுமை
prt_get_attr(
    முழு			hl_id,
    prt_text_attr_T	*pattr,
    முழு			modec)
{
    முழு     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    எழுத்து    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != வெற்று);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != வெற்று);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != வெற்று);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != வெற்று);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	எனில் (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    இல்லையெனில்
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (எழுத்து *)highlight_color(hl_id, (char_u *)"fg", modec);
	எனில் (color == வெற்று)
	    colorindex = 0;
	இல்லையெனில்
	    colorindex = atoi(color);

	எனில் (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	இல்லையெனில்
	    fg_color = PRCOLOR_BLACK;
    }

    எனில் (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    இல்லையெனில் எனில் (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    நிலையான வெறுமை
prt_set_fg(long_u fg)
{
    எனில் (fg == prt_curr_fg)
	திரும்பு;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    நிலையான வெறுமை
prt_set_bg(long_u bg)
{
    எனில் (bg == prt_curr_bg)
	திரும்பு;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    நிலையான வெறுமை
prt_set_font(முழு bold, முழு italic, முழு underline)
{
    எனில் (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	திரும்பு;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    நிலையான வெறுமை
prt_வரி_எண்(
    prt_settings_T *psettings,
    முழு		page_வரி,
    linenr_T	lnum)
{
    முழு		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->எண்.fg_color);
    prt_set_bg(psettings->எண்.bg_color);
    prt_set_font(psettings->எண்.bold, psettings->எண்.italic, psettings->எண்.underline);
    mch_print_start_வரி(TRUE, page_வரி);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((எழுத்து *)tbuf, "%6ld", (long)lnum);
    க்கு (i = 0; i < 6; i++)
	(வெறுமை)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    எனில் (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    இல்லையெனில்
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
    முழு
prt_header_height(வெறுமை)
{
    எனில் (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	திரும்பு printer_opts[OPT_PRINT_HEADERHEIGHT].எண்;
    திரும்பு 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    முழு
prt_use_எண்(வெறுமை)
{
    திரும்பு (printer_opts[OPT_PRINT_எண்].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_எண்].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    முழு
prt_get_unit(முழு idx)
{
    முழு		u = PRT_UNIT_NONE;
    முழு		i;
    நிலையான எழுத்து *(units[4]) = PRT_UNIT_NAMES;

    எனில் (!printer_opts[idx].present)
	திரும்பு PRT_UNIT_NONE;

    க்கு (i = 0; i < 4; ++i)
	எனில் (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    நிறுத்து;
	}
    திரும்பு u;
}

/*
 * Print the page header.
 */
    நிலையான வெறுமை
prt_header(
    prt_settings_T  *psettings,
    முழு		pagenum,
    linenr_T	lnum UNUSED)
{
    முழு		width = psettings->chars_per_வரி;
    முழு		page_வரி;
    char_u	*tbuf;
    char_u	*p;
    முழு		l;

    // Also use the space for the line number.
    எனில் (prt_use_எண்())
	width += PRINT_எண்_WIDTH;

    tbuf = alloc(width + IOSIZE);
    எனில் (tbuf == வெற்று)
	திரும்பு;

#ifdef FEAT_STL_OPT
    எனில் (*p_header != NUL)
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
	printer_page_இலக்கம் = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, வெற்று, வெற்று,
			 வெற்று);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    இல்லையெனில்
#endif
	sprintf((எழுத்து *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_வரி = 0 - prt_header_height();
    mch_print_start_வரி(TRUE, page_வரி);
    க்கு (p = tbuf; *p != NUL; )
    {
	எனில் (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_வரி;
	    எனில் (page_வரி >= 0) // out of room in header
		நிறுத்து;
	    mch_print_start_வரி(TRUE, page_வரி);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    எனில் (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    இல்லையெனில்
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
    வெறுமை
prt_message(char_u *s)
{
    screen_fill((முழு)Rows - 1, (முழு)Rows, 0, (முழு)Columns, ' ', ' ', 0);
    screen_puts(s, (முழு)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    வெறுமை
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    முழு			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    முழு			page_வரி;
    முழு			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_செய்தி = TRUE;
#endif

#எனில் defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    எனில் (*eap->arg == '>')
    {
	எழுத்து	*errormsg = வெற்று;

	// Expand things like "%.ps".
	எனில் (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    எனில் (errormsg != வெற்று)
		emsg(errormsg);
	    திரும்பு;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    இல்லையெனில் எனில் (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    எனில் (mch_print_init(&settings,
			curbuf->b_fname == வெற்று
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == வெற்று
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	திரும்பு;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    எனில் (gui.in_use)
	settings.modec = 'g';
    இல்லையெனில்
# endif
	எனில் (t_colors > 1)
	    settings.modec = 'c';
	இல்லையெனில்
	    settings.modec = 't';

    எனில் (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    இல்லையெனில் எனில் (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    இல்லையெனில்
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.எண்.fg_color = PRCOLOR_BLACK;
    settings.எண்.bg_color = PRCOLOR_WHITE;
    settings.எண்.bold = FALSE;
    settings.எண்.italic = TRUE;
    settings.எண்.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    எனில் (prt_use_எண்() && settings.do_syntax)
    {
	முழு		id;

	id = syn_name2id((char_u *)"LineNr");
	எனில் (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.எண், settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    க்கு (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    எனில் (bytes_to_print == 0)
    {
	செய்தி(_("No text to be printed"));
	செல் print_fail_no_begin;
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

    எனில் (!mch_print_begin(&settings))
	செல் print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_மொத்தம் = 0;
    க்கு (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	முழு		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_வரி = eap->line1;
	prtpos = page_prtpos;

	எனில் (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    எனில் (!mch_print_begin(&settings))
		செல் print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	க்கு (prt_page_மொத்தம் = 0; prtpos.file_வரி <= eap->line2; ++prt_page_மொத்தம்)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    க்கு (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		க்கு (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    எனில் (got_int || settings.user_abort)
			செல் print_fail;

		    sprintf((எழுத்து *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_மொத்தம் + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (முழு)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (முழு)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    எனில் (!mch_print_begin_page(IObuff))
			செல் print_fail;

		    எனில் (settings.n_collated_copies > 1)
			sprintf((எழுத்து *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    எனில் (prt_header_height() > 0)
			prt_header(&settings, prt_page_மொத்தம் + 1 + side,
							prtpos.file_வரி);

		    க்கு (page_வரி = 0; page_வரி < settings.வரிகள்_per_page;
								  ++page_வரி)
		    {
			prtpos.column = hardcopy_வரி(&settings,
							   page_வரி, &prtpos);
			எனில் (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_வரி)));
			    எனில் (++prtpos.file_வரி > eap->line2)
				நிறுத்து; // reached the end
			}
			இல்லையெனில் எனில் (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    நிறுத்து;
			}
		    }

		    எனில் (!mch_print_end_page())
			செல் print_fail;
		    எனில் (prtpos.file_வரி > eap->line2)
			நிறுத்து; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		எனில் (prtpos.file_வரி > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    எனில் (!mch_print_blank_page())
			செல் print_fail;
		}
	    }
	    எனில் (settings.duplex && prtpos.file_வரி <= eap->line2)
		++prt_page_மொத்தம்;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    எனில் (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_செய்தி
#endif
		))
    {
	sprintf((எழுத்து *)IObuff, "%s", _("Printing aborted"));
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
    நிலையான colnr_T
hardcopy_வரி(
    prt_settings_T	*psettings,
    முழு			page_வரி,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*வரி;
    முழு		need_break = FALSE;
    முழு		outputlen;
    முழு		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    முழு		id;
#endif

    எனில் (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	எனில் (!ppos->ff && prt_use_எண்())
	    prt_வரி_எண்(psettings, page_வரி, ppos->file_வரி);
	ppos->ff = FALSE;
    }
    இல்லையெனில்
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_வரி(0, page_வரி);
    வரி = ml_get(ppos->file_வரி);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    க்கு (col = ppos->column; வரி[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	எனில் (has_mbyte && (outputlen = (*mb_ptr2len)(வரி + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	எனில் (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_வரி, col, 1, வெற்று, FALSE);
	    எனில் (id > 0)
		id = syn_get_final_id(id);
	    இல்லையெனில்
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    வரி = ml_get(ppos->file_வரி);

	    எனில் (id != current_syn_id)
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
	எனில் (வரி[col] == TAB || tab_spaces != 0)
	{
	    எனில் (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#இல்லையெனில்
		tab_spaces = (முழு)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    போது (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		எனில் (need_break)
		    நிறுத்து;
	    }
	    // Keep the TAB if we didn't finish it.
	    எனில் (need_break && tab_spaces > 0)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (வரி[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	இல்லையெனில்
	{
	    need_break = mch_print_text_out(வரி + col, outputlen);
	    எனில் (has_mbyte)
		print_pos += (*mb_ptr2cells)(வரி + col);
	    இல்லையெனில்
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (முழு)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    எனில் (!ppos->ff
	    && (வரி[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	திரும்பு 0;
    திரும்பு col;
}

#endif //FEAT_PRINTER
