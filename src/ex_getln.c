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

#شامل "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_کلید	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ایستا cmdline_info_T ccline;

#ifdef FEAT_EVAL
ایستا صحیح	new_cmdpos;	// position set by set_cmdline_pos()
#endif

ایستا صحیح	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ایستا صحیح	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
ایستا صحیح	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

ایستا char_u	*getcmdline_int(صحیح firstc, long شمار, صحیح indent, صحیح clear_ccline);
ایستا صحیح	cmdline_charsize(صحیح idx);
ایستا تهی	set_cmdspos(تهی);
ایستا تهی	set_cmdspos_cursor(تهی);
ایستا تهی	correct_cmdspos(صحیح idx, صحیح cells);
ایستا تهی	dealloc_cmdbuff(تهی);
ایستا تهی	alloc_cmdbuff(size_t len);
ایستا تهی	draw_cmdline(صحیح start, صحیح len);
ایستا تهی	save_cmdline(cmdline_info_T *ccp);
ایستا تهی	restore_cmdline(cmdline_info_T *ccp);
ایستا صحیح	cmdline_paste(صحیح regname, صحیح literally, صحیح remcr);
ایستا تهی	redrawcmdprompt(تهی);
ایستا صحیح	ccheck_abbr(صحیح);
ایستا صحیح	open_cmdwin(تهی);
#ifdef FEAT_جستجو_EXTRA
ایستا صحیح	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

ایستا صحیح	cedit_کلید = -1;	// key value of 'cedit' option

    ایستا تهی
