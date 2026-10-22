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

#include "vim.h"

// Return value when handling keys in command-line mode.
#define CMDLINE_NOT_CHANGED	1
#define CMDLINE_CHANGED		2
#define GOTO_NORMAL_MODE	3
#define PROCESS_NEXT_KEY	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
static cmdline_info_T ccline;

#ifdef FEAT_EVAL
static ကိန်းပြည့်	new_cmdpos;	// position set by set_cmdline_pos()
#endif

static ကိန်းပြည့်	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
static ကိန်းပြည့်	extra_char_shift;

#ifdef FEAT_RIGHTLEFT
static ကိန်းပြည့်	cmd_hkmap = 0;	// Hebrew mapping during command line
#endif

static char_u	*getcmdline_int(ကိန်းပြည့် firstc, long count, ကိန်းပြည့် indent, ကိန်းပြည့် clear_ccline);
static ကိန်းပြည့်	cmdline_charsize(ကိန်းပြည့် idx);
static void	set_cmdspos(void);
static void	set_cmdspos_cursor(void);
static void	correct_cmdspos(ကိန်းပြည့် idx, ကိန်းပြည့် cells);
static void	dealloc_cmdbuff(void);
static void	alloc_cmdbuff(size_t len);
static void	draw_cmdline(ကိန်းပြည့် start, ကိန်းပြည့် len);
static void	save_cmdline(cmdline_info_T *ccp);
static void	restore_cmdline(cmdline_info_T *ccp);
static ကိန်းပြည့်	cmdline_paste(ကိန်းပြည့် regname, ကိန်းပြည့် literally, ကိန်းပြည့် remcr);
static void	redrawcmdprompt(void);
static ကိန်းပြည့်	ccheck_abbr(ကိန်းပြည့်);
static ကိန်းပြည့်	open_cmdwin(void);
#ifdef FEAT_SEARCH_EXTRA
static ကိန်းပြည့်	empty_pattern_magic(char_u *pat, size_t len, magic_T magic_val);
#endif

static ကိန်းပြည့်	cedit_key = -1;	// key value of 'cedit' option

    static void
