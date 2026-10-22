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

#ଅନ୍ତର୍ଭାଵ "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_ଚାବି	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ସ୍ଥିର cmdline_info_T ccline;

#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	new_cmdpos;	// position set by set_cmdline_pos()
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

ସ୍ଥିର char_u	*getcmdline_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା firstc, long ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clear_ccline);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmdline_charsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ	set_cmdspos(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	set_cmdspos_cursor(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	correct_cmdspos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells);
ସ୍ଥିର ଶୂନ୍ୟ	dealloc_cmdbuff(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	alloc_cmdbuff(size_t len);
ସ୍ଥିର ଶୂନ୍ୟ	draw_cmdline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ଶୂନ୍ୟ	save_cmdline(cmdline_info_T *ccp);
ସ୍ଥିର ଶୂନ୍ୟ	restore_cmdline(cmdline_info_T *ccp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmdline_paste(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା literally, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remcr);
ସ୍ଥିର ଶୂନ୍ୟ	redrawcmdprompt(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ccheck_abbr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	open_cmdwin(ଶୂନ୍ୟ);
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cedit_ଚାବି = -1;	// key value of 'cedit' option

    ସ୍ଥିର ଶୂନ୍ୟ
trigger_cmd_autocmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା typechar, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
abandon_cmdline(ଶୂନ୍ୟ)
{
    dealloc_cmdbuff();
    ଯଦି (ସନ୍ଦେଶ_scrolled == 0)
	compute_cmdrow();
    ସନ୍ଦେଶ("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
empty_pattern(char_u *p, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା delim)
{
    magic_T	magic_val = MAGIC_ON;

    ଯଦି (len > 0)
	(ଶୂନ୍ୟ) skip_regexp_ex(p, delim, magic_isset(), ଖାଲି, ଖାଲି, &magic_val);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ empty_pattern_magic(p, len, magic_val);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    ଯେତେବେଳେ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ଖାଲି)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ଫେରନ୍ତୁ len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    ସ୍ଥିର ଶୂନ୍ୟ
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

    ସ୍ଥିର ଶୂନ୍ୟ
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    pos_T	ସନ୍ଧାନ_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_incsearch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ସ୍ଥିର ଶୂନ୍ୟ
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->match_start = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->match_end);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->ସନ୍ଧାନ_start = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_ସନ୍ଧାନ_match(pos_T *t)
{
    t->lnum += ସନ୍ଧାନ_match_ଧାଡ଼ିଗୁଡ଼ିକ;
    t->col = ସନ୍ଧାନ_match_endcol;
    ଯଦି (t->lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	t->lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_pattern_and_range(
	pos_T	*incsearch_start,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ସନ୍ଧାନ_delim,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*skiplen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim_optional = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_last_pat;
    magic_T     magic = 0;
    ଅକ୍ଷର	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    ସନ୍ଧାନ_first_ଧାଡ଼ି = 0;
    ସନ୍ଧାନ_last_ଧାଡ଼ି = MAXLNUM;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_ଧାଡ଼ିଗୁଡ଼ିକ;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ଖାଲି);

    ଯଦି (vim_strchr((char_u *)"sgvlu", *cmd) == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // Skip over command name to find pattern separator
    ପାଇଁ (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    ଯଦି (*skipwhite(p) == NUL)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	ଯଦି (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	ଅନ୍ୟଥା ଯଦି (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	ଯଦି (*p == '!')
	    p = skipwhite(p + 1);
	ଯେତେବେଳେ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	ଯଦି (*p == NUL)
	    ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	ଯଦି (*p == '!')
	{
	    p++;
	    ଯଦି (*skipwhite(p) == NUL)
		ଫେରନ୍ତୁ FALSE;
	}
	ଯଦି (*cmd != 'g')
	    delim_optional = TRUE;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *ସନ୍ଧାନ_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), ଖାଲି, ଖାଲି, &magic);
    use_last_pat = end == p && *end == delim;

    ଯଦି (end == p && !use_last_pat)
	ଫେରନ୍ତୁ FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    ଯଦି (!use_last_pat)
    {
	ଅକ୍ଷର c = *end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	ଯଦି (empty)
	    ଫେରନ୍ତୁ FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ccline.cmdbuff);
    *patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    ଯଦି (ea.addr_ଗଣନା > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା reverse_match = ea.line2 < ea.line1;
	ସନ୍ଧାନ_first_ଧାଡ଼ି = reverse_match ? ea.line2 : ea.line1;
	ସନ୍ଧାନ_last_ଧାଡ଼ି = reverse_match ? ea.line1 : ea.line2;
    }
    ଅନ୍ୟଥା ଯଦି (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	ସନ୍ଧାନ_first_ଧାଡ଼ି = ସନ୍ଧାନ_last_ଧାଡ଼ି = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_incsearch_highlighting(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    firstc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *ସନ୍ଧାନ_delim,
	incsearch_state_T   *is_state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *skiplen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *patlen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    ଯଦି (!p_is || cmd_silent)
	ଫେରନ୍ତୁ FALSE;

    // By default search all lines
    ସନ୍ଧାନ_first_ଧାଡ଼ି = 0;
    ସନ୍ଧାନ_last_ଧାଡ଼ି = MAXLNUM;

    ଯଦି (firstc == '/' || firstc == '?')
    {
	*ସନ୍ଧାନ_delim = firstc;
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (firstc != ':')
	ଫେରନ୍ତୁ FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->ସନ୍ଧାନ_start, ସନ୍ଧାନ_delim,
	    skiplen, patlen);
    --emsg_off;

    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ଶୂନ୍ୟ
finish_incsearch_highlighting(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା gotesc,
	incsearch_state_T *is_state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା call_update_screen)
{
    ଯଦି (!is_state->did_incsearch)
	ଫେରନ୍ତୁ;

    is_state->did_incsearch = FALSE;
    ଯଦି (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    ଅନ୍ୟଥା
    {
	ଯଦି (!EQUAL_POS(is_state->save_cursor, is_state->ସନ୍ଧାନ_start))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->ସନ୍ଧାନ_start;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_match = FALSE;

    // by default search all lines
    ସନ୍ଧାନ_first_ଧାଡ଼ି = 0;
    ସନ୍ଧାନ_last_ଧାଡ଼ି = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    status_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    ଯଦି (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
may_do_incsearch_highlighting(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    firstc,
	long		    ଗଣନା_2,
	incsearch_state_T   *is_state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skiplen, patlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_last_pat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_do_incsearch = is_state->did_incsearch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_ସନ୍ଧାନ_pattern();

    ଯଦି (!do_incsearch_highlighting(firstc, &ସନ୍ଧାନ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_ସନ୍ଧାନ_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	ଯଦି (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ଫେରନ୍ତୁ;
    }

    // If there is a character waiting, search and redraw later.
    ଯଦି (char_avail())
    {
	restore_last_ସନ୍ଧାନ_pattern();
	is_state->incsearch_postponed = TRUE;
	ଫେରନ୍ତୁ;
    }
    is_state->incsearch_postponed = FALSE;

    ଯଦି (ସନ୍ଧାନ_first_ଧାଡ଼ି == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->ସନ୍ଧାନ_start;
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଧାନ_first_ଧାଡ଼ି > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	curwin->w_cursor.col = MAXCOL;
    }
    ଅନ୍ୟଥା
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = ସନ୍ଧାନ_first_ଧାଡ଼ି;
	curwin->w_cursor.col = 0;
    }

    // Use the previous pattern for ":s//".
    next_char = ccline.cmdbuff[skiplen + patlen];
    use_last_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == next_char;

    // If there is no pattern, don't do anything.
    ଯଦି (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_flags = ସନ୍ଧାନ_OPT + ସନ୍ଧାନ_NOOF + ସନ୍ଧାନ_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	ଯଦି (!p_hls)
	    ସନ୍ଧାନ_flags += ସନ୍ଧାନ_KEEP;
	ଯଦି (ସନ୍ଧାନ_first_ଧାଡ଼ି != 0)
	    ସନ୍ଧାନ_flags += ସନ୍ଧାନ_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_ସନ୍ଧାନ(ଖାଲି, firstc == ':' ? '/' : firstc, ସନ୍ଧାନ_delim,
				 ccline.cmdbuff + skiplen, patlen, ଗଣନା_2, ସନ୍ଧାନ_flags,
# ifdef FEAT_RELTIME
		&sia
# ଅନ୍ୟଥା
		ଖାଲି
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	ଯଦି (curwin->w_cursor.lnum < ସନ୍ଧାନ_first_ଧାଡ଼ି
		|| curwin->w_cursor.lnum > ସନ୍ଧାନ_last_ଧାଡ଼ି)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->ସନ୍ଧାନ_start;
	}

	// if interrupted while searching, behave like it failed
	ଯଦି (got_int)
	{
	    (ଶୂନ୍ୟ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	ଅନ୍ୟଥା ଯଦି (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    ଯଦି (found != 0)
	highlight_match = TRUE;		// highlight position
    ଅନ୍ୟଥା
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    ଯଦି (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_ସନ୍ଧାନ_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    ଅନ୍ୟଥା
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    ଯଦି (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	ଯଦି (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, ସନ୍ଧାନ_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    ଯଦି (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_ସନ୍ଧାନ_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    ଯଦି (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->ସନ୍ଧାନ_start;
    ଅନ୍ୟଥା ଯଦି (found != 0)
	curwin->w_cursor = end_pos;

    ସନ୍ଦେଶ_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_adjust_incsearch_highlighting(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା			firstc,
	long			ଗଣନା_2,
	incsearch_state_T	*is_state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ଖାଲି;
    char_u	*searchstr;
    char_u	*strcopy = ଖାଲି;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_flags = ସନ୍ଧାନ_NOOF;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_ସନ୍ଧାନ_pattern();

    ଯଦି (!do_incsearch_highlighting(firstc, &ସନ୍ଧାନ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_ସନ୍ଧାନ_pattern();
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_ସନ୍ଧାନ_pattern();
	ଫେରନ୍ତୁ FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (ଶୂନ୍ୟ)parse_ସନ୍ଧାନ_pattern_offset(&pat, &patlen_s, ସନ୍ଧାନ_delim,
				    ସନ୍ଧାନ_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    ଯଦି (c == Ctrl_G)
    {
	t = is_state->match_end;
	ଯଦି (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (ଶୂନ୍ୟ)decl(&t);
	ସନ୍ଧାନ_flags += ସନ୍ଧାନ_COL;
    }
    ଅନ୍ୟଥା
	t = is_state->match_start;
    ଯଦି (!p_hls)
	ସନ୍ଧାନ_flags += ସନ୍ଧାନ_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ଖାଲି,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, ଗଣନା_2, ସନ୍ଧାନ_flags, RE_ସନ୍ଧାନ, ଖାଲି);
    --emsg_off;
    ଯଦି (dircp != ଖାଲି)
	*dircp = ସନ୍ଧାନ_delim;
    ଯଦି (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->ସନ୍ଧାନ_start = match_start;
	ଯଦି (!offset.ଧାଡ଼ି && (offset.end || off != 0))
	{
	    ଯଦି (offset.end)
	    {
		is_state->ସନ୍ଧାନ_start = match_end;
		(ଶୂନ୍ୟ)decl(&is_state->ସନ୍ଧାନ_start);
	    }
	    ଯେତେବେଳେ (off > 0)
	    {
		ଯଦି (incl(&is_state->ସନ୍ଧାନ_start) == -1)
		    ଭାଙ୍ଗ;
		--off;
	    }
	    ଯେତେବେଳେ (off < 0)
	    {
		ଯଦି (decl(&is_state->ସନ୍ଧାନ_start) == -1)
		    ଭାଙ୍ଗ;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	ଯଦି (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ସନ୍ଧାନ_start = t;
	    (ଶୂନ୍ୟ)decl(&is_state->ସନ୍ଧାନ_start);
	}
	ଅନ୍ୟଥା ଯଦି (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ସନ୍ଧାନ_start = t;
	    (ଶୂନ୍ୟ)incl(&is_state->ସନ୍ଧାନ_start);
	}
	ଯଦି (LT_POS(t, is_state->ସନ୍ଧାନ_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->ସନ୍ଧାନ_start = t;
	    ଯଦି (firstc == '?')
		(ଶୂନ୍ୟ)incl(&is_state->ସନ୍ଧାନ_start);
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)decl(&is_state->ସନ୍ଧାନ_start);
	}

	set_ସନ୍ଧାନ_match(&is_state->match_end);
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
    ଅନ୍ୟଥା
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_ସନ୍ଧାନ_pattern();
    ଫେରନ୍ତୁ FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_add_char_to_ସନ୍ଧାନ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା firstc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *c, incsearch_state_T *is_state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skiplen, patlen, ସନ୍ଧାନ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_ସନ୍ଧାନ_pattern();

    ଯଦି (!do_incsearch_highlighting(firstc, &ସନ୍ଧାନ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_ସନ୍ଧାନ_pattern();
	ଫେରନ୍ତୁ FAIL;
    }
    restore_last_ସନ୍ଧାନ_pattern();

    // Add a character from under the cursor for 'incsearch'.
    ଯଦି (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	ଯଦି (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    ଯଦି (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    ଯଦି (*c == ସନ୍ଧାନ_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ଖାଲି)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    ଯଦି (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_c = *c;

		ଯେତେବେଳେ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_has_arabic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mb_l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    u8c;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    u8cc[MAX_MCO];

    ଯଦି (!enc_utf8)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	ଯଦି (ARABIC_CHAR(u8c))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

    ଶୂନ୍ୟ
cmdline_init(ଶୂନ୍ୟ)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_handle_ctrl_bsl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *gotesc)
{
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    ଯଦି (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && ଚାବି_Typed)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	ଫେରନ୍ତୁ PROCESS_NEXT_ଚାବି;
    }

#ifdef FEAT_EVAL
    ଯଦି (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	ଯଦି (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	ଅନ୍ୟଥା
	    new_cmdpos = ccline.cmdpos;

	c = get_ପଦସମଷ୍ଟି_register();
	ଯଦି (c == '=')
	{
	    char_u	*p = ଖାଲି;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_ପଦସମଷ୍ଟି_ଧାଡ଼ି();
	    --textlock;

	    ଯଦି (p != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

		ଯଦି (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    ଯଦି (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    ଅନ୍ୟଥା
			ccline.cmdpos = new_cmdpos;

		    ଚାବି_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ଫେରନ୍ତୁ CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ଫେରନ୍ତୁ CMDLINE_NOT_CHANGED;
    }
#endif

    ଯଦି (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ଫେରନ୍ତୁ GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_wildchar_complete(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escape,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*did_wild_list,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*wim_index_p,
	expand_T	*xp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*gotesc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wim_index = *wim_index_p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdpos_before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_NO_BEEP;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    ଯଦି (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    ଯଦି (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	ଯଦି (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (ଶୂନ୍ୟ)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	ଯଦି (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ଅନ୍ୟଥା ଯଦି (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	ଅନ୍ୟଥା
	    res = OK;	    // don't insert 'wildchar' now
    }
    ଅନ୍ୟଥା		    // typed p_wc first time
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_longest = (wim_flags[0] & WIM_LONGEST);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_list = (wim_flags[0] & WIM_LIST);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	ଯଦି (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    ଯଦି (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    ଯଦି (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    ଅନ୍ୟଥା
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	ଯଦି (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ଅନ୍ୟଥା
	{
	    ଯଦି (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    ଯଦି (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	ଯଦି (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	ଯଦି (got_int)
	{
	    (ଶୂନ୍ୟ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (ଶୂନ୍ୟ)ExpandOne(xp, ଖାଲି, ଖାଲି, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    ଫେରନ୍ତୁ CMDLINE_CHANGED;
	}

	// Display matches
	ଯଦି (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    ଯଦି (wim_longest)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_menu = p_wmnu && wim_full;

		ଯଦି (wim_list || show_menu)
		    (ଶୂନ୍ୟ)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		ଅନ୍ୟଥା ଯଦି (!found_longest_prefix)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_list_next = (wim_flags[1] & WIM_LIST);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_full_next = (wim_flags[1] & WIM_FULL);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    ଯଦି (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			ଯଦି (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			ଅନ୍ୟଥା
			    (ଶୂନ୍ୟ)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			ଯଦି (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		ଯଦି (wim_list || show_menu)
		    (ଶୂନ୍ୟ)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		ଅନ୍ୟଥା
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    ଯଦି (wim_list)
		*did_wild_list = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    ଯଦି (wim_index < 3)
	++wim_index;
    ଯଦି (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    ଫେରନ୍ତୁ (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_erase_chars(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    ଯଦି (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    ଯଦି (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    ଯଦି (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    ଯଦି (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	ଯଦି (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    ଯଦି (c == Ctrl_W)
	    {
		ଯେତେବେଳେ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		ଯେତେବେଳେ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		ଯଦି (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (c == Ctrl_W)
	{
	    ଯେତେବେଳେ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    ଯଦି (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		ଯେତେବେଳେ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	ଅନ୍ୟଥା
	    --p;
	ccline.cmdpos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	ଯେତେବେଳେ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ଯଦି (ccline.cmdlen == 0)
	{
	    isp->ସନ୍ଧାନ_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    ଅନ୍ୟଥା ଯଦି (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ଖାଲି && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	ଯଦି (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    ଫେରନ୍ତୁ CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	ଯଦି (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (cmdmsg_rl)
		ସନ୍ଦେଶ_col = cmdline_width;
	    ଅନ୍ୟଥା
#endif
		ସନ୍ଦେଶ_col = 0;
	    ସନ୍ଦେଶ_putchar(' ');		// delete ':'
	}
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	isp->ସନ୍ଧାନ_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	ଫେରନ୍ତୁ GOTO_NORMAL_MODE;
    }
    ଫେରନ୍ତୁ CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cmdline_toggle_langmap(long *b_im_ptr)
{
    ଯଦି (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_ନିବେଶ_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	ଯଦି (b_im_ptr != ଖାଲି)
	{
	    ଯଦି (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    ଅନ୍ୟଥା
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_ନିବେଶ_METHOD
    ଅନ୍ୟଥା
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	ଯଦି ((p_imdisable && b_im_ptr != ଖାଲି)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    ଯଦି (b_im_ptr != ଖାଲି)
		*b_im_ptr = B_IMODE_NONE;
	}
	ଅନ୍ୟଥା
	{
	    im_set_active(TRUE);	// Enable input method
	    ଯଦି (b_im_ptr != ଖାଲି)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    ଯଦି (b_im_ptr != ଖାଲି)
    {
	ଯଦି (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	ଅନ୍ୟଥା
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#ଯଦି defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_ଭର୍ତ୍ତି_reg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *gotesc UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		literally = FALSE;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;
    i = c = plain_vgetc();	// CTRL-R <char>
    ଯଦି (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    ଯଦି (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    ଯଦି (c == '=')
    {
	ଯଦି (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	ଅନ୍ୟଥା
	    c = get_ପଦସମଷ୍ଟି_register();
    }
#endif
    ଯଦି (c != ESC)	    // use ESC to cancel inserting register
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
	ଯଦି (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ଫେରନ୍ତୁ GOTO_NORMAL_MODE;
	}
#endif
	ଚାବି_Typed = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	ଯଦି (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    ଯଦି (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    ଅନ୍ୟଥା
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
    ଫେରନ୍ତୁ literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cmdline_ବାମ_ଡାହାଣ_mouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ignore_drag_release)
{
    ଯଦି (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    ଅନ୍ୟଥା
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    ଯଦି (!gui.in_use)
#endif
	ଯଦି (!mouse_has(MOUSE_COMMAND))
	    ଫେରନ୍ତୁ;
#ifdef FEAT_CLIPBOARD
    ଯଦି (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	ଯଦି (mouse_model_popup() && button == MOUSE_ବାମ
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_ଡାହାଣ;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	ଫେରନ୍ତୁ;
    }
#endif

    set_cmdspos();
    ପାଇଁ (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	i = cmdline_charsize(ccline.cmdpos);
	ଯଦି (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    ଭାଙ୍ଗ;
	ଯଦି (has_mbyte)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_browse_history(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	histype,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*hiscnt_p,
	expand_T *xp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_hiscnt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    ଯଦି (get_hislen() == 0 || firstc == NUL)	// no history
	ଫେରନ୍ତୁ CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    ଯଦି (lookfor == ଖାଲି)
    {
	ଯଦି ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ଖାଲି)
	    ଫେରନ୍ତୁ CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    ପାଇଁ (;;)
    {
	// one step backwards
	ଯଦି (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    ଯଦି (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    ଅନ୍ୟଥା ଯଦି (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    ଅନ୍ୟଥା ଯଦି (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    ଅନ୍ୟଥା			// at top of list
	    {
		hiscnt = orig_hiscnt;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା    // one step forwards
	{
	    // on last entry, clear the line
	    ଯଦି (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		ଭାଙ୍ଗ;
	    }

	    // not on a history line, nothing to do
	    ଯଦି (hiscnt == get_hislen())
		ଭାଙ୍ଗ;
	    ଯଦି (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    ଅନ୍ୟଥା
		++hiscnt;
	}
	ଯଦି (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ଖାଲି)
	{
	    hiscnt = orig_hiscnt;
	    ଭାଙ୍ଗ;
	}
	ଯଦି ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	ଯଦି (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	ଅନ୍ୟଥା
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	ଯଦି (histype == HIST_ସନ୍ଧାନ
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     i;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    ପାଇଁ (i = 0; i <= 1; ++i)
	    {
		len = 0;
		ପାଇଁ (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    ଯଦି (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			ଯଦି (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    ଅନ୍ୟଥା
		    {
			// Escape new sep, unless it is already
			// escaped.
			ଯଦି (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    ଯଦି (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			ଯଦି (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		ଯଦି (i == 0)
		{
		    alloc_cmdbuff(len);
		    ଯଦି (ccline.cmdbuff == ଖାଲି)
		    {
			res = GOTO_NORMAL_MODE;
			ଯାଅ done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
	}
	ଅନ୍ୟଥା
	{
	    alloc_cmdbuff(plen);
	    ଯଦି (ccline.cmdbuff == ଖାଲି)
	    {
		res = GOTO_NORMAL_MODE;
		ଯାଅ done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	ଯାଅ done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ଫେରନ୍ତୁ res;
}

/*
 * Initialize the current command-line info.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_ccline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା firstc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent)
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
    ଯଦି (ccline.cmdbuff == ଖାଲି)
    {
	ccline.cmdbufflen = 0;
	ଫେରନ୍ତୁ FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    ଯଦି (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		  firstc,
    long	  ଗଣନା_2,	// only used for incremental search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    ଫେରନ୍ତୁ getcmdline_int(firstc, ଗଣନା_2, indent, TRUE);
}

    ସ୍ଥିର char_u *
getcmdline_int(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		firstc,
    long	ଗଣନା_2 UNUSED,	// only used for incremental search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent,		// indent for inside conditionals
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clear_ccline)	// clear ccline first
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth = 0;	    // call depth
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gotesc = FALSE;		// TRUE when <ESC> just typed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ଖାଲି;	// string to match
    size_t	lookforlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hiscnt;			// current history line in use
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		histype;		// history type to be used
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    incsearch_state_T	is_state;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_wild_list = FALSE;	// did wild_list() recently
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wim_index = 0;		// index in wim_flags[]
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;	// remember State when called
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		some_ଚାବି_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = ଖାଲି;
    buf_T	*b_im_ptr_buf = ଖାଲି;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_save_ccline = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdline_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wild_type = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    ଯଦି (ccline.cmdbuff != ଖାଲି)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    ଯଦି (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    ଯଦି (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    ଯଦି (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    init_incsearch_state(&is_state);
#endif

    ଯଦି (init_ccline(firstc, indent) != OK)
	ଯାଅ theend;	// out of memory

    ଯଦି (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	ଯାଅ theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    ଅନ୍ୟଥା
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    ଯଦି (!cmd_silent)
    {
	i = ସନ୍ଦେଶ_scrolled;
	ସନ୍ଦେଶ_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	ସନ୍ଦେଶ_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ifndef BACKSLASH_IN_FILENAME
    xpc.xp_shell = FALSE;
#endif

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (ccline.ନିବେଶ_fn)
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
    ସନ୍ଦେଶ_scroll = FALSE;

    State = MODE_CMDLINE;

    ଯଦି (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	ଯଦି (curbuf->b_p_imsearch == B_IMODE_USE_ଭର୍ତ୍ତି)
	    b_im_ptr = &curbuf->b_p_iminsert;
	ଅନ୍ୟଥା
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	ଯଦି (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_ନିବେଶ_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_ନିବେଶ_METHOD
    ଅନ୍ୟଥା ଯଦି (p_imcmdline)
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
    ଯଦି (!debug_mode)
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
    ଯଦି (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    ଯଦି (!cmd_silent && ସନ୍ଦେଶ_scrolled == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ଯଦି (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	ଯଦି (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	ଯଦି (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	trigger_cmdlinechanged = TRUE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	end_wildmenu;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prev_cmdpos = ccline.cmdpos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	skip_pum_redraw = FALSE;

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

	ଯଦି (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_ଚାବି_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	ଯଦି (ccline.cmdbuff != ଖାଲି)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    ଯଦି (prev_cmdbuff == ଖାଲି)
		ଯାଅ returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	ଯଦି (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	କର
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} ଯେତେବେଳେ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	ଯଦି (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    ଯଦି (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (ଶୂନ୍ୟ)ExpandOne(&xpc, ଖାଲି, ଖାଲି, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	ଯଦି (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    ଜାରି;
	}

	ଯଦି (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    clen = ccline.cmdlen;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cc_ଗଣନା = aucmd_cmdline_changed_ଗଣନା;

	    ଯଦି (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		ଯଦି (clen == ccline.cmdlen
				    || cc_ଗଣନା != aucmd_cmdline_changed_ଗଣନା)
		    trigger_cmdlinechanged = FALSE;
		ଯାଅ cmdline_changed;
	    }
	}

	ଯଦି (ଚାବି_Typed)
	{
	    some_ଚାବି_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    ଯଦି (cmd_hkmap)
		c = hkmap(c);
	    ଯଦି (cmdmsg_rl && !ଚାବି_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		ବାଛ (c)
		{
		    ସ୍ଥିତି K_ଡାହାଣ:   c = K_ବାମ; ଭାଙ୍ଗ;
		    ସ୍ଥିତି K_S_ଡାହାଣ: c = K_S_ବାମ; ଭାଙ୍ଗ;
		    ସ୍ଥିତି K_C_ଡାହାଣ: c = K_C_ବାମ; ଭାଙ୍ଗ;
		    ସ୍ଥିତି K_ବାମ:    c = K_ଡାହାଣ; ଭାଙ୍ଗ;
		    ସ୍ଥିତି K_S_ବାମ:  c = K_S_ଡାହାଣ; ଭାଙ୍ଗ;
		    ସ୍ଥିତି K_C_ବାମ:  c = K_C_ଡାହାଣ; ଭାଙ୍ଗ;
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
	ଯଦି ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#ଯଦି defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	ଯଦି (lookfor != ଖାଲି
		&& c != K_S_DOWN && c != K_S_UP
		&& c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ବାମ && c != K_ଡାହାଣ
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	ଯଦି (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	ଯଦି (p_wmnu)
	    c = wildmenu_translate_ଚାବି(&ccline, c, &xpc, did_wild_list);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି_is_wc = (c == p_wc && ଚାବି_Typed) || c == p_wcm;
	ଯଦି ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !ଚାବି_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    ଯଦି (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		ଯଦି (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    ଭାଙ୍ଗ;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	ଯଦି (ଚାବି_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
	    ଯଦି ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	end_wildmenu = (!ଚାବି_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	end_wildmenu = end_wildmenu && (!cmdline_pum_active() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_UP || c == K_S_DOWN)
				 && (mod_mask & MOD_MASK_CTRL))));

	// free expanded names when finished walking through matches
	ଯଦି (end_wildmenu)
	{
	    ଯଦି (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !ଚାବି_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    ଯଦି (xpc.xp_numfiles != -1)
		(ଶୂନ୍ୟ)ExpandOne(&xpc, ଖାଲି, ଖାଲି, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    ଯଦି (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	ଯଦି (p_wmnu)
	    c = wildmenu_process_ଚାବି(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	ଯଦି (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    ଯଦି (res == CMDLINE_CHANGED)
		ଯାଅ cmdline_changed;
	    ଅନ୍ୟଥା ଯଦି (res == CMDLINE_NOT_CHANGED)
		ଯାଅ cmdline_not_changed;
	    ଅନ୍ୟଥା ଯଦି (res == GOTO_NORMAL_MODE)
		ଯାଅ returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	ଯଦି (c == cedit_ଚାବି || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    ଯଦି ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_ଚାବି_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	ଅନ୍ୟଥା
	    c = do_digraph(c);
#endif

	ଯଦି (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!ଚାବି_Typed || vim_strchr(p_cpo, CPO_ESC) != ଖାଲି)))
	{
	    // In Ex mode a backslash escapes a newline.
	    ଯଦି (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		ଯଦି (c == K_KENTER)
		    c = '\n';
	    }
	    ଅନ୍ୟଥା
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		ଯଦି (ccheck_abbr(c + ABBR_OFF))
		    ଯାଅ cmdline_changed;
		ଯଦି (!cmd_silent)
		{
		    windgoto(ସନ୍ଦେଶ_row, cmdline_col_off);
		    out_flush();
		}
		ଭାଙ୍ଗ;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	ଯଦି ((c == p_wc && !gotesc && ଚାବି_Typed) || c == p_wcm || c == K_WILD)
	{
	    ଯଦି (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		    &is_state.ସନ୍ଧାନ_start
#ଅନ୍ୟଥା
		    ଖାଲି
#endif
		    );
	    ଯଦି (c == K_WILD)
		--emsg_silent;
	    ଯଦି (res == CMDLINE_CHANGED)
		ଯାଅ cmdline_changed;
	    ଯଦି (c == K_WILD)
		ଯାଅ cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	ଯଦି (c == K_S_TAB && ଚାବି_Typed)
	{
	    ଯଦି (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		ଯଦି (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		ଯଦି (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    ଯାଅ cmdline_changed;
	    }
	}

	ଯଦି (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	ଯଦି (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	    // Apply search highlighting
	    ଯଦି (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    ଯଦି (ଚାବି_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, ଗଣନା_2, &is_state);
#endif
	    wild_type = 0;
	    ଯାଅ cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	ବାଛ (c)
	{
	ସ୍ଥିତି K_BS:
	ସ୍ଥିତି Ctrl_H:
	ସ୍ଥିତି K_DEL:
	ସ୍ଥିତି K_KDEL:
	ସ୍ଥିତି Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		    , &is_state
#endif
		    );
	    ଯଦି (res == CMDLINE_NOT_CHANGED)
		ଯାଅ cmdline_not_changed;
	    ଅନ୍ୟଥା ଯଦି (res == GOTO_NORMAL_MODE)
		ଯାଅ returncmd;		// back to cmd mode
	    ଯାଅ cmdline_changed;

	ସ୍ଥିତି K_INS:
	ସ୍ଥିତି K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ଖାଲି);
		ଯାଅ cmdline_not_changed;

//	case '@':   only in very old vi
	ସ୍ଥିତି Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		ଯେତେବେଳେ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		ଯଦି (ccline.cmdlen == 0)
		    is_state.ସନ୍ଧାନ_start = is_state.save_cursor;
#endif
		redrawcmd();
		ଯାଅ cmdline_changed;

#ifdef FEAT_CLIPBOARD
	ସ୍ଥିତି Ctrl_Y:
		// Copy the modeless selection, if there is one.
		ଯଦି (clip_star.state != SELECT_CLEARED)
		{
		    ଯଦି (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    ଯାଅ cmdline_not_changed;
		}
		ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି ESC:	// get here if p_wc != ESC or when ESC typed twice
	ସ୍ଥିତି Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		ଯଦି (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    ଯାଅ cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		ଯାଅ returncmd;		// back to cmd mode

	ସ୍ଥିତି Ctrl_R:			// insert register
		res = cmdline_ଭର୍ତ୍ତି_reg(&gotesc);
		ଯଦି (res == GOTO_NORMAL_MODE)
		    ଯାଅ returncmd;
		ଯଦି (res == CMDLINE_CHANGED)
		    ଯାଅ cmdline_changed;
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି Ctrl_D:
		ଯଦି (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    ଭାଙ୍ଗ;	// Use ^D as normal char instead

		redrawcmd();
		ଜାରି;	// don't do incremental search now

	ସ୍ଥିତି K_ଡାହାଣ:
	ସ୍ଥିତି K_S_ଡାହାଣ:
	ସ୍ଥିତି K_C_ଡାହାଣ:
		କର
		{
		    ଯଦି (ccline.cmdpos >= ccline.cmdlen)
			ଭାଙ୍ଗ;
		    i = cmdline_charsize(ccline.cmdpos);
		    ଯଦି (ଚାବି_Typed && ccline.cmdspos + i >= cmdline_width * Rows)
			ଭାଙ୍ଗ;
		    ccline.cmdspos += i;
		    ଯଦି (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    ଅନ୍ୟଥା
			++ccline.cmdpos;
		}
		ଯେତେବେଳେ ((c == K_S_ଡାହାଣ || c == K_C_ଡାହାଣ
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		ଯଦି (has_mbyte)
		    set_cmdspos_cursor();
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି K_ବାମ:
	ସ୍ଥିତି K_S_ବାମ:
	ସ୍ଥିତି K_C_ବାମ:
		ଯଦି (ccline.cmdpos == 0)
		    ଯାଅ cmdline_not_changed;
		କର
		{
		    --ccline.cmdpos;
		    ଯଦି (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		ଯେତେବେଳେ (ccline.cmdpos > 0
			&& (c == K_S_ବାମ || c == K_C_ବାମ
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		ଯଦି (has_mbyte)
		    set_cmdspos_cursor();
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		ଯାଅ cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ସ୍ଥିତି K_F4:
	    ଯଦି (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		ଯାଅ cmdline_not_changed;
	    }
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି K_MIDDLEDRAG:
	ସ୍ଥିତି K_MIDDLERELEASE:
		ଯାଅ cmdline_not_changed;	// Ignore mouse

	ସ୍ଥିତି K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		ଯଦି (!gui.in_use)
#endif
		    ଯଦି (!mouse_has(MOUSE_COMMAND))
			ଯାଅ cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		ଯଦି (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		ଅନ୍ୟଥା
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		ଯାଅ cmdline_changed;

#ifdef FEAT_DND
	ସ୍ଥିତି K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		ଯାଅ cmdline_changed;
#endif

	ସ୍ଥିତି K_LEFTDRAG:
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_RIGHTDRAG:
	ସ୍ଥିତି K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ଯଦି (ignore_drag_release)
		    ଯାଅ cmdline_not_changed;
		// FALLTHROUGH
	ସ୍ଥିତି K_LEFTMOUSE:
	ସ୍ଥିତି K_RIGHTMOUSE:
		cmdline_ବାମ_ଡାହାଣ_mouse(c, &ignore_drag_release);
		ଯାଅ cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	ସ୍ଥିତି K_MOUSEDOWN:
	ସ୍ଥିତି K_MOUSEUP:
	ସ୍ଥିତି K_MOUSELEFT:
	ସ୍ଥିତି K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		ଯଦି (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_ବାମ
				      : MSCR_ଡାହାଣ);
#endif
		ଯାଅ cmdline_not_changed;

	// Alternate buttons ignored here
	ସ୍ଥିତି K_X1MOUSE:
	ସ୍ଥିତି K_X1DRAG:
	ସ୍ଥିତି K_X1RELEASE:
	ସ୍ଥିତି K_X2MOUSE:
	ସ୍ଥିତି K_X2DRAG:
	ସ୍ଥିତି K_X2RELEASE:
	ସ୍ଥିତି K_MOUSEMOVE:
		ଯାଅ cmdline_not_changed;

#ifdef FEAT_GUI
	ସ୍ଥିତି K_LEFTMOUSE_NM:	// mousefocus click, ignored
	ସ୍ଥିତି K_LEFTRELEASE_NM:
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି K_VER_SCROLLBAR:
		ଯଦି (ସନ୍ଦେଶ_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି K_HOR_SCROLLBAR:
		ଯଦି (ସନ୍ଦେଶ_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_ମୂଲ୍ୟ);
		    redrawcmd();
		}
		ଯାଅ cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	ସ୍ଥିତି K_TABLINE:
	ସ୍ଥିତି K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		ଯଦି (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		ଯାଅ cmdline_not_changed;
#endif

	ସ୍ଥିତି K_SELECT:	    // end of Select mode mapping - ignore
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି Ctrl_B:	    // begin of command line
	ସ୍ଥିତି K_HOME:
	ସ୍ଥିତି K_KHOME:
	ସ୍ଥିତି K_S_HOME:
	ସ୍ଥିତି K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି Ctrl_E:	    // end of command line
	ସ୍ଥିତି K_END:
	ସ୍ଥିତି K_KEND:
	ସ୍ଥିତି K_S_END:
	ସ୍ଥିତି K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		ଯାଅ cmdline_not_changed;

	ସ୍ଥିତି Ctrl_A:	    // all matches
		ଯଦି (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		ଯଦି (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    ଭାଙ୍ଗ;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		ଯାଅ cmdline_changed;

	ସ୍ଥିତି Ctrl_L:
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		ଯଦି (may_add_char_to_ସନ୍ଧାନ(firstc, &c, &is_state) == OK)
		    ଯାଅ cmdline_not_changed;
#endif

		// completion: longest common part
		ଯଦି (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    ଭାଙ୍ଗ;
		ଯାଅ cmdline_changed;

	ସ୍ଥିତି Ctrl_N:	    // next match
	ସ୍ଥିତି Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		ଯଦି (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    ଯଦି (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    ଯାଅ cmdline_not_changed;
		}
#endif
		ଯଦି (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    ଯଦି (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ଭାଙ୍ଗ;
		    ଯାଅ cmdline_changed;
		}
		// FALLTHROUGH
	ସ୍ଥିତି K_UP:
	ସ୍ଥିତି K_DOWN:
	ସ୍ଥିତି K_S_UP:
	ସ୍ଥିତି K_S_DOWN:
	ସ୍ଥିତି K_PAGEUP:
	ସ୍ଥିତି K_KPAGEUP:
	ସ୍ଥିତି K_PAGEDOWN:
	ସ୍ଥିତି K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		ଯଦି (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    ଯଦି (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    ଯାଅ cmdline_not_changed;
		}
#endif
		ଯଦି (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    ଯଦି (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    ଯଦି (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ଭାଙ୍ଗ;
		    ଯାଅ cmdline_changed;
		}
		ଅନ୍ୟଥା
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    ଯଦି (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			ଯାଅ cmdline_changed;
		    }
		    ଅନ୍ୟଥା ଯଦି (res == GOTO_NORMAL_MODE)
			ଯାଅ returncmd;
		}
		ଯାଅ cmdline_not_changed;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ସ୍ଥିତି Ctrl_G:	    // next match
	ସ୍ଥିତି Ctrl_T:	    // previous match
		ଯଦି (may_adjust_incsearch_highlighting(
					  firstc, ଗଣନା_2, &is_state, c) == FAIL)
		    ଯାଅ cmdline_not_changed;
		ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି Ctrl_V:
	ସ୍ଥିତି Ctrl_Q:
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
		    ଯଦି (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			ସନ୍ଦେଶ_putchar(' ');
			cursorcmd();
		    }
		}

		ଭାଙ୍ଗ;

#ifdef FEAT_DIGRAPHS
	ସ୍ଥିତି Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		ଯଦି (c != NUL)
		    ଭାଙ୍ଗ;

		redrawcmd();
		ଯାଅ cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	ସ୍ଥିତି Ctrl__:	    // CTRL-_: switch language mode
		ଯଦି (!p_ari)
		    ଭାଙ୍ଗ;
		cmd_hkmap = !cmd_hkmap;
		ଯାଅ cmdline_not_changed;
#endif

	ସ୍ଥିତି K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ଖାଲି);
		ଯାଅ cmdline_changed;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ifdef UNIX
		ଯଦି (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    ଯାଅ returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		ଯଦି (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		ଭାଙ୍ଗ;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	ଯଦି (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    ଯାଅ cmdline_changed;

	/*
	 * put the character in the command line
	 */
	ଯଦି (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_ଚାବି_name(c, mod_mask), -1, TRUE);
	ଅନ୍ୟଥା
	{
	    ଯଦି (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    ଅନ୍ୟଥା
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	ଯାଅ cmdline_changed;

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
	ଯଦି (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ଯଦି (!is_state.incsearch_postponed)
	    ଜାରି;
#endif

cmdline_changed:
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	// If the window changed incremental search state is not valid.
	ଯଦି (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	ଯଦି (xpc.xp_context == EXPAND_NOTHING && (ଚାବି_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, ଗଣନା_2, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	ଯଦି (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ଖାଲି &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    ଯଦି (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	ଯଦି (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	ଯଦି (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    ଯଦି (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    ଯଦି (!event_cmdlineleavepre_triggered)
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
    ଯଦି (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ଖାଲି;
    clear_cmdline_orig();

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    ଯଦି (ccline.cmdbuff != ଖାଲି)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ଯଦି (ccline.cmdlen && firstc != NUL
		&& (some_ଚାବି_typed || histype == HIST_ସନ୍ଧାନ))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_ସନ୍ଧାନ ? firstc : NUL);
	    ଯଦି (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	ଯଦି (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    ସନ୍ଦେଶ_check();
    ସନ୍ଦେଶ_scroll = save_ସନ୍ଦେଶ_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    ଯଦି (some_ଚାବି_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    ଯଦି (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_ନିବେଶ_METHOD
    ଯଦି (b_im_ptr != ଖାଲି && buf_valid(b_im_ptr_buf)
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
	ଯଦି (did_save_ccline)
	    restore_cmdline(&save_ccline);
	ଅନ୍ୟଥା
	    ccline.cmdbuff = ଖାଲି;

	vim_free(prev_cmdbuff);
	ଫେରନ୍ତୁ p;
    }
}

#ଯଦି defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		firstc,
    char_u	*prompt,	// command line prompt
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr,		// attributes for prompt
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			did_save_ccline = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ସନ୍ଦେଶ_col_save = ସନ୍ଦେଶ_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ସନ୍ଦେଶ_silent_save = ସନ୍ଦେଶ_silent;

    ଯଦି (ccline.cmdbuff != ଖାଲି)
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
    ccline.ନିବେଶ_fn = (firstc == '@');
# endif
    ସନ୍ଦେଶ_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    ଯଦି (did_save_ccline)
	restore_cmdline(&save_ccline);

    ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    ଯଦି (ccline.cmdbuff != ଖାଲି)
	ସନ୍ଦେଶ_col = ସନ୍ଦେଶ_col_save;

    ଫେରନ୍ତୁ s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_opt_wim(ଶୂନ୍ୟ)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    ପାଇଁ (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    ପାଇଁ (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	ପାଇଁ (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	ଯଦି (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	ଅନ୍ୟଥା ଯଦି (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	ଅନ୍ୟଥା ଯଦି (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	ଅନ୍ୟଥା ଯଦି (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	ଅନ୍ୟଥା ଯଦି (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	ଅନ୍ୟଥା ଯଦି (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;
	p += i;
	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;
	ଯଦି (*p == ',')
	{
	    ଯଦି (idx == 3)
		ଫେରନ୍ତୁ FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    ଯେତେବେଳେ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    ପାଇଁ (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_locked(ଶୂନ୍ୟ)
{
    ଯଦି (cmdwin_type != 0)
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    ଶୂନ୍ୟ
text_locked_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    emsg(_(get_text_locked_ସନ୍ଦେଶ()));
}

    ଅକ୍ଷର *
get_text_locked_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    ଯଦି (cmdwin_type != 0)
	ଫେରନ୍ତୁ e_invalid_in_cmdline_window;
    ଫେରନ୍ତୁ e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
text_or_buf_locked(ଶୂନ୍ୟ)
{
    ଯଦି (text_locked())
    {
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
curbuf_locked(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
allbuf_locked(ଶୂନ୍ୟ)
{
    ଯଦି (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_charsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
#ଯଦି defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ଯଦି (cmdline_star > 0)	    // showing '*', always 1 position
	ଫେରନ୍ତୁ 1;
#endif
    ଫେରନ୍ତୁ ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_cmdspos(ଶୂନ୍ୟ)
{
    ଯଦି (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    ଅନ୍ୟଥା
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_cmdspos_cursor(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, m, c;

    set_cmdspos();
    ଯଦି (ଚାବି_Typed)
    {
	m = cmdline_width * Rows;
	ଯଦି (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    ଅନ୍ୟଥା
	m = MAXCOL;
    ପାଇଁ (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	ଯଦି (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	ଯଦି ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
correct_cmdspos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells)
{
    ଯଦି ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,		// normally ':', NUL for ":append"
    ଶୂନ୍ୟ	*cookie UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    ଯଦି (exec_from_reg && vpeekc() == ':')
	(ଶୂନ୍ୟ)vgetc();
    ଫେରନ୍ତୁ getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    ଶୂନ୍ୟ	*cookie UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	ଧାଡ଼ି_ga;
    char_u	*pend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaped = FALSE;	// CTRL-V typed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol = 0;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    ଯଦି ((ସନ୍ଦେଶ_col || ସନ୍ଦେଶ_didout) && promptc != '?')
	ସନ୍ଦେଶ_putchar('\n');
    ଯଦି (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	ଯଦି (p_prompt)
	    ସନ୍ଦେଶ_putchar(':');
	ଯେତେବେଳେ (indent-- > 0)
	    ସନ୍ଦେଶ_putchar(' ');
	startcol = ସନ୍ଦେଶ_col;
    }

    ga_init2(&ଧାଡ଼ି_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    ଯଦି (promptc <= 0)
    {
	vcol = indent;
	ଯେତେବେଳେ (indent >= 8)
	{
	    ga_append(&ଧାଡ଼ି_ga, TAB);
	    ସନ୍ଦେଶ_puts("        ");
	    indent -= 8;
	}
	ଯେତେବେଳେ (indent-- > 0)
	{
	    ga_append(&ଧାଡ଼ି_ga, ' ');
	    ସନ୍ଦେଶ_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    ଯେତେବେଳେ (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	ଯଦି (ga_grow(&ଧାଡ଼ି_ga, 40) == FAIL)
	    ଭାଙ୍ଗ;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	ଯଦି (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	ଅନ୍ୟଥା
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	ଯଦି (got_int)
	{
	    ସନ୍ଦେଶ_putchar('\n');
	    ଭାଙ୍ଗ;
	}

	ଯଦି (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &ଧାଡ଼ି_ga);
	    ଯାଅ redraw;
	}

	ଯଦି (!escaped)
	{
	    // CR typed means "enter", which is NL
	    ଯଦି (c1 == '\r')
		c1 = '\n';

	    ଯଦି (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		ଯଦି (ଧାଡ଼ି_ga.ga_len > 0)
		{
		    ଯଦି (has_mbyte)
		    {
			p = (char_u *)ଧାଡ଼ି_ga.ga_data;
			p[ଧାଡ଼ି_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + ଧାଡ଼ି_ga.ga_len - 1) + 1;
			ଧାଡ଼ି_ga.ga_len -= len;
		    }
		    ଅନ୍ୟଥା
			--ଧାଡ଼ି_ga.ga_len;
		    ଯାଅ redraw;
		}
		ଜାରି;
	    }

	    ଯଦି (c1 == Ctrl_U)
	    {
		ସନ୍ଦେଶ_col = startcol;
		ସନ୍ଦେଶ_clr_eos();
		ଧାଡ଼ି_ga.ga_len = 0;
		ଯାଅ redraw;
	    }

	    ଯଦି (c1 == Ctrl_T)
	    {
		sw = get_sw_ମୂଲ୍ୟ(curbuf);
		p = (char_u *)ଧାଡ଼ି_ga.ga_data;
		p[ଧାଡ଼ି_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		ଯେତେବେଳେ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (ଶୂନ୍ୟ)ga_grow(&ଧାଡ଼ି_ga, 2);  // one more for the NUL
		    p = (char_u *)ଧାଡ଼ି_ga.ga_data;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, ଧାଡ଼ି_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++ଧାଡ଼ି_ga.ga_len;
		}
redraw:
		// redraw the line
		ସନ୍ଦେଶ_col = startcol;
		vcol = 0;
		p = (char_u *)ଧାଡ଼ି_ga.ga_data;
		p[ଧାଡ଼ି_ga.ga_len] = NUL;
		ଯେତେବେଳେ (p < (char_u *)ଧାଡ଼ି_ga.ga_data + ଧାଡ଼ି_ga.ga_len)
		{
		    ଯଦି (*p == TAB)
		    {
			କର
			    ସନ୍ଦେଶ_putchar(' ');
			ଯେତେବେଳେ (++vcol % 8);
			++p;
		    }
		    ଅନ୍ୟଥା
		    {
			len = mb_ptr2len(p);
			ସନ୍ଦେଶ_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		ସନ୍ଦେଶ_clr_eos();
		windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
		ଜାରି;
	    }

	    ଯଦି (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)ଧାଡ଼ି_ga.ga_data;
		ଯଦି (prev_char == '0' || prev_char == '^')
		{
		    ଯଦି (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--ଧାଡ଼ି_ga.ga_len] = NUL;
		}
		ଅନ୍ୟଥା
		{
		    p[ଧାଡ଼ି_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    ଯଦି (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_ମୂଲ୍ୟ(curbuf);
		    }
		}
		ଯେତେବେଳେ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, ଧାଡ଼ି_ga.ga_len - (s - p) + 1);
		    --ଧାଡ଼ି_ga.ga_len;
		}
		ଯାଅ add_indent;
	    }

	    ଯଦି (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		ଜାରି;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    ଯଦି (IS_SPECIAL(c1))
		ଜାରି;
	}

	ଯଦି (IS_SPECIAL(c1))
	    c1 = '?';
	ଯଦି (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)ଧାଡ଼ି_ga.ga_data + ଧାଡ଼ି_ga.ga_len);
	ଅନ୍ୟଥା
	{
	    len = 1;
	    ((char_u *)ଧାଡ଼ି_ga.ga_data)[ଧାଡ଼ି_ga.ga_len] = c1;
	}
	ଯଦି (c1 == '\n')
	    ସନ୍ଦେଶ_putchar('\n');
	ଅନ୍ୟଥା ଯଦି (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    କର
		ସନ୍ଦେଶ_putchar(' ');
	    ଯେତେବେଳେ (++vcol % 8);
	}
	ଅନ୍ୟଥା
	{
	    ସନ୍ଦେଶ_outtrans_len(
		     ((char_u *)ଧାଡ଼ି_ga.ga_data) + ଧାଡ଼ି_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	ଧାଡ଼ି_ga.ga_len += len;
	escaped = FALSE;

	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
	pend = (char_u *)(ଧାଡ଼ି_ga.ga_data) + ଧାଡ଼ି_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	ଯଦି (ଧାଡ଼ି_ga.ga_len > 0 && pend[-1] == '\n'
		&& (ଧାଡ଼ି_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --ଧାଡ଼ି_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    ଭାଙ୍ଗ;
	}
    }

    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;

    // make following messages go to the next line
    ସନ୍ଦେଶ_didout = FALSE;
    ସନ୍ଦେଶ_col = 0;
    ଯଦି (ସନ୍ଦେଶ_row < Rows - 1)
	++ସନ୍ଦେଶ_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    ଯଦି (got_int)
	ga_clear(&ଧାଡ଼ି_ga);

    ଫେରନ୍ତୁ (char_u *)ଧାଡ଼ି_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_overstrike(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ccline.overstrike;
}

#ଯଦି defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_at_end(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#ଯଦି defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(ଶୂନ୍ୟ)
{
    ଯଦି (ccline.cmdbuff == ଖାଲି || ccline.cmdpos > ccline.cmdlen)
	ଫେରନ୍ତୁ MAXCOL;

    ଯଦି (has_mbyte)
    {
	colnr_T	col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i = 0;

	ପାଇଁ (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ଫେରନ୍ତୁ col;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ccline.cmdpos;
}
#endif

#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
redrawcmd_preedit(ଶୂନ୍ୟ)
{
    ଯଦି ((State & MODE_CMDLINE)
	    && xic != ଖାଲି
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmdpos = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmdspos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_col;
	colnr_T	col;

	old_row = ସନ୍ଦେଶ_row;
	old_col = ସନ୍ଦେଶ_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	ଯଦି (has_mbyte)
	{
	    ପାଇଁ (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	ଅନ୍ୟଥା
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	ସନ୍ଦେଶ_row = cmdline_row + (cmdspos / cmdline_width);
	ସନ୍ଦେଶ_col = cmdspos % cmdline_width;
	ଯଦି (ସନ୍ଦେଶ_row >= Rows)
	    ସନ୍ଦେଶ_row = Rows - 1;

	ପାଇଁ (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_len;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_attr;

	    char_attr = im_get_feedback_attr(col);
	    ଯଦି (char_attr < 0)
		ଭାଙ୍ଗ; // end of preedit string

	    ଯଦି (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    ଅନ୍ୟଥା
		char_len = 1;

	    ସନ୍ଦେଶ_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	ସନ୍ଦେଶ_row = old_row;
	ସନ୍ଦେଶ_col = old_col;
    }
}
#endif // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dealloc_cmdbuff(ଶୂନ୍ୟ)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    ଯଦି (len < 80)
	len = 100;
    ଅନ୍ୟଥା
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
realloc_cmdbuff(ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		plen;

    ଯଦି (len < ccline.cmdbufflen)
	ଫେରନ୍ତୁ OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    ଯଦି (ccline.cmdbuff == ଖାଲି)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ଫେରନ୍ତୁ FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    ଯଦି (ccline.xpc != ଖାଲି
	    && ccline.xpc->xp_pattern != ଖାଲି
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	ଯଦି (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_ARABIC)
ସ୍ଥିର char_u	*arshape_buf = ଖାଲି;

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_arshape_buf(ଶୂନ୍ୟ)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
draw_cmdline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ଯଦି defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (cmdline_star > 0)
	ପାଇଁ (i = 0; i < len; ++i)
	{
	    ସନ୍ଦେଶ_putchar('*');
	    ଯଦି (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_ARABIC
	ଯଦି (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	buflen = 0;
	char_u		*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlen = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_l;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pc, pc1 = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_c = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_c1 = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u8c;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u8cc[MAX_MCO];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	ଯଦି (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    ଯଦି (arshape_buf == ଖାଲି)
		ଫେରନ୍ତୁ;	// out of memory
	}

	ଯଦି (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	ପାଇଁ (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    ଯଦି (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		ଯଦି (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    ଯଦି (j + mb_l >= start + len)
			nc = NUL;
		    ଅନ୍ୟଥା
			nc = utf_ptr2char(p + mb_l);
		}
		ଅନ୍ୟଥା
		{
		    // displaying from left to right
		    ଯଦି (j + mb_l >= start + len)
			pc = NUL;
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ଖାଲି, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		ଯଦି (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    ଯଦି (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	ସନ୍ଦେଶ_outtrans_len(arshape_buf, newlen);
    }
    ଅନ୍ୟଥା
#endif
	ସନ୍ଦେଶ_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    ଶୂନ୍ୟ
putcmdline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift)
{
    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;
    ସନ୍ଦେଶ_no_more = TRUE;
    ସନ୍ଦେଶ_putchar(c);
    ଯଦି (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    ସନ୍ଦେଶ_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    ଶୂନ୍ୟ
unputcmdline(ଶୂନ୍ୟ)
{
    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;
    ସନ୍ଦେଶ_no_more = TRUE;
    ଯଦି (ccline.cmdlen == ccline.cmdpos)
	ସନ୍ଦେଶ_putchar(' ');
    ଅନ୍ୟଥା ଯଦି (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    ଅନ୍ୟଥା
	draw_cmdline(ccline.cmdpos, 1);
    ସନ୍ଦେଶ_no_more = FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_on_cmdline(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା redraw)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (len < 0)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    ଯଦି (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    ଅନ୍ୟଥା
	retval = OK;
    ଯଦି (retval == OK)
    {
	ଯଦି (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		ପାଇଁ (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		ପାଇଁ (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		ଯଦି (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		ଅନ୍ୟଥା
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    ଅନ୍ୟଥା ଯଦି (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	ଯଦି (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    ଯେତେବେଳେ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    ଯଦି (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ଯଦି (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		ଅନ୍ୟଥା
		    i = 0;
	    }
#endif
	    ଯଦି (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		ସନ୍ଦେଶ_col -= i;
		ଯଦି (ସନ୍ଦେଶ_col < 0)
		{
		    ସନ୍ଦେଶ_col += cmdline_width;
		    --ସନ୍ଦେଶ_row;
		}
	    }
	}

	ଯଦି (redraw && !cmd_silent)
	{
	    ସନ୍ଦେଶ_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    ଯଦି (cmdline_row != i || ccline.overstrike)
		ସନ୍ଦେଶ_clr_eos();
	    ସନ୍ଦେଶ_no_more = FALSE;
	}
	ଯଦି (ଚାବି_Typed)
	{
	    m = cmdline_width * Rows;
	    ଯଦି (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	ଅନ୍ୟଥା
	    m = MAXCOL;
	ପାଇଁ (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    ଯଦି (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    ଯଦି (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    ଯଦି (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		ଯଦି (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    ଯଦି (redraw)
	ସନ୍ଦେଶ_check();
    ଫେରନ୍ତୁ retval;
}

ସ୍ଥିର cmdline_info_T	prev_ccline;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
save_cmdline(cmdline_info_T *ccp)
{
    ଯଦି (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ଖାଲି;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_paste(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା literally,	// Insert text literally instead of "as typed"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    ଯଦି (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ଫେରନ୍ତୁ FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    ଧାଡ଼ି_breakcheck();
    ଯଦି (got_int)
	ଫେରନ୍ତୁ FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    ଯଦି (i)
    {
	// Got the value of a special register in "arg".
	ଯଦି (arg == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	ଯଦି (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

	    // Locate start of last word in the cmd buffer.
	    ପାଇଁ (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		ଯଦି (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    ଯଦି (!vim_iswordc(mb_ptr2char(w - len)))
			ଭାଙ୍ଗ;
		    w -= len;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (!vim_iswordc(w[-1]))
			ଭାଙ୍ଗ;
		    --w;
		}
	    }
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((ccline.cmdbuff + ccline.cmdpos) - w);
	    ଯଦି (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	ଯଦି (allocated)
	    vim_free(arg);
	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    ଶୂନ୍ୟ
cmdline_paste_str(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା literally)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c, cv;

    ଯଦି (literally)
	put_on_cmdline(s, -1, TRUE);
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (*s != NUL)
	{
	    cv = *s;
	    ଯଦି (cv == Ctrl_V && s[1])
		++s;
	    ଯଦି (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    ଅନ୍ୟଥା
		c = *s++;
	    ଯଦି (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    ଶୂନ୍ୟ
redrawcmdline(ଶୂନ୍ୟ)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    ଶୂନ୍ୟ
redrawcmdline_ex(ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_compute_cmdrow)
{
    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;
    need_wait_return = FALSE;
    ଯଦି (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ସ୍ଥିର ଶୂନ୍ୟ
redrawcmdprompt(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;
    ଯଦି (ccline.cmdfirstc != NUL)
	ସନ୍ଦେଶ_putchar(ccline.cmdfirstc);
    ଯଦି (ccline.cmdprompt != ଖାଲି)
    {
	ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = ସନ୍ଦେଶ_col + (ସନ୍ଦେଶ_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	ଯଦି (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    ଅନ୍ୟଥା
	ପାଇଁ (i = ccline.cmdindent; i > 0; --i)
	    ସନ୍ଦେଶ_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    ଶୂନ୍ୟ
redrawcmd(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_in_echowindow = in_echowindow;

    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;

    // when 'incsearch' is set there may be no command line while redrawing
    ଯଦି (ccline.cmdbuff == ଖାଲି)
    {
	windgoto(cmdline_row, cmdline_col_off);
	ସନ୍ଦେଶ_clr_eos();
	ଫେରନ୍ତୁ;
    }

    // Do not put this in the message window.
    in_echowindow = FALSE;

    sb_text_restart_cmdline();
    ସନ୍ଦେଶ_start();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    ସନ୍ଦେଶ_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    ସନ୍ଦେଶ_no_more = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    ସନ୍ଦେଶ_clr_eos();
    ସନ୍ଦେଶ_no_more = FALSE;

    set_cmdspos_cursor();
    ଯଦି (extra_char != NUL)
	putcmdline(extra_char, extra_char_shift);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    ସନ୍ଦେଶ_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    in_echowindow = save_in_echowindow;
}

    ଶୂନ୍ୟ
compute_cmdrow(ଶୂନ୍ୟ)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    ଯଦି (exmode_active || (ସନ୍ଦେଶ_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    ଅନ୍ୟଥା
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    ଶୂନ୍ୟ
cursorcmd(ଶୂନ୍ୟ)
{
    ଯଦି (cmd_silent)
	ଫେରନ୍ତୁ;

#ifdef FEAT_RIGHTLEFT
    ଯଦି (cmdmsg_rl)
    {
	ସନ୍ଦେଶ_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	ସନ୍ଦେଶ_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	ଯଦି (ସନ୍ଦେଶ_row <= 0)
	    ସନ୍ଦେଶ_row = Rows - 1;
    }
    ଅନ୍ୟଥା
#endif
    {
	ସନ୍ଦେଶ_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	ସନ୍ଦେଶ_col = ccline.cmdspos % cmdline_width;
	ଯଦି (ସନ୍ଦେଶ_row >= Rows)
	    ସନ୍ଦେଶ_row = Rows - 1;
    }

    windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ଯଦି (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    ଶୂନ୍ୟ
gotocmdline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା clr)
{
    ସନ୍ଦେଶ_start();
#ifdef FEAT_RIGHTLEFT
    ଯଦି (cmdmsg_rl)
	ସନ୍ଦେଶ_col = cmdline_width - 1;
    ଅନ୍ୟଥା
#endif
	ସନ୍ଦେଶ_col = 0;	    // always start in column 0
    ଯଦି (clr)		    // clear the bottom line(s)
	ସନ୍ଦେଶ_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ccheck_abbr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା spos = 0;

    ଯଦି (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ଫେରନ୍ତୁ FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    ଯେତେବେଳେ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    ଯଦି (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    ଅନ୍ୟଥା
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ଫେରନ୍ତୁ check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    ପାଇଁ (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	ଯଦି ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#ଅନ୍ୟଥା
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    ଯଦି (what == VSE_SHELL && csh_like_shell() && p != ଖାଲି)
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
    ଯଦି (p != ଖାଲି && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ଫେରନ୍ତୁ p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    ଶୂନ୍ୟ
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    ଶୂନ୍ୟ
tilde_replace(
    char_u  *orig_pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_files,
    char_u  **files)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    char_u  *p;

    ଯଦି (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	ପାଇଁ (i = 0; i < ଅଙ୍କ_files; ++i)
	{
	    p = home_replace_save(ଖାଲି, files[i]);
	    ଯଦି (p != ଖାଲି)
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
get_cmdline_info(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ସ୍ଥିର cmdline_info_T *
get_ccline_ptr(ଶୂନ୍ୟ)
{
    ଯଦି ((State & MODE_CMDLINE) == 0)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (ccline.cmdbuff != ଖାଲି)
	ଫେରନ୍ତୁ &ccline;
    ଯଦି (prev_ccline_used && prev_ccline.cmdbuff != ଖାଲି)
	ଫେରନ୍ତୁ &prev_ccline;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cmdline_type(ଶୂନ୍ୟ)
{
    cmdline_info_T *p = get_ccline_ptr();

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ NUL;
    ଯଦି (p->cmdfirstc == NUL)
	ଫେରନ୍ତୁ
#ifdef FEAT_EVAL
	    (p->ନିବେଶ_fn) ? '@' :
#endif
	    '-';
    ଫେରନ୍ତୁ p->cmdfirstc;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ସ୍ଥିର char_u *
get_cmdline_str(ଶୂନ୍ୟ)
{
    cmdline_info_T *p;

    ଯଦି (cmdline_star > 0)
	ଫେରନ୍ତୁ ଖାଲି;
    p = get_ccline_ptr();
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ସ୍ଥିର char_u *
get_cmdline_completion_pattern(ଶୂନ୍ୟ)
{
    cmdline_info_T *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xp_context;

    ଯଦି (cmdline_star > 0)
	ଫେରନ୍ତୁ ଖାଲି;

    p = get_ccline_ptr();
    ଯଦି (p == ଖାଲି || p->xpc == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    xp_context = p->xpc->xp_context;
    ଯଦି (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ଯଦି (xp_context == EXPAND_UNSUCCESSFUL)
	ଫେରନ୍ତୁ ଖାଲି;

    char_u *compl_pat = p->xpc->xp_pattern;

    ଯଦି (compl_pat == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ସ୍ଥିର char_u *
get_cmdline_completion(ଶୂନ୍ୟ)
{
    cmdline_info_T	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			xp_context;

    ଯଦି (cmdline_star > 0)
	ଫେରନ୍ତୁ ଖାଲି;

    p = get_ccline_ptr();
    ଯଦି (p == ଖାଲି || p->xpc == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    xp_context = p->xpc->xp_context;
    ଯଦି (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ଯଦି (xp_context == EXPAND_UNSUCCESSFUL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    ଶୂନ୍ୟ
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    ଶୂନ୍ୟ
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    ଶୂନ୍ୟ
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    ଶୂନ୍ୟ
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ସଂଖ୍ୟା = p != ଖାଲି ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    ଶୂନ୍ୟ
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ଖାଲି && p->cmdprompt != ଖାଲି ?
					    vim_strsave(p->cmdprompt) : ଖାଲି;
}

/*
 * "getcmdscreenpos()" function
 */
    ଶୂନ୍ୟ
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ସଂଖ୍ୟା = p != ଖାଲି ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    ଶୂନ୍ୟ
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    ଯଦି (rettv->vval.v_string == ଖାଲି)
	ଫେରନ୍ତୁ;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_cmdline_str(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ 1;

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
    ଯଦି (realloc_cmdbuff(len + 1) != OK)
	ଫେରନ୍ତୁ 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ଫେରନ୍ତୁ 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_cmdline_pos(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    ଯଦି (pos < 0)
	new_cmdpos = 0;
    ଅନ୍ୟଥା
	new_cmdpos = pos;

    ଫେରନ୍ତୁ 0;
}

// "setcmdline()" function
    ଶୂନ୍ୟ
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos = -1;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

	pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error) - 1;
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
	ଯଦି (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    ଫେରନ୍ତୁ;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_ସଂଖ୍ୟା = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    ଶୂନ୍ୟ
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]) - 1;
    ଯଦି (pos >= 0)
	rettv->vval.v_ସଂଖ୍ୟା = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cmdline_firstc(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_list_range(char_u **str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *num1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *num2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = FALSE;
    varnumber_T	ଅଙ୍କ;

    *str = skipwhite(*str);
    ଯଦି (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ଖାଲି, &len, 0, &ଅଙ୍କ, ଖାଲି, 0, FALSE, ଖାଲି);
	*str += len;
	// overflow
	ଯଦି (ଅଙ୍କ > INT_MAX)
	    ଫେରନ୍ତୁ FAIL;

	*num1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
	first = TRUE;
    }
    *str = skipwhite(*str);
    ଯଦି (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ଖାଲି, &len, 0, &ଅଙ୍କ, ଖାଲି, 0, FALSE, ଖାଲି);
	ଯଦି (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    ଯଦି (ଅଙ୍କ > INT_MAX)
		ଫେରନ୍ତୁ FAIL;

	    *num2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
	}
	ଅନ୍ୟଥା ଯଦି (!first)		// no number given at all
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (first)			// only one number given
	*num2 = *num1;
    ଫେରନ୍ତୁ OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    ଅକ୍ଷର *
did_set_cedit(optset_T *args UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

    ଯଦି (*p_cedit == NUL)
	cedit_ଚାବି = -1;
    ଅନ୍ୟଥା
    {
	n = string_to_ଚାବି(p_cedit, FALSE);
	ଯଦି (n == 0 || vim_isprintc(n))
	    ଫେରନ୍ତୁ e_invalid_argument;
	cedit_ଚାବି = n;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
open_cmdwin(ଶୂନ୍ୟ)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    linenr_T		lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			histtype;
    garray_T		winsizes;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_restart_edit = restart_edit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_State = State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_ଚାବି_Typed;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			newbuf_status;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    ଯଦି (text_or_buf_locked()
	    || cmdwin_type != 0
#ଯଦି defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	ଫେରନ୍ତୁ K_IGNORE;
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
    ଯଦି (win_split((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ଫେରନ୍ତୁ K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    ଯଦି (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ଫେରନ୍ତୁ Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_ONE, ECMD_HIDE,
					ଖାଲି);
    cmdwin_valid = win_valid(cmdwin_win);
    ଯଦି (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	ଯଦି (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	ଯଦି (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	ଯଦି (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(ଖାଲି, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ଖାଲି;
	beep_flush();
	ga_clear(&winsizes);
	ଫେରନ୍ତୁ Ctrl_C;
    }
    cmdwin_buf = curbuf;

    set_option_ମୂଲ୍ୟ_give_err((char_u *)"bt",
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
    ଯଦି (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	ଯଦି (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    add_map((char_u *)"<buffer> <Tab> <C-X><C-V>", MODE_ଭର୍ତ୍ତି, TRUE);
	    add_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", MODE_NORMAL, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    add_map((char_u *)"<buffer> <S-Tab> <C-P>", MODE_ଭର୍ତ୍ତି, TRUE);
	}
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_lock;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    ଯଦି (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	ଯଦି (i >= 0)
	{
	    lnum = 0;
	    କର
	    {
		ଯଦି (++i == get_hislen())
		    i = 0;
		ଯଦି (get_histentry(histtype)[i].hisstr != ଖାଲି)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    ଯେତେବେଳେ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    curwin->w_cursor.col = ccline.cmdpos;
    changed_ଧାଡ଼ି_abv_curs();
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
    ଯଦି (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ifdef FEAT_FOLDING
    save_ଚାବି_Typed = ଚାବି_Typed;
#endif

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ifdef FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    ଚାବି_Typed = save_ଚାବି_Typed;
#endif

    cmdwin_type = 0;
    cmdwin_buf = ଖାଲି;
    cmdwin_win = ଖାଲି;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    ଯଦି (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    ଅନ୍ୟଥା
    {
#ଯଦି defined(FEAT_EVAL)
	// autocmds may abort script processing
	ଯଦି (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	ଯଦି (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    ଯଦି (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    ଯଦି (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen;
		ccline.cmdbufflen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(plen + 1);
		cmdwin_result = CAR;
	    }
	    ଅନ୍ୟଥା
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	ଯଦି (ccline.cmdbuff == ଖାଲି)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	ଅନ୍ୟଥା
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    ଯଦି (ccline.cmdpos == ccline.cmdlen - 1
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
	ଯଦି (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	ଯଦି (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(ଖାଲି, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	ଯଦି (cmdwin_result == K_IGNORE)
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

    ଫେରନ୍ତୁ cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_in_cmdwin(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    ଯଦି (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ ଖାଲି;
#endif
}

#ଯଦି defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    ଶୂନ୍ୟ
get_user_ନିବେଶ(
    typval_T	*argvars,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inputdialog,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		secret)
{
    char_u	*prompt;
    char_u	*p = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ଖାଲି;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
    ଯଦି (ନିବେଶ_busy)
	ଫେରନ୍ତୁ;  // this doesn't work recursively.

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_ନିବେଶ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ଯଦି (no_console_ନିବେଶ() && !is_not_a_term())
	ଫେରନ୍ତୁ;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    ଯଦି (prompt != ଖାଲି)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	ଯଦି (p == ଖାଲି)
	    p = prompt;
	ଅନ୍ୟଥା
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    ସନ୍ଦେଶ_start();
	    ସନ୍ଦେଶ_clr_eos();
	    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)prompt, get_echo_attr());
	    ସନ୍ଦେଶ_didout = FALSE;
	    ସନ୍ଦେଶ_starthere();
	    *p = c;
	}
	cmdline_row = ସନ୍ଦେଶ_row;

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    ଯଦି (defstr != ଖାଲି)
		stuffReadbuffSpec(defstr);

	    ଯଦି (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ଖାଲି;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		ଯଦି (xp_name == ଖାଲି)
		    ଫେରନ୍ତୁ;

		xp_namelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(xp_name);

		ଯଦି (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ଫେରନ୍ତୁ;
	    }
	}

	ଯଦି (defstr != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ex_normal_busy = ex_normal_busy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_vgetc_busy = vgetc_busy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ନିବେଶ_busy = ନିବେଶ_busy;

	    ନିବେଶ_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    ନିବେଶ_busy = save_ନିବେଶ_busy;
	}
	ଯଦି (inputdialog && rettv->vval.v_string == ଖାଲି
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	ସନ୍ଦେଶ_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#endif

/*
 * "wildtrigger()" function
 */
    ଶୂନ୍ୟ
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ଯଦି (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd_type = get_cmdline_type();

    ଯଦି (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	ଚାବି_string[4];

	ଚାବି_string[0] = K_SPECIAL;
	ଚାବି_string[1] = KS_EXTRA;
	ଚାବି_string[2] = KE_WILD;
	ଚାବି_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(ଚାବି_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
