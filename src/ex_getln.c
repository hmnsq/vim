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

#ಅನ್ತರ್ಭಾವ "vim.h"

// Return value when handling keys in command-line mode.
#ವ್ಯಾಖ್ಯಾನಿಸು CMDLINE_NOT_ಬದಲಾಯಿತು	1
#ವ್ಯಾಖ್ಯಾನಿಸು CMDLINE_ಬದಲಾಯಿತು		2
#ವ್ಯಾಖ್ಯಾನಿಸು GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ	3
#ವ್ಯಾಖ್ಯಾನಿಸು PROCESS_ಮುಂದಿನ_ಕೀಲಿ	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ಸ್ಥಿರ cmdline_info_T ccline;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	new_cmdpos;	// position set by set_cmdline_pos()
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	extra_char_ಸರಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cmd_hkmap = 0;	// Hebrew mapping during command line
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ char_u	*getcmdline_int(ಪೂರ್ಣಾಂಕ firstc, ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ indent, ಪೂರ್ಣಾಂಕ clear_ccline);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cmdline_charsize(ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ	ಗಣ_cmdspos(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	ಗಣ_cmdspos_cursor(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	correct_cmdspos(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ cells);
ಸ್ಥಿರ ಶೂನ್ಯ	dealloc_cmdbuff(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	alloc_cmdbuff(size_t len);
ಸ್ಥಿರ ಶೂನ್ಯ	draw_cmdline(ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಶೂನ್ಯ	save_cmdline(cmdline_info_T *ccp);
ಸ್ಥಿರ ಶೂನ್ಯ	restore_cmdline(cmdline_info_T *ccp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cmdline_paste(ಪೂರ್ಣಾಂಕ regname, ಪೂರ್ಣಾಂಕ literally, ಪೂರ್ಣಾಂಕ remcr);
ಸ್ಥಿರ ಶೂನ್ಯ	redrawcmdprompt(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ccheck_abbr(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	open_cmdwin(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಖಾಲಿಗಳು_ನಮೂನೆ_magic(char_u *pat, size_t len, magic_T magic_val);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cedit_ಕೀಲಿ = -1;	// key value of 'cedit' option

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_cmd_autocmd(ಪೂರ್ಣಾಂಕ typechar, ಪೂರ್ಣಾಂಕ evt)
{
    char_u	typestr[2];

    typestr[0] = typechar;
    typestr[1] = NUL;
    apply_autocmds(evt, typestr, typestr, FALSE, curbuf);
}

/*
 * Abandon the command line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
abandon_cmdline(ಶೂನ್ಯ)
{
    dealloc_cmdbuff();
    ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
	compute_cmdrow();
    ಸಂದೇಶ("");
    redraw_cmdline = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಖಾಲಿಗಳು_ನಮೂನೆ(char_u *p, size_t len, ಪೂರ್ಣಾಂಕ delim)
{
    magic_T	magic_val = MAGIC_ON;

    ಆದರೆ (len > 0)
	(ಶೂನ್ಯ) skip_regexp_ex(p, delim, magic_isset(), ಖಾಲಿ, ಖಾಲಿ, &magic_val);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_ನಮೂನೆ_magic(p, len, magic_val);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಖಾಲಿಗಳು_ನಮೂನೆ_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    ಆಗ (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != ಖಾಲಿ)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    ಹಿಂತಿರುಗಿ len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
} viewstate_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
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
    vs->vs_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
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
    curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = vs->vs_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
}

// Struct to store the state of 'incsearch' highlighting.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    pos_T	ಹುಡುಕು_ಆರಂಭ_2;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    ಪೂರ್ಣಾಂಕ		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	ಹೊಂದಿಕೆ_ಆರಂಭ;
    pos_T	ಹೊಂದಿಕೆ_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		did_incsearch;
    ಪೂರ್ಣಾಂಕ		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ಸ್ಥಿರ ಶೂನ್ಯ
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->ಹೊಂದಿಕೆ_ಆರಂಭ = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->ಹೊಂದಿಕೆ_ಕೊನೆ);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->ಹುಡುಕು_ಆರಂಭ_2 = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಹುಡುಕು_ಹೊಂದಿಕೆ(pos_T *t)
{
    t->lnum += ಹುಡುಕು_ಹೊಂದಿಕೆ_ಸಾಲುಗಳು;
    t->ಲಂಬಸಾಲು = ಹುಡುಕು_ಹೊಂದಿಕೆ_endcol;
    ಆದರೆ (t->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	t->lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	coladvance((colnr_T)MAXCOL);
    }
}

/*
 * Parses the :[range]s/foo like commands and returns details needed for
 * incsearch and wildmenu completion.
 * Returns TRUE if pattern is valid.
 * Sets skiplen, patlen, search_first_line, and search_last_line.
 */
    ಪೂರ್ಣಾಂಕ
parse_ನಮೂನೆ_and_range(
	pos_T	*incsearch_ಆರಂಭ,
	ಪೂರ್ಣಾಂಕ	*ಹುಡುಕು_delim,
	ಪೂರ್ಣಾಂಕ	*skiplen,
	ಪೂರ್ಣಾಂಕ	*patlen)
{
    char_u	*cmd, *p, *ಕೊನೆ;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    ಪೂರ್ಣಾಂಕ		delim_optional = FALSE;
    ಪೂರ್ಣಾಂಕ		delim;
    ಪೂರ್ಣಾಂಕ		use_ಕೊನೆಯ_pat;
    magic_T     magic = 0;
    ಅಕ್ಷರ	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = 0;
    ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು = MAXLNUM;

    CLEAR_ಕ್ಷೇತ್ರ(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_ಸಾಲುಗಳು;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, ಖಾಲಿ);

    ಆದರೆ (vim_strchr((char_u *)"sgvlu", *cmd) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // Skip over command name to find pattern separator
    ಗಾಗಿ (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    ಆದರೆ (*skipwhite(p) == NUL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	ಆದರೆ (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	ಆದರೆ (*p == '!')
	    p = skipwhite(p + 1);
	ಆಗ (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	ಆದರೆ (*p == NUL)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	ಆದರೆ (*p == '!')
	{
	    p++;
	    ಆದರೆ (*skipwhite(p) == NUL)
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಆದರೆ (*cmd != 'g')
	    delim_optional = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *ಹುಡುಕು_delim = delim;

    ಕೊನೆ = skip_regexp_ex(p, delim, magic_isset(), ಖಾಲಿ, ಖಾಲಿ, &magic);
    use_ಕೊನೆಯ_pat = ಕೊನೆ == p && *ಕೊನೆ == delim;

    ಆದರೆ (ಕೊನೆ == p && !use_ಕೊನೆಯ_pat)
	ಹಿಂತಿರುಗಿ FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    ಆದರೆ (!use_ಕೊನೆಯ_pat)
    {
	ಅಕ್ಷರ c = *ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_2;

	*ಕೊನೆ = NUL;
	ಖಾಲಿಗಳು_2 = ಖಾಲಿಗಳು_ನಮೂನೆ_magic(p, (size_t)(ಕೊನೆ - p), magic);
	*ಕೊನೆ = c;
	ಆದರೆ (ಖಾಲಿಗಳು_2)
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (ಪೂರ್ಣಾಂಕ)(p - ccline.cmdbuff);
    *patlen = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_ಆರಂಭ;

    parse_cmd_ವಿಳಾಸ(&ea, &dummy, TRUE);

    ಆದರೆ (ea.addr_ಎಣಿಕೆ > 0)
    {
	ಪೂರ್ಣಾಂಕ ಹಿಮ್ಮುಖ_ಹೊಂದಿಕೆ = ea.line2 < ea.line1;
	ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = ಹಿಮ್ಮುಖ_ಹೊಂದಿಕೆ ? ea.line2 : ea.line1;
	ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು = ಹಿಮ್ಮುಖ_ಹೊಂದಿಕೆ ? ea.line1 : ea.line2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_incsearch_highlighting(
	ಪೂರ್ಣಾಂಕ		    firstc,
	ಪೂರ್ಣಾಂಕ		    *ಹುಡುಕು_delim,
	incsearch_state_T   *is_state,
	ಪೂರ್ಣಾಂಕ		    *skiplen,
	ಪೂರ್ಣಾಂಕ		    *patlen)
{
    ಪೂರ್ಣಾಂಕ retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    ಆದರೆ (!p_is || cmd_silent)
	ಹಿಂತಿರುಗಿ FALSE;

    // By default search all lines
    ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = 0;
    ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು = MAXLNUM;

    ಆದರೆ (firstc == '/' || firstc == '?')
    {
	*ಹುಡುಕು_delim = firstc;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (firstc != ':')
	ಹಿಂತಿರುಗಿ FALSE;

    ++emsg_off;
    retval = parse_ನಮೂನೆ_and_range(&is_state->ಹುಡುಕು_ಆರಂಭ_2, ಹುಡುಕು_delim,
	    skiplen, patlen);
    --emsg_off;

    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಮುಕ್ತಾಯ_incsearch_highlighting(
	ಪೂರ್ಣಾಂಕ gotesc,
	incsearch_state_T *is_state,
	ಪೂರ್ಣಾಂಕ call_update_screen)
{
    ಆದರೆ (!is_state->did_incsearch)
	ಹಿಂತಿರುಗಿ;

    is_state->did_incsearch = FALSE;
    ಆದರೆ (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!EQUAL_POS(is_state->save_cursor, is_state->ಹುಡುಕು_ಆರಂಭ_2))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->ಹುಡುಕು_ಆರಂಭ_2;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_ಹೊಂದಿಕೆ = FALSE;

    // by default search all lines
    ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = 0;
    ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    ಸ್ಥಿತಿ_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    ಆದರೆ (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_do_incsearch_highlighting(
	ಪೂರ್ಣಾಂಕ		    firstc,
	ದೀರ್ಘ		    ಎಣಿಕೆ_2,
	incsearch_state_T   *is_state)
{
    ಪೂರ್ಣಾಂಕ		skiplen, patlen;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು;  // do_search() result
    pos_T	ಕೊನೆ_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_char;
    ಪೂರ್ಣಾಂಕ		use_ಕೊನೆಯ_pat;
    ಪೂರ್ಣಾಂಕ		did_do_incsearch = is_state->did_incsearch;
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();

    ಆದರೆ (!do_incsearch_highlighting(firstc, &ಹುಡುಕು_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
	ಮುಕ್ತಾಯ_incsearch_highlighting(FALSE, is_state, TRUE);
	ಆದರೆ (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	ಹಿಂತಿರುಗಿ;
    }

    // If there is a character waiting, search and redraw later.
    ಆದರೆ (char_avail())
    {
	restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
	is_state->incsearch_postponed = TRUE;
	ಹಿಂತಿರುಗಿ;
    }
    is_state->incsearch_postponed = FALSE;

    ಆದರೆ (ಹುಡುಕು_ಮೊದಲ_ಸಾಲು == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->ಹುಡುಕು_ಆರಂಭ_2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹುಡುಕು_ಮೊದಲ_ಸಾಲು > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	curwin->w_cursor.ಲಂಬಸಾಲು = MAXCOL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = ಹುಡುಕು_ಮೊದಲ_ಸಾಲು;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    }

    // Use the previous pattern for ":s//".
    ಮುಂದಿನ_char = ccline.cmdbuff[skiplen + patlen];
    use_ಕೊನೆಯ_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == ಮುಂದಿನ_char;

    // If there is no pattern, don't do anything.
    ಆದರೆ (patlen == 0 && !use_ಕೊನೆಯ_pat)
    {
	ಸಿಕ್ಕಿತು = 0;
	ಗಣ_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ ಹುಡುಕು_flags = ಹುಡುಕು_OPT + ಹುಡುಕು_NOOF + ಹುಡುಕು_PEEK;

	cursor_off();	// so the user knows we're busy
	ಹೊರಹೋಗು_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	ಆದರೆ (!p_hls)
	    ಹುಡುಕು_flags += ಹುಡುಕು_KEEP;
	ಆದರೆ (ಹುಡುಕು_ಮೊದಲ_ಸಾಲು != 0)
	    ಹುಡುಕು_flags += ಹುಡುಕು_ಆರಂಭ;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_ಕ್ಷೇತ್ರ(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	ಸಿಕ್ಕಿತು = do_ಹುಡುಕು(ಖಾಲಿ, firstc == ':' ? '/' : firstc, ಹುಡುಕು_delim,
				 ccline.cmdbuff + skiplen, patlen, ಎಣಿಕೆ_2, ಹುಡುಕು_flags,
# ifdef FEAT_RELTIME
		&sia
# ಇಲ್ಲದಿದ್ದರೆ
		ಖಾಲಿ
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = ಮುಂದಿನ_char;
	--emsg_off;

	ಆದರೆ (curwin->w_cursor.lnum < ಹುಡುಕು_ಮೊದಲ_ಸಾಲು
		|| curwin->w_cursor.lnum > ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು)
	{
	    // match outside of address range
	    ಸಿಕ್ಕಿತು = 0;
	    curwin->w_cursor = is_state->ಹುಡುಕು_ಆರಂಭ_2;
	}

	// if interrupted while searching, behave like it failed
	ಆದರೆ (got_int)
	{
	    (ಶೂನ್ಯ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    ಸಿಕ್ಕಿತು = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    ಆದರೆ (ಸಿಕ್ಕಿತು != 0)
	highlight_ಹೊಂದಿಕೆ = TRUE;		// highlight position
    ಇಲ್ಲದಿದ್ದರೆ
	highlight_ಹೊಂದಿಕೆ = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    ಬದಲಾಯಿತು_cline_bef_curs();
    update_topline();

    ಆದರೆ (ಸಿಕ್ಕಿತು != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->ಹೊಂದಿಕೆ_ಆರಂಭ = curwin->w_cursor;
	ಗಣ_ಹುಡುಕು_ಹೊಂದಿಕೆ(&curwin->w_cursor);
	validate_cursor();
	ಕೊನೆ_pos = curwin->w_cursor;
	is_state->ಹೊಂದಿಕೆ_ಕೊನೆ = ಕೊನೆ_pos;
	curwin->w_cursor = save_pos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    ಆದರೆ (!use_ಕೊನೆಯ_pat)
    {
	ಮುಂದಿನ_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	ಆದರೆ (ಖಾಲಿಗಳು_ನಮೂನೆ(ccline.cmdbuff + skiplen, (size_t)patlen, ಹುಡುಕು_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    ಗಣ_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = ಮುಂದಿನ_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    ಆದರೆ (p_ru && curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ > 0)
	curwin->w_redr_ಸ್ಥಿತಿ = true;

    update_screen(UPD_SOME_VALID);
    highlight_ಹೊಂದಿಕೆ = FALSE;
    restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    ಆದರೆ (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->ಹುಡುಕು_ಆರಂಭ_2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಿಕ್ಕಿತು != 0)
	curwin->w_cursor = ಕೊನೆ_pos;

    ಸಂದೇಶ_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_adjust_incsearch_highlighting(
	ಪೂರ್ಣಾಂಕ			firstc,
	ದೀರ್ಘ			ಎಣಿಕೆ_2,
	incsearch_state_T	*is_state,
	ಪೂರ್ಣಾಂಕ			c)
{
    ಪೂರ್ಣಾಂಕ		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = ಖಾಲಿ;
    char_u	*searchstr;
    char_u	*strcopy = ಖಾಲಿ;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_flags = ಹುಡುಕು_NOOF;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();

    ಆದರೆ (!do_incsearch_highlighting(firstc, &ಹುಡುಕು_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (ಶೂನ್ಯ)parse_ಹುಡುಕು_ನಮೂನೆ_ಸ್ಥಾನಾಂತರ(&pat, &patlen_s, ಹುಡುಕು_delim,
				    ಹುಡುಕು_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &ಸ್ಥಾನಾಂತರ);

    cursor_off();
    ಹೊರಹೋಗು_flush();
    ಆದರೆ (c == Ctrl_G)
    {
	t = is_state->ಹೊಂದಿಕೆ_ಕೊನೆ;
	ಆದರೆ (LT_POS(is_state->ಹೊಂದಿಕೆ_ಆರಂಭ, is_state->ಹೊಂದಿಕೆ_ಕೊನೆ))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (ಶೂನ್ಯ)decl(&t);
	ಹುಡುಕು_flags += ಹುಡುಕು_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	t = is_state->ಹೊಂದಿಕೆ_ಆರಂಭ;
    ಆದರೆ (!p_hls)
	ಹುಡುಕು_flags += ಹುಡುಕು_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, ಖಾಲಿ,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, ಎಣಿಕೆ_2, ಹುಡುಕು_flags, RE_ಹುಡುಕು, ಖಾಲಿ);
    --emsg_off;
    ಆದರೆ (dircp != ಖಾಲಿ)
	*dircp = ಹುಡುಕು_delim;
    ಆದರೆ (i)
    {
	pos_T	ಹೊಂದಿಕೆ_ಆರಂಭ = is_state->ಹೊಂದಿಕೆ_ಆರಂಭ;
	pos_T	ಹೊಂದಿಕೆ_ಕೊನೆ = is_state->ಹೊಂದಿಕೆ_ಕೊನೆ;
	ದೀರ್ಘ	off = ಸ್ಥಾನಾಂತರ.off;

	is_state->ಹುಡುಕು_ಆರಂಭ_2 = ಹೊಂದಿಕೆ_ಆರಂಭ;
	ಆದರೆ (!ಸ್ಥಾನಾಂತರ.ಸಾಲು && (ಸ್ಥಾನಾಂತರ.ಕೊನೆ || off != 0))
	{
	    ಆದರೆ (ಸ್ಥಾನಾಂತರ.ಕೊನೆ)
	    {
		is_state->ಹುಡುಕು_ಆರಂಭ_2 = ಹೊಂದಿಕೆ_ಕೊನೆ;
		(ಶೂನ್ಯ)decl(&is_state->ಹುಡುಕು_ಆರಂಭ_2);
	    }
	    ಆಗ (off > 0)
	    {
		ಆದರೆ (incl(&is_state->ಹುಡುಕು_ಆರಂಭ_2) == -1)
		    ನಿಲ್ಲಿಸು;
		--off;
	    }
	    ಆಗ (off < 0)
	    {
		ಆದರೆ (decl(&is_state->ಹುಡುಕು_ಆರಂಭ_2) == -1)
		    ನಿಲ್ಲಿಸು;
		++off;
	    }
	}
	is_state->ಹೊಂದಿಕೆ_ಕೊನೆ = t;
	is_state->ಹೊಂದಿಕೆ_ಆರಂಭ = t;
	ಆದರೆ (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ಹುಡುಕು_ಆರಂಭ_2 = t;
	    (ಶೂನ್ಯ)decl(&is_state->ಹುಡುಕು_ಆರಂಭ_2);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ಹುಡುಕು_ಆರಂಭ_2 = t;
	    (ಶೂನ್ಯ)incl(&is_state->ಹುಡುಕು_ಆರಂಭ_2);
	}
	ಆದರೆ (LT_POS(t, is_state->ಹುಡುಕು_ಆರಂಭ_2) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->ಹುಡುಕು_ಆರಂಭ_2 = t;
	    ಆದರೆ (firstc == '?')
		(ಶೂನ್ಯ)incl(&is_state->ಹುಡುಕು_ಆರಂಭ_2);
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)decl(&is_state->ಹುಡುಕು_ಆರಂಭ_2);
	}

	ಗಣ_ಹುಡುಕು_ಹೊಂದಿಕೆ(&is_state->ಹೊಂದಿಕೆ_ಕೊನೆ);
	curwin->w_cursor = is_state->ಹೊಂದಿಕೆ_ಆರಂಭ;
	ಬದಲಾಯಿತು_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_ಹೊಂದಿಕೆ = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_ಹೊಂದಿಕೆ = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->ಹೊಂದಿಕೆ_ಕೊನೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_ಕೂಡಿಸು_char_ಗಮ್ಯ_ಹುಡುಕು(ಪೂರ್ಣಾಂಕ firstc, ಪೂರ್ಣಾಂಕ *c, incsearch_state_T *is_state)
{
    ಪೂರ್ಣಾಂಕ		skiplen, patlen, ಹುಡುಕು_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();

    ಆದರೆ (!do_incsearch_highlighting(firstc, &ಹುಡುಕು_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    restore_ಕೊನೆಯ_ಹುಡುಕು_ನಮೂನೆ();

    // Add a character from under the cursor for 'incsearch'.
    ಆದರೆ (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->ಹೊಂದಿಕೆ_ಕೊನೆ;
	*c = gchar_cursor();
	ಆದರೆ (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    ಆದರೆ (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    ಆದರೆ (*c == ಹುಡುಕು_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != ಖಾಲಿ)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    ಆದರೆ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		ಪೂರ್ಣಾಂಕ save_c = *c;

		ಆಗ (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.ಲಂಬಸಾಲು += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_has_arabic(ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	    j;
    ಪೂರ್ಣಾಂಕ	    mb_l;
    ಪೂರ್ಣಾಂಕ	    u8c;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    u8cc[MAX_MCO];

    ಆದರೆ (!enc_utf8)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (j = ಆರಂಭ; j < ಆರಂಭ + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, ಆರಂಭ + len - j);
	mb_l = utfc_ptr2len_len(p, ಆರಂಭ + len - j);
	ಆದರೆ (ARABIC_CHAR(u8c))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
cmdline_init(ಶೂನ್ಯ)
{
    CLEAR_ಕ್ಷೇತ್ರ(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_handle_ctrl_bsl(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ *gotesc)
{
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    c = plain_vgetc();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    ಆದರೆ (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && ಕೀಲಿ_Typed)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		|| cmdline_ನಕ್ಷತ್ರ > 0
#ಆದರೆ_ಅಂತ್ಯ
		))
    {
	vungetc(c);
	ಹಿಂತಿರುಗಿ PROCESS_ಮುಂದಿನ_ಕೀಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	ಆದರೆ (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	ಇಲ್ಲದಿದ್ದರೆ
	    new_cmdpos = ccline.cmdpos;

	c = get_ಅಭಿವ್ಯಕ್ತಿ_ರಿಜಿಸ್ಟರ್();
	ಆದರೆ (c == '=')
	{
	    char_u	*p = ಖಾಲಿ;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು();
	    --textlock;

	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

		ಆದರೆ (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    ಆದರೆ (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    ಇಲ್ಲದಿದ್ದರೆ
			ccline.cmdpos = new_cmdpos;

		    ಕೀಲಿ_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    ಹಿಂತಿರುಗಿ CMDLINE_ಬದಲಾಯಿತು;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	ಹಿಂತಿರುಗಿ CMDLINE_NOT_ಬದಲಾಯಿತು;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    ಹಿಂತಿರುಗಿ GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_wildchar_complete(
	ಪೂರ್ಣಾಂಕ		c,
	ಪೂರ್ಣಾಂಕ		escape,
	ಪೂರ್ಣಾಂಕ		*did_wild_list,
	ಪೂರ್ಣಾಂಕ		*wim_ಸೂಚ್ಯಂಕ_p,
	expand_T	*xp,
	ಪೂರ್ಣಾಂಕ		*gotesc,
	ಪೂರ್ಣಾಂಕ		redraw_if_menu_ಖಾಲಿಗಳು,
	pos_T		*pre_incsearch_pos)
{
    ಪೂರ್ಣಾಂಕ		wim_ಸೂಚ್ಯಂಕ = *wim_ಸೂಚ್ಯಂಕ_p;
    ಪೂರ್ಣಾಂಕ		res;
    ಪೂರ್ಣಾಂಕ		cmdpos_before;
    ಪೂರ್ಣಾಂಕ		options = WILD_NO_BEEP;
    ಪೂರ್ಣಾಂಕ		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    ಪೂರ್ಣಾಂಕ		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    ಆದರೆ (wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    ಆದರೆ (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	ಆದರೆ (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_LIST))
	{
	    (ಶೂನ್ಯ)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_ಸೂಚ್ಯಂಕ] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	ಆದರೆ (wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_ತುಂಬಿದ)
	    res = nextwild(xp, WILD_ಮುಂದಿನ, options, escape);
	ಇಲ್ಲದಿದ್ದರೆ
	    res = OK;	    // don't insert 'wildchar' now
    }
    ಇಲ್ಲದಿದ್ದರೆ		    // typed p_wc first time
    {
	ಪೂರ್ಣಾಂಕ wim_longest = (wim_flags[0] & WIM_LONGEST);
	ಪೂರ್ಣಾಂಕ wim_list = (wim_flags[0] & WIM_LIST);
	ಪೂರ್ಣಾಂಕ wim_ತುಂಬಿದ = (wim_flags[0] & WIM_ತುಂಬಿದ);

	wim_ಸೂಚ್ಯಂಕ = 0;
	ಆದರೆ (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_ನಮೂನೆ;
	    ಆದರೆ (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    ಆದರೆ (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	ಆದರೆ (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (wim_noselect || (wim_list && !wim_ತುಂಬಿದ))
		options |= WILD_NOSELECT;
	    ಆದರೆ (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	ಆದರೆ (redraw_if_menu_ಖಾಲಿಗಳು && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	ಆದರೆ (got_int)
	{
	    (ಶೂನ್ಯ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (ಶೂನ್ಯ)ExpandOne(xp, ಖಾಲಿ, ಖಾಲಿ, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_ಸೂಚ್ಯಂಕ_p = wim_ಸೂಚ್ಯಂಕ;
	    ಹಿಂತಿರುಗಿ CMDLINE_ಬದಲಾಯಿತು;
	}

	// Display matches
	ಆದರೆ (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    ಆದರೆ (wim_longest)
	    {
		ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_longest_ಪೂರ್ವಪ್ರತ್ಯಯ = (ccline.cmdpos != cmdpos_before);
		ಪೂರ್ಣಾಂಕ show_menu = p_wmnu && wim_ತುಂಬಿದ;

		ಆದರೆ (wim_list || show_menu)
		    (ಶೂನ್ಯ)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಸಿಕ್ಕಿತು_longest_ಪೂರ್ವಪ್ರತ್ಯಯ)
		{
		    ಪೂರ್ಣಾಂಕ wim_list_ಮುಂದಿನ = (wim_flags[1] & WIM_LIST);
		    ಪೂರ್ಣಾಂಕ wim_ತುಂಬಿದ_ಮುಂದಿನ = (wim_flags[1] & WIM_ತುಂಬಿದ);
		    ಪೂರ್ಣಾಂಕ wim_noselect_ಮುಂದಿನ = (wim_flags[1] & WIM_NOSELECT);
		    ಪೂರ್ಣಾಂಕ wim_noinsert_ಮುಂದಿನ = (wim_flags[1] & WIM_NOINSERT);
		    ಆದರೆ (wim_list_ಮುಂದಿನ || (p_wmnu && (wim_ತುಂಬಿದ_ಮುಂದಿನ
				    || wim_noselect_ಮುಂದಿನ || wim_noinsert_ಮುಂದಿನ)))
		    {
			ಆದರೆ (wim_ತುಂಬಿದ_ಮುಂದಿನ && !wim_noselect_ಮುಂದಿನ && !wim_noinsert_ಮುಂದಿನ)
			    nextwild(xp, WILD_ಮುಂದಿನ, options, escape);
			ಇಲ್ಲದಿದ್ದರೆ
			    (ಶೂನ್ಯ)showmatches(xp, p_wmnu && (wim_noselect_ಮುಂದಿನ
					|| wim_noinsert_ಮುಂದಿನ), wim_list_ಮುಂದಿನ,
				    p_wmnu ? wim_flags[1] : 0);

			ಆದರೆ (wim_list_ಮುಂದಿನ)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ show_menu = p_wmnu && (wim_ತುಂಬಿದ || wim_noselect
							    || wim_noinsert);

		ಆದರೆ (wim_list || show_menu)
		    (ಶೂನ್ಯ)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    ಆದರೆ (wim_list)
		*did_wild_list = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    ಆದರೆ (wim_ಸೂಚ್ಯಂಕ < 3)
	++wim_ಸೂಚ್ಯಂಕ;
    ಆದರೆ (c == ESC)
	*gotesc = TRUE;

    *wim_ಸೂಚ್ಯಂಕ_p = wim_ಸೂಚ್ಯಂಕ;
    ಹಿಂತಿರುಗಿ (res == OK) ? CMDLINE_ಬದಲಾಯಿತು : CMDLINE_NOT_ಬದಲಾಯಿತು;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_erase_chars(
	ಪೂರ್ಣಾಂಕ c,
	ಪೂರ್ಣಾಂಕ indent
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	, incsearch_state_T *isp
#ಆದರೆ_ಅಂತ್ಯ
	)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;

    ಆದರೆ (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    ಆದರೆ (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    ಆದರೆ (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_ಮುಂದಿನ(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    ಆದರೆ (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	ಆದರೆ (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    ಆದರೆ (c == Ctrl_W)
	    {
		ಆಗ (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		ಆಗ (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		ಆದರೆ (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_W)
	{
	    ಆಗ (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    ಆದರೆ (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		ಆಗ (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    --p;
	ccline.cmdpos = (ಪೂರ್ಣಾಂಕ)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	ಆಗ (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಆದರೆ (ccline.cmdlen == 0)
	{
	    isp->ಹುಡುಕು_ಆರಂಭ_2 = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#ಆದರೆ_ಅಂತ್ಯ
	redrawcmd();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == ಖಾಲಿ && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	ಆದರೆ (exmode_ಸಕ್ರಿಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    ಹಿಂತಿರುಗಿ CMDLINE_NOT_ಬದಲಾಯಿತು;

	dealloc_cmdbuff();	// no commandline to return

	ಆದರೆ (!cmd_silent)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (cmdmsg_rl)
		ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ಸಂದೇಶ_putchar(' ');		// delete ':'
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	isp->ಹುಡುಕು_ಆರಂಭ_2 = isp->save_cursor;
#ಆದರೆ_ಅಂತ್ಯ
	redraw_cmdline = TRUE;
	ಹಿಂತಿರುಗಿ GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
    }
    ಹಿಂತಿರುಗಿ CMDLINE_ಬದಲಾಯಿತು;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_toggle_langmap(ದೀರ್ಘ *b_im_ptr)
{
    ಆದರೆ (map_ಗಮ್ಯ_exists_ಬಹುಲಕ((char_u *)"", ಬಹುಲಕ_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= ಬಹುಲಕ_LANGMAP;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
	im_ಗಣ_ಸಕ್ರಿಯ(FALSE);	// Disable input method
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (b_im_ptr != ಖಾಲಿ)
	{
	    ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    ಇಲ್ಲದಿದ್ದರೆ
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	ಆದರೆ ((p_imdisable && b_im_ptr != ಖಾಲಿ)
		? *b_im_ptr == B_IMODE_IM : im_get_ಸ್ಥಿತಿ())
	{
	    im_ಗಣ_ಸಕ್ರಿಯ(FALSE);	// Disable input method
	    ಆದರೆ (b_im_ptr != ಖಾಲಿ)
		*b_im_ptr = B_IMODE_NONE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    im_ಗಣ_ಸಕ್ರಿಯ(TRUE);	// Enable input method
	    ಆದರೆ (b_im_ptr != ಖಾಲಿ)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (b_im_ptr != ಖಾಲಿ)
    {
	ಆದರೆ (b_im_ptr == &curbuf->b_p_iminsert)
	    ಗಣ_iminsert_global();
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_imsearch_global();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    ಸ್ಥಿತಿ_redraw_curbuf();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_ಸೇರಿಸು_reg(ಪೂರ್ಣಾಂಕ *gotesc ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		literally = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		save_new_cmdpos = new_cmdpos;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    i = c = plain_vgetc();	// CTRL-R <char>
    ಆದರೆ (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    ಆದರೆ (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    ಆದರೆ (c == '=')
    {
	ಆದರೆ (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_ನಕ್ಷತ್ರ > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    c = get_ಅಭಿವ್ಯಕ್ತಿ_ರಿಜಿಸ್ಟರ್();
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
			|| (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && c == '*')
			|| (clip_plus.ಲಭ್ಯ && c == '+')
#ಆದರೆ_ಅಂತ್ಯ
			;
	cmdline_paste(c, literally, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	ಆದರೆ (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    ಹಿಂತಿರುಗಿ GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಕೀಲಿ_Typed = FALSE;	// Don't do p_wc completion.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    ಆದರೆ (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    ಇಲ್ಲದಿದ್ದರೆ
		ccline.cmdpos = new_cmdpos;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#ಆದರೆ_ಅಂತ್ಯ

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    ಹಿಂತಿರುಗಿ literally ? CMDLINE_ಬದಲಾಯಿತು : CMDLINE_NOT_ಬದಲಾಯಿತು;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_ಎಡ_ಬಲ_mouse(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ *ignore_drag_release)
{
    ಆದರೆ (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	*ignore_drag_release = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!mouse_has(MOUSE_COMMAND))
	    ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು < cmdline_ಅಡ್ಡಸಾಲು && (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ || clip_plus.ಲಭ್ಯ))
    {
	ಪೂರ್ಣಾಂಕ	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	ಆದರೆ (mouse_model_popup() && button == MOUSE_ಎಡ
		&& (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_ಬಲ;
	    mod_ಮುಖವಾಡ &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
	}
	clip_modeless(button, is_click, is_drag);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_cmdspos();
    ಗಾಗಿ (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	ಪೂರ್ಣಾಂಕ	i;

	i = cmdline_charsize(ccline.cmdpos);
	ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು <= cmdline_ಅಡ್ಡಸಾಲು + ccline.cmdspos / cmdline_ಅಗಲ
		&& mouse_ಲಂಬಸಾಲು < ccline.cmdspos % cmdline_ಅಗಲ + i)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (has_mbyte)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_browse_history(
	ಪೂರ್ಣಾಂಕ	c,
	ಪೂರ್ಣಾಂಕ	firstc,
	char_u	**curcmdstr,
	size_t	*curcmdstrlen,
	ಪೂರ್ಣಾಂಕ	histype,
	ಪೂರ್ಣಾಂಕ	*hiscnt_p,
	expand_T *xp)
{
    ಪೂರ್ಣಾಂಕ		orig_hiscnt;
    ಪೂರ್ಣಾಂಕ		hiscnt = orig_hiscnt = *hiscnt_p;
    char_u	*lookfor = *curcmdstr;
    size_t	lookforlen = *curcmdstrlen;
    ಪೂರ್ಣಾಂಕ		res;

    ಆದರೆ (get_hislen() == 0 || firstc == NUL)	// no history
	ಹಿಂತಿರುಗಿ CMDLINE_NOT_ಬದಲಾಯಿತು;

    // save current command string so it can be restored later
    ಆದರೆ (lookfor == ಖಾಲಿ)
    {
	ಆದರೆ ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ CMDLINE_NOT_ಬದಲಾಯಿತು;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    ಗಾಗಿ (;;)
    {
	// one step backwards
	ಆದರೆ (c == K_ಮೇಲೆ || c == K_S_ಮೇಲೆ || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    ಆದರೆ (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    ಇಲ್ಲದಿದ್ದರೆ			// at top of list
	    {
		hiscnt = orig_hiscnt;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ    // one step forwards
	{
	    // on last entry, clear the line
	    ಆದರೆ (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		ನಿಲ್ಲಿಸು;
	    }

	    // not on a history line, nothing to do
	    ಆದರೆ (hiscnt == get_hislen())
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		++hiscnt;
	}
	ಆದರೆ (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== ಖಾಲಿ)
	{
	    hiscnt = orig_hiscnt;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ ((c != K_ಮೇಲೆ && c != K_ಕೆಳಕ್ಕೆ)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	ಪೂರ್ಣಾಂಕ	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	ಆದರೆ (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	ಆದರೆ (histype == HIST_ಹುಡುಕು
		&& p != lookfor
		&& (old_firstc = p[plen + 1]) != firstc)
	{
	    ಪೂರ್ಣಾಂಕ     i;
	    ಪೂರ್ಣಾಂಕ     j;
	    size_t  len;

	    // Correct for the separator character used when
	    // adding the history entry vs the one used now.
	    // First loop: count length.
	    // Second loop: copy the characters.
	    ಗಾಗಿ (i = 0; i <= 1; ++i)
	    {
		len = 0;
		ಗಾಗಿ (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    ಆದರೆ (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			ಆದರೆ (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Escape new sep, unless it is already
			// escaped.
			ಆದರೆ (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    ಆದರೆ (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			ಆದರೆ (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		ಆದರೆ (i == 0)
		{
		    alloc_cmdbuff(len);
		    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)
		    {
			res = GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
			ಹೋಗು ಮುಗಿದ;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (ಪೂರ್ಣಾಂಕ)len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    alloc_cmdbuff(plen);
	    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)
	    {
		res = GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
		ಹೋಗು ಮುಗಿದ;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (ಪೂರ್ಣಾಂಕ)plen;
	}

	redrawcmd();
	res = CMDLINE_ಬದಲಾಯಿತು;
	ಹೋಗು ಮುಗಿದ;
    }
    beep_flush();
    res = CMDLINE_NOT_ಬದಲಾಯಿತು;

ಮುಗಿದ:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Initialize the current command-line info.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_ccline(ಪೂರ್ಣಾಂಕ firstc, ಪೂರ್ಣಾಂಕ indent)
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
    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)
    {
	ccline.cmdbufflen = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_ಪಠ್ಯ_ಆರಂಭ_cmdline();

    // autoindent for :insert and :append
    ಆದರೆ (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		  firstc,
    ದೀರ್ಘ	  ಎಣಿಕೆ_2,	// only used for incremental search
    ಪೂರ್ಣಾಂಕ		  indent,	// indent for inside conditionals
    getline_opt_T do_concat ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ getcmdline_int(firstc, ಎಣಿಕೆ_2, indent, TRUE);
}

    ಸ್ಥಿರ char_u *
getcmdline_int(
    ಪೂರ್ಣಾಂಕ		firstc,
    ದೀರ್ಘ	ಎಣಿಕೆ_2 ಬಳಸದ,	// only used for incremental search
    ಪೂರ್ಣಾಂಕ		indent,		// indent for inside conditionals
    ಪೂರ್ಣಾಂಕ		clear_ccline)	// clear ccline first
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಆಳ = 0;	    // call depth
    ಪೂರ್ಣಾಂಕ		c = 0;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		gotesc = FALSE;		// TRUE when <ESC> just typed
    ಪೂರ್ಣಾಂಕ		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = ಖಾಲಿ;	// string to match
    size_t	lookforlen = 0;
    ಪೂರ್ಣಾಂಕ		hiscnt;			// current history line in use
    ಪೂರ್ಣಾಂಕ		histype;		// history type to be used
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    incsearch_state_T	is_state;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		did_wild_list = FALSE;	// did wild_list() recently
    ಪೂರ್ಣಾಂಕ		wim_ಸೂಚ್ಯಂಕ = 0;		// index in wim_flags[]
    ಪೂರ್ಣಾಂಕ		res;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		save_State = State;	// remember State when called
    ಪೂರ್ಣಾಂಕ		some_ಕೀಲಿ_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    ಪೂರ್ಣಾಂಕ		ignore_drag_release = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		break_ctrl_c = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    expand_T	xpc;
    ದೀರ್ಘ	*b_im_ptr = ಖಾಲಿ;
    buf_T	*b_im_ptr_buf = ಖಾಲಿ;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    ಪೂರ್ಣಾಂಕ		did_save_ccline = FALSE;
    ಪೂರ್ಣಾಂಕ		cmdline_type;
    ಪೂರ್ಣಾಂಕ		wild_type = 0;
    ಪೂರ್ಣಾಂಕ		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++ಆಳ;

    ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    ಆದರೆ (clear_ccline)
	CLEAR_ಕ್ಷೇತ್ರ(ccline);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    ಆದರೆ (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    init_incsearch_state(&is_state);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (init_ccline(firstc, indent) != OK)
	ಹೋಗು theend;	// out of memory

    ಆದರೆ (ಆಳ == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	ಹೋಗು theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	cmdmsg_rl = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    redir_off = TRUE;		// don't redirect the typed command
    ಆದರೆ (!cmd_silent)
    {
	i = ಸಂದೇಶ_scrolled;
	ಸಂದೇಶ_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	ಸಂದೇಶ_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	ಗಣ_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
    xpc.xp_shell = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (ccline.ಒಳಹರಿವು_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_ನಮೂನೆ = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    ಸಂದೇಶ_scroll = FALSE;

    State = ಬಹುಲಕ_CMDLINE;

    ಆದರೆ (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	ಆದರೆ (curbuf->b_p_imsearch == B_IMODE_USE_ಸೇರಿಸು)
	    b_im_ptr = &curbuf->b_p_iminsert;
	ಇಲ್ಲದಿದ್ದರೆ
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	ಆದರೆ (*b_im_ptr == B_IMODE_LMAP)
	    State |= ಬಹುಲಕ_LANGMAP;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
	im_ಗಣ_ಸಕ್ರಿಯ(*b_im_ptr == B_IMODE_IM);
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_imcmdline)
	im_ಗಣ_ಸಕ್ರಿಯ(TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (!debug_ಬಹುಲಕ)
	may_trigger_modechanged();
#ಆದರೆ_ಅಂತ್ಯ

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#ಆದರೆ_ಅಂತ್ಯ

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    ಆದರೆ (did_emsg)
	redrawcmd();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    ಆದರೆ (!cmd_silent && ಸಂದೇಶ_scrolled == 0)
    {
	ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_ಸ್ಥಿತಿ = true;
		ಸಿಕ್ಕಿತು_one = TRUE;
	    }

	ಆದರೆ (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    ಸಿಕ್ಕಿತು_one = TRUE;
	}

	ಆದರೆ (ಸಿಕ್ಕಿತು_one)
	    redraw_statuslines();
    }
#ಆದರೆ_ಅಂತ್ಯ

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	trigger_cmdlinechanged = TRUE;
	ಪೂರ್ಣಾಂಕ	ಕೊನೆ_wildmenu;
	ಪೂರ್ಣಾಂಕ	prev_cmdpos = ccline.cmdpos;
	ಪೂರ್ಣಾಂಕ	skip_pum_redraw = FALSE;

	VIM_CLEAR(prev_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
	quit_ಇನ್ನಷ್ಟು = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy == 0 && stuff_ಖಾಲಿಗಳು() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_ಕೀಲಿ_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    ಆದರೆ (prev_cmdbuff == ಖಾಲಿ)
		ಹೋಗು returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	ಆದರೆ (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	ಮಾಡು
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = ಸುರಕ್ಷಿತ_vgetc();
	} ಆಗ (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	ಆದರೆ (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
		cmdline_pum_remove(&ccline, FALSE);
	    (ಶೂನ್ಯ)ExpandOne(&xpc, ಖಾಲಿ, ಖಾಲಿ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_ಸೂಚ್ಯಂಕ = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	ಆದರೆ (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    ಪೂರ್ಣಾಂಕ	    clen = ccline.cmdlen;
	    ಪೂರ್ಣಾಂಕ	    cc_ಎಣಿಕೆ = aucmd_cmdline_ಬದಲಾಯಿತು_ಎಣಿಕೆ;

	    ಆದರೆ (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		ಆದರೆ (clen == ccline.cmdlen
				    || cc_ಎಣಿಕೆ != aucmd_cmdline_ಬದಲಾಯಿತು_ಎಣಿಕೆ)
		    trigger_cmdlinechanged = FALSE;
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;
	    }
	}

	ಆದರೆ (ಕೀಲಿ_Typed)
	{
	    some_ಕೀಲಿ_typed = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (cmd_hkmap)
		c = hkmap(c);
	    ಆದರೆ (cmdmsg_rl && !ಕೀಲಿ_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		ಆಯ್ಕೆ (c)
		{
		    ಸ್ಥಿತಿ K_ಬಲ:   c = K_ಎಡ; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ K_S_ಬಲ: c = K_S_ಎಡ; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ K_C_ಬಲ: c = K_C_ಎಡ; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ K_ಎಡ:    c = K_ಬಲ; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ K_S_ಎಡ:  c = K_S_ಬಲ; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ K_C_ಎಡ:  c = K_C_ಬಲ; ನಿಲ್ಲಿಸು;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	ಆದರೆ ((c == Ctrl_C
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		|| c == intr_char
#ಆದರೆ_ಅಂತ್ಯ
				)
#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_ಸಕ್ರಿಯ)
#ಆದರೆ_ಅಂತ್ಯ
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	ಆದರೆ (lookfor != ಖಾಲಿ
		&& c != K_S_ಕೆಳಕ್ಕೆ && c != K_S_ಮೇಲೆ
		&& c != K_ಕೆಳಕ್ಕೆ && c != K_ಮೇಲೆ
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_ಎಡ && c != K_ಬಲ
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	ಆದರೆ (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	ಆದರೆ (p_wmnu)
	    c = wildmenu_translate_ಕೀಲಿ(&ccline, c, &xpc, did_wild_list);

	ಪೂರ್ಣಾಂಕ ಕೀಲಿ_is_wc = (c == p_wc && ಕೀಲಿ_Typed) || c == p_wcm;
	ಆದರೆ ((cmdline_pum_ಸಕ್ರಿಯ() || wild_menu_showing || did_wild_list)
		&& !ಕೀಲಿ_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    ಆದರೆ (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		ಆದರೆ (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	ಆದರೆ (ಕೀಲಿ_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    || c == intr_char
#ಆದರೆ_ಅಂತ್ಯ
		    || c == Ctrl_C))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಗಣ_vim_var_char(c);  // Set v:char
#ಆದರೆ_ಅಂತ್ಯ
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
	    ಆದರೆ ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		ಗಣ_no_hlsearch(TRUE);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	ಕೊನೆ_wildmenu = (!ಕೀಲಿ_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	ಕೊನೆ_wildmenu = ಕೊನೆ_wildmenu && (!cmdline_pum_ಸಕ್ರಿಯ() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_ಮೇಲೆ || c == K_S_ಕೆಳಕ್ಕೆ)
				 && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL))));

	// free expanded names when finished walking through matches
	ಆದರೆ (ಕೊನೆ_wildmenu)
	{
	    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
	    {
		skip_pum_redraw = skip_pum_redraw && !ಕೀಲಿ_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    ಆದರೆ (xpc.xp_numfiles != -1)
		(ಶೂನ್ಯ)ExpandOne(&xpc, ಖಾಲಿ, ಖಾಲಿ, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    ಆದರೆ (!p_wmnu || (c != K_ಮೇಲೆ && c != K_ಕೆಳಕ್ಕೆ))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_ಸೂಚ್ಯಂಕ = 0;
	    wildmenu_cleanup(&ccline);
	}

	ಆದರೆ (p_wmnu)
	    c = wildmenu_process_ಕೀಲಿ(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	ಆದರೆ (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    ಆದರೆ (res == CMDLINE_ಬದಲಾಯಿತು)
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == CMDLINE_NOT_ಬದಲಾಯಿತು)
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
		ಹೋಗು returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	ಆದರೆ (c == cedit_ಕೀಲಿ || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    ಆದರೆ ((c == K_CMDWIN || ex_ಸಾಮಾನ್ಯ_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_ಕೀಲಿ_typed = TRUE;
	    }
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	ಇಲ್ಲದಿದ್ದರೆ
	    c = do_digraph(c);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!ಕೀಲಿ_Typed || vim_strchr(p_cpo, CPO_ESC) != ಖಾಲಿ)))
	{
	    // In Ex mode a backslash escapes a newline.
	    ಆದರೆ (exmode_ಸಕ್ರಿಯ
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		ಆದರೆ (c == K_KENTER)
		    c = '\n';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		ಆದರೆ (ccheck_abbr(c + ABBR_OFF))
		    ಹೋಗು cmdline_ಬದಲಾಯಿತು;
		ಆದರೆ (!cmd_silent)
		{
		    windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off);
		    ಹೊರಹೋಗು_flush();
		}
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	ಆದರೆ ((c == p_wc && !gotesc && ಕೀಲಿ_Typed) || c == p_wcm || c == K_WILD)
	{
	    ಆದರೆ (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_ಸೂಚ್ಯಂಕ, &xpc, &gotesc, c == K_WILD,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		    &is_state.ಹುಡುಕು_ಆರಂಭ_2
#ಇಲ್ಲದಿದ್ದರೆ
		    ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
		    );
	    ಆದರೆ (c == K_WILD)
		--emsg_silent;
	    ಆದರೆ (res == CMDLINE_ಬದಲಾಯಿತು)
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;
	    ಆದರೆ (c == K_WILD)
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	ಆದರೆ (c == K_S_TAB && ಕೀಲಿ_Typed)
	{
	    ಆದರೆ (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		ಆದರೆ (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    ಪೂರ್ಣಾಂಕ list = wim_flags[wim_ಸೂಚ್ಯಂಕ] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		ಆದರೆ (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    ಹೋಗು cmdline_ಬದಲಾಯಿತು;
	    }
	}

	ಆದರೆ (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	ಆದರೆ (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	    // Apply search highlighting
	    ಆದರೆ (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    ಆದರೆ (ಕೀಲಿ_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, ಎಣಿಕೆ_2, &is_state);
#ಆದರೆ_ಅಂತ್ಯ
	    wild_type = 0;
	    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
	}

	/*
	 * Big switch for a typed command line character.
	 */
	ಆಯ್ಕೆ (c)
	{
	ಸ್ಥಿತಿ K_BS:
	ಸ್ಥಿತಿ Ctrl_H:
	ಸ್ಥಿತಿ K_DEL:
	ಸ್ಥಿತಿ K_KDEL:
	ಸ್ಥಿತಿ Ctrl_W:
	    res = cmdline_erase_chars(c, indent
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		    , &is_state
#ಆದರೆ_ಅಂತ್ಯ
		    );
	    ಆದರೆ (res == CMDLINE_NOT_ಬದಲಾಯಿತು)
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
		ಹೋಗು returncmd;		// back to cmd mode
	    ಹೋಗು cmdline_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ K_INS:
	ಸ್ಥಿತಿ K_KINS:
		ccline.overstrike = !ccline.overstrike;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
		may_trigger_modechanged();
		ಸ್ಥಿತಿ_redraw_curbuf();
		redraw_statuslines();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : ಖಾಲಿ);
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

//	case '@':   only in very old vi
	ಸ್ಥಿತಿ Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		ಆಗ (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		ಆದರೆ (ccline.cmdlen == 0)
		    is_state.ಹುಡುಕು_ಆರಂಭ_2 = is_state.save_cursor;
#ಆದರೆ_ಅಂತ್ಯ
		redrawcmd();
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಸ್ಥಿತಿ Ctrl_Y:
		// Copy the modeless selection, if there is one.
		ಆದರೆ (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED)
		{
		    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_ಮುಗಿದ)
			clip_copy_modeless_selection(TRUE);
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		}
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ ESC:	// get here if p_wc != ESC or when ESC typed twice
	ಸ್ಥಿತಿ Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		ಆದರೆ (exmode_ಸಕ್ರಿಯ
			       && (ex_ಸಾಮಾನ್ಯ_busy == 0 || typebuf.tb_len > 0))
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		ಹೋಗು returncmd;		// back to cmd mode

	ಸ್ಥಿತಿ Ctrl_R:			// insert register
		res = cmdline_ಸೇರಿಸು_reg(&gotesc);
		ಆದರೆ (res == GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
		    ಹೋಗು returncmd;
		ಆದರೆ (res == CMDLINE_ಬದಲಾಯಿತು)
		    ಹೋಗು cmdline_ಬದಲಾಯಿತು;
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_D:
		ಆದರೆ (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    ನಿಲ್ಲಿಸು;	// Use ^D as normal char instead

		redrawcmd();
		ಮುಂದುವರಿ;	// don't do incremental search now

	ಸ್ಥಿತಿ K_ಬಲ:
	ಸ್ಥಿತಿ K_S_ಬಲ:
	ಸ್ಥಿತಿ K_C_ಬಲ:
		ಮಾಡು
		{
		    ಆದರೆ (ccline.cmdpos >= ccline.cmdlen)
			ನಿಲ್ಲಿಸು;
		    i = cmdline_charsize(ccline.cmdpos);
		    ಆದರೆ (ಕೀಲಿ_Typed && ccline.cmdspos + i >= cmdline_ಅಗಲ * ಅಡ್ಡಸಾಲುಗಳು)
			ನಿಲ್ಲಿಸು;
		    ccline.cmdspos += i;
		    ಆದರೆ (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    ಇಲ್ಲದಿದ್ದರೆ
			++ccline.cmdpos;
		}
		ಆಗ ((c == K_S_ಬಲ || c == K_C_ಬಲ
			       || (mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		ಆದರೆ (has_mbyte)
		    ಗಣ_cmdspos_cursor();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ K_ಎಡ:
	ಸ್ಥಿತಿ K_S_ಎಡ:
	ಸ್ಥಿತಿ K_C_ಎಡ:
		ಆದರೆ (ccline.cmdpos == 0)
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		ಮಾಡು
		{
		    --ccline.cmdpos;
		    ಆದರೆ (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_ತಲೆ_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		ಆಗ (ccline.cmdpos > 0
			&& (c == K_S_ಎಡ || c == K_C_ಎಡ
			       || (mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		ಆದರೆ (has_mbyte)
		    ಗಣ_cmdspos_cursor();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ಸ್ಥಿತಿ K_F4:
	    ಆದರೆ (mod_ಮುಖವಾಡ == MOD_ಮುಖವಾಡ_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
	    }
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_MIDDLEDRAG:
	ಸ್ಥಿತಿ K_MIDDLERELEASE:
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;	// Ignore mouse

	ಸ್ಥಿತಿ K_MIDDLEMOUSE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (!mouse_has(MOUSE_COMMAND))
			ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;   // Ignore mouse
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
		ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
		    cmdline_paste('*', TRUE, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	ಸ್ಥಿತಿ K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_LEFTDRAG:
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_RIGHTDRAG:
	ಸ್ಥಿತಿ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ಆದರೆ (ignore_drag_release)
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		// FALLTHROUGH
	ಸ್ಥಿತಿ K_LEFTMOUSE:
	ಸ್ಥಿತಿ K_RIGHTMOUSE:
		cmdline_ಎಡ_ಬಲ_mouse(c, &ignore_drag_release);
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	ಸ್ಥಿತಿ K_MOUSEDOWN:
	ಸ್ಥಿತಿ K_MOUSEUP:
	ಸ್ಥಿತಿ K_MOUSELEFT:
	ಸ್ಥಿತಿ K_MOUSERIGHT:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_ಕೆಳಕ್ಕೆ
				      : c == K_MOUSEUP ? MSCR_ಮೇಲೆ
				      : c == K_MOUSELEFT ? MSCR_ಎಡ
				      : MSCR_ಬಲ);
#ಆದರೆ_ಅಂತ್ಯ
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	// Alternate buttons ignored here
	ಸ್ಥಿತಿ K_X1MOUSE:
	ಸ್ಥಿತಿ K_X1DRAG:
	ಸ್ಥಿತಿ K_X1RELEASE:
	ಸ್ಥಿತಿ K_X2MOUSE:
	ಸ್ಥಿತಿ K_X2DRAG:
	ಸ್ಥಿತಿ K_X2RELEASE:
	ಸ್ಥಿತಿ K_MOUSEMOVE:
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಸ್ಥಿತಿ K_LEFTMOUSE_NM:	// mousefocus click, ignored
	ಸ್ಥಿತಿ K_LEFTRELEASE_NM:
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ K_VER_SCROLLBAR:
		ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ K_HOR_SCROLLBAR:
		ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_ಮೌಲ್ಯ);
		    redrawcmd();
		}
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	ಸ್ಥಿತಿ K_TABLINE:
	ಸ್ಥಿತಿ K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		ಆದರೆ (gui_use_tabline())
		    gui_mch_ಗಣ_curtab(tabpage_ಸೂಚ್ಯಂಕ(curtab));
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_SELECT:	    // end of Select mode mapping - ignore
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_B:	    // begin of command line
	ಸ್ಥಿತಿ K_HOME:
	ಸ್ಥಿತಿ K_KHOME:
	ಸ್ಥಿತಿ K_S_HOME:
	ಸ್ಥಿತಿ K_C_HOME:
		ccline.cmdpos = 0;
		ಗಣ_cmdspos();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_E:	    // end of command line
	ಸ್ಥಿತಿ K_ಕೊನೆ:
	ಸ್ಥಿತಿ K_KEND:
	ಸ್ಥಿತಿ K_S_ಕೊನೆ:
	ಸ್ಥಿತಿ K_C_ಕೊನೆ:
		ccline.cmdpos = ccline.cmdlen;
		ಗಣ_cmdspos_cursor();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_A:	    // all matches
		ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		ಆದರೆ (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    ನಿಲ್ಲಿಸು;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_L:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		ಆದರೆ (may_ಕೂಡಿಸು_char_ಗಮ್ಯ_ಹುಡುಕು(firstc, &c, &is_state) == OK)
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ

		// completion: longest common part
		ಆದರೆ (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    ನಿಲ್ಲಿಸು;
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;

	ಸ್ಥಿತಿ Ctrl_N:	    // next match
	ಸ್ಥಿತಿ Ctrl_P:	    // previous match
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ() && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))
		{
		    ಆದರೆ (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_ಮುಂದಿನ;
		    ಆದರೆ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ನಿಲ್ಲಿಸು;
		    ಹೋಗು cmdline_ಬದಲಾಯಿತು;
		}
		// FALLTHROUGH
	ಸ್ಥಿತಿ K_ಮೇಲೆ:
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_S_ಮೇಲೆ:
	ಸ್ಥಿತಿ K_S_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_PAGEUP:
	ಸ್ಥಿತಿ K_KPAGEUP:
	ಸ್ಥಿತಿ K_PAGEDOWN:
	ಸ್ಥಿತಿ K_KPAGEDOWN:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ()
			&& ((c == K_S_ಮೇಲೆ || c == K_S_ಕೆಳಕ್ಕೆ)
			    ? (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
				&& (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))))
		{
		    ಪೂರ್ಣಾಂಕ	ಮೇಲೆ = c == K_S_ಮೇಲೆ || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    ಆದರೆ (popup_scroll_info(ಮೇಲೆ ? -1 : 1,
					     c != K_S_ಮೇಲೆ && c != K_S_ಕೆಳಕ್ಕೆ))
			cmdline_pum_display();
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    ಆದರೆ (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    ಆದರೆ (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			ನಿಲ್ಲಿಸು;
		    ಹೋಗು cmdline_ಬದಲಾಯಿತು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    ಆದರೆ (res == CMDLINE_ಬದಲಾಯಿತು)
		    {
			did_hist_navigate = TRUE;
			ಹೋಗು cmdline_ಬದಲಾಯಿತು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == GOTO_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
			ಹೋಗು returncmd;
		}
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಸ್ಥಿತಿ Ctrl_G:	    // next match
	ಸ್ಥಿತಿ Ctrl_T:	    // previous match
		ಆದರೆ (may_adjust_incsearch_highlighting(
					  firstc, ಎಣಿಕೆ_2, &is_state, c) == FAIL)
		    ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ Ctrl_V:
	ಸ್ಥಿತಿ Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    ಆದರೆ (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			ಸಂದೇಶ_putchar(' ');
			cursorcmd();
		    }
		}

		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	ಸ್ಥಿತಿ Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		ಆದರೆ (c != NUL)
		    ನಿಲ್ಲಿಸು;

		redrawcmd();
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ // FEAT_DIGRAPHS

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಸ್ಥಿತಿ Ctrl__:	    // CTRL-_: switch language mode
		ಆದರೆ (!p_ari)
		    ನಿಲ್ಲಿಸು;
		cmd_hkmap = !cmd_hkmap;
		ಹೋಗು cmdline_not_ಬದಲಾಯಿತು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, ಖಾಲಿ);
		ಹೋಗು cmdline_ಬದಲಾಯಿತು;

	ಪೂರ್ವನಿಯೋಜಿತ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		ಆದರೆ (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    ಹೋಗು returncmd;	// back to Normal mode
		}
#ಆದರೆ_ಅಂತ್ಯ
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		ಆದರೆ (!IS_SPECIAL(c))
		    mod_ಮುಖವಾಡ = 0x0;
		ನಿಲ್ಲಿಸು;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	ಆದರೆ (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    ಹೋಗು cmdline_ಬದಲಾಯಿತು;

	/*
	 * put the character in the command line
	 */
	ಆದರೆ (IS_SPECIAL(c) || mod_ಮುಖವಾಡ != 0)
	    put_on_cmdline(get_special_ಕೀಲಿ_name(c, mod_ಮುಖವಾಡ), -1, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	ಹೋಗು cmdline_ಬದಲಾಯಿತು;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_ಬದಲಾಯಿತು:
	// Trigger CursorMovedC autocommands.
	ಆದರೆ (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಆದರೆ (!is_state.incsearch_postponed)
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ

cmdline_ಬದಲಾಯಿತು:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	// If the window changed incremental search state is not valid.
	ಆದರೆ (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	ಆದರೆ (xpc.xp_context == EXPAND_NOTHING && (ಕೀಲಿ_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, ಎಣಿಕೆ_2, &is_state);
#ಆದರೆ_ಅಂತ್ಯ
	// Trigger CmdlineChanged autocommands.
	ಆದರೆ (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != ಖಾಲಿ &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    ಆದರೆ (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Trigger CursorMovedC autocommands.
	ಆದರೆ (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    ಆದರೆ (vpeekc() == NUL)
		redrawcmd();
#ಆದರೆ_ಅಂತ್ಯ
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    ಆದರೆ (!event_cmdlineleavepre_triggered)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಗಣ_vim_var_char(c);  // Set v:char
#ಆದರೆ_ಅಂತ್ಯ
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_ಸೂಚ್ಯಂಕ = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = ಖಾಲಿ;
    clear_cmdline_orig();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಮುಕ್ತಾಯ_incsearch_highlighting(gotesc, &is_state, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ಆದರೆ (ccline.cmdlen && firstc != NUL
		&& (some_ಕೀಲಿ_typed || histype == HIST_ಹುಡುಕು))
	{
	    ಕೂಡಿಸು_ಗಮ್ಯ_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_ಹುಡುಕು ? firstc : NUL);
	    ಆದರೆ (firstc == ':')
	    {
		vim_free(new_ಕೊನೆಯ_cmdline);
		new_ಕೊನೆಯ_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	ಆದರೆ (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    ಸಂದೇಶ_check();
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    ಆದರೆ (some_ಕೀಲಿ_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_char(c);  // Set v:char
#ಆದರೆ_ಅಂತ್ಯ
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (!debug_ಬಹುಲಕ)
	may_trigger_modechanged();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    ಆದರೆ (b_im_ptr != ಖಾಲಿ && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_ಸ್ಥಿತಿ(b_im_ptr);
    im_ಗಣ_ಸಕ್ರಿಯ(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
    sb_ಪಠ್ಯ_ಕೊನೆ_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--ಆಳ;
	ಆದರೆ (did_save_ccline)
	    restore_cmdline(&save_ccline);
	ಇಲ್ಲದಿದ್ದರೆ
	    ccline.cmdbuff = ಖಾಲಿ;

	vim_free(prev_cmdbuff);
	ಹಿಂತಿರುಗಿ p;
    }
}

#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
/*
 * Get a command line with a prompt.
 * This is prepared to be called recursively from getcmdline() (e.g. by
 * f_input() when evaluating an expression from CTRL-R =).
 * Returns the command line in allocated memory, or NULL.
 */
    char_u *
getcmdline_prompt(
    ಪೂರ್ಣಾಂಕ		firstc,
    char_u	*prompt,	// command line prompt
    ಪೂರ್ಣಾಂಕ		attr,		// attributes for prompt
    ಪೂರ್ಣಾಂಕ		xp_context,	// type of expansion
    char_u	*xp_arg)	// user-defined expansion argument
{
    char_u		*s;
    cmdline_info_T	save_ccline;
    ಪೂರ್ಣಾಂಕ			did_save_ccline = FALSE;
    ಪೂರ್ಣಾಂಕ			ಸಂದೇಶ_ಲಂಬಸಾಲು_save = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ			ಸಂದೇಶ_silent_save = ಸಂದೇಶ_silent;

    ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_ಕ್ಷೇತ್ರ(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.ಒಳಹರಿವು_fn = (firstc == '@');
# endif
    ಸಂದೇಶ_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    ಆದರೆ (did_save_ccline)
	restore_cmdline(&save_ccline);

    ಸಂದೇಶ_silent = ಸಂದೇಶ_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು_save;

    ಹಿಂತಿರುಗಿ s;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    ಪೂರ್ಣಾಂಕ
check_opt_wim(ಶೂನ್ಯ)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		idx = 0;

    ಗಾಗಿ (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    ಗಾಗಿ (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	ಗಾಗಿ (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	ಆದರೆ (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_ತುಂಬಿದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;
	p += i;
	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (*p == ',')
	{
	    ಆದರೆ (idx == 3)
		ಹಿಂತಿರುಗಿ FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    ಆಗ (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    ಗಾಗಿ (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (cmdwin_type != 0)
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    ಶೂನ್ಯ
ಪಠ್ಯ_locked_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    emsg(_(get_ಪಠ್ಯ_locked_ಸಂದೇಶ()));
}

    ಅಕ್ಷರ *
get_ಪಠ್ಯ_locked_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಆದರೆ (cmdwin_type != 0)
	ಹಿಂತಿರುಗಿ e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ;
    ಹಿಂತಿರುಗಿ e_not_allowed_ಗಮ್ಯ_change_ಪಠ್ಯ_or_change_ಕಿಟಕಿ;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_or_buf_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    ಪೂರ್ಣಾಂಕ
curbuf_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf_ಬೀಗ > 0)
    {
	emsg(_(e_not_allowed_ಗಮ್ಯ_edit_another_ಬಫರ್_now));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    ಪೂರ್ಣಾಂಕ
allbuf_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (allbuf_ಬೀಗ > 0)
    {
	emsg(_(e_not_allowed_ಗಮ್ಯ_change_ಬಫರ್_information_now));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_charsize(ಪೂರ್ಣಾಂಕ idx)
{
#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ಆದರೆ (cmdline_ನಕ್ಷತ್ರ > 0)	    // showing '*', always 1 position
	ಹಿಂತಿರುಗಿ 1;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_cmdspos(ಶೂನ್ಯ)
{
    ಆದರೆ (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    ಇಲ್ಲದಿದ್ದರೆ
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_cmdspos_cursor(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i, m, c;

    ಗಣ_cmdspos();
    ಆದರೆ (ಕೀಲಿ_Typed)
    {
	m = cmdline_ಅಗಲ * ಅಡ್ಡಸಾಲುಗಳು;
	ಆದರೆ (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	m = MAXCOL;
    ಗಾಗಿ (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	ಆದರೆ (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	ಆದರೆ ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
correct_cmdspos(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ cells)
{
    ಆದರೆ ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_ಅಗಲ + cells > cmdline_ಅಗಲ)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    ಪೂರ್ಣಾಂಕ		c,		// normally ':', NUL for ":append"
    ಶೂನ್ಯ	*cookie ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    ಆದರೆ (exec_from_reg && vpeekc() == ':')
	(ಶೂನ್ಯ)vgetc();
    ಹಿಂತಿರುಗಿ getcmdline(c, 1L, indent, options);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 * Returns a string in allocated memory or NULL.
 */
    char_u *
getexmodeline(
    ಪೂರ್ಣಾಂಕ		promptc,	// normally ':', NUL for ":append" and '?' for
				// :s prompt
    ಶೂನ್ಯ	*cookie ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		indent,		// indent for inside conditionals
    getline_opt_T options ಬಳಸದ)
{
    garray_T	ಸಾಲು_ga;
    char_u	*pend;
    ಪೂರ್ಣಾಂಕ		startcol = 0;
    ಪೂರ್ಣಾಂಕ		c1 = 0;
    ಪೂರ್ಣಾಂಕ		escaped = FALSE;	// CTRL-V typed
    ಪೂರ್ಣಾಂಕ		vcol = 0;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		prev_char;
    ಪೂರ್ಣಾಂಕ		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    ಆದರೆ ((ಸಂದೇಶ_ಲಂಬಸಾಲು || ಸಂದೇಶ_didout) && promptc != '?')
	ಸಂದೇಶ_putchar('\n');
    ಆದರೆ (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	ಆದರೆ (p_prompt)
	    ಸಂದೇಶ_putchar(':');
	ಆಗ (indent-- > 0)
	    ಸಂದೇಶ_putchar(' ');
	startcol = ಸಂದೇಶ_ಲಂಬಸಾಲು;
    }

    ga_init2(&ಸಾಲು_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    ಆದರೆ (promptc <= 0)
    {
	vcol = indent;
	ಆಗ (indent >= 8)
	{
	    ga_append(&ಸಾಲು_ga, TAB);
	    ಸಂದೇಶ_puts("        ");
	    indent -= 8;
	}
	ಆಗ (indent-- > 0)
	{
	    ga_append(&ಸಾಲು_ga, ' ');
	    ಸಂದೇಶ_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    ಆಗ (!got_int)
    {
	ದೀರ್ಘ    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	ಆದರೆ (ga_grow(&ಸಾಲು_ga, 40) == FAIL)
	    ನಿಲ್ಲಿಸು;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	ಇಲ್ಲದಿದ್ದರೆ
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	ಆದರೆ (got_int)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &ಸಾಲು_ga);
	    ಹೋಗು redraw;
	}

	ಆದರೆ (!escaped)
	{
	    // CR typed means "enter", which is NL
	    ಆದರೆ (c1 == '\r')
		c1 = '\n';

	    ಆದರೆ (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		ಆದರೆ (ಸಾಲು_ga.ga_len > 0)
		{
		    ಆದರೆ (has_mbyte)
		    {
			p = (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
			p[ಸಾಲು_ga.ga_len] = NUL;
			len = (*mb_ತಲೆ_off)(p, p + ಸಾಲು_ga.ga_len - 1) + 1;
			ಸಾಲು_ga.ga_len -= len;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			--ಸಾಲು_ga.ga_len;
		    ಹೋಗು redraw;
		}
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (c1 == Ctrl_U)
	    {
		ಸಂದೇಶ_ಲಂಬಸಾಲು = startcol;
		ಸಂದೇಶ_clr_eos();
		ಸಾಲು_ga.ga_len = 0;
		ಹೋಗು redraw;
	    }

	    ಆದರೆ (c1 == Ctrl_T)
	    {
		sw = get_sw_ಮೌಲ್ಯ(curbuf);
		p = (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
		p[ಸಾಲು_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
ಕೂಡಿಸು_indent:
		ಆಗ (get_indent_str(p, 8, FALSE) < indent)
		{
		    (ಶೂನ್ಯ)ga_grow(&ಸಾಲು_ga, 2);  // one more for the NUL
		    p = (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, ಸಾಲು_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++ಸಾಲು_ga.ga_len;
		}
redraw:
		// redraw the line
		ಸಂದೇಶ_ಲಂಬಸಾಲು = startcol;
		vcol = 0;
		p = (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
		p[ಸಾಲು_ga.ga_len] = NUL;
		ಆಗ (p < (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ + ಸಾಲು_ga.ga_len)
		{
		    ಆದರೆ (*p == TAB)
		    {
			ಮಾಡು
			    ಸಂದೇಶ_putchar(' ');
			ಆಗ (++vcol % 8);
			++p;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			len = mb_ptr2len(p);
			ಸಂದೇಶ_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		ಸಂದೇಶ_clr_eos();
		windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
		ಆದರೆ (prev_char == '0' || prev_char == '^')
		{
		    ಆದರೆ (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--ಸಾಲು_ga.ga_len] = NUL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p[ಸಾಲು_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    ಆದರೆ (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_ಮೌಲ್ಯ(curbuf);
		    }
		}
		ಆಗ (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, ಸಾಲು_ga.ga_len - (s - p) + 1);
		    --ಸಾಲು_ga.ga_len;
		}
		ಹೋಗು ಕೂಡಿಸು_indent;
	    }

	    ಆದರೆ (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		ಮುಂದುವರಿ;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    ಆದರೆ (IS_SPECIAL(c1))
		ಮುಂದುವರಿ;
	}

	ಆದರೆ (IS_SPECIAL(c1))
	    c1 = '?';
	ಆದರೆ (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ + ಸಾಲು_ga.ga_len);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = 1;
	    ((char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ)[ಸಾಲು_ga.ga_len] = c1;
	}
	ಆದರೆ (c1 == '\n')
	    ಸಂದೇಶ_putchar('\n');
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    ಮಾಡು
		ಸಂದೇಶ_putchar(' ');
	    ಆಗ (++vcol % 8);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸಂದೇಶ_outtrans_len(
		     ((char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ) + ಸಾಲು_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	ಸಾಲು_ga.ga_len += len;
	escaped = FALSE;

	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
	pend = (char_u *)(ಸಾಲು_ga.ga_ದತ್ತಾಂಶ) + ಸಾಲು_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	ಆದರೆ (ಸಾಲು_ga.ga_len > 0 && pend[-1] == '\n'
		&& (ಸಾಲು_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --ಸಾಲು_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    ನಿಲ್ಲಿಸು;
	}
    }

    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

    // make following messages go to the next line
    ಸಂದೇಶ_didout = FALSE;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು - 1)
	++ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    emsg_on_display = FALSE;		// don't want ui_delay()

    ಆದರೆ (got_int)
	ga_clear(&ಸಾಲು_ga);

    ಹಿಂತಿರುಗಿ (char_u *)ಸಾಲು_ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_overstrike(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ccline.overstrike;
}

#ಆದರೆ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_at_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ccline.cmdpos >= ccline.cmdlen);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(ಶೂನ್ಯ)
{
    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ || ccline.cmdpos > ccline.cmdlen)
	ಹಿಂತಿರುಗಿ MAXCOL;

    ಆದರೆ (has_mbyte)
    {
	colnr_T	ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ	i = 0;

	ಗಾಗಿ (ಲಂಬಸಾಲು = 0; i < ccline.cmdpos; ++ಲಂಬಸಾಲು)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ccline.cmdpos;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redrawcmd_preedit(ಶೂನ್ಯ)
{
    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE)
	    && xic != ಖಾಲಿ
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	ಪೂರ್ಣಾಂಕ	cmdpos = 0;
	ಪೂರ್ಣಾಂಕ	cmdspos;
	ಪೂರ್ಣಾಂಕ	old_ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ	old_ಲಂಬಸಾಲು;
	colnr_T	ಲಂಬಸಾಲು;

	old_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	old_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	ಆದರೆ (has_mbyte)
	{
	    ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < preedit_ಆರಂಭ_ಲಂಬಸಾಲು
			  && cmdpos < ccline.cmdlen; ++ಲಂಬಸಾಲು)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cmdspos += preedit_ಆರಂಭ_ಲಂಬಸಾಲು;
	    cmdpos  += preedit_ಆರಂಭ_ಲಂಬಸಾಲು;
	}

	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು + (cmdspos / cmdline_ಅಗಲ);
	ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdspos % cmdline_ಅಗಲ;
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;

	ಗಾಗಿ (ಲಂಬಸಾಲು = 0; cmdpos < ccline.cmdlen; ++ಲಂಬಸಾಲು)
	{
	    ಪೂರ್ಣಾಂಕ char_len;
	    ಪೂರ್ಣಾಂಕ char_attr;

	    char_attr = im_get_feedback_attr(ಲಂಬಸಾಲು);
	    ಆದರೆ (char_attr < 0)
		ನಿಲ್ಲಿಸು; // end of preedit string

	    ಆದರೆ (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    ಇಲ್ಲದಿದ್ದರೆ
		char_len = 1;

	    ಸಂದೇಶ_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು;
	ಸಂದೇಶ_ಲಂಬಸಾಲು = old_ಲಂಬಸಾಲು;
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dealloc_cmdbuff(ಶೂನ್ಯ)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    ಆದರೆ (len < 80)
	len = 100;
    ಇಲ್ಲದಿದ್ದರೆ
	len += 20;

    ccline.cmdbuff = alloc(len);    // caller should check for out-of-memory
    ccline.cmdbufflen = (ಪೂರ್ಣಾಂಕ)len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
realloc_cmdbuff(ಪೂರ್ಣಾಂಕ len)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		plen;

    ಆದರೆ (len < ccline.cmdbufflen)
	ಹಿಂತಿರುಗಿ OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    ಆದರೆ (ccline.xpc != ಖಾಲಿ
	    && ccline.xpc->xp_ನಮೂನೆ != ಖಾಲಿ
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	ಪೂರ್ಣಾಂಕ i = (ಪೂರ್ಣಾಂಕ)(ccline.xpc->xp_ನಮೂನೆ - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	ಆದರೆ (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_ನಮೂನೆ = ccline.cmdbuff + i;
    }

    vim_free(p);

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_ARABIC)
ಸ್ಥಿರ char_u	*arshape_buf = ಖಾಲಿ;

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_arshape_buf(ಶೂನ್ಯ)
{
    vim_free(arshape_buf);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_cmdline(ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ len)
{
#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (cmdline_ನಕ್ಷತ್ರ > 0)
	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    ಸಂದೇಶ_putchar('*');
	    ಆದರೆ (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + ಆರಂಭ + i) - 1;
	}
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
	ಆದರೆ (p_arshape && !p_tbidi && cmdline_has_arabic(ಆರಂಭ, len))
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	buflen = 0;
	char_u		*p;
	ಪೂರ್ಣಾಂಕ		j;
	ಪೂರ್ಣಾಂಕ		newlen = 0;
	ಪೂರ್ಣಾಂಕ		mb_l;
	ಪೂರ್ಣಾಂಕ		pc, pc1 = 0;
	ಪೂರ್ಣಾಂಕ		prev_c = 0;
	ಪೂರ್ಣಾಂಕ		prev_c1 = 0;
	ಪೂರ್ಣಾಂಕ		u8c;
	ಪೂರ್ಣಾಂಕ		u8cc[MAX_MCO];
	ಪೂರ್ಣಾಂಕ		nc = 0;

	/*
	 * Do arabic shaping into a temporary buffer.  This is very
	 * inefficient!
	 */
	ಆದರೆ (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    ಆದರೆ (arshape_buf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;	// out of memory
	}

	ಆದರೆ (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + ಆರಂಭ)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	ಗಾಗಿ (j = ಆರಂಭ; j < ಆರಂಭ + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, ಆರಂಭ + len - j);
	    mb_l = utfc_ptr2len_len(p, ಆರಂಭ + len - j);
	    ಆದರೆ (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		ಆದರೆ (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    ಆದರೆ (j + mb_l >= ಆರಂಭ + len)
			nc = NUL;
		    ಇಲ್ಲದಿದ್ದರೆ
			nc = utf_ptr2char(p + mb_l);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // displaying from left to right
		    ಆದರೆ (j + mb_l >= ಆರಂಭ + len)
			pc = NUL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      ಆರಂಭ + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, ಖಾಲಿ, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		ಆದರೆ (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    ಆದರೆ (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	ಸಂದೇಶ_outtrans_len(arshape_buf, newlen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_outtrans_len(ccline.cmdbuff + ಆರಂಭ, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    ಶೂನ್ಯ
putcmdline(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ ಸರಿಸು)
{
    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = TRUE;
    ಸಂದೇಶ_putchar(c);
    ಆದರೆ (ಸರಿಸು)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_ಸರಿಸು = ಸರಿಸು;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    ಶೂನ್ಯ
unputcmdline(ಶೂನ್ಯ)
{
    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = TRUE;
    ಆದರೆ (ccline.cmdlen == ccline.cmdpos)
	ಸಂದೇಶ_putchar(' ');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    ಇಲ್ಲದಿದ್ದರೆ
	draw_cmdline(ccline.cmdpos, 1);
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = FALSE;
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
    ಪೂರ್ಣಾಂಕ
put_on_cmdline(char_u *str, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ redraw)
{
    ಪೂರ್ಣಾಂಕ		retval;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		m;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (len < 0)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    ಆದರೆ (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	retval = OK;
    ಆದರೆ (retval == OK)
    {
	ಆದರೆ (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		ಗಾಗಿ (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		ಗಾಗಿ (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		ಆದರೆ (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	ಆದರೆ (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    ಆಗ (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_ತಲೆ_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
	    ಆದರೆ (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_ತಲೆ_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ಆದರೆ (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    i = 0;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		ಸಂದೇಶ_ಲಂಬಸಾಲು -= i;
		ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು < 0)
		{
		    ಸಂದೇಶ_ಲಂಬಸಾಲು += cmdline_ಅಗಲ;
		    --ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
		}
	    }
	}

	ಆದರೆ (redraw && !cmd_silent)
	{
	    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = TRUE;
	    i = cmdline_ಅಡ್ಡಸಾಲು;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು != i || ccline.overstrike)
		ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = FALSE;
	}
	ಆದರೆ (ಕೀಲಿ_Typed)
	{
	    m = cmdline_ಅಗಲ * ಅಡ್ಡಸಾಲುಗಳು;
	    ಆದರೆ (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    m = MAXCOL;
	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    ಆದರೆ (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    ಆದರೆ (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    ಆದರೆ (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		ಆದರೆ (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    ಆದರೆ (redraw)
	ಸಂದೇಶ_check();
    ಹಿಂತಿರುಗಿ retval;
}

ಸ್ಥಿರ cmdline_info_T	prev_ccline;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prev_ccline_ಬಳಸಿದ = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_cmdline(cmdline_info_T *ccp)
{
    ಆದರೆ (!prev_ccline_ಬಳಸಿದ)
    {
	CLEAR_ಕ್ಷೇತ್ರ(prev_ccline);
	prev_ccline_ಬಳಸಿದ = TRUE;
    }
    *ccp = prev_ccline;
    prev_ccline = ccline;
    ccline.cmdbuff = ಖಾಲಿ;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_paste(
    ಪೂರ್ಣಾಂಕ regname,
    ಪೂರ್ಣಾಂಕ literally,	// Insert text literally instead of "as typed"
    ಪೂರ್ಣಾಂಕ remcr)		// remove trailing CR
{
    ದೀರ್ಘ		i;
    char_u		*arg;
    char_u		*p;
    ಪೂರ್ಣಾಂಕ			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    ಆದರೆ (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	ಹಿಂತಿರುಗಿ FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    ಸಾಲು_breakcheck();
    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#ಆದರೆ_ಅಂತ್ಯ

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    ಆದರೆ (i)
    {
	// Got the value of a special register in "arg".
	ಆದರೆ (arg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	ಆದರೆ (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    ಪೂರ್ಣಾಂಕ	    len;

	    // Locate start of last word in the cmd buffer.
	    ಗಾಗಿ (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		ಆದರೆ (has_mbyte)
		{
		    len = (*mb_ತಲೆ_off)(ccline.cmdbuff, w - 1) + 1;
		    ಆದರೆ (!vim_iswordc(mb_ptr2char(w - len)))
			ನಿಲ್ಲಿಸು;
		    w -= len;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (!vim_iswordc(w[-1]))
			ನಿಲ್ಲಿಸು;
		    --w;
		}
	    }
	    len = (ಪೂರ್ಣಾಂಕ)((ccline.cmdbuff + ccline.cmdpos) - w);
	    ಆದರೆ (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	ಆದರೆ (allocated)
	    vim_free(arg);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    ಶೂನ್ಯ
cmdline_paste_str(char_u *s, ಪೂರ್ಣಾಂಕ literally)
{
    ಪೂರ್ಣಾಂಕ		c, cv;

    ಆದರೆ (literally)
	put_on_cmdline(s, -1, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (*s != NUL)
	{
	    cv = *s;
	    ಆದರೆ (cv == Ctrl_V && s[1])
		++s;
	    ಆದರೆ (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    ಇಲ್ಲದಿದ್ದರೆ
		c = *s++;
	    ಆದರೆ (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    || c == intr_char
#ಆದರೆ_ಅಂತ್ಯ
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
    ಶೂನ್ಯ
redrawcmdline(ಶೂನ್ಯ)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    ಶೂನ್ಯ
redrawcmdline_ex(ಪೂರ್ಣಾಂಕ do_compute_cmdrow)
{
    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;
    need_wait_return = FALSE;
    ಆದರೆ (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
redrawcmdprompt(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ccline.cmdfirstc != NUL)
	ಸಂದೇಶ_putchar(ccline.cmdfirstc);
    ಆದರೆ (ccline.cmdprompt != ಖಾಲಿ)
    {
	ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = ಸಂದೇಶ_ಲಂಬಸಾಲು + (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು - cmdline_ಅಡ್ಡಸಾಲು) * cmdline_ಅಗಲ;
	// do the reverse of set_cmdspos()
	ಆದರೆ (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (i = ccline.cmdindent; i > 0; --i)
	    ಸಂದೇಶ_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    ಶೂನ್ಯ
redrawcmd(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ save_ಒಳ_echowindow = ಒಳ_echowindow;

    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;

    // when 'incsearch' is set there may be no command line while redrawing
    ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)
    {
	windgoto(cmdline_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off);
	ಸಂದೇಶ_clr_eos();
	ಹಿಂತಿರುಗಿ;
    }

    // Do not put this in the message window.
    ಒಳ_echowindow = FALSE;

    sb_ಪಠ್ಯ_restart_cmdline();
    ಸಂದೇಶ_ಆರಂಭ();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    ಸಂದೇಶ_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    ಸಂದೇಶ_clr_eos();
    ಸಂದೇಶ_no_ಇನ್ನಷ್ಟು = FALSE;

    ಗಣ_cmdspos_cursor();
    ಆದರೆ (extra_char != NUL)
	putcmdline(extra_char, extra_char_ಸರಿಸು);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    ಸಂದೇಶ_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    ಒಳ_echowindow = save_ಒಳ_echowindow;
}

    ಶೂನ್ಯ
compute_cmdrow(ಶೂನ್ಯ)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    ಆದರೆ (exmode_ಸಕ್ರಿಯ || (ಸಂದೇಶ_scrolled != 0 && !updating_screen))
	cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಇಲ್ಲದಿದ್ದರೆ
	cmdline_ಅಡ್ಡಸಾಲು = W_WINROW(lastwin) + lastwin->w_ಎತ್ತರ
						    + lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
}

    ಶೂನ್ಯ
cursorcmd(ಶೂನ್ಯ)
{
    ಆದರೆ (cmd_silent)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (cmdmsg_rl)
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು  + (ccline.cmdspos / (cmdline_ಅಗಲ - 1));
	ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - (ccline.cmdspos % (cmdline_ಅಗಲ - 1)) - 1;
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು <= 0)
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು + (ccline.cmdspos / cmdline_ಅಗಲ);
	ಸಂದೇಶ_ಲಂಬಸಾಲು = ccline.cmdspos % cmdline_ಅಗಲ;
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }

    windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MCH_CURSOR_SHAPE
    mch_update_cursor();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gotocmdline(ಪೂರ್ಣಾಂಕ clr)
{
    ಸಂದೇಶ_ಆರಂಭ();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (cmdmsg_rl)
	ಸಂದೇಶ_ಲಂಬಸಾಲು = cmdline_ಅಗಲ - 1;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;	    // always start in column 0
    ಆದರೆ (clr)		    // clear the bottom line(s)
	ಸಂದೇಶ_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ccheck_abbr(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ spos = 0;

    ಆದರೆ (p_paste || no_abbr)	    // no abbreviations or in paste mode
	ಹಿಂತಿರುಗಿ FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    ಆಗ (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    ಆದರೆ (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    ಇಲ್ಲದಿದ್ದರೆ
	// check abbreviation from the beginning of the commandline
	spos = 0;

    ಹಿಂತಿರುಗಿ check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
}

/*
 * Escape special characters in "fname", depending on "what":
 * VSE_NONE: for when used as a file name argument after a Vim command.
 * VSE_SHELL: for a shell command.
 * VSE_BUFFER: for the ":buffer" command.
 * Returns the result in allocated memory.
 */
    char_u *
vim_strsave_fnameescape(char_u *fname, ಪೂರ್ಣಾಂಕ what)
{
    char_u	*p;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    char_u	buf[20];
    ಪೂರ್ಣಾಂಕ		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    ಗಾಗಿ (p = what == VSE_ಬಫರ್ ? ಬಫರ್_ESC_CHARS : ಮಾರ್ಗ_ESC_CHARS;
								*p != NUL; ++p)
	ಆದರೆ ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#ಇಲ್ಲದಿದ್ದರೆ
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_ಬಫರ್ ? ಬಫರ್_ESC_CHARS : ಮಾರ್ಗ_ESC_CHARS);
    ಆದರೆ (what == VSE_SHELL && csh_like_shell() && p != ಖಾಲಿ)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    ಆದರೆ (p != ಖಾಲಿ && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    ಶೂನ್ಯ
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    ಶೂನ್ಯ
tilde_replace(
    char_u  *orig_pat,
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_files,
    char_u  **files)
{
    ಪೂರ್ಣಾಂಕ	    i;
    char_u  *p;

    ಆದರೆ (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_files; ++i)
	{
	    p = home_replace_save(ಖಾಲಿ, files[i]);
	    ಆದರೆ (p != ಖಾಲಿ)
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
get_cmdline_info(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ಸ್ಥಿರ cmdline_info_T *
get_ccline_ptr(ಶೂನ್ಯ)
{
    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (ccline.cmdbuff != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &ccline;
    ಆದರೆ (prev_ccline_ಬಳಸಿದ && prev_ccline.cmdbuff != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &prev_ccline;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_cmdline_type(ಶೂನ್ಯ)
{
    cmdline_info_T *p = get_ccline_ptr();

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ NUL;
    ಆದರೆ (p->cmdfirstc == NUL)
	ಹಿಂತಿರುಗಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    (p->ಒಳಹರಿವು_fn) ? '@' :
#ಆದರೆ_ಅಂತ್ಯ
	    '-';
    ಹಿಂತಿರುಗಿ p->cmdfirstc;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ಸ್ಥಿರ char_u *
get_cmdline_str(ಶೂನ್ಯ)
{
    cmdline_info_T *p;

    ಆದರೆ (cmdline_ನಕ್ಷತ್ರ > 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    p = get_ccline_ptr();
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ಸ್ಥಿರ char_u *
get_cmdline_completion_ನಮೂನೆ(ಶೂನ್ಯ)
{
    cmdline_info_T *p;
    ಪೂರ್ಣಾಂಕ		xp_context;

    ಆದರೆ (cmdline_ನಕ್ಷತ್ರ > 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = get_ccline_ptr();
    ಆದರೆ (p == ಖಾಲಿ || p->xpc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    xp_context = p->xpc->xp_context;
    ಆದರೆ (xp_context == EXPAND_NOTHING)
    {
	ಗಣ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ಆದರೆ (xp_context == EXPAND_UNSUCCESSFUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    char_u *compl_pat = p->xpc->xp_ನಮೂನೆ;

    ಆದರೆ (compl_pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ಸ್ಥಿರ char_u *
get_cmdline_completion(ಶೂನ್ಯ)
{
    cmdline_info_T	*p;
    ಪೂರ್ಣಾಂಕ			xp_context;

    ಆದರೆ (cmdline_ನಕ್ಷತ್ರ > 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = get_ccline_ptr();
    ಆದರೆ (p == ಖಾಲಿ || p->xpc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    xp_context = p->xpc->xp_context;
    ಆದರೆ (xp_context == EXPAND_NOTHING)
    {
	ಗಣ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    ಆದರೆ (xp_context == EXPAND_UNSUCCESSFUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ cmdcomplete_type_ಗಮ್ಯ_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    ಶೂನ್ಯ
f_getcmdcomplpat(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_ನಮೂನೆ();
}

/*
 * "getcmdcompltype()" function
 */
    ಶೂನ್ಯ
f_getcmdcompltype(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    ಶೂನ್ಯ
f_getcmdline(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    ಶೂನ್ಯ
f_getcmdpos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ಸಂಖ್ಯೆ = p != ಖಾಲಿ ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    ಶೂನ್ಯ
f_getcmdprompt(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != ಖಾಲಿ && p->cmdprompt != ಖಾಲಿ ?
					    vim_strsave(p->cmdprompt) : ಖಾಲಿ;
}

/*
 * "getcmdscreenpos()" function
 */
    ಶೂನ್ಯ
f_getcmdscreenpos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_ಸಂಖ್ಯೆ = p != ಖಾಲಿ ? cmdline_ಲಂಬಸಾಲು_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    ಶೂನ್ಯ
f_getcmdtype(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    ಆದರೆ (rettv->vval.v_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_cmdline_str(char_u *str, ಪೂರ್ಣಾಂಕ pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    ಪೂರ್ಣಾಂಕ		    len;

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
    ಆದರೆ (realloc_cmdbuff(len + 1) != OK)
	ಹಿಂತಿರುಗಿ 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_cmdline_pos(
    ಪೂರ್ಣಾಂಕ		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    ಆದರೆ (pos < 0)
	new_cmdpos = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	new_cmdpos = pos;

    ಹಿಂತಿರುಗಿ 0;
}

// "setcmdline()" function
    ಶೂನ್ಯ
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ pos = -1;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;

	pos = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error) - 1;
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (pos < 0)
	{
	    emsg(_(e_ವಾದ_must_be_ಧನಾತ್ಮಕ));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_ಸಂಖ್ಯೆ = ಗಣ_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    ಶೂನ್ಯ
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		pos;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pos = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]) - 1;
    ಆದರೆ (pos >= 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = ಗಣ_cmdline_pos(pos);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the first character of the current command line.
 */
    ಪೂರ್ಣಾಂಕ
get_cmdline_firstc(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ccline.cmdfirstc;
}

/*
 * Get indices "num1,num2" that specify a range within a list (not a range of
 * text lines in a buffer!) from a string.  Used for ":history" and ":clist".
 * Returns OK if parsed successfully, otherwise FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_list_range(char_u **str, ಪೂರ್ಣಾಂಕ *num1, ಪೂರ್ಣಾಂಕ *num2)
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = FALSE;
    varnumber_T	ಅಂಕೆ;

    *str = skipwhite(*str);
    ಆದರೆ (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, ಖಾಲಿ, &len, 0, &ಅಂಕೆ, ಖಾಲಿ, 0, FALSE, ಖಾಲಿ);
	*str += len;
	//ಅತಿಪ್ರವಾಹ
	ಆದರೆ (ಅಂಕೆ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	    ಹಿಂತಿರುಗಿ FAIL;

	*num1 = (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
	ಮೊದಲ = TRUE;
    }
    *str = skipwhite(*str);
    ಆದರೆ (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, ಖಾಲಿ, &len, 0, &ಅಂಕೆ, ಖಾಲಿ, 0, FALSE, ಖಾಲಿ);
	ಆದರೆ (len > 0)
	{
	    *str = skipwhite(*str + len);
	    //ಅತಿಪ್ರವಾಹ
	    ಆದರೆ (ಅಂಕೆ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
		ಹಿಂತಿರುಗಿ FAIL;

	    *num2 = (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಮೊದಲ)		// no number given at all
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ)			// only one number given
	*num2 = *num1;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cedit(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ n;

    ಆದರೆ (*p_cedit == NUL)
	cedit_ಕೀಲಿ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	n = string_ಗಮ್ಯ_ಕೀಲಿ(p_cedit, FALSE);
	ಆದರೆ (n == 0 || vim_isprintc(n))
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	cedit_ಕೀಲಿ = n;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
open_cmdwin(ಶೂನ್ಯ)
{
    bufref_T		old_curbuf;
    win_T		*old_curwin = curwin;
    bufref_T		bufref;
    win_T		*wp;
    ಪೂರ್ಣಾಂಕ			i;
    linenr_T		lnum;
    ಪೂರ್ಣಾಂಕ			histtype;
    garray_T		winsizes;
    ಪೂರ್ಣಾಂಕ			save_restart_edit = restart_edit;
    ಪೂರ್ಣಾಂಕ			save_State = State;
    ಪೂರ್ಣಾಂಕ			save_exmode = exmode_ಸಕ್ರಿಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಪೂರ್ಣಾಂಕ			save_cmdmsg_rl = cmdmsg_rl;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ			save_ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ			newbuf_ಸ್ಥಿತಿ;
    ಪೂರ್ಣಾಂಕ			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    ಆದರೆ (ಪಠ್ಯ_or_buf_locked()
	    || cmdwin_type != 0
#ಆದರೆ defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_ನಕ್ಷತ್ರ > 0
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ K_IGNORE;
    }
    ಗಣ_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_ಗಾತ್ರ_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    ಆದರೆ (win_ವಿಭಜಿಸು((ಪೂರ್ಣಾಂಕ)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	ಹಿಂತಿರುಗಿ K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    ಆದರೆ (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_ಬಫರ್ != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	ಹಿಂತಿರುಗಿ Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_ಸ್ಥಿತಿ = do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE, ECMD_HIDE,
					ಖಾಲಿ);
    cmdwin_valid = win_valid(cmdwin_win);
    ಆದರೆ (newbuf_ಸ್ಥಿತಿ == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_ಬಫರ್ != old_curbuf.br_buf)
    {
	ಆದರೆ (newbuf_ಸ್ಥಿತಿ == OK)
	    ಗಣ_bufref(&bufref, curbuf);
	ಆದರೆ (cmdwin_valid && !ಕೊನೆಯ_ಕಿಟಕಿ())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	ಆದರೆ (newbuf_ಸ್ಥಿತಿ == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_ಬಫರ್(ಖಾಲಿ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = ಖಾಲಿ;
	beep_flush();
	ga_clear(&winsizes);
	ಹಿಂತಿರುಗಿ Ctrl_C;
    }
    cmdwin_buf = curbuf;

    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_ಬೀಗ;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    ಆದರೆ (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	ಆದರೆ (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    ಕೂಡಿಸು_map((char_u *)"<buffer> <Tab> <C-X><C-V>", ಬಹುಲಕ_ಸೇರಿಸು, TRUE);
	    ಕೂಡಿಸು_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", ಬಹುಲಕ_ಸಾಮಾನ್ಯ, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    ಕೂಡಿಸು_map((char_u *)"<buffer> <S-Tab> <C-P>", ಬಹುಲಕ_ಸೇರಿಸು, TRUE);
	}
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_ಬೀಗ;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    ಆದರೆ (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	ಆದರೆ (i >= 0)
	{
	    lnum = 0;
	    ಮಾಡು
	    {
		ಆದರೆ (++i == get_hislen())
		    i = 0;
		ಆದರೆ (get_histentry(histtype)[i].hisstr != ಖಾಲಿ)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    ಆಗ (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    curwin->w_cursor.ಲಂಬಸಾಲು = ccline.cmdpos;
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_ಸಕ್ರಿಯ = 0;

    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_ಫಲಿತಾಂಶ = 0;

    // Trigger CmdwinEnter autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINENTER);
    ಆದರೆ (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ

    cmdwin_type = 0;
    cmdwin_buf = ಖಾಲಿ;
    cmdwin_win = ಖಾಲಿ;
    exmode_ಸಕ್ರಿಯ = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    ಆದರೆ (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_ಬಫರ್ != old_curbuf.br_buf)
    {
	cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
	emsg(_(e_ಸಕ್ರಿಯ_ಕಿಟಕಿ_or_ಬಫರ್_ಬದಲಾಯಿತು_or_deleted));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(FEAT_EVAL)
	// autocmds may abort script processing
	ಆದರೆ (aborting() && cmdwin_ಫಲಿತಾಂಶ != K_IGNORE)
	    cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
#ಆದರೆ_ಅಂತ್ಯ
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	ಆದರೆ (cmdwin_ಫಲಿತಾಂಶ == K_XF1 || cmdwin_ಫಲಿತಾಂಶ == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    ಆದರೆ (cmdwin_ಫಲಿತಾಂಶ == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    ಆದರೆ (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (ಪೂರ್ಣಾಂಕ)plen;
		ccline.cmdbufflen = (ಪೂರ್ಣಾಂಕ)(plen + 1);
		cmdwin_ಫಲಿತಾಂಶ = CAR;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdwin_ಫಲಿತಾಂಶ == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	ಆದರೆ (ccline.cmdbuff == ಖಾಲಿ)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ccline.cmdpos = curwin->w_cursor.ಲಂಬಸಾಲು;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    ಆದರೆ (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#ಆದರೆ_ಅಂತ್ಯ
	// First go back to the original window.
	wp = curwin;
	ಗಣ_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	ಆದರೆ (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	ಆದರೆ (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_ಬಫರ್(ಖಾಲಿ, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_ಗಾತ್ರ_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	ಆದರೆ (cmdwin_ಫಲಿತಾಂಶ == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    ಗಣ_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#ಆದರೆ_ಅಂತ್ಯ

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    ಹಿಂತಿರುಗಿ cmdwin_ಫಲಿತಾಂಶ;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    ಪೂರ್ಣಾಂಕ
is_ಒಳ_cmdwin(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap ಬಳಸದ, char_u *cmd ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    ಆದರೆ (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    ಶೂನ್ಯ
get_user_ಒಳಹರಿವು(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		inputdialog,
    ಪೂರ್ಣಾಂಕ		secret)
{
    char_u	*prompt;
    char_u	*p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    ಪೂರ್ಣಾಂಕ		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = ಖಾಲಿ;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    ಆದರೆ (ಒಳಹರಿವು_busy)
	ಹಿಂತಿರುಗಿ;  // this doesn't work recursively.

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_ಒಳಹರಿವು
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    ಆದರೆ (no_console_ಒಳಹರಿವು() && !is_not_a_ಪದ())
	ಹಿಂತಿರುಗಿ;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    ಆದರೆ (prompt != ಖಾಲಿ)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	ಆದರೆ (p == ಖಾಲಿ)
	    p = prompt;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    ಸಂದೇಶ_ಆರಂಭ();
	    ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)prompt, get_echo_attr());
	    ಸಂದೇಶ_didout = FALSE;
	    ಸಂದೇಶ_starthere();
	    *p = c;
	}
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    ಆದರೆ (defstr != ಖಾಲಿ)
		stuffReadbuffSpec(defstr);

	    ಆದರೆ (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		ಪೂರ್ಣಾಂಕ	xp_namelen;
		ದೀರ್ಘ	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = ಖಾಲಿ;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		ಆದರೆ (xp_name == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;

		xp_namelen = (ಪೂರ್ಣಾಂಕ)STRLEN(xp_name);

		ಆದರೆ (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ (defstr != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ save_ex_ಸಾಮಾನ್ಯ_busy = ex_ಸಾಮಾನ್ಯ_busy;
	    ಪೂರ್ಣಾಂಕ save_vgetc_busy = vgetc_busy;
	    ಪೂರ್ಣಾಂಕ save_ಒಳಹರಿವು_busy = ಒಳಹರಿವು_busy;

	    ಒಳಹರಿವು_busy |= vgetc_busy;
	    ex_ಸಾಮಾನ್ಯ_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_ಸಾಮಾನ್ಯ_busy = save_ex_ಸಾಮಾನ್ಯ_busy;
	    vgetc_busy = save_vgetc_busy;
	    ಒಳಹರಿವು_busy = save_ಒಳಹರಿವು_busy;
	}
	ಆದರೆ (inputdialog && rettv->vval.v_string == ಖಾಲಿ
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	ಸಂದೇಶ_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "wildtrigger()" function
 */
    ಶೂನ್ಯ
f_wildtrigger(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (!(State & ಬಹುಲಕ_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_ಸಕ್ರಿಯ())
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ cmd_type = get_cmdline_type();

    ಆದರೆ (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	ಕೀಲಿ_string[4];

	ಕೀಲಿ_string[0] = K_SPECIAL;
	ಕೀಲಿ_string[1] = KS_EXTRA;
	ಕೀಲಿ_string[2] = KE_WILD;
	ಕೀಲಿ_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(ಕೀಲಿ_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
