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

#अन्तर्भाव "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_किल्ली	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
स्थिर cmdline_info_T ccline;

#ifdef FEAT_EVAL
स्थिर पूर्णांक	new_cmdpos;	// position set by set_cmdline_pos()
#endif

स्थिर पूर्णांक	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
स्थिर पूर्णांक	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
स्थिर पूर्णांक	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

स्थिर char_u	*getcmdline_int(पूर्णांक firstc, दीर्घ मोजणी, पूर्णांक indent, पूर्णांक clear_ccline);
स्थिर पूर्णांक	cmdline_charsize(पूर्णांक idx);
स्थिर शून्य	set_cmdspos(शून्य);
स्थिर शून्य	set_cmdspos_cursor(शून्य);
स्थिर शून्य	correct_cmdspos(पूर्णांक idx, पूर्णांक cells);
स्थिर शून्य	dealloc_cmdbuff(शून्य);
स्थिर शून्य	alloc_cmdbuff(माप_प्रकार len);
स्थिर शून्य	draw_cmdline(पूर्णांक start, पूर्णांक len);
स्थिर शून्य	save_cmdline(cmdline_info_T *ccp);
स्थिर शून्य	restore_cmdline(cmdline_info_T *ccp);
स्थिर पूर्णांक	cmdline_paste(पूर्णांक regname, पूर्णांक literally, पूर्णांक remcr);
स्थिर शून्य	redrawcmdprompt(शून्य);
स्थिर पूर्णांक	ccheck_abbr(पूर्णांक);
स्थिर पूर्णांक	open_cmdwin(शून्य);
#ifdef FEAT_शोध_EXTRA
स्थिर पूर्णांक	empty_pattern_magic(char_u *pat, माप_प्रकार len, magic_T magic_val);
#endif

स्थिर पूर्णांक	cedit_किल्ली = -1;	// key value of 'cedit' option

    स्थिर शून्य
