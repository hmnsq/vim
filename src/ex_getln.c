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

#அந்தர்பாவ "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_சாவி	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
நிலையான cmdline_info_T ccline;

#ifdef FEAT_EVAL
நிலையான முழு	new_cmdpos;	// position set by set_cmdline_pos()
#endif

நிலையான முழு	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
நிலையான முழு	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
நிலையான முழு	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

நிலையான char_u	*getcmdline_int(முழு firstc, long மொத்தம், முழு indent, முழு clear_ccline);
நிலையான முழு	cmdline_charsize(முழு idx);
நிலையான வெறுமை	set_cmdspos(வெறுமை);
நிலையான வெறுமை	set_cmdspos_cursor(வெறுமை);
நிலையான வெறுமை	correct_cmdspos(முழு idx, முழு cells);
நிலையான வெறுமை	dealloc_cmdbuff(வெறுமை);
நிலையான வெறுமை	alloc_cmdbuff(size_t len);
நிலையான வெறுமை	draw_cmdline(முழு start, முழு len);
நிலையான வெறுமை	save_cmdline(cmdline_info_T *ccp);
நிலையான வெறுமை	restore_cmdline(cmdline_info_T *ccp);
நிலையான முழு	cmdline_paste(முழு regname, முழு literally, முழு remcr);
நிலையான வெறுமை	redrawcmdprompt(வெறுமை);
நிலையான முழு	ccheck_abbr(முழு);
நிலையான முழு	open_cmdwin(வெறுமை);
#ifdef FEAT_தேடல்_EXTRA
நிலையான முழு	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

நிலையான முழு	cedit_சாவி = -1;	// key value of 'cedit' option

    நிலையான வெறுமை
