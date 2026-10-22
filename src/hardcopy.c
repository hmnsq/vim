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

#include "vim.h"
#include "version.h"

#གལ་སྲིད defined(FEAT_PRINTER)

#include "hardcopy.h"
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
static const long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

static const long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

static ཧྲིལ		current_syn_id;
#endif

ཧྲིལ	prt_curr_italic;
ཧྲིལ	prt_curr_bold;
ཧྲིལ	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
ཧྲིལ	prt_page_count;

/*
 * These values determine the print position on a page.
 */
typedef sa_nracanaa
{
    ཧྲིལ		lead_spaces;	    // remaining spaces for a TAB
    ཧྲིལ		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_line;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    ཧྲིལ		ff;		    // seen form feed character
} prt_pos_T;

#གལ་སྲིད defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

static var_na *parse_list_options(char_u *option_str, option_table_T *table, ཧྲིལ table_size);

static colnr_T hardcopy_line(prt_settings_T *psettings, ཧྲིལ page_line, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    var_na *
parse_printoptions(optset_T *args UNUSED)
{
    ལོག parse_list_options(p_popt, printer_opts, OPT_PRINT_NUM_OPTIONS);
}

#གལ་སྲིད defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    var_na *
parse_printmbfont(optset_T *args UNUSED)
{
    ལོག parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_NUM_OPTIONS);
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
    static var_na *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    ཧྲིལ			table_size)
{
    option_table_T *old_opts;
    var_na	*ret = NULL;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    ཧྲིལ		idx = 0;		// init for GCC
    ཧྲིལ		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    གལ་སྲིད (old_opts == NULL)
	ལོག NULL;

    དོན་ལུ (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    སྐབས (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	གལ་སྲིད (colonp == NULL)
	{
	    ret = e_missing_colon_3;
	    bha_nga;
	}
	commap = vim_strchr(stringp, ',');
	གལ་སྲིད (commap == NULL)
	    commap = option_str + STRLEN(option_str);

	len = (ཧྲིལ)(colonp - stringp);

	དོན་ལུ (idx = 0; idx < table_size; ++idx)
	    གལ་སྲིད (STRNICMP(stringp, table[idx].name, len) == 0)
		bha_nga;

	གལ་སྲིད (idx == table_size)
	{
	    ret = e_illegal_component;
	    bha_nga;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	གལ་སྲིད (table[idx].hasnum)
	{
	    གལ་སྲིད (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		bha_nga;
	    }

	    table[idx].number = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (ཧྲིལ)(commap - p);

	stringp = commap;
	གལ་སྲིད (*stringp == ',')
	    ++stringp;
    }

    གལ་སྲིད (ret != NULL)
    {
	// Restore old options in case of error
	དོན་ལུ (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    ལོག ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    static long_u
darken_rgb(long_u rgb)
{
    ལོག	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    static long_u
prt_get_term_color(ཧྲིལ colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    གལ་སྲིད (t_colors > 8)
	ལོག cterm_color_16[colorindex % 16];
    ལོག cterm_color_8[colorindex % 8];
}

    static void
prt_get_attr(
    ཧྲིལ			hl_id,
    prt_text_attr_T	*pattr,
    ཧྲིལ			modec)
{
    ཧྲིལ     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    var_na    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != NULL);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != NULL);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != NULL);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != NULL);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# གལ་སྲིད defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    གལ་སྲིད (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	གལ་སྲིད (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    དེ་མིན
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (var_na *)highlight_color(hl_id, (char_u *)"fg", modec);
	གལ་སྲིད (color == NULL)
	    colorindex = 0;
	དེ་མིན
	    colorindex = atoi(color);

	གལ་སྲིད (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	དེ་མིན
	    fg_color = PRCOLOR_BLACK;
    }

    གལ་སྲིད (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    དེ་མིན གལ་སྲིད (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    static void
prt_set_fg(long_u fg)
{
    གལ་སྲིད (fg == prt_curr_fg)
	ལོག;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    static void
prt_set_bg(long_u bg)
{
    གལ་སྲིད (bg == prt_curr_bg)
	ལོག;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    static void
prt_set_font(ཧྲིལ bold, ཧྲིལ italic, ཧྲིལ underline)
{
    གལ་སྲིད (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	ལོག;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_set_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    static void
prt_line_number(
    prt_settings_T *psettings,
    ཧྲིལ		page_line,
    linenr_T	lnum)
{
    ཧྲིལ		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->number.fg_color);
    prt_set_bg(psettings->number.bg_color);
    prt_set_font(psettings->number.bold, psettings->number.italic, psettings->number.underline);
    mch_print_start_line(TRUE, page_line);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((var_na *)tbuf, "%6ld", (long)lnum);
    དོན་ལུ (i = 0; i < 6; i++)
	(void)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    གལ་སྲིད (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    དེ་མིན
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
    ཧྲིལ
prt_header_height(void)
{
    གལ་སྲིད (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	ལོག printer_opts[OPT_PRINT_HEADERHEIGHT].number;
    ལོག 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    ཧྲིལ
prt_use_number(void)
{
    ལོག (printer_opts[OPT_PRINT_NUMBER].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_NUMBER].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    ཧྲིལ
prt_get_unit(ཧྲིལ idx)
{
    ཧྲིལ		u = PRT_UNIT_NONE;
    ཧྲིལ		i;
    static var_na *(units[4]) = PRT_UNIT_NAMES;

    གལ་སྲིད (!printer_opts[idx].present)
	ལོག PRT_UNIT_NONE;

    དོན་ལུ (i = 0; i < 4; ++i)
	གལ་སྲིད (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    bha_nga;
	}
    ལོག u;
}

/*
 * Print the page header.
 */
    static void
prt_header(
    prt_settings_T  *psettings,
    ཧྲིལ		pagenum,
    linenr_T	lnum UNUSED)
{
    ཧྲིལ		width = psettings->chars_per_line;
    ཧྲིལ		page_line;
    char_u	*tbuf;
    char_u	*p;
    ཧྲིལ		l;

    // Also use the space for the line number.
    གལ་སྲིད (prt_use_number())
	width += PRINT_NUMBER_WIDTH;

    tbuf = alloc(width + IOSIZE);
    གལ་སྲིད (tbuf == NULL)
	ལོག;

#ifdef FEAT_STL_OPT
    གལ་སྲིད (*p_header != NUL)
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
	printer_page_num = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(width + IOSIZE), p_header,
			 (char_u *)"printheader", 0, ' ', width, NULL, NULL,
			 NULL);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    དེ་མིན
#endif
	sprintf((var_na *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_line = 0 - prt_header_height();
    mch_print_start_line(TRUE, page_line);
    དོན་ལུ (p = tbuf; *p != NUL; )
    {
	གལ་སྲིད (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_line;
	    གལ་སྲིད (page_line >= 0) // out of room in header
		bha_nga;
	    mch_print_start_line(TRUE, page_line);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    གལ་སྲིད (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    དེ་མིན
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
    void
prt_message(char_u *s)
{
    screen_fill((ཧྲིལ)Rows - 1, (ཧྲིལ)Rows, 0, (ཧྲིལ)Columns, ' ', ' ', 0);
    screen_puts(s, (ཧྲིལ)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    void
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    ཧྲིལ			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    ཧྲིལ			page_line;
    ཧྲིལ			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_msg = TRUE;
#endif

#གལ་སྲིད defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    གལ་སྲིད (*eap->arg == '>')
    {
	var_na	*errormsg = NULL;

	// Expand things like "%.ps".
	གལ་སྲིད (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    གལ་སྲིད (errormsg != NULL)
		emsg(errormsg);
	    ལོག;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    དེ་མིན གལ་སྲིད (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    གལ་སྲིད (mch_print_init(&settings,
			curbuf->b_fname == NULL
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == NULL
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	ལོག;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    གལ་སྲིད (gui.in_use)
	settings.modec = 'g';
    དེ་མིན
# endif
	གལ་སྲིད (t_colors > 1)
	    settings.modec = 'c';
	དེ་མིན
	    settings.modec = 't';

    གལ་སྲིད (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    དེ་མིན གལ་སྲིད (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    དེ་མིན
	settings.do_syntax = settings.has_color;
#endif

    // Set up printing attributes for line numbers
    settings.number.fg_color = PRCOLOR_BLACK;
    settings.number.bg_color = PRCOLOR_WHITE;
    settings.number.bold = FALSE;
    settings.number.italic = TRUE;
    settings.number.underline = FALSE;
#ifdef FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    གལ་སྲིད (prt_use_number() && settings.do_syntax)
    {
	ཧྲིལ		id;

	id = syn_name2id((char_u *)"LineNr");
	གལ་སྲིད (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.number, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    དོན་ལུ (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    གལ་སྲིད (bytes_to_print == 0)
    {
	msg(_("No text to be printed"));
	goto print_fail_no_begin;
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

    གལ་སྲིད (!mch_print_begin(&settings))
	goto print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_count = 0;
    དོན་ལུ (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	ཧྲིལ		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_line = eap->line1;
	prtpos = page_prtpos;

	གལ་སྲིད (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    གལ་སྲིད (!mch_print_begin(&settings))
		goto print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	དོན་ལུ (prt_page_count = 0; prtpos.file_line <= eap->line2; ++prt_page_count)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    དོན་ལུ (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		དོན་ལུ (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    གལ་སྲིད (got_int || settings.user_abort)
			goto print_fail;

		    sprintf((var_na *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_count + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (ཧྲིལ)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (ཧྲིལ)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    གལ་སྲིད (!mch_print_begin_page(IObuff))
			goto print_fail;

		    གལ་སྲིད (settings.n_collated_copies > 1)
			sprintf((var_na *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    གལ་སྲིད (prt_header_height() > 0)
			prt_header(&settings, prt_page_count + 1 + side,
							prtpos.file_line);

		    དོན་ལུ (page_line = 0; page_line < settings.lines_per_page;
								  ++page_line)
		    {
			prtpos.column = hardcopy_line(&settings,
							   page_line, &prtpos);
			གལ་སྲིད (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_line)));
			    གལ་སྲིད (++prtpos.file_line > eap->line2)
				bha_nga; // reached the end
			}
			དེ་མིན གལ་སྲིད (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    bha_nga;
			}
		    }

		    གལ་སྲིད (!mch_print_end_page())
			goto print_fail;
		    གལ་སྲིད (prtpos.file_line > eap->line2)
			bha_nga; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		གལ་སྲིད (prtpos.file_line > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    གལ་སྲིད (!mch_print_blank_page())
			goto print_fail;
		}
	    }
	    གལ་སྲིད (settings.duplex && prtpos.file_line <= eap->line2)
		++prt_page_count;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((var_na *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    གལ་སྲིད (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_msg
#endif
		))
    {
	sprintf((var_na *)IObuff, "%s", _("Printing aborted"));
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
    static colnr_T
hardcopy_line(
    prt_settings_T	*psettings,
    ཧྲིལ			page_line,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*line;
    ཧྲིལ		need_break = FALSE;
    ཧྲིལ		outputlen;
    ཧྲིལ		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    ཧྲིལ		id;
#endif

    གལ་སྲིད (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	གལ་སྲིད (!ppos->ff && prt_use_number())
	    prt_line_number(psettings, page_line, ppos->file_line);
	ppos->ff = FALSE;
    }
    དེ་མིན
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_start_line(0, page_line);
    line = ml_get(ppos->file_line);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    དོན་ལུ (col = ppos->column; line[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	གལ་སྲིད (has_mbyte && (outputlen = (*mb_ptr2len)(line + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	གལ་སྲིད (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_line, col, 1, NULL, FALSE);
	    གལ་སྲིད (id > 0)
		id = syn_get_final_id(id);
	    དེ་མིན
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    line = ml_get(ppos->file_line);

	    གལ་སྲིད (id != current_syn_id)
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
	གལ་སྲིད (line[col] == TAB || tab_spaces != 0)
	{
	    གལ་སྲིད (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#དེ་མིན
		tab_spaces = (ཧྲིལ)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    སྐབས (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		གལ་སྲིད (need_break)
		    bha_nga;
	    }
	    // Keep the TAB if we didn't finish it.
	    གལ་སྲིད (need_break && tab_spaces > 0)
		bha_nga;
	}
	དེ་མིན གལ་སྲིད (line[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	དེ་མིན
	{
	    need_break = mch_print_text_out(line + col, outputlen);
	    གལ་སྲིད (has_mbyte)
		print_pos += (*mb_ptr2cells)(line + col);
	    དེ་མིན
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (ཧྲིལ)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    གལ་སྲིད (!ppos->ff
	    && (line[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	ལོག 0;
    ལོག col;
}

#endif //FEAT_PRINTER
