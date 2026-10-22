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
#اگر_تعریف_شدہ FEAT_POSTSCRIPT
# include "hardcopy_postscript.h"
#اگر_ختم

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

#اگر_تعریف_شدہ FEAT_SYN_HL
ساکن ثابت long_u  cterm_color_8[8] =
{
    (long_u)0x000000L, (long_u)0xff0000L, (long_u)0x00ff00L, (long_u)0xffff00L,
    (long_u)0x0000ffL, (long_u)0xff00ffL, (long_u)0x00ffffL, (long_u)0xffffffL
};

ساکن ثابت long_u  cterm_color_16[16] =
{
    (long_u)0x000000L, (long_u)0x0000c0L, (long_u)0x008000L, (long_u)0x004080L,
    (long_u)0xc00000L, (long_u)0xc000c0L, (long_u)0x808000L, (long_u)0xc0c0c0L,
    (long_u)0x808080L, (long_u)0x6060ffL, (long_u)0x00ff00L, (long_u)0x00ffffL,
    (long_u)0xff8080L, (long_u)0xff40ffL, (long_u)0xffff00L, (long_u)0xffffffL
};

ساکن صحیح		current_syn_id;
#اگر_ختم

صحیح	prt_curr_italic;
صحیح	prt_curr_bold;
صحیح	prt_curr_underline;
long_u	prt_curr_bg;
long_u	prt_curr_fg;
صحیح	prt_صفحہ_گنتی;

/*
 * These values determine the print position on a page.
 */
