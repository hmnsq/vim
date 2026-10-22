/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_getln.c: Functions for entering and editing an Ex command line.
 */

#అన్తర్భావ "vim.h"

// Return value when handling keys in command-line mode.
#నిర్వచించు CMDLINE_NOT_మారింది	1
#నిర్వచించు CMDLINE_మారింది		2
#నిర్వచించు GOTO_సాధారణ_బహుళకం	3
#నిర్వచించు PROCESS_తదుపరి_కీలకం	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
స్థిర cmdline_info_T ccline;

#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం	new_cmdpos;	// position set by set_cmdline_pos()
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
స్థిర పూర్ణాంకం	extra_char_జరుపు;

#నిర్వచితమైతే FEAT_RIGHTLEFT
స్థిర పూర్ణాంకం	cmd_hkmap = 0;	// Hebrew mapping during command line
#అయితే_ముగింపు

స్థిర char_u	*getcmdline_int(పూర్ణాంకం firstc, దీర్ఘ లెక్క, పూర్ణాంకం indent, పూర్ణాంకం clear_ccline);
స్థిర పూర్ణాంకం	cmdline_charsize(పూర్ణాంకం idx);
స్థిర శూన్య	సమితి_cmdspos(శూన్య);
స్థిర శూన్య	సమితి_cmdspos_cursor(శూన్య);
స్థిర శూన్య	correct_cmdspos(పూర్ణాంకం idx, పూర్ణాంకం cells);
స్థిర శూన్య	dealloc_cmdbuff(శూన్య);
స్థిర శూన్య	alloc_cmdbuff(size_t len);
స్థిర శూన్య	draw_cmdline(పూర్ణాంకం ప్రారంభం, పూర్ణాంకం len);
స్థిర శూన్య	save_cmdline(cmdline_info_T *ccp);
స్థిర శూన్య	restore_cmdline(cmdline_info_T *ccp);
స్థిర పూర్ణాంకం	cmdline_paste(పూర్ణాంకం regname, పూర్ణాంకం literally, పూర్ణాంకం remcr);
స్థిర శూన్య	redrawcmdprompt(శూన్య);
స్థిర పూర్ణాంకం	ccheck_abbr(పూర్ణాంకం);
స్థిర పూర్ణాంకం	open_cmdwin(శూన్య);
#నిర్వచితమైతే FEAT_శోధన_EXTRA
స్థిర పూర్ణాంకం	ఖాళీలు_నమూనా_magic(char_u *pat, size_t len, magic_T magic_val);
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	cedit_కీలకం = -1;	// key value of 'cedit' option

    స్థిర శూన్య
