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

#ضمن "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_KEY	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ساكن cmdline_info_T ccline;

#ifdef FEAT_EVAL
ساكن صحيح	new_cmdpos;	// position set by set_cmdline_pos()
#endif

ساكن صحيح	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ساكن صحيح	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
ساكن صحيح	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

ساكن char_u	*getcmdline_int(صحيح firstc, long count, صحيح indent, صحيح clear_ccline);
ساكن صحيح	cmdline_charsize(صحيح idx);
ساكن فراغ	set_cmdspos(فراغ);
ساكن فراغ	set_cmdspos_cursor(فراغ);
ساكن فراغ	correct_cmdspos(صحيح idx, صحيح cells);
ساكن فراغ	dealloc_cmdbuff(فراغ);
ساكن فراغ	alloc_cmdbuff(size_t len);
ساكن فراغ	draw_cmdline(صحيح start, صحيح len);
ساكن فراغ	save_cmdline(cmdline_info_T *ccp);
ساكن فراغ	restore_cmdline(cmdline_info_T *ccp);
ساكن صحيح	cmdline_paste(صحيح regname, صحيح literally, صحيح remcr);
ساكن فراغ	redrawcmdprompt(فراغ);
ساكن صحيح	ccheck_abbr(صحيح);
ساكن صحيح	open_cmdwin(فراغ);
#ifdef FEAT_SEARCH_EXTRA
ساكن صحيح	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

ساكن صحيح	cedit_key = -1;	// key value of 'cedit' option

    ساكن فراغ
