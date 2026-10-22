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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#જો defined(FEAT_PRINTER)

#અન્તર્ભાવ "hardcopy.h"
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
સ્થિર અચલ long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

સ્થિર અચલ long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

સ્થિર પૂર્ણાંક		current_syn_id;
#endif

પૂર્ણાંક	prt_curr_italic;
પૂર્ણાંક	prt_curr_bold;
પૂર્ણાંક	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
પૂર્ણાંક	prt_page_ગણતરી;

/*
 * These values determine the print position on a page.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		lead_spaces;	    // remaining spaces for a TAB
    પૂર્ણાંક		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_લીટી;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    પૂર્ણાંક		ff;		    // seen form feed character
} prt_pos_T;

#જો defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

સ્થિર અક્ષર *parse_list_options(char_u *option_str, option_table_T *table, પૂર્ણાંક table_size);

સ્થિર colnr_T hardcopy_લીટી(prt_settings_T *psettings, પૂર્ણાંક page_લીટી, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    અક્ષર *
parse_printoptions(optset_T *args UNUSED)
{
    પાછા parse_list_options(p_popt, printer_opts, OPT_PRINT_અંક_OPTIONS);
}

#જો defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    અક્ષર *
parse_printmbfont(optset_T *args UNUSED)
{
    પાછા parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_અંક_OPTIONS);
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
    સ્થિર અક્ષર *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    પૂર્ણાંક			table_size)
{
    option_table_T *old_opts;
    અક્ષર	*ret = ખાલી;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    પૂર્ણાંક		idx = 0;		// init for GCC
    પૂર્ણાંક		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    જો (old_opts == ખાલી)
	પાછા ખાલી;

    માટે (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    જ્યારે (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	જો (colonp == ખાલી)
	{
	    ret = e_missing_colon_3;
	    તોડો;
	}
	commap = vim_strchr(stringp, ',');
	જો (commap == ખાલી)
	    commap = option_str + STRLEN(option_str);

	len = (પૂર્ણાંક)(colonp - stringp);

	માટે (idx = 0; idx < table_size; ++idx)
	    જો (STRNICMP(stringp, table[idx].name, len) == 0)
		તોડો;

	જો (idx == table_size)
	{
	    ret = e_illegal_component;
	    તોડો;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	જો (table[idx].hasnum)
	{
	    જો (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		તોડો;
	    }

	    table[idx].સંખ્યા = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (પૂર્ણાંક)(commap - p);

	stringp = commap;
	જો (*stringp == ',')
	    ++stringp;
    }

    જો (ret != ખાલી)
    {
	// Restore old options in case of error
	માટે (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    પાછા ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    સ્થિર long_u
darken_rgb(long_u rgb)
{
    પાછા	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    સ્થિર long_u
prt_get_term_color(પૂર્ણાંક colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    જો (t_colors > 8)
	પાછા cterm_color_16[colorindex % 16];
    પાછા cterm_color_8[colorindex % 8];
}

    સ્થિર શૂન્ય
prt_get_attr(
    પૂર્ણાંક			hl_id,
    prt_text_attr_T	*pattr,
    પૂર્ણાંક			modec)
{
    પૂર્ણાંક     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    અક્ષર    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != ખાલી);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != ખાલી);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != ખાલી);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != ખાલી);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	જો (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    નહીંતર
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (અક્ષર *)highlight_color(hl_id, (char_u *)"fg", modec);
	જો (color == ખાલી)
	    colorindex = 0;
	નહીંતર
	    colorindex = atoi(color);

	જો (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	નહીંતર
	    fg_color = PRCOLOR_BLACK;
    }

    જો (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    નહીંતર જો (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    સ્થિર શૂન્ય
prt_set_fg(long_u fg)
{
    જો (fg == prt_curr_fg)
	પાછા;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    સ્થિર શૂન્ય
prt_set_bg(long_u bg)
{
    જો (bg == prt_curr_bg)
	પાછા;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    સ્થિર શૂન્ય
prt_set_font(પૂર્ણાંક bold, પૂર્ણાંક italic, પૂર્ણાંક underline)
{
    જો (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	પાછા;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    સ્થિર શૂન્ય
prt_લીટી_સંખ્યા(
    prt_settings_T *psettings,
    પૂર્ણાંક		page_લીટી,
    linenr_T	lnum)
{
    પૂર્ણાંક		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->સંખ્યા.fg_color);
    prt_set_bg(psettings->સંખ્યા.bg_color);
    prt_set_font(psettings->સંખ્યા.bold, psettings->સંખ્યા.italic, psettings->સંખ્યા.underline);
    mch_print_start_લીટી(TRUE, page_લીટી);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((અક્ષર *)tbuf, "%6ld", (long)lnum);
    માટે (i = 0; i < 6; i++)
	(શૂન્ય)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    જો (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    નહીંતર
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
    પૂર્ણાંક
prt_header_height(શૂન્ય)
{
    જો (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	પાછા printer_opts[OPT_PRINT_HEADERHEIGHT].સંખ્યા;
    પાછા 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    પૂર્ણાંક
prt_use_સંખ્યા(શૂન્ય)
{
    પાછા (printer_opts[OPT_PRINT_સંખ્યા].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_સંખ્યા].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    પૂર્ણાંક
prt_get_unit(પૂર્ણાંક idx)
{
    પૂર્ણાંક		u = PRT_UNIT_NONE;
    પૂર્ણાંક		i;
    સ્થિર અક્ષર *(units[4]) = PRT_UNIT_NAMES;

    જો (!printer_opts[idx].present)
	પાછા PRT_UNIT_NONE;

    માટે (i = 0; i < 4; ++i)
	જો (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    તોડો;
	}
    પાછા u;
}

/*
 * Print the page header.
 */
    સ્થિર શૂન્ય
