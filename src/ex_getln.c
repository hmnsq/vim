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
#وصف_ڪريو CMDLINE_NOT_بدليو	1
#وصف_ڪريو CMDLINE_بدليو		2
#وصف_ڪريو GOTO_عام_گهڻائي	3
#وصف_ڪريو PROCESS_ايندڙ_ڪنجي	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ساكن cmdline_info_T ccline;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن صحيح	new_cmdpos;	// position set by set_cmdline_pos()
#جيڪڏهن_پورو

ساكن صحيح	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ساكن صحيح	extra_char_سرڪايو;

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
ساكن صحيح	cmd_hkmap = 0;	// Hebrew mapping during command line
#جيڪڏهن_پورو

ساكن char_u	*getcmdline_int(صحيح firstc, ڊگهو ڳڻپ, صحيح indent, صحيح clear_ccline);
ساكن صحيح	cmdline_charsize(صحيح idx);
ساكن فراغ	سيٽ_cmdspos(فراغ);
ساكن فراغ	سيٽ_cmdspos_cursor(فراغ);
ساكن فراغ	correct_cmdspos(صحيح idx, صحيح cells);
ساكن فراغ	dealloc_cmdbuff(فراغ);
ساكن فراغ	alloc_cmdbuff(size_t len);
ساكن فراغ	draw_cmdline(صحيح شروعات, صحيح len);
ساكن فراغ	save_cmdline(cmdline_info_T *ccp);
ساكن فراغ	restore_cmdline(cmdline_info_T *ccp);
ساكن صحيح	cmdline_paste(صحيح regname, صحيح literally, صحيح remcr);
ساكن فراغ	redrawcmdprompt(فراغ);
ساكن صحيح	ccheck_abbr(صحيح);
ساكن صحيح	open_cmdwin(فراغ);
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
ساكن صحيح	خالي_جايون_نمونو_magic(char_u *pat, size_t len, magic_T magic_val);
#جيڪڏهن_پورو

ساكن صحيح	cedit_ڪنجي = -1;	// key value of 'cedit' option

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
    إذا (نياپو_scrolled == 0)
	compute_cmdrow();
    نياپو("");
    redraw_cmdline = TRUE;
}

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ساكن صحيح
خالي_جايون_نمونو(char_u *p, size_t len, صحيح delim)
{
    magic_T	magic_val = MAGIC_ON;

    إذا (len > 0)
	(فراغ) skip_regexp_ex(p, delim, magic_isset(), عدم, عدم, &magic_val);
    وإلا
	أرجع TRUE;

    أرجع خالي_جايون_نمونو_magic(p, len, magic_val);
}

    ساكن صحيح
خالي_جايون_نمونو_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    طالما (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != عدم)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    أرجع len == 0
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
    linenr_T	vs_خالي_جايون_صفون;
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
    vs->vs_خالي_جايون_صفون = curwin->w_خالي_جايون_صفون;
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
    curwin->w_خالي_جايون_صفون = vs->vs_خالي_جايون_صفون;
}