trigger_cmd_autocmd(పూర్ణాంకం typechar, పూర్ణాంకం evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    స్థిర శూన్య
abandon_cmdline(శూన్య)
{
    dealloc_cmdbuff();
    ఒకవేళ (సందేశం_scrolled == 0)
	compute_cmdrow();
    సందేశం("");
    redraw_cmdline = TRUE;
}

#నిర్వచితమైతే FEAT_శోధన_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    స్థిర పూర్ణాంకం
ఖాళీలు_నమూనా(char_u *p, size_t len, పూర్ణాంకం delim)
{
    magic_T	magic_val = MAGIC_ON;

    ఒకవేళ (len > 0)
	(శూన్య) skip_regexp_ex(p, delim, magic_isset(), ఖాళీ, ఖాళీ, &magic_val);
    లేకపోతే
	తిరిగి TRUE;

    తిరిగి ఖాళీలు_నమూనా_magic(p, len, magic_val);
}

    స్థిర పూర్ణాంకం
ఖాళీలు_నమూనా_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    అయితే (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ఖాళీ)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    తిరిగి len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
రకనిర్వచనం నిర్మాణం {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    పూర్ణాంకం		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_ఖాళీలు_అడ్డువరుసలు;
} viewstate_T;

    స్థిర శూన్య
save_viewstate(viewstate_T *vs)
{
    vs->vs_curswant = curwin->w_curswant;
    vs->vs_leftcol = curwin->w_leftcol;
    vs->vs_skipcol = curwin->w_skipcol;
    vs->vs_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    vs->vs_topfill = curwin->w_topfill;
# endif
    vs->vs_botline = curwin->w_botline;
    vs->vs_ఖాళీలు_అడ్డువరుసలు = curwin->w_ఖాళీలు_అడ్డువరుసలు;
}

    స్థిర శూన్య
restore_viewstate(viewstate_T *vs)
{
    curwin->w_curswant = vs->vs_curswant;
    curwin->w_leftcol = vs->vs_leftcol;
    curwin->w_skipcol = vs->vs_skipcol;
    curwin->w_topline = vs->vs_topline;
# ifdef FEAT_DIFF
    curwin->w_topfill = vs->vs_topfill;
# endif
    curwin->w_botline = vs->vs_botline;
    curwin->w_ఖాళీలు_అడ్డువరుసలు = vs->vs_ఖాళీలు_అడ్డువరుసలు;
}

// Struct to store the state of 'incsearch' highlighting.
రకనిర్వచనం నిర్మాణం {
    pos_T	శోధన_ప్రారంభం_2;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    పూర్ణాంకం		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	సరిపోలిక_ప్రారంభం;
    pos_T	సరిపోలిక_చివర;
    పూర్ణాంకం		did_incsearch;
    పూర్ణాంకం		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    స్థిర శూన్య
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->సరిపోలిక_ప్రారంభం = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->సరిపోలిక_చివర);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->శోధన_ప్రారంభం_2 = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    స్థిర శూన్య
సమితి_శోధన_సరిపోలిక(pos_T *t)
{
    t->lnum += శోధన_సరిపోలిక_పంక్తులు;
    t->నిలువు = శోధన_సరిపోలిక_endcol;
    ఒకవేళ (t->lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	t->lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    పూర్ణాంకం
parse_నమూనా_and_range(
	pos_T	*incsearch_ప్రారంభం,
	పూర్ణాంకం	*శోధన_delim,
	పూర్ణాంకం	*skiplen,
	పూర్ణాంకం	*patlen)
{
    char_u	*cmd, *p, *చివర;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    పూర్ణాంకం		delim_optional = FALSE;
    పూర్ణాంకం		delim;
    పూర్ణాంకం		use_చివరి_pat;
    magic_T     magic = 0;
    అక్షరం	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    శోధన_మొదటి_పంక్తి = 0;
    శోధన_చివరి_పంక్తి = MAXLNUM;

    CLEAR_క్షేత్రం(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_పంక్తులు;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ఖాళీ);

    ఒకవేళ (vim_strchr((char_u *)"sgvlu", *cmd) == ఖాళీ)
	తిరిగి FALSE;

    // Skip over command name to find pattern separator
    కొరకు (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    ఒకవేళ (*skipwhite(p) == NUL)
	తిరిగి FALSE;

    ఒకవేళ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	ఒకవేళ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	లేకపోతే ఒకవేళ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    లేకపోతే ఒకవేళ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	ఒకవేళ (*p == '!')
	    p = skipwhite(p + 1);
	అయితే (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	ఒకవేళ (*p == NUL)
	    తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	ఒకవేళ (*p == '!')
	{
	    p++;
	    ఒకవేళ (*skipwhite(p) == NUL)
		తిరిగి FALSE;
	}
	ఒకవేళ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    లేకపోతే
	తిరిగి FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *శోధన_delim = delim;

    చివర = skip_regexp_ex(p, delim, magic_isset(), ఖాళీ, ఖాళీ, &magic);
    use_చివరి_pat = చివర == p && *చివర == delim;

    ఒకవేళ (చివర == p && !use_చివరి_pat)
	తిరిగి FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    ఒకవేళ (!use_చివరి_pat)
    {
	అక్షరం c = *చివర;
	పూర్ణాంకం ఖాళీలు_2;

	*చివర = NUL;
	ఖాళీలు_2 = ఖాళీలు_నమూనా_magic(p, (size_t)(చివర - p), magic);
	*చివర = c;
	ఒకవేళ (ఖాళీలు_2)
	    తిరిగి FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (పూర్ణాంకం)(p - ccline.cmdbuff);
    *patlen = (పూర్ణాంకం)(చివర - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_ప్రారంభం;

    parse_cmd_చిరునామా(&ea, &dummy, TRUE);

    ఒకవేళ (ea.addr_లెక్క > 0)
    {
	పూర్ణాంకం తిరగేయి_సరిపోలిక = ea.line2 < ea.line1;
	శోధన_మొదటి_పంక్తి = తిరగేయి_సరిపోలిక ? ea.line2 : ea.line1;
	శోధన_చివరి_పంక్తి = తిరగేయి_సరిపోలిక ? ea.line1 : ea.line2;
    }
    లేకపోతే ఒకవేళ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	శోధన_మొదటి_పంక్తి = శోధన_చివరి_పంక్తి = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    తిరిగి TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    స్థిర పూర్ణాంకం
do_incsearch_highlighting(
	పూర్ణాంకం		    firstc,
	పూర్ణాంకం		    *శోధన_delim,
	incsearch_state_T   *is_state,
	పూర్ణాంకం		    *skiplen,
	పూర్ణాంకం		    *patlen)
{
    పూర్ణాంకం retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    ఒకవేళ (!p_is || cmd_silent)
	తిరిగి FALSE;

    // By default search all lines
    శోధన_మొదటి_పంక్తి = 0;
    శోధన_చివరి_పంక్తి = MAXLNUM;

    ఒకవేళ (firstc == '/' || firstc == '?')
    {
	*శోధన_delim = firstc;
	తిరిగి TRUE;
    }

    ఒకవేళ (firstc != ':')
	తిరిగి FALSE;

    ++emsg_off;
    retval = parse_నమూనా_and_range(&is_state->శోధన_ప్రారంభం_2, శోధన_delim,
	    skiplen, patlen);
    --emsg_off;

    తిరిగి retval;
}

    స్థిర శూన్య
ముగింపు_incsearch_highlighting(
	పూర్ణాంకం gotesc,
	incsearch_state_T *is_state,
	పూర్ణాంకం call_update_screen)
{
    ఒకవేళ (!is_state->did_incsearch)
	తిరిగి;

    is_state->did_incsearch = FALSE;
    ఒకవేళ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    లేకపోతే
    {
	ఒకవేళ (!EQUAL_POS(is_state->save_cursor, is_state->శోధన_ప్రారంభం_2))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->శోధన_ప్రారంభం_2;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_సరిపోలిక = FALSE;

    // by default search all lines
    శోధన_మొదటి_పంక్తి = 0;
    శోధన_చివరి_పంక్తి = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    స్థితి_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    ఒకవేళ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    స్థిర శూన్య
may_do_incsearch_highlighting(
	పూర్ణాంకం		    firstc,
	దీర్ఘ		    లెక్క,
	incsearch_state_T   *is_state)
{
    పూర్ణాంకం		skiplen, patlen;
    పూర్ణాంకం		దొరికింది;  // do_search() result
    pos_T	చివర_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    పూర్ణాంకం		తదుపరి_char;
    పూర్ణాంకం		use_చివరి_pat;
    పూర్ణాంకం		did_do_incsearch = is_state->did_incsearch;
    పూర్ణాంకం		శోధన_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_చివరి_శోధన_నమూనా();

    ఒకవేళ (!do_incsearch_highlighting(firstc, &శోధన_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_చివరి_శోధన_నమూనా();
	ముగింపు_incsearch_highlighting(FALSE, is_state, TRUE);
	ఒకవేళ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	తిరిగి;
    }

    // If there is a character waiting, search and redraw later.
    ఒకవేళ (char_avail())
    {
	restore_చివరి_శోధన_నమూనా();
	is_state->incsearch_postponed = TRUE;
	తిరిగి;
    }
    is_state->incsearch_postponed = FALSE;

    ఒకవేళ (శోధన_మొదటి_పంక్తి == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->శోధన_ప్రారంభం_2;
    లేకపోతే ఒకవేళ (శోధన_మొదటి_పంక్తి > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	curwin->w_cursor.నిలువు = MAXCOL;
    }
    లేకపోతే
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = శోధన_మొదటి_పంక్తి;
	curwin->w_cursor.నిలువు = 0;
    }

    // Use the previous pattern for ":s//".
    తదుపరి_char = ccline.cmdbuff[skiplen + patlen];
    use_చివరి_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == తదుపరి_char;

    // If there is no pattern, don't do anything.
    ఒకవేళ (patlen == 0 && !use_చివరి_pat)
    {
	దొరికింది = 0;
	సమితి_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    లేకపోతే
    {
	పూర్ణాంకం శోధన_flags = శోధన_OPT + శోధన_NOOF + శోధన_PEEK;

	cursor_off();	// so the user knows we're busy
	బహిర్గతం_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	ఒకవేళ (!p_hls)
	    శోధన_flags += శోధన_KEEP;
	ఒకవేళ (శోధన_మొదటి_పంక్తి != 0)
	    శోధన_flags += శోధన_ప్రారంభం;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_క్షేత్రం(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	దొరికింది = do_శోధన(ఖాళీ, firstc == ':' ? '/' : firstc, శోధన_delim,
				 ccline.cmdbuff + skiplen, patlen, లెక్క, శోధన_flags,
# ifdef FEAT_RELTIME
		&sia
# లేకపోతే
		ఖాళీ
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = తదుపరి_char;
	--emsg_off;

	ఒకవేళ (curwin->w_cursor.lnum < శోధన_మొదటి_పంక్తి
		|| curwin->w_cursor.lnum > శోధన_చివరి_పంక్తి)
	{
	    // match outside of address range
	    దొరికింది = 0;
	    curwin->w_cursor = is_state->శోధన_ప్రారంభం_2;
	}

	// if interrupted while searching, behave like it failed
	ఒకవేళ (got_int)
	{
	    (శూన్య)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    దొరికింది = 0;
	}
	లేకపోతే ఒకవేళ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    ఒకవేళ (దొరికింది != 0)
	highlight_సరిపోలిక = TRUE;		// highlight position
    లేకపోతే
	highlight_సరిపోలిక = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    మారింది_cline_bef_curs();
    update_topline();

    ఒకవేళ (దొరికింది != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->సరిపోలిక_ప్రారంభం = curwin->w_cursor;
	సమితి_శోధన_సరిపోలిక(&curwin->w_cursor);
	validate_cursor();
	చివర_pos = curwin->w_cursor;
	is_state->సరిపోలిక_చివర = చివర_pos;
	curwin->w_cursor = save_pos;
    }
    లేకపోతే
	చివర_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    ఒకవేళ (!use_చివరి_pat)
    {
	తదుపరి_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	ఒకవేళ (ఖాళీలు_నమూనా(ccline.cmdbuff + skiplen, (size_t)patlen, శోధన_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    సమితి_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = తదుపరి_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    ఒకవేళ (p_ru && curwin->w_స్థితి_ఎత్తు > 0)
	curwin->w_redr_స్థితి = true;

    update_screen(UPD_SOME_VALID);
    highlight_సరిపోలిక = FALSE;
    restore_చివరి_శోధన_నమూనా();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    ఒకవేళ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->శోధన_ప్రారంభం_2;
    లేకపోతే ఒకవేళ (దొరికింది != 0)
	curwin->w_cursor = చివర_pos;

    సందేశం_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    స్థిర పూర్ణాంకం
may_adjust_incsearch_highlighting(
	పూర్ణాంకం			firstc,
	దీర్ఘ			లెక్క,
	incsearch_state_T	*is_state,
	పూర్ణాంకం			c)
{
    పూర్ణాంకం		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ఖాళీ;
    char_u	*searchstr;
    char_u	*strcopy = ఖాళీ;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	స్థానభ్రంశం;
    పూర్ణాంకం		శోధన_flags = శోధన_NOOF;
    పూర్ణాంకం		i;
    పూర్ణాంకం		శోధన_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_చివరి_శోధన_నమూనా();

    ఒకవేళ (!do_incsearch_highlighting(firstc, &శోధన_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_చివరి_శోధన_నమూనా();
	తిరిగి OK;
    }
    ఒకవేళ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_చివరి_శోధన_నమూనా();
	తిరిగి FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (శూన్య)parse_శోధన_నమూనా_స్థానభ్రంశం(&pat, &patlen_s, శోధన_delim,
				    శోధన_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &స్థానభ్రంశం);

    cursor_off();
    బహిర్గతం_flush();
    ఒకవేళ (c == Ctrl_G)
    {
	t = is_state->సరిపోలిక_చివర;
	ఒకవేళ (LT_POS(is_state->సరిపోలిక_ప్రారంభం, is_state->సరిపోలిక_చివర))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (శూన్య)decl(&t);
	శోధన_flags += శోధన_నిలువు;
    }
    లేకపోతే
	t = is_state->సరిపోలిక_ప్రారంభం;
    ఒకవేళ (!p_hls)
	శోధన_flags += శోధన_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ఖాళీ,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, లెక్క, శోధన_flags, RE_శోధన, ఖాళీ);
    --emsg_off;
    ఒకవేళ (dircp != ఖాళీ)
	*dircp = శోధన_delim;
    ఒకవేళ (i)
    {
	pos_T	సరిపోలిక_ప్రారంభం = is_state->సరిపోలిక_ప్రారంభం;
	pos_T	సరిపోలిక_చివర = is_state->సరిపోలిక_చివర;
	దీర్ఘ	off = స్థానభ్రంశం.off;

	is_state->శోధన_ప్రారంభం_2 = సరిపోలిక_ప్రారంభం;
	ఒకవేళ (!స్థానభ్రంశం.పంక్తి && (స్థానభ్రంశం.చివర || off != 0))
	{
	    ఒకవేళ (స్థానభ్రంశం.చివర)
	    {
		is_state->శోధన_ప్రారంభం_2 = సరిపోలిక_చివర;
		(శూన్య)decl(&is_state->శోధన_ప్రారంభం_2);
	    }
	    అయితే (off > 0)
	    {
		ఒకవేళ (incl(&is_state->శోధన_ప్రారంభం_2) == -1)
		    ఆపు;
		--off;
	    }
	    అయితే (off < 0)
	    {
		ఒకవేళ (decl(&is_state->శోధన_ప్రారంభం_2) == -1)
		    ఆపు;
		++off;
	    }
	}
	is_state->సరిపోలిక_చివర = t;
	is_state->సరిపోలిక_ప్రారంభం = t;
	ఒకవేళ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->శోధన_ప్రారంభం_2 = t;
	    (శూన్య)decl(&is_state->శోధన_ప్రారంభం_2);
	}
	లేకపోతే ఒకవేళ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->శోధన_ప్రారంభం_2 = t;
	    (శూన్య)incl(&is_state->శోధన_ప్రారంభం_2);
	}
	ఒకవేళ (LT_POS(t, is_state->శోధన_ప్రారంభం_2) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->శోధన_ప్రారంభం_2 = t;
	    ఒకవేళ (firstc == '?')
		(శూన్య)incl(&is_state->శోధన_ప్రారంభం_2);
	    లేకపోతే
		(శూన్య)decl(&is_state->శోధన_ప్రారంభం_2);
	}

	సమితి_శోధన_సరిపోలిక(&is_state->సరిపోలిక_చివర);
	curwin->w_cursor = is_state->సరిపోలిక_ప్రారంభం;
	మారింది_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_సరిపోలిక = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_సరిపోలిక = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->సరిపోలిక_చివర;
    }
    లేకపోతే
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_చివరి_శోధన_నమూనా();
    తిరిగి FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    స్థిర పూర్ణాంకం
may_కూడు_char_గమ్యం_శోధన(పూర్ణాంకం firstc, పూర్ణాంకం *c, incsearch_state_T *is_state)
{
    పూర్ణాంకం		skiplen, patlen, శోధన_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_చివరి_శోధన_నమూనా();

    ఒకవేళ (!do_incsearch_highlighting(firstc, &శోధన_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_చివరి_శోధన_నమూనా();
	తిరిగి FAIL;
    }
    restore_చివరి_శోధన_నమూనా();

    // Add a character from under the cursor for 'incsearch'.
    ఒకవేళ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->సరిపోలిక_చివర;
	*c = gchar_cursor();
	ఒకవేళ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    ఒకవేళ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    ఒకవేళ (*c == శోధన_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ఖాళీ)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    ఒకవేళ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		పూర్ణాంకం save_c = *c;

		అయితే (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.నిలువు += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    స్థిర పూర్ణాంకం
cmdline_has_arabic(పూర్ణాంకం ప్రారంభం, పూర్ణాంకం len)
{
    పూర్ణాంకం	    j;
    పూర్ణాంకం	    mb_l;
    పూర్ణాంకం	    u8c;
    char_u  *p;
    పూర్ణాంకం	    u8cc[MAX_MCO];

    ఒకవేళ (!enc_utf8)
	తిరిగి FALSE;

    కొరకు (j = ప్రారంభం; j < ప్రారంభం + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, ప్రారంభం + len - j);
	mb_l = utfc_ptr2len_len(p, ప్రారంభం + len - j);
	ఒకవేళ (ARABIC_CHAR(u8c))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

    శూన్య
cmdline_init(శూన్య)
{
    CLEAR_క్షేత్రం(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    స్థిర పూర్ణాంకం
cmdline_handle_ctrl_bsl(పూర్ణాంకం c, పూర్ణాంకం *gotesc)
{
    ++no_mapping;
    ++allow_కీలకాలు;
    c = plain_vgetc();
    --no_mapping;
    --allow_కీలకాలు;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    ఒకవేళ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && కీలకం_Typed)
#నిర్వచితమైతే FEAT_EVAL
		|| cmdline_నక్షత్రం > 0
#అయితే_ముగింపు
		))
    {
	vungetc(c);
	తిరిగి PROCESS_తదుపరి_కీలకం;
    }

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	ఒకవేళ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	లేకపోతే
	    new_cmdpos = ccline.cmdpos;

	c = get_సమాసం_రిజిస్టరు();
	ఒకవేళ (c == '=')
	{
	    char_u	*p = ఖాళీ;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_సమాసం_పంక్తి();
	    --textlock;

	    ఒకవేళ (p != ఖాళీ)
	    {
		పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(p);

		ఒకవేళ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    ఒకవేళ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    లేకపోతే
			ccline.cmdpos = new_cmdpos;

		    కీలకం_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    తిరిగి CMDLINE_మారింది;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	తిరిగి CMDLINE_NOT_మారింది;
    }
#అయితే_ముగింపు

    ఒకవేళ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    తిరిగి GOTO_సాధారణ_బహుళకం;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    స్థిర పూర్ణాంకం
cmdline_wildchar_complete(
	పూర్ణాంకం		c,
	పూర్ణాంకం		escape,
	పూర్ణాంకం		*did_wild_list,
	పూర్ణాంకం		*wim_సూచీ_p,
	expand_T	*xp,
	పూర్ణాంకం		*gotesc,
	పూర్ణాంకం		redraw_if_menu_ఖాళీలు,
	pos_T		*pre_incsearch_pos)
{
    పూర్ణాంకం		wim_సూచీ = *wim_సూచీ_p;
    పూర్ణాంకం		res;
    పూర్ణాంకం		cmdpos_before;
    పూర్ణాంకం		options = WILD_NO_BEEP;
    పూర్ణాంకం		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    పూర్ణాంకం		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    ఒకవేళ (wim_flags[wim_సూచీ] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    ఒకవేళ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	ఒకవేళ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_సూచీ] & WIM_LIST))
	{
	    (శూన్య)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_సూచీ] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	ఒకవేళ (wim_flags[wim_సూచీ] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	లేకపోతే ఒకవేళ (wim_flags[wim_సూచీ] & WIM_నిండిన)
	    res = nextwild(xp, WILD_తదుపరి, options, escape);
	లేకపోతే
	    res = OK;	    // don't insert 'wildchar' now
    }
    లేకపోతే		    // typed p_wc first time
    {
	పూర్ణాంకం wim_longest = (wim_flags[0] & WIM_LONGEST);
	పూర్ణాంకం wim_list = (wim_flags[0] & WIM_LIST);
	పూర్ణాంకం wim_నిండిన = (wim_flags[0] & WIM_నిండిన);

	wim_సూచీ = 0;
	ఒకవేళ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_నమూనా;
	    ఒకవేళ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    ఒకవేళ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    లేకపోతే
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	ఒకవేళ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	లేకపోతే
	{
	    ఒకవేళ (wim_noselect || (wim_list && !wim_నిండిన))
		options |= WILD_NOSELECT;
	    ఒకవేళ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	ఒకవేళ (redraw_if_menu_ఖాళీలు && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	ఒకవేళ (got_int)
	{
	    (శూన్య)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (శూన్య)ExpandOne(xp, ఖాళీ, ఖాళీ, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_సూచీ_p = wim_సూచీ;
	    తిరిగి CMDLINE_మారింది;
	}

	// Display matches
	ఒకవేళ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    ఒకవేళ (wim_longest)
	    {
		పూర్ణాంకం దొరికింది_longest_ఉపసర్గ = (ccline.cmdpos != cmdpos_before);
		పూర్ణాంకం show_menu = p_wmnu && wim_నిండిన;

		ఒకవేళ (wim_list || show_menu)
		    (శూన్య)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		లేకపోతే ఒకవేళ (!దొరికింది_longest_ఉపసర్గ)
		{
		    పూర్ణాంకం wim_list_తదుపరి = (wim_flags[1] & WIM_LIST);
		    పూర్ణాంకం wim_నిండిన_తదుపరి = (wim_flags[1] & WIM_నిండిన);
		    పూర్ణాంకం wim_noselect_తదుపరి = (wim_flags[1] & WIM_NOSELECT);
		    పూర్ణాంకం wim_noinsert_తదుపరి = (wim_flags[1] & WIM_NOINSERT);
		    ఒకవేళ (wim_list_తదుపరి || (p_wmnu && (wim_నిండిన_తదుపరి
				    || wim_noselect_తదుపరి || wim_noinsert_తదుపరి)))
		    {
			ఒకవేళ (wim_నిండిన_తదుపరి && !wim_noselect_తదుపరి && !wim_noinsert_తదుపరి)
			    nextwild(xp, WILD_తదుపరి, options, escape);
			లేకపోతే
			    (శూన్య)showmatches(xp, p_wmnu && (wim_noselect_తదుపరి
					|| wim_noinsert_తదుపరి), wim_list_తదుపరి,
				    p_wmnu ? wim_flags[1] : 0);

			ఒకవేళ (wim_list_తదుపరి)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    లేకపోతే
	    {
		పూర్ణాంకం show_menu = p_wmnu && (wim_నిండిన || wim_noselect
							    || wim_noinsert);

		ఒకవేళ (wim_list || show_menu)
		    (శూన్య)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		లేకపోతే
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    ఒకవేళ (wim_list)
		*did_wild_list = TRUE;
	}
	లేకపోతే ఒకవేళ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    ఒకవేళ (wim_సూచీ < 3)
	++wim_సూచీ;
    ఒకవేళ (c == ESC)
	*gotesc = TRUE;

    *wim_సూచీ_p = wim_సూచీ;
    తిరిగి (res == OK) ? CMDLINE_మారింది : CMDLINE_NOT_మారింది;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    స్థిర పూర్ణాంకం
cmdline_erase_chars(
	పూర్ణాంకం c,
	పూర్ణాంకం indent
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	, incsearch_state_T *isp
#అయితే_ముగింపు
	)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;

    ఒకవేళ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    ఒకవేళ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    ఒకవేళ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_తదుపరి(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    ఒకవేళ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	ఒకవేళ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    ఒకవేళ (c == Ctrl_W)
	    {
		అయితే (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		అయితే (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		ఒకవేళ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	లేకపోతే ఒకవేళ (c == Ctrl_W)
	{
	    అయితే (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    ఒకవేళ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		అయితే (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	లేకపోతే
	    --p;
	ccline.cmdpos = (పూర్ణాంకం)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	అయితే (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	ఒకవేళ (ccline.cmdlen == 0)
	{
	    isp->శోధన_ప్రారంభం_2 = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#అయితే_ముగింపు
	redrawcmd();
    }
    లేకపోతే ఒకవేళ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ఖాళీ && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	ఒకవేళ (exmode_చురుకైన
#నిర్వచితమైతే FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#అయితే_ముగింపు
	   )
	    తిరిగి CMDLINE_NOT_మారింది;

	dealloc_cmdbuff();	// no commandline to return

	ఒకవేళ (!cmd_silent)
	{
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (cmdmsg_rl)
		సందేశం_నిలువు = cmdline_వెడల్పు;
	    లేకపోతే
#అయితే_ముగింపు
		సందేశం_నిలువు = 0;
	    సందేశం_putchar(' ');		// delete ':'
	}
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	isp->శోధన_ప్రారంభం_2 = isp->save_cursor;
#అయితే_ముగింపు
	redraw_cmdline = TRUE;
	తిరిగి GOTO_సాధారణ_బహుళకం;
    }
    తిరిగి CMDLINE_మారింది;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    స్థిర శూన్య
cmdline_toggle_langmap(దీర్ఘ *b_im_ptr)
{
    ఒకవేళ (map_గమ్యం_exists_బహుళకం((char_u *)"", బహుళకం_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= బహుళకం_LANGMAP;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
	im_సమితి_చురుకైన(FALSE);	// Disable input method
#అయితే_ముగింపు
	ఒకవేళ (b_im_ptr != ఖాళీ)
	{
	    ఒకవేళ (State & బహుళకం_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    లేకపోతే
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    లేకపోతే
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	ఒకవేళ ((p_imdisable && b_im_ptr != ఖాళీ)
		? *b_im_ptr == B_IMODE_IM : im_get_స్థితి())
	{
	    im_సమితి_చురుకైన(FALSE);	// Disable input method
	    ఒకవేళ (b_im_ptr != ఖాళీ)
		*b_im_ptr = B_IMODE_NONE;
	}
	లేకపోతే
	{
	    im_సమితి_చురుకైన(TRUE);	// Enable input method
	    ఒకవేళ (b_im_ptr != ఖాళీ)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#అయితే_ముగింపు
    ఒకవేళ (b_im_ptr != ఖాళీ)
    {
	ఒకవేళ (b_im_ptr == &curbuf->b_p_iminsert)
	    సమితి_iminsert_global();
	లేకపోతే
	    సమితి_imsearch_global();
    }
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    స్థితి_redraw_curbuf();
#అయితే_ముగింపు
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    స్థిర పూర్ణాంకం
cmdline_చొప్పించు_reg(పూర్ణాంకం *gotesc వాడని)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;
    పూర్ణాంకం		literally = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		save_new_cmdpos = new_cmdpos;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#అయితే_ముగింపు
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_కీలకాలు;
    i = c = plain_vgetc();	// CTRL-R <char>
    ఒకవేళ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    ఒకవేళ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_కీలకాలు;
#నిర్వచితమైతే FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    ఒకవేళ (c == '=')
    {
	ఒకవేళ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_నక్షత్రం > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	లేకపోతే
	    c = get_సమాసం_రిజిస్టరు();
    }
#అయితే_ముగింపు
    ఒకవేళ (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#నిర్వచితమైతే FEAT_CLIPBOARD
			|| (clip_నక్షత్రం.అందుబాటు && c == '*')
			|| (clip_plus.అందుబాటు && c == '+')
#అయితే_ముగింపు
			;
	cmdline_paste(c, literally, FALSE);

#నిర్వచితమైతే FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	ఒకవేళ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    తిరిగి GOTO_సాధారణ_బహుళకం;
	}
#అయితే_ముగింపు
	కీలకం_Typed = FALSE;	// Don't do p_wc completion.
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    ఒకవేళ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    లేకపోతే
		ccline.cmdpos = new_cmdpos;
	}
#అయితే_ముగింపు
    }
#నిర్వచితమైతే FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#అయితే_ముగింపు

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    తిరిగి literally ? CMDLINE_మారింది : CMDLINE_NOT_మారింది;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    స్థిర శూన్య
cmdline_ఎడమ_కుడి_mouse(పూర్ణాంకం c, పూర్ణాంకం *ignore_drag_release)
{
    ఒకవేళ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    లేకపోతే
	*ignore_drag_release = FALSE;
#నిర్వచితమైతే FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
	ఒకవేళ (!mouse_has(MOUSE_COMMAND))
	    తిరిగి;
#నిర్వచితమైతే FEAT_CLIPBOARD
    ఒకవేళ (mouse_అడ్డువరుస < cmdline_అడ్డువరుస && (clip_నక్షత్రం.అందుబాటు || clip_plus.అందుబాటు))
    {
	పూర్ణాంకం	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	ఒకవేళ (mouse_model_popup() && button == MOUSE_ఎడమ
		&& (mod_ముసుగు & MOD_ముసుగు_జరుపు))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_కుడి;
	    mod_ముసుగు &= ~MOD_ముసుగు_జరుపు;
	}
	clip_modeless(button, is_click, is_drag);
	తిరిగి;
    }
#అయితే_ముగింపు

    సమితి_cmdspos();
    కొరకు (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	పూర్ణాంకం	i;

	i = cmdline_charsize(ccline.cmdpos);
	ఒకవేళ (mouse_అడ్డువరుస <= cmdline_అడ్డువరుస + ccline.cmdspos / cmdline_వెడల్పు
		&& mouse_నిలువు < ccline.cmdspos % cmdline_వెడల్పు + i)
	    ఆపు;
	ఒకవేళ (has_mbyte)
	{
	    // Count ">" for double-wide char that doesn't fit.
	    correct_cmdspos(ccline.cmdpos, i);
	    ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
		    + ccline.cmdpos) - 1;
	}
	ccline.cmdspos += i;
    }
}

/*
 * Handle the Up, Down, Page Up, Page down, CTRL-N and CTRL-P key in the
 * command-line mode. The pressed key is in 'c'.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    స్థిర పూర్ణాంకం
cmdline_browse_history(
	పూర్ణాంకం	c,
	పూర్ణాంకం	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	పూర్ణాంకం	histype,
	పూర్ణాంకం	*hiscnt_p,
	expand_T *xp)
{
    పూర్ణాంకం		orig_hiscnt;
    పూర్ణాంకం		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    పూర్ణాంకం		res;

    ఒకవేళ (get_hislen() == 0 || firstc == NUL)	// no history
	తిరిగి CMDLINE_NOT_మారింది;

    // save current command string so it can be restored later
    ఒకవేళ (lookfor == ఖాళీ)
    {
	ఒకవేళ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ఖాళీ)
	    తిరిగి CMDLINE_NOT_మారింది;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    కొరకు (;;)
    {
	// one step backwards
	ఒకవేళ (c == K_పైకి || c == K_S_పైకి || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    ఒకవేళ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    లేకపోతే ఒకవేళ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    లేకపోతే ఒకవేళ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    లేకపోతే			// at top of list
	    {
		hiscnt = orig_hiscnt;
		ఆపు;
	    }
	}
	లేకపోతే    // one step forwards
	{
	    // on last entry, clear the line
	    ఒకవేళ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		ఆపు;
	    }

	    // not on a history line, nothing to do
	    ఒకవేళ (hiscnt == get_hislen())
		ఆపు;
	    ఒకవేళ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    లేకపోతే
		++hiscnt;
	}
	ఒకవేళ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ఖాళీ)
	{
	    hiscnt = orig_hiscnt;
	    ఆపు;
	}
	ఒకవేళ ((c != K_పైకి && c != K_కిందికి)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    ఆపు;
    }

    ఒకవేళ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	పూర్ణాంకం	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	ఒకవేళ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	లేకపోతే
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	ఒకవేళ (histype == HIST_శోధన
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    పూర్ణాంకం     i;
	    పూర్ణాంకం     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    కొరకు (i = 0; i <= 1; ++i)
	    {
		len = 0;
		కొరకు (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    ఒకవేళ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			ఒకవేళ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    లేకపోతే
		    {
			// Escape new sep, unless it is already
			// escaped.
			ఒకవేళ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    ఒకవేళ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			ఒకవేళ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		ఒకవేళ (i == 0)
		{
		    alloc_cmdbuff(len);
		    ఒకవేళ (ccline.cmdbuff == ఖాళీ)
		    {
			res = GOTO_సాధారణ_బహుళకం;
			వెళ్ళు పూర్తి;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (పూర్ణాంకం)len;
	}
	లేకపోతే
	{
	    alloc_cmdbuff(plen);
	    ఒకవేళ (ccline.cmdbuff == ఖాళీ)
	    {
		res = GOTO_సాధారణ_బహుళకం;
		వెళ్ళు పూర్తి;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (పూర్ణాంకం)plen;
	}

	redrawcmd();
	res = CMDLINE_మారింది;
	వెళ్ళు పూర్తి;
    }
    beep_flush();
    res = CMDLINE_NOT_మారింది;

పూర్తి:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    తిరిగి res;
}

/*
 * Initialize the current command-line info.
 */
    స్థిర పూర్ణాంకం
init_ccline(పూర్ణాంకం firstc, పూర్ణాంకం indent)
{
    ccline.overstrike = FALSE;		    // always start in insert mode

    /*
     * set some variables for redrawcmd()
     */
    ccline.cmdfirstc = (firstc == '@' ? 0 : firstc);
    ccline.cmdindent = (firstc > 0 ? indent : 0);

    // alloc initial ccline.cmdbuff
    alloc_cmdbuff(indent + 50);
    ccline.cmdlen = ccline.cmdpos = 0;
    ఒకవేళ (ccline.cmdbuff == ఖాళీ)
    {
	ccline.cmdbufflen = 0;
	తిరిగి FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_పాఠ్యం_ప్రారంభం_cmdline();

    // autoindent for :insert and :append
    ఒకవేళ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    తిరిగి OK;
}

/*
 * getcmdline() - accept a command line starting with firstc.
 *
 * firstc == ':'	    get ":" command line.
 * firstc == '/' or '?'	    get search pattern
 * firstc == '='	    get expression
 * firstc == '@'	    get text for input() function
 * firstc == '>'	    get text for debug mode
 * firstc == NUL	    get text for :insert command
 * firstc == -1		    like NUL, and break on CTRL-C
 *
 * The line is collected in ccline.cmdbuff, which is reallocated to fit the
 * command line.
 *
 * Careful: getcmdline() can be called recursively!
 *
 * Return pointer to allocated string if there is a commandline, NULL
 * otherwise.
 */
    char_u *
getcmdline(
    పూర్ణాంకం		  firstc,
    దీర్ఘ	  లెక్క,	// only used for incremental search
    పూర్ణాంకం		  indent,	// indent for inside conditionals
    getline_opt_T do_concat వాడని)
{
    తిరిగి getcmdline_int(firstc, లెక్క, indent, TRUE);
}

    స్థిర char_u *
getcmdline_int(
    పూర్ణాంకం		firstc,
    దీర్ఘ	లెక్క వాడని,	// only used for incremental search
    పూర్ణాంకం		indent,		// indent for inside conditionals
    పూర్ణాంకం		clear_ccline)	// clear ccline first
{
    స్థిర పూర్ణాంకం	లోతు = 0;	    // call depth
    పూర్ణాంకం		c = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    పూర్ణాంకం		gotesc = FALSE;		// TRUE when <ESC> just typed
    పూర్ణాంకం		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ఖాళీ;	// string to match
    size_t	lookforlen = 0;
    పూర్ణాంకం		hiscnt;			// current history line in use
    పూర్ణాంకం		histype;		// history type to be used
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    incsearch_state_T	is_state;
#అయితే_ముగింపు
    పూర్ణాంకం		did_wild_list = FALSE;	// did wild_list() recently
    పూర్ణాంకం		wim_సూచీ = 0;		// index in wim_flags[]
    పూర్ణాంకం		res;
    పూర్ణాంకం		save_సందేశం_scroll = సందేశం_scroll;
    పూర్ణాంకం		save_State = State;	// remember State when called
    పూర్ణాంకం		some_కీలకం_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    పూర్ణాంకం		ignore_drag_release = TRUE;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		break_ctrl_c = FALSE;
#అయితే_ముగింపు
    expand_T	xpc;
    దీర్ఘ	*b_im_ptr = ఖాళీ;
    buf_T	*b_im_ptr_buf = ఖాళీ;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    పూర్ణాంకం		did_save_ccline = FALSE;
    పూర్ణాంకం		cmdline_type;
    పూర్ణాంకం		wild_type = 0;
    పూర్ణాంకం		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ఖాళీ;
    పూర్ణాంకం		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++లోతు;

    ఒకవేళ (ccline.cmdbuff != ఖాళీ)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    ఒకవేళ (clear_ccline)
	CLEAR_క్షేత్రం(ccline);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    ఒకవేళ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    init_incsearch_state(&is_state);
#అయితే_ముగింపు

    ఒకవేళ (init_ccline(firstc, indent) != OK)
	వెళ్ళు theend;	// out of memory

    ఒకవేళ (లోతు == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	వెళ్ళు theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    లేకపోతే
	cmdmsg_rl = FALSE;
#అయితే_ముగింపు

    redir_off = TRUE;		// don't redirect the typed command
    ఒకవేళ (!cmd_silent)
    {
	i = సందేశం_scrolled;
	సందేశం_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	సందేశం_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	సమితి_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
    xpc.xp_shell = FALSE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (ccline.ఆదానం_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_నమూనా = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#అయితే_ముగింపు

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    సందేశం_scroll = FALSE;

    State = బహుళకం_CMDLINE;

    ఒకవేళ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	ఒకవేళ (curbuf->b_p_imsearch == B_IMODE_USE_చొప్పించు)
	    b_im_ptr = &curbuf->b_p_iminsert;
	లేకపోతే
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	ఒకవేళ (*b_im_ptr == B_IMODE_LMAP)
	    State |= బహుళకం_LANGMAP;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
	im_సమితి_చురుకైన(*b_im_ptr == B_IMODE_IM);
#అయితే_ముగింపు
    }
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    లేకపోతే ఒకవేళ (p_imcmdline)
	im_సమితి_చురుకైన(TRUE);
#అయితే_ముగింపు

    setmouse();
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (!debug_బహుళకం)
	may_trigger_modechanged();
#అయితే_ముగింపు

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#నిర్వచితమైతే FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#అయితే_ముగింపు

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    ఒకవేళ (did_emsg)
	redrawcmd();

#నిర్వచితమైతే FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    ఒకవేళ (!cmd_silent && సందేశం_scrolled == 0)
    {
	పూర్ణాంకం	దొరికింది_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_స్థితి = true;
		దొరికింది_one = TRUE;
	    }

	ఒకవేళ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    దొరికింది_one = TRUE;
	}

	ఒకవేళ (దొరికింది_one)
	    redraw_statuslines();
    }
#అయితే_ముగింపు

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం	trigger_cmdlinechanged = TRUE;
	పూర్ణాంకం	చివర_wildmenu;
	పూర్ణాంకం	prev_cmdpos = ccline.cmdpos;
	పూర్ణాంకం	skip_pum_redraw = FALSE;

	VIM_CLEAR(prev_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#నిర్వచితమైతే USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#అయితే_ముగింపు
	quit_మరిన్ని = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	ఒకవేళ (ex_సాధారణ_busy == 0 && stuff_ఖాళీలు() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_కీలకం_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	ఒకవేళ (ccline.cmdbuff != ఖాళీ)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    ఒకవేళ (prev_cmdbuff == ఖాళీ)
		వెళ్ళు returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	ఒకవేళ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	చేయి
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = సురక్షిత_vgetc();
	} అయితే (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	ఒకవేళ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    ఒకవేళ (cmdline_pum_చురుకైన())
		cmdline_pum_remove(&ccline, FALSE);
	    (శూన్య)ExpandOne(&xpc, ఖాళీ, ఖాళీ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_సూచీ = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	ఒకవేళ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    కొనసాగు;
	}

	ఒకవేళ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    పూర్ణాంకం	    clen = ccline.cmdlen;
	    పూర్ణాంకం	    cc_లెక్క = aucmd_cmdline_మారింది_లెక్క;

	    ఒకవేళ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		ఒకవేళ (clen == ccline.cmdlen
				    || cc_లెక్క != aucmd_cmdline_మారింది_లెక్క)
		    trigger_cmdlinechanged = FALSE;
		వెళ్ళు cmdline_మారింది;
	    }
	}

	ఒకవేళ (కీలకం_Typed)
	{
	    some_కీలకం_typed = TRUE;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (cmd_hkmap)
		c = hkmap(c);
	    ఒకవేళ (cmdmsg_rl && !కీలకం_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		ఎంపిక (c)
		{
		    స్థితి K_కుడి:   c = K_ఎడమ; ఆపు;
		    స్థితి K_S_కుడి: c = K_S_ఎడమ; ఆపు;
		    స్థితి K_C_కుడి: c = K_C_ఎడమ; ఆపు;
		    స్థితి K_ఎడమ:    c = K_కుడి; ఆపు;
		    స్థితి K_S_ఎడమ:  c = K_S_కుడి; ఆపు;
		    స్థితి K_C_ఎడమ:  c = K_C_కుడి; ఆపు;
		}
	    }
#అయితే_ముగింపు
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	ఒకవేళ ((c == Ctrl_C
#నిర్వచితమైతే UNIX
		|| c == intr_char
#అయితే_ముగింపు
				)
#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_చురుకైన)
#అయితే_ముగింపు
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	ఒకవేళ (lookfor != ఖాళీ
		&& c != K_S_కిందికి && c != K_S_పైకి
		&& c != K_కిందికి && c != K_పైకి
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ఎడమ && c != K_కుడి
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	ఒకవేళ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	ఒకవేళ (p_wmnu)
	    c = wildmenu_translate_కీలకం(&ccline, c, &xpc, did_wild_list);

	పూర్ణాంకం కీలకం_is_wc = (c == p_wc && కీలకం_Typed) || c == p_wcm;
	ఒకవేళ ((cmdline_pum_చురుకైన() || wild_menu_showing || did_wild_list)
		&& !కీలకం_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    ఒకవేళ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		ఒకవేళ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    ఆపు;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	ఒకవేళ (కీలకం_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#నిర్వచితమైతే UNIX
		    || c == intr_char
#అయితే_ముగింపు
		    || c == Ctrl_C))
	{
#నిర్వచితమైతే FEAT_EVAL
	    సమితి_vim_var_char(c);  // Set v:char
#అయితే_ముగింపు
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#ఒకవేళ defined(FEAT_శోధన_EXTRA)
	    ఒకవేళ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		సమితి_no_hlsearch(TRUE);
#అయితే_ముగింపు
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	చివర_wildmenu = (!కీలకం_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	చివర_wildmenu = చివర_wildmenu && (!cmdline_pum_చురుకైన() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_పైకి || c == K_S_కిందికి)
				 && (mod_ముసుగు & MOD_ముసుగు_CTRL))));

	// free expanded names when finished walking through matches
	ఒకవేళ (చివర_wildmenu)
	{
	    ఒకవేళ (cmdline_pum_చురుకైన())
	    {
		skip_pum_redraw = skip_pum_redraw && !కీలకం_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    ఒకవేళ (xpc.xp_numfiles != -1)
		(శూన్య)ExpandOne(&xpc, ఖాళీ, ఖాళీ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    ఒకవేళ (!p_wmnu || (c != K_పైకి && c != K_కిందికి))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_సూచీ = 0;
	    wildmenu_cleanup(&ccline);
	}

	ఒకవేళ (p_wmnu)
	    c = wildmenu_process_కీలకం(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	ఒకవేళ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    ఒకవేళ (res == CMDLINE_మారింది)
		వెళ్ళు cmdline_మారింది;
	    లేకపోతే ఒకవేళ (res == CMDLINE_NOT_మారింది)
		వెళ్ళు cmdline_not_మారింది;
	    లేకపోతే ఒకవేళ (res == GOTO_సాధారణ_బహుళకం)
		వెళ్ళు returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	ఒకవేళ (c == cedit_కీలకం || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    ఒకవేళ ((c == K_CMDWIN || ex_సాధారణ_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_కీలకం_typed = TRUE;
	    }
	}
#నిర్వచితమైతే FEAT_DIGRAPHS
	లేకపోతే
	    c = do_digraph(c);
#అయితే_ముగింపు

	ఒకవేళ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!కీలకం_Typed || vim_strchr(p_cpo, CPO_ESC) != ఖాళీ)))
	{
	    // In Ex mode a backslash escapes a newline.
	    ఒకవేళ (exmode_చురుకైన
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		ఒకవేళ (c == K_KENTER)
		    c = '\n';
	    }
	    లేకపోతే
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		ఒకవేళ (ccheck_abbr(c + ABBR_OFF))
		    వెళ్ళు cmdline_మారింది;
		ఒకవేళ (!cmd_silent)
		{
		    windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off);
		    బహిర్గతం_flush();
		}
		ఆపు;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	ఒకవేళ ((c == p_wc && !gotesc && కీలకం_Typed) || c == p_wcm || c == K_WILD)
	{
	    ఒకవేళ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_సూచీ, &xpc, &gotesc, c == K_WILD,
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		    &is_state.శోధన_ప్రారంభం_2
#లేకపోతే
		    ఖాళీ
#అయితే_ముగింపు
		    );
	    ఒకవేళ (c == K_WILD)
		--emsg_silent;
	    ఒకవేళ (res == CMDLINE_మారింది)
		వెళ్ళు cmdline_మారింది;
	    ఒకవేళ (c == K_WILD)
		వెళ్ళు cmdline_not_మారింది;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	ఒకవేళ (c == K_S_TAB && కీలకం_Typed)
	{
	    ఒకవేళ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		ఒకవేళ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_సూచీ] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    పూర్ణాంకం list = wim_flags[wim_సూచీ] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		ఒకవేళ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    వెళ్ళు cmdline_మారింది;
	    }
	}

	ఒకవేళ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	ఒకవేళ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	    // Apply search highlighting
	    ఒకవేళ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    ఒకవేళ (కీలకం_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, లెక్క, &is_state);
#అయితే_ముగింపు
	    wild_type = 0;
	    వెళ్ళు cmdline_not_మారింది;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	ఎంపిక (c)
	{
	స్థితి K_BS:
	స్థితి Ctrl_H:
	స్థితి K_DEL:
	స్థితి K_KDEL:
	స్థితి Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		    , &is_state
#అయితే_ముగింపు
		    );
	    ఒకవేళ (res == CMDLINE_NOT_మారింది)
		వెళ్ళు cmdline_not_మారింది;
	    లేకపోతే ఒకవేళ (res == GOTO_సాధారణ_బహుళకం)
		వెళ్ళు returncmd;		// back to cmd mode
	    వెళ్ళు cmdline_మారింది;

	స్థితి K_INS:
	స్థితి K_KINS:
		ccline.overstrike = !ccline.overstrike;
#నిర్వచితమైతే CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#అయితే_ముగింపు
		may_trigger_modechanged();
		స్థితి_redraw_curbuf();
		redraw_statuslines();
		వెళ్ళు cmdline_not_మారింది;

	స్థితి Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ఖాళీ);
		వెళ్ళు cmdline_not_మారింది;

//	case '@':   only in very old vi
	స్థితి Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		అయితే (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		ఒకవేళ (ccline.cmdlen == 0)
		    is_state.శోధన_ప్రారంభం_2 = is_state.save_cursor;
#అయితే_ముగింపు
		redrawcmd();
		వెళ్ళు cmdline_మారింది;

#నిర్వచితమైతే FEAT_CLIPBOARD
	స్థితి Ctrl_Y:
		// Copy the modeless selection, if there is one.
		ఒకవేళ (clip_నక్షత్రం.state != SELECT_CLEARED)
		{
		    ఒకవేళ (clip_నక్షత్రం.state == SELECT_పూర్తి)
			clip_copy_modeless_selection(TRUE);
		    వెళ్ళు cmdline_not_మారింది;
		}
		ఆపు;
#అయితే_ముగింపు

	స్థితి ESC:	// get here if p_wc != ESC or when ESC typed twice
	స్థితి Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		ఒకవేళ (exmode_చురుకైన
			       && (ex_సాధారణ_busy == 0 || typebuf.tb_len > 0))
		    వెళ్ళు cmdline_not_మారింది;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		వెళ్ళు returncmd;		// back to cmd mode

	స్థితి Ctrl_R:			// insert register
		res = cmdline_చొప్పించు_reg(&gotesc);
		ఒకవేళ (res == GOTO_సాధారణ_బహుళకం)
		    వెళ్ళు returncmd;
		ఒకవేళ (res == CMDLINE_మారింది)
		    వెళ్ళు cmdline_మారింది;
		వెళ్ళు cmdline_not_మారింది;

	స్థితి Ctrl_D:
		ఒకవేళ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    ఆపు;	// Use ^D as normal char instead

		redrawcmd();
		కొనసాగు;	// don't do incremental search now

	స్థితి K_కుడి:
	స్థితి K_S_కుడి:
	స్థితి K_C_కుడి:
		చేయి
		{
		    ఒకవేళ (ccline.cmdpos >= ccline.cmdlen)
			ఆపు;
		    i = cmdline_charsize(ccline.cmdpos);
		    ఒకవేళ (కీలకం_Typed && ccline.cmdspos + i >= cmdline_వెడల్పు * అడ్డువరుసలు)
			ఆపు;
		    ccline.cmdspos += i;
		    ఒకవేళ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    లేకపోతే
			++ccline.cmdpos;
		}
		అయితే ((c == K_S_కుడి || c == K_C_కుడి
			       || (mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		ఒకవేళ (has_mbyte)
		    సమితి_cmdspos_cursor();
		వెళ్ళు cmdline_not_మారింది;

	స్థితి K_ఎడమ:
	స్థితి K_S_ఎడమ:
	స్థితి K_C_ఎడమ:
		ఒకవేళ (ccline.cmdpos == 0)
		    వెళ్ళు cmdline_not_మారింది;
		చేయి
		{
		    --ccline.cmdpos;
		    ఒకవేళ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_తల_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		అయితే (ccline.cmdpos > 0
			&& (c == K_S_ఎడమ || c == K_C_ఎడమ
			       || (mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		ఒకవేళ (has_mbyte)
		    సమితి_cmdspos_cursor();
		వెళ్ళు cmdline_not_మారింది;

	స్థితి K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		వెళ్ళు cmdline_not_మారింది;

#నిర్వచితమైతే FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	స్థితి K_F4:
	    ఒకవేళ (mod_ముసుగు == MOD_ముసుగు_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		వెళ్ళు cmdline_not_మారింది;
	    }
	    ఆపు;
#అయితే_ముగింపు

	స్థితి K_MIDDLEDRAG:
	స్థితి K_MIDDLERELEASE:
		వెళ్ళు cmdline_not_మారింది;	// Ignore mouse

	స్థితి K_MIDDLEMOUSE:
#నిర్వచితమైతే FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
		    ఒకవేళ (!mouse_has(MOUSE_COMMAND))
			వెళ్ళు cmdline_not_మారింది;   // Ignore mouse
#నిర్వచితమైతే FEAT_CLIPBOARD
		ఒకవేళ (clip_నక్షత్రం.అందుబాటు)
		    cmdline_paste('*', TRUE, TRUE);
		లేకపోతే
#అయితే_ముగింపు
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		వెళ్ళు cmdline_మారింది;

#నిర్వచితమైతే FEAT_DND
	స్థితి K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		వెళ్ళు cmdline_మారింది;
#అయితే_ముగింపు

	స్థితి K_LEFTDRAG:
	స్థితి K_LEFTRELEASE:
	స్థితి K_RIGHTDRAG:
	స్థితి K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ఒకవేళ (ignore_drag_release)
		    వెళ్ళు cmdline_not_మారింది;
		// FALLTHROUGH
	స్థితి K_LEFTMOUSE:
	స్థితి K_RIGHTMOUSE:
		cmdline_ఎడమ_కుడి_mouse(c, &ignore_drag_release);
		వెళ్ళు cmdline_not_మారింది;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	స్థితి K_MOUSEDOWN:
	స్థితి K_MOUSEUP:
	స్థితి K_MOUSELEFT:
	స్థితి K_MOUSERIGHT:
#నిర్వచితమైతే FEAT_PROP_POPUP
		ఒకవేళ (cmdline_pum_చురుకైన())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_కిందికి
				      : c == K_MOUSEUP ? MSCR_పైకి
				      : c == K_MOUSELEFT ? MSCR_ఎడమ
				      : MSCR_కుడి);
#అయితే_ముగింపు
		వెళ్ళు cmdline_not_మారింది;

	// Alternate buttons ignored here
	స్థితి K_X1MOUSE:
	స్థితి K_X1DRAG:
	స్థితి K_X1RELEASE:
	స్థితి K_X2MOUSE:
	స్థితి K_X2DRAG:
	స్థితి K_X2RELEASE:
	స్థితి K_MOUSEMOVE:
		వెళ్ళు cmdline_not_మారింది;

#నిర్వచితమైతే FEAT_GUI
	స్థితి K_LEFTMOUSE_NM:	// mousefocus click, ignored
	స్థితి K_LEFTRELEASE_NM:
		వెళ్ళు cmdline_not_మారింది;

	స్థితి K_VER_SCROLLBAR:
		ఒకవేళ (సందేశం_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		వెళ్ళు cmdline_not_మారింది;

	స్థితి K_HOR_SCROLLBAR:
		ఒకవేళ (సందేశం_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_విలువ);
		    redrawcmd();
		}
		వెళ్ళు cmdline_not_మారింది;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
	స్థితి K_TABLINE:
	స్థితి K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		ఒకవేళ (gui_use_tabline())
		    gui_mch_సమితి_curtab(tabpage_సూచీ(curtab));
		వెళ్ళు cmdline_not_మారింది;
#అయితే_ముగింపు

	స్థితి K_SELECT:	    // end of Select mode mapping - ignore
		వెళ్ళు cmdline_not_మారింది;

	స్థితి Ctrl_B:	    // begin of command line
	స్థితి K_HOME:
	స్థితి K_KHOME:
	స్థితి K_S_HOME:
	స్థితి K_C_HOME:
		ccline.cmdpos = 0;
		సమితి_cmdspos();
		వెళ్ళు cmdline_not_మారింది;

	స్థితి Ctrl_E:	    // end of command line
	స్థితి K_చివర:
	స్థితి K_KEND:
	స్థితి K_S_చివర:
	స్థితి K_C_చివర:
		ccline.cmdpos = ccline.cmdlen;
		సమితి_cmdspos_cursor();
		వెళ్ళు cmdline_not_మారింది;

	స్థితి Ctrl_A:	    // all matches
		ఒకవేళ (cmdline_pum_చురుకైన())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		ఒకవేళ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    ఆపు;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		వెళ్ళు cmdline_మారింది;

	స్థితి Ctrl_L:
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		ఒకవేళ (may_కూడు_char_గమ్యం_శోధన(firstc, &c, &is_state) == OK)
		    వెళ్ళు cmdline_not_మారింది;
#అయితే_ముగింపు

		// completion: longest common part
		ఒకవేళ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    ఆపు;
		వెళ్ళు cmdline_మారింది;

	స్థితి Ctrl_N:	    // next match
	స్థితి Ctrl_P:	    // previous match
#నిర్వచితమైతే FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		ఒకవేళ (cmdline_pum_చురుకైన() && (mod_ముసుగు & MOD_ముసుగు_జరుపు))
		{
		    ఒకవేళ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    వెళ్ళు cmdline_not_మారింది;
		}
#అయితే_ముగింపు
		ఒకవేళ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_తదుపరి;
		    ఒకవేళ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ఆపు;
		    వెళ్ళు cmdline_మారింది;
		}
		// FALLTHROUGH
	స్థితి K_పైకి:
	స్థితి K_కిందికి:
	స్థితి K_S_పైకి:
	స్థితి K_S_కిందికి:
	స్థితి K_PAGEUP:
	స్థితి K_KPAGEUP:
	స్థితి K_PAGEDOWN:
	స్థితి K_KPAGEDOWN:
#నిర్వచితమైతే FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		ఒకవేళ (cmdline_pum_చురుకైన()
			&& ((c == K_S_పైకి || c == K_S_కిందికి)
			    ? (mod_ముసుగు & MOD_ముసుగు_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_ముసుగు & MOD_ముసుగు_CTRL)
				&& (mod_ముసుగు & MOD_ముసుగు_జరుపు))))
		{
		    పూర్ణాంకం	పైకి = c == K_S_పైకి || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    ఒకవేళ (popup_scroll_info(పైకి ? -1 : 1,
					     c != K_S_పైకి && c != K_S_కిందికి))
			cmdline_pum_display();
		    వెళ్ళు cmdline_not_మారింది;
		}
#అయితే_ముగింపు
		ఒకవేళ (cmdline_pum_చురుకైన()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    ఒకవేళ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    ఒకవేళ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ఆపు;
		    వెళ్ళు cmdline_మారింది;
		}
		లేకపోతే
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    ఒకవేళ (res == CMDLINE_మారింది)
		    {
			did_hist_navigate = TRUE;
			వెళ్ళు cmdline_మారింది;
		    }
		    లేకపోతే ఒకవేళ (res == GOTO_సాధారణ_బహుళకం)
			వెళ్ళు returncmd;
		}
		వెళ్ళు cmdline_not_మారింది;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	స్థితి Ctrl_G:	    // next match
	స్థితి Ctrl_T:	    // previous match
		ఒకవేళ (may_adjust_incsearch_highlighting(
					  firstc, లెక్క, &is_state, c) == FAIL)
		    వెళ్ళు cmdline_not_మారింది;
		ఆపు;
#అయితే_ముగింపు

	స్థితి Ctrl_V:
	స్థితి Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_ముసుగు & MOD_ముసుగు_జరుపు);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    ఒకవేళ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			సందేశం_putchar(' ');
			cursorcmd();
		    }
		}

		ఆపు;

#నిర్వచితమైతే FEAT_DIGRAPHS
	స్థితి Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		ఒకవేళ (c != NUL)
		    ఆపు;

		redrawcmd();
		వెళ్ళు cmdline_not_మారింది;
#అయితే_ముగింపు // FEAT_DIGRAPHS

#నిర్వచితమైతే FEAT_RIGHTLEFT
	స్థితి Ctrl__:	    // CTRL-_: switch language mode
		ఒకవేళ (!p_ari)
		    ఆపు;
		cmd_hkmap = !cmd_hkmap;
		వెళ్ళు cmdline_not_మారింది;
#అయితే_ముగింపు

	స్థితి K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ఖాళీ);
		వెళ్ళు cmdline_మారింది;

	అప్రమేయ:
#నిర్వచితమైతే UNIX
		ఒకవేళ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    వెళ్ళు returncmd;	// back to Normal mode
		}
#అయితే_ముగింపు
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		ఒకవేళ (!IS_SPECIAL(c))
		    mod_ముసుగు = 0x0;
		ఆపు;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	ఒకవేళ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    వెళ్ళు cmdline_మారింది;

	/*
	 * put the character in the command line
	 */
	ఒకవేళ (IS_SPECIAL(c) || mod_ముసుగు != 0)
	    put_on_cmdline(get_special_కీలకం_name(c, mod_ముసుగు), -1, TRUE);
	లేకపోతే
	{
	    ఒకవేళ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    లేకపోతే
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	వెళ్ళు cmdline_మారింది;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_మారింది:
	// Trigger CursorMovedC autocommands.
	ఒకవేళ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	ఒకవేళ (!is_state.incsearch_postponed)
	    కొనసాగు;
#అయితే_ముగింపు

cmdline_మారింది:
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	// If the window changed incremental search state is not valid.
	ఒకవేళ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	ఒకవేళ (xpc.xp_context == EXPAND_NOTHING && (కీలకం_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, లెక్క, &is_state);
#అయితే_ముగింపు
	// Trigger CmdlineChanged autocommands.
	ఒకవేళ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ఖాళీ &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    ఒకవేళ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#అయితే_ముగింపు
	}

	// Trigger CursorMovedC autocommands.
	ఒకవేళ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    ఒకవేళ (vpeekc() == NUL)
		redrawcmd();
#అయితే_ముగింపు
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    ఒకవేళ (!event_cmdlineleavepre_triggered)
    {
#నిర్వచితమైతే FEAT_EVAL
	సమితి_vim_var_char(c);  // Set v:char
#అయితే_ముగింపు
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#నిర్వచితమైతే FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#అయితే_ముగింపు

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    ఒకవేళ (cmdline_pum_చురుకైన())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_సూచీ = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ఖాళీ;
    clear_cmdline_orig();

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    ముగింపు_incsearch_highlighting(gotesc, &is_state, FALSE);
#అయితే_ముగింపు

    ఒకవేళ (ccline.cmdbuff != ఖాళీ)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ఒకవేళ (ccline.cmdlen && firstc != NUL
		&& (some_కీలకం_typed || histype == HIST_శోధన))
	{
	    కూడు_గమ్యం_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_శోధన ? firstc : NUL);
	    ఒకవేళ (firstc == ':')
	    {
		vim_free(new_చివరి_cmdline);
		new_చివరి_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	ఒకవేళ (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    సందేశం_check();
    సందేశం_scroll = save_సందేశం_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    ఒకవేళ (some_కీలకం_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_char(c);  // Set v:char
#అయితే_ముగింపు
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (!debug_బహుళకం)
	may_trigger_modechanged();
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    ఒకవేళ (b_im_ptr != ఖాళీ && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_స్థితి(b_im_ptr);
    im_సమితి_చురుకైన(FALSE);
#అయితే_ముగింపు
    setmouse();
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
    sb_పాఠ్యం_చివర_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--లోతు;
	ఒకవేళ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	లేకపోతే
	    ccline.cmdbuff = ఖాళీ;

	vim_free(prev_cmdbuff);
	తిరిగి p;
    }
}

#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    పూర్ణాంకం		firstc,
    char_u	*prompt,	// command line prompt
    పూర్ణాంకం		attr,		// attributes for prompt
    పూర్ణాంకం		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    పూర్ణాంకం			did_save_ccline = FALSE;
    పూర్ణాంకం			సందేశం_నిలువు_save = సందేశం_నిలువు;
    పూర్ణాంకం			సందేశం_silent_save = సందేశం_silent;

    ఒకవేళ (ccline.cmdbuff != ఖాళీ)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_క్షేత్రం(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.ఆదానం_fn = (firstc == '@');
# endif
    సందేశం_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    ఒకవేళ (did_save_ccline)
	restore_cmdline(&save_ccline);

    సందేశం_silent = సందేశం_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    ఒకవేళ (ccline.cmdbuff != ఖాళీ)
	సందేశం_నిలువు = సందేశం_నిలువు_save;

    తిరిగి s;
}
#అయితే_ముగింపు

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    పూర్ణాంకం
check_opt_wim(శూన్య)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		idx = 0;

    కొరకు (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    కొరకు (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	కొరకు (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	ఒకవేళ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    తిరిగి FAIL;
	ఒకవేళ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	లేకపోతే ఒకవేళ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_నిండిన;
	లేకపోతే ఒకవేళ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	లేకపోతే ఒకవేళ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	లేకపోతే ఒకవేళ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	లేకపోతే ఒకవేళ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	లేకపోతే
	    తిరిగి FAIL;
	p += i;
	ఒకవేళ (*p == NUL)
	    ఆపు;
	ఒకవేళ (*p == ',')
	{
	    ఒకవేళ (idx == 3)
		తిరిగి FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    అయితే (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    కొరకు (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    తిరిగి OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    పూర్ణాంకం
పాఠ్యం_locked(శూన్య)
{
    ఒకవేళ (cmdwin_type != 0)
	తిరిగి TRUE;
    తిరిగి textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    శూన్య
పాఠ్యం_locked_సందేశం(శూన్య)
{
    emsg(_(get_పాఠ్యం_locked_సందేశం()));
}

    అక్షరం *
get_పాఠ్యం_locked_సందేశం(శూన్య)
{
    ఒకవేళ (cmdwin_type != 0)
	తిరిగి e_invalid_లోపల_cmdline_కిటికీ;
    తిరిగి e_not_allowed_గమ్యం_change_పాఠ్యం_or_change_కిటికీ;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    పూర్ణాంకం
పాఠ్యం_or_buf_locked(శూన్య)
{
    ఒకవేళ (పాఠ్యం_locked())
    {
	పాఠ్యం_locked_సందేశం();
	తిరిగి TRUE;
    }
    తిరిగి curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    పూర్ణాంకం
curbuf_locked(శూన్య)
{
    ఒకవేళ (curbuf_తాళం > 0)
    {
	emsg(_(e_not_allowed_గమ్యం_edit_another_బఫరు_now));
	తిరిగి TRUE;
    }
    తిరిగి allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    పూర్ణాంకం
allbuf_locked(శూన్య)
{
    ఒకవేళ (allbuf_తాళం > 0)
    {
	emsg(_(e_not_allowed_గమ్యం_change_బఫరు_information_now));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
cmdline_charsize(పూర్ణాంకం idx)
{
#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ఒకవేళ (cmdline_నక్షత్రం > 0)	    // showing '*', always 1 position
	తిరిగి 1;
#అయితే_ముగింపు
    తిరిగి ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    స్థిర శూన్య
సమితి_cmdspos(శూన్య)
{
    ఒకవేళ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    లేకపోతే
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    స్థిర శూన్య
సమితి_cmdspos_cursor(శూన్య)
{
    పూర్ణాంకం		i, m, c;

    సమితి_cmdspos();
    ఒకవేళ (కీలకం_Typed)
    {
	m = cmdline_వెడల్పు * అడ్డువరుసలు;
	ఒకవేళ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    లేకపోతే
	m = MAXCOL;
    కొరకు (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	ఒకవేళ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	ఒకవేళ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    ఆపు;
	}
	ఒకవేళ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    స్థిర శూన్య
correct_cmdspos(పూర్ణాంకం idx, పూర్ణాంకం cells)
{
    ఒకవేళ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_వెడల్పు + cells > cmdline_వెడల్పు)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    పూర్ణాంకం		c,		// normally ':', NUL for ":append"
    శూన్య	*cookie వాడని,
    పూర్ణాంకం		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    ఒకవేళ (exec_from_reg && vpeekc() == ':')
	(శూన్య)vgetc();
    తిరిగి getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    పూర్ణాంకం		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    శూన్య	*cookie వాడని,
    పూర్ణాంకం		indent,		// indent for inside conditionals
    getline_opt_T options వాడని)
{
    garray_T	పంక్తి_ga;
    char_u	*pend;
    పూర్ణాంకం		startcol = 0;
    పూర్ణాంకం		c1 = 0;
    పూర్ణాంకం		escaped = FALSE;	// CTRL-V typed
    పూర్ణాంకం		vcol = 0;
    char_u	*p;
    పూర్ణాంకం		prev_char;
    పూర్ణాంకం		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    ఒకవేళ ((సందేశం_నిలువు || సందేశం_didout) && promptc != '?')
	సందేశం_putchar('\n');
    ఒకవేళ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	ఒకవేళ (p_prompt)
	    సందేశం_putchar(':');
	అయితే (indent-- > 0)
	    సందేశం_putchar(' ');
	startcol = సందేశం_నిలువు;
    }

    ga_init2(&పంక్తి_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    ఒకవేళ (promptc <= 0)
    {
	vcol = indent;
	అయితే (indent >= 8)
	{
	    ga_append(&పంక్తి_ga, TAB);
	    సందేశం_puts("        ");
	    indent -= 8;
	}
	అయితే (indent-- > 0)
	{
	    ga_append(&పంక్తి_ga, ' ');
	    సందేశం_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_కీలకాలు;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    అయితే (!got_int)
    {
	దీర్ఘ    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	ఒకవేళ (ga_grow(&పంక్తి_ga, 40) == FAIL)
	    ఆపు;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	ఒకవేళ (ex_సాధారణ_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	లేకపోతే
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	ఒకవేళ (got_int)
	{
	    సందేశం_putchar('\n');
	    ఆపు;
	}

	ఒకవేళ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &పంక్తి_ga);
	    వెళ్ళు redraw;
	}

	ఒకవేళ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    ఒకవేళ (c1 == '\r')
		c1 = '\n';

	    ఒకవేళ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		ఒకవేళ (పంక్తి_ga.ga_len > 0)
		{
		    ఒకవేళ (has_mbyte)
		    {
			p = (char_u *)పంక్తి_ga.ga_దత్తాంశం;
			p[పంక్తి_ga.ga_len] = NUL;
			len = (*mb_తల_off)(p, p + పంక్తి_ga.ga_len - 1) + 1;
			పంక్తి_ga.ga_len -= len;
		    }
		    లేకపోతే
			--పంక్తి_ga.ga_len;
		    వెళ్ళు redraw;
		}
		కొనసాగు;
	    }

	    ఒకవేళ (c1 == Ctrl_U)
	    {
		సందేశం_నిలువు = startcol;
		సందేశం_clr_eos();
		పంక్తి_ga.ga_len = 0;
		వెళ్ళు redraw;
	    }

	    ఒకవేళ (c1 == Ctrl_T)
	    {
		sw = get_sw_విలువ(curbuf);
		p = (char_u *)పంక్తి_ga.ga_దత్తాంశం;
		p[పంక్తి_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
కూడు_indent:
		అయితే (get_indent_str(p, 8, FALSE) < indent)
		{
		    (శూన్య)ga_grow(&పంక్తి_ga, 2);  // one more for the NUL
		    p = (char_u *)పంక్తి_ga.ga_దత్తాంశం;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, పంక్తి_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++పంక్తి_ga.ga_len;
		}
redraw:
		// redraw the line
		సందేశం_నిలువు = startcol;
		vcol = 0;
		p = (char_u *)పంక్తి_ga.ga_దత్తాంశం;
		p[పంక్తి_ga.ga_len] = NUL;
		అయితే (p < (char_u *)పంక్తి_ga.ga_దత్తాంశం + పంక్తి_ga.ga_len)
		{
		    ఒకవేళ (*p == TAB)
		    {
			చేయి
			    సందేశం_putchar(' ');
			అయితే (++vcol % 8);
			++p;
		    }
		    లేకపోతే
		    {
			len = mb_ptr2len(p);
			సందేశం_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		సందేశం_clr_eos();
		windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
		కొనసాగు;
	    }

	    ఒకవేళ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)పంక్తి_ga.ga_దత్తాంశం;
		ఒకవేళ (prev_char == '0' || prev_char == '^')
		{
		    ఒకవేళ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--పంక్తి_ga.ga_len] = NUL;
		}
		లేకపోతే
		{
		    p[పంక్తి_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    ఒకవేళ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_విలువ(curbuf);
		    }
		}
		అయితే (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, పంక్తి_ga.ga_len - (s - p) + 1);
		    --పంక్తి_ga.ga_len;
		}
		వెళ్ళు కూడు_indent;
	    }

	    ఒకవేళ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		కొనసాగు;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    ఒకవేళ (IS_SPECIAL(c1))
		కొనసాగు;
	}

	ఒకవేళ (IS_SPECIAL(c1))
	    c1 = '?';
	ఒకవేళ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)పంక్తి_ga.ga_దత్తాంశం + పంక్తి_ga.ga_len);
	లేకపోతే
	{
	    len = 1;
	    ((char_u *)పంక్తి_ga.ga_దత్తాంశం)[పంక్తి_ga.ga_len] = c1;
	}
	ఒకవేళ (c1 == '\n')
	    సందేశం_putchar('\n');
	లేకపోతే ఒకవేళ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    చేయి
		సందేశం_putchar(' ');
	    అయితే (++vcol % 8);
	}
	లేకపోతే
	{
	    సందేశం_outtrans_len(
		     ((char_u *)పంక్తి_ga.ga_దత్తాంశం) + పంక్తి_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	పంక్తి_ga.ga_len += len;
	escaped = FALSE;

	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
	pend = (char_u *)(పంక్తి_ga.ga_దత్తాంశం) + పంక్తి_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	ఒకవేళ (పంక్తి_ga.ga_len > 0 && pend[-1] == '\n'
		&& (పంక్తి_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --పంక్తి_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    ఆపు;
	}
    }

    --no_mapping;
    --allow_కీలకాలు;

    // make following messages go to the next line
    సందేశం_didout = FALSE;
    సందేశం_నిలువు = 0;
    ఒకవేళ (సందేశం_అడ్డువరుస < అడ్డువరుసలు - 1)
	++సందేశం_అడ్డువరుస;
    emsg_on_display = FALSE;		// don't want ui_delay()

    ఒకవేళ (got_int)
	ga_clear(&పంక్తి_ga);

    తిరిగి (char_u *)పంక్తి_ga.ga_దత్తాంశం;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    పూర్ణాంకం
cmdline_overstrike(శూన్య)
{
    తిరిగి ccline.overstrike;
}

#ఒకవేళ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    పూర్ణాంకం
cmdline_at_చివర(శూన్య)
{
    తిరిగి (ccline.cmdpos >= ccline.cmdlen);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(శూన్య)
{
    ఒకవేళ (ccline.cmdbuff == ఖాళీ || ccline.cmdpos > ccline.cmdlen)
	తిరిగి MAXCOL;

    ఒకవేళ (has_mbyte)
    {
	colnr_T	నిలువు;
	పూర్ణాంకం	i = 0;

	కొరకు (నిలువు = 0; i < ccline.cmdpos; ++నిలువు)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	తిరిగి నిలువు;
    }
    లేకపోతే
	తిరిగి ccline.cmdpos;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    స్థిర శూన్య
redrawcmd_preedit(శూన్య)
{
    ఒకవేళ ((State & బహుళకం_CMDLINE)
	    && xic != ఖాళీ
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	పూర్ణాంకం	cmdpos = 0;
	పూర్ణాంకం	cmdspos;
	పూర్ణాంకం	old_అడ్డువరుస;
	పూర్ణాంకం	old_నిలువు;
	colnr_T	నిలువు;

	old_అడ్డువరుస = సందేశం_అడ్డువరుస;
	old_నిలువు = సందేశం_నిలువు;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	ఒకవేళ (has_mbyte)
	{
	    కొరకు (నిలువు = 0; నిలువు < preedit_ప్రారంభం_నిలువు
			  && cmdpos < ccline.cmdlen; ++నిలువు)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	లేకపోతే
	{
	    cmdspos += preedit_ప్రారంభం_నిలువు;
	    cmdpos  += preedit_ప్రారంభం_నిలువు;
	}

	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస + (cmdspos / cmdline_వెడల్పు);
	సందేశం_నిలువు = cmdspos % cmdline_వెడల్పు;
	ఒకవేళ (సందేశం_అడ్డువరుస >= అడ్డువరుసలు)
	    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;

	కొరకు (నిలువు = 0; cmdpos < ccline.cmdlen; ++నిలువు)
	{
	    పూర్ణాంకం char_len;
	    పూర్ణాంకం char_attr;

	    char_attr = im_get_feedback_attr(నిలువు);
	    ఒకవేళ (char_attr < 0)
		ఆపు; // end of preedit string

	    ఒకవేళ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    లేకపోతే
		char_len = 1;

	    సందేశం_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	సందేశం_అడ్డువరుస = old_అడ్డువరుస;
	సందేశం_నిలువు = old_నిలువు;
    }
}
#అయితే_ముగింపు // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    స్థిర శూన్య
dealloc_cmdbuff(శూన్య)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    స్థిర శూన్య
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    ఒకవేళ (len < 80)
	len = 100;
    లేకపోతే
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (పూర్ణాంకం)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
realloc_cmdbuff(పూర్ణాంకం len)
{
    char_u	*p;
    పూర్ణాంకం		plen;

    ఒకవేళ (len < ccline.cmdbufflen)
	తిరిగి OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    ఒకవేళ (ccline.cmdbuff == ఖాళీ)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	తిరిగి FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    ఒకవేళ (ccline.xpc != ఖాళీ
	    && ccline.xpc->xp_నమూనా != ఖాళీ
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	పూర్ణాంకం i = (పూర్ణాంకం)(ccline.xpc->xp_నమూనా - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	ఒకవేళ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_నమూనా = ccline.cmdbuff + i;
    }

    vim_free(p);

    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_ARABIC)
స్థిర char_u	*arshape_buf = ఖాళీ;

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_arshape_buf(శూన్య)
{
    vim_free(arshape_buf);
}
# endif
#అయితే_ముగింపు

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    స్థిర శూన్య
draw_cmdline(పూర్ణాంకం ప్రారంభం, పూర్ణాంకం len)
{
#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    పూర్ణాంకం		i;

    ఒకవేళ (cmdline_నక్షత్రం > 0)
	కొరకు (i = 0; i < len; ++i)
	{
	    సందేశం_putchar('*');
	    ఒకవేళ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + ప్రారంభం + i) - 1;
	}
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_ARABIC
	ఒకవేళ (p_arshape && !p_tbidi && cmdline_has_arabic(ప్రారంభం, len))
    {
	స్థిర పూర్ణాంకం	buflen = 0;
	char_u		*p;
	పూర్ణాంకం		j;
	పూర్ణాంకం		newlen = 0;
	పూర్ణాంకం		mb_l;
	పూర్ణాంకం		pc, pc1 = 0;
	పూర్ణాంకం		prev_c = 0;
	పూర్ణాంకం		prev_c1 = 0;
	పూర్ణాంకం		u8c;
	పూర్ణాంకం		u8cc[MAX_MCO];
	పూర్ణాంకం		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	ఒకవేళ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    ఒకవేళ (arshape_buf == ఖాళీ)
		తిరిగి;	// out of memory
	}

	ఒకవేళ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + ప్రారంభం)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	కొరకు (j = ప్రారంభం; j < ప్రారంభం + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, ప్రారంభం + len - j);
	    mb_l = utfc_ptr2len_len(p, ప్రారంభం + len - j);
	    ఒకవేళ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		ఒకవేళ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    ఒకవేళ (j + mb_l >= ప్రారంభం + len)
			nc = NUL;
		    లేకపోతే
			nc = utf_ptr2char(p + mb_l);
		}
		లేకపోతే
		{
		    // displaying from left to right
		    ఒకవేళ (j + mb_l >= ప్రారంభం + len)
			pc = NUL;
		    లేకపోతే
		    {
			పూర్ణాంకం	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      ప్రారంభం + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ఖాళీ, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		ఒకవేళ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    ఒకవేళ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    లేకపోతే
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	సందేశం_outtrans_len(arshape_buf, newlen);
    }
    లేకపోతే
#అయితే_ముగింపు
	సందేశం_outtrans_len(ccline.cmdbuff + ప్రారంభం, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    శూన్య
putcmdline(పూర్ణాంకం c, పూర్ణాంకం జరుపు)
{
    ఒకవేళ (cmd_silent)
	తిరిగి;
    సందేశం_no_మరిన్ని = TRUE;
    సందేశం_putchar(c);
    ఒకవేళ (జరుపు)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    సందేశం_no_మరిన్ని = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_జరుపు = జరుపు;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    శూన్య
unputcmdline(శూన్య)
{
    ఒకవేళ (cmd_silent)
	తిరిగి;
    సందేశం_no_మరిన్ని = TRUE;
    ఒకవేళ (ccline.cmdlen == ccline.cmdpos)
	సందేశం_putchar(' ');
    లేకపోతే ఒకవేళ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    లేకపోతే
	draw_cmdline(ccline.cmdpos, 1);
    సందేశం_no_మరిన్ని = FALSE;
    cursorcmd();
    extra_char = NUL;
}

/*
 * Put the given string, of the given length, onto the command line.
 * If len is -1, then STRLEN() is used to calculate the length.
 * If 'redraw' is TRUE then the new part of the command line, and the remaining
 * part will be redrawn, otherwise it will not.  If this function is called
 * twice in a row, then 'redraw' should be FALSE and redrawcmd() should be
 * called afterwards.
 */
    పూర్ణాంకం
put_on_cmdline(char_u *str, పూర్ణాంకం len, పూర్ణాంకం redraw)
{
    పూర్ణాంకం		retval;
    పూర్ణాంకం		i;
    పూర్ణాంకం		m;
    పూర్ణాంకం		c;

    ఒకవేళ (len < 0)
	len = (పూర్ణాంకం)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    ఒకవేళ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    లేకపోతే
	retval = OK;
    ఒకవేళ (retval == OK)
    {
	ఒకవేళ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	లేకపోతే
	{
	    ఒకవేళ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		కొరకు (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		కొరకు (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		ఒకవేళ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		లేకపోతే
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    లేకపోతే ఒకవేళ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	ఒకవేళ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    అయితే (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_తల_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#నిర్వచితమైతే FEAT_ARABIC
	    ఒకవేళ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_తల_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ఒకవేళ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		లేకపోతే
		    i = 0;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		సందేశం_నిలువు -= i;
		ఒకవేళ (సందేశం_నిలువు < 0)
		{
		    సందేశం_నిలువు += cmdline_వెడల్పు;
		    --సందేశం_అడ్డువరుస;
		}
	    }
	}

	ఒకవేళ (redraw && !cmd_silent)
	{
	    సందేశం_no_మరిన్ని = TRUE;
	    i = cmdline_అడ్డువరుస;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    ఒకవేళ (cmdline_అడ్డువరుస != i || ccline.overstrike)
		సందేశం_clr_eos();
	    సందేశం_no_మరిన్ని = FALSE;
	}
	ఒకవేళ (కీలకం_Typed)
	{
	    m = cmdline_వెడల్పు * అడ్డువరుసలు;
	    ఒకవేళ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	లేకపోతే
	    m = MAXCOL;
	కొరకు (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    ఒకవేళ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    ఒకవేళ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    ఒకవేళ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		ఒకవేళ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    ఒకవేళ (redraw)
	సందేశం_check();
    తిరిగి retval;
}

స్థిర cmdline_info_T	prev_ccline;
స్థిర పూర్ణాంకం		prev_ccline_వాడిన = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    స్థిర శూన్య
save_cmdline(cmdline_info_T *ccp)
{
    ఒకవేళ (!prev_ccline_వాడిన)
    {
	CLEAR_క్షేత్రం(prev_ccline);
	prev_ccline_వాడిన = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ఖాళీ;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    స్థిర శూన్య
restore_cmdline(cmdline_info_T *ccp)
{
    ccline = prev_ccline;
    prev_ccline = *ccp;
}

/*
 * Paste a yank register into the command line.
 * Used by CTRL-R command in command-line mode.
 * insert_reg() can't be used here, because special characters from the
 * register contents will be interpreted as commands.
 *
 * Return FAIL for failure, OK otherwise.
 */
    స్థిర పూర్ణాంకం
cmdline_paste(
    పూర్ణాంకం regname,
    పూర్ణాంకం literally,	// Insert text literally instead of "as typed"
    పూర్ణాంకం remcr)		// remove trailing CR
{
    దీర్ఘ		i;
    char_u		*arg;
    char_u		*p;
    పూర్ణాంకం			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    ఒకవేళ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	తిరిగి FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    పంక్తి_breakcheck();
    ఒకవేళ (got_int)
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#అయితే_ముగింపు

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    ఒకవేళ (i)
    {
	// Got the value of a special register in "arg".
	ఒకవేళ (arg == ఖాళీ)
	    తిరిగి FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	ఒకవేళ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    పూర్ణాంకం	    len;

	    // Locate start of last word in the cmd buffer.
	    కొరకు (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		ఒకవేళ (has_mbyte)
		{
		    len = (*mb_తల_off)(ccline.cmdbuff, w - 1) + 1;
		    ఒకవేళ (!vim_iswordc(mb_ptr2char(w - len)))
			ఆపు;
		    w -= len;
		}
		లేకపోతే
		{
		    ఒకవేళ (!vim_iswordc(w[-1]))
			ఆపు;
		    --w;
		}
	    }
	    len = (పూర్ణాంకం)((ccline.cmdbuff + ccline.cmdpos) - w);
	    ఒకవేళ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	ఒకవేళ (allocated)
	    vim_free(arg);
	తిరిగి OK;
    }

    తిరిగి cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    శూన్య
cmdline_paste_str(char_u *s, పూర్ణాంకం literally)
{
    పూర్ణాంకం		c, cv;

    ఒకవేళ (literally)
	put_on_cmdline(s, -1, TRUE);
    లేకపోతే
	అయితే (*s != NUL)
	{
	    cv = *s;
	    ఒకవేళ (cv == Ctrl_V && s[1])
		++s;
	    ఒకవేళ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    లేకపోతే
		c = *s++;
	    ఒకవేళ (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#నిర్వచితమైతే UNIX
		    || c == intr_char
#అయితే_ముగింపు
		    || (c == Ctrl_BSL && *s == Ctrl_N))
		stuffcharReadbuff(Ctrl_V);
	    stuffcharReadbuff(c);
	}
}

/*
 * This function is called when the screen size changes and with incremental
 * search and in other situations where the command line may have been
 * overwritten.
 */
    శూన్య
redrawcmdline(శూన్య)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    శూన్య
redrawcmdline_ex(పూర్ణాంకం do_compute_cmdrow)
{
    ఒకవేళ (cmd_silent)
	తిరిగి;
    need_wait_return = FALSE;
    ఒకవేళ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    స్థిర శూన్య
redrawcmdprompt(శూన్య)
{
    పూర్ణాంకం		i;

    ఒకవేళ (cmd_silent)
	తిరిగి;
    ఒకవేళ (ccline.cmdfirstc != NUL)
	సందేశం_putchar(ccline.cmdfirstc);
    ఒకవేళ (ccline.cmdprompt != ఖాళీ)
    {
	సందేశం_puts_attr((అక్షరం *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = సందేశం_నిలువు + (సందేశం_అడ్డువరుస - cmdline_అడ్డువరుస) * cmdline_వెడల్పు;
	// do the reverse of set_cmdspos()
	ఒకవేళ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    లేకపోతే
	కొరకు (i = ccline.cmdindent; i > 0; --i)
	    సందేశం_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    శూన్య
redrawcmd(శూన్య)
{
    పూర్ణాంకం save_లోపల_echowindow = లోపల_echowindow;

    ఒకవేళ (cmd_silent)
	తిరిగి;

    // when 'incsearch' is set there may be no command line while redrawing
    ఒకవేళ (ccline.cmdbuff == ఖాళీ)
    {
	windgoto(cmdline_అడ్డువరుస, cmdline_నిలువు_off);
	సందేశం_clr_eos();
	తిరిగి;
    }

    // Do not put this in the message window.
    లోపల_echowindow = FALSE;

    sb_పాఠ్యం_restart_cmdline();
    సందేశం_ప్రారంభం();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    సందేశం_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    సందేశం_no_మరిన్ని = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    సందేశం_clr_eos();
    సందేశం_no_మరిన్ని = FALSE;

    సమితి_cmdspos_cursor();
    ఒకవేళ (extra_char != NUL)
	putcmdline(extra_char, extra_char_జరుపు);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    సందేశం_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    లోపల_echowindow = save_లోపల_echowindow;
}

    శూన్య
compute_cmdrow(శూన్య)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    ఒకవేళ (exmode_చురుకైన || (సందేశం_scrolled != 0 && !updating_screen))
	cmdline_అడ్డువరుస = అడ్డువరుసలు - 1;
    లేకపోతే
	cmdline_అడ్డువరుస = W_WINROW(lastwin) + lastwin->w_ఎత్తు
						    + lastwin->w_స్థితి_ఎత్తు;
}

    శూన్య
cursorcmd(శూన్య)
{
    ఒకవేళ (cmd_silent)
	తిరిగి;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (cmdmsg_rl)
    {
	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస  + (ccline.cmdspos / (cmdline_వెడల్పు - 1));
	సందేశం_నిలువు = cmdline_వెడల్పు - (ccline.cmdspos % (cmdline_వెడల్పు - 1)) - 1;
	ఒకవేళ (సందేశం_అడ్డువరుస <= 0)
	    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస + (ccline.cmdspos / cmdline_వెడల్పు);
	సందేశం_నిలువు = ccline.cmdspos % cmdline_వెడల్పు;
	ఒకవేళ (సందేశం_అడ్డువరుస >= అడ్డువరుసలు)
	    సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
    }

    windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ఒకవేళ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#అయితే_ముగింపు
#నిర్వచితమైతే MCH_CURSOR_SHAPE
    mch_update_cursor();
#అయితే_ముగింపు
}

    శూన్య
gotocmdline(పూర్ణాంకం clr)
{
    సందేశం_ప్రారంభం();
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (cmdmsg_rl)
	సందేశం_నిలువు = cmdline_వెడల్పు - 1;
    లేకపోతే
#అయితే_ముగింపు
	సందేశం_నిలువు = 0;	    // always start in column 0
    ఒకవేళ (clr)		    // clear the bottom line(s)
	సందేశం_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_అడ్డువరుస, cmdline_నిలువు_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    స్థిర పూర్ణాంకం
ccheck_abbr(పూర్ణాంకం c)
{
    పూర్ణాంకం spos = 0;

    ఒకవేళ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	తిరిగి FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    అయితే (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    ఒకవేళ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    లేకపోతే
	// check abbreviation from the beginning of the commandline
	spos = 0;

    తిరిగి check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, పూర్ణాంకం what)
{
    char_u	*p;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    char_u	buf[20];
    పూర్ణాంకం		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    కొరకు (p = what == VSE_బఫరు ? బఫరు_ESC_CHARS : మార్గం_ESC_CHARS;
								*p != NUL; ++p)
	ఒకవేళ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#లేకపోతే
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_బఫరు ? బఫరు_ESC_CHARS : మార్గం_ESC_CHARS);
    ఒకవేళ (what == VSE_SHELL && csh_like_shell() && p != ఖాళీ)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#అయితే_ముగింపు

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    ఒకవేళ (p != ఖాళీ && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    తిరిగి p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    శూన్య
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    శూన్య
tilde_replace(
    char_u  *orig_pat,
    పూర్ణాంకం	    అంకె_files,
    char_u  **files)
{
    పూర్ణాంకం	    i;
    char_u  *p;

    ఒకవేళ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	కొరకు (i = 0; i < అంకె_files; ++i)
	{
	    p = home_replace_save(ఖాళీ, files[i]);
	    ఒకవేళ (p != ఖాళీ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }
	}
    }
}

/*
 * Get a pointer to the current command line info.
 */
    cmdline_info_T *
get_cmdline_info(శూన్య)
{
    తిరిగి &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    స్థిర cmdline_info_T *
get_ccline_ptr(శూన్య)
{
    ఒకవేళ ((State & బహుళకం_CMDLINE) == 0)
	తిరిగి ఖాళీ;
    ఒకవేళ (ccline.cmdbuff != ఖాళీ)
	తిరిగి &ccline;
    ఒకవేళ (prev_ccline_వాడిన && prev_ccline.cmdbuff != ఖాళీ)
	తిరిగి &prev_ccline;
    తిరిగి ఖాళీ;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    స్థిర పూర్ణాంకం
get_cmdline_type(శూన్య)
{
    cmdline_info_T *p = get_ccline_ptr();

    ఒకవేళ (p == ఖాళీ)
	తిరిగి NUL;
    ఒకవేళ (p->cmdfirstc == NUL)
	తిరిగి
#నిర్వచితమైతే FEAT_EVAL
	    (p->ఆదానం_fn) ? '@' :
#అయితే_ముగింపు
	    '-';
    తిరిగి p->cmdfirstc;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    స్థిర char_u *
get_cmdline_str(శూన్య)
{
    cmdline_info_T *p;

    ఒకవేళ (cmdline_నక్షత్రం > 0)
	తిరిగి ఖాళీ;
    p = get_ccline_ptr();
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    స్థిర char_u *
get_cmdline_completion_నమూనా(శూన్య)
{
    cmdline_info_T *p;
    పూర్ణాంకం		xp_context;

    ఒకవేళ (cmdline_నక్షత్రం > 0)
	తిరిగి ఖాళీ;

    p = get_ccline_ptr();
    ఒకవేళ (p == ఖాళీ || p->xpc == ఖాళీ)
	తిరిగి ఖాళీ;

    xp_context = p->xpc->xp_context;
    ఒకవేళ (xp_context == EXPAND_NOTHING)
    {
	సమితి_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ఒకవేళ (xp_context == EXPAND_UNSUCCESSFUL)
	తిరిగి ఖాళీ;

    char_u *compl_pat = p->xpc->xp_నమూనా;

    ఒకవేళ (compl_pat == ఖాళీ)
	తిరిగి ఖాళీ;

    తిరిగి vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    స్థిర char_u *
get_cmdline_completion(శూన్య)
{
    cmdline_info_T	*p;
    పూర్ణాంకం			xp_context;

    ఒకవేళ (cmdline_నక్షత్రం > 0)
	తిరిగి ఖాళీ;

    p = get_ccline_ptr();
    ఒకవేళ (p == ఖాళీ || p->xpc == ఖాళీ)
	తిరిగి ఖాళీ;

    xp_context = p->xpc->xp_context;
    ఒకవేళ (xp_context == EXPAND_NOTHING)
    {
	సమితి_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ఒకవేళ (xp_context == EXPAND_UNSUCCESSFUL)
	తిరిగి ఖాళీ;

    తిరిగి cmdcomplete_type_గమ్యం_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    శూన్య
f_getcmdcomplpat(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_నమూనా();
}

/*
 * "getcmdcompltype()" function
 */
    శూన్య
f_getcmdcompltype(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    శూన్య
f_getcmdline(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    శూన్య
f_getcmdpos(typval_T *argvars వాడని, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_సంఖ్య = p != ఖాళీ ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    శూన్య
f_getcmdprompt(typval_T *argvars వాడని, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ఖాళీ && p->cmdprompt != ఖాళీ ?
					    vim_strsave(p->cmdprompt) : ఖాళీ;
}

/*
 * "getcmdscreenpos()" function
 */
    శూన్య
f_getcmdscreenpos(typval_T *argvars వాడని, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_సంఖ్య = p != ఖాళీ ? cmdline_నిలువు_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    శూన్య
f_getcmdtype(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    ఒకవేళ (rettv->vval.v_string == ఖాళీ)
	తిరిగి;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    స్థిర పూర్ణాంకం
సమితి_cmdline_str(char_u *str, పూర్ణాంకం pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    పూర్ణాంకం		    len;

    ఒకవేళ (p == ఖాళీ)
	తిరిగి 1;

    len = (పూర్ణాంకం)STRLEN(str);
    ఒకవేళ (realloc_cmdbuff(len + 1) != OK)
	తిరిగి 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    తిరిగి 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    స్థిర పూర్ణాంకం
సమితి_cmdline_pos(
    పూర్ణాంకం		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    ఒకవేళ (p == ఖాళీ)
	తిరిగి 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    ఒకవేళ (pos < 0)
	new_cmdpos = 0;
    లేకపోతే
	new_cmdpos = pos;

    తిరిగి 0;
}

// "setcmdline()" function
    శూన్య
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం pos = -1;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం error = FALSE;

	pos = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[1], &error) - 1;
	ఒకవేళ (error)
	    తిరిగి;
	ఒకవేళ (pos < 0)
	{
	    emsg(_(e_వాదం_must_be_ధనాత్మక));
	    తిరిగి;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_సంఖ్య = సమితి_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    శూన్య
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		pos;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    pos = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]) - 1;
    ఒకవేళ (pos >= 0)
	rettv->vval.v_సంఖ్య = సమితి_cmdline_pos(pos);
}
#అయితే_ముగింపు

/*
 * Return the first character of the current command line.
 */
    పూర్ణాంకం
get_cmdline_firstc(శూన్య)
{
    తిరిగి ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    పూర్ణాంకం
get_list_range(char_u **str, పూర్ణాంకం *num1, పూర్ణాంకం *num2)
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		మొదటి = FALSE;
    varnumber_T	అంకె;

    *str = skipwhite(*str);
    ఒకవేళ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ఖాళీ, &len, 0, &అంకె, ఖాళీ, 0, FALSE, ఖాళీ);
	*str += len;
	//అధికప్రవాహం
	ఒకవేళ (అంకె > గరిష్ఠ_పూర్ణాంకం)
	    తిరిగి FAIL;

	*num1 = (పూర్ణాంకం)అంకె;
	మొదటి = TRUE;
    }
    *str = skipwhite(*str);
    ఒకవేళ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ఖాళీ, &len, 0, &అంకె, ఖాళీ, 0, FALSE, ఖాళీ);
	ఒకవేళ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    //అధికప్రవాహం
	    ఒకవేళ (అంకె > గరిష్ఠ_పూర్ణాంకం)
		తిరిగి FAIL;

	    *num2 = (పూర్ణాంకం)అంకె;
	}
	లేకపోతే ఒకవేళ (!మొదటి)		// no number given at all
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (మొదటి)			// only one number given
	*num2 = *num1;
    తిరిగి OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    అక్షరం *
did_సమితి_cedit(optset_T *వాదనలు వాడని)
{
    పూర్ణాంకం n;

    ఒకవేళ (*p_cedit == NUL)
	cedit_కీలకం = -1;
    లేకపోతే
    {
	n = string_గమ్యం_కీలకం(p_cedit, FALSE);
	ఒకవేళ (n == 0 || vim_isprintc(n))
	    తిరిగి e_invalid_వాదం;
	cedit_కీలకం = n;
    }
    తిరిగి ఖాళీ;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    స్థిర పూర్ణాంకం
open_cmdwin(శూన్య)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    పూర్ణాంకం			i;
    linenr_T		lnum;
    పూర్ణాంకం			histtype;
    garray_T		winsizes;
    పూర్ణాంకం			save_restart_edit = restart_edit;
    పూర్ణాంకం			save_State = State;
    పూర్ణాంకం			save_exmode = exmode_చురుకైన;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    పూర్ణాంకం			save_cmdmsg_rl = cmdmsg_rl;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    పూర్ణాంకం			save_కీలకం_Typed;
#అయితే_ముగింపు
    పూర్ణాంకం			newbuf_స్థితి;
    పూర్ణాంకం			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    ఒకవేళ (పాఠ్యం_or_buf_locked()
	    || cmdwin_type != 0
#ఒకవేళ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_నక్షత్రం > 0
#అయితే_ముగింపు
	    )
    {
	beep_flush();
	తిరిగి K_IGNORE;
    }
    సమితి_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_కొలత_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    ఒకవేళ (win_విభజించు((పూర్ణాంకం)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	తిరిగి K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    ఒకవేళ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_బఫరు != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	తిరిగి Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_స్థితి = do_ecmd(0, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_ONE, ECMD_HIDE,
					ఖాళీ);
    cmdwin_valid = win_valid(cmdwin_win);
    ఒకవేళ (newbuf_స్థితి == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_బఫరు != old_curbuf.br_buf)
    {
	ఒకవేళ (newbuf_స్థితి == OK)
	    సమితి_bufref(&bufref, curbuf);
	ఒకవేళ (cmdwin_valid && !చివరి_కిటికీ())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	ఒకవేళ (newbuf_స్థితి == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_బఫరు(ఖాళీ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ఖాళీ;
	beep_flush();
	ga_clear(&winsizes);
	తిరిగి Ctrl_C;
    }
    cmdwin_buf = curbuf;

    సమితి_option_విలువ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#నిర్వచితమైతే FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#అయితే_ముగింపు
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_తాళం;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    ఒకవేళ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	ఒకవేళ (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    కూడు_map((char_u *)"<buffer> <Tab> <C-X><C-V>", బహుళకం_చొప్పించు, TRUE);
	    కూడు_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", బహుళకం_సాధారణ, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    కూడు_map((char_u *)"<buffer> <S-Tab> <C-P>", బహుళకం_చొప్పించు, TRUE);
	}
	సమితి_option_విలువ_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_తాళం;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    ఒకవేళ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	ఒకవేళ (i >= 0)
	{
	    lnum = 0;
	    చేయి
	    {
		ఒకవేళ (++i == get_hislen())
		    i = 0;
		ఒకవేళ (get_histentry(histtype)[i].hisstr != ఖాళీ)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    అయితే (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_పంక్తి_లెక్క, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    curwin->w_cursor.నిలువు = ccline.cmdpos;
    మారింది_పంక్తి_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_చురుకైన = 0;

    State = బహుళకం_సాధారణ;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_ఫలితం = 0;

    // Trigger CmdwinEnter autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINENTER);
    ఒకవేళ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#నిర్వచితమైతే FEAT_FOLDING
    save_కీలకం_Typed = కీలకం_Typed;
#అయితే_ముగింపు

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#నిర్వచితమైతే FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    కీలకం_Typed = save_కీలకం_Typed;
#అయితే_ముగింపు

    cmdwin_type = 0;
    cmdwin_buf = ఖాళీ;
    cmdwin_win = ఖాళీ;
    exmode_చురుకైన = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    ఒకవేళ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_బఫరు != old_curbuf.br_buf)
    {
	cmdwin_ఫలితం = Ctrl_C;
	emsg(_(e_చురుకైన_కిటికీ_or_బఫరు_మారింది_or_deleted));
    }
    లేకపోతే
    {
#ఒకవేళ defined(FEAT_EVAL)
	// autocmds may abort script processing
	ఒకవేళ (aborting() && cmdwin_ఫలితం != K_IGNORE)
	    cmdwin_ఫలితం = Ctrl_C;
#అయితే_ముగింపు
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	ఒకవేళ (cmdwin_ఫలితం == K_XF1 || cmdwin_ఫలితం == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    ఒకవేళ (cmdwin_ఫలితం == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    ఒకవేళ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (పూర్ణాంకం)plen;
		ccline.cmdbufflen = (పూర్ణాంకం)(plen + 1);
		cmdwin_ఫలితం = CAR;
	    }
	    లేకపోతే
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	లేకపోతే ఒకవేళ (cmdwin_ఫలితం == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ఖాళీ;
	}
	లేకపోతే
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	ఒకవేళ (ccline.cmdbuff == ఖాళీ)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_ఫలితం = Ctrl_C;
	}
	లేకపోతే
	{
	    ccline.cmdpos = curwin->w_cursor.నిలువు;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    ఒకవేళ (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#నిర్వచితమైతే FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#అయితే_ముగింపు
	// First go back to the original window.
	wp = curwin;
	సమితి_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	ఒకవేళ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	ఒకవేళ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_బఫరు(ఖాళీ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_కొలత_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	ఒకవేళ (cmdwin_ఫలితం == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    సమితి_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#అయితే_ముగింపు

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    తిరిగి cmdwin_ఫలితం;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    పూర్ణాంకం
is_లోపల_cmdwin(శూన్య)
{
    తిరిగి cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap వాడని, char_u *cmd వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    ఒకవేళ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ఖాళీ)
	తిరిగి ఖాళీ;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
#లేకపోతే
    తిరిగి ఖాళీ;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    శూన్య
get_user_ఆదానం(
    typval_T	*argvars,
    typval_T	*rettv,
    పూర్ణాంకం		inputdialog,
    పూర్ణాంకం		secret)
{
    char_u	*prompt;
    char_u	*p = ఖాళీ;
    పూర్ణాంకం		c;
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    పూర్ణాంకం		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ఖాళీ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    ఒకవేళ (ఆదానం_busy)
	తిరిగి;  // this doesn't work recursively.

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	తిరిగి;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_ఆదానం
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ఒకవేళ (no_console_ఆదానం() && !is_not_a_పదం())
	తిరిగి;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    ఒకవేళ (prompt != ఖాళీ)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	ఒకవేళ (p == ఖాళీ)
	    p = prompt;
	లేకపోతే
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    సందేశం_ప్రారంభం();
	    సందేశం_clr_eos();
	    సందేశం_puts_attr((అక్షరం *)prompt, get_echo_attr());
	    సందేశం_didout = FALSE;
	    సందేశం_starthere();
	    *p = c;
	}
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    ఒకవేళ (defstr != ఖాళీ)
		stuffReadbuffSpec(defstr);

	    ఒకవేళ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		పూర్ణాంకం	xp_namelen;
		దీర్ఘ	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ఖాళీ;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		ఒకవేళ (xp_name == ఖాళీ)
		    తిరిగి;

		xp_namelen = (పూర్ణాంకం)STRLEN(xp_name);

		ఒకవేళ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    తిరిగి;
	    }
	}

	ఒకవేళ (defstr != ఖాళీ)
	{
	    పూర్ణాంకం save_ex_సాధారణ_busy = ex_సాధారణ_busy;
	    పూర్ణాంకం save_vgetc_busy = vgetc_busy;
	    పూర్ణాంకం save_ఆదానం_busy = ఆదానం_busy;

	    ఆదానం_busy |= vgetc_busy;
	    ex_సాధారణ_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_సాధారణ_busy = save_ex_సాధారణ_busy;
	    vgetc_busy = save_vgetc_busy;
	    ఆదానం_busy = save_ఆదానం_busy;
	}
	ఒకవేళ (inputdialog && rettv->vval.v_string == ఖాళీ
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	సందేశం_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#అయితే_ముగింపు

/*
 * "wildtrigger()" function
 */
    శూన్య
f_wildtrigger(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    ఒకవేళ (!(State & బహుళకం_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_చురుకైన())
	తిరిగి;

    పూర్ణాంకం cmd_type = get_cmdline_type();

    ఒకవేళ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	కీలకం_string[4];

	కీలకం_string[0] = K_SPECIAL;
	కీలకం_string[1] = KS_EXTRA;
	కీలకం_string[2] = KE_WILD;
	కీలకం_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(కీలకం_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