trigger_cmd_autocmd(पूर्णांक typechar, पूर्णांक evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    स्थिर शून्य
abandon_cmdline(शून्य)
{
    dealloc_cmdbuff();
    जर (संदेश_scrolled == 0)
	compute_cmdrow();
    संदेश("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_शोध_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    स्थिर पूर्णांक
empty_pattern(char_u *p, माप_प्रकार len, पूर्णांक delim)
{
    magic_T	magic_val = MAGIC_ON;

    जर (len > 0)
	(शून्य) skip_regexp_ex(p, delim, magic_isset(), रिक्त, रिक्त, &magic_val);
    अन्यथा
	परत TRUE;

    परत empty_pattern_magic(p, len, magic_val);
}

    स्थिर पूर्णांक
empty_pattern_magic(char_u *p, माप_प्रकार len, magic_T magic_val)
{
    // remove trailing \v and the like
    पर्यंत (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != रिक्त)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    परत len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
प्रकारव्याख्या संरचना {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    पूर्णांक		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    स्थिर शून्य
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

    स्थिर शून्य
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
प्रकारव्याख्या संरचना {
    pos_T	शोध_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    पूर्णांक		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    पूर्णांक		did_incsearch;
    पूर्णांक		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    स्थिर शून्य
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->शोध_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    स्थिर शून्य
set_शोध_match(pos_T *t)
{
    t->lnum += शोध_match_ओळी;
    t->col = शोध_match_endcol;
    जर (t->lnum > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	t->lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    पूर्णांक
parse_pattern_and_range(
	pos_T	*incsearch_start,
	पूर्णांक	*शोध_delim,
	पूर्णांक	*skiplen,
	पूर्णांक	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    पूर्णांक		delim_optional = FALSE;
    पूर्णांक		delim;
    पूर्णांक		use_last_pat;
    magic_T     magic = 0;
    अक्षर	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    शोध_first_ओळ = 0;
    शोध_last_ओळ = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_ओळी;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, रिक्त);

    जर (vim_strchr((char_u *)"sgvlu", *cmd) == रिक्त)
	परत FALSE;

    // Skip over command name to find pattern separator
    साठी (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    जर (*skipwhite(p) == NUL)
	परत FALSE;

    जर (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	जर (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	अन्यथा जर (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    अन्यथा जर (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	जर (*p == '!')
	    p = skipwhite(p + 1);
	पर्यंत (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	जर (*p == NUL)
	    परत FALSE;
    }
    अन्यथा जर (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	जर (*p == '!')
	{
	    p++;
	    जर (*skipwhite(p) == NUL)
		परत FALSE;
	}
	जर (*cmd != 'g')
	    delim_optional = TRUE;
    }
    अन्यथा
	परत FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *शोध_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), रिक्त, रिक्त, &magic);
    use_last_pat = end == p && *end == delim;

    जर (end == p && !use_last_pat)
	परत FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    जर (!use_last_pat)
    {
	अक्षर c = *end;
	पूर्णांक empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (माप_प्रकार)(end - p), magic);
	*end = c;
	जर (empty)
	    परत FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (पूर्णांक)(p - ccline.cmdbuff);
    *patlen = (पूर्णांक)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    जर (ea.addr_मोजणी > 0)
    {
	पूर्णांक reverse_match = ea.line2 < ea.line1;
	शोध_first_ओळ = reverse_match ? ea.line2 : ea.line1;
	शोध_last_ओळ = reverse_match ? ea.line1 : ea.line2;
    }
    अन्यथा जर (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	शोध_first_ओळ = शोध_last_ओळ = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    परत TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    स्थिर पूर्णांक
do_incsearch_highlighting(
	पूर्णांक		    firstc,
	पूर्णांक		    *शोध_delim,
	incsearch_state_T   *is_state,
	पूर्णांक		    *skiplen,
	पूर्णांक		    *patlen)
{
    पूर्णांक retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    जर (!p_is || cmd_silent)
	परत FALSE;

    // By default search all lines
    शोध_first_ओळ = 0;
    शोध_last_ओळ = MAXLNUM;

    जर (firstc == '/' || firstc == '?')
    {
	*शोध_delim = firstc;
	परत TRUE;
    }

    जर (firstc != ':')
	परत FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->शोध_start, शोध_delim,
	    skiplen, patlen);
    --emsg_off;

    परत retval;
}

    स्थिर शून्य
finish_incsearch_highlighting(
	पूर्णांक gotesc,
	incsearch_state_T *is_state,
	पूर्णांक call_update_screen)
{
    जर (!is_state->did_incsearch)
	परत;

    is_state->did_incsearch = FALSE;
    जर (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    अन्यथा
    {
	जर (!EQUAL_POS(is_state->save_cursor, is_state->शोध_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->शोध_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    शोध_first_ओळ = 0;
    शोध_last_ओळ = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    जर (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    स्थिर शून्य
may_do_incsearch_highlighting(
	पूर्णांक		    firstc,
	दीर्घ		    मोजणी,
	incsearch_state_T   *is_state)
{
    पूर्णांक		skiplen, patlen;
    पूर्णांक		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    पूर्णांक		next_char;
    पूर्णांक		use_last_pat;
    पूर्णांक		did_do_incsearch = is_state->did_incsearch;
    पूर्णांक		शोध_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_शोध_pattern();

    जर (!do_incsearch_highlighting(firstc, &शोध_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_शोध_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	जर (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	परत;
    }

    // If there is a character waiting, search and redraw later.
    जर (char_avail())
    {
	restore_last_शोध_pattern();
	is_state->incsearch_postponed = TRUE;
	परत;
    }
    is_state->incsearch_postponed = FALSE;

    जर (शोध_first_ओळ == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->शोध_start;
    अन्यथा जर (शोध_first_ओळ > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	curwin->w_cursor.col = MAXCOL;
    }
    अन्यथा
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = शोध_first_ओळ;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    जर (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    अन्यथा
    {
	पूर्णांक शोध_flags = शोध_OPT + शोध_NOOF + शोध_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	जर (!p_hls)
	    शोध_flags += शोध_KEEP;
	जर (शोध_first_ओळ != 0)
	    शोध_flags += शोध_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_शोध(रिक्त, firstc == ':' ? '/' : firstc, शोध_delim,
				 ccline.cmdbuff + skiplen, patlen, मोजणी, शोध_flags,
# ifdef FEAT_RELTIME
		&sia
# अन्यथा
		रिक्त
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	जर (curwin->w_cursor.lnum < शोध_first_ओळ
		|| curwin->w_cursor.lnum > शोध_last_ओळ)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->शोध_start;
	}

	// if interrupted while searching, behave like it failed
	जर (got_int)
	{
	    (शून्य)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	अन्यथा जर (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    जर (found != 0)
	highlight_match = TRUE;		// highlight position
    अन्यथा
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    जर (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_शोध_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    अन्यथा
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    जर (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	जर (empty_pattern(ccline.cmdbuff + skiplen, (माप_प्रकार)patlen, शोध_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    जर (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_शोध_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    जर (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->शोध_start;
    अन्यथा जर (found != 0)
	curwin->w_cursor = end_pos;

    संदेश_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    स्थिर पूर्णांक
may_adjust_incsearch_highlighting(
	पूर्णांक			firstc,
	दीर्घ			मोजणी,
	incsearch_state_T	*is_state,
	पूर्णांक			c)
{
    पूर्णांक		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = रिक्त;
    char_u	*searchstr;
    char_u	*strcopy = रिक्त;
    माप_प्रकार	searchstrlen;
    माप_प्रकार	patlen_s;
    soffset_T	offset;
    पूर्णांक		शोध_flags = शोध_NOOF;
    पूर्णांक		i;
    पूर्णांक		शोध_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_शोध_pattern();

    जर (!do_incsearch_highlighting(firstc, &शोध_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_शोध_pattern();
	परत OK;
    }
    जर (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_शोध_pattern();
	परत FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (माप_प्रकार)patlen;
    patlen_s = (माप_प्रकार)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (शून्य)parse_शोध_pattern_offset(&pat, &patlen_s, शोध_delim,
				    शोध_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    जर (c == Ctrl_G)
    {
	t = is_state->match_end;
	जर (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (शून्य)decl(&t);
	शोध_flags += शोध_COL;
    }
    अन्यथा
	t = is_state->match_start;
    जर (!p_hls)
	शोध_flags += शोध_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, रिक्त,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, मोजणी, शोध_flags, RE_शोध, रिक्त);
    --emsg_off;
    जर (dircp != रिक्त)
	*dircp = शोध_delim;
    जर (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	दीर्घ	off = offset.off;

	is_state->शोध_start = match_start;
	जर (!offset.ओळ && (offset.end || off != 0))
	{
	    जर (offset.end)
	    {
		is_state->शोध_start = match_end;
		(शून्य)decl(&is_state->शोध_start);
	    }
	    पर्यंत (off > 0)
	    {
		जर (incl(&is_state->शोध_start) == -1)
		    थांबा;
		--off;
	    }
	    पर्यंत (off < 0)
	    {
		जर (decl(&is_state->शोध_start) == -1)
		    थांबा;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	जर (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->शोध_start = t;
	    (शून्य)decl(&is_state->शोध_start);
	}
	अन्यथा जर (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->शोध_start = t;
	    (शून्य)incl(&is_state->शोध_start);
	}
	जर (LT_POS(t, is_state->शोध_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->शोध_start = t;
	    जर (firstc == '?')
		(शून्य)incl(&is_state->शोध_start);
	    अन्यथा
		(शून्य)decl(&is_state->शोध_start);
	}

	set_शोध_match(&is_state->match_end);
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
    अन्यथा
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_शोध_pattern();
    परत FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    स्थिर पूर्णांक
may_add_char_to_शोध(पूर्णांक firstc, पूर्णांक *c, incsearch_state_T *is_state)
{
    पूर्णांक		skiplen, patlen, शोध_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_शोध_pattern();

    जर (!do_incsearch_highlighting(firstc, &शोध_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_शोध_pattern();
	परत FAIL;
    }
    restore_last_शोध_pattern();

    // Add a character from under the cursor for 'incsearch'.
    जर (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	जर (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    जर (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    जर (*c == शोध_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != रिक्त)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    जर (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		पूर्णांक save_c = *c;

		पर्यंत (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    परत FAIL;
	}
    }
    परत OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    स्थिर पूर्णांक
cmdline_has_arabic(पूर्णांक start, पूर्णांक len)
{
    पूर्णांक	    j;
    पूर्णांक	    mb_l;
    पूर्णांक	    u8c;
    char_u  *p;
    पूर्णांक	    u8cc[MAX_MCO];

    जर (!enc_utf8)
	परत FALSE;

    साठी (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	जर (ARABIC_CHAR(u8c))
	    परत TRUE;
    }
    परत FALSE;
}
#endif

    शून्य
cmdline_init(शून्य)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    स्थिर पूर्णांक
cmdline_handle_ctrl_bsl(पूर्णांक c, पूर्णांक *gotesc)
{
    ++no_mapping;
    ++allow_किल्ल्या;
    c = plain_vgetc();
    --no_mapping;
    --allow_किल्ल्या;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    जर (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && किल्ली_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	परत PROCESS_NEXT_किल्ली;
    }

#ifdef FEAT_EVAL
    जर (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	जर (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	अन्यथा
	    new_cmdpos = ccline.cmdpos;

	c = get_अभिव्यक्ती_register();
	जर (c == '=')
	{
	    char_u	*p = रिक्त;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_अभिव्यक्ती_ओळ();
	    --textlock;

	    जर (p != रिक्त)
	    {
		पूर्णांक len = (पूर्णांक)STRLEN(p);

		जर (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    जर (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    अन्यथा
			ccline.cmdpos = new_cmdpos;

		    किल्ली_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    परत CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	परत CMDLINE_NOT_CHANGED;
    }
#endif

    जर (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    परत GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    स्थिर पूर्णांक
cmdline_wildchar_complete(
	पूर्णांक		c,
	पूर्णांक		escape,
	पूर्णांक		*did_wild_list,
	पूर्णांक		*wim_index_p,
	expand_T	*xp,
	पूर्णांक		*gotesc,
	पूर्णांक		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    पूर्णांक		wim_index = *wim_index_p;
    पूर्णांक		res;
    पूर्णांक		cmdpos_before;
    पूर्णांक		options = WILD_NO_BEEP;
    पूर्णांक		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    पूर्णांक		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    जर (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    जर (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	जर (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (शून्य)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	जर (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	अन्यथा जर (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	अन्यथा
	    res = OK;	    // don't insert 'wildchar' now
    }
    अन्यथा		    // typed p_wc first time
    {
	पूर्णांक wim_longest = (wim_flags[0] & WIM_LONGEST);
	पूर्णांक wim_list = (wim_flags[0] & WIM_LIST);
	पूर्णांक wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	जर (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    जर (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    जर (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    अन्यथा
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	जर (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	अन्यथा
	{
	    जर (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    जर (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	जर (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	जर (got_int)
	{
	    (शून्य)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (शून्य)ExpandOne(xp, रिक्त, रिक्त, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    परत CMDLINE_CHANGED;
	}

	// Display matches
	जर (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    जर (wim_longest)
	    {
		पूर्णांक found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		पूर्णांक show_menu = p_wmnu && wim_full;

		जर (wim_list || show_menu)
		    (शून्य)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		अन्यथा जर (!found_longest_prefix)
		{
		    पूर्णांक wim_list_next = (wim_flags[1] & WIM_LIST);
		    पूर्णांक wim_full_next = (wim_flags[1] & WIM_FULL);
		    पूर्णांक wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    पूर्णांक wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    जर (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			जर (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			अन्यथा
			    (शून्य)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			जर (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    अन्यथा
	    {
		पूर्णांक show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		जर (wim_list || show_menu)
		    (शून्य)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		अन्यथा
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    जर (wim_list)
		*did_wild_list = TRUE;
	}
	अन्यथा जर (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    जर (wim_index < 3)
	++wim_index;
    जर (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    परत (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    स्थिर पूर्णांक
cmdline_erase_chars(
	पूर्णांक c,
	पूर्णांक indent
#ifdef FEAT_शोध_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    पूर्णांक		i;
    पूर्णांक		j;

    जर (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    जर (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    जर (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    जर (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	जर (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    जर (c == Ctrl_W)
	    {
		पर्यंत (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		पर्यंत (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		जर (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	अन्यथा जर (c == Ctrl_W)
	{
	    पर्यंत (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    जर (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		पर्यंत (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	अन्यथा
	    --p;
	ccline.cmdpos = (पूर्णांक)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	पर्यंत (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_शोध_EXTRA
	जर (ccline.cmdlen == 0)
	{
	    isp->शोध_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    अन्यथा जर (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == रिक्त && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	जर (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    परत CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	जर (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    जर (cmdmsg_rl)
		संदेश_col = cmdline_width;
	    अन्यथा
#endif
		संदेश_col = 0;
	    संदेश_putchar(' ');		// delete ':'
	}
#ifdef FEAT_शोध_EXTRA
	isp->शोध_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	परत GOTO_NORMAL_MODE;
    }
    परत CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    स्थिर शून्य
cmdline_toggle_langmap(दीर्घ *b_im_ptr)
{
    जर (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_आदान_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	जर (b_im_ptr != रिक्त)
	{
	    जर (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    अन्यथा
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_आदान_METHOD
    अन्यथा
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	जर ((p_imdisable && b_im_ptr != रिक्त)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    जर (b_im_ptr != रिक्त)
		*b_im_ptr = B_IMODE_NONE;
	}
	अन्यथा
	{
	    im_set_active(TRUE);	// Enable input method
	    जर (b_im_ptr != रिक्त)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    जर (b_im_ptr != रिक्त)
    {
	जर (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	अन्यथा
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#जर defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    स्थिर पूर्णांक
cmdline_घाला_reg(पूर्णांक *gotesc UNUSED)
{
    पूर्णांक		i;
    पूर्णांक		c;
    पूर्णांक		literally = FALSE;
#ifdef FEAT_EVAL
    पूर्णांक		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_किल्ल्या;
    i = c = plain_vgetc();	// CTRL-R <char>
    जर (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    जर (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_किल्ल्या;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    जर (c == '=')
    {
	जर (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	अन्यथा
	    c = get_अभिव्यक्ती_register();
    }
#endif
    जर (c != ESC)	    // use ESC to cancel inserting register
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
	जर (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    परत GOTO_NORMAL_MODE;
	}
#endif
	किल्ली_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	जर (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    जर (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    अन्यथा
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
    परत literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    स्थिर शून्य
cmdline_डावा_उजवा_mouse(पूर्णांक c, पूर्णांक *ignore_drag_release)
{
    जर (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    अन्यथा
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    जर (!gui.in_use)
#endif
	जर (!mouse_has(MOUSE_COMMAND))
	    परत;
#ifdef FEAT_CLIPBOARD
    जर (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	पूर्णांक	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	जर (mouse_model_popup() && button == MOUSE_डावा
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_उजवा;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	परत;
    }
#endif

    set_cmdspos();
    साठी (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	पूर्णांक	i;

	i = cmdline_charsize(ccline.cmdpos);
	जर (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    थांबा;
	जर (has_mbyte)
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
    स्थिर पूर्णांक
cmdline_browse_history(
	पूर्णांक	c,
	पूर्णांक	firstc,
	char_u	**curcmdstr,
	माप_प्रकार	*curcmdstrlen,
	पूर्णांक	histype,
	पूर्णांक	*hiscnt_p,
	expand_T *xp)
{
    पूर्णांक		orig_hiscnt;
    पूर्णांक		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    माप_प्रकार	lookforlen = *curcmdstrlen;
    पूर्णांक		res;

    जर (get_hislen() == 0 || firstc == NUL)	// no history
	परत CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    जर (lookfor == रिक्त)
    {
	जर ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == रिक्त)
	    परत CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    साठी (;;)
    {
	// one step backwards
	जर (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    जर (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    अन्यथा जर (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    अन्यथा जर (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    अन्यथा			// at top of list
	    {
		hiscnt = orig_hiscnt;
		थांबा;
	    }
	}
	अन्यथा    // one step forwards
	{
	    // on last entry, clear the line
	    जर (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		थांबा;
	    }

	    // not on a history line, nothing to do
	    जर (hiscnt == get_hislen())
		थांबा;
	    जर (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    अन्यथा
		++hiscnt;
	}
	जर (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== रिक्त)
	{
	    hiscnt = orig_hiscnt;
	    थांबा;
	}
	जर ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    थांबा;
    }

    जर (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	माप_प्रकार	plen;
	पूर्णांक	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	जर (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	अन्यथा
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	जर (histype == HIST_शोध
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    पूर्णांक     i;
	    पूर्णांक     j;
	    माप_प्रकार  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    साठी (i = 0; i <= 1; ++i)
	    {
		len = 0;
		साठी (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    जर (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			जर (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    अन्यथा
		    {
			// Escape new sep, unless it is already
			// escaped.
			जर (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    जर (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			जर (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		जर (i == 0)
		{
		    alloc_cmdbuff(len);
		    जर (ccline.cmdbuff == रिक्त)
		    {
			res = GOTO_NORMAL_MODE;
			जा done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (पूर्णांक)len;
	}
	अन्यथा
	{
	    alloc_cmdbuff(plen);
	    जर (ccline.cmdbuff == रिक्त)
	    {
		res = GOTO_NORMAL_MODE;
		जा done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (पूर्णांक)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	जा done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    परत res;
}

/*
 * Initialize the current command-line info.
 */
    स्थिर पूर्णांक
init_ccline(पूर्णांक firstc, पूर्णांक indent)
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
    जर (ccline.cmdbuff == रिक्त)
    {
	ccline.cmdbufflen = 0;
	परत FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    जर (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    परत OK;
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
    पूर्णांक		  firstc,
    दीर्घ	  मोजणी,	// only used for incremental search
    पूर्णांक		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    परत getcmdline_int(firstc, मोजणी, indent, TRUE);
}

    स्थिर char_u *
getcmdline_int(
    पूर्णांक		firstc,
    दीर्घ	मोजणी UNUSED,	// only used for incremental search
    पूर्णांक		indent,		// indent for inside conditionals
    पूर्णांक		clear_ccline)	// clear ccline first
{
    स्थिर पूर्णांक	depth = 0;	    // call depth
    पूर्णांक		c = 0;
    पूर्णांक		i;
    पूर्णांक		j;
    पूर्णांक		gotesc = FALSE;		// TRUE when <ESC> just typed
    पूर्णांक		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = रिक्त;	// string to match
    माप_प्रकार	lookforlen = 0;
    पूर्णांक		hiscnt;			// current history line in use
    पूर्णांक		histype;		// history type to be used
#ifdef FEAT_शोध_EXTRA
    incsearch_state_T	is_state;
#endif
    पूर्णांक		did_wild_list = FALSE;	// did wild_list() recently
    पूर्णांक		wim_index = 0;		// index in wim_flags[]
    पूर्णांक		res;
    पूर्णांक		save_संदेश_scroll = संदेश_scroll;
    पूर्णांक		save_State = State;	// remember State when called
    पूर्णांक		some_किल्ली_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    पूर्णांक		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    पूर्णांक		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    दीर्घ	*b_im_ptr = रिक्त;
    buf_T	*b_im_ptr_buf = रिक्त;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    पूर्णांक		did_save_ccline = FALSE;
    पूर्णांक		cmdline_type;
    पूर्णांक		wild_type = 0;
    पूर्णांक		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = रिक्त;
    पूर्णांक		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    जर (ccline.cmdbuff != रिक्त)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    जर (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    जर (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    जर (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_शोध_EXTRA
    init_incsearch_state(&is_state);
#endif

    जर (init_ccline(firstc, indent) != OK)
	जा theend;	// out of memory

    जर (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	जा theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    जर (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    अन्यथा
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    जर (!cmd_silent)
    {
	i = संदेश_scrolled;
	संदेश_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	संदेश_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#जर defined(FEAT_EVAL)
    जर (ccline.आदान_fn)
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
    संदेश_scroll = FALSE;

    State = MODE_CMDLINE;

    जर (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	जर (curbuf->b_p_imsearch == B_IMODE_USE_घाला)
	    b_im_ptr = &curbuf->b_p_iminsert;
	अन्यथा
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	जर (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_आदान_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_आदान_METHOD
    अन्यथा जर (p_imcmdline)
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
    जर (!debug_mode)
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
    जर (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    जर (!cmd_silent && संदेश_scrolled == 0)
    {
	पूर्णांक	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    जर (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	जर (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	जर (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    साठी (;;)
    {
	पूर्णांक	trigger_cmdlinechanged = TRUE;
	पूर्णांक	end_wildmenu;
	पूर्णांक	prev_cmdpos = ccline.cmdpos;
	पूर्णांक	skip_pum_redraw = FALSE;

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

	जर (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_किल्ली_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	जर (ccline.cmdbuff != रिक्त)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    जर (prev_cmdbuff == रिक्त)
		जा returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	जर (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	करा
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} पर्यंत (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	जर (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    जर (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (शून्य)ExpandOne(&xpc, रिक्त, रिक्त, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	जर (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    सुरूठेवा;
	}

	जर (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    पूर्णांक	    clen = ccline.cmdlen;
	    पूर्णांक	    cc_मोजणी = aucmd_cmdline_changed_मोजणी;

	    जर (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		जर (clen == ccline.cmdlen
				    || cc_मोजणी != aucmd_cmdline_changed_मोजणी)
		    trigger_cmdlinechanged = FALSE;
		जा cmdline_changed;
	    }
	}

	जर (किल्ली_Typed)
	{
	    some_किल्ली_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    जर (cmd_hkmap)
		c = hkmap(c);
	    जर (cmdmsg_rl && !किल्ली_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		निवड (c)
		{
		    स्थिती K_उजवा:   c = K_डावा; थांबा;
		    स्थिती K_S_उजवा: c = K_S_डावा; थांबा;
		    स्थिती K_C_उजवा: c = K_C_डावा; थांबा;
		    स्थिती K_डावा:    c = K_उजवा; थांबा;
		    स्थिती K_S_डावा:  c = K_S_उजवा; थांबा;
		    स्थिती K_C_डावा:  c = K_C_उजवा; थांबा;
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
	जर ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#जर defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	जर (lookfor != रिक्त
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_डावा && c != K_उजवा
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	जर (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	जर (p_wmnu)
	    c = wildmenu_translate_किल्ली(&ccline, c, &xpc, did_wild_list);

	पूर्णांक किल्ली_is_wc = (c == p_wc && किल्ली_Typed) || c == p_wcm;
	जर ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !किल्ली_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    जर (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		जर (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    थांबा;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	जर (किल्ली_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#जर defined(FEAT_शोध_EXTRA)
	    जर ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!किल्ली_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	जर (end_wildmenu)
	{
	    जर (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !किल्ली_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    जर (xpc.xp_numfiles != -1)
		(शून्य)ExpandOne(&xpc, रिक्त, रिक्त, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    जर (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	जर (p_wmnu)
	    c = wildmenu_process_किल्ली(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	जर (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    जर (res == CMDLINE_CHANGED)
		जा cmdline_changed;
	    अन्यथा जर (res == CMDLINE_NOT_CHANGED)
		जा cmdline_not_changed;
	    अन्यथा जर (res == GOTO_NORMAL_MODE)
		जा returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	जर (c == cedit_किल्ली || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    जर ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_किल्ली_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	अन्यथा
	    c = do_digraph(c);
#endif

	जर (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!किल्ली_Typed || vim_strchr(p_cpo, CPO_ESC) != रिक्त)))
	{
	    // In Ex mode a backslash escapes a newline.
	    जर (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		जर (c == K_KENTER)
		    c = '\n';
	    }
	    अन्यथा
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		जर (ccheck_abbr(c + ABBR_OFF))
		    जा cmdline_changed;
		जर (!cmd_silent)
		{
		    windgoto(संदेश_row, cmdline_col_off);
		    out_flush();
		}
		थांबा;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	जर ((c == p_wc && !gotesc && किल्ली_Typed) || c == p_wcm || c == K_WILD)
	{
	    जर (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_शोध_EXTRA
		    &is_state.शोध_start
#अन्यथा
		    रिक्त
#endif
		    );
	    जर (c == K_WILD)
		--emsg_silent;
	    जर (res == CMDLINE_CHANGED)
		जा cmdline_changed;
	    जर (c == K_WILD)
		जा cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	जर (c == K_S_TAB && किल्ली_Typed)
	{
	    जर (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		जर (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    पूर्णांक list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		जर (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    जा cmdline_changed;
	    }
	}

	जर (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	जर (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_शोध_EXTRA
	    // Apply search highlighting
	    जर (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    जर (किल्ली_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, मोजणी, &is_state);
#endif
	    wild_type = 0;
	    जा cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	निवड (c)
	{
	स्थिती K_BS:
	स्थिती Ctrl_H:
	स्थिती K_DEL:
	स्थिती K_KDEL:
	स्थिती Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_शोध_EXTRA
		    , &is_state
#endif
		    );
	    जर (res == CMDLINE_NOT_CHANGED)
		जा cmdline_not_changed;
	    अन्यथा जर (res == GOTO_NORMAL_MODE)
		जा returncmd;		// back to cmd mode
	    जा cmdline_changed;

	स्थिती K_INS:
	स्थिती K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		जा cmdline_not_changed;

	स्थिती Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : रिक्त);
		जा cmdline_not_changed;

//	case '@':   only in very old vi
	स्थिती Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		पर्यंत (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_शोध_EXTRA
		जर (ccline.cmdlen == 0)
		    is_state.शोध_start = is_state.save_cursor;
#endif
		redrawcmd();
		जा cmdline_changed;

#ifdef FEAT_CLIPBOARD
	स्थिती Ctrl_Y:
		// Copy the modeless selection, if there is one.
		जर (clip_star.state != SELECT_CLEARED)
		{
		    जर (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    जा cmdline_not_changed;
		}
		थांबा;
#endif

	स्थिती ESC:	// get here if p_wc != ESC or when ESC typed twice
	स्थिती Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		जर (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    जा cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		जा returncmd;		// back to cmd mode

	स्थिती Ctrl_R:			// insert register
		res = cmdline_घाला_reg(&gotesc);
		जर (res == GOTO_NORMAL_MODE)
		    जा returncmd;
		जर (res == CMDLINE_CHANGED)
		    जा cmdline_changed;
		जा cmdline_not_changed;

	स्थिती Ctrl_D:
		जर (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    थांबा;	// Use ^D as normal char instead

		redrawcmd();
		सुरूठेवा;	// don't do incremental search now

	स्थिती K_उजवा:
	स्थिती K_S_उजवा:
	स्थिती K_C_उजवा:
		करा
		{
		    जर (ccline.cmdpos >= ccline.cmdlen)
			थांबा;
		    i = cmdline_charsize(ccline.cmdpos);
		    जर (किल्ली_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			थांबा;
		    ccline.cmdspos += i;
		    जर (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    अन्यथा
			++ccline.cmdpos;
		}
		पर्यंत ((c == K_S_उजवा || c == K_C_उजवा
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		जर (has_mbyte)
		    set_cmdspos_cursor();
		जा cmdline_not_changed;

	स्थिती K_डावा:
	स्थिती K_S_डावा:
	स्थिती K_C_डावा:
		जर (ccline.cmdpos == 0)
		    जा cmdline_not_changed;
		करा
		{
		    --ccline.cmdpos;
		    जर (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		पर्यंत (ccline.cmdpos > 0
			&& (c == K_S_डावा || c == K_C_डावा
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		जर (has_mbyte)
		    set_cmdspos_cursor();
		जा cmdline_not_changed;

	स्थिती K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		जा cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	स्थिती K_F4:
	    जर (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		जा cmdline_not_changed;
	    }
	    थांबा;
#endif

	स्थिती K_MIDDLEDRAG:
	स्थिती K_MIDDLERELEASE:
		जा cmdline_not_changed;	// Ignore mouse

	स्थिती K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		जर (!gui.in_use)
#endif
		    जर (!mouse_has(MOUSE_COMMAND))
			जा cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		जर (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		अन्यथा
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		जा cmdline_changed;

#ifdef FEAT_DND
	स्थिती K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		जा cmdline_changed;
#endif

	स्थिती K_LEFTDRAG:
	स्थिती K_LEFTRELEASE:
	स्थिती K_RIGHTDRAG:
	स्थिती K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		जर (ignore_drag_release)
		    जा cmdline_not_changed;
		// FALLTHROUGH
	स्थिती K_LEFTMOUSE:
	स्थिती K_RIGHTMOUSE:
		cmdline_डावा_उजवा_mouse(c, &ignore_drag_release);
		जा cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	स्थिती K_MOUSEDOWN:
	स्थिती K_MOUSEUP:
	स्थिती K_MOUSELEFT:
	स्थिती K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		जर (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_डावा
				      : MSCR_उजवा);
#endif
		जा cmdline_not_changed;

	// Alternate buttons ignored here
	स्थिती K_X1MOUSE:
	स्थिती K_X1DRAG:
	स्थिती K_X1RELEASE:
	स्थिती K_X2MOUSE:
	स्थिती K_X2DRAG:
	स्थिती K_X2RELEASE:
	स्थिती K_MOUSEMOVE:
		जा cmdline_not_changed;

#ifdef FEAT_GUI
	स्थिती K_LEFTMOUSE_NM:	// mousefocus click, ignored
	स्थिती K_LEFTRELEASE_NM:
		जा cmdline_not_changed;

	स्थिती K_VER_SCROLLBAR:
		जर (संदेश_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		जा cmdline_not_changed;

	स्थिती K_HOR_SCROLLBAR:
		जर (संदेश_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_मूल्य);
		    redrawcmd();
		}
		जा cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	स्थिती K_TABLINE:
	स्थिती K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		जर (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		जा cmdline_not_changed;
#endif

	स्थिती K_SELECT:	    // end of Select mode mapping - ignore
		जा cmdline_not_changed;

	स्थिती Ctrl_B:	    // begin of command line
	स्थिती K_HOME:
	स्थिती K_KHOME:
	स्थिती K_S_HOME:
	स्थिती K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		जा cmdline_not_changed;

	स्थिती Ctrl_E:	    // end of command line
	स्थिती K_END:
	स्थिती K_KEND:
	स्थिती K_S_END:
	स्थिती K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		जा cmdline_not_changed;

	स्थिती Ctrl_A:	    // all matches
		जर (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		जर (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    थांबा;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		जा cmdline_changed;

	स्थिती Ctrl_L:
#ifdef FEAT_शोध_EXTRA
		जर (may_add_char_to_शोध(firstc, &c, &is_state) == OK)
		    जा cmdline_not_changed;
#endif

		// completion: longest common part
		जर (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    थांबा;
		जा cmdline_changed;

	स्थिती Ctrl_N:	    // next match
	स्थिती Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		जर (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    जर (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    जा cmdline_not_changed;
		}
#endif
		जर (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    जर (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			थांबा;
		    जा cmdline_changed;
		}
		// FALLTHROUGH
	स्थिती K_UP:
	स्थिती K_DOWN:
	स्थिती K_S_UP:
	स्थिती K_S_DOWN:
	स्थिती K_PAGEUP:
	स्थिती K_KPAGEUP:
	स्थिती K_PAGEDOWN:
	स्थिती K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		जर (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    पूर्णांक	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    जर (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    जा cmdline_not_changed;
		}
#endif
		जर (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    जर (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    जर (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			थांबा;
		    जा cmdline_changed;
		}
		अन्यथा
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    जर (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			जा cmdline_changed;
		    }
		    अन्यथा जर (res == GOTO_NORMAL_MODE)
			जा returncmd;
		}
		जा cmdline_not_changed;

#ifdef FEAT_शोध_EXTRA
	स्थिती Ctrl_G:	    // next match
	स्थिती Ctrl_T:	    // previous match
		जर (may_adjust_incsearch_highlighting(
					  firstc, मोजणी, &is_state, c) == FAIL)
		    जा cmdline_not_changed;
		थांबा;
#endif

	स्थिती Ctrl_V:
	स्थिती Ctrl_Q:
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
		    जर (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			संदेश_putchar(' ');
			cursorcmd();
		    }
		}

		थांबा;

#ifdef FEAT_DIGRAPHS
	स्थिती Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		जर (c != NUL)
		    थांबा;

		redrawcmd();
		जा cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	स्थिती Ctrl__:	    // CTRL-_: switch language mode
		जर (!p_ari)
		    थांबा;
		cmd_hkmap = !cmd_hkmap;
		जा cmdline_not_changed;
#endif

	स्थिती K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, रिक्त);
		जा cmdline_changed;

	पूर्वनिर्धारित:
#ifdef UNIX
		जर (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    जा returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		जर (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		थांबा;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	जर (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    जा cmdline_changed;

	/*
	 * put the character in the command line
	 */
	जर (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_किल्ली_name(c, mod_mask), -1, TRUE);
	अन्यथा
	{
	    जर (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    अन्यथा
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	जा cmdline_changed;

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
	जर (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_शोध_EXTRA
	जर (!is_state.incsearch_postponed)
	    सुरूठेवा;
#endif

cmdline_changed:
#ifdef FEAT_शोध_EXTRA
	// If the window changed incremental search state is not valid.
	जर (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	जर (xpc.xp_context == EXPAND_NOTHING && (किल्ली_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, मोजणी, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	जर (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != रिक्त &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    जर (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	जर (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	जर (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    जर (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    जर (!event_cmdlineleavepre_triggered)
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
    जर (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = रिक्त;
    clear_cmdline_orig();

#ifdef FEAT_शोध_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    जर (ccline.cmdbuff != रिक्त)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	जर (ccline.cmdlen && firstc != NUL
		&& (some_किल्ली_typed || histype == HIST_शोध))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_शोध ? firstc : NUL);
	    जर (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	जर (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    संदेश_check();
    संदेश_scroll = save_संदेश_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    जर (some_किल्ली_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    जर (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_आदान_METHOD
    जर (b_im_ptr != रिक्त && buf_valid(b_im_ptr_buf)
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
	जर (did_save_ccline)
	    restore_cmdline(&save_ccline);
	अन्यथा
	    ccline.cmdbuff = रिक्त;

	vim_free(prev_cmdbuff);
	परत p;
    }
}

#जर defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    पूर्णांक		firstc,
    char_u	*prompt,	// command line prompt
    पूर्णांक		attr,		// attributes for prompt
    पूर्णांक		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    पूर्णांक			did_save_ccline = FALSE;
    पूर्णांक			संदेश_col_save = संदेश_col;
    पूर्णांक			संदेश_silent_save = संदेश_silent;

    जर (ccline.cmdbuff != रिक्त)
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
    ccline.आदान_fn = (firstc == '@');
# endif
    संदेश_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    जर (did_save_ccline)
	restore_cmdline(&save_ccline);

    संदेश_silent = संदेश_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    जर (ccline.cmdbuff != रिक्त)
	संदेश_col = संदेश_col_save;

    परत s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    पूर्णांक
check_opt_wim(शून्य)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		idx = 0;

    साठी (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    साठी (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	साठी (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	जर (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    परत FAIL;
	जर (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	अन्यथा जर (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	अन्यथा जर (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	अन्यथा जर (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	अन्यथा जर (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	अन्यथा जर (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	अन्यथा
	    परत FAIL;
	p += i;
	जर (*p == NUL)
	    थांबा;
	जर (*p == ',')
	{
	    जर (idx == 3)
		परत FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    पर्यंत (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    साठी (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    परत OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    पूर्णांक
text_locked(शून्य)
{
    जर (cmdwin_type != 0)
	परत TRUE;
    परत textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    शून्य
text_locked_संदेश(शून्य)
{
    emsg(_(get_text_locked_संदेश()));
}

    अक्षर *
get_text_locked_संदेश(शून्य)
{
    जर (cmdwin_type != 0)
	परत e_invalid_in_cmdline_window;
    परत e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    पूर्णांक
text_or_buf_locked(शून्य)
{
    जर (text_locked())
    {
	text_locked_संदेश();
	परत TRUE;
    }
    परत curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    पूर्णांक
curbuf_locked(शून्य)
{
    जर (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	परत TRUE;
    }
    परत allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    पूर्णांक
allbuf_locked(शून्य)
{
    जर (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	परत TRUE;
    }
    परत FALSE;
}

    स्थिर पूर्णांक
cmdline_charsize(पूर्णांक idx)
{
#जर defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    जर (cmdline_star > 0)	    // showing '*', always 1 position
	परत 1;
#endif
    परत ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    स्थिर शून्य
set_cmdspos(शून्य)
{
    जर (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    अन्यथा
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    स्थिर शून्य
set_cmdspos_cursor(शून्य)
{
    पूर्णांक		i, m, c;

    set_cmdspos();
    जर (किल्ली_Typed)
    {
	m = cmdline_width * Rows;
	जर (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    अन्यथा
	m = MAXCOL;
    साठी (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	जर (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	जर ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    थांबा;
	}
	जर (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    स्थिर शून्य
correct_cmdspos(पूर्णांक idx, पूर्णांक cells)
{
    जर ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    पूर्णांक		c,		// normally ':', NUL for ":append"
    शून्य	*cookie UNUSED,
    पूर्णांक		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    जर (exec_from_reg && vpeekc() == ':')
	(शून्य)vgetc();
    परत getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    पूर्णांक		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    शून्य	*cookie UNUSED,
    पूर्णांक		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	ओळ_ga;
    char_u	*pend;
    पूर्णांक		startcol = 0;
    पूर्णांक		c1 = 0;
    पूर्णांक		escaped = FALSE;	// CTRL-V typed
    पूर्णांक		vcol = 0;
    char_u	*p;
    पूर्णांक		prev_char;
    पूर्णांक		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    जर ((संदेश_col || संदेश_didout) && promptc != '?')
	संदेश_putchar('\n');
    जर (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	जर (p_prompt)
	    संदेश_putchar(':');
	पर्यंत (indent-- > 0)
	    संदेश_putchar(' ');
	startcol = संदेश_col;
    }

    ga_init2(&ओळ_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    जर (promptc <= 0)
    {
	vcol = indent;
	पर्यंत (indent >= 8)
	{
	    ga_append(&ओळ_ga, TAB);
	    संदेश_puts("        ");
	    indent -= 8;
	}
	पर्यंत (indent-- > 0)
	{
	    ga_append(&ओळ_ga, ' ');
	    संदेश_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_किल्ल्या;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    पर्यंत (!got_int)
    {
	दीर्घ    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	जर (ga_grow(&ओळ_ga, 40) == FAIL)
	    थांबा;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	जर (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	अन्यथा
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	जर (got_int)
	{
	    संदेश_putchar('\n');
	    थांबा;
	}

	जर (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &ओळ_ga);
	    जा redraw;
	}

	जर (!escaped)
	{
	    // CR typed means "enter", which is NL
	    जर (c1 == '\r')
		c1 = '\n';

	    जर (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		जर (ओळ_ga.ga_len > 0)
		{
		    जर (has_mbyte)
		    {
			p = (char_u *)ओळ_ga.ga_data;
			p[ओळ_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + ओळ_ga.ga_len - 1) + 1;
			ओळ_ga.ga_len -= len;
		    }
		    अन्यथा
			--ओळ_ga.ga_len;
		    जा redraw;
		}
		सुरूठेवा;
	    }

	    जर (c1 == Ctrl_U)
	    {
		संदेश_col = startcol;
		संदेश_clr_eos();
		ओळ_ga.ga_len = 0;
		जा redraw;
	    }

	    जर (c1 == Ctrl_T)
	    {
		sw = get_sw_मूल्य(curbuf);
		p = (char_u *)ओळ_ga.ga_data;
		p[ओळ_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		पर्यंत (get_indent_str(p, 8, FALSE) < indent)
		{
		    (शून्य)ga_grow(&ओळ_ga, 2);  // one more for the NUL
		    p = (char_u *)ओळ_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, ओळ_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++ओळ_ga.ga_len;
		}
redraw:
		// redraw the line
		संदेश_col = startcol;
		vcol = 0;
		p = (char_u *)ओळ_ga.ga_data;
		p[ओळ_ga.ga_len] = NUL;
		पर्यंत (p < (char_u *)ओळ_ga.ga_data + ओळ_ga.ga_len)
		{
		    जर (*p == TAB)
		    {
			करा
			    संदेश_putchar(' ');
			पर्यंत (++vcol % 8);
			++p;
		    }
		    अन्यथा
		    {
			len = mb_ptr2len(p);
			संदेश_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		संदेश_clr_eos();
		windgoto(संदेश_row, cmdline_col_off + संदेश_col);
		सुरूठेवा;
	    }

	    जर (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)ओळ_ga.ga_data;
		जर (prev_char == '0' || prev_char == '^')
		{
		    जर (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--ओळ_ga.ga_len] = NUL;
		}
		अन्यथा
		{
		    p[ओळ_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    जर (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_मूल्य(curbuf);
		    }
		}
		पर्यंत (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, ओळ_ga.ga_len - (s - p) + 1);
		    --ओळ_ga.ga_len;
		}
		जा add_indent;
	    }

	    जर (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		सुरूठेवा;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    जर (IS_SPECIAL(c1))
		सुरूठेवा;
	}

	जर (IS_SPECIAL(c1))
	    c1 = '?';
	जर (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)ओळ_ga.ga_data + ओळ_ga.ga_len);
	अन्यथा
	{
	    len = 1;
	    ((char_u *)ओळ_ga.ga_data)[ओळ_ga.ga_len] = c1;
	}
	जर (c1 == '\n')
	    संदेश_putchar('\n');
	अन्यथा जर (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    करा
		संदेश_putchar(' ');
	    पर्यंत (++vcol % 8);
	}
	अन्यथा
	{
	    संदेश_outtrans_len(
		     ((char_u *)ओळ_ga.ga_data) + ओळ_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	ओळ_ga.ga_len += len;
	escaped = FALSE;

	windgoto(संदेश_row, cmdline_col_off + संदेश_col);
	pend = (char_u *)(ओळ_ga.ga_data) + ओळ_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	जर (ओळ_ga.ga_len > 0 && pend[-1] == '\n'
		&& (ओळ_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --ओळ_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    थांबा;
	}
    }

    --no_mapping;
    --allow_किल्ल्या;

    // make following messages go to the next line
    संदेश_didout = FALSE;
    संदेश_col = 0;
    जर (संदेश_row < Rows - 1)
	++संदेश_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    जर (got_int)
	ga_clear(&ओळ_ga);

    परत (char_u *)ओळ_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    पूर्णांक
cmdline_overstrike(शून्य)
{
    परत ccline.overstrike;
}

#जर defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    पूर्णांक
cmdline_at_end(शून्य)
{
    परत (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#जर defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(शून्य)
{
    जर (ccline.cmdbuff == रिक्त || ccline.cmdpos > ccline.cmdlen)
	परत MAXCOL;

    जर (has_mbyte)
    {
	colnr_T	col;
	पूर्णांक	i = 0;

	साठी (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	परत col;
    }
    अन्यथा
	परत ccline.cmdpos;
}
#endif

#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    स्थिर शून्य
redrawcmd_preedit(शून्य)
{
    जर ((State & MODE_CMDLINE)
	    && xic != रिक्त
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	पूर्णांक	cmdpos = 0;
	पूर्णांक	cmdspos;
	पूर्णांक	old_row;
	पूर्णांक	old_col;
	colnr_T	col;

	old_row = संदेश_row;
	old_col = संदेश_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	जर (has_mbyte)
	{
	    साठी (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	अन्यथा
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	संदेश_row = cmdline_row + (cmdspos / cmdline_width);
	संदेश_col = cmdspos % cmdline_width;
	जर (संदेश_row >= Rows)
	    संदेश_row = Rows - 1;

	साठी (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    पूर्णांक char_len;
	    पूर्णांक char_attr;

	    char_attr = im_get_feedback_attr(col);
	    जर (char_attr < 0)
		थांबा; // end of preedit string

	    जर (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    अन्यथा
		char_len = 1;

	    संदेश_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	संदेश_row = old_row;
	संदेश_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    स्थिर शून्य
dealloc_cmdbuff(शून्य)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    स्थिर शून्य
alloc_cmdbuff(माप_प्रकार len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    जर (len < 80)
	len = 100;
    अन्यथा
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (पूर्णांक)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    पूर्णांक
realloc_cmdbuff(पूर्णांक len)
{
    char_u	*p;
    पूर्णांक		plen;

    जर (len < ccline.cmdbufflen)
	परत OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    जर (ccline.cmdbuff == रिक्त)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	परत FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (माप_प्रकार)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    जर (ccline.xpc != रिक्त
	    && ccline.xpc->xp_pattern != रिक्त
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	पूर्णांक i = (पूर्णांक)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	जर (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    परत OK;
}

#जर defined(FEAT_ARABIC)
स्थिर char_u	*arshape_buf = रिक्त;

# जर defined(EXITFREE)
    शून्य
free_arshape_buf(शून्य)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    स्थिर शून्य
draw_cmdline(पूर्णांक start, पूर्णांक len)
{
#जर defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    पूर्णांक		i;

    जर (cmdline_star > 0)
	साठी (i = 0; i < len; ++i)
	{
	    संदेश_putchar('*');
	    जर (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    अन्यथा
#endif
#ifdef FEAT_ARABIC
	जर (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	स्थिर पूर्णांक	buflen = 0;
	char_u		*p;
	पूर्णांक		j;
	पूर्णांक		newlen = 0;
	पूर्णांक		mb_l;
	पूर्णांक		pc, pc1 = 0;
	पूर्णांक		prev_c = 0;
	पूर्णांक		prev_c1 = 0;
	पूर्णांक		u8c;
	पूर्णांक		u8cc[MAX_MCO];
	पूर्णांक		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	जर (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    जर (arshape_buf == रिक्त)
		परत;	// out of memory
	}

	जर (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	साठी (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    जर (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		जर (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    जर (j + mb_l >= start + len)
			nc = NUL;
		    अन्यथा
			nc = utf_ptr2char(p + mb_l);
		}
		अन्यथा
		{
		    // displaying from left to right
		    जर (j + mb_l >= start + len)
			pc = NUL;
		    अन्यथा
		    {
			पूर्णांक	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, रिक्त, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		जर (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    जर (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    अन्यथा
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	संदेश_outtrans_len(arshape_buf, newlen);
    }
    अन्यथा
#endif
	संदेश_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    शून्य
putcmdline(पूर्णांक c, पूर्णांक shift)
{
    जर (cmd_silent)
	परत;
    संदेश_no_more = TRUE;
    संदेश_putchar(c);
    जर (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    संदेश_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    शून्य
unputcmdline(शून्य)
{
    जर (cmd_silent)
	परत;
    संदेश_no_more = TRUE;
    जर (ccline.cmdlen == ccline.cmdpos)
	संदेश_putchar(' ');
    अन्यथा जर (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    अन्यथा
	draw_cmdline(ccline.cmdpos, 1);
    संदेश_no_more = FALSE;
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
    पूर्णांक
put_on_cmdline(char_u *str, पूर्णांक len, पूर्णांक redraw)
{
    पूर्णांक		retval;
    पूर्णांक		i;
    पूर्णांक		m;
    पूर्णांक		c;

    जर (len < 0)
	len = (पूर्णांक)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    जर (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    अन्यथा
	retval = OK;
    जर (retval == OK)
    {
	जर (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (माप_प्रकार)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	अन्यथा
	{
	    जर (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		साठी (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		साठी (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		जर (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (माप_प्रकार)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		अन्यथा
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    अन्यथा जर (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (माप_प्रकार)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	जर (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    पर्यंत (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    जर (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		जर (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		अन्यथा
		    i = 0;
	    }
#endif
	    जर (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		संदेश_col -= i;
		जर (संदेश_col < 0)
		{
		    संदेश_col += cmdline_width;
		    --संदेश_row;
		}
	    }
	}

	जर (redraw && !cmd_silent)
	{
	    संदेश_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    जर (cmdline_row != i || ccline.overstrike)
		संदेश_clr_eos();
	    संदेश_no_more = FALSE;
	}
	जर (किल्ली_Typed)
	{
	    m = cmdline_width * Rows;
	    जर (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	अन्यथा
	    m = MAXCOL;
	साठी (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    जर (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    जर (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    जर (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		जर (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    जर (redraw)
	संदेश_check();
    परत retval;
}

स्थिर cmdline_info_T	prev_ccline;
स्थिर पूर्णांक		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    स्थिर शून्य
save_cmdline(cmdline_info_T *ccp)
{
    जर (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = रिक्त;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    स्थिर शून्य
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
    स्थिर पूर्णांक
cmdline_paste(
    पूर्णांक regname,
    पूर्णांक literally,	// Insert text literally instead of "as typed"
    पूर्णांक remcr)		// remove trailing CR
{
    दीर्घ		i;
    char_u		*arg;
    char_u		*p;
    पूर्णांक			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    जर (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	परत FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    ओळ_breakcheck();
    जर (got_int)
	परत FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    जर (i)
    {
	// Got the value of a special register in "arg".
	जर (arg == रिक्त)
	    परत FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	जर (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    पूर्णांक	    len;

	    // Locate start of last word in the cmd buffer.
	    साठी (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		जर (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    जर (!vim_iswordc(mb_ptr2char(w - len)))
			थांबा;
		    w -= len;
		}
		अन्यथा
		{
		    जर (!vim_iswordc(w[-1]))
			थांबा;
		    --w;
		}
	    }
	    len = (पूर्णांक)((ccline.cmdbuff + ccline.cmdpos) - w);
	    जर (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	जर (allocated)
	    vim_free(arg);
	परत OK;
    }

    परत cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    शून्य
cmdline_paste_str(char_u *s, पूर्णांक literally)
{
    पूर्णांक		c, cv;

    जर (literally)
	put_on_cmdline(s, -1, TRUE);
    अन्यथा
	पर्यंत (*s != NUL)
	{
	    cv = *s;
	    जर (cv == Ctrl_V && s[1])
		++s;
	    जर (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    अन्यथा
		c = *s++;
	    जर (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    शून्य
redrawcmdline(शून्य)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    शून्य
redrawcmdline_ex(पूर्णांक do_compute_cmdrow)
{
    जर (cmd_silent)
	परत;
    need_wait_return = FALSE;
    जर (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    स्थिर शून्य
redrawcmdprompt(शून्य)
{
    पूर्णांक		i;

    जर (cmd_silent)
	परत;
    जर (ccline.cmdfirstc != NUL)
	संदेश_putchar(ccline.cmdfirstc);
    जर (ccline.cmdprompt != रिक्त)
    {
	संदेश_puts_attr((अक्षर *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = संदेश_col + (संदेश_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	जर (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    अन्यथा
	साठी (i = ccline.cmdindent; i > 0; --i)
	    संदेश_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    शून्य
redrawcmd(शून्य)
{
    पूर्णांक save_in_echowindow = in_echowindow;

    जर (cmd_silent)
	परत;

    // when 'incsearch' is set there may be no command line while redrawing
    जर (ccline.cmdbuff == रिक्त)
    {
	windgoto(cmdline_row, cmdline_col_off);
	संदेश_clr_eos();
	परत;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    संदेश_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    संदेश_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    संदेश_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    संदेश_clr_eos();
    संदेश_no_more = FALSE;

    set_cmdspos_cursor();
    जर (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    संदेश_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    शून्य
compute_cmdrow(शून्य)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    जर (exmode_active || (संदेश_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    अन्यथा
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    शून्य
cursorcmd(शून्य)
{
    जर (cmd_silent)
	परत;

#ifdef FEAT_RIGHTLEFT
    जर (cmdmsg_rl)
    {
	संदेश_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	संदेश_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	जर (संदेश_row <= 0)
	    संदेश_row = Rows - 1;
    }
    अन्यथा
#endif
    {
	संदेश_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	संदेश_col = ccline.cmdspos % cmdline_width;
	जर (संदेश_row >= Rows)
	    संदेश_row = Rows - 1;
    }

    windgoto(संदेश_row, cmdline_col_off + संदेश_col);
#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    जर (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    शून्य
gotocmdline(पूर्णांक clr)
{
    संदेश_start();
#ifdef FEAT_RIGHTLEFT
    जर (cmdmsg_rl)
	संदेश_col = cmdline_width - 1;
    अन्यथा
#endif
	संदेश_col = 0;	    // always start in column 0
    जर (clr)		    // clear the bottom line(s)
	संदेश_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    स्थिर पूर्णांक
ccheck_abbr(पूर्णांक c)
{
    पूर्णांक spos = 0;

    जर (p_paste || no_abbr)	    // no abbreviations or in paste mode
	परत FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    पर्यंत (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    जर (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    अन्यथा
	// check abbreviation from the beginning of the commandline
	spos = 0;

    परत check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, पूर्णांक what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    पूर्णांक		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    साठी (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	जर ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#अन्यथा
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    जर (what == VSE_SHELL && csh_like_shell() && p != रिक्त)
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
    जर (p != रिक्त && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    परत p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    शून्य
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    जर (p == रिक्त)
	परत;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    शून्य
tilde_replace(
    char_u  *orig_pat,
    पूर्णांक	    अंक_files,
    char_u  **files)
{
    पूर्णांक	    i;
    char_u  *p;

    जर (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	साठी (i = 0; i < अंक_files; ++i)
	{
	    p = home_replace_save(रिक्त, files[i]);
	    जर (p != रिक्त)
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
get_cmdline_info(शून्य)
{
    परत &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    स्थिर cmdline_info_T *
get_ccline_ptr(शून्य)
{
    जर ((State & MODE_CMDLINE) == 0)
	परत रिक्त;
    जर (ccline.cmdbuff != रिक्त)
	परत &ccline;
    जर (prev_ccline_used && prev_ccline.cmdbuff != रिक्त)
	परत &prev_ccline;
    परत रिक्त;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    स्थिर पूर्णांक
get_cmdline_type(शून्य)
{
    cmdline_info_T *p = get_ccline_ptr();

    जर (p == रिक्त)
	परत NUL;
    जर (p->cmdfirstc == NUL)
	परत
#ifdef FEAT_EVAL
	    (p->आदान_fn) ? '@' :
#endif
	    '-';
    परत p->cmdfirstc;
}

#जर defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    स्थिर char_u *
get_cmdline_str(शून्य)
{
    cmdline_info_T *p;

    जर (cmdline_star > 0)
	परत रिक्त;
    p = get_ccline_ptr();
    जर (p == रिक्त)
	परत रिक्त;
    परत vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    स्थिर char_u *
get_cmdline_completion_pattern(शून्य)
{
    cmdline_info_T *p;
    पूर्णांक		xp_context;

    जर (cmdline_star > 0)
	परत रिक्त;

    p = get_ccline_ptr();
    जर (p == रिक्त || p->xpc == रिक्त)
	परत रिक्त;

    xp_context = p->xpc->xp_context;
    जर (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    जर (xp_context == EXPAND_UNSUCCESSFUL)
	परत रिक्त;

    char_u *compl_pat = p->xpc->xp_pattern;

    जर (compl_pat == रिक्त)
	परत रिक्त;

    परत vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    स्थिर char_u *
get_cmdline_completion(शून्य)
{
    cmdline_info_T	*p;
    पूर्णांक			xp_context;

    जर (cmdline_star > 0)
	परत रिक्त;

    p = get_ccline_ptr();
    जर (p == रिक्त || p->xpc == रिक्त)
	परत रिक्त;

    xp_context = p->xpc->xp_context;
    जर (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    जर (xp_context == EXPAND_UNSUCCESSFUL)
	परत रिक्त;

    परत cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    शून्य
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    शून्य
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    शून्य
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    शून्य
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_संख्या = p != रिक्त ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    शून्य
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != रिक्त && p->cmdprompt != रिक्त ?
					    vim_strsave(p->cmdprompt) : रिक्त;
}

/*
 * "getcmdscreenpos()" function
 */
    शून्य
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_संख्या = p != रिक्त ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    शून्य
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    जर (rettv->vval.v_string == रिक्त)
	परत;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    स्थिर पूर्णांक
set_cmdline_str(char_u *str, पूर्णांक pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    पूर्णांक		    len;

    जर (p == रिक्त)
	परत 1;

    len = (पूर्णांक)STRLEN(str);
    जर (realloc_cmdbuff(len + 1) != OK)
	परत 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    परत 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    स्थिर पूर्णांक
set_cmdline_pos(
    पूर्णांक		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    जर (p == रिक्त)
	परत 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    जर (pos < 0)
	new_cmdpos = 0;
    अन्यथा
	new_cmdpos = pos;

    परत 0;
}

// "setcmdline()" function
    शून्य
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक pos = -1;

    जर (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_संख्या_arg(argvars, 1) == FAIL)
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	पूर्णांक error = FALSE;

	pos = (पूर्णांक)tv_get_संख्या_chk(&argvars[1], &error) - 1;
	जर (error)
	    परत;
	जर (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    परत;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_संख्या = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    शून्य
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक		pos;

    जर (in_vim9script() && check_for_संख्या_arg(argvars, 0) == FAIL)
	परत;

    pos = (पूर्णांक)tv_get_संख्या(&argvars[0]) - 1;
    जर (pos >= 0)
	rettv->vval.v_संख्या = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    पूर्णांक
get_cmdline_firstc(शून्य)
{
    परत ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    पूर्णांक
get_list_range(char_u **str, पूर्णांक *num1, पूर्णांक *num2)
{
    पूर्णांक		len;
    पूर्णांक		first = FALSE;
    varnumber_T	अंक;

    *str = skipwhite(*str);
    जर (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, रिक्त, &len, 0, &अंक, रिक्त, 0, FALSE, रिक्त);
	*str += len;
	// overflow
	जर (अंक > पूर्णांक_उच्च)
	    परत FAIL;

	*num1 = (पूर्णांक)अंक;
	first = TRUE;
    }
    *str = skipwhite(*str);
    जर (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, रिक्त, &len, 0, &अंक, रिक्त, 0, FALSE, रिक्त);
	जर (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    जर (अंक > पूर्णांक_उच्च)
		परत FAIL;

	    *num2 = (पूर्णांक)अंक;
	}
	अन्यथा जर (!first)		// no number given at all
	    परत FAIL;
    }
    अन्यथा जर (first)			// only one number given
	*num2 = *num1;
    परत OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    अक्षर *
did_set_cedit(optset_T *args UNUSED)
{
    पूर्णांक n;

    जर (*p_cedit == NUL)
	cedit_किल्ली = -1;
    अन्यथा
    {
	n = string_to_किल्ली(p_cedit, FALSE);
	जर (n == 0 || vim_isprintc(n))
	    परत e_invalid_argument;
	cedit_किल्ली = n;
    }
    परत रिक्त;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    स्थिर पूर्णांक
open_cmdwin(शून्य)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    पूर्णांक			i;
    linenr_T		lnum;
    पूर्णांक			histtype;
    garray_T		winsizes;
    पूर्णांक			save_restart_edit = restart_edit;
    पूर्णांक			save_State = State;
    पूर्णांक			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    पूर्णांक			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    पूर्णांक			save_किल्ली_Typed;
#endif
    पूर्णांक			newbuf_status;
    पूर्णांक			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    जर (text_or_buf_locked()
	    || cmdwin_type != 0
#जर defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	परत K_IGNORE;
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
    जर (win_split((पूर्णांक)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	परत K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    जर (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	परत Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE, ECMD_HIDE,
					रिक्त);
    cmdwin_valid = win_valid(cmdwin_win);
    जर (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	जर (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	जर (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	जर (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(रिक्त, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = रिक्त;
	beep_flush();
	ga_clear(&winsizes);
	परत Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_मूल्य_give_err((char_u *)"bt",
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
    जर (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	जर (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_घाला, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_घाला, TRUE);
	}
	set_option_मूल्य_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    जर (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	जर (i >= 0)
	{
	    lnum = 0;
	    करा
	    {
		जर (++i == get_hislen())
		    i = 0;
		जर (get_histentry(histtype)[i].hisstr != रिक्त)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    पर्यंत (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_ओळ_मोजणी, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_ओळ_abv_curs();
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
    जर (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_किल्ली_Typed = किल्ली_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    किल्ली_Typed = save_किल्ली_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = रिक्त;
    cmdwin_win = रिक्त;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    जर (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    अन्यथा
    {
#जर defined(FEAT_EVAL)
	// autocmds may abort script processing
	जर (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	जर (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    माप_प्रकार  plen = 2;

	    जर (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    जर (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (पूर्णांक)plen;
		ccline.cmdbufflen = (पूर्णांक)(plen + 1);
		cmdwin_result = CAR;
	    }
	    अन्यथा
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	अन्यथा जर (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = रिक्त;
	}
	अन्यथा
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	जर (ccline.cmdbuff == रिक्त)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	अन्यथा
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    जर (ccline.cmdpos == ccline.cmdlen - 1
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
	जर (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	जर (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(रिक्त, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	जर (cmdwin_result == K_IGNORE)
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

    परत cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    पूर्णांक
is_in_cmdwin(शून्य)
{
    परत cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    जर (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == रिक्त)
	परत रिक्त;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    जर (l == रिक्त)
	परत रिक्त;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    परत (char_u *)ga.ga_data;
#अन्यथा
    परत रिक्त;
#endif
}

#जर defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    शून्य
get_user_आदान(
    typval_T	*argvars,
    typval_T	*rettv,
    पूर्णांक		inputdialog,
    पूर्णांक		secret)
{
    char_u	*prompt;
    char_u	*p = रिक्त;
    पूर्णांक		c;
    char_u	buf[NUMBUFLEN];
    पूर्णांक		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    पूर्णांक		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = रिक्त;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    जर (आदान_busy)
	परत;  // this doesn't work recursively.

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	परत;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_आदान
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    जर (no_console_आदान() && !is_not_a_term())
	परत;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    जर (prompt != रिक्त)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	जर (p == रिक्त)
	    p = prompt;
	अन्यथा
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    संदेश_start();
	    संदेश_clr_eos();
	    संदेश_puts_attr((अक्षर *)prompt, get_echo_attr());
	    संदेश_didout = FALSE;
	    संदेश_starthere();
	    *p = c;
	}
	cmdline_row = संदेश_row;

	जर (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    जर (defstr != रिक्त)
		stuffReadbuffSpec(defstr);

	    जर (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		पूर्णांक	xp_namelen;
		दीर्घ	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = रिक्त;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		जर (xp_name == रिक्त)
		    परत;

		xp_namelen = (पूर्णांक)STRLEN(xp_name);

		जर (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    परत;
	    }
	}

	जर (defstr != रिक्त)
	{
	    पूर्णांक save_ex_normal_busy = ex_normal_busy;
	    पूर्णांक save_vgetc_busy = vgetc_busy;
	    पूर्णांक save_आदान_busy = आदान_busy;

	    आदान_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    आदान_busy = save_आदान_busy;
	}
	जर (inputdialog && rettv->vval.v_string == रिक्त
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	संदेश_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    शून्य
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    जर (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	परत;

    पूर्णांक cmd_type = get_cmdline_type();

    जर (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	किल्ली_string[4];

	किल्ली_string[0] = K_SPECIAL;
	किल्ली_string[1] = KS_EXTRA;
	किल्ली_string[2] = KE_WILD;
	किल्ली_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(किल्ली_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