trigger_cmd_autocmd(ကိန်းပြည့် typechar, ကိန်းပြည့် evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    static void
abandon_cmdline(void)
{
    dealloc_cmdbuff();
    အကယ်၍ (msg_scrolled == 0)
	compute_cmdrow();
    msg("");
    redraw_cmdline = TRUE;
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    static ကိန်းပြည့်
empty_pattern(char_u *p, size_t len, ကိန်းပြည့် delim)
{
    magic_T	magic_val = MAGIC_ON;

    အကယ်၍ (len > 0)
	(void) skip_regexp_ex(p, delim, magic_isset(), NULL, NULL, &magic_val);
    မဟုတ်ပါက
	ပြန်ပေး TRUE;

    ပြန်ပေး empty_pattern_magic(p, len, magic_val);
}

    static ကိန်းပြည့်
empty_pattern_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    နေစဉ် (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != NULL)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ပြန်ပေး len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
typedef sa_nracanaa {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    ကိန်းပြည့်		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_empty_rows;
} viewstate_T;

    static void
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

    static void
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
typedef sa_nracanaa {
    pos_T	search_start;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    ကိန်းပြည့်		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	match_start;
    pos_T	match_end;
    ကိန်းပြည့်		did_incsearch;
    ကိန်းပြည့်		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    static void
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
    static void
set_search_match(pos_T *t)
{
    t->lnum += search_match_lines;
    t->col = search_match_endcol;
    အကယ်၍ (t->lnum > curbuf->b_ml.ml_line_count)
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
    ကိန်းပြည့်
parse_pattern_and_range(
	pos_T	*incsearch_start,
	ကိန်းပြည့်	*search_delim,
	ကိန်းပြည့်	*skiplen,
	ကိန်းပြည့်	*patlen)
{
    char_u	*cmd, *p, *end;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    ကိန်းပြည့်		delim_optional = FALSE;
    ကိန်းပြည့်		delim;
    ကိန်းပြည့်		use_last_pat;
    magic_T     magic = 0;
    var_na	*dummy;

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
    cmd = skip_range(ea.cmd, TRUE, NULL);

    အကယ်၍ (vim_strchr((char_u *)"sgvlu", *cmd) == NULL)
	ပြန်ပေး FALSE;

    // Skip over command name to find pattern separator
    အတွက် (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    အကယ်၍ (*skipwhite(p) == NUL)
	ပြန်ပေး FALSE;

    အကယ်၍ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	အကယ်၍ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	မဟုတ်ပါက အကယ်၍ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    မဟုတ်ပါက အကယ်၍ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	အကယ်၍ (*p == '!')
	    p = skipwhite(p + 1);
	နေစဉ် (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	အကယ်၍ (*p == NUL)
	    ပြန်ပေး FALSE;
    }
    မဟုတ်ပါက အကယ်၍ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	အကယ်၍ (*p == '!')
	{
	    p++;
	    အကယ်၍ (*skipwhite(p) == NUL)
		ပြန်ပေး FALSE;
	}
	အကယ်၍ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    မဟုတ်ပါက
	ပြန်ပေး FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *search_delim = delim;

    end = skip_regexp_ex(p, delim, magic_isset(), NULL, NULL, &magic);
    use_last_pat = end == p && *end == delim;

    အကယ်၍ (end == p && !use_last_pat)
	ပြန်ပေး FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    အကယ်၍ (!use_last_pat)
    {
	var_na c = *end;
	ကိန်းပြည့် empty;

	*end = NUL;
	empty = empty_pattern_magic(p, (size_t)(end - p), magic);
	*end = c;
	အကယ်၍ (empty)
	    ပြန်ပေး FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (ကိန်းပြည့်)(p - ccline.cmdbuff);
    *patlen = (ကိန်းပြည့်)(end - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_start;

    parse_cmd_address(&ea, &dummy, TRUE);

    အကယ်၍ (ea.addr_count > 0)
    {
	ကိန်းပြည့် reverse_match = ea.line2 < ea.line1;
	search_first_line = reverse_match ? ea.line2 : ea.line1;
	search_last_line = reverse_match ? ea.line1 : ea.line2;
    }
    မဟုတ်ပါက အကယ်၍ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	search_first_line = search_last_line = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ပြန်ပေး TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    static ကိန်းပြည့်
do_incsearch_highlighting(
	ကိန်းပြည့်		    firstc,
	ကိန်းပြည့်		    *search_delim,
	incsearch_state_T   *is_state,
	ကိန်းပြည့်		    *skiplen,
	ကိန်းပြည့်		    *patlen)
{
    ကိန်းပြည့် retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    အကယ်၍ (!p_is || cmd_silent)
	ပြန်ပေး FALSE;

    // By default search all lines
    search_first_line = 0;
    search_last_line = MAXLNUM;

    အကယ်၍ (firstc == '/' || firstc == '?')
    {
	*search_delim = firstc;
	ပြန်ပေး TRUE;
    }

    အကယ်၍ (firstc != ':')
	ပြန်ပေး FALSE;

    ++emsg_off;
    retval = parse_pattern_and_range(&is_state->search_start, search_delim,
	    skiplen, patlen);
    --emsg_off;

    ပြန်ပေး retval;
}

    static void
finish_incsearch_highlighting(
	ကိန်းပြည့် gotesc,
	incsearch_state_T *is_state,
	ကိန်းပြည့် call_update_screen)
{
    အကယ်၍ (!is_state->did_incsearch)
	ပြန်ပေး;

    is_state->did_incsearch = FALSE;
    အကယ်၍ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    မဟုတ်ပါက
    {
	အကယ်၍ (!EQUAL_POS(is_state->save_cursor, is_state->search_start))
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
    အကယ်၍ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    static void
may_do_incsearch_highlighting(
	ကိန်းပြည့်		    firstc,
	long		    count,
	incsearch_state_T   *is_state)
{
    ကိန်းပြည့်		skiplen, patlen;
    ကိန်းပြည့်		found;  // do_search() result
    pos_T	end_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    ကိန်းပြည့်		next_char;
    ကိန်းပြည့်		use_last_pat;
    ကိန်းပြည့်		did_do_incsearch = is_state->did_incsearch;
    ကိန်းပြည့်		search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    အကယ်၍ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	finish_incsearch_highlighting(FALSE, is_state, TRUE);
	အကယ်၍ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ပြန်ပေး;
    }

    // If there is a character waiting, search and redraw later.
    အကယ်၍ (char_avail())
    {
	restore_last_search_pattern();
	is_state->incsearch_postponed = TRUE;
	ပြန်ပေး;
    }
    is_state->incsearch_postponed = FALSE;

    အကယ်၍ (search_first_line == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->search_start;
    မဟုတ်ပါက အကယ်၍ (search_first_line > curbuf->b_ml.ml_line_count)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	curwin->w_cursor.col = MAXCOL;
    }
    မဟုတ်ပါက
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
    အကယ်၍ (patlen == 0 && !use_last_pat)
    {
	found = 0;
	set_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    မဟုတ်ပါက
    {
	ကိန်းပြည့် search_flags = SEARCH_OPT + SEARCH_NOOF + SEARCH_PEEK;

	cursor_off();	// so the user knows we're busy
	out_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	အကယ်၍ (!p_hls)
	    search_flags += SEARCH_KEEP;
	အကယ်၍ (search_first_line != 0)
	    search_flags += SEARCH_START;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_FIELD(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	found = do_search(NULL, firstc == ':' ? '/' : firstc, search_delim,
				 ccline.cmdbuff + skiplen, patlen, count, search_flags,
# ifdef FEAT_RELTIME
		&sia
# မဟုတ်ပါက
		NULL
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = next_char;
	--emsg_off;

	အကယ်၍ (curwin->w_cursor.lnum < search_first_line
		|| curwin->w_cursor.lnum > search_last_line)
	{
	    // match outside of address range
	    found = 0;
	    curwin->w_cursor = is_state->search_start;
	}

	// if interrupted while searching, behave like it failed
	အကယ်၍ (got_int)
	{
	    (void)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    found = 0;
	}
	မဟုတ်ပါက အကယ်၍ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    အကယ်၍ (found != 0)
	highlight_match = TRUE;		// highlight position
    မဟုတ်ပါက
	highlight_match = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    changed_cline_bef_curs();
    update_topline();

    အကယ်၍ (found != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->match_start = curwin->w_cursor;
	set_search_match(&curwin->w_cursor);
	validate_cursor();
	end_pos = curwin->w_cursor;
	is_state->match_end = end_pos;
	curwin->w_cursor = save_pos;
    }
    မဟုတ်ပါက
	end_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    အကယ်၍ (!use_last_pat)
    {
	next_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	အကယ်၍ (empty_pattern(ccline.cmdbuff + skiplen, (size_t)patlen, search_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    set_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = next_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    အကယ်၍ (p_ru && curwin->w_status_height > 0)
	curwin->w_redr_status = true;

    update_screen(UPD_SOME_VALID);
    highlight_match = FALSE;
    restore_last_search_pattern();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    အကယ်၍ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->search_start;
    မဟုတ်ပါက အကယ်၍ (found != 0)
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
    static ကိန်းပြည့်
may_adjust_incsearch_highlighting(
	ကိန်းပြည့်			firstc,
	long			count,
	incsearch_state_T	*is_state,
	ကိန်းပြည့်			c)
{
    ကိန်းပြည့်		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = NULL;
    char_u	*searchstr;
    char_u	*strcopy = NULL;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	offset;
    ကိန်းပြည့်		search_flags = SEARCH_NOOF;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    အကယ်၍ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	ပြန်ပေး OK;
    }
    အကယ်၍ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_last_search_pattern();
	ပြန်ပေး FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (void)parse_search_pattern_offset(&pat, &patlen_s, search_delim,
				    SEARCH_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &offset);

    cursor_off();
    out_flush();
    အကယ်၍ (c == Ctrl_G)
    {
	t = is_state->match_end;
	အကယ်၍ (LT_POS(is_state->match_start, is_state->match_end))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (void)decl(&t);
	search_flags += SEARCH_COL;
    }
    မဟုတ်ပါက
	t = is_state->match_start;
    အကယ်၍ (!p_hls)
	search_flags += SEARCH_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, NULL,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, count, search_flags, RE_SEARCH, NULL);
    --emsg_off;
    အကယ်၍ (dircp != NULL)
	*dircp = search_delim;
    အကယ်၍ (i)
    {
	pos_T	match_start = is_state->match_start;
	pos_T	match_end = is_state->match_end;
	long	off = offset.off;

	is_state->search_start = match_start;
	အကယ်၍ (!offset.line && (offset.end || off != 0))
	{
	    အကယ်၍ (offset.end)
	    {
		is_state->search_start = match_end;
		(void)decl(&is_state->search_start);
	    }
	    နေစဉ် (off > 0)
	    {
		အကယ်၍ (incl(&is_state->search_start) == -1)
		    bha_nga;
		--off;
	    }
	    နေစဉ် (off < 0)
	    {
		အကယ်၍ (decl(&is_state->search_start) == -1)
		    bha_nga;
		++off;
	    }
	}
	is_state->match_end = t;
	is_state->match_start = t;
	အကယ်၍ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (void)decl(&is_state->search_start);
	}
	မဟုတ်ပါက အကယ်၍ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->search_start = t;
	    (void)incl(&is_state->search_start);
	}
	အကယ်၍ (LT_POS(t, is_state->search_start) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->search_start = t;
	    အကယ်၍ (firstc == '?')
		(void)incl(&is_state->search_start);
	    မဟုတ်ပါက
		(void)decl(&is_state->search_start);
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
    မဟုတ်ပါက
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_last_search_pattern();
    ပြန်ပေး FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    static ကိန်းပြည့်
may_add_char_to_search(ကိန်းပြည့် firstc, ကိန်းပြည့် *c, incsearch_state_T *is_state)
{
    ကိန်းပြည့်		skiplen, patlen, search_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_last_search_pattern();

    အကယ်၍ (!do_incsearch_highlighting(firstc, &search_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_last_search_pattern();
	ပြန်ပေး FAIL;
    }
    restore_last_search_pattern();

    // Add a character from under the cursor for 'incsearch'.
    အကယ်၍ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->match_end;
	*c = gchar_cursor();
	အကယ်၍ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    အကယ်၍ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    အကယ်၍ (*c == search_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != NULL)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    အကယ်၍ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		ကိန်းပြည့် save_c = *c;

		နေစဉ် (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.col += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ပြန်ပေး FAIL;
	}
    }
    ပြန်ပေး OK;
}
#endif

#ifdef FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    static ကိန်းပြည့်
cmdline_has_arabic(ကိန်းပြည့် start, ကိန်းပြည့် len)
{
    ကိန်းပြည့်	    j;
    ကိန်းပြည့်	    mb_l;
    ကိန်းပြည့်	    u8c;
    char_u  *p;
    ကိန်းပြည့်	    u8cc[MAX_MCO];

    အကယ်၍ (!enc_utf8)
	ပြန်ပေး FALSE;

    အတွက် (j = start; j < start + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	mb_l = utfc_ptr2len_len(p, start + len - j);
	အကယ်၍ (ARABIC_CHAR(u8c))
	    ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}
#endif

    void
cmdline_init(void)
{
    CLEAR_FIELD(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    static ကိန်းပြည့်
cmdline_handle_ctrl_bsl(ကိန်းပြည့် c, ကိန်းပြည့် *gotesc)
{
    ++no_mapping;
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    အကယ်၍ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && KeyTyped)
#ifdef FEAT_EVAL
		|| cmdline_star > 0
#endif
		))
    {
	vungetc(c);
	ပြန်ပေး PROCESS_NEXT_KEY;
    }

#ifdef FEAT_EVAL
    အကယ်၍ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	အကယ်၍ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	မဟုတ်ပါက
	    new_cmdpos = ccline.cmdpos;

	c = get_expr_register();
	အကယ်၍ (c == '=')
	{
	    char_u	*p = NULL;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_expr_line();
	    --textlock;

	    အကယ်၍ (p != NULL)
	    {
		ကိန်းပြည့် len = (ကိန်းပြည့်)STRLEN(p);

		အကယ်၍ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    အကယ်၍ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    မဟုတ်ပါက
			ccline.cmdpos = new_cmdpos;

		    KeyTyped = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ပြန်ပေး CMDLINE_CHANGED;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ပြန်ပေး CMDLINE_NOT_CHANGED;
    }
#endif

    အကယ်၍ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ပြန်ပေး GOTO_NORMAL_MODE;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    static ကိန်းပြည့်
cmdline_wildchar_complete(
	ကိန်းပြည့်		c,
	ကိန်းပြည့်		escape,
	ကိန်းပြည့်		*did_wild_list,
	ကိန်းပြည့်		*wim_index_p,
	expand_T	*xp,
	ကိန်းပြည့်		*gotesc,
	ကိန်းပြည့်		redraw_if_menu_empty,
	pos_T		*pre_incsearch_pos)
{
    ကိန်းပြည့်		wim_index = *wim_index_p;
    ကိန်းပြည့်		res;
    ကိန်းပြည့်		cmdpos_before;
    ကိန်းပြည့်		options = WILD_NO_BEEP;
    ကိန်းပြည့်		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    ကိန်းပြည့်		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    အကယ်၍ (wim_flags[wim_index] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    အကယ်၍ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	အကယ်၍ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_index] & WIM_LIST))
	{
	    (void)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_index] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	အကယ်၍ (wim_flags[wim_index] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	မဟုတ်ပါက အကယ်၍ (wim_flags[wim_index] & WIM_FULL)
	    res = nextwild(xp, WILD_NEXT, options, escape);
	မဟုတ်ပါက
	    res = OK;	    // don't insert 'wildchar' now
    }
    မဟုတ်ပါက		    // typed p_wc first time
    {
	ကိန်းပြည့် wim_longest = (wim_flags[0] & WIM_LONGEST);
	ကိန်းပြည့် wim_list = (wim_flags[0] & WIM_LIST);
	ကိန်းပြည့် wim_full = (wim_flags[0] & WIM_FULL);

	wim_index = 0;
	အကယ်၍ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_PATTERN;
	    အကယ်၍ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    အကယ်၍ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    မဟုတ်ပါက
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	အကယ်၍ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	မဟုတ်ပါက
	{
	    အကယ်၍ (wim_noselect || (wim_list && !wim_full))
		options |= WILD_NOSELECT;
	    အကယ်၍ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	အကယ်၍ (redraw_if_menu_empty && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	အကယ်၍ (got_int)
	{
	    (void)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (void)ExpandOne(xp, NULL, NULL, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_index_p = wim_index;
	    ပြန်ပေး CMDLINE_CHANGED;
	}

	// Display matches
	အကယ်၍ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    အကယ်၍ (wim_longest)
	    {
		ကိန်းပြည့် found_longest_prefix = (ccline.cmdpos != cmdpos_before);
		ကိန်းပြည့် show_menu = p_wmnu && wim_full;

		အကယ်၍ (wim_list || show_menu)
		    (void)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		မဟုတ်ပါက အကယ်၍ (!found_longest_prefix)
		{
		    ကိန်းပြည့် wim_list_next = (wim_flags[1] & WIM_LIST);
		    ကိန်းပြည့် wim_full_next = (wim_flags[1] & WIM_FULL);
		    ကိန်းပြည့် wim_noselect_next = (wim_flags[1] & WIM_NOSELECT);
		    ကိန်းပြည့် wim_noinsert_next = (wim_flags[1] & WIM_NOINSERT);
		    အကယ်၍ (wim_list_next || (p_wmnu && (wim_full_next
				    || wim_noselect_next || wim_noinsert_next)))
		    {
			အကယ်၍ (wim_full_next && !wim_noselect_next && !wim_noinsert_next)
			    nextwild(xp, WILD_NEXT, options, escape);
			မဟုတ်ပါက
			    (void)showmatches(xp, p_wmnu && (wim_noselect_next
					|| wim_noinsert_next), wim_list_next,
				    p_wmnu ? wim_flags[1] : 0);

			အကယ်၍ (wim_list_next)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    မဟုတ်ပါက
	    {
		ကိန်းပြည့် show_menu = p_wmnu && (wim_full || wim_noselect
							    || wim_noinsert);

		အကယ်၍ (wim_list || show_menu)
		    (void)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		မဟုတ်ပါက
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    အကယ်၍ (wim_list)
		*did_wild_list = TRUE;
	}
	မဟုတ်ပါက အကယ်၍ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    အကယ်၍ (wim_index < 3)
	++wim_index;
    အကယ်၍ (c == ESC)
	*gotesc = TRUE;

    *wim_index_p = wim_index;
    ပြန်ပေး (res == OK) ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    static ကိန်းပြည့်
cmdline_erase_chars(
	ကိန်းပြည့် c,
	ကိန်းပြည့် indent
#ifdef FEAT_SEARCH_EXTRA
	, incsearch_state_T *isp
#endif
	)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		j;

    အကယ်၍ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    အကယ်၍ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    အကယ်၍ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_next(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    အကယ်၍ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	အကယ်၍ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    အကယ်၍ (c == Ctrl_W)
	    {
		နေစဉ် (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		နေစဉ် (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		အကယ်၍ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	မဟုတ်ပါက အကယ်၍ (c == Ctrl_W)
	{
	    နေစဉ် (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    အကယ်၍ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		နေစဉ် (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	မဟုတ်ပါက
	    --p;
	ccline.cmdpos = (ကိန်းပြည့်)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	နေစဉ် (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_SEARCH_EXTRA
	အကယ်၍ (ccline.cmdlen == 0)
	{
	    isp->search_start = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#endif
	redrawcmd();
    }
    မဟုတ်ပါက အကယ်၍ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == NULL && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	အကယ်၍ (exmode_active
#ifdef FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#endif
	   )
	    ပြန်ပေး CMDLINE_NOT_CHANGED;

	dealloc_cmdbuff();	// no commandline to return

	အကယ်၍ (!cmd_silent)
	{
#ifdef FEAT_RIGHTLEFT
	    အကယ်၍ (cmdmsg_rl)
		msg_col = cmdline_width;
	    မဟုတ်ပါက
#endif
		msg_col = 0;
	    msg_putchar(' ');		// delete ':'
	}
#ifdef FEAT_SEARCH_EXTRA
	isp->search_start = isp->save_cursor;
#endif
	redraw_cmdline = TRUE;
	ပြန်ပေး GOTO_NORMAL_MODE;
    }
    ပြန်ပေး CMDLINE_CHANGED;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    static void
cmdline_toggle_langmap(long *b_im_ptr)
{
    အကယ်၍ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
	im_set_active(FALSE);	// Disable input method
#endif
	အကယ်၍ (b_im_ptr != NULL)
	{
	    အကယ်၍ (State & MODE_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    မဟုတ်ပါက
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ifdef HAVE_INPUT_METHOD
    မဟုတ်ပါက
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	အကယ်၍ ((p_imdisable && b_im_ptr != NULL)
		? *b_im_ptr == B_IMODE_IM : im_get_status())
	{
	    im_set_active(FALSE);	// Disable input method
	    အကယ်၍ (b_im_ptr != NULL)
		*b_im_ptr = B_IMODE_NONE;
	}
	မဟုတ်ပါက
	{
	    im_set_active(TRUE);	// Enable input method
	    အကယ်၍ (b_im_ptr != NULL)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#endif
    အကယ်၍ (b_im_ptr != NULL)
    {
	အကယ်၍ (b_im_ptr == &curbuf->b_p_iminsert)
	    set_iminsert_global();
	မဟုတ်ပါက
	    set_imsearch_global();
    }
#ifdef CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#endif
#အကယ်၍ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    status_redraw_curbuf();
#endif
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    static ကိန်းပြည့်
cmdline_insert_reg(ကိန်းပြည့် *gotesc UNUSED)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		literally = FALSE;
#ifdef FEAT_EVAL
    ကိန်းပြည့်		save_new_cmdpos = new_cmdpos;
#endif

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#endif
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_keys;
    i = c = plain_vgetc();	// CTRL-R <char>
    အကယ်၍ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    အကယ်၍ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_keys;
#ifdef FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    အကယ်၍ (c == '=')
    {
	အကယ်၍ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_star > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	မဟုတ်ပါက
	    c = get_expr_register();
    }
#endif
    အကယ်၍ (c != ESC)	    // use ESC to cancel inserting register
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
	အကယ်၍ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ပြန်ပေး GOTO_NORMAL_MODE;
	}
#endif
	KeyTyped = FALSE;	// Don't do p_wc completion.
#ifdef FEAT_EVAL
	အကယ်၍ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    အကယ်၍ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    မဟုတ်ပါက
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
    ပြန်ပေး literally ? CMDLINE_CHANGED : CMDLINE_NOT_CHANGED;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    static void
cmdline_left_right_mouse(ကိန်းပြည့် c, ကိန်းပြည့် *ignore_drag_release)
{
    အကယ်၍ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    မဟုတ်ပါက
	*ignore_drag_release = FALSE;
#ifdef FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    အကယ်၍ (!gui.in_use)
#endif
	အကယ်၍ (!mouse_has(MOUSE_COMMAND))
	    ပြန်ပေး;
#ifdef FEAT_CLIPBOARD
    အကယ်၍ (mouse_row < cmdline_row && (clip_star.available || clip_plus.available))
    {
	ကိန်းပြည့်	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	အကယ်၍ (mouse_model_popup() && button == MOUSE_LEFT
		&& (mod_mask & MOD_MASK_SHIFT))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_RIGHT;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
	clip_modeless(button, is_click, is_drag);
	ပြန်ပေး;
    }
#endif

    set_cmdspos();
    အတွက် (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	ကိန်းပြည့်	i;

	i = cmdline_charsize(ccline.cmdpos);
	အကယ်၍ (mouse_row <= cmdline_row + ccline.cmdspos / cmdline_width
		&& mouse_col < ccline.cmdspos % cmdline_width + i)
	    bha_nga;
	အကယ်၍ (has_mbyte)
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
    static ကိန်းပြည့်
cmdline_browse_history(
	ကိန်းပြည့်	c,
	ကိန်းပြည့်	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	ကိန်းပြည့်	histype,
	ကိန်းပြည့်	*hiscnt_p,
	expand_T *xp)
{
    ကိန်းပြည့်		orig_hiscnt;
    ကိန်းပြည့်		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    ကိန်းပြည့်		res;

    အကယ်၍ (get_hislen() == 0 || firstc == NUL)	// no history
	ပြန်ပေး CMDLINE_NOT_CHANGED;

    // save current command string so it can be restored later
    အကယ်၍ (lookfor == NULL)
    {
	အကယ်၍ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == NULL)
	    ပြန်ပေး CMDLINE_NOT_CHANGED;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    အတွက် (;;)
    {
	// one step backwards
	အကယ်၍ (c == K_UP || c == K_S_UP || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    အကယ်၍ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    မဟုတ်ပါက အကယ်၍ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    မဟုတ်ပါက အကယ်၍ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    မဟုတ်ပါက			// at top of list
	    {
		hiscnt = orig_hiscnt;
		bha_nga;
	    }
	}
	မဟုတ်ပါက    // one step forwards
	{
	    // on last entry, clear the line
	    အကယ်၍ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		bha_nga;
	    }

	    // not on a history line, nothing to do
	    အကယ်၍ (hiscnt == get_hislen())
		bha_nga;
	    အကယ်၍ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    မဟုတ်ပါက
		++hiscnt;
	}
	အကယ်၍ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== NULL)
	{
	    hiscnt = orig_hiscnt;
	    bha_nga;
	}
	အကယ်၍ ((c != K_UP && c != K_DOWN)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    bha_nga;
    }

    အကယ်၍ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	ကိန်းပြည့်	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	အကယ်၍ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	မဟုတ်ပါက
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	အကယ်၍ (histype == HIST_SEARCH
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    ကိန်းပြည့်     i;
	    ကိန်းပြည့်     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    အတွက် (i = 0; i <= 1; ++i)
	    {
		len = 0;
		အတွက် (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    အကယ်၍ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			အကယ်၍ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    မဟုတ်ပါက
		    {
			// Escape new sep, unless it is already
			// escaped.
			အကယ်၍ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    အကယ်၍ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			အကယ်၍ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		အကယ်၍ (i == 0)
		{
		    alloc_cmdbuff(len);
		    အကယ်၍ (ccline.cmdbuff == NULL)
		    {
			res = GOTO_NORMAL_MODE;
			goto done;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (ကိန်းပြည့်)len;
	}
	မဟုတ်ပါက
	{
	    alloc_cmdbuff(plen);
	    အကယ်၍ (ccline.cmdbuff == NULL)
	    {
		res = GOTO_NORMAL_MODE;
		goto done;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (ကိန်းပြည့်)plen;
	}

	redrawcmd();
	res = CMDLINE_CHANGED;
	goto done;
    }
    beep_flush();
    res = CMDLINE_NOT_CHANGED;

done:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ပြန်ပေး res;
}

/*
 * Initialize the current command-line info.
 */
    static ကိန်းပြည့်
init_ccline(ကိန်းပြည့် firstc, ကိန်းပြည့် indent)
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
    အကယ်၍ (ccline.cmdbuff == NULL)
    {
	ccline.cmdbufflen = 0;
	ပြန်ပေး FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_text_start_cmdline();

    // autoindent for :insert and :append
    အကယ်၍ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ပြန်ပေး OK;
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
    ကိန်းပြည့်		  firstc,
    long	  count,	// only used for incremental search
    ကိန်းပြည့်		  indent,	// indent for inside conditionals
    getline_opt_T do_concat UNUSED)
{
    ပြန်ပေး getcmdline_int(firstc, count, indent, TRUE);
}

    static char_u *
getcmdline_int(
    ကိန်းပြည့်		firstc,
    long	count UNUSED,	// only used for incremental search
    ကိန်းပြည့်		indent,		// indent for inside conditionals
    ကိန်းပြည့်		clear_ccline)	// clear ccline first
{
    static ကိန်းပြည့်	depth = 0;	    // call depth
    ကိန်းပြည့်		c = 0;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		j;
    ကိန်းပြည့်		gotesc = FALSE;		// TRUE when <ESC> just typed
    ကိန်းပြည့်		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = NULL;	// string to match
    size_t	lookforlen = 0;
    ကိန်းပြည့်		hiscnt;			// current history line in use
    ကိန်းပြည့်		histype;		// history type to be used
#ifdef FEAT_SEARCH_EXTRA
    incsearch_state_T	is_state;
#endif
    ကိန်းပြည့်		did_wild_list = FALSE;	// did wild_list() recently
    ကိန်းပြည့်		wim_index = 0;		// index in wim_flags[]
    ကိန်းပြည့်		res;
    ကိန်းပြည့်		save_msg_scroll = msg_scroll;
    ကိန်းပြည့်		save_State = State;	// remember State when called
    ကိန်းပြည့်		some_key_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    ကိန်းပြည့်		ignore_drag_release = TRUE;
#ifdef FEAT_EVAL
    ကိန်းပြည့်		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = NULL;
    buf_T	*b_im_ptr_buf = NULL;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    ကိန်းပြည့်		did_save_ccline = FALSE;
    ကိန်းပြည့်		cmdline_type;
    ကိန်းပြည့်		wild_type = 0;
    ကိန်းပြည့်		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = NULL;
    ကိန်းပြည့်		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++depth;

    အကယ်၍ (ccline.cmdbuff != NULL)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    အကယ်၍ (clear_ccline)
	CLEAR_FIELD(ccline);

#ifdef FEAT_EVAL
    အကယ်၍ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    အကယ်၍ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

#ifdef FEAT_SEARCH_EXTRA
    init_incsearch_state(&is_state);
#endif

    အကယ်၍ (init_ccline(firstc, indent) != OK)
	goto theend;	// out of memory

    အကယ်၍ (depth == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	goto theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    မဟုတ်ပါက
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		// don't redirect the typed command
    အကယ်၍ (!cmd_silent)
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

#အကယ်၍ defined(FEAT_EVAL)
    အကယ်၍ (ccline.input_fn)
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

    အကယ်၍ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	အကယ်၍ (curbuf->b_p_imsearch == B_IMODE_USE_INSERT)
	    b_im_ptr = &curbuf->b_p_iminsert;
	မဟုတ်ပါက
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	အကယ်၍ (*b_im_ptr == B_IMODE_LMAP)
	    State |= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef HAVE_INPUT_METHOD
    မဟုတ်ပါက အကယ်၍ (p_imcmdline)
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
    အကယ်၍ (!debug_mode)
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
    အကယ်၍ (did_emsg)
	redrawcmd();

#ifdef FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    အကယ်၍ (!cmd_silent && msg_scrolled == 0)
    {
	ကိန်းပြည့်	found_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    အကယ်၍ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_status = true;
		found_one = TRUE;
	    }

	အကယ်၍ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    found_one = TRUE;
	}

	အကယ်၍ (found_one)
	    redraw_statuslines();
    }
#endif

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    အတွက် (;;)
    {
	ကိန်းပြည့်	trigger_cmdlinechanged = TRUE;
	ကိန်းပြည့်	end_wildmenu;
	ကိန်းပြည့်	prev_cmdpos = ccline.cmdpos;
	ကိန်းပြည့်	skip_pum_redraw = FALSE;

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

	အကယ်၍ (ex_normal_busy == 0 && stuff_empty() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_key_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	အကယ်၍ (ccline.cmdbuff != NULL)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    အကယ်၍ (prev_cmdbuff == NULL)
		goto returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	အကယ်၍ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	do
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = safe_vgetc();
	} နေစဉ် (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	အကယ်၍ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    အကယ်၍ (cmdline_pum_active())
		cmdline_pum_remove(&ccline, FALSE);
	    (void)ExpandOne(&xpc, NULL, NULL, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	အကယ်၍ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    continue;
	}

	အကယ်၍ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    ကိန်းပြည့်	    clen = ccline.cmdlen;
	    ကိန်းပြည့်	    cc_count = aucmd_cmdline_changed_count;

	    အကယ်၍ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		အကယ်၍ (clen == ccline.cmdlen
				    || cc_count != aucmd_cmdline_changed_count)
		    trigger_cmdlinechanged = FALSE;
		goto cmdline_changed;
	    }
	}

	အကယ်၍ (KeyTyped)
	{
	    some_key_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    အကယ်၍ (cmd_hkmap)
		c = hkmap(c);
	    အကယ်၍ (cmdmsg_rl && !KeyStuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		switch (c)
		{
		    pak_sa K_RIGHT:   c = K_LEFT; bha_nga;
		    pak_sa K_S_RIGHT: c = K_S_LEFT; bha_nga;
		    pak_sa K_C_RIGHT: c = K_C_LEFT; bha_nga;
		    pak_sa K_LEFT:    c = K_RIGHT; bha_nga;
		    pak_sa K_S_LEFT:  c = K_S_RIGHT; bha_nga;
		    pak_sa K_C_LEFT:  c = K_C_RIGHT; bha_nga;
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
	အကယ်၍ ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#အကယ်၍ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
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
	အကယ်၍ (lookfor != NULL
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
	အကယ်၍ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	အကယ်၍ (p_wmnu)
	    c = wildmenu_translate_key(&ccline, c, &xpc, did_wild_list);

	ကိန်းပြည့် key_is_wc = (c == p_wc && KeyTyped) || c == p_wcm;
	အကယ်၍ ((cmdline_pum_active() || wild_menu_showing || did_wild_list)
		&& !key_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    အကယ်၍ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		အကယ်၍ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    bha_nga;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	အကယ်၍ (KeyTyped && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
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
#အကယ်၍ defined(FEAT_SEARCH_EXTRA)
	    အကယ်၍ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
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
	အကယ်၍ (end_wildmenu)
	{
	    အကယ်၍ (cmdline_pum_active())
	    {
		skip_pum_redraw = skip_pum_redraw && !key_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    အကယ်၍ (xpc.xp_numfiles != -1)
		(void)ExpandOne(&xpc, NULL, NULL, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    အကယ်၍ (!p_wmnu || (c != K_UP && c != K_DOWN))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
	    wildmenu_cleanup(&ccline);
	}

	အကယ်၍ (p_wmnu)
	    c = wildmenu_process_key(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	အကယ်၍ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    အကယ်၍ (res == CMDLINE_CHANGED)
		goto cmdline_changed;
	    မဟုတ်ပါက အကယ်၍ (res == CMDLINE_NOT_CHANGED)
		goto cmdline_not_changed;
	    မဟုတ်ပါက အကယ်၍ (res == GOTO_NORMAL_MODE)
		goto returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	အကယ်၍ (c == cedit_key || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    အကယ်၍ ((c == K_CMDWIN || ex_normal_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_key_typed = TRUE;
	    }
	}
#ifdef FEAT_DIGRAPHS
	မဟုတ်ပါက
	    c = do_digraph(c);
#endif

	အကယ်၍ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!KeyTyped || vim_strchr(p_cpo, CPO_ESC) != NULL)))
	{
	    // In Ex mode a backslash escapes a newline.
	    အကယ်၍ (exmode_active
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		အကယ်၍ (c == K_KENTER)
		    c = '\n';
	    }
	    မဟုတ်ပါက
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		အကယ်၍ (ccheck_abbr(c + ABBR_OFF))
		    goto cmdline_changed;
		အကယ်၍ (!cmd_silent)
		{
		    windgoto(msg_row, cmdline_col_off);
		    out_flush();
		}
		bha_nga;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	အကယ်၍ ((c == p_wc && !gotesc && KeyTyped) || c == p_wcm || c == K_WILD)
	{
	    အကယ်၍ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_index, &xpc, &gotesc, c == K_WILD,
#ifdef FEAT_SEARCH_EXTRA
		    &is_state.search_start
#မဟုတ်ပါက
		    NULL
#endif
		    );
	    အကယ်၍ (c == K_WILD)
		--emsg_silent;
	    အကယ်၍ (res == CMDLINE_CHANGED)
		goto cmdline_changed;
	    အကယ်၍ (c == K_WILD)
		goto cmdline_not_changed;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	အကယ်၍ (c == K_S_TAB && KeyTyped)
	{
	    အကယ်၍ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		အကယ်၍ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_index] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    ကိန်းပြည့် list = wim_flags[wim_index] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		အကယ်၍ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    goto cmdline_changed;
	    }
	}

	အကယ်၍ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	အကယ်၍ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ifdef FEAT_SEARCH_EXTRA
	    // Apply search highlighting
	    အကယ်၍ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    အကယ်၍ (KeyTyped || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, count, &is_state);
#endif
	    wild_type = 0;
	    goto cmdline_not_changed;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	switch (c)
	{
	pak_sa K_BS:
	pak_sa Ctrl_H:
	pak_sa K_DEL:
	pak_sa K_KDEL:
	pak_sa Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ifdef FEAT_SEARCH_EXTRA
		    , &is_state
#endif
		    );
	    အကယ်၍ (res == CMDLINE_NOT_CHANGED)
		goto cmdline_not_changed;
	    မဟုတ်ပါက အကယ်၍ (res == GOTO_NORMAL_MODE)
		goto returncmd;		// back to cmd mode
	    goto cmdline_changed;

	pak_sa K_INS:
	pak_sa K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#endif
		may_trigger_modechanged();
		status_redraw_curbuf();
		redraw_statuslines();
		goto cmdline_not_changed;

	pak_sa Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : NULL);
		goto cmdline_not_changed;

//	case '@':   only in very old vi
	pak_sa Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		နေစဉ် (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ifdef FEAT_SEARCH_EXTRA
		အကယ်၍ (ccline.cmdlen == 0)
		    is_state.search_start = is_state.save_cursor;
#endif
		redrawcmd();
		goto cmdline_changed;

#ifdef FEAT_CLIPBOARD
	pak_sa Ctrl_Y:
		// Copy the modeless selection, if there is one.
		အကယ်၍ (clip_star.state != SELECT_CLEARED)
		{
		    အကယ်၍ (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    goto cmdline_not_changed;
		}
		bha_nga;
#endif

	pak_sa ESC:	// get here if p_wc != ESC or when ESC typed twice
	pak_sa Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		အကယ်၍ (exmode_active
			       && (ex_normal_busy == 0 || typebuf.tb_len > 0))
		    goto cmdline_not_changed;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		goto returncmd;		// back to cmd mode

	pak_sa Ctrl_R:			// insert register
		res = cmdline_insert_reg(&gotesc);
		အကယ်၍ (res == GOTO_NORMAL_MODE)
		    goto returncmd;
		အကယ်၍ (res == CMDLINE_CHANGED)
		    goto cmdline_changed;
		goto cmdline_not_changed;

	pak_sa Ctrl_D:
		အကယ်၍ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    bha_nga;	// Use ^D as normal char instead

		redrawcmd();
		continue;	// don't do incremental search now

	pak_sa K_RIGHT:
	pak_sa K_S_RIGHT:
	pak_sa K_C_RIGHT:
		do
		{
		    အကယ်၍ (ccline.cmdpos >= ccline.cmdlen)
			bha_nga;
		    i = cmdline_charsize(ccline.cmdpos);
		    အကယ်၍ (KeyTyped && ccline.cmdspos + i >= cmdline_width * Rows)
			bha_nga;
		    ccline.cmdspos += i;
		    အကယ်၍ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    မဟုတ်ပါက
			++ccline.cmdpos;
		}
		နေစဉ် ((c == K_S_RIGHT || c == K_C_RIGHT
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		အကယ်၍ (has_mbyte)
		    set_cmdspos_cursor();
		goto cmdline_not_changed;

	pak_sa K_LEFT:
	pak_sa K_S_LEFT:
	pak_sa K_C_LEFT:
		အကယ်၍ (ccline.cmdpos == 0)
		    goto cmdline_not_changed;
		do
		{
		    --ccline.cmdpos;
		    အကယ်၍ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		နေစဉ် (ccline.cmdpos > 0
			&& (c == K_S_LEFT || c == K_C_LEFT
			       || (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		အကယ်၍ (has_mbyte)
		    set_cmdspos_cursor();
		goto cmdline_not_changed;

	pak_sa K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		goto cmdline_not_changed;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	pak_sa K_F4:
	    အကယ်၍ (mod_mask == MOD_MASK_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		goto cmdline_not_changed;
	    }
	    bha_nga;
#endif

	pak_sa K_MIDDLEDRAG:
	pak_sa K_MIDDLERELEASE:
		goto cmdline_not_changed;	// Ignore mouse

	pak_sa K_MIDDLEMOUSE:
#ifdef FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		အကယ်၍ (!gui.in_use)
#endif
		    အကယ်၍ (!mouse_has(MOUSE_COMMAND))
			goto cmdline_not_changed;   // Ignore mouse
#ifdef FEAT_CLIPBOARD
		အကယ်၍ (clip_star.available)
		    cmdline_paste('*', TRUE, TRUE);
		မဟုတ်ပါက
#endif
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		goto cmdline_changed;

#ifdef FEAT_DND
	pak_sa K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		goto cmdline_changed;
#endif

	pak_sa K_LEFTDRAG:
	pak_sa K_LEFTRELEASE:
	pak_sa K_RIGHTDRAG:
	pak_sa K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		အကယ်၍ (ignore_drag_release)
		    goto cmdline_not_changed;
		// FALLTHROUGH
	pak_sa K_LEFTMOUSE:
	pak_sa K_RIGHTMOUSE:
		cmdline_left_right_mouse(c, &ignore_drag_release);
		goto cmdline_not_changed;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	pak_sa K_MOUSEDOWN:
	pak_sa K_MOUSEUP:
	pak_sa K_MOUSELEFT:
	pak_sa K_MOUSERIGHT:
#ifdef FEAT_PROP_POPUP
		အကယ်၍ (cmdline_pum_active())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_DOWN
				      : c == K_MOUSEUP ? MSCR_UP
				      : c == K_MOUSELEFT ? MSCR_LEFT
				      : MSCR_RIGHT);
#endif
		goto cmdline_not_changed;

	// Alternate buttons ignored here
	pak_sa K_X1MOUSE:
	pak_sa K_X1DRAG:
	pak_sa K_X1RELEASE:
	pak_sa K_X2MOUSE:
	pak_sa K_X2DRAG:
	pak_sa K_X2RELEASE:
	pak_sa K_MOUSEMOVE:
		goto cmdline_not_changed;

#ifdef FEAT_GUI
	pak_sa K_LEFTMOUSE_NM:	// mousefocus click, ignored
	pak_sa K_LEFTRELEASE_NM:
		goto cmdline_not_changed;

	pak_sa K_VER_SCROLLBAR:
		အကယ်၍ (msg_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		goto cmdline_not_changed;

	pak_sa K_HOR_SCROLLBAR:
		အကယ်၍ (msg_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_value);
		    redrawcmd();
		}
		goto cmdline_not_changed;
#endif
#ifdef FEAT_GUI_TABLINE
	pak_sa K_TABLINE:
	pak_sa K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		အကယ်၍ (gui_use_tabline())
		    gui_mch_set_curtab(tabpage_index(curtab));
		goto cmdline_not_changed;
#endif

	pak_sa K_SELECT:	    // end of Select mode mapping - ignore
		goto cmdline_not_changed;

	pak_sa Ctrl_B:	    // begin of command line
	pak_sa K_HOME:
	pak_sa K_KHOME:
	pak_sa K_S_HOME:
	pak_sa K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		goto cmdline_not_changed;

	pak_sa Ctrl_E:	    // end of command line
	pak_sa K_END:
	pak_sa K_KEND:
	pak_sa K_S_END:
	pak_sa K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		goto cmdline_not_changed;

	pak_sa Ctrl_A:	    // all matches
		အကယ်၍ (cmdline_pum_active())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		အကယ်၍ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    bha_nga;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		goto cmdline_changed;

	pak_sa Ctrl_L:
#ifdef FEAT_SEARCH_EXTRA
		အကယ်၍ (may_add_char_to_search(firstc, &c, &is_state) == OK)
		    goto cmdline_not_changed;
#endif

		// completion: longest common part
		အကယ်၍ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    bha_nga;
		goto cmdline_changed;

	pak_sa Ctrl_N:	    // next match
	pak_sa Ctrl_P:	    // previous match
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		အကယ်၍ (cmdline_pum_active() && (mod_mask & MOD_MASK_SHIFT))
		{
		    အကယ်၍ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    goto cmdline_not_changed;
		}
#endif
		အကယ်၍ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_NEXT;
		    အကယ်၍ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			bha_nga;
		    goto cmdline_changed;
		}
		// FALLTHROUGH
	pak_sa K_UP:
	pak_sa K_DOWN:
	pak_sa K_S_UP:
	pak_sa K_S_DOWN:
	pak_sa K_PAGEUP:
	pak_sa K_KPAGEUP:
	pak_sa K_PAGEDOWN:
	pak_sa K_KPAGEDOWN:
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		အကယ်၍ (cmdline_pum_active()
			&& ((c == K_S_UP || c == K_S_DOWN)
			    ? (mod_mask & MOD_MASK_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_mask & MOD_MASK_CTRL)
				&& (mod_mask & MOD_MASK_SHIFT))))
		{
		    ကိန်းပြည့်	up = c == K_S_UP || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    အကယ်၍ (popup_scroll_info(up ? -1 : 1,
					     c != K_S_UP && c != K_S_DOWN))
			cmdline_pum_display();
		    goto cmdline_not_changed;
		}
#endif
		အကယ်၍ (cmdline_pum_active()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    အကယ်၍ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    အကယ်၍ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			bha_nga;
		    goto cmdline_changed;
		}
		မဟုတ်ပါက
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    အကယ်၍ (res == CMDLINE_CHANGED)
		    {
			did_hist_navigate = TRUE;
			goto cmdline_changed;
		    }
		    မဟုတ်ပါက အကယ်၍ (res == GOTO_NORMAL_MODE)
			goto returncmd;
		}
		goto cmdline_not_changed;

#ifdef FEAT_SEARCH_EXTRA
	pak_sa Ctrl_G:	    // next match
	pak_sa Ctrl_T:	    // previous match
		အကယ်၍ (may_adjust_incsearch_highlighting(
					  firstc, count, &is_state, c) == FAIL)
		    goto cmdline_not_changed;
		bha_nga;
#endif

	pak_sa Ctrl_V:
	pak_sa Ctrl_Q:
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
		    အကယ်၍ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			msg_putchar(' ');
			cursorcmd();
		    }
		}

		bha_nga;

#ifdef FEAT_DIGRAPHS
	pak_sa Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		အကယ်၍ (c != NUL)
		    bha_nga;

		redrawcmd();
		goto cmdline_not_changed;
#endif // FEAT_DIGRAPHS

#ifdef FEAT_RIGHTLEFT
	pak_sa Ctrl__:	    // CTRL-_: switch language mode
		အကယ်၍ (!p_ari)
		    bha_nga;
		cmd_hkmap = !cmd_hkmap;
		goto cmdline_not_changed;
#endif

	pak_sa K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, NULL);
		goto cmdline_changed;

	default:
#ifdef UNIX
		အကယ်၍ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    goto returncmd;	// back to Normal mode
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		အကယ်၍ (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		bha_nga;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	အကယ်၍ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    goto cmdline_changed;

	/*
	 * put the character in the command line
	 */
	အကယ်၍ (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_key_name(c, mod_mask), -1, TRUE);
	မဟုတ်ပါက
	{
	    အကယ်၍ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    မဟုတ်ပါက
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	goto cmdline_changed;

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
	အကယ်၍ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ifdef FEAT_SEARCH_EXTRA
	အကယ်၍ (!is_state.incsearch_postponed)
	    continue;
#endif

cmdline_changed:
#ifdef FEAT_SEARCH_EXTRA
	// If the window changed incremental search state is not valid.
	အကယ်၍ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	အကယ်၍ (xpc.xp_context == EXPAND_NOTHING && (KeyTyped || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, count, &is_state);
#endif
	// Trigger CmdlineChanged autocommands.
	အကယ်၍ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != NULL &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ifdef FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    အကယ်၍ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#endif
	}

	// Trigger CursorMovedC autocommands.
	အကယ်၍ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ifdef FEAT_RIGHTLEFT
	အကယ်၍ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    အကယ်၍ (vpeekc() == NUL)
		redrawcmd();
#endif
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    အကယ်၍ (!event_cmdlineleavepre_triggered)
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
    အကယ်၍ (cmdline_pum_active())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_index = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = NULL;
    clear_cmdline_orig();

#ifdef FEAT_SEARCH_EXTRA
    finish_incsearch_highlighting(gotesc, &is_state, FALSE);
#endif

    အကယ်၍ (ccline.cmdbuff != NULL)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	အကယ်၍ (ccline.cmdlen && firstc != NUL
		&& (some_key_typed || histype == HIST_SEARCH))
	{
	    add_to_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_SEARCH ? firstc : NUL);
	    အကယ်၍ (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	အကယ်၍ (gotesc)
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
    အကယ်၍ (some_key_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ifdef FEAT_EVAL
    set_vim_var_char(c);  // Set v:char
#endif
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ifdef FEAT_EVAL
    အကယ်၍ (!debug_mode)
	may_trigger_modechanged();
#endif

#ifdef HAVE_INPUT_METHOD
    အကယ်၍ (b_im_ptr != NULL && buf_valid(b_im_ptr_buf)
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
	အကယ်၍ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	မဟုတ်ပါက
	    ccline.cmdbuff = NULL;

	vim_free(prev_cmdbuff);
	ပြန်ပေး p;
    }
}

#အကယ်၍ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    ကိန်းပြည့်		firstc,
    char_u	*prompt,	// command line prompt
    ကိန်းပြည့်		attr,		// attributes for prompt
    ကိန်းပြည့်		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    ကိန်းပြည့်			did_save_ccline = FALSE;
    ကိန်းပြည့်			msg_col_save = msg_col;
    ကိန်းပြည့်			msg_silent_save = msg_silent;

    အကယ်၍ (ccline.cmdbuff != NULL)
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

    အကယ်၍ (did_save_ccline)
	restore_cmdline(&save_ccline);

    msg_silent = msg_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    အကယ်၍ (ccline.cmdbuff != NULL)
	msg_col = msg_col_save;

    ပြန်ပေး s;
}
#endif

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    ကိန်းပြည့်
check_opt_wim(void)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		idx = 0;

    အတွက် (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    အတွက် (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	အတွက် (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	အကယ်၍ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ပြန်ပေး FAIL;
	အကယ်၍ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	မဟုတ်ပါက အကယ်၍ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_FULL;
	မဟုတ်ပါက အကယ်၍ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	မဟုတ်ပါက အကယ်၍ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	မဟုတ်ပါက အကယ်၍ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	မဟုတ်ပါက အကယ်၍ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	မဟုတ်ပါက
	    ပြန်ပေး FAIL;
	p += i;
	အကယ်၍ (*p == NUL)
	    bha_nga;
	အကယ်၍ (*p == ',')
	{
	    အကယ်၍ (idx == 3)
		ပြန်ပေး FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    နေစဉ် (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    အတွက် (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ပြန်ပေး OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    ကိန်းပြည့်
text_locked(void)
{
    အကယ်၍ (cmdwin_type != 0)
	ပြန်ပေး TRUE;
    ပြန်ပေး textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    void
text_locked_msg(void)
{
    emsg(_(get_text_locked_msg()));
}

    var_na *
get_text_locked_msg(void)
{
    အကယ်၍ (cmdwin_type != 0)
	ပြန်ပေး e_invalid_in_cmdline_window;
    ပြန်ပေး e_not_allowed_to_change_text_or_change_window;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    ကိန်းပြည့်
text_or_buf_locked(void)
{
    အကယ်၍ (text_locked())
    {
	text_locked_msg();
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    ကိန်းပြည့်
curbuf_locked(void)
{
    အကယ်၍ (curbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_edit_another_buffer_now));
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    ကိန်းပြည့်
allbuf_locked(void)
{
    အကယ်၍ (allbuf_lock > 0)
    {
	emsg(_(e_not_allowed_to_change_buffer_information_now));
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

    static ကိန်းပြည့်
cmdline_charsize(ကိန်းပြည့် idx)
{
#အကယ်၍ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    အကယ်၍ (cmdline_star > 0)	    // showing '*', always 1 position
	ပြန်ပေး 1;
#endif
    ပြန်ပေး ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    static void
set_cmdspos(void)
{
    အကယ်၍ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    မဟုတ်ပါက
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    static void
set_cmdspos_cursor(void)
{
    ကိန်းပြည့်		i, m, c;

    set_cmdspos();
    အကယ်၍ (KeyTyped)
    {
	m = cmdline_width * Rows;
	အကယ်၍ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    မဟုတ်ပါက
	m = MAXCOL;
    အတွက် (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	အကယ်၍ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	အကယ်၍ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    bha_nga;
	}
	အကယ်၍ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    static void
correct_cmdspos(ကိန်းပြည့် idx, ကိန်းပြည့် cells)
{
    အကယ်၍ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_width + cells > cmdline_width)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    ကိန်းပြည့်		c,		// normally ':', NUL for ":append"
    void	*cookie UNUSED,
    ကိန်းပြည့်		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    အကယ်၍ (exec_from_reg && vpeekc() == ':')
	(void)vgetc();
    ပြန်ပေး getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    ကိန်းပြည့်		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    void	*cookie UNUSED,
    ကိန်းပြည့်		indent,		// indent for inside conditionals
    getline_opt_T options UNUSED)
{
    garray_T	line_ga;
    char_u	*pend;
    ကိန်းပြည့်		startcol = 0;
    ကိန်းပြည့်		c1 = 0;
    ကိန်းပြည့်		escaped = FALSE;	// CTRL-V typed
    ကိန်းပြည့်		vcol = 0;
    char_u	*p;
    ကိန်းပြည့်		prev_char;
    ကိန်းပြည့်		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    အကယ်၍ ((msg_col || msg_didout) && promptc != '?')
	msg_putchar('\n');
    အကယ်၍ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	အကယ်၍ (p_prompt)
	    msg_putchar(':');
	နေစဉ် (indent-- > 0)
	    msg_putchar(' ');
	startcol = msg_col;
    }

    ga_init2(&line_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    အကယ်၍ (promptc <= 0)
    {
	vcol = indent;
	နေစဉ် (indent >= 8)
	{
	    ga_append(&line_ga, TAB);
	    msg_puts("        ");
	    indent -= 8;
	}
	နေစဉ် (indent-- > 0)
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
    နေစဉ် (!got_int)
    {
	long    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	အကယ်၍ (ga_grow(&line_ga, 40) == FAIL)
	    bha_nga;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	အကယ်၍ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	မဟုတ်ပါက
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	အကယ်၍ (got_int)
	{
	    msg_putchar('\n');
	    bha_nga;
	}

	အကယ်၍ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &line_ga);
	    goto redraw;
	}

	အကယ်၍ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    အကယ်၍ (c1 == '\r')
		c1 = '\n';

	    အကယ်၍ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		အကယ်၍ (line_ga.ga_len > 0)
		{
		    အကယ်၍ (has_mbyte)
		    {
			p = (char_u *)line_ga.ga_data;
			p[line_ga.ga_len] = NUL;
			len = (*mb_head_off)(p, p + line_ga.ga_len - 1) + 1;
			line_ga.ga_len -= len;
		    }
		    မဟုတ်ပါက
			--line_ga.ga_len;
		    goto redraw;
		}
		continue;
	    }

	    အကယ်၍ (c1 == Ctrl_U)
	    {
		msg_col = startcol;
		msg_clr_eos();
		line_ga.ga_len = 0;
		goto redraw;
	    }

	    အကယ်၍ (c1 == Ctrl_T)
	    {
		sw = get_sw_value(curbuf);
		p = (char_u *)line_ga.ga_data;
		p[line_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
add_indent:
		နေစဉ် (get_indent_str(p, 8, FALSE) < indent)
		{
		    (void)ga_grow(&line_ga, 2);  // one more for the NUL
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
		နေစဉ် (p < (char_u *)line_ga.ga_data + line_ga.ga_len)
		{
		    အကယ်၍ (*p == TAB)
		    {
			do
			    msg_putchar(' ');
			နေစဉ် (++vcol % 8);
			++p;
		    }
		    မဟုတ်ပါက
		    {
			len = mb_ptr2len(p);
			msg_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		msg_clr_eos();
		windgoto(msg_row, cmdline_col_off + msg_col);
		continue;
	    }

	    အကယ်၍ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)line_ga.ga_data;
		အကယ်၍ (prev_char == '0' || prev_char == '^')
		{
		    အကယ်၍ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--line_ga.ga_len] = NUL;
		}
		မဟုတ်ပါက
		{
		    p[line_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    အကယ်၍ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_value(curbuf);
		    }
		}
		နေစဉ် (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, line_ga.ga_len - (s - p) + 1);
		    --line_ga.ga_len;
		}
		goto add_indent;
	    }

	    အကယ်၍ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		continue;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    အကယ်၍ (IS_SPECIAL(c1))
		continue;
	}

	အကယ်၍ (IS_SPECIAL(c1))
	    c1 = '?';
	အကယ်၍ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)line_ga.ga_data + line_ga.ga_len);
	မဟုတ်ပါက
	{
	    len = 1;
	    ((char_u *)line_ga.ga_data)[line_ga.ga_len] = c1;
	}
	အကယ်၍ (c1 == '\n')
	    msg_putchar('\n');
	မဟုတ်ပါက အကယ်၍ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    do
		msg_putchar(' ');
	    နေစဉ် (++vcol % 8);
	}
	မဟုတ်ပါက
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
	အကယ်၍ (line_ga.ga_len > 0 && pend[-1] == '\n'
		&& (line_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --line_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    bha_nga;
	}
    }

    --no_mapping;
    --allow_keys;

    // make following messages go to the next line
    msg_didout = FALSE;
    msg_col = 0;
    အကယ်၍ (msg_row < Rows - 1)
	++msg_row;
    emsg_on_display = FALSE;		// don't want ui_delay()

    အကယ်၍ (got_int)
	ga_clear(&line_ga);

    ပြန်ပေး (char_u *)line_ga.ga_data;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    ကိန်းပြည့်
cmdline_overstrike(void)
{
    ပြန်ပေး ccline.overstrike;
}

#အကယ်၍ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    ကိန်းပြည့်
cmdline_at_end(void)
{
    ပြန်ပေး (ccline.cmdpos >= ccline.cmdlen);
}
#endif

#အကယ်၍ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(void)
{
    အကယ်၍ (ccline.cmdbuff == NULL || ccline.cmdpos > ccline.cmdlen)
	ပြန်ပေး MAXCOL;

    အကယ်၍ (has_mbyte)
    {
	colnr_T	col;
	ကိန်းပြည့်	i = 0;

	အတွက် (col = 0; i < ccline.cmdpos; ++col)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ပြန်ပေး col;
    }
    မဟုတ်ပါက
	ပြန်ပေး ccline.cmdpos;
}
#endif

#အကယ်၍ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    static void
redrawcmd_preedit(void)
{
    အကယ်၍ ((State & MODE_CMDLINE)
	    && xic != NULL
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	ကိန်းပြည့်	cmdpos = 0;
	ကိန်းပြည့်	cmdspos;
	ကိန်းပြည့်	old_row;
	ကိန်းပြည့်	old_col;
	colnr_T	col;

	old_row = msg_row;
	old_col = msg_col;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	အကယ်၍ (has_mbyte)
	{
	    အတွက် (col = 0; col < preedit_start_col
			  && cmdpos < ccline.cmdlen; ++col)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	မဟုတ်ပါက
	{
	    cmdspos += preedit_start_col;
	    cmdpos  += preedit_start_col;
	}

	msg_row = cmdline_row + (cmdspos / cmdline_width);
	msg_col = cmdspos % cmdline_width;
	အကယ်၍ (msg_row >= Rows)
	    msg_row = Rows - 1;

	အတွက် (col = 0; cmdpos < ccline.cmdlen; ++col)
	{
	    ကိန်းပြည့် char_len;
	    ကိန်းပြည့် char_attr;

	    char_attr = im_get_feedback_attr(col);
	    အကယ်၍ (char_attr < 0)
		bha_nga; // end of preedit string

	    အကယ်၍ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    မဟုတ်ပါက
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
    static void
dealloc_cmdbuff(void)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    static void
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    အကယ်၍ (len < 80)
	len = 100;
    မဟုတ်ပါက
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (ကိန်းပြည့်)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    ကိန်းပြည့်
realloc_cmdbuff(ကိန်းပြည့် len)
{
    char_u	*p;
    ကိန်းပြည့်		plen;

    အကယ်၍ (len < ccline.cmdbufflen)
	ပြန်ပေး OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    အကယ်၍ (ccline.cmdbuff == NULL)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ပြန်ပေး FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    အကယ်၍ (ccline.xpc != NULL
	    && ccline.xpc->xp_pattern != NULL
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	ကိန်းပြည့် i = (ကိန်းပြည့်)(ccline.xpc->xp_pattern - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	အကယ်၍ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_pattern = ccline.cmdbuff + i;
    }

    vim_free(p);

    ပြန်ပေး OK;
}

#အကယ်၍ defined(FEAT_ARABIC)
static char_u	*arshape_buf = NULL;

# အကယ်၍ defined(EXITFREE)
    void
free_arshape_buf(void)
{
    vim_free(arshape_buf);
}
# endif
#endif

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    static void
draw_cmdline(ကိန်းပြည့် start, ကိန်းပြည့် len)
{
#အကယ်၍ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ကိန်းပြည့်		i;

    အကယ်၍ (cmdline_star > 0)
	အတွက် (i = 0; i < len; ++i)
	{
	    msg_putchar('*');
	    အကယ်၍ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + start + i) - 1;
	}
    မဟုတ်ပါက
#endif
#ifdef FEAT_ARABIC
	အကယ်၍ (p_arshape && !p_tbidi && cmdline_has_arabic(start, len))
    {
	static ကိန်းပြည့်	buflen = 0;
	char_u		*p;
	ကိန်းပြည့်		j;
	ကိန်းပြည့်		newlen = 0;
	ကိန်းပြည့်		mb_l;
	ကိန်းပြည့်		pc, pc1 = 0;
	ကိန်းပြည့်		prev_c = 0;
	ကိန်းပြည့်		prev_c1 = 0;
	ကိန်းပြည့်		u8c;
	ကိန်းပြည့်		u8cc[MAX_MCO];
	ကိန်းပြည့်		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	အကယ်၍ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    အကယ်၍ (arshape_buf == NULL)
		ပြန်ပေး;	// out of memory
	}

	အကယ်၍ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + start)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	အတွက် (j = start; j < start + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, start + len - j);
	    mb_l = utfc_ptr2len_len(p, start + len - j);
	    အကယ်၍ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		အကယ်၍ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    အကယ်၍ (j + mb_l >= start + len)
			nc = NUL;
		    မဟုတ်ပါက
			nc = utf_ptr2char(p + mb_l);
		}
		မဟုတ်ပါက
		{
		    // displaying from left to right
		    အကယ်၍ (j + mb_l >= start + len)
			pc = NUL;
		    မဟုတ်ပါက
		    {
			ကိန်းပြည့်	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      start + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, NULL, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		အကယ်၍ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    အကယ်၍ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    မဟုတ်ပါက
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	msg_outtrans_len(arshape_buf, newlen);
    }
    မဟုတ်ပါက
#endif
	msg_outtrans_len(ccline.cmdbuff + start, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    void
putcmdline(ကိန်းပြည့် c, ကိန်းပြည့် shift)
{
    အကယ်၍ (cmd_silent)
	ပြန်ပေး;
    msg_no_more = TRUE;
    msg_putchar(c);
    အကယ်၍ (shift)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    msg_no_more = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_shift = shift;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    void
unputcmdline(void)
{
    အကယ်၍ (cmd_silent)
	ပြန်ပေး;
    msg_no_more = TRUE;
    အကယ်၍ (ccline.cmdlen == ccline.cmdpos)
	msg_putchar(' ');
    မဟုတ်ပါက အကယ်၍ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    မဟုတ်ပါက
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
    ကိန်းပြည့်
put_on_cmdline(char_u *str, ကိန်းပြည့် len, ကိန်းပြည့် redraw)
{
    ကိန်းပြည့်		retval;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		m;
    ကိန်းပြည့်		c;

    အကယ်၍ (len < 0)
	len = (ကိန်းပြည့်)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    အကယ်၍ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    မဟုတ်ပါက
	retval = OK;
    အကယ်၍ (retval == OK)
    {
	အကယ်၍ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		အတွက် (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		အတွက် (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		အကယ်၍ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		မဟုတ်ပါက
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    မဟုတ်ပါက အကယ်၍ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	အကယ်၍ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    နေစဉ် (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ifdef FEAT_ARABIC
	    အကယ်၍ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_head_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		အကယ်၍ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		မဟုတ်ပါက
		    i = 0;
	    }
#endif
	    အကယ်၍ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		msg_col -= i;
		အကယ်၍ (msg_col < 0)
		{
		    msg_col += cmdline_width;
		    --msg_row;
		}
	    }
	}

	အကယ်၍ (redraw && !cmd_silent)
	{
	    msg_no_more = TRUE;
	    i = cmdline_row;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    အကယ်၍ (cmdline_row != i || ccline.overstrike)
		msg_clr_eos();
	    msg_no_more = FALSE;
	}
	အကယ်၍ (KeyTyped)
	{
	    m = cmdline_width * Rows;
	    အကယ်၍ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	မဟုတ်ပါက
	    m = MAXCOL;
	အတွက် (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    အကယ်၍ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    အကယ်၍ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    အကယ်၍ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		အကယ်၍ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    အကယ်၍ (redraw)
	msg_check();
    ပြန်ပေး retval;
}

static cmdline_info_T	prev_ccline;
static ကိန်းပြည့်		prev_ccline_used = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    static void
save_cmdline(cmdline_info_T *ccp)
{
    အကယ်၍ (!prev_ccline_used)
    {
	CLEAR_FIELD(prev_ccline);
	prev_ccline_used = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = NULL;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    static void
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
    static ကိန်းပြည့်
cmdline_paste(
    ကိန်းပြည့် regname,
    ကိန်းပြည့် literally,	// Insert text literally instead of "as typed"
    ကိန်းပြည့် remcr)		// remove trailing CR
{
    long		i;
    char_u		*arg;
    char_u		*p;
    ကိန်းပြည့်			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    အကယ်၍ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ပြန်ပေး FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    line_breakcheck();
    အကယ်၍ (got_int)
	ပြန်ပေး FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    အကယ်၍ (i)
    {
	// Got the value of a special register in "arg".
	အကယ်၍ (arg == NULL)
	    ပြန်ပေး FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	အကယ်၍ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    ကိန်းပြည့်	    len;

	    // Locate start of last word in the cmd buffer.
	    အတွက် (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		အကယ်၍ (has_mbyte)
		{
		    len = (*mb_head_off)(ccline.cmdbuff, w - 1) + 1;
		    အကယ်၍ (!vim_iswordc(mb_ptr2char(w - len)))
			bha_nga;
		    w -= len;
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (!vim_iswordc(w[-1]))
			bha_nga;
		    --w;
		}
	    }
	    len = (ကိန်းပြည့်)((ccline.cmdbuff + ccline.cmdpos) - w);
	    အကယ်၍ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	အကယ်၍ (allocated)
	    vim_free(arg);
	ပြန်ပေး OK;
    }

    ပြန်ပေး cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    void
cmdline_paste_str(char_u *s, ကိန်းပြည့် literally)
{
    ကိန်းပြည့်		c, cv;

    အကယ်၍ (literally)
	put_on_cmdline(s, -1, TRUE);
    မဟုတ်ပါက
	နေစဉ် (*s != NUL)
	{
	    cv = *s;
	    အကယ်၍ (cv == Ctrl_V && s[1])
		++s;
	    အကယ်၍ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    မဟုတ်ပါက
		c = *s++;
	    အကယ်၍ (cv == Ctrl_V || c == ESC || c == Ctrl_C
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
    void
redrawcmdline(void)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    void
redrawcmdline_ex(ကိန်းပြည့် do_compute_cmdrow)
{
    အကယ်၍ (cmd_silent)
	ပြန်ပေး;
    need_wait_return = FALSE;
    အကယ်၍ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    static void
redrawcmdprompt(void)
{
    ကိန်းပြည့်		i;

    အကယ်၍ (cmd_silent)
	ပြန်ပေး;
    အကယ်၍ (ccline.cmdfirstc != NUL)
	msg_putchar(ccline.cmdfirstc);
    အကယ်၍ (ccline.cmdprompt != NULL)
    {
	msg_puts_attr((var_na *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = msg_col + (msg_row - cmdline_row) * cmdline_width;
	// do the reverse of set_cmdspos()
	အကယ်၍ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    မဟုတ်ပါက
	အတွက် (i = ccline.cmdindent; i > 0; --i)
	    msg_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    void
redrawcmd(void)
{
    ကိန်းပြည့် save_in_echowindow = in_echowindow;

    အကယ်၍ (cmd_silent)
	ပြန်ပေး;

    // when 'incsearch' is set there may be no command line while redrawing
    အကယ်၍ (ccline.cmdbuff == NULL)
    {
	windgoto(cmdline_row, cmdline_col_off);
	msg_clr_eos();
	ပြန်ပေး;
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
    အကယ်၍ (extra_char != NUL)
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

    void
compute_cmdrow(void)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    အကယ်၍ (exmode_active || (msg_scrolled != 0 && !updating_screen))
	cmdline_row = Rows - 1;
    မဟုတ်ပါက
	cmdline_row = W_WINROW(lastwin) + lastwin->w_height
						    + lastwin->w_status_height;
}

    void
cursorcmd(void)
{
    အကယ်၍ (cmd_silent)
	ပြန်ပေး;

#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (cmdmsg_rl)
    {
	msg_row = cmdline_row  + (ccline.cmdspos / (cmdline_width - 1));
	msg_col = cmdline_width - (ccline.cmdspos % (cmdline_width - 1)) - 1;
	အကယ်၍ (msg_row <= 0)
	    msg_row = Rows - 1;
    }
    မဟုတ်ပါက
#endif
    {
	msg_row = cmdline_row + (ccline.cmdspos / cmdline_width);
	msg_col = ccline.cmdspos % cmdline_width;
	အကယ်၍ (msg_row >= Rows)
	    msg_row = Rows - 1;
    }

    windgoto(msg_row, cmdline_col_off + msg_col);
#အကယ်၍ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    အကယ်၍ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#endif
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}

    void
gotocmdline(ကိန်းပြည့် clr)
{
    msg_start();
#ifdef FEAT_RIGHTLEFT
    အကယ်၍ (cmdmsg_rl)
	msg_col = cmdline_width - 1;
    မဟုတ်ပါက
#endif
	msg_col = 0;	    // always start in column 0
    အကယ်၍ (clr)		    // clear the bottom line(s)
	msg_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_row, cmdline_col_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    static ကိန်းပြည့်
ccheck_abbr(ကိန်းပြည့် c)
{
    ကိန်းပြည့် spos = 0;

    အကယ်၍ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ပြန်ပေး FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    နေစဉ် (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    အကယ်၍ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    မဟုတ်ပါက
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ပြန်ပေး check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, ကိန်းပြည့် what)
{
    char_u	*p;
#ifdef BACKSLASH_IN_FILENAME
    char_u	buf[20];
    ကိန်းပြည့်		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    အတွက် (p = what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS;
								*p != NUL; ++p)
	အကယ်၍ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#မဟုတ်ပါက
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_BUFFER ? BUFFER_ESC_CHARS : PATH_ESC_CHARS);
    အကယ်၍ (what == VSE_SHELL && csh_like_shell() && p != NULL)
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
    အကယ်၍ (p != NULL && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ပြန်ပေး p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    void
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    အကယ်၍ (p == NULL)
	ပြန်ပေး;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    void
tilde_replace(
    char_u  *orig_pat,
    ကိန်းပြည့်	    num_files,
    char_u  **files)
{
    ကိန်းပြည့်	    i;
    char_u  *p;

    အကယ်၍ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	အတွက် (i = 0; i < num_files; ++i)
	{
	    p = home_replace_save(NULL, files[i]);
	    အကယ်၍ (p != NULL)
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
get_cmdline_info(void)
{
    ပြန်ပေး &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    static cmdline_info_T *
get_ccline_ptr(void)
{
    အကယ်၍ ((State & MODE_CMDLINE) == 0)
	ပြန်ပေး NULL;
    အကယ်၍ (ccline.cmdbuff != NULL)
	ပြန်ပေး &ccline;
    အကယ်၍ (prev_ccline_used && prev_ccline.cmdbuff != NULL)
	ပြန်ပေး &prev_ccline;
    ပြန်ပေး NULL;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    static ကိန်းပြည့်
get_cmdline_type(void)
{
    cmdline_info_T *p = get_ccline_ptr();

    အကယ်၍ (p == NULL)
	ပြန်ပေး NUL;
    အကယ်၍ (p->cmdfirstc == NUL)
	ပြန်ပေး
#ifdef FEAT_EVAL
	    (p->input_fn) ? '@' :
#endif
	    '-';
    ပြန်ပေး p->cmdfirstc;
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    static char_u *
get_cmdline_str(void)
{
    cmdline_info_T *p;

    အကယ်၍ (cmdline_star > 0)
	ပြန်ပေး NULL;
    p = get_ccline_ptr();
    အကယ်၍ (p == NULL)
	ပြန်ပေး NULL;
    ပြန်ပေး vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    static char_u *
get_cmdline_completion_pattern(void)
{
    cmdline_info_T *p;
    ကိန်းပြည့်		xp_context;

    အကယ်၍ (cmdline_star > 0)
	ပြန်ပေး NULL;

    p = get_ccline_ptr();
    အကယ်၍ (p == NULL || p->xpc == NULL)
	ပြန်ပေး NULL;

    xp_context = p->xpc->xp_context;
    အကယ်၍ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    အကယ်၍ (xp_context == EXPAND_UNSUCCESSFUL)
	ပြန်ပေး NULL;

    char_u *compl_pat = p->xpc->xp_pattern;

    အကယ်၍ (compl_pat == NULL)
	ပြန်ပေး NULL;

    ပြန်ပေး vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    static char_u *
get_cmdline_completion(void)
{
    cmdline_info_T	*p;
    ကိန်းပြည့်			xp_context;

    အကယ်၍ (cmdline_star > 0)
	ပြန်ပေး NULL;

    p = get_ccline_ptr();
    အကယ်၍ (p == NULL || p->xpc == NULL)
	ပြန်ပေး NULL;

    xp_context = p->xpc->xp_context;
    အကယ်၍ (xp_context == EXPAND_NOTHING)
    {
	set_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    အကယ်၍ (xp_context == EXPAND_UNSUCCESSFUL)
	ပြန်ပေး NULL;

    ပြန်ပေး cmdcomplete_type_to_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    void
f_getcmdcomplpat(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_pattern();
}

/*
 * "getcmdcompltype()" function
 */
    void
f_getcmdcompltype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    void
f_getcmdline(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    void
f_getcmdpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_number = p != NULL ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    void
f_getcmdprompt(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != NULL && p->cmdprompt != NULL ?
					    vim_strsave(p->cmdprompt) : NULL;
}

/*
 * "getcmdscreenpos()" function
 */
    void
f_getcmdscreenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_number = p != NULL ? cmdline_col_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    void
f_getcmdtype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    အကယ်၍ (rettv->vval.v_string == NULL)
	ပြန်ပေး;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    static ကိန်းပြည့်
set_cmdline_str(char_u *str, ကိန်းပြည့် pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    ကိန်းပြည့်		    len;

    အကယ်၍ (p == NULL)
	ပြန်ပေး 1;

    len = (ကိန်းပြည့်)STRLEN(str);
    အကယ်၍ (realloc_cmdbuff(len + 1) != OK)
	ပြန်ပေး 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ပြန်ပေး 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    static ကိန်းပြည့်
set_cmdline_pos(
    ကိန်းပြည့်		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    အကယ်၍ (p == NULL)
	ပြန်ပေး 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    အကယ်၍ (pos < 0)
	new_cmdpos = 0;
    မဟုတ်ပါက
	new_cmdpos = pos;

    ပြန်ပေး 0;
}

// "setcmdline()" function
    void
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    ကိန်းပြည့် pos = -1;

    အကယ်၍ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_number_arg(argvars, 1) == FAIL)
	ပြန်ပေး;

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ကိန်းပြည့် error = FALSE;

	pos = (ကိန်းပြည့်)tv_get_number_chk(&argvars[1], &error) - 1;
	အကယ်၍ (error)
	    ပြန်ပေး;
	အကယ်၍ (pos < 0)
	{
	    emsg(_(e_argument_must_be_positive));
	    ပြန်ပေး;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_number = set_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    void
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    ကိန်းပြည့်		pos;

    အကယ်၍ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    pos = (ကိန်းပြည့်)tv_get_number(&argvars[0]) - 1;
    အကယ်၍ (pos >= 0)
	rettv->vval.v_number = set_cmdline_pos(pos);
}
#endif

/*
 * Return the first character of the current command line.
 */
    ကိန်းပြည့်
get_cmdline_firstc(void)
{
    ပြန်ပေး ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    ကိန်းပြည့်
get_list_range(char_u **str, ကိန်းပြည့် *num1, ကိန်းပြည့် *num2)
{
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		first = FALSE;
    varnumber_T	num;

    *str = skipwhite(*str);
    အကယ်၍ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, NULL, &len, 0, &num, NULL, 0, FALSE, NULL);
	*str += len;
	// overflow
	အကယ်၍ (num > INT_MAX)
	    ပြန်ပေး FAIL;

	*num1 = (ကိန်းပြည့်)num;
	first = TRUE;
    }
    *str = skipwhite(*str);
    အကယ်၍ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, NULL, &len, 0, &num, NULL, 0, FALSE, NULL);
	အကယ်၍ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    // overflow
	    အကယ်၍ (num > INT_MAX)
		ပြန်ပေး FAIL;

	    *num2 = (ကိန်းပြည့်)num;
	}
	မဟုတ်ပါက အကယ်၍ (!first)		// no number given at all
	    ပြန်ပေး FAIL;
    }
    မဟုတ်ပါက အကယ်၍ (first)			// only one number given
	*num2 = *num1;
    ပြန်ပေး OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    var_na *
did_set_cedit(optset_T *args UNUSED)
{
    ကိန်းပြည့် n;

    အကယ်၍ (*p_cedit == NUL)
	cedit_key = -1;
    မဟုတ်ပါက
    {
	n = string_to_key(p_cedit, FALSE);
	အကယ်၍ (n == 0 || vim_isprintc(n))
	    ပြန်ပေး e_invalid_argument;
	cedit_key = n;
    }
    ပြန်ပေး NULL;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    static ကိန်းပြည့်
open_cmdwin(void)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    ကိန်းပြည့်			i;
    linenr_T		lnum;
    ကိန်းပြည့်			histtype;
    garray_T		winsizes;
    ကိန်းပြည့်			save_restart_edit = restart_edit;
    ကိန်းပြည့်			save_State = State;
    ကိန်းပြည့်			save_exmode = exmode_active;
#ifdef FEAT_RIGHTLEFT
    ကိန်းပြည့်			save_cmdmsg_rl = cmdmsg_rl;
#endif
#ifdef FEAT_FOLDING
    ကိန်းပြည့်			save_KeyTyped;
#endif
    ကိန်းပြည့်			newbuf_status;
    ကိန်းပြည့်			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    အကယ်၍ (text_or_buf_locked()
	    || cmdwin_type != 0
#အကယ်၍ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_star > 0
#endif
	    )
    {
	beep_flush();
	ပြန်ပေး K_IGNORE;
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
    အကယ်၍ (win_split((ကိန်းပြည့်)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ပြန်ပေး K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    အကယ်၍ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ပြန်ပေး Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_status = do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, ECMD_HIDE,
					NULL);
    cmdwin_valid = win_valid(cmdwin_win);
    အကယ်၍ (newbuf_status == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_buffer != old_curbuf.br_buf)
    {
	အကယ်၍ (newbuf_status == OK)
	    set_bufref(&bufref, curbuf);
	အကယ်၍ (cmdwin_valid && !last_window())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	အကယ်၍ (newbuf_status == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_buffer(NULL, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = NULL;
	beep_flush();
	ga_clear(&winsizes);
	ပြန်ပေး Ctrl_C;
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
    အကယ်၍ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	အကယ်၍ (p_wc == TAB)
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
    အကယ်၍ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	အကယ်၍ (i >= 0)
	{
	    lnum = 0;
	    do
	    {
		အကယ်၍ (++i == get_hislen())
		    i = 0;
		အကယ်၍ (get_histentry(histtype)[i].hisstr != NULL)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    နေစဉ် (i != *get_hisidx(histtype));
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
    အကယ်၍ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    ကိန်းပြည့် save_RedrawingDisabled = RedrawingDisabled;
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
    cmdwin_buf = NULL;
    cmdwin_win = NULL;
    exmode_active = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    အကယ်၍ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_buffer != old_curbuf.br_buf)
    {
	cmdwin_result = Ctrl_C;
	emsg(_(e_active_window_or_buffer_changed_or_deleted));
    }
    မဟုတ်ပါက
    {
#အကယ်၍ defined(FEAT_EVAL)
	// autocmds may abort script processing
	အကယ်၍ (aborting() && cmdwin_result != K_IGNORE)
	    cmdwin_result = Ctrl_C;
#endif
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	အကယ်၍ (cmdwin_result == K_XF1 || cmdwin_result == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    အကယ်၍ (cmdwin_result == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    အကယ်၍ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (ကိန်းပြည့်)plen;
		ccline.cmdbufflen = (ကိန်းပြည့်)(plen + 1);
		cmdwin_result = CAR;
	    }
	    မဟုတ်ပါက
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	မဟုတ်ပါက အကယ်၍ (cmdwin_result == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = NULL;
	}
	မဟုတ်ပါက
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	အကယ်၍ (ccline.cmdbuff == NULL)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_result = Ctrl_C;
	}
	မဟုတ်ပါက
	{
	    ccline.cmdpos = curwin->w_cursor.col;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    အကယ်၍ (ccline.cmdpos == ccline.cmdlen - 1
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
	အကယ်၍ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	အကယ်၍ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_buffer(NULL, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_size_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	အကယ်၍ (cmdwin_result == K_IGNORE)
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

    ပြန်ပေး cmdwin_result;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    ကိန်းပြည့်
is_in_cmdwin(void)
{
    ပြန်ပေး cmdwin_type != 0 && get_cmdline_type() == NUL;
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

    အကယ်၍ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == NULL)
	ပြန်ပေး NULL;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    အကယ်၍ (l == NULL)
	ပြန်ပေး NULL;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ပြန်ပေး (char_u *)ga.ga_data;
#မဟုတ်ပါက
    ပြန်ပေး NULL;
#endif
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    void
get_user_input(
    typval_T	*argvars,
    typval_T	*rettv,
    ကိန်းပြည့်		inputdialog,
    ကိန်းပြည့်		secret)
{
    char_u	*prompt;
    char_u	*p = NULL;
    ကိန်းပြည့်		c;
    char_u	buf[NUMBUFLEN];
    ကိန်းပြည့်		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    ကိန်းပြည့်		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = NULL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    အကယ်၍ (input_busy)
	ပြန်ပေး;  // this doesn't work recursively.

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_INPUT
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    အကယ်၍ (no_console_input() && !is_not_a_term())
	ပြန်ပေး;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    အကယ်၍ (prompt != NULL)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	အကယ်၍ (p == NULL)
	    p = prompt;
	မဟုတ်ပါက
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    msg_start();
	    msg_clr_eos();
	    msg_puts_attr((var_na *)prompt, get_echo_attr());
	    msg_didout = FALSE;
	    msg_starthere();
	    *p = c;
	}
	cmdline_row = msg_row;

	အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    အကယ်၍ (defstr != NULL)
		stuffReadbuffSpec(defstr);

	    အကယ်၍ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		ကိန်းပြည့်	xp_namelen;
		long	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = NULL;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		အကယ်၍ (xp_name == NULL)
		    ပြန်ပေး;

		xp_namelen = (ကိန်းပြည့်)STRLEN(xp_name);

		အကယ်၍ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ပြန်ပေး;
	    }
	}

	အကယ်၍ (defstr != NULL)
	{
	    ကိန်းပြည့် save_ex_normal_busy = ex_normal_busy;
	    ကိန်းပြည့် save_vgetc_busy = vgetc_busy;
	    ကိန်းပြည့် save_input_busy = input_busy;

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
	အကယ်၍ (inputdialog && rettv->vval.v_string == NULL
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
    void
f_wildtrigger(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    အကယ်၍ (!(State & MODE_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_active())
	ပြန်ပေး;

    ကိန်းပြည့် cmd_type = get_cmdline_type();

    အကယ်၍ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
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
