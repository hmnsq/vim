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

#ضمن "vim.h"
#ضمن "version.h"

#إذا defined(FEAT_PRINTER)

#ضمن "hardcopy.h"
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
ساكن ثابت long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ساكن ثابت long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ساكن صحيح		current_syn_id;
#endif

صحيح	prt_curr_italic;
صحيح	prt_curr_bold;
صحيح	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
صحيح	prt_page_کيراءن;

/*
 * These values determine the print position on a page.
 */
تعريفنوع بنية
{
    صحيح		lead_spaces;	    // remaining spaces for a TAB
    صحيح		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_باريس;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    صحيح		ff;		    // seen form feed character
} prt_pos_T;

#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

ساكن حرف *parse_list_options(char_u *option_str, option_table_T *table, صحيح table_size);

ساكن colnr_T hardcopy_باريس(prt_settings_T *psettings, صحيح page_باريس, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printoptions(optset_T *args UNUSED)
{
    أرجع parse_list_options(p_popt, printer_opts, OPT_PRINT_اڠک_OPTIONS);
}

#إذا defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printmbfont(optset_T *args UNUSED)
{
    أرجع parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_اڠک_OPTIONS);
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
    ساكن حرف *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    صحيح			table_size)
{
    option_table_T *old_opts;
    حرف	*ret = عدم;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    صحيح		idx = 0;		// init for GCC
    صحيح		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    إذا (old_opts == عدم)
	أرجع عدم;

    لكل (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    طالما (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	إذا (colonp == عدم)
	{
	    ret = e_missing_colon_3;
	    اكسر;
	}
	commap = vim_strchr(stringp, ',');
	إذا (commap == عدم)
	    commap = option_str + STRLEN(option_str);

	len = (صحيح)(colonp - stringp);

	لكل (idx = 0; idx < table_size; ++idx)
	    إذا (STRNICMP(stringp, table[idx].name, len) == 0)
		اكسر;

	إذا (idx == table_size)
	{
	    ret = e_illegal_component;
	    اكسر;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	إذا (table[idx].hasnum)
	{
	    إذا (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		اكسر;
	    }

	    table[idx].نومبور = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (صحيح)(commap - p);

	stringp = commap;
	إذا (*stringp == ',')
	    ++stringp;
    }

    إذا (ret != عدم)
    {
	// Restore old options in case of error
	لكل (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    أرجع ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ساكن long_u
darken_rgb(long_u rgb)
{
    أرجع	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ساكن long_u
prt_get_term_color(صحيح colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    إذا (t_colors > 8)
	أرجع cterm_color_16[colorindex % 16];
    أرجع cterm_color_8[colorindex % 8];
}

    ساكن فراغ
prt_get_attr(
    صحيح			hl_id,
    prt_text_attr_T	*pattr,
    صحيح			modec)
{
    صحيح     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    حرف    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != عدم);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != عدم);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != عدم);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != عدم);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	إذا (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    وإلا
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (حرف *)highlight_color(hl_id, (char_u *)"fg", modec);
	إذا (color == عدم)
	    colorindex = 0;
	وإلا
	    colorindex = atoi(color);

	إذا (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	وإلا
	    fg_color = PRCOLOR_BLACK;
    }

    إذا (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    وإلا إذا (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    ساكن فراغ
prt_set_fg(long_u fg)
{
    إذا (fg == prt_curr_fg)
	أرجع;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    ساكن فراغ
prt_set_bg(long_u bg)
{
    إذا (bg == prt_curr_bg)
	أرجع;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    ساكن فراغ
prt_set_font(صحيح bold, صحيح italic, صحيح underline)
{
    إذا (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	أرجع;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ساكن فراغ
prt_باريس_نومبور(
    prt_settings_T *psettings,
    صحيح		page_باريس,
    linenr_T	lnum)
{
    صحيح		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->نومبور.fg_color);
    prt_set_bg(psettings->نومبور.bg_color);
    prt_set_font(psettings->نومبور.bold, psettings->نومبور.italic, psettings->نومبور.underline);
    mch_print_start_باريس(TRUE, page_باريس);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((حرف *)tbuf, "%6ld", (long)lnum);
    لكل (i = 0; i < 6; i++)
	(فراغ)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    إذا (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وإلا
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
    صحيح
prt_header_height(فراغ)
{
    إذا (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	أرجع printer_opts[OPT_PRINT_HEADERHEIGHT].نومبور;
    أرجع 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    صحيح
prt_use_نومبور(فراغ)
{
    أرجع (printer_opts[OPT_PRINT_نومبور].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_نومبور].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    صحيح
prt_get_unit(صحيح idx)
{
    صحيح		u = PRT_UNIT_NONE;
    صحيح		i;
    ساكن حرف *(units[4]) = PRT_UNIT_NAMES;

    إذا (!printer_opts[idx].present)
	أرجع PRT_UNIT_NONE;

    لكل (i = 0; i < 4; ++i)
	إذا (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    اكسر;
	}
    أرجع u;
}

/*
 * Print the page header.
 */
    ساكن فراغ
prt_header(
    prt_settings_T  *psettings,
    صحيح		pagenum,
    linenr_T	lnum UNUSED)
{
    صحيح		width = psettings->chars_per_باريس;
    صحيح		page_باريس;
    char_u	*tbuf;
    char_u	*p;
    صحيح		l;

    // Also use the space for the line number.
    إذا (prt_use_نومبور())
	width += PRINT_نومبور_WIDTH;

    tbuf = alloc(width + IOSIZE);
    إذا (tbuf == عدم)
	أرجع;

#ifdef FEAT_STL_OPT
    إذا (*p_header != NUL)
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
	printer_page_اڠک = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, عدم, عدم,
			 عدم);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    وإلا
#endif
	sprintf((حرف *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_باريس = 0 - prt_header_height();
    mch_print_start_باريس(TRUE, page_باريس);
    لكل (p = tbuf; *p != NUL; )
    {
	إذا (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_باريس;
	    إذا (page_باريس >= 0) // out of room in header
		اكسر;
	    mch_print_start_باريس(TRUE, page_باريس);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    إذا (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وإلا
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
    فراغ
prt_message(char_u *s)
{
    screen_fill((صحيح)Rows - 1, (صحيح)Rows, 0, (صحيح)Columns, ' ', ' ', 0);
    screen_puts(s, (صحيح)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    فراغ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    صحيح			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    صحيح			page_باريس;
    صحيح			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_ڤسنن = TRUE;
#endif

#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    إذا (*eap->arg == '>')
    {
	حرف	*errormsg = عدم;

	// Expand things like "%.ps".
	إذا (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    إذا (errormsg != عدم)
		emsg(errormsg);
	    أرجع;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    وإلا إذا (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    إذا (mch_print_init(&settings,
			curbuf->b_fname == عدم
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == عدم
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	أرجع;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    إذا (gui.in_use)
	settings.modec = 'g';
    وإلا
# endif
	إذا (t_colors > 1)
	    settings.modec = 'c';
	وإلا
	    settings.modec = 't';

    إذا (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    وإلا إذا (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    وإلا
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.نومبور.fg_color = PRCOLOR_BLACK;
    settings.نومبور.bg_color = PRCOLOR_WHITE;
    settings.نومبور.bold = FALSE;
    settings.نومبور.italic = TRUE;
    settings.نومبور.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    إذا (prt_use_نومبور() && settings.do_syntax)
    {
	صحيح		id;

	id = syn_name2id((char_u *)"LineNr");
	إذا (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.نومبور, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    لكل (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    إذا (bytes_to_print == 0)
    {
	ڤسنن(_("No text to be printed"));
	اذهب print_fail_no_begin;
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

    إذا (!mch_print_begin(&settings))
	اذهب print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_کيراءن = 0;
    لكل (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	صحيح		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_باريس = eap->line1;
	prtpos = page_prtpos;

	إذا (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    إذا (!mch_print_begin(&settings))
		اذهب print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	لكل (prt_page_کيراءن = 0; prtpos.file_باريس <= eap->line2; ++prt_page_کيراءن)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    لكل (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		لكل (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    إذا (got_int || settings.user_abort)
			اذهب print_fail;

		    sprintf((حرف *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_کيراءن + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (صحيح)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (صحيح)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    إذا (!mch_print_begin_page(IObuff))
			اذهب print_fail;

		    إذا (settings.n_collated_copies > 1)
			sprintf((حرف *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    إذا (prt_header_height() > 0)
			prt_header(&settings, prt_page_کيراءن + 1 + side,
							prtpos.file_باريس);

		    لكل (page_باريس = 0; page_باريس < settings.باريس_سموا_per_page;
								  ++page_باريس)
		    {
			prtpos.column = hardcopy_باريس(&settings,
							   page_باريس, &prtpos);
			إذا (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_باريس)));
			    إذا (++prtpos.file_باريس > eap->line2)
				اكسر; // reached the end
			}
			وإلا إذا (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    اكسر;
			}
		    }

		    إذا (!mch_print_end_page())
			اذهب print_fail;
		    إذا (prtpos.file_باريس > eap->line2)
			اكسر; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		إذا (prtpos.file_باريس > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    إذا (!mch_print_blank_page())
			اذهب print_fail;
		}
	    }
	    إذا (settings.duplex && prtpos.file_باريس <= eap->line2)
		++prt_page_کيراءن;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((حرف *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    إذا (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_ڤسنن
#endif
		))
    {
	sprintf((حرف *)IObuff, "%s", _("Printing aborted"));
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
    ساكن colnr_T
hardcopy_باريس(
    prt_settings_T	*psettings,
    صحيح			page_باريس,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*باريس;
    صحيح		need_break = FALSE;
    صحيح		outputlen;
    صحيح		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    صحيح		id;
#endif

    إذا (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	إذا (!ppos->ff && prt_use_نومبور())
	    prt_باريس_نومبور(psettings, page_باريس, ppos->file_باريس);
	ppos->ff = FALSE;
    }
    وإلا
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_باريس(0, page_باريس);
    باريس = ml_get(ppos->file_باريس);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    لكل (col = ppos->column; باريس[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	إذا (has_mbyte && (outputlen = (*mb_ptr2len)(باريس + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	إذا (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_باريس, col, 1, عدم, FALSE);
	    إذا (id > 0)
		id = syn_get_final_id(id);
	    وإلا
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    باريس = ml_get(ppos->file_باريس);

	    إذا (id != current_syn_id)
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
	إذا (باريس[col] == TAB || tab_spaces != 0)
	{
	    إذا (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
		tab_spaces = (صحيح)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    طالما (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		إذا (need_break)
		    اكسر;
	    }
	    // Keep the TAB if we didn't finish it.
	    إذا (need_break && tab_spaces > 0)
		اكسر;
	}
	وإلا إذا (باريس[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	وإلا
	{
	    need_break = mch_print_text_out(باريس + col, outputlen);
	    إذا (has_mbyte)
		print_pos += (*mb_ptr2cells)(باريس + col);
	    وإلا
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (صحيح)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    إذا (!ppos->ff
	    && (باريس[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	أرجع 0;
    أرجع col;
}

#endif //FEAT_PRINTER
