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
#که_تعریف_شوی FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#د_که_پای

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

#که_تعریف_شوی FEAT_SYN_HL
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
#د_که_پای

صحيح	prt_curr_italic;
صحيح	prt_curr_bold;
صحيح	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
صحيح	prt_پاڼه_شمېر;

/*
 * These values determine the print position on a page.
 */
تعريفنوع بنية
{
    صحيح		lead_spaces;	    // remaining spaces for a TAB
    صحيح		print_pos;	    // virtual column for computing TABs
    colnr_T	ستنه;		    // byte column
    linenr_T	file_کرښه;	    // line nr in the buffer
    long_u	بایټونه_printed;	    // bytes printed so far
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
#د_که_پای

ساكن حرف *parse_list_options(char_u *option_str, option_جدول_T *جدول, صحيح جدول_اندازه_2);

ساكن colnr_T hardcopy_کرښه(prt_settings_T *psettings, صحيح پاڼه_کرښه, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printoptions(optset_T *دلیلونه ناکارول_شوی)
{
    أرجع parse_list_options(p_popt, printer_opts, OPT_PRINT_عدد_OPTIONS);
}

#إذا defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printmbfont(optset_T *دلیلونه ناکارول_شوی)
{
    أرجع parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_عدد_OPTIONS);
}
#د_که_پای

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
    option_جدول_T	*جدول,
    صحيح			جدول_اندازه_2)
{
    option_جدول_T *old_opts;
    حرف	*ret = عدم;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    صحيح		idx = 0;		// init for GCC
    صحيح		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_جدول_T, جدول_اندازه_2);
    إذا (old_opts == عدم)
	أرجع عدم;

    لكل (idx = 0; idx < جدول_اندازه_2; ++idx)
    {
	old_opts[idx] = جدول[idx];
	جدول[idx].present = FALSE;
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

	لكل (idx = 0; idx < جدول_اندازه_2; ++idx)
	    إذا (STRNICMP(stringp, جدول[idx].name, len) == 0)
		اكسر;

	إذا (idx == جدول_اندازه_2)
	{
	    ret = e_illegal_component;
	    اكسر;
	}
	p = colonp + 1;
	جدول[idx].present = TRUE;

	إذا (جدول[idx].hasnum)
	{
	    إذا (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		اكسر;
	    }

	    جدول[idx].شمېره = getdigits(&p); // advances p
	}

	جدول[idx].string = p;
	جدول[idx].مزي_اوږدوالی = (صحيح)(commap - p);

	stringp = commap;
	إذا (*stringp == ',')
	    ++stringp;
    }

    إذا (ret != عدم)
    {
	// Restore old options in case of error
	لكل (idx = 0; idx < جدول_اندازه_2; ++idx)
	    جدول[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    أرجع ret;
}


#که_تعریف_شوی FEAT_SYN_HL
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
prt_get_غړی_color(صحيح colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    إذا (t_colors > 8)
	أرجع cterm_color_16[colorindex % 16];
    أرجع cterm_color_8[colorindex % 8];
}

    ساكن فراغ
prt_get_attr(
    صحيح			hl_id,
    prt_متن_attr_T	*pattr,
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
    إذا (USE_24_بټ)
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
	    fg_color = prt_get_غړی_color(colorindex);
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
#د_که_پای // FEAT_SYN_HL

    ساكن فراغ
prt_سېټ_fg(long_u fg)
{
    إذا (fg == prt_curr_fg)
	أرجع;

    prt_curr_fg = fg;
    mch_print_سېټ_fg(fg);
}

    ساكن فراغ
prt_سېټ_bg(long_u bg)
{
    إذا (bg == prt_curr_bg)
	أرجع;

    prt_curr_bg = bg;
    mch_print_سېټ_bg(bg);
}

    ساكن فراغ
prt_سېټ_font(صحيح bold, صحيح italic, صحيح underline)
{
    إذا (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	أرجع;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_سېټ_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ساكن فراغ
prt_کرښه_شمېره(
    prt_settings_T *psettings,
    صحيح		پاڼه_کرښه,
    linenr_T	lnum)
{
    صحيح		i;
    char_u	tbuf[20];

    prt_سېټ_fg(psettings->شمېره.fg_color);
    prt_سېټ_bg(psettings->شمېره.bg_color);
    prt_سېټ_font(psettings->شمېره.bold, psettings->شمېره.italic, psettings->شمېره.underline);
    mch_print_پیل_کرښه(TRUE, پاڼه_کرښه);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((حرف *)tbuf, "%6ld", (اوږد)lnum);
    لكل (i = 0; i < 6; i++)
	(فراغ)mch_print_متن_بهر(&tbuf[i], 1);

#که_تعریف_شوی FEAT_SYN_HL
    إذا (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وإلا
#د_که_پای
    {
	// Set colors and font back to normal.
	prt_سېټ_fg(PRCOLOR_BLACK);
	prt_سېټ_bg(PRCOLOR_WHITE);
	prt_سېټ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    صحيح
prt_سرلیک_لوړوالی(فراغ)
{
    إذا (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	أرجع printer_opts[OPT_PRINT_HEADERHEIGHT].شمېره;
    أرجع 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    صحيح
prt_use_شمېره(فراغ)
{
    أرجع (printer_opts[OPT_PRINT_شمېره].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_شمېره].string[0]) == 'y');
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
    ساكن حرف *(units[4]) = PRT_UNIT_نومونه;

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
prt_سرلیک(
    prt_settings_T  *psettings,
    صحيح		pagenum,
    linenr_T	lnum ناکارول_شوی)
{
    صحيح		پلنوالی = psettings->chars_فی_کرښه;
    صحيح		پاڼه_کرښه;
    char_u	*tbuf;
    char_u	*p;
    صحيح		l;

    // Also use the space for the line number.
    إذا (prt_use_شمېره())
	پلنوالی += PRINT_شمېره_پلنوالی;

    tbuf = alloc(پلنوالی + IOSIZE);
    إذا (tbuf == عدم)
	أرجع;

#که_تعریف_شوی FEAT_STL_OPT
    إذا (*p_سرلیک != NUL)
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
	printer_پاڼه_عدد = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(پلنوالی + IOSIZE), p_سرلیک,
			 (char_u *)"printheader", 0, ' ', پلنوالی, عدم, عدم,
			 عدم);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    وإلا
#د_که_پای
	sprintf((حرف *)tbuf, _("Page %d"), pagenum);

    prt_سېټ_fg(PRCOLOR_BLACK);
    prt_سېټ_bg(PRCOLOR_WHITE);
    prt_سېټ_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    پاڼه_کرښه = 0 - prt_سرلیک_لوړوالی();
    mch_print_پیل_کرښه(TRUE, پاڼه_کرښه);
    لكل (p = tbuf; *p != NUL; )
    {
	إذا (mch_print_متن_بهر(p, (l = (*mb_ptr2len)(p))))
	{
	    ++پاڼه_کرښه;
	    إذا (پاڼه_کرښه >= 0) // out of room in header
		اكسر;
	    mch_print_پیل_کرښه(TRUE, پاڼه_کرښه);
	}
	p += l;
    }

    vim_free(tbuf);

#که_تعریف_شوی FEAT_SYN_HL
    إذا (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    وإلا
#د_که_پای
    {
	// Set colors and font back to normal.
	prt_سېټ_fg(PRCOLOR_BLACK);
	prt_سېټ_bg(PRCOLOR_WHITE);
	prt_سېټ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    فراغ
prt_پیغام(char_u *s)
{
    screen_fill((صحيح)لیکې - 1, (صحيح)لیکې, 0, (صحيح)Columns, ' ', ' ', 0);
    screen_puts(s, (صحيح)لیکې - 1, 0, HL_ATTR(HLF_R));
    بهر_flush();
}

    فراغ
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    صحيح			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		بایټونه_منزل_print = 0;
    صحيح			پاڼه_کرښه;
    صحيح			jobsplit;

    CLEAR_ډګر(settings);
    settings.has_color = TRUE;
#که_تعریف_شوی FEAT_PRINT_PANGO
    settings.user_abort_پیغام = TRUE;
#د_که_پای

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
#د_که_پای

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

#که_تعریف_شوی FEAT_SYN_HL
# ifdef  FEAT_GUI
    إذا (gui.دننه_use)
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
#د_که_پای

    // Set up printing attributes for line numbers
    settings.شمېره.fg_color = PRCOLOR_BLACK;
    settings.شمېره.bg_color = PRCOLOR_WHITE;
    settings.شمېره.bold = FALSE;
    settings.شمېره.italic = TRUE;
    settings.شمېره.underline = FALSE;
#که_تعریف_شوی FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    إذا (prt_use_شمېره() && settings.do_syntax)
    {
	صحيح		id;

	id = syn_name2id((char_u *)"LineNr");
	إذا (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.شمېره, settings.modec);
    }
#د_که_پای

    /*
     * Estimate the total lines to be printed
     */
    لكل (lnum = eap->line1; lnum <= eap->line2; lnum++)
	بایټونه_منزل_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    إذا (بایټونه_منزل_print == 0)
    {
	پیغام(_("No text to be printed"));
	اذهب print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_سېټ_fg(PRCOLOR_BLACK);
    prt_سېټ_bg(PRCOLOR_WHITE);
    prt_سېټ_font(FALSE, FALSE, FALSE);
#که_تعریف_شوی FEAT_SYN_HL
    current_syn_id = -1;
#د_که_پای

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    إذا (!mch_print_begin(&settings))
	اذهب print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_پاڼه_شمېر = 0;
    لكل (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	پاڼه_prtpos;	// print position at page start
	صحيح		side;

	CLEAR_ډګر(پاڼه_prtpos);
	پاڼه_prtpos.file_کرښه = eap->line1;
	prtpos = پاڼه_prtpos;

	إذا (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_پای_ټکی(&settings);
	    إذا (!mch_print_begin(&settings))
		اذهب print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	لكل (prt_پاڼه_شمېر = 0; prtpos.file_کرښه <= eap->line2; ++prt_پاڼه_شمېر)
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
		prtpos = پاڼه_prtpos;

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
			    prt_پاڼه_شمېر + 1 + side,
			    prtpos.بایټونه_printed > 1000000
				? (صحيح)(prtpos.بایټونه_printed /
						       (بایټونه_منزل_print / 100))
				: (صحيح)((prtpos.بایټونه_printed * 100)
							   / بایټونه_منزل_print));
		    إذا (!mch_print_begin_پاڼه(IObuff))
			اذهب print_fail;

		    إذا (settings.n_collated_copies > 1)
			sprintf((حرف *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_پیغام(IObuff);

		    /*
		     * Output header if required
		     */
		    إذا (prt_سرلیک_لوړوالی() > 0)
			prt_سرلیک(&settings, prt_پاڼه_شمېر + 1 + side,
							prtpos.file_کرښه);

		    لكل (پاڼه_کرښه = 0; پاڼه_کرښه < settings.کرښې_فی_پاڼه;
								  ++پاڼه_کرښه)
		    {
			prtpos.ستنه = hardcopy_کرښه(&settings,
							   پاڼه_کرښه, &prtpos);
			إذا (prtpos.ستنه == 0)
			{
			    // finished a file line
			    prtpos.بایټونه_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_کرښه)));
			    إذا (++prtpos.file_کرښه > eap->line2)
				اكسر; // reached the end
			}
			وإلا إذا (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    اكسر;
			}
		    }

		    إذا (!mch_print_پای_ټکی_پاڼه())
			اذهب print_fail;
		    إذا (prtpos.file_کرښه > eap->line2)
			اكسر; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		إذا (prtpos.file_کرښه > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    إذا (!mch_print_blank_پاڼه())
			اذهب print_fail;
		}
	    }
	    إذا (settings.duplex && prtpos.file_کرښه <= eap->line2)
		++prt_پاڼه_شمېر;

	    // Remember the position where the next page starts.
	    پاڼه_prtpos = prtpos;
	}

	vim_snprintf((حرف *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_پیغام(IObuff);
    }

print_fail:
    إذا (got_int || (settings.user_abort
#که_تعریف_شوی FEAT_PRINT_PANGO
		&& settings.user_abort_پیغام
#د_که_پای
		))
    {
	sprintf((حرف *)IObuff, "%s", _("Printing aborted"));
	prt_پیغام(IObuff);
    }
    mch_print_پای_ټکی(&settings);

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    ساكن colnr_T
hardcopy_کرښه(
    prt_settings_T	*psettings,
    صحيح			پاڼه_کرښه,
    prt_pos_T		*ppos)
{
    colnr_T	ستون;
    char_u	*کرښه;
    صحيح		need_break = FALSE;
    صحيح		outputlen;
    صحيح		tab_spaces;
    long_u	print_pos;
#که_تعریف_شوی FEAT_SYN_HL
    prt_متن_attr_T attr;
    صحيح		id;
#د_که_پای

    إذا (ppos->ستنه == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	إذا (!ppos->ff && prt_use_شمېره())
	    prt_کرښه_شمېره(psettings, پاڼه_کرښه, ppos->file_کرښه);
	ppos->ff = FALSE;
    }
    وإلا
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_پیل_کرښه(0, پاڼه_کرښه);
    کرښه = ml_get(ppos->file_کرښه);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    لكل (ستون = ppos->ستنه; کرښه[ستون] != NUL && !need_break; ستون += outputlen)
    {
	outputlen = 1;
	إذا (has_mbyte && (outputlen = (*mb_ptr2len)(کرښه + ستون)) < 1)
	    outputlen = 1;
#که_تعریف_شوی FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	إذا (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_کرښه, ستون, 1, عدم, FALSE);
	    إذا (id > 0)
		id = syn_get_final_id(id);
	    وإلا
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    کرښه = ml_get(ppos->file_کرښه);

	    إذا (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_سېټ_font(attr.bold, attr.italic, attr.underline);
		prt_سېټ_fg(attr.fg_color);
		prt_سېټ_bg(attr.bg_color);
	    }
	}
#د_که_پای

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	إذا (کرښه[ستون] == TAB || tab_spaces != 0)
	{
	    إذا (tab_spaces == 0)
#که_تعریف_شوی FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
		tab_spaces = (صحيح)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#د_که_پای

	    طالما (tab_spaces > 0)
	    {
		need_break = mch_print_متن_بهر((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		إذا (need_break)
		    اكسر;
	    }
	    // Keep the TAB if we didn't finish it.
	    إذا (need_break && tab_spaces > 0)
		اكسر;
	}
	وإلا إذا (کرښه[ستون] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	وإلا
	{
	    need_break = mch_print_متن_بهر(کرښه + ستون, outputlen);
	    إذا (has_mbyte)
		print_pos += (*mb_ptr2cells)(کرښه + ستون);
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
	    && (کرښه[ستون] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	أرجع 0;
    أرجع ستون;
}

#د_که_پای //FEAT_PRINTER