trigger_cmd_autocmd(صحیح typechar, صحیح evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    ایستا تهی
abandon_cmdline(تهی)
{
    dealloc_cmdbuff();
    اگر (پیام_scrolled == 0)
	compute_cmdrow();
    پیام("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_جستجو_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ایستا صحیح
empty_pattern(char_u *p, size_t len, صحیح delim)
{
    magic_T	magic_val = MAGIC_ON;

    اگر (len > 0)
	(تهی) skip_regexp_ex(p, delim, magic_isset(), پوچ, پوچ, &magic_val);
    وگرنه
	بازگشت TRUE;

    بازگشت empty_pattern_magic(p, len, magic_val);
}

    ایستا صحیح
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    وقتی (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != پوچ)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    بازگشت len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
تعریفنوع ساختار {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    صحیح		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    ایستا تهی
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

    ایستا تهی
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
تعریفنوع ساختار {
    pos_T	جستجو_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    صحیح		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    صحیح		did_incsearch;
    صحیح		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ایستا تهی
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->جستجو_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ایستا تهی
set_جستجو_match(pos_T *t)
{
    t->lnum += جستجو_match_سطرها;
    t->col = جستجو_match_endcol;
    اگر (t->lnum > curbuf->b_ml.ml_سطر_شمار)
    {
	t->lnum = curbuf->b_ml.ml_سطر_شمار;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    صحیح
parse_pattern_and_range(
	pos_T	*incsearch_start,
	صحیح	*جستجو_delim,
	صحیح	*skiplen,
	صحیح	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    صحیح		delim_optional = FALSE;
    صحیح		delim;
    صحیح		use_last_pat;
    magic_T     magic = 0;
    نویسه	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    جستجو_first_سطر = 0;
    جستجو_last_سطر = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_سطرها;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, پوچ);

    اگر (vim_strchr((char_u *)"sgvlu", *cmd) == پوچ)
	بازگشت FALSE;

    // Skip over command name to find pattern separator
    برای (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    اگر (*skipwhite(p) == NUL)
	بازگشت FALSE;

    اگر (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	اگر (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	وگرنه اگر (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    وگرنه اگر (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	اگر (*p == '!')
	    p = skipwhite(p + 1);
	وقتی (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	اگر (*p == NUL)
	    بازگشت FALSE;
    }
    وگرنه اگر (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	اگر (*p == '!')
	{
	    p++;
	    اگر (*skipwhite(p) == NUL)
		بازگشت FALSE;
	}
	اگر (*cmd != 'g')
	    delim_optional = TRUE;
    }
    وگرنه
	بازگشت FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *جستجو_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), پوچ, پوچ, &magic);
    use_last_pat = end == p && *end == delim;

    اگر (end == p && !use_last_pat)
	بازگشت FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    اگر (!use_last_pat)
    {
	نویسه c = *end;
	صحیح empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	اگر (empty)
	    بازگشت FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (صحیح)(p - ccline.cmdbuff);
    *patlen = (صحیح)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    اگر (ea.addr_شمار > 0)
    {
	صحیح reverse_match = ea.line2 < ea.line1;
	جستجو_first_سطر = reverse_match ? ea.line2 : ea.line1;
	جستجو_last_سطر = reverse_match ? ea.line1 : ea.line2;
    }
    وگرنه اگر (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	جستجو_first_سطر = جستجو_last_سطر = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    بازگشت TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ایستا صحیح
do_incsearch_highlighting(
	صحیح		    firstc,
	صحیح		    *جستجو_delim,
	incsearch_state_T   *is_state,
	صحیح		    *skiplen,
	صحیح		    *patlen)
{
    صحیح retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    اگر (!p_is || cmd_silent)
	بازگشت FALSE;

    // By default search all lines
    جستجو_first_سطر = 0;
    جستجو_last_سطر = MAXLNUM;

    اگر (firstc == '/' || firstc == '?')
    {
	*جستجو_delim = firstc;
	بازگشت TRUE;
    }

    اگر (firstc != ':')
	بازگشت FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->جستجو_start, جستجو_delim,
	    skiplen, patlen);
    --emsg_off;

    بازگشت retval;
}

    ایستا تهی
finish_incsearch_highlighting(
	صحیح gotesc,
	incsearch_state_T *is_state,
	صحیح call_update_screen)
{
    اگر (!is_state->did_incsearch)
	بازگشت;

    is_state->did_incsearch = FALSE;
    اگر (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    وگرنه
    {
	اگر (!EQUAL_POS(is_state->save_cursor, is_state->جستجو_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->جستجو_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    جستجو_first_سطر = 0;
    جستجو_last_سطر = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    اگر (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ایستا تهی
may_do_incsearch_highlighting(
	صحیح		    firstc,
	long		    شمار,
	incsearch_state_T   *is_state)
{
    صحیح		skiplen, patlen;
    صحیح		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    صحیح		next_char;
    صحیح		use_last_pat;
    صحیح		did_do_incsearch = is_state->did_incsearch;
    صحیح		جستجو_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_جستجو_pattern();

    اگر (!do_incsearch_highlighting(firstc, &جستجو_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_جستجو_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	اگر (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	بازگشت;
    }

    // If there is a character waiting, search and redraw later.
    اگر (char_avail())
    {
	restore_last_جستجو_pattern();
	is_state->incsearch_postponed = TRUE;
	بازگشت;
    }
    is_state->incsearch_postponed = FALSE;

    اگر (جستجو_first_سطر == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->جستجو_start;
    وگرنه اگر (جستجو_first_سطر > curbuf->b_ml.ml_سطر_شمار)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	curwin->w_cursor.col = MAXCOL;
    }
    وگرنه
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = جستجو_first_سطر;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    اگر (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    وگرنه
    {
	صحیح جستجو_flags = جستجو_OPT + جستجو_NOOF + جستجو_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	اگر (!p_hls)
	    جستجو_flags += جستجو_KEEP;
	اگر (جستجو_first_سطر != 0)
	    جستجو_flags += جستجو_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_جستجو(پوچ, firstc == ':' ? '/' : firstc, جستجو_delim,
				 ccline.cmdbuff + skiplen, patlen, شمار, جستجو_flags,
# ifdef FEAT_RELTIME
		&sia
# وگرنه
		پوچ
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	اگر (curwin->w_cursor.lnum < جستجو_first_سطر
		|| curwin->w_cursor.lnum > جستجو_last_سطر)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->جستجو_start;
	}

	// if interrupted while searching, behave like it failed
	اگر (got_int)
	{
	    (تهی)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	وگرنه اگر (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    اگر (found != 0)
	highlight_match = TRUE;		// highlight position
    وگرنه
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    اگر (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_جستجو_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    وگرنه
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    اگر (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	اگر (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, جستجو_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    اگر (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_جستجو_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    اگر (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->جستجو_start;
    وگرنه اگر (found != 0)
	curwin->w_cursor = end_pos;

    پیام_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ایستا صحیح
may_adjust_incsearch_highlighting(
	صحیح			firstc,
	long			شمار,
	incsearch_state_T	*is_state,
	صحیح			c)
{
    صحیح		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = پوچ;
    char_u	*searchstr;
    char_u	*strcopy = پوچ;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    صحیح		جستجو_flags = جستجو_NOOF;
    صحیح		i;
    صحیح		جستجو_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_جستجو_pattern();

    اگر (!do_incsearch_highlighting(firstc, &جستجو_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_جستجو_pattern();
	بازگشت OK;
    }
    اگر (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_جستجو_pattern();
	بازگشت FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (تهی)parse_جستجو_pattern_offset(&pat, &patlen_s, جستجو_delim,
				    جستجو_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    اگر (c == Ctrl_G)
    {
	t = is_state->match_end;
	اگر (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (تهی)decl(&t);
	جستجو_flags += جستجو_COL;
    }
    وگرنه
	t = is_state->match_start;
    اگر (!p_hls)
	جستجو_flags += جستجو_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, پوچ,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, شمار, جستجو_flags, RE_جستجو, پوچ);
    --emsg_off;
    اگر (dircp != پوچ)
	*dircp = جستجو_delim;
    اگر (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->جستجو_start = match_start;
	اگر (!offset.سطر && (offset.end || off != 0))
	{
	    اگر (offset.end)
	    {
		is_state->جستجو_start = match_end;
		(تهی)decl(&is_state->جستجو_start);
	    }
	    وقتی (off > 0)
	    {
		اگر (incl(&is_state->جستجو_start) == -1)
		    بشکن;
		--off;
	    }
	    وقتی (off < 0)
	    {
		اگر (decl(&is_state->جستجو_start) == -1)
		    بشکن;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	اگر (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->جستجو_start = t;
	    (تهی)decl(&is_state->جستجو_start);
	}
	وگرنه اگر (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->جستجو_start = t;
	    (تهی)incl(&is_state->جستجو_start);
	}
	اگر (LT_POS(t, is_state->جستجو_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->جستجو_start = t;
	    اگر (firstc == '?')
		(تهی)incl(&is_state->جستجو_start);
	    وگرنه
		(تهی)decl(&is_state->جستجو_start);
	}

	set_جستجو_match(&is_state->match_end);
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
    وگرنه
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_جستجو_pattern();
    بازگشت FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ایستا صحیح
may_add_char_to_جستجو(صحیح firstc, صحیح *c, incsearch_state_T *is_state)
{
    صحیح		skiplen, patlen, جستجو_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_جستجو_pattern();

    اگر (!do_incsearch_highlighting(firstc, &جستجو_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_جستجو_pattern();
	بازگشت FAIL;
    }
    restore_last_جستجو_pattern();

    // Add a character from under the cursor for 'incsearch'.
    اگر (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	اگر (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    اگر (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    اگر (*c == جستجو_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != پوچ)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    اگر (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		صحیح save_c = *c;

		وقتی (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ایستا صحیح
cmdline_has_arabic(صحیح start, صحیح len)
{
    صحیح	    j;
    صحیح	    mb_l;
    صحیح	    u8c;
    char_u  *p;
    صحیح	    u8cc[MAX_MCO];

    اگر (!enc_utf8)
	بازگشت FALSE;

    برای (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	اگر (ARABIC_CHAR(u8c))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

    تهی
cmdline_init(تهی)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ایستا صحیح
cmdline_handle_ctrl_bsl(صحیح c, صحیح *gotesc)
{
    ++no_mapping;
    ++allow_کلیدها;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدها;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    اگر (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && کلید_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	بازگشت PROCESS_NEXT_کلید;
    }

#ifdef FEAT_EVAL
    اگر (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	اگر (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	وگرنه
	    new_cmdpos = ccline.cmdpos;

	c = get_عبارت_register();
	اگر (c == '=')
	{
	    char_u	*p = پوچ;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_عبارت_سطر();
	    --textlock;

	    اگر (p != پوچ)
	    {
		صحیح len = (صحیح)STRLEN(p);

		اگر (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    اگر (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    وگرنه
			ccline.cmdpos = new_cmdpos;

		    کلید_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    بازگشت CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	بازگشت CMDLINE_NOT_CHANGED;
    }
#endif

    اگر (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    بازگشت GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ایستا صحیح
cmdline_wildchar_complete(
	صحیح		c,
	صحیح		escape,
	صحیح		*did_wild_list,
	صحیح		*wim_index_p,
	expand_T	*xp,
	صحیح		*gotesc,
	صحیح		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    صحیح		wim_index = *wim_index_p;
    صحیح		res;
    صحیح		cmdpos_before;
    صحیح		options = WILD_NO_BEEP;
    صحیح		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    صحیح		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    اگر (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    اگر (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	اگر (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (تهی)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	اگر (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	وگرنه اگر (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	وگرنه
	    res = OK;	    // don't insert 'wildchar' now
    }
    وگرنه		    // typed p_wc first time
    {
	صحیح wim_longest = (wim_flags[0] & WIM_LONGEST);
	صحیح wim_list = (wim_flags[0] & WIM_LIST);
	صحیح wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	اگر (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    اگر (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    اگر (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    وگرنه
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	اگر (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	وگرنه
	{
	    اگر (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    اگر (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	اگر (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	اگر (got_int)
	{
	    (تهی)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (تهی)ExpandOne(xp, پوچ, پوچ, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    بازگشت CMDLINE_CHANGED;
	}

	// Display matches
	اگر (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    اگر (wim_longest)
	    {
		صحیح found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		صحیح show_menu = p_wmnu && wim_full;

		اگر (wim_list || show_menu)
		    (تهی)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		وگرنه اگر (!found_longest_prefix)
		{
		    صحیح wim_list_next = (wim_flags[1] & WIM_LIST);
		    صحیح wim_full_next = (wim_flags[1] & WIM_FULL);
		    صحیح wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    صحیح wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    اگر (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			اگر (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			وگرنه
			    (تهی)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			اگر (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    وگرنه
	    {
		صحیح show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		اگر (wim_list || show_menu)
		    (تهی)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		وگرنه
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    اگر (wim_list)
		*did_wild_list = TRUE;
	}
	وگرنه اگر (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    اگر (wim_index < 3)
	++wim_index;
    اگر (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    بازگشت (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ایستا صحیح
cmdline_erase_chars(
	صحیح c,
	صحیح indent
#ifdef FEAT_جستجو_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    صحیح		i;
    صحیح		j;

    اگر (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    اگر (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    اگر (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    اگر (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	اگر (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    اگر (c == Ctrl_W)
	    {
		وقتی (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		وقتی (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		اگر (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	وگرنه اگر (c == Ctrl_W)
	{
	    وقتی (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    اگر (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		وقتی (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	وگرنه
	    --p;
	ccline.cmdpos = (صحیح)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	وقتی (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_جستجو_EXTRA
	اگر (ccline.cmdlen == 0)
	{
	    isp->جستجو_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    وگرنه اگر (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == پوچ && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	اگر (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    بازگشت CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	اگر (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    اگر (cmdmsg_rl)
		پیام_col = cmdline_width;
	    وگرنه
#endif
		پیام_col = 0;
	    پیام_putchar(' ');		// delete ':'
	}
#ifdef FEAT_جستجو_EXTRA
	isp->جستجو_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	بازگشت GOTO_NORMAL_MODE;
    }
    بازگشت CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ایستا تهی
cmdline_toggle_langmap(long *b_im_ptr)
{
    اگر (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_درون_داد_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	اگر (b_im_ptr != پوچ)
	{
	    اگر (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    وگرنه
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_درون_داد_METHOD
    وگرنه
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	اگر ((p_imdisable && b_im_ptr != پوچ)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    اگر (b_im_ptr != پوچ)
		*b_im_ptr = B_IMODE_NONE;
	}
	وگرنه
	{
	    im_set_active(TRUE);	// Enable input method
	    اگر (b_im_ptr != پوچ)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    اگر (b_im_ptr != پوچ)
    {
	اگر (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	وگرنه
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#اگر defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ایستا صحیح
cmdline_درج_reg(صحیح *gotesc UNUSED)
{
    صحیح		i;
    صحیح		c;
    صحیح		literally = FALSE;
#ifdef FEAT_EVAL
    صحیح		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_کلیدها;
    i = c = plain_vgetc();	// CTRL-R <char>
    اگر (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    اگر (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_کلیدها;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    اگر (c == '=')
    {
	اگر (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	وگرنه
	    c = get_عبارت_register();
    }
#endif
    اگر (c != ESC)	    // use ESC to cancel inserting register
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
	اگر (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    بازگشت GOTO_NORMAL_MODE;
	}
#endif
	کلید_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	اگر (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    اگر (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    وگرنه
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
    بازگشت literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ایستا تهی
cmdline_چپ_راست_mouse(صحیح c, صحیح *ignore_drag_release)
{
    اگر (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    وگرنه
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    اگر (!gui.in_use)
#endif
	اگر (!mouse_has(MOUSE_COMMAND))
	    بازگشت;
#ifdef FEAT_CLIPBOARD
    اگر (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	صحیح	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	اگر (mouse_model_popup() && button == MOUSE_چپ
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_راست;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	بازگشت;
    }
#endif

    set_cmdspos();
    برای (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	صحیح	i;

	i = cmdline_charsize(ccline.cmdpos);
	اگر (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    بشکن;
	اگر (has_mbyte)
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
    ایستا صحیح
cmdline_browse_history(
	صحیح	c,
	صحیح	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	صحیح	histype,
	صحیح	*hiscnt_p,
	expand_T *xp)
{
    صحیح		orig_hiscnt;
    صحیح		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    صحیح		res;

    اگر (get_hislen() == 0 || firstc == NUL)	// no history
	بازگشت CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    اگر (lookfor == پوچ)
    {
	اگر ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == پوچ)
	    بازگشت CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    برای (;;)
    {
	// one step backwards
	اگر (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    اگر (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    وگرنه اگر (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    وگرنه اگر (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    وگرنه			// at top of list
	    {
		hiscnt = orig_hiscnt;
		بشکن;
	    }
	}
	وگرنه    // one step forwards
	{
	    // on last entry, clear the line
	    اگر (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		بشکن;
	    }

	    // not on a history line, nothing to do
	    اگر (hiscnt == get_hislen())
		بشکن;
	    اگر (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    وگرنه
		++hiscnt;
	}
	اگر (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== پوچ)
	{
	    hiscnt = orig_hiscnt;
	    بشکن;
	}
	اگر ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    بشکن;
    }

    اگر (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	صحیح	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	اگر (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	وگرنه
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	اگر (histype == HIST_جستجو
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    صحیح     i;
	    صحیح     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    برای (i = 0; i <= 1; ++i)
	    {
		len = 0;
		برای (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    اگر (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			اگر (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    وگرنه
		    {
			// Escape new sep, unless it is already
			// escaped.
			اگر (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    اگر (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			اگر (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		اگر (i == 0)
		{
		    alloc_cmdbuff(len);
		    اگر (ccline.cmdbuff == پوچ)
		    {
			res = GOTO_NORMAL_MODE;
			برو done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (صحیح)len;
	}
	وگرنه
	{
	    alloc_cmdbuff(plen);
	    اگر (ccline.cmdbuff == پوچ)
	    {
		res = GOTO_NORMAL_MODE;
		برو done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (صحیح)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	برو done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    بازگشت res;
}

/*
 * Initialize the current command-line info.
 */
    ایستا صحیح
init_ccline(صحیح firstc, صحیح indent)
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
    اگر (ccline.cmdbuff == پوچ)
    {
	ccline.cmdbufflen = 0;
	بازگشت FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    اگر (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    بازگشت OK;
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
    صحیح		  firstc,
    long	  شمار,	// only used for incremental search
    صحیح		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    بازگشت getcmdline_int(firstc, شمار, indent, TRUE);
}

    ایستا char_u *
getcmdline_int(
    صحیح		firstc,
    long	شمار UNUSED,	// only used for incremental search
    صحیح		indent,		// indent for inside conditionals
    صحیح		clear_ccline)	// clear ccline first
{
    ایستا صحیح	depth = 0;	    // call depth
    صحیح		c = 0;
    صحیح		i;
    صحیح		j;
    صحیح		gotesc = FALSE;		// TRUE when <ESC> just typed
    صحیح		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = پوچ;	// string to match
    size_t	lookforlen = 0;
    صحیح		hiscnt;			// current history line in use
    صحیح		histype;		// history type to be used
#ifdef FEAT_جستجو_EXTRA
    incsearch_state_T	is_state;
#endif
    صحیح		did_wild_list = FALSE;	// did wild_list() recently
    صحیح		wim_index = 0;		// index in wim_flags[]
    صحیح		res;
    صحیح		save_پیام_scroll = پیام_scroll;
    صحیح		save_State = State;	// remember State when called
    صحیح		some_کلید_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    صحیح		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    صحیح		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = پوچ;
    buf_T	*b_im_ptr_buf = پوچ;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    صحیح		did_save_ccline = FALSE;
    صحیح		cmdline_type;
    صحیح		wild_type = 0;
    صحیح		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = پوچ;
    صحیح		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    اگر (ccline.cmdbuff != پوچ)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    اگر (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    اگر (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    اگر (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_جستجو_EXTRA
    init_incsearch_state(&is_state);
#endif

    اگر (init_ccline(firstc, indent) != OK)
	برو theend;	// out of memory

    اگر (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	برو theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    وگرنه
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    اگر (!cmd_silent)
    {
	i = پیام_scrolled;
	پیام_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	پیام_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#اگر defined(FEAT_EVAL)
    اگر (ccline.درون_داد_fn)
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
    پیام_scroll = FALSE;

    State = MODE_CMDLINE;

    اگر (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	اگر (curbuf->b_p_imsearch == B_IMODE_USE_درج)
	    b_im_ptr = &curbuf->b_p_iminsert;
	وگرنه
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	اگر (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_درون_داد_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_درون_داد_METHOD
    وگرنه اگر (p_imcmdline)
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
    اگر (!debug_mode)
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
    اگر (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    اگر (!cmd_silent && پیام_scrolled == 0)
    {
	صحیح	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    اگر (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	اگر (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	اگر (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    برای (;;)
    {
	صحیح	trigger_cmdlinechanged = TRUE;
	صحیح	end_wildmenu;
	صحیح	prev_cmdpos = ccline.cmdpos;
	صحیح	skip_pum_redraw = FALSE;

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

	اگر (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_کلید_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	اگر (ccline.cmdbuff != پوچ)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    اگر (prev_cmdbuff == پوچ)
		برو returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	اگر (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	انجامبده
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} وقتی (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	اگر (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    اگر (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (تهی)ExpandOne(&xpc, پوچ, پوچ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	اگر (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    ادامه;
	}

	اگر (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    صحیح	    clen = ccline.cmdlen;
	    صحیح	    cc_شمار = aucmd_cmdline_changed_شمار;

	    اگر (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		اگر (clen == ccline.cmdlen
				    || cc_شمار != aucmd_cmdline_changed_شمار)
		    trigger_cmdlinechanged = FALSE;
		برو cmdline_changed;
	    }
	}

	اگر (کلید_Typed)
	{
	    some_کلید_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    اگر (cmd_hkmap)
		c = hkmap(c);
	    اگر (cmdmsg_rl && !کلید_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		انتخاب (c)
		{
		    حالت K_راست:   c = K_چپ; بشکن;
		    حالت K_S_راست: c = K_S_چپ; بشکن;
		    حالت K_C_راست: c = K_C_چپ; بشکن;
		    حالت K_چپ:    c = K_راست; بشکن;
		    حالت K_S_چپ:  c = K_S_راست; بشکن;
		    حالت K_C_چپ:  c = K_C_راست; بشکن;
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
	اگر ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#اگر defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	اگر (lookfor != پوچ
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_چپ && c != K_راست
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	اگر (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	اگر (p_wmnu)
	    c = wildmenu_translate_کلید(&ccline, c, &xpc, did_wild_list);

	صحیح کلید_is_wc = (c == p_wc && کلید_Typed) || c == p_wcm;
	اگر ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !کلید_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    اگر (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		اگر (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    بشکن;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	اگر (کلید_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#اگر defined(FEAT_جستجو_EXTRA)
	    اگر ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!کلید_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	اگر (end_wildmenu)
	{
	    اگر (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !کلید_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    اگر (xpc.xp_numfiles != -1)
		(تهی)ExpandOne(&xpc, پوچ, پوچ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    اگر (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	اگر (p_wmnu)
	    c = wildmenu_process_کلید(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	اگر (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    اگر (res == CMDLINE_CHANGED)
		برو cmdline_changed;
	    وگرنه اگر (res == CMDLINE_NOT_CHANGED)
		برو cmdline_not_changed;
	    وگرنه اگر (res == GOTO_NORMAL_MODE)
		برو returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	اگر (c == cedit_کلید || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    اگر ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_کلید_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	وگرنه
	    c = do_digraph(c);
#endif

	اگر (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!کلید_Typed || vim_strchr(p_cpo, CPO_ESC) != پوچ)))
	{
	    // In Ex mode a backslash escapes a newline.
	    اگر (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		اگر (c == K_KENTER)
		    c = '\n';
	    }
	    وگرنه
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		اگر (ccheck_abbr(c + ABBR_OFF))
		    برو cmdline_changed;
		اگر (!cmd_silent)
		{
		    windgoto(پیام_row, cmdline_col_off);
		    out_flush();
		}
		بشکن;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	اگر ((c == p_wc && !gotesc && کلید_Typed) || c == p_wcm || c == K_WILD)
	{
	    اگر (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_جستجو_EXTRA
		    &is_state.جستجو_start
#وگرنه
		    پوچ
#endif
		    );
	    اگر (c == K_WILD)
		--emsg_silent;
	    اگر (res == CMDLINE_CHANGED)
		برو cmdline_changed;
	    اگر (c == K_WILD)
		برو cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	اگر (c == K_S_TAB && کلید_Typed)
	{
	    اگر (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		اگر (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    صحیح list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		اگر (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    برو cmdline_changed;
	    }
	}

	اگر (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	اگر (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_جستجو_EXTRA
	    // Apply search highlighting
	    اگر (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    اگر (کلید_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, شمار, &is_state);
#endif
	    wild_type = 0;
	    برو cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	انتخاب (c)
	{
	حالت K_BS:
	حالت Ctrl_H:
	حالت K_DEL:
	حالت K_KDEL:
	حالت Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_جستجو_EXTRA
		    , &is_state
#endif
		    );
	    اگر (res == CMDLINE_NOT_CHANGED)
		برو cmdline_not_changed;
	    وگرنه اگر (res == GOTO_NORMAL_MODE)
		برو returncmd;		// back to cmd mode
	    برو cmdline_changed;

	حالت K_INS:
	حالت K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		برو cmdline_not_changed;

	حالت Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : پوچ);
		برو cmdline_not_changed;

//	case '@':   only in very old vi
	حالت Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		وقتی (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_جستجو_EXTRA
		اگر (ccline.cmdlen == 0)
		    is_state.جستجو_start = is_state.save_cursor;
#endif
		redrawcmd();
		برو cmdline_changed;

#ifdef FEAT_CLIPBOARD
	حالت Ctrl_Y:
		// Copy the modeless selection, if there is one.
		اگر (clip_star.state != SELECT_CLEARED)
		{
		    اگر (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    برو cmdline_not_changed;
		}
		بشکن;
#endif

	حالت ESC:	// get here if p_wc != ESC or when ESC typed twice
	حالت Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		اگر (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    برو cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		برو returncmd;		// back to cmd mode

	حالت Ctrl_R:			// insert register
		res = cmdline_درج_reg(&gotesc);
		اگر (res == GOTO_NORMAL_MODE)
		    برو returncmd;
		اگر (res == CMDLINE_CHANGED)
		    برو cmdline_changed;
		برو cmdline_not_changed;

	حالت Ctrl_D:
		اگر (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    بشکن;	// Use ^D as normal char instead

		redrawcmd();
		ادامه;	// don't do incremental search now

	حالت K_راست:
	حالت K_S_راست:
	حالت K_C_راست:
		انجامبده
		{
		    اگر (ccline.cmdpos >= ccline.cmdlen)
			بشکن;
		    i = cmdline_charsize(ccline.cmdpos);
		    اگر (کلید_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			بشکن;
		    ccline.cmdspos += i;
		    اگر (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    وگرنه
			++ccline.cmdpos;
		}
		وقتی ((c == K_S_راست || c == K_C_راست
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		اگر (has_mbyte)
		    set_cmdspos_cursor();
		برو cmdline_not_changed;

	حالت K_چپ:
	حالت K_S_چپ:
	حالت K_C_چپ:
		اگر (ccline.cmdpos == 0)
		    برو cmdline_not_changed;
		انجامبده
		{
		    --ccline.cmdpos;
		    اگر (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		وقتی (ccline.cmdpos > 0
			&& (c == K_S_چپ || c == K_C_چپ
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		اگر (has_mbyte)
		    set_cmdspos_cursor();
		برو cmdline_not_changed;

	حالت K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		برو cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالت K_F4:
	    اگر (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		برو cmdline_not_changed;
	    }
	    بشکن;
#endif

	حالت K_MIDDLEDRAG:
	حالت K_MIDDLERELEASE:
		برو cmdline_not_changed;	// Ignore mouse

	حالت K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		اگر (!gui.in_use)
#endif
		    اگر (!mouse_has(MOUSE_COMMAND))
			برو cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		اگر (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		وگرنه
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		برو cmdline_changed;

#ifdef FEAT_DND
	حالت K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		برو cmdline_changed;
#endif

	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		اگر (ignore_drag_release)
		    برو cmdline_not_changed;
		// FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_RIGHTMOUSE:
		cmdline_چپ_راست_mouse(c, &ignore_drag_release);
		برو cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	حالت K_MOUSEDOWN:
	حالت K_MOUSEUP:
	حالت K_MOUSELEFT:
	حالت K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		اگر (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_چپ
				      : MSCR_راست);
#endif
		برو cmdline_not_changed;

	// Alternate buttons ignored here
	حالت K_X1MOUSE:
	حالت K_X1DRAG:
	حالت K_X1RELEASE:
	حالت K_X2MOUSE:
	حالت K_X2DRAG:
	حالت K_X2RELEASE:
	حالت K_MOUSEMOVE:
		برو cmdline_not_changed;

#ifdef FEAT_GUI
	حالت K_LEFTMOUSE_NM:	// mousefocus click, ignored
	حالت K_LEFTRELEASE_NM:
		برو cmdline_not_changed;

	حالت K_VER_SCROLLBAR:
		اگر (پیام_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		برو cmdline_not_changed;

	حالت K_HOR_SCROLLBAR:
		اگر (پیام_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_مقدار);
		    redrawcmd();
		}
		برو cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	حالت K_TABLINE:
	حالت K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		اگر (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		برو cmdline_not_changed;
#endif

	حالت K_SELECT:	    // end of Select mode mapping - ignore
		برو cmdline_not_changed;

	حالت Ctrl_B:	    // begin of command line
	حالت K_HOME:
	حالت K_KHOME:
	حالت K_S_HOME:
	حالت K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		برو cmdline_not_changed;

	حالت Ctrl_E:	    // end of command line
	حالت K_END:
	حالت K_KEND:
	حالت K_S_END:
	حالت K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		برو cmdline_not_changed;

	حالت Ctrl_A:	    // all matches
		اگر (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		اگر (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    بشکن;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		برو cmdline_changed;

	حالت Ctrl_L:
#ifdef FEAT_جستجو_EXTRA
		اگر (may_add_char_to_جستجو(firstc, &c, &is_state) == OK)
		    برو cmdline_not_changed;
#endif

		// completion: longest common part
		اگر (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    بشکن;
		برو cmdline_changed;

	حالت Ctrl_N:	    // next match
	حالت Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		اگر (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    اگر (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    برو cmdline_not_changed;
		}
#endif
		اگر (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    اگر (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			بشکن;
		    برو cmdline_changed;
		}
		// FALLTHROUGH
	حالت K_UP:
	حالت K_DOWN:
	حالت K_S_UP:
	حالت K_S_DOWN:
	حالت K_PAGEUP:
	حالت K_KPAGEUP:
	حالت K_PAGEDOWN:
	حالت K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		اگر (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    صحیح	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    اگر (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    برو cmdline_not_changed;
		}
#endif
		اگر (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    اگر (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    اگر (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			بشکن;
		    برو cmdline_changed;
		}
		وگرنه
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    اگر (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			برو cmdline_changed;
		    }
		    وگرنه اگر (res == GOTO_NORMAL_MODE)
			برو returncmd;
		}
		برو cmdline_not_changed;

#ifdef FEAT_جستجو_EXTRA
	حالت Ctrl_G:	    // next match
	حالت Ctrl_T:	    // previous match
		اگر (may_adjust_incsearch_highlighting(
					  firstc, شمار, &is_state, c) == FAIL)
		    برو cmdline_not_changed;
		بشکن;
#endif

	حالت Ctrl_V:
	حالت Ctrl_Q:
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
		    اگر (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			پیام_putchar(' ');
			cursorcmd();
		    }
		}

		بشکن;

#ifdef FEAT_DIGRAPHS
	حالت Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		اگر (c != NUL)
		    بشکن;

		redrawcmd();
		برو cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	حالت Ctrl__:	    // CTRL-_: switch language mode
		اگر (!p_ari)
		    بشکن;
		cmd_hkmap = !cmd_hkmap;
		برو cmdline_not_changed;
#endif

	حالت K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, پوچ);
		برو cmdline_changed;

	پیشفرض:
#ifdef UNIX
		اگر (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    برو returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		اگر (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		بشکن;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	اگر (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    برو cmdline_changed;

	/*
	 * put the character in the command line
	 */
	اگر (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_کلید_name(c, mod_mask), -1, TRUE);
	وگرنه
	{
	    اگر (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    وگرنه
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	برو cmdline_changed;

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
	اگر (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_جستجو_EXTRA
	اگر (!is_state.incsearch_postponed)
	    ادامه;
#endif

cmdline_changed:
#ifdef FEAT_جستجو_EXTRA
	// If the window changed incremental search state is not valid.
	اگر (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	اگر (xpc.xp_context == EXPAND_NOTHING && (کلید_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, شمار, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	اگر (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != پوچ &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    اگر (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	اگر (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	اگر (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    اگر (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    اگر (!event_cmdlineleavepre_triggered)
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
    اگر (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = پوچ;
    clear_cmdline_orig();

#ifdef FEAT_جستجو_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    اگر (ccline.cmdbuff != پوچ)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	اگر (ccline.cmdlen && firstc != NUL
		&& (some_کلید_typed || histype == HIST_جستجو))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_جستجو ? firstc : NUL);
	    اگر (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	اگر (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    پیام_check();
    پیام_scroll = save_پیام_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    اگر (some_کلید_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    اگر (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_درون_داد_METHOD
    اگر (b_im_ptr != پوچ && buf_valid(b_im_ptr_buf)
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
	اگر (did_save_ccline)
	    restore_cmdline(&save_ccline);
	وگرنه
	    ccline.cmdbuff = پوچ;

	vim_free(prev_cmdbuff);
	بازگشت p;
    }
}

#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    صحیح		firstc,
    char_u	*prompt,	// command line prompt
    صحیح		attr,		// attributes for prompt
    صحیح		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    صحیح			did_save_ccline = FALSE;
    صحیح			پیام_col_save = پیام_col;
    صحیح			پیام_silent_save = پیام_silent;

    اگر (ccline.cmdbuff != پوچ)
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
    ccline.درون_داد_fn = (firstc == '@');
# endif
    پیام_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    اگر (did_save_ccline)
	restore_cmdline(&save_ccline);

    پیام_silent = پیام_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    اگر (ccline.cmdbuff != پوچ)
	پیام_col = پیام_col_save;

    بازگشت s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    صحیح
check_opt_wim(تهی)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    صحیح		i;
    صحیح		idx = 0;

    برای (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    برای (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	برای (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	اگر (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    بازگشت FAIL;
	اگر (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	وگرنه اگر (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	وگرنه اگر (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	وگرنه اگر (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	وگرنه اگر (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	وگرنه اگر (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	وگرنه
	    بازگشت FAIL;
	p += i;
	اگر (*p == NUL)
	    بشکن;
	اگر (*p == ',')
	{
	    اگر (idx == 3)
		بازگشت FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    وقتی (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    برای (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    بازگشت OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    صحیح
text_locked(تهی)
{
    اگر (cmdwin_type != 0)
	بازگشت TRUE;
    بازگشت textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    تهی
text_locked_پیام(تهی)
{
    emsg(_(get_text_locked_پیام()));
}

    نویسه *
get_text_locked_پیام(تهی)
{
    اگر (cmdwin_type != 0)
	بازگشت e_invalid_in_cmdline_window;
    بازگشت e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    صحیح
text_or_buf_locked(تهی)
{
    اگر (text_locked())
    {
	text_locked_پیام();
	بازگشت TRUE;
    }
    بازگشت curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    صحیح
curbuf_locked(تهی)
{
    اگر (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	بازگشت TRUE;
    }
    بازگشت allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    صحیح
allbuf_locked(تهی)
{
    اگر (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

    ایستا صحیح
cmdline_charsize(صحیح idx)
{
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    اگر (cmdline_star > 0)	    // showing '*', always 1 position
	بازگشت 1;
#endif
    بازگشت ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ایستا تهی
set_cmdspos(تهی)
{
    اگر (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    وگرنه
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ایستا تهی
set_cmdspos_cursor(تهی)
{
    صحیح		i, m, c;

    set_cmdspos();
    اگر (کلید_Typed)
    {
	m = cmdline_width * Rows;
	اگر (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    وگرنه
	m = MAXCOL;
    برای (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	اگر (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	اگر ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    بشکن;
	}
	اگر (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ایستا تهی
correct_cmdspos(صحیح idx, صحیح cells)
{
    اگر ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    صحیح		c,		// normally ':', NUL for ":append"
    تهی	*cookie UNUSED,
    صحیح		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    اگر (exec_from_reg && vpeekc() == ':')
	(تهی)vgetc();
    بازگشت getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    صحیح		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    تهی	*cookie UNUSED,
    صحیح		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	سطر_ga;
    char_u	*pend;
    صحیح		startcol = 0;
    صحیح		c1 = 0;
    صحیح		escaped = FALSE;	// CTRL-V typed
    صحیح		vcol = 0;
    char_u	*p;
    صحیح		prev_char;
    صحیح		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    اگر ((پیام_col || پیام_didout) && promptc != '?')
	پیام_putchar('\n');
    اگر (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	اگر (p_prompt)
	    پیام_putchar(':');
	وقتی (indent-- > 0)
	    پیام_putchar(' ');
	startcol = پیام_col;
    }

    ga_init2(&سطر_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    اگر (promptc <= 0)
    {
	vcol = indent;
	وقتی (indent >= 8)
	{
	    ga_append(&سطر_ga, TAB);
	    پیام_puts("        ");
	    indent -= 8;
	}
	وقتی (indent-- > 0)
	{
	    ga_append(&سطر_ga, ' ');
	    پیام_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_کلیدها;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    وقتی (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	اگر (ga_grow(&سطر_ga, 40) == FAIL)
	    بشکن;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	اگر (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	وگرنه
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	اگر (got_int)
	{
	    پیام_putchar('\n');
	    بشکن;
	}

	اگر (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &سطر_ga);
	    برو redraw;
	}

	اگر (!escaped)
	{
	    // CR typed means "enter", which is NL
	    اگر (c1 == '\r')
		c1 = '\n';

	    اگر (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		اگر (سطر_ga.ga_len > 0)
		{
		    اگر (has_mbyte)
		    {
			p = (char_u *)سطر_ga.ga_data;
			p[سطر_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + سطر_ga.ga_len - 1) + 1;
			سطر_ga.ga_len -= len;
		    }
		    وگرنه
			--سطر_ga.ga_len;
		    برو redraw;
		}
		ادامه;
	    }

	    اگر (c1 == Ctrl_U)
	    {
		پیام_col = startcol;
		پیام_clr_eos();
		سطر_ga.ga_len = 0;
		برو redraw;
	    }

	    اگر (c1 == Ctrl_T)
	    {
		sw = get_sw_مقدار(curbuf);
		p = (char_u *)سطر_ga.ga_data;
		p[سطر_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		وقتی (get_indent_str(p, 8, FALSE) < indent)
		{
		    (تهی)ga_grow(&سطر_ga, 2);  // one more for the NUL
		    p = (char_u *)سطر_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, سطر_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++سطر_ga.ga_len;
		}
redraw:
		// redraw the line
		پیام_col = startcol;
		vcol = 0;
		p = (char_u *)سطر_ga.ga_data;
		p[سطر_ga.ga_len] = NUL;
		وقتی (p < (char_u *)سطر_ga.ga_data + سطر_ga.ga_len)
		{
		    اگر (*p == TAB)
		    {
			انجامبده
			    پیام_putchar(' ');
			وقتی (++vcol % 8);
			++p;
		    }
		    وگرنه
		    {
			len = mb_ptr2len(p);
			پیام_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		پیام_clr_eos();
		windgoto(پیام_row, cmdline_col_off + پیام_col);
		ادامه;
	    }

	    اگر (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)سطر_ga.ga_data;
		اگر (prev_char == '0' || prev_char == '^')
		{
		    اگر (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--سطر_ga.ga_len] = NUL;
		}
		وگرنه
		{
		    p[سطر_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    اگر (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_مقدار(curbuf);
		    }
		}
		وقتی (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, سطر_ga.ga_len - (s - p) + 1);
		    --سطر_ga.ga_len;
		}
		برو add_indent;
	    }

	    اگر (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		ادامه;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    اگر (IS_SPECIAL(c1))
		ادامه;
	}

	اگر (IS_SPECIAL(c1))
	    c1 = '?';
	اگر (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)سطر_ga.ga_data + سطر_ga.ga_len);
	وگرنه
	{
	    len = 1;
	    ((char_u *)سطر_ga.ga_data)[سطر_ga.ga_len] = c1;
	}
	اگر (c1 == '\n')
	    پیام_putchar('\n');
	وگرنه اگر (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    انجامبده
		پیام_putchar(' ');
	    وقتی (++vcol % 8);
	}
	وگرنه
	{
	    پیام_outtrans_len(
		     ((char_u *)سطر_ga.ga_data) + سطر_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	سطر_ga.ga_len += len;
	escaped = FALSE;

	windgoto(پیام_row, cmdline_col_off + پیام_col);
	pend = (char_u *)(سطر_ga.ga_data) + سطر_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	اگر (سطر_ga.ga_len > 0 && pend[-1] == '\n'
		&& (سطر_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --سطر_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    بشکن;
	}
    }

    --no_mapping;
    --allow_کلیدها;

    // make following messages go to the next line
    پیام_didout = FALSE;
    پیام_col = 0;
    اگر (پیام_row < Rows - 1)
	++پیام_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    اگر (got_int)
	ga_clear(&سطر_ga);

    بازگشت (char_u *)سطر_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    صحیح
cmdline_overstrike(تهی)
{
    بازگشت ccline.overstrike;
}

#اگر defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    صحیح
cmdline_at_end(تهی)
{
    بازگشت (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#اگر defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(تهی)
{
    اگر (ccline.cmdbuff == پوچ || ccline.cmdpos > ccline.cmdlen)
	بازگشت MAXCOL;

    اگر (has_mbyte)
    {
	colnr_T	col;
	صحیح	i = 0;

	برای (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	بازگشت col;
    }
    وگرنه
	بازگشت ccline.cmdpos;
}
#endif

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ایستا تهی
redrawcmd_preedit(تهی)
{
    اگر ((State & MODE_CMDLINE)
	    && xic != پوچ
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	صحیح	cmdpos = 0;
	صحیح	cmdspos;
	صحیح	old_row;
	صحیح	old_col;
	colnr_T	col;

	old_row = پیام_row;
	old_col = پیام_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	اگر (has_mbyte)
	{
	    برای (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	وگرنه
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	پیام_row = cmdline_row + (cmdspos / cmdline_width);
	پیام_col = cmdspos % cmdline_width;
	اگر (پیام_row >= Rows)
	    پیام_row = Rows - 1;

	برای (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    صحیح char_len;
	    صحیح char_attr;

	    char_attr = im_get_feedback_attr(col);
	    اگر (char_attr < 0)
		بشکن; // end of preedit string

	    اگر (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    وگرنه
		char_len = 1;

	    پیام_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	پیام_row = old_row;
	پیام_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ایستا تهی
dealloc_cmdbuff(تهی)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ایستا تهی
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    اگر (len < 80)
	len = 100;
    وگرنه
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (صحیح)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    صحیح
realloc_cmdbuff(صحیح len)
{
    char_u	*p;
    صحیح		plen;

    اگر (len < ccline.cmdbufflen)
	بازگشت OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    اگر (ccline.cmdbuff == پوچ)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	بازگشت FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    اگر (ccline.xpc != پوچ
	    && ccline.xpc->xp_pattern != پوچ
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	صحیح i = (صحیح)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	اگر (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    بازگشت OK;
}

#اگر defined(FEAT_ARABIC)
ایستا char_u	*arshape_buf = پوچ;

# اگر defined(EXITFREE)
    تهی
free_arshape_buf(تهی)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ایستا تهی
draw_cmdline(صحیح start, صحیح len)
{
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    صحیح		i;

    اگر (cmdline_star > 0)
	برای (i = 0; i < len; ++i)
	{
	    پیام_putchar('*');
	    اگر (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    وگرنه
#endif
#ifdef FEAT_ARABIC
	اگر (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	ایستا صحیح	buflen = 0;
	char_u		*p;
	صحیح		j;
	صحیح		newlen = 0;
	صحیح		mb_l;
	صحیح		pc, pc1 = 0;
	صحیح		prev_c = 0;
	صحیح		prev_c1 = 0;
	صحیح		u8c;
	صحیح		u8cc[MAX_MCO];
	صحیح		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	اگر (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    اگر (arshape_buf == پوچ)
		بازگشت;	// out of memory
	}

	اگر (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	برای (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    اگر (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		اگر (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    اگر (j + mb_l >= start + len)
			nc = NUL;
		    وگرنه
			nc = utf_ptr2char(p + mb_l);
		}
		وگرنه
		{
		    // displaying from left to right
		    اگر (j + mb_l >= start + len)
			pc = NUL;
		    وگرنه
		    {
			صحیح	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, پوچ, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		اگر (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    اگر (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    وگرنه
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	پیام_outtrans_len(arshape_buf, newlen);
    }
    وگرنه
#endif
	پیام_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    تهی
putcmdline(صحیح c, صحیح shift)
{
    اگر (cmd_silent)
	بازگشت;
    پیام_no_more = TRUE;
    پیام_putchar(c);
    اگر (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    پیام_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    تهی
unputcmdline(تهی)
{
    اگر (cmd_silent)
	بازگشت;
    پیام_no_more = TRUE;
    اگر (ccline.cmdlen == ccline.cmdpos)
	پیام_putchar(' ');
    وگرنه اگر (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    وگرنه
	draw_cmdline(ccline.cmdpos, 1);
    پیام_no_more = FALSE;
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
    صحیح
put_on_cmdline(char_u *str, صحیح len, صحیح redraw)
{
    صحیح		retval;
    صحیح		i;
    صحیح		m;
    صحیح		c;

    اگر (len < 0)
	len = (صحیح)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    اگر (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    وگرنه
	retval = OK;
    اگر (retval == OK)
    {
	اگر (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	وگرنه
	{
	    اگر (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		برای (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		برای (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		اگر (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		وگرنه
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    وگرنه اگر (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	اگر (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    وقتی (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    اگر (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		اگر (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		وگرنه
		    i = 0;
	    }
#endif
	    اگر (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		پیام_col -= i;
		اگر (پیام_col < 0)
		{
		    پیام_col += cmdline_width;
		    --پیام_row;
		}
	    }
	}

	اگر (redraw && !cmd_silent)
	{
	    پیام_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    اگر (cmdline_row != i || ccline.overstrike)
		پیام_clr_eos();
	    پیام_no_more = FALSE;
	}
	اگر (کلید_Typed)
	{
	    m = cmdline_width * Rows;
	    اگر (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	وگرنه
	    m = MAXCOL;
	برای (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    اگر (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    اگر (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    اگر (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		اگر (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    اگر (redraw)
	پیام_check();
    بازگشت retval;
}

ایستا cmdline_info_T	prev_ccline;
ایستا صحیح		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ایستا تهی
save_cmdline(cmdline_info_T *ccp)
{
    اگر (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = پوچ;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ایستا تهی
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
    ایستا صحیح
cmdline_paste(
    صحیح regname,
    صحیح literally,	// Insert text literally instead of "as typed"
    صحیح remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    صحیح			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    اگر (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	بازگشت FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    سطر_breakcheck();
    اگر (got_int)
	بازگشت FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    اگر (i)
    {
	// Got the value of a special register in "arg".
	اگر (arg == پوچ)
	    بازگشت FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	اگر (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    صحیح	    len;

	    // Locate start of last word in the cmd buffer.
	    برای (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		اگر (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    اگر (!vim_iswordc(mb_ptr2char(w - len)))
			بشکن;
		    w -= len;
		}
		وگرنه
		{
		    اگر (!vim_iswordc(w[-1]))
			بشکن;
		    --w;
		}
	    }
	    len = (صحیح)((ccline.cmdbuff + ccline.cmdpos) - w);
	    اگر (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	اگر (allocated)
	    vim_free(arg);
	بازگشت OK;
    }

    بازگشت cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    تهی
cmdline_paste_str(char_u *s, صحیح literally)
{
    صحیح		c, cv;

    اگر (literally)
	put_on_cmdline(s, -1, TRUE);
    وگرنه
	وقتی (*s != NUL)
	{
	    cv = *s;
	    اگر (cv == Ctrl_V && s[1])
		++s;
	    اگر (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    وگرنه
		c = *s++;
	    اگر (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    تهی
redrawcmdline(تهی)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    تهی
redrawcmdline_ex(صحیح do_compute_cmdrow)
{
    اگر (cmd_silent)
	بازگشت;
    need_wait_return = FALSE;
    اگر (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ایستا تهی
redrawcmdprompt(تهی)
{
    صحیح		i;

    اگر (cmd_silent)
	بازگشت;
    اگر (ccline.cmdfirstc != NUL)
	پیام_putchar(ccline.cmdfirstc);
    اگر (ccline.cmdprompt != پوچ)
    {
	پیام_puts_attr((نویسه *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = پیام_col + (پیام_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	اگر (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    وگرنه
	برای (i = ccline.cmdindent; i > 0; --i)
	    پیام_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    تهی
redrawcmd(تهی)
{
    صحیح save_in_echowindow = in_echowindow;

    اگر (cmd_silent)
	بازگشت;

    // when 'incsearch' is set there may be no command line while redrawing
    اگر (ccline.cmdbuff == پوچ)
    {
	windgoto(cmdline_row, cmdline_col_off);
	پیام_clr_eos();
	بازگشت;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    پیام_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    پیام_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    پیام_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    پیام_clr_eos();
    پیام_no_more = FALSE;

    set_cmdspos_cursor();
    اگر (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    پیام_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    تهی
compute_cmdrow(تهی)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    اگر (exmode_active || (پیام_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    وگرنه
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    تهی
cursorcmd(تهی)
{
    اگر (cmd_silent)
	بازگشت;

#ifdef FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	پیام_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	پیام_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	اگر (پیام_row <= 0)
	    پیام_row = Rows - 1;
    }
    وگرنه
#endif
    {
	پیام_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	پیام_col = ccline.cmdspos % cmdline_width;
	اگر (پیام_row >= Rows)
	    پیام_row = Rows - 1;
    }

    windgoto(پیام_row, cmdline_col_off + پیام_col);
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    اگر (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    تهی
gotocmdline(صحیح clr)
{
    پیام_start();
#ifdef FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
	پیام_col = cmdline_width - 1;
    وگرنه
#endif
	پیام_col = 0;	    // always start in column 0
    اگر (clr)		    // clear the bottom line(s)
	پیام_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ایستا صحیح
ccheck_abbr(صحیح c)
{
    صحیح spos = 0;

    اگر (p_paste || no_abbr)	    // no abbreviations or in paste mode
	بازگشت FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    وقتی (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    اگر (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    وگرنه
	// check abbreviation from the beginning of the commandline
	spos = 0;

    بازگشت check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, صحیح what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    صحیح		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    برای (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	اگر ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#وگرنه
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    اگر (what == VSE_SHELL && csh_like_shell() && p != پوچ)
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
    اگر (p != پوچ && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    بازگشت p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    تهی
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    اگر (p == پوچ)
	بازگشت;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    تهی
tilde_replace(
    char_u  *orig_pat,
    صحیح	    رقم_files,
    char_u  **files)
{
    صحیح	    i;
    char_u  *p;

    اگر (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	برای (i = 0; i < رقم_files; ++i)
	{
	    p = home_replace_save(پوچ, files[i]);
	    اگر (p != پوچ)
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
get_cmdline_info(تهی)
{
    بازگشت &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ایستا cmdline_info_T *
get_ccline_ptr(تهی)
{
    اگر ((State & MODE_CMDLINE) == 0)
	بازگشت پوچ;
    اگر (ccline.cmdbuff != پوچ)
	بازگشت &ccline;
    اگر (prev_ccline_used && prev_ccline.cmdbuff != پوچ)
	بازگشت &prev_ccline;
    بازگشت پوچ;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ایستا صحیح
get_cmdline_type(تهی)
{
    cmdline_info_T *p = get_ccline_ptr();

    اگر (p == پوچ)
	بازگشت NUL;
    اگر (p->cmdfirstc == NUL)
	بازگشت
#ifdef FEAT_EVAL
	    (p->درون_داد_fn) ? '@' :
#endif
	    '-';
    بازگشت p->cmdfirstc;
}

#اگر defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ایستا char_u *
get_cmdline_str(تهی)
{
    cmdline_info_T *p;

    اگر (cmdline_star > 0)
	بازگشت پوچ;
    p = get_ccline_ptr();
    اگر (p == پوچ)
	بازگشت پوچ;
    بازگشت vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ایستا char_u *
get_cmdline_completion_pattern(تهی)
{
    cmdline_info_T *p;
    صحیح		xp_context;

    اگر (cmdline_star > 0)
	بازگشت پوچ;

    p = get_ccline_ptr();
    اگر (p == پوچ || p->xpc == پوچ)
	بازگشت پوچ;

    xp_context = p->xpc->xp_context;
    اگر (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    اگر (xp_context == EXPAND_UNSUCCESSFUL)
	بازگشت پوچ;

    char_u *compl_pat = p->xpc->xp_pattern;

    اگر (compl_pat == پوچ)
	بازگشت پوچ;

    بازگشت vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ایستا char_u *
get_cmdline_completion(تهی)
{
    cmdline_info_T	*p;
    صحیح			xp_context;

    اگر (cmdline_star > 0)
	بازگشت پوچ;

    p = get_ccline_ptr();
    اگر (p == پوچ || p->xpc == پوچ)
	بازگشت پوچ;

    xp_context = p->xpc->xp_context;
    اگر (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    اگر (xp_context == EXPAND_UNSUCCESSFUL)
	بازگشت پوچ;

    بازگشت cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    تهی
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    تهی
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    تهی
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    تهی
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_عدد = p != پوچ ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    تهی
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != پوچ && p->cmdprompt != پوچ ?
					    vim_strsave(p->cmdprompt) : پوچ;
}

/*
 * "getcmdscreenpos()" function
 */
    تهی
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_عدد = p != پوچ ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    تهی
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    اگر (rettv->vval.v_string == پوچ)
	بازگشت;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ایستا صحیح
set_cmdline_str(char_u *str, صحیح pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    صحیح		    len;

    اگر (p == پوچ)
	بازگشت 1;

    len = (صحیح)STRLEN(str);
    اگر (realloc_cmdbuff(len + 1) != OK)
	بازگشت 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    بازگشت 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ایستا صحیح
set_cmdline_pos(
    صحیح		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    اگر (p == پوچ)
	بازگشت 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    اگر (pos < 0)
	new_cmdpos = 0;
    وگرنه
	new_cmdpos = pos;

    بازگشت 0;
}

// "setcmdline()" function
    تهی
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    صحیح pos = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_عدد_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;

	pos = (صحیح)tv_get_عدد_chk(&argvars[1], &error) - 1;
	اگر (error)
	    بازگشت;
	اگر (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    بازگشت;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_عدد = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    تهی
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    صحیح		pos;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    pos = (صحیح)tv_get_عدد(&argvars[0]) - 1;
    اگر (pos >= 0)
	rettv->vval.v_عدد = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    صحیح
get_cmdline_firstc(تهی)
{
    بازگشت ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    صحیح
get_list_range(char_u **str, صحیح *num1, صحیح *num2)
{
    صحیح		len;
    صحیح		first = FALSE;
    varnumber_T	رقم;

    *str = skipwhite(*str);
    اگر (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, پوچ, &len, 0, &رقم, پوچ, 0, FALSE, پوچ);
	*str += len;
	// overflow
	اگر (رقم > INT_MAX)
	    بازگشت FAIL;

	*num1 = (صحیح)رقم;
	first = TRUE;
    }
    *str = skipwhite(*str);
    اگر (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, پوچ, &len, 0, &رقم, پوچ, 0, FALSE, پوچ);
	اگر (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    اگر (رقم > INT_MAX)
		بازگشت FAIL;

	    *num2 = (صحیح)رقم;
	}
	وگرنه اگر (!first)		// no number given at all
	    بازگشت FAIL;
    }
    وگرنه اگر (first)			// only one number given
	*num2 = *num1;
    بازگشت OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    نویسه *
did_set_cedit(optset_T *args UNUSED)
{
    صحیح n;

    اگر (*p_cedit == NUL)
	cedit_کلید = -1;
    وگرنه
    {
	n = string_to_کلید(p_cedit, FALSE);
	اگر (n == 0 || vim_isprintc(n))
	    بازگشت e_invalid_argument;
	cedit_کلید = n;
    }
    بازگشت پوچ;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ایستا صحیح
open_cmdwin(تهی)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    صحیح			i;
    linenr_T		lnum;
    صحیح			histtype;
    garray_T		winsizes;
    صحیح			save_restart_edit = restart_edit;
    صحیح			save_State = State;
    صحیح			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    صحیح			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    صحیح			save_کلید_Typed;
#endif
    صحیح			newbuf_status;
    صحیح			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    اگر (text_or_buf_locked()
	    || cmdwin_type != 0
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	بازگشت K_IGNORE;
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
    اگر (win_split((صحیح)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	بازگشت K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    اگر (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	بازگشت Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, پوچ, پوچ, پوچ, ECMD_ONE, ECMD_HIDE,
					پوچ);
    cmdwin_valid = win_valid(cmdwin_win);
    اگر (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	اگر (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	اگر (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	اگر (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(پوچ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = پوچ;
	beep_flush();
	ga_clear(&winsizes);
	بازگشت Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_مقدار_give_err((char_u *)"bt",
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
    اگر (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	اگر (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_درج, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_درج, TRUE);
	}
	set_option_مقدار_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    اگر (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	اگر (i >= 0)
	{
	    lnum = 0;
	    انجامبده
	    {
		اگر (++i == get_hislen())
		    i = 0;
		اگر (get_histentry(histtype)[i].hisstr != پوچ)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    وقتی (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_سطر_شمار, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_سطر_abv_curs();
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
    اگر (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_کلید_Typed = کلید_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    کلید_Typed = save_کلید_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = پوچ;
    cmdwin_win = پوچ;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    اگر (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    وگرنه
    {
#اگر defined(FEAT_EVAL)
	// autocmds may abort script processing
	اگر (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	اگر (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    اگر (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    اگر (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (صحیح)plen;
		ccline.cmdbufflen = (صحیح)(plen + 1);
		cmdwin_result = CAR;
	    }
	    وگرنه
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	وگرنه اگر (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = پوچ;
	}
	وگرنه
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	اگر (ccline.cmdbuff == پوچ)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	وگرنه
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    اگر (ccline.cmdpos == ccline.cmdlen - 1
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
	اگر (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	اگر (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(پوچ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	اگر (cmdwin_result == K_IGNORE)
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

    بازگشت cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    صحیح
is_in_cmdwin(تهی)
{
    بازگشت cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    اگر (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == پوچ)
	بازگشت پوچ;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    اگر (l == پوچ)
	بازگشت پوچ;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_اقلام(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    بازگشت (char_u *)ga.ga_data;
#وگرنه
    بازگشت پوچ;
#endif
}

#اگر defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    تهی
get_user_درون_داد(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		inputdialog,
    صحیح		secret)
{
    char_u	*prompt;
    char_u	*p = پوچ;
    صحیح		c;
    char_u	buf[NUMBUFLEN];
    صحیح		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    صحیح		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = پوچ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    اگر (درون_داد_busy)
	بازگشت;  // this doesn't work recursively.

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	بازگشت;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_درون_داد
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    اگر (no_console_درون_داد() && !is_not_a_term())
	بازگشت;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    اگر (prompt != پوچ)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	اگر (p == پوچ)
	    p = prompt;
	وگرنه
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    پیام_start();
	    پیام_clr_eos();
	    پیام_puts_attr((نویسه *)prompt, get_echo_attr());
	    پیام_didout = FALSE;
	    پیام_starthere();
	    *p = c;
	}
	cmdline_row = پیام_row;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    اگر (defstr != پوچ)
		stuffReadbuffSpec(defstr);

	    اگر (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		صحیح	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = پوچ;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		اگر (xp_name == پوچ)
		    بازگشت;

		xp_namelen = (صحیح)STRLEN(xp_name);

		اگر (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    بازگشت;
	    }
	}

	اگر (defstr != پوچ)
	{
	    صحیح save_ex_normal_busy = ex_normal_busy;
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_درون_داد_busy = درون_داد_busy;

	    درون_داد_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    درون_داد_busy = save_درون_داد_busy;
	}
	اگر (inputdialog && rettv->vval.v_string == پوچ
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	پیام_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    تهی
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    اگر (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	بازگشت;

    صحیح cmd_type = get_cmdline_type();

    اگر (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	کلید_string[4];

	کلید_string[0] = K_SPECIAL;
	کلید_string[1] = KS_EXTRA;
	کلید_string[2] = KE_WILD;
	کلید_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(کلید_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
