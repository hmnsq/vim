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

#അന്തര്ഭാവ "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_താക്കോൽ	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
സ്ഥിതം cmdline_info_T ccline;

#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ	new_cmdpos;	// position set by set_cmdline_pos()
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
സ്ഥിതം പൂർണ്ണസംഖ്യ	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
സ്ഥിതം പൂർണ്ണസംഖ്യ	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

സ്ഥിതം char_u	*getcmdline_int(പൂർണ്ണസംഖ്യ firstc, long എണ്ണം, പൂർണ്ണസംഖ്യ indent, പൂർണ്ണസംഖ്യ clear_ccline);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cmdline_charsize(പൂർണ്ണസംഖ്യ idx);
സ്ഥിതം ശൂന്യം	set_cmdspos(ശൂന്യം);
സ്ഥിതം ശൂന്യം	set_cmdspos_cursor(ശൂന്യം);
സ്ഥിതം ശൂന്യം	correct_cmdspos(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ cells);
സ്ഥിതം ശൂന്യം	dealloc_cmdbuff(ശൂന്യം);
സ്ഥിതം ശൂന്യം	alloc_cmdbuff(size_t len);
സ്ഥിതം ശൂന്യം	draw_cmdline(പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ len);
സ്ഥിതം ശൂന്യം	save_cmdline(cmdline_info_T *ccp);
സ്ഥിതം ശൂന്യം	restore_cmdline(cmdline_info_T *ccp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cmdline_paste(പൂർണ്ണസംഖ്യ regname, പൂർണ്ണസംഖ്യ literally, പൂർണ്ണസംഖ്യ remcr);
സ്ഥിതം ശൂന്യം	redrawcmdprompt(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	ccheck_abbr(പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ	open_cmdwin(ശൂന്യം);
#ifdef FEAT_തിരയൽ_EXTRA
സ്ഥിതം പൂർണ്ണസംഖ്യ	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	cedit_താക്കോൽ = -1;	// key value of 'cedit' option

    സ്ഥിതം ശൂന്യം
trigger_cmd_autocmd(പൂർണ്ണസംഖ്യ typechar, പൂർണ്ണസംഖ്യ evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    സ്ഥിതം ശൂന്യം
abandon_cmdline(ശൂന്യം)
{
    dealloc_cmdbuff();
    എങ്കിൽ (സന്ദേശം_scrolled == 0)
	compute_cmdrow();
    സന്ദേശം("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_തിരയൽ_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
empty_pattern(char_u *p, size_t len, പൂർണ്ണസംഖ്യ delim)
{
    magic_T	magic_val = MAGIC_ON;

    എങ്കിൽ (len > 0)
	(ശൂന്യം) skip_regexp_ex(p, delim, magic_isset(), ശൂന്യ, ശൂന്യ, &magic_val);
    അല്ലെങ്കിൽ
	തിരികെ TRUE;

    തിരികെ empty_pattern_magic(p, len, magic_val);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    ആകുമ്പോൾ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ശൂന്യ)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    തിരികെ len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
തരനിർവചനം ഘടന {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    സ്ഥിതം ശൂന്യം
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

    സ്ഥിതം ശൂന്യം
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
തരനിർവചനം ഘടന {
    pos_T	തിരയൽ_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    പൂർണ്ണസംഖ്യ		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    പൂർണ്ണസംഖ്യ		did_incsearch;
    പൂർണ്ണസംഖ്യ		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    സ്ഥിതം ശൂന്യം
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->തിരയൽ_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    സ്ഥിതം ശൂന്യം
set_തിരയൽ_match(pos_T *t)
{
    t->lnum += തിരയൽ_match_വരികൾ;
    t->col = തിരയൽ_match_endcol;
    എങ്കിൽ (t->lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	t->lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    പൂർണ്ണസംഖ്യ
parse_pattern_and_range(
	pos_T	*incsearch_start,
	പൂർണ്ണസംഖ്യ	*തിരയൽ_delim,
	പൂർണ്ണസംഖ്യ	*skiplen,
	പൂർണ്ണസംഖ്യ	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    പൂർണ്ണസംഖ്യ		delim_optional = FALSE;
    പൂർണ്ണസംഖ്യ		delim;
    പൂർണ്ണസംഖ്യ		use_last_pat;
    magic_T     magic = 0;
    അക്ഷരം	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    തിരയൽ_first_വരി = 0;
    തിരയൽ_last_വരി = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_വരികൾ;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ശൂന്യ);

    എങ്കിൽ (vim_strchr((char_u *)"sgvlu", *cmd) == ശൂന്യ)
	തിരികെ FALSE;

    // Skip over command name to find pattern separator
    വേണ്ടി (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    എങ്കിൽ (*skipwhite(p) == NUL)
	തിരികെ FALSE;

    എങ്കിൽ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	എങ്കിൽ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	എങ്കിൽ (*p == '!')
	    p = skipwhite(p + 1);
	ആകുമ്പോൾ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	എങ്കിൽ (*p == NUL)
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	എങ്കിൽ (*p == '!')
	{
	    p++;
	    എങ്കിൽ (*skipwhite(p) == NUL)
		തിരികെ FALSE;
	}
	എങ്കിൽ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    അല്ലെങ്കിൽ
	തിരികെ FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *തിരയൽ_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), ശൂന്യ, ശൂന്യ, &magic);
    use_last_pat = end == p && *end == delim;

    എങ്കിൽ (end == p && !use_last_pat)
	തിരികെ FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    എങ്കിൽ (!use_last_pat)
    {
	അക്ഷരം c = *end;
	പൂർണ്ണസംഖ്യ empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	എങ്കിൽ (empty)
	    തിരികെ FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (പൂർണ്ണസംഖ്യ)(p - ccline.cmdbuff);
    *patlen = (പൂർണ്ണസംഖ്യ)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    എങ്കിൽ (ea.addr_എണ്ണം > 0)
    {
	പൂർണ്ണസംഖ്യ reverse_match = ea.line2 < ea.line1;
	തിരയൽ_first_വരി = reverse_match ? ea.line2 : ea.line1;
	തിരയൽ_last_വരി = reverse_match ? ea.line1 : ea.line2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	തിരയൽ_first_വരി = തിരയൽ_last_വരി = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    തിരികെ TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_incsearch_highlighting(
	പൂർണ്ണസംഖ്യ		    firstc,
	പൂർണ്ണസംഖ്യ		    *തിരയൽ_delim,
	incsearch_state_T   *is_state,
	പൂർണ്ണസംഖ്യ		    *skiplen,
	പൂർണ്ണസംഖ്യ		    *patlen)
{
    പൂർണ്ണസംഖ്യ retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    എങ്കിൽ (!p_is || cmd_silent)
	തിരികെ FALSE;

    // By default search all lines
    തിരയൽ_first_വരി = 0;
    തിരയൽ_last_വരി = MAXLNUM;

    എങ്കിൽ (firstc == '/' || firstc == '?')
    {
	*തിരയൽ_delim = firstc;
	തിരികെ TRUE;
    }

    എങ്കിൽ (firstc != ':')
	തിരികെ FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->തിരയൽ_start, തിരയൽ_delim,
	    skiplen, patlen);
    --emsg_off;

    തിരികെ retval;
}

    സ്ഥിതം ശൂന്യം
finish_incsearch_highlighting(
	പൂർണ്ണസംഖ്യ gotesc,
	incsearch_state_T *is_state,
	പൂർണ്ണസംഖ്യ call_update_screen)
{
    എങ്കിൽ (!is_state->did_incsearch)
	തിരികെ;

    is_state->did_incsearch = FALSE;
    എങ്കിൽ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!EQUAL_POS(is_state->save_cursor, is_state->തിരയൽ_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->തിരയൽ_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    തിരയൽ_first_വരി = 0;
    തിരയൽ_last_വരി = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    എങ്കിൽ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    സ്ഥിതം ശൂന്യം
may_do_incsearch_highlighting(
	പൂർണ്ണസംഖ്യ		    firstc,
	long		    എണ്ണം,
	incsearch_state_T   *is_state)
{
    പൂർണ്ണസംഖ്യ		skiplen, patlen;
    പൂർണ്ണസംഖ്യ		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    പൂർണ്ണസംഖ്യ		next_char;
    പൂർണ്ണസംഖ്യ		use_last_pat;
    പൂർണ്ണസംഖ്യ		did_do_incsearch = is_state->did_incsearch;
    പൂർണ്ണസംഖ്യ		തിരയൽ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_തിരയൽ_pattern();

    എങ്കിൽ (!do_incsearch_highlighting(firstc, &തിരയൽ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_തിരയൽ_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	എങ്കിൽ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	തിരികെ;
    }

    // If there is a character waiting, search and redraw later.
    എങ്കിൽ (char_avail())
    {
	restore_last_തിരയൽ_pattern();
	is_state->incsearch_postponed = TRUE;
	തിരികെ;
    }
    is_state->incsearch_postponed = FALSE;

    എങ്കിൽ (തിരയൽ_first_വരി == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->തിരയൽ_start;
    അല്ലെങ്കിൽ എങ്കിൽ (തിരയൽ_first_വരി > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	curwin->w_cursor.col = MAXCOL;
    }
    അല്ലെങ്കിൽ
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = തിരയൽ_first_വരി;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    എങ്കിൽ (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ തിരയൽ_flags = തിരയൽ_OPT + തിരയൽ_NOOF + തിരയൽ_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	എങ്കിൽ (!p_hls)
	    തിരയൽ_flags += തിരയൽ_KEEP;
	എങ്കിൽ (തിരയൽ_first_വരി != 0)
	    തിരയൽ_flags += തിരയൽ_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_തിരയൽ(ശൂന്യ, firstc == ':' ? '/' : firstc, തിരയൽ_delim,
				 ccline.cmdbuff + skiplen, patlen, എണ്ണം, തിരയൽ_flags,
# ifdef FEAT_RELTIME
		&sia
# അല്ലെങ്കിൽ
		ശൂന്യ
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	എങ്കിൽ (curwin->w_cursor.lnum < തിരയൽ_first_വരി
		|| curwin->w_cursor.lnum > തിരയൽ_last_വരി)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->തിരയൽ_start;
	}

	// if interrupted while searching, behave like it failed
	എങ്കിൽ (got_int)
	{
	    (ശൂന്യം)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    എങ്കിൽ (found != 0)
	highlight_match = TRUE;		// highlight position
    അല്ലെങ്കിൽ
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    എങ്കിൽ (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_തിരയൽ_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    അല്ലെങ്കിൽ
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    എങ്കിൽ (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	എങ്കിൽ (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, തിരയൽ_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    എങ്കിൽ (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_തിരയൽ_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    എങ്കിൽ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->തിരയൽ_start;
    അല്ലെങ്കിൽ എങ്കിൽ (found != 0)
	curwin->w_cursor = end_pos;

    സന്ദേശം_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_adjust_incsearch_highlighting(
	പൂർണ്ണസംഖ്യ			firstc,
	long			എണ്ണം,
	incsearch_state_T	*is_state,
	പൂർണ്ണസംഖ്യ			c)
{
    പൂർണ്ണസംഖ്യ		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ശൂന്യ;
    char_u	*searchstr;
    char_u	*strcopy = ശൂന്യ;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    പൂർണ്ണസംഖ്യ		തിരയൽ_flags = തിരയൽ_NOOF;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		തിരയൽ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_തിരയൽ_pattern();

    എങ്കിൽ (!do_incsearch_highlighting(firstc, &തിരയൽ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_തിരയൽ_pattern();
	തിരികെ OK;
    }
    എങ്കിൽ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_തിരയൽ_pattern();
	തിരികെ FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (ശൂന്യം)parse_തിരയൽ_pattern_offset(&pat, &patlen_s, തിരയൽ_delim,
				    തിരയൽ_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    എങ്കിൽ (c == Ctrl_G)
    {
	t = is_state->match_end;
	എങ്കിൽ (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (ശൂന്യം)decl(&t);
	തിരയൽ_flags += തിരയൽ_COL;
    }
    അല്ലെങ്കിൽ
	t = is_state->match_start;
    എങ്കിൽ (!p_hls)
	തിരയൽ_flags += തിരയൽ_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ശൂന്യ,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, എണ്ണം, തിരയൽ_flags, RE_തിരയൽ, ശൂന്യ);
    --emsg_off;
    എങ്കിൽ (dircp != ശൂന്യ)
	*dircp = തിരയൽ_delim;
    എങ്കിൽ (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->തിരയൽ_start = match_start;
	എങ്കിൽ (!offset.വരി && (offset.end || off != 0))
	{
	    എങ്കിൽ (offset.end)
	    {
		is_state->തിരയൽ_start = match_end;
		(ശൂന്യം)decl(&is_state->തിരയൽ_start);
	    }
	    ആകുമ്പോൾ (off > 0)
	    {
		എങ്കിൽ (incl(&is_state->തിരയൽ_start) == -1)
		    നിർത്തുക;
		--off;
	    }
	    ആകുമ്പോൾ (off < 0)
	    {
		എങ്കിൽ (decl(&is_state->തിരയൽ_start) == -1)
		    നിർത്തുക;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	എങ്കിൽ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->തിരയൽ_start = t;
	    (ശൂന്യം)decl(&is_state->തിരയൽ_start);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->തിരയൽ_start = t;
	    (ശൂന്യം)incl(&is_state->തിരയൽ_start);
	}
	എങ്കിൽ (LT_POS(t, is_state->തിരയൽ_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->തിരയൽ_start = t;
	    എങ്കിൽ (firstc == '?')
		(ശൂന്യം)incl(&is_state->തിരയൽ_start);
	    അല്ലെങ്കിൽ
		(ശൂന്യം)decl(&is_state->തിരയൽ_start);
	}

	set_തിരയൽ_match(&is_state->match_end);
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
    അല്ലെങ്കിൽ
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_തിരയൽ_pattern();
    തിരികെ FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_add_char_to_തിരയൽ(പൂർണ്ണസംഖ്യ firstc, പൂർണ്ണസംഖ്യ *c, incsearch_state_T *is_state)
{
    പൂർണ്ണസംഖ്യ		skiplen, patlen, തിരയൽ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_തിരയൽ_pattern();

    എങ്കിൽ (!do_incsearch_highlighting(firstc, &തിരയൽ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_തിരയൽ_pattern();
	തിരികെ FAIL;
    }
    restore_last_തിരയൽ_pattern();

    // Add a character from under the cursor for 'incsearch'.
    എങ്കിൽ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	എങ്കിൽ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    എങ്കിൽ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    എങ്കിൽ (*c == തിരയൽ_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ശൂന്യ)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    എങ്കിൽ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		പൂർണ്ണസംഖ്യ save_c = *c;

		ആകുമ്പോൾ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_has_arabic(പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ	    j;
    പൂർണ്ണസംഖ്യ	    mb_l;
    പൂർണ്ണസംഖ്യ	    u8c;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    u8cc[MAX_MCO];

    എങ്കിൽ (!enc_utf8)
	തിരികെ FALSE;

    വേണ്ടി (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	എങ്കിൽ (ARABIC_CHAR(u8c))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

    ശൂന്യം
cmdline_init(ശൂന്യം)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_handle_ctrl_bsl(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ *gotesc)
{
    ++no_mapping;
    ++allow_താക്കോലുകൾ;
    c = plain_vgetc();
    --no_mapping;
    --allow_താക്കോലുകൾ;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    എങ്കിൽ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && താക്കോൽ_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	തിരികെ PROCESS_NEXT_താക്കോൽ;
    }

#ifdef FEAT_EVAL
    എങ്കിൽ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	എങ്കിൽ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	അല്ലെങ്കിൽ
	    new_cmdpos = ccline.cmdpos;

	c = get_വ്യഞ്ജകം_register();
	എങ്കിൽ (c == '=')
	{
	    char_u	*p = ശൂന്യ;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_വ്യഞ്ജകം_വരി();
	    --textlock;

	    എങ്കിൽ (p != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(p);

		എങ്കിൽ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    എങ്കിൽ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    അല്ലെങ്കിൽ
			ccline.cmdpos = new_cmdpos;

		    താക്കോൽ_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    തിരികെ CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	തിരികെ CMDLINE_NOT_CHANGED;
    }
#endif

    എങ്കിൽ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    തിരികെ GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_wildchar_complete(
	പൂർണ്ണസംഖ്യ		c,
	പൂർണ്ണസംഖ്യ		escape,
	പൂർണ്ണസംഖ്യ		*did_wild_list,
	പൂർണ്ണസംഖ്യ		*wim_index_p,
	expand_T	*xp,
	പൂർണ്ണസംഖ്യ		*gotesc,
	പൂർണ്ണസംഖ്യ		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    പൂർണ്ണസംഖ്യ		wim_index = *wim_index_p;
    പൂർണ്ണസംഖ്യ		res;
    പൂർണ്ണസംഖ്യ		cmdpos_before;
    പൂർണ്ണസംഖ്യ		options = WILD_NO_BEEP;
    പൂർണ്ണസംഖ്യ		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    പൂർണ്ണസംഖ്യ		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    എങ്കിൽ (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    എങ്കിൽ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	എങ്കിൽ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (ശൂന്യം)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	എങ്കിൽ (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	അല്ലെങ്കിൽ എങ്കിൽ (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	അല്ലെങ്കിൽ
	    res = OK;	    // don't insert 'wildchar' now
    }
    അല്ലെങ്കിൽ		    // typed p_wc first time
    {
	പൂർണ്ണസംഖ്യ wim_longest = (wim_flags[0] & WIM_LONGEST);
	പൂർണ്ണസംഖ്യ wim_list = (wim_flags[0] & WIM_LIST);
	പൂർണ്ണസംഖ്യ wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	എങ്കിൽ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    എങ്കിൽ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    എങ്കിൽ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    അല്ലെങ്കിൽ
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	എങ്കിൽ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    എങ്കിൽ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	എങ്കിൽ (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	എങ്കിൽ (got_int)
	{
	    (ശൂന്യം)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (ശൂന്യം)ExpandOne(xp, ശൂന്യ, ശൂന്യ, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    തിരികെ CMDLINE_CHANGED;
	}

	// Display matches
	എങ്കിൽ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    എങ്കിൽ (wim_longest)
	    {
		പൂർണ്ണസംഖ്യ found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		പൂർണ്ണസംഖ്യ show_menu = p_wmnu && wim_full;

		എങ്കിൽ (wim_list || show_menu)
		    (ശൂന്യം)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		അല്ലെങ്കിൽ എങ്കിൽ (!found_longest_prefix)
		{
		    പൂർണ്ണസംഖ്യ wim_list_next = (wim_flags[1] & WIM_LIST);
		    പൂർണ്ണസംഖ്യ wim_full_next = (wim_flags[1] & WIM_FULL);
		    പൂർണ്ണസംഖ്യ wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    പൂർണ്ണസംഖ്യ wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    എങ്കിൽ (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			എങ്കിൽ (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			അല്ലെങ്കിൽ
			    (ശൂന്യം)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			എങ്കിൽ (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		എങ്കിൽ (wim_list || show_menu)
		    (ശൂന്യം)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		അല്ലെങ്കിൽ
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    എങ്കിൽ (wim_list)
		*did_wild_list = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    എങ്കിൽ (wim_index < 3)
	++wim_index;
    എങ്കിൽ (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    തിരികെ (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_erase_chars(
	പൂർണ്ണസംഖ്യ c,
	പൂർണ്ണസംഖ്യ indent
#ifdef FEAT_തിരയൽ_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;

    എങ്കിൽ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    എങ്കിൽ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    എങ്കിൽ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    എങ്കിൽ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	എങ്കിൽ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    എങ്കിൽ (c == Ctrl_W)
	    {
		ആകുമ്പോൾ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		ആകുമ്പോൾ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		എങ്കിൽ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == Ctrl_W)
	{
	    ആകുമ്പോൾ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    എങ്കിൽ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		ആകുമ്പോൾ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	അല്ലെങ്കിൽ
	    --p;
	ccline.cmdpos = (പൂർണ്ണസംഖ്യ)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	ആകുമ്പോൾ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_തിരയൽ_EXTRA
	എങ്കിൽ (ccline.cmdlen == 0)
	{
	    isp->തിരയൽ_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ശൂന്യ && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	എങ്കിൽ (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    തിരികെ CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	എങ്കിൽ (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (cmdmsg_rl)
		സന്ദേശം_col = cmdline_width;
	    അല്ലെങ്കിൽ
#endif
		സന്ദേശം_col = 0;
	    സന്ദേശം_putchar(' ');		// delete ':'
	}
#ifdef FEAT_തിരയൽ_EXTRA
	isp->തിരയൽ_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	തിരികെ GOTO_NORMAL_MODE;
    }
    തിരികെ CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    സ്ഥിതം ശൂന്യം
cmdline_toggle_langmap(long *b_im_ptr)
{
    എങ്കിൽ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_നിവേശം_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	എങ്കിൽ (b_im_ptr != ശൂന്യ)
	{
	    എങ്കിൽ (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    അല്ലെങ്കിൽ
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_നിവേശം_METHOD
    അല്ലെങ്കിൽ
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	എങ്കിൽ ((p_imdisable && b_im_ptr != ശൂന്യ)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    എങ്കിൽ (b_im_ptr != ശൂന്യ)
		*b_im_ptr = B_IMODE_NONE;
	}
	അല്ലെങ്കിൽ
	{
	    im_set_active(TRUE);	// Enable input method
	    എങ്കിൽ (b_im_ptr != ശൂന്യ)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    എങ്കിൽ (b_im_ptr != ശൂന്യ)
    {
	എങ്കിൽ (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	അല്ലെങ്കിൽ
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#എങ്കിൽ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_തിരുകുക_reg(പൂർണ്ണസംഖ്യ *gotesc UNUSED)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		literally = FALSE;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_താക്കോലുകൾ;
    i = c = plain_vgetc();	// CTRL-R <char>
    എങ്കിൽ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    എങ്കിൽ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_താക്കോലുകൾ;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    എങ്കിൽ (c == '=')
    {
	എങ്കിൽ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	അല്ലെങ്കിൽ
	    c = get_വ്യഞ്ജകം_register();
    }
#endif
    എങ്കിൽ (c != ESC)	    // use ESC to cancel inserting register
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
	എങ്കിൽ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    തിരികെ GOTO_NORMAL_MODE;
	}
#endif
	താക്കോൽ_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	എങ്കിൽ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    എങ്കിൽ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    അല്ലെങ്കിൽ
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
    തിരികെ literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    സ്ഥിതം ശൂന്യം
cmdline_ഇടത്_വലത്_mouse(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ *ignore_drag_release)
{
    എങ്കിൽ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    അല്ലെങ്കിൽ
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    എങ്കിൽ (!gui.in_use)
#endif
	എങ്കിൽ (!mouse_has(MOUSE_COMMAND))
	    തിരികെ;
#ifdef FEAT_CLIPBOARD
    എങ്കിൽ (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	പൂർണ്ണസംഖ്യ	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	എങ്കിൽ (mouse_model_popup() && button == MOUSE_ഇടത്
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_വലത്;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	തിരികെ;
    }
#endif

    set_cmdspos();
    വേണ്ടി (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	പൂർണ്ണസംഖ്യ	i;

	i = cmdline_charsize(ccline.cmdpos);
	എങ്കിൽ (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    നിർത്തുക;
	എങ്കിൽ (has_mbyte)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_browse_history(
	പൂർണ്ണസംഖ്യ	c,
	പൂർണ്ണസംഖ്യ	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	പൂർണ്ണസംഖ്യ	histype,
	പൂർണ്ണസംഖ്യ	*hiscnt_p,
	expand_T *xp)
{
    പൂർണ്ണസംഖ്യ		orig_hiscnt;
    പൂർണ്ണസംഖ്യ		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    പൂർണ്ണസംഖ്യ		res;

    എങ്കിൽ (get_hislen() == 0 || firstc == NUL)	// no history
	തിരികെ CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    എങ്കിൽ (lookfor == ശൂന്യ)
    {
	എങ്കിൽ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ശൂന്യ)
	    തിരികെ CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    വേണ്ടി (;;)
    {
	// one step backwards
	എങ്കിൽ (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    എങ്കിൽ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    അല്ലെങ്കിൽ എങ്കിൽ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    അല്ലെങ്കിൽ			// at top of list
	    {
		hiscnt = orig_hiscnt;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ    // one step forwards
	{
	    // on last entry, clear the line
	    എങ്കിൽ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		നിർത്തുക;
	    }

	    // not on a history line, nothing to do
	    എങ്കിൽ (hiscnt == get_hislen())
		നിർത്തുക;
	    എങ്കിൽ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    അല്ലെങ്കിൽ
		++hiscnt;
	}
	എങ്കിൽ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ശൂന്യ)
	{
	    hiscnt = orig_hiscnt;
	    നിർത്തുക;
	}
	എങ്കിൽ ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    നിർത്തുക;
    }

    എങ്കിൽ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	പൂർണ്ണസംഖ്യ	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	എങ്കിൽ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	അല്ലെങ്കിൽ
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	എങ്കിൽ (histype == HIST_തിരയൽ
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    പൂർണ്ണസംഖ്യ     i;
	    പൂർണ്ണസംഖ്യ     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    വേണ്ടി (i = 0; i <= 1; ++i)
	    {
		len = 0;
		വേണ്ടി (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    എങ്കിൽ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			എങ്കിൽ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    അല്ലെങ്കിൽ
		    {
			// Escape new sep, unless it is already
			// escaped.
			എങ്കിൽ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    എങ്കിൽ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			എങ്കിൽ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		എങ്കിൽ (i == 0)
		{
		    alloc_cmdbuff(len);
		    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)
		    {
			res = GOTO_NORMAL_MODE;
			പോകുക done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (പൂർണ്ണസംഖ്യ)len;
	}
	അല്ലെങ്കിൽ
	{
	    alloc_cmdbuff(plen);
	    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)
	    {
		res = GOTO_NORMAL_MODE;
		പോകുക done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (പൂർണ്ണസംഖ്യ)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	പോകുക done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    തിരികെ res;
}

/*
 * Initialize the current command-line info.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
init_ccline(പൂർണ്ണസംഖ്യ firstc, പൂർണ്ണസംഖ്യ indent)
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
    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)
    {
	ccline.cmdbufflen = 0;
	തിരികെ FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    എങ്കിൽ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ		  firstc,
    long	  എണ്ണം,	// only used for incremental search
    പൂർണ്ണസംഖ്യ		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    തിരികെ getcmdline_int(firstc, എണ്ണം, indent, TRUE);
}

    സ്ഥിതം char_u *
getcmdline_int(
    പൂർണ്ണസംഖ്യ		firstc,
    long	എണ്ണം UNUSED,	// only used for incremental search
    പൂർണ്ണസംഖ്യ		indent,		// indent for inside conditionals
    പൂർണ്ണസംഖ്യ		clear_ccline)	// clear ccline first
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	depth = 0;	    // call depth
    പൂർണ്ണസംഖ്യ		c = 0;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;
    പൂർണ്ണസംഖ്യ		gotesc = FALSE;		// TRUE when <ESC> just typed
    പൂർണ്ണസംഖ്യ		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ശൂന്യ;	// string to match
    size_t	lookforlen = 0;
    പൂർണ്ണസംഖ്യ		hiscnt;			// current history line in use
    പൂർണ്ണസംഖ്യ		histype;		// history type to be used
#ifdef FEAT_തിരയൽ_EXTRA
    incsearch_state_T	is_state;
#endif
    പൂർണ്ണസംഖ്യ		did_wild_list = FALSE;	// did wild_list() recently
    പൂർണ്ണസംഖ്യ		wim_index = 0;		// index in wim_flags[]
    പൂർണ്ണസംഖ്യ		res;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_scroll = സന്ദേശം_scroll;
    പൂർണ്ണസംഖ്യ		save_State = State;	// remember State when called
    പൂർണ്ണസംഖ്യ		some_താക്കോൽ_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    പൂർണ്ണസംഖ്യ		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = ശൂന്യ;
    buf_T	*b_im_ptr_buf = ശൂന്യ;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    പൂർണ്ണസംഖ്യ		did_save_ccline = FALSE;
    പൂർണ്ണസംഖ്യ		cmdline_type;
    പൂർണ്ണസംഖ്യ		wild_type = 0;
    പൂർണ്ണസംഖ്യ		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    എങ്കിൽ (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    എങ്കിൽ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    എങ്കിൽ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_തിരയൽ_EXTRA
    init_incsearch_state(&is_state);
#endif

    എങ്കിൽ (init_ccline(firstc, indent) != OK)
	പോകുക theend;	// out of memory

    എങ്കിൽ (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	പോകുക theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    അല്ലെങ്കിൽ
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    എങ്കിൽ (!cmd_silent)
    {
	i = സന്ദേശം_scrolled;
	സന്ദേശം_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	സന്ദേശം_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (ccline.നിവേശം_fn)
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
    സന്ദേശം_scroll = FALSE;

    State = MODE_CMDLINE;

    എങ്കിൽ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	എങ്കിൽ (curbuf->b_p_imsearch == B_IMODE_USE_തിരുകുക)
	    b_im_ptr = &curbuf->b_p_iminsert;
	അല്ലെങ്കിൽ
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	എങ്കിൽ (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_നിവേശം_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_നിവേശം_METHOD
    അല്ലെങ്കിൽ എങ്കിൽ (p_imcmdline)
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
    എങ്കിൽ (!debug_mode)
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
    എങ്കിൽ (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    എങ്കിൽ (!cmd_silent && സന്ദേശം_scrolled == 0)
    {
	പൂർണ്ണസംഖ്യ	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	എങ്കിൽ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	എങ്കിൽ (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	trigger_cmdlinechanged = TRUE;
	പൂർണ്ണസംഖ്യ	end_wildmenu;
	പൂർണ്ണസംഖ്യ	prev_cmdpos = ccline.cmdpos;
	പൂർണ്ണസംഖ്യ	skip_pum_redraw = FALSE;

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

	എങ്കിൽ (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_താക്കോൽ_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    എങ്കിൽ (prev_cmdbuff == ശൂന്യ)
		പോകുക returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	എങ്കിൽ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	ചെയ്യുക
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} ആകുമ്പോൾ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	എങ്കിൽ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    എങ്കിൽ (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (ശൂന്യം)ExpandOne(&xpc, ശൂന്യ, ശൂന്യ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	എങ്കിൽ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    തുടരുക;
	}

	എങ്കിൽ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    പൂർണ്ണസംഖ്യ	    clen = ccline.cmdlen;
	    പൂർണ്ണസംഖ്യ	    cc_എണ്ണം = aucmd_cmdline_changed_എണ്ണം;

	    എങ്കിൽ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		എങ്കിൽ (clen == ccline.cmdlen
				    || cc_എണ്ണം != aucmd_cmdline_changed_എണ്ണം)
		    trigger_cmdlinechanged = FALSE;
		പോകുക cmdline_changed;
	    }
	}

	എങ്കിൽ (താക്കോൽ_Typed)
	{
	    some_താക്കോൽ_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (cmd_hkmap)
		c = hkmap(c);
	    എങ്കിൽ (cmdmsg_rl && !താക്കോൽ_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		തിരഞ്ഞെടുപ്പ് (c)
		{
		    അവസ്ഥ K_വലത്:   c = K_ഇടത്; നിർത്തുക;
		    അവസ്ഥ K_S_വലത്: c = K_S_ഇടത്; നിർത്തുക;
		    അവസ്ഥ K_C_വലത്: c = K_C_ഇടത്; നിർത്തുക;
		    അവസ്ഥ K_ഇടത്:    c = K_വലത്; നിർത്തുക;
		    അവസ്ഥ K_S_ഇടത്:  c = K_S_വലത്; നിർത്തുക;
		    അവസ്ഥ K_C_ഇടത്:  c = K_C_വലത്; നിർത്തുക;
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
	എങ്കിൽ ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	എങ്കിൽ (lookfor != ശൂന്യ
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ഇടത് && c != K_വലത്
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	എങ്കിൽ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	എങ്കിൽ (p_wmnu)
	    c = wildmenu_translate_താക്കോൽ(&ccline, c, &xpc, did_wild_list);

	പൂർണ്ണസംഖ്യ താക്കോൽ_is_wc = (c == p_wc && താക്കോൽ_Typed) || c == p_wcm;
	എങ്കിൽ ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !താക്കോൽ_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    എങ്കിൽ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		എങ്കിൽ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    നിർത്തുക;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	എങ്കിൽ (താക്കോൽ_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
	    എങ്കിൽ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!താക്കോൽ_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	എങ്കിൽ (end_wildmenu)
	{
	    എങ്കിൽ (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !താക്കോൽ_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    എങ്കിൽ (xpc.xp_numfiles != -1)
		(ശൂന്യം)ExpandOne(&xpc, ശൂന്യ, ശൂന്യ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    എങ്കിൽ (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	എങ്കിൽ (p_wmnu)
	    c = wildmenu_process_താക്കോൽ(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	എങ്കിൽ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    എങ്കിൽ (res == CMDLINE_CHANGED)
		പോകുക cmdline_changed;
	    അല്ലെങ്കിൽ എങ്കിൽ (res == CMDLINE_NOT_CHANGED)
		പോകുക cmdline_not_changed;
	    അല്ലെങ്കിൽ എങ്കിൽ (res == GOTO_NORMAL_MODE)
		പോകുക returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	എങ്കിൽ (c == cedit_താക്കോൽ || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    എങ്കിൽ ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_താക്കോൽ_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	അല്ലെങ്കിൽ
	    c = do_digraph(c);
#endif

	എങ്കിൽ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!താക്കോൽ_Typed || vim_strchr(p_cpo, CPO_ESC) != ശൂന്യ)))
	{
	    // In Ex mode a backslash escapes a newline.
	    എങ്കിൽ (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		എങ്കിൽ (c == K_KENTER)
		    c = '\n';
	    }
	    അല്ലെങ്കിൽ
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		എങ്കിൽ (ccheck_abbr(c + ABBR_OFF))
		    പോകുക cmdline_changed;
		എങ്കിൽ (!cmd_silent)
		{
		    windgoto(സന്ദേശം_row, cmdline_col_off);
		    out_flush();
		}
		നിർത്തുക;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	എങ്കിൽ ((c == p_wc && !gotesc && താക്കോൽ_Typed) || c == p_wcm || c == K_WILD)
	{
	    എങ്കിൽ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_തിരയൽ_EXTRA
		    &is_state.തിരയൽ_start
#അല്ലെങ്കിൽ
		    ശൂന്യ
#endif
		    );
	    എങ്കിൽ (c == K_WILD)
		--emsg_silent;
	    എങ്കിൽ (res == CMDLINE_CHANGED)
		പോകുക cmdline_changed;
	    എങ്കിൽ (c == K_WILD)
		പോകുക cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	എങ്കിൽ (c == K_S_TAB && താക്കോൽ_Typed)
	{
	    എങ്കിൽ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		എങ്കിൽ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    പൂർണ്ണസംഖ്യ list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		എങ്കിൽ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    പോകുക cmdline_changed;
	    }
	}

	എങ്കിൽ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	എങ്കിൽ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_തിരയൽ_EXTRA
	    // Apply search highlighting
	    എങ്കിൽ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    എങ്കിൽ (താക്കോൽ_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, എണ്ണം, &is_state);
#endif
	    wild_type = 0;
	    പോകുക cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	തിരഞ്ഞെടുപ്പ് (c)
	{
	അവസ്ഥ K_BS:
	അവസ്ഥ Ctrl_H:
	അവസ്ഥ K_DEL:
	അവസ്ഥ K_KDEL:
	അവസ്ഥ Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_തിരയൽ_EXTRA
		    , &is_state
#endif
		    );
	    എങ്കിൽ (res == CMDLINE_NOT_CHANGED)
		പോകുക cmdline_not_changed;
	    അല്ലെങ്കിൽ എങ്കിൽ (res == GOTO_NORMAL_MODE)
		പോകുക returncmd;		// back to cmd mode
	    പോകുക cmdline_changed;

	അവസ്ഥ K_INS:
	അവസ്ഥ K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		പോകുക cmdline_not_changed;

	അവസ്ഥ Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ശൂന്യ);
		പോകുക cmdline_not_changed;

//	case '@':   only in very old vi
	അവസ്ഥ Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		ആകുമ്പോൾ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_തിരയൽ_EXTRA
		എങ്കിൽ (ccline.cmdlen == 0)
		    is_state.തിരയൽ_start = is_state.save_cursor;
#endif
		redrawcmd();
		പോകുക cmdline_changed;

#ifdef FEAT_CLIPBOARD
	അവസ്ഥ Ctrl_Y:
		// Copy the modeless selection, if there is one.
		എങ്കിൽ (clip_star.state != SELECT_CLEARED)
		{
		    എങ്കിൽ (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    പോകുക cmdline_not_changed;
		}
		നിർത്തുക;
#endif

	അവസ്ഥ ESC:	// get here if p_wc != ESC or when ESC typed twice
	അവസ്ഥ Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		എങ്കിൽ (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    പോകുക cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		പോകുക returncmd;		// back to cmd mode

	അവസ്ഥ Ctrl_R:			// insert register
		res = cmdline_തിരുകുക_reg(&gotesc);
		എങ്കിൽ (res == GOTO_NORMAL_MODE)
		    പോകുക returncmd;
		എങ്കിൽ (res == CMDLINE_CHANGED)
		    പോകുക cmdline_changed;
		പോകുക cmdline_not_changed;

	അവസ്ഥ Ctrl_D:
		എങ്കിൽ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    നിർത്തുക;	// Use ^D as normal char instead

		redrawcmd();
		തുടരുക;	// don't do incremental search now

	അവസ്ഥ K_വലത്:
	അവസ്ഥ K_S_വലത്:
	അവസ്ഥ K_C_വലത്:
		ചെയ്യുക
		{
		    എങ്കിൽ (ccline.cmdpos >= ccline.cmdlen)
			നിർത്തുക;
		    i = cmdline_charsize(ccline.cmdpos);
		    എങ്കിൽ (താക്കോൽ_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			നിർത്തുക;
		    ccline.cmdspos += i;
		    എങ്കിൽ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    അല്ലെങ്കിൽ
			++ccline.cmdpos;
		}
		ആകുമ്പോൾ ((c == K_S_വലത് || c == K_C_വലത്
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		എങ്കിൽ (has_mbyte)
		    set_cmdspos_cursor();
		പോകുക cmdline_not_changed;

	അവസ്ഥ K_ഇടത്:
	അവസ്ഥ K_S_ഇടത്:
	അവസ്ഥ K_C_ഇടത്:
		എങ്കിൽ (ccline.cmdpos == 0)
		    പോകുക cmdline_not_changed;
		ചെയ്യുക
		{
		    --ccline.cmdpos;
		    എങ്കിൽ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		ആകുമ്പോൾ (ccline.cmdpos > 0
			&& (c == K_S_ഇടത് || c == K_C_ഇടത്
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		എങ്കിൽ (has_mbyte)
		    set_cmdspos_cursor();
		പോകുക cmdline_not_changed;

	അവസ്ഥ K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		പോകുക cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	അവസ്ഥ K_F4:
	    എങ്കിൽ (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		പോകുക cmdline_not_changed;
	    }
	    നിർത്തുക;
#endif

	അവസ്ഥ K_MIDDLEDRAG:
	അവസ്ഥ K_MIDDLERELEASE:
		പോകുക cmdline_not_changed;	// Ignore mouse

	അവസ്ഥ K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		എങ്കിൽ (!gui.in_use)
#endif
		    എങ്കിൽ (!mouse_has(MOUSE_COMMAND))
			പോകുക cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		എങ്കിൽ (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		അല്ലെങ്കിൽ
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		പോകുക cmdline_changed;

#ifdef FEAT_DND
	അവസ്ഥ K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		പോകുക cmdline_changed;
#endif

	അവസ്ഥ K_LEFTDRAG:
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_RIGHTDRAG:
	അവസ്ഥ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		എങ്കിൽ (ignore_drag_release)
		    പോകുക cmdline_not_changed;
		// FALLTHROUGH
	അവസ്ഥ K_LEFTMOUSE:
	അവസ്ഥ K_RIGHTMOUSE:
		cmdline_ഇടത്_വലത്_mouse(c, &ignore_drag_release);
		പോകുക cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	അവസ്ഥ K_MOUSEDOWN:
	അവസ്ഥ K_MOUSEUP:
	അവസ്ഥ K_MOUSELEFT:
	അവസ്ഥ K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		എങ്കിൽ (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_ഇടത്
				      : MSCR_വലത്);
#endif
		പോകുക cmdline_not_changed;

	// Alternate buttons ignored here
	അവസ്ഥ K_X1MOUSE:
	അവസ്ഥ K_X1DRAG:
	അവസ്ഥ K_X1RELEASE:
	അവസ്ഥ K_X2MOUSE:
	അവസ്ഥ K_X2DRAG:
	അവസ്ഥ K_X2RELEASE:
	അവസ്ഥ K_MOUSEMOVE:
		പോകുക cmdline_not_changed;

#ifdef FEAT_GUI
	അവസ്ഥ K_LEFTMOUSE_NM:	// mousefocus click, ignored
	അവസ്ഥ K_LEFTRELEASE_NM:
		പോകുക cmdline_not_changed;

	അവസ്ഥ K_VER_SCROLLBAR:
		എങ്കിൽ (സന്ദേശം_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		പോകുക cmdline_not_changed;

	അവസ്ഥ K_HOR_SCROLLBAR:
		എങ്കിൽ (സന്ദേശം_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_മൂല്യം);
		    redrawcmd();
		}
		പോകുക cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	അവസ്ഥ K_TABLINE:
	അവസ്ഥ K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		എങ്കിൽ (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		പോകുക cmdline_not_changed;
#endif

	അവസ്ഥ K_SELECT:	    // end of Select mode mapping - ignore
		പോകുക cmdline_not_changed;

	അവസ്ഥ Ctrl_B:	    // begin of command line
	അവസ്ഥ K_HOME:
	അവസ്ഥ K_KHOME:
	അവസ്ഥ K_S_HOME:
	അവസ്ഥ K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		പോകുക cmdline_not_changed;

	അവസ്ഥ Ctrl_E:	    // end of command line
	അവസ്ഥ K_END:
	അവസ്ഥ K_KEND:
	അവസ്ഥ K_S_END:
	അവസ്ഥ K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		പോകുക cmdline_not_changed;

	അവസ്ഥ Ctrl_A:	    // all matches
		എങ്കിൽ (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		എങ്കിൽ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    നിർത്തുക;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		പോകുക cmdline_changed;

	അവസ്ഥ Ctrl_L:
#ifdef FEAT_തിരയൽ_EXTRA
		എങ്കിൽ (may_add_char_to_തിരയൽ(firstc, &c, &is_state) == OK)
		    പോകുക cmdline_not_changed;
#endif

		// completion: longest common part
		എങ്കിൽ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    നിർത്തുക;
		പോകുക cmdline_changed;

	അവസ്ഥ Ctrl_N:	    // next match
	അവസ്ഥ Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		എങ്കിൽ (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    എങ്കിൽ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    പോകുക cmdline_not_changed;
		}
#endif
		എങ്കിൽ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    എങ്കിൽ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			നിർത്തുക;
		    പോകുക cmdline_changed;
		}
		// FALLTHROUGH
	അവസ്ഥ K_UP:
	അവസ്ഥ K_DOWN:
	അവസ്ഥ K_S_UP:
	അവസ്ഥ K_S_DOWN:
	അവസ്ഥ K_PAGEUP:
	അവസ്ഥ K_KPAGEUP:
	അവസ്ഥ K_PAGEDOWN:
	അവസ്ഥ K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		എങ്കിൽ (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    പൂർണ്ണസംഖ്യ	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    എങ്കിൽ (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    പോകുക cmdline_not_changed;
		}
#endif
		എങ്കിൽ (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    എങ്കിൽ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    എങ്കിൽ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			നിർത്തുക;
		    പോകുക cmdline_changed;
		}
		അല്ലെങ്കിൽ
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    എങ്കിൽ (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			പോകുക cmdline_changed;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (res == GOTO_NORMAL_MODE)
			പോകുക returncmd;
		}
		പോകുക cmdline_not_changed;

#ifdef FEAT_തിരയൽ_EXTRA
	അവസ്ഥ Ctrl_G:	    // next match
	അവസ്ഥ Ctrl_T:	    // previous match
		എങ്കിൽ (may_adjust_incsearch_highlighting(
					  firstc, എണ്ണം, &is_state, c) == FAIL)
		    പോകുക cmdline_not_changed;
		നിർത്തുക;
#endif

	അവസ്ഥ Ctrl_V:
	അവസ്ഥ Ctrl_Q:
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
		    എങ്കിൽ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			സന്ദേശം_putchar(' ');
			cursorcmd();
		    }
		}

		നിർത്തുക;

#ifdef FEAT_DIGRAPHS
	അവസ്ഥ Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		എങ്കിൽ (c != NUL)
		    നിർത്തുക;

		redrawcmd();
		പോകുക cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	അവസ്ഥ Ctrl__:	    // CTRL-_: switch language mode
		എങ്കിൽ (!p_ari)
		    നിർത്തുക;
		cmd_hkmap = !cmd_hkmap;
		പോകുക cmdline_not_changed;
#endif

	അവസ്ഥ K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ശൂന്യ);
		പോകുക cmdline_changed;

	സ്ഥിരസ്ഥിതി:
#ifdef UNIX
		എങ്കിൽ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    പോകുക returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		എങ്കിൽ (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		നിർത്തുക;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	എങ്കിൽ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    പോകുക cmdline_changed;

	/*
	 * put the character in the command line
	 */
	എങ്കിൽ (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_താക്കോൽ_name(c, mod_mask), -1, TRUE);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    അല്ലെങ്കിൽ
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	പോകുക cmdline_changed;

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
	എങ്കിൽ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_തിരയൽ_EXTRA
	എങ്കിൽ (!is_state.incsearch_postponed)
	    തുടരുക;
#endif

cmdline_changed:
#ifdef FEAT_തിരയൽ_EXTRA
	// If the window changed incremental search state is not valid.
	എങ്കിൽ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	എങ്കിൽ (xpc.xp_context == EXPAND_NOTHING && (താക്കോൽ_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, എണ്ണം, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	എങ്കിൽ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ശൂന്യ &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    എങ്കിൽ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	എങ്കിൽ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    എങ്കിൽ (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    എങ്കിൽ (!event_cmdlineleavepre_triggered)
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
    എങ്കിൽ (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ശൂന്യ;
    clear_cmdline_orig();

#ifdef FEAT_തിരയൽ_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	എങ്കിൽ (ccline.cmdlen && firstc != NUL
		&& (some_താക്കോൽ_typed || histype == HIST_തിരയൽ))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_തിരയൽ ? firstc : NUL);
	    എങ്കിൽ (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	എങ്കിൽ (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    സന്ദേശം_check();
    സന്ദേശം_scroll = save_സന്ദേശം_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    എങ്കിൽ (some_താക്കോൽ_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    എങ്കിൽ (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_നിവേശം_METHOD
    എങ്കിൽ (b_im_ptr != ശൂന്യ && buf_valid(b_im_ptr_buf)
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
	എങ്കിൽ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	അല്ലെങ്കിൽ
	    ccline.cmdbuff = ശൂന്യ;

	vim_free(prev_cmdbuff);
	തിരികെ p;
    }
}

#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    പൂർണ്ണസംഖ്യ		firstc,
    char_u	*prompt,	// command line prompt
    പൂർണ്ണസംഖ്യ		attr,		// attributes for prompt
    പൂർണ്ണസംഖ്യ		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    പൂർണ്ണസംഖ്യ			did_save_ccline = FALSE;
    പൂർണ്ണസംഖ്യ			സന്ദേശം_col_save = സന്ദേശം_col;
    പൂർണ്ണസംഖ്യ			സന്ദേശം_silent_save = സന്ദേശം_silent;

    എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
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
    ccline.നിവേശം_fn = (firstc == '@');
# endif
    സന്ദേശം_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    എങ്കിൽ (did_save_ccline)
	restore_cmdline(&save_ccline);

    സന്ദേശം_silent = സന്ദേശം_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
	സന്ദേശം_col = സന്ദേശം_col_save;

    തിരികെ s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    പൂർണ്ണസംഖ്യ
check_opt_wim(ശൂന്യം)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		idx = 0;

    വേണ്ടി (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    വേണ്ടി (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	വേണ്ടി (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	എങ്കിൽ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    തിരികെ FAIL;
	എങ്കിൽ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	അല്ലെങ്കിൽ എങ്കിൽ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	അല്ലെങ്കിൽ എങ്കിൽ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	അല്ലെങ്കിൽ എങ്കിൽ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	അല്ലെങ്കിൽ എങ്കിൽ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	അല്ലെങ്കിൽ എങ്കിൽ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;
	p += i;
	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;
	എങ്കിൽ (*p == ',')
	{
	    എങ്കിൽ (idx == 3)
		തിരികെ FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    ആകുമ്പോൾ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    വേണ്ടി (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    തിരികെ OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    പൂർണ്ണസംഖ്യ
text_locked(ശൂന്യം)
{
    എങ്കിൽ (cmdwin_type != 0)
	തിരികെ TRUE;
    തിരികെ textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    ശൂന്യം
text_locked_സന്ദേശം(ശൂന്യം)
{
    emsg(_(get_text_locked_സന്ദേശം()));
}

    അക്ഷരം *
get_text_locked_സന്ദേശം(ശൂന്യം)
{
    എങ്കിൽ (cmdwin_type != 0)
	തിരികെ e_invalid_in_cmdline_window;
    തിരികെ e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    പൂർണ്ണസംഖ്യ
text_or_buf_locked(ശൂന്യം)
{
    എങ്കിൽ (text_locked())
    {
	text_locked_സന്ദേശം();
	തിരികെ TRUE;
    }
    തിരികെ curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    പൂർണ്ണസംഖ്യ
curbuf_locked(ശൂന്യം)
{
    എങ്കിൽ (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	തിരികെ TRUE;
    }
    തിരികെ allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    പൂർണ്ണസംഖ്യ
allbuf_locked(ശൂന്യം)
{
    എങ്കിൽ (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_charsize(പൂർണ്ണസംഖ്യ idx)
{
#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    എങ്കിൽ (cmdline_star > 0)	    // showing '*', always 1 position
	തിരികെ 1;
#endif
    തിരികെ ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    സ്ഥിതം ശൂന്യം
set_cmdspos(ശൂന്യം)
{
    എങ്കിൽ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    അല്ലെങ്കിൽ
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    സ്ഥിതം ശൂന്യം
set_cmdspos_cursor(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i, m, c;

    set_cmdspos();
    എങ്കിൽ (താക്കോൽ_Typed)
    {
	m = cmdline_width * Rows;
	എങ്കിൽ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    അല്ലെങ്കിൽ
	m = MAXCOL;
    വേണ്ടി (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	എങ്കിൽ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	എങ്കിൽ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    നിർത്തുക;
	}
	എങ്കിൽ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    സ്ഥിതം ശൂന്യം
correct_cmdspos(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ cells)
{
    എങ്കിൽ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    പൂർണ്ണസംഖ്യ		c,		// normally ':', NUL for ":append"
    ശൂന്യം	*cookie UNUSED,
    പൂർണ്ണസംഖ്യ		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    എങ്കിൽ (exec_from_reg && vpeekc() == ':')
	(ശൂന്യം)vgetc();
    തിരികെ getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    പൂർണ്ണസംഖ്യ		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    ശൂന്യം	*cookie UNUSED,
    പൂർണ്ണസംഖ്യ		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	വരി_ga;
    char_u	*pend;
    പൂർണ്ണസംഖ്യ		startcol = 0;
    പൂർണ്ണസംഖ്യ		c1 = 0;
    പൂർണ്ണസംഖ്യ		escaped = FALSE;	// CTRL-V typed
    പൂർണ്ണസംഖ്യ		vcol = 0;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		prev_char;
    പൂർണ്ണസംഖ്യ		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    എങ്കിൽ ((സന്ദേശം_col || സന്ദേശം_didout) && promptc != '?')
	സന്ദേശം_putchar('\n');
    എങ്കിൽ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	എങ്കിൽ (p_prompt)
	    സന്ദേശം_putchar(':');
	ആകുമ്പോൾ (indent-- > 0)
	    സന്ദേശം_putchar(' ');
	startcol = സന്ദേശം_col;
    }

    ga_init2(&വരി_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    എങ്കിൽ (promptc <= 0)
    {
	vcol = indent;
	ആകുമ്പോൾ (indent >= 8)
	{
	    ga_append(&വരി_ga, TAB);
	    സന്ദേശം_puts("        ");
	    indent -= 8;
	}
	ആകുമ്പോൾ (indent-- > 0)
	{
	    ga_append(&വരി_ga, ' ');
	    സന്ദേശം_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_താക്കോലുകൾ;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    ആകുമ്പോൾ (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	എങ്കിൽ (ga_grow(&വരി_ga, 40) == FAIL)
	    നിർത്തുക;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	എങ്കിൽ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	അല്ലെങ്കിൽ
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	എങ്കിൽ (got_int)
	{
	    സന്ദേശം_putchar('\n');
	    നിർത്തുക;
	}

	എങ്കിൽ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &വരി_ga);
	    പോകുക redraw;
	}

	എങ്കിൽ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    എങ്കിൽ (c1 == '\r')
		c1 = '\n';

	    എങ്കിൽ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		എങ്കിൽ (വരി_ga.ga_len > 0)
		{
		    എങ്കിൽ (has_mbyte)
		    {
			p = (char_u *)വരി_ga.ga_data;
			p[വരി_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + വരി_ga.ga_len - 1) + 1;
			വരി_ga.ga_len -= len;
		    }
		    അല്ലെങ്കിൽ
			--വരി_ga.ga_len;
		    പോകുക redraw;
		}
		തുടരുക;
	    }

	    എങ്കിൽ (c1 == Ctrl_U)
	    {
		സന്ദേശം_col = startcol;
		സന്ദേശം_clr_eos();
		വരി_ga.ga_len = 0;
		പോകുക redraw;
	    }

	    എങ്കിൽ (c1 == Ctrl_T)
	    {
		sw = get_sw_മൂല്യം(curbuf);
		p = (char_u *)വരി_ga.ga_data;
		p[വരി_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		ആകുമ്പോൾ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (ശൂന്യം)ga_grow(&വരി_ga, 2);  // one more for the NUL
		    p = (char_u *)വരി_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, വരി_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++വരി_ga.ga_len;
		}
redraw:
		// redraw the line
		സന്ദേശം_col = startcol;
		vcol = 0;
		p = (char_u *)വരി_ga.ga_data;
		p[വരി_ga.ga_len] = NUL;
		ആകുമ്പോൾ (p < (char_u *)വരി_ga.ga_data + വരി_ga.ga_len)
		{
		    എങ്കിൽ (*p == TAB)
		    {
			ചെയ്യുക
			    സന്ദേശം_putchar(' ');
			ആകുമ്പോൾ (++vcol % 8);
			++p;
		    }
		    അല്ലെങ്കിൽ
		    {
			len = mb_ptr2len(p);
			സന്ദേശം_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		സന്ദേശം_clr_eos();
		windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
		തുടരുക;
	    }

	    എങ്കിൽ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)വരി_ga.ga_data;
		എങ്കിൽ (prev_char == '0' || prev_char == '^')
		{
		    എങ്കിൽ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--വരി_ga.ga_len] = NUL;
		}
		അല്ലെങ്കിൽ
		{
		    p[വരി_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    എങ്കിൽ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_മൂല്യം(curbuf);
		    }
		}
		ആകുമ്പോൾ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, വരി_ga.ga_len - (s - p) + 1);
		    --വരി_ga.ga_len;
		}
		പോകുക add_indent;
	    }

	    എങ്കിൽ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		തുടരുക;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    എങ്കിൽ (IS_SPECIAL(c1))
		തുടരുക;
	}

	എങ്കിൽ (IS_SPECIAL(c1))
	    c1 = '?';
	എങ്കിൽ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)വരി_ga.ga_data + വരി_ga.ga_len);
	അല്ലെങ്കിൽ
	{
	    len = 1;
	    ((char_u *)വരി_ga.ga_data)[വരി_ga.ga_len] = c1;
	}
	എങ്കിൽ (c1 == '\n')
	    സന്ദേശം_putchar('\n');
	അല്ലെങ്കിൽ എങ്കിൽ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    ചെയ്യുക
		സന്ദേശം_putchar(' ');
	    ആകുമ്പോൾ (++vcol % 8);
	}
	അല്ലെങ്കിൽ
	{
	    സന്ദേശം_outtrans_len(
		     ((char_u *)വരി_ga.ga_data) + വരി_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	വരി_ga.ga_len += len;
	escaped = FALSE;

	windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
	pend = (char_u *)(വരി_ga.ga_data) + വരി_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	എങ്കിൽ (വരി_ga.ga_len > 0 && pend[-1] == '\n'
		&& (വരി_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --വരി_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    നിർത്തുക;
	}
    }

    --no_mapping;
    --allow_താക്കോലുകൾ;

    // make following messages go to the next line
    സന്ദേശം_didout = FALSE;
    സന്ദേശം_col = 0;
    എങ്കിൽ (സന്ദേശം_row < Rows - 1)
	++സന്ദേശം_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    എങ്കിൽ (got_int)
	ga_clear(&വരി_ga);

    തിരികെ (char_u *)വരി_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    പൂർണ്ണസംഖ്യ
cmdline_overstrike(ശൂന്യം)
{
    തിരികെ ccline.overstrike;
}

#എങ്കിൽ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    പൂർണ്ണസംഖ്യ
cmdline_at_end(ശൂന്യം)
{
    തിരികെ (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#എങ്കിൽ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(ശൂന്യം)
{
    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ || ccline.cmdpos > ccline.cmdlen)
	തിരികെ MAXCOL;

    എങ്കിൽ (has_mbyte)
    {
	colnr_T	col;
	പൂർണ്ണസംഖ്യ	i = 0;

	വേണ്ടി (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	തിരികെ col;
    }
    അല്ലെങ്കിൽ
	തിരികെ ccline.cmdpos;
}
#endif

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    സ്ഥിതം ശൂന്യം
redrawcmd_preedit(ശൂന്യം)
{
    എങ്കിൽ ((State & MODE_CMDLINE)
	    && xic != ശൂന്യ
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	പൂർണ്ണസംഖ്യ	cmdpos = 0;
	പൂർണ്ണസംഖ്യ	cmdspos;
	പൂർണ്ണസംഖ്യ	old_row;
	പൂർണ്ണസംഖ്യ	old_col;
	colnr_T	col;

	old_row = സന്ദേശം_row;
	old_col = സന്ദേശം_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	എങ്കിൽ (has_mbyte)
	{
	    വേണ്ടി (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	സന്ദേശം_row = cmdline_row + (cmdspos / cmdline_width);
	സന്ദേശം_col = cmdspos % cmdline_width;
	എങ്കിൽ (സന്ദേശം_row >= Rows)
	    സന്ദേശം_row = Rows - 1;

	വേണ്ടി (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    പൂർണ്ണസംഖ്യ char_len;
	    പൂർണ്ണസംഖ്യ char_attr;

	    char_attr = im_get_feedback_attr(col);
	    എങ്കിൽ (char_attr < 0)
		നിർത്തുക; // end of preedit string

	    എങ്കിൽ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    അല്ലെങ്കിൽ
		char_len = 1;

	    സന്ദേശം_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	സന്ദേശം_row = old_row;
	സന്ദേശം_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    സ്ഥിതം ശൂന്യം
dealloc_cmdbuff(ശൂന്യം)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    സ്ഥിതം ശൂന്യം
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    എങ്കിൽ (len < 80)
	len = 100;
    അല്ലെങ്കിൽ
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (പൂർണ്ണസംഖ്യ)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
realloc_cmdbuff(പൂർണ്ണസംഖ്യ len)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		plen;

    എങ്കിൽ (len < ccline.cmdbufflen)
	തിരികെ OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	തിരികെ FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    എങ്കിൽ (ccline.xpc != ശൂന്യ
	    && ccline.xpc->xp_pattern != ശൂന്യ
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	പൂർണ്ണസംഖ്യ i = (പൂർണ്ണസംഖ്യ)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	എങ്കിൽ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_ARABIC)
സ്ഥിതം char_u	*arshape_buf = ശൂന്യ;

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_arshape_buf(ശൂന്യം)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    സ്ഥിതം ശൂന്യം
draw_cmdline(പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ len)
{
#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (cmdline_star > 0)
	വേണ്ടി (i = 0; i < len; ++i)
	{
	    സന്ദേശം_putchar('*');
	    എങ്കിൽ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_ARABIC
	എങ്കിൽ (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	സ്ഥിതം പൂർണ്ണസംഖ്യ	buflen = 0;
	char_u		*p;
	പൂർണ്ണസംഖ്യ		j;
	പൂർണ്ണസംഖ്യ		newlen = 0;
	പൂർണ്ണസംഖ്യ		mb_l;
	പൂർണ്ണസംഖ്യ		pc, pc1 = 0;
	പൂർണ്ണസംഖ്യ		prev_c = 0;
	പൂർണ്ണസംഖ്യ		prev_c1 = 0;
	പൂർണ്ണസംഖ്യ		u8c;
	പൂർണ്ണസംഖ്യ		u8cc[MAX_MCO];
	പൂർണ്ണസംഖ്യ		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	എങ്കിൽ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    എങ്കിൽ (arshape_buf == ശൂന്യ)
		തിരികെ;	// out of memory
	}

	എങ്കിൽ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	വേണ്ടി (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    എങ്കിൽ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		എങ്കിൽ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    എങ്കിൽ (j + mb_l >= start + len)
			nc = NUL;
		    അല്ലെങ്കിൽ
			nc = utf_ptr2char(p + mb_l);
		}
		അല്ലെങ്കിൽ
		{
		    // displaying from left to right
		    എങ്കിൽ (j + mb_l >= start + len)
			pc = NUL;
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ശൂന്യ, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		എങ്കിൽ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    എങ്കിൽ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	സന്ദേശം_outtrans_len(arshape_buf, newlen);
    }
    അല്ലെങ്കിൽ
#endif
	സന്ദേശം_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    ശൂന്യം
putcmdline(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ shift)
{
    എങ്കിൽ (cmd_silent)
	തിരികെ;
    സന്ദേശം_no_more = TRUE;
    സന്ദേശം_putchar(c);
    എങ്കിൽ (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    സന്ദേശം_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    ശൂന്യം
unputcmdline(ശൂന്യം)
{
    എങ്കിൽ (cmd_silent)
	തിരികെ;
    സന്ദേശം_no_more = TRUE;
    എങ്കിൽ (ccline.cmdlen == ccline.cmdpos)
	സന്ദേശം_putchar(' ');
    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    അല്ലെങ്കിൽ
	draw_cmdline(ccline.cmdpos, 1);
    സന്ദേശം_no_more = FALSE;
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
    പൂർണ്ണസംഖ്യ
put_on_cmdline(char_u *str, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ redraw)
{
    പൂർണ്ണസംഖ്യ		retval;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		m;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (len < 0)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    എങ്കിൽ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    അല്ലെങ്കിൽ
	retval = OK;
    എങ്കിൽ (retval == OK)
    {
	എങ്കിൽ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		വേണ്ടി (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		വേണ്ടി (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		എങ്കിൽ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		അല്ലെങ്കിൽ
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	എങ്കിൽ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    ആകുമ്പോൾ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    എങ്കിൽ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		എങ്കിൽ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		അല്ലെങ്കിൽ
		    i = 0;
	    }
#endif
	    എങ്കിൽ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		സന്ദേശം_col -= i;
		എങ്കിൽ (സന്ദേശം_col < 0)
		{
		    സന്ദേശം_col += cmdline_width;
		    --സന്ദേശം_row;
		}
	    }
	}

	എങ്കിൽ (redraw && !cmd_silent)
	{
	    സന്ദേശം_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    എങ്കിൽ (cmdline_row != i || ccline.overstrike)
		സന്ദേശം_clr_eos();
	    സന്ദേശം_no_more = FALSE;
	}
	എങ്കിൽ (താക്കോൽ_Typed)
	{
	    m = cmdline_width * Rows;
	    എങ്കിൽ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	അല്ലെങ്കിൽ
	    m = MAXCOL;
	വേണ്ടി (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    എങ്കിൽ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    എങ്കിൽ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    എങ്കിൽ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		എങ്കിൽ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    എങ്കിൽ (redraw)
	സന്ദേശം_check();
    തിരികെ retval;
}

സ്ഥിതം cmdline_info_T	prev_ccline;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    സ്ഥിതം ശൂന്യം
save_cmdline(cmdline_info_T *ccp)
{
    എങ്കിൽ (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ശൂന്യ;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_paste(
    പൂർണ്ണസംഖ്യ regname,
    പൂർണ്ണസംഖ്യ literally,	// Insert text literally instead of "as typed"
    പൂർണ്ണസംഖ്യ remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    പൂർണ്ണസംഖ്യ			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    എങ്കിൽ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	തിരികെ FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    വരി_breakcheck();
    എങ്കിൽ (got_int)
	തിരികെ FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    എങ്കിൽ (i)
    {
	// Got the value of a special register in "arg".
	എങ്കിൽ (arg == ശൂന്യ)
	    തിരികെ FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	എങ്കിൽ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    പൂർണ്ണസംഖ്യ	    len;

	    // Locate start of last word in the cmd buffer.
	    വേണ്ടി (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		എങ്കിൽ (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    എങ്കിൽ (!vim_iswordc(mb_ptr2char(w - len)))
			നിർത്തുക;
		    w -= len;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (!vim_iswordc(w[-1]))
			നിർത്തുക;
		    --w;
		}
	    }
	    len = (പൂർണ്ണസംഖ്യ)((ccline.cmdbuff + ccline.cmdpos) - w);
	    എങ്കിൽ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	എങ്കിൽ (allocated)
	    vim_free(arg);
	തിരികെ OK;
    }

    തിരികെ cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    ശൂന്യം
cmdline_paste_str(char_u *s, പൂർണ്ണസംഖ്യ literally)
{
    പൂർണ്ണസംഖ്യ		c, cv;

    എങ്കിൽ (literally)
	put_on_cmdline(s, -1, TRUE);
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ (*s != NUL)
	{
	    cv = *s;
	    എങ്കിൽ (cv == Ctrl_V && s[1])
		++s;
	    എങ്കിൽ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    അല്ലെങ്കിൽ
		c = *s++;
	    എങ്കിൽ (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    ശൂന്യം
redrawcmdline(ശൂന്യം)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    ശൂന്യം
redrawcmdline_ex(പൂർണ്ണസംഖ്യ do_compute_cmdrow)
{
    എങ്കിൽ (cmd_silent)
	തിരികെ;
    need_wait_return = FALSE;
    എങ്കിൽ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    സ്ഥിതം ശൂന്യം
redrawcmdprompt(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (cmd_silent)
	തിരികെ;
    എങ്കിൽ (ccline.cmdfirstc != NUL)
	സന്ദേശം_putchar(ccline.cmdfirstc);
    എങ്കിൽ (ccline.cmdprompt != ശൂന്യ)
    {
	സന്ദേശം_puts_attr((അക്ഷരം *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = സന്ദേശം_col + (സന്ദേശം_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	എങ്കിൽ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    അല്ലെങ്കിൽ
	വേണ്ടി (i = ccline.cmdindent; i > 0; --i)
	    സന്ദേശം_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    ശൂന്യം
redrawcmd(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ save_in_echowindow = in_echowindow;

    എങ്കിൽ (cmd_silent)
	തിരികെ;

    // when 'incsearch' is set there may be no command line while redrawing
    എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)
    {
	windgoto(cmdline_row, cmdline_col_off);
	സന്ദേശം_clr_eos();
	തിരികെ;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    സന്ദേശം_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    സന്ദേശം_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    സന്ദേശം_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    സന്ദേശം_clr_eos();
    സന്ദേശം_no_more = FALSE;

    set_cmdspos_cursor();
    എങ്കിൽ (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    സന്ദേശം_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    ശൂന്യം
compute_cmdrow(ശൂന്യം)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    എങ്കിൽ (exmode_active || (സന്ദേശം_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    അല്ലെങ്കിൽ
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    ശൂന്യം
cursorcmd(ശൂന്യം)
{
    എങ്കിൽ (cmd_silent)
	തിരികെ;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (cmdmsg_rl)
    {
	സന്ദേശം_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	സന്ദേശം_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	എങ്കിൽ (സന്ദേശം_row <= 0)
	    സന്ദേശം_row = Rows - 1;
    }
    അല്ലെങ്കിൽ
#endif
    {
	സന്ദേശം_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	സന്ദേശം_col = ccline.cmdspos % cmdline_width;
	എങ്കിൽ (സന്ദേശം_row >= Rows)
	    സന്ദേശം_row = Rows - 1;
    }

    windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    എങ്കിൽ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    ശൂന്യം
gotocmdline(പൂർണ്ണസംഖ്യ clr)
{
    സന്ദേശം_start();
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (cmdmsg_rl)
	സന്ദേശം_col = cmdline_width - 1;
    അല്ലെങ്കിൽ
#endif
	സന്ദേശം_col = 0;	    // always start in column 0
    എങ്കിൽ (clr)		    // clear the bottom line(s)
	സന്ദേശം_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ccheck_abbr(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ spos = 0;

    എങ്കിൽ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	തിരികെ FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    ആകുമ്പോൾ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    എങ്കിൽ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    അല്ലെങ്കിൽ
	// check abbreviation from the beginning of the commandline
	spos = 0;

    തിരികെ check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, പൂർണ്ണസംഖ്യ what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    പൂർണ്ണസംഖ്യ		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    വേണ്ടി (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	എങ്കിൽ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#അല്ലെങ്കിൽ
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    എങ്കിൽ (what == VSE_SHELL && csh_like_shell() && p != ശൂന്യ)
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
    എങ്കിൽ (p != ശൂന്യ && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    തിരികെ p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    ശൂന്യം
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    ശൂന്യം
tilde_replace(
    char_u  *orig_pat,
    പൂർണ്ണസംഖ്യ	    അക്കം_files,
    char_u  **files)
{
    പൂർണ്ണസംഖ്യ	    i;
    char_u  *p;

    എങ്കിൽ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	വേണ്ടി (i = 0; i < അക്കം_files; ++i)
	{
	    p = home_replace_save(ശൂന്യ, files[i]);
	    എങ്കിൽ (p != ശൂന്യ)
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
get_cmdline_info(ശൂന്യം)
{
    തിരികെ &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    സ്ഥിതം cmdline_info_T *
get_ccline_ptr(ശൂന്യം)
{
    എങ്കിൽ ((State & MODE_CMDLINE) == 0)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (ccline.cmdbuff != ശൂന്യ)
	തിരികെ &ccline;
    എങ്കിൽ (prev_ccline_used && prev_ccline.cmdbuff != ശൂന്യ)
	തിരികെ &prev_ccline;
    തിരികെ ശൂന്യ;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_cmdline_type(ശൂന്യം)
{
    cmdline_info_T *p = get_ccline_ptr();

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ NUL;
    എങ്കിൽ (p->cmdfirstc == NUL)
	തിരികെ
#ifdef FEAT_EVAL
	    (p->നിവേശം_fn) ? '@' :
#endif
	    '-';
    തിരികെ p->cmdfirstc;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    സ്ഥിതം char_u *
get_cmdline_str(ശൂന്യം)
{
    cmdline_info_T *p;

    എങ്കിൽ (cmdline_star > 0)
	തിരികെ ശൂന്യ;
    p = get_ccline_ptr();
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    സ്ഥിതം char_u *
get_cmdline_completion_pattern(ശൂന്യം)
{
    cmdline_info_T *p;
    പൂർണ്ണസംഖ്യ		xp_context;

    എങ്കിൽ (cmdline_star > 0)
	തിരികെ ശൂന്യ;

    p = get_ccline_ptr();
    എങ്കിൽ (p == ശൂന്യ || p->xpc == ശൂന്യ)
	തിരികെ ശൂന്യ;

    xp_context = p->xpc->xp_context;
    എങ്കിൽ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    എങ്കിൽ (xp_context == EXPAND_UNSUCCESSFUL)
	തിരികെ ശൂന്യ;

    char_u *compl_pat = p->xpc->xp_pattern;

    എങ്കിൽ (compl_pat == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരികെ vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    സ്ഥിതം char_u *
get_cmdline_completion(ശൂന്യം)
{
    cmdline_info_T	*p;
    പൂർണ്ണസംഖ്യ			xp_context;

    എങ്കിൽ (cmdline_star > 0)
	തിരികെ ശൂന്യ;

    p = get_ccline_ptr();
    എങ്കിൽ (p == ശൂന്യ || p->xpc == ശൂന്യ)
	തിരികെ ശൂന്യ;

    xp_context = p->xpc->xp_context;
    എങ്കിൽ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    എങ്കിൽ (xp_context == EXPAND_UNSUCCESSFUL)
	തിരികെ ശൂന്യ;

    തിരികെ cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    ശൂന്യം
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    ശൂന്യം
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    ശൂന്യം
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    ശൂന്യം
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_സംഖ്യ = p != ശൂന്യ ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    ശൂന്യം
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ശൂന്യ && p->cmdprompt != ശൂന്യ ?
					    vim_strsave(p->cmdprompt) : ശൂന്യ;
}

/*
 * "getcmdscreenpos()" function
 */
    ശൂന്യം
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_സംഖ്യ = p != ശൂന്യ ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    ശൂന്യം
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    എങ്കിൽ (rettv->vval.v_string == ശൂന്യ)
	തിരികെ;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_cmdline_str(char_u *str, പൂർണ്ണസംഖ്യ pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    പൂർണ്ണസംഖ്യ		    len;

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ 1;

    len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
    എങ്കിൽ (realloc_cmdbuff(len + 1) != OK)
	തിരികെ 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    തിരികെ 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_cmdline_pos(
    പൂർണ്ണസംഖ്യ		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    എങ്കിൽ (pos < 0)
	new_cmdpos = 0;
    അല്ലെങ്കിൽ
	new_cmdpos = pos;

    തിരികെ 0;
}

// "setcmdline()" function
    ശൂന്യം
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ pos = -1;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;

	pos = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], &error) - 1;
	എങ്കിൽ (error)
	    തിരികെ;
	എങ്കിൽ (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    തിരികെ;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_സംഖ്യ = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    ശൂന്യം
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		pos;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    pos = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]) - 1;
    എങ്കിൽ (pos >= 0)
	rettv->vval.v_സംഖ്യ = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    പൂർണ്ണസംഖ്യ
get_cmdline_firstc(ശൂന്യം)
{
    തിരികെ ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    പൂർണ്ണസംഖ്യ
get_list_range(char_u **str, പൂർണ്ണസംഖ്യ *num1, പൂർണ്ണസംഖ്യ *num2)
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		first = FALSE;
    varnumber_T	അക്കം;

    *str = skipwhite(*str);
    എങ്കിൽ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ശൂന്യ, &len, 0, &അക്കം, ശൂന്യ, 0, FALSE, ശൂന്യ);
	*str += len;
	// overflow
	എങ്കിൽ (അക്കം > INT_MAX)
	    തിരികെ FAIL;

	*num1 = (പൂർണ്ണസംഖ്യ)അക്കം;
	first = TRUE;
    }
    *str = skipwhite(*str);
    എങ്കിൽ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ശൂന്യ, &len, 0, &അക്കം, ശൂന്യ, 0, FALSE, ശൂന്യ);
	എങ്കിൽ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    എങ്കിൽ (അക്കം > INT_MAX)
		തിരികെ FAIL;

	    *num2 = (പൂർണ്ണസംഖ്യ)അക്കം;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!first)		// no number given at all
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (first)			// only one number given
	*num2 = *num1;
    തിരികെ OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    അക്ഷരം *
did_set_cedit(optset_T *args UNUSED)
{
    പൂർണ്ണസംഖ്യ n;

    എങ്കിൽ (*p_cedit == NUL)
	cedit_താക്കോൽ = -1;
    അല്ലെങ്കിൽ
    {
	n = string_to_താക്കോൽ(p_cedit, FALSE);
	എങ്കിൽ (n == 0 || vim_isprintc(n))
	    തിരികെ e_invalid_argument;
	cedit_താക്കോൽ = n;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
open_cmdwin(ശൂന്യം)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    പൂർണ്ണസംഖ്യ			i;
    linenr_T		lnum;
    പൂർണ്ണസംഖ്യ			histtype;
    garray_T		winsizes;
    പൂർണ്ണസംഖ്യ			save_restart_edit = restart_edit;
    പൂർണ്ണസംഖ്യ			save_State = State;
    പൂർണ്ണസംഖ്യ			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    പൂർണ്ണസംഖ്യ			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    പൂർണ്ണസംഖ്യ			save_താക്കോൽ_Typed;
#endif
    പൂർണ്ണസംഖ്യ			newbuf_status;
    പൂർണ്ണസംഖ്യ			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    എങ്കിൽ (text_or_buf_locked()
	    || cmdwin_type != 0
#എങ്കിൽ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	തിരികെ K_IGNORE;
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
    എങ്കിൽ (win_split((പൂർണ്ണസംഖ്യ)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	തിരികെ K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    എങ്കിൽ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	തിരികെ Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_ONE, ECMD_HIDE,
					ശൂന്യ);
    cmdwin_valid = win_valid(cmdwin_win);
    എങ്കിൽ (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	എങ്കിൽ (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	എങ്കിൽ (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	എങ്കിൽ (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(ശൂന്യ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ശൂന്യ;
	beep_flush();
	ga_clear(&winsizes);
	തിരികെ Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_മൂല്യം_give_err((char_u *)"bt",
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
    എങ്കിൽ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	എങ്കിൽ (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_തിരുകുക, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_തിരുകുക, TRUE);
	}
	set_option_മൂല്യം_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    എങ്കിൽ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	എങ്കിൽ (i >= 0)
	{
	    lnum = 0;
	    ചെയ്യുക
	    {
		എങ്കിൽ (++i == get_hislen())
		    i = 0;
		എങ്കിൽ (get_histentry(histtype)[i].hisstr != ശൂന്യ)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    ആകുമ്പോൾ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_വരി_എണ്ണം, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_വരി_abv_curs();
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
    എങ്കിൽ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_താക്കോൽ_Typed = താക്കോൽ_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    താക്കോൽ_Typed = save_താക്കോൽ_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = ശൂന്യ;
    cmdwin_win = ശൂന്യ;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    എങ്കിൽ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ defined(FEAT_EVAL)
	// autocmds may abort script processing
	എങ്കിൽ (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	എങ്കിൽ (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    എങ്കിൽ (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    എങ്കിൽ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (പൂർണ്ണസംഖ്യ)plen;
		ccline.cmdbufflen = (പൂർണ്ണസംഖ്യ)(plen + 1);
		cmdwin_result = CAR;
	    }
	    അല്ലെങ്കിൽ
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	എങ്കിൽ (ccline.cmdbuff == ശൂന്യ)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	അല്ലെങ്കിൽ
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    എങ്കിൽ (ccline.cmdpos == ccline.cmdlen - 1
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
	എങ്കിൽ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	എങ്കിൽ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(ശൂന്യ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	എങ്കിൽ (cmdwin_result == K_IGNORE)
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

    തിരികെ cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    പൂർണ്ണസംഖ്യ
is_in_cmdwin(ശൂന്യം)
{
    തിരികെ cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    എങ്കിൽ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ശൂന്യ)
	തിരികെ ശൂന്യ;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    തിരികെ (char_u *)ga.ga_data;
#അല്ലെങ്കിൽ
    തിരികെ ശൂന്യ;
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    ശൂന്യം
get_user_നിവേശം(
    typval_T	*argvars,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		inputdialog,
    പൂർണ്ണസംഖ്യ		secret)
{
    char_u	*prompt;
    char_u	*p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		c;
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    പൂർണ്ണസംഖ്യ		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ശൂന്യ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
    എങ്കിൽ (നിവേശം_busy)
	തിരികെ;  // this doesn't work recursively.

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	തിരികെ;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_നിവേശം
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    എങ്കിൽ (no_console_നിവേശം() && !is_not_a_term())
	തിരികെ;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    എങ്കിൽ (prompt != ശൂന്യ)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	എങ്കിൽ (p == ശൂന്യ)
	    p = prompt;
	അല്ലെങ്കിൽ
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    സന്ദേശം_start();
	    സന്ദേശം_clr_eos();
	    സന്ദേശം_puts_attr((അക്ഷരം *)prompt, get_echo_attr());
	    സന്ദേശം_didout = FALSE;
	    സന്ദേശം_starthere();
	    *p = c;
	}
	cmdline_row = സന്ദേശം_row;

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    എങ്കിൽ (defstr != ശൂന്യ)
		stuffReadbuffSpec(defstr);

	    എങ്കിൽ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		പൂർണ്ണസംഖ്യ	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ശൂന്യ;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		എങ്കിൽ (xp_name == ശൂന്യ)
		    തിരികെ;

		xp_namelen = (പൂർണ്ണസംഖ്യ)STRLEN(xp_name);

		എങ്കിൽ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    തിരികെ;
	    }
	}

	എങ്കിൽ (defstr != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ save_ex_normal_busy = ex_normal_busy;
	    പൂർണ്ണസംഖ്യ save_vgetc_busy = vgetc_busy;
	    പൂർണ്ണസംഖ്യ save_നിവേശം_busy = നിവേശം_busy;

	    നിവേശം_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    നിവേശം_busy = save_നിവേശം_busy;
	}
	എങ്കിൽ (inputdialog && rettv->vval.v_string == ശൂന്യ
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	സന്ദേശം_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    ശൂന്യം
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    എങ്കിൽ (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	തിരികെ;

    പൂർണ്ണസംഖ്യ cmd_type = get_cmdline_type();

    എങ്കിൽ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	താക്കോൽ_string[4];

	താക്കോൽ_string[0] = K_SPECIAL;
	താക്കോൽ_string[1] = KS_EXTRA;
	താക്കോൽ_string[2] = KE_WILD;
	താക്കോൽ_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(താക്കോൽ_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