trigger_cmd_autocmd(صحيح typechar, صحيح evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    ساكن فراغ
abandon_cmdline(فراغ)
{
    dealloc_cmdbuff();
    ܐܢ (msg_scrolled == 0)
	compute_cmdrow();
    msg("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ساكن صحيح
empty_pattern(char_u *p, size_t len, صحيح delim)
{
    magic_T	magic_val = MAGIC_ON;

    ܐܢ (len > 0)
	(فراغ) skip_regexp_ex(p, delim, magic_isset(), عدم, عدم, &magic_val);
    ܐܚܪܢܐ
	ܦܢܐ TRUE;

    ܦܢܐ empty_pattern_magic(p, len, magic_val);
}

    ساكن صحيح
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    ܟܡܐ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != عدم)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ܦܢܐ len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
تعريفنوع بنية {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    صحيح		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    ساكن فراغ
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

    ساكن فراغ
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
تعريفنوع بنية {
    pos_T	search_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    صحيح		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    صحيح		did_incsearch;
    صحيح		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ساكن فراغ
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->search_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ساكن فراغ
set_search_match(pos_T *t)
{
    t->lnum += search_match_lines;
    t->col = search_match_endcol;
    ܐܢ (t->lnum > curbuf->b_ml.ml_line_count)
    {
	t->lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    صحيح
parse_pattern_and_range(
	pos_T	*incsearch_start,
	صحيح	*search_delim,
	صحيح	*skiplen,
	صحيح	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    صحيح		delim_optional = FALSE;
    صحيح		delim;
    صحيح		use_last_pat;
    magic_T     magic = 0;
    حرف	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    search_first_line = 0;
    search_last_line = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_LINES;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, عدم);

    ܐܢ (vim_strchr((char_u *)"sgvlu", *cmd) == عدم)
	ܦܢܐ FALSE;

    // Skip over command name to find pattern separator
    لكل (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    ܐܢ (*skipwhite(p) == NUL)
	ܦܢܐ FALSE;

    ܐܢ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	ܐܢ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	ܐܚܪܢܐ ܐܢ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    ܐܚܪܢܐ ܐܢ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	ܐܢ (*p == '!')
	    p = skipwhite(p + 1);
	ܟܡܐ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	ܐܢ (*p == NUL)
	    ܦܢܐ FALSE;
    }
    ܐܚܪܢܐ ܐܢ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	ܐܢ (*p == '!')
	{
	    p++;
	    ܐܢ (*skipwhite(p) == NUL)
		ܦܢܐ FALSE;
	}
	ܐܢ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    ܐܚܪܢܐ
	ܦܢܐ FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *search_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), عدم, عدم, &magic);
    use_last_pat = end == p && *end == delim;

    ܐܢ (end == p && !use_last_pat)
	ܦܢܐ FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    ܐܢ (!use_last_pat)
    {
	حرف c = *end;
	صحيح empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	ܐܢ (empty)
	    ܦܢܐ FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (صحيح)(p - ccline.cmdbuff);
    *patlen = (صحيح)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    ܐܢ (ea.addr_count > 0)
    {
	صحيح reverse_match = ea.line2 < ea.line1;
	search_first_line = reverse_match ? ea.line2 : ea.line1;
	search_last_line = reverse_match ? ea.line1 : ea.line2;
    }
    ܐܚܪܢܐ ܐܢ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	search_first_line = search_last_line = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ܦܢܐ TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ساكن صحيح
do_incsearch_highlighting(
	صحيح		    firstc,
	صحيح		    *search_delim,
	incsearch_state_T   *is_state,
	صحيح		    *skiplen,
	صحيح		    *patlen)
{
    صحيح retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    ܐܢ (!p_is || cmd_silent)
	ܦܢܐ FALSE;

    // By default search all lines
    search_first_line = 0;
    search_last_line = MAXLNUM;

    ܐܢ (firstc == '/' || firstc == '?')
    {
	*search_delim = firstc;
	ܦܢܐ TRUE;
    }

    ܐܢ (firstc != ':')
	ܦܢܐ FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->search_start, search_delim,
	    skiplen, patlen);
    --emsg_off;

    ܦܢܐ retval;
}

    ساكن فراغ
finish_incsearch_highlighting(
	صحيح gotesc,
	incsearch_state_T *is_state,
	صحيح call_update_screen)
{
    ܐܢ (!is_state->did_incsearch)
	ܦܢܐ;

    is_state->did_incsearch = FALSE;
    ܐܢ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    ܐܚܪܢܐ
    {
	ܐܢ (!EQUAL_POS(is_state->save_cursor, is_state->search_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->search_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    search_first_line = 0;
    search_last_line = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    ܐܢ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ساكن فراغ
may_do_incsearch_highlighting(
	صحيح		    firstc,
	long		    count,
	incsearch_state_T   *is_state)
{
    صحيح		skiplen, patlen;
    صحيح		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    صحيح		next_char;
    صحيح		use_last_pat;
    صحيح		did_do_incsearch = is_state->did_incsearch;
    صحيح		search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    ܐܢ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	ܐܢ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ܦܢܐ;
    }

    // If there is a character waiting, search and redraw later.
    ܐܢ (char_avail())
    {
	restore_last_search_pattern();
	is_state->incsearch_postponed = TRUE;
	ܦܢܐ;
    }
    is_state->incsearch_postponed = FALSE;

    ܐܢ (search_first_line == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->search_start;
    ܐܚܪܢܐ ܐܢ (search_first_line > curbuf->b_ml.ml_line_count)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	curwin->w_cursor.col = MAXCOL;
    }
    ܐܚܪܢܐ
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = search_first_line;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    ܐܢ (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    ܐܚܪܢܐ
    {
	صحيح search_flags = SEARCH_OPT + SEARCH_NOOF + SEARCH_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	ܐܢ (!p_hls)
	    search_flags += SEARCH_KEEP;
	ܐܢ (search_first_line != 0)
	    search_flags += SEARCH_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_search(عدم, firstc == ':' ? '/' : firstc, search_delim,
				 ccline.cmdbuff + skiplen, patlen, count, search_flags,
# ifdef FEAT_RELTIME
		&sia
# ܐܚܪܢܐ
		عدم
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	ܐܢ (curwin->w_cursor.lnum < search_first_line
		|| curwin->w_cursor.lnum > search_last_line)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->search_start;
	}

	// if interrupted while searching, behave like it failed
	ܐܢ (got_int)
	{
	    (فراغ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	ܐܚܪܢܐ ܐܢ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    ܐܢ (found != 0)
	highlight_match = TRUE;		// highlight position
    ܐܚܪܢܐ
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    ܐܢ (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_search_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    ܐܚܪܢܐ
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    ܐܢ (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	ܐܢ (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, search_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    ܐܢ (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_search_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    ܐܢ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->search_start;
    ܐܚܪܢܐ ܐܢ (found != 0)
	curwin->w_cursor = end_pos;

    msg_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ساكن صحيح
may_adjust_incsearch_highlighting(
	صحيح			firstc,
	long			count,
	incsearch_state_T	*is_state,
	صحيح			c)
{
    صحيح		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = عدم;
    char_u	*searchstr;
    char_u	*strcopy = عدم;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    صحيح		search_flags = SEARCH_NOOF;
    صحيح		i;
    صحيح		search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    ܐܢ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	ܦܢܐ OK;
    }
    ܐܢ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_search_pattern();
	ܦܢܐ FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (فراغ)parse_search_pattern_offset(&pat, &patlen_s, search_delim,
				    SEARCH_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    ܐܢ (c == Ctrl_G)
    {
	t = is_state->match_end;
	ܐܢ (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (فراغ)decl(&t);
	search_flags += SEARCH_COL;
    }
    ܐܚܪܢܐ
	t = is_state->match_start;
    ܐܢ (!p_hls)
	search_flags += SEARCH_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, عدم,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, count, search_flags, RE_SEARCH, عدم);
    --emsg_off;
    ܐܢ (dircp != عدم)
	*dircp = search_delim;
    ܐܢ (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->search_start = match_start;
	ܐܢ (!offset.line && (offset.end || off != 0))
	{
	    ܐܢ (offset.end)
	    {
		is_state->search_start = match_end;
		(فراغ)decl(&is_state->search_start);
	    }
	    ܟܡܐ (off > 0)
	    {
		ܐܢ (incl(&is_state->search_start) == -1)
		    اكسر;
		--off;
	    }
	    ܟܡܐ (off < 0)
	    {
		ܐܢ (decl(&is_state->search_start) == -1)
		    اكسر;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	ܐܢ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (فراغ)decl(&is_state->search_start);
	}
	ܐܚܪܢܐ ܐܢ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (فراغ)incl(&is_state->search_start);
	}
	ܐܢ (LT_POS(t, is_state->search_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->search_start = t;
	    ܐܢ (firstc == '?')
		(فراغ)incl(&is_state->search_start);
	    ܐܚܪܢܐ
		(فراغ)decl(&is_state->search_start);
	}

	set_search_match(&is_state->match_end);
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
    ܐܚܪܢܐ
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_search_pattern();
    ܦܢܐ FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ساكن صحيح
may_add_char_to_search(صحيح firstc, صحيح *c, incsearch_state_T *is_state)
{
    صحيح		skiplen, patlen, search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    ܐܢ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	ܦܢܐ FAIL;
    }
    restore_last_search_pattern();

    // Add a character from under the cursor for 'incsearch'.
    ܐܢ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	ܐܢ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    ܐܢ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    ܐܢ (*c == search_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != عدم)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    ܐܢ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		صحيح save_c = *c;

		ܟܡܐ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ܦܢܐ FAIL;
	}
    }
    ܦܢܐ OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ساكن صحيح
cmdline_has_arabic(صحيح start, صحيح len)
{
    صحيح	    j;
    صحيح	    mb_l;
    صحيح	    u8c;
    char_u  *p;
    صحيح	    u8cc[MAX_MCO];

    ܐܢ (!enc_utf8)
	ܦܢܐ FALSE;

    لكل (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	ܐܢ (ARABIC_CHAR(u8c))
	    ܦܢܐ TRUE;
    }
    ܦܢܐ FALSE;
}
#endif

    فراغ
cmdline_init(فراغ)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ساكن صحيح
cmdline_handle_ctrl_bsl(صحيح c, صحيح *gotesc)
{
    ++no_mapping;
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    ܐܢ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && KeyTyped)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	ܦܢܐ PROCESS_NEXT_KEY;
    }

#ifdef FEAT_EVAL
    ܐܢ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	ܐܢ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	ܐܚܪܢܐ
	    new_cmdpos = ccline.cmdpos;

	c = get_expr_register();
	ܐܢ (c == '=')
	{
	    char_u	*p = عدم;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_expr_line();
	    --textlock;

	    ܐܢ (p != عدم)
	    {
		صحيح len = (صحيح)STRLEN(p);

		ܐܢ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    ܐܢ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    ܐܚܪܢܐ
			ccline.cmdpos = new_cmdpos;

		    KeyTyped = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ܦܢܐ CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ܦܢܐ CMDLINE_NOT_CHANGED;
    }
#endif

    ܐܢ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ܦܢܐ GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ساكن صحيح
cmdline_wildchar_complete(
	صحيح		c,
	صحيح		escape,
	صحيح		*did_wild_list,
	صحيح		*wim_index_p,
	expand_T	*xp,
	صحيح		*gotesc,
	صحيح		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    صحيح		wim_index = *wim_index_p;
    صحيح		res;
    صحيح		cmdpos_before;
    صحيح		options = WILD_NO_BEEP;
    صحيح		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    صحيح		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    ܐܢ (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    ܐܢ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	ܐܢ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (فراغ)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	ܐܢ (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ܐܚܪܢܐ ܐܢ (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	ܐܚܪܢܐ
	    res = OK;	    // don't insert 'wildchar' now
    }
    ܐܚܪܢܐ		    // typed p_wc first time
    {
	صحيح wim_longest = (wim_flags[0] & WIM_LONGEST);
	صحيح wim_list = (wim_flags[0] & WIM_LIST);
	صحيح wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	ܐܢ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    ܐܢ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    ܐܢ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    ܐܚܪܢܐ
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	ܐܢ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ܐܚܪܢܐ
	{
	    ܐܢ (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    ܐܢ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	ܐܢ (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	ܐܢ (got_int)
	{
	    (فراغ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (فراغ)ExpandOne(xp, عدم, عدم, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    ܦܢܐ CMDLINE_CHANGED;
	}

	// Display matches
	ܐܢ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    ܐܢ (wim_longest)
	    {
		صحيح found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		صحيح show_menu = p_wmnu && wim_full;

		ܐܢ (wim_list || show_menu)
		    (فراغ)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		ܐܚܪܢܐ ܐܢ (!found_longest_prefix)
		{
		    صحيح wim_list_next = (wim_flags[1] & WIM_LIST);
		    صحيح wim_full_next = (wim_flags[1] & WIM_FULL);
		    صحيح wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    صحيح wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    ܐܢ (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			ܐܢ (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			ܐܚܪܢܐ
			    (فراغ)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			ܐܢ (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    ܐܚܪܢܐ
	    {
		صحيح show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		ܐܢ (wim_list || show_menu)
		    (فراغ)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		ܐܚܪܢܐ
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    ܐܢ (wim_list)
		*did_wild_list = TRUE;
	}
	ܐܚܪܢܐ ܐܢ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    ܐܢ (wim_index < 3)
	++wim_index;
    ܐܢ (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    ܦܢܐ (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ساكن صحيح
cmdline_erase_chars(
	صحيح c,
	صحيح indent
#ifdef FEAT_SEARCH_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    صحيح		i;
    صحيح		j;

    ܐܢ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    ܐܢ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    ܐܢ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    ܐܢ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	ܐܢ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    ܐܢ (c == Ctrl_W)
	    {
		ܟܡܐ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		ܟܡܐ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		ܐܢ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	ܐܚܪܢܐ ܐܢ (c == Ctrl_W)
	{
	    ܟܡܐ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    ܐܢ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		ܟܡܐ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	ܐܚܪܢܐ
	    --p;
	ccline.cmdpos = (صحيح)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	ܟܡܐ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_SEARCH_EXTRA
	ܐܢ (ccline.cmdlen == 0)
	{
	    isp->search_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    ܐܚܪܢܐ ܐܢ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == عدم && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	ܐܢ (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    ܦܢܐ CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	ܐܢ (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    ܐܢ (cmdmsg_rl)
		msg_col = cmdline_width;
	    ܐܚܪܢܐ
#endif
		msg_col = 0;
	    msg_putchar(' ');		// delete ':'
	}
#ifdef FEAT_SEARCH_EXTRA
	isp->search_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	ܦܢܐ GOTO_NORMAL_MODE;
    }
    ܦܢܐ CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ساكن فراغ
cmdline_toggle_langmap(long *b_im_ptr)
{
    ܐܢ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	ܐܢ (b_im_ptr != عدم)
	{
	    ܐܢ (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    ܐܚܪܢܐ
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_INPUT_METHOD
    ܐܚܪܢܐ
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	ܐܢ ((p_imdisable && b_im_ptr != عدم)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    ܐܢ (b_im_ptr != عدم)
		*b_im_ptr = B_IMODE_NONE;
	}
	ܐܚܪܢܐ
	{
	    im_set_active(TRUE);	// Enable input method
	    ܐܢ (b_im_ptr != عدم)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    ܐܢ (b_im_ptr != عدم)
    {
	ܐܢ (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	ܐܚܪܢܐ
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#ܐܢ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ساكن صحيح
cmdline_insert_reg(صحيح *gotesc UNUSED)
{
    صحيح		i;
    صحيح		c;
    صحيح		literally = FALSE;
#ifdef FEAT_EVAL
    صحيح		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_keys;
    i = c = plain_vgetc();	// CTRL-R <char>
    ܐܢ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    ܐܢ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_keys;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    ܐܢ (c == '=')
    {
	ܐܢ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	ܐܚܪܢܐ
	    c = get_expr_register();
    }
#endif
    ܐܢ (c != ESC)	    // use ESC to cancel inserting register
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
	ܐܢ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ܦܢܐ GOTO_NORMAL_MODE;
	}
#endif
	KeyTyped = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	ܐܢ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    ܐܢ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    ܐܚܪܢܐ
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
    ܦܢܐ literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ساكن فراغ
cmdline_left_right_mouse(صحيح c, صحيح *ignore_drag_release)
{
    ܐܢ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    ܐܚܪܢܐ
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    ܐܢ (!gui.in_use)
#endif
	ܐܢ (!mouse_has(MOUSE_COMMAND))
	    ܦܢܐ;
#ifdef FEAT_CLIPBOARD
    ܐܢ (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	صحيح	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	ܐܢ (mouse_model_popup() && button == MOUSE_LEFT
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_RIGHT;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	ܦܢܐ;
    }
#endif

    set_cmdspos();
    لكل (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	صحيح	i;

	i = cmdline_charsize(ccline.cmdpos);
	ܐܢ (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    اكسر;
	ܐܢ (has_mbyte)
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
    ساكن صحيح
cmdline_browse_history(
	صحيح	c,
	صحيح	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	صحيح	histype,
	صحيح	*hiscnt_p,
	expand_T *xp)
{
    صحيح		orig_hiscnt;
    صحيح		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    صحيح		res;

    ܐܢ (get_hislen() == 0 || firstc == NUL)	// no history
	ܦܢܐ CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    ܐܢ (lookfor == عدم)
    {
	ܐܢ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == عدم)
	    ܦܢܐ CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    لكل (;;)
    {
	// one step backwards
	ܐܢ (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    ܐܢ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    ܐܚܪܢܐ ܐܢ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    ܐܚܪܢܐ ܐܢ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    ܐܚܪܢܐ			// at top of list
	    {
		hiscnt = orig_hiscnt;
		اكسر;
	    }
	}
	ܐܚܪܢܐ    // one step forwards
	{
	    // on last entry, clear the line
	    ܐܢ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		اكسر;
	    }

	    // not on a history line, nothing to do
	    ܐܢ (hiscnt == get_hislen())
		اكسر;
	    ܐܢ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    ܐܚܪܢܐ
		++hiscnt;
	}
	ܐܢ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== عدم)
	{
	    hiscnt = orig_hiscnt;
	    اكسر;
	}
	ܐܢ ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    اكسر;
    }

    ܐܢ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	صحيح	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	ܐܢ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	ܐܚܪܢܐ
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	ܐܢ (histype == HIST_SEARCH
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    صحيح     i;
	    صحيح     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    لكل (i = 0; i <= 1; ++i)
	    {
		len = 0;
		لكل (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    ܐܢ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			ܐܢ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    ܐܚܪܢܐ
		    {
			// Escape new sep, unless it is already
			// escaped.
			ܐܢ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    ܐܢ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			ܐܢ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		ܐܢ (i == 0)
		{
		    alloc_cmdbuff(len);
		    ܐܢ (ccline.cmdbuff == عدم)
		    {
			res = GOTO_NORMAL_MODE;
			اذهب done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (صحيح)len;
	}
	ܐܚܪܢܐ
	{
	    alloc_cmdbuff(plen);
	    ܐܢ (ccline.cmdbuff == عدم)
	    {
		res = GOTO_NORMAL_MODE;
		اذهب done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (صحيح)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	اذهب done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ܦܢܐ res;
}

/*
 * Initialize the current command-line info.
 */
    ساكن صحيح
init_ccline(صحيح firstc, صحيح indent)
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
    ܐܢ (ccline.cmdbuff == عدم)
    {
	ccline.cmdbufflen = 0;
	ܦܢܐ FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    ܐܢ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ܦܢܐ OK;
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
    صحيح		  firstc,
    long	  count,	// only used for incremental search
    صحيح		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    ܦܢܐ getcmdline_int(firstc, count, indent, TRUE);
}

    ساكن char_u *
getcmdline_int(
    صحيح		firstc,
    long	count UNUSED,	// only used for incremental search
    صحيح		indent,		// indent for inside conditionals
    صحيح		clear_ccline)	// clear ccline first
{
    ساكن صحيح	depth = 0;	    // call depth
    صحيح		c = 0;
    صحيح		i;
    صحيح		j;
    صحيح		gotesc = FALSE;		// TRUE when <ESC> just typed
    صحيح		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = عدم;	// string to match
    size_t	lookforlen = 0;
    صحيح		hiscnt;			// current history line in use
    صحيح		histype;		// history type to be used
#ifdef FEAT_SEARCH_EXTRA
    incsearch_state_T	is_state;
#endif
    صحيح		did_wild_list = FALSE;	// did wild_list() recently
    صحيح		wim_index = 0;		// index in wim_flags[]
    صحيح		res;
    صحيح		save_msg_scroll = msg_scroll;
    صحيح		save_State = State;	// remember State when called
    صحيح		some_key_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    صحيح		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    صحيح		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = عدم;
    buf_T	*b_im_ptr_buf = عدم;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    صحيح		did_save_ccline = FALSE;
    صحيح		cmdline_type;
    صحيح		wild_type = 0;
    صحيح		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = عدم;
    صحيح		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    ܐܢ (ccline.cmdbuff != عدم)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    ܐܢ (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    ܐܢ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    ܐܢ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_SEARCH_EXTRA
    init_incsearch_state(&is_state);
#endif

    ܐܢ (init_ccline(firstc, indent) != OK)
	اذهب theend;	// out of memory

    ܐܢ (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	اذهب theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    ܐܢ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    ܐܚܪܢܐ
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    ܐܢ (!cmd_silent)
    {
	i = msg_scrolled;
	msg_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	msg_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#ܐܢ defined(FEAT_EVAL)
    ܐܢ (ccline.input_fn)
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
    msg_scroll = FALSE;

    State = MODE_CMDLINE;

    ܐܢ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	ܐܢ (curbuf->b_p_imsearch == B_IMODE_USE_INSERT)
	    b_im_ptr = &curbuf->b_p_iminsert;
	ܐܚܪܢܐ
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	ܐܢ (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_INPUT_METHOD
    ܐܚܪܢܐ ܐܢ (p_imcmdline)
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
    ܐܢ (!debug_mode)
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
    ܐܢ (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    ܐܢ (!cmd_silent && msg_scrolled == 0)
    {
	صحيح	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ܐܢ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	ܐܢ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	ܐܢ (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    لكل (;;)
    {
	صحيح	trigger_cmdlinechanged = TRUE;
	صحيح	end_wildmenu;
	صحيح	prev_cmdpos = ccline.cmdpos;
	صحيح	skip_pum_redraw = FALSE;

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

	ܐܢ (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_key_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	ܐܢ (ccline.cmdbuff != عدم)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    ܐܢ (prev_cmdbuff == عدم)
		اذهب returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	ܐܢ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	ܥܒܕ
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} ܟܡܐ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	ܐܢ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    ܐܢ (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (فراغ)ExpandOne(&xpc, عدم, عدم, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	ܐܢ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    استمر;
	}

	ܐܢ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    صحيح	    clen = ccline.cmdlen;
	    صحيح	    cc_count = aucmd_cmdline_changed_count;

	    ܐܢ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		ܐܢ (clen == ccline.cmdlen
				    || cc_count != aucmd_cmdline_changed_count)
		    trigger_cmdlinechanged = FALSE;
		اذهب cmdline_changed;
	    }
	}

	ܐܢ (KeyTyped)
	{
	    some_key_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    ܐܢ (cmd_hkmap)
		c = hkmap(c);
	    ܐܢ (cmdmsg_rl && !KeyStuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		بدل (c)
		{
		    حالة K_RIGHT:   c = K_LEFT; اكسر;
		    حالة K_S_RIGHT: c = K_S_LEFT; اكسر;
		    حالة K_C_RIGHT: c = K_C_LEFT; اكسر;
		    حالة K_LEFT:    c = K_RIGHT; اكسر;
		    حالة K_S_LEFT:  c = K_S_RIGHT; اكسر;
		    حالة K_C_LEFT:  c = K_C_RIGHT; اكسر;
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
	ܐܢ ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#ܐܢ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	ܐܢ (lookfor != عدم
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_LEFT && c != K_RIGHT
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	ܐܢ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	ܐܢ (p_wmnu)
	    c = wildmenu_translate_key(&ccline, c, &xpc, did_wild_list);

	صحيح key_is_wc = (c == p_wc && KeyTyped) || c == p_wcm;
	ܐܢ ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !key_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    ܐܢ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		ܐܢ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    اكسر;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	ܐܢ (KeyTyped && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#ܐܢ defined(FEAT_SEARCH_EXTRA)
	    ܐܢ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!key_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	ܐܢ (end_wildmenu)
	{
	    ܐܢ (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !key_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    ܐܢ (xpc.xp_numfiles != -1)
		(فراغ)ExpandOne(&xpc, عدم, عدم, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    ܐܢ (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	ܐܢ (p_wmnu)
	    c = wildmenu_process_key(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	ܐܢ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    ܐܢ (res == CMDLINE_CHANGED)
		اذهب cmdline_changed;
	    ܐܚܪܢܐ ܐܢ (res == CMDLINE_NOT_CHANGED)
		اذهب cmdline_not_changed;
	    ܐܚܪܢܐ ܐܢ (res == GOTO_NORMAL_MODE)
		اذهب returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	ܐܢ (c == cedit_key || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    ܐܢ ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_key_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	ܐܚܪܢܐ
	    c = do_digraph(c);
#endif

	ܐܢ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!KeyTyped || vim_strchr(p_cpo, CPO_ESC) != عدم)))
	{
	    // In Ex mode a backslash escapes a newline.
	    ܐܢ (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		ܐܢ (c == K_KENTER)
		    c = '\n';
	    }
	    ܐܚܪܢܐ
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		ܐܢ (ccheck_abbr(c + ABBR_OFF))
		    اذهب cmdline_changed;
		ܐܢ (!cmd_silent)
		{
		    windgoto(msg_row, cmdline_col_off);
		    out_flush();
		}
		اكسر;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	ܐܢ ((c == p_wc && !gotesc && KeyTyped) || c == p_wcm || c == K_WILD)
	{
	    ܐܢ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_SEARCH_EXTRA
		    &is_state.search_start
#ܐܚܪܢܐ
		    عدم
#endif
		    );
	    ܐܢ (c == K_WILD)
		--emsg_silent;
	    ܐܢ (res == CMDLINE_CHANGED)
		اذهب cmdline_changed;
	    ܐܢ (c == K_WILD)
		اذهب cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	ܐܢ (c == K_S_TAB && KeyTyped)
	{
	    ܐܢ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		ܐܢ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    صحيح list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		ܐܢ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    اذهب cmdline_changed;
	    }
	}

	ܐܢ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	ܐܢ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_SEARCH_EXTRA
	    // Apply search highlighting
	    ܐܢ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    ܐܢ (KeyTyped || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, count, &is_state);
#endif
	    wild_type = 0;
	    اذهب cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	بدل (c)
	{
	حالة K_BS:
	حالة Ctrl_H:
	حالة K_DEL:
	حالة K_KDEL:
	حالة Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_SEARCH_EXTRA
		    , &is_state
#endif
		    );
	    ܐܢ (res == CMDLINE_NOT_CHANGED)
		اذهب cmdline_not_changed;
	    ܐܚܪܢܐ ܐܢ (res == GOTO_NORMAL_MODE)
		اذهب returncmd;		// back to cmd mode
	    اذهب cmdline_changed;

	حالة K_INS:
	حالة K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		اذهب cmdline_not_changed;

	حالة Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : عدم);
		اذهب cmdline_not_changed;

//	case '@':   only in very old vi
	حالة Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		ܟܡܐ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_SEARCH_EXTRA
		ܐܢ (ccline.cmdlen == 0)
		    is_state.search_start = is_state.save_cursor;
#endif
		redrawcmd();
		اذهب cmdline_changed;

#ifdef FEAT_CLIPBOARD
	حالة Ctrl_Y:
		// Copy the modeless selection, if there is one.
		ܐܢ (clip_star.state != SELECT_CLEARED)
		{
		    ܐܢ (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    اذهب cmdline_not_changed;
		}
		اكسر;
#endif

	حالة ESC:	// get here if p_wc != ESC or when ESC typed twice
	حالة Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		ܐܢ (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    اذهب cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		اذهب returncmd;		// back to cmd mode

	حالة Ctrl_R:			// insert register
		res = cmdline_insert_reg(&gotesc);
		ܐܢ (res == GOTO_NORMAL_MODE)
		    اذهب returncmd;
		ܐܢ (res == CMDLINE_CHANGED)
		    اذهب cmdline_changed;
		اذهب cmdline_not_changed;

	حالة Ctrl_D:
		ܐܢ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    اكسر;	// Use ^D as normal char instead

		redrawcmd();
		استمر;	// don't do incremental search now

	حالة K_RIGHT:
	حالة K_S_RIGHT:
	حالة K_C_RIGHT:
		ܥܒܕ
		{
		    ܐܢ (ccline.cmdpos >= ccline.cmdlen)
			اكسر;
		    i = cmdline_charsize(ccline.cmdpos);
		    ܐܢ (KeyTyped && ccline.cmdspos + i >= cmdline_width * Rows)
			اكسر;
		    ccline.cmdspos += i;
		    ܐܢ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    ܐܚܪܢܐ
			++ccline.cmdpos;
		}
		ܟܡܐ ((c == K_S_RIGHT || c == K_C_RIGHT
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		ܐܢ (has_mbyte)
		    set_cmdspos_cursor();
		اذهب cmdline_not_changed;

	حالة K_LEFT:
	حالة K_S_LEFT:
	حالة K_C_LEFT:
		ܐܢ (ccline.cmdpos == 0)
		    اذهب cmdline_not_changed;
		ܥܒܕ
		{
		    --ccline.cmdpos;
		    ܐܢ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		ܟܡܐ (ccline.cmdpos > 0
			&& (c == K_S_LEFT || c == K_C_LEFT
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		ܐܢ (has_mbyte)
		    set_cmdspos_cursor();
		اذهب cmdline_not_changed;

	حالة K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		اذهب cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالة K_F4:
	    ܐܢ (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		اذهب cmdline_not_changed;
	    }
	    اكسر;
#endif

	حالة K_MIDDLEDRAG:
	حالة K_MIDDLERELEASE:
		اذهب cmdline_not_changed;	// Ignore mouse

	حالة K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		ܐܢ (!gui.in_use)
#endif
		    ܐܢ (!mouse_has(MOUSE_COMMAND))
			اذهب cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		ܐܢ (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		ܐܚܪܢܐ
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		اذهب cmdline_changed;

#ifdef FEAT_DND
	حالة K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		اذهب cmdline_changed;
#endif

	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ܐܢ (ignore_drag_release)
		    اذهب cmdline_not_changed;
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		cmdline_left_right_mouse(c, &ignore_drag_release);
		اذهب cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	حالة K_MOUSEDOWN:
	حالة K_MOUSEUP:
	حالة K_MOUSELEFT:
	حالة K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		ܐܢ (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_LEFT
				      : MSCR_RIGHT);
#endif
		اذهب cmdline_not_changed;

	// Alternate buttons ignored here
	حالة K_X1MOUSE:
	حالة K_X1DRAG:
	حالة K_X1RELEASE:
	حالة K_X2MOUSE:
	حالة K_X2DRAG:
	حالة K_X2RELEASE:
	حالة K_MOUSEMOVE:
		اذهب cmdline_not_changed;

#ifdef FEAT_GUI
	حالة K_LEFTMOUSE_NM:	// mousefocus click, ignored
	حالة K_LEFTRELEASE_NM:
		اذهب cmdline_not_changed;

	حالة K_VER_SCROLLBAR:
		ܐܢ (msg_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		اذهب cmdline_not_changed;

	حالة K_HOR_SCROLLBAR:
		ܐܢ (msg_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_value);
		    redrawcmd();
		}
		اذهب cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	حالة K_TABLINE:
	حالة K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		ܐܢ (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		اذهب cmdline_not_changed;
#endif

	حالة K_SELECT:	    // end of Select mode mapping - ignore
		اذهب cmdline_not_changed;

	حالة Ctrl_B:	    // begin of command line
	حالة K_HOME:
	حالة K_KHOME:
	حالة K_S_HOME:
	حالة K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		اذهب cmdline_not_changed;

	حالة Ctrl_E:	    // end of command line
	حالة K_END:
	حالة K_KEND:
	حالة K_S_END:
	حالة K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		اذهب cmdline_not_changed;

	حالة Ctrl_A:	    // all matches
		ܐܢ (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		ܐܢ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    اكسر;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		اذهب cmdline_changed;

	حالة Ctrl_L:
#ifdef FEAT_SEARCH_EXTRA
		ܐܢ (may_add_char_to_search(firstc, &c, &is_state) == OK)
		    اذهب cmdline_not_changed;
#endif

		// completion: longest common part
		ܐܢ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    اكسر;
		اذهب cmdline_changed;

	حالة Ctrl_N:	    // next match
	حالة Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		ܐܢ (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    ܐܢ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    اذهب cmdline_not_changed;
		}
#endif
		ܐܢ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    ܐܢ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			اكسر;
		    اذهب cmdline_changed;
		}
		// FALLTHROUGH
	حالة K_UP:
	حالة K_DOWN:
	حالة K_S_UP:
	حالة K_S_DOWN:
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		ܐܢ (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    صحيح	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    ܐܢ (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    اذهب cmdline_not_changed;
		}
#endif
		ܐܢ (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    ܐܢ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    ܐܢ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			اكسر;
		    اذهب cmdline_changed;
		}
		ܐܚܪܢܐ
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    ܐܢ (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			اذهب cmdline_changed;
		    }
		    ܐܚܪܢܐ ܐܢ (res == GOTO_NORMAL_MODE)
			اذهب returncmd;
		}
		اذهب cmdline_not_changed;

#ifdef FEAT_SEARCH_EXTRA
	حالة Ctrl_G:	    // next match
	حالة Ctrl_T:	    // previous match
		ܐܢ (may_adjust_incsearch_highlighting(
					  firstc, count, &is_state, c) == FAIL)
		    اذهب cmdline_not_changed;
		اكسر;
#endif

	حالة Ctrl_V:
	حالة Ctrl_Q:
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
		    ܐܢ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			msg_putchar(' ');
			cursorcmd();
		    }
		}

		اكسر;

#ifdef FEAT_DIGRAPHS
	حالة Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		ܐܢ (c != NUL)
		    اكسر;

		redrawcmd();
		اذهب cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	حالة Ctrl__:	    // CTRL-_: switch language mode
		ܐܢ (!p_ari)
		    اكسر;
		cmd_hkmap = !cmd_hkmap;
		اذهب cmdline_not_changed;
#endif

	حالة K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, عدم);
		اذهب cmdline_changed;

	افتراضي:
#ifdef UNIX
		ܐܢ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    اذهب returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		ܐܢ (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		اكسر;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	ܐܢ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    اذهب cmdline_changed;

	/*
	 * put the character in the command line
	 */
	ܐܢ (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_key_name(c, mod_mask), -1, TRUE);
	ܐܚܪܢܐ
	{
	    ܐܢ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    ܐܚܪܢܐ
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	اذهب cmdline_changed;

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
	ܐܢ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_SEARCH_EXTRA
	ܐܢ (!is_state.incsearch_postponed)
	    استمر;
#endif

cmdline_changed:
#ifdef FEAT_SEARCH_EXTRA
	// If the window changed incremental search state is not valid.
	ܐܢ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	ܐܢ (xpc.xp_context == EXPAND_NOTHING && (KeyTyped || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, count, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	ܐܢ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != عدم &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    ܐܢ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	ܐܢ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	ܐܢ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    ܐܢ (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    ܐܢ (!event_cmdlineleavepre_triggered)
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
    ܐܢ (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = عدم;
    clear_cmdline_orig();

#ifdef FEAT_SEARCH_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    ܐܢ (ccline.cmdbuff != عدم)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ܐܢ (ccline.cmdlen && firstc != NUL
		&& (some_key_typed || histype == HIST_SEARCH))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_SEARCH ? firstc : NUL);
	    ܐܢ (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	ܐܢ (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    msg_check();
    msg_scroll = save_msg_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    ܐܢ (some_key_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    ܐܢ (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_INPUT_METHOD
    ܐܢ (b_im_ptr != عدم && buf_valid(b_im_ptr_buf)
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
	ܐܢ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	ܐܚܪܢܐ
	    ccline.cmdbuff = عدم;

	vim_free(prev_cmdbuff);
	ܦܢܐ p;
    }
}

#ܐܢ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    صحيح		firstc,
    char_u	*prompt,	// command line prompt
    صحيح		attr,		// attributes for prompt
    صحيح		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    صحيح			did_save_ccline = FALSE;
    صحيح			msg_col_save = msg_col;
    صحيح			msg_silent_save = msg_silent;

    ܐܢ (ccline.cmdbuff != عدم)
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
    ccline.input_fn = (firstc == '@');
# endif
    msg_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    ܐܢ (did_save_ccline)
	restore_cmdline(&save_ccline);

    msg_silent = msg_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    ܐܢ (ccline.cmdbuff != عدم)
	msg_col = msg_col_save;

    ܦܢܐ s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    صحيح
check_opt_wim(فراغ)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    صحيح		i;
    صحيح		idx = 0;

    لكل (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    لكل (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	لكل (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	ܐܢ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ܦܢܐ FAIL;
	ܐܢ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	ܐܚܪܢܐ ܐܢ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	ܐܚܪܢܐ ܐܢ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	ܐܚܪܢܐ ܐܢ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	ܐܚܪܢܐ ܐܢ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	ܐܚܪܢܐ ܐܢ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	ܐܚܪܢܐ
	    ܦܢܐ FAIL;
	p += i;
	ܐܢ (*p == NUL)
	    اكسر;
	ܐܢ (*p == ',')
	{
	    ܐܢ (idx == 3)
		ܦܢܐ FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    ܟܡܐ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    لكل (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ܦܢܐ OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    صحيح
text_locked(فراغ)
{
    ܐܢ (cmdwin_type != 0)
	ܦܢܐ TRUE;
    ܦܢܐ textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    فراغ
text_locked_msg(فراغ)
{
    emsg(_(get_text_locked_msg()));
}

    حرف *
get_text_locked_msg(فراغ)
{
    ܐܢ (cmdwin_type != 0)
	ܦܢܐ e_invalid_in_cmdline_window;
    ܦܢܐ e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    صحيح
text_or_buf_locked(فراغ)
{
    ܐܢ (text_locked())
    {
	text_locked_msg();
	ܦܢܐ TRUE;
    }
    ܦܢܐ curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    صحيح
curbuf_locked(فراغ)
{
    ܐܢ (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	ܦܢܐ TRUE;
    }
    ܦܢܐ allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    صحيح
allbuf_locked(فراغ)
{
    ܐܢ (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	ܦܢܐ TRUE;
    }
    ܦܢܐ FALSE;
}

    ساكن صحيح
cmdline_charsize(صحيح idx)
{
#ܐܢ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ܐܢ (cmdline_star > 0)	    // showing '*', always 1 position
	ܦܢܐ 1;
#endif
    ܦܢܐ ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ساكن فراغ
set_cmdspos(فراغ)
{
    ܐܢ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    ܐܚܪܢܐ
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ساكن فراغ
set_cmdspos_cursor(فراغ)
{
    صحيح		i, m, c;

    set_cmdspos();
    ܐܢ (KeyTyped)
    {
	m = cmdline_width * Rows;
	ܐܢ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    ܐܚܪܢܐ
	m = MAXCOL;
    لكل (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	ܐܢ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	ܐܢ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    اكسر;
	}
	ܐܢ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ساكن فراغ
correct_cmdspos(صحيح idx, صحيح cells)
{
    ܐܢ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    صحيح		c,		// normally ':', NUL for ":append"
    فراغ	*cookie UNUSED,
    صحيح		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    ܐܢ (exec_from_reg && vpeekc() == ':')
	(فراغ)vgetc();
    ܦܢܐ getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    صحيح		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    فراغ	*cookie UNUSED,
    صحيح		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	line_ga;
    char_u	*pend;
    صحيح		startcol = 0;
    صحيح		c1 = 0;
    صحيح		escaped = FALSE;	// CTRL-V typed
    صحيح		vcol = 0;
    char_u	*p;
    صحيح		prev_char;
    صحيح		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    ܐܢ ((msg_col || msg_didout) && promptc != '?')
	msg_putchar('\n');
    ܐܢ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	ܐܢ (p_prompt)
	    msg_putchar(':');
	ܟܡܐ (indent-- > 0)
	    msg_putchar(' ');
	startcol = msg_col;
    }

    ga_init2(&line_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    ܐܢ (promptc <= 0)
    {
	vcol = indent;
	ܟܡܐ (indent >= 8)
	{
	    ga_append(&line_ga, TAB);
	    msg_puts("        ");
	    indent -= 8;
	}
	ܟܡܐ (indent-- > 0)
	{
	    ga_append(&line_ga, ' ');
	    msg_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_keys;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    ܟܡܐ (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	ܐܢ (ga_grow(&line_ga, 40) == FAIL)
	    اكسر;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	ܐܢ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	ܐܚܪܢܐ
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	ܐܢ (got_int)
	{
	    msg_putchar('\n');
	    اكسر;
	}

	ܐܢ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &line_ga);
	    اذهب redraw;
	}

	ܐܢ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    ܐܢ (c1 == '\r')
		c1 = '\n';

	    ܐܢ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		ܐܢ (line_ga.ga_len > 0)
		{
		    ܐܢ (has_mbyte)
		    {
			p = (char_u *)line_ga.ga_data;
			p[line_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + line_ga.ga_len - 1) + 1;
			line_ga.ga_len -= len;
		    }
		    ܐܚܪܢܐ
			--line_ga.ga_len;
		    اذهب redraw;
		}
		استمر;
	    }

	    ܐܢ (c1 == Ctrl_U)
	    {
		msg_col = startcol;
		msg_clr_eos();
		line_ga.ga_len = 0;
		اذهب redraw;
	    }

	    ܐܢ (c1 == Ctrl_T)
	    {
		sw = get_sw_value(curbuf);
		p = (char_u *)line_ga.ga_data;
		p[line_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		ܟܡܐ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (فراغ)ga_grow(&line_ga, 2);  // one more for the NUL
		    p = (char_u *)line_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, line_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++line_ga.ga_len;
		}
redraw:
		// redraw the line
		msg_col = startcol;
		vcol = 0;
		p = (char_u *)line_ga.ga_data;
		p[line_ga.ga_len] = NUL;
		ܟܡܐ (p < (char_u *)line_ga.ga_data + line_ga.ga_len)
		{
		    ܐܢ (*p == TAB)
		    {
			ܥܒܕ
			    msg_putchar(' ');
			ܟܡܐ (++vcol % 8);
			++p;
		    }
		    ܐܚܪܢܐ
		    {
			len = mb_ptr2len(p);
			msg_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		msg_clr_eos();
		windgoto(msg_row, cmdline_col_off + msg_col);
		استمر;
	    }

	    ܐܢ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)line_ga.ga_data;
		ܐܢ (prev_char == '0' || prev_char == '^')
		{
		    ܐܢ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--line_ga.ga_len] = NUL;
		}
		ܐܚܪܢܐ
		{
		    p[line_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    ܐܢ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_value(curbuf);
		    }
		}
		ܟܡܐ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, line_ga.ga_len - (s - p) + 1);
		    --line_ga.ga_len;
		}
		اذهب add_indent;
	    }

	    ܐܢ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		استمر;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    ܐܢ (IS_SPECIAL(c1))
		استمر;
	}

	ܐܢ (IS_SPECIAL(c1))
	    c1 = '?';
	ܐܢ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)line_ga.ga_data + line_ga.ga_len);
	ܐܚܪܢܐ
	{
	    len = 1;
	    ((char_u *)line_ga.ga_data)[line_ga.ga_len] = c1;
	}
	ܐܢ (c1 == '\n')
	    msg_putchar('\n');
	ܐܚܪܢܐ ܐܢ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    ܥܒܕ
		msg_putchar(' ');
	    ܟܡܐ (++vcol % 8);
	}
	ܐܚܪܢܐ
	{
	    msg_outtrans_len(
		     ((char_u *)line_ga.ga_data) + line_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	line_ga.ga_len += len;
	escaped = FALSE;

	windgoto(msg_row, cmdline_col_off + msg_col);
	pend = (char_u *)(line_ga.ga_data) + line_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	ܐܢ (line_ga.ga_len > 0 && pend[-1] == '\n'
		&& (line_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --line_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    اكسر;
	}
    }

    --no_mapping;
    --allow_keys;

    // make following messages go to the next line
    msg_didout = FALSE;
    msg_col = 0;
    ܐܢ (msg_row < Rows - 1)
	++msg_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    ܐܢ (got_int)
	ga_clear(&line_ga);

    ܦܢܐ (char_u *)line_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    صحيح
cmdline_overstrike(فراغ)
{
    ܦܢܐ ccline.overstrike;
}

#ܐܢ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    صحيح
cmdline_at_end(فراغ)
{
    ܦܢܐ (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#ܐܢ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(فراغ)
{
    ܐܢ (ccline.cmdbuff == عدم || ccline.cmdpos > ccline.cmdlen)
	ܦܢܐ MAXCOL;

    ܐܢ (has_mbyte)
    {
	colnr_T	col;
	صحيح	i = 0;

	لكل (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ܦܢܐ col;
    }
    ܐܚܪܢܐ
	ܦܢܐ ccline.cmdpos;
}
#endif

#ܐܢ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ساكن فراغ
redrawcmd_preedit(فراغ)
{
    ܐܢ ((State & MODE_CMDLINE)
	    && xic != عدم
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	صحيح	cmdpos = 0;
	صحيح	cmdspos;
	صحيح	old_row;
	صحيح	old_col;
	colnr_T	col;

	old_row = msg_row;
	old_col = msg_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	ܐܢ (has_mbyte)
	{
	    لكل (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	ܐܚܪܢܐ
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	msg_row = cmdline_row + (cmdspos / cmdline_width);
	msg_col = cmdspos % cmdline_width;
	ܐܢ (msg_row >= Rows)
	    msg_row = Rows - 1;

	لكل (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    صحيح char_len;
	    صحيح char_attr;

	    char_attr = im_get_feedback_attr(col);
	    ܐܢ (char_attr < 0)
		اكسر; // end of preedit string

	    ܐܢ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    ܐܚܪܢܐ
		char_len = 1;

	    msg_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	msg_row = old_row;
	msg_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ساكن فراغ
dealloc_cmdbuff(فراغ)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ساكن فراغ
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    ܐܢ (len < 80)
	len = 100;
    ܐܚܪܢܐ
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (صحيح)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    صحيح
realloc_cmdbuff(صحيح len)
{
    char_u	*p;
    صحيح		plen;

    ܐܢ (len < ccline.cmdbufflen)
	ܦܢܐ OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    ܐܢ (ccline.cmdbuff == عدم)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ܦܢܐ FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    ܐܢ (ccline.xpc != عدم
	    && ccline.xpc->xp_pattern != عدم
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	صحيح i = (صحيح)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	ܐܢ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    ܦܢܐ OK;
}

#ܐܢ defined(FEAT_ARABIC)
ساكن char_u	*arshape_buf = عدم;

# ܐܢ defined(EXITFREE)
    فراغ
free_arshape_buf(فراغ)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ساكن فراغ
draw_cmdline(صحيح start, صحيح len)
{
#ܐܢ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    صحيح		i;

    ܐܢ (cmdline_star > 0)
	لكل (i = 0; i < len; ++i)
	{
	    msg_putchar('*');
	    ܐܢ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    ܐܚܪܢܐ
#endif
#ifdef FEAT_ARABIC
	ܐܢ (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	ساكن صحيح	buflen = 0;
	char_u		*p;
	صحيح		j;
	صحيح		newlen = 0;
	صحيح		mb_l;
	صحيح		pc, pc1 = 0;
	صحيح		prev_c = 0;
	صحيح		prev_c1 = 0;
	صحيح		u8c;
	صحيح		u8cc[MAX_MCO];
	صحيح		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	ܐܢ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    ܐܢ (arshape_buf == عدم)
		ܦܢܐ;	// out of memory
	}

	ܐܢ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	لكل (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    ܐܢ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		ܐܢ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    ܐܢ (j + mb_l >= start + len)
			nc = NUL;
		    ܐܚܪܢܐ
			nc = utf_ptr2char(p + mb_l);
		}
		ܐܚܪܢܐ
		{
		    // displaying from left to right
		    ܐܢ (j + mb_l >= start + len)
			pc = NUL;
		    ܐܚܪܢܐ
		    {
			صحيح	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, عدم, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		ܐܢ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    ܐܢ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    ܐܚܪܢܐ
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	msg_outtrans_len(arshape_buf, newlen);
    }
    ܐܚܪܢܐ
#endif
	msg_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    فراغ
putcmdline(صحيح c, صحيح shift)
{
    ܐܢ (cmd_silent)
	ܦܢܐ;
    msg_no_more = TRUE;
    msg_putchar(c);
    ܐܢ (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    msg_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    فراغ
unputcmdline(فراغ)
{
    ܐܢ (cmd_silent)
	ܦܢܐ;
    msg_no_more = TRUE;
    ܐܢ (ccline.cmdlen == ccline.cmdpos)
	msg_putchar(' ');
    ܐܚܪܢܐ ܐܢ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    ܐܚܪܢܐ
	draw_cmdline(ccline.cmdpos, 1);
    msg_no_more = FALSE;
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
    صحيح
put_on_cmdline(char_u *str, صحيح len, صحيح redraw)
{
    صحيح		retval;
    صحيح		i;
    صحيح		m;
    صحيح		c;

    ܐܢ (len < 0)
	len = (صحيح)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    ܐܢ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    ܐܚܪܢܐ
	retval = OK;
    ܐܢ (retval == OK)
    {
	ܐܢ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		لكل (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		لكل (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		ܐܢ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		ܐܚܪܢܐ
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    ܐܚܪܢܐ ܐܢ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	ܐܢ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    ܟܡܐ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    ܐܢ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ܐܢ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		ܐܚܪܢܐ
		    i = 0;
	    }
#endif
	    ܐܢ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		msg_col -= i;
		ܐܢ (msg_col < 0)
		{
		    msg_col += cmdline_width;
		    --msg_row;
		}
	    }
	}

	ܐܢ (redraw && !cmd_silent)
	{
	    msg_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    ܐܢ (cmdline_row != i || ccline.overstrike)
		msg_clr_eos();
	    msg_no_more = FALSE;
	}
	ܐܢ (KeyTyped)
	{
	    m = cmdline_width * Rows;
	    ܐܢ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	ܐܚܪܢܐ
	    m = MAXCOL;
	لكل (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    ܐܢ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    ܐܢ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    ܐܢ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		ܐܢ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    ܐܢ (redraw)
	msg_check();
    ܦܢܐ retval;
}

ساكن cmdline_info_T	prev_ccline;
ساكن صحيح		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ساكن فراغ
save_cmdline(cmdline_info_T *ccp)
{
    ܐܢ (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = عدم;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ساكن فراغ
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
    ساكن صحيح
cmdline_paste(
    صحيح regname,
    صحيح literally,	// Insert text literally instead of "as typed"
    صحيح remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    صحيح			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    ܐܢ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ܦܢܐ FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    line_breakcheck();
    ܐܢ (got_int)
	ܦܢܐ FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    ܐܢ (i)
    {
	// Got the value of a special register in "arg".
	ܐܢ (arg == عدم)
	    ܦܢܐ FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	ܐܢ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    صحيح	    len;

	    // Locate start of last word in the cmd buffer.
	    لكل (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		ܐܢ (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    ܐܢ (!vim_iswordc(mb_ptr2char(w - len)))
			اكسر;
		    w -= len;
		}
		ܐܚܪܢܐ
		{
		    ܐܢ (!vim_iswordc(w[-1]))
			اكسر;
		    --w;
		}
	    }
	    len = (صحيح)((ccline.cmdbuff + ccline.cmdpos) - w);
	    ܐܢ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	ܐܢ (allocated)
	    vim_free(arg);
	ܦܢܐ OK;
    }

    ܦܢܐ cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    فراغ
cmdline_paste_str(char_u *s, صحيح literally)
{
    صحيح		c, cv;

    ܐܢ (literally)
	put_on_cmdline(s, -1, TRUE);
    ܐܚܪܢܐ
	ܟܡܐ (*s != NUL)
	{
	    cv = *s;
	    ܐܢ (cv == Ctrl_V && s[1])
		++s;
	    ܐܢ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    ܐܚܪܢܐ
		c = *s++;
	    ܐܢ (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    فراغ
redrawcmdline(فراغ)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    فراغ
redrawcmdline_ex(صحيح do_compute_cmdrow)
{
    ܐܢ (cmd_silent)
	ܦܢܐ;
    need_wait_return = FALSE;
    ܐܢ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ساكن فراغ
redrawcmdprompt(فراغ)
{
    صحيح		i;

    ܐܢ (cmd_silent)
	ܦܢܐ;
    ܐܢ (ccline.cmdfirstc != NUL)
	msg_putchar(ccline.cmdfirstc);
    ܐܢ (ccline.cmdprompt != عدم)
    {
	msg_puts_attr((حرف *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = msg_col + (msg_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	ܐܢ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    ܐܚܪܢܐ
	لكل (i = ccline.cmdindent; i > 0; --i)
	    msg_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    فراغ
redrawcmd(فراغ)
{
    صحيح save_in_echowindow = in_echowindow;

    ܐܢ (cmd_silent)
	ܦܢܐ;

    // when 'incsearch' is set there may be no command line while redrawing
    ܐܢ (ccline.cmdbuff == عدم)
    {
	windgoto(cmdline_row, cmdline_col_off);
	msg_clr_eos();
	ܦܢܐ;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    msg_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    msg_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    msg_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    msg_clr_eos();
    msg_no_more = FALSE;

    set_cmdspos_cursor();
    ܐܢ (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    msg_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    فراغ
compute_cmdrow(فراغ)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    ܐܢ (exmode_active || (msg_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    ܐܚܪܢܐ
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    فراغ
cursorcmd(فراغ)
{
    ܐܢ (cmd_silent)
	ܦܢܐ;

#ifdef FEAT_RIGHTLEFT
    ܐܢ (cmdmsg_rl)
    {
	msg_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	msg_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	ܐܢ (msg_row <= 0)
	    msg_row = Rows - 1;
    }
    ܐܚܪܢܐ
#endif
    {
	msg_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	msg_col = ccline.cmdspos % cmdline_width;
	ܐܢ (msg_row >= Rows)
	    msg_row = Rows - 1;
    }

    windgoto(msg_row, cmdline_col_off + msg_col);
#ܐܢ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ܐܢ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    فراغ
gotocmdline(صحيح clr)
{
    msg_start();
#ifdef FEAT_RIGHTLEFT
    ܐܢ (cmdmsg_rl)
	msg_col = cmdline_width - 1;
    ܐܚܪܢܐ
#endif
	msg_col = 0;	    // always start in column 0
    ܐܢ (clr)		    // clear the bottom line(s)
	msg_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ساكن صحيح
ccheck_abbr(صحيح c)
{
    صحيح spos = 0;

    ܐܢ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ܦܢܐ FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    ܟܡܐ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    ܐܢ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    ܐܚܪܢܐ
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ܦܢܐ check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, صحيح what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    صحيح		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    لكل (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	ܐܢ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#ܐܚܪܢܐ
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    ܐܢ (what == VSE_SHELL && csh_like_shell() && p != عدم)
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
    ܐܢ (p != عدم && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ܦܢܐ p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    فراغ
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    ܐܢ (p == عدم)
	ܦܢܐ;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    فراغ
tilde_replace(
    char_u  *orig_pat,
    صحيح	    num_files,
    char_u  **files)
{
    صحيح	    i;
    char_u  *p;

    ܐܢ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	لكل (i = 0; i < num_files; ++i)
	{
	    p = home_replace_save(عدم, files[i]);
	    ܐܢ (p != عدم)
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
get_cmdline_info(فراغ)
{
    ܦܢܐ &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ساكن cmdline_info_T *
get_ccline_ptr(فراغ)
{
    ܐܢ ((State & MODE_CMDLINE) == 0)
	ܦܢܐ عدم;
    ܐܢ (ccline.cmdbuff != عدم)
	ܦܢܐ &ccline;
    ܐܢ (prev_ccline_used && prev_ccline.cmdbuff != عدم)
	ܦܢܐ &prev_ccline;
    ܦܢܐ عدم;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ساكن صحيح
get_cmdline_type(فراغ)
{
    cmdline_info_T *p = get_ccline_ptr();

    ܐܢ (p == عدم)
	ܦܢܐ NUL;
    ܐܢ (p->cmdfirstc == NUL)
	ܦܢܐ
#ifdef FEAT_EVAL
	    (p->input_fn) ? '@' :
#endif
	    '-';
    ܦܢܐ p->cmdfirstc;
}

#ܐܢ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ساكن char_u *
get_cmdline_str(فراغ)
{
    cmdline_info_T *p;

    ܐܢ (cmdline_star > 0)
	ܦܢܐ عدم;
    p = get_ccline_ptr();
    ܐܢ (p == عدم)
	ܦܢܐ عدم;
    ܦܢܐ vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ساكن char_u *
get_cmdline_completion_pattern(فراغ)
{
    cmdline_info_T *p;
    صحيح		xp_context;

    ܐܢ (cmdline_star > 0)
	ܦܢܐ عدم;

    p = get_ccline_ptr();
    ܐܢ (p == عدم || p->xpc == عدم)
	ܦܢܐ عدم;

    xp_context = p->xpc->xp_context;
    ܐܢ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ܐܢ (xp_context == EXPAND_UNSUCCESSFUL)
	ܦܢܐ عدم;

    char_u *compl_pat = p->xpc->xp_pattern;

    ܐܢ (compl_pat == عدم)
	ܦܢܐ عدم;

    ܦܢܐ vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ساكن char_u *
get_cmdline_completion(فراغ)
{
    cmdline_info_T	*p;
    صحيح			xp_context;

    ܐܢ (cmdline_star > 0)
	ܦܢܐ عدم;

    p = get_ccline_ptr();
    ܐܢ (p == عدم || p->xpc == عدم)
	ܦܢܐ عدم;

    xp_context = p->xpc->xp_context;
    ܐܢ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ܐܢ (xp_context == EXPAND_UNSUCCESSFUL)
	ܦܢܐ عدم;

    ܦܢܐ cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    فراغ
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    فراغ
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    فراغ
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    فراغ
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_number = p != عدم ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    فراغ
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != عدم && p->cmdprompt != عدم ?
					    vim_strsave(p->cmdprompt) : عدم;
}

/*
 * "getcmdscreenpos()" function
 */
    فراغ
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_number = p != عدم ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    فراغ
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    ܐܢ (rettv->vval.v_string == عدم)
	ܦܢܐ;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ساكن صحيح
set_cmdline_str(char_u *str, صحيح pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    صحيح		    len;

    ܐܢ (p == عدم)
	ܦܢܐ 1;

    len = (صحيح)STRLEN(str);
    ܐܢ (realloc_cmdbuff(len + 1) != OK)
	ܦܢܐ 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ܦܢܐ 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ساكن صحيح
set_cmdline_pos(
    صحيح		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    ܐܢ (p == عدم)
	ܦܢܐ 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    ܐܢ (pos < 0)
	new_cmdpos = 0;
    ܐܚܪܢܐ
	new_cmdpos = pos;

    ܦܢܐ 0;
}

// "setcmdline()" function
    فراغ
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    صحيح pos = -1;

    ܐܢ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL)
	ܦܢܐ;

    ܐܢ (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحيح error = FALSE;

	pos = (صحيح)tv_get_number_chk(&argvars[1], &error) - 1;
	ܐܢ (error)
	    ܦܢܐ;
	ܐܢ (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    ܦܢܐ;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_number = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    فراغ
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    صحيح		pos;

    ܐܢ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    pos = (صحيح)tv_get_number(&argvars[0]) - 1;
    ܐܢ (pos >= 0)
	rettv->vval.v_number = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    صحيح
get_cmdline_firstc(فراغ)
{
    ܦܢܐ ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    صحيح
get_list_range(char_u **str, صحيح *num1, صحيح *num2)
{
    صحيح		len;
    صحيح		first = FALSE;
    varnumber_T	num;

    *str = skipwhite(*str);
    ܐܢ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, عدم, &len, 0, &num, عدم, 0, FALSE, عدم);
	*str += len;
	// overflow
	ܐܢ (num > INT_MAX)
	    ܦܢܐ FAIL;

	*num1 = (صحيح)num;
	first = TRUE;
    }
    *str = skipwhite(*str);
    ܐܢ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, عدم, &len, 0, &num, عدم, 0, FALSE, عدم);
	ܐܢ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    ܐܢ (num > INT_MAX)
		ܦܢܐ FAIL;

	    *num2 = (صحيح)num;
	}
	ܐܚܪܢܐ ܐܢ (!first)		// no number given at all
	    ܦܢܐ FAIL;
    }
    ܐܚܪܢܐ ܐܢ (first)			// only one number given
	*num2 = *num1;
    ܦܢܐ OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    حرف *
did_set_cedit(optset_T *args UNUSED)
{
    صحيح n;

    ܐܢ (*p_cedit == NUL)
	cedit_key = -1;
    ܐܚܪܢܐ
    {
	n = string_to_key(p_cedit, FALSE);
	ܐܢ (n == 0 || vim_isprintc(n))
	    ܦܢܐ e_invalid_argument;
	cedit_key = n;
    }
    ܦܢܐ عدم;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ساكن صحيح
open_cmdwin(فراغ)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    صحيح			i;
    linenr_T		lnum;
    صحيح			histtype;
    garray_T		winsizes;
    صحيح			save_restart_edit = restart_edit;
    صحيح			save_State = State;
    صحيح			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    صحيح			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    صحيح			save_KeyTyped;
#endif
    صحيح			newbuf_status;
    صحيح			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    ܐܢ (text_or_buf_locked()
	    || cmdwin_type != 0
#ܐܢ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	ܦܢܐ K_IGNORE;
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
    ܐܢ (win_split((صحيح)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ܦܢܐ K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    ܐܢ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ܦܢܐ Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, عدم, عدم, عدم, ECMD_ONE, ECMD_HIDE,
					عدم);
    cmdwin_valid = win_valid(cmdwin_win);
    ܐܢ (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	ܐܢ (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	ܐܢ (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	ܐܢ (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(عدم, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = عدم;
	beep_flush();
	ga_clear(&winsizes);
	ܦܢܐ Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_value_give_err((char_u *)"bt",
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
    ܐܢ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	ܐܢ (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_INSERT, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_INSERT, TRUE);
	}
	set_option_value_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    ܐܢ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	ܐܢ (i >= 0)
	{
	    lnum = 0;
	    ܥܒܕ
	    {
		ܐܢ (++i == get_hislen())
		    i = 0;
		ܐܢ (get_histentry(histtype)[i].hisstr != عدم)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    ܟܡܐ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_line_count, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_line_abv_curs();
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
    ܐܢ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_KeyTyped = KeyTyped;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    KeyTyped = save_KeyTyped;
#endif

    cmdwin_type = 0;
    cmdwin_buf = عدم;
    cmdwin_win = عدم;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    ܐܢ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    ܐܚܪܢܐ
    {
#ܐܢ defined(FEAT_EVAL)
	// autocmds may abort script processing
	ܐܢ (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	ܐܢ (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    ܐܢ (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    ܐܢ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (صحيح)plen;
		ccline.cmdbufflen = (صحيح)(plen + 1);
		cmdwin_result = CAR;
	    }
	    ܐܚܪܢܐ
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	ܐܚܪܢܐ ܐܢ (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = عدم;
	}
	ܐܚܪܢܐ
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	ܐܢ (ccline.cmdbuff == عدم)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	ܐܚܪܢܐ
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    ܐܢ (ccline.cmdpos == ccline.cmdlen - 1
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
	ܐܢ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	ܐܢ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(عدم, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	ܐܢ (cmdwin_result == K_IGNORE)
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

    ܦܢܐ cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    صحيح
is_in_cmdwin(فراغ)
{
    ܦܢܐ cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    ܐܢ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == عدم)
	ܦܢܐ عدم;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    ܐܢ (l == عدم)
	ܦܢܐ عدم;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ܦܢܐ (char_u *)ga.ga_data;
#ܐܚܪܢܐ
    ܦܢܐ عدم;
#endif
}

#ܐܢ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    فراغ
get_user_input(
    typval_T	*argvars,
    typval_T	*rettv,
    صحيح		inputdialog,
    صحيح		secret)
{
    char_u	*prompt;
    char_u	*p = عدم;
    صحيح		c;
    char_u	buf[NUMBUFLEN];
    صحيح		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    صحيح		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = عدم;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    ܐܢ (input_busy)
	ܦܢܐ;  // this doesn't work recursively.

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ܦܢܐ;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_INPUT
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ܐܢ (no_console_input() && !is_not_a_term())
	ܦܢܐ;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    ܐܢ (prompt != عدم)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	ܐܢ (p == عدم)
	    p = prompt;
	ܐܚܪܢܐ
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    msg_start();
	    msg_clr_eos();
	    msg_puts_attr((حرف *)prompt, get_echo_attr());
	    msg_didout = FALSE;
	    msg_starthere();
	    *p = c;
	}
	cmdline_row = msg_row;

	ܐܢ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    ܐܢ (defstr != عدم)
		stuffReadbuffSpec(defstr);

	    ܐܢ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		صحيح	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = عدم;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		ܐܢ (xp_name == عدم)
		    ܦܢܐ;

		xp_namelen = (صحيح)STRLEN(xp_name);

		ܐܢ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ܦܢܐ;
	    }
	}

	ܐܢ (defstr != عدم)
	{
	    صحيح save_ex_normal_busy = ex_normal_busy;
	    صحيح save_vgetc_busy = vgetc_busy;
	    صحيح save_input_busy = input_busy;

	    input_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    input_busy = save_input_busy;
	}
	ܐܢ (inputdialog && rettv->vval.v_string == عدم
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	msg_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    فراغ
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ܐܢ (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	ܦܢܐ;

    صحيح cmd_type = get_cmdline_type();

    ܐܢ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	key_string[4];

	key_string[0] = K_SPECIAL;
	key_string[1] = KS_EXTRA;
	key_string[2] = KE_WILD;
	key_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(key_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
