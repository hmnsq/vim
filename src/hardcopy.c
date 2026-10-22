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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#जर defined(FEAT_PRINTER)

#अन्तर्भाव "hardcopy.h"
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
स्थिर स्थिरांक long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

स्थिर स्थिरांक long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

स्थिर पूर्णांक		current_syn_id;
#endif

पूर्णांक	prt_curr_italic;
पूर्णांक	prt_curr_bold;
पूर्णांक	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
पूर्णांक	prt_page_मोजणी;

/*
 * These values determine the print position on a page.
 */
प्रकारव्याख्या संरचना
{
    पूर्णांक		lead_spaces;	    // remaining spaces for a TAB
    पूर्णांक		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_ओळ;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    पूर्णांक		ff;		    // seen form feed character
} prt_pos_T;

#जर defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

स्थिर अक्षर *parse_list_options(char_u *option_str, option_table_T *table, पूर्णांक table_size);

स्थिर colnr_T hardcopy_ओळ(prt_settings_T *psettings, पूर्णांक page_ओळ, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    अक्षर *
parse_printoptions(optset_T *args UNUSED)
{
    परत parse_list_options(p_popt, printer_opts, OPT_PRINT_अंक_OPTIONS);
}

#जर defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    अक्षर *
parse_printmbfont(optset_T *args UNUSED)
{
    परत parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_अंक_OPTIONS);
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
    स्थिर अक्षर *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    पूर्णांक			table_size)
{
    option_table_T *old_opts;
    अक्षर	*ret = रिक्त;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    पूर्णांक		idx = 0;		// init for GCC
    पूर्णांक		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    जर (old_opts == रिक्त)
	परत रिक्त;

    साठी (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    पर्यंत (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	जर (colonp == रिक्त)
	{
	    ret = e_missing_colon_3;
	    थांबा;
	}
	commap = vim_strchr(stringp, ',');
	जर (commap == रिक्त)
	    commap = option_str + STRLEN(option_str);

	len = (पूर्णांक)(colonp - stringp);

	साठी (idx = 0; idx < table_size; ++idx)
	    जर (STRNICMP(stringp, table[idx].name, len) == 0)
		थांबा;

	जर (idx == table_size)
	{
	    ret = e_illegal_component;
	    थांबा;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	जर (table[idx].hasnum)
	{
	    जर (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		थांबा;
	    }

	    table[idx].संख्या = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].म_माप = (पूर्णांक)(commap - p);

	stringp = commap;
	जर (*stringp == ',')
	    ++stringp;
    }

    जर (ret != रिक्त)
    {
	// Restore old options in case of error
	साठी (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    परत ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    स्थिर long_u
darken_rgb(long_u rgb)
{
    परत	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    स्थिर long_u
prt_get_term_color(पूर्णांक colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    जर (t_colors > 8)
	परत cterm_color_16[colorindex % 16];
    परत cterm_color_8[colorindex % 8];
}

    स्थिर शून्य
prt_get_attr(
    पूर्णांक			hl_id,
    prt_text_attr_T	*pattr,
    पूर्णांक			modec)
{
    पूर्णांक     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    अक्षर    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != रिक्त);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != रिक्त);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != रिक्त);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != रिक्त);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	जर (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    अन्यथा
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (अक्षर *)highlight_color(hl_id, (char_u *)"fg", modec);
	जर (color == रिक्त)
	    colorindex = 0;
	अन्यथा
	    colorindex = म_से_प(color);

	जर (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	अन्यथा
	    fg_color = PRCOLOR_BLACK;
    }

    जर (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    अन्यथा जर (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    स्थिर शून्य
prt_set_fg(long_u fg)
{
    जर (fg == prt_curr_fg)
	परत;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    स्थिर शून्य
prt_set_bg(long_u bg)
{
    जर (bg == prt_curr_bg)
	परत;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    स्थिर शून्य
prt_set_font(पूर्णांक bold, पूर्णांक italic, पूर्णांक underline)
{
    जर (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	परत;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    स्थिर शून्य
prt_ओळ_संख्या(
    prt_settings_T *psettings,
    पूर्णांक		page_ओळ,
    linenr_T	lnum)
{
    पूर्णांक		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->संख्या.fg_color);
    prt_set_bg(psettings->संख्या.bg_color);
    prt_set_font(psettings->संख्या.bold, psettings->संख्या.italic, psettings->संख्या.underline);
    mch_print_start_ओळ(TRUE, page_ओळ);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    प्र_लिखो((अक्षर *)tbuf, "%6ld", (दीर्घ)lnum);
    साठी (i = 0; i < 6; i++)
	(शून्य)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    जर (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    अन्यथा
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
    पूर्णांक
prt_header_height(शून्य)
{
    जर (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	परत printer_opts[OPT_PRINT_HEADERHEIGHT].संख्या;
    परत 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    पूर्णांक
prt_use_संख्या(शून्य)
{
    परत (printer_opts[OPT_PRINT_संख्या].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_संख्या].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    पूर्णांक
prt_get_unit(पूर्णांक idx)
{
    पूर्णांक		u = PRT_UNIT_NONE;
    पूर्णांक		i;
    स्थिर अक्षर *(units[4]) = PRT_UNIT_NAMES;

    जर (!printer_opts[idx].present)
	परत PRT_UNIT_NONE;

    साठी (i = 0; i < 4; ++i)
	जर (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    थांबा;
	}
    परत u;
}

/*
 * Print the page header.
 */
    स्थिर शून्य
prt_header(
    prt_settings_T  *psettings,
    पूर्णांक		pagenum,
    linenr_T	lnum UNUSED)
{
    पूर्णांक		width = psettings->chars_per_ओळ;
    पूर्णांक		page_ओळ;
    char_u	*tbuf;
    char_u	*p;
    पूर्णांक		l;

    // Also use the space for the line number.
    जर (prt_use_संख्या())
	width += PRINT_संख्या_WIDTH;

    tbuf = alloc(width + IOSIZE);
    जर (tbuf == रिक्त)
	परत;

#ifdef FEAT_STL_OPT
    जर (*p_header != NUL)
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
	printer_page_अंक = pagenum;

	build_stl_str_hl(curwin, tbuf, (माप_प्रकार)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, रिक्त, रिक्त,
			 रिक्त);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    अन्यथा
#endif
	प्र_लिखो((अक्षर *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_ओळ = 0 - prt_header_height();
    mch_print_start_ओळ(TRUE, page_ओळ);
    साठी (p = tbuf; *p != NUL; )
    {
	जर (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_ओळ;
	    जर (page_ओळ >= 0) // out of room in header
		थांबा;
	    mch_print_start_ओळ(TRUE, page_ओळ);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    जर (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    अन्यथा
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
    शून्य
prt_message(char_u *s)
{
    screen_fill((पूर्णांक)Rows - 1, (पूर्णांक)Rows, 0, (पूर्णांक)Columns, ' ', ' ', 0);
    screen_puts(s, (पूर्णांक)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    शून्य
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    पूर्णांक			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    पूर्णांक			page_ओळ;
    पूर्णांक			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_संदेश = TRUE;
#endif

#जर defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    जर (*eap->arg == '>')
    {
	अक्षर	*errormsg = रिक्त;

	// Expand things like "%.ps".
	जर (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    जर (errormsg != रिक्त)
		emsg(errormsg);
	    परत;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    अन्यथा जर (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    जर (mch_print_init(&settings,
			curbuf->b_fname == रिक्त
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == रिक्त
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	परत;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    जर (gui.in_use)
	settings.modec = 'g';
    अन्यथा
# endif
	जर (t_colors > 1)
	    settings.modec = 'c';
	अन्यथा
	    settings.modec = 't';

    जर (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    अन्यथा जर (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    अन्यथा
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.संख्या.fg_color = PRCOLOR_BLACK;
    settings.संख्या.bg_color = PRCOLOR_WHITE;
    settings.संख्या.bold = FALSE;
    settings.संख्या.italic = TRUE;
    settings.संख्या.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    जर (prt_use_संख्या() && settings.do_syntax)
    {
	पूर्णांक		id;

	id = syn_name2id((char_u *)"LineNr");
	जर (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.संख्या, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    साठी (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    जर (bytes_to_print == 0)
    {
	संदेश(_("No text to be printed"));
	जा print_fail_no_begin;
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

    जर (!mch_print_begin(&settings))
	जा print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_मोजणी = 0;
    साठी (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	पूर्णांक		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_ओळ = eap->line1;
	prtpos = page_prtpos;

	जर (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    जर (!mch_print_begin(&settings))
		जा print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	साठी (prt_page_मोजणी = 0; prtpos.file_ओळ <= eap->line2; ++prt_page_मोजणी)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    साठी (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		साठी (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    जर (got_int || settings.user_abort)
			जा print_fail;

		    प्र_लिखो((अक्षर *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_मोजणी + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (पूर्णांक)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (पूर्णांक)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    जर (!mch_print_begin_page(IObuff))
			जा print_fail;

		    जर (settings.n_collated_copies > 1)
			प्र_लिखो((अक्षर *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    जर (prt_header_height() > 0)
			prt_header(&settings, prt_page_मोजणी + 1 + side,
							prtpos.file_ओळ);

		    साठी (page_ओळ = 0; page_ओळ < settings.ओळी_per_page;
								  ++page_ओळ)
		    {
			prtpos.column = hardcopy_ओळ(&settings,
							   page_ओळ, &prtpos);
			जर (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_ओळ)));
			    जर (++prtpos.file_ओळ > eap->line2)
				थांबा; // reached the end
			}
			अन्यथा जर (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    थांबा;
			}
		    }

		    जर (!mch_print_end_page())
			जा print_fail;
		    जर (prtpos.file_ओळ > eap->line2)
			थांबा; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		जर (prtpos.file_ओळ > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    जर (!mch_print_blank_page())
			जा print_fail;
		}
	    }
	    जर (settings.duplex && prtpos.file_ओळ <= eap->line2)
		++prt_page_मोजणी;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((अक्षर *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    जर (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_संदेश
#endif
		))
    {
	प्र_लिखो((अक्षर *)IObuff, "%s", _("Printing aborted"));
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
    स्थिर colnr_T
hardcopy_ओळ(
    prt_settings_T	*psettings,
    पूर्णांक			page_ओळ,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*ओळ;
    पूर्णांक		need_break = FALSE;
    पूर्णांक		outputlen;
    पूर्णांक		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    पूर्णांक		id;
#endif

    जर (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	जर (!ppos->ff && prt_use_संख्या())
	    prt_ओळ_संख्या(psettings, page_ओळ, ppos->file_ओळ);
	ppos->ff = FALSE;
    }
    अन्यथा
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_ओळ(0, page_ओळ);
    ओळ = ml_get(ppos->file_ओळ);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    साठी (col = ppos->column; ओळ[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	जर (has_mbyte && (outputlen = (*mb_ptr2len)(ओळ + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	जर (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_ओळ, col, 1, रिक्त, FALSE);
	    जर (id > 0)
		id = syn_get_final_id(id);
	    अन्यथा
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    ओळ = ml_get(ppos->file_ओळ);

	    जर (id != current_syn_id)
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
	जर (ओळ[col] == TAB || tab_spaces != 0)
	{
	    जर (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		tab_spaces = (पूर्णांक)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    पर्यंत (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		जर (need_break)
		    थांबा;
	    }
	    // Keep the TAB if we didn't finish it.
	    जर (need_break && tab_spaces > 0)
		थांबा;
	}
	अन्यथा जर (ओळ[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	अन्यथा
	{
	    need_break = mch_print_text_out(ओळ + col, outputlen);
	    जर (has_mbyte)
		print_pos += (*mb_ptr2cells)(ओळ + col);
	    अन्यथा
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (पूर्णांक)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    जर (!ppos->ff
	    && (ओळ[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	परत 0;
    परत col;
}

#endif //FEAT_PRINTER
