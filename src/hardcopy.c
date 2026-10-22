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

#если defined(FEAT_PRINTER)

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

static целое		current_syn_id;
#endif

целое	prt_curr_italic;
целое	prt_curr_bold;
целое	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
целое	prt_page_count;

/*
 * These values determine the print position on a page.
 */
typedef struct
{
    целое		lead_spaces;	    // remaining spaces for a TAB
    целое		print_pos;	    // virtual column for computing TABs
    colnr_T	column;		    // byte column
    linenr_T	file_line;	    // line nr in the buffer
    long_u	bytes_printed;	    // bytes printed so far
    целое		ff;		    // seen form feed character
} prt_pos_T;

#если defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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

static char *parse_list_options(char_u *option_str, option_table_T *table, целое table_size);

static colnr_T hardcopy_line(prt_settings_T *psettings, целое page_line, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    char *
parse_printoptions(optset_T *args UNUSED)
{
    вернуть parse_list_options(p_popt, printer_opts, OPT_PRINT_NUM_OPTIONS);
}

#если defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    char *
parse_printmbfont(optset_T *args UNUSED)
{
    вернуть parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_NUM_OPTIONS);
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
    static char *
parse_list_options(
    char_u		*option_str,
    option_table_T	*table,
    целое			table_size)
{
    option_table_T *old_opts;
    char	*ret = NULL;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    целое		idx = 0;		// init for GCC
    целое		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_table_T, table_size);
    если (old_opts == NULL)
	вернуть NULL;

    для (idx = 0; idx < table_size; ++idx)
    {
	old_opts[idx] = table[idx];
	table[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    пока (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	если (colonp == NULL)
	{
	    ret = e_missing_colon_3;
	    break;
	}
	commap = vim_strchr(stringp, ',');
	если (commap == NULL)
	    commap = option_str + STRLEN(option_str);

	len = (целое)(colonp - stringp);

	для (idx = 0; idx < table_size; ++idx)
	    если (STRNICMP(stringp, table[idx].name, len) == 0)
		break;

	если (idx == table_size)
	{
	    ret = e_illegal_component;
	    break;
	}
	p = colonp + 1;
	table[idx].present = TRUE;

	если (table[idx].hasnum)
	{
	    если (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		break;
	    }

	    table[idx].number = getdigits(&p); // advances p
	}

	table[idx].string = p;
	table[idx].strlen = (целое)(commap - p);

	stringp = commap;
	если (*stringp == ',')
	    ++stringp;
    }

    если (ret != NULL)
    {
	// Restore old options in case of error
	для (idx = 0; idx < table_size; ++idx)
	    table[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    вернуть ret;
}


#ifdef FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    static long_u
darken_rgb(long_u rgb)
{
    вернуть	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    static long_u
prt_get_term_color(целое colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    если (t_colors > 8)
	вернуть cterm_color_16[colorindex % 16];
    вернуть cterm_color_8[colorindex % 8];
}

    static void
prt_get_attr(
    целое			hl_id,
    prt_text_attr_T	*pattr,
    целое			modec)
{
    целое     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    char    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != NULL);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != NULL);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != NULL);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != NULL);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# если defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    если (USE_24BIT)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	если (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    иначе
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (char *)highlight_color(hl_id, (char_u *)"fg", modec);
	если (color == NULL)
	    colorindex = 0;
	иначе
	    colorindex = atoi(color);

	если (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_term_color(colorindex);
	иначе
	    fg_color = PRCOLOR_BLACK;
    }

    если (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    иначе если (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#endif // FEAT_SYN_HL

    static void
prt_set_fg(long_u fg)
{
    если (fg == prt_curr_fg)
	вернуть;

    prt_curr_fg = fg;
    mch_print_set_fg(fg);
}

    static void
prt_set_bg(long_u bg)
{
    если (bg == prt_curr_bg)
	вернуть;

    prt_curr_bg = bg;
    mch_print_set_bg(bg);
}

    static void
prt_set_font(целое bold, целое italic, целое underline)
{
    если (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	вернуть;

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
    целое		page_line,
    linenr_T	lnum)
{
    целое		i;
    char_u	tbuf[20];

    prt_set_fg(psettings->number.fg_color);
    prt_set_bg(psettings->number.bg_color);
    prt_set_font(psettings->number.bold, psettings->number.italic, psettings->number.underline);
    mch_print_start_line(TRUE, page_line);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((char *)tbuf, "%6ld", (long)lnum);
    для (i = 0; i < 6; i++)
	(void)mch_print_text_out(&tbuf[i], 1);

#ifdef FEAT_SYN_HL
    если (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    иначе
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
    целое
prt_header_height(void)
{
    если (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	вернуть printer_opts[OPT_PRINT_HEADERHEIGHT].number;
    вернуть 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    целое
prt_use_number(void)
{
    вернуть (printer_opts[OPT_PRINT_NUMBER].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_NUMBER].string[0]) == 'y');
}

/*
 * Return the unit used in a margin item in 'printoptions'.
 * Returns PRT_UNIT_NONE if not recognized.
 */
    целое
prt_get_unit(целое idx)
{
    целое		u = PRT_UNIT_NONE;
    целое		i;
    static char *(units[4]) = PRT_UNIT_NAMES;

    если (!printer_opts[idx].present)
	вернуть PRT_UNIT_NONE;

    для (i = 0; i < 4; ++i)
	если (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    break;
	}
    вернуть u;
}

/*
 * Print the page header.
 */
    static void
prt_header(
    prt_settings_T  *psettings,
    целое		pagenum,
    linenr_T	lnum UNUSED)
{
    целое		width = psettings->chars_per_line;
    целое		page_line;
    char_u	*tbuf;
    char_u	*p;
    целое		l;

    // Also use the space for the line number.
    если (prt_use_number())
	width += PRINT_NUMBER_WIDTH;

    tbuf = alloc(width + IOSIZE);
    если (tbuf == NULL)
	вернуть;

#ifdef FEAT_STL_OPT
    если (*p_header != NUL)
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
    иначе
#endif
	sprintf((char *)tbuf, _("Page %d"), pagenum);

    prt_set_fg(PRCOLOR_BLACK);
    prt_set_bg(PRCOLOR_WHITE);
    prt_set_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    page_line = 0 - prt_header_height();
    mch_print_start_line(TRUE, page_line);
    для (p = tbuf; *p != NUL; )
    {
	если (mch_print_text_out(p, (l = (*mb_ptr2len)(p))))
	{
	    ++page_line;
	    если (page_line >= 0) // out of room in header
		break;
	    mch_print_start_line(TRUE, page_line);
	}
	p += l;
    }

    vim_free(tbuf);

#ifdef FEAT_SYN_HL
    если (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    иначе
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
    screen_fill((целое)Rows - 1, (целое)Rows, 0, (целое)Columns, ' ', ' ', 0);
    screen_puts(s, (целое)Rows - 1, 0, HL_ATTR(HLF_R));
    out_flush();
}

    void
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    целое			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		bytes_to_print = 0;
    целое			page_line;
    целое			jobsplit;

    CLEAR_FIELD(settings);
    settings.has_color = TRUE;
#ifdef FEAT_PRINT_PANGO
    settings.user_abort_msg = TRUE;
#endif

#если defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    если (*eap->arg == '>')
    {
	char	*errormsg = NULL;

	// Expand things like "%.ps".
	если (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    если (errormsg != NULL)
		emsg(errormsg);
	    вернуть;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    иначе если (*eap->arg != NUL)
	settings.arguments = eap->arg;
#endif

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    если (mch_print_init(&settings,
			curbuf->b_fname == NULL
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == NULL
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	вернуть;

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    если (gui.in_use)
	settings.modec = 'g';
    иначе
# endif
	если (t_colors > 1)
	    settings.modec = 'c';
	иначе
	    settings.modec = 't';

    если (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    иначе если (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    иначе
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
    если (prt_use_number() && settings.do_syntax)
    {
	целое		id;

	id = syn_name2id((char_u *)"LineNr");
	если (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.number, settings.modec);
    }
#endif

    /*
     * Estimate the total lines to be printed
     */
    для (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    если (bytes_to_print == 0)
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

    если (!mch_print_begin(&settings))
	goto print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_page_count = 0;
    для (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	page_prtpos;	// print position at page start
	целое		side;

	CLEAR_FIELD(page_prtpos);
	page_prtpos.file_line = eap->line1;
	prtpos = page_prtpos;

	если (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_end(&settings);
	    если (!mch_print_begin(&settings))
		goto print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	для (prt_page_count = 0; prtpos.file_line <= eap->line2; ++prt_page_count)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    для (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = page_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		для (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    если (got_int || settings.user_abort)
			goto print_fail;

		    sprintf((char *)IObuff, _("Printing page %d (%d%%)"),
			    prt_page_count + 1 + side,
			    prtpos.bytes_printed > 1000000
				? (целое)(prtpos.bytes_printed /
						       (bytes_to_print / 100))
				: (целое)((prtpos.bytes_printed * 100)
							   / bytes_to_print));
		    если (!mch_print_begin_page(IObuff))
			goto print_fail;

		    если (settings.n_collated_copies > 1)
			sprintf((char *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_message(IObuff);

		    /*
		     * Output header if required
		     */
		    если (prt_header_height() > 0)
			prt_header(&settings, prt_page_count + 1 + side,
							prtpos.file_line);

		    для (page_line = 0; page_line < settings.lines_per_page;
								  ++page_line)
		    {
			prtpos.column = hardcopy_line(&settings,
							   page_line, &prtpos);
			если (prtpos.column == 0)
			{
			    // finished a file line
			    prtpos.bytes_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_line)));
			    если (++prtpos.file_line > eap->line2)
				break; // reached the end
			}
			иначе если (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    break;
			}
		    }

		    если (!mch_print_end_page())
			goto print_fail;
		    если (prtpos.file_line > eap->line2)
			break; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		если (prtpos.file_line > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    если (!mch_print_blank_page())
			goto print_fail;
		}
	    }
	    если (settings.duplex && prtpos.file_line <= eap->line2)
		++prt_page_count;

	    // Remember the position where the next page starts.
	    page_prtpos = prtpos;
	}

	vim_snprintf((char *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_message(IObuff);
    }

print_fail:
    если (got_int || (settings.user_abort
#ifdef FEAT_PRINT_PANGO
		&& settings.user_abort_msg
#endif
		))
    {
	sprintf((char *)IObuff, "%s", _("Printing aborted"));
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
    целое			page_line,
    prt_pos_T		*ppos)
{
    colnr_T	col;
    char_u	*line;
    целое		need_break = FALSE;
    целое		outputlen;
    целое		tab_spaces;
    long_u	print_pos;
#ifdef FEAT_SYN_HL
    prt_text_attr_T attr;
    целое		id;
#endif

    если (ppos->column == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	если (!ppos->ff && prt_use_number())
	    prt_line_number(psettings, page_line, ppos->file_line);
	ppos->ff = FALSE;
    }
    иначе
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
    для (col = ppos->column; line[col] != NUL && !need_break; col += outputlen)
    {
	outputlen = 1;
	если (has_mbyte && (outputlen = (*mb_ptr2len)(line + col)) < 1)
	    outputlen = 1;
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	если (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_line, col, 1, NULL, FALSE);
	    если (id > 0)
		id = syn_get_final_id(id);
	    иначе
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    line = ml_get(ppos->file_line);

	    если (id != current_syn_id)
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
	если (line[col] == TAB || tab_spaces != 0)
	{
	    если (tab_spaces == 0)
#ifdef FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#иначе
		tab_spaces = (целое)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#endif

	    пока (tab_spaces > 0)
	    {
		need_break = mch_print_text_out((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		если (need_break)
		    break;
	    }
	    // Keep the TAB if we didn't finish it.
	    если (need_break && tab_spaces > 0)
		break;
	}
	иначе если (line[col] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	иначе
	{
	    need_break = mch_print_text_out(line + col, outputlen);
	    если (has_mbyte)
		print_pos += (*mb_ptr2cells)(line + col);
	    иначе
		print_pos++;
	}
    }

    ppos->lead_spaces = tab_spaces;
    ppos->print_pos = (целое)print_pos;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line, unless we are doing a formfeed.
     */
    если (!ppos->ff
	    && (line[col] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	вернуть 0;
    вернуть col;
}

#endif //FEAT_PRINTER
