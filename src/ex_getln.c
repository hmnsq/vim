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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// Return value when handling keys in command-line mode.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMDLINE_NOT_ਬਦਲਿਆ	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMDLINE_ਬਦਲਿਆ		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROCESS_ਅਗਲਾ_ਕੁੰਜੀ	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ਸਥਿਰ cmdline_info_T ccline;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ	new_cmdpos;	// position set by set_cmdline_pos()
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ਸਥਿਰ ਪੂਰਨ	extra_char_ਖਿਸਕਾਓ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
ਸਥਿਰ ਪੂਰਨ	cmd_hkmap = 0;	// Hebrew mapping during command line
#ਜੇ_ਅੰਤ

ਸਥਿਰ char_u	*getcmdline_int(ਪੂਰਨ firstc, ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ indent, ਪੂਰਨ clear_ccline);
ਸਥਿਰ ਪੂਰਨ	cmdline_charsize(ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ	ਸੈੱਟ_cmdspos(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	ਸੈੱਟ_cmdspos_cursor(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	correct_cmdspos(ਪੂਰਨ idx, ਪੂਰਨ cells);
ਸਥਿਰ ਖਾਲੀ	dealloc_cmdbuff(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	alloc_cmdbuff(size_t len);
ਸਥਿਰ ਖਾਲੀ	draw_cmdline(ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ len);
ਸਥਿਰ ਖਾਲੀ	save_cmdline(cmdline_info_T *ccp);
ਸਥਿਰ ਖਾਲੀ	restore_cmdline(cmdline_info_T *ccp);
ਸਥਿਰ ਪੂਰਨ	cmdline_paste(ਪੂਰਨ regname, ਪੂਰਨ literally, ਪੂਰਨ remcr);
ਸਥਿਰ ਖਾਲੀ	redrawcmdprompt(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	ccheck_abbr(ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ	open_cmdwin(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
ਸਥਿਰ ਪੂਰਨ	ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ_magic(char_u *pat, size_t len, magic_T magic_val);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	cedit_ਕੁੰਜੀ = -1;	// key value of 'cedit' option

    ਸਥਿਰ ਖਾਲੀ
trigger_cmd_autocmd(ਪੂਰਨ typechar, ਪੂਰਨ evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    ਸਥਿਰ ਖਾਲੀ
abandon_cmdline(ਖਾਲੀ)
{
    dealloc_cmdbuff();
    ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
	compute_cmdrow();
    ਸੁਨੇਹਾ("");
    redraw_cmdline = TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ(char_u *p, size_t len, ਪੂਰਨ delim)
{
    magic_T	magic_val = MAGIC_ON;

    ਜੇ (len > 0)
	(ਖਾਲੀ) skip_regexp_ex(p, delim, magic_isset(), ਨਿਰਾ, ਨਿਰਾ, &magic_val);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ TRUE;

    ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ_magic(p, len, magic_val);
}

    ਸਥਿਰ ਪੂਰਨ
ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    ਜਦੋਂ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ਨਿਰਾ)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ਵਾਪਸ len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    ਪੂਰਨ		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
} viewstate_T;

    ਸਥਿਰ ਖਾਲੀ
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
    vs->vs_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
}

    ਸਥਿਰ ਖਾਲੀ
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
    curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = vs->vs_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
}

// Struct to store the state of 'incsearch' highlighting.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    pos_T	ਖੋਜ_ਸ਼ੁਰੂ_2;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    ਪੂਰਨ		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	ਮੇਲ_ਸ਼ੁਰੂ;
    pos_T	ਮੇਲ_ਸਿਰਾ;
    ਪੂਰਨ		did_incsearch;
    ਪੂਰਨ		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ਸਥਿਰ ਖਾਲੀ
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->ਮੇਲ_ਸ਼ੁਰੂ = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->ਮੇਲ_ਸਿਰਾ);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਖੋਜ_ਮੇਲ(pos_T *t)
{
    t->lnum += ਖੋਜ_ਮੇਲ_ਸਤਰਾਂ;
    t->ਖਾਨਾ = ਖੋਜ_ਮੇਲ_endcol;
    ਜੇ (t->lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	t->lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    ਪੂਰਨ
parse_ਨਮੂਨਾ_and_range(
	pos_T	*incsearch_ਸ਼ੁਰੂ,
	ਪੂਰਨ	*ਖੋਜ_delim,
	ਪੂਰਨ	*skiplen,
	ਪੂਰਨ	*patlen)
{
    char_u	*cmd, *p, *ਸਿਰਾ;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    ਪੂਰਨ		delim_optional = FALSE;
    ਪੂਰਨ		delim;
    ਪੂਰਨ		use_ਆਖਰੀ_pat;
    magic_T     magic = 0;
    ਅੱਖਰ	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = 0;
    ਖੋਜ_ਆਖਰੀ_ਸਤਰ = MAXLNUM;

    CLEAR_ਖੇਤਰ(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_ਸਤਰਾਂ;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ਨਿਰਾ);

    ਜੇ (vim_strchr((char_u *)"sgvlu", *cmd) == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // Skip over command name to find pattern separator
    ਲਈ (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    ਜੇ (*skipwhite(p) == NUL)
	ਵਾਪਸ FALSE;

    ਜੇ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	ਜੇ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	ਜੇ (*p == '!')
	    p = skipwhite(p + 1);
	ਜਦੋਂ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	ਜੇ (*p == NUL)
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	ਜੇ (*p == '!')
	{
	    p++;
	    ਜੇ (*skipwhite(p) == NUL)
		ਵਾਪਸ FALSE;
	}
	ਜੇ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *ਖੋਜ_delim = delim;

    ਸਿਰਾ = skip_regexp_ex(p, delim, magic_isset(), ਨਿਰਾ, ਨਿਰਾ, &magic);
    use_ਆਖਰੀ_pat = ਸਿਰਾ == p && *ਸਿਰਾ == delim;

    ਜੇ (ਸਿਰਾ == p && !use_ਆਖਰੀ_pat)
	ਵਾਪਸ FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    ਜੇ (!use_ਆਖਰੀ_pat)
    {
	ਅੱਖਰ c = *ਸਿਰਾ;
	ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_2;

	*ਸਿਰਾ = NUL;
	ਖਾਲੀ_ਥਾਵਾਂ_2 = ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ_magic(p, (size_t)(ਸਿਰਾ - p), magic);
	*ਸਿਰਾ = c;
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2)
	    ਵਾਪਸ FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (ਪੂਰਨ)(p - ccline.cmdbuff);
    *patlen = (ਪੂਰਨ)(ਸਿਰਾ - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_ਸ਼ੁਰੂ;

    parse_cmd_ਪਤਾ(&ea, &dummy, TRUE);

    ਜੇ (ea.addr_ਗਿਣਤੀ > 0)
    {
	ਪੂਰਨ ਉਲਟ_ਮੇਲ = ea.line2 < ea.line1;
	ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = ਉਲਟ_ਮੇਲ ? ea.line2 : ea.line1;
	ਖੋਜ_ਆਖਰੀ_ਸਤਰ = ਉਲਟ_ਮੇਲ ? ea.line1 : ea.line2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = ਖੋਜ_ਆਖਰੀ_ਸਤਰ = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ਵਾਪਸ TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
do_incsearch_highlighting(
	ਪੂਰਨ		    firstc,
	ਪੂਰਨ		    *ਖੋਜ_delim,
	incsearch_state_T   *is_state,
	ਪੂਰਨ		    *skiplen,
	ਪੂਰਨ		    *patlen)
{
    ਪੂਰਨ retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    ਜੇ (!p_is || cmd_silent)
	ਵਾਪਸ FALSE;

    // By default search all lines
    ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = 0;
    ਖੋਜ_ਆਖਰੀ_ਸਤਰ = MAXLNUM;

    ਜੇ (firstc == '/' || firstc == '?')
    {
	*ਖੋਜ_delim = firstc;
	ਵਾਪਸ TRUE;
    }

    ਜੇ (firstc != ':')
	ਵਾਪਸ FALSE;

    ++emsg_off;
    retval = parse_ਨਮੂਨਾ_and_range(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2, ਖੋਜ_delim,
	    skiplen, patlen);
    --emsg_off;

    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਖਾਲੀ
ਅੰਤ_incsearch_highlighting(
	ਪੂਰਨ gotesc,
	incsearch_state_T *is_state,
	ਪੂਰਨ call_update_screen)
{
    ਜੇ (!is_state->did_incsearch)
	ਵਾਪਸ;

    is_state->did_incsearch = FALSE;
    ਜੇ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!EQUAL_POS(is_state->save_cursor, is_state->ਖੋਜ_ਸ਼ੁਰੂ_2))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->ਖੋਜ_ਸ਼ੁਰੂ_2;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_ਮੇਲ = FALSE;

    // by default search all lines
    ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = 0;
    ਖੋਜ_ਆਖਰੀ_ਸਤਰ = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    ਸਥਿਤੀ_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    ਜੇ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ਸਥਿਰ ਖਾਲੀ
may_do_incsearch_highlighting(
	ਪੂਰਨ		    firstc,
	ਲੰਮਾ		    ਗਿਣਤੀ_2,
	incsearch_state_T   *is_state)
{
    ਪੂਰਨ		skiplen, patlen;
    ਪੂਰਨ		ਮਿਲਿਆ;  // do_search() result
    pos_T	ਸਿਰਾ_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    ਪੂਰਨ		ਅਗਲਾ_char;
    ਪੂਰਨ		use_ਆਖਰੀ_pat;
    ਪੂਰਨ		did_do_incsearch = is_state->did_incsearch;
    ਪੂਰਨ		ਖੋਜ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();

    ਜੇ (!do_incsearch_highlighting(firstc, &ਖੋਜ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
	ਅੰਤ_incsearch_highlighting(FALSE, is_state, TRUE);
	ਜੇ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ਵਾਪਸ;
    }

    // If there is a character waiting, search and redraw later.
    ਜੇ (char_avail())
    {
	restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
	is_state->incsearch_postponed = TRUE;
	ਵਾਪਸ;
    }
    is_state->incsearch_postponed = FALSE;

    ਜੇ (ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->ਖੋਜ_ਸ਼ੁਰੂ_2;
    ਨਹੀਂਤਾਂ ਜੇ (ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	curwin->w_cursor.ਖਾਨਾ = MAXCOL;
    }
    ਨਹੀਂਤਾਂ
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ;
	curwin->w_cursor.ਖਾਨਾ = 0;
    }

    // Use the previous pattern for ":s//".
    ਅਗਲਾ_char = ccline.cmdbuff[skiplen + patlen];
    use_ਆਖਰੀ_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == ਅਗਲਾ_char;

    // If there is no pattern, don't do anything.
    ਜੇ (patlen == 0 && !use_ਆਖਰੀ_pat)
    {
	ਮਿਲਿਆ = 0;
	ਸੈੱਟ_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ ਖੋਜ_flags = ਖੋਜ_OPT + ਖੋਜ_NOOF + ਖੋਜ_PEEK;

	cursor_off();	// so the user knows we're busy
	ਬਾਹਰਲਾ_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	ਜੇ (!p_hls)
	    ਖੋਜ_flags += ਖੋਜ_KEEP;
	ਜੇ (ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ != 0)
	    ਖੋਜ_flags += ਖੋਜ_ਸ਼ੁਰੂ;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_ਖੇਤਰ(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	ਮਿਲਿਆ = do_ਖੋਜ(ਨਿਰਾ, firstc == ':' ? '/' : firstc, ਖੋਜ_delim,
				 ccline.cmdbuff + skiplen, patlen, ਗਿਣਤੀ_2, ਖੋਜ_flags,
# ifdef FEAT_RELTIME
		&sia
# ਨਹੀਂਤਾਂ
		ਨਿਰਾ
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = ਅਗਲਾ_char;
	--emsg_off;

	ਜੇ (curwin->w_cursor.lnum < ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ
		|| curwin->w_cursor.lnum > ਖੋਜ_ਆਖਰੀ_ਸਤਰ)
	{
	    // match outside of address range
	    ਮਿਲਿਆ = 0;
	    curwin->w_cursor = is_state->ਖੋਜ_ਸ਼ੁਰੂ_2;
	}

	// if interrupted while searching, behave like it failed
	ਜੇ (got_int)
	{
	    (ਖਾਲੀ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    ਮਿਲਿਆ = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    ਜੇ (ਮਿਲਿਆ != 0)
	highlight_ਮੇਲ = TRUE;		// highlight position
    ਨਹੀਂਤਾਂ
	highlight_ਮੇਲ = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    ਬਦਲਿਆ_cline_bef_curs();
    update_topline();

    ਜੇ (ਮਿਲਿਆ != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->ਮੇਲ_ਸ਼ੁਰੂ = curwin->w_cursor;
	ਸੈੱਟ_ਖੋਜ_ਮੇਲ(&curwin->w_cursor);
	validate_cursor();
	ਸਿਰਾ_pos = curwin->w_cursor;
	is_state->ਮੇਲ_ਸਿਰਾ = ਸਿਰਾ_pos;
	curwin->w_cursor = save_pos;
    }
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    ਜੇ (!use_ਆਖਰੀ_pat)
    {
	ਅਗਲਾ_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_ਨਮੂਨਾ(ccline.cmdbuff + skiplen, (size_t)patlen, ਖੋਜ_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    ਸੈੱਟ_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = ਅਗਲਾ_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    ਜੇ (p_ru && curwin->w_ਸਥਿਤੀ_ਉਚਾਈ > 0)
	curwin->w_redr_ਸਥਿਤੀ = true;

    update_screen(UPD_SOME_VALID);
    highlight_ਮੇਲ = FALSE;
    restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    ਜੇ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->ਖੋਜ_ਸ਼ੁਰੂ_2;
    ਨਹੀਂਤਾਂ ਜੇ (ਮਿਲਿਆ != 0)
	curwin->w_cursor = ਸਿਰਾ_pos;

    ਸੁਨੇਹਾ_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ਸਥਿਰ ਪੂਰਨ
may_adjust_incsearch_highlighting(
	ਪੂਰਨ			firstc,
	ਲੰਮਾ			ਗਿਣਤੀ_2,
	incsearch_state_T	*is_state,
	ਪੂਰਨ			c)
{
    ਪੂਰਨ		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ਨਿਰਾ;
    char_u	*searchstr;
    char_u	*strcopy = ਨਿਰਾ;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	ਵਿੱਥ;
    ਪੂਰਨ		ਖੋਜ_flags = ਖੋਜ_NOOF;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਖੋਜ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();

    ਜੇ (!do_incsearch_highlighting(firstc, &ਖੋਜ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
	ਵਾਪਸ OK;
    }
    ਜੇ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
	ਵਾਪਸ FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (ਖਾਲੀ)parse_ਖੋਜ_ਨਮੂਨਾ_ਵਿੱਥ(&pat, &patlen_s, ਖੋਜ_delim,
				    ਖੋਜ_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &ਵਿੱਥ);

    cursor_off();
    ਬਾਹਰਲਾ_flush();
    ਜੇ (c == Ctrl_G)
    {
	t = is_state->ਮੇਲ_ਸਿਰਾ;
	ਜੇ (LT_POS(is_state->ਮੇਲ_ਸ਼ੁਰੂ, is_state->ਮੇਲ_ਸਿਰਾ))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (ਖਾਲੀ)decl(&t);
	ਖੋਜ_flags += ਖੋਜ_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
	t = is_state->ਮੇਲ_ਸ਼ੁਰੂ;
    ਜੇ (!p_hls)
	ਖੋਜ_flags += ਖੋਜ_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ਨਿਰਾ,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, ਗਿਣਤੀ_2, ਖੋਜ_flags, RE_ਖੋਜ, ਨਿਰਾ);
    --emsg_off;
    ਜੇ (dircp != ਨਿਰਾ)
	*dircp = ਖੋਜ_delim;
    ਜੇ (i)
    {
	pos_T	ਮੇਲ_ਸ਼ੁਰੂ = is_state->ਮੇਲ_ਸ਼ੁਰੂ;
	pos_T	ਮੇਲ_ਸਿਰਾ = is_state->ਮੇਲ_ਸਿਰਾ;
	ਲੰਮਾ	off = ਵਿੱਥ.off;

	is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = ਮੇਲ_ਸ਼ੁਰੂ;
	ਜੇ (!ਵਿੱਥ.ਸਤਰ && (ਵਿੱਥ.ਸਿਰਾ || off != 0))
	{
	    ਜੇ (ਵਿੱਥ.ਸਿਰਾ)
	    {
		is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = ਮੇਲ_ਸਿਰਾ;
		(ਖਾਲੀ)decl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2);
	    }
	    ਜਦੋਂ (off > 0)
	    {
		ਜੇ (incl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2) == -1)
		    ਰੋਕੋ;
		--off;
	    }
	    ਜਦੋਂ (off < 0)
	    {
		ਜੇ (decl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2) == -1)
		    ਰੋਕੋ;
		++off;
	    }
	}
	is_state->ਮੇਲ_ਸਿਰਾ = t;
	is_state->ਮੇਲ_ਸ਼ੁਰੂ = t;
	ਜੇ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = t;
	    (ਖਾਲੀ)decl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = t;
	    (ਖਾਲੀ)incl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2);
	}
	ਜੇ (LT_POS(t, is_state->ਖੋਜ_ਸ਼ੁਰੂ_2) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->ਖੋਜ_ਸ਼ੁਰੂ_2 = t;
	    ਜੇ (firstc == '?')
		(ਖਾਲੀ)incl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2);
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)decl(&is_state->ਖੋਜ_ਸ਼ੁਰੂ_2);
	}

	ਸੈੱਟ_ਖੋਜ_ਮੇਲ(&is_state->ਮੇਲ_ਸਿਰਾ);
	curwin->w_cursor = is_state->ਮੇਲ_ਸ਼ੁਰੂ;
	ਬਦਲਿਆ_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_ਮੇਲ = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_ਮੇਲ = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->ਮੇਲ_ਸਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
    ਵਾਪਸ FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ਸਥਿਰ ਪੂਰਨ
may_ਜੋੜੋ_char_ਮੰਜ਼ਿਲ_ਖੋਜ(ਪੂਰਨ firstc, ਪੂਰਨ *c, incsearch_state_T *is_state)
{
    ਪੂਰਨ		skiplen, patlen, ਖੋਜ_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();

    ਜੇ (!do_incsearch_highlighting(firstc, &ਖੋਜ_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();
	ਵਾਪਸ FAIL;
    }
    restore_ਆਖਰੀ_ਖੋਜ_ਨਮੂਨਾ();

    // Add a character from under the cursor for 'incsearch'.
    ਜੇ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->ਮੇਲ_ਸਿਰਾ;
	*c = gchar_cursor();
	ਜੇ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    ਜੇ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    ਜੇ (*c == ਖੋਜ_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ਨਿਰਾ)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    ਜੇ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		ਪੂਰਨ save_c = *c;

		ਜਦੋਂ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.ਖਾਨਾ += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_has_arabic(ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ len)
{
    ਪੂਰਨ	    j;
    ਪੂਰਨ	    mb_l;
    ਪੂਰਨ	    u8c;
    char_u  *p;
    ਪੂਰਨ	    u8cc[MAX_MCO];

    ਜੇ (!enc_utf8)
	ਵਾਪਸ FALSE;

    ਲਈ (j = ਸ਼ੁਰੂ; j < ਸ਼ੁਰੂ + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, ਸ਼ੁਰੂ + len - j);
	mb_l = utfc_ptr2len_len(p, ਸ਼ੁਰੂ + len - j);
	ਜੇ (ARABIC_CHAR(u8c))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
cmdline_init(ਖਾਲੀ)
{
    CLEAR_ਖੇਤਰ(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_handle_ctrl_bsl(ਪੂਰਨ c, ਪੂਰਨ *gotesc)
{
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    ਜੇ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && ਕੁੰਜੀ_Typed)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		|| cmdline_ਤਾਰਾ > 0
#ਜੇ_ਅੰਤ
		))
    {
	vungetc(c);
	ਵਾਪਸ PROCESS_ਅਗਲਾ_ਕੁੰਜੀ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	ਜੇ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	ਨਹੀਂਤਾਂ
	    new_cmdpos = ccline.cmdpos;

	c = get_ਵਿਅੰਜਕ_ਰਜਿਸਟਰ();
	ਜੇ (c == '=')
	{
	    char_u	*p = ਨਿਰਾ;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_ਵਿਅੰਜਕ_ਸਤਰ();
	    --textlock;

	    ਜੇ (p != ਨਿਰਾ)
	    {
		ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(p);

		ਜੇ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    ਜੇ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    ਨਹੀਂਤਾਂ
			ccline.cmdpos = new_cmdpos;

		    ਕੁੰਜੀ_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ਵਾਪਸ CMDLINE_ਬਦਲਿਆ;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ਵਾਪਸ CMDLINE_NOT_ਬਦਲਿਆ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ਵਾਪਸ GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_wildchar_complete(
	ਪੂਰਨ		c,
	ਪੂਰਨ		escape,
	ਪੂਰਨ		*did_wild_list,
	ਪੂਰਨ		*wim_ਸੂਚਕਾਂਕ_p,
	expand_T	*xp,
	ਪੂਰਨ		*gotesc,
	ਪੂਰਨ		redraw_if_menu_ਖਾਲੀ_ਥਾਵਾਂ,
	pos_T		*pre_incsearch_pos)
{
    ਪੂਰਨ		wim_ਸੂਚਕਾਂਕ = *wim_ਸੂਚਕਾਂਕ_p;
    ਪੂਰਨ		res;
    ਪੂਰਨ		cmdpos_before;
    ਪੂਰਨ		options = WILD_NO_BEEP;
    ਪੂਰਨ		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    ਪੂਰਨ		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    ਜੇ (wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    ਜੇ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	ਜੇ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_LIST))
	{
	    (ਖਾਲੀ)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_ਸੂਚਕਾਂਕ] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	ਜੇ (wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ਨਹੀਂਤਾਂ ਜੇ (wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_ਭਰਿਆ)
	    res = nextwild(xp, WILD_ਅਗਲਾ, options, escape);
	ਨਹੀਂਤਾਂ
	    res = OK;	    // don't insert 'wildchar' now
    }
    ਨਹੀਂਤਾਂ		    // typed p_wc first time
    {
	ਪੂਰਨ wim_longest = (wim_flags[0] & WIM_LONGEST);
	ਪੂਰਨ wim_list = (wim_flags[0] & WIM_LIST);
	ਪੂਰਨ wim_ਭਰਿਆ = (wim_flags[0] & WIM_ਭਰਿਆ);

	wim_ਸੂਚਕਾਂਕ = 0;
	ਜੇ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_ਨਮੂਨਾ;
	    ਜੇ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    ਜੇ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    ਨਹੀਂਤਾਂ
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	ਜੇ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (wim_noselect || (wim_list && !wim_ਭਰਿਆ))
		options |= WILD_NOSELECT;
	    ਜੇ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	ਜੇ (redraw_if_menu_ਖਾਲੀ_ਥਾਵਾਂ && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	ਜੇ (got_int)
	{
	    (ਖਾਲੀ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (ਖਾਲੀ)ExpandOne(xp, ਨਿਰਾ, ਨਿਰਾ, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_ਸੂਚਕਾਂਕ_p = wim_ਸੂਚਕਾਂਕ;
	    ਵਾਪਸ CMDLINE_ਬਦਲਿਆ;
	}

	// Display matches
	ਜੇ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    ਜੇ (wim_longest)
	    {
		ਪੂਰਨ ਮਿਲਿਆ_longest_ਅਗੇਤਰ = (ccline.cmdpos != cmdpos_before);
		ਪੂਰਨ show_menu = p_wmnu && wim_ਭਰਿਆ;

		ਜੇ (wim_list || show_menu)
		    (ਖਾਲੀ)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		ਨਹੀਂਤਾਂ ਜੇ (!ਮਿਲਿਆ_longest_ਅਗੇਤਰ)
		{
		    ਪੂਰਨ wim_list_ਅਗਲਾ = (wim_flags[1] & WIM_LIST);
		    ਪੂਰਨ wim_ਭਰਿਆ_ਅਗਲਾ = (wim_flags[1] & WIM_ਭਰਿਆ);
		    ਪੂਰਨ wim_noselect_ਅਗਲਾ = (wim_flags[1] & WIM_NOSELECT);
		    ਪੂਰਨ wim_noinsert_ਅਗਲਾ = (wim_flags[1] & WIM_NOINSERT);
		    ਜੇ (wim_list_ਅਗਲਾ || (p_wmnu && (wim_ਭਰਿਆ_ਅਗਲਾ
				    || wim_noselect_ਅਗਲਾ || wim_noinsert_ਅਗਲਾ)))
		    {
			ਜੇ (wim_ਭਰਿਆ_ਅਗਲਾ && !wim_noselect_ਅਗਲਾ && !wim_noinsert_ਅਗਲਾ)
			    nextwild(xp, WILD_ਅਗਲਾ, options, escape);
			ਨਹੀਂਤਾਂ
			    (ਖਾਲੀ)showmatches(xp, p_wmnu && (wim_noselect_ਅਗਲਾ
					|| wim_noinsert_ਅਗਲਾ), wim_list_ਅਗਲਾ,
				    p_wmnu ? wim_flags[1] : 0);

			ਜੇ (wim_list_ਅਗਲਾ)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ show_menu = p_wmnu && (wim_ਭਰਿਆ || wim_noselect
							    || wim_noinsert);

		ਜੇ (wim_list || show_menu)
		    (ਖਾਲੀ)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		ਨਹੀਂਤਾਂ
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    ਜੇ (wim_list)
		*did_wild_list = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    ਜੇ (wim_ਸੂਚਕਾਂਕ < 3)
	++wim_ਸੂਚਕਾਂਕ;
    ਜੇ (c == ESC)
	*gotesc = TRUE;

    *wim_ਸੂਚਕਾਂਕ_p = wim_ਸੂਚਕਾਂਕ;
    ਵਾਪਸ (res == OK) ? CMDLINE_ਬਦਲਿਆ : CMDLINE_NOT_ਬਦਲਿਆ;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_erase_chars(
	ਪੂਰਨ c,
	ਪੂਰਨ indent
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	, incsearch_state_T *isp
#ਜੇ_ਅੰਤ
	)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;

    ਜੇ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    ਜੇ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    ਜੇ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_ਅਗਲਾ(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    ਜੇ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	ਜੇ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    ਜੇ (c == Ctrl_W)
	    {
		ਜਦੋਂ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		ਜਦੋਂ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		ਜੇ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_W)
	{
	    ਜਦੋਂ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    ਜੇ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		ਜਦੋਂ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	ਨਹੀਂਤਾਂ
	    --p;
	ccline.cmdpos = (ਪੂਰਨ)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	ਜਦੋਂ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਜੇ (ccline.cmdlen == 0)
	{
	    isp->ਖੋਜ_ਸ਼ੁਰੂ_2 = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#ਜੇ_ਅੰਤ
	redrawcmd();
    }
    ਨਹੀਂਤਾਂ ਜੇ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ਨਿਰਾ && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	ਜੇ (exmode_ਸਰਗਰਮ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#ਜੇ_ਅੰਤ
	   )
	    ਵਾਪਸ CMDLINE_NOT_ਬਦਲਿਆ;

	dealloc_cmdbuff();	// no commandline to return

	ਜੇ (!cmd_silent)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (cmdmsg_rl)
		ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਸੁਨੇਹਾ_putchar(' ');		// delete ':'
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	isp->ਖੋਜ_ਸ਼ੁਰੂ_2 = isp->save_cursor;
#ਜੇ_ਅੰਤ
	redraw_cmdline = TRUE;
	ਵਾਪਸ GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
    }
    ਵਾਪਸ CMDLINE_ਬਦਲਿਆ;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ਸਥਿਰ ਖਾਲੀ
cmdline_toggle_langmap(ਲੰਮਾ *b_im_ptr)
{
    ਜੇ (map_ਮੰਜ਼ਿਲ_exists_ਬਹੁਲਕ((char_u *)"", ਬਹੁਲਕ_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= ਬਹੁਲਕ_LANGMAP;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
	im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);	// Disable input method
#ਜੇ_ਅੰਤ
	ਜੇ (b_im_ptr != ਨਿਰਾ)
	{
	    ਜੇ (State & ਬਹੁਲਕ_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    ਨਹੀਂਤਾਂ
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    ਨਹੀਂਤਾਂ
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	ਜੇ ((p_imdisable && b_im_ptr != ਨਿਰਾ)
		? *b_im_ptr == B_IMODE_IM : im_get_ਸਥਿਤੀ())
	{
	    im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);	// Disable input method
	    ਜੇ (b_im_ptr != ਨਿਰਾ)
		*b_im_ptr = B_IMODE_NONE;
	}
	ਨਹੀਂਤਾਂ
	{
	    im_ਸੈੱਟ_ਸਰਗਰਮ(TRUE);	// Enable input method
	    ਜੇ (b_im_ptr != ਨਿਰਾ)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#ਜੇ_ਅੰਤ
    ਜੇ (b_im_ptr != ਨਿਰਾ)
    {
	ਜੇ (b_im_ptr == &curbuf->b_p_iminsert)
	    ਸੈੱਟ_iminsert_global();
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_imsearch_global();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    ਸਥਿਤੀ_redraw_curbuf();
#ਜੇ_ਅੰਤ
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_ਪਾਓ_reg(ਪੂਰਨ *gotesc ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;
    ਪੂਰਨ		literally = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		save_new_cmdpos = new_cmdpos;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#ਜੇ_ਅੰਤ
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    i = c = plain_vgetc();	// CTRL-R <char>
    ਜੇ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    ਜੇ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    ਜੇ (c == '=')
    {
	ਜੇ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_ਤਾਰਾ > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	ਨਹੀਂਤਾਂ
	    c = get_ਵਿਅੰਜਕ_ਰਜਿਸਟਰ();
    }
#ਜੇ_ਅੰਤ
    ਜੇ (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
			|| (clip_ਤਾਰਾ.ਉਪਲਬਧ && c == '*')
			|| (clip_plus.ਉਪਲਬਧ && c == '+')
#ਜੇ_ਅੰਤ
			;
	cmdline_paste(c, literally, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	ਜੇ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ਵਾਪਸ GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
	}
#ਜੇ_ਅੰਤ
	ਕੁੰਜੀ_Typed = FALSE;	// Don't do p_wc completion.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    ਜੇ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    ਨਹੀਂਤਾਂ
		ccline.cmdpos = new_cmdpos;
	}
#ਜੇ_ਅੰਤ
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#ਜੇ_ਅੰਤ

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    ਵਾਪਸ literally ? CMDLINE_ਬਦਲਿਆ : CMDLINE_NOT_ਬਦਲਿਆ;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ਸਥਿਰ ਖਾਲੀ
cmdline_ਖੱਬਾ_ਸੱਜਾ_mouse(ਪੂਰਨ c, ਪੂਰਨ *ignore_drag_release)
{
    ਜੇ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    ਨਹੀਂਤਾਂ
	*ignore_drag_release = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
	ਜੇ (!mouse_has(MOUSE_COMMAND))
	    ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਜੇ (mouse_ਪੰਕਤੀ < cmdline_ਪੰਕਤੀ && (clip_ਤਾਰਾ.ਉਪਲਬਧ || clip_plus.ਉਪਲਬਧ))
    {
	ਪੂਰਨ	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	ਜੇ (mouse_model_popup() && button == MOUSE_ਖੱਬਾ
		&& (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_ਸੱਜਾ;
	    mod_ਨਕਾਬ &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
	}
	clip_modeless(button, is_click, is_drag);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_cmdspos();
    ਲਈ (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	ਪੂਰਨ	i;

	i = cmdline_charsize(ccline.cmdpos);
	ਜੇ (mouse_ਪੰਕਤੀ <= cmdline_ਪੰਕਤੀ + ccline.cmdspos / cmdline_ਚੌੜਾਈ
		&& mouse_ਖਾਨਾ < ccline.cmdspos % cmdline_ਚੌੜਾਈ + i)
	    ਰੋਕੋ;
	ਜੇ (has_mbyte)
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
    ਸਥਿਰ ਪੂਰਨ
cmdline_browse_history(
	ਪੂਰਨ	c,
	ਪੂਰਨ	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	ਪੂਰਨ	histype,
	ਪੂਰਨ	*hiscnt_p,
	expand_T *xp)
{
    ਪੂਰਨ		orig_hiscnt;
    ਪੂਰਨ		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    ਪੂਰਨ		res;

    ਜੇ (get_hislen() == 0 || firstc == NUL)	// no history
	ਵਾਪਸ CMDLINE_NOT_ਬਦਲਿਆ;

    // save current command string so it can be restored later
    ਜੇ (lookfor == ਨਿਰਾ)
    {
	ਜੇ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ਨਿਰਾ)
	    ਵਾਪਸ CMDLINE_NOT_ਬਦਲਿਆ;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    ਲਈ (;;)
    {
	// one step backwards
	ਜੇ (c == K_ਉੱਪਰ || c == K_S_ਉੱਪਰ || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    ਜੇ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    ਨਹੀਂਤਾਂ ਜੇ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    ਨਹੀਂਤਾਂ			// at top of list
	    {
		hiscnt = orig_hiscnt;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ    // one step forwards
	{
	    // on last entry, clear the line
	    ਜੇ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		ਰੋਕੋ;
	    }

	    // not on a history line, nothing to do
	    ਜੇ (hiscnt == get_hislen())
		ਰੋਕੋ;
	    ਜੇ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    ਨਹੀਂਤਾਂ
		++hiscnt;
	}
	ਜੇ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ਨਿਰਾ)
	{
	    hiscnt = orig_hiscnt;
	    ਰੋਕੋ;
	}
	ਜੇ ((c != K_ਉੱਪਰ && c != K_ਥੱਲੇ)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    ਰੋਕੋ;
    }

    ਜੇ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	ਪੂਰਨ	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	ਜੇ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	ਨਹੀਂਤਾਂ
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	ਜੇ (histype == HIST_ਖੋਜ
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    ਪੂਰਨ     i;
	    ਪੂਰਨ     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    ਲਈ (i = 0; i <= 1; ++i)
	    {
		len = 0;
		ਲਈ (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    ਜੇ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			ਜੇ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Escape new sep, unless it is already
			// escaped.
			ਜੇ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    ਜੇ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			ਜੇ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		ਜੇ (i == 0)
		{
		    alloc_cmdbuff(len);
		    ਜੇ (ccline.cmdbuff == ਨਿਰਾ)
		    {
			res = GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
			ਜਾਓ ਪੂਰਾ;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (ਪੂਰਨ)len;
	}
	ਨਹੀਂਤਾਂ
	{
	    alloc_cmdbuff(plen);
	    ਜੇ (ccline.cmdbuff == ਨਿਰਾ)
	    {
		res = GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
		ਜਾਓ ਪੂਰਾ;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (ਪੂਰਨ)plen;
	}

	redrawcmd();
	res = CMDLINE_ਬਦਲਿਆ;
	ਜਾਓ ਪੂਰਾ;
    }
    beep_flush();
    res = CMDLINE_NOT_ਬਦਲਿਆ;

ਪੂਰਾ:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ਵਾਪਸ res;
}

/*
 * Initialize the current command-line info.
 */
    ਸਥਿਰ ਪੂਰਨ
init_ccline(ਪੂਰਨ firstc, ਪੂਰਨ indent)
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
    ਜੇ (ccline.cmdbuff == ਨਿਰਾ)
    {
	ccline.cmdbufflen = 0;
	ਵਾਪਸ FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_ਪਾਠ_ਸ਼ੁਰੂ_cmdline();

    // autoindent for :insert and :append
    ਜੇ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ਵਾਪਸ OK;
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
    ਪੂਰਨ		  firstc,
    ਲੰਮਾ	  ਗਿਣਤੀ_2,	// only used for incremental search
    ਪੂਰਨ		  indent,	// indent for inside conditionals
    getline_opt_T do_concat ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ getcmdline_int(firstc, ਗਿਣਤੀ_2, indent, TRUE);
}

    ਸਥਿਰ char_u *
getcmdline_int(
    ਪੂਰਨ		firstc,
    ਲੰਮਾ	ਗਿਣਤੀ_2 ਅਣਵਰਤਿਆ,	// only used for incremental search
    ਪੂਰਨ		indent,		// indent for inside conditionals
    ਪੂਰਨ		clear_ccline)	// clear ccline first
{
    ਸਥਿਰ ਪੂਰਨ	ਡੂੰਘਾਈ = 0;	    // call depth
    ਪੂਰਨ		c = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    ਪੂਰਨ		gotesc = FALSE;		// TRUE when <ESC> just typed
    ਪੂਰਨ		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ਨਿਰਾ;	// string to match
    size_t	lookforlen = 0;
    ਪੂਰਨ		hiscnt;			// current history line in use
    ਪੂਰਨ		histype;		// history type to be used
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    incsearch_state_T	is_state;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		did_wild_list = FALSE;	// did wild_list() recently
    ਪੂਰਨ		wim_ਸੂਚਕਾਂਕ = 0;		// index in wim_flags[]
    ਪੂਰਨ		res;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		save_State = State;	// remember State when called
    ਪੂਰਨ		some_ਕੁੰਜੀ_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    ਪੂਰਨ		ignore_drag_release = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		break_ctrl_c = FALSE;
#ਜੇ_ਅੰਤ
    expand_T	xpc;
    ਲੰਮਾ	*b_im_ptr = ਨਿਰਾ;
    buf_T	*b_im_ptr_buf = ਨਿਰਾ;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    ਪੂਰਨ		did_save_ccline = FALSE;
    ਪੂਰਨ		cmdline_type;
    ਪੂਰਨ		wild_type = 0;
    ਪੂਰਨ		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ਨਿਰਾ;
    ਪੂਰਨ		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++ਡੂੰਘਾਈ;

    ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    ਜੇ (clear_ccline)
	CLEAR_ਖੇਤਰ(ccline);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    ਜੇ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    init_incsearch_state(&is_state);
#ਜੇ_ਅੰਤ

    ਜੇ (init_ccline(firstc, indent) != OK)
	ਜਾਓ theend;	// out of memory

    ਜੇ (ਡੂੰਘਾਈ == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	ਜਾਓ theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    ਨਹੀਂਤਾਂ
	cmdmsg_rl = FALSE;
#ਜੇ_ਅੰਤ

    redir_off = TRUE;		// don't redirect the typed command
    ਜੇ (!cmd_silent)
    {
	i = ਸੁਨੇਹਾ_scrolled;
	ਸੁਨੇਹਾ_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	ਸੁਨੇਹਾ_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	ਸੈੱਟ_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    xpc.xp_shell = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (ccline.ਨਿਵੇਸ਼_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_ਨਮੂਨਾ = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#ਜੇ_ਅੰਤ

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    ਸੁਨੇਹਾ_scroll = FALSE;

    State = ਬਹੁਲਕ_CMDLINE;

    ਜੇ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	ਜੇ (curbuf->b_p_imsearch == B_IMODE_USE_ਪਾਓ)
	    b_im_ptr = &curbuf->b_p_iminsert;
	ਨਹੀਂਤਾਂ
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	ਜੇ (*b_im_ptr == B_IMODE_LMAP)
	    State |= ਬਹੁਲਕ_LANGMAP;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
	im_ਸੈੱਟ_ਸਰਗਰਮ(*b_im_ptr == B_IMODE_IM);
#ਜੇ_ਅੰਤ
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    ਨਹੀਂਤਾਂ ਜੇ (p_imcmdline)
	im_ਸੈੱਟ_ਸਰਗਰਮ(TRUE);
#ਜੇ_ਅੰਤ

    setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (!debug_ਬਹੁਲਕ)
	may_trigger_modechanged();
#ਜੇ_ਅੰਤ

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#ਜੇ_ਅੰਤ

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    ਜੇ (did_emsg)
	redrawcmd();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    ਜੇ (!cmd_silent && ਸੁਨੇਹਾ_scrolled == 0)
    {
	ਪੂਰਨ	ਮਿਲਿਆ_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ਜੇ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_ਸਥਿਤੀ = true;
		ਮਿਲਿਆ_one = TRUE;
	    }

	ਜੇ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    ਮਿਲਿਆ_one = TRUE;
	}

	ਜੇ (ਮਿਲਿਆ_one)
	    redraw_statuslines();
    }
#ਜੇ_ਅੰਤ

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ	trigger_cmdlinechanged = TRUE;
	ਪੂਰਨ	ਸਿਰਾ_wildmenu;
	ਪੂਰਨ	prev_cmdpos = ccline.cmdpos;
	ਪੂਰਨ	skip_pum_redraw = FALSE;

	VIM_CLEAR(prev_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#ਜੇ_ਅੰਤ
	quit_ਹੋਰ = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	ਜੇ (ex_ਸਾਧਾਰਨ_busy == 0 && stuff_ਖਾਲੀ_ਥਾਵਾਂ() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_ਕੁੰਜੀ_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    ਜੇ (prev_cmdbuff == ਨਿਰਾ)
		ਜਾਓ returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	ਜੇ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	ਕਰੋ
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = ਸੁਰੱਖਿਅਤ_vgetc();
	} ਜਦੋਂ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	ਜੇ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    ਜੇ (cmdline_pum_ਸਰਗਰਮ())
		cmdline_pum_remove(&ccline, FALSE);
	    (ਖਾਲੀ)ExpandOne(&xpc, ਨਿਰਾ, ਨਿਰਾ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_ਸੂਚਕਾਂਕ = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	ਜੇ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    ਜਾਰੀ;
	}

	ਜੇ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    ਪੂਰਨ	    clen = ccline.cmdlen;
	    ਪੂਰਨ	    cc_ਗਿਣਤੀ = aucmd_cmdline_ਬਦਲਿਆ_ਗਿਣਤੀ;

	    ਜੇ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		ਜੇ (clen == ccline.cmdlen
				    || cc_ਗਿਣਤੀ != aucmd_cmdline_ਬਦਲਿਆ_ਗਿਣਤੀ)
		    trigger_cmdlinechanged = FALSE;
		ਜਾਓ cmdline_ਬਦਲਿਆ;
	    }
	}

	ਜੇ (ਕੁੰਜੀ_Typed)
	{
	    some_ਕੁੰਜੀ_typed = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (cmd_hkmap)
		c = hkmap(c);
	    ਜੇ (cmdmsg_rl && !ਕੁੰਜੀ_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		ਚੋਣ (c)
		{
		    ਹਾਲਤ K_ਸੱਜਾ:   c = K_ਖੱਬਾ; ਰੋਕੋ;
		    ਹਾਲਤ K_S_ਸੱਜਾ: c = K_S_ਖੱਬਾ; ਰੋਕੋ;
		    ਹਾਲਤ K_C_ਸੱਜਾ: c = K_C_ਖੱਬਾ; ਰੋਕੋ;
		    ਹਾਲਤ K_ਖੱਬਾ:    c = K_ਸੱਜਾ; ਰੋਕੋ;
		    ਹਾਲਤ K_S_ਖੱਬਾ:  c = K_S_ਸੱਜਾ; ਰੋਕੋ;
		    ਹਾਲਤ K_C_ਖੱਬਾ:  c = K_C_ਸੱਜਾ; ਰੋਕੋ;
		}
	    }
#ਜੇ_ਅੰਤ
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	ਜੇ ((c == Ctrl_C
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		|| c == intr_char
#ਜੇ_ਅੰਤ
				)
#ਜੇ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_ਸਰਗਰਮ)
#ਜੇ_ਅੰਤ
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	ਜੇ (lookfor != ਨਿਰਾ
		&& c != K_S_ਥੱਲੇ && c != K_S_ਉੱਪਰ
		&& c != K_ਥੱਲੇ && c != K_ਉੱਪਰ
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ਖੱਬਾ && c != K_ਸੱਜਾ
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	ਜੇ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	ਜੇ (p_wmnu)
	    c = wildmenu_translate_ਕੁੰਜੀ(&ccline, c, &xpc, did_wild_list);

	ਪੂਰਨ ਕੁੰਜੀ_is_wc = (c == p_wc && ਕੁੰਜੀ_Typed) || c == p_wcm;
	ਜੇ ((cmdline_pum_ਸਰਗਰਮ() || wild_menu_showing || did_wild_list)
		&& !ਕੁੰਜੀ_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    ਜੇ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		ਜੇ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    ਰੋਕੋ;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	ਜੇ (ਕੁੰਜੀ_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    || c == intr_char
#ਜੇ_ਅੰਤ
		    || c == Ctrl_C))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਸੈੱਟ_vim_var_char(c);  // Set v:char
#ਜੇ_ਅੰਤ
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
	    ਜੇ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		ਸੈੱਟ_no_hlsearch(TRUE);
#ਜੇ_ਅੰਤ
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	ਸਿਰਾ_wildmenu = (!ਕੁੰਜੀ_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	ਸਿਰਾ_wildmenu = ਸਿਰਾ_wildmenu && (!cmdline_pum_ਸਰਗਰਮ() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_ਉੱਪਰ || c == K_S_ਥੱਲੇ)
				 && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL))));

	// free expanded names when finished walking through matches
	ਜੇ (ਸਿਰਾ_wildmenu)
	{
	    ਜੇ (cmdline_pum_ਸਰਗਰਮ())
	    {
		skip_pum_redraw = skip_pum_redraw && !ਕੁੰਜੀ_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    ਜੇ (xpc.xp_numfiles != -1)
		(ਖਾਲੀ)ExpandOne(&xpc, ਨਿਰਾ, ਨਿਰਾ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    ਜੇ (!p_wmnu || (c != K_ਉੱਪਰ && c != K_ਥੱਲੇ))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_ਸੂਚਕਾਂਕ = 0;
	    wildmenu_cleanup(&ccline);
	}

	ਜੇ (p_wmnu)
	    c = wildmenu_process_ਕੁੰਜੀ(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	ਜੇ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    ਜੇ (res == CMDLINE_ਬਦਲਿਆ)
		ਜਾਓ cmdline_ਬਦਲਿਆ;
	    ਨਹੀਂਤਾਂ ਜੇ (res == CMDLINE_NOT_ਬਦਲਿਆ)
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
	    ਨਹੀਂਤਾਂ ਜੇ (res == GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
		ਜਾਓ returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	ਜੇ (c == cedit_ਕੁੰਜੀ || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    ਜੇ ((c == K_CMDWIN || ex_ਸਾਧਾਰਨ_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_ਕੁੰਜੀ_typed = TRUE;
	    }
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	ਨਹੀਂਤਾਂ
	    c = do_digraph(c);
#ਜੇ_ਅੰਤ

	ਜੇ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!ਕੁੰਜੀ_Typed || vim_strchr(p_cpo, CPO_ESC) != ਨਿਰਾ)))
	{
	    // In Ex mode a backslash escapes a newline.
	    ਜੇ (exmode_ਸਰਗਰਮ
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		ਜੇ (c == K_KENTER)
		    c = '\n';
	    }
	    ਨਹੀਂਤਾਂ
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		ਜੇ (ccheck_abbr(c + ABBR_OFF))
		    ਜਾਓ cmdline_ਬਦਲਿਆ;
		ਜੇ (!cmd_silent)
		{
		    windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off);
		    ਬਾਹਰਲਾ_flush();
		}
		ਰੋਕੋ;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	ਜੇ ((c == p_wc && !gotesc && ਕੁੰਜੀ_Typed) || c == p_wcm || c == K_WILD)
	{
	    ਜੇ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_ਸੂਚਕਾਂਕ, &xpc, &gotesc, c == K_WILD,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		    &is_state.ਖੋਜ_ਸ਼ੁਰੂ_2
#ਨਹੀਂਤਾਂ
		    ਨਿਰਾ
#ਜੇ_ਅੰਤ
		    );
	    ਜੇ (c == K_WILD)
		--emsg_silent;
	    ਜੇ (res == CMDLINE_ਬਦਲਿਆ)
		ਜਾਓ cmdline_ਬਦਲਿਆ;
	    ਜੇ (c == K_WILD)
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	ਜੇ (c == K_S_TAB && ਕੁੰਜੀ_Typed)
	{
	    ਜੇ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		ਜੇ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    ਪੂਰਨ list = wim_flags[wim_ਸੂਚਕਾਂਕ] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		ਜੇ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    ਜਾਓ cmdline_ਬਦਲਿਆ;
	    }
	}

	ਜੇ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	ਜੇ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	    // Apply search highlighting
	    ਜੇ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    ਜੇ (ਕੁੰਜੀ_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, ਗਿਣਤੀ_2, &is_state);
#ਜੇ_ਅੰਤ
	    wild_type = 0;
	    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	ਚੋਣ (c)
	{
	ਹਾਲਤ K_BS:
	ਹਾਲਤ Ctrl_H:
	ਹਾਲਤ K_DEL:
	ਹਾਲਤ K_KDEL:
	ਹਾਲਤ Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		    , &is_state
#ਜੇ_ਅੰਤ
		    );
	    ਜੇ (res == CMDLINE_NOT_ਬਦਲਿਆ)
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
	    ਨਹੀਂਤਾਂ ਜੇ (res == GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
		ਜਾਓ returncmd;		// back to cmd mode
	    ਜਾਓ cmdline_ਬਦਲਿਆ;

	ਹਾਲਤ K_INS:
	ਹਾਲਤ K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#ਜੇ_ਅੰਤ
		may_trigger_modechanged();
		ਸਥਿਤੀ_redraw_curbuf();
		redraw_statuslines();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ਨਿਰਾ);
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

//	case '@':   only in very old vi
	ਹਾਲਤ Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		ਜਦੋਂ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		ਜੇ (ccline.cmdlen == 0)
		    is_state.ਖੋਜ_ਸ਼ੁਰੂ_2 = is_state.save_cursor;
#ਜੇ_ਅੰਤ
		redrawcmd();
		ਜਾਓ cmdline_ਬਦਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਹਾਲਤ Ctrl_Y:
		// Copy the modeless selection, if there is one.
		ਜੇ (clip_ਤਾਰਾ.state != SELECT_CLEARED)
		{
		    ਜੇ (clip_ਤਾਰਾ.state == SELECT_ਪੂਰਾ)
			clip_copy_modeless_selection(TRUE);
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		}
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ ESC:	// get here if p_wc != ESC or when ESC typed twice
	ਹਾਲਤ Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		ਜੇ (exmode_ਸਰਗਰਮ
			       && (ex_ਸਾਧਾਰਨ_busy == 0 || typebuf.tb_len > 0))
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		ਜਾਓ returncmd;		// back to cmd mode

	ਹਾਲਤ Ctrl_R:			// insert register
		res = cmdline_ਪਾਓ_reg(&gotesc);
		ਜੇ (res == GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
		    ਜਾਓ returncmd;
		ਜੇ (res == CMDLINE_ਬਦਲਿਆ)
		    ਜਾਓ cmdline_ਬਦਲਿਆ;
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_D:
		ਜੇ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    ਰੋਕੋ;	// Use ^D as normal char instead

		redrawcmd();
		ਜਾਰੀ;	// don't do incremental search now

	ਹਾਲਤ K_ਸੱਜਾ:
	ਹਾਲਤ K_S_ਸੱਜਾ:
	ਹਾਲਤ K_C_ਸੱਜਾ:
		ਕਰੋ
		{
		    ਜੇ (ccline.cmdpos >= ccline.cmdlen)
			ਰੋਕੋ;
		    i = cmdline_charsize(ccline.cmdpos);
		    ਜੇ (ਕੁੰਜੀ_Typed && ccline.cmdspos + i >= cmdline_ਚੌੜਾਈ * ਪੰਕਤੀਆਂ)
			ਰੋਕੋ;
		    ccline.cmdspos += i;
		    ਜੇ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    ਨਹੀਂਤਾਂ
			++ccline.cmdpos;
		}
		ਜਦੋਂ ((c == K_S_ਸੱਜਾ || c == K_C_ਸੱਜਾ
			       || (mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		ਜੇ (has_mbyte)
		    ਸੈੱਟ_cmdspos_cursor();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ K_ਖੱਬਾ:
	ਹਾਲਤ K_S_ਖੱਬਾ:
	ਹਾਲਤ K_C_ਖੱਬਾ:
		ਜੇ (ccline.cmdpos == 0)
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		ਕਰੋ
		{
		    --ccline.cmdpos;
		    ਜੇ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_ਸਿਰ_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		ਜਦੋਂ (ccline.cmdpos > 0
			&& (c == K_S_ਖੱਬਾ || c == K_C_ਖੱਬਾ
			       || (mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		ਜੇ (has_mbyte)
		    ਸੈੱਟ_cmdspos_cursor();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ਹਾਲਤ K_F4:
	    ਜੇ (mod_ਨਕਾਬ == MOD_ਨਕਾਬ_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
	    }
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_MIDDLEDRAG:
	ਹਾਲਤ K_MIDDLERELEASE:
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;	// Ignore mouse

	ਹਾਲਤ K_MIDDLEMOUSE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
		    ਜੇ (!mouse_has(MOUSE_COMMAND))
			ਜਾਓ cmdline_not_ਬਦਲਿਆ;   // Ignore mouse
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
		ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ)
		    cmdline_paste('*', TRUE, TRUE);
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		ਜਾਓ cmdline_ਬਦਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	ਹਾਲਤ K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		ਜਾਓ cmdline_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_LEFTDRAG:
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_RIGHTDRAG:
	ਹਾਲਤ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ਜੇ (ignore_drag_release)
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		// FALLTHROUGH
	ਹਾਲਤ K_LEFTMOUSE:
	ਹਾਲਤ K_RIGHTMOUSE:
		cmdline_ਖੱਬਾ_ਸੱਜਾ_mouse(c, &ignore_drag_release);
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	ਹਾਲਤ K_MOUSEDOWN:
	ਹਾਲਤ K_MOUSEUP:
	ਹਾਲਤ K_MOUSELEFT:
	ਹਾਲਤ K_MOUSERIGHT:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		ਜੇ (cmdline_pum_ਸਰਗਰਮ())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_ਥੱਲੇ
				      : c == K_MOUSEUP ? MSCR_ਉੱਪਰ
				      : c == K_MOUSELEFT ? MSCR_ਖੱਬਾ
				      : MSCR_ਸੱਜਾ);
#ਜੇ_ਅੰਤ
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	// Alternate buttons ignored here
	ਹਾਲਤ K_X1MOUSE:
	ਹਾਲਤ K_X1DRAG:
	ਹਾਲਤ K_X1RELEASE:
	ਹਾਲਤ K_X2MOUSE:
	ਹਾਲਤ K_X2DRAG:
	ਹਾਲਤ K_X2RELEASE:
	ਹਾਲਤ K_MOUSEMOVE:
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਹਾਲਤ K_LEFTMOUSE_NM:	// mousefocus click, ignored
	ਹਾਲਤ K_LEFTRELEASE_NM:
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ K_VER_SCROLLBAR:
		ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ K_HOR_SCROLLBAR:
		ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_ਮੁੱਲ);
		    redrawcmd();
		}
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	ਹਾਲਤ K_TABLINE:
	ਹਾਲਤ K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		ਜੇ (gui_use_tabline())
		    gui_mch_ਸੈੱਟ_curtab(tabpage_ਸੂਚਕਾਂਕ(curtab));
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_SELECT:	    // end of Select mode mapping - ignore
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_B:	    // begin of command line
	ਹਾਲਤ K_HOME:
	ਹਾਲਤ K_KHOME:
	ਹਾਲਤ K_S_HOME:
	ਹਾਲਤ K_C_HOME:
		ccline.cmdpos = 0;
		ਸੈੱਟ_cmdspos();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_E:	    // end of command line
	ਹਾਲਤ K_ਸਿਰਾ:
	ਹਾਲਤ K_KEND:
	ਹਾਲਤ K_S_ਸਿਰਾ:
	ਹਾਲਤ K_C_ਸਿਰਾ:
		ccline.cmdpos = ccline.cmdlen;
		ਸੈੱਟ_cmdspos_cursor();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_A:	    // all matches
		ਜੇ (cmdline_pum_ਸਰਗਰਮ())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		ਜੇ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    ਰੋਕੋ;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		ਜਾਓ cmdline_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_L:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		ਜੇ (may_ਜੋੜੋ_char_ਮੰਜ਼ਿਲ_ਖੋਜ(firstc, &c, &is_state) == OK)
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ

		// completion: longest common part
		ਜੇ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    ਰੋਕੋ;
		ਜਾਓ cmdline_ਬਦਲਿਆ;

	ਹਾਲਤ Ctrl_N:	    // next match
	ਹਾਲਤ Ctrl_P:	    // previous match
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		ਜੇ (cmdline_pum_ਸਰਗਰਮ() && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
		{
		    ਜੇ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		}
#ਜੇ_ਅੰਤ
		ਜੇ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_ਅਗਲਾ;
		    ਜੇ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ਰੋਕੋ;
		    ਜਾਓ cmdline_ਬਦਲਿਆ;
		}
		// FALLTHROUGH
	ਹਾਲਤ K_ਉੱਪਰ:
	ਹਾਲਤ K_ਥੱਲੇ:
	ਹਾਲਤ K_S_ਉੱਪਰ:
	ਹਾਲਤ K_S_ਥੱਲੇ:
	ਹਾਲਤ K_PAGEUP:
	ਹਾਲਤ K_KPAGEUP:
	ਹਾਲਤ K_PAGEDOWN:
	ਹਾਲਤ K_KPAGEDOWN:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		ਜੇ (cmdline_pum_ਸਰਗਰਮ()
			&& ((c == K_S_ਉੱਪਰ || c == K_S_ਥੱਲੇ)
			    ? (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
				&& (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))))
		{
		    ਪੂਰਨ	ਉੱਪਰ = c == K_S_ਉੱਪਰ || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    ਜੇ (popup_scroll_info(ਉੱਪਰ ? -1 : 1,
					     c != K_S_ਉੱਪਰ && c != K_S_ਥੱਲੇ))
			cmdline_pum_display();
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		}
#ਜੇ_ਅੰਤ
		ਜੇ (cmdline_pum_ਸਰਗਰਮ()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    ਜੇ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    ਜੇ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ਰੋਕੋ;
		    ਜਾਓ cmdline_ਬਦਲਿਆ;
		}
		ਨਹੀਂਤਾਂ
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    ਜੇ (res == CMDLINE_ਬਦਲਿਆ)
		    {
			did_hist_navigate = TRUE;
			ਜਾਓ cmdline_ਬਦਲਿਆ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (res == GOTO_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
			ਜਾਓ returncmd;
		}
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਹਾਲਤ Ctrl_G:	    // next match
	ਹਾਲਤ Ctrl_T:	    // previous match
		ਜੇ (may_adjust_incsearch_highlighting(
					  firstc, ਗਿਣਤੀ_2, &is_state, c) == FAIL)
		    ਜਾਓ cmdline_not_ਬਦਲਿਆ;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ Ctrl_V:
	ਹਾਲਤ Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    ਜੇ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			ਸੁਨੇਹਾ_putchar(' ');
			cursorcmd();
		    }
		}

		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	ਹਾਲਤ Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		ਜੇ (c != NUL)
		    ਰੋਕੋ;

		redrawcmd();
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ // FEAT_DIGRAPHS

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਹਾਲਤ Ctrl__:	    // CTRL-_: switch language mode
		ਜੇ (!p_ari)
		    ਰੋਕੋ;
		cmd_hkmap = !cmd_hkmap;
		ਜਾਓ cmdline_not_ਬਦਲਿਆ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ਨਿਰਾ);
		ਜਾਓ cmdline_ਬਦਲਿਆ;

	ਮੂਲ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		ਜੇ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    ਜਾਓ returncmd;	// back to Normal mode
		}
#ਜੇ_ਅੰਤ
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		ਜੇ (!IS_SPECIAL(c))
		    mod_ਨਕਾਬ = 0x0;
		ਰੋਕੋ;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	ਜੇ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    ਜਾਓ cmdline_ਬਦਲਿਆ;

	/*
	 * put the character in the command line
	 */
	ਜੇ (IS_SPECIAL(c) || mod_ਨਕਾਬ != 0)
	    put_on_cmdline(get_special_ਕੁੰਜੀ_name(c, mod_ਨਕਾਬ), -1, TRUE);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	ਜਾਓ cmdline_ਬਦਲਿਆ;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_ਬਦਲਿਆ:
	// Trigger CursorMovedC autocommands.
	ਜੇ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਜੇ (!is_state.incsearch_postponed)
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ

cmdline_ਬਦਲਿਆ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	// If the window changed incremental search state is not valid.
	ਜੇ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	ਜੇ (xpc.xp_context == EXPAND_NOTHING && (ਕੁੰਜੀ_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, ਗਿਣਤੀ_2, &is_state);
#ਜੇ_ਅੰਤ
	// Trigger CmdlineChanged autocommands.
	ਜੇ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ਨਿਰਾ &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    ਜੇ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#ਜੇ_ਅੰਤ
	}

	// Trigger CursorMovedC autocommands.
	ਜੇ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    ਜੇ (vpeekc() == NUL)
		redrawcmd();
#ਜੇ_ਅੰਤ
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    ਜੇ (!event_cmdlineleavepre_triggered)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਸੈੱਟ_vim_var_char(c);  // Set v:char
#ਜੇ_ਅੰਤ
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#ਜੇ_ਅੰਤ

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    ਜੇ (cmdline_pum_ਸਰਗਰਮ())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_ਸੂਚਕਾਂਕ = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ਨਿਰਾ;
    clear_cmdline_orig();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਅੰਤ_incsearch_highlighting(gotesc, &is_state, FALSE);
#ਜੇ_ਅੰਤ

    ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ਜੇ (ccline.cmdlen && firstc != NUL
		&& (some_ਕੁੰਜੀ_typed || histype == HIST_ਖੋਜ))
	{
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_ਖੋਜ ? firstc : NUL);
	    ਜੇ (firstc == ':')
	    {
		vim_free(new_ਆਖਰੀ_cmdline);
		new_ਆਖਰੀ_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	ਜੇ (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    ਸੁਨੇਹਾ_check();
    ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    ਜੇ (some_ਕੁੰਜੀ_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_char(c);  // Set v:char
#ਜੇ_ਅੰਤ
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (!debug_ਬਹੁਲਕ)
	may_trigger_modechanged();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    ਜੇ (b_im_ptr != ਨਿਰਾ && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_ਸਥਿਤੀ(b_im_ptr);
    im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);
#ਜੇ_ਅੰਤ
    setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
    sb_ਪਾਠ_ਸਿਰਾ_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--ਡੂੰਘਾਈ;
	ਜੇ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	ਨਹੀਂਤਾਂ
	    ccline.cmdbuff = ਨਿਰਾ;

	vim_free(prev_cmdbuff);
	ਵਾਪਸ p;
    }
}

#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    ਪੂਰਨ		firstc,
    char_u	*prompt,	// command line prompt
    ਪੂਰਨ		attr,		// attributes for prompt
    ਪੂਰਨ		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    ਪੂਰਨ			did_save_ccline = FALSE;
    ਪੂਰਨ			ਸੁਨੇਹਾ_ਖਾਨਾ_save = ਸੁਨੇਹਾ_ਖਾਨਾ;
    ਪੂਰਨ			ਸੁਨੇਹਾ_silent_save = ਸੁਨੇਹਾ_silent;

    ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_ਖੇਤਰ(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.ਨਿਵੇਸ਼_fn = (firstc == '@');
# endif
    ਸੁਨੇਹਾ_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    ਜੇ (did_save_ccline)
	restore_cmdline(&save_ccline);

    ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
	ਸੁਨੇਹਾ_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ_save;

    ਵਾਪਸ s;
}
#ਜੇ_ਅੰਤ

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    ਪੂਰਨ
check_opt_wim(ਖਾਲੀ)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		idx = 0;

    ਲਈ (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    ਲਈ (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	ਲਈ (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	ਜੇ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ਵਾਪਸ FAIL;
	ਜੇ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	ਨਹੀਂਤਾਂ ਜੇ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_ਭਰਿਆ;
	ਨਹੀਂਤਾਂ ਜੇ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	ਨਹੀਂਤਾਂ ਜੇ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	ਨਹੀਂਤਾਂ ਜੇ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	ਨਹੀਂਤਾਂ ਜੇ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;
	p += i;
	ਜੇ (*p == NUL)
	    ਰੋਕੋ;
	ਜੇ (*p == ',')
	{
	    ਜੇ (idx == 3)
		ਵਾਪਸ FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    ਜਦੋਂ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    ਲਈ (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ਵਾਪਸ OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    ਪੂਰਨ
ਪਾਠ_locked(ਖਾਲੀ)
{
    ਜੇ (cmdwin_type != 0)
	ਵਾਪਸ TRUE;
    ਵਾਪਸ textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    ਖਾਲੀ
ਪਾਠ_locked_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    emsg(_(get_ਪਾਠ_locked_ਸੁਨੇਹਾ()));
}

    ਅੱਖਰ *
get_ਪਾਠ_locked_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਜੇ (cmdwin_type != 0)
	ਵਾਪਸ e_invalid_ਵਿੱਚ_cmdline_ਖਿੜਕੀ;
    ਵਾਪਸ e_not_allowed_ਮੰਜ਼ਿਲ_change_ਪਾਠ_or_change_ਖਿੜਕੀ;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    ਪੂਰਨ
ਪਾਠ_or_buf_locked(ਖਾਲੀ)
{
    ਜੇ (ਪਾਠ_locked())
    {
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    ਪੂਰਨ
curbuf_locked(ਖਾਲੀ)
{
    ਜੇ (curbuf_ਤਾਲਾ > 0)
    {
	emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_edit_another_ਬਫਰ_now));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    ਪੂਰਨ
allbuf_locked(ਖਾਲੀ)
{
    ਜੇ (allbuf_ਤਾਲਾ > 0)
    {
	emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_change_ਬਫਰ_information_now));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
cmdline_charsize(ਪੂਰਨ idx)
{
#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ਜੇ (cmdline_ਤਾਰਾ > 0)	    // showing '*', always 1 position
	ਵਾਪਸ 1;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_cmdspos(ਖਾਲੀ)
{
    ਜੇ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    ਨਹੀਂਤਾਂ
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_cmdspos_cursor(ਖਾਲੀ)
{
    ਪੂਰਨ		i, m, c;

    ਸੈੱਟ_cmdspos();
    ਜੇ (ਕੁੰਜੀ_Typed)
    {
	m = cmdline_ਚੌੜਾਈ * ਪੰਕਤੀਆਂ;
	ਜੇ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    ਨਹੀਂਤਾਂ
	m = MAXCOL;
    ਲਈ (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	ਜੇ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	ਜੇ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    ਰੋਕੋ;
	}
	ਜੇ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ਸਥਿਰ ਖਾਲੀ
correct_cmdspos(ਪੂਰਨ idx, ਪੂਰਨ cells)
{
    ਜੇ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_ਚੌੜਾਈ + cells > cmdline_ਚੌੜਾਈ)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    ਪੂਰਨ		c,		// normally ':', NUL for ":append"
    ਖਾਲੀ	*cookie ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    ਜੇ (exec_from_reg && vpeekc() == ':')
	(ਖਾਲੀ)vgetc();
    ਵਾਪਸ getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    ਪੂਰਨ		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    ਖਾਲੀ	*cookie ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		indent,		// indent for inside conditionals
    getline_opt_T options ਅਣਵਰਤਿਆ)
{
    garray_T	ਸਤਰ_ga;
    char_u	*pend;
    ਪੂਰਨ		startcol = 0;
    ਪੂਰਨ		c1 = 0;
    ਪੂਰਨ		escaped = FALSE;	// CTRL-V typed
    ਪੂਰਨ		vcol = 0;
    char_u	*p;
    ਪੂਰਨ		prev_char;
    ਪੂਰਨ		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    ਜੇ ((ਸੁਨੇਹਾ_ਖਾਨਾ || ਸੁਨੇਹਾ_didout) && promptc != '?')
	ਸੁਨੇਹਾ_putchar('\n');
    ਜੇ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	ਜੇ (p_prompt)
	    ਸੁਨੇਹਾ_putchar(':');
	ਜਦੋਂ (indent-- > 0)
	    ਸੁਨੇਹਾ_putchar(' ');
	startcol = ਸੁਨੇਹਾ_ਖਾਨਾ;
    }

    ga_init2(&ਸਤਰ_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    ਜੇ (promptc <= 0)
    {
	vcol = indent;
	ਜਦੋਂ (indent >= 8)
	{
	    ga_append(&ਸਤਰ_ga, TAB);
	    ਸੁਨੇਹਾ_puts("        ");
	    indent -= 8;
	}
	ਜਦੋਂ (indent-- > 0)
	{
	    ga_append(&ਸਤਰ_ga, ' ');
	    ਸੁਨੇਹਾ_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    ਜਦੋਂ (!got_int)
    {
	ਲੰਮਾ    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	ਜੇ (ga_grow(&ਸਤਰ_ga, 40) == FAIL)
	    ਰੋਕੋ;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	ਜੇ (ex_ਸਾਧਾਰਨ_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	ਨਹੀਂਤਾਂ
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	ਜੇ (got_int)
	{
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਰੋਕੋ;
	}

	ਜੇ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &ਸਤਰ_ga);
	    ਜਾਓ redraw;
	}

	ਜੇ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    ਜੇ (c1 == '\r')
		c1 = '\n';

	    ਜੇ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		ਜੇ (ਸਤਰ_ga.ga_len > 0)
		{
		    ਜੇ (has_mbyte)
		    {
			p = (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
			p[ਸਤਰ_ga.ga_len] = NUL;
			len = (*mb_ਸਿਰ_off)(p, p + ਸਤਰ_ga.ga_len - 1) + 1;
			ਸਤਰ_ga.ga_len -= len;
		    }
		    ਨਹੀਂਤਾਂ
			--ਸਤਰ_ga.ga_len;
		    ਜਾਓ redraw;
		}
		ਜਾਰੀ;
	    }

	    ਜੇ (c1 == Ctrl_U)
	    {
		ਸੁਨੇਹਾ_ਖਾਨਾ = startcol;
		ਸੁਨੇਹਾ_clr_eos();
		ਸਤਰ_ga.ga_len = 0;
		ਜਾਓ redraw;
	    }

	    ਜੇ (c1 == Ctrl_T)
	    {
		sw = get_sw_ਮੁੱਲ(curbuf);
		p = (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
		p[ਸਤਰ_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
ਜੋੜੋ_indent:
		ਜਦੋਂ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (ਖਾਲੀ)ga_grow(&ਸਤਰ_ga, 2);  // one more for the NUL
		    p = (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, ਸਤਰ_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++ਸਤਰ_ga.ga_len;
		}
redraw:
		// redraw the line
		ਸੁਨੇਹਾ_ਖਾਨਾ = startcol;
		vcol = 0;
		p = (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
		p[ਸਤਰ_ga.ga_len] = NUL;
		ਜਦੋਂ (p < (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ + ਸਤਰ_ga.ga_len)
		{
		    ਜੇ (*p == TAB)
		    {
			ਕਰੋ
			    ਸੁਨੇਹਾ_putchar(' ');
			ਜਦੋਂ (++vcol % 8);
			++p;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			len = mb_ptr2len(p);
			ਸੁਨੇਹਾ_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		ਸੁਨੇਹਾ_clr_eos();
		windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
		ਜਾਰੀ;
	    }

	    ਜੇ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
		ਜੇ (prev_char == '0' || prev_char == '^')
		{
		    ਜੇ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--ਸਤਰ_ga.ga_len] = NUL;
		}
		ਨਹੀਂਤਾਂ
		{
		    p[ਸਤਰ_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    ਜੇ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_ਮੁੱਲ(curbuf);
		    }
		}
		ਜਦੋਂ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, ਸਤਰ_ga.ga_len - (s - p) + 1);
		    --ਸਤਰ_ga.ga_len;
		}
		ਜਾਓ ਜੋੜੋ_indent;
	    }

	    ਜੇ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		ਜਾਰੀ;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    ਜੇ (IS_SPECIAL(c1))
		ਜਾਰੀ;
	}

	ਜੇ (IS_SPECIAL(c1))
	    c1 = '?';
	ਜੇ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ + ਸਤਰ_ga.ga_len);
	ਨਹੀਂਤਾਂ
	{
	    len = 1;
	    ((char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ)[ਸਤਰ_ga.ga_len] = c1;
	}
	ਜੇ (c1 == '\n')
	    ਸੁਨੇਹਾ_putchar('\n');
	ਨਹੀਂਤਾਂ ਜੇ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    ਕਰੋ
		ਸੁਨੇਹਾ_putchar(' ');
	    ਜਦੋਂ (++vcol % 8);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸੁਨੇਹਾ_outtrans_len(
		     ((char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ) + ਸਤਰ_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	ਸਤਰ_ga.ga_len += len;
	escaped = FALSE;

	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
	pend = (char_u *)(ਸਤਰ_ga.ga_ਅੰਕੜੇ) + ਸਤਰ_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	ਜੇ (ਸਤਰ_ga.ga_len > 0 && pend[-1] == '\n'
		&& (ਸਤਰ_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --ਸਤਰ_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    ਰੋਕੋ;
	}
    }

    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;

    // make following messages go to the next line
    ਸੁਨੇਹਾ_didout = FALSE;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ < ਪੰਕਤੀਆਂ - 1)
	++ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    emsg_on_display = FALSE;		// don't want ui_delay()

    ਜੇ (got_int)
	ga_clear(&ਸਤਰ_ga);

    ਵਾਪਸ (char_u *)ਸਤਰ_ga.ga_ਅੰਕੜੇ;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    ਪੂਰਨ
cmdline_overstrike(ਖਾਲੀ)
{
    ਵਾਪਸ ccline.overstrike;
}

#ਜੇ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    ਪੂਰਨ
cmdline_at_ਸਿਰਾ(ਖਾਲੀ)
{
    ਵਾਪਸ (ccline.cmdpos >= ccline.cmdlen);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(ਖਾਲੀ)
{
    ਜੇ (ccline.cmdbuff == ਨਿਰਾ || ccline.cmdpos > ccline.cmdlen)
	ਵਾਪਸ MAXCOL;

    ਜੇ (has_mbyte)
    {
	colnr_T	ਖਾਨਾ;
	ਪੂਰਨ	i = 0;

	ਲਈ (ਖਾਨਾ = 0; i < ccline.cmdpos; ++ਖਾਨਾ)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ਵਾਪਸ ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ccline.cmdpos;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ਸਥਿਰ ਖਾਲੀ
redrawcmd_preedit(ਖਾਲੀ)
{
    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE)
	    && xic != ਨਿਰਾ
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	ਪੂਰਨ	cmdpos = 0;
	ਪੂਰਨ	cmdspos;
	ਪੂਰਨ	old_ਪੰਕਤੀ;
	ਪੂਰਨ	old_ਖਾਨਾ;
	colnr_T	ਖਾਨਾ;

	old_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	old_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	ਜੇ (has_mbyte)
	{
	    ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < preedit_ਸ਼ੁਰੂ_ਖਾਨਾ
			  && cmdpos < ccline.cmdlen; ++ਖਾਨਾ)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    cmdspos += preedit_ਸ਼ੁਰੂ_ਖਾਨਾ;
	    cmdpos  += preedit_ਸ਼ੁਰੂ_ਖਾਨਾ;
	}

	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ + (cmdspos / cmdline_ਚੌੜਾਈ);
	ਸੁਨੇਹਾ_ਖਾਨਾ = cmdspos % cmdline_ਚੌੜਾਈ;
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;

	ਲਈ (ਖਾਨਾ = 0; cmdpos < ccline.cmdlen; ++ਖਾਨਾ)
	{
	    ਪੂਰਨ char_len;
	    ਪੂਰਨ char_attr;

	    char_attr = im_get_feedback_attr(ਖਾਨਾ);
	    ਜੇ (char_attr < 0)
		ਰੋਕੋ; // end of preedit string

	    ਜੇ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    ਨਹੀਂਤਾਂ
		char_len = 1;

	    ਸੁਨੇਹਾ_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	ਸੁਨੇਹਾ_ਪੰਕਤੀ = old_ਪੰਕਤੀ;
	ਸੁਨੇਹਾ_ਖਾਨਾ = old_ਖਾਨਾ;
    }
}
#ਜੇ_ਅੰਤ // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ਸਥਿਰ ਖਾਲੀ
dealloc_cmdbuff(ਖਾਲੀ)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ਸਥਿਰ ਖਾਲੀ
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    ਜੇ (len < 80)
	len = 100;
    ਨਹੀਂਤਾਂ
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (ਪੂਰਨ)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
realloc_cmdbuff(ਪੂਰਨ len)
{
    char_u	*p;
    ਪੂਰਨ		plen;

    ਜੇ (len < ccline.cmdbufflen)
	ਵਾਪਸ OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    ਜੇ (ccline.cmdbuff == ਨਿਰਾ)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ਵਾਪਸ FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    ਜੇ (ccline.xpc != ਨਿਰਾ
	    && ccline.xpc->xp_ਨਮੂਨਾ != ਨਿਰਾ
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	ਪੂਰਨ i = (ਪੂਰਨ)(ccline.xpc->xp_ਨਮੂਨਾ - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	ਜੇ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_ਨਮੂਨਾ = ccline.cmdbuff + i;
    }

    vim_free(p);

    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_ARABIC)
ਸਥਿਰ char_u	*arshape_buf = ਨਿਰਾ;

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_arshape_buf(ਖਾਲੀ)
{
    vim_free(arshape_buf);
}
# endif
#ਜੇ_ਅੰਤ

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_cmdline(ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ len)
{
#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ਪੂਰਨ		i;

    ਜੇ (cmdline_ਤਾਰਾ > 0)
	ਲਈ (i = 0; i < len; ++i)
	{
	    ਸੁਨੇਹਾ_putchar('*');
	    ਜੇ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + ਸ਼ੁਰੂ + i) - 1;
	}
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
	ਜੇ (p_arshape && !p_tbidi && cmdline_has_arabic(ਸ਼ੁਰੂ, len))
    {
	ਸਥਿਰ ਪੂਰਨ	buflen = 0;
	char_u		*p;
	ਪੂਰਨ		j;
	ਪੂਰਨ		newlen = 0;
	ਪੂਰਨ		mb_l;
	ਪੂਰਨ		pc, pc1 = 0;
	ਪੂਰਨ		prev_c = 0;
	ਪੂਰਨ		prev_c1 = 0;
	ਪੂਰਨ		u8c;
	ਪੂਰਨ		u8cc[MAX_MCO];
	ਪੂਰਨ		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	ਜੇ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    ਜੇ (arshape_buf == ਨਿਰਾ)
		ਵਾਪਸ;	// out of memory
	}

	ਜੇ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + ਸ਼ੁਰੂ)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	ਲਈ (j = ਸ਼ੁਰੂ; j < ਸ਼ੁਰੂ + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, ਸ਼ੁਰੂ + len - j);
	    mb_l = utfc_ptr2len_len(p, ਸ਼ੁਰੂ + len - j);
	    ਜੇ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		ਜੇ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    ਜੇ (j + mb_l >= ਸ਼ੁਰੂ + len)
			nc = NUL;
		    ਨਹੀਂਤਾਂ
			nc = utf_ptr2char(p + mb_l);
		}
		ਨਹੀਂਤਾਂ
		{
		    // displaying from left to right
		    ਜੇ (j + mb_l >= ਸ਼ੁਰੂ + len)
			pc = NUL;
		    ਨਹੀਂਤਾਂ
		    {
			ਪੂਰਨ	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      ਸ਼ੁਰੂ + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ਨਿਰਾ, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		ਜੇ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    ਜੇ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	ਸੁਨੇਹਾ_outtrans_len(arshape_buf, newlen);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_outtrans_len(ccline.cmdbuff + ਸ਼ੁਰੂ, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    ਖਾਲੀ
putcmdline(ਪੂਰਨ c, ਪੂਰਨ ਖਿਸਕਾਓ)
{
    ਜੇ (cmd_silent)
	ਵਾਪਸ;
    ਸੁਨੇਹਾ_no_ਹੋਰ = TRUE;
    ਸੁਨੇਹਾ_putchar(c);
    ਜੇ (ਖਿਸਕਾਓ)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    ਸੁਨੇਹਾ_no_ਹੋਰ = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_ਖਿਸਕਾਓ = ਖਿਸਕਾਓ;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    ਖਾਲੀ
unputcmdline(ਖਾਲੀ)
{
    ਜੇ (cmd_silent)
	ਵਾਪਸ;
    ਸੁਨੇਹਾ_no_ਹੋਰ = TRUE;
    ਜੇ (ccline.cmdlen == ccline.cmdpos)
	ਸੁਨੇਹਾ_putchar(' ');
    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    ਨਹੀਂਤਾਂ
	draw_cmdline(ccline.cmdpos, 1);
    ਸੁਨੇਹਾ_no_ਹੋਰ = FALSE;
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
    ਪੂਰਨ
put_on_cmdline(char_u *str, ਪੂਰਨ len, ਪੂਰਨ redraw)
{
    ਪੂਰਨ		retval;
    ਪੂਰਨ		i;
    ਪੂਰਨ		m;
    ਪੂਰਨ		c;

    ਜੇ (len < 0)
	len = (ਪੂਰਨ)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    ਜੇ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    ਨਹੀਂਤਾਂ
	retval = OK;
    ਜੇ (retval == OK)
    {
	ਜੇ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		ਲਈ (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		ਲਈ (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		ਜੇ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		ਨਹੀਂਤਾਂ
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	ਜੇ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    ਜਦੋਂ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_ਸਿਰ_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
	    ਜੇ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_ਸਿਰ_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ਜੇ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		ਨਹੀਂਤਾਂ
		    i = 0;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		ਸੁਨੇਹਾ_ਖਾਨਾ -= i;
		ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ < 0)
		{
		    ਸੁਨੇਹਾ_ਖਾਨਾ += cmdline_ਚੌੜਾਈ;
		    --ਸੁਨੇਹਾ_ਪੰਕਤੀ;
		}
	    }
	}

	ਜੇ (redraw && !cmd_silent)
	{
	    ਸੁਨੇਹਾ_no_ਹੋਰ = TRUE;
	    i = cmdline_ਪੰਕਤੀ;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    ਜੇ (cmdline_ਪੰਕਤੀ != i || ccline.overstrike)
		ਸੁਨੇਹਾ_clr_eos();
	    ਸੁਨੇਹਾ_no_ਹੋਰ = FALSE;
	}
	ਜੇ (ਕੁੰਜੀ_Typed)
	{
	    m = cmdline_ਚੌੜਾਈ * ਪੰਕਤੀਆਂ;
	    ਜੇ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	ਨਹੀਂਤਾਂ
	    m = MAXCOL;
	ਲਈ (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    ਜੇ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    ਜੇ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    ਜੇ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		ਜੇ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    ਜੇ (redraw)
	ਸੁਨੇਹਾ_check();
    ਵਾਪਸ retval;
}

ਸਥਿਰ cmdline_info_T	prev_ccline;
ਸਥਿਰ ਪੂਰਨ		prev_ccline_ਵਰਤਿਆ = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ਸਥਿਰ ਖਾਲੀ
save_cmdline(cmdline_info_T *ccp)
{
    ਜੇ (!prev_ccline_ਵਰਤਿਆ)
    {
	CLEAR_ਖੇਤਰ(prev_ccline);
	prev_ccline_ਵਰਤਿਆ = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ਨਿਰਾ;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ਸਥਿਰ ਖਾਲੀ
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
    ਸਥਿਰ ਪੂਰਨ
cmdline_paste(
    ਪੂਰਨ regname,
    ਪੂਰਨ literally,	// Insert text literally instead of "as typed"
    ਪੂਰਨ remcr)		// remove trailing CR
{
    ਲੰਮਾ		i;
    char_u		*arg;
    char_u		*p;
    ਪੂਰਨ			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    ਜੇ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ਵਾਪਸ FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    ਸਤਰ_breakcheck();
    ਜੇ (got_int)
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#ਜੇ_ਅੰਤ

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    ਜੇ (i)
    {
	// Got the value of a special register in "arg".
	ਜੇ (arg == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	ਜੇ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    ਪੂਰਨ	    len;

	    // Locate start of last word in the cmd buffer.
	    ਲਈ (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		ਜੇ (has_mbyte)
		{
		    len = (*mb_ਸਿਰ_off)(ccline.cmdbuff, w - 1) + 1;
		    ਜੇ (!vim_iswordc(mb_ptr2char(w - len)))
			ਰੋਕੋ;
		    w -= len;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (!vim_iswordc(w[-1]))
			ਰੋਕੋ;
		    --w;
		}
	    }
	    len = (ਪੂਰਨ)((ccline.cmdbuff + ccline.cmdpos) - w);
	    ਜੇ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	ਜੇ (allocated)
	    vim_free(arg);
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    ਖਾਲੀ
cmdline_paste_str(char_u *s, ਪੂਰਨ literally)
{
    ਪੂਰਨ		c, cv;

    ਜੇ (literally)
	put_on_cmdline(s, -1, TRUE);
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (*s != NUL)
	{
	    cv = *s;
	    ਜੇ (cv == Ctrl_V && s[1])
		++s;
	    ਜੇ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    ਨਹੀਂਤਾਂ
		c = *s++;
	    ਜੇ (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    || c == intr_char
#ਜੇ_ਅੰਤ
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
    ਖਾਲੀ
redrawcmdline(ਖਾਲੀ)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    ਖਾਲੀ
redrawcmdline_ex(ਪੂਰਨ do_compute_cmdrow)
{
    ਜੇ (cmd_silent)
	ਵਾਪਸ;
    need_wait_return = FALSE;
    ਜੇ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ਸਥਿਰ ਖਾਲੀ
redrawcmdprompt(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਜੇ (cmd_silent)
	ਵਾਪਸ;
    ਜੇ (ccline.cmdfirstc != NUL)
	ਸੁਨੇਹਾ_putchar(ccline.cmdfirstc);
    ਜੇ (ccline.cmdprompt != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = ਸੁਨੇਹਾ_ਖਾਨਾ + (ਸੁਨੇਹਾ_ਪੰਕਤੀ - cmdline_ਪੰਕਤੀ) * cmdline_ਚੌੜਾਈ;
	// do the reverse of set_cmdspos()
	ਜੇ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    ਨਹੀਂਤਾਂ
	ਲਈ (i = ccline.cmdindent; i > 0; --i)
	    ਸੁਨੇਹਾ_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    ਖਾਲੀ
redrawcmd(ਖਾਲੀ)
{
    ਪੂਰਨ save_ਵਿੱਚ_echowindow = ਵਿੱਚ_echowindow;

    ਜੇ (cmd_silent)
	ਵਾਪਸ;

    // when 'incsearch' is set there may be no command line while redrawing
    ਜੇ (ccline.cmdbuff == ਨਿਰਾ)
    {
	windgoto(cmdline_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off);
	ਸੁਨੇਹਾ_clr_eos();
	ਵਾਪਸ;
    }

    // Do not put this in the message window.
    ਵਿੱਚ_echowindow = FALSE;

    sb_ਪਾਠ_restart_cmdline();
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    ਸੁਨੇਹਾ_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    ਸੁਨੇਹਾ_no_ਹੋਰ = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    ਸੁਨੇਹਾ_clr_eos();
    ਸੁਨੇਹਾ_no_ਹੋਰ = FALSE;

    ਸੈੱਟ_cmdspos_cursor();
    ਜੇ (extra_char != NUL)
	putcmdline(extra_char, extra_char_ਖਿਸਕਾਓ);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    ਸੁਨੇਹਾ_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    ਵਿੱਚ_echowindow = save_ਵਿੱਚ_echowindow;
}

    ਖਾਲੀ
compute_cmdrow(ਖਾਲੀ)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    ਜੇ (exmode_ਸਰਗਰਮ || (ਸੁਨੇਹਾ_scrolled != 0 && !updating_screen))
	cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    ਨਹੀਂਤਾਂ
	cmdline_ਪੰਕਤੀ = W_WINROW(lastwin) + lastwin->w_ਉਚਾਈ
						    + lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ;
}

    ਖਾਲੀ
cursorcmd(ਖਾਲੀ)
{
    ਜੇ (cmd_silent)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (cmdmsg_rl)
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ  + (ccline.cmdspos / (cmdline_ਚੌੜਾਈ - 1));
	ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - (ccline.cmdspos % (cmdline_ਚੌੜਾਈ - 1)) - 1;
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ <= 0)
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ + (ccline.cmdspos / cmdline_ਚੌੜਾਈ);
	ਸੁਨੇਹਾ_ਖਾਨਾ = ccline.cmdspos % cmdline_ਚੌੜਾਈ;
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    }

    windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ਜੇ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MCH_CURSOR_SHAPE
    mch_update_cursor();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gotocmdline(ਪੂਰਨ clr)
{
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (cmdmsg_rl)
	ਸੁਨੇਹਾ_ਖਾਨਾ = cmdline_ਚੌੜਾਈ - 1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_ਖਾਨਾ = 0;	    // always start in column 0
    ਜੇ (clr)		    // clear the bottom line(s)
	ਸੁਨੇਹਾ_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ਸਥਿਰ ਪੂਰਨ
ccheck_abbr(ਪੂਰਨ c)
{
    ਪੂਰਨ spos = 0;

    ਜੇ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ਵਾਪਸ FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    ਜਦੋਂ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    ਜੇ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    ਨਹੀਂਤਾਂ
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ਵਾਪਸ check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, ਪੂਰਨ what)
{
    char_u	*p;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    char_u	buf[20];
    ਪੂਰਨ		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    ਲਈ (p = what == VSE_ਬਫਰ ? ਬਫਰ_ESC_CHARS : ਰਸਤਾ_ESC_CHARS;
								*p != NUL; ++p)
	ਜੇ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#ਨਹੀਂਤਾਂ
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_ਬਫਰ ? ਬਫਰ_ESC_CHARS : ਰਸਤਾ_ESC_CHARS);
    ਜੇ (what == VSE_SHELL && csh_like_shell() && p != ਨਿਰਾ)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#ਜੇ_ਅੰਤ

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    ਜੇ (p != ਨਿਰਾ && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ਵਾਪਸ p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    ਖਾਲੀ
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    ਖਾਲੀ
tilde_replace(
    char_u  *orig_pat,
    ਪੂਰਨ	    ਅੰਕ_files,
    char_u  **files)
{
    ਪੂਰਨ	    i;
    char_u  *p;

    ਜੇ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	ਲਈ (i = 0; i < ਅੰਕ_files; ++i)
	{
	    p = home_replace_save(ਨਿਰਾ, files[i]);
	    ਜੇ (p != ਨਿਰਾ)
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
get_cmdline_info(ਖਾਲੀ)
{
    ਵਾਪਸ &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ਸਥਿਰ cmdline_info_T *
get_ccline_ptr(ਖਾਲੀ)
{
    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) == 0)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (ccline.cmdbuff != ਨਿਰਾ)
	ਵਾਪਸ &ccline;
    ਜੇ (prev_ccline_ਵਰਤਿਆ && prev_ccline.cmdbuff != ਨਿਰਾ)
	ਵਾਪਸ &prev_ccline;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
get_cmdline_type(ਖਾਲੀ)
{
    cmdline_info_T *p = get_ccline_ptr();

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ NUL;
    ਜੇ (p->cmdfirstc == NUL)
	ਵਾਪਸ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    (p->ਨਿਵੇਸ਼_fn) ? '@' :
#ਜੇ_ਅੰਤ
	    '-';
    ਵਾਪਸ p->cmdfirstc;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ਸਥਿਰ char_u *
get_cmdline_str(ਖਾਲੀ)
{
    cmdline_info_T *p;

    ਜੇ (cmdline_ਤਾਰਾ > 0)
	ਵਾਪਸ ਨਿਰਾ;
    p = get_ccline_ptr();
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ਸਥਿਰ char_u *
get_cmdline_completion_ਨਮੂਨਾ(ਖਾਲੀ)
{
    cmdline_info_T *p;
    ਪੂਰਨ		xp_context;

    ਜੇ (cmdline_ਤਾਰਾ > 0)
	ਵਾਪਸ ਨਿਰਾ;

    p = get_ccline_ptr();
    ਜੇ (p == ਨਿਰਾ || p->xpc == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    xp_context = p->xpc->xp_context;
    ਜੇ (xp_context == EXPAND_NOTHING)
    {
	ਸੈੱਟ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ਜੇ (xp_context == EXPAND_UNSUCCESSFUL)
	ਵਾਪਸ ਨਿਰਾ;

    char_u *compl_pat = p->xpc->xp_ਨਮੂਨਾ;

    ਜੇ (compl_pat == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ਸਥਿਰ char_u *
get_cmdline_completion(ਖਾਲੀ)
{
    cmdline_info_T	*p;
    ਪੂਰਨ			xp_context;

    ਜੇ (cmdline_ਤਾਰਾ > 0)
	ਵਾਪਸ ਨਿਰਾ;

    p = get_ccline_ptr();
    ਜੇ (p == ਨਿਰਾ || p->xpc == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    xp_context = p->xpc->xp_context;
    ਜੇ (xp_context == EXPAND_NOTHING)
    {
	ਸੈੱਟ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ਜੇ (xp_context == EXPAND_UNSUCCESSFUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ cmdcomplete_type_ਮੰਜ਼ਿਲ_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    ਖਾਲੀ
f_getcmdcomplpat(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_ਨਮੂਨਾ();
}

/*
 * "getcmdcompltype()" function
 */
    ਖਾਲੀ
f_getcmdcompltype(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    ਖਾਲੀ
f_getcmdline(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    ਖਾਲੀ
f_getcmdpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ਸੰਖਿਆ = p != ਨਿਰਾ ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    ਖਾਲੀ
f_getcmdprompt(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ਨਿਰਾ && p->cmdprompt != ਨਿਰਾ ?
					    vim_strsave(p->cmdprompt) : ਨਿਰਾ;
}

/*
 * "getcmdscreenpos()" function
 */
    ਖਾਲੀ
f_getcmdscreenpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ਸੰਖਿਆ = p != ਨਿਰਾ ? cmdline_ਖਾਨਾ_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    ਖਾਲੀ
f_getcmdtype(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    ਜੇ (rettv->vval.v_string == ਨਿਰਾ)
	ਵਾਪਸ;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_cmdline_str(char_u *str, ਪੂਰਨ pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    ਪੂਰਨ		    len;

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ 1;

    len = (ਪੂਰਨ)STRLEN(str);
    ਜੇ (realloc_cmdbuff(len + 1) != OK)
	ਵਾਪਸ 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ਵਾਪਸ 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_cmdline_pos(
    ਪੂਰਨ		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    ਜੇ (pos < 0)
	new_cmdpos = 0;
    ਨਹੀਂਤਾਂ
	new_cmdpos = pos;

    ਵਾਪਸ 0;
}

// "setcmdline()" function
    ਖਾਲੀ
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ pos = -1;

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਪੂਰਨ error = FALSE;

	pos = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error) - 1;
	ਜੇ (error)
	    ਵਾਪਸ;
	ਜੇ (pos < 0)
	{
	    emsg(_(e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ));
	    ਵਾਪਸ;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_ਸੰਖਿਆ = ਸੈੱਟ_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    ਖਾਲੀ
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		pos;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pos = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]) - 1;
    ਜੇ (pos >= 0)
	rettv->vval.v_ਸੰਖਿਆ = ਸੈੱਟ_cmdline_pos(pos);
}
#ਜੇ_ਅੰਤ

/*
 * Return the first character of the current command line.
 */
    ਪੂਰਨ
get_cmdline_firstc(ਖਾਲੀ)
{
    ਵਾਪਸ ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    ਪੂਰਨ
get_list_range(char_u **str, ਪੂਰਨ *num1, ਪੂਰਨ *num2)
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		ਪਹਿਲਾ = FALSE;
    varnumber_T	ਅੰਕ;

    *str = skipwhite(*str);
    ਜੇ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ਨਿਰਾ, &len, 0, &ਅੰਕ, ਨਿਰਾ, 0, FALSE, ਨਿਰਾ);
	*str += len;
	//ਉਛਾਲ
	ਜੇ (ਅੰਕ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	    ਵਾਪਸ FAIL;

	*num1 = (ਪੂਰਨ)ਅੰਕ;
	ਪਹਿਲਾ = TRUE;
    }
    *str = skipwhite(*str);
    ਜੇ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ਨਿਰਾ, &len, 0, &ਅੰਕ, ਨਿਰਾ, 0, FALSE, ਨਿਰਾ);
	ਜੇ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    //ਉਛਾਲ
	    ਜੇ (ਅੰਕ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
		ਵਾਪਸ FAIL;

	    *num2 = (ਪੂਰਨ)ਅੰਕ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!ਪਹਿਲਾ)		// no number given at all
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ)			// only one number given
	*num2 = *num1;
    ਵਾਪਸ OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cedit(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ n;

    ਜੇ (*p_cedit == NUL)
	cedit_ਕੁੰਜੀ = -1;
    ਨਹੀਂਤਾਂ
    {
	n = string_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(p_cedit, FALSE);
	ਜੇ (n == 0 || vim_isprintc(n))
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	cedit_ਕੁੰਜੀ = n;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ਸਥਿਰ ਪੂਰਨ
open_cmdwin(ਖਾਲੀ)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    ਪੂਰਨ			i;
    linenr_T		lnum;
    ਪੂਰਨ			histtype;
    garray_T		winsizes;
    ਪੂਰਨ			save_restart_edit = restart_edit;
    ਪੂਰਨ			save_State = State;
    ਪੂਰਨ			save_exmode = exmode_ਸਰਗਰਮ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਪੂਰਨ			save_cmdmsg_rl = cmdmsg_rl;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਪੂਰਨ			save_ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ			newbuf_ਸਥਿਤੀ;
    ਪੂਰਨ			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    ਜੇ (ਪਾਠ_or_buf_locked()
	    || cmdwin_type != 0
#ਜੇ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_ਤਾਰਾ > 0
#ਜੇ_ਅੰਤ
	    )
    {
	beep_flush();
	ਵਾਪਸ K_IGNORE;
    }
    ਸੈੱਟ_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_ਮਾਪ_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    ਜੇ (win_ਵੰਡੋ((ਪੂਰਨ)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ਵਾਪਸ K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    ਜੇ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_ਬਫਰ != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ਵਾਪਸ Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_ਸਥਿਤੀ = do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE, ECMD_HIDE,
					ਨਿਰਾ);
    cmdwin_valid = win_valid(cmdwin_win);
    ਜੇ (newbuf_ਸਥਿਤੀ == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_ਬਫਰ != old_curbuf.br_buf)
    {
	ਜੇ (newbuf_ਸਥਿਤੀ == OK)
	    ਸੈੱਟ_bufref(&bufref, curbuf);
	ਜੇ (cmdwin_valid && !ਆਖਰੀ_ਖਿੜਕੀ())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	ਜੇ (newbuf_ਸਥਿਤੀ == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_ਬਫਰ(ਨਿਰਾ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ਨਿਰਾ;
	beep_flush();
	ga_clear(&winsizes);
	ਵਾਪਸ Ctrl_C;
    }
    cmdwin_buf = curbuf;

    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#ਜੇ_ਅੰਤ
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_ਤਾਲਾ;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    ਜੇ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	ਜੇ (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    ਜੋੜੋ_map((char_u *)"<buffer> <Tab> <C-X><C-V>", ਬਹੁਲਕ_ਪਾਓ, TRUE);
	    ਜੋੜੋ_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", ਬਹੁਲਕ_ਸਾਧਾਰਨ, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    ਜੋੜੋ_map((char_u *)"<buffer> <S-Tab> <C-P>", ਬਹੁਲਕ_ਪਾਓ, TRUE);
	}
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_ਤਾਲਾ;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    ਜੇ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	ਜੇ (i >= 0)
	{
	    lnum = 0;
	    ਕਰੋ
	    {
		ਜੇ (++i == get_hislen())
		    i = 0;
		ਜੇ (get_histentry(histtype)[i].hisstr != ਨਿਰਾ)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    ਜਦੋਂ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    curwin->w_cursor.ਖਾਨਾ = ccline.cmdpos;
    ਬਦਲਿਆ_ਸਤਰ_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_ਸਰਗਰਮ = 0;

    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_ਨਤੀਜਾ = 0;

    // Trigger CmdwinEnter autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINENTER);
    ਜੇ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ

    cmdwin_type = 0;
    cmdwin_buf = ਨਿਰਾ;
    cmdwin_win = ਨਿਰਾ;
    exmode_ਸਰਗਰਮ = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    ਜੇ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_ਬਫਰ != old_curbuf.br_buf)
    {
	cmdwin_ਨਤੀਜਾ = Ctrl_C;
	emsg(_(e_ਸਰਗਰਮ_ਖਿੜਕੀ_or_ਬਫਰ_ਬਦਲਿਆ_or_deleted));
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(FEAT_EVAL)
	// autocmds may abort script processing
	ਜੇ (aborting() && cmdwin_ਨਤੀਜਾ != K_IGNORE)
	    cmdwin_ਨਤੀਜਾ = Ctrl_C;
#ਜੇ_ਅੰਤ
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	ਜੇ (cmdwin_ਨਤੀਜਾ == K_XF1 || cmdwin_ਨਤੀਜਾ == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    ਜੇ (cmdwin_ਨਤੀਜਾ == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    ਜੇ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (ਪੂਰਨ)plen;
		ccline.cmdbufflen = (ਪੂਰਨ)(plen + 1);
		cmdwin_ਨਤੀਜਾ = CAR;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (cmdwin_ਨਤੀਜਾ == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	ਜੇ (ccline.cmdbuff == ਨਿਰਾ)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_ਨਤੀਜਾ = Ctrl_C;
	}
	ਨਹੀਂਤਾਂ
	{
	    ccline.cmdpos = curwin->w_cursor.ਖਾਨਾ;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    ਜੇ (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#ਜੇ_ਅੰਤ
	// First go back to the original window.
	wp = curwin;
	ਸੈੱਟ_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	ਜੇ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	ਜੇ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_ਬਫਰ(ਨਿਰਾ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_ਮਾਪ_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	ਜੇ (cmdwin_ਨਤੀਜਾ == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    ਸੈੱਟ_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#ਜੇ_ਅੰਤ

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    ਵਾਪਸ cmdwin_ਨਤੀਜਾ;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    ਪੂਰਨ
is_ਵਿੱਚ_cmdwin(ਖਾਲੀ)
{
    ਵਾਪਸ cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap ਅਣਵਰਤਿਆ, char_u *cmd ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    ਜੇ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    ਖਾਲੀ
get_user_ਨਿਵੇਸ਼(
    typval_T	*argvars,
    typval_T	*rettv,
    ਪੂਰਨ		inputdialog,
    ਪੂਰਨ		secret)
{
    char_u	*prompt;
    char_u	*p = ਨਿਰਾ;
    ਪੂਰਨ		c;
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    ਪੂਰਨ		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ਨਿਰਾ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
    ਜੇ (ਨਿਵੇਸ਼_busy)
	ਵਾਪਸ;  // this doesn't work recursively.

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_ਨਿਵੇਸ਼
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ਜੇ (no_console_ਨਿਵੇਸ਼() && !is_not_a_ਪਦ())
	ਵਾਪਸ;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    ਜੇ (prompt != ਨਿਰਾ)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	ਜੇ (p == ਨਿਰਾ)
	    p = prompt;
	ਨਹੀਂਤਾਂ
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	    ਸੁਨੇਹਾ_clr_eos();
	    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)prompt, get_echo_attr());
	    ਸੁਨੇਹਾ_didout = FALSE;
	    ਸੁਨੇਹਾ_starthere();
	    *p = c;
	}
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    ਜੇ (defstr != ਨਿਰਾ)
		stuffReadbuffSpec(defstr);

	    ਜੇ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		ਪੂਰਨ	xp_namelen;
		ਲੰਮਾ	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ਨਿਰਾ;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		ਜੇ (xp_name == ਨਿਰਾ)
		    ਵਾਪਸ;

		xp_namelen = (ਪੂਰਨ)STRLEN(xp_name);

		ਜੇ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ਵਾਪਸ;
	    }
	}

	ਜੇ (defstr != ਨਿਰਾ)
	{
	    ਪੂਰਨ save_ex_ਸਾਧਾਰਨ_busy = ex_ਸਾਧਾਰਨ_busy;
	    ਪੂਰਨ save_vgetc_busy = vgetc_busy;
	    ਪੂਰਨ save_ਨਿਵੇਸ਼_busy = ਨਿਵੇਸ਼_busy;

	    ਨਿਵੇਸ਼_busy |= vgetc_busy;
	    ex_ਸਾਧਾਰਨ_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_ਸਾਧਾਰਨ_busy = save_ex_ਸਾਧਾਰਨ_busy;
	    vgetc_busy = save_vgetc_busy;
	    ਨਿਵੇਸ਼_busy = save_ਨਿਵੇਸ਼_busy;
	}
	ਜੇ (inputdialog && rettv->vval.v_string == ਨਿਰਾ
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	ਸੁਨੇਹਾ_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#ਜੇ_ਅੰਤ

/*
 * "wildtrigger()" function
 */
    ਖਾਲੀ
f_wildtrigger(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (!(State & ਬਹੁਲਕ_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_ਸਰਗਰਮ())
	ਵਾਪਸ;

    ਪੂਰਨ cmd_type = get_cmdline_type();

    ਜੇ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	ਕੁੰਜੀ_string[4];

	ਕੁੰਜੀ_string[0] = K_SPECIAL;
	ਕੁੰਜੀ_string[1] = KS_EXTRA;
	ਕੁੰਜੀ_string[2] = KE_WILD;
	ਕੁੰਜੀ_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(ਕੁੰਜੀ_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
