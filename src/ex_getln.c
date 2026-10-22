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

#અન્તર્ભાવ "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_ચાવી	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
સ્થિર cmdline_info_T ccline;

#ifdef FEAT_EVAL
સ્થિર પૂર્ણાંક	new_cmdpos;	// position set by set_cmdline_pos()
#endif

સ્થિર પૂર્ણાંક	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
સ્થિર પૂર્ણાંક	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
સ્થિર પૂર્ણાંક	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

સ્થિર char_u	*getcmdline_int(પૂર્ણાંક firstc, long ગણતરી, પૂર્ણાંક indent, પૂર્ણાંક clear_ccline);
સ્થિર પૂર્ણાંક	cmdline_charsize(પૂર્ણાંક idx);
સ્થિર શૂન્ય	set_cmdspos(શૂન્ય);
સ્થિર શૂન્ય	set_cmdspos_cursor(શૂન્ય);
સ્થિર શૂન્ય	correct_cmdspos(પૂર્ણાંક idx, પૂર્ણાંક cells);
સ્થિર શૂન્ય	dealloc_cmdbuff(શૂન્ય);
સ્થિર શૂન્ય	alloc_cmdbuff(size_t len);
સ્થિર શૂન્ય	draw_cmdline(પૂર્ણાંક start, પૂર્ણાંક len);
સ્થિર શૂન્ય	save_cmdline(cmdline_info_T *ccp);
સ્થિર શૂન્ય	restore_cmdline(cmdline_info_T *ccp);
સ્થિર પૂર્ણાંક	cmdline_paste(પૂર્ણાંક regname, પૂર્ણાંક literally, પૂર્ણાંક remcr);
સ્થિર શૂન્ય	redrawcmdprompt(શૂન્ય);
સ્થિર પૂર્ણાંક	ccheck_abbr(પૂર્ણાંક);
સ્થિર પૂર્ણાંક	open_cmdwin(શૂન્ય);
#ifdef FEAT_શોધ_EXTRA
સ્થિર પૂર્ણાંક	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

સ્થિર પૂર્ણાંક	cedit_ચાવી = -1;	// key value of 'cedit' option

    સ્થિર શૂન્ય