// Struct to store the state of 'incsearch' highlighting.
تعريفنوع بنية {
    pos_T	ڳولا_شروعات_2;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    صحيح		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	ميلاپ_شروعات;
    pos_T	ميلاپ_پڇاڙي;
    صحيح		did_incsearch;
    صحيح		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ساكن فراغ
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->ميلاپ_شروعات = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->ميلاپ_پڇاڙي);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->ڳولا_شروعات_2 = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ساكن فراغ
سيٽ_ڳولا_ميلاپ(pos_T *t)
{
    t->lnum += ڳولا_ميلاپ_سٽون;
    t->کانو = ڳولا_ميلاپ_endcol;
    إذا (t->lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
    {
	t->lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
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
parse_نمونو_and_range(
	pos_T	*incsearch_شروعات,
	صحيح	*ڳولا_delim,
	صحيح	*skiplen,
	صحيح	*patlen)
{
    char_u	*cmd, *p, *پڇاڙي;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    صحيح		delim_optional = FALSE;
    صحيح		delim;
    صحيح		use_آخري_pat;
    magic_T     magic = 0;
    حرف	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    ڳولا_پهريون_سٽ = 0;
    ڳولا_آخري_سٽ = MAXLNUM;

    CLEAR_ميدان(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_سٽون;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, عدم);

    إذا (vim_strchr((char_u *)"sgvlu", *cmd) == عدم)
	أرجع FALSE;

    // Skip over command name to find pattern separator
    لكل (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    إذا (*skipwhite(p) == NUL)
	أرجع FALSE;

    إذا (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	إذا (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	وإلا إذا (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    وإلا إذا (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	إذا (*p == '!')
	    p = skipwhite(p + 1);
	طالما (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	إذا (*p == NUL)
	    أرجع FALSE;
    }
    وإلا إذا (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vimgrepadd", MAX(p - cmd, 8)) == 0
	    || STRNCMP(cmd, "lvimgrep", MAX(p - cmd, 2)) == 0
	    || STRNCMP(cmd, "lvimgrepadd", MAX(p - cmd, 9)) == 0
	    || STRNCMP(cmd, "global", p - cmd) == 0)
    {
	// skip optional "!"
	إذا (*p == '!')
	{
	    p++;
	    إذا (*skipwhite(p) == NUL)
		أرجع FALSE;
	}
	إذا (*cmd != 'g')
	    delim_optional = TRUE;
    }
    وإلا
	أرجع FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *ڳولا_delim = delim;

    پڇاڙي = skip_regexp_ex(p, delim, magic_isset(), عدم, عدم, &magic);
    use_آخري_pat = پڇاڙي == p && *پڇاڙي == delim;

    إذا (پڇاڙي == p && !use_آخري_pat)
	أرجع FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    إذا (!use_آخري_pat)
    {
	حرف c = *پڇاڙي;
	صحيح خالي_جايون_2;

	*پڇاڙي = NUL;
	خالي_جايون_2 = خالي_جايون_نمونو_magic(p, (size_t)(پڇاڙي - p), magic);
	*پڇاڙي = c;
	إذا (خالي_جايون_2)
	    أرجع FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (صحيح)(p - ccline.cmdbuff);
    *patlen = (صحيح)(پڇاڙي - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_شروعات;

    parse_cmd_پتو(&ea, &dummy, TRUE);

    إذا (ea.addr_ڳڻپ > 0)
    {
	صحيح ابتو_ميلاپ = ea.line2 < ea.line1;
	ڳولا_پهريون_سٽ = ابتو_ميلاپ ? ea.line2 : ea.line1;
	ڳولا_آخري_سٽ = ابتو_ميلاپ ? ea.line1 : ea.line2;
    }
    وإلا إذا (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	ڳولا_پهريون_سٽ = ڳولا_آخري_سٽ = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    أرجع TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ساكن صحيح
do_incsearch_highlighting(
	صحيح		    firstc,
	صحيح		    *ڳولا_delim,
	incsearch_state_T   *is_state,
	صحيح		    *skiplen,
	صحيح		    *patlen)
{
    صحيح retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    إذا (!p_is || cmd_silent)
	أرجع FALSE;

    // By default search all lines
    ڳولا_پهريون_سٽ = 0;
    ڳولا_آخري_سٽ = MAXLNUM;

    إذا (firstc == '/' || firstc == '?')
    {
	*ڳولا_delim = firstc;
	أرجع TRUE;
    }

    إذا (firstc != ':')
	أرجع FALSE;

    ++emsg_off;
    retval = parse_نمونو_and_range(&is_state->ڳولا_شروعات_2, ڳولا_delim,
	    skiplen, patlen);
    --emsg_off;

    أرجع retval;
}

    ساكن فراغ
پڄاڻي_incsearch_highlighting(
	صحيح gotesc,
	incsearch_state_T *is_state,
	صحيح call_update_screen)
{
    إذا (!is_state->did_incsearch)
	أرجع;

    is_state->did_incsearch = FALSE;
    إذا (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    وإلا
    {
	إذا (!EQUAL_POS(is_state->save_cursor, is_state->ڳولا_شروعات_2))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->ڳولا_شروعات_2;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_ميلاپ = FALSE;

    // by default search all lines
    ڳولا_پهريون_سٽ = 0;
    ڳولا_آخري_سٽ = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    حالت_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    إذا (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ساكن فراغ
may_do_incsearch_highlighting(
	صحيح		    firstc,
	ڊگهو		    ڳڻپ,
	incsearch_state_T   *is_state)
{
    صحيح		skiplen, patlen;
    صحيح		مليو;  // do_search() result
    pos_T	پڇاڙي_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    صحيح		ايندڙ_char;
    صحيح		use_آخري_pat;
    صحيح		did_do_incsearch = is_state->did_incsearch;
    صحيح		ڳولا_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخري_ڳولا_نمونو();

    إذا (!do_incsearch_highlighting(firstc, &ڳولا_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخري_ڳولا_نمونو();
	پڄاڻي_incsearch_highlighting(FALSE, is_state, TRUE);
	إذا (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	أرجع;
    }

    // If there is a character waiting, search and redraw later.
    إذا (char_avail())
    {
	restore_آخري_ڳولا_نمونو();
	is_state->incsearch_postponed = TRUE;
	أرجع;
    }
    is_state->incsearch_postponed = FALSE;

    إذا (ڳولا_پهريون_سٽ == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->ڳولا_شروعات_2;
    وإلا إذا (ڳولا_پهريون_سٽ > curbuf->b_ml.ml_سٽ_ڳڻپ)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	curwin->w_cursor.کانو = MAXCOL;
    }
    وإلا
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = ڳولا_پهريون_سٽ;
	curwin->w_cursor.کانو = 0;
    }

    // Use the previous pattern for ":s//".
    ايندڙ_char = ccline.cmdbuff[skiplen + patlen];
    use_آخري_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == ايندڙ_char;

    // If there is no pattern, don't do anything.
    إذا (patlen == 0 && !use_آخري_pat)
    {
	مليو = 0;
	سيٽ_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    وإلا
    {
	صحيح ڳولا_flags = ڳولا_OPT + ڳولا_NOOF + ڳولا_PEEK;

	cursor_off();	// so the user knows we're busy
	ٻاهريون_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	إذا (!p_hls)
	    ڳولا_flags += ڳولا_KEEP;
	إذا (ڳولا_پهريون_سٽ != 0)
	    ڳولا_flags += ڳولا_شروعات;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_ميدان(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	مليو = do_ڳولا(عدم, firstc == ':' ? '/' : firstc, ڳولا_delim,
				 ccline.cmdbuff + skiplen, patlen, ڳڻپ, ڳولا_flags,
# ifdef FEAT_RELTIME
		&sia
# وإلا
		عدم
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = ايندڙ_char;
	--emsg_off;

	إذا (curwin->w_cursor.lnum < ڳولا_پهريون_سٽ
		|| curwin->w_cursor.lnum > ڳولا_آخري_سٽ)
	{
	    // match outside of address range
	    مليو = 0;
	    curwin->w_cursor = is_state->ڳولا_شروعات_2;
	}

	// if interrupted while searching, behave like it failed
	إذا (got_int)
	{
	    (فراغ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    مليو = 0;
	}
	وإلا إذا (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    إذا (مليو != 0)
	highlight_ميلاپ = TRUE;		// highlight position
    وإلا
	highlight_ميلاپ = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    بدليو_cline_bef_curs();
    update_topline();

    إذا (مليو != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->ميلاپ_شروعات = curwin->w_cursor;
	سيٽ_ڳولا_ميلاپ(&curwin->w_cursor);
	validate_cursor();
	پڇاڙي_pos = curwin->w_cursor;
	is_state->ميلاپ_پڇاڙي = پڇاڙي_pos;
	curwin->w_cursor = save_pos;
    }
    وإلا
	پڇاڙي_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    إذا (!use_آخري_pat)
    {
	ايندڙ_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	إذا (خالي_جايون_نمونو(ccline.cmdbuff + skiplen, (size_t)patlen, ڳولا_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    سيٽ_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = ايندڙ_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    إذا (p_ru && curwin->w_حالت_اوچائي > 0)
	curwin->w_redr_حالت = true;

    update_screen(UPD_SOME_VALID);
    highlight_ميلاپ = FALSE;
    restore_آخري_ڳولا_نمونو();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    إذا (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->ڳولا_شروعات_2;
    وإلا إذا (مليو != 0)
	curwin->w_cursor = پڇاڙي_pos;

    نياپو_starthere();
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
	ڊگهو			ڳڻپ,
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
    soffset_T	وٿي;
    صحيح		ڳولا_flags = ڳولا_NOOF;
    صحيح		i;
    صحيح		ڳولا_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخري_ڳولا_نمونو();

    إذا (!do_incsearch_highlighting(firstc, &ڳولا_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخري_ڳولا_نمونو();
	أرجع OK;
    }
    إذا (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_آخري_ڳولا_نمونو();
	أرجع FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (فراغ)parse_ڳولا_نمونو_وٿي(&pat, &patlen_s, ڳولا_delim,
				    ڳولا_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &وٿي);

    cursor_off();
    ٻاهريون_flush();
    إذا (c == Ctrl_G)
    {
	t = is_state->ميلاپ_پڇاڙي;
	إذا (LT_POS(is_state->ميلاپ_شروعات, is_state->ميلاپ_پڇاڙي))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (فراغ)decl(&t);
	ڳولا_flags += ڳولا_کانو;
    }
    وإلا
	t = is_state->ميلاپ_شروعات;
    إذا (!p_hls)
	ڳولا_flags += ڳولا_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, عدم,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, ڳڻپ, ڳولا_flags, RE_ڳولا, عدم);
    --emsg_off;
    إذا (dircp != عدم)
	*dircp = ڳولا_delim;
    إذا (i)
    {
	pos_T	ميلاپ_شروعات = is_state->ميلاپ_شروعات;
	pos_T	ميلاپ_پڇاڙي = is_state->ميلاپ_پڇاڙي;
	ڊگهو	off = وٿي.off;

	is_state->ڳولا_شروعات_2 = ميلاپ_شروعات;
	إذا (!وٿي.سٽ && (وٿي.پڇاڙي || off != 0))
	{
	    إذا (وٿي.پڇاڙي)
	    {
		is_state->ڳولا_شروعات_2 = ميلاپ_پڇاڙي;
		(فراغ)decl(&is_state->ڳولا_شروعات_2);
	    }
	    طالما (off > 0)
	    {
		إذا (incl(&is_state->ڳولا_شروعات_2) == -1)
		    اكسر;
		--off;
	    }
	    طالما (off < 0)
	    {
		إذا (decl(&is_state->ڳولا_شروعات_2) == -1)
		    اكسر;
		++off;
	    }
	}
	is_state->ميلاپ_پڇاڙي = t;
	is_state->ميلاپ_شروعات = t;
	إذا (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ڳولا_شروعات_2 = t;
	    (فراغ)decl(&is_state->ڳولا_شروعات_2);
	}
	وإلا إذا (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->ڳولا_شروعات_2 = t;
	    (فراغ)incl(&is_state->ڳولا_شروعات_2);
	}
	إذا (LT_POS(t, is_state->ڳولا_شروعات_2) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->ڳولا_شروعات_2 = t;
	    إذا (firstc == '?')
		(فراغ)incl(&is_state->ڳولا_شروعات_2);
	    وإلا
		(فراغ)decl(&is_state->ڳولا_شروعات_2);
	}

	سيٽ_ڳولا_ميلاپ(&is_state->ميلاپ_پڇاڙي);
	curwin->w_cursor = is_state->ميلاپ_شروعات;
	بدليو_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_ميلاپ = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_ميلاپ = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->ميلاپ_پڇاڙي;
    }
    وإلا
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_آخري_ڳولا_نمونو();
    أرجع FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ساكن صحيح
may_جوڙيو_char_منزل_ڳولا(صحيح firstc, صحيح *c, incsearch_state_T *is_state)
{
    صحيح		skiplen, patlen, ڳولا_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخري_ڳولا_نمونو();

    إذا (!do_incsearch_highlighting(firstc, &ڳولا_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخري_ڳولا_نمونو();
	أرجع FAIL;
    }
    restore_آخري_ڳولا_نمونو();

    // Add a character from under the cursor for 'incsearch'.
    إذا (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->ميلاپ_پڇاڙي;
	*c = gchar_cursor();
	إذا (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    إذا (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    إذا (*c == ڳولا_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != عدم)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    إذا (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		صحيح save_c = *c;

		طالما (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.کانو += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    أرجع FAIL;
	}
    }
    أرجع OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ساكن صحيح
cmdline_has_arabic(صحيح شروعات, صحيح len)
{
    صحيح	    j;
    صحيح	    mb_l;
    صحيح	    u8c;
    char_u  *p;
    صحيح	    u8cc[MAX_MCO];

    إذا (!enc_utf8)
	أرجع FALSE;

    لكل (j = شروعات; j < شروعات + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, شروعات + len - j);
	mb_l = utfc_ptr2len_len(p, شروعات + len - j);
	إذا (ARABIC_CHAR(u8c))
	    أرجع TRUE;
    }
    أرجع FALSE;
}
#جيڪڏهن_پورو

    فراغ
cmdline_init(فراغ)
{
    CLEAR_ميدان(ccline);
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
    ++allow_ڪنجيون;
    c = plain_vgetc();
    --no_mapping;
    --allow_ڪنجيون;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    إذا (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && ڪنجي_Typed)
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		|| cmdline_تارو > 0
#جيڪڏهن_پورو
		))
    {
	vungetc(c);
	أرجع PROCESS_ايندڙ_ڪنجي;
    }

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	إذا (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	وإلا
	    new_cmdpos = ccline.cmdpos;

	c = get_اظهار_رجسٽر();
	إذا (c == '=')
	{
	    char_u	*p = عدم;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_اظهار_سٽ();
	    --textlock;

	    إذا (p != عدم)
	    {
		صحيح len = (صحيح)STRLEN(p);

		إذا (realloc_cmdbuff(len + 1) == OK)
		{
		    ccline.cmdlen = len;
		    STRCPY(ccline.cmdbuff, p);
		    vim_free(p);

		    // Restore the cursor or use the position set with
		    // set_cmdline_pos().
		    إذا (new_cmdpos > ccline.cmdlen)
			ccline.cmdpos = ccline.cmdlen;
		    وإلا
			ccline.cmdpos = new_cmdpos;

		    ڪنجي_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    أرجع CMDLINE_بدليو;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	أرجع CMDLINE_NOT_بدليو;
    }
#جيڪڏهن_پورو

    إذا (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    أرجع GOTO_عام_گهڻائي;
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
	صحيح		*wim_اشاريو_p,
	expand_T	*xp,
	صحيح		*gotesc,
	صحيح		redraw_if_menu_خالي_جايون,
	pos_T		*pre_incsearch_pos)
{
    صحيح		wim_اشاريو = *wim_اشاريو_p;
    صحيح		res;
    صحيح		cmdpos_before;
    صحيح		options = WILD_NO_BEEP;
    صحيح		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    صحيح		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    إذا (wim_flags[wim_اشاريو] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    إذا (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	إذا (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_اشاريو] & WIM_LIST))
	{
	    (فراغ)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_اشاريو] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	إذا (wim_flags[wim_اشاريو] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	وإلا إذا (wim_flags[wim_اشاريو] & WIM_ڀريل)
	    res = nextwild(xp, WILD_ايندڙ, options, escape);
	وإلا
	    res = OK;	    // don't insert 'wildchar' now
    }
    وإلا		    // typed p_wc first time
    {
	صحيح wim_longest = (wim_flags[0] & WIM_LONGEST);
	صحيح wim_list = (wim_flags[0] & WIM_LIST);
	صحيح wim_ڀريل = (wim_flags[0] & WIM_ڀريل);

	wim_اشاريو = 0;
	إذا (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_نمونو;
	    إذا (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    إذا (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    وإلا
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	إذا (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	وإلا
	{
	    إذا (wim_noselect || (wim_list && !wim_ڀريل))
		options |= WILD_NOSELECT;
	    إذا (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	إذا (redraw_if_menu_خالي_جايون && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	إذا (got_int)
	{
	    (فراغ)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (فراغ)ExpandOne(xp, عدم, عدم, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_اشاريو_p = wim_اشاريو;
	    أرجع CMDLINE_بدليو;
	}

	// Display matches
	إذا (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    إذا (wim_longest)
	    {
		صحيح مليو_longest_اڳياڙي = (ccline.cmdpos != cmdpos_before);
		صحيح show_menu = p_wmnu && wim_ڀريل;

		إذا (wim_list || show_menu)
		    (فراغ)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		وإلا إذا (!مليو_longest_اڳياڙي)
		{
		    صحيح wim_list_ايندڙ = (wim_flags[1] & WIM_LIST);
		    صحيح wim_ڀريل_ايندڙ = (wim_flags[1] & WIM_ڀريل);
		    صحيح wim_noselect_ايندڙ = (wim_flags[1] & WIM_NOSELECT);
		    صحيح wim_noinsert_ايندڙ = (wim_flags[1] & WIM_NOINSERT);
		    إذا (wim_list_ايندڙ || (p_wmnu && (wim_ڀريل_ايندڙ
				    || wim_noselect_ايندڙ || wim_noinsert_ايندڙ)))
		    {
			إذا (wim_ڀريل_ايندڙ && !wim_noselect_ايندڙ && !wim_noinsert_ايندڙ)
			    nextwild(xp, WILD_ايندڙ, options, escape);
			وإلا
			    (فراغ)showmatches(xp, p_wmnu && (wim_noselect_ايندڙ
					|| wim_noinsert_ايندڙ), wim_list_ايندڙ,
				    p_wmnu ? wim_flags[1] : 0);

			إذا (wim_list_ايندڙ)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    وإلا
	    {
		صحيح show_menu = p_wmnu && (wim_ڀريل || wim_noselect
							    || wim_noinsert);

		إذا (wim_list || show_menu)
		    (فراغ)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		وإلا
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    إذا (wim_list)
		*did_wild_list = TRUE;
	}
	وإلا إذا (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    إذا (wim_اشاريو < 3)
	++wim_اشاريو;
    إذا (c == ESC)
	*gotesc = TRUE;

    *wim_اشاريو_p = wim_اشاريو;
    أرجع (res == OK) ? CMDLINE_بدليو : CMDLINE_NOT_بدليو;
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
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	, incsearch_state_T *isp
#جيڪڏهن_پورو
	)
{
    صحيح		i;
    صحيح		j;

    إذا (c == K_KDEL)
	c = K_DEL;

    /*
     * Delete current character is the same as backspace on next
     * character, except at end of line.
     */
    إذا (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
	++ccline.cmdpos;
    إذا (has_mbyte && c == K_DEL)
	ccline.cmdpos += mb_off_ايندڙ(ccline.cmdbuff,
		ccline.cmdbuff + ccline.cmdpos);
    إذا (ccline.cmdpos > 0)
    {
	char_u *p;

	j = ccline.cmdpos;
	p = ccline.cmdbuff + j;
	إذا (has_mbyte)
	{
	    p = mb_prevptr(ccline.cmdbuff, p);
	    إذا (c == Ctrl_W)
	    {
		طالما (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		طالما (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		إذا (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	وإلا إذا (c == Ctrl_W)
	{
	    طالما (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    إذا (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		طالما (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	وإلا
	    --p;
	ccline.cmdpos = (صحيح)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	طالما (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	إذا (ccline.cmdlen == 0)
	{
	    isp->ڳولا_شروعات_2 = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#جيڪڏهن_پورو
	redrawcmd();
    }
    وإلا إذا (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == عدم && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	إذا (exmode_سرگرم
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#جيڪڏهن_پورو
	   )
	    أرجع CMDLINE_NOT_بدليو;

	dealloc_cmdbuff();	// no commandline to return

	إذا (!cmd_silent)
	{
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    إذا (cmdmsg_rl)
		نياپو_کانو = cmdline_ويڪر;
	    وإلا
#جيڪڏهن_پورو
		نياپو_کانو = 0;
	    نياپو_putchar(' ');		// delete ':'
	}
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	isp->ڳولا_شروعات_2 = isp->save_cursor;
#جيڪڏهن_پورو
	redraw_cmdline = TRUE;
	أرجع GOTO_عام_گهڻائي;
    }
    أرجع CMDLINE_بدليو;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ساكن فراغ
cmdline_toggle_langmap(ڊگهو *b_im_ptr)
{
    إذا (map_منزل_exists_گهڻائي((char_u *)"", گهڻائي_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= گهڻائي_LANGMAP;
#جيڪڏهن_وصف_ٿيل HAVE_داخلا_طريقو
	im_سيٽ_سرگرم(FALSE);	// Disable input method
#جيڪڏهن_پورو
	إذا (b_im_ptr != عدم)
	{
	    إذا (State & گهڻائي_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    وإلا
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#جيڪڏهن_وصف_ٿيل HAVE_داخلا_طريقو
    وإلا
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	إذا ((p_imdisable && b_im_ptr != عدم)
		? *b_im_ptr == B_IMODE_IM : im_get_حالت())
	{
	    im_سيٽ_سرگرم(FALSE);	// Disable input method
	    إذا (b_im_ptr != عدم)
		*b_im_ptr = B_IMODE_NONE;
	}
	وإلا
	{
	    im_سيٽ_سرگرم(TRUE);	// Enable input method
	    إذا (b_im_ptr != عدم)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#جيڪڏهن_پورو
    إذا (b_im_ptr != عدم)
    {
	إذا (b_im_ptr == &curbuf->b_p_iminsert)
	    سيٽ_iminsert_global();
	وإلا
	    سيٽ_imsearch_global();
    }
#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#جيڪڏهن_پورو
#إذا defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    حالت_redraw_curbuf();
#جيڪڏهن_پورو
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ساكن صحيح
cmdline_وجهو_reg(صحيح *gotesc اڻ_استعمال_ٿيل)
{
    صحيح		i;
    صحيح		c;
    صحيح		literally = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		save_new_cmdpos = new_cmdpos;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#جيڪڏهن_پورو
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_ڪنجيون;
    i = c = plain_vgetc();	// CTRL-R <char>
    إذا (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    إذا (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_ڪنجيون;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    إذا (c == '=')
    {
	إذا (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_تارو > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	وإلا
	    c = get_اظهار_رجسٽر();
    }
#جيڪڏهن_پورو
    إذا (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
			|| (clip_تارو.موجود && c == '*')
			|| (clip_plus.موجود && c == '+')
#جيڪڏهن_پورو
			;
	cmdline_paste(c, literally, FALSE);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	إذا (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    أرجع GOTO_عام_گهڻائي;
	}
#جيڪڏهن_پورو
	ڪنجي_Typed = FALSE;	// Don't do p_wc completion.
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    إذا (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    وإلا
		ccline.cmdpos = new_cmdpos;
	}
#جيڪڏهن_پورو
    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#جيڪڏهن_پورو

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    أرجع literally ? CMDLINE_بدليو : CMDLINE_NOT_بدليو;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ساكن فراغ
cmdline_کاٻو_ساڄو_mouse(صحيح c, صحيح *ignore_drag_release)
{
    إذا (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    وإلا
	*ignore_drag_release = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    إذا (!gui.اندريون_use)
#جيڪڏهن_پورو
	إذا (!mouse_has(MOUSE_COMMAND))
	    أرجع;
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    إذا (mouse_صف < cmdline_صف && (clip_تارو.موجود || clip_plus.موجود))
    {
	صحيح	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	إذا (mouse_model_popup() && button == MOUSE_کاٻو
		&& (mod_نقاب & MOD_نقاب_سرڪايو))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_ساڄو;
	    mod_نقاب &= ~MOD_نقاب_سرڪايو;
	}
	clip_modeless(button, is_click, is_drag);
	أرجع;
    }
#جيڪڏهن_پورو

    سيٽ_cmdspos();
    لكل (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	صحيح	i;

	i = cmdline_charsize(ccline.cmdpos);
	إذا (mouse_صف <= cmdline_صف + ccline.cmdspos / cmdline_ويڪر
		&& mouse_کانو < ccline.cmdspos % cmdline_ويڪر + i)
	    اكسر;
	إذا (has_mbyte)
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

    إذا (get_hislen() == 0 || firstc == NUL)	// no history
	أرجع CMDLINE_NOT_بدليو;

    // save current command string so it can be restored later
    إذا (lookfor == عدم)
    {
	إذا ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == عدم)
	    أرجع CMDLINE_NOT_بدليو;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    لكل (;;)
    {
	// one step backwards
	إذا (c == K_مٿي || c == K_S_مٿي || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    إذا (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    وإلا إذا (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    وإلا إذا (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    وإلا			// at top of list
	    {
		hiscnt = orig_hiscnt;
		اكسر;
	    }
	}
	وإلا    // one step forwards
	{
	    // on last entry, clear the line
	    إذا (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		اكسر;
	    }

	    // not on a history line, nothing to do
	    إذا (hiscnt == get_hislen())
		اكسر;
	    إذا (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    وإلا
		++hiscnt;
	}
	إذا (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== عدم)
	{
	    hiscnt = orig_hiscnt;
	    اكسر;
	}
	إذا ((c != K_مٿي && c != K_هيٺ_ڏانهن)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    اكسر;
    }

    إذا (hiscnt != orig_hiscnt)	// jumped to other entry
    {
	char_u	*p;
	size_t	plen;
	صحيح	old_firstc;

	dealloc_cmdbuff();

	xp->xp_context = EXPAND_NOTHING;
	إذا (hiscnt == get_hislen())
	{
	    p = lookfor;	// back to the old one
	    plen = lookforlen;
	}
	وإلا
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	إذا (histype == HIST_ڳولا
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
		    إذا (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			إذا (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    وإلا
		    {
			// Escape new sep, unless it is already
			// escaped.
			إذا (p[j] == firstc
				&& (j == 0 || p[j - 1] != '\\'))
			{
			    إذا (i > 0)
				ccline.cmdbuff[len] = '\\';
			    ++len;
			}
			إذا (i > 0)
			    ccline.cmdbuff[len] = p[j];
		    }
		    ++len;
		}
		إذا (i == 0)
		{
		    alloc_cmdbuff(len);
		    إذا (ccline.cmdbuff == عدم)
		    {
			res = GOTO_عام_گهڻائي;
			اذهب پورو;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (صحيح)len;
	}
	وإلا
	{
	    alloc_cmdbuff(plen);
	    إذا (ccline.cmdbuff == عدم)
	    {
		res = GOTO_عام_گهڻائي;
		اذهب پورو;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (صحيح)plen;
	}

	redrawcmd();
	res = CMDLINE_بدليو;
	اذهب پورو;
    }
    beep_flush();
    res = CMDLINE_NOT_بدليو;

پورو:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    أرجع res;
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
    إذا (ccline.cmdbuff == عدم)
    {
	ccline.cmdbufflen = 0;
	أرجع FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_متن_شروعات_cmdline();

    // autoindent for :insert and :append
    إذا (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    أرجع OK;
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
    ڊگهو	  ڳڻپ,	// only used for incremental search
    صحيح		  indent,	// indent for inside conditionals
    getline_opt_T do_concat اڻ_استعمال_ٿيل)
{
    أرجع getcmdline_int(firstc, ڳڻپ, indent, TRUE);
}

    ساكن char_u *
getcmdline_int(
    صحيح		firstc,
    ڊگهو	ڳڻپ اڻ_استعمال_ٿيل,	// only used for incremental search
    صحيح		indent,		// indent for inside conditionals
    صحيح		clear_ccline)	// clear ccline first
{
    ساكن صحيح	اونهائي = 0;	    // call depth
    صحيح		c = 0;
    صحيح		i;
    صحيح		j;
    صحيح		gotesc = FALSE;		// TRUE when <ESC> just typed
    صحيح		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = عدم;	// string to match
    size_t	lookforlen = 0;
    صحيح		hiscnt;			// current history line in use
    صحيح		histype;		// history type to be used
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
    incsearch_state_T	is_state;
#جيڪڏهن_پورو
    صحيح		did_wild_list = FALSE;	// did wild_list() recently
    صحيح		wim_اشاريو = 0;		// index in wim_flags[]
    صحيح		res;
    صحيح		save_نياپو_scroll = نياپو_scroll;
    صحيح		save_State = State;	// remember State when called
    صحيح		some_ڪنجي_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    صحيح		ignore_drag_release = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		break_ctrl_c = FALSE;
#جيڪڏهن_پورو
    expand_T	xpc;
    ڊگهو	*b_im_ptr = عدم;
    buf_T	*b_im_ptr_buf = عدم;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    صحيح		did_save_ccline = FALSE;
    صحيح		cmdline_type;
    صحيح		wild_type = 0;
    صحيح		event_cmdlineleavepre_triggered = FALSE;
    char_u	*prev_cmdbuff = عدم;
    صحيح		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++اونهائي;

    إذا (ccline.cmdbuff != عدم)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    إذا (clear_ccline)
	CLEAR_ميدان(ccline);

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    إذا (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
    init_incsearch_state(&is_state);
#جيڪڏهن_پورو

    إذا (init_ccline(firstc, indent) != OK)
	اذهب theend;	// out of memory

    إذا (اونهائي == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	اذهب theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    وإلا
	cmdmsg_rl = FALSE;
#جيڪڏهن_پورو

    redir_off = TRUE;		// don't redirect the typed command
    إذا (!cmd_silent)
    {
	i = نياپو_scrolled;
	نياپو_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	نياپو_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	سيٽ_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#جيڪڏهن_وصف_نه_ٿيل BACKSLASH_اندريون_FILENAME
    xpc.xp_shell = FALSE;
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
    إذا (ccline.داخلا_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_نمونو = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#جيڪڏهن_پورو

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    نياپو_scroll = FALSE;

    State = گهڻائي_CMDLINE;

    إذا (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	إذا (curbuf->b_p_imsearch == B_IMODE_USE_وجهو)
	    b_im_ptr = &curbuf->b_p_iminsert;
	وإلا
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	إذا (*b_im_ptr == B_IMODE_LMAP)
	    State |= گهڻائي_LANGMAP;
#جيڪڏهن_وصف_ٿيل HAVE_داخلا_طريقو
	im_سيٽ_سرگرم(*b_im_ptr == B_IMODE_IM);
#جيڪڏهن_پورو
    }
#جيڪڏهن_وصف_ٿيل HAVE_داخلا_طريقو
    وإلا إذا (p_imcmdline)
	im_سيٽ_سرگرم(TRUE);
#جيڪڏهن_پورو

    setmouse();
#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#جيڪڏهن_پورو

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (!debug_گهڻائي)
	may_trigger_modechanged();
#جيڪڏهن_پورو

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#جيڪڏهن_پورو

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    إذا (did_emsg)
	redrawcmd();

#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    إذا (!cmd_silent && نياپو_scrolled == 0)
    {
	صحيح	مليو_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    إذا (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_حالت = true;
		مليو_one = TRUE;
	    }

	إذا (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    مليو_one = TRUE;
	}

	إذا (مليو_one)
	    redraw_statuslines();
    }
#جيڪڏهن_پورو

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    لكل (;;)
    {
	صحيح	trigger_cmdlinechanged = TRUE;
	صحيح	پڇاڙي_wildmenu;
	صحيح	prev_cmdpos = ccline.cmdpos;
	صحيح	skip_pum_redraw = FALSE;

	VIM_CLEAR(prev_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#جيڪڏهن_وصف_ٿيل USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#جيڪڏهن_پورو
	quit_وڌيڪ = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	إذا (ex_عام_busy == 0 && stuff_خالي_جايون() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_ڪنجي_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	إذا (ccline.cmdbuff != عدم)
	{
	    prev_cmdbuff = vim_strsave(ccline.cmdbuff);
	    إذا (prev_cmdbuff == عدم)
		اذهب returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	إذا (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	افعل
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = محفوظ_vgetc();
	} طالما (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	إذا (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    إذا (cmdline_pum_سرگرم())
		cmdline_pum_remove(&ccline, FALSE);
	    (فراغ)ExpandOne(&xpc, عدم, عدم, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_اشاريو = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	إذا (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    استمر;
	}

	إذا (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    صحيح	    clen = ccline.cmdlen;
	    صحيح	    cc_ڳڻپ = aucmd_cmdline_بدليو_ڳڻپ;

	    إذا (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		إذا (clen == ccline.cmdlen
				    || cc_ڳڻپ != aucmd_cmdline_بدليو_ڳڻپ)
		    trigger_cmdlinechanged = FALSE;
		اذهب cmdline_بدليو;
	    }
	}

	إذا (ڪنجي_Typed)
	{
	    some_ڪنجي_typed = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	    إذا (cmd_hkmap)
		c = hkmap(c);
	    إذا (cmdmsg_rl && !ڪنجي_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		بدل (c)
		{
		    حالة K_ساڄو:   c = K_کاٻو; اكسر;
		    حالة K_S_ساڄو: c = K_S_کاٻو; اكسر;
		    حالة K_C_ساڄو: c = K_C_کاٻو; اكسر;
		    حالة K_کاٻو:    c = K_ساڄو; اكسر;
		    حالة K_S_کاٻو:  c = K_S_ساڄو; اكسر;
		    حالة K_C_کاٻو:  c = K_C_ساڄو; اكسر;
		}
	    }
#جيڪڏهن_پورو
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	إذا ((c == Ctrl_C
#جيڪڏهن_وصف_ٿيل UNIX
		|| c == intr_char
#جيڪڏهن_پورو
				)
#إذا defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_سرگرم)
#جيڪڏهن_پورو
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	إذا (lookfor != عدم
		&& c != K_S_هيٺ_ڏانهن && c != K_S_مٿي
		&& c != K_هيٺ_ڏانهن && c != K_مٿي
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_کاٻو && c != K_ساڄو
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    VIM_CLEAR(lookfor);
	    lookforlen = 0;
	}

	/*
	 * When there are matching completions to select <S-Tab> works like
	 * CTRL-P (unless 'wc' is <S-Tab>).
	 */
	إذا (c != p_wc && c == K_S_TAB && xpc.xp_numfiles > 0)
	    c = Ctrl_P;

	إذا (p_wmnu)
	    c = wildmenu_translate_ڪنجي(&ccline, c, &xpc, did_wild_list);

	صحيح ڪنجي_is_wc = (c == p_wc && ڪنجي_Typed) || c == p_wcm;
	إذا ((cmdline_pum_سرگرم() || wild_menu_showing || did_wild_list)
		&& !ڪنجي_is_wc && xpc.xp_numfiles > 0)
	{
	    // Ctrl-Y: Accept the current selection and close the popup menu.
	    // Ctrl-E: cancel the cmdline popup menu and return the original
	    // text.
	    إذا (c == Ctrl_E || c == Ctrl_Y)
	    {
		wild_type = (c == Ctrl_E) ? WILD_CANCEL : WILD_APPLY;
		إذا (nextwild(&xpc, wild_type, WILD_NO_BEEP,
							firstc != '@') == FAIL)
		    اكسر;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	إذا (ڪنجي_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#جيڪڏهن_وصف_ٿيل UNIX
		    || c == intr_char
#جيڪڏهن_پورو
		    || c == Ctrl_C))
	{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    سيٽ_vim_var_char(c);  // Set v:char
#جيڪڏهن_پورو
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#إذا defined(FEAT_ڳولا_EXTRA)
	    إذا ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		سيٽ_no_hlsearch(TRUE);
#جيڪڏهن_پورو
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	پڇاڙي_wildmenu = (!ڪنجي_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	پڇاڙي_wildmenu = پڇاڙي_wildmenu && (!cmdline_pum_سرگرم() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_مٿي || c == K_S_هيٺ_ڏانهن)
				 && (mod_نقاب & MOD_نقاب_CTRL))));

	// free expanded names when finished walking through matches
	إذا (پڇاڙي_wildmenu)
	{
	    إذا (cmdline_pum_سرگرم())
	    {
		skip_pum_redraw = skip_pum_redraw && !ڪنجي_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    إذا (xpc.xp_numfiles != -1)
		(فراغ)ExpandOne(&xpc, عدم, عدم, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    إذا (!p_wmnu || (c != K_مٿي && c != K_هيٺ_ڏانهن))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_اشاريو = 0;
	    wildmenu_cleanup(&ccline);
	}

	إذا (p_wmnu)
	    c = wildmenu_process_ڪنجي(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	إذا (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    إذا (res == CMDLINE_بدليو)
		اذهب cmdline_بدليو;
	    وإلا إذا (res == CMDLINE_NOT_بدليو)
		اذهب cmdline_not_بدليو;
	    وإلا إذا (res == GOTO_عام_گهڻائي)
		اذهب returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	إذا (c == cedit_ڪنجي || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    إذا ((c == K_CMDWIN || ex_عام_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_ڪنجي_typed = TRUE;
	    }
	}
#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
	وإلا
	    c = do_digraph(c);
#جيڪڏهن_پورو

	إذا (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!ڪنجي_Typed || vim_strchr(p_cpo, CPO_ESC) != عدم)))
	{
	    // In Ex mode a backslash escapes a newline.
	    إذا (exmode_سرگرم
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		إذا (c == K_KENTER)
		    c = '\n';
	    }
	    وإلا
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		إذا (ccheck_abbr(c + ABBR_OFF))
		    اذهب cmdline_بدليو;
		إذا (!cmd_silent)
		{
		    windgoto(نياپو_صف, cmdline_کانو_off);
		    ٻاهريون_flush();
		}
		اكسر;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	إذا ((c == p_wc && !gotesc && ڪنجي_Typed) || c == p_wcm || c == K_WILD)
	{
	    إذا (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_اشاريو, &xpc, &gotesc, c == K_WILD,
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
		    &is_state.ڳولا_شروعات_2
#وإلا
		    عدم
#جيڪڏهن_پورو
		    );
	    إذا (c == K_WILD)
		--emsg_silent;
	    إذا (res == CMDLINE_بدليو)
		اذهب cmdline_بدليو;
	    إذا (c == K_WILD)
		اذهب cmdline_not_بدليو;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	إذا (c == K_S_TAB && ڪنجي_Typed)
	{
	    إذا (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		إذا (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_اشاريو] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    صحيح list = wim_flags[wim_اشاريو] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		إذا (nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_PREV, 0, firstc != '@') == OK)
		    اذهب cmdline_بدليو;
	    }
	}

	إذا (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	إذا (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	    // Apply search highlighting
	    إذا (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    إذا (ڪنجي_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, ڳڻپ, &is_state);
#جيڪڏهن_پورو
	    wild_type = 0;
	    اذهب cmdline_not_بدليو;
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
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
		    , &is_state
#جيڪڏهن_پورو
		    );
	    إذا (res == CMDLINE_NOT_بدليو)
		اذهب cmdline_not_بدليو;
	    وإلا إذا (res == GOTO_عام_گهڻائي)
		اذهب returncmd;		// back to cmd mode
	    اذهب cmdline_بدليو;

	حالة K_INS:
	حالة K_KINS:
		ccline.overstrike = !ccline.overstrike;
#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#جيڪڏهن_پورو
		may_trigger_modechanged();
		حالت_redraw_curbuf();
		redraw_statuslines();
		اذهب cmdline_not_بدليو;

	حالة Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : عدم);
		اذهب cmdline_not_بدليو;

//	case '@':   only in very old vi
	حالة Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		طالما (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
		إذا (ccline.cmdlen == 0)
		    is_state.ڳولا_شروعات_2 = is_state.save_cursor;
#جيڪڏهن_پورو
		redrawcmd();
		اذهب cmdline_بدليو;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
	حالة Ctrl_Y:
		// Copy the modeless selection, if there is one.
		إذا (clip_تارو.state != SELECT_CLEARED)
		{
		    إذا (clip_تارو.state == SELECT_پورو)
			clip_copy_modeless_selection(TRUE);
		    اذهب cmdline_not_بدليو;
		}
		اكسر;
#جيڪڏهن_پورو

	حالة ESC:	// get here if p_wc != ESC or when ESC typed twice
	حالة Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		إذا (exmode_سرگرم
			       && (ex_عام_busy == 0 || typebuf.tb_len > 0))
		    اذهب cmdline_not_بدليو;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		اذهب returncmd;		// back to cmd mode

	حالة Ctrl_R:			// insert register
		res = cmdline_وجهو_reg(&gotesc);
		إذا (res == GOTO_عام_گهڻائي)
		    اذهب returncmd;
		إذا (res == CMDLINE_بدليو)
		    اذهب cmdline_بدليو;
		اذهب cmdline_not_بدليو;

	حالة Ctrl_D:
		إذا (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    اكسر;	// Use ^D as normal char instead

		redrawcmd();
		استمر;	// don't do incremental search now

	حالة K_ساڄو:
	حالة K_S_ساڄو:
	حالة K_C_ساڄو:
		افعل
		{
		    إذا (ccline.cmdpos >= ccline.cmdlen)
			اكسر;
		    i = cmdline_charsize(ccline.cmdpos);
		    إذا (ڪنجي_Typed && ccline.cmdspos + i >= cmdline_ويڪر * صفون)
			اكسر;
		    ccline.cmdspos += i;
		    إذا (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    وإلا
			++ccline.cmdpos;
		}
		طالما ((c == K_S_ساڄو || c == K_C_ساڄو
			       || (mod_نقاب & (MOD_نقاب_سرڪايو|MOD_نقاب_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		إذا (has_mbyte)
		    سيٽ_cmdspos_cursor();
		اذهب cmdline_not_بدليو;

	حالة K_کاٻو:
	حالة K_S_کاٻو:
	حالة K_C_کاٻو:
		إذا (ccline.cmdpos == 0)
		    اذهب cmdline_not_بدليو;
		افعل
		{
		    --ccline.cmdpos;
		    إذا (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_سر_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		طالما (ccline.cmdpos > 0
			&& (c == K_S_کاٻو || c == K_C_کاٻو
			       || (mod_نقاب & (MOD_نقاب_سرڪايو|MOD_نقاب_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		إذا (has_mbyte)
		    سيٽ_cmdspos_cursor();
		اذهب cmdline_not_بدليو;

	حالة K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		اذهب cmdline_not_بدليو;

#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالة K_F4:
	    إذا (mod_نقاب == MOD_نقاب_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		اذهب cmdline_not_بدليو;
	    }
	    اكسر;
#جيڪڏهن_پورو

	حالة K_MIDDLEDRAG:
	حالة K_MIDDLERELEASE:
		اذهب cmdline_not_بدليو;	// Ignore mouse

	حالة K_MIDDLEMOUSE:
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		إذا (!gui.اندريون_use)
#جيڪڏهن_پورو
		    إذا (!mouse_has(MOUSE_COMMAND))
			اذهب cmdline_not_بدليو;   // Ignore mouse
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
		إذا (clip_تارو.موجود)
		    cmdline_paste('*', TRUE, TRUE);
		وإلا
#جيڪڏهن_پورو
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		اذهب cmdline_بدليو;

#جيڪڏهن_وصف_ٿيل FEAT_DND
	حالة K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		اذهب cmdline_بدليو;
#جيڪڏهن_پورو

	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		إذا (ignore_drag_release)
		    اذهب cmdline_not_بدليو;
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		cmdline_کاٻو_ساڄو_mouse(c, &ignore_drag_release);
		اذهب cmdline_not_بدليو;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	حالة K_MOUSEDOWN:
	حالة K_MOUSEUP:
	حالة K_MOUSELEFT:
	حالة K_MOUSERIGHT:
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		إذا (cmdline_pum_سرگرم())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_هيٺ_ڏانهن
				      : c == K_MOUSEUP ? MSCR_مٿي
				      : c == K_MOUSELEFT ? MSCR_کاٻو
				      : MSCR_ساڄو);
#جيڪڏهن_پورو
		اذهب cmdline_not_بدليو;

	// Alternate buttons ignored here
	حالة K_X1MOUSE:
	حالة K_X1DRAG:
	حالة K_X1RELEASE:
	حالة K_X2MOUSE:
	حالة K_X2DRAG:
	حالة K_X2RELEASE:
	حالة K_MOUSEMOVE:
		اذهب cmdline_not_بدليو;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
	حالة K_LEFTMOUSE_NM:	// mousefocus click, ignored
	حالة K_LEFTRELEASE_NM:
		اذهب cmdline_not_بدليو;

	حالة K_VER_SCROLLBAR:
		إذا (نياپو_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		اذهب cmdline_not_بدليو;

	حالة K_HOR_SCROLLBAR:
		إذا (نياپو_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_قدر);
		    redrawcmd();
		}
		اذهب cmdline_not_بدليو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
	حالة K_TABLINE:
	حالة K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		إذا (gui_use_tabline())
		    gui_mch_سيٽ_curtab(tabpage_اشاريو(curtab));
		اذهب cmdline_not_بدليو;
#جيڪڏهن_پورو

	حالة K_SELECT:	    // end of Select mode mapping - ignore
		اذهب cmdline_not_بدليو;

	حالة Ctrl_B:	    // begin of command line
	حالة K_HOME:
	حالة K_KHOME:
	حالة K_S_HOME:
	حالة K_C_HOME:
		ccline.cmdpos = 0;
		سيٽ_cmdspos();
		اذهب cmdline_not_بدليو;

	حالة Ctrl_E:	    // end of command line
	حالة K_پڇاڙي:
	حالة K_KEND:
	حالة K_S_پڇاڙي:
	حالة K_C_پڇاڙي:
		ccline.cmdpos = ccline.cmdlen;
		سيٽ_cmdspos_cursor();
		اذهب cmdline_not_بدليو;

	حالة Ctrl_A:	    // all matches
		إذا (cmdline_pum_سرگرم())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		إذا (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    اكسر;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		اذهب cmdline_بدليو;

	حالة Ctrl_L:
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
		إذا (may_جوڙيو_char_منزل_ڳولا(firstc, &c, &is_state) == OK)
		    اذهب cmdline_not_بدليو;
#جيڪڏهن_پورو

		// completion: longest common part
		إذا (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    اكسر;
		اذهب cmdline_بدليو;

	حالة Ctrl_N:	    // next match
	حالة Ctrl_P:	    // previous match
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		إذا (cmdline_pum_سرگرم() && (mod_نقاب & MOD_نقاب_سرڪايو))
		{
		    إذا (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    اذهب cmdline_not_بدليو;
		}
#جيڪڏهن_پورو
		إذا (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_PREV : WILD_ايندڙ;
		    إذا (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			اكسر;
		    اذهب cmdline_بدليو;
		}
		// FALLTHROUGH
	حالة K_مٿي:
	حالة K_هيٺ_ڏانهن:
	حالة K_S_مٿي:
	حالة K_S_هيٺ_ڏانهن:
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		إذا (cmdline_pum_سرگرم()
			&& ((c == K_S_مٿي || c == K_S_هيٺ_ڏانهن)
			    ? (mod_نقاب & MOD_نقاب_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_نقاب & MOD_نقاب_CTRL)
				&& (mod_نقاب & MOD_نقاب_سرڪايو))))
		{
		    صحيح	مٿي = c == K_S_مٿي || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    إذا (popup_scroll_info(مٿي ? -1 : 1,
					     c != K_S_مٿي && c != K_S_هيٺ_ڏانهن))
			cmdline_pum_display();
		    اذهب cmdline_not_بدليو;
		}
#جيڪڏهن_پورو
		إذا (cmdline_pum_سرگرم()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    إذا (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    إذا (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			اكسر;
		    اذهب cmdline_بدليو;
		}
		وإلا
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    إذا (res == CMDLINE_بدليو)
		    {
			did_hist_navigate = TRUE;
			اذهب cmdline_بدليو;
		    }
		    وإلا إذا (res == GOTO_عام_گهڻائي)
			اذهب returncmd;
		}
		اذهب cmdline_not_بدليو;

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	حالة Ctrl_G:	    // next match
	حالة Ctrl_T:	    // previous match
		إذا (may_adjust_incsearch_highlighting(
					  firstc, ڳڻپ, &is_state, c) == FAIL)
		    اذهب cmdline_not_بدليو;
		اكسر;
#جيڪڏهن_پورو

	حالة Ctrl_V:
	حالة Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_نقاب & MOD_نقاب_سرڪايو);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    إذا (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			نياپو_putchar(' ');
			cursorcmd();
		    }
		}

		اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
	حالة Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		إذا (c != NUL)
		    اكسر;

		redrawcmd();
		اذهب cmdline_not_بدليو;
#جيڪڏهن_پورو // FEAT_DIGRAPHS

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	حالة Ctrl__:	    // CTRL-_: switch language mode
		إذا (!p_ari)
		    اكسر;
		cmd_hkmap = !cmd_hkmap;
		اذهب cmdline_not_بدليو;
#جيڪڏهن_پورو

	حالة K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, عدم);
		اذهب cmdline_بدليو;

	افتراضي:
#جيڪڏهن_وصف_ٿيل UNIX
		إذا (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    اذهب returncmd;	// back to Normal mode
		}
#جيڪڏهن_پورو
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		إذا (!IS_SPECIAL(c))
		    mod_نقاب = 0x0;
		اكسر;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	إذا (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c))
		&& (ccheck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
		    || c == Ctrl_RSB))
	    اذهب cmdline_بدليو;

	/*
	 * put the character in the command line
	 */
	إذا (IS_SPECIAL(c) || mod_نقاب != 0)
	    put_on_cmdline(get_special_ڪنجي_name(c, mod_نقاب), -1, TRUE);
	وإلا
	{
	    إذا (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    وإلا
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	اذهب cmdline_بدليو;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_بدليو:
	// Trigger CursorMovedC autocommands.
	إذا (ccline.cmdpos != prev_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    prev_cmdpos = ccline.cmdpos;
	}

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	إذا (!is_state.incsearch_postponed)
	    استمر;
#جيڪڏهن_پورو

cmdline_بدليو:
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	// If the window changed incremental search state is not valid.
	إذا (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	إذا (xpc.xp_context == EXPAND_NOTHING && (ڪنجي_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, ڳڻپ, &is_state);
#جيڪڏهن_پورو
	// Trigger CmdlineChanged autocommands.
	إذا (trigger_cmdlinechanged
		&& (ccline.cmdpos != prev_cmdpos
		    || (prev_cmdbuff != عدم &&
			STRCMP(prev_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    إذا (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#جيڪڏهن_پورو
	}

	// Trigger CursorMovedC autocommands.
	إذا (ccline.cmdpos != prev_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	إذا (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| (p_arshape && !p_tbidi
				       && cmdline_has_arabic(0, ccline.cmdlen))
# endif
		)
	    // Always redraw the whole command line to fix shaping and
	    // right-left typing.  Not efficient, but it works.
	    // Do it only when there are no characters left to read
	    // to avoid useless intermediate redraws.
	    إذا (vpeekc() == NUL)
		redrawcmd();
#جيڪڏهن_پورو
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    إذا (!event_cmdlineleavepre_triggered)
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	سيٽ_vim_var_char(c);  // Set v:char
#جيڪڏهن_پورو
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#جيڪڏهن_پورو

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    إذا (cmdline_pum_سرگرم())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_اشاريو = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = عدم;
    clear_cmdline_orig();

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
    پڄاڻي_incsearch_highlighting(gotesc, &is_state, FALSE);
#جيڪڏهن_پورو

    إذا (ccline.cmdbuff != عدم)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	إذا (ccline.cmdlen && firstc != NUL
		&& (some_ڪنجي_typed || histype == HIST_ڳولا))
	{
	    جوڙيو_منزل_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_ڳولا ? firstc : NUL);
	    إذا (firstc == ':')
	    {
		vim_free(new_آخري_cmdline);
		new_آخري_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
	    }
	}

	إذا (gotesc)
	    abandon_cmdline();
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    نياپو_check();
    نياپو_scroll = save_نياپو_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    إذا (some_ڪنجي_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    سيٽ_vim_var_char(c);  // Set v:char
#جيڪڏهن_پورو
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (!debug_گهڻائي)
	may_trigger_modechanged();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_داخلا_طريقو
    إذا (b_im_ptr != عدم && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_حالت(b_im_ptr);
    im_سيٽ_سرگرم(FALSE);
#جيڪڏهن_پورو
    setmouse();
#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#جيڪڏهن_پورو
    sb_متن_پڇاڙي_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--اونهائي;
	إذا (did_save_ccline)
	    restore_cmdline(&save_ccline);
	وإلا
	    ccline.cmdbuff = عدم;

	vim_free(prev_cmdbuff);
	أرجع p;
    }
}

#إذا defined(FEAT_CRYPT) || defined(FEAT_EVAL)
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
    صحيح			نياپو_کانو_save = نياپو_کانو;
    صحيح			نياپو_silent_save = نياپو_silent;

    إذا (ccline.cmdbuff != عدم)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_ميدان(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.داخلا_fn = (firstc == '@');
# endif
    نياپو_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    إذا (did_save_ccline)
	restore_cmdline(&save_ccline);

    نياپو_silent = نياپو_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    إذا (ccline.cmdbuff != عدم)
	نياپو_کانو = نياپو_کانو_save;

    أرجع s;
}
#جيڪڏهن_پورو

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
	إذا (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    أرجع FAIL;
	إذا (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	وإلا إذا (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_ڀريل;
	وإلا إذا (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	وإلا إذا (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	وإلا إذا (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	وإلا إذا (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	وإلا
	    أرجع FAIL;
	p += i;
	إذا (*p == NUL)
	    اكسر;
	إذا (*p == ',')
	{
	    إذا (idx == 3)
		أرجع FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    طالما (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    لكل (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    أرجع OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    صحيح
متن_locked(فراغ)
{
    إذا (cmdwin_type != 0)
	أرجع TRUE;
    أرجع textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    فراغ
متن_locked_نياپو(فراغ)
{
    emsg(_(get_متن_locked_نياپو()));
}

    حرف *
get_متن_locked_نياپو(فراغ)
{
    إذا (cmdwin_type != 0)
	أرجع e_invalid_اندريون_cmdline_دري;
    أرجع e_not_allowed_منزل_change_متن_or_change_دري;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    صحيح
متن_or_buf_locked(فراغ)
{
    إذا (متن_locked())
    {
	متن_locked_نياپو();
	أرجع TRUE;
    }
    أرجع curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    صحيح
curbuf_locked(فراغ)
{
    إذا (curbuf_تالو > 0)
    {
	emsg(_(e_not_allowed_منزل_edit_another_بفر_now));
	أرجع TRUE;
    }
    أرجع allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    صحيح
allbuf_locked(فراغ)
{
    إذا (allbuf_تالو > 0)
    {
	emsg(_(e_not_allowed_منزل_change_بفر_information_now));
	أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن صحيح
cmdline_charsize(صحيح idx)
{
#إذا defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    إذا (cmdline_تارو > 0)	    // showing '*', always 1 position
	أرجع 1;
#جيڪڏهن_پورو
    أرجع ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ساكن فراغ
سيٽ_cmdspos(فراغ)
{
    إذا (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    وإلا
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ساكن فراغ
سيٽ_cmdspos_cursor(فراغ)
{
    صحيح		i, m, c;

    سيٽ_cmdspos();
    إذا (ڪنجي_Typed)
    {
	m = cmdline_ويڪر * صفون;
	إذا (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    وإلا
	m = MAXCOL;
    لكل (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
    {
	c = cmdline_charsize(i);
	// Count ">" for double-wide multi-byte char that doesn't fit.
	إذا (has_mbyte)
	    correct_cmdspos(i, c);
	// If the cmdline doesn't fit, show cursor on last visible char.
	// Don't move the cursor itself, so we can still append.
	إذا ((ccline.cmdspos += c) >= m)
	{
	    ccline.cmdspos -= c;
	    اكسر;
	}
	إذا (has_mbyte)
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
    إذا ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_ويڪر + cells > cmdline_ويڪر)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    صحيح		c,		// normally ':', NUL for ":append"
    فراغ	*cookie اڻ_استعمال_ٿيل,
    صحيح		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    إذا (exec_from_reg && vpeekc() == ':')
	(فراغ)vgetc();
    أرجع getcmdline(c, 1L, indent, options);
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
    فراغ	*cookie اڻ_استعمال_ٿيل,
    صحيح		indent,		// indent for inside conditionals
    getline_opt_T options اڻ_استعمال_ٿيل)
{
    garray_T	سٽ_ga;
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
    إذا ((نياپو_کانو || نياپو_didout) && promptc != '?')
	نياپو_putchar('\n');
    إذا (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	إذا (p_prompt)
	    نياپو_putchar(':');
	طالما (indent-- > 0)
	    نياپو_putchar(' ');
	startcol = نياپو_کانو;
    }

    ga_init2(&سٽ_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    إذا (promptc <= 0)
    {
	vcol = indent;
	طالما (indent >= 8)
	{
	    ga_append(&سٽ_ga, TAB);
	    نياپو_puts("        ");
	    indent -= 8;
	}
	طالما (indent-- > 0)
	{
	    ga_append(&سٽ_ga, ' ');
	    نياپو_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_ڪنجيون;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    طالما (!got_int)
    {
	ڊگهو    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	إذا (ga_grow(&سٽ_ga, 40) == FAIL)
	    اكسر;

	/*
	 * Get one character at a time.
	 */
	prev_char = c1;

	// Check for a ":normal" command and no more characters left.
	إذا (ex_عام_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	وإلا
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	إذا (got_int)
	{
	    نياپو_putchar('\n');
	    اكسر;
	}

	إذا (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &سٽ_ga);
	    اذهب redraw;
	}

	إذا (!escaped)
	{
	    // CR typed means "enter", which is NL
	    إذا (c1 == '\r')
		c1 = '\n';

	    إذا (c1 == BS || c1 == K_BS
			  || c1 == DEL || c1 == K_DEL || c1 == K_KDEL)
	    {
		إذا (سٽ_ga.ga_len > 0)
		{
		    إذا (has_mbyte)
		    {
			p = (char_u *)سٽ_ga.ga_معلومات;
			p[سٽ_ga.ga_len] = NUL;
			len = (*mb_سر_off)(p, p + سٽ_ga.ga_len - 1) + 1;
			سٽ_ga.ga_len -= len;
		    }
		    وإلا
			--سٽ_ga.ga_len;
		    اذهب redraw;
		}
		استمر;
	    }

	    إذا (c1 == Ctrl_U)
	    {
		نياپو_کانو = startcol;
		نياپو_clr_eos();
		سٽ_ga.ga_len = 0;
		اذهب redraw;
	    }

	    إذا (c1 == Ctrl_T)
	    {
		sw = get_sw_قدر(curbuf);
		p = (char_u *)سٽ_ga.ga_معلومات;
		p[سٽ_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
جوڙيو_indent:
		طالما (get_indent_str(p, 8, FALSE) < indent)
		{
		    (فراغ)ga_grow(&سٽ_ga, 2);  // one more for the NUL
		    p = (char_u *)سٽ_ga.ga_معلومات;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, سٽ_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++سٽ_ga.ga_len;
		}
redraw:
		// redraw the line
		نياپو_کانو = startcol;
		vcol = 0;
		p = (char_u *)سٽ_ga.ga_معلومات;
		p[سٽ_ga.ga_len] = NUL;
		طالما (p < (char_u *)سٽ_ga.ga_معلومات + سٽ_ga.ga_len)
		{
		    إذا (*p == TAB)
		    {
			افعل
			    نياپو_putchar(' ');
			طالما (++vcol % 8);
			++p;
		    }
		    وإلا
		    {
			len = mb_ptr2len(p);
			نياپو_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		نياپو_clr_eos();
		windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
		استمر;
	    }

	    إذا (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)سٽ_ga.ga_معلومات;
		إذا (prev_char == '0' || prev_char == '^')
		{
		    إذا (prev_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--سٽ_ga.ga_len] = NUL;
		}
		وإلا
		{
		    p[سٽ_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    إذا (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_قدر(curbuf);
		    }
		}
		طالما (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, سٽ_ga.ga_len - (s - p) + 1);
		    --سٽ_ga.ga_len;
		}
		اذهب جوڙيو_indent;
	    }

	    إذا (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		استمر;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    إذا (IS_SPECIAL(c1))
		استمر;
	}

	إذا (IS_SPECIAL(c1))
	    c1 = '?';
	إذا (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)سٽ_ga.ga_معلومات + سٽ_ga.ga_len);
	وإلا
	{
	    len = 1;
	    ((char_u *)سٽ_ga.ga_معلومات)[سٽ_ga.ga_len] = c1;
	}
	إذا (c1 == '\n')
	    نياپو_putchar('\n');
	وإلا إذا (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    افعل
		نياپو_putchar(' ');
	    طالما (++vcol % 8);
	}
	وإلا
	{
	    نياپو_outtrans_len(
		     ((char_u *)سٽ_ga.ga_معلومات) + سٽ_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	سٽ_ga.ga_len += len;
	escaped = FALSE;

	windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
	pend = (char_u *)(سٽ_ga.ga_معلومات) + سٽ_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	إذا (سٽ_ga.ga_len > 0 && pend[-1] == '\n'
		&& (سٽ_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --سٽ_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    اكسر;
	}
    }

    --no_mapping;
    --allow_ڪنجيون;

    // make following messages go to the next line
    نياپو_didout = FALSE;
    نياپو_کانو = 0;
    إذا (نياپو_صف < صفون - 1)
	++نياپو_صف;
    emsg_on_display = FALSE;		// don't want ui_delay()

    إذا (got_int)
	ga_clear(&سٽ_ga);

    أرجع (char_u *)سٽ_ga.ga_معلومات;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    صحيح
cmdline_overstrike(فراغ)
{
    أرجع ccline.overstrike;
}

#إذا defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    صحيح
cmdline_at_پڇاڙي(فراغ)
{
    أرجع (ccline.cmdpos >= ccline.cmdlen);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(فراغ)
{
    إذا (ccline.cmdbuff == عدم || ccline.cmdpos > ccline.cmdlen)
	أرجع MAXCOL;

    إذا (has_mbyte)
    {
	colnr_T	کانو;
	صحيح	i = 0;

	لكل (کانو = 0; i < ccline.cmdpos; ++کانو)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	أرجع کانو;
    }
    وإلا
	أرجع ccline.cmdpos;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ساكن فراغ
redrawcmd_preedit(فراغ)
{
    إذا ((State & گهڻائي_CMDLINE)
	    && xic != عدم
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	صحيح	cmdpos = 0;
	صحيح	cmdspos;
	صحيح	old_صف;
	صحيح	old_کانو;
	colnr_T	کانو;

	old_صف = نياپو_صف;
	old_کانو = نياپو_کانو;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	إذا (has_mbyte)
	{
	    لكل (کانو = 0; کانو < preedit_شروعات_کانو
			  && cmdpos < ccline.cmdlen; ++کانو)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	وإلا
	{
	    cmdspos += preedit_شروعات_کانو;
	    cmdpos  += preedit_شروعات_کانو;
	}

	نياپو_صف = cmdline_صف + (cmdspos / cmdline_ويڪر);
	نياپو_کانو = cmdspos % cmdline_ويڪر;
	إذا (نياپو_صف >= صفون)
	    نياپو_صف = صفون - 1;

	لكل (کانو = 0; cmdpos < ccline.cmdlen; ++کانو)
	{
	    صحيح char_len;
	    صحيح char_attr;

	    char_attr = im_get_feedback_attr(کانو);
	    إذا (char_attr < 0)
		اكسر; // end of preedit string

	    إذا (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    وإلا
		char_len = 1;

	    نياپو_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	نياپو_صف = old_صف;
	نياپو_کانو = old_کانو;
    }
}
#جيڪڏهن_پورو // FEAT_XIM && FEAT_GUI_GTK

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
    إذا (len < 80)
	len = 100;
    وإلا
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

    إذا (len < ccline.cmdbufflen)
	أرجع OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    إذا (ccline.cmdbuff == عدم)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	أرجع FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    إذا (ccline.xpc != عدم
	    && ccline.xpc->xp_نمونو != عدم
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	صحيح i = (صحيح)(ccline.xpc->xp_نمونو - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	إذا (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_نمونو = ccline.cmdbuff + i;
    }

    vim_free(p);

    أرجع OK;
}

#إذا defined(FEAT_ARABIC)
ساكن char_u	*arshape_buf = عدم;

# إذا defined(EXITFREE)
    فراغ
free_arshape_buf(فراغ)
{
    vim_free(arshape_buf);
}
# endif
#جيڪڏهن_پورو

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ساكن فراغ
draw_cmdline(صحيح شروعات, صحيح len)
{
#إذا defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    صحيح		i;

    إذا (cmdline_تارو > 0)
	لكل (i = 0; i < len; ++i)
	{
	    نياپو_putchar('*');
	    إذا (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + شروعات + i) - 1;
	}
    وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
	إذا (p_arshape && !p_tbidi && cmdline_has_arabic(شروعات, len))
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
	إذا (len * 2 + 2 > buflen)
	{
	    // Re-allocate the buffer.  We keep it around to avoid a lot of
	    // alloc()/free() calls.
	    vim_free(arshape_buf);
	    buflen = len * 2 + 2;
	    arshape_buf = alloc(buflen);
	    إذا (arshape_buf == عدم)
		أرجع;	// out of memory
	}

	إذا (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + شروعات)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	لكل (j = شروعات; j < شروعات + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, شروعات + len - j);
	    mb_l = utfc_ptr2len_len(p, شروعات + len - j);
	    إذا (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		إذا (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = prev_c;
		    pc1 = prev_c1;
		    prev_c1 = u8cc[0];
		    إذا (j + mb_l >= شروعات + len)
			nc = NUL;
		    وإلا
			nc = utf_ptr2char(p + mb_l);
		}
		وإلا
		{
		    // displaying from left to right
		    إذا (j + mb_l >= شروعات + len)
			pc = NUL;
		    وإلا
		    {
			صحيح	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      شروعات + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = prev_c;
		}
		prev_c = u8c;

		u8c = arabic_shape(u8c, عدم, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		إذا (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    إذا (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    وإلا
	    {
		prev_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	نياپو_outtrans_len(arshape_buf, newlen);
    }
    وإلا
#جيڪڏهن_پورو
	نياپو_outtrans_len(ccline.cmdbuff + شروعات, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    فراغ
putcmdline(صحيح c, صحيح سرڪايو)
{
    إذا (cmd_silent)
	أرجع;
    نياپو_no_وڌيڪ = TRUE;
    نياپو_putchar(c);
    إذا (سرڪايو)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    نياپو_no_وڌيڪ = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_سرڪايو = سرڪايو;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    فراغ
unputcmdline(فراغ)
{
    إذا (cmd_silent)
	أرجع;
    نياپو_no_وڌيڪ = TRUE;
    إذا (ccline.cmdlen == ccline.cmdpos)
	نياپو_putchar(' ');
    وإلا إذا (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    وإلا
	draw_cmdline(ccline.cmdpos, 1);
    نياپو_no_وڌيڪ = FALSE;
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

    إذا (len < 0)
	len = (صحيح)STRLEN(str);

    // Check if ccline.cmdbuff needs to be longer
    إذا (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	retval = realloc_cmdbuff(ccline.cmdlen + len + 1);
    وإلا
	retval = OK;
    إذا (retval == OK)
    {
	إذا (!ccline.overstrike)
	{
	    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	وإلا
	{
	    إذا (has_mbyte)
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
		إذا (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		وإلا
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    وإلا إذا (ccline.cmdpos + len > ccline.cmdlen)
		ccline.cmdlen = ccline.cmdpos + len;
	}
	mch_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	ccline.cmdbuff[ccline.cmdlen] = NUL;

	إذا (enc_utf8)
	{
	    // When the inserted text starts with a composing character,
	    // backup to the character before it.  There could be two of them.
	    i = 0;
	    c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    طالما (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_سر_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
	    إذا (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_سر_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		إذا (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		وإلا
		    i = 0;
	    }
#جيڪڏهن_پورو
	    إذا (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		نياپو_کانو -= i;
		إذا (نياپو_کانو < 0)
		{
		    نياپو_کانو += cmdline_ويڪر;
		    --نياپو_صف;
		}
	    }
	}

	إذا (redraw && !cmd_silent)
	{
	    نياپو_no_وڌيڪ = TRUE;
	    i = cmdline_صف;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    إذا (cmdline_صف != i || ccline.overstrike)
		نياپو_clr_eos();
	    نياپو_no_وڌيڪ = FALSE;
	}
	إذا (ڪنجي_Typed)
	{
	    m = cmdline_ويڪر * صفون;
	    إذا (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	وإلا
	    m = MAXCOL;
	لكل (i = 0; i < len; ++i)
	{
	    c = cmdline_charsize(ccline.cmdpos);
	    // count ">" for a double-wide char that doesn't fit.
	    إذا (has_mbyte)
		correct_cmdspos(ccline.cmdpos, c);
	    // Stop cursor at the end of the screen, but do increment the
	    // insert position, so that entering a very long command
	    // works, even though you can't see it.
	    إذا (ccline.cmdspos + c < m)
		ccline.cmdspos += c;

	    إذا (has_mbyte)
	    {
		c = (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos) - 1;
		إذا (c > len - i - 1)
		    c = len - i - 1;
		ccline.cmdpos += c;
		i += c;
	    }
	    ++ccline.cmdpos;
	}
    }
    إذا (redraw)
	نياپو_check();
    أرجع retval;
}

ساكن cmdline_info_T	prev_ccline;
ساكن صحيح		prev_ccline_استعمال_ٿيل = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ساكن فراغ
save_cmdline(cmdline_info_T *ccp)
{
    إذا (!prev_ccline_استعمال_ٿيل)
    {
	CLEAR_ميدان(prev_ccline);
	prev_ccline_استعمال_ٿيل = TRUE;
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
    ڊگهو		i;
    char_u		*arg;
    char_u		*p;
    صحيح			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    إذا (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	أرجع FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    سٽ_breakcheck();
    إذا (got_int)
	أرجع FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#جيڪڏهن_پورو

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    إذا (i)
    {
	// Got the value of a special register in "arg".
	إذا (arg == عدم)
	    أرجع FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	إذا (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    صحيح	    len;

	    // Locate start of last word in the cmd buffer.
	    لكل (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		إذا (has_mbyte)
		{
		    len = (*mb_سر_off)(ccline.cmdbuff, w - 1) + 1;
		    إذا (!vim_iswordc(mb_ptr2char(w - len)))
			اكسر;
		    w -= len;
		}
		وإلا
		{
		    إذا (!vim_iswordc(w[-1]))
			اكسر;
		    --w;
		}
	    }
	    len = (صحيح)((ccline.cmdbuff + ccline.cmdpos) - w);
	    إذا (p_ic ? STRNICMP(w, arg, len) == 0 : STRNCMP(w, arg, len) == 0)
		p += len;
	}

	cmdline_paste_str(p, literally);
	إذا (allocated)
	    vim_free(arg);
	أرجع OK;
    }

    أرجع cmdline_paste_reg(regname, literally, remcr);
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

    إذا (literally)
	put_on_cmdline(s, -1, TRUE);
    وإلا
	طالما (*s != NUL)
	{
	    cv = *s;
	    إذا (cv == Ctrl_V && s[1])
		++s;
	    إذا (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    وإلا
		c = *s++;
	    إذا (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#جيڪڏهن_وصف_ٿيل UNIX
		    || c == intr_char
#جيڪڏهن_پورو
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
    إذا (cmd_silent)
	أرجع;
    need_wait_return = FALSE;
    إذا (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ساكن فراغ
redrawcmdprompt(فراغ)
{
    صحيح		i;

    إذا (cmd_silent)
	أرجع;
    إذا (ccline.cmdfirstc != NUL)
	نياپو_putchar(ccline.cmdfirstc);
    إذا (ccline.cmdprompt != عدم)
    {
	نياپو_puts_attr((حرف *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = نياپو_کانو + (نياپو_صف - cmdline_صف) * cmdline_ويڪر;
	// do the reverse of set_cmdspos()
	إذا (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    وإلا
	لكل (i = ccline.cmdindent; i > 0; --i)
	    نياپو_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    فراغ
redrawcmd(فراغ)
{
    صحيح save_اندريون_echowindow = اندريون_echowindow;

    إذا (cmd_silent)
	أرجع;

    // when 'incsearch' is set there may be no command line while redrawing
    إذا (ccline.cmdbuff == عدم)
    {
	windgoto(cmdline_صف, cmdline_کانو_off);
	نياپو_clr_eos();
	أرجع;
    }

    // Do not put this in the message window.
    اندريون_echowindow = FALSE;

    sb_متن_restart_cmdline();
    نياپو_شروعات();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    نياپو_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    نياپو_no_وڌيڪ = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    نياپو_clr_eos();
    نياپو_no_وڌيڪ = FALSE;

    سيٽ_cmdspos_cursor();
    إذا (extra_char != NUL)
	putcmdline(extra_char, extra_char_سرڪايو);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    نياپو_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    اندريون_echowindow = save_اندريون_echowindow;
}

    فراغ
compute_cmdrow(فراغ)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    إذا (exmode_سرگرم || (نياپو_scrolled != 0 && !updating_screen))
	cmdline_صف = صفون - 1;
    وإلا
	cmdline_صف = W_WINROW(lastwin) + lastwin->w_اوچائي
						    + lastwin->w_حالت_اوچائي;
}

    فراغ
cursorcmd(فراغ)
{
    إذا (cmd_silent)
	أرجع;

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (cmdmsg_rl)
    {
	نياپو_صف = cmdline_صف  + (ccline.cmdspos / (cmdline_ويڪر - 1));
	نياپو_کانو = cmdline_ويڪر - (ccline.cmdspos % (cmdline_ويڪر - 1)) - 1;
	إذا (نياپو_صف <= 0)
	    نياپو_صف = صفون - 1;
    }
    وإلا
#جيڪڏهن_پورو
    {
	نياپو_صف = cmdline_صف + (ccline.cmdspos / cmdline_ويڪر);
	نياپو_کانو = ccline.cmdspos % cmdline_ويڪر;
	إذا (نياپو_صف >= صفون)
	    نياپو_صف = صفون - 1;
    }

    windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    إذا (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MCH_CURSOR_SHAPE
    mch_update_cursor();
#جيڪڏهن_پورو
}

    فراغ
gotocmdline(صحيح clr)
{
    نياپو_شروعات();
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    إذا (cmdmsg_rl)
	نياپو_کانو = cmdline_ويڪر - 1;
    وإلا
#جيڪڏهن_پورو
	نياپو_کانو = 0;	    // always start in column 0
    إذا (clr)		    // clear the bottom line(s)
	نياپو_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_صف, cmdline_کانو_off);
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

    إذا (p_paste || no_abbr)	    // no abbreviations or in paste mode
	أرجع FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    طالما (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    إذا (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    وإلا
	// check abbreviation from the beginning of the commandline
	spos = 0;

    أرجع check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
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
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    char_u	buf[20];
    صحيح		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    لكل (p = what == VSE_بفر ? بفر_ESC_CHARS : رستو_ESC_CHARS;
								*p != NUL; ++p)
	إذا ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#وإلا
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_بفر ? بفر_ESC_CHARS : رستو_ESC_CHARS);
    إذا (what == VSE_SHELL && csh_like_shell() && p != عدم)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#جيڪڏهن_پورو

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    إذا (p != عدم && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    أرجع p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    فراغ
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    إذا (p == عدم)
	أرجع;

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
    صحيح	    انگ_files,
    char_u  **files)
{
    صحيح	    i;
    char_u  *p;

    إذا (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	لكل (i = 0; i < انگ_files; ++i)
	{
	    p = home_replace_save(عدم, files[i]);
	    إذا (p != عدم)
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
    أرجع &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ساكن cmdline_info_T *
get_ccline_ptr(فراغ)
{
    إذا ((State & گهڻائي_CMDLINE) == 0)
	أرجع عدم;
    إذا (ccline.cmdbuff != عدم)
	أرجع &ccline;
    إذا (prev_ccline_استعمال_ٿيل && prev_ccline.cmdbuff != عدم)
	أرجع &prev_ccline;
    أرجع عدم;
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

    إذا (p == عدم)
	أرجع NUL;
    إذا (p->cmdfirstc == NUL)
	أرجع
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    (p->داخلا_fn) ? '@' :
#جيڪڏهن_پورو
	    '-';
    أرجع p->cmdfirstc;
}

#إذا defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ساكن char_u *
get_cmdline_str(فراغ)
{
    cmdline_info_T *p;

    إذا (cmdline_تارو > 0)
	أرجع عدم;
    p = get_ccline_ptr();
    إذا (p == عدم)
	أرجع عدم;
    أرجع vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ساكن char_u *
get_cmdline_completion_نمونو(فراغ)
{
    cmdline_info_T *p;
    صحيح		xp_context;

    إذا (cmdline_تارو > 0)
	أرجع عدم;

    p = get_ccline_ptr();
    إذا (p == عدم || p->xpc == عدم)
	أرجع عدم;

    xp_context = p->xpc->xp_context;
    إذا (xp_context == EXPAND_NOTHING)
    {
	سيٽ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    إذا (xp_context == EXPAND_UNSUCCESSFUL)
	أرجع عدم;

    char_u *compl_pat = p->xpc->xp_نمونو;

    إذا (compl_pat == عدم)
	أرجع عدم;

    أرجع vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ساكن char_u *
get_cmdline_completion(فراغ)
{
    cmdline_info_T	*p;
    صحيح			xp_context;

    إذا (cmdline_تارو > 0)
	أرجع عدم;

    p = get_ccline_ptr();
    إذا (p == عدم || p->xpc == عدم)
	أرجع عدم;

    xp_context = p->xpc->xp_context;
    إذا (xp_context == EXPAND_NOTHING)
    {
	سيٽ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    إذا (xp_context == EXPAND_UNSUCCESSFUL)
	أرجع عدم;

    أرجع cmdcomplete_type_منزل_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    فراغ
f_getcmdcomplpat(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_نمونو();
}

/*
 * "getcmdcompltype()" function
 */
    فراغ
f_getcmdcompltype(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    فراغ
f_getcmdline(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    فراغ
f_getcmdpos(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_عدد = p != عدم ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    فراغ
f_getcmdprompt(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
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
f_getcmdscreenpos(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_عدد = p != عدم ? cmdline_کانو_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    فراغ
f_getcmdtype(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    إذا (rettv->vval.v_string == عدم)
	أرجع;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ساكن صحيح
سيٽ_cmdline_str(char_u *str, صحيح pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    صحيح		    len;

    إذا (p == عدم)
	أرجع 1;

    len = (صحيح)STRLEN(str);
    إذا (realloc_cmdbuff(len + 1) != OK)
	أرجع 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    أرجع 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ساكن صحيح
سيٽ_cmdline_pos(
    صحيح		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    إذا (p == عدم)
	أرجع 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    إذا (pos < 0)
	new_cmdpos = 0;
    وإلا
	new_cmdpos = pos;

    أرجع 0;
}

// "setcmdline()" function
    فراغ
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    صحيح pos = -1;

    إذا (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_عدد_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحيح error = FALSE;

	pos = (صحيح)tv_get_عدد_chk(&argvars[1], &error) - 1;
	إذا (error)
	    أرجع;
	إذا (pos < 0)
	{
	    emsg(_(e_دليل_must_be_مثبت));
	    أرجع;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_عدد = سيٽ_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    فراغ
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    صحيح		pos;

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    pos = (صحيح)tv_get_عدد(&argvars[0]) - 1;
    إذا (pos >= 0)
	rettv->vval.v_عدد = سيٽ_cmdline_pos(pos);
}
#جيڪڏهن_پورو

/*
 * Return the first character of the current command line.
 */
    صحيح
get_cmdline_firstc(فراغ)
{
    أرجع ccline.cmdfirstc;
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
    صحيح		پهريون = FALSE;
    varnumber_T	انگ;

    *str = skipwhite(*str);
    إذا (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, عدم, &len, 0, &انگ, عدم, 0, FALSE, عدم);
	*str += len;
	//حد_کان_مٿي
	إذا (انگ > وڌ_کان_وڌ_عدد)
	    أرجع FAIL;

	*num1 = (صحيح)انگ;
	پهريون = TRUE;
    }
    *str = skipwhite(*str);
    إذا (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, عدم, &len, 0, &انگ, عدم, 0, FALSE, عدم);
	إذا (len > 0)
	{
	    *str = skipwhite(*str + len);
	    //حد_کان_مٿي
	    إذا (انگ > وڌ_کان_وڌ_عدد)
		أرجع FAIL;

	    *num2 = (صحيح)انگ;
	}
	وإلا إذا (!پهريون)		// no number given at all
	    أرجع FAIL;
    }
    وإلا إذا (پهريون)			// only one number given
	*num2 = *num1;
    أرجع OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    حرف *
did_سيٽ_cedit(optset_T *دليلون اڻ_استعمال_ٿيل)
{
    صحيح n;

    إذا (*p_cedit == NUL)
	cedit_ڪنجي = -1;
    وإلا
    {
	n = string_منزل_ڪنجي(p_cedit, FALSE);
	إذا (n == 0 || vim_isprintc(n))
	    أرجع e_invalid_دليل;
	cedit_ڪنجي = n;
    }
    أرجع عدم;
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
    صحيح			save_exmode = exmode_سرگرم;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    صحيح			save_cmdmsg_rl = cmdmsg_rl;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    صحيح			save_ڪنجي_Typed;
#جيڪڏهن_پورو
    صحيح			newbuf_حالت;
    صحيح			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    إذا (متن_or_buf_locked()
	    || cmdwin_type != 0
#إذا defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_تارو > 0
#جيڪڏهن_پورو
	    )
    {
	beep_flush();
	أرجع K_IGNORE;
    }
    سيٽ_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_سائيز_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    إذا (win_ورهايو((صحيح)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	أرجع K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    إذا (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_بفر != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	أرجع Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_حالت = do_ecmd(0, عدم, عدم, عدم, ECMD_ONE, ECMD_HIDE,
					عدم);
    cmdwin_valid = win_valid(cmdwin_win);
    إذا (newbuf_حالت == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_بفر != old_curbuf.br_buf)
    {
	إذا (newbuf_حالت == OK)
	    سيٽ_bufref(&bufref, curbuf);
	إذا (cmdwin_valid && !آخري_دري())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	إذا (newbuf_حالت == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_بفر(عدم, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = عدم;
	beep_flush();
	ga_clear(&winsizes);
	أرجع Ctrl_C;
    }
    cmdwin_buf = curbuf;

    سيٽ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#جيڪڏهن_پورو
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_تالو;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    إذا (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	إذا (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    جوڙيو_map((char_u *)"<buffer> <Tab> <C-X><C-V>", گهڻائي_وجهو, TRUE);
	    جوڙيو_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", گهڻائي_عام, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    جوڙيو_map((char_u *)"<buffer> <S-Tab> <C-P>", گهڻائي_وجهو, TRUE);
	}
	سيٽ_option_قدر_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_تالو;

    // Reset 'textwidth' after setting 'filetype' (the Vim filetype plugin
    // sets 'textwidth' to 78).
    curbuf->b_p_tw = 0;

    // Fill the buffer with the history.
    init_history();
    إذا (get_hislen() > 0)
    {
	i = *get_hisidx(histtype);
	إذا (i >= 0)
	{
	    lnum = 0;
	    افعل
	    {
		إذا (++i == get_hislen())
		    i = 0;
		إذا (get_histentry(histtype)[i].hisstr != عدم)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    طالما (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_سٽ_ڳڻپ, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
    curwin->w_cursor.کانو = ccline.cmdpos;
    بدليو_سٽ_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_سرگرم = 0;

    State = گهڻائي_عام;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_نتيجو = 0;

    // Trigger CmdwinEnter autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINENTER);
    إذا (restart_edit != 0)	// autocmd with ":startinsert"
	stuffcharReadbuff(K_NOP);

    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    /*
     * Call the main loop until <CR> or CTRL-C is typed.
     */
    main_loop(TRUE, FALSE);

    RedrawingDisabled = save_RedrawingDisabled;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    save_ڪنجي_Typed = ڪنجي_Typed;
#جيڪڏهن_پورو

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    ڪنجي_Typed = save_ڪنجي_Typed;
#جيڪڏهن_پورو

    cmdwin_type = 0;
    cmdwin_buf = عدم;
    cmdwin_win = عدم;
    exmode_سرگرم = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    إذا (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_بفر != old_curbuf.br_buf)
    {
	cmdwin_نتيجو = Ctrl_C;
	emsg(_(e_سرگرم_دري_or_بفر_بدليو_or_deleted));
    }
    وإلا
    {
#إذا defined(FEAT_EVAL)
	// autocmds may abort script processing
	إذا (aborting() && cmdwin_نتيجو != K_IGNORE)
	    cmdwin_نتيجو = Ctrl_C;
#جيڪڏهن_پورو
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	إذا (cmdwin_نتيجو == K_XF1 || cmdwin_نتيجو == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    إذا (cmdwin_نتيجو == K_XF1)
	    {
		p = (char_u *)"qa!";
		plen = 3;
	    }

	    إذا (histtype == HIST_CMD)
	    {
		// Execute the command directly.
		ccline.cmdbuff = vim_strnsave(p, plen);
		ccline.cmdlen = (صحيح)plen;
		ccline.cmdbufflen = (صحيح)(plen + 1);
		cmdwin_نتيجو = CAR;
	    }
	    وإلا
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	وإلا إذا (cmdwin_نتيجو == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = عدم;
	}
	وإلا
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	إذا (ccline.cmdbuff == عدم)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_نتيجو = Ctrl_C;
	}
	وإلا
	{
	    ccline.cmdpos = curwin->w_cursor.کانو;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    إذا (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#جيڪڏهن_پورو
	// First go back to the original window.
	wp = curwin;
	سيٽ_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	إذا (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	إذا (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_بفر(عدم, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_سائيز_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	إذا (cmdwin_نتيجو == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    سيٽ_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#جيڪڏهن_پورو

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    أرجع cmdwin_نتيجو;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    صحيح
is_اندريون_cmdwin(فراغ)
{
    أرجع cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap اڻ_استعمال_ٿيل, char_u *cmd اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    إذا (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == عدم)
	أرجع عدم;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    إذا (l == عدم)
	أرجع عدم;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_شيون(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    أرجع (char_u *)ga.ga_معلومات;
#وإلا
    أرجع عدم;
#جيڪڏهن_پورو
}

#إذا defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    فراغ
get_user_داخلا(
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
    إذا (داخلا_busy)
	أرجع;  // this doesn't work recursively.

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	أرجع;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_داخلا
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    إذا (no_console_داخلا() && !is_not_a_رڪن())
	أرجع;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    إذا (prompt != عدم)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	إذا (p == عدم)
	    p = prompt;
	وإلا
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    نياپو_شروعات();
	    نياپو_clr_eos();
	    نياپو_puts_attr((حرف *)prompt, get_echo_attr());
	    نياپو_didout = FALSE;
	    نياپو_starthere();
	    *p = c;
	}
	cmdline_صف = نياپو_صف;

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    إذا (defstr != عدم)
		stuffReadbuffSpec(defstr);

	    إذا (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		صحيح	xp_namelen;
		ڊگهو	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = عدم;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		إذا (xp_name == عدم)
		    أرجع;

		xp_namelen = (صحيح)STRLEN(xp_name);

		إذا (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    أرجع;
	    }
	}

	إذا (defstr != عدم)
	{
	    صحيح save_ex_عام_busy = ex_عام_busy;
	    صحيح save_vgetc_busy = vgetc_busy;
	    صحيح save_داخلا_busy = داخلا_busy;

	    داخلا_busy |= vgetc_busy;
	    ex_عام_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_عام_busy = save_ex_عام_busy;
	    vgetc_busy = save_vgetc_busy;
	    داخلا_busy = save_داخلا_busy;
	}
	إذا (inputdialog && rettv->vval.v_string == عدم
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	نياپو_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#جيڪڏهن_پورو

/*
 * "wildtrigger()" function
 */
    فراغ
f_wildtrigger(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
    إذا (!(State & گهڻائي_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_سرگرم())
	أرجع;

    صحيح cmd_type = get_cmdline_type();

    إذا (cmd_type == ':' || cmd_type == '/' || cmd_type == '?')
    {
	// Add K_WILD as a single special key
	char_u	ڪنجي_string[4];

	ڪنجي_string[0] = K_SPECIAL;
	ڪنجي_string[1] = KS_EXTRA;
	ڪنجي_string[2] = KE_WILD;
	ڪنجي_string[3] = NUL;

	// Insert it into the typeahead buffer
	ins_typebuf(ڪنجي_string, REMAP_NONE, 0, TRUE, FALSE);
    }
}