trigger_cmd_autocmd(முழு typechar, முழு evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    நிலையான வெறுமை
abandon_cmdline(வெறுமை)
{
    dealloc_cmdbuff();
    எனில் (செய்தி_scrolled == 0)
	compute_cmdrow();
    செய்தி("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_தேடல்_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    நிலையான முழு
empty_pattern(char_u *p, size_t len, முழு delim)
{
    magic_T	magic_val = MAGIC_ON;

    எனில் (len > 0)
	(வெறுமை) skip_regexp_ex(p, delim, magic_isset(), வெற்று, வெற்று, &magic_val);
    இல்லையெனில்
	திரும்பு TRUE;

    திரும்பு empty_pattern_magic(p, len, magic_val);
}

    நிலையான முழு
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    போது (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != வெற்று)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    திரும்பு len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
வகைவரை கட்டமைப்பு {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    முழு		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    நிலையான வெறுமை
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

    நிலையான வெறுமை
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
வகைவரை கட்டமைப்பு {
    pos_T	தேடல்_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    முழு		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    முழு		did_incsearch;
    முழு		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    நிலையான வெறுமை
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->தேடல்_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    நிலையான வெறுமை
set_தேடல்_match(pos_T *t)
{
    t->lnum += தேடல்_match_வரிகள்;
    t->col = தேடல்_match_endcol;
    எனில் (t->lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	t->lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    முழு
parse_pattern_and_range(
	pos_T	*incsearch_start,
	முழு	*தேடல்_delim,
	முழு	*skiplen,
	முழு	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    முழு		delim_optional = FALSE;
    முழு		delim;
    முழு		use_last_pat;
    magic_T     magic = 0;
    எழுத்து	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    தேடல்_first_வரி = 0;
    தேடல்_last_வரி = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_வரிகள்;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, வெற்று);

    எனில் (vim_strchr((char_u *)"sgvlu", *cmd) == வெற்று)
	திரும்பு FALSE;

    // Skip over command name to find pattern separator
    க்கு (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    எனில் (*skipwhite(p) == NUL)
	திரும்பு FALSE;

    எனில் (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	எனில் (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	இல்லையெனில் எனில் (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    இல்லையெனில் எனில் (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	எனில் (*p == '!')
	    p = skipwhite(p + 1);
	போது (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	எனில் (*p == NUL)
	    திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	எனில் (*p == '!')
	{
	    p++;
	    எனில் (*skipwhite(p) == NUL)
		திரும்பு FALSE;
	}
	எனில் (*cmd != 'g')
	    delim_optional = TRUE;
    }
    இல்லையெனில்
	திரும்பு FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *தேடல்_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), வெற்று, வெற்று, &magic);
    use_last_pat = end == p && *end == delim;

    எனில் (end == p && !use_last_pat)
	திரும்பு FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    எனில் (!use_last_pat)
    {
	எழுத்து c = *end;
	முழு empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	எனில் (empty)
	    திரும்பு FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (முழு)(p - ccline.cmdbuff);
    *patlen = (முழு)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    எனில் (ea.addr_மொத்தம் > 0)
    {
	முழு reverse_match = ea.line2 < ea.line1;
	தேடல்_first_வரி = reverse_match ? ea.line2 : ea.line1;
	தேடல்_last_வரி = reverse_match ? ea.line1 : ea.line2;
    }
    இல்லையெனில் எனில் (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	தேடல்_first_வரி = தேடல்_last_வரி = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    திரும்பு TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    நிலையான முழு
do_incsearch_highlighting(
	முழு		    firstc,
	முழு		    *தேடல்_delim,
	incsearch_state_T   *is_state,
	முழு		    *skiplen,
	முழு		    *patlen)
{
    முழு retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    எனில் (!p_is || cmd_silent)
	திரும்பு FALSE;

    // By default search all lines
    தேடல்_first_வரி = 0;
    தேடல்_last_வரி = MAXLNUM;

    எனில் (firstc == '/' || firstc == '?')
    {
	*தேடல்_delim = firstc;
	திரும்பு TRUE;
    }

    எனில் (firstc != ':')
	திரும்பு FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->தேடல்_start, தேடல்_delim,
	    skiplen, patlen);
    --emsg_off;

    திரும்பு retval;
}

    நிலையான வெறுமை
finish_incsearch_highlighting(
	முழு gotesc,
	incsearch_state_T *is_state,
	முழு call_update_screen)
{
    எனில் (!is_state->did_incsearch)
	திரும்பு;

    is_state->did_incsearch = FALSE;
    எனில் (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    இல்லையெனில்
    {
	எனில் (!EQUAL_POS(is_state->save_cursor, is_state->தேடல்_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->தேடல்_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    தேடல்_first_வரி = 0;
    தேடல்_last_வரி = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    எனில் (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    நிலையான வெறுமை
may_do_incsearch_highlighting(
	முழு		    firstc,
	long		    மொத்தம்,
	incsearch_state_T   *is_state)
{
    முழு		skiplen, patlen;
    முழு		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    முழு		next_char;
    முழு		use_last_pat;
    முழு		did_do_incsearch = is_state->did_incsearch;
    முழு		தேடல்_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_தேடல்_pattern();

    எனில் (!do_incsearch_highlighting(firstc, &தேடல்_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_தேடல்_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	எனில் (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	திரும்பு;
    }

    // If there is a character waiting, search and redraw later.
    எனில் (char_avail())
    {
	restore_last_தேடல்_pattern();
	is_state->incsearch_postponed = TRUE;
	திரும்பு;
    }
    is_state->incsearch_postponed = FALSE;

    எனில் (தேடல்_first_வரி == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->தேடல்_start;
    இல்லையெனில் எனில் (தேடல்_first_வரி > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	curwin->w_cursor.col = MAXCOL;
    }
    இல்லையெனில்
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = தேடல்_first_வரி;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    எனில் (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    இல்லையெனில்
    {
	முழு தேடல்_flags = தேடல்_OPT + தேடல்_NOOF + தேடல்_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	எனில் (!p_hls)
	    தேடல்_flags += தேடல்_KEEP;
	எனில் (தேடல்_first_வரி != 0)
	    தேடல்_flags += தேடல்_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_தேடல்(வெற்று, firstc == ':' ? '/' : firstc, தேடல்_delim,
				 ccline.cmdbuff + skiplen, patlen, மொத்தம், தேடல்_flags,
# ifdef FEAT_RELTIME
		&sia
# இல்லையெனில்
		வெற்று
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	எனில் (curwin->w_cursor.lnum < தேடல்_first_வரி
		|| curwin->w_cursor.lnum > தேடல்_last_வரி)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->தேடல்_start;
	}

	// if interrupted while searching, behave like it failed
	எனில் (got_int)
	{
	    (வெறுமை)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	இல்லையெனில் எனில் (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    எனில் (found != 0)
	highlight_match = TRUE;		// highlight position
    இல்லையெனில்
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    எனில் (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_தேடல்_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    இல்லையெனில்
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    எனில் (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	எனில் (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, தேடல்_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    எனில் (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_தேடல்_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    எனில் (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->தேடல்_start;
    இல்லையெனில் எனில் (found != 0)
	curwin->w_cursor = end_pos;

    செய்தி_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    நிலையான முழு
may_adjust_incsearch_highlighting(
	முழு			firstc,
	long			மொத்தம்,
	incsearch_state_T	*is_state,
	முழு			c)
{
    முழு		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = வெற்று;
    char_u	*searchstr;
    char_u	*strcopy = வெற்று;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    முழு		தேடல்_flags = தேடல்_NOOF;
    முழு		i;
    முழு		தேடல்_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_தேடல்_pattern();

    எனில் (!do_incsearch_highlighting(firstc, &தேடல்_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_தேடல்_pattern();
	திரும்பு OK;
    }
    எனில் (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_தேடல்_pattern();
	திரும்பு FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (வெறுமை)parse_தேடல்_pattern_offset(&pat, &patlen_s, தேடல்_delim,
				    தேடல்_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    எனில் (c == Ctrl_G)
    {
	t = is_state->match_end;
	எனில் (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (வெறுமை)decl(&t);
	தேடல்_flags += தேடல்_COL;
    }
    இல்லையெனில்
	t = is_state->match_start;
    எனில் (!p_hls)
	தேடல்_flags += தேடல்_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, வெற்று,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, மொத்தம், தேடல்_flags, RE_தேடல், வெற்று);
    --emsg_off;
    எனில் (dircp != வெற்று)
	*dircp = தேடல்_delim;
    எனில் (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->தேடல்_start = match_start;
	எனில் (!offset.வரி && (offset.end || off != 0))
	{
	    எனில் (offset.end)
	    {
		is_state->தேடல்_start = match_end;
		(வெறுமை)decl(&is_state->தேடல்_start);
	    }
	    போது (off > 0)
	    {
		எனில் (incl(&is_state->தேடல்_start) == -1)
		    நிறுத்து;
		--off;
	    }
	    போது (off < 0)
	    {
		எனில் (decl(&is_state->தேடல்_start) == -1)
		    நிறுத்து;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	எனில் (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->தேடல்_start = t;
	    (வெறுமை)decl(&is_state->தேடல்_start);
	}
	இல்லையெனில் எனில் (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->தேடல்_start = t;
	    (வெறுமை)incl(&is_state->தேடல்_start);
	}
	எனில் (LT_POS(t, is_state->தேடல்_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->தேடல்_start = t;
	    எனில் (firstc == '?')
		(வெறுமை)incl(&is_state->தேடல்_start);
	    இல்லையெனில்
		(வெறுமை)decl(&is_state->தேடல்_start);
	}

	set_தேடல்_match(&is_state->match_end);
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
    இல்லையெனில்
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_தேடல்_pattern();
    திரும்பு FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    நிலையான முழு
may_add_char_to_தேடல்(முழு firstc, முழு *c, incsearch_state_T *is_state)
{
    முழு		skiplen, patlen, தேடல்_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_தேடல்_pattern();

    எனில் (!do_incsearch_highlighting(firstc, &தேடல்_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_தேடல்_pattern();
	திரும்பு FAIL;
    }
    restore_last_தேடல்_pattern();

    // Add a character from under the cursor for 'incsearch'.
    எனில் (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	எனில் (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    எனில் (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    எனில் (*c == தேடல்_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != வெற்று)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    எனில் (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		முழு save_c = *c;

		போது (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    நிலையான முழு
cmdline_has_arabic(முழு start, முழு len)
{
    முழு	    j;
    முழு	    mb_l;
    முழு	    u8c;
    char_u  *p;
    முழு	    u8cc[MAX_MCO];

    எனில் (!enc_utf8)
	திரும்பு FALSE;

    க்கு (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	எனில் (ARABIC_CHAR(u8c))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

    வெறுமை
cmdline_init(வெறுமை)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    நிலையான முழு
cmdline_handle_ctrl_bsl(முழு c, முழு *gotesc)
{
    ++no_mapping;
    ++allow_சாவிகள்;
    c = plain_vgetc();
    --no_mapping;
    --allow_சாவிகள்;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    எனில் (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && சாவி_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	திரும்பு PROCESS_NEXT_சாவி;
    }

#ifdef FEAT_EVAL
    எனில் (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	எனில் (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	இல்லையெனில்
	    new_cmdpos = ccline.cmdpos;

	c = get_கோவை_register();
	எனில் (c == '=')
	{
	    char_u	*p = வெற்று;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_கோவை_வரி();
	    --textlock;

	    எனில் (p != வெற்று)
	    {
		முழு len = (முழு)STRLEN(p);

		எனில் (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    எனில் (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    இல்லையெனில்
			ccline.cmdpos = new_cmdpos;

		    சாவி_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    திரும்பு CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	திரும்பு CMDLINE_NOT_CHANGED;
    }
#endif

    எனில் (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    திரும்பு GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    நிலையான முழு
cmdline_wildchar_complete(
	முழு		c,
	முழு		escape,
	முழு		*did_wild_list,
	முழு		*wim_index_p,
	expand_T	*xp,
	முழு		*gotesc,
	முழு		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    முழு		wim_index = *wim_index_p;
    முழு		res;
    முழு		cmdpos_before;
    முழு		options = WILD_NO_BEEP;
    முழு		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    முழு		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    எனில் (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    எனில் (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	எனில் (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (வெறுமை)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	எனில் (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	இல்லையெனில் எனில் (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	இல்லையெனில்
	    res = OK;	    // don't insert 'wildchar' now
    }
    இல்லையெனில்		    // typed p_wc first time
    {
	முழு wim_longest = (wim_flags[0] & WIM_LONGEST);
	முழு wim_list = (wim_flags[0] & WIM_LIST);
	முழு wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	எனில் (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    எனில் (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    எனில் (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    இல்லையெனில்
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	எனில் (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	இல்லையெனில்
	{
	    எனில் (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    எனில் (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	எனில் (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	எனில் (got_int)
	{
	    (வெறுமை)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (வெறுமை)ExpandOne(xp, வெற்று, வெற்று, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    திரும்பு CMDLINE_CHANGED;
	}

	// Display matches
	எனில் (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    எனில் (wim_longest)
	    {
		முழு found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		முழு show_menu = p_wmnu && wim_full;

		எனில் (wim_list || show_menu)
		    (வெறுமை)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		இல்லையெனில் எனில் (!found_longest_prefix)
		{
		    முழு wim_list_next = (wim_flags[1] & WIM_LIST);
		    முழு wim_full_next = (wim_flags[1] & WIM_FULL);
		    முழு wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    முழு wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    எனில் (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			எனில் (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			இல்லையெனில்
			    (வெறுமை)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			எனில் (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    இல்லையெனில்
	    {
		முழு show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		எனில் (wim_list || show_menu)
		    (வெறுமை)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		இல்லையெனில்
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    எனில் (wim_list)
		*did_wild_list = TRUE;
	}
	இல்லையெனில் எனில் (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    எனில் (wim_index < 3)
	++wim_index;
    எனில் (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    திரும்பு (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    நிலையான முழு
cmdline_erase_chars(
	முழு c,
	முழு indent
#ifdef FEAT_தேடல்_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    முழு		i;
    முழு		j;

    எனில் (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    எனில் (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    எனில் (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    எனில் (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	எனில் (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    எனில் (c == Ctrl_W)
	    {
		போது (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		போது (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		எனில் (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	இல்லையெனில் எனில் (c == Ctrl_W)
	{
	    போது (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    எனில் (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		போது (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	இல்லையெனில்
	    --p;
	ccline.cmdpos = (முழு)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	போது (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_தேடல்_EXTRA
	எனில் (ccline.cmdlen == 0)
	{
	    isp->தேடல்_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    இல்லையெனில் எனில் (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == வெற்று && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	எனில் (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    திரும்பு CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	எனில் (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    எனில் (cmdmsg_rl)
		செய்தி_col = cmdline_width;
	    இல்லையெனில்
#endif
		செய்தி_col = 0;
	    செய்தி_putchar(' ');		// delete ':'
	}
#ifdef FEAT_தேடல்_EXTRA
	isp->தேடல்_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	திரும்பு GOTO_NORMAL_MODE;
    }
    திரும்பு CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    நிலையான வெறுமை
cmdline_toggle_langmap(long *b_im_ptr)
{
    எனில் (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_உள்ளீடு_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	எனில் (b_im_ptr != வெற்று)
	{
	    எனில் (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    இல்லையெனில்
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_உள்ளீடு_METHOD
    இல்லையெனில்
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	எனில் ((p_imdisable && b_im_ptr != வெற்று)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    எனில் (b_im_ptr != வெற்று)
		*b_im_ptr = B_IMODE_NONE;
	}
	இல்லையெனில்
	{
	    im_set_active(TRUE);	// Enable input method
	    எனில் (b_im_ptr != வெற்று)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    எனில் (b_im_ptr != வெற்று)
    {
	எனில் (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	இல்லையெனில்
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#எனில் defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    நிலையான முழு
cmdline_செருகு_reg(முழு *gotesc UNUSED)
{
    முழு		i;
    முழு		c;
    முழு		literally = FALSE;
#ifdef FEAT_EVAL
    முழு		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_சாவிகள்;
    i = c = plain_vgetc();	// CTRL-R <char>
    எனில் (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    எனில் (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_சாவிகள்;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    எனில் (c == '=')
    {
	எனில் (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	இல்லையெனில்
	    c = get_கோவை_register();
    }
#endif
    எனில் (c != ESC)	    // use ESC to cancel inserting register
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
	எனில் (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    திரும்பு GOTO_NORMAL_MODE;
	}
#endif
	சாவி_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	எனில் (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    எனில் (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    இல்லையெனில்
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
    திரும்பு literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    நிலையான வெறுமை
cmdline_இடது_வலது_mouse(முழு c, முழு *ignore_drag_release)
{
    எனில் (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    இல்லையெனில்
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    எனில் (!gui.in_use)
#endif
	எனில் (!mouse_has(MOUSE_COMMAND))
	    திரும்பு;
#ifdef FEAT_CLIPBOARD
    எனில் (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	முழு	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	எனில் (mouse_model_popup() && button == MOUSE_இடது
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_வலது;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	திரும்பு;
    }
#endif

    set_cmdspos();
    க்கு (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	முழு	i;

	i = cmdline_charsize(ccline.cmdpos);
	எனில் (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    நிறுத்து;
	எனில் (has_mbyte)
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
    நிலையான முழு
cmdline_browse_history(
	முழு	c,
	முழு	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	முழு	histype,
	முழு	*hiscnt_p,
	expand_T *xp)
{
    முழு		orig_hiscnt;
    முழு		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    முழு		res;

    எனில் (get_hislen() == 0 || firstc == NUL)	// no history
	திரும்பு CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    எனில் (lookfor == வெற்று)
    {
	எனில் ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == வெற்று)
	    திரும்பு CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    க்கு (;;)
    {
	// one step backwards
	எனில் (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    எனில் (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    இல்லையெனில் எனில் (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    இல்லையெனில் எனில் (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    இல்லையெனில்			// at top of list
	    {
		hiscnt = orig_hiscnt;
		நிறுத்து;
	    }
	}
	இல்லையெனில்    // one step forwards
	{
	    // on last entry, clear the line
	    எனில் (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		நிறுத்து;
	    }

	    // not on a history line, nothing to do
	    எனில் (hiscnt == get_hislen())
		நிறுத்து;
	    எனில் (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    இல்லையெனில்
		++hiscnt;
	}
	எனில் (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== வெற்று)
	{
	    hiscnt = orig_hiscnt;
	    நிறுத்து;
	}
	எனில் ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    நிறுத்து;
    }

    எனில் (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	முழு	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	எனில் (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	இல்லையெனில்
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	எனில் (histype == HIST_தேடல்
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    முழு     i;
	    முழு     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    க்கு (i = 0; i <= 1; ++i)
	    {
		len = 0;
		க்கு (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    எனில் (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			எனில் (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    இல்லையெனில்
		    {
			// Escape new sep, unless it is already
			// escaped.
			எனில் (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    எனில் (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			எனில் (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		எனில் (i == 0)
		{
		    alloc_cmdbuff(len);
		    எனில் (ccline.cmdbuff == வெற்று)
		    {
			res = GOTO_NORMAL_MODE;
			செல் done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (முழு)len;
	}
	இல்லையெனில்
	{
	    alloc_cmdbuff(plen);
	    எனில் (ccline.cmdbuff == வெற்று)
	    {
		res = GOTO_NORMAL_MODE;
		செல் done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (முழு)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	செல் done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    திரும்பு res;
}

/*
 * Initialize the current command-line info.
 */
    நிலையான முழு
init_ccline(முழு firstc, முழு indent)
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
    எனில் (ccline.cmdbuff == வெற்று)
    {
	ccline.cmdbufflen = 0;
	திரும்பு FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    எனில் (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    திரும்பு OK;
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
    முழு		  firstc,
    long	  மொத்தம்,	// only used for incremental search
    முழு		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    திரும்பு getcmdline_int(firstc, மொத்தம், indent, TRUE);
}

    நிலையான char_u *
getcmdline_int(
    முழு		firstc,
    long	மொத்தம் UNUSED,	// only used for incremental search
    முழு		indent,		// indent for inside conditionals
    முழு		clear_ccline)	// clear ccline first
{
    நிலையான முழு	depth = 0;	    // call depth
    முழு		c = 0;
    முழு		i;
    முழு		j;
    முழு		gotesc = FALSE;		// TRUE when <ESC> just typed
    முழு		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = வெற்று;	// string to match
    size_t	lookforlen = 0;
    முழு		hiscnt;			// current history line in use
    முழு		histype;		// history type to be used
#ifdef FEAT_தேடல்_EXTRA
    incsearch_state_T	is_state;
#endif
    முழு		did_wild_list = FALSE;	// did wild_list() recently
    முழு		wim_index = 0;		// index in wim_flags[]
    முழு		res;
    முழு		save_செய்தி_scroll = செய்தி_scroll;
    முழு		save_State = State;	// remember State when called
    முழு		some_சாவி_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    முழு		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    முழு		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = வெற்று;
    buf_T	*b_im_ptr_buf = வெற்று;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    முழு		did_save_ccline = FALSE;
    முழு		cmdline_type;
    முழு		wild_type = 0;
    முழு		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = வெற்று;
    முழு		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    எனில் (ccline.cmdbuff != வெற்று)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    எனில் (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    எனில் (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    எனில் (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_தேடல்_EXTRA
    init_incsearch_state(&is_state);
#endif

    எனில் (init_ccline(firstc, indent) != OK)
	செல் theend;	// out of memory

    எனில் (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	செல் theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    இல்லையெனில்
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    எனில் (!cmd_silent)
    {
	i = செய்தி_scrolled;
	செய்தி_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	செய்தி_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#எனில் defined(FEAT_EVAL)
    எனில் (ccline.உள்ளீடு_fn)
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
    செய்தி_scroll = FALSE;

    State = MODE_CMDLINE;

    எனில் (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	எனில் (curbuf->b_p_imsearch == B_IMODE_USE_செருகு)
	    b_im_ptr = &curbuf->b_p_iminsert;
	இல்லையெனில்
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	எனில் (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_உள்ளீடு_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_உள்ளீடு_METHOD
    இல்லையெனில் எனில் (p_imcmdline)
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
    எனில் (!debug_mode)
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
    எனில் (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    எனில் (!cmd_silent && செய்தி_scrolled == 0)
    {
	முழு	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    எனில் (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	எனில் (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	எனில் (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    க்கு (;;)
    {
	முழு	trigger_cmdlinechanged = TRUE;
	முழு	end_wildmenu;
	முழு	prev_cmdpos = ccline.cmdpos;
	முழு	skip_pum_redraw = FALSE;

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

	எனில் (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_சாவி_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	எனில் (ccline.cmdbuff != வெற்று)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    எனில் (prev_cmdbuff == வெற்று)
		செல் returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	எனில் (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	செய்
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} போது (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	எனில் (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    எனில் (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (வெறுமை)ExpandOne(&xpc, வெற்று, வெற்று, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	எனில் (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    தொடர்;
	}

	எனில் (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    முழு	    clen = ccline.cmdlen;
	    முழு	    cc_மொத்தம் = aucmd_cmdline_changed_மொத்தம்;

	    எனில் (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		எனில் (clen == ccline.cmdlen
				    || cc_மொத்தம் != aucmd_cmdline_changed_மொத்தம்)
		    trigger_cmdlinechanged = FALSE;
		செல் cmdline_changed;
	    }
	}

	எனில் (சாவி_Typed)
	{
	    some_சாவி_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    எனில் (cmd_hkmap)
		c = hkmap(c);
	    எனில் (cmdmsg_rl && !சாவி_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		தேர்வு (c)
		{
		    நிலை K_வலது:   c = K_இடது; நிறுத்து;
		    நிலை K_S_வலது: c = K_S_இடது; நிறுத்து;
		    நிலை K_C_வலது: c = K_C_இடது; நிறுத்து;
		    நிலை K_இடது:    c = K_வலது; நிறுத்து;
		    நிலை K_S_இடது:  c = K_S_வலது; நிறுத்து;
		    நிலை K_C_இடது:  c = K_C_வலது; நிறுத்து;
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
	எனில் ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#எனில் defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	எனில் (lookfor != வெற்று
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_இடது && c != K_வலது
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	எனில் (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	எனில் (p_wmnu)
	    c = wildmenu_translate_சாவி(&ccline, c, &xpc, did_wild_list);

	முழு சாவி_is_wc = (c == p_wc && சாவி_Typed) || c == p_wcm;
	எனில் ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !சாவி_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    எனில் (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		எனில் (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    நிறுத்து;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	எனில் (சாவி_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#எனில் defined(FEAT_தேடல்_EXTRA)
	    எனில் ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!சாவி_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	எனில் (end_wildmenu)
	{
	    எனில் (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !சாவி_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    எனில் (xpc.xp_numfiles != -1)
		(வெறுமை)ExpandOne(&xpc, வெற்று, வெற்று, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    எனில் (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	எனில் (p_wmnu)
	    c = wildmenu_process_சாவி(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	எனில் (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    எனில் (res == CMDLINE_CHANGED)
		செல் cmdline_changed;
	    இல்லையெனில் எனில் (res == CMDLINE_NOT_CHANGED)
		செல் cmdline_not_changed;
	    இல்லையெனில் எனில் (res == GOTO_NORMAL_MODE)
		செல் returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	எனில் (c == cedit_சாவி || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    எனில் ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_சாவி_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	இல்லையெனில்
	    c = do_digraph(c);
#endif

	எனில் (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!சாவி_Typed || vim_strchr(p_cpo, CPO_ESC) != வெற்று)))
	{
	    // In Ex mode a backslash escapes a newline.
	    எனில் (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		எனில் (c == K_KENTER)
		    c = '\n';
	    }
	    இல்லையெனில்
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		எனில் (ccheck_abbr(c + ABBR_OFF))
		    செல் cmdline_changed;
		எனில் (!cmd_silent)
		{
		    windgoto(செய்தி_row, cmdline_col_off);
		    out_flush();
		}
		நிறுத்து;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	எனில் ((c == p_wc && !gotesc && சாவி_Typed) || c == p_wcm || c == K_WILD)
	{
	    எனில் (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_தேடல்_EXTRA
		    &is_state.தேடல்_start
#இல்லையெனில்
		    வெற்று
#endif
		    );
	    எனில் (c == K_WILD)
		--emsg_silent;
	    எனில் (res == CMDLINE_CHANGED)
		செல் cmdline_changed;
	    எனில் (c == K_WILD)
		செல் cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	எனில் (c == K_S_TAB && சாவி_Typed)
	{
	    எனில் (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		எனில் (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    முழு list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		எனில் (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    செல் cmdline_changed;
	    }
	}

	எனில் (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	எனில் (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_தேடல்_EXTRA
	    // Apply search highlighting
	    எனில் (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    எனில் (சாவி_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, மொத்தம், &is_state);
#endif
	    wild_type = 0;
	    செல் cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	தேர்வு (c)
	{
	நிலை K_BS:
	நிலை Ctrl_H:
	நிலை K_DEL:
	நிலை K_KDEL:
	நிலை Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_தேடல்_EXTRA
		    , &is_state
#endif
		    );
	    எனில் (res == CMDLINE_NOT_CHANGED)
		செல் cmdline_not_changed;
	    இல்லையெனில் எனில் (res == GOTO_NORMAL_MODE)
		செல் returncmd;		// back to cmd mode
	    செல் cmdline_changed;

	நிலை K_INS:
	நிலை K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		செல் cmdline_not_changed;

	நிலை Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : வெற்று);
		செல் cmdline_not_changed;

//	case '@':   only in very old vi
	நிலை Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		போது (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_தேடல்_EXTRA
		எனில் (ccline.cmdlen == 0)
		    is_state.தேடல்_start = is_state.save_cursor;
#endif
		redrawcmd();
		செல் cmdline_changed;

#ifdef FEAT_CLIPBOARD
	நிலை Ctrl_Y:
		// Copy the modeless selection, if there is one.
		எனில் (clip_star.state != SELECT_CLEARED)
		{
		    எனில் (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    செல் cmdline_not_changed;
		}
		நிறுத்து;
#endif

	நிலை ESC:	// get here if p_wc != ESC or when ESC typed twice
	நிலை Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		எனில் (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    செல் cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		செல் returncmd;		// back to cmd mode

	நிலை Ctrl_R:			// insert register
		res = cmdline_செருகு_reg(&gotesc);
		எனில் (res == GOTO_NORMAL_MODE)
		    செல் returncmd;
		எனில் (res == CMDLINE_CHANGED)
		    செல் cmdline_changed;
		செல் cmdline_not_changed;

	நிலை Ctrl_D:
		எனில் (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    நிறுத்து;	// Use ^D as normal char instead

		redrawcmd();
		தொடர்;	// don't do incremental search now

	நிலை K_வலது:
	நிலை K_S_வலது:
	நிலை K_C_வலது:
		செய்
		{
		    எனில் (ccline.cmdpos >= ccline.cmdlen)
			நிறுத்து;
		    i = cmdline_charsize(ccline.cmdpos);
		    எனில் (சாவி_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			நிறுத்து;
		    ccline.cmdspos += i;
		    எனில் (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    இல்லையெனில்
			++ccline.cmdpos;
		}
		போது ((c == K_S_வலது || c == K_C_வலது
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		எனில் (has_mbyte)
		    set_cmdspos_cursor();
		செல் cmdline_not_changed;

	நிலை K_இடது:
	நிலை K_S_இடது:
	நிலை K_C_இடது:
		எனில் (ccline.cmdpos == 0)
		    செல் cmdline_not_changed;
		செய்
		{
		    --ccline.cmdpos;
		    எனில் (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		போது (ccline.cmdpos > 0
			&& (c == K_S_இடது || c == K_C_இடது
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		எனில் (has_mbyte)
		    set_cmdspos_cursor();
		செல் cmdline_not_changed;

	நிலை K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		செல் cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	நிலை K_F4:
	    எனில் (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		செல் cmdline_not_changed;
	    }
	    நிறுத்து;
#endif

	நிலை K_MIDDLEDRAG:
	நிலை K_MIDDLERELEASE:
		செல் cmdline_not_changed;	// Ignore mouse

	நிலை K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		எனில் (!gui.in_use)
#endif
		    எனில் (!mouse_has(MOUSE_COMMAND))
			செல் cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		எனில் (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		இல்லையெனில்
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		செல் cmdline_changed;

#ifdef FEAT_DND
	நிலை K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		செல் cmdline_changed;
#endif

	நிலை K_LEFTDRAG:
	நிலை K_LEFTRELEASE:
	நிலை K_RIGHTDRAG:
	நிலை K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		எனில் (ignore_drag_release)
		    செல் cmdline_not_changed;
		// FALLTHROUGH
	நிலை K_LEFTMOUSE:
	நிலை K_RIGHTMOUSE:
		cmdline_இடது_வலது_mouse(c, &ignore_drag_release);
		செல் cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	நிலை K_MOUSEDOWN:
	நிலை K_MOUSEUP:
	நிலை K_MOUSELEFT:
	நிலை K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		எனில் (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_இடது
				      : MSCR_வலது);
#endif
		செல் cmdline_not_changed;

	// Alternate buttons ignored here
	நிலை K_X1MOUSE:
	நிலை K_X1DRAG:
	நிலை K_X1RELEASE:
	நிலை K_X2MOUSE:
	நிலை K_X2DRAG:
	நிலை K_X2RELEASE:
	நிலை K_MOUSEMOVE:
		செல் cmdline_not_changed;

#ifdef FEAT_GUI
	நிலை K_LEFTMOUSE_NM:	// mousefocus click, ignored
	நிலை K_LEFTRELEASE_NM:
		செல் cmdline_not_changed;

	நிலை K_VER_SCROLLBAR:
		எனில் (செய்தி_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		செல் cmdline_not_changed;

	நிலை K_HOR_SCROLLBAR:
		எனில் (செய்தி_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_மதிப்பு);
		    redrawcmd();
		}
		செல் cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	நிலை K_TABLINE:
	நிலை K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		எனில் (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		செல் cmdline_not_changed;
#endif

	நிலை K_SELECT:	    // end of Select mode mapping - ignore
		செல் cmdline_not_changed;

	நிலை Ctrl_B:	    // begin of command line
	நிலை K_HOME:
	நிலை K_KHOME:
	நிலை K_S_HOME:
	நிலை K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		செல் cmdline_not_changed;

	நிலை Ctrl_E:	    // end of command line
	நிலை K_END:
	நிலை K_KEND:
	நிலை K_S_END:
	நிலை K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		செல் cmdline_not_changed;

	நிலை Ctrl_A:	    // all matches
		எனில் (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		எனில் (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    நிறுத்து;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		செல் cmdline_changed;

	நிலை Ctrl_L:
#ifdef FEAT_தேடல்_EXTRA
		எனில் (may_add_char_to_தேடல்(firstc, &c, &is_state) == OK)
		    செல் cmdline_not_changed;
#endif

		// completion: longest common part
		எனில் (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    நிறுத்து;
		செல் cmdline_changed;

	நிலை Ctrl_N:	    // next match
	நிலை Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		எனில் (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    எனில் (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    செல் cmdline_not_changed;
		}
#endif
		எனில் (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    எனில் (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			நிறுத்து;
		    செல் cmdline_changed;
		}
		// FALLTHROUGH
	நிலை K_UP:
	நிலை K_DOWN:
	நிலை K_S_UP:
	நிலை K_S_DOWN:
	நிலை K_PAGEUP:
	நிலை K_KPAGEUP:
	நிலை K_PAGEDOWN:
	நிலை K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		எனில் (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    முழு	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    எனில் (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    செல் cmdline_not_changed;
		}
#endif
		எனில் (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    எனில் (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    எனில் (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			நிறுத்து;
		    செல் cmdline_changed;
		}
		இல்லையெனில்
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    எனில் (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			செல் cmdline_changed;
		    }
		    இல்லையெனில் எனில் (res == GOTO_NORMAL_MODE)
			செல் returncmd;
		}
		செல் cmdline_not_changed;

#ifdef FEAT_தேடல்_EXTRA
	நிலை Ctrl_G:	    // next match
	நிலை Ctrl_T:	    // previous match
		எனில் (may_adjust_incsearch_highlighting(
					  firstc, மொத்தம், &is_state, c) == FAIL)
		    செல் cmdline_not_changed;
		நிறுத்து;
#endif

	நிலை Ctrl_V:
	நிலை Ctrl_Q:
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
		    எனில் (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			செய்தி_putchar(' ');
			cursorcmd();
		    }
		}

		நிறுத்து;

#ifdef FEAT_DIGRAPHS
	நிலை Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		எனில் (c != NUL)
		    நிறுத்து;

		redrawcmd();
		செல் cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	நிலை Ctrl__:	    // CTRL-_: switch language mode
		எனில் (!p_ari)
		    நிறுத்து;
		cmd_hkmap = !cmd_hkmap;
		செல் cmdline_not_changed;
#endif

	நிலை K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, வெற்று);
		செல் cmdline_changed;

	இயல்பு:
#ifdef UNIX
		எனில் (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    செல் returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		எனில் (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		நிறுத்து;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	எனில் (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    செல் cmdline_changed;

	/*
	 * put the character in the command line
	 */
	எனில் (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_சாவி_name(c, mod_mask), -1, TRUE);
	இல்லையெனில்
	{
	    எனில் (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    இல்லையெனில்
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	செல் cmdline_changed;

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
	எனில் (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_தேடல்_EXTRA
	எனில் (!is_state.incsearch_postponed)
	    தொடர்;
#endif

cmdline_changed:
#ifdef FEAT_தேடல்_EXTRA
	// If the window changed incremental search state is not valid.
	எனில் (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	எனில் (xpc.xp_context == EXPAND_NOTHING && (சாவி_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, மொத்தம், &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	எனில் (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != வெற்று &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    எனில் (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	எனில் (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	எனில் (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    எனில் (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    எனில் (!event_cmdlineleavepre_triggered)
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
    எனில் (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = வெற்று;
    clear_cmdline_orig();

#ifdef FEAT_தேடல்_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    எனில் (ccline.cmdbuff != வெற்று)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	எனில் (ccline.cmdlen && firstc != NUL
		&& (some_சாவி_typed || histype == HIST_தேடல்))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_தேடல் ? firstc : NUL);
	    எனில் (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	எனில் (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    செய்தி_check();
    செய்தி_scroll = save_செய்தி_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    எனில் (some_சாவி_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    எனில் (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_உள்ளீடு_METHOD
    எனில் (b_im_ptr != வெற்று && buf_valid(b_im_ptr_buf)
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
	எனில் (did_save_ccline)
	    restore_cmdline(&save_ccline);
	இல்லையெனில்
	    ccline.cmdbuff = வெற்று;

	vim_free(prev_cmdbuff);
	திரும்பு p;
    }
}

#எனில் defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    முழு		firstc,
    char_u	*prompt,	// command line prompt
    முழு		attr,		// attributes for prompt
    முழு		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    முழு			did_save_ccline = FALSE;
    முழு			செய்தி_col_save = செய்தி_col;
    முழு			செய்தி_silent_save = செய்தி_silent;

    எனில் (ccline.cmdbuff != வெற்று)
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
    ccline.உள்ளீடு_fn = (firstc == '@');
# endif
    செய்தி_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    எனில் (did_save_ccline)
	restore_cmdline(&save_ccline);

    செய்தி_silent = செய்தி_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    எனில் (ccline.cmdbuff != வெற்று)
	செய்தி_col = செய்தி_col_save;

    திரும்பு s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    முழு
check_opt_wim(வெறுமை)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    முழு		i;
    முழு		idx = 0;

    க்கு (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    க்கு (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	க்கு (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	எனில் (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    திரும்பு FAIL;
	எனில் (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	இல்லையெனில் எனில் (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	இல்லையெனில் எனில் (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	இல்லையெனில் எனில் (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	இல்லையெனில் எனில் (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	இல்லையெனில் எனில் (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	இல்லையெனில்
	    திரும்பு FAIL;
	p += i;
	எனில் (*p == NUL)
	    நிறுத்து;
	எனில் (*p == ',')
	{
	    எனில் (idx == 3)
		திரும்பு FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    போது (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    க்கு (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    திரும்பு OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    முழு
text_locked(வெறுமை)
{
    எனில் (cmdwin_type != 0)
	திரும்பு TRUE;
    திரும்பு textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    வெறுமை
text_locked_செய்தி(வெறுமை)
{
    emsg(_(get_text_locked_செய்தி()));
}

    எழுத்து *
get_text_locked_செய்தி(வெறுமை)
{
    எனில் (cmdwin_type != 0)
	திரும்பு e_invalid_in_cmdline_window;
    திரும்பு e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    முழு
text_or_buf_locked(வெறுமை)
{
    எனில் (text_locked())
    {
	text_locked_செய்தி();
	திரும்பு TRUE;
    }
    திரும்பு curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    முழு
curbuf_locked(வெறுமை)
{
    எனில் (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	திரும்பு TRUE;
    }
    திரும்பு allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    முழு
allbuf_locked(வெறுமை)
{
    எனில் (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    நிலையான முழு
cmdline_charsize(முழு idx)
{
#எனில் defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    எனில் (cmdline_star > 0)	    // showing '*', always 1 position
	திரும்பு 1;
#endif
    திரும்பு ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    நிலையான வெறுமை
set_cmdspos(வெறுமை)
{
    எனில் (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    இல்லையெனில்
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    நிலையான வெறுமை
set_cmdspos_cursor(வெறுமை)
{
    முழு		i, m, c;

    set_cmdspos();
    எனில் (சாவி_Typed)
    {
	m = cmdline_width * Rows;
	எனில் (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    இல்லையெனில்
	m = MAXCOL;
    க்கு (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	எனில் (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	எனில் ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    நிறுத்து;
	}
	எனில் (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    நிலையான வெறுமை
correct_cmdspos(முழு idx, முழு cells)
{
    எனில் ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    முழு		c,		// normally ':', NUL for ":append"
    வெறுமை	*cookie UNUSED,
    முழு		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    எனில் (exec_from_reg && vpeekc() == ':')
	(வெறுமை)vgetc();
    திரும்பு getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    முழு		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    வெறுமை	*cookie UNUSED,
    முழு		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	வரி_ga;
    char_u	*pend;
    முழு		startcol = 0;
    முழு		c1 = 0;
    முழு		escaped = FALSE;	// CTRL-V typed
    முழு		vcol = 0;
    char_u	*p;
    முழு		prev_char;
    முழு		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    எனில் ((செய்தி_col || செய்தி_didout) && promptc != '?')
	செய்தி_putchar('\n');
    எனில் (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	எனில் (p_prompt)
	    செய்தி_putchar(':');
	போது (indent-- > 0)
	    செய்தி_putchar(' ');
	startcol = செய்தி_col;
    }

    ga_init2(&வரி_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    எனில் (promptc <= 0)
    {
	vcol = indent;
	போது (indent >= 8)
	{
	    ga_append(&வரி_ga, TAB);
	    செய்தி_puts("        ");
	    indent -= 8;
	}
	போது (indent-- > 0)
	{
	    ga_append(&வரி_ga, ' ');
	    செய்தி_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_சாவிகள்;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    போது (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	எனில் (ga_grow(&வரி_ga, 40) == FAIL)
	    நிறுத்து;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	எனில் (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	இல்லையெனில்
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	எனில் (got_int)
	{
	    செய்தி_putchar('\n');
	    நிறுத்து;
	}

	எனில் (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &வரி_ga);
	    செல் redraw;
	}

	எனில் (!escaped)
	{
	    // CR typed means "enter", which is NL
	    எனில் (c1 == '\r')
		c1 = '\n';

	    எனில் (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		எனில் (வரி_ga.ga_len > 0)
		{
		    எனில் (has_mbyte)
		    {
			p = (char_u *)வரி_ga.ga_data;
			p[வரி_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + வரி_ga.ga_len - 1) + 1;
			வரி_ga.ga_len -= len;
		    }
		    இல்லையெனில்
			--வரி_ga.ga_len;
		    செல் redraw;
		}
		தொடர்;
	    }

	    எனில் (c1 == Ctrl_U)
	    {
		செய்தி_col = startcol;
		செய்தி_clr_eos();
		வரி_ga.ga_len = 0;
		செல் redraw;
	    }

	    எனில் (c1 == Ctrl_T)
	    {
		sw = get_sw_மதிப்பு(curbuf);
		p = (char_u *)வரி_ga.ga_data;
		p[வரி_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		போது (get_indent_str(p, 8, FALSE) < indent)
		{
		    (வெறுமை)ga_grow(&வரி_ga, 2);  // one more for the NUL
		    p = (char_u *)வரி_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, வரி_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++வரி_ga.ga_len;
		}
redraw:
		// redraw the line
		செய்தி_col = startcol;
		vcol = 0;
		p = (char_u *)வரி_ga.ga_data;
		p[வரி_ga.ga_len] = NUL;
		போது (p < (char_u *)வரி_ga.ga_data + வரி_ga.ga_len)
		{
		    எனில் (*p == TAB)
		    {
			செய்
			    செய்தி_putchar(' ');
			போது (++vcol % 8);
			++p;
		    }
		    இல்லையெனில்
		    {
			len = mb_ptr2len(p);
			செய்தி_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		செய்தி_clr_eos();
		windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
		தொடர்;
	    }

	    எனில் (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)வரி_ga.ga_data;
		எனில் (prev_char == '0' || prev_char == '^')
		{
		    எனில் (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--வரி_ga.ga_len] = NUL;
		}
		இல்லையெனில்
		{
		    p[வரி_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    எனில் (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_மதிப்பு(curbuf);
		    }
		}
		போது (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, வரி_ga.ga_len - (s - p) + 1);
		    --வரி_ga.ga_len;
		}
		செல் add_indent;
	    }

	    எனில் (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		தொடர்;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    எனில் (IS_SPECIAL(c1))
		தொடர்;
	}

	எனில் (IS_SPECIAL(c1))
	    c1 = '?';
	எனில் (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)வரி_ga.ga_data + வரி_ga.ga_len);
	இல்லையெனில்
	{
	    len = 1;
	    ((char_u *)வரி_ga.ga_data)[வரி_ga.ga_len] = c1;
	}
	எனில் (c1 == '\n')
	    செய்தி_putchar('\n');
	இல்லையெனில் எனில் (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    செய்
		செய்தி_putchar(' ');
	    போது (++vcol % 8);
	}
	இல்லையெனில்
	{
	    செய்தி_outtrans_len(
		     ((char_u *)வரி_ga.ga_data) + வரி_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	வரி_ga.ga_len += len;
	escaped = FALSE;

	windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
	pend = (char_u *)(வரி_ga.ga_data) + வரி_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	எனில் (வரி_ga.ga_len > 0 && pend[-1] == '\n'
		&& (வரி_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --வரி_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    நிறுத்து;
	}
    }

    --no_mapping;
    --allow_சாவிகள்;

    // make following messages go to the next line
    செய்தி_didout = FALSE;
    செய்தி_col = 0;
    எனில் (செய்தி_row < Rows - 1)
	++செய்தி_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    எனில் (got_int)
	ga_clear(&வரி_ga);

    திரும்பு (char_u *)வரி_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    முழு
cmdline_overstrike(வெறுமை)
{
    திரும்பு ccline.overstrike;
}

#எனில் defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    முழு
cmdline_at_end(வெறுமை)
{
    திரும்பு (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#எனில் defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(வெறுமை)
{
    எனில் (ccline.cmdbuff == வெற்று || ccline.cmdpos > ccline.cmdlen)
	திரும்பு MAXCOL;

    எனில் (has_mbyte)
    {
	colnr_T	col;
	முழு	i = 0;

	க்கு (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	திரும்பு col;
    }
    இல்லையெனில்
	திரும்பு ccline.cmdpos;
}
#endif

#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    நிலையான வெறுமை
redrawcmd_preedit(வெறுமை)
{
    எனில் ((State & MODE_CMDLINE)
	    && xic != வெற்று
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	முழு	cmdpos = 0;
	முழு	cmdspos;
	முழு	old_row;
	முழு	old_col;
	colnr_T	col;

	old_row = செய்தி_row;
	old_col = செய்தி_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	எனில் (has_mbyte)
	{
	    க்கு (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	இல்லையெனில்
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	செய்தி_row = cmdline_row + (cmdspos / cmdline_width);
	செய்தி_col = cmdspos % cmdline_width;
	எனில் (செய்தி_row >= Rows)
	    செய்தி_row = Rows - 1;

	க்கு (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    முழு char_len;
	    முழு char_attr;

	    char_attr = im_get_feedback_attr(col);
	    எனில் (char_attr < 0)
		நிறுத்து; // end of preedit string

	    எனில் (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    இல்லையெனில்
		char_len = 1;

	    செய்தி_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	செய்தி_row = old_row;
	செய்தி_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    நிலையான வெறுமை
dealloc_cmdbuff(வெறுமை)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    நிலையான வெறுமை
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    எனில் (len < 80)
	len = 100;
    இல்லையெனில்
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (முழு)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    முழு
realloc_cmdbuff(முழு len)
{
    char_u	*p;
    முழு		plen;

    எனில் (len < ccline.cmdbufflen)
	திரும்பு OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    எனில் (ccline.cmdbuff == வெற்று)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	திரும்பு FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    எனில் (ccline.xpc != வெற்று
	    && ccline.xpc->xp_pattern != வெற்று
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	முழு i = (முழு)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	எனில் (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    திரும்பு OK;
}

#எனில் defined(FEAT_ARABIC)
நிலையான char_u	*arshape_buf = வெற்று;

# எனில் defined(EXITFREE)
    வெறுமை
free_arshape_buf(வெறுமை)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    நிலையான வெறுமை
draw_cmdline(முழு start, முழு len)
{
#எனில் defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    முழு		i;

    எனில் (cmdline_star > 0)
	க்கு (i = 0; i < len; ++i)
	{
	    செய்தி_putchar('*');
	    எனில் (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    இல்லையெனில்
#endif
#ifdef FEAT_ARABIC
	எனில் (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	நிலையான முழு	buflen = 0;
	char_u		*p;
	முழு		j;
	முழு		newlen = 0;
	முழு		mb_l;
	முழு		pc, pc1 = 0;
	முழு		prev_c = 0;
	முழு		prev_c1 = 0;
	முழு		u8c;
	முழு		u8cc[MAX_MCO];
	முழு		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	எனில் (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    எனில் (arshape_buf == வெற்று)
		திரும்பு;	// out of memory
	}

	எனில் (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	க்கு (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    எனில் (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		எனில் (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    எனில் (j + mb_l >= start + len)
			nc = NUL;
		    இல்லையெனில்
			nc = utf_ptr2char(p + mb_l);
		}
		இல்லையெனில்
		{
		    // displaying from left to right
		    எனில் (j + mb_l >= start + len)
			pc = NUL;
		    இல்லையெனில்
		    {
			முழு	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, வெற்று, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		எனில் (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    எனில் (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    இல்லையெனில்
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	செய்தி_outtrans_len(arshape_buf, newlen);
    }
    இல்லையெனில்
#endif
	செய்தி_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    வெறுமை
putcmdline(முழு c, முழு shift)
{
    எனில் (cmd_silent)
	திரும்பு;
    செய்தி_no_more = TRUE;
    செய்தி_putchar(c);
    எனில் (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    செய்தி_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    வெறுமை
unputcmdline(வெறுமை)
{
    எனில் (cmd_silent)
	திரும்பு;
    செய்தி_no_more = TRUE;
    எனில் (ccline.cmdlen == ccline.cmdpos)
	செய்தி_putchar(' ');
    இல்லையெனில் எனில் (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    இல்லையெனில்
	draw_cmdline(ccline.cmdpos, 1);
    செய்தி_no_more = FALSE;
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
    முழு
put_on_cmdline(char_u *str, முழு len, முழு redraw)
{
    முழு		retval;
    முழு		i;
    முழு		m;
    முழு		c;

    எனில் (len < 0)
	len = (முழு)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    எனில் (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    இல்லையெனில்
	retval = OK;
    எனில் (retval == OK)
    {
	எனில் (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	இல்லையெனில்
	{
	    எனில் (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		க்கு (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		க்கு (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		எனில் (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		இல்லையெனில்
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    இல்லையெனில் எனில் (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	எனில் (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    போது (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    எனில் (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		எனில் (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		இல்லையெனில்
		    i = 0;
	    }
#endif
	    எனில் (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		செய்தி_col -= i;
		எனில் (செய்தி_col < 0)
		{
		    செய்தி_col += cmdline_width;
		    --செய்தி_row;
		}
	    }
	}

	எனில் (redraw && !cmd_silent)
	{
	    செய்தி_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    எனில் (cmdline_row != i || ccline.overstrike)
		செய்தி_clr_eos();
	    செய்தி_no_more = FALSE;
	}
	எனில் (சாவி_Typed)
	{
	    m = cmdline_width * Rows;
	    எனில் (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	இல்லையெனில்
	    m = MAXCOL;
	க்கு (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    எனில் (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    எனில் (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    எனில் (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		எனில் (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    எனில் (redraw)
	செய்தி_check();
    திரும்பு retval;
}

நிலையான cmdline_info_T	prev_ccline;
நிலையான முழு		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    நிலையான வெறுமை
save_cmdline(cmdline_info_T *ccp)
{
    எனில் (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = வெற்று;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    நிலையான வெறுமை
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
    நிலையான முழு
cmdline_paste(
    முழு regname,
    முழு literally,	// Insert text literally instead of "as typed"
    முழு remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    முழு			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    எனில் (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	திரும்பு FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    வரி_breakcheck();
    எனில் (got_int)
	திரும்பு FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    எனில் (i)
    {
	// Got the value of a special register in "arg".
	எனில் (arg == வெற்று)
	    திரும்பு FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	எனில் (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    முழு	    len;

	    // Locate start of last word in the cmd buffer.
	    க்கு (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		எனில் (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    எனில் (!vim_iswordc(mb_ptr2char(w - len)))
			நிறுத்து;
		    w -= len;
		}
		இல்லையெனில்
		{
		    எனில் (!vim_iswordc(w[-1]))
			நிறுத்து;
		    --w;
		}
	    }
	    len = (முழு)((ccline.cmdbuff + ccline.cmdpos) - w);
	    எனில் (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	எனில் (allocated)
	    vim_free(arg);
	திரும்பு OK;
    }

    திரும்பு cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    வெறுமை
cmdline_paste_str(char_u *s, முழு literally)
{
    முழு		c, cv;

    எனில் (literally)
	put_on_cmdline(s, -1, TRUE);
    இல்லையெனில்
	போது (*s != NUL)
	{
	    cv = *s;
	    எனில் (cv == Ctrl_V && s[1])
		++s;
	    எனில் (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    இல்லையெனில்
		c = *s++;
	    எனில் (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    வெறுமை
redrawcmdline(வெறுமை)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    வெறுமை
redrawcmdline_ex(முழு do_compute_cmdrow)
{
    எனில் (cmd_silent)
	திரும்பு;
    need_wait_return = FALSE;
    எனில் (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    நிலையான வெறுமை
redrawcmdprompt(வெறுமை)
{
    முழு		i;

    எனில் (cmd_silent)
	திரும்பு;
    எனில் (ccline.cmdfirstc != NUL)
	செய்தி_putchar(ccline.cmdfirstc);
    எனில் (ccline.cmdprompt != வெற்று)
    {
	செய்தி_puts_attr((எழுத்து *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = செய்தி_col + (செய்தி_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	எனில் (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    இல்லையெனில்
	க்கு (i = ccline.cmdindent; i > 0; --i)
	    செய்தி_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    வெறுமை
redrawcmd(வெறுமை)
{
    முழு save_in_echowindow = in_echowindow;

    எனில் (cmd_silent)
	திரும்பு;

    // when 'incsearch' is set there may be no command line while redrawing
    எனில் (ccline.cmdbuff == வெற்று)
    {
	windgoto(cmdline_row, cmdline_col_off);
	செய்தி_clr_eos();
	திரும்பு;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    செய்தி_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    செய்தி_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    செய்தி_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    செய்தி_clr_eos();
    செய்தி_no_more = FALSE;

    set_cmdspos_cursor();
    எனில் (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    செய்தி_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    வெறுமை
compute_cmdrow(வெறுமை)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    எனில் (exmode_active || (செய்தி_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    இல்லையெனில்
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    வெறுமை
cursorcmd(வெறுமை)
{
    எனில் (cmd_silent)
	திரும்பு;

#ifdef FEAT_RIGHTLEFT
    எனில் (cmdmsg_rl)
    {
	செய்தி_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	செய்தி_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	எனில் (செய்தி_row <= 0)
	    செய்தி_row = Rows - 1;
    }
    இல்லையெனில்
#endif
    {
	செய்தி_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	செய்தி_col = ccline.cmdspos % cmdline_width;
	எனில் (செய்தி_row >= Rows)
	    செய்தி_row = Rows - 1;
    }

    windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    எனில் (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    வெறுமை
gotocmdline(முழு clr)
{
    செய்தி_start();
#ifdef FEAT_RIGHTLEFT
    எனில் (cmdmsg_rl)
	செய்தி_col = cmdline_width - 1;
    இல்லையெனில்
#endif
	செய்தி_col = 0;	    // always start in column 0
    எனில் (clr)		    // clear the bottom line(s)
	செய்தி_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    நிலையான முழு
ccheck_abbr(முழு c)
{
    முழு spos = 0;

    எனில் (p_paste || no_abbr)	    // no abbreviations or in paste mode
	திரும்பு FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    போது (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    எனில் (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    இல்லையெனில்
	// check abbreviation from the beginning of the commandline
	spos = 0;

    திரும்பு check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, முழு what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    முழு		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    க்கு (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	எனில் ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#இல்லையெனில்
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    எனில் (what == VSE_SHELL && csh_like_shell() && p != வெற்று)
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
    எனில் (p != வெற்று && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    திரும்பு p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    வெறுமை
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    எனில் (p == வெற்று)
	திரும்பு;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    வெறுமை
tilde_replace(
    char_u  *orig_pat,
    முழு	    இலக்கம்_files,
    char_u  **files)
{
    முழு	    i;
    char_u  *p;

    எனில் (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	க்கு (i = 0; i < இலக்கம்_files; ++i)
	{
	    p = home_replace_save(வெற்று, files[i]);
	    எனில் (p != வெற்று)
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
get_cmdline_info(வெறுமை)
{
    திரும்பு &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    நிலையான cmdline_info_T *
get_ccline_ptr(வெறுமை)
{
    எனில் ((State & MODE_CMDLINE) == 0)
	திரும்பு வெற்று;
    எனில் (ccline.cmdbuff != வெற்று)
	திரும்பு &ccline;
    எனில் (prev_ccline_used && prev_ccline.cmdbuff != வெற்று)
	திரும்பு &prev_ccline;
    திரும்பு வெற்று;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    நிலையான முழு
get_cmdline_type(வெறுமை)
{
    cmdline_info_T *p = get_ccline_ptr();

    எனில் (p == வெற்று)
	திரும்பு NUL;
    எனில் (p->cmdfirstc == NUL)
	திரும்பு
#ifdef FEAT_EVAL
	    (p->உள்ளீடு_fn) ? '@' :
#endif
	    '-';
    திரும்பு p->cmdfirstc;
}

#எனில் defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    நிலையான char_u *
get_cmdline_str(வெறுமை)
{
    cmdline_info_T *p;

    எனில் (cmdline_star > 0)
	திரும்பு வெற்று;
    p = get_ccline_ptr();
    எனில் (p == வெற்று)
	திரும்பு வெற்று;
    திரும்பு vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    நிலையான char_u *
get_cmdline_completion_pattern(வெறுமை)
{
    cmdline_info_T *p;
    முழு		xp_context;

    எனில் (cmdline_star > 0)
	திரும்பு வெற்று;

    p = get_ccline_ptr();
    எனில் (p == வெற்று || p->xpc == வெற்று)
	திரும்பு வெற்று;

    xp_context = p->xpc->xp_context;
    எனில் (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    எனில் (xp_context == EXPAND_UNSUCCESSFUL)
	திரும்பு வெற்று;

    char_u *compl_pat = p->xpc->xp_pattern;

    எனில் (compl_pat == வெற்று)
	திரும்பு வெற்று;

    திரும்பு vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    நிலையான char_u *
get_cmdline_completion(வெறுமை)
{
    cmdline_info_T	*p;
    முழு			xp_context;

    எனில் (cmdline_star > 0)
	திரும்பு வெற்று;

    p = get_ccline_ptr();
    எனில் (p == வெற்று || p->xpc == வெற்று)
	திரும்பு வெற்று;

    xp_context = p->xpc->xp_context;
    எனில் (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    எனில் (xp_context == EXPAND_UNSUCCESSFUL)
	திரும்பு வெற்று;

    திரும்பு cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    வெறுமை
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    வெறுமை
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    வெறுமை
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    வெறுமை
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_எண் = p != வெற்று ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    வெறுமை
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != வெற்று && p->cmdprompt != வெற்று ?
					    vim_strsave(p->cmdprompt) : வெற்று;
}

/*
 * "getcmdscreenpos()" function
 */
    வெறுமை
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_எண் = p != வெற்று ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    வெறுமை
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    எனில் (rettv->vval.v_string == வெற்று)
	திரும்பு;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    நிலையான முழு
set_cmdline_str(char_u *str, முழு pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    முழு		    len;

    எனில் (p == வெற்று)
	திரும்பு 1;

    len = (முழு)STRLEN(str);
    எனில் (realloc_cmdbuff(len + 1) != OK)
	திரும்பு 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    திரும்பு 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    நிலையான முழு
set_cmdline_pos(
    முழு		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    எனில் (p == வெற்று)
	திரும்பு 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    எனில் (pos < 0)
	new_cmdpos = 0;
    இல்லையெனில்
	new_cmdpos = pos;

    திரும்பு 0;
}

// "setcmdline()" function
    வெறுமை
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    முழு pos = -1;

    எனில் (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_எண்_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	முழு error = FALSE;

	pos = (முழு)tv_get_எண்_chk(&argvars[1], &error) - 1;
	எனில் (error)
	    திரும்பு;
	எனில் (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    திரும்பு;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_எண் = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    வெறுமை
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    முழு		pos;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    pos = (முழு)tv_get_எண்(&argvars[0]) - 1;
    எனில் (pos >= 0)
	rettv->vval.v_எண் = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    முழு
get_cmdline_firstc(வெறுமை)
{
    திரும்பு ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    முழு
get_list_range(char_u **str, முழு *num1, முழு *num2)
{
    முழு		len;
    முழு		first = FALSE;
    varnumber_T	இலக்கம்;

    *str = skipwhite(*str);
    எனில் (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, வெற்று, &len, 0, &இலக்கம், வெற்று, 0, FALSE, வெற்று);
	*str += len;
	// overflow
	எனில் (இலக்கம் > INT_MAX)
	    திரும்பு FAIL;

	*num1 = (முழு)இலக்கம்;
	first = TRUE;
    }
    *str = skipwhite(*str);
    எனில் (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, வெற்று, &len, 0, &இலக்கம், வெற்று, 0, FALSE, வெற்று);
	எனில் (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    எனில் (இலக்கம் > INT_MAX)
		திரும்பு FAIL;

	    *num2 = (முழு)இலக்கம்;
	}
	இல்லையெனில் எனில் (!first)		// no number given at all
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (first)			// only one number given
	*num2 = *num1;
    திரும்பு OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    எழுத்து *
did_set_cedit(optset_T *args UNUSED)
{
    முழு n;

    எனில் (*p_cedit == NUL)
	cedit_சாவி = -1;
    இல்லையெனில்
    {
	n = string_to_சாவி(p_cedit, FALSE);
	எனில் (n == 0 || vim_isprintc(n))
	    திரும்பு e_invalid_argument;
	cedit_சாவி = n;
    }
    திரும்பு வெற்று;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    நிலையான முழு
open_cmdwin(வெறுமை)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    முழு			i;
    linenr_T		lnum;
    முழு			histtype;
    garray_T		winsizes;
    முழு			save_restart_edit = restart_edit;
    முழு			save_State = State;
    முழு			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    முழு			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    முழு			save_சாவி_Typed;
#endif
    முழு			newbuf_status;
    முழு			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    எனில் (text_or_buf_locked()
	    || cmdwin_type != 0
#எனில் defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	திரும்பு K_IGNORE;
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
    எனில் (win_split((முழு)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	திரும்பு K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    எனில் (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	திரும்பு Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, வெற்று, வெற்று, வெற்று, ECMD_ONE, ECMD_HIDE,
					வெற்று);
    cmdwin_valid = win_valid(cmdwin_win);
    எனில் (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	எனில் (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	எனில் (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	எனில் (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(வெற்று, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = வெற்று;
	beep_flush();
	ga_clear(&winsizes);
	திரும்பு Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_மதிப்பு_give_err((char_u *)"bt",
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
    எனில் (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	எனில் (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_செருகு, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_செருகு, TRUE);
	}
	set_option_மதிப்பு_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    எனில் (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	எனில் (i >= 0)
	{
	    lnum = 0;
	    செய்
	    {
		எனில் (++i == get_hislen())
		    i = 0;
		எனில் (get_histentry(histtype)[i].hisstr != வெற்று)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    போது (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_வரி_மொத்தம், ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_வரி_abv_curs();
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
    எனில் (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_சாவி_Typed = சாவி_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    சாவி_Typed = save_சாவி_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = வெற்று;
    cmdwin_win = வெற்று;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    எனில் (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    இல்லையெனில்
    {
#எனில் defined(FEAT_EVAL)
	// autocmds may abort script processing
	எனில் (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	எனில் (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    எனில் (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    எனில் (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (முழு)plen;
		ccline.cmdbufflen = (முழு)(plen + 1);
		cmdwin_result = CAR;
	    }
	    இல்லையெனில்
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	இல்லையெனில் எனில் (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = வெற்று;
	}
	இல்லையெனில்
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	எனில் (ccline.cmdbuff == வெற்று)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	இல்லையெனில்
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    எனில் (ccline.cmdpos == ccline.cmdlen - 1
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
	எனில் (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	எனில் (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(வெற்று, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	எனில் (cmdwin_result == K_IGNORE)
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

    திரும்பு cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    முழு
is_in_cmdwin(வெறுமை)
{
    திரும்பு cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    எனில் (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == வெற்று)
	திரும்பு வெற்று;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    திரும்பு (char_u *)ga.ga_data;
#இல்லையெனில்
    திரும்பு வெற்று;
#endif
}

#எனில் defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    வெறுமை
get_user_உள்ளீடு(
    typval_T	*argvars,
    typval_T	*rettv,
    முழு		inputdialog,
    முழு		secret)
{
    char_u	*prompt;
    char_u	*p = வெற்று;
    முழு		c;
    char_u	buf[NUMBUFLEN];
    முழு		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    முழு		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = வெற்று;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
    எனில் (உள்ளீடு_busy)
	திரும்பு;  // this doesn't work recursively.

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	திரும்பு;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_உள்ளீடு
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    எனில் (no_console_உள்ளீடு() && !is_not_a_term())
	திரும்பு;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    எனில் (prompt != வெற்று)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	எனில் (p == வெற்று)
	    p = prompt;
	இல்லையெனில்
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    செய்தி_start();
	    செய்தி_clr_eos();
	    செய்தி_puts_attr((எழுத்து *)prompt, get_echo_attr());
	    செய்தி_didout = FALSE;
	    செய்தி_starthere();
	    *p = c;
	}
	cmdline_row = செய்தி_row;

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    எனில் (defstr != வெற்று)
		stuffReadbuffSpec(defstr);

	    எனில் (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		முழு	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = வெற்று;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		எனில் (xp_name == வெற்று)
		    திரும்பு;

		xp_namelen = (முழு)STRLEN(xp_name);

		எனில் (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    திரும்பு;
	    }
	}

	எனில் (defstr != வெற்று)
	{
	    முழு save_ex_normal_busy = ex_normal_busy;
	    முழு save_vgetc_busy = vgetc_busy;
	    முழு save_உள்ளீடு_busy = உள்ளீடு_busy;

	    உள்ளீடு_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    உள்ளீடு_busy = save_உள்ளீடு_busy;
	}
	எனில் (inputdialog && rettv->vval.v_string == வெற்று
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	செய்தி_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    வெறுமை
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    எனில் (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	திரும்பு;

    முழு cmd_type = get_cmdline_type();

    எனில் (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	சாவி_string[4];

	சாவி_string[0] = K_SPECIAL;
	சாவி_string[1] = KS_EXTRA;
	சாவி_string[2] = KE_WILD;
	சாவி_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(சாவி_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