trigger_cmd_autocmd(પૂર્ણાંક typechar, પૂર્ણાંક evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    સ્થિર શૂન્ય
abandon_cmdline(શૂન્ય)
{
    dealloc_cmdbuff();
    જો (સંદેશ_scrolled == 0)
	compute_cmdrow();
    સંદેશ("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_શોધ_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    સ્થિર પૂર્ણાંક
empty_pattern(char_u *p, size_t len, પૂર્ણાંક delim)
{
    magic_T	magic_val = MAGIC_ON;

    જો (len > 0)
	(શૂન્ય) skip_regexp_ex(p, delim, magic_isset(), ખાલી, ખાલી, &magic_val);
    નહીંતર
	પાછા TRUE;

    પાછા empty_pattern_magic(p, len, magic_val);
}

    સ્થિર પૂર્ણાંક
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    જ્યારે (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ખાલી)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    પાછા len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
પ્રકારવ્યાખ્યા સંરચના {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    પૂર્ણાંક		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    સ્થિર શૂન્ય
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

    સ્થિર શૂન્ય
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
પ્રકારવ્યાખ્યા સંરચના {
    pos_T	શોધ_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    પૂર્ણાંક		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    પૂર્ણાંક		did_incsearch;
    પૂર્ણાંક		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    સ્થિર શૂન્ય
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->શોધ_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    સ્થિર શૂન્ય
set_શોધ_match(pos_T *t)
{
    t->lnum += શોધ_match_લીટીઓ;
    t->col = શોધ_match_endcol;
    જો (t->lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	t->lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    પૂર્ણાંક
parse_pattern_and_range(
	pos_T	*incsearch_start,
	પૂર્ણાંક	*શોધ_delim,
	પૂર્ણાંક	*skiplen,
	પૂર્ણાંક	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    પૂર્ણાંક		delim_optional = FALSE;
    પૂર્ણાંક		delim;
    પૂર્ણાંક		use_last_pat;
    magic_T     magic = 0;
    અક્ષર	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    શોધ_first_લીટી = 0;
    શોધ_last_લીટી = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_લીટીઓ;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ખાલી);

    જો (vim_strchr((char_u *)"sgvlu", *cmd) == ખાલી)
	પાછા FALSE;

    // Skip over command name to find pattern separator
    માટે (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    જો (*skipwhite(p) == NUL)
	પાછા FALSE;

    જો (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	જો (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	નહીંતર જો (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    નહીંતર જો (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	જો (*p == '!')
	    p = skipwhite(p + 1);
	જ્યારે (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	જો (*p == NUL)
	    પાછા FALSE;
    }
    નહીંતર જો (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	જો (*p == '!')
	{
	    p++;
	    જો (*skipwhite(p) == NUL)
		પાછા FALSE;
	}
	જો (*cmd != 'g')
	    delim_optional = TRUE;
    }
    નહીંતર
	પાછા FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *શોધ_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), ખાલી, ખાલી, &magic);
    use_last_pat = end == p && *end == delim;

    જો (end == p && !use_last_pat)
	પાછા FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    જો (!use_last_pat)
    {
	અક્ષર c = *end;
	પૂર્ણાંક empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	જો (empty)
	    પાછા FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (પૂર્ણાંક)(p - ccline.cmdbuff);
    *patlen = (પૂર્ણાંક)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    જો (ea.addr_ગણતરી > 0)
    {
	પૂર્ણાંક reverse_match = ea.line2 < ea.line1;
	શોધ_first_લીટી = reverse_match ? ea.line2 : ea.line1;
	શોધ_last_લીટી = reverse_match ? ea.line1 : ea.line2;
    }
    નહીંતર જો (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	શોધ_first_લીટી = શોધ_last_લીટી = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    પાછા TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    સ્થિર પૂર્ણાંક
do_incsearch_highlighting(
	પૂર્ણાંક		    firstc,
	પૂર્ણાંક		    *શોધ_delim,
	incsearch_state_T   *is_state,
	પૂર્ણાંક		    *skiplen,
	પૂર્ણાંક		    *patlen)
{
    પૂર્ણાંક retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    જો (!p_is || cmd_silent)
	પાછા FALSE;

    // By default search all lines
    શોધ_first_લીટી = 0;
    શોધ_last_લીટી = MAXLNUM;

    જો (firstc == '/' || firstc == '?')
    {
	*શોધ_delim = firstc;
	પાછા TRUE;
    }

    જો (firstc != ':')
	પાછા FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->શોધ_start, શોધ_delim,
	    skiplen, patlen);
    --emsg_off;

    પાછા retval;
}

    સ્થિર શૂન્ય
finish_incsearch_highlighting(
	પૂર્ણાંક gotesc,
	incsearch_state_T *is_state,
	પૂર્ણાંક call_update_screen)
{
    જો (!is_state->did_incsearch)
	પાછા;

    is_state->did_incsearch = FALSE;
    જો (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    નહીંતર
    {
	જો (!EQUAL_POS(is_state->save_cursor, is_state->શોધ_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->શોધ_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    શોધ_first_લીટી = 0;
    શોધ_last_લીટી = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    જો (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    સ્થિર શૂન્ય
may_do_incsearch_highlighting(
	પૂર્ણાંક		    firstc,
	long		    ગણતરી,
	incsearch_state_T   *is_state)
{
    પૂર્ણાંક		skiplen, patlen;
    પૂર્ણાંક		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    પૂર્ણાંક		next_char;
    પૂર્ણાંક		use_last_pat;
    પૂર્ણાંક		did_do_incsearch = is_state->did_incsearch;
    પૂર્ણાંક		શોધ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_શોધ_pattern();

    જો (!do_incsearch_highlighting(firstc, &શોધ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_શોધ_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	જો (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	પાછા;
    }

    // If there is a character waiting, search and redraw later.
    જો (char_avail())
    {
	restore_last_શોધ_pattern();
	is_state->incsearch_postponed = TRUE;
	પાછા;
    }
    is_state->incsearch_postponed = FALSE;

    જો (શોધ_first_લીટી == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->શોધ_start;
    નહીંતર જો (શોધ_first_લીટી > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	curwin->w_cursor.col = MAXCOL;
    }
    નહીંતર
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = શોધ_first_લીટી;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    જો (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    નહીંતર
    {
	પૂર્ણાંક શોધ_flags = શોધ_OPT + શોધ_NOOF + શોધ_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	જો (!p_hls)
	    શોધ_flags += શોધ_KEEP;
	જો (શોધ_first_લીટી != 0)
	    શોધ_flags += શોધ_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_શોધ(ખાલી, firstc == ':' ? '/' : firstc, શોધ_delim,
				 ccline.cmdbuff + skiplen, patlen, ગણતરી, શોધ_flags,
# ifdef FEAT_RELTIME
		&sia
# નહીંતર
		ખાલી
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	જો (curwin->w_cursor.lnum < શોધ_first_લીટી
		|| curwin->w_cursor.lnum > શોધ_last_લીટી)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->શોધ_start;
	}

	// if interrupted while searching, behave like it failed
	જો (got_int)
	{
	    (શૂન્ય)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	નહીંતર જો (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    જો (found != 0)
	highlight_match = TRUE;		// highlight position
    નહીંતર
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    જો (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_શોધ_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    નહીંતર
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    જો (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	જો (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, શોધ_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    જો (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_શોધ_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    જો (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->શોધ_start;
    નહીંતર જો (found != 0)
	curwin->w_cursor = end_pos;

    સંદેશ_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    સ્થિર પૂર્ણાંક
may_adjust_incsearch_highlighting(
	પૂર્ણાંક			firstc,
	long			ગણતરી,
	incsearch_state_T	*is_state,
	પૂર્ણાંક			c)
{
    પૂર્ણાંક		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ખાલી;
    char_u	*searchstr;
    char_u	*strcopy = ખાલી;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    પૂર્ણાંક		શોધ_flags = શોધ_NOOF;
    પૂર્ણાંક		i;
    પૂર્ણાંક		શોધ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_શોધ_pattern();

    જો (!do_incsearch_highlighting(firstc, &શોધ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_શોધ_pattern();
	પાછા OK;
    }
    જો (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_શોધ_pattern();
	પાછા FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (શૂન્ય)parse_શોધ_pattern_offset(&pat, &patlen_s, શોધ_delim,
				    શોધ_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    જો (c == Ctrl_G)
    {
	t = is_state->match_end;
	જો (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (શૂન્ય)decl(&t);
	શોધ_flags += શોધ_COL;
    }
    નહીંતર
	t = is_state->match_start;
    જો (!p_hls)
	શોધ_flags += શોધ_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ખાલી,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, ગણતરી, શોધ_flags, RE_શોધ, ખાલી);
    --emsg_off;
    જો (dircp != ખાલી)
	*dircp = શોધ_delim;
    જો (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->શોધ_start = match_start;
	જો (!offset.લીટી && (offset.end || off != 0))
	{
	    જો (offset.end)
	    {
		is_state->શોધ_start = match_end;
		(શૂન્ય)decl(&is_state->શોધ_start);
	    }
	    જ્યારે (off > 0)
	    {
		જો (incl(&is_state->શોધ_start) == -1)
		    તોડો;
		--off;
	    }
	    જ્યારે (off < 0)
	    {
		જો (decl(&is_state->શોધ_start) == -1)
		    તોડો;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	જો (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->શોધ_start = t;
	    (શૂન્ય)decl(&is_state->શોધ_start);
	}
	નહીંતર જો (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->શોધ_start = t;
	    (શૂન્ય)incl(&is_state->શોધ_start);
	}
	જો (LT_POS(t, is_state->શોધ_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->શોધ_start = t;
	    જો (firstc == '?')
		(શૂન્ય)incl(&is_state->શોધ_start);
	    નહીંતર
		(શૂન્ય)decl(&is_state->શોધ_start);
	}

	set_શોધ_match(&is_state->match_end);
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
    નહીંતર
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_શોધ_pattern();
    પાછા FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    સ્થિર પૂર્ણાંક
may_add_char_to_શોધ(પૂર્ણાંક firstc, પૂર્ણાંક *c, incsearch_state_T *is_state)
{
    પૂર્ણાંક		skiplen, patlen, શોધ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_શોધ_pattern();

    જો (!do_incsearch_highlighting(firstc, &શોધ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_શોધ_pattern();
	પાછા FAIL;
    }
    restore_last_શોધ_pattern();

    // Add a character from under the cursor for 'incsearch'.
    જો (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	જો (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    જો (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    જો (*c == શોધ_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ખાલી)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    જો (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		પૂર્ણાંક save_c = *c;

		જ્યારે (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    પાછા FAIL;
	}
    }
    પાછા OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    સ્થિર પૂર્ણાંક
cmdline_has_arabic(પૂર્ણાંક start, પૂર્ણાંક len)
{
    પૂર્ણાંક	    j;
    પૂર્ણાંક	    mb_l;
    પૂર્ણાંક	    u8c;
    char_u  *p;
    પૂર્ણાંક	    u8cc[MAX_MCO];

    જો (!enc_utf8)
	પાછા FALSE;

    માટે (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	જો (ARABIC_CHAR(u8c))
	    પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

    શૂન્ય
cmdline_init(શૂન્ય)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    સ્થિર પૂર્ણાંક
cmdline_handle_ctrl_bsl(પૂર્ણાંક c, પૂર્ણાંક *gotesc)
{
    ++no_mapping;
    ++allow_ચાવીઓ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ચાવીઓ;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    જો (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && ચાવી_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	પાછા PROCESS_NEXT_ચાવી;
    }

#ifdef FEAT_EVAL
    જો (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	જો (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	નહીંતર
	    new_cmdpos = ccline.cmdpos;

	c = get_પદાવલિ_register();
	જો (c == '=')
	{
	    char_u	*p = ખાલી;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_પદાવલિ_લીટી();
	    --textlock;

	    જો (p != ખાલી)
	    {
		પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(p);

		જો (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    જો (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    નહીંતર
			ccline.cmdpos = new_cmdpos;

		    ચાવી_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    પાછા CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	પાછા CMDLINE_NOT_CHANGED;
    }
#endif

    જો (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    પાછા GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    સ્થિર પૂર્ણાંક
cmdline_wildchar_complete(
	પૂર્ણાંક		c,
	પૂર્ણાંક		escape,
	પૂર્ણાંક		*did_wild_list,
	પૂર્ણાંક		*wim_index_p,
	expand_T	*xp,
	પૂર્ણાંક		*gotesc,
	પૂર્ણાંક		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    પૂર્ણાંક		wim_index = *wim_index_p;
    પૂર્ણાંક		res;
    પૂર્ણાંક		cmdpos_before;
    પૂર્ણાંક		options = WILD_NO_BEEP;
    પૂર્ણાંક		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    પૂર્ણાંક		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    જો (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    જો (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	જો (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (શૂન્ય)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	જો (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	નહીંતર જો (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	નહીંતર
	    res = OK;	    // don't insert 'wildchar' now
    }
    નહીંતર		    // typed p_wc first time
    {
	પૂર્ણાંક wim_longest = (wim_flags[0] & WIM_LONGEST);
	પૂર્ણાંક wim_list = (wim_flags[0] & WIM_LIST);
	પૂર્ણાંક wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	જો (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    જો (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    જો (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    નહીંતર
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	જો (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	નહીંતર
	{
	    જો (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    જો (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	જો (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	જો (got_int)
	{
	    (શૂન્ય)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (શૂન્ય)ExpandOne(xp, ખાલી, ખાલી, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    પાછા CMDLINE_CHANGED;
	}

	// Display matches
	જો (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    જો (wim_longest)
	    {
		પૂર્ણાંક found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		પૂર્ણાંક show_menu = p_wmnu && wim_full;

		જો (wim_list || show_menu)
		    (શૂન્ય)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		નહીંતર જો (!found_longest_prefix)
		{
		    પૂર્ણાંક wim_list_next = (wim_flags[1] & WIM_LIST);
		    પૂર્ણાંક wim_full_next = (wim_flags[1] & WIM_FULL);
		    પૂર્ણાંક wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    પૂર્ણાંક wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    જો (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			જો (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			નહીંતર
			    (શૂન્ય)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			જો (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    નહીંતર
	    {
		પૂર્ણાંક show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		જો (wim_list || show_menu)
		    (શૂન્ય)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		નહીંતર
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    જો (wim_list)
		*did_wild_list = TRUE;
	}
	નહીંતર જો (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    જો (wim_index < 3)
	++wim_index;
    જો (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    પાછા (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    સ્થિર પૂર્ણાંક
cmdline_erase_chars(
	પૂર્ણાંક c,
	પૂર્ણાંક indent
#ifdef FEAT_શોધ_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;

    જો (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    જો (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    જો (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    જો (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	જો (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    જો (c == Ctrl_W)
	    {
		જ્યારે (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		જ્યારે (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		જો (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	નહીંતર જો (c == Ctrl_W)
	{
	    જ્યારે (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    જો (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		જ્યારે (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	નહીંતર
	    --p;
	ccline.cmdpos = (પૂર્ણાંક)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	જ્યારે (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_શોધ_EXTRA
	જો (ccline.cmdlen == 0)
	{
	    isp->શોધ_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    નહીંતર જો (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ખાલી && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	જો (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    પાછા CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	જો (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    જો (cmdmsg_rl)
		સંદેશ_col = cmdline_width;
	    નહીંતર
#endif
		સંદેશ_col = 0;
	    સંદેશ_putchar(' ');		// delete ':'
	}
#ifdef FEAT_શોધ_EXTRA
	isp->શોધ_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	પાછા GOTO_NORMAL_MODE;
    }
    પાછા CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    સ્થિર શૂન્ય
cmdline_toggle_langmap(long *b_im_ptr)
{
    જો (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_આગત_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	જો (b_im_ptr != ખાલી)
	{
	    જો (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    નહીંતર
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_આગત_METHOD
    નહીંતર
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	જો ((p_imdisable && b_im_ptr != ખાલી)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    જો (b_im_ptr != ખાલી)
		*b_im_ptr = B_IMODE_NONE;
	}
	નહીંતર
	{
	    im_set_active(TRUE);	// Enable input method
	    જો (b_im_ptr != ખાલી)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    જો (b_im_ptr != ખાલી)
    {
	જો (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	નહીંતર
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#જો defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    સ્થિર પૂર્ણાંક
cmdline_ઉમેરો_reg(પૂર્ણાંક *gotesc UNUSED)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;
    પૂર્ણાંક		literally = FALSE;
#ifdef FEAT_EVAL
    પૂર્ણાંક		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_ચાવીઓ;
    i = c = plain_vgetc();	// CTRL-R <char>
    જો (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    જો (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_ચાવીઓ;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    જો (c == '=')
    {
	જો (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	નહીંતર
	    c = get_પદાવલિ_register();
    }
#endif
    જો (c != ESC)	    // use ESC to cancel inserting register
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
	જો (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    પાછા GOTO_NORMAL_MODE;
	}
#endif
	ચાવી_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	જો (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    જો (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    નહીંતર
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
    પાછા literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    સ્થિર શૂન્ય
cmdline_ડાબું_જમણું_mouse(પૂર્ણાંક c, પૂર્ણાંક *ignore_drag_release)
{
    જો (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    નહીંતર
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    જો (!gui.in_use)
#endif
	જો (!mouse_has(MOUSE_COMMAND))
	    પાછા;
#ifdef FEAT_CLIPBOARD
    જો (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	પૂર્ણાંક	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	જો (mouse_model_popup() && button == MOUSE_ડાબું
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_જમણું;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	પાછા;
    }
#endif

    set_cmdspos();
    માટે (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	પૂર્ણાંક	i;

	i = cmdline_charsize(ccline.cmdpos);
	જો (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    તોડો;
	જો (has_mbyte)
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
    સ્થિર પૂર્ણાંક
cmdline_browse_history(
	પૂર્ણાંક	c,
	પૂર્ણાંક	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	પૂર્ણાંક	histype,
	પૂર્ણાંક	*hiscnt_p,
	expand_T *xp)
{
    પૂર્ણાંક		orig_hiscnt;
    પૂર્ણાંક		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    પૂર્ણાંક		res;

    જો (get_hislen() == 0 || firstc == NUL)	// no history
	પાછા CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    જો (lookfor == ખાલી)
    {
	જો ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ખાલી)
	    પાછા CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    માટે (;;)
    {
	// one step backwards
	જો (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    જો (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    નહીંતર જો (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    નહીંતર જો (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    નહીંતર			// at top of list
	    {
		hiscnt = orig_hiscnt;
		તોડો;
	    }
	}
	નહીંતર    // one step forwards
	{
	    // on last entry, clear the line
	    જો (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		તોડો;
	    }

	    // not on a history line, nothing to do
	    જો (hiscnt == get_hislen())
		તોડો;
	    જો (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    નહીંતર
		++hiscnt;
	}
	જો (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ખાલી)
	{
	    hiscnt = orig_hiscnt;
	    તોડો;
	}
	જો ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    તોડો;
    }

    જો (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	પૂર્ણાંક	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	જો (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	નહીંતર
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	જો (histype == HIST_શોધ
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    પૂર્ણાંક     i;
	    પૂર્ણાંક     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    માટે (i = 0; i <= 1; ++i)
	    {
		len = 0;
		માટે (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    જો (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			જો (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    નહીંતર
		    {
			// Escape new sep, unless it is already
			// escaped.
			જો (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    જો (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			જો (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		જો (i == 0)
		{
		    alloc_cmdbuff(len);
		    જો (ccline.cmdbuff == ખાલી)
		    {
			res = GOTO_NORMAL_MODE;
			જાઓ done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (પૂર્ણાંક)len;
	}
	નહીંતર
	{
	    alloc_cmdbuff(plen);
	    જો (ccline.cmdbuff == ખાલી)
	    {
		res = GOTO_NORMAL_MODE;
		જાઓ done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (પૂર્ણાંક)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	જાઓ done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    પાછા res;
}

/*
 * Initialize the current command-line info.
 */
    સ્થિર પૂર્ણાંક
init_ccline(પૂર્ણાંક firstc, પૂર્ણાંક indent)
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
    જો (ccline.cmdbuff == ખાલી)
    {
	ccline.cmdbufflen = 0;
	પાછા FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    જો (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    પાછા OK;
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
    પૂર્ણાંક		  firstc,
    long	  ગણતરી,	// only used for incremental search
    પૂર્ણાંક		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    પાછા getcmdline_int(firstc, ગણતરી, indent, TRUE);
}

    સ્થિર char_u *
getcmdline_int(
    પૂર્ણાંક		firstc,
    long	ગણતરી UNUSED,	// only used for incremental search
    પૂર્ણાંક		indent,		// indent for inside conditionals
    પૂર્ણાંક		clear_ccline)	// clear ccline first
{
    સ્થિર પૂર્ણાંક	depth = 0;	    // call depth
    પૂર્ણાંક		c = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    પૂર્ણાંક		gotesc = FALSE;		// TRUE when <ESC> just typed
    પૂર્ણાંક		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ખાલી;	// string to match
    size_t	lookforlen = 0;
    પૂર્ણાંક		hiscnt;			// current history line in use
    પૂર્ણાંક		histype;		// history type to be used
#ifdef FEAT_શોધ_EXTRA
    incsearch_state_T	is_state;
#endif
    પૂર્ણાંક		did_wild_list = FALSE;	// did wild_list() recently
    પૂર્ણાંક		wim_index = 0;		// index in wim_flags[]
    પૂર્ણાંક		res;
    પૂર્ણાંક		save_સંદેશ_scroll = સંદેશ_scroll;
    પૂર્ણાંક		save_State = State;	// remember State when called
    પૂર્ણાંક		some_ચાવી_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    પૂર્ણાંક		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    પૂર્ણાંક		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = ખાલી;
    buf_T	*b_im_ptr_buf = ખાલી;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    પૂર્ણાંક		did_save_ccline = FALSE;
    પૂર્ણાંક		cmdline_type;
    પૂર્ણાંક		wild_type = 0;
    પૂર્ણાંક		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ખાલી;
    પૂર્ણાંક		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    જો (ccline.cmdbuff != ખાલી)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    જો (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    જો (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    જો (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_શોધ_EXTRA
    init_incsearch_state(&is_state);
#endif

    જો (init_ccline(firstc, indent) != OK)
	જાઓ theend;	// out of memory

    જો (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	જાઓ theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    નહીંતર
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    જો (!cmd_silent)
    {
	i = સંદેશ_scrolled;
	સંદેશ_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	સંદેશ_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#જો defined(FEAT_EVAL)
    જો (ccline.આગત_fn)
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
    સંદેશ_scroll = FALSE;

    State = MODE_CMDLINE;

    જો (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	જો (curbuf->b_p_imsearch == B_IMODE_USE_ઉમેરો)
	    b_im_ptr = &curbuf->b_p_iminsert;
	નહીંતર
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	જો (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_આગત_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_આગત_METHOD
    નહીંતર જો (p_imcmdline)
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
    જો (!debug_mode)
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
    જો (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    જો (!cmd_silent && સંદેશ_scrolled == 0)
    {
	પૂર્ણાંક	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    જો (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	જો (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	જો (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    માટે (;;)
    {
	પૂર્ણાંક	trigger_cmdlinechanged = TRUE;
	પૂર્ણાંક	end_wildmenu;
	પૂર્ણાંક	prev_cmdpos = ccline.cmdpos;
	પૂર્ણાંક	skip_pum_redraw = FALSE;

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

	જો (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_ચાવી_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	જો (ccline.cmdbuff != ખાલી)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    જો (prev_cmdbuff == ખાલી)
		જાઓ returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	જો (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	કરો
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} જ્યારે (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	જો (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    જો (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (શૂન્ય)ExpandOne(&xpc, ખાલી, ખાલી, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	જો (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    ચાલુ;
	}

	જો (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    પૂર્ણાંક	    clen = ccline.cmdlen;
	    પૂર્ણાંક	    cc_ગણતરી = aucmd_cmdline_changed_ગણતરી;

	    જો (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		જો (clen == ccline.cmdlen
				    || cc_ગણતરી != aucmd_cmdline_changed_ગણતરી)
		    trigger_cmdlinechanged = FALSE;
		જાઓ cmdline_changed;
	    }
	}

	જો (ચાવી_Typed)
	{
	    some_ચાવી_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    જો (cmd_hkmap)
		c = hkmap(c);
	    જો (cmdmsg_rl && !ચાવી_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		પસંદગી (c)
		{
		    સ્થિતિ K_જમણું:   c = K_ડાબું; તોડો;
		    સ્થિતિ K_S_જમણું: c = K_S_ડાબું; તોડો;
		    સ્થિતિ K_C_જમણું: c = K_C_ડાબું; તોડો;
		    સ્થિતિ K_ડાબું:    c = K_જમણું; તોડો;
		    સ્થિતિ K_S_ડાબું:  c = K_S_જમણું; તોડો;
		    સ્થિતિ K_C_ડાબું:  c = K_C_જમણું; તોડો;
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
	જો ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#જો defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	જો (lookfor != ખાલી
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ડાબું && c != K_જમણું
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	જો (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	જો (p_wmnu)
	    c = wildmenu_translate_ચાવી(&ccline, c, &xpc, did_wild_list);

	પૂર્ણાંક ચાવી_is_wc = (c == p_wc && ચાવી_Typed) || c == p_wcm;
	જો ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !ચાવી_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    જો (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		જો (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    તોડો;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	જો (ચાવી_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#જો defined(FEAT_શોધ_EXTRA)
	    જો ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!ચાવી_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	જો (end_wildmenu)
	{
	    જો (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !ચાવી_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    જો (xpc.xp_numfiles != -1)
		(શૂન્ય)ExpandOne(&xpc, ખાલી, ખાલી, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    જો (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	જો (p_wmnu)
	    c = wildmenu_process_ચાવી(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	જો (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    જો (res == CMDLINE_CHANGED)
		જાઓ cmdline_changed;
	    નહીંતર જો (res == CMDLINE_NOT_CHANGED)
		જાઓ cmdline_not_changed;
	    નહીંતર જો (res == GOTO_NORMAL_MODE)
		જાઓ returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	જો (c == cedit_ચાવી || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    જો ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_ચાવી_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	નહીંતર
	    c = do_digraph(c);
#endif

	જો (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!ચાવી_Typed || vim_strchr(p_cpo, CPO_ESC) != ખાલી)))
	{
	    // In Ex mode a backslash escapes a newline.
	    જો (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		જો (c == K_KENTER)
		    c = '\n';
	    }
	    નહીંતર
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		જો (ccheck_abbr(c + ABBR_OFF))
		    જાઓ cmdline_changed;
		જો (!cmd_silent)
		{
		    windgoto(સંદેશ_row, cmdline_col_off);
		    out_flush();
		}
		તોડો;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	જો ((c == p_wc && !gotesc && ચાવી_Typed) || c == p_wcm || c == K_WILD)
	{
	    જો (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_શોધ_EXTRA
		    &is_state.શોધ_start
#નહીંતર
		    ખાલી
#endif
		    );
	    જો (c == K_WILD)
		--emsg_silent;
	    જો (res == CMDLINE_CHANGED)
		જાઓ cmdline_changed;
	    જો (c == K_WILD)
		જાઓ cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	જો (c == K_S_TAB && ચાવી_Typed)
	{
	    જો (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		જો (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    પૂર્ણાંક list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		જો (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    જાઓ cmdline_changed;
	    }
	}

	જો (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	જો (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_શોધ_EXTRA
	    // Apply search highlighting
	    જો (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    જો (ચાવી_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, ગણતરી, &is_state);
#endif
	    wild_type = 0;
	    જાઓ cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	પસંદગી (c)
	{
	સ્થિતિ K_BS:
	સ્થિતિ Ctrl_H:
	સ્થિતિ K_DEL:
	સ્થિતિ K_KDEL:
	સ્થિતિ Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_શોધ_EXTRA
		    , &is_state
#endif
		    );
	    જો (res == CMDLINE_NOT_CHANGED)
		જાઓ cmdline_not_changed;
	    નહીંતર જો (res == GOTO_NORMAL_MODE)
		જાઓ returncmd;		// back to cmd mode
	    જાઓ cmdline_changed;

	સ્થિતિ K_INS:
	સ્થિતિ K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		જાઓ cmdline_not_changed;

	સ્થિતિ Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ખાલી);
		જાઓ cmdline_not_changed;

//	case '@':   only in very old vi
	સ્થિતિ Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		જ્યારે (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_શોધ_EXTRA
		જો (ccline.cmdlen == 0)
		    is_state.શોધ_start = is_state.save_cursor;
#endif
		redrawcmd();
		જાઓ cmdline_changed;

#ifdef FEAT_CLIPBOARD
	સ્થિતિ Ctrl_Y:
		// Copy the modeless selection, if there is one.
		જો (clip_star.state != SELECT_CLEARED)
		{
		    જો (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    જાઓ cmdline_not_changed;
		}
		તોડો;
#endif

	સ્થિતિ ESC:	// get here if p_wc != ESC or when ESC typed twice
	સ્થિતિ Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		જો (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    જાઓ cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		જાઓ returncmd;		// back to cmd mode

	સ્થિતિ Ctrl_R:			// insert register
		res = cmdline_ઉમેરો_reg(&gotesc);
		જો (res == GOTO_NORMAL_MODE)
		    જાઓ returncmd;
		જો (res == CMDLINE_CHANGED)
		    જાઓ cmdline_changed;
		જાઓ cmdline_not_changed;

	સ્થિતિ Ctrl_D:
		જો (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    તોડો;	// Use ^D as normal char instead

		redrawcmd();
		ચાલુ;	// don't do incremental search now

	સ્થિતિ K_જમણું:
	સ્થિતિ K_S_જમણું:
	સ્થિતિ K_C_જમણું:
		કરો
		{
		    જો (ccline.cmdpos >= ccline.cmdlen)
			તોડો;
		    i = cmdline_charsize(ccline.cmdpos);
		    જો (ચાવી_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			તોડો;
		    ccline.cmdspos += i;
		    જો (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    નહીંતર
			++ccline.cmdpos;
		}
		જ્યારે ((c == K_S_જમણું || c == K_C_જમણું
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		જો (has_mbyte)
		    set_cmdspos_cursor();
		જાઓ cmdline_not_changed;

	સ્થિતિ K_ડાબું:
	સ્થિતિ K_S_ડાબું:
	સ્થિતિ K_C_ડાબું:
		જો (ccline.cmdpos == 0)
		    જાઓ cmdline_not_changed;
		કરો
		{
		    --ccline.cmdpos;
		    જો (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		જ્યારે (ccline.cmdpos > 0
			&& (c == K_S_ડાબું || c == K_C_ડાબું
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		જો (has_mbyte)
		    set_cmdspos_cursor();
		જાઓ cmdline_not_changed;

	સ્થિતિ K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		જાઓ cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	સ્થિતિ K_F4:
	    જો (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		જાઓ cmdline_not_changed;
	    }
	    તોડો;
#endif

	સ્થિતિ K_MIDDLEDRAG:
	સ્થિતિ K_MIDDLERELEASE:
		જાઓ cmdline_not_changed;	// Ignore mouse

	સ્થિતિ K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		જો (!gui.in_use)
#endif
		    જો (!mouse_has(MOUSE_COMMAND))
			જાઓ cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		જો (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		નહીંતર
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		જાઓ cmdline_changed;

#ifdef FEAT_DND
	સ્થિતિ K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		જાઓ cmdline_changed;
#endif

	સ્થિતિ K_LEFTDRAG:
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_RIGHTDRAG:
	સ્થિતિ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		જો (ignore_drag_release)
		    જાઓ cmdline_not_changed;
		// FALLTHROUGH
	સ્થિતિ K_LEFTMOUSE:
	સ્થિતિ K_RIGHTMOUSE:
		cmdline_ડાબું_જમણું_mouse(c, &ignore_drag_release);
		જાઓ cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	સ્થિતિ K_MOUSEDOWN:
	સ્થિતિ K_MOUSEUP:
	સ્થિતિ K_MOUSELEFT:
	સ્થિતિ K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		જો (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_ડાબું
				      : MSCR_જમણું);
#endif
		જાઓ cmdline_not_changed;

	// Alternate buttons ignored here
	સ્થિતિ K_X1MOUSE:
	સ્થિતિ K_X1DRAG:
	સ્થિતિ K_X1RELEASE:
	સ્થિતિ K_X2MOUSE:
	સ્થિતિ K_X2DRAG:
	સ્થિતિ K_X2RELEASE:
	સ્થિતિ K_MOUSEMOVE:
		જાઓ cmdline_not_changed;

#ifdef FEAT_GUI
	સ્થિતિ K_LEFTMOUSE_NM:	// mousefocus click, ignored
	સ્થિતિ K_LEFTRELEASE_NM:
		જાઓ cmdline_not_changed;

	સ્થિતિ K_VER_SCROLLBAR:
		જો (સંદેશ_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		જાઓ cmdline_not_changed;

	સ્થિતિ K_HOR_SCROLLBAR:
		જો (સંદેશ_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_કિંમત);
		    redrawcmd();
		}
		જાઓ cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	સ્થિતિ K_TABLINE:
	સ્થિતિ K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		જો (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		જાઓ cmdline_not_changed;
#endif

	સ્થિતિ K_SELECT:	    // end of Select mode mapping - ignore
		જાઓ cmdline_not_changed;

	સ્થિતિ Ctrl_B:	    // begin of command line
	સ્થિતિ K_HOME:
	સ્થિતિ K_KHOME:
	સ્થિતિ K_S_HOME:
	સ્થિતિ K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		જાઓ cmdline_not_changed;

	સ્થિતિ Ctrl_E:	    // end of command line
	સ્થિતિ K_END:
	સ્થિતિ K_KEND:
	સ્થિતિ K_S_END:
	સ્થિતિ K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		જાઓ cmdline_not_changed;

	સ્થિતિ Ctrl_A:	    // all matches
		જો (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		જો (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    તોડો;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		જાઓ cmdline_changed;

	સ્થિતિ Ctrl_L:
#ifdef FEAT_શોધ_EXTRA
		જો (may_add_char_to_શોધ(firstc, &c, &is_state) == OK)
		    જાઓ cmdline_not_changed;
#endif

		// completion: longest common part
		જો (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    તોડો;
		જાઓ cmdline_changed;

	સ્થિતિ Ctrl_N:	    // next match
	સ્થિતિ Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		જો (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    જો (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    જાઓ cmdline_not_changed;
		}
#endif
		જો (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    જો (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			તોડો;
		    જાઓ cmdline_changed;
		}
		// FALLTHROUGH
	સ્થિતિ K_UP:
	સ્થિતિ K_DOWN:
	સ્થિતિ K_S_UP:
	સ્થિતિ K_S_DOWN:
	સ્થિતિ K_PAGEUP:
	સ્થિતિ K_KPAGEUP:
	સ્થિતિ K_PAGEDOWN:
	સ્થિતિ K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		જો (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    પૂર્ણાંક	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    જો (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    જાઓ cmdline_not_changed;
		}
#endif
		જો (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    જો (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    જો (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			તોડો;
		    જાઓ cmdline_changed;
		}
		નહીંતર
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    જો (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			જાઓ cmdline_changed;
		    }
		    નહીંતર જો (res == GOTO_NORMAL_MODE)
			જાઓ returncmd;
		}
		જાઓ cmdline_not_changed;

#ifdef FEAT_શોધ_EXTRA
	સ્થિતિ Ctrl_G:	    // next match
	સ્થિતિ Ctrl_T:	    // previous match
		જો (may_adjust_incsearch_highlighting(
					  firstc, ગણતરી, &is_state, c) == FAIL)
		    જાઓ cmdline_not_changed;
		તોડો;
#endif

	સ્થિતિ Ctrl_V:
	સ્થિતિ Ctrl_Q:
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
		    જો (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			સંદેશ_putchar(' ');
			cursorcmd();
		    }
		}

		તોડો;

#ifdef FEAT_DIGRAPHS
	સ્થિતિ Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		જો (c != NUL)
		    તોડો;

		redrawcmd();
		જાઓ cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	સ્થિતિ Ctrl__:	    // CTRL-_: switch language mode
		જો (!p_ari)
		    તોડો;
		cmd_hkmap = !cmd_hkmap;
		જાઓ cmdline_not_changed;
#endif

	સ્થિતિ K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ખાલી);
		જાઓ cmdline_changed;

	મૂળભૂત:
#ifdef UNIX
		જો (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    જાઓ returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		જો (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		તોડો;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	જો (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    જાઓ cmdline_changed;

	/*
	 * put the character in the command line
	 */
	જો (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_ચાવી_name(c, mod_mask), -1, TRUE);
	નહીંતર
	{
	    જો (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    નહીંતર
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	જાઓ cmdline_changed;

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
	જો (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_શોધ_EXTRA
	જો (!is_state.incsearch_postponed)
	    ચાલુ;
#endif

cmdline_changed:
#ifdef FEAT_શોધ_EXTRA
	// If the window changed incremental search state is not valid.
	જો (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	જો (xpc.xp_context == EXPAND_NOTHING && (ચાવી_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, ગણતરી, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	જો (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ખાલી &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    જો (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	જો (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	જો (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    જો (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    જો (!event_cmdlineleavepre_triggered)
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
    જો (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ખાલી;
    clear_cmdline_orig();

#ifdef FEAT_શોધ_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    જો (ccline.cmdbuff != ખાલી)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	જો (ccline.cmdlen && firstc != NUL
		&& (some_ચાવી_typed || histype == HIST_શોધ))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_શોધ ? firstc : NUL);
	    જો (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	જો (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    સંદેશ_check();
    સંદેશ_scroll = save_સંદેશ_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    જો (some_ચાવી_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    જો (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_આગત_METHOD
    જો (b_im_ptr != ખાલી && buf_valid(b_im_ptr_buf)
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
	જો (did_save_ccline)
	    restore_cmdline(&save_ccline);
	નહીંતર
	    ccline.cmdbuff = ખાલી;

	vim_free(prev_cmdbuff);
	પાછા p;
    }
}

#જો defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    પૂર્ણાંક		firstc,
    char_u	*prompt,	// command line prompt
    પૂર્ણાંક		attr,		// attributes for prompt
    પૂર્ણાંક		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    પૂર્ણાંક			did_save_ccline = FALSE;
    પૂર્ણાંક			સંદેશ_col_save = સંદેશ_col;
    પૂર્ણાંક			સંદેશ_silent_save = સંદેશ_silent;

    જો (ccline.cmdbuff != ખાલી)
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
    ccline.આગત_fn = (firstc == '@');
# endif
    સંદેશ_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    જો (did_save_ccline)
	restore_cmdline(&save_ccline);

    સંદેશ_silent = સંદેશ_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    જો (ccline.cmdbuff != ખાલી)
	સંદેશ_col = સંદેશ_col_save;

    પાછા s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    પૂર્ણાંક
check_opt_wim(શૂન્ય)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		idx = 0;

    માટે (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    માટે (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	માટે (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	જો (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    પાછા FAIL;
	જો (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	નહીંતર જો (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	નહીંતર જો (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	નહીંતર જો (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	નહીંતર જો (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	નહીંતર જો (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	નહીંતર
	    પાછા FAIL;
	p += i;
	જો (*p == NUL)
	    તોડો;
	જો (*p == ',')
	{
	    જો (idx == 3)
		પાછા FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    જ્યારે (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    માટે (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    પાછા OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    પૂર્ણાંક
text_locked(શૂન્ય)
{
    જો (cmdwin_type != 0)
	પાછા TRUE;
    પાછા textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    શૂન્ય
text_locked_સંદેશ(શૂન્ય)
{
    emsg(_(get_text_locked_સંદેશ()));
}

    અક્ષર *
get_text_locked_સંદેશ(શૂન્ય)
{
    જો (cmdwin_type != 0)
	પાછા e_invalid_in_cmdline_window;
    પાછા e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    પૂર્ણાંક
text_or_buf_locked(શૂન્ય)
{
    જો (text_locked())
    {
	text_locked_સંદેશ();
	પાછા TRUE;
    }
    પાછા curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    પૂર્ણાંક
curbuf_locked(શૂન્ય)
{
    જો (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	પાછા TRUE;
    }
    પાછા allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    પૂર્ણાંક
allbuf_locked(શૂન્ય)
{
    જો (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	પાછા TRUE;
    }
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
cmdline_charsize(પૂર્ણાંક idx)
{
#જો defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    જો (cmdline_star > 0)	    // showing '*', always 1 position
	પાછા 1;
#endif
    પાછા ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    સ્થિર શૂન્ય
set_cmdspos(શૂન્ય)
{
    જો (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    નહીંતર
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    સ્થિર શૂન્ય
set_cmdspos_cursor(શૂન્ય)
{
    પૂર્ણાંક		i, m, c;

    set_cmdspos();
    જો (ચાવી_Typed)
    {
	m = cmdline_width * Rows;
	જો (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    નહીંતર
	m = MAXCOL;
    માટે (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	જો (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	જો ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    તોડો;
	}
	જો (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    સ્થિર શૂન્ય
correct_cmdspos(પૂર્ણાંક idx, પૂર્ણાંક cells)
{
    જો ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    પૂર્ણાંક		c,		// normally ':', NUL for ":append"
    શૂન્ય	*cookie UNUSED,
    પૂર્ણાંક		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    જો (exec_from_reg && vpeekc() == ':')
	(શૂન્ય)vgetc();
    પાછા getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    પૂર્ણાંક		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    શૂન્ય	*cookie UNUSED,
    પૂર્ણાંક		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	લીટી_ga;
    char_u	*pend;
    પૂર્ણાંક		startcol = 0;
    પૂર્ણાંક		c1 = 0;
    પૂર્ણાંક		escaped = FALSE;	// CTRL-V typed
    પૂર્ણાંક		vcol = 0;
    char_u	*p;
    પૂર્ણાંક		prev_char;
    પૂર્ણાંક		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    જો ((સંદેશ_col || સંદેશ_didout) && promptc != '?')
	સંદેશ_putchar('\n');
    જો (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	જો (p_prompt)
	    સંદેશ_putchar(':');
	જ્યારે (indent-- > 0)
	    સંદેશ_putchar(' ');
	startcol = સંદેશ_col;
    }

    ga_init2(&લીટી_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    જો (promptc <= 0)
    {
	vcol = indent;
	જ્યારે (indent >= 8)
	{
	    ga_append(&લીટી_ga, TAB);
	    સંદેશ_puts("        ");
	    indent -= 8;
	}
	જ્યારે (indent-- > 0)
	{
	    ga_append(&લીટી_ga, ' ');
	    સંદેશ_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_ચાવીઓ;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    જ્યારે (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	જો (ga_grow(&લીટી_ga, 40) == FAIL)
	    તોડો;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	જો (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	નહીંતર
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	જો (got_int)
	{
	    સંદેશ_putchar('\n');
	    તોડો;
	}

	જો (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &લીટી_ga);
	    જાઓ redraw;
	}

	જો (!escaped)
	{
	    // CR typed means "enter", which is NL
	    જો (c1 == '\r')
		c1 = '\n';

	    જો (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		જો (લીટી_ga.ga_len > 0)
		{
		    જો (has_mbyte)
		    {
			p = (char_u *)લીટી_ga.ga_data;
			p[લીટી_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + લીટી_ga.ga_len - 1) + 1;
			લીટી_ga.ga_len -= len;
		    }
		    નહીંતર
			--લીટી_ga.ga_len;
		    જાઓ redraw;
		}
		ચાલુ;
	    }

	    જો (c1 == Ctrl_U)
	    {
		સંદેશ_col = startcol;
		સંદેશ_clr_eos();
		લીટી_ga.ga_len = 0;
		જાઓ redraw;
	    }

	    જો (c1 == Ctrl_T)
	    {
		sw = get_sw_કિંમત(curbuf);
		p = (char_u *)લીટી_ga.ga_data;
		p[લીટી_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		જ્યારે (get_indent_str(p, 8, FALSE) < indent)
		{
		    (શૂન્ય)ga_grow(&લીટી_ga, 2);  // one more for the NUL
		    p = (char_u *)લીટી_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, લીટી_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++લીટી_ga.ga_len;
		}
redraw:
		// redraw the line
		સંદેશ_col = startcol;
		vcol = 0;
		p = (char_u *)લીટી_ga.ga_data;
		p[લીટી_ga.ga_len] = NUL;
		જ્યારે (p < (char_u *)લીટી_ga.ga_data + લીટી_ga.ga_len)
		{
		    જો (*p == TAB)
		    {
			કરો
			    સંદેશ_putchar(' ');
			જ્યારે (++vcol % 8);
			++p;
		    }
		    નહીંતર
		    {
			len = mb_ptr2len(p);
			સંદેશ_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		સંદેશ_clr_eos();
		windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
		ચાલુ;
	    }

	    જો (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)લીટી_ga.ga_data;
		જો (prev_char == '0' || prev_char == '^')
		{
		    જો (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--લીટી_ga.ga_len] = NUL;
		}
		નહીંતર
		{
		    p[લીટી_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    જો (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_કિંમત(curbuf);
		    }
		}
		જ્યારે (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, લીટી_ga.ga_len - (s - p) + 1);
		    --લીટી_ga.ga_len;
		}
		જાઓ add_indent;
	    }

	    જો (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		ચાલુ;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    જો (IS_SPECIAL(c1))
		ચાલુ;
	}

	જો (IS_SPECIAL(c1))
	    c1 = '?';
	જો (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)લીટી_ga.ga_data + લીટી_ga.ga_len);
	નહીંતર
	{
	    len = 1;
	    ((char_u *)લીટી_ga.ga_data)[લીટી_ga.ga_len] = c1;
	}
	જો (c1 == '\n')
	    સંદેશ_putchar('\n');
	નહીંતર જો (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    કરો
		સંદેશ_putchar(' ');
	    જ્યારે (++vcol % 8);
	}
	નહીંતર
	{
	    સંદેશ_outtrans_len(
		     ((char_u *)લીટી_ga.ga_data) + લીટી_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	લીટી_ga.ga_len += len;
	escaped = FALSE;

	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
	pend = (char_u *)(લીટી_ga.ga_data) + લીટી_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	જો (લીટી_ga.ga_len > 0 && pend[-1] == '\n'
		&& (લીટી_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --લીટી_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    તોડો;
	}
    }

    --no_mapping;
    --allow_ચાવીઓ;

    // make following messages go to the next line
    સંદેશ_didout = FALSE;
    સંદેશ_col = 0;
    જો (સંદેશ_row < Rows - 1)
	++સંદેશ_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    જો (got_int)
	ga_clear(&લીટી_ga);

    પાછા (char_u *)લીટી_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    પૂર્ણાંક
cmdline_overstrike(શૂન્ય)
{
    પાછા ccline.overstrike;
}

#જો defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    પૂર્ણાંક
cmdline_at_end(શૂન્ય)
{
    પાછા (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#જો defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(શૂન્ય)
{
    જો (ccline.cmdbuff == ખાલી || ccline.cmdpos > ccline.cmdlen)
	પાછા MAXCOL;

    જો (has_mbyte)
    {
	colnr_T	col;
	પૂર્ણાંક	i = 0;

	માટે (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	પાછા col;
    }
    નહીંતર
	પાછા ccline.cmdpos;
}
#endif

#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    સ્થિર શૂન્ય
redrawcmd_preedit(શૂન્ય)
{
    જો ((State & MODE_CMDLINE)
	    && xic != ખાલી
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	પૂર્ણાંક	cmdpos = 0;
	પૂર્ણાંક	cmdspos;
	પૂર્ણાંક	old_row;
	પૂર્ણાંક	old_col;
	colnr_T	col;

	old_row = સંદેશ_row;
	old_col = સંદેશ_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	જો (has_mbyte)
	{
	    માટે (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	નહીંતર
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	સંદેશ_row = cmdline_row + (cmdspos / cmdline_width);
	સંદેશ_col = cmdspos % cmdline_width;
	જો (સંદેશ_row >= Rows)
	    સંદેશ_row = Rows - 1;

	માટે (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    પૂર્ણાંક char_len;
	    પૂર્ણાંક char_attr;

	    char_attr = im_get_feedback_attr(col);
	    જો (char_attr < 0)
		તોડો; // end of preedit string

	    જો (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    નહીંતર
		char_len = 1;

	    સંદેશ_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	સંદેશ_row = old_row;
	સંદેશ_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    સ્થિર શૂન્ય
dealloc_cmdbuff(શૂન્ય)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    સ્થિર શૂન્ય
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    જો (len < 80)
	len = 100;
    નહીંતર
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (પૂર્ણાંક)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
realloc_cmdbuff(પૂર્ણાંક len)
{
    char_u	*p;
    પૂર્ણાંક		plen;

    જો (len < ccline.cmdbufflen)
	પાછા OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    જો (ccline.cmdbuff == ખાલી)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	પાછા FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    જો (ccline.xpc != ખાલી
	    && ccline.xpc->xp_pattern != ખાલી
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	પૂર્ણાંક i = (પૂર્ણાંક)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	જો (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    પાછા OK;
}

#જો defined(FEAT_ARABIC)
સ્થિર char_u	*arshape_buf = ખાલી;

# જો defined(EXITFREE)
    શૂન્ય
free_arshape_buf(શૂન્ય)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    સ્થિર શૂન્ય
draw_cmdline(પૂર્ણાંક start, પૂર્ણાંક len)
{
#જો defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    પૂર્ણાંક		i;

    જો (cmdline_star > 0)
	માટે (i = 0; i < len; ++i)
	{
	    સંદેશ_putchar('*');
	    જો (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    નહીંતર
#endif
#ifdef FEAT_ARABIC
	જો (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	સ્થિર પૂર્ણાંક	buflen = 0;
	char_u		*p;
	પૂર્ણાંક		j;
	પૂર્ણાંક		newlen = 0;
	પૂર્ણાંક		mb_l;
	પૂર્ણાંક		pc, pc1 = 0;
	પૂર્ણાંક		prev_c = 0;
	પૂર્ણાંક		prev_c1 = 0;
	પૂર્ણાંક		u8c;
	પૂર્ણાંક		u8cc[MAX_MCO];
	પૂર્ણાંક		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	જો (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    જો (arshape_buf == ખાલી)
		પાછા;	// out of memory
	}

	જો (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	માટે (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    જો (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		જો (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    જો (j + mb_l >= start + len)
			nc = NUL;
		    નહીંતર
			nc = utf_ptr2char(p + mb_l);
		}
		નહીંતર
		{
		    // displaying from left to right
		    જો (j + mb_l >= start + len)
			pc = NUL;
		    નહીંતર
		    {
			પૂર્ણાંક	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ખાલી, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		જો (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    જો (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    નહીંતર
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	સંદેશ_outtrans_len(arshape_buf, newlen);
    }
    નહીંતર
#endif
	સંદેશ_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    શૂન્ય
putcmdline(પૂર્ણાંક c, પૂર્ણાંક shift)
{
    જો (cmd_silent)
	પાછા;
    સંદેશ_no_more = TRUE;
    સંદેશ_putchar(c);
    જો (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    સંદેશ_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    શૂન્ય
unputcmdline(શૂન્ય)
{
    જો (cmd_silent)
	પાછા;
    સંદેશ_no_more = TRUE;
    જો (ccline.cmdlen == ccline.cmdpos)
	સંદેશ_putchar(' ');
    નહીંતર જો (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    નહીંતર
	draw_cmdline(ccline.cmdpos, 1);
    સંદેશ_no_more = FALSE;
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
    પૂર્ણાંક
put_on_cmdline(char_u *str, પૂર્ણાંક len, પૂર્ણાંક redraw)
{
    પૂર્ણાંક		retval;
    પૂર્ણાંક		i;
    પૂર્ણાંક		m;
    પૂર્ણાંક		c;

    જો (len < 0)
	len = (પૂર્ણાંક)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    જો (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    નહીંતર
	retval = OK;
    જો (retval == OK)
    {
	જો (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	નહીંતર
	{
	    જો (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		માટે (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		માટે (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		જો (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		નહીંતર
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    નહીંતર જો (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	જો (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    જ્યારે (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    જો (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		જો (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		નહીંતર
		    i = 0;
	    }
#endif
	    જો (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		સંદેશ_col -= i;
		જો (સંદેશ_col < 0)
		{
		    સંદેશ_col += cmdline_width;
		    --સંદેશ_row;
		}
	    }
	}

	જો (redraw && !cmd_silent)
	{
	    સંદેશ_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    જો (cmdline_row != i || ccline.overstrike)
		સંદેશ_clr_eos();
	    સંદેશ_no_more = FALSE;
	}
	જો (ચાવી_Typed)
	{
	    m = cmdline_width * Rows;
	    જો (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	નહીંતર
	    m = MAXCOL;
	માટે (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    જો (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    જો (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    જો (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		જો (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    જો (redraw)
	સંદેશ_check();
    પાછા retval;
}

સ્થિર cmdline_info_T	prev_ccline;
સ્થિર પૂર્ણાંક		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    સ્થિર શૂન્ય
save_cmdline(cmdline_info_T *ccp)
{
    જો (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ખાલી;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    સ્થિર શૂન્ય
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
    સ્થિર પૂર્ણાંક
cmdline_paste(
    પૂર્ણાંક regname,
    પૂર્ણાંક literally,	// Insert text literally instead of "as typed"
    પૂર્ણાંક remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    પૂર્ણાંક			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    જો (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	પાછા FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    લીટી_breakcheck();
    જો (got_int)
	પાછા FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    જો (i)
    {
	// Got the value of a special register in "arg".
	જો (arg == ખાલી)
	    પાછા FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	જો (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    પૂર્ણાંક	    len;

	    // Locate start of last word in the cmd buffer.
	    માટે (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		જો (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    જો (!vim_iswordc(mb_ptr2char(w - len)))
			તોડો;
		    w -= len;
		}
		નહીંતર
		{
		    જો (!vim_iswordc(w[-1]))
			તોડો;
		    --w;
		}
	    }
	    len = (પૂર્ણાંક)((ccline.cmdbuff + ccline.cmdpos) - w);
	    જો (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	જો (allocated)
	    vim_free(arg);
	પાછા OK;
    }

    પાછા cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    શૂન્ય
cmdline_paste_str(char_u *s, પૂર્ણાંક literally)
{
    પૂર્ણાંક		c, cv;

    જો (literally)
	put_on_cmdline(s, -1, TRUE);
    નહીંતર
	જ્યારે (*s != NUL)
	{
	    cv = *s;
	    જો (cv == Ctrl_V && s[1])
		++s;
	    જો (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    નહીંતર
		c = *s++;
	    જો (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    શૂન્ય
redrawcmdline(શૂન્ય)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    શૂન્ય
redrawcmdline_ex(પૂર્ણાંક do_compute_cmdrow)
{
    જો (cmd_silent)
	પાછા;
    need_wait_return = FALSE;
    જો (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    સ્થિર શૂન્ય
redrawcmdprompt(શૂન્ય)
{
    પૂર્ણાંક		i;

    જો (cmd_silent)
	પાછા;
    જો (ccline.cmdfirstc != NUL)
	સંદેશ_putchar(ccline.cmdfirstc);
    જો (ccline.cmdprompt != ખાલી)
    {
	સંદેશ_puts_attr((અક્ષર *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = સંદેશ_col + (સંદેશ_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	જો (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    નહીંતર
	માટે (i = ccline.cmdindent; i > 0; --i)
	    સંદેશ_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    શૂન્ય
redrawcmd(શૂન્ય)
{
    પૂર્ણાંક save_in_echowindow = in_echowindow;

    જો (cmd_silent)
	પાછા;

    // when 'incsearch' is set there may be no command line while redrawing
    જો (ccline.cmdbuff == ખાલી)
    {
	windgoto(cmdline_row, cmdline_col_off);
	સંદેશ_clr_eos();
	પાછા;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    સંદેશ_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    સંદેશ_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    સંદેશ_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    સંદેશ_clr_eos();
    સંદેશ_no_more = FALSE;

    set_cmdspos_cursor();
    જો (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    સંદેશ_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    શૂન્ય
compute_cmdrow(શૂન્ય)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    જો (exmode_active || (સંદેશ_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    નહીંતર
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    શૂન્ય
cursorcmd(શૂન્ય)
{
    જો (cmd_silent)
	પાછા;

#ifdef FEAT_RIGHTLEFT
    જો (cmdmsg_rl)
    {
	સંદેશ_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	સંદેશ_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	જો (સંદેશ_row <= 0)
	    સંદેશ_row = Rows - 1;
    }
    નહીંતર
#endif
    {
	સંદેશ_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	સંદેશ_col = ccline.cmdspos % cmdline_width;
	જો (સંદેશ_row >= Rows)
	    સંદેશ_row = Rows - 1;
    }

    windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    જો (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    શૂન્ય
gotocmdline(પૂર્ણાંક clr)
{
    સંદેશ_start();
#ifdef FEAT_RIGHTLEFT
    જો (cmdmsg_rl)
	સંદેશ_col = cmdline_width - 1;
    નહીંતર
#endif
	સંદેશ_col = 0;	    // always start in column 0
    જો (clr)		    // clear the bottom line(s)
	સંદેશ_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    સ્થિર પૂર્ણાંક
ccheck_abbr(પૂર્ણાંક c)
{
    પૂર્ણાંક spos = 0;

    જો (p_paste || no_abbr)	    // no abbreviations or in paste mode
	પાછા FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    જ્યારે (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    જો (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    નહીંતર
	// check abbreviation from the beginning of the commandline
	spos = 0;

    પાછા check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, પૂર્ણાંક what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    પૂર્ણાંક		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    માટે (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	જો ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#નહીંતર
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    જો (what == VSE_SHELL && csh_like_shell() && p != ખાલી)
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
    જો (p != ખાલી && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    પાછા p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    શૂન્ય
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    જો (p == ખાલી)
	પાછા;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    શૂન્ય
tilde_replace(
    char_u  *orig_pat,
    પૂર્ણાંક	    અંક_files,
    char_u  **files)
{
    પૂર્ણાંક	    i;
    char_u  *p;

    જો (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	માટે (i = 0; i < અંક_files; ++i)
	{
	    p = home_replace_save(ખાલી, files[i]);
	    જો (p != ખાલી)
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
get_cmdline_info(શૂન્ય)
{
    પાછા &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    સ્થિર cmdline_info_T *
get_ccline_ptr(શૂન્ય)
{
    જો ((State & MODE_CMDLINE) == 0)
	પાછા ખાલી;
    જો (ccline.cmdbuff != ખાલી)
	પાછા &ccline;
    જો (prev_ccline_used && prev_ccline.cmdbuff != ખાલી)
	પાછા &prev_ccline;
    પાછા ખાલી;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    સ્થિર પૂર્ણાંક
get_cmdline_type(શૂન્ય)
{
    cmdline_info_T *p = get_ccline_ptr();

    જો (p == ખાલી)
	પાછા NUL;
    જો (p->cmdfirstc == NUL)
	પાછા
#ifdef FEAT_EVAL
	    (p->આગત_fn) ? '@' :
#endif
	    '-';
    પાછા p->cmdfirstc;
}

#જો defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    સ્થિર char_u *
get_cmdline_str(શૂન્ય)
{
    cmdline_info_T *p;

    જો (cmdline_star > 0)
	પાછા ખાલી;
    p = get_ccline_ptr();
    જો (p == ખાલી)
	પાછા ખાલી;
    પાછા vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    સ્થિર char_u *
get_cmdline_completion_pattern(શૂન્ય)
{
    cmdline_info_T *p;
    પૂર્ણાંક		xp_context;

    જો (cmdline_star > 0)
	પાછા ખાલી;

    p = get_ccline_ptr();
    જો (p == ખાલી || p->xpc == ખાલી)
	પાછા ખાલી;

    xp_context = p->xpc->xp_context;
    જો (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    જો (xp_context == EXPAND_UNSUCCESSFUL)
	પાછા ખાલી;

    char_u *compl_pat = p->xpc->xp_pattern;

    જો (compl_pat == ખાલી)
	પાછા ખાલી;

    પાછા vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    સ્થિર char_u *
get_cmdline_completion(શૂન્ય)
{
    cmdline_info_T	*p;
    પૂર્ણાંક			xp_context;

    જો (cmdline_star > 0)
	પાછા ખાલી;

    p = get_ccline_ptr();
    જો (p == ખાલી || p->xpc == ખાલી)
	પાછા ખાલી;

    xp_context = p->xpc->xp_context;
    જો (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    જો (xp_context == EXPAND_UNSUCCESSFUL)
	પાછા ખાલી;

    પાછા cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    શૂન્ય
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    શૂન્ય
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    શૂન્ય
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    શૂન્ય
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_સંખ્યા = p != ખાલી ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    શૂન્ય
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ખાલી && p->cmdprompt != ખાલી ?
					    vim_strsave(p->cmdprompt) : ખાલી;
}

/*
 * "getcmdscreenpos()" function
 */
    શૂન્ય
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_સંખ્યા = p != ખાલી ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    શૂન્ય
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    જો (rettv->vval.v_string == ખાલી)
	પાછા;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    સ્થિર પૂર્ણાંક
set_cmdline_str(char_u *str, પૂર્ણાંક pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    પૂર્ણાંક		    len;

    જો (p == ખાલી)
	પાછા 1;

    len = (પૂર્ણાંક)STRLEN(str);
    જો (realloc_cmdbuff(len + 1) != OK)
	પાછા 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    પાછા 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    સ્થિર પૂર્ણાંક
set_cmdline_pos(
    પૂર્ણાંક		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    જો (p == ખાલી)
	પાછા 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    જો (pos < 0)
	new_cmdpos = 0;
    નહીંતર
	new_cmdpos = pos;

    પાછા 0;
}

// "setcmdline()" function
    શૂન્ય
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક pos = -1;

    જો (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક error = FALSE;

	pos = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], &error) - 1;
	જો (error)
	    પાછા;
	જો (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    પાછા;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_સંખ્યા = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    શૂન્ય
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		pos;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    pos = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]) - 1;
    જો (pos >= 0)
	rettv->vval.v_સંખ્યા = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    પૂર્ણાંક
get_cmdline_firstc(શૂન્ય)
{
    પાછા ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    પૂર્ણાંક
get_list_range(char_u **str, પૂર્ણાંક *num1, પૂર્ણાંક *num2)
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		first = FALSE;
    varnumber_T	અંક;

    *str = skipwhite(*str);
    જો (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ખાલી, &len, 0, &અંક, ખાલી, 0, FALSE, ખાલી);
	*str += len;
	// overflow
	જો (અંક > INT_MAX)
	    પાછા FAIL;

	*num1 = (પૂર્ણાંક)અંક;
	first = TRUE;
    }
    *str = skipwhite(*str);
    જો (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ખાલી, &len, 0, &અંક, ખાલી, 0, FALSE, ખાલી);
	જો (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    જો (અંક > INT_MAX)
		પાછા FAIL;

	    *num2 = (પૂર્ણાંક)અંક;
	}
	નહીંતર જો (!first)		// no number given at all
	    પાછા FAIL;
    }
    નહીંતર જો (first)			// only one number given
	*num2 = *num1;
    પાછા OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    અક્ષર *
did_set_cedit(optset_T *args UNUSED)
{
    પૂર્ણાંક n;

    જો (*p_cedit == NUL)
	cedit_ચાવી = -1;
    નહીંતર
    {
	n = string_to_ચાવી(p_cedit, FALSE);
	જો (n == 0 || vim_isprintc(n))
	    પાછા e_invalid_argument;
	cedit_ચાવી = n;
    }
    પાછા ખાલી;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    સ્થિર પૂર્ણાંક
open_cmdwin(શૂન્ય)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    પૂર્ણાંક			i;
    linenr_T		lnum;
    પૂર્ણાંક			histtype;
    garray_T		winsizes;
    પૂર્ણાંક			save_restart_edit = restart_edit;
    પૂર્ણાંક			save_State = State;
    પૂર્ણાંક			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    પૂર્ણાંક			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    પૂર્ણાંક			save_ચાવી_Typed;
#endif
    પૂર્ણાંક			newbuf_status;
    પૂર્ણાંક			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    જો (text_or_buf_locked()
	    || cmdwin_type != 0
#જો defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	પાછા K_IGNORE;
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
    જો (win_split((પૂર્ણાંક)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	પાછા K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    જો (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	પાછા Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, ખાલી, ખાલી, ખાલી, ECMD_ONE, ECMD_HIDE,
					ખાલી);
    cmdwin_valid = win_valid(cmdwin_win);
    જો (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	જો (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	જો (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	જો (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(ખાલી, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ખાલી;
	beep_flush();
	ga_clear(&winsizes);
	પાછા Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_કિંમત_give_err((char_u *)"bt",
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
    જો (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	જો (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_ઉમેરો, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_ઉમેરો, TRUE);
	}
	set_option_કિંમત_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    જો (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	જો (i >= 0)
	{
	    lnum = 0;
	    કરો
	    {
		જો (++i == get_hislen())
		    i = 0;
		જો (get_histentry(histtype)[i].hisstr != ખાલી)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    જ્યારે (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_લીટી_ગણતરી, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_લીટી_abv_curs();
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
    જો (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_ચાવી_Typed = ચાવી_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    ચાવી_Typed = save_ચાવી_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = ખાલી;
    cmdwin_win = ખાલી;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    જો (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    નહીંતર
    {
#જો defined(FEAT_EVAL)
	// autocmds may abort script processing
	જો (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	જો (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    જો (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    જો (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (પૂર્ણાંક)plen;
		ccline.cmdbufflen = (પૂર્ણાંક)(plen + 1);
		cmdwin_result = CAR;
	    }
	    નહીંતર
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	નહીંતર જો (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ખાલી;
	}
	નહીંતર
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	જો (ccline.cmdbuff == ખાલી)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	નહીંતર
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    જો (ccline.cmdpos == ccline.cmdlen - 1
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
	જો (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	જો (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(ખાલી, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	જો (cmdwin_result == K_IGNORE)
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

    પાછા cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    પૂર્ણાંક
is_in_cmdwin(શૂન્ય)
{
    પાછા cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    જો (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ખાલી)
	પાછા ખાલી;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    જો (l == ખાલી)
	પાછા ખાલી;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    પાછા (char_u *)ga.ga_data;
#નહીંતર
    પાછા ખાલી;
#endif
}

#જો defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    શૂન્ય
get_user_આગત(
    typval_T	*argvars,
    typval_T	*rettv,
    પૂર્ણાંક		inputdialog,
    પૂર્ણાંક		secret)
{
    char_u	*prompt;
    char_u	*p = ખાલી;
    પૂર્ણાંક		c;
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    પૂર્ણાંક		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ખાલી;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
    જો (આગત_busy)
	પાછા;  // this doesn't work recursively.

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	પાછા;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_આગત
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    જો (no_console_આગત() && !is_not_a_term())
	પાછા;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    જો (prompt != ખાલી)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	જો (p == ખાલી)
	    p = prompt;
	નહીંતર
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    સંદેશ_start();
	    સંદેશ_clr_eos();
	    સંદેશ_puts_attr((અક્ષર *)prompt, get_echo_attr());
	    સંદેશ_didout = FALSE;
	    સંદેશ_starthere();
	    *p = c;
	}
	cmdline_row = સંદેશ_row;

	જો (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    જો (defstr != ખાલી)
		stuffReadbuffSpec(defstr);

	    જો (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		પૂર્ણાંક	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ખાલી;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		જો (xp_name == ખાલી)
		    પાછા;

		xp_namelen = (પૂર્ણાંક)STRLEN(xp_name);

		જો (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    પાછા;
	    }
	}

	જો (defstr != ખાલી)
	{
	    પૂર્ણાંક save_ex_normal_busy = ex_normal_busy;
	    પૂર્ણાંક save_vgetc_busy = vgetc_busy;
	    પૂર્ણાંક save_આગત_busy = આગત_busy;

	    આગત_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    આગત_busy = save_આગત_busy;
	}
	જો (inputdialog && rettv->vval.v_string == ખાલી
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	સંદેશ_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    શૂન્ય
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    જો (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	પાછા;

    પૂર્ણાંક cmd_type = get_cmdline_type();

    જો (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	ચાવી_string[4];

	ચાવી_string[0] = K_SPECIAL;
	ચાવી_string[1] = KS_EXTRA;
	ચાવી_string[2] = KE_WILD;
	ચાવી_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(ચાવી_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