prt_header(
    prt_settings_T  *psettings,
    પૂર્ણાંક		pagenum,
    linenr_T	lnum UNUSED)
{
    પૂર્ણાંક		width = psettings->chars_per_લીટી;
    પૂર્ણાંક		page_લીટી;
    char_u	*tbuf;
    char_u	*p;
    પૂર્ણાંક		l;

    // Also use the space for the line number.
    જો (prt_use_સંખ્યા())
	width += PRINT_સંખ્યા_WIDTH;

    tbuf = alloc(width + IOSIZE);
    જો (tbuf == ખાલી)
	પાછા;

#ifdef FEAT_STL_OPT
    જો (*p_header != NUL)
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
	printer_page_અંક = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, ખાલી, ખાલી,
			 ખાલી);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    નહીંતર
#endif
	sprintf((અક્ષર *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_લીટી = 0 - prt_header_height();
    mch_print_start_લીટી(TRUE, page_લીટી);
    માટે (p = tbuf; *p != NUL; )
    {
	જો (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_લીટી;
	    જો (page_લીટી >= 0) // out of room in header
		તોડો;
	    mch_print_start_લીટી(TRUE, page_લીટી);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    જો (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    નહીંતર
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
    શૂન્ય
prt_message(char_u *s)
{
    screen_fill((પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Rows, 0, (પૂર્ણાંક)Columns, ' ', ' ', 0);
    screen_puts(s, (પૂર્ણાંક)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    શૂન્ય
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    પૂર્ણાંક			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    પૂર્ણાંક			page_લીટી;
    પૂર્ણાંક			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_સંદેશ = TRUE;
#endif

#જો defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    જો (*eap->arg == '>')
    {
	અક્ષર	*errormsg = ખાલી;

	// Expand things like "%.ps".
	જો (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    જો (errormsg != ખાલી)
		emsg(errormsg);
	    પાછા;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    નહીંતર જો (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    જો (mch_print_init(&settings,
			curbuf->b_fname == ખાલી
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == ખાલી
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	પાછા;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    જો (gui.in_use)
	settings.modec = 'g';
    નહીંતર
# endif
	જો (t_colors > 1)
	    settings.modec = 'c';
	નહીંતર
	    settings.modec = 't';

    જો (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    નહીંતર જો (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    નહીંતર
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.સંખ્યા.fg_color = PRCOLOR_BLACK;
    settings.સંખ્યા.bg_color = PRCOLOR_WHITE;
    settings.સંખ્યા.bold = FALSE;
    settings.સંખ્યા.italic = TRUE;
    settings.સંખ્યા.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    જો (prt_use_સંખ્યા() && settings.do_syntax)
    {
	પૂર્ણાંક		id;

	id = syn_name2id((char_u *)"LineNr");
	જો (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.સંખ્યા, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    માટે (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    જો (bytes_to_print == 0)
    {
	સંદેશ(_("No text to be printed"));
	જાઓ print_fail_no_begin;
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

    જો (!mch_print_begin(&settings))
	જાઓ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_ગણતરી = 0;
    માટે (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	પૂર્ણાંક		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_લીટી = eap->line1;
	prtpos = page_prtpos;

	જો (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    જો (!mch_print_begin(&settings))
		જાઓ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	માટે (prt_page_ગણતરી = 0; prtpos.file_લીટી <= eap->line2; ++prt_page_ગણતરી)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    માટે (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		માટે (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    જો (got_int || settings.user_abort)
			જાઓ print_fail;

		    sprintf((અક્ષર *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_ગણતરી + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (પૂર્ણાંક)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (પૂર્ણાંક)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    જો (!mch_print_begin_page(IObuff))
			જાઓ print_fail;

		    જો (settings.n_collated_copies > 1)
			sprintf((અક્ષર *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    જો (prt_header_height() > 0)
			prt_header(&settings, prt_page_ગણતરી + 1 + side,
							prtpos.file_લીટી);

		    માટે (page_લીટી = 0; page_લીટી < settings.લીટીઓ_per_page;
								  ++page_લીટી)
		    {
			prtpos.column = hardcopy_લીટી(&settings,
							   page_લીટી, &prtpos);
			જો (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_લીટી)));
			    જો (++prtpos.file_લીટી > eap->line2)
				તોડો; // reached the end
			}
			નહીંતર જો (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    તોડો;
			}
		    }

		    જો (!mch_print_end_page())
			જાઓ print_fail;
		    જો (prtpos.file_લીટી > eap->line2)
			તોડો; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		જો (prtpos.file_લીટી > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    જો (!mch_print_blank_page())
			જાઓ print_fail;
		}
	    }
	    જો (settings.duplex && prtpos.file_લીટી <= eap->line2)
		++prt_page_ગણતરી;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    જો (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_સંદેશ
#endif
		))
    {
	sprintf((અક્ષર *)IObuff, "%s", _("Printing aborted"));
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
    સ્થિર colnr_T
hardcopy_લીટી(
    prt_settings_T	*psettings,
    પૂર્ણાંક			page_લીટી,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*લીટી;
    પૂર્ણાંક		need_break = FALSE;
    પૂર્ણાંક		outputlen;
    પૂર્ણાંક		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    પૂર્ણાંક		id;
#endif

    જો (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	જો (!ppos->ff && prt_use_સંખ્યા())
	    prt_લીટી_સંખ્યા(psettings, page_લીટી, ppos->file_લીટી);
	ppos->ff = FALSE;
    }
    નહીંતર
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_લીટી(0, page_લીટી);
    લીટી = ml_get(ppos->file_લીટી);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    માટે (col = ppos->column; લીટી[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	જો (has_mbyte && (outputlen = (*mb_ptr2len)(લીટી + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	જો (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_લીટી, col, 1, ખાલી, FALSE);
	    જો (id > 0)
		id = syn_get_final_id(id);
	    નહીંતર
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    લીટી = ml_get(ppos->file_લીટી);

	    જો (id != current_syn_id)
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
	જો (લીટી[col] == TAB || tab_spaces != 0)
	{
	    જો (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#નહીંતર
		tab_spaces = (પૂર્ણાંક)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    જ્યારે (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		જો (need_break)
		    તોડો;
	    }
	    // Keep the TAB if we didn't finish it.
	    જો (need_break && tab_spaces > 0)
		તોડો;
	}
	નહીંતર જો (લીટી[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	નહીંતર
	{
	    need_break = mch_print_text_out(લીટી + col, outputlen);
	    જો (has_mbyte)
		print_pos += (*mb_ptr2cells)(લીટી + col);
	    નહીંતર
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (પૂર્ણાંક)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    જો (!ppos->ff
	    && (લીટી[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	પાછા 0;
    પાછા col;
}

#endif //FEAT_PRINTER
