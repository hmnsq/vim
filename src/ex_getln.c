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

#অন্তর্ভাৱ "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_চাবি	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
স্থির cmdline_info_T ccline;

#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক	new_cmdpos;	// position set by set_cmdline_pos()
#endif

স্থির পূর্ণাঙ্ক	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
স্থির পূর্ণাঙ্ক	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
স্থির পূর্ণাঙ্ক	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

স্থির char_u	*getcmdline_int(পূর্ণাঙ্ক firstc, long গুনতি, পূর্ণাঙ্ক indent, পূর্ণাঙ্ক clear_ccline);
স্থির পূর্ণাঙ্ক	cmdline_charsize(পূর্ণাঙ্ক idx);
স্থির শূন্য	set_cmdspos(শূন্য);
স্থির শূন্য	set_cmdspos_cursor(শূন্য);
স্থির শূন্য	correct_cmdspos(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক cells);
স্থির শূন্য	dealloc_cmdbuff(শূন্য);
স্থির শূন্য	alloc_cmdbuff(size_t len);
স্থির শূন্য	draw_cmdline(পূর্ণাঙ্ক start, পূর্ণাঙ্ক len);
স্থির শূন্য	save_cmdline(cmdline_info_T *ccp);
স্থির শূন্য	restore_cmdline(cmdline_info_T *ccp);
স্থির পূর্ণাঙ্ক	cmdline_paste(পূর্ণাঙ্ক regname, পূর্ণাঙ্ক literally, পূর্ণাঙ্ক remcr);
স্থির শূন্য	redrawcmdprompt(শূন্য);
স্থির পূর্ণাঙ্ক	ccheck_abbr(পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক	open_cmdwin(শূন্য);
#ifdef FEAT_অনুসন্ধান_EXTRA
স্থির পূর্ণাঙ্ক	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

স্থির পূর্ণাঙ্ক	cedit_চাবি = -1;	// key value of 'cedit' option

    স্থির শূন্য
trigger_cmd_autocmd(পূর্ণাঙ্ক typechar, পূর্ণাঙ্ক evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    স্থির শূন্য
abandon_cmdline(শূন্য)
{
    dealloc_cmdbuff();
    যদি (বাৰ্তা_scrolled == 0)
	compute_cmdrow();
    বাৰ্তা("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_অনুসন্ধান_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    স্থির পূর্ণাঙ্ক
empty_pattern(char_u *p, size_t len, পূর্ণাঙ্ক delim)
{
    magic_T	magic_val = MAGIC_ON;

    যদি (len > 0)
	(শূন্য) skip_regexp_ex(p, delim, magic_isset(), রিক্ত, রিক্ত, &magic_val);
    অন্যথা
	ঘূৰাই TRUE;

    ঘূৰাই empty_pattern_magic(p, len, magic_val);
}

    স্থির পূর্ণাঙ্ক
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    যেতিয়ালৈ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != রিক্ত)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ঘূৰাই len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
প্রকারনির্দেশ সংরচনা {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    পূর্ণাঙ্ক		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    স্থির শূন্য
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
    vs->vs_empty_rows = curwin->w_empty_rows;
}

    স্থির শূন্য
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
    curwin->w_empty_rows = vs->vs_empty_rows;
}

// Struct to store the state of 'incsearch' highlighting.
প্রকারনির্দেশ সংরচনা {
    pos_T	অনুসন্ধান_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    পূর্ণাঙ্ক		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    পূর্ণাঙ্ক		did_incsearch;
    পূর্ণাঙ্ক		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    স্থির শূন্য
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->অনুসন্ধান_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    স্থির শূন্য
set_অনুসন্ধান_match(pos_T *t)
{
    t->lnum += অনুসন্ধান_match_সাৰিগুলি;
    t->col = অনুসন্ধান_match_endcol;
    যদি (t->lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
    {
	t->lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    পূর্ণাঙ্ক
parse_pattern_and_range(
	pos_T	*incsearch_start,
	পূর্ণাঙ্ক	*অনুসন্ধান_delim,
	পূর্ণাঙ্ক	*skiplen,
	পূর্ণাঙ্ক	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    পূর্ণাঙ্ক		delim_optional = FALSE;
    পূর্ণাঙ্ক		delim;
    পূর্ণাঙ্ক		use_last_pat;
    magic_T     magic = 0;
    ৱর্ণ	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    অনুসন্ধান_first_সাৰি = 0;
    অনুসন্ধান_last_সাৰি = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_সাৰিগুলি;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, রিক্ত);

    যদি (vim_strchr((char_u *)"sgvlu", *cmd) == রিক্ত)
	ঘূৰাই FALSE;

    // Skip over command name to find pattern separator
    বাবে (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    যদি (*skipwhite(p) == NUL)
	ঘূৰাই FALSE;

    যদি (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	যদি (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	অন্যথা যদি (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    অন্যথা যদি (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	যদি (*p == '!')
	    p = skipwhite(p + 1);
	যেতিয়ালৈ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	যদি (*p == NUL)
	    ঘূৰাই FALSE;
    }
    অন্যথা যদি (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	যদি (*p == '!')
	{
	    p++;
	    যদি (*skipwhite(p) == NUL)
		ঘূৰাই FALSE;
	}
	যদি (*cmd != 'g')
	    delim_optional = TRUE;
    }
    অন্যথা
	ঘূৰাই FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *অনুসন্ধান_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), রিক্ত, রিক্ত, &magic);
    use_last_pat = end == p && *end == delim;

    যদি (end == p && !use_last_pat)
	ঘূৰাই FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    যদি (!use_last_pat)
    {
	ৱর্ণ c = *end;
	পূর্ণাঙ্ক empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	যদি (empty)
	    ঘূৰাই FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (পূর্ণাঙ্ক)(p - ccline.cmdbuff);
    *patlen = (পূর্ণাঙ্ক)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    যদি (ea.addr_গুনতি > 0)
    {
	পূর্ণাঙ্ক reverse_match = ea.line2 < ea.line1;
	অনুসন্ধান_first_সাৰি = reverse_match ? ea.line2 : ea.line1;
	অনুসন্ধান_last_সাৰি = reverse_match ? ea.line1 : ea.line2;
    }
    অন্যথা যদি (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	অনুসন্ধান_first_সাৰি = অনুসন্ধান_last_সাৰি = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ঘূৰাই TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    স্থির পূর্ণাঙ্ক
do_incsearch_highlighting(
	পূর্ণাঙ্ক		    firstc,
	পূর্ণাঙ্ক		    *অনুসন্ধান_delim,
	incsearch_state_T   *is_state,
	পূর্ণাঙ্ক		    *skiplen,
	পূর্ণাঙ্ক		    *patlen)
{
    পূর্ণাঙ্ক retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    যদি (!p_is || cmd_silent)
	ঘূৰাই FALSE;

    // By default search all lines
    অনুসন্ধান_first_সাৰি = 0;
    অনুসন্ধান_last_সাৰি = MAXLNUM;

    যদি (firstc == '/' || firstc == '?')
    {
	*অনুসন্ধান_delim = firstc;
	ঘূৰাই TRUE;
    }

    যদি (firstc != ':')
	ঘূৰাই FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->অনুসন্ধান_start, অনুসন্ধান_delim,
	    skiplen, patlen);
    --emsg_off;

    ঘূৰাই retval;
}

    স্থির শূন্য
finish_incsearch_highlighting(
	পূর্ণাঙ্ক gotesc,
	incsearch_state_T *is_state,
	পূর্ণাঙ্ক call_update_screen)
{
    যদি (!is_state->did_incsearch)
	ঘূৰাই;

    is_state->did_incsearch = FALSE;
    যদি (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    অন্যথা
    {
	যদি (!EQUAL_POS(is_state->save_cursor, is_state->অনুসন্ধান_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->অনুসন্ধান_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    অনুসন্ধান_first_সাৰি = 0;
    অনুসন্ধান_last_সাৰি = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    যদি (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    স্থির শূন্য
may_do_incsearch_highlighting(
	পূর্ণাঙ্ক		    firstc,
	long		    গুনতি,
	incsearch_state_T   *is_state)
{
    পূর্ণাঙ্ক		skiplen, patlen;
    পূর্ণাঙ্ক		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    পূর্ণাঙ্ক		next_char;
    পূর্ণাঙ্ক		use_last_pat;
    পূর্ণাঙ্ক		did_do_incsearch = is_state->did_incsearch;
    পূর্ণাঙ্ক		অনুসন্ধান_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_অনুসন্ধান_pattern();

    যদি (!do_incsearch_highlighting(firstc, &অনুসন্ধান_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_অনুসন্ধান_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	যদি (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ঘূৰাই;
    }

    // If there is a character waiting, search and redraw later.
    যদি (char_avail())
    {
	restore_last_অনুসন্ধান_pattern();
	is_state->incsearch_postponed = TRUE;
	ঘূৰাই;
    }
    is_state->incsearch_postponed = FALSE;

    যদি (অনুসন্ধান_first_সাৰি == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->অনুসন্ধান_start;
    অন্যথা যদি (অনুসন্ধান_first_সাৰি > curbuf->b_ml.ml_সাৰি_গুনতি)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
	curwin->w_cursor.col = MAXCOL;
    }
    অন্যথা
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = অনুসন্ধান_first_সাৰি;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    যদি (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    অন্যথা
    {
	পূর্ণাঙ্ক অনুসন্ধান_flags = অনুসন্ধান_OPT + অনুসন্ধান_NOOF + অনুসন্ধান_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	যদি (!p_hls)
	    অনুসন্ধান_flags += অনুসন্ধান_KEEP;
	যদি (অনুসন্ধান_first_সাৰি != 0)
	    অনুসন্ধান_flags += অনুসন্ধান_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_অনুসন্ধান(রিক্ত, firstc == ':' ? '/' : firstc, অনুসন্ধান_delim,
				 ccline.cmdbuff + skiplen, patlen, গুনতি, অনুসন্ধান_flags,
# ifdef FEAT_RELTIME
		&sia
# অন্যথা
		রিক্ত
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	যদি (curwin->w_cursor.lnum < অনুসন্ধান_first_সাৰি
		|| curwin->w_cursor.lnum > অনুসন্ধান_last_সাৰি)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->অনুসন্ধান_start;
	}

	// if interrupted while searching, behave like it failed
	যদি (got_int)
	{
	    (শূন্য)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	অন্যথা যদি (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    যদি (found != 0)
	highlight_match = TRUE;		// highlight position
    অন্যথা
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    যদি (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_অনুসন্ধান_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    অন্যথা
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    যদি (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	যদি (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, অনুসন্ধান_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    যদি (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_অনুসন্ধান_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    যদি (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->অনুসন্ধান_start;
    অন্যথা যদি (found != 0)
	curwin->w_cursor = end_pos;

    বাৰ্তা_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    স্থির পূর্ণাঙ্ক
may_adjust_incsearch_highlighting(
	পূর্ণাঙ্ক			firstc,
	long			গুনতি,
	incsearch_state_T	*is_state,
	পূর্ণাঙ্ক			c)
{
    পূর্ণাঙ্ক		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = রিক্ত;
    char_u	*searchstr;
    char_u	*strcopy = রিক্ত;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    পূর্ণাঙ্ক		অনুসন্ধান_flags = অনুসন্ধান_NOOF;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		অনুসন্ধান_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_অনুসন্ধান_pattern();

    যদি (!do_incsearch_highlighting(firstc, &অনুসন্ধান_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_অনুসন্ধান_pattern();
	ঘূৰাই OK;
    }
    যদি (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_অনুসন্ধান_pattern();
	ঘূৰাই FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (শূন্য)parse_অনুসন্ধান_pattern_offset(&pat, &patlen_s, অনুসন্ধান_delim,
				    অনুসন্ধান_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    যদি (c == Ctrl_G)
    {
	t = is_state->match_end;
	যদি (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (শূন্য)decl(&t);
	অনুসন্ধান_flags += অনুসন্ধান_COL;
    }
    অন্যথা
	t = is_state->match_start;
    যদি (!p_hls)
	অনুসন্ধান_flags += অনুসন্ধান_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, রিক্ত,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, গুনতি, অনুসন্ধান_flags, RE_অনুসন্ধান, রিক্ত);
    --emsg_off;
    যদি (dircp != রিক্ত)
	*dircp = অনুসন্ধান_delim;
    যদি (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->অনুসন্ধান_start = match_start;
	যদি (!offset.সাৰি && (offset.end || off != 0))
	{
	    যদি (offset.end)
	    {
		is_state->অনুসন্ধান_start = match_end;
		(শূন্য)decl(&is_state->অনুসন্ধান_start);
	    }
	    যেতিয়ালৈ (off > 0)
	    {
		যদি (incl(&is_state->অনুসন্ধান_start) == -1)
		    ভঙ্গ;
		--off;
	    }
	    যেতিয়ালৈ (off < 0)
	    {
		যদি (decl(&is_state->অনুসন্ধান_start) == -1)
		    ভঙ্গ;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	যদি (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->অনুসন্ধান_start = t;
	    (শূন্য)decl(&is_state->অনুসন্ধান_start);
	}
	অন্যথা যদি (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->অনুসন্ধান_start = t;
	    (শূন্য)incl(&is_state->অনুসন্ধান_start);
	}
	যদি (LT_POS(t, is_state->অনুসন্ধান_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->অনুসন্ধান_start = t;
	    যদি (firstc == '?')
		(শূন্য)incl(&is_state->অনুসন্ধান_start);
	    অন্যথা
		(শূন্য)decl(&is_state->অনুসন্ধান_start);
	}

	set_অনুসন্ধান_match(&is_state->match_end);
	curwin->w_cursor = is_state->match_start;
	changed_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_match = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_match = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->match_end;
    }
    অন্যথা
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_অনুসন্ধান_pattern();
    ঘূৰাই FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    স্থির পূর্ণাঙ্ক
may_add_char_to_অনুসন্ধান(পূর্ণাঙ্ক firstc, পূর্ণাঙ্ক *c, incsearch_state_T *is_state)
{
    পূর্ণাঙ্ক		skiplen, patlen, অনুসন্ধান_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_অনুসন্ধান_pattern();

    যদি (!do_incsearch_highlighting(firstc, &অনুসন্ধান_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_অনুসন্ধান_pattern();
	ঘূৰাই FAIL;
    }
    restore_last_অনুসন্ধান_pattern();

    // Add a character from under the cursor for 'incsearch'.
    যদি (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	যদি (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    যদি (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    যদি (*c == অনুসন্ধান_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != রিক্ত)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    যদি (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		পূর্ণাঙ্ক save_c = *c;

		যেতিয়ালৈ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ঘূৰাই FAIL;
	}
    }
    ঘূৰাই OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    স্থির পূর্ণাঙ্ক
cmdline_has_arabic(পূর্ণাঙ্ক start, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক	    j;
    পূর্ণাঙ্ক	    mb_l;
    পূর্ণাঙ্ক	    u8c;
    char_u  *p;
    পূর্ণাঙ্ক	    u8cc[MAX_MCO];

    যদি (!enc_utf8)
	ঘূৰাই FALSE;

    বাবে (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	যদি (ARABIC_CHAR(u8c))
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

    শূন্য
cmdline_init(শূন্য)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    স্থির পূর্ণাঙ্ক
cmdline_handle_ctrl_bsl(পূর্ণাঙ্ক c, পূর্ণাঙ্ক *gotesc)
{
    ++no_mapping;
    ++allow_চাবিগুলি;
    c = plain_vgetc();
    --no_mapping;
    --allow_চাবিগুলি;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    যদি (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && চাবি_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	ঘূৰাই PROCESS_NEXT_চাবি;
    }

#ifdef FEAT_EVAL
    যদি (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	যদি (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	অন্যথা
	    new_cmdpos = ccline.cmdpos;

	c = get_ৰাশিমালা_register();
	যদি (c == '=')
	{
	    char_u	*p = রিক্ত;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_ৰাশিমালা_সাৰি();
	    --textlock;

	    যদি (p != রিক্ত)
	    {
		পূর্ণাঙ্ক len = (পূর্ণাঙ্ক)STRLEN(p);

		যদি (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    যদি (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    অন্যথা
			ccline.cmdpos = new_cmdpos;

		    চাবি_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ঘূৰাই CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ঘূৰাই CMDLINE_NOT_CHANGED;
    }
#endif

    যদি (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ঘূৰাই GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    স্থির পূর্ণাঙ্ক
cmdline_wildchar_complete(
	পূর্ণাঙ্ক		c,
	পূর্ণাঙ্ক		escape,
	পূর্ণাঙ্ক		*did_wild_list,
	পূর্ণাঙ্ক		*wim_index_p,
	expand_T	*xp,
	পূর্ণাঙ্ক		*gotesc,
	পূর্ণাঙ্ক		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    পূর্ণাঙ্ক		wim_index = *wim_index_p;
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		cmdpos_before;
    পূর্ণাঙ্ক		options = WILD_NO_BEEP;
    পূর্ণাঙ্ক		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    পূর্ণাঙ্ক		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    যদি (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    যদি (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	যদি (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (শূন্য)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	যদি (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	অন্যথা যদি (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	অন্যথা
	    res = OK;	    // don't insert 'wildchar' now
    }
    অন্যথা		    // typed p_wc first time
    {
	পূর্ণাঙ্ক wim_longest = (wim_flags[0] & WIM_LONGEST);
	পূর্ণাঙ্ক wim_list = (wim_flags[0] & WIM_LIST);
	পূর্ণাঙ্ক wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	যদি (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    যদি (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    যদি (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    অন্যথা
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	যদি (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	অন্যথা
	{
	    যদি (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    যদি (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	যদি (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	যদি (got_int)
	{
	    (শূন্য)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (শূন্য)ExpandOne(xp, রিক্ত, রিক্ত, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    ঘূৰাই CMDLINE_CHANGED;
	}

	// Display matches
	যদি (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    যদি (wim_longest)
	    {
		পূর্ণাঙ্ক found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		পূর্ণাঙ্ক show_menu = p_wmnu && wim_full;

		যদি (wim_list || show_menu)
		    (শূন্য)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		অন্যথা যদি (!found_longest_prefix)
		{
		    পূর্ণাঙ্ক wim_list_next = (wim_flags[1] & WIM_LIST);
		    পূর্ণাঙ্ক wim_full_next = (wim_flags[1] & WIM_FULL);
		    পূর্ণাঙ্ক wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    পূর্ণাঙ্ক wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    যদি (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			যদি (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			অন্যথা
			    (শূন্য)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			যদি (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    অন্যথা
	    {
		পূর্ণাঙ্ক show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		যদি (wim_list || show_menu)
		    (শূন্য)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		অন্যথা
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    যদি (wim_list)
		*did_wild_list = TRUE;
	}
	অন্যথা যদি (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    যদি (wim_index < 3)
	++wim_index;
    যদি (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    ঘূৰাই (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    স্থির পূর্ণাঙ্ক
cmdline_erase_chars(
	পূর্ণাঙ্ক c,
	পূর্ণাঙ্ক indent
#ifdef FEAT_অনুসন্ধান_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;

    যদি (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    যদি (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    যদি (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    যদি (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	যদি (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    যদি (c == Ctrl_W)
	    {
		যেতিয়ালৈ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		যেতিয়ালৈ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		যদি (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	অন্যথা যদি (c == Ctrl_W)
	{
	    যেতিয়ালৈ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    যদি (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		যেতিয়ালৈ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	অন্যথা
	    --p;
	ccline.cmdpos = (পূর্ণাঙ্ক)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	যেতিয়ালৈ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_অনুসন্ধান_EXTRA
	যদি (ccline.cmdlen == 0)
	{
	    isp->অনুসন্ধান_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    অন্যথা যদি (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == রিক্ত && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	যদি (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    ঘূৰাই CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	যদি (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    যদি (cmdmsg_rl)
		বাৰ্তা_col = cmdline_width;
	    অন্যথা
#endif
		বাৰ্তা_col = 0;
	    বাৰ্তা_putchar(' ');		// delete ':'
	}
#ifdef FEAT_অনুসন্ধান_EXTRA
	isp->অনুসন্ধান_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	ঘূৰাই GOTO_NORMAL_MODE;
    }
    ঘূৰাই CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    স্থির শূন্য
cmdline_toggle_langmap(long *b_im_ptr)
{
    যদি (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_প্ৰবেশ_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	যদি (b_im_ptr != রিক্ত)
	{
	    যদি (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    অন্যথা
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_প্ৰবেশ_METHOD
    অন্যথা
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	যদি ((p_imdisable && b_im_ptr != রিক্ত)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    যদি (b_im_ptr != রিক্ত)
		*b_im_ptr = B_IMODE_NONE;
	}
	অন্যথা
	{
	    im_set_active(TRUE);	// Enable input method
	    যদি (b_im_ptr != রিক্ত)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    যদি (b_im_ptr != রিক্ত)
    {
	যদি (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	অন্যথা
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#যদি defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    স্থির পূর্ণাঙ্ক
cmdline_ঢোকাও_reg(পূর্ণাঙ্ক *gotesc UNUSED)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		literally = FALSE;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_চাবিগুলি;
    i = c = plain_vgetc();	// CTRL-R <char>
    যদি (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    যদি (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_চাবিগুলি;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    যদি (c == '=')
    {
	যদি (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	অন্যথা
	    c = get_ৰাশিমালা_register();
    }
#endif
    যদি (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#ifdef FEAT_CLIPBOARD
			|| (clip_star.available && c == '*')
			|| (clip_plus.available && c == '+')
#endif
			;
	cmdline_paste(c, literally, FALSE);

#ifdef FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	যদি (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ঘূৰাই GOTO_NORMAL_MODE;
	}
#endif
	চাবি_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	যদি (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    যদি (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    অন্যথা
		ccline.cmdpos = new_cmdpos;
	}
#endif
    }
#ifdef FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#endif

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    ঘূৰাই literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    স্থির শূন্য
cmdline_বাম_ডান_mouse(পূর্ণাঙ্ক c, পূর্ণাঙ্ক *ignore_drag_release)
{
    যদি (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    অন্যথা
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    যদি (!gui.in_use)
#endif
	যদি (!mouse_has(MOUSE_COMMAND))
	    ঘূৰাই;
#ifdef FEAT_CLIPBOARD
    যদি (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	পূর্ণাঙ্ক	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	যদি (mouse_model_popup() && button == MOUSE_বাম
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_ডান;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	ঘূৰাই;
    }
#endif

    set_cmdspos();
    বাবে (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	পূর্ণাঙ্ক	i;

	i = cmdline_charsize(ccline.cmdpos);
	যদি (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    ভঙ্গ;
	যদি (has_mbyte)
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
    স্থির পূর্ণাঙ্ক
cmdline_browse_history(
	পূর্ণাঙ্ক	c,
	পূর্ণাঙ্ক	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	পূর্ণাঙ্ক	histype,
	পূর্ণাঙ্ক	*hiscnt_p,
	expand_T *xp)
{
    পূর্ণাঙ্ক		orig_hiscnt;
    পূর্ণাঙ্ক		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    পূর্ণাঙ্ক		res;

    যদি (get_hislen() == 0 || firstc == NUL)	// no history
	ঘূৰাই CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    যদি (lookfor == রিক্ত)
    {
	যদি ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == রিক্ত)
	    ঘূৰাই CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    বাবে (;;)
    {
	// one step backwards
	যদি (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    যদি (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    অন্যথা যদি (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    অন্যথা যদি (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    অন্যথা			// at top of list
	    {
		hiscnt = orig_hiscnt;
		ভঙ্গ;
	    }
	}
	অন্যথা    // one step forwards
	{
	    // on last entry, clear the line
	    যদি (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		ভঙ্গ;
	    }

	    // not on a history line, nothing to do
	    যদি (hiscnt == get_hislen())
		ভঙ্গ;
	    যদি (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    অন্যথা
		++hiscnt;
	}
	যদি (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== রিক্ত)
	{
	    hiscnt = orig_hiscnt;
	    ভঙ্গ;
	}
	যদি ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    ভঙ্গ;
    }

    যদি (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	পূর্ণাঙ্ক	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	যদি (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	অন্যথা
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	যদি (histype == HIST_অনুসন্ধান
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    পূর্ণাঙ্ক     i;
	    পূর্ণাঙ্ক     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    বাবে (i = 0; i <= 1; ++i)
	    {
		len = 0;
		বাবে (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    যদি (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			যদি (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    অন্যথা
		    {
			// Escape new sep, unless it is already
			// escaped.
			যদি (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    যদি (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			যদি (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		যদি (i == 0)
		{
		    alloc_cmdbuff(len);
		    যদি (ccline.cmdbuff == রিক্ত)
		    {
			res = GOTO_NORMAL_MODE;
			গচ্ছ done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (পূর্ণাঙ্ক)len;
	}
	অন্যথা
	{
	    alloc_cmdbuff(plen);
	    যদি (ccline.cmdbuff == রিক্ত)
	    {
		res = GOTO_NORMAL_MODE;
		গচ্ছ done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (পূর্ণাঙ্ক)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	গচ্ছ done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ঘূৰাই res;
}

/*
 * Initialize the current command-line info.
 */
    স্থির পূর্ণাঙ্ক
init_ccline(পূর্ণাঙ্ক firstc, পূর্ণাঙ্ক indent)
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
    যদি (ccline.cmdbuff == রিক্ত)
    {
	ccline.cmdbufflen = 0;
	ঘূৰাই FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    যদি (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ঘূৰাই OK;
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
    পূর্ণাঙ্ক		  firstc,
    long	  গুনতি,	// only used for incremental search
    পূর্ণাঙ্ক		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    ঘূৰাই getcmdline_int(firstc, গুনতি, indent, TRUE);
}

    স্থির char_u *
getcmdline_int(
    পূর্ণাঙ্ক		firstc,
    long	গুনতি UNUSED,	// only used for incremental search
    পূর্ণাঙ্ক		indent,		// indent for inside conditionals
    পূর্ণাঙ্ক		clear_ccline)	// clear ccline first
{
    স্থির পূর্ণাঙ্ক	depth = 0;	    // call depth
    পূর্ণাঙ্ক		c = 0;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;
    পূর্ণাঙ্ক		gotesc = FALSE;		// TRUE when <ESC> just typed
    পূর্ণাঙ্ক		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = রিক্ত;	// string to match
    size_t	lookforlen = 0;
    পূর্ণাঙ্ক		hiscnt;			// current history line in use
    পূর্ণাঙ্ক		histype;		// history type to be used
#ifdef FEAT_অনুসন্ধান_EXTRA
    incsearch_state_T	is_state;
#endif
    পূর্ণাঙ্ক		did_wild_list = FALSE;	// did wild_list() recently
    পূর্ণাঙ্ক		wim_index = 0;		// index in wim_flags[]
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		save_বাৰ্তা_scroll = বাৰ্তা_scroll;
    পূর্ণাঙ্ক		save_State = State;	// remember State when called
    পূর্ণাঙ্ক		some_চাবি_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    পূর্ণাঙ্ক		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = রিক্ত;
    buf_T	*b_im_ptr_buf = রিক্ত;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    পূর্ণাঙ্ক		did_save_ccline = FALSE;
    পূর্ণাঙ্ক		cmdline_type;
    পূর্ণাঙ্ক		wild_type = 0;
    পূর্ণাঙ্ক		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = রিক্ত;
    পূর্ণাঙ্ক		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    যদি (ccline.cmdbuff != রিক্ত)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    যদি (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    যদি (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    যদি (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_অনুসন্ধান_EXTRA
    init_incsearch_state(&is_state);
#endif

    যদি (init_ccline(firstc, indent) != OK)
	গচ্ছ theend;	// out of memory

    যদি (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	গচ্ছ theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    অন্যথা
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    যদি (!cmd_silent)
    {
	i = বাৰ্তা_scrolled;
	বাৰ্তা_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	বাৰ্তা_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#যদি defined(FEAT_EVAL)
    যদি (ccline.প্ৰবেশ_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_pattern = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#endif

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    বাৰ্তা_scroll = FALSE;

    State = MODE_CMDLINE;

    যদি (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	যদি (curbuf->b_p_imsearch == B_IMODE_USE_ঢোকাও)
	    b_im_ptr = &curbuf->b_p_iminsert;
	অন্যথা
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	যদি (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_প্ৰবেশ_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_প্ৰবেশ_METHOD
    অন্যথা যদি (p_imcmdline)
	im_set_active(TRUE);
#endif

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#ifdef FEAT_EVAL
    যদি (!debug_mode)
	may_trigger_modechanged();
#endif

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#ifdef FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#endif

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    যদি (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    যদি (!cmd_silent && বাৰ্তা_scrolled == 0)
    {
	পূর্ণাঙ্ক	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    যদি (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	যদি (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	যদি (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    বাবে (;;)
    {
	পূর্ণাঙ্ক	trigger_cmdlinechanged = TRUE;
	পূর্ণাঙ্ক	end_wildmenu;
	পূর্ণাঙ্ক	prev_cmdpos = ccline.cmdpos;
	পূর্ণাঙ্ক	skip_pum_redraw = FALSE;

	VIM_CLEAR(prev_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#endif
	quit_more = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	যদি (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_চাবি_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	যদি (ccline.cmdbuff != রিক্ত)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    যদি (prev_cmdbuff == রিক্ত)
		গচ্ছ returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	যদি (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	কৰা
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} যেতিয়ালৈ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	যদি (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    যদি (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (শূন্য)ExpandOne(&xpc, রিক্ত, রিক্ত, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	যদি (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    অনুৱর্ত;
	}

	যদি (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    পূর্ণাঙ্ক	    clen = ccline.cmdlen;
	    পূর্ণাঙ্ক	    cc_গুনতি = aucmd_cmdline_changed_গুনতি;

	    যদি (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		যদি (clen == ccline.cmdlen
				    || cc_গুনতি != aucmd_cmdline_changed_গুনতি)
		    trigger_cmdlinechanged = FALSE;
		গচ্ছ cmdline_changed;
	    }
	}

	যদি (চাবি_Typed)
	{
	    some_চাবি_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    যদি (cmd_hkmap)
		c = hkmap(c);
	    যদি (cmdmsg_rl && !চাবি_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		ৱিকল্প (c)
		{
		    পক্ষ K_ডান:   c = K_বাম; ভঙ্গ;
		    পক্ষ K_S_ডান: c = K_S_বাম; ভঙ্গ;
		    পক্ষ K_C_ডান: c = K_C_বাম; ভঙ্গ;
		    পক্ষ K_বাম:    c = K_ডান; ভঙ্গ;
		    পক্ষ K_S_বাম:  c = K_S_ডান; ভঙ্গ;
		    পক্ষ K_C_বাম:  c = K_C_ডান; ভঙ্গ;
		}
	    }
#endif
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	যদি ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#যদি defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#endif
#ifdef FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_active)
#endif
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	যদি (lookfor != রিক্ত
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_বাম && c != K_ডান
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	যদি (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	যদি (p_wmnu)
	    c = wildmenu_translate_চাবি(&ccline, c, &xpc, did_wild_list);

	পূর্ণাঙ্ক চাবি_is_wc = (c == p_wc && চাবি_Typed) || c == p_wcm;
	যদি ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !চাবি_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    যদি (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		যদি (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    ভঙ্গ;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	যদি (চাবি_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#ifdef UNIX
		    || c == intr_char
#endif
		    || c == Ctrl_C))
	{
#ifdef FEAT_EVAL
	    set_vim_var_char(c);  // Set v:char
#endif
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#যদি defined(FEAT_অনুসন্ধান_EXTRA)
	    যদি ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		set_no_hlsearch(TRUE);
#endif
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	end_wildmenu = (!চাবি_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	যদি (end_wildmenu)
	{
	    যদি (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !চাবি_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    যদি (xpc.xp_numfiles != -1)
		(শূন্য)ExpandOne(&xpc, রিক্ত, রিক্ত, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    যদি (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	যদি (p_wmnu)
	    c = wildmenu_process_চাবি(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	যদি (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    যদি (res == CMDLINE_CHANGED)
		গচ্ছ cmdline_changed;
	    অন্যথা যদি (res == CMDLINE_NOT_CHANGED)
		গচ্ছ cmdline_not_changed;
	    অন্যথা যদি (res == GOTO_NORMAL_MODE)
		গচ্ছ returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	যদি (c == cedit_চাবি || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    যদি ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_চাবি_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	অন্যথা
	    c = do_digraph(c);
#endif

	যদি (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!চাবি_Typed || vim_strchr(p_cpo, CPO_ESC) != রিক্ত)))
	{
	    // In Ex mode a backslash escapes a newline.
	    যদি (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		যদি (c == K_KENTER)
		    c = '\n';
	    }
	    অন্যথা
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		যদি (ccheck_abbr(c + ABBR_OFF))
		    গচ্ছ cmdline_changed;
		যদি (!cmd_silent)
		{
		    windgoto(বাৰ্তা_row, cmdline_col_off);
		    out_flush();
		}
		ভঙ্গ;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	যদি ((c == p_wc && !gotesc && চাবি_Typed) || c == p_wcm || c == K_WILD)
	{
	    যদি (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_অনুসন্ধান_EXTRA
		    &is_state.অনুসন্ধান_start
#অন্যথা
		    রিক্ত
#endif
		    );
	    যদি (c == K_WILD)
		--emsg_silent;
	    যদি (res == CMDLINE_CHANGED)
		গচ্ছ cmdline_changed;
	    যদি (c == K_WILD)
		গচ্ছ cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	যদি (c == K_S_TAB && চাবি_Typed)
	{
	    যদি (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		যদি (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    পূর্ণাঙ্ক list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		যদি (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    গচ্ছ cmdline_changed;
	    }
	}

	যদি (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	যদি (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_অনুসন্ধান_EXTRA
	    // Apply search highlighting
	    যদি (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    যদি (চাবি_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, গুনতি, &is_state);
#endif
	    wild_type = 0;
	    গচ্ছ cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	ৱিকল্প (c)
	{
	পক্ষ K_BS:
	পক্ষ Ctrl_H:
	পক্ষ K_DEL:
	পক্ষ K_KDEL:
	পক্ষ Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_অনুসন্ধান_EXTRA
		    , &is_state
#endif
		    );
	    যদি (res == CMDLINE_NOT_CHANGED)
		গচ্ছ cmdline_not_changed;
	    অন্যথা যদি (res == GOTO_NORMAL_MODE)
		গচ্ছ returncmd;		// back to cmd mode
	    গচ্ছ cmdline_changed;

	পক্ষ K_INS:
	পক্ষ K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		গচ্ছ cmdline_not_changed;

	পক্ষ Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : রিক্ত);
		গচ্ছ cmdline_not_changed;

//	case '@':   only in very old vi
	পক্ষ Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		যেতিয়ালৈ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_অনুসন্ধান_EXTRA
		যদি (ccline.cmdlen == 0)
		    is_state.অনুসন্ধান_start = is_state.save_cursor;
#endif
		redrawcmd();
		গচ্ছ cmdline_changed;

#ifdef FEAT_CLIPBOARD
	পক্ষ Ctrl_Y:
		// Copy the modeless selection, if there is one.
		যদি (clip_star.state != SELECT_CLEARED)
		{
		    যদি (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    গচ্ছ cmdline_not_changed;
		}
		ভঙ্গ;
#endif

	পক্ষ ESC:	// get here if p_wc != ESC or when ESC typed twice
	পক্ষ Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		যদি (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    গচ্ছ cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		গচ্ছ returncmd;		// back to cmd mode

	পক্ষ Ctrl_R:			// insert register
		res = cmdline_ঢোকাও_reg(&gotesc);
		যদি (res == GOTO_NORMAL_MODE)
		    গচ্ছ returncmd;
		যদি (res == CMDLINE_CHANGED)
		    গচ্ছ cmdline_changed;
		গচ্ছ cmdline_not_changed;

	পক্ষ Ctrl_D:
		যদি (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    ভঙ্গ;	// Use ^D as normal char instead

		redrawcmd();
		অনুৱর্ত;	// don't do incremental search now

	পক্ষ K_ডান:
	পক্ষ K_S_ডান:
	পক্ষ K_C_ডান:
		কৰা
		{
		    যদি (ccline.cmdpos >= ccline.cmdlen)
			ভঙ্গ;
		    i = cmdline_charsize(ccline.cmdpos);
		    যদি (চাবি_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			ভঙ্গ;
		    ccline.cmdspos += i;
		    যদি (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    অন্যথা
			++ccline.cmdpos;
		}
		যেতিয়ালৈ ((c == K_S_ডান || c == K_C_ডান
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		যদি (has_mbyte)
		    set_cmdspos_cursor();
		গচ্ছ cmdline_not_changed;

	পক্ষ K_বাম:
	পক্ষ K_S_বাম:
	পক্ষ K_C_বাম:
		যদি (ccline.cmdpos == 0)
		    গচ্ছ cmdline_not_changed;
		কৰা
		{
		    --ccline.cmdpos;
		    যদি (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		যেতিয়ালৈ (ccline.cmdpos > 0
			&& (c == K_S_বাম || c == K_C_বাম
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		যদি (has_mbyte)
		    set_cmdspos_cursor();
		গচ্ছ cmdline_not_changed;

	পক্ষ K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		গচ্ছ cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	পক্ষ K_F4:
	    যদি (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		গচ্ছ cmdline_not_changed;
	    }
	    ভঙ্গ;
#endif

	পক্ষ K_MIDDLEDRAG:
	পক্ষ K_MIDDLERELEASE:
		গচ্ছ cmdline_not_changed;	// Ignore mouse

	পক্ষ K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		যদি (!gui.in_use)
#endif
		    যদি (!mouse_has(MOUSE_COMMAND))
			গচ্ছ cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		যদি (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		অন্যথা
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		গচ্ছ cmdline_changed;

#ifdef FEAT_DND
	পক্ষ K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		গচ্ছ cmdline_changed;
#endif

	পক্ষ K_LEFTDRAG:
	পক্ষ K_LEFTRELEASE:
	পক্ষ K_RIGHTDRAG:
	পক্ষ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		যদি (ignore_drag_release)
		    গচ্ছ cmdline_not_changed;
		// FALLTHROUGH
	পক্ষ K_LEFTMOUSE:
	পক্ষ K_RIGHTMOUSE:
		cmdline_বাম_ডান_mouse(c, &ignore_drag_release);
		গচ্ছ cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	পক্ষ K_MOUSEDOWN:
	পক্ষ K_MOUSEUP:
	পক্ষ K_MOUSELEFT:
	পক্ষ K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		যদি (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_বাম
				      : MSCR_ডান);
#endif
		গচ্ছ cmdline_not_changed;

	// Alternate buttons ignored here
	পক্ষ K_X1MOUSE:
	পক্ষ K_X1DRAG:
	পক্ষ K_X1RELEASE:
	পক্ষ K_X2MOUSE:
	পক্ষ K_X2DRAG:
	পক্ষ K_X2RELEASE:
	পক্ষ K_MOUSEMOVE:
		গচ্ছ cmdline_not_changed;

#ifdef FEAT_GUI
	পক্ষ K_LEFTMOUSE_NM:	// mousefocus click, ignored
	পক্ষ K_LEFTRELEASE_NM:
		গচ্ছ cmdline_not_changed;

	পক্ষ K_VER_SCROLLBAR:
		যদি (বাৰ্তা_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		গচ্ছ cmdline_not_changed;

	পক্ষ K_HOR_SCROLLBAR:
		যদি (বাৰ্তা_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_মান);
		    redrawcmd();
		}
		গচ্ছ cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	পক্ষ K_TABLINE:
	পক্ষ K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		যদি (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		গচ্ছ cmdline_not_changed;
#endif

	পক্ষ K_SELECT:	    // end of Select mode mapping - ignore
		গচ্ছ cmdline_not_changed;

	পক্ষ Ctrl_B:	    // begin of command line
	পক্ষ K_HOME:
	পক্ষ K_KHOME:
	পক্ষ K_S_HOME:
	পক্ষ K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		গচ্ছ cmdline_not_changed;

	পক্ষ Ctrl_E:	    // end of command line
	পক্ষ K_END:
	পক্ষ K_KEND:
	পক্ষ K_S_END:
	পক্ষ K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		গচ্ছ cmdline_not_changed;

	পক্ষ Ctrl_A:	    // all matches
		যদি (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		যদি (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    ভঙ্গ;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		গচ্ছ cmdline_changed;

	পক্ষ Ctrl_L:
#ifdef FEAT_অনুসন্ধান_EXTRA
		যদি (may_add_char_to_অনুসন্ধান(firstc, &c, &is_state) == OK)
		    গচ্ছ cmdline_not_changed;
#endif

		// completion: longest common part
		যদি (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    ভঙ্গ;
		গচ্ছ cmdline_changed;

	পক্ষ Ctrl_N:	    // next match
	পক্ষ Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		যদি (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    যদি (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    গচ্ছ cmdline_not_changed;
		}
#endif
		যদি (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    যদি (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ভঙ্গ;
		    গচ্ছ cmdline_changed;
		}
		// FALLTHROUGH
	পক্ষ K_UP:
	পক্ষ K_DOWN:
	পক্ষ K_S_UP:
	পক্ষ K_S_DOWN:
	পক্ষ K_PAGEUP:
	পক্ষ K_KPAGEUP:
	পক্ষ K_PAGEDOWN:
	পক্ষ K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		যদি (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    পূর্ণাঙ্ক	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    যদি (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    গচ্ছ cmdline_not_changed;
		}
#endif
		যদি (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    যদি (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    যদি (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ভঙ্গ;
		    গচ্ছ cmdline_changed;
		}
		অন্যথা
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    যদি (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			গচ্ছ cmdline_changed;
		    }
		    অন্যথা যদি (res == GOTO_NORMAL_MODE)
			গচ্ছ returncmd;
		}
		গচ্ছ cmdline_not_changed;

#ifdef FEAT_অনুসন্ধান_EXTRA
	পক্ষ Ctrl_G:	    // next match
	পক্ষ Ctrl_T:	    // previous match
		যদি (may_adjust_incsearch_highlighting(
					  firstc, গুনতি, &is_state, c) == FAIL)
		    গচ্ছ cmdline_not_changed;
		ভঙ্গ;
#endif

	পক্ষ Ctrl_V:
	পক্ষ Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_mask & MOD_MASK_SHIFT);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    যদি (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			বাৰ্তা_putchar(' ');
			cursorcmd();
		    }
		}

		ভঙ্গ;

#ifdef FEAT_DIGRAPHS
	পক্ষ Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		যদি (c != NUL)
		    ভঙ্গ;

		redrawcmd();
		গচ্ছ cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	পক্ষ Ctrl__:	    // CTRL-_: switch language mode
		যদি (!p_ari)
		    ভঙ্গ;
		cmd_hkmap = !cmd_hkmap;
		গচ্ছ cmdline_not_changed;
#endif

	পক্ষ K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, রিক্ত);
		গচ্ছ cmdline_changed;

	স্ৱভাৱ:
#ifdef UNIX
		যদি (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    গচ্ছ returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		যদি (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		ভঙ্গ;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	যদি (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    গচ্ছ cmdline_changed;

	/*
	 * put the character in the command line
	 */
	যদি (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_চাবি_name(c, mod_mask), -1, TRUE);
	অন্যথা
	{
	    যদি (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    অন্যথা
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	গচ্ছ cmdline_changed;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_changed:
	// Trigger CursorMovedC autocommands.
	যদি (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_অনুসন্ধান_EXTRA
	যদি (!is_state.incsearch_postponed)
	    অনুৱর্ত;
#endif

cmdline_changed:
#ifdef FEAT_অনুসন্ধান_EXTRA
	// If the window changed incremental search state is not valid.
	যদি (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	যদি (xpc.xp_context == EXPAND_NOTHING && (চাবি_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, গুনতি, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	যদি (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != রিক্ত &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    যদি (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	যদি (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	যদি (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    যদি (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    যদি (!event_cmdlineleavepre_triggered)
    {
#ifdef FEAT_EVAL
	set_vim_var_char(c);  // Set v:char
#endif
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#ifdef FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#endif

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    যদি (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = রিক্ত;
    clear_cmdline_orig();

#ifdef FEAT_অনুসন্ধান_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    যদি (ccline.cmdbuff != রিক্ত)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	যদি (ccline.cmdlen && firstc != NUL
		&& (some_চাবি_typed || histype == HIST_অনুসন্ধান))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_অনুসন্ধান ? firstc : NUL);
	    যদি (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	যদি (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    বাৰ্তা_check();
    বাৰ্তা_scroll = save_বাৰ্তা_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    যদি (some_চাবি_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    যদি (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_প্ৰবেশ_METHOD
    যদি (b_im_ptr != রিক্ত && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_status(b_im_ptr);
    im_set_active(FALSE);
#endif
    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    sb_text_end_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--depth;
	যদি (did_save_ccline)
	    restore_cmdline(&save_ccline);
	অন্যথা
	    ccline.cmdbuff = রিক্ত;

	vim_free(prev_cmdbuff);
	ঘূৰাই p;
    }
}

#যদি defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    পূর্ণাঙ্ক		firstc,
    char_u	*prompt,	// command line prompt
    পূর্ণাঙ্ক		attr,		// attributes for prompt
    পূর্ণাঙ্ক		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    পূর্ণাঙ্ক			did_save_ccline = FALSE;
    পূর্ণাঙ্ক			বাৰ্তা_col_save = বাৰ্তা_col;
    পূর্ণাঙ্ক			বাৰ্তা_silent_save = বাৰ্তা_silent;

    যদি (ccline.cmdbuff != রিক্ত)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_FIELD(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.প্ৰবেশ_fn = (firstc == '@');
# endif
    বাৰ্তা_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    যদি (did_save_ccline)
	restore_cmdline(&save_ccline);

    বাৰ্তা_silent = বাৰ্তা_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    যদি (ccline.cmdbuff != রিক্ত)
	বাৰ্তা_col = বাৰ্তা_col_save;

    ঘূৰাই s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    পূর্ণাঙ্ক
check_opt_wim(শূন্য)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		idx = 0;

    বাবে (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    বাবে (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	বাবে (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	যদি (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ঘূৰাই FAIL;
	যদি (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	অন্যথা যদি (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	অন্যথা যদি (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	অন্যথা যদি (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	অন্যথা যদি (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	অন্যথা যদি (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	অন্যথা
	    ঘূৰাই FAIL;
	p += i;
	যদি (*p == NUL)
	    ভঙ্গ;
	যদি (*p == ',')
	{
	    যদি (idx == 3)
		ঘূৰাই FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    যেতিয়ালৈ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    বাবে (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ঘূৰাই OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    পূর্ণাঙ্ক
text_locked(শূন্য)
{
    যদি (cmdwin_type != 0)
	ঘূৰাই TRUE;
    ঘূৰাই textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    শূন্য
text_locked_বাৰ্তা(শূন্য)
{
    emsg(_(get_text_locked_বাৰ্তা()));
}

    ৱর্ণ *
get_text_locked_বাৰ্তা(শূন্য)
{
    যদি (cmdwin_type != 0)
	ঘূৰাই e_invalid_in_cmdline_window;
    ঘূৰাই e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    পূর্ণাঙ্ক
text_or_buf_locked(শূন্য)
{
    যদি (text_locked())
    {
	text_locked_বাৰ্তা();
	ঘূৰাই TRUE;
    }
    ঘূৰাই curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    পূর্ণাঙ্ক
curbuf_locked(শূন্য)
{
    যদি (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	ঘূৰাই TRUE;
    }
    ঘূৰাই allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    পূর্ণাঙ্ক
allbuf_locked(শূন্য)
{
    যদি (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

    স্থির পূর্ণাঙ্ক
cmdline_charsize(পূর্ণাঙ্ক idx)
{
#যদি defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    যদি (cmdline_star > 0)	    // showing '*', always 1 position
	ঘূৰাই 1;
#endif
    ঘূৰাই ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    স্থির শূন্য
set_cmdspos(শূন্য)
{
    যদি (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    অন্যথা
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    স্থির শূন্য
set_cmdspos_cursor(শূন্য)
{
    পূর্ণাঙ্ক		i, m, c;

    set_cmdspos();
    যদি (চাবি_Typed)
    {
	m = cmdline_width * Rows;
	যদি (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    অন্যথা
	m = MAXCOL;
    বাবে (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	যদি (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	যদি ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    ভঙ্গ;
	}
	যদি (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    স্থির শূন্য
correct_cmdspos(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক cells)
{
    যদি ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    পূর্ণাঙ্ক		c,		// normally ':', NUL for ":append"
    শূন্য	*cookie UNUSED,
    পূর্ণাঙ্ক		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    যদি (exec_from_reg && vpeekc() == ':')
	(শূন্য)vgetc();
    ঘূৰাই getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    পূর্ণাঙ্ক		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    শূন্য	*cookie UNUSED,
    পূর্ণাঙ্ক		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	সাৰি_ga;
    char_u	*pend;
    পূর্ণাঙ্ক		startcol = 0;
    পূর্ণাঙ্ক		c1 = 0;
    পূর্ণাঙ্ক		escaped = FALSE;	// CTRL-V typed
    পূর্ণাঙ্ক		vcol = 0;
    char_u	*p;
    পূর্ণাঙ্ক		prev_char;
    পূর্ণাঙ্ক		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    যদি ((বাৰ্তা_col || বাৰ্তা_didout) && promptc != '?')
	বাৰ্তা_putchar('\n');
    যদি (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	যদি (p_prompt)
	    বাৰ্তা_putchar(':');
	যেতিয়ালৈ (indent-- > 0)
	    বাৰ্তা_putchar(' ');
	startcol = বাৰ্তা_col;
    }

    ga_init2(&সাৰি_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    যদি (promptc <= 0)
    {
	vcol = indent;
	যেতিয়ালৈ (indent >= 8)
	{
	    ga_append(&সাৰি_ga, TAB);
	    বাৰ্তা_puts("        ");
	    indent -= 8;
	}
	যেতিয়ালৈ (indent-- > 0)
	{
	    ga_append(&সাৰি_ga, ' ');
	    বাৰ্তা_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_চাবিগুলি;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    যেতিয়ালৈ (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	যদি (ga_grow(&সাৰি_ga, 40) == FAIL)
	    ভঙ্গ;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	যদি (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	অন্যথা
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	যদি (got_int)
	{
	    বাৰ্তা_putchar('\n');
	    ভঙ্গ;
	}

	যদি (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &সাৰি_ga);
	    গচ্ছ redraw;
	}

	যদি (!escaped)
	{
	    // CR typed means "enter", which is NL
	    যদি (c1 == '\r')
		c1 = '\n';

	    যদি (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		যদি (সাৰি_ga.ga_len > 0)
		{
		    যদি (has_mbyte)
		    {
			p = (char_u *)সাৰি_ga.ga_data;
			p[সাৰি_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + সাৰি_ga.ga_len - 1) + 1;
			সাৰি_ga.ga_len -= len;
		    }
		    অন্যথা
			--সাৰি_ga.ga_len;
		    গচ্ছ redraw;
		}
		অনুৱর্ত;
	    }

	    যদি (c1 == Ctrl_U)
	    {
		বাৰ্তা_col = startcol;
		বাৰ্তা_clr_eos();
		সাৰি_ga.ga_len = 0;
		গচ্ছ redraw;
	    }

	    যদি (c1 == Ctrl_T)
	    {
		sw = get_sw_মান(curbuf);
		p = (char_u *)সাৰি_ga.ga_data;
		p[সাৰি_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		যেতিয়ালৈ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (শূন্য)ga_grow(&সাৰি_ga, 2);  // one more for the NUL
		    p = (char_u *)সাৰি_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, সাৰি_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++সাৰি_ga.ga_len;
		}
redraw:
		// redraw the line
		বাৰ্তা_col = startcol;
		vcol = 0;
		p = (char_u *)সাৰি_ga.ga_data;
		p[সাৰি_ga.ga_len] = NUL;
		যেতিয়ালৈ (p < (char_u *)সাৰি_ga.ga_data + সাৰি_ga.ga_len)
		{
		    যদি (*p == TAB)
		    {
			কৰা
			    বাৰ্তা_putchar(' ');
			যেতিয়ালৈ (++vcol % 8);
			++p;
		    }
		    অন্যথা
		    {
			len = mb_ptr2len(p);
			বাৰ্তা_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		বাৰ্তা_clr_eos();
		windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
		অনুৱর্ত;
	    }

	    যদি (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)সাৰি_ga.ga_data;
		যদি (prev_char == '0' || prev_char == '^')
		{
		    যদি (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--সাৰি_ga.ga_len] = NUL;
		}
		অন্যথা
		{
		    p[সাৰি_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    যদি (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_মান(curbuf);
		    }
		}
		যেতিয়ালৈ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, সাৰি_ga.ga_len - (s - p) + 1);
		    --সাৰি_ga.ga_len;
		}
		গচ্ছ add_indent;
	    }

	    যদি (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		অনুৱর্ত;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    যদি (IS_SPECIAL(c1))
		অনুৱর্ত;
	}

	যদি (IS_SPECIAL(c1))
	    c1 = '?';
	যদি (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)সাৰি_ga.ga_data + সাৰি_ga.ga_len);
	অন্যথা
	{
	    len = 1;
	    ((char_u *)সাৰি_ga.ga_data)[সাৰি_ga.ga_len] = c1;
	}
	যদি (c1 == '\n')
	    বাৰ্তা_putchar('\n');
	অন্যথা যদি (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    কৰা
		বাৰ্তা_putchar(' ');
	    যেতিয়ালৈ (++vcol % 8);
	}
	অন্যথা
	{
	    বাৰ্তা_outtrans_len(
		     ((char_u *)সাৰি_ga.ga_data) + সাৰি_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	সাৰি_ga.ga_len += len;
	escaped = FALSE;

	windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
	pend = (char_u *)(সাৰি_ga.ga_data) + সাৰি_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	যদি (সাৰি_ga.ga_len > 0 && pend[-1] == '\n'
		&& (সাৰি_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --সাৰি_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    ভঙ্গ;
	}
    }

    --no_mapping;
    --allow_চাবিগুলি;

    // make following messages go to the next line
    বাৰ্তা_didout = FALSE;
    বাৰ্তা_col = 0;
    যদি (বাৰ্তা_row < Rows - 1)
	++বাৰ্তা_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    যদি (got_int)
	ga_clear(&সাৰি_ga);

    ঘূৰাই (char_u *)সাৰি_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    পূর্ণাঙ্ক
cmdline_overstrike(শূন্য)
{
    ঘূৰাই ccline.overstrike;
}

#যদি defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    পূর্ণাঙ্ক
cmdline_at_end(শূন্য)
{
    ঘূৰাই (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#যদি defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(শূন্য)
{
    যদি (ccline.cmdbuff == রিক্ত || ccline.cmdpos > ccline.cmdlen)
	ঘূৰাই MAXCOL;

    যদি (has_mbyte)
    {
	colnr_T	col;
	পূর্ণাঙ্ক	i = 0;

	বাবে (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ঘূৰাই col;
    }
    অন্যথা
	ঘূৰাই ccline.cmdpos;
}
#endif

#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    স্থির শূন্য
redrawcmd_preedit(শূন্য)
{
    যদি ((State & MODE_CMDLINE)
	    && xic != রিক্ত
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	পূর্ণাঙ্ক	cmdpos = 0;
	পূর্ণাঙ্ক	cmdspos;
	পূর্ণাঙ্ক	old_row;
	পূর্ণাঙ্ক	old_col;
	colnr_T	col;

	old_row = বাৰ্তা_row;
	old_col = বাৰ্তা_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	যদি (has_mbyte)
	{
	    বাবে (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	অন্যথা
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	বাৰ্তা_row = cmdline_row + (cmdspos / cmdline_width);
	বাৰ্তা_col = cmdspos % cmdline_width;
	যদি (বাৰ্তা_row >= Rows)
	    বাৰ্তা_row = Rows - 1;

	বাবে (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    পূর্ণাঙ্ক char_len;
	    পূর্ণাঙ্ক char_attr;

	    char_attr = im_get_feedback_attr(col);
	    যদি (char_attr < 0)
		ভঙ্গ; // end of preedit string

	    যদি (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    অন্যথা
		char_len = 1;

	    বাৰ্তা_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	বাৰ্তা_row = old_row;
	বাৰ্তা_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    স্থির শূন্য
dealloc_cmdbuff(শূন্য)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    স্থির শূন্য
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    যদি (len < 80)
	len = 100;
    অন্যথা
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (পূর্ণাঙ্ক)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
realloc_cmdbuff(পূর্ণাঙ্ক len)
{
    char_u	*p;
    পূর্ণাঙ্ক		plen;

    যদি (len < ccline.cmdbufflen)
	ঘূৰাই OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    যদি (ccline.cmdbuff == রিক্ত)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ঘূৰাই FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    যদি (ccline.xpc != রিক্ত
	    && ccline.xpc->xp_pattern != রিক্ত
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	পূর্ণাঙ্ক i = (পূর্ণাঙ্ক)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	যদি (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    ঘূৰাই OK;
}

#যদি defined(FEAT_ARABIC)
স্থির char_u	*arshape_buf = রিক্ত;

# যদি defined(EXITFREE)
    শূন্য
free_arshape_buf(শূন্য)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    স্থির শূন্য
draw_cmdline(পূর্ণাঙ্ক start, পূর্ণাঙ্ক len)
{
#যদি defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    পূর্ণাঙ্ক		i;

    যদি (cmdline_star > 0)
	বাবে (i = 0; i < len; ++i)
	{
	    বাৰ্তা_putchar('*');
	    যদি (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    অন্যথা
#endif
#ifdef FEAT_ARABIC
	যদি (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	স্থির পূর্ণাঙ্ক	buflen = 0;
	char_u		*p;
	পূর্ণাঙ্ক		j;
	পূর্ণাঙ্ক		newlen = 0;
	পূর্ণাঙ্ক		mb_l;
	পূর্ণাঙ্ক		pc, pc1 = 0;
	পূর্ণাঙ্ক		prev_c = 0;
	পূর্ণাঙ্ক		prev_c1 = 0;
	পূর্ণাঙ্ক		u8c;
	পূর্ণাঙ্ক		u8cc[MAX_MCO];
	পূর্ণাঙ্ক		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	যদি (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    যদি (arshape_buf == রিক্ত)
		ঘূৰাই;	// out of memory
	}

	যদি (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	বাবে (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    যদি (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		যদি (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    যদি (j + mb_l >= start + len)
			nc = NUL;
		    অন্যথা
			nc = utf_ptr2char(p + mb_l);
		}
		অন্যথা
		{
		    // displaying from left to right
		    যদি (j + mb_l >= start + len)
			pc = NUL;
		    অন্যথা
		    {
			পূর্ণাঙ্ক	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, রিক্ত, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		যদি (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    যদি (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    অন্যথা
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	বাৰ্তা_outtrans_len(arshape_buf, newlen);
    }
    অন্যথা
#endif
	বাৰ্তা_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    শূন্য
putcmdline(পূর্ণাঙ্ক c, পূর্ণাঙ্ক shift)
{
    যদি (cmd_silent)
	ঘূৰাই;
    বাৰ্তা_no_more = TRUE;
    বাৰ্তা_putchar(c);
    যদি (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    বাৰ্তা_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    শূন্য
unputcmdline(শূন্য)
{
    যদি (cmd_silent)
	ঘূৰাই;
    বাৰ্তা_no_more = TRUE;
    যদি (ccline.cmdlen == ccline.cmdpos)
	বাৰ্তা_putchar(' ');
    অন্যথা যদি (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    অন্যথা
	draw_cmdline(ccline.cmdpos, 1);
    বাৰ্তা_no_more = FALSE;
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
    পূর্ণাঙ্ক
put_on_cmdline(char_u *str, পূর্ণাঙ্ক len, পূর্ণাঙ্ক redraw)
{
    পূর্ণাঙ্ক		retval;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		m;
    পূর্ণাঙ্ক		c;

    যদি (len < 0)
	len = (পূর্ণাঙ্ক)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    যদি (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    অন্যথা
	retval = OK;
    যদি (retval == OK)
    {
	যদি (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	অন্যথা
	{
	    যদি (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		বাবে (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		বাবে (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		যদি (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		অন্যথা
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    অন্যথা যদি (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	যদি (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    যেতিয়ালৈ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    যদি (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		যদি (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		অন্যথা
		    i = 0;
	    }
#endif
	    যদি (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		বাৰ্তা_col -= i;
		যদি (বাৰ্তা_col < 0)
		{
		    বাৰ্তা_col += cmdline_width;
		    --বাৰ্তা_row;
		}
	    }
	}

	যদি (redraw && !cmd_silent)
	{
	    বাৰ্তা_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    যদি (cmdline_row != i || ccline.overstrike)
		বাৰ্তা_clr_eos();
	    বাৰ্তা_no_more = FALSE;
	}
	যদি (চাবি_Typed)
	{
	    m = cmdline_width * Rows;
	    যদি (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	অন্যথা
	    m = MAXCOL;
	বাবে (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    যদি (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    যদি (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    যদি (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		যদি (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    যদি (redraw)
	বাৰ্তা_check();
    ঘূৰাই retval;
}

স্থির cmdline_info_T	prev_ccline;
স্থির পূর্ণাঙ্ক		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    স্থির শূন্য
save_cmdline(cmdline_info_T *ccp)
{
    যদি (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = রিক্ত;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    স্থির শূন্য
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
    স্থির পূর্ণাঙ্ক
cmdline_paste(
    পূর্ণাঙ্ক regname,
    পূর্ণাঙ্ক literally,	// Insert text literally instead of "as typed"
    পূর্ণাঙ্ক remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    পূর্ণাঙ্ক			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    যদি (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ঘূৰাই FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    সাৰি_breakcheck();
    যদি (got_int)
	ঘূৰাই FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    যদি (i)
    {
	// Got the value of a special register in "arg".
	যদি (arg == রিক্ত)
	    ঘূৰাই FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	যদি (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    পূর্ণাঙ্ক	    len;

	    // Locate start of last word in the cmd buffer.
	    বাবে (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		যদি (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    যদি (!vim_iswordc(mb_ptr2char(w - len)))
			ভঙ্গ;
		    w -= len;
		}
		অন্যথা
		{
		    যদি (!vim_iswordc(w[-1]))
			ভঙ্গ;
		    --w;
		}
	    }
	    len = (পূর্ণাঙ্ক)((ccline.cmdbuff + ccline.cmdpos) - w);
	    যদি (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	যদি (allocated)
	    vim_free(arg);
	ঘূৰাই OK;
    }

    ঘূৰাই cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    শূন্য
cmdline_paste_str(char_u *s, পূর্ণাঙ্ক literally)
{
    পূর্ণাঙ্ক		c, cv;

    যদি (literally)
	put_on_cmdline(s, -1, TRUE);
    অন্যথা
	যেতিয়ালৈ (*s != NUL)
	{
	    cv = *s;
	    যদি (cv == Ctrl_V && s[1])
		++s;
	    যদি (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    অন্যথা
		c = *s++;
	    যদি (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#ifdef UNIX
		    || c == intr_char
#endif
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
    শূন্য
redrawcmdline(শূন্য)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    শূন্য
redrawcmdline_ex(পূর্ণাঙ্ক do_compute_cmdrow)
{
    যদি (cmd_silent)
	ঘূৰাই;
    need_wait_return = FALSE;
    যদি (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    স্থির শূন্য
redrawcmdprompt(শূন্য)
{
    পূর্ণাঙ্ক		i;

    যদি (cmd_silent)
	ঘূৰাই;
    যদি (ccline.cmdfirstc != NUL)
	বাৰ্তা_putchar(ccline.cmdfirstc);
    যদি (ccline.cmdprompt != রিক্ত)
    {
	বাৰ্তা_puts_attr((ৱর্ণ *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = বাৰ্তা_col + (বাৰ্তা_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	যদি (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    অন্যথা
	বাবে (i = ccline.cmdindent; i > 0; --i)
	    বাৰ্তা_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    শূন্য
redrawcmd(শূন্য)
{
    পূর্ণাঙ্ক save_in_echowindow = in_echowindow;

    যদি (cmd_silent)
	ঘূৰাই;

    // when 'incsearch' is set there may be no command line while redrawing
    যদি (ccline.cmdbuff == রিক্ত)
    {
	windgoto(cmdline_row, cmdline_col_off);
	বাৰ্তা_clr_eos();
	ঘূৰাই;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    বাৰ্তা_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    বাৰ্তা_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    বাৰ্তা_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    বাৰ্তা_clr_eos();
    বাৰ্তা_no_more = FALSE;

    set_cmdspos_cursor();
    যদি (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    বাৰ্তা_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    শূন্য
compute_cmdrow(শূন্য)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    যদি (exmode_active || (বাৰ্তা_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    অন্যথা
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    শূন্য
cursorcmd(শূন্য)
{
    যদি (cmd_silent)
	ঘূৰাই;

#ifdef FEAT_RIGHTLEFT
    যদি (cmdmsg_rl)
    {
	বাৰ্তা_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	বাৰ্তা_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	যদি (বাৰ্তা_row <= 0)
	    বাৰ্তা_row = Rows - 1;
    }
    অন্যথা
#endif
    {
	বাৰ্তা_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	বাৰ্তা_col = ccline.cmdspos % cmdline_width;
	যদি (বাৰ্তা_row >= Rows)
	    বাৰ্তা_row = Rows - 1;
    }

    windgoto(বাৰ্তা_row, cmdline_col_off + বাৰ্তা_col);
#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    যদি (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    শূন্য
gotocmdline(পূর্ণাঙ্ক clr)
{
    বাৰ্তা_start();
#ifdef FEAT_RIGHTLEFT
    যদি (cmdmsg_rl)
	বাৰ্তা_col = cmdline_width - 1;
    অন্যথা
#endif
	বাৰ্তা_col = 0;	    // always start in column 0
    যদি (clr)		    // clear the bottom line(s)
	বাৰ্তা_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    স্থির পূর্ণাঙ্ক
ccheck_abbr(পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক spos = 0;

    যদি (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ঘূৰাই FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    যেতিয়ালৈ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    যদি (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    অন্যথা
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ঘূৰাই check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, পূর্ণাঙ্ক what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    পূর্ণাঙ্ক		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    বাবে (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	যদি ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#অন্যথা
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    যদি (what == VSE_SHELL && csh_like_shell() && p != রিক্ত)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#endif

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    যদি (p != রিক্ত && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ঘূৰাই p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    শূন্য
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    যদি (p == রিক্ত)
	ঘূৰাই;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    শূন্য
tilde_replace(
    char_u  *orig_pat,
    পূর্ণাঙ্ক	    অঙ্ক_files,
    char_u  **files)
{
    পূর্ণাঙ্ক	    i;
    char_u  *p;

    যদি (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	বাবে (i = 0; i < অঙ্ক_files; ++i)
	{
	    p = home_replace_save(রিক্ত, files[i]);
	    যদি (p != রিক্ত)
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
get_cmdline_info(শূন্য)
{
    ঘূৰাই &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    স্থির cmdline_info_T *
get_ccline_ptr(শূন্য)
{
    যদি ((State & MODE_CMDLINE) == 0)
	ঘূৰাই রিক্ত;
    যদি (ccline.cmdbuff != রিক্ত)
	ঘূৰাই &ccline;
    যদি (prev_ccline_used && prev_ccline.cmdbuff != রিক্ত)
	ঘূৰাই &prev_ccline;
    ঘূৰাই রিক্ত;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    স্থির পূর্ণাঙ্ক
get_cmdline_type(শূন্য)
{
    cmdline_info_T *p = get_ccline_ptr();

    যদি (p == রিক্ত)
	ঘূৰাই NUL;
    যদি (p->cmdfirstc == NUL)
	ঘূৰাই
#ifdef FEAT_EVAL
	    (p->প্ৰবেশ_fn) ? '@' :
#endif
	    '-';
    ঘূৰাই p->cmdfirstc;
}

#যদি defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    স্থির char_u *
get_cmdline_str(শূন্য)
{
    cmdline_info_T *p;

    যদি (cmdline_star > 0)
	ঘূৰাই রিক্ত;
    p = get_ccline_ptr();
    যদি (p == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    স্থির char_u *
get_cmdline_completion_pattern(শূন্য)
{
    cmdline_info_T *p;
    পূর্ণাঙ্ক		xp_context;

    যদি (cmdline_star > 0)
	ঘূৰাই রিক্ত;

    p = get_ccline_ptr();
    যদি (p == রিক্ত || p->xpc == রিক্ত)
	ঘূৰাই রিক্ত;

    xp_context = p->xpc->xp_context;
    যদি (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    যদি (xp_context == EXPAND_UNSUCCESSFUL)
	ঘূৰাই রিক্ত;

    char_u *compl_pat = p->xpc->xp_pattern;

    যদি (compl_pat == রিক্ত)
	ঘূৰাই রিক্ত;

    ঘূৰাই vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    স্থির char_u *
get_cmdline_completion(শূন্য)
{
    cmdline_info_T	*p;
    পূর্ণাঙ্ক			xp_context;

    যদি (cmdline_star > 0)
	ঘূৰাই রিক্ত;

    p = get_ccline_ptr();
    যদি (p == রিক্ত || p->xpc == রিক্ত)
	ঘূৰাই রিক্ত;

    xp_context = p->xpc->xp_context;
    যদি (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    যদি (xp_context == EXPAND_UNSUCCESSFUL)
	ঘূৰাই রিক্ত;

    ঘূৰাই cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    শূন্য
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    শূন্য
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    শূন্য
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    শূন্য
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_সংখ্যা = p != রিক্ত ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    শূন্য
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != রিক্ত && p->cmdprompt != রিক্ত ?
					    vim_strsave(p->cmdprompt) : রিক্ত;
}

/*
 * "getcmdscreenpos()" function
 */
    শূন্য
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_সংখ্যা = p != রিক্ত ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    শূন্য
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    যদি (rettv->vval.v_string == রিক্ত)
	ঘূৰাই;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    স্থির পূর্ণাঙ্ক
set_cmdline_str(char_u *str, পূর্ণাঙ্ক pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    পূর্ণাঙ্ক		    len;

    যদি (p == রিক্ত)
	ঘূৰাই 1;

    len = (পূর্ণাঙ্ক)STRLEN(str);
    যদি (realloc_cmdbuff(len + 1) != OK)
	ঘূৰাই 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ঘূৰাই 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    স্থির পূর্ণাঙ্ক
set_cmdline_pos(
    পূর্ণাঙ্ক		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    যদি (p == রিক্ত)
	ঘূৰাই 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    যদি (pos < 0)
	new_cmdpos = 0;
    অন্যথা
	new_cmdpos = pos;

    ঘূৰাই 0;
}

// "setcmdline()" function
    শূন্য
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক pos = -1;

    যদি (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)
	ঘূৰাই;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	পূর্ণাঙ্ক error = FALSE;

	pos = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[1], &error) - 1;
	যদি (error)
	    ঘূৰাই;
	যদি (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    ঘূৰাই;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_সংখ্যা = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    শূন্য
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		pos;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    pos = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]) - 1;
    যদি (pos >= 0)
	rettv->vval.v_সংখ্যা = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    পূর্ণাঙ্ক
get_cmdline_firstc(শূন্য)
{
    ঘূৰাই ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    পূর্ণাঙ্ক
get_list_range(char_u **str, পূর্ণাঙ্ক *num1, পূর্ণাঙ্ক *num2)
{
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		first = FALSE;
    varnumber_T	অঙ্ক;

    *str = skipwhite(*str);
    যদি (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, রিক্ত, &len, 0, &অঙ্ক, রিক্ত, 0, FALSE, রিক্ত);
	*str += len;
	// overflow
	যদি (অঙ্ক > INT_MAX)
	    ঘূৰাই FAIL;

	*num1 = (পূর্ণাঙ্ক)অঙ্ক;
	first = TRUE;
    }
    *str = skipwhite(*str);
    যদি (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, রিক্ত, &len, 0, &অঙ্ক, রিক্ত, 0, FALSE, রিক্ত);
	যদি (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    যদি (অঙ্ক > INT_MAX)
		ঘূৰাই FAIL;

	    *num2 = (পূর্ণাঙ্ক)অঙ্ক;
	}
	অন্যথা যদি (!first)		// no number given at all
	    ঘূৰাই FAIL;
    }
    অন্যথা যদি (first)			// only one number given
	*num2 = *num1;
    ঘূৰাই OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    ৱর্ণ *
did_set_cedit(optset_T *args UNUSED)
{
    পূর্ণাঙ্ক n;

    যদি (*p_cedit == NUL)
	cedit_চাবি = -1;
    অন্যথা
    {
	n = string_to_চাবি(p_cedit, FALSE);
	যদি (n == 0 || vim_isprintc(n))
	    ঘূৰাই e_invalid_argument;
	cedit_চাবি = n;
    }
    ঘূৰাই রিক্ত;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    স্থির পূর্ণাঙ্ক
open_cmdwin(শূন্য)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    পূর্ণাঙ্ক			i;
    linenr_T		lnum;
    পূর্ণাঙ্ক			histtype;
    garray_T		winsizes;
    পূর্ণাঙ্ক			save_restart_edit = restart_edit;
    পূর্ণাঙ্ক			save_State = State;
    পূর্ণাঙ্ক			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    পূর্ণাঙ্ক			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    পূর্ণাঙ্ক			save_চাবি_Typed;
#endif
    পূর্ণাঙ্ক			newbuf_status;
    পূর্ণাঙ্ক			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    যদি (text_or_buf_locked()
	    || cmdwin_type != 0
#যদি defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	ঘূৰাই K_IGNORE;
    }
    set_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_size_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    যদি (win_split((পূর্ণাঙ্ক)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ঘূৰাই K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    যদি (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ঘূৰাই Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, রিক্ত, রিক্ত, রিক্ত, ECMD_ONE, ECMD_HIDE,
					রিক্ত);
    cmdwin_valid = win_valid(cmdwin_win);
    যদি (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	যদি (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	যদি (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	যদি (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(রিক্ত, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = রিক্ত;
	beep_flush();
	ga_clear(&winsizes);
	ঘূৰাই Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_মান_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#ifdef FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#endif
#ifdef FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#endif
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_lock;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    যদি (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	যদি (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_ঢোকাও, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_ঢোকাও, TRUE);
	}
	set_option_মান_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    যদি (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	যদি (i >= 0)
	{
	    lnum = 0;
	    কৰা
	    {
		যদি (++i == get_hislen())
		    i = 0;
		যদি (get_histentry(histtype)[i].hisstr != রিক্ত)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    যেতিয়ালৈ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_সাৰি_গুনতি, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_সাৰি_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_active = 0;

    State = MODE_NORMAL;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_result = 0;

    // Trigger CmdwinEnter autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINENTER);
    যদি (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    পূর্ণাঙ্ক save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_চাবি_Typed = চাবি_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    চাবি_Typed = save_চাবি_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = রিক্ত;
    cmdwin_win = রিক্ত;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    যদি (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    অন্যথা
    {
#যদি defined(FEAT_EVAL)
	// autocmds may abort script processing
	যদি (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	যদি (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    যদি (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    যদি (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (পূর্ণাঙ্ক)plen;
		ccline.cmdbufflen = (পূর্ণাঙ্ক)(plen + 1);
		cmdwin_result = CAR;
	    }
	    অন্যথা
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	অন্যথা যদি (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = রিক্ত;
	}
	অন্যথা
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	যদি (ccline.cmdbuff == রিক্ত)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	অন্যথা
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    যদি (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#ifdef FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#endif
	// First go back to the original window.
	wp = curwin;
	set_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	যদি (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	যদি (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(রিক্ত, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	যদি (cmdwin_result == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    set_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#ifdef FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#endif

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    ঘূৰাই cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    পূর্ণাঙ্ক
is_in_cmdwin(শূন্য)
{
    ঘূৰাই cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap UNUSED, char_u *cmd UNUSED)
{
#ifdef FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    যদি (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == রিক্ত)
	ঘূৰাই রিক্ত;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    যদি (l == রিক্ত)
	ঘূৰাই রিক্ত;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ঘূৰাই (char_u *)ga.ga_data;
#অন্যথা
    ঘূৰাই রিক্ত;
#endif
}

#যদি defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    শূন্য
get_user_প্ৰবেশ(
    typval_T	*argvars,
    typval_T	*rettv,
    পূর্ণাঙ্ক		inputdialog,
    পূর্ণাঙ্ক		secret)
{
    char_u	*prompt;
    char_u	*p = রিক্ত;
    পূর্ণাঙ্ক		c;
    char_u	buf[NUMBUFLEN];
    পূর্ণাঙ্ক		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    পূর্ণাঙ্ক		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = রিক্ত;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
    যদি (প্ৰবেশ_busy)
	ঘূৰাই;  // this doesn't work recursively.

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ঘূৰাই;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_প্ৰবেশ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    যদি (no_console_প্ৰবেশ() && !is_not_a_term())
	ঘূৰাই;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    যদি (prompt != রিক্ত)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	যদি (p == রিক্ত)
	    p = prompt;
	অন্যথা
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    বাৰ্তা_start();
	    বাৰ্তা_clr_eos();
	    বাৰ্তা_puts_attr((ৱর্ণ *)prompt, get_echo_attr());
	    বাৰ্তা_didout = FALSE;
	    বাৰ্তা_starthere();
	    *p = c;
	}
	cmdline_row = বাৰ্তা_row;

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    যদি (defstr != রিক্ত)
		stuffReadbuffSpec(defstr);

	    যদি (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		পূর্ণাঙ্ক	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = রিক্ত;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		যদি (xp_name == রিক্ত)
		    ঘূৰাই;

		xp_namelen = (পূর্ণাঙ্ক)STRLEN(xp_name);

		যদি (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ঘূৰাই;
	    }
	}

	যদি (defstr != রিক্ত)
	{
	    পূর্ণাঙ্ক save_ex_normal_busy = ex_normal_busy;
	    পূর্ণাঙ্ক save_vgetc_busy = vgetc_busy;
	    পূর্ণাঙ্ক save_প্ৰবেশ_busy = প্ৰবেশ_busy;

	    প্ৰবেশ_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    প্ৰবেশ_busy = save_প্ৰবেশ_busy;
	}
	যদি (inputdialog && rettv->vval.v_string == রিক্ত
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	বাৰ্তা_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    শূন্য
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    যদি (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	ঘূৰাই;

    পূর্ণাঙ্ক cmd_type = get_cmdline_type();

    যদি (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	চাবি_string[4];

	চাবি_string[0] = K_SPECIAL;
	চাবি_string[1] = KS_EXTRA;
	চাবি_string[2] = KE_WILD;
	চাবি_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(চাবি_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