قسمتعریف ساخت
{
    صحیح		lead_spaces;	    // remaining spaces for a TAB
    صحیح		print_pos;	    // virtual column for computing TABs
    colnr_T	عمود;		    // byte column
    linenr_T	file_سطر;	    // line nr in the buffer
    long_u	بائٹیں_printed;	    // bytes printed so far
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
#اگر_ختم

ساکن حرف *parse_list_options(char_u *option_str, option_جدول_T *جدول, صحیح جدول_سائز_2);

ساکن colnr_T hardcopy_سطر(prt_settings_T *psettings, صحیح صفحہ_سطر, prt_pos_T *ppos);

/*
 * Parse 'printoptions' and set the flags in "printer_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printoptions(optset_T *دلائل غیر_مستعمل)
{
    واپس parse_list_options(p_popt, printer_opts, OPT_PRINT_عدد_OPTIONS);
}

#اگر defined(FEAT_POSTSCRIPT)
/*
 * Parse 'printmbfont' and set the flags in "mbfont_opts".
 * Returns an error message or NULL;
 */
    حرف *
parse_printmbfont(optset_T *دلائل غیر_مستعمل)
{
    واپس parse_list_options(p_pmfn, mbfont_opts, OPT_MBFONT_عدد_OPTIONS);
}
#اگر_ختم

/*
 * Parse a list of options in the form
 * option:value,option:value,option:value
 *
 * "value" can start with a number which is parsed out, e.g.  margin:12mm
 *
 * Returns an error message for an illegal option, NULL otherwise.
 * Only used for the printer at the moment...
 */
    ساکن حرف *
parse_list_options(
    char_u		*option_str,
    option_جدول_T	*جدول,
    صحیح			جدول_سائز_2)
{
    option_جدول_T *old_opts;
    حرف	*ret = تہی;
    char_u	*stringp;
    char_u	*colonp;
    char_u	*commap;
    char_u	*p;
    صحیح		idx = 0;		// init for GCC
    صحیح		len;

    // Save the old values, so that they can be restored in case of an error.
    old_opts = ALLOC_MULT(option_جدول_T, جدول_سائز_2);
    اگر (old_opts == تہی)
	واپس تہی;

    برائے (idx = 0; idx < جدول_سائز_2; ++idx)
    {
	old_opts[idx] = جدول[idx];
	جدول[idx].present = FALSE;
    }

    /*
     * Repeat for all comma separated parts.
     */
    stringp = option_str;
    جب (*stringp)
    {
	colonp = vim_strchr(stringp, ':');
	اگر (colonp == تہی)
	{
	    ret = e_missing_colon_3;
	    توڑو;
	}
	commap = vim_strchr(stringp, ',');
	اگر (commap == تہی)
	    commap = option_str + STRLEN(option_str);

	len = (صحیح)(colonp - stringp);

	برائے (idx = 0; idx < جدول_سائز_2; ++idx)
	    اگر (STRNICMP(stringp, جدول[idx].name, len) == 0)
		توڑو;

	اگر (idx == جدول_سائز_2)
	{
	    ret = e_illegal_component;
	    توڑو;
	}
	p = colonp + 1;
	جدول[idx].present = TRUE;

	اگر (جدول[idx].hasnum)
	{
	    اگر (!VIM_ISDIGIT(*p))
	    {
		ret = e_digit_expected_2;
		توڑو;
	    }

	    جدول[idx].نمبر = getdigits(&p); // advances p
	}

	جدول[idx].string = p;
	جدول[idx].لڑی_لمبائی = (صحیح)(commap - p);

	stringp = commap;
	اگر (*stringp == ',')
	    ++stringp;
    }

    اگر (ret != تہی)
    {
	// Restore old options in case of error
	برائے (idx = 0; idx < جدول_سائز_2; ++idx)
	    جدول[idx] = old_opts[idx];
    }
    vim_free(old_opts);
    واپس ret;
}


#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    ساکن long_u
darken_rgb(long_u rgb)
{
    واپس	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    ساکن long_u
prt_get_رکن_color(صحیح colorindex)
{
    // TODO: Should check for xterm with 88 or 256 colors.
    اگر (t_colors > 8)
	واپس cterm_color_16[colorindex % 16];
    واپس cterm_color_8[colorindex % 8];
}

    ساکن خالی
prt_get_attr(
    صحیح			hl_id,
    prt_متن_attr_T	*pattr,
    صحیح			modec)
{
    صحیح     colorindex;
    long_u  fg_color;
    long_u  bg_color;
    حرف    *color;

    pattr->bold = (highlight_has_attr(hl_id, HL_BOLD, modec) != تہی);
    pattr->italic = (highlight_has_attr(hl_id, HL_ITALIC, modec) != تہی);
    pattr->underline = (highlight_has_attr(hl_id, HL_UNDERLINE, modec) != تہی);
    pattr->undercurl = (highlight_has_attr(hl_id, HL_UNDERCURL, modec) != تہی);
    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (USE_24_ثنائی_ہندسہ)
    {
	bg_color = highlight_gui_color_rgb(hl_id, FALSE);
	اگر (bg_color == PRCOLOR_BLACK)
	    bg_color = PRCOLOR_WHITE;

	fg_color = highlight_gui_color_rgb(hl_id, TRUE);
    }
    ورنہ
# endif
    {
	bg_color = PRCOLOR_WHITE;

	color = (حرف *)highlight_color(hl_id, (char_u *)"fg", modec);
	اگر (color == تہی)
	    colorindex = 0;
	ورنہ
	    colorindex = atoi(color);

	اگر (colorindex >= 0 && colorindex < t_colors)
	    fg_color = prt_get_رکن_color(colorindex);
	ورنہ
	    fg_color = PRCOLOR_BLACK;
    }

    اگر (fg_color == PRCOLOR_WHITE)
	fg_color = PRCOLOR_BLACK;
    ورنہ اگر (*p_bg == 'd')
	fg_color = darken_rgb(fg_color);

    pattr->fg_color = fg_color;
    pattr->bg_color = bg_color;
}
#اگر_ختم // FEAT_SYN_HL

    ساکن خالی
prt_سیٹ_fg(long_u fg)
{
    اگر (fg == prt_curr_fg)
	واپس;

    prt_curr_fg = fg;
    mch_print_سیٹ_fg(fg);
}

    ساکن خالی
prt_سیٹ_bg(long_u bg)
{
    اگر (bg == prt_curr_bg)
	واپس;

    prt_curr_bg = bg;
    mch_print_سیٹ_bg(bg);
}

    ساکن خالی
prt_سیٹ_font(صحیح bold, صحیح italic, صحیح underline)
{
    اگر (prt_curr_bold == bold
	    && prt_curr_italic == italic
	    && prt_curr_underline == underline)
	واپس;

    prt_curr_underline = underline;
    prt_curr_italic = italic;
    prt_curr_bold = bold;
    mch_print_سیٹ_font(bold, italic, underline);
}

/*
 * Print the line number in the left margin.
 */
    ساکن خالی
prt_سطر_نمبر(
    prt_settings_T *psettings,
    صحیح		صفحہ_سطر,
    linenr_T	lnum)
{
    صحیح		i;
    char_u	tbuf[20];

    prt_سیٹ_fg(psettings->نمبر.fg_color);
    prt_سیٹ_bg(psettings->نمبر.bg_color);
    prt_سیٹ_font(psettings->نمبر.bold, psettings->نمبر.italic, psettings->نمبر.underline);
    mch_print_آغاز_سطر(TRUE, صفحہ_سطر);

    // Leave two spaces between the number and the text; depends on
    // PRINT_NUMBER_WIDTH.
    sprintf((حرف *)tbuf, "%6ld", (طویل)lnum);
    برائے (i = 0; i < 6; i++)
	(خالی)mch_print_متن_برآمد(&tbuf[i], 1);

#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ورنہ
#اگر_ختم
    {
	// Set colors and font back to normal.
	prt_سیٹ_fg(PRCOLOR_BLACK);
	prt_سیٹ_bg(PRCOLOR_WHITE);
	prt_سیٹ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Get the currently effective header height.
 */
    صحیح
prt_سرخی_اونچائی(خالی)
{
    اگر (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	واپس printer_opts[OPT_PRINT_HEADERHEIGHT].نمبر;
    واپس 2;
}

/*
 * Return TRUE if using a line number for printing.
 */
    صحیح
prt_use_نمبر(خالی)
{
    واپس (printer_opts[OPT_PRINT_نمبر].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_نمبر].string[0]) == 'y');
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
    ساکن حرف *(units[4]) = PRT_UNIT_نام;

    اگر (!printer_opts[idx].present)
	واپس PRT_UNIT_NONE;

    برائے (i = 0; i < 4; ++i)
	اگر (STRNICMP(printer_opts[idx].string, units[i], 2) == 0)
	{
	    u = i;
	    توڑو;
	}
    واپس u;
}

/*
 * Print the page header.
 */
    ساکن خالی
prt_سرخی(
    prt_settings_T  *psettings,
    صحیح		pagenum,
    linenr_T	lnum غیر_مستعمل)
{
    صحیح		چوڑائی = psettings->chars_فی_سطر;
    صحیح		صفحہ_سطر;
    char_u	*tbuf;
    char_u	*p;
    صحیح		l;

    // Also use the space for the line number.
    اگر (prt_use_نمبر())
	چوڑائی += PRINT_نمبر_چوڑائی;

    tbuf = alloc(چوڑائی + IOSIZE);
    اگر (tbuf == تہی)
	واپس;

#اگر_تعریف_شدہ FEAT_STL_OPT
    اگر (*p_سرخی != NUL)
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
	printer_صفحہ_عدد = pagenum;

	build_stl_str_hl(curwin, tbuf, (size_t)(چوڑائی + IOSIZE), p_سرخی,
			 (char_u *)"printheader", 0, ' ', چوڑائی, تہی, تہی,
			 تہی);

	// Reset line numbers
	curwin->w_cursor.lnum = tmp_lnum;
	curwin->w_topline = tmp_topline;
	curwin->w_botline = tmp_botline;
    }
    ورنہ
#اگر_ختم
	sprintf((حرف *)tbuf, _("Page %d"), pagenum);

    prt_سیٹ_fg(PRCOLOR_BLACK);
    prt_سیٹ_bg(PRCOLOR_WHITE);
    prt_سیٹ_font(TRUE, FALSE, FALSE);

    // Use a negative line number to indicate printing in the top margin.
    صفحہ_سطر = 0 - prt_سرخی_اونچائی();
    mch_print_آغاز_سطر(TRUE, صفحہ_سطر);
    برائے (p = tbuf; *p != NUL; )
    {
	اگر (mch_print_متن_برآمد(p, (l = (*mb_ptr2len)(p))))
	{
	    ++صفحہ_سطر;
	    اگر (صفحہ_سطر >= 0) // out of room in header
		توڑو;
	    mch_print_آغاز_سطر(TRUE, صفحہ_سطر);
	}
	p += l;
    }

    vim_free(tbuf);

#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (psettings->do_syntax)
	// Set colors for next character.
	current_syn_id = -1;
    ورنہ
#اگر_ختم
    {
	// Set colors and font back to normal.
	prt_سیٹ_fg(PRCOLOR_BLACK);
	prt_سیٹ_bg(PRCOLOR_WHITE);
	prt_سیٹ_font(FALSE, FALSE, FALSE);
    }
}

/*
 * Display a print status message.
 */
    خالی
prt_پیغام(char_u *s)
{
    screen_fill((صحیح)صفیں - 1, (صحیح)صفیں, 0, (صحیح)Columns, ' ', ' ', 0);
    screen_puts(s, (صحیح)صفیں - 1, 0, HL_ATTR(HLF_R));
    برآمد_flush();
}

    خالی
ex_hardcopy(exarg_T *eap)
{
    linenr_T		lnum;
    صحیح			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    long_u		بائٹیں_مقصد_print = 0;
    صحیح			صفحہ_سطر;
    صحیح			jobsplit;

    CLEAR_میدان(settings);
    settings.has_color = TRUE;
#اگر_تعریف_شدہ FEAT_PRINT_PANGO
    settings.user_abort_پیغام = TRUE;
#اگر_ختم

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    اگر (*eap->arg == '>')
    {
	حرف	*errormsg = تہی;

	// Expand things like "%.ps".
	اگر (expand_filename(eap, eap->cmdlinep, &errormsg) == FAIL)
	{
	    اگر (errormsg != تہی)
		emsg(errormsg);
	    واپس;
	}
	settings.outfile = skipwhite(eap->arg + 1);
    }
    ورنہ اگر (*eap->arg != NUL)
	settings.arguments = eap->arg;
#اگر_ختم

    /*
     * Initialise for printing.  Ask the user for settings, unless forceit is
     * set.
     * The mch_print_init() code should set up margins if applicable. (It may
     * not be a real printer - for example the engine might generate HTML or
     * PS.)
     */
    اگر (mch_print_init(&settings,
			curbuf->b_fname == تہی
			    ? buf_spname(curbuf)
			    : curbuf->b_sfname == تہی
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit) == FAIL)
	واپس;

#اگر_تعریف_شدہ FEAT_SYN_HL
# ifdef  FEAT_GUI
    اگر (gui.اندر_use)
	settings.modec = 'g';
    ورنہ
# endif
	اگر (t_colors > 1)
	    settings.modec = 'c';
	ورنہ
	    settings.modec = 't';

    اگر (!syntax_present(curwin))
	settings.do_syntax = FALSE;
    ورنہ اگر (printer_opts[OPT_PRINT_SYNTAX].present
	    && TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) != 'a')
	settings.do_syntax =
	       (TOLOWER_ASC(printer_opts[OPT_PRINT_SYNTAX].string[0]) == 'y');
    ورنہ
	settings.do_syntax = settings.has_color;
#اگر_ختم

    // Set up printing attributes for line numbers
    settings.نمبر.fg_color = PRCOLOR_BLACK;
    settings.نمبر.bg_color = PRCOLOR_WHITE;
    settings.نمبر.bold = FALSE;
    settings.نمبر.italic = TRUE;
    settings.نمبر.underline = FALSE;
#اگر_تعریف_شدہ FEAT_SYN_HL
    /*
     * Syntax highlighting of line numbers.
     */
    اگر (prt_use_نمبر() && settings.do_syntax)
    {
	صحیح		id;

	id = syn_name2id((char_u *)"LineNr");
	اگر (id > 0)
	    id = syn_get_final_id(id);

	prt_get_attr(id, &settings.نمبر, settings.modec);
    }
#اگر_ختم

    /*
     * Estimate the total lines to be printed
     */
    برائے (lnum = eap->line1; lnum <= eap->line2; lnum++)
	بائٹیں_مقصد_print += (long_u)STRLEN(skipwhite(ml_get(lnum)));
    اگر (بائٹیں_مقصد_print == 0)
    {
	پیغام(_("No text to be printed"));
	جاؤ print_fail_no_begin;
    }

    // Set colors and font to normal.
    prt_curr_bg = (long_u)0xffffffffL;
    prt_curr_fg = (long_u)0xffffffffL;
    prt_curr_italic = MAYBE;
    prt_curr_bold = MAYBE;
    prt_curr_underline = MAYBE;

    prt_سیٹ_fg(PRCOLOR_BLACK);
    prt_سیٹ_bg(PRCOLOR_WHITE);
    prt_سیٹ_font(FALSE, FALSE, FALSE);
#اگر_تعریف_شدہ FEAT_SYN_HL
    current_syn_id = -1;
#اگر_ختم

    jobsplit = (printer_opts[OPT_PRINT_JOBSPLIT].present
	   && TOLOWER_ASC(printer_opts[OPT_PRINT_JOBSPLIT].string[0]) == 'y');

    اگر (!mch_print_begin(&settings))
	جاؤ print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    prt_صفحہ_گنتی = 0;
    برائے (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	prt_pos_T	prtpos;		// current print position
	prt_pos_T	صفحہ_prtpos;	// print position at page start
	صحیح		side;

	CLEAR_میدان(صفحہ_prtpos);
	صفحہ_prtpos.file_سطر = eap->line1;
	prtpos = صفحہ_prtpos;

	اگر (jobsplit && collated_copies > 0)
	{
	    // Splitting jobs: Stop a previous job and start a new one.
	    mch_print_انتہا(&settings);
	    اگر (!mch_print_begin(&settings))
		جاؤ print_fail_no_begin;
	}

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	برائے (prt_صفحہ_گنتی = 0; prtpos.file_سطر <= eap->line2; ++prt_صفحہ_گنتی)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     * For duplex: 12 12 12 34 34 34, ...
	     */
	    برائے (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		// Set the print position to the start of this page.
		prtpos = صفحہ_prtpos;

		/*
		 * Do front and rear side of a page.
		 */
		برائے (side = 0; side <= settings.duplex; ++side)
		{
		    /*
		     * Print one page.
		     */

		    // Check for interrupt character every page.
		    ui_breakcheck();
		    اگر (got_int || settings.user_abort)
			جاؤ print_fail;

		    sprintf((حرف *)IObuff, _("Printing page %d (%d%%)"),
			    prt_صفحہ_گنتی + 1 + side,
			    prtpos.بائٹیں_printed > 1000000
				? (صحیح)(prtpos.بائٹیں_printed /
						       (بائٹیں_مقصد_print / 100))
				: (صحیح)((prtpos.بائٹیں_printed * 100)
							   / بائٹیں_مقصد_print));
		    اگر (!mch_print_begin_صفحہ(IObuff))
			جاؤ print_fail;

		    اگر (settings.n_collated_copies > 1)
			sprintf((حرف *)IObuff + STRLEN(IObuff),
				_(" Copy %d of %d"),
				collated_copies + 1,
				settings.n_collated_copies);
		    prt_پیغام(IObuff);

		    /*
		     * Output header if required
		     */
		    اگر (prt_سرخی_اونچائی() > 0)
			prt_سرخی(&settings, prt_صفحہ_گنتی + 1 + side,
							prtpos.file_سطر);

		    برائے (صفحہ_سطر = 0; صفحہ_سطر < settings.سطریں_فی_صفحہ;
								  ++صفحہ_سطر)
		    {
			prtpos.عمود = hardcopy_سطر(&settings,
							   صفحہ_سطر, &prtpos);
			اگر (prtpos.عمود == 0)
			{
			    // finished a file line
			    prtpos.بائٹیں_printed +=
				  STRLEN(skipwhite(ml_get(prtpos.file_سطر)));
			    اگر (++prtpos.file_سطر > eap->line2)
				توڑو; // reached the end
			}
			ورنہ اگر (prtpos.ff)
			{
			    // Line had a formfeed in it - start new page but
			    // stay on the current line
			    توڑو;
			}
		    }

		    اگر (!mch_print_انتہا_صفحہ())
			جاؤ print_fail;
		    اگر (prtpos.file_سطر > eap->line2)
			توڑو; // reached the end
		}

		/*
		 * Extra blank page for duplexing with odd number of pages and
		 * more copies to come.
		 */
		اگر (prtpos.file_سطر > eap->line2 && settings.duplex
								 && side == 0
		    && uncollated_copies + 1 < settings.n_uncollated_copies)
		{
		    اگر (!mch_print_blank_صفحہ())
			جاؤ print_fail;
		}
	    }
	    اگر (settings.duplex && prtpos.file_سطر <= eap->line2)
		++prt_صفحہ_گنتی;

	    // Remember the position where the next page starts.
	    صفحہ_prtpos = prtpos;
	}

	vim_snprintf((حرف *)IObuff, IOSIZE, _("Printed: %s"),
							    settings.jobname);
	prt_پیغام(IObuff);
    }

print_fail:
    اگر (got_int || (settings.user_abort
#اگر_تعریف_شدہ FEAT_PRINT_PANGO
		&& settings.user_abort_پیغام
#اگر_ختم
		))
    {
	sprintf((حرف *)IObuff, "%s", _("Printing aborted"));
	prt_پیغام(IObuff);
    }
    mch_print_انتہا(&settings);

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    ساکن colnr_T
hardcopy_سطر(
    prt_settings_T	*psettings,
    صحیح			صفحہ_سطر,
    prt_pos_T		*ppos)
{
    colnr_T	ستون;
    char_u	*سطر;
    صحیح		need_break = FALSE;
    صحیح		outputlen;
    صحیح		tab_spaces;
    long_u	print_pos;
#اگر_تعریف_شدہ FEAT_SYN_HL
    prt_متن_attr_T attr;
    صحیح		id;
#اگر_ختم

    اگر (ppos->عمود == 0 || ppos->ff)
    {
	print_pos = 0;
	tab_spaces = 0;
	اگر (!ppos->ff && prt_use_نمبر())
	    prt_سطر_نمبر(psettings, صفحہ_سطر, ppos->file_سطر);
	ppos->ff = FALSE;
    }
    ورنہ
    {
	// left over from wrap halfway a tab
	print_pos = ppos->print_pos;
	tab_spaces = ppos->lead_spaces;
    }

    mch_print_آغاز_سطر(0, صفحہ_سطر);
    سطر = ml_get(ppos->file_سطر);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    برائے (ستون = ppos->عمود; سطر[ستون] != NUL && !need_break; ستون += outputlen)
    {
	outputlen = 1;
	اگر (has_mbyte && (outputlen = (*mb_ptr2len)(سطر + ستون)) < 1)
	    outputlen = 1;
#اگر_تعریف_شدہ FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	اگر (psettings->do_syntax)
	{
	    id = syn_get_id(curwin, ppos->file_سطر, ستون, 1, تہی, FALSE);
	    اگر (id > 0)
		id = syn_get_final_id(id);
	    ورنہ
		id = 0;
	    // Get the line again, a multi-line regexp may invalidate it.
	    سطر = ml_get(ppos->file_سطر);

	    اگر (id != current_syn_id)
	    {
		current_syn_id = id;
		prt_get_attr(id, &attr, psettings->modec);
		prt_سیٹ_font(attr.bold, attr.italic, attr.underline);
		prt_سیٹ_fg(attr.fg_color);
		prt_سیٹ_bg(attr.bg_color);
	    }
	}
#اگر_ختم

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	اگر (سطر[ستون] == TAB || tab_spaces != 0)
	{
	    اگر (tab_spaces == 0)
#اگر_تعریف_شدہ FEAT_VARTABS
		tab_spaces = tabstop_padding(print_pos, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ورنہ
		tab_spaces = (صحیح)(curbuf->b_p_ts - (print_pos % curbuf->b_p_ts));
#اگر_ختم

	    جب (tab_spaces > 0)
	    {
		need_break = mch_print_متن_برآمد((char_u *)" ", 1);
		print_pos++;
		tab_spaces--;
		اگر (need_break)
		    توڑو;
	    }
	    // Keep the TAB if we didn't finish it.
	    اگر (need_break && tab_spaces > 0)
		توڑو;
	}
	ورنہ اگر (سطر[ستون] == FF
		&& printer_opts[OPT_PRINT_FORMFEED].present
		&& TOLOWER_ASC(printer_opts[OPT_PRINT_FORMFEED].string[0])
								       == 'y')
	{
	    ppos->ff = TRUE;
	    need_break = 1;
	}
	ورنہ
	{
	    need_break = mch_print_متن_برآمد(سطر + ستون, outputlen);
	    اگر (has_mbyte)
		print_pos += (*mb_ptr2cells)(سطر + ستون);
	    ورنہ
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
	    && (سطر[ستون] == NUL
		|| (printer_opts[OPT_PRINT_WRAP].present
		    && TOLOWER_ASC(printer_opts[OPT_PRINT_WRAP].string[0])
								     == 'n')))
	واپس 0;
    واپس ستون;
}

#اگر_ختم //FEAT_PRINTER
