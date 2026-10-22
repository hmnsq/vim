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

#شامل "vim.h"
#شامل "version.h"

#اگر defined(FEAT_PRINTER)

#شامل "hardcopy.h"
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
ایستا ثابت long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ایستا ثابت long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ایستا صحیح		current_syn_id;
#endif

صحیح	prt_curr_italic;
صحیح	prt_curr_bold;
صحیح	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
صحیح	prt_page_شمار;

/*
 * These values determine the print position on a page.
 */
تعریفنوع ساختار
{
    صحیح		lead_spaces;	    // remaining spaces for a TAB
    صحیح		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_سطر;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    صحیح		ff;		    // seen form feed character
} prt_pos_T;

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

ایستا نویسه *parse_list_options(char_u *option_str, option_table_T *table, صحیح table_size);

ایستا colnr_T hardcopy_سطر(prt_settings_T *psettings, صحیح page_سطر, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    نویسه *
parse_printoptions(optset_T *args UNUSED)
{
    بازگشت parse_list_options(p_popt, printer_opts, OPT_PRINT_رقم_OPTIONS);
}

#اگر defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    نویسه *
parse_printmbfont(optset_T *args UNUSED)
{
    بازگشت parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_رقم_OPTIONS);
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
    ایستا نویسه *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    صحیح			table_size)
{
    option_table_T *old_opts;
    نویسه	*ret = پوچ;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    صحیح		idx = 0;		// init for GCC
    صحیح		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    اگر (old_opts == پوچ)
	بازگشت پوچ;

    برای (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    وقتی (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	اگر (colonp == پوچ)
	{
	    ret = e_missing_colon_3;
	    بشکن;
	}
	commap = vim_strchr(stringp, ',');
	اگر (commap == پوچ)
	    commap = option_str + STRLEN(option_str);

	len = (صحیح)(colonp - stringp);

	برای (idx = 0; idx < table_size; ++idx)
	    اگر (STRNICMP(stringp, table[idx].name, len) == 0)
		بشکن;

	اگر (idx == table_size)
	{
	    ret = e_illegal_component;
	    بشکن;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	اگر (table[idx].hasnum)
	{
	    اگر (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		بشکن;
	    }

	    table[idx].عدد = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (صحیح)(commap - p);

	stringp = commap;
	اگر (*stringp == ',')
	    ++stringp;
    }

    اگر (ret != پوچ)
    {
	// Restore old options in case of error
	برای (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    بازگشت ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ایستا long_u
darken_rgb(long_u rgb)
{
    بازگشت	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ایستا long_u
prt_get_term_color(صحیح colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    اگر (t_colors > 8)
	بازگشت cterm_color_16[colorindex % 16];
    بازگشت cterm_color_8[colorindex % 8];
}

    ایستا تهی
prt_get_attr(
    صحیح			hl_id,
    prt_text_attr_T	*pattr,
    صحیح			modec)
{
    صحیح     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    نویسه    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != پوچ);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != پوچ);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != پوچ);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != پوچ);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	اگر (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    وگرنه
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (نویسه *)highlight_color(hl_id, (char_u *)"fg", modec);
	اگر (color == پوچ)
	    colorindex = 0;
	وگرنه
	    colorindex = atoi(color);

	اگر (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	وگرنه
	    fg_color = PRCOLOR_BLACK;
    }

    اگر (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    وگرنه اگر (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    ایستا تهی
prt_set_fg(long_u fg)
{
    اگر (fg == prt_curr_fg)
	بازگشت;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    ایستا تهی
prt_set_bg(long_u bg)
{
    اگر (bg == prt_curr_bg)
	بازگشت;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    ایستا تهی
prt_set_font(صحیح bold, صحیح italic, صحیح underline)
{
    اگر (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	بازگشت;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ایستا تهی
prt_سطر_عدد(
    prt_settings_T *psettings,
    صحیح		page_سطر,
    linenr_T	lnum)
{
    صحیح		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->عدد.fg_color);
    prt_set_bg(psettings->عدد.bg_color);
    prt_set_font(psettings->عدد.bold, psettings->عدد.italic, psettings->عدد.underline);
    mch_print_start_سطر(TRUE, page_سطر);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((نویسه *)tbuf, "%6ld", (long)lnum);
    برای (i = 0; i < 6; i++)
	(تهی)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    اگر (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وگرنه
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
    صحیح
prt_header_height(تهی)
{
    اگر (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	بازگشت printer_opts[OPT_PRINT_HEADERHEIGHT].عدد;
    بازگشت 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    صحیح
prt_use_عدد(تهی)
{
    بازگشت (printer_opts[OPT_PRINT_عدد].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_عدد].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    صحیح
prt_get_unit(صحیح idx)
{
    صحیح		u = PRT_UNIT_NONE;
    صحیح		i;
    ایستا نویسه *(units[4]) = PRT_UNIT_NAMES;

    اگر (!printer_opts[idx].present)
	بازگشت PRT_UNIT_NONE;

    برای (i = 0; i < 4; ++i)
	اگر (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    بشکن;
	}
    بازگشت u;
}

/*
 * Print the page header.
 */
    ایستا تهی
prt_header(
    prt_settings_T  *psettings,
    صحیح		pagenum,
    linenr_T	lnum UNUSED)
{
    صحیح		width = psettings->chars_per_سطر;
    صحیح		page_سطر;
    char_u	*tbuf;
    char_u	*p;
    صحیح		l;

    // Also use the space for the line number.
    اگر (prt_use_عدد())
	width += PRINT_عدد_WIDTH;

    tbuf = alloc(width + IOSIZE);
    اگر (tbuf == پوچ)
	بازگشت;

#ifdef FEAT_STL_OPT
    اگر (*p_header != NUL)
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
	printer_page_رقم = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, پوچ, پوچ,
			 پوچ);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    وگرنه
#endif
	sprintf((نویسه *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_سطر = 0 - prt_header_height();
    mch_print_start_سطر(TRUE, page_سطر);
    برای (p = tbuf; *p != NUL; )
    {
	اگر (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_سطر;
	    اگر (page_سطر >= 0) // out of room in header
		بشکن;
	    mch_print_start_سطر(TRUE, page_سطر);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    اگر (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وگرنه
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
    تهی
prt_message(char_u *s)
{
    screen_fill((صحیح)Rows - 1, (صحیح)Rows, 0, (صحیح)Columns, ' ', ' ', 0);
    screen_puts(s, (صحیح)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    تهی
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    صحیح			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    صحیح			page_سطر;
    صحیح			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_پیام = TRUE;
#endif

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    اگر (*eap->arg == '>')
    {
	نویسه	*errormsg = پوچ;

	// Expand things like "%.ps".
	اگر (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    اگر (errormsg != پوچ)
		emsg(errormsg);
	    بازگشت;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    وگرنه اگر (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    اگر (mch_print_init(&settings,
			curbuf->b_fname == پوچ
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == پوچ
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	بازگشت;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    اگر (gui.in_use)
	settings.modec = 'g';
    وگرنه
# endif
	اگر (t_colors > 1)
	    settings.modec = 'c';
	وگرنه
	    settings.modec = 't';

    اگر (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    وگرنه اگر (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    وگرنه
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.عدد.fg_color = PRCOLOR_BLACK;
    settings.عدد.bg_color = PRCOLOR_WHITE;
    settings.عدد.bold = FALSE;
    settings.عدد.italic = TRUE;
    settings.عدد.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    اگر (prt_use_عدد() && settings.do_syntax)
    {
	صحیح		id;

	id = syn_name2id((char_u *)"LineNr");
	اگر (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.عدد, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    برای (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    اگر (bytes_to_print == 0)
    {
	پیام(_("No text to be printed"));
	برو print_fail_no_begin;
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

    اگر (!mch_print_begin(&settings))
	برو print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_شمار = 0;
    برای (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	صحیح		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_سطر = eap->line1;
	prtpos = page_prtpos;

	اگر (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    اگر (!mch_print_begin(&settings))
		برو print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	برای (prt_page_شمار = 0; prtpos.file_سطر <= eap->line2; ++prt_page_شمار)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    برای (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		برای (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    اگر (got_int || settings.user_abort)
			برو print_fail;

		    sprintf((نویسه *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_شمار + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (صحیح)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (صحیح)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    اگر (!mch_print_begin_page(IObuff))
			برو print_fail;

		    اگر (settings.n_collated_copies > 1)
			sprintf((نویسه *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    اگر (prt_header_height() > 0)
			prt_header(&settings, prt_page_شمار + 1 + side,
							prtpos.file_سطر);

		    برای (page_سطر = 0; page_سطر < settings.سطرها_per_page;
								  ++page_سطر)
		    {
			prtpos.column = hardcopy_سطر(&settings,
							   page_سطر, &prtpos);
			اگر (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_سطر)));
			    اگر (++prtpos.file_سطر > eap->line2)
				بشکن; // reached the end
			}
			وگرنه اگر (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    بشکن;
			}
		    }

		    اگر (!mch_print_end_page())
			برو print_fail;
		    اگر (prtpos.file_سطر > eap->line2)
			بشکن; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		اگر (prtpos.file_سطر > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    اگر (!mch_print_blank_page())
			برو print_fail;
		}
	    }
	    اگر (settings.duplex && prtpos.file_سطر <= eap->line2)
		++prt_page_شمار;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((نویسه *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    اگر (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_پیام
#endif
		))
    {
	sprintf((نویسه *)IObuff, "%s", _("Printing aborted"));
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
    ایستا colnr_T
hardcopy_سطر(
    prt_settings_T	*psettings,
    صحیح			page_سطر,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*سطر;
    صحیح		need_break = FALSE;
    صحیح		outputlen;
    صحیح		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    صحیح		id;
#endif

    اگر (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	اگر (!ppos->ff && prt_use_عدد())
	    prt_سطر_عدد(psettings, page_سطر, ppos->file_سطر);
	ppos->ff = FALSE;
    }
    وگرنه
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_سطر(0, page_سطر);
    سطر = ml_get(ppos->file_سطر);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    برای (col = ppos->column; سطر[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	اگر (has_mbyte && (outputlen = (*mb_ptr2len)(سطر + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	اگر (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_سطر, col, 1, پوچ, FALSE);
	    اگر (id > 0)
		id = syn_get_final_id(id);
	    وگرنه
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    سطر = ml_get(ppos->file_سطر);

	    اگر (id != current_syn_id)
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
	اگر (سطر[col] == TAB || tab_spaces != 0)
	{
	    اگر (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وگرنه
		tab_spaces = (صحیح)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    وقتی (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		اگر (need_break)
		    بشکن;
	    }
	    // Keep the TAB if we didn't finish it.
	    اگر (need_break && tab_spaces > 0)
		بشکن;
	}
	وگرنه اگر (سطر[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	وگرنه
	{
	    need_break = mch_print_text_out(سطر + col, outputlen);
	    اگر (has_mbyte)
		print_pos += (*mb_ptr2cells)(سطر + col);
	    وگرنه
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (صحیح)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    اگر (!ppos->ff
	    && (سطر[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	بازگشت 0;
    بازگشت col;
}

#endif //FEAT_PRINTER
