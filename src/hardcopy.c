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

#यदि defined(FEAT_PRINTER)

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
स्थिर ध्रुव long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

स्थिर ध्रुव long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

स्थिर पूर्णाङ्क		current_syn_id;
#endif

पूर्णाङ्क	prt_curr_italic;
पूर्णाङ्क	prt_curr_bold;
पूर्णाङ्क	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
पूर्णाङ्क	prt_page_गिनती;

/*
 * These values determine the print position on a page.
 */
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		lead_spaces;	    // remaining spaces for a TAB
    पूर्णाङ्क		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_पंक्ति;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    पूर्णाङ्क		ff;		    // seen form feed character
} prt_pos_T;

#यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

स्थिर वर्ण *parse_list_options(char_u *option_str, option_table_T *table, पूर्णाङ्क table_size);

स्थिर colnr_T hardcopy_पंक्ति(prt_settings_T *psettings, पूर्णाङ्क page_पंक्ति, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    वर्ण *
parse_printoptions(optset_T *args UNUSED)
{
    प्रत्यागम parse_list_options(p_popt, printer_opts, OPT_PRINT_अंक_OPTIONS);
}

#यदि defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    वर्ण *
parse_printmbfont(optset_T *args UNUSED)
{
    प्रत्यागम parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_अंक_OPTIONS);
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
    स्थिर वर्ण *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    पूर्णाङ्क			table_size)
{
    option_table_T *old_opts;
    वर्ण	*ret = रिक्त;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    पूर्णाङ्क		idx = 0;		// init for GCC
    पूर्णाङ्क		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    यदि (old_opts == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    यावत् (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	यदि (colonp == रिक्त)
	{
	    ret = e_missing_colon_3;
	    भङ्ग;
	}
	commap = vim_strchr(stringp, ',');
	यदि (commap == रिक्त)
	    commap = option_str + STRLEN(option_str);

	len = (पूर्णाङ्क)(colonp - stringp);

	प्रति (idx = 0; idx < table_size; ++idx)
	    यदि (STRNICMP(stringp, table[idx].name, len) == 0)
		भङ्ग;

	यदि (idx == table_size)
	{
	    ret = e_illegal_component;
	    भङ्ग;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	यदि (table[idx].hasnum)
	{
	    यदि (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		भङ्ग;
	    }

	    table[idx].संख्या = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].म_माप = (पूर्णाङ्क)(commap - p);

	stringp = commap;
	यदि (*stringp == ',')
	    ++stringp;
    }

    यदि (ret != रिक्त)
    {
	// Restore old options in case of error
	प्रति (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    प्रत्यागम ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    स्थिर long_u
darken_rgb(long_u rgb)
{
    प्रत्यागम	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    स्थिर long_u
prt_get_term_color(पूर्णाङ्क colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    यदि (t_colors > 8)
	प्रत्यागम cterm_color_16[colorindex % 16];
    प्रत्यागम cterm_color_8[colorindex % 8];
}

    स्थिर शून्य
prt_get_attr(
    पूर्णाङ्क			hl_id,
    prt_text_attr_T	*pattr,
    पूर्णाङ्क			modec)
{
    पूर्णाङ्क     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    वर्ण    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != रिक्त);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != रिक्त);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != रिक्त);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != रिक्त);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# यदि defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    यदि (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	यदि (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    अन्यथा
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (वर्ण *)highlight_color(hl_id, (char_u *)"fg", modec);
	यदि (color == रिक्त)
	    colorindex = 0;
	अन्यथा
	    colorindex = म_से_प(color);

	यदि (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	अन्यथा
	    fg_color = PRCOLOR_BLACK;
    }

    यदि (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    अन्यथा यदि (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    स्थिर शून्य
prt_set_fg(long_u fg)
{
    यदि (fg == prt_curr_fg)
	प्रत्यागम;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    स्थिर शून्य
prt_set_bg(long_u bg)
{
    यदि (bg == prt_curr_bg)
	प्रत्यागम;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    स्थिर शून्य
prt_set_font(पूर्णाङ्क bold, पूर्णाङ्क italic, पूर्णाङ्क underline)
{
    यदि (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	प्रत्यागम;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    स्थिर शून्य
prt_पंक्ति_संख्या(
    prt_settings_T *psettings,
    पूर्णाङ्क		page_पंक्ति,
    linenr_T	lnum)
{
    पूर्णाङ्क		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->संख्या.fg_color);
    prt_set_bg(psettings->संख्या.bg_color);
    prt_set_font(psettings->संख्या.bold, psettings->संख्या.italic, psettings->संख्या.underline);
    mch_print_start_पंक्ति(TRUE, page_पंक्ति);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    प्र_लिखो((वर्ण *)tbuf, "%6ld", (दीर्घ)lnum);
    प्रति (i = 0; i < 6; i++)
	(शून्य)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    यदि (psettings->do_syntax)
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
    पूर्णाङ्क
prt_header_height(शून्य)
{
    यदि (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	प्रत्यागम printer_opts[OPT_PRINT_HEADERHEIGHT].संख्या;
    प्रत्यागम 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    पूर्णाङ्क
prt_use_संख्या(शून्य)
{
    प्रत्यागम (printer_opts[OPT_PRINT_संख्या].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_संख्या].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    पूर्णाङ्क
prt_get_unit(पूर्णाङ्क idx)
{
    पूर्णाङ्क		u = PRT_UNIT_NONE;
    पूर्णाङ्क		i;
    स्थिर वर्ण *(units[4]) = PRT_UNIT_NAMES;

    यदि (!printer_opts[idx].present)
	प्रत्यागम PRT_UNIT_NONE;

    प्रति (i = 0; i < 4; ++i)
	यदि (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    भङ्ग;
	}
    प्रत्यागम u;
}

/*
 * Print the page header.
 */
    स्थिर शून्य
prt_header(
    prt_settings_T  *psettings,
    पूर्णाङ्क		pagenum,
    linenr_T	lnum UNUSED)
{
    पूर्णाङ्क		width = psettings->chars_per_पंक्ति;
    पूर्णाङ्क		page_पंक्ति;
    char_u	*tbuf;
    char_u	*p;
    पूर्णाङ्क		l;

    // Also use the space for the line number.
    यदि (prt_use_संख्या())
	width += PRINT_संख्या_WIDTH;

    tbuf = alloc(width + IOSIZE);
    यदि (tbuf == रिक्त)
	प्रत्यागम;

#ifdef FEAT_STL_OPT
    यदि (*p_header != NUL)
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
	प्र_लिखो((वर्ण *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_पंक्ति = 0 - prt_header_height();
    mch_print_start_पंक्ति(TRUE, page_पंक्ति);
    प्रति (p = tbuf; *p != NUL; )
    {
	यदि (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_पंक्ति;
	    यदि (page_पंक्ति >= 0) // out of room in header
		भङ्ग;
	    mch_print_start_पंक्ति(TRUE, page_पंक्ति);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    यदि (psettings->do_syntax)
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
    screen_fill((पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Rows, 0, (पूर्णाङ्क)Columns, ' ', ' ', 0);
    screen_puts(s, (पूर्णाङ्क)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    शून्य
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    पूर्णाङ्क			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    पूर्णाङ्क			page_पंक्ति;
    पूर्णाङ्क			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_संदेश = TRUE;
#endif

#यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    यदि (*eap->arg == '>')
    {
	वर्ण	*errormsg = रिक्त;

	// Expand things like "%.ps".
	यदि (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    यदि (errormsg != रिक्त)
		emsg(errormsg);
	    प्रत्यागम;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    अन्यथा यदि (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    यदि (mch_print_init(&settings,
			curbuf->b_fname == रिक्त
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == रिक्त
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	प्रत्यागम;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    यदि (gui.in_use)
	settings.modec = 'g';
    अन्यथा
# endif
	यदि (t_colors > 1)
	    settings.modec = 'c';
	अन्यथा
	    settings.modec = 't';

    यदि (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    अन्यथा यदि (printer_opts[OPT_PRINT_SYNTAX].present
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
    यदि (prt_use_संख्या() && settings.do_syntax)
    {
	पूर्णाङ्क		id;

	id = syn_name2id((char_u *)"LineNr");
	यदि (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.संख्या, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    प्रति (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    यदि (bytes_to_print == 0)
    {
	संदेश(_("No text to be printed"));
	गच्छ print_fail_no_begin;
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

    यदि (!mch_print_begin(&settings))
	गच्छ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_गिनती = 0;
    प्रति (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	पूर्णाङ्क		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_पंक्ति = eap->line1;
	prtpos = page_prtpos;

	यदि (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    यदि (!mch_print_begin(&settings))
		गच्छ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	प्रति (prt_page_गिनती = 0; prtpos.file_पंक्ति <= eap->line2; ++prt_page_गिनती)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    प्रति (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		प्रति (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    यदि (got_int || settings.user_abort)
			गच्छ print_fail;

		    प्र_लिखो((वर्ण *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_गिनती + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (पूर्णाङ्क)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (पूर्णाङ्क)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    यदि (!mch_print_begin_page(IObuff))
			गच्छ print_fail;

		    यदि (settings.n_collated_copies > 1)
			प्र_लिखो((वर्ण *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    यदि (prt_header_height() > 0)
			prt_header(&settings, prt_page_गिनती + 1 + side,
							prtpos.file_पंक्ति);

		    प्रति (page_पंक्ति = 0; page_पंक्ति < settings.पंक्तियाँ_per_page;
								  ++page_पंक्ति)
		    {
			prtpos.column = hardcopy_पंक्ति(&settings,
							   page_पंक्ति, &prtpos);
			यदि (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_पंक्ति)));
			    यदि (++prtpos.file_पंक्ति > eap->line2)
				भङ्ग; // reached the end
			}
			अन्यथा यदि (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    भङ्ग;
			}
		    }

		    यदि (!mch_print_end_page())
			गच्छ print_fail;
		    यदि (prtpos.file_पंक्ति > eap->line2)
			भङ्ग; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		यदि (prtpos.file_पंक्ति > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    यदि (!mch_print_blank_page())
			गच्छ print_fail;
		}
	    }
	    यदि (settings.duplex && prtpos.file_पंक्ति <= eap->line2)
		++prt_page_गिनती;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((वर्ण *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    यदि (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_संदेश
#endif
		))
    {
	प्र_लिखो((वर्ण *)IObuff, "%s", _("Printing aborted"));
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
hardcopy_पंक्ति(
    prt_settings_T	*psettings,
    पूर्णाङ्क			page_पंक्ति,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*पंक्ति;
    पूर्णाङ्क		need_break = FALSE;
    पूर्णाङ्क		outputlen;
    पूर्णाङ्क		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    पूर्णाङ्क		id;
#endif

    यदि (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	यदि (!ppos->ff && prt_use_संख्या())
	    prt_पंक्ति_संख्या(psettings, page_पंक्ति, ppos->file_पंक्ति);
	ppos->ff = FALSE;
    }
    अन्यथा
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_पंक्ति(0, page_पंक्ति);
    पंक्ति = ml_get(ppos->file_पंक्ति);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    प्रति (col = ppos->column; पंक्ति[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	यदि (has_mbyte && (outputlen = (*mb_ptr2len)(पंक्ति + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	यदि (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_पंक्ति, col, 1, रिक्त, FALSE);
	    यदि (id > 0)
		id = syn_get_final_id(id);
	    अन्यथा
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    पंक्ति = ml_get(ppos->file_पंक्ति);

	    यदि (id != current_syn_id)
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
	यदि (पंक्ति[col] == TAB || tab_spaces != 0)
	{
	    यदि (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		tab_spaces = (पूर्णाङ्क)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    यावत् (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		यदि (need_break)
		    भङ्ग;
	    }
	    // Keep the TAB if we didn't finish it.
	    यदि (need_break && tab_spaces > 0)
		भङ्ग;
	}
	अन्यथा यदि (पंक्ति[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	अन्यथा
	{
	    need_break = mch_print_text_out(पंक्ति + col, outputlen);
	    यदि (has_mbyte)
		print_pos += (*mb_ptr2cells)(पंक्ति + col);
	    अन्यथा
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (पूर्णाङ्क)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    यदि (!ppos->ff
	    && (पंक्ति[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	प्रत्यागम 0;
    प्रत्यागम col;
}

#endif //FEAT_PRINTER
